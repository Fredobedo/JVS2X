#ifdef JVS2KEYBOARD
#ifndef JVSHOSTHELPERKEYBOARD_H_
#define JVSHOSTHELPERKEYBOARD_H_


#include <Arduino.h>
#include <keylayouts.h>
#include "JVS_HOST/jvs_constants.h"
#include "JVS_UART/jvs_uart.h"
#include "JVS_CLIENT/jvs_client.h"
#include "JVS_HOST_HELPER/jvs_host_helper_base.h"
#include "USB_KEYBOARD/usb_keyboard_api.h"

#define MAX_SHIFTKEYS 20

class JvsHostHelperKeyboard: public JvsHostHelperBase {

    public:
        JvsHostHelperKeyboard(HardwareSerial& serial);

		bool parseSwitchInput(JvsClient* client) override;
		bool parseCoinInput(JvsClient* client) override;
		bool parseAnalogInput(JvsClient* client) override;
		bool parseLightgunInputChannel(JvsClient* client) override;
		void ForwardReportsToUSBDevice() override;

	private:
		unsigned long inputForShiftKeys=0;
		uint16_t nbrOfWaitCycle[MAX_SHIFTKEYS];

        inline void setKeyState(usb_keyboard_class* keyboard, uint16_t KeyCode, bool State);
        inline void setShiftKeyState(usb_keyboard_class* keyboard, uint16_t KeyCode, bool State, int idxShiftKey);

		uint8_t usb_keyboard_P1_previous_state[8]={0,0,0,0,0,0,0,0};
		uint8_t usb_keyboard_P2_previous_state[8]={0,0,0,0,0,0,0,0};
};


#endif 
#endif 