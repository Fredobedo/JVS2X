#ifndef JVSREPORTPARSERCONTROLLER_H_
#define JVSREPORTPARSERCONTROLLER_H_
#include <Arduino.h>
#include "JVS/jvs_constants.h"
#include "JVS/jvs_uart.h"
#include "JVS/jvs_client.h"
#include "JVS/jvs_report_parser_base.h"

class JvsReportParser: public JvsReportParserCommon {

    public:
        JvsReportParser(HardwareSerial& serial);

        //bool parseSupportedFeatures(JvsClient* client);
		bool parseSwitchInput(JvsClient* client);
		void parseSwitchInputPlayer(gamepad_state_t* gamepad_state);
		bool parseCoinInput(JvsClient* client);
		bool parseAnalogInput(JvsClient* client);
		bool parseLightgunInputChannel(JvsClient* client);
};

#endif 