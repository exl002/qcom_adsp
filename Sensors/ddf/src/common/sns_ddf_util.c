/** 
 * @file sns_ddf_util.c
 * @brief Utility library providing common services to drivers.
 *  
 * Copyright (c) 2010 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 
*/

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/ddf/src/common/sns_ddf_util.c#1 $
  $DateTime: 2014/05/16 10:56:56 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2014-02-26 MW   Replaced SNS_OS_MEMCOPY() with SNS_OS_MEMSCPY()
  2013-11-18 pn   Added sns_ddf_convert_usec_to_tick()
  2013-10-31 hw   add QDSS SW events
  2013-09-06 pn   Added sns_ddf_map_axes_multiple()
  2013-06-11 ae   Added QDSP SIM playback support
  2013-01-31 br   Updated sns_ddf_delay()
  2012-01-19 jh   Updated definition of sensor axes mapping
  2011-12-13 yk   Replaced call to memcpy with SNS_OS_MEMCOPY.
  2011-11-14 jhh  Updated sns_os_free call to meet new API
  2011-10-02 yk   Made the timer object definition internal to this module.
  2011-08-27 yk   Added check for null pointers in timer_init.
  2011-01-21 sc   Use local ticks instead of hard-coded DSPS ticks.
  2010-12-03 pg   Added __SNS_MODULE__ to be used by OI mem mgr in debug builds.
  2010-09-24 yk   Implemented timer functions. Added timer_dispatch().
  2010-08-25 yk   Implemented sns_ddf_delay() for target builds.
  2010-08-04 yk   Initial revision
==============================================================================*/

#define __SNS_MODULE__ SNS_DDF

#include "sns_ddf_util.h"
#include "sns_ddf_smgr_if.h"
#include "sns_memmgr.h"
#include "sns_em.h"
#include "sns_debug_api.h"
#include "sns_memmgr.h"
#include "sns_profiling.h"

#if !defined(SNS_PCSIM) && !defined(QDSP6)
#include "DALSys.h"
#endif

// Return -1 if an int8_t is negative, or +1 otherwise.
#define SIGN_OF_INT8(x) (1 | ((x) >> 7))


typedef struct sns_ddf_timer_obj_s
{
    sns_ddf_driver_if_s*  dd_interface; /**< Functions of the calling driver. */
    sns_ddf_handle_t      dd_handle;    /**< Instance of the calling driver. */
    void*                 arg;          /**< Argument for the handler fn. */
    sns_em_timer_obj_t    em_timer;     /**< Timer service object. */
    
} sns_ddf_timer_obj_s;


/** Helper function to convert common error codes to DDF error codes. */
static sns_ddf_status_e sns_ddf_convert_common_status(sns_err_code_e err)
{
    /* For now all we need is pass/fail. */
    return (err == SNS_SUCCESS) ? SNS_DDF_SUCCESS : SNS_DDF_EFAIL;
}


sns_ddf_status_e sns_ddf_malloc(void** ptr, uint16_t size)
{
    *ptr = SNS_OS_MALLOC(SNS_DBG_MOD_DSPS_DDF, size);
    return (*ptr != NULL) ? SNS_DDF_SUCCESS : SNS_DDF_ENOMEM;
}


sns_ddf_status_e sns_ddf_mfree(void* ptr)
{
    if(ptr == NULL)
    {
        return SNS_DDF_EFAIL;
    }

    SNS_OS_FREE(ptr);
    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e sns_ddf_timer_init(
    sns_ddf_timer_s*      timer_ptr,
    sns_ddf_handle_t      dd_handle,
    sns_ddf_driver_if_s*  dd_interface,
    void*                 arg,
    bool                  periodic)
{
    sns_err_code_e status;
    sns_ddf_status_e ddf_status;
    sns_ddf_timer_s timer;

    ddf_status = sns_ddf_malloc((void**)&timer, sizeof(sns_ddf_timer_obj_s));
    if(ddf_status != SNS_DDF_SUCCESS)
        return ddf_status;

    // The driver must have a timer handler to use a timer.
    if(dd_interface->handle_timer == NULL)
        return SNS_DDF_EINVALID_PARAM;

    timer->dd_handle = dd_handle;
    timer->dd_interface = dd_interface;
    timer->arg = arg;

    /* Create the timer with SMGR's callback function since it is responsible
     * for dispatching the driver's actual callback at its discretion. */
    status = sns_em_create_timer_obj(
        sns_ddf_smgr_notify_timer,
        timer,
        periodic ? SNS_EM_TIMER_TYPE_PERIODIC : SNS_EM_TIMER_TYPE_ONESHOT,
        &timer->em_timer);

    *timer_ptr = timer;
    return sns_ddf_convert_common_status(status);
}


sns_ddf_status_e sns_ddf_timer_start(sns_ddf_timer_s timer, uint32_t usec)
{
    sns_err_code_e status;
    
    status = sns_em_register_timer(
        timer->em_timer, 
        sns_em_convert_usec_to_localtick(usec));

    return sns_ddf_convert_common_status(status);
}


sns_ddf_status_e sns_ddf_timer_cancel(sns_ddf_timer_s timer)
{
    sns_err_code_e status;
    
    status = sns_em_cancel_timer(timer->em_timer);

    return sns_ddf_convert_common_status(status);
}


sns_ddf_status_e sns_ddf_timer_release(sns_ddf_timer_s timer)
{
    sns_err_code_e status;

    status = sns_em_delete_timer_obj(timer->em_timer);
    if(status != SNS_SUCCESS)
        return sns_ddf_convert_common_status(status);

    return sns_ddf_mfree(timer);
}


void sns_ddf_timer_dispatch(sns_ddf_timer_s timer)
{
#ifndef SNS_QDSP_SIM
    sns_profiling_log_qdss(SNS_SMGR_DD_ENTER_HANDLE_TIME, 0);
    timer->dd_interface->handle_timer(timer->dd_handle, timer->arg);
    sns_profiling_log_qdss(SNS_SMGR_DD_EXIT, 1, SNS_QDSS_DD_HANDLE_TIMER);
#endif
}


sns_ddf_time_t sns_ddf_get_timestamp(void)
{
    return sns_em_get_timestamp();
}

uint32_t sns_ddf_convert_usec_to_tick(uint32_t usec)
{
    return sns_em_convert_usec_to_dspstick(usec);
}

void sns_ddf_delay(uint32_t usec)
{
  uint64_t  usec_long;
  uint32_t  start_tick, wait_tick;
#if !defined(SNS_PCSIM)
# if !defined(QDSP6)
    DALSYS_BusyWait(usec);
# else
    usec_long = usec;
    start_tick = sns_ddf_get_timestamp();
    wait_tick = (usec_long * 32768 + (1000000-1)) / 1000000;  /* at least one tick */
    while ( ( sns_ddf_get_timestamp() - start_tick ) < wait_tick )
    {
      ;
    }
# endif
#endif
}

sns_ddf_handle_t sns_ddf_mutex_create(void)
{
    return NULL;
}


sns_ddf_status_e sns_ddf_mutex_delete(sns_ddf_handle_t handle)
{
    return SNS_DDF_EFAIL;
}


sns_ddf_status_e sns_ddf_mutex_lock(sns_ddf_handle_t handle)
{
    return SNS_DDF_EFAIL;
}


sns_ddf_status_e sns_ddf_mutex_release(sns_ddf_handle_t handle)
{
    return SNS_DDF_EFAIL;
}


/** Returns true if 'value' is a valid axes map registry value. */
static bool sns_ddf_axes_map_is_reg_value_valid(const int8_t value)
{
    return (value != 0) && (value >= -3) && (value <= 3);
}

/** Returns true if 'registry_data' contains all valid axes map values. */
static bool sns_ddf_axes_map_is_reg_data_valid(const int8_t* registry_data)
{
    return (registry_data != NULL) &&
        sns_ddf_axes_map_is_reg_value_valid(registry_data[0]) &&
        sns_ddf_axes_map_is_reg_value_valid(registry_data[1]) &&
        sns_ddf_axes_map_is_reg_value_valid(registry_data[2]);
}

void sns_ddf_axes_map_init(sns_ddf_axes_map_s* axes_map, uint8_t* registry_data)
{
    const int8_t* registry_axes_map = (int8_t*)registry_data;

    if(!sns_ddf_axes_map_is_reg_data_valid(registry_axes_map))
    {
        // Default to no change in orientation.
        axes_map->indx_x = 0;
        axes_map->indx_y = 1;
        axes_map->indx_z = 2;
        axes_map->sign_x = 1;
        axes_map->sign_y = 1;
        axes_map->sign_z = 1;
        return;
    }

    axes_map->sign_x = SIGN_OF_INT8(registry_axes_map[0]);
    axes_map->indx_x = registry_axes_map[0] * axes_map->sign_x - 1;

    axes_map->sign_y = SIGN_OF_INT8(registry_axes_map[1]);
    axes_map->indx_y = registry_axes_map[1] * axes_map->sign_y - 1;

    axes_map->sign_z = SIGN_OF_INT8(registry_axes_map[2]);
    axes_map->indx_z = registry_axes_map[2] * axes_map->sign_z - 1;
}


void sns_ddf_map_axes(sns_ddf_axes_map_s* axes_map, q16_t* device_data)
{
    static q16_t tmp[3];

    SNS_OS_MEMSCPY(tmp, sizeof(tmp), device_data, sizeof(tmp));
    device_data[0] = tmp[axes_map->indx_x] * axes_map->sign_x;
    device_data[1] = tmp[axes_map->indx_y] * axes_map->sign_y;
    device_data[2] = tmp[axes_map->indx_z] * axes_map->sign_z;
}

void sns_ddf_map_axes_multiple(const sns_ddf_axes_map_s* axes_map, 
                               sns_ddf_sensor_data_s* data_ptr)
{
    q16_t tmp[3];
    uint16_t i;
    for (i=0; i<data_ptr->num_samples; i+=3) /* num_samples is multiple of 3 */
    {
        tmp[0] = data_ptr->samples[i].sample;
        tmp[1] = data_ptr->samples[i+1].sample;
        tmp[2] = data_ptr->samples[i+2].sample;
        data_ptr->samples[i].sample   = tmp[axes_map->indx_x] * axes_map->sign_x;
        data_ptr->samples[i+1].sample = tmp[axes_map->indx_y] * axes_map->sign_y;
        data_ptr->samples[i+2].sample = tmp[axes_map->indx_z] * axes_map->sign_z;
    }
}

