#ifndef ELITEMSG_CUSTOM_H
#define ELITEMSG_CUSTOM_H

/**
@file EliteMsg_Custom.h

@brief This file defines secondary opcode and payload structure
for ELITE_CUSTOM_MSG that are common to Elite service.
*/


/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/28/10   sw      (Tech Pubs) Copied secondary msg info to mainpage.dox file.
02/04/10   DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All rights reserved.
   Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#include "EliteMsg.h"
/*This file has dependency on ADSPAudioDriver and APR packet format. Hence keep it separate
from EliteMsg.h and EliteMsgData.h
*/
//#include "ADSPAudioCommand.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

//#define    ELITEMSG_CUSTOM_ADSPAUDIO    0    /* Indicates the rest of payload is an ADSPAudioCommand */
//#define    ELITEMSG_CUSTOM_APR          1    /* Indicates the rest of payload is an APR packet */

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/*
   Payload structure for ELITE_CUSTOM_MSG and
   unSecOpCode == ELITEMSG_CUSTOM_ADSPAUDIO
 */
//typedef struct
//{
//    qurt_elite_queue_t   *pBufferReturnQ;  /* Queue to which this payload
//                                         buffer must be returned. */
//    qurt_elite_queue_t   *pResponseQ;      /* Queue to which to send the acknowledgment.
//                                        NULL indicates that no response is required. */
//    uint32_t            unClientToken;    /* Token to be given in the
//                                           acknowledgment. This is different
//                                           from the unResponeResult and can be
//                                           used to identify which service sent
//                                           the response to the server. */
//    uint32_t            unResponseResult; /* Response result that the server
//                                           sends back to the client. */
//    uint32_t           unSecOpCode;       /* This is the secondary operation code indicating
//                                                    the format for the rest of payload */
//    AdspAudioCommandHeader adspAudioCommandHeader;  /* The header of ADSPAudioCommand payload.
//                                                        The rest of payload is further derived
//                                                        base on this header. */
//} elite_msg_custom_adsp_audio_t;
//

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEMSG_CUSTOM_H

