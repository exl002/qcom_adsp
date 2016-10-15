#ifndef _ADSP_LSM_SESSION_COMMANDS_H_
#define _ADSP_LSM_SESSION_COMMANDS_H_

/*========================================================================*/
/**
@file adsp_lsm_session_commands.h

@brief This file contains LSM session commands and events structures
definitions.
*/

/*===========================================================================
NOTE: The description above does not appear in the PDF.

      The LSM_mainpage.dox file contains all file/group descriptions that are
      in the output PDF generated using Doxygen and Latex. To edit or update
      any of the file/group text in the PDF, edit the AFE_mainpage.dox file
      contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

/*========================================================================
Edit History

$Header: //source/qcom/qct/images/adsp/avs/rel/2.2/adsp_proc/avs/api/listen/inc/adsp_lsm_session_commands.h#7 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/07/2012 Sudhir   Initial version
========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


#include "mmdefs.h"


/** Maximum number of possible session IDs. IDs start from 1; 0 is reserved. */
#define LSM_MAX_SESSION_ID                                          (8)

/** Maximum number of LSM sessions. */
#define LSM_MAX_NUM_SESSIONS                                LSM_MAX_SESSION_ID

/** @ingroup lsm_session_cmd_shared_mem_map_regions
    Commands the LSM session to map multiple shared memory
    regions. All the mapped regions must be from the same memory
    pool.

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS \n
      Dst_port :   1 to LSM_MAX_SESSION_ID

    @apr_msg_payload
      @structure{lsm_session_cmd_shared_mem_map_regions_t}
      @tablespace

    @mappayload
      @structure{lsm_session_shared_map_region_payload_t}
      @tablespace

    @prerequisites
      All regions to be mapped must be from the same memory pool.

    @return
      LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS    
*/
#define LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS        0x00012A7F

/** Enumeration for the shared memory 8_4K pool ID.
*/
#define ADSP_MEMORY_MAP_SHMEM8_4K_POOL      3

/* LSM Session shared memory map regions structure  */
typedef struct  lsm_session_cmd_shared_mem_map_regions_t lsm_session_cmd_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @brief Memory map regions command payload used by the
    #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS .
    \n
    This structure allows clients to map multiple shared memory regions in a
    single command. Following this structure are num_regions of
    lsm_session_shared_map_region_payload_t.
*/
struct lsm_session_cmd_shared_mem_map_regions_t
{
    uint16_t                  mem_pool_id;
    /**< Type of memory on which this memory region is mapped.

         Supported values:
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved

         The memory pool ID implicitly defines the characteristics of the memory.
         Characteristics may include alignment type, permissions, etc.

         ADSP_MEMORY_MAP_SHMEM8_4K_POOL is shared memory, byte addressable, and 4 KB aligned. */


    uint16_t                  num_regions;
    /**< Number of regions to map. 

         Supported values:
         - Any value greater than zero */

    uint32_t                  property_flag;
    /**< Configures one common property for all the regions in the payload. 

         Supported values:
         - 0x00000000 to 0x00000001

         b0 -  bit 0 indicates physical or virtual mapping
            0  Shared memory address provided in lsm_session_shared_map_region_payload_t is a physical address.
               The shared memory needs to be mapped( hardware TLB entry) and a software entry needs to
               be added for internal book keeping.

            1  Shared memory address provided in lsm_session_shared_map_region_payload_t is a virtual address.
               The shared memory must not be mapped (since hardware TLB entry is already available)
               but a software entry needs to be added for internal book keeping. This can be useful
               if two services with in ADSP is communicating via APR. They can now directly communicate
               via the Virtual address instead of Physical address. The virtual regions must be
               contiguous. num_regions must be 1 in this case.    

        b31-b1 - reserved bits. must be set to zero
     
        num_regions of lsm_session_shared_map_region_payload_t  will be followed after this
        payload.
         @newpage */

} 
#include "adsp_end_pack.h"
;

#include "adsp_begin_pack.h"

/** @brief Map region payload used by the lsm_session_cmd_shared_mem_map_regions_t structure.
*/
struct lsm_session_shared_map_region_payload_t
{
    uint32_t                  shm_addr_lsw;
    /**< least significant word of starting address in the memory region to map. It must be 
         contiguous memory, and it must be 4 KB aligned.

         Supported values:
         - Valid 32 bit address */

    uint32_t                  shm_addr_msw;
    /**< most significant word of startng address in the memory region to map.  
         For 32 bit shared memory address, this field must be set to zero.
         For 36 bit shared memory address, bit31 to bit 4 must be set to zero

         Supported values:
         - For 32 bit shared memory address, this field must be set to zero.
         - For 36 bit shared memory address, bit31 to bit 4 must be set to zero
         - For 64 bit shared memory address, any 32 bit value */


    uint32_t                  mem_size_bytes;
    /**< Number of bytes in the region. 
         The aDSP will always map the regions as virtual contiguous memory,
         but the memory size must be in multiples of 4 KB to avoid gaps in
         the virtually contiguous mapped memory.

         Supported values:
         - multiples of 4KB
         @newpage */
}
#include "adsp_end_pack.h"
;

/** @ingroup lsm_session_cmdrsp_shared_mem_map_regions_t
    response sent by LSM session to the client in response to
    the successfuly processing of
    LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS command.

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS \n

    @apr_msg_payload
      @structure{lsm_session_cmdrsp_shared_mem_map_regions_t}
      @tablespace
      @inputtable{LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS_APR_msg_payload.tex}

    @prerequisites
      LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS cmd must be sent to receive this
      response from LSM session
      
    @return
      none      

    @newpage
*/
#define LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS               	0x00012A80

/* LSM Session shared memory map regions response structure  */
typedef struct  lsm_session_cmdrsp_shared_mem_map_regions_t lsm_session_cmdrsp_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @brief Memory map command response payload used by the
    #LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS commands.\n
    \n
    This structure is  response to the memory maping
    regions command. mem_map_handle must later be used by the
    client to unmap regions that are previously mapped.
*/
struct lsm_session_cmdrsp_shared_mem_map_regions_t
{
    uint32_t                  mem_map_handle;
      /**< A memory map handle encapsulating shared memory attributes is returned 
           iff LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS command is successful.
           In the case of failure , a generic APR error response is
           returned to the client.
       
           Supported Values:
           - Any 32 bit valid value

         @newpage */
}
#include "adsp_end_pack.h"
;

/** @ingroup lsm_session_cmd_mem_unmap_regions
    Commands the LSM to unmap multiple shared memory regions
    that were previously mapped.

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS \n
      Dst_port:  1 to LSM_MAX_SESSION_ID

   @apr_msg_payload
      @structure{lsm_session_cmd_shared_mem_unmap_regions_t}
      @tablespace
      @inputtable{LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS_APR_msg_payload.tex}

    @prerequisites
      Correspoding LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS cmd
      must be successfully processed.

    @return
      APRV2_IBASIC_RSP_RESULT .
    @newpage
*/
#define LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS              0x00012A81

/* LSM session shared memory unmap regions command structure */
typedef struct lsm_session_cmd_shared_mem_unmap_regions_t lsm_session_cmd_shared_mem_unmap_regions_t;

#include "adsp_begin_pack.h"

/** @brief Memory unmap regions command payload used by the
    #LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS \n
    \n
    This structure allows clients to unmap multiple shared memory regions in
    a single command. 
*/
struct lsm_session_cmd_shared_mem_unmap_regions_t
{
    uint32_t                  mem_map_handle;
      /**< memory map handle returned by LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS commands 
       
           Supported Values:
           - valid 32 bit value
       
         @newpage */
}
#include "adsp_end_pack.h"
;

/** @addtogroup lsm_session_cmd_open_tx
@{ */
/** Creates dynamic sessions for listen applications. Type of
    created application depends on the application id specifeid
    in the payload.

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMD_OPEN_TX 
      Dst_port:  1 to LSM_MAX_SESSION_ID (0 is
                 reserved)
    
    @apr_msg_payload
      @structure{lsm_session_cmd_open_tx_t}
      @tablespace
      @inputtable{LSM_SESSION_CMD_OPEN_TX_APR_msg_payload.tex}

    @description
      When creating a session,  client must specify the
      session IDs. The current release supports up to
      LSM_MAX_SESSION_ID groups.\n

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT

*/
#define LSM_SESSION_CMD_OPEN_TX                             0x00012A82

/** Voice wakeup application id*/
#define LSM_VOICE_WAKEUP_APP                           0x1

/* Payload format for an LSM open session command. */
typedef struct lsm_session_cmd_open_tx_t lsm_session_cmd_open_tx_t;


#include "adsp_begin_pack.h"

/**  @brief payload of the command LSM_SESSION_CMD_OPEN_TX,
           which is used for opening LSM session.
*/
struct lsm_session_cmd_open_tx_t
{
    uint16_t app_id;
    /**<  Application id which LSM needs to create/open.
          
          Supported values:  #LSM_VOICE_WAKEUP_APP */

    uint16_t reserved;
    /**< Reserved for 32-bit alignment. 
          This field must be set to 0. */

    uint32_t sample_rate;
    /**< Sampling rate on which application will run 
     
          Supported values: 16000 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup lsm_session_cmd_open_tx */


/** @addtogroup lsm_session_cmd_set_params
@{ */
/** @xreflabel{hdr:LSMSessionCmdSetParams}
    Sets one or more  parameters to LSM session. Supported module
    and parameter IDs are provided in Section @xref{hdr:cal_module_ids}.

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMD_SET_PARAMS \n
      Dst_port : 1 to LSM_MAX_SESSION_ID


    @apr_msg_payload
      @structure{lsm_session_cmd_set_params_t}
      @tablespace
      @inputtable{LSM_SESSION_CMD_SET_PARAMS_APR_msg_payload.tex}

    @parspace Parameter data variable payload
      @structure{lsm_session_param_data_t}
      @tablespace
      @inputtable{Shared_param_data_variable_payload.tex}

    @prerequisites
      This command will be accpeted only after the LSM_SESSION_CMD_OPEN_TX
      is processed succesfully.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

*/
#define LSM_SESSION_CMD_SET_PARAMS                                     0x00012A83

/* Payload format for a  Set Parameters command. */
typedef struct lsm_session_cmd_set_params_t lsm_session_cmd_set_params_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #LSM_SESSION_CMD_SET_PARAMS command, which allows
    one or more parameters to be set on a opened session.\n

    If 64 bit data_payload_addr=NULL, a series of lsm_session_param_data_t structures
    immediately follow, whose total size is data_payload_size bytes.
*/
struct lsm_session_cmd_set_params_t
{

    uint32_t                  data_payload_size;
    /**< Size in bytes of the variable payload accompanying the message, or in
         shared memory. This field is used for parsing the parameter payload.
     */

    uint32_t                  data_payload_addr_lsw;
    /**< LSW of parameter data payload address. Supported values: valid address. */
    uint32_t                  data_payload_addr_msw;
   /**< MSW of Parameter data payload address. Supported values: valid address.

        - Must be set to zero for in-band data.
        - In the case of 32 bit Shared memory address, msw  field must be set to zero.
        - In the case of 36 bit shared memory address, bit 31 to bit 4 of msw must be set to zero.
    */
    uint32_t                  mem_map_handle;
    /**< Supported Values: valid mem map handle.
      memory map handle returned by DSP through LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS command.
      if mmhandle is NULL, the ParamData payloads are within the message payload (in-band).
      If mmhandle is non-NULL, the ParamData payloads begin at the address specified in the.
      address msw and lsw (out-of-band).                                                      
                                                                                           
      An optional field is available if parameter data is in-band:
      lsm_session_param_data_t param_data[...]. For detailed payload content, see the
      lsm_session_param_data_t structure.     */

}
#include "adsp_end_pack.h"
;

/* Payload format for  parameter data. */
typedef struct lsm_session_param_data_t lsm_session_param_data_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the session parameter data of the
    #LSM_SESSION_CMD_SET_PARAMS command.\n
    \n
    Immediately following this structure are param_size bytes of parameter
    data.
*/
struct lsm_session_param_data_t
{
    uint32_t                  module_id;
    /**< Unique module ID. */

    uint32_t                  param_id;
    /**< Unique parameter ID. */

    uint16_t                  param_size;
    /**< Data size of the param_id/module_id combination. This is
         a multiple of 4 bytes. */

    uint16_t                  reserved;
        /**< Reserved for 32-bit alignment. 
          This field must be set to 0.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup lsm_session_cmd_set_params */

/** @addtogroup cal_module_ids
@{ */

/** Sets the calibration data for the voice wakeup module.

    This module supports the following parameter IDs:
    - #LSM_PARAM_ID_ENDPOINT_DETECT_THRESHOLD
    - #LSM_PARAM_ID_OPERATION_MODE
	- #LSM_PARAM_ID_GAIN
	- #LSM_PARAM_ID_CONNECT_TO_PORT
	- #LSM_PARAM_ID_KEYWORD_DETECT_SENSITIVITY
    - #LSM_PARAM_ID_USER_DETECT_SENSITIVITY
 
     In order to set the params client needs to send set param API LSM_SESSION_CMD_SET_PARAMS to the desired
     session and fill the module ID with LSM_MODULE_VOICE_WAKEUP , param id with the respective param ids as listed above.
 
     This command will be accepted only after the valid dynamic session is created.

  @apr_hdr_fields
    Opcode : LSM_MODULE_VOICE_WAKEUP
*/
#define LSM_MODULE_VOICE_WAKEUP           0x00012C00

/** @} */ /* end_addtogroup cal_module_ids */

/** @addtogroup cal_param_ids
@{ */

/** This param id will be used to set the beginning of speech and end of 
  speech thresholds. This param id will be accepted when the session is in INIT state

  @apr_hdr_fields
    Opcode : LSM_PARAM_ID_ENDPOINT_DETECT_THRESHOLD

  @apr_msg_payload
    @structure{lsm_param_id_epd_threshold_t}
    @tablespace
    
  @newpage
*/
#define LSM_PARAM_ID_ENDPOINT_DETECT_THRESHOLD      0x00012C01
 									
 									
/**  This version information is used to handle the new
     additions to the config interface in future in backward
     compatible manner.
 */
#define LSM_API_VERSION_EPD_THRESHOLD 									0x1
 									
/* Payload of the LSM_PARAM_ID_ENDPOINT_DETECT_THRESHOLD command. */
typedef struct lsm_param_id_epd_threshold_t lsm_param_id_epd_threshold_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the
    #LSM_PARAM_ID_ENDPOINT_DETECT_THRESHOLD parameter, which
    sets the beginning of speech and end of speech thresholds .
*/
struct lsm_param_id_epd_threshold_t
{
    uint32_t                  minor_version;
    /**< Minor version used for tracking the version of this param id. 
             Supported values:
               #LSM_API_VERSION_EPD_THRESHOLD  */

    int32_t epd_begin_threshold;

   /**< Threshold for beginning of speech in Q20 format 
         Supported Values: Range of Q20 values  
		 Default value:0x02710000
        */

    int32_t epd_end_threshold;

   /**< Threshold for end of speech in Q20 format 
        Supported Values: Range of Q20 values  
		Default value: 0xFA9B62B7 
	  */

}
#include "adsp_end_pack.h"
;

/** This param id will be used to set the different operations
    modes to be operated by voice wakeup module.
  @apr_hdr_fields
    Opcode : LSM_PARAM_ID_OPERATION_MODE

  @apr_msg_payload
    @structure{lsm_param_id_operation_mode_t}
    @tablespace
    
  @newpage
*/
#define LSM_PARAM_ID_OPERATION_MODE                0x00012C02
 									
/**  This version information is used to handle the new
     additions to the config interface in future in backward
     compatible manner.
 */
#define LSM_API_VERSION_OPERATION_MODE 									0x1

/* Payload of the LSM_PARAM_ID_OPERATION_MODE command. */
typedef struct lsm_param_id_operation_mode_t lsm_param_id_operation_mode_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the
    #LSM_PARAM_ID_OPERATION_MODE parameter, which sets the
    different types of operation mode of voice wakeup block
*/
struct lsm_param_id_operation_mode_t
{
    uint32_t                  minor_version;
    /**< Minor version used for tracking the version of this param id. 
             Supported values:
               #LSM_API_VERSION_OPERATION_MODE  */

    uint16_t mode;

   /**< Mode flags that configure the voice wake up module to run in different types of modes. 
    
         Supported values for bit 0:
    
         - 0 -  Keyword Detection Mode is disabled
         - 1 - Keyword Detection Mode is  enabled
    
         Supported values for bit 1:
         This bit is applicable only if bit 0 is set as 1. Otherwise this
         values are ignored.
    
         - 0 -  User Verification Mode is disabled
         - 1 -  User Verification Mode is  enabled
    
        Supported values for bit 2:
    
         - 0 - Events will be sent to HLOS only in case of succesful detection
         - 1 - Events will be sent to HLOS irresepective of success or failure */

    uint16_t reserved;
       /**< Reserved for 32-bit alignment. 
          This field must be set to 0. */

}
#include "adsp_end_pack.h"
;

/** This param id will be used to set the gain which is applied
    on the data coming from SW MAD thread.
  @apr_hdr_fields
    Opcode : LSM_PARAM_ID_GAIN

  @apr_msg_payload
    @structure{lsm_param_id_gain_t}
    @tablespace
    
  @newpage
*/
#define LSM_PARAM_ID_GAIN                         0x00012C03
 									
 									
/**  This version information is used to handle the new
    additions to the config interface in future in backward
    compatible manner.
 */
#define LSM_API_VERSION_GAIN 									0x1
 									
/* Payload of the LSM_PARAM_ID_GAIN command. */
typedef struct lsm_param_id_gain_t lsm_param_id_gain_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the LSM_PARAM_ID_GAIN parameter*/
struct lsm_param_id_gain_t
{
    uint32_t                  minor_version;
    /**< Minor version used for tracking the version of this param id. 
             Supported values:
               #LSM_API_VERSION_GAIN  */

    int16_t gain;
   /**< Gain in Q8 format. 
     Supported Values: Range of Q8 values  
	 Default value; 0x100*/

    uint16_t reserved;
       /**< Reserved for 32-bit alignment. 
          This field must be set to 0. */

}
#include "adsp_end_pack.h"
;

/** This param id will be used to connect the LSM
    application to the AFE port.
  @apr_hdr_fields
    Opcode : LSM_PARAM_ID_CONNECT_TO_PORT

  @apr_msg_payload
    @structure{lsm_param_id_connect_port_t}
    @tablespace
    
  @newpage
*/
#define LSM_PARAM_ID_CONNECT_TO_PORT                        0x00012C04
 									
/**  This version information is used to handle the new
    additions to the config interface in future in backward
    compatible manner.
 */
#define LSM_API_VERSION_CONNECT_TO_PORT 									0x1
 									
/* Payload of the LSM_PARAM_ID_CONNECT_TO_PORT command. */
typedef struct lsm_param_id_connect_port_t lsm_param_id_connect_port_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the LSM_PARAM_ID_CONNECT_TO_PORT parameter*/
struct lsm_param_id_connect_port_t
{
    uint32_t                  minor_version;
    /**< Minor version used for tracking the version of this param id. 
             Supported values:
               #LSM_API_VERSION_CONNECT_TO_PORT  */

    uint16_t port_id;
   /**<  Supported values: valid afe port id */ 

    uint16_t reserved;
       /**< Reserved for 32-bit alignment. 
          This field must be set to 0. */

}
#include "adsp_end_pack.h"
;


/** This param id is used to set the keyword sensitivity.
  @apr_hdr_fields
    Opcode : LSM_PARAM_ID_KEYWORD_DETECT_SENSITIVITY

  @apr_msg_payload
    @structure{lsm_param_id_keyword_det_sensitivity_t}
    @tablespace
    
  @newpage
*/
#define LSM_PARAM_ID_KEYWORD_DETECT_SENSITIVITY                    0x00012C05
 									
 									
/**  This version information is used to handle the new
     additions to the config interface in future in backward
     compatible manner.
 */
#define LSM_API_VERSION_KEYWORD_DETECT_SENSITIVITY 									0x1
 									
/* Payload of the LSM_PARAM_ID_KEYWORD_DETECT_SENSITIVITY command. */
typedef struct lsm_param_id_keyword_det_sensitivity_t lsm_param_id_keyword_det_sensitivity_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the
      LSM_PARAM_ID_KEYWORD_DETECT_SENSITIVITY parameter*/
struct lsm_param_id_keyword_det_sensitivity_t
{
    uint32_t                  minor_version;
    /**< Minor version used for tracking the version of this param id. 
     
             Supported values:
               #LSM_API_VERSION_KEYWORD_DETECT_SENSITIVITY  */

    int16_t keyword_sensitivity;
    /**<  Sensitivity of keyword: If the value is small application give low miss rate 
          and higher false alarms. If value is higher application give high miss rates
          but less false alarms.
     
          Supported values: 0 - 100*/

    uint16_t reserved;
       /**< Reserved for 32-bit alignment. 
          This field must be set to 0. */

}
#include "adsp_end_pack.h"
;

/** This param id is used to set the user sensitivity.
  @apr_hdr_fields
    Opcode : LSM_PARAM_ID_USER_DETECT_SENSITIVITY

  @apr_msg_payload
    @structure{lsm_param_id_user_sensitivity_t}
    @tablespace
    
  @newpage
*/ 
#define LSM_PARAM_ID_USER_DETECT_SENSITIVITY                                 0x00012C06

/**  This version information is used to handle the new
    additions to the config interface in future in backward
    compatible manner.
 */
#define LSM_API_VERSION_USER_DETECT_SENSITIVITY 									0x1
 									
/* Payload of the LSM_PARAM_ID_USER_DETECT_SENSITIVITY command. */
typedef struct lsm_param_id_user_det_sensitivity_t lsm_param_id_user_det_sensitivity_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the LSM_PARAM_ID_USER_DETECT_SENSITIVITY
          parameter*/
struct lsm_param_id_user_det_sensitivity_t
{
    uint32_t                  minor_version;
    /**< Minor version used for tracking the version of this param id. 
             Supported values:
               #LSM_API_VERSION_USER_DETECT_SENSITIVITY  */

    int16_t user_sensitivity;
    /**<  Sensitivity of user: If the value is small application give low miss rate 
          and more false alarms. If value is higher application give more miss rates
          but less false alarms.

         supported values 0 - 100*/


    uint16_t reserved;
       /**< Reserved for 32-bit alignment. 
          This field must be set to 0. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup cal_param_ids */

/** @addtogroup lsm_session_cmd_register_sound_model
@{ */
/** Registers sound model with the session. This command will be accepted only 
    when the session is in INIT state. 
    

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMD_REGISTER_SOUND_MODEL \n
      Dst_port: 1 to LSM_MAX_SESSION_ID (0 is
                 reserved)


    @newpage
    @apr_msg_payload
      @structure{lsm_session_cmd_register_sound_model_t}
      @tablespace
      @inputtable{LSM_SESSION_CMD_REGISTER_SOUND_MODEL_APR_msg_payload.tex}

    @description
      When creating a session, the client must specify the
      session IDs. The current release supports up to
      LSM_MAX_SESSION_ID groups.\n

    @prerequisites
      This command must be sent only after the LSM_SESSION_CMD_OPEN_TX comamnd processed
      succesfully.
      Sound model memory must be mapped using LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS
      before sending this command.

    @return
      APRV2_IBASIC_RSP_RESULT

*/
#define LSM_SESSION_CMD_REGISTER_SOUND_MODEL                             0x00012A84

/* Payload format for an LSM register sound model command. */
typedef struct lsm_session_cmd_register_sound_model_t lsm_session_cmd_register_sound_model_t;

#include "adsp_begin_pack.h"

/**  @brief payload of LSM_SESSION_CMD_REGISTER_SOUND_MODEL
           command
*/
struct lsm_session_cmd_register_sound_model_t
{
    uint32_t model_size;
    /**< Size of the registering sound model in  bytes */

    uint32_t model_addr_lsw;
     /**< LSW of parameter data payload address. Supported values: any. */

    uint32_t model_addr_msw;
     /**< MSW of parameter data payload address. Supported values: any. */

    uint32_t mem_map_handle; 
    /**< Supported Values: Any.
     * memory map handle returned by DSP through LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS command.*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup lsm_session_cmd_register_sound_model */

/** @ingroup lsm_session_cmd_deregister_sound_model 
    Deregisters sound model with the session. This command will be accepted only when the session is in
    INIT state. If session is in ACTIVE state bring session to INIT state and send this command.
   
    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMD_DEREGISTER_SOUND_MODEL
      Dst_port: 1 to LSM_MAX_SESSION_ID (0 is reserved)

    @apr_msg_payload
       None

    @prerequisites
      The session id must be a valid opened session .

    @return
      APRV2_IBASIC_RSP_RESULT

*/
#define LSM_SESSION_CMD_DEREGISTER_SOUND_MODEL                             0x00012A85

/** @ingroup lsm_session_cmd_start
    Starts the specified session. Only after receiving this command session will process
    data. This command will be accepted ony when the session is
    in INIT state. After processing this command suucesfully
    session will move to ACTIVE state.

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMD_START 
      Dst_port: 1 to LSM_MAX_SESSION_ID (0 is reserved)

    @apr_msg_payload
       None

    @prerequisites
      The session id must be a valid opened session .

    @return
      APRV2_IBASIC_RSP_RESULT

*/
#define LSM_SESSION_CMD_START                             0x00012A86


/** @ingroup lsm_session_cmd_stop
    Stops the specified session. In this state the session wont
    process any data. After processing this command session will
    move to INIT state.

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMD_STOP 
      Dst_port :1 to LSM_MAX_SESSION_ID (0 is
               reserved)
    @apr_msg_payload
       None

    @prerequisites
      The session id must be a valid opened session .

    @return
      APRV2_IBASIC_RSP_RESULT
*/
#define LSM_SESSION_CMD_STOP                             0x00012A87



/** @ingroup lsm_session_cmd_close
    Stops and closes a specified session.

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_CMD_CLOSE 
      Dst_port: 1 to LSM_MAX_SESSION_ID (0 is
               reserved)

    @apr_msg_payload
       None

    @prerequisites
      The session id must be a valid opened session .

    @return
      APRV2_IBASIC_RSP_RESULT

*/
#define LSM_SESSION_CMD_CLOSE                             0x00012A88



/** @addtogroup lsm_session_event_detection_status
@{ */
/** ADSP sends the event to the client processor with result
    updated by the listen application. Application is based on 
    value of "app_id" variable in LSM_SESSION_CMD_OPEN_TX
    command. ADSP will send the status in the APR payload of the
    LSM_SESSION_EVENT_DETECTION_STATUS command.

    @relevant_apr_hdr_fields
      Opcode : LSM_SESSION_EVENT_DETECTION_STATUS
      Dst_port: 1 to LSM_MAX_SESSION_ID (0 is reserved)

    @newpage
    @apr_msg_payload
      @structure{lsm_session_event_detect_status_t}
      @tablespace
      @inputtable{LSM_SESSION_EVENT_DETECTION_STATUS_APR_msg_payload.tex}

    @description
     Listen application will send the event to client processor
     after processing data.
    
     If the bit 2 of the variable "mode" is set as 1 in
     LSM_PARAM_ID_OPERATION_MODE events will be sent to HLOS
     irresepctive of success/failure.
     
     payload_size bytes will be followed after the
     lsm_session_event_detection_status_t payload.

    @prerequisites
     None

    @return
      None

*/
#define LSM_SESSION_EVENT_DETECTION_STATUS                             0x00012B00

/** Status when voice wakeup is processing data  */
#define LSM_VOICE_WAKEUP_STATUS_RUNNING     0x1

/** Status when voice wakeup detects keyword and/or user*/
#define LSM_VOICE_WAKEUP_STATUS_DETECTED     0x2

/** Status when voice wakeup detects end of speech */
#define LSM_VOICE_WAKEUP_STATUS_END_SPEECH   0x3

/** Status when voice wakeup rejects keyword and/or user*/
#define LSM_VOICE_WAKEUP_STATUS_REJECTED     0x4


/* Payload format for an LSM session event detection. */
typedef struct lsm_session_event_detection_status_t lsm_session_event_detection_status_t;

#include "adsp_begin_pack.h"

/** @brief payload of LSM_SESSION_EVENT_DETECTION_STATUS
           command.
*/
struct lsm_session_event_detection_status_t
{
    uint16_t  detection_status;
    /**< Status sending by voice wakeup to client
     
         Supported values: #LSM_VOICE_WAKEUP_STATUS_RUNNING
                           #LSM_VOICE_WAKEUP_STATUS_DETECTED
                           #LSM_VOICE_WAKEUP_STATUS_END_SPEECH
                           #LSM_VOICE_WAKEUP_STATUS_REJECTED  */

    uint16_t  payload_size;
    /**<  payload_size bytes that is followed after this structure in the apr payload*/

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup lsm_session_event_detection_status */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_LSM_SESSION_COMMANDS_H_ */
