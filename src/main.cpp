#include <Arduino.h>
#include "main.h"
#include <EEPROM.h>

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

  usb_gamepad_reset_state(gamepad_P1_state);
  usb_gamepad_reset_state(gamepad_P2_state);
 
  //ACTIVATING LED PIN
  TRACE("Activating LED\n", 1);
  pinMode(11, OUTPUT);

  //ACTIVATE Hardware Serial (Serial1 on Teensy 2.0)
  TRACE("Activating UART\n", 1);
  blinkState(START_HARDWARE_SERIAL_STATE, 25, 500, 0);
  Uart.begin(115200, DE_PIN);
 
  TRACE("Set sense line to HIGH\n", 1);
  pinMode(SENSE_PIN, OUTPUT);
  analogWrite(SENSE_PIN,1023);
  
  TRACE("Waiting for JVS Cable connection\n", 1);

  // 3FF when nothing is connected
  // 345 when connected but not init
  // 029 when down
  while ((analogRead(SENSE_PIN)>900)) {  }
  TRACE("JVS Cable Detected\n", 1);

  bool errorDetectedDuringInit;

  do
  {
    errorDetectedDuringInit = false;
    nbrOfIOBoards = 0;

    TRACE("JVS send reset command\n", 1);
    blinkState(START_JVS_INIT_STATE, 25, 500, 0);
    j.reset();
    TRACE(" -> done\n", 1);

    while (analogRead(SENSE_PIN) > 50){
        nbrOfIOBoards++;
        TRACE_TEXT_VALUE("Setting address on board ", nbrOfIOBoards, 1);
        j.setAddress(nbrOfIOBoards);

        if(j.state[nbrOfIOBoards-1]==errorSetAddress){
          errorDetectedDuringInit=true;
          TRACE("-> Error! Is the IO Board powered on?", 1);
          delay(2000);
          break;
        }
        j.state[nbrOfIOBoards-1]=initalized;
        
        TRACE_TEXT_VALUE("SENSE pin:", analogRead(SENSE_PIN), 1);

        TRACE("\nIO Board information\n", 1);
        TRACE("--------------------\n", 1);
        j.getBoardInfo(nbrOfIOBoards);
        j.GetSupportedFeatures(nbrOfIOBoards);
        j.DumpSupportedFeatures(nbrOfIOBoards);

        TRACE_TEXT_VALUE("\nStarting Fuzz calculation for board ", nbrOfIOBoards, 1);
        j.setAnalogFuzz(nbrOfIOBoards);
        j.dumpAllAnalogFuzz(nbrOfIOBoards);
        TRACE(" -> done\n",1);
    }
  } while(errorDetectedDuringInit);

  TRACE_TEXT_VALUE("Total IO boards: ", nbrOfIOBoards, 1);
  TRACE("\nJVS INIT SUCCESS !\n\n", 1);


  blinkState(END_JVS_INIT_STATE, 25, 1000, 1);
}

  //USB Full speed, about 8 millisec between each URB_INTERRUPT_IN
void loop() 
{
  //If JVS cable is removed
  if(analogRead(SENSE_PIN)>850){
    TRACE("JVS Cable removed -> Reboot\n", 1);
     _reboot_Teensyduino_();
  }

  for(int cp=1;cp < nbrOfIOBoards+1 ;cp++)
    j.GetAllInputs(cp, gamepad_P1_state, gamepad_P2_state);
   
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



