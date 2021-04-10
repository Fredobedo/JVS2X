#include <Arduino.h>
#include "JVS_HOST_HELPER/jvs_host_helper_controller.h"
#include "JVS_HOST_HELPER/jvs_controller_config.h"
#include "JVS_HOST/jvs_host_config.h"
#include "JVS_UART/jvs_uart.h"

#define BETWEEN(value, min, max) (value < max && value > min)

JvsHostHelperController::JvsHostHelperController(HardwareSerial& serial): 
    JvsHostHelperBase(serial) { }

bool JvsHostHelperController::parseLightgunInputChannel(JvsClient* client) 
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
            //Should do the same as for analog here
            //if(BETWEEN(incomingByte, client->analogFuzz[channel][0], client->analogFuzz[channel][1])){
                if(channel==1)       CONTROLLER_ANALOG_1  = incomingByte;
                else if(channel==2)  CONTROLLER_ANALOG_2  = incomingByte;                    
            //}
            UART_READ_UNESCAPED(); // LSB (not used here, PS3 analog precision is only 255 wide)  
        }
    }
    return true;
}

bool JvsHostHelperController::parseAnalogInput(JvsClient* client) 
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

bool JvsHostHelperController::parseCoinInput(JvsClient* client) 
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
            if(!configGamepad[client->address-1][cp]){
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
                        usb_controller_state_t* usb_controller=configGamepad[client->address-1][cp];
                        CONTROLLER_BUTTON_COIN=1;
                    }
                }
            }
        }
    }
    return true;
}

bool JvsHostHelperController::parseSwitchInput(JvsClient* client) 
{
    if (client->supportedFeaturesMask & FEATURE_HAS_PLAYERS)
    {
        /* Report Code for first command SWINP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        /* Tilt & Test buttons */ 
        UART_READ_UNESCAPED();

        usb_controller_state_t* usb_controller=configGamepad[client->address-1][0];
        CONTROLLER_BUTTON_TEST = (BTN_GENERAL_TEST==(incomingByte & BTN_GENERAL_TEST));

        /* 2 next bytes for player x */
        for(int cp=0;cp < client->supportedFeatures.switch_input.Players; cp++)
            parseSwitchInputPlayer(configGamepad[client->address-1][cp]);
    }
    return true;
}

void JvsHostHelperController::parseSwitchInputPlayer(usb_controller_state_t* usb_controller) 
{   
    if(!usb_controller){
            uartReadMultipleUnescaped(2);
    }
    else{
        CONTROLLER_HOME=0;
        CONTROLLER_BUTTON_TEST=0;
        CONTROLLER_START=0;
        
        /* First byte switch player x */
        UART_READ_UNESCAPED();

        //START + Button 1 + Button 2 -> Restart Teensy
        if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
        {
            TRACE_P(1,"\nSTART + Button 1 + Button 2 -> Restart Teensy!\n");
            requestReboot=true;
        }
        //START + Button 1 -> PS Button
        else if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START))){
            CONTROLLER_BUTTON_1=0;
            CONTROLLER_HOME=1;
        }
        //START + Button 2 -> Select
        else if((BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START))){
            CONTROLLER_BUTTON_2=0;
            CONTROLLER_BUTTON_TEST=1;
        }
        //Start
        else if((BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            CONTROLLER_START=1;
        
        else{
            //Other button combinations
            CONTROLLER_BUTTON_1        = (BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1));
            CONTROLLER_BUTTON_ANALOG_1 = CONTROLLER_BUTTON_1 * 0xFF;
            CONTROLLER_BUTTON_2        = (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2));
            CONTROLLER_BUTTON_ANALOG_2 = CONTROLLER_BUTTON_2 * 0xFF;
            
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
        CONTROLLER_BUTTON_3        = (BTN_PLAYER_PUSH3==(incomingByte & BTN_PLAYER_PUSH3));
        CONTROLLER_BUTTON_ANALOG_3 = CONTROLLER_BUTTON_3 * 0xFF;
        CONTROLLER_BUTTON_4        = (BTN_PLAYER_PUSH4==(incomingByte & BTN_PLAYER_PUSH4));
        CONTROLLER_BUTTON_ANALOG_4 = CONTROLLER_BUTTON_4 * 0xFF;
        CONTROLLER_BUTTON_5        = (BTN_PLAYER_PUSH5==(incomingByte & BTN_PLAYER_PUSH5));
        CONTROLLER_BUTTON_ANALOG_5 = CONTROLLER_BUTTON_5 * 0xFF;
        CONTROLLER_BUTTON_6        = (BTN_PLAYER_PUSH6==(incomingByte & BTN_PLAYER_PUSH6));
        CONTROLLER_BUTTON_ANALOG_6 = CONTROLLER_BUTTON_6 * 0xFF;
        CONTROLLER_BUTTON_7        = (BTN_PLAYER_PUSH7==(incomingByte & BTN_PLAYER_PUSH7));
        CONTROLLER_BUTTON_8        = (BTN_PLAYER_PUSH8==(incomingByte & BTN_PLAYER_PUSH8));
    }
}

bool JvsHostHelperController::ForwardReportsToUSBDevice() {
    if(memcmp(&usb_controller_1_previous_state, &usb_controller_p1, sizeof(usb_controller_state_t))){
        TRACE_P( 2, "Storing data controller 1 for next interrupt-in report\n");
        usbControllerP1SendReport();
        usb_controller_1_previous_state=usb_controller_p1;
    }
    
    if(memcmp(&usb_controller_2_previous_state, &usb_controller_p2, sizeof(usb_controller_state_t))){
        TRACE_P( 2, "Storing data controller 2 for next interrupt-in report\n");
        usbControllerP2SendReport();
        usb_controller_2_previous_state=usb_controller_p2;
    }

    return true;
}


