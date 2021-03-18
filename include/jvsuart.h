#ifndef JVSUART_H_
#define JVSUART_H_

#include <Arduino.h>
#include "jvsconstants.h"

#define MAX_RETRY_UART_AVAILABLE_COUNT  100
#define MAX_RETRY_UART_READ_COUNT    	250
#define WAIT_MICRO						100

#define WAIT_UART_AVAILABLE_FOR_WRITE() ({\
			while (!_Uart.availableForWrite()){delayMicroseconds(WAIT_MICRO);}\
		})

#define WAIT_UART_AVAILABLE_FOR_READ() ({\
			cpRetryUARTAvailable=0;\
			while (!_Uart.available())\
				delayMicroseconds(WAIT_MICRO);\
		})

#define UART_SEEK(c) ({\
			WAIT_UART_AVAILABLE_FOR_READ();\
			while (_Uart.read() != c){\
				delayMicroseconds(WAIT_MICRO);\
			}\
		})

#define UART_READ_UNESCAPED() ({\
			delayMicroseconds(WAIT_MICRO);\
			incomingByte = _Uart.read();\
			TRACE_HEX(2, incomingByte);\
			TRACE(2, " ");\
			if (incomingByte == 0xD0) {\
				delayMicroseconds(WAIT_MICRO);\
				incomingByte = _Uart.read();\
				incomingByte++;\
			}\
		})


	class JvsUart {
	protected:
        JvsUart(HardwareSerial& serial);
        HardwareSerial& _Uart;
    	int cpRetryUARTAvailable;
		int cpRetryUARTRead;
		int delayUARTAvailable = 100;
		int delayUARTRead = 100;

		char incomingByte;
		int getByte();
		void writePacket(char destination, char data[], int size);

    };


#endif