/*========================================================================*/
/**
@file adsp_media_fmt.h

This file contains the structure definitions for the media format
blocks used in client-ASM communications.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      audio_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*===========================================================================
Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */

/*===========================================================================
Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.

License to copy and use this software is granted provided that it is identified
as the "RSA Data Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing this software or this function.

License is also granted to make and use derivative works provided that such
works are identified as "derived from the RSA Data Security, Inc. MD5
Message-Digest Algorithm" in all material mentioning or referencing the
derived work.

RSA Data Security, Inc. makes no representations concerning either the
merchantability of this software or the suitability of this software for any
particular purpose. It is provided "as is" without express or implied warranty
of any kind.

These notices must be retained in any copies of any part of this documentation
and/or software.
======================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/audio/inc/adsp_media_fmt.h#17 $

when       who     what, where, why
--------   ---      -------------------------------------------------------
09/06/13   sw      (Tech Pubs) Merged Doxygen comments/markup from 2.0; edited 
                    new Doxygen comments for 2.2.
08/06/12   sw      (Tech Pubs) Edited comments; updated Doxygen markup to
                   Rev D.2 templates; updated legal statements for QTI.
05/30/11   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.

========================================================================== */

#ifndef _ADSP_MEDIA_FMT_H_
#define _ADSP_MEDIA_FMT_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * Include file to add new media format ID and media format block. This file is present at adsp_proc\hap\integration\audencdec
 * To include this file, set OEM_ROOT from command prompt
 */
#if defined(OEM_ROOT)
#include "custom_adsp_media_fmt.h"
#endif

/** @ingroup asmstrm_mediafmt_eg
    Media format example for clients to try out a dummy encoder or decoder.
    This is for illustration purposes only.

    @par Example format block
    No format block is required.

    @par Example encode configuration block
    No format block is required.
*/
#define ASM_MEDIA_FMT_EXAMPLE                   0x00010C4D

/** @addtogroup asmstrm_mediafmt_pcm_ch_defs
@{ */

/** Front left channel. */
#define PCM_CHANNEL_L    1

/** Front right channel. */
#define PCM_CHANNEL_R    2

/** Front center channel. */
#define PCM_CHANNEL_C    3

/** Left surround channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_LS   4

/** Right surround channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_RS   5

/** Low frequency effect channel. */
#define PCM_CHANNEL_LFE  6

/** Center surround channel; rear center channel. */
#define PCM_CHANNEL_CS   7

/** Center back channel. */
#define PCM_CHANNEL_CB  PCM_CHANNEL_CS

/** Left back channel; rear left channel. */
#define PCM_CHANNEL_LB   8

/** Right back channel; rear right channel. */
#define PCM_CHANNEL_RB   9

/** Top surround channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_TS   10

/** Center vertical height channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_CVH  11

/** Top front center channel.

    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_TFC  PCM_CHANNEL_CVH

/** Mono surround channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_MS   12

/** Front left of center channel. */
#define PCM_CHANNEL_FLC  13

/** Front right of center channel. */
#define PCM_CHANNEL_FRC  14

/** Rear left of center channel. */
#define PCM_CHANNEL_RLC  15

/** Rear right of center channel. */
#define PCM_CHANNEL_RRC  16

/** Secondary low frequency effect channel. */
#define PCM_CHANNEL_LFE2  17

/** Side left channel. */
#define PCM_CHANNEL_SL  18

/** Side right channel. */
#define PCM_CHANNEL_SR  19

/** Top front left channel. */
#define PCM_CHANNEL_TFL  20

/** Left vertical height channel. */
#define PCM_CHANNEL_LVH  PCM_CHANNEL_TFL

/** Top front right channel. */
#define PCM_CHANNEL_TFR 21

/** Right vertical height channel. */
#define PCM_CHANNEL_RVH PCM_CHANNEL_TFR

/** Top center channel. */
#define PCM_CHANNEL_TC  22

/** Top back left channel. */
#define PCM_CHANNEL_TBL  23

/** Top back right channel. */
#define PCM_CHANNEL_TBR  24

/** Top side left channel. */
#define PCM_CHANNEL_TSL  25

/** Top side right channel. */
#define PCM_CHANNEL_TSR  26

/** Top back center channel. */
#define PCM_CHANNEL_TBC  27

/** Bottom front center channel. */
#define PCM_CHANNEL_BFC  28

/** Bottom front left channel. */
#define PCM_CHANNEL_BFL  29

/** Bottom front right channel. */
#define PCM_CHANNEL_BFR  30

/** Left wide channel. */
#define PCM_CHANNEL_LW  31

/** Right wide channel. */
#define PCM_CHANNEL_RW  32

/** Left side direct channel. */
#define PCM_CHANNEL_LSD  33

/** Right side direct channel. */
#define PCM_CHANNEL_RSD  34

/** @} */ /* end_addtogroup asmstrm_mediafmt_pcm_ch_defs */

/** @ingroup asmstrm_mediafmt_multich_pcm
    Media format ID for multiple channel linear PCM.

    @par Multiple Channel PCM format block (asm_multi_channel_pcm_fmt_blk_v2_t}
    @tablens{weak__asm__multi__channel__pcm__fmt__blk__v2__t}

    @par Multiple Channel PCM encode configuration block (asm_multi_channel_pcm_enc_cfg_v2_t)
    @table{weak__asm__multi__channel__pcm__enc__cfg__v2__t}
*/
#define ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2                0x00010DA5

/* Linear multiple channel PCM decoder format block structure. */
typedef struct asm_multi_channel_pcm_fmt_blk_v2_t asm_multi_channel_pcm_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_fmt_blk_v2_t
@{ */
/* Payload of the Multiple Channel PCM decoder format block parameters
    ASM_STREAM_CMD_OPEN_WRITE_V2 command. */
/** The data must be in little-endian format, and it must always be
    interleaved.
*/
struct asm_multi_channel_pcm_fmt_blk_v2_t
{
    uint16_t  num_channels;
    /**< Number of channels.

         @values 1 to 8 */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 24

         When used for playback, the client must send 24-bit samples packed in
         32-bit words. The 24-bit samples must be placed in the most
         significant 24 bits of the 32-bit word.

         When used for recording, the aDSP sends 24-bit samples packed in
         32-bit words. The 24-bit samples are placed in the most significant
         24 bits of the 32-bit word. */

    uint32_t  sample_rate;
    /**< Number of samples per second.

         @values 2000 to 48000, 96000, 192000 Hz */

    uint16_t  is_signed;
    /**< Flag that indicates the PCM samples are signed (1). */

    uint16_t  reserved;
    /**< This field must be set to zero. */

    uint8_t   channel_mapping[8];
    /**< Channel array of size 8. Channel[i] mapping describes channel i. Each
         element i of the array describes channel i inside the buffer where 0
         @le i < num_channels. An unused channel is set to zero. 

         @values See Section @xref{hdr:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_fmt_blk_v2_t */

/* Multichannel PCM encoder configuration structure. */
typedef struct asm_multi_channel_pcm_enc_cfg_v2_t asm_multi_channel_pcm_enc_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_enc_cfg_v2_t
@{ */
/* Payload of the multichannel PCM encoder configuration parameters in
    the ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 media format.
*/
struct asm_multi_channel_pcm_enc_cfg_v2_t
{
   uint16_t  num_channels;
    /**< Number of PCM channels.

         @values
         - 0 -- Native mode
         - 1 -- 8

         Native mode indicates that encoding must be performed with the number
         of channels at the input. */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 24 */

    uint32_t  sample_rate;
    /**< Number of samples per second.

         @values 0, 8000 to 48000 Hz

         A value of 0 indicates the native sampling rate. Encoding is
         performed at the input sampling rate. */

    uint16_t  is_signed;
    /**< Flag that indicates the PCM samples are signed (1). Currently, only
         signed samples are supported. */

    uint16_t  reserved;
    /**< This field must be set to zero. */

    uint8_t   channel_mapping[8];
    /**< Channel mapping array expected at the encoder output.
         Channel[i] mapping describes channel i inside the buffer, where
         0 @le i < num_channels. All valid used channels must be present at
         the beginning of the array.

         If native mode is set for the channels, this field is ignored.

         @values See Section @xref{hdr:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_enc_cfg_v2_t */

/** @cond OEM_only */
/** @addtogroup asmstrm_mediafmt_adpcm
@{ */
/** Media format ID for adaptive differential PCM.

    @par ADPCM format block (asm_adpcm_fmt_blk_t)
    The encoder configuration block is the same as the decoder format block,
    with some differences in the values.
    @tablens{weak__asm__adpcm__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_ADPCM                     0x00010BE7

/* ADPCM decoder format block structure. */
typedef struct asm_adpcm_fmt_blk_t asm_adpcm_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_adpcm_fmt_blk_t
@{ */
/* Payload of the ADPCM encoder/decoder format block parameters in the
    ASM_MEDIA_FMT_ADPCM media format.
*/
struct asm_adpcm_fmt_blk_t
{
    uint16_t          num_channels;
    /**< Number of PCM channels.

         @values
         - 0 -- Native mode (encoder only)
         - 1 -- Mono (decoder and encoder)
         - 2 -- Stereo (decoder and encoder) @tablebulletend */

    uint16_t          bits_per_sample;
    /**< Number of bits per sample per channel.

         @values
         - 4 -- Decoder
         - 16 -- Encoder @tablebulletend */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values{for the decoder}
         - 2000 to 48000 Hz

         @values{for the encoder}
         - 0 -- Native mode
         - 1 -- 2000 to 48000

         Native mode indicates that encoding must be performed with the
         sampling rate at the input. */

    uint32_t          blk_size;
    /**< Block size for the decoder/encoder.

         @subhd{For the decoder}
         - Value that is retrieved from the bitstream by the parser (upper
           layer).
         - The parser must parse it and send it to the aDSP.

         @values Embedded in the bitstream

         @subhd{For the encoder}
         - Size of the block, which is a single unit or a frame of encoded
           samples.
         - Each block is completely independent of the other blocks in the
           stream, and each block contains the information necessary to decode
           a fixed number of PCM samples.

         @values 20 to 4096 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_adpcm_fmt_blk_t */

/* Encoder configuration block is the same as the ADPCM decoder
    format block. */
typedef asm_adpcm_fmt_blk_t asm_adpcm_enc_cfg_t;

/** @} */ /* end_addtogroup asmstrm_mediafmt_adpcm */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_mp2
    Media format ID for MP2.

    MP2 decoder-specific details:
    - Sampling rates -- 16 to 48 kHz
    - Channels -- 2
    - Bitwidth -- 16/24 bits per sample
    - Bitrates -- 8 to 384 kbps
    - Minimum buffer required per frame -- 1728 bytes
    - External bitstream support -- No

    @par MP2 format block 
    No format block is needed; all information is contained in the bitstream.
*/
#define ASM_MEDIA_FMT_MP2                                       0x00010DE9

/** @ingroup asmstrm_mediafmt_mp3
    Media format ID for MP3. 

    @par MP3 format block 
    No format block is needed; all information is contained in the bitstream.

    @par MP3 encode configuration block (asm_mp3_enc_cfg_t)
    @table{weak__asm__mp3__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_MP3                                       0x00010BE9

/* MP3 encoder configuration structure. */
typedef struct asm_mp3_enc_cfg_t asm_mp3_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_mp3_enc_cfg_t
@{ */
/* Payload of the MP3 encoder configuration parameters in the
    ASM_MEDIA_FMT_MP3 media format.
*/
struct asm_mp3_enc_cfg_t
{
    uint32_t          bit_rate;
    /**< Encoding rate in bits per second.

         @values 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192,
                 224, 256, 320 kbps

         For the best performance, Qualcomm recommends using 64 kbps per
         channel: 64 for mono encoding, and 128 for stereo encoding. */

    uint32_t          channel_cfg;
    /**< Number of channels to encode.

         @note1hang The Mp3 encoder supports mono and stereo.

         @values
         - 0 -- Native mode
         - 1 -- Mono
         - 2 -- Stereo
         - Other values are not supported

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         The number of channels must not change during encoding. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values
         - 16000, 22050, 24000, 32000, 44100, 48000 Hz
         - 0 -- Native mode

         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_aac_enc_cfg_v2_t */

/** @addtogroup asmstrm_mediafmt_aac
@{ */
/** Media format ID for the AAC format block, which supports both MPEG2 and
    MPEG4 AAC formats.

    @note1hang AOT=17 is the ER AAC-LC format, which currently is not
               supported.

    @par AAC format block (asm_aac_fmt_blk_v2_t)
    @tablens{weak__asm__aac__fmt__blk__v2__t}

    @par AAC encode configuration block (asm_aac_enc_cfg_v2_t)
    @table{weak__asm__aac__enc__cfg__v2__t}
    @vertspace{-18}
    @inputtable{Encoder_AAC_allowed_bitrates.tex}
    @inputtable{Encoder_AAC_supported_sampling_rates.tex}
*/
#define ASM_MEDIA_FMT_AAC_V2                                       0x00010DA6

/** Media format ID for the Dolby AAC decoder. This format ID is be used if
    the client wants to use the Dolby AAC decoder to decode MPEG2 and MPEG4
    AAC contents.
*/
#define ASM_MEDIA_FMT_DOLBY_AAC                                 0x00010D86

/** Enumeration for the audio data transport stream AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADTS                      0

/** Enumeration for low overhead audio stream AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LOAS                      1

/** Enumeration for the audio data interchange format AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADIF                      2

/** Enumeration for the raw AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_RAW                       3

/** Enumeration for the AAC low complexity audio object type:

    - When used for aac_fmt_flag in asm_aac_fmt_blk_v2_t, this parameter
      specifies that the AOT in the AAC stream is LC.
    - When used for enc_mode in asm_aac_enc_cfg_v2_t, this parameter specifies
      that the Encoding mode to be used is LC.
*/
#define ASM_MEDIA_FMT_AAC_AOT_LC                                2

/** Enumeration for the AAC spectral band replication AOT:

    - When used for aac_fmt_flag in asm_aac_fmt_blk_v2_t, this parameter
      specifies that the AOT in the AAC stream is SBR.
    - When used for enc_mode in asm_aac_enc_cfg_v2_t, this parameter specifies
      that the Encoding mode to be used is AAC+.
*/
#define ASM_MEDIA_FMT_AAC_AOT_SBR                               5

/** Enumeration for the AAC parametric stereo AOT:

    - When used for aac_fmt_flag in asm_aac_fmt_blk_v2_t, this parameter
      specifies that the AOT in the AAC stream is PS.
    - When used for enc_mode in asm_aac_enc_cfg_v2_t, this parameter specifies
      that the Encoding mode to be used is eAAC+.
*/
#define ASM_MEDIA_FMT_AAC_AOT_PS                                29

/** Enumeration for the bit-sliced arithmetic coding AOT:

    - When used for aac_fmt_flag in asm_aac_fmt_blk_v2_t, this parameter
      specifies that the AOT in the AAC stream is BSAC.
    - This parameter must not be used for enc_mode in
      asm_aac_enc_cfg_v2_t. BSAC encoding is not supported.
*/
#define ASM_MEDIA_FMT_AAC_AOT_BSAC                              22

/** @} */ /* end_addtogroup asmstrm_mediafmt_aac */

/* AAC decoder format block structure. */
typedef struct asm_aac_fmt_blk_v2_t asm_aac_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_aac_fmt_blk_v2_t
@{ */
/* Payload of the AAC decoder format block parameters in the
    ASM_MEDIA_FMT_AAC_V2 media format. */
/** If the total size of PCE > 0 bits, the PCE information follows the
    structure. In this case, the client must pad the additional bits for 32-bit
    alignment.
*/
struct asm_aac_fmt_blk_v2_t
{
    uint16_t          aac_fmt_flag;
    /**< Bitstream format option.

         @values
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADTS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LOAS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADIF
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_RAW @tablebulletend */

    uint16_t          audio_obj_type;
    /**< Audio Object Type (AOT) present in the AAC stream.

         @values
         - #ASM_MEDIA_FMT_AAC_AOT_LC
         - #ASM_MEDIA_FMT_AAC_AOT_SBR
         - #ASM_MEDIA_FMT_AAC_AOT_BSAC
         - #ASM_MEDIA_FMT_AAC_AOT_PS

         Other values are not supported. */

    uint16_t          channel_config;
    /**< Number of channels present in the AAC stream.

         @values
         - 0 -- PCE
         - 1 -- Mono
         - 2 -- Stereo
         - 6 -- 5.1 content @tablebulletend */

    uint16_t          total_size_of_PCE_bits;
    /**< For RAW formats and if channel_config=0 (PCE), the client can send
         the bitstream containing PCE immediately following this structure
         (in band).

         @values @ge 0 (does not include the bits required for 32-bit alignment)

         If this field is set to zero, the PCE information is assumed to be
         available in the audio bit stream and not in band.

         If this field is greater than zero, the PCE information follows this
         structure; additional bits may be required for 32-bit alignment. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 8000, 11025, 12000, 16000, 22050, 24000, 32000,
                 44100, 48000, 64000, 88200, 96000 Hz

         This field must be equal to the sample rate of the AAC-LC decoder's
         output.
         - For MP4 or 3GP containers, this is indicated by the
           samplingFrequencyIndex field in the AudioSpecificConfig element.
         - For ADTS format, this is indicated by the samplingFrequencyIndex
           in the ADTS fixed header.
         - For ADIF format, this is indicated by the samplingFrequencyIndex
           in the program_config_element present in the ADIF header.
           @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_aac_fmt_blk_v2_t */

/* AAC encoder configuration structure. */
typedef struct asm_aac_enc_cfg_v2_t asm_aac_enc_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_aac_enc_cfg_v2_t
@{ */
/* Payload of the AAC encoder configuration parameters in the
    ASM_MEDIA_FMT_AAC_V2 media format.
*/
struct asm_aac_enc_cfg_v2_t
{
    uint32_t          bit_rate;
    /**< Encoding rate in bits per second. */

    uint32_t          enc_mode;
    /**< Encoding mode.

         @values
         - #ASM_MEDIA_FMT_AAC_AOT_LC
         - #ASM_MEDIA_FMT_AAC_AOT_SBR
         - #ASM_MEDIA_FMT_AAC_AOT_PS @tablebulletend */

    uint16_t          aac_fmt_flag;
    /**< AAC format flag.

         @values
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADTS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_RAW @tablebulletend */

    uint16_t          channel_cfg;
    /**< Number of channels to encode.

         @note1hang The eAAC+ encoder mode supports only stereo.

         @values
         - 0 -- Native mode
         - 1 -- Mono
         - 2 -- Stereo
         - Other values are not supported

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         The number of channels must not change during encoding. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values
         - 0 -- Native mode
         - For other values, see Table
           @xref{tbl:AACencoderSupportedSamplingRates}

         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_aac_enc_cfg_v2_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup asmstrm_mediafmt_amrnb
@{ */
/** Media format ID for adaptive multirate narrowband.

    @par AMR-NB format block
    No format block is needed; all information is contained in the bitstream.

    @par AMR-NB encode configuration block (asm_amrnb_enc_cfg_t)
    @table{weak__asm__amrnb__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_AMRNB_FS                  0x00010BEB

/** Enumeration for 4.75 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR475                0

/** Enumeration for 5.15 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR515                1

/** Enumeration for 5.90 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR59                2

/** Enumeration for 6.70 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR67                3

/** Enumeration for 7.40 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR74                4

/** Enumeration for 7.95 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR795               5

/** Enumeration for 10.20 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR102               6

/** Enumeration for 12.20 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR122               7

/** Enumeration for disabling AMR-NB DTX mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_OFF                     0

/** Enumeration for enabling AMR-NB DTX mode VAD1. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_VAD1                    1

/** Enumeration for enabling AMR-NB DTX mode VAD2. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_VAD2                    2

/** Enumeration for enabling AMR-NB automatic DTX mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_AUTO                    3

/** @} */ /* end_addtogroup asmstrm_mediafmt_amrnb */

/* AMR-NB encoder configuration structure. */
typedef struct asm_amrnb_enc_cfg_t asm_amrnb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_amrnb_enc_cfg_t
@{ */
/* Payload of the AMR-NB encoder configuration parameters in the
    ASM_MEDIA_FMT_AMRNB_FS media format.
*/
struct asm_amrnb_enc_cfg_t
{
    uint16_t          enc_mode;
    /**< AMR-NB encoding rate.

         @values
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR475
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR515
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR59
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR67
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR74
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR795
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR102
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR122 @vertspace{-28} */

    uint16_t          dtx_mode;
    /**< Specifies whether DTX mode is disabled (0) or enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_amrnb_enc_cfg_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup asmstrm_mediafmt_amrwb
@{ */
/** Media format ID for adaptive multirate wideband.

    @par AMR-WB format block
    No format block is needed; all information is contained in the bitstream.

    @par AMR-WB encode configuration (asm_amrwb_enc_cfg_t)
    @table{weak__asm__amrwb__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_AMRWB_FS                  0x00010BEC

/** Enumeration for 6.6 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR66                 0

/** Enumeration for 8.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR885                1

/** Enumeration for 12.65 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1265               2

/** Enumeration for 14.25 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1425               3

/** Enumeration for 15.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1585               4

/** Enumeration for 18.25 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1825               5

/** Enumeration for 19.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1985               6

/** Enumeration for 23.05 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2305               7

/** Enumeration for 23.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2385               8

/** @} */ /* end_addtogroup asmstrm_mediafmt_amrwb */

/* AMR-WB encoder configuration structure. */
typedef struct asm_amrwb_enc_cfg_t asm_amrwb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_amrwb_enc_cfg_t
@{ */
/* Payload of the AMR-WB encoder configuration parameters in the
    ASM_STREAM_CMD_OPEN_READ_V2 command.
*/
struct asm_amrwb_enc_cfg_t
{
    uint16_t          enc_mode;
    /**< AMR-WB encoding rate.

         @values
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR66
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR885
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1265
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1425
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1585
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1825
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1985
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2305
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2385 @vertspace{-28} */

    uint16_t          dtx_mode;
    /**< Specifies whether DTX mode is disabled (0) or enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_amrwb_enc_cfg_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup asmstrm_mediafmt_v13k
@{ */
/** Media format ID for V13K Encoding mode.

    @par V13K format block
    No format block is needed; all information is contained in the bitstream.

    @par V13K encode configuration (asm_v13k_enc_cfg_t)
    @table{weak__asm__v13k__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_V13K_FS                      0x00010BED

/** Enumeration for 14.4 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1440                0

/** Enumeration for 12.2 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1220                1

/** Enumeration for 11.2 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1120                2

/** Enumeration for 9.0 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR90                  3

/** Enumeration for 7.2 kbps V13K eEncoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR720                 4

/** Enumeration for 1/8 vocoder rate.*/
#define ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE          1

/** Enumeration for 1/4 vocoder rate. */
#define ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE       2

/** Enumeration for 1/2 vocoder rate. */
#define ASM_MEDIA_FMT_VOC_HALF_RATE             3

/** Enumeration for full vocoder rate. */
#define ASM_MEDIA_FMT_VOC_FULL_RATE             4

/** @} */ /* end_addtogroup asmstrm_mediafmt_v13k */

/* V13K encoder configuration structure. */
typedef struct asm_v13k_enc_cfg_t asm_v13k_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_v13k_enc_cfg_t
@{ */
/* Payload of the V13K encoder configuration parameters in the
    ASM_MEDIA_FMT_V13K_FS media format.
*/
struct asm_v13k_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          reduced_rate_cmd;
    /**< Reduced rate command, used to change the average bitrate of the V13K
         vocoder.

         @values
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1440 (Default)
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1220
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1120
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR90
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR720 @tablebulletend */

    uint16_t          rate_mod_cmd;
    /**< Rate modulation command.

         @values 0 (Default)
         - If bit 0=1, rate control is enabled.
         - If bit 1=1, the maximum number of consecutive full rate frames is
           limited with numbers supplied in bits 2 to 10.
         - If bit 1=0, the minimum number of non-full rate frames in between two
           full rate frames is forced to the number supplied in bits 2 to 10.
         - In both cases of bit 1, if necessary, half rate is used to substitute
           full rate.
         - Bits 15 to 10 are reserved and must all be set to zero.
         @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_v13k_enc_cfg_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_evrc
    Media format ID for enhanced variable rate coder.

    @par EVRC format block
    No format block is needed; all information is contained in the bitstream.

    @par EVRC encode configuration (asm_evrc_enc_cfg_t)
    @table{weak__asm__evrc__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_EVRC_FS                   0x00010BEE

/* EVRC encoder configuration structure. */
typedef struct asm_evrc_enc_cfg_t asm_evrc_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_evrc_enc_cfg_t
@{ */
/* Payload of the EVRC encoder configuration parameters in the
    ASM_MEDIA_FMT_EVRC_FS media format.
*/
struct asm_evrc_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          rate_mod_cmd;
    /**< Rate modulation command.

         @values 0 (Default)
         - If bit 0=1, rate control is enabled.
         - If bit 1=1, the maximum number of consecutive full rate frames is
           limited with numbers supplied in bits 2 to 10.
         - If bit 1=0, the minimum number of non-full rate frames in between two
           full rate frames is forced to the number supplied in bits 2 to 10.
         - In both cases of bit 1, if necessary, half rate is used to substitute
           full rate.
         - Bits 15 to 10 are reserved and must all be set to zero. @tablebulletend */

    uint16_t          reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_evrc_enc_cfg_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup asmstrm_mediafmt_evcrb
@{ */
/** Media format ID for enhanced variable rate codec B.

    @par EVRC-B format block
    No format block is needed; all information is contained in the bitstream.

    @par EVRC-B encode configuration (asm_evrcb_enc_cfg_t)
    @table{weak__asm__evrcb__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_EVRCB_FS                  0x00010BEF

/** Enumeration for 9.3 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR930        0

/** Enumeration for 8.5 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR850        1

/** Enumeration for 7.5 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR750        2

/** Enumeration for 7.0 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR700        3

/** Enumeration for 6.6 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR660        4

/** Enumeration for 6.2 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR620        5

/** Enumeration for 5.8 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR580        6

/** Enumeration for 4.8 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR480        7

/** @} */ /* end_addtogroup asmstrm_mediafmt_evcrb */

/* EVCR-B encoder configuration structure. */
typedef struct asm_evrcb_enc_cfg_t asm_evrcb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_evrcb_enc_cfg_t
@{ */
/* Payload of the EVCR-B encoder configuration parameters in the
    ASM_MEDIA_FMT_EVRCB_FS media format.
*/
struct asm_evrcb_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          dtx_enable;
    /**< Specifies whether DTX mode is disabled (0) or enabled (1). */

    uint16_t          reduced_rate_level;
    /**< Reduced rate level for the average encoding rate.

         @values
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR930 (Default)
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR850
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR750
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR700
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR660
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR620
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR580
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR480 @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_evrcb_enc_cfg_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup asmstrm_mediafmt_evrcwb
@{ */
/** Media format ID for enhanced variable rate codec wideband.

    @par EVRC-WB format block
    No format block is needed; all information is contained in the bitstream.

    @par EVRC-WB encode configuration (asm_evrcwb_enc_cfg_t)
    @table{weak__asm__evrcwb__enc__cfg__t} @newpage
*/
#define ASM_MEDIA_FMT_EVRCWB_FS                 0x00010BF0

/** Enumeration for 8.5 kbps EVRC-WB Encoding mode.  */
#define ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR850        0

/** Enumeration for 9.3 kbps EVRC-WB Encoding mode. */
#define ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR930        4

/** Enumeration for 4.8 kbps EVRC-WB Encoding mode. */
#define ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR480        7

/** @} */ /* end_addtogroup asmstrm_mediafmt_evrcwb */

/* EVRC-WB encoder configuration structure. */
typedef struct asm_evrcwb_enc_cfg_t asm_evrcwb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_evrcwb_enc_cfg_t
@{ */
/* Payload of the EVRC-WB encoder configuration parameters in the
    #ASM_MEDIA_FMT_EVRCWB_FS media format.
*/
struct asm_evrcwb_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          dtx_enable;
    /**< Specifies whether DTX mode is disabled (0) or enabled (1). */

    uint16_t          reduced_rate_level;
    /**< Reduced rate level for the average encoding rate.

         @values
         - #ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR850 (Default)
         - #ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR930
         - #ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR480 @vertspace{-28} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_evrcwb_enc_cfg_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup asmstrm_mediafmt_midi
@{ */
/** Media format ID for MIDI.

    @par MIDI format block (asm_midi_fmt_blk_t)
    @table{weak__asm__midi__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_MIDI                      0x00010BF1

/** Enumeration for MIDI mode GM. */
#define ASM_MIDI_MODE_GM                        0

/** Enumeration for MIDI mode MA3. */
#define ASM_MIDI_MODE_MA3                       1

/** Enumeration for MIDI mode MA5. */
#define ASM_MIDI_MODE_MA5                       2

/** @} */ /* end_addtogroup asmstrm_mediafmt_midi */

/* MIDI format block structure. */
typedef struct asm_midi_fmt_blk_t asm_midi_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_midi_fmt_blk_t
@{ */
/* Payload of the MIDI format block parameters in the
    #ASM_MEDIA_FMT_MIDI media format.
*/
struct asm_midi_fmt_blk_t
{
    uint32_t          mode;
    /**< Operating mode that indicates which commercial MIDI device is to be
         emulated.

         @values
         - #ASM_MIDI_MODE_GM
         - #ASM_MIDI_MODE_MA3
         - #ASM_MIDI_MODE_MA5 @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_midi_fmt_blk_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup asmstrm_mediafmt_sbc
@{ */
/** Media format ID for SBC encode configuration.

    @par SBC encode configuration (asm_sbc_enc_cfg_t)
    @table{weak__asm__sbc__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_SBC                       0x00010BF2

/** Enumeration for SBC channel Mono mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_MONO                     1

/** Enumeration for SBC channel Stereo mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_STEREO                   2

/** Enumeration for SBC channel Dual Mono mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_DUAL_MONO                8

/** Enumeration for SBC channel Joint Stereo mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_JOINT_STEREO             9

/** Enumeration for SBC bit allocation method = loudness. */
#define ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_LOUDNESS            0

/** Enumeration for SBC bit allocation method = SNR.  */
#define ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_SNR                 1

/** @} */ /* end_addtogroup asmstrm_mediafmt_sbc */

/* SBC encoder configuration structure. */
typedef struct asm_sbc_enc_cfg_t asm_sbc_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_sbc_enc_cfg_t
@{ */
/* Payload of the SBC encoder configuration parameters in the
    #ASM_MEDIA_FMT_SBC media format.
*/
struct asm_sbc_enc_cfg_t
{
    uint32_t          num_subbands;
    /**< Number of sub-bands.

         @values 4, 8 */

    uint32_t          blk_len;
    /**< Size of the encoded block in samples.

         @values 4, 8, 12, 16 */

    uint32_t          channel_mode;
    /**< Mode used to allocate bits between channels.

         @values
         - 0 (Native mode)
         - #ASM_MEDIA_FMT_SBC_CHANNEL_MODE_MONO
         - #ASM_MEDIA_FMT_SBC_CHANNEL_MODE_STEREO
         - #ASM_MEDIA_FMT_SBC_CHANNEL_MODE_DUAL_MONO
         - #ASM_MEDIA_FMT_SBC_CHANNEL_MODE_JOINT_STEREO

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         If postprocessing outputs one-channel data, Mono mode is used. If
         postprocessing outputs two-channel data, Stereo mode is used.

         The number of channels must not change during encoding. */

    uint32_t          alloc_method;
    /**< Encoder bit allocation method.

         @values
         - #ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_LOUDNESS
         - #ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_SNR @tablebulletend */

    uint32_t          bit_rate;
    /**< Number of encoded bits per second.

         @values
         - Mono channel -- Maximum of 320 kbps
         - Stereo channel -- Maximum of 512 kbps @tablebulletend */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 0 (Native mode), 16000, 32000, 44100, 48000&nbsp;Hz

         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_sbc_enc_cfg_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_wmapro10
    Media format ID for the WMA v10 Professional format block.

    @par WMA v10 format block (asm_wmaprov10_fmt_blk_t)
    @table{weak__asm__wmaprov10__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_WMA_V10PRO_V2                0x00010DA7

/* WMA v10 Professional decoder format block structure. */
typedef struct asm_wmaprov10_fmt_blk_v2_t asm_wmaprov10_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_wmaprov10_fmt_blk_t
@{ */
/* Payload of the WMA Professional v10 decoder format block parameters in the
    ASM_MEDIA_FMT_WMA_V10PRO_V2 media format.
*/
struct asm_wmaprov10_fmt_blk_v2_t
{
    uint16_t          fmt_tag;
    /**< WMA format type.

         @values
         - 0x162 -- WMA 9 Pro
         - 0x163 -- WMA 9 Pro Lossless
         - 0x166 -- WMA 10 Pro
         - 0x167 -- WMA 10 Pro Lossless @tablebulletend */

    uint16_t          num_channels;
    /**< Number of channels encoded in the input stream.

         @values 1 to 8 */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 11025, 16000, 22050, 32000, 44100, 48000, 88200, 96000 Hz */

    uint32_t          avg_bytes_per_sec;
    /**< Bitrate expressed as the average bytes per second.

         @values 2000 to 96000 */

    uint16_t          blk_align;
    /**< Size of the bitstream packet size. WMA Pro files have a payload of one
         block per bitstream packet.

         @values @le 13376 bytes */

    uint16_t          bits_per_sample;
    /**< Number of bits per sample in the encoded WMA stream.

         @values 16, 24 */

    uint32_t          channel_mask;
    /**< Bit-packed double word (32-bits) that indicates the recommended
         speaker positions for each source channel.

         @values See the WMAPRO_CHANNEL_MASK_* macros for popular speaker
                 positions (starting with #WMAPRO_CHANNEL_MASK_RAW) */

    uint16_t          enc_options;
    /**< Bit-packed word with values that indicate whether certain features of
         the bitstream are used.

         @values
         - 0x0001 -- ENCOPT3_PURE_LOSSLESS
         - 0x0006 -- ENCOPT3_FRM_SIZE_MOD
         - 0x0038 -- ENCOPT3_SUBFRM_DIV
         - 0x0040 -- ENCOPT3_WRITE_FRAMESIZE_IN_HDR
         - 0x0080 -- ENCOPT3_GENERATE_DRC_PARAMS
         - 0x0100 -- ENCOPT3_RTMBITS @tablebulletend */

    uint16_t          usAdvancedEncodeOpt;
    /**< Advanced encoding option. This field is ignored. */

    uint32_t          advanced_enc_options2;
    /**< Advanced encoding option 2. This field is ignored. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_wmaprov10_fmt_blk_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_wma9
    Media format ID for the Windows Media Audio (WMA) v9 format block.

    @par WMA v9 format block (asm_wmastdv9_fmt_blk_v2_t)
    @table{weak__asm__wmastdv9__fmt__blk__v2__t}
*/
#define ASM_MEDIA_FMT_WMA_V9_V2                    0x00010DA8

/* WMA v9 decoder format block structure. */
typedef struct asm_wmastdv9_fmt_blk_v2_t asm_wmastdv9_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_wmastdv9_fmt_blk_v2_t
@{ */
/* Payload of the WMA v9 decoder format block parameters in the
    ASM_MEDIA_FMT_WMA_V9_V2 media format.
*/
struct asm_wmastdv9_fmt_blk_v2_t
{
    uint16_t          fmt_tag;
    /**< WMA format tag.

         @values 0x161 (WMA 9 standard) */

    uint16_t          num_channels;
    /**< Number of channels in the stream.

         @values 1, 2 */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 48000 Hz */

    uint32_t          avg_bytes_per_sec;
    /**< Bitrate expressed as the average bytes per second. */

    uint16_t          blk_align;
    /**< Block align. All WMA files with a maximum packet size of 13376
         are supported. */

    uint16_t          bits_per_sample;
    /**< Number of bits per sample in the output.

         @values 16 */

    uint32_t          channel_mask;
    /**< Channel mask.

         @values
         - 3 -- Stereo (front left/front right)
         - 4 -- Mono (center) @tablebulletend */

    uint16_t          enc_options;
    /**< Options used during encoding. */

    uint16_t          reserved;
    /**< Clients must set this field to zero. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_wmastdv9_fmt_blk_v2_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_wma8
    Media format ID for the Windows Media Audio (WMA) v8
    format block.

    @par WMA v8 format block (asm_wmastdv8_enc_cfg_t)
    @table{weak__asm__wmastdv8__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_WMA_V8                    0x00010D91

/* WMA v8 encoder configuration structure. */
typedef struct asm_wmastdv8_enc_cfg_t asm_wmastdv8_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_wmastdv8_enc_cfg_t
@{ */
/* Payload of the WMA v8 encoder configuration parameters in the
    ASM_MEDIA_FMT_WMA_V8 media format.
*/
struct asm_wmastdv8_enc_cfg_t
{
    uint32_t          bit_rate;
    /**< Encoding rate in bits per second. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 0 (Native mode), 22050, 32000, 44100, 48000&nbsp;Hz

         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding. */

    uint16_t          channel_cfg;
    /**< Number of channels to encode.

         @values
         - 0 -- Native mode
         - 1 -- Mono
         - 2 -- Stereo
         - Other values are not supported

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         The number of channels must not change during encoding. */

    uint16_t          reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_wmastdv8_enc_cfg_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_amrwbplus
    Media format ID for the AMR-WB Plus format block.

    @par AMR-WB Plus format block (asm_amrwbplus_fmt_blk_v2_t)
    @table{weak__asm__amrwbplus__fmt__blk__v2__t}
*/
#define ASM_MEDIA_FMT_AMR_WB_PLUS_V2               0x00010DA9

/* AMR-WB Plus decoder format block structure. */
typedef struct asm_amrwbplus_fmt_blk_v2_t asm_amrwbplus_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_amrwbplus_fmt_blk_v2_t
@{ */
/* Payload of the AMR-WB Plus decoder format block parameters in the
    ASM_MEDIA_FMT_AMR_WB_PLUS_V2 media format.
*/
struct asm_amrwbplus_fmt_blk_v2_t
{
    uint32_t          amr_frame_fmt;
    /**< AMR frame format.

         @values
         - 6 -- Transport Interface Format (TIF); contains a 2-byte header for
           each frame within the superframe
         - Any other value -- File storage format; contains one header per
           superframe @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_amrwbplus_fmt_blk_v2_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_ac3
    @deprecated Use ASM_MEDIA_FMT_AC3.

    Media format ID for an AC3 decoder. No format block is required.
*/
#define ASM_MEDIA_FMT_AC3_DEC                   0x00010BF6

/** @ingroup asmstrm_mediafmt_eac3
    @deprecated Use ASM_MEDIA_FMT_EAC3.

    Media format ID for an E-AC3 decoder.
*/
#define ASM_MEDIA_FMT_EAC3_DEC                   0x00010C3C

/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_g711_alaw
    Media format ID for the G711 A-law algorithm.

    @par G711 A-law format block (asm_g711_alaw_fmt_blk_t)
    @table{weak__asm__g711__alaw__fmt__blk__t}

    @par G711 ALAW encode configuration (asm_g711_alaw_enc_cfg_t)
    @table{weak__asm__g711__alaw__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_G711_ALAW_FS                   0x00010BF7

/* G711 A-law encoder configuration structure. */
typedef struct asm_g711_alaw_enc_cfg_t asm_g711_alaw_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_g711_alaw_enc_cfg_t
@{ */
/* Payload of the G711 A-law encoder configuration parameters in the
    ASM_MEDIA_FMT_G711_ALAW_FS media format.
*/
struct asm_g711_alaw_enc_cfg_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 8000, 16000 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_g711_alaw_enc_cfg_t */

/* G711 A-law decoder format block structure. */
typedef struct asm_g711_alaw_fmt_blk_t asm_g711_alaw_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_g711_alaw_fmt_blk_t
@{ */
/* Payload of the G711 A-law decoder format block parameters in the
    ASM_STREAM_CMD_OPEN_WRITE_V2 command.
 */
struct asm_g711_alaw_fmt_blk_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 8000, 16000 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_g711_alaw_fmt_blk_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_g711_mlaw
    Media format ID for the G711 m-law algorithm.

    @par G711 M-law format block (asm_g711_mlaw_fmt_blk_t)
    @table{weak__asm__g711__mlaw__fmt__blk__t}

    @par G711 MLAW encode configuration (asm_g711_mlaw_enc_cfg_t)
    @table{weak__asm__g711__mlaw__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_G711_MLAW_FS                   0x00010C2E

/* G711 m-law encoder configuration structure. */
typedef struct asm_g711_mlaw_enc_cfg_t asm_g711_mlaw_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_g711_mlaw_enc_cfg_t
@{ */
/* Payload of the G711 m-law encoder configuration parameters in the
    ASM_MEDIA_FMT_G711_MLAW_FS media format.
*/
struct asm_g711_mlaw_enc_cfg_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 8000, 16000 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_g711_mlaw_enc_cfg_t */

/* G711 m-law decoder format block structure. */
typedef struct asm_g711_mlaw_fmt_blk_t asm_g711_mlaw_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_g711_mlaw_fmt_blk_t
@{ */
/* Payload of the G711 m-law decoder format block parameters in the
    ASM_MEDIA_FMT_G711_MLAW_FS media format.
*/
struct asm_g711_mlaw_fmt_blk_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 8000, 16000 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_g711_mlaw_fmt_blk_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_g729a
    Media format ID for G729A.

    @par G729A format block
    No format block is required.

    @par G729A encode configuration block
    No encoder configuration block is required. This is currently unused.
*/
#define ASM_MEDIA_FMT_G729A_FS                 0x00010C2D

/** @ingroup asmstrm_mediafmt_dtmf
    Media format ID for DTMF.
*/
#define ASM_MEDIA_FMT_DTMF                     0x00010C2F

/** @ingroup asmstrm_mediafmt_gsmfr
    Media format ID for GSM-FR.
*/
#define ASM_MEDIA_FMT_FR_FS                     0x00010D6B
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_vorbis
    Media format ID for Vorbis.

    @par Vorbis format block (asm_vorbis_fmt_blk_t)
    @table{weak__asm__vorbis__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_VORBIS                    0x00010C15

/* Vorbis decoder format block structure. */
typedef struct asm_vorbis_fmt_blk_t asm_vorbis_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_vorbis_fmt_blk_t
@{ */
/* Payload of the Vorbis decoder format block parameters in the
    ASM_MEDIA_FMT_VORBIS media format.
*/
struct asm_vorbis_fmt_blk_t
{
    uint32_t          bit_stream_fmt;
    /**< Transcoded bitstream containing the size of the frame as the first
         word in each frame.

         @values
         - 0 -- Raw bitstream
         - 1 -- Transcoded bitstream

         Currently, the Vorbis decoder library supports only a transcoded
         bitstream. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_vorbis_fmt_blk_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_flac
    Media format ID for FLAC.

    @par FLAC format block (asm_flac_fmt_blk_t)
    @table{weak__asm__flac__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_FLAC                      0x00010C16

/* FLAC decoder format block structure. */
typedef struct asm_flac_fmt_blk_t asm_flac_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_flac_fmt_blk_t
@{ */
/* Payload of the FLAC decoder format block parameters in the
    ASM_MEDIA_FMT_FLAC media format.
*/
struct asm_flac_fmt_blk_t
{
    uint16_t          is_stream_info_present;
    /**< Specifies whether stream information is present in the FLAC format
         block.

         @values
         - 0 -- Stream information is not present in this message
         - 1 -- Stream information is present in this message

         When set to 1, the FLAC bitstream was successfully parsed by the
         client, and other fields in the FLAC format block can be read by the
         decoder to get metadata stream information. */

    uint16_t          num_channels;
    /**< Number of channels for decoding.

         @values 1 to 2 */

    uint16_t          min_blk_size;
    /**< Minimum block size (in samples) used in the stream. It must be less
         than or equal to max_blk_size. */

    uint16_t          max_blk_size;
    /**< Maximum block size (in samples) used in the stream. If the minimum
         block size equals the maximum block size, a fixed block size stream
         is implied. */

    uint16_t          md5_sum[8];
    /**< MD5 signature array of the unencoded audio data. This allows the
         decoder to determine if an error exists in the audio data, even when
         the error does not result in an invalid bitstream. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 8000 to 48000 Hz */

    uint32_t          min_frame_size;
    /**< Minimum frame size used in the stream.

         @values
         - > 0 bytes
         - 0 -- The value is unknown @tablebulletend */

    uint32_t          max_frame_size;
    /**< Maximum frame size used in the stream.

         @values
         - > 0 bytes
         - 0 -- The value is unknown @tablebulletend */

    uint16_t          sample_size;
    /**< Bits per sample.

         @values 8, 16 */

    uint16_t          reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_flac_fmt_blk_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_dts
    Media format ID for Digital Theater Systems.

    @par DTS format block
    No format block is needed; all information is contained in the bitstream.

    @par DTS encode configuration block (asm_dts_enc_cfg_t)
    @table{weak__asm__dts__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_DTS                    0x00010D88

/* DTS encoder configuration structure. */
typedef struct asm_dts_enc_cfg_t asm_dts_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_dts_enc_cfg_t
@{ */
/* Payload of the DTS encoder configuration parameters in the
    ASM_MEDIA_FMT_DTS media format.
*/
struct asm_dts_enc_cfg_t
{
    uint32_t          sample_rate;
    /**< Sampling rate at which input is to be encoded.

         @values
         - 44100 -- Encode at 44.1 kHz
         - 48000 -- Encode at 48 kHz @tablebulletend */

    uint32_t          num_channels;
    /**< Number of channels for multichannel encoding.

         @values 1 to 6 */

    uint8_t   channel_mapping[16];
    /**< Channel array of size 16. Channel[i] mapping describes channel i. Each
         element i of the array describes channel i inside the buffer where 0
         @le i < num_channels. An unused channel is set to zero. Only the first
         num_channels elements are valid.

         @values
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_dts_enc_cfg_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup asmstrm_mediafmt_ac3
    Media format ID for the AC3 encoder and decoder (DD encoder/decoder).

    @par DDP decode format block
    No format block is needed; all information is contained in the bitstream.

    @par DDP encode configuration block (asm_ddp_enc_cfg_t)
    @table{weak__asm__ddp__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_AC3                 0x00010DEE

/** @ingroup asmstrm_mediafmt_eac3
    Media format ID for the EAC3 encoder and decoder (DDP encoder/decoder).

    @par DDP decode format block
    No format block is needed; all information is contained in the bitstream.

    @par DDP encode configuration block (asm_ddp_enc_cfg_t)
    @table{weak__asm__ddp__enc__cfg__t}
 */
#define ASM_MEDIA_FMT_EAC3                0x00010DEF

/* DDP encoder configuration structure. */
typedef struct asm_ddp_enc_cfg_t asm_ddp_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_ddp_enc_cfg_t
@{ */
/* Payload of the dolby encoder configuration parameters in the
    ASM_MEDIA_FMT_AC3 or ASM_MEDIA_FMT_EAC3 media format.
*/
struct asm_ddp_enc_cfg_t
{
    uint32_t          sample_rate;
    /**< Samples at which input is to be encoded.

         @values 48000 -- Encode at 48 kHz */

    uint32_t        num_channels ;
    /**< Number of channels for multichannel encoding.

         @values 1 to 6 */

    uint32_t        bits_per_sample ;
    /**< Number of bits per sample.

         @values 16 or 24 */

    uint8_t   channel_mapping[16];
    /**< Channel array of size 16. Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero. Only first
         num_channels elements are valid

         @values
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_ddp_enc_cfg_t */

/** @ingroup asmstrm_mediafmt_atrac
    Media format ID for adaptive transform acoustic coding.
*/
#define ASM_MEDIA_FMT_ATRAC                  0x00010D89

/** @ingroup asmstrm_mediafmt_mat
    Media format ID for metadata-enhanced audio transmission.
*/
#define ASM_MEDIA_FMT_MAT                    0x00010D8A

/** @ingroup asmstrm_mediafmt_dts
    Media format ID for the Digital Theater Systems Low Bit Rate (LBR) format.
*/
#define ASM_MEDIA_FMT_DTS_LBR                0x00010DBB

/** @endcond */

/** @ingroup asmstrm_mediafmt_multich_pcmv3
    Media format ID for multiple channel linear PCM with support for indicating
    whether samples are packed in 32-bit words or in 24-bit words when
    bits_per_sample = 24.

    @par Multiple Channel PCM format block (asm_multi_channel_pcm_fmt_blk_v3_t}
    @tablens{weak__asm__multi__channel__pcm__fmt__blk__v3__t}

    @par Multiple Channel PCM encode configuration block (asm_multi_channel_pcm_enc_cfg_v3_t)
    @table{weak__asm__multi__channel__pcm__enc__cfg__v3__t}
*/
#define ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3                0x00010DDC

/* Linear multiple channel PCM decoder format block structure. */
typedef struct asm_multi_channel_pcm_fmt_blk_v3_t asm_multi_channel_pcm_fmt_blk_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_fmt_blk_v3_t
@{ */
/* Payload of the Multiple Channel PCM decoder format block parameters
 */
/** The data must be in little-endian format, and it must always be
    interleaved.
*/
struct asm_multi_channel_pcm_fmt_blk_v3_t
{
    uint16_t  num_channels;
    /**< Number of channels.

         @values 1 to 8 */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 24 */

    uint32_t  sample_rate;
    /**< Number of samples per second.

         @values 2000 to 48000, 96000, 192000 Hz */

    uint16_t  is_signed;
    /**< Flag that indicates the PCM samples are signed (1). */

    uint16_t    sample_word_size;
    /**< Size in bits of the word that holds a sample of a channel.

          @values 16, 24, 32
          - 16-bit samples are always placed in 16-bit words.
               - sample_word_size = 16
          - 24-bit samples can be placed in 32-bit words or in consecutive
            24-bit words.
               - sample_word_size = 32: 24-bit samples are placed in the
                 most significant 24 bits of a 32 bit word.
               - sample_word_size = 24: 24-bit samples are placed in 
                 24-bit words  @tablebulletend */

    uint8_t   channel_mapping[8];
    /**< Channel array of size 8. Channel[i] mapping describes channel i. Each
         element i of the array describes channel i inside the buffer where 0
         @le i < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_fmt_blk_v3_t */

/* Multichannel PCM encoder configuration structure. */
typedef struct asm_multi_channel_pcm_enc_cfg_v3_t asm_multi_channel_pcm_enc_cfg_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_enc_cfg_v3_t
@{ */
/* Payload of the multichannel PCM encoder configuration parameters in
    the ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 media format.
*/
struct asm_multi_channel_pcm_enc_cfg_v3_t
{
   uint16_t  num_channels;
    /**< Number of PCM channels.

         @values
         - 0 -- Native mode
         - 1 -- 8

         Native mode indicates that encoding must be performed with the number
         of channels at the input. */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 24 */

    uint32_t  sample_rate;
    /**< Number of samples per second.

         @values 0, 8000 to 48000 Hz

         A value of 0 indicates the native sampling rate. Encoding is
         performed at the input sampling rate. */

    uint16_t  is_signed;
    /**< Flag that indicates the PCM samples are signed (1). Currently, only
         signed PCM samples are supported. */

    uint16_t    sample_word_size;
    /**< The size in bits of the word that holds a sample of a channel.

         @values 16, 24, 32
         - 16-bit samples are always placed in 16-bit words.
             - sample_word_size = 1
         - 24-bit samples can be placed in 32-bit words or in consecutive
           24-bit words.
              - sample_word_size = 32: 24-bit samples are placed in the
                most significant 24 bits of a 32-bit word.
              - sample_word_size = 24: 24-bit samples are placed in
                24-bit words. @tablebulletend */

    uint8_t   channel_mapping[8];
    /**< Channel mapping array expected at the encoder output.
         Channel[i] mapping describes channel i inside the buffer, where
         0 @le i < num_channels. All valid used channels must be present at
         the beginning of the array.

         If native mode is set for the channels, this field is ignored.

         @values See Section @xref{hdr:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_enc_cfg_v3_t */

/* Custom encoder configuration structure. */
typedef struct asm_custom_enc_cfg_t asm_custom_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @ingroup asmstrm_mediafmt_custom_id
    Payload of the custom encoder configuration parameters in the default media
    format.

    Following this structure is the payload that is specific to the encoder
    media format. The size of this encoder-specific payload is provided by 
    custom_payload_size. The client must ensure 32-bit alignment.
*/
struct asm_custom_enc_cfg_t
{
    uint32_t  sample_rate;
       /**< Number of samples per second.

            @values 0, 8000 to 192000 Hz

            A value of 0 indicates the native sampling rate. Encoding is
            performed at the input sampling rate. */

    uint16_t  num_channels;
    /**< Number of PCM channels.

         @values
         - 0 -- Native mode
         - 1 -- 8

         Native mode indicates that encoding must be performed with the number
         of channels at the input. */

    uint16_t  reserved;
    /**< This field must be set to zero. */  /* reserved for 32-bit alignment */

    uint8_t   channel_mapping[8];
    /**< Channel mapping array expected at the encoder output.
         Channel[i] mapping describes channel i inside the buffer, where
         0 @le i < num_channels. All valid used channels must be present at
         the beginning of the array.

         If native mode is set for the channels, this field is ignored.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint32_t custom_payload_size;
    /**< Size of the custom payload that immediately follows this structure.

         @note1hang This size does not include bytes added for 32-bit
                    alignment. */

}
#include "adsp_end_pack.h"
;


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_ADSP_MEDIA_FMT_H_*/
