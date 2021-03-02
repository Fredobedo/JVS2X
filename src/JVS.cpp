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

#include "USB_HID/USB_PS3/usb_ps3.h"
#include "print.h"

bool analog;
bool mirroring;
bool full_joy;
unsigned long time;

JVS::JVS(HardwareSerial& serial) :
    _Uart(serial) // Need to initialize references before body
{
    coins1 = 0;
    coins2 = 0;
    coin_pressed_at = 0;
    initialized = false;
    shift_mode = false;
    pressed_smth = false;
    old_key = 0;
    analog = false;
    full_joy = false;
    mirroring = false;
    pinMode(6, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    //analog = !digitalRead(6);
    //mirroring = !digitalRead(5);
    //full_joy = !digitalRead(4);
    full_joy = 1;
}


void JVS::reset() {
    TRACE("RESET\n");
    char str[] = { (char)CMD_RESET, (char)CMD_RESET_ARG };
    this->write_packet(BROADCAST, str, 2);  // -> Broadcast Reset communication status to all slaves
    delay(500);
    TRACE("RESET\n");
    this->write_packet(BROADCAST, str, 2);  // -> Broadcast Reset communication status to all slaves
    delay(500);
/*
    if (analog)
        TRACE("Analogique active");
    if (mirroring)
        TRACE("Mirroring active");
    if (full_joy)
        TRACE("Full Joystick active");
 */
}


//It is not used ?
void JVS::assign(int attempt) {
    char str[] = { (char)CMD_ASSIGN_ADDR, (char)attempt };
    TRACE("SETADDR\n");
    this->cmd(BROADCAST, str, 2);
}

void JVS::init(int board) {
    TRACE("SETADDR\n"); 
    char str[] = { (char)CMD_ASSIGN_ADDR, (char)board };    // -> Set slave address (0xF1)
    this->cmd(BROADCAST, str, 2);                           //    Request size: 2  | Response size: 1
    delay(2000);
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
    

    initialized = true;
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

void JVS::switches(int board) {
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
    
    
    char incomingByte;
    int counter = 0;

    //gamepad_P1_state.select_btn = 0;
    //gamepad_P2_state.select_btn = 0;

    bool abordRequest = false;

    //Last Byte (SUM) is ignored
    //Measured elapse time: 13 millisec
    while (counter < length - 1 && !abordRequest) {
        while (!_Uart.available()) { } delayMicroseconds(100);

        //Response Packet Status
        //delayMicroseconds(500);
        incomingByte = _Uart.read();
        PHEX(incomingByte);
        TRACE(" ");

        //Check if the marker('Escape Byte') has been used -> 0xE0 or 0xD0 is in the payload.
        //If so, restore original value.
        if (incomingByte == 0xD0) {    
            incomingByte = _Uart.read();  
            incomingByte++;                
        }
        switch (counter) {
            /* Report Code for first command SWINP */
            case 0:
                abordRequest=!checkReportCode(incomingByte);
                break;

            /* Tilt & Test buttons */    
            case 1:
                gamepad_P1_state.select_btn = (BTN_GENERAL_TEST==(incomingByte & BTN_GENERAL_TEST));
                break;

            /* First byte switch player 1 */ 
            case 2:
                //START + Button 1 -> PS Home
                if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
                {
                    gamepad_P1_state.ps_btn=1;
                    gamepad_P1_state.start_btn=0;
                    break;
                }

                //START + Button 2 -> Select
                if((BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
                {
                    gamepad_P1_state.select_btn=1;
                    gamepad_P1_state.start_btn=0;
                    break;
                }

                //Start
                if((BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
                {
                    gamepad_P1_state.ps_btn=0;
                    gamepad_P1_state.select_btn=0;
                    gamepad_P1_state.start_btn=1;
                    break;
                }

                gamepad_P1_state.ps_btn=0;
                gamepad_P1_state.select_btn=0;
                gamepad_P1_state.start_btn=0;

                //Other button combinations
                gamepad_P1_state.cross_btn  = (BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1));
                gamepad_P1_state.circle_btn = (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2));
                
                gamepad_P1_state.direction = 8; // Center

                if ((BTN_PLAYER_DOWN==(incomingByte & BTN_PLAYER_DOWN))) {
                    gamepad_P1_state.direction = 4;
                    if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) 
                        gamepad_P1_state.direction = 3;
                    else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))) 
                        gamepad_P1_state.direction = 5;
                }
                else {
                    if ((BTN_PLAYER_UP==(incomingByte & BTN_PLAYER_UP))) {
                        gamepad_P1_state.direction = 0;
                        if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) 
                            gamepad_P1_state.direction = 1;
                        else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT)))
                            gamepad_P1_state.direction = 7;
                    }
                    else {
                        if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT)))
                            gamepad_P1_state.direction = 2;
                        else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))) 
                            gamepad_P1_state.direction = 6;
                    }
                }
                break;

            /* Second byte Player 1 */
            case 3:
                gamepad_P1_state.square_btn   = (BTN_PLAYER_PUSH3==(incomingByte & BTN_PLAYER_PUSH3));
                gamepad_P1_state.triangle_btn = (BTN_PLAYER_PUSH4==(incomingByte & BTN_PLAYER_PUSH4));
                gamepad_P1_state.l1_btn       = (BTN_PLAYER_PUSH5==(incomingByte & BTN_PLAYER_PUSH5));
                gamepad_P1_state.r1_btn       = (BTN_PLAYER_PUSH6==(incomingByte & BTN_PLAYER_PUSH6));
                gamepad_P1_state.l2_btn       = (BTN_PLAYER_PUSH7==(incomingByte & BTN_PLAYER_PUSH7));
                gamepad_P1_state.r2_btn       = (BTN_PLAYER_PUSH8==(incomingByte & BTN_PLAYER_PUSH8));
                break;

            /* first byte player 2 */
            case 4:
                if (shift_mode) {
                    if (bitRead(incomingByte, 7)) {
                        pressed_smth = true;
                    }
                }
                else {
                    gamepad_P2_state.select_btn = bitRead(incomingByte, 7);
                }

                gamepad_P2_state.square_btn = bitRead(incomingByte, 0);
                gamepad_P2_state.cross_btn = bitRead(incomingByte, 1);


                if (bitRead(incomingByte, 4)) {
                    gamepad_P2_state.direction = 0;
                    //X_LEFT
                    if (bitRead(incomingByte, 2)) {
                        gamepad_P2_state.direction = 7;
                    }
                    //X_RIGHT
                    else if (bitRead(incomingByte, 3)) {
                        gamepad_P2_state.direction = 1;
                    }
                }
                else {
                    //Y_DOWN
                    if (bitRead(incomingByte, 5)) {
                        gamepad_P2_state.direction = 4;
                        //X_LEFT
                        if (bitRead(incomingByte, 2)) {
                            gamepad_P2_state.direction = 5;
                        }
                        //X_RIGHT
                        else if (bitRead(incomingByte, 3)) {
                            gamepad_P2_state.direction = 3;
                        }
                    }

                    else {
                        //Y_CENTER
                        //X_LEFT
                        if (bitRead(incomingByte, 2)) {
                            gamepad_P2_state.direction = 6;
                        }
                        //X_RIGHT
                        else if (bitRead(incomingByte, 3)) {
                            gamepad_P2_state.direction = 2;
                        }
                    }
                }
                break;

            /* Second byte player 2 */
            case 5:
                gamepad_P2_state.r2_btn = bitRead(incomingByte, 2);
                gamepad_P2_state.l2_btn = bitRead(incomingByte, 3);
                gamepad_P2_state.r1_btn = bitRead(incomingByte, 4);
                gamepad_P2_state.l1_btn = bitRead(incomingByte, 5);
                gamepad_P2_state.triangle_btn = bitRead(incomingByte, 6);
                gamepad_P2_state.circle_btn = bitRead(incomingByte, 7);
                break;
            
            /* Report Code for second command COININP */
            case 6:
                abordRequest=!checkReportCode(incomingByte);
                break;

            /* Slot 1 status on 2 first bits (on the left) */    
            case 7:
                break;

            /* Slot 1 coin */
            case 8:
                //phex16(incomingByte);
                //phex16(initialSlot1CoinValue);
                if (incomingByte > initialSlot1CoinValue)
                {
                    initialSlot1CoinValue = incomingByte;
                    if(initialSlot1CoinValue>0)
                        gamepad_P1_state.start_btn=1;
                }
                break;

            /* Slot 2 status on 2 first bits (on the left) */    
            case 9:
                break;

            /* Slot 2 coin */
            case 10:
                if (incomingByte > initialSlot2CoinValue)
                    initialSlot2CoinValue = incomingByte;
                else
                    gamepad_P2_state.start_btn=1;
                break;

            /* Byte11 is report for third command (-> here CMD_READ_ANALOG player 1 & 2) */
            case 11:
                abordRequest=!checkReportCode(incomingByte);
                break;
            
            /* Analog Channel 1 (X) */
            case 12:
                //gamepad_P1_state.l_x_axis = incomingByte;
                break;

            /* Analog Channel 2 (Y) */
            case 14:
                //gamepad_P1_state.l_y_axis = incomingByte;
                break;

            /* Analog Channel 3 (Z) */
            case 16:
                //gamepad_P2_state.l_x_axis = incomingByte;
                break;

            /* Analog Channel 4 (Za) */
            case 18:
                //gamepad_P2_state.l_y_axis = incomingByte;
                break;
        }
        counter++;
    }

    
    //Measured elapse time: 1 millisec
    usb_gamepad_P1_send();
    usb_gamepad_P2_send();

    TRACE("\n");
     

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
int* JVS::cmd(char destination, char data[], int size) {
    this->write_packet(destination, data, size);

    char incomingByte;

    int length = WaitForPayload();

    int* res = (int*)malloc(length * sizeof(int));
    
    for (int counter=0; counter < length-1; counter++) {
        while (!_Uart.available()) { } delayMicroseconds(100);
        incomingByte = _Uart.read();
        
        //Check if the marker('Escape Byte') has been used -> 0xE0 or 0xD0 is in the payload.
        //If so, restore original value.
        /*
        if (incomingByte == 0xD0) {    
            incomingByte = _Uart.read();  
            incomingByte++;                
        }
*/
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
    while (!_Uart.available()) { } delayMicroseconds(100);
    TRACE(" -> ok\n");

    // E0 SYNC
    // 00 Address, 00 is master
    // XX Length
    TRACE("waiting for UART sync (0xE0)");
    while (_Uart.read() != 0xE0) { } // wait for sync
    TRACE(" -> ok\n");

    TRACE("Testing if for me (0x00)");
    while (_Uart.read() != 0) { } // only if it's for me
    TRACE(" -> ok\n");

    //TRACE("waiting for UART avaiability");
    while (!_Uart.available()) { } delayMicroseconds(100);
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
            sum = (sum + data[i]) % 256;
        }
        _Uart.write(sum);
        _Uart.flush();
        //Very strange, isn't it 500 usec
        delayMicroseconds(100);
    }
    
}


