#include <Arduino.h>
#include "jvshost.h"
#include "jvsclient.h"
#include "hidtrace.h"
#include "config.h"

unsigned long time;

JvsHost::JvsHost(HardwareSerial& serial) :
    JvsUart(serial)
{
    pinMode(6, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
}

/* Broadcast Reset communication status to all slaves     */
/* The request is sent twice, as recommanded by the sepcs */
void JvsHost::resetAllClients(){
    char str[] = { CMD_RESET, CMD_RESET_ARG };
    this->writePacket(BROADCAST, str, 2);  
    this->writePacket(BROADCAST, str, 2); 
    
    delay(1500);

    for(int cp=0; cp < MAX_JVS_CLIENT; cp++)
        if(jvsClient[cp]) delete jvsClient[cp];
    
    jvsClientCount=0;
}

/* Broadcast Reset communication status to all slaves */
/*  Note that the sense line is pulled down by the client only if there is no other client address, */
/*  thus the last request will be unsuccessfull and the sense line will be dropped down             */
bool JvsHost::GetNextClient() {
    if(jvsClientCount==MAX_JVS_CLIENT)
        return false;

    //int board= jvsClientCount++;
    char str[] = { CMD_ASSIGN_ADDR, (char)(jvsClientCount + 1)};
    char* rs=this->cmd(BROADCAST, str, 2); 
    //this->writePacket(BROADCAST, str, 2);

    /* Wait for the client to pull down the line */
    delay(200);
    
    free (rs);

    if (analogRead(SENSE_PIN) > 50){
        jvsClientCount++;
        jvsClient[jvsClientCount-1]= new JvsClient(jvsClientCount,
                                                    configGamepad[jvsClientCount-1][0], 
                                                    configGamepad[jvsClientCount-1][1]);
        
        return true;
    }
    else
        return false;
}

void JvsHost::dumpBaseBoardInfo(int boardIndex) {
    TRACE_ARGS( 1, "General information for client:  %d\n", 1, jvsClient[boardIndex]->address);
    TRACE_ARGS( 1, " - IO identity:      %s\n", jvsClient[boardIndex]->ioIdentity); 
    TRACE_ARGS( 1, " - JVS version:      %d\n", jvsClient[boardIndex]->jvsVersion); 
    TRACE_ARGS( 1, " - Command version:  %d\n", jvsClient[boardIndex]->commandVersion); 
    TRACE_ARGS( 1, " - Command revision: %d\n", jvsClient[boardIndex]->commandRevision); 
}

void JvsHost::getBaseBoardInfo(int boardIndex) {
    char* rs;
  
    char str1[] = { CMD_REQUEST_ID };                       // -> Master requests information about maker, IO board code, etc. (0x10)
    rs=this->cmd(jvsClient[boardIndex]->address, str1, 1);  //    Request size: 1  | Response size: max 102
    sprintf(jvsClient[boardIndex]->ioIdentity,"%s",rs);
    free (rs);

    char str2[] = { CMD_COMMAND_VERSION };                  // -> Command format revision (0x11)
    rs=this->cmd(jvsClient[boardIndex]->address, str2, 1);  //    Request size: 1  | Response size: 2 in BCD format
    sprintf(jvsClient[boardIndex]->commandVersion,"%d.%d", atoi(rs) & 0xF0, atoi(rs) & 0x0F);
    free (rs);

    char str3[] = { CMD_JVS_VERSION };                      // -> JVS revision (0x12)
    rs=this->cmd(jvsClient[boardIndex]->address, str3, 1);  //    Request size: 1  | Response size: 2 in BCD format
    sprintf(jvsClient[boardIndex]->jvsVersion,"%d.%d", atoi(rs) & 0xF0, atoi(rs) & 0x0F);
    free (rs);

    char str4[] = { CMD_COMMS_VERSION };                    // -> Communication version
    rs=this->cmd(jvsClient[boardIndex]->address, str4, 1);  //    Request size: 1  | Response size: 2 in BCD format
    sprintf(jvsClient[boardIndex]->commandRevision,"%d.%d", atoi(rs) & 0xF0, atoi(rs) & 0x0F);
    free (rs);
}

void JvsHost::resetAllAnalogFuzz()
{
    size_t numObjects = sizeof(jvsClient)/sizeof(jvsClient[0]);
    for ( size_t i = 0; i < numObjects; ++i  )
    {
        JvsClient* client = jvsClient[i];
        for(int col=0; col<8; col++)
        {
            client->analogFuzz[col]=0xFF;
            col++;
            client->analogFuzz[col]=0x00;            
        }
    }
}

/* debugging function */
void JvsHost::dumpAnalogFuzz(int boardIndex)
{
    TRACE(2, "Board"); TRACE_HEX(2, jvsClient[boardIndex]->address); TRACE(2, ": ");
    for(int col=0; col<8; col++)
    {
        TRACE_HEX(jvsClient[boardIndex]->analogFuzz[col], 2);
        TRACE(2, " ");            
    }

    TRACE(2, "\n");
}

//Mainly used to detect analog fuzz when IO Board supports analog controls but no one is connected
void JvsHost::setAnalogFuzz(int board)
{
    JvsClient* client=jvsClient[board-1];
    char str[] = { CMD_READ_ANALOG, 4};  
    int tolerance=2;

    //Poll 30 time and define min max fuzz
    for(int cp=0; cp<30;cp++)
    {
        this->writePacket(client->address, str, sizeof str);

        int length = getNextResponseLength();

        for (int counter=0; counter < length; counter++) {
            UART_READ_UNESCAPED();

            switch(counter){
                case 0:
                    if((incomingByte)       < client->analogFuzz[0]) client->analogFuzz[0]=incomingByte-tolerance;
                    else if((incomingByte)  > client->analogFuzz[1]) client->analogFuzz[1]=incomingByte+tolerance;
                    break;
                case 2:
                    if((incomingByte)       < client->analogFuzz[2]) client->analogFuzz[2]=incomingByte-tolerance;
                    else if((incomingByte)  > client->analogFuzz[3]) client->analogFuzz[3]=incomingByte+tolerance;
                    break;
                case 4:
                    if((incomingByte)       < client->analogFuzz[4]) client->analogFuzz[4]=incomingByte-tolerance;
                    else if((incomingByte)  > client->analogFuzz[5]) client->analogFuzz[5]=incomingByte+tolerance;
                    break;
                case 6:
                    if((incomingByte)       < client->analogFuzz[6]) client->analogFuzz[6]=incomingByte-tolerance;
                    else if((incomingByte)  > client->analogFuzz[7]) client->analogFuzz[7]=incomingByte+tolerance;
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
//    - ReportCode_1:
//    - Report_1_byte_1:
//    - Report_1_byte_2:
//    - Etc.
//    - ReportCode_2:
//    - Report_2_byte_1:
//    - Report_2_byte_2:
//    - Etc.
//    - ReportCode_3:
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
void JvsHost::getAllClientReports() {
    char str[] = {  CMD_READ_DIGITAL, 0x02, 0x02,       // Command 1: Read input switch for the 2 players in 2 bytes format
                    CMD_READ_COINS, 0x02,               // Command 2: Read coin values for 2 slots
                    CMD_READ_ANALOG, 0x04};             // Command 3: Read analog values for 4 channels
    //                CMD_READ_LIGHTGUN, 0x00,          // Command 4: Read Lightgun values for channel 0
    //                CMD_READ_LIGHTGUN, 0x01};         // Command 5: Read Lightgun values for channel 1
    //TOTAL Measured elapse time: 16 millisec

   size_t numObjects = sizeof(jvsClient)/sizeof(jvsClient[0]);
    for ( size_t i = 0; i < numObjects; ++i  )
    {
        JvsClient* client=jvsClient[i];

        // --- SEND REQUEST ---
        //Send 3 commands at once: SYNC + Node(board) + ByteNbr(sizeof str) + Payload (str) + SUM
        this->writePacket(client->address, str, sizeof str);
        
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
        int length = getNextResponseLength();

        if(length>0)
        {
            parseSwitchInput(client);
            parseCoinInput(client);
            parseAnalogInput(client);
            //parseLightgunInputChannel(client, gamepad_state_p1);
            //parseLightgunInputChannel(client, gamepad_state_p2);

            TRACE(2, "\n");
            
            //read SUM
            UART_READ_UNESCAPED();
        }
    }
}

void JvsHost::getSupportedFeatures(int boardIndex)
{
    TRACE_ARGS(2, "getSupportedFeatures for address %d\n", jvsClient[boardIndex]->address);
    char str[] = {CMD_CAPABILITIES};
    this->writePacket(jvsClient[boardIndex]->address, str, 1);
    
    int length = getNextResponseLength();

    if(length>0)
        parseSupportedFeatures(jvsClient[boardIndex]);
}

void JvsHost::setBulkCommand(int boardIndex)
{
    JvsClient* client=jvsClient[boardIndex];
    memset(client->bulkCommands, 0, sizeof client->bulkCommands);

    int idxCommand=0;

    if (client->supportedFeaturesMask & FEATURE_HAS_PLAYERS)   {
        client->bulkCommands[idxCommand][0] = CMD_READ_DIGITAL; // Read input switch for the x players in 2 bytes format
        client->bulkCommands[idxCommand][1] = client->supportedFeatures.switch_input.Players;
        client->bulkCommands[idxCommand][2] = 0x02; 
        idxCommand++;
    }

    if (client->supportedFeaturesMask & FEATURE_HAS_COINS)     {
        client->bulkCommands[idxCommand][0] = CMD_READ_COINS;   // Read coin values for x slots
        client->bulkCommands[idxCommand][1] = client->supportedFeatures.coin_input.Slots;
        idxCommand++;
    }

    if (client->supportedFeaturesMask & FEATURE_HAS_ANALOG_IN) {
        client->bulkCommands[idxCommand][0] = CMD_READ_ANALOG;   // Read analog values for x channels
        client->bulkCommands[idxCommand][1] = client->supportedFeatures.analog_input.Channels;
        idxCommand++;
    }

    if (client->supportedFeaturesMask & FEATURE_HAS_ROTARY)    {
        //TODO
    }

    if (client->supportedFeaturesMask & FEATURE_HAS_LIGHTGUN)  {
        for(int cp=0; cp < client->supportedFeatures.screen_position_input.Channels; cp++){
            client->bulkCommands[idxCommand][0] = CMD_READ_LIGHTGUN;   // Read lightgun channels
            client->bulkCommands[idxCommand][1] = cp;
            idxCommand++;
        }
    }
}

void JvsHost::dumpSupportedFeatures(int boardIndex)
{
    JvsClient* client=jvsClient[boardIndex];
    TRACE(1, "Features:\n");
    if (client->supportedFeaturesMask & FEATURE_HAS_PLAYERS){
        TRACE_ARGS( 1, " - Players: Number of Players               -> %d\n", jvsClient[boardIndex]->supportedFeatures.switch_input.Players);
        TRACE_ARGS( 1, " - Players: Number of buttons per player    -> %d\n", jvsClient[boardIndex]->supportedFeatures.switch_input.ButtonsPerPlayer);}
    if (client->supportedFeaturesMask & FEATURE_HAS_COINS)
        TRACE_ARGS( 1, " - Coins: Number of slots                   -> %d\n", jvsClient[boardIndex]->supportedFeatures.coin_input.Slots);
    if (client->supportedFeaturesMask & FEATURE_HAS_ANALOG_IN){
        TRACE_ARGS( 1, " - Analog input: Number of channels         -> %d\n", jvsClient[boardIndex]->supportedFeatures.analog_input.Channels);
        TRACE_ARGS( 1, " - Analog input: Bits per channel           -> %d\n", jvsClient[boardIndex]->supportedFeatures.analog_input.Bits);}
    if (client->supportedFeaturesMask & FEATURE_HAS_ROTARY)
        TRACE_ARGS( 1, " - Rotary:Number of rotary encoders         -> %d\n", jvsClient[boardIndex]->supportedFeatures.rotary_input.Channels);
    if (client->supportedFeaturesMask & FEATURE_HAS_LIGHTGUN){
        TRACE_ARGS( 1, " - Lightgun: Number of channels             -> %d\n", jvsClient[boardIndex]->supportedFeatures.screen_position_input.Channels);
        TRACE_ARGS( 1, " - Lightgun: Number of effective bits for X -> %d\n", jvsClient[boardIndex]->supportedFeatures.screen_position_input.Xbits);
        TRACE_ARGS( 1, " - Lightgun: Number of effective bits for Y -> %d\n", jvsClient[boardIndex]->supportedFeatures.screen_position_input.Ybits);}
    if (client->supportedFeaturesMask & FEATURE_HAS_CARD)        
        TRACE_ARGS( 1, " - Card System: Number of card slots        -> %d\n", jvsClient[boardIndex]->supportedFeatures.card_system_output.Slots);
    if (client->supportedFeaturesMask & FEATURE_HAS_HOPPER)
        TRACE_ARGS( 1, " - Medal hopper: Number of medal hoppers    -> %d\n", jvsClient[boardIndex]->supportedFeatures.medal_hopper_output.Channels);
    if (client->supportedFeaturesMask & FEATURE_HAS_GENERAL_PURPOSE_OUT)
        TRACE_ARGS( 1, " - General purpose output: Number of slots  -> %d\n", jvsClient[boardIndex]->supportedFeatures.general_purpose_output.Slots);
    if (client->supportedFeaturesMask & FEATURE_HAS_ANALOG_OUT)    
        TRACE_ARGS( 1, " - Analog output: Number of channels        -> %d\n", jvsClient[boardIndex]->supportedFeatures.analog_output.Channels);
    if (client->supportedFeaturesMask & FEATURE_HAS_DISPLAY){    
        TRACE_ARGS( 1, " - Display output: width                    -> %d\n", jvsClient[boardIndex]->supportedFeatures.character_output.width);
        TRACE_ARGS( 1, " - Display output: height                   -> %d\n", jvsClient[boardIndex]->supportedFeatures.character_output.height);
        TRACE_ARGS( 1, " - Display output: type                     -> %d\n", jvsClient[boardIndex]->supportedFeatures.character_output.type);}
    if (client->supportedFeaturesMask & FEATURE_HAS_BACKUP)        
        TRACE_ARGS( 1, " - Backup: has data                         -> %d\n", jvsClient[boardIndex]->supportedFeatures.backup_output.has_backup);

    //Wait for the serial to send all the data
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
inline bool JvsHost::parseSupportedFeatures(JvsClient* client)
{
    bool endCodeReached=false;

    /* Report Code for command SCRPOSINP */
    UART_READ_UNESCAPED();

    if(checkRequestStatus(incomingByte)!=REQUEST_STATUS_NORMAL)
        return false;
    
    //Each function (except for end code) is 4 bytes, consisting of 1 byte for function code + 3 bytes for parameters
    while(!endCodeReached){
        UART_READ_UNESCAPED();      // -> read function code

        switch (incomingByte) {
            case FEATURE_FUNCTION_CODE_PLAYERS:               //-> 0x01
                client->supportedFeaturesMask |= FEATURE_HAS_PLAYERS;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.switch_input.Players=incomingByte;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.switch_input.ButtonsPerPlayer=incomingByte;
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_COINS:                 //-> 0x02
                client->supportedFeaturesMask |= FEATURE_HAS_COINS; 
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.coin_input.Slots=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_ANALOG_IN:             //-> 0x03
                client->supportedFeaturesMask |= FEATURE_HAS_ANALOG_IN;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.analog_input.Channels=incomingByte;
                UART_READ_UNESCAPED();
                client->supportedFeatures.analog_input.Bits=incomingByte; 
                UART_READ_UNESCAPED();                 
            break;

            case FEATURE_FUNCTION_CODE_ROTARY:                //-> 0x04
                client->supportedFeaturesMask |= FEATURE_HAS_ROTARY;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.rotary_input.Channels=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_KEYPAD:                //-> 0x05
                client->supportedFeaturesMask |= FEATURE_HAS_KEYPAD;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.keycode_input.byte1=incomingByte;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.keycode_input.byte2=incomingByte;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.keycode_input.byte3=incomingByte;
            break;

            case FEATURE_FUNCTION_CODE_LIGHTGUN:              //-> 0x06
                client->supportedFeaturesMask |= FEATURE_HAS_LIGHTGUN;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.screen_position_input.Xbits=incomingByte;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.screen_position_input.Ybits=incomingByte;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.screen_position_input.Channels=incomingByte;
            break;

            case FEATURE_FUNCTION_CODE_GENERAL_PURPOSE_IN:    // -> 0x07
                client->supportedFeaturesMask |= FEATURE_HAS_GENERAL_PURPOSE_IN;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.misc_switch_input.byte1=incomingByte;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.misc_switch_input.byte2=incomingByte;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.misc_switch_input.byte3=incomingByte;
            break;

            case FEATURE_FUNCTION_CODE_CARD:                  //-> 0x10
                client->supportedFeaturesMask |= FEATURE_HAS_CARD;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.card_system_output.Slots=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_HOPPER:                //-> 0x11
                client->supportedFeaturesMask |= FEATURE_HAS_HOPPER;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.medal_hopper_output.Channels=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_GENERAL_PURPOSE_OUT:   // -> 0x12
                client->supportedFeaturesMask |= FEATURE_HAS_GENERAL_PURPOSE_OUT;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.general_purpose_output.Slots=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_ANALOG_OUT:              // -> 0x13
                client->supportedFeaturesMask |= FEATURE_HAS_ANALOG_OUT;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.analog_output.Channels=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED(); 
            break;

            case FEATURE_FUNCTION_CODE_DISPLAY:                 // -> 0x14
                client->supportedFeaturesMask |= FEATURE_HAS_DISPLAY;   
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.character_output.width=incomingByte;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.character_output.height=incomingByte;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.character_output.type=incomingByte;
            break;

            case FEATURE_FUNCTION_CODE_BACKUP:                  // -> 0x15
                client->supportedFeaturesMask |= FEATURE_HAS_BACKUP;
                UART_READ_UNESCAPED(); 
                client->supportedFeatures.backup_output.has_backup=incomingByte;
                UART_READ_UNESCAPED(); 
                UART_READ_UNESCAPED();
            break;

            case 0x00:
                endCodeReached=true;
            break;

            default:
                UART_READ_UNESCAPED(); 
                TRACE(1, "Warning, non-standard input/output function returned by the IO Board!\n");
                UART_READ_UNESCAPED();
                UART_READ_UNESCAPED();
            break;
        }
    }
    //read SUM
    UART_READ_UNESCAPED();

    TRACE(2, "\n");
    return true;
}


inline bool JvsHost::parseLightgunInputChannel(JvsClient* client, gamepad_state_t* gamepad_state)
{
    if (client->supportedFeaturesMask & FEATURE_HAS_LIGHTGUN)
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
            if(BETWEEN(incomingByte, client->analogFuzz[0], client->analogFuzz[1]))
                gamepad_state->l_x_axis = incomingByte;
            UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)

        /* Analog Channel 1 (X) */
            UART_READ_UNESCAPED(); // MSB
            if(BETWEEN(incomingByte, client->analogFuzz[2], client->analogFuzz[3]))
                gamepad_state->l_y_axis = incomingByte;
            UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)
        }
    }
    return true;
}

inline bool JvsHost::parseAnalogInput(JvsClient* client)
{
    if (client->supportedFeaturesMask & FEATURE_HAS_ANALOG_IN) 
    {
        /* Report Code for first command ANLINP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        if(!client->gamepad_state_p1){
            uartReadMultipleUnescaped(4);
        }
        else{
            /* Analog Channel 1 (X) */
            UART_READ_UNESCAPED();  // MSB
            if(BETWEEN(incomingByte, client->analogFuzz[0], client->analogFuzz[1]))
                client->gamepad_state_p1->l_x_axis = incomingByte;
            UART_READ_UNESCAPED();  // LSB (not used here, PS3 analog precision is only 255 wide)

            /* Analog Channel 2 (Y) */
            UART_READ_UNESCAPED(); // MSB
            if(BETWEEN(incomingByte, client->analogFuzz[2], client->analogFuzz[3]))
                client->gamepad_state_p1->l_y_axis = incomingByte;
            UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)
        }

        if(!client->gamepad_state_p2){
            uartReadMultipleUnescaped(4);
        }
        else{
            /* Analog Channel 3 (Z) */
            UART_READ_UNESCAPED(); // MSB
            if(BETWEEN(incomingByte, client->analogFuzz[4], client->analogFuzz[5]))
                client->gamepad_state_p2->l_x_axis = incomingByte;
            UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)

            /* Analog Channel 4 (Za) */
            UART_READ_UNESCAPED(); // MSB
            if(BETWEEN(incomingByte, client->analogFuzz[6], client->analogFuzz[7]))            
                client->gamepad_state_p2->l_y_axis = incomingByte;
            UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)
        }
    }
    return true;
}

inline bool JvsHost::parseCoinInput(JvsClient* client)
{
    if (client->supportedFeaturesMask & FEATURE_HAS_COINS) 
    {
        /* Report Code for first command COININP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        if(!client->gamepad_state_p1){
            uartReadMultipleUnescaped(2);
        }
        else{
            /* Slot 1 status on 2 first bits (on the left) */
            UART_READ_UNESCAPED();
            
            /* Slot 1 coin */
            UART_READ_UNESCAPED();
            if (incomingByte > client->initialSlot1CoinValue)
            {
                client->initialSlot1CoinValue = incomingByte;
                if(client->initialSlot1CoinValue>0)
                    client->gamepad_state_p1->start_btn=1;
            }
        }
        if(!client->gamepad_state_p2){
            uartReadMultipleUnescaped(2);
        }
        else{
            /* Slot 2 status on 2 first bits (on the left) */
            UART_READ_UNESCAPED();
            
            /* Slot 2 coin */
            UART_READ_UNESCAPED();
            if (incomingByte > client->initialSlot1CoinValue)
            {
                client->initialSlot1CoinValue = incomingByte;
                if(client->initialSlot1CoinValue>0)
                    client->gamepad_state_p2->start_btn=1;
            }
        }
    }
    return true;
}

inline bool JvsHost::parseSwitchInput(JvsClient* client)
{
    if (client->supportedFeaturesMask & FEATURE_HAS_PLAYERS)
    {
        /* Report Code for first command SWINP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        /* Tilt & Test buttons */ 
        UART_READ_UNESCAPED();
        client->gamepad_state_p1->select_btn = (BTN_GENERAL_TEST==(incomingByte & BTN_GENERAL_TEST));

        /* 2 next bytes for player 1 */
        parseSwitchInputPlayer(client->gamepad_state_p1);

        /* 2 next bytes for player 2 */
        parseSwitchInputPlayer(client->gamepad_state_p2);
    }
    return true;
}

inline void JvsHost::uartReadMultipleUnescaped(int nbr)
{
    for(int cp=0; cp < nbr; cp++){
        UART_READ_UNESCAPED();
    }
}

inline void JvsHost::parseSwitchInputPlayer(gamepad_state_t* gamepad_state)
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
bool JvsHost::checkRequestStatus(char requestStatus)
{
        TRACE(2, " "); TRACE_HEX(2, requestStatus);
        if(requestStatus == REQUEST_STATUS_NORMAL)
            return true;
        else if(requestStatus == REQUEST_STATUS_COMMAND_UNKNOWN)
            TRACE( 1, "Warning, command unknown\n");
        else if(requestStatus == REQUEST_STATUS_SUM_ERROR)
            TRACE( 1, "Warning, slave detected a SUM Error\n");
        else if(requestStatus == REQUEST_STATUS_BUSY)
            TRACE( 1, "ERROR, slave is too busy, it can't process the command\n");
        return false;
}

//Check the report 
bool JvsHost::checkReportCode(char reportCode)
{
    TRACE(2, " "); TRACE_HEX(reportCode, 2);
    if(reportCode == REPORT_CODE_NORMAL)
        return true;
    else if(reportCode == REPORT_CODE_PARAM_ERROR)
        TRACE( 1, "Warning, command parameter error, no return data\n");
    else if(reportCode == REPORT_CODE_PARAM_DATA_ERROR)
        TRACE( 1, "Warning, command parameter error, parameter is ignored\n");
    else if(reportCode == REPORT_CODE_BUSY)
        TRACE( 1, "ERROR, slave is too busy, it can't process the sub command\n");
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
char* JvsHost::cmd(char destination, char data[], int requestSize) {
    this->writePacket(destination, data, requestSize);

    int length = getNextResponseLength();

    // Result contains payload without length & checksum
    // Result will be stored in the heap for later use
    // Result will not contain the SUM but null terminator
    char* res = (char*)malloc(length * sizeof(char));
    memset(res, 0, sizeof(*res));

    for (int counter=0; counter < length; counter++) {
        incomingByte = this->getByte();

        TRACE(2, " ");
        TRACE_HEX(incomingByte, 2);

        // Check if the marker('Escape Byte') has been used -> 0xE0 or 0xD0 is in the payload.
        // If so, restore original value.
        if (incomingByte == 0xD0) {    
            incomingByte = this->getByte();  
            incomingByte++;                
        }

        // don't put checksum in the response
        if (counter < length)
            res[counter] = incomingByte;
    }
    
    res[length-1]='\0';

    TRACE(2, "here\n");
    return res;
}

/* debugging function */
void JvsHost::checkUart()
{
    TRACE_ARGS( 1, "UART Available: %d\n", _Uart.available());
    TRACE_ARGS( 1, "UART availableForWrite: /d\n", _Uart.availableForWrite());
}

/* This function:
     - waits for the reply packet
     - consumes the first byte of Payload to verify the request status 
     - returns 0 if request status is in error or returns the remaining bytes to read */
int JvsHost::getNextResponseLength()
{
    int length=0;

    // E0 SYNC
    // 00 Address, 00 is master
    // XX Length
    TRACE(2, "wait UART sync (0xE0)\n");
    UART_SEEK(0XE0); // wait for sync

    TRACE(2, "Test for me (0x00)\n");
    UART_SEEK(0); // Check if response packet is for host

    length = this->getByte();
    TRACE(2, " -> Received: E0 00 "); TRACE_HEX(2, length); 
    TRACE(2,"\n");

    if(checkRequestStatus(this->getByte()))
        return length - 1;
    else
        return 0;
}

