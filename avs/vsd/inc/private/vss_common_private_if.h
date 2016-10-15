#ifndef __VSS_COMMON_PRIVATE_IF_H__
#define __VSS_COMMON_PRIVATE_IF_H__

/*
   Copyright (C) 2010-2013 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary

   $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/vsd/inc/private/vss_common_private_if.h#12 $
   $Author: coresvc $
*/

#include "apr_comdef.h"
#include "vss_common_public_if.h"


/****************************************************************************
 * VOICE SYSTEM CONFIG APIS                                                 *
 ****************************************************************************/

/**
  Command issued by the MVM to attached streams and vocprocs, to propagate
  the current system configuration.

  @par Payload
  #vss_icommon_cmd_set_system_config_t

  @return
  #VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG -- In case of success.
  #APRV2_IBASIC_RSP_RESULT -- In case of failure.
 
  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG ( 0x00012A0F )

/** @brief Type definition for #vss_icommon_cmd_set_system_config_t.
*/
typedef struct vss_icommon_cmd_set_system_config_t vss_icommon_cmd_set_system_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_SYSTEM_COPNFIG command.
*/
struct vss_icommon_cmd_set_system_config_t
{
  uint32_t network_id;
    /** Network ID. Supported values: \n
        see @latexonly \hyperref[cal_network_ID]{Calibration Network IDs} @endlatexonly. */
  uint32_t voc_class;
    /** Vocoder class. Supported values: Supported values: \n
        see @latexonly \hyperref[cal_voc_class]{Calibration Vocoder Classes} @endlatexonly. */
  uint32_t dec_sr;
    /** Decoder sampling rate. */
  uint32_t enc_sr;
    /** Encoder sampling rate. */
  uint32_t rx_pp_sr;
    /** RX post-processing blocks sampling rate. */
  uint32_t tx_pp_sr;
    /** TX pre-processing blocks sampling rate. */
  uint32_t vsid;
    /**< Voice System ID as defined by DCN 80-NF711-1. */
  uint16_t vfr_mode;
    /**< Vocoder frame synchronization mode. Possible values:\n
         - #VSS_ICOMMON_VFR_MODE_SOFT -- No frame synchronization. \n
         - #VSS_ICOMMON_VFR_MODE_HARD -- Hard Vocoder Frame Reference (VFR). 
           A 20 ms VFR interrupt. */
  uint16_t timewarp_mode;
    /**< Timewarping mode. Possible values:\n
         - 0x0001 -- Timewarping enabled.
         - 0x0000 -- Timewarping disabled. */
}
#include "apr_pack_end.h"
;


/**
  Response to the #VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG command.

  @par Payload
  #vss_icommon_rsp_set_system_config_t

  @return
  None.

  @dependencies
  None.
*/

#define VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG ( 0x00012E69 )

/** @brief Type definition for #vss_icommon_rsp_set_system_config_t.
*/
typedef struct vss_icommon_rsp_set_system_config_t vss_icommon_rsp_set_system_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_RSP_SET_SYSTEM_COPNFIG command.
*/
struct vss_icommon_rsp_set_system_config_t
{
  uint32_t dec_sr;
    /** Decoder sampling rate. */
  uint32_t enc_sr;
    /** Encoder sampling rate. */
  uint32_t rx_pp_sr;
    /** RX post-processing blocks sampling rate. */
  uint32_t tx_pp_sr;
    /** TX pre-processing blocks sampling rate. */
  uint32_t vocproc_tx_topology_id;
    /**< Vocproc Tx path topology ID. */
  uint32_t vocproc_rx_topology_id;
    /**< Vocproc Rx path topology ID. */
  uint32_t enc_kpps;
    /**< Encoder KPPS requirements. */
  uint32_t dec_kpps;
    /**< Decoder KPPS requirements. */
  uint32_t dec_pp_kpps;
    /**< Stream RX post-processing block KPPS requirements. */
  uint32_t vp_rx_kpps;
    /**< Vocproc RX KPPS requirements. */
  uint32_t vp_tx_kpps;
    /**< Vocproc TX KPPS requirements. */
  uint16_t timewarp_mode;
    /**< Timewarping mode. Possible values:\n
         - 0x0001 -- Timewarping enabled.
         - 0x0000 -- Timewarping disabled. */
}
#include "apr_pack_end.h"
;

#endif /* __VSS_COMMON_PRIVATE_IF_H__ */

