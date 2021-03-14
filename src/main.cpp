#include <Arduino.h>
#include "main.h"



void setup()
{
  // Set controller's default values
  usb_gamepad_reset_state(gamepad_P1_state);
  usb_gamepad_reset_state(gamepad_P2_state);

  // Start USB initialization
  usb_init();

  // INIT USB DEVICE AND INTERFANCES
	while (!usb_configured());

  // Now we can trace in Software Serial (over USB HID)
  // But wait a bit, so the host has time to detect the device and the console can read the traces
  delay(USB_START_DELAY);

  TRACE("\nJVS2X Traces\n", 1);
  TRACE("============\n", 1);  
  TRACE("USB initialization -> done\n", 1);

  // Activate LED pin, will be used to report state
  TRACE("Activating LED\n", 1);
  pinMode(11, OUTPUT);

  // Activate Hardware Serial for RS485 communication (Serial1 on Teensy 2.0)
  TRACE("Activating UART\n", 1);
  blinkState(LED_START_HARDWARE_SERIAL_STATE, 25, 500, 0);
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
    boardIndex=0;

    TRACE("JVS send reset command\n", 1);
    blinkState(LED_START_JVS_INIT_STATE, 25, 500, 0);
    JVS::broadcastReset(Uart);
    TRACE(" -> done\n", 1);

    while (analogRead(SENSE_PIN) > 50){
        TRACE_TEXT_VALUE("Setting address on board ", boardIndex+1, 1);
        int rs = JVS::broadcastNewAddress(Uart, boardIndex+1);

        if(rs==0){
          errorDetectedDuringInit=true;
          TRACE("-> Error! Is the IO Board powered on?", 1);
          delay(2000);
          break;
        }
        jvsBoard[boardIndex]->setAddress(rs);
        jvsBoard[boardIndex]->assignUSBControllers(&gamepad_P1_state, &gamepad_P2_state);
        jvsBoard[boardIndex]->state=initalized;
        
        TRACE_TEXT_VALUE("SENSE pin:", analogRead(SENSE_PIN), 1);

        TRACE("\nIO Board information\n", 1);
        TRACE("--------------------\n", 1);
        jvsBoard[boardIndex]->getBoardInfo();
        jvsBoard[boardIndex]->getSupportedFeatures();
        jvsBoard[boardIndex]->dumpSupportedFeatures();

        TRACE_TEXT_VALUE("\nStarting Fuzz calculation for board ", boardIndex+1, 1);
        jvsBoard[boardIndex]->setAnalogFuzz();
        jvsBoard[boardIndex]->dumpAllAnalogFuzz();
        TRACE(" -> done\n",1);
    }
  } while(errorDetectedDuringInit);

  TRACE_TEXT_VALUE("Total IO boards: ", boardIndex+1, 1);
  TRACE("\nJVS INIT SUCCESS !\n\n", 1);

  blinkState(LED_END_JVS_INIT_STATE, 25, 1000, 1);
}

void loop() 
{
  //If JVS cable is removed
  if(analogRead(SENSE_PIN)>850){
    TRACE("JVS Cable removed -> Reboot\n", 1);
     _reboot_Teensyduino_();
  }

  for(int cp=0;cp < boardIndex+1 ;cp++)
    jvsBoard[cp]->getAllInputs();
   
  usbGamepadP1SendReport();
  usbGamepadP2SendReport();
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



