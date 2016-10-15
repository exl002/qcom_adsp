#ifndef ADSP_AMDB_H
#define ADSP_AMDB_H
/*==============================================================================

Copyright (c) 2012-2013 Qualcomm Technologies, Incorporated.  All Rights Reserved. 
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

#include "Elite_APPI.h"
#include "Elite_CAPI.h"
#include "Elite_CAPI_new.h"

#include "adsp_amdb.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/******************************************************************************* 
 ADSP Audio Module DataBase (adsp_amdb) API Overview

 AMDB is a repository for APPI and CAPI based audio modules.  It allows both
 static and dynamic modules to be added, queried, and removed.
 
 Modules added to the database are tracked using numeric reference counting.
 The reference is incremented by one when the module is added to the database
 and for each time the module is successfully queried.  upon a successful query
 a pointer to the module is returned, the reference count can be incremented or
 decremented using the addref and release functions.  When no longer needed the
 reference count added when the module is queried must be released.
 
 For example:
 
 adsp_amdb_add_appi - sets ref count to 1
 adsp_amdb_get_appi() - increments reference count and returns pointer to module
 adsp_amdb_appi_addref - increments ref count
 adsp_amdb_appi_release - decrements ref count
 adsp_amdb_appi_release - decrements ref count
 adsp_amdb_remove_appi - decreement ref count, count is no zero and module is
                         freed
 
 As long as a module is being used a reference count must be non-zero.
 
 Once a module is queried, that module can be used to create instances of
 the APPI or CAPI objects.  The modules reference count is incremented each time
 an APPI or CAPI object is created and decremented when the APPI or CAPI End()
 method is called.
 
 To create an APPI object:
 
   call adsp_amdb_appi_getsize_f()
   allocate memory
   call adsp_amdb_appi_init_f() and pass the pointer allocated
 
 To create a CAPI object:
 
   call adsp_amdb_capi_new_f()
 
 Once an APPI or CAPI object is created that interface's End() method must be
 called to decrement the reference count.  If it is not then that object will
 never be freed.
 
 When dealing with dynamic audio modules the module will only be unloaded when
 all objects using it are freed (ref count goes to zero).
 
*******************************************************************************/

/******************************************************************************* 
  amdb_capi module, used to create CAPI based objects
*******************************************************************************/
typedef struct adsp_amdb_capi_t adsp_amdb_capi_t;

/******************************************************************************* 
  increment/decrement the amdb_capi module
*******************************************************************************/
uint32_t adsp_amdb_capi_addref(adsp_amdb_capi_t* me);
uint32_t adsp_amdb_capi_release(adsp_amdb_capi_t* me);

/******************************************************************************* 
  creates new CAPI object
*******************************************************************************/
ADSPResult adsp_amdb_capi_new_f(adsp_amdb_capi_t* me, uint32_t format,
                                uint32_t bps, ICAPI** capi_ptr_ptr);


/*******************************************************************************
  amdb_appi module, used to create APPI based objects
*******************************************************************************/
typedef struct adsp_amdb_appi_t adsp_amdb_appi_t;

/******************************************************************************* 
  increment/decrement the amdb_appi module
*******************************************************************************/
uint32_t adsp_amdb_appi_addref(adsp_amdb_appi_t* me);
uint32_t adsp_amdb_appi_release(adsp_amdb_appi_t* me);

/******************************************************************************* 
  queries size and then initializes an APPI object
*******************************************************************************/
ADSPResult adsp_amdb_appi_getsize_f(adsp_amdb_appi_t* me, const appi_buf_t* params_ptr,
                                    uint32_t* size_ptr);
ADSPResult adsp_amdb_appi_init_f(adsp_amdb_appi_t* me, appi_t* appi_ptr, bool_t* is_inplace_ptr,
                                 const appi_format_t* in_format_ptr,
                                 appi_format_t* out_format_ptr, appi_buf_t* info_ptr);

/******************************************************************************* 
  Get an APPI or CAPI module based on module ID
*******************************************************************************/
ADSPResult adsp_amdb_get_capi(int id, adsp_amdb_capi_t** capi_ptr_ptr);
ADSPResult adsp_amdb_get_appi(int id, adsp_amdb_appi_t** appi_ptr_ptr);

/******************************************************************************* 
  Add a dynamic CAPI module to the database
 
  id - module id
  getsize_str - address of the module's getsize()
  ctor_str - address of the module's ctor() function
*******************************************************************************/
ADSPResult adsp_amdb_add_static_capi(int id, capi_getsize_f getsize_f,
                                     capi_ctor_f ctor_f);

/******************************************************************************* 
  Add a dynamic APPI module to the database
 
  id - module id
  getsize_str - address of the module's getsize()
  init_str - address of the module's init() function
*******************************************************************************/
ADSPResult adsp_amdb_add_static_appi(int id, appi_getsize_f getsize_f,
                                     appi_init_f init_f);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ADSP_AMDB_H
