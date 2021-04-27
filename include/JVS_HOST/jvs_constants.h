#ifndef JVSCONSTANTS_H_
#define JVSCONSTANTS_H_


// -------------------------------------------------------------------------------------------------
// --- JVS CONSTANTS  
// -------------------------------------------------------------------------------------------------
//               --- JVS Timing specs ---
// Extract from JVST_VER3.pdf (JAMMA Video Standard version 3)
// Japanese pdf document translated in english (Google translate + my understanding :) ) 
// 
// 1. Master start:                        The master starts communication after 2S from reset or power-on
// 2. Slave start:                         The slave is ready for communication within 1S after resetting or turning on the power
// 3. Address setting command:             The master issues a 'Set address' request command with an interval of minimum 1 mS
// 4. Address setting acknowledge:         The time the slave has to reply to 'Set address' request and to pull the sense line down (0 v) must be within 1mS.
// 5. Transmit/Emit switch:                After sending or receiving a packet, the direction of the transfer is switched (Transmit, receiver enable) is processed within 100 μS
// 6. Packet-to-packet dead time:          100 μS or more between request and acknowledge packets
// 7. Aknowledgment packet Reply timing:   The slave is 1mS after receiving the final data of the request packet.Start sending an acknowledge packet within. Master after 1ms
// 8. Acknowledgment packet length:        The time for the host, to fully receive the acknowledge packet is equals to the number of bytes (N) x 200 μ S or less, except for specific requests (i.e. 'Set address' -> 1 mS Acknowledge response)
// 9. Acknowledgment Packet Day Interval:  It is recommended that the interval between bytes in the acknowledge packet be 500 μS or less
//
// Which timing is taken into account for now? (via MICROs)
// 1. Master start                       -> SPECS_TIMING_MASTER_START
// 2. Slave start                        -> SPECS_TIMING_SLAVE_START_AFTER_RESET             
// 3. Address setting command            -> SPECS_TIMING_MASTER_ADDRESS_SETTING_INTERVAL     
// 4. Address setting acknowledge        -> SPECS_TIMING_SLAVE_ADDRESS_ACKNOWLEDGE
// 5. Transmit/Emit switch               -> 
// 6. Packet-to-packet dead time         ->  
// 7. Aknowledgment packet Reply timing  ->
// 8. Acknowledgment packet length       ->
// 9. Acknowledgment Packet Day Interval -> SPECS_TIMING_MASTER_READ_TIMEOUT
//
// Please note that effective timing is adapted based on the DEBUG level,
// i.e. in full trace mode (DEBUG 2), there is no need to add some extra delay, that could potentially break the communication between the Host and client(s).
#define SPECS_TIMING_MASTER_START						2000 	//msec
#define SPECS_TIMING_SLAVE_START_AFTER_RESET			1000	//msec
#define SPECS_TIMING_MASTER_ADDRESS_SETTING_INTERVAL	1000	//msec
#define SPECS_TIMING_SLAVE_ADDRESS_ACKNOWLEDGE			2000	//msec

#ifdef JVSDEBUG
	#if JVSDEBUG==2
		#define SPECS_TIMING_MASTER_READ_TIMEOUT				50	//usec
	#else
		#define SPECS_TIMING_MASTER_READ_TIMEOUT				120	//usec
	#endif
#else
	#define SPECS_TIMING_MASTER_READ_TIMEOUT					120	//usec
#endif

//According to specs, we could have a maximum of 31 clients, a bit overkill :) 
#define MAX_JVS_CLIENT 		31

// low-level protocol constants
#define SYNC 				0xE0
#define ESCAPE 				0xD0
#define BROADCAST 			0xFF 	// broadcast address
#define BUS_MASTER		    0x00 	        // (not used for now in this project)
#define DEVICE_ADDR_START	0x01	        // (not used for now in this project)

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


// ---  Shift Keys ---------------------------
// Shift keys memory for player 1 (first byte)
#define SHIFTKEY_PLAYER_1_START		1UL << 7
#define SHIFTKEY_PLAYER_1_SERVICE	1UL << 6
#define SHIFTKEY_PLAYER_1_UP		1UL << 5
#define SHIFTKEY_PLAYER_1_DOWN		1UL << 4
#define SHIFTKEY_PLAYER_1_LEFT		1UL << 3
#define SHIFTKEY_PLAYER_1_RIGHT		1UL << 2
#define SHIFTKEY_PLAYER_1_PUSH1		1UL << 1
#define SHIFTKEY_PLAYER_1_PUSH2		1UL << 0

// Shift keys memory for player 1 (sencond byte)
#define SHIFTKEY_PLAYER_1_PUSH3		1UL << 15
#define SHIFTKEY_PLAYER_1_PUSH4		1UL << 14
#define SHIFTKEY_PLAYER_1_PUSH5		1UL << 13
#define SHIFTKEY_PLAYER_1_PUSH6		1UL << 12
#define SHIFTKEY_PLAYER_1_PUSH7		1UL << 11
#define SHIFTKEY_PLAYER_1_PUSH8		1UL << 10
#define SHIFTKEY_PLAYER_1_PUSH9		1UL << 9

// Shift keys memory for player 2 (third byte)
#define SHIFTKEY_PLAYER_2_START		1UL << 23
#define SHIFTKEY_PLAYER_2_SERVICE	1UL << 22
#define SHIFTKEY_PLAYER_2_UP		1UL << 21
#define SHIFTKEY_PLAYER_2_DOWN		1UL << 20
#define SHIFTKEY_PLAYER_2_LEFT		1UL << 19
#define SHIFTKEY_PLAYER_2_RIGHT		1UL << 18
#define SHIFTKEY_PLAYER_2_PUSH1		1UL << 17
#define SHIFTKEY_PLAYER_2_PUSH2		1UL << 16

// Shift keys memory for player 2 (fourth byte)
#define SHIFTKEY_PLAYER_2_PUSH3		1UL << 31
#define SHIFTKEY_PLAYER_2_PUSH4		1UL << 30
#define SHIFTKEY_PLAYER_2_PUSH5		1UL << 29
#define SHIFTKEY_PLAYER_2_PUSH6		1UL << 28
#define SHIFTKEY_PLAYER_2_PUSH7		1UL << 27
#define SHIFTKEY_PLAYER_2_PUSH8		1UL << 26
#define SHIFTKEY_PLAYER_2_PUSH9		1UL << 25



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
