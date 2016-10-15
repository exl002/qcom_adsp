#ifndef IPC_ROUTER_QDI_DRIVER_H_
#define IPC_ROUTER_QDI_DRIVER_H_
/*===========================================================================

                      I P C    R O U T E R    Q D I
                          H E A D E R    F I L E
  This header describes the QDI interface of the IPC Router driver in the
  Guest OS. This is a QuRT specific header file.

 Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.  
 QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.adsp/2.2/mproc/ipc_router/src/ipc_router_qdi_driver.h#1 $ $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/21/12   aep     First revision
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include <stddef.h>
#include "qurt_qdi.h"
#include "stdint.h"

/*===========================================================================
                  CONSTANT / MACRO DECLARATIONS
===========================================================================*/


/** QDI method types */

#define IPC_ROUTER_QDI_INIT              (QDI_OPEN)
#define IPC_ROUTER_QDI_DEINIT            (QDI_CLOSE)

#define IPC_ROUTER_QDI_OPEN              (QDI_PRIVATE + 0)
#define IPC_ROUTER_QDI_CLOSE             (QDI_PRIVATE + 1)
#define IPC_ROUTER_QDI_SEND              (QDI_PRIVATE + 2)
#define IPC_ROUTER_QDI_SEND_CONF         (QDI_PRIVATE + 3)
#define IPC_ROUTER_QDI_REGISTER_SERVER   (QDI_PRIVATE + 4)
#define IPC_ROUTER_QDI_UNREGISTER_SERVER (QDI_PRIVATE + 5)
#define IPC_ROUTER_QDI_FIND_ALL_SERVERS  (QDI_PRIVATE + 6)
#define IPC_ROUTER_QDI_READ              (QDI_PRIVATE + 7)
#define IPC_ROUTER_QDI_EVENT_READ        (QDI_PRIVATE + 8)
#define IPC_ROUTER_QDI_PEEK              (QDI_PRIVATE + 9)

#define IPC_ROUTER_QDI_INTERNAL_ERR      (-1)
#define IPC_ROUTER_QDI_PARAM_ERR         (-2)
#define IPC_ROUTER_QDI_NO_SPACE_ERR      (-3)
#define IPC_ROUTER_QDI_PERM_ERR          (-4)
#define IPC_ROUTER_QDI_NO_MEM_ERR        (-5)
#define IPC_ROUTER_QDI_NOT_SUPP_ERR      (-6)

#define IPC_ROUTER_QDI_DEV_NAME          "/dev/ipc_router"

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

#endif
