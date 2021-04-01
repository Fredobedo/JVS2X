#ifndef JVSPARSERCONFIG_H_
#define JVSPARSERCONFIG_H_


#if (TARGET==JVS2PS3)
    #include "JVS_PARSER/jvs_report_parser_controller.h"
    #define JVSREPORTPARSER JvsReportParserController 
#elif (TARGET==JVS2XBOX360)
    #include "JVS_PARSER/jvs_report_parser_controller.h"
    #define JVSREPORTPARSER JvsReportParserController         
#elif (TARGET==JVS2XBOXONE)
    #include "JVS_PARSER/jvs_report_parser_controller.h"
    #define JVSREPORTPARSER JvsReportParserController 
#elif (TARGET==JVS2KEYBOARD)
    #include "JVS_PARSER/jvs_report_parser_keyboard.h"
    #define JVSREPORTPARSER JvsReportParserKeyboard 
#endif


#endif