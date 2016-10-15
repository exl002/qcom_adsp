/********************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of "ams AG" nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
/******************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
* THIS SOFTWARE IS PROVIDED FOR USE ONLY IN CONJUNCTION WITH AMS PRODUCTS.
* USE OF THE SOFTWARE IN CONJUNCTION WITH NON-AMS-PRODUCTS IS EXPLICITLY
* EXCLUDED.
*******************************************************************************/
/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Init and get_attrib routines for ALS/PRX driver. These can be placed outside
   TCM if needed.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
----------   ---     -----------------------------------------------------------
02/15/14     fv      Convert tmd37x2 driver to tmg399x
11/14/13     fv      Convert tmd377x driver to tmd37x2
06/10/13     fv      Convert tmd277x driver to tmd377x
03/21/13     fv      Clean up code, change vendor name to "ams AG", add new copyright text.
11/14/12     fv      Modified refrence driver to use ams/taos sensor
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "stdbool.h"
#include <string.h>
#include "sns_log_api_public.h"
#include "sns_log_types_public.h"

#include "ams_tmg399x.h"
#include "sns_dd_ams_tmg399x_priv.h"
#include "ams_i2c.h"

/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
static sns_dd_head_s dd_head = { NULL, {"dd_name", 1} };


/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
extern sns_ddf_status_e sns_dd_ams_tmg399x_reset( sns_ddf_handle_t dd_handle);
extern sns_ddf_driver_if_s sns_ams_tmg399x_alsprx_driver_fn_list;
//extern sns_ddf_driver_if_s sns_dd_vendor_if_2;  //fv
extern void sns_dd_ams_tmg399x_handle_irq( sns_ddf_handle_t handle, sns_ddf_irq_e irq );
extern void ams_set_default_reg_values(sns_dd_state_t* dd_ptr);
extern void ams_set_reg_values_from_nv(sns_dd_state_t* state);
extern sns_ddf_status_e ams_calibrate_als(sns_dd_state_t* state);
extern sns_ddf_status_e sns_dd_read_registry(sns_dd_state_t* state, uint32_t sub_dev_id, sns_ddf_nv_params_s* reg_params);
extern sns_ddf_status_e ams_i2c_write_buf_db(sns_dd_state_t* state, uint8_t byteCnt, uint8_t* buf);

extern int32_t subDevId_AlsProx;
extern int32_t subDevId_RgbCt;
extern int32_t subDevId_Gesture;



/*===========================================================================*/
/*
 * @brief sns_dd_fill_sub_dev_slot: fill all info for a sub device.
 *
 * @param[in] sub_dev_slot - sub device slot to fill
 * @param[in] smgr_handle - SMGR handle for this sub device
 * @param[in] gpio - GPIO num for this sub device
 * @param[in] num_sensors - number of sensors supported by this sub device
 * @param[in] sensors - sensor list supported by this sub device
 *
 * @return number of sensors
 */
/*===========================================================================*/
static uint32_t sns_dd_fill_sub_dev_slot
(
 sns_dd_sub_dev_t* sub_dev_slot,
 sns_ddf_handle_t  smgr_handle,
 uint16_t          gpio,
 uint32_t          num_sensors,
 sns_ddf_sensor_e  sensors[]
)
{
    int i;
    uint32_t num_sensors_filled = 0;

    // Check for null-pointers
    if (NULL == sub_dev_slot)
    {
        DD_MSG_1(ERROR, "fill_sub_dev_slot - sub_dev_slot: %d", sub_dev_slot);
        return 0;
    }


    // Check bounds
    if (num_sensors > DD_NUM_SENSORS_PER_SUB_DEV)
    {
        DD_MSG_1(ERROR, "fill_sub_dev_slot - num_sensors: %d", num_sensors);
        return 0;
    }

    for( i = 0 ; i < num_sensors ; i++ )
    {
        sub_dev_slot->sensors[num_sensors_filled++] = sensors[i];
    }

    sub_dev_slot->num_sensors    = num_sensors_filled;
    sub_dev_slot->interrupt_gpio = gpio;
    sub_dev_slot->dri_enable     = false;

    if(sub_dev_slot->interrupt_gpio != 0xFFFF)
    {
        sub_dev_slot->dri_enable     = true;
    }

    sub_dev_slot->smgr_handle    = smgr_handle;
    sub_dev_slot->powerstate     = SNS_DDF_POWERSTATE_LOWPOWER;
    sub_dev_slot->odr            = 0;

    return sub_dev_slot->num_sensors;
}


/*===========================================================================*/
/*
 * @brief sns_dd_find_same_port_dev: check if a device with the input
 *                         port config has already been initialized.
 *
 * @param[in] port_config_ptr - I2C port config.
 *
 * @return NULL if a similar port device was not initialized
 *         device state pointer if already initialized
 */
/*===========================================================================*/
static sns_dd_state_t *sns_dd_find_same_port_dev(sns_ddf_port_config_s *port_config_ptr)
{
    sns_dd_state_t *cur_dd_dev_ptr = (sns_dd_state_t*) dd_head.next;

    while (cur_dd_dev_ptr)
    {
        if (true == sns_ddf_is_same_port_id(port_config_ptr, &cur_dd_dev_ptr->port_id))
        {
  	    DD_MSG_1(MED, "find_same_port_dev - cur_dev_ptr: %d", cur_dd_dev_ptr);
            return cur_dd_dev_ptr;
        }
        else
        {
            cur_dd_dev_ptr = cur_dd_dev_ptr->next;
        }
    }
    return NULL;
}


/*===========================================================================*/
/*
 * @brief sns_dd_init_timer: initialize timer for the sub device.
 *
 * @param[in] state - DD state handle.
 * @param[in] sub_dev_id
 * @param[in] sensor
 *
 * @return SNS_DDF_SUCCESS if timer init is successful else error code
 */
/*===========================================================================*/
sns_ddf_status_e sns_dd_init_timer
(
 sns_dd_state_t*   state,
 uint32_t          sub_dev_id,
 sns_ddf_sensor_e* sensor
 )
{
    sns_ddf_status_e ret = SNS_DDF_SUCCESS;

    DD_I2C_DB_3(0x0001, 0x01, *sensor);
    DD_I2C_DB_3(0x0001, 0x02, sub_dev_id);


    if ( *sensor == SNS_DDF_SENSOR_PROXIMITY )  // example prox sensor
    {
        state->sub_dev[sub_dev_id].timer_arg[0] = SNS_DDF_SENSOR_PROXIMITY;

        DD_I2C_DB_3(0x0001, 0x03, state->sub_dev[sub_dev_id].timer_arg[0]);

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[0],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[0],       // NULL,
                                 FALSE);
        // add als
        state->sub_dev[sub_dev_id].timer_arg[1] = SNS_DDF_SENSOR_AMBIENT;

        DD_I2C_DB_3(0x0001, 0x04, state->sub_dev[sub_dev_id].timer_arg[1]);

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[1],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[1],       // NULL,
                                 FALSE);
        subDevId_AlsProx        = sub_dev_id;
        state->sub_dev_0_enable = false;
    }
    else if ( *sensor == SNS_DDF_SENSOR_RGB )   // example gesture sensor
    {
        state->sub_dev[sub_dev_id].timer_arg[0] = SNS_DDF_SENSOR_RGB;

        DD_I2C_DB_3(0x0001, 0x05, state->sub_dev[sub_dev_id].timer_arg[0]);

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[0],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[0],       // NULL,
                                 FALSE);

        state->sub_dev[sub_dev_id].timer_arg[1] = SNS_DDF_SENSOR_CT_C;

        DD_I2C_DB_3(0x0001, 0x06, state->sub_dev[sub_dev_id].timer_arg[1]);

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[1],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[1],       // NULL,
                                 FALSE);

        subDevId_RgbCt          = sub_dev_id;
        state->sub_dev_1_enable = false;
    }
    else if ( *sensor == SNS_DDF_SENSOR_IR_GESTURE )   // example gesture sensor
    {
        state->sub_dev[sub_dev_id].timer_arg[0] = SNS_DDF_SENSOR_IR_GESTURE;

        ret = sns_ddf_timer_init(&state->sub_dev[sub_dev_id].timer_obj[0],
                                 (sns_ddf_handle_t)((uint32_t)state | sub_dev_id),
                                 &sns_ams_tmg399x_alsprx_driver_fn_list,
                                 &state->sub_dev[sub_dev_id].timer_arg[0],       // NULL,
                                 FALSE);

        subDevId_Gesture = sub_dev_id;
    }
    else
    {
        ret = SNS_DDF_EINVALID_PARAM;
    }

    DD_I2C_DB_2(0x0001, 0xFF);

    return ret;
}


/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_init

===========================================================================*/
/*!
  @brief Initializes the Ambient Light Sensing and Proximity device driver
   Allocates a handle to a driver instance, opens a communication port to
   associated devices, configures the driver and devices, and places
   the devices in the default power state. Returns the instance handle along
   with a list of supported sensors. This function will be called at init
   time.

   @param[out] dd_handle_ptr  Pointer that this function must malloc and
                              populate. This is a handle to the driver
                              instance that will be passed in to all other
                              functions. NB: Do not use @a memhandler to
                              allocate this memory.
   @param[in]  smgr_handle    Handle used to identify this driver when it
                              calls into Sensors Manager functions.
   @param[in]  nv_params      NV parameters retrieved for the driver.
   @param[in]  device_info    Access info for physical devices controlled by
                              this driver. Used to configure the bus
                              and talk to the devices.
   @param[in]  num_devices    Number of elements in @a device_info.
   @param[in]  memhandler     Memory handler used to dynamically allocate
                              output parameters, if applicable. NB: Do not
                              use memhandler to allocate memory for
                              @a dd_handle_ptr.
   @param[out] sensors        List of supported sensors, allocated,
                              populated, and returned by this function.
   @param[out] num_sensors    Number of elements in @a sensors.

   @return Success if @a dd_handle_ptr was allocated and the driver was
           configured properly. Otherwise a specific error code is returned.

*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_init
(
 sns_ddf_handle_t*        dd_handle_ptr,
 sns_ddf_handle_t         smgr_handle,
 sns_ddf_nv_params_s*     nv_params,
 sns_ddf_device_access_s  device_info[],
 uint32_t                 num_devices,
 sns_ddf_memhandler_s*    memhandler,
 sns_ddf_sensor_e*        sensors[],
 uint32_t*                num_sensors
)
{
    sns_dd_state_t*       state;
    sns_ddf_status_e      status = SNS_DDF_SUCCESS;
    uint32_t              dd_num_sensors = 0;
    sns_ddf_handle_t      dd_handle;
    uint16_t              i;
    uint16_t              deviceId;

    DD_MSG_0(HIGH,"sns_dd_init - enter");

    /* basic sanity check */
    if (dd_handle_ptr == NULL || sensors == NULL || num_sensors == NULL)
    {
        DD_MSG_0(ERROR, "init - Pointers are NULL");
        return SNS_DDF_EINVALID_PARAM;
    }

    DD_MSG_1(MED, "init - sensors[0] = %d", (*sensors)[0]);

    // If the device instance does not exist, then initialize it
    state = sns_dd_find_same_port_dev(&device_info->port_config);

    if ( NULL == state )
    {
        /* allocate state memory */
        if (sns_ddf_malloc((void **)&state, sizeof(sns_dd_state_t)) != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR,"malloc failed");
            return SNS_DDF_ENOMEM;
        }

        memset(state, 0, sizeof(sns_dd_state_t));

        // Put this device instance at the front of the list (all inserts into
        // the list are placed at the front)
        state->next  = NULL;         // dd_head.next;       //
        dd_head.next = state;
        state->sub_dev_count = 0;


        // skip this function if API not available
        sns_ddf_get_port_id(&device_info->port_config, &state->port_id);

        if (sns_ddf_malloc((void **)&state->s_data, sizeof(state->s_data)) != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR,"malloc failed");
            sns_ddf_mfree(state);
            return SNS_DDF_ENOMEM;
        }

        if (sns_ddf_malloc((void **)&state->samples, sizeof(state->samples)) != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR,"malloc failed");
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            return SNS_DDF_ENOMEM;
        }

        /* open I2C port */
        status = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
        if (status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR, "init: I2C open failed");
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            return status;
        }

        deviceId = ams_getField_r(state, ID, &status);
        if (status != SNS_DDF_SUCCESS)
        {
            DD_MSG_0(ERROR, "init: I2C Read failed");
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            return status;
        }

        // Now check if the device ID is valid
        if(!ams_validDeviceId(deviceId))
        {
            DD_MSG_1(ERROR, "init: device id 0x%02X is invalid", deviceId);

            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            return SNS_DDF_EFAIL;
        }

#ifdef USING_OLD_QDSP6
        // DEBUG ++++
        {
            DD_I2C_DB_3(0x0000, 0x01, *num_sensors);

            DD_I2C_DB_3(0x0000, 0xF1, sizeof(long));
            DD_I2C_DB_3(0x0000, 0xF2, sizeof(int));
            DD_I2C_DB_3(0x0000, 0xF3, sizeof(short));

            for(i = 0; i < *num_sensors; ++i)
            {
                DD_I2C_DB_3(0x0000, 0x11, (*sensors)[i]);
            }

            // TEST
            *num_sensors  = 2;
            *sensors      = sns_ddf_memhandler_malloc( memhandler, sizeof(sns_ddf_sensor_e) * *num_sensors );

#if(AMS_INIT_FIRST == AMS_INIT_PRX_ALS)
            (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
            (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
#elif(AMS_INIT_FIRST == AMS_INIT_RGB_CT)
            (*sensors)[0] = SNS_DDF_SENSOR_RGB;     // RGB
            (*sensors)[1] = SNS_DDF_SENSOR_CT_C;    // Color temp
#elif(AMS_INIT_FIRST == AMS_INIT_GESTURE)
            *num_sensors  = 1;
            (*sensors)[0] = SNS_DDF_SENSOR_IR_GESTURE;
#else
            (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
            (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
#endif
            // TEST

            DD_I2C_DB_3(0x0000, 0x02, *num_sensors);

            for(i = 0; i < *num_sensors; ++i)
            {
                DD_I2C_DB_3(0x0000, 0x21, (*sensors)[i]);
            }

            DD_MSG_1(MED, "init - Number of sensors: %d", *num_sensors);
            DD_MSG_1(MED, "init - Number of devices: %d", num_devices);
        }
        // DEBUG ----
#endif

        // Fill out the first sub-device slot
        dd_num_sensors = sns_dd_fill_sub_dev_slot( &state->sub_dev[state->sub_dev_count],
                                                   smgr_handle,
                                                   device_info->first_gpio,
                                                   *num_sensors,
                                                   *sensors);
        if (dd_num_sensors == 0)
        {
            DD_I2C_DB_2(0x0000, 3);

            DD_MSG_0(ERROR, "init - Unable to initialize any sensors.");
            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            return SNS_DDF_EINVALID_DATA;
        }

        // init timers
        status = sns_dd_init_timer(state, state->sub_dev_count, (sns_ddf_sensor_e *)*sensors);
        if (status != SNS_DDF_SUCCESS)
        {
            DD_I2C_DB_2(0x0000, 4);
            DD_MSG_0(ERROR, "sns_dd_init_timer failed");

            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            return status;
        }

        // parse registry if applicable
        status = sns_dd_read_registry(state, state->sub_dev_count, nv_params);
        if(SNS_DDF_SUCCESS != status)
        {
            DD_I2C_DB_2(0x0000, 5);
            DD_MSG_0(ERROR, "INIT - Failed to read registry");

            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            return status;
        }

	DD_MSG_1(ERROR, "init - read_registry status: %d", status);

        // Assign the dd_handle_ptr
        dd_handle = (sns_ddf_handle_t)state;
        *dd_handle_ptr = dd_handle;

        // reset the device, reset also initializes registers
        status = sns_dd_ams_tmg399x_reset(dd_handle);  // sns_dd_reset(dd_handle);
        if( status != SNS_DDF_SUCCESS )
        {
            DD_I2C_DB_3(0x0000, 6, *num_sensors);

            DD_MSG_0(ERROR, "reset failed in init");
            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree(state->samples);
            sns_ddf_mfree(state->s_data);
            sns_ddf_mfree(state);
            return status;
        }

        // Set the output and return
        *num_sensors = dd_num_sensors;
        *sensors = (sns_ddf_sensor_e *)state->sub_dev[state->sub_dev_count].sensors;

        state->sub_dev_count++;

        status = SNS_DDF_SUCCESS;

        DD_I2C_DB_3(0x0000, 0x07, *num_sensors);
        DD_I2C_DB_3(0x0000, 0x08, state->sub_dev_count);
    }
    else
    {
        DD_I2C_DB_2(0x0000, 9);
        DD_I2C_DB_3(0x0000, 0x0A, state->sub_dev_count);

        // If there is an empty slot
        if (state->sub_dev_count < DD_NUM_SUB_DEV)
        {
#ifdef USING_OLD_QDSP6
            // DEBUG ++++
            {
                DD_I2C_DB_3(0x0000, 0x0B, *num_sensors);

                for(i = 0; i < *num_sensors; ++i)
                {
                    DD_I2C_DB_3(0x0000, 0xB1, (*sensors)[i]);
                }

                // TEST
                *num_sensors  = 2;
                *sensors = sns_ddf_memhandler_malloc( memhandler, sizeof(sns_ddf_sensor_e) * *num_sensors );

#if(AMS_INIT_SECOND == AMS_INIT_RGB_CT)
                (*sensors)[0] = SNS_DDF_SENSOR_RGB;     // RGB
                (*sensors)[1] = SNS_DDF_SENSOR_CT_C;    // Color temp
#elif(AMS_INIT_SECOND == AMS_INIT_PRX_ALS)
                (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
                (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
#elif(AMS_INIT_SECOND == AMS_INIT_GESTURE)
                *num_sensors  = 1;
                (*sensors)[0] = SNS_DDF_SENSOR_IR_GESTURE;
#else
                (*sensors)[0] = SNS_DDF_SENSOR_RGB;     // RGB
                (*sensors)[1] = SNS_DDF_SENSOR_CT_C;    // Color temp
#endif
                // TEST

                DD_I2C_DB_3(0x0000, 0x0C, *num_sensors);

                for(i = 0; i < *num_sensors; ++i)
                {
                    DD_I2C_DB_3(0x0000, 0xC1, (*sensors)[i]);
                }
            }
            // DEBUG ----
#endif

            // Fill out the next sub-device slot
            dd_num_sensors = sns_dd_fill_sub_dev_slot( &state->sub_dev[state->sub_dev_count],
                                                       smgr_handle,
                                                       device_info->first_gpio,
                                                       *num_sensors,
                                                       *sensors );

            DD_I2C_DB_3(0x0000, 0x0B, dd_num_sensors);

            if (dd_num_sensors == 0)
            {
                DD_I2C_DB_2(0x0000, 0x0D);

                DD_MSG_0(ERROR, "init - Unable to initialize any sensors.");
                return SNS_DDF_EFAIL;
            }

            // init timers
            status = sns_dd_init_timer(state, state->sub_dev_count, (sns_ddf_sensor_e *)*sensors);
            if (status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x0000, 0x0E);

	        DD_MSG_0(ERROR, "init - Failed to init timers");
                return status;
            }

            // parse registry
            status = sns_dd_read_registry(state, state->sub_dev_count, nv_params);

            // Assign the dd_handle_ptr
            dd_handle = (sns_ddf_handle_t) ((uint32_t) state | state->sub_dev_count);
            *dd_handle_ptr = dd_handle;

            // Reset the sub-device
            status = sns_dd_ams_tmg399x_reset(dd_handle);
            if(status != SNS_DDF_SUCCESS)
            {
                DD_I2C_DB_2(0x0000, 0x0F);

	        DD_MSG_0(ERROR, "init - Failed to reset tmg399x");
                return status;
            }

            //*num_sensors = dd_num_sensors;
            *sensors = (sns_ddf_sensor_e *)state->sub_dev[state->sub_dev_count].sensors;

            state->sub_dev_count++;

            status = SNS_DDF_SUCCESS;
        }
        else
        {
            DD_I2C_DB_2(0x0000, 0x10);

            DD_MSG_0(ERROR, "Out of sub-device slots.");
            return SNS_DDF_ENOMEM;
        }
    }

    // DEBUG ++++
    {
        DD_I2C_DB_3(0x0000, 0x11, *num_sensors);

        for(i = 0; i < *num_sensors; ++i)
        {
            DD_I2C_DB_3(0x0000, 0x12, (*sensors)[i]);
        }

        DD_MSG_1(HIGH,"sns_dd_i1it - exit: %d", status);
    }
    // DEBUG ----

    return status;
}


/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_prx_query

DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for proximitydistance data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_prx_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  uint16_t*             thresh_ptr;
  uint16_t*             accu_ptr;
  sns_ddf_power_info_s* power_ptr;
  uint32_t*             odr_ptr;

  sns_ddf_resolution_adc_s* resp;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      device_ranges = *value;
      *num_elems = 1;
      device_ranges->min = FX_FLTTOFIX_Q16(0);
      device_ranges->max = FX_FLTTOFIX_Q16(SNS_DD_PRX_RES); /* 5 cm = 0.05m */
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      resp->bit_len  = SNS_DD_ALSPRX_PRX_BITS;
      resp->max_freq = SNS_DD_ALSPRX_PRX_FREQ;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;

      *resp = FX_FLTTOFIX_Q16(SNS_DD_PRX_RES);
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current       = SNS_DD_PRX_PWR;
      power_ptr->lowpower_current     = SNS_DD_ALSPRX_LO_PWR;
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *accu_ptr = SNS_DD_PRX_ACCURACY;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_THRESHOLD:
      if ( (thresh_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *thresh_ptr = SNS_DD_PRX_THRESH_NEAR;
      *(uint16_t **)value = thresh_ptr;
      DD_MSG_0(HIGH, "get_attrib threshold");

      break;

    case SNS_DDF_ATTRIB_ODR:
      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *odr_ptr   = dd_ptr->chip.setup.prox.odr;
      *(uint32_t **)value = odr_ptr;
      DD_MSG_1(HIGH, "TMG399x get_attrib prx odr: %d",  *odr_ptr);
      break;

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_ams_tmg399x_als_query

DESCRIPTION   Called by sns_dd_alsprx_query to get an attribute value for ALS(light) data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_als_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  uint16_t*             accu_ptr;
  sns_ddf_power_info_s* power_ptr;
  uint32_t*             odr_ptr;

  sns_ddf_resolution_adc_s* resp;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler,
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      DD_MSG_0(HIGH, "in get_attr range");
      device_ranges = *value;
      *num_elems = 1;
      device_ranges->min = FX_FLTTOFIX_Q16(SNS_DD_ALS_RANGES_MIN);
      device_ranges->max = FX_FLTTOFIX_Q16(SNS_DD_ALS_RANGES_MAX);
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      DD_MSG_0(HIGH, "in get_attr resolution ADC");
      *num_elems = 1;
      resp->bit_len  = SNS_DD_ALSPRX_ALS_BITS;
      resp->max_freq = SNS_DD_ALSPRX_ALS_FREQ;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      DD_MSG_0(HIGH, "in get_attr resolution");
      *num_elems = 1;

      *resp = FX_FLTTOFIX_Q16(SNS_DD_ALS_RES);
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current   = SNS_DD_ALS_PWR;
      power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
     *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      dd_ptr->chip.als_info.accuracy = dd_ptr->chip.als_info.lux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
      if ( dd_ptr->chip.als_info.accuracy < SNS_DD_ALS_HALF_LUX )
      {
        dd_ptr->chip.als_info.accuracy = SNS_DD_ALS_HALF_LUX;
      }
      DD_MSG_0(HIGH, "accuracy");
      *accu_ptr = dd_ptr->chip.als_info.accuracy;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_ODR:
      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *odr_ptr   = dd_ptr->chip.setup.als.odr;
      *(uint32_t **)value = odr_ptr;
      DD_MSG_1(HIGH, "TMG399x get_attrib als odr: %d",  *odr_ptr);
      break;

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_ams_tmg399x_rgb_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
    //
    // handle generic attributes for RGB
    //
    // Same as ALS
    //
    sns_ddf_power_info_s* power_ptr;
    uint16_t*             accu_ptr;
    uint32_t*             odr_ptr;

    sns_ddf_resolution_adc_s* resp;

    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *device_ranges;
            if( (*value = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_range_s)))  == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr range");
            device_ranges = *value;
            *num_elems = 1;
            device_ranges->min = FX_FLTTOFIX_Q16(0.01);
            device_ranges->max = FX_FLTTOFIX_Q16(10000);
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution ADC");
            *num_elems = 1;
            resp->bit_len  = SNS_DD_ALSPRX_ALS_BITS;
            resp->max_freq = SNS_DD_ALSPRX_ALS_FREQ;
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t* resp;
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution");
            *num_elems = 1;

            *resp = FX_FLTTOFIX_Q16(SNS_DD_ALS_RES);
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_POWER_INFO:
        if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        power_ptr->active_current   = SNS_DD_ALS_PWR;
        power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
        *(sns_ddf_power_info_s **)value = power_ptr;
        break;

    case SNS_DDF_ATTRIB_ACCURACY:
        if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        dd_ptr->chip.als_info.accuracy = dd_ptr->chip.als_info.lux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
        if ( dd_ptr->chip.als_info.accuracy < SNS_DD_ALS_HALF_LUX )
        {
            dd_ptr->chip.als_info.accuracy = SNS_DD_ALS_HALF_LUX;
        }
        DD_MSG_0(HIGH, "accuracy");
        *accu_ptr = dd_ptr->chip.als_info.accuracy;
        *(uint16_t **)value = accu_ptr;
        break;

    case SNS_DDF_ATTRIB_ODR:
        if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        *odr_ptr   = dd_ptr->chip.setup.rgb.odr;
        *(uint32_t **)value = odr_ptr;
        DD_MSG_1(HIGH, "TMG399x get_attrib als odr: %d",  *odr_ptr);
        break;

    default:
        return SNS_DDF_EINVALID_ATTR;
    }
    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_ams_tmg399x_ct_c_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
    // handle generic attributes for CT_C
    sns_ddf_power_info_s* power_ptr;
    uint16_t*             accu_ptr;
    uint32_t*             odr_ptr;

    sns_ddf_resolution_adc_s* resp;

    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *device_ranges;
            if( (*value = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_range_s)))  == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr range");
            device_ranges = *value;
            *num_elems = 1;
            device_ranges->min = FX_FLTTOFIX_Q16(0.01);
            device_ranges->max = FX_FLTTOFIX_Q16(10000);
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution ADC");
            *num_elems = 1;
            resp->bit_len  = SNS_DD_ALSPRX_ALS_BITS;
            resp->max_freq = SNS_DD_ALSPRX_ALS_FREQ;
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t* resp;
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution");
            *num_elems = 1;

            *resp = FX_FLTTOFIX_Q16(SNS_DD_ALS_RES);
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_POWER_INFO:
        if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        power_ptr->active_current   = SNS_DD_ALS_PWR;
        power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
        *(sns_ddf_power_info_s **)value = power_ptr;
        break;

    case SNS_DDF_ATTRIB_ACCURACY:
        if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        dd_ptr->chip.als_info.accuracy = dd_ptr->chip.als_info.lux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
        if ( dd_ptr->chip.als_info.accuracy < SNS_DD_ALS_HALF_LUX )
        {
            dd_ptr->chip.als_info.accuracy = SNS_DD_ALS_HALF_LUX;
        }
        DD_MSG_0(HIGH, "accuracy");
        *accu_ptr = dd_ptr->chip.als_info.accuracy;
        *(uint16_t **)value = accu_ptr;
        break;

    case SNS_DDF_ATTRIB_ODR:
        if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        *odr_ptr   = dd_ptr->chip.setup.ct_c.odr;
        *(uint32_t **)value = odr_ptr;
        DD_MSG_1(HIGH, "TMG399x get_attrib als odr: %d",  *odr_ptr);
        break;

    default:
        return SNS_DDF_EINVALID_ATTR;
    }
    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_ams_tmg399x_ir_gesture_query
(
  sns_dd_state_t*        dd_ptr,
  uint32_t               sub_dev_id,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
    // handle generic attributes for gesture
    sns_ddf_power_info_s* power_ptr;
    uint16_t*             accu_ptr;
    uint32_t*             odr_ptr;

    sns_ddf_resolution_adc_s* resp;

    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *device_ranges;
            if( (*value = sns_ddf_memhandler_malloc(memhandler,
                                                    sizeof(sns_ddf_range_s)))  == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr range");
            device_ranges = *value;
            *num_elems = 1;
            device_ranges->min = FX_FLTTOFIX_Q16(0.01);
            device_ranges->max = FX_FLTTOFIX_Q16(10000);
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution ADC");
            *num_elems = 1;
            resp->bit_len  = SNS_DD_ALSPRX_ALS_BITS;
            resp->max_freq = SNS_DD_ALSPRX_ALS_FREQ;
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            sns_ddf_resolution_t* resp;
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "in get_attr resolution");
            *num_elems = 1;

            *resp = FX_FLTTOFIX_Q16(SNS_DD_ALS_RES);
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_POWER_INFO:
        if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        power_ptr->active_current   = SNS_DD_ALS_PWR;
        power_ptr->lowpower_current = SNS_DD_ALSPRX_LO_PWR;
        *(sns_ddf_power_info_s **)value = power_ptr;
        break;

    case SNS_DDF_ATTRIB_ACCURACY:
        if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        dd_ptr->chip.als_info.accuracy = dd_ptr->chip.als_info.lux / SNS_DD_ALS_ACCURACY_DIV_FACTOR;
        if ( dd_ptr->chip.als_info.accuracy < SNS_DD_ALS_HALF_LUX )
        {
            dd_ptr->chip.als_info.accuracy = SNS_DD_ALS_HALF_LUX;
        }
        DD_MSG_0(HIGH, "accuracy");
        *accu_ptr = dd_ptr->chip.als_info.accuracy;
        *(uint16_t **)value = accu_ptr;
        break;

    case SNS_DDF_ATTRIB_ODR:
        if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
        {
            return SNS_DDF_ENOMEM;
        }
        *num_elems = 1;
        *odr_ptr   = dd_ptr->chip.setup.gesture.odr;
        *(uint32_t **)value = odr_ptr;
        DD_MSG_1(HIGH, "TMG399x get_attrib als odr: %d",  *odr_ptr);
        break;

    default:
        return SNS_DDF_EINVALID_ATTR;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_ams_tmg399x_get_attrib

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  Returns the requested attribute

  @param[in]  handle      Handle to a driver instance.
  @param[in]  sensor      Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in]  memhandler  Memory handler used to dynamically allocate
                          output parameters, if applicable.
  @param[out] value       Pointer that this function will allocate or set
                          to the attribute's value.
  @param[out] num_elems   Number of elements in @a value.

  @return
    Success if the attribute was retrieved and the buffer was
    populated. Otherwise a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_ams_tmg399x_get_attrib
(
  sns_ddf_handle_t      handle,
  sns_ddf_sensor_e      sensor,
  sns_ddf_attribute_e   attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
)
{
    sns_dd_state_t*  state      = (sns_dd_state_t*)(((uint32_t)handle) & (uint32_t)(~DD_HANDLE_ALIGN));
    uint32_t         sub_dev_id = (uint32_t)handle & ((uint32_t)DD_HANDLE_ALIGN);

    sns_ddf_status_e           status         = SNS_DDF_SUCCESS;
    bool                       generic_attrib = false;
    sns_ddf_registry_group_s  *reg_group;
    uint8_t                   *reg_group_data;
    sns_ddf_driver_info_s     *driver_info_ptr;
    sns_ddf_driver_info_s      driver_info_l = {
        "TMG399X_ALS_Prox",  /* name */
        1                    /* version */
    };

    sns_ddf_driver_info_s  driver_info_2 = {
        "TMG399X_RGB_CT",    /* name */
        1                    /* version */
    };

    sns_ddf_driver_info_s  driver_info_3 = {
        "TMG399X_Gesture",    /* name */
        1                    /* version */
    };

    sns_ddf_device_info_s *device_info_ptr;

    sns_ddf_device_info_s  device_info_l = {
        "ALS_PRX",     /* name */
        "ams AG",      /* vendor */
        "TMG399X",     /* model */
        1              /* version */
    };

    sns_ddf_device_info_s  device_info_2 = {
        "RGB_CT",      /* name */
        "ams AG",      /* vendor */
        "TMG399X",     /* model */
        1              /* version */
    };

    sns_ddf_device_info_s  device_info_3 = {
        "Gesture",     /* name */
        "ams AG",      /* vendor */
        "TMG399X",     /* model */
        1              /* version */
    };

    DD_I2C_DB_3(0x3000, 0x00, sensor);
    DD_I2C_DB_3(0x3000, 0x01, attrib);
    DD_I2C_DB_3(0x3000, 0x02, *num_elems);
    DD_I2C_DB_3(0x3000, 0x03, sub_dev_id);

    if ( (handle == NULL)      ||
         (memhandler == NULL) )
    {
        DD_I2C_DB_2(0x3000, 0x04);

        return SNS_DDF_EINVALID_PARAM;
    }


    /* check first if the query is for generic attributes */
    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_DRIVER_INFO:
        DD_I2C_DB_2(0x3000, 0x05);

        if ( (driver_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s))) == NULL )
        {
            DD_I2C_DB_2(0x3000, 0x06);

            return SNS_DDF_ENOMEM;
        }

        if(sub_dev_id == subDevId_AlsProx)
        {
            DD_I2C_DB_2(0x3000, 0x07);

            *driver_info_ptr = driver_info_l;
        }
        else if(sub_dev_id == subDevId_RgbCt)
        {
            DD_I2C_DB_2(0x3000, 0x08);

            *driver_info_ptr = driver_info_2;
        }
        else if(sub_dev_id == subDevId_Gesture)
        {
            DD_I2C_DB_2(0x3000, 0xF8);

            *driver_info_ptr = driver_info_3;
        }
        else
        {
            DD_I2C_DB_2(0x3000, 0x09);

            return SNS_DDF_EINVALID_PARAM;
        }

        *(sns_ddf_driver_info_s**)value = driver_info_ptr;
        *num_elems = 1;
        generic_attrib = false;
        return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
        DD_I2C_DB_2(0x3000, 0x0A);

        if ( (device_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s))) == NULL )
        {
            DD_I2C_DB_2(0x3000, 0x0B);

            return SNS_DDF_ENOMEM;
        }

        if(sub_dev_id == subDevId_AlsProx)
        {
            DD_I2C_DB_2(0x3000, 0xA1);

            *device_info_ptr = device_info_l;
        }
        else if(sub_dev_id == subDevId_RgbCt)
        {
            DD_I2C_DB_2(0x3000, 0xA2);

            *device_info_ptr = device_info_2;
        }
        else if(sub_dev_id == subDevId_Gesture)
        {
            DD_I2C_DB_2(0x3000, 0xA3);

            *device_info_ptr = device_info_3;
        }
        else
        {
            DD_I2C_DB_2(0x3000, 0xA4);

            return SNS_DDF_EINVALID_PARAM;
        }

        *(sns_ddf_device_info_s**)value = device_info_ptr; /* Is memcopy needed instead? */
        *num_elems = 1;
        generic_attrib = false;
        return SNS_DDF_SUCCESS;


    case SNS_DDF_ATTRIB_REGISTRY_GROUP:
        DD_I2C_DB_2(0x3000, 0x0C);

        DD_MSG_0(HIGH, "in attrib reg group");
        if ( (reg_group = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_registry_group_s))) == NULL )
        {
            DD_I2C_DB_2(0x3000, 0x0D);
            DD_MSG_0(ERROR, "reg_group no mem");

	    return SNS_DDF_ENOMEM;
        }

        if (state->nv_db_size == 0)
        {
            DD_I2C_DB_2(0x3000, 0x0E);

            reg_group->group_data = NULL;
            reg_group->size       = 0;

            DD_MSG_0(HIGH, "nv db size 0");
        }
        else
        {
            DD_I2C_DB_2(0x3000, 0x0F);

            if ( (reg_group_data = sns_ddf_memhandler_malloc(memhandler, state->nv_db_size)) == NULL )
            {
                DD_MSG_0(ERROR, "reg_group no mem");

                return SNS_DDF_ENOMEM;
            }
            DD_MSG_0(HIGH, "at memcopy");

            memcpy(reg_group_data, &state->nv_db, sizeof(sns_dd_nv_db_type));
            reg_group->group_data = reg_group_data;
            reg_group->size = state->nv_db_size;
            DD_MSG_0(HIGH, "after memcopy");
        }

        *(sns_ddf_registry_group_s**)value = reg_group;
        *num_elems = 1;
        generic_attrib = true;
        DD_MSG_0(HIGH, "out of attrib group");
        return SNS_DDF_SUCCESS;

    default:
        DD_I2C_DB_2(0x3000, 0x11);

        /* do nothing */
        break;
    }

    /* The query is not for generic attribute but is for specific data type */
    if ( generic_attrib != true )
    {
        DD_I2C_DB_2(0x3000, 0x12);

        switch ( sensor )
        {
        case SNS_DDF_SENSOR_AMBIENT:
            DD_I2C_DB_2(0x3000, 0x13);

            status = sns_dd_ams_tmg399x_als_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        case SNS_DDF_SENSOR_PROXIMITY:
            DD_I2C_DB_2(0x3000, 0x14);

            status = sns_dd_ams_tmg399x_prx_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        case SNS_DDF_SENSOR_RGB:
            DD_I2C_DB_2(0x3000, 0x15);

            status = sns_dd_ams_tmg399x_rgb_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        case SNS_DDF_SENSOR_CT_C:
            DD_I2C_DB_2(0x3000, 0x16);

            status = sns_dd_ams_tmg399x_ct_c_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        case SNS_DDF_SENSOR_IR_GESTURE:
            DD_I2C_DB_2(0x3000, 0x17);

            status = sns_dd_ams_tmg399x_ir_gesture_query(state, sub_dev_id, memhandler, attrib, value, num_elems);
            break;

        default:
            DD_I2C_DB_2(0x3000, 0x18);

            status = SNS_DDF_EINVALID_ATTR;
        }
    }

    DD_I2C_DB_2(0x3000, 0xFF);

    return status;
}
