#ifndef PM_LIB_CMD_H
#define PM_LIB_CMD_H
/*! \file 
 *  \n
 *  \brief  pm_lib_cmd.h ---- PMIC library command parameters 
 *  \n
 *  \n This file is defines commands used in PMIC library APIs 
 *  \n
 *  \n &copy; Copyright 2003-2009 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.adsp/2.2/api/systemdrivers/pmic/pm_lib_cmd.h#1 $
  $DateTime: 2013/04/03 17:22:53 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/09   jtn     Updated file documentation
05/01/09   jtn     New file
=============================================================================*/


/**
 * Use these commands to enable or disable the different PMIC devices
 */
typedef enum
{
   /**
    * Command to turn off
    */
   PM_OFF_CMD,
   /**
    * Command to turn on
    */
   PM_ON_CMD,
   /**
    * Invalid selection, used for range checking in PMIC APIs
    */
   PM_INVALID_CMD
}pm_switch_cmd_type;


#endif
