#ifndef I2CDRIVER_H
#define I2CDRIVER_H
/*=============================================================================

FILE:   I2cDriver.h

        This file contains the API for the I2cDriver
 
        Copyright (c) 2011 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

  ===========================================================================*/
/* $Header: //components/rel/core.adsp/2.2/buses/i2c/src/drv/I2cDriver.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cTransfer.h"
#include "I2cError.h"


/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

typedef void (*DRV_ASYNC_CB)(void * pArg);

typedef void*     I2CDRV_HANDLE;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

/** @brief Initializes the driver.

    This Function Initializes the driver and creates the
    necessary data structures to support other calls into the
    driver.


    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_Init
(
   void
);

/** @brief Deinitializes the device.

    This Function Deinitializes the device and releases
    resources acquired during init.


    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_DeInit
(
   void
);

/** @brief Adds a device and sets the handle.
  
    @param[in]  uDevId  Pointer to device structure.
    @param[in]  phDev   Pointer to handle.
    
    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_AddDevice
(
   uint32           uDevId,
   I2CDRV_HANDLE   *phDev
);

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
   I2CDRV_HANDLE pDrvDev,
   uint32        dwaccessMode
);

/**
    This function is the main device open  function.

    @param[in]  hDev    Device handle.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2CDRV_Close
(
   I2CDRV_HANDLE pDrvDev
);

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
   I2CDRV_HANDLE                         hDev      ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg ,// if null keep previous
   uint32                               *uNumCompleted
); 

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
   I2CDRV_HANDLE                         hDev      ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg ,// if null keep previous
   uint32                               *uNumCompleted
);

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
   I2CDRV_HANDLE                         hDev      ,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg ,// if null keep previous
   I2cIoResult                          *pIoRes
);



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
   I2CDRV_HANDLE                         hDev      ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg,// if null keep previous
   I2cIoResult                          *pIoRes           ,
   DRV_ASYNC_CB                          pCallbackFn         ,
   void                                 *pArg            
); 

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
   I2CDRV_HANDLE                         hDev      ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg,// if null keep previous
   I2cIoResult                          *pIoRes             ,
   DRV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg            
);

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
   I2CDRV_HANDLE                         hDev      ,
   I2cSequence                          *pSequence          ,
   I2cClientConfig                      *pClntCfg,// if null keep previous
   I2cIoResult                          *pIoRes             ,
   DRV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg            
);


#endif /* #ifndef I2CDRIVER_H */
