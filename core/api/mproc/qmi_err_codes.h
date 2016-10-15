#ifndef _QMI_ERR_CODES_H_
#define _QMI_ERR_CODES_H_
/******************************************************************************
  @file    qmi_err_codes.h
  @brief   Common QMI client error codes

  DESCRIPTION
  QMI error codes which can be shared by multiple modules providing 
  client interfaces for the QMI protocol.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  ---------------------------------------------------------------------------
  Copyright (c) 2007-2012 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/api/mproc/qmi_err_codes.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/12   ap      Initial Version
===========================================================================*/

#define QMI_NO_ERR                0
#define QMI_INTERNAL_ERR          (-1)
#define QMI_SERVICE_ERR           (-2)
#define QMI_TIMEOUT_ERR           (-3)
#define QMI_EXTENDED_ERR          (-4)
#define QMI_PORT_NOT_OPEN_ERR     (-5)
#define QMI_MEMCOPY_ERROR         (-13)
#define QMI_INVALID_TXN           (-14)
#define QMI_CLIENT_ALLOC_FAILURE  (-15)

#endif
