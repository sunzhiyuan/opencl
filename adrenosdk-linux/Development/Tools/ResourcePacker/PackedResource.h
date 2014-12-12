//--------------------------------------------------------------------------------------
// File: PackedResource.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once

#include "Crossplatform.h"
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
class CImage;


// TODO: Really need define these based on if it's 64 bit or 32 bit..
#if OSX
#include "Platform.h"
#endif



//--------------------------------------------------------------------------------------
// Name: struct PAK_FILE_HEADER
// Desc: 
//--------------------------------------------------------------------------------------
const UINT32 PAK_FILE_SIGNATURE ( ('P'<<0) | ('A'<<8) | ('K'<<16) | (0<<24) );

struct PAK_FILE_HEADER
{
    UINT32 nFileSignature;
    UINT32 nSystemDataSize;
    UINT32 nBufferDataSize;
    UINT32 nNumResources;
};


//--------------------------------------------------------------------------------------
// Name: struct RESOURCE
// Desc: 
//--------------------------------------------------------------------------------------
struct RESOURCE
{
    CHAR*  strName;
    UINT32 nSystemDataOffset;
    UINT32 nBufferDataOffset;
    VOID*  pObject;
};


namespace PackedResource
{


//--------------------------------------------------------------------------------------
// Accessor functions for the packed resource
//--------------------------------------------------------------------------------------
RESOURCE* GetResources();
UINT32    GetNumResources();
VOID*     GetSystemData();
VOID*     GetBufferData();
UINT32    GetSystemDataOffset();
UINT32    GetBufferDataOffset();

//////////////////////////////////////////////////////////////////////////
// Clear functions
//////////////////////////////////////////////////////////////////////////
VOID    ClearResources();

//--------------------------------------------------------------------------------------
// Functions to add resources and binary data to the packed resource
//--------------------------------------------------------------------------------------
VOID    AddResource( const CHAR* strName );
bool AddSystemData( const VOID* pData, UINT32 nSize );
bool AddBufferData( const VOID* pData, UINT32 nSize );


//--------------------------------------------------------------------------------------
// Name: WriteImageData()
// Desc: 
//--------------------------------------------------------------------------------------
bool WriteImageData( CImage* pImage );


//--------------------------------------------------------------------------------------
// Name: WriteOutputFile()
// Desc: 
//--------------------------------------------------------------------------------------
bool WriteOutputFile( const CHAR* strOutputFilename );


}; // namespace PackedResource

