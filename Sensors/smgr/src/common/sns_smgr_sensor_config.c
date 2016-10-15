/*=============================================================================
  @file sns_smgr_sensor_config.c

  This file initializes the constant table using the sensor configuraiton file.

*******************************************************************************
*   Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
*   All Rights Reserved.
*   Qualcomm Confidential and Proprietary
*
********************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_sensor_config.c#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-12-17  ps   Added more sensors config data for ADSP_STANDALONE mode
  2013-02-25  ps   Added macros to support ADSP_STANDALONE mode
  2013-01-09  sd   Added support for SSI
  2012-05-01  vh   Added SNS_SMGR_SENSOR_<N>_BUS_INSTANCE to smgr_sensor_cfg_s
  2012-01-23  br   inserted a few fields for supporting DRI
  2011-08-04  br   inserted sns_smgr_get_sensor_config_info
  2011-07-18  br   seperated from sns_smgr_sensor.c and created this file


============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"

#include "sns_smgr_define.h"
#include "sns_dd.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "fixed_point.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_smgr_hw.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#undef DUMMY_ACCEL

/* Determine whether a sensor sample is short enough to be done in a batch
   of "immediate" reads. If longer, it is completed by a callback */
#define BATCH_READ_TIME_LIMIT_USEC  500
#define BATCH_READ_TIME_LIMIT_TICK  (uint32_t)((BATCH_READ_TIME_LIMIT_USEC/SNS_SMGR_USEC_PER_TICK)+1)

#define SMGR_LONG_READ_TICKS        (uint32_t)((SMGR_LONG_READ_USEC/SNS_SMGR_USEC_PER_TICK)+1)

/* Number of reset/retries before declaring a device failed */
#define SMGR_RETRY_LIMIT            2

/* Define SMGR_DD_INIT_DELAY to delay device driver initialization. This
   delay is added to work around I2C issue. */
#define SMGR_DD_INIT_DELAY
#define SMGR_DD_INIT_DELAY_US       3000000 /* 3.0s */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

  /* Constants for each sensor. Initialized from definitions in
     sns_smgr_sensor_config.h */
#ifdef ADSP_STANDALONE
smgr_sensor_cfg_s smgr_sensor_cfg[ SNS_SMGR_NUM_SENSORS_DEFINED ] =
  {
   {
    SNS_SMGR_SENSOR_0_DD_FN_LIST_PTR,
    (uint16_t)((SNS_SMGR_SENSOR_0_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_0_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_0_ENUM_CODE,
    SNS_SMGR_SENSOR_0_BUS_ADDRESS,
    SNS_SMGR_SENSOR_0_RANGE_TYPE,
    SNS_SMGR_SENSOR_0_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_0_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_0_REG_ITEM_ID,
    SNS_SMGR_SENSOR_0_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_0_CAL_PRI_ID,
    SNS_SMGR_SENSOR_0_DEVICE_ID,
    SNS_SMGR_SENSOR_0_FLAGS,
    {SNS_SMGR_SENSOR_0_DATA_TYPE_1, SNS_SMGR_SENSOR_0_DATA_TYPE_2},
    SNS_SMGR_SENSOR_0_GPIO_FIRST,
    SNS_SMGR_SENSOR_0_GPIO_SECOND,
    SNS_SMGR_SENSOR_0_BUS_INSTANCE
   },

   {
    SNS_SMGR_SENSOR_1_DD_FN_LIST_PTR,
    (uint16_t)((SNS_SMGR_SENSOR_1_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_1_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_1_ENUM_CODE,
    SNS_SMGR_SENSOR_1_BUS_ADDRESS,
    SNS_SMGR_SENSOR_1_RANGE_TYPE,
    SNS_SMGR_SENSOR_1_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_1_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_1_REG_ITEM_ID,
    SNS_SMGR_SENSOR_1_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_1_CAL_PRI_ID,
    SNS_SMGR_SENSOR_1_DEVICE_ID,
    SNS_SMGR_SENSOR_1_FLAGS,
    {SNS_SMGR_SENSOR_1_DATA_TYPE_1, SNS_SMGR_SENSOR_1_DATA_TYPE_2},
    SNS_SMGR_SENSOR_1_GPIO_FIRST,
    SNS_SMGR_SENSOR_1_GPIO_SECOND,
    SNS_SMGR_SENSOR_1_BUS_INSTANCE
   },

   {
    SNS_SMGR_SENSOR_2_DD_FN_LIST_PTR,
    (uint16_t)((SNS_SMGR_SENSOR_2_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_2_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_2_ENUM_CODE,
    SNS_SMGR_SENSOR_2_BUS_ADDRESS,
    SNS_SMGR_SENSOR_2_RANGE_TYPE,
    SNS_SMGR_SENSOR_2_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_2_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_2_REG_ITEM_ID,
    SNS_SMGR_SENSOR_2_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_2_CAL_PRI_ID,
    SNS_SMGR_SENSOR_2_DEVICE_ID,
    SNS_SMGR_SENSOR_2_FLAGS,
    {SNS_SMGR_SENSOR_2_DATA_TYPE_1, SNS_SMGR_SENSOR_2_DATA_TYPE_2},
    SNS_SMGR_SENSOR_2_GPIO_FIRST,
    SNS_SMGR_SENSOR_2_GPIO_SECOND,
    SNS_SMGR_SENSOR_2_BUS_INSTANCE
   },

   {
    SNS_SMGR_SENSOR_3_DD_FN_LIST_PTR,
    (uint16_t)((SNS_SMGR_SENSOR_3_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_3_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_3_ENUM_CODE,
    SNS_SMGR_SENSOR_3_BUS_ADDRESS,
    SNS_SMGR_SENSOR_3_RANGE_TYPE,
    SNS_SMGR_SENSOR_3_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_3_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_3_REG_ITEM_ID,
    SNS_SMGR_SENSOR_3_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_3_CAL_PRI_ID,
    SNS_SMGR_SENSOR_3_DEVICE_ID,
    SNS_SMGR_SENSOR_3_FLAGS,
    {SNS_SMGR_SENSOR_3_DATA_TYPE_1, SNS_SMGR_SENSOR_3_DATA_TYPE_2},
    SNS_SMGR_SENSOR_3_GPIO_FIRST,
    SNS_SMGR_SENSOR_3_GPIO_SECOND,
    SNS_SMGR_SENSOR_3_BUS_INSTANCE
   },

   {
    SNS_SMGR_SENSOR_4_DD_FN_LIST_PTR,
    (uint16_t)((SNS_SMGR_SENSOR_4_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_4_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_4_ENUM_CODE,
    SNS_SMGR_SENSOR_4_BUS_ADDRESS,
    SNS_SMGR_SENSOR_4_RANGE_TYPE,
    SNS_SMGR_SENSOR_4_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_4_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_4_REG_ITEM_ID,
    SNS_SMGR_SENSOR_4_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_4_CAL_PRI_ID,
    SNS_SMGR_SENSOR_4_DEVICE_ID,
    SNS_SMGR_SENSOR_4_FLAGS,
    {SNS_SMGR_SENSOR_4_DATA_TYPE_1, SNS_SMGR_SENSOR_4_DATA_TYPE_2},
    SNS_SMGR_SENSOR_4_GPIO_FIRST,
    SNS_SMGR_SENSOR_4_GPIO_SECOND,
    SNS_SMGR_SENSOR_4_BUS_INSTANCE
   },
   
   {
    SNS_SMGR_SENSOR_5_DD_FN_LIST_PTR,
    (uint16_t)((SNS_SMGR_SENSOR_5_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_5_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_5_ENUM_CODE,
    SNS_SMGR_SENSOR_5_BUS_ADDRESS,
    SNS_SMGR_SENSOR_5_RANGE_TYPE,
    SNS_SMGR_SENSOR_5_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_5_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_5_REG_ITEM_ID,
    SNS_SMGR_SENSOR_5_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_5_CAL_PRI_ID,
    SNS_SMGR_SENSOR_5_DEVICE_ID,
    SNS_SMGR_SENSOR_5_FLAGS,
    {SNS_SMGR_SENSOR_5_DATA_TYPE_1, SNS_SMGR_SENSOR_5_DATA_TYPE_2},
    SNS_SMGR_SENSOR_5_GPIO_FIRST,
    SNS_SMGR_SENSOR_5_GPIO_SECOND,
    SNS_SMGR_SENSOR_5_BUS_INSTANCE
   },

   {
    SNS_SMGR_SENSOR_6_DD_FN_LIST_PTR,
    (uint16_t)((SNS_SMGR_SENSOR_6_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_6_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_6_ENUM_CODE,
    SNS_SMGR_SENSOR_6_BUS_ADDRESS,
    SNS_SMGR_SENSOR_6_RANGE_TYPE,
    SNS_SMGR_SENSOR_6_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_6_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_6_REG_ITEM_ID,
    SNS_SMGR_SENSOR_6_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_6_CAL_PRI_ID,
    SNS_SMGR_SENSOR_6_DEVICE_ID,
    SNS_SMGR_SENSOR_6_FLAGS,
    {SNS_SMGR_SENSOR_6_DATA_TYPE_1, SNS_SMGR_SENSOR_6_DATA_TYPE_2},
    SNS_SMGR_SENSOR_6_GPIO_FIRST,
    SNS_SMGR_SENSOR_6_GPIO_SECOND,
    SNS_SMGR_SENSOR_6_BUS_INSTANCE
   },

   {
    SNS_SMGR_SENSOR_7_DD_FN_LIST_PTR,
    (uint16_t)((SNS_SMGR_SENSOR_7_OFF_TO_IDLE/SNS_SMGR_USEC_PER_TICK)+1),
    (uint16_t)((SNS_SMGR_SENSOR_7_IDLE_TO_READY/SNS_SMGR_USEC_PER_TICK)+1),
    SNS_SMGR_SENSOR_7_ENUM_CODE,
    SNS_SMGR_SENSOR_7_BUS_ADDRESS,
    SNS_SMGR_SENSOR_7_RANGE_TYPE,
    SNS_SMGR_SENSOR_7_SENSITIVITY_DEFAULT,
    SNS_SMGR_SENSOR_7_REG_ITEM_TYPE,
    SNS_SMGR_SENSOR_7_REG_ITEM_ID,
    SNS_SMGR_SENSOR_7_CAL_PRI_TYPE,
    SNS_SMGR_SENSOR_7_CAL_PRI_ID,
    SNS_SMGR_SENSOR_7_DEVICE_ID,
    SNS_SMGR_SENSOR_7_FLAGS,
    {SNS_SMGR_SENSOR_7_DATA_TYPE_1, SNS_SMGR_SENSOR_7_DATA_TYPE_2},
    SNS_SMGR_SENSOR_7_GPIO_FIRST,
    SNS_SMGR_SENSOR_7_GPIO_SECOND,
    SNS_SMGR_SENSOR_7_BUS_INSTANCE
   }
  };
#endif /* ADSP_STANDALONE */

/*===========================================================================

  FUNCTION:   sns_smgr_get_sensor_config_info

===========================================================================*/
/*!
  @brief This function returns the sensor configuration information.
  
  @detail  retuns the pointer to smgr_sensor_cfg table and the definition value of SNS_SMGR_NUM_SENSORS_DEFINED

  @param[o] num_sensor_ptr  pointer to a variable in which SNS_SMGR_NUM_SENSORS_DEFINED is saved.

  @return  the address of smgr_sensor_cfg table
 */
/*=========================================================================*/
smgr_sensor_cfg_s * sns_smgr_get_sensor_config_info (uint32_t *num_sensor_ptr)
{
  *num_sensor_ptr = SNS_SMGR_NUM_SENSORS_DEFINED;
  return smgr_sensor_cfg;
} 
 

