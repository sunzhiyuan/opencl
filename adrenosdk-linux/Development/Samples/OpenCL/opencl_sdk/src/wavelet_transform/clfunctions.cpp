/*======================================================================
*  FILE:
*      clfunctions.cpp
*  DESCRIPTION:
*      API functions and the functions doing OpenCL processing for wavelet
*      transform application in QCLBenchmark suite
*
*            Copyright (c) 2012 QUALCOMM Incorporated.
*                      All Rights Reserved.
*              QUALCOMM Confidential and Proprietary
*
*======================================================================*/

/**
* \addtogroup wavelettransform
* \{
* \file clfunctions.cpp
* \brief API functions and the functions doing OpenCL processing for wavelet
*        transform application in QCLBenchmark suite
*/
#include "cltransform.h"

using namespace std;

std::string sourceString;

cl_int ciErr[2];            // Error code var

strKernelInfo kernelInfoArray[NUM_OF_PROGRAMS] =
{       //program source        kernelName     kernelIdx,      Scheduler,    UseSource  buildoptions
    { "dwt_forward_x",      "fDWT_X",          enum_fDWT_X,    false,         true ,       " "   },
    { "dwt_forward_y",      "fDWT_Y",          enum_fDWT_Y,    false,         true ,       " "   },
    { "dwt_inverse_x",      "iDWT_X",          enum_iDWT_X,    false,         true ,       " "   },
    { "dwt_inverse_y",      "iDWT_Y",          enum_iDWT_Y,    false,        true ,       " "   },
    { "haar_forward",       "forward_haar",    enum_fHaar,     false,        true ,       " "   },
    { "haar_inverse",       "inverse_haar",    enum_iHaar,     false,         true ,       " "   },
};

strOpenCLInfo  *clInfo;

strOpenCLMem   memClSet;

strImgInfo *imgParaSet;

StrOpenCLEvents *clEventSet;

bool OpenCL_GPU = false; // to use CPU as OpenCL device by default.

short wavelet_level;

#if DEBUG
short *temp_imgbuf;

static int fCounter = 0;

FILE *fCLtemp;

#endif

/**
* \brief Obtain the OpenCL device information
*/
static void getCLDeviceInfo()
{
    cl_uint numOfPlatforms;

    ciErr[0] = clGetPlatformIDs( 0,
        NULL,
        &numOfPlatforms );

    ASSERT_ERR_CODE(ciErr[0]);

    if (0 == numOfPlatforms)
    {
        LOGI( "No platform is available\n Exiting ...");
        exit(1);
    }

    // 2nd step: query the platforms.

    cl_platform_id* platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * numOfPlatforms);

    ciErr[0] = clGetPlatformIDs(numOfPlatforms, platforms, NULL);

    ASSERT_ERR_CODE(ciErr[0]);

    if(NULL == platforms)
    {
        LOGI( "No platform found, Line %u in file %s !!!\n", __LINE__, __FILE__);
        exit(1);
    }

    clInfo->clPlatformId = 0;

    char platformName[100];

    for(unsigned i = 0; i < numOfPlatforms; i++)
    {
        ciErr[0] = clGetPlatformInfo(platforms[i], // platform ID
            CL_PLATFORM_VENDOR,     // property to query
            sizeof(platformName),    // sizeof the string to store the query information
            platformName,            // ptr to the string to store the query information
            NULL);

        ASSERT_ERR_CODE(ciErr[0]);

        if (!strcmp(platformName, "Advanced Micro Devices, Inc."))
        {
            clInfo->isQCOMplatform = false;
            clInfo->clPlatformId = platforms[i];
            break;
        }
        else if (!strcmp(platformName, "QUALCOMM"))
        {
            clInfo->isQCOMplatform = true;
            clInfo->clPlatformId = platforms[i];
            break;
        }
    }

    if(clInfo->clPlatformId)
    {
        LOGI("Platform found: %s\n", platformName);
    }
    else
    {
        LOGI("Desired platform not found\n Using default one.\n");
        clInfo->clPlatformId = platforms[0];
    }

    free(platforms);

    int numDevices;
    int numCPUDevices;
    int numGPUDevices;

    ciErr[0] = clGetDeviceIDs(clInfo->clPlatformId,
        CL_DEVICE_TYPE_ALL,
        0,
        0,
        (cl_uint*)&numDevices);

    ciErr[1] = clGetDeviceIDs(clInfo->clPlatformId,
        CL_DEVICE_TYPE_CPU,
        0,
        0,
        (cl_uint*)&numCPUDevices);

    ciErr[0] |= ciErr[1];

    ciErr[0] = clGetDeviceIDs(clInfo->clPlatformId,
        CL_DEVICE_TYPE_GPU,
        0,
        0,
        (cl_uint*)&numGPUDevices);

    ciErr[0] |= ciErr[1];

    if(0 == numDevices)
    {
        LOGI( "No device found! Line %u in file %s !!!\n", __LINE__, __FILE__);
        exit(1);
    }

    LOGI("devices on the platform: ALL: %d, CPU: %d, GPU, %d\n", numDevices, numCPUDevices, numGPUDevices);

    cl_device_type OpenCLDevice;

    if (OpenCL_GPU == true)
    {
        OpenCLDevice = CL_DEVICE_TYPE_GPU;
    }
    else
    {
        OpenCLDevice = CL_DEVICE_TYPE_CPU;
    }

    if(clGetDeviceIDs(clInfo->clPlatformId, OpenCLDevice, 1, &clInfo->clDevice, NULL) != CL_SUCCESS)
    {
        LOGI( "The OpenCL device is not available...\n");
        exit(0);
    }

    if((CL_DEVICE_TYPE_GPU == OpenCLDevice && numGPUDevices > 0))
    {
        LOGI( "Using the GPU device...\n");
    }
    else if((CL_DEVICE_TYPE_CPU == OpenCLDevice && numCPUDevices > 0))
    {
        LOGI( "Using the CPU device...\n");
    }
}

/**
* \brief Initialize the OpenCL platform. Query supported platform and device
*/
static void init_OpenCL()
{
    getCLDeviceInfo();

    clInfo->clContext = clCreateContext(0, 1, &clInfo->clDevice, NULL, NULL, &ciErr[0]);

    ASSERT_ERR_CODE(ciErr[0]);

    size_t profiling_resolution = 0;

    ASSERT_ERR_CODE(clGetDeviceInfo(clInfo->clDevice,
        CL_DEVICE_PROFILING_TIMER_RESOLUTION,
        sizeof(profiling_resolution),
        &profiling_resolution,
        NULL));

    clInfo->commandQ = clCreateCommandQueue(clInfo->clContext,
        clInfo->clDevice,
        CL_QUEUE_PROFILING_ENABLE,
        &ciErr[0]);

    clEventSet = (StrOpenCLEvents *)malloc(sizeof(StrOpenCLEvents));

    return;
}

/**
* \brief Create the OpenCL image/buffer objects
*/
static strOpenCLMem createCLbuf()
{
    strOpenCLMem memClSet;

    cl_context clContext = clInfo->clContext;

    unsigned short width = imgParaSet->imgWidth[0];

    unsigned short height = imgParaSet->imgHeight[0];

    unsigned int imgSize = width * height;

    for(int color = 0; color < 3; color ++)
    {
        memClSet.cl_haaroutput_DC[color] = clCreateBuffer(clContext, CL_MEM_READ_WRITE,
            (int)((width>>1) * (height>>1) * sizeof(short)), NULL, &ciErr[0]);

        ASSERT_ERR_CODE(ciErr[0]);

        memClSet.cl_haaroutput_AC[color] = clCreateBuffer(clContext, CL_MEM_READ_WRITE,
            (int)((width * height) * sizeof(short)), NULL, &ciErr[0]);

        ASSERT_ERR_CODE(ciErr[0]);

        for (int level = 1; level < wavelet_level; level++)
        {
            memClSet.dwt_img_DC[color][level - 1] =  clCreateBuffer(clContext, CL_MEM_READ_WRITE,
                (int)((width>>(level + 1)) * (height>>(level + 1))) * sizeof(short), NULL, &ciErr[0]);

            ASSERT_ERR_CODE(ciErr[0]);

            memClSet.dwt_img_AC[color][level - 1] =  clCreateBuffer(clContext, CL_MEM_READ_WRITE,
                4 * sizeof(short)*(width>>(level))*(height>>(level)), NULL, &ciErr[0]);

            ASSERT_ERR_CODE(ciErr[0]);
        }

        memClSet.tempImgMem4DWT[color] = clCreateBuffer(clContext, CL_MEM_READ_WRITE, sizeof(unsigned short) * imgSize, NULL, &ciErr[0]);

        ASSERT_ERR_CODE(ciErr[0]);
    }
    return memClSet;
}


/**
* \brief load the source string of the input file
* \param filename input source/binary file name
* \param finalLength source length
* \return source string
*/
static char *LoadProgSource(const char *cFilename,
                            size_t     *finalLength )
{
    // locals
    FILE* pFileStream = NULL;
    size_t sourceLength;

    if((pFileStream = fopen(cFilename, "rb")) == NULL)
    {
        return NULL;
    }

    fseek(pFileStream, 0, SEEK_END);
    sourceLength = ftell(pFileStream);
    fseek(pFileStream, 0, SEEK_SET);

    // allocate a buffer for the source code string and read it in
    char* cSourceString = (char *)malloc(sourceLength + 1);

    if(NULL == cSourceString)
    {
        fclose(pFileStream);
        return 0;
    }
    if (fread((cSourceString), sourceLength, 1, pFileStream) != 1)
    {
        fclose(pFileStream);
        free(cSourceString);
        return 0;
    }

    fclose(pFileStream);
    if(finalLength != 0)
    {
        *finalLength = sourceLength;
    }
    cSourceString[sourceLength] = '\0';

    return cSourceString;
}

/**
* \brief load the binary source file given the input filename
* \param filename input binary file name
* \return whether the funciton fails or passes
*/
static bool readBinaryFromFile( const char* fileName )
{
    FILE * input = NULL;
    size_t size = 0;
    char* binary = NULL;

    if((input = fopen(fileName, "rb")) == NULL)
    {
        return false;
    }

    fseek(input, 0L, SEEK_END);
    size = ftell(input);
    rewind(input);
    binary = (char*)malloc(size);
    if(binary == NULL)
    {
        fclose(input);
        return false;
    }
    fread(binary, sizeof(char), size, input);
    fclose(input);
    sourceString.assign(binary, size);

    return true;
}

/**
* \brief create a kernel based on the OpenCL program and collect work group size information
* \param currKernelStr kernel object
* \param Kernelname name of the kernel to be created
* \param Program4theKernel program that contains the kernel
* \param clDevice clDevice for this kernel
*/
static void BuildKernelForProgram( strKernelCompilationInfo *currKernelStr,
                                  const char               *Kernelname,
                                  cl_program                Program4theKernel,
                                  cl_device_id              clDevice )
{
#if DEBUG
    LOGI("clCreateKernel %s\n", Kernelname);
#endif

    currKernelStr->kernel = clCreateKernel(Program4theKernel, Kernelname, &ciErr[0]);

    ASSERT_ERR_CODE(ciErr[0]);

    ciErr[0] = clGetKernelWorkGroupInfo(currKernelStr->kernel,
        clDevice,
        CL_KERNEL_WORK_GROUP_SIZE,
        sizeof( currKernelStr->WGsize ),
        &(currKernelStr->WGsize), NULL );

    ASSERT_ERR_CODE(ciErr[0]);

    ciErr[0] = clGetKernelWorkGroupInfo(currKernelStr->kernel,
        clDevice,
        CL_KERNEL_LOCAL_MEM_SIZE,
        sizeof( currKernelStr->localMemory ),
        &(currKernelStr->localMemory), NULL );

    ASSERT_ERR_CODE(ciErr[0]);

    ciErr[0] = clGetKernelWorkGroupInfo(currKernelStr->kernel,
        clDevice,
        CL_KERNEL_COMPILE_WORK_GROUP_SIZE,
        3 * sizeof( size_t ),
        currKernelStr->compiler_WGsize, NULL );

    ASSERT_ERR_CODE(ciErr[0]);

#if DEBUG
    LOGI( "WGsize of kernel %s: %d, ", Kernelname, currKernelStr->WGsize);
#endif

    return;

}

/**
* \brief Prepare OpenCL kernels
*/

static void prepareKernels()
{
    char cPathAndName[100];      // var for full paths to data, src, etc.
    char *cSourceCL = NULL;         // Buffer to hold source for compilation
    char *cBinaryCL = NULL;         // Buffer to hold source for compilation

    // build the program
    std::string buildOpts;

    for (int i = 0; i < NUM_OF_PROGRAMS; i ++)
    {
        // Read the OpenCL kernel in from source file

        cl_program curClProgram;

        if(true == kernelInfoArray[i].isUseSource)
        {
            strcpy(cPathAndName, kernelInfoArray[i].programFileName);

            strcat(cPathAndName, ".cl");

            size_t szKernelLength;            // Byte size of kernel code

            if ((cSourceCL = LoadProgSource(cPathAndName,  &szKernelLength)) == NULL)
            {
                LOGI( "LoadProgSource error %s @ Line %u in file %s \n", cPathAndName,  __LINE__, __FILE__);
                exit(1);
            }
            // Create the program

            curClProgram = clCreateProgramWithSource(clInfo->clContext, 1, (const char **)&cSourceCL, &szKernelLength, &ciErr[0]);

            ASSERT_ERR_CODE(ciErr[0]);

            free(cSourceCL);
        }
        else
        {
            size_t szKernelLength;            // Byte size of kernel code

            strcpy(cPathAndName, kernelInfoArray[i].programFileName);

            strcat(cPathAndName, ".cl");

            readBinaryFromFile(cPathAndName);

            cBinaryCL = LoadProgSource(cPathAndName,   &szKernelLength);

            LOGI( "clCreateProgramWithBinary...\n");

            const char * binary = sourceString.c_str();

            size_t binarySize = sourceString.size();

            curClProgram = clCreateProgramWithBinary(clInfo->clContext, 1, &clInfo->clDevice, (const size_t*)&binarySize, (const unsigned char**)&binary, NULL, &ciErr[0]);

            ASSERT_ERR_CODE(ciErr[0] );

            free(cBinaryCL);
        }

        std::string BuildOptsSchduler = buildOpts;

#ifdef QCOM_SCHEDULER

        if(clInfo->isQCOMplatform)
        {
            if(kernelInfoArray[i].isSchedulerEnabled)
            {
                LOGI("Enabling QUALCOMM compiler early scheduler for kernel % s!\n", kernelInfoArray[i].programFileName);
                BuildOptsSchduler.append(" -qcom-scheduler ");
            }
        }
#endif

        BuildOptsSchduler.append(kernelInfoArray[i].programBuildOptions);

        cl_int clErr = (clBuildProgram(curClProgram, 0, NULL, BuildOptsSchduler.c_str(), NULL, NULL));

        if(clErr < 0) // kernel compilation error
        {
            //print kernel compilation error

            char programLog[1024];

            clErr = clGetProgramBuildInfo(curClProgram, clInfo->clDevice, CL_PROGRAM_BUILD_LOG, 1024, programLog, 0);

            std::cout<<programLog<<std::endl;

            ASSERT_ERR_CODE(clErr);
        }

        cl_build_status buildStatus;

        ASSERT_ERR_CODE(clGetProgramBuildInfo(curClProgram, clInfo->clDevice, CL_PROGRAM_BUILD_STATUS, sizeof(buildStatus), &buildStatus, NULL));

        if(buildStatus != CL_BUILD_SUCCESS)
        {
            LOGI( "The program %s is not built (buildStatus), Line %u in file %s !!!\n",  kernelInfoArray[i].programFileName, __LINE__, __FILE__);
            exit(1);
        }

        size_t    binarySize;

        clErr = clGetProgramInfo(curClProgram, CL_PROGRAM_BINARY_SIZES, sizeof( binarySize ), &binarySize, NULL);

        ASSERT_ERR_CODE(clErr);

        assert(binarySize);

        kernelInfoArray[i].compilerInfo.kernel = NULL;
        kernelInfoArray[i].compilerInfo.localMemory = NULL;
        kernelInfoArray[i].compilerInfo.WGsize = NULL;
        kernelInfoArray[i].compilerInfo.compiler_WGsize[0] = 0;
        kernelInfoArray[i].compilerInfo.compiler_WGsize[1] = 0;
        kernelInfoArray[i].compilerInfo.compiler_WGsize[2] = 0;

        BuildKernelForProgram(&(kernelInfoArray[i].compilerInfo), kernelInfoArray[i].kernelName, curClProgram, clInfo->clDevice);

        clReleaseProgram(curClProgram);

#ifdef OUTPUT_BINARY_KERNEL

        if(kernelInfoArray[i].isUseSource == true)
        {
            // Create a buffer and get the actual binary
            unsigned char *binary;
            binary = (unsigned char*)malloc(sizeof(unsigned char)*binarySize);
            unsigned char *buffers[ 1 ] = { binary };

            clErr = clGetProgramInfo(curClProgram, CL_PROGRAM_BINARIES, sizeof( buffers ), &buffers, NULL );

            ASSERT_ERR_CODE(clErr);

            FILE *pFile;

            pFile = fopen(strcat(kernelInfoArray[i].programFileName, ".o"), "wb");

            fwrite(buffers[0], sizeof(char), binarySize, pFile);
            fclose(pFile);
            free(binary);
        }
#endif
    }

    return;
}

/**
* \brief perform forward haar transform
* \param outImg_DC image object to store the DC results
* \param outBuf_AC buffer object to store the AC results
* \param inBuf input image object
* \param haar_width width of the transform region
* \param haar_height height of the transform region
* \param cl_cmd_q the command queue used for this kernel call
* \param curKernelInfoSet current kernel information
* \return current event
*/
static cl_event process_haar_forward_transform(cl_mem                    outImg_DC,
                                               cl_mem                    outBuf_AC,
                                               cl_mem                    inBuf,
                                               short                     haar_width,
                                               short                     haar_height,
                                               cl_command_queue          cl_cmd_q,
                                               strKernelCompilationInfo *curKernelInfoSet )
{
    size_t globalworksize[2] = {(haar_width >> 1),  (haar_height)};

    int stride = haar_width << 1;

    curKernelInfoSet->WGsize = curKernelInfoSet->WGsize  > (size_t) (haar_width >> 1)?
        (haar_width >> 1) : curKernelInfoSet->WGsize;

    while(globalworksize[0] % curKernelInfoSet->WGsize )
    {
        globalworksize[0]++;
    }

    size_t wgsize[2] = {curKernelInfoSet->WGsize, 1};

    cl_event currentEvent;

    cl_kernel curKernel = curKernelInfoSet->kernel;

    int kernelArgIdx = 0;

    ciErr[0]  = clSetKernelArg(curKernel, kernelArgIdx++, sizeof(cl_mem), (void*)&outImg_DC);
    ciErr[0] |= clSetKernelArg(curKernel, kernelArgIdx++, sizeof(cl_mem), (void*)&outBuf_AC);
    ciErr[0] |= clSetKernelArg(curKernel, kernelArgIdx++, sizeof(cl_mem), (void*)&inBuf);
    ciErr[0] |= clSetKernelArg(curKernel, kernelArgIdx++, sizeof(short),  (void*)&stride);
    ciErr[0] |= clSetKernelArg(curKernel, kernelArgIdx++, sizeof(short),  (void*)&haar_height);

    ASSERT_ERR_CODE (ciErr[0]);

#if 0
    LOGI( "Running %s, Workgroupsize: [%d, %d], globalworksize: [%d, %d]; \n", __FUNCTION__, wgsize[0], wgsize[1], globalworksize[0], globalworksize[1]);

    globalworksize[0] = globalworksize[1] = 1;
    wgsize[0] = wgsize[1] = 1;
    size_t wioffset[2] = {(haar_width >> 1) - 1,  (haar_height) - 1};
#endif

    ciErr[0] = clEnqueueNDRangeKernel(cl_cmd_q,
        curKernel,
        2,
        0, //wioffset, //
        globalworksize,
        wgsize,
        0,
        NULL,
        &currentEvent);

    ASSERT_ERR_CODE (ciErr[0]);

#if DEBUG

    size_t  fDWT_region[3] = {haar_width, haar_height, 1};

    size_t origin[3] = {0, 0, 0};

    ciErr[0] = clEnqueueReadBuffer(cl_cmd_q, outImg_DC, CL_TRUE, 0, haar_width * haar_height * sizeof(short), temp_imgbuf, 1,  &currentEvent, NULL);

    ciErr[0] = clEnqueueReadBuffer(cl_cmd_q, outBuf_AC, CL_TRUE, 0,   4 * haar_width * haar_height * sizeof(short),
        temp_imgbuf , 1, &currentEvent, NULL);

#endif

    return currentEvent;
}

/**
* \brief perform inverse haar transform
* \param output output image object
* \param inBuf input image object
* \param haar_width width of the transform region
* \param haar_height height of the transform region
* \param cl_cmd_q the command queue used for this kernel call
* \param numberofDependentEvents the number of events that this kernel call depends on
* \param dependentEventsList list of the events this kernel depends on
* \param curKernelInfoSet  current kernel information
* \return current event
*/
static cl_event process_haar_inverse_transform( cl_mem                   output,
                                               cl_mem                    inBuf,
                                               unsigned short            haar_width,
                                               unsigned short            haar_height,
                                               cl_command_queue          cl_cmd_q,
                                               size_t                    numberofDependentEvents,
                                               cl_event                 *dependentEventsList,
                                               strKernelCompilationInfo *curKernelInfoSet )
{
    cl_event eventHaar;

    size_t globalworksize[2] = {haar_width,  haar_height};

    size_t wgsize[2] = {curKernelInfoSet->WGsize, 1};

    while(globalworksize[0] % curKernelInfoSet->WGsize )
    {
        globalworksize[0]++;
    }

    int kernelArgIdx = 0;

    ciErr[0]  = clSetKernelArg(curKernelInfoSet->kernel, kernelArgIdx++, sizeof(cl_mem), (void*)&output);
    ciErr[0] |= clSetKernelArg(curKernelInfoSet->kernel, kernelArgIdx++, sizeof(cl_mem), (void*)&inBuf);
    ciErr[0] |= clSetKernelArg(curKernelInfoSet->kernel, kernelArgIdx++, sizeof(short), (void*)&haar_width);
    ciErr[0] |= clSetKernelArg(curKernelInfoSet->kernel, kernelArgIdx++, sizeof(short), (void*)&haar_height);

    ASSERT_ERR_CODE (ciErr[0]);

#if DEBUG
    LOGI( "Running %s, Workgroupsize: [%d, %d], globalworksize: [%d, %d]; \n", __FUNCTION__, wgsize[0], wgsize[1], globalworksize[0], globalworksize[1]);
#endif

    ciErr[0] = clEnqueueNDRangeKernel(
        cl_cmd_q,
        curKernelInfoSet->kernel,
        2,
        NULL,
        globalworksize,
        wgsize,
        numberofDependentEvents,
        dependentEventsList,
        &eventHaar);

    ASSERT_ERR_CODE (ciErr[0]);

#if DEBUG

    ciErr[0] = clEnqueueReadBuffer(cl_cmd_q, output, CL_TRUE, 0, sizeof(unsigned char) * haar_width * haar_height << 2,
        temp_imgbuf, 1,  &eventHaar, NULL);

    ASSERT_ERR_CODE (ciErr[0]);

#endif

    return eventHaar;
}

/**
* \brief perform forward wavelet transform, x direction first, followed by y direction.
* \param outImg_DC image object that stores DC output
* \param outImg_AC image object that stores AC output
* \param imgin input image object
* \param interMediaBuf an temporal image object that stores the results from X-direction transform results
* \param dwt_width width of the transform region
* \param dwt_height height of the transform region
* \param cl_cmd_q the command queue used for this kernel call
* \param numberofDependentEvents the number of events that this kernel call depends on
* \param dependentEventsList list of the events this kernel depends on
* \param dwtX_kernelInfoSet  information of the kernel that does x directional transform
* \param dwtY_kernelInfoSet  information of the kernel that does y directional transform
* \param event_1st  stores the event created for the first kernel call
* \return current event
*/
static cl_event process_fdwt_53(cl_mem                    outImg_DC,
                                cl_mem                    outBuf_AC,
                                cl_mem                    imgin,
                                cl_mem                    interMediaBuf,
                                int                       dwt_width,
                                int                       dwt_height,
                                cl_command_queue          cl_cmd_q,
                                int                       numofDependentEvent,
                                cl_event                 *dependentEventsList,
                                strKernelCompilationInfo *dwtX_kernelInfoSet,
                                strKernelCompilationInfo *dwtY_kernelInfoSet,
                                cl_event                 *event_1st )
{

    unsigned int width_half = (dwt_width >> 1);
    unsigned int height_half = (dwt_height >> 1);

    size_t globalworksize[2] = {width_half,  dwt_height};

    size_t wgsize[2] = {dwtX_kernelInfoSet->WGsize, 1};

    while (globalworksize[0] % wgsize[0])
    {
        globalworksize[0]++;
    }

    cl_event eventDWT[2];

    int  kernelArgIdx = 0;

    ciErr[0]  = clSetKernelArg(dwtX_kernelInfoSet->kernel, kernelArgIdx++, sizeof(cl_mem), (void*)&interMediaBuf);  //output
    ciErr[0] |= clSetKernelArg(dwtX_kernelInfoSet->kernel, kernelArgIdx++, sizeof(cl_mem), (void*)&imgin);//input
    ciErr[0] |= clSetKernelArg(dwtX_kernelInfoSet->kernel, kernelArgIdx++, sizeof(short), (void*)&width_half); //DWT_width[dwt_level]);            //width
    ciErr[0] |= clSetKernelArg(dwtX_kernelInfoSet->kernel, kernelArgIdx++, sizeof(short), (void*)&dwt_height); //DWT_width[dwt_level]);            //width

#if DEBUG
    LOGI( "Running %s, Workgroupsize: [%d, %d], globalworksize: [%d, %d]; \n", __FUNCTION__, wgsize[0], wgsize[1], globalworksize[0], globalworksize[1]);
#endif

    ciErr[1] = clEnqueueNDRangeKernel(cl_cmd_q,
        dwtX_kernelInfoSet->kernel,
        2,
        NULL,
        globalworksize,
        wgsize,
        numofDependentEvent,
        dependentEventsList,
        &eventDWT[0]);

    *event_1st = eventDWT[0];

    ASSERT_ERR_CODE(ciErr[1]);

#if DEBUG

    int dwt_ImgSize = dwt_width * dwt_height;

    ciErr[0] = clEnqueueReadBuffer(cl_cmd_q,
        interMediaBuf, CL_TRUE, 0, sizeof(short) * dwt_ImgSize,
        temp_imgbuf, 1, &eventDWT[0], NULL);

    ASSERT_ERR_CODE (ciErr[0]);

    clFinish(cl_cmd_q);
#endif

    // y direction.

    globalworksize[0] = width_half;
    globalworksize[1] = height_half ;

    wgsize[0] = 16;

    wgsize[1] = dwtY_kernelInfoSet->WGsize/wgsize[0];

    while( globalworksize[0] % wgsize[0])
    {
        globalworksize[0]++;
    }

    while( globalworksize[1] % wgsize[1])
    {
        globalworksize[1]++;
    }

#if DEBUG
    LOGI( "Running %s, Workgroupsize: [%d, %d], globalworksize: [%d, %d]; \n", __FUNCTION__, wgsize[0], wgsize[1], globalworksize[0], globalworksize[1]);
#endif

    kernelArgIdx = 0;

    ciErr[0]  = clSetKernelArg(dwtY_kernelInfoSet->kernel, kernelArgIdx++, sizeof(cl_mem), (void*)&outImg_DC);    //output
    ciErr[0] |= clSetKernelArg(dwtY_kernelInfoSet->kernel, kernelArgIdx++, sizeof(cl_mem), (void*)&outBuf_AC);    //output
    ciErr[0] |= clSetKernelArg(dwtY_kernelInfoSet->kernel, kernelArgIdx++, sizeof(cl_mem), (void*)&interMediaBuf); // input
    ciErr[0] |= clSetKernelArg(dwtY_kernelInfoSet->kernel, kernelArgIdx++, sizeof(short), (void*)&width_half);
    ciErr[0] |= clSetKernelArg(dwtY_kernelInfoSet->kernel, kernelArgIdx++, sizeof(short), (void*)&height_half);

    ASSERT_ERR_CODE (ciErr[0]);

    ciErr[0] = clEnqueueNDRangeKernel(cl_cmd_q,
        dwtY_kernelInfoSet->kernel,
        2,
        NULL,
        globalworksize,
        wgsize,
        1,
        &eventDWT[0],
        &eventDWT[1]);

    ASSERT_ERR_CODE (ciErr[0]);

#if DEBUG
    {
        size_t origin[3] = {0, 0, 0};

        size_t  fDWT_region[3] = {dwt_width >> 1 , dwt_height >> 1, 1};

        ciErr[0] = clEnqueueReadBuffer(cl_cmd_q, outImg_DC, CL_TRUE, 0, fDWT_region[0] * fDWT_region[1] * sizeof(short),
            temp_imgbuf, 1,  &eventDWT[1], NULL);


        ASSERT_ERR_CODE (ciErr[0]);

        ciErr[0] = clEnqueueReadBuffer(cl_cmd_q, outBuf_AC, CL_TRUE, 0, fDWT_region[0] * fDWT_region[1] * sizeof(short) * 4,
            temp_imgbuf, 1,  &eventDWT[1], NULL);

        ASSERT_ERR_CODE (ciErr[0]);
    }

#endif

    return eventDWT[1];

}

/**
* \brief perform inverse wavelet transform, y direction first, followed by x direction.
* \param bufout buffer object that stores the output
* \param bufin  input buffer object
* \param interMediaBuf an temporal image object that stores the results from y-direction transform results
* \param dwt_width width of the transform region
* \param dwt_height height of the transform region
* \param cl_cmd_q the command queue used for this kernel call
* \param numberofDependentEvents the number of events that this kernel call depends on
* \param dependentEventsList list of the events this kernel depends on
* \param idwtX_kernelInfoSet  information of the kernel that does x directional transform
* \param idwtY_kernelInfoSet  information of the kernel that does y directional transform
* \param event_1st  stores the event created for the first kernel call
* \return current event
*/
static cl_event process_idwt_53(cl_mem                    bufout,
                                cl_mem                    bufin,
                                cl_mem                    interMediaBuf,
                                unsigned short            dwt_width,
                                unsigned short            dwt_height,
                                cl_command_queue          cl_cmd_q,
                                short                     numberofDependentEvents,
                                cl_event                 *dependentEventsList,
                                strKernelCompilationInfo *idwtX_kernelInfoSet,
                                strKernelCompilationInfo *idwtY_kernelInfoSet,
                                cl_event                 *event_1st )
{
    cl_event event_iDWT[2];

    size_t origin[3]={0, 0, 0}, region[3]={dwt_width, dwt_height, 1};

    size_t globalworksize[2] =  {dwt_width,  dwt_height};

    cl_kernel currKernel = idwtY_kernelInfoSet->kernel;

    unsigned wgsize[2];

    wgsize[0] = 32;

    wgsize[1] = 4;

    while(wgsize[1] * wgsize[0] > idwtY_kernelInfoSet->WGsize)
    {
        wgsize[1]--;
    }

    assert(wgsize[1]);

    while( globalworksize[0] %  wgsize[0])
    {
        globalworksize[0]++;
    }

    while( globalworksize[1] %  wgsize[1])
    {
        globalworksize[1]++;
    }

    /*********************  Y direction   IDWT *************************/

    int kernelArgIdx = 0;

    ciErr[0]  = clSetKernelArg(currKernel, kernelArgIdx++, sizeof(cl_mem), (void*)&interMediaBuf);
    ciErr[0] |= clSetKernelArg(currKernel, kernelArgIdx++, sizeof(cl_mem), (void*)&bufin);
    ciErr[0] |= clSetKernelArg(currKernel, kernelArgIdx++, sizeof(short), (void*)&dwt_width);
    ciErr[0] |= clSetKernelArg(currKernel, kernelArgIdx++, sizeof(short), (void*)&dwt_height);

    ASSERT_ERR_CODE (ciErr[0]);

#if DEBUG
    LOGI( "Running DWT_Y in %s, Workgroupsize: [%d, %d], globalworksize: [%d, %d];  \n",
        __FUNCTION__, wgsize[0], wgsize[1], globalworksize[0], globalworksize[1]);
#endif

    ciErr[0] = clEnqueueNDRangeKernel(cl_cmd_q,
        currKernel,
        2,
        0,
        globalworksize,
        wgsize,
        numberofDependentEvents,
        dependentEventsList,
        &event_iDWT[0]);

    ASSERT_ERR_CODE (ciErr[0]);

    *event_1st = event_iDWT[0];

#if DEBUG
    {
        ciErr[0] = clEnqueueReadBuffer((cl_cmd_q), (interMediaBuf), CL_TRUE, 0, 4*sizeof(short)*(dwt_width * dwt_height),
            (temp_imgbuf), 1, &(event_iDWT[0]), NULL);

        ASSERT_ERR_CODE (ciErr[0]);

    }
#endif

    /*********************  X direction   IDWT *************************/

    currKernel = idwtX_kernelInfoSet->kernel;

    wgsize[0] = idwtX_kernelInfoSet->WGsize;

    wgsize[1] = 1;

    while(globalworksize[0] % wgsize[0])
    {
        globalworksize[0]++;
    }

    dwt_width >>=1;

    kernelArgIdx = 0;

    ciErr[0]  = clSetKernelArg(currKernel, kernelArgIdx++, sizeof(cl_mem), (void*)&bufout );
    ciErr[0] |= clSetKernelArg(currKernel, kernelArgIdx++, sizeof(cl_mem), (void*)&interMediaBuf);
    ciErr[0] |= clSetKernelArg(currKernel, kernelArgIdx++, sizeof(short),  (void*)&dwt_width);
    ciErr[0] |= clSetKernelArg(currKernel, kernelArgIdx++, sizeof(short),  (void*)&dwt_height);

    ASSERT_ERR_CODE (ciErr[0]);

#if DEBUG
    LOGI( "Running DWT_X in %s, Workgroupsize: [%d, %d], globalworksize: [%d, %d]; \n", __FUNCTION__, wgsize[0], wgsize[1], globalworksize[0], globalworksize[1]);
#endif

    ciErr[0] = clEnqueueNDRangeKernel(cl_cmd_q,
        currKernel,
        2,
        NULL,
        globalworksize,
        wgsize,
        1,
        &event_iDWT[0],
        &event_iDWT[1]);

    ASSERT_ERR_CODE (ciErr[0]);

#if DEBUG
    {
        ciErr[0] = clEnqueueReadBuffer((cl_cmd_q), (bufout), CL_TRUE, 0, 16*sizeof(short)*((dwt_width <<1) * dwt_height),
            (temp_imgbuf), 1, &(event_iDWT[1]), NULL);

        ASSERT_ERR_CODE (ciErr[0]);
    }
#endif

    return event_iDWT[1];

}


/**
* \brief Core function to do the transform
* \param imgBuf pointer to input image
* \param profilingTime execution time obtained from OpenCL API
* \param imgParaSet the parameter set of the input image
* \param memClSet the OpenCL memory objects used for transform
* \param clInfo the OpenCL platform/device/context information
*/
static bool coreWaveletTransform(unsigned char *imgBuf,
                                 float         *profilingTime,
                                 strImgInfo    *imgParaSet,
                                 strOpenCLMem   memClSet,
                                 strOpenCLInfo *clInfo )
{
    cl_ulong startTime = 0, endTime = 0;

    clEventSet->EventCounter = 0;

    cl_mem LLband[MAX_COLOR];
    cl_event dependentEvent_fdwt[MAX_COLOR];

    // haar transform

    for (short color = 0; color < MAX_COLOR; color++)
    {
        clEventSet->event_haar_forward[color] = process_haar_forward_transform(memClSet.cl_haaroutput_DC[color],
            memClSet.cl_haaroutput_AC[color], memClSet.cl_input[color],
            (imgParaSet->imgWidth[color] >> 1), (imgParaSet->imgHeight[color] >> 1), clInfo->commandQ,
            &kernelInfoArray[enum_fHaar].compilerInfo);

        clEventSet->EventList[clEventSet->EventCounter++] = clEventSet->event_haar_forward[color];

        LLband[color] = memClSet.cl_haaroutput_DC[color];
        dependentEvent_fdwt[color] = clEventSet->event_haar_forward[color];
    }

    // forward mult-layer 5/3 wavelet transform

    for(short dwt_level = 1; dwt_level < wavelet_level; dwt_level++)
    {
        for (short color = 0; color < MAX_COLOR; color++)
        {
            clEventSet->event_fDWT[color][dwt_level-1] = process_fdwt_53(memClSet.dwt_img_DC[color][dwt_level - 1],
                memClSet.dwt_img_AC[color][dwt_level - 1],
                LLband[color],
                memClSet.tempImgMem4DWT[color],
                (imgParaSet->imgWidth[color] >> (dwt_level)),
                (imgParaSet->imgHeight[color] >> (dwt_level)),
                clInfo->commandQ,
                1,
                &dependentEvent_fdwt[color],
                &kernelInfoArray[enum_fDWT_X].compilerInfo,
                &kernelInfoArray[enum_fDWT_Y].compilerInfo,
                &clEventSet->EventList[clEventSet->EventCounter++]);

            clEventSet->EventList[clEventSet->EventCounter++] =  clEventSet->event_fDWT[color][dwt_level-1];

            LLband[color] = memClSet.dwt_img_DC[color][dwt_level - 1];
        }
    }

    // Set dependent events for inverse transform
    for (int color = 0;  color < MAX_COLOR; color++)
    {
        clEventSet->depedent_event[color] = clEventSet->event_fDWT[color][wavelet_level-2];
    }


    // perform the 5/3 inverse transform
    for(unsigned int dwt_level = wavelet_level; dwt_level > 1; dwt_level--)
    {
        for (int color = 0;  color < MAX_COLOR; color++)
        {
            cl_mem input = memClSet.dwt_img_AC[color][dwt_level - 2];
            cl_mem output;

            if(dwt_level != 2)
            {
               output = memClSet.dwt_img_AC[color][dwt_level - 3];
            }
            else
            {
               output = memClSet.cl_haaroutput_AC[color];
            }

            clEventSet->depedent_event[color] = process_idwt_53(output,             // output to the idwt. buf object.
                input,
                memClSet.tempImgMem4DWT[color],
                (unsigned short)(imgParaSet->imgWidth[color] >> (dwt_level)),
                (unsigned short)(imgParaSet->imgHeight[color] >> (dwt_level)),
                clInfo->commandQ,
                1,
                &clEventSet->depedent_event[color],
                &kernelInfoArray[enum_iDWT_X].compilerInfo,
                &kernelInfoArray[enum_iDWT_Y].compilerInfo,
                &clEventSet->EventList[clEventSet->EventCounter++]);

            clEventSet->EventList[clEventSet->EventCounter++] = clEventSet->depedent_event[color];
        }
    }

    // final step: inverse haar transform.

    for(int color = 0; color < MAX_COLOR; color++)
    {
        clEventSet->event_haar_inverse[color] = process_haar_inverse_transform(memClSet.cl_input[color],
            memClSet.cl_haaroutput_AC[color],
            (unsigned short)(imgParaSet->imgWidth[color] >> (1)),
            (unsigned short)(imgParaSet->imgHeight[color] >> (1)),
            clInfo->commandQ,
            1, //# of dependent events
            &clEventSet->depedent_event[color],
            &kernelInfoArray[enum_iHaar].compilerInfo);

        clEventSet->EventList[clEventSet->EventCounter++] = clEventSet->event_haar_inverse[color];
    }

    for(int color = 0; color < MAX_COLOR; color++)
    {
        imgBuf = (unsigned char *) clEnqueueMapBuffer(clInfo->commandQ, memClSet.cl_input[color], CL_TRUE,
            CL_MAP_READ, 0, sizeof(unsigned char) * imgParaSet->imgWidth[color] * imgParaSet->imgHeight[color],
            1, &(clEventSet->event_haar_inverse[color]),
            NULL, &ciErr[0]);

        ASSERT_ERR_CODE (ciErr[0]);
    }

    // use event and OpenCL profiling API to collect the performance numbers
    cl_event firstEventIdx;

    firstEventIdx = clEventSet->event_haar_forward[0];

    clWaitForEvents(1, &firstEventIdx);

    clGetEventProfilingInfo(firstEventIdx, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &startTime, NULL);

    cl_event lastEventIdx = clEventSet->event_haar_inverse[2];

    clWaitForEvents(1, &lastEventIdx);

    clGetEventProfilingInfo(lastEventIdx, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &endTime, NULL);

    for(int i = 0; i < clEventSet->EventCounter; i ++)
    {
        clReleaseEvent(clEventSet->EventList[i]);
    }

    clEventSet->EventCounter = 0; // reset the event counter.

    *profilingTime = (float)(1.0e-6 * ((double)(endTime - startTime)));

    return true;
}

/**
* \brief Initialize the transform
* \param mainWavelet_level wavelet level, supports from 2 to 5
* \param imgWidth image width
* \param imgHeight image height
* \param isUsingOpenCL_GPU using CPU OpenCL or GPU OpenCL
*/
void init_test(short          mainWavelet_level,
               unsigned short imgWidth,
               unsigned short imgHeight,
               bool isUsingOpenCL_GPU)
{
    clInfo = (strOpenCLInfo *)malloc(sizeof(strOpenCLInfo));

    imgParaSet = (strImgInfo *)malloc(sizeof(strImgInfo));

    imgParaSet->imgWidth[0] = imgWidth;

    imgParaSet->imgHeight[0] = imgHeight;

    imgParaSet->imgSize[0] = imgWidth * imgHeight;

    wavelet_level = mainWavelet_level;

    OpenCL_GPU = isUsingOpenCL_GPU;

    imgParaSet->imgWidth[1] = imgParaSet->imgWidth[2]  = imgParaSet->imgWidth[0];

    imgParaSet->imgHeight[1] = imgParaSet->imgHeight[2] = imgParaSet->imgHeight[0];

    imgParaSet->imgSize[1] = imgParaSet->imgSize[2] = imgParaSet->imgSize[0];

#if DEBUG
    LOGI("Allocate and Init Host Mem...\n");
    temp_imgbuf = (short*)malloc(int(sizeof(short) * imgParaSet->imgSize[0] * 3));
#endif

    init_OpenCL();

    prepareKernels();

    memClSet = createCLbuf();

}

/**
* \brief API for transform. transformd image data is stored back to imgbuf
* \param imgbuf pointer to the image buffer
*/
void libTransform( unsigned char *imgbuf,  float *profilingtimefromAPI)
{
    *profilingtimefromAPI = 0;

    for(int i = 0; i < 3; i++)
    {
        // warning:
        // For current QUALCOMM platform, CL_MEM_USE_HOST_PTR will incur a memory copy in order to
        // map the host memory space to GPU memory space.
        // So the performance collected by OpenCL API and system timer are different, as the system timer record the
        // time used for memory copy.


        memClSet.cl_input[i]  = clCreateBuffer(clInfo->clContext, CL_MEM_READ_WRITE| CL_MEM_USE_HOST_PTR,
            sizeof(unsigned char) * imgParaSet->imgSize[i], imgbuf + i * imgParaSet->imgSize[i], &ciErr[0]);

        ASSERT_ERR_CODE (ciErr[0]);
    }

    coreWaveletTransform(imgbuf,
        profilingtimefromAPI,
        imgParaSet,
        memClSet,
        clInfo);
}

/**
* \brief Release the allocated OpenCL memory, object, contexts, and other resources
*/
void cleanupOpenCL()
{
    LOGI( "Starting to clean up...\n"   );

    for (int i = 0; i < NUM_OF_PROGRAMS; i ++)
    {
        if(kernelInfoArray[i].compilerInfo.kernel)
        {
            clReleaseKernel(kernelInfoArray[i].compilerInfo.kernel);
        }
    }

    clReleaseContext(clInfo->clContext);
    clReleaseCommandQueue(clInfo->commandQ);

    for ( int i = 0; i < MAX_COLOR; i ++)
    {
        clReleaseMemObject(memClSet.cl_input[i]);

        if(memClSet.tempImgMem4DWT[i])
            clReleaseMemObject(memClSet.tempImgMem4DWT[i]);

        if(memClSet.cl_haaroutput_AC[i])
            clReleaseMemObject(memClSet.cl_haaroutput_AC[i]);

        if(memClSet.cl_haaroutput_DC[i])
            clReleaseMemObject(memClSet.cl_haaroutput_DC[i]);

        for (int level = 0; level < wavelet_level-1; level++)
        {
            if(memClSet.dwt_img_DC[i][level])
                clReleaseMemObject(memClSet.dwt_img_DC[i][level]);
            if(memClSet.dwt_img_AC[i][level])
                clReleaseMemObject(memClSet.dwt_img_AC[i][level]);

        }
    }

    LOGI( "Cleaned up successfully\n"   );

    free(clInfo);

    free(clEventSet);

    free(imgParaSet);

#if DEBUG
    free(temp_imgbuf);
#endif
}
/** \} */
