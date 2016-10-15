#ifndef QURT_QDI_IMACROS_H
#define QURT_QDI_IMACROS_H

/**
  @file  qurt_qdi_imacros.h 
  @brief  Internal macros used for QDI. Mostly consists of tricky (and ugly)
  preprocessor hacks which permit us to do varargs function invocations
  where we pass optional arguments in registers and where we can do
  type casting and checking automatically.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2012 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.adsp/2.2/api/kernel/qurt/qurt_qdi_imacros.h#11 $
 	$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2012-02-29 	Jack			Add Header file
 ======================================================================*/

#define _QDMPASTE(a,b) _QDMPASTE_(a,b)
#define _QDMPASTE_(a,b) a##b
#define _QDMCNT(...) _QDMCNT_(__VA_ARGS__,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define _QDMCNT_(a,b,c,d,e,f,g,h,i,j,k,l,cnt,...) cnt

#endif
