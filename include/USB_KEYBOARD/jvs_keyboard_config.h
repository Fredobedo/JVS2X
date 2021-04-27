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

/* ---------------------------------------------------------------------------------------------------------------------------- */
/* --- Shift function (Hot keys)                                                                                            --- */
/* ---   - Mask:  can be combinations of these following JVS inputs (c/o bellow)                                            --- */
/* ---   - Value: is the keyCode sent                                                                                       --- */
/* ---   - waitCycle: is the number of time the JVS input is received before forwarding to keyboard (delay before send)     --- */
/* ---------------------------------------------------------------------------------------------------------------------------- */
// ---  Shift Keys player 1 ---------------------------
// - SHIFTKEY_PLAYER_1_START   - SHIFTKEY_PLAYER_1_SERVICE	
// - SHIFTKEY_PLAYER_1_UP      - SHIFTKEY_PLAYER_1_DOWN		
// - SHIFTKEY_PLAYER_1_LEFT    - SHIFTKEY_PLAYER_1_RIGHT
// - SHIFTKEY_PLAYER_1_PUSH1   - SHIFTKEY_PLAYER_1_PUSH2
// - SHIFTKEY_PLAYER_1_PUSH3   - SHIFTKEY_PLAYER_1_PUSH4
// - SHIFTKEY_PLAYER_1_PUSH5   - SHIFTKEY_PLAYER_1_PUSH6
// - SHIFTKEY_PLAYER_1_PUSH7   - SHIFTKEY_PLAYER_1_PUSH8
// - SHIFTKEY_PLAYER_1_PUSH9

// ---  Shift Keys player 2 ---------------------------
// - SHIFTKEY_PLAYER_2_START   - SHIFTKEY_PLAYER_2_SERVICE	
// - SHIFTKEY_PLAYER_2_UP      - SHIFTKEY_PLAYER_2_DOWN		
// - SHIFTKEY_PLAYER_2_LEFT    - SHIFTKEY_PLAYER_2_RIGHT
// - SHIFTKEY_PLAYER_2_PUSH1   - SHIFTKEY_PLAYER_2_PUSH2
// - SHIFTKEY_PLAYER_2_PUSH3   - SHIFTKEY_PLAYER_2_PUSH4
// - SHIFTKEY_PLAYER_2_PUSH5   - SHIFTKEY_PLAYER_2_PUSH6
// - SHIFTKEY_PLAYER_2_PUSH7   - SHIFTKEY_PLAYER_2_PUSH8
// - SHIFTKEY_PLAYER_2_PUSH9

struct shiftkey_t {
	uint32_t	mask;
	uint16_t	value;
    uint16_t    waitCycle;

} shiftkeys[] = {
	{SHIFTKEY_PLAYER_1_START & SHIFTKEY_PLAYER_1_PUSH1, '5', 0},
	{SHIFTKEY_PLAYER_1_START & SHIFTKEY_PLAYER_1_PUSH2, KEY_ESC, 2000},
	{SHIFTKEY_PLAYER_1_START & SHIFTKEY_PLAYER_1_LEFT,  KEY_ENTER, 2000},
	{SHIFTKEY_PLAYER_1_START & SHIFTKEY_PLAYER_1_RIGHT, KEY_TAB, 2000},
	{SHIFTKEY_PLAYER_1_START & SHIFTKEY_PLAYER_1_DOWN,  'p', 2000},
	{SHIFTKEY_PLAYER_2_START & SHIFTKEY_PLAYER_1_PUSH1, '6', 0}
};


#endif
#endif
