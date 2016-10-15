/** 
 * @file sns_ddf_comm.h 
 * @brief SDDF communication library responsible for read/write access to 
 *        devices.
 *  
 * Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All 
 * Rights Reserved. Qualcomm Technologies Proprietary and 
 * Confidential. 
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/ddf/inc/sns_ddf_comm.h#1 $
  $DateTime: 2014/05/16 10:56:56 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2014-01-30 lka  Added sns_ddf_read_port_u32_count()
  2014-01-22 dk   Added SPI transfer
  2013-07-05 MW	  Added single transfer (simple) I2C read and write APIs.
  2013-05-01 lka  Added/modified data types for DDF interface.
  2013-02-14 ag   - Added support for 16-bit register addressing
                  - I2C transfer optimizations
                  - support for multiple reads without STOP
  2012-07-12 sc   Added bus_instance in sns_ddf_port_config_s
  2012-05-31 sd   Edited comment
  2011-03-13 sd   Added sns_ddf_write_port_byte
  2011-08-30 yk   Updated comments to reflect proper error codes.
  2011-08-11 yk   Added SNS_DDF_I2C_DEVICE_DEFAULT as an I2C device type.
  2011-04-12 yk   Moved IRQ handling to sns_ddf_signal.
  2010-11-04 yk   Moved sns_ddf_irq_e here from sns_ddf_common.h.
  2010-08-19 yk   Added open_port() and close_port() along with DAL
                  configuration structures for I2C.
  2010-08-14 yk   Renamed identifiers to conform to DSPS naming convention.
  2010-07-30 yk   Initial revision
==============================================================================*/

#ifndef SNS_DDF_COMM_H
#define SNS_DDF_COMM_H

#include "sns_ddf_common.h"


/** Types of communication ports. */
typedef enum
{
    SNS_DDF_BUS_I2C,
    SNS_DDF_BUS_SPI
    /* Additional bus types will be added here. */

} sns_ddf_bus_e;


/** I2C address types. */
typedef enum
{
   SNS_DDF_I2C_ADDR_7BIT,
   SNS_DDF_I2C_ADDR_10BIT

} sns_ddf_port_addr_type_e;


/** I2C device types. */
typedef enum
{
   SNS_DDF_I2C_DEVICE_DEFAULT,
   SNS_DDF_I2C_DEVICE_MEMADDR,
   SNS_DDF_I2C_DEVICE_REGADDR

} sns_ddf_i2c_device_e;

/** I2C register address types. */
typedef enum
{
   SNS_DDF_I2C_REG_ADDR_8BIT,
   SNS_DDF_I2C_REG_ADDR_16BIT

} sns_ddf_i2c_reg_addr_e;

/** I2C read options. */
typedef enum
{
   SNS_DDF_I2C_START_BEFORE_RD = 1,
   SNS_DDF_I2C_STOP_START_BEFORE_RD

} sns_ddf_i2c_read_opt_e;


/** the identifier of a port id */
typedef struct
{
  sns_ddf_bus_e     bus;        /**< Type of bus being configured. */
  uint8_t           bus_inst;   /**< Bus instance ID */
  uint32_t          dev_addr;   /**< Device slave address.  */
} sns_ddf_port_id_s;

/** 
 * I2C configuration parameters. These are passed to the I2C driver when opening 
 * a port. @see sns_ddf_open_port().
 */ 
typedef struct
{
   uint32_t                slave_addr;      /**< Device slave address.  */
   uint32_t                bus_freq;        /**< Bus frequency in kHz.  */
   int32_t                 bus_acq_timeout; /**< Timeout in ms to acquire bus.
                                                 Use -1 for default. */
   int32_t                 xfer_timeout;    /**< Max transfer timeout. */
   sns_ddf_i2c_reg_addr_e  reg_addr_type;   /**< I2C register address type.
                                                 Drivers with 16 bit register addresses shall
                                                 set this to SNS_DDF_I2C_REG_ADDR_16BIT before
                                                 calling sns_ddf_open_port. */
   sns_ddf_port_addr_type_e addr_type;       /**< I2C address length. */
   sns_ddf_i2c_device_e     dev_type;        /**< I2C device type. */
   sns_ddf_i2c_read_opt_e  read_opt;        /**< I2C read option. */

} sns_ddf_i2c_config_s;


typedef struct SpiDeviceInfoType sns_ddf_spi_config_s;

/**
 * Pointers to bus configuration types.
 */
typedef union
{
    sns_ddf_i2c_config_s* i2c;
    sns_ddf_spi_config_s* spi;
    /* Additional bus types will go here... */

} sns_ddf_bus_config_ptr_u;


/** 
 * Port configuration parameters, used to open a port for read/write.
 */ 
typedef struct
{
    sns_ddf_bus_e             bus;         /**< Type of bus being configured. */
    sns_ddf_bus_config_ptr_u  bus_config;  /**< Pointer to config parameters.
                                                Type is determined by @a bus. */
    uint8_t                   bus_instance;/**< Bus instance ID */
} sns_ddf_port_config_s;


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
    sns_ddf_port_config_s*  config);


/**
 * @brief Closes a previously opened port. 
 *
 * @param[out] handle  I/O port handle.
 *
 * @return SNS_DDF_SUCCESS if the open operation was done successfully. 
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_close_port(sns_ddf_handle_t  handle);


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
    uint32_t*         read_count);

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
    uint8_t*          read_count);

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
    uint8_t*          read_count);

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
 * @return SNS_DDF_SUCCESS if the write operation was done 
 *  	   successfully. Otherwise SNS_DDF_EDEVICE,
 *  	   SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *  	   SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_write_port(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          write_count);

/**
 * @brief Performs a bus write operation. 
 *
 * @param[in]  handle       The I/O port handle that identifies a device. This 
 *                          is given to drivers as part of initialization.
 * @param[in]  reg_addr     Address of the register to write on the device.
 * @param[in]  value        value to be written 
 * @param[out] status       SNS_DDF_SUCCESS if the write 
 *  	   operation was done successfully. Otherwise
 *  	   SNS_DDF_EDEVICE, SNS_DDF_EBUS,
 *  	   SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate
 *  	   an error has occurred
 * 
 * @return none
 */
void sns_ddf_write_port_byte(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t           value,
    sns_ddf_status_e* status);

/**
*  @brief Performs SPI data transfer
*
*
* @param[in]   handle        The I/O port handle that identifies a device. This
*                            is given to drivers as part of initialization.
* @param[out]  read_buffer    Pointer to the buffer to which data read is deposited to
* @param[in]   read_len      Maximum number of bytes that can be stored in read_buffer
* @param[in]   write_buffer  Pointer to buffer that contains the data to be written
* @param[in]   write_len     Maximum number of bytes that can be stored in write_len
* @param[out]  byte_count  Number of SPI transfers that were be performed
*
*/
sns_ddf_sensor_e sns_ddf_spi_data_transfer(
          sns_ddf_handle_t handle,
          uint8_t *        read_buffer,
          uint32_t         read_len,
          uint8_t *        write_buffer,
          uint32_t         write_len,
          uint32_t*        byte_count);

/**
 * @brief Get the port indentifier from the port configuration
 *
 * @param[in]  port_config_ptr  The pointer to the port configuration
 * @param[out] port_id_ptr      The pointer in which the port id will be saved
 * 
 * @return none
 */
void sns_ddf_get_port_id(
const   sns_ddf_port_config_s*  port_config_ptr,
        sns_ddf_port_id_s*      port_id_ptr);


/**
 * @brief Get the port indentifier from the port configuration
 *
 * @param[in]  port_config_ptr  The pointer to the port configuration
 * @param[in]  port_id_ptr      The pointer in which the port id is saved
 * 
 * @return True if the port id of the port configuration is the same as the input port id
 */
bool sns_ddf_is_same_port_id(
const   sns_ddf_port_config_s*  port_config_ptr,
const   sns_ddf_port_id_s*      port_id_ptr);

/**
 * @brief Set address type of device
 *
 * @param[in/out]  port_config_ptr  The pointer to the port configuration
 * @param[in]  addr_type      The address type to be set
 *
 * @return status       SNS_DDF_SUCCESS if the setting bus address type
 *  	   operation was done successfully. Otherwise
 *  	   SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate
 *  	   an error has occurred
 */
sns_ddf_status_e sns_ddf_set_port_addr_type (
   sns_ddf_port_config_s*     port_config_ptr,
   sns_ddf_port_addr_type_e   addr_type  );

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
 *          Otherwise 
 *			SNS_DDF_EBUS - Failure in DAL device open or read
 *			SNS_DDF_EINVALID_PARAM - NULL input params
 */
sns_ddf_status_e sns_ddf_simple_read_port(
    sns_ddf_handle_t  handle,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          read_count);

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
 */
sns_ddf_status_e sns_ddf_simple_write_port(
    sns_ddf_handle_t  handle,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          write_count);
#endif
