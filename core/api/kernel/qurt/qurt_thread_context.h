#ifndef QURT_THREAD_CONTEXT_H
#define QURT_THREAD_CONTEXT_H
/**
  @file qurt_thread_context.h 
  @brief Kernel thread context structure
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2009 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_thread_context.h#11 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

=============================================================================*/

#include <qurt_qdi_constants.h>

static inline int qurt_thread_iterator_create(void)
{
   return qurt_qdi_handle_invoke(QDI_HANDLE_GENERIC, QDI_OS_THREAD_ITERATOR_CREATE);
}

static inline qurt_thread_t qurt_thread_iterator_next(int iter)
{
   return qurt_qdi_handle_invoke(iter, QDI_OS_THREAD_ITERATOR_NEXT);
}

static inline int qurt_thread_iterator_destroy(int iter)
{
   return qurt_qdi_close(iter);
}

#define THREAD_ITERATOR_END ((qurt_thread_t)(-1))

int qurt_thread_context_get_tname(unsigned int thread_id, char *name, unsigned char max_len);
int qurt_thread_context_get_prio(unsigned int thread_id, unsigned char *prio);
int qurt_thread_context_get_pcycles(unsigned int thread_id, unsigned long long int *pcycles);
int qurt_thread_context_get_stack_base(unsigned int thread_id, unsigned int *sbase);
int qurt_thread_context_get_stack_size(unsigned int thread_id, unsigned int *ssize);

int qurt_thread_context_get_pid(unsigned int thread_id, unsigned int *pid);
int qurt_thread_context_get_pname(unsigned int thread_id, char *name, unsigned int len);

#endif
