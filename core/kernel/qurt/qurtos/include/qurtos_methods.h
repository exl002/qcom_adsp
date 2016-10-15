#ifndef   QURTOS_METHODS_H
#define   QURTOS_METHODS_H

/*
||  qurtos_methods.h
||
||  Defines the private QDI methods used within the
||   QuRTOS layer.  In general, these methods are not
||   documented or used outside of this directory.
||
||  Nothing in this file is intended to be a public API and
||   everything in this file is to be considered subject to
||   change at any time.
*/

#define QDI_CLIENT_HANDLE_THREAD_ADD            192     /* Add a thread to a client handle */
#define QDI_CLIENT_HANDLE_THREAD_REMOVE         193     /* Delete a thread from a client handle */
#define QDI_CLIENT_HANDLE_GET_SINGLE_HANDLE     194     /* Get a "singleton" handle for an object */
                                                        /* If called multiple times without a close, */
                                                        /*  it should return the same handle each time. */

static inline int qurtos_user_client_add_thread(int handle,
                                                struct qurtos_thread_info *ptr)
{
   return qurt_qdi_handle_invoke(handle, QDI_CLIENT_HANDLE_THREAD_ADD, ptr);
}

static inline int qurtos_user_client_remove_thread(int handle,
                                                   struct qurtos_thread_info *ptr,
                                                   int exitstatus)
{
   return qurt_qdi_handle_invoke(handle, QDI_CLIENT_HANDLE_THREAD_REMOVE, ptr, exitstatus);
}

#endif /* QURTOS_METHODS_H */
