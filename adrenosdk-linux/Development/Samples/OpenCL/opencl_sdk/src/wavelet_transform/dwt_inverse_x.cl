/*======================================================================
*  FILE:
*      dwt_inverse_x.cl
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
* \file dwt_inverse_x.cl
* \brief This file contains a fixed point x-directional inverse DWT kernel developed for
*        QUALCOMM OpenCL benchmark application: wavelet transform.
*/

__constant short l[3] = {-11585, 23170, -11585};

__constant short h[5] = {-2896, 5793, 17378, 5793, -2896};

__kernel void iDWT_X(__global short *out_image,
                     __global const short *in_img,
                     unsigned short dwt_width,
                     unsigned short dwt_height)
{
    unsigned short  x_global_id = get_global_id(0);

    unsigned short  y_global_id = get_global_id(1);

    short2 X1, X2, X3, X4, X5, X6, X7; //X2, X4, X6 are low pass components.

    const int globalPixAdd = mul24((unsigned int)(y_global_id), (unsigned int)(dwt_width << 1)) + (x_global_id << 1);

    short4 left4, right4, middle4;

    if(x_global_id < dwt_width && y_global_id < dwt_height)
    {
        unsigned int globalAddOffset = globalPixAdd;

        left4   = *((__global short4 *)(in_img) + globalAddOffset);

        globalAddOffset++;

        middle4 = *((__global short4 *)(in_img) + globalAddOffset);

        globalAddOffset++;

        X2 = left4.xy;

        X3 = left4.zw;

        X4 = middle4.xy;

        X5 = middle4.zw;

        if(x_global_id > 0) // not on the left boundary.
        {
            X1 = *((__global short2 *)(in_img) + (globalPixAdd << 1) - 1);
        }
        else //on the left boundary.
        {
            X1  = left4.zw;
        }

        if(x_global_id + 1 < dwt_width) // right boundary not reached.
        {
            right4 = *((__global short4 *)(in_img) + globalPixAdd + 2);

            X6 = right4.xy;
            X7 = right4.zw;
        }
        else // right boundary
        {
            X6 = X4;  // low frequency.
            X7 = X3;
        }

        short2 x, y, z, w;

        // process low pass

        x = convert_short2(((l[0] * (convert_int2)(X1) + l[1] * (convert_int2)(X2) + l[2] * (convert_int2)(X3)) + (1 << 14)) >> 15);
        z = convert_short2(((l[0] * (convert_int2)(X3) + l[1] * (convert_int2)(X4) + l[2] * (convert_int2)(X5)) + (1 << 14)) >> 15);

        // process high pass

        y  = convert_short2(((h[0] * (convert_int2)(X1) + h[1] * (convert_int2)(X2) + h[2] * (convert_int2)(X3) + h[3] * (convert_int2)(X4) + h[4] * (convert_int2)(X5)) + (1 << 13)) >> 14);
        w  = convert_short2(((h[0] * (convert_int2)(X3) + h[1] * (convert_int2)(X4) + h[2] * (convert_int2)(X5) + h[3] * (convert_int2)(X6) + h[4] * (convert_int2)(X7)) + (1 << 13)) >> 14);

        int outadd = (y_global_id * dwt_width << 5) + (x_global_id << 4);

        // top line
        // x.s0, y.s0 z.s0 w.s0

        out_image[outadd] = x.s0;

        outadd += 4;

        out_image[outadd] = y.s0;

        outadd += 4;

        out_image[outadd] = z.s0;

        outadd += 4;

        out_image[outadd] = w.s0;

        // bottom line
        // x.s1, y.s1 z.s1 w.s1

        outadd += (dwt_width << 4);

        outadd -= 12;

        out_image[outadd] = x.s1;

        outadd += 4;

        out_image[outadd] = y.s1;

        outadd += 4;

        out_image[outadd] = z.s1;

        outadd += 4;

        out_image[outadd] = w.s1;
    }
}
/** \} */
