/*
 * Copyright (c) 2012-2013 QUALCOMM Technologies Inc. All Rights Reserved.
 * Qualcomm Technologies Confidential and Proprietary
 *
 */
#ifndef FASTRPC_INTERNAL_H
#define FASTRPC_INTERNAL_H

#include "remote.h"
#include "verify.h"

#define FASTRPC_IOCTL_INVOKE        _IOWR('R', 1, struct fastrpc_ioctl_invoke)
#define FASTRPC_IOCTL_MMAP          _IOWR('R', 2, struct fastrpc_ioctl_mmap)
#define FASTRPC_IOCTL_MUNMAP        _IOWR('R', 3, struct fastrpc_ioctl_munmap)
#define FASTRPC_IOCTL_INVOKE_FD     _IOWR('R', 4, struct fastrpc_ioctl_invoke_fd)
#define FASTRPC_IOCTL_SETMODE       _IOWR('R', 5, uint32)

#define DEVICE_NAME "adsprpc-smd"

#if !(defined __qdsp6__) && !(defined __hexagon__)
static __inline uint32 Q6_R_cl0_R(uint32 num) {
   int ii;
   for(ii = 31; ii >= 0; --ii) {
      if(num & (1 << ii)) {
         return 31 - ii;
      }
   }
   return 0;
}
#else
#include "hexagon_protos.h"
#endif


struct fastrpc_ioctl_invoke {
   remote_handle handle;
   uint32 sc;
   remote_arg* pra;
};

struct fastrpc_ioctl_invoke_fd {
	struct fastrpc_ioctl_invoke inv;
	int *fds;
};

struct fastrpc_ioctl_munmap {
	uint32 vaddrout;	   /* optional virtual address, if non zero, dsp will use vaaddrin */
	int  size;		      /* size */
};


struct fastrpc_ioctl_mmap {
   int fd;           /* ion handle */
	uint32 flags;	   /* flags to map with */
	uint32 vaddrin;	/* virtual address */
	int  size;		   /* size */
	uint32 vaddrout;	/* dsps virtual address */
};



#define FASTRPC_SMD_GUID "fastrpcsmd-apps-dsp"

struct smq_null_invoke {
   struct smq_invoke_ctx* ctx;   //! invoke caller context
   remote_handle handle;         //! handle to invoke
   uint32 sc;                    //! scalars structure describing the rest of the data
};

typedef uint32 smq_invoke_buf_phy_addr;

struct smq_phy_page {
   uint32 addr; //! physical address
   uint32 size; //! size
};

struct smq_invoke_buf {
   int num;
   int pgidx;
};

struct smq_invoke {
   struct smq_null_invoke header;
   struct smq_phy_page page;     //! remote arg and list of pages address
};

struct smq_msg {
   uint32 pid;
   uint32 tid;
   struct smq_invoke invoke;
};

struct smq_invoke_rsp {
   struct smq_invoke_ctx* ctx;   //! invoke caller context
   int nRetVal;                  //! invoke return value
};

static __inline struct smq_invoke_buf* smq_invoke_buf_start(remote_arg *pra, uint32 sc) {
   int len = REMOTE_SCALARS_LENGTH(sc);
   return (struct smq_invoke_buf*)(&pra[len]);
}

static __inline struct smq_phy_page* smq_phy_page_start(uint32 sc, struct smq_invoke_buf* buf) {
   int nTotal =  REMOTE_SCALARS_INBUFS(sc) + REMOTE_SCALARS_OUTBUFS(sc);
   return (struct smq_phy_page*)(&buf[nTotal]);
}

//! size of the out of band data
static __inline int smq_data_size(uint32 sc, int nPages) {
   struct smq_invoke_buf* buf = smq_invoke_buf_start(0, sc);
   struct smq_phy_page* page = smq_phy_page_start(sc, buf);
   return (int)(&(page[nPages]));
}

#endif // FASTRPC_INTERNAL_H
