#include <Arduino.h>
#include "jvsuart.h"
#include "hidtrace.h"

JvsUart::JvsUart(HardwareSerial& serial):_Uart(serial) {}

int JvsUart::getByte()
{
    WAIT_UART_AVAILABLE_FOR_READ();
    return _Uart.read(); 
}


int JvsUart::buildRawRequestPacket(char destination, char data[], int size, char rawPacket[])
{
    TRACE(2, "Building Packet:");
    int cp=0;

    rawPacket[cp++]=SYNC;          TRACE(2, " E0");
    rawPacket[cp++]=destination;   TRACE_ARGS(2, " %02X", destination);
    /* The byte count is:
            the number of bytes left in the packet, 
            + the SUM byte.*/
    rawPacket[cp++]=size + 1;      TRACE_ARGS(2, " %02X", size + 1);

    char sum = destination + size + 1;
    //char sum = size + 1;

    /* Please note that MARK processing (adding escapes for SYNC char in payload) is transparent to the protocol     */
    /*  -> We don't have to take into account of the potential additional characters in the SIZE & SUM calculation   */
    int i;
    for (i = 0; i < size; i++) {
        
        if (data[i] == SYNC || data[i] == ESCAPE) {
            rawPacket[cp++]=ESCAPE;                        TRACE_ARGS(2, " %02X", (uint8_t)ESCAPE);
            rawPacket[cp++]=data[i] - 1;                   TRACE_ARGS(2, " %02X", (uint8_t)(data[i] - 1));
        }
        else {
            rawPacket[cp++]=data[i];                       TRACE_ARGS(2, " %02X", (uint8_t)data[i]);
        }
        sum = (sum + data[i]) % 256;
    }
    rawPacket[cp++]=sum;                             TRACE_ARGS(2, " %02X", (uint8_t)(sum % 256));
    rawPacket[cp++]='\0';
    
    TRACE_ARGS_P(2, "\n -> build done, request length %d\n", i + 4);
    return i + 4;
}

void JvsUart::writeRawPacket(const char data[], int size)
{
    while(_Uart.available()){_Uart.read();}
    WAIT_UART_AVAILABLE_FOR_WRITE();
    for (int i = 0; i < size; i++)  _Uart.write(data[i]);
    _Uart.flush();
}

/* Serial.write() sends binary data <> Serial.print() with sends the ASCII representation */
/* */
void JvsUart::writePacket(char destination, char data[], int size) {
    
    if(strlen(data)>0)
    {
         TRACE(2, "Sending Packet:");

        // Before sending a new request, ensure the receive buffer is empty
        while(_Uart.available()){_Uart.read();}

        WAIT_UART_AVAILABLE_FOR_WRITE();

        _Uart.write(SYNC);          TRACE(2, " E0");
        _Uart.write(destination);   TRACE_ARGS(2, " %02X", destination);
        /* The byte count is:
              the number of bytes left in the packet, 
              + the SUM byte.*/
        _Uart.write(size + 1);      TRACE_ARGS(2, " %02X", size + 1);


        char sum = destination + size + 1;
        //char sum = size + 1;

        /* Please note that MARK processing (adding escapes for SYNC char in payload) is transparent to the protocol     */
        /*  -> We don't have to take into account of the potential additional characters in the SIZE & SUM calculation   */
        for (int i = 0; i < size; i++) {
            if (data[i] == SYNC || data[i] == ESCAPE) {
                _Uart.write(ESCAPE);                        TRACE_ARGS(2, " %02X", (uint8_t)ESCAPE);
                _Uart.write(data[i] - 1);                   TRACE_ARGS(2, " %02X", (uint8_t)(data[i] - 1));
            }
            else {
                _Uart.write(data[i]);                       TRACE_ARGS(2, " %02X", (uint8_t)data[i]);
            }
            sum+= data[i];
        }
        _Uart.write(sum % 256);                             TRACE_ARGS(2, " %02X", (uint8_t)(sum % 256));
        _Uart.flush();
        TRACE(2, "\nPacket sent\n");
        // delayMicroseconds(100);
        // TO TEST
        // HardwareSerial::available()          -> returns how many bytes can be written to receive  buffer of serial communication.
        // HardwareSerial::availableForWrite()  -> returns how many bytes can be written to transmit buffer of serial communication before blocking happens.
        // Serial.setTimeout(time)
        // Serial.readBytesUntil(character, buffer, length)
        // Serial.print()
    }
  }



