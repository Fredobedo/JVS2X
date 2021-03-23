#include <Arduino.h>
#include "main.h"
#include "jvsclient.h"
void setup()
{
  // Start USB initialization
  usb_init();

  // INIT USB DEVICE AND INTERFANCES
	while (!usb_configured());

  // Now we can trace in Software Serial (over USB HID)
  // But wait a bit, so the host has time to detect the device and the console can read the traces
  delay(USB_START_DELAY);

  TRACE_P( 1, "\nJVS2X Traces\n");
  TRACE_P( 1, "============\n");  
  TRACE_P( 1, "USB initialization -> done\n");

  // Activate LED pin, will be used to report state
  TRACE_P( 1, "Activating LED\n");
  pinMode(11, OUTPUT);

  // Activate Hardware Serial for RS485 communication (Serial1 on Teensy 2.0)
  TRACE_P( 1, "Activating UART\n");
  blinkState(LED_START_HARDWARE_SERIAL_STATE, 25, 500, 0);
  Uart.begin(115200, DE_PIN);

  jvsHost=new JvsHost(Uart);








  TRACE_P( 1, "Set sense line to HIGH\n");
  pinMode(SENSE_PIN, OUTPUT);
  analogWrite(SENSE_PIN,1023);
  
  TRACE_P( 1, "Waiting for JVS Cable connection\n");

  // 3FF when nothing is connected
  // 345 when connected but not init
  // 029 when down
  while ((analogRead(SENSE_PIN)>900)) {  }
  TRACE_P( 1, "Cable connected\n");

  bool errorDetectedDuringInit;

  do
  {
    errorDetectedDuringInit = false;

    TRACE_P( 1, "Host sends reset command\n");
    blinkState(LED_START_JVS_INIT_STATE, 25, 500, 0);
    jvsHost->resetAllClients();
    TRACE_P( 1, " -> done\n");

    TRACE_ARGS_P( 2, "SENSE pin before setting address(es): %d\n", analogRead(SENSE_PIN));
    while (jvsHost->GetNextClient()) {}

    TRACE_ARGS_P( 2, "SENSE pin: %d\n", analogRead(SENSE_PIN));
    TRACE_ARGS_P( 1, "Total clients: %d\n", jvsHost->jvsClientCount);

    for(int cp = 0; cp < jvsHost->jvsClientCount;cp++)    
    {
      TRACE_P( 1, "\nClient information\n");
      TRACE_P( 1, "------------------\n");
      jvsHost->getBaseBoardInfo(cp);
      jvsHost->dumpBaseBoardInfo(cp);
      jvsHost->getSupportedFeatures(cp);
      jvsHost->dumpSupportedFeatures(cp);

      TRACE_P( 2, "\nsetBulkCommand\n");
      jvsHost->setBulkCommand(cp);
      TRACE_ARGS_P( 1, "\nStarting Fuzz calculation for client: %d, nbr of channels: %d\n", 
                        cp+1, 
                        jvsHost->jvsClient[cp]->supportedFeatures.analog_input.Channels);

      //jvsHost->setAnalogFuzz(cp);
      //jvsHost->dumpAnalogFuzz(cp);
      
      TRACE_P( 1, " -> done\n");
    }
  } while(errorDetectedDuringInit);
  
  TRACE_P( 1, "\nJVS init success !\n\n");
  blinkState(LED_END_JVS_INIT_STATE, 25, 1000, 1);
}

void loop() 
{
  //If JVS cable is removed
  if(analogRead(SENSE_PIN)>850){
    TRACE_P( 1, "JVS Cable removed -> Reboot\n");
     _reboot_Teensyduino_();
  }

  jvsHost->getAllClientReports();
   
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



