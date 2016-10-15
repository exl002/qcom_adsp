/*=========================================================================
                       QDSS e-fuse check

GENERAL DESCRIPTION 
   The interface to check efuses associated with the QDSS for accessing its 
   trace functionality is implemented. The fuses that are checked include 
   DBGEN, SPIDEN, NIDEN and SPNIDEN. The functionality controlled by the fuses 
   are as follows. DBGEN controls non-secure invasive debugging, SPIDEN controls
   secured invasive debugging, NIDEN controls non-secure non-invasive debugging 
   and SPNIDEN controls secure non-invasive debugging.

   According to the ARM Coresight architecture, in all cases, the fuse 
   setting for invasive debugging overrides settings for non-invasive debugging.
    
 
EXTERNALIZED FUNCTIONS
   qdss_fuse_trace_access
 
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
   Requires System FPB clock to access the efuse registers.
   

      Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*========================================================================== 
 $Header: //components/rel/core.adsp/2.2/debugtrace/common/src/qdss_fuse_check.c#1 $
==========================================================================*/  



#include "qdss_fuse_check.h"
#include "halhwio_qdss.h"
#include "qdss_fuse_regs.h"



/*
Input: None
   
Description: 
   Returns the accessibility of of QDSS peripheral for in non-secure mode for 
   non-invasive debugging such as use of STM for software instrumented tracing,
   ETM and PFT. The function checks the fuse configuration of DBGEN and NIDEN. 
   If DBGEN is enabled, non-invasive debugging is also enabled regardless of the 
   configuration of NIDEN.

Return:
   1 - Trace is permitted in non-secure mode.
   0 - Trace is not permitted in non-secure mode.
*/

unsigned int qdss_fuse_trace_access_non_secure(void)
{
   unsigned int uNIDEN_DISABLE,uDBGGEN_DISABLE;
   unsigned int status=0;
   uNIDEN_DISABLE = HWIO_INF(OEM_CONFIG1,DAP_NIDEN_DISABLE);
   uDBGGEN_DISABLE = HWIO_INF(OEM_CONFIG1,DAP_DBGEN_DISABLE);				
   status = ((!uDBGGEN_DISABLE)||(!uNIDEN_DISABLE))?1:0;
	return status;
}


/*
Input: None
   
Description: 
   Returns the accessibility of of QDSS peripheral in secure mode for 
   non-invasive debugging such as use of STM for software instrumented tracing,
   ETM and PFT. The function checks the fuse configuration of SPIDEN and SPNIDEN. 
   If SPIDEN is enabled, non-invasive debugging is also enabled regardless of the 
   configuration of SPNIDEN.

Return:
   1 - Trace is permitted in secure mode.
   0 - Trace is not permitted in secure mode.
*/

unsigned int qdss_fuse_trace_access_secure(void)
{
   unsigned int uSPIDEN_DISABLE,uSPNIDEN_DISABLE;
   unsigned int status;
   
   uSPNIDEN_DISABLE = HWIO_INF(OEM_CONFIG1,DAP_SPNIDEN_DISABLE);
   uSPIDEN_DISABLE  = HWIO_INF(OEM_CONFIG1,DAP_SPIDEN_DISABLE);
   				
   status = ((!uSPIDEN_DISABLE)||(!uSPNIDEN_DISABLE))?1:0;
   return status;
}

/*
Input: None

Description:
   The specific security mode in which trace hardware is accessible is returned 
   by this function. Accesses are guarded by efuse settings. 

Return:
   QDSS_TRACE_ACCESS_ALL_MODES - Trace access is possible under both secure and 
   non-secure mode.

   QDSS_TRACE_ACCESS_NONE - Trace access is not possible in any mode.

   QDSS_TRACE_ACCESS_SECURE_MODE - Trace access is possible in secure mode only.

   QDSS_TRACE_ACCESS_NONSECURE_MODE - Trace access is possible in non-secure 
   mode only.
*/
eqdss_trace_ret qdss_fuse_trace_access_mode(void)
{
   unsigned char val;
   val = (qdss_fuse_trace_access_secure()<<1)|
          qdss_fuse_trace_access_non_secure();

   return (eqdss_trace_ret)val;
   
}

unsigned int qdss_fuse_trace_access(void)
{
   unsigned int ret=0;
   if(qdss_fuse_trace_access_mode()==QDSS_TRACE_ACCESS_ALL_MODES)
   {
      ret=1;
   }
   return ret;
}
