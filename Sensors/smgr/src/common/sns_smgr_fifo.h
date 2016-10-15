
#ifndef SNS_SMGR_FIFO_H
#define SNS_SMGR_FIFO_H

/*=============================================================================
  @file sns_smgr_fifo.h

  This header file contains the definitions used within the DSPS
  Sensor Manager (SMGR) FIFO

******************************************************************************
* Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
******************************************************************************/

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-04-07  sd   Added define for reducing FIFO wm when ODR is high
  2014-03-06  jms  Removing synchronization and enabling separate interrupt for each fifo
  2014-01-12  jms  Merged discrete with unified fifo support
  2013-12-23  jms  Initial version

============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <sns_smgr.h>
#include "sns_smgr_sensor_config.h"
#include "sns_ddf_common.h"
//#include "sns_smgr_define.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

//#define SNS_SMGR_FIFO_DEBUG  //ENEBLE FIFO DEBUG MSGS
#ifdef SNS_SMGR_FIFO_DEBUG
#define SNS_SMGR_FIFO_DBG0 SNS_SMGR_PRINTF0
#define SNS_SMGR_FIFO_DBG1 SNS_SMGR_PRINTF1
#define SNS_SMGR_FIFO_DBG2 SNS_SMGR_PRINTF2
#define SNS_SMGR_FIFO_DBG3 SNS_SMGR_PRINTF3
#else
#define SNS_SMGR_FIFO_DBG0(level,msg)
#define SNS_SMGR_FIFO_DBG1(level,msg,p1)
#define SNS_SMGR_FIFO_DBG2(level,msg,p1,p2)
#define SNS_SMGR_FIFO_DBG3(level,msg,p1,p2,p3)
#endif

#define SNS_SMGR_FIFO_MAX_DISCR_FIFOS 3

#define SNS_SMGR_FIFO_IS_UNIFIED(sensor_ptr) (sensor_ptr->fifo_cfg.shared_sensor_cnt > 0)

#define SNS_SMGR_FIFO_REDUCE_WM_HIGH_ODR   200

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/


//--------------- Unified fifo -----------------
typedef struct
{
  smgr_sensor_s*      sensor_ptr;
  smgr_sensor_id_e    sibling_sensor_id;
  sns_ddf_sensor_e    sibling_ddf_sensor_type;
  sns_ddf_handle_t*   sibling_dd_handle_ptr;
} smgr_fifo_shared_sensor_s;

//------------ General fifo support --------------
typedef struct
{
  bool                      is_fifo_enabled;        //true if ( WM > 0 )
  bool                      can_fifo_be_used;
  bool                      is_datatype_requested[SMGR_MAX_DATA_TYPES_PER_DEVICE];
  uint16_t                  suitable_max_watermark; //reserves space for time to read fifo
  uint16_t                  current_watermark;
  uint16_t                  desired_watermark;      //newly requested WM size to set
  uint16_t                  max_req_sample_rate;
  q16_t                     max_req_rpt_rate;
  uint16_t                  req_rpts_cnt; //number of reports requested for this sensor

  //TODO: ----- smgr_fifo_cfg_const_s ------
  //Set only once in sns_smgr_fifo_init_fifo_cfg on get_attr
  bool                      is_fifo_supported;
  bool                      is_hw_watermark_int_supported;
  uint16_t                  max_watermark;          //hardware FIFO size in samples
  uint16_t                  shared_sensor_cnt;     //sensors sharing unified fifo
  smgr_fifo_shared_sensor_s shared_ddf_sensor[SNS_SMGR_NUM_SENSORS_DEFINED*SMGR_MAX_DATA_TYPES_PER_DEVICE];

} smgr_sensor_fifo_cfg_s;

//--------------- Discrete fifo ----------------
typedef struct
{
  /* The lead sensor which interrupt is used as main interrupt for fifo sync. */
  smgr_ddf_sensor_s*   ddf_sensor_sync_intr_ptr;
  /* Fifo sensors active in Report Database */
  smgr_ddf_sensor_s*   ddf_sensors_active_ptr[SNS_SMGR_FIFO_MAX_DISCR_FIFOS];
} sns_smgr_fifo_s;


/*----------------------------------------------------------------------------
 * Public Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
sns_err_code_e sns_smgr_fifo_init_fifo_cfg( smgr_sensor_s *sensor_ptr );

void sns_smgr_fifo_on_event_odr_changed
(
    smgr_ddf_sensor_s* ddf_sensor_ptr,
    sns_ddf_sensor_e   sensor
);
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_wm_int
(
    const smgr_sensor_s* sensor_ptr,
    sns_ddf_sensor_e     sensor
);
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_overflow
(
    const smgr_sensor_s* sensor_ptr,
    sns_ddf_sensor_e     sensor
);
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_available
(
    smgr_sensor_s*    sensor_ptr,
    sns_ddf_sensor_e  sensor
);
sns_ddf_status_e sns_smgr_fifo_on_event_fifo_unavailable
(
    smgr_sensor_s*    sensor_ptr,
    sns_ddf_sensor_e  sensor
);


void     sns_smgr_fifo_flush( const smgr_sensor_s* sensor_ptr );

uint16_t sns_smgr_fifo_get_suitable_max_watermark( const smgr_sensor_s* sensor_ptr );
void     sns_smgr_fifo_configure( smgr_sensor_s* sensor_ptr );


/*----------------------------------------------------------------------------
 * Extern Function Declarations
 * -------------------------------------------------------------------------*/
void smgr_get_data(smgr_ddf_sensor_s* in_ddf_sensor_ptr);




#endif /* #ifndef SNS_SMGR_FIFO_H */
