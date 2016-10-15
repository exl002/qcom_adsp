/*=============================================================================

                 timer_qdi.c

GENERAL DESCRIPTION
   Implements QDI layer for ATS Timers

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010 - 2013
   by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.adsp/2.2/services/time/src/timer_qdi.c#8 $ 
$DateTime: 2013/10/28 12:05:15 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/24/12   abalanag   File created.

=============================================================================*/


/*****************************************************************************/
/*                           INCLUDE FILES                                   */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>
#include "qurt.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt_pimutex.h"
#include "timer.h"
#include "timer_v.h"
#include "timer_qdi_v.h"
#include "qurt_timer_qdi_v.h"
#include "msg.h"
#include "err.h"

/*===========================================================================
Data Structures
===========================================================================*/

#define TIMER_CLIENT_PIPE_SIZE     75
#define TIMER_MAX_CLIENTS          8
static timer_qdi_opener *timer_clients[TIMER_MAX_CLIENTS];
uint32 timer_clients_cntr = 0;

qurt_mutex_t timer_qdi_mutex;
/*MACRO for locking/unlocking the qmutex*/
#define TIMER_QDI_LOCK()  \
   qurt_pimutex_lock(&timer_qdi_mutex); \
  
#define TIMER_QDI_FREE()  \
   qurt_pimutex_unlock(&timer_qdi_mutex); \

/* extern variables */
/* Pointer to the mutex object for protecting critical-section of timer 
   code */
extern qurt_mutex_t timer_qemu_mutex;
extern void timer_process_active_timers(void);

/*MACRO for locking/unlocking the qmutex*/
#define ATS_INTLOCK()  \
   qurt_pimutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE()  \
   qurt_pimutex_unlock(&timer_qemu_mutex); \

#define ATS_INTLOCK_SAV(X) \
   qurt_pimutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE_SAV(X)   \
   qurt_pimutex_unlock(&timer_qemu_mutex); \

#define TIMER_TRACE_MUTEX_LOCK()  \
   qurt_pimutex_lock(&timer_trace_mutex); \
  
#define TIMER_TRACE_MUTEX_UNLOCK()  \
   qurt_pimutex_unlock(&timer_trace_mutex); \

/*===========================================================================
Declarations
===========================================================================*/
static int timer_qdi_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9);
                          
#ifdef TIMER_DELAY_PROFILE
/*****************************************************************************/
/*                      FUNCTION DEFINITIONS                                 */
/*****************************************************************************/
uint32 qurt_timer_test_get_cpu_freq
(
  void
)
{
  uint32 cpu_freq = -1;
  if( NULL != timer_test_CpuQueryHandle )
  {
          /* Get the current CPU frequency. */
    if( NPA_QUERY_SUCCESS == npa_query( timer_test_CpuQueryHandle,
                                                 NPA_QUERY_CURRENT_STATE,
                                                 &qres ) );
    cpu_freq = qres.data.value;
  }
  else /*try to initialize the handle now*/
  {
    timer_test_CpuQueryHandle = npa_create_query_handle("/clk/cpu");
  }

  return cpu_freq;
}
#endif /* TIMER_DELAY_PROFILE */


/*===========================================================================
Pipe Apis
===========================================================================*/
int timer_qdi_create_pipe(timer_qdi_opener *clntobj)
{
   qurt_pipe_attr_t pipe_attr;
   int ret;
   
   //Create Pipe for the pid
   qurt_pipe_attr_init(&pipe_attr);
   qurt_pipe_attr_set_elements(&pipe_attr, TIMER_CLIENT_PIPE_SIZE);
   ret = qurt_pipe_create(&clntobj->pipe, &pipe_attr);

   return ret;
} /* timer_qdi_create_pipe */

/*===========================================================================
Function Definitions
===========================================================================*/

/*=============================================================================

FUNCTION TIMER_QDI_GET_CB_INFO

DESCRIPTION
 Client Worker thread calls into this function and waits for callback data from pipe

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  This is a Blocking call

=============================================================================*/
int timer_qdi_get_cb_info(int client_handle, timer_qdi_opener *obj, timer_cb_info *cb)
{
   qurt_pipe_data_t data1, data2, data3;
   qurt_pipe_data_t *cb_ptr;
   int result;
   
   cb_ptr = (qurt_pipe_data_t *)cb;
   
   /* Read callback details from the pipe. 
      As timer_cb_info is 4words, reading pipe twice */
   data1 = qurt_pipe_receive(obj->pipe);
   data2 = qurt_pipe_receive(obj->pipe);
   data3 = qurt_pipe_receive(obj->pipe);
   
   result = qurt_qdi_copy_to_user(client_handle, cb_ptr, &data1, sizeof(data1));
   if(result >= 0)
      result = qurt_qdi_copy_to_user(client_handle, ++cb_ptr, &data2, sizeof(data2));
   if(result >= 0)
      result = qurt_qdi_copy_to_user(client_handle, ++cb_ptr, &data3, sizeof(data3));
   
   return result;
} /* timer_qdi_get_cb_info */

/*=============================================================================

FUNCTION TIMER_QDI_REMOTE_HANDLER

DESCRIPTION
  Timer Expire function to push timer cb details into corresponding process pipe.
 
 
DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

=============================================================================*/
void timer_qdi_remote_handler(uint32 process_idx, timer_ptr_type timer, timer_notify_type cb_type,
                              time_osal_notify_obj_ptr sigs_func_addr, time_osal_notify_data sigs_mask_data,
                              time_timetick_type run_time_ms)
{
   qurt_pipe_data_t data1 = 0;
   qurt_pipe_data_t data2 = 0;
   qurt_pipe_data_t data3 = 0;
   qurt_pipe_data_t *ptr = 0;
   timer_cb_info    temp_cb;
   static uint32    sequence = 0; 
   
   if(process_idx >= TIMER_MAX_CLIENTS)
      ERR_FATAL("ProcessIdx %d >= %d func 0x%x", process_idx, TIMER_MAX_CLIENTS, sigs_func_addr);
   
   TIMER_QDI_LOCK();
   if(timer_clients[process_idx] == NULL)
   {
      /* Process got cleared or deleted, ignoring the timers related to that process */
      return;
   }
   TIMER_QDI_FREE();

   temp_cb.timer   = timer;
   temp_cb.seq     = sequence++;    /* Just dummy */
   temp_cb.cb_type = cb_type;
   temp_cb.sig_func_addr = (uint32)sigs_func_addr;
   temp_cb.mask_data1 = (uint32)sigs_mask_data;
   temp_cb.data2 = (uint32)run_time_ms;
   
   ptr = (qurt_pipe_data_t *)&temp_cb;
   memscpy(&data1, sizeof(data1), (void *)ptr, sizeof(data1));
   memscpy(&data2, sizeof(data2), (void *)(++ptr), sizeof(data2));
   memscpy(&data3, sizeof(data3), (void *)(++ptr), sizeof(data3));

   /* Todo: Need to see if there is a way, where i can figure out pipe is not full */
   qurt_pipe_send((qurt_pipe_t *)timer_clients[process_idx]->pipe, data1);
   /* As first data is successfully sent, data2 should be able to send */
   qurt_pipe_send((qurt_pipe_t *)timer_clients[process_idx]->pipe, data2);
      /* As first data is successfully sent, data3 should be able to send */
   qurt_pipe_send((qurt_pipe_t *)timer_clients[process_idx]->pipe, data3);
   
   return;
} /* timer_qdi_remote_handler */

/*=============================================================================

                   TIMER API QDI Functions

=============================================================================*/

static inline timer_error_type timer_qdi_group_set_deferrable(int client_handle, timer_group_ptr pgroup, boolean condition)
{
   timer_group_type group;
   int              result;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   status = timer_drv_group_set_deferrable(&group, pgroup, condition);
   
   result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_group_set_deferrable */

static inline timer_error_type timer_qdi_group_enable(int client_handle, timer_group_ptr pgroup)
{
   timer_group_type group;
   int              result;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   status = timer_drv_group_enable(&group);
   
   result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_group_enable */

static inline timer_error_type timer_qdi_group_disable(int client_handle, timer_group_ptr pgroup)
{
   timer_group_type group;
   int              result;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   status = timer_drv_group_disable(&group);
   
   result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_group_disable */

static inline timer_error_type timer_qdi_get_time_till_first_non_deferrable_timer_64(int client_handle, time_timetick_type *ptick)
{
   time_timetick_type expiry_time;
   int                result;
   
   expiry_time = timer_drv_get_time_till_first_non_deferrable_timer_64();
   result = qurt_qdi_copy_to_user(client_handle, ptick, &expiry_time, sizeof(time_timetick_type));

   if(result)
      return TE_INVALID_PARAMETERS;
   return TE_SUCCESS;
} /* timer_qdi_get_time_till_first_non_deferrable_timer */

static inline timer_error_type timer_qdi_defer_match_interrupt_64(int client_handle, time_timetick_type *ptick)
{
   time_timetick_type expiry_time;
   int                result;
   
   expiry_time = timer_drv_defer_match_interrupt_64();
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &expiry_time, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
   
   return TE_SUCCESS;   
} /* timer_qdi_defer_match_interrupt */

static inline void timer_qdi_undefer_match_interrupt(void)
{
   timer_drv_undefer_match_interrupt();
} /* timer_qdi_undefer_match_interrupt */

static inline timer_error_type timer_qdi_def_osal(int client_handle, timer_ptr_type ptimer, timer_group_ptr pgroup, 
                               timer_notify_type cb_type, time_osal_notify_obj_ptr sigs_func_addr,
                               time_osal_notify_data sigs_mask_data, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   timer_group_type group;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
   
   if(pgroup != NULL)
   {
      result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
      if(result)
      {
         ATS_INTFREE();
         return TE_INVALID_PARAMETERS;
      }
         
      status = timer_drv_def_osal(&timer, ptimer, &group, pgroup, cb_type, sigs_func_addr, sigs_mask_data, process_idx);
   }
   else
   {
      status = timer_drv_def_osal(&timer, ptimer, NULL, NULL, cb_type, sigs_func_addr, sigs_mask_data, process_idx);
   }
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   if(pgroup != NULL)
   {
      result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
      if(result)
      {
         ATS_INTFREE();
         return TE_INVALID_PARAMETERS;
      }
   }
   
   ATS_INTFREE();
   return status;
} /* timer_qdi_def_osal */

static inline timer_error_type timer_qdi_def2(int client_handle, timer_ptr_type ptimer,
                                              timer_group_ptr pgroup, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   timer_group_type group;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   if(pgroup != NULL)
   {
      result = qurt_qdi_copy_from_user(client_handle, &group, pgroup, sizeof(timer_group_type));
      if(result)
      {
         ATS_INTFREE();
         return TE_INVALID_PARAMETERS;
      }
         
      status = timer_drv_def2(&timer, ptimer, &group, pgroup, process_idx);
   }
   else
   {
      status = timer_drv_def2(&timer, ptimer, NULL, NULL, process_idx);
   }
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   if(pgroup != NULL)
   {
      result = qurt_qdi_copy_to_user(client_handle, pgroup, &group, sizeof(timer_group_type));
      if(result)
      {
         ATS_INTFREE();
         return TE_INVALID_PARAMETERS;
      }
   }
   
   ATS_INTFREE();
   return status;
} /* timer_qdi_def2 */

static inline timer_error_type timer_qdi_reg(int client_handle, timer_ptr_type ptimer, 
                               timer_t2_cb_type func, timer_cb_data_type data,
                               timetick_type time_ms, timetick_type reload_ms, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }

   status = timer_drv_reg(&timer, func, data, time_ms, reload_ms, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));

   ATS_INTFREE();

   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_reg */

static inline timer_error_type timer_qdi_set_64(int client_handle, timer_ptr_type ptimer, time_timetick_type time, 
                                             time_timetick_type reload, timer_unit_type     unit, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }

   status = timer_drv_set_64(&timer, time, reload, unit, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_set */

static inline timer_error_type timer_qdi_set_absolute(int client_handle, timer_ptr_type ptimer, time_timetick_type time, unsigned int process_idx)
{
   int result;
   timer_error_type status;
   timer_type timer;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }

   status = timer_drv_set_absolute(&timer, time, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_set_absolute */

static inline timer_error_type timer_qdi_get_64(int client_handle, timer_ptr_type ptimer, timer_unit_type     unit,
                                             unsigned int process_idx, time_timetick_type *ptick)
{
   int result;
   timer_type timer;
   time_timetick_type tick;
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
      return TE_INVALID_PARAMETERS;
   
   /* Call timer_drv_get */
   tick = timer_drv_get_64(&timer, unit, process_idx);
   
   /* Note: Get apis doesnt change any thing in client timer structure.. Therefore, copy_to_user for timer is not needed */
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   return TE_SUCCESS;
} /* timer_qdi_get */

static inline boolean timer_qdi_is_active(int client_handle, timer_ptr_type ptimer, unsigned int process_idx)
{
   int result;
   boolean is_active;
   timer_type timer;
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
      return FALSE;

   is_active = timer_drv_is_active(&timer, process_idx);
   
   /* Note: Get apis doesnt change any thing in client timer structure.. Therefore, copy_to_user for timer is not needed */
   
   return is_active;
} /* timer_qdi_is_active */

static inline timer_error_type timer_qdi_expires_at_64(int client_handle, timer_ptr_type ptimer, 
                                                       unsigned int process_idx, time_timetick_type *ptick)
{
   int result;
   timer_type timer;
   time_timetick_type tick;
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   tick = timer_drv_expires_at_64(&timer, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   return TE_SUCCESS;
} /* timer_qdi_expires_at */

static inline timer_error_type timer_qdi_get_start_64(int client_handle, timer_ptr_type ptimer, 
                                                      unsigned int process_idx, time_timetick_type *ptick)
{
   int result;
   timer_type timer;
   time_timetick_type tick;
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   tick = timer_drv_get_start_64(&timer, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   return TE_SUCCESS;
} /* timer_qdi_get_start */

static inline timer_error_type timer_qdi_clr_64(int client_handle, timer_ptr_type ptimer, 
                                                timer_unit_type     unit, unsigned int process_idx, 
                                                time_timetick_type *ptick)
{
   int result;
   timer_type timer;
   timer_error_type status;
   time_timetick_type tick;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   status = timer_drv_clr_64(&timer, unit, process_idx, &tick);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   
   ATS_INTFREE();
      
   if(result)
   {
      return TE_INVALID_PARAMETERS;
   }

   if(ptick != NULL)
   {
      result = qurt_qdi_copy_to_user(client_handle, ptick, &tick, sizeof(time_timetick_type));
      if(result)
      {
         return TE_INVALID_PARAMETERS;
      }
   }
   
   return status;
} /* timer_qdi_clr */

static inline timer_error_type timer_qdi_undef(int client_handle, timer_ptr_type ptimer, unsigned int process_idx)
{
   int result;
   timer_type timer;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   status = timer_drv_undef(&timer, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_undef */

static inline timer_error_type timer_qdi_pause(int client_handle, timer_ptr_type ptimer, unsigned int process_idx)
{
   int result;
   timer_type timer;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }
      
   status = timer_drv_pause(&timer, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_pause */

static inline timer_error_type timer_qdi_resume(int client_handle, timer_ptr_type ptimer, unsigned int process_idx)
{
   int result;
   timer_type timer;
   timer_error_type status;
   
   ATS_INTLOCK();
   
   result = qurt_qdi_copy_from_user(client_handle, &timer, ptimer, sizeof(timer_type));
   if(result)
   {
      ATS_INTFREE();
      return TE_INVALID_PARAMETERS;
   }   
   status = timer_drv_resume(&timer, process_idx);
   
   result = qurt_qdi_copy_to_user(client_handle, ptimer, &timer, sizeof(timer_type));
   
   ATS_INTFREE();
   
   if(result)
      return TE_INVALID_PARAMETERS;
   return status;
} /* timer_qdi_resume */

static inline void timer_qdi_clr_osal_thread_timers(time_osal_thread_id thread_id)
{
   timer_drv_clr_osal_thread_timers(thread_id);
} /* timer_qdi_clr_osal_thread_timers */

static inline timer_error_type timer_qdi_get_sclk_till_expiry_64(int client_handle, time_timetick_type *ptick)
{
   time_timetick_type expiry_time;
   int                result;
   
   expiry_time = timer_drv_get_sclk_till_expiry_64();
   result = qurt_qdi_copy_to_user(client_handle, ptick, &expiry_time, sizeof(time_timetick_type));
   if(result)
      return TE_INVALID_PARAMETERS;
      
   return TE_SUCCESS;
} /* timer_qdi_get_sclk_till_expiry */

static inline timer_error_type timer_qdi_set_wakeup_64(time_timetick_type tick)
{
   return timer_drv_set_wakeup_64(tick);
} /* timer_qdi_set_wakeup */

static inline void timer_qdi_restart(void)
{
   timer_drv_restart();
} /* timer_qdi_restart */


/*=============================================================================

FUNCTION TIMER_QDI_RELEASE

DESCRIPTION
 Releases the Qdi object that corresponding to a process that got destroyed

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  Timers related to this process will be ignored from here

=============================================================================*/
void timer_qdi_release(qurt_qdi_obj_t *obj)
{
   timer_qdi_opener *clntobj = (timer_qdi_opener *)obj;
   uint32 process_idx = clntobj->process_idx;
   
   /* Note: Remove INTLOCK and use LOCK */
   TIMER_QDI_LOCK();
   if(clntobj->pipe)
   {
      qurt_pipe_delete(clntobj->pipe);
   }

   /* Free the object related to this process */
   free(timer_clients[process_idx]);
   timer_clients[process_idx] = NULL;

   timer_drv_delete_process_timers(process_idx);
  
   TIMER_QDI_FREE();
   return;
} /* timer_qdi_release */


/*=============================================================================

FUNCTION TIMER_QDI_OPEN

DESCRIPTION
 Creates timer driver object for the client and initializes required details

DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK if the timer is defined and set correctly.

SIDE EFFECTS
  None

=============================================================================*/
int timer_qdi_open(int client_handle, timer_qdi_opener *me)
{
   timer_qdi_opener *clntobj;
   int status;
   
   TIMER_QDI_LOCK();
   if(timer_clients_cntr == TIMER_MAX_CLIENTS)
   {
      TIMER_QDI_FREE();
      ERR_FATAL("Max Processes %d reached", timer_clients_cntr, 0, 0);
      return -1;
   }
   TIMER_QDI_FREE();
   
   clntobj = malloc(sizeof(timer_qdi_opener));
   if(NULL == clntobj)
   {
      printf("malloc err for %d bytes", sizeof(timer_qdi_opener));
      return -1;
   }
   
   clntobj->qdiobj.invoke = timer_qdi_invoke;
   clntobj->qdiobj.refcnt = QDI_REFCNT_INIT;
   clntobj->qdiobj.release = timer_qdi_release;
   
   /* Create Pipe only for remote processes */
   if(client_handle != QDI_HANDLE_LOCAL_CLIENT)
   {      
      status = timer_qdi_create_pipe(clntobj);
      if(status == QURT_EFAILED)
      {
         free(clntobj);
         return -1;
      }
   }
   else
   {
      clntobj->pipe = NULL;
      timer_drv_set_local_process_idx(timer_clients_cntr);
   }
   
   TIMER_QDI_LOCK();
   timer_clients[timer_clients_cntr] = clntobj;
   clntobj->process_idx = timer_clients_cntr++;
   TIMER_QDI_FREE();

   return qurt_qdi_handle_create_from_obj_t(client_handle, &clntobj->qdiobj);
} /* timer_qdi_open */

      extern int qurt_timer_shim_create (int client_handle, unsigned int process_idx, const qurt_timer_t *timer, const qurt_timer_attr_t *attr,
                  const qurt_anysignal_t *signal, unsigned int mask, timer_notify_type cb_type);
      extern int qurt_timer_shim_stop (qurt_timer_t timer, unsigned int process_idx);
      extern int qurt_timer_shim_delete(qurt_timer_t timer, unsigned int process_idx);
      extern int qurt_timer_shim_restart (qurt_timer_t timer, unsigned int process_idx, qurt_timer_duration_t time );
      //extern int qurt_timer_shim_sleep ( int client_handle, timer_qdi_opener *pobj, qurt_timer_duration_t duration );
      extern int qurt_timer_shim_get_attr(int client_handle, unsigned int process_idx, qurt_timer_t timer, qurt_timer_attr_t *clnt_attr);
      extern int qurt_timer_shim_get_ticks(int client_handle, unsigned long long *ticks);
      extern int qurt_timer_shim_group_enable (unsigned int group);
      extern int qurt_timer_shim_group_disable (unsigned int group);
      extern void qurt_timer_shim_recover_pc (void);


/*=============================================================================

FUNCTION TIMER_QDI_INVOKE

DESCRIPTION
 This function contains the switch which maps to all methods

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

=============================================================================*/
static int timer_qdi_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9)
{
   timer_qdi_opener *clntobj = (timer_qdi_opener *)pobj;
   unsigned int      process_idx = clntobj->process_idx;
   
   switch(method)
   {
      case QDI_OPEN: 
         return timer_qdi_open(client_handle, clntobj);
      
      case TIMER_QDI_GROUP_SET_DEFERRABLE: 
         return timer_qdi_group_set_deferrable(client_handle, a1.ptr, a2.num);
         
      case TIMER_QDI_GROUP_ENABLE: 
         return timer_qdi_group_enable(client_handle, a1.ptr);
         
      case TIMER_QDI_GROUP_DISABLE:  
         return timer_qdi_group_disable(client_handle, a1.ptr);
      
      case TIMER_QDI_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER_64:  
         return timer_qdi_get_time_till_first_non_deferrable_timer_64(client_handle, a1.ptr); 
         
      case TIMER_QDI_DEFER_MATCH_INTERRUPT_64: 
         return timer_qdi_defer_match_interrupt_64(client_handle, a1.ptr); 
         
      case TIMER_QDI_UNDEFER_MATCH_INTERRUPT: 
         timer_qdi_undefer_match_interrupt(); 
         return TE_SUCCESS;
      
      case TIMER_QDI_DEF_OSAL:  
         return timer_qdi_def_osal(client_handle, a1.ptr, a2.ptr, a3.num, a4.ptr, a5.num, process_idx);
         
      case TIMER_QDI_DEF2: 
         return timer_qdi_def2(client_handle, a1.ptr, a2.ptr, process_idx);
         
      case TIMER_QDI_REG:   
         return timer_qdi_reg(client_handle, a1.ptr, a2.ptr, a3.num, a4.num, a5.num, process_idx);
         
      case TIMER_QDI_SET_64:  
         return timer_qdi_set_64(client_handle, a1.ptr, ((time_timetick_type)a2.num<<32 | a3.num), ((time_timetick_type)a4.num<<32|a5.num), a6.num, process_idx);
         
      case TIMER_QDI_SET_ABSOLUTE: 
         return timer_qdi_set_absolute(client_handle, a1.ptr, ((time_timetick_type)a2.num<<32 | a3.num), process_idx);
         
      case TIMER_QDI_GET_64: 
         return timer_qdi_get_64(client_handle, a1.ptr, a2.num, process_idx, a3.ptr); 
         
      case TIMER_QDI_IS_ACTIVE: 
         return timer_qdi_is_active(client_handle, a1.ptr, process_idx);
         
      case TIMER_QDI_EXPIRES_AT_64: 
         return timer_qdi_expires_at_64(client_handle, a1.ptr, process_idx, a2.ptr); 
         
      case TIMER_QDI_GET_START_64: 
         return timer_qdi_get_start_64(client_handle, a1.ptr, process_idx, a2.ptr); 
         
      case TIMER_QDI_CLR_64: 
         return timer_qdi_clr_64(client_handle, a1.ptr, a2.num, process_idx, a3.ptr); 
         
      case TIMER_QDI_UNDEF: 
         return timer_qdi_undef(client_handle, a1.ptr, process_idx);
         
      case TIMER_QDI_PAUSE: 
         return timer_qdi_pause(client_handle, a1.ptr, process_idx);
         
      case TIMER_QDI_RESUME: 
         return timer_qdi_resume(client_handle, a1.ptr, process_idx);
         
      case TIMER_QDI_CLR_OSAL_THREAD_TIMERS: 
         timer_qdi_clr_osal_thread_timers(a1.num); 
         return TE_SUCCESS;
         
      case TIMER_QDI_GET_SCLK_TILL_EXPIRY_64: 
         return timer_qdi_get_sclk_till_expiry_64(client_handle, a1.ptr); 
         
      case TIMER_QDI_SET_WAKEUP_64: 
         return timer_qdi_set_wakeup_64(((time_timetick_type)a1.num <<32 | a2.num));
         
      case TIMER_QDI_RESTART: 
         timer_qdi_restart(); 
         return TE_SUCCESS;

      case TIMER_QDI_GET_CB_INFO: return timer_qdi_get_cb_info(client_handle, clntobj, a1.ptr);
      
      /*=============================================================================
                                     QURT TIMER METHODS
      =============================================================================*/
      case TIMER_QDI_QURT_CREATE: return qurt_timer_shim_create(client_handle, process_idx, a1.ptr, a2.ptr, a3.ptr, a4.num, TIMER_NATIVE_OS_SIGNAL_TYPE);
      case TIMER_QDI_QURT_CREATE_SIG2: return qurt_timer_shim_create(client_handle, process_idx, a1.ptr, a2.ptr, a3.ptr, a4.num, TIMER_NATIVE_OS_SIGNAL2_TYPE);
      case TIMER_QDI_QURT_STOP: return qurt_timer_shim_stop(a1.num, process_idx);
      case TIMER_QDI_QURT_DELETE: return qurt_timer_shim_delete(a1.num, process_idx); 
      case TIMER_QDI_QURT_RESTART: 
      {
         qurt_timer_duration_t duration = ((unsigned long long)a2.num << 32) + a3.num;
         return qurt_timer_shim_restart(a1.num, process_idx, duration);
      }
      
      #if 0 /* Sleep is being done using timer in client space itself */
      case TIMER_QDI_QURT_SLEEP: 
      {
         qurt_timer_duration_t duration = ((unsigned long long)a1.num << 32) + a2.num;
         return qurt_timer_shim_sleep(client_handle, clntobj, duration);
      }
      #endif /* #if 0 */
      
      case TIMER_QDI_QURT_GET_ATTR: return qurt_timer_shim_get_attr(client_handle, process_idx, a1.num, a2.ptr);
      case TIMER_QDI_QURT_GET_TICKS: return qurt_timer_shim_get_ticks(client_handle, (unsigned long long *)a1.ptr);
      case TIMER_QDI_QURT_GROUP_ENABLE: return qurt_timer_shim_group_enable(a1.num);
      case TIMER_QDI_QURT_GROUP_DISABLE: return qurt_timer_shim_group_disable(a1.num);
      case TIMER_QDI_QURT_RECOVER_PC:
      {
         qurt_timer_shim_recover_pc();
         return QURT_EOK;
      }


      default:
         return qurt_qdi_method_default(client_handle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
   }
} /* timer_qdi_invoke */


/*=============================================================================

FUNCTION TIMER_QDI_INIT

DESCRIPTION
 Registers with QDI Framework for ATS Timers
 Registers callback handler that sends timer callback details to user process.
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/
void timer_qdi_init 
(
  void
) 
{
  timer_qdi_opener *p_opener;

  /* initialize the mutex here */
  qurt_pimutex_init ( &timer_qdi_mutex );

   p_opener = (timer_qdi_opener *)malloc(sizeof(timer_qdi_opener));
   if(NULL == p_opener)
   {
      printf("malloc err for %d bytes", sizeof(timer_qdi_opener));
      return;
   }
   
   p_opener->qdiobj.invoke = timer_qdi_invoke;
   p_opener->qdiobj.refcnt = QDI_REFCNT_INIT;
   p_opener->qdiobj.release = timer_qdi_release;
   qurt_qdi_register_devname(TIMER_DRIVER_NAME, p_opener);
   
   /* Register Process Callback Handler with ATS Timers. 
      Assumption is timer_qdi_open() gets called first for local process */
   timer_drv_set_remote_process_cb_handler(timer_qdi_remote_handler);
} /* timer_qdi_init */