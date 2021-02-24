#include <Arduino.h>
#include "USB_HID/USB_PS3/usb_ps3.h"
#include "print.h"
#include "JVS.h"

#define DEBUG

HardwareSerial Uart = HardwareSerial();
JVS j = JVS(Uart);
unsigned long lastTime = 0;

/*
Receive:          RX ( -> ID 7/D2)
Transmit:         TX ( -> ID 8/D3)
Sense line:       SENSE_PIN (+5v PIN_B4 -> ID 13/A9)
Transmit enable:  DE_PIN (PIN_F6 -> ID 17/A4)
*/

/* Fred's notes: PRB & TODO
1. for now, it only works if IO Board is powered on first
   Then, teensy connected to IO Board
   Then, connected to PC/PS3

   -> Activate the sense line(put PIN HIGH)
      And use it as a sensor to know when a init must be started (when voltage is put to arround 2.5v)
  
  2. This code will stop working when IO Board is powered off and on again
   -> Use again the sense line which should be down, if not then restart the Teensy
*/

void setup()
{
  pinMode(11, OUTPUT);
   Uart.begin(115200, DE_PIN);

  //USB initialization
  usb_init();
	while (!usb_configured());

  delay(START_DELAY);
  TRACE("\nTraces JVS2X\n");
  TRACE("============\n");  
  TRACE("USB initialization -> done\n");
  TRACE("JVS initialization:");
  
  while (!j.initialized)
  {
    TRACE("JVS send reset command:\n");
		j.reset();
    TRACE(" -> done\n");

		//int i = 1;
		//The sense line is not set !?! This can not work
    //  We should first set it to HIGH (5v), 
    //  Then the IO Board reduces it to about 2.5v directly wehn connected
    //  And finally, the IO Board put the sense down (as I learned from Bobby's great OpenJVS :) )
    //while (analogRead(SENSE_PIN) > 20){
    TRACE("JVS send init command:\n");

		j.init(1);
		//}

	}
}

void loop() 
{
  // put your main code here, to run repeatedly:
      unsigned long time = millis();
    if(time - lastTime >= 20)
    {
        lastTime = time;
        j.switches(1);
    }
}



