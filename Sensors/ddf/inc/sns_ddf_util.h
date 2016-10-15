/** 
 * @file sns_ddf_util.h 
 * @brief Utility library providing common services to drivers.
 *  
 * Copyright (c) 2010-2013 by Qualcomm Technologies, Inc.  All 
 * Rights Reserved Qualcomm Technologies Proprietary and 
 * Confidential 
 *
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/ddf/inc/sns_ddf_util.h#1 $
  $DateTime: 2014/05/16 10:56:56 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2013-11-18 pn   Added sns_ddf_convert_usec_to_tick()
  2013-09-06 pn   Added sns_ddf_map_axes_multiple()
  2013-04-02 ag   Add debug message functionality
  2011-11-07 yk   Added axis mapping functionality.
  2011-10-02 yk   Changed timer object to an opaque pointer.
  2010-09-24 yk   Redefined timer object. Added timer_dispatch().
  2010-08-18 yk   Renamed time_delay() to delay() and changed arg to microsec..
                  Renamed get_time_ticks() to get_timestamp().
  2010-08-14 yk   Renamed identifiers to conform to DSPS naming convention.
  2010-08-04 yk   Initial revision
==============================================================================*/

#ifndef SNS_DDF_UTIL_H
#define SNS_DDF_UTIL_H

#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_em.h"

/** 
 *  Debug macros for device drivers.
 *  
 *  @params[in]  level  LOW| MED| HIGH| ERR| FATAL
 *  @params[in]  msg    for DSPS targets an ENUM such as
 *                      DBG_SMGR_GENERIC_STRING0
 *                      for ADSP targets a message string
 *                      similar to input to printf in C 
 *  @params[in]  p1     optional variable used by msg
 *  @params[in]  p2     optional variable used by msg
 *  @params[in]  p3     optional variable used by msg
 *  
 *  e.g. DDF_MSG_1(LOW, "Debug var: %d", p1); DDF_MSG_3(FATAL,
 *  "var1: %d var2: %x var3: %d", p1, p2, p3);
 *  
 *  Note - On DSPS targets, debug "msg" has to be a reference
 *  to a debug string defined in sns_debug_api.h on apps
 *  side. e.g. DBG_SMGR_GENERIC_STRING1. So msg will be ignored
 *  and instead a standard string will be used.
 *  
 *   */
#ifdef DDK
  #define DDF_MSG_0(level,msg)          printf(msg)
  #define DDF_MSG_1(level,msg,p1)       printf(msg,p1)
  #define DDF_MSG_2(level,msg,p1,p2)    printf(msg,p1,p2)
  #define DDF_MSG_3(level,msg,p1,p2,p3) printf(msg,p1,p2,p3)
#else
  #ifdef DD_DEBUG
    #if defined QDSP6
      #if defined ADSP_STANDALONE
        #define DDF_MSG_0(level,msg)          printf(msg)
        #define DDF_MSG_1(level,msg,p1)       printf(msg,p1)
        #define DDF_MSG_2(level,msg,p1,p2)    printf(msg,p1,p2)
        #define DDF_MSG_3(level,msg,p1,p2,p3) printf(msg,p1,p2,p3)
      #else
        #define DBG_MEDIUM_PRIO DBG_MED_PRIO
        #define DDF_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, msg)
        #define DDF_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1)
        #define DDF_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2)
        #define DDF_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, msg,p1,p2,p3)
      #endif
    #else
      #define MED MEDIUM
      #include "sns_debug_str.h"
      #define DDF_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
      #define DDF_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
      #define DDF_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
      #define DDF_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
      #endif
  #else
    #define DDF_MSG_0(level,msg)
    #define DDF_MSG_1(level,msg,p1)
    #define DDF_MSG_2(level,msg,p1,p2)
    #define DDF_MSG_3(level,msg,p1,p2,p3)
  #endif
#endif

/** A timer object used to dispatch a function at a later time. */
typedef struct sns_ddf_timer_obj_s* sns_ddf_timer_s;


/** Maps a sensor device's coordinate system to the phone's. */
typedef struct
{
    uint8_t indx_x, indx_y, indx_z;
    int8_t sign_x, sign_y, sign_z;
    
} sns_ddf_axes_map_s;


/** 
 * Initializes an axes map object from a set of raw values. It is assumed that 
 * the data consists of three axes, in the order x, y, z. The registry data 
 * is formatted such that each axis is represented as an integer in the range 
 * [-3, 3], excluding zero. Where 1 => X axis, 2 => Y, 3 => Z. For example, to 
 * map the device's X axis to the phone's -Y, set registry_data[0] = -2.
 *  
 * @params[out] axes_map       Object to be initialized.
 * @params[in]  registry_data  Config data values from the registry or NULL for 
 *                             no axis mapping.
 */
void sns_ddf_axes_map_init(sns_ddf_axes_map_s* axes_map, uint8_t* registry_data);

    
/** 
 * Performs a mapping from one coordinate system to another, according to the 
 * configuration with which this object was initialized. Mapping is done in 
 * place, so that 'device_data' may be changed when this function returns. 
 *  
 * @param[in]     axes_map     This object. 
 * @param[in,out] device_data  The data to be re-mapped.
 */ 
void sns_ddf_map_axes(sns_ddf_axes_map_s* axes_map, q16_t* device_data);


/**
 * Performs a mapping from one coordinate system to another, according to the 
 * configuration with which this object was initialized. Can map 
 * multiple sets of data. 
 *  
 * @param[in]     axes_map     This object. 
 * @param[in,out] data_ptr     The data to be re-mapped.
 */ 
void sns_ddf_map_axes_multiple(const sns_ddf_axes_map_s* axes_map, 
                               sns_ddf_sensor_data_s* data_ptr);


/**
 * @brief Allocates a memory block dynamically. Driver must use this function 
 *        instead of malloc(). 
 *
 * @param[out] ptr   Reference to buffer where resulting memory will reside. 
 * @param[in]  size  Number of bytes to allocate.
 *
 * @return SNS_DDF_SUCCESS if memory was allocated, otherwise SNS_DDF_ENOMEM or 
 *         SNS_DDF_EFAIL to indicate an error has occurred. 
 */
sns_ddf_status_e sns_ddf_malloc(void** ptr, uint16_t size);


/**
 * @brief Frees a block of memory that was allocated via sns_ddf_malloc(). 
 *  
 * @warning Calling this function with a parameter that wasn't returned by 
 *          sns_ddf_malloc() will result in unpredictable system behavior. 
 *
 * @param[out] ptr  A reference to the buffer containing the memory to be freed.
 *
 * @return SNS_DDF_SUCCESS if memory was freed properly, otherwise SNS_DDF_EFAIL to 
 *         indicate an error has occurred. 
 */
sns_ddf_status_e sns_ddf_mfree(void* ptr);


/**
 * @brief Initializes a timer object.
 *  
 * @note This function only initializes the object, it does *not* automatically 
 *       dispatch it. @see sns_ddf_timer_start().
 *
 * @param[out] timer         Pointer to the timer object to be initilized.
 * @param[in]  dd_handle     A pointer to the driver instance.
 * @param[in]  dd_interface  The driver's interface functions. 
 * @param[in]  arg           Argument to pass to the driver's callback function. 
 *                           NB: Only this pointer is saved with the timer (i.e.
 *                           the underlying value isn't copied). Therefore, this
 *                           must never point to automatic memory.
 * @param[in]  periodic      If true, this timer is re-armed after every 
 *                           expiration, until explicitly cancelled. Otherwise
 *                           this is a one-time timer.
 *  
 * @return SNS_DDF_SUCCESS if the timer was initialized successfully. Otherwise 
 *         SNS_DDF_ENOMEM or SNS_DDF_EFAIL to indicate an error occurred. 
 */
sns_ddf_status_e sns_ddf_timer_init(
    sns_ddf_timer_s*      timer,
    sns_ddf_handle_t      dd_handle,
    sns_ddf_driver_if_s*  dd_interface,
    void*                 arg,
    bool                  periodic);


/**
 * @brief Begins timer countdown. 
 *  
 * When the countdown is complete, the callback associated with the timer is 
 * called with the argument provided at initialization. 
 *  
 * @note Timers are not rearmed automatically. I.e. this function must be called 
 *       explicitly to start the timer again.
 *
 * @param[in] timer  Pointer to an initialized timer object. 
 * @param[in] usec   Number of microseconds before timer expires. 
 *
 * @return SNS_DDF_SUCCESS if timer was dispatched successfully. Otherwise 
 *         SNS_DDF_EINVALID_PARAM or SNS_DDF_EFAIL to indicate an error has occured.
 */
sns_ddf_status_e sns_ddf_timer_start(sns_ddf_timer_s timer, uint32_t usec);


/**
 * @brief Cancels a running timer.
 *  
 * @param[in] timer  Timer to be cancelled.
 *
 * @return SNS_DDF_SUCCESS if the timer was cancelled successfully.  Otherwise 
 *         SNS_DDF_EINVALID_PARAM or SNS_DDF_EFAIL to indicate an error has occured. 
 */
sns_ddf_status_e sns_ddf_timer_cancel(sns_ddf_timer_s timer);


/**
 * @brief Deallocates resources associated with this timer and invalidates the 
 *        timer object.
 *  
 * @note After this function is called, the timer cannot be used until it is 
 *       initialized again. @see sns_ddf_timer_init(). 
 *  
 * @param[in] timer  Timer to be cancelled.
 *
 * @return SNS_DDF_SUCCESS if the timer was released successfully.  Otherwise 
 *         SNS_DDF_EINVALID_PARAM or SNS_DDF_EFAIL to indicate an error has occured. 
 */
sns_ddf_status_e sns_ddf_timer_release(sns_ddf_timer_s timer);


/**
 * Dispatches the callback function registered with this timer.
 * 
 * @param[in] timer  Timer whose associated callback will be dispatched.
 */
void sns_ddf_timer_dispatch(sns_ddf_timer_s timer);


/**
 * @brief Returns system time in ticks, i.e. time since boot.
 *
 * @return Time in ticks.
 */
sns_ddf_time_t sns_ddf_get_timestamp(void);

uint32_t sns_ddf_convert_usec_to_tick(uint32_t usec);

/**
 * @brief Delays the current task without relinquishing the CPU. 
 *  
 * @note The delay functionality has some overhead associated with it and 
 *       therefore the exact amount of time delayed may include some error.
 *       The expected error tends to increase at smaller values.
 *  
 * @param[in] usec  Number of microseconds to delay the task.
 */
void sns_ddf_delay(uint32_t usec);


/**
 * @brief Creates and initializes a mutex.
 *
 * @return A handle used to identify the newly created mutex.
 */
sns_ddf_handle_t sns_ddf_mutex_create(void);


/**
 * @brief Deletes a mutex and deallocates any associated resources.
 * 
 * @note It is the user's responsibility to make sure the mutex is not being 
 *       used when this function is called. 
 *  
 * @param[in] handle  Handle to the mutex provided at creation.
 *
 * @return SNS_DDF_SUCCESS if the mutex was deleted successfully. Otherwise 
 *         SNS_DDF_EINVALID_PARAM or SNS_DDF_EFAIL to indicate an error has occurred. 
 */
sns_ddf_status_e sns_ddf_mutex_delete(sns_ddf_handle_t handle);


/**
 * @brief Acquires a mutex lock, thereby rendering a resource unavailable to 
 *        other system entities. @see sns_ddf_mutex_release().
 *
 * @param[in] handle  Handle to the mutex provided at creation.
 *
 * @return SNS_DDF_SUCCESS if the mutex was deleted successfully. Otherwise 
 *         SNS_DDF_EINVALID_PARAM or SNS_DDF_EFAIL to indicate an error has occurred. 
 */
sns_ddf_status_e sns_ddf_mutex_lock(sns_ddf_handle_t handle);


/**
 * @brief Releases a mutex lock that was previously locked, thereby making the
 *        resource available again. @see sns_ddf_mutex_release().
 *
 * @param[in] handle  Handle to the mutex provided at creation.
 *
 * @return SNS_DDF_SUCCESS if the mutex was deleted successfully. Otherwise 
 *         SNS_DDF_EINVALID_PARAM or SNS_DDF_EFAIL to indicate an error has occurred. 
 */
sns_ddf_status_e sns_ddf_mutex_release(sns_ddf_handle_t handle);

#endif
