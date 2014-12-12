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


#ifdef QC_OPT //This is defined in host code when run on GPU
//--------------------------------------------------------------------------------------
// Name: FFT2DRadix2_P0()
// Desc: This is a version of FFT2DRadix2 specilized for p=1. This is used for a first
// pass of processing of input sizes that are an odd power of 2.
//--------------------------------------------------------------------------------------
__kernel void FFT2DRadix2_P0( const int width,                  // Width of a row
                              __global const float2* pInArray,  // Input real/imaginary (FFTComplex) array
                              __global float2* pOutArray,       // Output real/imaginary (FFTComplex array)
                              const int m)                      // Such that 2^m == width
{

    int i = get_global_id(0)<<1;
    int t = get_global_size(0)<<1;
    int row = get_global_id(1);
    int startOffset = mul24(row, width);
    float8 u;

    //Vector-loading of 4 floats is an optimal load. This kernel has been devised so that
    //data can be loaded in this vectorized manner.
    u.s0145 = *(__global float4*)&pInArray[startOffset + i];
    u.s2367 = *(__global float4*)&pInArray[startOffset + i + t];

    u.s0123 = (float4)(u.s01 + u.s23, u.s01 - u.s23);

    int j = (i << 3) + (i >> (m - 3));
    pOutArray[startOffset + (j & (width-1)) ] = u.s01;
    pOutArray[startOffset + ((j + 4) & (width-1)) ] = u.s23;

    u.s4567 = (float4)(u.s45 + u.s67, u.s45 - u.s67);

    j += 8;
    pOutArray[startOffset + (j & (width-1)) ] = u.s45;
    pOutArray[startOffset + ((j + 4) & (width-1)) ] = u.s67;

}


inline float2 twiddleit(const float2 u, const int k, const int p, const float alpha)
{
    float2 twiddle;
    float2 uTwiddled;
    //Using "native" version of math functions (if supported) indicates that precision
    //is can be traded off for performance (similar effect as using -cl-fast-relaxed-math.
    //Note though most of the time the computation would still result in bi-correct results
    twiddle.x = native_cos(alpha);
    twiddle.y = native_sin(alpha);
    uTwiddled.x = u.x * twiddle.x - u.y * twiddle.y;
    uTwiddled.y = u.x * twiddle.y + u.y * twiddle.x;
    return uTwiddled;
}

//--------------------------------------------------------------------------------------
// Name: FFT2DRadix4()
// Desc: Compute forward 1D FFT on each row of the data using log4(arraySize/4) kernel
// invocations.  The input array must be power-of-4 sized in both dimensions.  The 2D FFT
// is accomplished by invoking first on all rows, then on all columns (by transposing)
//--------------------------------------------------------------------------------------
__kernel void FFT2DRadix4( const int width,                  // Width of a row
                           const int p,                      // 4^(iteration count)
                           __global const float2* pInArray,  // Input real/imaginary (FFTComplex) array
                           __global float2* pOutArray,       // Output real/imaginary (FFTComplex array)
                           const float oneOverN,             // 1/Width
                           const int m,                      // Such that 2^m == width
                           const int sign,                   // sign: -1 for forward, 1 for inverse
                           const float oneOverP)             // 1/p
{
    int i = get_global_id(0);
    int row = get_global_id(1);
    //When the range of int is known to be within 24 bits, mul24/mad24 should be used for better perf.
    int startOffset = mul24(row, width);
    int t = get_global_size(0);
    int k = i & ( p - 1 );

    float8 u;
    if(p == 1)
    {
        u.s01 = pInArray[startOffset + i];
        u.s23 = pInArray[startOffset + i + t];
        u.s45 = pInArray[startOffset + mad24(2, t, i)];
        u.s67 = pInArray[startOffset + mad24(3, t, i)];
    }
    else
    {
        //This algorithm has been devised to allow vector loading of input
        //data, which is the most optimal way of loading.
        u = *(__global float8*)&pInArray[startOffset + (i<<2)];

        float alpha = sign * k * oneOverP;
        u.s23 = twiddleit(u.s23, k, p, alpha * 1.5707963);//.5 * PI
        u.s45 = twiddleit(u.s45, k, p, alpha * PI);
        u.s67 = twiddleit(u.s67, k, p, alpha * 4.7123889);//1.5 * PI
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
        //oneOverN is pre-computed on host to avoid doing reciprocal per work item
        pOutArray[startOffset + i] = u.s01 * oneOverN;
        pOutArray[startOffset + i + t] = u.s23 * oneOverN;
        pOutArray[startOffset + mad24(2, t, i)] = u.s45 * oneOverN;
        pOutArray[startOffset + mad24(3, t, i)] = u.s67 * oneOverN;

    }
    else
    {
        int j = (i << 4) + (i >> (m - 4)) - mul24(12, k);

        pOutArray[startOffset + (j & (width-1)) ] = u.s01;
        pOutArray[startOffset + ((j + (p << 2)) & (width-1)) ] = u.s23;
        pOutArray[startOffset + ((j + (p << 3)) & (width-1)) ] = u.s45;
        pOutArray[startOffset + ((j + mul24(3, p<<2)) & (width-1)) ] = u.s67;
    }
}

//--------------------------------------------------------------------------------------
// Name: MatrixTranspose()
// Desc: Compute the transpose of a matrix
// This transpose kernel has been optimized to load and store float4 which happens to be
// a very optimal way of loading/storing. Note that the work group size has a tremendous
// effect on the performance of this kernel. Currently, it's been tuned to a 4x64 local
// work group but that needs to be tuned for particular device.
//--------------------------------------------------------------------------------------
__kernel void MatrixTranspose(const uint rows,
                              const uint cols,
                              __global float2* matrix,
                              __global float2* matrixTranspose)
{
    const uint i = get_global_id(0)<<1;
    const uint j = get_global_id(1)<<1;

    float4 temp = *(__global float4*)&matrix[mad24(j, cols, i)];
    float4 temp1 = *(__global float4*)&matrix[mad24(j, cols, i) + cols];

    *(__global float4*)&matrixTranspose[mad24(i, rows, j)] = (float4)(temp.s01, temp1.s01);
    *(__global float4*)&matrixTranspose[mad24(i, rows, j) + rows] = (float4)(temp.s23, temp1.s23);
}
#else
//--------------------------------------------------------------------------------------
// Name: FFT2DRadix2()
// Desc: Compute forward 1D FFT on each row of the data using log2(arraySize/2) kernel
// invocations.  The input array must be power-of-2 sized in both dimensions.  The 2D FFT
// is accomplished by invoking first on all rows, then on all columns (by transposing)
//--------------------------------------------------------------------------------------
__kernel void FFT2DRadix2( const int width,                  // Width of a row
                           const int p,                      // 2^(iteration count)
                           __global const float2* pInArray,  // Input real/imaginary (FFTComplex) array
                           __global float2* pOutArray )      // Output real/imaginary (FFTComplex array)
{
    int i = get_global_id(0);
    int row = get_global_id(1);
    int startOffset = row * width;
    int t = get_global_size(0);
    int k = i & ( p - 1 );

    float2 u0 = pInArray[startOffset + i];
    float2 u1 = pInArray[startOffset + i + t];

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
    j += startOffset;
    pOutArray[ j ] = u0 + u1Twiddled;
    pOutArray[ j + p ] = u0 - u1Twiddled;
}

//--------------------------------------------------------------------------------------
// Name: IFFT2DRadix2()
// Desc: Compute inverse 1D FFT on each row of the data using log2(arraySize/2) kernel
// invocations.  The input array must be power-of-2 sized in both dimensions.  The 2D FFT
// is accomplished by invoking first on all rows, then on all columns (by transposing)
//--------------------------------------------------------------------------------------
__kernel void IFFT2DRadix2( const int width,                  // Width of a row
                            const int p,                      // 2^(iteration count)
                            __global const float2* pInArray,  // Input real/imaginary (FFTComplex) array
                            __global float2* pOutArray,       // Output real/imaginary (FFTComplex array)
                            const float oneOverN )            // 1.0f / arraySize
{
    int i = get_global_id(0);
    int t = get_global_size(0);
    int row = get_global_id(1);
    int startOffset = row * width;
    int k = i & ( p - 1 );

    float2 u0 = pInArray[startOffset + i];
    float2 u1 = pInArray[startOffset + i + t];

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
    j+= startOffset;
    pOutArray[ j ] = ( u0 + u1Twiddled );
    pOutArray[ j + p ] = ( u0 - u1Twiddled );

    // On the last pass, divide the result by N
    if ( p == t )
    {
        pOutArray[ j ] *= oneOverN;
        pOutArray[ j + p ] *= oneOverN;
    }
}

//--------------------------------------------------------------------------------------
// Name: MatrixTranspose()
// Desc: Compute the transpose of a matrix
//--------------------------------------------------------------------------------------
__kernel void MatrixTranspose(const int rows,
                              const int cols,
                              __global float2* matrix,
                              __global float2* matrixTranspose)
{
    int row = get_global_id(0);

    if( row < rows )
    {
        for( int col = 0; col < cols; col++ )
        {
            matrixTranspose[col * rows + row] = matrix[row * cols + col];
        }
    }
}
#endif
