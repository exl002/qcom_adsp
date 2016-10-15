/*=============================================================================

                Qurt_Timer_Client_Test.c

GENERAL DESCRIPTION
      Qurt Timer Apis Test Code from Process Space

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


$Header: //components/rel/core.adsp/2.2/services/time/src/qurt_timer_client_test.c#2 $ 
$DateTime: 2013/07/10 17:05:21 $ $Author: coresvc $

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
}qurt_timer_test_enum_type;

  qurt_anysignal_t qurt_timer_client_test_cb_signal, qurt_timer_test_signal;
  timer_type qurt_timer_client_test_timer1;
  qurt_timer_attr_t attr;
  qurt_timer_t qurt_timer_client_test_timer;


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
void qurt_timer_client_test
(
  void *parameter
)
{
  unsigned int mask_fired=0;
  unsigned int i;
  int result;
  unsigned long long duration = 1000000;
  int timer_client_pid = qurt_getpid();
    
 
  /*initialize the task signal*/
  qurt_anysignal_init (&qurt_timer_test_signal);
  
   qurt_timer_attr_init(&attr);
   qurt_timer_attr_set_duration(&attr, 60 * duration);
   qurt_timer_attr_set_group(&attr, 0);
   qurt_timer_attr_set_type(&attr, QURT_TIMER_ONESHOT);

  /*prime the task*/
  (void) qurt_anysignal_set( &qurt_timer_test_signal, LPASS_TEST1 );
    
  //loop forever
  for(;;)
  {
      /* Todo: Need to delete all test code later from scons files */
      if(timer_client_pid == 0)
         break;
         
      mask_fired = qurt_anysignal_wait (&qurt_timer_test_signal, LPASS_TEST1 | LPASS_TEST2 | LPASS_TEST3);

      if( mask_fired & LPASS_TEST1) 
      {
        qurt_timer_attr_set_duration(&attr, 60 * duration);

        qurt_anysignal_init (&qurt_timer_client_test_cb_signal);

        /*define a timer for 1 sec*/
        //timer_def(&qurt_timer_client_test_timer1, NULL, &qurt_timer_client_test_cb_signal, 0x1, NULL, NULL);
        qurt_timer_create(&qurt_timer_client_test_timer1, &attr, &qurt_timer_client_test_cb_signal, 0x1);

        /*set the timer for 1 sec*/
        //timer_set(&qurt_timer_client_test_timer1, 60, 0, T_SEC);
        
        qurt_anysignal_wait(&qurt_timer_client_test_cb_signal, 0x1);

        MSG_HIGH("Qurt initial timer fired", 0, 0, 0);
        
        qurt_timer_attr_set_duration(&attr, duration);
        for(i=0; i < TIMER_TEST_LOOPS; i++ ) 
        {
          result = qurt_timer_create(&qurt_timer_client_test_timer, &attr, &qurt_timer_client_test_cb_signal, 0x1);
          //MSG_HIGH("Qurt initial timer set for %d time" , i, 0, 0);

          qurt_timer_stop(qurt_timer_client_test_timer);
          //MSG_HIGH("Qurt initial timer stopped",0,0,0);

          //qurt_timer_sleep(1000000);

          qurt_timer_restart(qurt_timer_client_test_timer, duration);
          //MSG_HIGH("Qurt initial timer restarted",0,0,0);

          qurt_anysignal_wait(&qurt_timer_client_test_cb_signal, 0x1);
          //MSG_HIGH("Qurt initial timer fired",0,0,0);
          
          qurt_timer_delete(qurt_timer_client_test_timer);
        }
        MSG_HIGH("qurt_timer_test1 is Done..",0,0,0);
      }
      
      if( mask_fired & LPASS_TEST2) 
      {
        qurt_timer_attr_set_duration(&attr, 60 * duration);

        qurt_anysignal_init (&qurt_timer_client_test_cb_signal);

        //timer_def(&qurt_timer_client_test_timer1, NULL, &qurt_timer_client_test_cb_signal, 0x1, NULL, NULL);
        qurt_timer_create(&qurt_timer_client_test_timer1, &attr, &qurt_timer_client_test_cb_signal, 0x1);

        /*set the timer for 1 sec*/
        //timer_set(&qurt_timer_client_test_timer1, 60, 0, T_SEC);
        
        /*define a timer for 1 sec*/
        //timer_def(&qurt_timer_client_test_timer1, NULL, &qurt_timer_client_test_cb_signal, 0x1, NULL, NULL);

        /*set the timer for 1 sec*/
        //timer_set(&qurt_timer_client_test_timer1, 60, 0, T_SEC);
        
        qurt_anysignal_wait(&qurt_timer_client_test_cb_signal, 0x1);

        MSG_HIGH("Qurt initial timer fired",0,0,0);
        
        result = qurt_timer_create(&qurt_timer_client_test_timer, &attr, &qurt_timer_client_test_cb_signal, 0x1);
        MSG_HIGH("Qurt test timer created" , 0, 0, 0);

        qurt_timer_attr_set_duration(&attr, 10 * duration);
        for(i=0; i < TIMER_TEST_LOOPS; i++ ) 
        {
          qurt_timer_restart(qurt_timer_client_test_timer, 10 * duration);
          MSG_HIGH("Qurt initial timer restarted for %d th time",i+1,0,0);

          qurt_anysignal_wait(&qurt_timer_client_test_cb_signal, 0x1);
          
          qurt_anysignal_clear(&qurt_timer_client_test_cb_signal, 0x1);
          //MSG_HIGH("Qurt initial timer fired",0,0,0);
        }
        qurt_timer_delete(qurt_timer_client_test_timer);
        MSG_HIGH("qurt_timer_test2 is Done..",0,0,0);
      }
  }
}