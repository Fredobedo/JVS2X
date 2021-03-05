#include <Arduino.h>
#include "main.h"

#define START_HARDWARE_SERIAL_STATE 1
#define START_JVS_INIT_STATE 2
#define END_JVS_INIT_STATE 3

HardwareSerial Uart = HardwareSerial();
JVS j = JVS(Uart);

//gamepad_state_t gamepad_P1_state;
//gamepad_state_t gamepad_P2_state;

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

  usb_gamepad_reset_state(gamepad_P1_state);
  usb_gamepad_reset_state(gamepad_P2_state);
 
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
  
  TRACE("\nWaiting for JVS Cable connection\n");

  // 3FF when nothing is connected
  // 345 when connected but not init
  // 029 when down
  while ((analogRead(SENSE_PIN)>900)) {  }

  TRACE("\nJVS send reset command:\n");
  blinkState(START_JVS_INIT_STATE, 25, 500, 0);
  j.reset();
  TRACE(" -> done\n");

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
  }

  TRACE("\nNbr of IO boards found:");
  PHEX(nbrOfIOBoards);

/*
  TRACE("\nEstimating UART Speed:\n");
  int test=j.estimateDelayUARTAvailable();

TRACE("FINISHED!!!!!!!\n");
TRACE("FINISHED!!!!!!!\n");
TRACE("FINISHED!!!!!!!\n");
TRACE("FINISHED!!!!!!!\n");
  TRACE("\n-> UART Minimum wait estimated to ");
  PHEX(test);
*/
  TRACE("\nsetAnalogFuzz\n");
  j.resetAllAnalogFuzz();
  j.setAnalogFuzz(nbrOfIOBoards);

  TRACE("\nJVS INIT SUCCESS !\n");
  blinkState(END_JVS_INIT_STATE, 25, 1000, 1);
}

  //USB Full speed, about 8 millisec between each URB_INTERRUPT_IN
void loop() 
{
  //If JVS cable is removed
  if(analogRead(SENSE_PIN)>850){
    print(PSTR("JVS Cable removed -> Reboot\n"));
     _reboot_Teensyduino_();
  }

  for(int cp=1;cp < nbrOfIOBoards+1 ;cp++)
    j.GetAllInputs(cp, gamepad_P1_state, gamepad_P2_state);
    
  //usb_gamepad_P1_send();
  //usb_gamepad_P2_send();
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



