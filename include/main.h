#include <Arduino.h>
#include "USB_HID/USB_PS3/usb_ps3.h"
#include "print.h"
#include "jvs.h"

//HARDWARE SERIAL/UART IS FIXED ON TWEENSY 2 -> RX:D2, TX:D3
//PIN F6 WILL BE USED FOR Transmit Enable
#define DE_PIN				PIN_F6	// ID 17/A4

//B4 WILL BE USED FOR +5V SENSE LINE
#define SENSE_PIN			PIN_B4	// ID 13/A8

#define USB_START_DELAY 	1000

int nbrOfIOBoards=0;
void blinkState(int nbrOfTime, int interval, int sleepAfter, int finalState);