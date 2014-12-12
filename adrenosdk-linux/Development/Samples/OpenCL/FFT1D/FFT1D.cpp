//--------------------------------------------------------------------------------------
// File: FFT1D.cpp
// Desc: Implementation of 1D FFT on OpenCL (and CPU reference)
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include <OpenCL/FrmKernel.h>
#include "FFT1D.h"

//--------------------------------------------------------------------------------------
// Name: CFFT1D()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CFFT1D::CFFT1D(CFrmTimer& t)
: m_context(0),
  m_commandQueue(0),
  timer(t)
{
    for( INT32 i = 0; i < NUM_FFT_KERNELS; i++ )
    {
        m_kernels[i] = 0;
    }
}

//--------------------------------------------------------------------------------------
// Name: ~CFFT1D()
// Desc: Destructor
//--------------------------------------------------------------------------------------
CFFT1D::~CFFT1D()
{
    Destroy();
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize the OpenCL kernel resources
//--------------------------------------------------------------------------------------
BOOL CFFT1D::Initialize( cl_context context, cl_device_id device, cl_command_queue commandQueue )
{
    m_context = context;
    m_commandQueue = commandQueue;
    cl_program program;
    char options[256]= " ";
    cl_int errNum = 0;

    errNum = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(devType), &devType, NULL);
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to obtain device type" );
        return FALSE;
    }

    /*
     * The FFT1D kernel has been optimized for Adreno GPU, therefore
     * we only enable the optimized kernels when running on GPU.
     */
    if (devType == CL_DEVICE_TYPE_GPU)
    {
        strncat(options, " -cl-fast-relaxed-math ", sizeof(options));
        strncat(options, " -DQC_OPT ", sizeof(options));
    }

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/FFT1D.cl", &program, context,
                                                 &device, 1, options ) )
    {
        return FALSE;
    }
    const CHAR* kernelNames[NUM_FFT_KERNELS] = {0};

    if (devType == CL_DEVICE_TYPE_CPU)
    {
        kernelNames[0] = "FFT1DRadix2";
        kernelNames[1] = "IFFT1DRadix2";
        kernelNames[2] = "FFT1DRadix2";//dummy
    }
    else
    {
        kernelNames[0] = "FFT1DRadix2_P0";
        kernelNames[1] = "FFT1DRadix2_P0";//dummy
        kernelNames[2] = "FFT1DRadix4";
    }

    for( INT32 i = 0; i < NUM_FFT_KERNELS; i++ )
    {
        m_kernels[i] = clCreateKernel( program, kernelNames[ i ], &errNum );
        if ( errNum != CL_SUCCESS )
        {
            CHAR str[256];
            FrmSprintf( str, sizeof(str), "ERROR: Failed to create kernel '%s'.\n", kernelNames[i] );
            FrmLogMessage( str );
            return FALSE;
        }
    }

    clReleaseProgram( program );

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: Cleanup OpenCL resources
//--------------------------------------------------------------------------------------
VOID CFFT1D::Destroy()
{
    for( INT32 i = 0; i < NUM_FFT_KERNELS; i++ )
    {
        if( m_kernels[i] != 0 )
        {
            clReleaseKernel( m_kernels[i] );
            m_kernels[i] = 0;
        }
    }
}

//--------------------------------------------------------------------------------------
// Name: Forward()
// Desc: Perform forward FFT on pInArray storing the results in pOutArray.  If bRefCPU
// is TRUE, perform the FFT on the CPU reference implementation.  Otherwise, use OpenCL.
//--------------------------------------------------------------------------------------
BOOL CFFT1D::Forward( const FFTComplex* pInArray, INT32 nArraySize,
                      FFTComplex* pOutArray, BOOL bRefCPU )
{
    if ( bRefCPU )
    {
        ForwardRef( pInArray, nArraySize, pOutArray );
        return TRUE;
    }
    else
    {
        return ExecuteRadix2FFT( FFT_RADIX_2, pInArray, nArraySize, pOutArray );
    }
}

//--------------------------------------------------------------------------------------
// Name: Inverse()
// Desc: Perform inverse FFT on pInArray storing the results in pOutArray.  If bRefCPU
// is TRUE, perform the FFT on the CPU reference implementation.  Otherwise, use OpenCL.
//--------------------------------------------------------------------------------------
BOOL CFFT1D::Inverse( const FFTComplex* pInArray, INT32 nArraySize,
                      FFTComplex* pOutArray, BOOL bRefCPU )
{
    if ( bRefCPU )
    {
        InverseRef( pInArray, nArraySize, pOutArray );
        return TRUE;
    }
    else
    {
        return ExecuteRadix2FFT( IFFT_RADIX_2, pInArray, nArraySize, pOutArray );
    }
}

//--------------------------------------------------------------------------------------
// Name: ExecuteRadix2FFT()
// Desc: Execute forward or inverse Radix-2 FFT with OpenCL
//--------------------------------------------------------------------------------------
BOOL CFFT1D::ExecuteRadix2FFT( KernelType kernelType, const FFTComplex* pInArray,
                               INT32 nArraySize, FFTComplex* pOutArray )
{
    cl_int errNum = 0;
    cl_mem inArray = 0,
           outArray = 0;

    if( FALSE == AllocateMemObjects( nArraySize, pInArray,
                                     &inArray, &outArray ) )
    {
        return FALSE;
    }

    timer.Start();

    cl_mem memObjs[2] = { inArray, outArray };
    if (devType == CL_DEVICE_TYPE_CPU)
    {
        cl_kernel kernel = m_kernels[ (int) kernelType ];

        // IFFT kernel needs 1.0f / arraySize
        if ( kernelType == IFFT_RADIX_2 )
        {
            cl_float oneOverN = 1.0f / (cl_float) nArraySize;
            errNum = clSetKernelArg( kernel, 3, sizeof(cl_float), &oneOverN );
        }

        for( INT32 p = 1; p <= nArraySize / 2; p *= 2 )
        {
            size_t globalWorkSize[1] = { nArraySize / 2 };

            // Set the value of p for this iteration
            errNum |= clSetKernelArg( kernel, 0, sizeof(cl_int), &p );

            // Ping-pong input/output arrays
            errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &memObjs[0] );
            errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &memObjs[1] );

            if( errNum != CL_SUCCESS )
            {
                FrmLogMessage( "Error setting kernel arguments" );
                return FALSE;
            }

            errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                            globalWorkSize, NULL, 0, NULL, NULL );
            if( errNum != CL_SUCCESS )
            {
                char str[256];
                FrmSprintf( str, sizeof(str), "Error queueing Radix2 kernel for execution (%d).\n", errNum );
                FrmLogMessage( str );
                return FALSE;
            }

            // Ping pong for next iteration
            cl_mem temp = memObjs[1];
            memObjs[1] = memObjs[0];
            memObjs[0] = temp;
        }
    }
    else
    {
        cl_kernel kernel;
        INT32 p = 1;
        cl_float oneOverN = 1.f / nArraySize;
        int m = 0;
        float oneOverP;
        int sign;
        int nWidth = nArraySize;

        {
            int n = nWidth;
            while(n>>=1)
                m++;
        }
        /*
        * If this is a power of 2 but not 4 (i.e. odd power of 2),
        * run a one-pass (p==1) radix-2 FFT first.
        */
        if(m & 1)
        {
            kernel = m_kernels[ (int) FFT_RADIX_2 ];
            size_t gws[1] = { nWidth / 4};
            size_t lws[1] = { 256 };
            size_t* lwsp = lws;
            if(nWidth < 4 * lws[0])
                lwsp = NULL;


            errNum |= clSetKernelArg( kernel, 0, sizeof(cl_int), &nWidth );
            errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &memObjs[0] );
            errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &memObjs[1] );
            errNum |= clSetKernelArg( kernel, 3, sizeof(cl_int), &m );

            if( errNum != CL_SUCCESS )
            {
                FrmLogMessage( "Error setting kernel arguments" );
                return FALSE;
            }

            errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                            gws, lwsp, 0, NULL, NULL );
            if( errNum != CL_SUCCESS )
            {
                char str[256];
                FrmSprintf( str, sizeof(str), "Error queueing Radix2 kernel for execution (%d).\n", errNum );
                FrmLogMessage( str );
                return FALSE;
            }
            cl_mem temp = memObjs[1];
            memObjs[1] = memObjs[0];
            memObjs[0] = temp;

            p = 2;
        }
        else
        {
            p = 1;
        }

        kernel = m_kernels[ (int) FFT_RADIX_4 ];

        size_t globalWorkSize[1] = { nWidth / 4 };
        size_t localWorkSize[1] = { 128 };
        size_t* lws = localWorkSize;

        if(nWidth < 4 * 128)
            lws = NULL;

        /* The kernel id for inverse is still IFFT_RADIX_2 for legacy.
        * To use the same kernel for both forward and inverse FFT, we
        * need to parameterize the kernel.
        */

        // IFFT kernel needs 1.0f / arraySize
        if ( kernelType == IFFT_RADIX_2 )
        {
            sign = 1;
            oneOverN = 1.0f / (cl_float) nWidth;
        }
        else
        {
            sign = -1;
            oneOverN = 1.f;
        }

        // Set the width of a row
        errNum |= clSetKernelArg( kernel, 0, sizeof(cl_int), &nWidth );
        errNum |= clSetKernelArg( kernel, 4, sizeof(cl_float), &oneOverN );
        errNum |= clSetKernelArg( kernel, 5, sizeof(cl_int), &m );
        errNum |= clSetKernelArg( kernel, 6, sizeof(cl_int), &sign );

        /*
        * Perform log4(N)-1 passes of Radix-4.
        */

        for(; p <= nWidth / 4; p *= 4)
        {
            oneOverP = 1.f / (float)p;
            // Set the value of p for this iteration
            errNum |= clSetKernelArg( kernel, 1, sizeof(cl_int), &p );
            // Ping-pong input/output arrays
            errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &memObjs[0] );
            errNum |= clSetKernelArg( kernel, 3, sizeof(cl_mem), &memObjs[1] );
            errNum |= clSetKernelArg( kernel, 7, sizeof(cl_float), &oneOverP );
            if( errNum != CL_SUCCESS )
            {
                FrmLogMessage( "Error setting kernel arguments" );
                return FALSE;
            }

            errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                            globalWorkSize, lws, 0, NULL, NULL );
            if( errNum != CL_SUCCESS )
            {
                char str[256];
                FrmSprintf( str, sizeof(str), "Error queueing Radix2 kernel for execution (%d).\n", errNum );
                FrmLogMessage( str );
                return FALSE;
            }

            // Ping pong for next iteration
            cl_mem temp = memObjs[1];
            memObjs[1] = memObjs[0];
            memObjs[0] = temp;
        }
    }

    clFinish(m_commandQueue);
    timer.Stop();
    // The result will be left in memObjs[0] from the final ping-pong.

    // Read the results back to the CPU
    errNum = clEnqueueReadBuffer( m_commandQueue, memObjs[0], CL_TRUE, 0, sizeof(FFTComplex) * nArraySize,
        pOutArray, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error enqueuing buffer read." );
        return FALSE;
    }

    clReleaseMemObject( inArray );
    clReleaseMemObject( outArray );
    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: AllocateMemObjects()
// Allocate OpenCL memory objects for input and output
//--------------------------------------------------------------------------------------
BOOL CFFT1D::AllocateMemObjects( INT32 nArraySize, const FFTComplex* pInArray,
                                 cl_mem* pObjIn, cl_mem* pObjOut )
{
    cl_int errNum = 0;
    *pObjIn = clCreateBuffer( m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, nArraySize * sizeof(FFTComplex),
                              (void*)pInArray, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device input array.\n" );
        return FALSE;
    }

    *pObjOut = clCreateBuffer( m_context, CL_MEM_READ_WRITE, nArraySize * sizeof(FFTComplex), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        clReleaseMemObject( *pObjIn );
        *pObjIn = 0;
        FrmLogMessage( "ERROR: allocation of device output array.\n" );
        return FALSE;
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: ForwardRef()
// Desc: Perform forward DFT on pInArray storing the results in pOutArray on the CPU.
//--------------------------------------------------------------------------------------
VOID CFFT1D::ForwardRef( const FFTComplex* pInArray, INT32 nArraySize,
                         FFTComplex* pOutArray )
{

    // Compute the forward DFT for reference
    for( INT32 i = 0; i < nArraySize; i++ )
    {
        pOutArray[i].fReal = 0.0f;
        pOutArray[i].fImaginary = 0.0f;

        FLOAT32 fArg = -1.0f * 2.0f * FRM_PI * (FLOAT32)i / (FLOAT32)nArraySize;
        for( INT32 j = 0; j < nArraySize; j++ )
        {
            FLOAT32 fCos = FrmCos( (FLOAT32)j * fArg );
            FLOAT32 fSin = FrmSin( (FLOAT32)j * fArg );

            pOutArray[i].fReal      += pInArray[j].fReal * fCos - pInArray[j].fImaginary * fSin;
            pOutArray[i].fImaginary += pInArray[j].fReal * fSin + pInArray[j].fImaginary * fCos;
        }
    }
}

//--------------------------------------------------------------------------------------
// Name: InverseRef()
// Desc: Perform inverse DFT on pInArray storing the results in pOutArray on the CPU.
//--------------------------------------------------------------------------------------
VOID CFFT1D::InverseRef( const FFTComplex* pInArray, INT32 nArraySize,
                         FFTComplex* pOutArray )
{

    // Compute the inverse DFT for reference
    for( INT32 i = 0; i < nArraySize; i++ )
    {
        pOutArray[i].fReal = 0.0f;
        pOutArray[i].fImaginary = 0.0f;

        FLOAT32 fArg = 2.0f * FRM_PI * (FLOAT32)i / (FLOAT32)nArraySize;
        for( INT32 j = 0; j < nArraySize; j++ )
        {
            FLOAT32 fCos = FrmCos( (FLOAT32)j * fArg );
            FLOAT32 fSin = FrmSin( (FLOAT32)j * fArg );

            pOutArray[i].fReal      += pInArray[j].fReal * fCos - pInArray[j].fImaginary * fSin;
            pOutArray[i].fImaginary += pInArray[j].fReal * fSin + pInArray[j].fImaginary * fCos;
        }

        pOutArray[i].fReal /= (FLOAT32) nArraySize;
        pOutArray[i].fImaginary /= (FLOAT32) nArraySize;
    }
}
