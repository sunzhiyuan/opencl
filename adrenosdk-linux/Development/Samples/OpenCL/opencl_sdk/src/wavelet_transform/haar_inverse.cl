/*======================================================================
 *  FILE:
 *      haar_inverse.cl
 *  DESCRIPTION:
 *      This file contains an OpenCL kernel of fixed point inverse
 *      Haar for QUALCOMM OpenCL benchmark application: wavelet transform.
 *
 *            Copyright (c) 2012 QUALCOMM Incorporated.
 *                      All Rights Reserved.
 *              QUALCOMM Confidential and Proprietary
 *
 *======================================================================*/

/**
 * \addtogroup wavelettransform
 * \{
 * \file haar_inverse.cl
 * \brief This file contains an OpenCL kernel of fixed point inverse
 *        Haar for QUALCOMM OpenCL benchmark application: wavelet transform.
 */


#define clamp2Uchar(x) \
    (unsigned char)(clamp((x), (short)(0), (short)(255)))

__constant unsigned short HaarCoef = 23170;

__constant unsigned int offset = (1 << 14);

__kernel void inverse_haar(__global unsigned char* out,
                           __global const short *inBuf,
                            unsigned short width,
                            unsigned short height)
{
    unsigned short xid = get_global_id(0);
    unsigned short yid = get_global_id(1);

    if(xid >= width || yid >= height)
        return;

    unsigned int lineIdx = mul24((unsigned int)(yid), (unsigned int)(width));

    unsigned int pixID = lineIdx + xid;

    int4 haarPixels;

    haarPixels = convert_int4(*((__global short4 *)(inBuf) + pixID)) * HaarCoef;

    unsigned int op_location1 = (lineIdx << 2) + (xid << 1);
    unsigned int op_location2 = op_location1 + (width << 1);

    short P0 = (haarPixels.x + haarPixels.z + offset) >> 15;
    short P1 = (haarPixels.x - haarPixels.z + offset) >> 15;
    short P2 = (haarPixels.y + haarPixels.w + offset) >> 15;
    short P3 = (haarPixels.y - haarPixels.w + offset) >> 15;

    out[op_location1]   = clamp2Uchar((short)(((P0 + P2) * HaarCoef + offset) >> 15));
    out[++op_location1] = clamp2Uchar((short)(((P0 - P2) * HaarCoef + offset) >> 15));
    out[op_location2]   = clamp2Uchar((short)(((P1 + P3) * HaarCoef + offset) >> 15));
    out[++op_location2] = clamp2Uchar((short)(((P1 - P3) * HaarCoef + offset) >> 15));

}
/** \} */
