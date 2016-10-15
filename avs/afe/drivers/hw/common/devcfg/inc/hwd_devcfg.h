
#ifndef HWDDEVCFG_H
#define HWDDEVCFG_H

#ifdef __cplusplus
#include <new>
extern "C" {
#endif //__cplusplus

//#include "typedef_drivers.h"
#include "qurt_elite.h"
#include "lpasshwio_devcfg.h"

#define DEBUG_HWD_DEV_CONFIG_DATA
#undef DEBUG_HWD_DEV_CONFIG_DATA

typedef enum 
{
   HWD_MODULE_TYPE_INVALID = -1,
   HWD_MODULE_TYPE_AUDIOIF,
   HWD_MODULE_TYPE_AVTIMER,
   HWD_MODULE_TYPE_DMLITE,
   HWD_MODULE_TYPE_RESAMPLER,
   HWD_MODULE_TYPE_MIDI,
	HWD_MODULE_TYPE_LPASS
} HwdModuleType;

/** @brief Read driver Configure Data based on LPASS HW version

   @param[in] moduleType - module type
   @param[out] pDev - pointer to device property type struct
   
   @return
   Success/failure of configure data read
*/   
ADSPResult HwdDevCfg_ReadModuleConfigData(HwdModuleType moduleType, void *pDev);

/** @brief query if the hw module is supported or not

   @param[in] moduleType - module type
  
   @return
   TRUE if is supported
   FALSE if it is not supported
*/

ADSPResult HwdDevCfg_Init(void);

boolean HwdDevCfg_HWSupport(HwdModuleType moduleType);

ADSPResult HwdDevCfg_GetVirtAddress(qurt_mem_region_t *pQmem_region, uint32 phyAddr, uint32 regSize, uint32 *pVirtAddr);

ADSPResult HwdDevCfg_DestoryVirtAddress(qurt_mem_region_t qmem_region);

void HwdDevCfg_GetLpassHwVersion(uint32_t * lpass_hw_version);

#ifdef DEBUG_HWD_DEV_CONFIG_DATA
void HwdDevCfg_Debug(void);
#endif

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef HWDDEVCFG_H
