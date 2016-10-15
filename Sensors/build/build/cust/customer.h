#ifndef CUSTOMER_H
#define CUSTOMER_H
/*===========================================================================

                   C U S T O M E R    H E A D E R    F I L E

DESCRIPTION
  This header file provides customer specific information for the current
  build.  It expects the compile time switch /DCUST_H=CUSTxxxx.H.  CUST_H
  indicates which customer file is to be used during the current build.
  Note that cust_all.h contains a list of ALL the option currently available.
  The individual CUSTxxxx.H files define which options a particular customer
  has requested.


Copyright (c) 2010       by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/build/build/cust/customer.h#1 $ 
$DateTime: 2014/05/16 10:56:56 $ 
$Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/28/10    ag     Initial revision

===========================================================================*/


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Make sure that CUST_H is defined and then include whatever file it
** specifies.
*/
#ifdef CUST_H
#include CUST_H
#else
#error Must Specify /DCUST_H=CUSTxxxx.H on the compile line
#endif

/* Now perform certain Sanity Checks on the various options and combinations
** of option.  Note that this list is probably NOT exhaustive, but just
** catches the obvious stuff.
*/

//#if defined( FEATURE_UI_ENHANCED_PRIVACY_SET ) && defined( FEATURE_VOICE_PRIVACY_SWITCH)
//#error defined( FEATURE_UI_ENHANCED_PRIVACY_SET ) && defined( FEATURE_VOICE_PRIVACY_SWITCH)
//#endif

#endif /* CUSTOMER_H */
