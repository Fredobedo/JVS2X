#ifdef __cplusplus
extern "C" {
#endif

#ifndef print_h__
#define print_h__
#include <Arduino.h>
#include <avr/pgmspace.h>
//#define DEBUG

#ifdef DEBUG
    #define TRACE(x)    print(PSTR(x))
    #define PHEX(x)     phex(x)
    #define PHEX16(x)   phex16(x)
#else
    #define TRACE(x)
        #define PHEX(x)
    #define PHEX16(x)
#endif /* DEBUG */

#define pchar(c) usb_debug_putchar(c)

void print(const char *s);
void phex(unsigned char c);
void phex1(unsigned char c);
void phex16(unsigned int i);

#endif
#ifdef __cplusplus
} // extern "C"
#endif

