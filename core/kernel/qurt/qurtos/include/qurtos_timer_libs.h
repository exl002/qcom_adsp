#ifndef QURT_TIMER_LIBS_H
#define QURT_TIMER_LIBS_H

/*=============================================================================

					 qurt_TimerLibs.h -- H E A D E R	F I L E

GENERAL DESCRIPTION
  Implements time-keeping functions using the TCXO clock.

  TCXO Clock Frequency          Granularity     Timer Range
    19.2MHz                      1 us             3.6 mins

EXTERNAL FUNCTIONS

  timer_init()
    Initializes the timer subsytem. 

  timer_def(timer, task, sigs)
    Defines and initializes a timer.
    Timers may also be staticly declared via the TIMER_DEF() macro

  timer_new(timer, ticks, reload)
    Sets an active timer to expire after a given period of time.
    Optionally, specifies the timer to repeatly expire with a given period.

  timer_get(timer)
    Get number of ticks before timer expires

  timer_cancel(timer)
    Stops an active timer

INITIALIZATION AND SEQUENCING REQUIREMENTS

    timer_init() must be called once before any other timer functions.

    timer_def() must be called, or the TIMER_DEF( ) macro used, for each
      timer, before other timer functions are used with that timer.

Copyright (c) 2006
							by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
$Header: //components/rel/core.adsp/2.2/kernel/qurt/qurtos/include/qurtos_timer_libs.h#6 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/06/06   saa     File created.

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "qurt_timer_defines.h"
#include <qurt_signal.h>
#include "qurt_timer.h"

/*=============================================================================
                           DATA DECLARATIONS
=============================================================================*/
/*---------------------------------------------------------------------------*/
/*  Timer internal errors                                                    */
/*---------------------------------------------------------------------------*/
#define ETIMER_TOOCLOSE                 0xffffffff

/*---------------------------------------------------------------------------*/
/*  Timer callback data type                                                 */
/*---------------------------------------------------------------------------*/
//typedef unsigned int                    qurt_timer_cb_data_type;

/*---------------------------------------------------------------------------*/
/*  Timer callback data type                                                 */
/*---------------------------------------------------------------------------*/
typedef struct qurt_timer_client_struct    *qurt_timer_client_ptr;

/*---------------------------------------------------------------------------*/
/*  Timer callback function type                                             */
/*---------------------------------------------------------------------------*/
typedef void (*qurt_timer_cb_type) (qurt_timer_client_ptr timer);

/*---------------------------------------------------------------------------*/
/*  Timer list structure type                                                */
/*  Values in this structure are for private use by "timer.c" only.          */
/*---------------------------------------------------------------------------*/
typedef struct qurt_timers_type_struct 
{

  /* A sorted linked list of all the active timer nodes */
  qurt_timer_client_ptr                active_list;

  /* match value set to the HW */
  qurt_timetick_word_t                 match_value;

  /* count value of HW when match value was set */
  qurt_timetick_word_t                 count_value;

  /* uptime ticks (wall clk) when match value was set */
  qurt_timetick_type                   set_expiry;
}
qurt_timers_type;

typedef struct qurt_timers_group_type_struct 
{
  /* A sorted linked list of all the active timer nodes */
  qurt_timer_client_ptr                list;
}
qurt_timers_group_type;

/*-----------------------------------------------------------------------------
  Timer structure type
    Values in this structure are for private use by "timer.c" only.
-----------------------------------------------------------------------------*/

typedef struct qurt_timer_client_struct
{
  /* Magic number to validate client pointer */
  unsigned int                   magic;

  /* Timer type */
  unsigned int                   type;

  /* APC (Asynchronous Procedure Call) function to call when timer expires */
  qurt_timer_cb_type                 func;

  /**
   * The signal pointer the client is waiting on.
   */
  qurt_anysignal_t *signal;
  /**
   * The signal mask the client is waiting on.
   */
  unsigned int mask;

  /* Reload offset when timer expires (0 = no reload at expiry) */
  /* SLEEP_TIMER */
  qurt_timer_duration_t               reload;

  /* TCX0 clock tick count when timer was started */
  qurt_timetick_type                 start;

  /* TCX0 clock tick count timer expires at or remaining ticks until expiry */
  qurt_timetick_type                 expiry;

  /* Pointer to the next timer in the list (list != NULL) */
  qurt_timer_client_ptr              next;

  /* Group no. of the timer. The criterion used to disable or enable the set
   * of timers. */
  unsigned int                   group;
}
qurt_timer_client;

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/**
 * Initializes the Timer Subsytem
 * Note: This function is executed from server side.
 *
 * SIDE EFFECTS: May install an ISR for the 32-bit TCXO clock match register
 *
 * @param     None
 * @return    None
 */
void qurt_timer_lib_init ( void );

/**
 * Return absolute current time since the hardware timer was enabled
 * Note: This function is executed from server side.
 *
 * @param     None
 * @return    Absolute current time
 */
qurt_timetick_type qurt_timer_lib_current_time (qurt_timetick_word_t *hw_count);

/**
 * Sets an active timer to expire after a given period of time.
 * Optionally, specifies the timer to repeatly expire with a given period.
 *
 * @param timer    Timer to set
 * @param time     timer duration
 * @param expiry   expiry time in micro-seconds
 * @param reload   Period in micro-seconds between repeated expiries (0 = not periodic)
 * @return         EOK
 */
int qurt_timer_lib_new ( qurt_timer_client_ptr timer, qurt_timer_duration_t time,  qurt_timer_time_t expiry, qurt_timer_duration_t reload, unsigned int group);


/**
 * Restart an active timer. x
 * "Duration" value can be different from orignal value.
 *
 * @param timer    Timer 
 * @param duration duration or expiry (duration from startup)
 */
int qurt_timer_lib_restart_active_timer ( qurt_timer_client_ptr timer, qurt_timer_duration_t duration);


/**
 * Stop an active timer. 
 *
 * @param timer    Timer 
 */
int qurt_timer_lib_stop_active_timer ( qurt_timer_client_ptr timer );


#if 0
/**
 * Get the payload pointer from the timer node
 *
 * @param timer      Timer to get
 * @param payload    Pointer to the payload
 * @return           EOK or error code
 */
inline int qurt_timer_lib_get_payload ( qurt_timer_client_ptr timer, qurt_timer_cb_data_type* data );
#endif /* 0 */

/**
 * Stops an active timer
 *
 * @param timer    Timer to stop
 * @return         EOK or error code
 */
int qurt_timer_lib_cancel ( qurt_timer_client_ptr timer );

/**
 * This function Processes the expired timer
 * Note: May insert messages in message queue, which can cause task switches.
 * 
 * @param timer   Expiring timer to be processed
 * @return        None
 */
void qurt_timer_lib_expired ( qurt_timer_client_ptr timer );

/**
 * This function processes expired timers in the active timer list, and then program the timer HW for next timer interrupt.
 * Considering that timerServer thread is not running at highest priority and context switch may happen during programming 
 * timer HW, this function detects context switch and then reprogram timer in a loopback mode. 
 *
 * @param    None
 * @return   None
 */
void qurt_timer_lib_process_active_timers ( void );

/**
 * Send the signal the client registered to receive upon timer expiry.
 * Note: This function is executed from qurt_timer server.
 *
 * @param token     Sleeping client thread ID.
 * @return          EOK if send is successful, error code otherwise
 */
void qurt_timer_lib_process_signal_callback (qurt_timer_client_ptr timer);

/**
 * Send the signal the client registered to receive upon timer expiry. It does little more than qurt_timer_lib_process_signal_expiry () as the timer needs to be freed.
 * Note: This function is executed from qurt_timer server.
 *
 * @param token     Sleeping client thread ID.
 * @return          EOK if send is successful, error code otherwise
 */
void qurt_timer_lib_process_sleep_callback (qurt_timer_client_ptr timer);

/**
 * Send the message the client registered to receive upon expiry.
 * Note: This function is executed from qurt_timer server pd.
 *
 * @param token     Sleeping client thread ID.
 * @return          EOK if send is successful, error code otherwise
 */
void qurt_timer_lib_process_qmsgq_callback (qurt_timer_client_ptr timer);

/**
 * Enable group timers, make them active
 *
 * @param group    group ID
 * @return         EOK, EINVAL_ARG if group ID is invalid
 */
int qurt_timer_lib_group_enable (unsigned int group);

/**
 * Disables group timers. Pull them from active timers
 *
 * @param group    group ID
 * @return         EOK, EINVAL_ARG if group ID is invalid
 */
int qurt_timer_lib_group_disable (unsigned int group);

/**
 * Process the group timers if the are alreday expired.
 *
 * @param group    group ID
 * @return         none
 */
void qurt_timer_lib_process_group_expiry (unsigned int group);

#endif /* QURT_TIMER_LIBS_H */
