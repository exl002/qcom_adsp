/*========================================================================
   This file contains HWD device configuration functions

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/afe/drivers/hw/common/devcfg/src/hwd_devcfg.c#21 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
//#include "lpm_prop.h"   // enable it when fwk lpm prop ready
#include "DALSys.h"
#include "DALSysTypes.h"
#include "DDIChipInfo.h"

#include "common.h"
#include "lpasshwio_devcfg.h"
#include "hwd_devcfg.h"
#include "hwd_devcfg_internal.h"
#include "qurt_elite.h"
#include "LPASS_ADDRESS_FILE.h"
#include <stringl.h>

#define DAL_CHIP_INFO_CHIP_VERSION_1    0x00010001   //it is 0x00010001 in real hw. will change to this once cosim supported


typedef struct
{
   HwdAudioIfPropertyType     audIfProp;
   HwdAvtimerPropertyType     avtProp;
   HwdDmlitePropertyType      dmlProp; 
   HwdResamplerPropertyType   rsProp;
   HwdMidiPropertyType        midiProp; 
   HwdLpassPropertyType       lpass_prop; 
} HwdDevCfgPropertyType;

static boolean               devCfgInitFlag = FALSE;
static HwdDevCfgPropertyType hwdDevCfgProp;

static DalChipInfoVersionType chipVersion;  //set V1 as default

static uint32_t hwd_dev_cfg_lpass_hw_ver;


ADSPResult HwdDevCfg_Init(void)
{
   ADSPResult rc = ADSP_EFAILED;

   if(ADSP_EOK != (rc = HwdDevCfg_ReadAudioIfConfigData(&(hwdDevCfgProp.audIfProp))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadAudioIfConfigData failed"); 
   }
   else if(ADSP_EOK != (rc = HwdDevCfg_ReadAvtimerConfigData(&(hwdDevCfgProp.avtProp))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadAvtimerConfigData failed"); 
   }
   else if(ADSP_EOK != (rc = HwdDevCfg_ReadDmliteConfigData(&(hwdDevCfgProp.dmlProp))))
   { 
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadDmliteConfigData failed"); 
   }
   else if(ADSP_EOK != (rc = HwdDevCfg_ReadResamplerConfigData(&(hwdDevCfgProp.rsProp))))
   { 
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadResamplerConfigData failed"); 
   }
   else if(ADSP_EOK != (rc = HwdDevCfg_ReadMidiConfigData(&(hwdDevCfgProp.midiProp))))
   { 
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadMidiConfigData failed"); 
   }
   else if(ADSP_EOK != (rc = HwdDevCfg_ReadLpassConfigData(&(hwdDevCfgProp.lpass_prop))))
   { 
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: HwdDevCfg_ReadLpassConfigData failed"); 
   }
   else
   {
      devCfgInitFlag = TRUE;
      rc = ADSP_EOK;
   }
   
   return rc;
}

#ifdef DEBUG_HWD_DEV_CONFIG_DATA
void HwdDevCfg_Debug(void)
{

   uint32 i, maxDataSizeInDword;
   uint32 *pConfigData;

   if(TRUE == devCfgInitFlag)
   {

      maxDataSizeInDword = sizeof(HwdDevCfgPropertyType)/4;
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DevCfg: HwdDevCfg Data Dump Start"); 
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Seq: audIfProp avtProp dmlProp rsProp midiProp lpass_prop"); 

      pConfigData = (uint32 *)&hwdDevCfgProp;

      for(i = 0; i < maxDataSizeInDword; i++)
      {
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"DevCfgData: 0x%x", (unsigned int)*pConfigData); 
          pConfigData++;
      }
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DevCfg: HwdDevCfg Data Dump End"); 
    
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DevCfg: HwdDevCfg Not initialized yet"); 
   }
}
#endif

ADSPResult HwdDevCfg_ReadModuleConfigData(HwdModuleType moduleType, void *pDev)
{
   ADSPResult rc = ADSP_EOK;

   if(NULL != pDev)
   {
      if(devCfgInitFlag) 
      {
         // Read Driver Configuration Data
         switch(moduleType)
         {
            // case AudioIf
            case HWD_MODULE_TYPE_AUDIOIF:
            {
               memscpy(pDev, sizeof(HwdAudioIfPropertyType),
                       (void *)&(hwdDevCfgProp.audIfProp), sizeof(HwdAudioIfPropertyType));
               break;
            }
            // case AVTimer
            case HWD_MODULE_TYPE_AVTIMER: 
            {
               memscpy(pDev, sizeof(HwdAvtimerPropertyType),
                      (void *)&(hwdDevCfgProp.avtProp), sizeof(HwdAvtimerPropertyType));
               break;
            }
            // case DMLite
            case HWD_MODULE_TYPE_DMLITE: 
            {
               memscpy(pDev, sizeof(HwdDmlitePropertyType),
                      (void *)&(hwdDevCfgProp.dmlProp), sizeof(HwdDmlitePropertyType));
               break;
            }
            // case Resampler
            case HWD_MODULE_TYPE_RESAMPLER:
            {
               memscpy(pDev, sizeof(HwdResamplerPropertyType),
                      (void *)&(hwdDevCfgProp.rsProp), sizeof(HwdResamplerPropertyType));
               break;
            }
            // case MIDI
            case HWD_MODULE_TYPE_MIDI:
            {
               memscpy(pDev, sizeof(HwdMidiPropertyType),
                      (void *)&(hwdDevCfgProp.midiProp), sizeof(HwdMidiPropertyType));
               break;
            }
            case HWD_MODULE_TYPE_LPASS:
            {
               memscpy(pDev, sizeof(HwdLpassPropertyType),
                      (void *)&(hwdDevCfgProp.lpass_prop), sizeof(HwdLpassPropertyType));
               break;
            }
            default:
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DevCfg: Invalid HWD Module Type %d", moduleType); 
               rc = ADSP_EFAILED;
            }   
         }
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DevCfg: it is not initialized sccessfully"); 
         rc = ADSP_EFAILED;
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: NULL dev pointer\n"); 
      rc = ADSP_EFAILED;
   }
   
   return rc;
}

boolean HwdDevCfg_HWSupport(HwdModuleType moduleType)
{
   boolean result = FALSE;

   if(devCfgInitFlag) 
   {
      switch(moduleType)
      {
         // case AudioIf
         case HWD_MODULE_TYPE_AUDIOIF:
         {
            if(0 != hwdDevCfgProp.audIfProp.hw_revision)
            {
              result = TRUE;
            }
            break;
         }
         // case AVTimer
         case HWD_MODULE_TYPE_AVTIMER: 
         {
            if(0 != hwdDevCfgProp.avtProp.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
         // case DMLite
         case HWD_MODULE_TYPE_DMLITE: 
         {
            if(0 != hwdDevCfgProp.dmlProp.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
         // case Resampler
         case HWD_MODULE_TYPE_RESAMPLER:
         {
            if(0 != hwdDevCfgProp.rsProp.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
         // case MIDI
         case HWD_MODULE_TYPE_MIDI:
         {
            if(0 != hwdDevCfgProp.midiProp.hw_revision)
            {
               result = TRUE;
            }
            break;
         }
         default:
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DevCfg: Invalid HWD Module Type %d", moduleType); 
         }   
      } //switch: moduleType
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"DevCfg: it is not initialized sccessfully"); 
   }
   
   return result;
}

static ADSPResult HwdDevCfg_ReadAudioIfConfigData(HwdAudioIfPropertyType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   ADSPResult rc = ADSP_EOK;
   HwdAudioIfPropertyType *pAudIfProp;
   char *devId = "AUDIOIF";
   uint32 i;
   
   // get device handle
   if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      // error handle
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
         "DevCfg: Error establishing AudioIf dev handles returned %d", rc);  
   }
   else
   {
      // read property ptr
      if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp,"AudioIfPropStructPtr",0,&PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
            "DevCfg: Error getting AudioIf Property Struct Ptr returned %d", rc);  
      }
      else
      {
         pAudIfProp = (HwdAudioIfPropertyType *)PropVar.Val.pStruct;

         pDev->audHwRegBase = pAudIfProp->audHwRegBase;
         pDev->audHwRegSize = pAudIfProp->audHwRegSize;
         pDev->hdmiAudHwRegBase = pAudIfProp->hdmiAudHwRegBase;
         pDev->hdmiAudHwRegSize = pAudIfProp->hdmiAudHwRegSize;
         pDev->dmaWordSize = pAudIfProp->dmaWordSize;
         pDev->outgoingDmaChCnt = pAudIfProp->outgoingDmaChCnt;
         pDev->incomingDmaChCnt = pAudIfProp->incomingDmaChCnt;
         pDev->infTypeCnt = pAudIfProp->infTypeCnt;
         for(i=0; i < MAX_AUDIF_TYPE_NUM; i++)
         {
            pDev->infTypeList[i] = pAudIfProp->infTypeList[i];
            pDev->infTypeDataLineCaps[i] = pAudIfProp->infTypeDataLineCaps[i];
            pDev->infTypeDirection[i] = pAudIfProp->infTypeDirection[i];
         }
         pDev->isrHookPinNum = pAudIfProp->isrHookPinNum;
         pDev->intrOutputIndex = pAudIfProp->intrOutputIndex;
         pDev->hw_revision = pAudIfProp->hw_revision; 
         pDev->avtimer_latch_type = pAudIfProp->avtimer_latch_type;
      }
   }
   
   return rc;
}

static ADSPResult HwdDevCfg_ReadAvtimerConfigData(HwdAvtimerPropertyType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   ADSPResult rc = ADSP_EOK;
   HwdAvtimerPropertyType *pAvtProp;
   char *devId = "AVTIMER";
    
   // get device handle
   if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      // error handle
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
         "DevCfg: Error establishing avtimer dev handles returned %d", rc);  
   }
   else
   {
      // read property ptr
      if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp,"AvtimerPropStructPtr",0,&PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
            "DevCfg: Error getting Avtimer Property Struct Ptr returned %d", rc);  
      }   
      else
      {
         int i;

         pAvtProp = (HwdAvtimerPropertyType *)PropVar.Val.pStruct;

         for(i = 0; i < NUM_MAX_AVTIMER_HW_INSTANCE; i++)
         {
            pDev->baseRegAddr[i] = pAvtProp->baseRegAddr[i];
            pDev->regSize[i] = pAvtProp->regSize[i];
            pDev->rootClk[i] = pAvtProp->rootClk[i];
         }
         pDev->isrHookPinNum = pAvtProp->isrHookPinNum;
         pDev->hw_revision = pAvtProp->hw_revision; 
      } 
   }
   
   return rc;
}

static ADSPResult HwdDevCfg_ReadDmliteConfigData(HwdDmlitePropertyType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   ADSPResult rc = ADSP_EOK;
   HwdDmlitePropertyType *pDmlProp;
   char *devId = "DMLITE";

   // get device handle
   if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      // error handle
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
         "DevCfg: Error establishing dmlite dev handles returned %d", rc);  
   }
   else
   {
      // read property ptr
      if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp,"DmlitePropStructPtr",0,&PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
            "DevCfg: Error getting Dmlite Property Struct Ptr returned %d", rc);  
      }   
      else
      {
         pDmlProp = (HwdDmlitePropertyType *)PropVar.Val.pStruct;

         pDev->baseRegAddr = pDmlProp->baseRegAddr;
         pDev->regSize = pDmlProp->regSize;
         pDev->intPinNo = pDmlProp->intPinNo;
         pDev->hw_revision = pDmlProp->hw_revision; 
      } 
   }
   
   return rc;
}

static ADSPResult HwdDevCfg_ReadResamplerConfigData(HwdResamplerPropertyType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   ADSPResult rc = ADSP_EOK;
   HwdResamplerPropertyType *pRsProp;
   char *devId = "RESAMPLER";
    
   // get device handle
   if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      // error handle
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
         "DevCfg: Error establishing resampler dev handles returned %d", rc);  
   }
   else
   {
      // read property ptr
      if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp,"ResamplerPropStructPtr",0,&PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
            "DevCfg: Error getting Resampler Property Struct Ptr returned %d", rc);  
      }   
      else
      {
         pRsProp = (HwdResamplerPropertyType *)PropVar.Val.pStruct;

         pDev->baseRegAddr = pRsProp->baseRegAddr;
         pDev->regSize = pRsProp->regSize;
         pDev->isrHookPinNum = pRsProp->isrHookPinNum; 
         pDev->hw_revision = pRsProp->hw_revision;

         //handle the difference between v1 and v2
         //hw resampler is supported on v2 or up
         chipVersion = DalChipInfo_ChipVersion();

         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"HwdDevCfg_ReadResamplerConfigData ChipVersion:%ld\n", chipVersion);

         if(DAL_CHIP_INFO_CHIP_VERSION_1 == chipVersion)
         {
#ifndef SIM
            pDev->baseRegAddr = 0;
            pDev->regSize = 0;
            pDev->isrHookPinNum = 0; 
            pDev->hw_revision = 0;
#endif
         }
      }
   }
   
   return rc;
}

static ADSPResult HwdDevCfg_ReadMidiConfigData(HwdMidiPropertyType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   ADSPResult rc = ADSP_EOK;
   HwdMidiPropertyType *pMidiProp;
   char *devId = "MIDI";

   // get device handle
   if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      // error handle
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
         "DevCfg: Error establishing MIDI dev handles returned %d", rc);
   }
   else
   {
      // read property ptr
      if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp,"MidiPropStructPtr",0,&PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "DevCfg: Error getting MIDI Property Struct Ptr returned %d", rc);
      }
      else
      {
         pMidiProp = (HwdMidiPropertyType *)PropVar.Val.pStruct;

         pDev->baseRegAddr = pMidiProp->baseRegAddr;
         pDev->regSize = pMidiProp->regSize;
         pDev->intPinNo = pMidiProp->intPinNo;
         pDev->hw_revision = pMidiProp->hw_revision;
      }
   }

   return rc;
}


static ADSPResult HwdDevCfg_ReadLpassConfigData(HwdLpassPropertyType *pDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar       PropVar;
   ADSPResult              rc = ADSP_EOK;
   HwdLpassPropertyType    *p_lpass_prop;
   char                    *devId = "LPASS";
   qurt_mem_region_t       lpass_qmem_region;
   uint32                  reg_virt_base, reg_offset;

   /* Get device handle */
   if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetDALPropertyHandleStr(devId, hProp)))
   {
      /* Error handle */
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: Error establishing LPASS dev handles returned %d", rc);
      return ADSP_EFAILED;
   }
   else
   {
      /* Read property ptr */
      if(ADSP_FAILED(rc = (ADSPResult)DALSYS_GetPropertyValue(hProp,"LpassPropStructPtr", 0, &PropVar)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DevCfg: Error getting LPASS Property Struct Ptr returned %d", rc);
         return ADSP_EFAILED;
      }
      else
      {
         p_lpass_prop = (HwdLpassPropertyType *)PropVar.Val.pStruct;
         pDev->hw_ver_reg_base = p_lpass_prop->hw_ver_reg_base;
      }
   }

   if(ADSP_EOK != (rc = HwdDevCfg_GetVirtAddress(&lpass_qmem_region,
                                                 pDev->hw_ver_reg_base,
                                                 0x1000, /* 4 KB min qurt_mem_region size */
                                                 &reg_virt_base)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPASS dev config: Error obtaining hw_version register virtual addr");

      return ADSP_EFAILED;
   }

   reg_offset = ((uint32_t *)reg_virt_base - (uint32_t *)pDev->hw_ver_reg_base);

   ReadRegister1(pDev->hw_ver_reg_base, reg_offset, &hwd_dev_cfg_lpass_hw_ver);

   /* Unmap the region after reading register value */
   HwdDevCfg_DestoryVirtAddress(lpass_qmem_region);

   return rc;
}

void HwdDevCfg_GetLpassHwVersion(uint32_t * lpass_hw_version)
{
   *lpass_hw_version = hwd_dev_cfg_lpass_hw_ver;
}
