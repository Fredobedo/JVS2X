#ifdef JVS2KEYBOARD
#include <Arduino.h>
#include "JVS_HOST_HELPER/jvs_host_helper_keyboard.h"
#include "USB_KEYBOARD/jvs_keyboard_config.h"
#include "JVS_HOST/jvs_host_config.h"
#include "JVS_UART/jvs_uart.h"

#define BETWEEN(value, min, max) (value < max && value > min)

JvsHostHelperKeyboard::JvsHostHelperKeyboard(HardwareSerial& serial): 
    JvsHostHelperBase(serial) { }

bool JvsHostHelperKeyboard::parseLightgunInputChannel(JvsClient* client) 
{
    //NOT MAPPED TO SOMETHING FOR NOW
    return true;
}

bool JvsHostHelperKeyboard::parseAnalogInput(JvsClient* client) 
{
    //NOT MAPPED TO SOMETHING FOR NOW
    return true;
}

bool JvsHostHelperKeyboard::parseCoinInput(JvsClient* client) 
{
    if (client->supportedFeaturesMask & FEATURE_HAS_COINS) 
    {
        /* Report Code for first command COININP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        /* 2 next bytes for player 1 */
        if(!configKeyboard[client->address-1][0]){
            uartReadMultipleUnescaped(2);
        }
        else{
            /* Slot x status on 2 first bits (on the left) */
            UART_READ_UNESCAPED();
            
            /* Slot x coin */
            UART_READ_UNESCAPED();
            if (incomingByte > client->initialSlot1CoinValue)
            {
                //This is not a correct implementation, but let's start with this 
                client->initialSlot1CoinValue = incomingByte;
                if(client->initialSlot1CoinValue>0){
                    setKeytate(configKeyboard[client->address-1][0], CONTROLLER_P1_BUTTON_COIN, 1);
                }
            }
        }

        /* 2 next bytes for player 2 */
        if(!configKeyboard[client->address-1][1] || client->supportedFeatures.coin_input.Slots==1){
            uartReadMultipleUnescaped(2);
        }
        else{
            /* Slot x status on 2 first bits (on the left) */
            UART_READ_UNESCAPED();
            
            /* Slot x coin */
            UART_READ_UNESCAPED();
            if (incomingByte > client->initialSlot2CoinValue)
            {
                //This is not a correct implementation, but let's start with this 
                client->initialSlot2CoinValue = incomingByte;
                if(client->initialSlot2CoinValue>0){
                    setKeytate(configKeyboard[client->address-1][1], CONTROLLER_P2_BUTTON_COIN, 1);
                }
            }
        }
    }
    return true;
}

void JvsHostHelperKeyboard::setKeytate(usb_keyboard_class* keyboard, uint16_t KeyCode, bool State)
{
    //TRACE_ARGS_P(2,"setKeytate, KeyCode=%d, state=%d\n", KeyCode, State);
    if(keyboard){
        if(State)
            keyboard->press(KeyCode);
        else
            keyboard->release(KeyCode);
    }
}

bool JvsHostHelperKeyboard::parseSwitchInput(JvsClient* client) 
{
    if (client->supportedFeaturesMask & FEATURE_HAS_PLAYERS)
    {
        /* Report Code for first command SWINP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        /* Tilt & Test buttons */ 
        UART_READ_UNESCAPED();

        usb_keyboard_class* usb_keyboard=configKeyboard[client->address-1][0];
        if(configKeyboard[client->address-1][0])
            setKeytate(usb_keyboard, CONTROLLER_BUTTON_TEST, BTN_GENERAL_TEST==(incomingByte & BTN_GENERAL_TEST));

        /* Emplement 2 players max in keyboard mode for now, don't know if anybody is intersted in more?  */
        //Player 1
        usb_keyboard=configKeyboard[client->address-1][0];
        if(!usb_keyboard){
                uartReadMultipleUnescaped(2);
        }
        else{
            setKeytate(usb_keyboard, CONTROLLER_BUTTON_TEST, 0);
            setKeytate(usb_keyboard, CONTROLLER_P1_START, 0);
            
            /* First byte switch player 1 */
            UART_READ_UNESCAPED();

            /* Save byte for configurable shiftKeys */
            inputForShiftKeys |=incomingByte;

            // --- Here are some non configurable Shift keys for player 1 ----------------------------
            //START + Button 1 + Button 2 -> Restart Teensy
            if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            {
                TRACE_P(1,"\nSTART + Button 1 + Button 2 -> Restart Teensy!\n");
                requestReboot=true;
            }
            //START + Button 2 -> Select
            else if((BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START))){
                setKeytate(usb_keyboard, CONTROLLER_P1_BUTTON_2, 0);
                setKeytate(usb_keyboard, CONTROLLER_BUTTON_TEST, 1);
            }
            //Start
            else if((BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
                setKeytate(usb_keyboard, CONTROLLER_P1_START, 1);
            
            else{
                //Other button combinations
                setKeytate(usb_keyboard, CONTROLLER_P1_BUTTON_1, BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1));
                setKeytate(usb_keyboard, CONTROLLER_P1_BUTTON_2, BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2));
                
                setKeytate(usb_keyboard, CONTROLLER_P1_DOWN,  BTN_PLAYER_DOWN==(incomingByte & BTN_PLAYER_DOWN));
                setKeytate(usb_keyboard, CONTROLLER_P1_RIGHT, BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT));
                setKeytate(usb_keyboard, CONTROLLER_P1_LEFT,  BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT));
                setKeytate(usb_keyboard, CONTROLLER_P1_UP,    BTN_PLAYER_UP==(incomingByte & BTN_PLAYER_UP));
            }

            /* second byte switch player 1 */
            UART_READ_UNESCAPED();
            
            /* Save byte for configurable shiftKeys */
            inputForShiftKeys |=(incomingByte << 8);

            setKeytate(usb_keyboard, CONTROLLER_P1_BUTTON_3, BTN_PLAYER_PUSH3==(incomingByte & BTN_PLAYER_PUSH3));
            setKeytate(usb_keyboard, CONTROLLER_P1_BUTTON_4, BTN_PLAYER_PUSH4==(incomingByte & BTN_PLAYER_PUSH4));
            setKeytate(usb_keyboard, CONTROLLER_P1_BUTTON_5, BTN_PLAYER_PUSH5==(incomingByte & BTN_PLAYER_PUSH5));
            setKeytate(usb_keyboard, CONTROLLER_P1_BUTTON_6, BTN_PLAYER_PUSH6==(incomingByte & BTN_PLAYER_PUSH6));
            setKeytate(usb_keyboard, CONTROLLER_P1_BUTTON_7, BTN_PLAYER_PUSH7==(incomingByte & BTN_PLAYER_PUSH7));
            setKeytate(usb_keyboard, CONTROLLER_P1_BUTTON_8, BTN_PLAYER_PUSH8==(incomingByte & BTN_PLAYER_PUSH8));
        }

        //Player 2
        usb_keyboard=configKeyboard[client->address-1][1];
        if(!usb_keyboard || client->supportedFeatures.switch_input.Players==1){
                uartReadMultipleUnescaped(2);
        }
        else{
            setKeytate(usb_keyboard, CONTROLLER_BUTTON_TEST ,0);
            setKeytate(usb_keyboard, CONTROLLER_P2_START ,0);
            
            /* First byte switch player 1 */
            UART_READ_UNESCAPED();
            
            /* Save byte for configurable shiftKeys */
            inputForShiftKeys |=((unsigned long)incomingByte << 16);

            // --- Here are some non configurable Shift keys for player 2 ----------------------------
            //START + Button 1 + Button 2 -> Restart Teensy
            if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            {
                TRACE_P(1,"\nSTART + Button 1 + Button 2 -> Restart Teensy!\n");
                requestReboot=true;
            }
            //START + Button 2 -> Select
            else if((BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START))){
                setKeytate(usb_keyboard, CONTROLLER_P2_BUTTON_2, 0);
                setKeytate(usb_keyboard, CONTROLLER_BUTTON_TEST, 1);
            }
            //Start
            else if((BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
                setKeytate(usb_keyboard, CONTROLLER_P2_START, 1);
            
            else{
                //Other button combinations
                setKeytate(usb_keyboard, CONTROLLER_P2_BUTTON_1, BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1));
                setKeytate(usb_keyboard, CONTROLLER_P2_BUTTON_2, BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2));
                
                setKeytate(usb_keyboard, CONTROLLER_P2_DOWN,  BTN_PLAYER_DOWN==(incomingByte & BTN_PLAYER_DOWN));
                setKeytate(usb_keyboard, CONTROLLER_P2_RIGHT, BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT));
                setKeytate(usb_keyboard, CONTROLLER_P2_LEFT,  BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT));
                setKeytate(usb_keyboard, CONTROLLER_P2_UP,    BTN_PLAYER_UP==(incomingByte & BTN_PLAYER_UP));
            }

            /* second byte switch player 1 */
            UART_READ_UNESCAPED();

            /* Save byte for configurable shiftKeys */
            inputForShiftKeys |=((unsigned long)incomingByte << 24);

            setKeytate(usb_keyboard, CONTROLLER_P2_BUTTON_3, BTN_PLAYER_PUSH3==(incomingByte & BTN_PLAYER_PUSH3));
            setKeytate(usb_keyboard, CONTROLLER_P2_BUTTON_4, BTN_PLAYER_PUSH4==(incomingByte & BTN_PLAYER_PUSH4));
            setKeytate(usb_keyboard, CONTROLLER_P2_BUTTON_5, BTN_PLAYER_PUSH5==(incomingByte & BTN_PLAYER_PUSH5));
            setKeytate(usb_keyboard, CONTROLLER_P2_BUTTON_6, BTN_PLAYER_PUSH6==(incomingByte & BTN_PLAYER_PUSH6));
            setKeytate(usb_keyboard, CONTROLLER_P2_BUTTON_7, BTN_PLAYER_PUSH7==(incomingByte & BTN_PLAYER_PUSH7));
            setKeytate(usb_keyboard, CONTROLLER_P2_BUTTON_8, BTN_PLAYER_PUSH8==(incomingByte & BTN_PLAYER_PUSH8));
        }

        /* configurable shift keys management here */
        for(int cp=0; cp < (int)(sizeof(shiftkeys)/sizeof(shiftkeys[0])); cp++)
            setKeytate(usb_keyboard, shiftkeys[cp].value, shiftkeys[cp].mask==(inputForShiftKeys & shiftkeys[cp].mask));

    }
    return true;
}


bool JvsHostHelperKeyboard::ForwardReportsToUSBDevice() {
    if(memcmp(&usb_keyboard_P1_previous_state, &keyboard_P1_state, sizeof(keyboard_P1_state))){
        TRACE_P( 2, "Storing data keyboard 1 for next interrupt-in report\n");
        Keyboard_P1.send_now();
        memcpy(usb_keyboard_P1_previous_state, keyboard_P1_state, sizeof(keyboard_P1_state));
    }

    if(memcmp(&usb_keyboard_P2_previous_state, &keyboard_P2_state, sizeof(keyboard_P2_state))){
        TRACE_P( 2, "Storing data keyboard 2 for next interrupt-in report\n");
        Keyboard_P2.send_now();
        memcpy(usb_keyboard_P2_previous_state, keyboard_P2_state, sizeof(keyboard_P2_state));
    }
    return true;
}

#endif

