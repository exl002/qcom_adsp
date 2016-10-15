#ifndef __DDISPIPD_H__
#define __DDISPIPD_H__
/**
  @file ddispipd.h 
  @brief 
  This file contains the interface declaration for SPI DAL device driver.
*/ 
/*===========================================================================

FILE:   ddispipd.h

DESCRIPTION:
    This file contains DAL interface declaration for SPI device driver, 
    Physical driver layer (DAL0).

===========================================================================

        Edit History

$Header: 

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
06/05/11 ag	   Doxygen Markups.
04/09/10 wx     Created

===========================================================================
        Copyright c 2010 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================*/

#include "DalDevice.h"
#include "spi_defs.h"
/**
@mainpage SPI DAL PD Documentation

@section S1 Introduction

Serial Peripheral Interface (SPI) DAL PD provides an abstracted interface to clients to access the SPI hw block for sending and receiving data.

@section S2 Interfaces
The clients are expected to first attach to the SPI hw by calling
\c DAL_SpiPdDeviceAttach. It is expected that the client will validate the
return value of the \c DAL_SpiPdDeviceAttach. prior to making any calls to any
of the interfaces documented here.

@subsection SS21 Parameters
All SPI DAL functions take device handle the client obtained from the attach process as their first parameter. There will be some level of validation on the parameters. However, the handles passed are assumed to be valid and that the client has done adequate validation against them at the time of the attach process.
*/
/**
@defgroup spipd_dal_api SPI DAL APIs for clients
@ingroup spipd_api
This section defines the various data structures used in the SPI DAL API. 

@defgroup spipd_dal_data_structures Data Structures.
@ingroup spipd_api
This section defines the various data structures used in the SPI DAL API. 

@defgroup spipd_dal_enum Enumeration Constants  
@ingroup spipd_api
This section defines the various enums used in the SPI DAL API. 
*/

/* Interface version format is (major,minor) */
#define DALSPIPD_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

typedef enum SpiPdStatusType SpiPdStatusType;
/**
@addtogroup spipd_dal_enum
@{ */
/**
 extend DALResult in addition to DAL_SUCCESS and DAL_ERROR.
*/ 
enum SpiPdStatusType
{
   SPIPD_ERROR_INVALID_PARAMETER  = 1,/**< INVALID paramets*/
   SPIPD_ERROR_BUS_ACQUISITION_TIMEOUT,/**< TIMED OUT waiting for lock*/
   SPIPD_ERROR_BUSY,/**< Unable to obtain the BUS lock */
   SPIPD_ERROR_OPERATION_DENIED,/**<Operation failed because Client hasn't locked the bus */
   SPIPD_ERROR_OPERATION_FAILED/**< Operation Failure*/
};
 /** 
@} 
*/

typedef struct DalPdSpiTransferType DalPdSpiTransferType;
/**
@addtogroup spi_dal_data_structures 
@{ */
/**
 Data structure that contains information regarding the SPIPD transfer.
*/ 
struct DalPdSpiTransferType
{
  SpiDataAddrType dataAddr;/**< Pointers to input and output data buffers */

  uint32 uDataLen;/**< units of uDataLen and uTotalLen in bytes
				    length of the data buffer */

  /* length of data actually read */
  uint32 uTotalLen;/**<length of data actually read */

   /*
    * nunmber of transfers is derive from uDataLena and N.
    * If N <= 8, the unit is in bytes (8 bits)
    * If 16 >=  N > 8, the unit is in half word (16 bits).
    * If N > 16, the unit is in word (32 bits).
    */
};
 /** 
@} 
*/

typedef struct DalSpiPd DalSpiPd;
struct DalSpiPd
{
   struct DalDevice DalDevice;
   DALResult (*Lock)(DalDeviceHandle * _h, int32  uWaitTime);
   DALResult (*Unlock)(DalDeviceHandle * _h);
   DALResult (*CombinedWriteRead)(DalDeviceHandle * _h,  SpiDeviceInfoType * pConfig, uint32 olen1,  DalPdSpiTransferType *  pRead,  DalPdSpiTransferType *  pWrite, uint32 olen3);
   DALResult (*Read)(DalDeviceHandle * _h,  SpiDeviceInfoType * pConfig, uint32 olen1,  DalPdSpiTransferType *  pRead);
   DALResult (*Write)(DalDeviceHandle * _h,  SpiDeviceInfoType * pConfig, uint32 olen1,  DalPdSpiTransferType *  pWrite, uint32 olen2);
   DALResult (*GetCoreProperty)(DalDeviceHandle * _h,  SpiInfoType *  pCoreProperty);
};

typedef struct DalSpiPdHandle DalSpiPdHandle; 
struct DalSpiPdHandle 
{
   uint32 dwDalHandleId;
   const DalSpiPd * pVtbl;
   void * pClientCtxt;
};

/**
@addtogroup spipd_dal_api
@{ */
/**
*DAL_SpipdDeviceAttach()
*
*	@brief
*	Just a convenient wrapper around DAL_DeviceAttachEx.
*
*
*	@param DevId - DalDevice Id of the SPI device to which the client intends to 
*				   attach.
*	@param hDalDevice - A pointer location to be filled by the driver with a handle 
*			to the DalDevice.
*
*	@dependencies
*  	None
*
*	@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
*/
#define DAL_SpiPdDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALSPIPD_INTERFACE_VERSION,hDalDevice)
/** 
@} 
*/

/**
@ingroup spipd_dal_api
@{ */
/**
* DalSpiPd_Lock()
*	@brief
*  		This function is used to gain exclusive access to the SPI bus. Function would 
*  		block until the lock is acquired or timeout, specified by uWaitTime.
*  		Clients must call this function prior to any read/write operation on the bus.
*  		Function does not support nested acquisitions.
*
*  	@param _h - Pointer to a DAL device handle
*  	@param uWaitTime - in Msec, the max wait time the function will wait for the
*                       lock to be available. -1 means wait forever. 
*                       0 means don't wait, bailout immediately if the lock cannot be acquired.
*
*	@dependencies
*  	None
*
*	@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*    SIDE EFFECTS
*  		This function acquires inter-client and inter-processor level locks for this
*  		SPI device.
*  		Function will exclude access of the SPI bus from other client upon success 
*  		return. DalSpiPd_Unlock() must be used later to release the BUS.
*
*/
static __inline DALResult
DalSpiPd_Lock(DalDeviceHandle * _h, int32  uWaitTime)
{
   return ((DalSpiPdHandle *)_h)->pVtbl->Lock( _h, uWaitTime);
}
/** 
@} 
*/


/**
@ingroup spipd_dal_api
@{ */
/**
* DalSpiPd_Unlock()
*	@brief
*        This function is used to release the lock that was previously acquired by 
*        DalSpiPd_Lock.
*        Clients must call this function after read/write operation is complete.
*
*  	@param _h - Pointer to a DAL device handle
*
*	@dependencies
*  	None
*
*	@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
*/
static __inline DALResult
DalSpiPd_Unlock(DalDeviceHandle * _h)
{
   return ((DalSpiPdHandle *)_h)->pVtbl->Unlock( _h);
}
/** 
@} 
*/


/**
@ingroup spipd_dal_api
@{ */
/**
* DalSpiPd_CombinedWriteRead()
*	@brief
*  		Conduct a bidirectional data transfer.
*  		The data transfer will be queued for its desired SPI core with 
*  		its desired configuration.
*  		
*  		This function is synchronous.
*
*  		pRead and pWrite contain buffer pointers are with void types that have both
*  		physical and virtual addresses in void * type.
*  		If unpacking is enabled, the output buffer is interpreted as word array, 
*  		  and is casted into (uint32 *)
*  		If packing is enabled, the input buffer is interpreted as word array, 
*  		  and is casted into (uint32 *)
*  		Otherwise, there are no packing/unpacking here:
*  		  N <= 8, (void *) is interpreted as byte array, and is casted into (uint8 *)
*  		  8 < N <=16 (void *) is interpreted as half-word array, and is casted into (uint16 *)
*  		  N > 16 (void *) is interpreted as word array, and is casted into (uint32 *)
*  		The above rules also applies to the same name parameter in Send, Receive, and SendAndReceive
*  		To illustrate the rules, here is the table to show data flow with different 
*  		  N and unpacking settings for outputData. inputData works similarly but governed
*  		  with Packing option:
* 
*  		  A 32Bit Data in the FIFO = 0x1234abcd, Word passed in by client
*  		                           =     0xabcd, Half Word passed in by client
*  		                           =       0xcd, Byte passed in by client
*  		  
*  		  N               UnPk       Transfers
*  		                  En         #1           #2      #3      #4      Comments
*  		   
*  		  32              X          0x1234abcd                           Client passes in Word Array.
*  		  16              0          0xabcd                               Client passes in Half Word Array.
*  		                  1          0xabcd       0x1234                  Client passes in Word Array.
*  		  8               0          0xcd                                 Client passes in Byte Array.
*  		                  1          0xcd         0xab    0x34    0x12    Client passes in Word Array.
*  		                  
*  		  28              X          0x234abcd                            Client passes in Word Array.
*  		  (16 < N < 32)
* 
*  		  12              0          0xbcd                                Client passes in Half Word Array.
*  		  (8<N<16)        1          0xbcd        0x234                   Client passes in Word Array.
* 
*  		  4               0          0xd                                  Client passes in Byte Array.
*  		  (N<8)           1          0xd          0xb     0x4     0x2     Client passes in Word Array.
*  		  X indiate does not matter.
*
*  	@param _h - Pointer to a DAL device handle
*    @param pConfig - SPI bus configuration for this transfer. See DALSpiConfigType for details.
*    @param olen1 - size of pConfig
*    @param pRead - buffers and sizes for receiving transfer data
*    @param pWrite - buffers and sizes for sending transfer data out
*    @param olen3 - size of pWrite
*
*
*	@dependencies
*  	None
*
*	@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
*/
static __inline DALResult
DalSpiPd_CombinedWriteRead(DalDeviceHandle * _h,  SpiDeviceInfoType * pConfig,  DalPdSpiTransferType *  pRead,  DalPdSpiTransferType *  pWrite)
{
   return ((DalSpiPdHandle *)_h)->pVtbl->CombinedWriteRead( _h, pConfig,sizeof( SpiDeviceInfoType ), pRead, pWrite,sizeof( DalPdSpiTransferType  ));
}
/** 
@} 
*/


/**
@ingroup spipd_dal_api
@{ */
/**
* DalSpiPd_Read()
*	@brief
*      Conduct a data read-only transfer, get input data.
*      This function is synchrous. It blocks until all data are read or failure happens.
*      Please see DalSpiPd_CombinedWriteRead for detailed pack/unpack rules.
*
*  	@param _h - Pointer to a DAL device handle
*    @param pConfig - SPI bus configuration for this transfer. See DALSpiConfigType for details.
*    @param olen1 - size of pConfig
*    @param pRead - buffers and sizes for receiving transfer data
*
*	@dependencies
*  	None
*
*	@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
**/
static __inline DALResult
DalSpiPd_Read(DalDeviceHandle * _h,  SpiDeviceInfoType * pConfig,  DalPdSpiTransferType *  pRead)
{
   return ((DalSpiPdHandle *)_h)->pVtbl->Read( _h, pConfig,sizeof( SpiDeviceInfoType ), pRead);
}
/** 
@} 
*/


/**
@ingroup spipd_dal_api
@{ */
/**
* DalSpiPd_Write()
*	@brief
*      Conduct a data write-only transfer, send out output data.
*      This function is synchrous. It blocks until all data are written or failure happens.
*      Please see DalSpiPd_CombinedWriteRead for detailed pack/unpack rules.
*
*  	@param _h - Pointer to a DAL device handle
*    @param pConfig - SPI bus configuration for this transfer. See DALSpiConfigType for details.
*    @param olen1 - size of pConfig
*    @param pWrite - buffers and sizes for sending transfer data out
*    @param olen2 - size of pWrite
*
*	@dependencies
*  	None
*
*	@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
*/
static __inline DALResult
DalSpiPd_Write(DalDeviceHandle * _h,  SpiDeviceInfoType * pConfig,  DalPdSpiTransferType *  pWrite)
{
   return ((DalSpiPdHandle *)_h)->pVtbl->Write( _h, pConfig,sizeof( SpiDeviceInfoType ), pWrite,sizeof( DalPdSpiTransferType  ));
}
/** 
@} 
*/


/**
@ingroup spipd_dal_api
@{ */
/**
*   DalSpiPd_GetCoreProperty()
*	@brief
*		This function get property of SPI core.
*  	@param _h - Pointer to a DAL device handle
*    @param pCoreProperty - SPI property is returned in this parameter.
*
*	@dependencies
*  	None
*
*	@return
*	DAL_SUCCESS if successful, DAL_ERROR if an error occured
*
*/
static __inline DALResult
DalSpiPd_GetCoreProperty(DalDeviceHandle * _h,  SpiInfoType *  pCoreProperty)
{
   return ((DalSpiPdHandle *)_h)->pVtbl->GetCoreProperty( _h, pCoreProperty);
}
/** 
@} 
*/


#endif /* __DDISPIPD_H__ */
