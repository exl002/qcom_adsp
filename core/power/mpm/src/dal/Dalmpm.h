#ifndef DALMPM_H
#define DALMPM_H
/*==============================================================================

FILE:      Dalmpm.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright(c) 2013 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALFramework.h"
#include "DDImpm.h"

/*------------------------------------------------------------------------------
Declaring a "mpm" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct mpmDrvCtxt mpmDrvCtxt;
typedef struct mpmDevCtxt mpmDevCtxt;
typedef struct mpmClientCtxt mpmClientCtxt;

/*------------------------------------------------------------------------------
Declaring a private "mpm" Vtable
------------------------------------------------------------------------------*/
typedef struct mpmDALVtbl mpmDALVtbl;
struct mpmDALVtbl
{
  int (*mpm_DriverInit)(mpmDrvCtxt *);
  int (*mpm_DriverDeInit)(mpmDrvCtxt *);
};

struct mpmDevCtxt
{
  //Base Members
  uint32   dwRefs;                                    
  DALDEVICEID DevId;                                  
  uint32   dwDevCtxtRefIdx; 
  mpmDrvCtxt  *pmpmDrvCtxt;                             
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32 Reserved[16];
  //mpm Dev state can be added by developers here
};

struct mpmDrvCtxt
{
  //Base Members
  mpmDALVtbl mpmDALVtbl;
  uint32  dwNumDev;
  uint32  dwSizeDevCtxt;
  uint32  bInit;
  uint32  dwRefs;
  mpmDevCtxt mpmDevCtxt[1];
  //mpm Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "mpm" Client Context
------------------------------------------------------------------------------*/
struct mpmClientCtxt
{
  //Base Members
  uint32  dwRefs;                     
  uint32  dwAccessMode;  
  void *pPortCtxt;
  mpmDevCtxt *pmpmDevCtxt;            
  DalmpmHandle DalmpmHandle; 
  //mpm Client state can be added by developers here
};

DALResult mpm_DriverInit(mpmDrvCtxt *);
DALResult mpm_DriverDeInit(mpmDrvCtxt *);
DALResult mpm_DeviceInit(mpmClientCtxt *);
DALResult mpm_DeviceDeInit(mpmClientCtxt *);
DALResult mpm_Reset(mpmClientCtxt *);
DALResult mpm_PowerEvent(mpmClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult mpm_Open(mpmClientCtxt *, uint32);
DALResult mpm_Close(mpmClientCtxt *);
DALResult mpm_Info(mpmClientCtxt *,DalDeviceInfo *, uint32);
DALResult mpm_InheritObjects(mpmClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*------------------------------------------------------------------------------
Functions specific to Dalmpm interface
------------------------------------------------------------------------------*/
DALResult mpm_ConfigInt( mpmClientCtxt *, uint32 , mpmint_detect_type , mpmint_polarity_type );
DALResult mpm_ConfigWakeup( mpmClientCtxt *, uint32 );
DALResult mpm_ConfigGpioWakeup( mpmClientCtxt *, uint32 , mpmint_detect_type , mpmint_polarity_type );
DALResult mpm_DisableWakeup( mpmClientCtxt *, uint32 );
DALResult mpm_DisableGpioWakeup( mpmClientCtxt *, uint32 );
DALResult mpm_GetNumMappedInterrupts( mpmClientCtxt *, uint32 * );
DALResult mpm_MapInterrupts( mpmClientCtxt *, mpmint_config_info_type *, uint32 , uint32 * );
DALResult mpm_SetupInterrupts( mpmClientCtxt *, mpmint_config_info_type *, uint32 );

#endif /*DALmpm_H*/


