#ifndef JVSHOSTCONFIG_H__
#define JVSHOSTCONFIG_H__


#include <Arduino.h>
#include "JVS_CLIENT/jvs_client.h"

#ifdef JVS2PS3
    /* ===================================================================================================================================================== */
    /*               CONFIGURE HERE YOUR ARCADE LAYOUT                                                                                                       */
    /* ===================================================================================================================================================== */
    /* ---------------------   GAME CONTROLLERS ------------------------------------------------------------------------------------------------------------ */
    /* Configuration example for 1 CAB/IOBoard with 1 IO Board and 2 players mapped on 2 Game controllers:                                                   */
    usb_controller_state_t* configGamepad[1][2]={{&usb_controller_p1, &usb_controller_p2}};

    /* Configuration example for 2 CABs/IOBoards in VS Mode:                                                                                                 */
    /*   - first CAB hast only player 1 active                                                                                                               */
    /*   - Second CAB hast only player 2 active                                                                                                              */
    /* -> usb_controller_state_t* configGamepad[1][2]={{&usb_controller_p1, &USB_CONTROLLER_UNASSIGNED} , {&USB_CONTROLLER_UNASSIGNED, &usb_controller_p2}}; */

    /* Configuration example for 2 CABs/IOBoards in VS Mode:                                                                                                 */
    /*    - first CAB hast only player 1 active                                                                                                              */
    /*    - Second CAB hast only player 1 active                                                                                                             */
    /* -> usb_controller_state_t* configGamepad[1][2]={{&usb_controller_p1, &USB_CONTROLLER_UNASSIGNED} , {&usb_controller_p2, &USB_CONTROLLER_UNASSIGNED}}; */

    /* Configuration example for 2 CABs/IOBoards in clone Mode:                                                                                              */
    /*    - first CAB hast player & player 2 active                                                                                                          */
    /*    - Second CAB hast also player 1 & player 2 active                                                                                                  */
    /* -> usb_controller_state_t* configGamepad[1][2]={{&usb_controller_p1, &usb_controller_p2} , {&usb_controller_p1, &usb_controller_p2}}; */
#endif

#ifdef JVS2KEYBOARD
#include "USB_KEYBOARD/usb_keyboard_api.h"
    /* ---------------------   KEYBOARD -------------------------------------------------------------------------------------------------------------------- */
    /* Configuration example for 1 CAB/IOBoard with 1 IO Board and 2 players mapped on 1 keyboard:                                                           */
    usb_keyboard_class* configKeyboard[1][2]={{&Keyboard_P1, NULL}};
    /* usb_keyboard_class* configKeyboard[1][2]={{&Keyboard_P1, &Keyboard_P2}}; */

    /* Configuration example for 2 CABs/IOBoards in VS Mode:                                                                                                 */
    /*   - first CAB hast only player 1 active                                                                                                               */
    /*   - Second CAB hast only player 2 active                                                                                                              */
    /* -> usb_keyboard_class* configKeyboard[1][2]={{&Keyboard_P1, &NULL} , {&NULL, &Keyboard_P2}};                            */

    /* Configuration example for 2 CABs/IOBoards in VS Mode:                                                                                                 */
    /*    - first CAB hast only player 1 active                                                                                                              */
    /*    - Second CAB hast only player 1 active                                                                                                             */
    /* -> usb_keyboard_class* configKeyboard[1][2]={{&Keyboard_P1, &NULL} , {&Keyboard_P2, &NULL}};                            */

    /* Configuration example for 2 CABs/IOBoards in clone Mode:                                                                                              */
    /*    - first CAB hast player & player 2 active                                                                                                          */
    /*    - Second CAB hast also player 1 & player 2 active                                                                                                  */
    /* -> usb_keyboard_class* configKeyboard[1][2]={{&Keyboard_P1, &Keyboard_P2} , {&Keyboard, &Keyboard_P2}};                                               */
#endif

#endif
