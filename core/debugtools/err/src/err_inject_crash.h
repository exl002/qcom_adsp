#ifndef ERR_INJECT_CRASH_H
#define ERR_INJECT_CRASH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR service crash simulator
                
GENERAL DESCRIPTION
  Command handler for injecting various types of crashes for testing

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.adsp/2.2/debugtools/err/src/err_inject_crash.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/28/11   mcg     File created.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

                     EXTERNAL DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  ERR_INJECT_ERR_FATAL,
  ERR_INJECT_WDOG_TIMEOUT,
  ERR_INJECT_NULLPTR,
  ERR_INJECT_DIV0,

  SIZEOF_ERR_INJECT_CRASH_TYPE
} err_inject_crash_type;

/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
void err_inject_crash_init(void);





#endif /* ERR_INJECT_CRASH_H */
