/******************************************************************************
 * rfsa_lock.c
 *
 * Implement platform dependent critical sections
 *
 * Copyright (c) 2012
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/storage/rfa_client/common/rfsa_lock.c#1 $
  $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2012-03-22   rh      Initial checkin
=============================================================================*/


#include <stddef.h>
#include <stdlib.h>
#include "qurt.h"
#include "rfsa_common.h"
#include "rfsa_lock.h"


typedef struct rfsa_int_lock_t
{
   qurt_mutex_t rfsa_mutex;
}
rfsa_int_lock_t;


int32_t rfsa_lock_create ( rfsa_lock_t* ret_lock )
{
   rfsa_int_lock_t* the_lock;

   if ( ret_lock == NULL )
   {
      return RFSA_EFAILED;
   }

   the_lock = ( ( rfsa_int_lock_t* ) malloc( sizeof( rfsa_int_lock_t ) ) );
   if ( the_lock == NULL )
   {
      return RFSA_EFAILED;
   }

   qurt_rmutex_init (&the_lock->rfsa_mutex);

   *ret_lock = the_lock;

   return RFSA_EOK;
}


int32_t rfsa_lock_destroy ( rfsa_lock_t lock )
{
   rfsa_int_lock_t* the_lock = lock;

   if ( the_lock == NULL )
   {
      return RFSA_EFAILED;
   }

   qurt_rmutex_destroy (&the_lock->rfsa_mutex);
   free( the_lock );

   return RFSA_EOK ;
}


int32_t rfsa_lock_enter ( rfsa_lock_t lock )
{
   rfsa_int_lock_t* the_lock = lock;

   if ( the_lock == NULL )
   {
      return RFSA_EFAILED;
   }

   qurt_rmutex_lock (&the_lock->rfsa_mutex);

   return RFSA_EOK;
}


int32_t rfsa_lock_leave ( rfsa_lock_t lock )
{
   rfsa_int_lock_t* the_lock = lock;

   if ( the_lock == NULL )
   {
      return RFSA_EFAILED;
   }

   qurt_rmutex_unlock (&the_lock->rfsa_mutex);

   return RFSA_EOK;
}

