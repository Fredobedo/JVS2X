#ifndef JVSHOST_H_
#define JVSHOST_H_
#include <Arduino.h>
#include "JVS/jvs_constants.h"
#include "JVS/jvs_uart.h"
#include "JVS/jvs_client.h"
#include "JVS/jvs_report_parser.h"

#define MAX_JVS_CLIENT 31

	class JvsHost: public JvsReportParser {
	public:
		void resetAll();
		bool GetNextClient();

		//Signature for function array
		typedef bool(JvsReportParser::*ParseFunction)(JvsClient* client);
		ParseFunction supportedParsingFunctions[8][10]{{0}};

		JvsHost(HardwareSerial& serial);
	
		bool cmd(char destination, char data[], int requestSize, char responseData[], int responseSize);
	
		bool getBaseBoardInfo(int boardIndex);
		bool getSupportedFeatures(int boardIndex);
		bool getAllClientReports();

		void dumpSupportedFeatures(int boardIndex);
		void dumpBaseBoardInfo(int boardIndex);
		void dumpAnalogFuzz(int boardIndex);

		void setAnalogFuzz(int boardIndex);
		void setBulkCommand(int boardIndex);

		void resetAllAnalogFuzz();

		uint8_t jvsClientCount=0;

		JvsClient* jvsClient[MAX_JVS_CLIENT]{nullptr};

	private:
		int getResponseLength();

		inline bool parseSupportedFeatures(JvsClient* client);		
	};
#endif 
