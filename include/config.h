
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
gamepad_state_t* configGamepad[1][2]={{&usb_controller_1, &usb_controller_2}};

uint8_t not_used;

#define NOT_USED                    not_used
#define CONTROLLER_BUTTON_COIN      usb_controller->start_btn
#define CONTROLLER_BUTTON_TEST      usb_controller->select_btn
#define CONTROLLER_HOME             usb_controller->ps_btn
#define CONTROLLER_START            usb_controller->start_btn
#define CONTROLLER_BUTTON_1         usb_controller->cross_btn
#define CONTROLLER_BUTTON_ANALOG_1  usb_controller->cross_axis    
#define CONTROLLER_BUTTON_2         usb_controller->circle_btn
#define CONTROLLER_BUTTON_ANALOG_2  usb_controller->circle_axis
#define CONTROLLER_BUTTON_3         usb_controller->square_btn
#define CONTROLLER_BUTTON_ANALOG_3  usb_controller->square_axis
#define CONTROLLER_BUTTON_4         usb_controller->triangle_btn
#define CONTROLLER_BUTTON_ANALOG_4  usb_controller->triangle_axis
#define CONTROLLER_BUTTON_5         usb_controller->l1_btn
#define CONTROLLER_BUTTON_ANALOG_5  usb_controller->l1_axis
#define CONTROLLER_BUTTON_6         usb_controller->r1_btn
#define CONTROLLER_BUTTON_ANALOG_6  usb_controller->r1_axis
#define CONTROLLER_BUTTON_7         usb_controller->l2_btn
#define CONTROLLER_BUTTON_8         usb_controller->r2_btn
#define CONTROLLER_LEFT_STICK_X     usb_controller->left_stick_axis_x
#define CONTROLLER_LEFT_STICK_Y     usb_controller->left_stick_axis_y
#define CONTROLLER_ANALOG_1         client->usb_controller[0]->left_stick_axis_x
#define CONTROLLER_ANALOG_2         client->usb_controller[0]->left_stick_axis_y
#define CONTROLLER_ANALOG_3         client->usb_controller[1]->left_stick_axis_x
#define CONTROLLER_ANALOG_4         client->usb_controller[1]->left_stick_axis_y 
#define CONTROLLER_ANALOG_5         NOT_USED
#define CONTROLLER_ANALOG_6         NOT_USED
#define CONTROLLER_ANALOG_7         NOT_USED
#define CONTROLLER_ANALOG_8         NOT_USED
#define CONTROLLER_ANALOG_9         NOT_USED
#define CONTROLLER_ANALOG_10        NOT_USED




#endif
