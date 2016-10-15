
#ifndef HWDDEVCFG_INTERNAL_H
#define HWDDEVCFG_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "hwd_devcfg.h"

static ADSPResult HwdDevCfg_ReadAudioIfConfigData(HwdAudioIfPropertyType *pDev);
static ADSPResult HwdDevCfg_ReadAvtimerConfigData(HwdAvtimerPropertyType *pDev);
static ADSPResult HwdDevCfg_ReadDmliteConfigData(HwdDmlitePropertyType *pDev);
static ADSPResult HwdDevCfg_ReadResamplerConfigData(HwdResamplerPropertyType *pDev);
static ADSPResult HwdDevCfg_ReadMidiConfigData(HwdMidiPropertyType *pDev);
static ADSPResult HwdDevCfg_ReadLpassConfigData(HwdLpassPropertyType *pDev);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef HWDDEVCFG_INTERNAL_H
