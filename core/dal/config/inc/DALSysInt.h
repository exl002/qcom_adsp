#ifndef DALSYSINT_H
#define DALSYSINT_H
/*==============================================================================
@file DALSysInt.h


Device Config Internal Functionality


        Copyright � 2007 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DDIPlatformInfo.h"
#include "DDIChipInfo.h"
/*------------------------------------------------------------------------------
* Defines
*-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
* Type declaration
*-----------------------------------------------------------------------------*/

typedef struct {
   DalPlatformInfoPlatformType platform_id;
   DALProps *plinker_platform_seg_sym;
}DEVCFG_PLATFORM_SEG_INFO;

typedef struct {
   uint32 tcsr_soc_info ;
   DALProps *plinker_soc_seg_sym;
   DEVCFG_PLATFORM_SEG_INFO *platform_info;
   uint32 num_platform_type;
}DEVCFG_TARGET_INFO;

/* 
Bits	Field Name	Description for TCSR_SOC_HW_VERSION register
31:28	FAMILY_NUMBER	
27:16	DEVICE_NUMBER	
15:8	MAJOR_VERSION	
7:0	MINOR_VERSION	
*/

typedef struct {
   uint16 hw_version ;
   uint16 soc_number ;
}DEVCFG_PLATFORM_INFO;

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*------------------------------------------------------------------------------
* Function declaration and documentation
*-----------------------------------------------------------------------------*/
typedef struct DALPropsDir DALPropsDir;
struct DALPropsDir
{
   uint32 dwLen;
   uint32 dwPropsNameSectionOffset;
   uint32 dwPropsStringSectionOffset;
   uint32 dwPropsByteSectionOffset;
   uint32 dwPropsUint32SectionOffset;
   uint32 dwNumDevices;
   uint32 dwPropsDeviceOffset[1][2]; // structure like this will follow...
};

/**
* @brief Identify the device config of interset for SOC/PLATFORM in execution 
*          -- internal API used by  DALPROPS.c 

* @param: pTargetInfoTableIter reference to meta data for 
*         all possible configuration
*
* @return void
*/
void
DALSYS_DoPropsSymLookUp(DEVCFG_TARGET_INFO *pTargetInfoTableIter);

/**
* @brief Identify the chipset -- internal API used by  DALPROPS.c 

* @param: void
*
* @return void
*/
void
DALSYS_ReadSocHwInfo(void);

/**
* @brief Load the shared object at runtime for device configuration 
*        -- internal API used by  DALPROPS.c 

* @param: void
*
* @return void
*/
DALResult DEVCFG_LOADER(void);

/**
* @brief Deload the shared object at runtime for device configuration 
*        -- internal API used by  DALPROPS.c 

* @param: void
*
* @return void
*/
DALResult DEVCFG_DELOADER(void);

/**
* @brief Get the DALConfig Property Dir -- internal API used by 
*        DALPROPS.c 

* @param: pDALPropsDir output parameter gets populate with 
*       property directory ptr
*
* @return void
*/
void
DALSYS_GetPropDir(DALPropsDir ** pDALPropsDir, DEVCFG_TARGET_INFO_TYPE dwInfoType);

/**
* @brief Get the DALConfig Property Info -- internal API used by 
*        DALPROPS.c 

* @param: pDALPropsDir output parameter gets populate with 
*       property directory ptr
*
* @return void
*/
DALProps *
DALSYS_GetPropsInfo(DEVCFG_TARGET_INFO_TYPE dwInfoType);

/**
* @brief Do string device object lookup for object reference -- internal 
*        API used by DALPROPS.c & DALModEnv.c

* @param pszDevName       : Desired Device Name.
         ppStringDeviceObj: DeviceObject pointer.
         pDALPtr         : DalProperty pointer.
*
* @return StringDevice Structure ptr
*/
DALResult DAL_StringDeviceObjectLookup(const char *pszDevName, 
                                       const StringDevice **ppStringDeviceObj,
                                       DALProps *pDALPtr);

/**
* @brief Get the DALConfig Property Structure array -- internal 
*        API used by DALPROPS.c

* @param: void
*
* @return DAL Prop Structure ptr
*/

const void ** 
DALSYS_GetPropStructPtrArray(DALProps *DALPROP_PropsInfo);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* DALSYSINT_H */
