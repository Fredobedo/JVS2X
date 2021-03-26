#include <Arduino.h>
#include <stdio.h>
#include "USB_HID/hid_trace.h"


#ifdef DEBUG

void traceArgs(int debugLevel, const char *format,  ...)
{
	if (debugLevel<= DEBUG)
	{
		va_list args;
		va_start (args, format);
		char buffer[100];
		vsprintf(buffer, format, args);
		va_end (args);
		
		for(int cp=0; cp < sizeof(buffer); cp++)
		{
			if(!buffer[cp]) break;
			if (buffer[cp] == '\n') usb_debug_putchar('\r');
			usb_debug_putchar(buffer[cp]);
		}
		usb_debug_flush_output();
	}
}

void traceArgs_P(int debugLevel, const char *format,  ...)
{
	if (debugLevel<= DEBUG)
	{
		va_list args;
		va_start (args, format);
		char buffer[100];
		vsprintf_P(buffer, format, args);
		va_end (args);
		
		for(int cp=0; cp < sizeof(buffer); cp++)
		{
			if(!buffer[cp]) break;
			if (buffer[cp] == '\n') usb_debug_putchar('\r');
			usb_debug_putchar(buffer[cp]);
		}
		usb_debug_flush_output();
	}
}

void traceC(int debugLevel, const char c)
{ 
	if (debugLevel<= DEBUG)
	{
		usb_debug_putchar(c);
		usb_debug_flush_output();
	}
}

void trace(int debugLevel, const char *s)
{ 
	if (debugLevel<= DEBUG)
	{
		char c;
		while (1) {
			c = *s++;
			if (!c) break;
			if (c == '\n') usb_debug_putchar('\r');
			usb_debug_putchar(c);
		}

		usb_debug_flush_output();
	}
}

void trace_P(int debugLevel, const char *s)
{ 
	if (debugLevel<= DEBUG)
	{
		char buffer[100];
		sprintf_P(buffer, s);
		
		for(int cp=0; cp < sizeof(buffer); cp++)
		{
			if(!buffer[cp]) break;
			if (buffer[cp] == '\n') usb_debug_putchar('\r');
			usb_debug_putchar(buffer[cp]);
		}
		usb_debug_flush_output();
	}
}

void traceHex1(int debugLevel, unsigned char c)
{
	if (debugLevel<= DEBUG) {
		usb_debug_putchar(c + ((c < 10) ? '0' : 'A' - 10));
		usb_debug_flush_output();
	}
}

void traceHex(int debugLevel, unsigned char c)
{
	if (debugLevel<= DEBUG)	{	
		traceHex1(c >> 4, debugLevel);
		traceHex1(c & 15, debugLevel);
	}

}

void traceHex16(int debugLevel, unsigned int i)
{
	if (debugLevel<= DEBUG)	{	
		traceHex(i >> 8, debugLevel);
		traceHex(i, debugLevel);
	}
}

#endif


