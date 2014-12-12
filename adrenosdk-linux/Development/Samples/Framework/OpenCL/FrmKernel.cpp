//--------------------------------------------------------------------------------------
// File: FrmShader.cpp
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "FrmPlatform.h"
#include "FrmUtils.h"
#include "FrmKernel.h"

// Macro to set the duration of priming the GPU in milliseconds
#define PRIME_TIME_DURATION 250

//--------------------------------------------------------------------------------------
// Name: FrmBuildComputeProgramFromFile()
// Desc: Compile an OpenCL C file and create a program
//--------------------------------------------------------------------------------------
BOOL FrmBuildComputeProgramFromFile( const CHAR* strKernelFileName,
                                     cl_program *pProgram,
                                     cl_context context,
                                     cl_device_id* pDevices,
                                     cl_uint deviceCount,
                                     const CHAR* options )
{
    CHAR logMsg[1024];
    CHAR* strKernelSource;
    if( FALSE == FrmLoadFile( strKernelFileName, (VOID**)&strKernelSource) )
    {
        FrmSprintf( logMsg, sizeof(logMsg), "ERROR: Could not load kernel file '%s'\n", strKernelFileName );
        FrmLogMessage( logMsg );
        return FALSE;
    }

    cl_program program;
    cl_int errNum;
    size_t strLength = strlen(strKernelSource);
    program = clCreateProgramWithSource( context, 1, (const char**)&strKernelSource,
                                         &strLength, &errNum );
    if( errNum != CL_SUCCESS || program == 0 )
    {
        FrmSprintf( logMsg, sizeof(logMsg), "ERROR: failed to create program with source from file '%s'\n", strKernelFileName );
        FrmLogMessage( logMsg );
        delete [] strKernelSource;
        return FALSE;
    }

    errNum = clBuildProgram( program, deviceCount, pDevices, options, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmSprintf( logMsg, sizeof(logMsg), "ERROR: faild to build program with source from file '%s'\n", strKernelFileName );
        FrmLogMessage( logMsg );
    }

    for (cl_uint i = 0; i < deviceCount; i++)
    {
        cl_build_status buildStatus;
        errNum = clGetProgramBuildInfo( program, pDevices[i], CL_PROGRAM_BUILD_STATUS, sizeof(buildStatus), &buildStatus, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "ERROR: Failed to get CL_PROGRAM_BUILD_STATUS\n" );
            delete [] strKernelSource;
            clReleaseProgram( program );
            return FALSE;
        }

        if (buildStatus != CL_BUILD_SUCCESS )
        {
            char *buildLog;
            size_t buildLogSize = 0;
            
            clGetProgramBuildInfo( program, pDevices[i], CL_PROGRAM_BUILD_LOG, 0, NULL, &buildLogSize );
            buildLog = new char[ buildLogSize ];

            clGetProgramBuildInfo( program, pDevices[i], CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, NULL);
            FrmSprintf( logMsg, sizeof(logMsg), "ERROR: Building program '%s' (build_status: %d) \n:", strKernelFileName, buildStatus );
            FrmLogMessage( logMsg );
            FrmLogMessage( buildLog );    
            delete [] buildLog;
            delete [] strKernelSource;         
            clReleaseProgram( program );
            return FALSE;     
        }
    }

    delete [] strKernelSource;
    *pProgram = program;
    return TRUE;
}

BOOL FrmPrimeGPU( cl_command_queue commandQueue, cl_kernel kernel,
                  cl_uint workDim, const size_t *globalWorkSize,
                  const size_t *localWorkSize)
{
    cl_int errNum;
    double elapsed_time = 0;

    while (elapsed_time < PRIME_TIME_DURATION) {
        CFrmTimer m_Timer;
        m_Timer.Reset();
        m_Timer.Start();

        errNum = clEnqueueNDRangeKernel( commandQueue, kernel, workDim, NULL,
                                    globalWorkSize, localWorkSize, 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error queueing kernel for execution." );
            return FALSE;
        }
        clFinish(commandQueue);

        m_Timer.Stop();
        //Convert seconds into milliseconds
        elapsed_time += m_Timer.GetTime() * 1000;
    }
    return TRUE;
}

