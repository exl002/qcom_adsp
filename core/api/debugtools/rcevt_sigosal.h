#ifndef _RCEVT_SIGOSAL_H
#define _RCEVT_SIGOSAL_H

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

 $Header: //components/rel/core.adsp/2.2/api/debugtools/rcevt_sigosal.h#1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------

 ===========================================================================*/

#if defined(__GNUC__)
#pragma message "UNSUPPORTED API, use RCEVT_REX.H instead of RCEVT_SIGOSAL.H"
#elif defined(__ARMCC_VERSION)
#warning "UNSUPPORTED API, use RCEVT_REX.H instead of RCEVT_SIGOSAL.H"
#else
#warning "UNSUPPORTED API, use RCEVT_REX.H instead of RCEVT_SIGOSAL.H"
#endif

//#include "rex.h"
//#include "rcevt.h"

#include "rcevt_rex.h"

#if defined(__cplusplus)
extern "C" {
#endif

//#define RCEVT_SIGOSAL_INCLUDE_ATTRIBUTE __attribute__ ((warning("UNSUPPORTED API, use RCEVT_REX.H instead of RCEVT_SIGOSAL.H")))
//
//typedef struct RCEVT_SIGOSAL_INCLUDE_ATTRIBUTE
//{
//   rex_tcb_type* signal;   // context for delivery
//   rex_sigs_type mask;     // payload for delivery
//
//} RCEVT_SIGEX_SIGOSAL;
//
#define RCEVT_SIGEX_SIGOSAL RCEVT_SIGEX_SIGREX // type must change from SIGOSAL to SIGREX
#define RCEVT_SIGEX_TYPE_SIGOSAL RCEVT_SIGEX_TYPE_SIGREX // type must change from SIGOSAL to SIGREX

#if defined(__cplusplus)
}
#endif

#endif
