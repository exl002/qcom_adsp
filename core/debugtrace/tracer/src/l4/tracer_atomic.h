#ifndef TRACER_ATOMIC_H
#define TRACER_ATOMIC_H
/*===========================================================================
  @file tracer_atomic.h

  Atomic operations.

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.adsp/2.2/debugtrace/tracer/src/l4/tracer_atomic.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include <kernel.h>

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
#define TRACER_ATOMIC_INC(_pVal)          okl4_atomic_inc((okl4_atomic_word_t*)_pVal)
#define TRACER_ATOMIC_DEC(_pVal)          okl4_atomic_dec((okl4_atomic_word_t*)_pVal)
#define TRACER_ATOMIC_SET(_pVal, _iVal)   okl4_atomic_set((okl4_atomic_word_t*)_pVal,_iVal)
#define TRACER_ATOMIC_CAS(_pVar, _oVar, _nVar) \
                              okl4_atomic_compare_and_set((okl4_atomic_word_t*)_pVar, _oVar, _nVar)
/*-------------------------------------------------------------------------*/

#endif /* #ifndef TRACER_ATOMIC_H */

