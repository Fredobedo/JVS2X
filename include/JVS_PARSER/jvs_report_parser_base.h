#ifndef JVSREPORTPARSERBASE_H_
#define JVSREPORTPARSERBASE_H_


#include <Arduino.h>
#include "JVS_HOST/jvs_constants.h"
#include "JVS_UART/jvs_uart.h"
#include "JVS_CLIENT/jvs_client.h"

class JvsReportParserBase: public JvsUart {

    public:
        JvsReportParserBase(HardwareSerial& serial);
        
		bool checkRequestStatus(char statusCode);
		bool checkReportCode(char reportCode);

        void uartReadMultipleUnescaped(int nbr);

        virtual bool parseSwitchInput(JvsClient* client)  = 0;
		virtual bool parseCoinInput(JvsClient* client) = 0;
		virtual bool parseAnalogInput(JvsClient* client) = 0;
		virtual bool parseLightgunInputChannel(JvsClient* client) = 0;
		virtual bool ForwardReportsToUSBDevice() = 0;
};


#endif 