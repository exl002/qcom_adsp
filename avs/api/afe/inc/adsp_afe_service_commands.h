#ifndef _ADSP_AFE_SERVICE_COMMANDS_H_
#define _ADSP_AFE_SERVICE_COMMANDS_H_

/*========================================================================*/
/**
@file adsp_afe_service_commands.h

This file contains AFE service commands and events structures definitions.
*/

/*===========================================================================
NOTE: The description above does not appear in the PDF.

      The AFE_mainpage.dox file contains all file/group descriptions that are
      in the output PDF generated using Doxygen and Latex. To edit or update
      any of the file/group text in the PDF, edit the AFE_mainpage.dox file or
      contact Tech Pubs.
===========================================================================*/

/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================ */

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/afe/inc/adsp_afe_service_commands.h#30 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/16/12    sw     (Tech Pubs) Edited comments and applied latest Doxygen markup.
03/27/12   mspk    Non backward compatible API changes for next generation
========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup afe_version
    Version of the aDSP AFE module.
*/
#define ADSP_AFE_VERSION    0x00200000

/** @addtogroup afe_port_id_ranges
@{ */

/** Size of the range of port IDs for the audio interface. */
#define  AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE                       0xF

/** Size of the range of port IDs for internal BT-FM ports. */
#define AFE_PORT_ID_INTERNAL_BT_FM_RANGE_SIZE                       0x6

/** Size of the range of port IDs for SLIMbus<sup>&reg;</sup> multichannel
    ports.
*/
#define AFE_PORT_ID_SLIMBUS_RANGE_SIZE                              0xE

/** Size of the range of port IDs for real-time proxy ports. */
#define  AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE                       0x6

/** Size of the range of port IDs for pseudoports. */
#define AFE_PORT_ID_PSEUDOPORT_RANGE_SIZE                           0x5

/** Size of the range of port IDs for HDMI Input */
#define AFE_PORT_ID_HDMI_INPUT_RANGE_SIZE                           0x1

/** Start of the range of port IDs for the audio interface. */
#define  AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START                      0x1000

/** End of the range of port IDs for the audio interface. */
#define  AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END                        (AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START+AFE_PORT_ID_AUDIO_IF_PORT_RANGE_SIZE-1)

/** Start of the range of port IDs for real-time proxy ports. */
#define  AFE_PORT_ID_RT_PROXY_PORT_RANGE_START                      0x2000

/** End of the range of port IDs for real-time proxy ports. */
#define  AFE_PORT_ID_RT_PROXY_PORT_RANGE_END                        (AFE_PORT_ID_RT_PROXY_PORT_RANGE_START+AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE-1)

/** Start of the range of port IDs for internal BT-FM devices. */
#define AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START                      0x3000

/** End of the range of port IDs for internal BT-FM devices. */
#define AFE_PORT_ID_INTERNAL_BT_FM_RANGE_END                        (AFE_PORT_ID_INTERNAL_BT_FM_RANGE_START+AFE_PORT_ID_INTERNAL_BT_FM_RANGE_SIZE-1)

/** Start of the range of port IDs for SLIMbus devices. */
#define AFE_PORT_ID_SLIMBUS_RANGE_START                             0x4000

/** End of the range of port IDs for SLIMbus devices. */
#define AFE_PORT_ID_SLIMBUS_RANGE_END                               (AFE_PORT_ID_SLIMBUS_RANGE_START+AFE_PORT_ID_SLIMBUS_RANGE_SIZE-1)

/** Start of the range of port IDs for pseudoports. */
#define AFE_PORT_ID_PSEUDOPORT_RANGE_START                          0x8001

/** End of the range of port IDs for pseudoports.  */
#define AFE_PORT_ID_PSEUDOPORT_RANGE_END                            (AFE_PORT_ID_PSEUDOPORT_RANGE_START+AFE_PORT_ID_PSEUDOPORT_RANGE_SIZE-1)

/** Start of the range of port IDs for pseudoports. */
#define AFE_PORT_ID_HDMI_INPUT_RANGE_START                          0x6001

/** End of the range of port IDs for pseudoports. */
#define AFE_PORT_ID_HDMI_INPUT_RANGE_END                            (AFE_PORT_ID_HDMI_INPUT_RANGE_START+AFE_PORT_ID_HDMI_INPUT_RANGE_SIZE-1)


/** @} */ /* end_addtogroup afe_port_id_ranges */

/** @addtogroup afe_defined_port_ids
@{ */
/** Primary MI2S Rx port ID. */
#define AFE_PORT_ID_PRIMARY_MI2S_RX                                 0x1000

/** Primary MI2S Tx port ID. */
#define AFE_PORT_ID_PRIMARY_MI2S_TX                                 0x1001

/** Secondary MI2S Rx port ID. */
#define AFE_PORT_ID_SECONDARY_MI2S_RX                               0x1002

/** Secondary MI2S Tx port ID. */
#define AFE_PORT_ID_SECONDARY_MI2S_TX                               0x1003

/** Teritary MI2S Rx port ID. */
#define AFE_PORT_ID_TERTIARY_MI2S_RX                                0x1004

/** Teritary MI2S Tx port ID. */
#define AFE_PORT_ID_TERTIARY_MI2S_TX                                0x1005

/** Quaternary MI2S Rx port ID. */
#define AFE_PORT_ID_QUATERNARY_MI2S_RX                              0x1006

/** Quaternary MI2S Tx port ID. */
#define AFE_PORT_ID_QUATERNARY_MI2S_TX                              0x1007

/** I2S Rx port ID. This macro is for backward compatability.
    For naming consistency, use #AFE_PORT_ID_I2S_RX instead.
*/
#define AUDIO_PORT_ID_I2S_RX                                        0x1008

/** I2S Rx port ID. */
#define AFE_PORT_ID_I2S_RX                                          AUDIO_PORT_ID_I2S_RX

/** Digital Mic Tx port ID. */
#define AFE_PORT_ID_DIGITAL_MIC_TX                                  0x1009

/**Primary Aux PCM Rx port ID. */
#define AFE_PORT_ID_PRIMARY_PCM_RX                                  0x100A

/** Primary Aux PCM Tx port ID. */
#define AFE_PORT_ID_PRIMARY_PCM_TX                                  0x100B

/** Secondary Aux PCM Rx port ID. */
#define AFE_PORT_ID_SECONDARY_PCM_RX                                0x100C

/** Secondary Aux PCM Tx port ID. */
#define AFE_PORT_ID_SECONDARY_PCM_TX                                0x100D

/** MultiChannel HDMI Rx port ID. */
#define AFE_PORT_ID_MULTICHAN_HDMI_RX                               0x100E

/** When this port is opened, the real-time port driver reads data
    from the AFE service and sends it to the real-time device.
*/
#define  AFE_PORT_ID_RT_PROXY_PORT_001_RX                           0x2000

/** When this port is opened, the real-time port driver writes data
    to the AFE service from the real-time device.
*/
#define  AFE_PORT_ID_RT_PROXY_PORT_001_TX                           0x2001

/** When this port is opened, the real-time port driver reads data
    from the AFE service and sends it to the real-time device.
*/
#define  AFE_PORT_ID_RT_PROXY_PORT_002_RX                           0x2002

/** When this port is opened, the real-time port driver writes data
    to the AFE service from the real-time device.
*/
#define  AFE_PORT_ID_RT_PROXY_PORT_002_TX                           0x2003

/** When this port is opened, the real-time port driver reads data
    from the AFE service and sends it to the real-time device.
*/
#define  AFE_PORT_ID_RT_PROXY_PORT_003_RX                           0x2004

/** When this port is opened, the real-time port driver writes data
    to the AFE service from the real-time device.
*/
#define  AFE_PORT_ID_RT_PROXY_PORT_003_TX                           0x2005

/** Internal Bluetooth Rx port ID. */
#define AFE_PORT_ID_INTERNAL_BT_SCO_RX                              0x3000

/** Internal Bluetooth Tx port ID. */
#define AFE_PORT_ID_INTERNAL_BT_SCO_TX                              0x3001

/** Internal A2DP Rx port ID. */
#define AFE_PORT_ID_INTERNAL_BT_A2DP_RX                             0x3002

/** Internal FM audio Rx port ID. */
#define AFE_PORT_ID_INTERNAL_FM_RX                                  0x3004

/** Internal FM audio Tx port ID. */
#define AFE_PORT_ID_INTERNAL_FM_TX                                  0x3005

/** SLIMbus Rx port on channel 0. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_RX                         0x4000

/** SLIMbus Tx port on channel 0. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_TX                         0x4001

/** SLIMbus Rx port on channel 1. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_1_RX                         0x4002

/** SLIMbus Tx port on channel 1. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_1_TX                         0x4003

/** SLIMbus Rx port on channel 2. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_2_RX                         0x4004

/** SLIMbus Tx port on channel 2. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_2_TX                         0x4005

/** SLIMbus Rx port on channel 3. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_3_RX                         0x4006

/** SLIMbus Tx port on channel 3. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_3_TX                         0x4007

/** SLIMbus Rx port on channel 4. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_4_RX                         0x4008

/** SLIMbus Tx port on channel 4. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_4_TX                         0x4009

/** SLIMbus Rx port on channel 5. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_5_RX                         0x400A

/** SLIMbus Tx port on channel 5. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_5_TX                         0x400B

/** SLIMbus Rx port on channel 6. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_6_RX                         0x400C

/** SLIMbus Tx port on channel 6. */
#define AFE_PORT_ID_SLIMBUS_MULTI_CHAN_6_TX                         0x400D

/** SPDIF RX port ID */
#define AFE_PORT_ID_SPDIF_RX                                        0x5000

/** HDMI TX port ID */
#define AFE_PORT_ID_HDMI_TX                                         0x6001


/** Generic pseudoport 1. */
#define AFE_PORT_ID_PSEUDOPORT_01                                   0x8001

/** Generic pseudoport 2. */
#define AFE_PORT_ID_PSEUDOPORT_02                                   0x8002

/** Pseudoport that corresponds to the voice Rx path. For recording, the voice
    Rx path samples are written to this port and consumed by the audio path.
*/
#define AFE_PORT_ID_VOICE_RECORD_RX                                 0x8003

/** Pseudoport that corresponds to the voice Tx path. For recording, the voice
    Tx path samples are written to this port and consumed by the audio path.
*/
#define AFE_PORT_ID_VOICE_RECORD_TX                                 0x8004

/** Pseudoport that corresponds to in-call voice delivery samples.

    During in-call audio delivery, the audio path delivers samples to this port
    from where the voice path delivers them on the Rx path.
*/
#define AFE_PORT_ID_VOICE_PLAYBACK_TX                               0x8005

/** Invalid port ID. */
#define AFE_PORT_ID_INVALID                                         0xFFFF

/** @} */ /* end_addtogroup afe_defined_port_ids */

/** @ingroup afe_cmd_start_dtmf
    Starts DTMF on any port, including a pseudoport.

  @apr_hdr_fields
    Opcode -- AFE_PORTS_CMD_DTMF_CTL

  @msg_payload{afe_ports_cmd_dtmf_ctl_t}
    @table{weak__afe__ports__cmd__dtmf__ctl__t}
    Following this structure in memory are num_ports number of port_ids. If
    num_ports is greater than one, the reserved field is one of the port_ids;
    otherwise, the reserved field must be set to zero.
    @par
    When multiple ports are involved, the status indicated by the
    acknowledgment means:
    @par
    - SUCCESS -- DTMF is initiated on all ports
    - FAILURE -- DTMF failed on one or more ports (may not be all ports)

  @detdesc
    When enabling DTMF on more than one port, specify num_ports with the number
    of ports where DTMF is to be enabled. The port ID of each port is to follow
    the port ID field in the command structure.
    @par
    @keep{6}
    For multiple ports, the command's return value indicates if there was a
    failure, i.e., DTMF might be successful on some ports and fail on others.
    If a return status is important, the command is to be sent to each port
    individually.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    The port must be active. If the port is inactive and a DTMF command is
    received, DTMF is not played. If the port becomes active, a new command
    must be received to play DTMF later.
*/ /* Q3 = Asynchronous Packet Router API (for MDM9x15 and Newer Devices)
           Interface Spec (80-N1463-2) */
#define AFE_PORTS_CMD_DTMF_CTL                                   0x00010102

/* Structure for the AFE_PORTS_CMD_DTMF_CTL command. */
typedef struct afe_ports_cmd_dtmf_ctl_t afe_ports_cmd_dtmf_ctl_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_ports_cmd_dtmf_ctl_t
@{ */
/* Payload of the AFE_PORTS_CMD_DTMF_CTL command, which starts DTMF on
    any port.
 */
struct afe_ports_cmd_dtmf_ctl_t
{
    int64_t                   dtmf_duration_in_ms;
    /**< Duration in milliseconds of the DTMF tone.

         @values
         - -1 -- Continuous DTMF of infinite duration
         - 0 -- Disable; this also stops a continuous DTMF if it was started

         Any other valid value indicates the duration in milliseconds. */

    uint16_t                  dtmf_high_freq;
    /**< DTMF high tone frequency.

         @values 1209, 1336, 1477, 1633 Hz */

    uint16_t                  dtmf_low_freq;
    /**< DTMF low tone frequency.

         @values 697, 770, 852, 941 Hz */

    uint16_t                  dtmf_gain;
    /**< Gain applied to a DTMF tone.

         @values Any number in Q13 format */

    uint16_t                  num_ports;
    /**< Number of ports following this structure in memory.

         @values @ge 1 */

    uint16_t                  port_ids;
    /**< Destination ports (array). For DTMF on multiple ports, port_ids is
         populated num_ports times, and they follow this element. */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_ports_cmd_dtmf_ctl_t */

/** @ingroup afe_module_sidetone_iir_filter
    ID of the Sidetone IIR module, which gets/sets the calibration data of
    the sidetone filter for a requested AFE Rx port.
    This module supports the following parameter IDs:
    - #AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG
    - #AFE_PARAM_ID_ENABLE

    This module is effective only when the sidetone path is enabled and active.

    @apr_hdr_fields
    Opcode -- AFE_MODULE_SIDETONE_IIR_FILTER @newpage
*/
#define AFE_MODULE_SIDETONE_IIR_FILTER                              0x00010202

/** @ingroup afe_common_params
    ID of the parameter used to enable/disable the following modules:
    - #AFE_MODULE_SIDETONE_IIR_FILTER
    - #AFE_MODULE_RMC
    - #AFE_MODULE_SPEAKER_PROTECTION
    - #AFE_MODULE_AANC
    - #AFE_MODULE_FB_SPKR_PROT_RX
    - #AFE_MODULE_FB_SPKR_PROT_VI_PROC
    - #AFE_MODULE_ID_SW_MAD

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_ENABLE

    @msg_payload{afe_mod_enable_param_t}
    @tablens{weak__afe__mod__enable__param__t}
*/
#define AFE_PARAM_ID_ENABLE                                          0x00010203

typedef struct afe_mod_enable_param_t afe_mod_enable_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_mod_enable_param_t
@{ */
/* Payload of the AFE_PARAM_ID_ENABLE parameter, which enables or
    disables any module. */
/** The fixed size of this structure is four bytes.
*/
struct afe_mod_enable_param_t
{
    uint16_t                  enable;
    /**< Specifies whether the module is enabled (1) or disabled (0). */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_mod_enable_param_t */

/** @ingroup afe_module_sidetone_iir_filter
    ID of the parameter used by #AFE_MODULE_SIDETONE_IIR_FILTER to configure
    the sidetone IIR filter. Both #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

  @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG

  @msg_payload{afe_sidetone_iir_filter_config_params_t}
    @table{weak__afe__sidetone__iir__filter__config__params__t}
    This structure is followed by the IIR filter coefficients:
    - Sequence of int32 ulFilterCoeffs
      Each band has five coefficients, each in int32 format in the order of b0,
      b1, b2, a1, a2. The total sequence length depends on the number of bands.
        - bx -- Numerator coefficients
        - ax -- Denominator coefficients
    - Sequence of int16 sNumShiftFactor
      One int16 per band; a numerator shift factor related to the Q factor of
      the filter coefficients. If the number of bands is odd, pad an extra value
      for alignment.

  @detdesc
    This structure has a field indicating the number of stages for filter and
    pregain, followed by variable-sized IIR filter structures. The size of these
    structures is defined by configurable parameters.
    @par
    Set the filter parameters before enabling the module so there will not be
    any audio quality issues because of a change in pregain from default to
    required values.
    @par
    The maximum size for a sidetone filter is 224 bytes. The maximum number of
    filter stages is assumed to be 10 (the original requirement was 5 stages).
    For an odd number of stages, 2 bytes of padding are required at the end of
    the payload.
*/
#define AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG                      0x00010204

typedef struct afe_sidetone_iir_filter_config_params_t afe_sidetone_iir_filter_config_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_sidetone_iir_filter_config_params_t
@{ */
/* Payload of the AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG parameter,
    which configures the sidetone IIR tuning filter module.
*/
struct afe_sidetone_iir_filter_config_params_t
{
    uint16_t                  num_biquad_stages;
    /**< Number of stages.

         @values Minimum of 5 and maximum of 10 */

    uint16_t                  pregain;
    /**< Pregain for the compensating filter response.

         @values Any number in Q13 format */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_sidetone_iir_filter_config_params_t */

/** @addtogroup afe_module_loopback
@{ */
/** ID of the Loopback module, which supports the following parameter IDs:
    - #AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH
    - #AFE_PARAM_ID_LOOPBACK_CONFIG

    @apr_hdr_fields
    Opcode -- AFE_MODULE_LOOPBACK @newpage
*/
#define AFE_MODULE_LOOPBACK                                           0x00010205

/** ID of the parameter used by #AFE_MODULE_LOOPBACK to get/set the loopback
    gain on a specified path.

    When multiple loopbacks are enabled, this parameter ID controls the gain
    of the loopback from the source port ID in the #AFE_PORT_CMD_SET_PARAM_V2
    to the destination port ID. Both #AFE_PORT_CMD_SET_PARAM_V2 and
    #AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH

    @msg_payload{afe_loopback_gain_per_path_param_t}
    @tablens{weak__afe__loopback__gain__per__path__param__t} @newpage
*/
#define AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH                         0x00010206

/** @} */ /* end_addtogroup afe_module_loopback */

typedef struct afe_loopback_gain_per_path_param_t afe_loopback_gain_per_path_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_loopback_gain_per_path_param_t
@{ */
/* Payload of the AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH parameter,
    which gets/sets loopback gain of a port to a Rx port. */
/** The Tx port ID of the loopback is part of the #AFE_PORT_CMD_SET_PARAM_V2
    command.
*/
struct afe_loopback_gain_per_path_param_t
{
   uint16_t                  rx_port_id;
   /**< Rx port of the loopback. */

   uint16_t                  gain;
   /**< Loopback gain per path of the port.

        @values Any number in Q13 format */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_loopback_gain_per_path_param_t */

/** @addtogroup afe_module_loopback
@{ */
/** ID of the parameter used by #AFE_MODULE_LOOPBACK to configure and
    enable/disable the loopback path.

    This parameter ID allows a Rx port to be configured as a source port on
    the loopback path. The port_id field in #AFE_PORT_CMD_SET_PARAM_V2 is the
    source port that can be designated as a Tx or Rx port. Both
	#AFE_PORT_CMD_SET_PARAM_V2 and #AFE_PORT_CMD_GET_PARAM_V2 can use this
	parameter ID.

    Also, this parameter ID can be used to configure the type of
    routing mode, which is useful for handling different use cases.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_LOOPBACK_CONFIG

    @msg_payload{afe_loopback_cfg_v1_t}
    @table{weak__afe__loopback__cfg__v1__t}
*/
#define AFE_PARAM_ID_LOOPBACK_CONFIG                                 0x0001020B

/** Version information used to handle future additions to the configuration
    interface (for backward compatibility).
*/
#define AFE_API_VERSION_LOOPBACK_CONFIG                                0x1

/** Data path type of the loopback routing mode.
*/
typedef enum afe_loopback_routing_mode_t
{
    LB_MODE_DEFAULT = 1,
    /**< Regular loopback from the source port to the destination port. */

    LB_MODE_SIDETONE,
    /**< Sidetone feed from the source port to the destination port.
         This field is supported only on the Tx port. */

    LB_MODE_EC_REF_VOICE_AUDIO,
    /**< Echo canceller reference: voice + audio + DTMF. This field is
         supported only on the Rx port. */

    LB_MODE_EC_REF_VOICE
    /**< Echo canceller reference: voice alone. Currently, this field is not
         supported. */

}/** @cond */ afe_loopback_routing_mode_t /** @endcond */ ;

/** @} */ /* end_addtogroup afe_module_loopback */

/* Structure for the AFE_PARAM_ID_LOOPBACK_CONFIG param. */
typedef struct afe_loopback_cfg_v1_t afe_loopback_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_loopback_cfg_v1_t
@{ */
/* Payload of the AFE_PARAM_ID_LOOPBACK_CONFIG parameter, which
    enables/disables one AFE loopback.
*/
struct afe_loopback_cfg_v1_t
{
    uint32_t                  loopback_cfg_minor_version;
    /**< Minor version used for tracking loopback module configuration.

         @values #AFE_API_VERSION_LOOPBACK_CONFIG */

    uint16_t                  dst_port_id;
    /**< Destination port ID.

         @values See <i>Hardware Ports</i> in Appendix @xref{sec:portIDs} */

    uint16_t                  routing_mode;
    /**< Specifies the data path type from the source to the destination port.

         @values
         - #LB_MODE_DEFAULT
         - #LB_MODE_SIDETONE
         - #LB_MODE_EC_REF_VOICE_AUDIO
         - #LB_MODE_EC_REF_VOICE @tablebulletend */

    uint16_t                  enable;
    /**< Specifies whether to enable (1) or disable (0) an AFE loopback. */

     uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_loopback_cfg_v1_t */

/** @addtogroup afe_module_rmc
@{ */
/** ID of the Remove Click Noise (RMC) module, which supports the following
    parameter IDs:
    - #AFE_PARAM_ID_RMC_CONFIG
    - #AFE_PARAM_ID_ENABLE

    @apr_hdr_fields
    Opcode -- AFE_MODULE_RMC

    @detdesc
    RMC module configuration and enabling is recommended when the AFE port is
    in the CONFIG state (i.e, after configuring the AFE port only). The module
    must be disabled only when the port is in the STOP state (i.e, after port
    STOP command only) to avoid errors or problems. @newpage
*/
#define AFE_MODULE_RMC                                               0x00010207

/** ID of the parameter used by #AFE_MODULE_RMC to get/set RMC configuration
    parameters. Both #AFE_PORT_CMD_SET_PARAM_V2 and #AFE_PORT_CMD_GET_PARAM_V2
	can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_RMC_CONFIG

    @msg_payload{afe_rmc_cfg_param_v1_t}
    @table{weak__afe__rmc__cfg__param__v1__t}
*/
#define AFE_PARAM_ID_RMC_CONFIG                                       0x00010208

/** Version information used to handle future additions to RMC module
    configuration (for backward compatibility).
*/
#define AFE_API_VERSION_RMC_CONFIG                                     0x1

/** @} */ /* end_addtogroup afe_module_rmc */

typedef struct afe_rmc_cfg_param_v1_t afe_rmc_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_rmc_cfg_param_v1_t
@{ */
/* Payload of the AFE_PARAM_ID_RMC_CONFIG parameter, which gets/sets
    RMC configuration parameters.
*/
struct afe_rmc_cfg_param_v1_t
{
   uint32_t                 rmc_minor_version;
   /**< Minor version used for tracking RMC module configuration.

        @values #AFE_API_VERSION_RMC_CONFIG */

   int16_t                  rmc_neg_on_set_detect_thresh;
   /**< Threshold used to trigger the start of negative spike tracking.

        @values 0 to 32767 */

   int16_t                  rmc_pos_on_set_detect_thresh;
   /**< Threshold used to trigger the start of positive spike tracking.

        @values 0 to 32767 */

   int16_t                  rmc_on_set_slope;
   /**< Ratio between x(n) and x(n-2) used to trigger the start of spike
        tracking.

        @values  1 to 32767 */

   int16_t                  rmc_neg_peak_detect_thresh;
   /**< Threshold used to determine the negative peak.

        @values 0 to 32767 */

   int16_t                  rmc_pos_peak_detect_thresh;
   /**< Threshold used to determine the positive peak.

        @values 0 to 32767 */

   int16_t                  rmc_neg_click_tail_thresh;
   /**< Threshold used to track the long smooth tail after the negative peak.

        @values 0 to 32767 */

   int16_t                  rmc_pos_click_tail_thresh;
   /**< Threshold used to track the long smooth tail after the positive peak.

        @values 0 to 32767 */

   uint16_t                 rmc_ramp_upDn_length;
   /**< Length of the gain ramping up/down.

        @values 2 to 48000 */

   uint16_t                 rmc_delay_length;
   /**< Delay buffer size used for look-ahead.

        @values 0 to 48000 */

   int16_t                  rmc_next_pos_on_set_thresh;
   /**< When still in negative spike detection and a positive spike arrives:
        threshold used to switch to positive spike detection.

        @values 0 to 32767 */

   int16_t                  rmc_next_pos_on_set_slope;
   /**< When still in negative spike detection and a positive spike arrives:
        ratio between x(n) and x(n-2) used to switch to positive spike
        detection.

        @values 1 to 32767 */

   int16_t                  rmc_gain_step_dB;
   /**< Gain step size used for gain ramping up/down.

        @values -11520 to 0 */

   uint32_t                 rmc_peak_length;
   /**< Length of the negative/positive peaks.

        @values 2 to 48000 */

   uint32_t                 rmc_click_tail_length;
   /**< Length of the negative/positive tail after peaks.

        @values 3 to 480000 */

   uint32_t                 rmc_total_neg_mute_length;
   /**< Length of the total muting time for a negative spike.

        @values 5 to 480000 */

   uint32_t                 rmc_total_pos_mute_length;
   /**< Length of the total muting time for a positive spike.

        @values 5 to 480000 */

   uint32_t                 rmc_next_pos_decay_length;
   /**< When still in negative spike detection and a positive spike arrives:
        length of the muting time that must be extended.

        @values 0 to 480000 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_rmc_cfg_param_v1_t */

/** @addtogroup afe_module_speaker_prot
@{ */
/** ID of the Speaker Protection module, which supports the following
    parameter IDs:
     - #AFE_PARAM_ID_SPKR_PROT_CONFIG
     - #AFE_PARAM_ID_ENABLE

    @apr_hdr_fields
    Opcode -- AFE_MODULE_SPEAKER_PROTECTION

    @detdesc
    The AFE does not cache the configuration information when stopping the
    port. The expected sequence is:
    @par
    -# Configure the port using #AFE_MODULE_AUDIO_DEV_INTERFACE.
    -# Configure and enable the speaker protection module using
       #AFE_PORT_CMD_SET_PARAM_V2 with the #AFE_MODULE_SPEAKER_PROTECTION
       module, including both parameters, #AFE_PARAM_ID_SPKR_PROT_CONFIG and
       #AFE_PARAM_ID_ENABLE.
    -# Start the port using #AFE_PORT_CMD_DEVICE_START.
    -# Stop the port when necessary using #AFE_PORT_CMD_DEVICE_STOP (the
       AFE destroys all the speaker protection-related information).
    @par
    This module is accepted only in the configuration state of the port (i.e.,
    after an AFE_MODULE_AUDIO_DEV_INTERFACE command and before an
    AFE_PORT_CMD_DEVICE_START command.
    @par
    The speaker protection module is supported only on a 16-bit mono or stereo
    Rx port running at a sampling rate of 48000 Hz. @newpage
*/
#define AFE_MODULE_SPEAKER_PROTECTION                               0x00010209

/** ID of the parameter used by #AFE_MODULE_SPEAKER_PROTECTION to configure
    for the speaker protection algorithm. Both #AFE_PORT_CMD_SET_PARAM_V2 and
    #AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_SPKR_PROT_CONFIG

    @msg_payload{afe_spkr_prot_cfg_param_v1_t}
    @table{weak__afe__spkr__prot__cfg__param__v1__t}
*/
#define AFE_PARAM_ID_SPKR_PROT_CONFIG                                 0x0001020a

/** Version information used to handle future additions to speaker protection
    configuration (for backward compatibility).
*/
#define AFE_API_VERSION_SPKR_PROT_CONFIG                            0x1

/** Length of the excursion transfer function. */
#define AFE_SPKR_PROT_EXCURSION_TF_LEN                                512

/** @} */ /* end_addtogroup afe_module_speaker_prot */

typedef struct afe_spkr_prot_cfg_param_v1_t afe_spkr_prot_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_spkr_prot_cfg_param_v1_t
@{ */
/* Configuration structure for the AFE_MODULE_SPEAKER_PROTECTION
    module (version 1).
*/
struct afe_spkr_prot_cfg_param_v1_t
{
   uint32_t       spkr_prot_minor_version;
   /**< Minor version used for tracking speaker protection module
        configuration.

        @values #AFE_API_VERSION_SPKR_PROT_CONFIG */

   int16_t        win_size;
   /**< Analysis and synthesis window size (nWinSize).

        @values 1024, 512, 256 samples */

   int16_t        margin;
   /**< Allowable margin for excursion prediction, in L16Q15 format. This is a
        control parameter to allow for overestimation of peak excursion. */

   int16_t        spkr_exc_limit;
   /**< Speaker excursion limit, in L16Q15 format. */

   int16_t        spkr_resonance_freq;
   /**< Resonance frequency of the speaker; used to define a frequency range
        for signal modification.

        @values 0 to 2000 Hz */

   int16_t        lim_thresh;
   /**< Threshold of the hard limiter; used to prevent overshooting beyond a
        signal level that was set by the limiter prior to speaker protection.

        @values 0 to 32767 */

   int16_t        hpf_cut_off_freq;
   /**< High pass filter cutoff frequency.

        @values 100, 200, 300 Hz */

   int16_t        hpf_enable;
   /**< Specifies whether the high pass filter is enabled (0) or disabled (1). */

   int16_t        reserved;
   /**< This field must be set to zero. */

   int32_t        amp_gain;
   /**< Amplifier gain in L32Q15 format. This is the RMS voltage at the
        loudspeaker when a 0dBFS tone is played in the digital domain. */

   int16_t        excursion_tf[AFE_SPKR_PROT_EXCURSION_TF_LEN];
   /**< Array for the excursion transfer function. The peak excursion of the
        loudspeaker diaphragm is measured in millimeters for 1 Vrms Sine tone
        at all FFT bin frequencies.

        @values Q15 format */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_spkr_prot_cfg_param_v1_t */

/** @addtogroup afe_module_aanc
@{ */
/** ID of the Adaptive ANC (AANC) module, which supports the following
    parameter IDs:
     - #AFE_PARAM_ID_AANC_PORT_CONFIG
     - #AFE_PARAM_ID_AANC_CFG1
     - #AFE_PARAM_ID_AANC_CFG2
     - #AFE_PARAM_ID_AANC_PROXIMITY_DETECTION
     - #AFE_PARAM_ID_SB_CODEC_REG_CFG
     - #AFE_PARAM_ID_ENABLE

    @apr_hdr_fields
    Opcode -- AFE_MODULE_AANC

    @detdesc
    The commands sent to this module are used to configure and enable the
    AANC algorithm in the aDSP. The algorithm will not be enabled until all
    the above parameters are passed via the #AFE_PORT_CMD_SET_PARAM_V2
    command.
    @par
    After the algorithm is enabled, only one of the following parameter IDs
    is allowed to be sent:
    @par
    - #AFE_PARAM_ID_AANC_CFG2
    - #AFE_PARAM_ID_ENABLE (disable) @newpage
*/
#define AFE_MODULE_AANC                   0x00010214

/** ID of the parameter used by #AFE_MODULE_AANC to configure the port for the
    Adaptive ANC algorithm. Only #AFE_PORT_CMD_SET_PARAM_V2 can use this
	parameter ID.

    @msg_payload{afe_aanc_port_cfg_param_v1_t}
    @table{weak__afe__aanc__port__cfg__param__v1__t}
*/
#define AFE_PARAM_ID_AANC_PORT_CONFIG   0x00010215

/** Version information used to handle future additions to Adaptive ANC
    configuration (for backward compatibility).
*/
#define AFE_API_VERSION_AANC_PORT_CONFIG 0x1

/** Enumeration for indicating if a channel is unused. */
#define AANC_TX_MIC_UNUSED              0

/** Enumeration for indicating if a channel contains voice samples. */
#define AANC_TX_VOICE_MIC               1

/** Enumeration for indicating if a channel contains error mic samples. */
#define AANC_TX_ERROR_MIC               2

/** Enumeration for indicating if a channel contains noise mic samples. */
#define AANC_TX_NOISE_MIC               3

/** Enumeration for setting the maximum number of channels per device. @newpage */
#define AFE_PORT_MAX_CHANNEL_CNT        0x8

/** @} */ /* end_addtogroup afe_module_aanc */

typedef struct afe_aanc_port_cfg_param_v1_t afe_aanc_port_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_aanc_port_cfg_param_v1_t
@{ */
/* Structure for the AFE_PARAM_ID_AANC_PORT_CONFIG parameter of
    AFE_MODULE_AANC.
*/
struct afe_aanc_port_cfg_param_v1_t
{
   uint32_t       aanc_port_cfg_minor_version;
   /**< Minor version used for tracking AANC module source port configuration.

        @values #AFE_API_VERSION_AANC_PORT_CONFIG */

   uint32_t       tx_port_sample_rate;
   /**< Sampling rate of the source Tx port.

        @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_192K @tablebulletend */

   uint8_t        tx_port_channel_map[AFE_PORT_MAX_CHANNEL_CNT];
   /**< Array of channel mapping for the Tx port signal carrying Noise (X),
        Error (E), and Voice (V) signals.

        @values
        - #AANC_TX_MIC_UNUSED
        - #AANC_TX_VOICE_MIC
        - #AANC_TX_ERROR_MIC
        - #AANC_TX_NOISE_MIC @tablebulletend */

   uint16_t       tx_port_num_channels;
   /**< Number of channels on the source Tx port. */

   uint16_t       rx_path_ref_port_id;
   /**< Port ID of the Rx path reference signal. (See <i>Hardware Ports</i>
        in Appendix @xhyperref{sec:portIDs,A}). */

   uint32_t       ref_port_sample_rate;
   /**< Sampling rate of the reference port.

        @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_192K @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_aanc_port_cfg_param_v1_t */

/** @addtogroup afe_module_aanc
@{ */
/** ID of the parameter used by #AFE_MODULE_AANC to configure static
    configuration parameters for the Adaptive ANC algorithm. Only
	#AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_AANC_CFG1

    @msg_payload{afe_aanc_cfg1_param_v1_t}
    @table{weak__afe__aanc__cfg1__param__v1__t}
*/
#define AFE_PARAM_ID_AANC_CFG1   0x00010216

/** Version information used to handle future additions to Adaptive ANC
    static configuration (for backward compatibility).
*/
#define AFE_API_VERSION_AANC_CFG1 0x1

/** Enumeration for indicating the maximum length of the IIR filter. */
#define ADAP_ANC_IIR_MAX_FILT_LEN         24

/** Enumeration for indicating the maximum length of the prefilter. @newpage */
#define ADAP_ANC_PREFILT_MAX_FILT_LEN     5

/** @} */ /* end_addtogroup afe_module_aanc */

typedef struct afe_aanc_cfg1_param_v1_t afe_aanc_cfg1_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_aanc_cfg1_param_v1_t
@{ */
/* Structure for the AFE_PARAM_ID_AANC_CFG1 parameter of
    AFE_MODULE_AANC.
*/
struct afe_aanc_cfg1_param_v1_t
{
   uint32_t       aanc_cfg1_minor_version;
   /**< Minor version used for tracking AANC module static
        configuration (CFG1).

        @values #AFE_API_VERSION_AANC_CFG1 */

   uint32_t       frame_size;
   /**< Input frame size for the voice, error, noise, and Rx path signal.

        @values 80, 160 ,240 */

   int32_t        ad_gain_default;
   /**< Default adaptive gain (Q3.13).

        @values -32768 to 32767 */

   uint16_t       ad_skip_k;
   /**< Skip K samples from the start of the frame.

        @values 0 to 240 */

   uint16_t       vad_gate_hist_len;
   /**< Length of the energy estimate history buffer (for minimum statistics).

        @values 1 to 1024 */

   int32_t        vad_gate_hist_init_val;
   /**< Initial value for the energy history buffer.

        @values 0 to 32767 */

   uint16_t       prefilt_biquad_iir_num_len;
   /**< Pre-filter biquad IIR filter; length of the numerator coefficient.

        @values 1 to 5 */

   uint16_t       prefilt_biquad_iir_deno_len;
   /**< Pre-filter biquad IIR filter; length of the denominator coefficient.

        @values 1 to 5 */

   int32_t        prefilt_biquad_iir_num[ADAP_ANC_PREFILT_MAX_FILT_LEN];
   /**< Pre-filter biquad IIR filter; numerator coefficient (Q2.14).

        @values -32768 to 32767 */

   int32_t        prefilt_biquad_iir_deno[ADAP_ANC_PREFILT_MAX_FILT_LEN];
   /**< Pre-filter biquad IIR filter; denominator coefficient (Q2.14).

        @values -32768 to 32767 */

   uint16_t       spath_biquad_iir_num_len;
   /**< Speaker transfer function biquad IIR filter; length of the numerator
        coefficient.

        @values 1 to 24 */

   uint16_t       spath_biquad_iir_deno_len;
   /**< Speaker transfer function biquad IIR filter; length of the denominator
        coefficient.

        @values 1 to 24 */

   int32_t        spath_biquad_iir_num[ADAP_ANC_IIR_MAX_FILT_LEN];
   /**< Speaker transfer function biquad IIR filter; numerator coefficient (Q.14)

        @values -32768 to 32767 */


   int32_t        spath_biquad_iir_deno[ADAP_ANC_IIR_MAX_FILT_LEN];
   /**< Speaker transfer function biquad IIR filter; denominator coefficient (Q2.14)

        @values -32768 to 32767 */

   uint16_t       wfilt_biquad_iir_num_len;
   /**< Adaptive Least Mean Square (LMS) biquad IIR filter; length of the
        numerator coefficient.

        @values 1 to 24 */

   uint16_t       wfilt_biquad_iir_deno_len;
   /**< Adaptive LMS biquad IIR filter; length of the denominator coefficient.

        @values 1 to 24 */

   int32_t        wfilt_biquad_iir_num[ADAP_ANC_IIR_MAX_FILT_LEN];
   /**< Adaptive LMS biquad IIR filter; numerator coefficient (Q2.14).

        @values -32768 to 32767 */

   int32_t        wfilt_biquad_iir_deno[ADAP_ANC_IIR_MAX_FILT_LEN];
   /**< Adaptive LMS biquad IIR filter; denominator coefficient (Q2.14).

        @values -32768 to 32767 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_aanc_cfg1_param_v1_t */

/** @addtogroup afe_module_aanc
@{ */
/** ID of the parameter used by #AFE_MODULE_AANC to set dynamic configuration
    parameters for the Adaptive ANC algorithm. Only #AFE_PORT_CMD_SET_PARAM_V2
	can use this parameter ID.

    @if OEM_only
    There are three versions of the payload.
    - Version 2 includes the block gate
    - Version 3 includes the addition of following new features such as
      max noise gate, wind gate and saturation gate. It also includes the
      feature improvement in noise gate and block gate.
    parameters.
    @endif

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_AANC_CFG2

    @msg_payload{afe_aanc_cfg2_param_v1_t}
    @table{weak__afe__aanc__cfg2__param__v1__t}

    @if OEM_only
    @msg_payload{afe_aanc_cfg2_param_v2_t}
    @tablens{weak__afe__aanc__cfg2__param__v2__t}

    @msg_payload{afe_aanc_cfg2_param_v3_t}
    @tablens{weak__afe__aanc__cfg2__param__v3__t}
    @endif
*/
#define AFE_PARAM_ID_AANC_CFG2   0x00010217

/** Version information used to handle future additions to Adaptive ANC
    dynamic configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_AANC_CFG2 0x3

/** @} */ /* end_addtogroup afe_module_aanc */

/**
  @brief Algorithm features, corresponding to bits in features word in wnd_gate_enable of afe_aanc_cfg2_param_v3_t
 */
#define ADANC_WND_GATE     (1 << 0)
#define ADANC_WND_DETECT   (1 << 1)

typedef struct afe_aanc_cfg2_param_v3_t afe_aanc_cfg2_param_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_aanc_cfg2_param_v3_t
@{ */
/* Structure for the AFE_PARAM_ID_AANC_CFG2 parameter of AFE_MODULE_AANC. This
   version 3 comprises of improvements in some features and addition of some new
   features as well. Following features have improvements:
      -  Noise Gate.
      -  Block Gate.

   Following are the new feature additions:
      -  Max Noise Gate.
      -  Saturation Gate.
      -  Proximity Detector.
      -  Wind Gate.
*/

struct afe_aanc_cfg2_param_v3_t
{
   uint32_t       aanc_cfg2_minor_version;
   /**< Minor version used for tracking AANC module dynamic
        configuration (CFG2).

        @values #AFE_API_VERSION_AANC_CFG2 */

   uint16_t       adapt_anc_enable;
   /**< Adaptive AANC is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       alc_enable;
   /**< Adaptive level controller is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       pre_filt_enable;
   /**< Prefilter is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       rx_factor_enable;
   /**< Rx energy factor is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       vad_factor_enable;
   /**< Voice activity detection factor is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       noise_gate_enable;
   /**< Noise gate is enabled (1) or disabled (0).

        @values 0, 1 */

   int32_t        ad_alpha;
   /**< Learning rate (Q8.8).

        @values -32768 to 32767 */

   int32_t        ad_delta_g2_bound;
   /**< Absolute maximum of delta G2 (Q2.14).

        @values 0 to 32767 */

   int32_t        ad_gain_bound[2];
   /**< Minimum and maximum values for the adaptive gain (Q3.13).

        @values -32768 to 32767 */

   int32_t        alc_cfg_sm_beta;
   /**< Adaptive level controller smoothing beta (Q1.15).

        @values 0 to 32767 */

   int32_t        alc_cfg_ftarget;
   /**< Adaptive level controller gain curve target (Q1.15).

        @values 0 to 32767 */

   int32_t        alc_cfg_fthresh;
   /**< Adaptive level controller gain curve threshold (Q1.15).

        @values 0 to 32767 */

   int32_t        rx_gate_cfg_sm_beta;
   /**< Rx gate smoothing beta (Q1.15).

        @values 0 to 32767 */

   int32_t        rx_gate_cfg_fthresh1;
   /**< Rx gate curve threshold 1 (Q1.15).

        @values 0 to 32767 */

   int32_t        rx_gate_cfg_fthresh2;
   /**< Rx gate curve threshold 2 (Q1.15).

        @values 0 to 32767 */

   int32_t        vad_gate_cfg_fthresh1;
   /**< Voice activity detection gate curve threshold 1 (Q8.8).

        @values 0 to 32767 */

   int32_t        vad_gate_cfg_fthresh2;
   /**< Voice activity detection gate curve threshold 2 (Q8.8).

        @values 0 to 32767 */

   int32_t        noise_gate_cfg_thresh;
   /**< Noise gate threshold (Q1.15).

        @values 0 to 32767 */

   int32_t        noise_gate_cfg_trstep;
   /**< Noise gate transition step (Q1.15).

        @values 0 to 32767 */

   uint16_t       block_gate_enable;
   /**< Block gate is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       block_offset_enable;
   /**< Block offset is enabled (1) or disabled (0).

        @values 0, 1 */

   int32_t        block_gate_cfg_sm_beta;
   /**< Block gate smoothing beta (Q1.15)

        @values 0 to 32767 */

   int32_t        block_gate_cfg_discount_factor;
   /**< Block gate discount factor (Q1.15)

        @values -32768 to 32767 */

   int32_t        block_gate_cfg_weight_factor;
   /**< Block gate weighting factor (Q1.15)

        @values -32768 to 32767 */

   int32_t        block_gate_cfg_rx_vad_threshold;
   /**< Block gate Rx VAD threshold (Q1.15)

        @values 0 to 32767 */

   int32_t        block_gate_cfg_rx_offset_factor;
   /**< Factor for generating an offset (Q1.15)

        @values -32768 to 32767 */

   int32_t        noise_gate_cfg_thresh2;
   /**< Noise gate threshold  (Q1.15).

        @values -32768 to 32767 */

   int32_t        noise_gate_cfg_thresh2targetgain;
   /**< Safe target gain (Q3.13)

        @values -32768 to 32767 */

   int32_t        block_gate_cfg_discount_gain_threshold;
   /**< Threshold on anc gain(Q.13) that determines dynamic discount factor.

        @values -32768 to 32767 */

   int32_t        block_gate_cfg_err_limit_factor;
   /**< Block gate Safe target gain (Q3.13).

        @values -32768 to 32767 */

   int32_t        wnd_gate_enable;
   /**< Wind noise detect feature bits on/off,default gate off detect on.
        Bit 0 for enabling gate, bit 1 for enabling detection.

        @values 0, ADANC_WND_GATE, ADANC_WND_DETECT, (ADANC_WND_GATE | ADANC_WND_DETECT)*/

   int32_t        wnd_tran_step;
   /**< wind noise transition step Q2.14..

        @values -32768 to 32767 */

   int32_t        wnd_min_gain;
   /**< Minimum gain when WND is detected (Q3.13).

        @values -32768 to 32767 */

   int32_t        wnd_hangover;
   /**< hangover before declaring no wind after a detection.

        @values 0 to 2147483647 */

   int32_t        wnd_sec_mic;
   /**< choice of secondary mic for wind noise detection.

        @values 0 to 2 */

   int32_t        wnd_cov_eps;
   /**< a small initialzation constant for the covariance, per sample.

        @values 0 to 2147483647 */

   int32_t        wnd_det_th[2];
   /**< lower and upper thresholds for windMas.

        @values 0 to 2147483647 */

   int32_t        wnd_detect_thresh_l32q31;
   /**< wind detection threshold.

        @values 0 to 2147483647 */

   int32_t        wnd_silent_thresh_l32q0;
   /**< wind silent threshold

        @values 0 to 2147483647 */

   int16_t        wnd_c_alph;
   /**< smooth alph for cov statistics

        @values 0 to 32767 */

   int16_t        wnd_cov_bit_shf;
   /**< R bit shift for corr before converting to int32

        @values -32 to 62 */

   int16_t        wnd_ws_alph_inc;
   /**< smooth alph for windscore, when wind increases.

        @values 0 to 32767 */

   int16_t        wnd_ws_alph_dec;
   /**< smooth alph for windscore, when wind decreases.

        @values 0 to 32767 */

   int16_t        wnd_boost_s16q10;
   /**< boosting self std for wind determination

        @values 0 to 32767 */

   int16_t        wnd_fact_s16q15;
   /**< percentage of low freq std in wind discrimination.

        @values 0 to 32767 */

   int32_t        sat_gate_enable;
   /**< SAT gate on/off switch.

        @values 0, 1 */

   int32_t        sat_gate_cfg_pcm_thresh;
   /**< Threshold for PCM sample before saturation (usually 2^15-1).

        @values 0 to 65535 */

   int32_t        sat_gate_cfg_count_thresh;
   /**< Threshold count to determine if saturation happens.

        @values 0 to 2147483647 */

   int32_t        sat_gate_cfg_gain_thresh;
   /**< Threshold on gain to determine if saturation happens

        @values 0 to 32767 */

   int32_t        sat_gate_cfg_hangover_len;
   /**< Length of Hangover frames when Saturation Detected.

        @values 0 to 2147483647 */

   int32_t        sat_gate_cfg_trans_step;
   /**< Gain Reduction Delta when SAT detected (Q2.14).

        @values 0 to 2147483647 */

   int32_t        sat_gate_cfg_min_gain;
   /**< Lower Bound for the Gain Transition Traget when SAT detected (Q3.13)

        @values -32768 to 32767 */

   int32_t        prox_gate_enable;
   /**< flag (0/1 == false/true == off/on)to enable proximity detection feature.

        @values 0, 1 */

   int32_t        prox_gate_trans_step;
   /**< Gain Reduction Delta when proximity is not detected (Q2.14).

        @values 0 to 2147483647 */

   int32_t        prox_gate_min_gain;
   /**< Lower limit for gain reduction (3.13).

        @values -32768 to 32767 */

   int32_t        prox_gate_trigger_hangover;
   /**< hangover before proximity detector flag is accepted.

        @values 0 to 2147483647 */
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_weakgroup weak_afe_aanc_cfg2_param_v3_t */


typedef struct afe_aanc_cfg2_param_v2_t afe_aanc_cfg2_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_aanc_cfg2_param_v2_t
@{ */
/* Structure for the Adaptive ANC dynamic configuration parameters. */
/** This version 2 comprises version 1 plus the block gate parameters.
*/
struct afe_aanc_cfg2_param_v2_t
{
   uint32_t       aanc_cfg2_minor_version;
   /**< Minor version used for tracking AANC module dynamic
        configuration (CFG2).

        @values #AFE_API_VERSION_AANC_CFG2 */

   uint16_t       adapt_anc_enable;
   /**< Adaptive AANC is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       alc_enable;
   /**< Adaptive level controller is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       pre_filt_enable;
   /**< Prefilter is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       rx_factor_enable;
   /**< Rx energy factor is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       vad_factor_enable;
   /**< Voice activity detection factor is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       noise_gate_enable;
   /**< Noise gate is enabled (1) or disabled (0).

        @values 0, 1 */

   int32_t        ad_alpha;
   /**< Learning rate (Q8.8).

        @values -32768 to 32767 */

   int32_t        ad_delta_g2_bound;
   /**< Absolute maximum of delta G2 (Q2.14).

        @values 0 to 32767 */

   int32_t        ad_gain_bound[2];
   /**< Minimum and maximum values for the adaptive gain (Q3.13).

        @values -32768 to 32767 */

   int32_t        alc_cfg_sm_beta;
   /**< Adaptive level controller smoothing beta (Q1.15).

        @values 0 to 32767 */

   int32_t        alc_cfg_ftarget;
   /**< Adaptive level controller gain curve target (Q1.15).

        @values 0 to 32767 */

   int32_t        alc_cfg_fthresh;
   /**< Adaptive level controller gain curve threshold (Q1.15).

        @values 0 to 32767 */

   int32_t        rx_gate_cfg_sm_beta;
   /**< Rx gate smoothing beta (Q1.15).

        @values 0 to 32767 */

   int32_t        rx_gate_cfg_fthresh1;
   /**< Rx gate curve threshold 1 (Q1.15).

        @values 0 to 32767 */

   int32_t        rx_gate_cfg_fthresh2;
   /**< Rx gate curve threshold 2 (Q1.15).

        @values 0 to 32767 */

   int32_t        vad_gate_cfg_fthresh1;
   /**< Voice activity detection gate curve threshold 1 (Q8.8).

        @values 0 to 32767 */

   int32_t        vad_gate_cfg_fthresh2;
   /**< Voice activity detection gate curve threshold 2 (Q8.8).

        @values 0 to 32767 */

   int32_t        noise_gate_cfg_thresh;
   /**< Noise gate threshold (Q1.15).

        @values 0 to 32767 */

   int32_t        noise_gate_cfg_trstep;
   /**< Noise gate transition step (Q1.15).

        @values 0 to 32767 */

   uint16_t       block_gate_enable;
   /**< Block gate is enabled (1) or disabled (0). */

   uint16_t       block_offset_enable;
   /**< Block offset is enabled (1) or disabled (0). */

   int32_t        block_gate_cfg_sm_beta;
   /**< Block gate smoothing beta (Q1.15)

        @values 0 to 32767 */

   int32_t        block_gate_cfg_discount_factor;
   /**< Block gate discount factor (Q1.15)

        @values -32768 to 32767 */

   int32_t        block_gate_cfg_weight_factor;
   /**< Block gate weighting factor (Q1.15)

        @values -32768 to 32767 */

   int32_t        block_gate_cfg_rx_vad_threshold;
   /**< Block gate Rx VAD threshold (Q1.15)

        @values 0 to 32767 */

   int32_t        block_gate_cfg_rx_offset_factor;
   /**< Factor for generating an offset (Q1.15)

        @values -32768 to 32767 */
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_weakgroup weak_afe_aanc_cfg2_param_v2_t */


typedef struct afe_aanc_cfg2_param_v1_t afe_aanc_cfg2_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_aanc_cfg2_param_v1_t
@{ */
/* Structure for the AFE_PARAM_ID_AANC_CFG2 parameter of
    AFE_MODULE_AANC.
*/
struct afe_aanc_cfg2_param_v1_t
{
   uint32_t       aanc_cfg2_minor_version;
   /**< Minor version used for tracking AANC module dynamic
        configuration (CFG2).

        @values #AFE_API_VERSION_AANC_CFG2 */

   uint16_t       adapt_anc_enable;
   /**< Adaptive AANC is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       alc_enable;
   /**< Adaptive level controller is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       pre_filt_enable;
   /**< Prefilter is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       rx_factor_enable;
   /**< Rx energy factor is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       vad_factor_enable;
   /**< Voice activity detection factor is enabled (1) or disabled (0).

        @values 0, 1 */

   uint16_t       noise_gate_enable;
   /**< Noise gate is enabled (1) or disabled (0).

        @values 0, 1 */


   int32_t        ad_alpha;
   /**< Learning rate (Q8.8).

        @values -32768 to 32767 */

   int32_t        ad_delta_g2_bound;
   /**< Absolute maximum of delta G2 (Q2.14).

        @values 0 to 32767 */

   int32_t        ad_gain_bound[2];
   /**< Minimum and maximum values for the adaptive gain (Q3.13).

        @values -32768 to 32767 */

   int32_t        alc_cfg_sm_beta;
   /**< Adaptive level controller smoothing beta (Q1.15).

        @values 0 to 32767 */

   int32_t        alc_cfg_ftarget;
   /**< Adaptive level controller gain curve target (Q1.15).

        @values 0 to 32767 */

   int32_t        alc_cfg_fthresh;
   /**< Adaptive level controller gain curve threshold (Q1.15).

        @values 0 to 32767 */

   int32_t        rx_gate_cfg_sm_beta;
   /**< Rx gate smoothing beta (Q1.15).

        @values 0 to 32767 */

   int32_t        rx_gate_cfg_fthresh1;
   /**< Rx gate curve threshold 1 (Q1.15).

        @values 0 to 32767 */

   int32_t        rx_gate_cfg_fthresh2;
   /**< Rx gate curve threshold 2 (Q1.15).

        @values 0 to 32767 */

   int32_t        vad_gate_cfg_fthresh1;
   /**< Voice activity detection gate curve threshold 1 (Q8.8).

        @values 0 to 32767 */

   int32_t        vad_gate_cfg_fthresh2;
   /**< Voice activity detection gate curve threshold 2 (Q8.8).

        @values 0 to 32767 */

   int32_t        noise_gate_cfg_thresh;
   /**< Noise gate threshold (Q1.15).

        @values 0 to 32767 */

   int32_t        noise_gate_cfg_trstep;
   /**< Noise gate transition step (Q1.15).

        @values 0 to 32767 */
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_weakgroup weak_afe_aanc_cfg2_param_v1_t */


/** @addtogroup afe_module_aanc
@{ */
/** ID of the parameter used by #AFE_MODULE_AANC to configure proximity sensor
    information at runtime. Only #AFE_PORT_CMD_SET_PARAM_V2 can use this
    parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_AANC_PROXIMITY_DETECTION

    @msg_payload{afe_aanc_proxy_det_cfg_param_v1_t }
    @table{weak_afe_aanc_proxy_det_cfg_param_v1_t }
*/


#define AFE_PARAM_ID_AANC_PROXIMITY_DETECTION   0x00010247


/** Version information used to handle future additions to proximity detection
    parameters configuration (for backward compatibility).
*/

#define AFE_API_VERSION_AANC_PROXIMITY_DETECTION   0x1


/** @} */ /* end_addtogroup afe_module_aanc */

typedef struct afe_aanc_proxy_det_cfg_param_v1_t afe_aanc_proxy_det_cfg_param_v1_t;

/** @brief Structure for the adap ANC proximity dynamic config parameters.
*/

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_aanc_prox_det_t
@{ */
/* Structure for the AFE_PARAM_ID_AANC_PROXIMITY_DETECTION parameter of
   AFE_MODULE_AANC. This structure provides information to AANC algorithm
   regarding Proximity Sensor Status.
*/

struct afe_aanc_proxy_det_cfg_param_v1_t
{
   uint32_t    aanc_prox_det_minor_version;
   /**< Minor version used for tracking AANC module proximity detection
        configuration.

        @values #AFE_API_VERSION_AANC_PROXIMITY_DETECTION */

   int32_t     prox_detect_flag;
   /**< 0/1 should be set to 1 when device next to ear, flag used only if prox_gate_enable is true.

        @values 0, 1 */
}
#include "adsp_end_pack.h"
;

/** @} */ /* end_weakgroup weak_afe_aanc_prox_det_t */

/** @addtogroup afe_module_aanc
@{ */
/** ID of the parameter used by #AFE_MODULE_AANC to log AANC algo tuning variables.

	This parameter ID is currently used for logging the algorithm internal
	variables.

  @msg_payload{afe_aanc_stats_v1_t}
    @table{weak__afe__aanc__stats__v1__t}

    @if OEM_only
    @par Algorithm logging data (afe_afe_algo_log_header_t)
    @tablens{weak__afe__afe__algo__log__header__t}
    @endif
*/
#define AFE_PARAM_ID_AANC_STATS						0x0001024A

/** Version information used to handle future additions to AANC
    statistics (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_AANC_STATS       					0x1

#define AFE_AANC_WND_MAX_CHANNELS							2

typedef struct afe_aanc_stats_v1_t afe_aanc_stats_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_aanc_stats_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_AANC_STATS parameter (version 1).
*/
struct afe_aanc_stats_v1_t
{
	   uint32_t       aanc_stats_minor_version;
	   /** Minor version used for tracking AANC
	        statistics. Currently, the supported version is 1.

	        @values #AFE_API_VERSION_AANC_STATS */

	   uint32_t        frame_duration_in_ms;
	   /** Frame duration in milliseconds for AANC variables.

	        @values Valid uint32 number */

	   uint32_t        frame_num;
	   /** Frame number of the AANC variables. This field is reset to zero
	        after crossing the maximum limit for uint32 variables.

	        @values Valid uint32 number */

	   int32_t        ad_gain;
	   /** adaptive gain

	        @values Valid int32 number in Q3.13 format*/

	   int32_t        delta_g;
	   /** delta G from adaptive algorithm

	        @values Valid int32 number in Q3.13 format*/

	   int32_t        out_gain;
	   /** output gain = adaptive gain * Noise gate Factor

	        @values Valid int32 number in Q3.13 format*/

	   int32_t        alc_data_sm_peak_x;
	   /** ALC smoothed signal peak

       @values Valid int32 number in Q1.15 format*/

	   int32_t        alc_data_gain_x;
	   /** ALC Gain

	        @values Valid uint32 number */

	   int32_t        rx_gate_data_sm_peak_x;
	   /** RxGate Smoothed signal peak

	        @values Valid int32 number in Q1.15 format*/

	   int32_t        rx_gate_data_factor;
	   /** RxPE learning rate gate factor

	        @values Valid int32 number in Q1.15 format*/

	   int32_t        vad_gate_data_detect_ratio;
	   /** Voice detect ration

	        @values Valid int32 number in Q8.8 format*/

	   int32_t        vad_gate_data_factor;
	   /** learning rate gate factor

	        @values Valid int32 number in Q1.15 format*/

	   int32_t        vad_gate_data_min_eng;
	   /** minimum energy for noise estimate

	        @values Valid int32 number */

	   int32_t        vad_gate_data_cur_eng;
	   /** current energy for noise estimate

	        @values Valid int32 number */

	   int32_t        noise_gate_data_nng_state;
	   /** noise gate state

	        @values Valid int32 number */

	   int32_t        noise_gate_data_nng_factor;
	   /** factor multiplied with ANC gain to generate finalGain

	        @values Valid int32 number in Q3.13 format*/

	   int32_t        block_gate_data_est_x;
	   /** peak estimate on X

	        @values Valid int32 number in Q1.15 format*/

	   int32_t        block_gate_data_est_e;
	   /** peak estimate on E

	        @values Valid int32 number in Q1.15 format*/

	   int32_t        block_gate_data_sm_est_x;
	   /** smootheded peak estimate on X

	        @values Valid int32 number in Q1.30 format*/

	   int32_t        block_gate_data_sm_est_e;
	   /** smootheded peak estimate on E

	        @values Valid int32 number in Q1.30 format*/

	   int32_t        block_gate_data_gate_delta;
	   /** delta factor to be added to FxLMS

	        @values Valid int32 number in Q.14 format*/

	   int32_t        block_gate_data_gate_offset;
	   /** offset proportional to estimate X & ratio

	        @values Valid int32 number in Q.14 format*/

	   int32_t        sat_gate_data_nsg_sat_count;
	   /** counted number of saturations in current frame

	        @values Valid int32 number */

	   int32_t        sat_gate_data_nsg_sat_hangover;
	   /** hangover counter (positive if Sat gate is active)

	        @values Valid int32 number */

	   int32_t        wnd_gate_data_wnd_state;
	   /** Wind Gate state

	        @values Valid int32 number */

	   int32_t        prox_gate_cfg_prox_detect_flag;
	   /** Proximity detect flag

	        @values Valid int32 number */

	   int32_t        prox_gate_data_prox_state;
	   /** proximity state

	        @values Valid int32 number */

	   int32_t        prox_gate_data_prox_hangover_count;
	   /** proximity gate Handover Count

	        @values Valid int32 number */

	   int32_t        std[AFE_AANC_WND_MAX_CHANNELS];
	   /** std of lp data

	        @values Valid int32 number */

	   int32_t        var_f[AFE_AANC_WND_MAX_CHANNELS];
	   /** non-lowpass varaince

	        @values Valid int32 number */

	   int32_t        std_f[AFE_AANC_WND_MAX_CHANNELS];
	   /** non-lowpass std

	        @values Valid int32 number */

	   int32_t        std_o[AFE_AANC_WND_MAX_CHANNELS];
	   /** lowest std of other channel (lowpassed)

	        @values Valid int32 number */

	   int32_t        wind_meas[AFE_AANC_WND_MAX_CHANNELS];
	   /** raw wind strength measure

	        @values Valid int32 number */

	   int32_t        wind_sc_q30[AFE_AANC_WND_MAX_CHANNELS];
	   /** wind score

	        @values Valid int32 number in Q1.30 format*/

	   int16_t        pri_wind_1q14[AFE_AANC_WND_MAX_CHANNELS];
	   /** likelihood channel is the one with wind, criteria 1

	        @values Valid uint16 number */

	   int16_t        pri_wind_2q14[AFE_AANC_WND_MAX_CHANNELS];
	   /** likelihood channel is the one with wind, criteria 2

	        @values Valid int16 number */

	   int16_t        pri_wind_q15[AFE_AANC_WND_MAX_CHANNELS];
	   /** likelihood channel is the one with wind

	        @values Valid int16 number */

	   int32_t        wind_sc_adj_q30[AFE_AANC_WND_MAX_CHANNELS];
	   /** wind score, adjusted for all bad correlaton

	        @values Valid int16 number in Q1.30 format*/

	   int32_t        wind_sc_adj_sm_q30[AFE_AANC_WND_MAX_CHANNELS];
	   /** wind score, adjusted for all bad correlaton, smoothed

	        @values Valid int32 number in Q1.30 format*/

	   int16_t        wnd_gate_data_wind_detect_flag;
	   /** Wind detection flag

	        @values Valid int16 number */

	   int16_t        wnd_channels;
	   /** Number of wnd channels

	        @values Valid int16 number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_aanc_stats_v1_t */

/** @addtogroup afe_module_aanc
@{ */
/** ID of the parameter used by #AFE_MODULE_AANC to configure codec registers
    through the SLIMbus interface. Only #AFE_PORT_CMD_SET_PARAM_V2 can use this
	parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_SB_CODEC_REG_CFG

    @msg_payload{afe_sb_codec_reg_cfg_v1_t}
    @table{weak__afe__sb__codec__reg__cfg__v1__t}
*/
#define AFE_PARAM_ID_SB_CODEC_REG_CFG   0x00010218

/** Version information used to handle future additions to hardware codec
    register configuration (for backward compatibility).
*/
#define AFE_API_VERSION_SB_CODEC_REG_CFG 0x1

/** Enumeration for the AANC_REG_LPF_FF_FB (CDC_ANCn_LPF_B1_CTL) register. */
#define AANC_REG_LPF_FF_FB               0x1

/** Enumeration for the AANC_REG_LPF_COEFF_MSB (CDC_ANCn_LPF_B2_CTL) register. */
#define AANC_REG_LPF_COEFF_MSB           0x2

/** Enumeration for the AANC_REG_LPF_COEFF_LSB (CDC_ANCn_LPF_B3_CTL) register. */
#define AANC_REG_LPF_COEFF_LSB           0x3

/** @} */ /* end_addtogroup afe_module_aanc */

typedef struct afe_sb_codec_reg_cfg_v1_t afe_sb_codec_reg_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_sb_codec_reg_cfg_v1_t
@{ */
/* Structure for AFE_PARAM_ID_SB_CODEC_REG_CFG parameter through SLIMbus
    interface; used by AFE_MODULE_AANC.
*/
struct afe_sb_codec_reg_cfg_v1_t
{
   uint32_t sb_ve_cfg_minor_version;
   /**< Minor version used for tracking SLIMbus codec register
        configuration.

        @values #AFE_API_VERSION_SB_CODEC_REG_CFG */

   uint32_t device_enum_addr_lsw;
   /**< Lower 32 bits of the SLIMbus slave device's 48-bit enumeration
        address. */

   uint32_t device_enum_addr_msw;
   /**< Upper 16 bits of the SLIMbus slave device's 48-bit enumeration
        address.

        @values
        - Bits 15 to 0 contain device_enum_addr_msw.
        - Bits 31 to 16 must be set to zero. @tablebulletend */

   uint32_t reg_logical_addr;
   /**< Logical address of the codec register to/from which the aDSP will
        write/read. */

   uint32_t reg_field_type;
   /**< Type of register field that points to this codec register read/write
        operation.

        @values
        - #AANC_REG_LPF_FF_FB
        - #AANC_REG_LPF_COEFF_MSB
        - #AANC_REG_LPF_COEFF_LSB @tablebulletend */

   uint32_t reg_field_bit_mask;
   /**< Register bitmask to specify the bitfields that must be accessed from
        the aDSP through this read/write operation. */

   uint16_t reg_bit_width;
   /**< Bit width of the register.

        @values 8, 16, 32 */

   uint16_t reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_sb_codec_reg_cfg_v1_t */

/** @ingroup afe_cmd_register_rt_port_dr
    Registers a real-time port driver with the AFE service for a given port.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER

  @msg_payload{afe_service_cmd_register_rt_port_driver_t}
    @table{weak__afe__service__cmd__register__rt__port__driver__t}

  @detdesc
    The APR destination address/port number are obtained from the header of
    this packet and are used for sending different events.
    @par
    By registering, the customer applications will receive
    #AFE_EVENT_RT_PROXY_PORT_STATUS events indicating when to start and stop
    the PCM exchange flow.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
*/
#define AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER                     0x000100E0

/* Structure for the AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER command. */
typedef struct afe_service_cmd_register_rt_port_driver_t    afe_service_cmd_register_rt_port_driver_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmd_register_rt_port_driver_t
@{ */
/* Payload of the AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER command,
    which registers a real-time port driver with the AFE service.
*/
struct afe_service_cmd_register_rt_port_driver_t
{
    uint16_t                  port_id;
    /**< Port ID with which the real-time driver exchanges data (registers for
         events).

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmd_register_rt_port_driver_t */

/** @ingroup afe_cmd_unregister_rt_port_dr
    Unregisters a real-time port driver from the AFE service for a given port.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER

  @msg_payload{afe_service_cmd_unregister_rt_port_driver_t}
    @table{weak__afe__service__cmd__unregister__rt__port__driver__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER must have been successfully
    processed.
*/
#define AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER                      0x000100E1

/* Structure for the AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER command. */
typedef struct afe_service_cmd_unregister_rt_port_driver_t    afe_service_cmd_unregister_rt_port_driver_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmd_unregister_rt_port_driver_t
@{ */
/* Payload of the AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER command,
    which unregisters a real-time port driver from the AFE service.
*/
struct afe_service_cmd_unregister_rt_port_driver_t
{
    uint16_t                  port_id;
    /**< Port ID from which the real-time driver unregisters for events.

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmd_unregister_rt_port_driver_t */

/** @addtogroup afe_event_rt_proxy_port_status
@{ */
/** Sent when an event is reported from the AFE service to a real-time driver.

  @apr_hdr_fields
    Opcode -- AFE_EVENT_RT_PROXY_PORT_STATUS

  @msg_payload{afe_event_rt_proxy_port_status_t}
    @table{weak__afe__event__rt__proxy__port__status__t}

  @detdesc
    This event communicates information about the real-time proxy port to the
    client. A Start or Stop event indicates that data transfer begins or stops.
    For a client to receive this notification, it must register using
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER.
    @par
    For a Start/Stop event, and depending on the value of port_id, the client
    starts/stops sending either filled buffers when the port_id is set for the
    mic path or empty buffers when the port_id is set for the speaker path.
    After acknowledging the Port Stop command, all buffers are returned by the
    AFE service.
    @par
    Low and high watermark events are sent based on the values of the
    corresponding thresholds configured using the using the
    #AFE_PARAM_ID_RT_PROXY_CONFIG parameter ID in the
    #AFE_MODULE_AUDIO_DEV_INTERFACE
    module.
    @par
    - In the source path, a low/high watermark event is sent when the application
      is too slow/fast in sending data.
    - In the sink path, a low watermark event indicates the application is
      consuming data at a faster rate than the AFE is getting from the Rx path,
      and a high watermark event indicates that the application is consuming
      slowly.
    @par
    @keep{4}
    Although these events can be used to adjust the rate at which applications
    produce or consume data, that is not the goal. These events are added for
    debugging, and they should not be used for non-real-time applications.

  @return
    None.

  @dependencies
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER
*/
#define AFE_EVENT_RT_PROXY_PORT_STATUS                              0x00010105

/** Type of event sent when a real-time proxy port is started. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_START                           0

/** Type of event sent when a real-time proxy port is stopped. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_STOP                            1

/** Type of event sent when a low watermark is reached. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_LOW_WATER_MARK                  2

/** Type of event sent when a high watermark is reached. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_HIGH_WATER_MARK                 3

/** Invalid event. */
#define AFE_EVENT_TYPE_RT_PROXY_PORT_INVALID                         0xFFFF

/** @} */ /* end_addtogroup event_rt_proxy_port_status */

/* Structure for the AFE_EVENT_RT_PROXY_PORT_STATUS event. */
typedef struct afe_event_rt_proxy_port_status_t  afe_event_rt_proxy_port_status_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_event_rt_proxy_port_status_t
@{ */
/* Payload of the AFE_EVENT_RT_PROXY_PORT_STATUS message, which sends
    an event from the AFE service to a registered client.
*/
struct afe_event_rt_proxy_port_status_t
{
    uint16_t                  port_id;
    /**< Port ID to which the event is sent.

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END */

    uint16_t                  event_type;
    /**< Type of event.

         @values
         - #AFE_EVENT_TYPE_RT_PROXY_PORT_START
         - #AFE_EVENT_TYPE_RT_PROXY_PORT_STOP
         - #AFE_EVENT_TYPE_RT_PROXY_PORT_LOW_WATER_MARK
         - #AFE_EVENT_TYPE_RT_PROXY_PORT_HIGH_WATER_MARK @vertspace{-28} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_event_rt_proxy_port_status_t */

/** @ingroup afe_cmd_rt_proxy_port_write
    Delivers a data buffer to the AFE service from a real-time device.

  @apr_hdr_fields
    Opcode -- AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2

  @msg_payload{afe_port_data_cmd_rt_proxy_port_write_v2_t}
    @table{weak__afe__port__data__cmd__rt__proxy__port__write__v2__t}

  @detdesc
    This command is used by the client for sending the data obtained from the
    source device to the AFE. An acknowledgment is sent immediately after
    copying the data to the circular buffer.
    @par
    If incoming data does not fit in the circular buffer due to overflow, it
    overwrites a portion of the data in the existing buffer, and that data is
    lost.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER -- The following commands are to be
    sent with the port_id set as a source (Tx) real-time proxy port ID:
    @par
    - #AFE_PARAM_ID_RT_PROXY_CONFIG through #AFE_MODULE_AUDIO_DEV_INTERFACE
    - #AFE_PORT_CMD_DEVICE_START
    @par
    The physical buffer must be fully contained in a memory region that has
    been mapped to the AFE service via the
    #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command.
*/
#define AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2                    0x000100ED

/* Structure for the AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2 command. */
typedef struct afe_port_data_cmd_rt_proxy_port_write_v2_t  afe_port_data_cmd_rt_proxy_port_write_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_data_cmd_rt_proxy_port_write_v2_t
@{ */
/* Payload of the AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2 command, which
    delivers a filled buffer to the AFE service.
*/
struct afe_port_data_cmd_rt_proxy_port_write_v2_t
{
    uint16_t                  port_id;
    /**< Tx (mic) proxy port ID with which the real-time driver exchanges data.

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END */

    uint16_t                  reserved;
    /**< This field must be set to zero. */

    uint32_t                  buffer_address_lsw;
    /**< Lower 32 bits of the 64-bit address buffer containing the data from
         the real-time source device on a client. */

    uint32_t                  buffer_address_msw;
    /**< Upper 32 bits of the 64-bit address buffer containing the data from
         the real-time source device on a client. */

    uint32_t                   mem_map_handle;
    /**< Unique identifier for an address.

         @values Any 32-bit value

         This memory map handle is returned by the aDSP through the
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command. */

    uint32_t                  available_bytes;
    /**< Number of valid bytes available in the buffer (including all
         channels: number of bytes per channel = availableBytes/numChannels).

         @values > 0

         This field must be equal to the frame size specified in the
         #AFE_MODULE_AUDIO_DEV_INTERFACE command that was sent to configure this
         port. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup afe_port_data_cmd_rt_proxy_port_write_v2_t */

/** @ingroup afe_cmd_rt_proxy_port_read
    Delivers an empty data buffer to the AFE service to be filled and sent to a
    real-time device.

  @apr_hdr_fields
    Opcode -- AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ

  @msg_payload{afe_port_data_cmd_rt_proxy_port_read_v2_t}
    @table{weak__afe__port__data__cmd__rt__proxy__port__read__v2__t}

  @detdesc
    This command is used by the client for receiving data (from the AFE service)
    to be written to the sink device. If sufficient data is available in the
    circular buffer, data is immediately copied and an acknowledgment is sent.
    @par
    If the circular buffer does not have enough data to fill in the circular
    buffer, an underrun is detected, partial data is copied, the rest of the
    buffer is filled with zeros, and an acknowledgment is sent.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    #AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER -- The following commands are to be
    sent with the port_id set as a sink (Rx) real-time proxy port ID:
    @par
    - #AFE_PARAM_ID_RT_PROXY_CONFIG through #AFE_MODULE_AUDIO_DEV_INTERFACE
    - #AFE_PORT_CMD_DEVICE_START
    @par
    The physical buffer must be fully contained in a memory region that has
    been mapped to the AFE service via the
    #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command.
*/
#define AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ_V2                        0x000100EE

/* Structure for the AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ_V2 command. */
typedef struct afe_port_data_cmd_rt_proxy_port_read_v2_t  afe_port_data_cmd_rt_proxy_port_read_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_data_cmd_rt_proxy_port_read_v2_t
@{ */
/* Payload of the AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ_V2 command, which
    delivers an empty buffer to the AFE service. On acknowledgment, data is
    filled in the buffer.
*/
struct afe_port_data_cmd_rt_proxy_port_read_v2_t
{
    uint16_t                  port_id;
    /**< Rx proxy port ID with which the real-time driver exchanges data.

         @values #AFE_PORT_ID_RT_PROXY_PORT_RANGE_START to
                 #AFE_PORT_ID_RT_PROXY_PORT_RANGE_END

         This must be a Rx (speaker) port. */

    uint16_t                  reserved;
    /**< This field must be set to zero. */

    uint32_t                  buffer_address_lsw;
    /**< Lower 32 bits of the 64-bit address containing the data sent from the
         AFE service to a real-time sink device on the client. */

    uint32_t                  buffer_address_msw;
    /**< Upper 32 bits of the 64-bit address containing the data sent from the
         AFE service to a real-time sink device on the client. */

    uint32_t                   mem_map_handle;
    /**< Unique identifier for an address.

         @values Any 32-bit value

         This memory map handle is returned by the aDSP through the
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command. */

    uint32_t                  available_bytes;
    /**< Number of valid bytes available in the buffer (including all
         channels).

         @values > 0 bytes

         This field must be equal to the frame size specified in the
         #AFE_MODULE_AUDIO_DEV_INTERFACE command that was sent to configure this
         port. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup afe_port_data_cmd_rt_proxy_port_read_v2_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the Audio Device Interface module, which configures devices such as
    I2S, PCM, HDMI, SLIMbus, etc.
    This module supports following parameter IDs:
     - #AFE_PARAM_ID_I2S_CONFIG
     - #AFE_PARAM_ID_PCM_CONFIG
     - #AFE_PARAM_ID_DIGI_MIC_CONFIG
     - #AFE_PARAM_ID_HDMI_CONFIG
     - #AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG
     - #AFE_PARAM_ID_SLIMBUS_CONFIG
     - #AFE_PARAM_ID_RT_PROXY_CONFIG
     - #AFE_PARAM_ID_PSEUDO_PORT_CONFIG
     - #AFE_PARAM_ID_INTERNAL_DIGIATL_CDC_CLK_CONFIG

    To configure the AFE port, the client must set the port via the
    #AFE_PORT_CMD_SET_PARAM_V2 command, and fill the module ID with the
    respective parameter IDs as listed above. The client must ensure the
    codec configuration and SLIMbus interface configuration (for a SLIMbus
	device) before sending this command to the AFE.

    @apr_hdr_fields
    Opcode -- AFE_MODULE_AUDIO_DEV_INTERFACE @newpage
*/
#define AFE_MODULE_AUDIO_DEV_INTERFACE                                0x0001020C

/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the I2S interface. Both #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_I2S_CONFIG

    @msg_payload{afe_param_id_i2s_cfg_t}
    @table{weak__afe__param__id__i2s__cfg__t}
*/
#define AFE_PARAM_ID_I2S_CONFIG                                      0x0001020D

/** Version information used to handle future additions to I2S interface
    configuration (for backward compatibility).
*/
#define AFE_API_VERSION_I2S_CONFIG                                     0x1

/** Sample rate is 8000 Hz. */
#define AFE_PORT_SAMPLE_RATE_8K                                     8000

/** Sample rate is 16000 Hz. */
#define AFE_PORT_SAMPLE_RATE_16K                                    16000

/** Sample rate is 22050 Hz. */
#define AFE_PORT_SAMPLE_RATE_22_05K                                 22050

/** Sample rate is 24000 Hz. */
#define AFE_PORT_SAMPLE_RATE_24K                                    24000

/** Sample rate is 32000 Hz. */
#define AFE_PORT_SAMPLE_RATE_32K                                    32000

/** Sample rate is 44100 Hz. */
#define AFE_PORT_SAMPLE_RATE_44_1K                                  44100

/** Sample rate is 88200 Hz. */
#define AFE_PORT_SAMPLE_RATE_88_2K                                  88200

/** Sample rate is 48000 Hz. */
#define AFE_PORT_SAMPLE_RATE_48K                                    48000

/** Sample rate is 96000 Hz. */
#define AFE_PORT_SAMPLE_RATE_96K                                    96000

/** Sample rate is 176400 Hz.*/
#define AFE_PORT_SAMPLE_RATE_176_4K                                 176400

/** Sample rate is 192000 Hz. */
#define AFE_PORT_SAMPLE_RATE_192K                                   192000


/** Enumeration for setting the data format to PCM when PCM data is
    transmited/received over I2S lines.
*/
#define AFE_LINEAR_PCM_DATA                                         0x0

/** Enumeration for setting the data format to Non-Linear (compressed) when
    non-linear (compressed) data is transmited/received over I2S lines.
*/
#define AFE_NON_LINEAR_DATA                                          0x1

/** Enumeration for setting the data format when PCM data packed in a 60958
    container is transmited/received over I2S lines.
*/
#define AFE_LINEAR_PCM_DATA_PACKED_60958                             0x2

/** Enumeration for setting the data format when non-linear data packed in a
    60958 container is transmited/received over I2S lines.
*/
#define AFE_NON_LINEAR_DATA_PACKED_60958                             0x3

/** Enumeration for setting the data format when PCM data packed in the
    custom parallel audio interface format.
*/
#define AFE_LINEAR_PCM_DATA_PACKED_CUSTOM_PAO                        0x4

/** Enumeration for setting the data format when non-LPCM data packed in
    the custom parallel audio interface format
*/
#define AFE_NON_LINEAR_DATA_PACKED_CUSTOM_PAO                        0x5

/** Enumeration for setting the I2S configuration channel_mode parameter to
    serial data wire number 0 (SD0).
*/
#define AFE_PORT_I2S_SD0                                            0x1

/** Enumeration for setting the I2S configuration channel_mode parameter to
    serial data wire number 1 (SD1).
*/
#define AFE_PORT_I2S_SD1                                            0x2

/** Enumeration for setting the I2S configuration channel_mode parameter to
    serial data wire number 2 (SD2).
*/
#define AFE_PORT_I2S_SD2                                            0x3

/** Enumeration for setting the I2S configuration channel_mode parameter to
    serial data wire number 3 (SD3).
*/
#define AFE_PORT_I2S_SD3                                            0x4

/** Enumeration for setting the I2S configuration channel_mode parameter to
    SD0 and SD1.
*/
#define AFE_PORT_I2S_QUAD01                                         0x5

/** Enumeration for setting the I2S configuration channel_mode parameter to
    SD2 and SD3.
*/
#define AFE_PORT_I2S_QUAD23                                         0x6

/** Enumeration for setting the I2S configuration channel_mode parameter to
    six channels.
*/
#define AFE_PORT_I2S_6CHS                                           0x7

/** Enumeration for setting the I2S configuration channel_mode parameter to
    8 channels.
*/
#define AFE_PORT_I2S_8CHS                                           0x8

/** Enumeration for setting the I2S configuration mono_stereo parameter to
    mono.
*/
#define AFE_PORT_I2S_MONO                                           0x0

/** Enumeration for setting the I2S configuration mono_stereo parameter to
    stereo.
*/
#define AFE_PORT_I2S_STEREO                                         0x1

/** Enumeration for setting the I2S configuration ws_src parameter to
    external.
*/
#define AFE_PORT_CONFIG_I2S_WS_SRC_EXTERNAL                         0x0

/** Enumeration for setting the I2S configuration ws_src parameter to
    internal. @newpage
*/
#define AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL                         0x1

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_i2s_cfg_t afe_param_id_i2s_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_i2s_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_I2S_CONFIG parameter ID used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_i2s_cfg_t
{
    uint32_t                  i2s_cfg_minor_version;
   /**< Minor version used for tracking I2S configuration.

        @values #AFE_API_VERSION_I2S_CONFIG */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24 */

    uint16_t                  channel_mode;
    /**< I2S lines and multichannel operation.

         @values
         - #AFE_PORT_I2S_SD0
         - #AFE_PORT_I2S_SD1
         - #AFE_PORT_I2S_SD2
         - #AFE_PORT_I2S_SD3
         - #AFE_PORT_I2S_QUAD01
         - #AFE_PORT_I2S_QUAD23
         - #AFE_PORT_I2S_6CHS
         - #AFE_PORT_I2S_8CHS @tablebulletend */

    uint16_t                  mono_stereo;
    /**< Specifies mono or stereo. This applies only when a single I2S line is
         used.

         @values
         - #AFE_PORT_I2S_MONO
         - #AFE_PORT_I2S_STEREO @tablebulletend */

    uint16_t                  ws_src;
    /**< Word select source: internal or external.

         @values
         - #AFE_PORT_CONFIG_I2S_WS_SRC_EXTERNAL
         - #AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL @tablebulletend */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_24K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K

         @contcell
         At 44100 Hz, the I2S interface supports both PCM and compressed data
         in both the Tx direction and Rx direction.

         There is no concurrent support of a voice call when the port operates
         at 44100 Hz because of Round Trip Delay (RTD) requirements.

         The AFE and ADM must be running with same sampling rate.
         All AFE clients must be at same sampling rate (44.1Khz) on the same
         device. This holds for other sampling rates as well which are either
         multiple or divisors of 44.1 kHz (e.g. 22.05 kHz, 88.2 kHz, 176.4 kHz).

         24 kHz and 32 kHz sampling rates are used mostly for compressed data.*/

    uint16_t                   data_format;
    /**< data format

         @values
         - #AFE_LINEAR_PCM_DATA
         - #AFE_NON_LINEAR_DATA
         - #AFE_LINEAR_PCM_DATA_PACKED_60958
         - #AFE_NON_LINEAR_DATA_PACKED_60958 @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_i2s_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the PCM interface. Both #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    Because of restrictions from the hardware, the AUX PCM interface must be
	enabled for both Rx and Tx at the same time. Rx or Tx cannot be used alone.
    For example, the primary AUX Tx PCM port must be enabled with the primary
	AUX Rx PCM port.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_PCM_CONFIG

    @msg_payload{afe_param_id_pcm_cfg_t}
    @table{weak__afe__param__id__pcm__cfg__t}
*/
#define AFE_PARAM_ID_PCM_CONFIG                                      0x0001020E

/** Version information used to handle future additions to PCM interface
    configuration (for backward compatibility).
*/
#define AFE_API_VERSION_PCM_CONFIG                                     0x1

/** Enumeration for the auxiliary PCM synchronization signal provided by an
    external source.
*/
#define AFE_PORT_PCM_SYNC_SRC_EXTERNAL                              0x0

/** Enumeration for the auxiliary PCM synchronization signal provided by an
    internal source.
*/
#define AFE_PORT_PCM_SYNC_SRC_INTERNAL                              0x1

/** Enumeration for the PCM configuration aux_mode parameter, which configures
    the auxiliary PCM interface to use short synchronization.
*/
#define AFE_PORT_PCM_AUX_MODE_PCM                                   0x0

/** Enumeration for the PCM configuration aux_mode parameter, which configures
    the auxiliary PCM interface to use long synchronization.
*/
#define AFE_PORT_PCM_AUX_MODE_AUX                                   0x1

/** Enumeration for setting the PCM configuration frame to 8. */
#define AFE_PORT_PCM_BITS_PER_FRAME_8                               0x0

/** Enumeration for setting the PCM configuration frame to 16. */
#define AFE_PORT_PCM_BITS_PER_FRAME_16                              0x1

/** Enumeration for setting the PCM configuration frame to 32. */
#define AFE_PORT_PCM_BITS_PER_FRAME_32                              0x2

/** Enumeration for setting the PCM configuration frame to 64. */
#define AFE_PORT_PCM_BITS_PER_FRAME_64                              0x3

/** Enumeration for setting the PCM configuration frame to 128. */
#define AFE_PORT_PCM_BITS_PER_FRAME_128                             0x4

/** Enumeration for setting the PCM configuration frame to 256. */
#define AFE_PORT_PCM_BITS_PER_FRAME_256                             0x5

/** Enumeration for setting the PCM configuration quant_type parameter to
    A-law with no padding.
*/
#define AFE_PORT_PCM_ALAW_NOPADDING                                 0x0

/** Enumeration for setting the PCM configuration quant_type parameter to
    mu-law with no padding.
*/
#define AFE_PORT_PCM_MULAW_NOPADDING                                0x1

/** Enumeration for setting the PCM configuration quant_type parameter to
    linear with no padding.
*/
#define AFE_PORT_PCM_LINEAR_NOPADDING                               0x2

/** Enumeration for setting the PCM configuration quant_type parameter to
    A-law with padding.
*/
#define AFE_PORT_PCM_ALAW_PADDING                                   0x3

/** Enumeration for setting the PCM configuration quant_type parameter to
    mu-law with padding.
*/
#define AFE_PORT_PCM_MULAW_PADDING                                  0x4

/** Enumeration for setting the PCM configuration quant_type parameter to
    linear with padding.
*/
#define AFE_PORT_PCM_LINEAR_PADDING                                 0x5

/** Enumeration for disabling the PCM configuration ctrl_data_out_enable
    parameter. The PCM block is the only master.
*/
#define AFE_PORT_PCM_CTRL_DATA_OE_DISABLE                           0x0

/** Enumeration for enabling the PCM configuration ctrl_data_out_enable
    parameter. The PCM block shares the signal with other masters. @newpage
*/
#define AFE_PORT_PCM_CTRL_DATA_OE_ENABLE                            0x1

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_pcm_cfg_t afe_param_id_pcm_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_pcm_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_PCM_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_pcm_cfg_t
{
    uint32_t                  pcm_cfg_minor_version;
   /**< Minor version used for tracking AUX PCM configuration.

        @values #AFE_API_VERSION_PCM_CONFIG */

    uint16_t                  aux_mode;
    /**< PCM synchronization setting.

         @values
         - #AFE_PORT_PCM_AUX_MODE_PCM
         - #AFE_PORT_PCM_AUX_MODE_AUX @tablebulletend */

    uint16_t                  sync_src;
    /**< Synchronization source.

         @values
         - #AFE_PORT_PCM_SYNC_SRC_EXTERNAL
         - #AFE_PORT_PCM_SYNC_SRC_INTERNAL @tablebulletend */

    uint16_t                  frame_setting;
    /**< Number of bits per frame.

         @values
         - #AFE_PORT_PCM_BITS_PER_FRAME_8
         - #AFE_PORT_PCM_BITS_PER_FRAME_16
         - #AFE_PORT_PCM_BITS_PER_FRAME_32
         - #AFE_PORT_PCM_BITS_PER_FRAME_64
         - #AFE_PORT_PCM_BITS_PER_FRAME_128
         - #AFE_PORT_PCM_BITS_PER_FRAME_256 @tablebulletend */

    uint16_t                  quant_type;
    /**< PCM quantization type.

         @values
         - #AFE_PORT_PCM_ALAW_NOPADDING
         - #AFE_PORT_PCM_MULAW_NOPADDING
         - #AFE_PORT_PCM_LINEAR_NOPADDING
         - #AFE_PORT_PCM_ALAW_PADDING
         - #AFE_PORT_PCM_MULAW_PADDING
         - #AFE_PORT_PCM_LINEAR_PADDING @tablebulletend */

    uint16_t                  ctrl_data_out_enable;
    /**< Specifies whether the PCM block shares the data-out signal to the
         drive with other masters.

         @values
         - #AFE_PORT_PCM_CTRL_DATA_OE_DISABLE
         - #AFE_PORT_PCM_CTRL_DATA_OE_ENABLE @tablebulletend */

    uint16_t                  reserved;
    /**< This field must be set to zero. */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K @tablebulletend */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16 */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values 1 to 4 */

    uint16_t                  slot_number_mapping[4];
    /**< Array that specifies the slot number for each channel in a
         multichannel scenario.

         @values 1 to 32 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_pcm_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the DIGI MIC interface. Both #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_DIGI_MIC_CONFIG

    @msg_payload{afe_param_id_digi_mic_cfg_t}
    @table{weak__afe__param__id__digi__mic__cfg__t}
*/
#define AFE_PARAM_ID_DIGI_MIC_CONFIG                                  0x0001020F

/** Version information used to handle future additions to DIGI MIC
    interface configuration (for backward compatibility).
*/
#define AFE_API_VERSION_DIGI_MIC_CONFIG                             0x1

/** Enumeration for setting the digital mic configuration channel_mode
    parameter to left 0.
*/
#define AFE_PORT_DIGI_MIC_MODE_LEFT0                                0x1

/** Enumeration for setting the digital mic configuration channel_mode
    parameter to right 0.
*/
#define AFE_PORT_DIGI_MIC_MODE_RIGHT0                               0x2

/** Enumeration for setting the digital mic configuration channel_mode
    parameter to left 1.
*/
#define AFE_PORT_DIGI_MIC_MODE_LEFT1                                0x3

/** Enumeration for setting the digital mic configuration channel_mode
    parameter to right 1.
*/
#define AFE_PORT_DIGI_MIC_MODE_RIGHT1                               0x4

/** Enumeration for setting the digital mic configuration channel_mode
    parameter to stereo 0.
*/
#define AFE_PORT_DIGI_MIC_MODE_STEREO0                              0x5

/** Enumeration for setting the digital mic configuration channel_mode
    parameter to stereo 1.
*/
#define AFE_PORT_DIGI_MIC_MODE_STEREO1                              0x6

/** Enumeration for setting the digital mic configuration channel_mode
    parameter to quad. @newpage
*/
#define AFE_PORT_DIGI_MIC_MODE_QUAD                                 0x7

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_digi_mic_cfg_t afe_param_id_digi_mic_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_digi_mic_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_DIGI_MIC_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_digi_mic_cfg_t
{
    uint32_t                  digi_mic_cfg_minor_version;
   /**< Minor version used for tracking DIGI Mic configuration.

        @values #AFE_API_VERSION_DIGI_MIC_CONFIG */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16 */

    uint16_t                  channel_mode;
    /**< Digital mic and multichannel operation.

         @values
         - #AFE_PORT_DIGI_MIC_MODE_LEFT0
         - #AFE_PORT_DIGI_MIC_MODE_RIGHT0
         - #AFE_PORT_DIGI_MIC_MODE_LEFT1
         - #AFE_PORT_DIGI_MIC_MODE_RIGHT1
         - #AFE_PORT_DIGI_MIC_MODE_STEREO0
         - #AFE_PORT_DIGI_MIC_MODE_STEREO1
         - #AFE_PORT_DIGI_MIC_MODE_QUAD @tablebulletend */

     uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_48K @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_digi_mic_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the HDMI interface. Both #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_HDMI_CONFIG

    @msg_payload{afe_param_id_hdmi_multi_chan_audio_cfg_t}
    @table{weak__afe__param__id__hdmi__multi__chan__audio__cfg__t}
*/
#define AFE_PARAM_ID_HDMI_CONFIG                                      0x00010210

/** Version information used to handle future additions to HDMI interface
    configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_HDMI_CONFIG                                 0x1

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_hdmi_multi_chan_audio_cfg_t afe_param_id_hdmi_multi_chan_audio_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_hdmi_multi_chan_audio_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_HDMI_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_hdmi_multi_chan_audio_cfg_t
{
   uint32_t                  hdmi_cfg_minor_version;
   /**< Minor version used for tracking HDMI module configuration.

        @values #AFE_API_VERSION_HDMI_CONFIG */

   uint16_t                  data_type;
    /**< Supported data type.

         @values
         - #AFE_LINEAR_PCM_DATA
         - #AFE_NON_LINEAR_DATA
         - #AFE_LINEAR_PCM_DATA_PACKED_60958
         - #AFE_NON_LINEAR_DATA_PACKED_60958 @tablebulletend */

   uint16_t                  channel_allocation;
   /**< HDMI channel allocation information for programming an HDMI frame.
        The default is 0 (Stereo).

        This information is defined in the HDMI standard, CEA 861-D
        (refer to @xhyperref{S1,[S1]}). The number of channels is also inferred
        from this parameter. */

   uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_22_05K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K

		 For the HDMI interface, sampling rates 22050, 44100, and 176400 Hz
         are applicable only for compressed data (non-PCM). */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24 */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_hdmi_multi_chan_audio_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the SPDIF interface.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_SPDIF_CONFIG

    @msg_payload{afe_param_id_spdif_audio_cfg_t}
    @table{weak__afe__param__id__spdif__audio__cfg__t}
*/
#define AFE_PARAM_ID_SPDIF_CONFIG                                      0x00010244

/** Version information used to handle future additions to SPDIF interface
    configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_SPDIF_CONFIG                                 0x1


/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_spdif_audio_cfg_t afe_param_id_spdif_audio_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_spdif_audio_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_SPDIF_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_spdif_audio_cfg_t
{
   uint32_t                  spdif_cfg_minor_version;
   /**< Minor version used for tracking SPDIF module configuration.

        @values #AFE_API_VERSION_SPDIF_CONFIG */

   uint32_t                  sample_rate;
   /**< Sampling rate of the port.

        @values
         - #AFE_PORT_SAMPLE_RATE_22_05K : This is only for compressed use case only.
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_88_2K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K*/

   uint16_t                  data_format;
   /**< Supported data type.

        @values
         - #AFE_LINEAR_PCM_DATA
         - #AFE_NON_LINEAR_DATA*/

   uint16_t                  num_channels;
   /**< Number of channels.

        @values 1 to 2 */

   uint16_t                  bit_width;
   /**< Bit width of the sample.

        @values 16, 24 */

   uint16_t                  reserved;
   /**< This field must be set to zero. */
}

#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_spdif_audio_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the channel status bits.
    - An channel status bits must be call afer an #AFE_PORT_CMD_SET_PARAM_V2 command
      is issued to configure the port parameter.
   - it may not able to support dynamic update on channel status bits depending on hardware version.
   - supported port :  AFE_PORT_ID_SPDIF_RX

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_CHSTATUS_CONFIG

    @msg_payload{afe_param_id_chstatus_cfg_t}
    @table{weak__afe__param__id__chstatus__cfg__t}
*/
#define AFE_PARAM_ID_CH_STATUS_CONFIG                           0x00010245

/** Version information used to handle future additions to SDPIF channel status
    configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_CH_STATUS_CONFIG                        0x1


/* Enumeration for channel type */

/** channel status bits for channel A */
#define AFE_CH_STATUS_A                                         0x1

/** channel status bits for channel B */
#define AFE_CH_STATUS_B                                         0x2

/** Enumeration for  channel status byte size  */
#define AFE_CH_STATUS_BYTE_SIZE                                 24


/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_chstatus_cfg_t afe_param_id_chstatus_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_chstatuso_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_CHSTATUS_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_chstatus_cfg_t
{
   uint32_t                  ch_status_cfg_minor_version;
   /**< Minor version used for tracking channel status configuration.

      @values #AFE_API_VERSION_CHSTATUS_CONFIG */

   uint32_t                  ch_status_type;
   /*** < indicate if the channel status is for channel A or B

        @values
        - #AFE_CHSTATUS_A
        - #AFE_CHSTATUS_B @tablebulletend */

   uint8_t   ch_stat_buf[AFE_CH_STATUS_BYTE_SIZE];
   /*** < 192 channel status bits for channel

        @values
        - The byte ordering is same as defined in IEC60958-3 */

   uint8_t   ch_stat_mask[AFE_CH_STATUS_BYTE_SIZE];
   /*** < 192 channel status bit masks for channel

        @values
        - The byte ordering is same as defined in IEC60958-3 */
}

#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_chstatus_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the HDMI Input Interface. Both #AFE_PORT_CMD_SET_PARAM_V2 and
   #AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_HDMI_CONFIG

    @msg_payload{afe_param_id_hdmi_input_cfg_t}
    @table{weak__afe__param__id__hdmi__input__cfg__t}
*/
#define AFE_PARAM_ID_HDMI_INPUT_CONFIG                              0x00010248

/** Version information used to handle future additions to HDMI Input interface
    configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_HDMI_INPUT_CONFIG                           0x1

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_hdmi_input_cfg_t afe_param_id_hdmi_input_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_hdmi_input_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_HDMI_INPUT_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_hdmi_input_cfg_t
{
   uint32_t                  hdmi_input_cfg_minor_version;
   /**< Minor version used for tracking HDMI module configuration.

        @values #AFE_API_VERSION_HDMI_INPUT_CONFIG */

   uint16_t                  data_type;
    /**< Supported data type.

         @values
         - #AFE_LINEAR_PCM_DATA_PACKED_CUSTOM_PAO
         - #AFE_NON_LINEAR_DATA_PACKED_CUSTOM_PAO @tablebulletend */

   uint16_t                  num_channels;
   /**< Number of channels

            @values 1-8 */

   uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_44_1K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_176_4K
         - #AFE_PORT_SAMPLE_RATE_192K
       */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24 */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_hdmi_input_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the BT or FM (RIVA) interface. Both #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG

    @msg_payload{afe_param_id_internal_bt_fm_cfg_t}
    @table{weak__afe__param__id__internal__bt__fm__cfg__t}
*/
#define AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG                          0x00010211

/** Version information used to handle future additions to BT or FM (RIVA)
    interface configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_INTERNAL_BT_FM_CONFIG                         0x1

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_internal_bt_fm_cfg_t afe_param_id_internal_bt_fm_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_internal_bt_fm_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_INTERNAL_BT_FM_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_internal_bt_fm_cfg_t
{
    uint32_t                  bt_fm_cfg_minor_version;
   /**< Minor version used for tracking BT and FM configuration.

        @values #AFE_API_VERSION_INTERNAL_BT_FM_CONFIG */

    uint16_t                  num_channels;
    /**< Number of channels.

	     @values
	     - 1 (only for BTSCO)
		 - 2 (only for FM and A2DP) */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16 */

    uint32_t                  sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K (only for BTSCO)
         - #AFE_PORT_SAMPLE_RATE_16K (only for BTSCO)
         - #AFE_PORT_SAMPLE_RATE_48K (FM and A2DP) @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_internal_bt_fm_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the SLIMbus interface using a shared channel approach. Both
    #AFE_PORT_CMD_SET_PARAM_V2 and #AFE_PORT_CMD_GET_PARAM_V2 can use this
	parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_SLIMBUS_CONFIG

    @msg_payload{afe_port_cmd_slimbus_cfg_t}
    @table{weak__afe__port__cmd__slimbus__cfg__t}
 */
#define AFE_PARAM_ID_SLIMBUS_CONFIG                                   0x00010212

/** Version information used to handle future additions to SLIMbus interface
    configuration (for backward compatibility).
*/
#define AFE_API_VERSION_SLIMBUS_CONFIG                                 0x1

/** Enumeration for setting SLIMbus device ID 1. */
#define AFE_SLIMBUS_DEVICE_1                                         0x0

/** Enumeration for setting SLIMbus device ID 2. */
#define AFE_SLIMBUS_DEVICE_2                                         0x1

/** Enumeration for setting the SLIMbus data formats. */
#define AFE_SB_DATA_FORMAT_NOT_INDICATED                             0x0

/** Enumeration for setting the maximum number of streams per device. @newpage */
#define AFE_PORT_MAX_AUDIO_CHAN_CNT                                   0x8

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_slimbus_cfg_t afe_param_id_slimbus_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_slimbus_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_SLIMBUS_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_slimbus_cfg_t
{
    uint32_t                  sb_cfg_minor_version;
    /**< Minor version used for tracking SLIMbus device configuration.

         @values #AFE_API_VERSION_SLIMBUS_CONFIG */

    uint16_t                  slimbus_dev_id;
    /**< SLIMbus hardware device ID, which is required to handle multiple
         SLIMbus hardware blocks.

         @values
         - #AFE_SLIMBUS_DEVICE_1
         - #AFE_SLIMBUS_DEVICE_2 @tablebulletend */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24 */

    uint16_t                  data_format;
    /**< Data format supported by the SLIMbus hardware. The default is 0
         (#AFE_SB_DATA_FORMAT_NOT_INDICATED), which indicates the hardware
         does not perform any format conversions before the data transfer. */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values 1 to #AFE_PORT_MAX_AUDIO_CHAN_CNT */

    uint8_t                  shared_channel_mapping[AFE_PORT_MAX_AUDIO_CHAN_CNT];
    /**< Array of shared channel IDs (128 to 255) to which the master port
         is to be connected.

         Shared_channel_mapping[i] represents the shared channel assigned for
         audio channel i in multichannel audio data. */

    uint32_t              sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_192K @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_slimbus_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the Real-time Proxy interface. Both #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode --AFE_PARAM_ID_RT_PROXY_CONFIG

    @msg_payload{afe_param_id_rt_proxy_port_cfg_t}
    @table{weak__afe__param__id__rt__proxy__port__cfg__t}
*/
#define AFE_PARAM_ID_RT_PROXY_CONFIG                                  0x00010213

/** Version information used to handle future additions to Real-time Proxy
    interface configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_RT_PROXY_CONFIG                             0x1

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_rt_proxy_port_cfg_t afe_param_id_rt_proxy_port_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_rt_proxy_port_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_RT_PROXY_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_rt_proxy_port_cfg_t
{
    uint32_t                  rt_proxy_cfg_minor_version;
    /**< Minor version used for tracking real-time proxy configuration.

         @values #AFE_API_VERSION_RT_PROXY_CONFIG */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24 */

    uint16_t                  interleaved;
    /**< Specifies whether the data exchanged between the AFE interface and
         real-time port is interleaved.

         @values
         - 0 -- Non-interleaved (samples from each channel are contiguous in the
                buffer)
         - 1 -- Interleaved (corresponding samples from each input channel are
                interleaved within the buffer) @tablebulletend */

    uint16_t                  frame_size;
    /**< Size of the frames that are used for PCM exchanges with this port.

         @values > 0, in bytes

         For example, 5 ms buffers of 16 bits and 16 kHz stereo samples
         is 5 ms * 16 samples/ms * 2 bytes/sample * 2 channels = 320 bytes. */

    uint16_t                  jitter_allowance;
    /**< Configures the amount of jitter that the port will allow.

         @values > 0

         For example, if +/-10 ms of jitter is anticipated in the
         timing of sending frames to the port, and the configuration is 16 kHz
         mono with 16-bit samples, this field is 10 ms * 16 samples/ms * 2
         bytes/sample = 320. */

    uint16_t                  low_water_mark;
    /**< Low watermark in bytes (including all channels).

         @values
         - 0 -- Do not send any low watermark events
         - > 0 -- Low watermark for triggering an event

         If the number of bytes in an internal circular buffer is lower than
         this low_water_mark parameter, a LOW_WATER_MARK event is sent to
         applications (via the #AFE_EVENT_RT_PROXY_PORT_STATUS event).

         The use of watermark events is optional for debugging purposes. */

    uint16_t                  high_water_mark;
    /**< High watermark in bytes (including all channels).

         @values
         - 0 -- Do not send any high watermark events
         - > 0 -- High watermark for triggering an event

         If the number of bytes in an internal circular buffer exceeds
         TOTAL_CIRC_BUF_SIZE minus high_water_mark, a high watermark event is
         sent to applications (via the #AFE_EVENT_RT_PROXY_PORT_STATUS event).

         The use of watermark events is optional and for debugging purposes. */

    uint32_t                    sample_rate;
    /**< Sampling rate of the port.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_16K
         - #AFE_PORT_SAMPLE_RATE_48K @tablebulletend*/

    uint16_t                  num_channels;
    /**< Number of channels.

         @values 1 to #AFE_PORT_MAX_AUDIO_CHAN_CNT */

    uint16_t                  reserved;
     /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_rt_proxy_port_cfg_t */

/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the Pseudoport interface. Only #AFE_PORT_CMD_SET_PARAM_V2 can use this
	parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_PSEUDO_PORT_CONFIG

    @msg_payload{afe_param_id_pseudo_port_cfg_t}
    @table{weak__afe__param__id__pseudo__port__cfg__t}
*/
#define AFE_PARAM_ID_PSEUDO_PORT_CONFIG                             0x00010219

/** Version information used to handle future additions to Pseudoport interface
    configuration (for backward compatibility).
*/
#define AFE_API_VERSION_PSEUDO_PORT_CONFIG                          0x1

/** Enumeration for setting the timing_mode parameter to faster than real
    time.
*/
#define AFE_PSEUDOPORT_TIMING_MODE_FTRT                             0x0

/** Enumeration for setting the timing_mode parameter to real time using
    timers. @newpage
*/
#define AFE_PSEUDOPORT_TIMING_MODE_TIMER                            0x1

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_pseudo_port_cfg_t afe_param_id_pseudo_port_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_pseudo_port_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_PSEUDO_PORT_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_pseudo_port_cfg_t
{
    uint32_t                  pseud_port_cfg_minor_version;
    /**< Minor version used for tracking pseudoport configuration.

         @values #AFE_API_VERSION_PSEUDO_PORT_CONFIG */

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24 */

    uint16_t                  num_channels;
    /**< Number of channels.

         @values  1 to 8  */

    uint16_t                  data_format;
    /**< Non-linear data format supported by the pseudoport (for future use).

         @values #AFE_LINEAR_PCM_DATA */

    uint16_t                  timing_mode;
    /**< Indicates whether the pseudoport synchronizes to the clock or
         operates faster than real time.

         @values
         - #AFE_PSEUDOPORT_TIMING_MODE_FTRT
         - #AFE_PSEUDOPORT_TIMING_MODE_TIMER @tablebulletend */

    uint32_t                  sample_rate;
    /**< Sample rate at which the pseudoport will run.

         @values
         - #AFE_PORT_SAMPLE_RATE_8K
         - #AFE_PORT_SAMPLE_RATE_32K
         - #AFE_PORT_SAMPLE_RATE_48K
         - #AFE_PORT_SAMPLE_RATE_96K
         - #AFE_PORT_SAMPLE_RATE_192K @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_pseudo_port_cfg_t */


/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the AHB clock for register access to specific hardware .
    Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_AHB_CLOCK_CONFIG

    @msg_payload{afe_param_id_pseudo_port_cfg_t}
    @table{weak__afe__param__id__pseudo__port__cfg__t}
*/
#define AFE_PARAM_ID_AHB_CLOCK_CONFIG                           0x00010249

/** Version information used to handle future additions to AHB clock
    configuration (for backward compatibility).
*/
#define AFE_API_VERSION_AHB_CLOCK_CONFIG                         0x1

/** Enumeration for disabling AHB clock for register access
*/
#define AFE_AHB_CLK_DISABLE                                      0x0

/** Enumeration for enabling AHB clock for register access
*/
#define AFE_AHB_CLK_ENABLE                                       0x1

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_ahb_clock_cfg_t afe_param_id_ahb_clock_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_ahb_clock_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_AHB_CLOCK_CONFIG parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_ahb_clock_cfg_t
{
    uint32_t                  ahb_clock_cfg_minor_version;
    /**< Minor version used for ahb clock configuration.

         @values #AFE_API_VERSION_AHB_CLOCK_CONFIG */

    uint32_t                  ahb_clk_enable;
    /**< flag to enable or disable the AHB clock for register access

        @values
           - #AFE_AHB_CLK_DISABLE
           - #AFE_AHB_CLK_ENABLE */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_ahb_clock_cfg_t */


/** @addtogroup afe_module_device
@{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to provide
    the Device Hardware delay.

    @apr_hdr_fields
    Opcode -- AFE_PARAM_ID_DEVICE_HW_DELAY

    @apr_msg_payload{afe_param_id_device_hw_delay_t}
    @table{weak__afe__param__id__device__hw__delay__t}
*/
#define AFE_PARAM_ID_DEVICE_HW_DELAY                             0x00010243

/** Version information used to handle future additions to the configuration
    interface (for backward compatibility).
*/
#define AFE_API_VERSION_DEVICE_HW_DELAY                          0x1

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_device_hw_delay_t afe_param_id_device_hw_delay_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_device_hw_delay_t
@{ */
/* Payload of the AFE_PARAM_ID_DEVICE_HW_DELAY parameter used by
    AFE_MODULE_AUDIO_DEV_INTERFACE.
*/
struct afe_param_id_device_hw_delay_t
{
    uint32_t                  device_hw_delay_minor_version;
	/**< Minor version used for tracking the version of  hw device delay config.
	  Current supported version is 1. */

    uint32_t                  delay_in_us;
    /**< device hw delay in micro seconds.

         @values valid uint32 number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_device_hw_delay_t */

/** @ingroup afe_cmd_device_start
    Starts the AFE interface specified by the port_id field.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_DEVICE_START

  @msg_payload{afe_port_cmd_device_start_t}
    @table{weak__afe__port__cmd__device__start__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    The port must be configured before sending this command; otherwise, it
    returns an error.
*/
#define AFE_PORT_CMD_DEVICE_START                                      0x000100E5

/* Structure for the AFE_PORT_CMD_DEVICE_START command. */
typedef struct afe_port_cmd_device_start_t afe_port_cmd_device_start_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_device_start_t
@{ */
/* Payload of the AFE_PORT_CMD_DEVICE_START command.
*/
struct afe_port_cmd_device_start_t
{
    uint16_t                  port_id;
    /**< Port interface and direction (Rx or Tx) to start. An even number
         represents the Rx direction, and an odd number represents the Tx
         direction.

         @values See <i>Hardware Ports</i> in Appendix @xref{sec:portIDs} */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_device_start_t */

/** @ingroup afe_cmd_device_stop
    Stops the AFE interface specified by the port_id field.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_DEVICE_STOP

  @msg_payload{afe_port_cmd_device_stop_t}
    @table{weak__afe__port__cmd__device__stop__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
*/
#define AFE_PORT_CMD_DEVICE_STOP                                      0x000100E6

/* Payload structure of the AFE_PORT_CMD_DEVICE_STOP command. */
typedef struct afe_port_cmd_device_stop_t afe_port_cmd_device_stop_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_device_stop_t
@{ */
/* Payload of the AFE_PORT_CMD_DEVICE_STOP command.
*/
struct afe_port_cmd_device_stop_t
{
    uint16_t                  port_id;
    /**< Port interface and direction (Rx or Tx) to start. An even number
         represents the Rx direction, and an odd number represents the Tx
         direction.

         @values See <i>Hardware Ports</i> in Appendix @xref{sec:portIDs} */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup afe_cmd_device_stop */

/** @addtogroup afe_cmd_mem_map
@{ */
/** Commands the AFE service to map multiple shared memory regions.
    All regions to be mapped must be from the same memory pool.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS

  @msg_payload{afe_service_cmd_shared_mem_map_regions_t}
    @tablens{weak__afe__service__cmd__shared__mem__map__regions__t}

  @mappayload{afe_service_shared_map_region_payload_t}
    @table{weak__afe__service__shared__map__region__payload__t}

  @return
    #AFE_SERVICE_CMDRSP_SHARED_MEM_MAP_REGIONS

  @dependencies
    None.
*/
#define AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS                       0x000100EA

/** Enumeration for the shared memory 8_4K pool ID. */
#define ADSP_MEMORY_MAP_SHMEM8_4K_POOL      3

/** @} */ /* end_addtogroup afe_cmd_mem_map */

/* AFE_SERVICE shared memory map regions structure  */
typedef struct  afe_service_cmd_shared_mem_map_regions_t afe_service_cmd_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmd_shared_mem_map_regions_t
@{ */
/* Memory map regions command payload used by the
    AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command that allows clients to map
    multiple shared memory regions in a single command. */
/** Immediately following this structure are num_regions of
    afe_service_shared_map_region_payload_t.
*/
struct afe_service_cmd_shared_mem_map_regions_t
{
    uint16_t                  mem_pool_id;
    /**< Type of memory on which this memory region is mapped.

         @values
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved

         The memory pool ID implicitly defines the characteristics of the memory.
         Characteristics can include alignment type, permissions, etc.

         ADSP_MEMORY_MAP_SHMEM8_4K_POOL is shared memory, byte addressable, and
         4 KB aligned. */

    uint16_t                  num_regions;
    /**< Number of regions to map.

         @values Any value greater than zero */

    uint32_t                  property_flag;
    /**< Configures one common property for all the regions in the payload.

         @values 0x00000000 to 0x00000001

         Bit 0 indicates physical or virtual mapping:
         - 0 -- The shared memory address provided in
           afe_service_shared_map_region_payload_t is a physical address.
           The shared memory must be mapped (hardware TLB entry), and a
           software entry must be added for internal bookkeeping.
         - 1 -- The shared memory address provided in
           afe_service_shared_map_region_payload_t is a virtual address.
           The shared memory must not be mapped (because the hardware TLB entry
           is already available), but a software entry must be added for
           internal bookkeeping. \n
           This is useful if two services within the aDSP are communicating
           via the APR. They can directly communicate via the virtual
           address instead of the physical address. The virtual regions must
           be contiguous. In this case, num_regions must be 1.

         Bits 31 to 1 are reserved and must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmd_shared_mem_map_regions_t */

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_shared_map_region_payload_t
@{ */
/* Payload of the map region; used by the
    afe_service_cmd_shared_mem_map_regions_t structure.
*/
struct afe_service_shared_map_region_payload_t
{
    uint32_t                  shm_addr_lsw;
    /**< Lower 32 bits of the shared memory address of the memory region to
         map. */

    uint32_t                  shm_addr_msw;
    /**< Upper 32 bits of the shared memory address of the memory region to
         map.

         The 64-bit number formed by shm_addr_lsw and shm_addr_msw word must be
         contiguous memory, and it must be 4 KB aligned.

         @values
         - For a 32 bit-shared memory address, this field must be set to zero.
         - For a 36 bit-shared memory address, bits 31 to 4 must be set to zero.
         - For a 64 bit-shared memory address, any 32-bit value. @tablebulletend */

    uint32_t                  mem_size_bytes;
    /**< Number of bytes in the region.

         @values Multiples of 4 KB

         The aDSP always maps the regions as virtual contiguous memory, but
         the memory size must be in multiples of 4 KB to avoid gaps in the
         virtually contiguous mapped memory. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_shared_map_region_payload_t */

/** @ingroup afe_resp_mem_map
    Response sent by the AFE service to the client in response to the
    successful processing of an #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS
    command.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMDRSP_SHARED_MEM_MAP_REGIONS \n

  @msg_payload{afe_service_cmdrsp_shared_mem_map_regions_t}
    @table{weak__afe__service__cmdrsp__shared__mem__map__regions__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A corresponding #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command must have
    been sent to receive this response from the AFE service.
*/
#define AFE_SERVICE_CMDRSP_SHARED_MEM_MAP_REGIONS                   0x000100EB

/* AFE_SERVICE shared memory map regions response structure  */
typedef struct  afe_service_cmdrsp_shared_mem_map_regions_t afe_service_cmdrsp_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmdrsp_shared_mem_map_regions_t
@{ */
/* Payload of the memory map command response used by the
    AFE_SERVICE_CMDRSP_SHARED_MEM_MAP_REGIONS command.
*/
struct afe_service_cmdrsp_shared_mem_map_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for an address.

         @values Any 32-bit value

         This memory map handle is returned by the aDSP through the
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command. This handle can be
         used as an abstract representation of the shared memory regions that
         are being mapped.

         The aDSP uses this handle to retrieve the shared memory attributes.
         The #AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS command uses this handle
         to unmap the mapped memory. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmdrsp_shared_mem_map_regions_t */

/** @ingroup afe_cmd_mem_unmap
    Commands the AFE service to unmap multiple shared memory regions that were
    previously mapped.

  @apr_hdr_fields
    Opcode -- AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS \n

  @msg_payload{afe_service_cmd_shared_mem_unmap_regions_t}
      @table{weak__afe__service__cmd__shared__mem__unmap__regions__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    A corresponding #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command must have
    been successfully processed.
*/
#define AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS                      0x000100EC

/* AFE_SERVICE shared memory unmap regions command structure */
typedef struct afe_service_cmd_shared_mem_unmap_regions_t afe_service_cmd_shared_mem_unmap_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_service_cmd_shared_mem_unmap_regions_t
@{ */
/* Payload of the memory map command response used by
    AFE_SERVICE_CMD_SHARED_MEM_UNMAP_REGIONS, which allows clients to unmap
    multiple shared memory regions in a single command.
*/
struct afe_service_cmd_shared_mem_unmap_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for an address.

         @values Any 32-bit value

         This memory map handle is returned by the aDSP through the
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command.

         The aDSP uses this handle to retrieve the associated shared
         memory regions and unmap them. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_service_cmd_shared_mem_unmap_regions_t */

/** @ingroup afe_cmd_port_set_params
    Configures one or more parameters on an AFE port. This command is
    typically used to apply calibration data to the modules.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_SET_PARAM_V2

  @msg_payload{afe_port_cmd_set_param_v2_t}
    @table{weak__afe__port__cmd__set__param__v2__t} @newpage

  @par Acknowledgment parameter payload (afe_port_param_data_v2_t)
    @tablens{weak__afe__port__param__data__v2__t}

  @detdesc
    This command is sent from the client to the aDSP to set the parameters of
    a port. A single command can be used to set multiple parameters, each
    belonging to multiple modules.
    @par
    Messages can be either in-band or out-of-band via shared memory. The modes
    are:
    @par
    - In-band set parameter -- In this mode, the physical address is set to
      NULL. The rest of the payload contains the parameters as shown in the
      variable payload structure (%afe_port_param_data_v2_t).
    - Shared memory set parameter -- In this mode, both the physical address to
      the parameter data and the data payload size are sent. The rest of the
      message is inside the shared memory in the variable payload format. The
      memory content is the same as the in-band payload structure
      (%afe_port_param_data_v2_t).

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
*/
#define AFE_PORT_CMD_SET_PARAM_V2                                      0x000100EF

/* Structure for the AFE parameter data. */
typedef struct afe_port_param_data_v2_t afe_port_param_data_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_param_data_v2_t
@{ */
/* Payload of the AFE parameter data used by
    AFE_PORT_CMD_SET_PARAM_V2, AFE_PORT_CMD_GET_PARAM_V2, and
    AFE_PORT_CMDRSP_GET_PARAM_V2. */
/** Immediately following this structure is the actual calibration data for the
    module_id/param_id pair.
*/
struct afe_port_param_data_v2_t
{
   uint32_t module_id;
   /**< ID of the module to be configured.

        @values Valid module ID (See Chapter @xref{hdr:CalibrationIDs}) */

   uint32_t param_id;
   /**< ID of the parameter corresponding to the supported parameters
        for the module ID.

        @values Valid parameter ID (See Chapter @xref{hdr:CalibrationIDs}) */

   uint16_t param_size;
   /**< Actual size of the data for the module_id/param_id pair. The size is a
        multiple of four bytes.

        @values > 0 */

   uint16_t reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_param_data_v2_t */




/* Structure for configuration/calibration settings for the AFE port. */
typedef struct afe_port_cmd_set_param_v2_t afe_port_cmd_set_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_set_param_v2_t
@{ */
/* Payload of the AFE_PORT_CMD_SET_PARAM_V2 command's
    configuration/calibration settings for the AFE port.
*/
struct afe_port_cmd_set_param_v2_t
{
   uint16_t port_id;
   /**< Port interface and direction (Rx or Tx) to start.

        @values See <i>Hardware Ports</i> in Appendix @xref{sec:portIDs} */

   uint16_t payload_size;
   /**< Actual size of the payload in bytes. This is used for parsing the
        parameter payload.

        @values > 0 */

   uint32_t payload_address_lsw;
    /**< Lower 32 bits of the payload address. */

   uint32_t payload_address_msw;
    /**< Upper 32 bits of the payload address.

         The 64-bit number formed by payload_address_lsw and
         payload_address_msw must be aligned to a 32-byte boundary, be 4 KB
         aligned, and be in contiguous memory.

         @values
         - For a 32-bit shared memory address, this field must be set to zero.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to zero. @tablebulletend */

   uint32_t mem_map_handle;
   /**< Unique identifier for an address.

        @values
        - NULL -- The message is in the payload (in-band).
        - Non-NULL -- The parameter data payload begins at the address
          specified by a pointer to the payload's physical address in shared
          memory (out-of-band).

        This memory map handle is returned by the aDSP through the
        #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command.

        An optional field is available if parameter data is in-band:
        %afe_port_param_data_v2_t param_data[...].
        For detailed payload content, see the next page. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_set_param_v2_t */

/** @ingroup afe_cmd_get_params
    Queries for the parameters of an AFE port.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMD_GET_PARAM_V2

  @msg_payload{afe_port_cmd_get_param_v2_t}
    @table{weak__afe__port__cmd__get__param__v2__t}

  @par Acknowledgment parameter payload (afe_port_param_data_v2_t)
    @tablens{weak__afe__port__param__data__v2__t}

  @detdesc
    This command is sent from the client to the aDSP to query the parameters of
    the requested port. If the payload_address is NULL (in-band), the aDSP
    sends the data in the APR payload of the AFE_PORT_CMDRSP_GET_PARAM_V2
    message.
    If the payload_address is non-NULL (out-of-band), data is stored at the
    address specified in the payload_address variable.
    @par
    The client receives a response from the aDSP with the values of the
    requested parameters in the AFE_PORT_CMDRSP_GET_PARAM_V2 message.

  @return
    An #AFE_PORT_CMDRSP_GET_PARAM_V2 message.

  @dependencies
    None.
*/
#define  AFE_PORT_CMD_GET_PARAM_V2                                    0x000100F0

/* Payload structure of the AFE_PORT_CMD_GET_PARAM_V2 command. */
typedef struct afe_port_cmd_get_param_v2_t afe_port_cmd_get_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmd_get_param_v2_t
@{ */
/* Payload of the AFE_PORT_CMD_GET_PARAM_V2 command, which queries for one
    post/preprocessing parameter of a stream.
*/
struct afe_port_cmd_get_param_v2_t
{
    uint16_t                  port_id;
    /**< Port interface and direction (Rx or Tx) to start.

         @values See <i>Hardware Ports</i> in Appendix @xref{sec:portIDs} */

    uint16_t                  payload_size;
    /**< Maximum data size of the parameter ID/module ID combination. This is
         a multiple of four bytes.

         @values > 0 bytes */

    uint32_t payload_address_lsw;
    /**< Lower 32 bits of the payload address. */

    uint32_t payload_address_msw;
    /**< Upper 32 bits of the payload address.

         The 64-bit number formed by payload_address_lsw and
         payload_address_msw must be aligned to a 32-byte boundary, be 4 KB
         aligned, and be in contiguous memory.

         @values
         - For a 32-bit shared memory address, this field must be set to zero.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to zero. @tablebulletend */

    uint32_t mem_map_handle;
    /**< Unique identifier for an address.

         @values
         - NULL -- The message is in the payload (in-band).
         - Non-NULL -- The parameter data payload begins at the address
           specified by a pointer to the payload's physical address in shared
           memory (out-of-band).

         This memory map handle is returned by the aDSP through the
         #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command.

         An optional field is available if parameter data is in-band:
         %afe_port_param_data_v2_t param_data[...].
         For detailed payload content, see the next page. */

    uint32_t                  module_id;
    /**< ID of the module to be queried.

         @values Valid module ID (See Chapter @xref{hdr:CalibrationIDs}) */

    uint32_t                  param_id;
    /**< ID of the parameter to be queried.

         @values Valid parameter ID (See Chapter @xref{hdr:CalibrationIDs}) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmd_get_param_v2_t */

/** @ingroup afe_resp_get_params
    Provides an acknowledgment response to an #AFE_PORT_CMD_GET_PARAM_V2
    command. The associated structure is afe_port_cmdrsp_get_param_v2_t.

    This event supports shared memory if an address is sent with
    #AFE_PORT_CMD_GET_PARAM_V2.

  @apr_hdr_fields
    Opcode -- AFE_PORT_CMDRSP_GET_PARAM_V2

  @msg_payload{afe_port_cmdrsp_get_param_v2_t}
    Immediately following this structure is the parameters structure
    (%afe_port_param_data_v2_t) containing the response (acknowledgment) parameter
    payload. This payload is included for an in-band scenario. For an
    address/shared memory-based set parameter, this payload is not needed.
    @tablens{weak__afe__port__cmdrsp__get__param__v2__t}

  @par Acknowledgment parameter payload (afe_port_param_data_v2_t)
    @tablens{weak__afe__port__param__data__v2__t} @newpage

  @detdesc
    This command is sent from the aDSP to the client with the parameter values
    requested in response to the #AFE_PORT_CMD_GET_PARAM_V2 command.
    @par
    For in-band parameters, the acknowledgment response contains the status,
    followed by the acknowledgment payload as described in the Acknowledgment
    parameter payload table.
    @par
    For out-of-band parameters, the response contains only the status, and the
    requested parameters are stored in the address specified in the
    AFE_PORT_CMD_GET_PARAM_v2. Data is stored in the form of the Acknowledgment
    parameter payload table.
    @par
    The choice of in-band or out-of-band depends on the values of
    payload_address present in the %AFE_PORT_CMD_GET_PARAM_V2 command.

  @return
    None.

  @dependencies
    The #AFE_PORT_CMD_GET_PARAM_V2 command must be sent before this response.
*/
#define AFE_PORT_CMDRSP_GET_PARAM_V2                                   0x00010106

/* Payload structure of the AFE_PORT_CMDRSP_GET_PARAM_V2 event. */
typedef struct afe_port_cmdrsp_get_param_v2_t afe_port_cmdrsp_get_param_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_port_cmdrsp_get_param_v2_t
@{ */
/* Payload of the AFE_PORT_CMDRSP_GET_PARAM_V2 message,
    which responds to an #AFE_PORT_CMD_GET_PARAM_V2 command.
*/
struct afe_port_cmdrsp_get_param_v2_t
{
    uint32_t                  status;
    /**< Status message or error code. For more information, refer to
         @xhyperref{Q3,[Q3]}.

         @values
          - APR_EOK -- Success
          - APR_EBADPARAM -- Failure @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_port_cmdrsp_get_param_v2_t */


#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_afe_algo_log_header_t
@{ */
/* Payload of the AFE algo log data used by
    Modules AFE_MODULE_FB_SPKR_PROT_RX and AFE_MODULE_FB_SPKR_PROT_VI_PROC
	for logging AFE_PARAM_ID_FBSP_TH_RX_STATS, AFE_PARAM_ID_FBSP_EX_RX_STATS,
	AFE_PARAM_ID_FBSP_CLIP_INT_STATS, AFE_PARAM_ID_FBSP_TH_VI_STATS
	and AFE_PARAM_ID_FBSP_EX_VI_STATS*/
/** Header structure for logging algorithm data. Immediately following this
    structure is the actual algorithm logging data for the module_id/param_id
    pair.
*/
struct afe_algo_log_header_t
{
   uint32_t module_id;
   /**< ID of the module to be configured.

        @values Valid module ID (See Chapter @xref{hdr:CalibrationIDs}) */

   uint32_t param_id;
   /**< ID of the parameter corresponding to the supported parameters
        for the module ID.

        @values Valid parameter ID (See Chapter @xref{hdr:CalibrationIDs}) */

   uint16_t param_size;
   /**< Actual size of the data for the module_id/param_id pair. The size is a
        multiple of four bytes.

        @values > 0 */

   uint16_t reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_algo_log_header_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** @ingroup afe_module_feedback_spkr_rx
    ID of the Feedback Speaker Protection Rx module, which supports the
    following parameter IDs:
     - #AFE_PARAM_ID_FBSP_MODE_RX_CFG
     - #AFE_PARAM_ID_FB_SPKR_PROT_FEAT_RX_CFG
     - #AFE_PARAM_ID_FBSP_IIR_RX_CFG
     - #AFE_PARAM_ID_SPKR_COIL_TX_RX_CFG
     - #AFE_PARAM_ID_FBSP_SPKR_COIL_RX_CFG
     - #AFE_PARAM_ID_FB_THERM_PROT_RX_CFG
     - #AFE_PARAM_ID_FB_EXCUR_CNTL_RX_CFG
     - #AFE_PARAM_ID_FBSP_ACM_RX_CFG
     - #AFE_PARAM_ID_FBSP_ACM_CDC_RX_CFG
     - #AFE_PARAM_ID_FBSP_EX_PARAM_RX_CFG
     - #AFE_PARAM_ID_FBSP_TH_RX_STATS
     - #AFE_PARAM_ID_FBSP_EX_RX_STATS
     - #AFE_PARAM_ID_FBSP_CLIP_INT_STATS
     - #AFE_PARAM_ID_ENABLE

    The AFE does not cache the configuration information when stopping the
    port. The expected sequence is:
    -# Configure the port using #AFE_MODULE_AUDIO_DEV_INTERFACE.
    -# Configure and enable the feedback speaker protection module using
       #AFE_PORT_CMD_SET_PARAM_V2 with the #AFE_MODULE_FB_SPKR_PROT_RX module,
       including the above mentioned parameters.
    -# Start the port using #AFE_PORT_CMD_DEVICE_START.
    -# Stop the port when needed using #AFE_PORT_CMD_DEVICE_STOP (the AFE
       destroys all the speaker protection-related information).

    Except enable, all other configuration for this module is accepted only in the
    configuration state of the port (i.e., after an AFE_MODULE_AUDIO_DEV_INTERFACE
    command and before an AFE_PORT_CMD_DEVICE_START command).

    Enable param ID for this module is also accepted when AFE port is in run state.
    This way runtime disable or enable of this feature can be achieved. There will be a
    glitch in audio output when the feature is disabled & enabled and vice versa during run time.
    So, it is recommended to avoid disable or enable of this feature during runtime.
    It is mandatory to send first enable param id to enable the feature when port is in configuration state.

    The feedback speaker protection module is supported only on a 16-bit or
    24-bit mono Rx port running at a sampling rate of 8000/16000/48000 Hz.

  @apr_hdr_fields
    Opcode -- AFE_MODULE_FB_SPKR_PROT_RX
*/
#define AFE_MODULE_FB_SPKR_PROT_RX                             0x0001021C

/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to configure the
    mode of feedback speaker protection Rx processing. Only
	#AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_mode_rx_cfg_v1_t}
    @table{weak__afe__fbsp__mode__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_FBSP_MODE_RX_CFG                          0x0001021D

/** Version information used to handle future additions to feedback speaker
    protection Rx mode configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_MODE_RX_CFG        					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_mode_rx_cfg_v1_t afe_fbsp_mode_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_mode_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_MODE_RX_CFG parameter (version 1).
*/
struct afe_fbsp_mode_rx_cfg_v1_t
{
   uint32_t       fbsp_mode_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker protection
        Rx processing configuration.

        @values #AFE_API_VERSION_FBSP_MODE_RX_CFG */

   uint32_t        mode;
   /**< Mode of feedback speaker protection Rx processing.

        @values
        - 0 -- Normal processing
        - 1 -- Thermal calibration @tablebulletend*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_mode_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to configure
    feedback speaker protection features Rx processing. Only
    #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_feat_rx_cfg_v1_t}
    @table{weak__afe__fbsp__feat__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_FB_SPKR_PROT_FEAT_RX_CFG     				      0x0001021E

/** Version information used to handle future additions to feedback speaker
    protection Rx feature configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_FEAT_RX_CFG        				      0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_feat_rx_cfg_v1_t afe_fbsp_feat_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_feat_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FB_SPKR_PROT_FEAT_RX_CFG parameter (version 1).
*/
struct afe_fbsp_feat_rx_cfg_v1_t
{
   uint32_t       fbsp_feat_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker
        protection Rx feature configuration.

        @values #AFE_API_VERSION_FBSP_FEAT_RX_CFG */

   uint32_t        feature;
   /**< Feature flag for the AFE_PARAM_ID_FB_SPKR_PROT_FEAT_RX_CFG parameter.

        @values
        - Bit 0 -- Notch high-pass filter
        - Bit 1 -- Thermal protection
        - Bit 2 -- Excursion control
        - Bit 3 -- Analog clip manager
        - Bits 31-4 -- Reserved; must be set to zero @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_feat_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to configure the
    IIR filter for feedback speaker protection Rx processing. Only
    #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_iir_rx_cfg_v1_t}
    @table{weak__afe__fbsp__iir__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_FBSP_IIR_RX_CFG     				            0x0001021F

/** Version information used to handle future additions to feedback speaker
    protection Rx IIR filter configuration (for backward compatibility).
*/
#define AFE_API_VERSION_FBSP_IIR_RX_CFG        				      0x1

/** Maximum number of stages in the IIR filter. @newpage */
#define AFE_FBSP_IIR_FILT_MAX_STAGES						5

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_iir_rx_cfg_v1_t afe_fbsp_iir_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_iir_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_IIR_RX_CFG parameter (version 1).
*/
struct afe_fbsp_iir_rx_cfg_v1_t
{
   uint32_t       fbsp_iir_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker protection Rx IIR
        filter feedback speaker configuration.

        @values #AFE_API_VERSION_FBSP_IIR_RX_CFG */

   int32_t        iir_nch_c_num[AFE_FBSP_IIR_FILT_MAX_STAGES * 3];
   /**< IIR notch filter numerator coefficients.
        Unused filter coefficients must be set to zero.

        @values -2147483648 to 2147483647 */

    int32_t       iir_nch_c_den[AFE_FBSP_IIR_FILT_MAX_STAGES * 2];
   /**< IIR notch filter denominator coefficients.
        Unused filter coefficients must be set to zero.

        @values -2147483648 to 2147483647 */

    int32_t       iir_nch_c_num_q[AFE_FBSP_IIR_FILT_MAX_STAGES];
    /**< IIR notch filter numerator Q factors.
         Unused filter factors must be set to zero.

        @values -32 to 32 */

    int32_t       iir_nch_c_den_q[AFE_FBSP_IIR_FILT_MAX_STAGES];
    /**< IIR notch filter denominator Q factors.
         Unused filter coeffs must be set to zero.

        @values -32 to 32 */

    int16_t       iir_nch_c_stages;
    /**< Number of stages in the IIR filter.

        @values 0 to 5 */

    int16_t       reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_iir_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX and
    #AFE_MODULE_FB_SPKR_PROT_VI_PROC to configure speaker coil Tx/Rx
    for feedback speaker protection Rx and Vsens/Isens (VI) processing.
	Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_spkr_coil_tx_rx_cfg_v1_t}
    @table{weak__afe__fbsp__spkr__coil__tx__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_SPKR_COIL_TX_RX_CFG     				      0x00010220

/** Version information used to handle future additions to feedback speaker
    coil Tx/Rx configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_SPKR_COIL_TX_RX_CFG       				   0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_spkr_coil_tx_rx_cfg_v1_t afe_fbsp_spkr_coil_tx_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_spkr_coil_tx_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_SPKR_COIL_TX_RX_CFG parameter (version 1).
*/
struct afe_fbsp_spkr_coil_tx_rx_cfg_v1_t
{
   uint32_t       spkr_coil_tx_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker coil Tx/Rx
        configuration.

        @values #AFE_API_VERSION_SPKR_COIL_TX_RX_CFG */

   int32_t        mt_l32_q24;
   /**< Enclosure independent parameter: MMD + MM1, where MMD is moving
        mass of diaphragm and MM1 is air mass load.

        @values
        - Minimum -- 1677 (0.0001g in Q24)
        - Maximum -- 1677721600 (100g in Q24) @tablebulletend */

   int32_t        re_l32_q24;
   /**< DC resistance of the speaker.

        @values
        - Minimum -- 167772 (0.01 Ohm in Q24)
        - Maximum -- 160054640 (100 Ohms in Q24) @tablebulletend */

   int32_t        le_l32_q24;
   /**< Inductance of the speaker.

        @values 0 to 160054640 (100 mH in Q24) */

   int32_t        bl_l32_q24;
   /**< Force factor.

        @values
        - Minimum -- 1677 (0.0001 in Q24)
        - Maximum -- 160054640 (100 in Q24) @tablebulletend */

   int32_t        fct_l32_q20;
   /**< Resonance frequency.

        @values
        - Minimum -- 1048576 (1 Hz in Q20)
        - Maximum -- 2147483647 (2047.99 Hz in Q20) @tablebulletend */

   int32_t        q_mct_l32_q24;
   /**< Mechanical Q factor (no unit).

        @values
        - Minimum -- 167772 (0.01 in Q24)
        - Maximum -- 160054640 (100 in Q24) @tablebulletend */

   int32_t        rect_l32_q24;
   /**< Resistance due to mechanical losses.

        @values
        - Minimum -- 167772 (0.01 Ohm in Q24)
        - Maximum -- 2147483647 (127.99 Ohms in Q24) @tablebulletend */

   int16_t        tolerance_re_l16;
   /**< Tolerance for DC resistance of the speaker.

        @values 1 to 100% */

   int16_t        tolerance_le_l16;
   /**< Tolerance for inductance of the speaker.

        @values 1 to 100% */

   int16_t        tolerance_bl_l16;
   /**< Tolerance for the force factor.

        @values 1 to 100% */

   int16_t        tolerance_fct_l16;
   /**< Tolerance for the resonance frequency.

        @values 1 to 100% */

   int16_t        tolerance_qmct_l16;
   /**< Tolerance for the Mechanical Q factor

        @values 1 to 100% */

   int16_t        tolerance_rect_l16;
   /**< Tolerance for resistance due to mechanical losses.

        @values 1 to 100% */

   uint16_t        temp_coef_inv_u16;
   /**< Resistivity coefficient. This is a speaker-dependent
        parameter.

        @values 10 to 65535 (1/degrees C) */

   int16_t        reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_spkr_coil_tx_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to configure
    speaker coil parameters used in Rx processing. Only
    #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_spkr_coil_rx_cfg_v1_t}
    @table{weak__afe__fbsp__spkr__coil__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_FBSP_SPKR_COIL_RX_CFG     						0x00010221

/** Version information used to handle future additions to feedback speaker
    protection speaker coil Rx configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_SPKR_COIL_RX_CFG        				0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_spkr_coil_rx_cfg_v1_t afe_fbsp_spkr_coil_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_spkr_coil_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_SPKR_COIL_RX_CFG parameter (version 1).
*/
struct afe_fbsp_spkr_coil_rx_cfg_v1_t
{
   uint32_t       fbsp_spkr_coil_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker protection speaker
        coil Rx configuration.

        @values #AFE_API_VERSION_FBSP_SPKR_COIL_RX_CFG */

   int16_t        r_spk_coil_q8;
   /**< Electrical resistance of speaker coil.

        @values
        - Minimum -- 512 (2 Ohms in Q8)
        - Maximum -- 16384 (64 Ohms in Q8) @tablebulletend */

   int16_t        v_full_sc_q10;
   /**< Speaker voltage for digitally full-scale signal.

        @values
        - Minimum -- 512 (0.5 volt in Q10)
        - Maximum -- 10240 (10 volts in Q10) @tablebulletend */

   int16_t        thermal_resistance_q6;
   /**< Voice coil thermal resistance (in degrees C/Watt).

        @values 0 to 32767 (511.9 in Q6) */

   int16_t        thermal_resistance_tot_q6;
   /**< Total thermal resistance (voice coil + magnet, etc.).

        @values 0 to 32767 (511.9 degrees C/Watt in Q6) */

   int16_t        thermal_tau_q10[2];
   /**< Estimate of the voice coil thermal time constant:
        - thermal_tau_q10[0] -- Low estimate
        - thermal_tau_q10[1] -- High estimate

        @values
        - Minimum -- 51 (0.049 seconds in Q10)
        - Maximum -- 32767 (31.9 seconds in Q10) @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_spkr_coil_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to configure
    feedback thermal protection Rx processing. Only
    #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fb_therm_prot_rx_cfg_v1_t}
    @table{weak__afe__fb__therm__prot__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_FB_THERM_PROT_RX_CFG   							0x00010222

/** Version information used to handle future additions to feedback speaker
    thermal protection Rx configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FB_THERM_PROT_RX_CFG      					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fb_therm_prot_rx_cfg_v1_t afe_fb_therm_prot_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fb_therm_prot_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FB_THERM_PROT_RX_CFG parameter (version 1).
*/
struct afe_fb_therm_prot_rx_cfg_v1_t
{
   uint32_t       fb_therm_prot_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker
        thermal protection Rx configuration.

        @values #AFE_API_VERSION_FB_THERM_PROT_RX_CFG */

   int16_t        cntl_input;
   /**< Type of control input for temperature feed.

        @values
        - 0 -- Use Feedback temperature only
        - 1 -- Use Feedback temperature with lowpass filter
        - 2 -- Feed forward estimated temperature @tablebulletend */

   int16_t        temp_loop_dly_ms;
   /**< Delay of predicted temperature feeding back to the Thermal Control
        module.

        @values 0 to 1000 ms */

   int16_t        pow_est_smooth;
   /**< Smoothing IIR filter flag for power estimation.

        @values
        - 0 -- Disable smoothing IIR filter
        - 1 -- Enable smoothing IIR filter @tablebulletend */

   int16_t        temp_tgt_q6;
   /**< Target control temperature.

        @values
        - Minimum -- -1920 (-30 degrees C in Q6)
        - Maximum -- 12800 (200 degrees C in Q6) @tablebulletend */

   int16_t        temp_nominal_q6;
   /**< Initial voice coil temperature.

        @values
        - Minimum -- -1920 (-30 degrees C in Q6)
        - Maximum -- 12800 (200 degrees C in Q6) @tablebulletend */

   int16_t        temp_min_q6;
   /**< Minimum reasonable measured temperature.

        A temperature lower than this setting is ignored.
        The temperature must not be higher than temp_nominal_q6 or temp_tgt_q6.

        @values
        - Minimum -- -1920 (-30 degrees C in Q6)
        - Maximum -- 12800 (200 degrees C in Q6) @tablebulletend */

   int16_t        temp_max_q6;
   /**< Maximum reasonable measured temperature.

        A temperature higher than this setting is ignored.
        The temperature must not be lower than temp_nominal_q6 or temp_tgt_q6.

        @values
        - Minimum -- -1920 (-30 degrees C in Q6)
        - Maximum -- 12800 (200 degrees C in Q6) @tablebulletend */

   int16_t        temp_max_chg_sec_q6;
   /**< Maximum temperature change per second.
        A new temperature is ignored if it is outside this range.

        @values
        - 0 to 32767 (511.9 degrees C in Q6)
        - 32767 -- Disable this feature @tablebulletend */

   int32_t        iir_tdc_c_num[AFE_FBSP_IIR_FILT_MAX_STAGES * 3];
   /**< Temperature smoothing IIR filter numerator coefficients.
        Unused filter coefficients must be set to zero.

        @values -2147483648 to 2147483647 */

   int32_t        iir_tdc_c_den[AFE_FBSP_IIR_FILT_MAX_STAGES * 2];
   /**< Temperature smoothing IIR filter denominator coefficients.
        Unused filter coefficients must be set to zero.

        @values -2147483648 to 2147483647 */

   int32_t        iir_tdc_c_num_q[AFE_FBSP_IIR_FILT_MAX_STAGES];
   /**< Integer part of the Q factor of the temperature smoothing IIR filter
        numerator coefficient.

        @values -2147483648 to 2147483647 */

   int32_t        iir_tdc_c_den_q[AFE_FBSP_IIR_FILT_MAX_STAGES];
   /**< Integer part of the Q factor of the temperature smoothing IIR filter
        denominator coefficient.

        @values -32 to 32 */

   int16_t        iir_tdc_c_stages;
   /**< Number of stages in the temperature smoothing IIR filter.

        @values 0 to 5 */

   int16_t        prop_x_q12;
   /**< Proportional term in Proportional and Integral (PI) control.

        @values 0 to 32767 (7.9 in Q12) */

   int16_t        intg_x_q12;
   /**< Integral term in PI control.

        @values 0 to 32767 (7.9 in Q12) */

   int16_t        min_gain_db_q7;
   /**< Minimum temperature control gain,

        @values -7680 (-60 dB in Q7) to 0 */

   int16_t        gain_alph_q15[2];
   /**< Smoothing of gain, attack (gain decreasing), and release (relaxing
        back to 0 dB).

        @values
        - Minimum -- 328 (0.01 dB in Q15)
        - Maximum -- 32767 (0.99 dB in Q15) @tablebulletend */

   int16_t        dead_zone_q6;
   /**< Dead zone (region near the target temperature) for a reduced
        proportional term (in degrees C).

        @values
        - Minimum -- 2 (0.03125 in Q6)
        - Maximum -- 2048 (32 in Q6) @tablebulletend */

   int16_t        dead_zone_sc_q15;
   /**< Scaling factor to modify the proportional term when inside a dead zone.

        @values
        - Minimum -- 328 (0.01 in Q15)
        - Maximum -- 32767 (0.99 in Q15) @tablebulletend */

   uint16_t        rx_scale_u16_q16;
   /**< Factor for scaling down the signal such that adding a pilot tone
        does not cause clipping.

        @values
        - Minimum -- 32768 (0.5 in Q16)
        - Maximum -- 65535 (0.99 in Q16) @tablebulletend */

   uint16_t        pi_scale_u16_q18;
   /**< Scale factor for the pilot tone, relative to 0 dBFs.

        @values 0 to 65535 (0.249 in Q18) */

   int32_t        clip_time_out_ms;
   /**< Clip interrupt timeout. A value greater than zero means the clip
        interrupt support feature is enabled with this timeout.

        @values 0 to 4000 ms */

   int32_t        vbat_time_out_ms;
   /**< VBAT timeouts. A value greater than zero means the VBAT interrupt
        support feature is enabled with this timeout.

        @values 0 to 1000000 ms */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fb_therm_prot_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to configure
    feedback excursion control Rx processing. Only
    #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fb_excur_cntl_rx_cfg_v1_t}
    @table{weak__afe__fb__excur__cntl__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_FB_EXCUR_CNTL_RX_CFG   							0x00010223

/** Version information used to handle future additions to feedback speaker
    excursion control Rx configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FB_EXCUR_CNTL_RX_CFG      					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fb_excur_cntl_rx_cfg_v1_t afe_fb_excur_cntl_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fb_excur_cntl_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FB_EXCUR_CNTL_RX_CFG parameter (version 1).
*/
struct afe_fb_excur_cntl_rx_cfg_v1_t
{
   uint32_t       excur_cntl_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker
        excursion control Rx configuration.

        @values #AFE_API_VERSION_FB_EXCUR_CNTL_RX_CFG */

   int16_t        f_limiter_only;
   /**< Specifies how the limiter is enabled.

        @values
        - 0 -- Limiter + excursion control
        - 1 -- Limiter only @tablebulletend @tablebulletend */

   int16_t        cutoff_freq;
   /**< Cutoff frequency for signal modification.

        @values 1000 to 4000 Hz */

   int32_t        amp_gain_l32_q24;
   /**< Constant gain representing the flat frequency response from the
        DAC to the speaker driver.
        This is the RMS voltage at the loudspeaker when 0 dB full-scale tone
        at 1 kHz is played in the digital domain.

        This setting allows for a negative value for phase inversion.

        @values
        - Minimum -- -2147483648 (-128 in Q24)
        - Maximum -- 2147483647 (127.99 in Q24) @tablebulletend */

   int32_t        margin_l32_q24;
   /**< Control parameter to allow for overestimation of peak excursion.

        @values
        - Minimum -- 8388608 (0.5 in Q24)
        - Maximum -- 33554432 (2 in Q24) @tablebulletend */

   int32_t        x_max_l32_q24;
   /**< Speaker excursion limit.

        @values
        - Minimum -- 167772 (0.01 mm in Q24)
        - Maximum -- 167772160 (10 mm in Q24) @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fb_excur_cntl_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to configure the
    Audio Clip Manager (ACM) for feedback speaker protection Rx processing.
	Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_acm_rx_cfg_v1_t}
    @table{weak__afe__fbsp__acm__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_FBSP_ACM_RX_CFG     							   0x00010224

/** Version information used to handle future additions to feedback speaker
    protection ACM configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_ACM_RX_CFG        					   0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_acm_rx_cfg_v1_t afe_fbsp_acm_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_acm_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_ACM_RX_CFG parameter (version 1).
*/
struct afe_fbsp_acm_rx_cfg_v1_t
{
   uint32_t       fbsp_acm_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker
        protection ACM configuration.

        @values #AFE_API_VERSION_FBSP_ACM_RX_CFG */

   int16_t        lim_to_clip_thr_ratio_q15;
   /**< Limiter threshold to clip the level ratio.

        @values
        - Minimum -- 16384 (0.5 in Q15)
        - Maximum -- 32767 (0.99 in Q15) @tablebulletend */

   int16_t        lowest_acc_clip_level_q15;
   /**< Lowest acceptable clip level. Any clipping detecting below this level
        is ignored.

        @values 0 to 26214 (0.799 in Q15) */

   int16_t        thr_inc_step_in_db_q15;
   /**< Limiter threshold increment per step_size_in_msec.

        @values 0 to 32767 (0.99 dB in Q15) */

   uint16_t        step_size_in_msec;
   /**< Step size for the limiter threshold increment.

        @values 1 to 10000 ms */

   uint32_t        spk_prot_acm_clip_lvl_det_mthd;
   /**< Methods for clip-level detection.

        In the Codec Maximum Value and Codec History buffer methods, the
        clip-level detection is done in the codec.

        In the Library History buffer, the clip-level detection is done in
        the aDSP.

        @values
        - 0 -- Codec Maximum Value
        - 1 -- Codec History buffer
        - 2 -- Library History buffer @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_acm_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to configure the
    ACM Codec mode for feedback speaker protection Rx processing. Only
    #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_acm_cdc_rx_cfg_v1_t}
    @table{weak__afe__fbsp__acm__cdc__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_FBSP_ACM_CDC_RX_CFG    							0x00010225

/** Version information used to handle future additions to feedback speaker
    protection ACM codec Rx mode configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_ACM_CDC_RX_CFG       					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_acm_cdc_rx_cfg_v1_t afe_fbsp_acm_cdc_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_acm_cdc_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_ACM_CDC_RX_CFG parameter (version 1).
*/
struct afe_fbsp_acm_cdc_rx_cfg_v1_t
{
   uint32_t       fbsp_acm_cdc_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker protection ACM codec
        Rx mode configuration.

        @values #AFE_API_VERSION_FBSP_ACM_CDC_RX_CFG */

   uint16_t        cdc_clip_det_trigger_thr;
   /**< Number of triggers the codec waits for before asserting the clip
        interrupt.

        @values 1 to 12 */

   uint16_t        num_cdc_fifos;
   /**< Depth of the trigger FIFO queue in the codec.

        @values 1 to 12 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_acm_cdc_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to configure
    excursion parameters for feedback speaker protection Rx processing.
	Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_ex_param_rx_cfg_v1_t}
    @table{weak__afe__fbsp__ex__param__rx__cfg__v1__t}
*/
#define AFE_PARAM_ID_FBSP_EX_PARAM_RX_CFG    							0x0001023b

/** Version information used to handle future additions to feedback speaker
    protection excursion parameter Rx processing configuration (for backward
    compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_EX_PARAM_RX_CFG       					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_ex_param_rx_cfg_v1_t afe_fbsp_ex_param_rx_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_ex_param_rx_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_EX_PARAM_RX_CFG parameter (version 1).
*/
struct afe_fbsp_ex_param_rx_cfg_v1_t
{
   uint32_t       fbsp_ex_param_rx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker protection
        excursion parameter Rx processing configuration.
        Currently, the supported version is 1.

        @values #AFE_API_VERSION_FBSP_EX_PARAM_RX_CFG */

   uint32_t        cx_data_size_in_bytes;
   /**< Size of the excursion control data in bytes.

        @values
        All valid sizes */

   uint32_t        cx_data_addr_lsw;
   /**< Lower 32 bits of the 64-bit payload address. */

   uint32_t        cx_data_addr_msw;
   /**< Upper 32 bits of the 64-bit payload address.

         The 64-bit number formed by cx_data_addr_lsw and cx_data_addr_msw
         word must be contiguous memory, and it must be 4 KB aligned.

		@values
         - For a 32 bit-shared memory address, this field must be set to zero.
         - For a 36 bit-shared memory address, bits 31 to 4 must be set to zero.
         - For a 64 bit-shared memory address, any 32-bit value. @tablebulletend */

   uint32_t        mem_map_handle;
   /**< Unique identifier for an address.

        @values
        - NULL -- The message is in the payload (in-band).
        - Non-NULL -- The parameter data payload begins at the address
          specified by a pointer to the payload's physical address in shared
          memory (out-of-band).

        This memory map handle is returned by the aDSP through the
        #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command.

        An optional field is available if parameter data is in-band:
        %afe_port_param_data_v2_t param_data[...].
        For detailed payload content, see afe_port_param_data_v2_t. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_ex_param_rx_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to log thermal
    Rx processing statistics for feedback speaker protection.

	This parameter ID is currently used for logging the algorithm internal
	variables. It is not supported by #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2.

    @msg_payload{afe_fbsp_th_rx_stats_v1_t}
    @table{weak__afe__fbsp__th__rx__stats__v1__t}

    @if OEM_only
    @par Algorithm logging data (afe_afe_algo_log_header_t)
    @tablens{weak__afe__afe__algo__log__header__t}
    @endif
*/
#define AFE_PARAM_ID_FBSP_TH_RX_STATS    							0x0001023c

/** Version information used to handle future additions to feedback speaker
    protection thermal Rx processing statistics (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_TH_RX_STATS       					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_th_rx_stats_v1_t afe_fbsp_th_rx_stats_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_th_rx_stats_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_TH_RX_STATS parameter (version 1).
*/
struct afe_fbsp_th_rx_stats_v1_t
{
   uint32_t       fbsp_th_rx_stats_minor_version;
   /**< Minor version used for tracking feedback speaker protection thermal
        Rx processing statistics. Currently, the supported version is 1.

        @values #AFE_API_VERSION_FBSP_TH_RX_STATS */

   uint32_t        frame_duration_in_ms;
   /**< Frame duration in milliseconds for thermal variables.

        @values Valid uint32 number */

   uint32_t        frame_num;
   /**< Frame number of the thermal variables. This field is reset to zero
        after crossing the maximum limit for uint32 variables.

        @values Valid uint32 number */

   int32_t        th_gain_dB_q23;
   /**< Thermal gain in dB.

        @values Valid int32 number in Q23 format */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_th_rx_stats_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to log excursion
    tuning Rx processing statistics for feedback speaker protection.

	This parameter ID is currently used for logging the algorithm internal
	variables. It is not supported by #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2.

    @msg_payload{afe_fbsp_ex_rx_stats_v1_t}
    @table{weak__afe__fbsp__ex__rx__stats__v1__t}

    @if OEM_only
    @par Algorithm logging data (afe_afe_algo_log_header_t)
    @tablens{weak__afe__afe__algo__log__header__t}
    @endif
*/
#define AFE_PARAM_ID_FBSP_EX_RX_STATS    							0x0001023d

/** Version information used to handle future additions to feedback speaker
    protection log excursion Rx tuning statistics (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_EX_RX_STATS       					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_ex_rx_stats_v1_t afe_fbsp_ex_rx_stats_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_ex_rx_stats_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_EX_RX_STATS parameter (version 1).
*/
struct afe_fbsp_ex_rx_stats_v1_t
{
   uint32_t       fbsp_ex_rx_stats_minor_version;
   /**< Minor version used for tracking feedback speaker protection log
        excursion Rx tuning statistic. Currently, the supported version is 1.

        @values #AFE_API_VERSION_FBSP_EX_RX_STATS */

   uint32_t        frame_duration_in_ms;
   /**< Frame duration in milliseconds for excursion variables.

        @values Valid uint32 number */

   uint32_t        frame_num;
   /**< Frame number of the excursion variables. This field is reset to zero
        after crossing the maximum limit for uint32 variables.

        @values Valid uint32 number */

   int16_t        max_exc;
   /**< Peak excursion (in millimeters).


        @values Valid int16 number in Q15 format */

   int16_t                 reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_ex_rx_stats_v1_t */

/** @addtogroup afe_module_feedback_spkr_rx
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_RX to log audio clip
    interrupt statistics for feedback speaker protection.

	This parameter ID is currently used for logging the algorithm internal
	variables. It is not supported by #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2.

    @msg_payload{afe_fbsp_clip_int_stats_v1_t}
    @table{weak__afe__fbsp__clip__int__stats__v1__t}

    @if OEM_only
    @par Algorithm logging data (afe_afe_algo_log_header_t)
    @tablens{weak__afe__afe__algo__log__header__t}
    @endif
*/
#define AFE_PARAM_ID_FBSP_CLIP_INT_STATS   							0x0001023e

/** Version information used to handle future additions to feedback speaker
    protection clip interrupt statistics (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_CLIP_INT_STATS       					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_rx */

typedef struct afe_fbsp_clip_int_stats_v1_t afe_fbsp_clip_int_stats_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_clip_int_stats_v1_t
@{ */
/* Configuration structure for the
    #AFE_PARAM_ID_FBSP_CLIP_INT_STATS parameter (version 1).
*/
struct afe_fbsp_clip_int_stats_v1_t
{
   uint32_t       fbsp_clip_int_stats_minor_version;
   /**< Minor version used for tracking feedback speaker protection clip
        interrupt statistics. Currently, the supported version is 1.

        @values #AFE_API_VERSION_FBSP_CLIP_INT_STATS */

   uint32_t        frame_duration_in_ms;
   /**< Frame duration in milliseconds for the clip interrupt.

        @values Valid uint32 number */

   uint32_t        frame_num;
   /**< Frame number of the clip interrupt variables. This field is reset to
        zero after crossing the maximum limit for uint32 variables.

        @values Valid uint32 number */

   uint32_t        clip_interrupt;
   /**< Audio clip interrupt.

        @values Valid uint32 number */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_clip_int_stats_v1_t */

/** @addtogroup afe_module_feedback_spkr_vi
@{ */
/** ID of the Feedback Speaker Protection VI processing module, which supports
    the following parameter IDs:
     - #AFE_PARAM_ID_MODE_VI_PROC_CFG
     - #AFE_PARAM_ID_THERMAL_VI_PROC_CFG
     - #AFE_PARAM_ID_EXCURSION_VI_PROC_CFG
     - #AFE_PARAM_ID_SPKR_CALIB_VI_PROC_CFG
     - #AFE_PARAM_ID_CALIB_RES_CFG
     - #AFE_PARAM_ID_FEEDBACK_PATH_CFG
     - #AFE_PARAM_ID_SPKR_COIL_TX_RX_CFG
     - #AFE_PARAM_ID_TUNING_VI_PROC_CFG
     - #AFE_PARAM_ID_FBSP_TH_VI_STATS
     - #AFE_PARAM_ID_FBSP_EX_VI_STATS
     - #AFE_PARAM_ID_ENABLE

    The AFE does not cache the configuration information when stopping the
    port. The expected sequence is:
    -# Configure the port using #AFE_MODULE_AUDIO_DEV_INTERFACE.
    -# Configure and enable the speaker protection module using
       #AFE_PORT_CMD_SET_PARAM_V2 and the #AFE_MODULE_SPEAKER_PROTECTION module,
       including both the #AFE_PARAM_ID_SPKR_PROT_CONFIG and
       #AFE_PARAM_ID_ENABLE parameters.
    -# Start the port using #AFE_PORT_CMD_DEVICE_START.
    -# Stop the port when needed using #AFE_PORT_CMD_DEVICE_STOP (the AFE destroys all
       the speaker protection-related information).

    This module is accepted only in the configuration state of the port (i.e.,
    after an AFE_MODULE_AUDIO_DEV_INTERFACE command and before an
    AFE_PORT_CMD_DEVICE_START command.

    The feedback speaker protection VI processing module is supported only on a 16-bit  stereo
    Tx port running at a sampling rate of 48000 Hz.

  @apr_hdr_fields
    Opcode -- AFE_MODULE_FB_SPKR_PROT_VI_PROC @newpage
*/
#define AFE_MODULE_FB_SPKR_PROT_VI_PROC           					0x00010226

/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_VI_PROC to configure
    the mode of feedback speaker protection VI processing. Only
    #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_mode_vi_proc_cfg_v1_t}
    @table{weak__afe__fbsp__mode__vi__proc__cfg__v1__t}
*/
#define AFE_PARAM_ID_MODE_VI_PROC_CFG     							0x00010227

/** Version information used to handle future additions to speaker protection
    VI Processing mode configuration (for backward compatibility).
*/
#define AFE_API_VERSION_MODE_VI_PROC_CFG        					0x1

/** Calibration mode is disabled. */
#define AFE_FBSP_CALIB_MODE_DISABLE 	0

/** Calibration mode is enabled. @newpage */
#define AFE_FBSP_CALIB_MODE_ENABLE 		1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_vi */

typedef struct afe_fbsp_mode_vi_proc_cfg_v1_t afe_fbsp_mode_vi_proc_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_mode_vi_proc_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_MODE_VI_PROC_CFG parameter (version 1).
*/
struct afe_fbsp_mode_vi_proc_cfg_v1_t
{
   uint32_t       mode_vi_proc_cfg_minor_version;
   /**< Minor version used for tracking speaker protection VI Processing mode
        configuration.

        @values #AFE_API_VERSION_MODE_VI_PROC_CFG */

   uint32_t        cali_mode;
   /**< Specifies if the Calibration mode is enabled or disabled.

        @values
        - #AFE_FBSP_CALIB_MODE_DISABLE
        - #AFE_FBSP_CALIB_MODE_ENABLE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_mode_vi_proc_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_vi
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_VI_PROC to configure
    parameters for thermal VI processing. Only #AFE_PORT_CMD_SET_PARAM_V2 can
	use this parameter ID.

    @msg_payload{afe_fbsp_thermal_vi_proc_cfg_v1_t}
    @table{weak__afe__fbsp__thermal__vi__proc__cfg__v1__t}
*/
#define AFE_PARAM_ID_THERMAL_VI_PROC_CFG    							0x00010228

/** Version information used to handle future additions to feedback speaker
    protection thermal VI processing configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_THERMAL_VI_PROC_CFG        					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_vi */

typedef struct afe_fbsp_thermal_vi_proc_cfg_v1_t afe_fbsp_thermal_vi_proc_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_thermal_vi_proc_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_THERMAL_VI_PROC_CFG parameter (version 1).
*/
struct afe_fbsp_thermal_vi_proc_cfg_v1_t
{
   uint32_t       therm_vi_proc_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker protection thermal
        VI processing configuration.

        @values #AFE_API_VERSION_THERMAL_VI_PROC_CFG */

   int32_t        v_meas_bias_q26;
   /**< Biased value of voltage measured to avoid division by 0.

        @values
        - Minimum -- 537 (0.000008 volt in Q26)
        - Maximum -- 536871 (0.008 volt in Q26) @tablebulletend */

   int32_t        i_meas_bias_q26;
   /**< Biased value of current measured to avoid division by 0.

        @values
        - Minimum -- 67 (0.000001 amp in Q26)
        - Maximum -- 67109 (0.001 amp in Q26) @tablebulletend */

   int32_t        r0_safe_q24;
   /**< Default calibration point resistance when per-device calibration is
        not available

        @values
        - Minimum -- 33554432 (2 Ohms in Q24)
        - Maximum -- 1073741824 (64 Ohms in Q24) @tablebulletend */

   int16_t        t0_safe_q6;
   /**< Default calibration point temperature when per device
        calibration is not available

        @values
        - Minimum -- -1920 (-30 degrees C in Q6)
        - Maximum -- 5120 (80 degrees C in Q6) @tablebulletend */

   int16_t        pow_sup_freq;
   /**< Power supply frequency.

        @values
        - 50 or 60 Hz
        - 0 -- Disabled */

   uint16_t        vsen_2_v_u16_q12;
   /**< Conversion factor for Vsens digital to volts.

        @values 0 to 65535 (15.99 in Q12) */

   uint16_t        isen_2_a_u16_q12;
   /**< Conversion factor for Isens digital to amps.

        @values 0 to 65535 (15.99 in Q12) */

   int32_t        r0_short_q24;
   /**< Resistance below this speaker is considered a shorted circuit.

        @values
        - Minimum -- 8388608 (0.5 Ohm in Q24)
        - Maximum -- 1073741824(64 Ohms in Q24) @tablebulletend */

   int32_t        r0_open_q24;
   /**< Resistance above this speaker is considered an open circuit.

        @values
        - Minimum -- 16777216 (1 Ohm in Q24)
        - Maximum -- 2147483647 (127.99 Ohms in Q24) @tablebulletend */

   int32_t        wait_time_ms;
   /**< Wait time for initial settling before collecting statistics during
        calibration.

        @values 100 to 5000 ms */

   int32_t        cali_time_ms;
   /**< Calibration time.

        @values 500 to 30000 ms */

   int32_t        r0_min_q24;
   /**< Minimum expected resistance from calibration.

        @values
        - Minimum -- 33554432 (2 Ohms in Q24)
        - Maximum -- 1073741824 (64 Ohms in Q24) @tablebulletend */

   int32_t        r0_max_q24;
   /**< Maximum expected resistance from calibration.

        @values
        - Minimum -- 33554432 (2 Ohms in Q24)
        - Maximum -- 1073741824 (64 Ohms in Q24) @tablebulletend */

   int16_t        t0_min_q6;
   /**< Minimum expected temperature from calibration.

        @values
        - Minimum -- -1920 (-30 degrees C in Q6)
        - Maximum -- 5120 (80 degrees C in Q6) @tablebulletend */

   int16_t        t0_max_q6;
   /**< Maximum expected temperature from calibration.

        @values
        - Minimum -- -1920 (-30 degrees C in Q6)
        - Maximum -- 5120 (80 degrees C in Q6) @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_thermal_vi_proc_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_vi
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_VI_PROC to configure
    excursion parameters for feedback speaker protection VI processing.
	Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_excursion_vi_proc_cfg_v1_t}
    @table{weak__afe__fbsp__excursion__vi__proc__cfg__v1__t}
*/
#define AFE_PARAM_ID_EXCURSION_VI_PROC_CFG    						0x00010229

/** Version information used to handle future additions to speaker protection
    excursion VI Processing mode configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_EXCURSION_VI_PROC_CFG        				0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_vi */

typedef struct afe_fbsp_excursion_vi_proc_cfg_v1_t afe_fbsp_excursion_vi_proc_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_excursion_vi_proc_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_EXCURSION_VI_PROC_CFG parameter (version 1).
*/
struct afe_fbsp_excursion_vi_proc_cfg_v1_t
{
   uint32_t       excursion_vi_proc_cfg_minor_version;
   /**< Minor version used for tracking speaker protection excursion VI
        Processing mode configuration.

        @values #AFE_API_VERSION_EXCURSION_VI_PROC_CFG */

   int32_t        v_scale_l32_q24;
   /**< Scaling factor for Vsens. To allow for phase inversion, set this
        field to a negative value.

        @values
        - Minimum -- -2147483648 (-128 in Q24)
        - Maximum -- 2147483647 (127.99 in Q24) @tablebulletend */

   int32_t        i_scale_l32_q24;
   /**< Scaling factor for Isens. To allow for phase inversion, set this
        field to a negative value.

        @values
        - Minimum -- -2147483648 (-128 in Q24)
        - Maximum -- 2147483647 (127.99 in Q24) @tablebulletend */

   int16_t        f_param_adaptation;
   /**< Enables (1) or disables (0) speaker parameter adaptation. */

   int16_t        rx_cutoff_freq;
   /**< Cutoff frequency for signal processing.

        @values 1000 to 4000 Hz */

   int32_t        rx_amp_gain_l32_q24;
   /**< Amplifier gain in the codec.

        @values
        - Minimum -- -2147483648 (-128 in Q24)
        - Maximum -- 2147483647 (127.99 in Q24) @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_excursion_vi_proc_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_vi
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_VI_PROC to configure
    calibration for feedback speaker VI calibration processing. Only
	#AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_spkr_calib_vi_proc_cfg_v1_t}
    @table{weak__afe__fbsp__spkr__calib__vi__proc__cfg__v1__t}
*/
#define AFE_PARAM_ID_SPKR_CALIB_VI_PROC_CFG    						0x0001022A

/** Version information used to handle future additions to VI processing
    speaker calibration configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_SPKR_CALIB_VI_PROC_CFG       				0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_vi */

typedef struct afe_fbsp_spkr_calib_vi_proc_cfg_v1_t afe_fbsp_spkr_calib_vi_proc_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_spkr_calib_vi_proc_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_SPKR_CALIB_VI_PROC_CFG parameter (version 1).
*/
struct afe_fbsp_spkr_calib_vi_proc_cfg_v1_t
{
   uint32_t       spkr_calib_vi_proc_cfg_minor_version;
   /**< Minor version used for tracking VI processing speaker calibration
        configuration.

        @values #AFE_API_VERSION_SPKR_CALIB_VI_PROC_CFG */

   int32_t        r0_cali_q24;
   /**< Caliberation point resistance per device.

        @values
        - Minimum -- 33554432 (2 Ohms in Q24)
        - Maximum -- 1073741824 (64 Ohms in Q24) @tablebulletend */

   int16_t        t0_cali_q6;
   /**< Caliberation point temperature per device. A valid value
        must be supplied both in Calibration mode and Processing mode.

        @values
        - Minimum -- -1920 (-30 degrees C in Q6)
        - Maximum -- 5120 (80 degrees C in Q6) @tablebulletend */

   int16_t        reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_spkr_calib_vi_proc_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_vi
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_VI_PROC to get the
    calibrated resistance value from the VI processing module.
	Only #AFE_PORT_CMD_GET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_fbsp_calib_res_cfg_v1_t}
    @table{weak__afe__fbsp__calib__res__cfg__v1__t}
*/
#define AFE_PARAM_ID_CALIB_RES_CFG    						         0x0001022B

/** Version information used to handle future additions to VI processing
    calibrated resistance configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_CALIB_RES_CFG               				0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_vi */

typedef struct afe_fbsp_calib_res_cfg_v1_t afe_fbsp_calib_res_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_calib_res_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_CALIB_RES_CFG parameter (version 1).
*/
struct afe_fbsp_calib_res_cfg_v1_t
{
   uint32_t       spkr_calib_res_tx_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker VI processing
        calibrated resistance configuration.

        @values #AFE_API_VERSION_CALIB_RES_CFG */

   int32_t        r0_cali_q24;
   /**< Calibrated resistance per device.

        @values
        - Minimum -- 33554432 (2 Ohms in Q24)
        - Maximum -- 1073741824 (64 Ohms in Q24) @tablebulletend */

   uint32_t        th_vi_ca_state;
   /**< Specifies the calibration state.

        @values
        - 0 -- Reserved
        - 1 -- Calibration is in progress
        - 2 -- Calibration was successful
        - 3 -- Calibration has failed @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_calib_res_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_vi
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_VI_PROC to configure
    a feedback connection between speaker protection Tx and Rx processing.
	Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

  @msg_payload{afe_fbsp_feedback_path_cfg_v1_t}
    @table{weak__afe__fbsp__feedback__path__cfg__v1__t}
*/
#define AFE_PARAM_ID_FEEDBACK_PATH_CFG                         0x0001022C

/** Version information used to handle future additions to configuration of
    the feedback connection between speaker protection Tx and Rx processing
    (for backward compatibility).
*/
#define AFE_API_VERSION_FEEDBACK_PATH_CFG                      0x1

/** Vsens comes from the left speaker. */
#define AFE_FBSP_VSENS_LEFT_CHAN				1

/** Isens comes from the left speaker. */
#define AFE_FBSP_ISENS_LEFT_CHAN				2

/** Vsens comes from the right speaker. */
#define AFE_FBSP_VSENS_RIGHT_CHAN				3

/** Isens comes from the right speaker. */
#define AFE_FBSP_ISENS_RIGHT_CHAN				4

/** Maximum number of VI channels supported. @newpage */
#define AFE_FBSP_VI_CHAN_MAX					4

/** @} */ /* end_addtogroup afe_module_feedback_spkr_vi */

typedef struct afe_fbsp_feedback_path_cfg_v1_t afe_fbsp_feedback_path_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_feedback_path_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FEEDBACK_PATH_CFG parameter (version 1).
*/
struct afe_fbsp_feedback_path_cfg_v1_t
{
   uint32_t       feedback_path_cfg_minor_version;
   /**< Minor version used for tracking speaker protection Tx and Rx
        processing configuration.

        @values #AFE_API_VERSION_FEEDBACK_PATH_CFG */

   int32_t        dst_portid;
   /**< Destination Rx port ID. It must be the same Rx port ID for
        which feedback speaker protection is configured.

        @values See <i>Rx Hardware Ports</i> in Appendix @xref{sec:portIDs} */

   int32_t        num_channels;
   /**< Number of Tx port channels. Each Vsens and Isens is considered an
        individual channel.

        @values
        - 2 -- Mono speaker
        - 4 -- Stereo speakers @tablebulletend */

   int32_t        chan_info[AFE_FBSP_VI_CHAN_MAX];
   /**< Channel mapping array that provides information on the order in
        which the Vsens and Isens of different speakers come into a Tx port.

	    @values
        - #AFE_FBSP_VSENS_LEFT_CHAN
			- #AFE_FBSP_ISENS_LEFT_CHAN
			- #AFE_FBSP_VSENS_RIGHT_CHAN
        - #AFE_FBSP_ISENS_RIGHT_CHAN @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_feedback_path_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_vi
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_VI_PROC to configure
    the Tuning mode for feedback speaker VI processing. Only
    #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_tuning_vi_proc_cfg_v1_t}
    @table{weak__afe__tuning__vi__proc__cfg__v1__t}
*/
#define AFE_PARAM_ID_TUNING_VI_PROC_CFG    						         0x0001023f

/** Version information used to handle future additions to feedback speaker
    VI processing Tuning mode configuration (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_TUNING_VI_PROC_CFG               				0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_vi */

typedef struct afe_tuning_vi_proc_cfg_v1_t afe_tuning_vi_proc_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_tuning_vi_proc_cfg_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_TUNING_VI_PROC_CFG parameter (version 1).
*/
struct afe_tuning_vi_proc_cfg_v1_t
{
   uint32_t       tuning_vi_proc_cfg_minor_version;
   /**< Minor version used for tracking feedback speaker VI processing Tuning
        mode configuration. Currently, the supported version is 1.

        @values #AFE_API_VERSION_TUNING_VI_PROC_CFG */

   int32_t        ex_tuning_mode;
   /**< Specifies whether the Tuning mode for excursion VI processing is
        enabled (1) or disabled (0). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_tuning_vi_proc_cfg_v1_t */

/** @addtogroup afe_module_feedback_spkr_vi
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_VI_PROC to log thermal
    statistics for feedback speaker protection thermal VI processing.

	This parameter ID is currently used for logging the algorithm internal
	variables. It is not supported by #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2.

  @msg_payload{afe_fbsp_th_vi_stats_v1_t}
    @table{weak__afe__fbsp__th__vi__stats__v1__t}

    @if OEM_only
    @par Algorithm logging data (afe_afe_algo_log_header_t)
    @tablens{weak__afe__afe__algo__log__header__t}
    @endif
*/
#define AFE_PARAM_ID_FBSP_TH_VI_STATS    							0x00010240

/** Version information used to handle future additions to feedback speaker
    protection thermal VI processing statistics (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_TH_VI_STATS       					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_vi */

typedef struct afe_fbsp_th_vi_stats_v1_t afe_fbsp_th_vi_stats_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_th_vi_stats_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_TH_VI_STATS parameter (version 1).
*/
struct afe_fbsp_th_vi_stats_v1_t
{
   uint32_t       fbsp_th_vi_stats_minor_version;
   /**< Minor version used for tracking feedback speaker protection thermal
        VI processing statistics. Currently, the supported version is 1.

        @values #AFE_API_VERSION_FBSP_TH_VI_STATS */

   uint32_t        frame_duration_in_ms;
   /**< Frame duration in milliseconds for thermal VI processing variables.

        @values Valid uint32 number */

   uint32_t        frame_num;
   /**< Frame number of the thermal variables. This field is reset to zero
        after crossing the maximum limit for a uint32 variable.

        @values Valid uint32 number */

   int32_t        x_r_q24;
   /**< Speaker coil resistance (in Ohms).

        @values Valid int32 number in Q24 format */

   int32_t        x_t_q22;
   /**< Speaker coil temperature (degrees C).

        @values Valid int32 number in Q22 format */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_th_vi_stats_v1_t */

/** @addtogroup afe_module_feedback_spkr_vi
@{ */
/** ID of the parameter used by #AFE_MODULE_FB_SPKR_PROT_VI_PROC to log
    excursion tuning statistics for feedback speaker VI processing.

	This parameter ID is currently used for logging the algorithm internal
	 variables. It is not supported by #AFE_PORT_CMD_SET_PARAM_V2 and
	#AFE_PORT_CMD_GET_PARAM_V2.

    @msg_payload{afe_fbsp_ex_vi_stats_v1_t}
    @table{weak__afe__fbsp__ex__vi__stats__v1__t}

    @if OEM_only
    @par Algorithm logging data (afe_afe_algo_log_header_t)
    @tablens{weak__afe__afe__algo__log__header__t}
    @endif
*/
#define AFE_PARAM_ID_FBSP_EX_VI_STATS    							0x00010241

/** Version information used to handle future additions to feedback speaker
    protection excursion VI processing statistics (for backward compatibility). @newpage
*/
#define AFE_API_VERSION_FBSP_EX_VI_STATS       					0x1

/** @} */ /* end_addtogroup afe_module_feedback_spkr_vi */

typedef struct afe_fbsp_ex_vi_stats_v1_t afe_fbsp_ex_vi_stats_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_fbsp_ex_vi_stats_v1_t
@{ */
/* Configuration structure for the
    AFE_PARAM_ID_FBSP_EX_VI_STATS parameter (version 1).
*/
struct afe_fbsp_ex_vi_stats_v1_t
{
   uint32_t       fbsp_ex_vi_stats_minor_version;
   /**< Minor version used for tracking feedback speaker protection excursion
        VI processing statistics. Currently, the supported version is 1.

        @values #AFE_API_VERSION_FBSP_EX_VI_STATS */

   uint32_t        frame_duration_in_ms;
   /**< Frame duration in milliseconds for excursion variables.

        @values Valid uint32 number */

   uint32_t        frame_num;
   /**< Frame number of the excursion variables. This field is reset to zero
        after crossing the maximum limit for uint32 variables.

        @values Valid uint32 number */

   int32_t        re_l32_q24;
   /**< Voice coil resistance (in Ohms).

        @values Valid int32 number in Q24 format */

   int32_t        le_l32_q24;
   /**< Voice coil inductance (in mH).

        @values Valid int32 number in Q24 format */

   int32_t        bl_l32_q24;
   /**< Force factor.

        @values Valid int32 number in Q24 format */

   int32_t        fct_l32_q20;
   /**< Resonance frequency (in Hz).

        @values Valid int32 number in Q20 format */

   int32_t        q_mct_l32_q24;
   /**< Mechanical Q factor.

        @values Valid int32 number in Q24 format */

   int32_t        rect_l32_q24;
   /**< Mechanical resistance (in Ohms).

        @values Valid int32 number in Q24 format */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_fbsp_ex_vi_stats_v1_t */

/** @ingroup afe_cmd_svc_set_params
    Configures one or more parameters on AFE service. This command is typically
    used to apply global calibration data to AFE service.

  @apr_hdr_fields
    Opcode -- AFE_SVC_CMD_SET_PARAM

  @msg_payload{afe_svc_cmd_set_param_t}
    @table{weak__afe__svc__cmd__set__param__t}

  @par Acknowledgment parameter payload (afe_svc_param_data_t)
    @table{weak__afe__svc__param__data__t}

  @description
    This command is sent from the client to the aDSP to set the parameters of
    AFE service. A single command can be used to set multiple parameters, each
    belonging to multiple modules.
    @par
    Messages can be either in-band or out-of-band via shared memory. The modes
    are:
    - In-band set parameter -- In this mode, the physical address is set to
      NULL. The rest of the payload contains the parameters as shown in the
      variable payload structure (afe_svc_param_data_t).
    - Shared memory set parameter -- In this mode, both the physical address to
      the parameter data and the data payload size are sent. The rest of the
      message is inside the shared memory in the variable payload format. The
      memory content is the same as the in-band payload structure
      (afe_svc_param_data_t).

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
*/
#define AFE_SVC_CMD_SET_PARAM  									0x000100f3

/* Payload of the AFE parameter data. */
typedef struct afe_svc_param_data_t afe_svc_param_data_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_svc_param_data_t
@{ */
/* Payload of the AFE parameter data used by AFE_SVC_CMD_SET_PARAM. */
/** Immediately following this structure is the actual calibration data for the
    module_id/param_id pair.
*/
struct afe_svc_param_data_t
{
   uint32_t module_id;
   /**< ID of the module to be configured.

        @values Valid module ID (See Chapter @xref{hdr:CalibrationIDs}) */

   uint32_t param_id;
   /**< ID of the parameter corresponding to the supported parameters
        for the module ID.

        @values Valid parameter ID (See Chapter @xref{hdr:CalibrationIDs}) */

   uint16_t param_size;
   /**< Actual size of the data for the module_id/param_id pair. The size is a
        multiple of four bytes.

        @values > 0 */

   uint16_t reserved;
   /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_svc_param_data_t */

/* Payload of configuration/calibration settings for the AFE Service. */
typedef struct afe_svc_cmd_set_param_t afe_svc_cmd_set_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_svc_cmd_set_param_t
@{ */
/* Payload of the AFE_SVC_CMD_SET_PARAM command's
    configuration/calibration settings for the AFE service.
*/
struct afe_svc_cmd_set_param_t
{
   uint32_t payload_size;
   /**< Actual size of the payload in bytes. This is used for parsing the
        parameter payload.

        @values  > 0 */

   uint32_t payload_address_lsw;
   /**< Lower 32 bits of the 64-bit payload address. */

   uint32_t payload_address_msw;
   /**< Upper 32 bits of the 64-bit payload address.

        The 64-bit number formed by payload_address_lsw and payload_address_msw
        word must be contiguous memory, and it must be 4 KB aligned.

         @values
         - For a 32 bit-shared memory address, this field must be set to zero.
         - For a 36 bit-shared memory address, bits 31 to 4 must be set to zero.
         - For a 64 bit-shared memory address, any 32-bit value. @tablebulletend */

   uint32_t mem_map_handle;
   /**< Unique identifier for an address.

        @values
        - NULL -- The message is in the payload (in-band).
        - Non-NULL -- The parameter data payload begins at the address
          specified by a pointer to the payload's physical address in shared
          memory (out-of-band).

        This memory map handle is returned by the aDSP through the
        #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command.

        An optional field is available if parameter data is in-band:
        %afe_port_param_data_v2_t param_data[...].
        For detailed payload content, see afe_port_param_data_v2_t. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_svc_cmd_set_param_t */

/** @addtogroup afe_module_sw_mad
 @{ */
/** ID of the software Microphone Activity Detection (MAD) module, which
    supports the following parameter IDs:
 - #AFE_PARAM_ID_SW_MAD_CFG
 - #AFE_PARAM_ID_SVM_MODEL
 - #AFE_PARAM_ID_ENABLE

 @apr_hdr_fields
   Opcode -- AFE_MODULE_ID_SW_MAD @newpage
 */
#define AFE_MODULE_ID_SW_MAD          							 0x0001022D

/** ID of the parameter used by #AFE_MODULE_ID_SW_MAD to configure information for
    the software MAD algorithm. Only #AFE_PORT_CMD_SET_PARAM_V2 can use this
	parameter ID.

    @msg_payload{afe_sw_mad_cfg_param_v1_t}
    @table{weak__afe__sw__mad__cfg__param__v1__t}
 */
#define AFE_PARAM_ID_SW_MAD_CFG     							 0x0001022E

/** Version information used to handle future additions to software MAD
    configuration (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_SW_MAD_CONFIG        					0x1

/** @} */ /* end_addtogroup afe_module_sw_mad */

typedef struct afe_sw_mad_cfg_param_v1_t afe_sw_mad_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_sw_mad_cfg_param_v1_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_SW_MAD_CFG
 module (version 1).
 */
struct afe_sw_mad_cfg_param_v1_t
{
	uint32_t sw_mad_minor_version;
    /**< Minor version used for tracking software MAD configuration.

	    @values #AFE_API_VERSION_SW_MAD_CONFIG */

	int32_t sw_mad_threshQ23;
    /**< Score threshold for the binary classifier.

	    @values
         - Minimum -- -2147483648 (-256 in Q23 format)
         - Maximum -- 2139095040  (255 in Q23 format)
         - Default -- -511573 @tablebulletend */

	int16_t sw_mad_gain_Q8;
    /**< Gain value to multiply to input samples.

	    @values
         - Minimum -- 32786 (-128 in Q8 format)
         - Maximum -- 32767 (127 in Q8 format)
         - Default -- 256 @tablebulletend */

	int16_t reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_sw_mad_cfg_param_v1_t */

/** @addtogroup afe_module_sw_mad
@{ */
/** ID of the Sound Vector Model (SVM) parameter used by #AFE_MODULE_ID_SW_MAD
    to configure information for the software MAD algorithm. Only
	#AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_svm_model_param_v1_t}
    @table{weak__afe__svm__model__param__v1__t}
 */
#define AFE_PARAM_ID_SVM_MODEL    								 0x0001022F

/** SVM version information used to handle future additions to the
    AFE_MODULE_ID_SW_MAD module (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_SVM_MODEL       						0x1

/** @} */ /* end_addtogroup afe_module_sw_mad */

typedef struct afe_svm_model_param_v1_t afe_svm_model_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_svm_model_param_v1_t
@{ */
/* Configuration structure for the #AFE_PARAM_ID_SVM_MODEL
 module (version 1).
 */
struct afe_svm_model_param_v1_t
{
	uint32_t svm_model_minor_version;
    /**< Minor version used for tracking the version of the SVM
         configuration interface.

	    @values #AFE_API_VERSION_SVM_MODEL */

	uint32_t model_size_in_bytes;
    /**< Size in bytes of the SVM.

	    @values All valid sizes*/

	uint32_t svm_model_addr_lsw;
    /**< Lower 32 bits of the 64-bit payload address for the SVM. */

    uint32_t svm_model_addr_msw;
    /**< Upper 32 bits of the 64-bit payload address for the SVM.

         The 64-bit number formed by svm_model_addr_lsw and svm_model_addr_msw
         word must be contiguous memory, and it must be 4 KB aligned.

         @values
         - For a 32 bit-shared memory address, this field must be set to zero.
         - For a 36 bit-shared memory address, bits 31 to 4 must be set to zero.
         - For a 64 bit-shared memory address, any 32-bit value. @tablebulletend */

	uint32_t mem_map_handle;
   /**< Unique identifier for an address.

        @values
        - NULL -- The message is in the payload (in-band).
        - Non-NULL -- The parameter data payload begins at the address
          specified by a pointer to the payload's physical address in shared
          memory (out-of-band).

        This memory map handle is returned by the aDSP through the
        #AFE_SERVICE_CMD_SHARED_MEM_MAP_REGIONS command.

        An optional field is available if parameter data is in-band:
        %afe_port_param_data_v2_t param_data[...].
        For detailed payload content, see afe_port_param_data_v2_t. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_svm_model_param_v1_t */

/** @addtogroup afe_module_hw_mad
@{ */
/** ID of the hardware MAD module, which supports the following parameter IDs:
 - #AFE_PARAM_ID_HW_MAD_CFG
 - #AFE_PARAM_ID_HW_MAD_CTRL
 - #AFE_PARAM_ID_SLIMBUS_SLAVE_PORT_CFG

 @apr_hdr_fields
   Opcode -- AFE_MODULE_HW_MAD @newpage
 */
#define AFE_MODULE_HW_MAD          								 0x00010230

/** ID of the parameter used by #AFE_MODULE_HW_MAD for hardware MAD
    configuration. Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_hw_mad_cfg_param_v1_t}
    @table{weak__afe__hw__mad__cfg__param__v1__t}
 */
#define AFE_PARAM_ID_HW_MAD_CFG     							0x00010231

/** Version information used to handle future additions to hardware MAD
    configuration (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_HW_MAD_CONFIG        					0x1

/** @} */ /* end_addtogroup afe_module_hw_mad */

typedef struct afe_hw_mad_cfg_param_v1_t afe_hw_mad_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_hw_mad_cfg_param_v1_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_HW_MAD_CFG
 parameter (version 1).
 */
struct afe_hw_mad_cfg_param_v1_t
{
	uint32_t hw_mad_minor_version;
    /**< Minor version used for tracking hardware MAD configuration.

	    @values #AFE_API_VERSION_HW_MAD_CONFIG */

	uint16_t audio_sleep_time_ms;
    /**< Sleep time for the MAD audio hardware.

         @values 0 to 60000 ms */

	uint16_t beacon_sleep_time_ms;
    /**< Sleep time for the MAD beacon hardware.

         @values 0 to 60000 ms */

	uint16_t us_sleep_time_ms;
    /**< Sleep time for the MAD ultrasound hardware.

         @values 0 to 60000 ms */

	int16_t reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_hw_mad_cfg_param_v1_t */

/** @addtogroup afe_module_hw_mad
@{ */
/** ID of the parameter used by #AFE_MODULE_HW_MAD to configure hardware MAD
    controls. Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_hw_mad_ctrl_param_v1_t}
    @table{weak__afe__hw__mad__ctrl__param__v1__t}
*/
#define AFE_PARAM_ID_HW_MAD_CTRL     							0x00010232

/** Version information used to handle future additions to hardware MAD control
    configuration (for backward compatibility).
 */
#define AFE_API_VERSION_HW_MAD_CTRL        					0x1

/** Bit to enable/disable hardware MAD audio. */
#define AFE_HW_MAD_AUDIO_BIT            0x0

/** Bit to enable/disable MAD beacon. */
#define AFE_HW_MAD_BEACON_BIT           0x1

/** Bit to enable/disable MAD ultrasound. @newpage */
#define AFE_HW_MAD_ULT_BIT              0x2

/** @} */ /* end_addtogroup afe_module_hw_mad */

typedef struct afe_hw_mad_ctrl_param_v1_t afe_hw_mad_ctrl_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_hw_mad_ctrl_param_v1_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_HW_MAD_CTRL
 parameter (version 1).
 */
struct afe_hw_mad_ctrl_param_v1_t
{
	uint32_t minor_version;
    /**< Minor version used for tracking hardware MAD control configuration.

	    @values #AFE_API_VERSION_HW_MAD_CTRL */

	uint16_t mad_type;
    /**< Type of the MAD hardware block.
         Multiple paths can be enabled (1) or disabled (0) by setting the
         respective bits:
         - Set bit 0 to 1 for #AFE_HW_MAD_AUDIO_BIT
         - Set bit 1 to 1 for #AFE_HW_MAD_BEACON_BIT
         - Set bit 2 to 1 for #AFE_HW_MAD_ULT_BIT @tablebulletend */

	uint16_t mad_enable;
    /**< Enable (TRUE) or disable (FALSE) the MAD hardware paths for audio,
         ultrasound, and beacon. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_hw_mad_ctrl_param_v1_t */

/** @addtogroup afe_module_hw_mad
@{ */
/** ID of the parameter used by #AFE_MODULE_HW_MAD to configure a SLIMbus
    slave port. Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_slimbus_slave_port_cfg_param_v1_t}
    @table{weak__afe__slimbus__slave__port__cfg__param__v1__t}
*/
#define AFE_PARAM_ID_SLIMBUS_SLAVE_PORT_CFG     						0x00010233

/** Version information used to handle future additions to SLIMbus slave
    port configuration (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_SLIMBUS_SLAVE_PORT_CFG        					0x1

/** @} */ /* end_addtogroup afe_module_hw_mad */

typedef struct afe_slimbus_slave_port_cfg_param_v1_t afe_slimbus_slave_port_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_slimbus_slave_port_cfg_param_v1_t
@{ */
/* AFE SLIMbus slave port configuration structure used by the
    AFE_PARAM_ID_SLIMBUS_SLAVE_PORT_CFG parameter (version 1).
 */
struct afe_slimbus_slave_port_cfg_param_v1_t
{
	uint32_t minor_version;
    /**< Minor version used for tracking SLIMbus slave port configuration.

	    @values #AFE_API_VERSION_SLIMBUS_SLAVE_PORT_CFG */

	uint16_t slimbus_dev_id;
    /**< Type of the MAD hardware block.
         Multiple paths can be enabled (1) or disabled (0) by setting the
         respective bits in mad_type.

         @values
         - #AFE_SLIMBUS_DEVICE_1
         - #AFE_SLIMBUS_DEVICE_2 @tablebulletend*/

	uint16_t slave_dev_pgd_la;
	/**< Ported generic device's 8-bit logical address of the slave port.

	    @values 0 to 255*/

	uint16_t slave_dev_intfdev_la;
	/**< Interface device's 8-bit logical address of the slave interface.

	    @values 0 to 255*/

    uint16_t                  bit_width;
    /**< Bit width of the sample.

         @values 16, 24 */

    uint16_t                  data_format;
    /**< Data format supported by the SLIMbus hardware. The default indicates
         the hardware does not perform any format conversions before the data
         transfer.

         @values #AFE_SB_DATA_FORMAT_NOT_INDICATED (Default) */

	uint16_t num_chnnels;
    /**< Number of channels.

        @values 1 to #AFE_PORT_MAX_AUDIO_CHAN_CNT */

	uint16_t slave_port_mapping[AFE_PORT_MAX_AUDIO_CHAN_CNT];
    /**< Mapping of slave port IDs to which the master port is to be
         connected.

        @values 0 to 24*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_slimbus_slave_port_cfg_param_v1_t */

/** @addtogroup afe_module_cdc_device
@{ */
/** ID of the hardware Codec Device Configuration module, which supports the
    following parameter IDs:
 - #AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG
 - #AFE_PARAM_ID_CDC_REG_CFG
 - #AFE_PARAM_ID_CDC_AANC_VERSION
    - #AFE_PARAM_ID_CLIP_BANK_SEL_CFG
    - #AFE_PARAM_ID_CDC_REG_CFG_INIT

 @apr_hdr_fields
    Opcode -- AFE_MODULE_CDC_DEV_CFG @newpage
 */
#define AFE_MODULE_CDC_DEV_CFG     								0x00010234

/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure a codec
    SLIMbus slave device. Only #AFE_SVC_CMD_SET_PARAM can use this parameter ID.

    @msg_payload{afe_cdc_slimbus_slave_cfg_param_v1_t}
    @table{weak__afe__cdc__slimbus__slave__cfg__param__v1__t}
 */
#define AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG						0x00010235

/** Version information used to handle future additions to codec
    SLIMbus device configuration (for backward compatibility). @newpage
 */
#define AFE_API_VERSION_CDC_SLIMBUS_SLAVE_CFG        					0x1

/** @} */ /* end_addtogroup afe_module_cdc_device */

typedef struct afe_cdc_slimbus_slave_cfg_param_v1_t afe_cdc_slimbus_slave_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_slimbus_slave_cfg_param_v1_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_CDC_SLIMBUS_SLAVE_CFG
 module (version 1).
 */
struct afe_cdc_slimbus_slave_cfg_param_v1_t
{
	uint32_t slave_dev_minor_version;
    /**< Minor version used for tracking codec SLIMbus device configuration.

        @values #AFE_API_VERSI ON_CDC_SLIMBUS_SLAVE_CFG */

    uint32_t device_enum_addr_lsw;
    /**< Lower 16 bits of the SLIMbus slave device 48-bit enumeration address.
         The lower half comes first to be consistent with little endian
         architecture. */

    uint32_t device_enum_addr_msw;
    /**< Upper 16 bits of the SLIMbus slave device 48-bit enumeration address.

         @values
         - Bits 15 to 0 contain the upper 16 bits of the address
         - Bits 31 to 16 must be set to zero @tablebulletend */

	uint16_t tx_slave_port_offset;
	/**< Offset value that can be used for Tx slave ports.

         @values Valid offset value based on the hardware design
                 (refer to the applicable hardware interface specification) */

	uint16_t rx_slave_port_offset;
	/**< Offset value that can be used for Rx slave ports.

         @values Valid offset value based on the hardware design
                 (refer to the applicable hardware interface specification) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_slimbus_slave_cfg_param_v1_t */

/** @addtogroup afe_module_cdc_device
@{ */
/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure the
    codec register. Only #AFE_SVC_CMD_SET_PARAM can use this parameter ID.

    @msg_payload{afe_cdc_reg_cfg_param_v1_t}
    @table{weak__afe__cdc__reg__cfg__param__v1__t}
 */
#define AFE_PARAM_ID_CDC_REG_CFG     						0x00010236

/** Version information used to handle future additions to codec register
    configuration (for backward compatibility).
 */
#define AFE_API_VERSION_CDC_REG_CFG        					        0x1

/** @xreflabel{First}
    AANC_REG_LPF_FF_FB (CDC_ANCn_LPF_B1_CTL) register. */
#define AFE_CRC_AANC_LPF_FF_FB										0x1

/** AANC_REG_LPF_COEFF_MSB (CDC_ANCn_LPF_B2_CTL) register. */
#define AFE_CRC_AANC_LPF_COEFF_MSB  								0x2

/** AANC_REG_LPF_COEFF_LSB (CDC_ANCn_LPF_B3_CTL) register. */
#define AFE_CRC_AANC_LPF_COEFF_LSB 								    0x3

/** Hardware register field used to enable MAD audio detection. */
#define AFE_CRC_HW_MAD_AUDIO_ENABLE  								0x4

/** Hardware register field used to enable MAD ultrasound detection. */
#define AFE_CRC_HW_MAD_ULTR_ENABLE									0x5

/** Hardware register field used to enable MAD beacon detection. */
#define AFE_CRC_HW_MAD_BEACON_ENABLE  								0x6

/** Hardware register field used to configure MAD audio sleep time. */
#define AFE_CRC_HW_MAD_AUDIO_SLEEP_TIME								0x7

/** Hardware register field used to configure MAD ultrasound sleep time. */
#define AFE_CRC_HW_MAD_ULTR_SLEEP_TIME  							0x8

/** Hardware register field used to configure MAD beacon sleep time. */
#define AFE_CRC_HW_MAD_BEACON_SLEEP_TIME							0x9

/** Hardware register field used to switch off the MAD Tx audio path after
    detection. */
#define AFE_CRC_HW_MAD_TX_AUDIO_SWITCH_OFF							0xA

/** Hardware register field used to switch off the MAD Tx ultrasound path after
    detection. */
#define AFE_CRC_HW_MAD_TX_ULTR_SWITCH_OFF							0xB

/** Hardware register field used to switch off the MAD Tx beacon path after
    detection. */
#define AFE_CRC_HW_MAD_TX_BEACON_SWITCH_OFF							0xC

/** Hardware register field used to route the MAD audio interrupt. */
#define AFE_CRC_MAD_AUDIO_INT_DEST_SELECT_REG						0xD

/** Hardware register field used to route the MAD ultrasound interrupt. */
#define AFE_CRC_MAD_ULT_INT_DEST_SELECT_REG							0xE

/** Hardware register field used to route the MAD beacon interrupt. */
#define AFE_CRC_MAD_BEACON_INT_DEST_SELECT_REG						0xF

/** Hardware register field used to route the audio clip interrupt. */
#define AFE_CRC_CLIP_INT_DEST_SELECT_REG							0x10

/** Hardware register field used to route the Battery Voltage (VBAT) attack
    interrupt. */
#define AFE_CRC_VBAT_INT_DEST_SELECT_REG						    0x11

/** More meaningful macro for AFE_CRC_VBAT_INT_DEST_SELECT_REG */
#define AFE_CRC_VBAT_ATTACK_INT_DEST_SELECT_REG						AFE_CRC_VBAT_INT_DEST_SELECT_REG

/** Hardware register field used to specify the MAD audio interrupt mask
    field. */
#define AFE_CRC_MAD_AUDIO_INT_MASK_REG								0x12

/** Hardware register field used to specify the MAD ultrasound interrupt mask
    field. */
#define AFE_CRC_MAD_ULT_INT_MASK_REG								0x13

/** Hardware register field used to specify the MAD beacon interrupt mask
    field. */
#define AFE_CRC_MAD_BEACON_INT_MASK_REG								0x14

/** Hardware register field used to specify the audio clip interrupt mask
    field. */
#define AFE_CRC_CLIP_INT_MASK_REG									0x15

/** Hardware register field used to specify the VBAT attack interrupt mask
    field. */
#define AFE_CRC_VBAT_INT_MASK_REG							        0x16

/** More meaningful macro for AFE_CRC_VBAT_INT_MASK_REG */
#define AFE_CRC_VBAT_ATTACK_INT_MASK_REG							AFE_CRC_VBAT_INT_MASK_REG

/** Hardware register field used to specify the MAD audio interrupt status
    field. */
#define AFE_CRC_MAD_AUDIO_INT_STATUS_REG							0x17

/** Hardware register field used to specify the MAD ultrasound interrupt
    status field. */
#define AFE_CRC_MAD_ULT_INT_STATUS_REG								0x18

/** Hardware register field used to specify the MAD beacon interrupt status
    field. */
#define AFE_CRC_MAD_BEACON_INT_STATUS_REG							0x19

/** Hardware register field used to specify the audio clip interrupt status
    field. */
#define AFE_CRC_CLIP_INT_STATUS_REG									0x1A

/** Hardware register field used to specify the VBAT attack interrupt
    status field. */
#define AFE_CRC_VBAT_INT_STATUS_REG						            0x1B

/** More meaningful macro for AFE_CRC_VBAT_INT_STATUS_REG */
#define AFE_CRC_VBAT_ATTACK_INT_STATUS_REG						    AFE_CRC_VBAT_INT_STATUS_REG

/** Hardware register field used to specify the MAD audio interrupt clear
    field. */
#define AFE_CRC_MAD_AUDIO_INT_CLEAR_REG								0x1C

/** Hardware register field used to specify the MAD ultrasound interrupt clear
    field. */
#define AFE_CRC_MAD_ULT_INT_CLEAR_REG								0x1D

/** Hardware register field used to specify the MAD beacon interrupt clear
    field. */
#define AFE_CRC_MAD_BEACON_INT_CLEAR_REG							0x1E

/** Hardware register field used to specify the audio clip interrupt clear
    field. */
#define AFE_CRC_CLIP_INT_CLEAR_REG									0x1F

/** Hardware register field used to specify the VBAT attack interrupt clear
    field. */
#define AFE_CRC_VBAT_INT_CLEAR_REG							        0x20

/** More meaningful macro for AFE_CRC_VBAT_INT_CLEAR_REG */
#define AFE_CRC_VBAT_ATTACK_INT_CLEAR_REG					        AFE_CRC_VBAT_INT_CLEAR_REG

/** Hardware register field used to specify the SLIMbus slave port (Tx)
    watermark configuration field. */
#define AFE_CRC_SB_PGD_PORT_TX_WATERMARK_n							0x21

/** Hardware register field used to specify the SLIMbus slave port (Tx)
    enable/disable configuration field. */
#define AFE_CRC_SB_PGD_PORT_TX_ENABLE_n								0x22

/** Hardware register field used to specify the SLIMbus slave port (Rx)
    watermark configuration field. */
#define AFE_CRC_SB_PGD_PORT_RX_WATERMARK_n							0x23

/** Hardware register field used to specify the SLIMbus slave port (Rx)
    enable/disable configuration field. */
#define AFE_CRC_SB_PGD_PORT_RX_ENABLE_n								0x24

/** Hardware register field used to enable SLIMbus multichannel 0 (Tx) for
    the given slave port. */
#define AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_0 0x25

/** Hardware register field used to enable SLIMbus multichannel 1 (Tx) for
    the given slave port. */
#define AFE_CRC_SB_PGD_TX_PORTn_MULTI_CHNL_1 0x26

/** Hardware register field used to enable SLIMbus multichannel 0 (Rx) for
    the given slave port. */
#define AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_0 0x27

/** Hardware register field used to enable SLIMbus multichannel 1 (Rx) for
    the given slave port. */
#define AFE_CRC_SB_PGD_RX_PORTn_MULTI_CHNL_1 0x28

/** Hardware register field used to enable the double banking, FFGAIN_ADAPTIVE
    bit (CDC_ANCn_IIR_B1_CTL). */
#define AFE_CRC_AANC_FF_GAIN_ADAPTIVE          0x29

/** Hardware register field used to copy AANC gain from the foreground to the
    background register (CDC_ANCn_IIR_B1_CTL). */
#define AFE_CRC_AANC_FFGAIN_ADAPTIVE_EN  0x2A

/** Hardware register field used to update AANC gain (CDC_ANCn_GAIN_CTL). */
#define AFE_CRC_AANC_GAIN_CONTROL            0x2B

/** Hardware register field used to update clip PCM-level bank select
    (CDC_SPKR_CLIPDET_B1_CTL). */
#define AFE_CRC_CLIP_PIPE_BANK_SEL            0x2C

/** Hardware register field used to update clip PCM-level register 0. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL0            0x2D

/** Hardware register field used to update clip PCM-level register 1. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL1            0x2E

/** Hardware register field used to update clip PCM-level register 2. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL2            0x2F

/** Hardware register field used to update clip PCM-level register 3. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL3            0x30

/** Hardware register field used to update clip PCM-level register 4. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL4            0x31

/** Hardware register field used to update clip PCM-level register 5. */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL5            0x32

/** Hardware register field used to update clip PCM-level register 6.*/
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL6            0x33

/** @xreflabel{Last}
    Hardware register field used to update clip PCM-level register 7. @newpage */
#define AFE_CRC_CDC_SPKR_CLIPDET_VAL7            0x34

/** Hardware register field used to route the Battery Voltage (VBAT) release
    interrupt. */
#define AFE_CRC_VBAT_RELEASE_INT_DEST_SELECT_REG						    0x35
/** Hardware register field used to specify the VBAT release interrupt mask
    field. */
#define AFE_CRC_VBAT_RELEASE_INT_MASK_REG							        0x36
/** Hardware register field used to specify the VBAT release interrupt
    status field. */
#define AFE_CRC_VBAT_RELEASE_INT_STATUS_REG						            0x37
/** Hardware register field used to specify the VBAT release interrupt clear
    field. */
#define AFE_CRC_VBAT_RELEASE_INT_CLEAR_REG							        0x38

/** @} */ /* end_addtogroup afe_module_cdc_device */

typedef struct afe_cdc_reg_cfg_param_v1_t afe_cdc_reg_cfg_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_reg_cfg_param_v1_t
@{ */
/* Hardware codec register configuration structure used by the
    AFE_PARAM_ID_CDC_REG_CFG parameter (version 1).
 */
struct afe_cdc_reg_cfg_param_v1_t
{
	uint32_t cdc_reg_cfg_minor_version;
    /**< Minor version used for tracking codec register configuration.

        @values #AFE_API_VERSION_CDC_REG_CFG */

	uint32_t reg_logical_addr;
    /**< Logical address of the value element mapped to the SLIMbus codec
         register to which read/write access is required.

        @values 32-bit logical address of the register to be accessed */

	uint32_t reg_field_type;
    /**< Enumeration for the register field to be updated.

         If more than one field in the register is to be updated, the same
         command must be sent for each field.

	    @values
         - 0 is reserved
         - See Sections @xref{First} through @xref{Last} @tablebulletend */

   uint32_t reg_field_bit_mask;
   /**< Register bitmask to which the ANC algorithm writes.

        @values Valid register bitmask */

   uint16_t reg_bit_width;
   /**< Bit width of the register.

        @values Valid register bit width */

   uint16_t reg_offset_scale;
   /**< Offset scale factor for codec registers.
        For example, 0x400+1*n, where 1 is reg_offset_scale.

        @values Valid register offset scale, if any (Default = 0) */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_reg_cfg_param_v1_t */

/** @addtogroup afe_module_cdc_device
 @{ */
/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure codec AANC.
	Only #AFE_SVC_CMD_SET_PARAM can use this parameter ID.

    @msg_payload{afe_cdc_aanc_version_param_v1_t}
    @table{weak__afe__cdc__aanc__version__param__v1__t}
 */
#define AFE_PARAM_ID_CDC_AANC_VERSION						0x0001023A

/** Version information used to handle future additions to codec AANC
    configuration (for backward compatibility).
 */
#define AFE_API_VERSION_CDC_AANC_VERSION        					0x1

/** AANC hardware block version 1.

    This version must be selected by applications when the
    #AFE_CRC_AANC_LPF_FF_FB, #AFE_CRC_AANC_LPF_COEFF_MSB, and
    #AFE_CRC_AANC_LPF_COEFF_LSB registers are used for gain updates.
    For example, for the Tabla codec.
 */
#define AFE_AANC_HW_BLOCK_VERSION_1                                 0x1

/** AANC hardware block version 2.

    This version must be selected by applications when the
    #AFE_CRC_AANC_FF_GAIN_ADAPTIVE, #AFE_CRC_AANC_FFGAIN_ADAPTIVE_EN,
    and #AFE_CRC_AANC_GAIN_CONTROL registers are used for gain updates.
    For example, for the Taiko codec. @newpage
 */
#define AFE_AANC_HW_BLOCK_VERSION_2                                 0x2

/** @} */ /* end_addtogroup afe_module_cdc_device */

typedef struct afe_cdc_aanc_version_param_v1_t afe_cdc_aanc_version_param_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_cdc_aanc_version_param_v1_t
@{ */
/* Configuration structure for the AFE_PARAM_ID_CDC_AANC_VERSION parameter
  (version 1).
 */
struct afe_cdc_aanc_version_param_v1_t
{
	uint32_t cdc_aanc_minor_version;
    /**< Minor version used for tracking codec AANC configuration.

         @values #AFE_API_VERSION_CDC_AANC_VERSION */

    uint32_t aanc_hw_version;
    /**< Type of AANC hardware version present in the codec.

         @values
         - #AFE_AANC_HW_BLOCK_VERSION_1
         - #AFE_AANC_HW_BLOCK_VERSION_2 @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_cdc_aanc_version_param_v1_t */

/** @addtogroup afe_module_cdc_device
 @{ */
/** ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to configure
    clip bank select. Only #AFE_SVC_CMD_SET_PARAM can use this parameter ID.

    @msg_payload{afe_clip_bank_sel_cfg_v1_t}
    @table{weak__afe__clip__bank__sel__cfg__v1__t}
 */
#define AFE_PARAM_ID_CLIP_BANK_SEL_CFG						0x00010242

/** Version information used to handle future additions to clip bank
    select configuration (for backward compatibility).
 */
#define AFE_API_VERSION_CLIP_BANK_SEL_CFG        					0x1

/** Maximum number of banks allowed for reading the PCM levels in the event of
    a clip interrupt. @newpage
 */
#define AFE_CLIP_MAX_BANKS                                 			0x4

/** @} */ /* end_addtogroup afe_module_cdc_device */

typedef struct afe_clip_bank_sel_cfg_v1_t afe_clip_bank_sel_cfg_v1_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_clip_bank_sel_cfg_v1_t
@{ */
/* Configuration structure for the
  AFE_PARAM_ID_CLIP_BANK_SEL_CFG parameter (version 1).
 */
struct afe_clip_bank_sel_cfg_v1_t
{
	uint32_t clip_bank_sel_cfg_minor_version;
	/**< Minor version used for tracking bank selection configuration.

      @values #AFE_API_VERSION_CLIP_BANK_SEL_CFG */

    uint32_t num_banks;
    /**< Number of banks to be read.

         @values 2 or 4 */

	uint32_t bank_map[AFE_CLIP_MAX_BANKS];
    /**< Bank number to be read.

         If the number of banks is 2, the bank map must be
         [1, 3, DC, DC] or [0, 2, DC, DC], where DC means ``do not care.''

         @values 0 to 3 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_clip_bank_sel_cfg_v1_t */

/** @ingroup afe_module_cdc_device
    ID of the parameter used by #AFE_MODULE_CDC_DEV_CFG to initialize codec
    device configuration.

    This parameter must be sent to the aDSP after sending all device
    configuration parameters so that initialization takes affect on all the
    configurations. Only #AFE_SVC_CMD_SET_PARAM can use this parameter ID.

    This parameter does not contain an explict payload and must be sent with
    the AFE_MODULE_CDC_DEV_CFG module.
 */
#define AFE_PARAM_ID_CDC_REG_CFG_INIT    						0x00010237

/** @addtogroup afe_module_device
 @{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    a Low Power Audio Interface (LPAIF) clock (I2S/PCM bit clock and OSR).
    - An LPAIF clock must be enabled before an #AFE_PORT_CMD_DEVICE_START command
      is issued.
    - An LPAIF clock must be disabled after an #AFE_PORT_CMD_DEVICE_STOP command
      is issued.

    Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.
    @msg_payload{afe_param_id_lpaif_clk_cfg_t}
    The payload for this parameter must be sent with the
    #AFE_MODULE_AUDIO_DEV_INTERFACE module.
    @tablens{weak__afe__param__id__lpaif__clk__cfg__t}
 */
#define AFE_PARAM_ID_LPAIF_CLK_CONFIG                0x00010238

/** Version information used to handle future additions to LPAIF clock
    configuration (for backward compatibility).
 */
#define AFE_API_VERSION_LPAIF_CLK_CONFIG               0x1

/* Enumeration for setting LPAIF clock source clk_src. */

/** LPAIF clock source is external. */
#define AFE_PORT_LPACLK_CLK_SRC_EXTERNAL                                     0x0

/** LPAIF clock source is internal. */
#define AFE_PORT_LPACLK_CLK_SRC_INTERNAL                                     0x1

/* Enumeration for setting LPAIF clock root clk_root. */

/** LPAIF clock is sourced from the audio PLL. */
#define AFE_PORT_LPACLK_CLK_ROOT_DEFAULT                                     0x0

/** LPAIF clock is sourced from the PXO. */
#define AFE_PORT_LPACLK_CLK_ROOT_PXO                                         0x1

/** LPAIF clock is sourced from the CXO. */
#define AFE_PORT_LPACLK_CLK_ROOT_CXO                                         0x2

/** LPAIF clock is sourced from LPA PLL. */
#define AFE_PORT_LPACLK_CLK_ROOT_LPAPLL                                      0x3

/** LPAIF clock is sourced from the second PLL. */
#define AFE_PORT_LPACLK_CLK_ROOT_SECPLL                                      0x4

/** LPAIF clock is sourced from LPA QDSP6 PLL. */
#define AFE_PORT_LPACLK_CLK_ROOT_LPAQ6PLL                                    0x5

/** LPAIF clock is sourced from pullable PLL 1. */
#define AFE_PORT_LPACLK_CLK_ROOT_PULLABLEPLL1                                0x6

/** LPAIF clock is sourced from pullable PLL 2. */
#define AFE_PORT_LPACLK_CLK_ROOT_PULLABLEPLL2                                0x7

/* Enumeration for specifying value1 and value2 valid setting via clk_set_mode.*/

/** Both clk_value1 and clk_value2 in afe_param_id_lpaif_clk_cfg_t are invalid. */
#define AFE_PORT_LPACLK_CLK_VALUE_INVALID                                    0x0

/** Only clk_value1 in afe_param_id_lpaif_clk_cfg_t is valid. */
#define AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY                                0x1

/** Only clk_value2 in afe_param_id_lpaif_clk_cfg_t is valid. */
#define AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY                                0x2

/** Both clk_value1 and clk_value2 in afe_param_id_lpaif_clk_cfg_t are valid. @newpage */
#define AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID                         0x3

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_lpaif_clk_cfg_t afe_param_id_lpaif_clk_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_lpaif_clk_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_LPAIF_CLK_CONFIG parameter.
*/
struct afe_param_id_lpaif_clk_cfg_t
{
    uint32_t                  lpaif_cfg_minor_version;
    /**< Minor version used for tracking LPAIF clock configuration.

         @values #AFE_API_VERSION_LPAIF_CLK_CONFIG */

    uint32_t                 clk_value1;
    /**< Clock frequency value 1 (in Hz) to be set.

         @values
         - I2S bit clock for an MI2S port, or PCM bit clock for a PCM port
         - 0 to disable the clock @tablebulletend */

    uint32_t                 clk_value2;
    /**< Clock frequency value 2 (in Hz) to be set.

         @values
         - I2S OSR for an MI2S port
         - 0 to disable the clock @tablebulletend */

    uint16_t                 clk_src;
    /**< Specifies I2S WS or PCM SYNC as the clock source.

         @values
         - #AFE_PORT_LPACLK_CLK_SRC_EXTERNAL
         - #AFE_PORT_LPACLK_CLK_SRC_INTERNAL @tablebulletend */

    uint16_t                 clk_root;
    /**< Specifies I2S or PCM as the root clock source.
         Currently, only AFE_PORT_LPACLK_CLK_ROOT_DEFAULT is valid.

         @values
         - #AFE_PORT_LPACLK_CLK_ROOT_DEFAULT
         - #AFE_PORT_LPACLK_CLK_ROOT_PXO
         - #AFE_PORT_LPACLK_CLK_ROOT_CXO
         - #AFE_PORT_LPACLK_CLK_ROOT_LPAPLL
         - #AFE_PORT_LPACLK_CLK_ROOT_SECPLL
         - #AFE_PORT_LPACLK_CLK_ROOT_LPAQ6PLL
         - #AFE_PORT_LPACLK_CLK_ROOT_PULLABLEPLL1
         - #AFE_PORT_LPACLK_CLK_ROOT_PULLABLEPLL2 @tablebulletend */

    uint16_t                 clk_set_mode;
      /**< Valid setting for clk_value1 and clk_value2 fields.

           @values
           - #AFE_PORT_LPACLK_CLK_VALUE_INVALID
           - #AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY
           - #AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY
           - #AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID @vertspace{-28} */

    uint16_t                 reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_lpaif_clk_cfg_t */

/** @addtogroup afe_module_device
 @{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    a SPDIF clock.
    - An SPDIF clock must be enabled before an #AFE_PORT_CMD_DEVICE_START command
      is issued.
    - An SPDIF clock must be disabled after an #AFE_PORT_CMD_DEVICE_STOP command
      is issued.

    @msg_payload{afe_param_id_spdif_clk_cfg_t}
    The payload for this parameter must be sent with the
    #AFE_MODULE_AUDIO_DEV_INTERFACE module.
    @tablens{weak__afe__param__id__spdif__clk__cfg__t}
 */
#define AFE_PARAM_ID_SPDIF_CLK_CONFIG                0x00010246

/** Version information used to handle future additions to SPDIF clock
    configuration (for backward compatibility).
 */
#define AFE_API_VERSION_SPDIF_CLK_CONFIG               0x1

/* Enumeration for setting clock root clk_root. */

/** clock is sourced from the PXO. */
#define AFE_PORT_CLK_ROOT_PXO                                         0x1

/** clock is sourced from the CXO. */
#define AFE_PORT_CLK_ROOT_CXO                                         0x2

/** clock is sourced from LPA PLL. */
#define AFE_PORT_CLK_ROOT_LPAPLL                                      0x3

/** clock is sourced from LPA QDSP6 PLL. */
#define AFE_PORT_CLK_ROOT_LPAQ6PLL                                    0x4


/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_spdif_clk_cfg_t afe_param_id_spdif_clk_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_spdif_clk_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_SPDIF_CLK_CONFIG parameter.
*/
struct afe_param_id_spdif_clk_cfg_t
{
    uint32_t                  spdif_clk_cfg_minor_version;
    /**< Minor version used for tracking SPDIF clock configuration.

         @values #AFE_API_VERSION_SPDIF_CLK_CONFIG */

    uint32_t                 clk_value;
    /**< Clock frequency value 1 (in Hz) to be set as twice the bit-rate.

         - 2 (bypase) * 32(60958 subframe size) * samplingrate *2 (channels A and B)
         - 0 to disable the clock @tablebulletend */

    uint32_t                 clk_root;
    /**< Specifies I2S or PCM as the root clock source.

         @values
         - #AFE_PORT_CLK_ROOT_LPAPLL
         - #AFE_PORT_CLK_ROOT_LPAQ6PLL  @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_spdif_clk_cfg_t */

/** @addtogroup afe_module_device
 @{ */
/** ID of the parameter used by #AFE_MODULE_AUDIO_DEV_INTERFACE to configure
    the internal digital codec core clock.

    The call sequence for this clock is:
    -# Enable the MI2S OSR clock if the client selects MI2S OSR as the
       internal digital core clock source.
    -# Enable the internal digital core clock.
    -# The clock is running.
    -# Disable the internal digital core clock.
    -# Disable the MI2S OSR clock if the client selects MI2S OSR as the
       internal digital core clock source.

    Only #AFE_PORT_CMD_SET_PARAM_V2 can use this parameter ID.

    @msg_payload{afe_param_id_internal_digital_cdc_clk_cfg_t}
    The payload must be sent with the #AFE_MODULE_AUDIO_DEV_INTERFACE module.
    @tablens{weak__afe__param__id__internal__digital__cdc__clk__cfg__t}
 */
#define AFE_PARAM_ID_INTERNAL_DIGIATL_CDC_CLK_CONFIG                0x00010239

/** Version information used to handle new additions to internal digital codec
    core clock configuration (for backward compatibility).
 */
#define AFE_API_VERSION_INTERNAL_DIGIATL_CDC_CLK_CONFIG               0x1

/* Enumerations for setting digital codec core clock root. */

/** Digital codec core clock root is invalid. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_INVALID                  0x0

/** Digital codec core clock root is Primary I2S OSR. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_PRII2SOSR                0x1

/** Digital codec core clock root is Secondary I2S OSR. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_SECI2SOSR                0x2

/** Digital codec core clock root is Tertiary I2S OSR. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_TERI2SOSR                0x3

/** Digital codec core clock root is Quaternary I2S OSR. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_QUAI2SOSR                0x4

/** Digital codec core clock root is Codec. */
#define AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_CODEC                    0x5

/** @} */ /* end_addtogroup afe_module_device */

typedef struct afe_param_id_internal_digital_cdc_clk_cfg_t afe_param_id_internal_digital_cdc_clk_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_afe_param_id_internal_digital_cdc_clk_cfg_t
@{ */
/* Payload of the AFE_PARAM_ID_INTERNAL_DIGIATL_CDC_CLK_CONFIG command's
    internal digital codec core clock configuration parameter.
*/
struct afe_param_id_internal_digital_cdc_clk_cfg_t
{
    uint32_t                  cdc_clk_cfg_minor_version;
    /**< Minor version used for tracking the internal digital
         codec core clock configuration.

         @values #AFE_API_VERSION_INTERNAL_DIGIATL_CDC_CLK_CONFIG */

    uint32_t                 clk_value;
    /**< Digital codec core clock frequency in Hz. */

    uint16_t                 clk_root;
    /**< Source of the digital codec core clock.

         @values
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_INVALID
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_PRII2SOSR
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_SECI2SOSR
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_TERI2SOSR
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_QUAI2SOSR
         - #AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_CODEC @vertspace{-28} */

    uint16_t                 reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_afe_param_id_internal_digital_cdc_clk_cfg_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* ADSP_AFE_SERVICE_COMMANDS_H_ */
