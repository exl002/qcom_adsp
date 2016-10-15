#ifndef DIAGCOMM_CTRL_H 
#define DIAGCOMM_CTRL_H
/*==========================================================================

              Diagnostic Communications for Diag Control Channel

General Description
  
Copyright (c) 2010-2011 by Qualcomm Technologies, Incorporated.
Copyright (c) 2010-2012 by Qualcomm Technologies, Incorporated.
Copyright (c) 2010-2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.adsp/2.2/services/diag/DCM/common/src/diagcomm_ctrl.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/11/13   sr      Mainling Diag central routing  
11/15/12   is      Support for preset masks
08/29/12   sa      Added max supported tx/rx chained packet size
03/02/12   sg      Added function to return the channel connected status
                   for the ctrl port
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/11/11   wjg     Added function to handle processing DIAG_CTRL_MSG_SIG 
04/18/11   is      Ensure CTRL msg sizes are constant
03/04/11   is      Support for Diag over SMD-Lite
12/12/10   mad     Added DIAG_CTRL_MSG_DIAGDATA and associated structures for dual mask.
                   Added diagcomm_ctrl_set_port_id().
10/12/10   mad     Changed diag_ctrl_msg_diagmode_type structure 
09/21/10   mad     Added message to control diag sleep-vote, buffering 
                   modality etc:  
09/14/10   is      New control channel and central routing features 

===========================================================================*/

#include "comdef.h"
#include "diagcomm_io.h"     /* For diagcomm_io_conn_type, diagcomm_enum_port_type */
#include "diaglogi.h"        /* For EQUIP_ID_MAX */


/* ------------------------------------------------------------------------
** Typedefs and defines
** ------------------------------------------------------------------------ */

#define DIAG_CTRL_MSG_REG            1   // Message passing for registration commands
#define DIAG_CTRL_MSG_DTR            2   // Message passing for DTR events
#define DIAG_CTRL_MSG_DIAGMODE       3   // Control Diag sleep vote, buffering etc
#define DIAG_CTRL_MSG_DIAGDATA       4   // Diag data based on "light" diag mask
#define DIAG_CTRL_MSG_LOG_MASK       5   // Deprecated
#define DIAG_CTRL_MSG_EVENT_MASK     6   // Deprecated
#define DIAG_CTRL_MSG_F3_MASK        7   // Deprecated
#define DIAG_CTRL_MSG_FEATURE        8   // Send diag internal feature mask 'diag_int_feature_mask'
#define DIAG_CTRL_MSG_EQUIP_LOG_MASK 9   // Send Diag log mask for a particular equip id (Still sent by HLOS Apps)
#define DIAG_CTRL_MSG_EVENT_MASK_V2  10  // Send Diag event mask (Still sent by HLOS Apps)
#define DIAG_CTRL_MSG_F3_MASK_V2     11  // Send Diag F3 mask (Still sent by HLOS Apps)
#define DIAG_CTRL_MSG_NUM_PRESETS    12  // Send # of preset masks supported
#define DIAG_CTRL_MSG_SET_CUR_PRESET 13  // Send current preset index to use
#define DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3 14  // Upgrade of DIAG_CTRL_MSG_EQUIP_LOG_MASK supporting presets
#define DIAG_CTRL_MSG_EVENT_MASK_V3  15  // Upgrade of DIAG_CTRL_MSG_EVENT_MASK_V2 supporting presets 
#define DIAG_CTRL_MSG_F3_MASK_V3     16  // Upgrade of DIAG_CTRL_MSG_F3_MASK_V2 supporting presets

#define DIAG_CTRL_MSG_REG_VER        1   // Version # for CTRL_MSG_REG
#define DIAG_CTRL_MSG_DTR_VER        1   // Version # for CTRL_MSG_DTR
#define DIAG_CTRL_MSG_DIAGMODE_VER   1   // Version # for DIAG_CTRL_MSG_MODE
#define DIAG_CTRL_MSG_DIAGDATA_VER   1   // Version # for DIAG_CTRL_MSG_DIAGDATA


#define DIAG_CTRL_MASK_INVALID            0
#define DIAG_CTRL_MASK_ALL_DISABLED       1
#define DIAG_CTRL_MASK_ALL_ENABLED        2
#define DIAG_CTRL_MASK_VALID              3


/* Max Chained Number of DSM Item */
#define DIAG_MAX_CHAINED_DSM_ITEM         3

/* Rx/Tx Control Packet Max DSM Chained Size*/
#if defined(DIAG_MP_MASTER)
  #define DIAG_CTRL_PKT_RX_MAX_CHAIN_SIZ  (DSMI_DIAG_SMD_CTRL_RX_ITEM_SIZ * DIAG_MAX_CHAINED_DSM_ITEM)
  #define DIAG_CTRL_PKT_TX_MAX_CHAIN_SIZ  (DSMI_DIAG_SMD_CTRL_RX_ITEM_SIZ * DIAG_MAX_CHAINED_DSM_ITEM)
#else
  #define DIAG_CTRL_PKT_RX_MAX_CHAIN_SIZ  (DSMI_DIAG_SIO_CTRL_RX_ITEM_SIZ * DIAG_MAX_CHAINED_DSM_ITEM)
  #define DIAG_CTRL_PKT_TX_MAX_CHAIN_SIZ  (DSMI_DIAG_SIO_CTRL_RX_ITEM_SIZ * DIAG_MAX_CHAINED_DSM_ITEM)
#endif

	



typedef PACK(struct)
{
  word cmd_code_lo;                   // Minimum Command code value
  word cmd_code_hi;                   // Maximum Command code value
} diag_ctrl_msg_user_table_entry_type;

typedef PACK(struct)
{
  uint32 version;                      // Version # of this msg type
  uint16 cmd_code;                     // The cmd code
  word subsysid;                       // The subsystem ID
  word count;                          // # of cmd entries
  uint16 port;                         // The smd port (ie: remote proc) to route request to
} diag_ctrl_msg_reg_type;

typedef PACK(struct)
{
  uint32 version;                      // Version # of this msg type
  boolean dte_ready_asserted;          // DTR ready state
} diag_ctrl_msg_dtr_type;

typedef PACK(struct)
{
   uint32 version;                     /* Version # of this msg type */
   unsigned int sleep_vote;            /* Set to zero to disable diag sleep voting, 1 to enable diag sleep voting */
   unsigned int real_time;             /* Set to zero to disable real-time logging (i.e. prevent frequent APPS wake-ups etc),
                                          1 to enable real-time logging */
   unsigned int use_nrt_values;        /* Can be used for testing. If set, the values below are used to tune diag. 
                                          If zero, those values are ignored and diag chooses its own parameters */
   unsigned int commit_threshold;
   unsigned int sleep_threshold; 
   unsigned int sleep_time; 
   unsigned int drain_timer_val; 
   unsigned int event_stale_timer_val; 
} diag_ctrl_msg_diagmode_type;

typedef PACK(struct)
{
   uint32 version;                     /* Version # of this message */
   uint32 ctrl_data_stream_num;        /* if later we need more than one control data stream */
} diag_ctrl_msg_diagdata_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint32 log_mask_size;               /* LOG_MASK_SIZE */
  byte log_mask[LOG_MASK_SIZE];       /* The log mask */
} diag_ctrl_msg_log_mask_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 status;
  uint8 event_config;                                 /* Event reporting config indicator */
  uint32 event_mask_size;                             /* EVENT_MASK_SIZE */
  //unsigned char event_mask[EVENT_MASK_SIZE];        /* The event mask */
} diag_ctrl_msg_event_mask_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 preset_id;
  uint8 status;
  uint8 event_config;                                 /* Event reporting config indicator */
  uint32 event_mask_size;                             /* EVENT_MASK_SIZE */
  //unsigned char event_mask[EVENT_MASK_SIZE];        /* The event mask */
} diag_ctrl_msg_event_mask_v3_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 status;
  uint8 msg_mode;                                      
  uint16 ssid_first;                                  /* Start of range of supported SSIDs */
  uint16 ssid_last;                                   /* Last SSID in range */
  uint32 msg_mask_size;                               /* Count of rt_mask_array (ssid_last - ssid_first + 1) */
  //uint32 *rt_mask_array;                            /* Array of (ssid_last - ssid_first + 1) masks */
} diag_ctrl_msg_f3_mask_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 preset_id;
  uint8 status;
  uint8 msg_mode;                                      
  uint16 ssid_first;                                  /* Start of range of supported SSIDs */
  uint16 ssid_last;                                   /* Last SSID in range */
  uint32 msg_mask_size;                               /* Count of rt_mask_array (ssid_last - ssid_first + 1) */
  //uint32 *rt_mask_array;                            /* Array of (ssid_last - ssid_first + 1) masks */
} diag_ctrl_msg_f3_mask_v3_type;

typedef PACK(struct)
{
  uint32 mask_len;                                   /* Length of 'mask' (DIAG_INT_FEATURE_MASK_LEN) */
  byte mask[DIAG_INT_FEATURE_MASK_LEN];              /* Diag internal feature mask */
} diag_ctrl_msg_feature_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 status;
  uint8 equip_id;
  uint32 last_item;                   /* Last log code for this equip_id */
  uint32 log_mask_size;               /* Size of log mask stored in log_mask[] */
  //byte log_mask[log_mask_size];     /* The log mask for equip_id */
} diag_ctrl_msg_equip_log_mask_type;

typedef PACK(struct)
{
  uint8 stream_id;
  uint8 preset_id;
  uint8 status;
  uint8 equip_id;
  uint32 last_item;                   /* Last log code for this equip_id */
  uint32 log_mask_size;               /* Size of log mask stored in log_mask[] */
  //byte log_mask[log_mask_size];     /* The log mask for equip_id */
} diag_ctrl_msg_equip_log_mask_v3_type;

typedef PACK(struct)
{
  uint8 num_presets;
} diag_ctrl_msg_num_presets_type;

typedef PACK(struct)
{
  uint8 preset_id;
} diag_ctrl_msg_cur_preset_type;

typedef PACK(union)
{
  diag_ctrl_msg_reg_type ctrl_msg_reg;                             /* DIAG_CTRL_MSG_REG */
  diag_ctrl_msg_dtr_type ctrl_msg_dtr;                             /* DIAG_CTRL_MSG_DTR */
  diag_ctrl_msg_diagmode_type ctrl_msg_diagmode;                   /* DIAG_CTRL_MSG_DIAGMODE */
  diag_ctrl_msg_diagdata_type ctrl_msg_diagdata;                   /* DIAG_CTRL_MSG_DIAGDATA */
  diag_ctrl_msg_event_mask_type ctrl_msg_event_mask;               /* DIAG_CTRL_MSG_EVENT_MASK_V2 */
  diag_ctrl_msg_f3_mask_type ctrl_msg_f3_mask;                     /* DIAG_CTRL_MSG_F3_MASK_V2 */
  diag_ctrl_msg_feature_type ctrl_msg_feature;                     /* DIAG_CTRL_MSG_FEATURE */
  diag_ctrl_msg_equip_log_mask_type ctrl_msg_equip_log_mask;       /* DIAG_CTRL_MSG_EQUIP_LOG_MASK */
  diag_ctrl_msg_num_presets_type ctrl_msg_num_presets;             /* DIAG_CTRL_MSG_NUM_PRESETS */
  diag_ctrl_msg_cur_preset_type cur_preset;                        /* DIAG_CTRL_MSG_SET_CUR_PRESET */
  diag_ctrl_msg_event_mask_v3_type ctrl_msg_event_mask_v3;         /* DIAG_CTRL_MSG_EVENT_MASK_V3 */
  diag_ctrl_msg_equip_log_mask_v3_type ctrl_msg_equip_log_mask_v3; /* DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3 */
  diag_ctrl_msg_f3_mask_v3_type ctrl_msg_f3_mask_v3;               /* DIAG_CTRL_MSG_F3_MASK_V3 */
} diag_ctrl_msg_data_type;

#define DIAGCOMM_CTRL_MSG_HDR_SIZE 8   /* Size of cmd_type + data_len in bytes */
typedef PACK(struct)
{
  uint32 cmd_type;
  uint32 data_len;
  diag_ctrl_msg_data_type data;
} diag_ctrl_msg_type;

/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */

/* Generic - CTRL specific */
uint16 diagcomm_ctrl_port_id( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
boolean diagcomm_ctrl_status( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
boolean diagcomm_ctrl_channel_status( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
boolean diagcomm_ctrl_open( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
void diagcomm_ctrl_close( diagcomm_port_type port_type, diagcomm_enum_port_type ctrl_channel );
void diagcomm_ctrl_init( diagcomm_port_type port_type, diagcomm_enum_port_type port_num );

#if defined(DIAG_MP_MASTER)
dword diagcomm_ctrl_clear_mask_status( diagcomm_port_type port_type, 
                                       diagcomm_enum_port_type port_num,
                                       dword mask, 
                                       uint8 stream_id,
                                       uint8 preset_id );
                                 
boolean diagcomm_ctrl_mask_status( diagcomm_port_type port_type, 
                                   diagcomm_enum_port_type port_num,
                                   dword mask,
                                   uint8 stream_id,
                                   uint8 preset_id );
                                  
boolean diagcomm_ctrl_msg_mask_status( diagcomm_port_type port_type, 
                                       diagcomm_enum_port_type port_num,
                                       uint8 stream_id,
                                       uint8 preset_id,
                                       uint8 index );
                                     
void diagcomm_ctrl_clear_msg_mask_status( diagcomm_port_type port_type, 
                                          diagcomm_enum_port_type port_num,
                                          uint8 stream_id,
                                          uint8 preset_id );
#endif
                                        
/* SIO/SMD specific */
#ifdef DIAG_SIO_SUPPORT
void diagcomm_ctrl_open_init( diagcomm_io_conn_type * conn );
#endif /* DIAG_SIO_SUPPORT */

#ifdef DIAG_SMD_SUPPORT
void diagcomm_process_ctrl( void );
#endif /* DIAG_SMD_SUPPORT */

void diagcomm_ctrl_send_feature_mask( void );
void diagcomm_ctrl_send_num_presets( void );
dsm_item_type * diagcomm_new_dsm( dsm_mempool_id_type pool_id, boolean priority );
uint32 diagcomm_ctrl_send_mult_pkt( PACK(void *) ctrl_pkt, uint32 ctrl_pktlen, 
                                    PACK(const void *) user_pkt, uint32 user_pktlen,
                                    diagcomm_port_type port_type,
                                    diagcomm_enum_port_type ctrl_channel, 
                                    boolean send_now, boolean * is_sent );

void diagpkt_process_ctrl_msg( PACK(void *)ctrl_pkt, uint32 ctrl_pkt_len,
                               diagcomm_port_type port_type,
                               diagcomm_enum_port_type ctrl_channel );
                               
#endif //DIAGCOMM_CTRL_H
