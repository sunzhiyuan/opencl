/*======================================================================
*  FILE:
*      cltransform.h
*  DESCRIPTION:
*      Macros, structures and API functions for the core part of wavelet
*      transform application
*
*            Copyright (c) 2012 QUALCOMM Incorporated.
*                      All Rights Reserved.
*              QUALCOMM Confidential and Proprietary
*
*======================================================================*/

/**
* \addtogroup wavelettransform
* \{
* \file cltransform.h
* \brief Macros, structures and API functions for the core part of wavelet
*        transform application
*/

#ifndef cltransform_H
#define cltransform_H

#include <CL/cl.h>

#include "common.h"

#define NUM_OF_PROGRAMS 6 // one kernel is used 3 times with different compilation options.

#define QCOM_SCHEDULER

//#define OUTPUT_BINARY_KERNEL

typedef struct OPENCL_DEVICE_PLATFORM_INFO
{
    bool             isQCOMplatform;
    cl_context       clContext;            ///< OpenCL context
    cl_command_queue commandQ;
    cl_platform_id   clPlatformId;         ///< OpenCL platform
    cl_platform_info clPlatformInfo;
    cl_device_id     clDevice;             ///< OpenCL device
}strOpenCLInfo;

//////////////////////////  Kernel information //////////////////////////

typedef struct KERNEL_COMPILATION_INFO
{
    cl_kernel       kernel;
    size_t          WGsize;
    cl_ulong        localMemory;
    size_t          compiler_WGsize[3];
}strKernelCompilationInfo;

typedef struct KERNEL_DESC
{
    char        programFileName[100];
    char        kernelName[100];
    short       kernelIdx;
    bool        isSchedulerEnabled;
    bool        isUseSource;
    char        programBuildOptions[100];
    strKernelCompilationInfo compilerInfo;
}strKernelInfo;

typedef enum  ENUM_KERNEL
{
    enum_fDWT_X,
    enum_fDWT_Y,
    enum_iDWT_X,
    enum_iDWT_Y,
    enum_fHaar,
    enum_iHaar
}enumKernels;

//////////////////////////  OpenCL execution variables //////////////////////////

// CL memory objects.
typedef struct OPENCL_MEM_OBJECTS
{
    cl_mem cl_input[MAX_COLOR];
    cl_mem cl_haaroutput_DC[MAX_COLOR];
    cl_mem cl_haaroutput_AC[MAX_COLOR];
    cl_mem dwt_img_DC[MAX_COLOR][MAX_LEVEL-1];
    cl_mem dwt_img_AC[MAX_COLOR][MAX_LEVEL-1];
    cl_mem tempImgMem4DWT[MAX_COLOR];
}strOpenCLMem;

// CL event objects.
typedef struct OPENCL_EVENT_OBJECTS
{
    cl_event event_fDWT[MAX_COLOR][MAX_LEVEL-1];

    cl_event event_haar_forward[MAX_COLOR];
    cl_event event_haar_inverse[MAX_COLOR];

    cl_event depedent_event[MAX_COLOR];
    cl_event EventList[70];
    int EventCounter;
}StrOpenCLEvents;

void init_test(short mainWavelet_level,
               unsigned short imgWidth,
               unsigned short imgHeight,
               bool isUsingOpenCL_GPU);

void libTransform( unsigned char *imgbuf, float *timer);

void cleanupOpenCL();

#endif
/** \} */
