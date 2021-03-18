#include <Arduino.h>
#include "USB_HID/USB_PS3/usb_ps3.h"
#include "hidtrace.h"
#include "jvshost.h"

// Some LEDs indicators to show JVS2X state
#define LED_START_HARDWARE_SERIAL_STATE 1
#define LED_START_JVS_INIT_STATE        2
#define LED_END_JVS_INIT_STATE          3

#define USB_START_DELAY 	1500

HardwareSerial Uart = HardwareSerial();
JvsHost* jvsHost;

//int boardIndex=0;
void blinkState(int nbrOfTime, int interval, int sleepAfter, int finalState);
