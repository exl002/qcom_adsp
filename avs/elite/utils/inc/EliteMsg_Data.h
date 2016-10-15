#ifndef ELITEMSG_DATA_H
#define ELITEMSG_DATA_H

/**
@file EliteMsg_Data.h

@brief This file defines secondary opcode and payload structure
for following Elite data path messages. ELITE_DATA_MEDIA_TYPE and ELITE_DATA_EOS
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The descriptions that appera in the PDF are maintained in the
      Elite_Core_mainpage.dox file. Contact Tech Pubs for support.
===========================================================================*/

/*========================================================================
   Copyright (c) 2010, 2013 Qualcomm Technologies, Inc.  All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.2.
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
02/04/10   DC      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
#include "EliteMsg.h"
#include "EliteAprIf.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup elite_msg_sec_opcodes
   @{ */

   /*-------------------------------------------------------------------------
   Preprocessor Definitions and Constants
   -------------------------------------------------------------------------*/
   /* These define the media type format that are supported. */
   //#define ELITEMSG_DATA_MEDIA_TYPE_ADSPAUDIO  0 /* This is deprecated. Indicates the rest of the payload is an AdspAudioFormat */

/** Indicates that the rest of the payload is in a media format defined in 
    @xhyperref{Q10,[Q10}.
 */ /* Q10 = Hex Multimedia 2.0: Audio API Interface Spec (80-NF768-1) */
#define ELITEMSG_DATA_MEDIA_TYPE_APR        1


   /* These define the EOS command format that are supported. */
   //#define ELITEMSG_DATA_EOS_ADSPAUDIO  0 /* This is deprecated. Indicates the rest of the payload is EOS command
   //                                                    in the format of AdspAudioAnyCommand.  */


/** Indicates that this EOS originates from the client's EOS command. The message
   payload is elite_msg_data_eos_apr_t.
   */
#define ELITEMSG_DATA_EOS_APR        1

/** Indicates that this Tx-path EOS is generated internally -- the EOS can be
   triggered either by the Elite service or by an EOS command from the client.

   On receiving this message, the encoder service generates an ASM_DATA_EOS event
   to the client. The message payload is elite_msg_data_tx_eos_t, which is the
   same as elite_msg_data_eos_header_t.
   */
#define ELITEMSG_DATA_TX_EOS        2


/** Indicates that the only action required to take place is resetting the
  session clock.
  
  If the next data buffer contains a valid timestamp, the
  session clock synchronizes with that timestamp. Otherwise, the session
  clock starts incrementing from zero.
*/
#define ELITEMSG_DATA_RESET_SESSION_CLK       3

/** Indicates that this End of Tone originates from the DTMF stream leg
    decoder. The message payload is elite_msg_data_eos_apr_t.
   */
#define ELITEMSG_DATA_DTMF_EOT              4

/** A special EOS type that is used in gapless playback when only one
  stream is open as part of a gapless playback session.

  The actions that are performed are similar to a regular non-gapless EOS
  with the exception that the session clock is reset.
*/
#define ELITEMSG_DATA_GAPLESS_TERMINATION_EOS          5

/** Multichannel PCM media format message type.
  This message is used between Elite modules/services while sending media type
  messages.

  The message payload is elite_multi_channel_pcm_fmt_blk_t}
 */
#define ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM             6

/**
 * Compressed media format message type: this message is used between elite modules/services
 * while sending media type message. Structure is elite_compressed_fmt_blk_t
 */
#define ELITEMSG_MEDIA_FMT_COMPRESSED             7

   /** Forward declaration of the payload type for the #ELITEMSG_DATA_TX_EOS
       message.
   */
   typedef   elite_msg_data_eos_header_t    elite_msg_data_tx_eos_t;


   /*-------------------------------------------------------------------------
   Type Declarations
   -------------------------------------------------------------------------*/

   /* Deprecated */
   //typedef struct
   //{
   //    qurt_elite_queue_t   *pBufferReturnQ;  /* Queue to which this payload
   //                                         buffer must be returned. */
   //    qurt_elite_queue_t   *pResponseQ;      /* Queue to which to send the acknowledgment.
   //                                        NULL indicates that no response is required. */
   //    uint32_t            unClientToken;    /* Token to be given in the
   //                                           acknowledgment. This is different
   //                                           from the unResponseResult and can be
   //                                           used to identify which service sent
   //                                           the response to the server. */
   //    uint32_t            unResponseResult; /* Response result that the server
   //                                           sends back to the client. */
   //    uint32_t            unMediaTypeFormat;  /* This field indicates the format of
   //                                                    the rest of the payload.   */
   //
   //    AdspAudioFormat   mediaFormat;  /* Media format buffer in ADSPAudioDriver format. */
   //
   //} elite_msg_data_media_type_adsp_audio_t;


   /** Payload of the #ELITE_DATA_MEDIA_TYPE message.

      This payload indicates the media type for all subsequent data buffers.
      Only unMediaTypeFormat = #ELITEMSG_DATA_MEDIA_TYPE_APR is supported.

   @note1hang When FLUSH completes, each service precedes any subsequent data
   buffers with ELITE_DATA_MEDIA_TYPE in case there is confusion about a media
   type after FLUSH.
   */
   typedef struct
   {
      qurt_elite_queue_t   *pBufferReturnQ;
      /**< Queue to which this payload buffer must be returned. */

      qurt_elite_queue_t   *pResponseQ;
      /**< Queue to which to send the acknowledgment. NULL indicates that no
                                         no response is required. */

      uint32_t        unClientToken;
      /**< Token to be given in the acknowledgment. This is different from the
           unResponseResult and can be used to identify which service sent the
                                         the response to the server.*/

      uint32_t        unResponseResult;
      /**< Response result that the server sends back to the client. */

      uint32_t        unMediaTypeFormat;
      /**< Format of the rest of the payload. When set to
           #ELITEMSG_DATA_MEDIA_TYPE_APR, the rest of the payload is as
           defined in @xhyperref{Q10,[Q10]}. */

      uint32_t        unMediaFormatID;
      /**< Media format ID as defined in @xhyperref{Q10,[Q10]}. */

   } elite_msg_data_media_type_apr_t;


   /** Part of the payload structure for the end of the stream with 
       unEosFormat == #ELITEMSG_DATA_EOS_APR. 
   */
   typedef struct
   {
      uint16_t self_port;
      /**< Port of the service that receives or originates the EOS. */

      uint16_t self_addr;
      /**< Address of the service that receives or originates the EOS. */

      uint16_t client_port;    /**< Client's port. */
      uint16_t client_addr;    /**< Client's address. */
      uint32_t client_token;   /**< Client's token. */

      uint32_t event_opcode;
      /**< Operation code for the EOS event as defined in @xhyperref{Q10,[Q10]}. */

   } elite_msg_data_eos_info_t;

   /** Payload of the ELITE_DATA_EOS message, signaling the end of the stream.

       The service that receives this message is requested to consume all
       buffers. It supports unEosFormat == #ELITEMSG_DATA_EOS_APR or
       #ELITEMSG_DATA_TX_EOS.
   */
   typedef struct
   {
      qurt_elite_queue_t   *pBufferReturnQ;
      /**< Queue to which this payload buffer must be returned. */

      qurt_elite_queue_t   *pResponseQ;
      /**< Queue to which to send the acknowledgment. NULL indicates that no
           response is required. */

      uint32_t        unClientToken;
      /**< Token to be given in the acknowledgment.

	       This is different from unResponseResult and can be used to identify
		   which service sent the response to the server. */

      uint32_t        unResponseResult;
      /**< Response result that the server sends back to the client. */

      uint32_t        unEosFormat;
      /**< Format of the rest of the payload.

           If set to #ELITEMSG_DATA_EOS_APR, this EOS originates from the
           client.

           If set to #ELITEMSG_DATA_TX_EOS, this EOS originates from Elite
           internally (triggered either by Elite or by an EOS command from
           the client). */

      elite_msg_data_eos_info_t eosInfo;
      /**< Contains the repacketized EOS information for sending an EOS
                                         acknowledgment back to the client.*/

   } elite_msg_data_eos_apr_t ;

   /** Payload of the ELITE_DATA_MARK_BUFFER message, signaling the end of the stream.

       The service that receives this message is requested to pass the buffer to the
       downstream service under normal conditions and raise mark buffer discarded event
       if they are instructed to flush data input before they can be processed.
   */
   typedef struct
      {
         qurt_elite_queue_t   *pBufferReturnQ;
         /**< Queue to which this payload buffer must be returned. */

         qurt_elite_queue_t   *pResponseQ;
         /**< Queue to which to send the acknowledgment. NULL indicates that no
              response is required. */

         uint32_t        unClientToken;
         /**< Token to be given in the acknowledgment.

             This is different from unResponseResult and can be used to identify
            which service sent the response to the server. */

         uint32_t        unResponseResult;
         /**< Response result that the server sends back to the client. */

         uint32_t       token_lsw;
         /**< MSW of the token of the mark buffer */

         uint32_t       token_msw;
         /**< MSW of the token of the mark buffer */
      } elite_msg_data_mark_buffer_t ;

   /** Payload of the #ELITE_DATA_SET_PARAM message, used to send parameters
      that are in sync with data.
   */
   typedef struct
   {
      qurt_elite_queue_t   *pBufferReturnQ;
	  /**< Queue to which this payload buffer must be returned. */

      qurt_elite_queue_t   *pResponseQ;
	  /**< Queue to which to send the acknowledgment. NULL indicates that
                                        no response is required. */

      uint32_t        unClientToken;
	  /**< Token to be given in the acknowledgment.

           This is different from the unResponseResult and can be used to
		   identify which service sent the response to the server. */

      uint32_t        unResponseResult;
	  /**< Response result that the server sends back to the client. */

      uint32_t        unPayloadSize;
	  /**< Size of the following payload. The payload format is (possibly)
	       multiple sets of asm_stream_param_data_t followed by the parameter
		   values. */

   } elite_msg_data_set_param_t ;

/** @} */ /* end_addtogroup elite_msg_sec_opcodes */

   /* deprecated */
   //typedef struct
   //{
   //    qurt_elite_queue_t   *pBufferReturnQ;  /* Queue to which this payload
   //                                         buffer must be returned. */
   //    qurt_elite_queue_t   *pResponseQ;      /* Queue to which to send the acknowledgment.
   //                                        NULL indicates that no response is required. */
   //    uint32_t            unClientToken;    /* TToken to be given in the
   //                                           acknowledgment. This is different
   //                                           from the unResponseResult and can be
   //                                           used to identify which service sent
   //                                           the response to the server. */
   //    uint32_t            unResponseResult; /* This is to be filled with acknowledgment results by the client. */
   //    uint32_t            unEosFormat;       /* This field indicates the format of
   //                                               the rest of the payload. */
   //    struct AdspAudioAnyCommand      eosCmd;     /* EOS command in ADSPAudioDriver format */
   //
   //} elite_msg_data_eos_adsp_audio_t ;

   /*---------------------------------------------------------------------------
   Class Definitions
   ----------------------------------------------------------------------------*/

   /*---------------------------------------------------------------------------
   Function Declarations and Documentation
   ----------------------------------------------------------------------------*/
   /** @ingroup elitemsg_func_get_med_fmt_blk
     Gets the media format block from elite_msg_data_media_type_apr_t.

     @datatypes
     elite_msg_data_media_type_apr_t

   @param[in] pPayload   Points to the message buffer.

   @return
   Pointer to the media format block.

   @dependencies
   None.
   */
   static inline void* elite_msg_get_media_fmt_blk( elite_msg_data_media_type_apr_t *pPayload)
   {
      //
      //This is the code if unFmtBlk is present return (void*) (&( pPayload->unFmtBlk )) ;

      //The following is the code if unFmtBlk is not present;
      uint8_t *pTmp = (uint8_t*) pPayload;
      uint8_t *pTmp2 = pTmp+ sizeof(elite_msg_data_media_type_apr_t);
      return (void*) pTmp2;
   }

   /** @ingroup elitemsg_func_extr_client_info
   Repacketizes an EOS command from the client to an Elite internal data
   structure that contains all necessary information for Elite services to
   raise an EOS acknowledgment to the client.

     @datatypes
     elite_apr_packet_t \n
     elite_msg_data_eos_info_t

   @param[in]  pAprPkt       EOS command from the client
   @param[out] pClientInfo   Elite internal EOS data structure.

   @return
   None.

   @dependencies
   None.
   */
   static inline void elite_msg_extract_client_info( elite_apr_packet_t *pAprPkt,
      elite_msg_data_eos_info_t *pClientInfo)
   {
      pClientInfo->self_port     = elite_apr_if_get_dst_port(pAprPkt);
      pClientInfo->self_addr     = elite_apr_if_get_dst_addr(pAprPkt);
      pClientInfo->client_port   = elite_apr_if_get_src_port(pAprPkt);
      pClientInfo->client_addr   = elite_apr_if_get_src_addr(pAprPkt);
      pClientInfo->client_token  = elite_apr_if_get_client_token(pAprPkt);
   }


/** @addtogroup elite_msg_sec_opcodes
@{ */
/** Definition of the invalid PCM channel. */
#define PCM_CHANNEL_INVALID 0

typedef struct elite_multi_channel_pcm_fmt_blk_t elite_multi_channel_pcm_fmt_blk_t;

/** Payload of the #ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM type.
*/
struct elite_multi_channel_pcm_fmt_blk_t
{
    uint16_t  num_channels;
    /**< Number of channels.

         Supported values: 1 to 8 */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.          */

    uint32_t  sample_rate;
    /**< Number of samples per second (in Hertz).          */

    uint16_t  is_signed;
    /**< Flag that indicates the samples are signed (1). */

    uint16_t  is_interleaved;
    /**< 1 - if data is interleaved, 0 - deinterleaved. */

    uint8_t   channel_mapping[8];
    /**< Channel array of size 8.

         Channel[i] mapping describes channel I. Each element i of the array
         describes channel I inside the buffer where 0 @le I < num_channels.
         An unused channel is set to zero. */
};
/** @} */ /* end_addtogroup elite_msg_sec_opcodes */

typedef struct elite_compressed_fmt_blk_t elite_compressed_fmt_blk_t;
struct elite_compressed_fmt_blk_t
{
   uint16_t  num_channels;
   /**< Number of channels.

      Supported values: 1 to 8 */

   uint16_t  bits_per_sample;
   /**< Number of bits per sample per channel.          */

   uint32_t  sample_rate;
   /**< Number of samples per second (in Hertz).          */

   uint32_t  media_format;
   /**< Format of the compressed data. Uses the ASM_MEDIA_FMT* definitions in adsp_media_fmt.h */
};

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITEMSG_DATA_H

