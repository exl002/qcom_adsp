/*========================================================================*/
/**
@file adsp_audproc_api.h

This file contains the structure definitions for Audio Post Processing
Topology IDs, Module IDs, Parameter IDs.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is maintained in the
      audio_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*===========================================================================
Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/audio/inc/adsp_audproc_api.h#25 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/06/13   sw      (Tech Pubs) Merged Doxygen comments/markup from 2.0; edited
                   new Doxygen comments for 2.2.
08/03/12   sw      (Tech Pubs) Edited Doxygen comments for 2.0 updates;
                   updated Doxygen markup to Dox 1.7.6.1.
02/08/12   sw      (Tech Pubs) Updated Doxygen comments based on 06.110 branch.
05/30/11   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
04/22/11    ss     Introducing Audproc API file that would hold all post processing
                   Module IDs, Parameter IDs and Topology IDs

========================================================================== */

#ifndef _ADSP_AUDPROC_API_H_
#define _ADSP_AUDPROC_API_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
 * Include file to add New PP topology and PP module. This file is present at adsp_proc\hap\integration\audproc
 * To include this file, set OEM_ROOT from command prompt
 */
#if defined(OEM_ROOT)
#include "custom_adsp_audproc_api.h"
#endif
/** @addtogroup audiopp_topos_asm
@{ */
/** ID of the default audio postprocessing stream topology.

  @if OEM_only
    @inputfig{1,ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT_topology.png}

    - Downmixer                         (#AUDPROC_MODULE_ID_DOWNMIX)
    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
  @endif

  @if ISV_only
    @inputfig{1,ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT_topology_subset.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
  @endif
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT                         0x00010BE4

/** @cond OEM_only */
/** ID of the default postprocessing topology with peak meter.

    @inputfig{1,ASM_STREAM_POSTPROC_TOPO_ID_PEAKMETER_topology.png}

    - Downmixer                         (#AUDPROC_MODULE_ID_DOWNMIX)
    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Peak Meter                        (#AUDPROC_MODULE_ID_PEAK_METER) @newpage
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_PEAKMETER                       0x00010D83
/** @endcond */

/** ID of the no-default audio postprocessing topology. The resampler
    is the only module present in this topology.

  @if OEM_only
    @inputfig{1,ASM_STREAM_POSTPROC_TOPO_ID_NONE_topology.png}

    - Downmixer                         (#AUDPROC_MODULE_ID_DOWNMIX)
    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
  @endif

  @if ISV_only
    @inputfig{1,ASM_STREAM_POSTPROC_TOPO_ID_NONE_topology_subset.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
  @endif
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_NONE                            0x00010C68

/** @cond OEM_only */
/** ID of the audo postprocessing topology with multichannel resampler, peak
    meter, and volume control.

    @inputfig{1,ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Peak Meter                        (#AUDPROC_MODULE_ID_PEAK_METER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
*/
#define ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL                     0x00010D8B
/** @endcond */

/** ID of the default audio preprocessing stream topology.

  @if OEM_only
    @inputfig{1,ASM_STREAM_PREPROC_TOPO_ID_DEFAULT_topology.png}

    - Downmixer                         (#AUDPROC_MODULE_ID_DOWNMIX)
    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL) @newpage
  @endif

  @if ISV_only
    @inputfig{1,ASM_STREAM_PREPROC_TOPO_ID_DEFAULT_topology_subset.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
  @endif
*/
#define ASM_STREAM_PREPROC_TOPO_ID_DEFAULT   ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT

/** @cond OEM_only */
/** ID of the no-default audio preprocessing topology. The resampler is
    the only module present in this topology.

    @inputfig{1,ASM_STREAM_PREPROC_TOPO_ID_NONE_topology.png}

    - Downmixer                         (#AUDPROC_MODULE_ID_DOWNMIX)
    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
*/
#define ASM_STREAM_PREPROC_TOPO_ID_NONE      ASM_STREAM_POSTPROC_TOPO_ID_NONE

/** ID of the DTS audio postprocessing topology.

    @inputfig{1,AUDPROC_POPP_TOPOLOGY_ID_DTS_AUDIO_topology.png}

    - DTS Dynamic Range Control         (#AUDPROC_MODULE_ID_DTS_DRC)
    - DTS DialNorm                      (#AUDPROC_MODULE_ID_DTS_DIALNORM)
*/
#define AUDPROC_POPP_TOPOLOGY_ID_DTS_AUDIO   0x0001071F

/** @endcond */
/** @} */ /* end_addtogroup audiopp_topos_asm */

/** @addtogroup audiopp_topos_adm
@{ */
/** ID of the no-default COPP audio postprocessing device topology
    (indicates None).

    @inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_NONE_AUDIO_COPP_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_NONE_AUDIO_COPP              0x00010312

/** @cond OEM_only */
/** ID of SPEAKER_MONO in the COPP audio postprocessing device topology.

@inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (#AUDPROC_MODULE_ID_MBDRC)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP        0x00010313

/** ID of SPEAKER_STEREO in the COPP audio postprocessing device topology.

@inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (#AUDPROC_MODULE_ID_MBDRC)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP      0x00010314

/** ID of SPEAKER_STEREO_IIR ID in the COPP audio postprocessing device
    topology.

@inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning Filter Left       (#AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER)
    - IIR Tuning Filter Right      (#AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER)
    - Multiband Dynamic Range Control   (#AUDPROC_MODULE_ID_MBDRC)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
     @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP  0x00010704

/** ID of the COPP topology of SPEAKER_MONO for MBDRC Version 2.

@inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP_MBDRCV2_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Dynamic Range Control             (#AUDPROC_MODULE_ID_MBDRCV2)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MONO_AUDIO_COPP_MBDRCV2 0x0001070D

/** IID of the COPP topology of SPEAKER_STEREO for MBDRC Version 2.

@inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP_MBDRCV2_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multi-band Dynamic Range Control  (#AUDPROC_MODULE_ID_MBDRCV2)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_AUDIO_COPP_MBDRCV2 0x0001070E

/** ID of the COPP topology of STEREO_IIR for MBDRC Version 2.

@inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP_MBDRCV2_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning filter left         (#AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER)
    - IIR Tuning filter right        #AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER)
    - Multi-band Dynamic Range Control  (#AUDPROC_MODULE_ID_MBDRCV2)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_STEREO_IIR_AUDIO_COPP_MBDRCV2 0x0001070F

/** ID of the COPP topology with multichannel peak meter and volume control.

    @inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MCH_PEAK_VOL_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Peak Meter                        (#AUDPROC_MODULE_ID_PEAK_METER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL) @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SPEAKER_MCH_PEAK_VOL          0x0001031B

/** ID of Tx MIC_MONO in the COPP audio postprocessing device topology.

@inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_MONO_AUDIO_COPP_topology.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Tx Mic Gain                       (#AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL)
    - High Pass Filter                  (#AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER)
    - Enhanced Audio Noise Suppression  (#AUDPROC_MODULE_ID_EANS)
    - Tx IIR Tuning Filter              (#AUDPROC_MODULE_ID_TX_IIR_FILTER)
    - Adaptive Input Gain               (#AUDPROC_MODULE_ID_AIG)
    - Dynamic Range Control             (#AUDPROC_MODULE_ID_MBDRC)
    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_MONO_AUDIO_COPP           0x00010315

/** ID of Tx MIC_STEREO in the COPP audio postprocessing device topology.

@inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_STEREO_AUDIO_COPP_topology.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Tx Mic Gain                       (#AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL)
    - High Pass Filter                  (#AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER)
    - Enhanced Audio Noise Suppression  (#AUDPROC_MODULE_ID_EANS)
    - Tx IIR Tuning Filter              (#AUDPROC_MODULE_ID_TX_IIR_FILTER)
    - Adaptive Input Gain               (#AUDPROC_MODULE_ID_AIG)
    - Dynamic Range Control             (#AUDPROC_MODULE_ID_MBDRC)
    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER) @newpage
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_MIC_STEREO_AUDIO_COPP         0x00010316

/** ID of Tx Multichannel IIR in the COPP audio postprocessing
    device topology.

    @inputfig{1,AUDPROC_COPP_TOPOLOGY_ID_MCHAN_IIR_AUDIO_topology.png}

    - Tx Multichannel IIR Tuning Filter
      (#AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER)
*/
#define AUDPROC_COPP_TOPOLOGY_ID_MCHAN_IIR_AUDIO                    0x00010715
/** @endcond */

/** ID of the default COPP audio postprocessing device topology.

  @if OEM_only
    @inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multi-band Dynamic Range Control  (#AUDPROC_MODULE_ID_MBDRC)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
    @newpage
  @endif

  @if ISV_only
    @inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP_topology_subset.png}

    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
  @endif

*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_DEFAULT_AUDIO_COPP           0x00010BE3

/** @cond OEM_only */
/** ID of the default COPP open topology with peak meter.

    @inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_PEAKMETER_AUDIO_COPP_topology.png}

    - Resampler                         (#AUDPROC_MODULE_ID_RESAMPLER)
    - Peak Meter                        (#AUDPROC_MODULE_ID_PEAK_METER)
    - Soft Volume Control               (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Equalizer                         (#AUDPROC_MODULE_ID_EQUALIZER)
    - Psychoacoustic Bass Enhancement   (#AUDPROC_MODULE_ID_PBE)
    - IIR Tuning filter                 (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER)
    - Multi-band Dynamic Range Control  (#AUDPROC_MODULE_ID_MBDRC)
    - Spectrum Analyzer                 (#AUDPROC_MODULE_ID_SPA)
    - Rx Codec Gain                     (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_PEAKMETER_AUDIO_COPP          0x00010317

/** ID of the default COPP open topology for compressed data.

    @inputfig{1,AUDPROC_POPP_TOPOLOGY_ID_COMPRESSED_DEFAULT_topology.png}

    - Latency (compressed)         (#AUDPROC_MODULE_ID_COMPRESSED_LATENCY)
    - Mute (compressed)            (#AUDPROC_MODULE_ID_COMPRESSED_MUTE)
*/
#define AUDPROC_TOPOLOGY_ID_COMPRESSED_DEFAULT                      0x0001076B

/** ID of the topology containing no modules for compressed data. @newpage */
#define AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE                         0x00010774

/** @endcond */
/** @} */ /* end_addtogroup audiopp_topos_adm */

/** @cond OEM_only */
/** @addtogroup audiopp_module_aig
@{ */
/** ID of the audio AIG module.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_AIG_ENABLE
    - #AUDPROC_PARAM_ID_AIG_CONFIG
*/
#define AUDPROC_MODULE_ID_AIG                         0x00010716

/** ID of the AIG Enable parameter used by AUDPROC_MODULE_ID_AIG.

    @msgpayload{Audio_AigEnable_t}
    @table{weak__audio__aigenable__t}
*/
#define AUDPROC_PARAM_ID_AIG_ENABLE                   0x00010717

/** @} */ /* end_addtogroup audiopp_module_aig */

/* Structure for enabling the configuration parameter for
    AUDIO AIG enable. */
typedef struct Audio_AigEnable_t Audio_AigEnable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audio_aigenable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_AIG_ENABLE parameter in the AIG
 module.
*/
struct Audio_AigEnable_t
{

    uint32_t                  enable;
    /**< Specifies whether AIG is disabled (0) or enabled (nonzero).
         The default is 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audio_aigenable_t */

/** @ingroup audiopp_module_aig
    ID of the AIG Tunable parameters used by AUDPROC_MODULE_ID_AIG.

    @msgpayload{Audio_AigParam_t}
    @table{weak__audio__aigparam__t}
*/
#define AUDPROC_PARAM_ID_AIG_CONFIG                   0x00010718

/* Structure for Audio Aig configuration parameters */
typedef struct Audio_AigParam_t Audio_AigParam_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audio_aigparam_t
@{ */
/* Payload of the AUDIO_PARAM_AIG parameters in the AIG module.
*/
struct Audio_AigParam_t
{

    uint16_t                           mode;
    /**< Mode word for enabling AIG/SIG mode .

         @values
         - 0 -- Static Input Gain (SIG)
         - 1 -- Adaptive Input Gain (AIG)

         Byte offset: 0 */

    int16_t                           staticGainL16Q12;
    /**< Static input gain when the mode field is set to 1.

         Byte offset: 2 */

    int16_t                           initialGainDBL16Q7;
    /**< Initial value from which the adaptive gain update starts (decibels in
         Q7 format).

         Byte offset: 4 */

    int16_t                           idealRMSDBL16Q7;
    /**< Average RMS level that the AIG tries to achieve (in Q8.7 format).

         Byte offset: 6 */

    int32_t                           noiseGateL32;
    /**< Threshold below which a signal is considered as noise and AIG is not
         applied.

         Byte offset: 8 */

    int32_t                           minGainL32Q15;
    /**< Minimum gain that can be provided by the AIG (in Q16.15 format).

         Byte offset: 12 */

    int32_t                           maxGainL32Q15;
    /**< Maximum gain that can be provided by the AIG (in Q16.15 format).

         Byte offset: 16 */

    uint32_t                          gainAtRtUL32Q31;
    /**< Attack/release time for the AIG update (in Q1.31 format).

         Byte offset: 20 */

    uint32_t                          longGainAtRtUL32Q31;
    /**< Long attack/release time while updating gain for noise/silence
         (in Q1.31 format).

         Byte offset: 24 */

    uint32_t                          rmsTavUL32Q32;
    /**< RMS smoothing time constant used for long-term RMS estimate (in
         Q0.32 format).

         Byte offset: 28 */

    uint32_t                          gainUpdateStartTimMsUL32Q0;
    /**< Waiting time before which the AIG applies the adaptive gain update
         (in Q32.0 format).

         Byte offset: 32 */
     }

#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audio_aigparam_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_eans
@{ */
/** ID of the EANS I module.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_EANS_ENABLE
    - #AUDPROC_PARAM_ID_EANS_PARAMS
*/
#define AUDPROC_MODULE_ID_EANS                            0x00010C4A

/** ID of the EANS Enable parameter used by AUDPROC_MODULE_ID_EANS.

    @msgpayload{audproc_eans_enable_t}
    @table{weak__audproc__eans__enable__t}
*/
#define AUDPROC_PARAM_ID_EANS_ENABLE                      0x00010C4B

/** ID of the EANS Configuration parameters used by AUDPROC_MODULE_ID_EANS.

    @msgpayload{audproc_eans_params_t}
    @table{weak__audproc__eans__params__t} @newpage
*/
#define AUDPROC_PARAM_ID_EANS_PARAMS                      0x00010C4C

/** @} */ /* end_addtogroup audiopp_module_eans */

/* Structure for enabling the configuration parameter for
    spectrum analyzer enable. */
typedef struct audproc_eans_enable_t audproc_eans_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eans_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EANS_ENABLE parameter in the EANS module.
*/
struct audproc_eans_enable_t
{

    uint32_t                  enable_flag;
    /**< Specifies whether EANS is disabled (0) or enabled (nonzero).

         This is supported only for sampling rates of 8, 12, 16, 24, 32, and
         48&nbsp;kHz. It is not supported for sampling rates of 11.025, 22.05, or
         44.1nbsp;kHz. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eans_enable_t */

/* Structure for EANS configuration parameters. */
typedef struct audproc_eans_params_t audproc_eans_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eans_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EANS_ENABLE parameters in the EANS module.
*/
struct audproc_eans_params_t
{
    int16_t                         eans_mode;
    /**< Mode word for enabling/disabling submodules.

         Byte offset: 0 */

    int16_t                         eans_input_gain;
    /**< Input gain to the EANS module (in Q2.13 format).

         Byte offset: 2 */

    int16_t                         eans_output_gain;
    /**< Output gain to the EANS module (in Q2.13 format).

         Byte offset: 4 */

    int16_t                         eans_target_ns;
    /**< Target noise suppression level in dB.

         Byte offset: 6 */

    int16_t                         eans_s_alpha;
    /**< Over-subtraction factor for stationary noise suppression (in Q3.12
         format).

         Byte offset: 8 */

    int16_t                         eans_n_alpha;
    /**< Over-subtraction factor for nonstationary noise suppression (in Q3.12
         format).

         Byte offset: 10 */

    int16_t                         eans_n_alphamax;
    /**< Maximum over-subtraction factor for nonstationary noise suppression
         (in Q3.12 format).

         Byte offset: 12 */

    int16_t                         eans_e_alpha;
    /**< Scaling factor for excess noise suppression (in Q15 format).

         Byte offset: 14 */

    int16_t                         eans_ns_snrmax;
    /**< Upper boundary in dB for SNR estimation.

         Byte offset: 16 */

    int16_t                         eans_sns_block;
    /**< Quarter block size for stationary noise suppression.

         Byte offset: 18  */

    int16_t                         eans_ns_i;
    /**< Initialization block size for noise suppression.

         Byte offset: 20 */

    int16_t                         eans_np_scale;
    /**< Power scale factor for nonstationary noise update.

         Byte offset: 22 */

    int16_t                         eans_n_lambda;
    /**< Smoothing factor for higher level nonstationary noise update.

         Byte offset: 24 */

    int16_t                         eans_n_lambdaf;
    /**< Medium averaging factor for noise update.

         Byte offset: 26 */

    int16_t                         eans_gs_bias;
    /**< Bias factor in dB for gain calculation.

         Byte offset: 28 */

    int16_t                         eans_gs_max;
    /**< SNR lower boundary in dB for aggressive gain calculation.

         Byte offset: 30 */

    int16_t                         eans_s_alpha_hb;
    /**< Over-subtraction factor for high-band stationary noise suppression
         (in Q3.12 format).

         Byte offset: 32 */

    int16_t                         eans_n_alphamax_hb;
    /**< Maximum over-subtraction factor for high-band nonstationary noise
         suppression (in Q3.12 format).

         Byte offset: 34 */

    int16_t                         eans_e_alpha_hb;
    /**< Scaling factor for high-band excess noise suppression (in Q15 format).

         Byte offset: 36 */

    int16_t                         eans_n_lambda0;
    /**< Smoothing factor for nonstationary noise update during speech activity.

         Byte offset: 38 */

    int16_t                         thresh;
    /**< Threshold for generating a binary VAD decision.

         Byte offset: 40 */

    int16_t                         pwr_scale;
    /**< Indirect lower boundary of the noise level estimate.

         Byte offset: 42 */

    int16_t                         hangover_max;
    /**< Avoids mid-speech clipping and reliably detects weak speech bursts at
         the end of speech activity.

         Byte offset: 44 */

    int16_t                         alpha_snr;
    /**< Controls responsiveness of the VAD.

         Byte offset: 46 */

    int16_t                         snr_diff_max;
    /**< Maximum SNR difference. Decreasing this parameter value may help in
         making correct decisions during abrupt changes; however, decreasing
         too much may increase false alarms during long pauses/silences.

         Byte offset: 48 */

    int16_t                         snr_diff_min;
    /**< Minimum SNR difference. Decreasing this parameter value may help in
         making correct decisions during abrupt changes; however, decreasing
         too much may increase false alarms during long pauses/silences.

         Byte offset: 50 */

    int16_t                         init_length;
    /**< Defines the number of frames for which a noise level estimate is set
         to a fixed value.

         Byte offset: 52 */

    int16_t                         max_val;
    /**< Defines the upper limit of the noise level.

         Byte offset: 54 */

    int16_t                         init_bound;
    /**< Defines the initial bounding value for the noise level estimate. This
         is used during the initial segment defined by the init_length
         parameter.

         Byte offset: 56 */

    int16_t                         reset_bound;
    /**< Reset boundary for noise tracking.

         Byte offset: 58  */

    int16_t                         avar_scale;
    /**< Defines the bias factor in noise estimation.

         Byte offset: 60 */

    int16_t                         sub_nc;
    /**< Defines the window length for noise estimation.

         Byte offset: 62 */

    int16_t                         spow_min;
    /**< Defines the minimum signal power required to update the boundaries
         for the noise floor estimate.

         Byte offset: 64 */

    int16_t                         eans_gs_fast;
    /**< Fast smoothing factor for postprocessor gain.

         Byte offset: 66 */

    int16_t                         eans_gs_med;
    /**< Medium smoothing factor for postprocessor gain.

         Byte offset: 68 */

    int16_t                         eans_gs_slow;
    /**< Slow smoothing factor for postprocessor gain.

         Byte offset: 70 */

    int16_t                         eans_swb_salpha;
    /**< Super wideband aggressiveness factor for stationary noise suppression
         (in Q3.12 format).

         Byte offset: 72 */

    int16_t                         eans_swb_nalpha;
    /**< Super wideband aggressiveness factor for nonstationary noise
         suppression (in Q3.12 format).

         Byte offset: 74 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eans_params_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_tx_mic_gain
@{ */
/** ID of the Tx Mic Gain Control module. */
#define AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL                          0x00010C35

/** ID of the Tx Mic Gain Control parameter used by
    AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL.

    @msgpayload{audproc_tx_mic_gain_t}
    @table{weak__audproc__tx__mic__gain__t}
*/
#define AUDPROC_PARAM_ID_TX_MIC_GAIN                                0x00010C36

/** @} */ /* end_addtogroup audiopp_module_tx_mic_gain */

/* Structure for a Tx mic gain parameter for the mic gain
    control module. */
typedef struct audproc_tx_mic_gain_t audproc_tx_mic_gain_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_tx_mic_gain_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_TX_MIC_GAIN parameter in the
    Tx Mic Gain Control module.
*/
struct audproc_tx_mic_gain_t
{
    uint16_t                  tx_mic_gain;
    /**< Linear gain (in Q13 format). */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_tx_mic_gain_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_rx_codec_gain
@{ */
/** ID of the Rx Codec Gain Control module. */
#define AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL                        0x00010C37

/** ID of the Rx Codec Gain Control parameter used by
    AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL.

    @msgpayload{audproc_rx_codec_gain_t}
    @table{weak__audproc__rx__codec__gain__t}
*/
#define AUDPROC_PARAM_ID_RX_CODEC_GAIN                              0x00010C38

/** @} */ /* end_addtogroup audiopp_module_rx_codec_gain */

/* Structure for the Rx common codec gain control module. */
typedef struct audproc_rx_codec_gain_t audproc_rx_codec_gain_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_rx_codec_gain_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_RX_CODEC_GAIN parameter in the
    Rx Codec Gain Control module.
*/
struct audproc_rx_codec_gain_t
{
    uint16_t                  rx_codec_gain;
    /**< Linear gain (in Q13 format). */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_rx_codec_gain_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_tx_hpf_iir_tuning_filter
@{ */
/** ID of the HPF IIR Tuning Filter module on the Tx path.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG
    - #AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN
    - #AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS
*/
#define AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER                         0x00010C3D

/** ID of the Tx HPF IIR Filter Enable parameter used by
    AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER.

    @msgpayload{audproc_hpf_tx_iir_filter_enable_cfg_t}
    @table{weak__audproc__hpf__tx__iir__filter__enable__cfg__t}
*/
#define AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG            0x00010C3E

/** ID of the Tx HPF IIR Filter Pregain parameter used by
    AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER.

    @msgpayload{audproc_hpf_tx_iir_filter_pre_gain_t}
    @table{weak__audproc__hpf__tx__iir__filter__pre__gain__t} @newpage
*/
#define AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN                 0x00010C3F

/** ID of the Tx HPF IIR Filter Configuration parameters used by
    #AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER.

    @msgpayload{audproc_hpf_tx_iir_filter_cfg_params_t}
    @table{weak__audproc__hpf__tx__iir__filter__cfg__params__t}
    @keep{8} This structure is followed by the HPF IIR filter coefficients
    on the Tx path as follows:
    @par
    - Sequence of int32 ulFilterCoeffs -- Each band has five coefficients, each
      in int32 format in the order of b0, b1, b2, a1, a2.
    - Sequence of int16 sNumShiftFactor -- One int16 per band. The numerator
      shift factor is related to the Q factor of the filter coefficients.
    - Sequence of uint16 usPanSetting -- One uint16 for each band to indicate
      if the filter is applied to left (0), center (1), and right (2) channels.
*/
#define AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS            0x00010C40

/** @} */ /* end_addtogroup audiopp_module_tx_hpf_iir_tuning_filter */

/* Structure for enabling a configuration parameter for
    the HPF IIR tuning filter module on the Tx path. */
typedef struct audproc_hpf_tx_iir_filter_enable_cfg_t audproc_hpf_tx_iir_filter_enable_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_hpf_tx_iir_filter_enable_cfg_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG
    parameter in the Tx path HPF Tuning Filter module.
*/
struct audproc_hpf_tx_iir_filter_enable_cfg_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the HPF tuning filter is disabled (0) or
         enabled (nonzero). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_hpf_tx_iir_filter_enable_cfg_t */

/* Structure for the pregain parameter for the HPF
    IIR tuning filter module on the Tx path. */
typedef struct audproc_hpf_tx_iir_filter_pre_gain_t audproc_hpf_tx_iir_filter_pre_gain_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_hpf_tx_iir_filter_pre_gain_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN parameter
    in the Tx path HPF Tuning Filter module.
*/
struct audproc_hpf_tx_iir_filter_pre_gain_t
{
    uint16_t                  pre_gain;
    /**< Linear gain (in Q13 format). */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_hpf_tx_iir_filter_pre_gain_t */

/* Structure for the configuration parameter for the
    HPF IIR tuning filter module on the Tx path. */
typedef struct audproc_hpf_tx_iir_filter_cfg_params_t audproc_hpf_tx_iir_filter_cfg_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_hpf_tx_iir_filter_cfg_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS
    parameters on the Tx path HPF Tuning Filter module.
*/
struct audproc_hpf_tx_iir_filter_cfg_params_t
{
    uint16_t                  num_biquad_stages;
    /**< Number of bands.

         @values 0 to 20 */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_hpf_tx_iir_filter_cfg_params_t */
/** @endcond */

/** @cond OEM_only */
/** @ingroup audiopp_module_tx_iir_tuning_filter
    ID of the IIR Tuning Filter module on the Tx path.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN
    - #AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS
*/
#define AUDPROC_MODULE_ID_TX_IIR_FILTER                             0x00010C41

/** @addtogroup audiopp_module_rx_iir_tuning_filters
@{ */
/** ID of the IIR Tuning Filter module on the Rx path for the left channel.

    The parameter IDs of the IIR tuning filter module
    (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER) are used for the left IIR Rx tuning
    filter.

    Pan parameters are not required for this per-channel IIR filter, and they
    are ignored by this module.
*/
#define AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER                     0x00010705

/** ID of the IIR Tuning Filter module on the Rx path for the right channel.

    The parameter IDs of the IIR tuning filter module
    (#AUDPROC_MODULE_ID_IIR_TUNING_FILTER) are used for the right IIR Rx tuning
    filter.

    Pan parameters are not required for this per-channel IIR filter, and they
    are ignored by this module.
*/
#define AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER                    0x00010706

/** @} */ /* end_addtogroup audiopp_module_rx_iir_tuning_filters */

/** @addtogroup audiopp_module_tx_iir_tuning_filter
@{ */
/** ID of the Tx IIR Filter Enable parameter used by
    AUDPROC_MODULE_ID_TX_IIR_FILTER.

    @msgpayload{audproc_tx_iir_filter_enable_cfg_t}
    @table{weak__audproc__tx__iir__filter__enable__cfg__t}
*/
#define AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG                0x00010C42

/** ID of the Tx IIR Filter Pregain parameter used by
    AUDPROC_MODULE_ID_TX_IIR_FILTER.

    @msgpayload{audproc_tx_iir_filter_pre_gain_t}
    @table{weak__audproc__tx__iir__filter__pre__gain__t}
*/
#define AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN                     0x00010C43

/** ID of the Tx IIR Filter Configuration parameters used by
    AUDPROC_MODULE_ID_TX_IIR_FILTER.

    @msgpayload{audproc_tx_iir_filter_cfg_params_t}
    @table{weak__audproc__tx__iir__filter__cfg__params__t}
    @keep{8} This structure is followed by the IIR filter coefficients on
    the Tx path as follows:
    @par
    - Sequence of int32 ulFilterCoeffs -- Each band has five coefficients, each
      in int32 format in the order of b0, b1, b2, a1, a2.
    - Sequence of int16 sNumShiftFactor -- One int16 per band. The numerator
      shift factor is related to the Q factor of the filter coefficients.
    - Sequence of uint16 usPanSetting -- One uint16 for each band to indicate
      if the filter is applied to left (0), center (1), and right (2) channels.
*/
#define AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS                0x00010C44

/** @} */ /* end_addtogroup audiopp_module_tx_iir_tuning_filter */

/* Structure for enabling the configuration parameter for the
    IIR filter module on the Tx path. */
typedef struct audproc_tx_iir_filter_enable_cfg_t audproc_tx_iir_filter_enable_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_tx_iir_filter_enable_cfg_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG parameter
    in the Tx Path IIR Tuning Filter module.
*/
struct audproc_tx_iir_filter_enable_cfg_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the IIR tuning filter is disabled (0) or enabled
         (nonzero). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_tx_iir_filter_enable_cfg_t */

/* Structure for the pregain parameter for the
    IIR filter module on the Tx path. */
typedef struct audproc_tx_iir_filter_pre_gain_t audproc_tx_iir_filter_pre_gain_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_tx_iir_filter_pre_gain_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN parameter
    in the Tx Path IIR Tuning Filter module.
*/
struct audproc_tx_iir_filter_pre_gain_t
{
    uint16_t                  pre_gain;
    /**< Linear gain (in Q13 format). */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_tx_iir_filter_pre_gain_t */

/* Structure for the configuration parameter for the
    IIR filter module on the Tx path. */
typedef struct audproc_tx_iir_filter_cfg_params_t audproc_tx_iir_filter_cfg_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_tx_iir_filter_cfg_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS parameter
    in the Tx Path IIR Tuning Filter module.
*/
struct audproc_tx_iir_filter_cfg_params_t
{
    uint16_t                  num_biquad_stages;
    /**< Number of bands.

         @values 0 to 20 */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_tx_iir_filter_cfg_params_t */
/** @endcond */

/** @addtogroup audiopp_module_upmix
*/
/*  ID for Upmix from mono to stereo converter
    To be used with custom topology in tunnel mode.
	The module converts the stream to stereo if the
	the channel is mono or else by passes the stream.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_MONO2STEREO_UPMIX_ENABLE

*/
#define AUDPROC_MODULE_ID_MONO2STEREO_UPMIX                                  0x000108d6

/** ID of the Virtualizer Enable parameter used by
    AUDPROC_MODULE_ID_MONO2STEREO_UPMIX.

    @msgpayload{audproc_mono2stereo_upmix_enable_t}
    @table{weak__audproc__mono2stereo__upmix__enable__t}
*/

#define AUDPROC_PARAM_ID_MONO2STEREO_UPMIX_ENABLE                      0x000108d7

#include "adsp_begin_pack.h"

/** @weakgroup weak__audproc__mono2stereo__upmix__enable__t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MONO2STEREO_UPMIX_ENABLE parameter used by the
    Mono2Stereo_upmix module.
*/

typedef struct audproc_mono2stereo_upmix_enable_t audproc_mono2stereo_upmix_enable_t;

struct audproc_mono2stereo_upmix_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the mono2stereo upmix is disabled (0) or enabled (1). */

}

#include "adsp_end_pack.h"
;

/** @addtogroup audiopp_module_volume_control
@{ */
/** ID of the Volume Control module pre/postprocessing block.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN
    - #AUDPROC_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN
    - #AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG
    - #AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS
    - #AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS
    @if OEM_only
    - #AUDPROC_PARAM_ID_MULTICHANNEL_GAIN
    - #AUDPROC_PARAM_ID_MULTICHANNEL_MUTE
    @endif
*/
#define AUDPROC_MODULE_ID_VOL_CTRL                                  0x00010BFE

/** ID of the Master Gain parameter used by AUDPROC_MODULE_ID_VOL_CTRL.

    @msgpayload{audproc_volume_ctrl_master_gain_t}
    @table{weak__audproc__volume__ctrl__master__gain__t}
*/
#define AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN                       0x00010BFF

/** ID of the Left/Right Channel Gain parameter used by
    AUDPROC_MODULE_ID_VOL_CTRL.

    @msgpayload{audproc_volume_ctrl_lr_chan_gain_t}
    @table{weak__audproc__volume__ctrl__lr__chan__gain__t}
*/
#define AUDPROC_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN                   0x00010C00

/** ID of the mute Configuration parameter used by
    #AUDPROC_MODULE_ID_VOL_CTRL.

    @msgpayload{audproc_volume_ctrl_mute_config_t}
    @table{weak__audproc__volume__ctrl__mute__config__t}
*/
#define AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG                       0x00010C01

/** ID of the Soft Stepping Volume parameters used by
    #AUDPROC_MODULE_ID_VOL_CTRL.

    @msgpayload{audproc_soft_step_volume_params_t}
    @table{weak__audproc__soft__step__volume__params__t}
*/
#define AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS               0x00010C29

/** Supported parameters for a soft stepping linear ramping curve. */
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR                               0

/** Exponential ramping curve. */
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP                                  1

/** Logarithmic ramping curve.  */
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG                                  2

/** Fractional exponent ramping curve. @newpage */
#define AUDPROC_PARAM_SVC_RAMPINGCURVE_FRAC_EXP                             3


/** ID of the Soft Pause parameters used by #AUDPROC_MODULE_ID_VOL_CTRL.

    @msgpayload{audproc_soft_pause_params_t}
    @table{weak__audproc__soft__pause__params__t}
*/
#define AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS                      0x00010D6A

/** @cond OEM_only */
/** ID of the Multichannel Volume Control parameters used by
    #AUDPROC_MODULE_ID_VOL_CTRL.

    @msgpayload{audproc_volume_ctrl_multichannel_gain_t}
    @table{weak__audproc__volume__ctrl__multichannel__gain__t}

    @par Payload format of the channel type/gain pairs (audproc_volume_ctrl_channel_type_gain_pair_t)
    @table{weak__audproc__volume__ctrl__channel__type__gain__pair__t}
*/
#define AUDPROC_PARAM_ID_MULTICHANNEL_GAIN                          0x00010713

/** Maximum number of channels. */
#define VOLUME_CONTROL_MAX_CHANNELS                       8

/** ID of the Multichannel Mute Configuration parameters used by
    #AUDPROC_MODULE_ID_VOL_CTRL.

    @msgpayload{audproc_volume_ctrl_multichannel_mute_t}
    @table{weak__audproc__volume__ctrl__multichannel__mute__t}

    @par Payload format of the channel type/mute setting pairs (audproc_volume_ctrl_channel_type_mute_pair_t)
    @table{weak__audproc__volume__ctrl__channel__type__mute__pair__t} @newpage
*/
#define AUDPROC_PARAM_ID_MULTICHANNEL_MUTE                          0x00010714

/** @endcond */
/** @} */ /* end_addtogroup audiopp_module_volume_control */

/* Structure for the master gain parameter for a volume control module. */
typedef struct audproc_volume_ctrl_master_gain_t audproc_volume_ctrl_master_gain_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_volume_ctrl_master_gain_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN parameter used
    by the Volume Control module.
*/
struct audproc_volume_ctrl_master_gain_t
{
    uint16_t                  master_gain;
    /**< Linear master gain (in Q13 format). */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_volume_ctrl_master_gain_t */

/* Structure for the left/right channel gain parameter for a
    volume control module. */
typedef struct audproc_volume_ctrl_lr_chan_gain_t audproc_volume_ctrl_lr_chan_gain_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_volume_ctrl_lr_chan_gain_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN parameters
    used by the Volume Control module.
*/
struct audproc_volume_ctrl_lr_chan_gain_t
{
    uint16_t                  l_chan_gain;
    /**< Linear gain (in Q13 format) for the left channel. */

    uint16_t                  r_chan_gain;
    /**< Linear gain (in Q13 format) for the right channel. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_volume_ctrl_lr_chan_gain_t */

/* Structure for the mute configuration parameter for a
    volume control module. */
typedef struct audproc_volume_ctrl_mute_config_t audproc_volume_ctrl_mute_config_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_volume_ctrl_mute_config_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG parameter used
    by the Volume Control module.
*/
struct audproc_volume_ctrl_mute_config_t
{
    uint32_t                  mute_flag;
    /**< Specifies whether mute is disabled (0) or enabled (nonzero). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_volume_ctrl_mute_config_t */

/* Structure for holding soft stepping volume parameters. */
typedef struct audproc_soft_step_volume_params_t audproc_soft_step_volume_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_soft_step_volume_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS
    parameters used by the Volume Control module.
*/
struct audproc_soft_step_volume_params_t
{
    uint32_t                  period;
    /**< Period in milliseconds.

         @values 0 to 15000 */

    uint32_t                  step;
    /**< Step in microseconds.

         @values 0 to 15000000 */

    uint32_t                  ramping_curve;
    /**< Ramping curve type.

         @values
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_FRAC_EXP @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_soft_step_volume_params_t */

/* Structure for holding soft pause parameters. */
typedef struct audproc_soft_pause_params_t audproc_soft_pause_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_soft_pause_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS parameters used
    by the Volume Control module.
*/
struct audproc_soft_pause_params_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether soft pause is disabled (0) or enabled (nonzero). */

    uint32_t                  period;
    /**< Period in milliseconds.

         @values 0 to 15000 */

    uint32_t                  step;
    /**< Step in microseconds.

         @values 0 to 15000000 */

    uint32_t                  ramping_curve;
    /**< Ramping curve.

         @values
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG
         - #AUDPROC_PARAM_SVC_RAMPINGCURVE_FRAC_EXP @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_soft_pause_params_t */

/* Structure for holding one channel type - gain pair. */
typedef struct audproc_volume_ctrl_channel_type_gain_pair_t audproc_volume_ctrl_channel_type_gain_pair_t;

#include "adsp_begin_pack.h"

/** @cond OEM_only */
/** @weakgroup weak_audproc_volume_ctrl_channel_type_gain_pair_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MULTICHANNEL_GAIN channel type/gain
    pairs used by the Volume Control module.
    This structure immediately follows the
    audproc_volume_ctrl_multichannel_gain_t structure.
*/
struct audproc_volume_ctrl_channel_type_gain_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the gain setting is to be applied.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint8_t                   reserved1;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved2;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved3;
    /**< Clients must set this field to zero. */

    uint32_t                  gain;
    /**< Gain value for this channel in Q28 format. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_volume_ctrl_channel_type_gain_pair_t */

/* Structure for the multichannel gain command */
typedef struct audproc_volume_ctrl_multichannel_gain_t audproc_volume_ctrl_multichannel_gain_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_volume_ctrl_multichannel_gain_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MULTICHANNEL_GAIN parameters used by
    the Volume Control module.
*/
struct audproc_volume_ctrl_multichannel_gain_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which gain values are provided. Any channels
         present in the data for which gain is not provided are set to unity
         gain.

         @values 1 to #VOLUME_CONTROL_MAX_CHANNELS */

    audproc_volume_ctrl_channel_type_gain_pair_t     gain_data[VOLUME_CONTROL_MAX_CHANNELS];
    /**< Array of channel type/gain pairs.

         @values See the <b>Payload format</b> table below */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_volume_ctrl_multichannel_gain_t */

/* Structure for holding one channel type - mute pair. */
typedef struct audproc_volume_ctrl_channel_type_mute_pair_t audproc_volume_ctrl_channel_type_mute_pair_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_volume_ctrl_channel_type_mute_pair_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MULTICHANNEL_MUTE channel type/mute
    setting pairs used by the Volume Control module.
    This structure immediately follows the
    audproc_volume_ctrl_multichannel_mute_t structure.
*/
struct audproc_volume_ctrl_channel_type_mute_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the mute setting is to be applied.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint8_t                   reserved1;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved2;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved3;
    /**< Clients must set this field to zero. */

    uint32_t                  mute;
    /**< Specifies whether this channel is set to unmute (0) or mute
         (nonzero). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_volume_ctrl_channel_type_mute_pair_t */

/* Structure for the multichannel mute command */
typedef struct audproc_volume_ctrl_multichannel_mute_t audproc_volume_ctrl_multichannel_mute_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_volume_ctrl_multichannel_mute_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MULTICHANNEL_MUTE parameters used by
    the Volume Control module.
*/
struct audproc_volume_ctrl_multichannel_mute_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which mute configuration is provided. Any
         channels present in the data for which mute configuration is not
         provided are set to unmute.

         @values 1 to #VOLUME_CONTROL_MAX_CHANNELS */

    audproc_volume_ctrl_channel_type_mute_pair_t    mute_data[VOLUME_CONTROL_MAX_CHANNELS];
    /**< Array of channel type/mute setting pairs.

         @values See the <b>Payload format</b> table below */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_volume_ctrl_multichannel_mute_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_iir_tuning_filter
@{ */
/** ID of the IIR Tuning Filter module.
    This module supports the following parameter IDs:
    - AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG
    - AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN
    - AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS
*/
#define AUDPROC_MODULE_ID_IIR_TUNING_FILTER                         0x00010C02

/** ID of the IIR Tuning Filter Enable parameter used by
    AUDPROC_MODULE_ID_IIR_TUNING_FILTER.

    @msgpayload{audproc_iir_tuning_filter_enable_t}
    @table{weak__audproc__iir__tuning__filter__enable__t}
*/
#define AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG            0x00010C03

/** ID of the IIR Tuning Filter Pregain parameter used by
    AUDPROC_MODULE_ID_IIR_TUNING_FILTER.

    @msgpayload{audproc_iir_tuning_filter_pregain_t}
    @table{weak__audproc__iir__tuning__filter__pregain__t}
*/
#define AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN                 0x00010C04

/** ID of the IIR Tuning Filter Configuration parameters used by
    AUDPROC_MODULE_ID_IIR_TUNING_FILTER.

    @msgpayload{audproc_iir_filter_config_params_t}
    @table{weak__audproc__iir__filter__config__params__t} @newpage
    @keep{8} This structure is followed by the IIR filter coefficients on
    the Tx path as follows:
    @par
    - Sequence of int32 ulFilterCoeffs -- Each band has five coefficients, each
      in int32 format in the order of b0, b1, b2, a1, a2.
    - Sequence of int16 sNumShiftFactor -- One int16 per band. The numerator
      shift factor is related to the Q factor of the filter coefficients.
    - Sequence of uint16 usPanSetting -- One uint16 for each band to indicate
      if the filter is applied to left (0), center (1), and right (2) channels.
*/
#define AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS            0x00010C05

/** @} */ /* end_addtogroup audiopp_module_iir_tuning_filter */

/* Structure for an enable configuration parameter for an
    IIR tuning filter module. */
typedef struct audproc_iir_tuning_filter_enable_t audproc_iir_tuning_filter_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_iir_tuning_filter_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG
    parameter used by the IIR Tuning Filter module.
*/
struct audproc_iir_tuning_filter_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the IIR tuning filter is disabled (0) or
         enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_iir_tuning_filter_enable_t */

/* Structure for the pregain parameter for an IIR tuning filter module. */
typedef struct audproc_iir_tuning_filter_pregain_t audproc_iir_tuning_filter_pregain_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_iir_tuning_filter_pregain_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN
    parameters used by the IIR Tuning Filter module.
*/
struct audproc_iir_tuning_filter_pregain_t
{
    uint16_t                  pregain;
    /**< Linear gain (in Q13 format). */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_iir_tuning_filter_pregain_t */

/* Structure for the configuration parameter for an IIR tuning filter module. */
typedef struct audproc_iir_filter_config_params_t audproc_iir_filter_config_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_iir_filter_config_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS
    parameters used by the IIR Tuning Filter module.
*/
struct audproc_iir_filter_config_params_t
{
    uint16_t                  num_biquad_stages;
    /**< Number of bands.

         @values 0 to 20 */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_iir_filter_config_params_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_mbdrc
@{ */
/** ID of the Multiband Dynamic Range Control (MBDRC) module on the Tx/Rx
    paths.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_MBDRC_ENABLE
    - #AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS
*/
#define AUDPROC_MODULE_ID_MBDRC                                    0x00010C06

/** ID of the MBDRC Enable parameter used by AUDPROC_MODULE_ID_MBDRC.

    @msgpayload{audproc_mbdrc_enable_t}
    @table{weak__audproc__mbdrc__enable__t}
*/
#define AUDPROC_PARAM_ID_MBDRC_ENABLE                              0x00010C07

/** ID of the MBDRC Configuration parameter used by AUDPROC_MODULE_ID_MBDRC.

    @msgpayload{audproc_mbdrc_config_params_t}
    Following this structure is the <b>Payload for sub-band DRC configuration
    parameters</b>. This sub-band structure must be repeated for each band.
    @tablens{weak__audproc__mbdrc__config__params__t}
    After this DRC structure is configured for valid bands, the next MBDRC
    setparams expects the sequence of sub-band MBDRC filter coefficients (the
    length depends on the number of bands) plus the mute flag for that band
    plus uint16 padding.
    @par
    The filter coefficient and mute flag are of type int16:
    - FIR coefficient = int16 firFilter
    - Mute flag = int16 fMuteFlag
    @par
    @keep{6} The sequence is as follows:
    - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding
    - 2 bands = 97 FIR coefficients + 2 mute flags + uint16 padding
    - 3 bands = 97+33 FIR coefficients + 3 mute flags + uint16 padding
    - 4 bands = 97+33+33 FIR coefficients + 4 mute flags + uint16 padding
    - 5 bands = 97+33+33+33 FIR coefficients + 5 mute flags + uint16 padding
    @par
    @keep{6} For improved filterbank, the sequence is as follows:
    - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding
    - 2 bands = 141 FIR coefficients + 2 mute flags + uint16 padding
    - 3 bands = 141+81 FIR coefficients + 3 mute flags + uint16 padding
    - 4 bands = 141+81+61 FIR coefficients + 4 mute flags + uint16 padding
    - 5 bands = 141+81+61+61 FIR coefficients + 5 mute flags + uint16 padding

    @par Sub-band DRC configuration parameters (audproc_subband_drc_config_params_t)
    @table{weak__audproc__subband__drc__config__params__t}
    @keep{6} To obtain legacy ADRC from MBDRC, use the calibration tool to:
    - Enable MBDRC (EnableFlag = TRUE)
    - Set number of bands to 1 (uiNumBands = 1)
    - Enable the first MBDRC band (DrcMode[0] = DRC_ENABLED = 1)
    - Clear the first band mute flag (MuteFlag[0] = 0)
    - Set the first band makeup gain to unity (compMakeUpGain[0] = 0x2000)
    - Use the legacy ADRC parameters to calibrate the rest of the MBDRC
      parameters. @newpage
*/
#define AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS                         0x00010C08

/** @} */ /* end_addtogroup audiopp_module_mbdrc */

/** @addtogroup audiopp_module_mbdrcv2
@{ */
/** ID of the MBDRC version 2 module pre/postprocessing block.

    This MBDRCV2 module differs from the original MBDRC
    (#AUDPROC_MODULE_ID_MBDRC) in the length of the filters used in each
    sub-band.
*/
#define AUDPROC_MODULE_ID_MBDRCV2                                0x0001070B

/** ID of the Configuration parameter used by AUDPROC_MODULE_ID_MBDRCV2 for the
    improved filter structure of the MBDRC v2 pre/postprocessing block.

    The update to this configuration structure from the original MBDRC is the
    number of filter coefficients in the filter structure. The sequence for
    is as follows:

    - 1 band = 0 FIR coefficient + 1 mute flag + uint16 padding
    - 2 bands = 141 FIR coefficients + 2 mute flags + uint16 padding
    - 3 bands = 141+81 FIR coefficients + 3 mute flags + uint16 padding
    - 4 bands = 141+81+61 FIR coefficients + 4 mute flags + uint16 padding
    - 5 bands = 141+81+61+61 FIR coefficients + 5 mute flags + uint16 padding

    @msgpayload
    This block uses the same parameter structure as
    #AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS.
*/
#define AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2  0x0001070C

/** @} */ /* end_addtogroup audiopp_module_mbdrcv2 */

/* Structure for the enable parameter for an MBDRC module. */
typedef struct audproc_mbdrc_enable_t audproc_mbdrc_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_mbdrc_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MBDRC_ENABLE parameter used by the
    MBDRC module.
*/
struct audproc_mbdrc_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether MBDRC is disabled (0) or enabled (nonzero). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_mbdrc_enable_t */

/* Structure for the configuration parameters for an MBDRC module. */
typedef struct audproc_mbdrc_config_params_t audproc_mbdrc_config_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_mbdrc_config_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS parameters used
    by the MBDRC module.

    Following this structure is the payload for sub-band DRC configuration
    parameters (audproc_subband_drc_config_params_t). This sub-band structure
    must be repeated for each band.
*/
struct audproc_mbdrc_config_params_t
{
    uint16_t                  num_bands;
    /**< Number of bands.

         @values 1 to 5 */

    int16_t                   limiter_threshold;
    /**< Threshold in decibels for the limiter output.

         @values -72 to 18 \n
         Recommended value: 3994 (-0.22 db in Q3.12 format) */

    int16_t                   limiter_makeup_gain;
    /**< Makeup gain in decibels for the limiter output.

         @values -42 to 42 \n
         Recommended value: 256 (0 dB in Q7.8 format) */

    int16_t                   limiter_gc;
    /**< Limiter gain recovery coefficient.

         @values 0.5 to 0.99 \n
         Recommended value: 32440 (0.99 in Q15 format) */

    int16_t                   limiter_delay;
    /**< Limiter delay in samples.

         @values 0 to 10 \n
         Recommended value: 262 (0.008 samples in Q15 format) */

    int16_t                   limiter_max_wait;
    /**< Maximum limiter waiting time in samples.

         @values 0 to 10 \n
         Recommended value: 262 (0.008 samples in Q15 format) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_mbdrc_config_params_t */

/* DRC configuration structure for each sub-band of an MBDRC module. */
typedef struct audproc_subband_drc_config_params_t audproc_subband_drc_config_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_subband_drc_config_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS DRC configuration
    parameters for each sub-band in the MBDRC module.
    (See also audproc_mbdrc_config_params_t.)
*/
struct audproc_subband_drc_config_params_t
{
    int16_t                   drc_stereo_linked_flag;
    /**< Specifies whether all stereo channels have the same applied dynamics
         or if they process their dynamics independently.

         @values
         - 0 -- Not linked; channels process the dynamics independently
         - 1 -- Linked; channels have the same applied dynamics @tablebulletend */

    int16_t                   drc_mode;
    /**< Specifies whether DRC mode is bypassed for sub-bands.

         @values
         - 0 -- Disabled
         - 1 -- Enabled @tablebulletend */

    int16_t                   drc_down_sample_level;
    /**< DRC down sample level.

         @values @ge 1 */

    int16_t                   drc_delay;
    /**< DRC delay in samples.

         @values 0 to 1200 */

    uint16_t                  drc_rms_time_avg_const;
    /**< RMS signal energy time-averaging constant.

         @values 0 to 2^16-1 */

    uint16_t                  drc_makeup_gain;
    /**< DRC makeup gain in decibels.

         @values 258 to 64917 */

    /* Down expander settings */

    int16_t                   down_expdr_threshold;
    /**< Down expander threshold.

         @values 1320 to up_cmpsr_threshold (in Q7 format) */

    int16_t                   down_expdr_slope;
    /**< Down expander slope.

         @values -32768 to 0 (in Q8 format) */

    uint32_t                  down_expdr_attack;
    /**< Down expander attack constant.

         @values 196844 to 2^31 (in Q31 format) */

    uint32_t                  down_expdr_release;
    /**< Down expander release constant.

         @values 19685 to 2^31 (in Q31 format) */

    uint16_t                  down_expdr_hysteresis;
    /**< Down expander hysteresis constant.

         @values 1 to 32690 (in Q14 format) */

    uint16_t                  reserved;
    /**< Clients must set this field to zero. */

    int32_t                   down_expdr_min_gain_db;
    /**< Down expander minimum gain.

         @values -805306368 to 0.  (in Q23 format)*/

    /* Up compressor settings */

    int16_t                   up_cmpsr_threshold;
    /**< Up compressor threshold.

         @values down_expdr_threshold to down_cmpsr_threshold (in Q7 format) */

    uint16_t                  up_cmpsr_slope;
    /**< Up compressor slope.

         @values 0 to 64881  (in Q16 format)*/

    uint32_t                  up_cmpsr_attack;
    /**< Up compressor attack constant.

         @values 196844 to 2^31  (in Q31 format)*/

    uint32_t                  up_cmpsr_release;
    /**< Up compressor release constant.

         @values 19685 to 2^31  (in Q31 format)*/

    uint16_t                  up_cmpsr_hysteresis;
    /**< Up compressor hysteresis constant.

         @values 1 to 32690 (in Q14 format) */

    /* Down compressor settings */

    int16_t                   down_cmpsr_threshold;
    /**< Down compressor threshold.

         @values up_cmpsr_threshold to 11560 (in Q7 format) */

    uint16_t                  down_cmpsr_slope;
    /**< Down compressor slope.

         @values{(in Q16 format)}: 0 to 64881 */

    uint16_t                  reserved1;
    /**< Clients must set this field to zero. */

    uint32_t                  down_cmpsr_attack;
    /**< Down compressor attack constant.

         @values 196844 to 2^31 (in Q31 format) */

    uint32_t                  down_cmpsr_release;
    /**< Down compressor release constant.

         @values 19685 to 2^31 (in Q31 format) */

    uint16_t                  down_cmpsr_hysteresis;
    /**< Down compressor hysteresis constant.

         @values 1 to 32690 (in Q14 format) */

    uint16_t                  reserved2;
    /**< Clients must set this field to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_subband_drc_config_params_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_equalizer
@{ */
/** ID of the Equalizer module postprocessing block. */
#define AUDPROC_MODULE_ID_EQUALIZER                                 0x00010C27

/** ID of the Equalizer Configuration parameter used by
    AUDPROC_MODULE_ID_EQUALIZER.

    @msgpayload{audproc_eq_params_t}
    @tablens{weak__audproc__eq__params__t}

    @par Per-band equalizer parameters (audproc_eq_per_band_params_t)
    @table{weak__audproc__eq__per__band__params__t}
*/
#define AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS                       0x00010C28

/** @} */ /* end_addtogroup audiopp_module_equalizer */

/* Structure for equalizer module parameters. */
typedef struct audproc_eq_params_t audproc_eq_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS parameters used
    by the Equalizer module. */
/** Following this structure is a sequence of <b>Per-band equalizer
    parameters</b>. The length is dependent on the num_bands value.
*/
struct audproc_eq_params_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the equalizer module is disabled (0) or enabled
         (nonzero). */

    uint32_t                  num_bands;
    /**< Number of bands.

         @values 1 to 12 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_params_t */

/** @addtogroup audiopp_module_equalizer
@{ */
/** No equalizer effect. */
#define AUDPROC_PARAM_EQ_TYPE_NONE                                       0

/** Bass boost equalizer effect. */
#define AUDPROC_PARAM_EQ_BASS_BOOST                                      1

/** Bass cut equalizer effect. */
#define AUDPROC_PARAM_EQ_BASS_CUT                                        2

/** Treble boost equalizer effect. */
#define AUDPROC_PARAM_EQ_TREBLE_BOOST                                    3

/** Treble cut equalizer effect. */
#define AUDPROC_PARAM_EQ_TREBLE_CUT                                      4

/** Band boost equalizer effect. */
#define AUDPROC_PARAM_EQ_BAND_BOOST                                      5

/** Band cut equalizer effect. @newpage */
#define AUDPROC_PARAM_EQ_BAND_CUT                                        6

/** @} */ /* end_addtogroup audiopp_module_equalizer */

/* Structure for per-band parameters for the equalizer module. */
typedef struct audproc_eq_per_band_params_t audproc_eq_per_band_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_per_band_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS per-band
    parameters used by the Equalizer module. (See also audproc_eq_params_t.)
*/
struct audproc_eq_per_band_params_t
{
    uint32_t                  band_idx;
    /**< Band index.

         @values 0 to 11 */

    uint32_t                  filter_type;
    /**< Type of filter.

         @values
         - #AUDPROC_PARAM_EQ_TYPE_NONE
         - #AUDPROC_PARAM_EQ_BASS_BOOST
         - #AUDPROC_PARAM_EQ_BASS_CUT
         - #AUDPROC_PARAM_EQ_TREBLE_BOOST
         - #AUDPROC_PARAM_EQ_TREBLE_CUT
         - #AUDPROC_PARAM_EQ_BAND_BOOST
         - #AUDPROC_PARAM_EQ_BAND_CUT @tablebulletend */

    uint32_t                  center_freq_hz;
    /**< Filter band center frequency in Hertz. */

    int32_t                   filter_gain;
    /**< Filter band initial gain.

         @values +12 to -12 dB in 1 dB increments */

    int32_t                   q_factor;
    /**< Filter band quality factor expressed as a Q8 number, i.e., a
         fixed-point number with q factor of 8. For example, 3000/(2^8). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_per_band_params_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_pbe
@{ */
/** ID of the Psychoacoustic Bass Enhancement module.
    This module supports the following parameter IDs:
    - AUDPROC_PARAM_ID_PBE_ENABLE
    - AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG
*/
#define AUDPROC_MODULE_ID_PBE                              0x00010C2A

/** ID of the PBE Enable Configuration parameter used by AUDPROC_MODULE_ID_PBE.

    @msgpayload{audproc_pbe_enable_t}
    @table{weak__audproc__pbe__enable__t}
*/
#define AUDPROC_PARAM_ID_PBE_ENABLE                       0x00010C2B

/** ID of the PBE Configuration parameter used by AUDPROC_MODULE_ID_PBE.

    @msgpayload{audproc_pbe_params_t}
    @table{weak__audproc__pbe__params__t}
    @keep{8} This structure is followed by the filter coefficients. The length is
    dependent on the number of bands:
    @par
    - Sequence of int32 p1LowpassCoeffsLQ30 -- Low pass filter coefficients if
      uiXoverFltOrder = 3 {length = 5 * 2}. Else, {length = 5}.
    - Sequence of int32 p1HighpassCoeffsQ30 -- High pass filter coefficients
      if uiXoverFltOrder = 3 {length = 5 * 2}. Else, {length = 5}.
    - Sequence of int32 p1BandpassCoeffsQ30 -- Band pass filter coefficients
      if length is 5 * uiBandpassFltOrder. Each band has five coefficients,
      and each coefficient is in int32 format in the order of b0, b1, b2, a1,
      a2.
    - Sequence of int32 p1BassShelfCoeffsQ30 -- Bass shelf filter coefficients
      of length 5. Each coefficient is in int32 format in the order of b0, b1,
      b2, a1, a2.
    - Sequence of int32 p1TrebleShelfCoeffsQ30 -- Treble shelf filter
      coefficients of length 5. Each coefficient is in int32 format in the
      order of b0, b1, b2, a1, a2.
*/
#define AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG                0x00010C49

/** @} */ /* end_addtogroup audiopp_module_pbe */

/* Structure for the enable configuration parameter for PBE enable. */
typedef struct audproc_pbe_enable_t audproc_pbe_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_pbe_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_PBE_ENABLE parameter used by the PBE module.
*/
struct audproc_pbe_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the PBE module is disabled (0) or enabled
         (nonzero). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_pbe_enable_t */

/* Structure for PBE configuration parameters. */
typedef struct audproc_pbe_params_t audproc_pbe_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_pbe_params_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG parameters used by
    the PBE module.
*/
struct audproc_pbe_params_t
{
    int16_t                   real_bass_mix_q15;
    /**< Mix amount of real bass harmonics and virtual bass harmonics.

         @values 0.0 (all virtual) to 1.0 (all real) \n
         Default: 0.4 */

    int16_t                   bass_color_ctrl_q15;
    /**< Color control of virtual bass.

         @values 0.0 (more even harmonics) to 1.0 (more odd harmonics) \n
         Default: 0.3 */

    uint16_t                  main_chain_delay;
    /**< Sample delay in the non-bass path to compensate an algorithm delay in
         the bass path.

         @values 0 to 32 \n
         Default: 15 */

    uint16_t                  xover_filter_order;
    /**< Low-pass and HPF order.

         @values 1 to 3
         - 1 -- 6 dB/oct
         - 2 -- 12 dB/oct (Default)
         - 3 -- 18 dB/oct @tablebulletend */

    uint16_t                  bpass_filter_order;
    /**< Band pass filter order.

         @values 1 to 3
         - 1 -- 6 dB/oct
         - 2 -- 12 dB/oct (Default)
         - 3 -- 18 dB/oct @tablebulletend */

    int16_t                   drc_delay;
    /**< Delay on the signal path to which to apply DRC gain.

         @values 0.0 to 3.0 \n
         Delay: 2.0 */

    uint16_t                  rms_time_avg_ul16q16;
    /**< Short-term root mean square energy estimation time.

         @values 0.0 to 3.0 \n
         Delay: 2.0 */

    int16_t                   expdr_threshold_l16q8;
    /**< Downward expansion threshold.

         @values -80.0 to cmpsr_threashold_l16q7 \n
         Default: -35.0 */

    uint16_t                  expdr_slope_l16q8;
    /**< Downward expansion ratio.

         @values 0.0 to 1.0 \n
         Default: 0.6 */

    int16_t                   cmpsr_threashold_l16q7;
    /**< Downward compression threshold.

         @values expdr_threshold_l16q8 to 0.0 \n
         Default: -20.0 */

    uint16_t                  cmpsr_slope_ul16q16;
    /**< Downward compression ratio.

         @values 1.0 to 30.0 \n
         Default: 10.0 */

    uint16_t                  makeup_gain_ul16q12;
    /**< Makeup gain.

         @values -18.0 to 18.0 \n
         Default: 6.0 */

    uint32_t                  cmpsr_attack_ul32q31;
    /**< Downward compression gain smooth attack time.

         @values 0.0 to 500.0 \n
         Default: 10.0 */

    uint32_t                  cmpsr_release_ul32q31;
    /**< Downward compression gain smooth release time.

         @values 0.0 to 5000.0 \n
         Default: 100.0 */

    uint32_t                  expdr_attack_ul32q31;
    /**< Downward expansion gain smooth attack time.

         @values 0.0 to 500.0 \n
         Default: 50.0 */

    uint32_t                  expdr_release_ul32q31;
    /**< Downward expansion gain smooth release time.

         @values 0.0 to 5000.0 \n
         Default: 50.0 */

    int16_t                   limiter_bass_threshold_q12;
    /**< Output level of bass content.

         @values 0.0 to 48.0 \n
         Default: 27.0 */

    int16_t                   limiter_high_threshold_q12;
    /**< Output level of non-bass content.

         @values 0.0 to 48.0 \n
         Default: 6.0 */

    int16_t                   limiter_bass_makeup_gain_q8;
    /**< Extra boost gain for bass content.

         @values 0.0 to 1.0 \n
         Default: 0.99 */

    int16_t                   limiter_high_makeup_gain_q8;
    /**< Extra boost gain for non-bass content.

         @values 0.0 to 1.0 \n
         Default: 0.99 */

    int16_t                   limiter_bass_grc_q15;
    /**< Limiter gain recovery constant for bass content.

         @values 0.0 to 1.0 \n
         Default: 0.99 */

    int16_t                   limiter_high_grc_q15;
    /**< Limiter gain recovery constant for non-bass content.

         @values 0.0 to 1.0 \n
         Default: 0.99 */

    int16_t                   limiter_delay_q15;
    /**< Limiter delay and maximum waiting time.

         @values 0.0 to 10.0 \n
         Default: 10.0 */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_pbe_params_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_spa
@{ */
/** ID of the Spectrum Analyzer module.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_SPA_ENABLE
    - #AUDPROC_PARAM_ID_SPA_PARAMS_CONFIG
*/
#define AUDPROC_MODULE_ID_SPA                             0x00010C39

/** ID of the SPA Enable parameter used by AUDPROC_MODULE_ID_SPA.

    @msgpayload{audproc_spa_enable_t}
    @table{weak__audproc__spa__enable__t}
*/
#define AUDPROC_PARAM_ID_SPA_ENABLE                       0x00010C3A

/** ID of the SPA Configuration parameter used by AUDPROC_MODULE_ID_SPA.

    @msgpayload{audproc_spa_param_t}
    @table{weak__audproc__spa__param__t}
*/
#define AUDPROC_PARAM_ID_SPA_PARAMS_CONFIG                0x00010C3B

/** @} */ /* end_addtogroup audiopp_module_spa */

/* Structure for the enable configuration parameter for spectrum analyzer enable. */
typedef struct audproc_spa_enable_t audproc_spa_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_spa_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_SPA_ENABLE parameter used by the
    Spectrum Analyzer module.
*/
struct audproc_spa_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the spectrum analyzer is disabled (0) or enabled
         (nonzero). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_spa_enable_t */

/* Structure for the configuration parameters for the spectrum analyzer
    module. */
typedef struct audproc_spa_param_t audproc_spa_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_spa_param_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_SPA_PARAMS_CONFIG parameters used by the
    Spectrum Analyzer module.
*/
struct audproc_spa_param_t
{
    uint32_t                  sample_interval;
    /**< Sample interval.

         @values @ge 512 */

    uint16_t                  sample_points;
    /**< Supported Fast Fourier Transform (FFT) size.

         @values 32, 64, 128, 256 */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_spa_param_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_peakmeter
@{ */
/** ID of the Peak Meter module.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_PEAK_INTERVAL
    - #AUDPROC_PARAM_ID_PEAK_VALUES
*/
#define AUDPROC_MODULE_ID_PEAK_METER            0x00010712

/** ID of the Peak Meter Configuration parameter used by
    AUDPROC_MODULE_ID_PEAK_METER. This parameter indicates the update
    interval of the peak meter.

    @msgpayload{audproc_peakmeter_interval_t}
    @table{weak__audproc__peakmeter__interval__t} @newpage
*/
#define AUDPROC_PARAM_ID_PEAK_INTERVAL                  0x00010711

/** ID of the Peak Meter Values parameters used by
    #AUDPROC_MODULE_ID_PEAK_METER. The peak meter values are returned
    by the aDSP.

    @msgpayload{audproc_peakmeter_values_t}
    @table{weak__audproc__peakmeter__values__t}
*/
#define AUDPROC_PARAM_ID_PEAK_VALUES                    0x00010710

/** Maximum number of channels that supports peak meter measurement. @newpage */
#define PEAKMETER_MAX_NUM_CHANNELS                  8

/** @} */ /* end_addtogroup audiopp_module_peakmeter */

typedef struct audproc_peakmeter_interval_t audproc_peakmeter_interval_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_peakmeter_interval_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_PEAK_INTERVAL parameter used by
    the Peak Meter module.
*/
struct audproc_peakmeter_interval_t
{
    uint32_t            sample_interval;
    /**<  Update time interval in milliseconds of the peak meter.

          @values 0 to 0xFFFFFFFF

          A value of zero disables the peak meter. In this case, the aDSP will
          not report valid peak meter values.

          A nonzero value indicates the peak meter log/reset frequency. For
          example, a value of 20 indicates an interval of 20 ms. This means
          a peak value is updated every 20 ms, and the newly calculated peak
          value replaces the existing value once every 20 ms.

          @note1hang The recommended sample interval is a multiple of 10 ms. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_peakmeter_interval_t */

typedef struct audproc_peakmeter_values_t audproc_peakmeter_values_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_peakmeter_values_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_PEAK_VALUES parameters used by the
    Peak Meter module.
*/
struct audproc_peakmeter_values_t
{
    uint32_t            num_supported_channels;
    /**<  Number of channels for which the peak value was calculated. A value
          of zero indicates that the peak meter is disabled.

          @values 0 to #PEAKMETER_MAX_NUM_CHANNELS */

    int16_t             peak_values[PEAKMETER_MAX_NUM_CHANNELS];
    /**<  Array of peak values for each channel in the most recent window.

          @values 0 to 32767 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_peakmeter_values_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_multichan_iir_tuning_filter
@{ */
/** ID of the Multiple Channel IIR Tuning Filter module.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE
    - #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN
    - #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS
*/
#define AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER              0x0001031F

/** ID of the Multichannel IIR Tuning Filter Enable parameters used by
    AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER.

    @msgpayload{audproc_mchan_iir_enable_t}
    @table{weak__audproc__mchan__iir__enable__t}

    @par Payload format of channel type/IIR enable flag pairs (audproc_channel_type_iir_enable_pair_t)
    @table{weak__audproc__channel__type__iir__enable__pair__t}
*/
#define AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE        0x0001031C

/** Maximum number of channels for the multichannel IIR tuning filter. */
#define IIR_TUNING_FILTER_MAX_CHANNELS                 8

/** @} */ /* end_addtogroup audiopp_module_multichan_iir_tuning_filter */

/* Structure for holding one channel type - IIR enable pair. */
typedef struct audproc_channel_type_iir_enable_pair_t audproc_channel_type_iir_enable_pair_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_channel_type_iir_enable_pair_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE
    channel type/IIR enable pairs used by the Multiple Channel IIR Tuning
    Filter module.

    This structure immediately follows the audproc_mchan_iir_enable_t
    structure.
*/
struct audproc_channel_type_iir_enable_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the IIR enable is to be applied.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint8_t                   reserved1;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved2;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved3;
    /**< Clients must set this field to zero. */

    uint32_t                  enable_flag;
    /**< Specifies whether the channel IIR is disabled (0) or enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_channel_type_iir_enable_pair_t */

/* Structure for the multichannel IIR enable command */
typedef struct audproc_mchan_iir_enable_t audproc_mchan_iir_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_mchan_iir_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_ENABLE
    parameters used by the Multiple Channel IIR Tuning Filter module.
*/
struct audproc_mchan_iir_enable_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which enable flags are provided.

         @values 0 to #IIR_TUNING_FILTER_MAX_CHANNELS */

    audproc_channel_type_iir_enable_pair_t     enable_flag_settings[IIR_TUNING_FILTER_MAX_CHANNELS];
    /**< Channel type/enable_flag value pairs.

         @values See the <b>Payload format</b> table below */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_mchan_iir_enable_t */

/** @ingroup audiopp_module_multichan_iir_tuning_filter
    ID of the Multiple Channel IIR Tuning Filter Pregain parameters used by
    #AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER.

    @msgpayload{audproc_mchan_iir_pregain_t}
    @table{weak__audproc__mchan__iir__pregain__t}

    @par Payload format of channel type/IIR pregain pairs (audproc_channel_type_iir_pregain_pair_t)
    @table{weak__audproc__channel__type__iir__pregain__pair__t}
*/
#define AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN        0x0001031D

/* Structure for holding one channel type - IIR pregain pair. */
typedef struct audproc_channel_type_iir_pregain_pair_t audproc_channel_type_iir_pregain_pair_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_channel_type_iir_pregain_pair_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN
    channel type/IIR pregain pairs used by the Multiple Channel IIR Tuning
    Filter module.
    This structure immediately follows the audproc_mchan_iir_pregain_t
    structure.
*/
struct audproc_channel_type_iir_pregain_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the IIR pregain is to be applied.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint8_t                   reserved1;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved2;
    /**< Clients must set this field to zero. */

    uint8_t                   reserved3;
    /**< Clients must set this field to zero. */

    int32_t                   preGain;
    /**< Pregain of IIR for this channel (in Q27 format). @newpagetable */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_channel_type_iir_pregain_pair_t */

/* Structure for the multichannel IIR preGain command */
typedef struct audproc_mchan_iir_pregain_t audproc_mchan_iir_pregain_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_mchan_iir_pregain_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_PREGAIN
    parameters used by the Multiple Channel IIR Tuning Filter module.
*/
struct audproc_mchan_iir_pregain_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which pregains are provided.

         @values 0 to #IIR_TUNING_FILTER_MAX_CHANNELS */

    audproc_channel_type_iir_pregain_pair_t     preGain_settings[IIR_TUNING_FILTER_MAX_CHANNELS];
    /**< Channel type/pregain value pairs.

         @values See the <b>Payload format</b> table below */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_mchan_iir_pregain_t */

/** @ingroup audiopp_module_multichan_iir_tuning_filter
    ID of the Multiple Channel IIR Tuning Filter Configuration parameters used
    by #AUDPROC_MODULE_ID_MCHAN_IIR_TUNING_FILTER.

    @msgpayload{audproc_mchan_iir_config_t}
    @table{weak__audproc__mchan__iir__config__t}
    @keep{8} This structure is followed by the multichannel IIR filter
    coefficients as follows:
    @par
    - Channel type/configuration pairs -- See the <b>Payload format</b> table
      below.
    - Sequence of int32 ulFilterCoeffs -- Five coefficients for each band, each
      in int32 format in the order of b0, b1, b2, a1, a2.
    - Sequence of int16 sNumShiftFactor -- One int16 per band. The numerator
      shift factor is related to the Q factor of the filter coefficients b0,
      b1, b2.
    @par
    There must be one data sequence per channel.
    @par
    If the number of bands is odd, there must be an extra 16-bit padding by
    the end of the numerator shift factors. This extra 16-bit padding makes
    the entire structure 32-bit aligned. The padding bits must be all zeros.

    @par Payload format of channel type/configuration pairs (audproc_channel_type_iir_config_pair_t)
    @table{weak__audproc__channel__type__iir__config__pair__t}
*/
#define AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS     0x0001031E

/* Structure for the configuration parameter for multichannel IIR tuning filter module. */
typedef struct audproc_channel_type_iir_config_pair_t audproc_channel_type_iir_config_pair_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_channel_type_iir_config_pair_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS
    channel type/configuration pairs used by the Multichannel IIR Tuning Filter
    module.
*/
struct audproc_channel_type_iir_config_pair_t
{
    uint8_t                   channel_type;
    /**< Channel type for which the IIR configuration is to be applied.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint8_t                   reserved;
    /**< Clients must set this field to zero. */

    uint16_t                  num_biquad_stages;
    /**< Number of biquad-IIR bands.

         @values 0 to 20 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_channel_type_iir_config_pair_t */

/* Structure for the multichannel IIR config params */
typedef struct audproc_mchan_iir_config_t audproc_mchan_iir_config_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_mchan_iir_config_t
@{ */
/* Payload of the #AUDPROC_PARAM_ID_MCHAN_IIR_TUNING_FILTER_CONFIG_PARAMS
    parameters used by the Multichannel IIR Tuning Filter module.
*/
struct audproc_mchan_iir_config_t
{
    uint32_t                  num_channels;
    /**< Number of channels for which enable flags are provided.

         @values 0 to #IIR_TUNING_FILTER_MAX_CHANNELS */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_mchan_iir_config_t */

/** @ingroup audiopp_topos_adm
    ID of the COPP topology for the SRS TruMedia module.

    @inputfig{1,ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SRS_TRUMEDIA_TOPOLOGY.png}

    - Resampler                       (#AUDPROC_MODULE_ID_RESAMPLER)
    - SRS TruMedia                    (#APPI_SRS_TRUMEDIA_MODULE_ID)
    - Peak Meter                      (#AUDPROC_MODULE_ID_PEAK_METER)
    - Soft Volume Control             (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Rx Codec Gain                   (#AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL)
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SRS_TRUMEDIA_TOPOLOGY          0x00010D90

/** @addtogroup audiopp_module_srs_trumedia
@{ */
/** ID of the SRS TruMedia module.
    This module supports the following parameter IDs:
    - #APPI_SRS_TRUMEDIA_PARAMS
    - #APPI_SRS_TRUMEDIA_WOWHD_PARAMS
    - #APPI_SRS_TRUMEDIA_CSHP_PARAMS
    - #APPI_SRS_TRUMEDIA_HPF_PARAMS
    - #APPI_SRS_TRUMEDIA_PEQ_PARAMS
    - #APPI_SRS_TRUMEDIA_HL_PARAMS
*/
#define APPI_SRS_TRUMEDIA_MODULE_ID                   0x10005010

/** ID of the SRS Trumedia enable parameters used by
    APPI_SRS_TRUMEDIA_MODULE_ID.

    @msgpayload{srs_TruMedia_params_t}
    @table{weak__srs__trumedia__params__t}
*/
#define APPI_SRS_TRUMEDIA_PARAMS                      0x10005011

/* The following are bit fields for the  Enable_Flags
 * field in srs_TruMedia_params_t structure defined below*/

/** Any SRS processing may occur. This bit must be set if any of the
    other enable macros are to be used.
*/
#define SRS_ENABLE_GLOBAL    0x00000001

/** SRS WOW HD&tm;&nbsp;processing stage is active. */
#define SRS_ENABLE_WOWHD     0x00000002

/** CS Headphone&tm;&nbsp;(CSHP) processing stage is active. */
#define SRS_ENABLE_CSHP      0x00000004

/** High Pass Filter stage is active. */
#define SRS_ENABLE_HPF       0x00000008

/** TruEQ&tm;&nbsp; stage is active. */
#define SRS_ENABLE_PEQ       0x00000010

/** Hard Limiter stage is active. @newpage */
#define SRS_ENABLE_HL        0x00000020

/** @} */ /* end_addtogroup audiopp_module_srs_trumedia */

typedef struct srs_TruMedia_params_t srs_TruMedia_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_srs_trumedia_params_t
@{ */
/* Payload of the APPI_SRS_TRUMEDIA_PARAMS parameters used by the SRS
    TruMedia module.
*/
struct srs_TruMedia_params_t {
   uint8_t                  Enable_Flags;
   /**< Bitfield of macros used to enable SRS Trumedia.

        @values
        - #SRS_ENABLE_GLOBAL
        - #SRS_ENABLE_WOWHD
        - #SRS_ENABLE_CSHP
        - #SRS_ENABLE_HPF
        - #SRS_ENABLE_PEQ
        - #SRS_ENABLE_HL

        The values are a bitwise OR of any of these macros. When a bit
        corresponding to a particular macro is set, the processing block
        associated with that macro is enabled.

        The bit corresponding to SRS_ENABLE_GLOBAL must be set if any of the
        other bits in the bit field are to be set. */

   uint8_t                  Reserved;
   /**< This field must be set to zero. */

   uint8_t                  I_0;
   /**< SRS licensing payload used to verify the SRS Licensing scheme with the
        aDSP module. */

   uint8_t                  V_0;
   /**< SRS licensing payload. */

   uint8_t                  I_1;
   /**< SRS licensing payload. */

   uint8_t                  V_1;
   /**< SRS licensing payload. */

   uint8_t                  I_2;
   /**< SRS licensing payload. */

    uint8_t                  V_2;
   /**< SRS licensing payload. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_srs_trumedia_params_t */

/** @addtogroup audiopp_module_srs_trumedia
@{ */
/** ID of the WOW HD configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.

    @msgpayload{srs_TruMedia_wowhd_params_t}
    @table{weak__srs__trumedia__wowhd__params__t}
*/
#define APPI_SRS_TRUMEDIA_WOWHD_PARAMS                0x10005012

/* The following are bit fields for the EnableFlags field in
 * srs_TruMedia_wowhd_params_t defined below*/

/** Limiter is enabled. */
#define SRS_TRUMEDIA_WOWHD_DOLIMIT          0x0001

/** SRS 3D is enabled. */
#define SRS_TRUMEDIA_WOWHD_DOSRS3D          0x0002

/** TruBass is enabled. */
#define SRS_TRUMEDIA_WOWHD_DOTB             0x0004

/** Split-analysis TruBass is active. */
#define SRS_TRUMEDIA_WOWHD_DOSATB           0x0008

/** Focus is enabled. */
#define SRS_TRUMEDIA_WOWHD_DOFOCUS          0x0010

/** Definition is enabled. */
#define SRS_TRUMEDIA_WOWHD_DODEF            0x0020

/** TruBass is stereo when set, and mono when not set. */
#define SRS_TRUMEDIA_WOWHD_TBMODE           0x0040

/** Headphone mode for SRS 3D is active when set. */
#define SRS_TRUMEDIA_WOWHD_SRSTYPE          0x0080

/** Defines the SRSMODE values. */
#define SRS_TRUMEDIA_WOWHD_SRSMODE_MASK     0x0F00

/** SRS 3D mode is SRSSrs3dMono. */
#define SRS_TRUMEDIA_WOWHD_SRSMODE_MONO     0x0100

/** SRS 3D mode is SRSSrs3dSingleSpeaker. */
#define SRS_TRUMEDIA_WOWHD_SRSMODE_SINGLE   0x0200

/** SRS 3D mode is SRSSrs3dStereo. */
#define SRS_TRUMEDIA_WOWHD_SRSMODE_STEREO   0x0300

/** SRS 3D mode is SRSSrs3dExtreme. @newpage */
#define SRS_TRUMEDIA_WOWHD_SRSMODE_XTREME   0x0400

/** @} */ /* end_addtogroup audiopp_module_srs_trumedia */

typedef struct srs_TruMedia_wowhd_params_t srs_TruMedia_wowhd_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_srs_trumedia_wowhd_params_t
@{ */
/* Payload of the APPI_SRS_TRUMEDIA_WOWHD_PARAMS used by the SRS TruMedia
    module.
*/
struct srs_TruMedia_wowhd_params_t {
    uint32_t                EnableFlags;
   /**< Bitfield of macros used to enable the SRS TruMedia WOW HD
        parameters.

        @values
        - #SRS_TRUMEDIA_WOWHD_DOLIMIT
        - #SRS_TRUMEDIA_WOWHD_DOSRS3D
        - #SRS_TRUMEDIA_WOWHD_DOTB
        - #SRS_TRUMEDIA_WOWHD_DOSATB
        - #SRS_TRUMEDIA_WOWHD_DOFOCUS
        - #SRS_TRUMEDIA_WOWHD_DODEF
        - #SRS_TRUMEDIA_WOWHD_TBMODE
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_MASK
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_MONO
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_SINGLE
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_STEREO
        - #SRS_TRUMEDIA_WOWHD_SRSMODE_XTREME

        The values are a bitwise OR of any of these macros. When a bit
        corresponding to a particular macro is set, the processing block
        associated with that macro is enabled. */

   uint16_t             Gain_Input;
   /**< Specifies the gain (in Q15 format) applied to the signal before
        processing. */

   uint16_t             Gain_Output;
   /**< Specifies the gain (in Q15 format) applied to the signal after
        processing. */

   uint16_t             Gain_Bypass;
   /**< Inactive in this revision. */

   uint16_t             Focus_Level;
   /**< Specifies the level of focus effect (in Q15 format). */

   uint16_t             Def_Level;
   /**< Specifies the level of definition effect (in Q15 format). */

   uint16_t             Center_Level;
   /**< Specifies the SRS 3D Center control (in Q15 format). */

   uint16_t             Space_Level;
   /**< Specifies the SRS 3D Space control (in Q15 format). */

   uint16_t             Reserved;
   /**< This field must be set to zero. */

   uint32_t             Limit_Level;
   /**< Specifies the limiter's limit (in Q30 format). */

   uint16_t             TruBass_Level;
   /**< Specifies the level of TruBass effect (in Q15 format). */

   uint16_t             TruBass_Compress;
   /**< Specifies the TruBass compressor's level (in Q15 format). */

   uint32_t             TruBass_Coefs[16];
   /**< TruBass coefficients. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_srs_trumedia_wowhd_params_t */

/** @addtogroup audiopp_module_srs_trumedia
@{ */
/** ID of the CS Headphone configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.

    @msgpayload{srs_TruMedia_cshp_params_t}
    @table{weak__srs__trumedia__cshp__params__t}
*/
#define APPI_SRS_TRUMEDIA_CSHP_PARAMS                 0x10005013

/* The following are bit fields for the EnableFlags field in
 * srs_TruMedia_cshp_params_t defined below*/

/** Limiter is enabled. */
#define SRS_TRUMEDIA_CSHP_DOLIMIT       0x0001

/** Dialog enhancement is enabled. */
#define SRS_TRUMEDIA_CSHP_DODIALOG      0x0002

/** TruBass is enabled. */
#define SRS_TRUMEDIA_CSHP_DOTB          0x0004

/** Split-Analysis TruBass is active. */
#define SRS_TRUMEDIA_CSHP_DOSATB        0x0008

/** Circle Surround<sup>&reg;</sup> (CS) decoder is enabled. */
#define SRS_TRUMEDIA_CSHP_DODECODE      0x0010

/** CS decoding is active when set; passive matrix decoding when not set. */
#define SRS_TRUMEDIA_CSHP_DECODEMODE    0x0020

/** Definition is enabled. */
#define SRS_TRUMEDIA_CSHP_DODEF         0x0040

/** TruBass is stereo when set, and mono when not set. @newpage*/
#define SRS_TRUMEDIA_CSHP_TBMODE        0x0080

/** @} */ /* end_addtogroup audiopp_module_srs_trumedia */

typedef struct srs_TruMedia_cshp_params_t srs_TruMedia_cshp_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_srs_trumedia_cshp_params_t
@{ */
/* Payload of the APPI_SRS_TRUMEDIA_CSHP_PARAMS parameters used by
    the SRS TruMedia module.
*/
struct srs_TruMedia_cshp_params_t {
    uint32_t                EnableFlags;
   /**< Bitfield of macros used to specify how SRS TruMedia CSHP is
        enabled.

        @values
        - #SRS_TRUMEDIA_CSHP_DOLIMIT
        - #SRS_TRUMEDIA_CSHP_DODIALOG
        - #SRS_TRUMEDIA_CSHP_DOTB
        - #SRS_TRUMEDIA_CSHP_DOSATB
        - #SRS_TRUMEDIA_CSHP_DODECODE
        - #SRS_TRUMEDIA_CSHP_DECODEMODE
        - #SRS_TRUMEDIA_CSHP_DODEF
        - #SRS_TRUMEDIA_CSHP_TBMODE

        The values are a bitwise OR of any of these macros. When a bit
        corresponding to a particular macro is set, the processing block
        associated with that macro is enabled. */

   uint16_t             Gain_Input;
   /**< Specifies the gain (in Q15 format) applied to the signal before
        processing. */

   uint16_t             Gain_Output;
   /**< Specifies the gain (in Q15 format) applied to the signal after
        processing. */

   uint16_t             Gain_Bypass;
   /**< Currently not used. */

   uint16_t             Dialog_Level;
   /**< Specifies the level of dialog enhancement (in Q15 format). */

   uint16_t             Def_Level;
   /**< Specifies the level of definition effect (in Q15 format). */

   uint16_t             Reserved;
   /**< This field must be set to zero. */

   uint32_t             Limit_Level;
   /**< Specifies the limiter's limit (in Q30 format). */

   uint16_t             TruBass_Level;
   /**< Specifies the level of TruBass effect (in Q15 format). */

   uint16_t             TruBass_Compress;
   /**< Specifies the TruBass compressor's level (in Q15 format). */

   uint32_t             TruBass_Coefs[16];
   /**< TruBass coefficients. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_srs_trumedia_cshp_params_t */

/** @ingroup audiopp_module_srs_trumedia
    ID of the high pass filter configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.

    @msgpayload{srs_TruMedia_hpf_params_t}
    @table{weak__srs__trumedia__hpf__params__t}
*/
#define APPI_SRS_TRUMEDIA_HPF_PARAMS                  0x10005014

typedef struct srs_TruMedia_hpf_params_t srs_TruMedia_hpf_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_srs_trumedia_hpf_params_t
@{ */
/* Payload of the APPI_SRS_TRUMEDIA_HPF_PARAMS parameters used by the
    SRS TruMedia module.
*/
struct srs_TruMedia_hpf_params_t {
    uint32_t                Order;
   /**< Order of the filter. */

   uint32_t             Coefs[26];
   /**< High pass filter coefficients. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_srs_trumedia_hpf_params_t */

/** @addtogroup audiopp_module_srs_trumedia
@{ */
/** ID of the Parametric Equalizer (PEQ) configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.

    @msgpayload{srs_TruMedia_peq_params_t}
    @table{weak__srs__trumedia__peq__params__t}
*/
#define APPI_SRS_TRUMEDIA_PEQ_PARAMS                  0x10005015

/* The following are bit fields for the EnableFlags field in
 * srs_TruMedia_peq_params_t defined below*/

/** Left channel is enabled. */
#define SRS_TRUMEDIA_TRUEQ_DOL        0x0001

/** Right channel is enabled. */
#define SRS_TRUMEDIA_TRUEQ_DOR        0x0002

/** Left channel (band 0) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_LB0        0x0010

/** Left channel (band 1) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_LB1        0x0020

/** Left channel (band 2) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_LB2        0x0040

/** Left channel (band 3) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_LB3        0x0080

/** Right channel (band 0) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_RB0        0x0100

/** Right channel (band 1) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_RB1        0x0200

/** Right channel (band 2) is enabled. */
#define SRS_TRUMEDIA_TRUEQ_RB2        0x0400

/** Right Channel (band 3) is enabled. @newpage */
#define SRS_TRUMEDIA_TRUEQ_RB3        0x0800

/** @} */ /* end_addtogroup audiopp_module_srs_trumedia */

typedef struct srs_TruMedia_peq_params_t srs_TruMedia_peq_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_srs_trumedia_peq_params_t
@{ */
/* Payload of the APPI_SRS_TRUMEDIA_PEQ_PARAMS parameters used by the
    SRS TruMedia module.
*/
struct srs_TruMedia_peq_params_t {
   uint32_t             EnableFlags;
   /**< Bitfield of macros used to enable the SRS TruMedia PEQ parameters.

        @values
        - #SRS_TRUMEDIA_TRUEQ_DOL
        - #SRS_TRUMEDIA_TRUEQ_DOR
        - #SRS_TRUMEDIA_TRUEQ_LB0
        - #SRS_TRUMEDIA_TRUEQ_LB1
        - #SRS_TRUMEDIA_TRUEQ_LB2
        - #SRS_TRUMEDIA_TRUEQ_LB3
        - #SRS_TRUMEDIA_TRUEQ_RB0
        - #SRS_TRUMEDIA_TRUEQ_RB1
        - #SRS_TRUMEDIA_TRUEQ_RB2
        - #SRS_TRUMEDIA_TRUEQ_RB3

        The values are a bitwise OR of any of these macros. When a bit
        corresponding to a particular macro is set, the processing block
        associated with that macro is enabled. */

   uint16_t             Gain_Input;
   /**< Specifies the gain (in Q12 format) applied to the signal before
        processing. */

   uint16_t             Gain_Output;
   /**< Specifies the gain (in Q12 format) applied to the signal after
        processing. */

   uint16_t             Gain_Bypass;
   /**< Currently not used. */

   uint16_t             Reserved;
   /**< This field must be set to zero. */

   uint32_t             L_Coefs[26];
   /**< Left-channel PEQ coefficients. */

   uint32_t             R_Coefs[26];
   /**< Left-channel PEQ coefficients. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_srs_trumedia_peq_params_t */

/** @ingroup audiopp_module_srs_trumedia
    ID of the hard limiter configuration parameters used by
    #APPI_SRS_TRUMEDIA_MODULE_ID.

    @msgpayload{srs_TruMedia_hl_params_t}
    @table{weak__srs__trumedia__hl__params__t}
*/
#define APPI_SRS_TRUMEDIA_HL_PARAMS                   0x10005016

typedef struct srs_TruMedia_hl_params_t srs_TruMedia_hl_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_srs_trumedia_hl_params_t
@{ */
/* Payload of the APPI_SRS_TRUMEDIA_HL_PARAMS parameters used by the
    SRS TruMedia module.
*/
struct srs_TruMedia_hl_params_t {
   uint16_t                Gain_Input;
   /**< Specifies the gain (in Q13 format) applied to the signal before
        processing. */

   uint16_t             Gain_Output;
   /**< Specifies the gain (in Q13 format) applied to the signal after
        processing. */

   uint16_t             Gain_Bypass;
   /**< Currently not used. */

   uint16_t             Reserved_1;
   /**< This field must be set to zero. */

   int32_t              Delay_Len;
   /**< Number of samples in the delay buffer. */

   uint32_t             Boost;
   /**< Specifies the gain (in Q23 format) applied to the signal before
        limiting. */

   uint16_t             Limit;
   /**< Specifies the limiter's limit (in Q15 format). */

   uint16_t             Reserved_2;
   /**< This field must be set to zero. */

   uint32_t             DecaySmooth;
   /**< When nonzero, decay smoothing is enabled. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_srs_trumedia_hl_params_t */
/** @cond OEM_only */
/** @ingroup audio_pp_topos
    ID of the COPP topology for the SRS SS3D module.
*/
#define ADM_CMD_COPP_OPEN_TOPOLOGY_ID_SRS_SS3D_TOPOLOGY  0x00010720  /* temporary define */

/** @addtogroup audio_pp_module_ss3d
@{ */
/** ID of the SRS Studio Sound 3D (SS3D) module, currently known as DTS Studio
    Sound@tmns.
    This module supports the following parameter IDs:

    - #APPI_SRS_SS3D_GLOBAL_PARAMS
    - #APPI_SRS_SS3D_CTRL_PARAMS
    - #APPI_SRS_SS3D_FILTER_PARAMS
*/
#define APPI_SRS_SS3D_MODULE_ID             0x10005020

/** ID of the SRS SS3D parameters used by #APPI_SRS_SS3D_MODULE_ID to enable
    or disable SRS SS3D.

    @msgpayload{srs_ss3d_globals_t}
    @tablens{weak__srs__ss3d__globals__t}
*/
#define APPI_SRS_SS3D_GLOBAL_PARAMS         0x10005021

/** @} */ /* end_addtogroup audio_pp_module_ss3d */

/* Non-IP enable/disable flags used by the APPI_SRS_SS3D_PARAMS command */
typedef struct srs_ss3d_globals_t srs_ss3d_globals_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_srs_ss3d_globals_t
@{ */
/* Payload of the APPI_SRS_SS3D_GLOBAL_PARAMS parameters used by the SRS
    SS3D module. */
/** This structure also contains a payload used to verify the SRS Licensing
    scheme with the aDSP module. Do not fill this structure directly.
*/
struct srs_ss3d_globals_t {
    uint8_t                  Enable_Flags;
   /**< Bitfield of macros used to enable SRS SS3D.

        @values
        - #SRS_ENABLE_GLOBAL
        - #SRS_ENABLE_WOWHD
        - #SRS_ENABLE_CSHP
        - #SRS_ENABLE_HPF
        - #SRS_ENABLE_PEQ
        - #SRS_ENABLE_HL

        The values are a bitwise OR of any of these macros. When a bit
        corresponding to a particular macro is set, the processing block
        associated with that macro is enabled.

        The bit corresponding to #SRS_ENABLE_GLOBAL must be set if any of the
        other bits in the bitfield are to be set. */

    uint8_t                  Reserved;
    /**< Clients must set this field to zero. */

    uint8_t                  I_0;
   /**< SRS Licensing payload -- DO NOT MODIFY. */

    uint8_t                  V_0;
   /**< SRS Licensing payload -- DO NOT MODIFY. */

    uint8_t                  I_1;
   /**< SRS Licensing payload -- DO NOT MODIFY. */

    uint8_t                  V_1;
   /**< SRS Licensing payload -- DO NOT MODIFY. */

    uint8_t                  I_2;
   /**< SRS Licensing payload -- DO NOT MODIFY. */

    uint8_t                  V_2;
   /**< SRS Licensing payload -- DO NOT MODIFY. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_srs_ss3d_globals_t */

/** @ingroup audio_pp_module_ss3d
    ID of the SRS SS3D control parameters used by #APPI_SRS_SS3D_MODULE_ID.

    @msgpayload{srs_ss3d_ctrls_t}
    @table{weak__srs__ss3d__ctrls__t}
*/
#define APPI_SRS_SS3D_CTRL_PARAMS           0x10005022

/* SS3D ctrl paramater block
    This structure is used to deliver ctrl parameters to Studio Sound 3D
*/
typedef struct srs_ss3d_ctrls_t srs_ss3d_ctrls_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_srs_ss3d_ctrls_t
@{ */
/* Payload of the APPI_SRS_SS3D_CTRL_PARAMS parameters used by the SRS
    SS3D module.
*/
struct srs_ss3d_ctrls_t {
   int32_t          Enable_Flags;
   /**< Bitfield of flags/macros used to enable the control parameters. */

   int32_t           MainTech;
   /**< Main tech. */

   int16_t           HeadroomGain;
   /**< Headroom gain. */

   int16_t           BypassGain;
   /**< Bypass gain. */

   int16_t           AEQ_InputGain;
   /**< AEQ input gain. */

   int16_t           AEQ_BypassGain;
   /**< AEQ bypass gain. */

   int32_t           HPF_Order;
   /**< HPF filter order. */

   int32_t           HPF_Coefs[32];
   /**< Array of HPF coefficients. */

   int16_t           TBHD_RefOffset;
   /**< TBHD reference offset. */

   int16_t           TBHD_Level;
   /**< TBHD level. */

   int16_t           TBHD_SpkFreq;
   /**< TBHD speaker frequency. */

   int16_t           TBHD_AnaFreq;
   /**< TBHD analysis frequency. */

   int16_t           TBHD_Mode;
   /**< TBHD mode. */

   int16_t           TBHD_CompressorLevel;
   /**< TBHD compressor level. */

   int16_t           TBHD_TbQ;
   /**< TBHD TbQ. */

   int16_t           Def_Level;
   /**< TBHD definition level. */

   int16_t           TVol_LKFS;
   /**< TVol LKFS. */

   int16_t           TDiag_ClarityGain;
   /**< TDiag clarity gain. */

   int16_t           Share_DlgClarityLevel;
   /**< TDiag clarity level. */

   int16_t           Share_TruSurLevel;
   /**< TruSurround level. */

   int16_t           Share_Srs3DSpaceLevel;
   /**< 3D space level. */

   int16_t           Share_Srs3DCenterLevel;
   /**< 3D center level. */

   int16_t           Share_FocusLevel;
   /**< Focus level. */

   int16_t           Share_SpaceHPFCutoffFreq;
   /**< HPF space cutoff frequency. */

   int16_t           CSD_Mode;
   /**< CSD mode. */

   int16_t           GEQ_InputGain;
   /**< GEQ input gain. */

   int16_t           GEQ_Gains[5];
   /**< Array of GEQ gains. */

   int16_t           Gain_WOWHD;
   /**< WOWHD gain. */

   int16_t           Gain_TSHD;
   /**< TSHD gain. */

   int16_t           Gain_CC3D;
   /**< CC3D gain. */

   int16_t           Gain_CSHP;
   /**< CSHP gain. */

   int16_t           Gain_TBHD;
   /**< TBHD gain. */

   int16_t           HL_OutputGain;
   /**< HL output gain. */

   int16_t           Tone_InputGain;
   /**< Tone input gain. */

   int32_t           TVol_LevelIntensity;
   /**< TVol level intensity. */

   int32_t           TDiag_ProcessGain;
   /**< TDiag process gain. */

   int32_t           MVol_VolStep;
   /**< MVol volume step. */

   int32_t           MVol_LRBalance;
   /**< MVol left/right balance. */

   int32_t           HL_BoostOffset;
   /**< HL boost offset. */

   int32_t           Tone_BassGain;
   /**< Tone bass gain. */

   int32_t           Tone_TrebleGain;
   /**< Tone treble gain. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_srs_ss3d_ctrls_t */

/** @ingroup audio_pp_module_ss3d
    ID of the SRS SS3D filter parameters used by #APPI_SRS_SS3D_MODULE_ID.

    @msgpayload{srs_ss3d_filters_t}
    @table{weak__srs__ss3d__filters__t}
*/
#define APPI_SRS_SS3D_FILTER_PARAMS         0x10005023

/* SS3D filter paramater block
    This structure is used to deliver filters to Studio Sound 3D */
typedef struct srs_ss3d_filters_t srs_ss3d_filters_t;

#include "adsp_begin_pack.h"

/** @ingroup audio_pp_module_ss3d
    Maximum size of the SS3D filter.
*/
#define SRS_SS3D_FILTER_MAX_SIZE 2752

/** @weakgroup weak_srs_ss3d_filters_t
@{ */
/* Payload of the APPI_SRS_SS3D_FILTER_PARAMS parameters used by the SRS
    SS3D module.
*/
struct srs_ss3d_filters_t {
    int32_t         HpfOrder;
   /**< HPF filter order. */

    int32_t       HpfWl;
   /**< HPF filter coefficient word length. */

    int32_t         AEQIirOrderL;
   /**< AEQ filter order for IIR, left channel. */

    int32_t         AEQFirOrderL;
   /**< AEQ filter order for FIR, left channel. */

    int32_t         AEQIirOrderR;
   /**< AEQ filter order for IIR, right channel. */

    int32_t         AEQFirOrderR;
   /**< AEQ filter order for FIR, right channel. */

    int32_t         TBCustom;
   /**< Specifies whether custom TB coefficients are included (TRUE or FALSE). */

#ifdef WIN32
    int8_t          *data;
#else
    int8_t          data[SRS_SS3D_FILTER_MAX_SIZE];
#endif
   /**< All filter coefficients. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_srs_ss3d_filters_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_dts_dialnorm
@{ */
/** ID of the DTS DialNorm module. */
#define AUDPROC_MODULE_ID_DTS_DIALNORM    0x0001071B

/** ID of the DTS DialNorm Enable parameter used by
    AUDPROC_MODULE_ID_DTS_DIALNORM.

    @msgpayload{audproc_dts_dialnorm_enable_t}
    @table{weak__audproc__dts__dialnorm__enable__t}
*/
#define AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE    0x0001071C

/** @} */ /* end_addtogroup audiopp_module_dts_dialnorm */

typedef struct audproc_dts_dialnorm_enable_t audproc_dts_dialnorm_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_dts_dialnorm_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_DTS_DIALNORM_ENABLE enable_flag parameter
    used by the DialNorm module.
*/
struct audproc_dts_dialnorm_enable_t
{

    uint32_t                  enable_flag;
    /**< Specifies whether DTS DialNorm is disabled (0, the default) or
         enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_dts_dialnorm_enable_t */

/** @addtogroup audiopp_module_dts_drc
@{ */
/** ID of the DTS DRC module. */
#define AUDPROC_MODULE_ID_DTS_DRC    0x0001071D

/** ID of the DTS DRC Ratio parameter used by AUDPROC_MODULE_ID_DTS_DRC.

    @msgpayload{audproc_dts_drc_param_t}
    @table{weak__audproc__dts__drc__param__t}
*/
#define AUDPROC_PARAM_ID_DTS_DRC_RATIO    0x0001071E

/** @} */ /* end_addtogroup audiopp_module_dts_drc */

typedef struct audproc_dts_drc_param_t audproc_dts_drc_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_dts_drc_param_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_DTS_DRC_RATIO parameter used by the
    DRC module.
*/
struct audproc_dts_drc_param_t
{

    uint32_t                  drc_ratio;
    /**< Specifies the DTS DRC_ratio used by the DRC.

         @values 0  to 100

         If this value is greater than 4, the DRC is enabled; otherwise, it is
         bypassed. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_dts_drc_param_t */
/** @endcond */

/** @addtogroup audiopp_module_resampler
@{ */
/** ID of the Resampler module. */
#define AUDPROC_MODULE_ID_RESAMPLER   0x00010719

/** ID of the Enable Low Degradation Resampler parameter used by
    AUDPROC_MODULE_ID_RESAMPLER.

    @note1hang Although the parameter ID indicates HIGH_THD, it is actually
               defining the low threshold.

    @msgpayload{audproc_high_thd_resamp_enable_t}
    @table{weak__audproc__high__thd__resamp__enable__t}
*/
#define AUDPROC_PARAM_ID_ENABLE_HIGH_THD_RESAMPLER 0x0001071A

/** @} */ /* end_addtogroup audiopp_module_resampler */

/* Structure for enabling the configuration parameter for high thd resampler. */
typedef struct audproc_high_thd_resamp_enable_t audproc_high_thd_resamp_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_high_thd_resamp_enable_t
@{ */
/* Payload of the AUDPROC_MODULE_ID_RESAMPLER enable_flag parameter used by
    the Resampler module.
*/
struct audproc_high_thd_resamp_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the resampler is disabled (0) or enabled (1).

         This resampler is applicable only for the input sampling rate of
         44.1 kHz and output sampling rate of 48 kHz. It is not supported for
         the other sampling rates.

         If this resampler is enabled and input sampling and output sampling
         rates are not 44.1 kHz and 48 kHz respectively, the generic resampler
         is used.

         If this resampler is disabled, the generic resampler is used for all
         sampling rate conversions. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_high_thd_resamp_enable_t  */

/** @cond OEM_only */
/** @addtogroup audiopp_module_downmixer
@{ */
/** ID for the Downmixer module. This module does not have any
    parameters.
*/
#define AUDPROC_MODULE_ID_DOWNMIX     0x00010721

/** ID for the Sample Slipping/stuffing module. This module does
    not have any parameters.
*/
#define AUDPROC_MODULE_ID_SAMPLESLIP  0x00010722

/** @} */ /* end_addtogroup audiopp_module_downmixer */
/** @endcond */

typedef struct audproc_module_info_t audproc_module_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_module_info_t
@{ */
/* Defines the modules used by the ASM_CMD_ADD_TOPOLOGIES and
    ADM_CMD_ADD_TOPOLOGIES_V5 commands.
*/
struct audproc_module_info_t
{
   uint32_t module_id;
   /**< ID of the module to be created.

        @values Any valid module ID */

   uint32_t use_lpm;
   /**< Specifies whether to put this module in low power memory.

        @values
        - 0 -- Do not put this module in low power memory
        - 1 -- Put this module in low power memory

        If there is insufficient low power memory, this module is placed in
        the default memory pool.*/

   uint8_t  init_params[16];
   /**< Array of initialization parameters passed to the module when it is
        created.

        This array of 16 bytes is sent to the getsize() and init() methods of
        the module. The interpretation of this information is module specific. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_module_info_t */

typedef struct audproc_topology_definition_t audproc_topology_definition_t;

#include "adsp_begin_pack.h"

/** @ingroup asmsvc_cmd_add_topos
    Maximum number of modules supported in a custom topology.
*/
#define AUDPROC_MAX_MODULES_PER_TOPOLOGY 16

/** @weakgroup weak_audproc_topology_definition_t
@{ */
/* Defines a custom topology that is to be added to the aDSP. This structure
    is followed by num_modules of audproc_module_info_t.
*/
struct audproc_topology_definition_t
{
   uint32_t topology_id;
   /**< ID of this custom topology.

        @values 0x10000000-0x1FFFFFFF */

   uint32_t num_modules;
   /**< Number of modules in this topology.

        @values 0 to #AUDPROC_MAX_MODULES_PER_TOPOLOGY */

   audproc_module_info_t module_info[AUDPROC_MAX_MODULES_PER_TOPOLOGY];
   /**< Array of module information structures with 16 elements.

        The order of the elements in this array specify the order in which the
        modules will be connected in the topology, from source to sink. Only
        the first num_modules elements of this array are required. The rest of
        the elements are ignored. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_topology_definition_t */

typedef struct audproc_custom_topologies_t audproc_custom_topologies_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_custom_topologies_t
@{ */
/* Header structure for the custom topologies command buffer. This structure
    is followed by num_topologies of audproc_topology_definition_t.
*/
struct audproc_custom_topologies_t
{
   uint32_t num_topologies;
   /**< Specifies the number of topologies in this command.

        @values Any nonzero number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_custom_topologies_t */

/** @cond OEM_only */
/** @addtogroup audiopp_module_compr_latency
@{ */
/** ID of the Latency module for the compressed data path.

    The Latency module for compressed data introduces non-negative delays in
    the path. This achieved by introducing IEC61937 pause bursts for the
    specified delay. When the delay is decreased, data is dropped.

    Because the minimum pause burst duration that is possible depends on the
    format of the compressed data, the minimum delay possible also varies
    according to the format. For AC3/EAC3, the minimum is three samples duration.
*/
#define AUDPROC_MODULE_ID_COMPRESSED_LATENCY            0x0001076E

/** ID of the Delay parameter used by AUDPROC_MODULE_ID_COMPRESSED_LATENCY.

    @msgpayload{audproc_comp_latency_param_t}
    @table{weak__audproc__comp__latency__param__t}
*/
#define AUDPROC_PARAM_ID_COMPRESSED_LATENCY             0x0001076F

/** @} */ /* end_addtogroup audiopp_module_compr_latency */

/* Structure for latency parameter in compressed data paths. */
typedef struct audproc_compressed_latency_param_t audproc_compressed_latency_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_comp_latency_param_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_COMPRESSED_LATENCY parameter used by
    AUDPROC_MODULE_ID_COMPRESSED_LATENCY.
*/
struct audproc_compressed_latency_param_t
{
    uint32_t                  delay_us;
    /**< Delay in microseconds.

         @values 0 to 100000

         This value is the amount of delay introduced by the Latency module.
         It must be greater than zero. If the value is zero, this module is
         disabled.

         The actual resolution of the delay value depends on the compressed
         data format. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_comp_latency_param_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_compr_mute
@{ */
/** ID of the Compressed Mute module. */
#define AUDPROC_MODULE_ID_COMPRESSED_MUTE    0x00010770

/** ID of the Compressed Mute parameter used by
    AUDPROC_MODULE_ID_COMPRESSED_MUTE.

    @msgpayload{audproc_compressed_mute_param_t}
    @table{weak__audproc__compressed__mute__param__t}
*/
#define AUDPROC_PARAM_ID_COMPRESSED_MUTE     0x00010771

/** @} */ /* end_addtogroup audiopp_module_compr_mute */

/* Structure for Compressed Mute parameter muteFlag. */
typedef struct audproc_compressed_mute_param_t audproc_compressed_mute_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_compressed_mute_param_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_COMPRESSED_MUTE muteFlag parameter used by
    AUDPROC_MODULE_ID_COMPRESSED_MUTE.
*/
struct audproc_compressed_mute_param_t
{
    uint32_t                  mute_flag;
    /**< Specifies whether compressed mute is enabled.

         @values 0 to 4294967295
         - 0 -- Disables mute (Default)
         - Any nonzero value -- Enables mute */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak__audproc_compressed_mute_param_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_delay_latency
@{ */
/** ID of the Delay/Latency module on the LPCM data path.

    This module introduces the specified amount of delay in the audio path.
    If the delay is increased, silence is inserted. If the delay is decreased,
    data is dropped.

    There are no smooth transitions. The resolution of the delay applied is
    limited by the period of a single sample. Qualcomm recommends muting the
    device path when the delay is changed (to avoid glitches).
*/
#define AUDPROC_MODULE_ID_DELAY                    0x00010772

/** ID of the Delay parameter used by AUDPROC_MODULE_ID_DELAY.

    @msgpayload{audproc_delay_param_t}
    @table{weak__audproc__delay__param__t}
*/
#define AUDPROC_PARAM_ID_DELAY                     0x00010773

/** @} */ /* end_addtogroup audiopp_module_delay_latency */

/* Structure for delay parameter in LPCM data paths. */
typedef struct audproc_delay_param_t audproc_delay_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_delay_param_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_DELAY parameter used by
    AUDPROC_MODULE_ID_DELAY.
*/
struct audproc_delay_param_t
{
    uint32_t                  delay_us;
    /**< Delay in micro seconds.

         @values 0 to 100000

         The amount of delay must be greater than zero.  If the value is
         zero, this module is disabled.

         The actual resolution of the delay is limited by the period of a
         single audio sample. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_delay_param_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_bass_boost
@{ */
/** ID of the Bass Boost module.
    This module supports the following parameter IDs:
    - #AUDPROC_PARAM_ID_BASS_BOOST_ENABLE
    - #AUDPROC_PARAM_ID_BASS_BOOST_MODE
    - #AUDPROC_PARAM_ID_BASS_BOOST_STRENGTH
*/
#define AUDPROC_MODULE_ID_BASS_BOOST                             0x000108A1

/** ID of the Bass Boost Enable parameter used by
    AUDPROC_MODULE_ID_BASS_BOOST.

    @msgpayload{audproc_bass_boost_enable_t}
    @table{weak__audproc__bass__boost__enable__t}
*/
#define AUDPROC_PARAM_ID_BASS_BOOST_ENABLE                       0x000108A2

/** ID of the Bass Boost Mode parameter used by
    AUDPROC_MODULE_ID_BASS_BOOST.

    @msgpayload{audproc_bass_boost_mode_t}
    @table{weak__audproc__bass__boost__mode__t} @newpage
*/
#define AUDPROC_PARAM_ID_BASS_BOOST_MODE                         0x000108A3

/** ID of the Bass Boost Strength parameter used by
    #AUDPROC_MODULE_ID_BASS_BOOST.

    @msgpayload{audproc_bass_boost_strength_t}
    @table{weak__audproc__bass__boost__strength__t}
*/
#define AUDPROC_PARAM_ID_BASS_BOOST_STRENGTH                     0x000108A4

/** @} */ /* end_addtogroup audiopp_module_bass_boost */

/* Structure for the enable parameter of Bass Boost module. */
typedef struct audproc_bass_boost_enable_t audproc_bass_boost_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_bass_boost_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_BASS_BOOST_ENABLE parameter used by the
    Bass Boost module.
*/
struct audproc_bass_boost_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether Bass Boost is disabled (0) or enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_bass_boost_enable_t */

/* Structure for the mode parameter of Bass Boost module. */
typedef struct audproc_bass_boost_mode_t audproc_bass_boost_mode_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_bass_boost_mode_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_BASS_BOOST_MODE parameter used by the
    Bass Boost module.
*/
struct audproc_bass_boost_mode_t
{
    uint32_t                  bass_boost_mode;
    /**< Specifies the Bass Boost mode.

         @values
         - 0 -- Physical boost; used by the headphone
         - 1 -- Virtual boost; used by small speakers

         Currently, only Physical Boost mode (0) is supported. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_bass_boost_mode_t */

/* Structure for the strength parameter of Bass Boost module. */
typedef struct audproc_bass_boost_strength_t audproc_bass_boost_strength_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_bass_boost_strength_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_BASS_BOOST_STRENGTH parameter used by the
    Bass Boost module.
*/
struct audproc_bass_boost_strength_t
{
    uint32_t                  strength;
    /**< Specifies the effects of bass boost.

         @values 0 to 1000, where:
         - 0 -- No effects
         - 1000 -- Maximum effects */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_bass_boost_strength_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_virtualizer
@{ */
/** ID of the Virtualizer module. This module supports the
    following parameter IDs:
    - #AUDPROC_PARAM_ID_VIRTUALIZER_ENABLE
    - #AUDPROC_PARAM_ID_VIRTUALIZER_STRENGTH
    - #AUDPROC_PARAM_ID_VIRTUALIZER_OUT_TYPE
    - #AUDPROC_PARAM_ID_VIRTUALIZER_GAIN_ADJUST
*/
#define AUDPROC_MODULE_ID_VIRTUALIZER                            0x000108A5

/** ID of the Virtualizer Enable parameter used by
    AUDPROC_MODULE_ID_VIRTUALIZER.

    @msgpayload{audproc_virtualizer_enable_t}
    @table{weak__audproc__virtualizer__enable__t}
*/
#define AUDPROC_PARAM_ID_VIRTUALIZER_ENABLE                      0x000108A6

/** ID of the Virtualizer Strength parameter used by
    AUDPROC_MODULE_ID_VIRTUALIZER.

    @msgpayload{audproc_virtualizer_strength_t}
    @table{weak__audproc__virtualizer__strength__t} @newpage
*/
#define AUDPROC_PARAM_ID_VIRTUALIZER_STRENGTH                    0x000108A7

/** ID of the Virtualizer Out Type parameter used by
    #AUDPROC_MODULE_ID_VIRTUALIZER.

    @msgpayload{audproc_virtualizer_out_type_t}
    @table{weak__audproc__virtualizer__out__type__t}
*/
#define AUDPROC_PARAM_ID_VIRTUALIZER_OUT_TYPE                    0x000108A8

/** ID of the Virtualizer Gain Adjust parameter used by
    #AUDPROC_MODULE_ID_VIRTUALIZER.

    @msgpayload{audproc_virtualizer_gain_adjust_t}
    @table{weak__audproc__virtualizer__gain__adjust__t}
*/
#define AUDPROC_PARAM_ID_VIRTUALIZER_GAIN_ADJUST                 0x000108A9

/** @} */ /* end_addtogroup audiopp_module_virtualizer */

/* Structure for the enable parameter of Virtualizer module. */
typedef struct audproc_virtualizer_enable_t audproc_virtualizer_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_virtualizer_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_VIRTUALIZER_ENABLE parameter used by the
    Virtualizer module.
*/
struct audproc_virtualizer_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether the virtualizer is disabled (0) or enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_virtualizer_enable_t */

/* Structure for the strength parameter of VIRTUALIZER module. */
typedef struct audproc_virtualizer_strength_t audproc_virtualizer_strength_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_virtualizer_strength_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_VIRTUALIZER_STRENGTH parameter used by the
    Virtualizer module.
*/
struct audproc_virtualizer_strength_t
{
    uint32_t                  strength;
    /**< Specifies the effects of the virtualizer.

         @values 0 to 1000, where:
         - 0 -- No effects
         - 1000 -- Maximum effects */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_virtualizer_strength_t */

/* Structure for the strength parameter of VIRTUALIZER module. */
typedef struct audproc_virtualizer_out_type_t audproc_virtualizer_out_type_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_virtualizer_out_type_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_VIRTUALIZER_OUT_TYPE parameter used by the
    Virtualizer module.
*/
struct audproc_virtualizer_out_type_t
{
    uint32_t                  out_type;
    /**< Specifies the output device type of the virtualizer.

         @values
         - 0 -- Headphone
         - 1 -- Desktop speakers */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_virtualizer_out_type_t */

/* Structure for the strength parameter of VIRTUALIZER module. */
typedef struct audproc_virtualizer_gain_adjust_t audproc_virtualizer_gain_adjust_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_virtualizer_gain_adjust_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_VIRTUALIZER_GAIN_ADJUST parameter used by the
    Virtualizer module.
*/
struct audproc_virtualizer_gain_adjust_t
{
    int32_t                  gain_adjust;
    /**< Specifies the overall gain adjustment of virtualizer outputs.

         @values -600 to +600 millibels */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_virtualizer_gain_adjust_t */
/** @endcond */

/** @cond OEM_only */
/** @addtogroup audiopp_module_reverb
@{ */
/** ID of the Reverberation module. This module supports the following
    parameter IDs:
    - #AUDPROC_PARAM_ID_REVERB_ENABLE
    - #AUDPROC_PARAM_ID_REVERB_MODE
    - #AUDPROC_PARAM_ID_REVERB_PRESET
    - #AUDPROC_PARAM_ID_REVERB_WET_MIX
    - #AUDPROC_PARAM_ID_REVERB_GAIN_ADJUST
    - #AUDPROC_PARAM_ID_REVERB_ROOM_LEVEL
    - #AUDPROC_PARAM_ID_REVERB_ROOM_HF_LEVEL
    - #AUDPROC_PARAM_ID_REVERB_DECAY_TIME
    - #AUDPROC_PARAM_ID_REVERB_DECAY_HF_RATIO
    - #AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL
    - #AUDPROC_PARAM_ID_REVERB_REFLECTIONS_DELAY
    - #AUDPROC_PARAM_ID_REVERB_LEVEL
    - #AUDPROC_PARAM_ID_REVERB_DELAY
    - #AUDPROC_PARAM_ID_REVERB_DIFFUSION
    - #AUDPROC_PARAM_ID_REVERB_DENSITY
*/
#define AUDPROC_MODULE_ID_REVERB                          0x000108AA

/** ID of the Reverb Enable parameter used by AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_enable_t}
    @table{weak__audproc__reverb__enable__t} @newpage
*/
#define AUDPROC_PARAM_ID_REVERB_ENABLE                    0x000108AB

/** ID of the Reverb Mode parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_mode_t}
    @table{weak__audproc__reverb__mode__t}
*/
#define AUDPROC_PARAM_ID_REVERB_MODE                      0x000108AC

/** ID of the Reverb Preset parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_preset_t}
    @table{weak__audproc__reverb__preset__t}
*/
#define AUDPROC_PARAM_ID_REVERB_PRESET                    0x000108AD

/** ID of the Reverb Wet Mix parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_wet_mix_t}
    @table{weak__audproc__reverb__wet__mix__t} @newpage
*/
#define AUDPROC_PARAM_ID_REVERB_WET_MIX                   0x000108AE

/** ID of the Reverb Gain Adjust parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_room_level_t}
    @table{weak__audproc__reverb__room__level__t}
*/
#define AUDPROC_PARAM_ID_REVERB_GAIN_ADJUST               0x000108AF

/** ID of the Reverb Room Level parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_room_level_t}
    @table{weak__audproc__reverb__room__level__t}
*/
#define AUDPROC_PARAM_ID_REVERB_ROOM_LEVEL                0x000108B0

/** ID of the Reverb Room High Frequency Level parameter used by
    #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_room_hf_level_t}
    @table{weak__audproc__reverb__room__hf__level__t}
*/
#define AUDPROC_PARAM_ID_REVERB_ROOM_HF_LEVEL             0x000108B1

/** ID of the Reverb Decay Time parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_decay_time_t}
    @table{weak__audproc__reverb__decay__time__t} @newpage
*/
#define AUDPROC_PARAM_ID_REVERB_DECAY_TIME                0x000108B2

/** ID of the Reverb Decay High Frequency Ratio parameter used by
    #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_decay_hf_ratio_t}
    @table{weak__audproc__reverb__decay__hf__ratio__t}
*/
#define AUDPROC_PARAM_ID_REVERB_DECAY_HF_RATIO            0x000108B3

/** ID of the Reverb Reflections Level parameter used by
    #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_reflections_level_t}
    @table{weak__audproc__reverb__reflections__level__t}
*/
#define AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL         0x000108B4

/** ID of the Reverb Reflections Delay parameter used by
    #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_reflections_delay_t}
    @table{weak__audproc__reverb__reflections__delay__t}
*/
#define AUDPROC_PARAM_ID_REVERB_REFLECTIONS_DELAY         0x000108B5

/** ID of the Reverb Level parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_level_t}
    @table{weak__audproc__reverb__level__t} @newpage
*/
#define AUDPROC_PARAM_ID_REVERB_LEVEL                      0x000108B6

/** ID of the Reverb Delay parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_delay_t}
    @table{weak__audproc__reverb__delay__t}
*/
#define AUDPROC_PARAM_ID_REVERB_DELAY                      0x000108B7

/** ID of the Reverb Diffusion parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_diffusion_t}
    @table{weak__audproc__reverb__diffusion__t}
*/
#define AUDPROC_PARAM_ID_REVERB_DIFFUSION                  0x000108B8

/** ID of the Reverb Density parameter used by #AUDPROC_MODULE_ID_REVERB.

    @msgpayload{audproc_reverb_density_t}
    @table{weak__audproc__reverb__density__t}
*/
#define AUDPROC_PARAM_ID_REVERB_DENSITY                    0x000108B9

/** @} */ /* end_addtogroup audiopp_module_reverb */

/* Structure for the enable parameter of Reverb module. */
typedef struct audproc_reverb_enable_t audproc_reverb_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_ENABLE parameter used by the
    Reverb module.
*/
struct audproc_reverb_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether reverberation is disabled (0) or enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_enable_t */

/* Structure for the mode parameter of Reverb module. */
typedef struct audproc_reverb_mode_t audproc_reverb_mode_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_mode_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_MODE parameter used by the
    Reverb module.
*/
struct audproc_reverb_mode_t
{
    uint32_t                  reverb_mode;
    /**< Specifies the reverberation topology mode.

         @values
         0 -- Insert Effects. Reverberation is cascaded with other audio
              processing modules.
         1 -- Auxiliary Effects. Reverberation runs in parallel to other
              processing modules.

         The #AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_HEADPHONE and
         #AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_SPEAKER topologies support only
         Insert Effects mode.
         For Auxiliary Effect mode, a custom topology must be defined. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_mode_t */

/** @addtogroup audiopp_module_reverb
@{ */
/** User-customized preset (with environmental reverberation parameters
    specified individually).
*/
#define  AUDPROC_REVERB_PRESET_CUSTOM            0

/** Simulates an environment in a room. */
#define  AUDPROC_REVERB_PRESET_ROOM              1

/** Simulates an environment in a bathroom. */
#define  AUDPROC_REVERB_PRESET_BATHROOM          2

/** Simulates an environment in a concert hall. */
#define  AUDPROC_REVERB_PRESET_CONCERTHALL       3

/** Simulates an environment in a cave. */
#define  AUDPROC_REVERB_PRESET_CAVE              4

/** Simulates an environment in an arena. */
#define  AUDPROC_REVERB_PRESET_ARENA             5

/** Simulates an environment in a forest. */
#define  AUDPROC_REVERB_PRESET_FOREST            6

/** Simulates an environment in a city. */
#define  AUDPROC_REVERB_PRESET_CITY              7

/** Simulates an environment in the mountains (open air). */
#define  AUDPROC_REVERB_PRESET_MOUNTAINS         8

/** Simulates an environment under the water. */
#define  AUDPROC_REVERB_PRESET_UNDERWATER        9

/** Simulates an environment in an auditorium. */
#define  AUDPROC_REVERB_PRESET_AUDITORIUM        10

/** Simulates an environment in an alley. */
#define  AUDPROC_REVERB_PRESET_ALLEY             11

/** Simulates an environment in a hallway. */
#define  AUDPROC_REVERB_PRESET_HALLWAY           12

/** Simulates an environment in a hangar. */
#define  AUDPROC_REVERB_PRESET_HANGAR            13

/** Simulates an environment in a living room. */
#define  AUDPROC_REVERB_PRESET_LIVINGROOM        14

/** Simulates an environment in a small room. */
#define  AUDPROC_REVERB_PRESET_SMALLROOM         15

/** Simulates an environment in a medium-sized room. */
#define  AUDPROC_REVERB_PRESET_MEDIUMROOM        16

/** Simulates an environment in a large room. */
#define  AUDPROC_REVERB_PRESET_LARGEROOM         17

/** Simulates an environment in a medium-sized hall. */
#define  AUDPROC_REVERB_PRESET_MEDIUMHALL        18

/** Simulates an environment in a large hall. */
#define  AUDPROC_REVERB_PRESET_LARGEHALL         19

/** Simulates sound being sent to a metal plate, which vibrates back and forth.
    These vibrations are transformed into an audio signal.
*/
#define  AUDPROC_REVERB_PRESET_PLATE             20

/** Simulates a generic reverberation effect. */
#define  AUDPROC_REVERB_PRESET_GENERIC           21

/** Simulates an environment in a padded cell. */
#define  AUDPROC_REVERB_PRESET_PADDEDCELL        22

/** Simulates an environment in a stone room. */
#define  AUDPROC_REVERB_PRESET_STONEROOM         23

/** Simulates an environment in a carpeted hallway. */
#define  AUDPROC_REVERB_PRESET_CARPETEDHALLWAY   24

/** Simulates an environment in a stone corridor. */
#define  AUDPROC_REVERB_PRESET_STONECORRIDOR     25

/** Simulates an environment in a quarry. */
#define  AUDPROC_REVERB_PRESET_QUARRY            26

/** Simulates an environment on an open plain. */
#define  AUDPROC_REVERB_PRESET_PLAIN             27

/** Simulates an environment in a parking lot. */
#define  AUDPROC_REVERB_PRESET_PARKINGLOT        28

/** Simulates an environment in a sewer pipe. */
#define  AUDPROC_REVERB_PRESET_SEWERPIPE         29

/** Synthetic environment preset: drugged. */
#define  AUDPROC_REVERB_PRESET_DRUGGED           30

/** Synthetic environment preset: dizzy. */
#define  AUDPROC_REVERB_PRESET_DIZZY             31

/** Synthetic environment preset: psychotic. */
#define  AUDPROC_REVERB_PRESET_PSYCHOTIC         32

/** @} */ /* end_addtogroup audiopp_module_reverb */

/* Structure for the preset parameter of Reverb module. */
typedef struct audproc_reverb_preset_t audproc_reverb_preset_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_preset_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_PRESET parameter used by the
    Reverb module.
*/
struct audproc_reverb_preset_t
{
    uint32_t                  reverb_preset;
    /**< Specifies one of the reverberation presets that create special
         environmental audio effects.

         @values 0 to 32 (#AUDPROC_REVERB_PRESET_CUSTOM) to
                 (#AUDPROC_REVERB_PRESET_PSYCHOTIC) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_preset_t */

/* Structure for the wet mix parameter of Reverb module. */
typedef struct audproc_reverb_wet_mix_t audproc_reverb_wet_mix_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_wet_mix_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_WET_MIX parameter used by the
    Reverb module.
*/
struct audproc_reverb_wet_mix_t
{
    uint32_t                  reverb_wet_mix;
    /**< Specifies the reverberation wet/dry mix ratio for the Insert Effect mode.

         @values 0 to 1000

         This parameter has no effect if the Reverb mode is set to Auxiliary
         Effects. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_wet_mix_t */

/* Structure for the gain adjust parameter of Reverb module. */
typedef struct audproc_reverb_gain_adjust_t audproc_reverb_gain_adjust_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_gain_adjust_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_GAIN_ADJUST parameter used by the
    Reverb module.
*/
struct audproc_reverb_gain_adjust_t
{
    int32_t                  gain_adjust;
    /**< Specifies the overall gain adjustment of reverberation outputs.

         @values -600 to +600 millibels */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_gain_adjust_t */

/* Structure for the room level parameter of Reverb module. */
typedef struct audproc_reverb_room_level_t audproc_reverb_room_level_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_room_level_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_ROOM_LEVEL parameter used by the
    Reverb module.
*/
struct audproc_reverb_room_level_t
{
    int32_t                  room_level;
    /**< Specifies the master volume level of the environment reverberation effect.

         @values -9600 to 0 millibels */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_room_level_t */

/* Structure for the room hf level parameter of Reverb module. */
typedef struct audproc_reverb_room_hf_level_t audproc_reverb_room_hf_level_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_room_hf_level_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_ROOM_HF_LEVEL parameter used by the
    Reverb module.
*/
struct audproc_reverb_room_hf_level_t
{
    int32_t                  room_hf_level;
    /**< Specifies the volume level at 5 kHz relative to the volume level at
         low frequencies of the overall reverberation effect.

         @values -9600 to 0 millibels */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_room_hf_level_t */

/* Structure for the decay time parameter of Reverb module. */
typedef struct audproc_reverb_decay_time_t audproc_reverb_decay_time_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_decay_time_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_DECAY_TIME parameter used by the
    Reverb module.
*/
struct audproc_reverb_decay_time_t
{
    uint32_t                  decay_time;
    /**< Specifies the time taken for the level of reverberation to decay by
         60 dB.

         @values 100 to 20000 milliseconds */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_decay_time_t */

/* Structure for the decay hf ratio parameter of Reverb module. */
typedef struct audproc_reverb_decay_hf_ratio_t audproc_reverb_decay_hf_ratio_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_decay_hf_ratio_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_DECAY_HF_RATIO parameter used by the
    Reverb module.
*/
struct audproc_reverb_decay_hf_ratio_t
{
    uint32_t                  decay_hf_ratio;
    /**< Specifies the ratio of high frequency decay time (at 5 kHz) relative
         to the decay time at low frequencies.

         @values 100 to 2000 milliseconds */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_decay_hf_ratio_t */

/* Structure for the reverb level parameter of Reverb module. */
typedef struct audproc_reverb_level_t audproc_reverb_level_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_level_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_LEVEL parameter used by the
    Reverb module.
*/
struct audproc_reverb_level_t
{
    int32_t                  reverb_level;
    /**< Specifies the volume level of the late reverberation.

         @values -9600 to 2000 millibels */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_level_t */

/* Structure for the reverb delay parameter of Reverb module. */
typedef struct audproc_reverb_delay_t audproc_reverb_delay_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_delay_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_DELAY parameter used by the
    Reverb module.
*/
struct audproc_reverb_delay_t
{
    uint32_t                  reverb_delay;
    /**< Specifies the time between the first reflection and the late
         reverberation.

         @values 0 to 100 milliseconds */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_delay_t */

/* Structure for the reverb reflections level parameter of Reverb module. */
typedef struct audproc_reverb_reflections_level_t audproc_reverb_reflections_level_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_reflections_level_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL parameter used by the
    Reverb module.
*/
struct audproc_reverb_reflections_level_t
{
    int32_t                  reflections_level;
    /**< Specifies the volume level of the early reflections.

         @values -9600 to +1000 millibels */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_reflections_level_t */

/* Structure for the reverb reflections delay parameter of Reverb module. */
typedef struct audproc_reverb_reflections_delay_t audproc_reverb_reflections_delay_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_reflections_delay_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_REFLECTIONS_DELAY parameter used by the
    Reverb module.
*/
struct audproc_reverb_reflections_delay_t
{
    uint32_t                  reflections_delay;
    /**< Specifies the time between the first reflection and the late
         reverberation ().

         @values 0 to +300 milliseconds */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_reflections_delay_t */

/* Structure for the reverb diffusion parameter of Reverb module. */
typedef struct audproc_reverb_diffusion_t audproc_reverb_diffusion_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_diffusion_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_DIFFUSION parameter used by the
    Reverb module.
*/
struct audproc_reverb_diffusion_t
{
    uint32_t                  diffusion;
    /**< Specifies the echo density in the late reverberation decay.

         @values 0 to +1000 per millie */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_diffusion_t */

/* Structure for the reverb density parameter of Reverb module. */
typedef struct audproc_reverb_density_t audproc_reverb_density_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_reverb_density_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_REVERB_DENSITY parameter used by the
    Reverb module.
*/
struct audproc_reverb_density_t
{
    uint32_t                  density;
    /**< Specifies the modal density of the late reverberation decay.

         @values 0 to +1000 per millie */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_reverb_density_t */
/** @endcond */

/** @addtogroup audiopp_module_popless_equalizer
@{ */
/** ID of the Popless Equalizer module. This module supports the
    following parameter IDs:
    - #AUDPROC_PARAM_ID_EQ_ENABLE
    - #AUDPROC_PARAM_ID_EQ_CONFIG
    - #AUDPROC_PARAM_ID_EQ_NUM_BANDS
    - #AUDPROC_PARAM_ID_EQ_BAND_LEVELS
    - #AUDPROC_PARAM_ID_EQ_BAND_LEVEL_RANGE
    - #AUDPROC_PARAM_ID_EQ_BAND_FREQS
    - #AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE
    - #AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ
    - #AUDPROC_PARAM_ID_EQ_BAND_INDEX
    - #AUDPROC_PARAM_ID_EQ_PRESET_ID
    - #AUDPROC_PARAM_ID_EQ_NUM_PRESETS
    - #AUDPROC_PARAM_ID_EQ_PRESET_NAME
*/
#define AUDPROC_MODULE_ID_POPLESS_EQUALIZER                    0x000108BA

/** ID of the Popless Equalizer Enable parameter used by
    AUDPROC_MODULE_ID_POPLESS_EQUALIZER.

    @msgpayload{audproc_eq_enable_t}
    @table{weak__audproc__eq__enable__t} @newpage
*/
#define AUDPROC_PARAM_ID_EQ_ENABLE                             0x000108BB

/** ID of the Popless Equalizer Configuration parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER.

    @msgpayload{audproc_eq_config_t}
    @tablens{weak__audproc__eq__config__t}
*/
#define AUDPROC_PARAM_ID_EQ_CONFIG                             0x000108BC

/** ID of the Popless Equalizer Number of Bands parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER. This parameter ID is
    used for the Get parameter only.

    @msgpayload{audproc_eq_num_bands_t}
    @table{weak__audproc__eq__num__bands__t} @newpage
*/
#define AUDPROC_PARAM_ID_EQ_NUM_BANDS                          0x000108BD

/** ID of the Popless Equalizer Band Levels parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER. This parameter ID is
    used for the Get parameter only.

    @msgpayload{audproc_eq_band_levels_t}
    @table{weak__audproc__eq__band__levels__t}
*/
#define AUDPROC_PARAM_ID_EQ_BAND_LEVELS                        0x000108BE

/** ID of the Popless Equalizer Band Level Range parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER. This parameter ID is
    used for the Get parameter only.

    @msgpayload{audproc_eq_band_level_range_t}
    @table{weak__audproc__eq__band__level__range__t} @newpage
*/
#define AUDPROC_PARAM_ID_EQ_BAND_LEVEL_RANGE                   0x000108BF

/** ID of the Popless Equalizer Band Frequencies parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER. This parameter ID is
    used for the Get parameter only.

    @msgpayload{audproc_eq_band_freqs_t}
    @table{weak__audproc__eq__band__freqs__t}
*/
#define AUDPROC_PARAM_ID_EQ_BAND_FREQS                         0x000108C0

/** ID of the Popless Equalizer Single Band Frequency range parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER.
    This parameter ID is used for the Get parameter only.

    @msgpayload{audproc_eq_single_band_freq_range_t}
    @table{weak__audproc__eq__single__band__freq__range__t} @newpage
*/
#define AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE             0x000108C1

/** ID of the Popless Equalizer Single Band Frequency parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER. This parameter ID
    is used for the Set parameter only.

    @msgpayload{audproc_eq_single_band_freq_t}
    @table{weak__audproc__eq__single__band__freq__t}
*/
#define AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ                   0x000108C2

/** ID of the Popless Equalizer Band Index parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER.

    @msgpayload{audproc_eq_band_index_t}
    @table{weak__audproc__eq__band__index__t}
*/
#define AUDPROC_PARAM_ID_EQ_BAND_INDEX                         0x000108C3

/** ID of the Popless Equalizer Preset ID parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER. This parameter ID is used
    for the Get parameter only.

    @msgpayload{audproc_eq_preset_id_t}
    @table{weak__audproc__eq__preset__id__t}
*/
#define AUDPROC_PARAM_ID_EQ_PRESET_ID                          0x000108C4

/** ID of the Popless Equalizer Number of Presets parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER. This parameter ID is used
    for the Get parameter only.

    @msgpayload{audproc_eq_num_presets_t}
    @table{weak__audproc__eq__num__presets__t}
*/
#define AUDPROC_PARAM_ID_EQ_NUM_PRESETS                        0x000108C5

/** ID of the Popless Equalizer Preset Name parameter used by
    #AUDPROC_MODULE_ID_POPLESS_EQUALIZER. This parameter ID is used
    for the Get parameter only.

    @msgpayload{audproc_eq_preset_name_t}
    @table{weak__audproc__eq__preset__name__t}
*/
#define AUDPROC_PARAM_ID_EQ_PRESET_NAME                        0x000108C6

/** @} */ /* end_addtogroup audiopp_module_popless_equalizer */

/* Structure for the enable parameter of Popless Equalizer module. */
typedef struct audproc_eq_enable_t audproc_eq_enable_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_enable_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_ENABLE parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_enable_t
{
    uint32_t                  enable_flag;
    /**< Specifies whether Popless Equalizer is disabled (0) or enabled (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_enable_t */

/** @addtogroup audiopp_module_popless_equalizer
@{ */
/* Presets -1 to 16 are for Qualcomm, and 18 to 28 are for OpenSL. */
/** User-customized equalizer preset (with audio effects specified
    individually). */
#define  AUDPROC_EQ_PRESET_USER_CUSTOM     (-1)

/** Equalizer preset for no audio effects. */
#define  AUDPROC_EQ_PRESET_BLANK           0

/** Equalizer preset for audio effects like a club. */
#define  AUDPROC_EQ_PRESET_CLUB            1

/** Equalizer preset for audio effects like a dance. */
#define  AUDPROC_EQ_PRESET_DANCE           2

/** Qualcomm equalizer preset for full bass audio effects. */
#define  AUDPROC_EQ_PRESET_FULLBASS        3

/** Equalizer preset for bass and treble audio effects. */
#define  AUDPROC_EQ_PRESET_BASSTREBLE      4

/** Equalizer preset for full treble audio effects. */
#define  AUDPROC_EQ_PRESET_FULLTREBLE      5

/** Equalizer preset for audio effects that are suitable for playback through
    laptop/phone speakers.
*/
#define  AUDPROC_EQ_PRESET_LAPTOP          6

/** Equalizer preset for audio effects that sound like a large hall. */
#define  AUDPROC_EQ_PRESET_LHALL           7

/** Equalizer preset for audio effects that sound like a live performance. */
#define  AUDPROC_EQ_PRESET_LIVE            8

/** Equalizer preset for audio effects that sound like a party. */
#define  AUDPROC_EQ_PRESET_PARTY           9

/** Equalizer preset for pop audio effects. */
#define  AUDPROC_EQ_PRESET_POP             10

/** Equalizer preset for reggae audio effects. */
#define  AUDPROC_EQ_PRESET_REGGAE          11

/** Equalizer preset for rock audio effects. */
#define  AUDPROC_EQ_PRESET_ROCK            12

/** Equalizer preset for Ska audio effects. */
#define  AUDPROC_EQ_PRESET_SKA             13

/** Equalizer preset for soft pop audio effects. */
#define  AUDPROC_EQ_PRESET_SOFT            14

/** Equalizer preset for soft rock audio effects. */
#define  AUDPROC_EQ_PRESET_SOFTROCK        15

/** Equalizer preset for techno audio effects. */
#define  AUDPROC_EQ_PRESET_TECHNO          16

/** User-customized equalizer preset (with audio effects specified
    individually) (OpenSL).
*/
#define  AUDPROC_EQ_PRESET_USER_CUSTOM_AUDIO_FX    18

/** Equalizer preset for normal (generic) audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_NORMAL_AUDIO_FX         19

/** Equalizer preset for classical audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_CLASSICAL_AUDIO_FX      20

/** Equalizer preset for dance audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_DANCE_AUDIO_FX          21

/** Equalizer preset for no audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_FLAT_AUDIO_FX           22

/** Equalizer preset for folk audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_FOLK_AUDIO_FX           23

/** Equalizer preset for heavy metal audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_HEAVYMETAL_AUDIO_FX     24

/** Equalizer preset for hip hop audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_HIPHOP_AUDIO_FX         25

/** Equalizer preset for jazz audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_JAZZ_AUDIO_FX           26

/** Equalizer preset for pop audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_POP_AUDIO_FX            27

/** Equalizer preset for rock audio effects (OpenSL). */
#define  AUDPROC_EQ_PRESET_ROCK_AUDIO_FX           28

/** @} */ /* end_addtogroup audiopp_module_popless_equalizer */

/* Structure for the config parameter of Popless Equalizer module. */
typedef struct audproc_eq_config_t audproc_eq_config_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_config_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_CONFIG parameter used by the
    Popless Equalizer module. */
/** Following this structure is a sequence of per-band equalizer parameter
    structures. The number of per-band parameter structures is dependent on the
    num_bands field.

    For pre-defined Qualcomm and pre-defined OpenSL EQ, the sequence of
    per-band parameters is not required, and num_bands must be set to zero.
*/
struct audproc_eq_config_t
{
    int32_t                  eq_pregain;
    /**< Gain in Q27 before any equalization processing. */

    int32_t                  preset_id;
    /**< Specifies either the user-customized equalizers or two sets of
         equalizers pre-defined by Qualcomm and OpenSL, respectively.

         @values
         - -1 -- Custom equalizer that meets Qualcomm requirements
         - 0 to 16 -- Equalizers pre-defined by Qualcomm
         - 18 -- Custom equalizer that meets OpenSL requirements
         - 19 to 28 -- Equalizers pre-defined by OpenSL @tablebulletend */

    uint32_t                  num_bands;
    /**< Number of subbands for equalization when a custom preset_id is selected.

         @values
         - 1 to 12 -- If preset_id = -1
         - 5 -- If preset_id = 18
         - 0 -- For all other preset_id settings @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_config_t */

/* Structure for per-band parameters for the Popless Equalizer module. */
typedef struct audproc_eq_per_band_config_t audproc_eq_per_band_config_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_per_band_config_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_CONFIG per-band config
    parameters used by the Popless Equalizer module. (See also audproc_eq_config_t.)
*/
struct audproc_eq_per_band_config_t
{
    uint32_t                  filter_type;
    /**< Type of filter.

         @values
         - #AUDPROC_PARAM_EQ_TYPE_NONE
         - #AUDPROC_PARAM_EQ_BASS_BOOST
         - #AUDPROC_PARAM_EQ_BASS_CUT
         - #AUDPROC_PARAM_EQ_TREBLE_BOOST
         - #AUDPROC_PARAM_EQ_TREBLE_CUT
         - #AUDPROC_PARAM_EQ_BAND_BOOST
         - #AUDPROC_PARAM_EQ_BAND_CUT @tablebulletend */

    uint32_t                  freq_millihertz;
    /**< Filter band frequency in MilliHertz (center or cutoff frequency) */

    int32_t                   gain_millibels;
    /**< Filter band initial gain.

         @values +1500 to -1500 mdB in 100 mdB increments */

    uint32_t                  quality_factor;
    /**< Filter band quality factor expressed as a Q8 number, i.e., a
         fixed-point number with a q factor of 8. For example, 3000/(2^8). */

    uint32_t                  band_idx;
    /**< Band index.

         @values 0 to num_bands - 1 (num_bands is specified in
                 audproc_eq_config_t) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_per_band_config_t */

/* Structure for the num bands parameter of Popless Equalizer module. */
typedef struct audproc_eq_num_bands_t audproc_eq_num_bands_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_num_bands_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_NUM_BANDS parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_num_bands_t
{
    uint32_t                  num_bands;
    /**< Number of subbands in the current equalizer filter. For the Get
         parameter only, the library returns the value.

         @values
         - 1 to 12 -- For equalizers compliant to or pre-defined by Qualcomm
         - 5 -- For equalizers compliant to or pre-defined by OpenSL @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_num_bands_t */

/* Structure for the band levels parameter of Popless Equalizer module. */
typedef struct audproc_eq_band_levels_t audproc_eq_band_levels_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_band_levels_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_BAND_LEVELS parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_band_levels_t
{
    uint32_t                  num_bands;
    /**< Number of subbands in the current equalizer filter. For the Get
         parameter only, the library returns the value.

         @values
         - 1 to 12 -- For equalizers compliant to or pre-defined by Qualcomm
         - 5 -- For equalizers compliant to or pre-defined by OpenSL. */

    int32_t                   band_levels[12];
    /**< Array of gains (in millibels) of each subband filter.
         For the Get parameter only, the library returns the values.

         The actual meaningful contents in the array depends on num_bands.
         In other words, band_levels[0] - band_levels[num_bands-1] contains
         valid values returned by the library. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_band_levels_t */

/* Structure for the band level range parameter of Popless Equalizer module. */
typedef struct audproc_eq_band_level_range_t audproc_eq_band_level_range_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_band_level_range_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_BAND_LEVEL_RANGE parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_band_level_range_t
{
    int32_t                  min_level_millibels;
    /**< Minimum supported equalizer gain.
         For the Get parameter only, the library returns the value.

         @values Always -1500 */

    int32_t                  max_level_millibels;
    /**< Maximum supported equalizer gain.
         For the Get parameter only, the library returns the value.

         @values Always +1500 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_band_level_range_t */

/* Structure for the band frequency parameter of Popless Equalizer module. */
typedef struct audproc_eq_band_freqs_t audproc_eq_band_freqs_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_band_freqs_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_BAND_FREQS parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_band_freqs_t
{
    uint32_t                  num_bands;
    /**< Number of subbands in the current equalizer filter. For the Get
         parameter only, the library returns the value.

         @values
         1 to 12 -- For equalizers compliant to or pre-defined by Qualcomm
         - 5 -- For equalizers compliant to or pre-defined by OpenSL  @tablebulletend*/

    uint32_t                   band_freqs[12];
    /**< Array of center or cutoff frequency of each subband filter, in
         millihertz. For the Get parameter only, the library returns the values.

         The actual meaningful contents in the array depends on num_bands.
         In other words, band_freqs[0] - band_freqs[num_bands-1] contains valid
         values returned by the library. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_band_freqs_t */

/* Structure for the band frequency range parameter of Popless Equalizer module. */
typedef struct audproc_eq_single_band_freq_range_t audproc_eq_single_band_freq_range_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_single_band_freq_range_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_single_band_freq_range_t
{
    uint32_t                  min_freq_millihertz;
    /**< Lower frequency boundary of the subband equalizer filter with the
         band_index provided by a previous Set Parameter call with
         #AUDPROC_PARAM_ID_EQ_BAND_INDEX.

         For the Get parameter only, the library returns the value.

         @values Any value in the range of 0 to sample_rate/2

         If the band index is not provided by a previous Set Parameter call,
         the parameters of the first subband with band_index = 0 are returned. */

    uint32_t                  max_freq_millihertz;
    /**< Upper frequency boundary of the subband equalizer filter with the
         band_index provided by a previous Set Parameter call with
         #AUDPROC_PARAM_ID_EQ_BAND_INDEX.

         For the Get parameter only, the library returns the value.

         @values Any value in the range of 0 to sample_rate/2

         If the band index is not provided by a previous Set Parameter call,
         the parameters of the first subband with band_index = 0 are returned. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_single_band_freq_range_t */

/* Structure for the band frequency parameter of Popless Equalizer module. */
typedef struct audproc_eq_single_band_freq_t audproc_eq_single_band_freq_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_single_band_freq_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_single_band_freq_t
{
    uint32_t                  freq_millihertz;
    /**< For the Set parameter only, center or cutoff frequency of the subband
         equalizer filter for which the band_index is requested in a following
         Get Parameter call with #AUDPROC_PARAM_ID_EQ_BAND_INDEX.

         @values Any value in the range of 0 to sample_rate/2 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_single_band_freq_t */

/* Structure for the band index parameter of Popless Equalizer module. */
typedef struct audproc_eq_band_index_t audproc_eq_band_index_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_band_index_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_BAND_INDEX parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_band_index_t
{
    uint32_t                  band_index;
    /**< Index of each band.

         @values
         - 0 to 11 -- For Qualcomm compliant equalizers
         - 0 to 4 -- For OpenSL compliant equalizers

        If AUDPROC_PARAM_ID_EQ_BAND_INDEX is used in the Set Parameter call,
        this band_index is used to get the subband frequency range in the next
        Get Parameter call with #AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ_RANGE.

        If this parameter ID is used in the Get Parameter call, the band_index
        with the center or cutoff frequency set by the Set parameter of
        #AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ is returned.

        If the center/cutoff frequency of the requested subband is not set
        before the Get parameter of #AUDPROC_PARAM_ID_EQ_BAND_INDEX, the
        default band_index of zero is returned. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_band_index_t */

/* Structure for the preset id parameter of Popless Equalizer module. */
typedef struct audproc_eq_preset_id_t audproc_eq_preset_id_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_preset_id_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_PRESET_ID parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_preset_id_t
{
    int32_t                  preset_id;
    /**< Preset ID of the current equalizer configuration. For the Get parameter
         only, the library returns the value.

         @values
         - -1 to 16 -- For Qualcomm compliant equalizers
         - 18 to 28 -- For OpenSL compliant equalizers @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_preset_id_t */

/* Structure for the num presets parameter of Popless Equalizer module. */
typedef struct audproc_eq_num_presets_t audproc_eq_num_presets_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_num_presets_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_NUM_PRESETS parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_num_presets_t
{
    uint32_t                 num_presets;
    /**< Total number of supported presets in the current equalizer
         configuration.
         For the Get parameter only, the library returns the value.

         @values
         - 17 -- For Qualcomm compliant equalizers
         - 10 -- For OpenSL compliant equalizers @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_num_presets_t */

/* Structure for the preset name parameter of Popless Equalizer module. */
typedef struct audproc_eq_preset_name_t audproc_eq_preset_name_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_audproc_eq_preset_name_t
@{ */
/* Payload of the AUDPROC_PARAM_ID_EQ_PRESET_NAME parameter used by the
    Popless Equalizer module.
*/
struct audproc_eq_preset_name_t
{
    uint8_t                 preset_name[32];
    /**< Name of the current EQ preset in ASCII.
         For the Get parameter only, the library returns the value. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_audproc_eq_preset_name_t */

/** @addtogroup audiopp_topos_adm
@{ */
/** ID of AudioPlus in the COPP audio postprocessing headphone topology.

    @inputfig{1,AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_HEADPHONE_topology.png}

    - Virtualizer     (#AUDPROC_MODULE_ID_VIRTUALIZER)
    - Reverb          (#AUDPROC_MODULE_ID_REVERB)
    - Bass Boost      (#AUDPROC_MODULE_ID_BASS_BOOST)
*/
#define AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_HEADPHONE           0x0001089E

/** ID of AudioPlus in the COPP audio postprocessing speaker topology.

    @inputfig{1,AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_SPEAKER_topology.png}

    - Virtualizer        (#AUDPROC_MODULE_ID_VIRTUALIZER)
    - Reverb             (#AUDPROC_MODULE_ID_REVERB)
    - PBE                (#AUDPROC_MODULE_ID_PBE) @newpage
*/
#define AUDPROC_COPP_TOPOLOGY_ID_AUDIO_PLUS_SPEAKER             0x0001089F

/** ID of AudioPlus in the POPP audio postprocessing topology.

    @inputfig{1,AUDPROC_POPP_TOPOLOGY_ID_AUDIO_PLUS_topology.png}

    - Resampler                (#AUDPROC_MODULE_ID_RESAMPLER)
    - Soft Volume Control      (#AUDPROC_MODULE_ID_VOL_CTRL)
    - Popless Equalizer        (#AUDPROC_MODULE_ID_POPLESS_EQUALIZER)
*/
#define AUDPROC_POPP_TOPOLOGY_ID_AUDIO_PLUS                      0x000108A0

/** @} */ /* end_addtogroup audiopp_topos_adm */


// The following names have been deprecated. Please use the new names.
#define ADM_MODULE_ID_EANS AUDPROC_MODULE_ID_EANS
#define ADM_PARAM_ID_EANS_ENABLE AUDPROC_PARAM_ID_EANS_ENABLE
#define ADM_PARAM_ID_EANS_PARAMS AUDPROC_PARAM_ID_EANS_PARAMS
#define ADM_MODULE_ID_TX_MIC_GAIN_CTRL AUDPROC_MODULE_ID_TX_MIC_GAIN_CTRL
#define ADM_PARAM_ID_TX_MIC_GAIN AUDPROC_PARAM_ID_TX_MIC_GAIN
#define ADM_MODULE_ID_RX_CODEC_GAIN_CTRL AUDPROC_MODULE_ID_RX_CODEC_GAIN_CTRL
#define ADM_PARAM_ID_RX_CODEC_GAIN AUDPROC_PARAM_ID_RX_CODEC_GAIN
#define ADM_MODULE_ID_HPF_IIR_TX_FILTER AUDPROC_MODULE_ID_HPF_IIR_TX_FILTER
#define ADM_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_ENABLE_CONFIG
#define ADM_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_PRE_GAIN
#define ADM_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS AUDPROC_PARAM_ID_HPF_IIR_TX_FILTER_CONFIG_PARAMS
#define ADM_MODULE_ID_TX_IIR_FILTER AUDPROC_MODULE_ID_TX_IIR_FILTER
#define ADM_MODULE_ID_LEFT_IIR_TUNING_FILTER AUDPROC_MODULE_ID_LEFT_IIR_TUNING_FILTER
#define ADM_MODULE_ID_RIGHT_IIR_TUNING_FILTER AUDPROC_MODULE_ID_RIGHT_IIR_TUNING_FILTER
#define ADM_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG AUDPROC_PARAM_ID_TX_IIR_FILTER_ENABLE_CONFIG
#define ADM_PARAM_ID_TX_IIR_FILTER_PRE_GAIN AUDPROC_PARAM_ID_TX_IIR_FILTER_PRE_GAIN
#define ADM_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS AUDPROC_PARAM_ID_TX_IIR_FILTER_CONFIG_PARAMS
#define ASM_MODULE_ID_VOL_CTRL AUDPROC_MODULE_ID_VOL_CTRL
#define ASM_PARAM_ID_VOL_CTRL_MASTER_GAIN AUDPROC_PARAM_ID_VOL_CTRL_MASTER_GAIN
#define ASM_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN AUDPROC_PARAM_ID_VOL_CTRL_LR_CHANNEL_GAIN
#define ASM_PARAM_ID_VOL_CTRL_MUTE_CONFIG AUDPROC_PARAM_ID_VOL_CTRL_MUTE_CONFIG
#define ASM_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS AUDPROC_PARAM_ID_SOFT_VOL_STEPPING_PARAMETERS
#define ASM_PARAM_ID_SOFT_PAUSE_PARAMETERS AUDPROC_PARAM_ID_SOFT_PAUSE_PARAMETERS
#define ASM_PARAM_ID_MULTICHANNEL_GAIN AUDPROC_PARAM_ID_MULTICHANNEL_GAIN
#define ASM_PARAM_ID_MULTICHANNEL_MUTE AUDPROC_PARAM_ID_MULTICHANNEL_MUTE
#define ASM_PARAM_SVC_RAMPINGCURVE_LINEAR AUDPROC_PARAM_SVC_RAMPINGCURVE_LINEAR
#define ASM_PARAM_SVC_RAMPINGCURVE_EXP AUDPROC_PARAM_SVC_RAMPINGCURVE_EXP
#define ASM_PARAM_SVC_RAMPINGCURVE_LOG AUDPROC_PARAM_SVC_RAMPINGCURVE_LOG
#define ASM_MODULE_ID_IIR_TUNING_FILTER AUDPROC_MODULE_ID_IIR_TUNING_FILTER
#define ASM_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG AUDPROC_PARAM_ID_IIR_TUNING_FILTER_ENABLE_CONFIG
#define ASM_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN AUDPROC_PARAM_ID_IIR_TUNING_FILTER_PRE_GAIN
#define ASM_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS AUDPROC_PARAM_ID_IIR_TUNING_FILTER_CONFIG_PARAMS
#define ASM_MODULE_ID_MBDRC AUDPROC_MODULE_ID_MBDRC
#define ASM_PARAM_ID_MBDRC_ENABLE AUDPROC_PARAM_ID_MBDRC_ENABLE
#define ASM_PARAM_ID_MBDRC_CONFIG_PARAMS AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS
#define ASM_MODULE_ID_MBDRCV2 AUDPROC_MODULE_ID_MBDRCV2
#define ASM_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2 AUDPROC_PARAM_ID_MBDRC_CONFIG_PARAMS_IMPROVED_FILTBANK_V2
#define ASM_MODULE_ID_EQUALIZER AUDPROC_MODULE_ID_EQUALIZER
#define ASM_PARAM_ID_EQUALIZER_PARAMETERS AUDPROC_PARAM_ID_EQUALIZER_PARAMETERS
#define ASM_PARAM_EQ_TYPE_NONE AUDPROC_PARAM_EQ_TYPE_NONE
#define ASM_PARAM_EQ_BASS_BOOST AUDPROC_PARAM_EQ_BASS_BOOST
#define ASM_PARAM_EQ_BASS_CUT AUDPROC_PARAM_EQ_BASS_CUT
#define ASM_PARAM_EQ_TREBLE_BOOST AUDPROC_PARAM_EQ_TREBLE_BOOST
#define ASM_PARAM_EQ_TREBLE_CUT AUDPROC_PARAM_EQ_TREBLE_CUT
#define ASM_PARAM_EQ_BAND_BOOST AUDPROC_PARAM_EQ_BAND_BOOST
#define ASM_PARAM_EQ_BAND_CUT AUDPROC_PARAM_EQ_BAND_CUT
#define ASM_PARAM_ID_PBE AUDPROC_MODULE_ID_PBE
#define ASM_PARAM_ID_PBE_ENABLE AUDPROC_PARAM_ID_PBE_ENABLE
#define ASM_PARAM_ID_PBE_PARAMS_CONFIG AUDPROC_PARAM_ID_PBE_PARAMS_CONFIG
#define ASM_MODULE_ID_SPA AUDPROC_MODULE_ID_SPA
#define ASM_PARAM_ID_SPA_ENABLE AUDPROC_PARAM_ID_SPA_ENABLE
#define ASM_PARAM_ID_SPA_PARAMS_CONFIG AUDPROC_PARAM_ID_SPA_PARAMS_CONFIG

typedef audproc_eans_enable_t adm_eans_enable_t;
typedef audproc_eans_params_t adm_eans_params_t;
typedef audproc_tx_mic_gain_t adm_tx_mic_gain_t;
typedef audproc_rx_codec_gain_t adm_rx_codec_gain_t;
typedef audproc_hpf_tx_iir_filter_enable_cfg_t adm_hpf_tx_iir_filter_enable_cfg_t;
typedef audproc_hpf_tx_iir_filter_pre_gain_t adm_hpf_tx_iir_filter_pre_gain_t;
typedef audproc_hpf_tx_iir_filter_cfg_params_t adm_hpf_tx_iir_filter_cfg_params_t;
typedef audproc_tx_iir_filter_enable_cfg_t adm_tx_iir_filter_enable_cfg_t;
typedef audproc_tx_iir_filter_pre_gain_t adm_tx_iir_filter_pre_gain_t;
typedef audproc_tx_iir_filter_cfg_params_t adm_tx_iir_filter_cfg_params_t;
typedef audproc_mono2stereo_upmix_enable_t asm_mono2stereo_upmix_enable_t;
typedef audproc_volume_ctrl_master_gain_t asm_volume_ctrl_master_gain_t;
typedef audproc_volume_ctrl_lr_chan_gain_t asm_volume_ctrl_lr_chan_gain_t;
typedef audproc_volume_ctrl_mute_config_t asm_volume_ctrl_mute_config_t;
typedef audproc_soft_step_volume_params_t asm_soft_step_volume_params_t;
typedef audproc_soft_pause_params_t asm_soft_pause_params_t;
typedef audproc_volume_ctrl_channel_type_gain_pair_t asm_volume_ctrl_channel_type_gain_pair_t;
typedef audproc_volume_ctrl_multichannel_gain_t asm_volume_ctrl_multichannel_gain_t;
typedef audproc_volume_ctrl_channel_type_mute_pair_t asm_volume_ctrl_channel_type_mute_pair_t;
typedef audproc_volume_ctrl_multichannel_mute_t asm_volume_ctrl_multichannel_mute_t;
typedef audproc_iir_tuning_filter_enable_t asm_iir_tuning_filter_enable_t;
typedef audproc_iir_tuning_filter_pregain_t asm_iir_tuning_filter_pregain_t;
typedef audproc_iir_filter_config_params_t asm_iir_filter_config_params_t;
typedef audproc_mbdrc_enable_t asm_mbdrc_enable_t;
typedef audproc_mbdrc_config_params_t asm_mbdrc_config_params_t;
typedef audproc_subband_drc_config_params_t asm_subband_drc_config_params_t;
typedef audproc_eq_params_t asm_eq_params_t;
typedef audproc_eq_per_band_params_t asm_eq_per_band_params_t;
typedef audproc_pbe_enable_t asm_pbe_enable_t;
typedef audproc_pbe_params_t asm_pbe_params_t;
typedef audproc_spa_enable_t asm_spa_enable_t;
typedef audproc_spa_param_t asm_spa_param_t;


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_AUDPROC_API_H_ */
