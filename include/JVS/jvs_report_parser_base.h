#ifndef JVSREPORTPARSERBASE_H_
#define JVSREPORTPARSERBASE_H_
#include <Arduino.h>
#include "JVS/jvs_constants.h"
#include "JVS/jvs_uart.h"
#include "JVS/jvs_client.h"

class JvsReportParserCommon: public JvsUart {

    public:
        JvsReportParserCommon(HardwareSerial& serial);
        
		bool checkRequestStatus(char statusCode);
		bool checkReportCode(char reportCode);

        void uartReadMultipleUnescaped(int nbr);
};

#endif 