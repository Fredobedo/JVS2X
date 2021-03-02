#ifdef __cplusplus
extern "C" {
#endif

#ifndef print_h__
#define print_h__
#include <Arduino.h>
#include <avr/pgmspace.h>
//#define DEBUG

#ifdef DEBUG
    #define TRACE(x,y)    print(PSTR(x),y)
    #define PHEX(x,y)     phex(x,y)
    #define PHEX16(x,y)   phex16(x,y)
#else
    #define TRACE(x,y)
    #define PHEX(x,y)
    #define PHEX16(x,y)
#endif /* DEBUG */

#define pchar(c) usb_debug_putchar(c)

void print(const char *s, int debugLevel);
void phex(unsigned char c, int debugLevel);
void phex1(unsigned char c, int debugLevel);
void phex16(unsigned int i, int debugLevel);

#endif
#ifdef __cplusplus
} // extern "C"
#endif

