/**

@file   HALbimc.c

@brief  Implementation file for HAL BIMC.
        
===========================================================================

                             Edit History

$Header: //components/rel/core.adsp/2.2/buses/icb/src/common/HALbimc.c#1 $

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
#include "HALbimci.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** 
@copydoc HAL_bimc_Init
*/
void HAL_bimc_Init( HAL_bimc_InfoType *pInfo )
{
  BIMC_VERIFY_PTR( pInfo );
  bimc_Init( pInfo );
}

/**
@copydoc HAL_bimc_Reset
*/
void HAL_bimc_Reset( HAL_bimc_InfoType *pInfo )
{
  BIMC_VERIFY_PTR( pInfo );
  bimc_Reset( pInfo );
}

/**
@copydoc HAL_bimc_Save
*/
void HAL_bimc_Save( HAL_bimc_InfoType *pInfo )
{
  BIMC_VERIFY_PTR( pInfo );
  bimc_Save( pInfo );
}

/**
@copydoc HAL_bimc_Restore
*/
void HAL_bimc_Restore( HAL_bimc_InfoType *pInfo )
{
  BIMC_VERIFY_PTR( pInfo );
  bimc_Restore( pInfo );
}

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_bimc_SetSlaveSegment
*/
void HAL_bimc_SetSlaveSegment
(
  HAL_bimc_InfoType *        pInfo,
  uint32_t                   uSlaveIdx,
  uint32_t                   uSegmentIdx,
  HAL_bimc_SlaveSegmentType *pSegment
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_SLAVE_IDX( pInfo, uSlaveIdx );
  BIMC_VERIFY_PTR( pSegment );
  bimc_SetSlaveSegment( pInfo, uSlaveIdx, uSegmentIdx, pSegment );
}

/**
@copydoc HAL_bimc_SetSlaveClockGating
*/
void HAL_bimc_SetSlaveClockGating
(
  HAL_bimc_InfoType *       pInfo,
  uint32_t                  uSlaveIdx,
  HAL_bimc_ClockGatingType *pGating
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_SLAVE_IDX( pInfo, uSlaveIdx );
  BIMC_VERIFY_PTR( pGating );
  bimc_SetSlaveClockGating( pInfo, uSlaveIdx, pGating );
}

/**
@copydoc HAL_bimc_SetMasterClockGating
*/
void HAL_bimc_SetMasterClockGating
(
  HAL_bimc_InfoType *       pInfo,
  uint32_t                  uMasterIdx,
  HAL_bimc_ClockGatingType *pGating
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_MASTER_IDX( pInfo, uMasterIdx );
  BIMC_VERIFY_PTR( pGating );
  bimc_SetMasterClockGating( pInfo, uMasterIdx, pGating );
}

/**
@copydoc HAL_bimc_ArbitrationEnable
*/
void HAL_bimc_ArbitrationEnable
(
  HAL_bimc_InfoType *pInfo,
  uint32_t           uSlaveIdx,
  bool               bEnable
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_SLAVE_IDX( pInfo, uSlaveIdx );
  bimc_ArbitrationEnable( pInfo, uSlaveIdx, bEnable );
}

/**
@copydoc HAL_bimc_SetQosMode
*/
void HAL_bimc_SetQosMode
(
  HAL_bimc_InfoType *  pInfo,
  uint32_t             uMasterIdx,
  HAL_bimc_QosModeType eMode
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_MASTER_IDX( pInfo, uMasterIdx );
  bimc_SetQosMode( pInfo, uMasterIdx, eMode );
}

/**
@copydoc HAL_bimc_SetQosPriority
*/
void HAL_bimc_SetQosPriority
(
  HAL_bimc_InfoType *       pInfo,
  uint32_t                  uMasterIdx,
  HAL_bimc_QosModeType      eMode,
  HAL_bimc_QosPriorityType *pPriority
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_MASTER_IDX( pInfo, uMasterIdx );
  BIMC_VERIFY_PTR( pPriority );
  bimc_SetQosPriority( pInfo, uMasterIdx, eMode, pPriority );
}

/**
@copydoc HAL_bimc_SetQosBandwidth
*/
void HAL_bimc_SetQosBandwidth
(
  HAL_bimc_InfoType *        pInfo,
  uint32_t                   uMasterIdx,
  HAL_bimc_QosBandwidthType *pBandwidth
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_MASTER_IDX( pInfo, uMasterIdx );
  BIMC_VERIFY_PTR( pBandwidth );
  bimc_SetQosBandwidth( pInfo, uMasterIdx, pBandwidth );
}

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_bimc_GetDeviceParameters
*/
void HAL_bimc_GetDeviceParameters
(
  HAL_bimc_InfoType *        pInfo,
  HAL_bimc_DeviceParamsType *pDevParams
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_PTR( pDevParams );
  bimc_GetDeviceParameters( pInfo, pDevParams );
}

/**
@copydoc HAL_bimc_GetMasterParameters
*/
void HAL_bimc_GetMasterParameters
(
  HAL_bimc_InfoType *            pInfo,
  uint32_t                       uMasterIdx, 
  HAL_bimc_MasterPortParamsType *pPortParams
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_MASTER_IDX( pInfo, uMasterIdx );
  BIMC_VERIFY_PTR( pPortParams );
  bimc_GetMasterParameters( pInfo, uMasterIdx, pPortParams );
}

/**
@copydoc HAL_bimc_GetSlaveParameters
*/
void HAL_bimc_GetSlaveParameters
(
  HAL_bimc_InfoType *           pInfo,
  uint32_t                      uSlaveIdx,
  HAL_bimc_SlavePortParamsType *pPortParams
)
{
  BIMC_VERIFY_PTR( pInfo );
  BIMC_VERIFY_SLAVE_IDX( pInfo, uSlaveIdx );
  BIMC_VERIFY_PTR( pPortParams );
  bimc_GetSlaveParameters( pInfo, uSlaveIdx, pPortParams );
}

