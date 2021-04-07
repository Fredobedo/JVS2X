#include <Arduino.h>
#include "JVS_HOST/jvs_host.h"

JvsHost::JvsHost(HardwareSerial& serial) :
    JVSHOSTHELPER(serial)
{
    pinMode(6, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
}

/* Broadcast Reset communication status to all slaves     */
/* The request is sent twice, as recommanded by the sepcs */
void JvsHost::resetAll(){
    delay(SPECS_TIMING_MASTER_START);

    char str[] = { (char)CMD_RESET, (char)CMD_RESET_ARG };
    this->writePacket((char)BROADCAST, str, 2);  
    delay(SPECS_TIMING_SLAVE_START_AFTER_RESET);
    this->writePacket((char)BROADCAST, str, 2); 
    delay(SPECS_TIMING_SLAVE_START_AFTER_RESET);

    for(int cp=0; cp < MAX_JVS_CLIENT; cp++)
        if(jvsClient[cp]) delete jvsClient[cp];
    
    jvsClientCount=0;
    errorTimeout=false;
}

bool JvsHost::GetNextClient() {
    if(jvsClientCount==MAX_JVS_CLIENT)
        return false;

	char response[100];
    int responseLen=0;

    char str[] = { (char)CMD_ASSIGN_ADDR, (char)(jvsClientCount+1)};

    if(sendCommand((char)BROADCAST, str, 2, response, responseLen)){
        jvsClient[jvsClientCount]= new JvsClient(jvsClientCount+1);
        jvsClientCount++;
    }

    delay(SPECS_TIMING_MASTER_ADDRESS_SETTING_INTERVAL);

    return (analogRead(SENSE_PIN) > 50 && !errorTimeout);
}

void JvsHost::dumpBaseBoardInfo(int boardIndex) {
#ifdef JVSDEBUG
    TRACE_ARGS_P( 1, "General information for client:  %d\n", 1, jvsClient[boardIndex]->address);
    TRACE_ARGS_P( 1, " - IO identity:      %s\n", jvsClient[boardIndex]->ioIdentity); 
    TRACE_ARGS_P( 1, " - JVS version:      %s\n", jvsClient[boardIndex]->jvsVersion); 
    TRACE_ARGS_P( 1, " - Command version:  %s\n", jvsClient[boardIndex]->commandVersion); 
    TRACE_ARGS_P( 1, " - Command revision: %s\n", jvsClient[boardIndex]->commandRevision); 
#endif
}

bool JvsHost::getBaseBoardInfo(int boardIndex) {
    char response[100];
    int responseLen=0;

    char str1[] = { CMD_REQUEST_ID };                                                     // -> Master requests information about maker, IO board code, etc. (0x10)
    if(!sendCommand(jvsClient[boardIndex]->address, str1, 1, response, responseLen))      //    Request size: 1  | Response size: max 102
        return false;

    sprintf(jvsClient[boardIndex]->ioIdentity,"%s",response);

    char str2[] = { CMD_COMMAND_VERSION };                                                // -> Command format revision (0x11)
    if(!sendCommand(jvsClient[boardIndex]->address, str2, 1, response, responseLen))      //    Request size: 1  | Response size: 2 in BCD format
        return false;

    sprintf(jvsClient[boardIndex]->commandVersion,"%d.%d", ((int)(response[1]) & 0xF0) >> 4, (int)(response[1]) & 0x0F);

    char str3[] = { CMD_JVS_VERSION };                                                    // -> JVS revision (0x12)
    if(!sendCommand(jvsClient[boardIndex]->address, str3, 1, response, responseLen))      //    Request size: 1  | Response size: 2 in BCD format
        return false;

    sprintf(jvsClient[boardIndex]->jvsVersion,"%d.%d", ((int)(response[1]) & 0xF0) >> 4, (int)(response[1]) & 0x0F);

    char str4[] = { CMD_COMMS_VERSION };                                                  // -> Communication version
    if (!sendCommand(jvsClient[boardIndex]->address, str4, 1, response, responseLen))     //    Request size: 1  | Response size: 2 in BCD format
        return false;

    sprintf(jvsClient[boardIndex]->commandRevision,"%d.%d", ((int)(response[1]) & 0xF0) >> 4, (int)(response[1]) & 0x0F);
    return true;
}

void JvsHost::resetAllAnalogFuzz()
{
    size_t numObjects = sizeof(jvsClient)/sizeof(jvsClient[0]);
    for ( size_t i = 0; i < numObjects; ++i  )
    {
        JvsClient* client = jvsClient[i];
        for(int col=0; col<8; col++)
        {
            client->analogFuzz[col][0]=0xFF;
            col++;
            client->analogFuzz[col][1]=0x00;            
        }
    }
}

/* debugging function */
void JvsHost::dumpAnalogFuzz(int boardIndex)
{
    TRACE_ARGS_P(2, "Analog Fuzz for client %d: ", jvsClient[boardIndex]->address);
    for(int col=0; col<8; col++)
        TRACE_ARGS(2, " %02X", jvsClient[boardIndex]->analogFuzz[col]);

    TRACE(2, "\n");
}

/* mainly used to detect analog fuzz when IO Board supports analog controls but no one is connected */
/* this is a very basic implementation but it looks ok for now                                      */
void JvsHost::setAnalogFuzz(int boardIndex)
{
    JvsClient* client=jvsClient[boardIndex];
    char str[] = { CMD_READ_ANALOG, (char)client->supportedFeatures.analog_output.Channels};  
    int tolerance=2;

    //Poll 30 time all supported channels and define min max fuzz
    for(int i=0; i<30;i++)
    {
        this->writePacket(client->address, str, sizeof str);

        if(getResponseLength())
        {
            for (int cp=0; cp < client->supportedFeatures.analog_output.Channels; cp++) {
                UART_READ_UNESCAPED();
                if((incomingByte)       < client->analogFuzz[cp][0]) client->analogFuzz[cp][0]=incomingByte-tolerance;
                else if((incomingByte)  > client->analogFuzz[cp][1]) client->analogFuzz[cp][1]=incomingByte+tolerance;
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
bool JvsHost::getAllClientReports() {
    bool rs=true;
    int idxFunction=0;
    for ( int idxClient = 0; idxClient < jvsClientCount && rs==true; idxClient++ ) {
        rs=this->writeRawPacket(jvsClient[idxClient]->rawReportRequest, jvsClient[idxClient]->rawReportRequestLen);
        if(getResponseLength())
        {
            for (idxFunction=0; supportedParsingFunctions[idxClient][idxFunction]!=nullptr && rs==true; idxFunction++){
                rs=(this->*supportedParsingFunctions[idxClient][idxFunction])(jvsClient[idxClient]);
            }

            //read SUM
            UART_READ_UNESCAPED();
            TRACE(2, "\n");
        }
    }

    if(!rs)
        TRACE_ARGS_P(1, "ERROR during parsing funcion idx %d !\n", idxFunction);
    return rs;
}

bool JvsHost::getSupportedFeatures(int boardIndex)
{
    char str[] = {CMD_CAPABILITIES};
    this->writePacket(jvsClient[boardIndex]->address, str, 1);
    
    if(getResponseLength())
        return parseSupportedFeatures(jvsClient[boardIndex]);
    else
        return false;
}

void JvsHost::setBulkCommand(int boardIndex)
{
    int idxParsingFunction=0;

    if(boardIndex < jvsClientCount)
    { 
        JvsClient* client=jvsClient[boardIndex];
        char bulkCommands[20];
        int idxCommand=0;

        if (client->supportedFeaturesMask & FEATURE_HAS_PLAYERS)   {
            bulkCommands[idxCommand++] = CMD_READ_DIGITAL; // Read input switch for the x players in 2 bytes format
            bulkCommands[idxCommand++] = (char)client->supportedFeatures.switch_input.Players;
            bulkCommands[idxCommand++] = 0x02; 
            supportedParsingFunctions[boardIndex][idxParsingFunction++]=&JvsHostHelperBase::parseSwitchInput;
        }

        if (client->supportedFeaturesMask & FEATURE_HAS_COINS)     {
            bulkCommands[idxCommand++] = CMD_READ_COINS;   // Read coin values for x slots
            bulkCommands[idxCommand++] = (char)client->supportedFeatures.coin_input.Slots;
            supportedParsingFunctions[boardIndex][idxParsingFunction++]=&JvsHostHelperBase::parseCoinInput;
        }

        if (client->supportedFeaturesMask & FEATURE_HAS_ANALOG_IN) {
            bulkCommands[idxCommand++] = CMD_READ_ANALOG;   // Read analog values for x channels
            bulkCommands[idxCommand++] = (char)client->supportedFeatures.analog_input.Channels;
            supportedParsingFunctions[boardIndex][idxParsingFunction++]=&JvsHostHelperBase::parseAnalogInput;
        }

        if (client->supportedFeaturesMask & FEATURE_HAS_ROTARY)    {
            //TODO
        }

        if (client->supportedFeaturesMask & FEATURE_HAS_LIGHTGUN)  {
            for(int cp=0; cp < client->supportedFeatures.screen_position_input.Channels; cp++){
                bulkCommands[idxCommand++] = CMD_READ_LIGHTGUN;   // Read lightgun channels
                bulkCommands[idxCommand++] = (char)cp;
                supportedParsingFunctions[boardIndex][idxParsingFunction++]=&JvsHostHelperBase::parseLightgunInputChannel;
            }
        }
        TRACE_ARGS(2, "before buildRawRequestPacket, idxCommand:%d, sizeof(bulkCommands):%d, strlen(bulkCommands):%d\n", idxCommand, sizeof(bulkCommands), strlen(bulkCommands));
        client->rawReportRequestLen=buildRawRequestPacket(client->address, bulkCommands, idxCommand, client->rawReportRequest); 
    }
}

void JvsHost::dumpSupportedFeatures(int boardIndex)
{
#ifdef JVSDEBUG
    JvsClient* client=jvsClient[boardIndex];
    TRACE(1, "Features:\n");
    if (client->supportedFeaturesMask & FEATURE_HAS_PLAYERS){
        TRACE_ARGS_P( 1, " - Players: Number of Players               -> %d\n", jvsClient[boardIndex]->supportedFeatures.switch_input.Players);
        TRACE_ARGS_P( 1, " - Players: Number of buttons per player    -> %d\n", jvsClient[boardIndex]->supportedFeatures.switch_input.ButtonsPerPlayer);}
    if (client->supportedFeaturesMask & FEATURE_HAS_COINS)
        TRACE_ARGS_P( 1, " - Coins: Number of slots                   -> %d\n", jvsClient[boardIndex]->supportedFeatures.coin_input.Slots);
    if (client->supportedFeaturesMask & FEATURE_HAS_ANALOG_IN){
        TRACE_ARGS_P( 1, " - Analog input: Number of channels         -> %d\n", jvsClient[boardIndex]->supportedFeatures.analog_input.Channels);
        TRACE_ARGS_P( 1, " - Analog input: Bits per channel           -> %d\n", jvsClient[boardIndex]->supportedFeatures.analog_input.Bits);}
    if (client->supportedFeaturesMask & FEATURE_HAS_ROTARY)
        TRACE_ARGS_P( 1, " - Rotary:Number of rotary encoders         -> %d\n", jvsClient[boardIndex]->supportedFeatures.rotary_input.Channels);
    if (client->supportedFeaturesMask & FEATURE_HAS_LIGHTGUN){
        TRACE_ARGS_P( 1, " - Lightgun: Number of channels             -> %d\n", jvsClient[boardIndex]->supportedFeatures.screen_position_input.Channels);
        TRACE_ARGS_P( 1, " - Lightgun: Number of effective bits for X -> %d\n", jvsClient[boardIndex]->supportedFeatures.screen_position_input.Xbits);
        TRACE_ARGS_P( 1, " - Lightgun: Number of effective bits for Y -> %d\n", jvsClient[boardIndex]->supportedFeatures.screen_position_input.Ybits);}
    if (client->supportedFeaturesMask & FEATURE_HAS_CARD)        
        TRACE_ARGS_P( 1, " - Card System: Number of card slots        -> %d\n", jvsClient[boardIndex]->supportedFeatures.card_system_output.Slots);
    if (client->supportedFeaturesMask & FEATURE_HAS_HOPPER)
        TRACE_ARGS_P( 1, " - Medal hopper: Number of medal hoppers    -> %d\n", jvsClient[boardIndex]->supportedFeatures.medal_hopper_output.Channels);
    if (client->supportedFeaturesMask & FEATURE_HAS_GENERAL_PURPOSE_OUT)
        TRACE_ARGS_P( 1, " - General purpose output: Number of slots  -> %d\n", jvsClient[boardIndex]->supportedFeatures.general_purpose_output.Slots);
    if (client->supportedFeaturesMask & FEATURE_HAS_ANALOG_OUT)    
        TRACE_ARGS_P( 1, " - Analog output: Number of channels        -> %d\n", jvsClient[boardIndex]->supportedFeatures.analog_output.Channels);
    if (client->supportedFeaturesMask & FEATURE_HAS_DISPLAY){    
        TRACE_ARGS_P( 1, " - Display output: width                    -> %d\n", jvsClient[boardIndex]->supportedFeatures.character_output.width);
        TRACE_ARGS_P( 1, " - Display output: height                   -> %d\n", jvsClient[boardIndex]->supportedFeatures.character_output.height);
        TRACE_ARGS_P( 1, " - Display output: type                     -> %d\n", jvsClient[boardIndex]->supportedFeatures.character_output.type);}
    if (client->supportedFeaturesMask & FEATURE_HAS_BACKUP)        
        TRACE_ARGS_P( 1, " - Backup: has data                         -> %d\n", jvsClient[boardIndex]->supportedFeatures.backup_output.has_backup);

    //Wait for the serial to send all the data
    delay(500);
#endif    
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
                TRACE_P(1, "Warning, non-standard input/output function returned by the IO Board!\n");
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

//JVS Commands layout:
//  - SYNC
//  - Node No:  Destination node Nbr, 0 is for the host
//  - Lengh:    nbr of following data bytes including the SUM byte
//  - byte 1:   data
//  - byte 2:   data
//  - etc.
//  - SUM:      CheckSum on all bytes in packet -SYNC -SUM modulo 256
bool JvsHost::sendCommand(char destination, char requestData[], int requestSize, char responseData[], int responseSize) {

    this->writePacket(destination, requestData, requestSize);

    //remember, here length is remaining bytes - 1 byte (requestStatus is already consumed by the method)
    responseSize = getResponseLength();
    if(!responseSize)
        return false;

    // Result contains payload without length & checksum
    // Result will be stored in the heap for later use
    // Result will not contain the SUM but null terminator
    for (int counter=0; counter < responseSize; counter++) {
        UART_READ_UNESCAPED(); 
        responseData[counter] = incomingByte;
    }
    
    responseData[responseSize-1]='\0';

    TRACE(2, "\n\n");
    
    return true;
}

/* This function:
     - waits for the reply packet
     - consumes the first byte of Payload to verify the request status 
     - returns 0 if request status is in error or returns the remaining bytes to read */
int JvsHost::getResponseLength()
{
    int length=0;

    // E0 SYNC
    // 00 Address, 00 is master
    // XX Length
    TRACE_P(2, "Wait UART sync (0xE0)\n");
    UART_SEEK(0XE0); // wait for sync
    if(errorTimeout) return 0;

    TRACE_P(2, "Check if for Host (0x00)\n");
    UART_SEEK(0); // Check if response packet is for host
    if(errorTimeout) return 0;

    length = this->getByte();
    TRACE_ARGS_P(2, " -> Received: E0 00 %02X", length); 

    if(checkRequestStatus(this->getByte()))
        return length - 1;
    else
        return 0;
}

