/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR service crash simulator
                
GENERAL DESCRIPTION
  Command handler for injecting various types of crashes for testing

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.adsp/2.2/debugtools/err/src/err_inject_crash.c#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/01/13   rks     Incorporated the timer API changes
10/28/11   mcg     File created.

===========================================================================*/

#ifndef ERR_INJECT_CRASH
  #error Required definition is missing, check configuration
#endif //ERR_INJECT_CRASH

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "err.h"
#include "erri.h"
#include "err_inject_crash.h"
#include "diagpkt.h"
#include "err_diagpkt.h"
#include "diagcmd.h"
#include "osal.h"
#include "msg.h"
#include "timer.h"

#ifdef ERR_USES_DEVCFG
#include <stdint.h>
#include "DALSysTypes.h"
#include "DALPropDef.h"
#endif



/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
PACK(void *) err_inject_crash_diag (PACK(void *), uint16);



/*===========================================================================

                     INTERNAL DATA DECLARATIONS

===========================================================================*/
// default value if no delay is provided (min value for device config input)
#define ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT  1
#define ERR_INJECT_CRASH_DELAY_DEVCFG_MIN   60

/* table for diag packet command handler registration */
/* kludge for single-PD lpass+sensors */
#define ERR_DIAGPKT_CORE_DUMP_DSPS (ERR_DIAGPKT_CORE_DUMP - ERR_DIAG_PROC_LPASS + ERR_DIAG_PROC_DSPS) 
static const diagpkt_user_table_entry_type err_inject_crash_diagpkt_tbl[] =
{
  {ERR_DIAGPKT_CORE_DUMP, ERR_DIAGPKT_CORE_DUMP, err_inject_crash_diag},
  {ERR_DIAGPKT_CORE_DUMP_DSPS, ERR_DIAGPKT_CORE_DUMP_DSPS, err_inject_crash_diag}
};

typedef PACK(struct) 
{
  diagpkt_subsys_header_type hdr;
  uint8 crash_type;
  uint8 crash_delay;
} err_inject_crash_req_type;

typedef PACK(struct) {
  diagpkt_subsys_header_type hdr;
} err_inject_crash_rsp_type;

static volatile int err_inject_crash_zero = 0;

static timer_type       eicTimer;
static timer_group_type eic_timer_group;
static boolean          crash_pending = FALSE;

/*===========================================================================

                        Private function prototypes

===========================================================================*/
extern void dog_intentional_timeout(void);

static void err_inject_crash_err_fatal(void);
static void err_inject_crash_wdog(void);
static void err_inject_null_access(void);
static void err_inject_crash_div0(void);

static void err_inject_crash_initiate(err_inject_crash_type, uint32);
static void err_inject_crash_execute(timer_cb_data_type);
static void err_inject_crash_devcfg_check(void);


/*===========================================================================

                              Function definitions

===========================================================================*/

// weak stub for LPASS api to force bite
// if this is not resolving, take it up with the audio team
// they opted to use the brittle weak linking mechanism
// rather than a registration api
void __attribute__((weak)) WDT_trigger_BITE(void) 
{
  MSG_ERROR("No wdog crash handler has been provided",0,0,0);
}

/*===========================================================================

FUNCTION err_inject_crash_init

DESCRIPTION
  Initializes the crash injection feature

DEPENDENCIES
  diag must be available to register command handler

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void err_inject_crash_init (void)
{
#ifdef ERR_USES_DEVCFG
  //Check for device config method before registering with diag (thus avoiding a mutex)
  err_inject_crash_devcfg_check();
#endif //ERR_USES_DEVCFG

  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (ERR_DIAG_PROC_ID, DIAG_SUBSYS_DEBUG, err_inject_crash_diagpkt_tbl);
                                         
  //initialize timer group to be safe
  memset(&eic_timer_group, 0, sizeof(eic_timer_group)); 
                                       
}

/*===========================================================================

FUNCTION err_inject_crash_diag

DESCRIPTION
  Force a crash to occur remotely by calling ERR_FATAL

DEPENDENCIES
  err_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr

SIDE EFFECTS

===========================================================================*/
PACK(void *)
err_inject_crash_diag (PACK(void *) req_ptr, uint16 pkt_len)
{
  err_inject_crash_req_type *req;
  err_inject_crash_rsp_type *rsp = NULL;
  err_inject_crash_type injected_crash = ERR_INJECT_ERR_FATAL;
  uint8 injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;

  /* build response */
  rsp = (err_inject_crash_rsp_type *) diagpkt_subsys_alloc(
     DIAG_SUBSYS_DEBUG,                       
     ERR_DIAGPKT_CORE_DUMP,
     sizeof( err_inject_crash_rsp_type ));

  diagpkt_commit(rsp);

  // Diag only handles one request at a time, and dalcfg is checked before we register with diag
  // so simple mutual exclusion is sufficient (for now)
  if (crash_pending)
  {
    MSG_HIGH("err_inject_crash: crash pending, ignoring subsequent request",0,0,0);
    return(rsp);
  }

  // decode request
  req = (err_inject_crash_req_type *) req_ptr;

  // default parameters
  if (pkt_len <= sizeof(req->hdr)) 
  {
    injected_crash = ERR_INJECT_ERR_FATAL;
    injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;
  }
  // default delay parameter
  else if (pkt_len <= (sizeof(req->hdr) + sizeof(req->crash_type)) )
  {
    injected_crash = (err_inject_crash_type)(req->crash_type);
    injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;
  }
  else
  {
    injected_crash = (err_inject_crash_type)(req->crash_type);
    injected_delay = (err_inject_crash_type)(req->crash_delay);
  }

  err_inject_crash_initiate(injected_crash, injected_delay);

  return(rsp);
}


/*===========================================================================

FUNCTION err_inject_crash_initiate

DESCRIPTION
  Initiates the crash task which will perform the actual crash once timer expires

DEPENDENCIES
  err_init must be called prior

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void err_inject_crash_initiate(err_inject_crash_type req_crash, uint32 req_delay)
{
  // validate crash cause
  if (req_crash >= SIZEOF_ERR_INJECT_CRASH_TYPE) 
  {
    //do not err_fatal, as this may present false positives
    MSG_ERROR("err_inject_crash: invalid crash ID %#x", req_crash, 0, 0);
    return;
  }

  //check for immediate crash (from caller context)
  if (0==req_delay)
  {
    err_inject_crash_execute(req_crash);
  }
  else
  {
    if (crash_pending)
    {
      return;
    }
    crash_pending = TRUE;

    //set cb timer to trigger actual crash
    timer_def_osal(&eicTimer, &eic_timer_group, TIMER_FUNC1_CB_TYPE, err_inject_crash_execute, (timer_cb_data_type)req_crash);
    timer_set_64(&eicTimer, (timetick_type)req_delay, 0, T_SEC);
  }

}


static void err_inject_crash_err_fatal(void)
{
  ERR_FATAL("Crash injected via Diag",0,0,0);
}

static void err_inject_crash_wdog(void)
{
  //dog_intentional_timeout();
  WDT_trigger_BITE();

}

static void err_inject_null_access(void)
{
  static int result;

  result = *(int*)err_inject_crash_zero;

  MSG_ERROR("Should have crashed due to null access: %#x->%#x", err_inject_crash_zero, result, 0);
}

static void err_inject_crash_div0(void)
{
  static float result;

  result = 7/err_inject_crash_zero;

  MSG_ERROR("Should have crashed due to div0: 7/%u=%d", err_inject_crash_zero, result, 0);
}


void err_inject_crash_execute(timer_cb_data_type crash_type)
{
  crash_pending = FALSE;

  //crash  
  switch(crash_type)
  {
  case ERR_INJECT_DIV0: 
    err_inject_crash_div0();
    break;
  case ERR_INJECT_WDOG_TIMEOUT:
    err_inject_crash_wdog();
    break;
  case ERR_INJECT_NULLPTR:
    err_inject_null_access();
    break;
  case ERR_INJECT_ERR_FATAL :
    err_inject_crash_err_fatal();
  default:
    MSG_ERROR("err_inject_crash: unhandled crash ID %#x", crash_type, 0, 0);
  }
}


#ifdef ERR_USES_DEVCFG
static void err_inject_crash_devcfg_check(void)
{
  err_inject_crash_type eic_type;
  uint32 eic_delay = ERR_INJECT_CRASH_DELAY_DEVCFG_MIN;

  DALSYSPropertyVar propValue;
  DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

  DALSYS_GetDALPropertyHandleStr("tms_err",pHandle);

  // Check for devcfg crashing enabled
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"eic_crash_enable", 0, &propValue))
  {
    
    if(0 == propValue.Val.dwVal)
    {
      return;
    }
  }
  else
  {
    MSG_HIGH("Could not read eic_crash_enable devcfg", 0, 0, 0);
    return;
  }

  // Crashing enabled, read eic_crash_type parameter
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"eic_crash_type", 0, &propValue))
  {
    if (SIZEOF_ERR_INJECT_CRASH_TYPE <= propValue.Val.dwVal)
    {
      MSG_HIGH("Bad enum in eic_crash_type dalconfig", 0, 0, 0);
      return;
    }

    eic_type = (err_inject_crash_type)propValue.Val.dwVal;
  }
  else
  {
    MSG_HIGH("Could not read eic_crash_type devcfg", 0, 0, 0);
    return;
  }

  // Read eic_crash_delay parameter (initialized to DEFAULT above)
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"eic_crash_delay", 0, &propValue))
  {
    if (ERR_INJECT_CRASH_DELAY_DEVCFG_MIN > propValue.Val.dwVal)        
    {
      MSG_LOW("Small eic_crash_delay dalconfig, using %d", ERR_INJECT_CRASH_DELAY_DEVCFG_MIN, 0, 0);
      eic_delay = ERR_INJECT_CRASH_DELAY_DEVCFG_MIN;
    }
    else
    {
      eic_delay = propValue.Val.dwVal;
    }
  }
  else
  {
    MSG_HIGH("Could not read eic_crash_delay devcfg, using %d ", ERR_INJECT_CRASH_DELAY_DEVCFG_MIN, 0, 0);
    eic_delay = ERR_INJECT_CRASH_DELAY_DEVCFG_MIN;
  }
    
    //good to initiate timer
    err_inject_crash_initiate(eic_type, eic_delay);
  }


#endif //ERR_USES_DEVCFG

