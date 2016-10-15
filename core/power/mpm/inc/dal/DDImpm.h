#ifndef __DDIMPM_H__
#define __DDIMPM_H__

/*===========================================================================
               Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
===========================================================================*/
#include "DalDevice.h"
#include "mpmint_types.h"
#define DALMPM_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/*===========================================================================
                       FUNCTION DECLARATIONS                             
===========================================================================*/

typedef struct Dalmpm Dalmpm;
struct Dalmpm
{
  struct DalDevice DalDevice;
  DALResult (*ConfigInt)(DalDeviceHandle    *_h,
                         uint32              int_num,
                         mpmint_detect_type  detection,
                         mpmint_polarity_type polarity);

  DALResult (*ConfigWakeup)(DalDeviceHandle *_h, uint32 int_num);

  DALResult (*ConfigGpioWakeup)(DalDeviceHandle     *_h,
                                uint32               which_gpio,
                                mpmint_detect_type   detection,
                                mpmint_polarity_type polarity);
   
  DALResult (*DisableWakeup)(DalDeviceHandle *_h, uint32 int_num);

  DALResult (*DisableGpioWakeup)(DalDeviceHandle *_h, uint32 which_gpio);

  DALResult (*GetNumMappedInterrupts)(DalDeviceHandle *_h, uint32 *pnum);
   
  DALResult (*MapInterrupts)(DalDeviceHandle         *_h,
                             mpmint_config_info_type *intrs,
                             uint32                   intrs_count,
                             uint32                  *intrs_mapped);
   
  DALResult (*SetupInterrupts)(DalDeviceHandle         *_h,
                               mpmint_config_info_type *intrs,
                               uint32                   intrs_count);
};

typedef struct DalmpmHandle DalmpmHandle; 
struct DalmpmHandle 
{
  uint32 dwDalHandleId;
  const Dalmpm * pVtbl;
  void * pClientCtxt;
};

#define Dalmpm_StringDeviceAttach(DevName,hDalDevice)\
  DAL_StringDeviceAttachEx(NULL,DevName,DALMPM_INTERFACE_VERSION,hDalDevice)

	
/*=========================================================================
FUNCTION  Dalmpm_ConfigInt
===========================================================================*/
/**
 * @brief Configure the polarity (ACTIVE HIGH/ACTIVE LOW) and detection type
 *        (EDGE/LEVEL) of an MPM fixed-function interrupt.
 *
 * If you want to configure an MPM-routed GPIO interrupt, you probably want to
 * use Dalmpm_config_gpio_wakeup instead.  Also, if you simply want to enable
 * an interrupt as a wakeup source, see Dalmpm_config_wakeup.
 *
 * Since fixed-function interrupts generally have only a single configuration
 * that makes sense, this driver populates each one with a default setting at
 * init time.  In the unlikely event you want to override the default you can
 * call this function--but it is not expected to be used in general.
 * Look DALmpmintTypes.h for more information on detection and polarity types.
 *
 * @param _h        Handle to MPM DAL device.
 * @param int_num   The interrupt that you wish to configure.
 * @param detection The desired detection method (edge/level).
 * @param polarity  The desired polarity (high/rising vs. low/falling).
 *
 * @return   
 *  DAL_SUCCESS : Interrupt was configured successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 *
 * @note If it is only a vaild wake up and is of interrupt type then configures 
 * otherwise does nothing and returns.
 *       
 * @see Dalmpm_ConfigGpioWakeup
 * @see Dalmpm_ConfigWakeup
 */

static __inline DALResult Dalmpm_ConfigInt(DalDeviceHandle     *_h,
                                           uint32               int_num,
                                           mpmint_detect_type   detection,
                                           mpmint_polarity_type polarity)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->ConfigInt( _h, int_num, detection, polarity);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*=========================================================================
FUNCTION  Dalmpm_ConfigWakeup
===========================================================================*/
/**
 * @brief Enable an MPM-routed interrupt as a wakeup source during deep sleep.
 *
 * If the interrupt occurs during deep sleep, the MPM will wake the system and
 * this driver will soft-trigger the interrupt into the local interrupt
 * controller (that is, you do not have to handle the MPM interrupt seperate
 * from the interrupt's regular incarnation).
 *
 * @param _h      Handle to MPM DAL device.
 * @param int_num Which interrupt to enable.
 *
 * @note If it is a vaild wake up interrupt and is of interrupt type then configures 
 * as wakeup source otherwsie does nothing and returns.
 *
 * @return   
 *  DAL_SUCCESS : Interrupt was configured as wakeup source successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 */

static __inline DALResult Dalmpm_ConfigWakeup(DalDeviceHandle *_h,
                                              uint32           int_num)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->ConfigWakeup( _h, int_num);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*=========================================================================
FUNCTION  Dalmpm_ConfigGpioWakeup
===========================================================================*/
/**
 * @brief Configure a GPIO interrupt as a wakeup source.
 *
 * Configure the polarity (ACTIVE HIGH/ACTIVE LOW) and detection type
 * (EDGE/LEVEL) of an MPM-routed GPIO interrupt, and enable that GPIO as a
 * wakeup source during deep sleep.
 *
 * @param _h         Handle to MPM DAL device.
 * @param which_gpio The number of the GPIO to configure.
 * @param detection  The desired detection method (edge/level).
 * @param polarity   The desired polarity (high/rising vs. low/falling).
 *
 * @note If the GPIO is not supported by the MPM hardware this function does
 *       nothing and returns.
 *
 * @return   
 *  DAL_SUCCESS : Interrupt is configured as wakeup source successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 */

static __inline DALResult Dalmpm_ConfigGpioWakeup(DalDeviceHandle     *_h,
                                                  uint32               which_gpio,
                                                  mpmint_detect_type   detection,
                                                  mpmint_polarity_type polarity)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->ConfigGpioWakeup( _h, which_gpio, detection, polarity);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*=========================================================================
FUNCTION  Dalmpm_DisableWakeup
===========================================================================*/
/**
 * @brief Disable an MPM-routed interrupt as a wakeup source during deep sleep.
 *
 * @note When an interrupt is disabled in the MPM, that interrupt may be
 *       completely discarded during deep sleep (not even latched for later
 *       processing).  However, it can still cause a wakeup from "shallower"
 *       forms of sleep (like simple halt, etc.).
 *
 * @param _h      Handle to MPM DAL device.
 * @param int_num Which interrupt to disable.
 *
 *
 * @return   
 *  DAL_SUCCESS : Interrupt was desabled as a wakeup source successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 */

static __inline DALResult Dalmpm_DisableWakeup(DalDeviceHandle *_h,
                                               uint32           int_num)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->DisableWakeup( _h, int_num);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*=========================================================================
FUNCTION  Dalmpm_DisableGpioWakeup
===========================================================================*/
/**
 * @brief Disable an MPM-routed GPIO as a wakeup source during deep sleep.
 *
 * @note When an interrupt is disabled in the MPM, that interrupt may be
 *       completely discarded during deep sleep (not even latched for later
 *       processing).  However, it can still cause a wakeup from "shallower"
 *       forms of sleep (like simple halt, etc.).
 *
 * @param _h         Handle to MPM DAL device.
 * @param which_gpio The number of the GPIO to disable.
 *
 *
 * @note If the GPIO is not supported by the MPM hardware this function does
 *       nothing and returns.
 *
 * @return   
 *  DAL_SUCCESS : If GPIO is disabled as wakeup source successfully.
 *  DAL_ERROR   : If invalid parameters are passed

 */

static __inline DALResult Dalmpm_DisableGpioWakeup(DalDeviceHandle *_h,
                                                   uint32           which_gpio)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->DisableGpioWakeup( _h, which_gpio);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*=========================================================================
FUNCTION  Dalmpm_GetNumMappedInterrupts
===========================================================================*/
/**
 * @brief Gives the number of interrupts (IRQs) that are mapped to mpm for
 *        a master it's running (i.e. modem, apps, q6).
 *
 * @param _h    Handle to MPM DAL device.
 * @param pnum  Number of interrupts mapped.
 *
 * @return   
 *  DAL_SUCCESS : If successful.
 *  DAL_ERROR   : If invalid parameters are passed
 */

static __inline DALResult Dalmpm_GetNumMappedInterrupts(DalDeviceHandle *_h,
                                                        uint32          *pnum)
{
  if(_h != NULL && pnum != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->GetNumMappedInterrupts( _h, pnum);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*=========================================================================
FUNCTION  Dalmpm_MapInterrupts
===========================================================================*/
/**
 * @brief Fills in the input arrays with the mapping of mpm interrupt id
 *        and corresponding master interrupt id.
 *
 * It should be noted that the first two fields of mpmint_config_info_type
 * must not be changed by caller or it can result into configuring other
 * interrupts which are not meant to be.
 *
 * @param _h            Handle to MPM DAL device.
 * @param intrs         Array in which above mapping will be filled in.
 * @param intrs_count   Size of the array.
 * @param intrs_mapped  Number of mappings actually filled in (should be
 *                      same as intrs_count in normal scenario).
 *
 * @return   
 *  DAL_SUCCESS : If interrupts are mapped successfully.
 *  DAL_ERROR   : If invalid parameters are passed
 */

static __inline DALResult Dalmpm_MapInterrupts(DalDeviceHandle         *_h,
                                               mpmint_config_info_type *intrs,
                                               uint32                   intrs_count,
                                               uint32                  *intrs_mapped)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->MapInterrupts( _h, intrs, intrs_count, intrs_mapped);
  }
  else
  {
    return DAL_ERROR;
  }
}

/*=========================================================================
FUNCTION  Dalmpm_SetupInterrupts
===========================================================================*/
/**
 * @brief Sets up various fields of more than one interrupts at MPM level
 *        like trigger type and status.
 *
 * @note  As of now this function is intended to use for interrupts only 
 *        (no GPIO). 
 *
 * @param _h          Handle to MPM DAL device.
 * @param intrs       Input array containing various interrupt information 
 * @param intrs_count Number of interrupts in the above array.
 *
 *
 * @return   
 *  DAL_SUCCESS : If successful in setting up interrupts filds like trigger
 *                type and statuc.
 *  DAL_ERROR   : If invalid parameters are passed.
 */

static __inline DALResult Dalmpm_SetupInterrupts(DalDeviceHandle         *_h,
                                                 mpmint_config_info_type *intrs,
                                                 uint32                   intrs_count)
{
  if(_h != NULL)
  {
    return ((DalmpmHandle *)_h)->pVtbl->SetupInterrupts( _h, intrs, intrs_count);
  }
  else
  {
    return DAL_ERROR;
  }
}

#endif
