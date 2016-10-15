#ifndef QURTOS_SCLK_H
#define QURTOS_SCLK_H
/*=============================================================================

                 qurt_sclk.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   The header file that describes the APIs supported by QURT system sclk
   feature.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010 - 2011
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/kernel/qurt/qurtos/include/qurtos_sclk.h#6 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/08/10   balac   File created.
03/03/11   op      Fix warning when running doxygen 
=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/*=============================================================================

                           DATA DECLARATIONS

=============================================================================*/

/**
 * Max. client supported by QURT system sclk module
 */
#define QURT_SCLK_CLIENTS_MAX                      4

/*=============================================================================
                        TYPEDEFS
=============================================================================*/

typedef unsigned int                        qurt_sclk_t;

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif /* __cplusplus */

#endif /* QURTOS_SCLK_H */
