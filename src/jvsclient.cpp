#include <Arduino.h>
#include "jvsclient.h"

JvsClient::JvsClient(int address, gamepad_state_t* usb_controller[])
{
    this->address=address;
    this->usb_controller=usb_controller;
    //linkUSBGamepad(controller_state_p1, controller_state_p2);
}

/*
void JvsClient::linkUSBGamepad(gamepad_state_t* gamepad_state_p1, gamepad_state_t* gamepad_state_p2){
    // Set controller's default values
    usbGamepadResetState(*gamepad_state_p1);
    usbGamepadResetState(*gamepad_state_p2);

    
    this->gamepad_state_p1=gamepad_state_p1;
    this->gamepad_state_p2=gamepad_state_p2;
};
*/