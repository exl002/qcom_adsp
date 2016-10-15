/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
*
* File Name         : sns_dd_press_lps25h.h
* Authors           : Jianjian Huo
* Version           : V 1.0.4
* Date              : 04/10/2014
* Description       : LPS25H pressure sensor driver header file
*
********************************************************************************
* Copyright (c) 2014, STMicroelectronics.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of the STMicroelectronics nor the
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

#ifndef __LPS25H__
#define __LPS25H__

#include "fixed_point.h"

#define STM_LPS25H_PRESS_RANGE_MIN    FX_CONV_Q16(260,0)
#define STM_LPS25H_PRESS_RANGE_MAX    FX_CONV_Q16(1260,0)
#define STM_LPS25H_TEMP_RANGE_MIN     FX_CONV_Q16(0,0)
#define STM_LPS25H_TEMP_RANGE_MAX     FX_CONV_Q16(80,0)

#define STM_LPS25H_MAX_RES_PRESS      FX_FLTTOFIX_Q16(0.0002)
#define STM_LPS25H_MAX_RES_TEMP       FX_FLTTOFIX_Q16(0.002)

/**
 * Pressure sensor LPS25H output data rate
 * * This is bits [ODR2:ODR0] ([6:4]) in register CTRL_REG1(20h)
 * * The values at the end of name indicate pressure output rate first, temperature output rate second
 */
typedef enum 
{
    //STM_LPS25H_ODR_0_0 =           0x00,  /*Pressure: one shot, Temperature: one shot*/
    STM_LPS25H_ODR_1_1 =             0x01,  /*Pressure: 1Hz, Temperature: 1Hz*/
    STM_LPS25H_ODR_7_7 =             0x02,  /*Pressure: 7Hz, Temperature: 7Hz*/
    STM_LPS25H_ODR_12p5_12p5 =       0x03,  /*Pressure: 12.5Hz, Temperature: 12.5Hz*/
    STM_LPS25H_ODR_25_25 =       	 0x04,  /*Pressure: 25Hz, Temperature: 25Hz*/
} stm_lps25h_odr;

#define STM_LPS25H_ODR_NUM      4
#define STM_LPS25H_MAX_ODR      STM_LPS25H_ODR_25_25
#define STM_LPS25H_MAX_ODR_HZ	25

/**
 * Pressure sensor LPS25H I2C address
 */
#define STM_LPS25H_I2C_ADDR         0x5D

/**
 * Pressure sensor LPS25H WHO AM I register
 */
#define STM_LPS25H_WHO_AM_I_VALUE   0xBD

/**
 * Pressure sensor LPS25H register addresses
 */
#define STM_LPS25H_REF_P_XL             0x08
#define STM_LPS25H_REF_P_L              0x09
#define STM_LPS25H_REF_P_H              0x0A
#define STM_LPS25H_WHO_AM_I             0x0F
#define STM_LPS25H_RES_CONF             0x10
#define STM_LPS25H_RES_ALLOW            0x16
#define STM_LPS25H_CTRL_REG1            0x20
#define STM_LPS25H_CTRL_REG2            0x21
#define STM_LPS25H_CTRL_REG3            0x22
#define STM_LPS25H_CTRL_REG4            0x23
#define STM_LPS25H_INT_CFG              0x24
#define STM_LPS25H_INT_SOURCE           0x25
#define STM_LPS25H_STATUS_REG           0x27
#define STM_LPS25H_PRESS_OUT_XL         0x28
#define STM_LPS25H_PRESS_OUT_L          0x29
#define STM_LPS25H_PRESS_OUT_H          0x2A
#define STM_LPS25H_TEMP_OUT_L           0x2B
#define STM_LPS25H_TEMP_OUT_H           0x2C
#define STM_LPS25H_FIFO_CTRL            0x2E
#define STM_LPS25H_FIFO_STATUS          0x2F

#define STMERR_TEST_OK					0
#define STMERR_TEST_UNKNOWN				-1
#define STMERR_TEST_1ST_SENSOR_FAIL		1
#define STMERR_TEST_2ND_SENSOR_FAIL		2
#define STMERR_TEST_I2C_FAIL		2

#endif  /* __LPS25H__ */
