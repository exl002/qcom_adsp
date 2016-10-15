#ifndef ADSPPM_THERM_MITIGATION_DEVICE_SERVICE_H
#define ADSPPM_THERM_MITIGATION_DEVICE_SERVICE_H

/*============================================================================
  FILE:         therm_npa_mitigate.h
  
  OVERVIEW:     Thermal NPA mitigation resources header file.
 
            
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.

============================================================================*/
/*=======================================================================
$Header: //components/rel/core.adsp/2.2/power/adsppm/thermal/adsppm_therm_mitigation_device_service.h#2 $
$DateTime: 2013/07/30 13:08:31 $
$Author: coresvc $
========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/*
===============================================================================

                             TYPE DEFINITION(S)

===============================================================================
*/

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
 * @brief Initialization function for the Thermal QMI service
 *
 * This function implements the initialization code for the 
 * Thermal QMI service 
 */
void adsppm_therm_qmi_init(void);


#endif /* ADSPPM_THERM_MITIGATION_DEVICE_SERVICE_H */
