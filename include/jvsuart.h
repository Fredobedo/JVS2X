#ifndef JVSUART_H_
#define JVSUART_H_

#include <Arduino.h>
#include "jvsconstants.h"

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
// 7. Aknowledgment packet Reply timing:   The slave is 1mS after receiving the final data of the request packet.Start sending an acknowledge packet within. Master after 1m
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
#define SPECS_TIMING_MASTER_START						delay(2000)
#define SPECS_TIMING_SLAVE_START_AFTER_RESET			delay(1000)
#define SPECS_TIMING_MASTER_ADDRESS_SETTING_INTERVAL	delay(1000)
#define SPECS_TIMING_SLAVE_ADDRESS_ACKNOWLEDGE			delay(2000)

#ifdef DEBUG
	#if DEBUG==2
		#define SPECS_TIMING_MASTER_READ_TIMEOUT				delayMicroseconds(50)
	#else
		#define SPECS_TIMING_MASTER_READ_TIMEOUT				delayMicroseconds(120)
	#endif
#else
	#define SPECS_TIMING_MASTER_READ_TIMEOUT				delayMicroseconds(120)
#endif

#define TIMING_MASTER_READ_TIMEOUT 120
/*
#define WAIT_UART_AVAILABLE_FOR_WRITE() ({\
			while (!_Uart.availableForWrite()){}\
		})

#define WAIT_UART_AVAILABLE_FOR_READ() ({\
			while (!_Uart.available()&& !errorTimeout){\
			}\
		})

#define WAIT_UART_AVAILABLE_FOR_READ() ({\
			timeStarted=millis();\
			while (!_Uart.available()&& !errorTimeout){\
				delayMicroseconds(WAIT_MICRO);\
				if (millis() - timeStarted >= TIMEOUT)\
					errorTimeout=true;\
			}\
		})


#define UART_SEEK(c) ({\
			WAIT_UART_AVAILABLE_FOR_READ();\
			timeStarted=millis();\
			while ((_Uart.read() != c) && !errorTimeout){\
				delayMicroseconds(WAIT_MICRO);\
				if (millis() - timeStarted >= TIMEOUT)\
					errorTimeout=true;\
			}\
		})
*/

#define WAIT_UART_AVAILABLE_FOR_READ() ({\
			timeStarted=millis();\
			while (!_Uart.available()&& !errorTimeout){\
				if (millis() - timeStarted >= TIMING_MASTER_READ_TIMEOUT)\
					errorTimeout=true;\
			}\
		})

#define UART_SEEK(c) ({\
		WAIT_UART_AVAILABLE_FOR_READ();\
		while ((!errorTimeout &&_Uart.read() != c)){\
			if (millis() - timeStarted >= TIMING_MASTER_READ_TIMEOUT)\
				errorTimeout=true;\
			}\
		})

#define UART_READ_UNESCAPED() ({\
			SPECS_TIMING_MASTER_READ_TIMEOUT;\
			incomingByte = _Uart.read();\
			TRACE_ARGS(2," %02X", (uint8_t)incomingByte);\
			if (incomingByte == 0xD0 && !errorTimeout) {\
				SPECS_TIMING_MASTER_READ_TIMEOUT;\
				incomingByte = _Uart.read();\
				incomingByte++;\
			}\
		})


	class JvsUart {
	public:
        JvsUart(HardwareSerial& serial);
        HardwareSerial& _Uart;
    	int cpRetryUARTAvailable;
		int cpRetryUARTRead;
		int delayUARTAvailable = 100;
		int delayUARTRead = 100;
		bool errorTimeout;
		unsigned long timeStarted;

		char incomingByte;
		int getByte();
		bool writePacket(char destination,  char data[], int payloadSize);
		bool writeRawPacket(const char data[], int size);
		int buildRawRequestPacket(char destination, char data[], int size, char rawPacket[]);

    };


#endif