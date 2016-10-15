/*=============================================================================

                 QURT_TIMER_SHIM

GENERAL DESCRIPTION
   Implements a shim layer for Qurt Timers to connect to ATS Timers

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010 - 2013
   by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.adsp/2.2/services/time/src/qurt_timer_shim.c#7 $ 
$DateTime: 2013/09/11 08:59:47 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/20/13   ab        Added changes for MultiPD
08/01/12   gparikh   File created.

=============================================================================*/


/*****************************************************************************/
/*                           INCLUDE FILES                                   */
/*****************************************************************************/
//#include "../../../core/kernel/qurt/qurtos/include/qurtos_timer_libs.h"
//#include "../../../core/kernel/qurt/libs/qurt/include/public/qurt.h"
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "timer_v.h"
#include "timer_qdi_v.h"
#include "qmutex.h"
#include "msg.h"
#include "err.h"
#include "qurt.h"
#include "qurt_event.h"
#include "qurt_cycles.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt_futex.h"
#include "qurt_pimutex.h"


/*===========================================================================
Data Structures
===========================================================================*/
static boolean   qurt_timer_groups_init = FALSE;
static timer_group_type qurt_timer_groups[QURT_TIMER_MAX_GROUPS];

/*extern variables*/

extern qurt_mutex_t timer_qdi_mutex;
/*MACRO for locking/unlocking the qmutex*/
#define TIMER_QDI_LOCK()  \
   qurt_pimutex_lock(&timer_qdi_mutex); \
  
#define TIMER_QDI_FREE()  \
   qurt_pimutex_unlock(&timer_qdi_mutex); \

/* Pointer to the mutex object for protecting critical-section of timer 
   code*/
extern qurt_mutex_t timer_qemu_mutex;
extern void timer_process_active_timers(void);

/*MACRO for locking/unlocking the qmutex*/
#define ATS_INTLOCK()  \
   qurt_pimutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE()  \
   qurt_pimutex_unlock(&timer_qemu_mutex); \

/*===========================================================================
Declarations
===========================================================================*/
int qurt_timer_shim_create (int client_handle, unsigned int process_idx, const qurt_timer_t *timer, const qurt_timer_attr_t *attr,
                  const qurt_anysignal_t *signal, unsigned int mask, timer_notify_type cb_type);

int qurt_timer_shim_delete(qurt_timer_t timer, unsigned int process_idx);
                          
/*===========================================================================
Function Definitions
===========================================================================*/
/*=============================================================================

FUNCTION QURT_TIMER_GROUPS_INITIALIZE

DESCRIPTION
 Initializes Qurt group 0 to be deferrable amd 
 rest all groups to be non-deferrable

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/
static void qurt_timer_groups_initialize
(
  void
)
{
  int group_counter = 1;

  /* The expectation is that only qurt group '0' is deferrable
   All the other groups are non-deferrable */
  timer_group_set_deferrable(&qurt_timer_groups[0], TRUE);
  for(; group_counter < QURT_TIMER_MAX_GROUPS; group_counter++ ) 
  {
    timer_drv_group_set_deferrable(&qurt_timer_groups[group_counter], &qurt_timer_groups[group_counter], FALSE);
  }

  return;
} /* qurt_timer_groups_initialize */

/*=============================================================================

FUNCTION QURT_TIMER_SHIM_GROUP_ENABLE

DESCRIPTION
 Enables the specified qurt timer group.
 Any timer associated with this group will now be added back to the ATS
 timer list
 
DEPENDENCIES
  None

RETURN VALUE
 Returns success

SIDE EFFECTS
  None

=============================================================================*/
int qurt_timer_shim_group_enable (unsigned int group)
{
  timer_error_type status;
  
  if(group < QURT_TIMER_MAX_GROUPS) 
  {
     status = timer_drv_group_enable(&qurt_timer_groups[group]);
     if(status == TE_SUCCESS)
       return QURT_EOK;
     else
       return QURT_EFAILED;
  }
  else
  {
     MSG_ERROR("wrong group no %d", group, 0, 0);
     return QURT_EINVALID;
  }
} /* qurt_timer_shim_group_enable */

/*=============================================================================

FUNCTION QURT_TIMER_SHIM_GROUP_DISABLE

DESCRIPTION
 Disables the specified qurt timer group.
 Any timer associated with this group will be taken off the ATS
 timer list
 
DEPENDENCIES
  None

RETURN VALUE
 Returns success

SIDE EFFECTS
  None

=============================================================================*/
int qurt_timer_shim_group_disable (unsigned int group)
{
  timer_error_type status;
  
  if(group < QURT_TIMER_MAX_GROUPS) 
  {
     status = timer_drv_group_disable(&qurt_timer_groups[group]);
     if(status == TE_SUCCESS)
        return QURT_EOK;
     else
        return QURT_EFAILED;
  }
  else
  {
     MSG_ERROR("wrong group no %d", group, 0, 0);
     return QURT_EINVALID;
  }
} /* qurt_timer_shim_group_disable */

/*=============================================================================

FUNCTION QURT_TIMER_SHIM_GET_TICKS

DESCRIPTION
 Current time 
 
DEPENDENCIES
  None

RETURN VALUE
 Returns the current timetick 

SIDE EFFECTS
  None

=============================================================================*/
int qurt_timer_shim_get_ticks(int client_handle, unsigned long long *ticks) 
{
    time_timetick_type now;
    
    TIMER_QDI_LOCK();
    now = timetick_get_tmp64();
    TIMER_QDI_FREE();

    return qurt_qdi_copy_to_user(client_handle, ticks, &now, sizeof(now));
} /* qurt_timer_shim_get_ticks */

/*=============================================================================

FUNCTION QURT_TIMER_SHIM_GET_ATTR

DESCRIPTION
 Gets the timer attributes for the specified timer
 
DEPENDENCIES
  None

RETURN VALUE
 QURT_EINVALID if the timer specified is a NULL or there is no 
 associated internal timer with the timer specified

 QURT_EOK if the timer is found and the attributes populated

SIDE EFFECTS
  None

=============================================================================*/
int qurt_timer_shim_get_attr(int client_handle, unsigned int process_idx, qurt_timer_t timer, qurt_timer_attr_t *clnt_attr)
{
  /* absolute time tick elapsed */
  time_timetick_type now;
  qurt_timer_attr_t  attr;
  unsigned int group_counter=0;
  boolean is_reload = FALSE;
  timer_group_type group;
  timer_expiry_type expiry_type;
  timer_type timer_handle = timer;

  if(TE_SUCCESS != timer_drv_is_periodic_timer(&timer_handle, process_idx, &is_reload, &attr.duration))
     return QURT_EINVALID;
  
  /* get current absolute time tick */
  now = timetick_get_tmp64();

  /* Fill the magic number */
  attr.magic = QURT_TIMER_ATTR_MAGIC;
  attr.remaining = timer_drv_get_64(&timer_handle, T_USEC, process_idx);

  /* If this is a PERIODIC timer, reload gives the duration */
  if ( is_reload == TRUE )
  {
    /* Set the timer type */
    attr.type     = QURT_TIMER_PERIODIC;
    /* Convert timeticks into microsections */
    attr.duration = QURT_TIMER_TIMETICK_TO_US(timer_drv_expires_at_64(&timer_handle, process_idx) - timer_drv_get_start_64(&timer_handle, process_idx));
  }
  else
  {
    (void)timer_drv_get_expiry_type(&timer_handle, process_idx, &expiry_type);
    
    if(expiry_type == TIMER_ABSOLUTE_EXPIRY)
    {
        /* Convert timeticks into microsections */
        attr.expiry = QURT_TIMER_TIMETICK_TO_US(timer_drv_expires_at_64(&timer_handle, process_idx));
        attr.duration = QURT_TIMER_DEFAULT_DURATION;
    }
    else
    {
       attr.duration = QURT_TIMER_TIMETICK_TO_US(timer_drv_expires_at_64(&timer_handle, process_idx) - timer_drv_get_start_64(&timer_handle, process_idx));
       attr.expiry = QURT_TIMER_DEFAULT_EXPIRY;
    }
    /* Set the timer type */
    attr.type     = QURT_TIMER_ONESHOT;
  }

  (void)timer_drv_get_group(&timer_handle, process_idx, &group);

  /*assign group counter*/
  for(group_counter = 0; group_counter < QURT_TIMER_MAX_GROUPS; group_counter++) 
  {
    if(group == qurt_timer_groups[group_counter]) 
    {
      attr.group = group_counter;
      break;
    }
  }

  qurt_qdi_copy_to_user(client_handle, clnt_attr, &attr, sizeof(attr));
  return QURT_EOK;    
} /* qurt_timer_shim_get_attr */


#if 0 /* Sleep is being done using timer in client space itself */
/*=============================================================================

FUNCTION QURT_TIMER_SHIM_SLEEP

DESCRIPTION
 Sleep function.
 Cause the calling thread to be suspended from execution
 until the specified duration has elapsed.
 
 
DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK:       Successfully entered and exit sleep,
 QURT_EINVALID:   if the timer specified was null

SIDE EFFECTS
  None

=============================================================================*/
int qurt_timer_shim_sleep ( int client_handle, timer_qdi_opener *pobj, qurt_timer_duration_t duration )
{

  /* Parameters to send to message queue server */
  qurt_anysignal_t cb_signal;
  qurt_timer_t     qurt_timer;
  qurt_timer_attr_t attr;
  int status = QURT_EINVALID;
        
  /* Verify duration with min & max timer duration */
  if ( QURT_TIMER_MIN_DURATION > duration ||
          QURT_TIMER_MAX_DURATION < duration )
  {
    MSG_ERROR("Invalid duration", 0, 0, 0);
    return status;
  }
    
  /* Initialize the qurt signal */
  qurt_anysignal_init (&cb_signal);
  qurt_timer_attr_init(&attr);
  qurt_timer_attr_set_duration(&attr, duration);
  qurt_timer_attr_set_group(&attr, 0);
  qurt_timer_attr_set_type(&attr, QURT_TIMER_ONESHOT);

  qurt_timer = qurt_timer_shim_create(client_handle, process_idx, &attr, &cb_signal, QURT_TIMER_CLIENT_ACK_SIGMASK, TIMER_NATIVE_OS_SIGNAL_TYPE);
  if(0 == qurt_timer)
  {
     MSG_ERROR("couldnt create timer", 0, 0, 0);
     return status;
  }
  
  qurt_anysignal_wait (&cb_signal, QURT_TIMER_CLIENT_ACK_SIGMASK);

  /* delete the timer */
  qurt_timer_shim_delete(qurt_timer);
  /* Release the resources allocated by the signal */
  qurt_anysignal_destroy (&cb_signal);

  return QURT_EOK;
} /* qurt_timer_shim_sleep */
#endif /* #if 0 */

/*=============================================================================

FUNCTION QURT_TIMER_SHIM_RESTART

DESCRIPTION
 Restart a one-shot timer with a duration. 

DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK:        Successful restart
 QURT_EINVALID:   Invalid timer ID
 QURT_ENOTALLOWED: Timer is not a oneshot timer.
 QURT_EMEM:        Out of memory error,

SIDE EFFECTS
  None

=============================================================================*/
int qurt_timer_shim_restart (qurt_timer_t timer, unsigned int process_idx, qurt_timer_duration_t time)
{
  timer_type timer_handle = NULL;
  timer_error_type status;
  timer_expiry_type expiry_type;
  boolean is_periodic;

  ATS_INTLOCK();
  timer_handle = (timer_type)timer;
  
  if(TE_SUCCESS != timer_drv_is_periodic_timer(&timer_handle, process_idx, &is_periodic, NULL))
  {
     ATS_INTFREE();
     return QURT_EINVALID;
  }
     
  if(is_periodic == TRUE)
  {
     ATS_INTFREE();
     return QURT_ENOTALLOWED;
  }

  /* Already timer validity is checked above */
  (void) timer_drv_get_expiry_type(&timer_handle, process_idx, &expiry_type);

  /* Restart the expired timer */
  if(expiry_type == TIMER_ABSOLUTE_EXPIRY)
  {
     status = timer_drv_set_absolute(&timer_handle, QURT_TIMER_TIMETICK_FROM_US(time), process_idx); 
  }
  else
  {
    /* Verify duration with min & max timer duration */
    if ( time < QURT_TIMER_MIN_DURATION || time > QURT_TIMER_MAX_DURATION )
    {
      ATS_INTFREE();
      return QURT_EINVALID;
    }

    status = timer_drv_set_64(&timer_handle, time, 0, T_USEC, process_idx);
  }
  ATS_INTFREE();
  
  if(status != TE_SUCCESS)
    return QURT_EFAILED;
  else
    return QURT_EOK;
} /* qurt_timer_shim_restart */

/*=============================================================================

FUNCTION QURT_TIMER_SHIM_DELETE

DESCRIPTION
 Deletes timer
 
DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK:  Successful create,
 EFAILED:   If null timer is specified,

SIDE EFFECTS
  None

=============================================================================*/
int qurt_timer_shim_delete(qurt_timer_t timer, unsigned int process_idx)
{
  int result = QURT_EFAILED;
  timer_error_type status;

  timer_type timer_handle = (timer_type) timer;

  status = timer_drv_undef(&timer_handle, process_idx);
  if(status == TE_SUCCESS ||
     status == TE_INVALID_TIMER_HANDLE ||
     status == TE_TIMER_ALREADY_IN_SAME_STATE)
     result = QURT_EOK;

  return result;
} /* qurt_timer_shim_delete */

/*=============================================================================

FUNCTION QURT_TIMER_SHIM_STOP

DESCRIPTION
 Stop a one-shot timer. 

DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK:        Successful stop
 QURT_EINVALID:   Invalid timer ID
 QURT_ENOTALLOWED: Timer is not a oneshot timer.
 QURT_EMEM:        Out of memory error,

SIDE EFFECTS
  None

=============================================================================*/
int qurt_timer_shim_stop (qurt_timer_t timer, unsigned int process_idx)
{
  timer_type              timer_handle = NULL; 
  int                     result = QURT_EFAILED;
  timer_error_type        status;
  boolean                 is_reload = FALSE;
  uint64                  rem_ticks;
  
  timer_handle = (timer_type)timer;

  ATS_INTLOCK();
    /* Todo: Actually reload timer should also be able to stop. 
             Talk to Qurt team regarding this */
    if(TE_SUCCESS != timer_drv_is_periodic_timer(&timer_handle, process_idx, &is_reload, NULL))
    {
       ATS_INTFREE();
       return QURT_EINVALID;
    }
       
    if(is_reload == TRUE) 
    {
       ATS_INTFREE();
       return QURT_ENOTALLOWED;
    }

    status = timer_drv_clr_64(&timer_handle, T_TICK, process_idx, &rem_ticks);
    ATS_INTFREE();
	
    if(status == TE_TIMER_ALREADY_IN_SAME_STATE ||
	   status == TE_TIMER_NOT_ACTIVE ||
	   status == TE_SUCCESS)
       result = QURT_EOK;
       
    return result;
} /* qurt_timer_shim_stop */

/*=============================================================================

FUNCTION QURT_TIMER_SHIM_CREATE

DESCRIPTION
 Creates timer using attributes and provides the handle

DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK if the timer is defined and set correctly.

SIDE EFFECTS
  None

=============================================================================*/
int qurt_timer_shim_create (int client_handle, unsigned int process_idx, const qurt_timer_t *timer, const qurt_timer_attr_t *clnt_attr,
                  const qurt_anysignal_t *signal, unsigned int mask, timer_notify_type cb_type)
{
  int result = QURT_EINVALID;
  timer_type  timer_handle = 0;
  timer_group_ptr timer_grp = NULL;
  time_timetick_type   reload = 0;
  timer_error_type   status;
  qurt_timer_attr_t  attr;
  ATS_INTLOCK();
  
  /*init the qurt groups if not already initialized*/
  if(FALSE == qurt_timer_groups_init) 
  {
    qurt_timer_groups_initialize();  
    qurt_timer_groups_init = TRUE;
  }
  
  result = qurt_qdi_copy_from_user(client_handle, &attr, clnt_attr, sizeof(attr));
  if(result == -1)
  {
    ATS_INTFREE();
    return 0;
  }
  
  timer_grp = &qurt_timer_groups[attr.group];  

  if(QURT_TIMER_ONESHOT != attr.type ) 
  {
    reload = attr.duration;
  }

   status = timer_drv_def_osal(&timer_handle, (timer_ptr_type) timer, timer_grp, timer_grp, cb_type, (time_osal_notify_obj_ptr) signal, 
                          (time_osal_notify_data) mask, process_idx);
   if(status != TE_SUCCESS)
   {
      ATS_INTFREE();
      return 0;
   }

  /* If the timer is supposed to be expiring at a particular timetick */
  if(attr.expiry != QURT_TIMER_DEFAULT_EXPIRY ) 
  {
    status = timer_drv_set_absolute(&timer_handle, QURT_TIMER_TIMETICK_FROM_US(attr.expiry), process_idx);      
  }
  else
  {
    status = timer_drv_set_64(&timer_handle, attr.duration, reload, T_USEC, process_idx);
  }
  
  if(status != TE_SUCCESS)
  {
     ATS_INTFREE();
     return 0;
  }

  ATS_INTFREE();
  return (unsigned int)timer_handle;
} /* qurt_timer_shim_create */

/*=============================================================================

FUNCTION QURT_TIMER_SHIM_RECOVER_PC

DESCRIPTION
 Forces iteration through the active timer list
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/
void qurt_timer_shim_recover_pc (void)
{
  timer_process_active_timers();
} /* qurt_timer_shim_recover_pc */