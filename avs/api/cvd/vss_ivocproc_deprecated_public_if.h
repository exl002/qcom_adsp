#ifndef __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_deprecated_public_if.h
  @brief This file contains the deprecated APR API definitions for Core VocProc
  (CVP) module.
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

  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/cvd/vss_ivocproc_deprecated_public_if.h#9 $
  $Author: svutukur $

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_ivocproc_deprecated_public_if
@{ */

/****************************************************************************
 * CORE VOCPROC APIS                                                        *
 ****************************************************************************/

/**
  Creates a new full control vocproc session.

  @deprecated
  This command is deprecated. Use #VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2.

  @par Payload
  #vss_ivocproc_cmd_create_full_control_session_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION ( 0x000100C3 )

/** @brief Type definition for #vss_ivocproc_cmd_create_full_control_session_t.
*/
typedef struct vss_ivocproc_cmd_create_full_control_session_t vss_ivocproc_cmd_create_full_control_session_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the 
    #VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION command.
*/
struct vss_ivocproc_cmd_create_full_control_session_t
{
  uint16_t direction;
    /**< Stream direction. Possible values:\n
         - 0 -- Tx only \n
         - 1 -- Rx only \n
         - 2 -- Tx and Rx */
  uint32_t tx_port_id;
    /**< Tx device port ID to which the vocproc connects. If a port ID
         is not being supplied, set this to #VSS_IVOCPROC_PORT_ID_NONE. */
  uint32_t tx_topology_id;
    /**< Tx leg topology ID. If a topology ID is not being supplied, set
	     this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */
  uint32_t rx_port_id;
    /**< Rx device port ID to which the vocproc connects. If a port ID
         is not being supplied, set this to #VSS_IVOCPROC_PORT_ID_NONE. */
  uint32_t rx_topology_id;
    /**< Rx leg topology ID. If a topology ID is not being supplied, set
         this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */
  uint32_t network_id;
    /**< Network ID (see @latexonly \hyperref[network_ID]{Network IDs}@endlatexonly).
         If a network ID is not being supplied, set this to
         #VSS_NETWORK_ID_DEFAULT. */

#if __STDC_VERSION__ >= 199901L
  char_t session_name[];
    /**< Session name string used to identify a session that can be shared with
         passive controllers (optional). The string size, including the NULL
         termination character, is limited to 31 characters. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;


/**
  Sets a new voice device configuration on the vocproc.

  @deprecated
  This command is deprecated. Use #VSS_IVOCPROC_CMD_SET_DEVICE_V2.

  @par Payload
  #vss_ivocproc_cmd_set_device_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  The configuration provided must match the direction of the vocproc. For
  example, when a vocproc is created as a Tx only direction, the Rx
  parameters are ignored.
*/
#define VSS_IVOCPROC_CMD_SET_DEVICE ( 0x000100C4 )

/** @brief Type definition for  #vss_ivocproc_cmd_set_device_t.
*/
typedef struct vss_ivocproc_cmd_set_device_t vss_ivocproc_cmd_set_device_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_CMD_SET_DEVICE command.
*/
struct vss_ivocproc_cmd_set_device_t
{
  uint32_t tx_port_id;
    /**< Tx device port ID to which the vocproc connects. Setting this to
         #VSS_IVOCPROC_PORT_ID_NONE means the vocproc does not connect to
         any port. */
  uint32_t tx_topology_id;
    /**< Tx leg topology ID. Setting this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE
         means the vocproc does not contain any pre/postprocessing blocks and
         the data is passed through unmodified. */
  uint32_t rx_port_id;
    /**< Rx device port ID to which the vocproc connects. Setting this to
         #VSS_IVOCPROC_PORT_ID_NONE means the vocproc does not connect to
         any port. */
  uint32_t rx_topology_id;
    /**< Rx leg topology ID. Setting this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE
         means the vocproc does not contain any pre/postprocessing blocks and
         is passed through unmodified. */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/**
  Provides per-network calibration data to the vocproc. Data provided here
  is cached and applied every time the vocproc transitions to the Run state. If
  this command is issued when the vocproc is in the Run state, the data is
  not applied until the next time the vocproc transitions to the Run state.

  @deprecated
  This command is deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2
  for sending calibration data to the vocproc.

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
#define VSS_IVOCPROC_CMD_CACHE_CALIBRATION_DATA ( 0x000110E3 )


/**
  Provides per-volume calibration data to the vocproc. Data provided here
  is cached and applied every time the volume changes.

  @deprecated
  This command is deprecated. Use
  #VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA for
  sending the volume calibration table to the vocproc.

  @par Payload
  @verbatim
  <-------- 32 bits -------->           <-------- 32 bits -------->

   -------------------------
  | num_vol_steps           | --> number of volume steps per network ID
   -------------------------
  | network_id              | --> network_id
   -------------------------
  | vol_step_len            | --> length of calibration data for this volume step
   -------------------------
  | vol_step_cal_data       | -------->  -------------------------
  |                         |           | mod_id                  |
  | ...                     |            -------------------------
   -------------------------            | param_id                |
  | vol_step_len            |            -------------------------
   -------------------------            | param_len  | reserved   |
  |                         |           |-------------------------
  | vol_step_cal_data       |           | param_data              |
  |                         |           |                         |
  |                         |           | ...                     |
  | ...                     |            -------------------------
   -------------------------            | mod_id                  |
  |                         |           .                         .
  .                         .           .                         .
  .                         .           .                         .
  .                         .
   -------------------------
  | network_id              |
   -------------------------
  | vol_step_len            |
   -------------------------
  | vol_step_cal_data       |
  |                         |
  | ...                     |
   -------------------------
  |                         |
  .                         .
  .                         .
  @endverbatim

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_CACHE_VOLUME_CALIBRATION_TABLE ( 0x000110E4 )


/**
  Deregisters the memory that contains the volume calibration data table from
  the vocproc and any cal-indexing memory provided.

  @deprecated
  This command is deprecated. Use #VSS_IVOCPROC_CMD_DEREGISTER_VOLUME_CALIBRATION_DATA
  instead.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_VOLUME_CALIBRATION_TABLE ( 0x00011278 )


/****************************************************************************
 *  VOCPROC VOLUME/MUTE APIS                                                *
 ****************************************************************************/

/**
  Changes the volume and recalibrates for the new volume.

  @deprecated
  This command is deprecated. Use #VSS_IVOLUME_CMD_SET_STEP.

  @par Payload
  #vss_ivocproc_cmd_set_rx_volume_index_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX ( 0x000110EE )

/** @brief Type definition for #vss_ivocproc_cmd_set_rx_volume_index_t.
*/
typedef struct vss_ivocproc_cmd_set_rx_volume_index_t vss_ivocproc_cmd_set_rx_volume_index_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_CMD_SET_RX_VOLUME_INDEX
    command.
*/
struct vss_ivocproc_cmd_set_rx_volume_index_t
{
  uint16_t vol_index;
    /**< Volume index used by the vocproc to index into the volume table
         provided in #VSS_IVOCPROC_CMD_CACHE_VOLUME_CALIBRATION_TABLE and
         to set the volume on the Voice Digital Signal Processor (VDSP). */
}
#include "apr_pack_end.h"
;


/**
  Changes the mute setting.

  @deprecated
  This command is deprecated. Use #VSS_IVOLUME_CMD_MUTE_V2.

  @par Payload
  #vss_ivocproc_cmd_set_mute_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_SET_MUTE ( 0x000110EF )

/** @brief Type definition for #vss_ivocproc_cmd_set_mute_t.
*/
typedef struct vss_ivocproc_cmd_set_mute_t vss_ivocproc_cmd_set_mute_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_CMD_SET_MUTE command.
*/
struct vss_ivocproc_cmd_set_mute_t
{
  uint16_t direction;
    /**< Stream direction. Possible values:\n
         - 0 -- Tx only \n
         - 1 -- Rx only \n
         - 2 -- Tx and Rx */
  uint16_t mute_flag;
    /**< Turn mute on or off. Possible values:\n
         - 0 -- Silence is disabled (mute is off).\n
         - 1 -- Silence is enabled (mute is on). */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_ivocproc_deprecated_public_if */

#endif /* __VSS_IVOCPROC_DEPRECATED_PUBLIC_IF_H__ */

