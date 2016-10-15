#ifndef __AXICFGOS_H__
#define __AXICFGOS_H__
/**
 * @file axicfgos.h
 * @note Internal header file. Platform independent supporting API
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/core.adsp/2.2/buses/icb/src/common/axicfgos.h#1 $ 
 * $DateTime: 2013/04/03 17:22:53 $ 
 * $Author: coresvc $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 12/05/2011   dj      Created file
 *  
 *  
 * Copyright (c) 2011-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 */ 

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  @brief  Used to initialize platform dependent resources
  
  @return bool
      @retval true  if successfully initialized
      @retval false if initialization failed
 */
bool AxiCfgOs_Init( void );

/**
  @brief  Used to acquire the mutex. 
          Can only be called after AxiCfgOs_Init
 */
void AxiCfgOs_MutexLock( void ); 

/**
  @brief  Used to release the mutex acquired with AxiCfgOs_MutexLock
 */
void AxiCfgOs_MutexUnlock( void ); 

/**
  @brief  Used to map a hardware register base
          Can only be called after AxiCfgOs_Init

  @param  pBaseName   A char string that gives the register base name
  @param  uBaseAddr   The physical base address
  @param  uRegionSize The size of the register base area

  @return uint32_t 
          @retval 0         if mapping failed
          @retval virtaddr  if mapping succeeded
 */
uint32_t AxiCfgOs_Mmap( char * pBaseName, uint64_t uBaseAddr, uint32_t uRegionSize );

/**
  @brief  Used to fetch a value from xml property file in target config lib
          Can only be called after AxiCfgOs_Init

  @param  pString   Name of the property string

  @return void*   A pointer to the data
  */
void* AxiCfgOs_GetProperty( char * pString );

/**
  @brief  Used to fetch the clock id for a non-npa clock.

  @param  pName   The name of the clock
  @param  uId     A pointer to where to store the clock id

  @return bool
      @retval true  if successful
      @retval false if failed
  */
bool AxiCfgOs_GetClockId( const char * pName, uint32_t * puId );

/**
  @brief  Enable the given non-npa clock

  @param  uId   The clock id for the clock to enable
  */
void AxiCfgOs_EnableClock( uint32_t uId );

/**
  @brief  Disable the given non-npa clock

  @param  uId   The clock id for the clock to enable
  */
void AxiCfgOs_DisableClock( uint32_t uId );

#ifdef __cplusplus
}
#endif

#endif /* __AXICFGOS_H__ */
