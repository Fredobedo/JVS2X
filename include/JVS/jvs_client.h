#ifndef JVSCLIENT_H_
#define JVSCLIENT_H_

    #if (TARGET==JVS2PS3)
        #include "JVS/jvs_client_controller.h"
    #elif (TARGET==JVS2KEYBOARD)
        #include "JVS/jvs_client_keyboard.h"
    #endif

#endif