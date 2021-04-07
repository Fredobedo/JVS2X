#ifndef HIDTRACE_H_
#define HIDTRACE_H_

/* Enable C linkage for C++ Compilers: */
#ifdef __cplusplus
extern "C" {
#endif


#include <Arduino.h>
#include <avr/pgmspace.h>
#include "USB_HID/usb.h"

#ifdef JVSDEBUG

    #define TRACE_ARGS(debugLevel, format,...)      traceArgs(debugLevel, format, __VA_ARGS__)
    #define TRACE_ARGS_P(debugLevel, format,...) ({\
            static const char __format[] PROGMEM = format;\
        traceArgs_P(debugLevel, __format, __VA_ARGS__);\
    })         
    #define TRACE(debugLevel, s)                    trace(debugLevel, s)
    #define TRACE_P(debugLevel, s)({\
            static const char __txt[] PROGMEM = s;\
        trace_P(debugLevel, __txt);\
    })         
    #define TRACE_C(debugLevel, c)                  traceC(debugLevel, c)
    #define TRACE_HEX(debugLevel, c)                traceHex(debugLevel, c)
    #define TRACE_HEX1(debugLevel, c)               traceHex1(debugLevel, c)
    #define TRACE_HEX16(debugLevel, i)              traceHex16(debugLevel, i)

    //variadic functions
    void traceArgs(int debugLevel, const char *format, ...);
    void traceArgs_P(int debugLevel, const char *format, ...);

    void trace(int debugLevel, const char *s);
    void trace_P(int debugLevel, const char *s);
    void traceC(int debugLevel, const char c);
    void traceHex(int debugLevel, unsigned char c);
    void traceHex1(int debugLevel, unsigned char c);
    void traceHex16(int debugLevel, unsigned int i);

#else

    #define TRACE_ARGS(debugLevel, format,...)  
    #define TRACE_ARGS_P(debugLevel, format,...)
    #define TRACE(debugLevel, s)  
    #define TRACE_P(debugLevel, s)
    #define TRACE_C(debugLevel, c) 
    #define TRACE_HEX(debugLevel, c) 
    #define TRACE_HEX1(debugLevel, c)
    #define TRACE_HEX16(debugLevel, i) 

#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif

