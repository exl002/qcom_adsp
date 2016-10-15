/*===========================================================================
  FILE: tracer_portget.c

  OVERVIEW: Get a STM port from the port pool. Extension of tracer_portmgr.c

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "rex.h"
#include "tracer_portmgr.h"

/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/

#define INT_LOCK()     rex_int_lock()
#define INT_UNLOCK()   rex_int_free()

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/

//=========================================================================//
//Caller can check that port manager is initialized before calling this
//function.
STMTracePortType tracer_port_get(int *key)
{
   int myCount = 0;  //Escape route.
   int myKey;

   if (TRUE == bPortMgrInitialized) // Safeguard.
   {
      INT_LOCK();
      do {
         myKey = _gPortMgr.nextKey;
         _gPortMgr.nextKey = ((myKey + 1) >= TRACER_PORT_POOL_SIZE) ? 0 : (myKey + 1);
         myCount++;
         if (myCount > TRACER_PORT_POOL_SIZE)
         {  // Get out if can't find a port
            INT_UNLOCK();
            *key = ~0;
            return (_gPortMgr.trashPort);
         }
      } while (1 == _gPortMgr.pool[myKey].inUseFlag);
      _gPortMgr.pool[myKey].inUseFlag = 1;

      // Track usage
      _gPortMgr.inUseCount++;
      if (_gPortMgr.inUseHiMark < _gPortMgr.inUseCount)
      {
         _gPortMgr.inUseHiMark = _gPortMgr.inUseCount;
      }

      INT_UNLOCK();
      *key = myKey;  // Return key value
      return (_gPortMgr.pool[myKey].port); // Return port number.
   }
   else
   {
      INT_UNLOCK();
      *key = ~0;
      return 0;
   }
}

//=========================================================================//
void tracer_port_free(int key)
{
   if (~0 != key)
   {
      INT_LOCK();
      _gPortMgr.pool[key].inUseFlag = 0;
      _gPortMgr.inUseCount--;
      INT_UNLOCK();
   }
   return;
}

