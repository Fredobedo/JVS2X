#ifndef JVSCONTROLLERCONFIG_H_
#define JVSCONTROLLERCONFIG_H_


#ifdef JVS2PS3
    #include "USB_PS3/jvs_controller_ps3_config.h"
#endif

#ifdef JVS2XBOX360
    #include "USB_XBOX360/jvs_controller_xbox360_config.h"
#endif

#ifdef JVS2XBOXONE
    #include "USB_XBOXONE/jvs_controller_xboxone_config.h"
#endif

#ifdef JVS2KEYBOARD
    #include "USB_KEYBOARD/jvs_keyboard_config.h"    
#endif


#endif