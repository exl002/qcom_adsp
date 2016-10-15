/*=============================================================================

  FILE:   DALI2CFWK.C

  OVERVIEW: This file implements a framework I2C DeviceDriver.
 
            Copyright c 2008, 2009, 2010 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary
  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/buses/i2c/src/drv/I2cDriverDalFwk.c#1 $
  $DateTime: 2013/04/03 17:22:53 $$Author: coresvc $
 
  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  02/22/10 UR     Checking for return value of DALSYS_Malloc before doing a
                  DALSYS_memset.
  02/02/10 UR     Changed Bus Acquire Wait Time resolution to milliseconds
  06/29/08 PS     Created

  ===========================================================================*/
 

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

#include "I2cDriverDalFwk.h"
#include "I2cDriver.h"


/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

#define  I2C_GET_DRV_DEVICE(h) (((I2CClientCtxt *)((h)->pClientCtxt))->pI2CDevCtxt->hDevDrv)

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

DALResult
I2C_DalI2C_Attach
(
   const char *,
   DALDEVICEID,
   DalDeviceHandle **
);


/*-------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ----------------------------------------------------------------------*/

static uint32
I2C_DalI2C_AddRef
(
   DalI2CHandle* h
)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

/** @brief Detaches from the device.

    @param[in] h Pointer to dal device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static uint32
I2C_DalI2C_Detach
(
   DalDeviceHandle* h
)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   if( 0 == dwref ) {
      /* Release the client ctxt*/
      DALSYS_Free(h->pClientCtxt);
   }
   return dwref;
}

/** @brief Initializes the i2c device.

    @param[in] h Pointer to dal device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Init
(
   DalDeviceHandle *h
)
{
   I2CClientCtxt *pClientCtxt = (I2CClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pI2CDevCtxt->DevId,
	                           pClientCtxt->pI2CDevCtxt->hProp);

   return I2CDRV_AddDevice(pClientCtxt->pI2CDevCtxt->DevId, &I2C_GET_DRV_DEVICE(h));
}

/** @brief Deinitializes the i2c device.

    @param[in] h Pointer to dal device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_DeInit
(
   DalDeviceHandle *h
)
{

   return DAL_SUCCESS;

}

/** @brief Sends a power event the i2c device.

    @param[in] h           Pointer to dal device handle.
    @param[in] PowerCmd    Power cmd.
    @param[in] PowerDomain Power domain.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_PowerEvent
(
   DalDeviceHandle *h,
   DalPowerCmd PowerCmd,
   DalPowerDomain PowerDomain 
)
{
   return DAL_SUCCESS;
}

/** @brief Opens the handle to the i2c device.

    @param[in] h           Pointer to dal device handle.
    @param[in] mode        Open mode.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Open
(
   DalDeviceHandle* h,
   uint32           mode
)
{
   return I2CDRV_Open(I2C_GET_DRV_DEVICE(h), mode);
}

/** @brief Closes the handle to the i2c device.

    @param[in] h           Pointer to dal device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Close
(
   DalDeviceHandle* h
)
{
   return I2CDRV_Close(I2C_GET_DRV_DEVICE(h));
}

/** @brief Opens the handle to the i2c device.

    @param[in]  h           Pointer to dal device handle.
    @param[out] pInfo       Pointer to info structure.
    @param[in]  uInfoSize   Info size.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Info
(
   DalDeviceHandle *h,
   DalDeviceInfo   *pInfo,
   uint32           uInfoSize
)
{
   pInfo->Version = DALI2C_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

/** @brief Sends a sys request to the i2c device.

    @param[in]   h               Pointer to dal device handle.
    @param[out]  ReqIdx          Request index.
    @param[in]   pSrcBuf         Pointer to src buffer.
    @param[in]   uSrcBufLen      Src buffer length.
    @param[out]  pDestBuf        Pointer to dest buffer.
    @param[out]  uDestBufLen     Dest buffer length.
    @param[out]  puDestBufLenReq Dest buffer length req.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_SysRequest
(
   DalDeviceHandle  *h,
   DalSysReq         ReqIdx,
   const void       *pSrcBuf,
   uint32            uSrcBufLen,
   void             *pDestBuf,
   uint32            uDestBufLen,
   uint32           *puDestBufLenReq
)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalI2C Interface.
------------------------------------------------------------------------------*/

/** @brief Performs a synchronous read transfer on the i2c
           device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pTransfer       Pointer to transfer structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] puNumCompleted  Pointer to number of completed
                                transfers.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Read
(
   DalDeviceHandle  *h,
   I2cTransfer      *pTransfer,
   I2cClientConfig  *pClntCfg,
   uint32           *puNumCompleted
)
{
   return I2CDRV_Read(I2C_GET_DRV_DEVICE(h), pTransfer, pClntCfg, puNumCompleted);
}

/** @brief Performs a write transfer on the i2c device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pTransfer       Pointer to transfer structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] puNumCompleted  Pointer to number of complete
                                transfers.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_Write
(
   DalDeviceHandle  *h,
   I2cTransfer      *pTransfer,
   I2cClientConfig  *pClntCfg,
   uint32           *puNumCompleted
)
{
   return I2CDRV_Write(I2C_GET_DRV_DEVICE(h), pTransfer, pClntCfg, puNumCompleted);
}

/** @brief Opens the handle to the i2c device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pSequence       Pointer to sequence structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] pIoRes          Pointer to IO result.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_BatchTransfer
(
   DalDeviceHandle *h,
   I2cSequence     *pSequence,
   I2cClientConfig *pClntCfg,
   I2cIoResult     *pIoRes
)
{
   return I2CDRV_BatchTransfer( I2C_GET_DRV_DEVICE(h), pSequence, pClntCfg, pIoRes);
}

/** @brief Performs an asynchronous read transfer on the i2c
           device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pTransfer       Pointer to transfer structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] pIoRes          Pointer to io result.
    @param[out] pCallbackFn     Pointer to callback function.
    @param[out] pArg            Pointer to arg passed to 
                                callback function.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_AsyncRead
(
   DalDeviceHandle *h,
   I2cTransfer     *pTransfer,
   I2cClientConfig *pClntCfg,
   I2cIoResult     *pIoRes,
   DDIDRV_ASYNC_CB  pCallbackFn,
   void            *pArg
)
{
   return I2CDRV_AsyncRead(I2C_GET_DRV_DEVICE(h),
                           pTransfer,
                           pClntCfg,
                           pIoRes,
                           pCallbackFn,
                           pArg);
}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pTransfer       Pointer to transfer structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] pIoRes          Pointer to io result.
    @param[out] pCallbackFn     Pointer to callback function.
    @param[out] pArg            Pointer to arg passed to 
                                callback function.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_AsyncWrite
(
   DalDeviceHandle  *h,
   I2cTransfer      *pTransfer,
   I2cClientConfig  *pClntCfg,
   I2cIoResult      *pIoRes,
   DDIDRV_ASYNC_CB   pCallbackFn,
   void             *pArg
)
{
   return  I2CDRV_AsyncWrite(I2C_GET_DRV_DEVICE(h),
                             pTransfer,
                             pClntCfg,
                             pIoRes,
                             pCallbackFn,
                             pArg);
}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  h               Pointer to dal device handle.
    @param[in]  pSequence       Pointer to sequence structure.
    @param[in]  pClntCfg        Pointer to client structure.
    @param[out] pIoRes          Pointer to io result.
    @param[out] pCallbackFn     Pointer to callback function.
    @param[out] pArg            Pointer to arg passed to 
                                callback function.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
static DALResult
I2C_DalI2C_AsyncBatchTransfer
(
   DalDeviceHandle *h,
   I2cSequence     *pSequence,
   I2cClientConfig *pClntCfg,
   I2cIoResult     *pIoRes,
   DDIDRV_ASYNC_CB  pCallbackFn,
   void            *pArg
)
{
   return I2CDRV_AsyncBatchTransfer(I2C_GET_DRV_DEVICE(h),
                                    pSequence,
                                    pClntCfg,
                                    pIoRes,
                                    pCallbackFn,
                                    pArg);
}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  pclientCtxt     Pointer to client context.
    
    @return          Nothing.
  */
static void
I2C_InitInterface
(
   I2CClientCtxt* pclientCtxt
)
{
    static const DalI2C vtbl = {
       {
          I2C_DalI2C_Attach,
          I2C_DalI2C_Detach,
          I2C_DalI2C_Init,
          I2C_DalI2C_DeInit,
          I2C_DalI2C_Open,
          I2C_DalI2C_Close,
          I2C_DalI2C_Info,
          I2C_DalI2C_PowerEvent,
          I2C_DalI2C_SysRequest
        } ,
		I2C_DalI2C_Read,
		I2C_DalI2C_Write,
		I2C_DalI2C_BatchTransfer,
		I2C_DalI2C_AsyncRead,
		I2C_DalI2C_AsyncWrite,
		I2C_DalI2C_AsyncBatchTransfer
    };
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalI2CHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalI2CHandle.pVtbl  = &vtbl;
    pclientCtxt->DalI2CHandle.pClientCtxt = pclientCtxt;

}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  pDalDrvCtxt     Pointer to dal driver context.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
int
I2C_DalI2C_DriverInit
(
   I2CDrvCtxt *pDalDrvCtxt
)
{
   return I2CDRV_Init();
}

/** @brief Performs an asynchronous write transfer on the i2c
           device.

    @param[in]  pDalDrvCtxt     Pointer to dal driver context.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
int
I2C_DalI2C_DriverDeInit
(
   I2CDrvCtxt *pDalDrvCtxt
)
{
   return I2CDRV_DeInit();
}

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Attaches to the device and creates a dal device
           handle.

    @param[in] pszArg       Char string.
    @param[in] DeviceId     Platform device id.
    @param[out] phDalDevice Pointer to pointer to device handle.
    
    @return          DAL_SUCCESS if successful, error otherwise.
  */
DALResult
I2C_DalI2C_Attach
(
   const char       *pszArg,
   DALDEVICEID       DeviceId,
   DalDeviceHandle **phDalDevice
)
{
  DALResult nErr;
  static I2CDrvCtxt *pDrvCtxt = NULL;
  uint32         uDrvCtxtSize;
  

  I2CClientCtxt *pclientCtxt =  NULL;

  if (NULL == pDrvCtxt) {
     /*
     nErr = DALSYS_GetDALPropertyHandleStr("/dev/buses/i2c", hPropI2C);
     if (nErr != DAL_SUCCESS) {
        return nErr;
     }
     nErr = DALSYS_GetPropertyValue(hPropI2C,"SW_DRV_NUM_DEVICES",0,&prop);
     if (nErr != DAL_SUCCESS) {
        return nErr;
     }
     */

     
     if ( 0 == uNumI2cBuses ) {
         return DAL_ERROR;
     }
     uDrvCtxtSize = sizeof(I2CDrvCtxt) + (uNumI2cBuses - 1) *
     sizeof(I2CDevCtxt);
     nErr = DALSYS_Malloc(uDrvCtxtSize, (void **)&pDrvCtxt);

     if ( nErr != DAL_SUCCESS ) {
        return nErr;
     }

     if ( NULL == pDrvCtxt ) {
        return DAL_ERROR;
     }
     DALSYS_memset(pDrvCtxt, 0, uDrvCtxtSize);
     pDrvCtxt->I2CDALVtbl.I2C_DriverInit = I2C_DalI2C_DriverInit;
     pDrvCtxt->I2CDALVtbl.I2C_DriverDeInit = I2C_DalI2C_DriverDeInit;
     pDrvCtxt->dwNumDev = uNumI2cBuses;
     pDrvCtxt->dwSizeDevCtxt = sizeof(I2CDevCtxt);

  }

  nErr = DALSYS_Malloc(sizeof(I2CClientCtxt), (void **)&pclientCtxt);

  *phDalDevice = NULL;

  if ((DAL_SUCCESS == nErr) && (NULL != pclientCtxt)) {
    DALSYS_memset(pclientCtxt,0,sizeof(I2CClientCtxt));

    nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)pDrvCtxt,
        (DALClientCtxt *)pclientCtxt);
    if (DAL_SUCCESS == nErr)
    {
      I2C_InitInterface(pclientCtxt);
      I2C_DalI2C_AddRef(&(pclientCtxt->DalI2CHandle));
      *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalI2CHandle);
    }
  }
  return nErr;
}

