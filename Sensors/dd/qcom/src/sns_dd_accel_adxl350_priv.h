/*===========================================================================

         S E N S O R S    A C C E L E R O M E T E R    D R I V E R

DESCRIPTION

  ADXL350 Accelerometer Sensor Registers address and thier format.


Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/ssc.adsp/2.4.1/adsp_proc/Sensors/dd/qcom/src/sns_dd_accel_adxl350_priv.h#1 $


when           who     what, where, why
----------     ---     ------------------------------------------------------
12/13/10     AS      Draft
==========================================================================*/

#ifndef _ADXL350_H_
#define _ADXL350_H_

/*
 * ADXL350 Accelerometer Sensor Registers address
 */

#define ADXL350_REG_DEVICE_ID                   0x00

#define ADXL350_REG_TAP_THRESHOLD               0x1D
#define ADXL350_REG_OFFSET_X                    0x1E
#define ADXL350_REG_OFFSET_Y                    0x1F
#define ADXL350_REG_OFFSET_Z                    0x20
#define ADXL350_REG_TAP_DURATION                0x21
#define ADXL350_REG_TAP_LATENCY                 0x22
#define ADXL350_REG_TAP_WINDOW                  0x23
#define ADXL350_REG_ACTIVITY_THRESHOLD          0x24
#define ADXL350_REG_INACTIVITY_THRESHOLD        0x25
#define ADXL350_REG_INACTIVITY_TIME             0x26
#define ADXL350_REG_ACTIVITY_INACTIVITY_CONTROL 0x27
#define ADXL350_REG_FREE_FALL_THRESHOLD         0x28
#define ADXL350_REG_FREE_FALL_TIME              0x29
#define ADXL350_REG_TAP_CONTROL                 0x2A
#define ADXL350_REG_TAP_STATUS                  0x2B
#define ADXL350_REG_RATE_AND_POWER_MODE         0x2C
#define ADXL350_REG_POWER_SAVE                  0x2D
#define ADXL350_REG_INTERRUPT_ENABLE            0x2E
#define ADXL350_REG_INTERRUPT_MAP               0x2F
#define ADXL350_REG_INTERRUPT_SOURCE            0x30
#define ADXL350_REG_DATA_FORMAT                 0x31
#define ADXL350_REG_DATA_X0                     0x32
#define ADXL350_REG_DATA_X1                     0x33
#define ADXL350_REG_DATA_Y0                     0x34
#define ADXL350_REG_DATA_Y1                     0x35
#define ADXL350_REG_DATA_Z0                     0x36
#define ADXL350_REG_DATA_Z1                     0x37
#define ADXL350_REG_FIFO_CONTROL                0x38
#define ADXL350_REG_FIFO_STATUS                 0x39

/*
 * ADXL350 Accelerometer Sensor Registers format
 */

/* ADXL350_REG_DEVICE_ID */
#define ADXL350_DEVICE_ID               0xE5
#define ADXL346_DEVICE_ID               0xE6

/* ADXL350_REG_ACTIVITY_INACTIVITY_CONTROL */
#define ADXL350_INACT_Z_ENABLE          0x01
#define ADXL350_INACT_Y_ENABLE          0x02
#define ADXL350_INACT_X_ENABLE          0x04
#define ADXL350_INACT_ACDC              0x08
#define ADXL350_ACT_Z_ENABLE            0x10
#define ADXL350_ACT_Y_ENABLE            0x20
#define ADXL350_ACT_X_ENABLE            0x40
#define ADXL350_ACT_ACDC                0x80

/* ADXL350_REG_TAP_CONTROL */
#define ADXL350_TAP_Z_ENABLE            0x01
#define ADXL350_TAP_Y_ENABLE            0x02
#define ADXL350_TAP_X_ENABLE            0x04
#define ADXL350_SUPRESS_DOUBLE_TAP      0x08

/* ADXL350_REG_TAP_STATUS */
#define ADXL350_TAP_Z_SOURCE            0x01
#define ADXL350_TAP_Y_SOURCE            0x02
#define ADXL350_TAP_X_SOURCE            0x04
#define ADXL350_ASLEEP                  0x08
#define ADXL350_ACT_Z_SOURCE            0x10
#define ADXL350_ACT_Y_SOURCE            0x20
#define ADXL350_ACT_X_SOURCE            0x40

/* ADXL350_REG_RATE_AND_POWER_MODE */
#define ADXL350_RATE_MASK               0x0F
#define ADXL350_LOW_POWER               0x10

/* ADXL350_REG_POWER_SAVE */
#define ADXL350_SLEEP_FREQ_MASK         0x03

#define ADXL350_POWER_MODE_MASK         0x0C
#define ADXL350_SLEEP                   0x04
#define ADXL350_MEASURE                 0x08 /* 0 = standby mode */
#define ADXL350_AUTO_SLEEP              0x10

#define ADXL350_LINK                    0x20

/* ADXL350_REG_INTERRUPT_ENABLE */
#define ADXL350_INT_ALL_MASK            0xFF

#define ADXL350_INT_OVERRUN             0x01
#define ADXL350_INT_WATERMARK           0x02
#define ADXL350_INT_FREE_FALL           0x04
#define ADXL350_INT_INACTIVITY          0x08
#define ADXL350_INT_ACTIVITY            0x10
#define ADXL350_INT_DOUBLE_TAP          0x20
#define ADXL350_INT_SINGLE_TAP          0x40
#define ADXL350_INT_DATA_READY          0x80

/* ADXL350_REG_DATA_FORMAT */
#define ADXL350_RANGE_MASK              0x03
#define ADXL350_LEFT_JUSTIFY            0x04
#define ADXL350_FULL_RES                0x08
#define ADXL350_INT_INVERT              0x20 /* 0=Active-Hight, 1=Active-Low */
#define ADXL350_SPI                     0x40
#define ADXL350_SELF_TEST               0x80

/* ADXL350_REG_FIFO_CONTROL */
/*
 * Number of samples needed for trigger a watermark interrupt.
 * Depends on the FIFO mode.
 */
#define ADXL350_SAMPLES_NUM_MASK        0x1F

#define ADXL350_TRIGGER_INT_NUM         0x20 /* 0=INT1, 1=INT2 */
#define ADXL350_FIFO_MODE_MASK          0xC0

/* ADXL350_REG_FIFO_STATUS */
#define ADXL350_ENTRIES_NUM_MASK        0x3F
#define ADXL350_FIFO_TRIG               0x80


#endif /* _ADXL350_H_ */



