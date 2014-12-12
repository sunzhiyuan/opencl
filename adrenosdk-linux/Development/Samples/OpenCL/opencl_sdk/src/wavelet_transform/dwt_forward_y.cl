/*======================================================================
*  FILE:
*      dwt_forward_y.cl
*  DESCRIPTION:
*      This file contains a fixed point forward DWT kernel developed for
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
* \file dwt_forward_y.cl
* \brief This file contains a fixed point y-directional forward DWT kernel developed for
*        QUALCOMM OpenCL benchmark application: wavelet transform.
*/


// Q14

__constant short l_0 =  2896;
__constant short l_1 =  5793;
__constant short l_2 =  17378;

// Q15

__constant short h_0 =  11585;
__constant short h_1 =  23170;

__kernel void fDWT_Y(__global short *outImgDC, // __global short *outImg,
                     __global short *outBufAC,
                     __global const short *inImg,
                     unsigned short dwt_width_half,
                     unsigned short dwt_height_half)
{

    unsigned short global_id_x = get_global_id(0);
    unsigned short global_id_y = get_global_id(1);

    // need six pixels.

    // id_x    0           1           2           3
    // X | y1.x y1.y | y1.x y1.y | y1.x y1.y | y1.x y1.y |    Y_id
    // X | y2.x y2.y | y2.x y2.y | y2.x y2.y | y2.x y2.y |
    // O | y3.x y3.y | y3.x y3.y | y3.x y3.y | y3.x y3.y |    0
    // O | y4.x y4.y | y4.x y4.y | y4.x y4.y | y4.x y4.y |
    // X | y5.x y5.y | y5.x y5.y | y5.x y5.y | y5.x y5.y |    1

    short2 y1, y2, y3, y4, y5;

    if(global_id_x < dwt_width_half && global_id_y < dwt_height_half)
    {
        // calculate the address:
        int add = mul24((global_id_y << 1), dwt_width_half) + global_id_x;

        y3 = *((__global short2 *)(inImg) + add);

        y4 = *((__global short2 *)(inImg) + add + dwt_width_half);

        if(global_id_y + 1 != dwt_height_half)// not on boundary
        {
            y5 = *((__global short2 *)(inImg) + add + dwt_width_half + dwt_width_half);
        }
        else // bottome boundary
        {
            y5 = y3;
        }

        if(global_id_y != 0)// not on boundary
        {
            add -= (dwt_width_half << 1);

            y1 = *((__global short2 *)(inImg) + add);

            add += dwt_width_half;

            y2 = *((__global short2 *)(inImg) + add);
        }
        else // bottom boundary
        {
            y1 = y5;

            y2 = y4;
        }

        int2 lowP  = (convert_int2)((convert_int2)(-y1) * l_0 + (convert_int2)(y2) * l_1 + (convert_int2)(y3) * l_2 + (convert_int2)(y4) * l_1 - (convert_int2)(y5) * l_0);

        int2 highP  = (convert_int2)((convert_int2)(-y3) * h_0 + (convert_int2)(y4) * h_1 - (convert_int2)(y5) * h_0);

        lowP = (lowP + (1 << 13)) >> 14;

        highP = (highP + (1 << 14)) >> 15;

        int store_offset = mul24((uint)global_id_y, (uint)(dwt_width_half)) + global_id_x;

        outImgDC[store_offset] = lowP.s0;

        *((__global short4 *)(outBufAC) + store_offset) = (short4)(lowP.s0, lowP.s1, highP.s0, highP.s1);
    }
}
/** \} */
