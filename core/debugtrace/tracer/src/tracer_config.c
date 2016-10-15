/*===========================================================================
  FILE: tracer_config.c

  OVERVIEW: QUALCOMM Debug Subsystem (QDSS) Tracer configuration and
  control of trace output generation.

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //components/rel/core.adsp/2.2/debugtrace/tracer/src/tracer_config.c#3 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
//#include <stringl\stringl.h>   // memset (not available Venus, LPASS)
#include <string.h>

#include "comdef.h"

#include "qdss.h"
#include "tracer.h"
#include "tracer_event_ids.h"
#include "tracer_mutex.h"
#include "tracer_config.h"
#ifndef TRACER_VENUS_BUILD
#include "tracer_diag.h"
#endif
#include "tracer_portmgr.h"
#include "tracer_stp.h"
#include "qdss_imem.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
/** Entity tables (values and name).
 */
#undef DEFINE_ENTITY
#define DEFINE_ENTITY(txt, val, cmt) val,
tracer_ost_entity_id_t tracer_entity_vals[] =
{
   #include "tracer_entity_defn.h"
};
#define TRACER_NUM_USER_ENTITIES  (sizeof(tracer_entity_vals) /  \
                                   sizeof(tracer_entity_vals[0]))
//=========================================================================//
/**
  @brief Base size for bit array to enable (disable) output per
         entity.
 */
typedef uint32 entity_op_ctrl_t;
typedef uint32 event_op_ctrl_t;

#define ENTITY_OP_CTRL_TOTAL_ITEMS \
      ((TRACER_ENTITY_MAX >> 3) / sizeof(entity_op_ctrl_t)) + \
      ((TRACER_ENTITY_MAX & ((sizeof(entity_op_ctrl_t) << 3) - 1)) ? \
       1 : 0)

#define EVENT_OP_CTRL_TOTAL_ITEMS \
      ((TRACER_EVENT_ID_MAX >> 3) / sizeof(event_op_ctrl_t)) + \
      ((TRACER_EVENT_ID_MAX & ((sizeof(event_op_ctrl_t) << 3) - 1)) ? \
      1 : 0)
//=========================================================================//
/**
  @brief Structure containing tracer configuration.
 */
typedef struct {

   /** Flag indicating the tracer is to suppress all trace output
    *  requests.
    */
   boolean output_enabled;

   /** Bit mask for processing (bit set) or suppressing (bit
    *  cleared) traces associated with an entity id. Entity Id
    *  value maps to corresponding bit position.
    */
   entity_op_ctrl_t entity_op_ctrl[ENTITY_OP_CTRL_TOTAL_ITEMS]; //bit array

   int32 entity_in_use_count;

   event_op_ctrl_t event_op_ctrl[EVENT_OP_CTRL_TOTAL_ITEMS]; //bit array

   uint32 event_in_use_count;

   uint32 systest_ctl_addr;
} tracer_config_t;


/*---------------------------------------------------------------------------
 * External Variable Declarations
 * ------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
/**  Mutex in not self initialzing. Fudge it.
 */
static boolean bTracerMutexInitialized = FALSE;

/** Flag indicating the tracer has been initialized and is ready
    to output trace data.
 */
static volatile boolean bTracerInitialized = FALSE;

/** Flag indicating the tracer has been initialized for and
    ready to output handleless SW events.
 */
static boolean bTracerEventSimpleInitialized = FALSE;

/** Tracer configuration parameters.
 */
static tracer_config_t tracer_config;

/*---------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ------------------------------------------------------------------------*/
extern void tracer_init_client_list(void);

#if TRACER_TESTING_ON
int tracer_test(void);
#endif

//=========================================================================//
// Mapping of a value to its corresponding position in a bit array
// Base data type of array could be 8, 16, 32, or 64-bit ==>
// DataTypeBytes = 1, 2, 4, or 8 respectively.
//
// Array elements needed
//  = (int)(MAX_VALUES/8/typebytes) + ([MAX_VALUE & ((8*typebytes) - 1)] ? 1 : 0 )
//  = (MAX_VALUES >> 3) / typebytes) + ([MAX_VALUE & ((typebytes<<3)- 1)] ? 1 : 0 )
// Value = (DataTypeBytes * 8 * ArrayIndex) + BitPos
// BitPos = Value % (DataTypeBytes * 8)   <-- modulo or remainder of division
//    Since each is a multiple of 8 just grab the appropriate # of lsbs,
//    = Value & (((sizeof((pArray)[0])) << 3) - 1)
// ArrayIndex = (int) Value / (int)(DataTypeBytes * 8)  <-- Integer division

#define ARRAY_IDX(pArray, val)      \
   ( (int)(val) / ((int)(sizeof((pArray)[0]) << 3)) )

#define ELEM_BIT(type, pArray, val) \
   ((type)((val) & ((sizeof((pArray)[0]) << 3) - 1)))

#define MASK_BIT(type, pArray, val) \
   ((type)(((uint32)1) << ELEM_BIT(type, pArray, val)))

#define SET_ARRAY_BIT(type, pArray, value)      \
   {                                            \
      if (((value) >> 3) < sizeof((pArray)))    \
      {                                         \
         (pArray)[ARRAY_IDX(pArray, value)] |=  \
            MASK_BIT(type, pArray, value);      \
      }                                         \
   }

#define CLR_ARRAY_BIT(type, pArray, value)      \
   {                                            \
      if (((value) >> 3) < sizeof((pArray)))    \
      {                                         \
         pArray[ARRAY_IDX(pArray, value)] &=    \
            ~(MASK_BIT(type, pArray, value));   \
      }                                         \
   }

#define GET_ARRAY_BIT(type, pArray, value)      \
      (((pArray)[ARRAY_IDX(pArray, value)] &    \
      (MASK_BIT(type, pArray, value))) ? 1 : 0)

/*-------------------------------------------------------------------------*/
// Dummy function when not using sw events.
tracer_cmdresp_t _tcfg_rtnerr(void) { return TRACER_CMDRESP_FAIL; }

/*---------------------------------------------------------------------------
 * Internalized Function Definitions
 * ------------------------------------------------------------------------*/

//=========================================================================//
// Access data tracer configuration data.
//=========================================================================//
/*-------------------------------------------------------------------------*/
boolean tcfg_is_tracer_initialized(void)
{
   return (bTracerInitialized);
}
/*-------------------------------------------------------------------------*/
boolean tcfg_is_event_initialized(void)
{
   return (bTracerEventSimpleInitialized);
}

/*-------------------------------------------------------------------------*/
int32 tcfg_get_entity_count(void)
{
   if (bTracerInitialized)
   {
      return tracer_config.entity_in_use_count;
   }
   else
   {
      return 0;
   }
}

/*-------------------------------------------------------------------------*/
uint32 tcfg_get_event_count(void)
{
   if (bTracerInitialized)
   {
      return tracer_config.event_in_use_count;
   }
   else
   {
      return 0;
   }
}

/*-------------------------------------------------------------------------*/
void * tcfg_cpy_event_tag(void * dest, int numBytes)
{
   char tag[] = EVENT_TABLE_TAG;
   size_t tag_size = sizeof(tag) - 1; //tag size without null terminator

   if (tag_size < numBytes)
   {
      memset(dest, '\0', numBytes);
      numBytes = tag_size;
   }
   //copy last numBytes chars
   memcpy (dest, &tag[tag_size-numBytes], numBytes);
   return dest;
}

/*-------------------------------------------------------------------------*/
void * tcfg_cpy_entity_op_ctrl(void *dest, int numBytes)
{
   if (bTracerInitialized)
   {
      if (sizeof(tracer_config.entity_op_ctrl) < numBytes)
      {
         memset(dest, 0, numBytes);
         numBytes = sizeof(tracer_config.entity_op_ctrl);
      }
      memcpy (dest, tracer_config.entity_op_ctrl, numBytes);
   }
   else
   {
      memset(dest, 0, numBytes);
   }
   return dest;
}

/*-------------------------------------------------------------------------*/
void * tcfg_cpy_event_op_ctrl(void *dest, uint8 group, int dest_size)
{
   int ctrl_size;
   int copy_bytes;

   memset(dest, 0, dest_size);
   if (bTracerInitialized)
   {
      ctrl_size = sizeof(tracer_config.event_op_ctrl);

      if (ctrl_size < (group * dest_size))
      {  // Request ID range starts beyond target's range.
         return dest;
      }
      if (ctrl_size < ((group + 1) * dest_size))
      {  // Request ID range extends beyond target's range.
         copy_bytes = ctrl_size - (group * dest_size);
      }
      else
      {
         copy_bytes = dest_size;
      }
      memcpy(dest, &(tracer_config.event_op_ctrl[
             (group * dest_size) / sizeof(event_op_ctrl_t)]),
             copy_bytes);
   }
   return dest;
}

//=========================================================================//
// Setting data tracer configuration data.
//=========================================================================//
/*-------------------------------------------------------------------------*/
static tracer_cmdresp_t _entity_op_set_all(tracer_entity_state_t state)
{
   tracer_cmdresp_t ret_val;
   int ii;

   ret_val = TRACER_CMDRESP_S_UNCHANGED;
   for (ii = 0; ii < TRACER_NUM_USER_ENTITIES; ii++)
   {
      if (TRACER_ENTITY_SWEVT != tracer_entity_vals[ii])
      {
         if (((0 == state) ? TRACER_ENTITY_OFF : TRACER_ENTITY_ON) !=
             GET_ARRAY_BIT(entity_op_ctrl_t, tracer_config.entity_op_ctrl, tracer_entity_vals[ii]))
         {
            if (TRACER_ENTITY_ON == state)
            {
               SET_ARRAY_BIT(entity_op_ctrl_t, tracer_config.entity_op_ctrl,
                             tracer_entity_vals[ii]);
               tracer_config.entity_in_use_count++;
            }
            else
            {
               CLR_ARRAY_BIT(entity_op_ctrl_t, tracer_config.entity_op_ctrl,
                             tracer_entity_vals[ii]);
               tracer_config.entity_in_use_count--;
            }
            ret_val = TRACER_CMDRESP_SUCCESS;
         }
      }
   }
   return ret_val;
}

/*-------------------------------------------------------------------------*/
static tracer_cmdresp_t _entity_op_set(tracer_ost_entity_id_enum_t eid,
                                   tracer_entity_state_t state)
{
   if (TRACER_ENTITY_SWEVT == eid)
   {
      return TRACER_CMDRESP_F_INVALID;
   }

   if (((0 == state) ? TRACER_ENTITY_OFF : TRACER_ENTITY_ON) !=
       GET_ARRAY_BIT(entity_op_ctrl_t, tracer_config.entity_op_ctrl, eid))
   {
      if (TRACER_ENTITY_ON == state)
      {
         SET_ARRAY_BIT(entity_op_ctrl_t, tracer_config.entity_op_ctrl, eid);
         tracer_config.entity_in_use_count++;
      }
      else
      {
         CLR_ARRAY_BIT(entity_op_ctrl_t, tracer_config.entity_op_ctrl, eid);
         tracer_config.entity_in_use_count--;
      }
      return TRACER_CMDRESP_SUCCESS;
   }
   return TRACER_CMDRESP_S_UNCHANGED;
}
/*-------------------------------------------------------------------------*/
// Preserve event id 0 - Reserved for internal use (TRACER_EVENT_RESERVE_0).
#if TRACER_SWEVT
static tracer_cmdresp_t _event_op_set_all(tracer_event_state_t state)
{
   int ii;
   int saveBit;

   saveBit = GET_ARRAY_BIT(event_op_ctrl_t,
                           tracer_config.event_op_ctrl,
                           TRACER_EVENT_RESERVE_0);
   if (TRACER_EVENT_ON == state)
   {
      if ((TRACER_EVENT_ID_MAX - 1) !=
          tracer_config.event_in_use_count)
      {
         memset (tracer_config.event_op_ctrl, 0xFF,
                 TRACER_EVENT_ID_MAX >> 3);
         for (ii = (TRACER_EVENT_ID_MAX & (~(uint32)0x07));
              ii < TRACER_EVENT_ID_MAX; ii++)
         {
            SET_ARRAY_BIT(event_op_ctrl_t, tracer_config.event_op_ctrl,
                          ii);
         }
         if (0 == saveBit)
         {
            CLR_ARRAY_BIT(event_op_ctrl_t,
                          tracer_config.event_op_ctrl,
                          TRACER_EVENT_RESERVE_0);
         }
         tracer_config.event_in_use_count =
            TRACER_EVENT_ID_MAX - 1; // less RESERVE0
         return TRACER_CMDRESP_SUCCESS;
      }
   }
   else
   {
      if (0 != tracer_config.event_in_use_count)
      {
         //for (ii = 1; TRACER_EVENT_ID_MAX > ii; ii++)
         //{
         //   tracer_event_tbl[ii].evt_id = TRACER_EVENT_ID_MAX;
         //}
         memset (tracer_config.event_op_ctrl, 0,
                 sizeof(tracer_config.event_op_ctrl));
         if (1 == saveBit)
         {
            SET_ARRAY_BIT(event_op_ctrl_t,
                          tracer_config.event_op_ctrl,
                          TRACER_EVENT_RESERVE_0);
         }

         tracer_config.event_in_use_count = 0;
         return TRACER_CMDRESP_SUCCESS;
      }
   }
   return TRACER_CMDRESP_S_UNCHANGED;
}
#endif
/*-------------------------------------------------------------------------*/
// Evt_id must be within valid range.
// Can be used internally to set reserved event id 0.
#if TRACER_SWEVT
static tracer_cmdresp_t _event_op_set(tracer_event_id_t event_id,
                                  tracer_event_state_t state)
{
   if (TRACER_EVENT_ON == state)
   {
      //if (event_id != tracer_event_tbl[event_id].evt_id)
      if (0 == GET_ARRAY_BIT(event_op_ctrl_t,
          tracer_config.event_op_ctrl, event_id))
      {
         //tracer_event_tbl[event_id].evt_id = event_id;
         SET_ARRAY_BIT(event_op_ctrl_t, tracer_config.event_op_ctrl, event_id);
         tracer_config.event_in_use_count++;
         return TRACER_CMDRESP_SUCCESS;
      }
   }
   else
   {
      //if (event_id == tracer_event_tbl[event_id].evt_id)
      if (1 == GET_ARRAY_BIT(event_op_ctrl_t,
          tracer_config.event_op_ctrl, event_id))
      {
         //tracer_event_tbl[event_id].evt_id = TRACER_EVENT_ID_MAX;
         CLR_ARRAY_BIT(event_op_ctrl_t, tracer_config.event_op_ctrl, event_id);
         tracer_config.event_in_use_count--;
         return TRACER_CMDRESP_SUCCESS;
      }
   }
   return TRACER_CMDRESP_S_UNCHANGED;
}
#endif

//=========================================================================//
// Check of tracer configuration settings.
//=========================================================================//
/*-------------------------------------------------------------------------*/
boolean tcfg_is_op_enabled(void)
{
   if (bTracerInitialized)
   {
      return (TRACER_OUTPUT_ENABLE == tracer_config.output_enabled);
   }
   else
   {
      return FALSE;
   }
}

/*-------------------------------------------------------------------------*/
boolean tcfg_is_entity_on(tracer_ost_entity_id_enum_t _eid)
{
   if (bTracerInitialized)
   {
      return (GET_ARRAY_BIT(entity_op_ctrl_t,
         tracer_config.entity_op_ctrl, _eid)) ?
         TRUE : FALSE;
   }
   else
   {
      return FALSE;
   }
}

/*-------------------------------------------------------------------------*/
boolean tcfg_is_event_on(tracer_event_id_t _eid)
{
   if (TRACER_EVENT_ID_MAX > _eid)
   {
      //return (tracer_event_tbl[_eid].evt_id == _eid);
      return ((1 == GET_ARRAY_BIT(event_op_ctrl_t,
          tracer_config.event_op_ctrl, _eid))? TRUE : FALSE);
   }
   else
   {
      return FALSE;
   }
}

/*-------------------------------------------------------------------------*/
__inline static boolean _is_entity_in_use(void)
{
   return (0 < tracer_config.entity_in_use_count) ? TRUE : FALSE;
}

/*-------------------------------------------------------------------------*/
__inline static boolean _is_event_in_use(void)
{
   return (0 < tracer_config.event_in_use_count) ? TRUE : FALSE;
}

/*-------------------------------------------------------------------------*/
__inline static boolean _is_tracer_idle(void)
{
   return ( (!bTracerInitialized) || (!tcfg_is_op_enabled()) ||
            ( (!_is_entity_in_use()) && (!_is_event_in_use()) )
          ) ? TRUE : FALSE;
}

//=========================================================================//
// Control functions
//=========================================================================//
/*=========================================================================*/
tracer_cmdresp_t tracer_cfg_op_ctrl(tracer_op_state_t state)
{
   tracer_cmdresp_t ret_val;

   if (state == tracer_config.output_enabled)
   {
      return TRACER_CMDRESP_S_UNCHANGED;
   }
   if (!bTracerInitialized)
   {
      return TRACER_CMDRESP_F_INIT;
   }

   ret_val = TRACER_CMDRESP_FAIL;
   tracer_mutex_lock();
   if (TRACER_OUTPUT_ENABLE == state)
   {
      if ( _is_entity_in_use() || _is_event_in_use() )
      {
         if (0 == QDSSOn())
         {
            tracer_config.output_enabled = state;
            ret_val = TRACER_CMDRESP_SUCCESS;
         }
         else
         {
            ret_val = TRACER_CMDRESP_F_AVAIL;
         }
      }
      else
      {
         tracer_config.output_enabled = state;
         ret_val = TRACER_CMDRESP_S_NO_OUTPUT;
      }
   }
   else
   {
      tracer_config.output_enabled = state;
      QDSSOff();
      ret_val = TRACER_CMDRESP_SUCCESS;
   }
   tracer_mutex_unlock();

   return ret_val;
}

/*=========================================================================*/
__inline static boolean _is_entity_valid(tracer_ost_entity_id_enum_t _eid)
{
   int ii;
   if (TRACER_ENTITY_SWEVT == _eid)
   {
      return FALSE;  // Reserved, internal tracer use.
   }
   for (ii = 0; ii < TRACER_NUM_USER_ENTITIES; ii++ )
   {
      if (_eid == tracer_entity_vals[ii])
      {
         return TRUE;
      }
   }
   return FALSE;
}

/*-------------------------------------------------------------------------*/
tracer_cmdresp_t _tcfg_entity_ctrl(boolean bSetAll,
                                     tracer_ost_entity_id_enum_t eid,
                                     tracer_entity_state_t state)
{
   tracer_cmdresp_t ret_val;

   if (!bTracerInitialized)
   {
      return TRACER_CMDRESP_F_INIT;
   }
   if (!bSetAll)
   {
      if (!_is_entity_valid(eid))
      {
         return TRACER_CMDRESP_F_INVALID;
      }
      if (state == GET_ARRAY_BIT(entity_op_ctrl_t,
         tracer_config.entity_op_ctrl, eid))
      {
         return TRACER_CMDRESP_S_UNCHANGED;
      }
   }

   ret_val = TRACER_CMDRESP_FAIL;

   tracer_mutex_lock();
   if (TRACER_ENTITY_ON == state)
   {
      if (tcfg_is_op_enabled())
      {
         if (0 == QDSSOn())
         {
            ret_val = (bSetAll) ? _entity_op_set_all(state) :
                                  _entity_op_set(eid, state);
         }
      }
      else // !tcfg_is_op_enabled()
      {
         // Clock should already be off, make sure then enable.
         QDSSOff();
         ret_val = (bSetAll) ? _entity_op_set_all(state) :
                               _entity_op_set(eid, state);
         if (TRACER_CMDRESP_SUCCESS == ret_val)
         {
            ret_val = TRACER_CMDRESP_S_NO_OUTPUT;
         }
      }
   }
   else
   {  // Turn entity off
      ret_val = (bSetAll) ? _entity_op_set_all(state) :
                            _entity_op_set(eid, state);
   }
      if (_is_tracer_idle())
      {
         QDSSOff();
      }
   tracer_mutex_unlock();

   return ret_val;
}

/*-------------------------------------------------------------------------*/
tracer_cmdresp_t tracer_cfg_entity_ctrl(tracer_ost_entity_id_enum_t eid,
                                    tracer_entity_state_t state)
{
   return _tcfg_entity_ctrl(FALSE, eid, state);
}
/*-------------------------------------------------------------------------*/
tracer_cmdresp_t tracer_cfg_entity_ctrl_all(tracer_entity_state_t state)
{
   return _tcfg_entity_ctrl(TRUE, TRACER_ENTITY_ALL, state);
}

/*=========================================================================*/
#if TRACER_SWEVT
__inline static boolean _is_event_id_valid(tracer_event_id_t event_id)
{
   return ((TRACER_EVENT_ID_MAX > event_id) &&
          (TRACER_EVENT_RESERVE_0 != event_id));
}
#endif
/*-------------------------------------------------------------------------*/
tracer_cmdresp_t tracer_event_ctrl(tracer_event_id_t event_id,
                                  tracer_event_state_t state)
{
#if TRACER_SWEVT
   tracer_cmdresp_t ret_val;
   ret_val = TRACER_CMDRESP_FAIL;

   if (!bTracerInitialized)
   {
      return TRACER_CMDRESP_F_INIT;
   }

   if (!_is_event_id_valid(event_id))
   {
      return TRACER_CMDRESP_F_INVALID;
   }

   tracer_mutex_lock();
   if ((TRACER_EVENT_ON == state))
   {
      if (tcfg_is_op_enabled())
      {
         if (0 == QDSSOn())
         {
            ret_val = _event_op_set(event_id, state);
         }
      }
      else
      {
         // Clock should already be off, make sure then enable.
         QDSSOff();
         ret_val = _event_op_set(event_id, state);
         if (TRACER_CMDRESP_SUCCESS == ret_val)
         {
            ret_val = TRACER_CMDRESP_S_NO_OUTPUT;
         }
      }
   }
   else
   {
      ret_val = _event_op_set(event_id, state);
   }
      if (_is_tracer_idle())
      {
         QDSSOff();
      }
   tracer_mutex_unlock();
   return ret_val;
#else
   return TRACER_CMDRESP_F_INVALID;
#endif
}

/*-------------------------------------------------------------------------*/
tracer_cmdresp_t tracer_event_ctrl_all(tracer_event_state_t state)
{
#if TRACER_SWEVT
   tracer_cmdresp_t ret_val;

   if (!bTracerInitialized)
   {
      return TRACER_CMDRESP_F_INIT;
   }

   ret_val = TRACER_CMDRESP_FAIL;

   tracer_mutex_lock();
   if ((TRACER_EVENT_ON == state))
   {
      if (tcfg_is_op_enabled())
      {
         if (0 == QDSSOn())
         {
            ret_val = _event_op_set_all(TRACER_EVENT_ON);
         }
      }
      else
      {
         // Clock should already be off, make sure then enable.
         QDSSOff();
         ret_val = _event_op_set_all(TRACER_EVENT_ON);
         if (TRACER_CMDRESP_SUCCESS == ret_val)
         {
            ret_val = TRACER_CMDRESP_S_NO_OUTPUT;
         }
      }
   }
   else
   {
      ret_val = _event_op_set_all(TRACER_EVENT_OFF);
   }
      if (_is_tracer_idle())
      {
         QDSSOff();
      }
   tracer_mutex_unlock();

   return ret_val;
#else
   return TRACER_CMDRESP_F_INVALID;
#endif
}

/*-------------------------------------------------------------------------*/
void tcfg_event_set_default(void)
{
   int ii;
   char *pDest;
   char *pSrc;
   char tval;
   uint32 swevt_defcfg[] = TRACER_EVENT_PREENABLE_SCONS;


   tracer_config.systest_ctl_addr = qdss_imem_base();

   // Initialize with all disabled.
   memset(tracer_config.event_op_ctrl, 0,
          sizeof(tracer_config.event_op_ctrl));
   tracer_config.event_in_use_count = 0;

   if (QDSS_SYSTEST_EVENTS_ENABLE &
       qdss_get_systest_ctl(tracer_config.systest_ctl_addr))
   {
      // Configure sw events per scons settings
      pDest = (char *) tracer_config.event_op_ctrl;
      pSrc = (char *) swevt_defcfg;
      if (sizeof(swevt_defcfg) <= sizeof(tracer_config.event_op_ctrl))
      {
         for (ii = 0; ii < (sizeof(swevt_defcfg)); ii++)
         {
            pDest[ii] = pSrc[ii];
            tval = pDest[ii];
            while  (tval != 0)
            {
               tracer_config.event_in_use_count++;
               tval &= (tval - 1);
            }
         }
      }
      if (0 < tracer_config.event_in_use_count)
      {
         tracer_config.output_enabled = TRACER_OUTPUT_ENABLE;
      }
   }
}
//=========================================================================//
// Tracer initialization functions
//=========================================================================//
/*-------------------------------------------------------------------------*/
/**
  @brief One time initialization of the tracer library.

  The tracer library is self initializing. The first client that
  attempts to register with the tracer will trigger the tracer to
  initialize itself. Upon initialization the tracer:

    - attaches to the STM device driver;
    - initializes tracer information to default values;
    - and registers itself as a SW client.

  Upon successful initialization, tracer is placed in the initialized state.
  Otherwise tracer is placed in the uninitialized state.

  The tracer generates a status message, if possible, and placed it into
  the trace output stream.

  The STM device (and other QDSS hardware) must be initialized first
  by the resource and power manager (RPM) or elsewhere.

  May only be called once. All tracer client information is cleared.
  Thus client registrations are lost which may result in registered
  clients not properly unregistered (see tracer_unregister()).

  @return None. See note.
  @note bTracerInitialized indicates whether initialization succeeded
      or failed.
 */
void tcfg_tracer_init(void)
{
#ifdef TRACER_VENUS_BUILD
   QDSSInit();
#endif
   // Want tracer library to initialize itself. However...
   // No self or statically initialized mutex available. Argh!
   // May get initialized over and over again, by each who jumps in.
   if (FALSE == bTracerMutexInitialized)
   {
      tracer_mutex_init();
      bTracerMutexInitialized = TRUE;
   }

   tracer_mutex_lock();
   if (FALSE == bTracerInitialized)
   {
      // By default, tracer output is turned off on target unless one of
      // the following occurs
      // 1) A T32/Host is used to change the setting appropriately.
      // 2) Configured to initialize with pre-enable SW events.
      tracer_config.output_enabled = TRACER_OUTPUT_DISABLE;
      memset(tracer_config.entity_op_ctrl, 0,
             sizeof(tracer_config.entity_op_ctrl));
      tracer_config.entity_in_use_count = 0;

      tcfg_event_set_default();    //Check and set to default config.

      tracer_init_client_list();
#ifndef TRACER_VENUS_BUILD
      tracer_diag_init();
#endif

      if (TRACER_RETURN_SUCCESS == tracer_stp_attach_stm())
      {
         bTracerInitialized = TRUE;
      }
   }
   tracer_mutex_unlock();

#ifdef QDSS_STANDALONE_MODE
   if (bTracerInitialized)
   {
      tracer_cfg_entity_ctrl_all(TRACER_ENTITY_ON);
      tracer_event_ctrl_all(TRACER_EVENT_ON);
      tracer_cfg_op_ctrl(TRACER_OUTPUT_ENABLE);
   }
#endif

   return;
}

/*-------------------------------------------------------------------------*/
/**
  @brief One time initialization for simple (handle-less) tracer events.

  If using simple tracer events, processor must make this call
  during the its initialization sequence.

  @note bTracerEventSimpleInitialized indicates whether initialization
        succeeded or failed.
 */
void tcfg_event_simple_init(void)
{
   tcfg_tracer_init();

   tracer_mutex_lock();
   if (bTracerInitialized)
   {
      if (!bTracerEventSimpleInitialized)
      {
         // Once port manager is initialized, the port pool is never freed.
         bTracerEventSimpleInitialized = tracer_portmgr_init();
      }
   }
   tracer_mutex_unlock();

   tracer_event_simple(TRACER_EVENT_INIT_COMPLETE);

#if TRACER_TESTING_ON
   tracer_test();
#endif


   return;
}

/*-------------------------------------------------------------------------*/
tracer_cmdresp_t tcfg_set_systest_ctl(uint32 value)
{
   if (QDSS_INVALID_IMEM_ADDR == tracer_config.systest_ctl_addr ) {
      return TRACER_CMDRESP_FAIL;
   }

   value &= ~QDSS_SYSTEST_MAGIC_MASK;
   value |= QDSS_SYSTEST_MAGIC;
   __outpdw(tracer_config.systest_ctl_addr,value);
   return TRACER_CMDRESP_SUCCESS;

}
