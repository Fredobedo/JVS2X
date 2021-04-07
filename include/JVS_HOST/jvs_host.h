#ifndef JVSHOST_H_
#define JVSHOST_H_


#include <Arduino.h>
#include "JVS_HOST/jvs_constants.h"
#include "JVS_UART/jvs_uart.h"
#include "JVS_HOST_HELPER/jvs_host_helper.h"

class JvsHost: public JVSHOSTHELPER {

public:
	void resetAll();
	bool GetNextClient();

	//Signature for function array
	typedef bool(JvsHostHelperBase::*ParseFunction)(JvsClient* client);
	ParseFunction supportedParsingFunctions[8][10]{{0}};

	JvsHost(HardwareSerial& serial);

	bool sendCommand(char destination, char data[], int requestSize, char responseData[], int responseSize);

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
