/*
 * constants.h
 *
 *  Created on: 14/apr/2014
 *      Author: k4roshi
 * 
 *  Initial version is in Python: https://github.com/TheOnlyJoey/openjvs/blob/master/python/jvs_constants.py
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_


//Switch delay is 50 usec
//reset JVS Between broadcast reset and assign 1 sec
//

// CONFIG
// Delays have been adapted here and we use delayMicroseconds() instead of Delay() 
// This for 2 reasons:
//   - some values are under the millisecond
//   - delayMicroseconds() does not use interrupt, thus is not blocking for other interrupts (i.e. USB)
#define CMD_DELAY 		(uint16_t)20   * 1000 
#define SWCH_DELAY 		(uint16_t)500

#define ASSIGN_DELAY 	500 
#define START_DELAY 	2000

// #define CMD_DELAY 			200
// #define ASSIGN_DELAY 		500
// #define START_DELAY 			3000
// #define SWCH_DELAY 			10
/*
// From python version
# timing data for the bus, in seconds
INIT_DELAY			= 1.0	# delay after a bus reset to wait for devices to initialize
CMD_DELAY			= 0.01	# delay between commands
*/

//HARDWARE SERIAL/UART IS FIXED ON TWEENSY 2 -> RX:D2, TX:D3
//PIN F6 WILL BE USED FOR Transmit Enable
#define DE_PIN				PIN_F6	// ID 17/A4

//B4 WILL BE USED FOR +5V SENSE LINE
#define SENSE_PIN			PIN_B4	// ID 13/A8
#define DEBUG_MODE			false
#if DEBUG_MODE
	#define START_DELAY 		7000
	#define SWCH_DELAY 			1000
#endif
// END CONFIG




// -------------------------------------------------------------------------------------------------
// --- JVS CONSTANTS  
// -------------------------------------------------------------------------------------------------
// low-level protocol constants
#define SYNC 				0xE0
#define ESCAPE 				0xD0
#define BROADCAST 			0xFF 	// broadcast address
//#define BUS_MASTER		0x00 	// (not used for now in this project)
//#define DEVICE_ADDR_START	0x01	// (not used for now in this project)

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


/*
//FROM PYTHON VERSION

//#define CMD_RETRANSMIT		0x2F	// ask device to retransmit data 	(not used for now in this project)
//#define CMD_DECREASE_COINS	0x30	// decrease number of coins 		(not used for now in this project)

//#define CMD_WRITE_GPO		0x32	// write to general-purpose outputs	(not used for now in this project)
//#define CMD_WRITE_ANALOG	0x33	// write to analog outputs 			(not used for now in this project)
//#define CMD_WRITE_DISPLAY	0x34	// write to an alphanumeric display (not used for now in this project)

# manufacturer-specific
CMD_MANUFACTURER_START	= 0x60	# start of manufacturer-specific commands
CMD_MANUFACTURER_END	= 0x7F	# end of manufacturer-specific commands


# capability structure values
CAP_END			= 0x00	# end of structure

# inputs
CAP_PLAYERS		= 0x01	# player/switch info
CAP_COINS		= 0x02	# coin slot info
CAP_ANALOG_IN	= 0x03	# analog info
CAP_ROTARY		= 0x04	# rotary encoder info
CAP_KEYPAD		= 0x05	# keypad info
CAP_LIGHTGUN	= 0x06	# light gun info
CAP_GPI			= 0x07	# general purpose input info

# outputs
CAP_CARD		= 0x10	# card system info
CAP_HOPPER		= 0x11	# token hopper info
CAP_GPO			= 0x12	# general purpose output info
CAP_ANALOG_OUT	= 0x13	# analog output info
CAP_DISPLAY		= 0x14	# character display info
CAP_BACKUP		= 0x15	# backup memory?

# string values for encodings
ENCODINGS = [ "unknown", "ascii numeric", "ascii alphanumeric", "alphanumeric/katakana", "alphanumeric/SHIFT-JIS" ]

# button bit masks for replies to command 
# general byte
BTN_GENERAL_TEST	= 1 << 7
BTN_GENERAL_TILT1	= 1 << 6
BTN_GENERAL_TILT2	= 1 << 5
BTN_GENERAL_TILT3	= 1 << 4

# first player byte
BTN_PLAYER_START	= 1 << 7
BTN_PLAYER_SERVICE	= 1 << 6
BTN_PLAYER_UP		= 1 << 5
BTN_PLAYER_DOWN		= 1 << 4
BTN_PLAYER_LEFT		= 1 << 3
BTN_PLAYER_RIGHT	= 1 << 2
BTN_PLAYER_PUSH1	= 1 << 1
BTN_PLAYER_PUSH2	= 1 << 0

# part of second byte
BTN_PLAYER_PUSH3	= 1 << 7
BTN_PLAYER_PUSH4	= 1 << 6
BTN_PLAYER_PUSH5	= 1 << 5
BTN_PLAYER_PUSH6	= 1 << 4
BTN_PLAYER_PUSH7	= 1 << 3
BTN_PLAYER_PUSH8	= 1 << 2
BTN_PLAYER_PUSH9	= 1 << 1


# timing data for the bus, in seconds
INIT_DELAY			= 1.0	# delay after a bus reset to wait for devices to initialize
CMD_DELAY			= 0.01	# delay between commands
*/

#endif /* CONSTANTS_H_ */
