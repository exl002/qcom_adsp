/*========================================================================

*//** @file qurt_elite_channel.cpp
This file contains a utility to form a channel of a combination of up to
32 signals/queues/timers. Client can wait on any combination thereof and
be woken when any desired element is active.

Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/elite/qurt_elite/src/qurt_elite_channel.cpp#10 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/04/10   mwc      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Function Definitions
** ======================================================================= */


/****************************************************************************
** Channels
*****************************************************************************/
static ADSPResult qurt_elite_channel_check_bit_mask(qurt_elite_channel_t* pChannel, uint32_t *pUnBitMask)
{
   uint32_t unBitMask = *pUnBitMask;

   //check if all available bits were taken
   if (QURT_ELITE_CHANNEL_ALL_BITS == pChannel->unBitsUsedMask)
   {
      return ADSP_ENEEDMORE;
   }

   //check if a single bit is requested, i.e., unBitMask must be power of 2
   //check if the requested bit is available on channel
   if ( (NULL != unBitMask) && ( (0 != (unBitMask & (unBitMask-1)))
          ||(0 != (unBitMask & pChannel->unBitsUsedMask))) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Incorrect BitMask of queue/signal!");
      return ADSP_EBADPARAM;
   }

   if (NULL == unBitMask)
   {  // user do not care about the position of channel bit,
      // obtain the available bit from LSB, pChannel->unBitsUsedMask is the
      // bookkeeper of available bits, 1-used, 0-available
      //unBitMask = 1;
      //while ( pChannel->unBitsUsedMask & unBitMask) unBitMask <<= 1;
      unBitMask = 1 << (31 - Q6_R_cl1_R(pChannel->unBitsUsedMask));
   }

   *pUnBitMask = unBitMask;

   return ADSP_EOK;
}

ADSPResult qurt_elite_channel_addq(qurt_elite_channel_t* pChannel, qurt_elite_queue_t* pQ, uint32_t unBitMask)
{
   ADSPResult  result;
   if ( ADSP_FAILED(result = qurt_elite_channel_check_bit_mask(pChannel,&unBitMask)) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cannot add queue!");
      return result;
   }

   // clear any residual signal
   (void) qurt_signal2_clear(&pChannel->anysig, unBitMask);

   pQ->pChannel = pChannel;
   pQ->myChannelBit = unBitMask;
#ifdef DEBUG_QURT_ELITE_CHANNEL
   MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO, "ADDQ: Q=0x%x Channelptr=0x%x Bitfield=0x%x", pQ, pChannel, pQ->myChannelBit);
#endif //DEBUG_QURT_ELITE_CHANNEL
   //bookkeeping available channel bits: 1-used, 0-available
   pChannel->unBitsUsedMask |= unBitMask;

   return ADSP_EOK;
}

ADSPResult qurt_elite_channel_add_signal(qurt_elite_channel_t* pChannel, qurt_elite_signal_t* pSignal, uint32_t unBitMask)
{
   ADSPResult  result;
   if ( ADSP_FAILED(result = qurt_elite_channel_check_bit_mask(pChannel,&unBitMask)) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cannot add signal!");
      return result;
   }

   // clear any residual signal
   (void) qurt_signal2_clear(&pChannel->anysig, unBitMask);

   pSignal->pChannel = pChannel;
   pSignal->unMyChannelBit = unBitMask;
#ifdef DEBUG_QURT_ELITE_CHANNEL
   MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO, "ADDSIG: Channelptr=0x%x Signalptr=0x%x ChannelBit=%d", pChannel, pSignal, pSignal->unMyChannelBit);
#endif //DEBUG_QURT_ELITE_CHANNEL
   //bookkeeping available channel bits: 1-used, 0-available
   pChannel->unBitsUsedMask |= unBitMask;

   return ADSP_EOK;
}

