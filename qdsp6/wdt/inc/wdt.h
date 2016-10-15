#ifndef WDT_H
#define WDT_H

/* ========================================================================
   WDT header file

  *//** @file wdt.h
  This file contains data constructs for WDT`.

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  *//*====================================================================== */

/*-------------------------------------------------------------------------------------------*/
/* Version information:                                                                      */
/* $Header: //components/rel/dspcore.adsp/2.2/wdt/inc/wdt.h#5 $    */
/* $DateTime: 2013/06/19 12:03:09 $                                                          */
/* $Author: coresvc $                                                                       */
/*-------------------------------------------------------------------------------------------*/

/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   11/15/10    weshmawy       Creation.

   ========================================================================= */



int WDT_init(void);
void WDT_task(unsigned long int unused_param);
void WDT_timer_init(void);
void WDT_timer_pet(void);
void WDT_active(void);
void WDT_idle(void);
void WDT_trigger_BITE(void);
void WDT_set_BITE_busywait(void);

#endif /* WDT_H */
