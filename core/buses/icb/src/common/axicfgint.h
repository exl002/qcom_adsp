#ifndef __AXICFGINT_H__
#define __AXICFGINT_H__
/**
 * @file axicfgint.h
 * @note Internal header file for Bus Configuration
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/core.adsp/2.2/buses/icb/src/common/axicfgint.h#1 $ 
 * $DateTime: 2013/04/03 17:22:53 $ 
 * $Author: coresvc $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 12/05/2011   dj      Created file
 *  
 *  
 * Copyright (c) 2011-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 */ 

#include "ddiaxicfgint.h"
#include "icb_soc_data.h"

#ifdef __cplusplus
extern "C" {
#endif

struct axicfg_soc_info 
{
  icb_master_def_type ** pMasterList;
  uint32_t uMasterCount;
  icb_slave_def_type ** pSlaveList;
  uint32_t uSlaveCount;
  icb_bus_def_type ** pBusList;
  uint32_t uBusCount;
};

struct axicfg_master_search_info
{
  ICBId_MasterType   ePort;
  icb_master_def_type * pCurrMaster;
};

struct axicfg_slave_search_info
{
  ICBId_SlaveType eSlave;
  icb_slave_def_type * pCurrSlave;
};

/**
  @brief Used to check if an external master id is valid

  @pre   pInfo != NULL

  @param[inout] pInfo  pointer to structure that's used to hold master search info

  @return bool
    @retval true  if valid
    @retval false if not valid
 */
bool AxiCfgInt_SearchPort( struct axicfg_master_search_info *pInfo );

/**
  @brief Used to check if an external slave id is valid

  @pre   pInfo != NULL

  @param[inout] pInfo  pointer to structure that's used to hold slave search info

  @return bool
    @retval true  if valid
    @retval false if not valid
 */
bool AxiCfgInt_SearchSlave( struct axicfg_slave_search_info *pInfo  );

/**
  @brief Used to check if an external master id is a BIMC master id

  @pre   AxiCfgInt_SearchPort returned true on pInfo
         NULL != puCount
         NULL != puMasterPort

  @param[inout] pInfo         pointer to structure that's used to hold master search info
  @param[out] puMasterPort  pointer to internal master port id array
  @param[out] puCount       pointer to internal master port id count

  @return bool
    @retval true  if pInfo->ePort is valid and puMasterPort, puCount are set
    @retval false if pInfo->ePort is invalid. Both puMasterPort and puCount are not set
 */
bool AxiCfgInt_GetBimcMasterInfo( struct axicfg_master_search_info *pInfo, uint32_t ** puMasterPort, uint32_t * puCount );

/**
  @brief Used to check if an external master id is remoted

  @pre   AxiCfgInt_SearchPort returned true on pInfo

  @param[in] pInfo  pointer to structure that's used to hold master search info
 
  @return bool
    @retval true  if pInfo->ePort is remoted
    @retval false if pInfo->ePort is not remoted or unsupported 
 */
bool AxiCfgInt_IsRemoteMaster( struct axicfg_master_search_info *pInfo );

/**
  @brief Used to check if an external slave id is a BIMC slave id

  @pre   AxiCfgInt_SearchSlave returned true on pInfo
         puSlaveWay != NULL
         puCount != NULL 

  @param[inout] pInfo         pointer to structure that's used to hold slave search info
  @param[out] puSlaveWay    pointer to internal slave port id array
  @param[out] puCount       pointer to internal slave port id count

  @return bool
    @retval true  if pInfo->eSlave is valid and puSlaveWay, puCount are set
    @retval false if pInfo->eSlave is invalid. Both puSlaveWay and puCount are not set
 */
bool AxiCfgInt_GetBimcSlaveInfo( struct axicfg_slave_search_info *pInfo, uint32_t ** puSlaveWay, uint32_t * puCount );

/**
  @brief Used to check if an external slave id is remoted

  @pre   AxiCfgInt_SearchSlave returned true on pInfo

  @param[in] pInfo         pointer to structure that's used to hold slave search info
 
  @return bool
    @retval true  if eSlave is remoted
    @retval false if eSlave is not remoted or unsupported 
 */
bool AxiCfgInt_IsRemoteSlave( struct axicfg_slave_search_info *pInfo );

/**
  @brief Used to get master hal info

  @pre   AxiCfgInt_SearchPort returned true on pInfo
         pSearchInfo != NULL 
         pHalInfo != NULL 
         *pHalInfo != NULL

  @param[in]  pSearchInfo   pointer to structure that's used to hold master search info
  @param[out] pHalInfo      a pointer to master hal info
 */
void AxiCfgInt_GetMasterHalInfo( struct axicfg_master_search_info *pSearchInfo, void **pHalInfo );

/**
  @brief Used to get slave hal info

  @pre   AxiCfgInt_SearchSlave returned true on pInfo
         pSearchInfo != NULL 
         pHalInfo != NULL 
         *pHalInfo != NULL

  @param[in]  pSearchInfo   pointer to structure that's used to hold slave search info
  @param[out] pHalInfo      a pointer to slave hal info
 */
void AxiCfgInt_GetSlaveHalInfo( struct axicfg_slave_search_info *pSearchInfo, void **pHalInfo );


#ifdef __cplusplus
}
#endif

#endif /* __AXICFGINT_H__ */
