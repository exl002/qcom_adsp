#ifndef QURT_LIFO_H
#define QURT_LIFO_H
/**
  @file qurt_lifo.h

  @brief
  Provide lock free LastInFirstOut algorithm, which can be used in a
  variety of situations for allocation/free fixed size buffer    
  This implementation will touch the first word of your FREED buffer. Even
	though it doesn't matter how you use it when it is allocated, you might want
	to be a bit careful not to put your MAGIC number as the first field.
	Because it will not hold the magic value for "freed"

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
 
 	$Header: //components/rel/core.adsp/2.2/kernel/qurt/libs/qurt/include/public/qurt_lifo.h#6 $ 
 	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
  2012-12-16    cm          (Tech Pubs) Edited/added Doxygen comments and markup.
 ======================================================================*/
	/*=====================================================================
	 Functions
	======================================================================*/

/*======================================================================*/
/**
  Pops an element out the LIFO 

  @param[in] freelist  Pointer to the head of your list 
	
  @return 				
  The top object from the list 
		 
  @dependencies
  None.
*/
/* ======================================================================*/
void * qurt_lifo_pop(void *freelist);

 
/*======================================================================*/
/**
  Pushes an element into the LIFO
	
		@param freelist [IN] pointer to the head of your list 
	  @param buf [IN] pointer to your buffer to push into the list 
	
  @return
  None.
		 
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_lifo_push(void *freelist, void *buf);

void qurt_lifo_remove(void *freelist, void *buf);
#endif /* QURT_LIFO_H */

