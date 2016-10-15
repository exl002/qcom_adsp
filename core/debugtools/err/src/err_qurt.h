#ifndef ERR_QURT_H
#define ERR_QURT_H

/*===========================================================================

                    Error Handling QURT OSAL Header File

Description
  ERR Services OS Abstraction for QURT OS 

Copyright (c) 2012-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.adsp/2.2/debugtools/err/src/err_qurt.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/12   mcg     Initial creation

===========================================================================*/

#include "qurt.h"
#include "qurt_event.h"




#define ERR_MUTEX_TYPE qurt_mutex_t
#define ERR_MUTEX_INIT qurt_pimutex_init
#define ERR_MUTEX_LOCK qurt_pimutex_lock
#define ERR_MUTEX_UNLOCK qurt_pimutex_unlock

#define ERR_FATAL_ENTER_SINGLE_THREADED_MODE qurt_exception_raise_fatal
#define ERR_FATAL_FLUSH_CACHE_NO_RETURN qurt_exception_raise_shutdown_fatal

extern unsigned int qurt_exception_wait(unsigned int*, unsigned int*, unsigned int*, unsigned int*);
#define ERR_REG_EXHNDLR qurt_exception_wait




#endif //ERR_QURT_H
