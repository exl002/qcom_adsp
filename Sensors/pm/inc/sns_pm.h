#ifndef SNS_PM_H
#define SNS_PM_H

/*============================================================================

  @file sns_pm.h

  @brief
  This file contains definitions for use by other Sensor Modules.

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

============================================================================*/


/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 
//source/qcom/qct/core/sensors/dsps/dsps/main/latest/pm/inc/sns_pm.h#1 
$ 


when         who     what, where, why
----------   ---     ---------------------------------------------------------
04-29-2011   jh      Removed compiler warning 
03-31-2011   sj      Initial version
========================================================================*/
 
/*=======================================================================
                   INCLUDES
========================================================================*/
#include "sns_common.h"


/*========================================================================
                   FUNCTIONS
========================================================================*/
/*===========================================================================

  FUNCTION:   sns_pm_main

===========================================================================*/
/*!
  @brief
  This function is the main pm thread function that pends on signals 
   
  @param[i] 
  No input arguments

  @detail 
  - Implements the power manager main function. In the main function the 
    QMI messages in the SMR queue are processed.
  
  @return
  No return value
 
*/
/*=========================================================================*/
void sns_pm_main( void );

#endif /* SNS_PM_H */
