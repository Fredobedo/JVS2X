#include <Arduino.h>

#include "JVS/jvs_report_parser_controller.h"
#include "JVS/jvs_controller_config.h"
#include "JVS/jvs_uart.h"

#define BETWEEN(value, min, max) (value < max && value > min)

JvsReportParser::JvsReportParser(HardwareSerial& serial): 
    JvsReportParserCommon(serial)
{
    
}

bool JvsReportParser::parseLightgunInputChannel(JvsClient* client)
{
    if (client->supportedFeaturesMask & FEATURE_HAS_LIGHTGUN)
    {
        /* Report Code for command SCRPOSINP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        // For now, map all supported analog channels based on nbr of players
        // not sure it's the best...
        for(int channel=0;channel < client->supportedFeatures.screen_position_input.Channels; channel++)
        {
            UART_READ_UNESCAPED(); // MSB
            //if(BETWEEN(incomingByte, client->analogFuzz[channel][0], client->analogFuzz[channel][1])){
                if(channel==1)       CONTROLLER_ANALOG_1  = incomingByte;
                else if(channel==2)  CONTROLLER_ANALOG_2  = incomingByte;                    
            //}
            UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)  
        }
    }
    return true;
}

bool JvsReportParser::parseAnalogInput(JvsClient* client)
{
    if (client->supportedFeaturesMask & FEATURE_HAS_ANALOG_IN)
    {
        /* Report Code for command SCRPOSINP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        // For now, map all supported analog channels based on nbr of players
        // not sure it's the best...
        for(int channel=0;channel < client->supportedFeatures.analog_input.Channels; channel++)
        {
            UART_READ_UNESCAPED(); // MSB
            if(BETWEEN(incomingByte, client->analogFuzz[channel][0], client->analogFuzz[channel][1])){
                if(channel==1)       CONTROLLER_ANALOG_1  = incomingByte;
                else if(channel==2)  CONTROLLER_ANALOG_2  = incomingByte;                    
                else if(channel==3)  CONTROLLER_ANALOG_3  = incomingByte;                    
                else if(channel==4)  CONTROLLER_ANALOG_4  = incomingByte;                    
                else if(channel==5)  CONTROLLER_ANALOG_5  = incomingByte;                    
                else if(channel==6)  CONTROLLER_ANALOG_6  = incomingByte;                    
                else if(channel==7)  CONTROLLER_ANALOG_7  = incomingByte;                    
                else if(channel==8)  CONTROLLER_ANALOG_8  = incomingByte;                    
                else if(channel==9)  CONTROLLER_ANALOG_9  = incomingByte;                    
                else if(channel==10) CONTROLLER_ANALOG_10 = incomingByte; 
            }
            UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)  
        }
    }
    return true;
}

bool JvsReportParser::parseCoinInput(JvsClient* client)
{
    if (client->supportedFeaturesMask & FEATURE_HAS_COINS) 
    {
        /* Report Code for first command COININP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        /* 2 next bytes for player x */
        for(int cp=0;cp < client->supportedFeatures.coin_input.Slots; cp++)
        {
            if(!client->usb_controller[cp]){
                uartReadMultipleUnescaped(2);
            }
            else{
                /* Slot x status on 2 first bits (on the left) */
                UART_READ_UNESCAPED();
                
                /* Slot x coin */
                UART_READ_UNESCAPED();
                if (incomingByte > client->initialSlot1CoinValue)
                {
                    client->initialSlot1CoinValue = incomingByte;
                    if(client->initialSlot1CoinValue>0){
                        gamepad_state_t* usb_controller=client->usb_controller[cp];
                        CONTROLLER_BUTTON_COIN=1;
                    }
                }
            }
        }
    }
    return true;
}

bool JvsReportParser::parseSwitchInput(JvsClient* client)
{
    if (client->supportedFeaturesMask & FEATURE_HAS_PLAYERS)
    {
        /* Report Code for first command SWINP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        /* Tilt & Test buttons */ 
        UART_READ_UNESCAPED();
        gamepad_state_t* usb_controller=client->usb_controller[0];
        CONTROLLER_BUTTON_TEST = (BTN_GENERAL_TEST==(incomingByte & BTN_GENERAL_TEST));

        /* 2 next bytes for player x */
        for(int cp=0;cp < client->supportedFeatures.switch_input.Players; cp++)
            parseSwitchInputPlayer(client->usb_controller[cp]);
    }
    return true;
}

void JvsReportParser::parseSwitchInputPlayer(gamepad_state_t* usb_controller)
{   
    if(!usb_controller){
            uartReadMultipleUnescaped(2);
    }
    else{
        CONTROLLER_START=0;
        CONTROLLER_HOME=0;
        CONTROLLER_BUTTON_TEST=0;

        /* First byte switch player x */
        UART_READ_UNESCAPED();

        //START + Button 1 + Button 2 -> Restart Teensy
        if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            _reboot_Teensyduino_();
        
        //START + Button 1 -> PS Button
        else if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            CONTROLLER_HOME=1;
       
        //START + Button 2 -> Select
        else if((BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            CONTROLLER_BUTTON_TEST=1;
        
        //Start
        else if((BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            CONTROLLER_START=1;
        
        else{
            //Other button combinations
            CONTROLLER_BUTTON_1        = (BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1));
            CONTROLLER_BUTTON_ANALOG_1 = usb_controller->cross_btn * 0xFF;
            CONTROLLER_BUTTON_2        = (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2));
            CONTROLLER_BUTTON_ANALOG_2 = usb_controller->circle_btn * 0xFF;
            
            usb_controller->direction = 8; // Center
            CONTROLLER_LEFT_STICK_X=0x80;
            CONTROLLER_LEFT_STICK_Y=0x80;

            if ((BTN_PLAYER_DOWN==(incomingByte & BTN_PLAYER_DOWN))) {
                usb_controller->direction = 4;
                CONTROLLER_LEFT_STICK_Y=0xFF;
                if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) {
                    usb_controller->direction = 3;
                    CONTROLLER_LEFT_STICK_X=0xFF;
                }
                else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))) {
                    usb_controller->direction = 5;
                    CONTROLLER_LEFT_STICK_X=0x00;
                }
            }
            else {
                if ((BTN_PLAYER_UP==(incomingByte & BTN_PLAYER_UP))) {
                    usb_controller->direction = 0;
                    CONTROLLER_LEFT_STICK_Y=0x00;
                    if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) {
                        usb_controller->direction = 1;
                        CONTROLLER_LEFT_STICK_X=0xFF;
                    }
                    else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))){
                        usb_controller->direction = 7;
                        CONTROLLER_LEFT_STICK_X=0x00;
                    }
                }
                else {
                    if ((BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT))) {
                        usb_controller->direction = 2;
                        CONTROLLER_LEFT_STICK_X=0xFF;
                    }
                    else if ((BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT))) {
                        usb_controller->direction = 6;
                        CONTROLLER_LEFT_STICK_X=0x00;
                    }
                }
            }
        }

        /* second byte switch player x */
        UART_READ_UNESCAPED();
        usb_controller->square_btn    = (BTN_PLAYER_PUSH3==(incomingByte & BTN_PLAYER_PUSH3));
        usb_controller->square_axis   = usb_controller->square_btn * 0xFF;
        usb_controller->triangle_btn  = (BTN_PLAYER_PUSH4==(incomingByte & BTN_PLAYER_PUSH4));
        usb_controller->triangle_axis = usb_controller->triangle_btn * 0xFF;
        usb_controller->l1_btn        = (BTN_PLAYER_PUSH5==(incomingByte & BTN_PLAYER_PUSH5));
        usb_controller->l1_axis       = usb_controller->l1_btn * 0xFF;
        usb_controller->r1_btn        = (BTN_PLAYER_PUSH6==(incomingByte & BTN_PLAYER_PUSH6));
        usb_controller->r1_axis       = usb_controller->r1_btn * 0xFF;
        usb_controller->l2_btn        = (BTN_PLAYER_PUSH7==(incomingByte & BTN_PLAYER_PUSH7));
        usb_controller->r2_btn        = (BTN_PLAYER_PUSH8==(incomingByte & BTN_PLAYER_PUSH8));
    }
}




