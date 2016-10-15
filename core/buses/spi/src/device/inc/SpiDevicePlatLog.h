#ifndef _SPIDEVICEPLATLOG_H_
#define _SPIDEVICEPLATLOG_H_
/*
===========================================================================

FILE:   SpiDevicePlatLog.h

DESCRIPTION:
    This file contains the API for the platform services.

===========================================================================

        Edit History

$Header: //components/rel/core.adsp/2.2/buses/spi/src/device/inc/SpiDevicePlatLog.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
07/14/12 ag      FIXED EA specific compilation issues.
04/20/12 ddk    Added updates to enable logging.
09/26/11 ag		Created (Based on I2c implementation by LK)

===========================================================================
        Copyright c 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiDevicePlatSvc.h"
#include "SpiLog.h"

// MODULE NAMES NEED TO BE PREDEFINED
// LOG LEVEL NAMES NEED TO BE PREDEFINED

// THIS SECTION CAN BE PLACED IN THE SPECIFIC
// SECTION OF THE LOGGER SINCE MODULE NAMES
// AND LOG LEVELS CAN BE DEFINED OUT OF THE COMMON
// LOGGING API

// Every module name used needs to be in the list

// MODULES


#define LOG_LEVEL_1
#define LOG_LEVEL_2

#define QUP_HAL_DBG

// thise will be generated for undefined MODULES

#ifndef LOG_LEVEL_1
   #define IBUSLOG_LEVEL_1(...)
#else
   #define IBUSLOG_LOG_LEVEL_1(uDataCnt,...)\
       IBUSLOG_PRINTFN(1,uDataCnt,__VA_ARGS__)
#endif

#ifndef LOG_LEVEL_2
   #define IBUSLOG_LOG_LEVEL_2(...)
#else
   #define IBUSLOG_LOG_LEVEL_2(uDataCnt,...)\
       IBUSLOG_PRINTFN(2,uDataCnt,__VA_ARGS__)
#endif

#ifndef LOG_LEVEL_3
   #define IBUSLOG_LOG_LEVEL_3(...)
#else
   #define IBUSLOG_LOG_LEVEL_3(uDataCnt,...)\
       IBUSLOG_PRINTFN(3,uDataCnt,__VA_ARGS__)
#endif

#ifndef LOG_LEVEL_4
   #define IBUSLOG_LOG_LEVEL_4(...)
#else
   #define IBUSLOG_LOG_LEVEL_4(uDataCnt,args...)\
       IBUSLOG_PRINTFN(4,uDataCnt,__VA_ARGS__)
#endif

#ifndef LOG_LEVEL_5
   #define IBUSLOG_LOG_LEVEL_5(...)
#else
   #define IBUSLOG_LOG_LEVEL_5(uDataCnt,...)\
       IBUSLOG_PRINTFN(5,uDataCnt,__VA_ARGS__)
#endif


#ifndef QUP_HAL_DBG
   #define IBUSLOG_QUP_HAL_DBG(...)
#else
   #define IBUSLOG_QUP_HAL_DBG(LOG_LEVEL, ...)\
              IBUSLOG##LOG_LEVEL(__VA_ARGS__)
#endif

#define IBUSLOG(MODULE_NAME,LOG_LEVEL, ... ) \
   IBUSLOG_##MODULE_NAME(_##LOG_LEVEL,__VA_ARGS__)




#endif //_SPIDEVICEPLATLOG_H_
