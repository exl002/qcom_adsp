/******************************************************************************
 * rfsa_client.c
 *
 * Remote File System Access, client API implementations
 *
 * Copyright (c) 2012-2013
 * Qualcomm Technology Inc.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/storage/rfa_client/src/rfsa_client.c#1 $
  $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2013-02-25   rh      Replace strncpy with strlcpy
2013-02-14   rh      Make call to qmi_init non blocking
2012-11-28   rh      Add call to deinit routine to the QMI layer
2012-10-11   rh      Adding opening file with checking on server side
2012-02-22   rh      Initial checkin
=============================================================================*/

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#ifdef QDSP 
#include "qmemory.h"
#endif

#include "rfsa_common.h"
#include "rfsa_client.h"
#include "rfsa_vtl_client.h"
#include "rfsa_misc.h"

#include "remote_filesystem_access_v01.h"


/*****************************************************************************
* Defines                                                                   *
****************************************************************************/

#define RFSA_CLIENT_LEN_SHARED_MEM   128
#define RFSA_MAX_OPEN_FILES          64

#define EOK                          0
/*****************************************************************************
* Definitions                                                               *
****************************************************************************/

typedef struct rfsa_info_client {
   int32_t     handle;
   char        filename[RFSA_MAX_FILE_PATH_V01];
   int         open;
   int32_t     pos;
}rfsa_info_t;


/*****************************************************************************
* Variables                                                                 *
****************************************************************************/
uint8_t rfsa_client_init_done = 0;
char   rfsa_client_shared_mem[RFSA_CLIENT_LEN_SHARED_MEM];
struct rfsa_info_client rfsa_open_files[RFSA_MAX_OPEN_FILES];

rfsa_lock_t rfsa_client_lock;

/*****************************************************************************
* Implementations                                                           *
****************************************************************************/


int32_t rfsa_get_new_index_file (void)
{
   int32_t i, index = -1;

   (void)rfsa_lock_enter(rfsa_client_lock);

   for(i = 0; i < RFSA_MAX_OPEN_FILES; i++)
   {
      if(rfsa_open_files[i].open == 0)
      {
         rfsa_open_files[i].open = 1;
         index = i;
         break;
      }
   }

   (void)rfsa_lock_leave(rfsa_client_lock);

   return index;
}


int32_t rfsa_check_index_file(int32_t index_file)
{
   if((index_file < 0) || (index_file >= RFSA_MAX_OPEN_FILES))
   {
      LOGE1("Invalid handle: %d", (int)index_file);
      return 0;
   }

   if(rfsa_open_files[index_file].open == 0)
   {
      LOGE1("Invalid handle: %d", (int)index_file);
      return 0;
   }

   return 1;
}

rfsa_info_t *rfsa_get_index_file (int32_t index_file)
{
   return &(rfsa_open_files[index_file]);
}


int rfsa_client_init(void)
{
   if(rfsa_client_init_done)
   {
      LOGE("rfsa_client_init was already called!\n"); 
      return -1;
   }

   memset(rfsa_open_files, 0, RFSA_MAX_OPEN_FILES*sizeof(struct rfsa_info_client));

   (void)rfsa_lock_create(&rfsa_client_lock);

   int rc = (rfsa_vtl_client_init() == RFSA_EFAILED) ? -1 : 0;
   if (rc == -1)
   {
      return rc;
   }

   rfsa_client_init_done = 1;
   return 0;
}


int rfsa_client_deinit(void)
{
   if(!rfsa_client_init_done)
   {
      LOGE("rfsa_client_init wasn't called!");
      return -1;
   }

   (void)rfsa_lock_destroy(rfsa_client_lock);

   rfsa_client_init_done = 0;
   return (rfsa_vtl_client_deinit() == RFSA_EFAILED) ? -1 : 0;
}


int32_t rfsa_open(const char *filename, int flags)
{
   rfsa_file_stat_req_msg_v01 file_stat;
   rfsa_file_stat_resp_msg_v01 file_stat_resp;
   rfsa_file_create_req_msg_v01 file_creat;
   rfsa_file_create_resp_msg_v01 file_creat_resp;
   int32_t index_file;
   int32_t rc;
   int32_t fnlen;
   rfsa_info_t *pinfo;

   if(!rfsa_client_init_done)
   {
      rc = rfsa_client_init();
      if (rc != 0) 
      {
         return rc;
      }
   }

   if(filename == NULL)
   {
      LOGE("Error on rfsa_client_file_open, filename is NULL!");
      return -1;
   }

   if ((flags & O_ACCMODE) == O_RDONLY) 
   {
      // Only do pending check if the client did not set the O_NO_OPEN_CHK flag
      if (!(flags & O_NO_OPEN_CHK)) 
      {
         fnlen = strlcpy (file_stat.filename, filename, RFSA_MAX_FILE_PATH_V01);
         if (fnlen >= RFSA_MAX_FILE_PATH_V01) 
         {
            LOGE("Filename too long!");
            return -1;
         }
         rc = rfsa_vtl_stat(&file_stat, &file_stat_resp);
         if (rc == RFSA_EFAILED || file_stat_resp.resp.result != QMI_RESULT_SUCCESS_V01 || 
             file_stat_resp.resp.error != QMI_ERR_NONE_V01 ||
             !(file_stat_resp.flags & RFSA_ACCESS_FLAG_READ_V01))
         {
            LOGE("STAT call failed on the server side!");
            return -1;
         }
      }
   }
   else if ((flags & O_ACCMODE) == O_WRONLY)
   {
      fnlen = strlcpy (file_creat.filename, filename, RFSA_MAX_FILE_PATH_V01);
      if (fnlen >= RFSA_MAX_FILE_PATH_V01) 
      {
         LOGE("Filename too long!");
         return -1;
      }
      file_creat.flags = 0;
      file_creat.flags = RFSA_ACCESS_FLAG_WRITE_V01;
      if ((flags & O_CREAT) == O_CREAT)
      {
         file_creat.flags |= RFSA_ACCESS_FLAG_CREATE_V01;
      }
      rc = rfsa_vtl_fcreat(&file_creat, &file_creat_resp);
      if (rc == RFSA_EFAILED || file_creat_resp.resp.result != QMI_RESULT_SUCCESS_V01 || 
          file_creat_resp.resp.error != QMI_ERR_NONE_V01)
      {
         LOGE("CREATE call failed on the server side!");
         return -1;
      }
   }
   else
   {
      LOGE("Error open mode not supported!");
      return -1;
   }

   // No error in the process, get a new file index to pass back to the client
   index_file = rfsa_get_new_index_file ();
   if(index_file == -1)
   {
      LOGE("Too many open files!");
      return -1;
   }

   pinfo = rfsa_get_index_file (index_file);
   fnlen = strlcpy (pinfo->filename, filename, RFSA_MAX_FILE_PATH_V01);
   if (fnlen >= RFSA_MAX_FILE_PATH_V01) 
   {
      LOGE("Filename too long!");
      return -1;
   }
   pinfo->pos = 0;

   return ++index_file;
}


int32_t rfsa_close(int32_t handle)
{
   int32_t index_file = handle - 1;
   rfsa_info_t *pinfo;

   if(!rfsa_check_index_file(index_file))
   {
      return EOF;
   }

   pinfo = rfsa_get_index_file (index_file);
   (void)rfsa_lock_enter(rfsa_client_lock);
   pinfo->open = 0;
   (void)rfsa_lock_leave(rfsa_client_lock);

   return 0;
}


int rfsa_seek(int32_t handle, int32_t count, int32_t way)
{
   int32_t index_file = handle - 1;
   rfsa_info_t *pinfo;

   if(!rfsa_check_index_file(index_file))
   {
      return -1;
   }

   pinfo = rfsa_get_index_file (index_file);
   if (way == RFSA_SEEK_SET) {
      pinfo->pos = count;
   } else if (way == RFSA_SEEK_CUR) {
      pinfo->pos += count;
   }
   else {
      return -1;
   }

   return EOK;
}


long rfsa_ftell(int32_t handle)
{
   int32_t index_file = handle - 1;
   rfsa_info_t *pinfo;

   if(!rfsa_check_index_file(index_file))
   {
      return (long)-1;
   }

   pinfo = rfsa_get_index_file (index_file);
   return pinfo->pos;
}


long rfsa_flen(int32_t handle)
{
   rfsa_file_stat_req_msg_v01 file_stat;
   rfsa_file_stat_resp_msg_v01 file_stat_resp;
   int32_t index_file = handle - 1;
   rfsa_info_t *pinfo;
   int32_t rc;
   int32_t fnlen;

   if(!rfsa_client_init_done)
   {
      rc = rfsa_client_init();
      if (rc != 0) 
      {
         return rc;
      }
   }

   if(!rfsa_check_index_file(index_file))
   {
      return (long)-1;
   }

   pinfo = rfsa_get_index_file (index_file);
   fnlen = strlcpy(file_stat.filename, pinfo->filename, RFSA_MAX_FILE_PATH_V01);
   if (fnlen >= RFSA_MAX_FILE_PATH_V01) 
   {
      return -1;
   }
   rc = rfsa_vtl_stat(&file_stat, &file_stat_resp);

   // Check for error
   if (rc == RFSA_EFAILED || file_stat_resp.resp.result != QMI_RESULT_SUCCESS_V01 || 
       file_stat_resp.resp.error != QMI_ERR_NONE_V01 || !file_stat_resp.size_valid)
   {
      return -1;
   }
   else
   {
      return (file_stat_resp.size);
   }
}


int32_t rfsa_read(int32_t handle, char *buffer, int size)
{
   int32_t rc;
   int32_t fnlen;
   int32_t crt_pos, size_to_read;
   rfsa_file_read_req_msg_v01  file_read;
   rfsa_file_read_resp_msg_v01 file_read_ret;
   int32_t total_read;
   int32_t index_file = handle - 1;
   rfsa_info_t *pinfo;

   //LOGI1("CLIENT SIDE --- rfsa_read start: Size %d\n", size);

   if(!rfsa_client_init_done)
   {
      rc = rfsa_client_init();
      if (rc != 0) 
      {
         return rc;
      }
   }

   if(!rfsa_check_index_file(index_file))
   {
      return 0;
   }

   pinfo = rfsa_get_index_file (index_file);

   size_to_read = size;
   crt_pos = pinfo->pos;
   total_read = 0;

   fnlen = strlcpy(file_read.filename, pinfo->filename, RFSA_MAX_FILE_PATH_V01);
   if (fnlen >= RFSA_MAX_FILE_PATH_V01) 
   {
      return -1;
   }

   while(size_to_read > 0)
   {
      file_read.size = size_to_read;
      file_read.offset = crt_pos;

      memset(&file_read_ret, 0, sizeof(rfsa_file_read_resp_msg_v01));
      file_read_ret.data.buffer = (uint32_t)(buffer + total_read);

      rc = rfsa_vtl_read(&file_read, &file_read_ret);
      if(rc != RFSA_EOK)
      {
         LOGE("rfsa_read failed\n");
         total_read = -1;
         break;
      }

      crt_pos += file_read_ret.data.count;
      size_to_read -= file_read_ret.data.count;
      total_read += file_read_ret.data.count;

      //LOGI2("rfsa_read position: %d Rtn Data Cnt: %d\n", (int)crt_pos, (int)file_read_ret.data.count);

      // Read terminates when the return count is 0
      if ((int32_t)file_read_ret.data.count == 0)
      {
         break;
      }
   }

   // Record the last read position
   pinfo->pos = crt_pos;

   return total_read;
}


