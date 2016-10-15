#ifndef __RFS_VTL_CLIENT_H__
#define __RFS_VTL_CLIENT_H__
/******************************************************************************
 * rfsa_vtl_client.h
 *
 * Remote file system access, client side RPC packet router selection layer. 
 * Header file.
 *
 * Copyright (c) 2012
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/storage/rfa_client/src/rfsa_vtl_client.h#1 $
  $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2012-02-22   rh      Initial checkin
=============================================================================*/


#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "remote_filesystem_access_v01.h"
#include "rfsa_common.h"

int32_t rfsa_vtl_client_init(void);
int32_t rfsa_vtl_client_deinit(void);
int32_t rfsa_vtl_client_send(rfsa_packet_t *packet_ptr, void *data_ptr);

int32_t rfsa_vtl_stat(rfsa_file_stat_req_msg_v01 *file_stat_ptr,
                     rfsa_file_stat_resp_msg_v01 *file_stat_resp_ptr);
int32_t rfsa_vtl_read(rfsa_file_read_req_msg_v01 *file_read_ptr, 
                     rfsa_file_read_resp_msg_v01 *file_read_ret_ptr);
int32_t rfsa_vtl_fcreat(rfsa_file_create_req_msg_v01 *file_stat_ptr,
                       rfsa_file_create_resp_msg_v01 *file_stat_resp_ptr);

int32_t rfsa_vtl_get_buff_addr (rfsa_get_buff_addr_req_msg_v01 *get_buff_addr_ptr, 
                               rfsa_get_buff_addr_resp_msg_v01 *get_buff_addr_ret_ptr);
int32_t rfsa_vtl_readv  (rfsa_iovec_file_read_req_msg_v01 *iovec_read_ptr);
int32_t rfsa_vtl_writev (rfsa_iovec_file_write_req_msg_v01 *iovec_write_ptr);

#endif /* __RFS_VTL_CLIENT_H__ */

