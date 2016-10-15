
/*
  Copyright (C) 2009-2010 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR.

  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/apr/apr_pack_begin.h#9 $
  $Author: svutukur $
*/

#if defined( __GNUC__ )
#elif defined( __arm__ )
  __packed
#elif defined( _MSC_VER )
  #pragma warning( disable:4103 )  /* Another header changing "pack". */
  #pragma pack( push, 1 )
#else
  #error Unsupported compiler.
#endif /* __GNUC__ */

