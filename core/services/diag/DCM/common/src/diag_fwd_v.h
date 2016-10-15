#ifndef DIAG_FWD_V_H
#define DIAG_FWD_V_H
/*==========================================================================

                     Diagnostic Fwd Task Header File

Description
  Global Data declarations for the diag_fwd_task.

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                         Edit History

      $Header: //components/rel/core.adsp/2.2/services/diag/DCM/common/src/diag_fwd_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/10   SG      Increased the diag_fwd_task stack size to 16k
05/25/10   JV      Added comments
04/06/10   JV      Created
===========================================================================*/

#include "osal.h"

#define DIAG_FWD_STACK_SIZE 2048  /* type is unsigned long long, so stack size will be 16K */

#define DIAG_TX_SLAVE_SIG           0x00080000    /* Signal to process Diag traffic
                                                   generated at slave Processors */

#define DIAG_FWD_RPT_TIMER_SIG      0x02000000    /* timer signal for watchdog
                                                   for the diag_fwd_task */

#if defined(FEATURE_WINCE) 
    #include "diag_shared_i.h"
    #define DIAG_FWD_PRI (diagdiag_get_diag_task_pri() + 1)
#elif defined(DIAG_QDSP6_APPS_PROC)
	#include "diagstub.h"
	#define DIAG_FWD_PRI (DIAG_PRI + 1)
#else
	#define DIAG_FWD_PRI (DIAG_PRI - 1);
#endif

extern osal_tcb_t diag_fwd_task_tcb;


/*===========================================================================

FUNCTION DIAG_FWD_TASK_START

DESCRIPTION
  This function contains the diagnostic fwd task. This task processes TX traffic
  from the slave processor.

  This procedure does not return.

===========================================================================*/
void diag_fwd_task_start(uint32 params);

/*===========================================================================

FUNCTION DIAG_FWD_KICK_WATCHDOG

DESCRIPTION
  This function kicks the watchdog for the diagnostic fwd task

===========================================================================*/
void diag_fwd_kick_watchdog(void);

/*===========================================================================

FUNCTION DIAG_FWD_HANDLE_SIGS

DESCRIPTION
  This routine waits for the DIAG_TX_SLAVE_SIG and then processes the TX
  traffic from slave processors.

===========================================================================*/
void diag_fwd_handle_sigs(void);

#endif /*DIAG_FWD_V_H*/

