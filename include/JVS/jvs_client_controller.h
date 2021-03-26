#ifndef JVSCLIENTCONTROLLER_H_
#define JVSCLIENTCONTROLLER_H_

#include <Arduino.h>
#include "JVS/jvs_client_base.h"
#include "USB_HID/usb.h"

    class JvsClient: public JvsClientBase{
        public:
            JvsClient(int address);

			gamepad_state_t** usb_controller;
    };

#endif