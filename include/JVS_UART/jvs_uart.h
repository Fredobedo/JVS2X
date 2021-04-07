#ifndef JVSUART_H_
#define JVSUART_H_

#include <Arduino.h>
#include "JVS_HOST/jvs_constants.h"
#include "USB_HID/hid_trace.h"

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


#define WAIT_UART_AVAILABLE_FOR_READ() ({\
			timeStarted=millis();\
			while (!_Uart.available()&& !errorTimeout){\
				if (millis() - timeStarted >= SPECS_TIMING_MASTER_READ_TIMEOUT)\
					errorTimeout=true;\
			}\
		})

#define UART_SEEK(c) ({\
		WAIT_UART_AVAILABLE_FOR_READ();\
		while ((!errorTimeout &&_Uart.read() != c)){\
			if (millis() - timeStarted >= SPECS_TIMING_MASTER_READ_TIMEOUT)\
				errorTimeout=true;\
			}\
		})

#define UART_READ_UNESCAPED() ({\
			incomingByte = getByte();\
			TRACE_ARGS(2," %02X", (uint8_t)incomingByte);\
			if ((uint8_t)incomingByte == 0xD0 && !errorTimeout) {\
				incomingByte = getByte();\
				incomingByte++;\
			}\
		})


class JvsUart {
public:
	bool errorTimeout;

protected:
	JvsUart(HardwareSerial& serial);
	HardwareSerial& _Uart;
	int cpRetryUARTAvailable;
	int cpRetryUARTRead;
	int delayUARTAvailable = 100;
	int delayUARTRead = 100;
	
	unsigned long timeStarted;

	char incomingByte;
	int getByte();
	bool writePacket(char destination,  char data[], int payloadSize);
	bool writeRawPacket(const char data[], int size);
	int buildRawRequestPacket(char destination, char data[], int size, char rawPacket[]);

};


#endif