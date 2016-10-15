/*==============================================================================

FILE: DalTF_Bootstrap.h

DESCRIPTION: 

  Declaration and implementation of the DalTF Bootstrapper (BSL) API.
  The API selects and runs tests dumped into memory by the Trace32 debugger.
  The results are then collected in a result buffer and passed to the client
 
INITIALIZATION AND SEQUENCING REQUIREMENTS: 
    The execution sequence is as follows.
    DalTF_BSL_Init is called, initializing a test buffer structure.
    DalTF_BSL_Load_Tests is called, loading the tests using the T32
    DalTF_BSL_Start_Tests is called beginning the test run sequence
    with function declaration in DalTFi_BSL.h.

PUBLIC CLASSES: None
IALIZATION AND SEQUENCING REQUIREMENTS: N/A

      Copyright (c) 2012 QUALCOMM Technologies Incorporated.
              All Rights Reserved.
          QUALCOMM Proprietary/GTDR
==============================================================================*/

#ifndef _DALTF_BOOTSTRAP
#define _DALTF_BOOTSTRAP

#include "DDITF.h"
#include "DALSys.h"

typedef struct _daltf_bsl_results
{
    uint32 nrOfTests;               /*<-- Total number of tests in result */
    uint32 allTestsPassed;        /*<-- Flag indicating that all tests have passed*/
    uint32* resultList;             /*<-- List of each test result */ 
    uint32 resultsReady;            /*<-- Flag Indicating whether or not the results are ready*/
} BSL_Result_Buffer;

typedef struct _daltf_bsl_test_buffer
{
    char *startBuffer;                /*<-- Start of the test list buffer */
    volatile uint32 size;           /*<-- Size of the test list buffer in bytes */
    volatile uint32 is_Full;        /*<-- Flag to indicate that the tests have been loaded */
} BSL_Test_Buffer;

/**
 * @brief Start executing the test cases
 *
 * @param [in] BSL_DalTF_handle   valid Dal handle
 * @param [in] DalTF_BSL_Test_Buf populated BSL buffer, containg the test list
 * @param [out] BSL_Result        test result buffer
 */

DALResult 
DalTF_BSL_Start_Tests(DalTFHandle* BSL_DalTF_handle, 
                      BSL_Test_Buffer* DalTFBSL_Test_Buf, BSL_Result_Buffer** BSL_Result);
/**
 * @brief Prepares the buffer to which to load the list of test cases.
 * This operation of this function is heavily manipulated by the T32 script
 *
 * Detailed Operation
 * Upon entering, this function, the T32 script will calculate a size for
 * @param [in] DalTF_BSL_TestBufferSize and update it out of context.
 * The script will then step over the malloc and check its return,
 *
 * If the malloc was successful, the script will load the list of tests at the address
 * of the malloc'ed block and update BSL_Test_Buffer->is_Full flag, 
 * and resume normal flow.
 */
DALResult
DalTF_BSL_Load_Tests(DalTFHandle* BSL_DalTF_handle,BSL_Test_Buffer* DalTF_BSL_Buf);

/**
 * @brief Initialize the test buffer and load the test files
 *
 * Most of this function's parameters are manipulated out of context by the T32
 * load script.
 *
 * Debugging in this function must be done carefully as locals must be intialized out
 * of context to avoid segmentation errors.
 * 
 * @param [in]   BSL_DalTF_handle    valid Dal handle.
 * @param [out]  DalTF_BSL_Test_Buf  BSL buffer containing populated test list
 */
DALResult
DalTF_BSL_Init(DalTFHandle* BSL_DalTF_handle,BSL_Test_Buffer** DalTF_BSL_Test_Buf);



#endif
