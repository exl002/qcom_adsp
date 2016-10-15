#ifndef ELITEMSG_H
#define ELITEMSG_H

/**
@file EliteMsg.h

@brief This file defines messages structures, IDs and payloads
 (that do not depend on secondary opcode) for ELite messages.

The Elite message structure used internally throughout Elite is
an 8-uint8_t structure that has been designed for optimal passing
of data buffers through the system, which is the prime
consideration for steady-state performance. The QDSP6 can
achieve an 8-uint8_t copy into and out of an queue in a single
cycle. The 8 bytes is sufficient space for an opcode followed by
a payload. This opcode is regarded as primary opcode.
Depending on the primary opcode, the payload typically is a
pointer to a message payload buffer, but can be an immediate
payload value as well.

The primary opcodes are globally defined. The recommended way to
process the 8-uint8_t Elite message is to use a function table with
each entry being the message handler corresponding to the
primary opcode. To reduced the function table size, the number
of primary opcode is tightly limited. Services can define more
specific messages and these service-specific messages are
further multiplexed through the payload of
ELITE_CUSTOM_MSG, which reserves space for a secondary
opcode to further distingush the content of messages. The
secondary opcode shall also be globally unique.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The descriptions that appera in the PDF are maintained in the
      Elite_Core_mainpage.dox file. Contact Tech Pubs for support.
===========================================================================*/

/*========================================================================
   Copyright (c) 2010, 2013 Qualcomm Technologies, Inc.  All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.2.
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
08/31/10   DG      Changed the nFlags field in elite_msg_data_buffer_t to
                   unsigned, since it is a bitmask. This removes warnings
                   when it is ORed with 0x80000000.
02/04/10   DC      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "EliteSvc.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/* These primary operation codes are the only valid values allowed in the
  unOpCode field of the elite_msg_any_t structure. They are tightly controlled
  in a centralized location to avoid potential reuse of the same operation 
  code. */

/** @addtogroup elite_msg_prim_opcodes
@{ */
#define    ELITE_CUSTOM_MSG            0x00000000L
    /**< Primary operation code for a service-customized message.
         The message payload is elite_msg_custom_header_t. */

#define    ELITE_CMD_START_SERVICE     0x00000001L
    /**< Primary operation code for a start service message. */
#define    ELITE_CMD_DESTROY_SERVICE   0x00000002L
    /**< Primary operation code for a destroy service message. */
#define    ELITE_CMD_CONNECT           0x00000003L
    /**< Primary operation code for a connect service message.
         This message commands one service to connect to another service.

         The message payload is elite_msg_param_cal_t. */

#define    ELITE_CMD_DISCONNECT        0x00000004L
    /**< Primary operation code for a disconnect service message.
         This message commands one service to disconnect from another service.

         The message payload is elite_msg_cmd_connect_t. */

#define    ELITE_CMD_PAUSE             0x00000005L
    /**< Primary operation code for a pause service message. */

#define    ELITE_CMD_RUN               0x00000006L
    /**< Primary operation code for a run service message.
         The message payload is elite_msg_cmd_run_t. */

#define    ELITE_CMD_FLUSH             0x00000007L
    /**< Primary operation code for a flush service message. @newpage */

#define    ELITE_CMD_SET_PARAM         0x00000008L
    /**< Primary operation code for a set-parameter-of-service message.
         The message payload is elite_msg_param_cal_t.
         - When the payload data is a uint32_t, i.e.,  ParamID =
           #ELITEMSG_PARAM_ID_THREAD_PRIO, elite_msg_param_any_t
         - Payload header fields are in elite_msg_param_header_t */

#define    ELITE_CMD_GET_PARAM         0x00000009L
    /**< Primary operation code for a get-parameter-of-service message. 
         The message payload is elite_msg_param_cal_t
         - When the payload data is a uint32_t, i.e.,  ParamID =
           #ELITEMSG_PARAM_ID_THREAD_PRIO, elite_msg_param_any_t
         - Payload header fields are in elite_msg_param_header_t */

#define    ELITE_DATA_BUFFER           0x0000000AL
    /**< Primary operation code for a data buffer message, which is used to
         exchange a buffer between services.
         The message payload is elite_msg_data_buffer_t. */

#define    ELITE_DATA_MEDIA_TYPE       0x0000000BL
    /**< Primary operation code for a media type message.

         The message payload is elite_msg_data_media_type_apr_t.
         Payload header fields are in elite_msg_data_media_type_header_t */

#define    ELITE_DATA_EOS              0x0000000CL
    /**< Primary operation code for an end-of-stream message.
         This message can be flushed.

         The message payload is elite_msg_data_eos_apr_t.
         Payload header fields are in elite_msg_data_eos_header_t */

#define    ELITE_DATA_RAW_BUFFER       0x0000000DL
    /**< Primary operation code for a raw-buffer message. */

#define    ELITE_CMD_STOP_SERVICE      0x0000000EL
    /**< Primary operation code for a stop service message. This is optional
         for audio. */

#define    ELITE_APR_PACKET            0x0000000FL
    /**< Primary operation code to indicate that the message payload points to
         an APR packet. @newpage */

#define    ELITE_MIPS_CHANGE_EVENT     0x00000010L
    /**< Primary operation code to indicate that the MIPS requirement has
         changed for a service. The actual MIPS value is indicated by using a
         shared variable.

          */

#define   ELITE_DATA_SET_PARAM         0x00000011L
    /**< Primary operation code to send any set parameters that are
         synchronized with the data. */

#define    ELITE_DML_CHANGE_EVENT     0x00000012L
    /**< Primary operation code to indicate that the DMLite requirement has changed
         for a service. */

#define    ELITE_BW_CHANGE_EVENT     0x00000013L
   /**< Primary operation code to indicate that the bandwidth requirement has
        changed for a service. The actual BW value is indicated by using a
        shared variable. */
#define    ELITE_DATA_MARK_BUFFER      0x00000014L
    /**< Primary operation code for an end-of-stream message.
         This message can be flushed.*/
/** @} */ /* end_addtogroup elite_msg_prim_opcodes */


/** @addtogroup elite_msg_constants_macros
@{ */
/** Macro that determines the actual required buffer size if nReqBufSize bytes
  is requested.
 */
#define GET_ELITEMSG_DATABUF_REQ_SIZE( nReqBufSize ) ( (nReqBufSize) + sizeof(elite_msg_data_buffer_t) - 4 )

/** Timestamp Valid bitmask for the nFlag parameter in the
    elite_msg_data_buffer_t structure.
*/
#define ELITE_DATA_BUFFER_TIME_STAMP_BITMASK       0x80000000L

/** Speed Factor bitmask for the nFlag parameter in the
    elite_msg_data_buffer_t structure.
*/
#define ELITE_DATA_BUFFER_SPEED_FACTOR_BITS        0x0000FFFFL

/** Macro that gets the timestamp valid bit from the nFlag parameter of the
    elite_msg_data_buffer_t structure.
 */
#define GET_DATA_BUFFER_TIME_STAMP_BIT( nFlag ) ( (nFlag) & (ELITE_DATA_BUFFER_TIME_STAMP_BITMASK) )

/** Macro that sets the timestamp valid bit in the nFlag parameter of the
    elite_msg_data_buffer_t structure.
 */
#define SET_DATA_BUFFER_TIME_STAMP_BIT( nFlag ) ( (nFlag) | (ELITE_DATA_BUFFER_TIME_STAMP_BITMASK) )

/** Macro that clears the timestamp valid bit in the nFlag parameter of the
    elite_msg_data_buffer_t structure.
 */
#define CLR_DATA_BUFFER_TIME_STAMP_BIT( nFlag ) ( (nFlag) & (~(ELITE_DATA_BUFFER_TIME_STAMP_BITMASK)) )

/** Macro that gets the actual buffer pointer from the audDataBuf parameter of
    the elite_msg_data_buffer_t structure.
 */
#define GET_ELITEMSG_DATABUF_BUF_ADDR( audDataBuf ) (  &( (audDataBuf).nDataBuf )   )

/** Macro that clears the speed factor bits in the nFlag parameter of the
    elite_msg_data_buffer_t structure.
*/
#define CLR_DATA_BUFFER_SPEED_FACTOR_BITS( nFlag ) ( (nFlag) & (~(ELITE_DATA_BUFFER_SPEED_FACTOR_BITS)) )

/** Macro that sets the speed factor bits in the nFlag parameter of the
    elite_msg_data_buffer_t structure.
*/
#define SET_DATA_BUFFER_SPEED_FACTOR_BITS( nFlag, speed_factor ) ( ((nFlag) | (speed_factor)) )

/** Macro that gets the speed factor bits from the nFlag parameter of the
    elite_msg_data_buffer_t structure.
*/
#define GET_DATA_BUFFER_SPEED_FACTOR_BITS( nFlag ) ( (nFlag) & (ELITE_DATA_BUFFER_SPEED_FACTOR_BITS) )

/** @} */ /* end_addtogroup elite_msg_constants_macros */

/** @addtogroup elite_msg_datatypes
@{ */

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/** Message structure for all Elite messages. The structure is of size 8 uint8_t.
 */
typedef struct
{
   QURT_ELITE_ALIGN(uint32_t, 8) unOpCode;
   /**< Elite primary operation code (32-bit) to help distinguish the payload.

        This field starts at the 8-byte alignment boundary to ensure that the
        message is also 8-byte aligned. */

   void    *pPayload;
   /**< Payload buffer (32-bit) pointer or payload value. The interpretation
                                           depends on the above operation code. */

} elite_msg_any_t;

/** Common header structure for the payload of most Elite command messages.
 */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment. This is different from the
         unResponseResult and can be used to identify which service sent the
         response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

} elite_msg_any_payload_t;

/** Forward declaration for the payload of #ELITE_CMD_START_SERVICE, which is
    used to request the start of static services.
  */
typedef elite_msg_any_payload_t    elite_msg_cmd_start_svc_t;

/** Forward declaration for the payload of  #ELITE_CMD_DESTROY_SERVICE, which
    requests a service to destroy itself. The sender of this command message
    must know the service master thread ID and join that thread to await full
    cleanup.
  */
typedef elite_msg_any_payload_t    elite_msg_cmd_destroy_svc_t;

/** Forward declaration for the payload of #ELITE_CMD_PAUSE. When paused, a
    service stops reading its data queue and stops delivering data to its peers.
    */
typedef elite_msg_any_payload_t    elite_msg_cmd_pause_t;

/** Forward declaration for the payload of #ELITE_CMD_FLUSH, which commands a
    service to empty its input data queue, empty any partially filled input or
    output data buffers, and return all buffers to their originating buffer queues.
    */
typedef elite_msg_any_payload_t    elite_msg_cmd_flush_t;

/** Forward declaration for the payload of #ELITE_CMD_STOP_SERVICE. When
    stopped, a service resets any parameters and starts delivering data to its
    peers.
    */
typedef elite_msg_any_payload_t    elite_msg_cmd_stop_t;


/** Payload of the following messages:
  - The #ELITE_CMD_CONNECT message, which commands one service to connect to
  another service.
  - The #ELITE_CMD_DISCONNECT message, which commands one service to disconnect
    from another service.
 */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment. This is different from the
         unResponseResult and can be used to identify which service sent the
         response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    elite_svc_handle_t *pSvcHandle;
    /**< Handle to the peer with which to connect or from which to disconnect. */

} elite_msg_cmd_connect_t ;

/** Common fields in an #ELITE_CMD_SET_PARAM or #ELITE_CMD_GET_PARAM 
  payload.
*/
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment. This is different from the
         unResponseResult and can be used to identify which service sent the
         response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unParamId;
    /**< Parameter ID. The rest of the payload is derived based on this ID.

         The parameter ID is 32 bits and is globally unique. A service can
         define a globally unique parameter ID as follows:

    - The most significant 16 bits are the most significant 16 bits of the
      service ID that defines this parameter ID.
    - The least significant 16 bits are defined by the service.

   @note1hang If the most significant 16 bits of the parameter ID are all
   zeros, this message is reserved for a common parameter ID. */

} elite_msg_param_header_t;

/** Payload of the #ELITE_DATA_BUFFER message, which is used to exchange a 
  buffer between services.
 */
typedef struct
{
   /* Must start with 8 uint8_t aligned boundary */
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment. This is different from the
         unResponseResult and can be used to identify which service sent the
         response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    /*Must be 8 uint8_t aligned here. */
    uint64_t        ullTimeStamp;
    /**< Timestamp of buffer in microseconds. */

    uint32_t        nFlag;
    /**< Flag associated with the buffer.

         Bit 31 specifies whether the timestamp is valid:
         - 1 -- Valid timestamp
         - 0 -- Invalid timestamp
         The bitmask is #ELITE_DATA_BUFFER_TIME_STAMP_BITMASK.

         Bits 0 to 15 indicate the speed factor:
											- Speed factor is a q12 value
         - Range of the speed factor is from 1024 to 16384
         - Default value for normal speed is 4096
         The bitmask is #ELITE_DATA_BUFFER_SPEED_FACTOR_BITS.

         All other bits are reserved. */

    int32_t         nOffset;
    /**< Offset into the data buffer where the valid data begins. */

    int32_t         nActualSize;
    /**< Number of valid bytes in the buffer. */

    int32_t         nMaxSize;
    /**< Total number of bytes allocated in the data region. */

    /* 8 uint8_t aligned here. Assuming malloc always return 8 uint8_t aligned boundary.  */
    /* Data is in band. */
    int32_t         nDataBuf;
    /**< Beginning of the data buffer. The caller uses the
         GET_AUDDATABUF_REQ_SIZE macro to allocate enough space. @newpagetable */
                                       /* compiler might insert memory holes here!!!!!! */
} elite_msg_data_buffer_t ;

/** Common fields in any #ELITE_CUSTOM_MSG message payload.
*/
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment. This is different from the
         unResponseResult and can be used to identify which service sent the
         response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unSecOpCode;
    /**< Secondary operation code that indicates the format for the rest of
         the payload.

         The secondary operation code for #ELITE_CUSTOM_MSG is 32 bits and
         is globally unique. A service can define a globally unique secondary
         operation  code as  follows:

         - The most significant 16 bits are the most significant 16 bits of
           the service ID that defines this secondary operation code.
   - The least significant 16 bits are defined by the service.

         @note1hang If the most significant 16 bits of the secondary operation
                    code are all zeros, this message is reserved for a common
                    custom message. @newpagetable */

} elite_msg_custom_header_t;

/** Common fields in the #ELITE_DATA_MEDIA_TYPE message payload.
  This payload indicates the media type for all subsequent data buffers.

  When FLUSH completes, each service precedes any subsequent data
  buffers with ELITE_DATA_MEDIA_TYPE in case there is confusion about a media
  type after FLUSH.
 */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment. This is different from the
         unResponseResult and can be used to identify which service sent the
         response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unMediaTypeFormat;
    /**< Format of the rest of the payload. */

} elite_msg_data_media_type_header_t;


/** Common fields in the #ELITE_DATA_EOS message payload.
 */
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
         response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment. This is different from the
         unResponseResult and can be used to identify which service sent the
         response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unEosFormat;
    /**< Format of the rest of the payload. @newpagetable */

} elite_msg_data_eos_header_t ;

/** Payload of the #ELITE_CMD_RUN message. A client sends this command to 
  start rendering samples to the codec.
*/
typedef struct
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that no
	     response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

	     This is different from the unResponseResult and can be used to
		 identify which service sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */
} elite_msg_cmd_run_t ;

/**
  Forward declaration for the payload of the #ELITE_MIPS_CHANGE_EVENT message,
  which is used to indicate a change in the MIPS requirement of the service. */
typedef elite_msg_any_payload_t    elite_msg_evt_mips_change_t;

/** this event turns all DML related ADSPPM resources off */
#define ELITE_DML_CHANGE_EVENT__DML_VOTE_OFF_LPM_VOTE_REGULAR_OFF 0
/** this event turns all DML related ADSPPM resources on */
#define ELITE_DML_CHANGE_EVENT__DML_VOTE_ON_LPM_VOTE_REGULAR_ON  1
/** this event turns DML related ADSPPM resources off and puts LPM in retention */
#define ELITE_DML_CHANGE_EVENT__DML_VOTE_OFF_LPM_VOTE_RETENTION_ON 2
/** this event turns DML related ADSPPM resources off and puts LPM in retention also off. */
#define ELITE_DML_CHANGE_EVENT__DML_VOTE_OFF_LPM_VOTE_RETENTION_OFF 3

/** Payload of the #ELITE_DML_CHANGE_EVENT message, which is used to
  indicate a change in the DML hardware requirement of the service.
  */
typedef struct _elite_msg_evt_dml_req_t
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that
                                           no response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

	     This is different from the unResponseResult and can be used to
		 identify which service sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unDmlReqvalue;
    /**< DMLite hardware requirement flag.

        @values
		- 0 -- DML off
		- 1 -- DML on
		- 2 -- DML off but LPM in retention mode.
		- 3 -- DML off and LPM retention off.
		@tablebulletend @newpagetable */

} elite_msg_evt_dml_req_t;

/** Payload of the #ELITE_BW_CHANGE_EVENT message, which is used to
  indicate a change in the DML hardware requirement of the service.
 */
typedef struct _elite_msg_evt_bw_change_t
{
    qurt_elite_queue_t   *pBufferReturnQ;
    /**< Queue to which this payload buffer must be returned. */

    qurt_elite_queue_t   *pResponseQ;
    /**< Queue to which to send the acknowledgment. NULL indicates that
         no response is required. */

    uint32_t        unClientToken;
    /**< Token to be given in the acknowledgment.

        This is different from the unResponseResult and can be used to
       identify which service sent the response to the server. */

    uint32_t        unResponseResult;
    /**< Response result that the server sends back to the client. */

    uint32_t        unDMLused;
    /**< DMLite hardware being used flag.

        @values
      - Set to 1 if DML is being used.
      - Otherwise, set to 0 @tablebulletend @newpagetable */

} elite_msg_evt_bw_change_t;


/** @} */ /* end_addtogroup elite_msg_datatypes */

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEMSG_H

