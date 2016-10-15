#ifndef __RFSA_LOCK_H__
#define __RFSA_LOCK_H__
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

  $Header: //components/rel/core.adsp/2.2/storage/rfa_client/common/rfsa_lock.h#1 $
  $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2012-03-22   rh      Initial checkin
=============================================================================*/

typedef void* rfsa_lock_t;


int32_t rfsa_lock_create ( rfsa_lock_t* ret_lock );
int32_t rfsa_lock_destroy ( rfsa_lock_t lock );
int32_t rfsa_lock_enter ( rfsa_lock_t lock );
int32_t rfsa_lock_leave ( rfsa_lock_t lock );

#endif /* __RFSA_LOCK_H__ */

