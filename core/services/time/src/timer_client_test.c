/*=============================================================================

                Timer_Client_Test.c

GENERAL DESCRIPTION
      Timer Client Test Code that runs from different processes

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


$Header: //components/rel/core.adsp/2.2/services/time/src/timer_client_test.c#5 $ 
$DateTime: 2013/09/11 08:59:47 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/20/13   ab      Add file
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
}qurt_timer_test_enum_type;

#define TIMER_TEST_LOOPS 500

qurt_anysignal_t timer_client_test_cb_signal;
timer_type timer_client_test_timer;
int timer_client_pid;

void timer_test_cb_func(timer_cb_data_type x)
{
   MSG_HIGH("%d timer_test_cb_func: ATS client timer fired", x, 0, 0);
   qurt_anysignal_set( &timer_client_test_cb_signal, 0x1 );
}

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
void timer_client_test
(
  void *parameter
)
{
  unsigned int mask_fired=0;
  unsigned int i;
  qurt_anysignal_t ats_timer_test_signal;
    
  
  /*initialize the task signal*/
  qurt_anysignal_init (&ats_timer_test_signal);

  /*prime the task*/
  (void) qurt_anysignal_set( &ats_timer_test_signal, LPASS_TEST1 );
  
  timer_client_pid = qurt_getpid();
    
  //loop forever
  for(;;)
  {
      mask_fired = qurt_anysignal_wait (&ats_timer_test_signal, LPASS_TEST1 | LPASS_TEST2 | LPASS_TEST3);

      if( mask_fired & LPASS_TEST1) 
      {

        qurt_anysignal_init (&timer_client_test_cb_signal);

        /*define a timer for 1 sec*/
        timer_def_osal(&timer_client_test_timer, NULL, TIMER_FUNC1_CB_TYPE, &timer_test_cb_func, timer_client_pid);

        /*set the timer for 1 sec*/
        timer_set_64(&timer_client_test_timer, 60, 0, T_SEC);
        
        qurt_anysignal_wait(&timer_client_test_cb_signal, 0x1);

        MSG_HIGH("%d ATS initial timer fired",timer_client_pid,0,0);

        for(i=0; i < TIMER_TEST_LOOPS; i++ ) 
        {
          timer_set_64(&timer_client_test_timer, 1, 0, T_SEC);
          MSG_HIGH("%d ATS client timer set for %dth time", timer_client_pid, i+1,0);

          timer_pause(&timer_client_test_timer);
          MSG_HIGH("%d ATS client timer paused ",timer_client_pid,0,0);

          qurt_timer_sleep(1000000);

          MSG_HIGH("%d ATS client timer restarted", timer_client_pid, 0, 0);
          timer_resume(&timer_client_test_timer);

          qurt_anysignal_wait(&timer_client_test_cb_signal, 0x1);
          qurt_anysignal_clear (&timer_client_test_cb_signal, 0x1);
          
          //MSG_HIGH("%d ATS client timer fired",timer_client_pid, 0, 0);
        }
        
        timer_undef(&timer_client_test_timer);
        MSG_HIGH("ats_timer_test is Done..",0,0,0);
      }
           
  }
}

