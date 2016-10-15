#ifndef QURT_TIMER_H
#define QURT_TIMER_H
/**
  @file qurt_timer.h
  @brief  Prototypes of qurt_timer API 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2010-2011 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/kernel/qurt/libs/timer/include/public/qurt_timer.h#13 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
12/16/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/

#include "qurt_anysignal.h"
#include "qurt_signal2.h"
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

/*
 Define magic                                                          */
#define QURT_TIMER_MAGIC                   0xadadadad
#define QURT_TIMER_EXPIRED                 0xedededed
#define QURT_TIMER_INVALID                 0xdfdfdfdf
#define QURT_TIMER_ATTR_MAGIC              0xaeaeaeae
#define QURT_TIMER_ATTR_INVALID            QURT_TIMER_INVALID

/*
 Default Values                                                            
*/
#define QURT_TIMER_DEFAULT_TYPE            QURT_TIMER_ONESHOT
/**@ingroup timer_types */
#define QURT_TIMER_DEFAULT_DURATION        1000uL /**< Default value. */
#define QURT_TIMER_DEFAULT_EXPIRY          0uL

/*
 Conversion from microseconds to timer ticks.
 */
#define QURT_TIMER_TIMETICK_FROM_US(us) qurt_sysclock_timetick_from_us(us)

/*
 Conversion from timer ticks to microseconds at the nominal frequency.
*/
#define QURT_TIMER_TIMETICK_TO_US(ticks) qurt_timer_timetick_to_us(ticks)

/**@ingroup timer_const_macros
 Minimum microseconds value = 100 microseconds (Sleep timer).               
*/
#define QURT_TIMER_MIN_DURATION             100uL

/**@ingroup timer_const_macros
  For Qtimer: Maximum microseconds value = 1042499 hours.
*/
#define QURT_TIMER_MAX_DURATION  qurt_sysclock_max_duration()

/* 
 For Qtimer: Timer clock is 19.2MHz.
*/
#define QURT_TIMER_MAX_DURATION_TICKS qurt_sysclock_max_duration_ticks()

/** 
 For Qtimer: Sleep timer error margin, 1000 ticks ~52us.
*/
#define QURT_TIMETICK_ERROR_MARGIN qurt_sysclock_error_margin()

/** @addtogroup timer_types
@{ */
/** 
  qurt_timer group defines.                                                    
*/
#define QURT_TIMER_MAX_GROUPS              5 /**< */
#define QURT_TIMER_DEFAULT_GROUP           0 /**< */

/**
   qurt_timer_type types.                                                       
 */
typedef enum
{
  QURT_TIMER_ONESHOT = 0,  /**< */
  QURT_TIMER_PERIODIC      /**< */
} qurt_timer_type_t;


/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/** qurt_timer_t types.*/
typedef unsigned int                        qurt_timer_t;

/** qurt_timer_duration_t types.  */
typedef unsigned long long                  qurt_timer_duration_t;

/** qurt_timer_time_t types. */
typedef unsigned long long                  qurt_timer_time_t;

/** qurt_timer_attr_t types. */
typedef struct 
{
    /** @cond */
    unsigned int        magic; /**< Magic number to verify qmsgq_attr_t pointer.  */    
    
    qurt_timer_duration_t   duration; /**< Specifies the duration of new timer. */

    qurt_timer_time_t   expiry; /**< Specifies the absolute expiry of new timer. */

    qurt_timer_duration_t   remaining; /**< Specifies the remaining time of an active timer. */

    qurt_timer_type_t       type;  /**< Specifies the timer type, only QURT_TIMER_ONESHOT and
       QURT_TIMER_PERIODIC are currently supported.  */

    unsigned int        group;  /**<  Group number of the timer, the criterion used to disable or enable the set.
       of timers.  */
    /** @endcond */
}
qurt_timer_attr_t;

/** @} */ /* end_addtogroup timer_types */
/*=============================================================================
                        FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_timer_stop
  @xreflabel{sec:qurt_timer_stop}
  Stops a running timer.
  The timer must be a one-shot timer.

  @note1hang Stopped timers can be restarted with the timer restart operation,
             see Section @xref{sec:qurt_timer_restart}. 

  @datatypes
  #qurt_timer_t
  
  @param[in] timer    Timer object. 

  @return
  QURT_EOK -- Success. \n
  QURT_EINVALID -- Invalid timer ID or duration value. \n
  QURT_ENOTALLOWED -- Timer is not a oneshot timer. \n
  QURT_EMEM -- Out of memory error.

  @dependencies
  None.
 */
int qurt_timer_stop (qurt_timer_t timer);

/**@ingroup func_qurt_timer_restart
   @xreflabel{sec:qurt_timer_restart}
  Restarts a stopped timer with the specified duration.
  The timer must be a one-shot timer.

  @note1hang Timers become stopped after they have expired or after they are explicitly
             stopped with the timer stop operation, see Section @xref{sec:qurt_timer_stop}.

  @datatypes
  #qurt_timer_t \n
  #qurt_timer_duration_t

  @param[in] timer        Timer object. 
  @param[in] duration     Timer duration (in microseconds) before the restarted timer
                          expires again.
                          Valid range is QURT_TIMER_MIN_DURATION to
                          QURT_TIMER_MAX_DURATION.

  @return             
  QURT_EOK -- Success. \n
  QURT_EINVALID -- Invalid timer ID or duration value. \n
  QURT_ENOTALLOWED -- Timer is not a one-shot timer. \n
  QURT_EMEM --  Out-of-memory error.

  @dependencies
  None.
 */
int qurt_timer_restart (qurt_timer_t timer, qurt_timer_duration_t duration);


/**@ingroup func_qurt_timer_create
  Creates a timer.\n
  Allocates and initializes a timer object, and starts the timer.

  @note1hang A timer event handler must be defined to wait on the specified signal 
             in order to handle the timer event.

  @datatypes
  #qurt_timer_t \n
  #qurt_timer_attr_t \n
  #qurt_anysignal_t

  @param[out] timer   Pointer to the created timer object.
  @param[in]  attr    Pointer to the timer attribute structure.
  @param[in]  signal  Pointer to the signal object set when timer expires.
  @param[in]  mask    Signal mask specifying signal to set in the signal object when the
                      time expires.

  @return
  QURT_EOK -- Success. \n
  QURT_EMEM -- Not enough memory to create the timer.

  @dependencies
  None.
 */
int qurt_timer_create (qurt_timer_t *timer, const qurt_timer_attr_t *attr,
                  const qurt_anysignal_t *signal, unsigned int mask);

int qurt_timer_create_sig2 (qurt_timer_t *timer, const qurt_timer_attr_t *attr, 
                  const qurt_signal2_t *signal, unsigned int mask);

/**@ingroup func_qurt_timer_attr_init
  Initializes the specified timer attribute structure with default attribute values: \n
  - Timer duration -- QURT_TIMER_DEFAULT_DURATION (Section @xref{dox:timers}) \n
  - Timer type -- QURT_TIMER_ONESHOT \n
  - Timer group -- QURT_TIMER_DEFAULT_GROUP

  @datatypes
  #qurt_timer_attr_t

  @param[in,out] attr Pointer to the destination structure for the timer attributes.

  @return
  None.

  @dependencies
  None.
 */
void qurt_timer_attr_init(qurt_timer_attr_t *attr);

/**@ingroup qurt_timer_attr_set_duration
  Sets the timer duration in the specified timer attribute structure.\n

  The timer duration specifies the interval (in microseconds) between the creation of the
  timer object and the generation of the corresponding timer event.

  The timer duration value must be between QURT_TIMER_MIN_DURATION and
  QURT_TIMER_MAX_DURATION (Section @xref{dox:timers}). Otherwise, the set operation is ignored.

  @note1hang The maximum timer duration is 36 hours.

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_timer_duration_t

  @param[in,out] attr    Pointer to the timer attribute structure.
  @param[in] duration    Timer duration (in microseconds).
                         Valid range is QURT_TIMER_MIN_DURATION to
                         QURT_TIMER_MAX_DURATION.

  @return
  None.

  @dependencies
  None.
 */
void qurt_timer_attr_set_duration(qurt_timer_attr_t *attr, qurt_timer_duration_t duration);

/**@ingroup func_qurt_timer_attr_set_expiry
   Sets the absolute expiry time in the specified timer attribute structure.\n
   The timer expiry specifies the absolute time (in microseconds) of the generation of the
   corresponding timer event.\n
   Timer expiries are relative to when the system first began executing.

   @datatypes
   #qurt_timer_attr_t \n
   #qurt_timer_time_t

   @param[in,out] attr  Timer attribute structure.
   @param[in]     time  Timer expiry.

   @return
   None.

   @dependencies
   None.
 */
void qurt_timer_attr_set_expiry(qurt_timer_attr_t *attr, qurt_timer_time_t time);

/**@ingroup func_qurt_timer_attr_get_duration
  Gets the timer duration from the specified timer attribute structure.

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_timer_duration_t

  @param[in]  attr       Pointer to the timer attributes object
  @param[out] duration   Pointer to the destination variable for timer duration.

  @return
  None.

  @dependencies
  None.
 */
void qurt_timer_attr_get_duration(qurt_timer_attr_t *attr, qurt_timer_duration_t *duration);

/**@ingroup func_qurt_timer_attr_get_remaining
  Gets the timer remaining duration from the specified timer attribute structure. \n

  The timer remaining duration indicates (in microseconds) how much time remains before
  the generation of the next timer event on the corresponding timer.

  @note1hang This attribute is read-only and thus cannot be set.

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_timer_duration_t

  @param[in] attr          Pointer to the timer attribute object.
  @param[out] remaining    Pointer to the destination variable for remaining time.

  @return
  None.

  @dependencies
  None.
 */
void qurt_timer_attr_get_remaining(qurt_timer_attr_t *attr, qurt_timer_duration_t *remaining);

/**@ingroup func_qurt_timer_attr_set_type
  Sets the timer type in the specified timer attribute structure.

  The timer type specifies the functional behavior of the timer: \n
  - A one-shot timer (QURT_TIMER_ONESHOT) waits for the specified timer duration
      and then generates a single timer event. After this the timer is nonfunctional. \n
  - A periodic timer (QURT_TIMER_PERIODIC) repeatedly waits for the specified
     timer duration and then generates a timer event. The result is a series of timer
     events with interval equal to the timer duration.

   @datatypes 
   #qurt_timer_attr_t \n
   #qurt_timer_type_t
   
   @param[in,out]  attr  Timer attribute structure.
   @param[in]      type  Timer type. Values are: \n
                   - QURT_TIMER_ONESHOT -- One-shot timer. \n
                   - QURT_TIMER_PERIODIC -- Periodic timer.

   @return
   None.

   @dependencies
   None.
 */
void qurt_timer_attr_set_type(qurt_timer_attr_t *attr, qurt_timer_type_t type);

/**@ingroup func_qurt_timer_attr_get_type
  Gets the timer type from the specified timer attribute structure.

  @datatypes
  #qurt_timer_attr_t \n
  #qurt_timer_type_t

  @param[in]  attr   Pointer to the timer attribute structure.
  @param[out] type  Pointer to the destination variable for the timer type.

  @return
  None.

  @dependencies
  None.
 */
void qurt_timer_attr_get_type(qurt_timer_attr_t *attr, qurt_timer_type_t *type);

/**@ingroup func_qurt_timer_attr_set_group
  Sets the timer group identifier in the specified timer attribute structure.\n
  The timer group identifier specifies the group that the timer belongs to. Timer groups are
  used to enable or disable one or more timers in a single operation. \n
  The timer group identifier value must be between 0 and (QURT_TIMER_MAX_GROUPS-1)
  (Section @xref{dox:timers}).

  @datatypes
  #qurt_timer_attr_t

  @param[in,out]  attr  Pointer to the timer attribute object
  @param[in] group      Timer group identifier.
                        Valid range 0 to (QURT_TIMER_MAX_GROUPS - 1).

  @return
  None.

  @dependencies
  None.
 */
void qurt_timer_attr_set_group(qurt_timer_attr_t *attr, unsigned int group);

/**@ingroup func_qurt_timer_attr_get_group
  Gets the timer group identifier from the specified timer attribute structure.
  
  @datatypes
  #qurt_timer_attr_t
  
  @param[in]  attr   Pointer to the timer attribute structure.
  @param[out] group  Pointer to the destination variable for the timer group identifier.

  @return
  None.

  @dependencies
  None.
 */
void qurt_timer_attr_get_group(qurt_timer_attr_t *attr, unsigned int *group);

/**@ingroup func_qurt_timer_get_attr
  Gets the timer attributes of the specified timer.

  @note1hang After a timer is created, its attributes cannot be changed.

  @datatypes
  #qurt_timer_t \n
  #qurt_timer_attr_t

  @param[in] timer  Pointer to the timer object.
  @param[out] attr  Destination structure for timer attributes.

  @return
  QURT_EOK -- Success. \n
  QURT_VAL -- Argument passed is not a valid timer.

  @dependencies
  None.
 */
int qurt_timer_get_attr(qurt_timer_t timer, qurt_timer_attr_t *attr);

/**@ingroup func_qurt_timer_delete
  Deletes the timer.\n
  Destroys the specified timer and deallocates the timer object.
  
  @datatypes
  #qurt_timer_t
  
  @param[in] timer  Pointer to the timer object.

  @return       
  QURT_EOK -- Success. \n
  QURT_EVAL -- Argument passed is not a valid timer. 

  @dependencies
  None.
 */
int qurt_timer_delete(qurt_timer_t timer);

/**@ingroup func_qurt_timer_sleep
  Suspends the current thread for the specified amount of time.
  The sleep duration value must be between QURT_TIMER_MIN_DURATION and
  QURT_TIMER_MAX_DURATION (Section @xref{dox:timers}).

  @note1hang The maximum sleep duration is 36 hours. The error margin of the sleep timer 
             is approximately 90 microseconds (due to a setup time of 2 ticks and resolution
             of 1 tick).  

  @datatypes
  #qurt_timer_duration_t

  @param[in] duration  The interval (in microseconds) between when the thread is suspended
                       and when it is re-awakened. 

  @return 
  QURT_EOK -- Success. \n
  QURT_EMEM -- Not enough memory to perform operation.

  @dependencies
  None.
 */

int qurt_timer_sleep(qurt_timer_duration_t duration);

/**@ingroup func_qurt_timer_group_disable
  Disables all timers that are assigned to the specified timer group.
  If a specified timer is already disabled, ignore it.
  If a specified timer is expired, do not process it.
  If the specified timer group is empty, do nothing.

  @note1hang When a timer is disabled its remaining time does not change, thus it
             cannot generate a timer event.
 
  @param[in] group  Timer group identifier.

  @return 
  QURT_EOK -- Success.

  @dependencies
  None.
 */
int qurt_timer_group_disable (unsigned int group);

/**@ingroup func_qurt_timer_group_enable
  Enables all timers that are assigned to the specified timer group.
  If a specified timer is already enabled, ignore it.
  If a specified timer is expired, process it.
  If the specified timer group is empty, do nothing.

  @param[in] group  Timer group identifier.

  @return 
  QURT_EOK -- Success.

  @dependencies
  None.
 */
int qurt_timer_group_enable (unsigned int group);


/**
  Notifies the timer server recovery from power collapse. The server
  has to account for any missed interrupts during power collapse. 
 */
void qurt_timer_recover_pc (void);

/**
   Determines whether the Qtimer is initialized.

   @return
   0        -- Not initialized. \n
   Nonzero -- Initialized.
 */
static inline int qurt_timer_is_init (void) {return 1;};

/**
   Gets current ticks. The ticks are accumulated since the RTOS
   has started. Each tick is equal to a single timer clock
   cycle, where the frequency is 32 KHz on RGPT or 19.2 MHz on Qtimer.
  
   @return             
   Ticks since system started.
 */
unsigned long long qurt_timer_get_ticks (void);


#define qurt_timer_timetick_from_us(us) qurt_sysclock_timetick_from_us(us)
unsigned long long qurt_timer_timetick_to_us(unsigned long long ticks);
unsigned long long qurt_timer_max_duration(void);
unsigned long long qurt_timer_max_duration_ticks(void);
unsigned int qurt_timetick_error_magin(void);



#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_TIMER_H */
