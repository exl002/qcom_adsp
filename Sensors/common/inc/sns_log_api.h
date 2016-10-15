#ifndef SNS_LOG_API_H
#define SNS_LOG_API_H

/*============================================================================

@file 
sns_log_api.h

@brief
Contains function prototypes of loggings API's.

Copyright (c) 2010-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

			EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/common/inc/sns_log_api.h#1 $

when       who    what, where, why
(mm/dd/yyyy)
--------   ---    ----------------------------------------------------------
02/28/2014  MW     Moved sns_logpkt_malloc and sns_logpkt_commit to 
					sns_log_api_public.h. Removed sns_logpkt_shorten.
11/09/2010  ad     added support for log filtering 
09/17/2010  sj     Usage of <type>_t (avoids including comdef)
8/12/2010   sj     Created
===========================================================================*/

/*=====================================================================
                 INCLUDE FILES
=======================================================================*/
#include "sns_diag_dsps_v01.h"
#include "sns_log_api_public.h"
#include <stddef.h>

/*===========================================================================

  FUNCTION:   sns_diag_dsps_set_log_mask

===========================================================================*/
/*!
  @brief
  Sets the log mask on DSPS
  
   
  @param[i] 
  msg_ptr: pointer to message containing log mask that indicates which
           log packets are enabled/disabled.

  @return
  none
*/
/*=========================================================================*/
void sns_diag_dsps_set_log_mask(sns_diag_set_log_mask_req_msg_v01* msg_ptr);

#endif /* SNS_LOG_API_H */
