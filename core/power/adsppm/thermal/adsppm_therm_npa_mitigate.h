#ifndef ADSPPM_THERM_NPA_MITIGATE_H
#define ADSPPM_THERM_NPA_MITIGATE_H

/*============================================================================
  FILE:         therm_npa_mitigate.h
  
  OVERVIEW:     Thermal NPA mitigation resources header file.
 
              
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.

============================================================================*/
/*=======================================================================
$Header: //components/rel/core.adsp/2.2/power/adsppm/thermal/adsppm_therm_npa_mitigate.h#3 $
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

#define ELDO_NODE_NAME "/vdd/thermal"
#define ELDO_ADSPPM_CLIENT_NAME "adsppmeLDO"

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
  @brief adsppm_therm_npa_mitigate_init

  Thermal mitigate init function.
*/
void adsppm_therm_npa_mitigate_init( void );


#ifdef __cplusplus
}
#endif

#endif /* THERM_NPA_MITIGATE_H */
