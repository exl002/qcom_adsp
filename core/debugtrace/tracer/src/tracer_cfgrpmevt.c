/*===========================================================================
  FILE: tracer_cfgrpmevt.c

  OVERVIEW:     Modem-only image; wrapper to configure SW events on RPM

  DEPENDENCIES:

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  $Header: //components/rel/core.adsp/2.2/debugtrace/tracer/src/tracer_cfgrpmevt.c#1 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "tracer_cfgrpmevt.h"
#include "rpm_swevent.h"

/*---------------------------------------------------------------------------
 * Externalized Function Definitions
 * ------------------------------------------------------------------------*/
void _rpm_swevent_cfg_send(uint32 setting, uint32 type, uint64 bitmask)
{
   rpm_swevent_cfg_send(setting, type, bitmask);
   return;
}

