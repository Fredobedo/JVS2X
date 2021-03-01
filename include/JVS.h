#ifndef JVS_H_
#define JVS_H_
#include <Arduino.h>
#include "jvs_constants.h"

	class JVS {
	public:
		JVS(HardwareSerial& serial); // Actor
		void reset();
		void switches(int board);
		void init(int board);
		int* cmd(char destination, char data[], int size);
		bool initialized;
		bool checkRequestStatus(char statusCode);
		bool checkReportCode(char reportCode);
		void tic();
		void toc(const char *s);
	private:
		int coins1;
		int coins2;
		bool shift_mode;
		bool pressed_smth;
		int old_key;
		unsigned long coin_pressed_at;
		HardwareSerial& _Uart;
		void write_packet(char destination, char data[], int size);
		void assign(int attempt);
		int WaitForPayload();

		unsigned long beginTime;
		unsigned long elapsedTime;
	};

#endif /* JVS_H_ */
