#ifndef __VSS_ISTREAM_DEPRECATED_PUBLIC_IF_H__
#define __VSS_ISTREAM_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_istream_deprecated_public_if.h
  @brief This file contains the deprecated APR APIs for Core Voice Stream (CVS)
  module.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011, 2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/cvd/vss_istream_deprecated_public_if.h#9 $
  $Author: svutukur $

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_istream_deprecated_public_if
@{ */

/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/**
  Provides per-network calibration data to the stream. Data provided here
  is cached and applied every time the stream transitions to the Run state. If
  this command is issued when the stream is in the Run state, the data is not
  applied until the next time the stream transitions to the Run state.

  @deprecated
  This command is deprecated. Use #VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V2
  for sending calibration data to the stream.

  @par Payload
  @verbatim
  <-------- 32 bits -------->           <-------- 32 bits -------->

   -------------------------
  | network_id              | --> network_id
   -------------------------
  | data_len                | --> byte length of data block that follows
   -------------------------
  | data                    | -------->  -------------------------
  |                         |           | mod_id                  |
  |                         |            -------------------------
  | ...                     |           | param_id                |
   -------------------------             -------------------------
  | network_id              |           | param_len  | reserved   |
   -------------------------             -------------------------
  | data_len                |           | param_data              |
   -------------------------            |                         |
  | data                    |           | ...                     |
  |                         |            -------------------------
  |                         |           | mod_id                  |
  | ...                     |           .                         .
   -------------------------            .                         .
  |                         |           .                         .
  .                         .
  .                         .
  @endverbatim

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_CACHE_CALIBRATION_DATA ( 0x000110FB )


/****************************************************************************
 * IN-CALL CONVERSATION RECORDING APIS                                      *
 ****************************************************************************/

/**
  Starts the in-call conversation recording.

  This command forwards the in-call conversation stream to Rx Analog Front
  End (AFE) port 0x8003 and to Tx AFE port 0x8004.

  @deprecated
  This command is deprecated. Use #VSS_IRECORD_CMD_START.

  @par Payload
  #vss_istream_cmd_start_record_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Recording is to be stopped before sending this command.

  @comments
  The audio path is used to record the voice streams. If recording silence
  before a call or during handovers is desired, AFE ports 0x8003 and 0x8004
  must be configured to live mode.
*/
#define VSS_ISTREAM_CMD_START_RECORD ( 0x00011236 )

/** Indicates no tapping for the specified path. */
#define VSS_TAP_POINT_NONE ( 0x00010F78 )

/** Indicates that the specified path is to be tapped at the end of the stream. */
#define VSS_TAP_POINT_STREAM_END ( 0x00010F79 )

/** @brief Type definition for #vss_istream_cmd_start_record_t.
*/
typedef struct vss_istream_cmd_start_record_t vss_istream_cmd_start_record_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_START_RECORD command.
*/
struct vss_istream_cmd_start_record_t
{
   uint32_t rx_tap_point;
      /**< Tap point to use on the Rx path. Supported values:\n
           #VSS_TAP_POINT_NONE -- Do not record Rx path.\n
           #VSS_TAP_POINT_STREAM_END -- Rx tap point is at the end of the stream. */
   uint32_t tx_tap_point;
      /**< Tap point to use on the Tx path. Supported values:\n
           #VSS_TAP_POINT_NONE -- Do not record Tx path.\n
           #VSS_TAP_POINT_STREAM_END -- Tx tap point is at the end of the stream. */
   }
#include "apr_pack_end.h"
;


/**
  Stops the in-call conversation recording.

  This command stops forwarding the in-call conversation stream
  to Rx AFE port 0x8003 and to Tx AFE port 0x8004.

  @deprecated
  This command is deprecated. Use #VSS_IRECORD_CMD_STOP.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_STOP_RECORD ( 0x00011237 )


/****************************************************************************
 * IN-CALL MUSIC DELIVERY APIS                                              *
 ****************************************************************************/

/**
  Starts the in-call music delivery on the Tx voice path.

  This command establishes a connection between the TX voice stream and TX AFE
  port 0x8005, allowing the voice stream to receive music from the audio path.

  @deprecated
  This command is deprecated. Use #VSS_IPLAYBACK_CMD_START.
  
  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  The audio path is used to inject music to the Tx voice path. Configuring AFE
  port 0x8005 to non-live mode allows the voice modules to throttle transfer
  jitters.
*/
#define VSS_ISTREAM_CMD_START_PLAYBACK ( 0x00011238 )


/**
  Stops the in-call music delivery on the Tx voice path.

  This command causes the Tx voice stream to stop receiving music from Tx AFE
  port 0x8005.

  @deprecated
  This command is deprecated. Use #VSS_IPLAYBACK_CMD_STOP.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ISTREAM_CMD_STOP_PLAYBACK ( 0x00011239 )

/** @} */ /* end_addtogroup vss_istream_deprecated_public_if */

#endif /* __VSS_ISTREAM_DEPRECATED_PUBLIC_IF_H__ */

