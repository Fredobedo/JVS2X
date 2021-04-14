#ifdef JVS2PS3
#ifndef JVSCONTROLLERPS3CONFIG_H__
#define JVSCONTROLLERPS3CONFIG_H__

#include <Arduino.h>
#include "USB_PS3/controller_ps3_constants.h"

/* =========================================================================== */
/*               REMAP HERE YOUR ARCADE CONTROLLER ASSIGNEMENTS                */
/* =========================================================================== */
/* Arcade controls                  PS3 controls                               */
/* ---------------------------      ------------------------------------------ */
#define CONTROLLER_BUTTON_COIN      PS3_START_BUTTON
#define CONTROLLER_BUTTON_TEST      PS3_SELECT_BUTTON
#define CONTROLLER_HOME             PS3_PS_BUTTON
#define CONTROLLER_START            PS3_START_BUTTON
#define CONTROLLER_BUTTON_1         PS3_SQUARE_BUTTON
#define CONTROLLER_BUTTON_ANALOG_1  PS3_SQUARE_AXIS
#define CONTROLLER_BUTTON_2         PS3_CROSS_BUTTON
#define CONTROLLER_BUTTON_ANALOG_2  PS3_CROSS_AXIS
#define CONTROLLER_BUTTON_3         PS3_CIRCLE_BUTTON
#define CONTROLLER_BUTTON_ANALOG_3  PS3_CIRCLE_AXIS
#define CONTROLLER_BUTTON_4         PS3_TRIANGLE_BUTTON
#define CONTROLLER_BUTTON_ANALOG_4  PS3_TRIANGLE_AXIS
#define CONTROLLER_BUTTON_5         PS3_L1_BUTTON
#define CONTROLLER_BUTTON_ANALOG_5  PS3_L1_AXIS
#define CONTROLLER_BUTTON_6         PS3_R1_BUTTON
#define CONTROLLER_BUTTON_ANALOG_6  PS3_R1_AXIS
#define CONTROLLER_BUTTON_7         PS3_L2_BUTTON
#define CONTROLLER_BUTTON_8         PS3_R2_BUTTON
#define CONTROLLER_LEFT_STICK_X     PS3_LEFT_STICK_AXIS_X
#define CONTROLLER_LEFT_STICK_Y     PS3_LEFT_STICK_AXIS_Y
#define CONTROLLER_ANALOG_1         PS3_P1_LEFT_STICK_AXIS_X
#define CONTROLLER_ANALOG_2         PS3_P1_LEFT_STICK_AXIS_Y
#define CONTROLLER_ANALOG_3         PS3_P2_LEFT_STICK_AXIS_X
#define CONTROLLER_ANALOG_4         PS3_P2_LEFT_STICK_AXIS_Y
#define CONTROLLER_ANALOG_5         NOT_USED
#define CONTROLLER_ANALOG_6         NOT_USED
#define CONTROLLER_ANALOG_7         NOT_USED
#define CONTROLLER_ANALOG_8         NOT_USED
#define CONTROLLER_ANALOG_9         NOT_USED
#define CONTROLLER_ANALOG_10        NOT_USED


#endif
#endif
