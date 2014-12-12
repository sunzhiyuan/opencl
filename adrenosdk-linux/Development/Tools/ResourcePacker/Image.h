//--------------------------------------------------------------------------------------
// File: Image.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon Toolkit
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once


//--------------------------------------------------------------------------------------
// Image flags
//--------------------------------------------------------------------------------------
#define IMG_COMPRESSED   0x00000004

#include "Platform.h"

// OpenGL pixel formats
enum IMAGE_PIXEL_FORMAT
{
    GL_ALPHA                                     = 0x1906,
    GL_RGB                                       = 0x1907,
    GL_RGBA                                      = 0x1908,
    GL_LUMINANCE                                 = 0x1909,
    GL_LUMINANCE_ALPHA                           = 0x190A,
	GL_COMPRESSED_RGB_S3TC_DXT1_EXT              = 0x83F0,
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT             = 0x83F1,
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT             = 0x83F2,
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT             = 0x83F3,
    GL_3DC_X_AMD                                 = 0x87F9,
    GL_3DC_XY_AMD                                = 0x87FA,
	GL_SRGB8_ALPHA8                              = 0x8C43,
    GL_ATC_RGB_AMD                               = 0x8C92,
    GL_ATC_RGBA_AMD                              = 0x8C93,
    GL_ATC_RGB_AINTERP_AMD                       = 0x8C94,
    GL_ETC1_RGB8_OES                             = 0x8D64,
    GL_ETC1_RGB_A4_OES                           = 0x8D65,
    GL_ETC1_RGB_AINTERP_OES                      = 0x8D66,
	GL_COMPRESSED_R11_EAC                        = 0x9270,
    GL_COMPRESSED_SIGNED_R11_EAC                 = 0x9271,
    GL_COMPRESSED_RG11_EAC                       = 0x9272,
    GL_COMPRESSED_SIGNED_RG11_EAC                = 0x9273,
	GL_COMPRESSED_RGB8_ETC2                      = 0x9274,
    GL_COMPRESSED_SRGB8_ETC2                     = 0x9275,
    GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2  = 0x9276,
    GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9277,
    GL_COMPRESSED_RGBA8_ETC2_EAC                 = 0x9278,
    GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC          = 0x9279,
};

// OpenGL pixel types
enum IMAGE_PIXEL_TYPE
{
    GL_NONE                           = 0,
    GL_UNSIGNED_BYTE                  = 0x1401,
    GL_FLOAT                          = 0x1406,
    GL_UNSIGNED_SHORT_4_4_4_4         = 0x8033,
    GL_UNSIGNED_SHORT_5_5_5_1         = 0x8034,
    GL_UNSIGNED_SHORT_5_6_5           = 0x8363,
};


//--------------------------------------------------------------------------------------
// Valid pixel formats/types:
//--------------------------------------------------------------------------------------
struct PIXELFORMAT
{
    const CHAR*        strName;
    IMAGE_PIXEL_FORMAT nPixelFormat;
    IMAGE_PIXEL_TYPE   nPixelType;
};

const PIXELFORMAT g_PixelFormats[] = 
{
    { "ALPHA",						GL_ALPHA,										GL_UNSIGNED_BYTE },
    { "ALPHA_FLOAT",				GL_ALPHA,										GL_FLOAT },
    { "RGB",						GL_RGB,											GL_UNSIGNED_BYTE },
    { "RGB_FLOAT",					GL_RGB,											GL_FLOAT },
    { "RGB_5_6_5",					GL_RGB,											GL_UNSIGNED_SHORT_5_6_5 },
    { "RGBA",						GL_RGBA,										GL_UNSIGNED_BYTE },
    { "RGBA_4_4_4_4",				GL_RGBA,										GL_UNSIGNED_SHORT_4_4_4_4 },
    { "RGBA_5_5_5_1",				GL_RGBA,										GL_UNSIGNED_SHORT_5_5_5_1 },
    { "LUMINANCE",					GL_LUMINANCE,									GL_UNSIGNED_BYTE },
    { "LUMINANCE_FLOAT",			GL_LUMINANCE,									GL_FLOAT },
    { "LUMINANCE_ALPHA",			GL_LUMINANCE_ALPHA,								GL_UNSIGNED_BYTE },
    { "LUMINANCE_ALPHA_FLOAT",		GL_LUMINANCE_ALPHA,								GL_FLOAT },
	{ "SRGB8_ALPHA8",				GL_SRGB8_ALPHA8,								GL_UNSIGNED_BYTE },
    { "3DC_X_AMD",					GL_3DC_X_AMD,									GL_NONE },
    { "3DC_XY_AMD",					GL_3DC_XY_AMD,									GL_NONE },
    { "ATC_RGB_AMD",				GL_ATC_RGB_AMD,									GL_NONE },
    { "ATC_RGBA_AMD",				GL_ATC_RGBA_AMD,								GL_NONE },
	{ "ETC1_RGB8",					GL_ETC1_RGB8_OES,								GL_NONE },
	{ "ETC2_RGB8",					GL_COMPRESSED_RGB8_ETC2,						GL_NONE },
	{ "ETC2_RGBA8",					GL_COMPRESSED_RGBA8_ETC2_EAC,					GL_NONE },
	{ "EAC_R_SIGNED",				GL_COMPRESSED_SIGNED_R11_EAC,					GL_NONE },
	{ "EAC_R_UNSIGNED",				GL_COMPRESSED_R11_EAC,							GL_NONE },
	{ "EAC_RG_SIGNED",				GL_COMPRESSED_SIGNED_RG11_EAC,					GL_NONE },
	{ "EAC_RG_UNSIGNED",			GL_COMPRESSED_RG11_EAC,							GL_NONE },
	{ "ETC2_PUNCHTHROUGH_RGB8_A1",	GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,	GL_NONE },
    { "ETC2_PUNCHTHROUGH_SRGB8_A1",	GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,	GL_NONE },
    { "ETC2_SRGB8",					GL_COMPRESSED_SRGB8_ETC2,						GL_NONE },
    { "ETC2_SRGB8_A8",				GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,			GL_NONE },
	{ "S3TC_DXT1_RGB",				GL_COMPRESSED_RGB_S3TC_DXT1_EXT,				GL_NONE },
    { "S3TC_DXT1_RGBA",				GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,				GL_NONE },
    { "S3TC_DXT3_RGBA",				GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,				GL_NONE },
    { "S3TC_DXT5_RGBA",				GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,				GL_NONE },

    // Shortcuts (that probably should be phased out)
    { "A8R8G8B8",					GL_RGBA,										GL_UNSIGNED_BYTE },
    { "ATC",        				GL_ATC_RGBA_AMD,								GL_NONE },
};

const UINT32 g_nNumPixelFormats = sizeof(g_PixelFormats) / sizeof(g_PixelFormats[0]);


//--------------------------------------------------------------------------------------
// Name: class CImage
// Desc: 
//--------------------------------------------------------------------------------------
class CImage
{
public:
    IMAGE_PIXEL_FORMAT m_nPixelFormat;
    IMAGE_PIXEL_TYPE   m_nPixelType;
    BYTE*    m_pData;
    UINT32   m_nWidth;
    UINT32   m_nHeight;
    UINT32   m_nNumLevels;
    UINT32   m_nRowPitch;
    UINT32   m_nBaseSize;
    UINT32   m_nMipChainSize;
    UINT32   m_nMinMipSize;
    UINT32   m_nFlags;

public:
    bool Load( const CHAR* strFilename );
    bool LoadUsingFreeImage( const CHAR* strFilename );
    bool LoadATC( const CHAR* strFilename );
    bool LoadPFM( const CHAR* strFilename );

    bool Resize( UINT32 nWidth, UINT32 nHeight );
    bool Flip();
    bool Rotate( UINT32 nAngle );
    bool ConvertFormat( IMAGE_PIXEL_FORMAT nPixelFormat, IMAGE_PIXEL_TYPE nPixelType );
    bool ConvertToCompressedFormat( IMAGE_PIXEL_FORMAT nPixelFormat, IMAGE_PIXEL_TYPE nPixelType );
    bool ConvertToFloatingPointFormat( IMAGE_PIXEL_FORMAT nPixelFormat, IMAGE_PIXEL_TYPE nPixelType );
    bool ConvertToFixedPointFormat( IMAGE_PIXEL_FORMAT nPixelFormat, IMAGE_PIXEL_TYPE nPixelType );

    CImage();
    ~CImage();
};


//--------------------------------------------------------------------------------------
// Name: LoadImage()
// Desc: 
//--------------------------------------------------------------------------------------
bool LoadImage( const CHAR* strFileName, const CHAR* strFormat,
                   UINT32 nWidth, UINT32 nHeight, BOOL bGenMipMaps,
                   BOOL bFlipImage, UINT32 nRotate,
                   CImage* pImage );

