/*  Date: 2014/05/09  20:10:00
 *  Revision: 2.3.4.5
 */


/*******************************************************************************
 * Copyright (c) 2014, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/


/*==============================================================================

  S E N S O R S    G Y R O S C O P E    D R I V E R

  DESCRIPTION

  Impelements the driver for the Gyroscope sensor

  EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.




  when         who     what, where, why

  03/03/2012 albert  set attr, init, get data,get attr,reset
  07/24/2012 albert  self test
  04/08/2013 albert  set sensor to suspend at reset v1.8
  04/19/2013 albert  support DRI  v1.9
  07/08/2013 albert  support bmg of bmi058 v2.0
  11/11/2013 lk      FIFO wml interrupt
  02/24/2014 lk      add an old way of ODR handling for the customer S
  03/14/2014 lk      add an old way of ODR handling and downsampling for the customer S
  03/14/2014 mw      merge the self test code fix
  03/14/2014 mw      merge the code fix for static analysis from KW
  03/14/2014 lk      merge the code from qcm and bst, update version information
  03/15/2014 lk      interval review by bst
  03/24/2014 lk      use dev_select to handle chips variations
  04/02/2014 lk      heavy testing performed with qcm
  04/11/2014 lk      add SNS_DDF_ATTRIB_RESOLUTION
  04/11/2014 lk      set low power mode to BMG160_MODE_DEEPSUSPEND, add code to detect BMI058 automatically
  04/17/2014 lk      set range to +/-2000 by default
  04/25/2014 lk      add config for fastest odr
  04/25/2014 MW      eliminate compiler warning
  05/04/2014 lk      fix the timestamp issue when wml is 0

  ----------   ---     -----------------------------------------------------------

  ==============================================================================*/

/*============================================================================

  INCLUDE FILES

  ============================================================================*/
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"

#include "sns_dd_inertial_test.h"


#include "sns_ddf_signal.h"

//Include files required to support gyro log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

#include "sns_dd_bmg160.h"


#define SENSOR_NAME "bmg160"


#define BMG160_DEV_SEL_NUM_BMI055 0x55
#define BMG160_DEV_SEL_NUM_BMI058 0x58

#define BMG160_CONFIG_FASTEST_ODR 250
#define BMG160_CONFIG_MAX_FREQ    BMG160_CONFIG_FASTEST_ODR

#define BMG160_CONFIG_TOLERANCE_DRYD_TIME_ACCURACY (0.2)


#define BMG160_CONFIG_WMI_TRIGGER_LEVEL  0
#define BMG160_CONFIG_DRI_MOCKUP_WMI     1

// Enable BMG160_CONFIG_BW_SEL_OLD_WAY to enable
// down-sampling in the driver
#define BMG160_CONFIG_BW_SEL_OLD_WAY     0
#define BMG160_CONFIG_FIFO_LOG           1

#define BMG160_FIFO_HEAD_ROOM            10

#define BMG160_IRQ_TRIGGER_TYPE_DEFAULT  SNS_DDF_SIGNAL_IRQ_RISING


#define SENSOR_CHIP_ID_BMG (0x0f)

#define BMG_REG_NAME(name) BMG160_##name
#define BMG_VAL_NAME(name) BMG160_##name
#define BMG_CALL_API(name) bmg160_##name

#define BMG160_ABS(x) (((x) >= 0) ? (x) : (-x))

#define BST_ARRARY_SIZE(array) (sizeof(array)/sizeof(array[0]))

struct bst_val_pair {
	uint32_t l;
	uint32_t r;
};


static const uint32_t  sns_dd_gyro_range[5] =
{
	BOSCH_GYRO_SENSOR_RANGE_125,
	BOSCH_GYRO_SENSOR_RANGE_250,
	BOSCH_GYRO_SENSOR_RANGE_500,
	BOSCH_GYRO_SENSOR_RANGE_1000,
	BOSCH_GYRO_SENSOR_RANGE_2000
};

static const uint32_t  sns_dd_gyro_bw[8] =
{
	BOSCH_GYRO_SENSOR_BW_12HZ,
	BOSCH_GYRO_SENSOR_BW_23HZ,
	BOSCH_GYRO_SENSOR_BW_32HZ,
	BOSCH_GYRO_SENSOR_BW_47HZ,
	BOSCH_GYRO_SENSOR_BW_64HZ,
	BOSCH_GYRO_SENSOR_BW_116HZ,
	BOSCH_GYRO_SENSOR_BW_230HZ,
	BOSCH_GYRO_SENSOR_BW_523HZ
};

static const struct bst_val_pair BMG160_MAP_BW_ODR[] = {
	{BOSCH_GYRO_SENSOR_BW_12HZ, 100},
	{BOSCH_GYRO_SENSOR_BW_23HZ, 200},
	{BOSCH_GYRO_SENSOR_BW_32HZ, 100},
	{BOSCH_GYRO_SENSOR_BW_47HZ, 400},
	{BOSCH_GYRO_SENSOR_BW_64HZ, 200},
	{BOSCH_GYRO_SENSOR_BW_116HZ, 1000},
	{BOSCH_GYRO_SENSOR_BW_230HZ, 2000},
	{BOSCH_GYRO_SENSOR_BW_523HZ, 2000}
};

static const uint32_t sns_dd_gyro_freq[8] =
{
	BOSCH_GYRO_SENSOR_BW_VALUE_12HZ,
	BOSCH_GYRO_SENSOR_BW_VALUE_23HZ,
	BOSCH_GYRO_SENSOR_BW_VALUE_32HZ,
	BOSCH_GYRO_SENSOR_BW_VALUE_47HZ,
	BOSCH_GYRO_SENSOR_BW_VALUE_64HZ,
	BOSCH_GYRO_SENSOR_BW_VALUE_116HZ,
	BOSCH_GYRO_SENSOR_BW_VALUE_230HZ,
	BOSCH_GYRO_SENSOR_BW_VALUE_523HZ
};

typedef enum {
	SDD_GYRO_X = 0,
	SDD_GYRO_Y,
	SDD_GYRO_Z,
	SDD_GYRO_NUM_AXES,
	SDD_GYRO_NUM_DATATYPES
} sdd_gyro_data_type_e;


typedef enum {
	BMG160_FIFO_MODE_BYPASS = 0,
	BMG160_FIFO_MODE_FIFO,
	BMG160_FIFO_MODE_STREAM,
	BMG160_FIFO_MODE_RESERVED
} BMG160_FIFO_MODE;

/* State struct for BMG16- driver */
typedef struct {
	sns_ddf_handle_t smgr_handle;    /* SDDI handle used to notify_data */
	q16_t            bias[SDD_GYRO_NUM_AXES];    /* Accelerometer bias */
	q16_t            data_cache[SDD_GYRO_NUM_DATATYPES]; /* data cache */
	uint8_t          out_of_bound_flag;
	sns_ddf_handle_t port_handle; /* handle used to access the I2C bus */
	struct bmg160 device;
	uint8_t          range;
	uint16_t         lowpass_bw;
	/*! GPIO number used for interrupts */
	uint32_t gpio_num;

	uint32_t dev_select;

	uint32_t ticks_per_ms;
	/* odr_reported reported to SMGR */
	uint32_t odr_reported;
	/* odr value actually used by HW */
	uint32_t odr_hw;
	bool  dri_enabled;
	bool                    bw_timer_pending;
	sns_ddf_timer_s         bw_timer;

	uint8_t                 bw_decided;
	/* the ODR used for the whole device */
	uint32_t                odr_shared;

	/*! time of last update */
	sns_ddf_time_t          data_cache_time;
	sns_ddf_status_e        data_status[SDD_GYRO_NUM_DATATYPES];

	/*! used when notifying the data to SMGR */
	sns_ddf_sensor_data_s   sensor_data;

	/*! FIFO */
	/*! cache used when notifying the data to SMGR */
	sns_ddf_sensor_data_s   f_frames_cache;

	/* water mark level */
	uint8_t                 f_wml;
	sns_ddf_time_t          f_avg_sample_interval;
	sns_ddf_time_t          ts_start_first_sample;
	sns_ddf_time_t          ts_irq;
	uint32_t                en_dri          :1;
	uint32_t                en_fifo_int     :1;
	uint32_t                en_md           :1;
	uint32_t                en_db_tap_int   :1;
	uint32_t                soft_reset:1;
	uint32_t                test_init:1;

	uint32_t                num_irq_drdy;
	sns_ddf_time_t          ts_last_drdy;

	sns_ddf_axes_map_s      axes_map;
	q16_t                   data_cache_fifo[SDD_GYRO_NUM_DATATYPES];
} sns_dd_bmg160_state_t;

static sns_ddf_status_e sns_dd_bmg160_init(
		sns_ddf_handle_t*        dd_handle_ptr,
		sns_ddf_handle_t         smgr_handle,
		sns_ddf_nv_params_s*     nv_params,
		sns_ddf_device_access_s  device_info[],
		uint32_t                 num_devices,
		sns_ddf_memhandler_s*    memhandler,
		sns_ddf_sensor_e*        sensors[],
		uint32_t*                num_sensors);

static sns_ddf_status_e sns_dd_bmg160_get_data(
		sns_ddf_handle_t        dd_handle,
		sns_ddf_sensor_e        sensors[],
		uint32_t                num_sensors,
		sns_ddf_memhandler_s*   memhandler,
		sns_ddf_sensor_data_s*  data[]);

static sns_ddf_status_e sns_dd_bmg160_set_attr(
		sns_ddf_handle_t     dd_handle,
		sns_ddf_sensor_e     sensor,
		sns_ddf_attribute_e  attrib,
		void*                value);

static sns_ddf_status_e sns_dd_bmg160_get_attr(
		sns_ddf_handle_t       dd_handle,
		sns_ddf_sensor_e       sensor,
		sns_ddf_attribute_e    attrib,
		sns_ddf_memhandler_s*  memhandler,
		void**                 value,
		uint32_t*              num_elems);


static sns_ddf_status_e sns_dd_bmg160_reset(sns_ddf_handle_t dd_handle);

static void sns_dd_bmg160_update_sample_ts(
		sns_dd_bmg160_state_t *state,
		int in_irq,
		int number);

static sns_ddf_status_e sns_dd_bmg160_enable_sched_data(
		sns_ddf_handle_t dd_handle,
		sns_ddf_sensor_e sensor,
		bool enable);

static sns_ddf_status_e sns_dd_gyro_bmg160_self_test(
		sns_ddf_handle_t dd_handle,
		sns_ddf_sensor_e sensor,
		sns_ddf_test_e test,
		uint32_t* err);


static void sns_dd_bmg160_interrupt_handler(
		sns_ddf_handle_t        dd_handle,
		uint32_t                gpio_num,
		sns_ddf_time_t          timestamp);

static sns_ddf_status_e bmg160_fifo_data_read_out_frames(
		sns_dd_bmg160_state_t * state,
		uint8_t f_count,
		int in_irq);

static sns_ddf_status_e bmg160_calc_matching_wml(
		uint8_t desired_wml,
		uint8_t downsampling_factor,
		uint8_t *cal_wml)
{
	uint8_t i;

	for (i = desired_wml; i > 0; i--) {
		if ((0 == (desired_wml % i))
				&& ((i * downsampling_factor)
					<= (BMG160_MAX_FIFO_LEVEL - BMG160_FIFO_HEAD_ROOM))) {
			*cal_wml = i * downsampling_factor;
			return SNS_DDF_SUCCESS;
		}
	}

	return SNS_DDF_EINVALID_PARAM;
}


static struct bmg160 *p_bmg160;

static sns_ddf_handle_t bmg160_port_handle;

sns_ddf_driver_if_s sns_bmg160_driver_fn_list;


BMG160_RETURN_FUNCTION_TYPE bmg160_init(struct bmg160 *bmg160)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;

	p_bmg160 = bmg160;

	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_interrupt_status_reg_1(
		unsigned char *status1_data)
{
	BMG160_RETURN_FUNCTION_TYPE comres;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,
				BMG160_INT_STATUS1, status1_data, 1);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_bw(unsigned char bandwidth)
{
	BMG160_RETURN_FUNCTION_TYPE comres;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		if (bandwidth < C_BMG160_Eight_U8X)
		{
			comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,
					BMG160_DSP_BW_ADDR_BW__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,
					BMG160_DSP_BW_ADDR_BW, bandwidth);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr,
					BMG160_DSP_BW_ADDR_BW__REG, &v_data_u8r, 1);
		}
		else
		{
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_range_reg(unsigned char range)
{
	BMG160_RETURN_FUNCTION_TYPE comres;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		if (range < C_BMG160_Five_U8X)
		{
			comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_DSP_RANGE_ADDR_RANGE__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_DSP_RANGE_ADDR_RANGE, range);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_DSP_RANGE_ADDR_RANGE__REG, &v_data_u8r, 1);
		}
		else
		{
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_dataXYZ(
		struct bmg160_data *data)
{
	BMG160_RETURN_FUNCTION_TYPE comres;
	unsigned char a_data_u8r[6];
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,BMG160_DATAX_LSB_VALUEX__REG,a_data_u8r,6);
		/* EasyCASE - */
		/* Data X */
		a_data_u8r[0] = BMG160_GET_BITSLICE(a_data_u8r[0],BMG160_DATAX_LSB_VALUEX);
		data->datax = (BMG160_S16)((((BMG160_S16)((signed char)a_data_u8r[1])) << BMG160_SHIFT_8_POSITION) | (a_data_u8r[0]));
		/* Data Y */
		a_data_u8r[2] = BMG160_GET_BITSLICE(a_data_u8r[2],BMG160_DATAY_LSB_VALUEY);
		data->datay = (BMG160_S16)((((BMG160_S16)((signed char)a_data_u8r[3])) << BMG160_SHIFT_8_POSITION) | (a_data_u8r[2]));

		/* Data Z */
		a_data_u8r[4] = BMG160_GET_BITSLICE(a_data_u8r[4],BMG160_DATAZ_LSB_VALUEZ);
		data->dataz = (BMG160_S16)((((BMG160_S16)((signed char)a_data_u8r[5])) << BMG160_SHIFT_8_POSITION) | (a_data_u8r[4]));
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_soft_reset()
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_SoftReset_u8r;
	v_SoftReset_u8r = 0xB6;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_BA_SOFTRESET_ADDR, &v_SoftReset_u8r, 1);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_watermark_enable(unsigned char fifo_wn_en)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		if (fifo_wn_en < C_BMG160_Two_U8X)
		{
			comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_INT_4_FIFO_WM_EN__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_INT_4_FIFO_WM_EN, fifo_wn_en);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_INT_4_FIFO_WM_EN__REG, &v_data_u8r, 1);
		}
		else
		{
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_mode(uint8_t mode)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char data1,data2;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		if (mode < C_BMG160_Six_U8X)
		{
			comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_MODE_CTRL_REG, &data1, C_BMG160_One_U8X );
			comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_PMU_LPW1_ADDR, &data2, C_BMG160_One_U8X );
			switch (mode)
			{
				case BMG160_MODE_NORMAL:
					data1  = BMG160_SET_BITSLICE(data1, BMG160_MODE_CTRL, C_BMG160_Zero_U8X);
					data2  = BMG160_SET_BITSLICE(data2, BMG160_PMU_LPW1_ADDR_FAST_POWERUP, C_BMG160_Zero_U8X);
					break;
				case BMG160_MODE_LOWPOWERNORMAL:
					data1  = BMG160_SET_BITSLICE(data1, BMG160_MODE_CTRL, C_BMG160_Two_U8X);
					data2  = BMG160_SET_BITSLICE(data2, BMG160_PMU_LPW1_ADDR_FAST_POWERUP, C_BMG160_Zero_U8X);
					break;
				case BMG160_MODE_SUSPENDNORMAL:
					data1  = BMG160_SET_BITSLICE(data1, BMG160_MODE_CTRL, C_BMG160_Four_U8X);
					data2  = BMG160_SET_BITSLICE(data2, BMG160_PMU_LPW1_ADDR_FAST_POWERUP, C_BMG160_Zero_U8X);
					break;
				case BMG160_MODE_DEEPSUSPEND:
					data1  = BMG160_SET_BITSLICE(data1, BMG160_MODE_CTRL, C_BMG160_One_U8X);
					data2  = BMG160_SET_BITSLICE(data2, BMG160_PMU_LPW1_ADDR_FAST_POWERUP, C_BMG160_Zero_U8X);
					break;
				default:
					break;
			}
			comres += p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_MODE_CTRL_REG, &data1, C_BMG160_One_U8X);
			p_bmg160->delay_msec(1);
			comres += p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_PMU_LPW1_ADDR, &data2, C_BMG160_One_U8X);
		}
		else
		{
			comres = SNS_DDF_EINVALID_PARAM ;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_enable_int_drdy(unsigned char data_en)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		comres = p_bmg160->BMG160_BUS_READ_FUNC\
		         (p_bmg160->dev_addr,\
		          BMG160_INT_ENABLE0_DATAEN__REG, &v_data_u8r, 1);
		v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
				BMG160_INT_ENABLE0_DATAEN, data_en);
		comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
		         (p_bmg160->dev_addr,\
		          BMG160_INT_ENABLE0_DATAEN__REG, &v_data_u8r, 1);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_enable_int_fifo(unsigned char fifo_en)
{
	BMG160_RETURN_FUNCTION_TYPE comres= SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		if (fifo_en < C_BMG160_Two_U8X)
		{
			comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_INT_ENABLE0_FIFOEN__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_INT_ENABLE0_FIFOEN, fifo_en);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_INT_ENABLE0_FIFOEN__REG, &v_data_u8r, 1);
		}
		else
		{
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_int_od(unsigned char param, unsigned char int_od)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		switch (param) {
			case BMG160_INT1:
				comres = p_bmg160->BMG160_BUS_READ_FUNC\
				         (p_bmg160->dev_addr,\
				          BMG160_INT_ENABLE1_IT1_OD__REG, &v_data_u8r, 1);
				v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
						BMG160_INT_ENABLE1_IT1_OD, int_od);
				comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
				         (p_bmg160->dev_addr,\
				          BMG160_INT_ENABLE1_IT1_OD__REG, &v_data_u8r, 1);
				break;
			case BMG160_INT2:
				comres = p_bmg160->BMG160_BUS_READ_FUNC\
				         (p_bmg160->dev_addr,\
				          BMG160_INT_ENABLE1_IT2_OD__REG, &v_data_u8r, 1);
				v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
						BMG160_INT_ENABLE1_IT2_OD, int_od);
				comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
				         (p_bmg160->dev_addr,\
				          BMG160_INT_ENABLE1_IT2_OD__REG, &v_data_u8r, 1);
				break;
			default:
				comres = SNS_DDF_EINVALID_PARAM;
				break;
		}
	}
	return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_map_int1_fifo(unsigned char fifo_int1)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		if (fifo_int1 < C_BMG160_Two_U8X)
		{
			comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT1_FIFO__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_MAP_1_INT1_FIFO, fifo_int1);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT1_FIFO__REG, &v_data_u8r, 1);
		}
		else
		{
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_map_int1_drdy(unsigned char en)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT1_DATA__REG, &v_data_u8r, 1);
		v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_MAP_1_INT1_DATA, !!en);
		comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT1_DATA__REG, &v_data_u8r, 1);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_int2_fifo(unsigned char fifo_int2)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL)
	{
		comres = SNS_DDF_EINVALID_PARAM;
	}
	else
	{
		if (fifo_int2 < C_BMG160_Two_U8X)
		{
			comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT2_FIFO__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_MAP_1_INT2_FIFO, fifo_int2);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT2_FIFO__REG, &v_data_u8r, 1);
		}
		else
		{
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_int_data(unsigned char axis, unsigned char int_data)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		switch (axis) {
			case BMG160_INT1_DATA:
				comres = p_bmg160->BMG160_BUS_READ_FUNC\
				         (p_bmg160->dev_addr,\
				          BMG160_MAP_1_INT1_DATA__REG, &v_data_u8r, 1);
				v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
						BMG160_MAP_1_INT1_DATA, int_data);
				comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
				         (p_bmg160->dev_addr,\
				          BMG160_MAP_1_INT1_DATA__REG, &v_data_u8r, 1);
				break;
			case BMG160_INT2_DATA:
				comres = p_bmg160->BMG160_BUS_READ_FUNC\
				         (p_bmg160->dev_addr,\
				          BMG160_MAP_1_INT2_DATA__REG, &v_data_u8r, 1);
				v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
						BMG160_MAP_1_INT2_DATA, int_data);
				comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
				         (p_bmg160->dev_addr,\
				          BMG160_MAP_1_INT2_DATA__REG, &v_data_u8r, 1);
				break;
			default:
				comres = SNS_DDF_EINVALID_PARAM;
				break;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_tag(unsigned char *tag)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
				BMG160_FIFO_CGF1_ADDR_TAG__REG, &v_data_u8r, 1);
		*tag = BMG160_GET_BITSLICE(v_data_u8r,\
				BMG160_FIFO_CGF1_ADDR_TAG);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_tag(unsigned char tag)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		if (tag < C_BMG160_Two_U8X) {
			comres = p_bmg160->BMG160_BUS_READ_FUNC\
			         (p_bmg160->dev_addr,\
			          BMG160_FIFO_CGF1_ADDR_TAG__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
					BMG160_FIFO_CGF1_ADDR_TAG, tag);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
			         (p_bmg160->dev_addr,\
			          BMG160_FIFO_CGF1_ADDR_TAG__REG, &v_data_u8r, 1);
		} else {
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_watermarklevel(
		unsigned char *water_mark_level)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
				BMG160_FIFO_CGF1_ADDR_WML__REG, &v_data_u8r, 1);
		*water_mark_level = BMG160_GET_BITSLICE(v_data_u8r,\
				BMG160_FIFO_CGF1_ADDR_WML);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_watermarklevel(
		unsigned char water_mark_level)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		if (water_mark_level < C_BMG160_OneTwentyEight_U8X) {
			comres = p_bmg160->BMG160_BUS_READ_FUNC\
			         (p_bmg160->dev_addr,\
			          BMG160_FIFO_CGF1_ADDR_WML__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
					BMG160_FIFO_CGF1_ADDR_WML, water_mark_level);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
			         (p_bmg160->dev_addr,\
			          BMG160_FIFO_CGF1_ADDR_WML__REG, &v_data_u8r, 1);
		} else {
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_data_reg(unsigned char *fifo_data)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
				BMG160_FIFO_DATA_ADDR, &v_data_u8r, 1);
		*fifo_data = v_data_u8r;
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifostatus_reg(
		unsigned char *fifo_status)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
				BMG160_FIFO_STATUS, fifo_status, 1);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_framecount(
		unsigned char *fifo_framecount)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
				BMG160_FIFO_STATUS_FRAME_COUNTER__REG, &v_data_u8r, 1);
		*fifo_framecount = BMG160_GET_BITSLICE(v_data_u8r,\
				BMG160_FIFO_STATUS_FRAME_COUNTER);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_overrun(
		unsigned char *fifo_overrun)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
				BMG160_FIFO_STATUS_OVERRUN__REG, &v_data_u8r, 1);
		*fifo_overrun = BMG160_GET_BITSLICE(v_data_u8r,\
				BMG160_FIFO_STATUS_OVERRUN);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_mode(unsigned char *mode)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
				BMG160_FIFO_CGF0_ADDR_MODE__REG, &v_data_u8r, 1);
		*mode = BMG160_GET_BITSLICE(v_data_u8r,\
				BMG160_FIFO_CGF0_ADDR_MODE);
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_mode(unsigned char mode)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		if (mode < C_BMG160_Four_U8X) {
			comres = p_bmg160->BMG160_BUS_READ_FUNC\
			         (p_bmg160->dev_addr,\
			          BMG160_FIFO_CGF0_ADDR_MODE__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
					BMG160_FIFO_CGF0_ADDR_MODE, mode);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
			         (p_bmg160->dev_addr,\
			          BMG160_FIFO_CGF0_ADDR_MODE__REG, &v_data_u8r, 1);
		} else {
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_data_sel(unsigned char *data_sel)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
				BMG160_FIFO_CGF0_ADDR_DATA_SEL__REG, &v_data_u8r, 1);
		*data_sel = BMG160_GET_BITSLICE(v_data_u8r,\
				BMG160_FIFO_CGF0_ADDR_DATA_SEL);
	}
	return comres;
}

/*****************************************************************************
 * Description: *//**\brief This API is used to set the status of fifo data
 * sel

                   *  \param unsigned char data_sel
                   *         data_sel --> [0:3]
                   *         0 --> X,Y and Z (DEFAULT)
                   *         1 --> X only
                   *         2 --> Y only
                   *         3 --> Z only

                   *  \return communication results
                   *

                   *****************************************************************************/

BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_data_sel(unsigned char data_sel)
{
	BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
	unsigned char v_data_u8r;
	if (p_bmg160 == BMG160_NULL) {
		comres = SNS_DDF_EINVALID_PARAM;
	} else {
		if (data_sel < C_BMG160_Four_U8X) {
			comres = p_bmg160->BMG160_BUS_READ_FUNC\
			         (p_bmg160->dev_addr,\
			          BMG160_FIFO_CGF0_ADDR_DATA_SEL__REG, &v_data_u8r, 1);
			v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
					BMG160_FIFO_CGF0_ADDR_DATA_SEL, data_sel);
			comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
			         (p_bmg160->dev_addr,\
			          BMG160_FIFO_CGF0_ADDR_DATA_SEL__REG, &v_data_u8r, 1);
		} else {
			comres = SNS_DDF_EINVALID_PARAM;
		}
	}
	return comres;
}

static sns_ddf_status_e bmg160_enable_int_fifo_wm(
		sns_ddf_handle_t port_handle,
		int en)
{
	sns_ddf_status_e status;

	if ((status = BMG_CALL_API(set_fifo_tag)(0)) != SNS_DDF_SUCCESS)
	{
		return status;
	}

	status = bmg160_set_fifo_watermark_enable(!!en);

	return status;
}

static sns_ddf_status_e bmg160_config_fifo_mode(
		sns_ddf_handle_t port_handle,
		int wml)
{
	sns_ddf_status_e status;
	uint8_t fifo_mode;

	if (wml <= 0) {
		fifo_mode = BMG160_FIFO_MODE_BYPASS;
	} else {
		fifo_mode = BMG160_FIFO_MODE_STREAM;
	}

#if BMG160_CONFIG_DRI_MOCKUP_WMI
	if (1 == wml) {
		fifo_mode = BMG160_FIFO_MODE_BYPASS;
	}
#endif
	status = bmg160_set_fifo_mode(fifo_mode);

	return status;
}

static void sns_dd_bmg160_convertNcorrect_sample(
		sns_dd_bmg160_state_t *state,
		struct bmg160_data *gyro_data,
		q16_t samples[3])
{
	samples[SDD_GYRO_X] = FX_FLTTOFIX_Q16(gyro_data->datax
			* ((4000>>state->range) * PI / 180 / 65536)) + state->bias[0];
	samples[SDD_GYRO_Y] = FX_FLTTOFIX_Q16(gyro_data->datay
			* ((4000>>state->range) * PI / 180 / 65536)) + state->bias[1];
	samples[SDD_GYRO_Z] = FX_FLTTOFIX_Q16(gyro_data->dataz
			* ((4000>>state->range) * PI / 180 / 65536)) + state->bias[2];

	BMG160_MSG_3_P(ERROR, "gyro FIFO frame raw data %d %d %d",
			gyro_data->datax,
			gyro_data->datay,
			gyro_data->dataz);

	BMG160_MSG_3_P(ERROR, "gyro FIFO frame Q16 data before remapping %d %d %d",
			samples[SDD_GYRO_X],
			samples[SDD_GYRO_Y],
			samples[SDD_GYRO_Z]);

	sns_ddf_map_axes(&state->axes_map, samples);

	BMG160_MSG_3_P(ERROR, "gyro FIFO frame Q16 data after remapping %d %d %d",
			samples[SDD_GYRO_X],
			samples[SDD_GYRO_Y],
			samples[SDD_GYRO_Z]);
}

static sns_ddf_status_e sns_dd_bmg160_config_range(
		sns_dd_bmg160_state_t *state)
{
	uint8_t bosch_write_buffer;
	sns_ddf_status_e stat;

	bosch_write_buffer = state->range ;
	if ((stat =  BMG_CALL_API(set_range_reg)(bosch_write_buffer)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_bmg160_config_bw(
		sns_dd_bmg160_state_t *state)
{
	uint8_t bosch_write_buffer;
	sns_ddf_status_e stat;
	uint32_t i;
	bosch_write_buffer = state->lowpass_bw;

	for (i = 0; i < BST_ARRARY_SIZE(BMG160_MAP_BW_ODR); i++) {
		if (BMG160_MAP_BW_ODR[i].l == bosch_write_buffer) {
			break;
		}
	}

	if (i >= BST_ARRARY_SIZE(BMG160_MAP_BW_ODR)) {
		return SNS_DDF_EINVALID_PARAM;
	}

	if ((stat = BMG_CALL_API(set_bw)(bosch_write_buffer)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	state->odr_hw = BMG160_MAP_BW_ODR[i].r;

	return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_bmg160_config_odr(
		sns_dd_bmg160_state_t *state,
		uint32_t odr_value)
{
	sns_ddf_status_e stat;
	uint8_t bosch_write_buffer;
	uint8_t flag_downsampling = false;

#if BMG160_CONFIG_BW_SEL_OLD_WAY
	/* For Normal and UI */
	if (odr_value <= 5) { /* NORMAL */
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_12HZ;
		state->odr_reported = 5;
		state->odr_hw = 100;
		flag_downsampling = true;
	} else if (odr_value <= 20) { /* UI */
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_12HZ;
		state->odr_reported = 20;
		state->odr_hw = 100;
		flag_downsampling = true;
	} else if (odr_value <= 50) {   /* GAME */
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_23HZ;
		state->odr_reported = 50;
		state->odr_hw = 200;
		flag_downsampling = true;
	} /* for FASTEST */
	else if (odr_value < 1000) {
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_116HZ;
		state->odr_reported = BMG160_CONFIG_FASTEST_ODR;
		state->odr_hw = 1000;
		flag_downsampling = true;
	}
	else {
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_230HZ;
		state->odr_reported = 2000;
	}

#else
	if (odr_value < 101)
	{
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_12HZ;
		state->odr_reported = 100;
	}
	else if (odr_value < 201)
	{
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_23HZ;
		state->odr_reported = 200;
	}
#if 0
	//this part is disabled to support 250HZ
	else if (odr_value < 401)
	{
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_47HZ;
		state->odr_reported = 400;
	}
#endif
	else if (odr_value < 1001)
	{
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_116HZ;
		state->odr_reported = 1000;
	}

	else
	{
		bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_230HZ;
		state->odr_reported = 2000;
	}
#endif
	if (!flag_downsampling) {
		state->odr_hw = state->odr_reported;
	}

	if ((stat = BMG_CALL_API(set_bw)(bosch_write_buffer)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}


	return SNS_DDF_SUCCESS;
}

sns_ddf_status_e bmg160_smbus_read_byte_block(
		sns_ddf_handle_t port_handle,
		unsigned char reg_addr,
		unsigned char *data,
		uint32_t len)
{
	sns_ddf_status_e stat;
	uint8_t out;

	if ((stat = sns_ddf_read_port(port_handle,
					reg_addr,
					data,
					len,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}
	return stat;
}

#define SNS_DDF_PORT_READ_LEN_LIMIT 255
sns_ddf_status_e bmg160_read_fifo_frames(
		sns_ddf_handle_t port_handle,
		uint32_t f_len,
		uint32_t f_count,
		uint8_t *buf)
{
	sns_ddf_status_e stat = SNS_DDF_SUCCESS;
	uint32_t bytes_to_read = f_len * f_count;
	uint32_t bytes_to_read_once;
	uint8_t *head;

	head = buf;
	while (bytes_to_read > 0) {
		if (bytes_to_read > SNS_DDF_PORT_READ_LEN_LIMIT) {
			bytes_to_read_once = SNS_DDF_PORT_READ_LEN_LIMIT
				/ f_len * f_len;
		} else {
			bytes_to_read_once = bytes_to_read;
		}

		if ((stat = bmg160_smbus_read_byte_block(port_handle,
						BMG160_FIFO_DATA_ADDR,
						head,
						bytes_to_read_once))
				!= SNS_DDF_SUCCESS)
			return stat;

		head = head + bytes_to_read_once;
		bytes_to_read -= bytes_to_read_once;
	}

	return stat;
}

sns_ddf_status_e bmg_read_wrapper(
		uint8_t dev_addr,
		uint8_t reg_addr,
		uint8_t *bosch_read_data,
		uint8_t len)
{

	sns_ddf_status_e stat;
	uint8_t out;

	stat = sns_ddf_read_port(bmg160_port_handle,
			reg_addr,
			bosch_read_data,
			len,
			&out);

	return stat;
}

sns_ddf_status_e bmg_write_wrapper(uint8_t dev_addr,
		uint8_t reg_addr,
		uint8_t *bosch_write_buffer,
		uint8_t len)
{

	sns_ddf_status_e stat;
	uint8_t out;

	stat = sns_ddf_write_port(bmg160_port_handle,
			reg_addr,
			bosch_write_buffer,
			len,
			&out);
	return stat;
}
static void bmg_delay(uint32_t msec)
{
	sns_ddf_delay(1000*msec);
}


/*============================================================================

  Global Data Definitions

  ============================================================================*/


static sns_ddf_sensor_e my_sensors[] =
{
	SNS_DDF_SENSOR_GYRO,
};


sns_ddf_status_e sns_dd_bmg160_reg_dump(sns_dd_bmg160_state_t *state)
{
	sns_ddf_status_e stat;
	uint8_t readbyte = 0;
	uint8_t  out;

	if ((stat = sns_ddf_read_port(state->port_handle,
					BMG160_FIFO_CGF1_ADDR,
					&readbyte,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_reg_dump BMG160_FIFO_CGF1_ADDR %d %d %d",
			55555581, out, readbyte);

	if ((stat = sns_ddf_read_port(state->port_handle,
					BMG160_DSP_BW_ADDR,
					&readbyte,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_reg_dump BMG160_DSP_BW_ADDR %d %d %d",
			55555581, out, readbyte);

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_reg_dump odr and wml 55555583 %d %d %d",
			state->odr_reported, state->odr_hw, state->f_wml);

	if ((stat = sns_ddf_read_port(state->port_handle,
					BMG160_RST_LATCH_ADDR,
					&readbyte,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_reg_dump BMG160_RST_LATCH_ADDR %d %d %d",
			5555582, out, readbyte);


	if ((stat = sns_ddf_read_port(state->port_handle,
					BMG160_INT_ENABLE0,
					&readbyte,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_reg_dump BMG160_INT_ENABLE0 %d %d %d",
			5555583, out, readbyte);


	if ((stat = sns_ddf_read_port(state->port_handle,
					BMG160_INT_ENABLE1,
					&readbyte,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_reg_dump BMG160_INT_ENABLE1 %d %d %d",
			5555584, out, readbyte);


	if ((stat = sns_ddf_read_port(state->port_handle,
					BMG160_INT_MAP_1,
					&readbyte,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_reg_dump BMG160_INT_MAP_1 %d %d %d",
			5555585, out, readbyte);
	return stat;
}

/*===========================================================================

FUNCTION:   sns_dd_bmg160_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
  in after init() was called.

  @detail


  @param[in] state: Ptr to the driver structure

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Check if open and close port operation is required here?
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_bmg160_reset(sns_ddf_handle_t dd_handle)
{
	uint8_t i;
	sns_ddf_status_e stat;
	sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
	sns_ddf_time_t time_start;
	sns_ddf_time_t time_end;

#ifdef BMG160_DEBUG
	sns_ddf_axes_map_s      *axes_map = &state->axes_map;
#endif
	BMG160_MSG_3_P(HIGH,"55555565 %d %d %d", axes_map->indx_x, axes_map->indx_y, axes_map->indx_z);
	BMG160_MSG_3_P(HIGH,"55555565 %d %d %d", axes_map->sign_x, axes_map->sign_y, axes_map->sign_z);

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_reset 55555560 0x%x %d %d",
			dd_handle, state->gpio_num, state->dev_select);
	BMG160_MSG_1(HIGH, "ticks_per_ms: %d", state->ticks_per_ms);


	if (!state->soft_reset) {
		return SNS_DDF_SUCCESS;
	}

	BMG_CALL_API(init)(&(state->device));


	for (i = 0; i < SDD_GYRO_NUM_AXES; i++)
	{
		state->bias[i] = 0;
	}

	state->out_of_bound_flag = 0;

	state->range = BOSCH_GYRO_SENSOR_RANGE_2000;
	state->lowpass_bw = BOSCH_GYRO_SENSOR_BW_23HZ;


	if ((stat = BMG_CALL_API(set_soft_reset)()) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	time_start = sns_ddf_get_timestamp();
	sns_ddf_delay(40000);//40ms
	time_end = sns_ddf_get_timestamp();

	/* estimate the ticks_per_ms */
	state->ticks_per_ms = (uint32_t)(((int32_t)time_end - (int32_t)time_start) / 40.0);

	if ((stat = sns_dd_bmg160_config_range(state)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	if ((stat = sns_dd_bmg160_config_bw(state)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}


	if ((stat = bmg160_set_int_od(BMG160_INT1,
					INT_PIN_PUSH_PULL)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	sns_ddf_delay(5000);//5ms

	/*! Set to suspend */
	if (!state->test_init) {
		if ((stat = BMG_CALL_API(set_mode)(BMG160_MODE_DEEPSUSPEND)) != SNS_DDF_SUCCESS)
		{
			return stat;
		}
	}

	/* maps interrupt to INT1 pin */
#if 0
	if ((stat = BMG_CALL_API(set_int_data)(BMG160_INT1, INT_ENABLE)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}
#endif

	sns_ddf_delay(1000);
	sns_dd_bmg160_reg_dump(state);

	return stat;

}

sns_ddf_status_e bmg_check_chip_id(sns_dd_bmg160_state_t *state)
{
	uint8_t chip_id = 0;
	sns_ddf_status_e stat;
	uint8_t  out;

	if ((stat = sns_ddf_read_port(state->port_handle,
					BMG160_CHIP_ID,
					&chip_id,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}


	if ((chip_id & 0xff) != SENSOR_CHIP_ID_BMG) {
		stat = SNS_DDF_EDEVICE;
	}

	return stat;
}

static sns_ddf_status_e bmg160_get_dev_model_info(
		sns_dd_bmg160_state_t *state)
{
	sns_ddf_status_e stat;
	uint8_t reg_val;
	uint8_t out;
	uint8_t bit0;
	uint8_t bit1;
	uint8_t bit2;

	/* check the part info further */
	reg_val = 0x8A;
	if ((stat = sns_ddf_write_port(state->port_handle,
					0x35,
					&reg_val,
					1,
					&out)) != SNS_DDF_SUCCESS) {
		return stat;
	}
	sns_ddf_delay(2000);
	reg_val = 0x8A;
	if ((stat = sns_ddf_write_port(state->port_handle,
					0x35,
					&reg_val,
					1,
					&out)) != SNS_DDF_SUCCESS) {
		return stat;
	}
	sns_ddf_delay(2000);

	if ((stat = sns_ddf_read_port(state->port_handle,
					0x4C,
					&reg_val,
					1,
					&out)) != SNS_DDF_SUCCESS) {
		return stat;
	}
	sns_ddf_delay(500);
	if ((stat = sns_ddf_read_port(state->port_handle,
					0x4C,
					&reg_val,
					1,
					&out)) != SNS_DDF_SUCCESS) {
		return stat;
	}
	bit0 = reg_val & (1<<7);

	if ((stat = sns_ddf_read_port(state->port_handle,
					0x56,
					&reg_val,
					1,
					&out)) != SNS_DDF_SUCCESS) {
		return stat;
	}
	sns_ddf_delay(500);

	if ((stat = sns_ddf_read_port(state->port_handle,
					0x56,
					&reg_val,
					1,
					&out)) != SNS_DDF_SUCCESS) {
		return stat;
	}
	bit1 = reg_val & (1<<7);

	if ((stat = sns_ddf_read_port(state->port_handle,
					0x60,
					&reg_val,
					1,
					&out)) != SNS_DDF_SUCCESS) {
		return stat;
	}
	sns_ddf_delay(500);

	if ((stat = sns_ddf_read_port(state->port_handle,
					0x60,
					&reg_val,
					1,
					&out)) != SNS_DDF_SUCCESS) {
		return stat;
	}
	bit2 = reg_val & (1<<7);

	reg_val = 0x0A;
	if ((stat = sns_ddf_write_port(state->port_handle,
					0x35,
					&reg_val,
					1,
					&out)) != SNS_DDF_SUCCESS) {
		return stat;
	}
	sns_ddf_delay(2000);

	if ((bit0) && (!bit1) && (!bit2)) {
		state->dev_select = BMG160_DEV_SEL_NUM_BMI058;
	}
	/* end of check */

	return stat;
}


sns_ddf_status_e sns_dd_bmg160_enable_sched_data(
		sns_ddf_handle_t dd_handle,
		sns_ddf_sensor_e sensor,
		bool             enable)
{
	sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
	sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
	uint32_t signal_registered = 0;

	uint8_t retry = 0;
	uint8_t readbyte = 0;
	uint8_t out = 0;
	uint8_t dri_mockup_wmi;

	uint32_t irq_trigger_type = BMG160_IRQ_TRIGGER_TYPE_DEFAULT;

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_enable_sched_data  %d %d %d",
			5555590, sensor, enable);

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_enable_sched_data 5555591 %d %d %d",
			state->odr_reported, state->odr_hw, state->f_wml);

	if (!sns_ddf_signal_irq_enabled())
	{
		return SNS_DDF_EINVALID_PARAM;
	}

	signal_registered = state->en_dri || state->en_fifo_int;

	if (enable)
	{
		while (retry++ < 5) {
			sns_ddf_read_port(state->port_handle,
					BMG160_INT_ENABLE1,
					&readbyte,
					1,
					&out);
			if (BMG160_INT_EN_1_EXPECTED_VAL_INT1_PUSHPULL_ACTIVE_HIGH == readbyte) {
				break;
			} else {
				if ((ret_val = bmg160_set_int_od(BMG160_INT1, INT_PIN_PUSH_PULL))
						!= SNS_DDF_SUCCESS)
				{
					return ret_val;
				}
				sns_ddf_delay(1000 * 1);
			}
		}
		BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_enable_sched_data %d %d %d",
				5555592, retry, readbyte);

		if (state->f_wml > 0) {
#if BMG160_CONFIG_DRI_MOCKUP_WMI
			if (1 == state->f_wml) {
				dri_mockup_wmi = 1;
			} else {
				dri_mockup_wmi = 0;
			}
#else
			dri_mockup_wmi = 0;
#endif
			if (!dri_mockup_wmi) {
				if ((ret_val = bmg160_enable_int_fifo(1))
						!= SNS_DDF_SUCCESS)
				{
					return ret_val;
				}

				if ((ret_val = bmg160_enable_int_fifo_wm(state->port_handle, 1))
						!= SNS_DDF_SUCCESS)
				{
					return ret_val;
				}

				/* maps interrupt to INT1 pin */
				if ((ret_val = bmg160_map_int1_fifo(1)) != SNS_DDF_SUCCESS)
				{
					return ret_val;
				}

#if BMG160_CONFIG_WMI_TRIGGER_LEVEL
				irq_trigger_type = SNS_DDF_SIGNAL_IRQ_HIGH;
#endif
			} else {
				//wml == 1, implicit dri
				if ((ret_val = bmg160_enable_int_drdy(INT_ENABLE)) != SNS_DDF_SUCCESS) {
					return ret_val;
				}
				state->ts_start_first_sample = sns_ddf_get_timestamp();
				state->num_irq_drdy = 0;
				bmg160_map_int1_drdy(1);

				bmg160_map_int1_fifo(0);
			}

			state->en_fifo_int = true;
		} else {
			//wml == 0, explicit dri
			if ((ret_val = bmg160_enable_int_drdy(INT_ENABLE)) != SNS_DDF_SUCCESS) {
				return ret_val;
			}
			state->ts_start_first_sample = sns_ddf_get_timestamp();
			state->num_irq_drdy = 0;
			bmg160_map_int1_drdy(INT_ENABLE);

			state->en_dri = true;

		}

		if (!signal_registered)
		{
			/*! Register to receive notification when the interrupt fires */
			if ((ret_val = sns_ddf_signal_register(
							state->gpio_num,
							dd_handle,
							&sns_bmg160_driver_fn_list,
							irq_trigger_type)) != SNS_DDF_SUCCESS)
			{
				return ret_val;
			}
			BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_enable_sched_data register handler %d %d %d",
					5555590, state->gpio_num, 2);
		}
	}
	else
	{
		state->en_fifo_int = false;
		state->en_dri = false;

		if (signal_registered)
		{
			if ((ret_val = sns_ddf_signal_deregister(state->gpio_num))
					!= SNS_DDF_SUCCESS)
			{
				return ret_val;
			}
			BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_enable_sched_data  deregister handler %d %d %d",
					5555590, state->gpio_num, 3);
		}

		bmg160_enable_int_fifo_wm(state->port_handle, 0);
		bmg160_enable_int_fifo(0);
		bmg160_enable_int_drdy(0);
		bmg160_map_int1_fifo(0);
		bmg160_map_int1_drdy(0);
	}

	BMG160_MSG_3(ERROR, "sns_dd_bmg160_enable_sched_data ts_start_first_sample: %d",
			state->ts_start_first_sample, state->en_fifo_int, state->en_dri);

	sns_dd_bmg160_reg_dump(state);

	return ret_val;
}

/**
 * @brief Parses the nv_params and fills in appropriate values for the axes_map
 *
 */
sns_ddf_status_e sns_dd_bmg160_parse_nv_params(
		sns_ddf_nv_params_s         *entire_nv_params,
		sns_dd_bmg160_state_t       *state)
{
	sns_ddf_axes_map_s      *axes_map = &state->axes_map;

	// If there are no nv_params handed in, then set all to default values
	if (NULL == entire_nv_params) {
		BMG160_MSG_3_P(ERROR, "%d %d %d",
				55555701, entire_nv_params, axes_map);
		sns_ddf_axes_map_init(axes_map, NULL);
	}
	// Else, parse the nv_params and validate each entry
	else {
		/*const sns_dd_bmg160_nv_params_s* nv_params; */
		if (NULL == entire_nv_params->data) {
			BMG160_MSG_3_P(ERROR,"sns_dd_bmg160_parse_nv_params no nv ram data %d %d %d", 0,1,1);
			return SNS_DDF_EFAIL;
		}
		/*nv_params = (sns_dd_bmg160_nv_params_s*)entire_nv_params->data;*/
		// Initialize the axes-map
		sns_ddf_axes_map_init(axes_map, entire_nv_params->data);
		BMG160_MSG_3_P(HIGH,"%d %d %d", axes_map->indx_x, axes_map->indx_y, axes_map->indx_z);
		BMG160_MSG_3_P(HIGH,"%d %d %d", axes_map->sign_x, axes_map->sign_y, axes_map->sign_z);
	}

	return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_bmg160_init

===========================================================================*/
/*!
  @brief Initializes Bosch magnetometer bmg160
  determines the device to use

  @detail
  - Allocates memory for driver state structure.
  Opens the device port by calling sns_ddf_open_port routine
  Calls sns_dd_bmg160_reset routine

  @param[out] dd_handle_ptr  Pointer that this function must malloc and
  populate. This is a handle to the driver
  instance that will be passed in to all
  other functions.
  @param[in]  smgr_handle    Handle used to identify this driver when it
  calls into Sensors Manager functions.
  @param[in]  nv_params      NV parameters retrieved for the driver.
  @param[in]  device_info    Information describing each of the physical
  devices controlled by this driver. This is
  used to configure the bus and talk to the
  device.
  @param[in]  memhandler     Memory handler used to dynamically allocate
  output parameters, if applicable. NB: Do
  not use memhandler to allocate memory for
  @a dd_handle_ptr.
  @param[in]  num_devices    Length of @a device_info.
  @param[out] sensors        List of supported sensors, populated and
  returned by this function.
  @param[out] num_sensors    Length of @a sensors.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_bmg160_init(
		sns_ddf_handle_t*        dd_handle_ptr,
		sns_ddf_handle_t         smgr_handle,
		sns_ddf_nv_params_s*     nv_params,
		sns_ddf_device_access_s  device_info[],
		uint32_t                 num_devices,
		sns_ddf_memhandler_s*    memhandler,
		sns_ddf_sensor_e*        sensors[],
		uint32_t*                num_sensors)
{
	sns_ddf_status_e stat;
	sns_dd_bmg160_state_t *ptr;

	sns_ddf_status_e err = SNS_DDF_SUCCESS;

	if (sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_bmg160_state_t)) != SNS_DDF_SUCCESS)
	{
		return SNS_DDF_ENOMEM;
	}
	ptr->smgr_handle = smgr_handle;
	ptr->gpio_num = device_info->first_gpio;


#if 0
	if ((stat = sns_ddf_signal_register(
					ptr->gpio_num,
					ptr,
					&sns_bmg160_driver_fn_list,
					SNS_DDF_SIGNAL_IRQ_RISING))
			!= SNS_DDF_SUCCESS)
	{
		BMG160_MSG_0(ERROR, "fail to register ISR for bmg160");
		return stat;
	}
#endif

	if ((stat = sns_ddf_open_port(&(ptr->port_handle) , &(device_info->port_config))) != SNS_DDF_SUCCESS)
	{
		return stat;
	}
	bmg160_port_handle = ptr->port_handle;

	*num_sensors = 1;
	*sensors = my_sensors;
	*dd_handle_ptr = (sns_ddf_handle_t)ptr;

	err = bmg_check_chip_id(ptr);
	if (!err) {
	} else {
		err = SNS_DDF_EDEVICE;
		return err;
	}

	sns_dd_bmg160_parse_nv_params(nv_params, ptr);

	/* h/w init */
	ptr->device.bus_read = bmg_read_wrapper;
	ptr->device.bus_write = bmg_write_wrapper;
	ptr->device.delay_msec = bmg_delay;

	stat = bmg160_get_dev_model_info(ptr);
	// Resets the bmg160
	ptr->soft_reset = 1;
	stat = sns_dd_bmg160_reset(ptr);
	ptr->soft_reset = 0;

	stat = sns_ddf_malloc((void**)&(((sns_dd_bmg160_state_t *)ptr)->f_frames_cache.samples),
			sizeof(sns_ddf_sensor_sample_s)
			* SDD_GYRO_NUM_AXES* BMG160_MAX_FIFO_LEVEL);
	if (stat != SNS_DDF_SUCCESS)
	{
		return stat;
	}
	return stat;
}
/*===========================================================================

FUNCTION:   sns_dd_bmg160_config_mode

===========================================================================*/
/*!
  @brief Sets the BOSCH magnetometer  to the passed in operating mode

  @detail
  - Set the operational mode (sleep or suspend) by programming
  BMC050_POWER_CNTL register

  @param[in] state: Ptr to the driver structure
  @param[in] mode:

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Get away with read modify write
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_bmg160_config_mode(
		sns_dd_bmg160_state_t *state,
		sns_ddf_powerstate_e  mode)
{
	sns_ddf_status_e status;
	uint8_t bosch_write_buffer;

	switch (mode)
	{
		case SNS_DDF_POWERSTATE_ACTIVE:
			bosch_write_buffer = BMG160_MODE_NORMAL;
			break;
		case SNS_DDF_POWERSTATE_LOWPOWER:
			//deep suspend
			bosch_write_buffer = BMG160_MODE_DEEPSUSPEND;
			break;
		default:
			return SNS_DDF_EINVALID_PARAM;

	}

	/* Set opmode */
	if ((status = BMG_CALL_API(set_mode)(bosch_write_buffer)) != SNS_DDF_SUCCESS)
	{
		return status;
	}


	return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e sns_dd_bmg160_set_attr_fifo(
		sns_ddf_handle_t     dd_handle,
		sns_ddf_sensor_e     sensor,
		void*                value)
{
	sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
	sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
	uint8_t         desired_wml = *((uint8_t *)value);
	uint8_t         hw_wml;
	uint8_t         downsampling_factor = 1;

	BMG160_MSG_3_F(ERROR, "SNS_DDF_ATTRIB_FIFO %d %d %d",
			state->odr_reported, state->odr_hw, desired_wml);

	if ((0 == state->odr_reported)
			|| (desired_wml >= BMG160_MAX_FIFO_LEVEL)) {
		return SNS_DDF_EINVALID_PARAM;
	}

	downsampling_factor = (state->odr_hw / state->odr_reported);
	/* we are using streaming FIFO MODE, thus max FIFO depth is 99 */

	if (desired_wml > 0) {
		if (downsampling_factor > 1) {
			ret_val = bmg160_calc_matching_wml(desired_wml, downsampling_factor, &hw_wml);
			if (SNS_DDF_SUCCESS != ret_val) {
				return ret_val;
			}
		} else {
			hw_wml = desired_wml;
		}
	} else {
		hw_wml = 0;
	}

	/* This assumes that ATTRIB_ODR is configured before ATTRIB_FIFO */
	if (hw_wml > 0) {
		ret_val = bmg160_set_fifo_watermarklevel((uint8_t)hw_wml);

		if (SNS_DDF_SUCCESS != ret_val) {
			BMG160_MSG_3_F(ERROR, "SNS_DDF_ATTRIB_FIFO %d %d %d",
					desired_wml, hw_wml, ret_val);
			return ret_val;
		}

	}

	state->f_wml = (uint8_t)hw_wml;

	ret_val = bmg160_config_fifo_mode(state->port_handle, state->f_wml);
	if (SNS_DDF_SUCCESS != ret_val) {
		BMG160_MSG_3_F(ERROR, "SNS_DDF_ATTRIB_FIFO %d %d %d",
				desired_wml, hw_wml, ret_val);
		return ret_val;
	}

	if (state->f_wml > 0) {
		state->ts_start_first_sample = sns_ddf_get_timestamp();
	}

	return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_bmg160_set_attr

===========================================================================*/
/*!
  @brief Sets an attribute of the bosch magnetometer

  @detail
  - Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
  SNS_DDF_EINVALID_PARAM


*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_bmg160_set_attr(
		sns_ddf_handle_t     dd_handle,
		sns_ddf_sensor_e     sensor,
		sns_ddf_attribute_e  attrib,
		void*                value)
{
	sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
	sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;

	switch (attrib)
	{
		case SNS_DDF_ATTRIB_POWER_STATE:
			/* set power mode */
			ret_val = sns_dd_bmg160_config_mode((sns_dd_bmg160_state_t*)dd_handle,
					*((sns_ddf_powerstate_e *)value));
			break;
		case SNS_DDF_ATTRIB_RANGE:
			if (*((uint8_t *)value) >= SNS_DD_GYRO_MAX_RANGE)
			{
				return SNS_DDF_EINVALID_ATTR;
			}
			else
			{
				((sns_dd_bmg160_state_t*)dd_handle)->range = sns_dd_gyro_range[*((uint8_t *)value)];
			}

			ret_val = sns_dd_bmg160_config_range((sns_dd_bmg160_state_t*)dd_handle);

			break;

		case SNS_DDF_ATTRIB_LOWPASS:


			if (*((uint8_t *)value) > BOSCH_GYRO_SENSOR_BW_32HZ)
			{
				return SNS_DDF_EINVALID_ATTR;
			}
			else
			{
				((sns_dd_bmg160_state_t*)dd_handle)->lowpass_bw = sns_dd_gyro_bw[*((uint8_t *)value)];
			}

			ret_val = sns_dd_bmg160_config_bw((sns_dd_bmg160_state_t*)dd_handle);

			break;

		case SNS_DDF_ATTRIB_ODR:
			{
				sns_ddf_odr_t odr_value = *(sns_ddf_odr_t *)value;

				if (0 == odr_value) {
					state->odr_reported = 0;
				} else {
					if ((ret_val = sns_dd_bmg160_config_odr(
									state,
									odr_value))
							!= SNS_DDF_SUCCESS)
					{
						return ret_val;
					}

				}
				return SNS_DDF_SUCCESS;
			}

		case SNS_DDF_ATTRIB_RESOLUTION_ADC:
			break;

		case SNS_DDF_ATTRIB_FIFO:
			if (sensor == SNS_DDF_SENSOR_GYRO) {
				return sns_dd_bmg160_set_attr_fifo(dd_handle, sensor, value);
			} else {
				return SNS_DDF_EINVALID_PARAM;
			}

			break;

		default:

			return SNS_DDF_EINVALID_PARAM;
	}

	return (ret_val);
}




/*===========================================================================

FUNCTION:   sns_dd_mag_bosch_get_data_all

===========================================================================*/
/*!
  @brief Gets the magnetometer data from the bosch magnetometer

  @detail
  - read the accel data and temp data registers and scales the values
  and stores them in the driver state structure in Q16 format

  @param[in] state: Ptr to the driver structure

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_bmg160_get_data_all(
		sns_dd_bmg160_state_t *state)
{
	sns_ddf_status_e stat;
	struct bmg160_data value;
	BMG160_S16 tmp;

	state->out_of_bound_flag = 0;

	if ((stat = BMG_CALL_API(get_dataXYZ)(&value)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	if (BMG160_DEV_SEL_NUM_BMI058 == state->dev_select) {
		tmp = value.datax;
		value.datax = value.datay;
		value.datay = tmp;
	}

	sns_dd_bmg160_convertNcorrect_sample(state, &value, state->data_cache);

	return SNS_DDF_SUCCESS;
}

/*===========================================================================
FUNCTION:   sns_dd_gyro_log_fifo
===========================================================================*/
/*!
  @brief log accel sensor data in fifo

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] accel_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
static void sns_dd_gyro_log_fifo(sns_ddf_sensor_data_s *gyro_data_ptr)
{
#if BMG160_CONFIG_FIFO_LOG
	sns_err_code_e err_code;
	sns_log_sensor_data_pkt_s* log_struct_ptr;
	uint16 idx =0 ;

	//Allocate log packet
	err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
			sizeof(sns_log_sensor_data_pkt_s) + (gyro_data_ptr->num_samples -1)*sizeof(int32_t),
			(void**)&log_struct_ptr);

	if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
	{
		log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
		log_struct_ptr->sensor_id = SNS_DDF_SENSOR_GYRO;
		log_struct_ptr->vendor_id = SNS_DDF_VENDOR_BOSCH;

		//Timestamp the log with sample time
		log_struct_ptr->timestamp = gyro_data_ptr->timestamp;
		log_struct_ptr->end_timestamp = gyro_data_ptr->end_timestamp;

		log_struct_ptr->num_data_types = SDD_GYRO_NUM_AXES;
		log_struct_ptr->num_samples = gyro_data_ptr->num_samples / SDD_GYRO_NUM_AXES;

		//Log the sensor fifo data
		log_struct_ptr->data[0]  = gyro_data_ptr->samples[0].sample;
		log_struct_ptr->data[1]  = gyro_data_ptr->samples[1].sample;
		log_struct_ptr->data[2]  = gyro_data_ptr->samples[2].sample;

		for(idx=0; idx<gyro_data_ptr->num_samples; idx++)
		{
			log_struct_ptr->samples[idx]  = gyro_data_ptr->samples[idx].sample;
		}

		//Commit log (also frees up the log packet memory)
		(void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
				log_struct_ptr);
	}
#endif
}


/*===========================================================================

FUNCTION:   sns_dd_bmg160_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  - Requests a single sample of sensor data from each of the specified
  sensors. Data is returned immediately after being read from the
  sensor, in which case data[] is populated in the same order it was
  requested,

  @param[in]  dd_handle    Handle to a driver instance.
  @param[in] sensors       List of sensors for which data isrequested.
  @param[in] num_sensors   Length of @a sensors.
  @param[in] memhandler    Memory handler used to dynamically allocate
  output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements
  must match @a num_sensors.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Update the each sample status after a check for bound is done
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_bmg160_get_data(
		sns_ddf_handle_t        dd_handle,
		sns_ddf_sensor_e        sensors[],
		uint32_t                num_sensors,
		sns_ddf_memhandler_s*   memhandler,
		sns_ddf_sensor_data_s*  data[])
{
	uint8_t i;
	sns_ddf_status_e stat;
	sns_ddf_sensor_data_s *data_ptr;
	sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;

	/* Sanity check*/
	for (i = 0; i < num_sensors; i++)
	{
		if (sensors[i] != SNS_DDF_SENSOR_GYRO)
		{
			return SNS_DDF_EINVALID_PARAM;
		}
	}

	/* this is a sync driver, so try to read data now */
	if ((stat = sns_dd_bmg160_get_data_all(
					(sns_dd_bmg160_state_t*)dd_handle)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	if ((data_ptr = sns_ddf_memhandler_malloc(memhandler,
					(num_sensors)*(sizeof(sns_ddf_sensor_data_s)))) == NULL)
	{
		return SNS_DDF_ENOMEM;
	}


	*data = data_ptr;

	for (i=0; i <num_sensors; i++)
	{
		data_ptr[i].sensor = sensors[i];
		data_ptr[i].status = SNS_DDF_SUCCESS;
		data_ptr[i].timestamp = sns_ddf_get_timestamp();

		if (sensors[i] == SNS_DDF_SENSOR_GYRO)
		{
			if ((data_ptr[i].samples = sns_ddf_memhandler_malloc(memhandler,
							SDD_GYRO_NUM_AXES*sizeof(sns_ddf_sensor_sample_s))) == NULL)
			{
				return SNS_DDF_ENOMEM;
			}


			data_ptr[i].samples[0].sample  = state->data_cache[SDD_GYRO_X];
			data_ptr[i].samples[1].sample  = state->data_cache[SDD_GYRO_Y];
			data_ptr[i].samples[2].sample  = state->data_cache[SDD_GYRO_Z];

			data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;
			data_ptr[i].samples[1].status = SNS_DDF_SUCCESS;
			data_ptr[i].samples[2].status = SNS_DDF_SUCCESS;

			data_ptr[i].num_samples = SDD_GYRO_NUM_AXES;
		}

	}

	return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_bmg160_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - resolution info is from the device data sheet.

  @param[in]  dd_handle   Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in] memhandler  Memory handler used to dynamically allocate
  output parameters, if applicable.
  @param[out] value      Pointer that this function will allocate or set
  to the attribute's value.
  @param[out] num_elems  Number of elements in @a value.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
  */
/*=========================================================================*/

static sns_ddf_status_e sns_dd_bmg160_get_attr(
		sns_ddf_handle_t       dd_handle,
		sns_ddf_sensor_e       sensor,
		sns_ddf_attribute_e    attrib,
		sns_ddf_memhandler_s*  memhandler,
		void**                 value,
		uint32_t*              num_elems)
{
	switch (attrib)
	{
		case SNS_DDF_ATTRIB_POWER_INFO:
			{
				sns_ddf_power_info_s* power_attrib;
				if ((*value = sns_ddf_memhandler_malloc(memhandler,
								sizeof(sns_ddf_power_info_s))) == NULL)
				{
					return SNS_DDF_ENOMEM;
				}
				power_attrib = *value;
				power_attrib->active_current = 5000;//5mA
				power_attrib->lowpower_current = 5;//5uA max
				*num_elems = 1;
			}
			break;

		case SNS_DDF_ATTRIB_RANGE:
			{
				sns_ddf_range_s *device_ranges;


				if ((*value = sns_ddf_memhandler_malloc(memhandler,
								SNS_DD_GYRO_MAX_RANGE*sizeof(sns_ddf_range_s)))
						== NULL)
				{
					return SNS_DDF_ENOMEM;
				}
				device_ranges = *value;
				*num_elems = 5;
				device_ranges[0].min = BOSCH_GYRO_SENSOR_RANGE_125_MIN;
				device_ranges[0].max = BOSCH_GYRO_SENSOR_RANGE_125_MAX;
				device_ranges[1].min = BOSCH_GYRO_SENSOR_RANGE_250_MIN;
				device_ranges[1].max = BOSCH_GYRO_SENSOR_RANGE_250_MAX;
				device_ranges[2].min = BOSCH_GYRO_SENSOR_RANGE_500_MIN;
				device_ranges[2].max = BOSCH_GYRO_SENSOR_RANGE_500_MAX;
				device_ranges[3].min = BOSCH_GYRO_SENSOR_RANGE_1000_MIN;
				device_ranges[3].max = BOSCH_GYRO_SENSOR_RANGE_1000_MAX;
				device_ranges[4].min = BOSCH_GYRO_SENSOR_RANGE_2000_MIN;
				device_ranges[4].max = BOSCH_GYRO_SENSOR_RANGE_2000_MAX;
			}
			break;


		case SNS_DDF_ATTRIB_LOWPASS:
			{
				uint8_t i;
				sns_ddf_lowpass_freq_t *freq_set;
				*num_elems = 8;
				if ((*value = sns_ddf_memhandler_malloc(memhandler,(*num_elems)*sizeof(sns_ddf_lowpass_freq_t))) == NULL)
				{
					return SNS_DDF_ENOMEM;
				}
				freq_set = *value;


				*num_elems = 8;
				for(i=0; i<*num_elems; i++)
				{
					*(freq_set+i) = sns_dd_gyro_freq[i];
				}
			}
			break;

		case SNS_DDF_ATTRIB_RESOLUTION_ADC:
			{
				sns_ddf_resolution_adc_s *device_res;
				if ((*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_adc_s))) == NULL)
				{
					return SNS_DDF_ENOMEM;
				}

				device_res = *value;

				*num_elems = 1;

				device_res->bit_len = 16;
				device_res->max_freq = BMG160_CONFIG_MAX_FREQ;
			}
			break;

		case SNS_DDF_ATTRIB_RESOLUTION:
			{
				sns_ddf_resolution_t *res;
				sns_dd_bmg160_state_t *state = dd_handle;

				res = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t));
				if (NULL == res)
					return SNS_DDF_ENOMEM;

				*res = FX_FLTTOFIX_Q16((4000>>state->range) * PI / 180 / 65536);
				*value = res;
				*num_elems = 1;

				return SNS_DDF_SUCCESS;
			}


		case SNS_DDF_ATTRIB_DELAYS:
			{
				sns_ddf_delays_s *device_delay;
				if ((*value = sns_ddf_memhandler_malloc(memhandler,
								sizeof(sns_ddf_delays_s))) == NULL)
				{
					return SNS_DDF_ENOMEM;
				}
				device_delay = *value;
				*num_elems = 1;

				device_delay->time_to_active = 30; // 30ms is the startup time
				device_delay->time_to_data = 30;   //30 ms
			}
			break;


		case SNS_DDF_ATTRIB_DEVICE_INFO:
			{
				sns_ddf_device_info_s *device_info;
				sns_dd_bmg160_state_t *state = dd_handle;

				if ((*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_device_info_s))) == NULL)
				{
					return SNS_DDF_ENOMEM;
				}
				device_info = *value;
				*num_elems = 1;
				if (BMG160_DEV_SEL_NUM_BMI058 == state->dev_select) {
					device_info->model = "BMI058";
				} else if (BMG160_DEV_SEL_NUM_BMI055 == state->dev_select) {
					device_info->model = "BMI055";
				} else {
					device_info->model = "BMG160";
				}
				device_info->vendor = "BOSCH";
				device_info->name = "Gyroscope";
				device_info->version = 1;
			}
			break;

		case SNS_DDF_ATTRIB_BIAS:
			{
				sns_dd_bmg160_state_t *state = dd_handle;

				if ((*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_dd_bmg160_state_t))) == NULL)
				{
					return SNS_DDF_ENOMEM;
				}

				if (sensor == SNS_DDF_SENSOR_TEMP)
					return SNS_DDF_EINVALID_PARAM;

				*value = state->bias;
				*num_elems = 3;

				return SNS_DDF_SUCCESS;
			}

		case SNS_DDF_ATTRIB_ODR:
			{
				uint32_t *odr_ptr;
				sns_dd_bmg160_state_t *state = dd_handle;

				odr_ptr = &state->odr_reported;
				*value = odr_ptr;
				*num_elems = 1;

				return SNS_DDF_SUCCESS;
			}

		case SNS_DDF_ATTRIB_FIFO:
			{
				if (sensor == SNS_DDF_SENSOR_GYRO)
				{
					// for GYRO
					sns_ddf_fifo_attrib_get_s *fifo_attrib_info = sns_ddf_memhandler_malloc(
							memhandler ,sizeof(sns_ddf_fifo_attrib_get_s));
					if (NULL == fifo_attrib_info)
						return SNS_DDF_ENOMEM;

					fifo_attrib_info->is_supported = true;
					fifo_attrib_info->is_sw_watermark = false;
					fifo_attrib_info->max_watermark = 100;
					fifo_attrib_info->share_sensor_cnt = 0;
					fifo_attrib_info->share_sensors[0] =  NULL;
					*value = fifo_attrib_info;
					*num_elems = 1;

					return SNS_DDF_SUCCESS;
				}
				else
				{
					return SNS_DDF_EINVALID_ATTR;
				}

				break;
			}

		default:
			return SNS_DDF_EINVALID_PARAM;
	}

	return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_bmg160_reset_fifo(
		sns_dd_bmg160_state_t *state
		)
{
	sns_ddf_status_e stat;
	uint8_t readbyte = 0;
	uint8_t  out;

	if ((stat = sns_ddf_read_port(state->port_handle,
					BMG160_FIFO_CGF0_ADDR,
					&readbyte,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	if ((stat = sns_ddf_write_port(state->port_handle,
					BMG160_FIFO_CGF0_ADDR,
					&readbyte,
					1,
					&out)) != SNS_DDF_SUCCESS)
	{
		return stat;
	}

	return stat;

}

static void sns_dd_bmg160_interrupt_handler(
		sns_ddf_handle_t        dd_handle,
		uint32_t                gpio_num,
		sns_ddf_time_t          timestamp)
{
	sns_ddf_status_e status;
	sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
	uint8_t int_stat1;
	int in_irq;
	uint8_t f_stat;
	uint8_t f_count;
	uint8_t f_overrun;
	uint8_t int_stat_fifo = 0;
#ifdef BMG160_DEBUG
	uint8_t int_stat_drdy = 0;
#endif // BMG160_DEBUG

	uint8_t dri_mockup_wmi = 0;
	uint8_t downsampling_factor = (state->odr_hw / state->odr_reported);

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_interrupt_handler %d %d %d",
			55555550, timestamp, gpio_num);
	if (gpio_num != ((sns_dd_bmg160_state_t*)dd_handle)->gpio_num) {
		return;
	}

	state->ts_irq = timestamp;

#if BMG160_CONFIG_DRI_MOCKUP_WMI
	if ((state->en_fifo_int) && (1 == state->f_wml)) {
		dri_mockup_wmi = 1;
	}
#endif
	if ((state->en_dri) || (dri_mockup_wmi)) {
		//this is to reduce false drdy interrupts rate
		if (BMG160_ABS(((int32_t)timestamp - (int32_t)state->ts_last_drdy) -
					state->ticks_per_ms * (1000 / state->odr_hw))
				<= (int32_t)(state->ticks_per_ms * (1000 / state->odr_hw)
					* BMG160_CONFIG_TOLERANCE_DRYD_TIME_ACCURACY)) {
			state->num_irq_drdy++;

			state->ts_last_drdy = timestamp;
		}

		if (0 == (state->num_irq_drdy % downsampling_factor)) {
			if (dri_mockup_wmi) {
				status = sns_ddf_smgr_notify_event(state->smgr_handle,
						SNS_DDF_SENSOR_GYRO,
						SNS_DDF_EVENT_FIFO_WM_INT);
			}

			sns_dd_bmg160_get_data_all(state);

			state->f_frames_cache.samples[SDD_GYRO_X].sample = state->data_cache[SDD_GYRO_X];
			state->f_frames_cache.samples[SDD_GYRO_X].status = SNS_DDF_SUCCESS;
			state->f_frames_cache.samples[SDD_GYRO_Y].sample = state->data_cache[SDD_GYRO_Y];
			state->f_frames_cache.samples[SDD_GYRO_Y].status = SNS_DDF_SUCCESS;
			state->f_frames_cache.samples[SDD_GYRO_Z].sample = state->data_cache[SDD_GYRO_Z];
			state->f_frames_cache.samples[SDD_GYRO_Z].status = SNS_DDF_SUCCESS;

			state->f_frames_cache.status = SNS_DDF_SUCCESS;
			state->f_frames_cache.sensor = SNS_DDF_SENSOR_GYRO;

			/*! notice : the number samples value */
			state->f_frames_cache.num_samples = SDD_GYRO_NUM_AXES;
			sns_dd_bmg160_update_sample_ts(state, 1, 1 * downsampling_factor);

			status = sns_ddf_smgr_notify_data(state->smgr_handle,
					&state->f_frames_cache, 1);
			sns_dd_gyro_log_fifo(&state->f_frames_cache);
		}
	}

	if (state->en_fifo_int && (!dri_mockup_wmi)) {
		/* for FIFO, status1 does not need to be checked */
		int_stat1 = 0;
		if ((status = bmg160_get_interrupt_status_reg_1(&int_stat1)) != SNS_DDF_SUCCESS)
		{
			return;
		}
#ifdef BMG160_DEBUG
		int_stat_drdy = BMG160_GET_BITSLICE(int_stat1, BMG160_INT_STATUS1_DATA_INT);
#endif // BMG160_DEBUG
		int_stat_fifo = BMG160_GET_BITSLICE(int_stat1, BMG160_INT_STATUS1_FIFO_INT);

		f_stat = 0;
		if ((status = bmg160_get_fifostatus_reg(&f_stat))
				!= SNS_DDF_SUCCESS) {
			return;
		}

		f_count = BMG160_GET_BITSLICE(f_stat,BMG160_FIFO_STATUS_FRAME_COUNTER);
		f_overrun = BMG160_GET_BITSLICE(f_stat, BMG160_FIFO_STATUS_OVERRUN);

		if (f_overrun) {
			status = sns_ddf_smgr_notify_event(state->smgr_handle,
					SNS_DDF_SENSOR_GYRO,
					SNS_DDF_EVENT_FIFO_OVERFLOW);
			BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_interrupt_handler SNS_DDF_EVENT_FIFO_OVERFLOW0 %d %d %d",
					55555551, 1, f_count);
			sns_dd_bmg160_reset_fifo(state);
			return;
		}

#ifdef BMG160_DEBUG
		BMG160_MSG_3_P(ERROR, "sns_dd_bmg160_interrupt_handler <---1---> int_stat_fifo= %d  int_stat_drdy= %d f_count =%d",
				int_stat_fifo, int_stat_drdy, f_count);
#endif // BMG160_DEBUG

		if (int_stat_fifo
				|| (f_count >= state->f_wml)
		   ) {
			status = sns_ddf_smgr_notify_event(state->smgr_handle,
					SNS_DDF_SENSOR_GYRO,
					SNS_DDF_EVENT_FIFO_WM_INT);
		}

		in_irq = 1;
		while (int_stat_fifo
				|| (f_count >= state->f_wml)
		      ) {
			f_count = f_count / state->f_wml * state->f_wml;
#if 0
			if (f_count < 2 * state->f_wml) {
				f_count = state->f_wml;
			}
#endif
			bmg160_fifo_data_read_out_frames(state, f_count, in_irq);
			in_irq = 0;

			if (state->f_frames_cache.num_samples > 0) {
				status = sns_ddf_smgr_notify_data(state->smgr_handle,
						&state->f_frames_cache, 1);
				sns_dd_gyro_log_fifo(&state->f_frames_cache);

			}
			//poll the status and frame counter again
			int_stat1 = 0;
			if ((status = bmg160_get_interrupt_status_reg_1(&int_stat1)) != SNS_DDF_SUCCESS)
			{
				return;
			}
#ifdef BMG160_DEBUG
			int_stat_drdy = BMG160_GET_BITSLICE(int_stat1, BMG160_INT_STATUS1_DATA_INT);
#endif // BMG160_DEBUG
			int_stat_fifo = BMG160_GET_BITSLICE(int_stat1, BMG160_INT_STATUS1_FIFO_INT);

			f_stat = 0;
			if ((status = bmg160_get_fifostatus_reg(&f_stat))
					!= SNS_DDF_SUCCESS) {
				return;
			}

			f_count = BMG160_GET_BITSLICE(f_stat,BMG160_FIFO_STATUS_FRAME_COUNTER);
			f_overrun = BMG160_GET_BITSLICE(f_stat, BMG160_FIFO_STATUS_OVERRUN);

#ifdef BMG160_DEBUG
			BMG160_MSG_3_P(ERROR, "sns_dd_bmg160_interrupt_handler <---2---> int_stat_fifo= %d  int_stat_drdy= %d f_count =%d",
					int_stat_fifo, int_stat_drdy, f_count);
#endif //BMG160_DEBUG
		}
	}

	return;
}

static void sns_dd_gyro_bmg160_test_notify(
		sns_ddf_handle_t handle,
		sns_ddf_sensor_e sensor,
		sns_ddf_status_e status,
		uint32_t err,
		q16_t* biases)
{
	static const uint8_t num_axes = 3;
	sns_dd_bmg160_state_t *state = handle;
	q16_t* biases_ptr;
	uint8_t i;

	sns_ddf_smgr_notify_test_complete(state->smgr_handle,
			sensor,
			status,err);
	if (status == SNS_DDF_SUCCESS)
	{
		biases_ptr = state->bias;

		for (i = 0; i < num_axes; i++)
			biases_ptr[i] = biases[i];

		sns_ddf_smgr_notify_event(state->smgr_handle,
				sensor,
				SNS_DDF_EVENT_BIAS_READY);
	}

}

static sns_ddf_status_e sns_dd_gyro_bmg160_run_test_self(
		sns_dd_bmg160_state_t   *state,
		sns_ddf_sensor_e        sensor_type,
		uint32_t                *err)
{
	sns_ddf_status_e        status = SNS_DDF_SUCCESS;
	uint8_t                 si_buf = 0;
	uint8_t                 bytes_read;

	if ((status = sns_ddf_read_port(state->port_handle,
					BMG160_CHIP_ID,
					&si_buf,
					1,
					&bytes_read)) != SNS_DDF_SUCCESS) {
		return status;
	}

	if (!(SENSOR_CHIP_ID_BMG == si_buf)) {
		*err = si_buf;
		return SNS_DDF_EDEVICE;
	}

	*err = 0;
	return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_gyro_bmg160_self_test(
		sns_ddf_handle_t dd_handle,
		sns_ddf_sensor_e sensor,
		sns_ddf_test_e test,
		uint32_t* err)
{
	sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
	sns_ddf_status_e stat;
	uint8_t out;

	unsigned char read_value = 0;
	unsigned char timeout = 0;
	uint8_t bosch_write_buffer = 0;
	sns_ddf_status_e status;
	sns_dd_inertial_test_config_s test_config;

	*err = 0;

	if (test == SNS_DDF_TEST_OEM)
	{
		test_config.sample_rate =  100;
		test_config.num_samples =  64;
		test_config.max_variance = (int64_t)(FX_CONV((FX_FLTTOFIX_Q16(0.1)),16,32));
		test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(0.087);
		test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(0.087);
		test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(0.087);

		state->soft_reset = 1;
		state->test_init = 1;
		sns_dd_bmg160_reset(dd_handle);
		state->test_init = 0;
		state->soft_reset = 0;

		status = sns_dd_inertial_test_run(
				sensor,
				dd_handle,
				&sns_bmg160_driver_fn_list,
				&test_config,
				&sns_dd_gyro_bmg160_test_notify);

		if (status != SNS_DDF_SUCCESS)
			return status;

		return SNS_DDF_PENDING;

	}
	else if (test == SNS_DDF_TEST_SELF_SW)
	{
		if ((stat = BMG_CALL_API(set_soft_reset)()) != SNS_DDF_SUCCESS)
		{
			return stat;
		}
		sns_ddf_delay(40000);//40ms

		stat = SNS_DDF_EINVALID_TEST;
		if ((stat = sns_ddf_read_port(state->port_handle,
						BMG160_FPGA_REV_ADDR,
						&read_value,
						1,
						&out)) != SNS_DDF_SUCCESS)
		{
			*err = 1; return stat;
		}

		if ((read_value&0x10)==0x10){

			bosch_write_buffer = 0x01;
			if ((stat = sns_ddf_write_port(state->port_handle,
							BMG160_FPGA_REV_ADDR,
							&bosch_write_buffer,
							1,
							&out)) != SNS_DDF_SUCCESS)
			{
				*err = 2; return stat;
			}

			do {
				sns_ddf_delay(2000);
				if ((stat = sns_ddf_read_port(state->port_handle,
								BMG160_FPGA_REV_ADDR,
								&read_value,
								1,
								&out)) != SNS_DDF_SUCCESS)
				{
					*err = 3;
					return stat;
				}

				timeout++;
				if (timeout == 100) {
					stat = SNS_DDF_EFAIL;
					return stat;
				};

			} while ((read_value&0x02)!=0x02);

			if ((stat = sns_ddf_read_port(state->port_handle,
							BMG160_FPGA_REV_ADDR,
							&read_value,
							1,
							&out)) != SNS_DDF_SUCCESS)
			{
				*err = 4;
				return stat;
			}

			if ((read_value&0x04)!=0x04) {

				stat = SNS_DDF_SUCCESS;

			} else {
				stat = SNS_DDF_EFAIL;
			}

		} else {
			stat = SNS_DDF_EFAIL;
		}

		state->soft_reset = 1;
		state->test_init = 0;
		sns_dd_bmg160_reset(dd_handle);
		state->soft_reset = 0;

		return stat;
	}
	else if (test == SNS_DDF_TEST_SELF) {
		stat = sns_dd_gyro_bmg160_run_test_self(state, sensor, err);
		return stat;
	}

	else
	{
		return SNS_DDF_EINVALID_TEST;
	}
}

static sns_ddf_status_e bmg160_parse_fifo_frame(
		unsigned char *data,
		struct bmg160_data *gyro,
		sns_dd_bmg160_state_t *state)
{
	sns_ddf_status_e comres = SNS_DDF_SUCCESS;

	switch (state->dev_select) {
		case BMG160_DEV_SEL_NUM_BMI058:
			data[0] = BMG160_GET_BITSLICE(data[0],BMG160_DATAY_LSB_VALUEY);
			gyro->datay = (BMG160_S16)((((BMG160_S16)((signed char)data[1])) << BMG160_SHIFT_8_POSITION) | (data[0]));

			data[2] = BMG160_GET_BITSLICE(data[2],BMG160_DATAX_LSB_VALUEX);
			gyro->datax = (BMG160_S16)((((BMG160_S16)((signed char)data[3])) << BMG160_SHIFT_8_POSITION) | (data[2]));
			break;
		default:
			data[0] = BMG160_GET_BITSLICE(data[0],BMG160_DATAX_LSB_VALUEX);
			gyro->datax = (BMG160_S16)((((BMG160_S16)((signed char)data[1])) << BMG160_SHIFT_8_POSITION) | (data[0]));

			data[2] = BMG160_GET_BITSLICE(data[2],BMG160_DATAY_LSB_VALUEY);
			gyro->datay = (BMG160_S16)((((BMG160_S16)((signed char)data[3])) << BMG160_SHIFT_8_POSITION) | (data[2]));
			break;
	}

	data[4] = BMG160_GET_BITSLICE(data[4],BMG160_DATAZ_LSB_VALUEZ);
	gyro->dataz = (BMG160_S16)((((BMG160_S16)((signed char)data[5])) << BMG160_SHIFT_8_POSITION) | (data[4]));

	return comres;
}

static void sns_dd_bmg160_update_sample_ts(
		sns_dd_bmg160_state_t *state,
		int in_irq,
		int number)
{
	sns_ddf_time_t start;
	sns_ddf_time_t end;
	sns_ddf_time_t interval;
	uint8_t flag_downsampling = (state->odr_hw > state->odr_reported);
	uint8_t downsampling_factor = (state->odr_hw / state->odr_reported);

	if (0 == number) {
		BMG160_MSG_2(HIGH, "invalid parameters: %d %d", in_irq, state->f_wml);
		return;
	}

	start = state->ts_start_first_sample;
	if (in_irq) {
		end = state->ts_irq;

		if (state->f_wml > 0) {
			interval = (sns_ddf_time_t)((int32_t)end -
					(int32_t)start) / state->f_wml;
		} else {
			interval = (sns_ddf_time_t)((int32_t)end -
					(int32_t)start) / number;
		}
	} else {
		end = sns_ddf_get_timestamp();

		interval = (sns_ddf_time_t)((int32_t)end -
				(int32_t)start) / number;
	}

	if (!flag_downsampling)  {
		state->f_frames_cache.timestamp = state->ts_start_first_sample +
			interval;
		state->f_frames_cache.end_timestamp = state->ts_start_first_sample +
			interval * number;
	} else {
		if (number >= downsampling_factor) {
			state->f_frames_cache.timestamp = state->ts_start_first_sample +
				interval * downsampling_factor;
			state->f_frames_cache.end_timestamp = state->ts_start_first_sample +
				interval * (number / downsampling_factor * downsampling_factor);
		}
	}

	//this works for trigger_fifo_data() as well
	state->ts_start_first_sample = end;
}


static sns_ddf_status_e bmg160_fifo_data_read_out_frames(
		sns_dd_bmg160_state_t *state,
		uint8_t f_count,
		int in_irq)
{
	int i;
	unsigned char fifo_data_out[BMG160_MAX_FIFO_LEVEL * BMG160_MAX_FIFO_F_BYTES] = {0};
	unsigned char *frame_pointer;
	unsigned char f_len = 6;
	//unsigned char fifo_datasel = 0;
	struct bmg160_data gyro_data = {0};
	sns_ddf_status_e stat;
	uint8_t flag_downsampling = (state->odr_hw > state->odr_reported);
	uint8_t downsampling_factor = (state->odr_hw / state->odr_reported);
	uint8_t frames_to_report = 0;


	sns_dd_bmg160_update_sample_ts(state, in_irq, f_count);

	if ((stat = bmg160_read_fifo_frames(state->port_handle,
					f_len, f_count, fifo_data_out))
			!= SNS_DDF_SUCCESS)
		return stat;

	BMG160_MSG_3_P(ERROR, "read_out_frames f_count = %d first_ts = %d end_ts = %d",
			f_count, state->f_frames_cache.timestamp, state->f_frames_cache.end_timestamp);
	//parse the frames
	frame_pointer = fifo_data_out;

#if 0
	buffer = 0x80;
	stat = sns_ddf_write_port(
			state->port_handle, BMA2X2_INT_CTRL_REG,
			&buffer, 1, &read_count);
#endif
	for (i = 0; i < f_count; i++, frame_pointer += f_len) {
		if (flag_downsampling) {
			if (0 != ((i + 1) % downsampling_factor)) {
				continue;
			}
		}

		bmg160_parse_fifo_frame(frame_pointer, &gyro_data, state);

		sns_dd_bmg160_convertNcorrect_sample(state, &gyro_data, state->data_cache_fifo);

		state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_X].sample = state->data_cache_fifo[SDD_GYRO_X];
		state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_X].status = SNS_DDF_SUCCESS;
		state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Y].sample = state->data_cache_fifo[SDD_GYRO_Y];
		state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Y].status = SNS_DDF_SUCCESS;
		state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Z].sample = state->data_cache_fifo[SDD_GYRO_Z];
		state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Z].status = SNS_DDF_SUCCESS;

		frames_to_report++;
		//TODO: bias not applied

	}

	state->f_frames_cache.status = SNS_DDF_SUCCESS;
	state->f_frames_cache.sensor = SNS_DDF_SENSOR_GYRO;

	/*! notice : the number samples value */
	state->f_frames_cache.num_samples = frames_to_report * SDD_GYRO_NUM_AXES;

	return stat;
}

/**
 * @brief Retrieves a set of sensor data. Asynchronous API
 *
 * Requests sample of sensor data from the specified sensor.
 *
 * @note If a sensor has failed or
 *		 isn't available, @a sns_ddf_sensor_data_s.status must be used to
 *		 reflect this status.
 *
 * @param[in]  dd_handle	   Handle to a driver instance.
 * @param[in]  sensor		   sensor for which data is requested.
 *
 * @param[in]  num_samples	   number of samples to retrieve as available. Drain the FIFO if value is set to Zero.
 * @param[in]  trigger now	   trigger nofity fifo data now or
 *		 later when trigger_now is set to true.
 *
 *
 * @return SNS_DDF_SUCCESS	   if data was populated successfully.
 *		   via sns_ddf_smgr_data_notify() or if trigger_now is
 *		   set to false; Otherwise a specific error code is
 *		   returned.
 *
 * @see sns_ddf_data_notify_data() as this will be used to report the data.
 */

sns_ddf_status_e sns_dd_bmg160_triger_fifo_data(
		sns_ddf_handle_t    dd_handle,
		sns_ddf_sensor_e    sensor,
		uint16_t            num_samples,
		bool                trigger_now)
{
	sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
	sns_ddf_status_e stat;
	uint8_t f_count = 0;
	uint8_t downsampling_factor = (state->odr_hw / state->odr_reported);

	BMG160_MSG_3_F(ERROR, "sns_dd_bmg160_triger_fifo_data %d num_samples = %d trigger_now = %d",
			55555521, num_samples, trigger_now);

	if (0 == trigger_now) {
		return SNS_DDF_SUCCESS;
	}


	if ((stat = BMG_CALL_API(get_fifo_framecount)(&f_count)) != SNS_DDF_SUCCESS) {
		return stat;
	}

	if (0 == num_samples) {
	} else {
		if (num_samples * downsampling_factor < f_count) {
			f_count = num_samples * downsampling_factor;
		}
	}

	if (f_count > 0) {
		bmg160_fifo_data_read_out_frames(state, f_count, 0);
		/*! notice : the number data len */
		if (state->f_frames_cache.num_samples > 0) {
			stat = sns_ddf_smgr_notify_data(state->smgr_handle,
					&state->f_frames_cache, 1);
			sns_dd_gyro_log_fifo(&state->f_frames_cache);
		}
	}

	return stat;
}



sns_ddf_status_e sns_dd_bmg160_probe(
		sns_ddf_device_access_s*  device_info,
		sns_ddf_memhandler_s*     memhandler,
		uint32_t*                 num_sensors,
		sns_ddf_sensor_e**        sensors)
{
	sns_ddf_status_e status;
	sns_ddf_handle_t port_handle;

	uint8_t si_buf;
	uint8_t bytes_read;
	*num_sensors = 0;
	*sensors = NULL;

	status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
	if (status != SNS_DDF_SUCCESS)
	{
		return status;
	}

	if ((status = sns_ddf_read_port(port_handle,
					BMG160_CHIP_ID,
					&si_buf,
					1,
					&bytes_read)) != SNS_DDF_SUCCESS)
	{
		sns_ddf_close_port(port_handle);
		return status;
	}

	sns_ddf_close_port(port_handle);

	if (SENSOR_CHIP_ID_BMG == si_buf)
	{
		*num_sensors = 1;
		*sensors = sns_ddf_memhandler_malloc(memhandler,
				sizeof(sns_ddf_sensor_e) * (*num_sensors));
		if (*sensors != NULL)
		{
			(*sensors)[0] = SNS_DDF_SENSOR_GYRO;
			status = SNS_DDF_SUCCESS;
		}
		else
		{
			status = SNS_DDF_ENOMEM;
		}
		return status;
	}
	else
	{
		/* Unknown device ID. Return now with nothing detected */
		return SNS_DDF_SUCCESS;
	}
}


sns_ddf_driver_if_s sns_bmg160_driver_fn_list =
{
	.init                 = &sns_dd_bmg160_init,
	.get_data             = &sns_dd_bmg160_get_data,
	.set_attrib           = &sns_dd_bmg160_set_attr,
	.get_attrib           = &sns_dd_bmg160_get_attr,
	.handle_irq           = &sns_dd_bmg160_interrupt_handler,
	.reset                = &sns_dd_bmg160_reset,
	.run_test             = &sns_dd_gyro_bmg160_self_test,
	.enable_sched_data    = &sns_dd_bmg160_enable_sched_data,
	.probe                = &sns_dd_bmg160_probe,
	.trigger_fifo_data    = &sns_dd_bmg160_triger_fifo_data
};

