#include "ipc_router_core.h"
#include "ipc_router_xal.h"
#include "ipc_router_xal_smdl.h"
#include "DALSys.h"
#include "DALSysTypes.h"

/* This file is just a template. Each target must maintain its own version of ipc_router_init.c */

#define IPC_ROUTER_DEFAULT_PROCESSOR_ID 5

extern void ipc_router_qdi_init(void);

static int ipc_router_inited = 0;

static unsigned int ipc_router_get_processor_id(void)
{
  DALSYS_PROPERTY_HANDLE_DECLARE(hSpmDevCfg);
  DALSYSPropertyVar prop;
  DALResult result;

  result = DALSYS_GetDALPropertyHandleStr("/dev/core/mproc/ipc_router", hSpmDevCfg);
  if(result != DAL_SUCCESS)
  {
    return IPC_ROUTER_DEFAULT_PROCESSOR_ID;
  }
  result = DALSYS_GetPropertyValue( hSpmDevCfg, "local_processor_id", 0, &prop );
  if(result != DAL_SUCCESS)
  {
    return IPC_ROUTER_DEFAULT_PROCESSOR_ID;
  }
  return (unsigned int)prop.Val.dwVal;
}

void ipc_router_init(void)
{
  if(ipc_router_inited)
    return;
  ipc_router_inited = 1;
  /* Initialize router and start transports */
  ipc_router_core_init(ipc_router_get_processor_id());

  /* Link to Apps */
  {
    static ipc_router_xal_smdl_param_type param = {"IPCRTR", 
                                                   SMD_APPS_QDSP, 
                                                   SMD_STANDARD_FIFO,
                                                   SMDL_OPEN_FLAGS_MODE_PACKET, 
                                                   FALSE};
    ipc_router_xal_start_xport(&ipc_router_xal_smdl, (void *)&param, 0, "APPS");
  }

  /* Link to Modem */
  {
    static ipc_router_xal_smdl_param_type param = {"IPCRTR", 
                                                   SMD_MODEM_QDSP, 
                                                   SMD_STANDARD_FIFO,
                                                   SMDL_OPEN_FLAGS_MODE_PACKET, 
                                                   FALSE};
    ipc_router_xal_start_xport(&ipc_router_xal_smdl, (void *)&param, 0, "MODM");
  }

  /* Link to Riva */
  {
    static ipc_router_xal_smdl_param_type param = {"IPCRTR", 
                                                   SMD_QDSP_RIVA, 
                                                   SMD_STANDARD_FIFO,
                                                   SMDL_OPEN_FLAGS_MODE_PACKET, 
                                                   FALSE};
    ipc_router_xal_start_xport(&ipc_router_xal_smdl, (void *)&param, 0, "RIVA");
  }

#ifdef FEATURE_IPC_ROUTER_QDI_DRIVER
  ipc_router_qdi_init();
#endif
}

