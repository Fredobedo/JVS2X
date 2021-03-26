#include <Arduino.h>
#include "JVS/jvs_client_base.h"

JvsClientBase::JvsClientBase(int address)
{
    this->address=address;
}

