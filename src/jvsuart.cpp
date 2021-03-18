#include <Arduino.h>
#include "jvsuart.h"
#include "hidtrace.h"

JvsUart::JvsUart(HardwareSerial& serial):_Uart(serial) {}

int JvsUart::getByte()
{
    WAIT_UART_AVAILABLE_FOR_READ();
    return _Uart.read(); 
}


/* Serial.write() sends binary data <> Serial.print() with sends the ASCII representation */
/* */
void JvsUart::writePacket(char destination, char data[], int size) {
    
    if(strlen(data)>0)
    {
        TRACE(2, "before Packet sent\n");

        WAIT_UART_AVAILABLE_FOR_WRITE();
        // Before sending a new request, ensure the receive buffer is empty
        while(_Uart.available()){_Uart.read();}

        _Uart.write(SYNC);
        _Uart.write(destination);
        /* The byte count is:
              the number of bytes left in the packet, 
              + the SUM byte.*/
        _Uart.write(size + 1);

        // Still do not undestand why I should add 1 here ?
        char sum = destination + size + 1;

        /* Please note that the MARK processing (adding escapes for SYNC char in payload) is transparent to the protocol */
        /*  -> We don't have to take into account of the potential additional characters in the SIZE & SUM calculation   */
        /* We can not use _Uart.print() as we may have to escape some characters                                         */
        /* Anyway, the _Uart.print() does also itarate and write byte by byte                                            */
       // _Uart.print()
        for (int i = 0; i < size; i++) {
            if (data[i] == SYNC || data[i] == ESCAPE) {
                _Uart.write(ESCAPE);
                _Uart.write(data[i] - 1);
            }
            else {
                _Uart.write(data[i]);
            }
            sum+= data[i];
        }
        _Uart.write(sum % 256);
        _Uart.flush();
        TRACE(2, "Packet sent\n");
        // delayMicroseconds(100);
        // TO TEST
        // HardwareSerial::available()          -> returns how many bytes can be written to receive  buffer of serial communication.
        // HardwareSerial::availableForWrite()  -> returns how many bytes can be written to transmit buffer of serial communication before blocking happens.
        // Serial.setTimeout(time)
        // Serial.readBytesUntil(character, buffer, length)
        // Serial.print()
    }
  }



