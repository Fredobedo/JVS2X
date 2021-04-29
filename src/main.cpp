#include <Arduino.h>
#include "main.h"


void setup()
{
  // Start USB initialization
  usb_init();

  // INIT USB DEVICE AND INTERFANCES
	while (!usb_configured());

  // Now we can trace in Software Serial (over USB HID)
  // But wait a bit, so the host has time to detect the device and the console can read the traces
  delay(USB_START_DELAY);

  TRACE_ARGS_P( 1, "\nJVS2X %d.%d Traces\n", JVS2X_VERSION, JVS2X_REVISION);
  TRACE_P( 1, "================\n");  
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
}

void loop() 
{
  bool errorDetectedDuringInit;

  TRACE_P( 1, "Waiting for JVS Cable connection\n");

  // 3FF when nothing is connected
  // 345 when connected but not init
  // 029 when down
  while ((analogRead(SENSE_PIN)>900)) {blinkState(2, 500, 0, 0); }
  TRACE_P( 1, "Cable connected\n");
  
  errorDetectedDuringInit = false;

  TRACE_P( 1, "Host sends reset command\n");
  blinkState(LED_START_JVS_INIT_STATE, 25, 500, 0);
  jvsHost->resetAll();
  TRACE_P( 1, " -> done\n");

  TRACE_ARGS_P( 2, "SENSE pin before setting address(es): %d\n", analogRead(SENSE_PIN));

  TRACE_P( 1, "Host sets address(es)\n");
  while (jvsHost->GetNextClient()) {}

  if(!jvsHost->errorTimeout && jvsHost->jvsClientCount>0) 
  {
    TRACE_ARGS_P( 2, "SENSE pin: %d\n", analogRead(SENSE_PIN));
    TRACE_ARGS_P( 1, "Total clients: %d\n", jvsHost->jvsClientCount);

    for(int cp = 0; cp < jvsHost->jvsClientCount;cp++)    
    {
      TRACE_ARGS_P( 1, "\nClient information %d\n", cp+1);
      TRACE_P( 1, "--------------------\n");
      if((errorDetectedDuringInit=!jvsHost->getBaseBoardInfo(cp)))      break;
      jvsHost->dumpBaseBoardInfo(cp);

      if((errorDetectedDuringInit=!jvsHost->getSupportedFeatures(cp)))  break;
      jvsHost->dumpSupportedFeatures(cp);

      TRACE_P( 2, "\nsetBulkCommand\n");
      jvsHost->setBulkCommand(cp);
      
      TRACE_ARGS_P( 1, "\nStarting Fuzz calculation for client: %d, nbr of channels: %d\n", 
                        cp+1, 
                        jvsHost->jvsClient[cp]->supportedFeatures.analog_input.Channels);


      jvsHost->setAnalogFuzz(cp);
      jvsHost->dumpAnalogFuzz(cp);
      
      TRACE_P( 1, " -> done\n");
    }

    if(errorDetectedDuringInit){
      TRACE_P( 1, "\nError during init phase, will restart !\n\n");
    }
    else
    {
      TRACE_P( 1, "\nJVS init success !\n\n");
      blinkState(LED_END_JVS_INIT_STATE, 25, 1000, 1);

      int nbrOfErrors=0;
      while(1)
      {
        if(analogRead(SENSE_PIN)>50)             break; //If JVS cable is removed, the SENSE is up again
        else if(jvsHost->requestReboot){
          TRACE_P( 1, "\nRequest reboot !\n\n");
                    _reboot_Teensyduino_();
        }
        else if(!jvsHost->getAllClientReports()) {
          nbrOfErrors++;
          delay(1000);

          if(nbrOfErrors==3) {
            TRACE_P( 2, "\nToo many errors getting JVS Client reports, will restart !\n\n");

            break;
          }
        }
        // HID report is done via Interrupt IN transfers:
        // - An Interrupt request is queued by the device until the host polls the USB device asking for data. (ACK + Data).
        // - If on the other hand, an interrupt condition was not present when the host polled the interrupt endpoint with an IN token, 
        //   then the function signals this state by sending a NAK.
        //
        // Maximum data payload size for full-speed devices (usb 2.0) is 64 bytes
        //
        // -> Let's try to not bombaring the consumer here... 
        else{
          nbrOfErrors=0;
          jvsHost->ForwardReportsToUSBDevice();
        }
      }
    }
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



