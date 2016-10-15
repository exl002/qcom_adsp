/*=============================================================================

                Qurt_Time_Test.c

GENERAL DESCRIPTION
      Qurt Timer Apis Test Code that executes from Guest OS

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


$Header: //components/rel/core.adsp/2.2/services/time/src/qurt_timer_test.c#5 $ 
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
#include "qurt_signal2.h"


typedef enum
{
    LPASS_TEST1 = 1,
    LPASS_TEST2 = 2,
    LPASS_TEST3 = 4,
}qurt_timer_test_enum_type;

  qurt_signal2_t qurt_timer_test_cb_signal, qurt_timer_test_signal;
  timer_type qurt_test_timer1;
  qurt_timer_attr_t attr;
  qurt_timer_t qurt_test_timer;


#define TIMER_TEST_LOOPS 50


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
void qurt_timer_test_task
(
  void
)
{
#if 1
  unsigned int mask_fired=0;
  unsigned int i = 0;
  int result;
#endif /* #if 0 */
  unsigned long long duration = 1000000;
    
  
  //wait on rcinit handshake
  rcinit_handshake_startup();
  
  /*initialize the task signal*/
  qurt_signal2_init (&qurt_timer_test_signal);
  
   qurt_timer_attr_init(&attr);
   qurt_timer_attr_set_duration(&attr, duration);
   qurt_timer_attr_set_group(&attr, 0);
   qurt_timer_attr_set_type(&attr, QURT_TIMER_ONESHOT);

  /*prime the task*/
  (void) qurt_signal2_set( &qurt_timer_test_signal, LPASS_TEST1 );

/* Todo: Need to remove this once testing is done */
#if 1    
  //loop forever
  for(;;)
  {
      
      mask_fired = qurt_signal2_wait (&qurt_timer_test_signal, LPASS_TEST1 | LPASS_TEST2 | LPASS_TEST3, QURT_SIGNAL_ATTR_WAIT_ANY);

      if( mask_fired & LPASS_TEST1) 
      {

        qurt_signal2_init (&qurt_timer_test_cb_signal);

        /*define a timer for 1 sec*/
        timer_def_osal(&qurt_test_timer1, NULL, TIMER_NATIVE_OS_SIGNAL2_TYPE, &qurt_timer_test_cb_signal, 0x1);
        
        /*set the timer for 1 sec*/
        timer_set_64(&qurt_test_timer1, 60, 0, T_SEC);
        
        MSG_HIGH("Qurt initial timer started",0,0,0);        
        
        qurt_signal2_wait(&qurt_timer_test_cb_signal, 0x1, QURT_SIGNAL_ATTR_WAIT_ANY);

        MSG_HIGH("Qurt initial timer fired",0,0,0);

        for(i=0; i < TIMER_TEST_LOOPS; i++ ) 
        {
          result = qurt_timer_create_sig2(&qurt_test_timer, &attr, &qurt_timer_test_cb_signal, 0x1);
          MSG_HIGH("Qurt timer set for %d time" , i+1, 0, 0);

          qurt_timer_stop(qurt_test_timer);
          MSG_HIGH("Qurt timer stopped",0,0,0);

          qurt_timer_sleep(1000000);

          qurt_timer_restart(qurt_test_timer, duration);
          MSG_HIGH("Qurt timer restarted",0,0,0);

          qurt_signal2_wait(&qurt_timer_test_cb_signal, 0x1, QURT_SIGNAL_ATTR_WAIT_ANY);
          MSG_HIGH("Qurt timer fired",0,0,0);
          
          qurt_timer_delete(qurt_test_timer);
        }
        MSG_HIGH("qurt_timer_test1 is Done..",0,0,0);
      }
      
      if( mask_fired & LPASS_TEST2) 
      {

        qurt_signal2_init (&qurt_timer_test_cb_signal);

        /*define a timer for 1 sec*/
        timer_def_osal(&qurt_test_timer1, NULL, TIMER_NATIVE_OS_SIGNAL2_TYPE, &qurt_timer_test_cb_signal, 0x1);

        /*set the timer for 1 sec*/
        timer_set_64(&qurt_test_timer1, 60, 0, T_SEC);
        
        qurt_signal2_wait(&qurt_timer_test_cb_signal, 0x1, QURT_SIGNAL_ATTR_WAIT_ANY);

        MSG_HIGH("ATS initial timer fired",0,0,0);
        
        result = qurt_timer_create_sig2(&qurt_test_timer, &attr, &qurt_timer_test_cb_signal, 0x1);
        MSG_HIGH("Qurt initial timer set for %d time" , 0, 0, 0);

        for(i=0; i < TIMER_TEST_LOOPS; i++ ) 
        {
          qurt_timer_restart(qurt_test_timer, duration);
          //MSG_HIGH("Qurt initial timer restarted for ith time",i,0,0);

          qurt_signal2_wait(&qurt_timer_test_cb_signal, 0x1, QURT_SIGNAL_ATTR_WAIT_ANY);
          
          qurt_signal2_clear(&qurt_timer_test_cb_signal, 0x1);
          //MSG_HIGH("Qurt initial timer fired",0,0,0);
        }
        qurt_timer_delete(qurt_test_timer);
        MSG_HIGH("qurt_timer_test2 is Done..",0,0,0);
      }
  }
#endif /* #if 0 */
}

