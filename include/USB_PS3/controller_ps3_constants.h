#ifdef JVS2PS3
#ifndef JVSCONTROLLERPS3CONSTANTS_H__
#define JVSCONTROLLERPS3CONSTANTS_H__

uint8_t not_used=0;
#define NOT_USED                    not_used
#define PS3_START_BUTTON            usb_controller->start_btn
#define PS3_SELECT_BUTTON           usb_controller->select_btn
#define PS3_PS_BUTTON               usb_controller->ps_btn
#define PS3_SQUARE_BUTTON           usb_controller->square_btn
#define PS3_SQUARE_AXIS             usb_controller->square_axis
#define PS3_CROSS_BUTTON            usb_controller->cross_btn
#define PS3_CROSS_AXIS              usb_controller->cross_axis
#define PS3_CIRCLE_BUTTON           usb_controller->circle_btn
#define PS3_CIRCLE_AXIS             usb_controller->circle_axis
#define PS3_TRIANGLE_BUTTON         usb_controller->triangle_btn
#define PS3_TRIANGLE_AXIS           usb_controller->triangle_axis
#define PS3_L1_BUTTON               usb_controller->l1_btn
#define PS3_L1_AXIS                 usb_controller->l1_axis
#define PS3_R1_BUTTON               usb_controller->r1_btn
#define PS3_R1_AXIS                 usb_controller->r1_axis
#define PS3_L2_BUTTON               usb_controller->l2_btn
#define PS3_R2_BUTTON               usb_controller->r2_btn
#define PS3_LEFT_STICK_AXIS_X       usb_controller->left_stick_axis_x
#define PS3_LEFT_STICK_AXIS_Y       usb_controller->left_stick_axis_y
#define PS3_P1_LEFT_STICK_AXIS_X    configGamepad[client->address-1][0]->left_stick_axis_x
#define PS3_P1_LEFT_STICK_AXIS_Y    configGamepad[client->address-1][0]->left_stick_axis_y
#define PS3_P2_LEFT_STICK_AXIS_X    configGamepad[client->address-1][1]->left_stick_axis_x
#define PS3_P2_LEFT_STICK_AXIS_Y    configGamepad[client->address-1][1]->left_stick_axis_y 


#endif
#endif