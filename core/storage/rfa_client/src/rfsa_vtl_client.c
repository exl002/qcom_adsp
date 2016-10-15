/******************************************************************************
 * rfsa_vtl_client.c
 *
 * Remote file system access, client side RPC packet router selection layer
 *
 * Copyright (c) 2012
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/storage/rfa_client/src/rfsa_vtl_client.c#1 $
  $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2012-11-28   rh      Adding deinit routine to the QMI layer
2012-10-11   rh      Code cleanup
2012-02-22   rh      Initial checkin
=============================================================================*/

#include "rfsa_common.h"
#include "rfsa_qmi_client.h"
#include "remote_filesystem_access_v01.h"

/****************************************************************************
* Defines                                                                   *
****************************************************************************/

#define APR_LAYER 0
#define QMI_LAYER 1

#define RFSA_CLIENT_ID        0x011013ec

/****************************************************************************
* Implementations                                                           *
****************************************************************************/


int32_t rfsa_vtl_client_init(void)
{ 
   return rfsa_qmi_client_init();
}


int32_t rfsa_vtl_client_deinit(void)
{
   return rfsa_qmi_client_deinit();
}


int32_t rfsa_vtl_client_send(rfsa_packet_t *packet_ptr, void *data_ptr)
{
   return RFSA_EOK;  
}
  

int32_t rfsa_vtl_stat(rfsa_file_stat_req_msg_v01 *file_stat_ptr, 
                  rfsa_file_stat_resp_msg_v01 *file_stat_ret_ptr)
{
   return rfsa_qmi_stat(file_stat_ptr, file_stat_ret_ptr);
}


int32_t rfsa_vtl_fcreat(rfsa_file_create_req_msg_v01 *file_creat_ptr, 
                  rfsa_file_create_resp_msg_v01 *file_creat_ret_ptr)
{
   return rfsa_qmi_fcreat(file_creat_ptr, file_creat_ret_ptr);
}


int32_t rfsa_vtl_read (rfsa_file_read_req_msg_v01 *file_read_ptr, 
                      rfsa_file_read_resp_msg_v01 *file_read_ret_ptr)
{
   file_read_ptr->client_id = RFSA_CLIENT_ID;
   return rfsa_qmi_read(file_read_ptr, file_read_ret_ptr);
}


int32_t rfsa_vtl_get_buff_addr (rfsa_get_buff_addr_req_msg_v01 *get_buff_addr_ptr, 
                               rfsa_get_buff_addr_resp_msg_v01 *get_buff_addr_ret_ptr)
{
   get_buff_addr_ptr->client_id = RFSA_CLIENT_ID;
   return rfsa_get_buff_addr (get_buff_addr_ptr, get_buff_addr_ret_ptr);
}

int32_t rfsa_vtl_readv (rfsa_iovec_file_read_req_msg_v01 *iovec_read_ptr)
{
   rfsa_iovec_file_read_resp_msg_v01 iovec_read_ret;
   iovec_read_ptr->client_id = RFSA_CLIENT_ID;
   return rfsa_iovec_read (iovec_read_ptr, &iovec_read_ret);
}

int32_t rfsa_vtl_writev (rfsa_iovec_file_write_req_msg_v01 *iovec_write_ptr)
{
   rfsa_iovec_file_write_resp_msg_v01 iovec_write_ret;
   iovec_write_ptr->client_id = RFSA_CLIENT_ID;
   return rfsa_iovec_write (iovec_write_ptr, &iovec_write_ret);
}

