#ifndef __CLOCKCPUBSP_H__
#define __CLOCKCPUBSP_H__

/*=========================================================================

                    C L O C K   D E V I C E   D R I V E R
               
              C P U   B O A R D   S U P P O R T   P A C K A G E

GENERAL DESCRIPTION
  This file contains the BSP interface definitions for the
  MSM DAL Clock Device Driver CPU data structures.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.adsp/2.2/systemdrivers/clock/hw/mdm9x35/inc/ClockLPASSCPU.h#1 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
11/07/12   dcf     Initial release.

==========================================================================*/ 

/*=========================================================================
      Includes
==========================================================================*/

/*=========================================================================
      Constants, Macros & Typedefs
==========================================================================*/

/*
 * ClockCPUConfigType
 *
 * Configuration parameters for a performance level of a CPU.
 * 
 * CoreConfig     - CPU core configuration 
 * Mux            - General mux configuration
 * nVDDCPU        - VDD CPU value in millivolts
 * nLDOQ6UV       - Q6SS LDO voltage requirement in uV
 * nLDOHeadRoomUV - The required headroom to operate the LDO.
 * nStrapACCVal   - ACC value
 * nChipset       - The chipset the configuration is supported on.  Leave value at '0'
 *                  for frequencies supported on all chipsets.
 */
typedef struct
{
  HAL_clk_ClockConfigType  CoreConfig;
  ClockMuxConfigType       Mux;
  uint32                   nVDDCPU;
  uint32                   nLDOQ6UV;
  uint32                   nLDOHeadRoomUV;
  uint32                   nStrapACCVal;
  uint32                   nChipset;
} ClockCPUConfigType;


/*
 * Actual format for the data stored in the BSP for a CPU. 
 *  
 * nMinPerfLevel  - The minimum supported performance level for the CPU on this chipset. 
 * nMaxPerfLevel  - The maximum supported performance level for the CPU on this chipset. 
 * nInitPerfLevel - The performance level that should be programmed during initialization by 
 *                  the clock driver. 
 */
typedef struct
{
  uint32              nMinPerfLevel;
  uint32              nMaxPerfLevel;
  uint32              nInitPerfLevel;
  uint32              nTotalConfigs;
  uint32              *panPerfLevel;
} ClockCPUPerfConfigType;


/*
 * LPASS PLL to actual PLL mapping 
 *  
 * nSource     - The supported PLL for this chipset. 
 * bInitialize - Specifies if the driver should initialize this during clock initialization. 
 */
typedef struct
{
  HAL_clk_SourceType nSource;
  boolean            bInitialize;
} ClockImagePLLConfigType;


/*
 * LPASS BSP data 
 *  
 * pCPUConfig             - Contains CPU-specific configurtion information. 
 * pCPUPerConfig          - Contains the performance level information for the CPU. 
 * bGlobalLDOEnable       - Flag specifying whether the embedded LDO should be enabled.
 * bIsFusionMDM           - Flag specifying if the current target is an MDM that supports 
 *                          Fusion. 
 * bLDOSupporteByEFuse    - Flag that will be set if the EFUSE determins eLDO can be used. 
 * nTotalSupportedConfigs - The total number of configurations in pCPUConfig that 
 *                          are supported on the current platform.
 * pSPMCTLSources         - Pointer to an array of sources that support SPMCTL override.
 */
typedef struct
{
  ClockCPUConfigType       *pCPUConfig;
  ClockCPUPerfConfigType   *pCPUPerfConfig;
  boolean                   bGlobalLDOEnable;
  boolean                   bIsFusionMDM;
  boolean                   bLDOSupporteByEFuse;
  uint32                    nTotalSupportedConfigs;
} ClockImageConfigType;


/*
 * NPA Remote Node support flags type to determine 
 * chipset specific initialization of NPA Remote Nodes. 
 *  
 * bOCMEM - Specifies if OCMEM is supported 
 *  
*/
typedef struct
{
  boolean bOCMEM;

}ClockNPARemoteNodeSupportType;


#endif  /* __CLOCKCPUBSP_H__ */ 

