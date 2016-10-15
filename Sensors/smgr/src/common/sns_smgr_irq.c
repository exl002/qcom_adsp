/** 
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_irq.c#1 $
  $DateTime: 2014/05/16 10:56:56 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2011-12-26 yk   Initial revision
==============================================================================*/

#include "sns_smgr_irq.h"
#include "sns_em.h"
#if defined(FEATURE_TEST_DRI)
#include "sns_smgr_test_dri.h" /* for DRI test purpose */
#endif


void sns_smgr_irq_queue_init(sns_smgr_irq_queue_s* queue)
{
    queue->push_idx = 0;
    queue->pop_idx = 0;
    queue->size = 0;
}


bool sns_smgr_irq_queue_push(sns_smgr_irq_queue_s* queue, uint32_t irq_param)
{
    if(queue->size == SNS_SMGR_IRQ_QUEUE_CAPACITY)
    {
       return false;
    }

    queue->irqs[queue->push_idx].irq_param = irq_param;
    queue->irqs[queue->push_idx].timestamp = sns_em_get_timestamp();

    #if defined(FEATURE_TEST_DRI)
    sns_test_dri_timestamp_delay();
    #endif /* defined(FEATURE_TEST_DRI) */

    queue->push_idx = (queue->push_idx + 1) % SNS_SMGR_IRQ_QUEUE_CAPACITY;
    queue->size++;

    return true;
}


bool sns_smgr_irq_queue_pop(sns_smgr_irq_queue_s* queue, sns_smgr_irq_s* irq)
{
    if(queue->size == 0)
    {
        return false;
    }

    // Eschew memcpy for efficiency.
    irq->irq_param = queue->irqs[queue->pop_idx].irq_param;
    irq->timestamp = queue->irqs[queue->pop_idx].timestamp;

    queue->pop_idx = (queue->pop_idx + 1) % SNS_SMGR_IRQ_QUEUE_CAPACITY;
    queue->size--;

    return true;
}

