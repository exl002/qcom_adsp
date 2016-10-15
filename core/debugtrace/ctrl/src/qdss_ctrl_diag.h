#ifndef QDSS_CTRL_DIAG_H
#define QDSS_CTRL_DIAG_H

/*=============================================================================

FILE:         qdss_ctrl_diag.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/
#include "diagpkt.h"
#include "diagcmd.h"

#define QDSS_CTRL_FILTER_ETM                0x02


typedef PACK(struct)
{
  uint8 cmdCode;        /**< Diag Message ID */
  uint8 subsysId;       /**< Subsystem ID (DIAG_SUBSYS_QDSS)*/
  uint16 subsysCmdCode; /**< Subsystem command code */
} qdss_ctrl_diag_pkt_hdr;

/*-------------------------------------------------------------------------*/
/**
  @brief Response packet: Generic reponse with result.

  Common packet when only returning a result.
 */

typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
} qdss_ctrl_diag_pkt_req;


typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr; /**< Header */
  uint8 result;            /**< See QDSS_CMDRESP_... definitions */
} qdss_ctrl_diag_pkt_rsp;


typedef PACK(struct)
{
  qdss_ctrl_diag_pkt_hdr hdr;
  uint8  state;
} qdss_ctrl_filter_etm_req;

typedef qdss_ctrl_diag_pkt_rsp qdss_ctrl_filter_etm_rsp; 



int qdss_ctrl_filter_etm_handler(qdss_ctrl_filter_etm_req *pReq,
                                 int req_len,
                                 qdss_ctrl_filter_etm_rsp *pRsp,
                                 int rsp_len);

void qdss_ctrl_diag_init(void);

#endif //QDSS_CTRL_DIAG_H
