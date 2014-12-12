//--------------------------------------------------------------------------------------
// File: FFT1D.h
// Desc: Implementation of 1D FFT on OpenCL (and CPU reference)
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef FFT1D_H
#define FFT1D_H

typedef struct
{
    // Real part of complex number
    FLOAT32 fReal;

    // Imaginary part of complex number
    FLOAT32 fImaginary;

}  FFTComplex;
//--------------------------------------------------------------------------------------
// Name: class CFFT1D
// Desc: 1D FFT (forward and inverse) on OpenCL (and CPU reference)
//--------------------------------------------------------------------------------------
class CFFT1D
{
public:
    typedef enum
    {
        FFT_RADIX_2 = 0,
        IFFT_RADIX_2,
        FFT_RADIX_4,
        NUM_FFT_KERNELS
    } KernelType;

    // Constructor/Destructor
    CFFT1D(CFrmTimer& m_Timer);
    virtual ~CFFT1D();

    // Initialize the OpenCL kernel resources
    BOOL Initialize( cl_context context, cl_device_id device, cl_command_queue commandQueue );

    // Perform Forward FFT
    BOOL Forward( const FFTComplex* pInArray, INT32 nArraySize,
                  FFTComplex* pOutArray, BOOL bRefCPU = FALSE );

    // Perform Inverse FFT
    BOOL Inverse( const FFTComplex* pInArray, INT32 nArraySize,
                  FFTComplex* pOutArray, BOOL bRefCPU = FALSE );

private:

    // Perform Forward DFT (Reference CPU implementation)
    VOID ForwardRef( const FFTComplex* pInArray, INT32 nArraySize,
                     FFTComplex* pOutArray );

    // Perform Inverse DFT (Reference CPU implementation)
    VOID InverseRef( const FFTComplex* pInArray, INT32 nArraySize,
                     FFTComplex* pOutArray );

    // Execute forward or inverse Radix-2 FFT with OpenCL
    BOOL ExecuteRadix2FFT( KernelType kernelType, const FFTComplex* pInArray,
                           INT32 nArraySize, FFTComplex* pOutArray );

    // Allocate memory objects for input and output
    BOOL AllocateMemObjects( INT32 nArraySize, const FFTComplex* pInArray,
                             cl_mem* pObjIn, cl_mem* pObjOut );

    // Cleanup internal resources
    VOID Destroy();

private:

    // OpenCL context
    cl_context m_context;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL kernel
    cl_kernel m_kernels[NUM_FFT_KERNELS];

    CFrmTimer& timer;
    cl_device_type devType;

};

#endif // FFT1D_H
