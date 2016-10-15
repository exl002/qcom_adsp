/*==============================================================================

FILE:      ABT_data.c

DESCRIPTION: This file contains target/platform specific configuration data.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.adsp/2.2/buses/icb/src/8084/ABT_data.c#1 $ 
$DateTime: 2013/06/10 03:47:48 $
$Author: coresvc $
$Change: 3892863 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2013/05/30  pm     Port to APQ8084  
2013/04/16  pm     Added interrupt priority  
2012/10/04  av     Support for disabling ABT 
2012/05/31  av     Created
 
        Copyright (c) 2013 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "ABTimeout.h"
#include "ABTimeout_HWIO.h"

/*============================================================================
                      TARGET AND PLATFORM SPECIFIC DATA
============================================================================*/

/* Base address for devices */
#define ABT_LPASS0_BASE_ADDR  LPASS_AHBE_TIME_REG_BASE
#define ABT_LPASS1_BASE_ADDR  LPASS_AHBI_TIME_REG_BASE 

/* Bit Mask for ABT Slaves */
#define ABT_LPASS0_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT_0_SHFT
#define ABT_LPASS1_BMSK   HWIO_TCSR_TIMEOUT_INTR_STATUS_LPASS_IRQ_OUT_AHB_TIMEOUT_1_SHFT

/* ABT Slave CLK Name */
#define ABT_LPASS0_CLK   "audio_wrapper_timeout_clk"
#define ABT_LPASS1_CLK   "audio_core_timeout_clk"

/* Timeout Interrupt Register Address */
#define ABT_TIMEOUT_INTR_LPASS_ENABLE  HWIO_TCSR_TIMEOUT_INTR_LPASS_ENABLE_ADDR
#define ABT_TIMEOUT_INTR_STATUS        HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR 
#define ABT_TIMEOUT_SLAVE_GLB_EN       HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR

/* TCSR Summary Interrupt Vectors */
#define ABT_TCSR_LPASS_INTR_VECTOR     90

/* LPASS Interrupt Priority (Note:  This varies with the processor, as DAL    */
/*                                  passes this through to the underlying     */
/*                                  kernel/OS                                 */
#define ABT_LPASS_INTR_PRIORITY     1


/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/*---------------------------------------------------------------------------*/
/*          Properties data for device ID  = "/dev/ABTimeout"                */
/*---------------------------------------------------------------------------*/

/* ABT Configuration Data*/
static ABT_slave_info_type ABT_cfgdata[] = 
{ 
//ABT_SLAVE_INFO(  name, sl_en, int_en, to_val)
  ABT_SLAVE_INFO(LPASS0,  TRUE,   TRUE,   0xFF),//  LPASS AHBE
  ABT_SLAVE_INFO(LPASS1,  TRUE,   TRUE,   0xFF) //  LPASS AHBI
};

/* ABT Platform Data type */
static ABT_platform_info_type ABT_platform_info =
{
    "LPASS",                               /* Image name */
    (void*)ABT_TIMEOUT_INTR_LPASS_ENABLE,  /* INTR Enable address */
    (void*)ABT_TIMEOUT_INTR_STATUS,        /* INTR Status Register address */
    (void*)ABT_TIMEOUT_SLAVE_GLB_EN,       /* ABT Slave global en address */
    ABT_TCSR_LPASS_INTR_VECTOR,            /* ABT TCSR Summary interrupt vector */
    ABT_LPASS_INTR_PRIORITY,               /* ABT LPASS Interrupt priority */
};


/* ABT Configuration Property Data*/
ABT_propdata_type ABT_propdata = 
{
    /* Length of the config  data array */
    sizeof(ABT_cfgdata)/sizeof(ABT_slave_info_type), 
    /* Pointer to config data array */ 
    ABT_cfgdata,
    /* Pointer to platform info data */ 
    &ABT_platform_info                                    
};

