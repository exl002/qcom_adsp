#ifndef SLIMBUS_H
#define SLIMBUS_H
/*
===========================================================================

FILE:         SlimBus.h

DESCRIPTION:  This file defines data structures and macros used by 
              the SLIMbus core driver.

===========================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/buses/slimbus/src/common/SlimBus.h#7 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
05/29/14   MJS     Add workaround to limit clock drift due to gear changes.
07/25/13   MJS     Add support for upper watermark, shadow counter and 
                   hardware capability APIs.
07/25/13   MJS     Perform logical addresses assignment from worker thread.
04/30/13   MJS     Support for framer handover.
04/16/13   MJS     Support for retrying satellite ack responses.
03/21/13   MJS     Support for inactive framer.
03/21/13   MJS     Support for standalone satellite hardware configuration.
03/21/13   MJS     Track channel definitions on per-client basis.
03/21/13   MJS     Allow shared channel to be allocated by multiple clients.
03/21/13   MJS     Cleanup low power and pause clock logic.
02/21/13   MJS     Add port synchronization lock.
02/05/13   dj      Fix Code analysis warnings
01/09/12   MJS     Support more than 32 channels using dynamic allocation.
11/29/12   MJS     Support BAM deferred processing within client context.
11/29/12   MJS     Move ResetMsgFifo to common source file.
11/20/12   MJS     Export SlimBus_CleanupMsgEvent.
11/06/12   MJS     Fix for SSR hardware init issue after role reversal.
10/25/12   MJS     Support for voltage control and role reversal.
09/05/12   MJS     Add message queue index normalization function.
08/18/12   MJS     Support to wait to receive ACK for secondary disconnect msg.
07/19/12   MJS     Support for detecting when BAM descriptors have flushed.
07/16/12   SG      Reverting CR371751 fix.
06/29/12   MJS     Increase BAM reset delay from 500us to 5ms.
06/26/12   MJS     Add final port disconnection bitmask for power management.
06/13/12   MJS     Support for standlone satellite low-power management.
06/09/12   MJS     Attempt cleanup of data channels to keep master-satellite
                   state in sync.
05/25/12   MJS     Strip out idle timer.
05/16/12   MJS     Update logging macros.
04/25/12   MJS     Slight restructure for QNX binary compatibility.
04/19/12   MJS     Enhanced logic for detecting initial channel activation.
04/14/12   MJS     Add back GetPortEvent to common code.
04/11/12   MJS     Add common device dealloc routine.
04/09/12   MJS     Fix SAL warnings.
04/04/12   MJS     Reset BAM pipe after delay timer in workloop.
04/04/12   MJS     Fixes for level 4 warnings.
03/28/12   MJS     Compatibility with WPP logging.
02/27/12   MJS     Less resource use by reducing size of descriptor FIFOs.
02/07/12   MJS     Add reconfiguration sequence timeout mechanism.
01/25/12   MJS     Improve BAM driver abstraction.
01/23/12   MJS     Support for entering pause clock only after idle timeout.
01/16/12   MJS     Create platform-specific device structure.
01/16/12   MJS     Split bNoMalloc flag from bInIsr flag.
12/11/11   MJS     Remove references to DAL.
11/29/11   MJS     Add support for deallocating ports before the ports have
                   finished disconnecting.
11/29/11   MJS     Fix race condition where power manager could put driver 
                   into idle state during framer wake up.
10/25/11   MJS     Implement reconfiguration client queue.
10/21/11   MJS     Detect channels being activated from reconfiguration
                   sequence and simulate master ack for the reconfig request.
10/12/11   AV      Added Slimbus SSR feature. 
10/07/11   MJS     Support for purging data channels.
09/27/11   MJS     Add client shared channel support.
09/26/11   MJS     Working pause clock implementation, report unsupported
                   Rx messages.
08/23/11   MJS     Implement alternate SW workaround for CR-0000156195,
                   only disconnect BAM after port is disconnected.
07/19/11   MJS     Fix for spurious disconnection event, wait for manual
                   port disconnection to complete.
06/06/11   MJS     Cache port options, timeout for reenabling port int.
05/27/11   MJS     Add clock control variables.
05/24/11   MJS     Add pause clock support and ref clock management.
05/24/11   MJS     Remove hard-coded max client limit, add GPIO config,
                   make no port disconnection interrupt the norm.
05/13/11   MJS     Increase response timeout value to 100ms.
05/10/11   MJS     Add port disconnection interrupt workaround.
05/10/11   MJS     Add support for slimbus master component data loopback.
04/14/11   MJS     Add logging facility.
04/08/11   MJS     Implement workarounds for Waverider v1 and v2.
06/21/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010, 2011, 2012, 2013, 2014 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SlimBusTypes.h"
#include "halsb.h"
#include "SlimBusConsts.h"

/* Define default logging macros, target header can override these */
#ifndef SB_LOG_DECLARE
#define SB_LOG_DECLARE() uint32 uLogLevel
#endif
#ifndef SB_LOG_INIT
#define SB_LOG_INIT(pDevCtxt)
#endif
#ifndef SB_LOG_ERR_0
  #define SB_LOG_ERR_0(format)
#endif
#ifndef SB_LOG_ERR_1
  #define SB_LOG_ERR_1(format, ...)
#endif
#ifndef SB_LOG_ERR_2
  #define SB_LOG_ERR_2(format, ...)
#endif
#ifndef SB_LOG_ERR_3
  #define SB_LOG_ERR_3(format, ...)
#endif
#ifndef SB_LOG_ERR_4
  #define SB_LOG_ERR_4(format, ...)
#endif
#ifndef SB_LOG_ERR_5
  #define SB_LOG_ERR_5(format, ...)
#endif
#ifndef SB_LOG_ERR_6
  #define SB_LOG_ERR_6(format, ...)
#endif
#ifndef SB_LOG_ERR_7
  #define SB_LOG_ERR_7(format, ...)
#endif
#ifndef SB_LOG_WARN_0
  #define SB_LOG_WARN_0(format)
#endif
#ifndef SB_LOG_WARN_1
  #define SB_LOG_WARN_1(format, ...)
#endif
#ifndef SB_LOG_WARN_2
  #define SB_LOG_WARN_2(format, ...)
#endif
#ifndef SB_LOG_WARN_3
  #define SB_LOG_WARN_3(format, ...)
#endif
#ifndef SB_LOG_WARN_4
  #define SB_LOG_WARN_4(format, ...)
#endif
#ifndef SB_LOG_WARN_5
  #define SB_LOG_WARN_5(format, ...)
#endif
#ifndef SB_LOG_WARN_6
  #define SB_LOG_WARN_6(format, ...)
#endif
#ifndef SB_LOG_WARN_7
  #define SB_LOG_WARN_7(format, ...)
#endif
#ifndef SB_LOG_INFO_0
  #define SB_LOG_INFO_0(format, ...)
#endif
#ifndef SB_LOG_INFO_0
  #define SB_LOG_INFO_0(format)
#endif
#ifndef SB_LOG_INFO_1
  #define SB_LOG_INFO_1(format, ...)
#endif
#ifndef SB_LOG_INFO_2
  #define SB_LOG_INFO_2(format, ...)
#endif
#ifndef SB_LOG_INFO_3
  #define SB_LOG_INFO_3(format, ...)
#endif
#ifndef SB_LOG_INFO_4
  #define SB_LOG_INFO_4(format, ...)
#endif
#ifndef SB_LOG_INFO_5
  #define SB_LOG_INFO_5(format, ...)
#endif
#ifndef SB_LOG_INFO_6
  #define SB_LOG_INFO_6(format, ...)
#endif
#ifndef SB_LOG_INFO_7
  #define SB_LOG_INFO_7(format, ...)
#endif
#ifndef SB_LOG_VERB_0
  #define SB_LOG_VERB_0(format)
#endif
#ifndef SB_LOG_VERB_1
  #define SB_LOG_VERB_1(format, ...)
#endif
#ifndef SB_LOG_VERB_2
  #define SB_LOG_VERB_2(format, ...)
#endif
#ifndef SB_LOG_VERB_3
  #define SB_LOG_VERB_3(format, ...)
#endif
#ifndef SB_LOG_VERB_4
  #define SB_LOG_VERB_4(format, ...)
#endif
#ifndef SB_LOG_VERB_5
  #define SB_LOG_VERB_5(format, ...)
#endif
#ifndef SB_LOG_VERB_6
  #define SB_LOG_VERB_6(format, ...)
#endif
#ifndef SB_LOG_VERB_7
  #define SB_LOG_VERB_7(format, ...)
#endif
#ifndef SB_LOG_VERB_8
  #define SB_LOG_VERB_8(format, ...)
#endif


/*------------------------------------------------------------------------------
Declaring a "SlimBus" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct SlimBusDrvCtxt SlimBusDrvCtxt;
typedef struct SlimBusDevCtxt SlimBusDevCtxt;
typedef struct SlimBusClientCtxt SlimBusClientCtxt;
struct SlimBusDevTargetCtxt;
typedef struct SlimBusBasicClientCtxt SlimBusBasicClientCtxt;
struct SlimBusIsrCtxt;
typedef struct SlimBusBamCtxt SlimBusBamCtxt;

#include "SlimBusTarget.h"

#define BITS_PER_UINT32 32

/* HANDLE: */
#define SB_HANDLE_IDX_MASK  0x000000ff
#define SB_HANDLE_PRN_MASK  0x0000ff00
#define SB_HANDLE_LA_MASK   0x0000ff00
#define SB_HANDLE_TYPE_MASK 0x00ff0000

#define SB_HANDLE_IDX_SHFT  0
#define SB_HANDLE_PRN_SHFT  8
#define SB_HANDLE_LA_SHFT   8
#define SB_HANDLE_TYPE_SHFT 16

#define SB_GET_HANDLE_IDX( handle ) \
   (((handle)&SB_HANDLE_IDX_MASK)>>SB_HANDLE_IDX_SHFT)
#define SB_GET_HANDLE_PRN( handle ) \
   (((handle)&SB_HANDLE_PRN_MASK)>>SB_HANDLE_PRN_SHFT)
#define SB_GET_HANDLE_LA( handle ) \
   (((handle)&SB_HANDLE_LA_MASK)>>SB_HANDLE_LA_SHFT)
#define SB_GET_HANDLE_TYPE( handle ) \
   ((SlimBusHandleType)(((handle)&SB_HANDLE_TYPE_MASK)>>SB_HANDLE_TYPE_SHFT))

#define SB_SET_HANDLE_IDX( idx ) \
   (((idx)<<SB_HANDLE_IDX_SHFT)&SB_HANDLE_IDX_MASK)
#define SB_SET_HANDLE_PRN( prn ) \
   (((prn)<<SB_HANDLE_PRN_SHFT)&SB_HANDLE_PRN_MASK)
#define SB_SET_HANDLE_LA( la ) \
   (((la)<<SB_HANDLE_LA_SHFT)&SB_HANDLE_LA_MASK)
#define SB_SET_HANDLE_TYPE( type ) \
   (((type)<<SB_HANDLE_TYPE_SHFT)&SB_HANDLE_TYPE_MASK)

#define SB_PRN_MAX_VALUE ((SB_HANDLE_PRN_MASK>>SB_HANDLE_PRN_SHFT)+1)

#define SB_DEFAULT_CLIENTS_PER_DEVICE 4

#define SB_MAX_LOCAL_DATA_CHANNELS 32
#define SB_MAX_DATA_CHANNEL_SLOTS 128

#define SB_MAX_MASTER_PORTS  24
#define SB_MAX_DEVICES       16
#define SB_MAX_COUNTERS      12
#define SB_MAX_GROUPS        16
#define SB_MAX_TIDS          64

#define SB_MAX_DATA_CHANNEL_PARAMS 256

#define SB_LA_IDX_MASK  0x0f
#define SB_TID_IDX_MASK 0x07

#define SB_LA_DEFAULT_BASE 128

#define SB_DEFAULT_RX_TIMEOUT 400000
#define SB_TIMEOUT_FACTOR 4
#define SB_RECONFIG_TIMEOUT_COUNT 3

#define SB_BAM_RESET_TIMEOUT 500

#define SB_HANDLE_INVALID 0xffffffff

#define SB_ACK_SUCCESS_FLAG 0x08000000
#define SB_ACK_FAILURE_FLAG 0x10000000

#define SB_TX_MSG_BUF_LENGTH 256
#define SB_RX_MSG_BUF_LENGTH 64
#define SB_CFG_MSG_BUF_LENGTH 64
#define SB_RX_MSG_DESC_SIZE 4

#define SB_TX_DESC_FIFO_SIZE 0x100
#define SB_RX_DESC_FIFO_SIZE 0x80

#define SB_MAX_MSG_LENGTH_BYTES 40
#define SB_MAX_MSG_LENGTH_WORDS (SB_MAX_MSG_LENGTH_BYTES/sizeof(uint32))

#define SB_ACK_IDX_UNUSED 0xffffffff

#define SB_MAX_CORE_HW_REVS 4

#define SB_MAX_CORE_VERSIONS 3

#define SB_MAX_GPIOS 2

#define SB_MAX_DATA_SEGMENT_LEN 8

#define SB_LOG_LEVEL_UNDEFINED -1
#define SB_LOG_LEVEL_DEFAULT 1

/* Number of microseconds to wait for message sync recovery following clock pause */
#define SB_PAUSE_RECOVERY_USEC 2500

/* Reserve 480000 bps for messaging at 24.576 MHz = 30 slots */
#define SB_INTERNAL_RESV_MSG_SLOTS 30

#define SB_MAX_INT_REENABLE_WAIT 100

#define SB_TID_NOACK 0xffffffff

#define SB_QC_SUPPORTED_PROTOCOLS  SB_QC_MASTER_PROTOCOL_V1

#define SB_MSG_ASM_FIRST_WORD(length,mt,mc,dt,la) \
   (SB_MSG_SET(length,GENERIC_0,LEN)| \
    SB_MSG_SET(mt,GENERIC_0,MT)| \
    SB_MSG_SET(mc,GENERIC_0,MC)| \
    SB_MSG_SET(dt,GENERIC_0,DT)| \
    SB_MSG_SET(la,GENERIC_0,LA))
   
#define SB_GET_MGR_DEV_PTR(pDevCtxt) \
   ((pDevCtxt)->pDevices[SB_MSG_LA_MANAGER&SB_LA_IDX_MASK])
#define  SLIMBUS_PORT_INVALID_FLOW ((SlimBusPortFlowType)0x7ffffffe)

#define SB_IS_CHAN_BITMAP_SET(puBitMap, uIdx) \
  (((uIdx)<SB_MAX_DATA_CHANNEL_PARAMS)?((puBitMap)[(uIdx)/BITS_PER_UINT32]&(1<<((uIdx)%BITS_PER_UINT32))):0)
  
#define SB_SET_CHAN_BITMAP(puBitMap, uIdx) \
   if ((uIdx)<SB_MAX_DATA_CHANNEL_PARAMS) (puBitMap)[(uIdx)/BITS_PER_UINT32]|=(1<<((uIdx)%BITS_PER_UINT32))

#define SB_CLEAR_CHAN_BITMAP(puBitMap, uIdx) \
   if ((uIdx)<SB_MAX_DATA_CHANNEL_PARAMS) (puBitMap)[(uIdx)/BITS_PER_UINT32]&=(~(1<<((uIdx)%BITS_PER_UINT32)))

#define SLIMBUS_ERR_RETRY          0x78000000
#define SLIMBUS_ERR_UNHANDLED_MSG  0x78000001

/* Enumeration of Slimbus driver modes */
typedef enum
{
  SB_MASTER_DRIVER,
  SB_SATELLITE_DRIVER
} SlimBusDriverType;

/* Enumeration of coefficients for segment rate multipliers */
typedef enum
{
  SB_RATE_COEFF_1 = 0,
  SB_RATE_COEFF_3 = 1,
} SlimBusRateCoeffType;

/* Enumeration of types of client resource handles */
typedef enum
{
  SB_HANDLE_TYPE_MASTER_PORT = 1,
  SB_HANDLE_TYPE_SLAVE_PORT,
  SB_HANDLE_TYPE_CHANNEL,
  SB_HANDLE_TYPE_PROGRESS_COUNTER,
  SB_HANDLE_TYPE_GROUP,
  SB_HANDLE_TYPE_SHARED_CHANNEL
} SlimBusHandleType;

/* Enumeration of client resource handle states */
typedef enum
{
  SB_HANDLE_UNAVAILABLE,
  SB_HANDLE_AVAILABLE,
  SB_HANDLE_ALLOCATED,
  SB_HANDLE_DEALLOCATED
} SlimBusHandleStateType;

/* Enumeration of Slimbus device enumeration states */
typedef enum
{
  SB_ENUM_INACTIVE,
  SB_ENUM_RESERVED,
  SB_ENUM_PRESENT,
  SB_ENUM_ACTIVE,
} SlimBusEnumStateType;

/* Enumeration of message FIFO flags */
typedef enum
{
  SB_MSG_FIFO_FLAGS_NONE = 0,
  SB_MSG_FIFO_ENABLE_NOTIFY = 1,  /* Whether completion notification is supported */
  SB_MSG_FIFO_USE_RX_OVERHANG = 2,  /* This message FIFO uses Rx overhang logic */
} SlimBusMsgFifoFlagType;

/* Enumerations of hardware bugs requiring software workarounds */
typedef enum
{
  SB_WRKRND_O_NO_PORT_MULTI_CHAN = 0x1,
  SB_WRKRND_O_PC_FIFO_SAMPLE_OFFSET = 0x2,
  SB_WRKRND_O_RX_PORT_RESET = 0x4,
  SB_WRKRND_O_IE_CLEAR = 0x8,
  /* Below is not actually a HW bug */  
  SB_WRKRND_O_SETUP_TRUST = 0x1000,
} SlimBusWorkaroundType;

/* Enumeration of pause clock states */
typedef enum
{
  SB_PAUSE_NOT_PAUSED,
  SB_PAUSE_ENTERING_PAUSE,
  SB_PAUSE_PLACEHOLDER_2,
  SB_PAUSE_PAUSED,
  SB_PAUSE_PLACEHOLDER_4,
  SB_PAUSE_RESUMING_FROM_PAUSE
} SlimBusPauseStateType;

/* Enumeration of master port disconection states */
typedef enum
{
  SB_PORT_NOT_DISCONNECTING,
  SB_PORT_DISCONNECTING,
  SB_PORT_WAITING_TO_DISCONNECT
} SlimBusPortDisconnectStateType;

typedef enum
{
  SB_FRM_HANDOVER_IDLE,
  SB_FRM_HANDOVER_PREPARE,
  SB_FRM_HANDOVER_EXECUTE,
  SB_FRM_HANDOVER_CLEANUP
} SlimBusFramerHandoverStateType;

typedef struct
{
  /* Structure version */
  uint32 uStructVer;
  /* String name for this driver instance */
  const char *pszInstName;
  /* Base enumeration address for the master device */
  uint8 uaMasterEA[SLIMBUS_ENUM_ADDR_BUF_SIZE];
  /* HWIO base address string */
  char *pszHwioBase;
  /* Offset from the HWIO base address */
  uint32 uHwioBaseOffset;
  /* HWIO base physical address */
  uint32 uHwioBase;
  /* Handle to the BAM device handle */
  uint32 hBamDev;
  /* The interrupt ID */
  uint32 uIntId;
  /* The BAM interrupt ID */
  uint32 uBamIntId;
  /* The EE number for this driver */
  uint32 uMyEE;
  /* The GPIOs to configure */
  uint32 uaGpioConfig[SB_MAX_GPIOS];
  /* The GPIO interrupt number to configure */
  uint32 uGpioIntNum;
  /* The Slimbus core version corresponding to different revisions of the MSM HW */
  uint32 uCoreVer[SB_MAX_CORE_HW_REVS];
} SlimBusBSPType;

/* Basic linked-list structure for Slimbus linked lists */
typedef struct SlimBusLinkNodeType
{
  struct SlimBusLinkNodeType *pNext;
  struct SlimBusLinkNodeType *pPrev;
} SlimBusLinkNodeType;

typedef struct SlimBusDeviceType SlimBusDeviceType;
typedef struct SlimBusMsgFifoType SlimBusMsgFifoType;
/* Structure for circular message FIFO */
/* Note that the indexes are used modulo the FIFO length, and
   are maintained such that following is always true:
     head index >= tail index >= acknowledgement index */
/* If the acknowledgement index is not used it is set to SB_ACK_IDX_UNUSED */
/* pBuf must point to a buffer that is SB_MAX_MSG_LENGTH_BYTES longer than uLen */
struct SlimBusMsgFifoType
{
  uint32 *pBuf;  /* Pointer to buffer */
  uint32 *pPhysBuf;  /* Pointer to the physical buffer */
  uint32 uLen;  /* Length of buffer, must be power of 2 */
  uint32 uOverhang;  /* Number bytes occupied in the buffer past uLen */
  uint32 uHeadIdx;  /* Head index, location where new messages are allocated */
  uint32 uTailIdx;  /* Tail index, location of oldest unsent message */
  uint32 uAckIdx;  /* Acknowledgement index, location of sent but unacked messages */
  SBBOOL bBusy;  /* Whether the message interface is busy */
  SlimBusMsgFifoFlagType eFlags;  /* Flags controlling how the FIFO is used */
  SlimBusBamCtxt *pBamCtxt;  /* Pointer to the message queue BAM context */
  void *hDevCtxt; /* Pointer to device context */
  /* Function pointer to allocate space for a new message in the FIFO */
  uint32* (*getBufFcn)(SlimBusClientCtxt *, SlimBusDeviceType *, SlimBusMsgFifoType *, uint32);
  /* Function to send/process the latest messages added to the FIFO */
  void (*sendBufFcn)(SlimBusDevCtxt *, SlimBusMsgFifoType *);
};

/* Union of data used for Tx markers for event notification */
typedef union
{
  uint32 uCount;
  uint32 *pBuf;
} SlimBusTxMarkerType;

/* Structure for Slimbus event notification */
typedef struct SlimBusEventNodeType
{
  /* Circular linked list node */
  SlimBusLinkNodeType Node;
  /* Where the event node should be returned when its freed */
  SlimBusLinkNodeType *pFreeRoot;
  /* Event handle */
  SlimBusEventHandle hEvent;
  /* Indicator used to process Tx message events */
  SlimBusTxMarkerType TxMarker;
  /* Indicator used to process Rx message events */
  uint32 uRxMarker;
  /* Pointer to location to store Rx data */
  uint8 *pBuf;
  /* Pointer to location to store size of Rx data */
  uint32 *pBufSize;
  /* Are we doing a synchronous wait internal to the driver? */
  SBBOOL bSyncWait;
  /* Pointer to location to store messaging error code */
  SlimBusErrorType *peMsgErr;
} SlimBusEventNodeType;

typedef struct SlimBusChannelType SlimBusChannelType;
typedef struct SlimBusProgressCounterType SlimBusProgressCounterType;
typedef struct SlimBusGroupType SlimBusGroupType;

/* Structure for Slimbus master ports */
typedef struct
{
  /* Linked list node */
  SlimBusLinkNodeType LinkNode;
  /* master port number */
  uint32 portNum;
  /* Handle for this master port */
  SlimBusResourceHandle handle;
  /* Allocation state of this port handle */
  SlimBusHandleStateType eState;
  /* Client requirements for this port */
  SlimBusPortReqType eReqs;
  /* Client options for this port */
  SlimBusPortOptionsType eOptions;
  /* Pointer to the BAM context for this port */
  SlimBusBamCtxt *pBamCtxt;
  /* Is the BAM handle connected? */
  SBBOOL bBamConnected;
  /* Pointer to client owning this master port */
  SlimBusClientCtxt *pClientCtxt;
  /* Pointer to channel structure connected to this port */
  SlimBusChannelType *pChannel;
  /* Pointer to counter structure connected to this port */
  SlimBusProgressCounterType *pCounter;
  /* Direction of flow for this port */
  SlimBusPortFlowType eFlow;
  /* Client event handle for port notifications */
  SlimBusEventHandle hEvent;
  /* User data pointer for event notifications */
  void *pEventUserData;
  /* Bitmap of events that client wants notification for this port */
  SlimBusPortEventType eEnabledEvents;
  /* Bitmap of events that have occurred for this port */
  SlimBusPortEventType eActiveEvents;
  /* Should disconnection event be signalled to the client? */
  SBBOOL bSignalDisconnect;
  /* Counter incremented when client is waiting to be signalled for 
     overflow or underflow event; client should be signalled when count
     is greater than zero */
  SBBOOL uSignalDescEmptyCnt;
  /* Is the port in the middle of a disconnection? */
  SlimBusPortDisconnectStateType eDisconnectState;
  /* Number of BAM descriptors submitted (for debug) */
  uint32 uDescSubmit;
  /* Number of bytes submitted in descriptors for processing (for debug) */
  uint32 uBytesSubmit;
} SlimBusMasterPortType;

/* Structure for Slimbus channels */
struct SlimBusChannelType
{
  /* Linked list node for group containing this channel */
  SlimBusLinkNodeType GroupNode;
  /* Pointer to the group containing this channel, for debug only */
  SlimBusGroupType *pGroup; /* For DEBUGGING only */
  /* Channel number */
  uint32 channelNum;
  /* Handle for this channel */
  SlimBusResourceHandle handle;
  /* Allocation state for this channel */
  SlimBusHandleStateType eState;
  /* Pointer to the client owning this channel */
  SlimBusClientCtxt *pClientCtxt;
  /* Updated (pending) status for this channel */
  SlimBusDataChannelStatusType eNewStatus;
  /* Current (active) status for this chanel */
  SlimBusDataChannelStatusType eCurStatus;
  /* Head of linked list of master ports connected to this channel */
  SlimBusLinkNodeType MasterPortList;
  /* Channel parameters requested by client, for debugging only */
  SlimBusChannelReqType ChannelReq;  /* For DEBUGGING only */
};

/* Structure for Slimbus progress counters */
struct SlimBusProgressCounterType
{
  /* The counter number */
  uint32 counterNum;
  /* Handle for this progress counter */
  SlimBusResourceHandle handle;
  /* Allocation state for this counter */
  SlimBusHandleStateType eState;
  /* Pointer to client owning this counter */
  SlimBusClientCtxt *pClientCtxt;
  /* Pointer to the port associated with this counter */
  SlimBusMasterPortType *pPort;
};

/* Enumeration of channel activation states */
typedef enum
{
  SB_CHANNEL_INACTIVE,
  SB_CHANNEL_NEW,
  SB_CHANNEL_UPDATED,
  SB_CHANNEL_ACTIVE,
  SB_CHANNEL_PENDING_REMOVAL
} SlimBusChannelAllocType;

#define SB_GROUP_FLAG_START 1
#define SB_GROUP_FLAG_END   2

/* Structure of TDM channel parameters */
typedef struct
{
  /* Linked list node */
  SlimBusLinkNodeType LinkNode;
  /* Channel number */
  uint32 channelNum;
  /* Activation state for this channel */
  SlimBusChannelAllocType eStatus;
  /* Segment length of the channel */
  uint8 ucSegLen;
  /* Protocol ID for the channel */
  uint8 ucProtocol;
  /* Coefficient of the channel segment rate multiplier */
  SlimBusRateCoeffType eCoeff;
  /* Exponent of the segment rate multiplier under the root superframe */
  uint8 ucRootExp;
  /* The current (active) segment interval */
  uint32 uCurInterval;
  /* The current (active) segment offset */
  uint32 uCurOffset;
  /* The updated (pending) segment interval */
  uint32 uNewInterval;
  /* The updated (pending) segment interval */
  uint32 uNewOffset;
  /* Flags indicating the grouping of this channel */
  uint32 uGrpFlag;
  /* The current reference count for the channel */
  uint32 uCurRefCnt;
  /* The updated (pending) reference count for the channel */
  uint32 uNewRefCnt;
  /* The curent clock drift for this channel */
  int32 iClkGearDrift;
  /* The max allowed clock drift for this channel */
  int32 iMaxClkGearDrift;
} SlimBusChannelParamType;

/* Structure for base client context data, 
   applies to both local and remote clients */
struct SlimBusBasicClientCtxt
{
  /* Linked list node */
  SlimBusLinkNodeType LinkNode;
  /* FIFO for reconfiguration sequence messages */
  SlimBusMsgFifoType ConfigMsgFifo;
  /* The updated (pending) number of message slots reserved for this client */
  uint32 uNewReservedMsgSlots;
  /* The current (active) number of message slots reserved for this client */
  uint32 uCurReservedMsgSlots;
  /* The TID associated with the current client reconfiguration sequence 
     (remote clients only) */
  uint32 uConfigTID;
  /* Pointer to the device structure for NGD associated with the client EE
     (remote clients only) */
  SlimBusDeviceType *pDevice;
  /* The client number for this client (number is unique per EE) */
  uint32 uClientNum;
  /* Pointer to the main client data, for local clients only */
  SlimBusClientCtxt *pClientCtxt;
  /* Do we need to purge the client's owned data channels? */
  SBBOOL bPurgeNeeded;
  /* The active channels owned or partially-owned by this client */
  uint32 uaChanBitMap[SB_MAX_DATA_CHANNEL_PARAMS/BITS_PER_UINT32];
};

/* Structure containing data used by the master for satellite EEs */
typedef struct
{
  /* The number of client contexts associated with this satellite */
  uint32 numClientSlots;
  /* Array of client contexts associated with the satellite */
  SlimBusBasicClientCtxt **paClientCtxt;
  /* Have we sent a master capability message and are expecting a reply? */
  SBBOOL bExpectAnnouce;
  /* What satellite messaging protocol version to use for this satellite */
  uint32 uSatProtoVer;
} SlimBusSatelliteType;

/* Structure for native Slimbus software devices */
struct SlimBusDeviceType
{
  /* Enumeration address of the device */
  uint8 aEA[SLIMBUS_ENUM_ADDR_BUF_SIZE];
  /* Device class */
  uint8 DC;
  /* Device class version */
  uint8 DV;
  /* Logical address assigned to the device */
  SlimBusLogicalAddrType LA;
  /* Enumeration state of the device */
  SlimBusEnumStateType eState;
  /* Maximum number of TIDs supported by the device */
  uint32 uMaxTrans;
  /* Number of available (unused) TIDs */
  uint32 uAvailTrans;
  /* head for linked list of events to wait for available TIDs */
  SlimBusLinkNodeType TransEventQueue;
  /* Index map of outstanding TIDs sent to the device */
  SlimBusEventNodeType *TIDMap[SB_MAX_TIDS];
  /* Base TID offset to use for the device */
  uint32 uBaseTID;
  /* Next TID index to use for the device */
  uint32 uNextTIDIdx;
  /* Pointer to satellite driver context for the device (NGDs only) */
  SlimBusSatelliteType *pSat;
};

/* Slimbus resource group structure */
struct SlimBusGroupType
{
  /* Head of the linked list of resources in the group */
  SlimBusLinkNodeType ResourceList;
  /* Handle for the resource group */
  SlimBusResourceHandle handle;
};

/* Structure for overall Slimbus scheduling parameters and state */
typedef struct
{
  /* Index map of all active channels */
  SlimBusChannelParamType *apChanParams[SB_MAX_DATA_CHANNEL_PARAMS];
  /* Linked list head for channels with SRM coefficient of 3 */
  SlimBusLinkNodeType ChanParamList3;
  /* Linked list head for channels with SRM coefficient of 1 */
  SlimBusLinkNodeType ChanParamList1;
  /* The current subframe mode */
  uint32 uSubFrmCode;
  /* The number of slots used */
  uint32 uSlotsUsed;
  /* The number of message slots used */
  uint32 uMsgSlotReq;
  /* Whether there is a reconfiguration sequence in progress on the bus */
  SBBOOL bReconfigBusy;
  /* The current clock pause state */
  SlimBusPauseStateType ePause;
  /* Whether to enable pause clock to save power */
  SBBOOL bEnablePause;
  /* Whether to purge channels */
  SBBOOL bPurge;
  /* Array representing all the slots in the superframe, to verify 
     bandwidth allocation correctness (for debugging only) */
  uint8 aSlotArr[SB_SLOTS_PER_SUPERFRAME]; /* For DEBUGGING only */
  
  /* Linked list head for ununsed scheduling event nodes */
  SlimBusLinkNodeType FreeSchedEventStack;
  /* Event node that can be used by the master scheduling context */
  SlimBusEventNodeType SchedEventNode;
  /* Handle to the master scheduling event */
  SlimBusEventHandle hSchedEvent;
  /* Number of times timer event has occured while waiting for hSchedEvent */
  uint32 uSchedTimerCnt;
  
  /* Framer handover state */
  SlimBusFramerHandoverStateType eFrmHandState;
  /* LA of remote framer device used for framer handover */
  SlimBusLogicalAddrType FrmHandFrmLA;
  /* LA of NGD device associated with remote component for framer handover */
  SlimBusLogicalAddrType FrmHandNgdLA;
} SlimBusScheduleType;

/* Slimbus device structure */
struct SlimBusDevCtxt
{
  //Base Members
  SlimBusBaseDevType Base;
  //SlimBus Dev state can be added by developers here
  
  /* The type of driver, master or satellite */
  SlimBusDriverType eDrvType;
  
  /* Handle to the Slimbus HAL */
  HAL_sb_HandleType hSB;
  /* The HAL enumeration for the messaging device to use */
  HAL_sb_DeviceType eMsgDev;
  
  /* Device synchronization handle */
  SlimBusSyncHandle hSync;
  /* Curent pseudo-random number seed for handle uniqueness */
  uint8 prnSeed;

  /* Pointer to the device BSP data */
  SlimBusBSPType *pBSP;
  /* The software workarounds required for the HW */
  SlimBusWorkaroundType eWrkrnd;
  
  /* Do we own the hardware instance (whether as master or satellite)? */
  SBBOOL bIsHwOwner;
  
  /* Does this hardware instance contain the active framer? */
  SBBOOL bActiveFramer;

  /* The clock gear as seen by the framer */
  uint32 uFrmClkGear;
  /* The default clock gear */
  uint32 uDefClkGear;
  /* The clock gear as seen by power management */
  uint32 uPowerClkGear;
  
  /* The number of clients that can attached to this device */
  uint32 numClientSlots;
  /* Index map of clients attached to this device */  
  SlimBusClientCtxt **paClientCtxt;
  
  /* Array of master ports allocatable for this device */
  SlimBusMasterPortType Ports[SB_MAX_MASTER_PORTS];
  /* Size of the master port array */
  uint32 numPorts;
  /* Array of pointers to channels allocatable for this device */
  SlimBusChannelType *pChannels[SB_MAX_LOCAL_DATA_CHANNELS];
  /* Size of the channel array */
  uint32 numChannelSlots;
  /* Size of the shared channel array */
  uint32 numSharedChannelSlots;
  /* Array of pointers to Slimbus devices (enumerated or not) */
  SlimBusDeviceType *pDevices[SB_MAX_DEVICES];
  /* Size of the Slimbus device array */
  uint32 numDeviceSlots;
  /* Array of resource groups allocatable for this device */
  SlimBusGroupType Groups[SB_MAX_GROUPS];
  /* Size of the resource group array */
  uint32 numGroupSlots;
  /* Array of progress counters allocatable for this device */
  SlimBusProgressCounterType Counters[SB_MAX_COUNTERS];
  /* Size of the progress counter array */
  uint32 numCounterSlots;
  
  /* Port synchronization handle */
  SlimBusSyncHandle hPortSync;

  /* The bitmask of ports that are awaiting disconnection but will not
     generate a port disconnection interrupt.  For these ports we need
     to check whether they are still active or not whenever we receive 
     a reconfig_done interrupt. */
  uint32 uPortDisconnectMask;
  
  /* The bitmask of ports that are awaiting disconnection and will
     generate a port disconnection interrupt.  This bitmask is used 
     for power management purposes, to make sure the GPIOs are not 
     disabled if we are waiting for a disconnect. */
  uint32 uPortFinalDisconnectMask;
  
  /* The bitmask of ports that are have been ACKed by master as successfully
     activated in a reconfiguration sequence, but are not actually active yet
     because the reconfiguration boundary has not been reached */
  uint32 uPortConnectMask;
    
  /* The mask for BAM ports that need to be reset when the BAM reset
     timer fires */
  uint32 uPortBamResetMask;
  /* The ports that need to be reset that were collected after the 
     BAM reset timer was started, after firing once the timer will be
     restarted and then these ports will be reset */
  uint32 uPortBamResetDeferMask;
  
  /* The port number base offset for ports allocated by this device */
  uint32 uBasePortNum;
  /* The channel number base offset for channels allocated by this device */
  uint32 uBaseChanNum;
  
  /* The shared channel number base offset */
  uint32 uBaseSharedChanNum;

  /* The LA of the master PGD */
  SlimBusLogicalAddrType pgdLA;
  
  /* Need to keep certain calculations within 32-bit integer range, so store
     root frequency divided by 8 instead of actual root freq */
  uint32 uRootFreqDiv8;
  uint32 uSuperFreq;
  
  /* Message FIFO structure for Tx messages */  
  SlimBusMsgFifoType TxMsgFifo;
  /* Message FIFO structure for Rx messages */  
  SlimBusMsgFifoType RxMsgFifo;
  /* Handle to the message queue BAM event */
  SlimBusEventHandle hBamEvent;

  /* Handle to physical memory used for message buffers */
  SlimBusMemHandle hPhysMem;

  /* Head of the linked list for Tx events */
  SlimBusLinkNodeType TxEventQueue;
  /* Head of the linked list for Rx events */
  SlimBusLinkNodeType RxEventQueue;
    
  /* Handle for the Rx message workloop */
  SlimBusWorkLoopHandle hMsgWorkLoop;
  /* Event handle for the Rx message workloop */
  SlimBusEventHandle hMsgWorkLoopEvent;
  /* Has the Rx message workloop been signalled already? */
  SBBOOL bMsgWorkLoopSignalled;

  /* Handle for the master workloop */
  SlimBusWorkLoopHandle hMasterWorkLoop;
  /* Event handle for the master workloop */
  SlimBusEventHandle hMasterWorkLoopEvent;
  
  /* List of clients waiting to do a reconfiguration */
  SlimBusLinkNodeType ReconfigList;
  
  /* Number of local clients current pending completion of reconfig */
  uint32 uLocalReconfigPendCnt;
  
  /* List of clients waiting for message transaction completion */
  SlimBusLinkNodeType MsgWaitList;
  
  /* Handle to the timer */
  SlimBusTimerHandle hTimerHandle;
  /* Handle to the Timer event */
  SlimBusEventHandle hTimerEvent;
  /* Handle for the timer workloop */
  SlimBusWorkLoopHandle hTimerWorkLoop;
  /* Event handle for the timer workloop */
  SlimBusEventHandle hTimerWorkLoopEvent;
  /* Is the timer currently active? */
  SBBOOL bTimerActive;
  
  /* Handle to the BAM Reset timer */
  SlimBusTimerHandle hBamResetTimerHandle;
  /* Handle to the BAM Reset Timer event */
  SlimBusEventHandle hBamResetTimerEvent;
  /* Handle for the BAM Resset Timer workloop */
  SlimBusWorkLoopHandle hBamResetTimerWorkLoop;
  /* Event handle for the BAM Reset Timer workloop */
  SlimBusEventHandle hBamResetTimerWorkLoopEvent;

  /* Pointer to client context used for BAM reset timer callbacks */
  SlimBusClientCtxt *pBamResetClientCtxt;
  
  /* Event handle to synchronize device initialization */
  SlimBusEventHandle hInitEvent;
  
  /* Are we inside of an ISR? */
  SBBOOL bInIsr;
  /* Are we within a context where malloc is not allowed? */
  SBBOOL bNoMalloc;
  /* Are we in a timer callback */
  SBBOOL bInTimer;
  /* Are we in the process of exiting the driver (in case we need the functionality) */
  SBBOOL bInExit;
  
  /* Whether to use BAM queues or CSR access for messaging */
  SBBOOL bUseRxMsgQueue;
  SBBOOL bUseTxMsgQueue;
  
  /* Structure used to store bandwidth allocation state */
  SlimBusScheduleType *pSched;
  
  /* head for linked list of events to wait for resume from clock pause */
  SlimBusLinkNodeType PauseEventQueue;
  /* the number of clients that are waiting to send Tx messages */
  uint32 uNumTxMsgWaiters;
    
  /* Is the clock currently on? */
  SBBOOL bClockIsOn;

  /* The version of the satellite messaging protocol to use to communicate
     with the master driver (applies only to satellite driver) */
  uint32 uSatProtoVer;
  
  /* Pointer to client waiting for the reconfiguration done interrupt */
  SlimBusClientCtxt *pReconfigWaitClient;
  
  /* Logging data structures */  
  SB_LOG_DECLARE();
  
  /* Platform-specific device data structure */
  SlimBusDevPlatType Plat;
    
  /* Handle to the BAM descriptor memory associated with the Slimbus */
  SlimBusMemHandle hBamDescMem;
  
  const uint8 *auEeAssign;
  
  /* Number of clients that have the device open */
  uint32 uOpenCnt;
  /* Has the device ever been opened before? */
  SBBOOL bEverOpened;
  /* Has the hardware been initialized before? */
  SBBOOL bHwInit;
  
  /* Function to perform driver-specific logic to connect a port to a channel */
  SBResult (*connectPortFcn)(SlimBusClientCtxt *, SlimBusDeviceType *, SlimBusPortFlowType, uint32, uint32 );
  /* Function to perform driver-specific logic to disconnect a port from a channel */
  SBResult (*disconnectPortFcn)(SlimBusClientCtxt *, SlimBusDevCtxt *, SlimBusDeviceType *, uint32 );
  /* Function to process a reconfiguration sequence */
  SBResult (*doReconfigSeqFcn)(SlimBusClientCtxt *);
  /* Function to check and enter Slimbus low-power mode */
  SBResult (*checkEnterLowPowerFcn)(SlimBusDevCtxt *);
  /* Function to leave Slimbus low-power mode */
  SBResult (*leaveLowPowerFcn)(SlimBusDevCtxt *);

  /* Bit to store the state of SSR cb registration */
  SBBOOL bSSRcbRegistered;

  /* SSR state, true during Subsystem restart */
  SBBOOL bPendingSSR;
};

/* Slimbus driver state */
struct SlimBusDrvCtxt
{
  //Base Members
  SlimBusBaseDrvType Base;
  SlimBusDevCtxt SlimBusDevCtxt[SLIMBUS_TARGET_NUM_DEVICES];
  //SlimBus Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "SlimBus" Client Context
------------------------------------------------------------------------------*/
struct SlimBusClientCtxt
{
  //Base Members
  uint32  dwRefs;
  uint32  dwAccessMode;
  void *pPortCtxt;
  void *hDevCtxt;
  SlimBusRefHandle SlimBusRefHandle;
  //SlimBus Client state can be added by developers here
  
  /* Has Device_Open() been called for the client context? */
  SBBOOL bOpen;
  
  /* Old state of bOpen during subsystem restart */
  SBBOOL bOpenb4SSR;
  
  /* Array of event nodes that can be used by this client context */
  SlimBusEventNodeType EventNodes[8];
  /* Linked list head for ununsed event nodes */
  SlimBusLinkNodeType FreeEventStack;
  
  /* The base client context data */
  SlimBusBasicClientCtxt BasicClient;
  
  /* Array of pointers to shared channels allocatable for this client */
  SlimBusChannelType **paSharedChannels;

  /* Handle to the client event */
  SlimBusEventHandle hClientEvent;
  /* Client event object data */
  SlimBusEventObj ClientEventObj;
  /* Client synchronization handle */
  SlimBusSyncHandle hSync;
  
  /* The updated (pending) number of message slots reserved for this client */
  uint32 uNewReservedMsgSlots;
  /* The current (active) number of message slots reserved for this client */
  uint32 uCurReservedMsgSlots;

  /* Does the client currently have a reconfiguration sequence pending? */
  SBBOOL bReConfigPending;
  
  /* The number of pending NEXT_* reconfiguration requests that would be
     activated at the next DoReconfigureNow */
  uint32 uPendingReconfigReqs;
  
  /* The TID of any pending satellite reconfiguration request */
  uint32 uSatReconfigTID;
  
  /* The last asynchronous error code */
  SlimBusErrorType eLastMsgErr;
  
  /* The reconfiguration error code */
  SlimBusErrorType eLastReconfigErr;

  /* windows extension */
  SBBOOL bForceActive;
};

/* Global variable to allow interactive override of log level */
extern int32 g_iSlimBusLogLevelOverride;

/**
 * @brief Perform target-specific initialization for Slimbus
 *
 * This function performs target-specific initialization for 
 * Slimbus 
 * 
 * @param pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS
 */
extern SBResult SlimBus_DeviceInitTarget(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Enable interrupts
 *
 * This function enables interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
extern SBResult SlimBus_DeviceEnableInterrupts(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Disable interrupts
 *
 * This function disables interrupts
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on sucess, error code on error 
 */
extern SBResult SlimBus_DeviceDisableInterrupts(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Initialize the device
 *
 * This function initializes the device
 * 
 * @param[in] h  Client handle to the device
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
extern SBResult SlimBus_DeviceInitHardware(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Create a new Slimbus native device data structure
 *
 * This function creates a new Slimbus native device data 
 * structure. 
 * 
 * @return Pointer to the data structure created
 */
extern SlimBusDeviceType* SlimBus_NewDeviceStruct( void );

/**
 * @brief Allocate space the next available message in a message 
 *        FIFO
 *
 * This function allocates space for the next available message 
 * in a message FIFO 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pDevice  Pointer to the Slimbus native device
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * @param[in] uNumBytes  Number of bytes in the message to 
 *       allocate
 * 
 * @return Pointer to the space allocated for the message 
 */
extern uint32 *SlimBus_GetMsgBuffer
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDeviceType *pDevice,
  SlimBusMsgFifoType *pMsgFifo,
  uint32 uNumBytes
);

/**
 * @brief Stub for "sending" the next message in a message FIFO
 *
 * This function is a stub to handle "sending" the next message 
 * in a non-Tx message FIFO, no action is performed for 
 * "sending". 
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus device
 * @param[in] pMsgFifo  Pointer to the message FIFO structure 
 */
extern void SlimBus_DefaultSendBuffer
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusMsgFifoType *pMsgFifo
);

/**
 * @brief Setup and configure message queues
 *
 * This function sets up and configures message queues, if 
 * message queue functionality is enabled. 
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus device
 * 
 * @return SB_SUCCESS if setup was successful, error code 
 *         otherwise
 */
_IRQL_requires_(DISPATCH_LEVEL)
extern SBResult SlimBus_SetupMsgQueues(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Enqueues a message event for a Tx message needed 
 *        completion notification
 *
 * This function enqueues a message event for a Tx message the 
 * needs completion notification. The calling function should 
 * verify that there is an available event structure before 
 * calling this function. 
 * 
 * @param pMsgFifo[in]  Pointer to the message FIFO
 * @param pClientCtxt[in]  Pointer to the client context
 * @param pBuf[in]  Pointer to the message buffer needing 
 *            acknowledgement
 * @param hEvent[in]  Event handle that will receive 
 *              notification
 * 
 * @return  Pointer to the event structure enqueued
 */
extern SlimBusEventNodeType *SlimBus_EnqueueMsgEvent
(
  SlimBusMsgFifoType *pMsgFifo,
  SlimBusClientCtxt *pClientCtxt,
  uint32 *pBuf,
  SlimBusEventHandle hEvent
);

/**
 * @brief Peek the oldest message from a message FIFO
 *
 * This function peeks the oldest message (from the tail) of a 
 * message FIFO. 
 * 
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * @param[out] puNumWords  The number of words in the message 
 * 
 * @return  Pointer to the peeked message 
 */
extern uint32* SlimBus_PeekBuffer
(
  SlimBusMsgFifoType *pMsgFifo,
  uint32 *puNumWords
);

/**
 * @brief Initialize the device
 *
 * This function initializes the device
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus device
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
extern SBResult
SlimBus_DeviceInitHardware(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Pop the oldest message from a message FIFO
 *
 * This function pops the oldest message (from the tail) of a 
 * message FIFO. 
 * 
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * @param[out] puNumWords  The number of words in the message 
 *                    and the number of words popped from the
 *                    FIFO
 * 
 * @return  Pointer to the popped message 
 */
SLIMBUS_INLINE uint32* SlimBus_PopBuffer
(
  SlimBusMsgFifoType *pMsgFifo,
  uint32 *puNumWords
)
{
  uint32 uNumWords = 0;
  uint32 *pBuf = SlimBus_PeekBuffer(pMsgFifo, &uNumWords);

  if ( uNumWords > 0 )
  {
    pMsgFifo->uTailIdx += uNumWords;
  }
  
  if (NULL != puNumWords)
  {
    *puNumWords = uNumWords;
  }
  
  return pBuf;
}

/**
 * @brief Normalize index between min and max values
 *
 * This function normalizes and index value within a range of 
 * minimum and maximum values 
 * 
 * @param[in,out] puIdx  Pointer to the index to normalize
 * @param[in] uMinIdx  Minimum value for index
 * @param[in] uMaxIdx  Maximum value for index
 * @param[in] uLen  Power of 2 size of buffer associated with 
 *       the index
 */
SLIMBUS_INLINE void SlimBus_NormalizeIndexMinMax
(
  uint32 *puIdx,
  uint32 uMinIdx,
  uint32 uMaxIdx,
  uint32 uLen
)
{
  if (NULL != puIdx)
  {
    uint32 uIdx = *puIdx;
    uint32 uLenMask = uLen-1;
    
    /* Take an initial guess at the correct normalized range for the index value */
    uIdx = (uMinIdx&~uLenMask) | (uIdx&uLenMask);
    
    if (uIdx < uMinIdx)
    {
      /* Try to make index greater than or equal to the minimum */
      uIdx += uLen;
    }
    if (uIdx > uMaxIdx)
    {
      /* But prevent the index from becoming greater than the max */
      uIdx -= uLen;
    }
    if (uIdx > uMaxIdx)
    {
      /* For sanity, in case previous subtraction involved wrap-around */
      uIdx = uMinIdx;
    }
    
    *puIdx = uIdx;
  }
}

/**
 * @brief Get the next Transaction ID (TID) to use for 
 *        user-defined messages sent to the master Slimbus
 *        driver.
 *
 * This functions gets the next driver Transaction ID (TID) to 
 * use for user-defined messages sent to the master Slimbus 
 * driver. 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return  The next TID to use
 */
SLIMBUS_INLINE uint8 SlimBus_GetNextMasterTID(SlimBusDevCtxt *pDevCtxt)
{
  if (SB_MASTER_DRIVER == pDevCtxt->eDrvType)
  {
    return 0;
  }
  else
  {
    SlimBusDeviceType *pDevice = SB_GET_MGR_DEV_PTR(pDevCtxt);
    do {
      pDevice->uNextTIDIdx = (pDevice->uNextTIDIdx + 1) % SB_MAX_TIDS;
    } while (pDevice->TIDMap[pDevice->uNextTIDIdx] != NULL);
    return (uint8) (pDevice->uBaseTID + pDevice->uNextTIDIdx);
  }
}


/**
 * @brief Process a list of events that need signalling
 *
 * This function processes a list of events that need to be 
 * signalled. 
 * 
 * @param pDevCtxt[in]  Pointer to the device context
 * @param pSignalList[in]  Pointer to the list of events that
 *                   need to be signalled
 */
extern void SlimBus_ProcessSignalList
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusLinkNodeType *pSignalList
);

/**
 * @brief Process a received message
 *
 * Process a received message 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] length  Length of the message
 * @param[in] pDataBuf  Pointer to the first word in the 
 *       received message buffer
 * @param[in] pSignalList  Pointer to the list of events that 
 *       need to be signalled
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
extern SBResult SlimBus_ProcessRxMsg
(
  SlimBusDevCtxt *pDevCtxt,
  uint32 *pDataBuf,
  SlimBusLinkNodeType *pSignalList
);

/**
 * @brief Submit descriptors to the Rx message queue FIFO
 *
 * This function submits descriptors to the Rx message queue 
 * FIFO, until a descriptor has been submitted for all the bytes
 * in the message FIFO, or the descriptor FIFO is full.
 * 
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * 
 * @return  Number of descriptors submitted to the FIFO
 */
extern uint32 SlimBus_SubmitRxDescriptors( SlimBusMsgFifoType *pMsgFifo );

/**
 * @brief Open the device for the client
 *
 * This function opens the device for the client and initializes
 * the client data structures 
 * 
 * @param[in] h  Client device handle
 * @param[in] mode  Mode used to open the device
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
extern SBResult
SlimBus_DeviceOpen(SlimBusHandle h, uint32 mode);

/**
 * @brief Close the device for the client
 *
 * This function closes the device for the client 
 * 
 * @param[in] h  Client device handle
 * 
 * @return SB_SUCCESS 
 */
extern SBResult
SlimBus_DeviceClose(SlimBusHandle h);

/**
 * @brief Detach from the device context
 *
 * This function detaches the client from the device context
 * 
 * @param[in] h Client handle to the device context
 * 
 * @return Reference count for the device context
 */
extern uint32 SlimBus_DeviceDetach(SlimBusHandle h);

/**
 * @brief Retrieve the logical address corresponding to a 
 *        Slimbus native device enumeration address
 *
 * This function retrieves the logical address corresponding to 
 * a Slimbus native device enumeration address.  This function 
 * is used by external clients and performs mutex locking. 
 * 
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] pEA  Pointer to buffer containing device 
        enumeration address to query
  @param[in] uEASize Size of device enumeration address buffer 
  @param[out] pLA  Pointer to location to store of the logical 
        address of the device
   
  @return  SB_SUCCESS on success, an error code on error
 */
extern SBResult
SlimBus_GetDeviceLogicalAddr(SlimBusHandle h, const uint8 *pEA, uint32 uEASize, SlimBusLogicalAddrType * pLA);

/**
  @brief Request a particular SLIMbus root frequency
   
  This function requests a particular SLIMbus root frequency. 
  The frequency request will only be honored if there are no 
  data channels active on the bus.

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] uFreqKhz  Requested root frequency in kilohertz
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_RequestRootFreq(SlimBusHandle h, uint32  uFreqKhz);

/**
  @brief Get the last SLIMbus driver error.
   
  This function retrieves the last SLIMbus driver errror that 
  occurred.

  @param[in] h  Client handle to the SLIMbus driver 
  @param[out] peError  Pointer to location to store the last 
        SLIMbus driver error
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult 
SlimBus_GetLastError(SlimBusHandle h, SlimBusErrorType * peError);

/**
  @brief Get the SLIMbus hardware capabilities.
 
  This function gets the SLIMbus hardware capabilities and 
  optional functionalities. 
  
  @param[in] _h  Client handle to the SLIMbus driver
   
  @param[out] peHwCap  Pointer to location to store the 
        enumeration of hardware capabilities
  
  @return SB_SUCCESS on success, error code on error
 */
extern SBResult
SlimBus_GetHardwareCapability(SlimBusHandle h, SlimBusHwCapType *peHwCap);

/**
  @brief Deallocate ports from the SLIMbus master
   
  This function deallocates ports from the SLIMbus master. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] phPortBuf  Location of the handles to the ports to 
        deallocate
  @param[in] uPortBufSize  Size of the buffer of port handles 
        in bytes, used to derive the number of port handles to
        deallocate

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_DeallocMasterPorts(SlimBusHandle h, SlimBusResourceHandle * phPortBuf, uint32  uPortBufSize);

/**
  @brief Allocate port from the SLIMbus master 
   
  This function allocates one or more ports from the SLIMbus 
  master. 
   
  Ports allocated by this function correspond to logical ports 
  as defined by the SLIMbus spec, e.g. a half-duplex port is 
  considered a single logical port even though the port may 
  consume additional hardware resources within the SLIMbus 
  master. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] eReqs  Bitmap of requirements for the type of
        port(s) to allocate
  @param[in] uMinFifoBytes  Minimum required size of port FIFO 
        in bytes
  @param[out] phPortBuf  Location to store the handles to the 
        allocated ports
  @param[in] uPortBufSize  Size of the buffer of port handles in
        bytes, used to derive the number of port handles to
        allocate

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_AllocMasterPorts(SlimBusHandle h, SlimBusPortReqType  eReqs, uint32  uMinFifoBytes, SlimBusResourceHandle * phPortBuf, uint32  uPortBufSize);

/**
  @brief Get handle to a port on a slave device
   
  This function retrieves a handle to a port on a slave device
  that can be used for setting up a data channel.
   
  @param[in] h  Client handle to the SLIMbus driver
  @param[in] LA  Logical address of slave device containing the port
  @param[in] uPortNum  Port number on the slave device
  @param[out] phPort  Location to store the port handle

  @return  SB_SUCCESS on success, an error code on error
   
  */
extern SBResult
SlimBus_GetSlavePortHandle(SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uPortNum, SlimBusResourceHandle * phPort);

/**
  @brief Configure the parameters of a SLIMbus master port
   
  This function configures the parameters of a SLIMbus master 
  port.  SLIMbus slave ports cannot be configured by this 
  function. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Client handle to the master port 
  @param[in] pConfig  Pointer to the configuration to set for 
        the master port

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_ConfigMasterPort(SlimBusHandle h, SlimBusResourceHandle  hPort, const SlimBusPortConfigType * pConfig);

/**
  @brief Sets the messaging bandwidth required by the client
  
  Sets the total messaging bandwidth required by the client for 
  control and status operations.  Set the required bandwidth to 
  zero to relinquish reserved bandwidth. The bandwidth request 
  will take effect at the next reconfiguration boundary. 
  
  @param[in] h  Client handle to the SLIMbus driver
  @param[in] uBandwidth_bps  The bandwidth to reserve in bits per second

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_NextReserveMsgBandwidth(SlimBusHandle h, uint32  uBandwidth_bps);

/**
  @brief Read value elements from a slave device.
  
  This function reads value element(s) from a slave device using
  byte-based access.  This function will perform an asynchronous
  read if the hEvent parameter passed is not NULL, otherwise a 
  synchronous read will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to read values 
        elements from
  @param[in] uByteAddr  The byte address of the first byte to 
        read.
  @param[out] pucReadData  Pointer to the buffer to store the 
        data read from the value elements
  @param[in] uReadDataLen  Number of value element bytes to read 
        and the size of the read data buffer; allowed values for
        uReadDataLen are 1, 2, 3, 4, 6, 8, 12 and 16
  @param[in] puActualReadDataLen  Optional pointer to location 
        to store the number of value element bytes actually read
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
   
  */
extern SBResult
SlimBus_ReadValueElement(SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr,  uint8*  pucReadData,  uint32  uReadDataLen,  uint32*  puActualReadDataLen, SlimBusEventHandle  hEvent);

/**
  @brief Write value elements to a slave device.
  
  This function writes value element(s) to a slave device using 
  byte-based access.  This function will perform a synchronous 
  write if the hEvent parameter is equal to NULL, a non-blocking
  write if the hEvent parameter is equal to 
  SLIMBUS_EVENT_HANDLE_NON_BLOCKING, or otherwise an 
  asynchronous write will be performed.
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to write values 
        elements to
  @param[in] uByteAddr  The byte address of the first byte to 
        write.
  @param[in] uByteSize  The number of value element bytes to 
        write; allowed values for uByteSize are 1, 2, 3, 4, 6,
        8, 12 and 16
  @param[in] pucWriteData  Optional pointer to the buffer 
        containing the data to write to the value elements; if
        NULL is passed for pucWriteData then 0 must be passed
        for uWriteDataLen
  @param[in] uWriteDataLen  Size of the write data buffer; if 
        the write buffer length is less than the number of bytes
        to write, the destination device will pad the end of the
        write data with zeros; if the write data buffer length
        is larger than the number of bytes to write, the extra
        bytes at the end will be ignored
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_WriteValueElement(SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr, uint32  uByteSize, const uint8*  pucWriteData,  uint32  uWriteDataLen, SlimBusEventHandle  hEvent);

/**
  @brief Perform atomic exchange of value elements on a slave 
         device.
  
  This function performs an atomic exchange of value element(s) 
  on a slave device  This function will perform an asynchronous 
  exchange if the hEvent parameter passed is not NULL, otherwise 
  a synchronous exchange will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to exchange 
        values elements on
  @param[in] uByteAddr  The byte address of the first byte to 
        read.
  @param[in] pucWriteData  Optional pointer to the buffer 
        containing the data to write to the value elements; if
        NULL is passed for pucWriteData then 0 must be passed
        for uWriteDataLen
  @param[in] uWriteDataLen  Size of the write data buffer; if 
        the write buffer length is less than the number of bytes
        to read, the destination device will pad the end of
        the write data with zeros; if the write data buffer
        length is larger than the number of bytes to read, the
        extra bytes at the end will be ignored
  @param[out] pucReadData  Pointer to the buffer to store the 
        data read from the value elements
  @param[in] uReadDataLen  Number of value element bytes to 
        write and read, and the size of the read data buffer;
        allowed values for uReadDataLen are 1, 2, 3, 4, 6, 8, 12
        and 16
  @param[in] puActualReadDataLen  Optional pointer to location 
        to store the number of value element bytes actually read
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_ExchangeValueElement(SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr,  const uint8*  pucWriteData,  uint32  uWriteDataLen,  uint8*  pucReadData,  uint32  uReadDataLen,  uint32*  puActualReadDataLen, SlimBusEventHandle  hEvent);

/**
  @brief Read information elements from a slave device.
  
  This function reads information element(s) from a slave device 
  using byte-based access.  This function will perform an 
  asynchronous read if the hEvent parameter passed is not NULL, 
  otherwise a synchronous read will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError().
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to read 
        information elements from
  @param[in] uByteAddr  The byte address of the first byte to 
        read.
  @param[out] pucReadData  Pointer to the buffer to store the 
        data read from the information elements
  @param[in] uReadDataLen  Number of information element bytes 
        to read and the size of the read data buffer; allowed
        values for uReadDataLen are 1, 2, 3, 4, 6, 8, 12 and 16
  @param[in] puActualReadDataLen  Optional pointer to location 
        to store number of information element bytes actually
        read
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_ReadInfoElement(SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr,  uint8*  pucReadData,  uint32  uReadDataLen,  uint32*  puActualReadDataLen, SlimBusEventHandle  hEvent);

/**
  @brief Clear information elements on a slave device.
  
  This function clears information element(s) on a slave device 
  using byte-based access. This function will perform a synchronous 
  clear if the hEvent parameter is equal to NULL, a non-blocking
  clear if the hEvent parameter is equal to 
  SLIMBUS_EVENT_HANDLE_NON_BLOCKING, or otherwise an 
  asynchronous clear will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to clear 
        information elements on
  @param[in] uByteAddr  The byte address of the first byte to 
        clear
  @param[in] uByteSize  Number of information element bytes to 
        clear; allowed values for uByteSize are 1, 2, 3, 4, 6,
        8, 12 and 16
  @param[in] pucClearMask  Optional pointer to the buffer 
        containing the mask to use for clearing information
        elements, where every bit set to '1' in the mask
        indicates that the corresponding information element bit
        should be cleared; if NULL is passed for pucClearMask
        then 0 must be passed for uClearMaskLen
  @param[in] uClearMaskLen  Size of the clear mask buffer; if 
        the clear mask length is less than the number of bytes
        to clear, the destination device will pad the end of the
        clear mask with ones; if the clear mask length is larger
        than the number of bytes to clear, the extra bytes at
        the end will be ignored
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_ClearInfoElement(SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr, uint32  uByteSize, const uint8*  pucClearMask,  uint32  uClearMaskLen, SlimBusEventHandle  hEvent);

/**
  @brief Perform atomic read and clear of information elements 
         on a slave device.
  
  This function performs an atomic read and clear of information
  element(s) on a slave device using byte-based access.  This 
  function will perform an asynchronous operation if the hEvent 
  parameter passed is not NULL, otherwise a synchronous 
  operation will be performed. 
   
  For an asynchronous transaction, the hEvent will be signalled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] LA  Logical address of the device to read and clear
        information elements on
  @param[in] uByteAddr  The byte address of the first byte to 
        read and clear
  @param[in] pucClearMask  Optional pointer to the buffer 
        containing the mask to use for clearing information
        elements, where every bit set to '1' in the mask
        indicates that the corresponding information element bit
        should be cleared; if NULL is passed for pucClearMask
        then 0 must be passed for uClearMaskLen
  @param[in] uClearMaskLen  Size of the clear mask buffer; if 
        the clear mask length is less than the number of bytes
        to clear, the destination device will pad the end of the
        clear mask with ones; if the clear mask length is larger
        than the number of bytes to clear, the extra bytes at
        the end will be ignored
  @param[out] pucReadData  Pointer to the buffer to store the 
        data read from the information elements
  @param[in] uReadDataLen  Number of information element bytes 
        to read and clear, and the size of the read data buffer;
        allowed values for uReadDataLen are 1, 2, 3, 4, 6, 8, 12
        and 16
  @param[out] puActualReadDataLen  Optional pointer to location 
        to store number of information element bytes actually
        read
  @param[in] hEvent  Event handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_ReadAndClearInfoElement(SlimBusHandle h, SlimBusLogicalAddrType  LA, uint32  uByteAddr,  const uint8*  pucClearMask,  uint32  uClearMaskLen, uint8*  pucReadData,  uint32  uReadDataLen,  uint32*  puActualReadDataLen, SlimBusEventHandle  hEvent);

/**
  @brief Send a destination-referred user-defined message to a 
         slave device.
 
  This function sends a destination-referred user-defined 
  message to a slave device.  This function will perform an 
  asynchronous operation if the hEvent parameter is not NULL, 
  otherwise a synchronous operation is performed and the 
  function will return when the message transmission is 
  complete. 
  
  For an asynchronous transaction, the hEvent will be signaled 
  when the transaction is complete, and the error status can be 
  retrieved by SlimBus_GetLastError(). 
  
  @param[in] _h  Client handle to the SLIMbus driver
  
  @param[in] LA  Logical address of the device to send the
        user-defined message to
  
  @param[in] MC  Message code of the user-defined message
  
  @param[in] pucPayload  Pointer to the payload data to send
  
  @param[in] uPayloadLen  Size of the payload data in bytes
  
  @param[in] hEvent  Event handle
  
  @return SB_SUCCESS on success, an error code on error 
   
  @see SlimBus_GetLastError()
 */
extern SBResult
SlimBus_SendUserDefinedMessage(SlimBusHandle h, SlimBusLogicalAddrType LA, uint32 MC, const uint8* pucPayload, uint32 uPayloadLen, SlimBusEventHandle hEvent);

/**
  @brief Allocate SLIMbus data channel
   
  This function allocates a SLIMbus data channel.

  @param[in] h  Client handle to the SLIMbus driver 
  @param[out] phChannel  Location to store the allocated data 
        channel handle

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_AllocDataChannel(SlimBusHandle h, SlimBusResourceHandle * phChannel);

/**
  @brief Allocate shared SLIMbus data channel
   
  This function allocates a SLIMbus data channel that can be 
  shared and utilized from more than one execution environment. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] uChannelNum  Shared channel number
  @param[out] phChannel  Location to store the allocated data 
        channel handle

  @return  SB_SUCCESS on success, an error code on error
   
  @see SlimBus_DeallocDataChannel(), SlimBus_AllocDataChannel() 
  */
extern SBResult
SlimBus_AllocSharedDataChannel(SlimBusHandle h, uint32 uChannelNum, SlimBusResourceHandle * phChannel);

/**
  @brief Set or change the requested parameters of a data 
         channel
   
  This function sets or changes the requested parameters of a 
  data channel.  The new parameters take effect at the next 
  reconfiguration boundary. 
   
  The event handle passed as a parameter will be signalled if 
  the requested data channel parameters are rejected due to lack 
  of bandwidth.  After requesting data channel parameters, the 
  data channel status returned by SlimBus_GetDataChannelStatus() 
  will be updated once the data channel is accepted. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle or resource group 
        handle
  @param[in] pChannelReq  Pointer to the requested data channel 
        parameters
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_NextDefineDataChannel(SlimBusHandle h, SlimBusResourceHandle  hChannel, const SlimBusChannelReqType * pChannelReq);

/**
  @brief Connect a port to a data channel
   
  This function connects the specified port to the data channel. 
  The connection will take effect immediately. 
   
  For master ports, if the BAM pipe has already been configured, 
  the port FIFO will begin to fill as soon as the port is 
  connected. 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle 
  @param[in] eFlow  The type of port connection to make, source 
        or sink
  @param[in] hPort  Port handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_ConnectPortToChannel(SlimBusHandle h, SlimBusResourceHandle  hChannel, SlimBusPortFlowType  eFlow, SlimBusResourceHandle  hPort);

/**
  @brief Disconnect a port from a data channel
   
  This function disconnects the specified port from the data 
  channel. The disconnection will take effect immediately. 
   
  For master ports, data flow will stop and the port and BAM 
  pipe will be reset when the port is disconnected.  It is not 
  necessary to explicitly disconnect ports when removing a data 
  channel as described under SlimBus_NextDataChannelControl(). 

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Port handle
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_DisconnectPortFromChannel(SlimBusHandle h, SlimBusResourceHandle  hPort);

/**
  @brief Perform data channel control operation
   
  This function performs a data channel control operation, to 
  either activate, deactivate or remove the data channel.  The 
  control operation will take effect at the next reconfiguration 
  boundary. 
   
  For the remove operation, all ports connected to the data 
  channel will be disconnected and reset at the next 
  reconfiguration boundary, and the BAM pipes corresponding to 
  master ports connected to the data channel will be reset at 
  the next reconfiguration boundary.  For the remove operation 
  it is not necessary to explicitly disconnect ports connected 
  to the channel using SlimBus_DisconnectPortFromChannel().

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle or group handle 
  @param[in] eCtrl  The type of channel control operation 
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_NextDataChannelControl(SlimBusHandle h, SlimBusResourceHandle  hChannel, SlimBusChannelCtrlType  eCtrl);

/**
  @brief Perform reconfigure now operation
   
  This function performs a reconfigure now operation.  All 
  operations queued by Next*() API functions will take effect. 
  
  @param[in] h  Client handle to the SLIMbus driver 
  
  @return  SB_SUCCESS if the reconfiguration sequence was 
           accepted, an error code on error or if the
           reconfiguration sequence was rejected
  */
extern SBResult
SlimBus_DoReconfigureNow(SlimBusHandle h);

/**
  @brief Gets the status of a SLIMbus data channel
   
  This function retrives the status of a SLIMbus data 
  channel.
  
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle 
  @param[in] peStatus  Location to store the data channel status
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_GetDataChannelStatus(SlimBusHandle h, SlimBusResourceHandle  hChannel, SlimBusDataChannelStatusType * peStatus);

/**
  @brief Deallocate SLIMbus data channel

  This function deallocates a SLIMbus data channel.  A data 
  channel that has not yet been removed will be removed as 
  described by SlimBus_NextDataChannelControl().

  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hChannel  Data channel handle 
  
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_DeallocDataChannel(SlimBusHandle h, SlimBusResourceHandle  hChannel);

/**
  @brief Get the FIFO status of a SLIMbus master port 
   
  This function retrieves the FIFO status of a SLIMbus master 
  port.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle
  @param[in] eTransferDir  Direction of data flow for FIFO to 
        get the status for.  For bi-directional ports, there is
        one FIFO for each of the transmit and receive
        directions.
  @param[out] puFifoWordCnt  Pointer to the location to store 
        the FIFO word count
  @param[out] pbWatermarkIsHit  Pointer to the location to store
        a boolean value indicating whether the watermark has
        been hit or not

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_GetPortFifoStatus(SlimBusHandle h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, uint32 * puFifoWordCnt, SBBOOL * pbWatermarkIsHit);

/**
  @brief Register for specific asynchronous SLIMbus master port 
         events
   
  This function registers for specific asynchronous SLIMbus 
  mater port events.  When the event object is signalled, the 
  condition that triggered the event will be available for 
  retrieval by the SlimBus_GetPortEvent() function.  Calls to 
  this function for a particular port will supercede any 
  previous calls to this function or to 
  SlimBus_RegisterPortEvent() for the port. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle
  @param[in] hEvent  Event handle to receive asynchronous port 
        event notifications
  @param[in] pUser  User pointer that will be returned in event 
        notification structure
  @param[in] eType  Type of port events to register for 

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult 
SlimBus_RegisterPortEventEx
(
  SlimBusHandle h,
  SlimBusResourceHandle hPort,
  SlimBusEventHandle hEvent,
  void * pUser,
  SlimBusPortEventType eType
);

/**
  @brief Read information on recent port events
   
  This function reads information on recent asynchronous port 
  events.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle; this parameter can be 
        set to NULL to get the pending events for any port owned
        by the client
  @param[out] pNotify  Pointer to event notification structure 

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult 
SlimBus_GetPortEvent
(
  SlimBusHandle h,
  SlimBusResourceHandle  hPort,
  SlimBusEventNotifyType * pNotify
);

/**
  @brief Allocate a port progress counter.
   
  This function allocates a progress counter for a SLIMbus 
  master port.  Progress counters are typically used for audio 
  sync applications. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hPort  Master port handle that the progress counter 
        should be associated with
  @param[in] eTransferDir  Direction of data flow for the BAM 
        pipe to allocate the progress counter for.  For
        bi-directional ports, there is one BAM pipe for each of
        the transmit and receive directions.
  @param[out] phCounter  Pointer to location to store allocated
        progress counter handle

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_AllocProgressCounter(SlimBusHandle h, SlimBusResourceHandle  hPort, SlimBusBamTransferType  eTransferDir, SlimBusResourceHandle * phCounter);

/**
  @brief Deallocate a port progress counter.
   
  This function deallocates a progress counter for a SLIMbus 
  master port.  Progress counters are typically used for audio 
  sync applications. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle

  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_DeallocProgressCounter(SlimBusHandle h, SlimBusResourceHandle  hCounter);

/**
  @brief Initialize the value of a port progress counter.
   
  This function initializes the value of a SLIMbus master port 
  progress counter.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle
  @param[in] uCount  Value to initialize the progress counter 
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult 
SlimBus_InitProgressCounter(SlimBusHandle h, SlimBusResourceHandle  hCounter, uint32  uCount);

/**
  @brief Read the value of a port progress counter. 
   
  This function reads the value of a SLIMbus master port 
  progress counter.  Either the free running counter, VFR
  counter ro shadow counter can be read. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle 
  @param[in] eType  The type of counter to read, either the free 
        running, VFR or shadow counter
  @param[out] puNumDMA  Pointer to store the number of 32-bit 
        words DMA-ed to or from the associated port
  @param[out] puFifoSamples  Number of samples currently in the 
        port FIFO plus and samples in the phy/FL stages,
        taking into account segment length and bit packing
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_ReadProgressCounter(SlimBusHandle h, SlimBusResourceHandle  hCounter, SlimBusCounterType  eType, uint32 * puNumDMA, uint32 * puFifoSamples);

/**
  @brief Read the value of a port shadow progress counter and 
         timestamp.
 
  This function reads the value of a SLIMbus master port shadow 
  progress counter and timestamp.
  
  @param[in] _h  Client handle to the SLIMbus driver
   
  @param[in] hCounter  Progress Counter handle
   
  @param[out] puNumDMA  Pointer to store the number of 32-bit 
        words DMA-ed to or from the associated port at the last
        DMA interrupt
   
  @param[out] puFifoSamples  Number of samples that were in the 
        port FIFO plus samples in the phy/FL stages, taking
        into account segment length and bit packing, at the
        last DMA interrupt
   
  @param[out] puTimestamp  Pointer to store the timestamp value
        for the last DMA interrupt
   
  @param[out] pbSampleMissed  Pointer to store indication of 
        whether and DMA timestamp values have been missed
  
  @return SB_SUCCESS if the timestamp sample and counter values
          are valid, error code otherwise
   
  @see SlimBus_ReadProgressCounter() 
 */
extern SBResult
SlimBus_ReadProgressCounterTimestamp(SlimBusHandle h, SlimBusResourceHandle hCounter, uint32 *puNumDMA, uint32 *puFifoSamples, uint64 *puTimestamp, SBBOOL *pbSampleMissed);

/**
  @brief Read the status of whether a progress counter VFR 
         interrupt pulse has occurred.
   
  This function reads the status of whether a progress counter 
  VFR interrupt pulse has occurred.  The status is cleared by 
  the SlimBus_ClearProgressCounterVFRStatus() function.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle 
  @param[out] pbSet  Pointer to store boolean indicating whether
        VFR interrupt pulse has occurred
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_ReadProgressCounterVFRStatus(SlimBusHandle h, SlimBusResourceHandle  hCounter, SBBOOL * pbSet);

/**
  @brief Clear the progress counter VFR interrupt pulse status. 
   
  This function clears the progress counter VFR interrupt pulse 
  status.
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hCounter  Progress Counter handle 
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_ClearProgressCounterVFRStatus(SlimBusHandle h, SlimBusResourceHandle  hCounter);

/**
  @brief Allocate a resource group.
   
  This function allocates a resource group used to manipulate
  multiple resources at the same time. 
   
  Resource groups are currently supported by the following 
  functions: SlimBus_NextDefineDataChannel(), 
  SlimBus_NextDataChannelControl() 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[out] phGroup  Pointer to location to store allocated 
        resource group handle
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_AllocResourceGroup(SlimBusHandle h, SlimBusResourceHandle * phGroup);

/**
  @brief Deallocate a resource group.
   
  This function deallocates a resource group used to manipulate
  multiple resources at the same time. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hGroup  resource group handle
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_DeallocResourceGroup(SlimBusHandle h, SlimBusResourceHandle  hGroup);

/**
  @brief Add a resource to a resource group.
   
  This function adds a resource to a resource group.  All 
  resources within a group must be of the same type.  A resource 
  is limited to belonging to a single group at a time.
   
  Currently only data channel resources can
  be added to a resource group. 
   
  @param[in] h  Client handle to the SLIMbus driver 
  @param[in] hGroup  resource group handle 
  @param[in] hResource  resource handle to add to the group 
   
  @return  SB_SUCCESS on success, an error code on error
  */
extern SBResult
SlimBus_AddResourceToGroup(SlimBusHandle h, SlimBusResourceHandle  hGroup, SlimBusResourceHandle  hResource);

/**
 * @brief Initialize the device
 *
 * This function initializes the device
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
extern SBResult
SlimBus_DeviceInitHardware(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Initialize the device
 *
 * This function initializes the device
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return SB_SUCCESS on success, error code on error 
 */
extern SBResult SlimBus_DeviceInitInternal(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief SLIMbus driver common interrupt service routine
 *
 * SLIMbus driver common interrupt service routine
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pIsrCtxt  Pointer to the interrupt context 
 * 
 * @return Pointer to NULL 
 */
extern SBResult SlimBusCommonIsr(SlimBusDevCtxt *pDevCtxt, struct SlimBusIsrCtxt *pIsrCtxt);

/**
 * @brief SLIMbus driver common deferred procedure call
 *
 * SLIMbus driver common deferred procedure call
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pIsrCtxt  Pointer to the interrupt context 
 * 
 * @return SB_SUCCESS on success, an error code on error 
 */
extern SBResult SlimBusCommonDpc(SlimBusDevCtxt *pDevCtxt, struct SlimBusIsrCtxt *pIsrCtxt);

/**
 * @brief Process messages received in an ISR context
 *
 * This function processes Rx messages received in an ISR 
 * context.  If a message is not able to be processed in ISR 
 * context, then SLIMBUS_ERR_RETRY will be returned. 
 * 
 * @param pDevCtxt[in]  Pointer to the device context
 * @param pSignalList[in]  Pointer to the root node to enqueue 
 *                   events that need to be signalled
 * 
 * @return  SB_SUCCESS on success, SLIMBUS_ERR_RETRY if the 
 *          messages could not be processed in ISR context, or
 *          an error code otherwise
 */
extern SBResult SlimBus_HandleIsrRxMsgs
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusLinkNodeType *pSignalList
);

/**
 * @brief Callback for the message timer
 *
 * This function implements the callback for the message timer
 * 
 * @param pDevCtxt  Pointer to the device context
 */
extern void SlimBusTimerCallback(SlimBusDevCtxt *pDevCtxt);
  
/**
 * @brief Handles a BAM reset timer event
 *
 * This function handles a BAM reset timer event
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 */
extern void SlimBusBamResetTimerCallback(SlimBusDevCtxt *pDevCtxt);

/**
 * @brief Allocate buffer space for Tx and Rx message queues
 *
 * This function allocates buffer space for the Tx and Rx 
 * message queues.  If BAM message queues are enabled, the 
 * buffer space is allocated from physical uncached memory. 
 * 
 * @param pDevCtxt[in]  Pointer to the device context
 * @param puOffset[out]  Location to store the offset into the 
 *                physical buffer of space unused by the message
 *                queues
 * 
 * @return SB_SUCCESS on success, error code otherwise
 */
extern SBResult SlimBus_AllocMsgBuffers(SlimBusDevCtxt *pDevCtxt, uint32 *puOffset);

/**
 * @brief Send the next (oldest) message in a Tx message FIFO 
 *        using the BAM
 *
 * This function sends the next (oldest) message in a Tx message 
 * FIFO, using the BAM. If the interface is busy (there is a BAM
 * descriptor in flight with the EOT flag set), no new message 
 * will be sent, instead messages would get sent later when the 
 * ISR detects that the descriptor with EOT has been sent has 
 * been processed. 
 * 
 * @param[in] pDevCtxt  Pointer to the SLIMbus device
 * @param[in] pMsgFifo  Pointer to the message FIFO structure
 */
extern void SlimBus_SendTxBamBuffer
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusMsgFifoType *pMsgFifo
);

/**
 * @brief Allocate space the next available message in a Tx 
 *        message FIFO
 *
 * This function allocates space for the next available message 
 * in a Tx message FIFO.  If the destination device is not ready 
 * to receive another non-posted transaction, this function will
 * block until another transaction ID is available. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pDevice  Pointer to the Slimbus native device
 * @param[in] pMsgFifo  Pointer to the message FIFO
 * @param[in] uNumBytes  Number of bytes in the message to 
 *       allocate
 * 
 * @return Pointer to the space allocated for the message 
 */
extern uint32 *SlimBus_GetTxMsgBuffer
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusDeviceType *pDevice,
  SlimBusMsgFifoType *pMsgFifo,
  uint32 uNumBytes
);

/**
 * @brief Process the disconnection of a port
 *
 * This function processes the disconnection of a port
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * @param[in] pPort  Pointer to the master port data structure
 * @param[out] peEvents  Pointer to the location to store 
 *                  detected port events
 */
extern void SlimBus_ProcessPortDisconnection
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusMasterPortType *pPort,
  SlimBusPortEventType *peEvents
);

/**
 * @brief Enable interrupt for a port
 *
 * This function enables the interrupt for a port, and as 
 * necessary, clears the interrupt disabled flag for the port 
 * and disables the reconfig_done interrupt enable. 
 * 
 * @param pDevCtxt  Pointer to the device context
 * @param uIntMask  Bitmask of ports to enable
 */
extern void EnablePortInterrupt(SlimBusDevCtxt *pDevCtxt, uint32 uIntMask);

/**
 * @brief Disable interrupt for a port
 *
 * This function disables the interrupt for a port, setting the 
 * interrupt disabled flag for the port, and enabling the 
 * reconfig_done interrupt if needed.
 * 
 * @param pDevCtxt  Pointer to the device context
 * @param uIntMask  Bitmask of ports to disable
 */
extern void DisablePortInterrupt(SlimBusDevCtxt *pDevCtxt, uint32 uIntMask);

/**
 * @brief Perform cleanup for a message event
 *
 * This function performs cleanup operations for a message event
 * 
 * @param[in] pDevice  Pointer to a SLIMbus native device 
 *       structure
 * @param[in] TID  Transaction ID
 * @param[in] pEventNode  Pointer to the event node
 * @param[in] pRootNode  Pointer to the list to return the event 
 *       node
 */
extern void SlimBus_CleanupMsgEvent
(
  SlimBusDeviceType *pDevice,
  uint32 TID,
  SlimBusEventNodeType *pEventNode,
  SlimBusLinkNodeType *pRootNode
);

/**
 * @brief This function will reset MsgFifo
 * 
 * @param[in] pMsgFifo pointer to message fifo.
 * 
 * @return None 
 */
extern void SlimBus_ResetMsgFifo(SlimBusMsgFifoType *pMsgFifo);

/**
 * @brief Perform work related to processing notification of a 
 *        received message
 *
 * This function performs work related to processing 
 * notification of a received message 
 * 
 * @param[in] pDevCtxt  Pointer to the SlimBus device
 * @param[in] pSignalList  Pointer to the signal list
 * 
 * @return SB_SUCCESS on success, error code on error
 */
extern SBResult SlimBus_MsgWorkerCallback
(
  SlimBusDevCtxt *pDevCtxt,
  SlimBusLinkNodeType *pSignalList
);

/**
 * @brief Wait for message transaction completion event
 *
 * This function performs functionality related to waiting for 
 * the completion event for a message transaction. 
 * 
 * @param[in] pClientCtxt  Pointer to the client context
 * @param[in] pEventNode  Pointer to the event node
 * 
 * @return SB_SUCCESS on success, error code on error
 */
_IRQL_requires_(DISPATCH_LEVEL)
extern SBResult SlimBus_WaitForMsgEvent
(
  SlimBusClientCtxt *pClientCtxt,
  SlimBusEventNodeType *pEventNode
);

/**
 * @brief Check if the common driver is ready to enter low-power 
 *        mode
 *
 * This function checks if the common driver is ready to enter 
 * low-power mode 
 * 
 * @param[in] pDevCtxt  Pointer to the device context
 * 
 * @return TRUE if ready to enter low-power mode, FALSE 
 *         otherwise
 */
extern SBBOOL SlimBus_ReadyForLowPowerMode(SlimBusDevCtxt *pDevCtxt);

#endif /* SLIMBUS_H */

