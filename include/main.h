#include <Arduino.h>
#include "JVS_HOST/jvs_host.h"

// Some LEDs indicators to show JVS2X state
#define LED_START_HARDWARE_SERIAL_STATE 1
#define LED_START_JVS_INIT_STATE        2
#define LED_END_JVS_INIT_STATE          3

#define USB_START_DELAY 	            1500

#define JVS2X_VERSION                   1
#define JVS2X_REVISION                  0

HardwareSerial Uart = HardwareSerial();
JvsHost* jvsHost;

void blinkState(int nbrOfTime, int interval, int sleepAfter, int finalState);
