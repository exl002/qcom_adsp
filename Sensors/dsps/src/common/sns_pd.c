/*=============================================================================
  @file sns_pd.c

  This file contains sensors PD initialization code.

*******************************************************************************
* Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved
* Qualcomm Technologies Confidential and Proprietary.
********************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/dsps/src/common/sns_pd.c#2 $ */
/* $DateTime: 2014/05/28 12:36:49 $ */
/* $Author: rpoliset $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-05-13   pk   Isolate enhanced batching to SSC_BATCH_ENHANCE switch
  2014-05-12   ad   Support enhanced batching
  2013-10-18  JHH   Remove timer and wait forever
  2013-08-22   PK   Increased SNS heap size for Hammerhead
  2013-07-26   VH   Eliminated compiler warnings
  2012-12-02   AG   Initial version
  2013-02-05   AG   Use RCInit, remove previous workaround
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdio.h>
#include <qurt.h>
#include "qurt_timer.h"
#include "qurt_elite.h"
#include "sns_init.h"

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/
/* Heap size to be defined by each User PD Image owner */
#ifdef SSC_BATCH_ENHANCE
    unsigned int heapSize=0x100000;
#else
    unsigned int heapSize=0x40000;
#endif

extern void coremain_main(void);

int main (void)
{
	qurt_signal_t foreverWaitSnsPD;

    printf("Inside the sensor Image Main function \n");
    printf("user pid is %d\n", qurt_getpid());

    /* Core Init for user PD */
    coremain_main();

    printf("Core Init for sensors image done\n");

    /* Init global state structure */
    qurt_elite_globalstate_init();

    /* Sensors Initialization */
    sns_init();

    qurt_signal_init(&foreverWaitSnsPD);
    /* Currently User Process exit is not supported and the
       User PD Images will have their exception handlers */
    while(1){
      qurt_signal_wait (&foreverWaitSnsPD,((unsigned int) 0xFFFFFFFF), ((unsigned int)QURT_SIGNAL_ATTR_WAIT_ALL)); // wait for ever
    }

	qurt_signal_destroy(&foreverWaitSnsPD);
    return 0; /* never reaches, no user exit handling yet */
}

