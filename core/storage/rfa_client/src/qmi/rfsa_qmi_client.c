/******************************************************************************
 * rfsa_qmi_client.c
 *
 * QMI RPC communication interface function, client side supporting function
 * for Remote File System Access
 *
 * Copyright (c) 2012-2013
 * Qualcomm Technology Inc.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/storage/rfa_client/src/qmi/rfsa_qmi_client.c#1 $
  $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2013-02-14   rh      Make call to qmi_init non blocking
2012-11-28   rh      Adding critical section for all QMI calls
2012-10-12   rh      Fix detection of end of file correctly for read on Q6
2012-10-11   rh      Reduce un-necessary message printed
2012-02-22   rh      Initial check-in
=============================================================================*/

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "rfsa_misc.h"
#include "rfsa_common.h"

#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "remote_filesystem_access_v01.h"
#include "qmi_client.h"

#ifndef ANDROID
#include "qurt.h"
#endif


//#if !define(RFSA_USES_COSIM_ENV) && !defined(QDSP)
//#include <sys/mman.h>
//#endif

#ifdef ANDROID
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>
#endif

#define TRUE  1
#define FALSE 0

/*****************************************************************************
* Defines                                                                   *
****************************************************************************/

#define RFSA_MAX_ENTRIES   10

#ifndef PMEM_GET_PHYS
#define PMEM_GET_PHYS   _IOW('p', 1, unsigned int)
#endif //PMEM_GET_PHYS

#define QMI_CLNT_WAIT_SIG  0x00010000
#define QMI_CLNT_TIMER_SIG 0x00000001

/*****************************************************************************
* Definitions                                                               *
****************************************************************************/
struct rfsa_pmem_region {
   unsigned long offset;
   unsigned long len;
};

/*****************************************************************************
* Variables                                                                 *
****************************************************************************/
qmi_idl_service_object_type   service_object;
qmi_cci_os_signal_type        os_params;     /**< Signal on which to wait on */
uint32_t                      num_services;  /**< Number of Services */
uint8_t                       client_init_done;
qmi_client_type               notifier;
uint32_t                      num_entries;
qmi_service_info              service_info[RFSA_MAX_ENTRIES];
qmi_client_type               client;
rfsa_lock_t                   rfsa_qmi_lock = NULL;


/*****************************************************************************
* Implementations                                                           *
****************************************************************************/

/*=============================================================================
*
*    rfsa_indicator - Indicator function called when service needs to 
*                         indicate a APPS RESET or Force sync
*
* PARAMETERS
*    \param client
*         Client handle
*    \param msg_id
*         Message ID
*    \param ind_buf
*         Encoded Indicator Message
*    \param ind_buf_len
*         Encoded Indicator Message size
*    \param ind_cb_data
*         Indicator callback data
*
*===========================================================================*/
static void rfsa_indicator
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
 )
{
   LOGI1("Remotefs Indicator Called, MSGID=%d\n", msg_id);
} 


int32_t rfsa_qmi_client_init(void)
{
   qmi_client_error_type rc;

   if (rfsa_qmi_lock == NULL) {
      (void)rfsa_lock_create(&rfsa_qmi_lock);
   }

   service_object = rfsa_get_service_object_v01();
   if (!service_object)
   {
      LOGE("rfsa_get_service_object_v01 error!\n");
      return RFSA_EFAILED;
   }

   os_params.timed_out = 0;


#ifndef ANDROID
   // QURT need more information set
   os_params.sig = QMI_CLNT_WAIT_SIG;
   os_params.timer_sig = QMI_CLNT_TIMER_SIG;
#endif

   rc = qmi_client_notifier_init(service_object, &os_params, &notifier);

   if(rc != QMI_NO_ERR)
   {
      LOGE("qmi_client_notifier_init error!\n");
      return RFSA_EFAILED;
   }

   /* Check if the service is up, if not exit with failure */
   rc = qmi_client_get_service_list(service_object, NULL, NULL,
      (unsigned int *)&num_services);

   if(rc != QMI_NO_ERR)
   {
      return RFSA_EFAILED;
   }

   QMI_CCI_OS_SIGNAL_CLEAR(&os_params);

   num_entries = RFSA_MAX_ENTRIES;
   /* The server has come up, store the information in info variable */
   rc = qmi_client_get_service_list(service_object,
      service_info, 
      (unsigned int *)&num_entries,
      (unsigned int *)&num_services);

   if(rc != QMI_NO_ERR)
   {
      LOGE("qmi_client_get_service_list error!\n");
      return RFSA_EFAILED;
   }

   rc = qmi_client_init(&service_info[0], service_object, rfsa_indicator,
                        NULL, &os_params, &client);
   if(rc != QMI_NO_ERR)
   {
      LOGE("qmi_client_init error!\n");
      return RFSA_EFAILED;
   }

   client_init_done = TRUE;

   return RFSA_EOK;
}


int32_t rfsa_qmi_client_deinit(void)
{
   (void)rfsa_lock_destroy(rfsa_qmi_lock);
   rfsa_qmi_lock = NULL;
   return RFSA_EOK;
}


int32_t rfsa_qmi_stat (rfsa_file_stat_req_msg_v01 *file_stat_ptr, 
                      rfsa_file_stat_resp_msg_v01 *file_stat_ret_ptr)
{
   qmi_client_error_type         rc;

   (void)rfsa_lock_enter(rfsa_qmi_lock);

   rc = qmi_client_send_msg_sync(client,
                                  QMI_RFSA_FILE_STAT_REQ_MSG_V01,
                                  (void *)file_stat_ptr, sizeof(rfsa_file_stat_req_msg_v01),
                                  (void *)file_stat_ret_ptr, sizeof(rfsa_file_stat_resp_msg_v01),
      0);
   (void)rfsa_lock_leave(rfsa_qmi_lock);

   if (rc != QMI_NO_ERR)
   {
      LOGE1("RFS Stat Call Failed: Status:%d\n", rc);
      (void)qmi_client_release(client);
      return RFSA_EFAILED;
   }

   return RFSA_EOK;
}


int32_t rfsa_qmi_fcreat (rfsa_file_create_req_msg_v01 *file_create_ptr, 
                      rfsa_file_create_resp_msg_v01 *file_create_ret_ptr)
{
   qmi_client_error_type         rc;

   (void)rfsa_lock_enter(rfsa_qmi_lock);

   rc = qmi_client_send_msg_sync(client,
                                  QMI_RFSA_FILE_CREATE_REQ_MSG_V01,
                                  (void *)file_create_ptr, sizeof(rfsa_file_create_req_msg_v01),
                                  (void *)file_create_ret_ptr, sizeof(rfsa_file_create_resp_msg_v01),
      0);
   (void)rfsa_lock_leave(rfsa_qmi_lock);

   if (rc != QMI_NO_ERR)
   {
      LOGE1("RFS File Create Call Failed: Status:%d\n", rc);
      (void)qmi_client_release(client);
      return RFSA_EFAILED;
   }

   return RFSA_EOK;
}


int32_t rfsa_qmi_read (rfsa_file_read_req_msg_v01 *file_read_ptr, 
                      rfsa_file_read_resp_msg_v01 *file_read_ret_ptr)
{
   qmi_client_error_type         rc;
   char *buffer_ret; 
   int32_t ret = RFSA_EOK;
   uint32_t buffer32;

   buffer32   = (uint32_t)(file_read_ret_ptr->data.buffer);
   buffer_ret = (char *)buffer32;

   (void)rfsa_lock_enter(rfsa_qmi_lock);

   rc = qmi_client_send_msg_sync(client, QMI_RFSA_FILE_READ_REQ_MSG_V01, 
                                 (void *)file_read_ptr, sizeof(rfsa_file_read_req_msg_v01), 
                                 (void *)file_read_ret_ptr, sizeof(rfsa_file_read_resp_msg_v01), 0);
   if (rc != QMI_NO_ERR)
   {
      LOGE1("RFS Read Call Failed: Status:%d\n", rc);
      (void)qmi_client_release(client);
      return RFSA_EFAILED;
   }

#ifndef ANDROID
   do
   {
      qurt_mem_region_attr_t fsread_attr;
      qurt_mem_region_t fsread_memmapregion;
      qurt_mem_pool_t fsread_pool;
      unsigned char *virtualAddress;
      unsigned int addr = 0;

      if(file_read_ret_ptr->data.count == 0)
      {
         // End of file is reached, return with success
         break;
      }
      if(file_read_ret_ptr->data.count > 0 && file_read_ret_ptr->data.buffer != 0)
      {
         if(0 != (rc = qurt_mem_pool_attach("RFSA_pool", &fsread_pool)))
         {
            LOGE1("Error in qurt_mem_pool_attach, rc: %d\n", rc); 
            break;
         }

         qurt_mem_region_attr_init(&fsread_attr);
         qurt_mem_region_attr_set_cache_mode(&fsread_attr, QURT_MEM_CACHE_NONE);
         qurt_mem_region_attr_set_mapping(&fsread_attr, QURT_MEM_MAPPING_PHYS_CONTIGUOUS);
         qurt_mem_region_attr_set_physaddr(&fsread_attr, (uint32_t)file_read_ret_ptr->data.buffer);

         //LOGI1("count: %d\n", (unsigned int)file_read_ret_ptr->data.count);
         //LOGI1("PA: 0x%x\n", (unsigned int)file_read_ret_ptr->data.buffer);

         /* create the memory region */
         if (0 != (rc = qurt_mem_region_create(
            &fsread_memmapregion,
                            file_read_ret_ptr->data.count,
            fsread_pool,
            &fsread_attr)))
         {
            LOGE1("Error in qurt_mem_region_create, rc: %d\n", rc);
            break;
         }

         if (0 != (rc = qurt_mem_region_attr_get(fsread_memmapregion, &fsread_attr)))
         {
            LOGE1("Error in qurt_mem_region_attr_get, rc: %d\n", rc);
            qurt_mem_region_delete(fsread_memmapregion);
            break;
         }

         qurt_mem_region_attr_get_physaddr(&fsread_attr, &addr);
         //LOGI1("PA default: 0x%x\n", addr);

         qurt_mem_region_attr_get_virtaddr (&fsread_attr, (unsigned int *)(&virtualAddress));
         memcpy (buffer_ret, virtualAddress, file_read_ret_ptr->data.count);
         qurt_mem_region_delete(fsread_memmapregion);
         //LOGI2("Client file read: %d buffer:0x%x\n", (unsigned int)file_read_ret_ptr->data.count, 
         //                                           (unsigned int)file_read_ret_ptr->data.buffer); 
      }
      else
      {
         ret = RFSA_EFAILED;
      }
   } while(0);
#endif

#ifdef ANDROID
   // Andriod version of the code is used to test client side operation locally on the Android OS
   do 
   {
      uint8_t *virtualAddr;
      uint32_t addr = 0;
      uint32_t count = file_read_ret_ptr->data.count;
      int mem_fd;
      struct rfsa_pmem_region region;

      if(file_read_ret_ptr->data.count > 0 && file_read_ret_ptr->data.buffer != 0)
      {
         /* Use /dev/mem to read the physical address provided */
         mem_fd = open ("/dev/mem", O_RDWR);

         if(mem_fd < 0) {
            LOGE ("/dev/mem open failed\n");
            ret = RFSA_EFAILED;
            break;
         }

         /* Map the PMEM file descriptor into current process address space */
         virtualAddr = (uint8_t*) mmap (NULL, count,
                                    PROT_READ, MAP_SHARED, mem_fd, file_read_ret_ptr->data.buffer);

         if (virtualAddr == MAP_FAILED)
         {
            LOGE ("\n mmap() failed\n");
            close(mem_fd);
            ret = RFSA_EFAILED;
            break;
         }

         memcpy(buffer_ret, virtualAddr, count);

         // Unmap the shared memory buffer
         if (EINVAL == munmap (virtualAddr, count))
         {
            printf ("\n Error in Unmapping the buffer\n");
         }

         close(mem_fd);
      }
   } while (0);
   
#endif
   (void)rfsa_lock_leave(rfsa_qmi_lock);

   return ret;
}


int32_t rfsa_get_buff_addr (rfsa_get_buff_addr_req_msg_v01 *get_buff_addr_ptr, 
                           rfsa_get_buff_addr_resp_msg_v01 *get_buff_addr_ret_ptr)
{
   qmi_client_error_type         rc;

   (void)rfsa_lock_enter(rfsa_qmi_lock);
   rc = qmi_client_send_msg_sync (client,
                                  QMI_RFSA_GET_BUFF_ADDR_REQ_MSG_V01,
                                  (void *)get_buff_addr_ptr, sizeof(rfsa_get_buff_addr_req_msg_v01),
                                  (void *)get_buff_addr_ret_ptr, sizeof(rfsa_get_buff_addr_resp_msg_v01),
      0);
   (void)rfsa_lock_leave(rfsa_qmi_lock);

   if (rc != QMI_NO_ERR)
   {
      LOGE1("RFS Alloc_buff Failed: Status:%d\n", rc);
      (void)qmi_client_release(client);
      return RFSA_EFAILED;
   }

   return RFSA_EOK;
}


int32_t rfsa_iovec_read (rfsa_iovec_file_read_req_msg_v01 *iovec_read_ptr, 
                        rfsa_iovec_file_read_resp_msg_v01 *iovec_read_ret_ptr)
{
   qmi_client_error_type         rc;

   (void)rfsa_lock_enter(rfsa_qmi_lock);
   
   rc = qmi_client_send_msg_sync (client,
                        QMI_RFSA_IOVEC_FILE_READ_REQ_MSG_V01,
                        (void *)iovec_read_ptr, sizeof(rfsa_iovec_file_read_req_msg_v01),
                        (void *)iovec_read_ret_ptr, sizeof(rfsa_iovec_file_read_resp_msg_v01),
      0);
   (void)rfsa_lock_leave(rfsa_qmi_lock);

   if (rc != QMI_NO_ERR)
   {
      LOGE1("RFS iov_read Call Failed: Status:%d\n", rc);
      (void)qmi_client_release(client);
      return RFSA_EFAILED;
   }

   return RFSA_EOK;
}


int32_t rfsa_iovec_write (rfsa_iovec_file_write_req_msg_v01 *iovec_write_ptr, 
                         rfsa_iovec_file_write_resp_msg_v01 *iovec_write_ret_ptr)
{
   qmi_client_error_type         rc;

   (void)rfsa_lock_enter(rfsa_qmi_lock);

   rc = qmi_client_send_msg_sync(client,
                        QMI_RFSA_IOVEC_FILE_WRITE_REQ_MSG_V01,
                        (void *)iovec_write_ptr, sizeof(rfsa_iovec_file_write_req_msg_v01),
                        (void *)iovec_write_ret_ptr, sizeof(rfsa_iovec_file_write_resp_msg_v01),
      0);
   (void)rfsa_lock_leave(rfsa_qmi_lock);

   if (rc != QMI_NO_ERR)
   {
      LOGE1("RFS iov_write Call Failed: Status:%d\n", rc);
      (void)qmi_client_release(client);
      return RFSA_EFAILED;
   }

   return RFSA_EOK;
}
