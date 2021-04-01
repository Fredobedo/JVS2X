#include <Arduino.h>
#include "JVS_CLIENT/jvs_client.h"

JvsClient::JvsClient(int address)
{
    this->address=address;
}

