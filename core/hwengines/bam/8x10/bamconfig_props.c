/**
  @file bamconfig_props.c
  @brief
  This file contains implementation of the interfaces that provide
  target specific base addresses & irq vector data to the BAM driver.

*/
/*
===============================================================================

                             Edit History

 $Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/23/13   SS      Created

===============================================================================
                   Copyright (c) 2013 QUALCOMM Technologies Inc.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
/* If DAL_APPS_IMAGE set the local define for the appropriate config */
#ifdef DAL_CONFIG_IMAGE_APPS
#ifndef BAM_APPS_CONFIG
#define BAM_APPS_CONFIG
#endif
#endif

#include "bamconfig_props.h"
#include "bamtgtcfg.h"
#ifdef BAM_XML_PROPS
#ifdef BAM_ADSP_CONFIG
#include "bamtgtcfgdata_lpass.h"
#elif BAM_MODEM_CONFIG
#include "bamtgtcfgdata_mpss.h"
#elif BAM_TZOS_CONFIG
#include "bamtgtcfgdata_tz.h"
#endif
#endif


#ifdef BAM_APPS_CONFIG

const bamclient_config_info_type  bam_blsp1_baseirq_info[] =
{
    { BAM_LITE_BLSP1_BASE, BAM_LITE_BLSP1_IRQVECTOR }
};
const bamclient_config_info_type  bam_crypto1_baseirq_info[] =
{
    { BAM_NDP_CRYPTO1_BASE, BAM_NDP_CRYPTO1_IRQVECTOR }
};
const bamclient_config_info_type  bam_qdss_baseirq_info[] =
{
    { BAM_NDP_QDSS_BASE, BAM_NDP_QDSS_IRQVECTOR }
};
const bamclient_config_info_type  bam_usb2_baseirq_info[] =
{
    { BAM_USB2_BASE, BAM_USB2_IRQVECTOR }
};
const bamclient_config_info_type  bam_sdc1_baseirq_info[] =
{
    { BAM_SDC1_BASE, BAM_SDC1_IRQVECTOR }
};

const bamclient_config_info_type  bam_sdc2_baseirq_info[] =
{
    { BAM_SDC2_BASE, BAM_SDC2_IRQVECTOR }
};
#elif BAM_MODEM_CONFIG

const bamclient_config_info_type  bam_blsp1_baseirq_info[] =
{
    { BAM_LITE_BLSP1_BASE, BAM_LITE_BLSP1_IRQVECTOR }
};
const bamclient_config_info_type  bam_crypto_baseirq_info[] =
{
    { BAM_NDP_MSS_CRYPTO_BASE, BAM_NDP_MSS_CRYPTO_IRQVECTOR }
};
const bamclient_config_info_type  bam_mssa2_baseirq_info[] =
{
    { BAM_NDP_MSS_A2_BASE, BAM_MSS_A2_IRQVECTOR }
};
#elif BAM_TZOS_CONFIG

const bamclient_config_info_type  bam_blsp1_baseirq_info[] =
{
    { BAM_LITE_BLSP1_BASE, BAM_IRQVECTOR_NONE }
};

const bamclient_config_info_type  bam_crypto1_baseirq_info[] =
{
    { BAM_NDP_CRYPTO1_BASE, BAM_IRQVECTOR_NONE }
};

#elif BAM_ADSP_CONFIG

const bamclient_config_info_type  bam_blsp1_baseirq_info[] =
{
    { BAM_LITE_BLSP1_BASE, BAM_LITE_BLSP1_IRQVECTOR }
};

const bamclient_config_info_type  bam_crypto1_baseirq_info[] =
{
    { BAM_NDP_CRYPTO1_BASE, BAM_NDP_CRYPTO1_IRQVECTOR }
};

#endif





















