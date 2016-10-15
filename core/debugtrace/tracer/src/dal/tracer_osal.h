#ifndef TRACER_OSAL_H
#define TRACER_OSAL_H
/*===========================================================================
  @file tracer_osal.h

Target system specific information needed by TDS.

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.adsp/2.2/debugtrace/tracer/src/dal/tracer_osal.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "DALSysCmn.h"
#include "rex.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/

// Sizes correspond to TDS header fields.
typedef uint8 tracer_cpu_id_t;      /**< CPU Id as used by TDS. */
typedef uint8 tracer_process_id_t;  /**< Process Id as used by TDS. */
typedef uint16 tracer_thread_id_t;  /**< Thread Id as used by TDS. */

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/**
  @brief Get an identifier for the CPU
  @return CPU identifier
 */
static __inline tracer_cpu_id_t tracer_osal_cpu_id(void)
{
   uint32 targetId;
   uint32 procArch;
   DALSYSCMN_GetTargetInfo(&targetId, &procArch);
   return (targetId);
};

/**
  @brief Get an identifier for the process
  @return Process identifier
 */
static __inline tracer_process_id_t tracer_osal_process_id(void)
{
   return (0);
};

/**
  @brief Get an identifier for the thread
  @return Thread identifier
 */
static __inline tracer_thread_id_t tracer_osal_thread_id(void)
{
   rex_tcb_type *pMe = rex_self();
   return ((tracer_thread_id_t)(pMe->thread_id & 0x0FFFF));
};

/**
  @brief Get the name of the task (thread)
  @return task (thread) name
 */
static __inline char *tracer_osal_thread_name(void)
{
   rex_tcb_type *pMe = rex_self();
   return (pMe->task_name);
};

#endif /* #ifndef TRACER_OSAL_H */

