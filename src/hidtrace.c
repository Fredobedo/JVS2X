/* Very basic print functions, intended to be used with usb_debug_only.c
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2008 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Version 1.0: Initial Release
#include <Arduino.h>
#include <avr/io.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#include "hidtrace.h"
#include "USB_HID/USB_PS3/usb_ps3.h"

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


