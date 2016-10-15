#ifndef __VSS_IVOCPROC_PUBLIC_IF_H__
#define __VSS_IVOCPROC_PUBLIC_IF_H__

/**
  @file vss_ivocproc_public_if.h
  @brief This file contains the APR API definitions for Core VocProc (CVP)
  module.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_ivocproc_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011-2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/cvd/vss_ivocproc_public_if.h#10 $
  $Author: coresvc $

===========================================================================*/

#include "apr_comdef.h"
#include "vss_ivocproc_deprecated_public_if.h"

/** @addtogroup vss_ivocproc_public_if
@{ */

/****************************************************************************
 * VOCPROC DEFINES                                                          *
 ****************************************************************************/

/*---------------------------------------------------------------------------*
 * Vocproc Directions                                                        *
 *---------------------------------------------------------------------------*/

/** @name Vocproc Directions
@{ */

/** Select Rx only direction. */
#define VSS_IVOCPROC_DIRECTION_RX ( 0 )

/** Select Tx only direction. */
#define VSS_IVOCPROC_DIRECTION_TX ( 1 )

/** Select both Rx and Tx directions. */
#define VSS_IVOCPROC_DIRECTION_RX_TX ( 2 )

/** @} */ /* end_name_group Vocproc Directions */
 
/*--------------------------------------------------------------------------*
 * Vocproc Modes                                                            *
 *--------------------------------------------------------------------------*/

/** @name Vocproc Modes
@{ */

/** Select internal mixing mode. */
#define VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING ( 0x00010F7C )

/** Select external mixing mode. */
#define VSS_IVOCPROC_VOCPROC_MODE_EC_EXT_MIXING ( 0x00010F7D )

/** @} */ /* end_name_group Vocproc Modes */
 
/*--------------------------------------------------------------------------*
 * AFE PORTS                                                                *
 *--------------------------------------------------------------------------*/

/** @name AFE Port IDs
@{ */

/** Default AFE port ID. Applicable to Tx and Rx. */
#define VSS_IVOCPROC_PORT_ID_NONE ( 0xFFFF )

/** @} */ /* end_name_group AFE Port IDs */

/*--------------------------------------------------------------------------*
 * VOCPROC TOPOLOGIES                                                       *
 *--------------------------------------------------------------------------*/

/** @name Vocproc Topology IDs
@{ */

/** Pass-through topology ID. Applicable to both Rx and Tx. */
#define VSS_IVOCPROC_TOPOLOGY_ID_NONE ( 0x00010F70 )

/** Tx topology ID for the single-mic echo cancellation noise suppression. */
#define VSS_IVOCPROC_TOPOLOGY_ID_TX_SM_ECNS ( 0x00010F71 )

/** Tx topology ID for the dual-mic Fluence&tm;&nbsp;algorithm. */
#define VSS_IVOCPROC_TOPOLOGY_ID_TX_DM_FLUENCE ( 0x00010F72 )

/** Default Rx topology ID for Rx processing with all available algorithms. */
#define VSS_IVOCPROC_TOPOLOGY_ID_RX_DEFAULT ( 0x00010F77 )

/** @} */ /* end_name_group Vocproc Topology IDs */

/****************************************************************************
 * CORE VOCPROC APIS                                                        *
 ****************************************************************************/

/**
  Creates a new full control vocproc session.

  @par Payload
  #vss_ivocproc_cmd_create_full_control_session_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 ( 0x000112BF )

/** @brief Type definition for #vss_ivocproc_cmd_create_full_control_session_v2_t.
*/
typedef struct vss_ivocproc_cmd_create_full_control_session_v2_t vss_ivocproc_cmd_create_full_control_session_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the 
    #VSS_IVOCPROC_CMD_CREATE_FULL_CONTROL_SESSION_V2 command.
*/
struct vss_ivocproc_cmd_create_full_control_session_v2_t
{
  uint16_t direction;
    /**< Vocproc direction. The supported values:\n
         #VSS_IVOCPROC_DIRECTION_RX \n
         #VSS_IVOCPROC_DIRECTION_TX \n
         #VSS_IVOCPROC_DIRECTION_RX_TX */ 
  uint16_t tx_port_id;
    /**< Tx device port ID to which the vocproc connects. If a port ID is not
         being supplied, set this to #VSS_IVOCPROC_PORT_ID_NONE. */
  uint32_t tx_topology_id;
    /**< Tx path topology ID. If a topology ID is not being supplied, set
         this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */
  uint16_t rx_port_id;
    /**< Rx device port ID to which the vocproc connects. If a port ID is not
         being supplied, set this to #VSS_IVOCPROC_PORT_ID_NONE. */
  uint32_t rx_topology_id;
    /**< Rx path topology ID. If a topology ID is not being supplied, set
         this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */
  uint32_t profile_id;
    /**< Voice calibration profile ID. */
  uint32_t vocproc_mode;
    /**< Vocproc mode. The supported values:\n
         #VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING \n
         #VSS_IVOCPROC_VOCPROC_MODE_EC_EXT_MIXING */
  uint16_t ec_ref_port_id;
    /**< Port ID to which the vocproc connects for receiving echo
         cancellation reference signal. If a port ID is not being supplied,
         set this to #VSS_IVOCPROC_PORT_ID_NONE. This parameter value is
         ignored when the vocproc_mode parameter is set to
         #VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING. */
#if __STDC_VERSION__ >= 199901L
  char_t session_name[];
    /**< Session name string used to identify a session that can be shared
         with passive controllers (optional). The string size, including the
         NULL termination character, is limited to 31 characters. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;


/**
  Sets a new voice device configuration on the vocproc.

  @par Payload
  #vss_ivocproc_cmd_set_device_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  The configuration provided must match the direction of the vocproc. For
  example, when a vocproc is created as a Tx only direction, the Rx
  parameters are ignored.
*/
#define VSS_IVOCPROC_CMD_SET_DEVICE_V2 ( 0x000112C6 )

/** @brief Type definition for  #vss_ivocproc_cmd_set_device_v2_t.
*/
typedef struct vss_ivocproc_cmd_set_device_v2_t vss_ivocproc_cmd_set_device_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_CMD_SET_DEVICE_V2 command.
*/
struct vss_ivocproc_cmd_set_device_v2_t
{
  uint16_t tx_port_id;
    /**< Tx device port ID to which the vocproc connects. If a port ID is not
         being supplied, set this to #VSS_IVOCPROC_PORT_ID_NONE. */
  uint32_t tx_topology_id;
    /**< Tx path topology ID. If a topology ID is not being supplied, set
         this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */
  uint16_t rx_port_id;
    /**< Rx device port ID to which the vocproc connects. If a port ID is not
         being supplied, set this to #VSS_IVOCPROC_PORT_ID_NONE. */
  uint32_t rx_topology_id;
    /**< Rx path topology ID. If a topology ID is not being supplied, set
         this to #VSS_IVOCPROC_TOPOLOGY_ID_NONE. */
  uint32_t vocproc_mode;
    /**< Vocproc mode. The supported values:\n
         #VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING \n
         #VSS_IVOCPROC_VOCPROC_MODE_EC_EXT_MIXING */
  uint16_t ec_ref_port_id;
    /**< Port ID to which the vocproc connects for receiving echo
         cancellation reference signal. If a port ID is not being supplied,
         set this to #VSS_IVOCPROC_PORT_ID_NONE. This parameter value is
         ignored when the vocproc_mode parameter is set to
         #VSS_IVOCPROC_VOCPROC_MODE_EC_INT_MIXING. */
}
#include "apr_pack_end.h"
;

/*
  Sets the vocproc sampling rates.

  If no sampling rates are provided to the vocproc (i.e. this command is
  not called) then the operating and calibration sampling rates that the
  vocproc uses are the ones it recieves from the MVM (which were provided
  to the MVM via the #VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE (or
  #VSS_IMVM_CMD_SET_CAL_VAR_VOC_SAMPLING_RATE).

  If sampling rates are provided to the vocproc (i.e. this command is
  called), then the operating and calibration sample rates that the vocproc
  uses are the ones provided in this command.
  
  @par Payload
  #vss_ivocproc_cmd_set_sampling_rate_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_SET_SAMPLING_RATE ( 0x00011370 )

/** @brief Type definition for #vss_ivocproc_cmd_set_sampling_rate_t.
*/
typedef struct vss_ivocproc_cmd_set_sampling_rate_t vss_ivocproc_cmd_set_sampling_rate_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_CMD_SET_SAMPLING_RATE command.
*/
struct vss_ivocproc_cmd_set_sampling_rate_t
{
  uint32_t rx;
    /**< Rx sample rate. Supported values: \n
         - 8000 Hz \n
         - 16000 Hz \n
         - 48000 Hz */
  uint32_t tx;
    /**< Tx sampling rate. Supported values: \n
         - 8000 Hz \n
         - 16000 Hz \n
         - 48000 Hz */
}
#include "apr_pack_end.h"
;


/**
  Enables the vocproc.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments  
  Running vocproc sessions while unattached to any MVM session is a deprecated
  behavior, and will be disallowed in the future.

  When attached to the MVM, the vocproc enters the Enable state after both the
  client and the MVM trigger the vocproc to be enabled. Otherwise, the vocproc
  is enabled by the client trigger.
*/
#define VSS_IVOCPROC_CMD_ENABLE ( 0x000100C6 )


/**
  Disables the vocproc.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_DISABLE ( 0x000110E1 )


/****************************************************************************
 * DEVICE CONFIGURATION DATA APIS                                           *
 ****************************************************************************/

/**
  Registers the memory that contains device specific configuration data with
  the vocproc.

  The client must register device configuration data with the vocproc that
  corresponds with the device being set on the vocproc.
    
  @par Payload
  #vss_ivocproc_cmd_register_device_config_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  The memory registered here must first be mapped with CVD by following
  @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
  @endlatexonly. 
  The client must provide memory for the vocproc session before registering 
  device configuration data by following @latexonly \hyperref [session_memory]
  {Session Memory Usage Guideline} @endlatexonly.

  @comments
  The client is not allowed to modify this memory while it
  is registered and the vocproc does not modify this memory while registered.
  The device configuration data at the registered memory is valid until
  unregistered. Registering a memory region for device configuration data
  while another region is still registered causes this command to fail.
*/
#define VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG ( 0x00011371 )

/** @brief Type definition for
    #vss_ivocproc_cmd_register_device_config_t.
*/
typedef struct vss_ivocproc_cmd_register_device_config_t vss_ivocproc_cmd_register_device_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG command.

    Expected buffer format:

    @verbatim

     <-------- 32 bits -------->

      -------------------------
     | mod_id                  |
      -------------------------
     | param_id                |
      -------------------------
     | param_size | reserved   |
      -------------------------
     | param_data              |
     |                         |
     | ...                     |
      ------------------------- 
     | mod_id                  |
     .                         .
     .                         .
     .                         .

    @endverbatim

*/
struct vss_ivocproc_cmd_register_device_config_t
{

  uint32_t mem_handle;
    /**< Handle to the shared memory that holds the per-network calibration
         data. See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
         Guideline} @endlatexonly. */
  uint64_t mem_address;
    /**< Location of calibration data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t mem_size;
    /**< Size of the calibration data in bytes. The amount of memory allocated
         at mem_address must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
}
#include "apr_pack_end.h"
;


/**
  Deregisters the memory that holds device configuration data from the
  vocproc.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_DEVICE_CONFIG ( 0x00011372 )


/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/** @brief Type definition for #vss_ivocproc_cal_na_value_t.
*/
typedef union vss_ivocproc_cal_na_value_t vss_ivocproc_cal_na_value_t;

#include "apr_pack_begin.h"

/** @brief Union for specifying a calibration column N/A value.
*/
union vss_ivocproc_cal_na_value_t
{
  uint8_t uint8_val;
    /**< UINT8 N/A value. */
  uint16_t uint16_val;
    /**< UINT16 N/A value. */
  uint32_t uint32_val;
    /**< UINT32 N/A value. */
  uint64_t uint64_val;
    /**< UINT64 N/A value. */
}
#include "apr_pack_end.h"
;


/** @brief Type definition for #vss_ivocproc_cal_column_t.
*/
typedef struct vss_ivocproc_cal_column_t vss_ivocproc_cal_column_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying a calibration column.
*/
struct vss_ivocproc_cal_column_t
{
  uint32_t id;
    /**< Column ID. Supported values: \n
         see @latexonly \hyperref[calibration_column]{Calibration Columns}
         @endlatexonly. */
  uint32_t type;
    /**< Column type. Supported values: \n
         see @latexonly \hyperref[calibration_column_type]{Calibration Column Types}
         @endlatexonly. */
  vss_ivocproc_cal_na_value_t na_value;
    /**< The N/A column value associated with this column ID and type.
         
         The N/A value is a reseved value indicating that the calibration
         data entry corresponding to it in the table is independent of this
         column (and therefore the data is applied irrespective of what
         the current system configuration is for this column).

         The size of na_value is dependent on the column type. */
}
#include "apr_pack_end.h"
;


/**
  Registers the memory that contains common calibration data table with
  the vocproc.

  As part of this registration, in addition to the address, size and handle
  of the memory containing the calibration table, the client also porvides
  the columns and associated column types which the vocproc uses to form a
  key that is used to index into the calibration table and look-up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.

  Calibration data is applied when the vocproc transitions to the run state.

  @par Payload
  #vss_ivocproc_cmd_register_calibration_data_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Any memory registered here must first be mapped with CVD by following
  @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
  @endlatexonly. 

  @comments
  The client is not allowed to modify the calibration data memory while it
  is registered and the vocproc does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.
*/
#define VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2 ( 0x00011373 )

/** @brief Type definition for
    #vss_ivocproc_cmd_register_calibration_data_v2_t.
*/
typedef struct vss_ivocproc_cmd_register_calibration_data_v2_t vss_ivocproc_cmd_register_calibration_data_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_IVOCPROC_CMD_REGISTER_CALIBRATION_DATA_V2 command.

    Expected buffer format:

    @verbatim
                                           <-------- 32 bits -------->
      -------------------------
     | column_1                | --> eg. network_id
      -------------------------
     | column_2                | --> eg. rx_sampling_rate
      -------------------------
     |  .                      |
     |	.                      |
      -------------------------
     | column_n                |
      -------------------------
     | data_size               | --> byte length of data block that follows
      -------------------------
     | data                    | -------->  -------------------------
     |                         |           | mod_id                  |
     |                         |            -------------------------
     | ...                     |           | param_id                |
      -------------------------             -------------------------
     | column_1                |           | param_size | reserved   |
      -------------------------	            -------------------------
     | column_2                |           | param_data              |
      -------------------------	           |                         |
     |  .                      |           | ...                     |
     |  .                      |            -------------------------
      -------------------------            | mod_id                  |
     | column_n                |           .                         .
      -------------------------            .                         .
     | data_size               |           .                         .
      -------------------------
     | data                    |
     |                         |
     |                         |
     | ...                     |
      -------------------------
     |                         |
     .                         .
     .                         .

    @endverbatim                                               

*/
struct vss_ivocproc_cmd_register_calibration_data_v2_t
{
  uint32_t cal_mem_handle;
    /**< Handle to the shared memory that holds the per-network calibration
         data. See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
         Guideline} @endlatexonly. */
  uint64_t cal_mem_address;
    /**< Location of calibration data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t cal_mem_size;
    /**< Size of the calibration data in bytes. The amount of memory allocated
         at mem_address must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
  uint32_t num_columns;
    /**< Number of columns used to form the key for looking up a calibration data
         entry. */
#if __STDC_VERSION__ >= 199901L
  vss_ivocproc_cal_column_t columns[];
#endif /* __STDC_VERSION__ >= 199901L */
    /**< Array of columns of size num_columns. The order in which the colums are
         provided here must match the order in which they exist in the calibration
         table provided. */
}
#include "apr_pack_end.h"
;


/**
  Deregisters the memory that contains the common calibration data table from
  the vocproc and any cal-indexing memory provided.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_CALIBRATION_DATA ( 0x00011276 )


/**
  Registers the memory that contains the volume calibration data table with
  the vocproc.

  As part of this registration, in addition to the address, size and handle
  of the memory containing the calibration table, the client also porvides
  the columns and associated column types which the vocproc uses to form a
  key that is used to index into the calibration table and look-up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.

  Calibration data is applied when the vocproc transisions to the run state
  as well as when the volume step changes during the run state.
  
  @par Payload
  #vss_ivocproc_cmd_register_volume_calibration_data_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Any memory registered here must first be mapped with CVD by following
  @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
  @endlatexonly. 

  @comments
  The client is not allowed to modify the calibration data memory while it
  is registered and the vocproc does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.
*/
#define VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA ( 0x00011374 )

/** @brief Type definition for
    #vss_ivocproc_cmd_register_volume_calibration_data_t.
*/
typedef struct vss_ivocproc_cmd_register_volume_calibration_data_t vss_ivocproc_cmd_register_volume_calibration_data_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_IVOCPROC_CMD_REGISTER_VOLUME_CALIBRATION_DATA command.

    Expected buffer format.

    @verbatim
                                           <-------- 32 bits -------->
      -------------------------
     | column_1                | --> eg. network_id
      -------------------------
     | column_2                | --> eg. rx_sampling_rate
      -------------------------
     |  .                      |
     |  .                      |
      -------------------------
     | column_n                | --> eg. volume_index
      -------------------------
     | data_size               | --> byte length of data block that follows
      -------------------------
     | data                    | -------->  -------------------------
     |                         |           | mod_id                  |
     |                         |            -------------------------
     | ...                     |           | param_id                |
      -------------------------             -------------------------
     | column_1                |           | param_size | reserved   |
      -------------------------	            -------------------------
     | column_2                |           | param_data              |
      -------------------------	           |                         |
     |  .                      |           | ...                     |
     |  .                      |            -------------------------
      -------------------------            | mod_id                  |
     | column_n                |           .                         .
      -------------------------            .                         .
     | data_size               |           .                         .
      -------------------------
     | data                    |
     |                         |
     |                         |
     | ...                     |
      -------------------------
     |                         |
     .                         .
     .                         .

   @endverbatim
*/
struct vss_ivocproc_cmd_register_volume_calibration_data_t
{
  uint32_t cal_mem_handle;
    /**< Handle to the shared memory that holds the volume calibration
         data. See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
         Guideline} @endlatexonly. */
  uint64_t cal_mem_address;
    /**< Location of volume calibration data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t cal_mem_size;
    /**< Size of the volume calibration data in bytes. The amount of memory allocated
         at mem_address must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
  uint32_t num_columns;
    /**< Number of columns used to form the key for looking up a calibration data
         entry. */
#if __STDC_VERSION__ >= 199901L
  vss_ivocproc_cal_column_t columns[];
#endif /* __STDC_VERSION__ >= 199901L */
    /**< Array of columns of size num_columns. The order in which the colums are
         provided here must match the order in which they exist in the calibration
         table provided. */
}
#include "apr_pack_end.h"
;


/**
  Deregisters the memory that contains the volume calibration data table from
  the vocproc and any cal-indexing memory provided.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IVOCPROC_CMD_DEREGISTER_VOLUME_CALIBRATION_DATA ( 0x00011375 )


/****************************************************************************
 * VOCPROC DTMF DETECTION APIS                                              *
 ****************************************************************************/

/**
  Enables/disables Tx DTMF detection. The DTMF detection status is sent
  to the client that is sending this command via
  #VSS_IVOCPROC_EVT_TX_DTMF_DETECTED events.

  @par Payload
  #vss_ivocproc_cmd_set_tx_dtmf_detection_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Only one client can request Tx DTMF detection on a given vocproc at a
  given time. When another client requests Tx DTMF detection while the
  previous client's Tx DTMF detection is still active, the request fails.
*/
#define VSS_IVOCPROC_CMD_SET_TX_DTMF_DETECTION ( 0x00011033 )

/** DTMF detection enable flag. */
#define VSS_IVOCPROC_TX_DTMF_DETECTION_ENABLE ( 1 )

/** DTMF detection disable flag. */
#define VSS_IVOCPROC_TX_DTMF_DETECTION_DISABLE ( 0 )

/** @brief Type definition for #vss_ivocproc_cmd_set_tx_dtmf_detection_t.
*/
typedef struct vss_ivocproc_cmd_set_tx_dtmf_detection_t vss_ivocproc_cmd_set_tx_dtmf_detection_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_CMD_SET_TX_DTMF_DETECTION
    command.
*/
struct vss_ivocproc_cmd_set_tx_dtmf_detection_t
{
   uint32_t enable;
      /**< Enables/disables Tx DTMF detection. Supported values:
           #VSS_IVOCPROC_TX_DTMF_DETECTION_ENABLE \n
           #VSS_IVOCPROC_TX_DTMF_DETECTION_DISABLE */
}
#include "apr_pack_end.h"
;


/**
  Event sent by the vocproc to the client that enables Tx DTMF detection
  whenever DTMF is detected in the Tx path.

  @par Payload
  #vss_ivocproc_evt_tx_dtmf_detected_t

  @return
  None.

  @comments
  The DTMF detection feature can only be used to detect DTMF frequencies
  as listed in ITU-T Recommendation Q.23: <i>Technical Features of Push-button
  Telephone Sets, Section 6 (see </i>@latexonly \hyperref[S5]{[S5]}@endlatexonly).
  Therefore, the possible values for the low_freq and high_freq parameters are
  as listed in the vss_ivocproc_evt_tx_dtmf_detected_t structure.
*/
#define VSS_IVOCPROC_EVT_TX_DTMF_DETECTED ( 0x00011034 )

/** @brief Type definition for #vss_ivocproc_evt_tx_dtmf_detected_t.
*/
typedef struct vss_ivocproc_evt_tx_dtmf_detected_t vss_ivocproc_evt_tx_dtmf_detected_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IVOCPROC_EVT_TX_DTMF_DETECTED event.
*/
struct vss_ivocproc_evt_tx_dtmf_detected_t
{
   uint16_t low_freq;
      /**< Detected low frequency. Possible values:\n
           - 697 Hz \n
           - 770 Hz \n
           - 852 Hz \n
           - 941 Hz */
   uint16_t high_freq;
      /**< Detected high frequency. Possible values:\n
           - 1209 Hz \n
           - 1336 Hz \n
           - 1477 Hz \n
           - 1633 Hz */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_ivocproc_public_if */

#endif /* __VSS_IVOCPROC_PUBLIC_IF_H__ */

