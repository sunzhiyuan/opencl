//--------------------------------------------------------------------------------------
// File: TextureResource.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon Toolkit
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Crossplatform.h"

#if defined (_WIN32)            // WIN32
#include <windows.h>
#elif LINUX_OR_OSX
#include <strings.h>
#include "Platform.h"
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
// Name: struct CUBEMAPRESOURCE
// Desc: 
//--------------------------------------------------------------------------------------
struct CUBEMAPRESOURCE
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
// Name: ProcessCubeMapResource()
// Desc: 
//--------------------------------------------------------------------------------------
bool ProcessCubeMapResource( const CHAR* strResourceID, const CHAR* strFileName[6],
                                const CHAR* strFormat, UINT32 nSize,
                                BOOL bGenMipMaps, BOOL bFlipImages, UINT32 nRotate,
                                const CHAR* strMagFilter, const CHAR* strMinFilter,
                                const CHAR* strMipFilter, const CHAR* strWrapMode )
{
    if( NULL == strResourceID )
        return false;
    if( NULL == strFileName[0] )
        return false;
    if( NULL == strFileName[1] )
        return false;
    if( NULL == strFileName[2] )
        return false;
    if( NULL == strFileName[3] )
        return false;
    if( NULL == strFileName[4] )
        return false;
    if( NULL == strFileName[5] )
        return false;

    // Load the images for each face of the cubemap
    CImage m_Images[6];
    for( UINT32 i=0; i<6; i++ )
    {
        if (!LoadImage( strFileName[i], strFormat, nSize, nSize,
                        bGenMipMaps, bFlipImages, nRotate,
                        &m_Images[i] ))
            return false;

        // Use the dimensions of the first image to make sure all the other images conform
        nSize  = m_Images[0].m_nWidth;
    }

    // Compute the mip chain size
    UINT32 nMipChainSize = 0;
    if( m_Images[0].m_nFlags & IMG_COMPRESSED )
    {
        nMipChainSize = m_Images[0].m_nMipChainSize;
    }
    else
    {
        UINT32 nTexelSize = m_Images[0].m_nRowPitch / m_Images[0].m_nWidth;
        UINT32 nMipWidth  = m_Images[0].m_nWidth;
        UINT32 nMipHeight = m_Images[0].m_nHeight;

        for( UINT32 i=0; i<m_Images[0].m_nNumLevels; i++ )
        {
            nMipChainSize += nMipHeight * nMipWidth * nTexelSize;

            nMipWidth  = nMipWidth  / 2;
            nMipHeight = nMipHeight / 2;
        }
    }

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
    CUBEMAPRESOURCE header;
    header.nWidth            = m_Images[0].m_nWidth;
    header.nHeight           = m_Images[0].m_nHeight;
    header.nNumLevels        = m_Images[0].m_nNumLevels;
    header.nPixelFormat      = m_Images[0].m_nPixelFormat;
    header.nPixelType        = m_Images[0].m_nPixelType;
    header.nPitch            = m_Images[0].m_nRowPitch;
    header.nBaseSize         = m_Images[0].m_nBaseSize;
    header.nMipChainSize     = nMipChainSize;
    header.nMinMipSize       = m_Images[0].m_nMinMipSize;
    header.nMagFilter        = nMagFilter;
    header.nMinFilter        = nMinFilter;
    header.nWrapMode         = nWrapMode;

    // Write the texture data to the resource
    {
        PackedResource::AddResource( strResourceID );
        PackedResource::AddSystemData( &header, sizeof(CUBEMAPRESOURCE) );

        for (UINT32 i = 0; i < 6; i++)
        {
            if( !PackedResource::WriteImageData( &m_Images[i] ) )
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

