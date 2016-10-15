/*============================================================================

FILE:      DalChipInfo.c

DESCRIPTION:
  This file implements the core portion of the Chip DAL.

PUBLIC CLASSES:
  ChipInfo_DriverDeInit
  ChipInfo_DeviceInit
  ChipInfo_DeviceDeInit
  ChipInfo_PowerEvent
  ChipInfo_Open
  ChipInfo_Close
  ChipInfo_Info

==============================================================================
        Copyright (c) 2011-13 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.adsp/2.2/systemdrivers/chipinfo/src/DalChipInfo.c#1 $
  $DateTime: 2013/04/03 17:22:53 $
  $Author: coresvc $
 
when       who     what, where, why
--------   ---     --------------------------------------------------------
07/11/11   pbi     Fixed warnings.
============================================================================*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "DalChipInfo.h"
#include <stringl/stringl.h>

/*=========================================================================
      Implementation
==========================================================================*/


DALResult 
ChipInfo_DeviceInit(ChipInfoClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult 
ChipInfo_DeviceDeInit(ChipInfoClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
ChipInfo_PowerEvent(ChipInfoClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
  return DAL_SUCCESS;
}

DALResult 
ChipInfo_Open(ChipInfoClientCtxt *pCtxt, uint32 dwAccessMode )
{
  return DAL_SUCCESS;
}

DALResult 
ChipInfo_Close(ChipInfoClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult 
ChipInfo_Info(ChipInfoClientCtxt *pCtxt,DalDeviceInfo *pDeviceInfo, uint32 dwSize)
{
  return DAL_SUCCESS;
}


/*==========================================================================

  FUNCTION      ChipInfo_GetChipVersion

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_GetChipVersion
(
  ChipInfoClientCtxt     *pCtxt,
  uint32                  nNotUsed,
  DalChipInfoVersionType *pnVersion
)
{
  /*-----------------------------------------------------------------------*/
  /* Return chip version.                                                  */
  /*-----------------------------------------------------------------------*/

  *pnVersion = pCtxt->pChipInfoDevCtxt->pChipInfoDrvCtxt->nChipVersion;

  return DAL_SUCCESS;

} /* END ChipInfo_GetChipVersion */


/*==========================================================================

  FUNCTION      ChipInfo_GetRawChipVersion

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_GetRawChipVersion
(
  ChipInfoClientCtxt *pCtxt,
  uint32              nNotUsed,
  uint32             *pnVersion
)
{
  /*-----------------------------------------------------------------------*/
  /* Return raw chip version.                                              */
  /*-----------------------------------------------------------------------*/

  *pnVersion = pCtxt->pChipInfoDevCtxt->pChipInfoDrvCtxt->nRawChipVersion;

  return DAL_SUCCESS;

} /* END ChipInfo_GetRawChipVersion */


/*==========================================================================

  FUNCTION      ChipInfo_GetChipId

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_GetChipId
(
  ChipInfoClientCtxt *pCtxt,
  uint32              nNotUsed,
  DalChipInfoIdType  *peId
)
{
  /*-----------------------------------------------------------------------*/
  /* Return chip version.                                                  */
  /*-----------------------------------------------------------------------*/

  *peId = pCtxt->pChipInfoDevCtxt->pChipInfoDrvCtxt->eChipId;

  return DAL_SUCCESS;

} /* END ChipInfo_GetChipId */


/*==========================================================================

  FUNCTION      ChipInfo_GetRawChipId

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_GetRawChipId
(
  ChipInfoClientCtxt *pCtxt,
  uint32              nNotUsed,
  uint32             *pnId
)
{
  /*-----------------------------------------------------------------------*/
  /* Return raw chip part number.                                          */
  /*-----------------------------------------------------------------------*/

  *pnId = pCtxt->pChipInfoDevCtxt->pChipInfoDrvCtxt->nRawChipId;

  return DAL_SUCCESS;

} /* END ChipInfo_GetRawChipId */


/*==========================================================================

  FUNCTION      ChipInfo_GetChipIdString

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_GetChipIdString
(
  ChipInfoClientCtxt  *pCtxt,
  char                *szIdString,
  uint32               nMaxLength
)
{
  ChipInfoDrvCtxt *pDrvCtxt;

  /*-----------------------------------------------------------------------*/
  /* Get the driver context.                                               */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = pCtxt->pChipInfoDevCtxt->pChipInfoDrvCtxt;

  /*-----------------------------------------------------------------------*/
  /* Copy the string to the user.                                          */
  /*-----------------------------------------------------------------------*/
  strlcpy(
    (char *)szIdString, pDrvCtxt->szChipIdString,nMaxLength);

  return DAL_SUCCESS;

} /* END ChipInfo_GetChipIdString */


/*==========================================================================

  FUNCTION      ChipInfo_GetChipFamily

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_GetChipFamily
(
  ChipInfoClientCtxt    *pCtxt,
  uint32                 nNotUsed,
  DalChipInfoFamilyType *peFamily
)
{
  /*-----------------------------------------------------------------------*/
  /* Return chip family.                                                   */
  /*-----------------------------------------------------------------------*/

  *peFamily = pCtxt->pChipInfoDevCtxt->pChipInfoDrvCtxt->eChipFamily;

  return DAL_SUCCESS;

} /* END ChipInfo_GetChipFamily */


/*==========================================================================

  FUNCTION      ChipInfo_GetModemSupport

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_GetModemSupport
(
  ChipInfoClientCtxt   *pCtxt,
  uint32                nNotUsed,
  DalChipInfoModemType *pnModem
)
{
  /*-----------------------------------------------------------------------*/
  /* Return the modem support value.                                       */
  /*-----------------------------------------------------------------------*/

  *pnModem = pCtxt->pChipInfoDevCtxt->pChipInfoDrvCtxt->nModemSupport;

  return DAL_SUCCESS;

} /* END ChipInfo_GetModemSupport */




