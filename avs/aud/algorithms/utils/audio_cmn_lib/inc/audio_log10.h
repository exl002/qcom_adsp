/*========================================================================*/
/**
@file audio_log10.h

This file contains ASM data commands and events structures definitions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The elite_audio_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/* ====================================================================== 
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All rights reserved.
Qualcomm Confidential and Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/aud/algorithms/utils/audio_cmn_lib/inc/audio_log10.h#6 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/09/10   rkc      Created file.
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */

#ifndef _LOG10_H_
#define _LOG10_H_

#include "AudioComdef.h"

/*=============================================================================
      Function Declarations 
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup math_operations
@{ */

/**
  Fixed point function for log10. It calculates 10*log10(input) using a table 
  lookup for log2(input): 10*log10(input)=3.0103*log2(input).

  @param[in] input    Input value.

  @return
  Signed 32-bit value 10log10(input).

  @dependencies
  None.
*/
int32_t log10_fixed(int32_t input); /* computes 10log10 of int32_t input */

/** @} */ /* end_addtogroup math_operations */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //_LOG10_H_

