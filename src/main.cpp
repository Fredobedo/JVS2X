#include <Arduino.h>
#include "USB_HID/USB_PS3/usb_ps3.h"
#include "print.h"
#include "JVS.h"

#define LED_CONFIG	(DDRD |= (1<<6))  
#define LED_OFF	(PORTD &= ~(1<<6))    // ID 11/A10   1000000
#define LED_ON	(PORTD |= (1<<6))

HardwareSerial Uart = HardwareSerial();
JVS j = JVS(Uart);
unsigned long lastTime = 0;

int swFred=0;
/*
Receive:          RX ( -> ID 7/D2)
Transmit:         TX ( -> ID 8/D3)
Sense line:       SENSE_PIN (+5v PIN_B4 -> ID 13/A9)
Transmit enable:  DE_PIN (PIN_F6 -> ID 17/A4)
*/
void setup()
{
  //Hardware serial initialization...
  Uart.begin(115200, DE_PIN);


  //USB initialization...
  usb_init();
	while (!usb_configured());
  delay(START_DELAY);

  print(PSTR("\nTraces JVS2X\n"));
  print(PSTR("============\n"));  
  print(PSTR("USB initialization -> done\n"));

  //print(PSTR("Init start, turning on led"));
  //pinMode(11, OUTPUT);
  //digitalWrite(11, HIGH);
  //print(PSTR(" -> done\n"));

  print(PSTR("JVS initialization:"));
  usb_debug_flush_output();

	while (!j.initialized)
  {
    print(PSTR("JVS send reset command:\n"));
		j.reset();
     print(PSTR(" -> done\n"));

		int i = 1;
		//The sense line is not set !?! This can not work
    //  We should first set it to HIGH (5v), 
    //  Then the IO Board reduces it to about 2.5v directly wehn connected
    //  And finally, the IO Board put the sense down (as I learned from Bobby's great OpenJVS :) )
    //while (analogRead(SENSE_PIN) > 20){
      print(PSTR("JVS send init command:\n"));
      usb_debug_flush_output();

			j.init(i++);
		//}

	}
  print(PSTR(" -> done\n"));

  lastTime = millis();

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



/*
void setup()
{
  //USB initialization
  usb_init();
	while (!usb_configured());

  delay(1000);

}

void loop() 
{
    usb_gamepad_P1_reset_state();

		delay(2000);

  	if (swFred == 1)
		{
			print_P(PSTR(".flip\n"));

			swFred = 0;
			gamepad_P1_state.ps_btn = 0;
			gamepad_P1_state.circle_btn = 1;
			LED_ON;
			LED_OFF;
			LED_ON;
		}
		else
		{
			print_P(PSTR(".flop\n"));
			swFred = 1;
			gamepad_P1_state.ps_btn = 1;
			gamepad_P1_state.circle_btn = 0;
			LED_OFF;
			LED_ON;
			LED_OFF;
		}
		//end test
    usb_debug_flush_output();

		usb_gamepad_P1_send();
}

*/


