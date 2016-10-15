#ifndef HALNOC_H
#define HALNOC_H
/**

@file   HALnoc.h

@brief  This module provides the Application Programing Interface (API) 
        to the NOC HAL layer.
*/
/*
===========================================================================

FILE:         HALnoc.h

DESCRIPTION:  Header file for the HAL NOC interface.

===========================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/buses/icb/src/common/HALnoc.h#2 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2013/04/03   sds     Add fields to info structure to support save/restore
2011/08/15   sds     Created 

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
/* QOS mode enumeration */
typedef enum
{
  NOC_QOS_MODE_FIXED     = 0,
  NOC_QOS_MODE_LIMITER   = 1,
  NOC_QOS_MODE_BYPASS    = 2,
  NOC_QOS_MODE_REGULATOR = 3,
  NOC_QOS_MODE_COUNT,
} HAL_noc_QosModeType;

/* QOS allowed modes enumeration mask */
typedef enum
{
  NOC_QOS_ALLOWED_MODE_FIXED     = (1 << NOC_QOS_MODE_FIXED),
  NOC_QOS_ALLOWED_MODE_LIMITER   = (1 << NOC_QOS_MODE_LIMITER),
  NOC_QOS_ALLOWED_MODE_BYPASS    = (1 << NOC_QOS_MODE_BYPASS),
  NOC_QOS_ALLOWED_MODE_REGULATOR = (1 << NOC_QOS_MODE_REGULATOR),
} HAL_noc_QosMaskType;

#define ALL_NOC_QOS_MODES_ALLOWED (NOC_QOS_ALLOWED_MODE_FIXED | \
                                   NOC_QOS_ALLOWED_MODE_LIMITER | \
                                   NOC_QOS_ALLOWED_MODE_BYPASS | \
                                   NOC_QOS_ALLOWED_MODE_REGULATOR)

typedef struct
{
  uint32_t uAllowedModes;
} HAL_noc_QosMasterInfoType;

/** NOC device info structure */
typedef struct
{
  uint32_t                    uBaseAddr;
  uint32_t                    uNumMasters;
  uint32_t                    uNumQosMasters;
  uint32_t                    uNumSlaves;
  uint32_t                    uQosFreq;   /**< QoS clock, in KHz */
  HAL_noc_QosMasterInfoType  *aMasters;
  bool                        bSaveValid; /**< Indicates whether or not the save buffer
                                               contains valid data. */
  uint32_t                   *pSaveBuf;   /**< Save/Restore buffer pointer. */
  uint32_t                    uNumAddrs;  /**< Number of additional registers to save. */
  uint32_t                  **paAddrList; /**< Additional registers to save list. */
} HAL_noc_InfoType;

/* QOS priority structure */
typedef struct
{
  uint32_t p1; /* High/Read priority, depending on the mode */
  uint32_t p0; /* Low/Write priority, depending on the mode */
} HAL_noc_QosPriorityType;

/* Qos bandwidth structure */
typedef struct
{
  uint64_t uBandwidth;  /**< Bandwidth requirement in bytes per second */
  uint32_t uWindowSize; /**< Integration window size, in nanoseconds */
} HAL_noc_QosBandwidthType;

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** @name Mandatory Interface */
/** @{ */

/** 
@brief Initialization function.

@param[inout] pInfo - A pointer to the NOC info structure

@remarks The base address and non-hardware derived information must
         be prepopulated in the info structure.
*/
void HAL_noc_Init( HAL_noc_InfoType * pInfo );

/**
@brief This function resets the selected NOC hardware to a default state.

@param[in] pInfo - A pointer to the NOC info structure

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_Reset( HAL_noc_InfoType * pInfo ) ;

/**
@brief Save the current NOC state.

@param[in] pInfo - A pointer to the NOC info structure

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
@sa HAL_noc_Restore()
*/
void HAL_noc_Save( HAL_noc_InfoType * pInfo );

/**
@brief Restore an NOC to a previously saved state.

@param[in] pInfo - A pointer to the NOC info structure

@pre HAL_noc_Init() and HAL_noc_Save() must have been called.
@pre HAL_noc_Save() must be called for each restore required.

@sa HAL_noc_Init()
@sa HAL_noc_Save()
*/
void HAL_noc_Restore( HAL_noc_InfoType * pInfo );
/** @} */

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/** @name Configuration Interface */
/** @{ */

/**
@brief Set the Qos mode for the given QoS port number (masters).

@param[in] pInfo -       A pointer to the NOC info structure
@param[in] uMasterPort - The port number of the master Qos to modify
@param[in] eMode -       The mode to change to

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_SetQosMode( HAL_noc_InfoType *  pInfo,
                         uint32_t            uMasterPort,
                         HAL_noc_QosModeType eMode );

/**
@brief Set the Qos priority levels for the given QoS port number.

@param[in] pInfo -       A pointer to the NOC info structure
@param[in] uMasterPort - The port number of the master Qos to modify
@param[in] eMode -       The mode to change priority information for
@param[in] pPriority -   A pointer to the priorities to set

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_SetQosPriority( HAL_noc_InfoType *       pInfo,
                             uint32_t                 uMasterPort,
                             HAL_noc_QosModeType      eMode,
                             HAL_noc_QosPriorityType *pPriority );

/**
@brief Restore an NOC to a previously saved state.

@param[in] pInfo -       A pointer to the NOC info structure
@param[in] uMasterPort - The port number of the master Qos to modify
@param[in] pBandwidth -  A pointer to the bandwidth values to set

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_SetQosBandwidth( HAL_noc_InfoType *        pInfo,
                              uint32_t                  uMasterPort,
                              HAL_noc_QosBandwidthType *pBandwidth );

/** @} */

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/** @name Data Interface */
/** @{ */

/**
@brief Get the Qos mode for the given QoS port number (masters).

@param[in] pInfo -       A pointer to the NOC info structure
@param[in] uMasterPort - The port number of the master Qos to view
@param[in] pMode -       The location to store the mode information

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_GetQosMode( HAL_noc_InfoType *   pInfo,
                         uint32_t             uMasterPort,
                         HAL_noc_QosModeType *pMode );

/**
@brief Get the Qos priority levels for the given QoS port number.

@param[in] pInfo -       A pointer to the NOC info structure
@param[in] uMasterPort - The port number of the master Qos to view
@param[in] pPriority -   A location to store the priorities

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_GetQosPriority( HAL_noc_InfoType *       pInfo,
                             uint32_t                 uMasterPort,
                             HAL_noc_QosPriorityType *pPriority );

/**
@brief Restore an NOC to a previously saved state.

@param[in] pInfo -       A pointer to the NOC info structure
@param[in] uMasterPort - The port number of the master Qos to view
@param[in] pBandwidth -  A location to store the bandwidth

@pre HAL_noc_Init() must have been called.

@sa HAL_noc_Init()
*/
void HAL_noc_GetQosBandwidth( HAL_noc_InfoType *        pInfo,
                              uint32_t                  uMasterPort,
                              HAL_noc_QosBandwidthType *pBandwidth );
/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef HALNOC_H */

