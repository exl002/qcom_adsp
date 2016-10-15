/**
 * @file sns_ddf_comm.c
 * @brief Implementation of the Device Driver Framework communication module.
 *
 * @see sns_ddf_comm.h
 *
 * Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/ddf/src/8084/sns_ddf_comm.c#1 $
  $DateTime: 2014/05/16 10:56:56 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2014-04-07 DC   Fix return code in SNS_DDF_BUS_SPI when DalDevice_Open fails
  2014-02-26 MW   Fix KW errors
  2014-01-30 lka  Added sns_ddf_read_port_u32_count()
  2014-01-28 dk   Added SPI transfer
  2014-01-27 hw   Add QDSS logging
  2013-07-05 MW	  Added single transfer (simple) I2C read and write APIs.  2013-05-01 lka  Added IO functions for enhanced DDF interface.
  2013-04-28 vh   DDF handle validation before calling read/write
  2013-03-18 vh   Added appropriate return errors
  2013-02-14 ag   - Added support for 16-bit register addressing
                  - I2C transfer optimizations
                  - support for multiple reads without STOP
  2013-01-09 pn   Updated sns_ddf_write_port() to handle zero-byte write.
  2012-09-26 sc   Handle I2C read/write error condition (call I2C close)
  2012-09-12 sc   Call I2C open/close in each read/write operation
  2012-08-29 sc   Enabled I2C access by default
  2012-08-22 sc   Populated bus_instance[] array (based on Amolika's fix)
  2012-06-08 vh   Added run time switch for I2C code
  2012-05-17 vh   Enabled I2C functionality
  2012-05-01 vh   Modified to support latest I2C driver
  2012-02-16 yk   Added sns_ddf_write_port_byte
  2012-01-05 br   Adjust alignment
  2011-11-27 br   Deleted an unnecessary line in both ddf_read_port() and ddf_write_port()
  2011-07-19 dc   Lowercase inclusion of DALStdErr fails on linux builds.
  2011-07-01 agk  Bypassed DAL1 for i2c communication.
  2011-04-11 yk   Moved ASSERT macro from header file to here.
  2011-03-11 ag   Update path to handle building on Linux machines
  2011-01-21 sc   Modified on PCSIM support.
  2010-11-22 pg   Added raw I2C logs.
  2010-08-23 yk   Added alternate implementation for PCSim (via flag SNS_PCSIM).
  2010-08-20 yk   Added cleanup in case of failure of open_port().
  2010-08-16 yk   Initial revision
==============================================================================*/

#include "sns_ddf_comm.h"
#include "sns_profiling.h"
#if !defined (SNS_PCSIM)

#include <DALDeviceId.h>
#include <DALStdErr.h>
#include <DALStdDef.h>
#include <DALSys.h>
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_ddf_util.h"
#include "sns_memmgr.h"
#include "DalDevice.h"
#include "I2cTransfer.h"
#include "I2cTransferCfg.h"
#include "ddii2c.h"
#include "sns_debug_str.h"
#include "ddispi.h"

#include "sns_ddf_comm.h"

/*----------------------------------------------------------------------------
 * Defines and Constants
 * -------------------------------------------------------------------------*/
#define DDF_COMM_MAX( ARG1, ARG2 )         (((ARG1) > (ARG2)) ? (ARG1) : (ARG2))

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
typedef struct
{
  DalDeviceHandle *hI2c;
  uint8 slave_addr;
  sns_ddf_i2c_reg_addr_e reg_addr_type;
}sns_ddf_sensor_dali2c_info_s;

typedef struct
{
  DalDeviceHandle *hSPI;
  uint8 slave_select;
  SpiTransferModeType transferMode;/**< Spi transfer mode type to be used for the transfer.*/
}sns_ddf_sensor_dalspi_info_s;

typedef struct
{
  sns_ddf_bus_e bus;
  union
  {
    sns_ddf_sensor_dali2c_info_s i2c_s;
    sns_ddf_sensor_dalspi_info_s spi_s;
  };
}sns_ddf_sensor_info_s;
/*----------------------------------------------------------------------------
 * Global Variables
 * -------------------------------------------------------------------------*/
volatile boolean EnableI2C = true;
volatile boolean EnableSPI = false;

/*----------------------------------------------------------------------------
 * Local Variables
 * -------------------------------------------------------------------------*/
static const DALDEVICEID bus_instances[] =
{
  0,
  DALDEVICEID_I2C_DEVICE_1,
  DALDEVICEID_I2C_DEVICE_2,
  DALDEVICEID_I2C_DEVICE_3,
  DALDEVICEID_I2C_DEVICE_4,
  DALDEVICEID_I2C_DEVICE_5,
  DALDEVICEID_I2C_DEVICE_6,
  DALDEVICEID_I2C_DEVICE_7,
  DALDEVICEID_I2C_DEVICE_8,
  DALDEVICEID_I2C_DEVICE_9,
  DALDEVICEID_I2C_DEVICE_10,
  DALDEVICEID_I2C_DEVICE_11,
  DALDEVICEID_I2C_DEVICE_12
};

static const DALDEVICEID spi_bus_instances[] = 
{
  0,
  DALDEVICEID_SPI_DEVICE_1,
  DALDEVICEID_SPI_DEVICE_2,
  DALDEVICEID_SPI_DEVICE_3,
  DALDEVICEID_SPI_DEVICE_4,
  DALDEVICEID_SPI_DEVICE_5,
  DALDEVICEID_SPI_DEVICE_6,
  DALDEVICEID_SPI_DEVICE_7,
  DALDEVICEID_SPI_DEVICE_8,
  DALDEVICEID_SPI_DEVICE_9,
  DALDEVICEID_SPI_DEVICE_10,
  DALDEVICEID_SPI_DEVICE_11,
  DALDEVICEID_SPI_DEVICE_12
};

/**
 * @brief Initializes and configures a communication port.
 *
 * @param[out] handle  Opaque I/O port handle assigned by this API. This handle
 *                     is given to drivers as part of initialization. Drivers
 *                     will pass this handle as a parameter to read/write API
 *                     calls to the port.
 * @param[in]  config  Configuration data for the port.
 *
 * @return SNS_DDF_SUCCESS if the open operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_open_port(
    sns_ddf_handle_t*       handle,
    sns_ddf_port_config_s*  config)
{

  
  /* Sanity check. */
  if (config == NULL)
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  *handle = NULL;
  switch(config->bus)
  {
    case SNS_DDF_BUS_I2C:
      if (EnableI2C == TRUE)
      {
        sns_ddf_status_e         ddf_result;
        DALResult                dal_result;
        sns_ddf_sensor_info_s   *ddf_handle = NULL;

        /* check bus instance number */
        if (config->bus_instance >= ARR_SIZE(bus_instances) )
        {
          return SNS_DDF_EINVALID_PARAM;
        }

        /* allocate memory */
        ddf_result = sns_ddf_malloc((void **)&ddf_handle, sizeof(sns_ddf_sensor_info_s));

        if (SNS_DDF_SUCCESS != ddf_result)
        {
          SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                    "Malloc fail, requested size = %d",
                                    sizeof(sns_ddf_sensor_info_s));
          return ddf_result;
        }

        /* Initialize member params */
        ddf_handle->bus = SNS_DDF_BUS_I2C;
        ddf_handle->i2c_s.slave_addr = config->bus_config.i2c->slave_addr;
        ddf_handle->i2c_s.reg_addr_type = config->bus_config.i2c->reg_addr_type;

        /* Obtain the handle for the port. */
        dal_result = DAL_I2CDeviceAttach(bus_instances[config->bus_instance], &ddf_handle->i2c_s.hI2c);

        if (DAL_SUCCESS != dal_result)
        {
          SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                    "DAL_I2CDeviceAttach fail, dal_result = %d",
                                    dal_result);
          sns_ddf_mfree(ddf_handle);
          return SNS_DDF_EBUS;
        }

        *handle = (sns_ddf_handle_t)ddf_handle;

      }
      break;
    case SNS_DDF_BUS_SPI:
      if(EnableSPI)
      {
        sns_ddf_status_e       ddf_result;
        DALResult              dal_result;
        sns_ddf_sensor_info_s *ddf_handle = NULL;
        sns_ddf_spi_config_s  *spi_config = (sns_ddf_spi_config_s *) config->bus_config.spi;
        
        /* Sanity check. */
        if (config == NULL)
        {
          return SNS_DDF_EINVALID_PARAM;
        }
        
        /* check bus instance number */
        if (config->bus_instance > 12)
        {
          return SNS_DDF_EINVALID_PARAM;
        }

        /* allocate memory */
        ddf_result = sns_ddf_malloc((void **)&ddf_handle, sizeof(sns_ddf_sensor_info_s));
        if (SNS_DDF_SUCCESS != ddf_result)
        {
          SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                    "Malloc fail, requested size = %d",
                                    sizeof(sns_ddf_sensor_info_s));
          return ddf_result;
        }
        
        /* Initialize member params */
        ddf_handle->bus = SNS_DDF_BUS_SPI;
        ddf_handle->spi_s.slave_select = config->bus_config.spi->deviceBoardInfo.nSlaveNumber;
        ddf_handle->spi_s.transferMode = config->bus_config.spi->transferParameters.eTransferMode;
        
        
        /* Obtain the handle for the port. */
        dal_result = DAL_SpiDeviceAttach(spi_bus_instances[config->bus_instance],
                                         &ddf_handle->spi_s.hSPI);
        
        if (DAL_SUCCESS != dal_result)
        {
          SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                    "DAL_I2CDeviceAttach fail, dal_result = %d",
                                    dal_result);
          sns_ddf_mfree(ddf_handle);
          return SNS_DDF_EBUS;
        }
        
        
        *handle = (sns_ddf_handle_t)ddf_handle;
        
        /* Open SPI port*/
        
        if( DAL_SUCCESS != DalDevice_Open(ddf_handle->spi_s.hSPI,
                                          DAL_OPEN_SHARED) )
        {
          SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                    "DalDevice_Open fail, dal_result = %d",
                                    dal_result);          
          DAL_DeviceDetach(ddf_handle->spi_s.hSPI);
          sns_ddf_mfree(ddf_handle);
          return SNS_DDF_EBUS;
        }
        
        /* Configure SPI port*/
        dal_result = DalSpi_ConfigureDevice(ddf_handle->spi_s.hSPI, (SpiDeviceInfoType *) spi_config);

        if(DAL_SUCCESS != dal_result)
        {
          SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                    "DalSpi_ConfigureDevice fail, dal_result = %d",
                                    dal_result);
          sns_ddf_mfree(ddf_handle);
          return SNS_DDF_EBUS;
        }

        /* Close device - this only turns the clocks off */
        DalDevice_Close(ddf_handle->spi_s.hSPI);
	
      }
      else
      {
        return SNS_DDF_EINVALID_PARAM;
      }
      break;

    default:
      return SNS_DDF_EINVALID_PARAM;
  }
  return SNS_DDF_SUCCESS;
}


/**
 * @brief Closes a previously opened port.
 *
 * @param[out] handle  I/O port handle.
 *
 * @return SNS_DDF_SUCCESS if the open operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_close_port(sns_ddf_handle_t  handle)
{
  if ((EnableI2C == TRUE) &&
      (((sns_ddf_sensor_info_s *)handle)->bus == SNS_DDF_BUS_I2C))
  {
    sns_ddf_sensor_info_s *ddf_handle = (sns_ddf_sensor_info_s *)handle;

    DalDevice_Detach(ddf_handle->i2c_s.hI2c);
  }
  else if ((EnableSPI) &&
           (((sns_ddf_sensor_info_s *)handle)->bus == SNS_DDF_BUS_SPI))
  {
    sns_ddf_sensor_info_s *ddf_handle = (sns_ddf_sensor_info_s *)handle;

    DalDevice_Detach(ddf_handle->spi_s.hSPI);

  }
  sns_ddf_mfree(handle);

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Performs a bus read operation for up to 2^32 - 1 bytes 
 *        at a time.
 *
 * @param[in]  handle      The I/O port handle that identifies a device. This is
 *                         given to drivers as part of initialization.
 * @param[in]  reg_addr    Address of the register to read on the device.
 * @param[out] buffer      Location where result will be stored. Note size of
 *                         this buffer must >= @a bytes.
 * @param[in]  bytes       Number of bytes to read.
 * @param[out] read_count  Number of bytes actually read into the buffer.
 *
 * @return SNS_DDF_SUCCESS if the open operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
 sns_ddf_status_e sns_ddf_read_port_u32_count(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint32_t          bytes,
    uint32_t*         read_count)
{
  
  /* valid handle ? */
  if (handle == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "handle is NULL");
    return SNS_DDF_EINVALID_PARAM;    
  }

  if ((EnableI2C == TRUE) &&
      (((sns_ddf_sensor_info_s *)handle)->bus == SNS_DDF_BUS_I2C))
  {
    DALResult           dal_result;
    I2cClientConfig     clntCfg = {400, 2500};
    I2cTransfer         transfer[2];
    I2cBuffDesc         iovec[2];
    I2cSequence         rdSeq;
    I2cIoResult         ioRes;
    sns_ddf_sensor_info_s *ddf_handle = (sns_ddf_sensor_info_s *) handle;
    uint8_t             reg_addr8[2];

    /* valid handle ? */
    if (ddf_handle == NULL)
    {
      SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "ddf_handle is NULL");
      return SNS_DDF_EINVALID_PARAM;    
    }
    
    sns_profiling_log_qdss(SNS_SMGR_DD_BUS_READ_ENTER, 0);
    /* open I2C device */
    dal_result = DalDevice_Open(ddf_handle->i2c_s.hI2c, DAL_OPEN_SHARED );
    if (DAL_SUCCESS != dal_result)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "DalDevice_Open fail, dal_result = %d",
                                dal_result);
      return SNS_DDF_EBUS;
    }

    /* initialise read count */
    *read_count = 0;
    reg_addr8[0] = (reg_addr & 0xFF);
    reg_addr8[1] = ((reg_addr >> 8) & 0xFF);

    /*
     * Setup Batch transfer Step 1 (write slave address and reg address)
     */

    iovec[0].pBuff = reg_addr8;
    iovec[0].uBuffSize = 1;
    if (ddf_handle->i2c_s.reg_addr_type == SNS_DDF_I2C_REG_ADDR_16BIT)
    {
      iovec[0].uBuffSize = 2;
    }

    transfer[0].pI2cBuffDesc = &iovec[0];
    transfer[0].eTranDirection = I2cTranDirOut;
    transfer[0].uTrSize = 1;
    transfer[0].tranCfg.uSlaveAddr = ddf_handle->i2c_s.slave_addr;
    transfer[0].eTranCtxt = I2cTrCtxSeqStart;

    /*
     * Setup Batch transfer Step 2 (multiple byte read)
     */

    iovec[1].pBuff = buffer;
    iovec[1].uBuffSize = bytes;

    transfer[1].pI2cBuffDesc = &iovec[1];
    transfer[1].eTranDirection = I2cTranDirIn;
    transfer[1].uTrSize = iovec[1].uBuffSize;
    transfer[1].tranCfg.uSlaveAddr = ddf_handle->i2c_s.slave_addr;
    transfer[1].eTranCtxt = I2cTrCtxSeqEnd;

    rdSeq.pTransfer = transfer;
    rdSeq.uNumTransfers = 2;

    /* Issue Batch Transfer */
    dal_result = DalI2C_BatchTransfer(ddf_handle->i2c_s.hI2c, &rdSeq, &clntCfg ,&ioRes);

    if ((dal_result != DAL_SUCCESS) && (ioRes.uInByteCnt != iovec[1].uBuffSize))
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_BatchTransfer fail, dal_result = %d",
                                dal_result);

      DalDevice_Close(ddf_handle->i2c_s.hI2c);
      sns_profiling_log_qdss(SNS_SMGR_DD_BUS_READ_EXIT, 1, dal_result);
      return SNS_DDF_EBUS;
    }

    *read_count = iovec[1].uBuffSize;

    /* close I2C device */
    DalDevice_Close(ddf_handle->i2c_s.hI2c);
    sns_profiling_log_qdss(SNS_SMGR_DD_BUS_READ_EXIT, 1, SNS_DDF_SUCCESS);
  }
  else if (((sns_ddf_sensor_info_s *)handle)->bus == SNS_DDF_BUS_SPI)
  {
    sns_profiling_log_qdss(SNS_SMGR_DD_BUS_READ_EXIT, 1, SNS_DDF_EINVALID_PARAM);
    return SNS_DDF_EINVALID_PARAM;
  }
  else
  {
    *read_count = bytes;
  }

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Performs a bus read operation.
 *
 * @param[in]  handle      The I/O port handle that identifies a device. This is
 *                         given to drivers as part of initialization.
 * @param[in]  reg_addr    Address of the register to read on the device.
 * @param[out] buffer      Location where result will be stored. Note size of
 *                         this buffer must >= @a bytes.
 * @param[in]  bytes       Number of bytes to read.
 * @param[out] read_count  Number of bytes actually read into the buffer.
 *
 * @return SNS_DDF_SUCCESS if the open operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
 sns_ddf_status_e sns_ddf_read_port(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          read_count)
{
  sns_ddf_status_e status;
  uint32 bytes_read = 0;
  status = sns_ddf_read_port_u32_count(handle, reg_addr, buffer, bytes, &bytes_read);
  *read_count = (uint8_t)bytes_read;
  return status;
}


/**
 * @brief Performs a bus read operation of multiple bytes using 
 *        repeated start
 *
 * @param[in]  handle      The I/O port handle that identifies a device. This is
 *                         given to drivers as part of initialization.
 * @param[in]  reg_addr    Address of the register to read on the device.
 * @param[out] buffer      Location where result will be stored. Note size of
 *                         this buffer must >= @a bytes.
 * @param[in]  bytes       Number of bytes to read.
 * @param[out] read_count  Number of bytes actually read into the buffer.
 *
 * @return SNS_DDF_SUCCESS if the open operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
 sns_ddf_status_e sns_ddf_read_port_sr(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          read_count)
{

  /* valid handle ? */
  if (handle == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "handle is NULL");
    return SNS_DDF_EINVALID_PARAM;    
  }

  if ((EnableI2C == TRUE) &&
      (((sns_ddf_sensor_info_s *)handle)->bus == SNS_DDF_BUS_I2C))
  {
    sns_ddf_status_e    ddf_result;
    DALResult           dal_result;
    I2cClientConfig     clntCfg = {400, 2500};
    I2cTransfer*        transfer;
    I2cBuffDesc*        iovec;
    I2cSequence         rdSeq;
    I2cIoResult         ioRes;
    sns_ddf_sensor_info_s *ddf_handle = (sns_ddf_sensor_info_s *) handle;
    uint8_t             i;
    uint8_t             reg_addr8[2];

    /* valid handle ? */
  if ((sns_ddf_sensor_info_s *)handle  == NULL)
    {
      SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "ddf_handle is NULL");
      return SNS_DDF_EINVALID_PARAM;    
    }

    /* open I2C device */
    dal_result = DalDevice_Open(ddf_handle->i2c_s.hI2c, DAL_OPEN_SHARED );
    if (DAL_SUCCESS != dal_result)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "DalDevice_Open fail, dal_result = %d",
                                dal_result);
      return SNS_DDF_EBUS;
    }

    /* allocate memory */
    ddf_result = sns_ddf_malloc((void **)&transfer, sizeof(I2cTransfer)*2*bytes);
    if (SNS_DDF_SUCCESS != ddf_result)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "Malloc fail, requested size = %d",
                                sizeof(I2cTransfer)*2*bytes);

      return ddf_result;
    }

    /* allocate memory */
    ddf_result = sns_ddf_malloc((void **)&iovec, sizeof(I2cBuffDesc)*2*bytes);
    if (SNS_DDF_SUCCESS != ddf_result)
    {
      sns_ddf_mfree(transfer);
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "Malloc fail, requested size = %d",
                                sizeof(I2cBuffDesc)*2*bytes);

      return ddf_result;
    }

    *read_count = 0;
    reg_addr8[0] = (reg_addr & 0xFF);
    reg_addr8[1] = ((reg_addr >> 8) & 0xFF);

    for (i=0; i<2*bytes; i+=2)
    {
      /*
       * Setup Batch transfer Step 1 (write slave address and reg address)
       */

      iovec[i].pBuff = reg_addr8; 
      iovec[i].uBuffSize = 1;
      if (ddf_handle->i2c_s.reg_addr_type == SNS_DDF_I2C_REG_ADDR_16BIT)
      {
        iovec[i].uBuffSize = 2;
      }

      transfer[i].pI2cBuffDesc = &iovec[i];
      transfer[i].eTranDirection = I2cTranDirOut;
      transfer[i].uTrSize = 1;
      transfer[i].tranCfg.uSlaveAddr = ddf_handle->i2c_s.slave_addr;
      transfer[i].eTranCtxt = ((i==0)?I2cTrCtxSeqStart:I2cTrCtxSeqContinue);

      /* 
       * Setup Batch transfer Step 2 (write slave address and read from reg address)
       */

      iovec[i+1].pBuff = &buffer[i/2];
      iovec[i+1].uBuffSize = 1;

      transfer[i+1].pI2cBuffDesc = &iovec[i+1];
      transfer[i+1].eTranDirection = I2cTranDirIn;
      transfer[i+1].uTrSize = iovec[1].uBuffSize;
      transfer[i+1].tranCfg.uSlaveAddr = ddf_handle->i2c_s.slave_addr;
      transfer[i+1].eTranCtxt = (((i+2)==2*bytes)?I2cTrCtxSeqEnd:I2cTrCtxSeqContinue);

    }
    rdSeq.pTransfer = transfer;
    rdSeq.uNumTransfers = 2*bytes;

    /* Issue Batch Transfer */
    dal_result = DalI2C_BatchTransfer(ddf_handle->i2c_s.hI2c, &rdSeq, &clntCfg ,&ioRes);
    if (dal_result != DAL_SUCCESS)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_BatchTransfer fail, dal_result = %d",
                                dal_result);
      DalDevice_Close(ddf_handle->i2c_s.hI2c);
      return SNS_DDF_EBUS;
    }

    *read_count = bytes;

    /* close I2C device */
    DalDevice_Close(ddf_handle->i2c_s.hI2c);
  }
  else
  {
    *read_count = bytes;
  }

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Performs a bus write operation.
 *
 * @param[in]  handle       The I/O port handle that identifies a device. This
 *                          is given to drivers as part of initialization.
 * @param[in]  reg_addr     Address of the register to write on the device.
 * @param[out] buffer       Location where value to be written is stored. Note
 *                          that size of this buffer must >= @a bytes.
 * @param[in]  bytes        Number of bytes to write.
 * @param[out] write_count  Number of bytes actually written from the buffer.
 *
 * @return SNS_DDF_SUCCESS if the open operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_write_port(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          write_count)
{
  /* valid handle ? */
  if (handle == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "handle is NULL");
    return SNS_DDF_EINVALID_PARAM;    
  }

  if ((EnableI2C == TRUE) &&
      (((sns_ddf_sensor_info_s *)handle)->bus == SNS_DDF_BUS_I2C))
  {
    DALResult           dal_result;
    I2cTransfer         transfer;
    I2cClientConfig     clntCfg = {400, 2500};
    I2cBuffDesc         iovec[2];
    sns_ddf_sensor_info_s *ddf_handle = (sns_ddf_sensor_info_s *) handle;
    uint32_t            iocount=0;
    uint8_t             reg_addr8[2];

    /* valid handle ? */
    if (ddf_handle == NULL)
    {
      SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "ddf_handle is NULL");
      return SNS_DDF_EINVALID_PARAM;    
    }

    /* open device */
    dal_result = DalDevice_Open(ddf_handle->i2c_s.hI2c, DAL_OPEN_SHARED );
    if (DAL_SUCCESS != dal_result)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "DalDevice_Open fail, dal_result = %d",
                                dal_result);
      return SNS_DDF_EBUS;
    }

    /* initialise written count */
    *write_count = 0;
    reg_addr8[0] = (reg_addr & 0xFF);
    reg_addr8[1] = ((reg_addr >> 8) & 0xFF);

    iovec[0].pBuff = reg_addr8;
    iovec[0].uBuffSize = 1;
    if (ddf_handle->i2c_s.reg_addr_type == SNS_DDF_I2C_REG_ADDR_16BIT)
    {
      iovec[0].uBuffSize = 2;
    }

    iovec[1].uBuffSize = 0;
    if ( bytes > 0)
    {
      iovec[1].pBuff = buffer;
      iovec[1].uBuffSize = bytes;
    }

    transfer.pI2cBuffDesc = iovec;
    transfer.eTranDirection = I2cTranDirOut;
    transfer.uTrSize = iovec[0].uBuffSize + iovec[1].uBuffSize;
    transfer.tranCfg.uSlaveAddr = ddf_handle->i2c_s.slave_addr;
    transfer.eTranCtxt = I2cTrCtxNotASequence;

    /* issue I2C write command */
    dal_result = DalI2C_Write(ddf_handle->i2c_s.hI2c, &transfer, &clntCfg, &iocount);

    if (dal_result != DAL_SUCCESS)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_Write fail, dal_result = %d",
                                dal_result);

      DalDevice_Close(ddf_handle->i2c_s.hI2c);
      return SNS_DDF_EBUS;
    }

    *write_count = bytes;

    /* close I2C device */
    DalDevice_Close(ddf_handle->i2c_s.hI2c);

  }
  else
  {
    *write_count = bytes;
  }

  return SNS_DDF_SUCCESS;
}


/**
*  @brief Performs SPI data transfer
*
*
* @param[in]   handle        The I/O port handle that identifies a device. This
*                            is given to drivers as part of initialization.
* @param[out]  read_buffer   Pointer to the buffer to which data read is deposited to
* @param[in]   read_len      Maximum number of bytes that can be stored in read_buffer
* @param[in]   write_buffer  Pointer to buffer that contains the data to be written
* @param[in]   write_len     Maximum number of bytes that can be stored in write_len
* @param[out]  byte_count    Number of SPI transfers that were be performed
*
*/
sns_ddf_sensor_e sns_ddf_spi_data_transfer(
          sns_ddf_handle_t handle,
          uint8_t *  read_buffer,
          uint32_t   read_len,
          uint8_t *  write_buffer,
          uint32_t   write_len,
          uint32_t * read_count)
{
  DALSYSMemInfo writeMemInfo;
  DALSYSMemInfo readMemInfo;

  SpiDataAddrType read;
  SpiDataAddrType write;

  DALSYSMemHandle hWriteMem;
  DALSYSMemHandle hReadMem;
  uint8 * pWriteBuf;
  uint8 * pReadBuf;

  /* valid handle ? */
  if (handle == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "handle is NULL");
    return SNS_DDF_EINVALID_PARAM;    
  }

  if((EnableSPI) &&
     (((sns_ddf_sensor_info_s *) handle)->bus == SNS_DDF_BUS_SPI))
  {
    DALResult              dal_result;
    sns_ddf_sensor_info_s *ddf_handle = (sns_ddf_sensor_info_s *) handle;


    /* check valid parameters */
    if ( ddf_handle == NULL ||
         read_buffer == NULL ||
         write_buffer == NULL ||
         read_len == 0 ||
         write_len == 0 )
    {
      return SNS_DDF_EINVALID_PARAM;    
    }

    /* Alloc uncached write buffer */
    if( DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                              DALSYS_MEM_ADDR_NOT_SPECIFIED, // dont specify virt
                              DALSYS_MEM_ADDR_NOT_SPECIFIED, // dont specify phys
                              write_len*sizeof(uint8),
                              &hWriteMem,                 // returned handle
                              NULL                        // no alloc for this object
                              ) != DAL_SUCCESS )
    {
      return SNS_DDF_EINVALID_PARAM;
    }

    DALSYS_MemInfo(hWriteMem, &writeMemInfo);

    /* Set up SPI tx param: addr type */
    write.virtualAddr = ( void * )writeMemInfo.VirtualAddr;
    write.physicalAddr = ( void * )writeMemInfo.PhysicalAddr;

    /* Alloc uncached read buffer */
    if( DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                              DALSYS_MEM_ADDR_NOT_SPECIFIED,
                              DALSYS_MEM_ADDR_NOT_SPECIFIED, // dont specify phys
                              read_len*sizeof(uint8),
                              &hReadMem,                 // returned handle
                              NULL                        // no alloc for this object
                              ) != DAL_SUCCESS )
    {
      return SNS_DDF_EINVALID_PARAM;
    }

    DALSYS_MemInfo(hReadMem, &readMemInfo);

    /* Set up SPI tx param: addr type */
    read.virtualAddr = ( void * )readMemInfo.VirtualAddr;
    read.physicalAddr = ( void * )readMemInfo.PhysicalAddr;

	
    pWriteBuf = (uint8 *) writeMemInfo.VirtualAddr;
    SNS_OS_MEMSCPY(pWriteBuf, write_len, write_buffer, write_len);
    DalDevice_Open(ddf_handle->spi_s.hSPI,DAL_OPEN_SHARED);

    /* SPI bi-directional data transfer */
    dal_result = DalSpi_SendAndReceive(ddf_handle->spi_s.hSPI,             // Handle
                                       (SpiDataAddrType *) &write,           // Write buffer
                                       write_len,                           // Write length
                                       DDF_COMM_MAX(read_len, write_len),   // Number of 8 bit transactions
                                       (SpiDataAddrType *) &read,            // Read buffer
                                       read_len,                            // Read length
                                       (int *) read_count);                 // Number of bytes read from SPI bus


    DalDevice_Close(ddf_handle->spi_s.hSPI);
	
    pReadBuf = (uint8 *) readMemInfo.VirtualAddr;
    SNS_OS_MEMSCPY(read_buffer, read_len, pReadBuf, read_len);
	
    DALSYS_DestroyObject(hReadMem);
    DALSYS_DestroyObject(hWriteMem);

    if (dal_result != SPI_COMPLETE)
    {
      return SNS_DDF_EBUS;
    }

    return SNS_DDF_SUCCESS;
  }
  else
  {
    return SNS_DDF_EINVALID_PARAM;
  }
}


void sns_ddf_write_port_byte(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t           buffer,
    sns_ddf_status_e* status)
{
    uint8_t write_count;
    uint8_t buff = buffer;

    if( status == NULL )
    {
        return;
    }
    *status = sns_ddf_write_port(handle, reg_addr, &buff, 1, &write_count);

    if(*status != SNS_DDF_SUCCESS)
        return;

    if(write_count != 1)
        *status = SNS_DDF_EBUS;
}


/**
 * @brief Get the port identifier from the port configuration
 *
 * @param[in]  port_config_ptr  The pointer to the port configuration
 * @param[out] port_id_ptr      The pointer in which the port id is saved
 *
 * @return none
 */
void sns_ddf_get_port_id(
const   sns_ddf_port_config_s*  port_config_ptr,
        sns_ddf_port_id_s*      port_id_ptr)
{
  if(port_config_ptr != NULL && port_id_ptr != NULL)
  {
    port_id_ptr->bus = port_config_ptr->bus;
    port_id_ptr->bus_inst = port_config_ptr->bus_instance;
    port_id_ptr->dev_addr = port_config_ptr->bus_config.i2c->slave_addr;
  }
}


/**
 * @brief Determine if the port configuration and the port id are referring to
 *        the same port.
 *
 * @param[in]  port_config_ptr  The pointer to the port configuration
 * @param[in]  port_id_ptr      The pointer in which the port id is saved
 *
 * @return True if the port id of the port configuration is the same as the input port id
 */
bool sns_ddf_is_same_port_id(
const   sns_ddf_port_config_s*  port_config_ptr,
const   sns_ddf_port_id_s*      port_id_ptr)
{
  if ( port_config_ptr != NULL && port_id_ptr != NULL &&
       port_id_ptr->bus == port_config_ptr->bus &&
       port_id_ptr->bus_inst == port_config_ptr->bus_instance &&
       port_id_ptr->dev_addr == port_config_ptr->bus_config.i2c->slave_addr )
  {
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * @brief Set address type of device
 *
 * @param[in/out]  port_config_ptr  The pointer to the port configuration
 * @param[in]  addr_type      The address type to be set
 *
 * @param[out] status       SNS_DDF_SUCCESS if the setting bus address type
 *  	   operation was done successfully. Otherwise
 *  	   SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate
 *  	   an error has occurred
 */
sns_ddf_status_e sns_ddf_set_i2c_port_addr_type (
   sns_ddf_port_config_s*     port_config_ptr,
   sns_ddf_port_addr_type_e   addr_type  )
{
  if(port_config_ptr != NULL)
  {
    port_config_ptr->bus_config.i2c->addr_type = addr_type;
    return SNS_DDF_SUCCESS;
  }

  return SNS_DDF_EFAIL;
}

/**
 * @brief Performs an I2C bus read operation with data sequence:
 *	[S] [Addr+R] [ACK] [Device_data_MSB] [ACK] .... [Device_data_LSB] [NACK] [P]
 *
 * @param[in]  handle      The I/O port handle that identifies a device. This is 
 *                         given to drivers as part of initialization.
 * @param[out] buffer      Location where result will be stored. Note size of 
 *                         this buffer must >= @a bytes.
 * @param[in]  bytes       Number of bytes to read.
 * @param[out] read_count  Number of bytes actually read into the buffer.
 * 
 * @return SNS_DDF_SUCCESS if the open operation was done successfully. 
 *         Otherwise 
 *			SNS_DDF_EBUS - Failure in DAL device open or read
 *			SNS_DDF_EINVALID_PARAM - NULL input params
 **/
sns_ddf_status_e sns_ddf_simple_read_port(
    sns_ddf_handle_t  handle,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          read_count)
{
	DALResult           dal_result;
	I2cClientConfig     clntCfg = {400, 2500};
	I2cTransfer         transfer;
	I2cBuffDesc         iovec;
	sns_ddf_sensor_info_s *ddf_handle = (sns_ddf_sensor_info_s *) handle;
	sns_ddf_status_e	status = SNS_DDF_SUCCESS;
	uint32_t			data_bytes = 0;
	
	/* Sanity checks */
	if( NULL == ddf_handle || NULL == buffer || NULL == read_count )
	{
		SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "Input param is NULL");		
		return SNS_DDF_EINVALID_PARAM;
	}

	if ( TRUE == EnableI2C && ddf_handle->bus == SNS_DDF_BUS_I2C )
	{
		/* Open I2C device */
		dal_result = DalDevice_Open( ddf_handle->i2c_s.hI2c, DAL_OPEN_SHARED );
		
		if ( DAL_SUCCESS != dal_result )
		{
			SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
									"DalDevice_Open fail, dal_result = %d",
									dal_result);
			return SNS_DDF_EBUS;
		}

		/* Init read count */
		*read_count = 0;

		/* Populate the IO Vector to point to the data buffer */
		iovec.pBuff = buffer;
		iovec.uBuffSize = bytes;
		
		/* Populate transfer desciptor with transfer information */
		transfer.pI2cBuffDesc = &iovec;
		transfer.eTranDirection = I2cTranDirIn;
		transfer.uTrSize = iovec.uBuffSize;
		transfer.tranCfg.uSlaveAddr = ddf_handle->i2c_s.slave_addr;
		transfer.eTranCtxt = I2cTrCtxNotASequence;

		/* Start a I2C read transfer */
		dal_result = DalI2C_Read(ddf_handle->i2c_s.hI2c, &transfer, &clntCfg, &data_bytes);
	    
		if ( DAL_SUCCESS != dal_result )
		{
			SNS_PRINTF_STRING_ERROR_2(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_Read fail, dal_result = %d, read_count = %d",
                                dal_result, data_bytes);

			status = SNS_DDF_EBUS;
		}

		*read_count = data_bytes;

		/* Close I2C device */
		DalDevice_Close(ddf_handle->i2c_s.hI2c);
	}
      else if ( ddf_handle->bus == SNS_DDF_BUS_I2C )
      {
        return SNS_DDF_EINVALID_PARAM;
      }
	else
	{
		*read_count = bytes;
	}

	return status;
}

/**
 * @brief Performs a bus write operation with data sequence:
 *  [S] [Addr+W] [ACK] [Master_data_MSB] [ACK] .... [Master_data_LSB] [ACK] [P]
 *
 * @param[in]  handle       The I/O port handle that identifies a device. This 
 *                          is given to drivers as part of initialization.
 * @param[out] buffer       Location where value to be written is stored. Note 
 *                          that size of this buffer must >= @a bytes.
 * @param[in]  bytes        Number of bytes to write.
 * @param[out] write_count  Number of bytes actually written from the buffer.
 * 
 * @return SNS_DDF_SUCCESS if the write operation was done 
 *  	   successfully. Otherwise 
 *				SNS_DDF_EINVALID_PARAM - Input param is NULL
 *				SNS_DDF_EBUS - Failure in DAL device open or write
 **/
sns_ddf_status_e sns_ddf_simple_write_port(
    sns_ddf_handle_t  handle,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          write_count)
{
	DALResult           dal_result;
	I2cClientConfig     clntCfg = {400, 2500};
	I2cTransfer         transfer;
	I2cBuffDesc         iovec;
	sns_ddf_sensor_info_s *ddf_handle = (sns_ddf_sensor_info_s *) handle;
	sns_ddf_status_e	status = SNS_DDF_SUCCESS;
	uint32_t			data_bytes = 0;
	
	/* Sanity checks */
	if( (NULL == ddf_handle) || (NULL == buffer) || (NULL == write_count) )
	{
		SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "Input param is NULL");		
		return SNS_DDF_EINVALID_PARAM;
	}

	if ( TRUE == EnableI2C && ddf_handle->bus == SNS_DDF_BUS_I2C )
	{
		/* Open I2C device */
		dal_result = DalDevice_Open(ddf_handle->i2c_s.hI2c, DAL_OPEN_SHARED);
		
		if ( DAL_SUCCESS != dal_result )
		{
			SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
									"DalDevice_Open fail, dal_result = %d",
									dal_result);
			return SNS_DDF_EBUS;
		}

		/* Init write count */
		*write_count = 0;

		/* Populate the IO Vector to point to the data buffer */
		iovec.pBuff = buffer;
		iovec.uBuffSize = bytes;
		
		/* Populate transfer desciptor with transfer information */
		transfer.pI2cBuffDesc = &iovec;
		transfer.eTranDirection = I2cTranDirOut;
		transfer.uTrSize = iovec.uBuffSize;
		transfer.tranCfg.uSlaveAddr = ddf_handle->i2c_s.slave_addr;
		transfer.eTranCtxt = I2cTrCtxNotASequence;

		/* Start a I2C read transfer */
		dal_result = DalI2C_Write( ddf_handle->i2c_s.hI2c, &transfer, &clntCfg, &data_bytes );
	    
		if ( DAL_SUCCESS != dal_result )
		{
			SNS_PRINTF_STRING_ERROR_2(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_Write fail, dal_result = %d, read_count = %d",
                                dal_result, data_bytes);

			status = SNS_DDF_EBUS;
		}

		*write_count = data_bytes;

		/* Close I2C device */
		DalDevice_Close( ddf_handle->i2c_s.hI2c );
	}
	else if ( ddf_handle->bus == SNS_DDF_BUS_SPI )
      {
          return SNS_DDF_EINVALID_PARAM;
      }
	else
	{
		*write_count = bytes;
	}

	return status;
}

#else
/*------------------------------------------------------------------------------
  Compiling on PCSIM...
------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "qurt_elite.h"
#include "adsp_error_codes.h"

sns_ddf_status_e sns_ddf_open_port(
    sns_ddf_handle_t*       handle,
    sns_ddf_port_config_s*  config)
{
   /*
      For now it is stubbed out.
      Later when we add driver-level playback support,it wil call
      the playback interface to open files,etc.
      This allows centralization of playback utilites, as well as the
      flexibility to control from user shell.
   */
   return SNS_DDF_SUCCESS;
}


sns_ddf_status_e sns_ddf_close_port(sns_ddf_handle_t  handle)
{
    fclose(handle);
    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e sns_ddf_read_port(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          read_count)
{
   /* future driver-level playback will be done through playback API */
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ddf_read_port");

    *read_count = bytes;
    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e sns_ddf_write_port(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          write_count)
{
    *write_count = bytes;
    return SNS_DDF_SUCCESS;
}
#endif
