#ifndef DBGBUF_H
#define DBGBUF_H

#ifdef DBGBUF_PRINT

#include "AEEstd.h"
#include "AEEatomic.h"

static char gdbgbuf[64*1024];
static uint32 gdbgbufix = 0;
static uint32 gdbgbuflast;

#ifdef __qdsp6__
#define DBGBUF_FLUSH(buf,len) qurt_mem_cache_clean((qurt_addr_t)buf, len, QURT_MEM_CACHE_FLUSH_INVALIDATE, QURT_MEM_DCACHE)
#else
#define DBGBUF_FLUSH(buf,len) (void)0
#endif

#define DBGBUF(fmt, ...)\
   do {\
      uint32 sz = std_snprintf(0, 0, __FILE_LINE__ " " fmt "\n", ##__VA_ARGS__);\
      uint32 st;\
      int over = -1;\
      do {\
         uint32 prev;\
         do {\
            st = gdbgbufix;\
            prev = atomic_CompareAndExchange(&gdbgbufix, st + sz, st);\
         } while(prev != st);\
         st = st % sizeof(gdbgbuf);\
         over++;\
      } while(st + sz > sizeof(gdbgbuf));\
      if(over) {\
         std_memset(gdbgbuf, '\n', st);\
      }\
      std_snprintf(gdbgbuf + st , sizeof(gdbgbuf) - st, __FILE_LINE__ " " fmt "\n", ##__VA_ARGS__);\
      DBGBUF_FLUSH(gdbgbuf + st, sizeof(gdbgbuf) - st);\
      gdbgbuflast = st + sz;\
   } while(0)

#else //DBGBUF_PRINT

//#define DBGBUF(fmt, ...) printf(__FILE_LINE__ " " fmt "\n", ##__VA_ARGS__);

#define DBGBUF(fmt, ...) (void)0

#endif //DBGBUF_PRINT

#endif //DBGBUF_H
