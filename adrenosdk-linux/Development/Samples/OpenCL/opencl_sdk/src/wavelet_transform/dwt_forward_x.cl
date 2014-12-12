/*======================================================================
*  FILE:
*      dwt_forward_x.cl
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
* \file dwt_forward_x.cl
* \brief This file contains a fixed point x-directional forward DWT  kernel developed for
*        QUALCOMM OpenCL benchmark application: wavelet transform.
*/

// Q14

__constant short l_0 = 2896;
__constant short l_1 = 5793;
__constant short l_2 = 17378;

// Q15

__constant short h_0 = 11585;
__constant short h_1 = 23170;

__kernel void fDWT_X(__global short* outImg,
                     __global short* inImg,
                     unsigned short dwt_width_half,
                     unsigned short dwt_height)
{
    unsigned short imgStride = (dwt_width_half << 1);

    unsigned short global_id_x = get_global_id(0);

    int global_id_y = get_global_id(1);

    if(global_id_x < dwt_width_half && global_id_y < dwt_height)
    {
        short4 vec4pixel;

        short pixel5;

        __global short* pPixelAdd = inImg + mul24(global_id_y, imgStride) + (global_id_x << 1);

        if(global_id_x != 0) // not on the left boundary
        {
            vec4pixel = *(__global short4 *)( pPixelAdd - 2);

            pixel5 = (global_id_x + 1 != dwt_width_half) ? (*(pPixelAdd + 2)) : (*pPixelAdd);
        }
        else // on the left boundary, simply dumplicate
        {
            short4 tempV = *(__global short4 *)(pPixelAdd);

            vec4pixel.x = tempV.z;

            vec4pixel.y = tempV.y;

            vec4pixel.z = tempV.x;

            vec4pixel.w = tempV.y;

            pixel5 = tempV.z;
        }

        int x  = -vec4pixel.x * l_0 + vec4pixel.y * l_1 + vec4pixel.z * l_2 + vec4pixel.w * l_1 - pixel5 * l_0;

        int y  = -vec4pixel.z * h_0 + vec4pixel.w * h_1 - pixel5 * h_0;

        unsigned int address_out = mul24(global_id_y, imgStride) + (global_id_x << 1);

        outImg[address_out] = (x + (1 << 13)) >> 14;

        outImg[address_out+1] = (y + (1 << 14)) >> 15;
    }
}

/** \} */
