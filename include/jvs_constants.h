// -------------------------------------------------------------------------------------------------
// --- JVS CONSTANTS  
// -------------------------------------------------------------------------------------------------
#ifndef JVS_CONSTANTS_H_
#define JVS_CONSTANTS_H_

#define RESTART_ADDR 0xE000ED0C

#define CMD_DELAY 		(uint16_t)20   * 1000 
#define SWCH_DELAY 		(uint16_t)500
//#define ASSIGN_DELAY 	    500 

// low-level protocol constants
#define SYNC 				0xE0
#define ESCAPE 				0xD0
#define BROADCAST 			0xFF 	// broadcast address
#define BUS_MASTER		    0x00 	// (not used for now in this project)
#define DEVICE_ADDR_START	0x01	// (not used for now in this project)

// broadcast commands
#define CMD_RESET 			0xF0	// reset bus														(RESET)											
#define CMD_RESET_ARG		0xD9	// fixed argument to reset command						
#define CMD_ASSIGN_ADDR		0xF1	// assign address to slave											(SETADDR)
#define CMD_SET_COMMS_MODE	0xF2	// switch communications mode for devices that support it			(COMMCHG)

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
// general byte
#define BTN_GENERAL_TEST	1 << 7
#define BTN_GENERAL_TILT1	1 << 6
#define BTN_GENERAL_TILT2	1 << 5
#define BTN_GENERAL_TILT3	1 << 4

// players byte 1
#define BTN_PLAYER_START	1 << 7
#define BTN_PLAYER_SERVICE	1 << 6
#define BTN_PLAYER_UP		1 << 5
#define BTN_PLAYER_DOWN		1 << 4
#define BTN_PLAYER_LEFT		1 << 3
#define BTN_PLAYER_RIGHT	1 << 2
#define BTN_PLAYER_PUSH1	1 << 1
#define BTN_PLAYER_PUSH2	1 << 0

// Player byte 2
#define BTN_PLAYER_PUSH3	1 << 7
#define BTN_PLAYER_PUSH4	1 << 6
#define BTN_PLAYER_PUSH5	1 << 5
#define BTN_PLAYER_PUSH6	1 << 4
#define BTN_PLAYER_PUSH7	1 << 3
#define BTN_PLAYER_PUSH8	1 << 2
#define BTN_PLAYER_PUSH9	1 << 1

// capability structure values for replies on FEATCHK command
#define CAP_END			0x00	// end of structure

// inputs
#define CAP_PLAYERS		0x01	// player/switch info
#define CAP_COINS		0x02	// coin slot info
#define CAP_ANALOG_IN	0x03	// analog info
#define CAP_ROTARY		0x04	// rotary encoder info
#define CAP_KEYPAD		0x05	// keypad info
#define CAP_LIGHTGUN	0x06	// light gun info
#define CAP_GPI			0x07	// general purpose input info

// outputs
#define CAP_CARD		0x10	// card system info
#define CAP_HOPPER		0x11	// token hopper info
#define CAP_GPO			0x12	// general purpose output info
#define CAP_ANALOG_OUT	0x13	// analog output info
#define CAP_DISPLAY		0x14	// character display info
#define CAP_BACKUP		0x15	// backup memory coins

#endif /* CONSTANTS_H_ */
