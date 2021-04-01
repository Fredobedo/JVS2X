#ifndef USB_H_
#define USB_H_

#if (TARGET==JVS2PS3)
    #include "USB_HID/usb_ps3.h"
#elif (TARGET==JVS2XBOX360)
    #include "USB_HID/usb_xbox360.h"
#elif (TARGET==JVS2XBOXONE)
    #include "USB_HID/usb_xboxone.h"
#elif (TARGET==JVS2KEYBOARD)
    #include "USB_HID/usb_keyboard.h"
#endif

#endif