/*========================================================================*/
/**
@file audio_log2.h

This file contains declaration for log routine to the base 2.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The elite_audio_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      elite_audio_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All rights reserved.
Qualcomm Confidential and Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/aud/algorithms/utils/audio_cmn_lib/inc/audio_log2.h#6 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/20/10   sw,kli  (Tech Pubs) Edited/added Doxygen comments and markup.
========================================================================== */

#ifndef _LOG2
#define _LOG2

#include "AudioComdef.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup math_operations
@{ */

/**
  Logarithm function. It computes log2 of the input.

  @param[in] Linput   40-bit input.

  @return
  int32_t log2(Linput)

  @dependencies
  None.
*/
int32_t log2_lut(int40 Linput);

/** @} */ /* end_addtogroup math_operations */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_LOG2*/
