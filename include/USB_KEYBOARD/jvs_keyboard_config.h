#ifdef JVS2KEYBOARD
#ifndef JVSKEYBOARDCONFIG_H__
#define JVSKEYBOARDCONFIG_H__


#include <Arduino.h>


/* =========================================================================== */
/*               REMAP HERE YOUR ARCADE CONTROLLER ASSIGNEMENTS                */
/* =========================================================================== */
/* Arcade controls                     Keyboard KeyCode or Unicode             */
/* ---------------------------         --------------------------------------- */
#define CONTROLLER_BUTTON_TEST         KEY_F2
#define VOLUME_UP                       '='
#define VOLUME_DOWN                     '-'
#define PAUSE                           'p'

/* --- Player 1 --- */
#define CONTROLLER_P1_BUTTON_COIN      '5'
#define CONTROLLER_P1_START            '1'
#define CONTROLLER_P1_BUTTON_1         KEY_LEFT_CTRL
#define CONTROLLER_P1_BUTTON_2         KEY_LEFT_ALT
#define CONTROLLER_P1_BUTTON_3         KEY_SPACE
#define CONTROLLER_P1_BUTTON_4         KEY_LEFT_SHIFT
#define CONTROLLER_P1_BUTTON_5         'z'
#define CONTROLLER_P1_BUTTON_6         'x'
#define CONTROLLER_P1_BUTTON_7         'c'
#define CONTROLLER_P1_BUTTON_8         'v'
#define CONTROLLER_P1_RIGHT            KEY_RIGHT
#define CONTROLLER_P1_LEFT             KEY_LEFT
#define CONTROLLER_P1_DOWN             KEY_DOWN
#define CONTROLLER_P1_UP               KEY_UP

/* --- Player 2 --- */
#define CONTROLLER_P2_BUTTON_COIN      '6'
#define CONTROLLER_P2_START            '2'
#define CONTROLLER_P2_BUTTON_1         'a'
#define CONTROLLER_P2_BUTTON_2         's'
#define CONTROLLER_P2_BUTTON_3         'q'
#define CONTROLLER_P2_BUTTON_4         'w'
#define CONTROLLER_P2_BUTTON_5         'i'
#define CONTROLLER_P2_BUTTON_6         'k'
#define CONTROLLER_P2_BUTTON_7         'j'
#define CONTROLLER_P2_BUTTON_8         'l'
#define CONTROLLER_P2_RIGHT            'g'
#define CONTROLLER_P2_LEFT             'd'
#define CONTROLLER_P2_DOWN             'f'
#define CONTROLLER_P2_UP               'r'

/* --- Shift Functions (Hot keys)                --- */
/* --- Default settings based on IPAC's defaults --- */
#define SHIFT_FUNCTION_EXIT             {CONTROLLER_P1_START, CONTROLLER_P2_START}  
#define SHIFT_FUNCTION_ENTER            {CONTROLLER_P1_START, CONTROLLER_P1_LEFT}
#define SHIFT_FUNCTION_TAB
#define SHIFT_FUNCTION_VOLUME
#define SHIFT_FUNCTION_PAUSE
#define SHIFT_FUNCTION_COIN_P1
#define SHIFT_FUNCTION_COIN_P2

/* --- Next function is not implemented yet but could be if someone is intersted --- */
/* --- Multiple keyboard layout support:                                         --- */
//#define SHIFT_FUNCTION_KEYBOARD_LAYOUT_1    {CONTROLLER_P2_START, CONTROLLER_P2_UP}
//#define SHIFT_FUNCTION_KEYBOARD_LAYOUT_2    {CONTROLLER_P2_START, CONTROLLER_P2_RIGHT}
//#define SHIFT_FUNCTION_KEYBOARD_LAYOUT_3    {CONTROLLER_P2_START, CONTROLLER_P2_LEFT}
//#define SHIFT_FUNCTION_KEYBOARD_LAYOUT_4    {CONTROLLER_P2_START, CONTROLLER_P2_DOWN}

#endif
#endif
