/*============================================================================
  FILE: sns_em.c

  This file contains the Sensors Event Manager implementation

  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/evmgr/src/sns_em.c#1 $  */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-03-11  jhh  Add NULL pointer check before using it.
  2014-01-23  jhh  Use ATS timer instead of qurt timer
  2013-10-03  sc   Use qurt_timer_creaet_sig2() that is required for the new
                   qurt_elite timer structure.
  2013-05-07  sc   Fix klocwork errors; check for NULL pointers.
  2013-02-10  sc   - Round up when converting usec to tick
                   - Ensure duration is in allowable range when using qurt timer
  2013-01-10  sd   return SNS_ERR_FAILED if register timer fails
  2012-12-14  pn   Implemented sns_em_cancel_timer()
  2012-12-14  jhh  Add new function qurt_elite_non_deferrable_timer_periodic_start to
                   support non-deferrable periodic timer
  2012-11-15  ps   Change heap ID from QURT_ELITE_HEAP_DEFAULT to sns_heap_id 
  2012-11-02  vh   Change the task create to sns_os_task_create_ext for SNS_DSPS_BUILD
  2012-10-19  ag   Remove debug messages
  2012-10-03  sc   Scale timestamp back down to 32768Hz tick.
  2012-10-02  jhh  Add new funciton qurt_elite_timer_create_nondeferrable to use
                   non-deferrable timer until qurt_elite support is available
  2012-09-12  sc   Fix sns_em_convert_sec_in_q16_to_localtick to get right tick
  2012-08-30  sc   Use mutex to protect g_em_channel when signal is added to it
  2012-08-26  sc   Clear common signal after the EM thread receives it
  2012-08-23  ag   Fix timestamps; handle 19.2MHz QTimer
  2012-07-02  sc   Added a common signal to update the timer signals to wait for;
                   this approach eliminates polling within EM thread
                   Reduced EM thread stack size
  2012-01-16  sd   Time Stamp change for DSPS using MPM time tick
  2011-11-14  jhh  Updated SNS_OS_MALLOC to meet new API
  2011-04-27  br   Disabled the long periodic timer
  2011-02-28  br   chaned SNS_OS_TIME_US when SNS_DSPS_PROFILE_ON is defined
  2011-02-23  br   Calling uCOS timer hook when SNS_DSPS_PROFILE_ON is defined
  2010-12-21  hm   Backing out the timer workaround
  2010-12-15  hm   Workaround for triggering the timer interrupt if it has expired
  2010-12-03  pg   Added __SNS_MODULE__ to be used by OI mem mgr in debug builds.
  2010-11-30  ad   Added function to convert time in seconds in Q16 format
                   to time in local ticks
  2010-11-16  pg   Workaround for periodic timer not working after unregister
                   and re-register.
  2010-09-14  hm   Implemented get next pending timer and cancel all timers
  2010-08-31  jtl  Calling sns_init_done() when init is done.
  2010-08-24  hm   Added support for periodic and one-shot timers
  2010-08-13  hm   Initial version

============================================================================*/

#define __SNS_MODULE__ SNS_EM

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "fixed_point.h"
#include "sns_em.h"
#include "sns_init.h"
#include "sns_memmgr.h"

#include "qurt_elite.h"
#include "sns_common.h"
#include "sns_osa.h"

// ATS timer
#include "timer.h"

/// ADSP error codes
#include "adsp_error_codes.h"

/* -----------------------------------------------------------------------
** Static variables
** ----------------------------------------------------------------------- */
static timer_group_type sSensorTimerGroup;

typedef struct sns_em_tmr_s{
  uint8_t                bTimerDefined;
  timer_type             sTimer;
  timer_ptr_type         pTimer;
  timer_group_ptr        pTimerGroup;
  sns_em_timer_type_e    tmr_type;
} sns_em_tmr_s;


/*---------------------------------------------------------------------------
* Definitions and Types
* -------------------------------------------------------------------------*/
#define USEC_PER_SEC 1000000

OS_EVENT *g_em_mutex;

/*=====================================================================================
  FUNCTION:  sns_em_init
=====================================================================================*/
sns_err_code_e sns_em_init(void)
{

  uint8_t os_err;

  // using priority of 10 for now 
  g_em_mutex = sns_os_mutex_create(10, &os_err);
  if(os_err != OS_ERR_NONE)
  {
    MSG_1(MSG_SSID_SNS, DBG_ERROR_PRIO, "sns_os_mutex_create failed %d", os_err);
    return SNS_ERR_FAILED;
  }


  sns_init_done();

  return SNS_SUCCESS;
}

/*=====================================================================================
  FUNCTION:  sns_em_get_get_timestamp
=====================================================================================*/
uint32_t sns_em_get_timestamp( void )
{
  uint64_t raw_tick = qurt_sysclock_get_hw_ticks();
  /* convert 19.2MHz tick to 32768Hz */
  return (uint32_t)((raw_tick << 4) / 9375);
}

/*=====================================================================================
  FUNCTION:  sns_em_create_timer_obj
=====================================================================================*/
sns_err_code_e sns_em_create_timer_obj( void (*timer_cb)(void *), 
                                        void *timer_cb_arg, 
                                        sns_em_timer_type_e timer_category,
                                        sns_em_timer_obj_t* timer_obj_ptr )
{
  uint8_t mutex_err;
  timer_error_type err;


  sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

  sns_em_tmr_s *new_tmr = qurt_elite_memory_malloc(sizeof(sns_em_tmr_s),sns_heap_id);

  if ( new_tmr == NULL )
  {
    MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "ATS timer create fail");
    sns_os_mutex_post(g_em_mutex);
    return SNS_ERR_NOMEM;
  }
  else
  {
    SNS_OS_MEMZERO(new_tmr, sizeof(sns_em_tmr_s));
  }

  new_tmr->tmr_type = timer_category;
  new_tmr->pTimer = &new_tmr->sTimer;
  new_tmr->pTimerGroup = &sSensorTimerGroup;

  *timer_obj_ptr = (sns_em_timer_obj_t)new_tmr;
  
  timer_group_set_deferrable(new_tmr->pTimerGroup, FALSE);

  err = timer_def_osal((*timer_obj_ptr)->pTimer, (*timer_obj_ptr)->pTimerGroup, TIMER_FUNC1_CB_TYPE, (timer_t1_cb_type)timer_cb, (time_osal_notify_data)timer_cb_arg);
  if( err != TE_SUCCESS )
  {
    MSG_1(MSG_SSID_SNS, DBG_ERROR_PRIO, "create ats timer  failed with status %d",err);
    sns_os_mutex_post(g_em_mutex);
    return SNS_ERR_FAILED;
  }  

  (*timer_obj_ptr)->bTimerDefined = 1;

  sns_os_mutex_post(g_em_mutex);
  return SNS_SUCCESS;
}


/*=====================================================================================
  FUNCTION:  sns_em_delete_timer
=====================================================================================*/
sns_err_code_e sns_em_delete_timer_obj( sns_em_timer_obj_t timer_obj )
{
  uint8_t mutex_err;
  sns_em_tmr_s *temp;
  uint8_t rv;

  sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

  if(timer_obj != NULL)
  {
    temp = timer_obj;
    temp->bTimerDefined = 0;
    if(timer_undef(temp->pTimer) != TE_SUCCESS)
    {
      MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "Timer_undef failed");
    }
    qurt_elite_memory_free(temp);

    rv = SNS_SUCCESS;
  }
  else
  {
    rv = SNS_ERR_FAILED;
  }

  sns_os_mutex_post(g_em_mutex);

  return rv;
}


/*=====================================================================================
  FUNCTION:  sns_em_register_timer
=====================================================================================*/
sns_err_code_e sns_em_register_timer( sns_em_timer_obj_t  timer_obj, 
                                      uint32_t            delta_tick_time )
{
  uint8_t mutex_err;

  sns_em_tmr_s *temp;
  uint32_t usec;
  timer_error_type err;
  uint8_t rv = SNS_ERR_FAILED;


  sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

  if(timer_obj == NULL)
  {
    MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "timer_obj is NULL");
    sns_os_mutex_post(g_em_mutex);
    return rv;
  }

  temp = timer_obj;

  usec = sns_em_convert_dspstick_to_usec(delta_tick_time);

  //ATS timer's minimal duration is the same as Qurt timers'
  if (usec < QURT_TIMER_MIN_DURATION)
      usec = QURT_TIMER_MIN_DURATION;

  if(temp->bTimerDefined == 1)
  {
    if(temp->tmr_type == SNS_EM_TIMER_TYPE_ONESHOT )
    {
      err = timer_set_64(temp->pTimer, (time_timetick_type)usec, 0, T_USEC);
      if ( err != 0 )
      {
        MSG_1(MSG_SSID_SNS, DBG_ERROR_PRIO, "start oneshot ats timer failed with status %d", err);
        rv = SNS_ERR_FAILED;
      }
      else
      {
        rv = (SNS_SUCCESS);
      }
    }
    else if(temp->tmr_type == SNS_EM_TIMER_TYPE_PERIODIC )
    {
      err = timer_set_64(temp->pTimer, (time_timetick_type)usec, (time_timetick_type)usec, T_USEC);
      if ( err != 0 )
      {
        MSG_1(MSG_SSID_SNS, DBG_ERROR_PRIO, "start periodic timer failed with status %d",err);
        rv = SNS_ERR_FAILED;
      }
      else
      {
        rv = (SNS_SUCCESS);
      }
    }
  }
  else
  {
    MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "Timer not defined - trynig to start timer that's not defined");
  }

  sns_os_mutex_post(g_em_mutex);

  return rv;

}

/*=====================================================================================
  FUNCTION:  sns_em_cancel_timer
=====================================================================================*/
sns_err_code_e sns_em_cancel_timer( sns_em_timer_obj_t timer_obj )
{
  uint8_t mutex_err;

  sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

  sns_em_tmr_s* temp = timer_obj;
  sns_err_code_e err = SNS_ERR_FAILED;
  if ( temp->tmr_type == SNS_EM_TIMER_TYPE_ONESHOT )
  {
    MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "Cancel SNS_EM_TIMER_TYPE_ONESHOT");
    timer_clr_64(temp->pTimer, T_USEC );
    err = SNS_SUCCESS;
  }
  sns_os_mutex_post(g_em_mutex);

  return err;
}


/*=====================================================================================
  FUNCTION:  sns_em_cancel_all_timers
=====================================================================================*/
sns_err_code_e sns_em_cancel_all_timers ( void )
{

  return (SNS_ERR_FAILED);

}


/*=====================================================================================
  FUNCTION:  sns_em_check_timer_pending
=====================================================================================*/
uint32_t sns_em_check_timer_pending( void )
{
  return 0;
}

/*=========================================================================
  FUNCTION:  sns_em_timer_get_remaining_time
  =========================================================================*/
/*!
  @brief Gets the timer remaining duration in microseconds

  @param[i] timer: timer
  @param[o] remaining: remaining duration in microseconds
 
  @return Sensors error code
*/
/*=======================================================================*/
sns_err_code_e sns_em_timer_get_remaining_time(
   const sns_em_timer_obj_t timer, uint32_t *remaining_time)
{
   uint8_t mutex_err;
   sns_em_tmr_s *temp;

   sns_os_mutex_pend(g_em_mutex, 0, &mutex_err);

   if(timer != NULL)
   {
     temp = timer;
     *remaining_time = timer_get_64( temp->pTimer, T_USEC );
   }

   sns_os_mutex_post(g_em_mutex);

   return SNS_SUCCESS;

}

/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_usec
=====================================================================================*/
uint32_t sns_em_convert_dspstick_to_usec( uint32_t dsps_tick )
{
  int64_t usec; 
  usec = ((int64_t)dsps_tick * USEC_PER_SEC)/DSPS_SLEEP_CLK;
  return (uint32_t)usec;
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_localtick_to_usec
=====================================================================================*/
uint32_t sns_em_convert_localtick_to_usec( uint32_t dsps_tick )
{
  return (sns_em_convert_dspstick_to_usec(dsps_tick));
}


/*=====================================================================================
  FUNCTION:  sns_em_convert_usec_to_dspstick
=====================================================================================*/
uint32_t sns_em_convert_usec_to_dspstick( uint32_t usec )
{
  int64_t dspstick; 
  /* round up the result */
  dspstick = ((int64_t)usec * DSPS_SLEEP_CLK + USEC_PER_SEC - 1)/USEC_PER_SEC;
  return (uint32_t)dspstick;
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_usec_to_localtick
=====================================================================================*/
uint32_t sns_em_convert_usec_to_localtick( uint32_t usec )
{
  return (sns_em_convert_usec_to_dspstick(usec) );
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_appstime
=====================================================================================*/
uint32_t sns_em_convert_dspstick_to_appstime( uint32_t dsps_tick )
{
  return 0;
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_gpstime
=====================================================================================*/
uint32_t sns_em_convert_dspstick_to_gpstime( uint32_t dsps_tick )
{
  return 0;
}

/*=====================================================================================
  FUNCTION:  sns_em_convert_sec_in_q16_to_localtick
=====================================================================================*/
uint32_t sns_em_convert_sec_in_q16_to_localtick( uint32_t time_sec_q16 )
{
  uint32_t repPerioduSec = 
    (uint32_t)(FX_CONV(((uint64_t)time_sec_q16 * 1000000), FX_QFACTOR, 0));

  return (sns_em_convert_usec_to_dspstick(repPerioduSec));
}
