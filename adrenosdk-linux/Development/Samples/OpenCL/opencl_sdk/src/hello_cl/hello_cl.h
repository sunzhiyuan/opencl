//=============================================================================
// FILE: helloCL.h
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

#include <CL/opencl.h>

class helloCL
{
public:
    cl_device_id        device;
    cl_uint                numDevices;
    cl_platform_id        platform;
    cl_uint               numPlatforms;
    cl_context            ctx;
    cl_command_queue      cmdq;
    cl_program              program;
    cl_kernel              kernel;

    cl_event            evt;
    cl_mem                buff[2];

    unsigned int*        hostData;

public:
    /** Setup host (CPU) data*/
    void initHost();
    /** Setup platform and device*/
    void initCLPlatform( unsigned int selected_device=CL_DEVICE_TYPE_DEFAULT);
    /** Setup CL buffers*/
    void initCLBuffer();
    /** Build CL program and create kernel*/
    void initCLKernel();
    /** Launch CL kernels*/
    void runCL();
    /** Cleanup the resources*/
    void cleanupCL();
    /** Verify correctness*/
    void verify();
};

extern "C" {int hello_cl(void);}
