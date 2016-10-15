#ifndef HALBIMCI_H
#define HALBIMCI_H
/**

@file   HALbimci.h

@brief  Internal header file for HAL BIMC.
        
===========================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/buses/icb/src/common/HALbimci.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/03/11   sds     Created 

===========================================================================
                  Copyright (c) 2011 Qualcomm Technologies Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALbimc.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define BIMC_VERIFY_PTR(ptr)             if(NULL == (ptr)) return
#define BIMC_VERIFY_MASTER_IDX(info,idx) if((idx) >= (info)->devParams.uNumMasters) return
#define BIMC_VERIFY_SLAVE_IDX(info,idx)  if((idx) >= (info)->devParams.uNumSlaves) return

#define CEILING_DIV(dividend,divisor)    (((dividend)+(divisor)-1)/(divisor))

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/** Slave block types */
typedef enum
{
  BIMC_SLAVE_BLOCK_TYPE_RESERVED  = 0,
  BIMC_SLAVE_BLOCK_TYPE_SLAVE_WAY = 1,
  BIMC_SLAVE_BLOCK_TYPE_XPU       = 2,
  BIMC_SLAVE_BLOCK_TYPE_ARBITER   = 3,
  BIMC_SLAVE_BLOCK_TYPE_SCMO      = 4,
} HAL_bimc_SlaveBlockType;

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** @name Mandatory Interface */
/** @{ */

/** 
@copydoc HAL_bimc_Init
*/
void bimc_Init( HAL_bimc_InfoType *pInfo );

/**
@copydoc HAL_bimc_Reset
*/
void bimc_Reset( HAL_bimc_InfoType *pInfo );

/**
@copydoc HAL_bimc_Save
*/
void bimc_Save( HAL_bimc_InfoType *pInfo );

/**
@copydoc HAL_bimc_Restore
*/
void bimc_Restore ( HAL_bimc_InfoType *pInfo );
/** @} */

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/** @name Configuration Interface */
/** @{ */

/**
@copydoc HAL_bimc_SetSlaveSegment
*/
void bimc_SetSlaveSegment( HAL_bimc_InfoType *        pInfo,
                           uint32_t                   uSlaveIdx,
                           uint32_t                   uSegmentIdx,
                           HAL_bimc_SlaveSegmentType *pSegment );

/**
@copydoc HAL_bimc_SetSlaveClockGating
*/
void bimc_SetSlaveClockGating( HAL_bimc_InfoType *       pInfo,
                               uint32_t                  uSlaveIdx,
                               HAL_bimc_ClockGatingType *pGating );

/**
@copydoc HAL_bimc_SetMasterClockGating
*/
void bimc_SetMasterClockGating( HAL_bimc_InfoType *       pInfo,
                                uint32_t                  uMasterIdx,
                                HAL_bimc_ClockGatingType *pGating );

/**
@copydoc HAL_bimc_ArbitrationEnable
*/
void bimc_ArbitrationEnable( HAL_bimc_InfoType *pInfo,
                             uint32_t           uSlaveIdx,
                             bool               bEnable );

/**
@copydoc HAL_bimc_SetQosMode
*/
void bimc_SetQosMode( HAL_bimc_InfoType *  pInfo,
                      uint32_t             uMasterIdx,
                      HAL_bimc_QosModeType eMode );

/**
@copydoc HAL_bimc_SetQosPriority
*/
void bimc_SetQosPriority( HAL_bimc_InfoType *       pInfo,
                          uint32_t                  uMasterIdx,
                          HAL_bimc_QosModeType      eMode,
                          HAL_bimc_QosPriorityType *pPriority );

/**
@copydoc HAL_bimc_SetQosBandwidth
*/
void bimc_SetQosBandwidth( HAL_bimc_InfoType *        pInfo,
                           uint32_t                   uMasterIdx,
                           HAL_bimc_QosBandwidthType *pBandwidth );

/** @} */

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/** @name Data Interface */
/** @{ */

/**
@copydoc HAL_bimc_GetDeviceParameters
*/
void bimc_GetDeviceParameters( HAL_bimc_InfoType *        pInfo,
                               HAL_bimc_DeviceParamsType *pDevParams );

/**
@copydoc HAL_bimc_GetMasterParameters
*/
void bimc_GetMasterParameters( HAL_bimc_InfoType *            pInfo,
                               uint32_t                       uMasterIdx, 
                               HAL_bimc_MasterPortParamsType *pPortParams );

/**
@copydoc HAL_bimc_GetSlaveParameters
*/
void bimc_GetSlaveParameters( HAL_bimc_InfoType *           pInfo,
                              uint32_t                      uSlaveIdx,
                              HAL_bimc_SlavePortParamsType *pPortParams );

/** @} */
#endif /* #ifndef HALBIMCI_H */
