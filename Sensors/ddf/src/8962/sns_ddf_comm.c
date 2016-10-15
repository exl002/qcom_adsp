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

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/ddf/src/8962/sns_ddf_comm.c#1 $
  $DateTime: 2014/05/16 10:56:56 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2013-12-23 pn   Added sns_ddf_read_port_u32_count()
  2013-09-26 ps	  pulled changes from ddf/8084 to meet common binary requirement with 8962
  2013-09-06 MW	  Added single transfer (simple) I2C read and write APIs.
  2013-05-01 lka  Added IO functions for enhanced DDF interface.
  2013-04-28 vh  DDF handle validation before calling read/write
  2013-03-18 vh  Added appropriate return errors
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

#if !defined (SNS_PCSIM)

#include <DALDeviceId.h>
#include <DALStdErr.h>
#include <DALStdDef.h>
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_ddf_util.h"
#include "sns_memmgr.h"
#include "DalDevice.h"
#include "I2cTransfer.h"
#include "I2cTransferCfg.h"
#include "ddii2c.h"
#include "sns_debug_str.h"

/*----------------------------------------------------------------------------
 * Structure Definitions
 * -------------------------------------------------------------------------*/
typedef struct
{
  DalDeviceHandle *hI2c;
  uint8 slave_addr;
  sns_ddf_i2c_reg_addr_e reg_addr_type;
}sns_ddf_sensor_dali2c_info_s;

/*----------------------------------------------------------------------------
 * Extern Variables
 * -------------------------------------------------------------------------*/
volatile boolean EnableI2C = true;

/*----------------------------------------------------------------------------
 * Local Global Variables
 * -------------------------------------------------------------------------*/
static DALDEVICEID bus_instances[] =
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
  if (EnableI2C == TRUE)
  {
    sns_ddf_status_e         ddf_result;
    DALResult                dal_result;
    sns_ddf_sensor_dali2c_info_s *ddf_handle = NULL;

    /* Sanity check. */
    if (config == NULL)
    {
      return SNS_DDF_EINVALID_PARAM;
    }

    /* Currently only supporting I2C. */
    if (config->bus != SNS_DDF_BUS_I2C)
    {
      return SNS_DDF_EINVALID_PARAM;
    }

    /* check bus instance number */
    if (config->bus_instance > 12)
    {
      return SNS_DDF_EINVALID_PARAM;
    }

    /* allocate memory */
    ddf_result = sns_ddf_malloc((void **)&ddf_handle, sizeof(sns_ddf_sensor_dali2c_info_s));

    if (SNS_DDF_SUCCESS != ddf_result)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "Malloc fail, requested size = %d",
                                sizeof(sns_ddf_sensor_dali2c_info_s));
      return ddf_result;
    }

    /* Initialize member params */
    ddf_handle->slave_addr = config->bus_config.i2c->slave_addr;
    ddf_handle->reg_addr_type = config->bus_config.i2c->reg_addr_type;

    /* Obtain the handle for the port. */
    dal_result = DAL_I2CDeviceAttach(bus_instances[config->bus_instance], &ddf_handle->hI2c);

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
  if (EnableI2C == TRUE)
  {
    sns_ddf_sensor_dali2c_info_s *ddf_handle = (sns_ddf_sensor_dali2c_info_s *)handle;

    DalDevice_Detach(ddf_handle->hI2c);
    sns_ddf_mfree(ddf_handle);
  }

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
  if (EnableI2C == TRUE)
  {
    DALResult           dal_result;
    I2cClientConfig     clntCfg = {400, 2500};
    I2cTransfer         transfer[2];
    I2cBuffDesc         iovec[2];
    I2cSequence         rdSeq;
    I2cIoResult         ioRes;
    sns_ddf_sensor_dali2c_info_s *ddf_handle = (sns_ddf_sensor_dali2c_info_s *) handle;
    uint8_t             reg_addr8[2];

    /* valid handle ? */
    if (ddf_handle == NULL)
    {
      SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "ddf_handle is NULL");
      return SNS_DDF_EINVALID_PARAM;    
    }
    
    /* open I2C device */
    dal_result = DalDevice_Open(ddf_handle->hI2c, DAL_OPEN_SHARED );
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
    if (ddf_handle->reg_addr_type == SNS_DDF_I2C_REG_ADDR_16BIT)
    {
      iovec[0].uBuffSize = 2;
    }

    transfer[0].pI2cBuffDesc = &iovec[0];
    transfer[0].eTranDirection = I2cTranDirOut;
    transfer[0].uTrSize = 1;
    transfer[0].tranCfg.uSlaveAddr = ddf_handle->slave_addr;
    transfer[0].eTranCtxt = I2cTrCtxSeqStart;

    /*
     * Setup Batch transfer Step 2 (multiple byte read)
     */

    iovec[1].pBuff = buffer;
    iovec[1].uBuffSize = bytes;

    transfer[1].pI2cBuffDesc = &iovec[1];
    transfer[1].eTranDirection = I2cTranDirIn;
    transfer[1].uTrSize = iovec[1].uBuffSize;
    transfer[1].tranCfg.uSlaveAddr = ddf_handle->slave_addr;
    transfer[1].eTranCtxt = I2cTrCtxSeqEnd;

    rdSeq.pTransfer = transfer;
    rdSeq.uNumTransfers = 2;

    /* Issue Batch Transfer */
    dal_result = DalI2C_BatchTransfer(ddf_handle->hI2c, &rdSeq, &clntCfg ,&ioRes);

    if ((dal_result != DAL_SUCCESS) && (ioRes.uInByteCnt != iovec[1].uBuffSize))
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_BatchTransfer fail, dal_result = %d",
                                dal_result);

      DalDevice_Close(ddf_handle->hI2c);
      return SNS_DDF_EBUS;
    }

    *read_count = iovec[1].uBuffSize;

    /* close I2C device */
    DalDevice_Close(ddf_handle->hI2c);
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

  if (EnableI2C == TRUE)
  {
    sns_ddf_status_e    ddf_result;
    DALResult           dal_result;
    I2cClientConfig     clntCfg = {400, 2500};
    I2cTransfer*        transfer;
    I2cBuffDesc*        iovec;
    I2cSequence         rdSeq;
    I2cIoResult         ioRes;
    sns_ddf_sensor_dali2c_info_s *ddf_handle = (sns_ddf_sensor_dali2c_info_s *) handle;
    uint8_t             i;
    uint8_t             reg_addr8[2];

    /* valid handle ? */
    if (ddf_handle == NULL)
    {
      SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "ddf_handle is NULL");
      return SNS_DDF_EINVALID_PARAM;    
    }

    /* open I2C device */
    dal_result = DalDevice_Open(ddf_handle->hI2c, DAL_OPEN_SHARED );
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
      if (ddf_handle->reg_addr_type == SNS_DDF_I2C_REG_ADDR_16BIT)
      {
        iovec[i].uBuffSize = 2;
      }

      transfer[i].pI2cBuffDesc = &iovec[i];
      transfer[i].eTranDirection = I2cTranDirOut;
      transfer[i].uTrSize = 1;
      transfer[i].tranCfg.uSlaveAddr = ddf_handle->slave_addr;
      transfer[i].eTranCtxt = ((i==0)?I2cTrCtxSeqStart:I2cTrCtxSeqContinue);

      /* 
       * Setup Batch transfer Step 2 (write slave address and read from reg address)
       */

      iovec[i+1].pBuff = &buffer[i/2];
      iovec[i+1].uBuffSize = 1;

      transfer[i+1].pI2cBuffDesc = &iovec[i+1];
      transfer[i+1].eTranDirection = I2cTranDirIn;
      transfer[i+1].uTrSize = iovec[1].uBuffSize;
      transfer[i+1].tranCfg.uSlaveAddr = ddf_handle->slave_addr;
      transfer[i+1].eTranCtxt = (((i+2)==2*bytes)?I2cTrCtxSeqEnd:I2cTrCtxSeqContinue);

    }
    rdSeq.pTransfer = transfer;
    rdSeq.uNumTransfers = 2*bytes;

    /* Issue Batch Transfer */
    dal_result = DalI2C_BatchTransfer(ddf_handle->hI2c, &rdSeq, &clntCfg ,&ioRes);
    if (dal_result != DAL_SUCCESS)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_BatchTransfer fail, dal_result = %d",
                                dal_result);
      DalDevice_Close(ddf_handle->hI2c);
      return SNS_DDF_EBUS;
    }

    *read_count = bytes;

    /* close I2C device */
    DalDevice_Close(ddf_handle->hI2c);
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
  if (EnableI2C == TRUE)
  {
    DALResult           dal_result;
    I2cTransfer         transfer;
    I2cClientConfig     clntCfg = {400, 2500};
    I2cBuffDesc         iovec[2];
    sns_ddf_sensor_dali2c_info_s *ddf_handle = (sns_ddf_sensor_dali2c_info_s *) handle;
    uint32_t            iocount=0;
    uint8_t             reg_addr8[2];

    /* valid handle ? */
    if (ddf_handle == NULL)
    {
      SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "ddf_handle is NULL");
      return SNS_DDF_EINVALID_PARAM;    
    }

    /* open device */
    dal_result = DalDevice_Open(ddf_handle->hI2c, DAL_OPEN_SHARED );
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
    if (ddf_handle->reg_addr_type == SNS_DDF_I2C_REG_ADDR_16BIT)
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
    transfer.tranCfg.uSlaveAddr = ddf_handle->slave_addr;
    transfer.eTranCtxt = I2cTrCtxNotASequence;

    /* issue I2C write command */
    dal_result = DalI2C_Write(ddf_handle->hI2c, &transfer, &clntCfg, &iocount);

    if (dal_result != DAL_SUCCESS)
    {
      SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_Write fail, dal_result = %d",
                                dal_result);

      DalDevice_Close(ddf_handle->hI2c);
      return SNS_DDF_EBUS;
    }

    *write_count = bytes;

    /* close I2C device */
    DalDevice_Close(ddf_handle->hI2c);

  }
  else
  {
    *write_count = bytes;
  }

  return SNS_DDF_SUCCESS;
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
 * @brief Performs a bus read operation with data sequence:
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
	sns_ddf_sensor_dali2c_info_s *ddf_handle = (sns_ddf_sensor_dali2c_info_s *) handle;
	sns_ddf_status_e	status = SNS_DDF_SUCCESS;
	uint32_t			data_bytes = 0;
	
	/* Sanity checks */
	if( NULL == ddf_handle || NULL == buffer || NULL == read_count )
	{
		SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "Input param is NULL");		
		return SNS_DDF_EINVALID_PARAM;
	}

	if ( TRUE == EnableI2C )
	{
		/* Open I2C device */
		dal_result = DalDevice_Open( ddf_handle->hI2c, DAL_OPEN_SHARED );
		
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
		transfer.tranCfg.uSlaveAddr = ddf_handle->slave_addr;
		transfer.eTranCtxt = I2cTrCtxNotASequence;

		/* Start a I2C read transfer */
		dal_result = DalI2C_Read(ddf_handle->hI2c, &transfer, &clntCfg, &data_bytes);
	    
		if ( DAL_SUCCESS != dal_result )
		{
			SNS_PRINTF_STRING_ERROR_2(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_Read fail, dal_result = %d, read_count = %d",
                                dal_result, data_bytes);

			status = SNS_DDF_EBUS;
		}

		*read_count = data_bytes;

		/* Close I2C device */
		DalDevice_Close(ddf_handle->hI2c);
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
	sns_ddf_sensor_dali2c_info_s *ddf_handle = (sns_ddf_sensor_dali2c_info_s *) handle;
	sns_ddf_status_e	status = SNS_DDF_SUCCESS;
	uint32_t			data_bytes = 0;
	
	/* Sanity checks */
	if( (NULL == ddf_handle) || (NULL == buffer) || (NULL == write_count) )
	{
		SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "Input param is NULL");		
		return SNS_DDF_EINVALID_PARAM;
	}

	if ( TRUE == EnableI2C )
	{
		/* Open I2C device */
		dal_result = DalDevice_Open(ddf_handle->hI2c, DAL_OPEN_SHARED);
		
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
		transfer.tranCfg.uSlaveAddr = ddf_handle->slave_addr;
		transfer.eTranCtxt = I2cTrCtxNotASequence;

		/* Start a I2C read transfer */
		dal_result = DalI2C_Write( ddf_handle->hI2c, &transfer, &clntCfg, &data_bytes );
	    
		if ( DAL_SUCCESS != dal_result )
		{
			SNS_PRINTF_STRING_ERROR_2(SNS_DBG_MOD_DSPS_DDF,
                                "DalI2C_Write fail, dal_result = %d, read_count = %d",
                                dal_result, data_bytes);

			status = SNS_DDF_EBUS;
		}

		*write_count = data_bytes;

		/* Close I2C device */
		DalDevice_Close( ddf_handle->hI2c );
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
