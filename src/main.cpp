#include <Arduino.h>
#include "main.h"

#define START_HARDWARE_SERIAL_STATE 1
#define START_JVS_INIT_STATE 2
#define END_JVS_INIT_STATE 3

HardwareSerial Uart = HardwareSerial();
JVS j = JVS(Uart);

unsigned long lastTime = 0;
int cpLoop = 0;
int swFred = 0;
/*
Receive:          RX ( -> ID 7/D2)
Transmit:         TX ( -> ID 8/D3)
Sense line:       SENSE_PIN (+5v PIN_B4 -> ID 13/A8)
Transmit enable:  DE_PIN (PIN_F6 -> ID 17/A4)
*/

/* Fred's notes: PRB & TODO
1. for now, it only works if IO Board is powered on first
   Then, teensy connected to IO Board
   Then, connected to PC/PS3

   -> Declare SENSE_PIN as OUTPUT and activate it(put it HIGH)
      And use it as a sensor to know when a init must be started (when voltage is put to arround 2.5v)
      Then, when the slave has its address set, it puts the sense line to low.
      Master should stop setting addresses when its sense is low.
  
  2. This code will stop working when IO Board is powered off and on again
   -> Use again the sense line which should be down, if not then restart the Teensy
*/

void setup()
{
  //USB initialization
  usb_init();

  //INIT USB DEVICE AND INTERFANCES
	while (!usb_configured());

  //NOW WE CAN TRACE IN SOFWARE SERIAL (over USB)
  delay(USB_START_DELAY);

  TRACE("\nJVS2X Traces\n", 1);
  TRACE("============\n", 1);  
  TRACE("USB initialization -> done\n", 1);

  delay(30);
  usb_gamepad_P1_reset_state();
  usb_gamepad_P2_reset_state();
 
  //ACTIVATING LED PIN
  TRACE("Activating LED\n", 2);
  pinMode(11, OUTPUT);

  //ACTIVATE Hardware Serial (Serial1 on Teensy 2.0)
  TRACE("Activating UART\n", 2);
  blinkState(START_HARDWARE_SERIAL_STATE, 25, 500, 0);
  Uart.begin(115200, DE_PIN);
 
  TRACE("Set sense line to HIGH\n", 2);
  pinMode(SENSE_PIN, OUTPUT);
  analogWrite(SENSE_PIN,1023);
  TRACE("analogRead(SENSE_PIN):", 2);
  PHEX16(analogRead(SENSE_PIN), 2);

  TRACE("\nJVS initialization:\n", 2);
  blinkState(START_JVS_INIT_STATE, 25, 500, 0);

  ///while ((analogRead(SENSE_PIN)>20)) 
  while (!j.initialized)
  {
    TRACE("analogRead(SENSE_PIN):", 2);
    PHEX16(analogRead(SENSE_PIN), 2);
    TRACE("\nJVS send reset command:\n", 2);
		j.reset();
    TRACE(" -> done\n", 2);

		int i = 1;
		//The sense line is not set !?! This can not work
    //  We should first set it to HIGH (5v), 
    //  Then the IO Board reduces it to about 2.5v directly wehn connected
    //  And finally, the IO Board put the sense down (as I learned from Bobby's great OpenJVS :) )
    //while (analogRead(SENSE_PIN) > 20){
        TRACE("analogRead(SENSE_PIN):", 2);
        PHEX16(analogRead(SENSE_PIN), 2);
        TRACE("\nJVS send init command:\n", 2);
		    j.init(i++);
		//}
	}

  TRACE("\nanalogRead(SENSE_PIN):", 2);
  PHEX16(analogRead(SENSE_PIN),2 );

  TRACE("\nJVS INIT SUCCESS !\n",2);
   blinkState(END_JVS_INIT_STATE, 25, 1000, 1);
}

void loop() 
{
   // put your main code here, to run repeatedly:
    

    //USB Full speed, about 8 millisec between each URB_INTERRUPT_IN
    //OpenJVSCore is polling each 50 microsecond
    //if(time - lastTime > 40)
    //{
        //lastTime = time;
        
        //delayMicroseconds(SWCH_DELAY);
        //j.tic();
        

        j.switches(1);

        //j.toc(PSTR("switch finished msec (hex): ")); 

        // if(++cpLoop>100){
        //   cpLoop=0;

        //   if(analogRead(SENSE_PIN)>1000)
        //     _restart_Teensyduino_();
        // }
    //}
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



