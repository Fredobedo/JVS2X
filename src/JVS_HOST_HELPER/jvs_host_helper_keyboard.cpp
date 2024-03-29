#ifdef JVS2KEYBOARD
#include <Arduino.h>
#include "JVS_HOST_HELPER/jvs_host_helper_keyboard.h"
#include "JVS_HOST/jvs_host_config.h"
#include "JVS_UART/jvs_uart.h"
#include "USB_KEYBOARD/jvs_keyboard_config.h"

#define BETWEEN(value, min, max) (value < max && value > min)

JvsHostHelperKeyboard::JvsHostHelperKeyboard(HardwareSerial& serial): 
    JvsHostHelperBase(serial) {
        for(int cp=0; cp < (int)(sizeof(shiftkeys)/sizeof(shiftkeys[0])); cp++)
            nbrOfWaitCycle[cp]=shiftkeys[cp].waitCycle;
    }

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

            if(incomingByte > client->Slot1CoinValue){
                keyPressAndRelease(configKeyboard[client->address-1][0], CONTROLLER_P1_BUTTON_COIN);
                client->Slot1CoinValue = incomingByte;
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

            if(incomingByte > client->Slot2CoinValue){
                keyPressAndRelease(configKeyboard[client->address-1][1], CONTROLLER_P2_BUTTON_COIN);
                client->Slot2CoinValue = incomingByte;
            }
        }
    }
    return true;
}

void JvsHostHelperKeyboard::setKeyState(usb_keyboard_class* keyboard, uint16_t KeyCode, bool State)
{
    if(keyboard && State)
        keyboard->press(KeyCode);
    else
        keyboard->release(KeyCode);
}

void JvsHostHelperKeyboard::keyPressAndRelease(usb_keyboard_class* keyboard, uint16_t KeyCode)
{
    keyboard->press(KeyCode);
    keyboard->send_now();
    delay(20);
    keyboard->release(KeyCode);
    keyboard->send_now();
}

bool JvsHostHelperKeyboard::parseSwitchInput(JvsClient* client) 
{
    if (client->supportedFeaturesMask & FEATURE_HAS_PLAYERS)
    {
        /* Report Code for first command SWINP */
        UART_READ_UNESCAPED();
        if(checkReportCode(incomingByte)!=REPORT_CODE_NORMAL)
            return false;

        inputForShiftKeys = 0LU;

        /* Tilt & Test buttons */ 
        UART_READ_UNESCAPED();

        usb_keyboard_class* usb_keyboard=configKeyboard[client->address-1][0];
        if(configKeyboard[client->address-1][0])
            setKeyState(usb_keyboard, CONTROLLER_BUTTON_TEST, BTN_GENERAL_TEST==(incomingByte & BTN_GENERAL_TEST));

        /* Emplement 2 players max in keyboard mode for now, don't know if anybody is intersted in more?  */
        //Player 1
        usb_keyboard=configKeyboard[client->address-1][0];
        if(!usb_keyboard){
                uartReadMultipleUnescaped(2);
        }
        else{
            /* First byte switch player 1 */
            UART_READ_UNESCAPED();

            /* Save byte for configurable shiftKeys */
            inputForShiftKeys =(unsigned long)incomingByte;
            // --- Here are some non configurable Shift keys for player 1 ----------------------------
            //START + Button 1 + Button 2 -> Restart Teensy
            if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            {
                TRACE_P(1,"\nSTART + Button 1 + Button 2 -> Restart Teensy!\n");
                requestReboot=true;
            }

            //Start button is triggered on button_up here, not button_down !
            // if P1 start was previously pressed but not anymore and it was not in the scope of a shift key, then we send the "start"
            else if(previous_controller_p1_start && (BTN_PLAYER_START!=(incomingByte & BTN_PLAYER_START))){
                previous_controller_p1_start=false;
                if(!previous_shiftkey_used)
                    keyPressAndRelease(usb_keyboard, CONTROLLER_P1_START);
            }
            else if((BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            {
                if(!previous_controller_p1_start){
                    previous_controller_p1_start=true;
                    previous_shiftkey_used=false;
                }
            }
            else{
                //Other button combinations
                setKeyState(usb_keyboard, CONTROLLER_P1_BUTTON_1, BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1));
                setKeyState(usb_keyboard, CONTROLLER_P1_BUTTON_2, BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2));
                setKeyState(usb_keyboard, CONTROLLER_P1_SERVICE, BTN_PLAYER_SERVICE==(incomingByte & BTN_PLAYER_SERVICE));                  
                setKeyState(usb_keyboard, CONTROLLER_P1_DOWN,  BTN_PLAYER_DOWN==(incomingByte & BTN_PLAYER_DOWN));
                setKeyState(usb_keyboard, CONTROLLER_P1_RIGHT, BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT));
                setKeyState(usb_keyboard, CONTROLLER_P1_LEFT,  BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT));
                setKeyState(usb_keyboard, CONTROLLER_P1_UP,    BTN_PLAYER_UP==(incomingByte & BTN_PLAYER_UP));
            }

            /* second byte switch player 1 */
            UART_READ_UNESCAPED();
            
            /* Save byte for configurable shiftKeys */
            inputForShiftKeys |=((unsigned long)incomingByte) << 8;

            setKeyState(usb_keyboard, CONTROLLER_P1_BUTTON_3, BTN_PLAYER_PUSH3==(incomingByte & BTN_PLAYER_PUSH3));
            setKeyState(usb_keyboard, CONTROLLER_P1_BUTTON_4, BTN_PLAYER_PUSH4==(incomingByte & BTN_PLAYER_PUSH4));
            setKeyState(usb_keyboard, CONTROLLER_P1_BUTTON_5, BTN_PLAYER_PUSH5==(incomingByte & BTN_PLAYER_PUSH5));
            setKeyState(usb_keyboard, CONTROLLER_P1_BUTTON_6, BTN_PLAYER_PUSH6==(incomingByte & BTN_PLAYER_PUSH6));
            setKeyState(usb_keyboard, CONTROLLER_P1_BUTTON_7, BTN_PLAYER_PUSH7==(incomingByte & BTN_PLAYER_PUSH7));
            setKeyState(usb_keyboard, CONTROLLER_P1_BUTTON_8, BTN_PLAYER_PUSH8==(incomingByte & BTN_PLAYER_PUSH8));
        }

        //Player 2
        usb_keyboard=configKeyboard[client->address-1][1];
        if(!usb_keyboard || client->supportedFeatures.switch_input.Players==1){
                uartReadMultipleUnescaped(2);
        }
        else{
            /* First byte switch player 2 */
            UART_READ_UNESCAPED();
            
            /* Save byte for configurable shiftKeys */
            inputForShiftKeys |=(((unsigned long)incomingByte) << 16);

            // --- Here are some non configurable Shift keys for player 2 ----------------------------
            //START + Button 1 + Button 2 -> Restart Teensy
            if((BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1)) && (BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2)) && (BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            {
                TRACE_P(1,"\nSTART + Button 1 + Button 2 -> Restart Teensy!\n");
                requestReboot=true;
            }
 
            //Start button is triggered on button_up here, not button_down !
            // if P2 start was previously pressed but not anymore and it was not in the scope of a shift key, then we send the "start"
            else if(previous_controller_p2_start && (BTN_PLAYER_START!=(incomingByte & BTN_PLAYER_START))){
                previous_controller_p2_start=false;
                if(!previous_shiftkey_used)
                    keyPressAndRelease(usb_keyboard, CONTROLLER_P2_START);
            }
            else if((BTN_PLAYER_START==(incomingByte & BTN_PLAYER_START)))
            {
                if(!previous_controller_p2_start){
                    previous_controller_p2_start=true;
                    previous_shiftkey_used=false;
                }
            }
            else{
                setKeyState(usb_keyboard, CONTROLLER_P2_BUTTON_1, BTN_PLAYER_PUSH1==(incomingByte & BTN_PLAYER_PUSH1));
                setKeyState(usb_keyboard, CONTROLLER_P2_BUTTON_2, BTN_PLAYER_PUSH2==(incomingByte & BTN_PLAYER_PUSH2));
                setKeyState(usb_keyboard, CONTROLLER_P2_SERVICE, BTN_PLAYER_SERVICE==(incomingByte & BTN_PLAYER_SERVICE));                
                setKeyState(usb_keyboard, CONTROLLER_P2_DOWN,  BTN_PLAYER_DOWN==(incomingByte & BTN_PLAYER_DOWN));
                setKeyState(usb_keyboard, CONTROLLER_P2_RIGHT, BTN_PLAYER_RIGHT==(incomingByte & BTN_PLAYER_RIGHT));
                setKeyState(usb_keyboard, CONTROLLER_P2_LEFT,  BTN_PLAYER_LEFT==(incomingByte & BTN_PLAYER_LEFT));
                setKeyState(usb_keyboard, CONTROLLER_P2_UP,    BTN_PLAYER_UP==(incomingByte & BTN_PLAYER_UP));
            }

            /* second byte switch player 2 */
            UART_READ_UNESCAPED();

            /* Save byte for configurable shiftKeys */
            inputForShiftKeys |=((unsigned long)incomingByte) << 24;

            setKeyState(usb_keyboard, CONTROLLER_P2_BUTTON_3, BTN_PLAYER_PUSH3==(incomingByte & BTN_PLAYER_PUSH3));
            setKeyState(usb_keyboard, CONTROLLER_P2_BUTTON_4, BTN_PLAYER_PUSH4==(incomingByte & BTN_PLAYER_PUSH4));
            setKeyState(usb_keyboard, CONTROLLER_P2_BUTTON_5, BTN_PLAYER_PUSH5==(incomingByte & BTN_PLAYER_PUSH5));
            setKeyState(usb_keyboard, CONTROLLER_P2_BUTTON_6, BTN_PLAYER_PUSH6==(incomingByte & BTN_PLAYER_PUSH6));
            setKeyState(usb_keyboard, CONTROLLER_P2_BUTTON_7, BTN_PLAYER_PUSH7==(incomingByte & BTN_PLAYER_PUSH7));
            setKeyState(usb_keyboard, CONTROLLER_P2_BUTTON_8, BTN_PLAYER_PUSH8==(incomingByte & BTN_PLAYER_PUSH8));
        }

        //Let the time for the USB host to retrieve the report before changing it again...
        //I should place it somewhere else :(
        delay(1);

#ifndef SHIFTKEY_DISABLED
        /* configurable shift keys management here */
        for(int cp=0; cp < (int)(sizeof(shiftkeys)/sizeof(shiftkeys[0])); cp++)
            setShiftKeyState(usb_keyboard, shiftkeys[cp].value, shiftkeys[cp].mask==(inputForShiftKeys & shiftkeys[cp].mask), cp);
#endif

    }
    return true;
}

void JvsHostHelperKeyboard::setShiftKeyState(usb_keyboard_class* keyboard, uint16_t KeyCode, bool State, int idxShiftKey)
{
    if(State){
        if(!nbrOfWaitCycle[idxShiftKey])      // Send mapped key
            setKeyState(keyboard, KeyCode, State);
        else
            nbrOfWaitCycle[idxShiftKey]--;

        previous_shiftkey_used=true;
    }
    else{
        setKeyState(keyboard, KeyCode, State);
        nbrOfWaitCycle[idxShiftKey]=shiftkeys[idxShiftKey].waitCycle;
    }
}

void JvsHostHelperKeyboard::ForwardReportsToUSBDevice() {
    if(memcmp(&usb_keyboard_P1_previous_state, &keyboard_P1_state, sizeof(keyboard_P1_state))){
        // Send keydown keys
        Keyboard_P1.send_now();

        // Storing data keyboard 1 for next interrupt-in report
        memcpy(usb_keyboard_P1_previous_state, keyboard_P1_state, sizeof(keyboard_P1_state));        
    }


    if(memcmp(&usb_keyboard_P2_previous_state, &keyboard_P2_state, sizeof(keyboard_P2_state))){
        // Send keydown keys
        Keyboard_P2.send_now();

        // Storing data keyboard 1 for next interrupt-in report
        memcpy(usb_keyboard_P2_previous_state, keyboard_P2_state, sizeof(keyboard_P2_state));        
    }
}


void JvsHostHelperKeyboard::DoTest()
{
    keyPressAndRelease(configKeyboard[0][0], KEY_SPACE);
}


#endif

