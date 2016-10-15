#ifndef I2CDRIVERDALFWK_H
#define I2CDRIVERDALFWK_H
/*=============================================================================

  @file   I2cDriverDalFwk.h

          This file has the API for I2C driver.
 
          Copyright (c) 2011,212 Qualcomm Technologies Incorporated.
          All Rights Reserved.
          Qualcomm Confidential and Proprietary

  ===========================================================================*/

/* $Header: //components/rel/core.adsp/2.2/buses/i2c/src/drv/I2cDriverDalFwk.h#1 $ */


/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "DALFramework.h"
#include "DALSys.h"
#include "ddii2c.h"
#include "I2cDriver.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

typedef struct I2CDrvCtxt I2CDrvCtxt;
typedef struct I2CDevCtxt I2CDevCtxt;
typedef struct I2CClientCtxt I2CClientCtxt;

typedef struct I2CDALVtbl I2CDALVtbl;
struct I2CDALVtbl
{
  int (*I2C_DriverInit)(I2CDrvCtxt *);
  int (*I2C_DriverDeInit)(I2CDrvCtxt *);
};

struct I2CDevCtxt
{
  //Base Members
  uint32   dwRefs;                                    
  DALDEVICEID DevId;                                  
  uint32   dwDevCtxtRefIdx; 
  I2CDrvCtxt  *pI2CDrvCtxt;                             
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];
  //I2C Dev state can be added by developers here
  
  /* handle to the implementation of the driver */
  I2CDRV_HANDLE hDevDrv;
};
/* Declaring a "I2C" Client Context */
struct I2CClientCtxt
{
  //Base Members
  uint32  dwRefs;                     
  uint32  dwAccessMode;  
  void *pPortCtxt;
  I2CDevCtxt *pI2CDevCtxt;            
  DalI2CHandle DalI2CHandle; 
  /* I2C Client state can be added by developers here */
};

struct I2CDrvCtxt
{
  //Base Members
  I2CDALVtbl I2CDALVtbl;
  uint32  dwNumDev;
  uint32  dwSizeDevCtxt;
  uint32  bInit;
  uint32  dwRefs;
  I2CDevCtxt I2CDevCtxt[1]; // piggy back after this struct as neccessary
  /* I2C Drv state can be added by developers here */
};

extern const uint32      uNumI2cBuses;

#endif /* #ifndef I2CDRIVERDALFWK_H */

