/*=============================================================================

FILE:         qdss_ctrl.c

DESCRIPTION:  Implements handlers for diag comamnds to configure qdss on
              a particular core
================================================================================
              Copyright (c) 2012 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "qdss_ctrl_diag.h"
#include "qdss_ctrl_etm.h"
#include "qdss_ctrl_utils.h"

#include "DALSys.h"

struct qdss_ctrl {
   DALSYSSyncHandle hSync;
};

struct qdss_ctrl qdss_ctl;

/*-------------------------------------------------------------------------*/

/**
  @brief  Handles the QDSS_CTRL_FILTER_ETM diag command

  @return 0 if successful, error code otherwise
 */

int qdss_ctrl_filter_etm_handler(qdss_ctrl_filter_etm_req *pReq,
                          int req_len,
                          qdss_ctrl_filter_etm_rsp *pRsp,
                          int rsp_len)
{
   int nErr = DAL_ERROR;

   if (0==qdss_ctl.hSync) {
      nErr= DAL_SYNC_ENTER_FAILED;
      pRsp->result = nErr;
      return nErr;
   }

   DALSYS_SyncEnter(qdss_ctl.hSync);


   if (pReq->state) {
      nErr = qdss_ctrl_etm_enable();
   }
   else {
      nErr = qdss_ctrl_etm_disable();
   }

   pRsp->result = nErr;
   DALSYS_SyncLeave(qdss_ctl.hSync);
   return nErr;
}


/*-------------------------------------------------------------------------*/


/**
  @brief Initializes the QDSS control subsystem.

  This is called from RC init.

  @return None
 */
void qdss_ctrl_init(void)
{
   int nErr;
   qdss_ctrl_diag_init();
   qdss_ctl.hSync = 0;

   TRY(nErr,DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &qdss_ctl.hSync, NULL));

   CATCH(nErr) {
   }
   IGNORE(nErr);
}


