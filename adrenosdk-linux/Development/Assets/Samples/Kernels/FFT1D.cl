//--------------------------------------------------------------------------------------
// File: FFT1D.cl
// Desc: Fast Fourier Transform 1D (forward and inverse) kernels
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#define PI (3.1415926f)


/**

 QC_OPT is defined in FFT1D.h. The original kernels are kept here for comparison reason.
 Undefine QC_OPT in FFT1D.h to enable original implementation.

**/
#ifndef QC_OPT
//--------------------------------------------------------------------------------------
// Name: FFT1DRadix2()
// Desc: Compute forward 1D FFT using log2(arraySize/2) kernel invocations.  The input
// array must be power-of-2 sized.
// Algorithm based on "OpenCL Fast Fourier Transform" by Eric Bainville
//--------------------------------------------------------------------------------------
__kernel void FFT1DRadix2( const int p,                      // 2^(iteration count)
                           __global const float2* pInArray,  // Input real/imaginary (FFTComplex) array
                           __global float2* pOutArray )      // Output real/imaginary (FFTComplex array)
{
    int i = get_global_id(0);
    int t = get_global_size(0);
    int k = i & ( p - 1 );

    float2 u0 = pInArray[i];
    float2 u1 = pInArray[i+t];

    // Compute the twiddle - the twiddle factor is described here:
    //  http://en.wikipedia.org/wiki/Cooley-Tukey_FFT_algorithm
    float alpha = -PI * (float) k / (float) p;
    float2 twiddle;
    twiddle.x = cos(alpha);
    twiddle.y = sin(alpha);

    // Apply the twiddle to value in second half of array
    float2 u1Twiddled;
    u1Twiddled.x = u1.x * twiddle.x - u1.y * twiddle.y;
    u1Twiddled.y = u1.x * twiddle.y + u1.y * twiddle.x;

    // Output the DFT2
    int j = (i << 1) - k;
    pOutArray[ j ] = u0 + u1Twiddled;
    pOutArray[ j + p ] = u0 - u1Twiddled;
}

//--------------------------------------------------------------------------------------
// Name: IFFT1DRadix2()
// Desc: Compute inverse 1D FFT using log2(arraySize/2) kernel invocations.  The input
// array must be power-of-2 sized.
//--------------------------------------------------------------------------------------
__kernel void IFFT1DRadix2( const int p,                      // 2^(iteration count)
                            __global const float2* pInArray,  // Input real/imaginary (FFTComplex) array
                            __global float2* pOutArray,       // Output real/imaginary (FFTComplex array)
                            const float oneOverN )            // 1.0f / arraySize
{
    int i = get_global_id(0);
    int t = get_global_size(0);
    int k = i & ( p - 1 );

    float2 u0 = pInArray[i];
    float2 u1 = pInArray[i+t];

    // Compute the twiddle - the twiddle factor is described here:
    //  http://en.wikipedia.org/wiki/Cooley-Tukey_FFT_algorithm
    float alpha = PI * (float) k / (float) p;
    float2 twiddle;
    twiddle.x = cos(alpha);
    twiddle.y = sin(alpha);

    // Apply the twiddle to value in second half of array
    float2 u1Twiddled;
    u1Twiddled.x = u1.x * twiddle.x - u1.y * twiddle.y;
    u1Twiddled.y = u1.x * twiddle.y + u1.y * twiddle.x;

    // Output the DFT2
    int j = (i << 1) - k;
    pOutArray[ j ] = ( u0 + u1Twiddled );
    pOutArray[ j + p ] = ( u0 - u1Twiddled );

    // On the last pass, divide the result by N
    if ( p == t )
    {
        pOutArray[ j ] *= oneOverN;
        pOutArray[ j + p ] *= oneOverN;
    }
}

#else

//--------------------------------------------------------------------------------------
// Name: FFT1DRadix2()
// Desc: This is an specialized version of FFT1DRadix2 kernel; it is run only once and only
// for input sizes with an odd power of 2.The input array must be power-of-2 sized in both
// dimensions. This kernel is used along with FFT1DRadix4.
// Each work item would do 2 x butterflies as defined in radix-2 FFT.
//--------------------------------------------------------------------------------------
__kernel void FFT1DRadix2_P0( const int width,                  // Width of a row
                              __global const float2* pInArray,  // Input real/imaginary (FFTComplex) array
                              __global float2* pOutArray,       // Output real/imaginary (FFTComplex array)
                              const int m)      // Such that 2^m == width
{

    int i = get_global_id(0)<<1;
    int t = get_global_size(0)<<1;
    float8 u;

 //Vector-loading of 4 floats is an optimal load. This kernel has been devised so that
 //data can be loaded in this vectorized manner.
    u.s0145 = *(__global float4*)&pInArray[i];
    u.s2367 = *(__global float4*)&pInArray[i + t];

    float2 temp = u.s23;
    u.s23 = u.s01 - u.s23;
    u.s01 = u.s01 + temp;

    int j = (i << 3) + (i >> (m - 3));
    pOutArray[j & (width-1) ] = u.s01;
    pOutArray[(j + 4) & (width-1) ] = u.s23;

    temp = u.s67;
    u.s67 = u.s45 - u.s67;
    u.s45 = u.s45 + temp;

    j += 8;
    pOutArray[j & (width-1) ] = u.s45;
    pOutArray[(j + 4) & (width-1) ] = u.s67;

}

//--------------------------------------------------------------------------------------
// Name: twiddleit()
// This is an auxiliary function for Radix-4 kernels.
//--------------------------------------------------------------------------------------

inline float2 twiddleit(const float2 u, const float alpha)
{
    float2 twiddle;
    float2 uTwiddled;
    twiddle.x = cos(alpha);
    twiddle.y = sin(alpha);
    uTwiddled.x = u.x * twiddle.x - u.y * twiddle.y;
    uTwiddled.y = u.x * twiddle.y + u.y * twiddle.x;
    return uTwiddled;
}



//--------------------------------------------------------------------------------------
// Name: FFT2DRadix4()
// Desc: Compute forward 1D FFT on each row of the data using log4(arraySize/4) kernel
// invocations.  The input array must be power-of-2 sized in both dimensions.
//--------------------------------------------------------------------------------------
__kernel void FFT1DRadix4( const int width,                  // Width of a row
                           const int p,                      // 4^(iteration count)
                           __global const float2* pInArray,  // Input real/imaginary (FFTComplex) array
                           __global float2* pOutArray,       // Output real/imaginary (FFTComplex array)
                           const float oneOverN,    // 1/width (used to avoid doing reciprocal)
                           const int m,       // Such that 2^m == width
                           const int sign,      // sign: -1 when forward, 1 when inverse
                           const float oneOverP)    // 1/p
{
    int i = get_global_id(0);
    int t = get_global_size(0);
    int k = i & ( p - 1 );

    float8 u;
    if(p == 1)
    {
        u.s01 = pInArray[i];
        u.s23 = pInArray[i + t];
        //When the range of int ops are within 24 bits, use mul24/mad24
        u.s45 = pInArray[mad24(2, t, i)];
        u.s67 = pInArray[mad24(3, t, i)];
    }
    else
    {
        u = *(__global float8*)&pInArray[i<<2];


        float alpha = sign * k * oneOverP;
        u.s23 = twiddleit(u.s23, alpha * 1.5707963);//.5 * PI
        u.s45 = twiddleit(u.s45, alpha * PI);
        u.s67 = twiddleit(u.s67, alpha * 4.7123889);//1.5 * PI
    }

    {
        float2 v0, v1, v2, v3;
        v0 = u.s01 + u.s45;
        v1 = u.s23 + u.s67;
        v2 = u.s01 - u.s45;
        v3.x = u.s3 - u.s7;
        v3.y = u.s6 - u.s2;
        v3 *= -sign;
        u.s01 = v0 + v1;
        u.s45 = v0 - v1;
        u.s23 = v2 + v3;
        u.s67 = v2 - v3;
    }

    if(p == t)
    {
        pOutArray[i] = u.s01 * oneOverN;
        pOutArray[i + t] = u.s23 * oneOverN;
        pOutArray[mad24(2, t, i)] = u.s45 * oneOverN;
        pOutArray[mad24(3, t, i)] = u.s67 * oneOverN;

    }
    else
    {
        int j = (i << 4) + (i >> (m - 4)) - mul24(12, k);

        pOutArray[j & (width-1) ] = u.s01;
        pOutArray[mad24(4, p, j) & (width-1) ] = u.s23;
        pOutArray[mad24(8, p, j) & (width-1) ] = u.s45;
        pOutArray[mad24(12, p, j) & (width-1) ] = u.s67;
    }
}

#endif
