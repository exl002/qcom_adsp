/*=============================================================================

                Qurt_Timer_Client.c

GENERAL DESCRIPTION
      Qurt Timer Client Process Code

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009 - 2013
      by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/services/time/src/qurt_timer_client.c#4 $ 
$DateTime: 2013/09/11 08:59:47 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/20/13   ab      Add header file
=============================================================================*/

/*****************************************************************************/
/*                           INCLUDE FILES                                   */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "qurt.h"
#include "qurt_timer_qdi_v.h"

/*****************************************************************************/
/*                          DATA DECLARATIONS                                */
/*****************************************************************************/

/* Qdi Timer Client Handle to communicate with Guest OS layer */
extern int timer_client_qdi_handle;

/*****************************************************************************/
/*                      FUNCTION DEFINITIONS                                 */
/*****************************************************************************/
/**
 * Verify attributes passed to qurt_timer_verify_args
 *
 * The qurt_timer_verify_args is used to verfiy all the input arguments to 
 * qurt_timer_create function. Error checks related to minimum and maximum duration 
 * and type of timer are done in this function
 * function.
 *
 * This function can only be used from "qTimerClient.c" 
 *
 * @param attr        Specifies timer duration and timer type
 * @return            QURT_EOK if creation is successful, error code otherwise
 */
static inline int qurt_timer_verify_args ( const qurt_timer_attr_t *attr )
{
    int rc = QURT_EOK;

    /* If attribute pointer non-null, verify all attributes */
    if ( NULL != attr ) 
    {
        /* Verify Magic number */
        if ( QURT_TIMER_ATTR_MAGIC != attr->magic )
        {
            rc = QURT_EINVALID;
            printf ("invalid attr->magic\n");
        }

        /* Verify duration with min & max timer duration */
        if ( QURT_TIMER_MIN_DURATION > attr->duration ||
                QURT_TIMER_MAX_DURATION < (attr->duration ))
        {
            rc = QURT_EINVALID;
            printf ("invalid attr->duration\n");
        }

        /* Verify timer type */
        if ( QURT_TIMER_ONESHOT != attr->type &&
                QURT_TIMER_PERIODIC < attr->type )
        {
            rc = QURT_EINVALID;
            printf ("invalid attr->type\n");
        }

        /* Verify timer expiry */
        if (attr->type == QURT_TIMER_PERIODIC && attr->expiry != QURT_TIMER_DEFAULT_EXPIRY )
        {
            rc = QURT_EINVALID;
            printf ("attr->type == QURT_TIMER_PERIODIC && attr->expiry != QURT_TIMER_DEFAULT_EXPIRY\n");
        }

        if (attr->group >= QURT_TIMER_MAX_GROUPS)
        {
            rc = QURT_EINVALID;
            printf ("invalid attr->group\n");
        }
    } /* if ( null != attr ) */
    else /* Null attributes are not allowed while creating a message queue */
    {
        rc = QURT_EINVALID;
        printf ("Null attr for qurt_timer_create \n");
    }

    /* All input arguments are verified */
    return rc;
    
} /* qurt_timer_verify_args */


/*****************************************************************************/
/*                                                                           */
/*                   Functions exposed to the user                           */
/*                                                                           */
/*****************************************************************************/

/**
 * Stop a one-shot timer. 
 *
 * @param timer        Timer ID. 
 * @return             QURT_EOK:        Successful stop
 * @return             QURT_EINVALID:   Invalid timer ID
 * @return             QURT_ENOTALLOWED: Timer is not a oneshot timer.
 * @return             QURT_EMEM:        Out of memory error,
 */

int qurt_timer_stop (qurt_timer_t timer )
{
   if(timer == NULL)
      return QURT_EINVALID;

   return qtimer_stop(timer_client_qdi_handle, timer);
}

/**
 * Restart a one-shot timer with a duration. 
 *
 * @param timer        Timer ID. 
 * @param time         duration or expiry (dration from startup).
 * @return             QURT_EOK:        Successful restart
 * @return             QURT_EINVALID:   Invalid timer ID or duration value
 * @return             QURT_ENOTALLOWED: Timer is not a oneshot timer.
 * @return             QURT_EMEM:        Out of memory error,
 */

int qurt_timer_restart (qurt_timer_t timer, qurt_timer_duration_t time )
{
    /* Check the timer ID */
    if ( timer == NULL )
    {
        return QURT_EINVALID;
    }
    
    return qtimer_restart(timer_client_qdi_handle, timer, time);
}


/**
 * Create a timer. 
 *
 * @param timer        Timer ID. 
 * @param attr         Attributes structure.
 * @param signal       Signal objct.
 * @param mask         Mask to be set to signal.
 * @return             QURT_EOK:        Successful restart
 * @return             QURT_EINVALID:   Invalid timer ID or duration value
 * @return             QURT_ENOTALLOWED: Timer is not a oneshot timer.
 * @return             QURT_EMEM:        Out of memory error,
 */
int qurt_timer_create (qurt_timer_t *timer, const qurt_timer_attr_t *attr,
                  const qurt_anysignal_t *signal, unsigned int mask)
{
    int result;
    qurt_timer_t created_timer;
    
    if(timer_client_qdi_handle == -1)
    {
      MSG_ERROR("Timer system not initialized", 0, 0, 0);
      return QURT_EFAILED;
    }
    
    /* Verify all the input arguments */
    if(QURT_EOK != (result = qurt_timer_verify_args ( attr ) ) )
    {
        return result;
    }

    created_timer = qtimer_create(timer_client_qdi_handle, timer, attr, signal, mask);
    if (created_timer != 0)
    {
        *timer = created_timer;
        return QURT_EOK;
    }
    else
        return QURT_EUNKNOWN;
} /* qurt_timer_create */


/**
 * Create a timer of signal2 type. 
 *
 * @param timer        Timer ID. 
 * @param attr         Attributes structure.
 * @param signal       Signal2 objct.
 * @param mask         Mask to be set to signal.
 * @return             QURT_EOK:        Successful restart
 * @return             QURT_EINVALID:   Invalid timer ID or duration value
 * @return             QURT_ENOTALLOWED: Timer is not a oneshot timer.
 * @return             QURT_EMEM:        Out of memory error,
 */
int qurt_timer_create_sig2 (qurt_timer_t *timer, const qurt_timer_attr_t *attr,
                  const qurt_signal2_t *signal, unsigned int mask)
{
    int result;
    qurt_timer_t created_timer;
    
    if(timer_client_qdi_handle == -1)
    {
      MSG_ERROR("Timer system not initialized", 0, 0, 0);
      return QURT_EFAILED;
    }
    
    /* Verify all the input arguments */
    if(QURT_EOK != (result = qurt_timer_verify_args ( attr ) ) )
    {
        return result;
    }

    created_timer = qtimer_create_sig2(timer_client_qdi_handle, timer, attr, signal, mask);
    if (created_timer != 0)
    {
        *timer = created_timer;
        return QURT_EOK;
    }
    else
        return QURT_EUNKNOWN;
} /* qurt_timer_create_sig2 */


/**
 * Initialize attribute object with default values.
 *
 * The default values are QURT_TIMER_ONESHOT for timer type and
 * 1ms for timer duration
 *
 * @param attr  Attributes object
 */
void qurt_timer_attr_init ( qurt_timer_attr_t *attr )
{
    /* Initialize attribute data structure */
    attr->magic        = QURT_TIMER_ATTR_MAGIC;
    attr->type         = QURT_TIMER_DEFAULT_TYPE;
    attr->duration     = QURT_TIMER_DEFAULT_DURATION;
    attr->expiry       = QURT_TIMER_DEFAULT_EXPIRY;
    attr->remaining    = QURT_TIMER_DEFAULT_DURATION;
    attr->group        = QURT_TIMER_DEFAULT_GROUP;

    /* Return success */
    return;

} /* qurt_timer_attr_init */

/**
 * Get attributes of a message queue
 *
 * @param msgq         Message queue object
 * @param attr  [OUT]  Message queue attributes
 * @return             QURT_EOK:       get_attr successful,
 *                     EFAILED:   IPC related failures,
 *                     QURT_EINVALID:  Wrong parameters,
 */
int qurt_timer_get_attr(qurt_timer_t timer, qurt_timer_attr_t *attr)
{
   if(timer == NULL)
      return QURT_EINVALID;
    return qtimer_get_attr(timer_client_qdi_handle, timer, attr);
}


/**
 * Set duration
 *
 * @param attr      Timer attributes object
 * @param duration  Duration for the timer
 */
void qurt_timer_attr_set_duration(qurt_timer_attr_t *attr, qurt_timer_duration_t duration)
{
    /* Verify Magic number */
    if ( QURT_TIMER_ATTR_MAGIC != attr->magic )
    {
       return;
    }

    /* Check whether Expiry Field has been changed */
    /* qurt_timer_attr_set_duration() and qurt_timer_attr_setexpiry() are mutual exlusive */ 
    if ( QURT_TIMER_DEFAULT_EXPIRY != attr->expiry )
    {
       return;
    }

    /* Set duration in the attribute */
    attr->duration = duration;
    attr->remaining = duration;

    /* Return success */
    return;

} /* qurt_timer_attr_set_duration */


/**
 * Set expiry
 *
 * @param attr      Timer attributes object
 * @param time      Absolute expiry time for the timer in micro-seconds
 */
void qurt_timer_attr_set_expiry(qurt_timer_attr_t *attr, qurt_timer_time_t time)
{
    /* Verify Magic number */
    if ( QURT_TIMER_ATTR_MAGIC != attr->magic )
    {
       return;
    }

    /* Check whether Duration Field has been changed */
    /* qurt_timer_attr_set_duration() and qurt_timer_attr_setexpiry() are mutual exlusive */ 
    if ( QURT_TIMER_DEFAULT_DURATION != attr->duration )
    {
       return;
    }

    /* Set Expiry in the attribute */
    attr->expiry = time;

    /* Return success */
    return;
} /* qurt_timer_attr_set_duration */

/**
 * Get duration
 *
 * @param attr      Timer attributes object
 * @param duration  Duration for the timer
 */
void qurt_timer_attr_get_duration(qurt_timer_attr_t *attr, qurt_timer_duration_t *duration)
{
    /* If attribute pointer non-null, verify attribute pointer */
    if ( NULL != attr ) 
    {
        /* Verify Magic number */
        if ( QURT_TIMER_ATTR_MAGIC != attr->magic )
        {
            return;
        }

        /* Duration should be non-null */
        if ( NULL == duration )
        {
            return;
        }

        /* Return duration */
        *duration = attr->duration;

        /* Return success */
        return;

    } /* if ( null != attr ) */

    /* Attribute pointer is not valid */
    return;
} /* qurt_timer_attr_get_duration */

/**
 * Get remaining time
 *
 * @param attr          Timer attributes object
 * @param remaining     Remaining time for the timer
 */
void qurt_timer_attr_get_remaining(qurt_timer_attr_t *attr, qurt_timer_duration_t *remaining)
{
    /* If attribute pointer non-null, verify attribute pointer */
    if ( NULL != attr ) 
    {
        /* Verify Magic number */
        if ( QURT_TIMER_ATTR_MAGIC != attr->magic )
        {
            return;
        }

        /* Duration should be non-null */
        if ( NULL == remaining )
        {
            return;
        }

        /* Return duration */
        *remaining = attr->remaining;

        /* Return success */
        return;

    } /* if ( null != attr ) */

    /* Attribute pointer is not valid */
    return;
} /* qurt_timer_attr_get_remaining */

/**
 * Set timer type - one shot or periodic
 *
 * @param attr   Timer attributes object
 * @param type   Timer type
 */
void qurt_timer_attr_set_type(qurt_timer_attr_t *attr, qurt_timer_type_t type)
{
    /* Verify Magic number */
    if ( QURT_TIMER_ATTR_MAGIC != attr->magic )
    {
       return;
    }
    
    /* Set timer type in the attribute */
    attr->type = type;

    /* Return success */
    return;
} /* qurt_timer_attr_set_type */

/**
 * Set duration
 *
 * @param attr  Timer attributes object
 * @param type  Timer type
 */
void qurt_timer_attr_get_type(qurt_timer_attr_t *attr, qurt_timer_type_t *type)
{
    /* If attribute pointer non-null, verify attribute pointer */
    if ( NULL != attr ) 
    {
        /* Verify Magic number */
        if ( QURT_TIMER_ATTR_MAGIC != attr->magic )
        {
            return;
        }

        /* type should be non-null */
        if ( NULL == type )
        {
            return;
        }

        /* Return duration */
        *type = attr->type;

        /* Return success */
        return;

    } /* if ( null != attr ) */

    /* Attribute pointer is not valid */
    return;
} /* qurt_timer_attr_get_type */

/**
 * Set timer group, ranging 0 to QURT_TIMER_MAX_GROUPS - 1
 *
 * @param attr   Timer attributes object
 * @param group  Group ID
 */
void qurt_timer_attr_set_group (qurt_timer_attr_t *attr, unsigned int group)
{
    /* Verify Magic number */
    if ( QURT_TIMER_ATTR_MAGIC != attr->magic )
    {
       return;
    }
    
    /* Set timer type in the attribute */
    attr->group = group;

    /* Return success */
    return;
} /* qurt_timer_attr_set_group */

/**
 * Get the group ID from the attribute
 *
 * @param attr  Timer attributes object
 * @param type  Timer type
 */
void qurt_timer_attr_get_group (qurt_timer_attr_t *attr, unsigned int *group)
{
    /* If attribute pointer non-null, verify attribute pointer */
    if ( NULL != attr ) 
    {
        /* Verify Magic number */
        if ( QURT_TIMER_ATTR_MAGIC != attr->magic )
        {
            return;
        }

        /* Read group ID */
        *group = attr->group;

        /* Return success */
        return;

    } /* if ( null != attr ) */

    /* Attribute pointer is not valid */
    return;
} /* qurt_timer_attr_get_group */

/**
 * Deletes timer
 *
 * @param timer  Pointer to timer object
 * @return       QURT_EOK:       Successful create,
 *               EFAILED:   IPC related failures,
 *               QURT_EINVALID:  Wrong timer 
 */
int qurt_timer_delete(qurt_timer_t timer)
{
   if(timer == NULL)
      return QURT_EINVALID;

   return qtimer_delete(timer_client_qdi_handle, timer);
} /* qurt_timer_delete */

/**
 * Qurt Timer Sleep function.
 *  
 * Cause the calling thread to be suspended from execution
 * until the specified duration has elapsed.
 *
 * @param duration  number of us to sleep for 
 * @return             QURT_EOK:       Successful create,
 *                     QURT_EMEM:      Out of memory error,
 *                     EFAILED:   IPC related failures,
 *                     EVAL:      Wrong duration value.
 */
int qurt_timer_sleep( qurt_timer_duration_t duration )
{
   qurt_anysignal_t sleep_signal;
   qurt_timer_attr_t attr;
   qurt_timer_t sleep_timer;
   int result;
   
   /* Initialize the qurt signal */
   qurt_anysignal_init (&sleep_signal);
   
   qurt_timer_attr_init(&attr);
   qurt_timer_attr_set_duration(&attr, duration);

   /* Create timer */
   result = qurt_timer_create(&sleep_timer, &attr, &sleep_signal, 0x1);
   if(result != QURT_EOK)
      return result;
   
   /* Wait for the signal */
   qurt_anysignal_wait (&sleep_signal, 0x1);
   
   /* Release the resources allocated */
   qurt_timer_delete(sleep_timer);
   qurt_anysignal_destroy (&sleep_signal);

   return QURT_EOK;
} /* qurt_timer_sleep */

unsigned long long qurt_timer_get_ticks (void)
{
   return qurt_sysclock_get_hw_ticks();
}

int qurt_timer_group_enable (unsigned int group)
{
    return qtimer_group_enable(timer_client_qdi_handle, group);
}

int qurt_timer_group_disable (unsigned int group)
{
    return qtimer_group_disable(timer_client_qdi_handle, group);
}

void qurt_timer_recover_pc (void) 
{
    qtimer_recover_pc(timer_client_qdi_handle);
}