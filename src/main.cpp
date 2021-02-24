#include "main.h"
#include <Arduino.h>

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

   -> Declare SENSE_PIN as OUTPUT and activate it(put it HIGH)
      And use it as a sensor to know when a init must be started (when voltage is put to arround 2.5v)
  
  2. This code will stop working when IO Board is powered off and on again
   -> Use again the sense line which should be down, if not then restart the Teensy
*/

void setup()
{
  //USB initialization
  usb_init();

  //INIT USB DEVICE AND INTERFANCES
	while (!usb_configured());

  pinMode(11, OUTPUT);

  //ACTIVATE Hardware Serial (Serial1 on Teensy 2.0)
  blinkState(1, 500, 1000, 0);
  Uart.begin(115200, DE_PIN);

  //NOW WE CAN TRACE IN SOFWARE SERIAL (over USB)
  delay(START_DELAY);
  TRACE("\nTraces JVS2X\n");
  TRACE("============\n");  

  print(PSTR("test"));
  TRACE("USB initialization -> done\n");
  TRACE("JVS initialization:");
  
  blinkState(2, 500, 1000, 0);
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

  digitalWrite(11,0);
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

void blinkState(int nbrOfTime, int interval, int sleepAfter, int finalState)
{
  digitalWrite(11,0);
  for(int cp=0; cp<nbrOfTime;cp++)
  {
    delay(interval);
    digitalWrite(11,1);
    delay(interval);
    digitalWrite(11,0);
  }

  digitalWrite(11,finalState);
  delay(sleepAfter);
}



