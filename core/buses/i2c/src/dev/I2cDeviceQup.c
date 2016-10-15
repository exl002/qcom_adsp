/*=============================================================================

FILE:      I2cDeviceQup.c

OVERVIEW:  This file contains the implementation of the API for the QUPI2CDeviceQup
           I2CDEV_Init must be called before any other api call.

           Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
           All Rights Reserved.
           Qualcomm Confidential and Proprietary

=============================================================================*/

/*=============================================================================
  EDIT HISTORY FOR MODULE
  

$Header: //components/rel/core.adsp/2.2/buses/i2c/src/dev/I2cDeviceQup.c#7 $
$DateTime: 2014/10/01 05:00:56 $$Author: pwbldsvc $ 
 
When     Who    What, where, why
-------- ---    -----------------------------------------------------------
08/05/14 MS     Fixed Posted write issue in race condition between IST and Close.
05/12/14 LK     Fixed setting power state from after to before calling power API.
10/01/13 LK     Fixed out of transfer interrupts cusing crashes.
07/01/13 LK     Driver sequence validation changes.
04/22/13 LK     Added FLASH_STOP to pad single byte reads to slot boundary.
04/22/13 LK     Added Abort to stop transfers in case of wait errors.
02/22/13 LK     Added repeated start after read.
02/07/13 LK     Removed FLASH_STOP and single read descriptors.
01/23/13 LK     Fixed client config does't change.
09/26/11 LK     Created


=============================================================================*/

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

#include "I2cDevice.h"
#include "I2cPlatSvc.h"
#include "I2cPlatBam.h"
#include "I2cSys.h"
#include "HALqupe.h"
#include "I2cError.h"

// Define and include the header file that adds the relevant log headers
#define I2C_DEVICE_QUP_LOG
#include "I2cLog.h"

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

#define MAX_DATA_SEGMENT_LENGTH       255
#define I2CDEVQUP_DEFAULT_BUS_FREQ_KHZ               100
#define I2CDEVQUP_DEFAULT_BYTE_TRANSFER_TIMEOUT_US   200

#define I2C_HS_MASTER_ADDRESS            (0x0F)
#define I2C_FAST_STANDARD_FREQ_THRESHOLD_KHZ         (1000)

// this can be a constant for now but it can be
// read from enumeration data.
#define MAX_DEVICES    12
// pause is the worst case
#define QUP_RUN_STATE_CHANGE_TIME_OUT_US         100
#define QUP_BUSY_WAIT_STEP_US                    5

#define IQUPI2CDEV_VALIDATE_SYNC_TRAN(hDev,pTrh,pClntCfg,puNumCompleted)\
     ( (NULL == hDev)?           I2CDEVQUP_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pTrh)?           I2CDEVQUP_ERR_INVALID_TRANSFER_HANDLE:     \
       (NULL == puNumCompleted)? I2CDEVQUP_ERR_INVALID_NUMCOMPLETE_POINTER: \
         I2CDEVQUP_RES_SUCCESS                                                    \
     )

#define IQUPI2CDEV_VALIDATE_SYNC_SEQ(hDev,hSeq,pClntCfg,puNumCompleted) \
     ( (NULL == hDev)?           I2CDEVQUP_ERR_INVALID_DEV_HANDLE:           \
       (NULL == hSeq)?           I2CDEVQUP_ERR_INVALID_SEQUENCE_HANDLE:     \
       (NULL == puNumCompleted)? I2CDEVQUP_ERR_INVALID_NUMCOMPLETE_POINTER: \
       I2CDEVQUP_RES_SUCCESS                                                    \
     )

#define IQUPI2CDEV_VALIDATE_ASYNC_TRAN(hDev,pTrh,pClntCfg,pIoRes,pCallbackFn,pArg)\
     ( (NULL == hDev)?           I2CDEVQUP_ERR_INVALID_DEV_HANDLE:           \
       (NULL == pTrh)?           I2CDEVQUP_ERR_INVALID_TRANSFER_HANDLE:     \
       (NULL == pIoRes)?       I2CDEVQUP_ERR_INVALID_TRAN_RESULT_POINTER:  \
       (NULL == pCallbackFn)?    I2CDEVQUP_ERR_INVALID_CALLBACK_FN_POINTER:  \
       (NULL == pArg)?           I2CDEVQUP_ERR_INVALID_CALLBACK_ARG_POINTER:  \
       I2CDEVQUP_RES_SUCCESS                                                    \
     )
#define IQUPI2CDEV_VALIDATE_ASYNC_SEQ(hDev,hSeq,pClntCfg,pIoRes,pCallbackFn,pArg)\
     ( (NULL == hDev)?           I2CDEVQUP_ERR_INVALID_DEV_HANDLE:           \
       (NULL == hSeq)?           I2CDEVQUP_ERR_INVALID_SEQUENCE_HANDLE:     \
       (NULL == pIoRes)?       I2CDEVQUP_ERR_INVALID_TRAN_RESULT_POINTER:  \
       (NULL == pCallbackFn)?    I2CDEVQUP_ERR_INVALID_CALLBACK_FN_POINTER:  \
       (NULL == pArg)?           I2CDEVQUP_ERR_INVALID_CALLBACK_ARG_POINTER:  \
       I2CDEVQUP_RES_SUCCESS                                                   \
     )

#define                            BAM_TMPBUFF_MAX             (768)
#define                            I2CDEVQUP_HW_EVT_QUE_MAX    (16)



#define I2CDEVQUP_IS_FIRST_HS_TRANSFER(pSeqInfo, pTransfer)\
         (\
          pSeqInfo->bHsTransfer &&\
          ((I2cTrCtxSeqStart == pTransfer->eTranCtxt) ||\
           (I2cTrCtxNotASequence == pTransfer->eTranCtxt)\
          )\
         )


/*-------------------------------------------------------------------------
* Type Declarations
* ----------------------------------------------------------------------*/

enum  I2CDEVQUP_Error
{
   I2CDEVQUP_RES_SUCCESS   =      I2C_RES_SUCCESS,
   I2CDEVQUP_ERR_BASE = I2C_RES_ERROR_CLS_QUP_DEV,
   // Under device class the first byte is reserved to qup errors
   I2CDEVQUP_ERR_INVALID_DEVICE_ID = 0x30000100,
   I2CDEVQUP_ERR_IN_SEQ_OUT_OF_SYNC            ,
   I2CDEVQUP_ERR_INVALID_RUNSTATE              ,
   I2CDEVQUP_ERR_RUNSTATE_CHANGE_TIMEOUT       ,
   I2CDEVQUP_ERR_NEW_RUNSTATE_INVALID          ,
   I2CDEVQUP_ERR_INVALID_POWER_STATE           ,
   I2CDEVQUP_ERR_BUS_BUSY                      ,
   I2CDEVQUP_ERR_BUS_NOT_RELEASED              ,
   I2CDEVQUP_ERR_BUS_CLK_NOT_FORCED_LOW        ,
   I2CDEVQUP_ERR_INPUT_DONE_TRANSFERS_NOT      ,

   I2CDEVQUP_ERR_INVALID_DEV_HANDLE            ,
   I2CDEVQUP_ERR_INVALID_TRANSFER_HANDLE       ,
   I2CDEVQUP_ERR_INVALID_TRANSFER_OBJPTR       ,
   I2CDEVQUP_ERR_INVALID_SEQUENCE_HANDLE       ,
   I2CDEVQUP_ERR_INVALID_SEQUENCE_OBJPTR       ,
   I2CDEVQUP_ERR_INVALID_NUMCOMPLETE_POINTER   ,
   I2CDEVQUP_ERR_INVALID_TRAN_RESULT_POINTER   ,
   I2CDEVQUP_ERR_INVALID_CALLBACK_FN_POINTER   ,
   I2CDEVQUP_ERR_INVALID_CALLBACK_ARG_POINTER  ,
   I2CDEVQUP_ERR_INVALID_TRANSFER_DIRECTION    ,
   I2CDEVQUP_ERR_OUT_OF_OUTPUT_TAG_BUFFER      ,
   I2CDEVQUP_ERR_INPUT_DESC_BUFFER_ALLOC_FAILED,
   

   I2CDEVQUP_ERR_BAM_ERROR    ,

};

typedef struct I2CDEVQUP_SequenceIOInfo I2CDEVQUP_SequenceIOInfo;

typedef enum I2CDEVQUP_InitState
{
   I2CDEVQUP_InitState_Uninitialized             = 0x0,
   I2CDEVQUP_InitState_TargetInit_Done           = 0x1,
   I2CDEVQUP_InitState_Events_Done               = 0x2,
   I2CDEVQUP_InitState_DeviceInit_Done           = 0x4,
   I2CDEVQUP_InitState_HardwareInit_Done         = 0x8,
   I2CDEVQUP_InitState_PhysMemAlloc_Done         = 0x10,
   I2CDEVQUP_InitState_CritSectionCreate_Done    = 0x20,
   I2CDEVQUP_InitState_ISR_IST_Registration_Done = 0x40,

} I2CDEVQUP_InitState;

typedef volatile struct I2CDEVQUP_QupIsrHwEvt
{
   uint32                              uIrqCnt;
   uint32                              uQupErrors;
   uint32                              uOperational;
   uint32                              uOutTranCnt;
   uint32                              uInTranCnt;
   HAL_qupe_I2cStatus                  i2cStatus;

} I2CDEVQUP_QupIsrHwEvt;

typedef volatile struct I2CDEVQUP_QupBamHwEvt
{
   uint32                              uBamEvt;
} I2CDEVQUP_QupBamHwEvt;

typedef volatile struct I2CDEVQUP_QupHwEvtQueue
{
   I2CDEVQUP_QupIsrHwEvt      aHwEvt[I2CDEVQUP_HW_EVT_QUE_MAX];
   I2CDEVQUP_QupIsrHwEvt     *pHead;
   I2CDEVQUP_QupIsrHwEvt     *pTail;

} I2CDEVQUP_QupHwEvtQueue;

typedef struct I2CDEVQUP_QupInfo
{
   uint32                              uIrqCnt;
   I2CDEVQUP_QupHwEvtQueue             hwEvtQueue;
   I2CPLATBAM_EventType                eBamEvt;
   uint32                              uInFreeSlotReqCnt;
   uint32                              uOutFreeSlotReqCnt;
   uint32                              uQupErrors;
   uint32                              uOperational;
   HAL_qupe_I2cStatus                  i2cStatus;
   uint32                              uInFreeSlotServicedCnt;
   uint32                              uOutFreeSlotServicedCnt;
   uint32                              uCurOutTransfers;
   uint32                              uCurInTransfers;
   HAL_qupe_IOModeType                 eOutFifoMode;
   HAL_qupe_IOModeType                 eInFifoMode;
} I2CDEVQUP_QupInfo;

typedef enum I2CDEVQUP_TransferState
{
   I2CDEVQUP_TR_STATE_PAD_OUTPUT,
   I2CDEVQUP_TR_STATE_TIME_STAMP_TAG,
   I2CDEVQUP_TR_STATE_TIME_STAMP_VALUE,
   
   I2CDEVQUP_TR_STATE_MASTER_START_TAG,
   I2CDEVQUP_TR_STATE_MASTER_ADDR,

   I2CDEVQUP_TR_STATE_START_TAG,
   I2CDEVQUP_TR_STATE_SLAVE_ADDR,
   I2CDEVQUP_TR_STATE_RD_DATA_TAG,
   I2CDEVQUP_TR_STATE_WR_DATA_TAG,
   I2CDEVQUP_TR_STATE_DATA_CNT,
   I2CDEVQUP_TR_STATE_EOT_TAG,
   I2CDEVQUP_TR_STATE_DATA_VALUE,
   I2CDEVQUP_TR_STATE_DATA_PAD,

   I2CDEVQUP_TR_STATE_NOT_IN_TRANSFER,

   I2CDEVQUP_TR_STATE_EXTEND  = 0x7FFFFFFF
} I2CDEVQUP_TransferState;


typedef enum I2CDEVQUP_OutputState
{
   I2CDEVQUP_OUTPUT_IDDLE            ,
   I2CDEVQUP_OUTPUT_STALLED          ,
   I2CDEVQUP_OUTPUT_FILL_FIFO        ,
   I2CDEVQUP_OUTPUT_FILL_FIFO_DONE   ,
   I2CDEVQUP_OUTPUT_TRANSMIT_DONE    ,
   I2CDEVQUP_OUTPUT_ERROR    ,
} I2CDEVQUP_OutputState;

typedef struct I2CDEVQUP_OutSeqInfo
{
   I2CDEVQUP_OutputState               eOutputState;
   I2cTransfer                        *pTransfer;
   uint32                              uTotalOutBytes; //data + format
   uint32                              uCurOutBytes; //data + format
   uint32                              uNumOutDtBytes;//data cnt
   uint32                              uOutTrCnt;
   uint32                              uTrIndex;
   uint8                               *aOutBamBuff;
   uint32                              uBamBuffIndex;

   struct OutTransferInfo
   {
      I2CDEVQUP_TransferState          eOutTrState;
      uint32                           uDataIndex;
      uint32                           uNextDataSegIndex;
      I2cBuffDesc                     *pTrIOVec;
      uint32                           IoVecBuffIndex;
   } outTr;
   I2CDEVQUP_SequenceIOInfo           *pSeq;
} I2CDEVQUP_OutSeqInfo;

typedef enum I2CDEVQUP_InputState
{
   I2CDEVQUP_INPUT_IDDLE,
   I2CDEVQUP_INPUT_STALLED,
   I2CDEVQUP_INPUT_MEMORY_FLUSH,
   I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO,
   I2CDEVQUP_INPUT_PUSH_DESCRIPTORS,
   I2CDEVQUP_INPUT_WAIT_READ_DATA_READY,
   I2CDEVQUP_INPUT_RECEIVE_DONE,
   I2CDEVQUP_INPUT_ERROR,
} I2CDEVQUP_InputState;

typedef struct  I2CDEVQUP_InSeqInfo
{
   I2CDEVQUP_InputState                eInSeqState;
   I2cTransfer                        *pTransfer;
   uint32                              uTotalInBytes;//data + format
   uint32                              uNumInDtBytes;//data cnt
   uint32                              uInPadCnt;
   uint32                              uInTrCnt;
   uint32                              uTrIndex;
   uint32                              uLastTrIndex;
   uint32                              uNumInBytesReceived;
   uint8                              *aInBamBuff;

   struct InTransferInfo
   {
      I2CDEVQUP_TransferState          eInTrState;
      uint32                           uDataIndex; //transfer data index
      uint32                           uNextDataSegIndex;
      //uint32                           uTimeStamp; // for now do nothing with this.
      I2cBuffDesc                     *pTrIOVec;
      uint8                           *pIoVecBuff;
      uint32                           IoVecBuffIndex;
   } inTr;
   I2CDEVQUP_SequenceIOInfo           *pSeq;
} I2CDEVQUP_InSeqInfo;

typedef enum I2CDEVQUP_SequenceState
{
   I2CDEVQUP_SEQSTATE_IDDLE,
   I2CDEVQUP_SEQSTATE_INPUT_OUTPUT,
   I2CDEVQUP_SEQSTATE_COMPLETING,   
   I2CDEVQUP_SEQSTATE_COMPLETE,
   I2CDEVQUP_SEQSTATE_QUP_STATE_ERROR,
} I2CDEVQUP_SequenceState;

struct I2CDEVQUP_SequenceIOInfo
{
   I2cTransfer                     *pTransfer;
   uint32                           uNumTransfers;
   uint32                           uSeqTimeoutUs;
   I2CDEVQUP_OutSeqInfo             outSeqInfo;
   I2CDEVQUP_InSeqInfo              inSeqInfo;
   I2CSYS_PhysMemDesc               physMem;
   boolean                          bBamTransfer;
   boolean                          bHsTransfer;
   I2CDEVQUP_SequenceState          eSeqState;
   I2cIoResult                      ioRes;
};

typedef struct I2CDEVQUP_AsyncCallData
{
   DEV_ASYNC_CB                        pCallbackFn;
   void                               *pArg;
   I2cIoResult                        *pIoRes;
} I2CDEVQUP_AsyncCallData;

typedef struct I2CDEVQUP_Device I2CDEVQUP_Device;
typedef void (* PFnPerformOutput)(I2CDEVQUP_Device  *pDev);
typedef void (* PFnPerformInput) (I2CDEVQUP_Device *pDev);

struct I2CDEVQUP_Device
{
   uint32                              uPlatDevId;
   I2CPLATDEV_PropertyType             devProps;
   HAL_qupe_HwInfoType                 devHwInfo;
   char                               *pHalVer;
   uint32                              devInitState;
   I2CDEVQUP_SequenceIOInfo            seqInfo;
   I2CDEVQUP_QupInfo                   qupInfo;
   I2CSYS_EVENT_HANDLE                 hQupEvt;

   I2CPLATDEV_HANDLE                   hDevTgt;

   I2cClientConfig                     clntCfg;
   I2CDEV_PowerStates                  devPowerState;
   I2CDEVQUP_AsyncCallData             asyncCall;
   I2CSYS_CRITSECTION_HANDLE           hOperationSync;

   PFnPerformOutput                    pFnPerformOutput;
   PFnPerformInput                     pFnPerformInput;
   I2CLOG_DevType                     *pDevLog;

   I2CDEVQUP_Device                   *pNext; /* used to link the dev structs */
};


/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

I2CDEVQUP_Device *pDevListHead = NULL;

/*-------------------------------------------------------------------------
* Static Function Declarations and Definitions
* ----------------------------------------------------------------------*/
static int32
I2CDEV_AbortTransfer
(
   I2CDEVQUP_Device *pDev
);

/** @brief Searches for a device in the list.
  
    @param[in] uPlatDevId    Platform id for the device.
    
    @return             Pointer to dev if found, otherwise NULL.
  */
static I2CDEVQUP_Device * 
I2CDEVQUP_SearchDevice
(
   uint32          uPlatDevId 
)
{
   I2CDEVQUP_Device * pDev = pDevListHead;
   while ( pDev ) {
      if ( pDev->uPlatDevId == uPlatDevId ) {
         return pDev;
      }
      pDev = pDev->pNext;
   }

   return NULL;
}

/** @brief Links a device in the list.

    @param[in] pDev     Pointer to device structure.
    
    @return             Nothing.
  */
static void 
I2CDEVQUP_LinkDevice
(
   I2CDEVQUP_Device * pDev 
)
{
   pDev->pNext = pDevListHead;
   pDevListHead = pDev;
}

/** @brief Unlinks a device in the list.

    @param[in] pDev     Pointer to device structure.
    
    @return             Nothing.
  */
static void 
I2CDEVQUP_UnlinkDevice
(
   I2CDEVQUP_Device * pDev 
)
{
   I2CDEVQUP_Device  *pCurDev;
   I2CDEVQUP_Device **ppPrev;

   ppPrev  = &pDevListHead;
   pCurDev = pDevListHead;

   while ( NULL != pCurDev ) {
      if ( pCurDev == pDev ) {
         *ppPrev = pCurDev->pNext;
         return;
      }
      ppPrev = &pCurDev->pNext;
      pCurDev = pCurDev->pNext;  
   }
}

/** @brief Updatest the transfer count stored in the device.

    This Function Updatest the transfer count stored in the device.

    @param[in] pDev    Pointer to device struct.

    @return          Nothing.
  */
static void QUP_UpdateTransferCnt
(
   I2CDEVQUP_Device *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;

   pQupInfo    = &pDev->qupInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   pQupInfo->uCurOutTransfers =
         HAL_qupe_GetOutTransferCountCurrentExt(
                           pDev->devProps.virtBlockAddr,
                           pQupInfo->eOutFifoMode,
                           pOutSeqInfo->uTotalOutBytes);
   pQupInfo->uCurInTransfers =
         HAL_qupe_GetInTransferCountCurrentExt(
                           pDev->devProps.virtBlockAddr,
                           pQupInfo->eInFifoMode,
                           pInSeqInfo->uTotalInBytes);
}

/** @brief Wait for state to go valid or timeout.
           
    @param[in] pQupAddr       QUP block address.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */

static int32
I2CDEVQUP_WaitForRunStateValid(uint8 *pQupAddr)
{
   uint32 uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;

   while( (!HAL_qupe_RunStateIsValid(pQupAddr)) && (0 != uWaitTimeUs) ) {
      I2CSYS_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
   }
   if( 0 == uWaitTimeUs ) {
      return I2CDEVQUP_ERR_INVALID_RUNSTATE;
   }

   return I2C_RES_SUCCESS;
}

/** @brief Resets the qup core and waits until state is valid.
           
    @param[in] pQupAddr       QUP block address.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */

static int32
I2CDEVQUP_ResetQupCore
(
   I2CDEVQUP_Device      *pDev
)
{
  int32               res, platRes;      

  I2C_CALLTRACE_LEVEL2(pDev->pDevLog, 2,
                        "DEV:0x%x: I2CDEVQUP_ResetQupCore: Reseting Platform pDev=0x%x",
                         pDev->uPlatDevId, (uint32) pDev);
  platRes = I2CPLATDEV_ResetTarget(pDev->hDevTgt);
  
  I2C_CALLTRACE_LEVEL2(pDev->pDevLog, 2,
                        "DEV:0x%x: I2CDEVQUP_ResetQupCore: Reseting QUP pDev=0x%x",
                         pDev->uPlatDevId, (uint32) pDev);
  HAL_qupe_Reset(pDev->devProps.virtBlockAddr);
  res = I2CDEVQUP_WaitForRunStateValid(pDev->devProps.virtBlockAddr);
  if ( I2C_RES_SUCCESS != platRes ) {
     res = platRes;     
  }

  return res;
}

/** @brief Sets the runstate. It will wait untill the state is valid
           before trying to set the new state or timeout.
           
    @param[in] pQupAddr       QUP block address.
    @param[in] cRunState      The new run state.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */
static int32
I2CDEVQUP_SetRunState(uint8 *pQupAddr, const HAL_qupe_RunStateType eNewRunState)
{
   uint32 uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;
   HAL_qupe_RunStateType eRunState;

   HAL_qupe_SetRunStateExt(pQupAddr, eNewRunState);
   eRunState = HAL_qupe_GetRunState(pQupAddr);
   while ( ( 0 != uWaitTimeUs ) && ( eNewRunState != eRunState ) ) {
      I2CSYS_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
      eRunState = HAL_qupe_GetRunState(pQupAddr);
   }
   if( 0 == uWaitTimeUs ) {
      return I2CDEVQUP_ERR_RUNSTATE_CHANGE_TIMEOUT;
   }

   return I2C_RES_SUCCESS;
}


/** @brief Sets the runstate. It will wait untill the state is valid
           before trying to set the new state or timeout.
           
    @param[in] pQupAddr       QUP block address.
    @param[in] cRunState      The new run state.
    @return           Returns timeout error if the previous state is invalid,
                      success otherwise.
  */
static int32
I2CDEVQUP_SetRunStateIfValid(uint8 *pQupAddr, const HAL_qupe_RunStateType eNewRunState)
{
   uint32 uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;
   HAL_qupe_RunStateType eRunState;
   
   eRunState = HAL_qupe_GetRunState(pQupAddr);
   while( (HAL_QUPE_RUNSTATE_INVALID == eRunState) &&
          (0 != uWaitTimeUs) ) {
      I2CSYS_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
      eRunState = HAL_qupe_GetRunState(pQupAddr);
   }
   if( 0 == uWaitTimeUs ) {
      return I2CDEVQUP_ERR_INVALID_RUNSTATE;
   }

   if ( eRunState == eNewRunState) {
      return I2C_RES_SUCCESS;
   }

   return I2CDEVQUP_SetRunState(pQupAddr, eNewRunState);
}

/** @brief Initializes the event queue


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   Nothing.
  */
static void
I2CDEVQUP_QupHwEvtQueue_Init
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   pHwEvtQ->pHead = pHwEvtQ->aHwEvt;
   pHwEvtQ->pTail = pHwEvtQ->aHwEvt;
}

/** @brief Checks if the event queue is full


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   TRUE if full, FALSE otherwise.
  */
static boolean
I2CDEVQUP_QupHwEvtQueue_IsFull
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   I2CDEVQUP_QupIsrHwEvt *pNext;

   pNext = pHwEvtQ->pHead + 1; /* advance the pointer by 1 slot. */
   if ( pNext == (pHwEvtQ->aHwEvt + I2CDEVQUP_HW_EVT_QUE_MAX) ) {
      pNext = pHwEvtQ->aHwEvt; 
   }
   return (pNext == pHwEvtQ->pTail);
}

/** @brief Checks if the event queue is full


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   TRUE if full, FALSE otherwise.
  */
static boolean
I2CDEVQUP_QupHwEvtQueue_IsEmpty
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   return (pHwEvtQ->pHead == pHwEvtQ->pTail);
}

/** @brief Moves tail pointer one slot


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   Nothing.
  */
static void
I2CDEVQUP_QupHwEvtQueue_TailNext
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   I2CDEVQUP_QupIsrHwEvt *pNext;

   pNext = pHwEvtQ->pTail + 1; /* advance the pointer by 1 slot. */
   if ( pNext == (pHwEvtQ->aHwEvt + I2CDEVQUP_HW_EVT_QUE_MAX) ) {
      pNext = pHwEvtQ->aHwEvt; 
   }
   pHwEvtQ->pTail = pNext;
}

/** @brief Moves head pointer one slot


    @param[in] pHwEvtQ        Pointer to HW event queue.

    @return                   Nothing.
  */
static void
I2CDEVQUP_QupHwEvtQueue_HeadNext
(
   I2CDEVQUP_QupHwEvtQueue *pHwEvtQ
)
{
   I2CDEVQUP_QupIsrHwEvt *pNext;

   pNext = pHwEvtQ->pHead + 1; /* advance the pointer by 1 slot. */
   if ( pNext == (pHwEvtQ->aHwEvt + I2CDEVQUP_HW_EVT_QUE_MAX) ) {
      pNext = pHwEvtQ->aHwEvt; 
   }
   pHwEvtQ->pHead = pNext;
}


/** @brief This function Performs fifo/block mode specific servicing.
   
    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void 
I2CDEVQUP_QupHwEvtQueue_ProcessIsrEvt
(
   I2CDEVQUP_Device      *pDev,
   I2CDEVQUP_QupIsrHwEvt *pIsrEvt
)
{   
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;

   pQupInfo = &pDev->qupInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;

   pQupInfo->uQupErrors                |= pIsrEvt->uQupErrors;
   pQupInfo->uOperational              |= pIsrEvt->uOperational;

   /* OR the RawStatus only  and not the line state */
   pQupInfo->i2cStatus.uRawLineState    = pIsrEvt->i2cStatus.uRawLineState;
   pQupInfo->i2cStatus.uRawStatus      |= pIsrEvt->i2cStatus.uRawStatus &
       HAL_QUPE_I2C_OP_STATUS_ERROR_FIELD;

   pQupInfo->uCurOutTransfers = pIsrEvt->uOutTranCnt;
   pQupInfo->uCurInTransfers  = pIsrEvt->uInTranCnt;

   /* If output service then fill the output queue with another block.       */
   if ( pIsrEvt->uOperational & HAL_QUPE_OPSTATE_OUTPUT_SERVICE ) {
      if ( (HAL_QUPE_IO_MODE_BLOCK == pQupInfo->eOutFifoMode) &&
       (pIsrEvt->uOperational & HAL_QUPE_OPSTATE_OUT_BLOCK_WRITE_REQ) ) {
         pQupInfo->uOutFreeSlotReqCnt += pDev->devHwInfo.uNumOutputBlockSlots;
      }
      /* In fifo mode the OUTPUT_SERVICE is set and the MAX_OUTPUT_DONE
       * is not set at the same time. */
      if ( HAL_QUPE_IO_MODE_FIFO == pQupInfo->eOutFifoMode ) {
         pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_TRANSMIT_DONE;
      }
   }
   if ( pIsrEvt->uOperational & HAL_QUPE_OPSTATE_MAX_OUTPUT_DONE ) {
      pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_TRANSMIT_DONE;
   }

   /* If input service then get the blocks from queue.                       */
   if ( pIsrEvt->uOperational & HAL_QUPE_OPSTATE_INPUT_SERVICE ) {
      if ( HAL_QUPE_IO_MODE_BLOCK == pQupInfo->eInFifoMode ) {
         /* last block gets this as well as normal block requests. */
         pQupInfo->uInFreeSlotReqCnt += pDev->devHwInfo.uNumInputBlockSlots;
      }
      else {
         pQupInfo->uInFreeSlotReqCnt +=
            HAL_qupe_GetInFifoOccupiedSlotsCount(pDev->devProps.virtBlockAddr);
      //   pQupInfo->bRxDone = TRUE;
      }
   }
   if ( pIsrEvt->i2cStatus.uRawStatus & HAL_QUPE_I2C_OP_STATUS_ERROR_FIELD ) {
      /* Don't continue filling the output fifo if there is an error. */
      pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_ERROR;
   }

   return;
}

/** @brief Calculate the number of input and output bytes on a transfer.

    Calculate the number of input and output bytes for a transfer.

    @param[in]  pTransfer       Transfer pointer.
    @param[out] puNumInBytes    Pointer to num input bytes.
    @param[out] puNumOutBytes   Pointer to num output bytes.
          reads.

    @return                   Nothing.
  */
void
I2CDEVQUP_CalcTransferInOutSize
(
   I2cTransfer                     *pTransfer,
   uint32                 *puNumInBytes,
   uint32                 *puNumOutBytes
)
{
   uint32 uNumDataSegments;
   uint32 uTransferSize = pTransfer->uTrSize;
  
   uNumDataSegments = uTransferSize/MAX_DATA_SEGMENT_LENGTH;
   if ( uTransferSize % MAX_DATA_SEGMENT_LENGTH ) {
      uNumDataSegments++;
   }
   if ( I2cTranDirIn == pTransfer->eTranDirection ) {
      /* read:
         st_tag + sl_addr + {dt_tag + bt_cnt} * n */
      *puNumOutBytes = 2 +  uNumDataSegments*2;
      /* {dt_tag + bt_cnt} * n + data_len               */
      *puNumInBytes  =  uNumDataSegments*2 + uTransferSize;
   }
   else {
      /* write:
         st_tag + sl_addr + {dt_tag + bt_cnt} * n + data_len */
      *puNumOutBytes = 2 +  uNumDataSegments*2 + uTransferSize;
      *puNumInBytes  = 0;
   }
}



/** @brief Calculate the number of input and output data bytes transferred.

    Calculate the number of input and output bytes for a transfer.

    @param[in]  pDev            Pointer to device.
 
    @return                   Nothing.
  */
static void
I2CDEVQUP_CalcTransferredDataBytes
(
   I2CDEVQUP_Device                    *pDev
)
{
   uint32                              uNumDataSegments;
   I2CDEVQUP_SequenceIOInfo           *pSeqInfo;
   I2CDEVQUP_QupInfo                  *pQupInfo;
   uint32                              uOutByteCnt;
   uint32                              uInByteCnt;
   uint32                              uNumInTrBytes;
   uint32                              uNumOutTrBytes;
   I2CDEVQUP_OutSeqInfo               *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo                *pInSeqInfo;
   I2cTransfer                        *pTransfer;
   uint32                              uTrIndex;
   boolean                             bOutComplete;
   boolean                             bInComplete;

   pSeqInfo    = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   pQupInfo    = &pDev->qupInfo;


   if (I2C_RES_SUCCESS == pSeqInfo->ioRes.nOperationResult) {
      pSeqInfo->ioRes.uInByteCnt  = pInSeqInfo->uNumInDtBytes;
      pSeqInfo->ioRes.uOutByteCnt = pOutSeqInfo->uNumOutDtBytes;
      return;
   }

   uInByteCnt   = pQupInfo->uCurInTransfers;
   uOutByteCnt  = pQupInfo->uCurOutTransfers;
   pSeqInfo->ioRes.uInByteCnt  = 0;
   pSeqInfo->ioRes.uOutByteCnt = 0;
   bOutComplete = bInComplete = FALSE;

   if ( uOutByteCnt != 0 ) {
      uTrIndex = 0;
      do {
         pTransfer = &pSeqInfo->pTransfer[uTrIndex];
         I2CDEVQUP_CalcTransferInOutSize(pTransfer,&uNumInTrBytes, &uNumOutTrBytes);

         if ( uOutByteCnt >= uNumOutTrBytes ) {
            uOutByteCnt -= uNumOutTrBytes;
            if ( 0 == uOutByteCnt ) {
               bOutComplete = TRUE;/* full output last */
            }
         }
         else {
            break;/* partial output do not count this transfer */
         }

         if ( uInByteCnt >= uNumInTrBytes ) {
            uInByteCnt -= uNumInTrBytes;
            if ( 0 == uInByteCnt ) {
               bInComplete = TRUE;/* full input last */
            }
         }
         else {
            break;/* partial input do not count this */
         }

         if ( I2cTranDirIn == pTransfer->eTranDirection ) {
            pSeqInfo->ioRes.uInByteCnt += pTransfer->uTrSize;
         }
         else {
            pSeqInfo->ioRes.uOutByteCnt += pTransfer->uTrSize;
         }

         ++uTrIndex;
      } while( (uTrIndex < pSeqInfo->uNumTransfers) &&
               (!bOutComplete || !bInComplete) );
   }

   if ( !bOutComplete || !bInComplete ) {
      if ( I2cTranDirIn == pOutSeqInfo->pTransfer->eTranDirection ) {
         /* read
             {bt_cnt} * n + data_len */
         uNumDataSegments = uInByteCnt/(MAX_DATA_SEGMENT_LENGTH + 1);
         if (uInByteCnt % (MAX_DATA_SEGMENT_LENGTH + 1)) {
            uNumDataSegments++;
         }
         /* count out the formating overhead, 1 byte per segment */
         pSeqInfo->ioRes.uInByteCnt += (uInByteCnt - uNumDataSegments);
      }
      else {
         /* write
             st_tag + sl_addr + {dt_tag + bt_cnt} * n + data_segment */
         if (uOutByteCnt > 4) {
            uOutByteCnt -= 2; /* drop st_tag + sl_addr */
            uNumDataSegments = uOutByteCnt/(MAX_DATA_SEGMENT_LENGTH + 2);
            if (uOutByteCnt % (MAX_DATA_SEGMENT_LENGTH + 2)) {
               uNumDataSegments++;
            }
            /* count out the formating overhead, 2 bytes per segment */
            pSeqInfo->ioRes.uOutByteCnt += (uOutByteCnt - uNumDataSegments*2);
         }
      }
   }
}

/**
    This function waits for bus free.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         TRUE if the bus is free, FALSE otherwise.
  */
static boolean QUP_WaitForI2cBusFree
(
   I2CDEVQUP_Device               *pDev
)
{
   uint32                 uTimeout;
   I2CDEVQUP_QupInfo     *pQupInfo;
   const uint32 cuTimeStep = QUP_BUSY_WAIT_STEP_US; /* 5 us */

   pQupInfo = &pDev->qupInfo;
   /* Check stored state. */
   if (!HAL_qupe_IsI2CStateBusy(&pQupInfo->i2cStatus)) {
      return TRUE;
   }
   /* Checks the actual bus state. */
   if ( !HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr) ) {
      return TRUE;
   }
   
   uTimeout = pDev->clntCfg.uByteTransferTimeoutUs;
   do{
      I2CSYS_BusyWait(cuTimeStep);
      if ( uTimeout > cuTimeStep ) {
         uTimeout -= cuTimeStep;
      }
      else {
         uTimeout = 0;
      }
   } while ( uTimeout && HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr) );

   return (!HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr));
}

/**
    This function waits for bus clk to go to forced low state.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         TRUE if the bus is free, FALSE otherwise.
  */
static boolean QUP_WaitForI2cBusClkForcedLow
(
   I2CDEVQUP_Device               *pDev
)
{
   uint32 uTimeout;
   const uint32 cuTimeStep = QUP_BUSY_WAIT_STEP_US; /* 5 us */

   if (HAL_qupe_IsI2CBusClkForcedLow (pDev->devProps.virtBlockAddr)) {
      return TRUE;
   }
   
   uTimeout = pDev->clntCfg.uByteTransferTimeoutUs;
   do{
      I2CSYS_BusyWait(cuTimeStep);
      if ( uTimeout > cuTimeStep ) {
         uTimeout -= cuTimeStep;
      }
      else {
         uTimeout = 0;
      }
   } while ( uTimeout && !HAL_qupe_IsI2CBusClkForcedLow(pDev->devProps.virtBlockAddr) );

   return (HAL_qupe_IsI2CBusClkForcedLow(pDev->devProps.virtBlockAddr));
}

/**
    This function performs completion.


    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void 
QUP_Completion
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2cTransfer                    *pTransfer;
   int32  res;
   /* If this is an asynchronous transfer then set results and
      dispatch callback else just signal the waiting thread. */

   pSeqInfo = &pDev->seqInfo;
   pTransfer = pSeqInfo->pTransfer;

   I2C_CALLTRACE_LEVEL2(pDev->pDevLog, 2,
                        "DEV:0x%x: QUP_Completion pDev=0x%x",
                         pDev->uPlatDevId, (uint32) pDev);


   /* Before we calculate the counts, we must wait for the bus to be either idle
    * or forced low */
   if (((pTransfer->eTranCtxt == I2cTrCtxSeqStart) ||
        (pTransfer->eTranCtxt == I2cTrCtxSeqContinue)) &&
        (pSeqInfo->uNumTransfers == 1)) {
       /* Wait for FORCED LOW state on CLK STATE */
       if ( !QUP_WaitForI2cBusClkForcedLow(pDev) ) {
           if ( I2C_RES_SUCCESS == pDev->seqInfo.ioRes.nOperationResult ) {
               pDev->seqInfo.ioRes.nOperationResult = I2CDEVQUP_ERR_BUS_CLK_NOT_FORCED_LOW;
           }     
       }
   } else {
      /* 
         QCTDD00352568.
         The driver needs to wait for bus idle before in sets the runstate 
         to RESET.
       */
      if ( !QUP_WaitForI2cBusFree(pDev) ) {
         if ( I2C_RES_SUCCESS == pDev->seqInfo.ioRes.nOperationResult ) {
            pDev->seqInfo.ioRes.nOperationResult = I2CDEVQUP_ERR_BUS_NOT_RELEASED;
         }     
      }
   }

   I2CDEVQUP_CalcTransferredDataBytes(pDev);

   /* Reset the state */
   res = I2CDEVQUP_SetRunStateIfValid(pDev->devProps.virtBlockAddr,
                                      HAL_QUPE_RUNSTATE_RESET);
   if( I2C_RES_SUCCESS != res ) {
      I2CDEVQUP_ResetQupCore(pDev);
      HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
      if ( I2C_RES_SUCCESS == pDev->seqInfo.ioRes.nOperationResult ) {
         pDev->seqInfo.ioRes.nOperationResult = res;
       }
   }

   /* After doing the above reset, the bus will be in one of the below states:
      1. Bus is idle. This state can occur in 2 cases
         a) If this is NOT a split transfer and the above code waits until the
            bus is idle.
         b) If this is a SPLIT TRANSFER and slave is not stretching the clock
            after the last byte.
      2. Bus is NOT IDLE. It means the slave is stretching the clock. This can
         occur in 2 cases
         a) If this is NOT a split transfer and the above code waits until the
            bus is idle, but the slave is stretching the clock beyond the
            configured value. Solution: Slave/Client must increase the clk
            stretch timeout.
         b) If this is a SPLIT TRANSFER and slave is stretching the clock after
            the last byte. In this case, we will wait for the bus idle or the
            configured clock stretch timeout (whichever occurs first).
    */      
   /* Execute 2b in above comment */
   if (((pTransfer->eTranCtxt == I2cTrCtxSeqStart) ||
        (pTransfer->eTranCtxt == I2cTrCtxSeqContinue)) &&
        (pSeqInfo->uNumTransfers == 1)) {
      if (!QUP_WaitForI2cBusFree(pDev)) {
         I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] QUP_Completion(), After FORCED LOW Wait for IDLE failed", pDev->uPlatDevId);     
      }
   }

   /*
    *  To Avoid race condition between writes posted, Which not yet gone through AHB bus 
    *  AND calling thread disabling clocks via Drv_Close, hence read back interrupt status here   
    *  to confirm write is done.
    */
	
   for(;;)
   {
	  uint32	uQupSvcFlags = 0;
	  uint32	uQupErrFlags = 0;		
		
   	  /* Disable input output interrupts. */
      HAL_qupe_SetIrqMask(pDev->devProps.virtBlockAddr,
                              HAL_QUPE_IRQMASK_INPUT_SERVICE_FLAG |
                              HAL_QUPE_IRQMASK_OUTPUT_SERVICE_FLAG);
	  
      /* Disable qup error interrupts */
      HAL_qupe_SetErrStatusIrqEnabled(pDev->devProps.virtBlockAddr,0);

	  /*Whether all three interrupts are disabled, read back and confirm*/	
	  uQupSvcFlags = HAL_qupe_GetIrqMask(pDev->devProps.virtBlockAddr);
	  uQupErrFlags = HAL_qupe_GetErrStatusIrqEnabled(pDev->devProps.virtBlockAddr); 
	  
      if( (uQupSvcFlags & HAL_QUPE_IRQMASK_INPUT_SERVICE_FLAG) &&
	  	  (uQupSvcFlags & HAL_QUPE_IRQMASK_OUTPUT_SERVICE_FLAG) && 
          (!uQupErrFlags))
      {
        break;
      }
	  
   } 
   
}

/**
    This function checks if this call is synchronous with
    interrupts enabled.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         TRUE if this call is synchronous and interrupts enabled,
                    FALSE otherwise.
  */
static boolean
QUP_IsSyncEvtCall
(
   I2CDEVQUP_Device               *pDev
)
{
   return ( pDev->devProps.bInterruptBased && 
        ( NULL == pDev->asyncCall.pCallbackFn ) );
}

/**
    This function checks if this call is asynchronous with
    interrupts enabled.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         TRUE if this call is asynchronous and interrupts enabled,
                    FALSE otherwise.
  */
static boolean
QUP_IsAsyncCall
(
   I2CDEVQUP_Device               *pDev
)
{
   return ( pDev->devProps.bInterruptBased && 
        ( NULL != pDev->asyncCall.pCallbackFn ) );
}

/**
    This function performs calls callbacks or sets events.
    Used in bam mode.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void 
QUP_NotifySyncEvtCall
(
   I2CDEVQUP_Device               *pDev
)
{
   int32 res;

   res = (int32)I2CSYS_SetEvent(pDev->hQupEvt);
   if ( I2C_RES_SUCCESS == pDev->seqInfo.ioRes.nOperationResult ) {
      pDev->seqInfo.ioRes.nOperationResult = res;
   }     
}

/**
    This function performs calls callbacks or sets events.
    Used in bam mode.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
/*
static void 
QUP_AsyncCallbackAsyncCall
(
   I2CDEVQUP_Device               *pDev
)
{
   *pDev->asyncCall.pIoRes      = pDev->seqInfo.ioRes;
   pDev->asyncCall.pCallbackFn(pDev->asyncCall.pArg);   
}
*/

/**
    This function processes the qup hw events .

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
void
I2CDEVQUP_ProcesQupHwEvts
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo     *pQupInfo;
   I2CDEVQUP_QupIsrHwEvt *pIsrEvt;

   /* I2CPLATDEV_AcquireIntLock(pDev->hDevTgt); */
   pQupInfo = &pDev->qupInfo;
   
   while ( !I2CDEVQUP_QupHwEvtQueue_IsEmpty(&pQupInfo->hwEvtQueue) ) {
      /* Tail is modified the I2CDEVQUP_QupHwEvtQueue_TailNext. */     
      pIsrEvt  = pQupInfo->hwEvtQueue.pTail; 
      I2CDEVQUP_QupHwEvtQueue_ProcessIsrEvt(pDev, pIsrEvt);     
      I2CDEVQUP_QupHwEvtQueue_TailNext(&pQupInfo->hwEvtQueue);
   }
   /* I2CPLATDEV_ReleaseIntLock(pDev->hDevTgt); */
}

/**
    This function processes the bam events .

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
void
I2CDEVQUP_ProcesBamEvts
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;


   pQupInfo    = &pDev->qupInfo;
   pSeqInfo    = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   switch ( pQupInfo->eBamEvt ) {
      case I2CPLATBAM_EVT_BAM_ERROR:
           /* TODO: should not happen abort transfer. */
           pDev->seqInfo.ioRes.nOperationResult = I2CDEVQUP_ERR_BAM_ERROR;
           break;
      case I2CPLATBAM_EVT_TX_EOT:
           pSeqInfo->outSeqInfo.eOutputState = I2CDEVQUP_OUTPUT_TRANSMIT_DONE;
           pQupInfo->uCurOutTransfers = HAL_qupe_GetOutTransferCountCurrentExt(
                           pDev->devProps.virtBlockAddr,
                           pQupInfo->eOutFifoMode,
                           pOutSeqInfo->uTotalOutBytes);
           break;
      case I2CPLATBAM_EVT_TX_DESC_DONE: /* INTERRUPT fired. */
           pSeqInfo->outSeqInfo.eOutputState = I2CDEVQUP_OUTPUT_FILL_FIFO;
           break;
      case I2CPLATBAM_EVT_RX_EOT:
           pSeqInfo->inSeqInfo.eInSeqState = I2CDEVQUP_INPUT_RECEIVE_DONE;
           pQupInfo->uCurInTransfers  =  HAL_qupe_GetInTransferCountCurrentExt(
                           pDev->devProps.virtBlockAddr,
                           pQupInfo->eInFifoMode,
                           pInSeqInfo->uTotalInBytes);
           break;
      case I2CPLATBAM_EVT_RX_DESC_DONE:/* INTERRUPT fired */
           pSeqInfo->inSeqInfo.eInSeqState = I2CDEVQUP_INPUT_PUSH_DESCRIPTORS;
	   break;

      case I2CPLATBAM_EVT_NULL: /*  */
           break;
   }

   I2C_CALLDATA_LEVEL2(pDev->pDevLog, 4,
                       "DEV:0x%x: I2CDEVQUP_ProcesBamEvts: bam_evt=0x%x, eOutputState=0x%x, eInSeqState=0x%x",
                       pDev->uPlatDevId, pQupInfo->eBamEvt, pSeqInfo->outSeqInfo.eOutputState, pSeqInfo->inSeqInfo.eInSeqState);
   pQupInfo->eBamEvt = I2CPLATBAM_EVT_NULL;
}


/**
    This function performs sequence input/output.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_PerformInputOutput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   boolean                         bComplete;
   I2CDEVQUP_AsyncCallData         asyncCall;
   boolean                         bAsyncNotify;
   uint32                          uWhileOnce = 0;
   


   pQupInfo = &pDev->qupInfo;
   pSeqInfo = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   
   bComplete        = FALSE;
   bAsyncNotify     = FALSE;

   if( I2CDEV_POWER_STATE_2 != pDev->devPowerState ) {
      I2C_CALLDATA_LEVEL2(pDev->pDevLog, 2,
                        "DEV:0x%x: QUP_PerformInputOutput:Exit not servicable devPowerState=0x%x",
                         pDev->uPlatDevId, pDev->devPowerState);     
	  
      return;     
   }
   
   I2CSYS_EnterCriticalSection(pDev->hOperationSync);
   I2C_CALLTRACE_LEVEL2(pDev->pDevLog, 2,
                        "DEV:0x%x: QUP_PerformInputOutput pDev=0x%x",
                         pDev->uPlatDevId, (uint32) pDev);
   do {
       
      I2CDEVQUP_ProcesQupHwEvts(pDev);
      I2CDEVQUP_ProcesBamEvts(pDev);
      
      asyncCall = pDev->asyncCall;

      if ( I2CDEVQUP_SEQSTATE_INPUT_OUTPUT != pSeqInfo->eSeqState ) {
         break;     
      }

      /* Check for qup hw errors. This should not happen.
         It indicates most likely a programming error.
       */
      if ( pQupInfo->uQupErrors & HAL_QUPE_ERRSTATE_FIELD ) {
         pSeqInfo->ioRes.nOperationResult = I2CDEVQUP_ERR_BASE |
                     (pQupInfo->uQupErrors & HAL_QUPE_ERRSTATE_FIELD);
         I2CDEVQUP_ResetQupCore(pDev);
         HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
         bComplete = TRUE;
         break;
      }

      /* Check if there is a transfer to be placed on the out fifo. */
      if ( I2CDEVQUP_OUTPUT_FILL_FIFO == pOutSeqInfo->eOutputState ) {
         pDev->pFnPerformOutput(pDev);
      }

      /* Input sequence handling.
         If there is an error collect as much input as possible.
         All the data in the input fifo.
       */
      if ( I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO == pInSeqInfo->eInSeqState ) {
         pDev->pFnPerformInput(pDev);
      }

      /* Process any I2C errors */
      if ( pQupInfo->i2cStatus.uRawStatus & HAL_QUPE_I2C_OP_STATUS_ERROR_FIELD ) {
         pSeqInfo->ioRes.nOperationResult = I2C_RES_ERROR_CLS_I2C_CORE;
         pSeqInfo->ioRes.nOperationResult |=
             HAL_QUPE_PACK28_I2C_STATE(pQupInfo->i2cStatus.uRawStatus,
                                       pQupInfo->i2cStatus.uRawLineState);
         /* Handle the case where the slave stretches the clock after NACK */
         if (pQupInfo->i2cStatus.uRawStatus & HAL_QUPE_I2C_OP_STATUS_PACKET_NACKED) {
             QUP_WaitForI2cBusFree(pDev);
         }

         I2CDEVQUP_ResetQupCore(pDev);
         HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
         bComplete = TRUE;
         break;
      }

      /* When we get an I2C error interrupt in BAM mode, we should not push
       * descriptors */
      if ( I2CDEVQUP_INPUT_PUSH_DESCRIPTORS == pInSeqInfo->eInSeqState ) {
         pDev->pFnPerformInput(pDev);
      }

      if ( I2C_RES_SUCCESS != pSeqInfo->ioRes.nOperationResult ) {
         bComplete = TRUE;
         break;     
      }

      /* If the transfer has already finished do completion. */
      if ( (I2CDEVQUP_OUTPUT_TRANSMIT_DONE == pOutSeqInfo->eOutputState) &&
           (0 == pInSeqInfo->uTotalInBytes) ) {
         bComplete = TRUE;
         break;
      }

      if ( (I2CDEVQUP_INPUT_RECEIVE_DONE == pInSeqInfo->eInSeqState) &&
           (I2CDEVQUP_OUTPUT_TRANSMIT_DONE == pOutSeqInfo->eOutputState) ) {
         bComplete = TRUE;
         break;
      }
   } while ( uWhileOnce );

   I2C_CALLDATA_LEVEL2(pDev->pDevLog, 5,
                        "DEV:0x%x: QUP_PerformInputOutput eOutputState=0x%x eInSeqState=0x%x"
                        " nOperationResult=0x%x bComplete=0x%x",
                         pDev->uPlatDevId, pOutSeqInfo->eOutputState, pInSeqInfo->eInSeqState,
                         pSeqInfo->ioRes.nOperationResult, bComplete);

   if ( bComplete ) {
      pSeqInfo->eSeqState = I2CDEVQUP_SEQSTATE_COMPLETING;
      QUP_Completion(pDev);
      /*
          The synchronous event notification needs to be done within critical
          section otherwise there is a chance that the notification could go
          to the next transfer if the previous one terminated and the next one
          scheduled.
       */
      if ( QUP_IsSyncEvtCall(pDev) ) {
         QUP_NotifySyncEvtCall(pDev);
      }
      else if ( QUP_IsAsyncCall(pDev) ) {
         /* ... store the callback arguments within crit section */
         *pDev->asyncCall.pIoRes      = pDev->seqInfo.ioRes;
         asyncCall = pDev->asyncCall;
         bAsyncNotify = TRUE;
      }
   }
   
   I2CSYS_LeaveCriticalSection(pDev->hOperationSync);

   if ( bAsyncNotify ) {
      asyncCall.pCallbackFn(asyncCall.pArg); 
   }
   
   return;
}

/** @brief This is a hook for the thread servicing the qup.

    This Function is called by the framework to handle the data
    transfer in response to qup irq events.

    @param[in] pdata    void pointer to data.

    @return             Nothing.
  */
static void 
I2CDEVQUP_IST_HOOK
(
   void *pdata
)
{
   if ( NULL == pdata ) {
      return;
   }
   QUP_PerformInputOutput((I2CDEVQUP_Device*)pdata);
}



/** @brief This function initializes the output transfer info structure.

    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void 
QUP_OutSeqInitTransfer
(
   I2CDEVQUP_OutSeqInfo       *pOutSeqInfo
)
{
   
   pOutSeqInfo->outTr.uNextDataSegIndex = 0;
   pOutSeqInfo->outTr.uDataIndex = 0;


   if ( I2CDEVQUP_IS_FIRST_HS_TRANSFER(pOutSeqInfo->pSeq, pOutSeqInfo->pTransfer) ) {
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_MASTER_START_TAG;     
   }
   else {
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_START_TAG;     
   }

   if ( I2cTranDirOut == pOutSeqInfo->pTransfer->eTranDirection ) {
      pOutSeqInfo->outTr.pTrIOVec  = pOutSeqInfo->pTransfer->pI2cBuffDesc;
      pOutSeqInfo->outTr.IoVecBuffIndex = 0;
   }
   else { /* input transfer. */
      /* The current byte offset into the slot being prepared.
       * If the next transfer is single byte read then we need to pad
       * in order to avoid reading more than required by the transfer
       * due to tag splitting over different slots. */
      if ( 1 == pOutSeqInfo->pTransfer->uTrSize ) {    
         if ( pOutSeqInfo->uCurOutBytes & 0x3 ) { /* pad 3 bytes. */
            pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_PAD_OUTPUT;
         }
      }
   }
}

/** @brief This function initializes the input transfer info structure.
      
    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void 
QUP_InSeqInitTransfer
(
   I2CDEVQUP_InSeqInfo       *pInSeqInfo
)
{
    pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
    pInSeqInfo->inTr.uDataIndex = 0;
    pInSeqInfo->inTr.uNextDataSegIndex = 0;

    pInSeqInfo->inTr.pTrIOVec   = pInSeqInfo->pTransfer->pI2cBuffDesc;
    pInSeqInfo->inTr.IoVecBuffIndex = 0;
}

/******************************************************************************
 * The next group of functions handle output sequence/transfer state
 * transitions.
 *****************************************************************************/
/** @brief This function moves the output transfer pointer to next transfer.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void
QUP_OutFsm_MoveNextTransfer
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo
)
{
   pOutSeqInfo->uTrIndex++;
   if ( pOutSeqInfo->uTrIndex == pOutSeqInfo->pSeq->uNumTransfers ) {
      /* all data has been pushed into the output fifo. */
      /*
       * QCTDD00309260 prevents the driver from adding a FLUSH_STOP tag to
       * terminate the transfer if fifo/block mode.
       * QCTDD00309260:
       * When using i2c version2 tags in non-BAM mode, MAX_OUTPUT_DONE_FLAG
       * is not asserted if the last tag is not DATA_WR_*, DATA_RD_* or
       * NOP_WAIT.
       */
      /* Do not add FLUSH_STOP to handle QCTDD00352578.
       *  See issue above  QCTDD00309260 
       */
      pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_FILL_FIFO_DONE;
      
   }
   else { /* initialize next transfer.*/
      pOutSeqInfo->pTransfer++; /* move to next transfer. */
      QUP_OutSeqInitTransfer(pOutSeqInfo);
   }
}

/** @brief This function handles transfer transition from output pad Tag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_PadOutputTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint32   uSlotByteOffset;

   uSlotByteOffset = pOutSeqInfo->uCurOutBytes & 0x3;   
   if ( 3 == uSlotByteOffset ){
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_START_TAG;
   }

   *pData = HAL_QUPE_TAG_FLUSH_STOP;
   
   pOutSeqInfo->uCurOutBytes++;
}


/** @brief This function handles transfer transition from MasterStartTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_MasterStartTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   *pData = HAL_QUPE_TAG_START;
   pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_MASTER_ADDR;

   pOutSeqInfo->uCurOutBytes++;
}

/** @brief This function handles transfer transition from MasterAddress.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_MasterAddressNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   *pData = I2C_HS_MASTER_ADDRESS;
   pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_START_TAG;
   pOutSeqInfo->uCurOutBytes++;
}

/** @brief This function handles transfer transition from StartTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_StartTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   if ( (I2cTranDirOut == pOutSeqInfo->pTransfer->eTranDirection ) &&
        (0 == pOutSeqInfo->pTransfer->uTrSize) ){
      *pData = HAL_QUPE_TAG_START_STOP;
   }
   else {
      *pData = HAL_QUPE_TAG_START;
   }
   pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_SLAVE_ADDR;
   pOutSeqInfo->uCurOutBytes++;
}



/** @brief This function handles transfer transition from SlaveAddress.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_SlaveAddressNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   
   utmpVal =  (uint8) (pOutSeqInfo->pTransfer->tranCfg.uSlaveAddr<<1);
   if ( I2cTranDirIn == pOutSeqInfo->pTransfer->eTranDirection ) {
      utmpVal |=    1;
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
   }
   else if ( 0 == pOutSeqInfo->pTransfer->uTrSize ) {
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   }
   else {
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_WR_DATA_TAG;
   }
   pOutSeqInfo->uCurOutBytes++;
   *pData = utmpVal;
}

/** @brief This function handles transfer transition from RdDataTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_RdDataTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   uint32 uBytesLeft;
   
   if ( (pOutSeqInfo->pTransfer->uTrSize -
         pOutSeqInfo->outTr.uNextDataSegIndex) >
                     MAX_DATA_SEGMENT_LENGTH ) {
      pOutSeqInfo->outTr.uNextDataSegIndex +=MAX_DATA_SEGMENT_LENGTH;
      utmpVal =    HAL_QUPE_TAG_DATA_READ;
      /* add special condition to handle the last 1 byte segment
         make the previous segment shorter and leave 2 bytes on the last
         segment. */
      uBytesLeft = pOutSeqInfo->pTransfer->uTrSize - pOutSeqInfo->outTr.uNextDataSegIndex -1;
      if ( 1 == uBytesLeft ) {
         pOutSeqInfo->outTr.uNextDataSegIndex--;
      }
   }
   else {
      pOutSeqInfo->outTr.uNextDataSegIndex =
              pOutSeqInfo->pTransfer->uTrSize;
      
      if ((I2cTrCtxSeqStart == pOutSeqInfo->pTransfer->eTranCtxt) ||
          (I2cTrCtxSeqContinue == pOutSeqInfo->pTransfer->eTranCtxt)) {
         utmpVal = HAL_QUPE_TAG_DATA_READ_NACK;
      }
      else {
         utmpVal = HAL_QUPE_TAG_DATA_READ_STOP;
      }
   }
   pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_DATA_CNT;
   pOutSeqInfo->uCurOutBytes++;
   *pData = utmpVal;
}

/** @brief This function handles transfer transition from WrDataTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void
QUP_OutFsm_WrDataTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   if ( (pOutSeqInfo->pTransfer->uTrSize -
         pOutSeqInfo->outTr.uNextDataSegIndex) >
                     MAX_DATA_SEGMENT_LENGTH) {// shift the next segment
      pOutSeqInfo->outTr.uNextDataSegIndex +=MAX_DATA_SEGMENT_LENGTH;
      utmpVal =    HAL_QUPE_TAG_DATA_WRITE;
   }
   else {
      pOutSeqInfo->outTr.uNextDataSegIndex =
                        pOutSeqInfo->pTransfer->uTrSize;
      if ( (I2cTrCtxSeqStart == pOutSeqInfo->pTransfer->eTranCtxt)   ||
           (I2cTrCtxSeqContinue == pOutSeqInfo->pTransfer->eTranCtxt) ) {
         utmpVal =    HAL_QUPE_TAG_DATA_WRITE;
      }
      else {
         utmpVal = HAL_QUPE_TAG_DATA_WRITE_STOP;
      }
   }
   pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_DATA_CNT;
   pOutSeqInfo->uCurOutBytes++;
   *pData = utmpVal;
}



/** @brief This function handles transfer transition from DataCnt.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_DataCntNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 uByteCnt;

   uByteCnt = (uint8) (pOutSeqInfo->outTr.uNextDataSegIndex -
                            pOutSeqInfo->outTr.uDataIndex);
   if ( 0 == uByteCnt ) { 
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   }
   else if ( I2cTranDirIn == pOutSeqInfo->pTransfer->eTranDirection ) {
      pOutSeqInfo->outTr.uDataIndex += uByteCnt;
      if ( pOutSeqInfo->outTr.uDataIndex == pOutSeqInfo->pTransfer->uTrSize ) {
         //last read and bam transferthen next is EOT
         if ( pOutSeqInfo->pSeq->bBamTransfer &&
              (pOutSeqInfo->pSeq->inSeqInfo.uLastTrIndex == pOutSeqInfo->uTrIndex) ) {
            pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_EOT_TAG;
         }
         else {
            QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
         }
      }
      else {
         pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
      }
   }
   else {
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_DATA_VALUE;
   }
   pOutSeqInfo->uCurOutBytes++;
   *pData = uByteCnt;
}

/** @brief This function handles transfer transition from DataValue.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void 
QUP_OutFsm_DataValueNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,uint8* pData
)
{
   uint8 utmpVal;
   
   if ( pOutSeqInfo->outTr.IoVecBuffIndex == pOutSeqInfo->outTr.pTrIOVec->uBuffSize ) {
      /* get next buffer. */
      pOutSeqInfo->outTr.pTrIOVec++;
      pOutSeqInfo->outTr.IoVecBuffIndex = 0;
   }
   /* gather index. */
   utmpVal = pOutSeqInfo->outTr.pTrIOVec->pBuff[pOutSeqInfo->outTr.IoVecBuffIndex++];
   pOutSeqInfo->outTr.uDataIndex++;
   pOutSeqInfo->uCurOutBytes++;
   *pData = utmpVal;

   if ( pOutSeqInfo->outTr.uDataIndex == pOutSeqInfo->pTransfer->uTrSize ) {
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   } 
   else if ( pOutSeqInfo->outTr.uDataIndex ==
             pOutSeqInfo->outTr.uNextDataSegIndex ) {
      /* next segment. */
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_WR_DATA_TAG;
   }
}

/** @brief This function handles transfer transition from DataValue.

    This function moves a buffer at a time.
    This is used in bam mode only all other functions move a byte at a time.

    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     ppData        Pointer to pointer to buffer where the data chunk is.
    @param[out]     pSize         Pointer to size of the data chunk.
    @return         Nothing.
  */
static void
QUP_OutFsm_DataValueNextBuff
(
   I2CDEVQUP_OutSeqInfo  *pOutSeqInfo,
   uint8                **ppData,
   uint32                *pSize
)
{
   uint8 *ptmpData;
   uint32 uBuffSize;
   uint32 uNumTotalRemaining;
   uint32 uNumToEndDataSeg;
   
   if ( pOutSeqInfo->outTr.IoVecBuffIndex == pOutSeqInfo->outTr.pTrIOVec->uBuffSize ) {
      /* get next buffer. */
      pOutSeqInfo->outTr.pTrIOVec++;
      pOutSeqInfo->outTr.IoVecBuffIndex = 0;
   }
   
   ptmpData = &pOutSeqInfo->outTr.pTrIOVec->pBuff[pOutSeqInfo->outTr.IoVecBuffIndex];
   uBuffSize = pOutSeqInfo->outTr.pTrIOVec->uBuffSize -
                pOutSeqInfo->outTr.IoVecBuffIndex;
   uNumTotalRemaining = pOutSeqInfo->pTransfer->uTrSize -
                    pOutSeqInfo->outTr.uDataIndex;
   uNumToEndDataSeg = pOutSeqInfo->outTr.uNextDataSegIndex - pOutSeqInfo->outTr.uDataIndex;

   /* lowest number. */
   if ( uBuffSize > uNumTotalRemaining ) {
      uBuffSize = uNumTotalRemaining;
   }
   if ( uBuffSize > uNumToEndDataSeg ) {
      uBuffSize = uNumToEndDataSeg;
   }

   /* advance data pointer. */
   pOutSeqInfo->outTr.uDataIndex     += uBuffSize;
   pOutSeqInfo->outTr.IoVecBuffIndex += uBuffSize;

   if ( uBuffSize == uNumTotalRemaining ) {
      QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   } 
   else if ( uBuffSize == uNumToEndDataSeg ) {
      /* next segment. */
      pOutSeqInfo->outTr.eOutTrState = I2CDEVQUP_TR_STATE_WR_DATA_TAG;
   }
   pOutSeqInfo->uCurOutBytes += uBuffSize;
   *ppData = ptmpData;
   *pSize = uBuffSize;
}

/** @brief This function handles transfer transition from EotTag.
      
    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static void
QUP_OutFsm_EotTagNext
(
   I2CDEVQUP_OutSeqInfo *pOutSeqInfo,
   uint8                *pData
)
{
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;

   pInSeqInfo = &pOutSeqInfo->pSeq->inSeqInfo;

   QUP_OutFsm_MoveNextTransfer(pOutSeqInfo);
   pOutSeqInfo->uCurOutBytes++;
   pInSeqInfo->uInPadCnt++; /* eot will be copied to input. */
   *pData = HAL_QUPE_TAG_INPUT_EOT;
}





/** @brief This function drives output seq state transitions.

    It advances the output sequence one slot at the time.
    This function is used to fill the output fifo in fifo/block mode.

    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pData         Data produced from this state-transition.
    @return         Nothing.
  */
static uint32 
QUP_GetNextOutSeqSlot
(
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo   
)
{
   uint32                          uSlot;
   uint32                          uSlotByteIndex;
   uint8                           uByteVal;

   uSlot = 0;
   uSlotByteIndex = 0;

   /* create slot. */
   while ( (uSlotByteIndex < 4) &&
	        (pOutSeqInfo->eOutputState != I2CDEVQUP_OUTPUT_FILL_FIFO_DONE) ) {

      switch ( pOutSeqInfo->outTr.eOutTrState ) {
         case I2CDEVQUP_TR_STATE_PAD_OUTPUT       : /* start of a transfer. */
               QUP_OutFsm_PadOutputTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_MASTER_START_TAG       : /* start of a transfer. */
               QUP_OutFsm_MasterStartTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_MASTER_ADDR       : /* start of a transfer. */
               QUP_OutFsm_MasterAddressNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_START_TAG       : /* start of a transfer. */
               QUP_OutFsm_StartTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_SLAVE_ADDR      :
               QUP_OutFsm_SlaveAddressNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_RD_DATA_TAG     :
               QUP_OutFsm_RdDataTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_WR_DATA_TAG     :
               QUP_OutFsm_WrDataTagNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_DATA_CNT        :
               QUP_OutFsm_DataCntNext(pOutSeqInfo,&uByteVal);
               break;
         case I2CDEVQUP_TR_STATE_DATA_VALUE      :
               QUP_OutFsm_DataValueNext(pOutSeqInfo,&uByteVal);
               break;
         
         default:
              /*TODO: set error sate of invalid output seq state. */
              return 0;
      }
      uSlot |= (((uint32)uByteVal ) << (uSlotByteIndex * 8));
      uSlotByteIndex++;
   }
   return uSlot;   
}

/**
    This function performs output on the output fifo if there are free slots.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void 
QUP_PerformOutput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   uint32                          uSlot;

   pQupInfo = &pDev->qupInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;

   
   /* If transfer to be queued Check if there is space in the output fifo
      If to be qued and space continue state machine. */

   /* If output is done update the state of the output sequence. */
   while ( (I2CDEVQUP_OUTPUT_FILL_FIFO == pOutSeqInfo->eOutputState) &&
          (pQupInfo->uOutFreeSlotReqCnt > pQupInfo->uOutFreeSlotServicedCnt) ) {
      uSlot = QUP_GetNextOutSeqSlot(pOutSeqInfo);
      /* push slot into output fifo. */
      HAL_qupe_PutWordIntoOutFifo(pDev->devProps.virtBlockAddr, uSlot);
      pQupInfo->uOutFreeSlotServicedCnt++;

   }
}

/**
    This function handles bam callbacks.

    @param[in]      bamPlatRes  Bam callback result structure.
    @return         Nothing.
  */
void 
QUP_BamCallback
(
   I2CPLATBAM_CallbackResultType bamPlatRes
)
{
   I2CDEVQUP_Device               *pDev;
   I2CDEVQUP_QupInfo              *pQupInfo;

   pDev = (I2CDEVQUP_Device *)bamPlatRes.pArg;

   pQupInfo = &pDev->qupInfo;
   
   I2CSYS_EnterCriticalSection(pDev->hOperationSync);
   pQupInfo->eBamEvt = bamPlatRes.bamEvt;
   I2CSYS_LeaveCriticalSection(pDev->hOperationSync);
   I2C_CALLDATA_LEVEL2(pDev->pDevLog, 3,
                       "DEV:0x%x: QUP_BamCallback: raw bam_evt=0x%x: eBamEvt=0x%x",
                       pDev->uPlatDevId, bamPlatRes.bamEvt, pQupInfo->eBamEvt);
   QUP_PerformInputOutput(pDev);
}

/** @brief This function drives output seq state transitions in bam mode.

    It advances the output sequence one descriptor at the time.
    This function is used to fill the output fifo in bam mode.

    @param[in, out] pOutSeqInfo   Pointer to output sequence info structure.
    @param[out]     pBamIoVec     Pointer to bam io vector.
    @return         Nothing.
  */
static int32
QUP_GetNextOutSeqDescriptor
(
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo,
   I2CPLATBAM_BamIoVecType        *pBamIoVec 
)
{
   uint8                          *pDataAddr; /* used for unbuffered tr. */
   uint32                          uDataSize; /* used for unbuffered tr. */
   uint8                          *pBamBuffAddr;/* used for buffered tr. */
   uint16                          uFlags;
   boolean                         bDescReady;
   boolean                         bUsesBamBuffer;
   

   uFlags = 0;
   pDataAddr = NULL;
   uDataSize = 0;

   pBamBuffAddr = &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex];

   bDescReady = FALSE;
   bUsesBamBuffer = TRUE;
   /* The driver loops under two scenarios:
      1. no descriptor is ready to be pushed into the bam
         and the transfer is buffered (tags,addr ... etc)
      2. no descriptor is ready and the client data is being
         pushed into the bam so no need for buffering.  */
   /* create descriptor. */
   while( (!bDescReady) &&
          ((pOutSeqInfo->uBamBuffIndex < BAM_TMPBUFF_MAX) ||
          (I2CDEVQUP_TR_STATE_DATA_VALUE == pOutSeqInfo->outTr.eOutTrState)) &&
          (pOutSeqInfo->eOutputState != I2CDEVQUP_OUTPUT_FILL_FIFO_DONE) ) {
      switch ( pOutSeqInfo->outTr.eOutTrState ) {
         case I2CDEVQUP_TR_STATE_PAD_OUTPUT       : /* start of a transfer. */
            QUP_OutFsm_PadOutputTagNext(pOutSeqInfo,
                                        &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_MASTER_START_TAG       : /* start of a transfer. */
               QUP_OutFsm_MasterStartTagNext(pOutSeqInfo,
                                             &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
               break;
         case I2CDEVQUP_TR_STATE_MASTER_ADDR       : /* start of a transfer. */
               QUP_OutFsm_MasterAddressNext(pOutSeqInfo,
                                            &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
               break;   
            break;     
         case I2CDEVQUP_TR_STATE_START_TAG       : /* start of a transfer. */
            QUP_OutFsm_StartTagNext(pOutSeqInfo,
                                    &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_SLAVE_ADDR      :
            QUP_OutFsm_SlaveAddressNext(pOutSeqInfo,
                                        &pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_RD_DATA_TAG        :
            QUP_OutFsm_RdDataTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_WR_DATA_TAG        :
            QUP_OutFsm_WrDataTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;
         case I2CDEVQUP_TR_STATE_DATA_CNT        :
            QUP_OutFsm_DataCntNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            /* if next state is data value then flush the buffer. */
            if ( I2CDEVQUP_TR_STATE_DATA_VALUE == pOutSeqInfo->outTr.eOutTrState ) {
               bDescReady = TRUE;
            }
            break;
         case I2CDEVQUP_TR_STATE_DATA_VALUE      :
            QUP_OutFsm_DataValueNextBuff(pOutSeqInfo,&pDataAddr,&uDataSize);
            bDescReady = TRUE;
            bUsesBamBuffer = FALSE;/* the data is the raw clnt data. */
            break;
         case I2CDEVQUP_TR_STATE_EOT_TAG:
            QUP_OutFsm_EotTagNext(pOutSeqInfo,&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex++]);
            break;

         default:
            // do nothing
            break;
      }

      if ( BAM_TMPBUFF_MAX == pOutSeqInfo->uBamBuffIndex ) {
         return I2CDEVQUP_ERR_OUT_OF_OUTPUT_TAG_BUFFER;    
      }
   }
   /* last descriptor for output transfer. */
   if ( pOutSeqInfo->eOutputState == I2CDEVQUP_OUTPUT_FILL_FIFO_DONE ) {
      uFlags |= ( PLAT_BAM_DESC_FLAG_EOT |
                  PLAT_BAM_DESC_FLAG_NWD |
                  PLAT_BAM_DESC_FLAG_INT );
   }
      
   /* pick the buffer and size. */
   if (!bUsesBamBuffer) {/* this descriptor points to clnt data. */
      pBamIoVec->pBuff = pDataAddr;
      pBamIoVec->buffSize = (uint16)uDataSize;
   }
   else {
      /* this descriptor points to the allocated buffer.*/
      pBamIoVec->pBuff = pBamBuffAddr;
      pBamIoVec->buffSize = (uint16) (&pOutSeqInfo->aOutBamBuff[pOutSeqInfo->uBamBuffIndex] - pBamBuffAddr);
      pBamBuffAddr += pBamIoVec->buffSize; 
   }
   /* push slot into output fifo. */
   pBamIoVec->eDir = I2CPLATBAM_DIR_OUTPUT;
   pBamIoVec->flags = uFlags;

   return I2C_RES_SUCCESS;
}

/**
    This function fills the output queue one descriptor at a time.
   
    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_BamPerformOutput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   uint32                          uNumFreeDesc;
   I2CPLATBAM_BamIoVecType         bamIoVec;
   int32                           res;
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
  
   pSeqInfo = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   res = I2C_RES_SUCCESS;
   /* If transfer to be queued Check if there is space in the bam queue. */
   uNumFreeDesc = I2CPLATDEV_BamGetPipeFreeSpace(pDev->hDevTgt,
                                                 I2CPLATBAM_DIR_OUTPUT);

   /* If output is done update the state of the output sequence. */
   while( (I2CDEVQUP_OUTPUT_FILL_FIFO == pOutSeqInfo->eOutputState) &&
          (uNumFreeDesc > 0) ) {
      res = QUP_GetNextOutSeqDescriptor(pOutSeqInfo, &bamIoVec);
      if ( I2C_RES_SUCCESS != res ) {
         pSeqInfo->ioRes.nOperationResult = res;
         break;  
      }
      res = I2CPLATDEV_PlatBamTransfer(pDev->hDevTgt,
                                       &bamIoVec,
                                       &uNumFreeDesc,
                                       QUP_BamCallback,
                                       (void *)pDev);
      if ( I2C_RES_SUCCESS != res ) {
         pSeqInfo->ioRes.nOperationResult = res;
         break;  
      }
   }
   if ( I2C_RES_SUCCESS != res ) {
      pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_ERROR;     
      I2CDEV_AbortTransfer(pDev);     
   }
}


/******************************************************************************
 * The next group of functions handle input sequence/transfer state
 * transitions.
 *****************************************************************************/

/** @brief This function moves the input transfer pointer to next transfer.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @return         Nothing.
  */
static void
QUP_InFsm_MoveNextTransfer
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo
)
{
   pInSeqInfo->uInTrCnt--;
   if ( 0 == pInSeqInfo->uInTrCnt ) {
      /* all data has been read from input fifo */
      pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_NOT_IN_TRANSFER;
      return;
   }
   
   do {
      pInSeqInfo->uTrIndex++;
      pInSeqInfo->pTransfer = &pInSeqInfo->pSeq->pTransfer[pInSeqInfo->uTrIndex];
      if ( I2cTranDirIn == pInSeqInfo->pTransfer->eTranDirection ) {
         break;
      }
   } while(pInSeqInfo->uTrIndex != pInSeqInfo->uLastTrIndex);

   QUP_InSeqInitTransfer(pInSeqInfo);
}

/** @brief This function handles transfer transition from RdDataTag.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @return         Nothing.
  */
static void
QUP_InFsm_RdDataTagNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo
)
{
   uint32 uBytesLeft;

   if ( (pInSeqInfo->pTransfer->uTrSize -
         pInSeqInfo->inTr.uNextDataSegIndex) >
                     MAX_DATA_SEGMENT_LENGTH ) {
      pInSeqInfo->inTr.uNextDataSegIndex +=MAX_DATA_SEGMENT_LENGTH;
      /* add special condition to handle the last 1 byte segment
         make the previous segment shorter and leave 2 bytes on the last
         segment. */
      uBytesLeft = pInSeqInfo->pTransfer->uTrSize - pInSeqInfo->inTr.uNextDataSegIndex -1;
      if (1 == uBytesLeft) {
         pInSeqInfo->inTr.uNextDataSegIndex--;
      }
   }
   else {
      pInSeqInfo->inTr.uNextDataSegIndex =
              pInSeqInfo->pTransfer->uTrSize;
   }
   pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_DATA_CNT;
}

/** @brief This function handles transfer transition from RdDataCntTag.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @return         Nothing.
  */
static void
QUP_InFsm_RdDataCntTagNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo
)
{
   uint8 utmpVal;

   utmpVal = (uint8)(pInSeqInfo->inTr.uNextDataSegIndex -
                            pInSeqInfo->inTr.uDataIndex);
   if ( 0 == utmpVal ) { 
      QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   } else {
      pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_DATA_VALUE;
   }
}

/** @brief This function handles transfer transition from RdDataValue one byte at a time.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @param[out]     uByte         Data with this single byte read from this state-transition.
    @return         Nothing.
  */
static void
QUP_InFsm_RdDataValueSingleNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo,uint8 uByte
)
{
   uint8 *pVal;

   if ( pInSeqInfo->inTr.IoVecBuffIndex == pInSeqInfo->inTr.pTrIOVec->uBuffSize ) {
      /* get next buffer. */
      pInSeqInfo->inTr.pTrIOVec++;
      pInSeqInfo->inTr.IoVecBuffIndex = 0;
   }
   /* gather index. */
   pVal = &pInSeqInfo->inTr.pTrIOVec->pBuff[pInSeqInfo->inTr.IoVecBuffIndex++];
   *pVal = uByte;
   pInSeqInfo->inTr.uDataIndex++;
   if ( pInSeqInfo->inTr.uDataIndex == pInSeqInfo->pTransfer->uTrSize ) {
      QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   } 
   else if ( pInSeqInfo->inTr.uDataIndex ==
             pInSeqInfo->inTr.uNextDataSegIndex ) {
      /* next segment. */
      pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
   }
}

/** @brief This function handles transfer transition from RdDataValue a buffer at a time.

    The data read will be place on the supplied client buffer of the specified size.
    Used in bam mode.

    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @param[out]     pClntDtBuff   Pointer to a client buffer pointer.
    @param[out]     pClntDtSize   Pointer to buffer size.
    @return         Nothing.
  */
static void
QUP_InFsm_RdDataValueMultipleNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo,
   uint8              **pClntDtBuff,
   uint16              *pClntDtSize
)
{
   uint8 *pTmpClntDtBuff;
   uint16 uBuffSize;
   uint32 uNumTotalRemaining;
   uint32 uNumToEndDataSeg;

   if ( pInSeqInfo->inTr.IoVecBuffIndex == pInSeqInfo->inTr.pTrIOVec->uBuffSize ) {
      /* get next buffer. */
      pInSeqInfo->inTr.pTrIOVec++;
      pInSeqInfo->inTr.IoVecBuffIndex = 0;
   }

   /* gather index. */
   pTmpClntDtBuff = &pInSeqInfo->inTr.pTrIOVec->pBuff[pInSeqInfo->inTr.IoVecBuffIndex];
   uBuffSize = (uint16)(pInSeqInfo->inTr.pTrIOVec->uBuffSize - pInSeqInfo->inTr.IoVecBuffIndex);

   uNumTotalRemaining = pInSeqInfo->pTransfer->uTrSize - pInSeqInfo->inTr.uDataIndex;
   uNumToEndDataSeg = pInSeqInfo->inTr.uNextDataSegIndex - pInSeqInfo->inTr.uDataIndex;

   /* lowest number. */
   if ( uBuffSize > uNumTotalRemaining ) {
      uBuffSize = (uint16) uNumTotalRemaining;
   }
   if ( uBuffSize > uNumToEndDataSeg ) {
      uBuffSize = (uint16) uNumToEndDataSeg;
   }

   /* advance data pointer. */
   pInSeqInfo->inTr.uDataIndex     += uBuffSize;
   pInSeqInfo->inTr.IoVecBuffIndex += uBuffSize;

   if ( pInSeqInfo->inTr.uDataIndex == pInSeqInfo->pTransfer->uTrSize ) {
      /* if the last transfer then move to eot. */
      if ( 1 == pInSeqInfo->uInTrCnt ) {
         pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_DATA_PAD;
      }
      else {
         QUP_InFsm_MoveNextTransfer(pInSeqInfo);
      }
   } 
   else if ( pInSeqInfo->inTr.uDataIndex ==
             pInSeqInfo->inTr.uNextDataSegIndex ) {
      /* next segment. */
      pInSeqInfo->inTr.eInTrState = I2CDEVQUP_TR_STATE_RD_DATA_TAG;
   }
   *pClntDtBuff  = pTmpClntDtBuff;
   *pClntDtSize = uBuffSize;
}

/** @brief This function handles transfer transition from RdTrailer.

    The new state is a wait for the memory to be flushed.

    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @return         Nothing.
  */
static void
QUP_InFsm_RdTrailerNext
(
   I2CDEVQUP_InSeqInfo *pInSeqInfo
)
{
   QUP_InFsm_MoveNextTransfer(pInSeqInfo);
   pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_RECEIVE_DONE;
}

/** @brief This function advances input sequence one slot at a time.
      
    @param[in, out] pInSeqInfo    Pointer to input sequence info structure.
    @param[in]      uSlot         Data slot received.
    @return         Nothing.
  */
static void
QUP_ProcessNextInputSlot
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo,
   uint32                          uSlot
)
{
   uint32                          uSlotByteIndex;
   uint32                          uByteVal;

   uSlotByteIndex = 0;
   
   /* unpack slot. */
   while ( (uSlotByteIndex < 4) && 
           (I2CDEVQUP_TR_STATE_NOT_IN_TRANSFER != pInSeqInfo->inTr.eInTrState) ) {
      uByteVal = (uSlot >> (uSlotByteIndex * 8)) &0xFF;
      uSlotByteIndex++;
      switch (pInSeqInfo->inTr.eInTrState) {
         case I2CDEVQUP_TR_STATE_RD_DATA_TAG        :
               QUP_InFsm_RdDataTagNext(pInSeqInfo);
               break;
         case I2CDEVQUP_TR_STATE_DATA_CNT        :
               QUP_InFsm_RdDataCntTagNext(pInSeqInfo);
               break;
         case I2CDEVQUP_TR_STATE_DATA_VALUE      :
               QUP_InFsm_RdDataValueSingleNext(pInSeqInfo, (uint8) uByteVal);
               break;
         default:
               /* TODO: set error sate of invalid output seq state. */
               return;
      }
   } /* end of slot processing. */

   pInSeqInfo->uNumInBytesReceived += 4;
   
   if ((pInSeqInfo->uNumInBytesReceived >= pInSeqInfo->uTotalInBytes) ||
       (0 == pInSeqInfo->uInTrCnt))    {
      pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_RECEIVE_DONE;
   }
}

/**
    This function empties the input queue in block/fifo mode.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_PerformInput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   uint32                          uSlot;

   pQupInfo    = &pDev->qupInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   while ((I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO == pInSeqInfo->eInSeqState) &&
          (pQupInfo->uInFreeSlotReqCnt > pQupInfo->uInFreeSlotServicedCnt) ) {
      HAL_qupe_GetWordFromInFifo(pDev->devProps.virtBlockAddr, &uSlot); /* get slot from fifo. */
      QUP_ProcessNextInputSlot(pInSeqInfo, uSlot);
      pQupInfo->uInFreeSlotServicedCnt++;
   }
}

/**
    This function advances the input sequence one io vector at a time.
    Used in bam mode.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
/* This implementation generates multiple input descriptors.
   The V1 chip dosn't support canceling/closing multiple
   input descriptors in case of NACK or error.
 */ 
static void
QUP_GetNextInSeqDescriptor
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo,
   I2CPLATBAM_BamIoVecType        *pBamIoVec 
)
{
   uint8                          *pClntDtBuff;
   uint16                          uClntDtSize;
   uint16                          uTagDataSize;
   boolean                         bUsesBamBuffer;
   boolean                         bDescReady;

  
   pClntDtBuff  = NULL;
   uClntDtSize  = 0;
   uTagDataSize = 0;

   bDescReady = FALSE;
   bUsesBamBuffer = TRUE;

   pBamIoVec->flags = 0;

   /* unpack slot. */
   while( !bDescReady ) {
      switch ( pInSeqInfo->inTr.eInTrState ) {
         case I2CDEVQUP_TR_STATE_RD_DATA_TAG        :
               QUP_InFsm_RdDataTagNext(pInSeqInfo);
               break;
         case I2CDEVQUP_TR_STATE_DATA_CNT        :
               QUP_InFsm_RdDataCntTagNext(pInSeqInfo);
               uTagDataSize   = 2;
               bDescReady     = TRUE; /* this is a throw away slot. */
               bUsesBamBuffer = TRUE;
               break;
         case I2CDEVQUP_TR_STATE_DATA_VALUE      :
               QUP_InFsm_RdDataValueMultipleNext(pInSeqInfo,&pClntDtBuff, &uClntDtSize);
               bDescReady = TRUE; /* this is a throw away slot. */
               bUsesBamBuffer = FALSE;
               break;
         case I2CDEVQUP_TR_STATE_DATA_PAD      :
               QUP_InFsm_RdTrailerNext(pInSeqInfo);
               uTagDataSize   = (uint16) pInSeqInfo->uInPadCnt;
               bDescReady     = TRUE; /* this is a throw away slot. */
               bUsesBamBuffer = TRUE;
               pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_WAIT_READ_DATA_READY;
               break;   
         default:
               /* TODO: set error sate of invalid output seq state. */
               return;
      }
   }

   /* pick the buffer and size. */
   if ( !bUsesBamBuffer ) {
      /* this descriptor points to clnt data. */
      pBamIoVec->pBuff    = pClntDtBuff;
      pBamIoVec->buffSize = (uint16) uClntDtSize;
   }
   else {
      /* this descriptor points to the allocated buffer. */
      pBamIoVec->pBuff    = pInSeqInfo->aInBamBuff;
      pBamIoVec->buffSize = uTagDataSize; /* throw away tags. */
   }
   /* push slot into input fifo. */
   pBamIoVec->eDir = I2CPLATBAM_DIR_INPUT;
}





/**
    This function Performs input in bam mode.
    Used in bam mode.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_BamPerformInput
(
   I2CDEVQUP_Device               *pDev
)
{
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   uint32                          uNumFreeDesc;
   I2CPLATBAM_BamIoVecType         bamIoVec;
   int32                           res;

   pSeqInfo    = &pDev->seqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   res = I2C_RES_SUCCESS;

   /* Check if there is a transfer to be placed on the out fifo
      If transfer to be queued Check if there is space in the output fifo
      If to be queued and space continue state machine
      As we loop through the transfers we update read tr cnt
      If output is done update the state of the output sequence. */
   uNumFreeDesc = I2CPLATDEV_BamGetPipeFreeSpace(pDev->hDevTgt, I2CPLATBAM_DIR_INPUT);
   while( (I2CDEVQUP_INPUT_PUSH_DESCRIPTORS == pInSeqInfo->eInSeqState) &&
          (uNumFreeDesc > 0) ) {
      QUP_GetNextInSeqDescriptor(pInSeqInfo, &bamIoVec);
           
      res = I2CPLATDEV_PlatBamTransfer(pDev->hDevTgt,
                                      &bamIoVec,
                                      &uNumFreeDesc,
                                      QUP_BamCallback,
                                      (void *)pDev);
      if ( I2C_RES_SUCCESS != res ) {
         pSeqInfo->ioRes.nOperationResult = res;     
         break;     
      }
   }
   if ( I2C_RES_SUCCESS != res ) {
      pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_ERROR;     
      I2CDEV_AbortTransfer(pDev);     
   }
}


/** @brief This function Performs qup servicing.
 
    This function is the main entry point for qup servicing.
    It is called when events happen.
      
    @param[in, out] pDev   Pointer to qup device structure.
    @return         Nothing.
  */
static void
QUP_ServiceQUP
(
   I2CDEVQUP_Device *pDev
)
{
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   I2CDEVQUP_QupIsrHwEvt          *pIsrEvt;
   uint32                          uQupErrors;
   uint32                          uOperational;
   uint32                          uOperationaSvcFlags;
   HAL_qupe_I2cStatus              i2cStatus;
      
   pQupInfo = &pDev->qupInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   pIsrEvt  = pQupInfo->hwEvtQueue.pHead;


   I2C_CALLTRACE_LEVEL2(pDev->pDevLog, 2,
                        "DEV:0x%x: QUP_ServiceQUP pDev=0x%x",
                         pDev->uPlatDevId, (uint32) pDev);

   if( I2CDEV_POWER_STATE_2 != pDev->devPowerState ) {
      I2C_CALLDATA_LEVEL2(pDev->pDevLog, 2,
                        "DEV:0x%x: QUP_ServiceQUP:Exit not servicable devPowerState=0x%x",
                         pDev->uPlatDevId, pDev->devPowerState);     
      return;     
   }
   /* Check operating and error status.                                      */
   /* According to the HW designer both the QUP error flags and the minicore
      status register need to be read and then the interrupts can be cleared.
      If status registers are not read then the interrupt will remain set.
      I have seen this happen for the interrupts asserted by the minicore.   */
   uQupErrors   = HAL_qupe_GetErrStatusFlags(pDev->devProps.virtBlockAddr);
   uOperational = HAL_qupe_GetOpStatusFlags(pDev->devProps.virtBlockAddr);
   HAL_qupe_GetI2cState(pDev->devProps.virtBlockAddr, &i2cStatus);

   uOperationaSvcFlags =
       (uOperational & 
       (HAL_QUPE_OPSTATE_OUTPUT_SERVICE | HAL_QUPE_OPSTATE_INPUT_SERVICE));

   /* Check if service events are set return if not. */
   if ( (!(uQupErrors & HAL_QUPE_ERRSTATE_FIELD)) &&
        (!(i2cStatus.uRawStatus & HAL_QUPE_I2C_OP_STATUS_ERROR_FIELD)) &&
        (!uOperationaSvcFlags)
      )
   {
      I2C_CALLDATA_LEVEL2(pDev->pDevLog, 4,
                        "DEV:0x%x: QUP_ServiceQUP:Exit not servicable uQupErrors=0x%x uOperational=0x%x"
                        " i2cStatus.uRawStatus=0x%x",
                         pDev->uPlatDevId, uQupErrors, uOperational,
                         i2cStatus.uRawStatus);     
      return;     
   }

   pQupInfo->uIrqCnt++; /* Count interrupts */
   pIsrEvt->uIrqCnt = pQupInfo->uIrqCnt;
   pIsrEvt->uQupErrors   = uQupErrors;
   pIsrEvt->uOperational = uOperational;
   pIsrEvt->i2cStatus    = i2cStatus;
   pIsrEvt->uOutTranCnt  = 
           HAL_qupe_GetOutTransferCountCurrentExt(
                           pDev->devProps.virtBlockAddr,
                           pQupInfo->eOutFifoMode,
                           pOutSeqInfo->uTotalOutBytes);
   /* do not read the current input count if no input is programmed. */
   if (pInSeqInfo->uTotalInBytes) {
      pIsrEvt->uInTranCnt   = 
           HAL_qupe_GetInTransferCountCurrentExt(
                           pDev->devProps.virtBlockAddr,
                           pQupInfo->eInFifoMode,
                           pInSeqInfo->uTotalInBytes);
   }
   else {
      pIsrEvt->uInTranCnt   = 0;     
   }

   if ( !I2CDEVQUP_QupHwEvtQueue_IsFull(&pQupInfo->hwEvtQueue) ) {
      I2CDEVQUP_QupHwEvtQueue_HeadNext(&pQupInfo->hwEvtQueue);     
   }

   I2C_CALLDATA_LEVEL2(pDev->pDevLog, 4,
                        "DEV:0x%x: QUP_ServiceQUP:Entry uQupErrors=0x%x uOperational=0x%x"
                        " i2cStatus.uRawStatus=0x%x",
                         pDev->uPlatDevId, uQupErrors, uOperational,
                         i2cStatus.uRawStatus);

   /* Check for errors.                                                      */
   if ( uQupErrors & HAL_QUPE_ERRSTATE_FIELD ){
      /* TODO: HAL_qupe_LogRegs(pDev->devProps.virtBlockAddr); */
      HAL_qupe_ClearErrStatusFlags(pDev->devProps.virtBlockAddr, uQupErrors);
   }

   if( i2cStatus.uRawStatus & HAL_QUPE_I2C_OP_STATUS_ERROR_FIELD ) {
      /* Should reset the core to protect against re-entering the ISR due to
         the same error condition being set.                              */
      /* TODO: HAL_qupe_LogRegs(pDev->devProps.virtBlockAddr); */
      
      HAL_qupe_ClearI2cErrors(
            pDev->devProps.virtBlockAddr,
            (i2cStatus.uRawStatus & HAL_QUPE_I2C_OP_STATUS_ERROR_FIELD));
   }
   if ( uOperationaSvcFlags ) {
      HAL_qupe_ClearOpStatusFlags(
          pDev->devProps.virtBlockAddr, 
          uOperationaSvcFlags);
   }

   I2C_CALLDATA_LEVEL2(pDev->pDevLog, 5,
                        "DEV:0x%x: QUP_ServiceQUP:Exit uQupErrors=0x%x uOperational=0x%x",
                         pDev->uPlatDevId,
                         HAL_qupe_GetErrStatusFlags(pDev->devProps.virtBlockAddr),
                         HAL_qupe_GetOpStatusFlags(pDev->devProps.virtBlockAddr) 
                      );
}

/** @brief This is a hook for the ISR servicing the qup.

    This Function is called by the framework to handle the irq
    events triggered by the core. This function does not do
    data transfer. The IST thread handles data transfer.

    @param[in] pdata    void pointer to data.

    @return             Nothing.
  */
static void
I2CDEVQUP_ISR_HOOK
(
   void *pdata
)
{
   if ( NULL == pdata ) {
      return;
   }
   QUP_ServiceQUP((I2CDEVQUP_Device*)pdata);
}

/** @brief Configures the device.

    This function configures the device and registers the
    IST hook.

    @param[in] uPlatDevId    Platform Device ID.
    @param[in] phDev         Pointer to device handle.

    @return          int32 .
  */
static int32
I2CDEVQUP_SetupHw
(
   I2CDEVQUP_Device *pDev
)
{
   int32 res;

   HAL_qupe_Init(pDev->devProps.virtBlockAddr,
                 &pDev->pHalVer);
   
   res = I2CDEVQUP_WaitForRunStateValid(pDev->devProps.virtBlockAddr);
   if ( I2C_RES_SUCCESS != res ) {
      return res;     
   }
   
   HAL_qupe_GetHwInfo(pDev->devProps.virtBlockAddr, &pDev->devHwInfo);
   HAL_qupe_Configure(pDev->devProps.virtBlockAddr);

   if ( pDev->devProps.bInterruptBased ) {
      res = I2CSYS_RegisterISTHooks(pDev->hDevTgt,
                                    I2CDEVQUP_ISR_HOOK,
                                    (void*) pDev,
                                    I2CDEVQUP_IST_HOOK,
                                    (void*) pDev);
      if ( I2C_RES_SUCCESS != res ) {
         return (int32)res;
      }
      pDev->devInitState |= I2CDEVQUP_InitState_ISR_IST_Registration_Done;
   }
   pDev->devInitState |= I2CDEVQUP_InitState_HardwareInit_Done;

   return I2C_RES_SUCCESS;
}





/**
    This function free's the bus.

    @param[in, out] pDev  Pointer to qup device structure.
    @return         TRUE if the bus is free, FALSE otherwise.
  */
static boolean QUP_FreeI2cBus
(
   I2CDEVQUP_Device               *pDev
)
{
   if ( !HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr) ) {
      return TRUE;
   }
   /* clear bus resets the core in virtio. */
   HAL_qupe_ClearI2CBus(pDev->devProps.virtBlockAddr);
   I2CSYS_BusyWait(pDev->clntCfg.uByteTransferTimeoutUs);
   HAL_qupe_Configure(pDev->devProps.virtBlockAddr);

   return (!HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr));
}



/*
static boolean QUP_PauseQup
(
   I2CDEVQUP_Device               *pDev
)
{
   if (HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr)){
      return TRUE;
   }

   HAL_qupe_ClearI2CBus(pDev->devProps.virtBlockAddr);
   I2CSYS_BusyWait(pDev->clntCfg.uByteTransferTimeoutUs);

   return (!HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr));
}
*/

/**
    This Checks if the transfer has finished.
    It checks if the transfer has completed transmiton and/or
    reception or it is interrupted due to rerrors.

    @param[in, out] pDev     Pointer to Qup device structure.
    @param[in]      pSeqInfo Pointer to sequence info.
    @return         TRUE if the transfer completed or was interrupted,
                    FALSE otherwise.
  */
static boolean
QUP_IsTransferFinished
(
   I2CDEVQUP_Device               *pDev,
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo
)
{

   if ( I2C_RES_SUCCESS != pDev->seqInfo.ioRes.nOperationResult ) {
      return TRUE;
   }
   else if ( (I2CDEVQUP_OUTPUT_TRANSMIT_DONE ==
              pSeqInfo->outSeqInfo.eOutputState) &&
             (0 == pSeqInfo->inSeqInfo.uTotalInBytes) ) {
      return TRUE;
   }
   else if ( (I2CDEVQUP_OUTPUT_TRANSMIT_DONE ==
              pSeqInfo->outSeqInfo.eOutputState) &&
             (I2CDEVQUP_INPUT_RECEIVE_DONE ==
              pSeqInfo->inSeqInfo.eInSeqState) ) {
      return TRUE;
   }

   return FALSE;
}

/** @brief Initializes output sequence info structures.

    Initializes output sequence info structures.

    @param[out] pOutSeqInfo   Pointer to out sequence struct.

    @return                   Nothing.
  */
void
I2CDEVQUP_InitOutSeqInfo
(
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo
)
{
   pOutSeqInfo->pTransfer    = NULL;
   pOutSeqInfo->uOutTrCnt    = 0;
   pOutSeqInfo->uTrIndex     = 0;
   pOutSeqInfo->eOutputState = I2CDEVQUP_OUTPUT_FILL_FIFO;
   pOutSeqInfo->uBamBuffIndex = 0;
   pOutSeqInfo->uCurOutBytes  = 0;
   pOutSeqInfo->uNumOutDtBytes = 0;
   pOutSeqInfo->uTotalOutBytes = 0;
}

/** @brief Initializes input sequence info structures.

    Initializes input sequence info structures.

    @param[out] pInSeqInfo    Pointer to input info sequence.

    @return                   Nothing.
  */
void
I2CDEVQUP_InitInSeqInfo
(
   I2CDEVQUP_InSeqInfo            *pInSeqInfo
)
{
   pInSeqInfo->uNumInBytesReceived      = 0;
   pInSeqInfo->eInSeqState              = I2CDEVQUP_INPUT_IDDLE;
   pInSeqInfo->pTransfer                = NULL;
   pInSeqInfo->uInTrCnt                 = 0;
   pInSeqInfo->uTrIndex                 = 0;
   pInSeqInfo->uLastTrIndex             = 0;
   pInSeqInfo->uNumInDtBytes            = 0;
   pInSeqInfo->uTotalInBytes            = 0;
   pInSeqInfo->uInPadCnt                = 0;
}



void
I2CDEVQUP_ConfigureSequenceTransferIo
(
   I2CDEVQUP_Device                    *pDev,
   I2cSequence                         *pSeq
)
{
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   uint32 uTranIndex;
   I2cTransfer                    *pTransfer;
   uint32                          uNumInBytes, uNumOutBytes;
   uint32                          uPadCnt;
   

   pSeqInfo = &pDev->seqInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;

   pSeqInfo->pTransfer              = pSeq->pTransfer;
   pSeqInfo->uNumTransfers          = pSeq->uNumTransfers;
   pSeqInfo->ioRes.uInByteCnt       = pSeqInfo->ioRes.uOutByteCnt = 0;
   pSeqInfo->ioRes.nOperationResult = I2C_RES_SUCCESS;
   if (pDev->clntCfg.uBusFreqKhz > I2C_FAST_STANDARD_FREQ_THRESHOLD_KHZ) {
      pSeqInfo->bHsTransfer         = TRUE; /* High speed transfer */     
   }
   else {
      pSeqInfo->bHsTransfer         = FALSE; /* High speed transfer */     
   }
   pSeqInfo->bBamTransfer           = FALSE;
   
   pSeqInfo->eSeqState              = I2CDEVQUP_SEQSTATE_INPUT_OUTPUT;

   I2CDEVQUP_InitOutSeqInfo(pOutSeqInfo);
   pOutSeqInfo->pTransfer       = pSeqInfo->pTransfer;
   pOutSeqInfo->uOutTrCnt       = pSeqInfo->uNumTransfers;
   QUP_OutSeqInitTransfer(pOutSeqInfo);

   I2CDEVQUP_InitInSeqInfo(pInSeqInfo);
  
   pTransfer = pOutSeqInfo->pTransfer; /* get first transfer. */
   
   for (uTranIndex = 0; uTranIndex < pSeqInfo->uNumTransfers; uTranIndex++) {
      pTransfer = &pSeq->pTransfer[uTranIndex]; /* get transfer */
      
      if ( I2cTranDirIn == pTransfer->eTranDirection ) {
         pInSeqInfo->uInTrCnt++;
         if ( 1 == pInSeqInfo->uInTrCnt ) { /* first read of the sequence. */
            pInSeqInfo->pTransfer   = pTransfer;  /* first read transfer. */
            pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_RECEIVE_FROM_INPUT_FIFO;
            pInSeqInfo->uTrIndex    = uTranIndex;
            QUP_InSeqInitTransfer(pInSeqInfo);
         }
         /* If single byte input transfer then pad so that it starts at
          * slot boundary */
         if ( (1 == pTransfer->uTrSize) && (pOutSeqInfo->uTotalOutBytes & 0x3)
            ) {
            /* Pad to slot boundary. */
            uPadCnt = ((4 - (pOutSeqInfo->uTotalOutBytes & 0x3)) & 0x3);     
            pOutSeqInfo->uTotalOutBytes += uPadCnt;
            //pInSeqInfo->uTotalInBytes   += uPadCnt;     
         }
         pInSeqInfo->uLastTrIndex = uTranIndex;
         pInSeqInfo->uNumInDtBytes += pTransfer->uTrSize;
      }
      else {
         pOutSeqInfo->uNumOutDtBytes += pTransfer->uTrSize;
      }
      I2CDEVQUP_CalcTransferInOutSize(pTransfer,&uNumInBytes, &uNumOutBytes);
      if ( I2CPLATDEV_IsBamEnabled(pDev->hDevTgt) ) {
         if ( (uNumInBytes  >= pDev->devProps.uBamThreshold) ||
              (uNumOutBytes >= pDev->devProps.uBamThreshold) ) {
            pSeqInfo->bBamTransfer = TRUE;
         }
      }
      /* Start followed by master code needs to be inserted
       * in case of a high speed transfer. */
      if (I2CDEVQUP_IS_FIRST_HS_TRANSFER(pSeqInfo, pTransfer) ) {
         pOutSeqInfo->uTotalOutBytes += 2;     
      } 
      pInSeqInfo->uTotalInBytes   += uNumInBytes;
      pOutSeqInfo->uTotalOutBytes += uNumOutBytes;
   }
   /* adjust for EOT TAG and FLUSH_TAG to mark end of input. */
   if ( pSeqInfo->bBamTransfer ) {
      pOutSeqInfo->uTotalOutBytes++; /* Account for FLUSH_STOP */

      /* Padding to handle QCTDD00347454.
         If transfer_length %4 = 3 pad with additional
         FLUSH_STOP.
       */
      // FLUSH_STOP seems to cause problems so will disable for 9625
      //if ( 3 == (pOutSeqInfo->uTotalOutBytes & 0x3) ) {
      //   pOutSeqInfo->uTotalOutBytes++;      
      //}

      if ( pInSeqInfo->uTotalInBytes ) {
         pInSeqInfo->eInSeqState = I2CDEVQUP_INPUT_PUSH_DESCRIPTORS;     
         pOutSeqInfo->uTotalOutBytes++; /* Account for EOT */
         pInSeqInfo->uTotalInBytes++;   /* Account for EOT */
      }
   }

   pSeqInfo->uSeqTimeoutUs =
      (pInSeqInfo->uTotalInBytes + pOutSeqInfo->uTotalOutBytes) *
      pDev->clntCfg.uByteTransferTimeoutUs;
}





/** @brief returns the runstate. It will wait untill the state is valid
           before returning or timeout.

    @param[in] pQupAddr       QUP block address.
    @param[in] pRunState      Pointer to returned run state.
    @return           Returns timeout error if the state is invalid,
                      success otherwise.
  */
/*
 * If useful later
static int32
I2CDEVQUP_GetRunState(uint8 *pQupAddr, HAL_qupe_RunStateType *pRunState)
{
   uint32 uWaitTimeUs = QUP_RUN_STATE_CHANGE_TIME_OUT_US;

   while((!HAL_qupe_RunStateIsValid(pQupAddr)) && (0 != uWaitTimeUs)) {
      I2CSYS_BusyWait(QUP_BUSY_WAIT_STEP_US);
      uWaitTimeUs -= QUP_BUSY_WAIT_STEP_US;
   }
   if(0 == uWaitTimeUs){
      return I2CDEVQUP_ERR_INVALID_RUNSTATE;
   }

   *pRunState = HAL_qupe_GetRunState(pQupAddr);

   return I2C_RES_SUCCESS;
}
*/

/** @brief Waits for completion.
    @param[in] pDev         Pointer to qup device structure.
    @return                 Nothing.
  */
static void
QUP_WaitForCompletion
(
   I2CDEVQUP_Device                    *pDev
)
{
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   uint32                          uTmpTimeout;
   boolean                         bTimeout      = FALSE;
   int32                           osRes;

   I2C_CALLTRACE_LEVEL2(pDev->pDevLog, 2,
                        "DEV:0x%x: QUP_WaitForCompletion pDev=0x%x",
                         pDev->uPlatDevId, (uint32) pDev);

   pSeqInfo = &pDev->seqInfo;
   if (pDev->devProps.bInterruptBased) /* Interrupt based wait.          */
   {
      osRes = I2CSYS_Wait(pDev->hQupEvt, pSeqInfo->uSeqTimeoutUs/1000);
      if (I2CSYS_RESULT_ERROR_EVT_WAIT_TIMEOUT == osRes) {
         bTimeout = TRUE;
      }
      else if (I2C_RES_SUCCESS != (I2cResult)osRes) {
         pDev->seqInfo.ioRes.nOperationResult = osRes;
         I2CDEV_AbortTransfer(pDev);
      }
   }
   else {
      /* Polled wait.                                                */
      uTmpTimeout = pSeqInfo->uSeqTimeoutUs;
      /* loop until either an event happens or timeout                 */
      while( (!QUP_IsTransferFinished(pDev, pSeqInfo)) &&
             (!bTimeout) ) {
         QUP_ServiceQUP(pDev);
         
         QUP_PerformInputOutput(pDev);
         if ( I2C_RES_SUCCESS != pDev->seqInfo.ioRes.nOperationResult ) {
            break;
         }

         I2CSYS_BusyWait(QUP_BUSY_WAIT_STEP_US);
         if ( uTmpTimeout > QUP_BUSY_WAIT_STEP_US ) {
            uTmpTimeout -= QUP_BUSY_WAIT_STEP_US;
         }
         else {
            bTimeout    = TRUE;
         }
      }
   }

   if ( bTimeout ) { /* set error code. */
      I2CDEV_AbortTransfer(pDev);
   }
   I2C_CALLTRACE_LEVEL2(pDev->pDevLog, 2,
                        "DEV:0x%x: QUP_WaitForCompletion()=0x%x",
                         pDev->uPlatDevId, pDev->seqInfo.ioRes.nOperationResult);
}




/** @brief Configures qup hw base on the current sequence.

    Initializes transfer info structures.

    @param[in] pDev           Pointer to device struct.

    @return                   result of operation.
  */
int32
I2CDEVQUP_ConfigureQupIo
(
   I2CDEVQUP_Device                    *pDev
)
{
   int32                           res;
   I2CDEVQUP_OutSeqInfo           *pOutSeqInfo;
   I2CDEVQUP_InSeqInfo            *pInSeqInfo;
   I2CDEVQUP_QupInfo              *pQupInfo;
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2cTransfer                    *pTransfer;


   pQupInfo = &pDev->qupInfo;
   pOutSeqInfo = &pDev->seqInfo.outSeqInfo;
   pInSeqInfo  = &pDev->seqInfo.inSeqInfo;
   pSeqInfo = &pDev->seqInfo;
   pTransfer = pSeqInfo->pTransfer;

   /* Set Reset run state.                                                   */
   res = I2CDEVQUP_SetRunStateIfValid(pDev->devProps.virtBlockAddr,
                                      HAL_QUPE_RUNSTATE_RESET);
   if( I2C_RES_SUCCESS != res ) {
      I2CDEVQUP_ResetQupCore(pDev);
      HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
      return res;
   }
   /*
    * Reseting the target after with delay causes another issue.
    * The pipe tries to forcefeed the output fifo and causes overrun,
    * as well as crashe */
   //if (pQupInfo->bTargetReset) {
   //   res = I2CPLATDEV_ResetTarget(pDev->hDevTgt);
   //   if ( I2C_RES_SUCCESS != res ) {
   //      return res;     
   //   }
   //   pQupInfo->bTargetReset = 0;
   //}
   
   I2CDEVQUP_QupHwEvtQueue_Init(&pQupInfo->hwEvtQueue);
   pQupInfo->eBamEvt = I2CPLATBAM_EVT_NULL;
   
   pQupInfo->uIrqCnt                           = 0;

   pQupInfo->uInFreeSlotReqCnt                 = 0;
   pQupInfo->uOutFreeSlotReqCnt                = 0;
   pQupInfo->uQupErrors                        = 0;
   pQupInfo->uOperational                      = 0;
   pQupInfo->i2cStatus.uRawStatus              = 0;
   pQupInfo->i2cStatus.uRawLineState           = 0;

   pQupInfo->uInFreeSlotServicedCnt            = 0;
   pQupInfo->uOutFreeSlotServicedCnt           = 0;


   /* decide which mode to use, block, fifo or bam mode. */
   if ( pSeqInfo->bBamTransfer ) {
      pDev->pFnPerformOutput = QUP_BamPerformOutput;
      pDev->pFnPerformInput  = QUP_BamPerformInput;
      pQupInfo->eOutFifoMode = HAL_QUPE_IO_MODE_BAM;
      pQupInfo->eInFifoMode  = HAL_QUPE_IO_MODE_BAM;


      HAL_qupe_SetIoModeExt(pDev->devProps.virtBlockAddr,
                           HAL_QUPE_IO_MODE_BAM, HAL_QUPE_IO_MODE_BAM,
                           FALSE, TRUE, TRUE);
       /* by convention set to zero. */
      res = HAL_qupe_SetOutTransferCountExt(pDev->devProps.virtBlockAddr,
                                            HAL_QUPE_IO_MODE_BAM, 0);
      if (I2C_RES_SUCCESS != res) {
         return res;
      }

      res = HAL_qupe_SetInTransferCountExt(pDev->devProps.virtBlockAddr,
                                           HAL_QUPE_IO_MODE_BAM, 0);
      if (I2C_RES_SUCCESS != res) {
         return res;
      }

      
      HAL_qupe_SetIrqMask(pDev->devProps.virtBlockAddr,
                          HAL_QUPE_IRQMASK_INPUT_SERVICE_FLAG |
                          HAL_QUPE_IRQMASK_OUTPUT_SERVICE_FLAG);
   }
   else {
      pDev->pFnPerformOutput = QUP_PerformOutput;
      pDev->pFnPerformInput  = QUP_PerformInput;

      /* We use block mode for transfers that are sent separately to the core
       * and we need to end them without a stop. When we have only one transfer
       * and it is not of type NotASequence, it means that we are dealing with
       * separate transfers that require to end without a STOP. This condition
       * is checked for both INPUT and OUTPUT mode configurations */

      if ((((pOutSeqInfo->uTotalOutBytes+3)/4) > pDev->devHwInfo.uNumOutputSlots) ||
           ((pSeqInfo->uNumTransfers == 1) && (pTransfer->eTranCtxt != I2cTrCtxNotASequence))) {
         /* block mode. */
         pQupInfo->eOutFifoMode = HAL_QUPE_IO_MODE_BLOCK;
      }
      else {
         /* fifo mode. */
         pQupInfo->eOutFifoMode = HAL_QUPE_IO_MODE_FIFO;
         pQupInfo->uOutFreeSlotReqCnt  = pDev->devHwInfo.uNumOutputSlots;
         pQupInfo->uOutFreeSlotReqCnt = pDev->devHwInfo.uNumOutputSlots;
      }

      if ((((pInSeqInfo->uTotalInBytes+3)/4) > pDev->devHwInfo.uNumInputSlots) ||
           ((pSeqInfo->uNumTransfers == 1) && (pTransfer->eTranCtxt != I2cTrCtxNotASequence))) {
         pQupInfo->eInFifoMode = HAL_QUPE_IO_MODE_BLOCK;
      }
      else {
         pQupInfo->eInFifoMode = HAL_QUPE_IO_MODE_FIFO;
      }
     
      HAL_qupe_SetIoModeExt(pDev->devProps.virtBlockAddr,
                            pQupInfo->eOutFifoMode, pQupInfo->eInFifoMode,
                            FALSE, TRUE, TRUE);

      res = HAL_qupe_SetOutTransferCountExt(pDev->devProps.virtBlockAddr,
                                            pQupInfo->eOutFifoMode      ,
                                            pOutSeqInfo->uTotalOutBytes);
      if ( I2C_RES_SUCCESS != res ) {
         return res;
      }

      res = HAL_qupe_SetInTransferCountExt(pDev->devProps.virtBlockAddr,
                                           pQupInfo->eInFifoMode       ,
                                           pInSeqInfo->uTotalInBytes);
      if ( I2C_RES_SUCCESS != res ) {
         return res;
      }
     
      /* enable interrupts. */
      HAL_qupe_SetIrqMask(pDev->devProps.virtBlockAddr,0);
   }

   /* Enable qup error interrupts */
   HAL_qupe_SetErrStatusIrqEnabled(
        pDev->devProps.virtBlockAddr,
        HAL_QUPE_ERRSTATE_FIELD
   );
   res = HAL_qupe_SetI2cConfig(pDev->devProps.virtBlockAddr,
                               HAL_QUPE_I2C_TAGS_V2,
                               pDev->devProps.uI2cSrcClkKhz,
                               pDev->clntCfg.uBusFreqKhz);
   if( I2C_RES_SUCCESS != res )
   {
      I2CDEVQUP_ResetQupCore(pDev);
      HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
      I2CDEVQUP_SetRunStateIfValid(pDev->devProps.virtBlockAddr,
                                   HAL_QUPE_RUNSTATE_RESET);
      return res;
   }

   /* The state is reset as set at the beggining of the function.
    * This avoids an extra register read. */
   res = I2CDEVQUP_SetRunState(pDev->devProps.virtBlockAddr,
                               HAL_QUPE_RUNSTATE_RUN);
   if( I2C_RES_SUCCESS != res )
   {
      pSeqInfo->eSeqState = I2CDEVQUP_SEQSTATE_QUP_STATE_ERROR;
      I2CDEVQUP_ResetQupCore(pDev);
      HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
      I2CDEVQUP_SetRunStateIfValid(pDev->devProps.virtBlockAddr,
                                   HAL_QUPE_RUNSTATE_RESET);
      return res;
   }

   return I2C_RES_SUCCESS;
}

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Read a buffer from i2c device.

    Read a buffer from i2c device.

    @param[in] hDev            Device handle.
    @param[in] pTransfer       Pointer where transfer data will
          be stored.
    @param[in] pClntCfg        Pointer to Client configuration.
                               If null keep previous.
    @param[in] puNumCompleted  Pointer to return completed
          reads.

    @return          int32 .
  */
int32
I2CDEV_Read
(
   I2CDEV_HANDLE                         hDev      ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg ,
   uint32                               *puNumCompleted
)
{
   I2CDEVQUP_Device                      *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                  res = I2C_RES_SUCCESS;
   uint32                                 uWhileOnce = 0;
   I2cSequence                            seq;


   res = (int32)IQUPI2CDEV_VALIDATE_SYNC_TRAN(hDev,pTransfer,pClntCfg,puNumCompleted);
   if ( I2C_RES_SUCCESS != res ) {
      return res;
   }
 
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 5,
                        "DEV:0x%x: I2CDEV_Read hdev=0x%x pTransfer=0x%x pClntCfg=0x%x puNumCompleted=0x%x",
                        pDev->uPlatDevId, (uint32) hDev, (uint32) pTransfer, (uint32) pClntCfg, (uint32) puNumCompleted);
   I2CLOG_LogTransferData(pDev->pDevLog, pTransfer);
   do {
      if ( pTransfer->eTranDirection != I2cTranDirIn ) {
         res = I2CDEVQUP_ERR_INVALID_TRANSFER_DIRECTION;
         break;
      }

      if( I2CDEV_POWER_STATE_2 != pDev->devPowerState ) {
        res = I2CDEVQUP_ERR_INVALID_POWER_STATE;
        break;
      }
      if ( 0 == (pDev->devInitState & I2CDEVQUP_InitState_HardwareInit_Done) ) {
         res = I2CDEVQUP_SetupHw(pDev);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }

      if ( pDev->devProps.bInterruptBased ) {
         res  = I2CSYS_ClearEvent( pDev->hQupEvt);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }

      if ( !QUP_FreeI2cBus(pDev) ) {
         res = I2CDEVQUP_ERR_BUS_BUSY;
         break;
      }

      pDev->asyncCall.pCallbackFn = NULL;
      pDev->asyncCall.pArg        = NULL;
      /* check if client configuration changed. */
      if ( NULL != pClntCfg ) {
         pDev->clntCfg = *pClntCfg;
      }
      I2CLOG_LogCltCfgData(pDev->pDevLog, &pDev->clntCfg);

      seq.pTransfer = pTransfer;
      seq.uNumTransfers = 1;
      I2CDEVQUP_ConfigureSequenceTransferIo(pDev, &seq);
      res = I2CDEVQUP_ConfigureQupIo(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      QUP_PerformInputOutput(pDev);
      QUP_WaitForCompletion(pDev);
      *puNumCompleted = pDev->seqInfo.ioRes.uInByteCnt;
   } while(uWhileOnce);

   if ( I2C_RES_SUCCESS != res ) {
     pDev->seqInfo.ioRes.nOperationResult = res;
     I2C_ERROR_LOG_LEVEL0(pDev->pDevLog,2,"[DEV:0x%x] I2CDEV_Read() Error =%d",
                          pDev->uPlatDevId,pDev->seqInfo.ioRes.nOperationResult);
   }

   I2C_CALLTRACE_LEVEL1(pDev->pDevLog,2,"[DEV:0x%x] I2CDEV_Read()=%d",
                        pDev->uPlatDevId,pDev->seqInfo.ioRes.nOperationResult);

   return pDev->seqInfo.ioRes.nOperationResult;
}


/** @brief Initializes the device.

    This Function Initializes the device and creates the
    necessary data structures to support other calls into the
    device.

    @param[in] uPlatDevId    Platform Device ID.
    @param[inout] phDev         Pointer to device handle.

    @return          int32 .
  */
int32
I2CDEV_Init
(
   uint32                               uPlatDevId,
   I2CDEV_HANDLE                       *phDev
)
{
   int32                               res = I2C_RES_SUCCESS;
   I2CDEVQUP_Device                    *pDev = NULL;
   uint32                               uWhileOnce = 0;


   pDev = I2CDEVQUP_SearchDevice(uPlatDevId);
   if ( NULL != pDev ) {
      *phDev = pDev;
      return I2C_RES_SUCCESS;
   }

   res = (int32)I2CSYS_Malloc((void **)&pDev, sizeof(I2CDEVQUP_Device));
   if ( I2C_RES_SUCCESS != res )  {
      return res;
   }
   res = I2CSYS_Memset(pDev, 0, sizeof(I2CDEVQUP_Device));
   if ( I2C_RES_SUCCESS != res )  {
      I2CSYS_Free(pDev);     
      return res;
   }

   pDev->uPlatDevId = uPlatDevId;
   pDev->devInitState = I2CDEVQUP_InitState_Uninitialized;

   do {
      res = (int32)I2CPLATDEV_InitTarget(uPlatDevId, &pDev->hDevTgt);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      pDev->devInitState |= I2CDEVQUP_InitState_TargetInit_Done;

      res = (int32)I2CPLATDEV_ReadProperties(pDev->hDevTgt, &pDev->devProps);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      pDev->seqInfo.outSeqInfo.pSeq      = &pDev->seqInfo;
      pDev->seqInfo.inSeqInfo.pSeq       = &pDev->seqInfo;

      if ( pDev->devProps.bBamAvailable ) {
         /* allocate bam buffer. */
         pDev->seqInfo.physMem.uSize = 2*BAM_TMPBUFF_MAX;// input+output
         res = I2CSYS_PhysMemAlloc(&pDev->seqInfo.physMem);
         if (I2C_RES_SUCCESS != res) {
            break;
         }
         pDev->devInitState |= I2CDEVQUP_InitState_PhysMemAlloc_Done;
         /* use top half for output and bottom half for input. */
         pDev->seqInfo.outSeqInfo.aOutBamBuff =
                     pDev->seqInfo.physMem.pVirtAddress;
         pDev->seqInfo.inSeqInfo.aInBamBuff = 
              &pDev->seqInfo.outSeqInfo.aOutBamBuff[BAM_TMPBUFF_MAX];
      }

      if ( pDev->devProps.bInterruptBased ) {
         res = (int32)I2CSYS_CreateEvent(&pDev->hQupEvt);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
         pDev->devInitState |= I2CDEVQUP_InitState_Events_Done;
      }

      res = I2CSYS_CreateCriticalSection(&pDev->hOperationSync);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      pDev->devInitState |= I2CDEVQUP_InitState_CritSectionCreate_Done;

      pDev->clntCfg.uBusFreqKhz = I2CDEVQUP_DEFAULT_BUS_FREQ_KHZ;
      pDev->clntCfg.uByteTransferTimeoutUs =
         I2CDEVQUP_DEFAULT_BYTE_TRANSFER_TIMEOUT_US;
      pDev->devInitState |= I2CDEVQUP_InitState_DeviceInit_Done;
      pDev->devPowerState = I2CDEV_POWER_STATE_0;
      /* Init device logging. */
      I2cLog_Init(pDev->uPlatDevId, &pDev->pDevLog);
      I2cLog_AssociateBlockAddress(pDev->pDevLog, (uint32)pDev->devProps.virtBlockAddr);
      
      I2CDEVQUP_LinkDevice(pDev);

      *phDev = pDev;
   } while ( uWhileOnce );

   if ( I2C_RES_SUCCESS != res ) { /* in case of error undo initialization. */
      I2CDEV_DeInit(pDev);
   }

   return res;
}

/** @brief Deinitializes the device.

    This Function Deinitializes the device and releases
    resources acquired during init.

    @param[in] hDev      Device handle.

    @return          int32 .
  */
int32
I2CDEV_DeInit
(
   I2CDEV_HANDLE                        hDev
)
{
   int32 res = I2C_RES_SUCCESS;
   int32 opRes;
   I2CDEVQUP_Device *pDev = (I2CDEVQUP_Device *)hDev;


   if ( NULL == pDev ) {
      return I2C_RES_SUCCESS;
   }

   if ( pDev->devInitState & I2CDEVQUP_InitState_DeviceInit_Done ) {
      I2CDEVQUP_UnlinkDevice(pDev);     
   }

   if ( pDev->devInitState & I2CDEVQUP_InitState_CritSectionCreate_Done ) {
      opRes = I2CSYS_CloseHandle(pDev->hOperationSync);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }

   if ( pDev->devInitState & I2CDEVQUP_InitState_Events_Done ) {
      opRes = I2CSYS_CloseHandle(pDev->hQupEvt);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }
   
   if ( pDev->devInitState & I2CDEVQUP_InitState_PhysMemAlloc_Done ) {
      opRes = I2CSYS_PhysMemFree(&pDev->seqInfo.physMem);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }

   if ( pDev->devInitState & I2CDEVQUP_InitState_ISR_IST_Registration_Done ) {
      opRes = I2CSYS_UnRegisterISTHooks(pDev->hDevTgt);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }

   if ( pDev->devInitState & I2CDEVQUP_InitState_TargetInit_Done ) {
      opRes = I2CPLATDEV_DeInitTarget(pDev->hDevTgt);
      if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
      }     
   }
   
   I2cLog_DeInit(pDev->pDevLog);
   opRes = (int32)I2CSYS_Free(pDev);
   if ( (I2C_RES_SUCCESS != opRes ) && ( I2C_RES_SUCCESS == res ) ) {
         res = opRes;
   }

   return res;
}

/** @brief Write a buffer to i2c device.

    Write a buffer to i2c device.

    @param[in] hDev            Device handle.
    @param[in] pTransfer       Pointer to transfer data.
    @param[in] pClntCfg        Pointer to Client configuration.
                               If null keep previous.
    @param[in] puNumCompleted  Pointer to return completed
          bytes.

    @return          int32 .
  */
int32
I2CDEV_Write
(
   I2CDEV_HANDLE                         hDev      ,
   I2cTransfer                          *pTransfer,
   I2cClientConfig                      *pClntCfg ,// if null keep previous
   uint32                               *puNumCompleted
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res = I2C_RES_SUCCESS;
   uint32                               uWhileOnce = 0;
   I2cSequence                          seq;


   res = IQUPI2CDEV_VALIDATE_SYNC_TRAN(hDev,pTransfer,pClntCfg,puNumCompleted);
   if (I2C_RES_SUCCESS != (I2cResult)res) {
      return res;
   }
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog,5,"[DEV:0x%x] I2CDEV_Write(hdev=0x%x, pTransfer=0x%x,"
                        "pClntCfg=0x%x, puNumCompleted=0x%x)",
                         pDev->uPlatDevId, (uint32) hDev, (uint32) pTransfer, (uint32) pClntCfg, (uint32) puNumCompleted);
   I2CLOG_LogTransferData(pDev->pDevLog, pTransfer);

   do {
      if ( pTransfer->eTranDirection != I2cTranDirOut ) {
         res = I2CDEVQUP_ERR_INVALID_TRANSFER_DIRECTION;
         break;
      }

      if( I2CDEV_POWER_STATE_2 != pDev->devPowerState ) {
        res = I2CDEVQUP_ERR_INVALID_POWER_STATE;
        break;
      }

      if ( 0 == (pDev->devInitState & I2CDEVQUP_InitState_HardwareInit_Done) ) {
         res = I2CDEVQUP_SetupHw(pDev);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }
      
      if ( pDev->devProps.bInterruptBased ) {
         res  = I2CSYS_ClearEvent( pDev->hQupEvt);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }

      if ( !QUP_FreeI2cBus(pDev) ) {
         res = I2CDEVQUP_ERR_BUS_BUSY;
         break;
      }

      pDev->asyncCall.pCallbackFn = NULL;
      pDev->asyncCall.pArg        = NULL;
      /* check if client configuration changed. */
      if ( NULL != pClntCfg ) {
         pDev->clntCfg = *pClntCfg;
      }
      I2CLOG_LogCltCfgData(pDev->pDevLog,&pDev->clntCfg);

      seq.pTransfer = pTransfer;
      seq.uNumTransfers = 1;
      I2CDEVQUP_ConfigureSequenceTransferIo(pDev, &seq);
      res = I2CDEVQUP_ConfigureQupIo(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      QUP_PerformInputOutput(pDev);
      QUP_WaitForCompletion(pDev);
      *puNumCompleted = pDev->seqInfo.ioRes.uOutByteCnt;
   } while ( uWhileOnce );

   if ( I2C_RES_SUCCESS != res ) {
     pDev->seqInfo.ioRes.nOperationResult = res;
     I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_Write() Error =%d",
                          pDev->uPlatDevId,pDev->seqInfo.ioRes.nOperationResult);
   }

   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_Write()=%d",
                        pDev->uPlatDevId, pDev->seqInfo.ioRes.nOperationResult);

   return pDev->seqInfo.ioRes.nOperationResult;
}

/** @brief Does a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to sequence.
    @param[in] pClntCfg       Pointer to Client configuration.
                              If null keep previous.
    @param[out] pIoRes        Pointer to result.
    
    @return          int32 .
  */
int32
I2CDEV_BatchTransfer
(
   I2CDEV_HANDLE                         hDev     ,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg ,// if null keep previous
   I2cIoResult                          *pIoRes
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int                                  res = I2C_RES_SUCCESS;
   uint32                               uWhileOnce = 0;

   res = IQUPI2CDEV_VALIDATE_SYNC_SEQ(hDev, pSequence, pClntCfg, pIoRes);
   if ( I2C_RES_SUCCESS != (I2cResult)res ) {
      return res;
   }
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 5, "[DEV:0x%x] I2CDEV_BatchTransfer(hdev=0x%x, pSequence=0x%x,"
                        "pClntCfg=0x%x, pIoRes=0x%x)",
                        pDev->uPlatDevId, (uint32) hDev, (uint32) pSequence, (uint32) pClntCfg, (uint32) pIoRes);
   I2CLOG_LogSeqData(pDev->pDevLog, pSequence);
   do {
      if ( I2CDEV_POWER_STATE_2 != pDev->devPowerState ) {
        res =  I2CDEVQUP_ERR_INVALID_POWER_STATE ;
        break;
      }
      if ( 0 == (pDev->devInitState & I2CDEVQUP_InitState_HardwareInit_Done) ) {
         res = I2CDEVQUP_SetupHw(pDev);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }

      if ( pDev->devProps.bInterruptBased ) {
         res  = I2CSYS_ClearEvent( pDev->hQupEvt);
         if (I2C_RES_SUCCESS != res) {
            break;
         }
      }

      if ( !QUP_FreeI2cBus(pDev) ) {
         res = I2CDEVQUP_ERR_BUS_BUSY;
         break;
      }

      pDev->asyncCall.pCallbackFn = NULL;
      pDev->asyncCall.pArg        = NULL;
      /* check if client configuration changed. */
      if ( NULL != pClntCfg ) {
         pDev->clntCfg = *pClntCfg;
      }
      I2CLOG_LogCltCfgData(pDev->pDevLog, &pDev->clntCfg);

      I2CDEVQUP_ConfigureSequenceTransferIo(pDev, pSequence);
      res = I2CDEVQUP_ConfigureQupIo(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      QUP_PerformInputOutput(pDev);
      QUP_WaitForCompletion(pDev);
      *pIoRes = pDev->seqInfo.ioRes;
   } while ( uWhileOnce );

   if ( I2C_RES_SUCCESS != res ) {
     pDev->seqInfo.ioRes.nOperationResult = res;
     I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_BatchTransfer() Error =%d",
                          pDev->uPlatDevId, pDev->seqInfo.ioRes.nOperationResult);
   }

   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_BatchTransfer()=%d",
                        pDev->uPlatDevId, pDev->seqInfo.ioRes.nOperationResult);

   return pDev->seqInfo.ioRes.nOperationResult;
}



/** @brief Schedules a async buffer read from i2c device.

    Schedules an async buffer read from i2c device.
    Once the read is complete or an error occurs
    the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer data.
    @param[in] pClntCfg       Pointer to Client configuration.
                               If null keep previous.
    @param[in] pIoRes         Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   int32 .
  */
int32
I2CDEV_AsyncRead
(
   I2CDEV_HANDLE                         hDev               ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg           ,
   I2cIoResult                          *pIoRes           ,
   DEV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res = I2C_RES_SUCCESS;
   uint32                               uWhileOnce = 0;
   I2cSequence                          seq;

   res = IQUPI2CDEV_VALIDATE_ASYNC_TRAN(hDev, pTransfer, pClntCfg,
                                        pIoRes, pCallbackFn, pArg);
   if ( I2C_RES_SUCCESS != (I2cResult)res ) {
      return res;
   }
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 7, "[DEV:0x%x] I2CDEV_AsyncRead(hdev=0x%x, pTransfer=0x%x,"
                        "pClntCfg=0x%x, pIoRes=0x%x, pCallbackFn=0x%x,pArg=0x%x)",
                        pDev->uPlatDevId, (uint32) hDev, (uint32) pTransfer, (uint32) pClntCfg, (uint32) pIoRes, (uint32) pCallbackFn, (uint32) pArg);
   I2CLOG_LogTransferData(pDev->pDevLog, pTransfer);

   do {
      if ( pTransfer->eTranDirection != I2cTranDirIn ) {
         res = I2CDEVQUP_ERR_INVALID_TRANSFER_DIRECTION;
         break;
      }

      if( I2CDEV_POWER_STATE_2 != pDev->devPowerState ) {
         res = I2CDEVQUP_ERR_INVALID_POWER_STATE  ;
         break;
      }

      if ( 0 == (pDev->devInitState & I2CDEVQUP_InitState_HardwareInit_Done) ) {
         res = I2CDEVQUP_SetupHw(pDev);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }
      pDev->asyncCall.pCallbackFn = pCallbackFn;
      pDev->asyncCall.pArg        = pArg;
      pDev->asyncCall.pIoRes      = pIoRes;
      
      if ((pTransfer->eTranCtxt == I2cTrCtxNotASequence) ||
          (pTransfer->eTranCtxt == I2cTrCtxSeqStart)) {
          /* In one case the I2C_MASTER_STATUS was 0xc112100 for a transfer that
           * began soon after the power was up POWER STATE 2. In this case the
           * BUS seems to be active, but the SDA and SCL lines are high. This is
           * unusual, so we make sure we reset the core before performing this
           * check */

          res = I2CDEVQUP_SetRunState(pDev->devProps.virtBlockAddr, HAL_QUPE_RUNSTATE_RESET);
          if( I2C_RES_SUCCESS != res ) {
              I2CDEVQUP_ResetQupCore(pDev);
              HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
              break;
          }

          if ( HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr) ) {
              res = I2CDEVQUP_ERR_BUS_BUSY;
              break;
          }
      }
      /* check if client configuration changed. */
      if (NULL != pClntCfg) {
         pDev->clntCfg = *pClntCfg;
      }
      I2CLOG_LogCltCfgData(pDev->pDevLog, &pDev->clntCfg);

      seq.pTransfer = pTransfer;
      seq.uNumTransfers = 1;
      I2CDEVQUP_ConfigureSequenceTransferIo(pDev, &seq);
      res = I2CDEVQUP_ConfigureQupIo(pDev);
      I2CSYS_LeaveCriticalSection(pDev->hOperationSync);

      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      QUP_PerformInputOutput(pDev);
   } while (uWhileOnce);

   /* if an error occured callback. */
   if ( I2C_RES_SUCCESS != res ) {
      I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AsyncRead(), Error =%d",
                        pDev->uPlatDevId, pIoRes->nOperationResult);     
   }
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AsyncRead()=%d",
                        pDev->uPlatDevId, pIoRes->nOperationResult);
   return res;
}

/** @brief Schedules an assynchronous buffer write to i2c device.

    Schedules an asynchronous buffer write to i2c device. Once the write is
    complete or an error occurs the callback will be called.

    @param[in] hDev           Device handle.
    @param[in] pTransfer      Pointer to transfer data.
    @param[in] pClntCfg       Pointer to Client configuration.
                               If null keep previous.
    @param[in] pIoRes         Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   int32.
  */
int32
I2CDEV_AsyncWrite
(
   I2CDEV_HANDLE                         hDev               ,
   I2cTransfer                          *pTransfer          ,
   I2cClientConfig                      *pClntCfg           ,
   I2cIoResult                          *pIoRes           ,
   DEV_ASYNC_CB                          pCallbackFn        ,
   void                                 *pArg
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res = I2C_RES_SUCCESS;
   uint32                               uWhileOnce = 0;
   I2cSequence                          seq;

   res = (int32)IQUPI2CDEV_VALIDATE_ASYNC_TRAN(hDev, pTransfer, pClntCfg,
                                               pIoRes, pCallbackFn, pArg);
   if ( I2C_RES_SUCCESS != (I2cResult)res ) {
      return res;
   }
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 7, "[DEV:0x%x] I2CDEV_AsyncWrite(hdev=0x%x, pTransfer=0x%x,"
                        "pClntCfg=0x%x, pIoRes=0x%x, pCallbackFn=0x%x,pArg=0x%x)",
                         pDev->uPlatDevId, (uint32) hDev, (uint32) pTransfer, (uint32) pClntCfg, (uint32) pIoRes, (uint32) pCallbackFn, (uint32) pArg);
   I2CLOG_LogTransferData(pDev->pDevLog, pTransfer);
   do {
      if ( pTransfer->eTranDirection != I2cTranDirOut ) {
         res = I2CDEVQUP_ERR_INVALID_TRANSFER_DIRECTION;
         break;
      }

      if( I2CDEV_POWER_STATE_2 != pDev->devPowerState ) {
         res = I2CDEVQUP_ERR_INVALID_POWER_STATE;
         break;
      }
      if ( 0 == (pDev->devInitState & I2CDEVQUP_InitState_HardwareInit_Done) ) {
         res = I2CDEVQUP_SetupHw(pDev);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }
      pDev->asyncCall.pCallbackFn = pCallbackFn;
      pDev->asyncCall.pArg        = pArg;
      pDev->asyncCall.pIoRes      = pIoRes;
      
      if ((pTransfer->eTranCtxt == I2cTrCtxNotASequence) ||
          (pTransfer->eTranCtxt == I2cTrCtxSeqStart)) {
          /* In one case the I2C_MASTER_STATUS was 0xc112100 for a transfer that
           * began soon after the power was up POWER STATE 2. In this case the
           * BUS seems to be active, but the SDA and SCL lines are high. This is
           * unusual, so we make sure we reset the core before performing this
           * check */

          res = I2CDEVQUP_SetRunState(pDev->devProps.virtBlockAddr, HAL_QUPE_RUNSTATE_RESET);
          if( I2C_RES_SUCCESS != res ) {
              I2CDEVQUP_ResetQupCore(pDev);
              HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
              break;
          }

          if ( HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr) ) {
              res = I2CDEVQUP_ERR_BUS_BUSY;
              break;
          }
      }
      /* check if client configuration changed. */
      if ( NULL != pClntCfg ) {
         pDev->clntCfg = *pClntCfg;
      }
      I2CLOG_LogCltCfgData(pDev->pDevLog, &pDev->clntCfg);

      seq.pTransfer = pTransfer;
      seq.uNumTransfers = 1;
      I2CDEVQUP_ConfigureSequenceTransferIo(pDev, &seq);

      /* In one case, the core interrupts as soon as we set the state to RUN
       * inside this function and there is a slave address NACK. The IST
       * immediately processes the NACK and completes the request and leaves the
       * core in RESET state. After that this function keeps checking if it was
       * successfully able to set RUN state and fails. This results in returning
       * an error to the caller and ends up in error handling in the caller
       * thread even though the request already completed. */
       
      I2CSYS_EnterCriticalSection(pDev->hOperationSync);
      res = I2CDEVQUP_ConfigureQupIo(pDev);
      I2CSYS_LeaveCriticalSection(pDev->hOperationSync);

      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      QUP_PerformInputOutput(pDev);
   } while ( uWhileOnce );

   /* if an error occured callback. */
   if ( I2C_RES_SUCCESS != res ) {
      I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2,
                           "[DEV:0x%x] I2CDEV_AsyncWrite(), Error =%d",
                           pDev->uPlatDevId, pIoRes->nOperationResult);     
   }
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AsyncWrite()=%d",
                        pDev->uPlatDevId, pIoRes->nOperationResult);
   return res;
}

/** @brief Asynchronously schedules a batch of transfers in a sequence.

    Asynchronously schedules a batch of transfers in a sequence and returns.
    The callback will be called to notify whether the transfer
    is done or has failed.

    @param[in] hDev           Device handle.
    @param[in] pSequence      Pointer to sequence.
    @param[in] pClntCfg       Pointer to Client configuration.
                               If null keep previous.
    @param[in] pIoRes         Pointer to returned result of the
                               transfer.
    @param[in] pCallbackFn    Pointer to a callback function to
                               be called when transfer finishes
                               or aboarded.
    @param[in] pArg           Pointer to be passed to the
                               callback function.

    @return                   int32.
  */
int32
I2CDEV_AsyncBatchTransfer
(
   I2CDEV_HANDLE                        hDev               ,
   I2cSequence                         *pSequence          ,
   I2cClientConfig                     *pClntCfg           ,
   I2cIoResult                         *pIoRes             ,
   DEV_ASYNC_CB                         pCallbackFn        ,
   void                                *pArg
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res = I2C_RES_SUCCESS;
   uint32                               uWhileOnce = 0;

   res = (int32)IQUPI2CDEV_VALIDATE_ASYNC_SEQ(hDev, pSequence, pClntCfg,
                                              pIoRes, pCallbackFn, pArg);
   if ( I2C_RES_SUCCESS != (I2cResult)res ) {
      return res;
   }
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 7, "[DEV:0x%x] I2CDEV_AsyncBatchTransfer(hdev=0x%x, pSequence=0x%x,"
                        "pClntCfg=0x%x, pIoRes=0x%x, pCallbackFn=0x%x,pArg=0x%x)",
                        pDev->uPlatDevId, (uint32) hDev, (uint32) pSequence, (uint32) pClntCfg, (uint32) pIoRes, (uint32) pCallbackFn, (uint32) pArg);
   I2CLOG_LogSeqData(pDev->pDevLog, pSequence);
   
   do {
      if( I2CDEV_POWER_STATE_2 != pDev->devPowerState ) {
         res = I2CDEVQUP_ERR_INVALID_POWER_STATE  ;
         break;
      }
      if ( 0 == (pDev->devInitState & I2CDEVQUP_InitState_HardwareInit_Done) ) {
         res = I2CDEVQUP_SetupHw(pDev);
         if ( I2C_RES_SUCCESS != res ) {
            break;
         }
      }
      pDev->asyncCall.pCallbackFn = pCallbackFn;
      pDev->asyncCall.pArg        = pArg;
      pDev->asyncCall.pIoRes      = pIoRes;

      /* In one case the I2C_MASTER_STATUS was 0xc112100 for a transfer that
       * began soon after the power was up POWER STATE 2. In this case the
       * BUS seems to be active, but the SDA and SCL lines are high. This is
       * unusual, so we make sure we reset the core before performing this
       * check */

      res = I2CDEVQUP_SetRunState(pDev->devProps.virtBlockAddr, HAL_QUPE_RUNSTATE_RESET);
      if( I2C_RES_SUCCESS != res ) {
          I2CDEVQUP_ResetQupCore(pDev);
          HAL_qupe_Configure(pDev->devProps.virtBlockAddr);
          break;
      }

      if ( HAL_qupe_IsI2CBusBusy(pDev->devProps.virtBlockAddr) ) {
         res = I2CDEVQUP_ERR_BUS_BUSY;
         break;
      }
      /* check if client configuration changed. */
      if ( NULL != pClntCfg ) {
         pDev->clntCfg = *pClntCfg;
      }
      I2CLOG_LogCltCfgData(pDev->pDevLog, &pDev->clntCfg);
      
      I2CDEVQUP_ConfigureSequenceTransferIo(pDev, pSequence);

      I2CSYS_EnterCriticalSection(pDev->hOperationSync);
      res = I2CDEVQUP_ConfigureQupIo(pDev);
      I2CSYS_LeaveCriticalSection(pDev->hOperationSync);

      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      QUP_PerformInputOutput(pDev);
   } while ( uWhileOnce );

   // if an error occured callback
   if ( I2C_RES_SUCCESS != res ) {
      I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2,
                           "[DEV:0x%x] I2CDEV_AsyncBatchTransfer() Error =%d",
                           pDev->uPlatDevId, pIoRes->nOperationResult);    
   }

   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AsyncBatchTransfer()=%d",
                        pDev->uPlatDevId, pIoRes->nOperationResult);
   return res;
}

/** @brief Cancels ongoing transfer.

    Cancels ongoing transfer by reseting the core.

    @param[in] hDev           Device handle.

    @return                   int32.
  */
int32
I2CDEV_CancelTransfer
(
   I2CDEV_HANDLE                        hDev
)
{
   I2CDEVQUP_Device *pDev = (I2CDEVQUP_Device *)hDev;
   I2CDEVQUP_AsyncCallData asyncCall;

   if( NULL == pDev) {
      return I2CDEVQUP_ERR_INVALID_DEV_HANDLE;	   
   }

   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_CancelTransfer(hdev=0x%x)", pDev->uPlatDevId, (uint32) hDev);

   I2CDEV_AbortTransfer(pDev);

   I2CSYS_EnterCriticalSection(pDev->hOperationSync);

   asyncCall = pDev->asyncCall;
   *asyncCall.pIoRes = pDev->seqInfo.ioRes;
   
   I2CSYS_LeaveCriticalSection(pDev->hOperationSync);

   /* Async callback should be called outside locks */
   asyncCall.pCallbackFn(asyncCall.pArg); 

   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_CancelTransfer()=", pDev->uPlatDevId);

   return I2C_RES_SUCCESS;
}

/** @brief Cancels ongoing transfer.

    Cancels ongoing transfer by reseting the core.

    @param[in] pDev           Device structure.

    @return                   int32.
  */
static int32
I2CDEV_AbortTransfer
(
   I2CDEVQUP_Device *pDev
)
{
   I2CDEVQUP_SequenceIOInfo       *pSeqInfo;
   I2cTransfer                    *pTransfer;
   HAL_qupe_RunStateType           eRunState;
   boolean                         bSkipBusClear = FALSE; 

   pSeqInfo = &pDev->seqInfo;
   pTransfer = pSeqInfo->pTransfer;

   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer(pDev=0x%x)",
                        pDev->uPlatDevId, (uint32) pDev);
   I2CSYS_EnterCriticalSection(pDev->hOperationSync);

   eRunState = HAL_qupe_GetRunState(pDev->devProps.virtBlockAddr);

   /* No point of all this if we are already in RESET state */
   if ((eRunState != HAL_QUPE_RUNSTATE_RESET) &&
       (pSeqInfo->eSeqState != I2CDEVQUP_SEQSTATE_COMPLETE))
   {
      /* Try to gracefully stop at data boundary. */
      I2CDEVQUP_SetRunState(pDev->devProps.virtBlockAddr,
            HAL_QUPE_RUNSTATE_PAUSE);

      pSeqInfo->eSeqState = I2CDEVQUP_SEQSTATE_COMPLETE;

      /* CHECK: We still see a Bus Clear and STOP on the fly. If multiple
       * transfers are queued and we paused the core just around the end of one
       * of the transfers, then the bus may already be idle, but we do not know
       * what condition to query to determine that. If you see a stray bus clear
       * and STOP without any START bit, you need to add a logic here */

      /* A PAUSE will let core transfer a couple more bytes and CLK goes to
       * FORCED_LOW_STATE. Note that if we did not reach FORCED_LOW, it means
       * that we have already waited for the clock-stretch amount of time that
       * wsa configured */
      if (!QUP_WaitForI2cBusClkForcedLow(pDev)) {
         I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer(), Wait for FORCED CLK LOW failed", pDev->uPlatDevId);     
      }

      QUP_UpdateTransferCnt(pDev);
      I2CDEVQUP_CalcTransferredDataBytes(pDev);
      pDev->seqInfo.ioRes.nOperationResult = I2C_RES_ERROR_CLS_QUP_DEV_TIMEOUT |
         HAL_qupe_Pack28_OperationalState(pDev->devProps.virtBlockAddr);

      /* Reset core calls into reset pipes and that will cause a deadlock in the
       * bam driver if the callback tries to get the same locks. */

      // I2CDEVQUP_ResetQupCore(pDev);
      // HAL_qupe_Configure(pDev->devProps.virtBlockAddr);

      /* I see that slave stretches the clock here right after the address is
       * sent out on the bus. In this case, we may prematurely put the core into
       * RUN state and that results in BUS ERROR. We must wait for the clock
       * stretch amount of time before doing anything. DATA_STATE is
       * TX_ADDR_STATE at this point. We will check for that.  Note that in this
       * case the core does send out 9 clocks irrespective or a read/write
       * transfer, so we will skip the bus clear.
       */

      if (HAL_qupe_GetI2cDataState(pDev->devProps.virtBlockAddr) ==
          HAL_QUPE_I2C_DATA_TX_ADDR_STATE)
      {
         I2CSYS_BusyWait(pDev->clntCfg.uByteTransferTimeoutUs);
         bSkipBusClear = TRUE;
      }

      I2CDEVQUP_SetRunState(pDev->devProps.virtBlockAddr,
            HAL_QUPE_RUNSTATE_RESET);

      if (!QUP_WaitForI2cBusFree(pDev)) {
         I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer(), After FORCED LOW Wait for IDLE failed", pDev->uPlatDevId);     
      }

      if ((pTransfer->eTranDirection == I2cTranDirIn) &&
          (bSkipBusClear == FALSE))
      {
         /* Bus Clear will not work if core is not in RUN STATE */
         I2CDEVQUP_SetRunState(pDev->devProps.virtBlockAddr,
               HAL_QUPE_RUNSTATE_RUN);

         /* We need to do Bus Clear especially to handle Read termination */ 
         HAL_qupe_ClearI2CBus(pDev->devProps.virtBlockAddr);

         if (!QUP_WaitForI2cBusFree(pDev)) {
            I2C_ERROR_LOG_LEVEL0(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer(), Wait for BUS FREE failed", pDev->uPlatDevId);     
         }     
      }
      /* Disable input output interrupts. */
      HAL_qupe_SetIrqMask(pDev->devProps.virtBlockAddr,
                          HAL_QUPE_IRQMASK_INPUT_SERVICE_FLAG |
                          HAL_QUPE_IRQMASK_OUTPUT_SERVICE_FLAG);
      /* Disable qup error interrupts */
      HAL_qupe_SetErrStatusIrqEnabled(pDev->devProps.virtBlockAddr,0);
   }

   /* Moving the critical section here to avoid a race with IST */
   I2CSYS_LeaveCriticalSection(pDev->hOperationSync);
   
   /* Reset BAM if we cancel an ongoing BAM transfer */
   I2CPLATDEV_ResetTarget(pDev->hDevTgt);


   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_AbortTransfer()=", pDev->uPlatDevId);
   return I2C_RES_SUCCESS;
}

/** @brief Set the power state of the device.

    This Function sets the power state for the device.

    @param[in] hDev           Device handle.
    @param[in] ePowerState    the new power state

    @return          int32 .
  */
int32
I2CDEV_SetPowerState
(
   I2CDEV_HANDLE                        hDev      ,
   I2CDEV_PowerStates                   ePowerState
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res  = I2C_RES_SUCCESS;
   uint32                               uWhileOnce = 0;

   if( NULL == pDev) {
      return I2CDEVQUP_ERR_INVALID_DEV_HANDLE;
   }
   
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 3, "[DEV:0x%x] I2CDEV_SetPowerState(hdev=0x%x, ePowerState=%d)",
                        pDev->uPlatDevId, (uint32) hDev, ePowerState);
   do
   {
      /*
       * The reason for setting the state before actually changing it is that
       * in some cases an interrupt may happen while disabling clocks
       * causing crashes.
       * */     
      pDev->devPowerState = ePowerState;     
      res = I2CPLATDEV_SetPowerState(pDev->hDevTgt, ePowerState);
   } while ( uWhileOnce );
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_SetPowerState()=%d", pDev->uPlatDevId, res);

   return res;
}

/** @brief Gets the power state of the device.

    This Function gets the power state of the device.

    @param[in] hDev           Device handle.
    @param[in] pePowerState   Pointer to power state.

    @return          int32 .
  */
int32
I2CDEV_GetPowerState
(
   I2CDEV_HANDLE                         hDev      ,
   I2CDEV_PowerStates                   *pePowerState
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res = I2C_RES_SUCCESS;
   uint32                               uDevId  = 0;

   if (NULL == hDev) {
      return I2CDEVQUP_ERR_INVALID_DEV_HANDLE;
   }

   uDevId  = pDev->uPlatDevId;

   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 3, "[DEV:0x%x] I2CDEV_GetPowerState(hdev=0x%x, pePowerState=%d)",
                        uDevId, (uint32) hDev, (uint32) pePowerState);
   *pePowerState = pDev->devPowerState;
   
   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 2, "[DEV:0x%x] I2CDEV_GetPowerState()=%d", uDevId, res);
   return res;
}

/** @brief Sends a cmd to the i2c dev.

    This Function  Sends a cmd to the i2c dev.

    @param[in]     hDev            Device handle.
    @param[in]     devCmd          Command to be sent to device.
    @param[inout]  pCmdData        Data related to the command.

    @return          int32 .
  */
int32
I2CDEV_SendCmd
(
   I2CDEV_HANDLE                        hDev      ,
   I2CDEV_Command                       devCmd    ,
   void                                *pCmdData
)
{
   I2CDEVQUP_Device                    *pDev = (I2CDEVQUP_Device *)hDev;
   int32                                res = I2C_RES_SUCCESS;

   (void) pCmdData;

   if (NULL == hDev) {
      return I2CDEVQUP_ERR_INVALID_DEV_HANDLE;
   }

   I2C_CALLTRACE_LEVEL1(pDev->pDevLog, 3, "[DEV:0x%x] I2CDEV_SendCmd(hdev=0x%x, devCmd=%d)",
                         pDev->uPlatDevId, (uint32) hDev, devCmd);     
   switch ( devCmd ) {
      case I2CDEV_COMMAND_RESET_DEVICE:
        res = I2CDEVQUP_ResetQupCore(pDev);      
        break;
      case I2CDEV_COMMAND_NULL:
      default:
        break;
   }

   return res;
}


