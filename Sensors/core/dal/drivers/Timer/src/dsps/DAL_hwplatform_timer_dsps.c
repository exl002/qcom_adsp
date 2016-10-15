/*==============================================================================

FILE:      DAL_hwplatform_timer_dsps.c

DESCRIPTION: This file implements a HW platform specific Timer DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/core/dal/drivers/Timer/src/dsps/DAL_hwplatform_timer_dsps.c#1 $  */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-12-07  pg   Changes required to pull in DALTimer.c#6. These changes
                   resolve the bug DAL timer ISR which would not handle the
                   case when timer tick has not incremented since the match that
                   triggered the timer interrupt.
  2010-10-21  hm   Fixed compiler warning (uninitialized variable)
  2010-10-04  hm   Keep hardware timer enabled even when DalTimer cancels all
                   timers. Removed redundant loop in platform_timer_hw_get_count() 
  2010-09-28  hm   Disable the hardware timer when all timers are cancelled. 
  2010-07-19  ag   Initial version

============================================================================*/



#include "dalconfig.h"
#include "DALDeviceId.h"
#include "DAL_hwplatform_timer.h"
#include "DALStdErr.h"
#include "DalDevice.h"
#include "DALSys.h"
#include "msm.h"
#include "tramp.h"
#include "HALpic.h"
#include "msmhwioreg.h"

#ifdef DAL_IMAGE_CONFIG_H
#include DAL_IMAGE_CONFIG_H
#else
#error "Missing DAL image config file"
#endif

#define DSPS_INVALID_INTERRUPT_ID 0xFFFFFFFF

typedef struct
{
   uint32 frequency;
   uint32 interrupt;
   uint32 max_match_val;
} HWTimer;

typedef enum
{
  SLEEP_TIMER0,
  SLEEP_TIMER1
} HWTimerID;

static const HWTimer hw_timers[DAL_CONFIG_NUM_HW_TIMERS] =
{
   // Timer0 - PCXO clk
   /*
   {
      24580000,
      (uint32)TRAMP_GP_TIMER_IRQ,
      0xFFFFFFFF
   },
   {
      24580000,
      (uint32)TRAMP_DEBUG_TIMER_IRQ,
      0xFFFFFFFF
   }, 
   */ 
   // Timer1 - 32.768 KHz clk
   {
      32768,
      (uint32)TRAMP_SLEEP_TIMER_0_IRQ,
      0xF0000000 
   },
   {
      32768,
      (uint32)TRAMP_SLEEP_TIMER_1_IRQ,
      0xF0000000 
   }
};

static DALDEVICEHANDLE hStaticIntrCtrl = NULL;

void platform_timer_hw_set_enable(uint32 timerId, boolean enabled)
{
   if(enabled)
   {
     switch (timerId)
     {
       /*
       case TCXO_TIMER0:
       case TCXO_TIMER1:
         HWIO_OUT(PPSS_TIMER0_CLK_CTL, HWIO_FMSK(PPSS_TIMER0_CLK_CTL, CLK_BRANCH_ENA) | 
                  HWIO_FMSK(PPSS_TIMER0_CLK_CTL, PXO_SRC_BRANCH_ENA));
         HWIO_OUTI(TCXO_TMRn_CONTROL, timerId, HWIO_FMSK(TCXO_TMRn_CONTROL, EN));
         break;
       */
       case SLEEP_TIMER0:
         HWIO_OUT(PPSS_TIMER1_CLK_CTL, HWIO_FMSK(PPSS_TIMER1_CLK_CTL, CLK_BRANCH_ENA));
         HWIO_OUTI(SLP_TMRn_CONTROL, 0, HWIO_FMSK(SLP_TMRn_CONTROL, EN));
         break;
       case SLEEP_TIMER1:
         HWIO_OUT(PPSS_TIMER1_CLK_CTL, HWIO_FMSK(PPSS_TIMER1_CLK_CTL, CLK_BRANCH_ENA));
         HWIO_OUTI(SLP_TMRn_CONTROL, 1, HWIO_FMSK(SLP_TMRn_CONTROL, EN));
         break;
       default:
         break;
     }
   }
   /* 
   else
   {
      
     switch (timerId)
     {
        case TCXO_TIMER0:
        case TCXO_TIMER1:
          HWIO_OUTI(TCXO_TMRn_CLEAR_CNT, timerId, 1);
          HWIO_OUTF(PPSS_TIMER0_CLK_CTL, CLK_BRANCH_ENA, 0);
         break;
        case SLEEP_TIMER0:
        case SLEEP_TIMER1:
          HWIO_OUTI(SLP_TMRn_CLEAR_CNT, (timerId - SLEEP_TIMER0), 1);
          HWIO_OUTF(PPSS_TIMER1_CLK_CTL, CLK_BRANCH_ENA, 0);
         break;
       default:
         break;
     }   
   } 
   */
} /* platform_timer_hw_set_enable */

DALResult platform_timer_hw_clear_interrupt(uint32 timerId)
{
  switch (timerId)
  {
    /*
    case TCXO_TIMER0:
    case TCXO_TIMER1:
      HWIO_OUTI(TCXO_TMRn_CLEAR_INT, timerId, 1);
      break;
    */
    case SLEEP_TIMER0:
      HWIO_OUTI(SLP_TMRn_CLEAR_INT, 0, 1);
      break;
    case SLEEP_TIMER1:
      HWIO_OUTI(SLP_TMRn_CLEAR_INT, 1, 1);
      break;
    default:
      break;
  }
   return DAL_SUCCESS;
}

void platform_timer_hw_interrupt_trigger(uint32 timerId)
{
   uint32 intrId = hw_timers[timerId].interrupt;

   if (intrId != DSPS_INVALID_INTERRUPT_ID && hStaticIntrCtrl)
   {
      (void)DalInterruptController_InterruptTrigger(hStaticIntrCtrl, intrId);
   }
}

void platform_timer_hw_set_matchval(uint32 timerId, uint32 match_val)
{
   switch (timerId)
   {
     /*
     case TCXO_TIMER0:
     case TCXO_TIMER1:
       HWIO_OUTI(TCXO_TMRn_MATCH_VAL, timerId, match_val);
       break;
     */
     case SLEEP_TIMER0:
       HWIO_OUTI(SLP_TMRn_MATCH_VAL, 0, match_val);
       break;
     case SLEEP_TIMER1:
       HWIO_OUTI(SLP_TMRn_MATCH_VAL, 1, match_val);
       break;
     default:
       break;
   }
} /* platform_timer_hw_set_matchval */

uint32 platform_timer_hw_get_count(uint32 timerId)
{
   uint32 val = 0;
   uint32 prev_val;

   switch (timerId)
   {
     /*
     case TCXO_TIMER0:
     case TCXO_TIMER1:
       val = HWIO_INI(TCXO_TMRn_COUNT_VAL, timerId);
       do
       {
          prev_val = val;
          val = HWIO_INI(TCXO_TMRn_COUNT_VAL, timerId);
       } while (val == prev_val);
       break;
     */
     case SLEEP_TIMER0:
       val = HWIO_INI(SLP_TMRn_COUNT_VAL, 0);
       /*
       do
       {
          prev_val = val;
          val = HWIO_INI(SLP_TMRn_COUNT_VAL, 0);
       } while (val == prev_val);
       */
       break;
     case SLEEP_TIMER1:
       val = HWIO_INI(SLP_TMRn_COUNT_VAL, 1);
       do
       {
          prev_val = val;
          val = HWIO_INI(SLP_TMRn_COUNT_VAL, 1);
       } while (val == prev_val);
       break;
     default:
       break;
   }
   return val;
} /* platform_timer_hw_get_count */

DALResult platform_timer_hw_init
(
   uint32 timerId,
   const DALISR isr,
   void *pTimerDevCtxt,
   uint32 *max_match_val
)
{
   if (!hStaticIntrCtrl)
   {
      DAL_InterruptControllerDeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                                          &hStaticIntrCtrl);
   }

   *max_match_val = hw_timers[timerId].max_match_val;

   platform_timer_hw_set_enable(timerId, TRUE );
   // check if intrId is 0xFFFFFFFF meaning that this HW timer cannot do ISR
   if (hw_timers[timerId].interrupt != DSPS_INVALID_INTERRUPT_ID &&
       DAL_SUCCESS != DalInterruptController_RegisterISR(hStaticIntrCtrl,
                         hw_timers[timerId].interrupt, isr, pTimerDevCtxt,
                         DALINTRCTRL_ENABLE_DEFAULT_SETTINGS))
   {
      return DAL_ERROR;
   }
   return DAL_SUCCESS;
} /* platform_timer_hw_init */

void platform_timer_hw_deinit(uint32 timerId)
{
   if (hStaticIntrCtrl)
   {
      DalInterruptController_Unregister(hStaticIntrCtrl,
                                        hw_timers[timerId].interrupt);
      hStaticIntrCtrl = NULL;
   }
   platform_timer_hw_set_enable(timerId, FALSE);
} /* platform_timer_hw_deinit */

uint32 platform_timer_hw_get_frequency(uint32 timerId)
{
   return hw_timers[timerId].frequency;
} /* platform_timer_hw_get_frequency */

// Stub function .. not used in dsps at this point.
void platform_timer_hw_update_offset(uint32 timerId)
{
   (void)timerId;
}
