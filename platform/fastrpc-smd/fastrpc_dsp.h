/*========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
  ====================================================================== */
#ifndef FASTRPC_DSP_H
#define FASTRPC_DSP_H

#include "qurt_fastint.h"
#include "qurt_types.h"
#include "qurt_memory.h"
#include "qurt_thread.h"
#include "qurt_sem.h"
#include "qurt_mutex.h"
//#include "q6protos.h"
#include "dbgbuf.h"
#include "AEEStdDef.h"

#define PAGE_SIZE  0x1000
#define PAGE_MASK  ~(PAGE_SIZE-1)

static __inline uint32 buf_page_start(void *buf) {
   uint32 start = (uint32)buf & PAGE_MASK;
   return start;
}

static __inline uint32 buf_page_offset(void *buf) {
   uint32 offset = (uint32)buf & (PAGE_SIZE-1);
   return offset;
}

static __inline uint32 buf_page_size(uint32 size) {
   uint32 sz = (size + (PAGE_SIZE - 1)) & PAGE_MASK;
   return sz;
}

#endif
