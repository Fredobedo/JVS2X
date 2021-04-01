
#ifndef JVSHOSTCONFIG_H__
#define JVSHOSTCONFIG_H__


#include <Arduino.h>
#include "JVS_CLIENT/jvs_client.h"

/* ===================================================================================================================================================== */
/*               CONFIGURE HERE YOUR ARCADE LAYOUT                                                                                                       */
/* ===================================================================================================================================================== */
/* Configuration example for 1 CAB//IOBoard with 1 IO Board and 2 players:                                                                               */
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
/*    - Second CAB hast also player & player 2 active                                                                                                    */
/* -> usb_controller_state_t* configGamepad[1][2]={{&usb_controller_p1, &USB_CONTROLLER_UNASSIGNED} , {&usb_controller_p2, &USB_CONTROLLER_UNASSIGNED}}; */


#endif
