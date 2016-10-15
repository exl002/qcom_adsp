#ifndef STACKCHECK_H
#define STACKCHECK_H

/* ========================================================================
   Stack Check header file

   *//** @file StackCheck.h
  

   Copyright (c) 2012 QUALCOMM Technologies Incorporated.
   All Rights Reserved. Qualcomm Proprietary and Confidential.
   ======================================================================== */

/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   03/01/12   corinc  Creation.

   ========================================================================= */



extern void *__stack_chk_guard;

extern void init_stack_chk_guard(void);

void __wrap___stack_chk_fail(void);

#endif /* STACKCHECK_H */
