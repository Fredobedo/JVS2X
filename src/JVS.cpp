/*
   JVS.cpp
    Created on: 14/apr/2014
        Author: k4roshi

        p1 start works as shift, when pressed with the following buttons it causes different keypresses:
        button 1: coin (presses keyb 5)
        J1 right: Tab
        J1 down: P (pause)
        J1 left: enter
        P2: esc

        Here's the binary build:
        https://dl.dropboxusercontent.com/u/63724693/arcade/JVSy-1.0.hex
        I also managed to read analog inputs, but have no way of "calibrating" them.
        This debug build outputs to the serial console the binary readings it receives from the analog ports. If anybody wants to flash this and report back to me i can make them work in little time. This doesn't work as a joystick input, so don't flash unless you know what you're doing.
        https://dl.dropboxusercontent.com/u/63724693/arcade/JVSy-analog.hex

        I studied a bit the lightgun controls and they should be easy to implement, but again, i would need someone to help me test it.


        PRB Keyboard 6 keys only & sense line:
        https://www.neo-arcadia.com/forum/viewtopic.php?t=53344
*/

#include "Arduino.h"
#include "JVS.h"
#include "constants.h"

#include "USB_HID/USB_PS3/usb_ps3.h"
#include "print.h"

bool analog;
bool mirroring;
bool full_joy;

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
    char str[] = { (char)CMD_RESET, (char)CMD_RESET_ARG };
    this->write_packet(BROADCAST, str, 2);
    delay(ASSIGN_DELAY);
    if (analog)
        TRACE("Analogique active");
    if (mirroring)
        TRACE("Mirroring active");
    if (full_joy)
        TRACE("Full Joystick active");
}

void JVS::assign(int attempt) {
    char str[] = { (char)CMD_ASSIGN_ADDR, (char)attempt };
    this->cmd(BROADCAST, str, 2);
    TRACE("ADDR");
}

void JVS::init(int board) {
    TRACE("ADDR\n"); 
    char str[] = { (char)CMD_ASSIGN_ADDR, (char)board };
    this->cmd(BROADCAST, str, 2);
    TRACE("REQ\n"); 
    char str1[] = { (char)CMD_REQUEST_ID};
    this->cmd(board, str1, 1);
    TRACE("CMD\n"); 
    char str2[] = { (char)CMD_COMMAND_VERSION };
    this->cmd(board, str2, 1);
    TRACE("JVS\n"); 
    char str3[] = { (char)CMD_JVS_VERSION };
    this->cmd(board, str3, 1);
    TRACE("CMS\n"); 
    char str4[] = { (char)CMD_COMMS_VERSION };
    this->cmd(board, str4, 1);
    TRACE("CAP\n"); 
    char str5[] = { (char)CMD_CAPABILITIES };
    this->cmd(board, str5, 1);
    //print(PSTR("Init finshed, turn off led\n")); usb_debug_flush_output();
    //digitalWrite(11, LOW);
    initialized = true;
}


//JVS always sends:
//  - SYNC
//  - Lengh (nbr of following data bytes
//  - byte 1
//  - byte 2
//  - etc.
//  - CheckSum
void JVS::switches(int board) {
    char str[] = { CMD_READ_DIGITAL, 0x02, 0x02, CMD_READ_COINS, 0x02, CMD_READ_ANALOG, 0x04 };
    //char str[ ] = { 0x20, 0x02, 0x02, 0x21, 0x02, 0x22, 0x08};
    this->write_packet(board, str, sizeof str);
    char incomingByte;
    
    while (!_Uart.available()) {}
    while (_Uart.read() != 0xE0) {} // wait for sync
    while (_Uart.read() != 0) {}    // only if it's for me
    while (!_Uart.available()) {}   // wait for length

    int length = _Uart.read();
    int counter = 0;

    //int X_player1 = 512;
    //int Y_player1 = 512;
    //int X_player2 = 512;
    //int Y_player2 = 512;

    int coin1 = 0;
    int coin2 = 0;
    bool old_shift = shift_mode;


    gamepad_P1_state.select_btn = 0;
    gamepad_P2_state.select_btn = 0;

    while (counter < length) {
        while (!_Uart.available()) {
        }
        incomingByte = _Uart.read();

        TRACE(" ");
        phex(incomingByte);
        TRACE(" ");
        phex16(incomingByte);

        if ((unsigned)(int)incomingByte == 0xFFFFFFD0) {
            int escapeByte = _Uart.read();
            if (escapeByte == 0xDF)
                incomingByte = 0xE0;
        }
        switch (counter) {
            // fourth byte (first three bytes are sync and
        case 2:
            //TEST PRESSED ON JVS BOARD
            if bitRead(incomingByte, 7)
                //Keyboard.pressKey(KEY_F2);
                gamepad_P1_state.ps_btn = 1;
            else
                //Keyboard.releaseKey(KEY_F2);
                gamepad_P1_state.ps_btn = 0;
            break;

        case 3:
            // player1 board1
            shift_mode = bitRead(incomingByte, 7);

            if (shift_mode != old_shift) {
                if (shift_mode == 0 && !pressed_smth) {
                    //never pressed anything, wants start
                    //Joystick.button(9, 1);
                    gamepad_P1_state.select_btn = 1;
                }
                else
                    pressed_smth = false;
            }
            if (shift_mode) {
                //print("shiftmode on");
                if (bitRead(incomingByte, 1)) {
                    pressed_smth = true;
                }

                if (bitRead(incomingByte, 2)) {
                    pressed_smth = true;
                }

                if (bitRead(incomingByte, 3)) {
                    pressed_smth = true;
                }

                if (bitRead(incomingByte, 4)) {
                    pressed_smth = true;
                }
            }
            else {
                //BUTTON 1 & 2
                //Joystick.button(1, bitRead(incomingByte, 1));
                //Joystick.button(2, bitRead(incomingByte, 0));
                gamepad_P1_state.square_btn = bitRead(incomingByte, 1);
                gamepad_P1_state.cross_btn = bitRead(incomingByte, 0);

                ////X POSITION
                //if bitRead(incomingByte, 2)
                //    X_player1 += 511;
                //if bitRead(incomingByte, 3)
                //    X_player1 -= 512;
                //Joystick.X(X_player1);

                ////Y POSITION
                //if bitRead(incomingByte, 4)
                //    Y_player1 += 511;
                //if bitRead(incomingByte, 5)
                //    Y_player1 -= 512;
                //Joystick.Y(Y_player1);

                // PS3 digital direction, use the dir_* constants(enum)
                // 8 = center, 0 = up, 1 = up/right, 2 = right, 3 = right/down
                // 4 = down, 5 = down/left, 6 = left, 7 = left/up

                //Y_UP
                if (bitRead(incomingByte, 4)) {
                    gamepad_P1_state.direction = 0;
                    //X_LEFT
                    if (bitRead(incomingByte, 2)) {
                        gamepad_P1_state.direction = 7;
                    }
                    //X_RIGHT
                    else if (bitRead(incomingByte, 3)) {
                        gamepad_P1_state.direction = 1;
                    }
                }
                else {
                    //Y_DOWN
                    if (bitRead(incomingByte, 5)) {
                        gamepad_P1_state.direction = 4;
                        //X_LEFT
                        if (bitRead(incomingByte, 2)) {
                            gamepad_P1_state.direction = 5;
                        }
                        //X_RIGHT
                        else if (bitRead(incomingByte, 3)) {
                            gamepad_P1_state.direction = 3;
                        }
                    }

                    else {
                        //Y_CENTER
                        //X_LEFT
                        if (bitRead(incomingByte, 2)) {
                            gamepad_P1_state.direction = 6;
                        }
                        //X_RIGHT
                        else if (bitRead(incomingByte, 3)) {
                            gamepad_P1_state.direction = 2;
                        }
                    }
                }
            }
            break;

        case 4:
            // p1 b2
            if (shift_mode) {
                if (bitRead(incomingByte, 7)) {
                    //Keyboard.pressKey(KEY_ESC);
                    gamepad_P1_state.circle_btn = 1;
                    pressed_smth = true;
                }
            }
            else {
                //Joystick.button(8, bitRead(incomingByte, 2));
                //Joystick.button(7, bitRead(incomingByte, 3));
                //Joystick.button(6, bitRead(incomingByte, 4));
                //Joystick.button(5, bitRead(incomingByte, 5));
                //Joystick.button(4, bitRead(incomingByte, 6));
                //Joystick.button(3, bitRead(incomingByte, 7));
                gamepad_P1_state.r2_btn = bitRead(incomingByte, 2);
                gamepad_P1_state.l2_btn = bitRead(incomingByte, 3);
                gamepad_P1_state.r1_btn = bitRead(incomingByte, 4);
                gamepad_P1_state.l1_btn = bitRead(incomingByte, 5);
                gamepad_P1_state.triangle_btn = bitRead(incomingByte, 6);
                gamepad_P1_state.circle_btn = bitRead(incomingByte, 7);
            }
            break;

        case 5:
            // p2 b1
            if (shift_mode) {
                if (bitRead(incomingByte, 7)) {
                    pressed_smth = true;
                }
            }
            else {
                //Joystick2.button(9, bitRead(incomingByte, 7));
                gamepad_P2_state.select_btn = bitRead(incomingByte, 7);
            }

            //Joystick2.button(1, bitRead(incomingByte, 1));
            //Joystick2.button(2, bitRead(incomingByte, 0));
            gamepad_P2_state.square_btn = bitRead(incomingByte, 1);
            gamepad_P2_state.cross_btn = bitRead(incomingByte, 0);

            //if bitRead(incomingByte, 2)
            //    X_player2 += 511;
            //if bitRead(incomingByte, 3)
            //    X_player2 -= 512;
            //Joystick2.X(X_player2);

            //if bitRead(incomingByte, 4)
            //    Y_player2 += 511;
            //if bitRead(incomingByte, 5)
            //    Y_player2 -= 512;
            //Joystick2.Y(Y_player2);
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

        case 6:
            // p2 b2
            //Joystick2.button(8, bitRead(incomingByte, 2));
            //Joystick2.button(7, bitRead(incomingByte, 3));
            //Joystick2.button(6, bitRead(incomingByte, 4));
            //Joystick2.button(5, bitRead(incomingByte, 5));
            //Joystick2.button(4, bitRead(incomingByte, 6));
            //Joystick2.button(3, bitRead(incomingByte, 7));
            gamepad_P2_state.r2_btn = bitRead(incomingByte, 2);
            gamepad_P2_state.l2_btn = bitRead(incomingByte, 3);
            gamepad_P2_state.r1_btn = bitRead(incomingByte, 4);
            gamepad_P2_state.l1_btn = bitRead(incomingByte, 5);
            gamepad_P2_state.triangle_btn = bitRead(incomingByte, 6);
            gamepad_P2_state.circle_btn = bitRead(incomingByte, 7);

            break;

        case 8:
            // coins 1 status
            break;

        case 9:
            // coins1
            if (incomingByte > coins1) {
                // added coin
                coin1 = 1;
                coins1 = incomingByte;
            }
            if (coin1) {
                coin_pressed_at = millis();
                //Keyboard.pressKey(KEY_5);
            }
            else if (coin_pressed_at > 0) {
                if (millis() - coin_pressed_at > 50) {
                    coin_pressed_at = 0;
                    //Keyboard.releaseKey(KEY_5);
                }
            }
            break;

        case 10:
            // coins2 status
            break;
        case 11:
            // coins2
            if (incomingByte > coins2) {
                // added coin
                coin2 = 1;
                coins2 = incomingByte;
            }
            if (coin2) {
                coin_pressed_at = millis();
                //Keyboard.pressKey(KEY_6);
            }
            else if (coin_pressed_at > 0) {
                if (millis() - coin_pressed_at > 50) {
                    coin_pressed_at = 0;
                    //Keyboard.releaseKey(KEY_6);
                }
            }
            break;
        case 13:
            // Analog X P1
            break;

        case 15:
            // Analog Y P1
            break;

        case 17:
            // Analog X P2
            break;

        case 19:
            // Analog Y P2
            break;

        }
        counter++;

        //Joystick.send_now();
        //Joystick2.send_now();
        usb_gamepad_P1_send();
        usb_gamepad_P2_send();
    }

    //Keyboard.send();
    delay(SWCH_DELAY);

    //	if (coins1 > 0){
    //		char str1[ ] = {CMD_DECREASE_COIN};
    //		this->cmd(board, str1, 1);
    //	}
    if (DEBUG_MODE)
        TRACE("\n");
}

int* JVS::cmd(char destination, char data[], int size) {
    this->write_packet(destination, data, size);
    char incomingByte;
    TRACE("waiting for UART avaiability");
    while (!_Uart.available()) {
        delay(500);
    }
    TRACE(" -> ok\n");

    TRACE("waiting for UART sync");
    while (_Uart.read() != 0xE0) {
    } // wait for sync
    TRACE(" -> ok\n");

    TRACE("Testing if for me");
    while (_Uart.read() != 0) {
    } // only if it's for me
    TRACE(" -> ok\n");

    TRACE("waiting for UART avaiability");
    while (!_Uart.available()) {
    } // wait for length
    TRACE(" -> ok\n");

    TRACE("Reading");
    int length = _Uart.read();
    TRACE(" -> Received: E0 0 ");
    phex(length);
    int counter = 0;
    int* res = (int*)malloc(length * sizeof(int));
    while (counter < length) {
        while (!_Uart.available()) {
        }
        incomingByte = _Uart.read();
        res[counter] = incomingByte;
        // actually do something with incomingByte
        TRACE(" ");
        phex(res[counter]);
        counter++;
    }
    TRACE("\n");
    delay(CMD_DELAY);
    return res;
}

void JVS::write_packet(char destination, char data[], int size) {
    TRACE("write_packet: step 1 (SYNC)\n");
    _Uart.write(SYNC);
    TRACE("write_packet: step 2\n");
    _Uart.write(destination);
    TRACE("write_packet: step 3\n");
    _Uart.write(size + 1);
    TRACE("write_packet: step 4\n");
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
    TRACE("write_packet: step 5\n");
    _Uart.write(sum);
    TRACE("write_packet: step 6\n");
    _Uart.flush();
    TRACE("write_packet: step 7\n");
}


