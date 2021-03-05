#ifndef JVS_H_
#define JVS_H_
#include <Arduino.h>
#include "jvs_constants.h"
#include "USB_HID/USB_PS3/usb_ps3.h"

#define BETWEEN(value, min, max) (value < max && value > min)

#define MAX_RETRY_COUNT     100
#define WAIT_UART_AVAILABLE() ({\
			cpRetry=0;\
			while (!_Uart.available())\
				{cpRetry++;\
				delayMicroseconds(50);\
				if(cpRetry==MAX_RETRY_COUNT){\
					print(PSTR("UART Available timeout -> Reboot\n"));\
					 _reboot_Teensyduino_();\
				}\
			}\
			delayMicroseconds(50);\
		})

#define WAIT_UART_READ(c) ({\
			cpRetry=0;\
			WAIT_UART_AVAILABLE();\
			while (_Uart.read() != c)\
				{cpRetry++;\
				delayMicroseconds(50);\
			}\
		})

#define UART_READ_UNESCAPED() ({\
			incomingByte = _Uart.read();\
			PHEX(incomingByte);\
			TRACE(" ");\
			if (incomingByte == 0xD0) {\
				incomingByte = _Uart.read();\
				incomingByte++;\
			}\
		})

	class JVS {
	public:
		JVS(HardwareSerial& serial); // Actor
		void reset();
		void setAddress(int board);
		void getBoardInfo(int board);
		//void switches(int board);
		int* cmd(char destination, char data[], int requestSize);
		bool initialized;
		bool checkRequestStatus(char statusCode);
		bool checkReportCode(char reportCode);
		void resetAllAnalogFuzz();
		void dumpAllAnalogFuzzArray();
		void setAnalogFuzz(int board);
		void tic();
		void toc(const char *s);
		void GetAllInputs(int board, gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2);
		int estimateDelayUARTAvailable();

	private:
		int cpRetry;
		int delayUARTAvailable = 100;
		int delayUARTRead = 100;
		HardwareSerial& _Uart;
		char incomingByte;
		void write_packet(char destination, char data[], int size);
		int WaitForPayload();
		unsigned long beginTime;
		unsigned long elapsedTime;

		int initialSlot1CoinValue=-1;
		int initialSlot2CoinValue=-1;

		int analogEstimatedFuzz[4][8];

		inline bool parseSwitchInput(gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2);
		inline void parseSwitchInputPlayerX(gamepad_state_t &gamepad_state);
		inline bool parseCoinInput(gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2);
		inline bool parseAnalogInput(int board, gamepad_state_t &gamepad_state_p1, gamepad_state_t &gamepad_state_p2);
		//inline int parseGunInput(gamepad_state_t gamepad_state);
	};

#endif /* JVS_H_ */
