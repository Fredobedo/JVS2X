#ifndef JVSCONTROLLERCONFIG_H_
#define JVSCONTROLLERCONFIG_H_


#if (TARGET==JVS2PS3)
    #include "JVS_PARSER/jvs_controller_ps3_config.h"
#elif (TARGET==JVS2XBOX360)
    #include "JVS_PARSER/jvs_controller_xbox360_config.h"
#elif (TARGET==JVS2XBOXONE)
    #include "JVS_PARSER/jvs_controller_xboxone_config.h"
#endif


#endif