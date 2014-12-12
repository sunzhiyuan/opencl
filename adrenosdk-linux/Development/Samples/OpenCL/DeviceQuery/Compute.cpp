//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Incorporated.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include "Compute.h"

//--------------------------------------------------------------------------------------
//  Macros
//--------------------------------------------------------------------------------------
#define ALIGN_SPACES    (40)
#define ALIGNED_LEN(s)  (FrmMax( strlen(s), ALIGN_SPACES ))

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeApplication* FrmCreateComputeApplicationInstance()
{
    return new CSample( "DeviceQuery" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeApplication( strName )
{
}

//--------------------------------------------------------------------------------------
// Name: ShowPlatformInfo()
// Desc: Display platform information
//--------------------------------------------------------------------------------------
BOOL CSample::ShowPlatformInfo( cl_platform_info platformInfo, const CHAR* infoName )
{
    cl_int errNum;
    size_t platformInfoSize = 0;

    errNum = clGetPlatformInfo( m_platform, platformInfo, 0, NULL, &platformInfoSize );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to get platform info size" );
        return FALSE;
    }

    CHAR* infoStr = new CHAR[ platformInfoSize ];
    errNum = clGetPlatformInfo( m_platform, platformInfo, platformInfoSize, infoStr, NULL );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to get platform info" );
        delete [] infoStr;
        return FALSE;
    }


    CHAR* strMessage = new CHAR[ platformInfoSize + ALIGNED_LEN(infoName) + 4 ];
    FrmSprintf ( strMessage, platformInfoSize + ALIGNED_LEN(infoName) + 4, "%*s: %s\n", ALIGN_SPACES, infoName, infoStr );
    FrmLogMessage ( strMessage );
    delete [] infoStr;
    delete [] strMessage;
    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: ShowDeviceInfo()
// Desc: Display char[] device information
//--------------------------------------------------------------------------------------
BOOL CSample::ShowDeviceInfoStr( cl_device_id deviceId, cl_device_info deviceInfo, const CHAR *infoName )
{
    cl_int errNum;
    size_t deviceInfoSize = 0;

    errNum = clGetDeviceInfo( deviceId, deviceInfo, 0, NULL, &deviceInfoSize );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to get device info size" );
        return FALSE;
    }

    CHAR* infoStr = new CHAR[ deviceInfoSize ];
    errNum = clGetDeviceInfo( deviceId, deviceInfo, deviceInfoSize, infoStr, NULL );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to get device info" );
        delete [] infoStr;
        return FALSE;
    }

    CHAR* strMessage = new CHAR[ deviceInfoSize + ALIGNED_LEN(infoName) + 4 ];
    FrmSprintf ( strMessage, deviceInfoSize + ALIGNED_LEN(infoName) + 4, "%*s: %s\n", ALIGN_SPACES, infoName, infoStr );
    FrmLogMessage ( strMessage );
    delete [] infoStr;
    delete [] strMessage;
    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: GetDeviceInfo()
// Desc: Get device information for non-string values
//--------------------------------------------------------------------------------------
BOOL CSample::GetDeviceInfo( cl_device_id deviceId, cl_device_info deviceInfo, VOID* infoResult, size_t infoResultSize )
{
    cl_int errNum;
    size_t deviceInfoSize = 0;

    errNum = clGetDeviceInfo( deviceId, deviceInfo, 0, NULL, &deviceInfoSize );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to get device info size" );
        return FALSE;
    }

    if ( deviceInfoSize > infoResultSize )
    {
        FrmLogMessage( "Result buffer not large enough" );
        return FALSE;
    }

    errNum = clGetDeviceInfo( deviceId, deviceInfo, deviceInfoSize, infoResult, NULL );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to get device info" );
        return FALSE;
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: ShowBitfield()
// Desc: Show cl_bitfield value as string
//--------------------------------------------------------------------------------------
VOID CSample::ShowBitfield( const CHAR* infoName, cl_bitfield bitfield, BitfieldToString *bitfieldToString, INT32 count )
{
    CHAR bitfieldStr[1024];
    bitfieldStr[0] = '\0';

    BOOL firstShown = FALSE;
    for (INT32 i = 0; i < count; i++)
    {
        if ( bitfield & bitfieldToString[i].value )
        {
            if ( firstShown )
            {
                FrmStrcat( bitfieldStr, " | " );
            }
            FrmStrcat( bitfieldStr, bitfieldToString[i].str );
            firstShown = TRUE;
        }
    }

    CHAR strMessage[ 1024 ];
    FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %s\n", ALIGN_SPACES, infoName, bitfieldStr );
    FrmLogMessage ( strMessage );
}

//--------------------------------------------------------------------------------------
// Name: ShowUInt()
// Desc: Show cl_uint value as string
//--------------------------------------------------------------------------------------
VOID CSample::ShowUInt( const CHAR* infoName, cl_uint uintValue, UIntToString *uintToString, INT32 count )
{
    CHAR uintStr[1024];
    uintStr[0] = '\0';

    BOOL firstShown = FALSE;
    for (INT32 i = 0; i < count; i++)
    {
        if (uintValue == uintToString[i].value )
        {
            FrmStrcat( uintStr, uintToString[i].str );
        }
    }

    CHAR strMessage[ 1024 ];
    FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %s\n", ALIGN_SPACES, infoName, uintStr );
    FrmLogMessage ( strMessage );
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    if(!FrmOpenConsole())
        return FALSE;

    return TRUE;
}




//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
}



//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    BOOL result = TRUE;

    // Output platform information
    result &= ShowPlatformInfo( CL_PLATFORM_PROFILE, "CL_PLATFORM_PROFILE" );
    result &= ShowPlatformInfo( CL_PLATFORM_VERSION, "CL_PLATFORM_VERSION" );
    result &= ShowPlatformInfo( CL_PLATFORM_VENDOR, "CL_PLATFORM_VENDOR" );
    result &= ShowPlatformInfo( CL_PLATFORM_EXTENSIONS, "CL_PLATFORM_EXTENSIONS" );

    // Show all devices
    for ( INT32 device = 0; device < m_deviceCount; device++ )
    {
        cl_device_id deviceId = m_devices[device];

        FrmLogMessage( "\n" );
        result &= ShowDeviceInfoStr( deviceId, CL_DEVICE_NAME, "CL_DEVICE_NAME" );
        result &= ShowDeviceInfoStr( deviceId, CL_DEVICE_PROFILE, "CL_DEVICE_PROFILE" );
        result &= ShowDeviceInfoStr( deviceId, CL_DEVICE_VENDOR, "CL_DEVICE_VENDOR" );
        result &= ShowDeviceInfoStr( deviceId, CL_DEVICE_VERSION, "CL_DEVICE_VERSION" );
        result &= ShowDeviceInfoStr( deviceId, CL_DRIVER_VERSION, "CL_DRIVER_VERSION");
        result &= ShowDeviceInfoStr( deviceId, CL_DEVICE_EXTENSIONS, "CL_DEVICE_EXTENSIONS" );

        CHAR strMessage[ 1024 ];
        cl_uint uintResult = 0;
        cl_ulong ulongResult = 0;
        cl_bool boolResult = 0;
        cl_bitfield bitfieldResult = 0;
        size_t sizetResult = 0;

        result &= GetDeviceInfo( deviceId, CL_DEVICE_ADDRESS_BITS, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_ADDRESS_BITS", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_AVAILABLE, (VOID*)&boolResult, sizeof(boolResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %s\n", ALIGN_SPACES, "CL_DEVICE_AVAILABLE", boolResult ? "CL_TRUE" : "CL_FALSE" );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_COMPILER_AVAILABLE, (VOID*)&boolResult, sizeof(boolResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %s\n", ALIGN_SPACES, "CL_DEVICE_COMPILER_AVAILABLE", boolResult ? "CL_TRUE" : "CL_FALSE" );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_ENDIAN_LITTLE, (VOID*)&boolResult, sizeof(boolResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %s\n", ALIGN_SPACES, "CL_DEVICE_ENDIAN_LITTLE", boolResult ? "CL_TRUE" : "CL_FALSE" );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_ERROR_CORRECTION_SUPPORT, (VOID*)&boolResult, sizeof(boolResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %s\n", ALIGN_SPACES, "CL_DEVICE_ERROR_CORRECTION_SUPPORT", boolResult ? "CL_TRUE" : "CL_FALSE" );
        FrmLogMessage ( strMessage );

        BitfieldToString executionCaps[] =
        {
            { CL_EXEC_KERNEL,        "CL_EXEC_KERNEL" },
            { CL_EXEC_NATIVE_KERNEL, "CL_EXEC_NATIVE_KERNEL" }
        };
        result &= GetDeviceInfo( deviceId, CL_DEVICE_EXECUTION_CAPABILITIES, (VOID*)&bitfieldResult, sizeof(bitfieldResult) );
        ShowBitfield( "CL_DEVICE_EXECUTION_CAPABILITIES", bitfieldResult, executionCaps, sizeof(executionCaps) / sizeof(BitfieldToString) );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, (VOID*)&ulongResult, sizeof(ulongResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %llu\n", ALIGN_SPACES, "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE", ulongResult );
        FrmLogMessage ( strMessage );

        UIntToString memCacheType[] =
        {
            { CL_NONE,              "CL_NONE" },
            { CL_READ_ONLY_CACHE,   "CL_READ_ONLY_CACHE" },
            { CL_READ_WRITE_CACHE,  "CL_READ_WRITE_CACHE" }
        };
        result &= GetDeviceInfo( deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, (VOID*)&uintResult, sizeof(uintResult) );
        ShowUInt( "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE", uintResult, memCacheType, sizeof(memCacheType) / sizeof(UIntToString) );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, (VOID*)&ulongResult, sizeof(ulongResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %llu\n", ALIGN_SPACES, "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE", ulongResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_GLOBAL_MEM_SIZE, (VOID*)&ulongResult, sizeof(ulongResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %llu\n", ALIGN_SPACES, "CL_DEVICE_GLOBAL_MEM_SIZE", ulongResult );
        FrmLogMessage ( strMessage );

        // CL_DEVICE_HALF_FP_CONFIG
        BitfieldToString fpConfig[] =
        {
            { CL_FP_DENORM,             "CL_FP_DENORM" },
            { CL_FP_INF_NAN,            "CL_FP_INF_NAN" },
            { CL_FP_ROUND_TO_NEAREST,   "CL_FP_ROUND_TO_NEAREST" },
            { CL_FP_ROUND_TO_ZERO,      "CL_FP_ROUND_TO_ZERO" },
            { CL_FP_ROUND_TO_INF,       "CL_FP_ROUND_TO_INF" },
            { CL_FP_FMA,                "CL_FP_FMA" },
        };
        //GetDeviceInfo( deviceId, CL_DEVICE_HALF_FP_CONFIG, (VOID*)&bitfieldResult, sizeof(bitfieldResult) );
        //ShowBitfield( "CL_DEVICE_HALF_FP_CONFIG", bitfieldResult, fpConfig, sizeof(fpConfig) / sizeof(BitfieldToString) );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_IMAGE_SUPPORT, (VOID*)&boolResult, sizeof(boolResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %s\n", ALIGN_SPACES, "CL_DEVICE_IMAGE_SUPPORT", boolResult ? "CL_TRUE" : "CL_FALSE" );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_IMAGE2D_MAX_HEIGHT, (VOID*)&sizetResult, sizeof(sizetResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_IMAGE2D_MAX_HEIGHT", sizetResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_IMAGE2D_MAX_WIDTH, (VOID*)&sizetResult, sizeof(sizetResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_IMAGE2D_MAX_WIDTH", sizetResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_IMAGE3D_MAX_DEPTH, (VOID*)&sizetResult, sizeof(sizetResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_IMAGE3D_MAX_DEPTH", sizetResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_IMAGE3D_MAX_HEIGHT, (VOID*)&sizetResult, sizeof(sizetResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_IMAGE3D_MAX_HEIGHT", sizetResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_IMAGE3D_MAX_WIDTH, (VOID*)&sizetResult, sizeof(sizetResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_IMAGE3D_MAX_WIDTH", sizetResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_LOCAL_MEM_SIZE, (VOID*)&ulongResult, sizeof(ulongResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %llu\n", ALIGN_SPACES, "CL_DEVICE_LOCAL_MEM_SIZE", ulongResult );
        FrmLogMessage ( strMessage );

        UIntToString memType[] =
        {
            { CL_LOCAL,     "CL_LOCAL" },
            { CL_GLOBAL,    "CL_GLOBAL" },
        };
        result &= GetDeviceInfo( deviceId, CL_DEVICE_LOCAL_MEM_TYPE, (VOID*)&uintResult, sizeof(uintResult) );
        ShowUInt( "CL_DEVICE_LOCAL_MEM_TYPE", uintResult, memType, sizeof(memType) / sizeof(UIntToString) );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MAX_CLOCK_FREQUENCY", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_COMPUTE_UNITS, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MAX_COMPUTE_UNITS", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_CONSTANT_ARGS, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MAX_CONSTANT_ARGS", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, (VOID*)&ulongResult, sizeof(ulongResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %llu\n", ALIGN_SPACES, "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE", ulongResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE, (VOID*)&ulongResult, sizeof(ulongResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %llu\n", ALIGN_SPACES, "CL_DEVICE_MAX_MEM_ALLOC_SIZE", ulongResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_PARAMETER_SIZE, (VOID*)&sizetResult, sizeof(sizetResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MAX_PARAMETER_SIZE", sizetResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_READ_IMAGE_ARGS, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MAX_READ_IMAGE_ARGS", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_SAMPLERS, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MAX_SAMPLERS", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, (VOID*)&sizetResult, sizeof(sizetResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MAX_WORK_GROUP_SIZE", sizetResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS", uintResult );
        FrmLogMessage ( strMessage );

        cl_uint maxDims = uintResult;
        size_t* workItemSizes = new size_t[ maxDims ];
        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, workItemSizes, sizeof(size_t) * maxDims );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d", ALIGN_SPACES, "CL_DEVICE_MAX_WORK_ITEM_SIZES", workItemSizes[0] );
        for (UINT32 dim = 1; dim < maxDims; dim++)
        {
            CHAR val[1024];
            FrmSprintf( val, sizeof(val), "x%d", workItemSizes[dim] );
            FrmStrcat( strMessage, val );
        }
        FrmStrcat( strMessage, "\n" );
        FrmLogMessage( strMessage );
        delete [] workItemSizes;

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MAX_WRITE_IMAGE_ARGS", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MEM_BASE_ADDR_ALIGN, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MEM_BASE_ADDR_ALIGN", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE", uintResult );
        FrmLogMessage ( strMessage );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_PROFILING_TIMER_RESOLUTION, (VOID*)&sizetResult, sizeof(sizetResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_PROFILING_TIMER_RESOLUTION", sizetResult );
        FrmLogMessage ( strMessage );

        BitfieldToString deviceQueueProps[] =
        {
            { CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE" },
            { CL_QUEUE_PROFILING_ENABLE,              "CL_QUEUE_PROFILING_ENABLE" }
        };
        result &= GetDeviceInfo( deviceId, CL_DEVICE_QUEUE_PROPERTIES, (VOID*)&bitfieldResult, sizeof(bitfieldResult) );
        ShowBitfield( "CL_DEVICE_QUEUE_PROPERTIES", bitfieldResult, deviceQueueProps, sizeof(deviceQueueProps) / sizeof(BitfieldToString) );

        result &= GetDeviceInfo( deviceId, CL_DEVICE_SINGLE_FP_CONFIG, (VOID*)&bitfieldResult, sizeof(bitfieldResult) );
        ShowBitfield( "CL_DEVICE_SINGLE_FP_CONFIG", bitfieldResult, fpConfig, sizeof(fpConfig) / sizeof(BitfieldToString) );

        BitfieldToString deviceType[] =
        {
            { CL_DEVICE_TYPE_CPU,           "CL_DEVICE_TYPE_CPU" },
            { CL_DEVICE_TYPE_GPU,           "CL_DEVICE_TYPE_GPU" },
            { CL_DEVICE_TYPE_ACCELERATOR,   "CL_DEVICE_TYPE_ACCELERATOR" },
            { CL_DEVICE_TYPE_DEFAULT,       "CL_DEVICE_TYPE_DEFAULT" }
        };
        result &= GetDeviceInfo( deviceId, CL_DEVICE_TYPE, (VOID*)&bitfieldResult, sizeof(bitfieldResult) );
        ShowBitfield( "CL_DEVICE_TYPE", bitfieldResult, deviceType, sizeof(deviceType) / sizeof(BitfieldToString) );


        result &= GetDeviceInfo( deviceId, CL_DEVICE_VENDOR_ID, (VOID*)&uintResult, sizeof(uintResult) );
        FrmSprintf ( strMessage, sizeof(strMessage), "%*s: %d\n", ALIGN_SPACES, "CL_DEVICE_VENDOR_ID", uintResult );
        FrmLogMessage ( strMessage );
    }

    return result;
}

