/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
        DIAGNOSTIC MESSAGE SERVICE: ARRAYS FOR BUILD/RUN-TIME MASKS


GENERAL DESCRIPTION
   Contains array definitions for build-time and run-time masks.
  
Copyright (c) 2010-11 by Qualcomm Technologies, Incorporated.
Copyright (c) 2009-2013 by Qualcomm Technologies, Incorporated.
Copyright (c) 2010-13 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

  $Header: //components/rel/core.adsp/2.2/services/diag/diag_mask_tbl/src/msg_arrays_i.c#4 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/25/13   xy      Added new message SSIDs 
08/01/13   sr      Added new message SSIDs  
04/01/13   sr      Added new message SSIDs 
11/15/12   is      Support for preset masks
06/22/12   rh      Added new message SSIDs 
02/29/12   rh      Added new message SSIDs 
02/17/12   rs      Added build mask for MSG_SSID_SEC_WIDEVINE 
01/05/12   rh      Added QCNEA SSIDs  
12/08/11   rh      Added build mask for MSG_SSID_ADC 
11/29/11   rh      Added SSID category for CTA 
10/18/11   hm      Renamed reserved MCS SSIDs 
09/01/11   hm      Added new WCDMA and TDSCDMA SSIDs 
08/05/11   hm      Added new SSID 
07/01/11   hm      Added new SSID 
04/25/11   hm      Added new SSID 
04/05/11   hm      Added new SSID for PPM module 
03/24/11   hm      Reverted Octopus Changes and added new QCHAT SSIDs 
03/07/11   hm      Added new set of SSIDs for Octopus 
12/21/10   hm      Dual Mask Changes 
12/15/10   vg      Added GPS SSID and LTE Events 
12/13/10   vg      Added 2 New SSIDs 
12/06/10   vg      Added PMIC API  
07/27/10   sg      Added new SSID for multimedia
07/07/10   sg      Added new SSID for data services 
07/06/10   sg      Changed MSG_SSID_MCS_RESERVED_1 to  MSG_SSID_FWS
05/05/10   sg      Added new SSID MSG_SSID_CFM
04/27/10   sg      Added new SSIDS for Sound Routing Driver , DAL
04/20/10   sg      Added new SSIDS for Audio Team
04/09/10   sg      Added new SSIDS for OMA device management 
                   Secure Instant Wireless Access 
03/04/10   sg      Added new SSIDs for IMS team
02/22/10   sg      Added new SSIDs for Data Services
02/08/10   sg      Added new SSIDs for IMS team
01/13/10   sg      Added new SSID for CAD team
12/22/09   sg      Added New SSID for Connectivity Engine team
10/28/09   sg      Added New SSID for Chaos CoOrdinator Service
10/22/09   sg      Added New SSID for  MSG_BUILD_MASK_MSG_SSID_DS_MUX
10/06/09   sg      Added  msg_mask_tbl_size to support compilation with FTM
                   on older targets
09/29/09   mad     Created from msgcfg.h. Moved all mask array definitions
                   here.
===========================================================================*/

#include "msg.h"
#include "msg_arrays_i.h"

/* This table must be maintained as SSIDs are added!
   NOTE: The legacy build mask is handled on a per-file basis. */
static const uint32 msg_bld_masks_gen[] = 
{
  MSG_LVL_LOW, /* special case - may vary by file is MSG_LEVEL is used */
  MSG_BUILD_MASK_MSG_SSID_AUDFMT,
  MSG_BUILD_MASK_MSG_SSID_AVS,
  MSG_BUILD_MASK_MSG_SSID_BOOT,
  MSG_BUILD_MASK_MSG_SSID_BT,
  MSG_BUILD_MASK_MSG_SSID_CM,
  MSG_BUILD_MASK_MSG_SSID_CMX,
  MSG_BUILD_MASK_MSG_SSID_DIAG,
  MSG_BUILD_MASK_MSG_SSID_DSM,
  MSG_BUILD_MASK_MSG_SSID_FS,
  MSG_BUILD_MASK_MSG_SSID_HS,
  MSG_BUILD_MASK_MSG_SSID_MDSP,
  MSG_BUILD_MASK_MSG_SSID_QDSP,
  MSG_BUILD_MASK_MSG_SSID_REX,
  MSG_BUILD_MASK_MSG_SSID_RF,
  MSG_BUILD_MASK_MSG_SSID_SD,
  MSG_BUILD_MASK_MSG_SSID_SIO,
  MSG_BUILD_MASK_MSG_SSID_VS,     
  MSG_BUILD_MASK_MSG_SSID_WMS,
  MSG_BUILD_MASK_MSG_SSID_GPS,
  MSG_BUILD_MASK_MSG_SSID_MMOC,
  MSG_BUILD_MASK_MSG_SSID_RUIM,
  MSG_BUILD_MASK_MSG_SSID_TMC,
  MSG_BUILD_MASK_MSG_SSID_FTM,
  MSG_BUILD_MASK_MSG_SSID_MMGPS,
  MSG_BUILD_MASK_MSG_SSID_SLEEP,
  MSG_BUILD_MASK_MSG_SSID_SAM,
  MSG_BUILD_MASK_MSG_SSID_SRM,
  MSG_BUILD_MASK_MSG_SSID_SFAT,
  MSG_BUILD_MASK_MSG_SSID_JOYST,
  MSG_BUILD_MASK_MSG_SSID_MFLO,
  MSG_BUILD_MASK_MSG_SSID_DTV,
  MSG_BUILD_MASK_MSG_SSID_TCXOMGR,
  MSG_BUILD_MASK_MSG_SSID_EFS,
  MSG_BUILD_MASK_MSG_SSID_IRDA,
  MSG_BUILD_MASK_MSG_SSID_FM_RADIO,
  MSG_BUILD_MASK_MSG_SSID_AAM,
  MSG_BUILD_MASK_MSG_SSID_BM,
  MSG_BUILD_MASK_MSG_SSID_PE,
  MSG_BUILD_MASK_MSG_SSID_QIPCALL,
  MSG_BUILD_MASK_MSG_SSID_FLUTE,
  MSG_BUILD_MASK_MSG_SSID_CAMERA,
  MSG_BUILD_MASK_MSG_SSID_HSUSB,
  MSG_BUILD_MASK_MSG_SSID_FC,
  MSG_BUILD_MASK_MSG_SSID_USBHOST,
  MSG_BUILD_MASK_MSG_SSID_PROFILER,
  MSG_BUILD_MASK_MSG_SSID_MGP,
  MSG_BUILD_MASK_MSG_SSID_MGPME,
  MSG_BUILD_MASK_MSG_SSID_GPSOS,
  MSG_BUILD_MASK_MSG_SSID_MGPPE,
  MSG_BUILD_MASK_MSG_SSID_GPSSM,
  MSG_BUILD_MASK_MSG_SSID_IMS,
  MSG_BUILD_MASK_MSG_SSID_MBP_RF,
  MSG_BUILD_MASK_MSG_SSID_SNS,
  MSG_BUILD_MASK_MSG_SSID_WM,
  MSG_BUILD_MASK_MSG_SSID_LK,
  MSG_BUILD_MASK_MSG_SSID_PWRDB,
  MSG_BUILD_MASK_MSG_SSID_DCVS,
  MSG_BUILD_MASK_MSG_SSID_ANDROID_ADB,
  MSG_BUILD_MASK_MSG_SSID_VIDEO_ENCODER,
  MSG_BUILD_MASK_MSG_SSID_VENC_OMX,
  MSG_BUILD_MASK_MSG_SSID_GAN,
  MSG_BUILD_MASK_MSG_SSID_KINETO_GAN,
  MSG_BUILD_MASK_MSG_SSID_ANDROID_QCRIL,
  MSG_BUILD_MASK_MSG_SSID_A2,
  MSG_BUILD_MASK_MSG_SSID_LINUX_DATA,
  MSG_BUILD_MASK_MSG_SSID_ECALL,
  MSG_BUILD_MASK_MSG_SSID_CHORD,
  MSG_BUILD_MASK_MSG_SSID_QCNE,
  MSG_BUILD_MASK_MSG_SSID_APPS_CAD_GENERAL,
  MSG_BUILD_MASK_MSG_SSID_OMADM,      /* OMA device management */
  MSG_BUILD_MASK_MSG_SSID_SIWA,       /* Secure Instant Wireless Access */
  MSG_BUILD_MASK_MSG_SSID_APR_MODEM,  /* Audio Packet Router Modem */
  MSG_BUILD_MASK_MSG_SSID_APR_APPS,   /* Audio Packet Router Apps*/
  MSG_BUILD_MASK_MSG_SSID_APR_ADSP,   /* Audio Packet Router Adsp*/
  MSG_BUILD_MASK_MSG_SSID_SRD_GENERAL ,
  MSG_BUILD_MASK_MSG_SSID_ACDB_GENERAL ,
  MSG_BUILD_MASK_MSG_SSID_DALTF,
  MSG_BUILD_MASK_MSG_SSID_CFM,
  MSG_BUILD_MASK_MSG_SSID_PMIC,
  MSG_BUILD_MASK_MSG_SSID_GPS_SDP,
  MSG_BUILD_MASK_MSG_SSID_TLE,
  MSG_BUILD_MASK_MSG_SSID_TLE_XTM,
  MSG_BUILD_MASK_MSG_SSID_TLE_TLM,
  MSG_BUILD_MASK_MSG_SSID_TLE_TLM_MM,
  MSG_BUILD_MASK_MSG_SSID_WWAN_LOC,
  MSG_BUILD_MASK_MSG_SSID_GNSS_LOCMW,
  MSG_BUILD_MASK_MSG_SSID_QSET,
  MSG_BUILD_MASK_MSG_SSID_QBI,
  MSG_BUILD_MASK_MSG_SSID_ADC,
  MSG_BUILD_MASK_MSG_SSID_MMODE_QMI,
  MSG_BUILD_MASK_MSG_SSID_MCFG,
  MSG_BUILD_MASK_MSG_SSID_SSM,
  MSG_BUILD_MASK_MSG_SSID_MPOWER,
  MSG_BUILD_MASK_MSG_SSID_RMTS,
  MSG_BUILD_MASK_MSG_SSID_ADIE,
  MSG_BUILD_MASK_MSG_SSID_VT_VCEL,
  MSG_BUILD_MASK_MSG_SSID_FLASH_SCRUB,
  MSG_BUILD_MASK_MSG_SSID_STRIDE,
  MSG_BUILD_MASK_MSG_SSID_POLICYMAN,
  MSG_BUILD_MASK_MSG_SSID_TMS,
  MSG_BUILD_MASK_MSG_SSID_LWIP
/* EXAMPLE, placing build mask in table for msg.c.  All build masks not defined
   in diagtgt.h will have a table entry that is the same as msg.c. */
#if 0
  MSG_BUILD_MASK_MSG_SSID_FOO
#endif
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_gen[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_gen) / sizeof(msg_bld_masks_gen[0])];
static uint32 msg_rt_masks_dci_gen[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_gen) / sizeof(msg_bld_masks_gen[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_oncrpc[] = 
{
  MSG_BUILD_MASK_MSG_SSID_ONCRPC,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_MODEM,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_MISC_APPS,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_MODEM,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_CM_APPS,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_DB,
  MSG_BUILD_MASK_MSG_SSID_ONCRPC_SND,
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_oncrpc[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_oncrpc) / sizeof(msg_bld_masks_oncrpc[0])];
static uint32 msg_rt_masks_dci_oncrpc[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_oncrpc) / sizeof(msg_bld_masks_oncrpc[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_1X[] = 
{
  MSG_BUILD_MASK_MSG_SSID_1X,
  MSG_BUILD_MASK_MSG_SSID_1X_ACP,
  MSG_BUILD_MASK_MSG_SSID_1X_DCP,
  MSG_BUILD_MASK_MSG_SSID_1X_DEC,
  MSG_BUILD_MASK_MSG_SSID_1X_ENC,
  MSG_BUILD_MASK_MSG_SSID_1X_GPSSRCH,
  MSG_BUILD_MASK_MSG_SSID_1X_MUX,
  MSG_BUILD_MASK_MSG_SSID_1X_SRCH
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_1X[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_1X) / sizeof(msg_bld_masks_1X[0])];
static uint32 msg_rt_masks_dci_1X[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_1X) / sizeof(msg_bld_masks_1X[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_hdr[] = 
{
  MSG_BUILD_MASK_MSG_SSID_HDR_PROT,
  MSG_BUILD_MASK_MSG_SSID_HDR_DATA,
  MSG_BUILD_MASK_MSG_SSID_HDR_SRCH,
  MSG_BUILD_MASK_MSG_SSID_HDR_DRIVERS,
  MSG_BUILD_MASK_MSG_SSID_HDR_IS890,
  MSG_BUILD_MASK_MSG_SSID_HDR_DEBUG,
  MSG_BUILD_MASK_MSG_SSID_HDR_HIT,
  MSG_BUILD_MASK_MSG_SSID_HDR_PCP,
  MSG_BUILD_MASK_MSG_SSID_HDR_HEAPMEM
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_hdr[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_hdr) / sizeof(msg_bld_masks_hdr[0])];
static uint32 msg_rt_masks_dci_hdr[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_hdr) / sizeof(msg_bld_masks_hdr[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_umts[] = 
{
  MSG_BUILD_MASK_MSG_SSID_UMTS,
  MSG_BUILD_MASK_MSG_SSID_WCDMA_L1,
  MSG_BUILD_MASK_MSG_SSID_WCDMA_L2,
  MSG_BUILD_MASK_MSG_SSID_WCDMA_MAC,
  MSG_BUILD_MASK_MSG_SSID_WCDMA_RLC,
  MSG_BUILD_MASK_MSG_SSID_WCDMA_RRC,
  MSG_BUILD_MASK_MSG_SSID_NAS_CNM,
  MSG_BUILD_MASK_MSG_SSID_NAS_MM,
  MSG_BUILD_MASK_MSG_SSID_NAS_MN,
  MSG_BUILD_MASK_MSG_SSID_NAS_RABM,
  MSG_BUILD_MASK_MSG_SSID_NAS_REG,
  MSG_BUILD_MASK_MSG_SSID_NAS_SM,
  MSG_BUILD_MASK_MSG_SSID_NAS_TC,
  MSG_BUILD_MASK_MSG_SSID_NAS_CB,
  MSG_BUILD_MASK_MSG_SSID_WCDMA_LEVEL
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_umts[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_umts) / sizeof(msg_bld_masks_umts[0])];
static uint32 msg_rt_masks_dci_umts[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_umts) / sizeof(msg_bld_masks_umts[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_gsm[] = 
{
  MSG_BUILD_MASK_MSG_SSID_GSM,
  MSG_BUILD_MASK_MSG_SSID_GSM_L1,
  MSG_BUILD_MASK_MSG_SSID_GSM_L2,
  MSG_BUILD_MASK_MSG_SSID_GSM_RR,
  MSG_BUILD_MASK_MSG_SSID_GSM_GPRS_GCOMMON,
  MSG_BUILD_MASK_MSG_SSID_GSM_GPRS_GLLC,
  MSG_BUILD_MASK_MSG_SSID_GSM_GPRS_GMAC,
  MSG_BUILD_MASK_MSG_SSID_GSM_GPRS_GPL1,
  MSG_BUILD_MASK_MSG_SSID_GSM_GPRS_GRLC,
  MSG_BUILD_MASK_MSG_SSID_GSM_GPRS_GRR,
  MSG_BUILD_MASK_MSG_SSID_GSM_GPRS_GSNDCP
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_gsm[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_gsm) / sizeof(msg_bld_masks_gsm[0])];
static uint32 msg_rt_masks_dci_gsm[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_gsm) / sizeof(msg_bld_masks_gsm[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_wlan[] = 
{
  MSG_BUILD_MASK_MSG_SSID_WLAN,
  MSG_BUILD_MASK_MSG_SSID_WLAN_ADP,
  MSG_BUILD_MASK_MSG_SSID_WLAN_CP,
  MSG_BUILD_MASK_MSG_SSID_WLAN_FTM,
  MSG_BUILD_MASK_MSG_SSID_WLAN_OEM,
  MSG_BUILD_MASK_MSG_SSID_WLAN_SEC,
  MSG_BUILD_MASK_MSG_SSID_WLAN_TRP,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_1,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_2,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_3,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_4,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_5,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_6,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_7,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_8,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_9,
  MSG_BUILD_MASK_MSG_SSID_WLAN_RESERVED_10,
  MSG_BUILD_MASK_MSG_SSID_WLAN_TL,
  MSG_BUILD_MASK_MSG_SSID_WLAN_BAL,
  MSG_BUILD_MASK_MSG_SSID_WLAN_SAL,
  MSG_BUILD_MASK_MSG_SSID_WLAN_SSC,
  MSG_BUILD_MASK_MSG_SSID_WLAN_HDD,
  MSG_BUILD_MASK_MSG_SSID_WLAN_SME,
  MSG_BUILD_MASK_MSG_SSID_WLAN_PE,
  MSG_BUILD_MASK_MSG_SSID_WLAN_HAL,
  MSG_BUILD_MASK_MSG_SSID_WLAN_SYS,
  MSG_BUILD_MASK_MSG_SSID_WLAN_VOSS 
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_wlan[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_wlan) / sizeof(msg_bld_masks_wlan[0])];
static uint32 msg_rt_masks_dci_wlan[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_wlan) / sizeof(msg_bld_masks_wlan[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_mcs[] = 
{
  MSG_BUILD_MASK_MSG_SSID_ATS,           
  MSG_BUILD_MASK_MSG_SSID_MSGR,          
  MSG_BUILD_MASK_MSG_SSID_APPMGR,        
  MSG_BUILD_MASK_MSG_SSID_QTF,           
  MSG_BUILD_MASK_MSG_SSID_FWS,
  MSG_BUILD_MASK_MSG_SSID_SRCH4,
  MSG_BUILD_MASK_MSG_SSID_CMAPI,
  MSG_BUILD_MASK_MSG_SSID_MMAL,
  MSG_BUILD_MASK_MSG_SSID_MCS_RESERVED_5,
  MSG_BUILD_MASK_MSG_SSID_MCS_RESERVED_6,
  MSG_BUILD_MASK_MSG_SSID_MCS_RESERVED_7,
  MSG_BUILD_MASK_MSG_SSID_MCS_RESERVED_8,
  MSG_BUILD_MASK_MSG_SSID_IRATMAN,
  MSG_BUILD_MASK_MSG_SSID_CXM,
  MSG_BUILD_MASK_MSG_SSID_VSTMR
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_mcs[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_mcs) / sizeof(msg_bld_masks_mcs[0])];
static uint32 msg_rt_masks_dci_mcs[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_mcs) / sizeof(msg_bld_masks_mcs[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_ds[] = 
{
  MSG_BUILD_MASK_MSG_SSID_DS,
  MSG_BUILD_MASK_MSG_SSID_DS_RLP,
  MSG_BUILD_MASK_MSG_SSID_DS_PPP,
  MSG_BUILD_MASK_MSG_SSID_DS_TCPIP,
  MSG_BUILD_MASK_MSG_SSID_DS_IS707,
  MSG_BUILD_MASK_MSG_SSID_DS_3GMGR,
  MSG_BUILD_MASK_MSG_SSID_DS_PS,
  MSG_BUILD_MASK_MSG_SSID_DS_MIP,
  MSG_BUILD_MASK_MSG_SSID_DS_UMTS,
  MSG_BUILD_MASK_MSG_SSID_DS_GPRS,
  MSG_BUILD_MASK_MSG_SSID_DS_GSM,
  MSG_BUILD_MASK_MSG_SSID_DS_SOCKETS,
  MSG_BUILD_MASK_MSG_SSID_DS_ATCOP,
  MSG_BUILD_MASK_MSG_SSID_DS_SIO,
  MSG_BUILD_MASK_MSG_SSID_DS_BCMCS,
  MSG_BUILD_MASK_MSG_SSID_DS_MLRLP,
  MSG_BUILD_MASK_MSG_SSID_DS_RTP,
  MSG_BUILD_MASK_MSG_SSID_DS_SIPSTACK,
  MSG_BUILD_MASK_MSG_SSID_DS_ROHC,
  MSG_BUILD_MASK_MSG_SSID_DS_DOQOS,
  MSG_BUILD_MASK_MSG_SSID_DS_IPC,
  MSG_BUILD_MASK_MSG_SSID_DS_SHIM,
  MSG_BUILD_MASK_MSG_SSID_DS_ACLPOLICY,
  MSG_BUILD_MASK_MSG_SSID_DS_APPS,
  MSG_BUILD_MASK_MSG_SSID_DS_MUX,
  MSG_BUILD_MASK_MSG_SSID_DS_3GPP,
  MSG_BUILD_MASK_MSG_SSID_DS_LTE,
  MSG_BUILD_MASK_MSG_SSID_DS_WCDMA,
  MSG_BUILD_MASK_MSG_SSID_DS_ACLPOLICY_APPS,
  MSG_BUILD_MASK_MSG_SSID_DS_HDR,
  MSG_BUILD_MASK_MSG_SSID_DS_IPA
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_ds[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_ds) / sizeof(msg_bld_masks_ds[0])];
static uint32 msg_rt_masks_dci_ds[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_ds) / sizeof(msg_bld_masks_ds[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_sec[] = 
{
  MSG_BUILD_MASK_MSG_SSID_SEC,
  MSG_BUILD_MASK_MSG_SSID_SEC_CRYPTO,
  MSG_BUILD_MASK_MSG_SSID_SEC_SSL,
  MSG_BUILD_MASK_MSG_SSID_SEC_IPSEC,
  MSG_BUILD_MASK_MSG_SSID_SEC_SFS,
  MSG_BUILD_MASK_MSG_SSID_SEC_TEST,
  MSG_BUILD_MASK_MSG_SSID_SEC_CNTAGENT,
  MSG_BUILD_MASK_MSG_SSID_SEC_RIGHTSMGR,
  MSG_BUILD_MASK_MSG_SSID_SEC_ROAP,
  MSG_BUILD_MASK_MSG_SSID_SEC_MEDIAMGR,
  MSG_BUILD_MASK_MSG_SSID_SEC_IDSTORE,
  MSG_BUILD_MASK_MSG_SSID_SEC_IXFILE,
  MSG_BUILD_MASK_MSG_SSID_SEC_IXSQL,
  MSG_BUILD_MASK_MSG_SSID_SEC_IXCOMMON,
  MSG_BUILD_MASK_MSG_SSID_SEC_BCASTCNTAGENT,
  MSG_BUILD_MASK_MSG_SSID_SEC_PLAYREADY,
  MSG_BUILD_MASK_MSG_SSID_SEC_WIDEVINE,
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_sec[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_sec) / sizeof(msg_bld_masks_sec[0])];
static uint32 msg_rt_masks_dci_sec[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_sec) / sizeof(msg_bld_masks_sec[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_apps[] = 
{
  MSG_BUILD_MASK_MSG_SSID_APPS,
  MSG_BUILD_MASK_MSG_SSID_APPS_APPMGR,
  MSG_BUILD_MASK_MSG_SSID_APPS_UI,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STATISTICS,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VENCODER,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_MODEM,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_UI,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_STACK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QVP_VDECODER,
  MSG_BUILD_MASK_MSG_SSID_APPS_ACM,
  MSG_BUILD_MASK_MSG_SSID_APPS_HEAP_PROFILE,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_GENERAL,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEBUG,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STATISTICS,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_UI_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MP4_PLAYER,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_AUDIO_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_VIDEO_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAMING,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_MPEG4_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_FILE_OPS,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTP,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTCP,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_RTSP,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_SDP_PARSE,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_ATOM_PARSE,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_TEXT_TASK,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_DEC_DSP_IF,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_STREAM_RECORDING,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_CONFIGURATION,
  MSG_BUILD_MASK_MSG_SSID_APPS_QCAMERA,
  MSG_BUILD_MASK_MSG_SSID_APPS_QCAMCORDER,
  MSG_BUILD_MASK_MSG_SSID_APPS_BREW,
  MSG_BUILD_MASK_MSG_SSID_APPS_QDJ,
  MSG_BUILD_MASK_MSG_SSID_APPS_QDTX,
  MSG_BUILD_MASK_MSG_SSID_APPS_QTV_BCAST_FLO,
  MSG_BUILD_MASK_MSG_SSID_APPS_MDP_GENERAL,
  MSG_BUILD_MASK_MSG_SSID_APPS_PBM,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GENERAL,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_EGL,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENGL,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DIRECT3D,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_SVG,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_OPENVG,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_2D,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_QXPROFILER,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_DSP,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_GRP,
  MSG_BUILD_MASK_MSG_SSID_APPS_GRAPHICS_MDP,
  MSG_BUILD_MASK_MSG_SSID_APPS_CAD,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_DPL,               
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_FW,                
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_SIP,               
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_REGMGR,            
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_RTP,               
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_SDP,               
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_VS,                
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_XDM,              
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_HOM,               
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_IM_ENABLER,        
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_IMS_CORE,         
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_FWAPI,             
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_SERVICES,          
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_POLICYMGR,         
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_PRESENCE,          
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_QIPCALL,           
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_SIGCOMP,  
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_PSVT,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_UNKNOWN,  
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_SETTINGS,
  MSG_BUILD_MASK_MSG_SSID_OMX_COMMON,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_RCS_CD,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_RCS_IM,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_RCS_FT,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_RCS_IS,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_RCS_AUTO_CONFIG,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_RCS_COMMON,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_UT,
  MSG_BUILD_MASK_MSG_SSID_APPS_IMS_XML,

};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_apps[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_apps) / sizeof(msg_bld_masks_apps[0])];
static uint32 msg_rt_masks_dci_apps[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_apps) / sizeof(msg_bld_masks_apps[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_adsptasks[] = 
{
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_KERNEL,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AFETASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOICEPROCTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCDECTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VOCENCTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VFETASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_VIDEOENCTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_JPEGTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPPTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY0TASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY1TASK, 
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY2TASK, 
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY3TASK, 
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPLAY4TASK, 
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_LPMTASK,      
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_DIAGTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDRECTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_AUDPREPROCTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_MODMATHTASK,
  MSG_BUILD_MASK_MSG_SSID_ADSPTASKS_GRAPHICSTASK,
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_adsptasks[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_adsptasks) / sizeof(msg_bld_masks_adsptasks[0])];
static uint32 msg_rt_masks_dci_adsptasks[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_adsptasks) / sizeof(msg_bld_masks_adsptasks[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_l4_linux[] = 
{
  MSG_BUILD_MASK_MSG_SSID_L4LINUX_KERNEL,
  MSG_BUILD_MASK_MSG_SSID_L4LINUX_KEYPAD,
  MSG_BUILD_MASK_MSG_SSID_L4LINUX_APPS,
  MSG_BUILD_MASK_MSG_SSID_L4LINUX_QDDAEMON,
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_l4_linux[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_l4_linux) / sizeof(msg_bld_masks_l4_linux[0])];
static uint32 msg_rt_masks_dci_l4_linux[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_l4_linux) / sizeof(msg_bld_masks_l4_linux[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_l4_iguana[] =
{
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_IGUANASERVER,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_EFS2,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_QDMS,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_REX,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SMMS,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_FRAMEBUFFER,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_KEYPAD,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_NAMING,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SDIO,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_SERIAL,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TIMER,
  MSG_BUILD_MASK_MSG_SSID_L4IGUANA_TRAMP,
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_l4_iguana[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_l4_iguana) / sizeof(msg_bld_masks_l4_iguana[0])];
static uint32 msg_rt_masks_dci_l4_iguana[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_l4_iguana) / sizeof(msg_bld_masks_l4_iguana[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_l4_amss[] =
{
  MSG_BUILD_MASK_MSG_SSID_L4AMSS_QDIAG,
  MSG_BUILD_MASK_MSG_SSID_L4AMSS_APS,
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_l4_amss[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_l4_amss) / sizeof(msg_bld_masks_l4_amss[0])];
static uint32 msg_rt_masks_dci_l4_amss[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_l4_amss) / sizeof(msg_bld_masks_l4_amss[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_hit[] = 
{
  MSG_BUILD_MASK_MSG_SSID_HIT
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_hit[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_hit) / sizeof(msg_bld_masks_hit[0])];
static uint32 msg_rt_masks_dci_hit[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_hit) / sizeof(msg_bld_masks_hit[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_qdsp6[] = 
{
  MSG_BUILD_MASK_MSG_SSID_QDSP6,
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_SVC,
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_ENCDEC,
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_VOC,
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_VS,
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_MIDI, 
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_POSTPROC,   
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_PREPROC,    
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_AFE,          
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_MSESSION,      
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_DSESSION,      
  MSG_BUILD_MASK_MSG_SSID_ADSP_AUD_DCM,       
  MSG_BUILD_MASK_MSG_SSID_ADSP_VID_ENC,         
  MSG_BUILD_MASK_MSG_SSID_ADSP_VID_ENCRPC,        
  MSG_BUILD_MASK_MSG_SSID_ADSP_VID_DEC,          
  MSG_BUILD_MASK_MSG_SSID_ADSP_VID_DECRPC,        
  MSG_BUILD_MASK_MSG_SSID_ADSP_VID_COMMONSW,      
  MSG_BUILD_MASK_MSG_SSID_ADSP_VID_HWDRIVER,      
  MSG_BUILD_MASK_MSG_SSID_ADSP_JPG_ENC,           
  MSG_BUILD_MASK_MSG_SSID_ADSP_JPG_DEC,            
  MSG_BUILD_MASK_MSG_SSID_ADSP_OMM,            
  MSG_BUILD_MASK_MSG_SSID_ADSP_PWRDEM,          
  MSG_BUILD_MASK_MSG_SSID_ADSP_RESMGR,          
  MSG_BUILD_MASK_MSG_SSID_ADSP_CORE, 
  MSG_BUILD_MASK_MSG_SSID_ADSP_RDA, 
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_qdsp6[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_qdsp6) / sizeof(msg_bld_masks_qdsp6[0])];
static uint32 msg_rt_masks_dci_qdsp6[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_qdsp6) / sizeof(msg_bld_masks_qdsp6[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_umb[] = 
{
  MSG_BUILD_MASK_MSG_SSID_UMB,
  MSG_BUILD_MASK_MSG_SSID_UMB_APP,
  MSG_BUILD_MASK_MSG_SSID_UMB_DS,
  MSG_BUILD_MASK_MSG_SSID_UMB_CP,
  MSG_BUILD_MASK_MSG_SSID_UMB_RLL,
  MSG_BUILD_MASK_MSG_SSID_UMB_MAC,
  MSG_BUILD_MASK_MSG_SSID_UMB_SRCH,
  MSG_BUILD_MASK_MSG_SSID_UMB_FW,
  MSG_BUILD_MASK_MSG_SSID_UMB_PLT,
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_umb[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_umb) / sizeof(msg_bld_masks_umb[0])];
static uint32 msg_rt_masks_dci_umb[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_umb) / sizeof(msg_bld_masks_umb[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_lte[] = 
{
  MSG_BUILD_MASK_MSG_SSID_LTE,
  MSG_BUILD_MASK_MSG_SSID_LTE_RRC,
  MSG_BUILD_MASK_MSG_SSID_LTE_MACUL,
  MSG_BUILD_MASK_MSG_SSID_LTE_MACDL,
  MSG_BUILD_MASK_MSG_SSID_LTE_MACCTRL,
  MSG_BUILD_MASK_MSG_SSID_LTE_RLCUL,
  MSG_BUILD_MASK_MSG_SSID_LTE_RLCDL,
  MSG_BUILD_MASK_MSG_SSID_LTE_PDCPUL,
  MSG_BUILD_MASK_MSG_SSID_LTE_PDCPDL,
  MSG_BUILD_MASK_MSG_SSID_LTE_ML1
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_lte[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_lte) / sizeof(msg_bld_masks_lte[0])];
static uint32 msg_rt_masks_dci_lte[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_lte) / sizeof(msg_bld_masks_lte[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_qchat[] = 
{
    MSG_BUILD_MASK_MSG_SSID_QCHAT,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_CAPP,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_CENG,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_CREG,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_CMED,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_CAUTH,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_QBAL,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_OSAL,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_OEMCUST,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_MULTI_PROC,
    MSG_BUILD_MASK_MSG_SSID_QCHAT_UPK
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_qchat[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_qchat) / sizeof(msg_bld_masks_qchat[0])];
static uint32 msg_rt_masks_dci_qchat[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_qchat) / sizeof(msg_bld_masks_qchat[0])];

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_tdscdma[] =
{
   MSG_BUILD_MASK_MSG_SSID_TDSCDMA_L1, 
   MSG_BUILD_MASK_MSG_SSID_TDSCDMA_L2, 
   MSG_BUILD_MASK_MSG_SSID_TDSCDMA_MAC,
   MSG_BUILD_MASK_MSG_SSID_TDSCDMA_RLC, 
   MSG_BUILD_MASK_MSG_SSID_TDSCDMA_RRC 
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_tdscdma[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_tdscdma) / sizeof(msg_bld_masks_tdscdma[0])]; 
static uint32 msg_rt_masks_dci_tdscdma[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_tdscdma) / sizeof(msg_bld_masks_tdscdma[0])]; 

/* This table must be maintained as SSIDs are added! */
static const uint32 msg_bld_masks_cta[] =
{
    MSG_BUILD_MASK_MSG_SSID_CTA
};

/* To be initialized by msg_init() */
static uint32 msg_rt_masks_preset_cta[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_cta) / sizeof(msg_bld_masks_cta[0])];
static uint32 msg_rt_masks_dci_cta[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_cta) / sizeof(msg_bld_masks_cta[0])];


static const uint32 msg_bld_masks_qcnea[] =
{
    MSG_BUILD_MASK_MSG_SSID_QCNEA,
    MSG_BUILD_MASK_MSG_SSID_QCNEA_CAC,                           
    MSG_BUILD_MASK_MSG_SSID_QCNEA_CORE,                         
    MSG_BUILD_MASK_MSG_SSID_QCNEA_CORE_CAS,                      
    MSG_BUILD_MASK_MSG_SSID_QCNEA_CORE_CDE,                     
    MSG_BUILD_MASK_MSG_SSID_QCNEA_CORE_COM,                      
    MSG_BUILD_MASK_MSG_SSID_QCNEA_CORE_LEE,                     
    MSG_BUILD_MASK_MSG_SSID_QCNEA_CORE_QMI,                     
    MSG_BUILD_MASK_MSG_SSID_QCNEA_CORE_SRM,                     
    MSG_BUILD_MASK_MSG_SSID_QCNEA_GENERIC,                       
    MSG_BUILD_MASK_MSG_SSID_QCNEA_NETLINK,                      
    MSG_BUILD_MASK_MSG_SSID_QCNEA_NIMS,
    MSG_BUILD_MASK_MSG_SSID_QCNEA_NSRM,
    MSG_BUILD_MASK_MSG_SSID_QCNEA_NSRM_CORE,                    
    MSG_BUILD_MASK_MSG_SSID_QCNEA_NSRM_GATESM,                  
    MSG_BUILD_MASK_MSG_SSID_QCNEA_NSRM_TRG,                    
    MSG_BUILD_MASK_MSG_SSID_QCNEA_PLCY,                        
    MSG_BUILD_MASK_MSG_SSID_QCNEA_PLCY_ANDSF,                  
    MSG_BUILD_MASK_MSG_SSID_QCNEA_TEST,                         
    MSG_BUILD_MASK_MSG_SSID_QCNEA_WQE,                          
    MSG_BUILD_MASK_MSG_SSID_QCNEA_WQE_BQE,                       
    MSG_BUILD_MASK_MSG_SSID_QCNEA_WQE_CQE,                      
    MSG_BUILD_MASK_MSG_SSID_QCNEA_WQE_ICD,                       
    MSG_BUILD_MASK_MSG_SSID_QCNEA_WQE_IFSEL,                     
    MSG_BUILD_MASK_MSG_SSID_QCNEA_WQE_IFSELRSM                 
};

static uint32 msg_rt_masks_preset_qcnea[DIAG_MAX_PRESET_ID][sizeof(msg_bld_masks_qcnea) / sizeof(msg_bld_masks_qcnea[0])];
static uint32 msg_rt_masks_dci_qcnea[DIAG_MAX_DCI_ID][sizeof(msg_bld_masks_qcnea) / sizeof(msg_bld_masks_qcnea[0])];

/*----------!!!! Important Note: !!!!!!!--------*/
/* MSG_MASK_TBL_CNT needs to be updated manually,
when we add a new RANGE of SSIDs to msg_mask_tbl */
msg_mask_tbl_type msg_mask_tbl[] = {
 
  {
    MSG_SSID_GEN_FIRST,
    /* FIRST + array count */
    MSG_SSID_GEN_FIRST +
      sizeof (msg_bld_masks_gen) / sizeof (msg_bld_masks_gen[0]) - 1,
    msg_bld_masks_gen,
    { (uint32*)msg_rt_masks_preset_gen, (uint32*)msg_rt_masks_dci_gen },
    { (uint32*)msg_rt_masks_preset_gen, (uint32*)msg_rt_masks_dci_gen }
  }
  
  ,
  {
    MSG_SSID_ONCRPC,
    /* FIRST + array count */
    MSG_SSID_ONCRPC +
      sizeof (msg_bld_masks_oncrpc) / sizeof (msg_bld_masks_oncrpc[0]) - 1,
    msg_bld_masks_oncrpc,
    { (uint32*)msg_rt_masks_preset_oncrpc, (uint32*)msg_rt_masks_dci_oncrpc },
    { (uint32*)msg_rt_masks_preset_oncrpc, (uint32*)msg_rt_masks_dci_oncrpc }
  }

  ,
  {
    MSG_SSID_1X,
    /* FIRST + array count */
    MSG_SSID_1X +
      sizeof (msg_bld_masks_1X) / sizeof (msg_bld_masks_1X[0]) - 1,
    msg_bld_masks_1X,
    { (uint32*)msg_rt_masks_preset_1X, (uint32*)msg_rt_masks_dci_1X },
    { (uint32*)msg_rt_masks_preset_1X, (uint32*)msg_rt_masks_dci_1X }
  }

  ,
  {
    MSG_SSID_HDR_PROT,
    /* FIRST + array count */
    MSG_SSID_HDR_PROT+
      sizeof (msg_bld_masks_hdr) / sizeof (msg_bld_masks_hdr[0]) - 1,
    msg_bld_masks_hdr,
    { (uint32*)msg_rt_masks_preset_hdr, (uint32*)msg_rt_masks_dci_hdr },
    { (uint32*)msg_rt_masks_preset_hdr, (uint32*)msg_rt_masks_dci_hdr }
  }

  ,
  {
    MSG_SSID_UMTS,
    /* FIRST + array count */
    MSG_SSID_UMTS +
      sizeof (msg_bld_masks_umts) / sizeof (msg_bld_masks_umts[0]) - 1,
    msg_bld_masks_umts,
    { (uint32*)msg_rt_masks_preset_umts, (uint32*)msg_rt_masks_dci_umts },
    { (uint32*)msg_rt_masks_preset_umts, (uint32*)msg_rt_masks_dci_umts }
  }

  ,
  {
    MSG_SSID_GSM,
    /* FIRST + array count */
    MSG_SSID_GSM +
      sizeof (msg_bld_masks_gsm) / sizeof (msg_bld_masks_gsm[0]) - 1,
    msg_bld_masks_gsm,
    { (uint32*)msg_rt_masks_preset_gsm, (uint32*)msg_rt_masks_dci_gsm },
    { (uint32*)msg_rt_masks_preset_gsm, (uint32*)msg_rt_masks_dci_gsm }
  }

  ,
  {
    MSG_SSID_WLAN,
    /* FIRST + array count */
    MSG_SSID_WLAN +
      sizeof (msg_bld_masks_wlan) / sizeof (msg_bld_masks_wlan[0]) - 1,
    msg_bld_masks_wlan,
    {(uint32*)msg_rt_masks_preset_wlan, (uint32*)msg_rt_masks_dci_wlan },
    {(uint32*)msg_rt_masks_preset_wlan, (uint32*)msg_rt_masks_dci_wlan }
  }

  ,
  {
    MSG_SSID_ATS,
    /* FIRST + array count */
    MSG_SSID_ATS +
      sizeof (msg_bld_masks_mcs) / sizeof (msg_bld_masks_mcs[0]) - 1,
    msg_bld_masks_mcs,
    { (uint32*)msg_rt_masks_preset_mcs, (uint32*)msg_rt_masks_dci_mcs },
    { (uint32*)msg_rt_masks_preset_mcs, (uint32*)msg_rt_masks_dci_mcs }
  }

  ,
  {
    MSG_SSID_DS,
    /* FIRST + array count */
    MSG_SSID_DS +
      sizeof (msg_bld_masks_ds) / sizeof (msg_bld_masks_ds[0]) - 1,
    msg_bld_masks_ds,
    { (uint32*)msg_rt_masks_preset_ds, (uint32*)msg_rt_masks_dci_ds },
    { (uint32*)msg_rt_masks_preset_ds, (uint32*)msg_rt_masks_dci_ds }
  }

  ,
  {
    MSG_SSID_SEC,
    /* FIRST + array count */
    MSG_SSID_SEC +
      sizeof (msg_bld_masks_sec) / sizeof (msg_bld_masks_sec[0]) - 1,
    msg_bld_masks_sec,
    { (uint32*)msg_rt_masks_preset_sec, (uint32*)msg_rt_masks_dci_sec },
    { (uint32*)msg_rt_masks_preset_sec, (uint32*)msg_rt_masks_dci_sec }
  }

  ,
  {
    MSG_SSID_APPS,
    /* FIRST + array count */
    MSG_SSID_APPS +
      sizeof (msg_bld_masks_apps) / sizeof (msg_bld_masks_apps[0]) - 1,
    msg_bld_masks_apps,
    { (uint32*)msg_rt_masks_preset_apps, (uint32*)msg_rt_masks_dci_apps },
    { (uint32*)msg_rt_masks_preset_apps, (uint32*)msg_rt_masks_dci_apps }
  }

  ,
  {

    MSG_SSID_ADSPTASKS,
    /* FIRST + array count */
    MSG_SSID_ADSPTASKS +
      sizeof (msg_bld_masks_adsptasks) / sizeof (msg_bld_masks_adsptasks[0]) - 1,
    msg_bld_masks_adsptasks,
    { (uint32*)msg_rt_masks_preset_adsptasks, (uint32*)msg_rt_masks_dci_adsptasks },
    { (uint32*)msg_rt_masks_preset_adsptasks, (uint32*)msg_rt_masks_dci_adsptasks }
  }

  ,
  {
    MSG_SSID_L4LINUX_KERNEL,
    /* FIRST + array count */
    MSG_SSID_L4LINUX_KERNEL +
      sizeof (msg_bld_masks_l4_linux) / sizeof (msg_bld_masks_l4_linux[0]) - 1,
    msg_bld_masks_l4_linux,
    { (uint32*)msg_rt_masks_preset_l4_linux, (uint32*)msg_rt_masks_dci_l4_linux },
    { (uint32*)msg_rt_masks_preset_l4_linux, (uint32*)msg_rt_masks_dci_l4_linux }
  }

  ,
  {
    MSG_SSID_L4IGUANA_IGUANASERVER,
    /* FIRST + array count */
    MSG_SSID_L4IGUANA_IGUANASERVER +
      sizeof (msg_bld_masks_l4_iguana) / sizeof (msg_bld_masks_l4_iguana[0]) - 1,
    msg_bld_masks_l4_iguana,
    { (uint32*)msg_rt_masks_preset_l4_iguana, (uint32*)msg_rt_masks_dci_l4_iguana },
    { (uint32*)msg_rt_masks_preset_l4_iguana, (uint32*)msg_rt_masks_dci_l4_iguana }
  }

  ,
  {
    MSG_SSID_L4AMSS_QDIAG,
    /* FIRST + array count */
    MSG_SSID_L4AMSS_QDIAG +
      sizeof (msg_bld_masks_l4_amss) / sizeof (msg_bld_masks_l4_amss[0]) - 1,
    msg_bld_masks_l4_amss,
    { (uint32*)msg_rt_masks_preset_l4_amss, (uint32*)msg_rt_masks_dci_l4_amss },
    { (uint32*)msg_rt_masks_preset_l4_amss, (uint32*)msg_rt_masks_dci_l4_amss }
  }

  ,
  {
    MSG_SSID_HIT,
    /* FIRST + array count */
    MSG_SSID_HIT+
      sizeof (msg_bld_masks_hit) / sizeof (msg_bld_masks_hit[0]) - 1,
    msg_bld_masks_hit,
    { (uint32*)msg_rt_masks_preset_hit, (uint32*)msg_rt_masks_dci_hit },
    { (uint32*)msg_rt_masks_preset_hit, (uint32*)msg_rt_masks_dci_hit }
  }

  ,
  {
    MSG_SSID_QDSP6,
    /* FIRST + array count */
    MSG_SSID_QDSP6+
      sizeof (msg_bld_masks_qdsp6) / sizeof (msg_bld_masks_qdsp6[0]) - 1,
    msg_bld_masks_qdsp6,
    { (uint32*)msg_rt_masks_preset_qdsp6, (uint32*)msg_rt_masks_dci_qdsp6 },
    { (uint32*)msg_rt_masks_preset_qdsp6, (uint32*)msg_rt_masks_dci_qdsp6 }
  }

  ,
  {
    MSG_SSID_UMB,
    /* FIRST + array count */
    MSG_SSID_UMB+
      sizeof (msg_bld_masks_umb) / sizeof (msg_bld_masks_umb[0]) - 1,
    msg_bld_masks_umb,
    { (uint32*)msg_rt_masks_preset_umb, (uint32*)msg_rt_masks_dci_umb },
    { (uint32*)msg_rt_masks_preset_umb, (uint32*)msg_rt_masks_dci_umb }
  }

  ,
  {
    MSG_SSID_LTE,
    /* FIRST + array count */
    MSG_SSID_LTE+
      sizeof (msg_bld_masks_lte) / sizeof (msg_bld_masks_lte[0]) - 1,
    msg_bld_masks_lte,
    { (uint32*)msg_rt_masks_preset_lte, (uint32*)msg_rt_masks_dci_lte },
    { (uint32*)msg_rt_masks_preset_lte, (uint32*)msg_rt_masks_dci_lte }
  }

  ,
  {
    MSG_SSID_QCHAT,
    /* FIRST + array count */
    MSG_SSID_QCHAT+
      sizeof (msg_bld_masks_qchat) / sizeof(msg_bld_masks_qchat[0]) - 1,
    msg_bld_masks_qchat,
    { (uint32*)msg_rt_masks_preset_qchat, (uint32*)msg_rt_masks_dci_qchat },
    { (uint32*)msg_rt_masks_preset_qchat, (uint32*)msg_rt_masks_dci_qchat }
  }

  ,
  {
    MSG_SSID_TDSCDMA_L1,
    /* FIRST + array count */
    MSG_SSID_TDSCDMA_L1+
      sizeof (msg_bld_masks_tdscdma) / sizeof(msg_bld_masks_tdscdma[0]) - 1,
    msg_bld_masks_tdscdma,
    { (uint32*)msg_rt_masks_preset_tdscdma, (uint32*)msg_rt_masks_dci_tdscdma },
    { (uint32*)msg_rt_masks_preset_tdscdma, (uint32*)msg_rt_masks_dci_tdscdma }
  }

  ,
  {
    MSG_SSID_CTA,
    /* FIRST + array count */
    MSG_SSID_CTA+
        sizeof(msg_bld_masks_cta) / sizeof(msg_bld_masks_cta[0]) - 1,
    msg_bld_masks_cta,
    { (uint32*)msg_rt_masks_preset_cta, (uint32*)msg_rt_masks_dci_cta },
    { (uint32*)msg_rt_masks_preset_cta, (uint32*)msg_rt_masks_dci_cta }
  }
    ,
  {
      MSG_SSID_QCNEA,
      MSG_SSID_QCNEA+
        sizeof(msg_bld_masks_qcnea) / sizeof(msg_bld_masks_qcnea[0]) - 1,
      msg_bld_masks_qcnea,
     { (uint32*)msg_rt_masks_preset_qcnea, (uint32*)msg_rt_masks_dci_qcnea },
     { (uint32*)msg_rt_masks_preset_qcnea, (uint32*)msg_rt_masks_dci_qcnea }
  }
};

const uint32 msg_mask_tbl_size = (sizeof (msg_mask_tbl) / sizeof (msg_mask_tbl[0])) ;
