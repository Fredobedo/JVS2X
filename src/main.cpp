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

  TRACE("\nJVS2X Traces\n");
  TRACE("============\n");  
  TRACE("USB initialization -> done\n");

  delay(30);
  usb_gamepad_P1_reset_state();
  usb_gamepad_P2_reset_state();
 
  //ACTIVATING LED PIN
  TRACE("Activating LED\n");
  pinMode(11, OUTPUT);

  //ACTIVATE Hardware Serial (Serial1 on Teensy 2.0)
  TRACE("Activating UART\n");
  blinkState(START_HARDWARE_SERIAL_STATE, 25, 500, 0);
  Uart.begin(115200, DE_PIN);
 
  TRACE("Set sense line to HIGH\n");
  pinMode(SENSE_PIN, OUTPUT);
  analogWrite(SENSE_PIN,1023);
  TRACE("analogRead(SENSE_PIN):");
  PHEX16(analogRead(SENSE_PIN));
  
  TRACE("\nWaiting for JVS Cable connection\n");

  // 3FF when nothing is connected
  // 345 when connected but not init
  // 029 when down
  while ((analogRead(SENSE_PIN)>900)) {  }

  TRACE("JVS initialization start\n");
  blinkState(START_JVS_INIT_STATE, 25, 500, 0);

 
    TRACE("analogRead(SENSE_PIN):");
    PHEX16(analogRead(SENSE_PIN));
    TRACE("\nJVS send reset command:\n");
		j.reset();
    TRACE(" -> done\n");

    j.resetAllAnalogFuzz();

		nbrOfIOBoards = 0;
    while (analogRead(SENSE_PIN) > 50){
        nbrOfIOBoards++;
        TRACE("JVS send init command on board ");
        PHEX16(nbrOfIOBoards);
        TRACE("\n");

        j.setAddress(nbrOfIOBoards);
        TRACE("After set address, analogRead(SENSE_PIN):");
        PHEX16(analogRead(SENSE_PIN));

        TRACE("\ngetBoardInfo\n");
		    j.getBoardInfo(nbrOfIOBoards);

        TRACE("setAnalogFuzz\n");
        j.setAnalogFuzz(nbrOfIOBoards);
		}

  TRACE("\nNbr of IO boards found:");
  PHEX(nbrOfIOBoards);

	j.dumpAllAnalogFuzzArray();

  TRACE("\nJVS INIT SUCCESS !\n");
  blinkState(END_JVS_INIT_STATE, 25, 1000, 1);
}

void loop() 
{
  //USB Full speed, about 8 millisec between each URB_INTERRUPT_IN

  //If JVS cable is removed
  if(analogRead(SENSE_PIN)>850)
     _reboot_Teensyduino_();

  for(int cp=1;cp < nbrOfIOBoards+1 ;cp++)
    j.switches(cp);
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



