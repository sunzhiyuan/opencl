//--------------------------------------------------------------------------------------
// File: TextureResource.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Crossplatform.h"

#if defined (_WIN32)            // WIN32
#include <windows.h>
#elif LINUX_OR_OSX
#include "Platform.h"
#include <strings.h>
#define _stricmp strcasecmp
#endif
#include <stdio.h>
#include <math.h>
#include "PackedResource.h"
#include "Image.h"


// Filter flags for OpenGL ES 2.0
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703

// WrapMode flags for OpenGL ES 2.0
#define GL_REPEAT                         0x2901
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_MIRRORED_REPEAT                0x8370


//--------------------------------------------------------------------------------------
// Name: struct TEXTURERESOURCE
// Desc: 
//--------------------------------------------------------------------------------------
struct TEXTURERESOURCE
{
    UINT32 nWidth;
    UINT32 nHeight;
    UINT32 nNumLevels;
    UINT32 nPixelFormat;
    UINT32 nPixelType;
    UINT32 nPitch;
    UINT32 nBaseSize;
    UINT32 nMipChainSize;
    UINT32 nMinMipSize;
    UINT32 nMagFilter;
    UINT32 nMinFilter;
    UINT32 nWrapMode;
};


//--------------------------------------------------------------------------------------
// Name: ProcessTextureResource()
// Desc: 
//--------------------------------------------------------------------------------------
bool ProcessTextureResource( const CHAR* strResourceID, const CHAR* strFileName,
                                const CHAR* strFormat, UINT32 nWidth, UINT32 nHeight,
                                BOOL bGenMipMaps, BOOL bFlipImage, UINT32 nRotate,
                                const CHAR* strMagFilter, const CHAR* strMinFilter,
                                const CHAR* strMipFilter, const CHAR* strWrapMode )
{
    if ((NULL == strResourceID) || (NULL == strFileName))
        return false;

    CImage m_Image;
    if (!LoadImage(strFileName, strFormat, nWidth, nHeight,
                    bGenMipMaps, bFlipImage, nRotate,
                    &m_Image))
        return false;

    // Build the filter flags
    UINT32 nMagFilter = 0;
    if( strMagFilter )
    {
        if(      !_stricmp( strMagFilter, "NEAREST" ) ) nMagFilter = GL_NEAREST;
        else if( !_stricmp( strMagFilter, "LINEAR" ) )  nMagFilter = GL_LINEAR;
        else {} // Warning! Invalid filter mode
    }

    UINT32 nMinFilter = 0;
    if( strMinFilter )
    {
        if(      !_stricmp( strMinFilter, "NEAREST" ) ) nMinFilter = GL_NEAREST;
        else if( !_stricmp( strMinFilter, "LINEAR" ) )  nMinFilter = GL_LINEAR;
        else {} // Warning! Invalid filter mode
    }

    // Merge mip flags with the min filter
    if( strMipFilter )
    {
        if( !_stricmp( strMipFilter, "NEAREST" ) )
        {
            if( nMinFilter == GL_LINEAR ) nMinFilter = GL_LINEAR_MIPMAP_NEAREST;
            else 				          nMinFilter = GL_NEAREST_MIPMAP_NEAREST;
        }
        else if( !_stricmp( strMipFilter, "LINEAR" ) )
        {
            if( nMinFilter == GL_LINEAR ) nMinFilter = GL_LINEAR_MIPMAP_LINEAR;
            else 				          nMinFilter = GL_NEAREST_MIPMAP_LINEAR;
        }
        else {} // Warning! Invalid filter mode
    }

    UINT32 nWrapMode = 0;
    if( strWrapMode )
    {
        if(      !_stricmp( strWrapMode, "REPEAT" ) )          nWrapMode = GL_REPEAT;
        else if( !_stricmp( strWrapMode, "CLAMP_TO_EDGE" ) )   nWrapMode = GL_CLAMP_TO_EDGE;
        else if( !_stricmp( strWrapMode, "MIRRORED_REPEAT" ) ) nWrapMode = GL_MIRRORED_REPEAT;
        else {} // Warning! Invalid wrap mode
    }

    // Build the texture header
    TEXTURERESOURCE header;
    header.nWidth            = m_Image.m_nWidth;
    header.nHeight           = m_Image.m_nHeight;
    header.nNumLevels        = m_Image.m_nNumLevels;
    header.nPixelFormat      = m_Image.m_nPixelFormat;
    header.nPixelType        = m_Image.m_nPixelType;
    header.nPitch            = m_Image.m_nRowPitch;
    header.nBaseSize         = m_Image.m_nBaseSize;
    header.nMipChainSize     = m_Image.m_nMipChainSize;
    header.nMinMipSize       = m_Image.m_nMinMipSize;
    header.nMagFilter        = nMagFilter;
    header.nMinFilter        = nMinFilter;
    header.nWrapMode         = nWrapMode;

    // Write the texture data to the resource
    {
        PackedResource::AddResource( strResourceID );
        PackedResource::AddSystemData( &header, sizeof(TEXTURERESOURCE) );

        if (!PackedResource::WriteImageData(&m_Image))
        {
            printf("\nError: Could not write image data to PAK for texture resource %s\n", strFileName);
            return false;
        }
    }

    // Verify the final pixel format
    for( UINT32 i=0; i<g_nNumPixelFormats; i++ )
    {
        if( ( g_PixelFormats[i].nPixelFormat == header.nPixelFormat ) &&
            ( g_PixelFormats[i].nPixelType == header.nPixelType ) )
        {
            printf( " [%s]", g_PixelFormats[i].strName );
            break;
        }
    }

    return true;
}

