/*
===========================================================================

FILE:   SpiDeviceBam.c

DESCRIPTION:

===========================================================================

        Edit History

$Header: //components/rel/core.adsp/2.2/buses/spi/src/device/SpiDeviceBam.c#2 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
10/03/13 vmk    BAM CMD descriptor usage is determined from properties
05/30/12 ddk    Added support for common qup hal.
04/20/12 ddk    Added some more logging.
04/20/12 ddk    Added updates to enable logging.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
04/07/12 ddk    Updated BAM flags being used.
03/29/12 ddk    Added deinit steps.
12/10/12 ag     Created.
===========================================================================
        Copyright c 2011-13 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "SpiDeviceBam.h"
#include "SpiDevicePlatBam.h"
#include "SpiDeviceOsSvc.h"
#include <string.h>
#include "SpiDevicePlatSvc.h"
#include "SpiDevicePlatLog.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define SPIDEVICEBAM_CHK_RESULT_BREAK(res)\
   if (SPIDEVICE_RES_SUCCESS !=res) {\
      break;\
   }

static void spiDeviceBamCallBack(SpiDevicePlatBamResult *platBamRes)
{
   SpiBamPlatCBArgs *cbArgs = (SpiBamPlatCBArgs *)platBamRes->pUser;
   SpiDeviceBamType *spiBam = cbArgs->spiBamObj;
   SpiDeviceTransferType *transfer = spiBam->transfer;
   if(SPIDEVICE_BAM_WRITE_TAG == cbArgs->bamTags){
      transfer->bWriteTransferDone = TRUE;
      spiBam->result.result = platBamRes->result;
      IBUSLOG_LOG_LEVEL_2(0,"spiDeviceBamCallBack: Write Tag Callback\n");
   } else if (SPIDEVICE_BAM_READ_TAG == cbArgs->bamTags){
      transfer->bReadTransferDone = TRUE;
      spiBam->result.result = platBamRes->result;
      IBUSLOG_LOG_LEVEL_2(0,"spiDeviceBamCallBack: Read Tag Callback\n");
   }
   if((transfer->bReadTransferDone) && (transfer->bWriteTransferDone)){
      if(spiBam->pCallbackFn){
         spiBam->pCallbackFn(&(spiBam->result));
      }
   }
}

static void
spiDeviceBamCancelTransfer
(
 SpiDeviceBamType *spiBam, 
 SpiDeviceTransferType *transfer
)
{
	SpiDevice_ResetPlatBam(spiBam->hPlatBam);
}

static void
spiDeviceBamSetTransferCallback
(
 SpiDeviceBamType *spiBam,
 SPIDEVICE_BAM_TRANSFER_CB pCallBackFn
)
{
	spiBam->pCallbackFn = pCallBackFn;
}

static void
spiDeviceBamSetTransferCmdParms
(
 SpiDeviceBamType *spiBam, 
 SpiDeviceBamCmdParms *cmdParms
)
{
	spiBam->cmdParms = cmdParms;
}

static int32
spiDeviceBamGetResetDesc 
(
 SpiDeviceBamType *spiBam
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   uint32         pQupAddr;
   HAL_qupe_BamCommandDescType halBamCmdDescInfo;

   do
   {
      if (NULL == spiBam)
      {
         IBUSLOG_LOG_LEVEL_1(0,"spiDeviceBamGetResetDesc: Null ptr error\n");
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      pQupAddr = IBUSPLATCFG_PlatInternal_GetQupCorePhysicalAddr(spiBam->hPlat);


      halBamCmdDescInfo.cmd_element_base = spiBam->resetCmdDesc.physAddr;
      halBamCmdDescInfo.cmd_element_index = 0;
      
      
      result = HAL_qupe_SetCoreState_CommandElement((uint8 *)pQupAddr,
                                           HAL_QUPE_RUNSTATE_RESET,
                                           &halBamCmdDescInfo);

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         IBUSLOG_LOG_LEVEL_1(1,
            "spiDeviceBamGetResetDesc: Cmd element creation failed err 0x%x\n",
            (unsigned int )result);
         result = IBUSBAM_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      /* Save the command element index */
      spiBam->resetCmdDesc.cmd_element_index = halBamCmdDescInfo.cmd_element_index;
      spiBam->resetCmdDesc.size_in_bytes = halBamCmdDescInfo.cmd_size_in_bytes;

   }while(0);

   return result;
}


static int32
SpiDeviceBamGetSettingsDesc
(
   SpiDeviceBamType *spiBam
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   uint32         uPhysicalQupAddr;
   uint32         uVirtualQupAddr;
   HAL_qupe_BamCommandDescType halBamCmdDescInfo;

   do
   {
      if (NULL == spiBam)
      {
         IBUSLOG_LOG_LEVEL_1(0,"SpiDeviceBamGetSettingsDesc: Null ptr error\n");
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      if (NULL == spiBam->settingsCmdDesc.physAddr)
      {
         IBUSLOG_LOG_LEVEL_1(0,"SpiDeviceBamGetSettingsDesc: Null ptr error\n");
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      uPhysicalQupAddr = 
          IBUSPLATCFG_PlatInternal_GetQupCorePhysicalAddr(spiBam->hPlat);

      uVirtualQupAddr = 
          IBUSPLATCFG_PlatInternal_GetQupCoreVirtualAddr(spiBam->hPlat);

      halBamCmdDescInfo.cmd_element_base = spiBam->settingsCmdDesc.physAddr;
      halBamCmdDescInfo.cmd_element_index = 0;


      result = HAL_qupe_SetCoreMode_CommandElement((uint8 *)uPhysicalQupAddr,
                                           HAL_QUPE_MINI_CORE_SPI,
                                           &halBamCmdDescInfo);      

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         IBUSLOG_LOG_LEVEL_1(1,
            "SpiDeviceBamGetSettingsDesc: SetCoreMode Cmd element creation failed err 0x%x\n",
            (unsigned int )result);

         result = IBUSBAM_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      result = HAL_qupe_Spi_SetDeviceParameters_CommandElement(
                            (uint8 *)uPhysicalQupAddr,
                            (uint8 *)uVirtualQupAddr,
                            &(spiBam->mHalDeviceParameters),
                            &halBamCmdDescInfo);

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         IBUSLOG_LOG_LEVEL_1(1,
            "SpiDeviceBamGetSettingsDesc:"
            "SetDeviceParameters Cmd element creation failed err 0x%x\n",
            (unsigned int )result);
         result = IBUSBAM_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      result = HAL_qupe_Spi_SetTransferParameters_CommandElement(
                           (uint8 *)uPhysicalQupAddr,
                           &(spiBam->mHalTransferParameters),
                           &halBamCmdDescInfo);

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         IBUSLOG_LOG_LEVEL_1(1,
            "SpiDeviceBamGetSettingsDesc:"
            "SetTransferParameters Cmd element creation failed err 0x%x\n",
            (unsigned int )result);
         result = IBUSBAM_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      result = HAL_qupe_SetCoreState_CommandElement((uint8 *)uPhysicalQupAddr,
                                           HAL_QUPE_RUNSTATE_RUN,
                                           &halBamCmdDescInfo);

      if (HAL_QUPE_RESULT_SUCCESS != result)
      {
         IBUSLOG_LOG_LEVEL_1(1,
            "SpiDeviceBamGetSettingsDesc: SetCoreMode Cmd element t creation failed err 0x%x\n",
            (unsigned int )result);
         result = IBUSBAM_CMD_ELEMENT_CREATION_FAILED;
         break;
      }

      /* Save the command element index and calculate the no: of bytes */
      spiBam->settingsCmdDesc.cmd_element_index = halBamCmdDescInfo.cmd_element_index;
      spiBam->settingsCmdDesc.size_in_bytes = halBamCmdDescInfo.cmd_size_in_bytes;

   }while(0);


   return result;
}

static int32 
spiDeviceBamQueueCmdDescs
(
 SpiDeviceBamType *spiBam
 )
{
   int32 res = SPIDEVICE_RES_SUCCESS;

   void *writeCBArgs = (void *)&(spiBam->writeArgs);
   
   do
   {
      res = spiDeviceBamGetResetDesc(spiBam);

      SPIDEVICEBAM_CHK_RESULT_BREAK(res);

      res = SpiDevice_PlatBamTransfer(spiBam->hPlatBam, 
         SPIDEVICE_BAM_OUT, spiBam->resetCmdDesc.physAddr, 
         spiBam->resetCmdDesc.size_in_bytes, writeCBArgs,
         PLAT_BAM_DESC_FLAG_CMD | PLAT_BAM_DESC_FLAG_NWD);

      SPIDEVICEBAM_CHK_RESULT_BREAK(res);
      
      res = SpiDeviceBamGetSettingsDesc(spiBam);

      SPIDEVICEBAM_CHK_RESULT_BREAK(res);

      res = SpiDevice_PlatBamTransfer(spiBam->hPlatBam, 
         SPIDEVICE_BAM_OUT, (uint32)spiBam->settingsCmdDesc.physAddr,
            spiBam->settingsCmdDesc.size_in_bytes, writeCBArgs,
            PLAT_BAM_DESC_FLAG_CMD | PLAT_BAM_DESC_FLAG_NWD);

      SPIDEVICEBAM_CHK_RESULT_BREAK(res);

      IBUSLOG_LOG_LEVEL_2(0,"spiDeviceBamQueueCmdDescs: Cmd Desc Queued\n");
   }while(0);

   return res;

}

static int32  
spiDeviceBamQueueWriteDescs
(
 SpiDeviceBamType *spiBam
)
{
   int32 bamRes = SPIDEVICE_RES_SUCCESS;
   SpiDeviceTransferDataResult bufAvail, bufNextAvail;
   uint8 *dataBuf;
   uint32 uDataSize;
   uint32 uFlags = PLAT_BAM_DESC_FLAG_NO_SUBMIT;
   SpiDeviceTransferType *transfer = spiBam->transfer;
   void *writeCBArgs = (void *)&(spiBam->writeArgs);

   /*If the buffer is not the last buffer of the list then queue to 
    * platform BAM layer*/
   do{
      bamRes = SPIDEVICE_RES_SUCCESS;
      bufAvail = transfer->getWriteBufPhys(transfer, &dataBuf);
      uDataSize = transfer->getWriteBufLen(transfer);
      bufNextAvail = transfer->moveWriteNext(transfer);
   }while((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufNextAvail) &&
         (!(bamRes = SpiDevice_PlatBamTransfer(spiBam->hPlatBam, 
            SPIDEVICE_BAM_OUT, (uint32)dataBuf, uDataSize, 
            writeCBArgs, uFlags))));

   /*Check if we got the last buffer, then check if EOT is required
    * then set the flags according before queuing to BAM */
   if((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufAvail)&& !bamRes)
   {
      uFlags = 0; 
      
      if(spiBam->eot){
         uFlags |= PLAT_BAM_DESC_FLAG_EOT |PLAT_BAM_DESC_FLAG_NWD | PLAT_BAM_DESC_FLAG_INT ;
      }

      bamRes = SpiDevice_PlatBamTransfer(spiBam->hPlatBam, 
               SPIDEVICE_BAM_OUT, (uint32)dataBuf, uDataSize, 
               writeCBArgs, uFlags);
   }
   else if (SPIDEVICE_TRANSFER_BUFF_NONE == bufAvail)
   {
      /* Even if there is no output we need to send a Zero Length Packet*/
      uFlags = 0; 
      
      if(spiBam->eot){
         uFlags |= PLAT_BAM_DESC_FLAG_EOT |PLAT_BAM_DESC_FLAG_NWD;
      }

      bamRes = SpiDevice_PlatBamTransfer(spiBam->hPlatBam, 
               SPIDEVICE_BAM_OUT, 0, 0, 
               writeCBArgs, uFlags);
   }


   return bamRes;
}

static int32
spiDeviceBamQueueReadDescs
(
 SpiDeviceBamType *spiBam
)
{
   int32 bamRes = SPIDEVICE_RES_SUCCESS;
   SpiDeviceTransferDataResult bufAvail, bufNextAvail;
   uint8 *dataBuf;
   uint32 uDataSize;
   uint32 uFlags = PLAT_BAM_DESC_FLAG_NO_SUBMIT;
   SpiDeviceTransferType *transfer = spiBam->transfer;
   void *readCBArgs = (void *)&(spiBam->readArgs);

   /*If the buffer is not the last buffer of the list then queue to 
    * platform BAM layer*/
   do{
      bamRes = SPIDEVICE_RES_SUCCESS;
      bufAvail = transfer->getReadBufPhys(transfer, &dataBuf);
      uDataSize = transfer->getReadBufLen(transfer);
      bufNextAvail = transfer->moveReadNext(transfer);
   }while((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufNextAvail) &&
         (!(bamRes = SpiDevice_PlatBamTransfer(spiBam->hPlatBam, 
                     SPIDEVICE_BAM_IN, (uint32)dataBuf, uDataSize, 
                     readCBArgs, uFlags))));

   /*Check if we got the last buffer, then check if EOT is required
    * then set the flags according before queuing to BAM */
   if((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufAvail) && !bamRes){
      /*We always queue (meaning remove the PLAT_BAM_DESC_FLAG_NO_SUBMIT 
       * flag) the last read transfer*/
      uFlags = 0;
      if(spiBam->eot){
         uFlags |= PLAT_BAM_DESC_FLAG_EOT | PLAT_BAM_DESC_FLAG_INT;
      }
      bamRes = SpiDevice_PlatBamTransfer(spiBam->hPlatBam, 
            SPIDEVICE_BAM_IN, (uint32)dataBuf, uDataSize, 
            readCBArgs, uFlags);
   }

   return bamRes;
}

int32
spiDeviceBamStartTransfer
(
 SpiDeviceBamType *spiBam, 
 SpiDeviceTransferType *transfer,
 void *pUser,
 boolean eot
)
{
   int32 res = SPIDEVICE_RES_SUCCESS;

   do
   {
      spiBam->result.pUser = pUser;
      spiBam->transfer = transfer;
      spiBam->eot = eot;
      
      /*Queue the descriptors to BAM*/
      if (TRUE == spiBam->useCmdDescs)
      {
         res = spiDeviceBamQueueCmdDescs(spiBam);
         SPIDEVICEBAM_CHK_RESULT_BREAK(res);
      }
   
      res = spiDeviceBamQueueReadDescs(spiBam);
      SPIDEVICEBAM_CHK_RESULT_BREAK(res);
   
      res = spiDeviceBamQueueWriteDescs(spiBam);
      SPIDEVICEBAM_CHK_RESULT_BREAK(res);
   }while(0);
   
   return res;
}

static void
spiDeviceBamSetTransferParameters
(
   SpiDeviceBamType *spiBam, 
   HAL_QUPE_SPI_TransferParametersType *mHalTransferParameters
)
{
   memcpy(&spiBam->mHalTransferParameters,
      mHalTransferParameters,
      sizeof(spiBam->mHalTransferParameters));    
}

static void
spiDeviceBamSetDeviceParameters
(
    SpiDeviceBamType *spiBam, 
    HAL_QUPE_SPI_DeviceParametersType *mHalDeviceParameters
)
{
   memcpy(&spiBam->mHalDeviceParameters,
      mHalDeviceParameters,
      sizeof(spiBam->mHalDeviceParameters));     
}

int32 SpiDeviceBamInit
(
 SpiDeviceBamType **pSpiBam, 
 IBUSPLATCFG_HANDLE  hPlat
 )
{
   SpiDeviceBamType *spiBam = NULL;
   int32 res = SPIDEVICE_RES_SUCCESS;
   int32 return_value = SPIDEVICE_RES_SUCCESS;
   
   do
   {
      if(IBUSOS_RESULT_OK != (res = IBUSOS_Malloc((void **)&spiBam, 
                                              sizeof(SpiDeviceBamType))))
      {
         IBUSLOG_LOG_LEVEL_1(0,"SpiDeviceBamInit: spiBam Malloc failure\n");
         break;
      }
      IBUSOS_Memset((void *)spiBam, 0, sizeof(SpiDeviceBamType));
      
       /* TODO: To pass the correct argument for parg */
      if(SPIDEVICE_RES_SUCCESS != 
         (res = SpiDevice_InitPlatBam(hPlat, 
                                      &(spiBam->hPlatBam), 
                                      spiDeviceBamCallBack,NULL)))
      {
         IBUSLOG_LOG_LEVEL_1(1,
                     "SpiDeviceBamInit: SpiDevice_InitPlatBam failure , err 0x%x\n",
                     (unsigned int)res);
         break;
      }

      spiBam->resetCmdDesc.physMemHandle = NULL;
      spiBam->settingsCmdDesc.physMemHandle = NULL;   
      
      if(IBUSOS_RESULT_OK != 
          (res = IBUSOSSPI_PhysMemAlloc(SETTINGS_CMD_BUFFER_SIZE, 
                                            &(spiBam->settingsCmdDesc.physMemHandle),
                                            &(spiBam->settingsCmdDesc.physAddr),
                                            &(spiBam->settingsCmdDesc.virtualAddr))))
      {
         IBUSLOG_LOG_LEVEL_1(0,"SpiDeviceBamInit: Settings cmd desc Malloc failure\n");
         break;
      }
   
   
      if(IBUSOS_RESULT_OK != 
          (res = IBUSOSSPI_PhysMemAlloc(RESET_CMD_BUFFER_SIZE, 
                                            &(spiBam->resetCmdDesc.physMemHandle),
                                            &(spiBam->resetCmdDesc.physAddr),
                                            &(spiBam->resetCmdDesc.virtualAddr))))
      {
         IBUSLOG_LOG_LEVEL_1(0,"SpiDeviceBamInit: Reset cmd desc Malloc failure\n");
         break;
      }
   
      spiBam->hPlat = hPlat;
      spiBam->startBamTransfer = spiDeviceBamStartTransfer;
      spiBam->cancelBamTransfer = spiDeviceBamCancelTransfer;
      spiBam->setBamTransferCallback = spiDeviceBamSetTransferCallback;
      spiBam->setBamTransferCmdParms = spiDeviceBamSetTransferCmdParms;
      spiBam->setQupTransferParms = spiDeviceBamSetTransferParameters;
      spiBam->setQupDeviceParms = spiDeviceBamSetDeviceParameters;
      spiBam->readArgs.spiBamObj = spiBam;
      spiBam->readArgs.bamTags = SPIDEVICE_BAM_READ_TAG;
      spiBam->writeArgs.spiBamObj = spiBam;
      spiBam->writeArgs.bamTags = SPIDEVICE_BAM_WRITE_TAG;
      *pSpiBam = spiBam;
   
   }while(0);   

   if (SPIDEVICE_RES_SUCCESS != res)
   {
      return_value = res;

      res = SpiDeviceBamDeInit(spiBam,hPlat);
   }

   return return_value;
}


int32 SpiDeviceBamDeInit
(
   SpiDeviceBamType *spiBam, 
   IBUSPLATCFG_HANDLE	hPlat
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;

   do
   {
      if (NULL == spiBam)
      {
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      result = SpiDevice_DeInitPlatBam(hPlat,spiBam->hPlatBam);

      if (SPIDEVICE_RES_SUCCESS != result)
      {
         IBUSLOG_LOG_LEVEL_1(1,"SpiDevice_DeInitPlatBam failed err 0x%x\n",
                   (unsigned int)result);
      }
      
      if (NULL != spiBam->resetCmdDesc.physMemHandle)
      {
         result = IBUSOSSPI_PhysMemFree(spiBam->resetCmdDesc.physMemHandle);

         spiBam->resetCmdDesc.physMemHandle = NULL;
         spiBam->resetCmdDesc.virtualAddr = NULL;         
         spiBam->resetCmdDesc.physAddr = NULL;         
         
         if (SPIDEVICE_RES_SUCCESS != result)
         {
            IBUSLOG_LOG_LEVEL_1(1,"SpiDeviceBamDeInit: Free failed err 0x%x\n",
                          (unsigned int)result);
         }
      }

      if (NULL != spiBam->settingsCmdDesc.physMemHandle)
      {
         result = IBUSOSSPI_PhysMemFree(spiBam->settingsCmdDesc.physMemHandle);

         spiBam->settingsCmdDesc.physMemHandle = NULL;
         spiBam->settingsCmdDesc.virtualAddr = NULL;         
         spiBam->settingsCmdDesc.physAddr = NULL;          
         
         if (SPIDEVICE_RES_SUCCESS != result)
         {
            IBUSLOG_LOG_LEVEL_1(1,"SpiDeviceBamDeInit: Free failed err 0x%x\n",
                           (unsigned int)result);
         }
      }

      result = IBUSOS_Free(spiBam);

   }while(0);
   
   return result;
}
