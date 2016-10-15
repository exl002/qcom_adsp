#ifndef SNS_SMGR_TEST_DRI_H
#define SNS_SMGR_TEST_DRI_H
/*=============================================================================
  @file sns_smgr_test_dri.h
 
  This header file contains definitions used in DRI test code.
 
******************************************************************************   
*   Copyright (c) 2012 Qualcomm Technologies Incorporated. 
*   All Rights Reserved.
*   Qualcomm Confidential and Proprietary
*
******************************************************************************/

/* $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/smgr/src/common/sns_smgr_test_dri.h#1 $ */
/* $DateTime: 2014/05/16 10:56:56 $ */
/* $Author: coresvc $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2012-03-01  PN   Initial version
  
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
void sns_test_dri_init( uint8_t**, uint8_t* );
void sns_test_dri_round_trip_delay( boolean b_req_rcvd );
void sns_test_dri_sampling_latency( void );
void sns_test_dri_timestamp_delay( void );
boolean sns_test_dri_init_done( void );
#endif /* #ifndef SNS_SMGR_TEST_DRI_H */
