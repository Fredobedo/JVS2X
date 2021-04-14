#ifndef JVSHOSTHELPER_H_
#define JVSHOSTHELPER_H_


#ifdef JVS2PS3
    #include "JVS_HOST_HELPER/jvs_host_helper_controller.h"
    #define JVSHOSTHELPER JvsHostHelperController 
#endif

#ifdef JVS2XBOX360
    #include "JVS_HOST_HELPER/jvs_host_helper_controller.h"
    #define JVSHOSTHELPER JvsHostHelperController  
#endif

#ifdef JVS2XBOXONE
    #include "JVS_HOST_HELPER/jvs_host_helper_controller.h"
    #define JVSHOSTHELPER JvsHostHelperController 
#endif

#ifdef JVS2KEYBOARD
    #include "JVS_HOST_HELPER/jvs_host_helper_keyboard.h"
    #define JVSHOSTHELPER JvsHostHelperKeyboard 
#endif


#endif