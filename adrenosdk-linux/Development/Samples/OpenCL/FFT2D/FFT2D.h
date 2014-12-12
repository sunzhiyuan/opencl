//--------------------------------------------------------------------------------------
// File: FFT1D.h
// Desc: Implementation of 2D FFT on OpenCL (and CPU reference)
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef FFT2D_H
#define FFT2D_H


typedef struct
{
    // Real part of complex number
    FLOAT32 fReal;

    // Imaginary part of complex number
    FLOAT32 fImaginary;

}  FFTComplex;

//--------------------------------------------------------------------------------------
// Name: class CFFT2D
// Desc: 2D FFT (forward and inverse) on OpenCL (and CPU reference)
//--------------------------------------------------------------------------------------
class CFFT2D
{
public:
    typedef enum
    {
        FFT_RADIX_2 = 0,
        IFFT_RADIX_2,
        FFT_RADIX_4,
        MATRIX_TRANSPOSE,
        NUM_FFT_KERNELS
    } KernelType;

    typedef enum
    {
        FORWARD,
        INVERSE
    } Direction;

    // Constructor/Destructor
    CFFT2D(CFrmTimer& t);
    virtual ~CFFT2D();

    // Initialize the OpenCL kernel resources
    BOOL Initialize( cl_context context, cl_device_id device, cl_command_queue commandQueue );

    // Perform Forward FFT
    BOOL Forward( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                  FFTComplex* pOutArray, BOOL bRefCPU = FALSE );

    // Perform Inverse FFT
    BOOL Inverse( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                  FFTComplex* pOutArray, BOOL bRefCPU = FALSE );

private:

    // Perform Forward FFT (Reference CPU implementation)
    VOID ForwardRef( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                     FFTComplex* pOutArray );

    // Perform Inverse FFT (Reference CPU implementation)
    VOID InverseRef( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                     FFTComplex* pOutArray );

    // Execute forward or inverse Radix-2 FFT with OpenCL
    BOOL ExecuteRadix2FFT( KernelType kernelType, const FFTComplex* pInArray,
                           INT32 nWidth, INT32 nHeight, FFTComplex* pOutArray );

    // Execute forward or inverse 1D FFT on each row of input using OpenCL
    BOOL ExecuteRowWiseRadix2FFT( KernelType kernelType, cl_mem memObjs[2],
                                  INT32 nWidth, INT32 nHeight );

    // Execute transpose inputMat and place results in outputMat using OpenCL
    BOOL ExecuteTranspose( INT32 nRows, INT32 nCols, cl_mem inputMat, cl_mem outputMat );

    // Allocate memory objects for input and output
    BOOL AllocateMemObjects( INT32 nWidth, INT32 nHeight, const FFTComplex* pInArray,
                             cl_mem* pObjIn, cl_mem* pObjOut, cl_mem* pObjScratch );

    // Perform Forward or Inverse FFT (Reference CPU implementation)
    VOID ForwardOrInverseRef( const FFTComplex* pInArray, INT32 nWidth, INT32 nHeight,
                              FFTComplex* pOutArray, Direction eDirection );

    // Recursive FFT reference implementation
    VOID FFTRef( INT32 nArraySize, INT32 nOffset, INT32 nDelta, const FFTComplex* pInData,
                 FFTComplex* pOutData, FFTComplex* pScratch );

    // Recursive IFFT reference implementation
    VOID IFFTRef( INT32 nArraySize, const FFTComplex* pInData,
                  FFTComplex* pOutData );


    VOID TransposeRef( INT32 nWidth, INT32 nHeight, const FFTComplex* pInData,
                       FFTComplex* pOutData );

    // Cleanup internal resources
    VOID Destroy();



private:

    // Execute forward or inverse 1D FFT on each row of input using OpenCL
    BOOL ExecuteRowWiseRadix2FFTOrig( KernelType kernelType, cl_mem memObjs[2],
                                  INT32 nWidth, INT32 nHeight );
    // Execute forward or inverse 1D FFT on each row of input using OpenCL
    BOOL ExecuteRowWiseRadix2FFTOpt( KernelType kernelType, cl_mem memObjs[2],
                                  INT32 nWidth, INT32 nHeight );


    // OpenCL context
    cl_context m_context;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL kernel
    cl_kernel m_kernels[NUM_FFT_KERNELS];

    CFrmTimer& timer;
    cl_device_type devType;
    cl_device_id deviceID;

};

#endif // FFT2D_H
