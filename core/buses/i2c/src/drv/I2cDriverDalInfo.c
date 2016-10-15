/*=============================================================================

  FILE:     DALQupI2cPdInfo.C

  OVERVIEW: This file implements target specific QUP I2C DeviceDriver Info. 
 
            Copyright (c) 2009 - 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary 

  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/buses/i2c/src/drv/I2cDriverDalInfo.c#1 $
  $DateTime: 2013/04/03 17:22:53 $$Author: coresvc $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  25/03/11 LK     Removed gsbi11,gsbi12 since they are dedicated to sps.
  12/03/09 LK     Created.

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "DALReg.h"
#include "DALDeviceId.h"


/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define I2C_MAX_NUM_DEVICES                (12)


/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


DALResult 
I2C_DalI2C_Attach(const char *, DALDEVICEID,DalDeviceHandle **);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*-------------------------------------------------------------------------
 * Static Variable Definitions
 * ----------------------------------------------------------------------*/

static DALDEVICEID DalI2C_DeviceId[I2C_MAX_NUM_DEVICES] =
       {DALDEVICEID_I2C_DEVICE_1, DALDEVICEID_I2C_DEVICE_2,
        DALDEVICEID_I2C_DEVICE_3, DALDEVICEID_I2C_DEVICE_4,
        DALDEVICEID_I2C_DEVICE_5, DALDEVICEID_I2C_DEVICE_6,
	     DALDEVICEID_I2C_DEVICE_7, DALDEVICEID_I2C_DEVICE_8,
	     DALDEVICEID_I2C_DEVICE_9, DALDEVICEID_I2C_DEVICE_10,
	     DALDEVICEID_I2C_DEVICE_11, DALDEVICEID_I2C_DEVICE_12};

/*-------------------------------------------------------------------------
 * Global Data Definitions
 * ----------------------------------------------------------------------*/

const DALREG_DriverInfo
DALI2C_DriverInfo = { I2C_DalI2C_Attach,
                      I2C_MAX_NUM_DEVICES,
                      DalI2C_DeviceId
                    };
const uint32      uNumI2cBuses = I2C_MAX_NUM_DEVICES;


