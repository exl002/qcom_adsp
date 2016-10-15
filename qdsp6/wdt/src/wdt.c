/* ========================================================================
   Watch Dog Timer main file

   *//** @file wdt.c
     This is a file for WDT functions and thread

     Copyright (c) 2010-2011 QUALCOMM Technologies Incorporated.
     All Rights Reserved. Qualcomm Proprietary and Confidential.
   *//*==================================================================== */
/*-------------------------------------------------------------------------------------------*/
/* Version information:                                                                      */
/* $Header: //components/rel/dspcore.adsp/2.2/wdt/src/wdt.c#8 $    */
/* $DateTime: 2013/08/23 17:59:20 $                                                          */
/* $Author: coresvc $                                                                       */
/*-------------------------------------------------------------------------------------------*/
/* ========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ----------------------------------------------------
   11/15/10   weshmawy     Created file.
   ======================================================================= */


/* =======================================================================
                           INCLUDE FILES
   ======================================================================= */
#include <stdio.h>
#include <stdlib.h>
#include "wdt.h"
#include "HALhwio.h"
#include "msmhwioreg_qdsp6ss_wdt.h"
#include "qurt_elite.h"
#include "sys_m_smsm.h"
//#include "wdtMmpm.h"

/* =======================================================================
                           DEFINE Variables
   ======================================================================= */
#define WDOG_TIMER_INTERRUPT 32                          // updated to 32?
#define WDT_STACK_SIZE 384
#define WAKE_COUNTER 1                                   // after Q6 power up, for counting how long the Q6 should stay awake. 
#define MAX_WDOG_COUNT 12787                             // the max time when the watchdog times out
#define MAX_WDOG_TIME 390                                // MAX_WDOG_COUNT/frequency = 12787/32K = 12787/32/1024 = 0.390

unsigned long long wdt_stack[WDT_STACK_SIZE];
unsigned int iwake_mintime=400;                          // minimal wakeup time. actual wakeup time is detemined by (iwake_mintime * WAKE_COUNTER)
unsigned int ibark_time=1000;                            // expressed in ms
unsigned int ibite_time=10000;
unsigned int inmi_time=9500;         
volatile unsigned int iWakeCounter=0;                    // bound define wake time
unsigned int iWDTkick=0;
int SMP2PSTATUS=0;                                       // check whether SMP2P error handling bit is set successfully   
int SMP2PFirstSendFlag = -1;                             // call sys_m_smsm_apps_set function the 1st time after boot, and set the flag 
unsigned int iWDTIdleCounter=0;  
int WDT_INIT_FLAG = 0;
int WDT_PANIC_FLAG=0;                                  // flag to indicate whether there is a WDT_triggle_bite function called while ADSP is in idle mode
int WDT_ACTIVE_FLAG=0;                                 // flag to indicate WDT in active mode or idle mode    

qurt_mutex_t wdt_Mutex;
static qurt_sem_t WDT_IST_started;

/*
 * @brief wdt_init starts a qurt thread for the wdt_task function
 *
 */

int WDT_init( )                                          //  make this argument void
{
   typedef void (*wdt_handler_func_t) (void *);
   int ret = 0;
   qurt_thread_attr_t attr;
	qurt_thread_t tid;
   static char cname[16]= "WDT_ist";
  
   //qurt_printf("Initializing WDT");
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Initializing WDT");

   qurt_mutex_init(&wdt_Mutex);

	qurt_thread_attr_init (&attr);
	qurt_thread_attr_set_stack_size (&attr, WDT_STACK_SIZE * 8);
	qurt_thread_attr_set_stack_addr (&attr, wdt_stack);
	qurt_thread_attr_set_priority (&attr, (unsigned short)0x02);
   qurt_thread_attr_set_name(&attr,cname );
   ret =  qurt_thread_create(&tid, &attr, (wdt_handler_func_t)WDT_task, NULL);
    
   if(ret == QURT_EFATAL){
   //    qurt_printf("qurt_thread_creat failed for wdt_task\n");
   MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create WDT Task %d");
   return -1;
   }

   // finish wdt initialization, and give  WDT_INIT_FLAG = 1
   WDT_INIT_FLAG = 1;

  return 0;
}


/* 
 * @brief wdt_task start wdt timer, answer pet interrupts
 *
 */ 
void WDT_task
  (
    /* Parameter received from Main Control task - ignored */
    unsigned long int unused_param
  )
{
   int nIntNo=WDOG_TIMER_INTERRUPT;  // Watch Dog Timer Interrupt //
   qurt_anysignal_t iSig;
   unsigned int uObservedMask;                               // interrupt is 32, need to use long int
   unsigned int uWaitMask;


   /* Initialize the wdt subsystem */
   WDT_timer_init();    
    
   /* Register interrupt */
   uWaitMask = 1<<0;                                     // signal mask for the interrupt
   qurt_anysignal_init(&iSig);
   
   if( qurt_interrupt_register(nIntNo,&iSig, uWaitMask) != QURT_EOK)
   {
      qurt_printf("qurt_interrupt_register failed for wdt\n");
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "qurt_interrupt_register failed for wdt\n");
      return;
   }

   qurt_sem_up(&WDT_IST_started);   
   
   while (1) {
      /* wait for interrupt*/
      uObservedMask = qurt_anysignal_wait(&iSig,uWaitMask);
      /* check if it the right feedback*/
      if (uObservedMask==uWaitMask) {
         qurt_anysignal_clear(&iSig, uObservedMask );
         WDT_timer_pet();
         qurt_interrupt_acknowledge(nIntNo);                      // ackowlege the interrupt

         if(iWDTkick%1000 == 1) { 
            if (sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_ERR_HDL_RDY) != SYS_M_SMP2P_SUCCESS) {
               qurt_printf("ADSP WDT: smp2p bit set did not succeed\n");
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP WDT: smp2p bit set did not succeed");
               SMP2PSTATUS=-1;
            }
            else {
               qurt_printf("ADSP WDT: smp2p bit set succeed\n");
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP WDT: smp2p bit set succeed");
               SMP2PSTATUS=1;
            }
         }
      }
      else 
      {
         qurt_printf("Issue with WDT timer receiving\n");
         qurt_printf("Exiting WDT thread\n");
         qurt_anysignal_destroy(&iSig);
         qurt_thread_stop();
      }
   }
}

/* 
 * @brief wdt_timer_init start wdt timer 
 *
 */ 
void WDT_timer_init()
{
   unsigned int tmp;
   // qurt_printf("Initializing WDT timer\n");
   iWDTkick=0;
   SMP2PSTATUS=0;
   SMP2PFirstSendFlag=0;

   HWIO_OUT(LPASS_QDSP6SS_WDOG_CTL, 0x7);             // enable watchdog timer and enable watchdog to trigger NMI 
                                                      // this step need to be done first before initilize BARK, BITE, NMI time, otherwise when WDT expires, bit 0 of Status register won't be set to 1   

   tmp=(iwake_mintime*MAX_WDOG_COUNT/MAX_WDOG_TIME);  // when first start the WDT task, set bark time counter to be the iwake_mintime, for Q6 to stay awake for amount of time
   HWIO_OUT(LPASS_QDSP6SS_WDOG_BARK_TIME, tmp);

   tmp=(ibite_time*MAX_WDOG_COUNT/MAX_WDOG_TIME);     // init BITE time
   HWIO_OUT(LPASS_QDSP6SS_WDOG_BITE_TIME, tmp);

   tmp=(inmi_time*MAX_WDOG_COUNT/MAX_WDOG_TIME);      // init NMI time
   HWIO_OUT(LPASS_QDSP6SS_WDOG_NMI_TIME, tmp);

   WDT_PANIC_FLAG=0;
   WDT_ACTIVE_FLAG=1; 
}


/* 
 * @brief wdt_timer_pet answer pet interrupts
 *
 */ 
void WDT_timer_pet()
{
   unsigned int tmp;

   iWDTkick +=1;
   if (iWakeCounter <= WAKE_COUNTER){
      iWakeCounter++;
   }
   
   qurt_mutex_lock(&wdt_Mutex);
	
   
   HWIO_OUT(LPASS_QDSP6SS_WDOG_RESET,  0x1);
   if(iWDTkick%100 == 0)
   { 
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "WDT kick  %d;  WDT idle: %d ", iWDTkick, iWDTIdleCounter);
   }
   
   if (iWakeCounter == WAKE_COUNTER) {
      
      tmp=(ibark_time*MAX_WDOG_COUNT/MAX_WDOG_TIME);                       //  setup the actual bark time
      HWIO_OUT(LPASS_QDSP6SS_WDOG_BARK_TIME, tmp);       
   }
   qurt_mutex_unlock(&wdt_Mutex);		                                    //
}


void WDT_trigger_BITE ()
{
   
   if ( WDT_INIT_FLAG == 0){
      qurt_printf("WatchDog is not initilized for WDT_trigger_BITE()!\n");
      return;
   }
   qurt_mutex_lock(&wdt_Mutex);
   WDT_PANIC_FLAG=0;
   if (WDT_ACTIVE_FLAG) {
     WDT_set_BITE_busywait();
   } else {
     WDT_PANIC_FLAG=1;
   }
   qurt_mutex_unlock(&wdt_Mutex);
}

/* 
 * @brief WDT_active resets the timer
 *
 */ 
void WDT_active()                                                          //  no need to do mutext,    this is the power up function
{
   if ( WDT_INIT_FLAG == 0){
      qurt_printf("WatchDog is not initilized for WDT_active()!\n");
      return;
   }
   qurt_mutex_lock(&wdt_Mutex);
   unsigned int tmp;
   
   WDT_ACTIVE_FLAG=1;

   iWakeCounter = 0;

    tmp=(iwake_mintime*MAX_WDOG_COUNT/MAX_WDOG_TIME);                // when power up, set bark time counter to be the iwake_mintime, for Q6 to stay awake for amount of time
    HWIO_OUT(LPASS_QDSP6SS_WDOG_BARK_TIME, tmp);                     // 
    
    HWIO_OUT(LPASS_QDSP6SS_WDOG_RESET, 0x1);                         // reset
    HWIO_OUT(LPASS_QDSP6SS_WDOG_CTL, 0x7);                           // enable watchdog timer and enable watchdog to trigger NMI

    if (WDT_PANIC_FLAG==1) {                                         // if there is a triggle bit function called while ADSP is in idle mode
       WDT_PANIC_FLAG=0;                                             // clear the PANIC FLAG after triggle bite
       WDT_set_BITE_busywait();
    }

    qurt_mutex_unlock(&wdt_Mutex);
}


/* 
 * @brief WDT_idle freezes the timer and enables the interrupts
 *
 */ 

void WDT_idle()
{
   if ( WDT_INIT_FLAG == 0){
      qurt_printf("WatchDog is not initilized for WDT_idle()!\n");
      return;
   }

   qurt_mutex_lock(&wdt_Mutex);
   iWDTIdleCounter++;
   
   WDT_ACTIVE_FLAG=0;                                                   
   WDT_PANIC_FLAG=0; 

	
   HWIO_OUT(LPASS_QDSP6SS_WDOG_CTL, 0x2);                                  // disable watchdog timer and enable interrupt to wakeup watchdog from QDSP6 idle or power collapse 
   HWIO_OUT(LPASS_QDSP6SS_WDOG_RESET, 0x1);                                // reset


   if (!SMP2PFirstSendFlag){
      if (sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_ERR_HDL_RDY) != SYS_M_SMP2P_SUCCESS) {
         qurt_printf("ADSP WDT 1st idle: smp2p bit set did not succeed\n");
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP WDT 1st idle: smp2p bit set did not succeed");
         SMP2PSTATUS=-1;
      }
      else {
         qurt_printf("ADSP WDT 1st idle: smp2p bit set succeed\n");
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP WDT 1st idle: smp2p bit set succeed");
         SMP2PSTATUS=1;
      }
      SMP2PFirstSendFlag = 1;							// set this flag to be 1 after call sys_m_smsm_apps_set in idle function the first time after boot
   }                                        

	qurt_mutex_unlock(&wdt_Mutex);
}

void WDT_set_BITE_busywait()
{
   HWIO_OUT(LPASS_QDSP6SS_WDOG_BITE_TIME, 0x10);                         // set bite and nmi time to be smaller value than bark time. for triggering a bite immediately
   HWIO_OUT(LPASS_QDSP6SS_WDOG_NMI_TIME, 0x8);
   qurt_printf("WDT: bite and nmi time is set to be smaller value than bark time. for triggering a bite\nEnter busy wait loop\n");
   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "WDT: bite and nmi time is set to be smaller value than bark time. for triggering a bite\nEnter busy wait loop\n");
   while(1) {                                   // busy wait after setting the bite/nmi value, to avoid ADSP going to power collapse before ssr
   }
}

