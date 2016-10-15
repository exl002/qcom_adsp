#ifndef __CLOCKBSP_H__
#define __CLOCKBSP_H__

/*=========================================================================

                    C L O C K   D E V I C E   D R I V E R
               
        B O A R D   S U P P O R T   P A C K A G E   D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains the BSP interface definitions for the
  MSM DAL Clock Device Driver.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

      Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.adsp/2.2/systemdrivers/clock/src/ClockBSP.h#4 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
11/30/10   vs      Initial release.

==========================================================================*/ 

/*=========================================================================
      Includes
==========================================================================*/


#include "HALclk.h"
#include "DDIChipInfo.h"
#include "HALavs.h"
#include "npa_remote_resource.h"


/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/

/*
 * Macro to force an enumeration to be a full 32 bits wide.
 */
#define CLOCK_BSP_ENUM_32BITS(name) CLOCK_##name##_32BITS = 0x7FFFFFFF

/*
 * Macros for compare support BSP within the range of HW version
 * BSP HW VERSION:
 *     Bits[ 0: 7] - Max HW version minor bits
 *     Bits[15: 8] - Max HW version major bits
 *     Bits[23:16] - Min HW version minor bits
 *     Bits[31:24] - Min HW version major bits
*/

#define BSP_SUPPORTED     0
#define BSP_NOT_SUPPORTED 0xFFFF0000
#define BSP_EFUSE         0xF0F00000
#define BSP_EFUSE_KEY     0x00000F00
#define BSP_EFUSE_VALUE   0x000000FF

#define BSP_GET_EFUSEKEY(eFuse) ((eFuse & BSP_EFUSE_KEY) >> 8)

#define BSP_HW_VER(nMinMajor, nMinMinor, nMaxMajor, nMaxMinor)               \
            ( ((nMinMajor<<24)& 0xFF000000) | ((nMinMinor<<16) & 0x00FF0000) | \
              ((nMaxMajor<<8) & 0x0000FF00) |  (nMaxMinor&0xFF) )

#define BSP_HW_VER_MIN( nBSPVersion ) ( ((nBSPVersion & 0xFF000000)>>8) | \
                                        ((nBSPVersion & 0x00FF0000)>>16) )
#define BSP_HW_VER_MAX( nBSPVersion ) ( ((nBSPVersion & 0x0000FF00)<<8) | \
                                         (nBSPVersion & 0x000000FF) )

/*
 * Macros for consolidating PLL configuration in a single mask.
 */
#define CLOCK_CONFIG_PLL_FSM_MODE_ENABLE        (1 << 0)
#define CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE      (1 << 1)
#define CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE     (1 << 2)
#define CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE    (1 << 3)


/*
 * Voltage request resource name.
*/
#define CLOCK_PMIC_RESOURCE_VDD_DIG          "/pmic/client/rail_cx"
#define CLOCK_PMIC_RESOURCE_VDD_DIG_CLIENT   "/clkregim/rail_cx"

/*
 * Define aliases for the NPA resource nodes here as they may be 
 * used in multiple places within the clock code. 
*/
#define CLOCK_NPA_RESOURCE_QDSS "/clk/qdss"
#define CLOCK_NPA_RESOURCE_PNOC "/clk/pnoc"
#define CLOCK_NPA_RESOURCE_SNOC "/clk/snoc"
#define CLOCK_NPA_RESOURCE_CNOC "/clk/cnoc"
#define CLOCK_NPA_RESOURCE_BIMC "/clk/bimc"
#define CLOCK_NPA_RESOURCE_OCMEM "/clk/ocmem"


/**
 * Unified flag bits
 *
 * NOTE: These flags can be used in any node that supports the
 * nFlags field.
 *
 *  LOG_STATE_CHANGE - Enable logging of enable/disable calls
 *    to a particular node.
 *  LOG_FREQ_CHANGE - Enable logging of frequency changes to
 *    a particular node.
 *  LOG_CONFIG_CHANGE - Enable logging of configuration changes
 *    to a particular node.
 *  LOG_RESET - Enable logging of resets to a particular node.
 *  DO_NOT_DISABLE - Ignore requests to disable a particular
 *    node.
 *  SUPPRESSIBLE - Indicates XO shutdown will not be held off
 *    while the clock is on.
 *  DOMAIN_HOLD_VREG_REQUEST - Hold the voltage request even
 *    when the clock is off. When set this means the voltage
 *    request will be based solely on the configured clock
 *    frequency and will not be released when the clock is
 *    disabled.
 */
#define CLOCK_FLAG_LOG_STATE_CHANGE           0x00000001
#define CLOCK_FLAG_LOG_FREQ_CHANGE            0x00000002
#define CLOCK_FLAG_LOG_CONFIG_CHANGE          0x00000004
#define CLOCK_FLAG_LOG_RESET                  0x00000008
// Reserved for future logging                0x00000010
// Reserved for future logging                0x00000020
// Reserved for future logging                0x00000040
// Reserved for future logging                0x00000080
#define CLOCK_FLAG_DO_NOT_DISABLE             0x00000100
#define CLOCK_FLAG_SUPPRESSIBLE               0x00000200
#define CLOCK_FLAG_DOMAIN_HOLD_VREG_REQUEST   0x00000400
#define CLOCK_FLAG_DOMAIN_DYNAMIC_CONFIG      0x00000800
#define CLOCK_FLAG_AUTOGATE                   0x00001000
#define CLOCK_FLAG_BIST_FAILURE_ANTICIPATED   0x00002000


/**
 * Global flag bits
 *
 * NOTE: These flags are to be used only in the nGlobalFlags
 * field of the driver context structure.
 *
 *  LOG_x_STATE_CHANGE - Enable logging of enable/disable
 *    calls to all clocks, sources, or power domains as
 *    specified by x.
 *  LOG_CLOCK_FREQ_CHANGE - Enable logging of frequency changes
 *    to all clocks.
 *  LOG_CLOCK_CONFIG_CHANGE - Enable logging of configuration
 *    changes to all clocks.
 *  LOG_CLOCK_RESET - Enable logging of resets to all clocks.
 *  DO_NOT_DISABLE_x - Ignore requests to disable all clocks,
 *    sources, or power domains as specified by x.
 */
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_STATE_CHANGE          0x00000001
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_FREQ_CHANGE           0x00000002
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_CONFIG_CHANGE         0x00000004
#define CLOCK_GLOBAL_FLAG_LOG_CLOCK_RESET                 0x00000008
#define CLOCK_GLOBAL_FLAG_LOG_SOURCE_STATE_CHANGE         0x00000010
#define CLOCK_GLOBAL_FLAG_LOG_POWER_DOMAIN_STATE_CHANGE   0x00000020
// Reserved for future logging                            0x00000040
// Reserved for future logging                            0x00000080
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_CLOCKS           0x00000100
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_SOURCES          0x00000200
#define CLOCK_GLOBAL_FLAG_DO_NOT_DISABLE_POWER_DOMAINS    0x00000400


/*
 * ClockVRegLevelType
 *
 * List of voltage levels that a client can request.  Note that the
 * enumeration values map to pmic NPA node request values.
 *
 *  OFF          - No vote
 *  RETENTION    - Retention voltage level.
 *  LOW_MINUS    - Lowered low voltage level.
 *  LOW          - Lowest voltage level.
 *  NOMINAL      - Nominal voltage level.
 *  NOMINAL PLUS - Elevated nominal voltage level.
 *  HIGH         - Elevated, or "turbo" voltage level.
 */
typedef enum
{
  CLOCK_VREG_LEVEL_OFF          = 0,
  CLOCK_VREG_LEVEL_RETENTION    = 1,
  CLOCK_VREG_LEVEL_LOW_MINUS    = 2,
  CLOCK_VREG_LEVEL_LOW          = 3,
  CLOCK_VREG_LEVEL_NOMINAL      = 4,
  CLOCK_VREG_LEVEL_NOMINAL_PLUS = 5,
  CLOCK_VREG_LEVEL_HIGH         = 6,
  CLOCK_VREG_NUM_LEVELS         = 7,
  CLOCK_BSP_ENUM_32BITS(VREG_LEVEL)
} ClockVRegLevelType;


/*
 * ClockHWVersionType;
 *
 * HW version information.
 *
 *  Min             - Minimum supported HW Major/Minor revision
 *  Max             - Maximum supported HW Major/Minor revision
 *                    '0xFF' indicates all versions.
 *  eChipInfoFamily - Chip this configuration is for. '0' indicates all chips.
 */
typedef struct
{
  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Min;

  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Max;

  DalChipInfoFamilyType eChipInfoFamily;
  const DalChipInfoIdType *aChipInfoId;

} ClockHWVersionType;


/*
 * ClockSourceConfigType
 *
 * Configuration information for a PLL.
 * 
 *  eSource     - Source ENUM value.
 *  HALConfig   - Configuration to pass to the HAL.
 *  nConfigMask - Mask of misc PLL configurations such as aux out, bist out,
 *                early out, etc.
 *  nFreqHz     - Frequency in Hz for this configuration.
 *  eVRegLevel  - Voltage level for this configuration.
 *  HWVersion   - The chipset that this source configuration is supported on.
 *                A '0' value means that it is applicable everywhere.
 */
typedef struct
{
  HAL_clk_SourceType      eSource;
  HAL_clk_PLLConfigType   HALConfig;
  uint32                  nConfigMask;
  uint32                  nFreqHz;
  ClockVRegLevelType      eVRegLevel;
  ClockHWVersionType      HWVersion;
} ClockSourceConfigType;


/*
 * ClockMuxConfigType
 *
 * Configuration information for a clock mux.
 *
 *  nFreqHz       - Frequency in Hz for this configuration.
 *  HALConfig     - Configuration to pass to the HAL.
 *  eVRegLevel    - Voltage level for this configuration.
 *  nChipVersion  - Version of the chip this configuration is for. '0'
 *                  inidicates all versions.
 *  eChipId       - Chip this configuration is for. '0' indicates all chips.
 *  pSourceConfig - Optional pointer to a PLL to configure for this.
 */
typedef struct
{
  uint32                      nFreqHz;
  HAL_clk_ClockMuxConfigType  HALConfig;
  ClockVRegLevelType          eVRegLevel;
  DalChipInfoVersionType      nChipVersion;
  DalChipInfoIdType           eChipId;
  ClockSourceConfigType      *pSourceConfig;
} ClockMuxConfigType;


/*
 * ClockLogType
 *
 * Default configuration for the clock log.
 *
 *  nLogSize         - Log size in bytes.  Must be a power of 2.
 *  nGlobalLogFlags  - Global log flags.
 */
typedef struct
{
  uint32      nLogSize;
  uint8       nGlobalLogFlags;
} ClockLogType;


/*
 * ClockXOVoteType
 *
 * Structure to hold clocks which need to explicitly hold a source.
 */
typedef struct
{
  const char *szName;
} ClockXOVoteType;


/*
 * ClockPropertyValueType
 *
 * Generic clock property data.  Actual data type will depend on the property
 * and will not even necessarily be a pointer (i.e. could be straight uint32).
 */
typedef const void *ClockPropertyValueType;


/*
 * ClockPropertyType
 *
 * Generic clock property structure, the BSP data in non-DAL environments
 * will consist of an array of these.
 *
 *  szName - Name of the property
 *  Value  - Property value
 */
typedef struct
{
  const char            *szName;
  ClockPropertyValueType Value;
} ClockPropertyType;



/*
 * ClockAVSCoreCfgType
 *
 * BSP Data structure for describing delay synthesizer to CPU frequency 
 * requirements.
 *
 * nDelaySynth    - The synthesizer value to program to the coprocessor.
 * nMinVdd        - The lowest voltage that AVS can set automatically at this frequency.
 * nPerfVdd       - The performance level associated voltage in uV.
 */
typedef struct
{
  uint32 nDelaySynth;
  uint32 nMinVdd;
  uint32 nPerfVdd;
}ClockAVSCoreFreqCfgType;


/*
 * ClockAVSCoreConfigType
 *
 * BSP Data structure for initializing an AVS core.
 *
 * nPhysicalBase    - The physical base address for this AVS core.
 * eCore            - The Core ID for this instance.
 * ePMIC            - The PMIC type for this core.
 * eStepSize        - The step size of voltage adjustment requests.
 * nVddRequestDelay - The delay between HW voltage requests.
 * nClockDiv        - The clock divider.
 */
typedef struct
{
  uint32               nPhysicalBase;
  HAL_avs_CoreType     eCore;
  HAL_avs_PmicType     ePMIC;
  HAL_avs_StepSizeType eStepSize;
  uint32               nVddRequestDelay;
  uint32               nClockDiv;
}ClockAVSCoreConfigType;


/*
 * ClockFlagNodeType
 *
 * List of nodes that init flags can be applied to.
 *
 *  CLOCK        - Flags are to be applied to the specified clock.
 *  CLOCK_DOMAIN - Flags are to be applied to the specified domain.
 *  SOURCE       - Flags are to be applied to the specified source.
 *  POWER_DOMAIN - Flags are to be applied to the specified power domain.
 *
 */
typedef enum
{
  CLOCK_FLAG_NODE_TYPE_NONE          = 0,
  CLOCK_FLAG_NODE_TYPE_CLOCK         = 1,
  CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN  = 2,
  CLOCK_FLAG_NODE_TYPE_SOURCE        = 3,
  CLOCK_FLAG_NODE_TYPE_POWER_DOMAIN  = 4,
  CLOCK_FLAG_NODE_TYPE_MAX_SUPPORTED = 5
} ClockFlagNodeType;


/*
 * ClockFlagInitType
 *
 * BSP Data structure for initializing the flags of an individual clock,
 * clock domain, power domain, or source.
 *
 *  eNodeType - The node to which the flags should be applied.
 *  id        - The ID to lookup this node info.
 *              (This type is dependent upon the eNodeType).
 *  nFlag     - Init value for the clock's flags.
 */
typedef struct
{
  ClockFlagNodeType  eNodeType;
  void              *pID;
  uint32             nFlag;
} ClockFlagInitType;


/*
 * ClockLDOInitType
 *
 * BSP data for LDO programming
 *
 * nLDOCFG0          - The value for the first configuration register.
 * nLDOCFG1          - The value for the second configuration register.
 * nLDOCFG2          - The value for the third configuration register.
 * nRetentionVoltage - The retention voltage in mV.
 * nVddRequestDelay  - The initial operating voltage in mV.
 */
typedef struct
{
  uint32               nLDOCFG0;
  uint32               nLDOCFG1;
  uint32               nLDOCFG2;
  uint32               nRetentionVoltage;
  uint32               nOperatingVoltage;
}ClockLDODataType;

/*
 * ClockNPAResourcePubType
 *
 * BSP data for publishing NPA nodes to user domains
 *
 * nDomain           - The domain that requires the resource published.
 * ppszResourceList  - An array of string client resources to publish.
 * nTotalResources   - The number of resources to publish.
 */
typedef struct
{
   uint32 nDomain;
   const char** ppszResourceList;
   uint32 nTotalResources;
}ClockNPAResourcePubType;

#endif  /* __CLOCKBSP_H__ */ 

