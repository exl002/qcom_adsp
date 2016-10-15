#ifndef ELITEMSG_PARAM_H
#define ELITEMSG_PARAM_H

/**
@file EliteMsg_Param.h

@brief This file defines common param ID and payload structure
for ELITE_CMD_SET_PARAM  and ELITE_CMD_GET_PARAM messages.
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
02/04/10   DC      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* Audio */
#include "EliteMsg.h"
#include "EliteAprIf.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/
/** @addtogroup elite_msg_param_ids
@{ */
/** Defines the parameter ID for calibrating the postprocessor/preprocessor with
  the payload as defined in @xhyperref{Q10,[Q10]}.
*/ /* Q10 = Hex Multimedia 2.0: Audio API Interface Spec (80-NF768-1) */
#define ELITEMSG_PARAM_ID_CAL         0x00000001

/** Payload of the #ELITE_CMD_SET_PARAM and #ELITE_CMD_GET_PARAM messages
  when unParamId&nbsp;= #ELITEMSG_PARAM_ID_CAL.
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

   uint32_t         unParamId;
   /**< Elite parameter ID; supports only #ELITEMSG_PARAM_ID_CAL. The rest of
        the payload is derived based on this ID and the message. */

   int32_t          *pnParamData;
   /**< Points to *_param_data_t (e.g., adm_param_data_v5_t,
        voice_param_data_t, asm_stream_param_data_v2_t).

        The payload is derived from the get or set parameter structure defined
        in @xhyperref{Q10,[Q10]} and @xhyperref{Q11,[Q11]}. */
   /* Q10 = Hex Multimedia 2.0: Audio API Interface Spec (80-NF768-1) 
      Q11 = Hex Multimedia 2.0: Voice API Interface Spec (80-NF768-1) 
   */

   uint16_t          unSize;
   /**< Payload size in bytes for the memory pointed by pnParamData. */

} elite_msg_param_cal_t;

/** Defines the parameter ID that sets thread priority.

    The valid range for thread priorities is from 0 to 255.
    The higher number indicates a higher priority if QuRT_Elite functions are
    used.
  */
#define ELITEMSG_PARAM_ID_THREAD_PRIO     0x00000002

/** Low thread priority. */
#define ELITEMSG_PARAM_THREAD_PRIO_LOW         50

/** Medium thread priority. */
#define ELITEMSG_PARAM_THREAD_PRIO_MED         100

/** High thread priority. */
#define ELITEMSG_PARAM_THREAD_PRIO_HIGH        150

/** Critical (highest) thread priority. @newpage */
#define ELITEMSG_PARAM_THREAD_PRIO_CRITICAL    200

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/** Payload of the #ELITE_CMD_SET_PARAM and #ELITE_CMD_GET_PARAM messages
    when the payload data is a uint32_t, i.e., when
    unParamId&nbsp;= #ELITEMSG_PARAM_ID_THREAD_PRIO.
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

   uint32_t         unParamId;
   /**< Parameter ID. The rest of the payload is derived based on this ID. */

   uint32_t         unParam;
   /**< Parameter value to get/set. */

} elite_msg_param_any_t;

/** @} */ /* end_addtogroup elite_msg_param_ids */

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEMSG_PARAM_H

