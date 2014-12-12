//=============================================================================
// FILE: hello_cl.cpp
//
// Android native specific implementation
//
// OS specific implementation need to perform the following tasks:
// 1) Take care of the application entry point
// 2) Create the application container
// 3) Handle system message (keypresses, program interuption)
// 4) Run the application message loop
//
// Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.
//=============================================================================

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <android/log.h>
#include "hello_cl.h"
#include "log_def.h"

#define DATA_SIZE (1024)
#define app_name "hello_cl"

const char *kernelSource =                                           "\n" \
"__kernel void SimpleIndexing (                                       \n" \
"   __global int *input,                                              \n" \
"   __global int *output                                              \n" \
"   )                                                                 \n" \
"{                                                                    \n" \
"   uint id = get_local_id(0) + get_group_id(0)*get_local_size(0);      \n" \
"   output[id] = input[id];                                            \n" \
"}                                                                    \n" \
"\n";

void helloCL::initHost()
{
    /** Init host data */
    hostData = (unsigned int*)malloc(sizeof(unsigned int)*DATA_SIZE);

    for (int i = 0; i < DATA_SIZE; i++)
    {
        hostData[i] = i;
    }
}

void helloCL::initCLPlatform(unsigned int selected_device)
{
    /** Pre-fault */
    cl_int err = CL_FALSE;

    /** Get the platform */
    err = clGetPlatformIDs(1, &platform, &numPlatforms);
    if(CL_SUCCESS != err)
    {
        LOGE("clGetPlatformIDs failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clGetPlatformIDs succeed: platformID: %x found", (unsigned int)platform);
    }

    /** Check for Qualcomm platform*/
    char platformInfo[1024];
    err = clGetPlatformInfo(platform,CL_PLATFORM_VENDOR, 1024, platformInfo, 0);
    if (CL_SUCCESS != err)
    {
        LOGE("clGetPlatformInfo failed: error: %d\n", err);
        exit(err);
    }

    if(strcmp("QUALCOMM", platformInfo))
    {
        LOGE("Platform: QUALCOMM not found, error %d\n", err);
        exit(0);
    }
    else
    {
        LOGI("Platform: QUALCOMM found\n");
    }

    /** Get the device*/
    err = clGetDeviceIDs(platform, selected_device, 1, &device, &numDevices);
    if(CL_SUCCESS != err)
    {
        LOGE("clGetDeviceIDs failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clGetDeviceIDs succeed: deviceID: %x found", (unsigned int)device);
    }


    LOGI("Selected device is: ");
    switch (selected_device)
    {
    case CL_DEVICE_TYPE_CPU:
        LOGI("CPU\n");
        break;
    case CL_DEVICE_TYPE_GPU:
        LOGI("GPU\n");
        break;
    case CL_DEVICE_TYPE_DEFAULT:
        LOGI("DEFAULT\n");
        break;
    }

    char deviceInfo[1024];
    err = clGetDeviceInfo(device, CL_DEVICE_NAME, 1024, deviceInfo, 0);
    if(CL_SUCCESS != err)
    {
        LOGE("clGetDeviceInfo failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("Device: %s found", deviceInfo);
    }

    /** Create the context */
    ctx = clCreateContext(0, 1, &device, NULL, NULL, &err);
    if(CL_SUCCESS != err)
    {
        LOGE("clCreateContext failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clCreateContext succeed: contextID: %x", (unsigned int)ctx);
    }

    cmdq = clCreateCommandQueue(ctx, device, 0, &err);
    if(CL_SUCCESS != err)
    {
        LOGE("clCreateCommandQueue failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clCreateCommandQueue succeed: commandqueueID: %x", (unsigned int)cmdq);
    }
}

void helloCL::initCLBuffer()
{
    cl_int err = CL_FALSE;

    /** Input buffer */
    buff[0] = clCreateBuffer(ctx, CL_MEM_READ_ONLY, sizeof(int)*DATA_SIZE, NULL, &err);
    if(CL_SUCCESS != err)
    {
        LOGE("clCreateBuffer [0] failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clCreateBuffer [0] succeed: bufferID: %x", (unsigned int)buff[0]);
    }

    /** Output buffer */
    buff[1] = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, sizeof(int)*DATA_SIZE, NULL, &err);
    if(CL_SUCCESS != err)
    {
        LOGE("clCreateBuffer [1] failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clCreateBuffer [1] succeed: bufferID: %x", (unsigned int)buff[1]);
    }
}

void helloCL::initCLKernel()
{
    cl_int err = CL_FALSE;

    /** Load kernel source and build the program */
    program = clCreateProgramWithSource(ctx, 1, (const char**)&kernelSource, NULL, &err);
    if(CL_SUCCESS != err)
    {
        LOGE("clCreateProgramWithSource failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clCreateProgramWithSource succeed: programID: %x", (unsigned int)program);
    }

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if(CL_SUCCESS != err)
    {
        LOGE("clBuildProgram failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clBuildProgram succeed");
    }

    /** Create the kernel */
    kernel = clCreateKernel(program, "SimpleIndexing", &err);
    if(CL_SUCCESS != err)
    {
        LOGE("clCreateKernel failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clCreateKernel succeed: kernelID: %x", (unsigned int)kernel);
    }
}

void helloCL::runCL()
{
    cl_int err = CL_FALSE;
    cl_event event;
    /** Global and local work size*/
    size_t gws[3] = {DATA_SIZE, 0, 0};
    size_t lws[3] = {4, 0, 0};

    /** Set the kernel arguments */
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buff[0]);
    err &= clSetKernelArg(kernel, 1, sizeof(cl_mem), &buff[1]);
    if(CL_SUCCESS != err)
    {
        LOGE("clSetKernelArg failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clSetKernelArg succeed");
    }

    /** Enqueue the buffer write command and kernel launch to the command queue */
    err = clEnqueueWriteBuffer(cmdq, buff[0], CL_FALSE, 0, sizeof(int)*DATA_SIZE, hostData, 0, NULL, &event);
    if(CL_SUCCESS != err)
    {
        LOGE("clEnqueueWriteBuffer failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clEnqueueWriteBuffer succeed");
    }

    err = clEnqueueNDRangeKernel(cmdq, kernel, 1, 0, gws, lws, 1, &event, NULL);
    if(CL_SUCCESS != err)
    {
        LOGE("clEnqueueNDRangeKernel failed: error: %d\n", err);
        exit(err);
    }
    else
    {
        LOGI("clEnqueueNDRangeKernel succeed");
    }

    /** Block until finish */
    clFlush(cmdq);
    clFinish(cmdq);
    clReleaseEvent(event);
}

void helloCL::verify()
{
    cl_int err = CL_FALSE;
    unsigned int* hostOutput = (unsigned int*)malloc(sizeof(int)*DATA_SIZE);
    err = clEnqueueReadBuffer(cmdq, buff[1], CL_TRUE, 0, sizeof(int)*DATA_SIZE, hostOutput, 0, NULL, 0);

    int countDiff = 0;
    for(int i = 0; i < DATA_SIZE; i++)
    {
        if(hostData[i] != hostOutput[i])
        {
            countDiff++;
        }
    }

    if(countDiff != 0)
        LOGI("Output verification failed: num of mismatch: %d", countDiff);
    else
        LOGI("Output verification succeed");

    free(hostOutput);
}

void helloCL::cleanupCL()
{
    free(hostData);

    clReleaseMemObject(buff[1]);
    clReleaseMemObject(buff[0]);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdq);
    clReleaseContext(ctx);
}


int hello_cl(void) {
    helloCL CLApp;
    int selected_device = CL_DEVICE_TYPE_DEFAULT;

#if defined(USE_CPU)
    selected_device = CL_DEVICE_TYPE_CPU;
#elif defined(USE_GPU)
    selected_device = CL_DEVICE_TYPE_GPU;
#endif

#if defined(USE_GPU) && defined(USE_CPU)
#error "Both USE_CPU and USE_GPU defined - unsupported"
#endif

    CLApp.initHost();
    CLApp.initCLPlatform(selected_device);
    CLApp.initCLBuffer();
    CLApp.initCLKernel();
    CLApp.runCL();
    CLApp.verify();
    CLApp.cleanupCL();
    LOGI("Hello CL finished successfully");
    return 0;
}
