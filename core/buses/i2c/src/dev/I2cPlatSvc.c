/*=============================================================================

  FILE:   I2cPlatSvc.c

  OVERVIEW: This file contains the implementation for the platform services.
 
          Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated.
          All Rights Reserved.
          Qualcomm Confidential and Proprietary 

=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //components/rel/core.adsp/2.2/buses/i2c/src/dev/I2cPlatSvc.c#5 $
$DateTime: 2013/08/22 22:11:42 $$Author: coresvc $
When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/20/13 LK     Added uAppClkFreqKhz property reading.
07/29/13 MS     Added PNOC voting upon target init and deinit.
02/22/13 LK     Fixed platform id not being set and added a debug logging function.
09/26/11 LK     Created
=============================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cPlatSvc.h"
#include "I2cPlatBam.h"
#include "I2cSys.h"
#include "I2cError.h"
#include "I2cLog.h"

#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDIClock.h"
#include "DDIHWIO.h"

#include "DDIInterruptController.h"

#include "bam.h"
#include "DDITlmm.h"

#include "icbarb.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QUP_FREQ_HZ                            19200000

#define I2C_DEVICE_PLATBAM_MAX_BAM_THRESHOLD (32*1024)
#define I2C_DEVICE_PLATBAM_MAX_DESC_SIZE     0x100
#define I2C_DEVICE_PLATBAM_MAX_COMMAND_SIZE  0x100

#define I2C_ICB_CLIENT_CNT          1
#define I2C_PNOC_MSTRSLV_PAIRS_NUM	1

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum I2CPLATSVC_Error
{
   I2CPLATDEV_ERROR_BASE = I2C_RES_ERROR_CLS_DEV_PLATFORM,

   I2CPLATSVC_ERROR_DAL_GET_PROPERTY_HANDLE,
   I2CPLATSVC_ERROR_DAL_GET_PROPERTY_VALUE,
   I2CPLATSVC_ERROR_DAL_GET_CHIP_BUS_INDEX_PROPERTY_VALUE,
   I2CPLATSVC_ERROR_ATTACH_TO_CLOCKS,
   I2CPLATSVC_ERROR_DETACH_FROM_CLOCKS,
   I2CPLATSVC_ERROR_ATTACH_TO_DALHWIO,
   I2CPLATSVC_ERROR_DETACH_FROM_DALHWIO,
   I2CPLATSVC_ERROR_GETTING_APPCLK_ID,
   I2CPLATSVC_ERROR_GETTING_HCLK_ID,
   I2CPLATSVC_ERROR_CREATING_DISPATCH_WORKLOOP,
   I2CPLATSVC_ERROR_CREATING_DISPATCH_EVENT,
   I2CPLATSVC_ERROR_ASSOCIATING_EVENT_WITH_WORKLOOP,
   I2CPLATSVC_ERROR_INVALID_POWER_STATE,
   I2CPLATSVC_ERROR_FAILED_TO_SET_APPCLK_FREQ,
   I2CPLATSVC_ERROR_FAILED_TO_ENABLE_APPCLK,
   I2CPLATSVC_ERROR_FAILED_TO_DISABLE_APPCLK,
   I2CPLATSVC_ERROR_FAILED_TO_ENABLE_HCLK,
   I2CPLATSVC_ERROR_FAILED_TO_DISABLE_HCLK,
   I2CPLATSVC_ERROR_FAILED_TO_MAP_BLOCK_HWIO,
   I2CPLATSVC_ERROR_FAILED_ATTACH_TO_IRQCTRL,
   I2CPLATSVC_ERROR_FAILED_TO_REGISTER_IST,
   I2CPLATSVC_ERROR_FAILED_TO_UNREGISTER_IST,
   I2CPLATSVC_ERROR_FAILED_TO_INIT_BAM_HANDLE,
   I2CPLATSVC_ERROR_FAILED_TO_DEINIT_BAM_HANDLE,
   I2CPLATSVC_ERROR_FAILED_TO_INIT_OUTPIPE_HANDLE,
   I2CPLATSVC_ERROR_FAILED_TO_DEINIT_OUTPIPE_HANDLE,
   I2CPLATSVC_ERROR_FAILED_TO_INIT_INPIPE_HANDLE,
   I2CPLATSVC_ERROR_FAILED_TO_DEINIT_INPIPE_HANDLE,
   I2CPLATSVC_ERROR_BAM_PIPE_TRANSFER,
   I2CPLATSVC_ERROR_BAM_PIPE_FULL,
   I2CPLATSVC_ERROR_FAILED_TO_ATTACH_TO_TLMM,
   I2CPLATSVC_ERROR_FAILED_TO_DETTACH_FROM_TLMM,
   I2CPLATSVC_ERROR_FAILED_TO_OPEN_TLMM,
   I2CPLATSVC_ERROR_FAILED_TO_CLOSE_TLMM,
   I2CPLATSVC_ERROR_FAILED_TO_CONFIGURE_GPIO_0,
   I2CPLATSVC_ERROR_FAILED_TO_CONFIGURE_GPIO_1, 
   I2CPLATSVC_ERROR_VIRT_TO_PHYS_ADDR_ERROR,
   I2CPLATSVC_ERROR_FAILED_TO_DISABLE_TX_PIPE,
   I2CPLATSVC_ERROR_FAILED_TO_RE_ENABLE_TX_PIPE,
   I2CPLATSVC_ERROR_FAILED_TO_DISABLE_RX_PIPE,
   I2CPLATSVC_ERROR_FAILED_TO_RE_ENABLE_RX_PIPE,
   I2CPLATSVC_ERROR_FAILED_TO_SET_APP_CLK_FREQ,
   I2CPLATSVC_ERROR_FAILED_TO_CREATE_PNOC_CLIENT,
   I2CPLATSVC_ERROR_FAILED_TO_ISSUE_PNOC_REQ,   

} I2CPLATSVC_Error;

typedef enum I2CPLATDEV_PowerStates
{
   I2CPLATDEV_POWER_STATE_0,/**< Bus and clocks off. */
   I2CPLATDEV_POWER_STATE_1,/**< Bus on clocks off. */
   I2CPLATDEV_POWER_STATE_2,/**< Bus on clocks on. */
} I2CPLATDEV_PowerStates;

typedef enum I2CPLATDEV_BamPipeState
{
   I2CPLATDEV_BAMPIPESTATE_UNINITIALIZED   = 0x0,
   I2CPLATDEV_BAMPIPESTATE_INIT_DONE       = 0x1,
   I2CPLATDEV_BAMPIPESTATE_MEMALLOC_DONE   = 0x2,

   I2CPLATDEV_BAMPIPESTATE_ACTIVE          = 0x10,
   I2CPLATDEV_BAMPIPESTATE_INACTIVE,
   I2CPLATDEV_BAMPIPESTATE_ERROR
} I2CPLATDEV_BamPipeState;

typedef enum I2CPLATDEV_BamState
{
   I2CPLATDEV_BAMSTATE_UNINITIALIZED     = 0x0,
   I2CPLATDEV_BAMSTATE_TARGETINIT_DONE   = 0x1,
   I2CPLATDEV_BAMSTATE_DEINIT_DONE,
   I2CPLATDEV_BAMSTATE_ERROR
} I2CPLATDEV_BamState;


typedef struct I2CPLATDEV_PlatPropertiesType
{
   boolean    bInterruptBased;
   boolean    bDisablePm;
   uint32     uInterruptId;
   uint32     uIrqTriggerCfg;
   boolean    bHasGpios;
   uint32     aGpioCfg[2];
   char      *pDevName;
   char      *pQupHClkName;
   char      *pQupAppClkName;
   uint32     uAppClkFreqKhz;
   uint32     uQupBlockOffset;
   boolean    bBamEnabled;
   uint32     uBamPhysAddr;
   uint32     uBamIrqId;
   uint32     uBamThreshold;
   uint32     uBamInPipeId;
   uint32     uBamOutPipeId;
   boolean    bPnocEnable;   
   uint64     uPnocIBVal;
   uint64     uPnocABVal;
   uint32     uPnocMaster;
   uint32     uPnocSlave;  
} I2CPLATDEV_PlatPropertiesType;

typedef struct I2CPLATDEV_BamPipeCtxt
{
   bam_handle                      hBamPipe;  
   uint32                          bamPipeNum;  
   I2CPLATDEV_BamPipeState         bamPipeState;
   bam_pipe_config_type            bamPipeConfig;
   bam_callback_type               bamPipeCallbackCfg;
   uint32                          uFreeDescCnt;
   I2CPLATBAM_CallbackResultType   bamCbRes;  
   I2CDEVICE_PLATBAM_CB            pfnCb;
   uint16                          uPendingFlags;
   I2CSYS_PhysMemDesc              pipeMemory;
} I2CPLATDEV_BamPipeCtxt;


typedef struct I2CPLATDEV_BamCfgType
{
   bam_handle                      hBam; 
   bam_config_type                 bamConfig;
   I2CPLATDEV_BamState             bamState;
   bam_callback_type               bamCallbackCfg;
   I2CPLATDEV_BamPipeCtxt          bamRxPipeCtxt;  
   I2CPLATDEV_BamPipeCtxt          bamTxPipeCtxt;
   
} I2CPLATDEV_BamCfgType, *I2CPLATDEV_PBamDevCfgType;

typedef struct I2cOs_IrqType
{
   ISR_HOOK IsrHook;
   IST_HOOK IstHook;
   void     *pIsrData;
   void     *pIstData;
   uint32   uInterruptId;
   uint32   uIrqTriggerCfg;
} I2cOs_IrqType;

typedef enum I2CPLATDEV_TargetInitState
{
   I2CPLATDEV_TGT_INIT_NOT_DONE = 0,
   I2CPLATDEV_TGT_INIT_CLK_ALLOCATED  = 0x01,
   I2CPLATDEV_TGT_INIT_HWIO_ALLOCATED = 0x02,
   I2CPLATDEV_TGT_INIT_BAM_INITED     = 0x04,
   I2CPLATDEV_TGT_INIT_TLMM_ALLOCATED = 0x08,
   I2CPLATDEV_TGT_INIT_TLMM_OPENED    = 0x10,
   I2CPLATDEV_TGT_INIT_PNOC_VOTE_DONE = 0x40,
} I2CPLATDEV_TargetInitState;

typedef struct I2CPLATDEV_TargetCfgType
{
  uint32                        uPlatDevId;
  DalDeviceHandle              *pClkHandle;
  ClockIdType                   QupHClkId;
  ClockIdType                   QupAppClkId;
  uint32                        uQupBlockOffset;
  DalDeviceHandle              *phDalHWIO;

  I2cOs_IrqType                 irq;
  DalDeviceHandle              *hInterruptCtrl;
  DalDeviceHandle              *hTlmm;
  I2CPLATDEV_PlatPropertiesType platProps;
  I2CPLATDEV_BamCfgType         bam;

  uint32                        tmpPowerState;//untill clocks are operational
  I2CPLATDEV_TargetInitState    initState;
  npa_client_handle             hNpaClient;
} I2CPLATDEV_TargetCfgType, *I2CPLATDEV_PTargetCfgType;



/*-------------------------------------------------------------------------
 * Static Variable Definitions
 * ----------------------------------------------------------------------*/
static const char                    *pPeriphSsAddrName;
static uint8                         *pPeriphSsBasePtr;



/*-------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ----------------------------------------------------------------------*/

/* There is a circular dependency between I2CPLATDEV_InitBam and
   I2CPLATDEV_BamCallback */
static int32 I2CPLATDEV_InitBam(I2CPLATDEV_TargetCfgType *pDev);


/** @brief Maps the hwio address space.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_InitHwio
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult dalRes;

   if ( NULL == pPeriphSsBasePtr ) {
      dalRes = DAL_DeviceAttach(DALDEVICEID_HWIO, &pDev->phDalHWIO);
      if ( (DAL_SUCCESS != dalRes) || (NULL == pDev->phDalHWIO) ) {
         return I2CPLATSVC_ERROR_ATTACH_TO_DALHWIO;
      }
      pDev->initState |= I2CPLATDEV_TGT_INIT_HWIO_ALLOCATED;
      if ( DAL_SUCCESS !=
             DalHWIO_MapRegion(pDev->phDalHWIO,
                               pPeriphSsAddrName,
                               &pPeriphSsBasePtr) ) {
         return I2CPLATSVC_ERROR_FAILED_TO_MAP_BLOCK_HWIO;
      }
   }
   return I2C_RES_SUCCESS;
}

/** @brief detaches from DALDEVICEID_HWIO.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_DeInitHwio
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult dalRes;

   if ( pDev->initState & I2CPLATDEV_TGT_INIT_HWIO_ALLOCATED ) {
      dalRes = DAL_DeviceDetach(pDev->phDalHWIO);
      if ( DAL_SUCCESS != dalRes ) {
         return I2CPLATSVC_ERROR_DETACH_FROM_DALHWIO;
      }
   }
   return I2C_RES_SUCCESS;
}

/** @brief Initializes the scl,sda gpios.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_InitGpios
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult dalRes;

   if (FALSE == pDev->platProps.bHasGpios) {
      return I2C_RES_SUCCESS;
   }
   dalRes = DAL_DeviceAttachEx(NULL, DALDEVICEID_TLMM,
                               DALTLMM_INTERFACE_VERSION, &pDev->hTlmm);
   if ( (DAL_SUCCESS != dalRes) || (NULL == pDev->hTlmm) ) {
      return I2CPLATSVC_ERROR_FAILED_TO_ATTACH_TO_TLMM;
   }
   pDev->initState |= I2CPLATDEV_TGT_INIT_TLMM_ALLOCATED;

   dalRes = DalDevice_Open(pDev->hTlmm, DAL_OPEN_SHARED);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_OPEN_TLMM;
   }
   pDev->initState |= I2CPLATDEV_TGT_INIT_TLMM_OPENED;

   dalRes = DalTlmm_ConfigGpio(pDev->hTlmm, pDev->platProps.aGpioCfg[0],
                               DAL_TLMM_GPIO_ENABLE);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_CONFIGURE_GPIO_0;
   }

   dalRes = DalTlmm_ConfigGpio(pDev->hTlmm, pDev->platProps.aGpioCfg[1],
                               DAL_TLMM_GPIO_ENABLE);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_CONFIGURE_GPIO_1;
   }

   return I2C_RES_SUCCESS;
}

/** @brief Initializes the scl,sda gpios.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_DeInitGpios
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult dalRes;

   if ( FALSE == pDev->platProps.bHasGpios ) {
      return I2C_RES_SUCCESS;
   }
   if ( pDev->initState & I2CPLATDEV_TGT_INIT_TLMM_OPENED ) {
      dalRes = DalDevice_Close(pDev->hTlmm);
      if ( DAL_SUCCESS != dalRes ) {
         return I2CPLATSVC_ERROR_FAILED_TO_CLOSE_TLMM;
      }
   }
   if ( pDev->initState & I2CPLATDEV_TGT_INIT_TLMM_ALLOCATED ) {
      dalRes = DAL_DeviceDetach(pDev->hTlmm);
      if ( DAL_SUCCESS != dalRes ) {
         return I2CPLATSVC_ERROR_FAILED_TO_DETTACH_FROM_TLMM;
      }
   }

   return I2C_RES_SUCCESS; 
}

/** @brief Initializes the clocks for the qup core.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_InitClocks
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult dalRes;
   I2CPLATDEV_PlatPropertiesType   *pProps;
   uint32                           uSetClkFreq;

   pProps = &pDev->platProps;

   /* Fails to attach to clocks, they are not available */
   dalRes = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &pDev->pClkHandle);
   if ( (DAL_SUCCESS != dalRes) || (NULL == pDev->pClkHandle) ) {
      return I2CPLATSVC_ERROR_ATTACH_TO_CLOCKS;
   }
   pDev->initState |= I2CPLATDEV_TGT_INIT_CLK_ALLOCATED;

   if ( DAL_SUCCESS !=
        DalClock_GetClockId(pDev->pClkHandle,
                            pDev->platProps.pQupAppClkName,
                            &pDev->QupAppClkId) ) {
      return I2CPLATSVC_ERROR_GETTING_APPCLK_ID;
   }

   dalRes = DalClock_SetClockFrequency(pDev->pClkHandle,
                                       pDev->QupAppClkId,
                                       pProps->uAppClkFreqKhz,
                                       CLOCK_FREQUENCY_KHZ_EXACT,
                                       &uSetClkFreq);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_SET_APP_CLK_FREQ;     
   }    
   
   if ( DAL_SUCCESS != 
        DalClock_GetClockId(pDev->pClkHandle,
                            pDev->platProps.pQupHClkName,
                            &pDev->QupHClkId) ) {
      return I2CPLATSVC_ERROR_GETTING_HCLK_ID;
   }
   return I2C_RES_SUCCESS; 
}

/** @brief Deinitializes the clocks for the qup core.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_DeInitClocks
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult dalRes;

   if ( pDev->initState & I2CPLATDEV_TGT_INIT_CLK_ALLOCATED ) {
      dalRes = DAL_DeviceDetach(pDev->pClkHandle);
      if ( DAL_SUCCESS != dalRes ) {
         return I2CPLATSVC_ERROR_DETACH_FROM_CLOCKS;
      }
   }
   return I2C_RES_SUCCESS; 
}

/** @brief Enables the clocks for the qup core.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_EnableClocks
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult                        dalRes;
   
   /* Enable QUP Clock */  
   dalRes = DalClock_EnableClock(pDev->pClkHandle, pDev->QupAppClkId);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_ENABLE_APPCLK;
   }

   /* Enable HCLK */
   dalRes = DalClock_EnableClock(pDev->pClkHandle, pDev->QupHClkId);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_ENABLE_HCLK;
   }

   return I2C_RES_SUCCESS;
}

/** @brief Disables the clocks for the qup core.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_DisableClocks
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult dalRes;

   /* Disable QUP Clock */
   dalRes = DalClock_DisableClock(pDev->pClkHandle, pDev->QupAppClkId);
   if ( DAL_SUCCESS != dalRes ) {
       return I2CPLATSVC_ERROR_FAILED_TO_DISABLE_APPCLK; 
   }

   /* Disable HCLK */
   dalRes = DalClock_DisableClock(pDev->pClkHandle, pDev->QupHClkId);
   if ( DAL_SUCCESS != dalRes ) {
       return I2CPLATSVC_ERROR_FAILED_TO_DISABLE_HCLK;
   }

   return I2C_RES_SUCCESS;
}

/** @brief Tx pipe event callback.
  
    @param[in] bam_result     Pointer to platform device
          structure.
    
    @return             None.
  */
static void
I2CPLATDEV_TxPipeCallback
(
   bam_result_type bam_result
)
{
   I2CPLATDEV_BamPipeCtxt *pPipeCtxt = 
      (I2CPLATDEV_BamPipeCtxt *)(bam_result.cb_data);
 
   if ( NULL == pPipeCtxt ) {
      return;
   }
   switch ( bam_result.event ) {
      case BAM_EVENT_EOT:   
         pPipeCtxt->bamCbRes.bamEvt = I2CPLATBAM_EVT_TX_EOT;
         break;
      case BAM_EVENT_DESC_DONE:
           pPipeCtxt->bamCbRes.bamEvt = I2CPLATBAM_EVT_TX_DESC_DONE;
           break;

      case BAM_EVENT_OUT_OF_DESC:
      case BAM_EVENT_ERROR:
           pPipeCtxt->bamCbRes.bamEvt = I2CPLATBAM_EVT_BAM_ERROR;  
           break;

      default:   /* unhandled event. */
           pPipeCtxt->bamCbRes.bamEvt = I2CPLATBAM_EVT_BAM_ERROR;  
           break;
   }
   if ( NULL != pPipeCtxt->pfnCb ) {
      pPipeCtxt->pfnCb(pPipeCtxt->bamCbRes);
   }
}

/** @brief Rx pipe event callback.
  
    @param[in] bam_result     Pointer to platform device
          structure.
    
    @return             None.
  */
static void
I2CPLATDEV_RxPipeCallback
(
   bam_result_type bam_result
)
{
   I2CPLATDEV_BamPipeCtxt *pPipeCtxt = 
      (I2CPLATDEV_BamPipeCtxt *)(bam_result.cb_data);
   
   if ( NULL == pPipeCtxt ) {
      return;
   }
   switch( bam_result.event ) {
      case BAM_EVENT_EOT:   
           pPipeCtxt->bamCbRes.bamEvt = I2CPLATBAM_EVT_RX_EOT;
           break;
      case BAM_EVENT_DESC_DONE:
           pPipeCtxt->bamCbRes.bamEvt = I2CPLATBAM_EVT_RX_DESC_DONE;
           break;

      case BAM_EVENT_OUT_OF_DESC:
      case BAM_EVENT_ERROR:
           pPipeCtxt->bamCbRes.bamEvt = I2CPLATBAM_EVT_BAM_ERROR;  
           break;

      default:   /* unhandled event. */
           pPipeCtxt->bamCbRes.bamEvt = I2CPLATBAM_EVT_BAM_ERROR;  
           break;
   }
   if (NULL != pPipeCtxt->pfnCb) {
      pPipeCtxt->pfnCb(pPipeCtxt->bamCbRes);
   }
}

/** @brief Platform IST. Used to dispatch common drivers ISR,
           IST.
  
    @param[in] pCtxt    Context passed to the Platform IST.
    
    @return             void pointer.
  */
static void*
I2CPLATDEV_Isr
(
   void *pCtxt
)
{
   I2CPLATDEV_TargetCfgType *pDev = (I2CPLATDEV_TargetCfgType *)pCtxt;
   I2cOs_IrqType  *pIrq;

   if (NULL == pDev) {
      return NULL; 
   }
   pIrq = &pDev->irq;
   pIrq->IsrHook(pIrq->pIsrData);
   pIrq->IstHook(pIrq->pIstData);
   DalInterruptController_InterruptDone(pDev->hInterruptCtrl,pIrq->uInterruptId);
   return NULL;
}


/** @brief Deinitializes bam.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_BamDeInit
(
   I2CPLATDEV_TargetCfgType *pDev
)
{
   int32             txMemRes,rxMemRes;
   bam_status_type   txPipeRes , rxPipeRes,bamRes;
   int32 res = I2C_RES_SUCCESS;
   I2CPLATDEV_BamCfgType      *pBamCfg = &pDev->bam;

   if ( !pDev->platProps.bBamEnabled ) {
      return I2C_RES_SUCCESS; 
   }

   if ( pBamCfg->bamTxPipeCtxt.bamPipeState & I2CPLATDEV_BAMPIPESTATE_MEMALLOC_DONE ) {
      txMemRes = I2CSYS_PhysMemFree(&pBamCfg->bamTxPipeCtxt.pipeMemory);
      if ( txMemRes != I2CSYS_RESULT_OK ) {
        res = txMemRes;
      }
   }
   if ( pBamCfg->bamTxPipeCtxt.bamPipeState & I2CPLATDEV_BAMPIPESTATE_INIT_DONE ) {
      txPipeRes = bam_pipe_deinit(pBamCfg->bamTxPipeCtxt.hBamPipe);
      if ( txPipeRes != BAM_SUCCESS ) {
         res = I2CPLATSVC_ERROR_FAILED_TO_DEINIT_OUTPIPE_HANDLE;
      }
   }
   if ( pBamCfg->bamRxPipeCtxt.bamPipeState & I2CPLATDEV_BAMPIPESTATE_MEMALLOC_DONE ) {
      rxMemRes = I2CSYS_PhysMemFree(&pBamCfg->bamRxPipeCtxt.pipeMemory);
      if ( rxMemRes != I2CSYS_RESULT_OK ) {
         res = rxMemRes;
      }
   }
   if ( pBamCfg->bamRxPipeCtxt.bamPipeState & I2CPLATDEV_BAMPIPESTATE_INIT_DONE ) {   
      rxPipeRes = bam_pipe_deinit(pBamCfg->bamRxPipeCtxt.hBamPipe);
      if ( rxPipeRes != BAM_SUCCESS ) {
         res = I2CPLATSVC_ERROR_FAILED_TO_DEINIT_INPIPE_HANDLE;
      }
   } 
   if ( NULL != pBamCfg->hBam ) {
      bamRes = bam_deinit(pBamCfg->hBam, 0);
      if ( bamRes != BAM_SUCCESS ) {
         res = I2CPLATSVC_ERROR_FAILED_TO_DEINIT_BAM_HANDLE;
      }
   }
     
   return res;
}

/** @brief Bam event callback.
  
    @param[in] bam_result     Pointer to platform device
          structure.
    
    @return             None.
  */
static void
I2CPLATDEV_BamCallback
(
   bam_result_type bam_result
)
{
   I2CPLATDEV_BamCfgType         *pBamCfg;
   I2CPLATDEV_TargetCfgType *pDev = 
      (I2CPLATDEV_TargetCfgType *)(bam_result.cb_data);
  
   
   if ( NULL == pDev ) {
      return;
   }

   pBamCfg = &pDev->bam;

   switch ( bam_result.event ) {
        
        case BAM_EVENT_ERROR: /* this event should not happen. */
             bam_reset(pBamCfg->hBam);
             I2CPLATDEV_InitBam(pDev);
             pBamCfg->bamTxPipeCtxt.bamCbRes.bamEvt
                = I2CPLATBAM_EVT_BAM_ERROR;
             pBamCfg->bamTxPipeCtxt.pfnCb(pBamCfg->bamTxPipeCtxt.bamCbRes);
             break;

        default:   /* unhandled event . */
                break;
   }
}

/** @brief Initializes bam.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32 I2CPLATDEV_InitBam
(
   I2CPLATDEV_TargetCfgType *pDev
)
{
   int32         sysRes;
   int32         res;
   bam_callback_type *bam_cb = NULL;
   bam_config_type *bam_config = NULL;
   bam_pipe_config_type *pipe_cfg = NULL;
   bam_callback_type *pipe_cb = NULL;
   I2CPLATDEV_PlatPropertiesType *pPlatProps = &(pDev->platProps);
   I2CPLATDEV_BamCfgType      *pBamCfg = &pDev->bam;

   
   
   res = I2C_RES_SUCCESS;

   if ( !pDev->platProps.bBamEnabled ) {
      return I2C_RES_SUCCESS; 
   }

   do {

      if ( NULL == pBamCfg->hBam ) {
         bam_config = &(pDev->bam.bamConfig);
         bam_cb = &(pDev->bam.bamCallbackCfg);
            
         bam_config->bam_pa = pPlatProps->uBamPhysAddr;        
         bam_config->bam_va = 0; /* BAM should remap if this is zero*/
         bam_config->bam_irq = pPlatProps->uBamIrqId;        

         bam_config->bam_irq_mask = BAM_IRQ_HRESP_ERR_EN|BAM_IRQ_ERR_EN;
         bam_config->sum_thresh   = I2C_DEVICE_PLATBAM_MAX_BAM_THRESHOLD;        
         bam_config->options      = 0; /* No use, set it to zero for now */
            
         bam_cb->func = I2CPLATDEV_BamCallback;
         bam_cb->data = pDev;

         pBamCfg->hBam = bam_init(bam_config, bam_cb);
         if ( NULL == pBamCfg->hBam ) {
            res = I2CPLATSVC_ERROR_FAILED_TO_INIT_BAM_HANDLE;
            break;
         }
      }
      
      if ( NULL == pBamCfg->bamTxPipeCtxt.hBamPipe ) {
         pBamCfg->bamTxPipeCtxt.bamPipeState = I2CPLATDEV_BAMPIPESTATE_UNINITIALIZED;
         pBamCfg->bamTxPipeCtxt.pipeMemory.uSize = I2C_DEVICE_PLATBAM_MAX_DESC_SIZE;
         sysRes = I2CSYS_PhysMemAlloc(&pBamCfg->bamTxPipeCtxt.pipeMemory);
         if (sysRes != I2CSYS_RESULT_OK) {
            res = sysRes;
            break;
         }
         pBamCfg->bamTxPipeCtxt.bamPipeState = I2CPLATDEV_BAMPIPESTATE_MEMALLOC_DONE;

         pipe_cfg = &(pBamCfg->bamTxPipeCtxt.bamPipeConfig);
         pipe_cb = &(pBamCfg->bamTxPipeCtxt.bamPipeCallbackCfg);

         pipe_cfg->options            = BAM_O_DESC_DONE|BAM_O_EOT|BAM_O_ERROR;
         pipe_cfg->dir                = BAM_DIR_CONSUMER;
         pipe_cfg->mode               = BAM_MODE_SYSTEM;
         pipe_cfg->desc_base_va       = (uint32)pBamCfg->bamTxPipeCtxt.pipeMemory.pVirtAddress;
         pipe_cfg->desc_base_pa       = (uint32)pBamCfg->bamTxPipeCtxt.pipeMemory.pPhysAddress;
         pipe_cfg->desc_size          = I2C_DEVICE_PLATBAM_MAX_DESC_SIZE;
         
         pipe_cb->func = I2CPLATDEV_TxPipeCallback;
         pipe_cb->data = &pBamCfg->bamTxPipeCtxt;

         /* TODO: How to get the pipenum for a particular BAM*/
         pBamCfg->bamTxPipeCtxt.bamPipeNum = pPlatProps->uBamOutPipeId;
         
         pBamCfg->bamTxPipeCtxt.hBamPipe = 
               bam_pipe_init(pBamCfg->hBam, 
                             pBamCfg->bamTxPipeCtxt.bamPipeNum, 
                             pipe_cfg, 
                             pipe_cb);

         if ( NULL == pBamCfg->bamTxPipeCtxt.hBamPipe ) {
            res = I2CPLATSVC_ERROR_FAILED_TO_INIT_BAM_HANDLE;
            break;
         }
         pBamCfg->bamTxPipeCtxt.uFreeDescCnt = bam_pipe_getfreecount(pBamCfg->bamTxPipeCtxt.hBamPipe);

         pBamCfg->bamTxPipeCtxt.bamPipeState |= I2CPLATDEV_BAMPIPESTATE_INIT_DONE;
      }
      if ( NULL == pBamCfg->bamRxPipeCtxt.hBamPipe ) {
         pBamCfg->bamRxPipeCtxt.bamPipeState = I2CPLATDEV_BAMPIPESTATE_UNINITIALIZED;     
         pipe_cfg = &(pBamCfg->bamRxPipeCtxt.bamPipeConfig);
         pipe_cb = &(pBamCfg->bamRxPipeCtxt.bamPipeCallbackCfg);

         pBamCfg->bamRxPipeCtxt.pipeMemory.uSize = I2C_DEVICE_PLATBAM_MAX_DESC_SIZE;
         sysRes = I2CSYS_PhysMemAlloc(&pBamCfg->bamRxPipeCtxt.pipeMemory);
         if (sysRes != I2CSYS_RESULT_OK) {
            res = sysRes;
            break;
         }
         pBamCfg->bamRxPipeCtxt.bamPipeState = I2CPLATDEV_BAMPIPESTATE_MEMALLOC_DONE;

         pipe_cfg->options          = BAM_O_DESC_DONE|BAM_O_EOT|BAM_O_ERROR;
         pipe_cfg->dir              = BAM_DIR_PRODUCER;
         pipe_cfg->mode             = BAM_MODE_SYSTEM;
         pipe_cfg->desc_base_va     = (uint32)pBamCfg->bamRxPipeCtxt.pipeMemory.pVirtAddress;
         pipe_cfg->desc_base_pa     = (uint32)pBamCfg->bamRxPipeCtxt.pipeMemory.pPhysAddress;
         pipe_cfg->desc_size        = I2C_DEVICE_PLATBAM_MAX_DESC_SIZE;
         
         pipe_cb->func = I2CPLATDEV_RxPipeCallback;
         pipe_cb->data = &pBamCfg->bamRxPipeCtxt;

         /* TODO: How to get the pipenum for a particular BAM*/
         pBamCfg->bamRxPipeCtxt.bamPipeNum = pPlatProps->uBamInPipeId;
            
         pBamCfg->bamRxPipeCtxt.hBamPipe = 
               bam_pipe_init(pBamCfg->hBam, 
                             pBamCfg->bamRxPipeCtxt.bamPipeNum, 
                             pipe_cfg, 
                             pipe_cb);

         if ( NULL == pBamCfg->bamRxPipeCtxt.hBamPipe ) {
            res = I2CPLATSVC_ERROR_FAILED_TO_INIT_INPIPE_HANDLE;
            break;
         }
         pBamCfg->bamRxPipeCtxt.uFreeDescCnt = bam_pipe_getfreecount(pBamCfg->bamRxPipeCtxt.hBamPipe);
         pBamCfg->bamRxPipeCtxt.bamPipeState |= I2CPLATDEV_BAMPIPESTATE_INIT_DONE;         
      }
   } while ( 0 );

   if ( res != I2C_RES_SUCCESS ) {
      I2CPLATDEV_BamDeInit(pDev);
   }
   else {
      pDev->initState |= I2CPLATDEV_TGT_INIT_BAM_INITED;
   }   

   return res;
}

//static void I2CPLATDEV_LogDescriptor
//(
//   I2CPLATDEV_TargetCfgType     *pDev,
//   I2CPLATBAM_BamIoVecType *pBamIoVec
//)
/*
static void I2CPLATDEV_LogDescriptor
(
   I2CPLATDEV_TargetCfgType    *pDev,
   uint8                       *pBuff,
   uint32                       buffSize,
   uint32                       flags,
   int32                        dir
)
{
   static I2CLOG_DevType *pLogDev = NULL;
   //uint32 buffSize;
   //uint32 logedSize;
   //uint8 *pBuff;
   
   if ( NULL == pLogDev ) {
      pLogDev = I2cLog_SearchDevice(pDev->uPlatDevId);
   }
   if ( NULL != pLogDev ) {
      I2C_CALLDATA_LEVEL1(pLogDev, 4, "Log Descriptor: addr:0x%x size:0x%x flags:0x%x direction:0x%x",
                          pBuff, buffSize, flags, dir);
      I2cLog_LogBuffer(pLogDev, pBuff, buffSize);
   } 

}
*/

/**
 * @brief Schedule a single BAM transfer
  
   This function schedules a single BAM transfer.
  
   @param[in]  hPlat       Platform BAM handle
   @param[in]  pBamIoVec   Pointer to bam IO vector
   @param[out] pFreeCnt    Pointer to free descriptor count
   @param[in]  pfnCb       Function callback pointer.
   @param[in]  pUser       Arguments passed to the func cb.
  
   @return I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
 */

int32 I2CPLATDEV_PlatBamTransfer
(
   I2CPLATDEV_HANDLE        hPlat,  
   I2CPLATBAM_BamIoVecType *pBamIoVec,
   uint32                  *pFreeCnt,
   I2CDEVICE_PLATBAM_CB     pfnCb,
   void                    *pArg
)
{
   I2CPLATDEV_TargetCfgType     *pDev;
   I2CPLATDEV_BamPipeCtxt       *pPipeCtxt = NULL;
   uint32                        phAddress;
   bam_status_type               bam_res;
   int32                         res;

   pDev = (I2CPLATDEV_TargetCfgType*)hPlat;

   if (  !(pDev->initState & I2CPLATDEV_TGT_INIT_BAM_INITED) ) {
      res = I2CPLATDEV_InitBam(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         return res;     
      }     
   }

   if ( I2CPLATBAM_DIR_OUTPUT == pBamIoVec->eDir ) {
      pPipeCtxt = &(pDev->bam.bamTxPipeCtxt);
   }
   else {
      pPipeCtxt = &(pDev->bam.bamRxPipeCtxt);
   }

   if ( pPipeCtxt->uFreeDescCnt <= 1 ) {
      pPipeCtxt->uFreeDescCnt = bam_pipe_getfreecount(pPipeCtxt->hBamPipe);
   }

   if ( 0 == pPipeCtxt->uFreeDescCnt ) {
      return I2CPLATSVC_ERROR_BAM_PIPE_FULL;
   }
   else if ( 1 == pPipeCtxt->uFreeDescCnt ) {
      /* interrupt if no more space. */
      pPipeCtxt->uPendingFlags |= BAM_IOVEC_FLAG_INT;
   }

   pPipeCtxt->bamCbRes.pArg  = pArg;  
   pPipeCtxt->pfnCb          =  pfnCb;

   pPipeCtxt->uFreeDescCnt--;
   
   /* store flags. */
   pPipeCtxt->uPendingFlags |= pBamIoVec->flags;

   phAddress = I2CSYS_VirtToPhys (pBamIoVec->pBuff, pBamIoVec->buffSize);
   if ( 0 == phAddress ) {
      return I2CPLATSVC_ERROR_VIRT_TO_PHYS_ADDR_ERROR;
   }
   //I2CPLATDEV_LogDescriptor(pDev, (uint8*)phAddress, pBamIoVec->buffSize,
   //                         pBamIoVec->flags,(int32)pBamIoVec->eDir);
   bam_res = bam_pipe_transfer(pPipeCtxt->hBamPipe,
                               phAddress, 
                               pBamIoVec->buffSize, 
                               pBamIoVec->flags, 
                               pPipeCtxt);
   if( BAM_SUCCESS != bam_res ) {
      return I2CPLATSVC_ERROR_BAM_PIPE_TRANSFER;
   }

   *pFreeCnt = pPipeCtxt->uFreeDescCnt;

   pPipeCtxt->bamPipeState = I2CPLATDEV_BAMPIPESTATE_ACTIVE;
     
   return I2C_RES_SUCCESS;
}

/**
   @brief Find Available free space in a BAM pipe
 
   @param[in] hPlat  Platform handle
   @param[in] uDir   Producer or Consumer transfer

   @return Available free space. 
 */
uint32
I2CPLATDEV_BamGetPipeFreeSpace
(
   I2CPLATDEV_HANDLE        hPlat,
   I2CPLATBAM_DirectionType eDir
)
{
   I2CPLATDEV_TargetCfgType     *pDev;
   I2CPLATDEV_BamPipeCtxt       *pPipeCtxt;
   int32                         res;

   

   pDev = (I2CPLATDEV_TargetCfgType*)hPlat;

   if (  !(pDev->initState & I2CPLATDEV_TGT_INIT_BAM_INITED) ) {
      res = I2CPLATDEV_InitBam(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         return res;     
      }     
   }

   if ( eDir == I2CPLATBAM_DIR_OUTPUT ) {
      pPipeCtxt  = &pDev->bam.bamTxPipeCtxt;
   }
   else {
      pPipeCtxt  = &pDev->bam.bamRxPipeCtxt;
   }

   pPipeCtxt->uFreeDescCnt = bam_pipe_getfreecount(pPipeCtxt->hBamPipe);
   return pPipeCtxt->uFreeDescCnt;
}

/** @brief Initializes bam.
  
    @param[in]  uPlatDevId  Platform device ID.
    @param[out] pDev        Pointer to platform device
          structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_ReadPlatConfig
(
   uint32                    uPlatDevId,
   I2CPLATDEV_TargetCfgType *pDev
)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar               PropVar;
   int32 result;

   if ( DAL_SUCCESS != DALSYS_GetDALPropertyHandle(uPlatDevId, hProp ) ) {
      return I2CPLATSVC_ERROR_DAL_GET_PROPERTY_HANDLE;
   }

   result = I2CPLATSVC_ERROR_DAL_GET_PROPERTY_VALUE; /* set the error. */
   do {
      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "SW_USE_POLLING_MODE", 0, &PropVar) ) {
         break;
      }
      pDev->platProps.bInterruptBased = ((uint32)PropVar.Val.dwVal == 1)? 0: 1;

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "SW_DISABLE_PM", 0, &PropVar) ) {
         break;
      }
      pDev->platProps.bDisablePm = (uint32)PropVar.Val.dwVal;
	  
      if ( DAL_SUCCESS ==
           DALSYS_GetPropertyValue(hProp, "I2C_PNOC_VOTE_ENABLE", 0, &PropVar) ) {
            
		   pDev->platProps.bPnocEnable = (uint32)PropVar.Val.dwVal;
			
		   if( pDev->platProps.bPnocEnable ) {
		   	
			 if ( DAL_SUCCESS !=
				DALSYS_GetPropertyValue(hProp, "I2C_PNOC_IB_VAL", 0, &PropVar) )
			 {
				break;
			 }	
			 pDev->platProps.uPnocIBVal = (uint64)PropVar.Val.dwVal;
		  
			 if ( DAL_SUCCESS !=
			    DALSYS_GetPropertyValue(hProp, "I2C_PNOC_AB_VAL", 0, &PropVar) )
			 {
				break;
			 }
			 pDev->platProps.uPnocABVal = (uint64)PropVar.Val.dwVal;
			   
			 if ( DAL_SUCCESS !=
				DALSYS_GetPropertyValue(hProp, "I2C_PNOC_ARB_MASTER", 0, &PropVar) )
		     {
				break;
			 }
			 pDev->platProps.uPnocMaster = (uint32)PropVar.Val.dwVal;
		  
			 if ( DAL_SUCCESS !=
		        DALSYS_GetPropertyValue(hProp, "I2C_PNOC_ARB_SLAVE", 0, &PropVar) )
		     {
				break;
			 }
			 pDev->platProps.uPnocSlave = (uint32)PropVar.Val.dwVal;			 
		  }		  
      }


      if ( pDev->platProps.bInterruptBased ) {
         if ( DAL_SUCCESS !=
               DALSYS_GetPropertyValue(hProp, "CHIP_IRQ_ID", 0, &PropVar)) {
            break;
         }
         pDev->platProps.uInterruptId = (uint32)PropVar.Val.dwVal;

         if ( DAL_SUCCESS !=
               DALSYS_GetPropertyValue(hProp, "CHIP_IRQ_TRIGGER_CFG", 0, &PropVar)) {
            break;
         }
         pDev->platProps.uIrqTriggerCfg = (uint32)PropVar.Val.dwVal;
      }

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "SW_DEVICE_NAME", 0, &PropVar)) {
         break;
      }
      pDev->platProps.pDevName = PropVar.Val.pszVal;

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_APP_CLK_NAME", 0, &PropVar)) {
         break;
      }
      pDev->platProps.pQupAppClkName = PropVar.Val.pszVal;

      if ( DAL_SUCCESS !=
               DALSYS_GetPropertyValue(hProp, "CHIP_APP_CLK_FREQ_KHZ", 0, &PropVar)) {
         break;     
      }
      pDev->platProps.uAppClkFreqKhz = (uint32)PropVar.Val.dwVal;

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_HCLK_NAME", 0, &PropVar )) {
         break;
      }
      pDev->platProps.pQupHClkName = PropVar.Val.pszVal;

      if ( DAL_SUCCESS ==
            DALSYS_GetPropertyValue(hProp, "CHIP_GPIO_CONFIG_ARR", 0, &PropVar )) {
         pDev->platProps.bHasGpios = 1;
         pDev->platProps.aGpioCfg[0] = PropVar.Val.pdwVal[0];
         pDev->platProps.aGpioCfg[1] = PropVar.Val.pdwVal[1];
      }
      else {
          pDev->platProps.bHasGpios = 0;
      }

      if ( NULL == pPeriphSsAddrName ) {
         if ( DAL_SUCCESS != DALSYS_GetPropertyValue(
                                   hProp,"CHIP_PERIPH_SS_BASE_NAME", 
                                   0, &PropVar ) ) {
            break;
         }
         pPeriphSsAddrName = PropVar.Val.pszVal;
	   }
	   if ( DAL_SUCCESS != DALSYS_GetPropertyValue(
                                   hProp,"CHIP_BLOCK_OFFSET", /* "QUPX_BASE" */
                                   0, &PropVar ) ) {
        break;
      }
      pDev->platProps.uQupBlockOffset = PropVar.Val.dwVal;
           
      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "SW_USE_BAM", 0, &PropVar) ) {
         break;
      }
      pDev->platProps.bBamEnabled = ((uint32)PropVar.Val.dwVal == 1)? 1: 0;
      if ( pDev->platProps.bBamEnabled ) {
        if (DAL_SUCCESS !=DALSYS_GetPropertyValue(hProp, 
                                       "CHIP_BAM_PHYS_ADDR", 0, &PropVar) ) {
           break;
        }
        pDev->platProps.uBamPhysAddr = (uint32)PropVar.Val.dwVal;
        if ( DAL_SUCCESS != DALSYS_GetPropertyValue(
                                       hProp, "CHIP_BAM_INTRERRUPT_ID",
                                       0, &PropVar) ) {
           break;
        }
        pDev->platProps.uBamIrqId = (uint32)PropVar.Val.dwVal;
   
        if ( DAL_SUCCESS != DALSYS_GetPropertyValue(hProp, "SW_BAM_THRESHOLD",
                                                    0, &PropVar) ) {
           break;
        }
        pDev->platProps.uBamThreshold = (uint32)PropVar.Val.dwVal;

        if ( DAL_SUCCESS != DALSYS_GetPropertyValue(
                                       hProp, "CHIP_QUP_INPUT_BAM_PIPE_NUM",
                                       0, &PropVar) ) {
           break;
        }
        pDev->platProps.uBamInPipeId = (uint32)PropVar.Val.dwVal;

        if ( DAL_SUCCESS !=DALSYS_GetPropertyValue(
                                       hProp, "CHIP_QUP_OUTPUT_BAM_PIPE_NUM",
                                       0, &PropVar) ) {
           break;
        }
        pDev->platProps.uBamOutPipeId = (uint32)PropVar.Val.dwVal;
      }
      result = I2C_RES_SUCCESS;
   } while ( 0 );

   return result;
}


/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Sets the power state.
  
    @param[in] hPlat  Platform handle.
    @param[in] state  New power state.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_SetPowerState
(
   I2CPLATDEV_HANDLE hPlat,
   uint32            state
)
{
   I2CPLATDEV_TargetCfgType     *pDev = (I2CPLATDEV_TargetCfgType *)hPlat;
   int32                         res;

   if ( I2CPLATDEV_POWER_STATE_0 == state ) {
      res = I2CPLATDEV_DisableClocks(pDev);
   }
   else if ( I2CPLATDEV_POWER_STATE_1 == state ) {
      res = I2CPLATDEV_DisableClocks(pDev); /*  bus state is powered. */
   }
   else if ( I2CPLATDEV_POWER_STATE_2 == state ) {      
      res = I2CPLATDEV_EnableClocks(pDev); /* bus state is active. */
   }
   else {
      return I2CPLATSVC_ERROR_INVALID_POWER_STATE;
   }

   if ( I2C_RES_SUCCESS == res ) {
      pDev->tmpPowerState = state;
   }

   return res;
}

/** @brief Gets the power state.
  
    @param[in] hPlat  Platform handle.
    @param[in] pState Pointer to power state.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_GetPowerState
(
   I2CPLATDEV_HANDLE  hPlat,
   uint32            *pState
)
{
   I2CPLATDEV_TargetCfgType     *pDev = (I2CPLATDEV_TargetCfgType *)hPlat;

   *pState = pDev->tmpPowerState;
   return I2C_RES_SUCCESS;
}

/** @brief Sets the resource exclusive .
  
    @param[in] hPlat  Platform handle.
    @param[in] hRes   Resource handle.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_SetResourceExclusive
(
   I2CPLATDEV_HANDLE hPlat,
   QUPRES_HANDLE     hRes
)
{
   return I2C_RES_SUCCESS;
}

/** @brief Releases the resource exclusive .
  
    @param[in] hPlat  Platform handle.
    @param[in] hRes   Resource handle.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_ReleaseResourceExclusive
(
    I2CPLATDEV_HANDLE hPlat,
    QUPRES_HANDLE     hRes
)
{
   return I2C_RES_SUCCESS;
}

/** @brief Reads the properties for this device.

    @param[in]  hPlat     Platform handle.
    @param[out] pDevProp  Pointer to device properties.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_ReadProperties
(
   I2CPLATDEV_HANDLE        hPlat,
   I2CPLATDEV_PropertyType *pDevProp
)
{
   I2CPLATDEV_TargetCfgType     *pDev;
   
   pDev = (I2CPLATDEV_TargetCfgType*)hPlat;
   
   pDevProp->pDevName        = pDev->platProps.pDevName;
   pDevProp->virtBlockAddr   = pPeriphSsBasePtr + pDev->platProps.uQupBlockOffset;
   pDevProp->bInterruptBased = pDev->platProps.bInterruptBased;
   pDevProp->bDisablePm      = pDev->platProps.bDisablePm;
   pDevProp->bBamAvailable   = pDev->platProps.bBamEnabled;
   pDevProp->uBamThreshold   = pDev->platProps.uBamThreshold;
   pDevProp->uI2cSrcClkKhz   = pDev->platProps.uAppClkFreqKhz;
   return I2C_RES_SUCCESS;
}


/** @brief sets the vote for PNOC.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */

static int32
I2CPLATDEV_AddPNOCVote
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   ICBArb_RequestType     i2cPnocRequest;
   ICBArb_MasterSlaveType i2cArbMasterSlave;
   ICBArb_ErrorType   i2cArbError;
   	  
   i2cArbMasterSlave.eMaster = pDev->platProps.uPnocMaster; 
   i2cArbMasterSlave.eSlave  = pDev->platProps.uPnocSlave;  

   /*How to name the node?*/
   pDev->hNpaClient = icbarb_create_suppressible_client_ex
                                          ("/node/core/bus/i2c/clk", 
   											&i2cArbMasterSlave,
   											I2C_ICB_CLIENT_CNT, NULL );
   if( pDev->hNpaClient == NULL) 
   {
     return I2CPLATSVC_ERROR_FAILED_TO_CREATE_PNOC_CLIENT;
   }  

   i2cPnocRequest.arbType =    ICBARB_REQUEST_TYPE_3;     /* Ib/Ab pair */
  
   /* Bandwidth in Bytes/second */
   i2cPnocRequest.arbData.type3.uIb = (uint64) pDev->platProps.uPnocIBVal;      
   i2cPnocRequest.arbData.type3.uAb = (uint64) pDev->platProps.uPnocABVal;     

   i2cArbError = icbarb_issue_request(pDev->hNpaClient, &i2cPnocRequest,
                                      I2C_PNOC_MSTRSLV_PAIRS_NUM);
   
   if(i2cArbError != ICBARB_ERROR_SUCCESS)
   {
     return I2CPLATSVC_ERROR_FAILED_TO_ISSUE_PNOC_REQ;
   }
    
   pDev->initState |= I2CPLATDEV_TGT_INIT_PNOC_VOTE_DONE;
   return I2C_RES_SUCCESS;
}

/**
    This Function initializes target support structures and subsystems.

    @param[in]  uPlatDevId  Platform specific device identifier.
    @param[out] phPlat      Pointer to platform handle.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_InitTarget
(
   uint32             uPlatDevId,
   I2CPLATDEV_HANDLE *phPlat
)
{
   I2CPLATDEV_TargetCfgType       *pDev;
   int32                           res;
 
   res = I2CSYS_Malloc((void **)&pDev, sizeof(I2CPLATDEV_TargetCfgType));
   if (I2C_RES_SUCCESS != res) {
      return res;
   }
   res = I2CSYS_Memset(pDev, 0, sizeof(I2CPLATDEV_TargetCfgType));
   if ( I2C_RES_SUCCESS != res )  {
      I2CSYS_Free(pDev);     
      return res;
   }
   pDev->initState = I2CPLATDEV_TGT_INIT_NOT_DONE;
   do
   {
      res = I2CPLATDEV_ReadPlatConfig(uPlatDevId, pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }

      if ( pDev->platProps.bPnocEnable ) /* Process  If Voting is requested */
      {
 	     res = I2CPLATDEV_AddPNOCVote(pDev); 	  
 	     if ( I2C_RES_SUCCESS != res ) {
          break;
         }
      }
	  
      res = I2CPLATDEV_InitClocks(pDev);
      if ( I2C_RES_SUCCESS != res ) {
       break;
      }

      res = I2CPLATDEV_InitHwio(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
     
      res = I2CPLATDEV_InitGpios(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      pDev->uPlatDevId = uPlatDevId;
      *phPlat = (I2CPLATDEV_HANDLE) pDev;
      res = I2C_RES_SUCCESS;
    } while ( 0 );

   if ( I2C_RES_SUCCESS != res ) {
      I2CPLATDEV_DeInitTarget(pDev);
   }
   return res;
}

/** @brief Resets resources like bam pipes following a qup core reset.

    @param[in] hPlatDev platform device handle.

    @return          int32 .
  */
int32
I2CPLATDEV_ResetTarget
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
   I2CPLATDEV_TargetCfgType     *pDev;
   int32                         res;
   I2CPLATDEV_BamPipeCtxt       *pPipeCtxt = NULL;
   bam_status_type               bamRes;
   
   pDev = (I2CPLATDEV_TargetCfgType*)hPlatDev;
   
   if (  !(pDev->initState & I2CPLATDEV_TGT_INIT_BAM_INITED) ) {
      return I2C_RES_SUCCESS;     
   }
   res = I2C_RES_SUCCESS;
   pPipeCtxt = &(pDev->bam.bamTxPipeCtxt);
   bamRes = bam_pipe_disable(pPipeCtxt->hBamPipe);
   if ( BAM_SUCCESS != bamRes ) {
      res = I2CPLATSVC_ERROR_FAILED_TO_DISABLE_TX_PIPE;     
   }
   bamRes = bam_pipe_enable(pPipeCtxt->hBamPipe);
   if ( (BAM_SUCCESS != bamRes) && (I2C_RES_SUCCESS == res) ) {
      res = I2CPLATSVC_ERROR_FAILED_TO_RE_ENABLE_TX_PIPE;     
   }

   pPipeCtxt = &(pDev->bam.bamRxPipeCtxt);
   bamRes = bam_pipe_disable(pPipeCtxt->hBamPipe);
   if ( (BAM_SUCCESS != bamRes) && (I2C_RES_SUCCESS == res) ) {
      res = I2CPLATSVC_ERROR_FAILED_TO_DISABLE_RX_PIPE;     
   }
   bamRes = bam_pipe_enable(pPipeCtxt->hBamPipe);
   if ( (BAM_SUCCESS != bamRes) && (I2C_RES_SUCCESS == res) ) {
      res = I2CPLATSVC_ERROR_FAILED_TO_RE_ENABLE_RX_PIPE;     
   }

   return res;
}

/** @brief unvote for PNOC.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */

static int32
I2CPLATDEV_RemovePNOCVote
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{ 
   if ( pDev->initState & I2CPLATDEV_TGT_INIT_PNOC_VOTE_DONE) {
     icbarb_complete_request(pDev->hNpaClient);
     icbarb_destroy_client(pDev->hNpaClient);  
   }
   return I2C_RES_SUCCESS; 
}

/**
    This Function deinitializes target support structures and
    subsystems.

    @param[in]  hPlatDev     Platform handle.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_DeInitTarget
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
   I2CPLATDEV_TargetCfgType       *pDev;
   int32                           res ;
   int32                           clkRes,hwioRes,bamRes,gpioRes,freeRes,npaRes;

   pDev = (I2CPLATDEV_TargetCfgType *)hPlatDev;
   res = clkRes = hwioRes = bamRes = gpioRes = freeRes = I2C_RES_SUCCESS;

   clkRes = I2CPLATDEV_DeInitClocks(pDev);
   npaRes = I2CPLATDEV_RemovePNOCVote(pDev);
   hwioRes = I2CPLATDEV_DeInitHwio(pDev);
   bamRes = I2CPLATDEV_BamDeInit(pDev);
   gpioRes = I2CPLATDEV_DeInitGpios(pDev);
   freeRes = I2CSYS_Free(pDev);

   if (clkRes)
   {
      res = clkRes;
   }
   else if(npaRes)
   {
      res = npaRes;
   }
   else if (hwioRes)
   {
      res = hwioRes;
   }
   else if (bamRes)
   {
      res = bamRes;
   }
   else if (gpioRes)
   {
      res = gpioRes;
   }
   else if (freeRes) {
      res = freeRes;
   }

   return res;
}

/**
    This Function registers IST and ISR callbacks.

    @param[in]  hPlatDev     Platform handle.
    @param[in]  IsrHook      ISR function pointer.
    @param[in]  pIsrData     pointer to ISR callback data.
    @param[in]  IstHook      IST function pointer.
    @param[in]  pIstData     pointer to IST callback data.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CSYS_RegisterISTHooks
(
   I2CPLATDEV_HANDLE hPlatDev,
   ISR_HOOK          IsrHook,
   void*             pIsrData,
   IST_HOOK          IstHook,
   void*             pIstData
)
{
   DALResult                     dalRes;
   I2CPLATDEV_TargetCfgType     *pDev;
   
   pDev = (I2CPLATDEV_TargetCfgType*)hPlatDev;
   
   if ( NULL == pDev->hInterruptCtrl ) {
      dalRes =
          DAL_InterruptControllerDeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                                              &pDev->hInterruptCtrl);
      if( (DAL_SUCCESS != dalRes) || (NULL == pDev->hInterruptCtrl) ) {
         return I2CPLATSVC_ERROR_FAILED_ATTACH_TO_IRQCTRL; 
      }
   }

   pDev->irq.IsrHook = IsrHook;
   pDev->irq.IstHook = IstHook;
   pDev->irq.pIsrData = pIsrData;
   pDev->irq.pIstData = pIstData;
   pDev->irq.uInterruptId = pDev->platProps.uInterruptId;
   pDev->irq.uIrqTriggerCfg = pDev->platProps.uIrqTriggerCfg;
   dalRes = DalInterruptController_RegisterISR(
                        pDev->hInterruptCtrl,
                        pDev->irq.uInterruptId,
                        I2CPLATDEV_Isr,
                        (DALISRCtx)pDev,
                        pDev->irq.uIrqTriggerCfg);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_REGISTER_IST;
   }
  
   return I2C_RES_SUCCESS;
}

/**
    This Function unregisters IST and ISR callbacks.

    @param[in]  hPlatDev     Platform handle.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CSYS_UnRegisterISTHooks
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
   DALResult                     dalRes;
   I2CPLATDEV_TargetCfgType     *pDev;
   pDev = (I2CPLATDEV_TargetCfgType*)hPlatDev;
   
   if ( NULL == pDev->hInterruptCtrl ) {
      return I2CPLATSVC_ERROR_FAILED_ATTACH_TO_IRQCTRL; 
   }

   dalRes = DalInterruptController_Unregister(pDev->hInterruptCtrl,pDev->irq.uInterruptId);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_UNREGISTER_IST;
   }

   return I2C_RES_SUCCESS;
}

/**
    This Function gets the app clock freq in KHz.

    @param[in]  hPlatDev     Platform handle.

    @return             frequency in KHz.
  */
uint32
I2CPLATDEV_GetAppClkKhz
(
   I2CPLATDEV_HANDLE hPlat
)
{
   I2CPLATDEV_TargetCfgType     *pDev;
   pDev = (I2CPLATDEV_TargetCfgType*)hPlat;     

   return pDev->platProps.uAppClkFreqKhz;
}
/** @brief Acquires an interrupt lock.

    @param[in] hPlatDev platform device handle.
    @return          I2CSYS_Result .
  */
void
I2CPLATDEV_AcquireIntLock
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
  
}

/** @brief Releases the interrupt lock.

    @param[in] hPlatDev platform device handle.
    @return          I2CSYS_Result .
  */
void
I2CPLATDEV_ReleaseIntLock
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
  
}

/** @brief Checks if BAM is enabled.

    @param[in] hPlatDev platform device handle.
    @return    TRUE if BAM is enabled FALSE otherwise .
  */
boolean
I2CPLATDEV_IsBamEnabled
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
   I2CPLATDEV_TargetCfgType     *pDev;
   pDev = (I2CPLATDEV_TargetCfgType*)hPlatDev;

   return pDev->platProps.bBamEnabled;
}



