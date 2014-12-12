//--------------------------------------------------------------------------------------
// File: FrmKernel.cpp
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef _FRM_KERNEL_H_
#define _FRM_KERNEL_H_

#include "FrmPlatform.h"
#include <CL/cl.h>

BOOL FrmBuildComputeProgramFromFile( const CHAR* strKernelFileName,
                                     cl_program *pProgram,
                                     cl_context context,
                                     cl_device_id* pDevices,
                                     cl_uint deviceCount,
                                     const CHAR* options = NULL );
BOOL FrmPrimeGPU( cl_command_queue commandQueue, cl_kernel kernel,
                  cl_uint workDim, const size_t *globalWorkSize,
                  const size_t *localWorkSize);

#endif // _FRM_KERNEL_H_
