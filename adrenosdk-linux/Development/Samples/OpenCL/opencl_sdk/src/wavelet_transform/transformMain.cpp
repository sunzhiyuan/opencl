/*======================================================================
*  FILE:
*      transformMain.cpp
*  DESCRIPTION:
*      Main functions for wavelet transform application
*
*            Copyright (c) 2012 QUALCOMM Incorporated.
*                      All Rights Reserved.
*              QUALCOMM Confidential and Proprietary
*
*======================================================================*/

/**
* \addtogroup wavelettransform
* \{
* \file wavelettransform\transformMain.cpp
* \brief Main functions for wavelet transform application
*/
#include "cltransform.h"
#include "common.h"

#if !defined(ANDROID)
#include <windows.h>                // for Windows APIs
#else
#include <sys/time.h>
#endif

#define ENABLE_VERIFICATION             ///< enable/disable verification.

#define ITERATIONS 1                    ///< For profiling purpose: number of iterations to collect meaningful data.

unsigned char clamp2Uchar( int x )
{
    return (unsigned char) (MAX(MIN(((int)(x)),255),0));
}

unsigned char clamp2Uchar( float x )
{
    return (unsigned char) (MAX(MIN(x,255),0));
}


const int parameter_q_factor = 15;
const int image_q_factor = 0;

static int mismatches = 0;   ///< for verification.


extern int reference_transform(unsigned short level,
                        int            input_width,
                        int            input_height,
                        int           *y_image,
                        int           *cb_image,
                        int           *cr_image);


/**
* \brief Initialize the transform application
*/
void readImg(strImgBufSet  *imgBufset,
             strImgInfo    *imgParaSet,
             FILE          *fin,
             bool          isVerificationEnabled)
{
    imgBufset->imgBuf[0] = imgBufset->imgBuf[1] = imgBufset->imgBuf[2] = NULL;

    imgBufset->imgBuf[0] = (unsigned char *)malloc(sizeof(unsigned char ) * imgParaSet->imgSize[0] * 3);

    imgBufset->imgBuf[1] = imgBufset->imgBuf[0] + imgParaSet->imgSize[0];

    imgBufset->imgBuf[2] = imgBufset->imgBuf[1] + imgParaSet->imgSize[0];

    if(isVerificationEnabled)
    {
        imgBufset->ref_image[0] = (int *)malloc(sizeof(int) * imgParaSet->imgSize[0] * 3);
        imgBufset->ref_image[1] = imgBufset->ref_image[0] + imgParaSet->imgSize[0];
        imgBufset->ref_image[2] = imgBufset->ref_image[1] + imgParaSet->imgSize[0];
    }

    unsigned char *temp_buffer = (unsigned char *) malloc (sizeof (unsigned char) * (imgParaSet->imgWidth[0] + imgParaSet->imgWidthPad[0]) * 3);

    if(temp_buffer == NULL)
    {
        LOGI("No sufficient memory (temp_buffer)!\n");
        exit(1);
    }

    for (int i = 0; i < imgParaSet->imgHeight[0]; i++)
    {
        fread(temp_buffer, sizeof (unsigned char), imgParaSet->imgWidth[0] * 3, fin);

        for (int j = 0; j < imgParaSet->imgWidth[0]; j++)
        {
            for(int color = 0; color < MAX_COLOR; color++)
            {
                imgBufset->imgBuf[color][i * (imgParaSet->imgWidth[0] +  imgParaSet->imgWidthPad[0])+ j] = ((unsigned char) temp_buffer[j * MAX_COLOR + color]);
            }

            imgBufset->ref_image[0][i * (imgParaSet->imgWidth[0] +  imgParaSet->imgWidthPad[0]) + j] = temp_buffer[j * MAX_COLOR];
            imgBufset->ref_image[1][i * (imgParaSet->imgWidth[1] +  imgParaSet->imgWidthPad[1]) + j] = temp_buffer[j * MAX_COLOR + 1];
            imgBufset->ref_image[2][i * (imgParaSet->imgWidth[2] +  imgParaSet->imgWidthPad[2]) + j] = temp_buffer[j * MAX_COLOR + 2];
        }

        //padding

        for (int j = imgParaSet->imgWidth[0]; j < imgParaSet->imgWidth[0] + imgParaSet->imgWidthPad[0]; j++)
        {
            for(int color = 0; color < MAX_COLOR; color++)
            {
                imgBufset->imgBuf[color][i * (imgParaSet->imgWidth[0] +  imgParaSet->imgWidthPad[0]) + j] =
                    imgBufset->imgBuf[color][i * (imgParaSet->imgWidth[0] +  imgParaSet->imgWidthPad[0]) + j - 1];
            }
        }
    }

    free(temp_buffer);
    fclose (fin);
}



/**
* \brief Set the image parameters
*/

void setImgPara( strImgInfo    *imginfo,
                unsigned short Ywidth,
                unsigned short Yheight,
                unsigned short wavelet_level)
{
    imginfo->imgWidth[0] = imginfo->imgWidth[1] = imginfo->imgWidth[2] = Ywidth;
    imginfo->imgHeight[0] = imginfo->imgHeight[1] = imginfo->imgHeight[2] = Yheight;

    // check if padding is needed.

    imginfo->imgWidthPad[0]  = 0;
    imginfo->imgHeightPad[0] = 0;

    if (imginfo->imgWidth[0] &  ((1 << (wavelet_level + 1)) - 1))
    {
        imginfo->imgWidthPad[0] = (1 << (wavelet_level + 1)) - (imginfo->imgWidth[0] &  ((1 << (wavelet_level + 1)) - 1));
    }

    if (imginfo->imgHeight[0] &  ((1 << (wavelet_level )) - 1))
    {
        imginfo->imgHeightPad[0] = (1 << (wavelet_level )) - (imginfo->imgHeight[0] &  ((1 << (wavelet_level )) - 1));
    }

    imginfo->imgHeightPad[1] = imginfo->imgHeightPad[2] = imginfo->imgHeightPad[0];
    imginfo->imgWidthPad[1]  = imginfo->imgWidthPad[2]  = imginfo->imgWidthPad[0];

    for(int i = 0; i < MAX_COLOR; i++)
    {
        imginfo->imgSize[i] = (imginfo->imgWidth[i] + imginfo->imgWidthPad[i]) * (imginfo->imgHeight[i] + imginfo->imgHeightPad[i]);
    }

}

/**
* \brief Obtain the results from reference code and its performance number
*/
double getRefResults(strImgBufSet  *imgbuf,
                     strImgInfo    *imginfo,
                     short          wavelet_level)
{
#if !defined(ANDROID)
#include <windows.h>                // for Windows APis
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
#else
#include <sys/time.h>
    timeval t1, t2;
#endif

#if !defined(ANDROID)
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);
#else
    gettimeofday(&t1, NULL);
#endif

    reference_transform(wavelet_level,
        imginfo->imgWidth[0],
        imginfo->imgHeight[0],
        imgbuf->ref_image[0],
        imgbuf->ref_image[1],
        imgbuf->ref_image[2]);

#if !defined(ANDROID)
    QueryPerformanceCounter(&t2);
    double elapsedTime;
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
#else
    double elapsedTime;
    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
#endif

    return elapsedTime;
}

/**
* \brief verify the results and output to a file
*/
void outputResults( unsigned char *OpenCLResults[3],
                   int           *referResults[3],
                   strImgInfo    *imgdim,
                   FILE          *fp_out,
                   short          isVerificationEnabled)
{
    // finish the three components and write out the results to file.

    unsigned short ImgOrgWidth [MAX_COLOR];
    unsigned short ImgOrgHeight[MAX_COLOR];

    for(short i = 0; i < MAX_COLOR; i ++)
    {
        ImgOrgWidth [i] = imgdim->imgWidth [i] - imgdim->imgWidthPad[i];
        ImgOrgHeight[i] = imgdim->imgHeight[i] - imgdim->imgHeightPad[i];
    }

    unsigned char *temp_buffer = (unsigned char *) malloc (sizeof (unsigned char) * imgdim->imgWidth[0] * MAX_COLOR);

    if(temp_buffer == NULL)
    {
        LOGI("No sufficient memory (temp_buffer)!\n");
        exit(1);
    }

    unsigned char *ref_buffer;

    FILE *fRef;

    if(isVerificationEnabled)
    {
        fRef = fopen("referenceimg.ycbcr", "wb");
        ref_buffer = (unsigned char *) malloc (sizeof (unsigned char) * imgdim->imgWidth[0] * MAX_COLOR);

        if(ref_buffer == NULL)
        {
            LOGI("No sufficient memory (temp_buffer)!\n");
            exit(1);
        }

    }

    int max = -1, min = 1000;

    for (int i = 0; i < ImgOrgHeight[0]; i++)
    {
        int pixcounter = 0;

        int ref_pixcounter = 0;

        for (int j = 0; j < imgdim->imgWidth[0]; j++)
        {
            int pos = i * imgdim->imgWidth[0] + j;

            temp_buffer[pixcounter++] = OpenCLResults[0][pos];
            temp_buffer[pixcounter++] = OpenCLResults[1][pos];
            temp_buffer[pixcounter++] = OpenCLResults[2][pos];

            if(isVerificationEnabled)
            {

                ref_buffer[ref_pixcounter++] = (unsigned char)(referResults[0][pos]);
                ref_buffer[ref_pixcounter++] = (unsigned char)(referResults[1][pos]);
                ref_buffer[ref_pixcounter++] = (unsigned char)(referResults[2][pos]);

                //verify the results.
                int diff =(OpenCLResults[0][i * imgdim->imgWidth[0] + j]) -  clamp2Uchar(referResults[0][i * imgdim->imgWidth[0] + j]);

                if(diff != 0 )
                {
                    mismatches ++;
                }

                diff  = OpenCLResults[1][i * imgdim->imgWidth[0] + j] - clamp2Uchar(referResults[1][i * imgdim->imgWidth[0] + j]);  //?

                max = diff > max ? diff: max;
                min = diff < min ? diff: min;

                if ((diff != 0))
                {
                    mismatches ++;
                }

                diff = OpenCLResults[2][i * imgdim->imgWidth[0] + j] - clamp2Uchar(referResults[2][i * imgdim->imgWidth[0] + j]);

                max = diff > max ? diff: max;
                min = diff < min ? diff: min;
                if((diff != 0))
                {
                    mismatches ++;
                }
            }
        }
        fwrite (temp_buffer, sizeof (unsigned char), ImgOrgWidth[0] * MAX_COLOR, fp_out);

        if(isVerificationEnabled)
        {
            fwrite (ref_buffer, sizeof (unsigned char), ImgOrgWidth[0] * MAX_COLOR, fRef);
        }
    }

    free(temp_buffer);

    if(isVerificationEnabled)
    {
        free(ref_buffer);
        fclose(fRef);
    }

    fclose (fp_out);

    //print a summary of the settings.

    LOGI( "\n************************************ Verification *********************************\n");

    if(isVerificationEnabled) //verify the results.
    {
        if(mismatches == 0)
        {
            LOGI("\n\tCongratulations! no mismatches found (verification enabled)!\n", mismatches);
        }
        else
        {
            LOGI("\n%d mismatches found (verification enabled)!\n (Enable DEBUG macro in common.h to dump the intermediate results)\n", mismatches);
            LOGI( "The mismatches are not expected!\n (Enable DEBUG macro in common.h to dump the intermediate results)\n", mismatches);
        }
    }
    else
    {
        LOGI( "VERIFICATION is disabled!\n");
    }
    LOGI( "\n************************************     end     **********************************\n");
    return;
}


/**
* \brief main function to do the transform
* \param argc 6
* \param argv thisApplication inputImage outputImage Imagewidth Imageheight
*/
int main (int argc, char *argv[] )
{
    if (argc < 5 || argc > 7)
    {
        LOGI("usage: %s inputfile outputfile width height waveletLevel OpenCLDevice(GPU/CPU)\n", argv[0]);
        LOGI("For example: transformMain.exe test.ycbcr test_out.ycbcr 768 512 4 GPU\n", argv[0]);
        LOGI("waveletLevel: by default it is set to 4 (range from 2 to 4)\n");
        LOGI("OpenCLDevice: GPU or CPU (by default CPU is enabled)\n");
        exit(0);
    }

    /**************************************  check input/output file and transform profile file *******************/

    int argidx = 1;

    FILE *fp_in = fopen (argv[argidx++], "rb");

    if(!fp_in)
    {
        LOGI( "Cannot open the input file to read: %s\n", argv[1]);
        exit(1);
    }

    FILE *fp_out = fopen (argv[argidx], "wb");

    if(!fp_out)
    {
        LOGI( "Cannot open the file to write: %s\n", argv[2]);
        exit(1);
    }

    argidx++;

    /**************************************  ckeck image dimenstion *******************/

    unsigned tempwidth  = atoi (argv[argidx++]);
    unsigned tempheight = atoi (argv[argidx++]);

    if(tempwidth < 16 || tempheight < 16)
    {
        LOGI( "Image is too small for this applicaiton: %d x %d\n", tempwidth, tempheight);
        exit(1);
    }

    /**************************************  ckeck wavelet level  *******************/

    unsigned temp_wavelet_level;

    if(argc > 5)
    {
        if((temp_wavelet_level = atoi(argv[argidx++])) > MAX_LEVEL)
        {
            LOGI( "Invalid wavelet level: exceed MAX_LEVEL (%d)\n", MAX_LEVEL);
            exit(2);
        }
        else if(temp_wavelet_level < 2)
        {
            LOGI( "Invalid wavelet level: minimum level has to be 2\n");
            exit(2);
        }
    }
    else // set to default.
    {
        temp_wavelet_level = 4;
    }

    /**************************************  ckeck OpenCL device  *******************/

    bool OpenCL_GPU = true;

    if(argc > 6)
    {
        if((strcmp("GPU", argv[argidx]) == 0) || (strcmp("gpu", argv[argidx]) == 0))
        {
            OpenCL_GPU = true;
        }
        else if((strcmp("CPU", argv[argidx]) == 0) || (strcmp("cpu", argv[argidx]) == 0))
        {
            OpenCL_GPU = false;
        }
        else
        {
            LOGI( "Invalid CPU/GPU mode!\n The filtering mode must be CPU/GPU");
            exit(2);
        }
    }

    // set image parameters.

    strImgInfo *imgParaSet = (strImgInfo *)malloc(sizeof(strImgInfo));

    setImgPara(imgParaSet,  tempwidth, tempheight, temp_wavelet_level); // padding depends on wavelet level.

    strImgBufSet *imgBufSet= (strImgBufSet *)malloc(sizeof(strImgBufSet));

    readImg(imgBufSet, imgParaSet, fp_in, true);  //verification enabled

    double elapsedTimeref = getRefResults(imgBufSet, imgParaSet, temp_wavelet_level);

#if !defined(ANDROID)
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
#else
#include <sys/time.h>
    timeval t1, t2;
#endif

#if !defined(ANDROID)
    // get ticks per second
    QueryPerformanceFrequency(&frequency);
#endif

    init_test(temp_wavelet_level,
        imgParaSet->imgWidth[0],
        imgParaSet->imgHeight[0],
        OpenCL_GPU);

#if !defined(ANDROID)
    QueryPerformanceCounter(&t1);
#else
    gettimeofday(&t1, NULL);
#endif

    float totaltimefromAPI = 0;

    for(int Iterations = 0; Iterations < ITERATIONS; Iterations++)
    {
        float time4eachrun = 0;

        libTransform(imgBufSet->imgBuf[0], &time4eachrun);

        totaltimefromAPI +=  time4eachrun;
    }

#if !defined(ANDROID)
    QueryPerformanceCounter(&t2);
#else
    gettimeofday(&t2, NULL);
#endif

    double elapsedTimeOpenCL;

#if !defined(ANDROID)
    elapsedTimeOpenCL = ((t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart)/ITERATIONS;
#else
    elapsedTimeOpenCL = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTimeOpenCL += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
#endif

    // print the summary.
    LOGI( "\n**************************** OpenCL Wavelet transform Summary  ********************\n");

    LOGI( "Original Image: \t%s\n", argv[1]);

    LOGI( "Processed Image: \t%s\n", argv[2]);

    LOGI( "Image Format: \t\t%s\n", "444");

    LOGI( "Image Dimensions\t%dx%d\n", imgParaSet->imgWidth[0], imgParaSet->imgHeight[0]);

    if(imgParaSet->imgHeightPad[0] || imgParaSet->imgWidthPad[0])
    {
        LOGI( "Pixel padding (Y): \t[%d, %d]\n", imgParaSet->imgWidthPad[0], imgParaSet->imgHeightPad[0]);
        LOGI( "Pixel padding (UV): \t[%d, %d]\n", imgParaSet->imgWidthPad[1], imgParaSet->imgHeightPad[1]);
    }

    LOGI( "Wavelet level: \t\t%d\n", temp_wavelet_level);

    LOGI( "Transform Mode: \t%s\n", "YUV mode");

    if(OpenCL_GPU)
        LOGI( "OpenCL device: \t\t%s\n", "GPU");
    else
        LOGI( "OpenCL device: \t\t%s\n", "CPU");


    LOGI( "\n************************************ Performance **********************************\n");

    // warning:
    // For current QUALCOMM platform, use of CL_MEM_USE_HOST_PTR will incur a memory copy in driver in order to
    // map the host memory space to GPU memory space.
    // As a consequence, the performance collected by OpenCL API and system timer are different in this application.
    // The system timer contains the time used for memory copy.

    LOGI( "Time measured by the OpenCL API: \t\t%3.2f millisecs\n", totaltimefromAPI/ITERATIONS);

    LOGI( "Time measured by system time functions: \t%3.2f millisecs\n", elapsedTimeOpenCL);

    LOGI( "Time used by the reference system code: \t%3.2f millisecs\n", elapsedTimeref);

    //************************************************************************************************************//

    outputResults(imgBufSet->imgBuf, imgBufSet->ref_image, imgParaSet, fp_out, true);

    cleanupOpenCL();

    free(imgBufSet->imgBuf[0]);
    free(imgParaSet);
    free(imgBufSet);
}
/** \} */
