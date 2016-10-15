/*===========================================================================
  @file tracer_mutex.c

  Tracer mutex

               Copyright (c) 2011 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.adsp/2.2/debugtrace/tracer/src/armv5/tracer_mutex.c#1 $ */

#include "tracer.h"

/* Tracer initialization mutex
   Must be 4-bytes and aligned.
 */
volatile uint32 tracerInitMutex = 0;     //0 = unlocked; 1 = locked

