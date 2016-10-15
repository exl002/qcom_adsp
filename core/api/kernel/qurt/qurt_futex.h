#ifndef QURT_FUTEX_H
#define QURT_FUTEX_H
/**
  @file  qurt_futex.h

  @brief  Prototypes of Kernel futex API functions      
  Futex calls directly in case you want to play with them from C
  
 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_futex.h#11 $ 
 	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
  2012-12-15    cm          (Tech Pubs) Edited/added Doxygen comments and markup.
 ======================================================================*/
/*=====================================================================
 Functions
======================================================================*/

int qurt_futex_wait(void *lock, int val);
int qurt_futex_wait64(void *lock, long long val);
int qurt_futex_wake(void *lock, int n_to_wake);

#endif /* QURT_FUTEX_H */

