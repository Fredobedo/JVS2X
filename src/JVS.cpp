#include <Arduino.h>
#include "jvs.h"
#include "print.h"

unsigned long time;

JVS::JVS(HardwareSerial& serial, gamepad_state_t* controller_state_p1, gamepad_state_t* controller_state_p2) :
    _Uart(serial)
{
    pinMode(6, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    assignUSBControllers(controller_state_p1, controller_state_p2);
}

void JVS::broadcastReset(HardwareSerial& Uart){
    Uart.write(SYNC);
    Uart.write(BROADCAST);
    Uart.write(3);
    Uart.write((char)CMD_RESET);
    Uart.write((char)CMD_RESET_ARG);
    Uart.write(4 % 256);
    Uart.flush();
    delayMicroseconds(100);
}

int JVS::broadcastNewAddress(HardwareSerial& Uart, int board) {
    Uart.write(SYNC);
    Uart.write(BROADCAST);
    Uart.write(3);
    Uart.write((char)CMD_ASSIGN_ADDR);
    Uart.write((char)(char)board);
    Uart.write(4 % 256);
    Uart.flush();
    delay(2000);

    return board;
}

void JVS::getBoardInfo() {
    TRACE("General:\n", 1);
    TRACE(" - IOIDENT\n", 1);   
    char str1[] = { (char)CMD_REQUEST_ID};                  // -> Master requests information about maker, IO board code, etc. (0x10)
    this->cmd(board, str1, 1);                              //    Request size: 1  | Response size: max 102
    TRACE("- CMDREV\n", 1); 
    char str2[] = { (char)CMD_COMMAND_VERSION };            // -> Command format revision (0x11)
    this->cmd(board, str2, 1);                              //    Request size: 1  | Response size: 2
    TRACE(" - JVSREV\n", 1); 
    char str3[] = { (char)CMD_JVS_VERSION };                // -> JVS revision (0x12)
    this->cmd(board, str3, 1);                              //    Request size: 1  | Response size: 2
    TRACE(" - COMMVER\n", 1); 
    char str4[] = { (char)CMD_COMMS_VERSION };              // -> Communication version
    this->cmd(board, str4, 1);                              //    Request size: 1  | Response size: 2
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

/* debugging function */
void JVS::dumpAllAnalogFuzz()
{
    TRACE("Board", 2); PHEX(board, 2); TRACE(": ", 2);
    for(int col=0; col<8; col++)
    {
        PHEX(analogEstimatedFuzz[board][col], 2);
        TRACE(" ", 2);
    }
    TRACE("\n", 2);

}

//Mainly used to detect analog fuzz when IO Board supports analog controls but no one is connected
void JVS::setAnalogFuzz()
{
    char str[] = { (char)CMD_READ_ANALOG, 4};  
    int tolerance=2;
    char incomingByte;

    //Poll 30 time and define min max fuzz
    for(int cp=0; cp<30;cp++)
    {
        this->writePacket(board, str, sizeof str);

        int length = waitForPayload();

        for (int counter=0; counter < length; counter++) {
            UART_READ_UNESCAPED();

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
void JVS::getAllInputs() {
    char str[] = {  CMD_READ_DIGITAL, 0x02, 0x02,       // Command 1: Read input switch for the 2 players in 2 bytes format
                    CMD_READ_COINS, 0x02,               // Command 2: Read coin values for 2 slots
                    CMD_READ_ANALOG, 0x04};              // Command 3: Read analog values for 4 channels
    //                CMD_READ_LIGHTGUN, 0x00,            // Command 4: Read Lightgun values for channel 0
    //                CMD_READ_LIGHTGUN, 0x01};           // Command 5: Read Lightgun values for channel 1
    //TOTAL Measured elapse time: 16 millisec

    // --- SEND REQUEST ---
    //Send 3 commands at once: SYNC + Node(board) + ByteNbr(sizeof str) + Payload (str) + SUM
    this->writePacket(board, str, sizeof str);
    
    // --- READ THE RESPONSE CONTAINING THE 3 REPORTS FOR THE 3 COMMANDS SENT ---
    // SYNC + Node + ByteNbr + RequestStatus + 
    //       ReportCode_1 + Report_1 + 
    //       ReportCode_2 + Report_2 +
    //       ReportCode_3 + Report_3 +
    // SUM
    //
    // Example: E0 00 16(22)
    //          requestStatus:01 reportCode:01 Tilt:00 switchP1:00 00 switchP2:00 00 reportCode:01 coins:00 00 80 00 reportCode:01 analog:14 00 14 00 14 00 14 00 
    //
    //Measured elapse time: 1 millisec 
    int length = waitForPayload();

    if(length>0)
    {
        parseSwitchInput();
        parseCoinInput();
        parseAnalogInput();
        TRACE("\n",2);
        //parseLightgunInputChannel(board, gamepad_state_p1);
        //parseLightgunInputChannel(board, gamepad_state_p2);

        //read SUM
        UART_READ_UNESCAPED();
    }
}

void JVS::getSupportedFeatures()
{
    char str[] = {CMD_CAPABILITIES};

    this->writePacket(board, str, sizeof str);
    
    int length = waitForPayload();

    if(length>0)
        parseSupportedFeatures();

}

void JVS::dumpSupportedFeatures()
{
    TRACE("Features:\n", 1);
    if (supportedFeatures & FEATURE_HAS_PLAYERS){
        TRACE_TEXT_VALUE(" - Players: Number of Players               -> ", supported_feature.switch_input.Players, 1);
        TRACE_TEXT_VALUE(" - Players: Number of buttons per player    -> ", supported_feature.switch_input.ButtonsPerPlayer, 1);}
    if (supportedFeatures & FEATURE_HAS_COINS)
        TRACE_TEXT_VALUE(" - Coins: Number of slots                   -> ", supported_feature.coin_input.Slots, 1);
    if (supportedFeatures & FEATURE_HAS_ANALOG_IN){
        TRACE_TEXT_VALUE(" - Analog input: Number of channels         -> ", supported_feature.analog_input.Channels, 1);
        TRACE_TEXT_VALUE(" - Analog input: Bits per channel           -> ", supported_feature.analog_input.Bits, 1);}
    if (supportedFeatures & FEATURE_HAS_ROTARY)
        TRACE_TEXT_VALUE(" - Rotary:Number of rotary encoders         -> ", supported_feature.rotary_input.Channels, 1);
    if (supportedFeatures & FEATURE_HAS_LIGHTGUN){
        TRACE_TEXT_VALUE(" - Lightgun: Number of channels             -> ", supported_feature.screen_position_input.Channels, 1);
        TRACE_TEXT_VALUE(" - Lightgun: Number of effective bits for X -> ", supported_feature.screen_position_input.Xbits, 1);
        TRACE_TEXT_VALUE(" - Lightgun: Number of effective bits for Y -> ", supported_feature.screen_position_input.Ybits, 1);}
    if (supportedFeatures & FEATURE_HAS_CARD)        
        TRACE_TEXT_VALUE(" - Card System: Number of card slots        -> ", supported_feature.card_system_output.Slots, 1);
    if (supportedFeatures & FEATURE_HAS_HOPPER)
        TRACE_TEXT_VALUE(" - Medal hopper: Number of medal hoppers    -> ", supported_feature.medal_hopper_output.Channels, 1);
    if (supportedFeatures & FEATURE_HAS_GENERAL_PURPOSE_OUT)
        TRACE_TEXT_VALUE(" - General purpose output: Number of slots  -> ", supported_feature.general_purpose_output.Slots, 1);
    if (supportedFeatures & FEATURE_HAS_ANALOG_OUT)    
        TRACE_TEXT_VALUE(" - Analog output: Number of channels        -> ", supported_feature.analog_output.Channels, 1);
    if (supportedFeatures & FEATURE_HAS_DISPLAY){    
        TRACE_TEXT_VALUE(" - Display output: width                    -> ", supported_feature.character_output.width, 1);
        TRACE_TEXT_VALUE(" - Display output: height                   -> ", supported_feature.character_output.height, 1);
        TRACE_TEXT_VALUE(" - Display output: type                     -> ", supported_feature.character_output.type, 1);}
    if (supportedFeatures & FEATURE_HAS_BACKUP)        
        TRACE_TEXT_VALUE(" - Backup: has data                         -> ", supported_feature.backup_output.has_backup, 1);
    
    delay(500);
}

/*
Response example from Sega JVS IO BOARD 
01 01 02 0D 00 02 02 00 00 03 08 00 00 12 06 00 00 00 
01 -------------------------------------------------- Command status
   01 ----------------------------------------------- Function code (Switch Input)
      02 --------------------------------------------  - Players
         0D -----------------------------------------  - Buttons per players
            00 --------------------------------------  - not used
               02 ----------------------------------- Function code (Coin input)
                  02 --------------------------------  - Slot numbers
                     00 -----------------------------  - not used
                        00 --------------------------  - not used
                           03 ----------------------- Function code (Analog input)
                              08 --------------------  - Nbr of channels
                                 00 -----------------  - bits per channel (0 for unknown)
                                    00 --------------  - not used
                                       12 ----------- Function code (general purpose output)
                                          06 --------  - slots
                                             00 -----  - not used
                                                00 --  - not used
                                                   00 End code
*/
inline bool JVS::parseSupportedFeatures()
{
    bool endCodeReached=false;

    /* Report Code for command SCRPOSINP */
    UART_READ_UNESCAPED();
    //TRACE_TEXT_VALUE("Request Status: ", incomingByte, 2);
    if(checkRequestStatus(incomingByte)!=REQUEST_STATUS_NORMAL)
        return false;
    
    //Each function (except for end code) is 4 bytes, consisting of 1 byte for function code + 3 bytes for parameters
    while(!endCodeReached){
        UART_READ_UNESCAPED();      // -> read function code
        //TRACE_TEXT_VALUE("Request Status: ", incomingByte, 2);

        switch (incomingByte) {
            case FEATURE_FUNCTION_CODE_PLAYERS:               //-> 0x01
                supportedFeatures |= FEATURE_HAS_PLAYERS;
                UART_READ_UNESCAPED(); 
                supported_feature.switch_input.Players=incomingByte;
                UART_READ_UNESCAPED(); 
                supported_feature.switch_input.ButtonsPerPlayer=incomingByte;
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_COINS:                 //-> 0x02
                supportedFeatures |= FEATURE_HAS_COINS; 
                UART_READ_UNESCAPED(); 
                supported_feature.coin_input.Slots=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_ANALOG_IN:             //-> 0x03
                supportedFeatures |= FEATURE_HAS_ANALOG_IN;
                UART_READ_UNESCAPED(); 
                supported_feature.analog_input.Channels=incomingByte;
                UART_READ_UNESCAPED();
                supported_feature.analog_input.Bits=incomingByte; 
                UART_READ_UNESCAPED();                 
            break;

            case FEATURE_FUNCTION_CODE_ROTARY:                //-> 0x04
                supportedFeatures |= FEATURE_HAS_ROTARY;
                UART_READ_UNESCAPED(); 
                supported_feature.rotary_input.Channels=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_KEYPAD:                //-> 0x05
                supportedFeatures |= FEATURE_HAS_KEYPAD;
                UART_READ_UNESCAPED(); 
                supported_feature.keycode_input.byte1=incomingByte;
                UART_READ_UNESCAPED(); 
                supported_feature.keycode_input.byte2=incomingByte;
                UART_READ_UNESCAPED(); 
                supported_feature.keycode_input.byte3=incomingByte;
            break;

            case FEATURE_FUNCTION_CODE_LIGHTGUN:              //-> 0x06
                supportedFeatures |= FEATURE_HAS_LIGHTGUN;
                UART_READ_UNESCAPED(); 
                supported_feature.screen_position_input.Xbits=incomingByte;
                UART_READ_UNESCAPED(); 
                supported_feature.screen_position_input.Ybits=incomingByte;
                UART_READ_UNESCAPED(); 
                supported_feature.screen_position_input.Channels=incomingByte;
            break;

            case FEATURE_FUNCTION_CODE_GENERAL_PURPOSE_IN:    // -> 0x07
                supportedFeatures |= FEATURE_HAS_GENERAL_PURPOSE_IN;
                UART_READ_UNESCAPED(); 
                supported_feature.misc_switch_input.byte1=incomingByte;
                UART_READ_UNESCAPED(); 
                supported_feature.misc_switch_input.byte2=incomingByte;
                UART_READ_UNESCAPED(); 
                supported_feature.misc_switch_input.byte3=incomingByte;
            break;

            case FEATURE_FUNCTION_CODE_CARD:                  //-> 0x10
                supportedFeatures |= FEATURE_HAS_CARD;
                UART_READ_UNESCAPED(); 
                supported_feature.card_system_output.Slots=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_HOPPER:                //-> 0x11
                supportedFeatures |= FEATURE_HAS_HOPPER;
                UART_READ_UNESCAPED(); 
                supported_feature.medal_hopper_output.Channels=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_GENERAL_PURPOSE_OUT:   // -> 0x12
                supportedFeatures |= FEATURE_HAS_GENERAL_PURPOSE_OUT;
                UART_READ_UNESCAPED(); 
                supported_feature.general_purpose_output.Slots=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_ANALOG_OUT:              // -> 0x13
                supportedFeatures |= FEATURE_HAS_ANALOG_OUT;
                UART_READ_UNESCAPED(); 
                supported_feature.analog_output.Channels=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_DISPLAY:                 // -> 0x14
                supportedFeatures |= FEATURE_HAS_DISPLAY;   
                UART_READ_UNESCAPED(); 
                supported_feature.character_output.width=incomingByte;
                UART_READ_UNESCAPED(); 
                supported_feature.character_output.height=incomingByte;
                UART_READ_UNESCAPED(); 
                supported_feature.character_output.type=incomingByte;
            break;

            case FEATURE_FUNCTION_CODE_BACKUP:                  // -> 0x15
                supportedFeatures |= FEATURE_HAS_BACKUP;
                UART_READ_UNESCAPED(); 
                supported_feature.backup_output.has_backup=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED();
            break;

            case 0x00:
                endCodeReached=true;
            break;

            default:
                UART_READ_UNESCAPED(); 
                TRACE("Warning, non-standard input/output function returned by the IO Board!\n", 1);
                UART_READ_UNESCAPED();
                UART_READ_UNESCAPED();
            break;
        }
    }
    //read SUM
    UART_READ_UNESCAPED();

    TRACE("\n", 2);
    return true;
}


inline bool JVS::parseLightgunInputChannel(gamepad_state_t* gamepad_state)
{
    if(!gamepad_state){
        uartReadMultipleUnescaped(5);
        return true;
    }
    else{
        /* Report Code for command SCRPOSINP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

    /* Analog Channel 1 (X) */
        UART_READ_UNESCAPED(); // MSB
        if(BETWEEN(incomingByte, analogEstimatedFuzz[board][0], analogEstimatedFuzz[board][1]))
            gamepad_state->l_x_axis = incomingByte;
        UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)

    /* Analog Channel 1 (X) */
        UART_READ_UNESCAPED(); // MSB
        if(BETWEEN(incomingByte, analogEstimatedFuzz[board][2], analogEstimatedFuzz[board][3]))
            gamepad_state->l_y_axis = incomingByte;
        UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)

        return true;
    }
}

inline bool JVS::parseAnalogInput()
{
    /* Report Code for first command ANLINP */
    UART_READ_UNESCAPED();
    if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
        return false;

    if(!gamepad_state_p1){
        uartReadMultipleUnescaped(4);
    }
    else{
        /* Analog Channel 1 (X) */
        UART_READ_UNESCAPED();  // MSB
        if(BETWEEN(incomingByte, analogEstimatedFuzz[board][0], analogEstimatedFuzz[board][1]))
            gamepad_state_p1->l_x_axis = incomingByte;
        UART_READ_UNESCAPED();  // LSB (not used here, PS3 analog precision is only 255 wide)

        /* Analog Channel 2 (Y) */
        UART_READ_UNESCAPED(); // MSB
        if(BETWEEN(incomingByte, analogEstimatedFuzz[board][2], analogEstimatedFuzz[board][3]))
            gamepad_state_p1->l_y_axis = incomingByte;
        UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)
    }

    if(!gamepad_state_p2){
        uartReadMultipleUnescaped(4);
    }
    else{
        /* Analog Channel 3 (Z) */
        UART_READ_UNESCAPED(); // MSB
        if(BETWEEN(incomingByte, analogEstimatedFuzz[board][4], analogEstimatedFuzz[board][5]))
            gamepad_state_p2->l_x_axis = incomingByte;
        UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)

        /* Analog Channel 4 (Za) */
        UART_READ_UNESCAPED(); // MSB
        if(BETWEEN(incomingByte, analogEstimatedFuzz[board][6], analogEstimatedFuzz[board][7]))            
            gamepad_state_p2->l_y_axis = incomingByte;
        UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)
    }
    return true;
}

inline bool JVS::parseCoinInput()
{
    /* Report Code for first command COININP */
    UART_READ_UNESCAPED();
    if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
        return false;

    if(!gamepad_state_p1){
        uartReadMultipleUnescaped(2);
    }
    else{
        /* Slot 1 status on 2 first bits (on the left) */
        UART_READ_UNESCAPED();
        
        /* Slot 1 coin */
        UART_READ_UNESCAPED();
        if (incomingByte > initialSlot1CoinValue)
        {
            initialSlot1CoinValue = incomingByte;
            if(initialSlot1CoinValue>0)
                gamepad_state_p1->start_btn=1;
        }
    }
    if(!gamepad_state_p2){
        uartReadMultipleUnescaped(2);
    }
    else{
        /* Slot 2 status on 2 first bits (on the left) */
        UART_READ_UNESCAPED();
        
        /* Slot 2 coin */
        UART_READ_UNESCAPED();
        if (incomingByte > initialSlot1CoinValue)
        {
            initialSlot1CoinValue = incomingByte;
            if(initialSlot1CoinValue>0)
                gamepad_state_p2->start_btn=1;
        }
    }
    return true;
}

inline bool JVS::parseSwitchInput()
{
    /* Report Code for first command SWINP */
    UART_READ_UNESCAPED();
    if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
        return false;

    /* Tilt & Test buttons */ 
    UART_READ_UNESCAPED();
    gamepad_state_p1->select_btn = (BTN_GENERAL_TEST==(incomingByte & BTN_GENERAL_TEST));

    /* 2 next bytes for player 1 */
    parseSwitchInputPlayer(gamepad_state_p1);

    /* 2 next bytes for player 2 */
    parseSwitchInputPlayer(gamepad_state_p2);

    return true;
}

inline void JVS::uartReadMultipleUnescaped(int nbr)
{
    for(int cp=0; cp < nbr; cp++){
        UART_READ_UNESCAPED();
    }
}

inline void JVS::parseSwitchInputPlayer(gamepad_state_t* gamepad_state)
{   
    if(!gamepad_state){
            uartReadMultipleUnescaped(2);
    }
    else{
        /* First byte switch player x */
        UART_READ_UNESCAPED();
        //START + Button 1 + Button 2 -> Restart Teensy
        if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
        {
            _reboot_Teensyduino_();
        }
        //START + Button 1 -> PS Button
        else if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
        {
            gamepad_state->start_btn=0;
            gamepad_state->cross_btn=0;
            gamepad_state->ps_btn=1;
        }
        //START + Button 2 -> Select
        else if((BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
        {
            gamepad_state->start_btn=0;
            gamepad_state->circle_btn=0;
            gamepad_state->select_btn=1;
        }
        //Start
        else if((BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
        {
            gamepad_state->ps_btn=0;
            gamepad_state->select_btn=0;
            gamepad_state->start_btn=1;
        }
        else{
            gamepad_state->ps_btn=0;
            gamepad_state->select_btn=0;
            gamepad_state->start_btn=0;

            //Other button combinations
            gamepad_state->cross_btn   = (BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1));
            gamepad_state->cross_axis  = gamepad_state->cross_btn * 0xFF;
            gamepad_state->circle_btn  = (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2));
            gamepad_state->circle_axis = gamepad_state->circle_btn * 0xFF;
            
            gamepad_state->direction = 8; // Center
            gamepad_state->l_x_axis=0x80;
            gamepad_state->l_y_axis=0x80;

            if ((BTN_PLAYER_DOWN==(incomingByte & BTN_PLAYER_DOWN))) {
                gamepad_state->direction = 4;
                gamepad_state->l_y_axis=0xFF;
                if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) {
                    gamepad_state->direction = 3;
                    gamepad_state->l_x_axis=0xFF;}
                else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))) {
                    gamepad_state->direction = 5;
                    gamepad_state->l_x_axis=0x00;}
            }
            else {
                if ((BTN_PLAYER_UP==(incomingByte & BTN_PLAYER_UP))) {
                    gamepad_state->direction = 0;
                    gamepad_state->l_y_axis=0x00;
                    if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) {
                        gamepad_state->direction = 1;
                        gamepad_state->l_x_axis=0xFF;}
                    else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))){
                        gamepad_state->direction = 7;
                        gamepad_state->l_x_axis=0x00;}
                }
                else {
                    if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) {
                        gamepad_state->direction = 2;
                        gamepad_state->l_x_axis=0xFF;}
                    else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))) {
                        gamepad_state->direction = 6;
                        gamepad_state->l_x_axis=0x00;}
                }
            }
        }

        /* second byte switch player x */
        UART_READ_UNESCAPED();
        gamepad_state->square_btn    = (BTN_PLAYER_PUSH3==(incomingByte & BTN_PLAYER_PUSH3));
        gamepad_state->square_axis   = gamepad_state->square_btn * 0xFF;
        gamepad_state->triangle_btn  = (BTN_PLAYER_PUSH4==(incomingByte & BTN_PLAYER_PUSH4));
        gamepad_state->triangle_axis = gamepad_state->triangle_btn * 0xFF;
        gamepad_state->l1_btn        = (BTN_PLAYER_PUSH5==(incomingByte & BTN_PLAYER_PUSH5));
        gamepad_state->l1_axis       = gamepad_state->l1_btn * 0xFF;
        gamepad_state->r1_btn        = (BTN_PLAYER_PUSH6==(incomingByte & BTN_PLAYER_PUSH6));
        gamepad_state->r1_axis       = gamepad_state->r1_btn * 0xFF;
        gamepad_state->l2_btn        = (BTN_PLAYER_PUSH7==(incomingByte & BTN_PLAYER_PUSH7));
        gamepad_state->r2_btn        = (BTN_PLAYER_PUSH8==(incomingByte & BTN_PLAYER_PUSH8));
    }
}

// Check the request status returned by the slave
bool JVS::checkRequestStatus(char requestStatus)
{
        if(requestStatus == REQUEST_STATUS_NORMAL)
            return true;
        else if(requestStatus == REQUEST_STATUS_COMMAND_UNKNOWN)
            TRACE("Warning, command unknown\n", 1);
        else if(requestStatus == REQUEST_STATUS_SUM_ERROR)
            TRACE("Warning, slave detected a SUM Error\n",1);
        else if(requestStatus == REQUEST_STATUS_BUSY)
            TRACE("ERROR, slave is too busy, it can't process the command\n", 1);
        return false;
}

//Check the report 
bool JVS::checkReportCode(char reportCode)
{
    if(reportCode == REPORT_CODE_NORMAL)
        return true;
    else if(reportCode == REPORT_CODE_PARAM_ERROR)
        TRACE("Warning, command parameter error, no return data\n", 1);
    else if(reportCode == REPORT_CODE_PARAM_DATA_ERROR)
        TRACE("Warning, command parameter error, parameter is ignored\n", 1);
    else if(reportCode == REPORT_CODE_BUSY)
        TRACE("ERROR, slave is too busy, it can't process the sub command\n", 1);
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
int* JVS::cmd(int destination, char data[], int requestSize) {

    this->writePacket(destination, data, requestSize);

    char incomingByte;

    int length = waitForPayload();

    //Result contains payload without length & checksum
    int* res = (int*)malloc(length-2 * sizeof(int));
    
    for (int counter=0; counter < length-1; counter++) {
        WAIT_UART_AVAILABLE(destination);
        incomingByte = _Uart.read();

        TRACE(" ", 2);
        PHEX(incomingByte, 2);

        //Check if the marker('Escape Byte') has been used -> 0xE0 or 0xD0 is in the payload.
        //If so, restore original value.

        if (incomingByte == 0xD0) {    
            incomingByte = _Uart.read();  
            incomingByte++;                
        }

        //don't put checksum in the response
        if (counter < length-2)
            res[counter] = incomingByte;
    }
    
    TRACE("\n", 2);
    return res;
}

int JVS::waitForPayload()
{
    int length=0;

    TRACE("wait UART avail.\n", 2);
    WAIT_UART_AVAILABLE(board);

    // E0 SYNC
    // 00 Address, 00 is master
    // XX Length
    TRACE("wait UART sync (0xE0)\n", 2);
    WAIT_UART_READ(0XE0); // wait for sync

    TRACE("Test for me (0x00)\n", 2);
    WAIT_UART_READ(0); // only if it's for me

    TRACE("wait UART avail.\n",2);
    WAIT_UART_AVAILABLE(board);

    length = _Uart.read();
    TRACE(" -> Received: E0 00 ", 2);
    PHEX(length, 2); // <-- This is not always the lengh but Error code too !!!
    TRACE("\n", 2);

    delayMicroseconds(100);
    if(checkRequestStatus(_Uart.read()))
        return length;
    else
        return 0;
}

void JVS::writePacket(int destination, char data[], int size) {
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


