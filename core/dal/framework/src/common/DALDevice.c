/*======================================================================
@file  DALDevice.c

DAL device management

        Copyright � 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
 ======================================================================*/
#include "DALStdErr.h"
#include "DALSysCmn.h"
#include "DALSys.h"
#include "DALReg.h"
#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALFramework.h"
#include "DALSysInt.h"
#ifdef FEATURE_DAL_REMOTE
#include "DALQdiDefs.h"
#endif

//List of Drivers in a Module.
extern DALREG_DriverInfoList gDALModDriverInfoList;

/*------------------------------------------------------------------------------
Per process global that holds the list of drivers modules
------------------------------------------------------------------------------*/
DALREG_DriverInfoList * gDALProcessDriverModList[DALSYSCMN_MAX_NUM_MODS_PER_PROCESS];

/*------------------------------------------------------------------------------
synchronization object to protect the driver module list
------------------------------------------------------------------------------*/
static DALSYSSyncHandle ghDALProcessDriverModListSync = NULL;

/*------------------------------------------------------------------------------
DAL Environment
------------------------------------------------------------------------------*/
extern DALProps DAL_Mod_Info;

#define _DALSYSCMN_MOD_LIST_SYNC_LOCK()    DALSYS_SyncEnter(ghDALProcessDriverModListSync);

#define _DALSYSCMN_MOD_LIST_SYNC_UN_LOCK() DALSYS_SyncLeave(ghDALProcessDriverModListSync);


static DALResult
DALSYSCMN_GetDeviceObject(DALDEVICEID DevId, const char *pszArg, 
                          DalDeviceHandle **phDevice)
{
   uint32 dwModIdx = 0;
   /*--------------------------------------------------------------------------
   For all modules in this list, find the drivers infos
   --------------------------------------------------------------------------*/
   for(dwModIdx=0;dwModIdx<DALSYSCMN_MAX_NUM_MODS_PER_PROCESS;dwModIdx++)
   {
      DALREG_DriverInfoList *pModDriverInfoList = gDALProcessDriverModList[dwModIdx];

      if(NULL != pModDriverInfoList)
      {
         uint32 dwDriverIdx = 0;
         /*------------------------------------------------------------------
           For all drivers of this mod search for requested device id
          -----------------------------------------------------------------*/
         for(dwDriverIdx =0;dwDriverIdx<pModDriverInfoList->dwLen;dwDriverIdx++)
         {
            /*--------------------------------------------------------------
            For each devices supported by this driver
            --------------------------------------------------------------*/
            uint32 dwDeviceIdx = 0;
            DALREG_DriverInfo *pDriverInfo = pModDriverInfoList->pDriverInfo[dwDriverIdx];
            for(dwDeviceIdx=0;dwDeviceIdx<pDriverInfo->dwNumDevices;dwDeviceIdx++)
            {
               if(DevId == pDriverInfo->pDeviceId[dwDeviceIdx])
               {
                  /*------------------------------------------------------
                  search complete
                  ------------------------------------------------------*/
                  return 
                  pDriverInfo->pfnDALNewFunc(pszArg,DevId,phDevice);
               }
            }
         }
      }
   }
   /*--------------------------------------------------------------------------
   Did not find the requested device Id in this process
   --------------------------------------------------------------------------*/
   DALSYS_LOG_ERROR_EVENT("Device not found- pszArg:0x%x", pszArg);
   return DAL_ERROR_DEVICE_NOT_FOUND;
}

static DALResult
DALSYSCMN_GetStringDeviceObject(const char *pszDevName, DalDeviceHandle **phDevice)
{ 
   StringDevice const *pStringDeviceObj = NULL;
   DALREG_DriverInfo *pDriverInfo = NULL;
   
   if(DAL_SUCCESS == DAL_StringDeviceObjectLookup(pszDevName, &pStringDeviceObj, &DAL_Mod_Info)){
      //Get the Property Dir Info Ptr
      pDriverInfo = pStringDeviceObj->pFunctionName;
      return pDriverInfo->pfnDALNewFunc(pszDevName,0,phDevice);
   } 

   DALSYS_LOG_ERROR_EVENT("device not found- pszDevName:%s", pszDevName);
   return DAL_ERROR_DEVICE_NOT_FOUND;
}

static DALResult
DALSYSCMN_RegisterModDriverList(DALREG_DriverInfoList *pModDriverList)
{
   uint32 dwModIdx = 0;
   _DALSYSCMN_MOD_LIST_SYNC_LOCK();
   for(dwModIdx=0;dwModIdx<DALSYSCMN_MAX_NUM_MODS_PER_PROCESS;dwModIdx++)
   {
      if(NULL == gDALProcessDriverModList[dwModIdx])
      {
         gDALProcessDriverModList[dwModIdx] = pModDriverList;
         _DALSYSCMN_MOD_LIST_SYNC_UN_LOCK();
         return DAL_SUCCESS;
      }
   }
   _DALSYSCMN_MOD_LIST_SYNC_UN_LOCK();
   DALSYS_LOG_ERROR_EVENT( "Failed- pModDriverList:0x%x", pModDriverList);
   return DAL_ERROR;
}

static DALResult
DALSYSCMN_DeRegisterModDriverList(DALREG_DriverInfoList *pModDriverList)
{
   uint32 dwModIdx = 0;
   _DALSYSCMN_MOD_LIST_SYNC_LOCK();
   for(dwModIdx=0;dwModIdx<DALSYSCMN_MAX_NUM_MODS_PER_PROCESS;dwModIdx++)
   {
      if(pModDriverList == gDALProcessDriverModList[dwModIdx])
      {
         gDALProcessDriverModList[dwModIdx] = NULL;
         _DALSYSCMN_MOD_LIST_SYNC_UN_LOCK();
         return DAL_SUCCESS;
      }
   }
   _DALSYSCMN_MOD_LIST_SYNC_UN_LOCK();
   DALSYS_LOG_ERROR_EVENT("Failed- pModDriverList:0x%x", pModDriverList);
   return DAL_ERROR;
}

DALResult
DAL_DeviceAttachLocal(const char *pszArg, DALDEVICEID DevId,DalDeviceHandle **phDalDevice)
{
    DALResult ret;

    *phDalDevice = NULL; // in case of error

    _DALSYSCMN_MOD_LIST_SYNC_LOCK(); // Lock during actual attach call

    ret = DALSYSCMN_GetDeviceObject(DevId, pszArg, phDalDevice);

    if (DAL_SUCCESS == ret)
    {
        // grab reference count after attach but within lock
        uint32 refcnt = DAL_GET_DEVICE_ATTACH_COUNT(*phDalDevice);   

        // if first attach was me and this time, then call init 
        if (1 == refcnt)
        {
            ret = DalDevice_Init(*phDalDevice);
            if (DAL_SUCCESS != ret)
            {
                DAL_DeviceDetach(*phDalDevice);
                *phDalDevice = NULL;
            }
        }
    }

    _DALSYSCMN_MOD_LIST_SYNC_UN_LOCK();

    return ret;
}

DALResult
DAL_DeviceAttach(DALDEVICEID DevId,DalDeviceHandle **phDalDevice)
{
    DALResult ret = DAL_ERROR;

    ret = DAL_DeviceAttachLocal(NULL, DevId, phDalDevice);

#ifdef FEATURE_DAL_REMOTE_CLIENT
    /* device not found locally, try remoting*/
    if (DAL_ERROR_DEVICE_NOT_FOUND == (uint32)ret)
    {
        ret = _DoRemoteDeviceAttach(DevId, phDalDevice);
        if (ret != DAL_SUCCESS)
        {
            *phDalDevice = NULL;
        }

    }
    else
#endif
    /* if failed due to any other reason, return immediately*/
    if (DAL_SUCCESS != ret)
    {
        *phDalDevice = NULL;
    }


    return ret;
}

DALResult
DAL_DeviceAttachEx(const char *pszArg,
                   DALDEVICEID DevId,
                   DALInterfaceVersion ClientVersion,
                   DalDeviceHandle **phDalDevice)
{
    DalDeviceInfo DeviceInfo;
    DALResult ret = DAL_ERROR;

    ret = DAL_DeviceAttachLocal(NULL, DevId, phDalDevice);  
    if ( DAL_SUCCESS == ret)
    {
        if (DAL_SUCCESS != DalDevice_Info(*phDalDevice,&DeviceInfo,
                                          sizeof(DeviceInfo)))
            return DAL_ERROR;

        //do version check
        if (DALINTERFACE_COMPATIBLE(ClientVersion, DeviceInfo.Version))
            return DAL_SUCCESS;

        //failed version check, interface change
        (void)DAL_DeviceDetach(*phDalDevice);
        *phDalDevice = NULL;
        return DAL_ERROR_DDI_VERSION;   
    }

#ifdef FEATURE_DAL_REMOTE_CLIENT
    /* device not found locally, try remoting*/
    else if (DAL_ERROR_DEVICE_NOT_FOUND == (uint32)ret)
    {
        ret =  _DoRemoteDeviceAttachEx(pszArg, DevId, ClientVersion, phDalDevice);
        if (ret != DAL_SUCCESS)
        {
            *phDalDevice = NULL;
            DALSYS_LOG_ERROR_EVENT("device not found- DevId:0x%x", DevId);
        }
    }
#endif

    /* if failed due to any other reason, return immediately*/
    else
    {
        *phDalDevice = NULL;
    }    

    return ret;
}

DALResult
DAL_StringDeviceAttachLocal(const char *pszArg, const char *pszDevName,
                            DalDeviceHandle **phDalDevice)
{
   DALResult ret;

   *phDalDevice = NULL; // in case of error

   _DALSYSCMN_MOD_LIST_SYNC_LOCK(); // Lock during actual attach call

   ret = DALSYSCMN_GetStringDeviceObject(pszDevName, phDalDevice);
   
   if(DAL_SUCCESS == ret)
   {
      // grab reference count after attach but within lock
      uint32 refcnt = DAL_GET_DEVICE_ATTACH_COUNT(*phDalDevice);   

      // if first attach was me and this time, then call init 
      if(1 == refcnt)  
      {
         ret = DalDevice_Init(*phDalDevice);
         if(DAL_SUCCESS != ret)
         {
            DAL_DeviceDetach(*phDalDevice);
            *phDalDevice = NULL;
         }
      }
   }
  
  _DALSYSCMN_MOD_LIST_SYNC_UN_LOCK();

   return ret;
}

DALResult
DAL_StringDeviceAttach(const char *pszDevName, DalDeviceHandle **phDalDevice)
{
    DALResult ret;

    ret = DAL_StringDeviceAttachLocal(NULL, pszDevName, phDalDevice);

#ifdef FEATURE_DAL_REMOTE_CLIENT
    /* device not found locally, try remoting*/
    if (DAL_ERROR_DEVICE_NOT_FOUND == (uint32)ret)
    {
        ret = _DoRemoteStringDeviceAttach(pszDevName, phDalDevice);
        if (ret != DAL_SUCCESS)
        {
           DALSYS_LOG_ERROR_EVENT("device not found: pszDevName:%s", pszDevName);
           *phDalDevice = NULL;
        }

    }
    else
#endif

    /* if failed due to any other reason, return immediately*/
    if (DAL_SUCCESS != ret)
    {
        *phDalDevice = NULL;
    }

    return ret;
}

DALResult
DAL_StringDeviceAttachEx(const char *pszArg,
                         const char *pszDevName,
                         DALInterfaceVersion ClientVersion,
                         DalDeviceHandle **phDalDevice)
{
    DalDeviceInfo DeviceInfo;
    DALResult ret = DAL_ERROR;

    ret = DAL_StringDeviceAttachLocal(NULL, pszDevName, phDalDevice);
    if ( DAL_SUCCESS == ret)
    {
        if (DAL_SUCCESS != DalDevice_Info(*phDalDevice,&DeviceInfo,
                                          sizeof(DeviceInfo)))
            return DAL_ERROR;

        //do version check
        if (DALINTERFACE_COMPATIBLE(ClientVersion, DeviceInfo.Version))
            return DAL_SUCCESS;

        //failed version check, interface change
        (void)DAL_DeviceDetach(*phDalDevice);
        *phDalDevice = NULL;
        return DAL_ERROR_DDI_VERSION;   
    }

#ifdef FEATURE_DAL_REMOTE_CLIENT
    /* device not found locally, try remoting*/
    else if (DAL_ERROR_DEVICE_NOT_FOUND == (uint32)ret)
    {
        ret =  _DoRemoteStringDeviceAttachEx(pszArg, pszDevName, 
              ClientVersion, phDalDevice);
        if (ret != DAL_SUCCESS)
        {
            DALSYS_LOG_ERROR_EVENT("device not found remotely: pszDevName:%s", 
                  pszDevName);
            *phDalDevice = NULL;
        }
    }
#endif

    /* if failed due to any other reason, return immediately*/
    else
    {
        *phDalDevice = NULL;
    }    

    return ret;
}

#ifdef FEATURE_DAL_REMOTE_CLIENT
DALResult
DAL_DeviceAttachRemote(const char *pszArg,
                       DALDEVICEID DevId,
                       DALInterfaceVersion ClientVersion,
                       DalDeviceHandle **phDalDevice)
{
    DALResult ret;

    ret =  _DoRemoteDeviceAttachEx(pszArg, DevId, ClientVersion, phDalDevice);
    if (ret != DAL_SUCCESS)
    {
        *phDalDevice = NULL;
        DALSYS_LOG_ERROR_EVENT("failed- DevId:0x%x", DevId);
    }

    return ret;
}

DALResult
DAL_StringDeviceAttachRemote(const char *pszArg,
                             const char *pszDevName,
                             DALInterfaceVersion ClientVersion,
                             DalDeviceHandle **phDalDevice)
{
    DALResult ret;

    ret =  _DoRemoteStringDeviceAttachEx(pszArg, pszDevName, ClientVersion, 
          phDalDevice);
    if (ret != DAL_SUCCESS)
    {
        DALSYS_LOG_ERROR_EVENT("device not found: pszDevName:%s", pszDevName);
        *phDalDevice = NULL;
    }
    return ret;
}
#endif

DALResult
DAL_DeviceDetach(DalDeviceHandle * hDalDevice)
{
   if(hDalDevice)
   {
#ifdef FEATURE_DAL_REMOTE_CLIENT
      if(DALISREMOTEDEVICE(hDalDevice))
         _DoRemoteDetach(hDalDevice);
      else
#endif
      {
         //do the detach
         _DALSYSCMN_MOD_LIST_SYNC_LOCK(); 
         if(1 == DAL_GET_DEVICE_ATTACH_COUNT(hDalDevice))
         {
             (void)DalDevice_DeInit(hDalDevice);
         }
         DalDevice_Detach(hDalDevice);
         _DALSYSCMN_MOD_LIST_SYNC_UN_LOCK();
      }
      return DAL_SUCCESS;
   }
   DALSYS_LOG_ERROR_EVENT("hDalDevice is NULL");
   return DAL_ERROR;
}


void DALDeviceFwk_Init(void)
{
   //create the global sync lock
   if(NULL == ghDALProcessDriverModListSync)
   {
      DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
            &ghDALProcessDriverModListSync, NULL);
   }
   DALSYSCMN_RegisterModDriverList(&gDALModDriverInfoList);

#ifdef FEATURE_DAL_REMOTE
   DALRemoteInit();
#endif   
}

void DALDeviceFwk_DeInit(void)
{
#ifdef FEATURE_DAL_REMOTE
   DALRemoteDeinit();
#endif   
   DALSYSCMN_DeRegisterModDriverList(&gDALModDriverInfoList);
   if (NULL != ghDALProcessDriverModListSync)
   {
      DALSYS_DestroySyncObject(ghDALProcessDriverModListSync);
      ghDALProcessDriverModListSync = NULL;
   }
}
