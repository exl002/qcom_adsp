/*========================================================================

*//** @file qurt_elite_timer.cpp
This file contains utilities of Timers , such as
timer create, start/restart,delete.

Copyright (c) 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/elite/qurt_elite/src/qurt_elite_timer.cpp#11 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/13/09   mwc     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "audio_basic_op.h"

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
** Timers
*****************************************************************************/

/*
APIs: create, delete, get duration
   Handler: signal
   Timer Type: ONESHOT, PERIODIC, SLEEP
   Duration: In us
 */
/*
qurt_elite_timer_create:
* @param[in] pTimer Pointer to qurt_elite timer object
* @param[in] timerType  One of : QURT_ELITE_TIMER_ONESHOT_DURATION, QURT_ELITE_TIMER_PERIODIC,QURT_ELITE_TIMER_ONESHOT_ABSOLUTE
* @param[in] clockSource  One of : QURT_ELITE_TIMER_USER, QURT_ELITE_TIMER_RTOS, QURT_ELITE_TIMER_DMA_I2S, QURT_ELITE_TIMER_DMA_MI2S, QURT_ELITE_TIMER_WDOG, QURT_ELITE_TIMER_AVS
* even when not used. Only QURT_ELITE_TIMER_USER is implemented currently.
* @param[in] pSignal pointer to signal to be generated when timer expires.
* @param[in] handlerType  One of :
    QURT_ELITE_TIMER_NONE No handler - just start timer
    QURT_ELITE_TIMER_SIGNAL Signal generated to calling thread
    QURT_ELITE_TIMER_CALLBACK Callback a specified function, not implemented

* @return indication of success (0) or failure (non-0)
*/

int qurt_elite_timer_create(qurt_elite_timer_t *pTimer, qurt_elite_timer_duration_t timerType, qurt_elite_timer_src_t clockSource,
                   qurt_elite_signal_t *pSignal)
{
   qurt_timer_attr_t     timer_attr;
   int nStatus;


   /* Do error checks */

   if (QURT_ELITE_TIMER_USER != clockSource)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Only USER TIMER supported for now");
      return ADSP_EBADPARAM;
   }

   pTimer->pChannel = qurt_elite_signal_get_channel(pSignal);

   if (NULL==pTimer->pChannel)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "signal does not belong to any channel");
      return ADSP_EBADPARAM;
   }

   pTimer->uTimerType = (unsigned int)timerType;
   pTimer->timer_sigmask = qurt_elite_signal_get_channel_bit(pSignal);
   pTimer->istimerCreated = FALSE;

   qurt_timer_attr_init(&timer_attr);

   /* At present qurt is not supporting any API which will create timer without arming it*/
   /* so here arming the timer with MAX duration and immediately cancelling/stopping this timer*/
   /* This is not applicable to Periodic timers because as the restart and stop APIS only */
   /* applicable for ONESHOT timers                                             */

   if (QURT_ELITE_TIMER_PERIODIC != timerType)
   {

      if (QURT_ELITE_TIMER_ONESHOT_DURATION == timerType)
        qurt_timer_attr_set_duration(&timer_attr, QURT_TIMER_MAX_DURATION);
      else
        qurt_timer_attr_set_expiry(&timer_attr, QURT_TIMER_MAX_DURATION);

      if ( EOK != (nStatus = qurt_timer_create_sig2( (qurt_timer_t *) &(pTimer->qTimerObj), &timer_attr,
                                                &(pTimer->pChannel->anysig), pTimer->timer_sigmask)) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed creating timer with status=%d!", nStatus);
         return ADSP_EFAILED;
   }

      if ( EOK != (nStatus = qurt_timer_stop (pTimer->qTimerObj)))
       {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed Stopping timer with status=%d!", nStatus);
          return ADSP_EFAILED;
      }

   }
#ifdef DEBUG_QURT_ELITE_TIMER
   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Qurt timer setup done");
#endif //DEBUG_QURT_ELITE_TIMER
   return ADSP_EOK;

}

int qurt_elite_timer_sleep(int64_t llMicrosec)
{
   qurt_timer_t          timer;
   qurt_timer_attr_t     timer_attr;
   qurt_timer_duration_t timer_duration;
   qurt_signal2_t anysignal;
   int               nStatus;
   uint32_t            unTimerSignalBit;
   unsigned int      timer_sigmask = 1;

   if (llMicrosec < QURT_TIMER_MIN_DURATION)
   {
       timer_duration = (qurt_timer_duration_t)QURT_TIMER_MIN_DURATION;
   }
   else
   {
       timer_duration = (qurt_timer_duration_t) llMicrosec;
   }

   /* setup timer attributes */
   qurt_timer_attr_init(&timer_attr);
   qurt_timer_attr_set_type(&timer_attr, QURT_TIMER_ONESHOT );
   qurt_timer_attr_set_duration(&timer_attr, timer_duration);

   qurt_signal2_init(&anysignal);

   if ( EOK != (nStatus = qurt_timer_create_sig2( &timer, &timer_attr, &anysignal, timer_sigmask)) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed creating sleep timer with status=%d!", nStatus);
      return nStatus;
   }
#ifdef DEBUG_QURT_ELITE_TIMER
   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "wait until sleep timer expires");
#endif //DEBUG_QURT_ELITE_TIMER
   unTimerSignalBit = qurt_signal2_wait_any(&anysignal, timer_sigmask);
   if ( timer_sigmask != unTimerSignalBit )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Sleep timer received unexpected signal=%lu", unTimerSignalBit);
      nStatus = qurt_timer_delete(timer);
      return ADSP_EFAILED;
   }

   qurt_signal2_destroy(&anysignal);

   nStatus = qurt_timer_delete(timer);

   return nStatus;

}


int qurt_elite_timer_delete(qurt_elite_timer_t *pTimer)
{
  if (NULL == pTimer || NULL == pTimer->qTimerObj)
  {
     return ADSP_EBADPARAM;
  }

  int   nStatus;

  if(EOK != (nStatus=qurt_timer_delete((qurt_timer_t)(pTimer->qTimerObj))))
{
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed deleting timer with status=%d!", nStatus);
    return ADSP_EFAILED;
  }

  memset(pTimer,0,sizeof(qurt_elite_timer_t));
  return ADSP_EOK;
}


unsigned long long qurt_elite_timer_get_duration(qurt_elite_timer_t *pTimer)
{
   int                nStatus;
   qurt_timer_attr_t      timer_attr;
   qurt_timer_duration_t  timer_duration;

   if ( EOK != (nStatus=qurt_timer_get_attr(pTimer->qTimerObj, &timer_attr)) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed getting timer attributes with %d!", nStatus);
      /* return 0 rather than error status yet interpreted as an unsigned number */
      return 0;
   }

   /* query the OS for the timer duration */
   qurt_timer_attr_get_duration(&timer_attr, &timer_duration);
   return ((unsigned long long) timer_duration);
}

/*
qurt_elite_timer_get_time:
Gets the QURT system clock time
* Converts ticks into microseconds
* 1 tick = 1/19.2MHz seconds
* Micro Seconds = Ticks * 10ULL/192ULL. Compiler uses magic multiply functions to resolve this 
* repeasted fractional binary.  
* performance is 10 cycles

@returns the number of ticks elapsed on QURT system clock in terms of microseconds
*/


uint64_t qurt_elite_timer_get_time(void)
{

  uint64_t ullCurrTime =  (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);

  return ullCurrTime;

}


/*
qurt_elite_timer_get_time_in_msec:
* Converts ticks into milliseconds
* 1 tick = 1/19.2MHz seconds
* MilliSeconds = Ticks * 10ULL/192000ULL. Compiler uses magic multiply functions to resolve this 
* repeated fractional binary.  
* performance is 10 cycles

@returns the number of ticks elapsed on QURT system clock in terms of milliseconds
*/


uint64_t qurt_elite_timer_get_time_in_msec(void)
{

  uint64_t time =  (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192000ull);

  return time;

}


/*
Start/Restart Absolute One shot timer
@param[in] timer object
@param[in] duration : absolute time of the timer in usec
@return indication of success (0) or failure (non-0)
*/



int qurt_elite_timer_oneshot_start_absolute(qurt_elite_timer_t *pTimer, int64_t time)

{

   int nStatus;

   QURT_ELITE_ASSERT(QURT_ELITE_TIMER_ONESHOT_ABSOLUTE == pTimer->uTimerType);

     if ( EOK != (nStatus =qurt_timer_restart((qurt_timer_t)(pTimer->qTimerObj),(qurt_timer_time_t)time)))
   {
     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed restarting Absolute timer with status=%d!", nStatus);
     return ADSP_EFAILED;
   }

   return ADSP_EOK;
}



/*
Start/Restart Duration based One shot timer
@param[in] timer object
@param[in] duration : duration of the timer in usec
@return indication of success (0) or failure (non-0)
*/



int qurt_elite_timer_oneshot_start_duration(qurt_elite_timer_t *pTimer, int64_t duration)

{

   int nStatus;

   qurt_timer_time_t timer_duration;

   QURT_ELITE_ASSERT(QURT_ELITE_TIMER_ONESHOT_DURATION == pTimer->uTimerType);

   if ( (QURT_TIMER_MAX_DURATION < (uint64_t)duration) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Timer is too long!");
      return ADSP_EBADPARAM;
   }

   timer_duration = (qurt_timer_time_t)duration;

   if ( (QURT_TIMER_MIN_DURATION > (uint64_t)duration) )
   {
       timer_duration = (qurt_timer_time_t)QURT_TIMER_MIN_DURATION;
   }

   if ( EOK != (nStatus =qurt_timer_restart((qurt_timer_t)(pTimer->qTimerObj),timer_duration)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed restarting Duration timer with status=%d!", nStatus);
      return ADSP_EFAILED;
   }
   return ADSP_EOK;
}

/*
Cancel Duration based One shot timer
@param[in] timer object
@return indication of success (0) or failure (non-0)
*/
int qurt_elite_timer_oneshot_cancel(qurt_elite_timer_t *pTimer)

{

   int nStatus;

   QURT_ELITE_ASSERT( (QURT_ELITE_TIMER_ONESHOT_DURATION==pTimer->uTimerType)||(QURT_ELITE_TIMER_ONESHOT_ABSOLUTE==pTimer->uTimerType) );

   if ( EOK != (nStatus =qurt_timer_stop((qurt_timer_t)(pTimer->qTimerObj))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed stop Duration timer with status=%d!", nStatus);
      return ADSP_EFAILED;
   }
   return ADSP_EOK;
}

/*
Start Periodic timer
@param[in] timer object
@param[in] duration : duration of the timer in usec
@return indication of success (0) or failure (non-0)
*/
int qurt_elite_timer_periodic_start(qurt_elite_timer_t *pTimer, int64_t duration)

{
   int nStatus;
   qurt_timer_attr_t timer_attr;


   QURT_ELITE_ASSERT(QURT_ELITE_TIMER_PERIODIC == pTimer->uTimerType);

   if(pTimer->istimerCreated == FALSE)
   {
     qurt_timer_attr_init(&timer_attr);
     qurt_timer_attr_set_type((qurt_timer_attr_t *)&(timer_attr), QURT_TIMER_PERIODIC );
     qurt_timer_attr_set_duration((qurt_timer_attr_t *)&(timer_attr), duration);


     if ( EOK != (nStatus =qurt_timer_create_sig2( (qurt_timer_t *) &(pTimer->qTimerObj), (qurt_timer_attr_t *)&(timer_attr),
                                            &(pTimer->pChannel->anysig), pTimer->timer_sigmask)))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed Starting Periodic timer with status=%d!", nStatus);
       return ADSP_EFAILED;
     }
     pTimer->istimerCreated = TRUE;
   }
   else
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Restarting is not supported for periodic timer!");
     return ADSP_EFAILED;
   }

   return ADSP_EOK;
}



