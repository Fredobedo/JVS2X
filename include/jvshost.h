#ifndef JVSHOST_H_
#define JVSHOST_H_
#include <Arduino.h>
#include "jvsconstants.h"
#include "jvsuart.h"
#include "jvsclient.h"
#include "USB_HID/USB_PS3/usb_ps3.h"

/*
Receive:          RX ( -> ID 7/D2)
Transmit:         TX ( -> ID 8/D3)
Sense line:       SENSE_PIN (+5v PIN_B4 -> ID 13/A8)
Transmit enable:  DE_PIN (PIN_F6 -> ID 17/A4)
*/

// Hardware serial is fixed on Teensy 2.0 -> RX:D2, TX:D3
// PIN F6 is used for Transmit Enable
#define DE_PIN				PIN_F6	// ID 17/A4

// B4 is used for sense line +5V
#define SENSE_PIN			PIN_B4	// ID 13/A8

#define BETWEEN(value, min, max) (value < max && value > min)
#define MAX_JVS_CLIENT 4

	class JvsHost:JvsUart {
	public:
		void resetAllClients();
		bool GetNextClient();

		typedef int(*ParseFunction)(int);
		ParseFunction supportedFunctions[10];

		JvsHost(HardwareSerial& serial);
	
		char* cmd(char destination, char data[], int requestSize);

		bool checkRequestStatus(char statusCode);
		bool checkReportCode(char reportCode);
	
		void getBaseBoardInfo(int boardIndex);
		void getSupportedFeatures(int boardIndex);
		void getAllClientReports();

		void dumpBaseBoardInfo(int boardIndex);
		void dumpSupportedFeatures(int boardIndex);
		void dumpAnalogFuzz(int boardIndex);

		void setAnalogFuzz(int boardIndex);
		void setBulkCommand(int boardIndex);

		void resetAllAnalogFuzz();

		int jvsClientCount=0;

	private:
		JvsClient* jvsClient[MAX_JVS_CLIENT]{nullptr};

		void checkUart();
		int getNextResponseLength();
		
		inline bool parseSupportedFeatures(JvsClient* client);
		inline bool parseSwitchInput(JvsClient* client);
		inline void parseSwitchInputPlayer(gamepad_state_t* gamepad_state);
		inline bool parseCoinInput(JvsClient* client);
		inline bool parseAnalogInput(JvsClient* client);
		inline bool parseLightgunInputChannel(JvsClient* client, gamepad_state_t* gamepad_state);
		inline void uartReadMultipleUnescaped(int nbr);
	};
#endif 
