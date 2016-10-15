/******************************************************************************
 * rfsa_qmi_client.h
 *
 * QMI RPC communication interface function, client side supporting function
 * for Remote File System Access
 *
 * Copyright (c) 2012
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/storage/rfa_client/src/qmi/rfsa_qmi_client.h#1 $
  $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2012-11-28   rh      Add deinit routine
2012-02-22   rh      Initial check-in
=============================================================================*/

#ifndef __RFS_QMI_CLIENT_H__
#define __RFS_QMI_CLIENT_H__

#include "remote_filesystem_access_v01.h"

int rfsa_qmi_client_init(void);
int rfsa_qmi_client_deinit(void);
int32_t rfsa_qmi_stat (rfsa_file_stat_req_msg_v01 *file_stat_ptr, 
                      rfsa_file_stat_resp_msg_v01 *file_stat_ret_ptr);
int32_t rfsa_qmi_fcreat (rfsa_file_create_req_msg_v01 *file_create_ptr, 
                      rfsa_file_create_resp_msg_v01 *file_create_ret_ptr);
int32_t rfsa_qmi_read (rfsa_file_read_req_msg_v01 *file_read_ptr, 
                      rfsa_file_read_resp_msg_v01 *file_read_ret_ptr);

int32_t rfsa_get_buff_addr (rfsa_get_buff_addr_req_msg_v01 *get_buff_addr_ptr, 
                           rfsa_get_buff_addr_resp_msg_v01 *get_buff_addr_ret_ptr);
int32_t rfsa_iovec_read (rfsa_iovec_file_read_req_msg_v01 *iovec_read_ptr, 
                        rfsa_iovec_file_read_resp_msg_v01 *iovec_read_ret_ptr);
int32_t rfsa_iovec_write (rfsa_iovec_file_write_req_msg_v01 *iovec_write_ptr, 
                         rfsa_iovec_file_write_resp_msg_v01 *iovec_write_ret_ptr);

#endif /* __RFS_QMI_CLIENT_H__ */

