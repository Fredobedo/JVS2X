
#ifndef config_h__
#define config_h__

#include <Arduino.h>
#include "jvs.h"

/*
Receive:          RX ( -> ID 7/D2)
Transmit:         TX ( -> ID 8/D3)
Sense line:       SENSE_PIN (+5v PIN_B4 -> ID 13/A8)
Transmit enable:  DE_PIN (PIN_F6 -> ID 17/A4)
*/
HardwareSerial Uart = HardwareSerial();

/* ================================================================= */
/*               CONFIGURE HERE YOUR ARCADE LAYOUT                   */
/* ================================================================= */
/* Configuration example for 1 CAB//IOBoard with 1 IO Board and 2 players */
/* Note that if IO Board only supports 1 player, the second gamepad will just be ignored */
JVS* jvsBoard[1]{new JVS(Uart, &gamepad_P1_state, &gamepad_P2_state)};

/* Configuration example for 2 CABs/IOBoards in VS Mode:
    - first CAB hast only player 1 active
    - Second CAB hast only player 2 active
  JVS* jvsBoard[2]{new JVS(Uart, &gamepad_P1_state, NULL), new JVS(Uart, NULL, &gamepad_P2_state)};
*/

/* Configuration example for 2 CABs/IOBoards in VS Mode:
    - first CAB hast only player 1 active
    - Second CAB hast only player 1 active
JVS* jvsBoard[2]{new JVS(Uart, &gamepad_P1_state, NULL), new JVS(Uart, &gamepad_P2_state, NULL)};
*/

/* Configuration example for 2 CABs/IOBoards in VS Mode:
    - first CAB hast only player 1 active
    - Second CAB hast only player 1 active
JVS* jvsBoard[2]{new JVS(Uart, &gamepad_P1_state, NULL), new JVS(Uart, &gamepad_P2_state, NULL)};
*/

/* Configuration example for 2 CABs/IOBoards in clone Mode:
    - first CAB hast player & player 2 active
    - Second CAB hast also player & player 2 active
JVS* jvsBoard[2]{new JVS(Uart, &gamepad_P1_state, &gamepad_P2_state), new JVS(Uart, gamepad_P1_state, &gamepad_P2_state)};
*/

#endif
