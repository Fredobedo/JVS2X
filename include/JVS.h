#ifndef JVS_H_
#define JVS_H_
#include <Arduino.h>
#include "jvs_constants.h"
#include "USB_HID/USB_PS3/usb_ps3.h"

#define BETWEEN(value, min, max) (value < max && value > min)

#define MAX_RETRY_UART_AVAILABLE_COUNT  100
#define MAX_RETRY_UART_READ_COUNT    	250
#define WAIT_MICRO						200

#define WAIT_UART_AVAILABLE(board) ({\
			cpRetryUARTAvailable=0;\
			while (!_Uart.available()){\
				cpRetryUARTAvailable++;\
				delayMicroseconds(WAIT_MICRO);\
				if(cpRetryUARTAvailable==MAX_RETRY_UART_AVAILABLE_COUNT){\
					if(state[board-1]==settingAddress){\
						state[board-1]=errorSetAddress;\
						break;\
					}\
					else{\
						TRACE("UART Available timeout -> Reboot\n",1);\
						_reboot_Teensyduino_();\
					}\
				}\
			}\
			delayMicroseconds(WAIT_MICRO);\
		})

#define WAIT_UART_READ(c) ({\
			WAIT_UART_AVAILABLE(1);\
			while (_Uart.read() != c){\
				delayMicroseconds(WAIT_MICRO);\
			}\
		})

#define UART_READ_UNESCAPED() ({\
			delayMicroseconds(WAIT_MICRO);\
			incomingByte = _Uart.read();\
			PHEX(incomingByte, 2);\
			TRACE(" ", 2);\
			if (incomingByte == 0xD0) {\
				delayMicroseconds(WAIT_MICRO);\
				incomingByte = _Uart.read();\
				incomingByte++;\
			}\
		})

	/* Response input function code FEATURE_PLAYERS (0x01) */
	struct switch_input_t{
		/* Number of players supported by the IO Board */
		int Players;
		/* Number of buttons assigned per user (Start & service included) */
		int ButtonsPerPlayer;
	};

	/* Response input function code FEATURE_COINS(0x02) */
	struct coin_input_t{
		/* Number of supported coin slots */
		int Slots;
	};

	/* Response input function code FEATURE_ANALOG_IN(0x03) */
	struct analog_input_t{
		/* Number of analog channels */ 
		int Channels;
		/* Number of effective bits (0 for unknown) */
		int Bits;
	};

	/* Response input function code FEATURE_ROTARY(0x04) */
	struct rotary_input_t{
		/* Number of rotary encoders */
		int Channels;
	};

	/* Response input function code FEATURE_KEYPAD(0x05) */
	struct keycode_input_t{
		int byte1;
		int byte2;
		int byte3;
	};

	/* Response input function code FEATURE_LIGHTGUN(0x06) */
	struct screen_position_input_t{
		/* Number of effective bits for X position */
		int Xbits;
		/* Number of effective bits for Y position */
		int Ybits;
		/* Number of supported channles for poitner device */
		int Channels;
	};

	/* Response input function code FEATURE_GENERAL_PURPOSE_IN(0x07) */
	struct misc_switch_input_t{
		int byte1;
		int byte2;
		int byte3;
	};

	/* Response ouptut function code FEATURE_CARD(0x10) */
	struct card_system_output_t{
		/* Number of card slots */
		int Slots;
	};

	/* Response ouptut function code FEATURE_HOPPER(0x11) */
	struct medal_hopper_output_t{
		/* Number of medal hoppers */
		int Channels;
	};

	/* Response ouptut function code FEATURE_GENERAL_PURPOSE(0x12) */
	struct general_purpose_output_t{
		/* Number of other general purpose outputs */
		int Slots;
	};

	/* Response ouptut function code FEATURE_ANALOG_OUT(0x13) */
	struct analog_output_t{
		/* Number of analog output channels */
		int Channels;
	};

	/* Response ouptut function code FEATURE_DISPLAY(0x14) */
	struct character_output_t{
		/* Output display resolution width */
		int width;
		/* Output display resolution height */
		int height;
		/* Output display character code type                */
		/*   - 00: Unknown                                   */
		/*   - 01: ASCII (numeric)                           */
		/*   - 02: ASCII (alphanumeric)                      */
		/*   - 03: ASCII (alphanumeric, half-width katakana) */
		/*   - 04: ASCII (Kanji support, SHIFT-JIS)          */
		int type;
	};

	/* Response ouptut function code FEATURE_BACKUP(0x15) */
	struct backup_output_t{
		/* Has backup data such as coin number */
		int has_backup;
	};

	struct supported_feature_t{
		switch_input_t 				switch_input;
		coin_input_t 				coin_input;
		analog_input_t 				analog_input;
		rotary_input_t 				rotary_input;
		keycode_input_t				keycode_input;
		screen_position_input_t 	screen_position_input;
		misc_switch_input_t			misc_switch_input;
		card_system_output_t 		card_system_output;
		medal_hopper_output_t 		medal_hopper_output;
		general_purpose_output_t 	general_purpose_output;
		analog_output_t 			analog_output;
		character_output_t 			character_output;
		backup_output_t 			backup_output;
	} ;
	
	enum boardState { notSet, errorSetAddress, settingAddress, initalized }; 


	class JVS {
	public:
		boardState state[4]={notSet,notSet,notSet,notSet};  
		JVS(HardwareSerial& serial); // Actor
		void reset();
		void setAddress(int board);
		void getBoardInfo(int board);
		//void switches(int board);
		int* cmd(int destination, char data[], int requestSize);
		bool initialized;
		bool checkRequestStatus(char statusCode);
		bool checkReportCode(char reportCode);
		void resetAllAnalogFuzz();
		void dumpAllAnalogFuzz(int board);
		void setAnalogFuzz(int board);
		void tic();
		void toc();
		void GetSupportedFeatures(int board);
		void GetAllInputs(int board, gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2);
		//int estimateDelayUARTAvailable();
		void DumpSupportedFeatures(int board);
		long supportedFeatures[4];

	private:

		int cpRetryUARTAvailable;
		int cpRetryUARTRead;
		int delayUARTAvailable = 100;
		int delayUARTRead = 100;
		HardwareSerial& _Uart;
		char incomingByte;
		void write_packet(int destination, char data[], int size);
		int WaitForPayload(int board);
		unsigned long beginTime;
		unsigned long elapsedTime;

		int initialSlot1CoinValue=-1;
		int initialSlot2CoinValue=-1;

		int analogEstimatedFuzz[4][8];

		supported_feature_t supported_feature;
		inline bool parseSupportedFeatures(int board);
		inline bool parseSwitchInput(gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2);
		inline void parseSwitchInputPlayer(gamepad_state_t &gamepad_state);
		inline bool parseCoinInput(gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2);
		inline bool parseAnalogInput(int board, gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2);
		inline bool parseLightgunInputChannel(int board, gamepad_state_t &gamepad_state);
	};

#endif /* JVS_H_ */
