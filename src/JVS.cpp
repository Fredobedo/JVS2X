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

#include <Arduino.h>
#include "JVS.h"
#include "constants.h"

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
    delay(ASSIGN_DELAY);
    TRACE("RESET\n");
    this->write_packet(BROADCAST, str, 2);  // -> Broadcast Reset communication status to all slaves
    delay(ASSIGN_DELAY);

    if (analog)
        TRACE("Analogique active");
    if (mirroring)
        TRACE("Mirroring active");
    if (full_joy)
        TRACE("Full Joystick active");
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
    delayMicroseconds(500);
    TRACE("REQ\n");   
    char str1[] = { (char)CMD_REQUEST_ID};                  // -> Master requests to initiate a communication, Get slave ID Data (0x10)
    this->cmd(board, str1, 1);                              //    Request size: 1  | Response size: max 102
    delayMicroseconds(500);
    TRACE("CMD\n"); 
    char str2[] = { (char)CMD_COMMAND_VERSION };            // -> Command format revision (0x11)
    this->cmd(board, str2, 1);                              //    Request size: 1  | Response size: 2
    delayMicroseconds(500);
    TRACE("JVS\n"); 
    char str3[] = { (char)CMD_JVS_VERSION };                // -> JVS revision (0x12)
    this->cmd(board, str3, 1);                              //    Request size: 1  | Response size: 2
    TRACE("CMS\n"); 
    delayMicroseconds(500);
    char str4[] = { (char)CMD_COMMS_VERSION };              // -> Communication version
    this->cmd(board, str4, 1);                              //    Request size: 1  | Response size: 2
    delayMicroseconds(500);
    TRACE("CAP\n"); 
    char str5[] = { (char)CMD_CAPABILITIES };               // -> Check Slave features
    this->cmd(board, str5, 1);                              //    Request size: 1  | Response size: 6+
    delayMicroseconds(500);

    initialized = true;
}


//JVS always sends:
//  - SYNC
//  - Node No:  Destination node Nbr, 0 is for the host
//  - Lengh:    nbr of following data bytes including the SUM byte
//  - byte 1:   data
//  - byte 2:   data
//  - etc.
//  - SUM:      CheckSum on all bytes in packet -SYNC -SUM modulo 256
//
//
// CMD_READ_DIGITAL  | Switch inputs (read controller switches)     | 3 | 3+
// CMD_READ_COINS    | Coin Inputs (read coin counters)             | 2 | 2x slots
// CMD_READ_ANALOG   | Analog Inputs (read analog inputs)           | 2 | 2x ports
// CMD_READ_ROTARY   | Rotary Inputs (read rotary inputs)           | 2 | 2x ports
// CMD_READ_KEYPAD   | Keycode Inputs (read keycode inputs)         | 1 | 2
// CMD_READ_LIGHTGUN | Gun Inputs (read lightgun inputs)            | 2 | 2x ports
// CMD_READ_GPI      | Misc Switch Inputs (read misc switch inputs) | 2 | 2+

// TODO
// Verify response value:
//  --- Report Code ---
//  - 0x01: Normal
//  - 0x02: Error, incorrect Nbr of parameters were sent
//  - 0x03: Error, data supplied is invalid
//  - 0x04: Busy, I/O Board cannot receive the command 
void JVS::switches(int board) {
    char str[] = {  CMD_READ_DIGITAL, 0x02, 0x02,       // Command 1: Read input switch for the 2 players in 2 bytes format
                    CMD_READ_COINS, 0x02,               // Command 2: Read coin values for 2 slots
                    CMD_READ_ANALOG, 0x04 };            // Command 3: Read analog values for 4 channels

    // --- SEND REQUEST ---
    //SYNC + Node(board) + ByteNbr(sizeof str) + Payload (str) + SUM
    this->write_packet(board, str, sizeof str);
    char incomingByte;
    
    // --- READ RESPONSE ---
    //SYNC + Node + ByteNbr + Status
    while (!_Uart.available()) {}
    while (_Uart.read() != 0xE0) {} // wait for sync
    while (_Uart.read() != 0) {
        print(PSTR("\nStrange, something not for me in the reply, what is it?? : "));

    }    // only if it's for me
    while (!_Uart.available()) {}   // wait for length

    int length = _Uart.read();
    int counter = 0;

    //TO DO: Recenter PAD & analog pos

    //int X_player1 = 512;
    //int Y_player1 = 512;
    //int X_player2 = 512;
    //int Y_player2 = 512;

    int coin1 = 0;
    int coin2 = 0;
    bool old_shift = shift_mode;


    gamepad_P1_state.select_btn = 0;
    gamepad_P2_state.select_btn = 0;

    bool abordRequest = false;
    while (counter < length - 1 && !abordRequest) {
        while (!_Uart.available()) {
        }
        incomingByte = _Uart.read();

        PHEX(incomingByte);
        TRACE(" -> ");
        PHEX16(incomingByte);

        ///Check reply for the complete packet
        abordRequest=!checkRequestStatus(incomingByte);

        if(abordRequest)
            continue;
        else
        {
            //Check if the marker('Escape Byte') has been used -> 0xE0 or 0xD0 is in the payload.
            //If so, restore original value.
            if ((unsigned)(int)incomingByte == 0xFFFFFFD0) {    
                int escapeByte = _Uart.read();                  
                if (escapeByte == 0xDF)                         
                    incomingByte = 0xE0;                        
                else if (escapeByte == 0xCF)                         
                    incomingByte = 0xD0;   
            }
            switch (counter) {
            case 0:
                // Byte0 is report for first command (-> here CMD_READ_DIGITAL Player 1 & 2)
                // Then, 3 bytes for Player 1 + 2 bytes for player 2:
                //      Byte1: Test & Tilt
                //      Byte2: Player 1 switches
                //      Byte3: Player 1 switches
                //      Byte4: Player 2 switches
                //      Byte5: Player 2 switches
                abordRequest=!checkReportStatus(incomingByte);
                break;
            case 1:
                // TEST is on byte1 bit7
                if bitRead(incomingByte, 7)
                    //Keyboard.pressKey(KEY_F2);
                    gamepad_P1_state.ps_btn = 1;
                else
                    //Keyboard.releaseKey(KEY_F2);
                    gamepad_P1_state.ps_btn = 0;
                break;

            case 2:
                 //shift_mode is activate by presing 1P start + something -> byte2 bit7
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

                    //Y_UP=5
                    //Y_DOWN=4
                    //X_LEFT=3
                    //X_RIGHT=2

                    //Y_DOWN: Byte2 bit4
                    if (bitRead(incomingByte, 4)) {
                        gamepad_P1_state.direction = 4;
                        //X_RIGHT
                        if (bitRead(incomingByte, 2)) {
                            gamepad_P1_state.direction = 3;
                        }
                        //X_LEFT
                        else if (bitRead(incomingByte, 3)) {
                            gamepad_P1_state.direction = 5;
                        }
                    }
                    else {
                        //Y_UP
                        if (bitRead(incomingByte, 5)) {
                            gamepad_P1_state.direction = 0;
                            //X_RIGHT
                            if (bitRead(incomingByte, 2)) {
                                gamepad_P1_state.direction = 1;
                            }
                            //X_LEFT
                            else if (bitRead(incomingByte, 3)) {
                                gamepad_P1_state.direction = 7;
                            }
                        }

                        else {
                            //Y_CENTER
                            //X_RIGHT
                            if (bitRead(incomingByte, 2)) {
                                gamepad_P1_state.direction = 2;
                            }
                            //X_LEFT
                            else if (bitRead(incomingByte, 3)) {
                                gamepad_P1_state.direction = 6;
                            }
                        }
                    }
                }
                break;

            case 3:
                if (shift_mode) {
                    if (bitRead(incomingByte, 7)) {
                        //Keyboard.pressKey(KEY_ESC);
                        gamepad_P1_state.circle_btn = 1;
                        pressed_smth = true;
                    }
                }
                else {
                    gamepad_P1_state.r2_btn = bitRead(incomingByte, 2);
                    gamepad_P1_state.l2_btn = bitRead(incomingByte, 3);
                    gamepad_P1_state.r1_btn = bitRead(incomingByte, 4);
                    gamepad_P1_state.l1_btn = bitRead(incomingByte, 5);
                    gamepad_P1_state.triangle_btn = bitRead(incomingByte, 6);
                    gamepad_P1_state.circle_btn = bitRead(incomingByte, 7);
                }
                break;

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

            case 5:
                gamepad_P2_state.r2_btn = bitRead(incomingByte, 2);
                gamepad_P2_state.l2_btn = bitRead(incomingByte, 3);
                gamepad_P2_state.r1_btn = bitRead(incomingByte, 4);
                gamepad_P2_state.l1_btn = bitRead(incomingByte, 5);
                gamepad_P2_state.triangle_btn = bitRead(incomingByte, 6);
                gamepad_P2_state.circle_btn = bitRead(incomingByte, 7);

                break;
            case 6:
                // Byte6 is report for second command (-> here CMD_READ_COINS)
                // then, 2 bytes per Slot:
                //      Byte7: Slot1 state
                //      Byte8: Slot1 count
                //      Byte9: Slot2 state
                //      Byte10: Slot2 count
                abordRequest=!checkReportStatus(incomingByte);
                break;
            case 7:
                // coins 1 status
                break;

            case 8:
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

            case 9:
                // Coin Slot2 status
                break;
            case 10:
                // coin slot2 
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
            case 11:
                // Byte11 is report for third command (-> here CMD_READ_ANALOG player 1 & 2)
                // Then, we have 8 bytes for the 4 analogue channels (2 bytes per channel)
                abordRequest=!checkReportStatus(incomingByte);
                break;
            case 12:
                // Analog X P1
                break;

            case 14:
                // Analog Y P1
                break;

            case 16:
                // Analog X P2
                break;

            case 18:
                // Analog Y P2
                break;
            }
            counter++;

            usb_gamepad_P1_send();
            usb_gamepad_P2_send();
        }
    }
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
            TRACE("Warning, slave is too busy, it can't process the command\n");
        return false;
}

//Check the report 
bool JVS::checkReportStatus(char reportStatus)
{
    if(reportStatus == 0x01)
        return true;
    else if(reportStatus == 0x02)
        TRACE("Warning, command parameter error, no return data\n");
    else if(reportStatus == 0x03)
        TRACE("Warning, command parameter error, parameter is ignored\n");
    else if(reportStatus == 0x04)
        TRACE("Warning, slave is too busy, it can't process the command\n");
    return false;
}

// TODO
// Verify response value:
//  --- Status Code ---
//  - 0x01: Normal
//  - 0x02: Unsupported command
//  - 0x03: Checksum error
//  - 0x04: Acknowledge overflow <--- !!!!!
int* JVS::cmd(char destination, char data[], int size) {
    this->write_packet(destination, data, size);
    char incomingByte;

    TRACE("waiting for UART avaiability");
    while (!_Uart.available()) { }
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

    TRACE("waiting for UART avaiability");
    while (!_Uart.available()) { } // wait for length
    TRACE(" -> ok\n");

    TRACE("Reading");
    int length = _Uart.read();
    TRACE(" -> Received: E0 00 ");
    PHEX(length); // <-- This is not always the lengh but Error code too !!!
    TRACE("\n");

    int counter = 0;
    int* res = (int*)malloc(length * sizeof(int));
    while (counter < length) {
        while (!_Uart.available()) {
        }
        incomingByte = _Uart.read();
        res[counter] = incomingByte;
        // actually do something with incomingByte
        TRACE(" ");
        PHEX(res[counter]);
        counter++;
    }
    TRACE("\n");
    delayMicroseconds(CMD_DELAY);
    return res;
}

void JVS::write_packet(char destination, char data[], int size) {
    if(strlen(data)>0)
    {
        //while (!_Uart.available()) { } 

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
    }
}


