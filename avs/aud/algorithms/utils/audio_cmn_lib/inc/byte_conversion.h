/*========================================================================*/
/**
@file byte_conversion.h

This file contains declaration for byte conversion routine
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
Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All rights reserved.
Qualcomm Confidential and Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/aud/algorithms/utils/audio_cmn_lib/inc/byte_conversion.h#7 $

when       who     what, where, why
--------   ---     -------------------------------------------------------

========================================================================== */

#ifndef __BYTE_CONV__
#define __BYTE_CONV__

#include "AudioComdef.h"
#include "audio_basic_op.h"
#include "audio_basic_op_ext.h"

typedef enum
{
	BYTE_UP_CONV = 0,
	BYTE_DOWN_CONV
}byte_conv;


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


void byte_up_down_convertor_deintlv(int8_t * ptr_src, int8_t * ptr_out, uint32_t num_samp_per_ch, \
												uint32_t num_channels, uint32_t chan_spacing_in, \
												uint32_t chan_spacing_out, uint16_t conv_mode);

void byte_up_down_convertor_intlv(int8_t * ptr_src_buf, int8_t * ptr_out_buf, 
											 uint32_t total_num_samples, uint16_t conv_mode);

ADSPResult byte_convertor_intlv_16_out(int8_t * ptr_src_buf, int8_t * ptr_out_buf, uint32_t total_num_samples,
      uint16_t in_word_size, uint16_t in_Q_fact);

ADSPResult byte_convertor_intlv_32_out(int8_t *ptr_src_buf, int8_t *ptr_out_buf, uint32_t total_num_samples,
      uint16_t in_word_size, uint16_t in_Q_fact);


ADSPResult byte_convertor_with_intlv_16_in(int8_t * ptr_src_buf, int8_t * ptr_out_buf, uint32_t num_sampl_p_ch,
      uint32_t num_channels, uint16_t out_word_size, uint16_t out_Q_fact, uint16_t out_bits_p_sample,
      bool_t is_in_intlvd , uint32_t channel_spacing_samples);

ADSPResult byte_convertor_with_intlv_32_in(int8_t * ptr_src_buf, int8_t * ptr_out_buf, uint32_t num_sampl_p_ch,
      uint32_t num_channels, uint16_t out_word_size, uint16_t out_Q_fact,  uint16_t out_bits_p_sample,
      bool_t is_in_intlvd , uint32_t channel_spacing_samples);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BYTE_CONV__ */
