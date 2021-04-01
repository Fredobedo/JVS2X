#ifndef JVSREPORTPARSERCONTROLLER_H_
#define JVSREPORTPARSERCONTROLLER_H_


#include <Arduino.h>
#include "JVS_HOST/jvs_constants.h"
#include "JVS_UART/jvs_uart.h"
#include "JVS_CLIENT/jvs_client.h"
#include "JVS_PARSER/jvs_report_parser_base.h"

class JvsReportParserController: public JvsReportParserBase {

    public:
        JvsReportParserController(HardwareSerial& serial);

		bool parseSwitchInput(JvsClient* client) override;
		void parseSwitchInputPlayer(usb_controller_state_t* gamepad_state);
		bool parseCoinInput(JvsClient* client) override;
		bool parseAnalogInput(JvsClient* client) override;
		bool parseLightgunInputChannel(JvsClient* client) override;
		bool ForwardReportsToUSBDevice() override;

	private:
		usb_controller_state_t usb_controller_1_previous_state, usb_controller_2_previous_state;
};


#endif 