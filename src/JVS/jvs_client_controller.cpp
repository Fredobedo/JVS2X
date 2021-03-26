#include <Arduino.h>
#include "JVS/jvs_client_controller.h"
#include "JVS/jvs_host_config.h"

JvsClient::JvsClient(int address):JvsClientBase(address)
{
    this->usb_controller=configGamepad[address-1];
}

