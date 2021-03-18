
#ifndef config_h__
#define config_h__

#include <Arduino.h>
#include "jvshost.h"
#include "jvsclient.h"


/* ================================================================= */
/*               CONFIGURE HERE YOUR ARCADE LAYOUT                   */
/* ================================================================= */
/* Configuration example for 1 CAB//IOBoard with 1 IO Board and 2 players */
/* Note that if IO Board only supports 1 player, the second gamepad will just be ignored */


//JvsClient* jvsClient[1]{new JvsClient(&gamepad_P1_state, &gamepad_P2_state)};
/* Configuration example for 2 CABs/IOBoards in VS Mode:
    - first CAB hast only player 1 active
    - Second CAB hast only player 2 active
  JvsHost* jvsBoard[2]{new JvsHost(Uart, &gamepad_P1_state, NULL), new JvsHost(Uart, NULL, &gamepad_P2_state)};
*/

/* Configuration example for 2 CABs/IOBoards in VS Mode:
    - first CAB hast only player 1 active
    - Second CAB hast only player 1 active
JvsHost* jvsBoard[2]{new JvsHost(Uart, &gamepad_P1_state, NULL), new JvsHost(Uart, &gamepad_P2_state, NULL)};
*/

/* Configuration example for 2 CABs/IOBoards in VS Mode:
    - first CAB hast only player 1 active
    - Second CAB hast only player 1 active
JvsHost* jvsBoard[2]{new JvsHost(Uart, &gamepad_P1_state, NULL), new JvsHost(Uart, &gamepad_P2_state, NULL)};
*/

/* Configuration example for 2 CABs/IOBoards in clone Mode:
    - first CAB hast player & player 2 active
    - Second CAB hast also player & player 2 active
JvsHost* jvsBoard[2]{new JvsHost(Uart, &gamepad_P1_state, &gamepad_P2_state), new JvsHost(Uart, gamepad_P1_state, &gamepad_P2_state)};
*/

gamepad_state_t* configGamepad[1][2]={{&gamepad_P1_state, &gamepad_P2_state}};


#endif
