#ifndef __VSS_COMMON_DEPRECATED_PUBLIC_IF_H__
#define __VSS_COMMON_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_common_deprecated_public_if.h
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

      The above description for this file is part of the
      vss_common_deprecated_public_if group description in the CVD_mainpage.dox
      file.
=============================================================================*/
/*===========================================================================
Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/cvd/vss_common_deprecated_public_if.h#10 $
  $Author: coresvc $

===========================================================================*/

#include "apr_comdef.h"
#include "vss_common_public_if.h"

/** @addtogroup vss_common_deprecated_public_if
@{ */

/*--------------------------------------------------------------------------*
 * NETWORK DEFINITIONS                                                      *
 *--------------------------------------------------------------------------*/

/** @name Deprecated Network IDs
  @latexonly \label{deprecated_network_ID} @endlatexonly
  @{ */

/** Default network ID. */
#define VSS_NETWORK_ID_DEFAULT ( 0x00010037 )

/** CDMA narrowband network ID. */
#define VSS_NETWORK_ID_CDMA_NB ( 0x00010021 )

/** CDMA wideband network ID. */
#define VSS_NETWORK_ID_CDMA_WB ( 0x00010022 )

/** CDMA WideVoice network ID. */
#define VSS_NETWORK_ID_CDMA_WV ( 0x00011100 )

/** GSM narrowband network ID. */
#define VSS_NETWORK_ID_GSM_NB ( 0x00010023 )

/** GSM wideband network ID. */
#define VSS_NETWORK_ID_GSM_WB ( 0x00010024 )

/** GSM WideVoice network ID. */
#define VSS_NETWORK_ID_GSM_WV ( 0x00011101 )

/** WCDMA narrowband network ID. */
#define VSS_NETWORK_ID_WCDMA_NB ( 0x00010025 )

/** WCDMA wideband network ID. */
#define VSS_NETWORK_ID_WCDMA_WB ( 0x00010026 )

/** WCDMA WideVoice network ID. */
#define VSS_NETWORK_ID_WCDMA_WV ( 0x00011102 )

/** Voice over Internet Protocol (VoIP) narrowband network ID. */
#define VSS_NETWORK_ID_VOIP_NB ( 0x00011240 )

/** VoIP wideband network ID. */
#define VSS_NETWORK_ID_VOIP_WB ( 0x00011241 )

/** VoIP WideVoice network ID. */
#define VSS_NETWORK_ID_VOIP_WV ( 0x00011242 )

/** LTE narrowband network ID. */
#define VSS_NETWORK_ID_LTE_NB ( 0x00011324 )

/** LTE wideband network ID. */
#define VSS_NETWORK_ID_LTE_WB ( 0x00011325 )

/** LTE WideVoice network ID. */
#define VSS_NETWORK_ID_LTE_WV ( 0x00011326 )

/** @} */ /* end_name_group Deprecated Network IDs */

/*--------------------------------------------------------------------------*
 * MEDIA TYPE DEFINITIONS                                                   *
 *--------------------------------------------------------------------------*/

/** @name Deprecated Media Type IDs
  @latexonly \label{deprecated_media_ID} @endlatexonly
  @{ */

/** CDMA variable 13K vocoder format. */
#define VSS_MEDIA_ID_13K_MODEM ( 0x00010FC1 )

/** CDMA Enhanced Variable Rate Vocoder (EVRC) format. */
#define VSS_MEDIA_ID_EVRC_MODEM ( 0x00010FC2 )

/** CDMA 4th generation NB vocoder format. */
#define VSS_MEDIA_ID_4GV_NB_MODEM ( 0x00010FC3 )

/** CDMA 4th generation WB vocoder format. */
#define VSS_MEDIA_ID_4GV_WB_MODEM ( 0x00010FC4 )

/** CDMA 4th generation Narrow-Wide (NW) vocoder format. */
#define VSS_MEDIA_ID_4GV_NW_MODEM ( 0x00010FC5 )

/** Universal Mobile Telecommunications System (UMTS) AMR-NB vocoder. */
#define VSS_MEDIA_ID_AMR_NB_MODEM ( 0x00010FC6 )

/** UMTS AMR-WB vocoder format. */
#define VSS_MEDIA_ID_AMR_WB_MODEM ( 0x00010FC7 )

/** GSM enhanced full rate vocoder format. */
#define VSS_MEDIA_ID_EFR_MODEM ( 0x00010FC8 )

/** GSM full rate vocoder format. */
#define VSS_MEDIA_ID_FR_MODEM ( 0x00010FC9 )

/** GSM half rate vocoder format. */
#define VSS_MEDIA_ID_HR_MODEM ( 0x00010FCA )

/** Linear Pulse Code Modulation (PCM) vocoder modem format; mono, 16 bits,
  little endian, 8 KHz. */
#define VSS_MEDIA_ID_PCM_NB ( 0x00010FCB )

/** Linear PCM vocoder modem format; mono, 16 bits, little endian, 16 KHz. */
#define VSS_MEDIA_ID_PCM_WB ( 0x00010FCC )

/** @} */ /* end_name_group Deprecated Media Type IDs */


/****************************************************************************
 * NETWORK SYSTEM APIS                                                      *
 ****************************************************************************/

/**
  Sets the network type.

  @deprecated
  This command is deprecated. Use #VSS_IMVM_CMD_SET_CAL_NETWORK.

  @par Payload
  #vss_icommon_cmd_set_network_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_NETWORK ( 0x0001119C )               

/** @brief Type definition for #vss_icommon_cmd_set_network_t.
*/
typedef struct vss_icommon_cmd_set_network_t vss_icommon_cmd_set_network_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_NETWORK command.
*/
struct vss_icommon_cmd_set_network_t
{
  uint32_t network_id;
    /**< Network ID (see @latexonly \hyperref[network_ID]{Network IDs}
         @endlatexonly). */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * VOICE TIMING APIS                                                        *
 ****************************************************************************/

/**
  Sets the voice timing parameters. 
   
  @deprecated
  This command is deprecated. Use #VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2. 

  @par Payload
  #vss_icommon_cmd_set_voice_timing_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_VOICE_TIMING ( 0x000111E0 )

/** @brief Type definition for #vss_icommon_cmd_set_voice_timing_t.
*/
typedef struct vss_icommon_cmd_set_voice_timing_t vss_icommon_cmd_set_voice_timing_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_VOICE_TIMING command.
*/
struct vss_icommon_cmd_set_voice_timing_t
{
  uint16_t mode;
    /**< Vocoder frame synchronization mode. Possible values:\n
         - #VSS_ICOMMON_VFR_MODE_SOFT -- No frame synchronization. \n
         - #VSS_ICOMMON_VFR_MODE_HARD -- Hard Vocoder Frame Reference (VFR). 
           A 20 ms VFR interrupt. */
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

/** @} */ /* end_addtogroup vss_common_deprecated_public_if */

#endif /* __VSS_COMMON_DEPRECATED_PUBLIC_IF_H__ */

