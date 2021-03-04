/*
   JVS.cpp
        p1 start works as shift, when pressed with the following buttons it causes different keypresses:
        button 1: coin (presses keyb 5)
        J1 right: Tab
        J1 down: P (pause)
        J1 left: enter
        P2: esc
*/

#include <Arduino.h>
#include "jvs.h"
#include "print.h"

unsigned long time;

JVS::JVS(HardwareSerial& serial) :
    _Uart(serial) // Need to initialize references before body
{
    pinMode(6, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
}


void JVS::reset() {
    TRACE("RESET\n");
    char str[] = { (char)CMD_RESET, (char)CMD_RESET_ARG };
    this->write_packet(BROADCAST, str, 2);  // -> Broadcast Reset communication status to all slaves
    delay(500);
    TRACE("RESET\n");
    this->write_packet(BROADCAST, str, 2);  // -> Broadcast Reset communication status to all slaves
    delay(500);
}


//It is not used ?
void JVS::setAddress(int board) {
    TRACE("SETADDR\n");
    char str[] = { (char)CMD_ASSIGN_ADDR, (char)board };  // -> Set slave address (0xF1)
    this->cmd(BROADCAST, str, 2);                           //    Request size: 2  | Response size: 1
    delay(2000);
}

void JVS::getBoardInfo(int board) {
    TRACE("IOIDENT\n");   
    char str1[] = { (char)CMD_REQUEST_ID};                  // -> Master requests to initiate a communication, Get slave ID Data (0x10)
    this->cmd(board, str1, 1);                              //    Request size: 1  | Response size: max 102
    TRACE("CMDREV\n"); 
    char str2[] = { (char)CMD_COMMAND_VERSION };            // -> Command format revision (0x11)
    this->cmd(board, str2, 1);                              //    Request size: 1  | Response size: 2
    TRACE("JVSREV\n"); 
    char str3[] = { (char)CMD_JVS_VERSION };                // -> JVS revision (0x12)
    this->cmd(board, str3, 1);                              //    Request size: 1  | Response size: 2
    TRACE("COMMVER\n"); 
    char str4[] = { (char)CMD_COMMS_VERSION };              // -> Communication version
    this->cmd(board, str4, 1);                              //    Request size: 1  | Response size: 2
    TRACE("FEATCHK\n"); 
    char str5[] = { (char)CMD_CAPABILITIES };               // -> Check Slave features
    this->cmd(board, str5, 1);                              //    Request size: 1  | Response size: 6+
}

void JVS::resetAllAnalogFuzz()
{
    for(int row=0; row < 4; row++){
        for(int col=0; col<8; col++)
        {
            analogEstimatedFuzz[row][col]=0xFF;
            col++;
            analogEstimatedFuzz[row][col]=0x00;
        }
    }
}

void JVS::dumpAllAnalogFuzzArray()
{
    for(int row=0; row < 4; row++){
        TRACE("Board"); PHEX(row); TRACE(": ");
        for(int col=0; col<8; col++)
        {
            PHEX(analogEstimatedFuzz[row][col]);
            TRACE(" ");
        }
        TRACE("\n");
    }
}
//Mainly used to detect analog fuzz when IO Board supports analog controls but no one is connected
void JVS::setAnalogFuzz(int board)
{
    TRACE("Starting Fuzz calculation\n");
    char str[] = { (char)CMD_READ_ANALOG, 4};  
    //Poll 30 time and define min max fuzz
    for(int cp=0; cp<30;cp++)
    {

        this->write_packet(board, str, sizeof str);

        char incomingByte;
        int length = WaitForPayload();
        int tolerance=1;

        for (int counter=0; counter < length-1; counter++) {
            WAIT_UART_AVAILABLE();
            incomingByte = _Uart.read();

            switch(counter){
                case 0:
                    if((incomingByte)       < analogEstimatedFuzz[board][0]) analogEstimatedFuzz[board][0]=incomingByte-tolerance;
                    else if((incomingByte)  > analogEstimatedFuzz[board][1]) analogEstimatedFuzz[board][1]=incomingByte+tolerance;
                    break;
                case 2:
                    if((incomingByte)       < analogEstimatedFuzz[board][2]) analogEstimatedFuzz[board][2]=incomingByte-tolerance;
                    else if((incomingByte)  > analogEstimatedFuzz[board][3]) analogEstimatedFuzz[board][3]=incomingByte+tolerance;
                    break;
                case 4:
                    if((incomingByte)       < analogEstimatedFuzz[board][4]) analogEstimatedFuzz[board][4]=incomingByte-tolerance;
                    else if((incomingByte)  > analogEstimatedFuzz[board][5]) analogEstimatedFuzz[board][5]=incomingByte+tolerance;
                    break;
                case 6:
                    if((incomingByte)       < analogEstimatedFuzz[board][6]) analogEstimatedFuzz[board][6]=incomingByte-tolerance;
                    else if((incomingByte)  > analogEstimatedFuzz[board][7]) analogEstimatedFuzz[board][7]=incomingByte+tolerance;
                    break;                                                            
            }
        }
    }
}

//JVS Response layout (based on request with multiple commands):
//  - SYNC
//  - Node No:  Destination node Nbr, 0 is for the host
//  - Lengh:    nbr of following data bytes including the SUM byte
//  - RequestStatus: 
//    - ReportStatus_1:
//    - Report_1_byte_1:
//    - Report_1_byte_2:
//    - Etc.
//    - ReportStatus_2:
//    - Report_2_byte_1:
//    - Report_2_byte_2:
//    - Etc.
//    - ReportStatus_3:
//    - Report_3_byte_1:
//    - Report_3_byte_2:
//    - Etc.
//  - SUM:      CheckSum on all bytes in packet -SYNC -SUM modulo 256
//
// CMD_READ_DIGITAL  | Switch inputs (read controller switches)   
// CMD_READ_COINS    | Coin Inputs (read coin counters)           
// CMD_READ_ANALOG   | Analog Inputs (read analog inputs)         
// CMD_READ_ROTARY   | Rotary Inputs (read rotary inputs)        
// CMD_READ_KEYPAD   | Keycode Inputs (read keycode inputs)      
// CMD_READ_LIGHTGUN | Gun Inputs (read lightgun inputs)          
// CMD_READ_GPI      | Misc Switch Inputs (read misc switch inputs) 
void JVS::GetAllInputs(int board, gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2) {
    char str[] = {  CMD_READ_DIGITAL, 0x02, 0x02,       // Command 1: Read input switch for the 2 players in 2 bytes format
                    CMD_READ_COINS, 0x02,               // Command 2: Read coin values for 2 slots
                    CMD_READ_ANALOG, 0x04 };            // Command 3: Read analog values for 4 channels
    //TOTAL Measured elapse time: 16 millisec

    // --- SEND REQUEST ---
    //Send 3 command at once: SYNC + Node(board) + ByteNbr(sizeof str) + Payload (str) + SUM
    this->write_packet(board, str, sizeof str);
    
    // --- READ THE RESPONSE CONTAINING THE 3 REPORTS FOR THE 3 COMMANDS SENT ---
    // SYNC + Node + ByteNbr + RequestStatus + 
    //       ReportStatus_1 + Report_1 + 
    //       ReportStatus_2 + Report_2 +
    //       ReportStatus_3 + Report_3 +
    // SUM
    //
    // Example: E0 00 16(22)
    //          requestStatus:01 reportCode:01 Tilt:00 switchP1:00 00 switchP2:00 00 reportCode:01 coins:00 00 80 00 reportCode:01 analog:14 00 14 00 14 00 14 00 
    //
    //Measured elapse time: 1 millisec 
    int length = WaitForPayload();

    if(length>0)
    {
        if(parseSwitchInput(gamepad_state_p1,gamepad_state_p2)){
            if(parseCoinInput(gamepad_state_p1,gamepad_state_p2)){
                if(parseAnalogInput(board, gamepad_state_p1,gamepad_state_p2)){
                    usb_gamepad_P1_send(gamepad_state_p1);
                    usb_gamepad_P2_send(gamepad_state_p2);
                }
            }
        }
    }
}

inline bool JVS::parseAnalogInput(int board, gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2)
{
    /* Report Code for first command SWINP */
    UART_READ_UNESCAPED();
    if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
        return false;

    /* Analog Channel 1 (X) */
    UART_READ_UNESCAPED();
    if(BETWEEN(incomingByte, analogEstimatedFuzz[board][0], analogEstimatedFuzz[board][1]))
        gamepad_state_p1.l_x_axis = incomingByte;
    UART_READ_UNESCAPED();

    /* Analog Channel 2 (Y) */
    UART_READ_UNESCAPED();
    if(BETWEEN(incomingByte, analogEstimatedFuzz[board][2], analogEstimatedFuzz[board][3]))
        gamepad_state_p1.l_y_axis = incomingByte;
    UART_READ_UNESCAPED();

    /* Analog Channel 3 (Z) */
    UART_READ_UNESCAPED();
    if(BETWEEN(incomingByte, analogEstimatedFuzz[board][4], analogEstimatedFuzz[board][5]))
        gamepad_state_p2.l_x_axis = incomingByte;
    UART_READ_UNESCAPED();

    /* Analog Channel 4 (Za) */
    UART_READ_UNESCAPED();
    if(BETWEEN(incomingByte, analogEstimatedFuzz[board][6], analogEstimatedFuzz[board][7]))            
        gamepad_state_p2.l_y_axis = incomingByte;
    UART_READ_UNESCAPED();

    return true;
}

inline bool JVS::parseCoinInput(gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2)
{
    /* Report Code for first command SWINP */
    UART_READ_UNESCAPED();
    if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
        return false;

    /* Slot 1 status on 2 first bits (on the left) */
    UART_READ_UNESCAPED();
    
    /* Slot 1 coin */
    UART_READ_UNESCAPED();
    if (incomingByte > initialSlot1CoinValue)
    {
        initialSlot1CoinValue = incomingByte;
        if(initialSlot1CoinValue>0)
            gamepad_state_p1.start_btn=1;
    }

    /* Slot 2 status on 2 first bits (on the left) */
    UART_READ_UNESCAPED();
    
    /* Slot 2 coin */
    UART_READ_UNESCAPED();
    if (incomingByte > initialSlot1CoinValue)
    {
        initialSlot1CoinValue = incomingByte;
        if(initialSlot1CoinValue>0)
            gamepad_state_p2.start_btn=1;
    }
    return true;
}

inline bool JVS::parseSwitchInput(gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2)
{
    /* Report Code for first command SWINP */
    UART_READ_UNESCAPED();
    if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
        return false;

    /* Tilt & Test buttons */ 
    UART_READ_UNESCAPED();
    gamepad_state_p1.select_btn = (BTN_GENERAL_TEST==(incomingByte & BTN_GENERAL_TEST));

    /* 2 next bytes for player 1 */
    parseSwitchInputPlayerX(gamepad_state_p1);

    /* 2 next bytes for player 2 */
    parseSwitchInputPlayerX(gamepad_state_p2);

    return true;
}

inline void JVS::parseSwitchInputPlayerX(gamepad_state_t &gamepad_state)
{   
    /* First byte switch player x */
    UART_READ_UNESCAPED();
    //START + Button 1 -> PS Home
    if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
    {
        gamepad_state.ps_btn=1;
        gamepad_state.start_btn=0;
    }
    //START + Button 2 -> Select
    else if((BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
    {
        gamepad_state.select_btn=1;
        gamepad_state.start_btn=0;
    }
    //Start
    else if((BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
    {
        gamepad_state.ps_btn=0;
        gamepad_state.select_btn=0;
        gamepad_state.start_btn=1;
    }
    else{
        gamepad_state.ps_btn=0;
        gamepad_state.select_btn=0;
        gamepad_state.start_btn=0;

        //Other button combinations
        gamepad_state.cross_btn   = (BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1));
        gamepad_state.cross_axis  = gamepad_state.cross_btn * 0xFF;
        gamepad_state.circle_btn  = (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2));
        gamepad_state.circle_axis = gamepad_state.circle_btn * 0xFF;
        
        gamepad_state.direction = 8; // Center
        gamepad_state.l_x_axis=0x80;
        gamepad_state.l_y_axis=0x80;

        if ((BTN_PLAYER_DOWN==(incomingByte & BTN_PLAYER_DOWN))) {
            gamepad_state.direction = 4;
            gamepad_state.l_y_axis=0xFF;
            if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) {
                gamepad_state.direction = 3;
                gamepad_state.l_x_axis=0xFF;}
            else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))) {
                gamepad_state.direction = 5;
                gamepad_state.l_x_axis=0x00;}
        }
        else {
            if ((BTN_PLAYER_UP==(incomingByte & BTN_PLAYER_UP))) {
                gamepad_state.direction = 0;
                gamepad_state.l_y_axis=0x00;
                if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) {
                    gamepad_state.direction = 1;
                    gamepad_state.l_x_axis=0xFF;}
                else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))){
                    gamepad_state.direction = 7;
                    gamepad_state.l_x_axis=0x00;}
            }
            else {
                if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) {
                    gamepad_state.direction = 2;
                    gamepad_state.l_x_axis=0xFF;}
                else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))) {
                    gamepad_state.direction = 6;
                    gamepad_state.l_x_axis=0x00;}
            }
        }
    }

    /* second byte switch player x */
    UART_READ_UNESCAPED();
    gamepad_state.square_btn    = (BTN_PLAYER_PUSH3==(incomingByte & BTN_PLAYER_PUSH3));
    gamepad_state.square_axis   = gamepad_state.square_btn * 0xFF;
    gamepad_state.triangle_btn  = (BTN_PLAYER_PUSH4==(incomingByte & BTN_PLAYER_PUSH4));
    gamepad_state.triangle_axis = gamepad_state.triangle_btn * 0xFF;
    gamepad_state.l1_btn        = (BTN_PLAYER_PUSH5==(incomingByte & BTN_PLAYER_PUSH5));
    gamepad_state.r1_btn        = (BTN_PLAYER_PUSH6==(incomingByte & BTN_PLAYER_PUSH6));
    gamepad_state.l2_btn        = (BTN_PLAYER_PUSH7==(incomingByte & BTN_PLAYER_PUSH7));
    gamepad_state.r2_btn        = (BTN_PLAYER_PUSH8==(incomingByte & BTN_PLAYER_PUSH8));
}

void JVS::tic() {
    beginTime = millis();
}


void JVS::toc(const char *s)
{
    elapsedTime = millis() - beginTime;
    print(s);
    phex16(elapsedTime);
    print(PSTR("\n"));
}

// Check the request status returned by the slave
bool JVS::checkRequestStatus(char requestStatus)
{
        if(requestStatus == 0x01)
            return true;
        else if(requestStatus == 0x02)
            TRACE("Warning, command unknown\n");
        else if(requestStatus == 0x03)
            TRACE("Warning, slave detected a SUM Error\n");
        else if(requestStatus == 0x04)
            print(PSTR("ERROR, slave is too busy, it can't process the command\n"));
        return false;
}

//Check the report 
bool JVS::checkReportCode(char reportCode)
{
    if(reportCode == 0x01)
        return true;
    else if(reportCode == 0x02)
        TRACE("Warning, command parameter error, no return data\n");
    else if(reportCode == 0x03)
        TRACE("Warning, command parameter error, parameter is ignored\n");
    else if(reportCode == 0x04)
        print(PSTR("ERROR, slave is too busy, it can't process the sub command\n"));
    return false;
}

//JVS Commands layout:
//  - SYNC
//  - Node No:  Destination node Nbr, 0 is for the host
//  - Lengh:    nbr of following data bytes including the SUM byte
//  - byte 1:   data
//  - byte 2:   data
//  - etc.
//  - SUM:      CheckSum on all bytes in packet -SYNC -SUM modulo 256
int* JVS::cmd(char destination, char data[], int requestSize) {
    this->write_packet(destination, data, requestSize);

    char incomingByte;

    int length = WaitForPayload();

    int* res = (int*)malloc(length * sizeof(int));
    
    for (int counter=0; counter < length-1; counter++) {
        WAIT_UART_AVAILABLE();
        incomingByte = _Uart.read();
        
        //Check if the marker('Escape Byte') has been used -> 0xE0 or 0xD0 is in the payload.
        //If so, restore original value.

        if (incomingByte == 0xD0) {    
            incomingByte = _Uart.read();  
            incomingByte++;                
        }

        res[counter] = incomingByte;
        // actually do something with incomingByte
        TRACE(" ");
        PHEX(res[counter]);
    }
    
    TRACE("\n");
    return res;
}

int JVS::WaitForPayload()
{
    int length=0;

    TRACE("waiting for UART avaiability");
    WAIT_UART_AVAILABLE();
    TRACE(" -> ok\n");

    // E0 SYNC
    // 00 Address, 00 is master
    // XX Length
    TRACE("waiting for UART sync (0xE0)");
    WAIT_UART_READ(0XE0); // wait for sync
    TRACE(" -> ok\n");

    TRACE("Testing if for me (0x00)");
    WAIT_UART_READ(0); // only if it's for me
    TRACE(" -> ok\n");

    TRACE("waiting for UART avaiability");
    WAIT_UART_AVAILABLE();
    TRACE(" -> ok\n");

    TRACE("Reading");
    length = _Uart.read();
    TRACE(" -> Received: E0 00 ");
    PHEX(length); // <-- This is not always the lengh but Error code too !!!
    TRACE("\n");

    delayMicroseconds(100);
    if(checkRequestStatus(_Uart.read()))
        return length;
    else
        return 0;
}

void JVS::write_packet(char destination, char data[], int size) {
    if(strlen(data)>0)
    {
        _Uart.write(SYNC);
        _Uart.write(destination);
        _Uart.write(size + 1);

        char sum = destination + size + 1;
        for (int i = 0; i < size; i++) {
            if (data[i] == SYNC || data[i] == ESCAPE) {
                _Uart.write(ESCAPE);
                _Uart.write(data[i] - 1);
            }
            else {
                _Uart.write(data[i]);
            }
            sum+= data[i];
        }
        _Uart.write(sum % 256);
        _Uart.flush();

        delayMicroseconds(100);
    }
  }


