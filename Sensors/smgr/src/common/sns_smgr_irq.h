/** 
 * SMGR's interrupt queue. 
 *  
 * Interrupts are pushed into the queue in ISR context and serviced by SMGR in 
 * SMGR task context. Note that the actual administrative work to deliver 
 * signals to drivers is done by DDF, SMGR only serves as the thread manager. 
 *  
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *  
 * @see sns_ddf_smgr_if.h 
 * @see sns_ddf_comm.h 
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_irq.h#1 $
  $DateTime: 2014/05/16 10:56:56 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2011-12-26 yk   Initial revision
==============================================================================*/

#ifndef SNS_SMGR_IRQ__H
#define SNS_SMGR_IRQ__H

#include "sns_smgr_sensor_config.h"
#include <stdbool.h>


#define SNS_SMGR_IRQ_QUEUE_CAPACITY  SNS_SMGR_NUM_SENSORS_DEFINED


/**
 * Interrupt request. This item gets queued in @sns_smgr_irq_queue_s.
 */
typedef struct
{
    uint32_t  irq_param; //< Parameter passed in by the interrupt handler.
    uint32_t  timestamp; //< Time at which the ISR handled the interrupt.
} sns_smgr_irq_s;


/**
 * Interrupt request queue.
 */
typedef struct
{
    sns_smgr_irq_s  irqs[SNS_SMGR_IRQ_QUEUE_CAPACITY];
    uint32_t        push_idx;
    uint32_t        pop_idx;
    uint32_t        size;
} sns_smgr_irq_queue_s;


/**
 * Initialize the IRQ queue. This must be called before any other function.
 *  
 * @param queue[in]  Queue to be initialized.
 */
void sns_smgr_irq_queue_init(sns_smgr_irq_queue_s* queue);


/**
 * Push an IRQ into the interrupt queue.
 * 
 * @param[in] queue      Queue to push into.
 * @param[in] irq_param  Parameter passed in by the interrupt.
 * 
 * @return True if the item was pushed successfully. False if the queue is full.
 */
bool sns_smgr_irq_queue_push(sns_smgr_irq_queue_s* queue, uint32_t irq_param);


/**
 * Get the next (i.e., oldest) request from the interrupt queue.
 * 
 * @param[in]  queue  Queue
 * @param[out] irq    Popped interrupt request.
 * 
 * @return True if an item was popped successfully. False if queue was empty.
 */
bool sns_smgr_irq_queue_pop(sns_smgr_irq_queue_s* queue, sns_smgr_irq_s* irq);

#endif

