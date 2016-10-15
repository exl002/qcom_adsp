/*============================================================================

  @file sns_osa_dsps.c

  @brief

  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/common/core/src/common/sns_osa_dsps.c#1 $


when         who     what, where, why
----------   ---     ---------------------------------------------------------
  2013-10-08 sc  Fixed an issue with OS_FLAG_CLR option in sns_os_sigs_post().
  2013-10-03 sc  On behalf of Amithash from QMI team: use native qurt signal
                 instead of qurt_elite channel/signal. This is to temporarily
                 address the incompatibility recently introduced between
                 qurt_elite and QMI API.
  2013-05-21 br  Inserted appropriated functionality into sns_os_mutex_del()
  2013-05-07 sc  Address klocwork errors; check for NULL pointers
  2012-11-15 ps  Change heap ID from QURT_ELITE_HEAP_DEFAULT to sns_heap_id 
  2012-08-04 ag  Bug fixes in post and pend wrappers
  2012-07-11 sc  Fixed the stack size passed into qurt thread create call
  2012-07-06 sc  Implementation for sns_os_set_qmi_csi_params() on QDSP6 
  2011-02-24 br  Inserted sns_os_time_dly() and changed sns_os_task_create_ext()

============================================================================*/

/*=====================================================================
  INCLUDES
  =======================================================================*/
#include "sns_osa.h"
/*=====================================================================
  INTERNAL FUNCTION
  =======================================================================*/

uint8_t thread_id_ref[255];

//Task
uint8_t       sns_os_task_create           (void           (*task)(void *p_arg),
                                            void            *p_arg,
                                            OS_STK          *ptos,
                                            uint8_t          prio)
{
  int tid;
  tid = qurt_elite_thread_launch(&tid, "Default", 0, 4096, prio, (int (*)(void *))task, p_arg, sns_heap_id);
  thread_id_ref[prio] = tid;

  //MSG_1(MSG_SSID_SNS, DBG_HIGH_PRIO, "THREAD RUNNING = %d", prio);

  return ADSP_EOK;
}

uint8_t       sns_os_task_create_ext       (void           (*task)(void *p_arg),
                                            void            *p_arg,
                                            OS_STK          *ptos,
                                            uint8_t          prio,
                                            uint16_t         id,
                                            OS_STK          *pbos,
                                            uint32_t         stk_size,
                                            void            *pext,
                                            uint16_t         opt,
                                            uint8_t          *name)
{
  int tid;
  tid = qurt_elite_thread_launch(&tid, (char *)name, NULL, (stk_size*4), prio, (int (*)(void *))task, p_arg, sns_heap_id);
  thread_id_ref[prio] = tid;

  //MSG_1(MSG_SSID_SNS, DBG_HIGH_PRIO, "THREAD RUNNING = %d", prio);

  return ADSP_EOK;
}

//Flag

OS_FLAG_GRP  *sns_os_sigs_create           (OS_FLAGS         flags,
                                            uint8_t         *perr)
{
  //add safety feature that returns perr = NULL when anything fails
  OS_FLAG_GRP *flag_to_return;

  if (perr == NULL)
  {
    MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "error pointer is NULL");
    return NULL;
  }

  flag_to_return = qurt_elite_memory_malloc(sizeof(OS_FLAG_GRP), sns_heap_id);
  if (flag_to_return == NULL)
  {
    MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "signal group create fail");
    *perr = OS_ERR_MEM_FULL;
    return NULL;
  }
  qurt_anysignal_init(&flag_to_return->signal);
  flag_to_return->unChannelWaitMask = flags;
  *perr = OS_ERR_NONE;
  return flag_to_return;
}

sig_node    *sns_os_sigs_add              (OS_FLAG_GRP     *pgrp,
                                           OS_FLAGS         flags)
{
  if (pgrp == NULL)
  {
    MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "signal group pointer is NULL" );
    return NULL;
  }

  pgrp->unChannelWaitMask |= flags;

  return &(pgrp->unChannelWaitMask); /* Not used anymore */
}


void         sns_os_set_qmi_csi_params    (OS_FLAG_GRP       *pgrp,
                                           OS_FLAGS           flags,
                                           qmi_csi_os_params *os_params,
                                           uint8_t           *perr)
{
  if (pgrp == NULL)
  {
    *perr = OS_ERR_FLAG_INVALID_PGRP;
    return;
  }
  os_params->signal = &pgrp->signal;
  os_params->sig = flags;

  *perr = OS_ERR_NONE;
}

OS_FLAG_GRP  *sns_os_sigs_del              (OS_FLAG_GRP     *pgrp,
                                            uint8_t          opt,
                                            uint8_t         *perr)
{
  /* XXX ? */
  return 0;
}

OS_FLAGS      sns_os_sigs_pend             (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          wait_type,
                                            uint32_t         timeout,
                                            uint8_t         *perr)
{
  uint32_t result = 0;

  //MSG(MSG_SSID_SNS, DBG_LOW_PRIO, "sig pend : wait on channel");
  // block on channel to wait
  qurt_anysignal_wait(&pgrp->signal, pgrp->unChannelWaitMask);
  result = qurt_anysignal_get(&pgrp->signal);
  // when it receives, clear and exit

  *perr = OS_ERR_EVENT_TYPE;
  qurt_anysignal_clear(&pgrp->signal, result);
  *perr = OS_ERR_NONE;

  return result;
}

OS_FLAGS      sns_os_sigs_accept           (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          wait_type,
                                            uint8_t         *perr)
{
  uint32_t result;
  result = qurt_anysignal_get(&pgrp->signal);
  *perr = OS_ERR_NONE;
  return result & flags; /* XXX ?*/
}


OS_FLAGS      sns_os_sigs_post             (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          opt,
                                            uint8_t         *perr)
{
  uint8_t os_err = OS_ERR_NONE;

  if (pgrp == NULL)
  {
    MSG(MSG_SSID_SNS, DBG_HIGH_PRIO, "pgrp is NULL");
    return 0;
  }

  if (opt == OS_FLAG_SET)
  {
    qurt_anysignal_set(&pgrp->signal, flags);
  }
  else if (opt == OS_FLAG_CLR)
  {
    qurt_anysignal_clear(&pgrp->signal, flags);
  }
  else
  {
    os_err = OS_ERR_INVALID_OPT;
  }
  
  if (perr != NULL)
  {
    *perr = os_err;
  }
  return flags;
}



//Mutex
OS_EVENT     *sns_os_mutex_create          (uint8_t          prio,
                                            uint8_t         *perr)
{
  //MSG(MSG_SSID_SNS, DBG_HIGH_PRIO, "mutex create malloc");
  OS_EVENT *mutex_event = qurt_elite_memory_malloc(sizeof(OS_EVENT), sns_heap_id);
  if (mutex_event == NULL)
  {
    MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "mutex create fail");
    *perr = OS_ERR_MEM_FULL;
    return NULL;
  }

  //MSG(MSG_SSID_SNS, DBG_HIGH_PRIO, "mutex create mutex init");
  qurt_elite_mutex_init(&(mutex_event->mutex));

  *perr = OS_ERR_NONE;
  //MSG(MSG_SSID_SNS, DBG_HIGH_PRIO, "mutex create return");
  return mutex_event;
}

OS_EVENT     *sns_os_mutex_del             (OS_EVENT        *pevent,
                                            uint8_t          opt,
                                            uint8_t         *perr)
{
  if ( NULL == pevent || NULL == perr )
  {
    if ( NULL != perr )
    {
      *perr = OS_ERR_PDATA_NULL;
    }
    return pevent;
  }
  else
  {
    qurt_elite_mutex_destroy(&pevent->mutex);
    qurt_elite_memory_free(pevent);
    *perr = OS_ERR_NONE;
    return (OS_EVENT *)0;
  }
}

void          sns_os_mutex_pend            (OS_EVENT        *pevent,
                                            uint32_t         timeout,
                                            uint8_t         *perr)
{
  //MSG(MSG_SSID_SNS, DBG_HIGH_PRIO, "...... MUTEX LOCK ......");
  qurt_elite_mutex_lock(&(pevent->mutex));
  *perr = OS_ERR_NONE;
}

uint8_t       sns_os_mutex_post            (OS_EVENT        *pevent)
{
  //MSG(MSG_SSID_SNS, DBG_HIGH_PRIO, "...... MUTEX UNLOCK ......");
  qurt_elite_mutex_unlock(&(pevent->mutex));
  return 0;
}

#ifdef SNS_DSPS_PROFILE_ON
void sns_os_time_dly( uint16_t ticks)
{
  OSTimeDly(ticks);
}
#endif

