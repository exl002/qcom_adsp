#ifndef __VSS_ISTREAM_PRIVATE_IF_H__
#define __VSS_ISTREAM_PRIVATE_IF_H__

/*
   Copyright (C) 2010-2013 QUALCOMM Technologies Incorporated.
   All rights reserved.
   Qualcomm Confidential and Proprietary

   $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/vsd/inc/private/vss_istream_private_if.h#12 $
   $Author: coresvc $
*/


#include "apr_comdef.h"
#include "vss_istream_public_if.h"


/****************************************************************************
 * CORE STREAM APIS                                                         *
 ****************************************************************************/

#define VSS_ISTREAM_CMD_ENABLE ( 0x00010009 )
   /**< No payload. Wait for APRV2_IBASIC_RSP_RESULT response. */


#define VSS_ISTREAM_CMD_DISABLE ( 0x0001000B )
   /**< No payload. Wait for APRV2_IBASIC_RSP_RESULT response. */


#define VSS_ISTREAM_CMD_REINIT (0x00011228)
   /**< No payload.
    * Issued to stream to flush the buffer and state.
    */

/**
  Indicates to the stream that an MVM session is attaching to it.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT.

  @dependencies
  None.

  @comments
  None.
*/
#define VSS_ISTREAM_CMD_MVM_ATTACH ( 0x00012E6E )


/**
  Indicates to the stream that an MVM session is detaching from it.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT.

  @dependencies
  None.

  @comments
  None.
*/
#define VSS_ISTREAM_CMD_MVM_DETACH ( 0x00012E6F )


/****************************************************************************
 * STREAM CONCURRENCY MONITORING APIS                                       *
 ****************************************************************************/

/**
  Event generated by the stream to the MVM session to which it is connected,
  notifying the MVM session that the stream's configuration has changed.
  Specifically, this event is generated under any of the following
  circumstances:
 
    - upon the stream receiving a #VSS_ISTREAM_CMD_SET_MEDIA_TYPE command
      with a different media type from that currently configured on the
      stream.
 
    - upon the stream receiving a #VSS_ISTREAM_CMD_SET_VAR_VOC_SAMPLING_RATE
      command with different sample rates from those currently configured
      on the stream.
 
    - upon the stream receiving a #VSS_ICOMMON_CMD_SET_PARAM_V2 command that
      results in a KPPS requirements change.
 
    - upon a new set of calibration data being registered with the stream.
 
  All of the above events have the effect of potentially changing the current
  KPPS requirements of the stream.

  @par Payload
  None.

  @return
  None.

  @dependencies
  None.

  @comments
  None.
*/
#define VSS_ISTREAM_EVT_RECONFIG ( 0x00012E70 )


/****************************************************************************
 * STREAM TIMING APIS                                                       *
 ****************************************************************************/

/**
  Sets voice path timing offsets on the stream.

  @par Payload
  #vss_istream_cmd_set_voice_timing_t

  @return
  #APRV2_IBASIC_RSP_RESULT.

  @dependencies
  None.

  @comments
  None.
*/
#define VSS_ISTREAM_CMD_SET_VOICE_TIMING ( 0x00012E71 )

/** @brief Type definition for #vss_istream_cmd_set_voice_timing_t.
*/
typedef struct vss_istream_cmd_set_voice_timing_t vss_istream_cmd_set_voice_timing_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_VOICE_TIMING command.
*/
struct vss_istream_cmd_set_voice_timing_t
{
  uint16_t enc_offset;
    /**< Offset in microseconds from the VFR to deliver a Tx vocoder
        packet. Supported values:\n
         - 0 to 20000 microseconds. */
  uint16_t dec_req_offset;
    /**< The offset in microseconds from the VFR to request for an Rx vocoder
         packet. Supported values:\n
         - 0 to 20000 microseconds. */
  uint16_t dec_offset;
    /**< Offset in microseconds from the VFR to indicate the deadline to
         receive an Rx vocoder packet. Rx vocoder packets received after this
         deadline are not guaranteed to be processed. Supported values:\n
         - 0 to 20000 microseconds. */
  uint16_t dec_pp_start_offset;
    /**< Offset in microseconds from the VFR to indicate the start time of
         the stream RX post-processing block. Supported values:\n
         - 0 to 20000 microseconds. */
  uint16_t vp_tx_delivery_offset;
    /**< Offset in microseconds from the VFR to indicate the delivery to encoder 
         time of the vocproc TX block. Supported values:\n
         - 0 to 20000 microseconds. */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * STREAM AVSYNC APIS                                                       *
 ****************************************************************************/

/**
  Sets the vocproc RX and TX normalized total AVSync delays.

  @par Payload
  #vss_istream_cmd_set_vocproc_avsync_delays_t

  @return
  #APRV2_IBASIC_RSP_RESULT.

  @dependencies
  None.

  @comments
  None.
*/
#define VSS_ISTREAM_CMD_SET_VOCPROC_AVSYNC_DELAYS ( 0x00012EF4 )

/** @brief Type definition for #vss_istream_cmd_set_vocproc_avsync_delays_t.
*/
typedef struct vss_istream_cmd_set_vocproc_avsync_delays_t vss_istream_cmd_set_vocproc_avsync_delays_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ISTREAM_CMD_SET_VOCPROC_AVSYNC_DELAYS command.
*/
struct vss_istream_cmd_set_vocproc_avsync_delays_t
{
  uint32_t vp_rx_normalized_total_delay;
    /**< Normalized total vocproc RX delay + total AFE RX delay in microseconds. */
  uint32_t vp_tx_normalized_total_delay;
    /**< Normalized total vocproc TX delay + total AFE TX delay in microseconds. */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * STREAM PROPERTY APIS                                                     *
 ****************************************************************************/

#define VSS_ISTREAM_CMD_RESYNC_CTM ( 0x00011021 )
   /**< Wait for APRV2_IBASIC_RSP_RESULT response. */


#define VSS_ISTREAM_CMD_SET_DEJITTER_MODE ( 0x0001000C )
   /**< Wait for APRV2_IBASIC_RSP_RESULT response. */

typedef struct vss_istream_cmd_set_dejitter_mode_t vss_istream_cmd_set_dejitter_mode_t;

#include "apr_pack_begin.h"
struct vss_istream_cmd_set_dejitter_mode_t
{
   uint16_t mode;
      /**<
       * Set the dejitter mode.
       *
       * 0x0000 : Default (voice call)
       * 0x0001 : Circular buffer (oldest frames dropped)
       */
   uint16_t num_frames;
      /**<
       * Set the number of frames to buffer.
       */
}
#include "apr_pack_end.h"
;


#define VSS_ISTREAM_CMD_SET_DATA_CLIENT ( 0x0001113A )
    /**< No payload. Wait for APRV2_IBASIC_RSP_RESULT response. */


/****************************************************************************
 * CDMA VOIP VOCODER APIS                                                   *
 ****************************************************************************/

#define VSS_ISTREAM_CMD_CDMA_VOIP_SET_ENC_DTX_MODE ( 0x0001000F )
   /**< Wait for APRV2_IBASIC_RSP_RESULT response. */

typedef struct vss_istream_cmd_cdma_voip_set_enc_dtx_mode_t vss_istream_cmd_cdma_voip_set_enc_dtx_mode_t;

#include "apr_pack_begin.h"
struct vss_istream_cmd_cdma_voip_set_enc_dtx_mode_t
{
   uint16_t enable;
      /**<
       * Toggle DTX on or off.
       *
       * 0x0000 : Disables DTX
       * 0xFFFF : Enables DTX
       */
   uint16_t min_interval;
      /**<
       * Set the minimum DTX update interval in number of frames.
       *
       * 0x0000 : Default (12 frames)
       * 0x**** : Number of frames
       */
   uint16_t max_interval;
      /**<
       * Set the maximum DTX update interval in number of frames.
       *
       * 0x0000 : Default (32 frames)
       * 0x**** : Number of frames
       */
   uint16_t hangover;
      /**<
       * Set the Eighth-Rate Hangover in number of frames.
       *
       * 0x0000 : Default (1 frame)
       * 0x**** : Number of frames
       */
}
#include "apr_pack_end.h"
;

/****************************************************************************
 * CALIBRATION DATA APIS                                                    *
 ****************************************************************************/

/**
  Evaluates the amount of memory required by the stream from the client for
  using a faster-than-linear search algorithm for looking up entries in the
  client-registered calibration data table.

  @par Payload
  #vss_istream_cmd_eval_cal_indexing_mem_size_t

  @return
  #VSS_ISTREAM_RSP_EVAL_CAL_INDEXING_MEM_SIZE -- In case of success.\n
  #APRV2_IBASIC_RSP_RESULT -- In case of failure.

  @dependencies
  The calibration data table memory provided here must first be mapped with CVD
  by following @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
  Guideline} @endlatexonly.

  @comments
  None.
*/
#define VSS_ISTREAM_CMD_EVAL_CAL_INDEXING_MEM_SIZE ( 0x00011354 )

/** @brief Type definition for #vss_istream_cmd_eval_cal_indexing_mem_size_t.
*/
typedef struct vss_istream_cmd_eval_cal_indexing_mem_size_t vss_istream_cmd_eval_cal_indexing_mem_size_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_ISTREAM_CMD_EVAL_CAL_INDEXING_MEM_SIZE command.

    Expected buffer format: Must match the format specified in the
    #VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V3 command.
*/
struct vss_istream_cmd_eval_cal_indexing_mem_size_t
{
  uint32_t mem_handle;
    /**< Handle to the shared memory that holds the calibration data.
         See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
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
  uint32_t num_columns;
    /**< Number of columns used to form the key for looking up a calibration data
         entry. */
#if __STDC_VERSION__ >= 199901L
  vss_istream_cal_column_t columns[];
#endif /* __STDC_VERSION__ >= 199901L */
    /**< Array of columns of size num_columns. The order in which the colums are
         provided here must match the order in which they exist in the calibration
         table provided. */
}
#include "apr_pack_end.h"
;


/**
  Response to the #VSS_ISTREAM_CMD_EVAL_CAL_INDEXING_MEM_SIZE
  command.

  @par Payload
  #vss_istream_rsp_eval_cal_indexing_mem_size_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_ISTREAM_RSP_EVAL_CAL_INDEXING_MEM_SIZE ( 0x00011355 )

/** @brief Type definition for #vss_istream_rsp_eval_cal_indexing_mem_size_t.
*/
typedef struct vss_istream_rsp_eval_cal_indexing_mem_size_t vss_istream_rsp_eval_cal_indexing_mem_size_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_ISTREAM_RSP_EVAL_CAL_INDEXING_MEM_SIZE command response.
*/
struct vss_istream_rsp_eval_cal_indexing_mem_size_t
{
  uint32_t mem_size;
    /**< Size in bytes of the memory required by the stream from the client for
         using a faster-than-linear search algorithm for looking up entries
         in the client-registered calibration data table. */
}
#include "apr_pack_end.h"
;


/**
  Registers the memory that contains common calibration data table with the
  stream.

  As part of this registration, in addition to the address, size and handle
  of the memory containing the calibration table, the client also porvides
  the columns and associated column types which the stream uses to form a
  key that is used to index into the calibration table and look-up a matching
  entry. The columns/types must be provided in this command in the same order
  that they exist in the calibration table. If no matching entry is found,
  calibration data is not applied.

  Calibration data is applied when the stream transisions to the run state.

  @par Payload
  #vss_istream_cmd_register_calibration_data_v3_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  Any memory registered here must first be mapped with CVD by following
  @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
  @endlatexonly. 

  For using a faster-than-linear search algorithm when looking up entries in
  the registered calibration data table, the client must provide cal-indexing
  memory to the stream. The size of the memory that the client must provide
  is queried via #VSS_ISTREAM_CMD_EVAL_CAL_INDEXING_MEM_SIZE, and the memory
  must be readable and writable by CVD. The calibration data table used for
  evaluation must be the same as the one registered here. If the client does
  not provide cal-indexing memory to the stream, then linear search is 
  performed whenever calibration data must be looked up to be applied to the
  stream.
  
  @comments
  The client is not allowed to modify the calibration data memory while it
  is registered and the stream does not modify this memory while registered.
  The calibration data at the registered memory is valid until unregistered. 
  Registering a memory region for calibration data while another region is
  still registered causes the command to fail.
  
  The client is not allowed to modify the cal-indexing memory while
  it is registered with the stream. The stream modifies this memory for its
  internal purposes while it is registered.
*/
#define VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V3 ( 0x00011386 )

/** @brief Type definition for #vss_istream_cmd_register_calibration_data_v3_t.
*/
typedef struct vss_istream_cmd_register_calibration_data_v3_t vss_istream_cmd_register_calibration_data_v3_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the
    #VSS_ISTREAM_CMD_REGISTER_CALIBRATION_DATA_V3 command.

    Expected buffer format:

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
     -------------------------             -------------------------
    | column_2                |           | param_data              |
     -------------------------	          |                         |
    |  .                      |           | ...                     |
    |  .                      |            -------------------------
     -------------------------	          | mod_id                  |
    | column_n                |           |                         |
     -------------------------	          |                         |
    | data_size               |           .                         .
     -------------------------            .                         .
    | data                    |           .                         .
    |                         |
    |                         |
    | ...                     |
     ------------------------- 
    |                         |
    .                         .
    .                         .

    @endverbatim
*/
struct vss_istream_cmd_register_calibration_data_v3_t
{
  uint32_t cal_indexing_mem_handle;
    /**< Handle to the shared memory that is used by the stream for performing 
         faster-than-linear look-up of calibration data table entries. See 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. Set this field to 0 if the memory is not provided. */
  uint64_t cal_indexing_mem_address;
    /**< Location of the shared memory that is used by the stream for performing 
         faster-than-linear look-up of calibration data table entries. See
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly on how the address is intrepreted. This field is ignored
         if the cal_indexing_mem_handle is 0. */
  uint32_t cal_indexing_mem_size;
    /**< Size of the shared memory that is used by the stream for performing 
         faster-than-linear look-up of calibration data table entries. The amount
         of memory allocated at cal_indexing_mem_handle must meet the requirement
         as specified in @latexonly \hyperref [shmem_guideline]{Shared Memory Usage
         Guideline} @endlatexonly. This field is ignored if the cal_indexing_mem_handle
         is 0. */
  uint32_t cal_mem_handle;
    /**< Handle to the shared memory that holds the calibration data.
         See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage
         Guideline} @endlatexonly. */
  uint64_t cal_mem_address;
    /**< Location of calibration data. See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is intrepreted. */
  uint32_t cal_mem_size;
    /**< Size of the calibration data in bytes. The amount of memory allocated
         at cal_mem_address must meet the requirement as specified in 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
  uint32_t num_columns;
    /**< Number of columns used to form the key for looking up a calibration data
         entry. */
#if __STDC_VERSION__ >= 199901L
  vss_istream_cal_column_t columns[];
#endif /* __STDC_VERSION__ >= 199901L */
    /**< Array of columns of size num_columns. The order in which the colums are
         provided here must match the order in which they exist in the calibration
         table provided. */
}
#include "apr_pack_end.h"
;


#endif /* __VSS_ISTREAM_PRIVATE_IF_H__ */
