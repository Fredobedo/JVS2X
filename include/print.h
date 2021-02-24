#ifdef __cplusplus
extern "C" {
#endif

#ifndef print_h__
#define print_h__

#include <avr/pgmspace.h>
#define DEBUG 


#ifdef DEBUG
#define TRACE(x)    print(PSTR(x))
#else
#define TRACE(x)
#endif /* DEBUG */

// this macro allows you to write print("some text") and
// the string is automatically placed into flash memory :)
#define pchar(c) usb_debug_putchar(c)

void print(const char *s);
void phex(unsigned char c);
void phex16(unsigned int i);

#endif
#ifdef __cplusplus
} // extern "C"
#endif

