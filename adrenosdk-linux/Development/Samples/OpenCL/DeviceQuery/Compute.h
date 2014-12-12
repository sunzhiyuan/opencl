//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Incorporated.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef COMPUTE_H
#define COMPUTE_H

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmComputeApplication
{

public:
    virtual BOOL Initialize();
    virtual VOID Destroy();
    virtual BOOL Compute();

    CSample( const CHAR* strName );
private:

    // Structure for converting bitfields to string descriptions
    typedef struct
    {
        cl_bitfield value;
        const CHAR* str;
    } BitfieldToString;
    // Structure for converting cl_uint values to strings
    typedef struct
    {
        cl_uint value;
        const CHAR* str;
    } UIntToString;

    BOOL ShowPlatformInfo( cl_platform_info platformInfo, const CHAR *infoName );
    BOOL ShowDeviceInfoStr( cl_device_id deviceId, cl_device_info deviceInfo, const CHAR *infoName );
    VOID ShowBitfield( const CHAR* infoName, cl_bitfield bitfield, BitfieldToString *bitfieldToString, INT32 count );
    VOID ShowUInt( const CHAR* infoName, cl_uint uintValue, UIntToString *uintToString, INT32 count );


    BOOL GetDeviceInfo( cl_device_id deviceId, cl_device_info deviceInfo, VOID* infoResult, size_t infoResultSize );
};


#endif // COMPUTE_H
