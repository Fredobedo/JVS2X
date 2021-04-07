#include <Arduino.h>
#include "JVS_HOST_HELPER/jvs_host_helper_base.h"
#include "JVS_UART/jvs_uart.h"

JvsHostHelperBase::JvsHostHelperBase(HardwareSerial& serial): 
    JvsUart(serial) { }

void JvsHostHelperBase::uartReadMultipleUnescaped(int nbr)
{
    for(int cp=0; cp < nbr; cp++){
        UART_READ_UNESCAPED();
    }
}

// Check the request status returned by the slave
bool JvsHostHelperBase::checkRequestStatus(char requestStatus)
{
        if(requestStatus == REQUEST_STATUS_NORMAL)
            return true;
        else if(requestStatus == REQUEST_STATUS_COMMAND_UNKNOWN)
            TRACE_P( 1, "Warning, command unknown\n");
        else if(requestStatus == REQUEST_STATUS_SUM_ERROR)
            TRACE_P( 1, "Warning, slave detected a SUM Error\n");
        else if(requestStatus == REQUEST_STATUS_BUSY)
            TRACE_P( 1, "ERROR, slave is too busy, it can't process the command\n");
        return false;
}

//Check the report 
bool JvsHostHelperBase::checkReportCode(char reportCode)
{
    if(reportCode == REPORT_CODE_NORMAL)
        return true;
    else if(reportCode == REPORT_CODE_PARAM_ERROR)
        TRACE_P( 1, "Warning, command parameter error, no return data\n");
    else if(reportCode == REPORT_CODE_PARAM_DATA_ERROR)
        TRACE_P( 1, "Warning, command parameter error, parameter is ignored\n");
    else if(reportCode == REPORT_CODE_BUSY)
        TRACE_P( 1, "ERROR, slave is too busy, it can't process the sub command\n");
    return false;
}
