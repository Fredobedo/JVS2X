#ifndef JVSHOSTHELPER_H_
#define JVSHOSTHELPER_H_


#if (TARGET==JVS2PS3)
    #include "JVS_HOST_HELPER/jvs_host_helper_controller.h"
    #define JVSHOSTHELPER JvsHostHelperController 
#elif (TARGET==JVS2XBOX360)
    #include "JVS_HOST_HELPER/jvs_host_helper_controller.h"
    #define JVSHOSTHELPER JvsHostHelperController         
#elif (TARGET==JVS2XBOXONE)
    #include "JVS_HOST_HELPER/jvs_host_helper_controller.h"
    #define JVSHOSTHELPER JvsHostHelperController 
#elif (TARGET==JVS2KEYBOARD)
    #include "JVS_HOST_HELPER/jvs_host_helper_keyboard.h"
    #define JVSHOSTHELPER JvsReportParserKeyboard 
#endif


#endif