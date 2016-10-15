/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the AMSS exception handler

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2007-2009        by Qualcomm Technologies, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/core.adsp/2.2/debugtools/err/src/err_exception_handler.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/08   tbg     Merged in changes to support FEATURE_QUARTZ_20
07/25/07   tbg     Initial verion (pulled code from mobile.c)
===========================================================================*/

#include <stdio.h>
#include "target.h"     
#include "customer.h"   
#include "comdef.h"
#include "armasm.h"
#include "err.h"
#include "errlog.h"
#include "msg.h"
#include "qurt_event.h"
#include "erri.h"
#include "tms_utils.h"



#define SWI_NUM_DIVIDE_BY_0     0xD0  /* SWI # used in __default_signal_handler
                                         for DivideByZero exception */
/* The following offset is calculated out from SP(stack pointer) to the saved 
   pointer/address in stack, which points to the next instruction after the
   instruction of <blx __rt_sdiv>. */
/* Note: === This might change if __rt_sdiv() changes. === */
#define SWI_DIVISION_OFFSET     0x14

typedef enum {
  EXCP_UNKNOWN,
  EXCP_SWI,
  EXCP_UNDEF_INST,
  EXCP_MIS_ALIGN,
  EXCP_PAGE_FAULT,
  EXCP_EXE_FAULT,
  EXCP_DIV_BY_0,
  EXCP_MAX_NUM
} exception_type;

char  exception_info[EXCP_MAX_NUM][14] = {
  "    :Excep  ",
  "    :No SWI ",
  "    :Undef  ",
  "    :MisAlgn",
  "    :PFault ",
  "    :XFault ",
  "    :DivBy0 ",
};

char              qxdm_dbg_msg[80];

extern coredump_type coredump;
void err_exception_handler( void );


#define MAX_FAILURE_COUNT 10

/*===========================================================================
FUNCTION err_exception_handler

DESCRIPTION
  Handle IPC from L4 Kernel when exceptions occur.

===========================================================================*/
void err_exception_handler(void)
{
  unsigned int     tid, ip, sp, badva, cause;
  union arch_coredump_union *p_regs=&coredump.arch.regs;
  unsigned int failure_count=0;
  int written_val = 0;
  
  for (;;)
  {
    /* Register self as Exception_Handler. */
    tid = qurt_exception_wait(&ip, &sp, &badva, &cause);

    if (-1==tid)
    {
      written_val = tms_utils_fmt((int8_t *)qxdm_dbg_msg, sizeof(qxdm_dbg_msg),
        "Failed to register with qurt_reg_error_handler: tid=%x", tid);
      written_val = written_val; /*added to resolve compiler error :unsused(and ignored) return written_val */
      MSG_FATAL("Failed to register with qurt_reg_error_handler: tid=%x", tid, 0, 0);
      failure_count++;
      if(failure_count >= MAX_FAILURE_COUNT)
      {
        ERR_FATAL("Failed to register with qurt_reg_error_handler",0,0,0);
      }
      continue;
    }

    p_regs->name.pc = ip;
    p_regs->name.sp = sp;
    p_regs->name.badva = badva;
    p_regs->name.ssr = cause;
    
    written_val = tms_utils_fmt((int8_t *)qxdm_dbg_msg, sizeof(qxdm_dbg_msg),
      "ExIPC: Exception recieved tid=%x inst=%x", tid, ip);
    written_val = written_val; /*added to resolve compiler error :unsused(and ignored) return written_val */
    /* enter critical section */
    err_fatal_lock();
    /* Halt other cores */
    ERR_FATAL_ENTER_SINGLE_THREADED_MODE();
    /* perform higher level error logging - no return */
    err_fatal_jettison_core ( 0, exception_info[EXCP_UNKNOWN], "Exception detected", 0, 0, 0);
  }
} /* end of err_exception_handler */



