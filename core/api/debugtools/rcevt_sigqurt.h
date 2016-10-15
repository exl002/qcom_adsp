#ifndef _RCEVT_QURT_H
#define _RCEVT_QURT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 RCE - RUN CONTROL EVENT API

 GENERAL DESCRIPTION
 this module contains the source implementation for the event notification
 service

 EXTERNALIZED FUNCTIONS
 yes

 INITIALIZATION AND SEQUENCING REQUIREMENTS
 yes

 Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcevt_sigqurt.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#if defined(__GNUC__)
#pragma message "UNSUPPORTED API, use RCEVT_QURT.H instead of RCEVT_SIGQURT.H"
#elif defined(__ARMCC_VERSION)
#warning "UNSUPPORTED API, use RCEVT_QURT.H instead of RCEVT_SIGQURT.H"
#else
#warning "UNSUPPORTED API, use RCEVT_QURT.H instead of RCEVT_SIGQURT.H"
#endif

//#include "qurt.h"
//#include "rcevt.h"

#include "rcevt_qurt.h"

#if defined(__cplusplus)
extern "C" {
#endif

//#define RCEVT_SIGOSAL_INCLUDE_ATTRIBUTE __attribute__ ((warning("UNSUPPORTED API, use RCEVT_QURT.H instead of RCEVT_SIGQURT.H")))
//
//typedef struct RCEVT_SIGOSAL_INCLUDE_ATTRIBUTE
//{
//   qurt_anysignal_t* signal;    // context for delivery
//   unsigned int mask;            // payload for delivery
//
//} RCEVT_SIGEX_SIGOSAL;
//
//#define RCEVT_SIGEX_SIGOSAL RCEVT_SIGEX_SIGQURT // type must change from SIGOSAL to SIGQURT
//#define RCEVT_SIGEX_TYPE_SIGOSAL RCEVT_SIGEX_TYPE_SIGQURT // type must change from SIGOSAL to SIGQURT

#if defined(__cplusplus)
}
#endif

#endif
