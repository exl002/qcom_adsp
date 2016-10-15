/*========================================================================

  Copyright (c) 2012 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
  ====================================================================== */
#ifndef FASTRPC_PROCESS_GROUP_H
#define FASTRPC_PROCESS_GROUP_H
#include "AEEStdDef.h"
#include "qurt_types.h"
#include "qurt_thread.h"
#include "adsp_pls.h"
#include "adsp_mmap.h"

struct cache_entry {
   qurt_mem_cache_mode_t in;
   qurt_mem_cache_mode_t rout;
};

int fastrpc_get_current_pid(uint32* po);
int fastrpc_group_notify(void);
int fastrpc_thread_notify(qurt_thread_t tidQ);
int fastrpc_thread_wait(void);
boolean fastrpc_in_rpc(void);
int fastrpc_dsp_wait_init_complete(void);

#define ADSP_MMAP_FIXED_ADDR  1
#endif
