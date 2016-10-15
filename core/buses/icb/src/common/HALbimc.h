#ifndef HALBIMC_H
#define HALBIMC_H
/**

@file   HALbimc.h

@brief  This module provides the Application Programing Interface (API) 
        to the BIMC HAL layer.
*/
/*
===========================================================================

FILE:         HALbimc.h

DESCRIPTION:  Header file for the HAL BIMC interface.

===========================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/buses/icb/src/common/HALbimc.h#2 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2013/04/22   sds     Changed bandwidth type defintion comments.
2013/04/08   sds     Change defaults type from union to struct due to c89 limitations
2011/08/02   sds     Created 

===========================================================================
                  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/** BIMC device parameter structure */
typedef struct
{
  uint32_t uBusId;
  uint32_t uAddrWidth; /**< Width in bits. */
  uint32_t uDataWidth; /**< Width in bits. */
  uint32_t uNumMasters;
  uint32_t uNumSlaves;
} HAL_bimc_DeviceParamsType;

/** BMIC device info structure */
typedef struct
{
  uint32_t                  uBaseAddr;
  uint32_t                  uQosFreq; /**< QoS clock, in KHz */
  HAL_bimc_DeviceParamsType devParams;
} HAL_bimc_InfoType;

/** BMIC slave port parameter structure */
typedef struct
{
  uint32_t         uConnMask;  /**< Connectivity bit mask */
  uint32_t         uDataWidth; /**< Width in bits. */
} HAL_bimc_MasterPortParamsType;

/** Arbitration modes */
typedef enum
{
  BIMC_ARB_MODE_RR          = 0,
  BIMC_ARB_MODE_PRIORITY_RR = 1,
  BIMC_ARB_MODE_TIERED_RR   = 2,
  BIMC_ARB_MODE_COUNT,
  BIMC_ARB_MODE_WIDTH       = 0x7FFFFFFF
} HAL_bimc_ArbModeType;

/** BMIC slave port parameter structure */
typedef struct
{
  uint32_t             uConnMask;  /**< Connectivity bit mask */
  uint32_t             uDataWidth; /**< Width in bits. */
  HAL_bimc_ArbModeType eArbMode;
} HAL_bimc_SlavePortParamsType;

/** Slave segment configuration */
typedef enum
{
  BIMC_SEGMENT_TYPE_ADDITIVE    = 0,
  BIMC_SEGMENT_TYPE_SUBTRACTIVE = 1,
  BIMC_SEGMENT_TYPE_COUNT,
  BIMC_SEGMENT_TYPE_WIDTH       = 0x7FFFFFFF
} HAL_bimc_SegmentType;

typedef enum
{
  BIMC_INTERLEAVE_NONE  = 0,
  BIMC_INTERLEAVE_ODD,
  BIMC_INTERLEAVE_EVEN,
  BIMC_INTERLEAVE_COUNT,
  BIMC_INTERLEAVE_WIDTH = 0x7FFFFFFF
} HAL_bimc_InterleaveType;

typedef struct
{
  bool                    bEnable;
  uint64_t                uStartAddr;
  uint64_t                uSegmentSize;
  HAL_bimc_SegmentType    type;
  HAL_bimc_InterleaveType interleave;
} HAL_bimc_SlaveSegmentType;

/** BIMC Qos types */
typedef enum
{
  BIMC_QOS_MODE_FIXED     = 0,
  BIMC_QOS_MODE_LIMITER   = 1,
  BIMC_QOS_MODE_BYPASS    = 2,
  BIMC_QOS_MODE_REGULATOR = 3,
  BIMC_QOS_MODE_COUNT,
  BIMC_QOS_MODE_WIDTH     = 0x7FFFFFFF,
} HAL_bimc_QosModeType;

typedef struct
{
  bool     bLimitCommands;
  uint32_t uAReqPriority;
  uint32_t uPriorityLvl;
} HAL_bimc_QosHealthType;

typedef struct
{
  /** Fixed priority mode */
  struct
  {
    uint32_t               uPriorityLvl;
    uint32_t               uAReqPriorityRead;
    uint32_t               uAReqPriorityWrite;
  } fixed;

  /** Regulator/Limiter modes */
  struct
  {
    HAL_bimc_QosHealthType health3;
    HAL_bimc_QosHealthType health2;
    HAL_bimc_QosHealthType health1;
    HAL_bimc_QosHealthType health0;
  } regulator;
} HAL_bimc_QosPriorityType;

typedef struct
{
  uint64_t uBandwidth;       /**< Reference bandwidth requirement in bytes per second */
  uint32_t uWindowSize;      /**< Integration window size, in nanoseconds */

  /* Regulator/Limiter mode items. Ignored if the mode being set isn't R/L */
  uint64_t uThresholdHigh;   /**< High threshold, in bytes per second. */
  uint64_t uThresholdMedium; /**< Medium threshold, in bytes per second. */
  uint64_t uThresholdLow;    /**< Low threshold, in bytes per second. */
} HAL_bimc_QosBandwidthType;

/** BIMC clock gating type.
 *  Note: Not all fields apply to all ports.
*/
typedef struct
{
  bool bCoreClockGateEn; /**< Gating of the registers on the clock of the core behind the port. */
  bool bArbClockGateEn;  /**< Gating of the arbiter on the port. */ 
  bool bPortClockGateEn; /**< Gating of the registers on the BIMC core clock. */
} HAL_bimc_ClockGatingType;

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** @name Mandatory Interface */
/** @{ */

/** 
@brief Initialization function.

@param[in] pInfo - A pointer to the BIMC info structure
*/
void HAL_bimc_Init( HAL_bimc_InfoType *pInfo );

/**
@brief This function resets the selected BIMC hardware to a default state.

@param[in] pInfo - A pointer to the BIMC info structure

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_Init()
*/
void HAL_bimc_Reset( HAL_bimc_InfoType *pInfo ) ;

/**
@brief Save the current BIMC state.

@param[in] pInfo - A pointer to the BIMC info structure

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_Init()
@sa HAL_bimc_Restore()
*/
void HAL_bimc_Save( HAL_bimc_InfoType *pInfo );

/**
@brief Restore an BIMC to a previously saved state.

@param[in] pInfo - A pointer to the BIMC info structure

@pre HAL_bimc_Init() and HAL_bimc_Save() must have been called.

@sa HAL_bimc_Init()
@sa HAL_bimc_Save()
*/
void HAL_bimc_Restore ( HAL_bimc_InfoType *pInfo );
/** @} */

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/** @name Configuration Interface */
/** @{ */

/**
@brief Configure the slave segment address range for a given slave and segment.

Configure the address range for a given slave segment. There are three
requirements on the segment configuration: The minimum size of the region
is 1 MB (2^20), the size of the region must be a power of 2, and the region
must start on a multiple of its size.

@param[in] pInfo -       A pointer to the BIMC info structure
@param[in] uSlaveIdx -   The slave index
@param[in] uSegmentIdx - The segment index
@param[in] pSegment -    A pointer to the segment configuration

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_Init()
*/
void HAL_bimc_SetSlaveSegment( HAL_bimc_InfoType *        pInfo,
                               uint32_t                   uSlaveIdx,
                               uint32_t                   uSegmentIdx,
                               HAL_bimc_SlaveSegmentType *pSegment );

/**
@brief Configure the clock gating for the indicated slave.

@param[in] pInfo -     A pointer to the BIMC info structure
@param[in] uSlaveIdx - The slave index
@param[in] pGating -   A pointer to the clock gating configuration

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_Init()
*/
void HAL_bimc_SetSlaveClockGating( HAL_bimc_InfoType *       pInfo,
                                   uint32_t                  uSlaveIdx,
                                   HAL_bimc_ClockGatingType *pGating );

/**
@brief Configure the clock gating for the indicated slave.

@param[in] pInfo -      A pointer to the BIMC info structure
@param[in] uMasterIdx - The slave index
@param[in] pGating -    A pointer to the clock gating configuration

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_Init()
*/
void HAL_bimc_SetMasterClockGating( HAL_bimc_InfoType *       pInfo,
                                    uint32_t                  uMasterIdx,
                                    HAL_bimc_ClockGatingType *pGating );
/**
@brief Enable or disable arbitration on a given slave port.

@param[in] pInfo -     A pointer to the BIMC info structure
@param[in] uSlaveIdx - The slave index
@param[in] bEnable -   Enable/disable

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_Init()
*/
void HAL_bimc_ArbitrationEnable( HAL_bimc_InfoType *pInfo,
                                 uint32_t           uSlaveIdx,
                                 bool               bEnable );

/**
@brief Set the Qos mode for a particular master port.

@param[in] pInfo -      A pointer to the BIMC info structure
@param[in] uMasterIdx - The master index
@param[in] eMode -      The Qos mode to use

@pre HAL_bimc_Init() must have been called.
@pre The master QoS should be disabled before calling this function.

@sa HAL_bimc_Init()
*/
void HAL_bimc_SetQosMode( HAL_bimc_InfoType *  pInfo,
                          uint32_t             uMasterIdx,
                          HAL_bimc_QosModeType eMode );

/**
@brief Set the Qos priority for a given mode

@param[in] pInfo -      A pointer to the BIMC info structure
@param[in] uMasterIdx - The master index
@param[in] eMode -      The mode to change priority information for
@param[in] pPriority -  The priority information

@pre HAL_bimc_Init() must have been called.
@pre The master QoS should be disabled before calling this function.

@sa HAL_bimc_Init()
*/
void HAL_bimc_SetQosPriority( HAL_bimc_InfoType *       pInfo,
                              uint32_t                  uMasterIdx,
                              HAL_bimc_QosModeType      eMode,
                              HAL_bimc_QosPriorityType *pPriority );

/**
@brief Set the bandwidth and arbitration window parameters for the given master port.

@param[in] pInfo -      A pointer to the BIMC info structure
@param[in] uMasterIdx - The master index
@param[in] pBandwidth - The bandwidth parameters

@pre HAL_bimc_Init() must have been called.
@pre The master QoS should be disabled before calling this function.

@sa HAL_bimc_Init()
*/
void HAL_bimc_SetQosBandwidth( HAL_bimc_InfoType *        pInfo,
                               uint32_t                   uMasterIdx,
                               HAL_bimc_QosBandwidthType *pBandwidth );

/** @} */

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/** @name Data Interface */
/** @{ */

/**
@brief Get BIMC hardware device parameters.

   This function reads the device parameters from the hardware.

@param[in]  pInfo - A pointer to the BIMC info structure
@param[out] pDevParams - A pointer to the structure to store the device parameters

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_Init()
*/
void HAL_bimc_GetDeviceParameters( HAL_bimc_InfoType *        pInfo,
                                   HAL_bimc_DeviceParamsType *pDevParams );

/**
@brief Get BIMC master port device parameters.

   This function reads the master port parameters from the hardware.

@param[in]  pInfo -       A pointer to the BIMC info structure
@param[in]  uMasterIdx -  The index of the master port
@param[out] pPortParams - A pointer to the structure to store the device parameters

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_Init()
*/
void HAL_bimc_GetMasterParameters( HAL_bimc_InfoType *            pInfo,
                                   uint32_t                       uMasterIdx, 
                                   HAL_bimc_MasterPortParamsType *pPortParams );

/**
@brief Get BIMC slaveway device parameters.

   This function reads the slaveway parameters from the hardware.

@param[in]  pInfo -       A pointer to the BIMC info structure
@param[in]  uSlaveIdx -   The index of the slave port
@param[out] pPortParams - A pointer to the structure to store the device parameters

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_Init()
*/
void HAL_bimc_GetSlaveParameters( HAL_bimc_InfoType *           pInfo,
                                  uint32_t                      uSlaveIdx,
                                  HAL_bimc_SlavePortParamsType *pPortParams );

/**
@brief Get BIMC slaveway arbitration status.

   This function reads the slaveway arbitration status.

@param[in] pInfo -       A pointer to the BIMC info structure
@param[in] uSlaveIdx -   The index of the slave port

@pre HAL_bimc_Init() must have been called.

@sa HAL_bimc_GetArbitrationEnable()
@sa HAL_bimc_Init()
*/
bool HAL_bimc_GetArbitrationEnable( HAL_bimc_InfoType *pInfo,
                                    uint32_t           uSlaveIdx );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef HALBIMC_H */

