/*======================================================================
 *  FILE:
 *      common.h
 *  DESCRIPTION:
 *      Common macros shared between the reference and OpenCL functions for
 *      wavelet transform application
 *
 *            Copyright (c) 2012 QUALCOMM Incorporated.
 *                      All Rights Reserved.
 *              QUALCOMM Confidential and Proprietary
 *
 *======================================================================*/

/**
 * \defgroup wavelettransform wavelettransform
 * \brief QCLBenchmark wavelettransform project
 * Wavelet transform is to do mult-layer wavelet transform
 * \{
 * \file wavelettransform\common.h
 * \brief Common macros shared between the reference and OpenCL functions for
 *        wavelet transform application
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <math.h>
#include <time.h>
#include <string.h>

#define DEBUG 0


#define MAX_COLOR 3
#define MAX_LEVEL 4
//
//#if !defined(ANDROID)
#define LOGI printf
#define LOGE printf
//#else
//#include <android/log.h>  // enable this one for android app development.
//#endif

#define ASSERT_ERR_CODE(__code) \
    if(0 != (__code)){ \
    LOGI("Nonzero error code (0x%d) at %s:%d in function %s\n", (__code),__FILE__,__LINE__, __FUNCTION__ ); \
    exit(1); \
    }

#define ASSERT_COND(__cond) \
    if(!(__cond)){ \
    LOGI("Assert fired at %s:%d\n",__FILE__,__LINE__); \
    exit(1); \
    }

#define MIN(a,b) ((a)>(b)?(b):(a))
#define MAX(a,b) ((a)<(b)?(b):(a))


//////////////////////////   Settings for transform //////////////////////////

typedef struct TRANSFORMSETTINGS
{
    unsigned short wavelet_level;        ///< wavelet transform: can be from 2 to 4
    bool           isYenabled;
    bool           isUVenabled;
    bool           isScheduledEnabled;  ///< OpenCl related options.
    bool           isUseSource;         ///< OpenCl related options.
}StrtransformSettings;

// image buffers

typedef struct IMAGE_INFO_SET
{
    unsigned int        imgSize[MAX_COLOR];
    unsigned short      imgWidth[MAX_COLOR];
    unsigned short      imgHeight[MAX_COLOR];
    short               imgWidthPad[MAX_COLOR];
    short               imgHeightPad[MAX_COLOR];

}strImgInfo;

typedef struct IMAGE_BUFFER_SET
{
    // Image IO related buffers.
    unsigned char       *imgBuf[MAX_COLOR];      ///< Host buffers for OpenCL test

    // for verification.
    int                 *ref_image[MAX_COLOR];
}strImgBufSet;

#endif

/** \} */
