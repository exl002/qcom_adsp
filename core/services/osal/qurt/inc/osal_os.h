#ifndef __OSAL_OS_H__
#define __OSAL_OS_H__
/***********************************************************************
 * osal_os.h
 *
 * Copyright (C) 2011 Qualcomm Technologies, Inc.
 *
 * This file header file includes all os-dependent header files.
 * 
 **********************************************************************/

/*===========================================================================


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2011-09-15    sg   Created File 
===========================================================================*/

#include "comdef.h"
#include "osal_qurt_types.h"

/* The lock and unlock mutex APIs are function calls in WM, qube, qurt
 and map to macros in rex */
/*===========================================================================
  FUNCTION   OSAL_LOCK_MUTEX

  DESCRIPTION
       Locks a mutex

  PARAMETERS
       mutex - Address of Mutex object
  
  DEPENDENCIES
       None
    
   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
       OSAL_NULL_PTR_RECVD 2 
           
  SIDE EFFECTS
       None
  ===========================================================================*/
  void osal_lock_mutex(osal_mutex_arg_t *mutex);
  
/*===========================================================================
  FUNCTION   OSAL_UNLOCK_MUTEX

  DESCRIPTION
       Unlocks the mutex
    
  PARAMETERS
       mutex - Address of Mutex object
  
  DEPENDENCIES
       None

   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
       OSAL_NULL_PTR_RECVD 2 
           
  SIDE EFFECTS
       None
  ===========================================================================*/
  void osal_unlock_mutex(osal_mutex_arg_t *mutex);
  int osal_atomic_compare_and_set(osal_atomic_word_t *target,
                       osal_atomic_plain_word_t old_val,
                       osal_atomic_plain_word_t new_val );

void osal_atomic_set_bit(osal_atomic_word_t *target, unsigned long bit);

void osal_atomic_clear_bit(osal_atomic_word_t *target, unsigned long bit);

void osal_atomic_add(osal_atomic_word_t *target, osal_atomic_plain_word_t v);


#define osal_disable_interrupts()
#define osal_enable_interrupts()
  
#endif /* not __OSAL_OS_H__ */

