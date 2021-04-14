#ifndef USB_H_
#define USB_H_

#ifdef JVS2PS3
    #include "USB_PS3/usb_ps3.h"
#endif

#ifdef JVS2XBOX360
    #include "USB_XBOX360/usb_xbox360.h"
#endif

#ifdef JVS2XBOXONE
    #include "USB_XBOXONE/usb_xboxone.h"
#endif
    
#ifdef JVS2KEYBOARD
    #include "USB_KEYBOARD/usb_keyboard.h"
#endif

#endif