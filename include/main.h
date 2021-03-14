#include <Arduino.h>
#include "USB_HID/USB_PS3/usb_ps3.h"
#include "print.h"
#include "jvs.h"
#include "config.h"

// Some LEDs indicators to show JVS2X state
#define LED_START_HARDWARE_SERIAL_STATE 1
#define LED_START_JVS_INIT_STATE        2
#define LED_END_JVS_INIT_STATE          3

// Hardware serial is fixed on Teensy 2.0 -> RX:D2, TX:D3
// PIN F6 is used for Transmit Enable
#define DE_PIN				PIN_F6	// ID 17/A4

// B4 is used for sense line +5V
#define SENSE_PIN			PIN_B4	// ID 13/A8

#define USB_START_DELAY 	1500

int boardIndex=0;
void blinkState(int nbrOfTime, int interval, int sleepAfter, int finalState);
