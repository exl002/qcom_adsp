/**
@file rcxh.c
@brief This file contains the implementation details for the Exception Handling service.
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.adsp/2.2/debugtools/rcxh/src/rcxh.c#1 $
$DateTime: 2013/04/03 17:22:53 $
$Author: coresvc $
===============================================================================*/

#include "err.h"
#include "rcxh.h"

/**
TLS Dependency; Determine if POSIX or QURT
*/
/* #include "pthread.h" */
#include "qurt.h"

/**
Internal, Structure of states and variables useful for instrumentation by debugger
*/
static struct rcxh_internal_s
{
   // pthread_key_t tls_key;                                                     /**< posix */ sm

   int tls_key;                                                                  /**< qurt */

} rcxh_internal;

/**
Internal, Scope of current exception handler Try/Catch/Finally/EndTry
@returns
Current scope of active exception handler Try/Catch/Finally/EndTry
*/
static _rcxh_scope_p _rcxh_get_scope(void)                                       /**< TLS accessor */
{
   // return ((_rcxh_scope_p)pthread_getspecific(rcxh_internal.tls_key));        /**< posix */ sm

   return ((_rcxh_scope_p)qurt_tls_get_specific(rcxh_internal.tls_key));         /**< qurt */
}

/**
Internal, Scope of current exception handler Try/Catch/Finally/EndTry
@param[in] scope Sets current scope for active exception handler Try/Catch/Finally/EndTry
@returns
NHLOS TLS return status
*/
static int _rcxh_set_scope(_rcxh_scope_p scope)                                  /**< TLS accessor */
{
   // return (pthread_setspecific(rcxh_internal.tls_key, (void*)scope));         /**< posix */ sm

   return (qurt_tls_set_specific(rcxh_internal.tls_key, (void*)scope));          /**< qurt */
}

/**
Internal, Exception handler root "class" for all derived exception "class"
*/
struct _rcxh_s __rcxh_undef = { "undef", NULL };   /**< Exception handler root class "undef" */
struct _rcxh_s* _rcxh_undef = &__rcxh_undef;       /**< Exception handler root class "undef" */

/**
Internal, Handler routine of Try/Catch/Finally/EndTry Throw implementation
@param[in] file NULL terminated string, file name of throw
@param[in] line Integer, line number of throw
@param[in] handle Handle to the exception "class" of throw
@returns
None.
*/
void _rcxh_throw(char* file, int line, RCXH_HANDLE handle)
{
   _rcxh_scope_p scope = _rcxh_get_scope();                                      /**< TLS accessor */

   if (NULL != scope)                                                            /**< NULL check */
   {
      if (_rcxh_where_finally == scope->where || _rcxh_where_exp == scope->where) /**< throw upward from from any where Finally or Expression */
      {
         _rcxh_set_scope(scope->old);                                            /**< TLS accessor */

         _rcxh_throw(file, line, handle);                                        /**< throw upward from from any where Finally or Expression */
      }

      scope->src = scope->where;                                                 /**< record source of throw */

      scope->file = file;                                                        /**< record source of throw */

      scope->line = line;                                                        /**< record source of throw */

      scope->handle = handle;                                                    /**< record source of throw */

      if (NULL == scope->frame)                                                  /**< react to the possiblity of NULL storage space to appease static analysis tools, scope is not dynamic */
      {
         ERR_FATAL("uncaught exception", 0, 0, 0);                               /**< react to the possiblity of NULL storage space to appease static analysis tools, scope is not dynamic */
      }
      else
      {
         scope->where = _rcxh_where_exp;                                         /**< record current scope where */

         scope->state = _rcxh_state_thrown;                                      /**< record current scope state */

         longjmp(scope->frame, RCXH_TRUE);                                       /**< unwind scope by longjmp */
      }
   }
   else
   {
      ERR_FATAL("uncaught exception", 0, 0, 0);                                  /**< react to the possiblity of NULL storage space to appease static analysis tools, scope is not dynamic */
   }
}

/**
Internal, Handler routine of Try/Catch/Finally/EndTry Try clause initialization implementation
@param[in] scope Current exception handler scope
@returns
None.
*/
void _rcxh_scope_init(_rcxh_scope_p scope)
{
   if (NULL != scope)                                                            /**< NULL check */
   {
      scope->old = _rcxh_get_scope();                                            /**< TLS accessor */

      scope->where = _rcxh_where_try;                                            /**< defined */
      scope->state = _rcxh_state_unthrown;                                       /**< defined */
      scope->src = _rcxh_where_none;                                             /**< undefined until _rcxh_state_thrown */
      scope->file = NULL;                                                        /**< undefined until _rcxh_state_thrown */
      scope->line = 0;                                                           /**< undefined until _rcxh_state_thrown */
      scope->handle = NULL;                                                      /**< undefined until _rcxh_state_thrown */

      _rcxh_set_scope(scope);                                                    /**< TLS accessor */
   }
   else
   {
      ERR_FATAL("uncaught exception", 0, 0, 0);                                  /**< react to the possiblity of NULL storage space to appease static analysis tools, scope is not dynamic */
   }
}

/**
Internal, Handler routine of Try/Catch/Finally/EndTry Catch clause initialization implementation
@param[in] scope Current exception handler scope
@param[in] handle Handle for active exception handler Try/Catch/Finally/EndTry
@returns
RCXH_TRUE -- Catching exception
RCXH_FALSE -- Not catching exception
*/
RCXH_BOOL _rcxh_catching(_rcxh_scope_p scope, RCXH_HANDLE handle)
{
   int rc = RCXH_FALSE;                                                          /**< return result */

   if (NULL != scope)                                                            /**< NULL check */
   {
      if (_rcxh_where_catch != scope->src)                                       /**< evaluate only if not yet caught */
      {
         while (RCXH_FALSE == rc && NULL != handle)                              /**< iterate looking for matching exception "class" */
         {
            RCXH_HANDLE prev;                                                    /**< starting handle */

            for (prev = scope->handle; RCXH_FALSE == rc && NULL != prev; prev = prev->parent) /**< iteration over exception "class" to locate handler */
            {
               if (prev == handle)                                               /**< explict catch */
               {
                  scope->where = _rcxh_where_catch;                              /**< record current scope where */

                  scope->state = _rcxh_state_caught;                             /**< record current scope state */

                  rc = RCXH_TRUE;                                                /**< has been caught */
               }
            }
         }
      }
   }
   else
   {
      ERR_FATAL("uncaught exception", 0, 0, 0);                                  /**< react to the possiblity of NULL storage space to appease static analysis tools, scope is not dynamic */
   }

   return rc;                                                                    /**< return result */
}

/**
Internal, Handler routine of Try/Catch/Finally/EndTry Finally clause initialization implementation
@param[in] scope Current exception handler scope
@returns
None.
*/
void _rcxh_finally(_rcxh_scope_p scope)
{
   if (NULL != scope)                                                            /**< NULL check */
   {
      scope->where = _rcxh_where_finally;                                        /**< record where */
   }
   else
   {
      ERR_FATAL("uncaught exception", 0, 0, 0);                                  /**< react to the possiblity of NULL storage space to appease static analysis tools, scope is not dynamic */
   }
}

/**
Internal, Handler routine of Try/Catch/Finally/EndTry EndTry clause initialization implementation
@param[in] scope Current exception handler scope
@returns
None.
*/
void _rcxh_end(_rcxh_scope_p scope)
{
   if (NULL != scope)                                                            /**< NULL check */
   {
      _rcxh_set_scope(scope->old);                                               /**< TLS accessor */

      if (_rcxh_state_thrown == scope->state)                                    /**< check state */
      {
         _rcxh_throw(scope->file, scope->line, scope->handle);                   /**< throw upwards */
      }
   }
   else
   {
      ERR_FATAL("uncaught exception", 0, 0, 0);                                  /**< react to the possiblity of NULL storage space to appease static analysis tools, scope is not dynamic */
   }
}

/**
Initialization of service prior to use
@return
None.
*/
void rcxh_init(void)
{
   // pthread_key_create(&rcxh_internal.tls_key, NULL);                        /**< posix */ sm

   qurt_tls_create_key(&rcxh_internal.tls_key, NULL);                            /**< qurt */
}

/**
Termination of service following use
@return
None.
*/
void rcxh_term(void)
{
   /* NULL */ /* DECISION TO NOT CLEANUP SERVICE FOR POST MORTEM REASONS */
}
