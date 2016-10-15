#ifndef TRACER_MUTEX_H
#define TRACER_MUTEX_H
/*===========================================================================
  @file tracer_mutex.h

   tracer_lock mutex
   To be included by only one file unless modified.

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "DALSys.h"

extern DALSYSSyncHandle hSync;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

__inline void tracer_mutex_init(void)
{
   if (hSync==NULL)
   {
      DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &hSync, NULL);
   }
   return;
}
__inline void tracer_mutex_destroy(void)
{
   return;
}
__inline void tracer_mutex_lock(void)
{
   DALSYS_SyncEnter(hSync);

}
__inline void tracer_mutex_unlock(void)
{
   DALSYS_SyncLeave(hSync);
}

#endif /* #ifndef TRACER_MUTEX_H */

