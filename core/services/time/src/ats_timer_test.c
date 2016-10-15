/*=============================================================================

                Ats_Time_Test.c

GENERAL DESCRIPTION
      ATS Timer Apis Test Code that executes from Guest OS

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009 - 2013
      by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/services/time/src/ats_timer_test.c#4 $ 
$DateTime: 2013/09/11 08:59:47 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/20/13   ab      Added file
=============================================================================*/
#include "msg.h"
#include "rcinit.h"
#include "msg.h"
#include "qurt_timer.h"
#include "timetick.h"
#include "time_timetick.h"
  /* test purpose */
#include "qw.h"
#include "../../../core/kernel/qurt/libs/qurt/include/public/qurt_cycles.h"
#include "npa.h"
//#include "qurt_timer_defines.h"
#include "timer.h"
#include "timer_v.h"


typedef enum
{
    LPASS_TEST1 = 1,
    LPASS_TEST2=2,
    LPASS_TEST3=4,
}ats_timer_test_enum_type;

timer_type ats_test_timer;


#define TIMER_TEST_LOOPS 500


/*=============================================================================

FUNCTION TIMER_GET_TIMETICK_HANDLE

DESCRIPTION
 This function is called from timer_init to pass timetick handle to timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void ats_timer_test_task
(
  void
)
{
  unsigned int mask_fired=0;
  unsigned int i;
  qurt_anysignal_t ats_timer_test_cb_signal, ats_timer_test_signal;
    
  
  //wait on rcinit handshake
  rcinit_handshake_startup();       /* Note: This is only for tasks that register with rcinit */
  
  /*initialize the task signal*/
  qurt_anysignal_init (&ats_timer_test_signal);

  /*prime the task*/
  (void) qurt_anysignal_set( &ats_timer_test_signal,LPASS_TEST1 );
    
  //loop forever
  for(;;)
  {
      mask_fired = qurt_anysignal_wait (&ats_timer_test_signal, LPASS_TEST1 | LPASS_TEST2 | LPASS_TEST3);

      if( mask_fired & LPASS_TEST1) 
      {

        qurt_anysignal_init (&ats_timer_test_cb_signal);

        /*define a timer for 1 sec*/
        timer_def_osal(&ats_test_timer, NULL, TIMER_NATIVE_OS_SIGNAL_TYPE, &ats_timer_test_cb_signal, 0x1);

        /*set the timer for 1 sec*/
        timer_set_64(&ats_test_timer, 60, 0, T_SEC);
        
        qurt_anysignal_wait(&ats_timer_test_cb_signal, 0x1);

        MSG_HIGH("ATS initial timer fired",0,0,0);

        for(i=0; i < TIMER_TEST_LOOPS; i++ ) 
        {

          timer_set_64(&ats_test_timer, 1, 0, T_SEC);
          MSG_HIGH("ATS timer set for %dth time", i+1,0,0);

          timer_pause(&ats_test_timer);
          MSG_HIGH("ATS initial timer paused",0,0,0);

          qurt_timer_sleep(1000000);

          MSG_HIGH("ATS initial timer restarted",0,0,0);
          timer_resume(&ats_test_timer);

          qurt_anysignal_wait(&ats_timer_test_cb_signal, 0x1);

          MSG_HIGH("ATS initial timer fired",0,0,0);

  
        }
        MSG_HIGH("ats_timer_test is Done..",0,0,0);

        
      }
           
  }
}



