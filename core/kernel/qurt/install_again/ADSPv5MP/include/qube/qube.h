#ifndef QUBE_H
#define QUBE_H
/*=============================================================================

                 qube.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qpd API

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/kernel/qurt/libs/qube/include/public/qube.h#6 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include <qurt.h>
#include <qmutex.h>

/* Define Error codes as QuRT error codes preceed with QURT_ */
#ifndef EOK
#define EOK                             QURT_EOK
#endif /* EOK */
#ifndef EVAL
#define EVAL                            QURT_EVAL
#endif /* EVAL */
#ifndef EMEM
#define EMEM                            QURT_EMEM
#endif /* EMEM */
#ifndef EINVALID
#define EINVALID                        QURT_EINVALID
#endif /* EINVALID */


/*=============================================================================
                      FUNCTION DECLARATIONS                                
=============================================================================*/

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QUBE_H */
