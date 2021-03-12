#include <Arduino.h>
#include "main.h"
#include <EEPROM.h>

#define LED_START_HARDWARE_SERIAL_STATE 1
#define LED_START_JVS_INIT_STATE        2
#define LED_END_JVS_INIT_STATE          3

/*
Receive:          RX ( -> ID 7/D2)
Transmit:         TX ( -> ID 8/D3)
Sense line:       SENSE_PIN (+5v PIN_B4 -> ID 13/A8)
Transmit enable:  DE_PIN (PIN_F6 -> ID 17/A4)
*/
HardwareSerial Uart = HardwareSerial();
JVS j[4]{Uart,Uart, Uart, Uart};


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
    IOBoard=1;

    TRACE("JVS send reset command\n", 1);
    blinkState(LED_START_JVS_INIT_STATE, 25, 500, 0);
    JVS::broadcastReset(Uart);
    TRACE(" -> done\n", 1);

    while (analogRead(SENSE_PIN) > 50){
        TRACE_TEXT_VALUE("Setting address on board ", IOBoard, 1);
        int rs = JVS::broadcastNewAddress(Uart, IOBoard);

        if(rs==0){
          errorDetectedDuringInit=true;
          TRACE("-> Error! Is the IO Board powered on?", 1);
          delay(2000);
          break;
        }
        j[IOBoard-1].setAddress(rs);
        j[IOBoard-1].assignUSBControllers(&gamepad_P1_state, &gamepad_P2_state);
        j[IOBoard-1].state=initalized;
        
        TRACE_TEXT_VALUE("SENSE pin:", analogRead(SENSE_PIN), 1);

        TRACE("\nIO Board information\n", 1);
        TRACE("--------------------\n", 1);
        j[IOBoard-1].getBoardInfo();
        j[IOBoard-1].GetSupportedFeatures();
        j[IOBoard-1].DumpSupportedFeatures();

        TRACE_TEXT_VALUE("\nStarting Fuzz calculation for board ", IOBoard, 1);
        j[IOBoard-1].setAnalogFuzz();
        j[IOBoard-1].dumpAllAnalogFuzz();
        TRACE(" -> done\n",1);
    }
  } while(errorDetectedDuringInit);

  TRACE_TEXT_VALUE("Total IO boards: ", IOBoard, 1);
  TRACE("\nJVS INIT SUCCESS !\n\n", 1);

  blinkState(LED_END_JVS_INIT_STATE, 25, 1000, 1);
}

//USB Full speed, about 8 millisec between each URB_INTERRUPT_IN
void loop() 
{
  //If JVS cable is removed
  if(analogRead(SENSE_PIN)>850){
    TRACE("JVS Cable removed -> Reboot\n", 1);
     _reboot_Teensyduino_();
  }

  for(int cp=0;cp < IOBoard ;cp++)
    j[cp].GetAllInputs();
   
  usbGamepadP1SendReport();
  usbGamepadP2SendReport();
  //usbGamepadP3SendReport();
  //usbGamepadP4SendReport();
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



