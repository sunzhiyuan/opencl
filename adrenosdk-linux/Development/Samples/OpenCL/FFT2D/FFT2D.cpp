//--------------------------------------------------------------------------------------
// File: FFT2D.cpp
// Desc: Implementation of 2D FFT on OpenCL (and CPU reference)
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
#include "FFT2D.h"


//--------------------------------------------------------------------------------------
// Name: CFFT2D()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CFFT2D::CFFT2D(CFrmTimer& t)
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
// Name: ~CFFT2D()
// Desc: Destructor
//--------------------------------------------------------------------------------------
CFFT2D::~CFFT2D()
{
    Destroy();
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize the OpenCL kernel resources
//--------------------------------------------------------------------------------------
BOOL CFFT2D::Initialize( cl_context context, cl_device_id device, cl_command_queue commandQueue )
{
    m_context = context;
    m_commandQueue = commandQueue;
    cl_program program;
    cl_int errNum = 0;
    char options[256]=" ";
    deviceID = device;

    errNum = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(devType), &devType, NULL);
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to obtain device type" );
        return FALSE;
    }

    /*
     * The FFT2D kernel has been optimized for Adreno GPU, therefore
     * we only enable the optimized kernels when running on GPU.
     */
    if (devType == CL_DEVICE_TYPE_GPU)
    {
        strncat(options, " -cl-fast-relaxed-math ", sizeof(options));
        strncat(options, " -DQC_OPT ", sizeof(options));
    }

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/FFT2D.cl", &program, context,
                                                 &device, 1, options ) )
    {
        return FALSE;
    }

    const CHAR* kernelNames[NUM_FFT_KERNELS] = {0};

    if (devType == CL_DEVICE_TYPE_CPU)
    {
        kernelNames[FFT_RADIX_2]  = "FFT2DRadix2";
        kernelNames[IFFT_RADIX_2] = "IFFT2DRadix2";
        kernelNames[FFT_RADIX_4]  = "FFT2DRadix2"; //Dummy
    }
    else
    {
        kernelNames[FFT_RADIX_2]  = "FFT2DRadix2_P0";
        kernelNames[IFFT_RADIX_2] = "FFT2DRadix2_P0";//This is to keep the loop below unchanged (even though the same kernel code is loaded)
        kernelNames[FFT_RADIX_4]  = "FFT2DRadix4";
    }

    kernelNames[MATRIX_TRANSPOSE] = "MatrixTranspose";

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
VOID CFFT2D::Destroy()
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
BOOL CFFT2D::Forward( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                      FFTComplex* pOutArray, BOOL bRefCPU )
{
    if ( bRefCPU )
    {
        ForwardRef( pInArray, nWidth, nHeight, pOutArray );
        return TRUE;
    }
    else
    {
        return ExecuteRadix2FFT( FFT_RADIX_2, pInArray, nWidth, nHeight, pOutArray );
    }
}

//--------------------------------------------------------------------------------------
// Name: Inverse()
// Desc: Perform inverse FFT on pInArray storing the results in pOutArray.  If bRefCPU
// is TRUE, perform the FFT on the CPU reference implementation.  Otherwise, use OpenCL.
//--------------------------------------------------------------------------------------
BOOL CFFT2D::Inverse( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                      FFTComplex* pOutArray, BOOL bRefCPU )
{
    if ( bRefCPU )
    {
        InverseRef( pInArray, nWidth, nHeight, pOutArray );
        return TRUE;
    }
    else
    {
        return ExecuteRadix2FFT( IFFT_RADIX_2, pInArray, nWidth, nHeight, pOutArray );
    }
}

//--------------------------------------------------------------------------------------
// Name: ExecuteRadix2FFT()
// Desc: Execute forward or inverse 2D Radix-2 FFT with OpenCL
//--------------------------------------------------------------------------------------
BOOL CFFT2D::ExecuteRadix2FFT( KernelType kernelType, const FFTComplex* pInArray,
                               INT32 nWidth, INT32 nHeight, FFTComplex* pOutArray )
{
    cl_kernel kernel = m_kernels[ (int) kernelType ];
    cl_int errNum = 0;
    cl_mem inArray = 0, outArray = 0, scratchArray = 0;

    // Allocate OpenCL memory objects for input array, output array, and scratch space
    // needed for transpose
    if( FALSE == AllocateMemObjects( nWidth, nHeight, pInArray,
                                     &inArray, &outArray, &scratchArray ) )
    {
        return FALSE;
    }
    timer.Start();

    // Execute 1D FFT (or IFFT) on each row of the 2D input array
    BOOL result;
    cl_mem memObjs[2] = { inArray, outArray };
    result = ExecuteRowWiseRadix2FFT( kernelType, memObjs, nWidth, nHeight );
    if( FALSE == result )
        return FALSE;

    // Transpose the result
    result = ExecuteTranspose( nWidth, nHeight, memObjs[0], scratchArray );
    if( FALSE == result )
        return FALSE;

    // Execute 1D FFT (or IFFT) on each col of the 2D input array
    memObjs[0] = scratchArray;
    memObjs[1] = outArray;

    result = ExecuteRowWiseRadix2FFT( kernelType, memObjs, nHeight, nWidth );
    if( FALSE == result )
        return FALSE;
    // Transpose the final result
    ExecuteTranspose( nHeight, nWidth, memObjs[0], memObjs[1] );
    if( FALSE == result )
        return FALSE;

    //This is here to make measuring kernel work possible.
    clFinish(m_commandQueue);

    timer.Stop();
    // Read the results back to the CPU - the result is left in inArray by the last Transpose()
    errNum = clEnqueueReadBuffer( m_commandQueue, memObjs[1], CL_TRUE, 0, sizeof(FFTComplex) * nWidth * nHeight,
        pOutArray, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error enqueuing buffer read." );
        return FALSE;
    }

    clReleaseMemObject( inArray );
    clReleaseMemObject( outArray );
    clReleaseMemObject( scratchArray );
    return TRUE;
}

BOOL CFFT2D::ExecuteRowWiseRadix2FFT( KernelType kernelType, cl_mem memObjs[2],
                                      INT32 nWidth, INT32 nHeight )
{
	BOOL ret = false;
    if (devType == CL_DEVICE_TYPE_GPU)
    {
        ret = ExecuteRowWiseRadix2FFTOpt(kernelType, memObjs, nWidth, nHeight);
    }
    else
    {
        ret = ExecuteRowWiseRadix2FFTOrig(kernelType, memObjs, nWidth, nHeight);
    }
	return ret;
}
//--------------------------------------------------------------------------------------
// Name: ExecuteRadix2FFT()
// Desc: Execute forwad or inverse 1D FFT on each row of input using OpenCL
//--------------------------------------------------------------------------------------
BOOL CFFT2D::ExecuteRowWiseRadix2FFTOrig( KernelType kernelType, cl_mem memObjs[2],
                                      INT32 nWidth, INT32 nHeight )
{
    cl_kernel kernel = m_kernels[ (int) kernelType ];
    cl_int errNum = 0;

    // IFFT kernel needs 1.0f / arraySize
    if ( kernelType == IFFT_RADIX_2 )
    {
        cl_float oneOverN = 1.0f / (cl_float) nWidth;
        errNum = clSetKernelArg( kernel, 4, sizeof(cl_float), &oneOverN );
    }

    // Set the width of a row
    errNum |= clSetKernelArg( kernel, 0, sizeof(cl_int), &nWidth );
    for( INT32 p = 1; p <= nWidth / 2; p *= 2 )
    {
        // Compute over a 2D domain.
        //  [0] = halfArraySize (each kernel invocation computes 2 FFT values)
        //  [1] = height (execute the FFT for each row
        size_t globalWorkSize[2] = { nWidth / 2,  nHeight };

        // Set the value of p for this iteration
        errNum |= clSetKernelArg( kernel, 1, sizeof(cl_int), &p );

        // Ping-pong input/output arrays
        errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &memObjs[0] );
        errNum |= clSetKernelArg( kernel, 3, sizeof(cl_mem), &memObjs[1] );

        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error setting kernel arguments" );
            return FALSE;
        }

        errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 2, NULL,
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

    // The result is left in memObjs[0]
    return TRUE;
}
//--------------------------------------------------------------------------------------
// Name: ExecuteRadix2FFT()
// Desc: Execute forwad or inverse 1D FFT on each row of input using OpenCL
//--------------------------------------------------------------------------------------
BOOL CFFT2D::ExecuteRowWiseRadix2FFTOpt( KernelType kernelType, cl_mem memObjs[2],
                                      INT32 nWidth, INT32 nHeight )
{
    cl_kernel kernel;
    cl_int errNum = 0;
    INT32 p = 1;
    cl_float oneOverN = 1.0f / nWidth;
    int m = 0;
    int sign;
    float oneOverP;

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

        size_t gws[2] = { nWidth / 4,  nHeight };

        //The workgroup size has a HUGE effect on performance. This needs to be
        //tuned for a particular GPU.
        size_t lws[2] = { 128, 1 };
        size_t* lwsp = lws;

        if(nWidth < 4 * lws[0])
            lwsp = NULL;

        errNum |= clSetKernelArg( kernel, 0, sizeof(cl_int), &nWidth );
        // Ping-pong input/output arrays
        errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &memObjs[0] );
        errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &memObjs[1] );
        errNum |= clSetKernelArg( kernel, 3, sizeof(cl_int), &m );

        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error setting kernel arguments" );
            return FALSE;
        }

        errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 2, NULL,
                                        gws, lwsp, 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            char str[256];
            FrmSprintf( str, sizeof(str), "Error queueing Radix2 kernel pass for execution (%d).\n", errNum );
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
        p  = 1;
    }

    kernel = m_kernels[ (int) FFT_RADIX_4 ];

    size_t globalWorkSize[2] = { nWidth / 4,  nHeight };
    size_t localWorkSize[2] = { 128, 1 };
    size_t* lws = localWorkSize;

    if(nWidth < 4 * localWorkSize[0])
        lws = NULL;

    /* The kernel id for inverse is still IFFT_RADIX_2 for legacy.
    * To use the same kernel for both forward and inverse FFT, we
    * need to parameterize the kernel.
    */
    // IFFT kernel needs 1.0f / arraySize
    if ( kernelType == IFFT_RADIX_2 )
    {
        sign = 1;
        oneOverN = 1.0f / nWidth;
    }
    else
    {
        sign = -1;
        oneOverN = 1.0f;
    }

    // Set the width of a row
    errNum |= clSetKernelArg( kernel, 0, sizeof(cl_int), &nWidth );
    errNum |= clSetKernelArg( kernel, 4, sizeof(cl_float), &oneOverN );
    errNum |= clSetKernelArg( kernel, 5, sizeof(cl_int), &m );
    errNum |= clSetKernelArg( kernel, 6, sizeof(cl_int), &sign );

    for(; p <= nWidth / 4; p *= 4 )
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

    errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 2, NULL,
                                    globalWorkSize, lws, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        char str[256];
        FrmSprintf( str, sizeof(str), "Error queueing Radix4 kernel for execution (%d).\n", errNum );
        FrmLogMessage( str );
        return FALSE;
    }

    // Ping pong for next iteration
    cl_mem temp = memObjs[1];
    memObjs[1] = memObjs[0];
    memObjs[0] = temp;

    }
    // The result is left in memObjs[0]
    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: ExecuteTranspose()
// Desc: Transpose inputMat and place results in outputMat using OpenCL
//--------------------------------------------------------------------------------------
BOOL CFFT2D::ExecuteTranspose( INT32 nCols, INT32 nRows, cl_mem inputMat, cl_mem outputMat )
{
    char str[256];

    cl_kernel kernel = m_kernels[ MATRIX_TRANSPOSE ];

    // Set the kernel arguments
    cl_int errNum = 0;
    errNum |= clSetKernelArg( kernel, 0, sizeof(int), &nRows);
    errNum |= clSetKernelArg( kernel, 1, sizeof(int), &nCols );
    errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &inputMat );
    errNum |= clSetKernelArg( kernel, 3, sizeof(cl_mem), &outputMat );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments for MatrixTranspose kernel.\n" );
        return FALSE;
    }

    if (devType == CL_DEVICE_TYPE_GPU)
    {
        // Determine the localWorkSize based on the maximum allowed by the device for this kernel
        size_t maxWorkGroupSize = 0;
        errNum = clGetKernelWorkGroupInfo( kernel, deviceID, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL );
        if (errNum != CL_SUCCESS )
        {
            FrmLogMessage( "ERROR: clGetKernelInfo on CL_KERNEL_WORK_GROUP_SIZE\n");
            return FALSE;
        }
        size_t globalWorkSize[2] = { nCols / 2, nRows / 2};
        //The workgroup size has a HUGE effect on performance. This needs to be
        //tuned for a particular GPU.
        size_t localWorkSize[2] = {4, 64};
        // try to match 4x64
        if (localWorkSize[0] * localWorkSize[1] > maxWorkGroupSize)
        {
            localWorkSize[1] = maxWorkGroupSize / localWorkSize[0];
        }
        // Queue the kernel for execution
        errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 2, NULL,
                                     globalWorkSize, localWorkSize, 0, NULL, NULL );

    }
    else
    {
        size_t globalWorkSize[1] = { nRows };
        // Queue the kernel for execution
        errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                     globalWorkSize, NULL, 0, NULL, NULL );

    }


    if( errNum != CL_SUCCESS )
    {
        FrmSprintf( str, sizeof(str), "Error queueing transpose kernel for execution. %d\n", errNum );
        FrmLogMessage( str );
        return FALSE;
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: AllocateMemObjects()
// Allocate OpenCL memory objects for input and output
//--------------------------------------------------------------------------------------
BOOL CFFT2D::AllocateMemObjects( INT32 nWidth, INT32 nHeight, const FFTComplex* pInArray,
                                 cl_mem* pObjIn, cl_mem* pObjOut, cl_mem *pObjScratch )
{
    cl_int nArraySize = nWidth * nHeight;
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

    *pObjScratch = clCreateBuffer( m_context, CL_MEM_READ_WRITE, nArraySize * sizeof(FFTComplex), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        clReleaseMemObject( *pObjIn );
        clReleaseMemObject( *pObjOut );
        *pObjIn = 0;
        *pObjOut = 0;
        FrmLogMessage( "ERROR: allocation of device scratch array.\n" );
        return FALSE;
    }


    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: ForwardRef()
// Desc: Perform forward FFT on pInArray storing the results in pOutArray on the CPU.
//--------------------------------------------------------------------------------------
VOID CFFT2D::ForwardRef( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                         FFTComplex* pOutArray )
{
    ForwardOrInverseRef( pInArray, nWidth, nHeight, pOutArray, FORWARD );
}

//--------------------------------------------------------------------------------------
// Name: InverseRef()
// Desc: Perform inverse FFT on pInArray storing the results in pOutArray on the CPU.
//--------------------------------------------------------------------------------------
VOID CFFT2D::InverseRef( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                         FFTComplex* pOutArray )
{
    ForwardOrInverseRef( pInArray, nWidth, nHeight, pOutArray, INVERSE );
}

//--------------------------------------------------------------------------------------
// Name: ForwardOrInverseRef()
// Desc: Perform Forward or Inverse FFT (Reference CPU implementation)
//--------------------------------------------------------------------------------------
VOID CFFT2D::ForwardOrInverseRef( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                                     FFTComplex* pOutArray, Direction eDirection )
{
    // Allocate scratch space for the rows
    FFTComplex* pScratch = new FFTComplex[ nWidth ];

    // First compute the FFT on all the rows
    for( INT32 y = 0; y < nHeight; y++ )
    {
        const FFTComplex* pInRowData = &pInArray[y * nWidth];
        FFTComplex* pOutRowData = &pOutArray[y * nWidth];

        if( eDirection == FORWARD )
            FFTRef( nWidth, 0, 1, pInRowData, pOutRowData, pScratch );
        else
            IFFTRef( nWidth, pInRowData, pOutRowData );
    }
    delete [] pScratch;

    // Allocate scratch space for the cols
    pScratch = new FFTComplex[ nHeight ];

    // Allocate temporary storage for the transposed results
    FFTComplex* pTransposedOutput = new FFTComplex[ nWidth * nHeight ];

    // Now transpose into a new buffer and compute the columns
    FFTComplex* pRowFFTArrayT = new FFTComplex[ nHeight * nWidth ];
    TransposeRef( nWidth, nHeight, pOutArray, pRowFFTArrayT );

    // Now compute the FFT on all the rows in the transposed buffer
    for( INT32 x = 0; x < nWidth; x++ )
    {
        const FFTComplex* pInColData = &pRowFFTArrayT[x * nHeight];
        FFTComplex* pOutColData = &pTransposedOutput[x * nHeight];

        if( eDirection == FORWARD )
            FFTRef( nHeight, 0, 1, pInColData, pOutColData, pScratch );
        else
            IFFTRef( nHeight, pInColData, pOutColData );
    }
    delete [] pScratch;

    // Now transpose back to the input layout
    TransposeRef( nHeight, nWidth, pTransposedOutput, pOutArray );

    delete [] pRowFFTArrayT;
    delete [] pTransposedOutput;
}

//--------------------------------------------------------------------------------------
// Name: FFTRef()
// Desc: Perform FFT on pInData storing the results in pOutData on the CPU.  This is
// used to do a single 1D FFT of a row of data.
//--------------------------------------------------------------------------------------
VOID CFFT2D::FFTRef( INT32 nArraySize, INT32 nOffset, INT32 nDelta, const FFTComplex* pInData,
                     FFTComplex* pOutData, FFTComplex* pScratch )
{
    INT32 nHalfSize = nArraySize / 2;

    if( nArraySize != 2 )
    {
        FFTRef( nHalfSize, nOffset, 2 * nDelta, pInData, pScratch, pOutData );
        FFTRef( nHalfSize, nOffset + nDelta, 2 * nDelta, pInData, pScratch, pOutData );

        for( INT32 k = 0; k < nHalfSize; k++ )
        {
            // Compute the butterflies
            INT32 k00 = nOffset + k * nDelta;
            INT32 k01 = k00 + nHalfSize * nDelta;
            INT32 k10 = nOffset + 2 * k * nDelta;
            INT32 k11 = k10 + nDelta;

            FLOAT32 fCos = FrmCos( 2.0f * FRM_PI * k / (FLOAT32) nArraySize );
            FLOAT32 fSin = FrmSin( 2.0f * FRM_PI * k / (FLOAT32) nArraySize );

            FLOAT32 fTmp0 = fCos * pScratch[k11].fReal + fSin * pScratch[k11].fImaginary;
            FLOAT32 fTmp1 = fCos * pScratch[k11].fImaginary - fSin * pScratch[k11].fReal;

            pOutData[k01].fReal = pScratch[k10].fReal - fTmp0;
            pOutData[k01].fImaginary = pScratch[k10].fImaginary - fTmp1;
            pOutData[k00].fReal = pScratch[k10].fReal + fTmp0;
            pOutData[k00].fImaginary = pScratch[k10].fImaginary + fTmp1;
        }
    }
    else
    {
        INT32 k00 = nOffset;
        INT32 k01 = k00 + nDelta;
        pOutData[k01].fReal = pInData[k00].fReal - pInData[k01].fReal;
        pOutData[k01].fImaginary = pInData[k00].fImaginary - pInData[k01].fImaginary;
        pOutData[k00].fReal = pInData[k00].fReal + pInData[k01].fReal;
        pOutData[k00].fImaginary = pInData[k00].fImaginary + pInData[k01].fImaginary;

    }
}

//--------------------------------------------------------------------------------------
// Name: IFFTRef()
// Desc: Perform IFFT on pInData storing the results in pOutData on the CPU.  This is
// used to do a single 1D IFFT of a row of data.
//--------------------------------------------------------------------------------------
VOID CFFT2D::IFFTRef( INT32 nArraySize, const FFTComplex* pInData,
                      FFTComplex* pOutData )
{
    // Allocate scratch memory
    FFTComplex* pScratch = new FFTComplex[ nArraySize ];

    // Run forward FFT
    FFTRef( nArraySize, 0, 1, pInData, pOutData, pScratch );

    // Calculate IFFT
    INT32 nHalfSize = nArraySize / 2;
    pOutData[0].fReal = pOutData[0].fReal / nArraySize;
    pOutData[0].fImaginary = pOutData[0].fImaginary / nArraySize;

    pOutData[nHalfSize].fReal = pOutData[nHalfSize].fReal / nArraySize;
    pOutData[nHalfSize].fImaginary = pOutData[nHalfSize].fImaginary / nArraySize;

    for( INT32 i = 1; i < nHalfSize; i++ )
    {
        FFTComplex temp = pOutData[i];
        temp.fReal /= nArraySize;
        temp.fImaginary /= nArraySize;

        pOutData[i].fReal = pOutData[nArraySize - i].fReal / nArraySize;
        pOutData[i].fImaginary = pOutData[nArraySize - i].fImaginary / nArraySize;

        pOutData[nArraySize - i] = temp;
    }
}

//--------------------------------------------------------------------------------------
// Name: TransposeRef()
// Desc: Perform transpose of pInData to pOutData on CPU (reference)
//--------------------------------------------------------------------------------------
VOID CFFT2D::TransposeRef( INT32 nWidth, INT32 nHeight, const FFTComplex* pInData,
                           FFTComplex* pOutData )
{
    for( INT32 y = 0; y < nHeight; y++ )
    {
        for( INT32 x = 0; x < nWidth; x++ )
        {
            pOutData[x * nHeight + y] = pInData[y * nWidth + x];
        }
    }
}
