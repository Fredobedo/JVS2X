#ifndef JVSCLIENT_H_
#define JVSCLIENT_H_

#include <Arduino.h>
#include "USB_HID/USB_PS3/usb_ps3.h"

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

    class JvsClient{
        public:
            JvsClient(int address, gamepad_state_t* usb_controller[]);
            //JvsClient(int address, gamepad_state_t* controller_state_p1, gamepad_state_t* controller_state_p2);
            boardState state=notSet;  

			int address=0;
			char ioIdentity[102]={0};       // max 102, from specs
			char jvsVersion[5]={0};         // originaly, 1 Byte in BCD format
			char commandVersion[5]={0};     // originaly, 1 Byte in BCD format
			char commandRevision[5]={0};    // originaly, 1 Byte in BCD format
		
			int supportedFeaturesMask=0;
			supported_feature_t supportedFeatures;

			char rawReportRequest[20];
			int  rawReportRequestLen;

			//void linkUSBGamepad(gamepad_state_t* gamepad_state_p1, gamepad_state_t* gamepad_state_p2);
			//void linkUSBGamepad(gamepad_state_t* gamepad_state_p1, gamepad_state_t* gamepad_state_p2);
			void setAddress(int address){this->address=address;};

			int initialSlot1CoinValue=-1;
			int initialSlot2CoinValue=-1;

			gamepad_state_t** usb_controller;
			//gamepad_state_t* gamepad_state_p1;
			//gamepad_state_t* gamepad_state_p2;

			int analogFuzz[16][2]{};
    };




#endif