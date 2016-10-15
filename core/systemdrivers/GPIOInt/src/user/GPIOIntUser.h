#ifndef __DALGPIOInt_H
#define __DALGPIOInt_H
/*===========================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R    

DESCRIPTION
  This modules contains enumeration definitions to support the Dal GPIO 
  interrupt controller. The definitions in this module are used internally 
  in the GPIO interrupt controller and are not to be exposed to the clients.

REFERENCES

       Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.adsp/2.2/systemdrivers/GPIOInt/src/user/GPIOIntUser.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/
#include "DALFramework.h"
#include "DDIGPIOInt.h"
#include "qurt.h"

/*------------------------------------------------------------------------------
Declaring a "GPIOInt" Driver,Device and Client Context
------------------------------------------------------------------------------*/
typedef struct GPIOIntDrvCtxt    GPIOIntDrvCtxt;
typedef struct GPIOIntDevCtxt    GPIOIntDevCtxt;
typedef struct GPIOIntClientCtxt GPIOIntClientCtxt;

/*------------------------------------------------------------------------------
 GPIO interrupt controller constant declarations.
------------------------------------------------------------------------------*/

/*
 * Size of the GPIO log
 */
#define MAX_INTERRUPT_LOG_ENTRIES  100
#define GPIOIST_TASKNAME_SIZE 16
/*
 * As an Upper limit we keep the maximum number of GPIOs as 200.
 * This number needs to be updated if the GPIO number exceeds 200.
 */
#define MAX_NUMBER_OF_GPIOS        200
#define GPIOINTF_PROXY             0x0001
#define GPIOINTF_MPMINT            0x0002
#define GPIOINTF_WAKEUP            0x0004
#define GPIOINTF_REGISTERED        0x0008

/*
 * The below flags indicate what type of interrupt controller the GPIOInt hooks into.
 */
#define GPIOINT_QGIC  0x1
#define GPIOINT_L2VIC 0x2

/*=========================================================================

                           DATA TYPE DEFINITIONS

===========================================================================*/

typedef enum{
  GPIOINT_PLATFORM_L4,
  GPIOINT_PLATFORM_WM,
  GPIOINT_PLATFORM_SYMBIAN,
  GPIOINT_PLATFORM_LINUX,  
  GPIOINT_PLATFORM_BLAST,
  GPIOINT_PLATFORM_NONE,
  PLACEHOLDER_GPIOIntPlatformType = 0x7fffffff
}GPIOIntPlatformType;

/*
 * GPIOIntdata_type
 * Structure storing the details for a SUMMARY group interrupt
 */
typedef struct
{
  GPIOINTISR                     isr;
  GPIOINTISRCtx                  isr_param;
  DALSYSEventHandle              event;
  uint32                         cnt;
  uint32                         last;
  uint32                         unhandled;
  GPIOIntTriggerType             intr_trigger;
  uint16                         gpio_intr_flags;
  qurt_thread_t                  nThreadID;
  unsigned char *                pISTStack;
  char                           aISTName[GPIOIST_TASKNAME_SIZE];
  qurt_anysignal_t *             pISTSignal;
  qurt_thread_attr_t             ThreadAttr;
  uint32                         nPriority;
  uint32                         nISTStackSize;
  uint32                         nGPIO;

} GPIOIntDataType;

/*
 * GPIOIntlog_entry_type
 *
 * Log entry type.
 *
 *  gpio:         The gpio number
 *  timestamp:    The timestamp in sclks.
 */
typedef struct
{
  uint32                         gpio;
  uint32                         timestamp;
} GPIOIntLogEntryType;

/*
 * GPIOIntlog_type
 *
 * Log type container.
 */
typedef struct
{
  uint32                         index;
  GPIOIntLogEntryType            entries[MAX_INTERRUPT_LOG_ENTRIES];
} GPIOIntLogType;

/*
 * GPIOIntIsrDataType
 *
 * work loop thread data to be passed to.
 */
typedef struct
{
  uint32                         gpio;
  GPIOIntDevCtxt                  *device;

} GPIOIntIsrDataType;
 
/*
 * GPIOIntCntrlType
 *
 * Container for all local data.
 *
 * initialized: Indicates if the driver has been started or not.  Needed
 *              mostly because some compilers complain about empty structs.
 * table:       Table of registered GPIO_INT handler functions.
 * wakeup_isr:  ISR to invoke when a monitored GPIO interrupt triggers.
 * log:         Log storage.
 */
typedef struct
{
  /* GPIOInt Dev state can be added by developers here */

  /* Flag to Initialize GPIOInt_Init is called first 
   * before anything else can attach 
   */
  uint8                          GPIOInt_Init;

  /* interrupt_state Table of registered GPIO_INT handler functions */
  GPIOIntDataType                state[MAX_NUMBER_OF_GPIOS];

  /* 
   * total number of GPIOs present on the target
   */
  uint32                         gpio_number;

  /* 
   * Interrupt Log storage.
   */
  GPIOIntLogType                 log;

  GPIOIntDevCtxt                 *pGPIOIntDevCtxt;       
} GPIOIntCntrlType;


/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/
/*------------------------------------------------------------------------------
Declaring a private "GPIOInt" Vtable
------------------------------------------------------------------------------*/
typedef struct GPIOIntDALVtbl GPIOIntDALVtbl;
struct GPIOIntDALVtbl
{
	int (*GPIOIntr_DriverInit)(GPIOIntDrvCtxt *);
	int (*GPIOIntr_DriverDeInit)(GPIOIntDrvCtxt *);
};

struct GPIOIntDevCtxt
{
  //Base Members
  uint32                         dwRefs;                                    
  DALDEVICEID                    DevId;                                  
  uint32                         dwDevCtxtRefIdx; 
  GPIOIntDrvCtxt                 *pGPIOIntDrvCtxt;                             
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32                         Reserved[16];
  DALSYS_SYNC_OBJECT(GPIOIntSyncObject);
  DALSYSSyncHandle               gpioint_synchronization;
  int                            gpioint_qdi;

  //GPIOInt Dev state can be added by developers here
  GPIOIntCntrlType *             gpioint_cntrl;   
  //GPIOInt Device state can be added by developers here
  uint8 is_initialized;
};

struct GPIOIntDrvCtxt
{
  //Base Members
  GPIOIntDALVtbl GPIOIntDALVtbl;
  uint32  dwNumDev;
  uint32  dwSizeDevCtxt;
  uint32  bInit;
  uint32  dwRefs;
  GPIOIntDevCtxt GPIOIntDevCtxt[2];
    //GPIOInt Drv state can be added by developers here
};

/*------------------------------------------------------------------------------
Declaring a "GPIOInt" Client Context
------------------------------------------------------------------------------*/
struct GPIOIntClientCtxt
{
  //Base Members
  uint32  dwRefs;                     
  uint32  dwAccessMode;  
  void *pPortCtxt;
  GPIOIntDevCtxt *pGPIOIntDevCtxt;            
  GPIOIntHandle GPIOIntHandle; 
  //GPIOInt Client state can be added by developers here
  uint8 is_initialized;
};

DALResult GPIOIntr_DriverInit(GPIOIntDrvCtxt *);
DALResult GPIOIntr_DriverDeInit(GPIOIntDrvCtxt *);
DALResult GPIOIntr_DeviceInit(GPIOIntClientCtxt *);
DALResult GPIOIntr_DeviceDeInit(GPIOIntClientCtxt *);
DALResult GPIOIntr_Reset(GPIOIntClientCtxt *);
DALResult GPIOIntr_PowerEvent(GPIOIntClientCtxt *,DalPowerCmd, DalPowerDomain);
DALResult GPIOIntr_Open(GPIOIntClientCtxt *,uint32);
DALResult GPIOIntr_Close(GPIOIntClientCtxt *);
DALResult GPIOIntr_Info(GPIOIntClientCtxt *, DalDeviceInfo *, uint32);
DALResult GPIOIntr_InheritObjects(GPIOIntClientCtxt *,DALInheritSrcPram *,DALInheritDestPram *);



/*=============================================================================

     Functions internal to DalGPIOInt

===============================================================================


  FUNCTION      GPIOINTR_INIT_NPA

  DESCRIPTION   This ia standard registration function for the NPA server.
                Once the NPA resource is available we need to create client.
                This returns a client handle that can be used to issue 
                requests to the NPA resource we need.

  PARAMETERS    
  void          *context
  unsigned int  event_type
  void          *resource_name   
  The resource string name we need to issue requests to.
  unsigned int resource_strlen the length of the resource string. 

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.
============================================================================

  FUNCTION      GPIOINTR_UPDATE_NPA_REQUEST

  DESCRIPTION   Update the minimum voltage requirement. If all Interrupts are 
                registered which do not require a wakeup we need to request 
                for a lower voltage. Upon registration of a new interrupt 
                that requires a wakeup we need to cancel this request of 
                lower voltage.

  PARAMETERS    GPIOIntDevCtxt* device Device Context of the GPIOInt DAL.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.
==========================================================================


  FUNCTION      GPIOIntr_DefaultIsr

  DESCRIPTION   
  Default interrupt service routine for GPIO interrupts.Should not ever be 
  invoked.

  PARAMETERS
  GPIOIntDevCtxt *device The GPIOInt Driver's Device Context.
  gpio - which GPIO is this.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

=============================================================================*/


void GPIOIntr_DefaultIsr
(
  GPIOIntDevCtxt *device,
  uint32 gpio
);


/*=============================================================================

FUNCTION      GPIOIntr_GetTargetData

  DESCRIPTION  
  This is an internal function that sets up the target processor data that each 
  instance of interrupt controller needs.

  PARAMETERS    
  GPIOIntClientCtxt * :The client context pointer of the interrupt controller 
                       DAL.

  DEPENDENCIES  None.

  RETURN VALUE  
  DALResult.
  DAL_SUCCESS :
  is returned if the GPIOInt target specific data was successfully
  extracted from the parsed xml file.
  DAL_ERROR :
  DAL_ERROR is returned if the xml data was corrupt for any reason.
  SIDE EFFECTS  None.

=============================================================================*/


DALResult 
GPIOIntr_GetTargetData
(
  GPIOIntClientCtxt *pCtxt
);

/*=============================================================================

  FUNCTION      GPIOIntr_AttachToDals

  DESCRIPTION   
  This is a default function that consolidates all the other dal attaches that 
  are needed. i.e. This GPIO interrupt controller uses the functionality of 
  other Device access libraries. We need to make sure we have handles to access 
  these.
   
  PARAMETERS    
  GPIOIntDevCtxt *device: Device context of the GPIO interrupt controller dal. 

  DEPENDENCIES  None.

  RETURN VALUE  
  DALResult 
  DAL_SUCCESS: this indicates if the dal attach was a success.
  GPIOInt returns a DAL_ERROR otherwise.

  SIDE EFFECTS  None.

=============================================================================*/


DALResult 
GPIOIntr_AttachToDals
(
  GPIOIntDevCtxt *device
);

/*=============================================================================


  FUNCTION      GPIOIntr_RunIsr

  DESCRIPTION   
  Run the ISR associated with a GPIO.During power collapse, all apps gpio 
  interrupts are not clear, so the same interrupt will be fired after switched 
  back to apps processor.

  PARAMETERS  
  GPIOIntDevCtxt *device : The Device context pointer of the GPIOInt driver.
  gpio                   : which GPIO's ISR to execute.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

=============================================================================*/


void 
GPIOIntr_RunIsr
(
  GPIOIntDevCtxt *device,
  uint32 gpio
);

/*=============================================================================


  FUNCTION      GPIOIntr_GetDirectIntrTrigger

  DESCRIPTION
  This function gets the GPIO interrupt trigger that is routed to the main 
  level 1 interrupt controller. The main interrupt controller trigger 
  flags are different from the GPIOInt triggers. 

  PARAMETERS    
  GPIOIntTriggerType trigger : It takes in the GPIOInt trigger flag and gives
  back the DalInterrupt controller trigger.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

=============================================================================

  FUNCTION      GPIOIntr_LogEvent

  DESCRIPTION   
  This function can be used to log the GPIOInt driver.

  PARAMETERS   
  GPIOIntCntrlType *gpioint_cntrl : Pointer to the GPIOInt controller.
  uint32 gpio :                     The GPIO pin whose interrupt state needs to
                                    be logged.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

=============================================================================*/

void 
GPIOIntr_LogEvent
(
  GPIOIntDevCtxt *device,
  uint32 gpio
);

/*=============================================================================

  FUNCTION      GPIOIntr_GetFreeDirectConnectInterruptId

  DESCRIPTION   
  This function is used to get a free direct connect interrupt line if
  available oterwise it gives back a default value of GPIOINT_NONE for
  a summary interrupt.

  PARAMETERS   
  GPIOIntDevCtxt * device : The device context of the GPIOInt driver
  uint32 gpio :             The GPIO pin whoes interrupt id needs to be queried.

  DEPENDENCIES  None.

  RETURN VALUE  
  For avai

  SIDE EFFECTS  None.

=============================================================================*/

uint32
GPIOIntr_GetFreeDirectConnectInterruptId
(
  GPIOIntDevCtxt * device,
  uint32 gpio
);


/*=============================================================================

  FUNCTION      GPIOIntr_GetMPMProcId

  DESCRIPTION   
  This is an internal function that gets the Msm power manager's processor id.

  PARAMETERS    GPIOIntProcessorType processor

  DEPENDENCIES  None.

  RETURN VALUE  maoint_processor_type.

  SIDE EFFECTS  None.


=============================================================================

                                                    External APIs

 =============================================================================

  FUNCTION      GPIOIntr_ClearInterrupt

  DESCRIPTION   This function clears the summary interrupt without 
                deregistering the interrupt handler.
   
  PARAMETERS    
  GPIOIntClientCtxt * pCtxt : Pointer to the driver client context .
  uint32 gpio :               GPIO pin number.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult : this indicates if the dal attach was a failure.

  SIDE EFFECTS  None.

=============================================================================*/

DALResult 
GPIOIntr_ClearInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
);


/*=============================================================================
  
  FUNCTION      GPIOIntr_SetTrigger

  DESCRIPTION   This is an function sets the trigger of a given GPIO.
                interrupt.

  PARAMETERS     
  GPIOIntClientCtxt * :         The client context pointer of the interrupt 
                                controller DAL.
  uint32 gpio :                Number of the interrupt.
  GPIOIntTriggerType trigger : Type of interrupt trigger.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult.

  SIDE EFFECTS  None.

=============================================================================*/

DALResult GPIOIntr_SetTrigger
( 
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio, 
  GPIOIntTriggerType  trigger
);

/*=============================================================================
  FUNCTION      GPIOIntr_RegisterIsr

  DESCRIPTION   This function registers the isr routine to a given GPIO 
  interrupt.Whenever an interrupt is fired the isr routine registered by the 
  user is called.

  PARAMETERS     
  GPIOIntClientCtxt * :        The client context pointer of the interrupt 
                               controller DAL.
  uint32  gpio :               GPIO pin number of the interrupt.
  GPIOIntTriggerType trigger : Interrupt trigger specified by the client for 
                               GPIO signal transitions.
  GPIOIntHandlerType  isr :    The Interrupt Service routine for the callback 
                               function pointer.
  uint32 param :               The uint32 parameter given to the isr routine 
                                when called.

  DEPENDENCIES  None.

  DAL_ERROR :
  Returns an Error if the GPIOInt driver was not able to register the interrupt
  service routine (ISR). This can happen if the GPIO interrupt already has a 
  prior event handle or a callback function interrupt Service routine registered 
  to it which is different from the current one in attempt at event registration. 
  An error is also returned if the GPIO pin is out of bounds or not supported.
  DAL_SUCCESS :
  Returns a success if the ISR registration was a success.

  SIDE EFFECTS  None.

=============================================================================*/

DALResult GPIOIntr_RegisterIsr
(
  GPIOIntClientCtxt * pCtxt,
  uint32 gpio,
  GPIOIntTriggerType trigger,
  GPIOINTISR isr,
  GPIOINTISRCtx param
);

/*===========================================================================

  FUNCTION      GPIOIntr_DeregisterIsr

  DESCRIPTION   This function deregisters the isr routine to a given GPIO 
                interrupt.Whenever an interrupt is fired the isr routine 
                registered by the user is called.

  PARAMETERS     
  GPIOIntClientCtxt * :    The client context pointer of the interrupt controller 
                           DAL.
  uint32 gpio         :    Interrupt number.
  GPIOIntHandlerType isr : The interrupt service routine (ISR) registered for 
                           that GPIO.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult.

  SIDE EFFECTS  None.

=============================================================================*/

DALResult GPIOIntr_DeregisterIsr
( GPIOIntClientCtxt * pCtxt, 
  uint32  gpio, 
  GPIOINTISR isr
);


/*=============================================================================

  FUNCTION      GPIOIntr_IsInterruptEnabled

  DESCRIPTION   This function tells the user if the given GPIO interrupt is 
                enabled or not. For Direct connect interrupts. It tells the 
                user if the main level 1 interruptto which the secondary GPIO 
                interrupt line is hooked up to is enabled.

  PARAMETERS    GPIOIntClientCtxt *:
                the client context pointer of the interrupt controller DAL.
                uint32 gpio
                uint32 * state that is given back to the user that 
                indicates whether the given interrupt is enabled.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult.

  SIDE EFFECTS  None.

=============================================================================*/

DALResult 
GPIOIntr_IsInterruptEnabled
(
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *  state
);
  
/*=============================================================================

  FUNCTION      GPIOIntr_IsInterruptPending

  DESCRIPTION   This function tells the user if the given gpio interrupt is 
                pending or not.

  PARAMETERS    GPIOIntClientCtxt * :
                the client context pointer of the interrupt controller DAL.
                uint32 gpio
                uint32 * state that is given back to the user that 
                indicates whether the given interrupt is pending.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult.

  SIDE EFFECTS  None.

=============================================================================*/

DALResult 
GPIOIntr_IsInterruptPending
( 
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *  state 
);


/*=============================================================================

  FUNCTION      GPIOIntr_IsInterruptSet

  DESCRIPTION   This function tells the user if the given gpio interrupt is 
                fired or not.This is regardless of it being enabled.

  PARAMETERS    GPIOIntClientCtxt * :
                the client context pointer of the interrupt controller DAL.
                uint32 gpio
                uint32 * state that is given back to the user that 
                indicates whether the given interrupt is pending.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult.

  SIDE EFFECTS  None.

=============================================================================*/

DALResult 
GPIOIntr_IsInterruptSet
( 
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *  state 
);

/*=============================================================================
  
  FUNCTION      GPIOIntr_Save

  DESCRIPTION   
  This function is the default power saving function that saves any volatile data 
  or state during power collapse.

  PARAMETERS    
  GPIOIntClientCtxt * :        The client context pointer of the interrupt 
                               controller DAL.
  GPIOIntPowerCollapseType :   Type of power collapse the chip is in.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult.

  SIDE EFFECTS  None.
=============================================================================*/

DALResult 
GPIOIntr_Save
( 
  GPIOIntClientCtxt * pCtxt
);

/*=============================================================================


  FUNCTION      GPIOIntr_Restore

  DESCRIPTION   This function is the default power saving function that 
                restores any saved data or state during power collapse upon 
                coming out of power collapse. As of now there is no percieved 
                implementation of this function in use.

  PARAMETERS    GPIOIntClientCtxt * :
                the client context pointer of the interrupt controller DAL.
                GPIOIntPowerCollapseType type of power collapse the 
                chip is in.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult.

  SIDE EFFECTS  None.

=============================================================================*/

DALResult GPIOIntr_Restore
( 
  GPIOIntClientCtxt *
);

/*=============================================================================
  
  FUNCTION      GPIOIntr_DisableInterrupt

  DESCRIPTION   This is an internal function that is used to disable the 
                SUMMARY and DIRECT CONNECT interrupts.

  PARAMETERS    GPIOIntClientCtxt * :
                the client context pointer of the interrupt controller DAL.
                uint32 gpio number to which the interrupt is assigned.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult.

  SIDE EFFECTS  None.

=============================================================================*/

DALResult GPIOIntr_DisableInterrupt
( 
  GPIOIntClientCtxt * pCtxt,
  uint32 gpio
);



/*=============================================================================

  FUNCTION      GPIOIntr_EnableInterrupt

  DESCRIPTION   
  This is an internal function used to enable the registered interrupt.

  PARAMETERS    
  GPIOIntClientCtxt * pCtxt : 
  uint32 gpio :              

  DEPENDENCIES  None.

  RETURN VALUE  maoint_processor_type.

  SIDE EFFECTS  None.


=============================================================================*/

DALResult GPIOIntr_EnableInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
);

/*=============================================================================

  FUNCTION      GPIOIntr_TriggerInterrupt

  DESCRIPTION   
  This function is used to trigger an interrupt without the GPIO interrupt 
  being actually set in hardware.

  PARAMETERS    
  GPIOIntClientCtxt * pCtxt : Client Context of the GPIOInt Driver.
  uint32 gpio               : GPIO pin whose interrupt needs to be triggered.
  DEPENDENCIES  None.

  RETURN VALUE  maoint_processor_type.

  SIDE EFFECTS  None.


=============================================================================*/

DALResult 
GPIOIntr_TriggerInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio
);

/*=============================================================================

  FUNCTION      GPIOIntr_MapMPMInterrupts

  DESCRIPTION   
  This function is used by the modem power manager to map the GPIO interrupts
  that can wakeup the msm from low power mode on being triggered.

  PARAMETERS    
  GPIOIntClientCtxt * pCtxt : Client Context of the GPIOInt Driver.
  uint32 gpio               : GPIO pin whose interrupt needs to be mapped as
                              a mpm wakeup interrupt.
  uint32 mpm_interrupt_id   : Each GPIO interupt that is a wakeup interrupt
                              has a corresponding MPM interrupt id.

  DEPENDENCIES  None.

  RETURN VALUE  DALResult
  DAL_ERROR :    This error code is returned if the GPIO pin is unsupported.
  DAL_SUCCESS :  This result code is returned if the gpio interrupt is supported.

  SIDE EFFECTS  None.


=============================================================================*/

DALResult GPIOIntr_MapMPMInterrupt
(
  GPIOIntClientCtxt * pCtxt,
  uint32 gpio, 
  uint32 mpm_interrupt_id
); 

/*=============================================================================

   External APIs specific to DalGPIOInt interface for MDM 9k

=============================================================================

  FUNCTION      GPIOIntr_AttachRemote

  DESCRIPTION   
  This is mdm9k specific function that is used by GPIOINt to attach to its 
  remote component on master and peripheral processors. This attach is 
  primarily used for inter processor communication for relaying interrupts
  fired on the master processor

  PARAMETERS
  GPIOIntClientCtxt*  pCtxt : Client context of the GPIOInt Driver
  uint32 processor :          The remote processor id used to specify which 
                              GPIOINT device to communicate to. 

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.


=============================================================================*/

DALResult GPIOIntr_AttachRemote
(
  GPIOIntClientCtxt*  pCtxt,
  uint32 processor
);

/*=============================================================================

  FUNCTION      GPIOIntr_MonitorInterrupts

  DESCRIPTION   
  This is an MDM9k specific function monitors all the gpio interrupts on the
  master processor so that the respective processor can be woken up depending 
  the entended GPIO interrupt destination. This is used by the MPM (allways on
  domain.)

  PARAMETERS
  GPIOIntClientCtxt * pCtxt : Client context of the GPIOInt Driver
  GPIOINTISR isr            : interupt service routine calback.
  uint32 enable             : flag to either enable or disable monitoring of 
                              interrupts.

  DEPENDENCIES  DALResult.
  DAL_SUCCESS : this is returned if the GPIO pin is within bounds.
  DAL_ERROR   : Otherwise.
  RETURN VALUE  None

  SIDE EFFECTS  None.


=============================================================================*/

DALResult 
GPIOIntr_MonitorInterrupts
(
  GPIOIntClientCtxt * pCtxt,  
  GPIOINTISR isr,
  uint32 enable
);

#endif /*___DALGPIOInt_H*/
