#ifndef JVS_H_
#define JVS_H_
#include <Arduino.h>
#include "jvs_constants.h"

#define BETWEEN(value, min, max) (value < max && value > min)

#define MAX_RETRY_COUNT     100
#define WAIT_UART_AVAILABLE() ({\
			cpRetry=0;\
			while (!_Uart.available())\
				{cpRetry++;\
				delayMicroseconds(100);\
				if(cpRetry==MAX_RETRY_COUNT)\
					 _reboot_Teensyduino_();\
				}\
			delayMicroseconds(100);})

#define WAIT_UART_READ(c) ({\
			cpRetry=0;\
			while (_Uart.read() != c)\
				{cpRetry++;\
				delayMicroseconds(100);\
				if(cpRetry==MAX_RETRY_COUNT)\
					 _reboot_Teensyduino_();\
				}\
			})


	class JVS {
	public:
		JVS(HardwareSerial& serial); // Actor
		void reset();
		void setAddress(int board);
		void getBoardInfo(int board);
		void switches(int board);
		int* cmd(char destination, char data[], int requestSize);
		bool initialized;
		bool checkRequestStatus(char statusCode);
		bool checkReportCode(char reportCode);
		void resetAllAnalogFuzz();
		void dumpAllAnalogFuzzArray();
		void setAnalogFuzz(int board);
		void tic();
		void toc(const char *s);

	private:
		int cpRetry;
		HardwareSerial& _Uart;
		void write_packet(char destination, char data[], int size);
		int WaitForPayload();
		unsigned long beginTime;
		unsigned long elapsedTime;

		int initialSlot1CoinValue=-1;
		int initialSlot2CoinValue=-1;

		int analogEstimatedFuzz[4][8];
	};

#endif /* JVS_H_ */
