#ifndef JVSCONTROLLERCONFIG_H__
#define JVSCONTROLLERCONFIG_H__


#include <Arduino.h>

/* =========================================================================== */
/*               REMAP HERE YOUR ARCADE CONTROLLER ASSIGNEMENTS                */
/* =========================================================================== */
uint8_t not_used=0;

#define NOT_USED                    not_used
#define CONTROLLER_BUTTON_COIN      usb_controller->start_btn
#define CONTROLLER_BUTTON_TEST      usb_controller->select_btn
#define CONTROLLER_HOME             usb_controller->ps_btn
#define CONTROLLER_START            usb_controller->start_btn
#define CONTROLLER_BUTTON_1         usb_controller->square_btn
#define CONTROLLER_BUTTON_ANALOG_1  usb_controller->square_axis
#define CONTROLLER_BUTTON_2         usb_controller->cross_btn
#define CONTROLLER_BUTTON_ANALOG_2  usb_controller->cross_axis
#define CONTROLLER_BUTTON_3         usb_controller->circle_btn
#define CONTROLLER_BUTTON_ANALOG_3  usb_controller->circle_axis
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
#define CONTROLLER_ANALOG_1         configGamepad[client->address-1][0]->left_stick_axis_x
#define CONTROLLER_ANALOG_2         configGamepad[client->address-1][0]->left_stick_axis_y
#define CONTROLLER_ANALOG_3         configGamepad[client->address-1][1]->left_stick_axis_x
#define CONTROLLER_ANALOG_4         configGamepad[client->address-1][1]->left_stick_axis_y 
#define CONTROLLER_ANALOG_5         NOT_USED
#define CONTROLLER_ANALOG_6         NOT_USED
#define CONTROLLER_ANALOG_7         NOT_USED
#define CONTROLLER_ANALOG_8         NOT_USED
#define CONTROLLER_ANALOG_9         NOT_USED
#define CONTROLLER_ANALOG_10        NOT_USED


#endif
