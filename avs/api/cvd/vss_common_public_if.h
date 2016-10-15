#ifndef __VSS_COMMON_PUBLIC_IF_H__
#define __VSS_COMMON_PUBLIC_IF_H__

/**
  @file vss_common_public_if.h
  @brief This file contains the APR API definitions that are common among
  modules.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_common_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011-2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/cvd/vss_common_public_if.h#10 $
  $Author: coresvc $

===========================================================================*/

#include "apr_comdef.h"
#include "vss_common_deprecated_public_if.h"

/** @addtogroup vss_common_public_if
@{ */

/*--------------------------------------------------------------------------*
 * MEDIA TYPE DEFINITIONS                                                   *
 *--------------------------------------------------------------------------*/

/** @name Media Type IDs
  @latexonly \label{media_ID} @endlatexonly
  @{ */

/** Default media type ID. */
#define VSS_MEDIA_ID_NONE ( 0x00010FC0 )

/** CDMA variable 13K vocoder format. */
#define VSS_MEDIA_ID_13K ( 0x00010FC1 )

/** CDMA Enhanced Variable Rate Vocoder (EVRC) format. */
#define VSS_MEDIA_ID_EVRC ( 0x00010FC2 )

/** CDMA 4th generation NB vocoder format. */
#define VSS_MEDIA_ID_4GV_NB ( 0x00010FC3 )

/** CDMA 4th generation WB vocoder format. */
#define VSS_MEDIA_ID_4GV_WB ( 0x00010FC4 )

/** CDMA 4th generation Narrow-Wide (NW) vocoder format. */
#define VSS_MEDIA_ID_4GV_NW ( 0x00010FC5 )

/** CDMA 4th generation Narrow-Wide-2K (NW2K) vocoder format. */
#define VSS_MEDIA_ID_4GV_NW2K ( 0x00010FD7 )

/** Universal Mobile Telecommunications System (UMTS) AMR-NB vocoder format. */
#define VSS_MEDIA_ID_AMR_NB ( 0x00010FC6 )

/** UMTS AMR-WB vocoder format. */
#define VSS_MEDIA_ID_AMR_WB ( 0x00010FC7 )

/** eAMR (Enhanced AMR) vocoder format. */
#define VSS_MEDIA_ID_EAMR ( 0x00010FD4 )

/** GSM enhanced full rate vocoder format. */
#define VSS_MEDIA_ID_EFR ( 0x00010FC8 )

/** GSM full rate vocoder format. */
#define VSS_MEDIA_ID_FR ( 0x00010FC9 )

/** GSM half rate vocoder format. */
#define VSS_MEDIA_ID_HR ( 0x00010FCA )

/** Linear Pulse Code Modulation (PCM) vocoder format; mono, 16 bits,
  little endian, 8 KHz. */
#define VSS_MEDIA_ID_PCM_8_KHZ ( 0x00010FCB )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 16 KHz. */
#define VSS_MEDIA_ID_PCM_16_KHZ ( 0x00010FCC )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 44.1 KHz. */
#define VSS_MEDIA_ID_PCM_44_1_KHZ ( 0x00010FD5 )

/** Linear PCM vocoder format; mono, 16 bits, little endian, 48 KHz. */
#define VSS_MEDIA_ID_PCM_48_KHZ ( 0x00010FD6 )

/** G.711 a-law vocoder format; contains two 10 ms vocoder frames. */
#define VSS_MEDIA_ID_G711_ALAW ( 0x00010FCD )

/** G.711 mu-law vocoder format; contains two 10 ms vocoder frames. */
#define VSS_MEDIA_ID_G711_MULAW ( 0x00010FCE )

/** G.711 linear PCM vocoder format. */
#define VSS_MEDIA_ID_G711_LINEAR ( 0x00010FCF )

/** G.729AB vocoder format; contains two 10 ms vocoder frames. Refer to
  @latexonly [\hyperref[S4]{S4}]@endlatexonly, Figure 4. */
#define VSS_MEDIA_ID_G729 ( 0x00010FD0 )

/** G.722 vocoder format; contains one 20 ms vocoder frame. */
#define VSS_MEDIA_ID_G722 ( 0x00010FD1 )

/** @} */ /* end_name_group Media Type IDs */


/****************************************************************************
 * CALIBRATION FRAMEWORK DEFINITIONS                                        *
 ****************************************************************************/

/*--------------------------------------------------------------------------*
 * CALIBRATION COLUMN IDS                                                   *
 *--------------------------------------------------------------------------*/

/** @name Calibration Columns
  @latexonly \label{calibration_column} @endlatexonly
  @{ */

/** Column network ID. 
 * (see @latexonly \hyperref[cal_network_ID]{Calibration Network IDs} @endlatexonly).
 */
#define VSS_ICOMMON_CAL_COLUMN_NETWORK ( 0x00011350 )

/** Column RX sampling rate. */
#define VSS_ICOMMON_CAL_COLUMN_RX_SAMPLING_RATE ( 0x00011351 )

/** Column TX sampling rate. */
#define VSS_ICOMMON_CAL_COLUMN_TX_SAMPLING_RATE ( 0x00011352 )

/** Column volume index. */
#define VSS_ICOMMON_CAL_COLUMN_VOLUME_INDEX ( 0x00011358 )

/** Column vocoder class. */
#define VSS_ICOMMON_CAL_COLUMN_VOC_CLASS ( 0x00011359 )

/** @} */ /* end_name_group Calibration Columns */

/*--------------------------------------------------------------------------*
 * CALIBRATION COLUMN TYPES                                                 *
 *--------------------------------------------------------------------------*/

/** @name Calibration Column Types
  @latexonly \label{calibration_column_type} @endlatexonly
  @{ */

/** 8-bit calibration column data type */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT8 ( 0x0001135A )

/** 16-bit calibration column data type */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT16 ( 0x0001135B )

/** 32-bit calibration column data type */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT32 ( 0x0001135C )

/** 64-bit calibration column data type */
#define VSS_ICOMMON_CAL_COLUMN_TYPE_UINT64 ( 0x0001135D )

/** @} */ /* end_name_group Calibration Column Types */

/*--------------------------------------------------------------------------*
 * CALIBRATION NETWORK DEFINITIONS                                          *
 *--------------------------------------------------------------------------*/

/** @name Calibration Network IDs
  @latexonly \label{cal_network_ID} @endlatexonly
  @{ */

/** NONE network ID. Network ID is not applicable. */
#define VSS_ICOMMON_CAL_NETWORK_ID_NONE ( 0x0001135E )

/** CDMA network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_CDMA ( 0x0001135F )

/** GSM network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_GSM ( 0x00011360 )

/** WCDMA network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_WCDMA ( 0x00011361 )

/** VOIP network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_VOIP ( 0x00011362 )

/** LTE network ID. */
#define VSS_ICOMMON_CAL_NETWORK_ID_LTE ( 0x00011363 )

/** @} */ /* end_name_group Network IDs */

/*--------------------------------------------------------------------------*
 * CALIBRATION VOCODER CLASS DEFINITIONS                                    *
 *--------------------------------------------------------------------------*/

/** @name Calibration Vocoder Classes
  @latexonly \label{cal_voc_class} @endlatexonly
  @{ */

/** NONE vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_NONE ( 0x00011364 )

/** CDMA vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_CDMA ( 0x00011365 )

/** GSM vocoder class. */
#define VSS_ICOMMON_CAL_VOC_CLASS_GSM ( 0x00011366 )

/** WCDMA vocoder class */
#define VSS_ICOMMON_CAL_VOC_CLASS_WCDMA ( 0x00011367 )


/** @} */ /* end_name_group Calibration Vocoder Classes */


/****************************************************************************
 * STREAM/VOCPROC CALIBRATION APIS                                          *
 ****************************************************************************/

/** @brief Type definition for #vss_icommon_param_data_t.
*/
typedef struct vss_icommon_param_data_t vss_icommon_param_data_t;

#include "apr_pack_begin.h"

/** @brief Parameter data structure used by the #VSS_ICOMMON_CMD_SET_PARAM_V2
    and #VSS_ICOMMON_RSP_GET_PARAM commands.
*/
struct vss_icommon_param_data_t
{
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
  uint16_t param_size;
    /**< Size of the parameter in bytes. */
  uint16_t reserved;
    /**< Reserved; set to 0. */
#if 0
  uint8_t param_data[param_size];
    /**< Parameter data when received in-band.\n
         Additional repetitions of the modules_id, param_id, param_size,
         reserved sequence can continue below: \n
         uint32_t next_module_id; \n
         . \n
         . \n
         . */

#endif /* 0 */
}
#include "apr_pack_end.h"
;

/**
  Sets one or more calibration parameters. This API is intended for private
  use. Client must not use this API for calibration or UI control purposes.

  @par Payload
  #vss_icommon_cmd_set_param_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with CVD by following @latexonly \hyperref[shmem_guideline]
  {Shared Memory Usage Guideline} @endlatexonly. The parameter data in the 
  memory must be valid and read-only until a response to this command is 
  received.
*/
#define VSS_ICOMMON_CMD_SET_PARAM_V2 ( 0x0001133D )

/** @brief Type definition for #vss_icommon_cmd_set_param_v2_t.
*/
typedef struct vss_icommon_cmd_set_param_v2_t vss_icommon_cmd_set_param_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_PARAM_V2 command.
*/
struct vss_icommon_cmd_set_param_v2_t
{
  uint32_t mem_handle;
    /**< If the parameter data is in-band, set this field to 0. If the
         parameter data is out-of-band, this field is the handle to the shared
         memory that holds the parameter data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly. */
  uint64_t mem_address;
    /**< Location of the parameter data payload. The payload is an array of 
         #vss_icommon_param_data_t. See @latexonly \hyperref[shmem_guideline]
         {Shared Memory Usage Guideline} @endlatexonly on how the address is 
         intrepreted. If the mem_handle is 0, this field is ignored. */
  uint32_t mem_size;
    /**< Size of the parameter data payload in bytes. This field is applicable
         to both in-band and out-of-band parameter data. For out-of-band 
         parameter, the amount of memory allocated at mem_address must meet the
         requirement as specified in @latexonly \hyperref[shmem_guideline]
         {Shared Memory Usage Guideline} @endlatexonly. */

#if 0
  vss_icommon_param_data_t param_data[];
    /**< param_data is present when the data is in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;

/**
  Gets a calibration parameter. This API is intended for private use. Client
  must not use this API for calibration or UI control purposes.

  @par Payload
  #vss_icommon_cmd_get_param_v2_t

  @return
  #VSS_ICOMMON_RSP_GET_PARAM -- In case of success, invalid module ID, invalid
                                parameter ID. \n
  #APRV2_IBASIC_RSP_RESULT -- In case of any failure other than invalid module
                              ID or invalid parameter ID.

  @dependencies
  If the parameter data is out-of-band, the memory that holds the data must
  first be mapped with CVD by following @latexonly \hyperref[shmem_guideline]
  {Shared Memory Usage Guideline} @endlatexonly. The client must not modify 
  the memory and the memory must be valid and writable until a response to this
  command is received.
*/
#define VSS_ICOMMON_CMD_GET_PARAM_V2 ( 0x0001133E )

/** @brief Type definition for #vss_icommon_cmd_get_param_v2_t.
*/
typedef struct vss_icommon_cmd_get_param_v2_t vss_icommon_cmd_get_param_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_GET_PARAM_V2 command.
*/
struct vss_icommon_cmd_get_param_v2_t
{
  uint32_t mem_handle;
    /**< If the parameter data is in-band, set this field to 0. If the
         parameter data is out-of-band, this field is the handle to the shared
         memory that holds the parameter data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly. */
  uint64_t mem_address;
    /**< Location to hold the parameter data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. A single #vss_icommon_param_data_t that 
         contains the header and the algorithm's parameter data will be placed
         at this location. If the mem_handle is 0, this field is ignored. */
  uint16_t mem_size;
    /**< Size of the memory in bytes to hold the parameter. This field is 
         applicable to both in-band and out-of-band parameter. The size must
         be large enough to hold the algorithm's parameter data plus the header
         #vss_icommon_param_data_t. For out-of-band parameter, the size must
         also meet the requirement as specified in @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly. */
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
}
#include "apr_pack_end.h"
;

/**
  Response to the #VSS_ICOMMON_CMD_GET_PARAM_V2 command.

  @par Payload
  #vss_icommon_rsp_get_param_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ICOMMON_RSP_GET_PARAM ( 0x00011008 )

/** @brief Type definition for #vss_icommon_rsp_get_param_t.
*/
typedef struct vss_icommon_rsp_get_param_t vss_icommon_rsp_get_param_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_RSP_GET_PARAM command
   response.
*/
struct vss_icommon_rsp_get_param_t
{
  uint32_t status;
    /**< Status of the GET_PARAM command. Possible values:\n
         - 0 -- Success.\n
         - 1 -- Invalid module ID.\n
         - 2 -- Invalid parameter ID. */

#if 0
  vss_icommon_param_data_t param_data[1];
    /**< param_data is present when the data is in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * STREAM/VOCPROC UI PROPERTY APIS                                          *
 ****************************************************************************/

/**
  Sets a UI property.

  @par Payload
  #vss_icommon_cmd_set_ui_property_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_UI_PROPERTY ( 0x00011103 )

/** @brief Type definition for #vss_icommon_cmd_set_ui_property_t.
*/
typedef struct vss_icommon_cmd_set_ui_property_t vss_icommon_cmd_set_ui_property_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_UI_PROPERTY command.
*/
struct vss_icommon_cmd_set_ui_property_t
{
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
  uint16_t param_size;
    /**< Size of the parameter in bytes. */
  uint16_t reserved;
    /**< Reserved; set to 0. */
#if 0
  uint8_t param_data[param_size];
    /**< Parameter data sent in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;

/**
  Gets a UI property.

  @par Payload
  #vss_icommon_cmd_get_ui_property_t

  @return
  #VSS_ICOMMON_RSP_GET_UI_PROPERTY -- Success; UI property was retrieved.\n
  #APRV2_IBASIC_RSP_RESULT -- Failure; UI property was not retrieved.

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_GET_UI_PROPERTY ( 0x00011104 )

/** @brief Type definition for #vss_icommon_cmd_get_ui_property_t.
*/
typedef struct vss_icommon_cmd_get_ui_property_t vss_icommon_cmd_get_ui_property_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_GET_UI_PROPERTY command.
*/
struct vss_icommon_cmd_get_ui_property_t
{
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
  uint32_t param_size;
    /**< Size of the parameter in bytes. */
}
#include "apr_pack_end.h"
;

/**
  Response to the #VSS_ICOMMON_CMD_GET_UI_PROPERTY command.

  @par Payload
  #vss_icommon_rsp_get_ui_property_t

  @return
  None.

  @dependencies
  None.
 */
#define VSS_ICOMMON_RSP_GET_UI_PROPERTY ( 0x00011105 )

/** @brief Type definition for vss_icommon_rsp_get_ui_property_t.
*/
typedef struct vss_icommon_rsp_get_ui_property_t vss_icommon_rsp_get_ui_property_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_RSP_GET_UI_PROPERTY command
    response.
*/
struct vss_icommon_rsp_get_ui_property_t
{
  uint32_t status;
    /**< Status of the GET_UI_PROPERTY command. Possible values:\n
         - 0 -- Success \n
         - 1 -- Invalid module ID \n
         - 2 -- Invalid parameter ID */
  uint32_t module_id;
    /**< Unique ID of the module. */
  uint32_t param_id;
    /**< Unique ID of the parameter. */
  uint16_t param_size;
    /**< Size of the parameter in bytes. */
  uint16_t reserved;
    /**< Reserved; set to 0. */
#if 0
  uint8_t param_data[param_size];
    /**< Parameter data received in-band. */
#endif /* 0 */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * VOICE TIMING APIS                                                        *
 ****************************************************************************/

/**
  Sets the voice timing parameters and VFR (Vocoder Frame Reference).

  @par Payload
  #vss_icommon_cmd_set_voice_timing_v2_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2 ( 0x00012E66 )

/** Soft VFR mode flag. */
#define VSS_ICOMMON_VFR_MODE_SOFT ( 0 )

/** Hard VFR mode flag.*/
#define VSS_ICOMMON_VFR_MODE_HARD ( 1 )

/** @brief Type definition for #vss_icommon_cmd_set_voice_timing_v2_t.
*/
typedef struct vss_icommon_cmd_set_voice_timing_v2_t vss_icommon_cmd_set_voice_timing_v2_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2 command.
*/
struct vss_icommon_cmd_set_voice_timing_v2_t
{
  uint16_t mode;
    /**< Vocoder frame synchronization mode. Possible values:\n
         - #VSS_ICOMMON_VFR_MODE_SOFT -- No frame synchronization. \n
         - #VSS_ICOMMON_VFR_MODE_HARD -- Hard Vocoder Frame Reference (VFR). 
           A 20 ms VFR interrupt. */
  uint32_t vsid;
    /**< Voice System ID as defined by DCN 80-NF711-1. */
  uint16_t enc_offset;
    /**< Offset in microseconds from the VFR to deliver a Tx vocoder
        packet. The offset is to be less than 20000 us. */
  uint16_t dec_req_offset;
    /**< The offset in microseconds from the VFR to request for an Rx vocoder
         packet. The offset is to be less than 20000 us. */
  uint16_t dec_offset;
    /**< Offset in microseconds from the VFR to indicate the deadline to
         receive an Rx vocoder packet. The offset is to be less than 20000 us.
         Rx vocoder packets received after this deadline are not guaranteed to
         be processed.  */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_common_public_if */

#endif /* __VSS_COMMON_PUBLIC_IF_H__ */

