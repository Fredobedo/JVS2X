#ifndef JVSREPORTPARSER_H_
#define JVSREPORTPARSER_H_

    #if (TARGET==JVS2PS3)
        #include "JVS/jvs_report_parser_controller.h"
    #elif (TARGET==JVS2KEYBOARD)
        #include "JVS/jvs_report_parser_keyboard.h"
    #endif

#endif