#ifndef QURT_PMU_H
#define QURT_PMU_H
/**
  @file qurt_pmu.h 
  @brief  Prototypes of pipe interface API.  
	 A pipe or message queue will block if too full (send) or empty (receive).
	 Unless a nonblocking option is used, all datagrams are 64 bits.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2010 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.adsp/2.2/kernel/qurt/libs/qurt/include/public/qurt_pmu.h#8 $ 
$DateTime: 2014/04/24 05:20:21 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
12/16/12   cm      (Tech Pubs) Edited/added Doxygen comments and markup.
=============================================================================*/

/*=============================================================================
												FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_pmu_set
  Sets the value of the specified PMU register.

  @note1hang Setting PMUEVTCFG automatically clears the PMU registers PMUCNT0
             through PMUCNT3.
 
  @param[in] reg_id   PMU register. Values: \n
              QURT_PMUCNT0 -- PMUCNT0 register \n
              QURT_PMUCNT1 -- PMUCNT1 register \n
              QURT_PMUCNT2 -- PMUCNT2 register \n
              QURT_PMUCNT3 -- PMUCNT3 register \n
              QURT_PMUCNT4 -- PMUCFG register \n
              QURT_PMUCNT5 -- PMUEVTCFG register
 
  @param[in] reg_value  Register value.
 
  @return
  None.
   
  @dependencies
  None.
 */
void qurt_pmu_set (int reg_id, unsigned int reg_value);
 
/**@ingroup func_qurt_pmu_get
  Gets the PMU register.\n
 Returns the current value of the specified PMU register.

  @param[in] red_id   PMU register. Values: \n
              QURT_PMUCNT0 -- PMUCNT0 register \n
              QURT_PMUCNT1 -- PMUCNT1 register \n
              QURT_PMUCNT2 -- PMUCNT2 register \n
              QURT_PMUCNT3 -- PMUCNT3 register \n
              QURT_PMUCNT4 -- PMUCFG register \n
              QURT_PMUCNT5 -- PMUEVTCFG register 
 
  @return
   Integer -- Current value of the specified PMU register.

  @dependencies
  None.
 */
unsigned int  qurt_pmu_get (int red_id);
 
/**@ingroup func_qurt_pmu_enable
  Enables or disables the Hexagon processor performance monitor unit (PMU).
  Profiling is disabled by default. \n
  @note1hang Enabling profiling does not automatically reset the count registers -- this must
            be done explicitly before starting event counting.
 
  @param[in] enable Performance monitor. Values: \n
                    0 -- Disable performance monitor \n
                    1 -- Enable performance monitor
 
  @return 
  None.

  @dependencies
  None.
 */
void qurt_pmu_enable (int enable);

#endif /* QURT_PMU_H */
