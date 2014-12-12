/*======================================================================
 *  FILE:
 *      haar_forward.cl
 *  DESCRIPTION:
 *      This file contains a fixed point inverse DWT kernel developed for
 *      QUALCOMM OpenCL benchmark application: wavelet transform.
 *
 *            Copyright (c) 2012 QUALCOMM Incorporated.
 *                      All Rights Reserved.
 *              QUALCOMM Confidential and Proprietary
 *
 *======================================================================*/

/**
 * \addtogroup wavelettransform
 * \{
 * \file haar_forward.cl
 * \brief This file contains a fixed point inverse DWT kernel developed for
 *        QUALCOMM OpenCL benchmark application: wavelet transform.
 */


__constant unsigned short HaarCoef = 23170;

__constant unsigned short offset = 1 << 14;

__kernel void forward_haar(__global short *outimg_DC,
                           __global int   *outbuf_AC,
                           __global const unsigned int* in,
                           unsigned short width,   //768
                           unsigned short height_half) //256, half of the original image width 512.
{
    unsigned short  global_id_x = get_global_id(0);
    int  global_id_y = get_global_id(1);

    if((global_id_x << 2) >= width)
        return;

    unsigned int lineOffset = mul24(global_id_y,  (int)(width >> 1));
    unsigned int load_offset = lineOffset + global_id_x;

    unsigned int pixels_1;
    unsigned int pixels_2;

    pixels_1 =  in[load_offset]; // note that the data type is int.. the input pixel is 8 bit. pack 4 pixel to read.
    pixels_2 =  in[load_offset + (width >> 2)];  // >> 2 is due to the pixel pack loading.

    unsigned int AC_store_offset = ((global_id_x << 1) + lineOffset);

    unsigned int dcAdd = (global_id_x << 1) + (lineOffset); //global_id_y * width;

    for(short i = 0; i < 2; i ++)
    {
        int x =  pixels_1 & 0xFF;
        int y = (pixels_1 >> 8) & 0xFF;
        int z =  pixels_2 & 0xFF;
        int w = (pixels_2 >> 8) & 0xFF;

        x *= HaarCoef;
        y *= HaarCoef;
        z *= HaarCoef;
        w *= HaarCoef;

        short P0 = (x + y + offset) >> 15;
        short P1 = (x - y + offset) >> 15;
        short P2 = (z + w + offset) >> 15;
        short P3 = (z - w + offset) >> 15;

        ushort4 vAC;

        outimg_DC[dcAdd++] = ((P0 + P2) * HaarCoef + offset) >> 15;

        vAC.x = 0;
        vAC.y = ((P1 + P3) * HaarCoef + offset) >> 15;
        vAC.z = ((P0 - P2) * HaarCoef + offset) >> 15;
        vAC.w = ((P1 - P3) * HaarCoef + offset) >> 15;

        *((__global ushort4 *)(outbuf_AC) + AC_store_offset) = vAC;

        AC_store_offset++;

        pixels_1 >>= 16;
        pixels_2 >>= 16;
    }
}


/** \} */
