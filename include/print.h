/* Enable C linkage for C++ Compilers: */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef print_h__
#define print_h__
#include <Arduino.h>
#include <avr/pgmspace.h>
#define DEBUG 1


#ifdef DEBUG
    #define pchar(c) usb_debug_putchar(c)

    #define TRACE(txt,debugLebel) ({\
        static const char __txt[] PROGMEM = txt;\
        print(__txt,debugLebel);\
    }) 
    #define TRACE_TEXT_VALUE(txt, value, debugLebel) ({\
        static const char __txt[] PROGMEM = txt;\
        print(__txt, debugLebel);\
        phex16(value, debugLebel);\
        static const char __nl[] PROGMEM = "\n";\
        print(__nl, debugLebel);\
    }) 
    
    #define PHEX(c, debugLebel)     phex(c, debugLebel)
    #define PHEX16(i, debugLebel)   phex16(i, debugLebel)

    void print(const char *s, int debugLevel);
    void phex(unsigned char c, int debugLevel);
    void phex1(unsigned char c, int debugLevel);
    void phex16(unsigned int i, int debugLevel);
#else
    #define TRACE(txt, debugLebel)
    #define TRACE_TEXT_VALUE(txt, value, debugLebel) 
    #define PHEX(c, debugLebel)
    #define PHEX16(i, debugLebel)
#endif /* DEBUG */


#endif
#ifdef __cplusplus
} // extern "C"
#endif

