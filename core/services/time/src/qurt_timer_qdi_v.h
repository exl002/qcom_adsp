/*=============================================================================

                Qurt_Timer_Qdi_v.h  --  Header File

GENERAL DESCRIPTION
      Qurt Shim Timer QDI Layer Header File

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


$Header: //components/rel/core.adsp/2.2/services/time/src/qurt_timer_qdi_v.h#3 $ 
$DateTime: 2013/09/11 08:59:47 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/20/13   ab      Add header file
=============================================================================*/
#ifndef QURT_TIMER_QDI_V_H
#define QURT_TIMER_QDI_V_H

#include "qurt.h"
#include "qurt_signal2.h"
#include "timer_qdi_v.h"


#define TIMER_QDI_QURT_CREATE         (TIMER_QDI_ATS_LAST_METHOD + 1)
#define TIMER_QDI_QURT_CREATE_SIG2    (TIMER_QDI_QURT_CREATE + 1)
#define TIMER_QDI_QURT_STOP           (TIMER_QDI_QURT_CREATE_SIG2 + 1)
#define TIMER_QDI_QURT_DELETE         (TIMER_QDI_QURT_STOP + 1)
#define TIMER_QDI_QURT_RESTART        (TIMER_QDI_QURT_DELETE + 1)
#define TIMER_QDI_QURT_SLEEP          (TIMER_QDI_QURT_RESTART + 1)
#define TIMER_QDI_QURT_GET_ATTR       (TIMER_QDI_QURT_SLEEP + 1)
#define TIMER_QDI_QURT_GET_TICKS      (TIMER_QDI_QURT_GET_ATTR + 1)
#define TIMER_QDI_QURT_GROUP_ENABLE   (TIMER_QDI_QURT_GET_TICKS + 1)
#define TIMER_QDI_QURT_GROUP_DISABLE  (TIMER_QDI_QURT_GROUP_ENABLE + 1)
#define TIMER_QDI_QURT_RECOVER_PC     (TIMER_QDI_QURT_GROUP_DISABLE + 1)

static inline int qtimer_create(int handle, qurt_timer_t *timer, const qurt_timer_attr_t *attr, 
                                const qurt_anysignal_t *signal, unsigned int mask)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_CREATE, timer, attr, signal, mask);
}

static inline int qtimer_create_sig2(int handle, qurt_timer_t *timer, const qurt_timer_attr_t *attr, 
                                const qurt_signal2_t *signal, unsigned int mask)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_CREATE_SIG2, timer, attr, signal, mask);
}

static inline int qtimer_stop(int handle, qurt_timer_t timer)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_STOP, timer);
}

static inline int qtimer_restart(int handle, qurt_timer_t timer, qurt_timer_duration_t time)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_RESTART, timer, (time>>32), time);
}

static inline int qtimer_delete(int handle, qurt_timer_t timer)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_DELETE, timer);
}

#if 0 /* Sleep is being done using timer in client space itself */
static inline int qtimer_sleep(int handle, qurt_timer_duration_t duration)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_SLEEP, duration>>32, duration);
}
#endif /* #if 0 */

static inline int qtimer_group_enable(int handle, unsigned int group)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_GROUP_ENABLE, group);
}

static inline int qtimer_group_disable(int handle, unsigned int group)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_GROUP_DISABLE, group);
}

static inline int qtimer_recover_pc(int handle)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_RECOVER_PC);
}

static inline int qtimer_get_attr(int handle, qurt_timer_t timer, qurt_timer_attr_t *attr)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_GET_ATTR, timer, attr);
}

static inline int qtimer_get_ticks(int handle, unsigned long long *ticks)
{
   return qurt_qdi_handle_invoke(handle, TIMER_QDI_QURT_GET_TICKS, ticks);
}

#endif /* QURT_TIMER_QDI_V_H */