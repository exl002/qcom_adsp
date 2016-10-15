/*=============================================================================

  FILE:   I2cDriver.c

  OVERVIEW:     This file contains the implementation of QUPI2CDriver API
  DEPENDENCIES: I2CDRV_AddDevice must be called first then other api's.
 
                Copyright c 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/buses/i2c/src/drv/I2cDriver.c#1 $
  $DateTime: 2013/04/03 17:22:53 $$Author: coresvc $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  01/22/12 LK     Removed Asynchronous calls and the queue 
                  to optimise transfers.
  09/26/11 LK     Created

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cDriver.h"
#include "I2cError.h"
#include "I2cDevice.h"
#include "DALSys.h"
#include "I2cLog.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

#define QUI2CDRV_REQUEST_QUEUE_SIZE                16


#define I2CDRV_VALIDATE_SYNC_TRAN(hDev,pTransfer,pClntCfg,puNumCompleted)\
     ( (NULL == hDev)?           I2CDRV_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pTransfer)?      I2CDRV_ERR_INVALID_TRANSFER_POINTER:     \
       (NULL == puNumCompleted)? I2CDRV_ERR_INVALID_NUMCOMPLETED_POINTER: \
       I2C_RES_SUCCESS                                                    \
     )

#define I2CDRV_VALIDATE_SYNC_SEQ(hDev,pSequence,pClntCfg,puNumCompleted) \
     ( (NULL == hDev)?           I2CDRV_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pSequence)?      I2CDRV_ERR_INVALID_SEQUENCE_POINTER:     \
       (NULL == puNumCompleted)? I2CDRV_ERR_INVALID_NUMCOMPLETED_POINTER: \
       I2C_RES_SUCCESS                                                    \
     )

#define I2CDRV_VALIDATE_ASYNC_TRAN(hDev,pTransfer,pClntCfg,pIoRes,pCallbackFn,pArg)\
     ( (NULL == hDev)?           I2CDRV_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pTransfer)?      I2CDRV_ERR_INVALID_TRANSFER_POINTER:     \
       (NULL == pIoRes)?       I2CDRV_ERR_INVALID_TRAN_RESULT_POINTER:  \
       (NULL == pCallbackFn)?    I2CDRV_ERR_INVALID_CALLBACK_FN_POINTER:  \
       (NULL == pArg)?           I2CDRV_ERR_INVALID_CALLBACK_ARG_POINTER:  \
       I2C_RES_SUCCESS                                                    \
     )
#define I2CDRV_VALIDATE_ASYNC_SEQ(hDev,pSequence,pClntCfg,pIoRes,pCallbackFn,pArg)\
     ( (NULL == hDev)?           I2CDRV_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pSequence)?      I2CDRV_ERR_INVALID_SEQUENCE_POINTER:     \
       (NULL == pIoRes)?       I2CDRV_ERR_INVALID_TRAN_RESULT_POINTER:  \
       (NULL == pCallbackFn)?    I2CDRV_ERR_INVALID_CALLBACK_FN_POINTER:  \
       (NULL == pArg)?           I2CDRV_ERR_INVALID_CALLBACK_ARG_POINTER:  \
       I2C_RES_SUCCESS                                                    \
     )

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

enum  I2CDRV_Error
{
   I2CDRV_ERR_BASE = I2C_RES_ERROR_CLS_I2C_DRV,

   I2CDRV_ERR_FAILED_TO_ALLOCATE_DEVICE,

   I2CDRV_ERR_FAILED_TO_CREATE_DRVDEV_SYNC_HANDLE,

   I2CDRV_ERR_INVALID_DEV_HANDLE,
   I2CDRV_ERR_INVALID_TRANSFER_POINTER,
   I2CDRV_ERR_INVALID_SEQUENCE_POINTER,
   I2CDRV_ERR_INVALID_NUMCOMPLETED_POINTER,
   I2CDRV_ERR_INVALID_TRAN_RESULT_POINTER,
   I2CDRV_ERR_INVALID_CALLBACK_FN_POINTER,
   I2CDRV_ERR_INVALID_CALLBACK_ARG_POINTER,
   I2CDRV_ERR_NOT_IMPLEMENTED,

};


typedef struct I2CDRV_DalEvt
{
   DALSYSEventHandle            hEvt;
   DALSYSEventObj               objEvt;
} I2CDRV_DalEvt;


typedef struct I2CDRV_DalSync
{
   DALSYSSyncHandle             hSync;
   DALSYS_SYNC_OBJECT(objSync);
} I2CDRV_DalSync;


typedef struct I2CDRV_DevType I2CDRV_DevType;
typedef struct I2CDRV_DevType* I2CDRV_PDevType;

struct I2CDRV_DevType
{

   I2CDEV_HANDLE     hDev;
   uint32            uDevId;
   uint32            uOpenedClientCount;

   I2CLOG_DevType   *pDevLog;

   I2CDRV_DalSync    syncDrvDev;
   I2CDRV_DevType*   pNext;
};

/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

static I2CDRV_DevType* pDrvDevHead;


/*-------------------------------------------------------------------------
* Static Function Declarations and Definitions
* ----------------------------------------------------------------------*/


/** @brief Searches for a device driver struct in the list.
  
    @param[in] uDevId    Platform id for the device.
    
    @return             Pointer to dev if found, otherwise NULL.
  */
static I2CDRV_DevType*
I2CDRV_SearchDevice
(
   uint32          uDevId 
)
{
   I2CDRV_DevType * pDevDrv = pDrvDevHead;
   while ( pDevDrv ) {
      if ( pDevDrv->uDevId == uDevId ) {
         return pDevDrv;
      }
      pDevDrv = pDevDrv->pNext;
   }

   return NULL;
}

/** @brief Links a device driver structure in the list.
  
    @param[in] pDevDrv  Pointer to device structure.
    
    @return             Nothing.
  */
static void
I2CDRV_LinkDevice
(
   I2CDRV_DevType* pDevDrv
)
{
   pDevDrv->pNext = pDrvDevHead;
   pDrvDevHead = pDevDrv;
}


/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Adds a device and sets the handle.
  
    @param[in]  uDevId  Pointer to device structure.
    @param[in]  phDev   Pointer to handle.
    
    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_AddDevice
(
   uint32          uDevId,
   I2CDRV_HANDLE   *phDev
)
{
   DALResult     dalRes;
   int32 res = I2C_RES_SUCCESS;
   I2CDRV_PDevType pDevDrv;
   enum I2CDRV_AddDevState
   {
      I2CDRV_AddDevState_Malloc,
      I2CDRV_AddDevState_Inited,
   } eAddDevState;


   pDevDrv = I2CDRV_SearchDevice(uDevId);
   if ( pDevDrv ) {
      *phDev = (I2CDRV_HANDLE *)pDevDrv;
      return I2C_RES_SUCCESS;
   }
   
   dalRes = DALSYS_Malloc(sizeof(I2CDRV_DevType), (void **)&pDevDrv);
   if ( (DAL_SUCCESS != dalRes) || (NULL == pDevDrv) ) {
      return I2CDRV_ERR_FAILED_TO_ALLOCATE_DEVICE;
   }
   DALSYS_memset(pDevDrv, 0x0, sizeof(I2CDRV_DevType));

   pDevDrv->uDevId = uDevId;
   eAddDevState = I2CDRV_AddDevState_Malloc;
   do
   {
      dalRes = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                                 &(pDevDrv->syncDrvDev.hSync),
                                 &(pDevDrv->syncDrvDev.objSync));
      if ( DAL_SUCCESS != dalRes ) {
         res = I2CDRV_ERR_FAILED_TO_CREATE_DRVDEV_SYNC_HANDLE;
         break;
      }

      res = I2CDEV_Init(pDevDrv->uDevId, &pDevDrv->hDev);
      if ( I2C_RES_SUCCESS != res ) {
        return res;
      }
      eAddDevState = I2CDRV_AddDevState_Inited;

      /* Init device logging. */
      I2cLog_Init(pDevDrv->uDevId, &pDevDrv->pDevLog);
      
      I2CDRV_LinkDevice(pDevDrv);
      *phDev = pDevDrv;
   } while ( 0 );

   if ( I2C_RES_SUCCESS != res ) {
      switch ( eAddDevState ) {
         case I2CDRV_AddDevState_Inited:
            I2CDEV_DeInit(pDevDrv->hDev);
         case I2CDRV_AddDevState_Malloc:
            DALSYS_Free(pDevDrv);
      }
   }
      
   return res;
}


/** @brief Initializes the driver.

    This Function Initializes the driver and creates the
    necessary data structures to support other calls into the
    driver.


    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32 I2CDRV_Init(void)
{
   /*
   if (DAL_SUCCESS != 
         DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                           &I2CDRV_DevList.syncDrv.hSync,
                           &I2CDRV_DevList.syncDrv.objSync )) {
      
      return I2CDRV_ERR_FAILED_TO_CREATE_DRV_SYNC_HANDLE;
   }
   */
   return I2C_RES_SUCCESS;
}

/** @brief Deinitializes the device.

    This Function Deinitializes the device and releases
    resources acquired during init.


    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_DeInit(void)
{
   return I2C_RES_SUCCESS;
}

/**
    This function opens the device handle.

    @param[in]  hDev         Device handle.
    @param[in]  dwaccessMode Access mode.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_Open
(
   I2CDRV_HANDLE hDev,
   uint32        dwaccessMode
)
{
   int32 res = I2C_RES_SUCCESS;
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;

   /* add accounting. */
   DALSYS_SyncEnter(pDevDrv->syncDrvDev.hSync);

   if( 0 == pDevDrv->uOpenedClientCount ) {
      res = I2CDEV_SetPowerState(pDevDrv->hDev, I2CDEV_POWER_STATE_2);
   }
   if( I2C_RES_SUCCESS == res ) {
      pDevDrv->uOpenedClientCount++;
   }
 
   DALSYS_SyncLeave(pDevDrv->syncDrvDev.hSync);
 
   return res;
}/* I2CDRV_Open */

/**
    This function closes the device handle.

    @param[in]  hDev    Device handle.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_Close
(
   I2CDRV_HANDLE hDev
)
{
   int32 res = I2C_RES_SUCCESS;
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;

   // add accounting
   DALSYS_SyncEnter(pDevDrv->syncDrvDev.hSync);

   if( 1 == pDevDrv->uOpenedClientCount ) {
      res = I2CDEV_SetPowerState(pDevDrv->hDev, I2CDEV_POWER_STATE_0);
   }
   if( I2C_RES_SUCCESS == res ) {
      pDevDrv->uOpenedClientCount--;
   }
 
   DALSYS_SyncLeave(pDevDrv->syncDrvDev.hSync);
 
   return res;
}/* I2CDRV_Close */

/** @brief Read a buffer from i2c device.

    Read a buffer from i2c device.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] puNumCompleted Pointer to return completed reads.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_Read
(
   I2CDRV_HANDLE                         hDev     ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg , /* if null keep previous. */
   uint32                               *puNumCompleted
)
{
   int32                  res;
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;

   res = I2CDRV_VALIDATE_SYNC_TRAN(hDev, pTransfer, pClntCfg, puNumCompleted);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }
   DALSYS_SyncEnter(pDevDrv->syncDrvDev.hSync);
   res = I2CDEV_Read(pDevDrv->hDev,
                     pTransfer,
                     pClntCfg, 
                     puNumCompleted);
   DALSYS_SyncLeave(pDevDrv->syncDrvDev.hSync);
   return res;
} 

/** @brief Write a buffer to i2c device.

    Write a buffer to i2c device.

    @param[in] hDev            Device handle.
    @param[in] pTransfer       Pointer to transfer data.
    @param[in] pClntCfg        Pointer to Client configuration.
    @param[in] puNumCompleted  Pointer to return completed
          bytes.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2CDRV_Write
(
   I2CDRV_HANDLE                         hDev     ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg , /* if null keep previous. */
   uint32                               *puNumCompleted
)
{
   int32              res;
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;

   res = I2CDRV_VALIDATE_SYNC_TRAN(hDev, pTransfer, pClntCfg, puNumCompleted);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }
   I2C_CALLTRACE_LEVEL2(pDevDrv->pDevLog, 1,
                        "I2CDRV: I2CDRV_BatchTransfer ENTRY pDev=0x%x",
                        pDevDrv);
   DALSYS_SyncEnter(pDevDrv->syncDrvDev.hSync);
   res = I2CDEV_Write(pDevDrv->hDev,
                      pTransfer,
                      pClntCfg, 
                      puNumCompleted);
   DALSYS_SyncLeave(pDevDrv->syncDrvDev.hSync);
   return res;
}

/** @brief Does a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to a sequence of transfer
          data.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] pIoRes         Pointer to io result.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2CDRV_BatchTransfer
(
   I2CDRV_HANDLE                         hDev     ,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg , /* if null keep previous. */
   I2cIoResult                          *pIoRes
)
{
   int32              res;
   I2CDRV_PDevType    pDevDrv = (I2CDRV_PDevType)hDev;

   res = I2CDRV_VALIDATE_SYNC_SEQ(hDev,pSequence,pClntCfg,pIoRes);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }

   I2C_CALLTRACE_LEVEL2(pDevDrv->pDevLog, 1,
                        "I2CDRV: I2CDRV_BatchTransfer ENTRY pDev=0x%x",
                        pDevDrv);
   DALSYS_SyncEnter(pDevDrv->syncDrvDev.hSync);
   res =I2CDEV_BatchTransfer(pDevDrv->hDev,
                              pSequence,
                              pClntCfg, 
                              pIoRes);
   DALSYS_SyncLeave(pDevDrv->syncDrvDev.hSync);
   return res;
}



/** @brief Schedules a buffer read from i2c device.

    Schedules a buffer read from i2c device.
    Once the read is complete or an error occurs
    the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] pIoRes         Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return          I2C_RES_SUCCESS if queuing successful,
                     error otherwise.
  */
int32
I2CDRV_AsyncRead
(
   I2CDRV_HANDLE                         hDev               ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg, /* if null keep previous. */
   I2cIoResult                          *pIoRes             ,
   DRV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg            
)
{
   return I2CDRV_ERR_NOT_IMPLEMENTED;
} 

/** @brief Schedules a buffer write to i2c device.

    Schedules a buffer write to i2c device. Once the write is
    complete or an error occurs the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] pIoRes       Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2CDRV_AsyncWrite
(
   I2CDRV_HANDLE                         hDev               ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg,/* if null keep previous. */
   I2cIoResult                          *pIoRes           ,
   DRV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg            
)
{
   return I2CDRV_ERR_NOT_IMPLEMENTED;
}

/** @brief Schedules a batch of transfers in a sequence.

    Schedulest a batch of transfers in a sequence and returns.
    The callback will be called to notify transfer is done or
    has failed.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to the sequence of
          transfers.
    @param[in] pClntCfg       Pointer to Client configuration.
    @param[in] pIoRes         Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return          I2C_RES_SUCCESS if queuing successful,
                     error otherwise.
  */
int32
I2CDRV_AsyncBatchTransfer
(
   I2CDRV_HANDLE                         hDev               ,
   I2cSequence                          *pSequence          ,
   I2cClientConfig                      *pClntCfg,// if null keep previous
   I2cIoResult                          *pIoRes           ,
   DRV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg            
)
{
   return I2CDRV_ERR_NOT_IMPLEMENTED;
}





