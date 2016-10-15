#ifndef QURT_TLS_H
#define QURT_TLS_H
/**
  @file qurt_tls.h 
  @brief  Prototypes of TLS APIs 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2009 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_tls.h#11 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
12/12/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.

=============================================================================*/

/*=============================================================================
												FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_tls_create_key
  @xreflabel{sec:tls_create_key}
  Creates a key for accessing a thread local storage data item.\n
  The key value is used in subsequent get and set operations.

  @note1hang The destructor function performs any clean-up operations needed by a thread
             local storage item when its containing thread is deleted (Section @xref{sec:qurt_thread_exit}).

  @param[out] key         Pointer to the newly-created thread local storage key value.
  @param[in]  destructor  Pointer to the key-specific destructor function. Passing NULL 
                          specifies that no destructor function is defined for the key.

  @return	
  QURT_EOK -- Key successfully created. \n
  QURT_ETLSAVAIL -- No free TLS key available. 

  @dependencies
  None.
 */
int qurt_tls_create_key (int *key, void (*destructor)(void *));

/**@ingroup func_qurt_tls_set_specific
  Stores a data item to thread local storage along with the specified key.

  @param[in]    key  Thread local storage key value.
  @param[in]    value  Pointer to user data value to store.

  @return  
  QURT_EOK -- Data item successfully stored. \n
  QURT_EINVALID -- Invalid key. \n
  QURT_EFAILED -- Invoked from a non-thread context.
 */
int qurt_tls_set_specific (int key, const void *value);

/**@ingroup func_qurt_tls_get_specific
  Loads the data item from thread local storage. \n
  Returns the data item that is stored in thread local storage with the specified key.
  The data item is always a pointer to user data.

  @param[in]    key Thread local storage key value.

  @return
  Pointer -- Data item indexed by key in thread local storage. \n
  0 (NULL) -- Key out of range.

  @dependencies
  None.
 */
void *qurt_tls_get_specific (int key);


/**@ingroup func_qurt_tls_delete_key
  Deletes the specified key from thread local storage.

  @note1hang Explicitly deleting a key does not execute any destructor function that is
             associated with the key (Section @xref{sec:tls_create_key}).

  @param[in]   key  Thread local storage key value to delete.

  @return  
  QURT_EOK -- Key successfully deleted. \n
  QURT_ETLSENTRY -- Key already free.

  @dependencies
  None.
 */
int qurt_tls_delete_key (int key);


#endif /* QURT_TLS_H */
