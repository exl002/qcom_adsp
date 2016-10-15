/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N   M O D U L E

GENERAL DESCRIPTION
  This module contains the Main procedure.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1992-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.adsp/2.2/debugtools/task/src/coremain.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

// *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK*
// *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK*

#ifndef COREMAIN_EXCLUDE_KERNEL_HACK
#ifndef COREMAIN_EXCLUDE_MAIN
#ifdef GUEST_OS_MULTI_PD_FLAVOR
#ifdef LPASS_SENSOR_IMG

// THIS IS NOT AN DEBUGTOOLS DELIVERABLE. DEBUGTOOLS TEAM DOES NOT
// SUPPORT NOR MAINTAIN KERNEL CONFIGURATION THROUGH GLOBAL VARIABLES.
// THE KERNEL MUST CONFIGURE THEIR SOFTWARE PACKAGES APPROPRIATELY PER
// IMAGE. PUT THIS HACK IN $SUB$$MAIN$() WHICH CALLS $SUPER$$MAIN()
// WHICH IS THE MAIN() IN THIS MODULE.

 unsigned int heapSize=0x40000;   // The heap size needs to be defined for user pd in main. The heapsize is weak symbol from qurt
                                  // This needs to be defined until xml support for user pd comes into picture, as the kernel team
                                  // does not manage their deliverable configuration.

#endif // LPASS_SENSOR_IMG
#endif // GUEST_OS_MULTI_PD_FLAVOR
#endif // COREMAIN_EXCLUDE_MAIN
#endif

// *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK*
// *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK* *HACK*

#include "rcinit_dal.h"
#include "qurt.h"

void err_bootstrap_exception_handler(void); // forward reference
void rcinit_bootstrap_rcinit_task(void); // forward reference

/*===========================================================================
FUNCTION COREMAIN_MAIN

DESCRIPTION
  Bootstrap the context for error processing
  Bootstrap the context for rcinit processing

DEPENDENCIES
  None

RETURN VALUE
  None, this routine returns at the completion of rcinit startup processing
  to facilitate other images being able to initialize corebsp and then regain
  control of the context following the rcinit startup process.

SIDE EFFECTS
  None

===========================================================================*/
void coremain_main(void)
{
   DALSYS_InitMod(NULL);               // no error return

   err_bootstrap_exception_handler();  // creates context for error processing

   rcinit_bootstrap_rcinit_task();     // creates context for rcinit processing
}

#ifndef COREMAIN_EXCLUDE_MAIN

#ifndef COREMAIN_EXCLUDE_PRINTF
#include "stdio.h" // This is included for printf calls. Must be removed with printf() calls
#endif

/*===========================================================================
FUNCTION COREMAIN_PD_SPAWN

DESCRIPTION
  QURT based image spawning. Compile time registrations. Future extenstion to
  this internal function *is not* supported. All QURT spawn calls to start images
  must be introduced by the owning tech area as an init function call to the
  rcinit startup process. Images spawned should be included as part of rcinit
  group 7 processing by the tech area.

DEPENDENCIES
  Yes. Tech team analysis upon when their image should be started by the normal
  rcinit process. Suggestion is to utilize rcinit group 7.

RETURN VALUE
  None

SIDE EFFECTS
  Yes. Tech team analysis upon when their image should be started by the normal
  rcinit process. Suggestion is to utilize rcinit group 7.

===========================================================================*/
static void coremain_pd_spawn(void)
{
#ifdef GUEST_OS_MULTI_PD_FLAVOR   //This is defined in task.scons based on USES flag which will be defined for Guest OS in multi pd flavor

   int cid;

   char* image_name[] =
   {
      SENSOR_IMG_NAME,
#ifdef SPAWN_AUDIO_USERPD
      AUDIO_IMG_NAME,
#endif
      NULL
   };

   char** image_p;

   for (image_p = image_name; NULL != *image_p; image_p++)
   {
      cid = qurt_spawn(*image_p);

#ifndef COREMAIN_EXCLUDE_PRINTF
      printf("Image %s spawned with ID = %d\n", *image_p, cid); // Trace32 console output diagnostics, must be removed
#endif
   }

#endif //GUEST_OS_MULTI_PD_FLAVOR
}

// void template_spawn_fn(void) // call this from rcinit group 7
// {
//  int cid = qurt_spawn("TEMPLATE");
// }

/*===========================================================================
FUNCTION MAIN

DESCRIPTION
  Bootstrap the context for core_main through call to rex_init

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int main(void)
{
   DALSYSEventHandle hEvent;

   coremain_main(); // when main is not delivered here, this is the entry to start cbsp

   coremain_pd_spawn(); // qurt image spawning, compile time registered, not a supported interface.

   // Block Forever

   if ((DAL_SUCCESS == DALSYS_EventCreate(DALSYS_EVENT_ATTR_NORMAL, &hEvent, NULL)) &&
       (DAL_SUCCESS == DALSYS_EventWait(hEvent)))
   {
      for (;;) /* NULL */ ;
   }

   else
   {
      for (;;) /* NULL */ ;
   }

   /* NOTREACHED */

   return (0);
}

#endif
