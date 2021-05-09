#ifndef JVSCLIENT_H_
#define JVSCLIENT_H_


#include <Arduino.h>

class JvsClient{
	public:
		JvsClient(int address);

		int address=0;
		char ioIdentity[102]={0};       // max 102, from specs
		char jvsVersion[5]={0};         // originaly, 1 Byte in BCD format
		char commandVersion[5]={0};     // originaly, 1 Byte in BCD format
		char commandRevision[5]={0};    // originaly, 1 Byte in BCD format
	
		int supportedFeaturesMask=0;

		struct supported_feature_t{
			/* Response input function code FEATURE_PLAYERS (0x01) */
			struct switch_input_t{
				/* Number of players supported by the IO Board */
				int Players;
				/* Number of buttons assigned per user (Start & service included) */
				int ButtonsPerPlayer;
			} switch_input;

			/* Response input function code FEATURE_COINS(0x02) */
			struct coin_input_t{
				/* Number of supported coin slots */
				int Slots;
			} coin_input;

			/* Response input function code FEATURE_ANALOG_IN(0x03) */
			struct analog_input_t{
				/* Number of analog channels */ 
				int Channels;
				/* Number of effective bits (0 for unknown) */
				int Bits;
			} analog_input;

			/* Response input function code FEATURE_ROTARY(0x04) */
			struct rotary_input_t{
				/* Number of rotary encoders */
				int Channels;
			} rotary_input;

			/* Response input function code FEATURE_KEYPAD(0x05) */
			struct keycode_input_t{
				int byte1;
				int byte2;
				int byte3;
			} keycode_input;

			/* Response input function code FEATURE_LIGHTGUN(0x06) */
			struct screen_position_input_t{
				/* Number of effective bits for X position */
				int Xbits;
				/* Number of effective bits for Y position */
				int Ybits;
				/* Number of supported channles for poitner device */
				int Channels;
			} screen_position_input;

			/* Response input function code FEATURE_GENERAL_PURPOSE_IN(0x07) */
			struct misc_switch_input_t{
				int byte1;
				int byte2;
				int byte3;
			} misc_switch_input;

			/* Response ouptut function code FEATURE_CARD(0x10) */
			struct card_system_output_t{
				/* Number of card slots */
				int Slots;
			} card_system_output;

			/* Response ouptut function code FEATURE_HOPPER(0x11) */
			struct medal_hopper_output_t{
				/* Number of medal hoppers */
				int Channels;
			} medal_hopper_output;

			/* Response ouptut function code FEATURE_GENERAL_PURPOSE(0x12) */
			struct general_purpose_output_t{
				/* Number of other general purpose outputs */
				int Slots;
			} general_purpose_output;

			/* Response ouptut function code FEATURE_ANALOG_OUT(0x13) */
			struct analog_output_t{
				/* Number of analog output channels */
				int Channels;
			} analog_output;

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
			} character_output;

			/* Response ouptut function code FEATURE_BACKUP(0x15) */
			struct backup_output_t{
				/* Has backup data such as coin number */
				int has_backup;
			} backup_output;
		} supportedFeatures;

		char rawReportRequest[20];
		int  rawReportRequestLen;

		void setAddress(int address){this->address=address;};

		int Slot1CoinValue=0;
		int Slot2CoinValue=0;
		int initialSlot1CoinValue;
		
		int analogFuzz[16][2]{};
};


#endif