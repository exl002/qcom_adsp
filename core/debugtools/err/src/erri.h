#ifndef ERRI_H
#define ERRI_H
 
/*===========================================================================

                    Error Handling Service Internal Header File

Description
 
Copyright (c) 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

$Header: //components/rel/core.adsp/2.2/debugtools/err/src/erri.h#2 $
 
===========================================================================*/

#ifdef CUST_H
#include "customer.h"
#endif

#include "comdef.h"
#include "err.h"
#include "errlog.h"

#if defined(ERR_HW_QDSP6)
  #include "err_hw_qdsp6.h"
#else
  #include "err_hw_arm.h"
#endif

#if defined(ERR_IMG_ADSP)
  #include "err_img_adsp.h"
#else
  #error "bad configuration"
#endif

#ifdef ERR_CFG_QURT
  #include "err_qurt.h"
#endif //ERR_CFG_QURT

/* Number of info files stored before we begin overwriting them. */
/* Only applicacable if FEATURE_SAVE_TACE_EXTENDED is not defined. */
/* Otherwise, we use the same value as diag_debug_max_files */
//#ifndef FEATURE_SAVE_TRACE_EXTENDED
//#define ERR_DATA_NUM_FILES 4
//#endif

extern coredump_type coredump;


/* Magic number that determine whether the data in uninitialized RAM has
 * already been saved to flash or not.
 */
#define ERR_DATA_MAGIC_NUMBER       (uint64)0x5252452041544144ULL
#define ERR_DATA_RESET_MAGIC_NUMBER (uint64)0xABCD06261974EA29ULL

#define ERR_LOOP_DELAY_USEC 10000
#define ERR_CLK_PAUSE_SMALL 500
#define ERR_CLK_PAUSE_KICK 100


#define ERR_MIN_POLL_TIME_MS 500


//Err log definitions
  /* Maximum number of error logs to store per session */
  // no longer configurable, being deprecated
  #define ERR_DATA_MAX_LOGS 1
  
  /* Maximum size of the extended info written out to flash. */
  #ifndef ERR_DATA_MAX_SIZE  
  #define ERR_DATA_MAX_SIZE 0x4000
  #endif

  typedef uint64 err_data_flag_type;

  typedef struct {
    err_data_flag_type flag;
    uint32 length;
    char   data[ERR_DATA_MAX_SIZE];
  } err_data_log_type;
  
  typedef struct {
    err_data_flag_type reset_flag;
    err_data_log_type err_log[ERR_DATA_MAX_LOGS];
  } err_data_type;


void err_pause_usec(uint32 usec);

void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3) ERR_NORETURN_ATTRIBUTE;

#endif /* ERRI_H */
