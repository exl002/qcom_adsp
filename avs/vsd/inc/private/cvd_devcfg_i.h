#ifndef __CVD_DEVCFG_I_H__
#define __CVD_DEVCFG_I_H__

/*
  Copyright (C) 2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/vsd/inc/private/cvd_devcfg_i.h#2 $
  $Author: coresvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "vss_public_if.h"

#ifndef WINSIM
  #include "mmpm.h"
#endif /* !WINSIM */

/****************************************************************************
 * CVD DEVICE CONFIGURATION BRIEF BACKGROUND                                *
 ****************************************************************************/

/**
  The CVD device configuration framework contains the following:
  1. Device configuration data structures (defined in cvd_devcfg_i.h).
  2. Device configuration data (stored in cvd_devcfg.c and cvd_devcfg.xml).
  3. Device configuration parser (implemented by cvd_devcfg_parser_i.h and 
     cvd_devcfg_parser.c).

  The CVD device configuration currently stores the following data:
  1. Voice use case <-> clock table (stored in cvd_devcfg.c).
     The table stores a number of clock levels. Each clock level specifies:
     - Per HW thread clock value in Hz.
     - Number of supported voice use cases for this clock level.
     - An array of supported voice use cases.

  2. MMPM core information which is required for MMPM registration
     (stored in cvd_devcfg.c):
     - Core ID.
     - Instance ID.

  3. MMPM bus bandwidth requirement (stored in cvd_devcfg.c):
     - Number of bus bandwidth requirements.
     - For each bus bandwidth requirement:
       - Bus route master port.
       - Bus route slave port.
       - Usage in bytes per second.
       - Usage percentage.
       - Usage type.

   4. Sleep latency in microsends (stored in the cvd_devcfg.xml). During voice
      call, the ADSP should not go to sleep (power collapse) unless it can wake
      up within this latency.

   5. Number of clock cycles required to execute 1000 instruction packets
      (stored in the cvd_devcfg.xml).

   The device configuration data is compiled and stored into a device
   configuration image for a particular chipset. The data is then queried
   during run-time by the CVD device configuration parser.

   Currently the device configuration parser client is MVM-CCM where it 
   queries the following information:
   1. MMPM core information, which is used to register with MMPM.
   2. Clock and bus bandwidth configurations for a particular voice use case.
      Based on the returned configurations, MVM-CCM can then request
      resources from MMPM.
*/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define CVD_DEVCFG_DEV_ID_NAME ( "QCOM_CVD" )
  /**< The CVD device config device ID name. */

/* Property names associated with the CVD_DEVCFG_DEV_ID_NAME. */
#define CVD_DEVCFG_CLOCK_TABLE_PROP_NAME ( "CLOCK_TABLE" )
#define CVD_DEVCFG_VOICE_USE_CASE_NA_VALUES_PROP_NAME ( "USE_CASE_NA_VAL" )
#define CVD_DEVCFG_MMPM_CORE_INFO_PROP_NAME ( "MMPM_CORE_INFO" )
#define CVD_DEVCFG_MMPM_BW_REQUIREMENT_PROP_NAME ( "MMPM_BW_REQUIREMENT" )
#define CVD_DEVCFG_MMPM_SLEEP_LATENCY_PROP_NAME ( "MMPM_SLEEP_LATENCY" )
#define CVD_DEVCFG_CYCLES_PER_THOUSAND_INSTR_PACKET_PROP_NAME ( "CYCLES_PER_KP_FACTOR" )

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

/* Defines a supported use case for a particular clock level. */
typedef struct cvd_devcfg_voice_use_case_t cvd_devcfg_supported_voice_use_case_t;

/* Defines the don't care value for each parameter for a voice use case. When 
 * a parameter for a supported voice use case in a particular clock level 
 * matches the don't care value, it means this parameter is a don't care (i.e.
 * any value for this parameter can be supported by the current clock level).
 */
typedef struct cvd_devcfg_voice_use_case_t cvd_devcfg_voice_use_case_na_values_t;

struct cvd_devcfg_voice_use_case_t
{
  uint16_t max_num_voice_sessions;
    /**< Max number of active voice sessions in the system. */
  uint16_t max_num_nb_streams;
    /**< Max number of narrow-band (8 KHz) streams in the system. */
  uint16_t max_num_wb_plus_streams;
    /**< Max number of wide-band (16 KHz) or higher streams in the system. */
  uint16_t max_num_nb_vocprocs;
    /**< Max number of narrow-band (8 KHz) vocprocs in the system. */
  uint16_t max_num_wb_plus_vocprocs;
    /**< Max number of wide-band (16 Khz) or higher vocprocs in the system. */
  uint32_t tx_topology_id;
    /**< Tx vocproc topology ID. Applicable only if there is a single active  
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint32_t rx_topology_id;
    /**< Rx vocproc topology ID. Applicable only if there is a single active  
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint16_t vfr_mode;
    /**< The VFR mode. Applicable only if there is a single active voice 
         session in the system, which has a single vocproc and/or stream 
         attached to it. It is ignored otherwise.
         The supported values:\n
         #VSS_ICOMMON_VFR_MODE_SOFT \n
         #VSS_ICOMMON_VFR_MODE_HARD */
  uint16_t max_num_timewarp_streams;
    /**< Maximum number of streams with time warping enabled in the system. */
};

typedef struct cvd_devcfg_clock_level_t cvd_devcfg_clock_level_t;

struct cvd_devcfg_clock_level_t
{
  uint32_t per_hw_thread_clock_hz;
    /**< VDSP per hardware thread clock in Hz for this clock level. */
  uint16_t num_supported_use_cases;
    /**< Number of supported use cases for this clock level. */
  cvd_devcfg_supported_voice_use_case_t* supported_use_cases;
    /**< Array of supported use cases for this clock level, with size
         num_supported_use_cases. */
};

typedef struct cvd_devcfg_clock_table_t cvd_devcfg_clock_table_t;

struct cvd_devcfg_clock_table_t
{
  uint16_t num_clock_levels;
    /**< Number of clock levels in this clock table. */
  cvd_devcfg_clock_level_t* clock_levels;
    /**< Array of clock levels, with size num_clock_levels. The clock levels
         are in increasing order of the clock value. The client must find the
         lowest clock level that meets the client's use case requirement. */
};

#ifndef WINSIM

typedef struct cvd_devcfg_mmpm_core_info_t cvd_devcfg_mmpm_core_info_t;

struct cvd_devcfg_mmpm_core_info_t
{
  MmpmCoreIdType core_id;
    /**< Indicate which H/W core to use. */
  MmpmCoreInstanceIdType instance_id;
    /**< Indicate which instance of the same H/W core to use. */
};

typedef struct cvd_devcfg_mmpm_bw_value_t cvd_devcfg_mmpm_bw_value_t;

struct cvd_devcfg_mmpm_bw_value_t
{
  MmpmBusRouteType bus_route;
    /**< Indicate master<->slave pair for a bus route. */
  MmpmBusBWDataUsageType data_usage;
    /**< Indicate bus bandwidth usage, usage percentage and usage type. */
};

typedef struct cvd_devcfg_mmpm_bw_requirement_t cvd_devcfg_mmpm_bw_requirement_t;

struct cvd_devcfg_mmpm_bw_requirement_t
{
  uint32_t num_bw_values;
    /**< Number of bus bandwidth values. */
  cvd_devcfg_mmpm_bw_value_t* bw_values;
    /**< Array of bus bandwidth values, with size num_bw_values. */
};

#endif /* !WINSIM */

#endif /* __CVD_DEVCFG_I_H__ */

