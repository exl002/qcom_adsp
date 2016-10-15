/*========================================================================*/
/**
@file adsp_usm_stream_commands.h

@brief This file contains USM stream commands and events structures definitions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Ultrasound_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Ultrasound_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/ultrasound/inc/adsp_usm_stream_commands.h#10 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/06/12   sw      (Tech Pubs) Updated legal statements (lines 20 & 22) for QTI;
                   updated Doxygen markup for D.2 templates; edited new comments.
04/15/12   NR      Update signal detection command 
02/14/12   sw      (Tech Pubs) Edited Doxygen markup/comments for Rev B of the IS.
01/04/12   lb      Addition of signal detection command and result event
10/18/11   sw      (Tech Pubs) Edited Doxygen markup/comments for new Interface Spec.
10/13/10   NR      File created

========================================================================== */

#ifndef _ADSP_USM_STREAM_COMMANDS_H_
#define _ADSP_USM_STREAM_COMMANDS_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @ingroup usm_stream_cmd_open_write
    Creates a new ultrasound Rx stream. There is one decoder per Rx stream.

  @apr_hdr_fields
    Opcode -- USM_STREAM_CMD_OPEN_WRITE \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION
 
  @apr_msg_payload{usm_stream_cmd_open_write_t}
    @tablens{weak__usm__stream__cmd__open__write__t}

  @detdesc
    The command simply provides the media format of the decoding to be
    performed by the aDSP. The decoder configuration block must be provided to
    the aDSP through #USM_DATA_CMD_MEDIA_FMT_UPDATE before sending data
    buffers with data to be decoded by the aDSP.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    The handle is defined by the client as an input.

  @dependencies
    None.
*/
/* Q3 = Asynchronous Packet Router API (for MDM9x15 and Newer) Interface Spec
   (80-N1463-2) */
#define USM_STREAM_CMD_OPEN_WRITE                                   0x00011271

/* Payload format for stream open write command */
typedef struct usm_stream_cmd_open_write_t usm_stream_cmd_open_write_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_stream_cmd_open_write_t
@{ */
/* Payload of the USM_STREAM_CMD_OPEN_WRITE command, which opens a
    write stream for a given session ID and stream ID. */
/** The dst_port field in the APR packet contains both the session ID and the
    stream ID. \n
    \n
    After successfully opening the stream, a media format update command must
    be sent on the data path to configure the decoder before sending any data.
*/
struct usm_stream_cmd_open_write_t
{
    uint32_t                    dec_fmt_id;
    /**< Configuration ID of the decoder media format.
         
         @values #USM_MEDIA_FMT_FREE_FORM */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_stream_cmd_open_write_t */

/** @ingroup usm_stream_cmd_open_read
    Creates a new ultrasound Tx stream. There is one encoder per stream.

  @apr_hdr_fields
    Opcode -- USM_STREAM_CMD_OPEN_READ \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION

  @apr_msg_payload{usm_stream_cmd_open_read_t}
    @tablens{weak__usm__stream__cmd__open__read__t}

  @detdesc
    The command simply provides the media format of the encoding to be
    performed by the aDSP. The encoder configuration block must be provided to
    the aDSP through #USM_STREAM_CMD_SET_ENCDEC_PARAM before sending data
    buffers to get the encoded data from the aDSP. The
    USM_STREAM_CMD_SET_ENCDEC_PARAM command can be sent only once.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    The handle is defined by the client as an input.

  @dependencies
    None.
*/
#define USM_STREAM_CMD_OPEN_READ                                    0x00012309

/* Payload format for stream open read command */
typedef struct usm_stream_cmd_open_read_t usm_stream_cmd_open_read_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_stream_cmd_open_read_t
@{ */
/* Payload of the USM_STREAM_CMD_OPEN_READ command, which opens a
    read stream for a given session ID and stream ID. */
/** The dst_port field in the APR packet contains both the session ID and the
    stream ID.
*/
struct usm_stream_cmd_open_read_t
{
    uint32_t                    reserved0;
    /**< This field must be set to zero. */

    uint32_t                    reserved1;
    /**< This field must be set to zero. */

    uint32_t                    reserved2;
    /**< This field must be set to zero. */

    uint32_t                    enc_cfg_id;
    /**< Media configuration ID for encoded output.

         @values
         - #USM_MEDIA_FMT_EPOS_STYLUS
         - #USM_MEDIA_FMT_FREE_FORM @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_stream_cmd_open_read_t */

/** @ingroup usm_stream_cmd_close
    Closes a USM stream (provided by the dst_port).

  @apr_hdr_fields
    Opcode -- USM_STREAM_CMD_CLOSE \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION

  @apr_msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}.

  @dependencies
    None.
*/
#define USM_STREAM_CMD_CLOSE                                        0x0001230A


/** @ingroup usm_stream_cmd_set_params
    Sets one parameter for the encoder on a stream. This is typically used to
    change encoder settings during runtime.

    @note1hang To set/change decoder parameters, use
               #USM_DATA_CMD_MEDIA_FMT_UPDATE.

  @apr_hdr_fields
    Opcode -- USM_STREAM_CMD_SET_ENCDEC_PARAM \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION

  @apr_msg_payload{usm_stream_cmd_set_encdec_param_t}
    @tablens{weak__usm__stream__cmd__set__encdec__param__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    This command must be applied to a valid stream. @newpage
*/
#define USM_STREAM_CMD_SET_ENCDEC_PARAM                             0x0001230B

/* Payload format for USM stream set encdec param command */
typedef struct usm_stream_cmd_set_encdec_param_t usm_stream_cmd_set_encdec_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_stream_cmd_set_encdec_param_t
@{ */
/* Payload of the USM_STREAM_CMD_SET_ENCDEC_PARAM command, which
    sets one parameter for the encoder/decoder on a stream. */
/** Immediately following this structure are param_size bytes representing
    the parameter data. The size depends on the param_id/param_size
    combination.
*/
struct usm_stream_cmd_set_encdec_param_t
{
    uint32_t                  param_id;
    /**< ID of the parameter to be configured.

         @values
         - #USM_PARAM_ID_ENCDEC_BITRATE
         - #USM_PARAM_ID_ENCDEC_ENC_CFG_BLK @tablebulletend */

    uint32_t                  param_size;
    /**< Data size of this parameter. The size is a multiple of
         4 bytes.

         @values > 0 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_stream_cmd_set_encdec_param_t */

/** @ingroup usm_stream_cmd_set_params
    ID of the bitrate parameter used by the #USM_STREAM_CMD_SET_ENCDEC_PARAM
    command.

    @apr_msg_payload{usm_bitrate_param_t}
    @table{weak__usm__bitrate__param__t}
*/
#define USM_PARAM_ID_ENCDEC_BITRATE                                 0x0001230C

/* Structure for bit rate param */
typedef struct usm_bitrate_param_t usm_bitrate_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_bitrate_param_t
@{ */
/* Payload of the USM_PARAM_ID_ENCDEC_BITRATE parameter in the
    USM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct usm_bitrate_param_t
{
    uint32_t                  bitrate;
    /**< Maximum bitrate.

         Currently, this bitrate parameter is not supported. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_bitrate_param_t */
 
/** @ingroup usm_stream_cmd_set_params
    ID of the encoder configuration block parameter used by the
    #USM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @par Encoder configuration block (usm_enc_cfg_blk_param_t)
    @tablens{weak__usm__enc__cfg__blk__param__t}
*/
#define USM_PARAM_ID_ENCDEC_ENC_CFG_BLK                             0x0001230D

/* Structure for encoder configuration block */
typedef struct usm_enc_cfg_blk_param_t usm_enc_cfg_blk_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_enc_cfg_blk_param_t
@{ */
/* Payload of the USM_PARAM_ID_ENCDEC_ENC_CFG_BLK parameter in the
    USM_STREAM_CMD_SET_ENCDEC_PARAM command. */
/** Immediately following this structure is an encoder configuration block
    (usm_free_form_enc_cfg_t or usm_epos_stylus_enc_cfg_t) of size
    enc_cfg_blk_size bytes.
*/
struct usm_enc_cfg_blk_param_t
{
    uint32_t                  frames_per_buf;
    /**< Number of encoded frames to pack into each buffer. */

    uint32_t                  enc_cfg_id;
    /**< Configuration ID of the encoded data.
         
         @values           
         - #USM_MEDIA_FMT_FREE_FORM
         - #USM_MEDIA_FMT_EPOS_STYLUS @tablebulletend */

    uint32_t                  enc_cfg_blk_size;
    /**< Size in bytes of the encoder configuration block that follows this
         parameter.
     */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_enc_cfg_blk_param_t */

/** @addtogroup usm_stream_cmd_set_signal_detect
@{ */

/** Signals the ultrasound encoder to enter a Signal Detection mode, in which
    it searches for an ultrasound signal, or to stop Signal Detection mode.
    This command is applicable only for EPOS Tx streams.

  @apr_hdr_fields
    Opcode -- USM_STREAM_CMD_SET_SIGNAL_DETECT_MODE \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION

  @apr_msg_payload{usm_stream_cmd_set_signal_detect_mode_t}
    @table{weak__usm__stream__cmd__set__signal__detect__mode__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]})

  @dependencies
    This command can be sent only after a proper
    #USM_STREAM_CMD_SET_ENCDEC_PARAM has configured the encoder for EPOS Tx.
    @par
    This command can be sent both before and after the #USM_SESSION_CMD_RUN
    has been sent.
*/
#define USM_STREAM_CMD_SET_SIGNAL_DETECT_MODE                         0x00012719

/** Deactivates Signal Detection mode. */
#define USM_STREAM_CMD_SIGNAL_DETECT_MODE_DISABLED    (0)

/** Activates Signal Detection mode with automatic PCM resume. */
#define USM_STREAM_CMD_SIGNAL_DETECT_MODE_AUTO_RESUME (1)

/** Activates Signal Detection mode with one-shot detection. */
#define USM_STREAM_CMD_SIGNAL_DETECT_MODE_ONE_SHOT    (2)

/** @} */ /* end_addtogroup usm_stream_cmd_set_signal_detect */

/* Payload structure for USM signal detect mode command */
typedef struct usm_stream_cmd_set_signal_detect_mode_t usm_stream_cmd_set_signal_detect_mode_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_stream_cmd_set_signal_detect_mode_t
@{ */
/* Payload of the USM_STREAM_CMD_SET_SIGNAL_DETECT_MODE command, which
    activates or deactivates the ultrasound Signal Detection mode.
*/
struct usm_stream_cmd_set_signal_detect_mode_t
{
    uint32_t                  signal_detect_mode;
    /**< Activates or deactivates ultrasound Signal Detection mode.

         @values
         - #USM_STREAM_CMD_SIGNAL_DETECT_MODE_DISABLED
         - #USM_STREAM_CMD_SIGNAL_DETECT_MODE_AUTO_RESUME
         - #USM_STREAM_CMD_SIGNAL_DETECT_MODE_ONE_SHOT

         @subhd{Disabled mode}
         Setting this parameter to zero disables Signal Detection mode and
         returns the ultrasound encoder to its normal operating state.

         @subhd{Auto Resume mode}
         Upon signal detection, the ultrasound encoder sends a
         #USM_STREAM_EVENT_SIGNAL_DETECT_DONE event to the client processor
         and resumes sending PCM data to the client processor. If an
         ultrasound signal is not detected, no event or data is sent to the
         client processor. The encoder repeats this sequence until an
         ultrasound signal is detected. The interval between signal detection
         retries is determined by the skip_interval field below.

         @subhd{One Shot mode}
         After analyzing the spectrum for the presence of an ultrasound signal
         once, the ultrasound encoder sends an event to the client processor
         with the detection result (positive or negative). The encoder does
         not resume sending PCM data to the client processor. The encoder
         performs this spectrum analysis only once, and it drops every AFE
         buffer following it.  */

    uint32_t                  skip_interval;
    /**< During Signal Detection mode, this is the length of the interval in
         which AFE samples are dropped and are not being used for ultrasound
         signal detection. This parameter is applicable only for Signal Detection
         mode 1 (automatic PCM resume).

         @values > 0 ms */

    uint32_t                  algorithm_cfg_size;
    /**< Size of the algorithm configuration structure (all fields following
         this field).

         @values > 0 */

    uint32_t                  accum_length;
    /**< During Signal Detection mode, number of samples that are used for
         ultrasound signal detection spectrum analysis.

         @values 1024 */

    uint32_t                  freq_range_start;
    /**< Lower boundary of the frequency range used in the signal detection
         energy calculation. This must be smaller than the interval end
         parameter.

         @values 0 to (sampling_rate_KHz/2) */

    uint32_t                  freq_range_end;
    /**< Upper boundary of the frequency range used in the signal detection
         energy calculation. This must be larger than the interval start
         parameter.

         @values 0 to (sampling_rate_KHz/2) */

    uint32_t                  energy_threshold;
    /**< Threshold used by the signal detection algorithm. If the energy in
         the frequency interval exceeds this threshold, a signal is considered
         to be present.

         @values @ge 0 */

    uint32_t                  narrow_band_limit;
    /**< Filters out narrow band interferences.

         @values @ge 0

         If a tone's energy exceeds this value, the energy is trimmed down to
         this value. */

    uint32_t                   channel_index;
    /**< Index of channels used to detect an ultrasound signal in Stand-by
         mode.

         @values 0 to num_channels - 1 */

    uint32_t                  alg_cfg_flags;
    /**< Bitmask used to configure a signal detection algorithm. 
     
         @values{for bit 0 (enable FFT scaling)}
          - 0 -- FFT scaling is disabled
          - 1 -- FFT scaling is enabled @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_stream_cmd_set_signal_detect_mode_t */

/** @ingroup usm_stream_evt_signal_detect_done
    Signals the result of a spectrum analysis for an ultrasound signal. This
    event is initiated by the #USM_STREAM_CMD_SET_SIGNAL_DETECT_MODE command.

  @apr_hdr_fields
    Opcode -- USM_STREAM_EVENT_SIGNAL_DETECT_DONE \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION

  @apr_msg_payload{usm_stream_event_signal_detect_done_t}
    @table{weak__usm__stream__event__signal__detect__done__t}

  @return
    None.

  @dependencies
    None.
*/
#define USM_STREAM_EVENT_SIGNAL_DETECT_DONE                          0x00012720

/* Structure for signal detection result event payload */
typedef struct usm_stream_event_signal_detect_done_t usm_stream_event_signal_detect_done_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_stream_event_signal_detect_done_t
@{ */
/* Payload of the USM_STREAM_EVENT_SIGNAL_DETECT_DONE event, which
    indicates the result of the spectrum analysis.
*/
struct usm_stream_event_signal_detect_done_t
{
   uint32_t                  signal_detect_result;
   /**< Indicates the result of the spectrum analysis.

        @values
        - 0 -- Signal was not detected
        - 1 -- Signal was detected @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_stream_event_signal_detect_done_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_USM_STREAM_COMMANDS_H_ */
