#ifndef SNS_SSM_PRIV_H
#define SNS_SSM_PRIV_H
/*=============================================================================
  @file sns_ssm_priv.h

  Sensors Service Manager header

  This header file contains the data types, macros and constants used by 
  Sensors Service Manager

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/ssm/framework/inc/sns_ssm_priv.h#1 $  */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*=============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  ------------------------------------------------------------
  2014-01-22  dk   Initial version

=============================================================================*/


/*-----------------------------------------------------------------------------
* Include Files
* ---------------------------------------------------------------------------*/

#include "stdbool.h"
#include "sns_common.h"
#include "sns_common_v01.h"
#include "sns_debug_api.h"

#include "sns_smr_util.h"
#include "sns_em.h"
#include "sns_debug_str.h"


/*-----------------------------------------------------------------------------
* Definitions for SSM thread
* ----------------------------------------------------------------------------*/

#define SNS_SSM_MODULE           SNS_MODULE_DSPS_SSM

// SSM Max constants 
#define SNS_SSM_MAX_CLIENT_CONNECTIONS 5
#define SNS_SSM_MAX_SERVICES           3

// Bitmasks for SSM signals
#define SNS_SSM_TIMER_SIG             0x01
#define SNS_SSM_QMI_WAIT_SIG          0x02


/*-----------------------------------------------------------------------------
* Macros for SSM
* ---------------------------------------------------------------------------*/

//F3 messages
#if defined QDSP6
#ifndef DBG_MEDIUM_PRIO
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#endif //DBG_MEDIUM_PRIO
#define SNS_SSM_PRINTF0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO,msg)
#define SNS_SSM_PRINTF1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO,msg,p1)
#define SNS_SSM_PRINTF2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO,msg,p1,p2)
#define SNS_SSM_PRINTF3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO,msg,p1,p2,p3)
#elif defined(ADSP_STANDALONE)
#define SNS_SSM_PRINTF0(level,msg)          qurt_printf("\n"); qurt_printf(msg)
#define SNS_SSM_PRINTF1(level,msg,p1)       qurt_printf("\n"); qurt_printf(msg,p1)
#define SNS_SSM_PRINTF2(level,msg,p1,p2)    qurt_printf("\n"); qurt_printf(msg,p1,p2)
#define SNS_SSM_PRINTF3(level,msg,p1,p2,p3) qurt_printf("\n"); qurt_printf(msg,p1,p2,p3)
#endif //QDSP6


//SSM Macros
#define SSM_BIT_TEST(f,b)   (((f) & (b)) == (b))


/*-----------------------------------------------------------------------------
* Definitions for structures maintained by SSM
* ---------------------------------------------------------------------------*/


/**
*  Services contained in SSM
*/
typedef enum
{
   SNS_SSM_SRV_NONE = 0,
   SNS_SSM_SRV_TIME_SERVICE = 1,

   /* Add more services here */
} sns_ssm_services_e;

/** 
* QCSI Connection info used by services
*/
typedef struct
{
   qmi_client_handle    client_handle;
   bool                 connection_valid;
} sns_ssm_qmi_connection_s;

/**
* QCSI information used by a service in SSM
*/
typedef struct 
{
   //Handles provided by QCSI framework
   sns_ssm_qmi_connection_s   connection_handle[SNS_SSM_MAX_CLIENT_CONNECTIONS];
   qmi_csi_os_params          os_params;
   sns_ssm_services_e         service;
   void                       *service_cookie_ptr;
   uint32_t                   service_status;
   qmi_csi_service_handle     service_handle;
   qmi_csi_options            service_options;
   bool                       service_valid;
} sns_ssm_service;

/**
* All the data required by the SSM framework
*/
typedef struct 
{
   //OS signals
   OS_FLAG_GRP *ssm_flag_grp; /* Flags set outside SSM, to be consumed by SSM*/
   OS_FLAGS     ssm_flags;    /* Flags set by SSM to be consumed by SSM*/

   //Services
   sns_ssm_service ssm_service[SNS_SSM_MAX_SERVICES];

}sns_ssm_type_s;


#endif /*#ifndef SNS_SSM_PRIV_H*/
