#ifndef TRACER_MUTEX_H
#define TRACER_MUTEX_H
/*===========================================================================
  @file tracer_mutex.h

   tracer_lock mutex
   To be included by only one file unless modified.

               Copyright (c) 2011 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.adsp/2.2/debugtrace/tracer/src/armv5/tracer_mutex.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
extern uint32 tracerInitMutex;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
__inline void tracer_mutex_init(void)
{
   return;
}
__inline void tracer_mutex_destroy(void)
{
   return;
}

/**
  @brief Translator to target specific mutex lock function.
 */
// For versions earlier than ARMv6
__inline void tracer_mutex_lock(void)
{
   uint32 uLockVal;
   uint32 uLocked = 1;
   uint32 uTemp = 0;
__asm
{
LOOP1:
   SWP      uLockVal, uLocked, [tracerInitMutex]
   CMP      uLockVal, #1      // Already in use?
   BNE      LEND              // Not in use, proceed.

   // Wait for turn, then try again
   MCR      p15, 0, uTemp, c7, c0, 4 // Wait for Interrupt
   B        LOOP1             // Try again

LEND:                         // Prepare IMB
   MCR      p15, 0, uTemp, c7, c10, 3  // Clear dcache
   MCR      p15, 0, uTemp, c7, c10, 4  // Drain write buffer
   MCR      p15, 0, uTemp, c7, c5, 0   // Invalidate icache
}

   return;
}
/**
  @brief Translator to target specific mutex unlock function.
 */
__inline void tracer_mutex_unlock(void)
{
   uint32 uUnlocked = 0;
   uint32 uTemp = 0;
__asm
{
   MCR      p15, 0, uTemp, c7, c10, 1  // Clean dcache
   MCR      p15, 0, uTemp, c7, c10, 4  // Drain write buffer
   MCR      p15, 0, uTemp, c7, c5, 1   // Invalidate icache
   STR      uUnlocked, [tracerInitMutex]
   MCR      p15, 0, uTemp, c7, c10, 4  // Drain write buffer

  //  need an iSignal to wake waiting tasks. // Inform others lock is free.
}
   return;
}

// For versions ARMv6 or later. Untested.
/*
__inline void tracer_mutex_lock2(void)
{
   uint32 uLockVal;
   uint32 uLocked = 1;
__asm
{
LOOP1:
   LDREX    uLockVal, [tracerInitMutex]
   CMP      uLockVal, #1
   BEQ      LWAIT
   STREXNE  uLockVal, uLocked, [tracerInitMutex]
   CMPNE    uLockVal, #1
   BEQ      LOOP1
   B        LEND
LWAIT:
   WFE                  // wait
   B        LOOP1
LEND:
   DMB                  //Complete earlier writes.
}

   return;
}
__inline void tracer_mutex_unlock2(void)
{
uint32 uUnlock = 0;
   __asm
{
   DMB      // ensure earlier work completed.
   STREX    uUnlock, [tracerInitMutex]
   DSB      // Ensure mutex updated.
   SEV      // Wake up anyone waiting.
}
   return;
}
*/

#endif /* #ifndef TRACER_MUTEX_H */

