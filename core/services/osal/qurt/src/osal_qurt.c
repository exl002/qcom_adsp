/***********************************************************************
 * OSAL_qurt.c
 * Description: This file contains the implementation of the OS abstraction
 * layer for QURT.
 * Copyright (C) 2011-2013 QUALCOMM Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                         EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.adsp/2.2/services/osal/qurt/src/osal_qurt.c#6 $ $DateTime: 2014/03/20 03:17:41 $ $Author: coresvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-02-13    rh   Made TE_TIMER_ALREADY_IN_SAME_STATE not an error in osal_reset_timer
2013-10-03    ph   OSAL APIs updated to use ATS timers instead of qurt timers.
2013-06-10    ph   Destroy the signal initialized in create thread at the time
                   delete of thread happens.
2013-06-10    sm   Fixed compiler warnings that are now errors, fixed
                   osal_get_remaining_time() to return correct value.
2013-02-10    sg   Fixed Klock work warnings
2012-08-20    sg   Create a thread only when we have a free entry in the qurt_tcb_map_table
2012-08-10    sg   Updated thread id in tcb ptr with proper value of thread id
                   while creating thread
2011-09-28    is   Support osal_create_diag_timer() and osal_create_diag_timer_ex() APIs
2011-09-15    sg   Created

===========================================================================*/


#include "osal.h"
#include <stdlib.h>
#include <stdio.h>


static tcb_map_table    qurt_tcb_map_table[QURT_MAX_THREADS];

/* mutex to protect the tcb_map_table */
static qurt_mutex_t         tcb_map_table_lock;

/* Called just once, the first time this OSAL is used.
Initializes the mutexes. Can be used in future for other
initializations */
void osal_init (void);
static int thread_count = 0;
/*Mutex for diag timers */
static osal_mutex_arg_t diag_timer_mutex;

/*common timer group for diag timers */
static osal_timer_group diag_timer_group;

/* Qube functions implementation
 *=========================
 * Synchronization
 * Critical sections
 *=========================*/
 /*===========================================================================
FUNCTION - OSAL_INIT_CRIT_SECT */
/*!
  @brief Initializes a critical section

  @param[in] crit_sect - Pointer to a critical section object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/* ===========================================================================*/
int osal_init_crit_sect(osal_crit_sect_t *crit_sect)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (crit_sect)
    {
        qurt_rmutex_init(crit_sect);
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);

}
/*===========================================================================
FUNCTION - OSAL_DELETE_CRIT_SECT */
/*!
  @brief Deletes a critical section

  @param[in] crit_sect - Pointer to a critical section object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */

/*===========================================================================*/
int osal_delete_crit_sect(osal_crit_sect_t *crit_sect)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (crit_sect)
    {
        qurt_rmutex_destroy(crit_sect);
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}
/*===========================================================================
  FUNCTION   OSAL_ENTER_CRIT_SECT */
/*!
  @brief Locks a critical section

  @param[in] crit_sect - Pointer to a critical section object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_enter_crit_sect(osal_crit_sect_t *crit_sect)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (crit_sect)
    {
        qurt_rmutex_lock(crit_sect);
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}

/*===========================================================================
  FUNCTION   OSAL_EXIT_CRIT_SECT */
/*!
  @brief Unlocks a critical section

  @param[in] crit_sect - Pointer to critical section object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/* ===========================================================================*/
int osal_exit_crit_sect(osal_crit_sect_t *crit_sect)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (crit_sect)
    {
        qurt_rmutex_unlock(crit_sect);
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}

/*=======================================
 *              Mutexes
 ========================================*/
/*===========================================================================
  FUNCTION   OSAL_INIT_MUTEX */
/*!
  @brief Initializes a mutex

  @param[in] mutex - Address of Mutex object. Mutex will be initialized after
                     the call.

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_init_mutex(osal_mutex_arg_t *mutex)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if(mutex)
    {
        qurt_rmutex_init(&(mutex->handle));
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}
/*===========================================================================
  FUNCTION   OSAL_DELETE_MUTEX */
/*!
  @brief Deletes a mutex

  @param[in] mutex - Address of Mutex object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_delete_mutex(osal_mutex_arg_t *mutex)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if(mutex)
    {
        qurt_rmutex_destroy(&(mutex->handle));
        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}
/*===========================================================================
  FUNCTION   OSAL_LOCK_MUTEX */
/*!
  @brief    Locks a mutex

  @param[in] mutex - Address of Mutex object

  @dependencies None

  @return None

  @sideeffects None */
/*===========================================================================*/
void osal_lock_mutex(osal_mutex_arg_t *mutex)
{
    if(mutex)
    {
        qurt_rmutex_lock(&(mutex->handle));
    }
}
/*===========================================================================
  FUNCTION   OSAL_UNLOCK_MUTEX */

/*!
  @brief    unlocks a mutex

  @param[in] mutex - Address of Mutex object

  @dependencies None

  @return None

  @sideeffects None */
/*===========================================================================*/
void osal_unlock_mutex(osal_mutex_arg_t *mutex)
{
    if (mutex)
    {
        qurt_rmutex_unlock(&(mutex->handle));
    }
}
/*==========================================
             Signaling
============================================*/
/*===========================================================================
  FUNCTION   OSAL_SET_SIGS */
/*!
  @brief
    Sets a signal in the task TCB for systems running on rex and BLAST.
    In the case of Qube, sends a message to the thread whose TCB is passed in.

  @param[in]  tcb_ptr - tcb pointer of the task/thread for which to set the signal
  @param[in]  sig     - the signals/messages to be set
  @param[out] prev_sig -Address of signal object. The sigs prior to the set
                          action will be filled into this.

  @dependencies
       Every task or thread using this OSAL needs to maintain an array of integers
       containing priorities of each signal/msg it uses. Needs to call
       set_msg_priority() with the pointer to that array before the call
       to create the task. The set_msg_priority() is no-oped in rex.

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_set_sigs(osal_tcb_t *tcb_ptr, osal_sigs_t sig, osal_sigs_t *prev_sig)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (tcb_ptr && prev_sig)
    {

        *prev_sig = qurt_anysignal_set(&(tcb_ptr->current_sigs), sig);

        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}
/*===========================================================================
  FUNCTION   OSAL_RESET_SIGS */
/*!
  @brief
       Clears the specified signals or messages for the TCB

  @param[in]  tcb_ptr - tcb pointer of the task/thread for which to reset the
                        signal
  @param[in]  sig     - the signals/messages to be reset or cleared
  @param[out] prev_sig - Address of signal object. The sigs prior to the reset
                          action will be filled into this.

  @dependencies
       Every task or thread using this OSAL needs to maintain an array of
       integers containing priorities of each signal/msg it uses. Needs to call
       set_msg_priority() with the pointer to that array before the call
       to create the task. The set_msg_priority() is no-oped in rex.

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */

/*===========================================================================*/
int osal_reset_sigs(osal_tcb_t *tcb_ptr, osal_sigs_t sig, osal_sigs_t *prev_sig)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (tcb_ptr && prev_sig)
    {
        *prev_sig = qurt_anysignal_clear(&(tcb_ptr->current_sigs), sig);

        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}
/*===========================================================================
  FUNCTION   OSAL_GET_SIGS */
/*!
  @brief Gets the currently asserted signals/msgs for the task/thread

  @param[in]  tcb_ptr - tcb pointer of the task/thread whose signal to get
  @param[out] current_sig - Address of signal object. The current sigs/messages
                          for a task will be filled here

  @dependencies
       Every task or thread using this OSAL needs to maintain an array of integers
       containing priorities of each signal/msg it uses. Needs to call
       set_msg_priority() with the pointer to that array before the call
       to create the task. The set_msg_priority() is no-oped in rex.

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_get_sigs(osal_tcb_t *tcb_ptr, osal_sigs_t *current_sig)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (tcb_ptr && current_sig)
    {

        *current_sig = qurt_anysignal_get(&(tcb_ptr->current_sigs));

        ret_value = OSAL_SUCCESS;
    }
    return(ret_value);
}
/*===========================================
*                 Timers
=============================================*/
/*===========================================================================
  FUNCTION   OSAL_CREATE_TIMER */
/*!
@brief Creates a timer in the default timer group which is defferable.

  @param[out] timer - Address of a valid timer object
  @param[in]  tcb_ptr - tcb of task/thread to set the signal for
  @param[in]  sig -    Signal/message to be set when the timer expires

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None
 =========================================================================== */
int osal_create_timer(osal_timer_t *timer,  osal_tcb_t *tcb_ptr, osal_sigs_t sig)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    timer_error_type time_err=TE_FAIL;

    if (timer && tcb_ptr )
    {
        /* memsetting to 0, so that we will be able to check later if the handle
        is being re-used or not */
        memset(timer,0x0,sizeof(osal_timer_t));

        /*Passing NULL timer group (2nd param) to timer_def_osal creates
        deferrable timer */
        time_err= timer_def_osal(timer, NULL, TIMER_NATIVE_OS_SIGNAL_TYPE,
                                            &(tcb_ptr->current_sigs), sig);

        if (time_err !=TE_SUCCESS)
        {
            ret_value=OSAL_FAILURE;
        }
        else
        {
            ret_value = OSAL_SUCCESS;
        }
    }

    return(ret_value);
}

/*===========================================================================
  FUNCTION   OSAL_CREATE_TIMER_EX */
/*!
    @brief
       Extended version of timer-creation function. Allows the
       specification of a callback function and an argument to this
       function. The callback is called when the timer expires. The
       callback happens in the context of the task that calls
       osal_create_timer_ex.

       This timer will be created in the default timer group
       which is defferable.

  @param[out] timer - Address of a valid timer object
  @param[in] tcb_ptr -    Signal/message to be set when the timer expires
  @param[in] timer_cb_ptr - pointer to timer callback
  @param[in] cb_param - argument passed to timer callback

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/

int osal_create_timer_ex(osal_timer_t *timer, osal_tcb_t *tcb_ptr,
                osal_timer_cb_type timer_cb_ptr, osal_timer_param_type cb_param)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    timer_error_type time_err=TE_FAIL;

    (void) tcb_ptr; /*to supress KW warning */

    if (timer && timer_cb_ptr)
    {
        /* memsetting to 0, so that we will be able to check later if the handle
        is being re-used or not */
        memset(timer,0x0,sizeof(osal_timer_t));

        /*Passing NULL timer group (2nd param) to timer_def_osal creates
        deferrable timer */
        time_err= timer_def_osal(timer, NULL, TIMER_FUNC1_CB_TYPE,
                                            timer_cb_ptr, cb_param);

        if (time_err !=TE_SUCCESS)
        {
            ret_value=OSAL_FAILURE;
        }
        else
        {
            ret_value = OSAL_SUCCESS;
        }
    }
    return(ret_value);

}
/*===========================================================================
  FUNCTION   OSAL_CREATE_DIAG_TIMER */
/*!
  @brief
    Creates a timer in the default diag timer group (diag_timer_group),
    which is non-defferable.

  @param[out] timer - Address of a valid timer object
  @param[in]  tcb_ptr - tcb of task/thread to set the signal for
  @param[in]  sig -    Signal/message to be set when the timer expires

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None
 =========================================================================== */
int osal_create_diag_timer(osal_timer_t *timer,  osal_tcb_t *tcb_ptr,
                            osal_sigs_t sig)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    timer_error_type time_err=TE_FAIL;


    if (timer && tcb_ptr )
    {
        /* memsetting to 0, so that we will be able to check later if the handle is
        being re-used or not */
        memset(timer,0x0,sizeof(osal_timer_t));

        /*Passing diag_timer_group to timer_def_osal creates non-deferrable timer */
        time_err= timer_def_osal(timer, &diag_timer_group,
                TIMER_NATIVE_OS_SIGNAL_TYPE, &(tcb_ptr->current_sigs), sig);

        if (time_err !=TE_SUCCESS)
        {
            ret_value=OSAL_FAILURE;
        }
        else
        {
            ret_value = OSAL_SUCCESS;
        }
    }

    return(ret_value);
}
/*===========================================================================
  FUNCTION   OSAL_CREATE_DIAG_TIMER_EX */
/*!
  @brief
       Extended version of timer-creation function. Allows the
       specification of a callback function and an argument to this
       function. The callback is called when the timer expires. The
       callback happens in the context of the task that calls
       create_timer_ex.

       This timer will be created in the diag timer group
       (diag_timer_group), which is made non-defferable.

  @param[out] timer - Address of a valid timer object
  @param[in]  tcb_ptr -    Signal/message to be set when the timer expires
  @param[in]  timer_cb_ptr - pointer to timer callback
  @param[in]  cb_param - argument passed to timer callback

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_create_diag_timer_ex(osal_timer_t *timer, osal_tcb_t *tcb_ptr,
               osal_timer_cb_type timer_cb_ptr, osal_timer_param_type cb_param)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    timer_error_type time_err=TE_FAIL;

    (void) tcb_ptr; /*to supress KW warning */

    if (timer && timer_cb_ptr)
    {
        /*memsetting to 0, so that we will be able to check later if the handle
        is being re-used or not */
        memset(timer,0x0,sizeof(osal_timer_t));

        /*Passing diag_timer_group to timer_def_osal creates non-deferrable timer */
        time_err= timer_def_osal(timer, &diag_timer_group, TIMER_FUNC1_CB_TYPE,
                                    timer_cb_ptr, cb_param);

        if (time_err !=TE_SUCCESS)
        {
            ret_value=OSAL_FAILURE;
        }
        else
        {
            ret_value = OSAL_SUCCESS;
        }
    }
    return(ret_value);

}
/*===========================================================================
  FUNCTION   OSAL_SET_TIMER */

/*!
  @brief Starts a timer

  @param[in] timer_ptr - Address of the timer object to start.
  @param[in] timer_cnt - Duration for the timer in ms

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_set_timer(osal_timer_t *timer_ptr, osal_timer_cnt_t timer_cnt)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    timer_error_type time_err=TE_FAIL;
    if (timer_ptr)
    {
        osal_lock_mutex(&diag_timer_mutex);

        time_err=timer_set_64(timer_ptr, (timetick_type)timer_cnt, 0, T_MSEC);
        if(time_err != TE_SUCCESS)
        {
            ret_value = OSAL_FAILURE;
        }
        else
        {
            ret_value = OSAL_SUCCESS;
        }
        osal_unlock_mutex(&diag_timer_mutex);
    }
    return(ret_value);
}
/*===========================================================================
  FUNCTION   OSAL_TIMED_WAIT */
/*!
  @brief
       Suspends a task and sets a timer. When either the timer or one of the
       signals is set and the task is the highest ready task, the task resumes.

  @param[in] tcb_ptr - task associated with the timer
  @param[in] sig - sigs to wait on
  @param[in] timer_ptr - pointer to timer to be set and wait on
  @param[in] timer_cnt - time to wait

  @dependencies
       None

  @return
        returns the signal that was set.

  @sideeffects
       Causes a task swap if the signals are not already set for the calling task.*/

/*===========================================================================*/
osal_sigs_t osal_timed_wait(osal_tcb_t *tcb_ptr, osal_sigs_t sig,
                            osal_timer_t *timer_ptr, osal_timer_cnt_t timer_cnt)
{
    if (tcb_ptr && timer_ptr)
    {
        osal_sigs_t return_sigs;

        osal_set_timer(timer_ptr, timer_cnt);

        return_sigs =  osal_thread_wait(tcb_ptr, sig);

        return return_sigs;
    }
    return (osal_sigs_t)NULL;
}
/*===========================================================================
  FUNCTION   OSAL_RESET_TIMER */
/*!
  @brief
       Re-sets a timer

  @param[in] timer_ptr - Address of the timer object to reset.

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_reset_timer(osal_timer_t *timer_ptr)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    timer_error_type time_err=TE_FAIL;
    if (timer_ptr)
    {
        osal_lock_mutex(&diag_timer_mutex);
        time_err= timer_stop(timer_ptr, T_MSEC, NULL);
        if(time_err != TE_SUCCESS && time_err != TE_TIMER_ALREADY_IN_SAME_STATE)
        {
            ret_value = OSAL_FAILURE;
        }
        else
        {
            ret_value = OSAL_SUCCESS;
        }
        osal_unlock_mutex(&diag_timer_mutex);
    }
    return(ret_value);
}
/*===========================================================================
  FUNCTION   OSAL_DELETE_TIMER */
/*!
  @brief
       Deletes or clears a timer

  @param[in] timer_ptr - Address of the timer object to clear


  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_delete_timer(osal_timer_t *timer_ptr)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    if (timer_ptr)
    {
        osal_lock_mutex(&diag_timer_mutex);
        if (TE_SUCCESS != timer_undef(timer_ptr))
        {
            ret_value = OSAL_FAILURE;
        }
        else
        {
            ret_value = OSAL_SUCCESS;
        }
        
		osal_unlock_mutex(&diag_timer_mutex);
    }
    return(ret_value);
}
/*===========================================================================
  FUNCTION   OSAL_GET_REMAINING_TIME */
/*!
  @brief
       Returns the current remaining count of a timer. 0 is returned if the timer
       is not on the list.

  @param[in] timer_ptr - pointer to the timer to get

  @dependencies
       None

  @return
  <ul>
  <li> time count
  <li> 0
  </ul>

  @sideeffects None */
/*===========================================================================*/
osal_timer_cnt_t osal_get_remaining_time(osal_timer_t *timer_ptr)
{
  osal_timer_cnt_t time_remaining=0;

  if (timer_ptr != NULL)
  {
    /*remaining time returned in ticks */
    time_remaining= timer_get_64(timer_ptr, T_TICK);
  }
  return time_remaining;
}

/*================================================
*             Thread Manipulation
==================================================*/
/*==========================================================================
  FUNCTION   OSAL_THREAD_SELF */
/*!
  @brief
       Gets a pointer to the TCB of the currently running task/thread

  @param None

  @dependencies
       None

  @return
  <ul>
  <li> pointer to qurt_tcb_t object.
  <li> NULL
  </ul>

  @sideeffects None */
/*==========================================================================*/
osal_tcb_t *osal_thread_self()
{
     int ctr = 0;
     int thread_id;
     thread_id = qurt_thread_get_id();

     for( ctr=0; ctr < QURT_MAX_THREADS; ctr++ )
     {
         /* look up in the table for the thread ID corresponding to the
         tcb pointer */
         if( qurt_tcb_map_table[ctr].thd_id == thread_id )
         {

             return (qurt_tcb_map_table[ctr].tcb_id);

         }
     }
     return NULL;
}
/*===========================================================================
  FUNCTION   OSAL_THREAD_WAIT */
/*!
  @brief
       Waits on specified signals/messages

  @param[in] tcb_ptr - tcb of task that the signals are associated with
  @param[in] sig - signals/messages to wait on

  @dependencies None

  @return Signals/messages asserted

  @sideeffects None */
/*===========================================================================*/
osal_sigs_t osal_thread_wait(osal_tcb_t *tcb_ptr, osal_sigs_t sigs)
{
    osal_sigs_t return_value = 0;
    if(tcb_ptr)
    {
     return_value = qurt_anysignal_wait(&(tcb_ptr->current_sigs), sigs);
    }
     return (return_value);
}
/*===========================================================================
  FUNCTION   OSAL_CREATE_THREAD */
/*!
  @brief
       Creates a thread/task

  @param[in] tcb_ptr - Valid tcb for new task/thread
  @param[in] attr - Thread attributes object
  @param[in] pri - array to pass in the priorities of msg queues for qube.
                   Ignore for all other OSes.

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_create_thread(osal_tcb_t *tcb_ptr, osal_thread_attr_t *attr, int *pri)
{
  typedef void (* thread_handler_func_t) (void *);
  int ret_value = OSAL_NULL_PTR_RECVD;
  int ctr = 0;
  osal_thread_t  thread_id;
  qurt_thread_attr_t tattr;
  unsigned int stackbase;

    (void) pri; /*To supress warning */
    if(tcb_ptr && attr)
    {

        if ( thread_count >= QURT_MAX_THREADS )
        {
         fprintf(stderr, " osal_create_thread() failed since we reached maximum thread count \n");
         return(OSAL_FAILURE);
        }
        else
        {
          qurt_anysignal_init(&(tcb_ptr->current_sigs));

          /* The QURT OS expects the initial stack address. Stack grows upwards */
          stackbase = (unsigned int)attr->stack_address;

          qurt_thread_attr_init (&tattr);
          qurt_thread_attr_set_stack_size (&tattr, (attr->stack_size -8));
          qurt_thread_attr_set_stack_addr (&tattr, (void*)((stackbase +7) &(~7)) );
          qurt_thread_attr_set_priority (&tattr, (unsigned short)attr->priority);
          qurt_thread_attr_set_name(&tattr,attr->name);


          /* update the global table to map the thread ID to the tcb */
          qurt_rmutex_lock(&tcb_map_table_lock);
          for( ctr=0; ctr < QURT_MAX_THREADS; ctr++ )
          {
            /* look up in the table for the thread ID corresponding to the
            tcb pointer */

            if( qurt_tcb_map_table[ctr].tcb_id == 0)
            {
              ret_value =  qurt_thread_create(&thread_id, &tattr, (thread_handler_func_t)attr->start_func, (void *)attr->arg);

              if (QURT_EFATAL == ret_value)
              {
                ret_value = OSAL_FAILURE;
                qurt_rmutex_unlock(&tcb_map_table_lock);
                return(ret_value);
              }
              else
              {
                tcb_ptr->thd_id = thread_id;
                ret_value = OSAL_SUCCESS;
              }

              qurt_tcb_map_table[ctr].thd_id = thread_id;
              qurt_tcb_map_table[ctr].tcb_id = tcb_ptr;
              ++thread_count;
              break;
            }

          }

          qurt_rmutex_unlock(&tcb_map_table_lock);

          qurt_rmutex_init(&tcb_ptr->tcb_msg_lock);
        }
    }
    return(ret_value);
}

/*===========================================================================
  FUNCTION   OSAL_DELETE_THREAD */
/*!
  @brief
       Exits the current calling thread/task.  Calling osal_delete_thread()
       for an application's primary thread/task causes the application to
       terminate.

  @param[in] tcb_ptr - Valid tcb for the task/thread

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_delete_thread(osal_tcb_t *tcb_ptr)
{
    int ret_value = OSAL_NULL_PTR_RECVD;
    int ctr = 0;
    int thread_id;

    if(tcb_ptr)
    {
        ret_value = OSAL_FAILURE;
        thread_id = tcb_ptr->thd_id;
        qurt_rmutex_lock(&tcb_map_table_lock);
        for( ctr=0; ctr < QURT_MAX_THREADS; ctr++ )
        {
            /* look up in the table for the thread ID corresponding to the
            tcb pointer */
            if( qurt_tcb_map_table[ctr].thd_id == thread_id )
            {
                /*Destroy the signal created in osal_create_thread*/
                qurt_anysignal_destroy(&(tcb_ptr->current_sigs));
                qurt_rmutex_destroy(&tcb_ptr->tcb_msg_lock);
                qurt_tcb_map_table[ctr].thd_id = 0;
                qurt_tcb_map_table[ctr].tcb_id = 0;
                --thread_count;
                ret_value = OSAL_SUCCESS;
                break;
            }
        }
        qurt_rmutex_unlock(&tcb_map_table_lock);
    }
    return(ret_value);
}

#if 0
/*===========================================================================
  FUNCTION   OSAL_SET_TASK_NAME */
/*!
  @brief
       Set the name for the current task/thread

  @param[in] name - Pointer to the string

  @dependencies
       None

  @return None

  @sideeffects None */
/*==========================================================================*/
void osal_set_task_name(void *name)
{
  qurt_thread_attr_t attr;

  /* set thread name */
  qurt_thread_attr_set_name(&attr, name);
}
#endif
/*===========================================================================
  FUNCTION   OSAL_THREAD_EXIT */
/*!
  @brief
    The thread_exit() function terminates the calling thread, making its exit
    status available to any waiting threads.

  @param[in] status - exit status

  @dependencies
       None

  @return None

  @sideeffects None */
/*==========================================================================*/
void osal_thread_exit(int status)
{
     qurt_thread_exit(status);
}
/*===========================================================================
  FUNCTION   OSAL_THREAD_JOIN */
/*!
  @brief
    The thread_join() function shall suspend execution of the calling thread
    until the target thread terminates unless the target thread has already
    terminated.

  @param[in] tcb pointer - tcb pointer of the thread
  @param[out] status - address to fill in the status

  @dependencies
       None

  @return None

  @sideeffects None */
/*==========================================================================*/
void osal_thread_join(osal_tcb_t *tcb_ptr, int *status)
{
    qurt_thread_join((unsigned int)tcb_ptr->thd_id, status);
}
/*===========================================================================
  FUNCTION   osal_thread_get_pri */
/*!
  @brief
       Gets the priority of the currently thread

  @param None

  @dependencies
       None

  @return Priority of the thread

  @sideeffects None */
/*==========================================================================*/
osal_priority_type osal_thread_get_pri()
{
    unsigned int thread_id = qurt_thread_get_id();
    return (qurt_thread_get_priority(thread_id));
}
/*===========================================================================
FUNCTION   OSAL_THREAD_SET_PRI */

/*!
  @brief
       Sets the priority of the currently running thread

  @param[in] prio - priority to be set to

  @dependencies None

  @return The priority before it was changed.

  @sideeffects None */
/*==========================================================================*/
osal_priority_type osal_thread_set_pri(osal_priority_type pri)
{
    int ret_value = 0;

    unsigned int thread_id = qurt_thread_get_id();

    ret_value = qurt_thread_get_priority(thread_id);

    qurt_thread_set_priority(thread_id, pri);
    return ret_value;
}
/*===========================================================================
FUNCTION   OSAL_INIT */
/*!
  @brief
       Init routine for OSAL

  @param[in] None

  @dependencies None

  @return None.

  @sideeffects None */
/*==========================================================================*/

void osal_init (void)
{
    qurt_rmutex_init(&tcb_map_table_lock);
    osal_init_mutex(&diag_timer_mutex);
}

/*================================================================
* Certain APIs related to interrupts and the watchdog
==================================================================*/
/*===========================================================================
  FUNCTION   OSAL_AUTODOG_ENABLE */

/*!
  @brief
    Since there's no dog services these platforms,
    this function does nothing and returns 0.

  @param None

  @dependencies None

  @return 0

  @sideeffects None */
/*===========================================================================*/
int osal_autodog_enable(int dog_report_val)
{
  return 0;
}
/*===========================================================================
FUNCTION   OSAL_AUTODOG_DISABLE */

/*!
  @brief
    Since there's no dog services these platforms,
    this function does nothing and returns 0.

  @param None

  @dependencies None

  @return 0

  @sideeffects None */
/*===========================================================================*/
int osal_autodog_disable(void)
{
  return 0;
}
/*===========================================================================
FUNCTION    OSAL_INTS_ARE_LOCKED */

/*!
  @brief
       Gets the interrupts Lock status
       On ADSP it currently does nothing and always returns 0

  @param None

  @dependencies None

  @return 0

  @sideeffects None */
/*===========================================================================*/
int osal_ints_are_locked(void)
{
  return 0;
}
/*===========================================================================
FUNCTION    OS_IS_IN_IRQ_MODE */

/*!
  @brief
    This API is only for REX OS and hence returns FALSE on this subsystem.

  @param None

  @dependencies None

  @return FALSE

  @sideeffects None */
/*===========================================================================*/
boolean os_is_in_irq_mode( void )
{
  return FALSE;
}
/*===========================================================================
FUNCTION    OSAL_ATOMIC_COMPARE_AND_SET */

/*!
  @brief
    Compares the atomic word value to old_value and if it matches
    then sets new value.

  @param
    [in] target    Pointer to the atomic word.
    [in] old_val   old value to compare
    [in] new_val   new value to be set.

  @dependencies None

  @return One of the following
  <ul>
  <li> FALSE
  <li> TRUE
  </ul>

  @sideeffects None */
/*===========================================================================*/
int osal_atomic_compare_and_set(osal_atomic_word_t *target,
                       osal_atomic_plain_word_t old_val,
                       osal_atomic_plain_word_t new_val )
{
   return atomic_compare_and_set(target,old_val,new_val);

}
/*===========================================================================
FUNCTION    OSAL_ATOMIC_SET_BIT */

/*!
  @brief
    Sets the input bit to the atomic word.

  @param
    [in] target    Pointer to the atomic word.
    [in] bit       bit to set.

  @dependencies None

  @return None

  @sideeffects None */
/*===========================================================================*/
void osal_atomic_set_bit(osal_atomic_word_t *target, unsigned long bit)
{
   atomic_set_bit(target,bit);
}
/*===========================================================================
FUNCTION    OSAL_ATOMIC_CLEAR_BIT */

/*!
  @brief
    Clears the bit to the atomic word.

  @param
    [in] target    Pointer to the atomic word.
    [in] bit       bit to clear.

  @dependencies None

  @return None

  @sideeffects None */
/*===========================================================================*/
void osal_atomic_clear_bit(osal_atomic_word_t *target, unsigned long bit)
{

   atomic_clear_bit(target,bit);
}
/*===========================================================================
FUNCTION    OSAL_ATOMIC_ADD */

/*!
  @brief
    Atomic add of the input word to the target word.

  @param
    [in] target    Pointer to the atomic word.
    [in] v         Word to add.

  @dependencies None

  @return None

  @sideeffects None */
/*===========================================================================*/
void osal_atomic_add(osal_atomic_word_t *target, osal_atomic_plain_word_t v)
{
   atomic_add(target, v);
}

