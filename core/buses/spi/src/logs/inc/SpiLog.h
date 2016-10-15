#ifndef _SPI_LOG_H
#define _SPI_LOG_H

/*
===========================================================================

FILE:   SpiLog.h

DESCRIPTION:
    This file contains functions data structure declarations for SPI logging
    driver
 
          Copyright (c) 20011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary

===========================================================================

        Edit History

$Header: //components/rel/core.adsp/2.2/buses/spi/src/logs/inc/SpiLog.h#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
07/14/12 ag     Include headers common to both EA and DAL environments.
11/02/11 sg     Created

===========================================================================
        Copyright c 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
//#include "comdef.h"a
#include "SpiDeviceTypes.h"
#include <stdlib.h>

extern int32 SpiLog_Init(void);

#define SPI_ERROR_LOG_LEVEL0(...) 

#define SPI_ERROR_LOG_LEVEL1(...) 

#define SPI_ERROR_LOG_LEVEL2(...) 

#define SPI_INFO_LOG_LEVEL0(...) 

#define SPI_INFO_LOG_LEVEL1(...) 

#define SPI_INFO_LOG_LEVEL2(...) 


#endif /* _SPI_LOG_H */

