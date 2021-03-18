// -------------------------------------------------------------------------------------------------
// --- JVS CONSTANTS  
// -------------------------------------------------------------------------------------------------
#ifndef JVSCONSTANTS_H_
#define JVSCONSTANTS_H_

// low-level protocol constants
#define SYNC 				(int8_t)0xE0
#define ESCAPE 				(int8_t)0xD0
#define BROADCAST 			(int8_t)0xFF 	// broadcast address
#define BUS_MASTER		    0x00 	        // (not used for now in this project)
#define DEVICE_ADDR_START	0x01	        // (not used for now in this project)

// broadcast commands
#define CMD_RESET 			(int8_t)0xF0	// reset bus														(RESET)											
#define CMD_RESET_ARG		(int8_t)0xD9	// fixed argument to reset command						
#define CMD_ASSIGN_ADDR		(int8_t)0xF1	// assign address to slave											(SETADDR)
#define CMD_SET_COMMS_MODE	(int8_t)0xF2	// switch communications mode for devices that support it			(COMMCHG)

// Single commands
#define CMD_REQUEST_ID		 0x10	// requests an ID string from a device								(IOIDENT)					
#define CMD_COMMAND_VERSION	 0x11	// gets command format version as two BCD digits packed in a byte	(CMDREV)
#define CMD_JVS_VERSION		 0x12	// gets JVS version as two BCD digits packed in a byte				(JVSREV)
#define CMD_COMMS_VERSION	 0x13	// gets communications version as two BCD digits packed in a byte	(COMMVER)
#define CMD_CAPABILITIES	 0x14	// gets a special capability structure from the device				(FEATCHK)
#define CMD_CONVEY_ID		 0x15	// convey main board info to device (maker, game, soft ver, etc.)	(MAINID)

// DATA I/O commands
#define CMD_READ_DIGITAL 	0x20	// read switch inputs			(SWINP)
#define CMD_READ_COINS		0x21	// read coin inputs				(COININP)
#define CMD_READ_ANALOG		0x22	// read analog inputs			(ANLINP)
#define CMD_READ_ROTARY		0x23	// read rotary encoder inputs	(ROTINP)
#define CMD_READ_KEYPAD		0x24	// read keypad inputs			(KEYINP)
#define CMD_READ_LIGHTGUN	0x25	// read light gun inputs		(SCRPOSINP)
#define CMD_READ_GPI		0x26	// read general-purpose inputs	(MISCSWINP)
#define CMD_RETRANSMIT		0x2F	// If Master detects checksum error, ask retransmit (must contain the same payload) (RETRANSMIT) 
#define CMD_DECREASE_COIN	0x30	// decrease number of coins		(COINDEC)
#define CMD_INCREASE_COIN	0x31 	// increase number of coins		(PAYINC)

// Switch bit masks for replies to CMD_READ_DIGITAL command 
//  byte 1 = general
#define BTN_GENERAL_TEST	1 << 7
#define BTN_GENERAL_TILT1	1 << 6
#define BTN_GENERAL_TILT2	1 << 5
#define BTN_GENERAL_TILT3	1 << 4

// byte 2 = general + player x part 1
#define BTN_PLAYER_START	1 << 7
#define BTN_PLAYER_SERVICE	1 << 6
#define BTN_PLAYER_UP		1 << 5
#define BTN_PLAYER_DOWN		1 << 4
#define BTN_PLAYER_LEFT		1 << 3
#define BTN_PLAYER_RIGHT	1 << 2
#define BTN_PLAYER_PUSH1	1 << 1
#define BTN_PLAYER_PUSH2	1 << 0

// bytet 3 = player x part 2
#define BTN_PLAYER_PUSH3	1 << 7
#define BTN_PLAYER_PUSH4	1 << 6
#define BTN_PLAYER_PUSH5	1 << 5
#define BTN_PLAYER_PUSH6	1 << 4
#define BTN_PLAYER_PUSH7	1 << 3
#define BTN_PLAYER_PUSH8	1 << 2
#define BTN_PLAYER_PUSH9	1 << 1

// inputs
#define FEATURE_FUNCTION_CODE_PLAYERS		        0x01	// player/switch info
#define FEATURE_FUNCTION_CODE_COINS		            0x02	// coin slot info
#define FEATURE_FUNCTION_CODE_ANALOG_IN	            0x03	// analog info
#define FEATURE_FUNCTION_CODE_ROTARY		        0x04	// rotary encoder info
#define FEATURE_FUNCTION_CODE_KEYPAD		        0x05	// keypad info
#define FEATURE_FUNCTION_CODE_LIGHTGUN	            0x06	// light gun info
#define FEATURE_FUNCTION_CODE_GENERAL_PURPOSE_IN    0x07	// general purpose input info

// outputs
#define FEATURE_FUNCTION_CODE_CARD		            0x10	// card system info
#define FEATURE_FUNCTION_CODE_HOPPER		        0x11	// token hopper info
#define FEATURE_FUNCTION_CODE_GENERAL_PURPOSE_OUT	0x12	// general purpose output info
#define FEATURE_FUNCTION_CODE_ANALOG_OUT	        0x13	// analog output info
#define FEATURE_FUNCTION_CODE_DISPLAY		        0x14	// character display info
#define FEATURE_FUNCTION_CODE_BACKUP		        0x15	// backup memory coins

// bitmap for storing detected features
#define FEATURE_HAS_PLAYERS		        1 << 0	// player/switch info
#define FEATURE_HAS_COINS		        1 << 1	// coin slot info
#define FEATURE_HAS_ANALOG_IN	        1 << 2	// analog info
#define FEATURE_HAS_ROTARY		        1 << 3	// rotary encoder info
#define FEATURE_HAS_KEYPAD		        1 << 4	// keypad info
#define FEATURE_HAS_LIGHTGUN	        1 << 5	// light gun info
#define FEATURE_HAS_GENERAL_PURPOSE_IN  1 << 6	// general purpose input info

#define FEATURE_HAS_CARD		        1 << 7	// card system info
#define FEATURE_HAS_HOPPER		        1 << 8	// token hopper info
#define FEATURE_HAS_GENERAL_PURPOSE_OUT	1 << 9	// general purpose output info
#define FEATURE_HAS_ANALOG_OUT	        1 << 10	// analog output info
#define FEATURE_HAS_DISPLAY		        1 << 11	// character display info
#define FEATURE_HAS_BACKUP		        1 << 12	// backup memory coins

#define REPORT_CODE_NORMAL            0x01
#define REPORT_CODE_PARAM_ERROR       0x02
#define REPORT_CODE_PARAM_DATA_ERROR  0x03
#define REPORT_CODE_BUSY              0x04

#define REQUEST_STATUS_NORMAL           0x01
#define REQUEST_STATUS_COMMAND_UNKNOWN  0x02
#define REQUEST_STATUS_SUM_ERROR        0x03
#define REQUEST_STATUS_BUSY             0x04

#endif /* CONSTANTS_H_ */
