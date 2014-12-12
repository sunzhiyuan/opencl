/*======================================================================
 *  FILE:
 *      dwt_inverse_y.cl
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
 * \file dwt_inverse_y.cl
 * \brief This file contains a fixed point y-directional inverse DWT kernel developed for
 *        QUALCOMM OpenCL benchmark application: wavelet transform.
 */

__constant int2 idwt_x = {-11585, -2896};
__constant int2 idwt_y = {23170, 5793};
__constant int2 idwt_z = {-11585, 17378};
__constant int2 idwt_h_last = {5793, -2896};

__kernel void iDWT_Y(__global short *out_image,
                     __global short *in_img,
                     unsigned short dwt_width,
                     unsigned short dwt_height)
{
    unsigned short x_global_id = get_global_id(0);
    unsigned short y_global_id = get_global_id(1);

    unsigned int offset;

    short4 top_pixel, bot_pixel, cur_pixel;

    if(x_global_id < dwt_width && y_global_id < dwt_height)
    {
        offset = x_global_id + mul24((unsigned int)y_global_id, (unsigned int)dwt_width);

        // read top line.
        // if it is the global top boundary, replicate the top line.

        cur_pixel = *((__global short4 *)(in_img) + offset);

        if(y_global_id) // not on top boundary
        {
            top_pixel = *((__global short4 *)(in_img) + offset - dwt_width);
        }
        else
        {
            top_pixel = cur_pixel;
        }

        if(y_global_id + 1 != dwt_height) // not on bot boundary
        {
            bot_pixel = *((__global short4 *)(in_img) + offset + dwt_width);
        }
        else
        {
            bot_pixel = cur_pixel;
            bot_pixel.xy = cur_pixel.xy;
            bot_pixel.zw = top_pixel.zw;
        }

        int4 fdwt;

        fdwt.xy = (top_pixel.z * idwt_x + cur_pixel.x * idwt_y + cur_pixel.z * idwt_z);

        fdwt.y += bot_pixel.x * idwt_h_last.x + bot_pixel.z * idwt_h_last.y;

        fdwt.zw = (top_pixel.w * idwt_x + cur_pixel.y * idwt_y +  cur_pixel.w * idwt_z);

        fdwt.w += bot_pixel.y * idwt_h_last.x + bot_pixel.w * idwt_h_last.y;

        fdwt.xz = (fdwt.xz + (1 << 14)) >> 15;

        fdwt.yw = (fdwt.yw + (1 << 13)) >> 14;

        *((__global short4 *)(out_image) + offset) = (convert_short4)(fdwt);
    }
 }

 /** \} */
