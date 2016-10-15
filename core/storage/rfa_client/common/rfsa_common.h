#ifndef __RFSA_COMMON_H__
#define __RFSA_COMMON_H__
/******************************************************************************
 * rfs_common.h
 *
 * General definitions that is shared between the server and client side
 *
 * Copyright (c) 2012
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/storage/rfa_client/common/rfsa_common.h#1 $
  $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2012-03-22   rh      Initial checkin
=============================================================================*/

 
#include <stdint.h>
#include "remote_filesystem_access_v01.h"


/****************************************************************************
 * The definitions.                                                         *
 ****************************************************************************/

/**
  * Return codes to the client/server layers
  */

#define RFSA_EOK                 0x000 
#define RFSA_EFAILED             0x001
#define RFSA_EEOS                0x002

#define QDSP

#ifdef QDSP 
#include "msg_diag_service.h"
#endif

/****************************************************************************
 * The macros.                                                         *
 ****************************************************************************/

/**
  * Used to display messages 
  */
#ifdef APPS
   #define LOGI(...)      fprintf(stdout,__VA_ARGS__)
#else
   #define LOGI(...)      MSG(MSG_SSID_QDSP6, MSG_LVL_HIGH, __VA_ARGS__);
#endif

#ifdef APPS
   #define LOGI1(...)      fprintf(stdout,__VA_ARGS__)
#else
   #define LOGI1(...)      MSG_1(MSG_SSID_QDSP6, MSG_LVL_LOW, __VA_ARGS__);
#endif

#ifdef APPS
   #define LOGI2(...)      fprintf(stdout,__VA_ARGS__)
#else
   #define LOGI2(...)      MSG_2(MSG_SSID_QDSP6, MSG_LVL_HIGH, __VA_ARGS__);
#endif

#ifdef APPS
   #define LOGI3(...)      fprintf(stdout,__VA_ARGS__)
#else
   #define LOGI3(...)      MSG_2(MSG_SSID_QDSP6, MSG_LVL_HIGH, __VA_ARGS__);
#endif

#ifdef APPS
   #define LOGI8(...)      fprintf(stdout,__VA_ARGS__)
#else
   #define LOGI8(...)      MSG_8(MSG_SSID_QDSP6, MSG_LVL_HIGH, __VA_ARGS__);
#endif

#ifdef APPS
   #define LOGE(...)      fprintf(stderr,__VA_ARGS__)
#else
   #define LOGE(...)      MSG(MSG_SSID_QDSP6, MSG_LVL_ERROR, __VA_ARGS__);
#endif

#ifdef APPS
   #define LOGE1(...)      fprintf(stderr,__VA_ARGS__)
#else
   #define LOGE1(...)      MSG_1(MSG_SSID_QDSP6, MSG_LVL_ERROR, __VA_ARGS__);
#endif

#ifdef APPS
   #define LOGE2(...)      fprintf(stderr,__VA_ARGS__)
#else
   #define LOGE2(...)      MSG_2(MSG_SSID_QDSP6, MSG_LVL_ERROR, __VA_ARGS__);
#endif

#ifdef APPS
   #define LOGE3(...)      fprintf(stderr,__VA_ARGS__)
#else
   #define LOGE3(...)      MSG_3(MSG_SSID_QDSP6, MSG_LVL_ERROR, __VA_ARGS__);
#endif

/****************************************************************************
 * The type definitions.                                                    *
 ****************************************************************************/

/**
  *  
  */
typedef struct
{
  /**
    * Opcode of the command
    */
  int32_t                   opcode;
  /**
    * The payload associated with the command
    */
  void                      *data_ptr;
  /**
    * The payload size 
    */
  int32_t                   data_size;
  /**
    * The structures returned by the command
    */ 
  union {
		rfsa_file_stat_req_msg_v01		      file_stat_req;
		rfsa_file_create_req_msg_v01			file_create_req;
		rfsa_file_read_req_msg_v01		      file_read_req;
      rfsa_get_buff_addr_req_msg_v01      get_buff_addr_req;
		rfsa_release_buff_addr_req_msg_v01	free_buff_addr_req;
      rfsa_iovec_file_read_req_msg_v01    iovec_read_req;
      rfsa_iovec_file_write_req_msg_v01   iovec_write_req;
  } rfsa_req;


  union {
		rfsa_file_stat_resp_msg_v01	      file_stat_ret;
		rfsa_file_create_resp_msg_v01			file_create_ret;
		rfsa_file_read_resp_msg_v01	      file_read_ret;
      rfsa_get_buff_addr_resp_msg_v01     get_buff_addr_ret;
		rfsa_release_buff_addr_resp_msg_v01	free_buff_addr_ret;
      rfsa_iovec_file_read_resp_msg_v01   iovec_read_ret;
      rfsa_iovec_file_write_resp_msg_v01  iovec_write_ret;
  } rfs_ret;

} rfsa_packet_t;


/**
  * The callback between the client/server layer and and the one below 
  */
typedef int32_t ( *rfsa_callback ) (rfsa_packet_t *packet);


#endif /* __RFSA_COMMON_H__ */

