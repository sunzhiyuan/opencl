//--------------------------------------------------------------------------------------
// File: ImageRecursiveGaussianFilter.cl
// Desc: Performs a recursive gaussian filter on an image.  This implementation is 
// based on the following papers:
//
//  "Recursive Gaussian Filter for Mathematica using CUDA by Patrick Scheibe
//  "Recursive Implementation of the Gaussian Filter" by Ian T. Young and Lucas J. van Vilet
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Name: ImageGaussianFilterHorizontal()
// Desc: Each invocation of this kernel performs the recursive gaussian kernel for
// a horizontal row of the image.  
//--------------------------------------------------------------------------------------
__kernel void ImageGaussianFilterHorizontal( __read_only image2d_t source, // Source image
                                             __write_only image2d_t dest,  // Intermediate dest image
                                             sampler_t sampler,            // Sampler
                                             int imgWidth,                 // Image width
                                             __local float4* rowData,      // Local working memory for row data
                                             float b0,                     // Gaussian Params b0 (see CSample::GaussParams)
                                             float b1,                     // Gaussian Params b1 (see CSample::GaussParams)
                                             float b2,                     // Gaussian Params b2 (see CSample::GaussParams)
                                             float b3,                     // Gaussian Params b3 (see CSample::GaussParams)
                                             float b4)                     // Gaussian Params b4 (see CSample::GaussParams)
{
    int y = get_global_id(0);

    // First, read all of the image data for this row into local memory.  This is
    // to reduce global memory access and because the algorithm must
    // read/write to the same regions of memory
    for( int x = 0; x < imgWidth; x++ )
    {
        rowData[x] = read_imagef( source, sampler, (int2) ( x, y ) );
    }

    // Now compute the recursive gaussian filter on the row data
    __local float4* in = rowData;
    float4 val = in[0];
            
    // The first three pixels are computed manually assuming "clamping" (e.g.,
    // val is reused for each pixel)
    in[0] = (b4 * val + ( b1 * val + b2 * val + b3 * val ) / b0);
    in[1] = (b4 * in[1] + ( b1 * in[0] + b2 * val + b3 * val ) / b0);
    in[2] = (b4 * in[2] + ( b1 * in[1] + b2 * in[0] + b3 * val ) / b0);

    // Now for each pixel that has three previous neighbors, process recursively forward
    for( int x = 3; x < imgWidth; x++ )
    {
        in[x] = (b4 * in[x] + (b1 * in[x-1] + b2 * in[x-2] + b3 * in[x-3]) / b0);
    }

    // Process the last three pixels clamping to the last value
    int x = imgWidth - 1;
    val = in[x];
    in[x] = (b4 * val + ( b1 * val + b2 * val + b3 * val) / b0);
    in[x-1] = (b4 * in[x-1] + ( b1 * in[x] + b2 * val + b3 * val) / b0);
    in[x-2] = (b4 * in[x-2] + ( b1 * in[x-1] + b2 * in[x] + b3 * val) / b0);

    // Now for each pixel has has three post neighbors, process recursively backwards
    for( int i = x - 3; i >= 0; --i )
    {
        in[i] = (b4 * in[i] + (b1 * in[i+1] + b2 * in[i+2] + b3 * in[i+3]) / b0);
    }

    // Finally write all of the processed row out to the destination
    for( int x = 0; x < imgWidth; x++ )
    {
        write_imagef( dest, (int2) ( x, y ), rowData[x] );
    }
}

//--------------------------------------------------------------------------------------
// Name: ImageGaussianFilterVertical()
// Desc: Each invocation of this kernel performs the recursive gaussian kernel for
// a vertical column of the image.  
//--------------------------------------------------------------------------------------
__kernel void ImageGaussianFilterVertical( __read_only image2d_t source,   // Intermediate image processed by ImageGaussianFilterHorizontal()
                                           __write_only image2d_t dest,  // Final destination image
                                           sampler_t sampler,            // Sampler
                                           int imgHeight,                // Image height
                                           __local float4* colData,      // Local working memory for column data
                                           float b0,                     // Gaussian Params b0 (see CSample::GaussParams)
                                           float b1,                     // Gaussian Params b1 (see CSample::GaussParams)
                                           float b2,                     // Gaussian Params b2 (see CSample::GaussParams)
                                           float b3,                     // Gaussian Params b3 (see CSample::GaussParams)
                                           float b4)                     // Gaussian Params b4 (see CSample::GaussParams)
{
    int x = get_global_id(0);

    // First, read all of the image data for this column into local memory.  This is
    // to reduce global memory access and because the algorithm must
    // read/write to the same regions of memory
    for( int y = 0; y < imgHeight; y++ )
    {
        colData[y] = read_imagef( source, sampler, (int2) ( x, y ) );
    }

    // Now compute the recursive gaussian filter on the row data
    __local float4* in = colData;
    float4 val = in[0];
            
    // The first three pixels are computed manually assuming "clamping" (e.g.,
    // val is reused for each pixel)
    in[0] = (b4 * val + ( b1 * val + b2 * val + b3 * val ) / b0);
    in[1] = (b4 * in[1] + ( b1 * in[0] + b2 * val + b3 * val ) / b0);
    in[2] = (b4 * in[2] + ( b1 * in[1] + b2 * in[0] + b3 * val ) / b0);

    // Now for each pixel that has three previous neighbors, process recursively forward
    for( int y = 3; y < imgHeight; y++ )
    {
        in[y] = (b4 * in[y] + (b1 * in[y-1] + b2 * in[y-2] + b3 * in[y-3]) / b0);
    }

    // Process the last three pixels clamping to the last value
    int y = imgHeight - 1;
    val = in[y];
    in[y] = (b4 * val + ( b1 * val + b2 * val + b3 * val) / b0);
    in[y-1] = (b4 * in[y-1] + ( b1 * in[y] + b2 * val + b3 * val) / b0);
    in[y-2] = (b4 * in[y-2] + ( b1 * in[y-1] + b2 * in[y] + b3 * val) / b0);

    // Now for each pixel has has three post neighbors, process recursively backwards
    for( int i = y - 3; i >= 0; --i )
    {
        in[i] = (b4 * in[i] + (b1 * in[i+1] + b2 * in[i+2] + b3 * in[i+3]) / b0);
    }

    // Finally write all of the processed row out to the destination
    for( int y = 0; y < imgHeight; y++ )
    {
        write_imagef( dest, (int2) ( x, y ), colData[y] );
    }
}
