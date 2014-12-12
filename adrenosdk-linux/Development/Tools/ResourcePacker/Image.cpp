//--------------------------------------------------------------------------------------
// File: Image.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Crossplatform.h"

#if defined (_WIN32)
#include <windows.h>
#undef min
#undef max
#elif LINUX_OR_OSX
#include "Platform.h"
#endif
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include "TextureConverter.h"
#include "FreeImage.h"
#include "Image.h"

#if LINUX_OR_OSX
#include <stdint.h>
#endif

#if !defined(min)
using std::min;
#endif

#if !defined(max)
using std::max;
#endif

// Conversion helpers
struct BYTE4  { BYTE  r,g,b,a; };
struct BYTE3  { BYTE  r,g,b; };
struct BYTE2  { BYTE  l,a; };
struct BYTE1  { union { BYTE l; BYTE a; }; };
struct FLOAT4 { FLOAT r,g,b,a; };
struct FLOAT3 { FLOAT r,g,b; };
struct FLOAT2 { FLOAT l,a; };
struct FLOAT1 { union { FLOAT l; FLOAT a; }; };

inline FLOAT ClampF( FLOAT x ) { return min( 1.0f, max( 0.0f, x ) ); }


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CImage::CImage()
{
    m_nPixelFormat  = GL_RGBA;
    m_nPixelType    = GL_UNSIGNED_BYTE;
    m_pData         = NULL;
    m_nWidth        = 0;
    m_nHeight       = 0;
    m_nRowPitch     = 0;
    m_nBaseSize     = 0;
    m_nMipChainSize = 0;
    m_nMinMipSize   = 0;
    m_nFlags        = 0;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
CImage::~CImage()
{
    delete[] m_pData;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
bool CImage::Load( const CHAR* strFilename )
{
    if (!LoadATC(strFilename))
    {
        if (!LoadUsingFreeImage(strFilename))
        {
            if (!LoadPFM(strFilename))
            {
                return false;
            }
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
// Support for ATC-compressed texture files
//-----------------------------------------------------------------------------

// ATC file header
struct ATC_HEADER
{
    UINT nSignature;
    UINT nWidth;
    UINT nHeight;
    UINT nFlags;
    UINT nDataOffset;
};

// CTES file header
struct CTES_HEADER
{
    UINT nSignature;
    UINT nWidth;
    UINT nHeight;
    UINT nFlags;
    UINT nDataOffset;
    UINT nReserved1;
    UINT nNumLevels;
    UINT nCompressionFlags;
};


// File signatures
#define ATC_SIGNATURE                   0xCCC40002
#define ETC_SIGNATURE                   0xEC000001

// ATC flags
#define CTES_RGB                        0x00000001
#define CTES_RGBA                       0x00000002
#define CTES_INTERPOLATED_ALPHA         0x00000010


//-----------------------------------------------------------------------------
// Name: LoadATC()
// Desc: Attempts to load the image from an ATC compressed file
//-----------------------------------------------------------------------------
bool CImage::LoadATC( const CHAR* strFilename )
{
    DWORD nNumBytesRead;
#if defined (_WIN32)
    HANDLE hFile = CreateFileA( strFilename, GENERIC_READ, FILE_SHARE_READ, NULL, 
                                OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL );
    if( INVALID_HANDLE_VALUE == hFile )
        return false;
#elif LINUX_OR_OSX
    FILE *fptr = fopen(strFilename, "rb");
    if (!fptr)
        return false;
#endif

    // Read in the header and check if it's an ATC file or a CTES file
    UINT32 nSignature;
    CTES_HEADER Header;
#if defined (_WIN32)
    ReadFile( hFile, &nSignature, sizeof(nSignature), &nNumBytesRead, NULL );
    if( ATC_SIGNATURE != nSignature && ETC_SIGNATURE != nSignature )
    {
        CloseHandle( hFile );
        return false;
    }

    // Rewind the file
    SetFilePointer( hFile, 0, NULL, FILE_BEGIN );

    // Read in the header 

    ReadFile( hFile, &Header, sizeof(Header), &nNumBytesRead, NULL );
#elif LINUX_OR_OSX
    fread(&nSignature, sizeof(nSignature), 1, fptr);
    if (ATC_SIGNATURE != nSignature && ETC_SIGNATURE != nSignature)
    {
        fclose(fptr);
        return false;
    }

    // rewind the file
    fseek(fptr, 0, SEEK_SET);

    // read in the header
    nNumBytesRead = fread(&Header, sizeof(Header), 1, NULL);

#endif


    // File in missing pieces for ATC files
    if( ATC_SIGNATURE == nSignature )
    {
        Header.nReserved1        = 1;
        Header.nNumLevels        = 1;
        Header.nCompressionFlags = (Header.nFlags & CTES_RGB ) ? GL_ATC_RGB_AMD : GL_ATC_RGBA_AMD;
    }

    // Determine the various image attributes
    UINT nBytesPerBlock;

    switch( Header.nCompressionFlags )
    {
        case GL_ATC_RGB_AMD:
            m_nPixelFormat = GL_ATC_RGB_AMD;
            m_nPixelType   = GL_NONE;
            nBytesPerBlock = 8;
            break;
        case GL_ATC_RGBA_AMD:
            m_nPixelFormat = GL_ATC_RGBA_AMD;
            m_nPixelType   = GL_NONE;
            nBytesPerBlock = 16;
            break;
        case GL_ATC_RGB_AINTERP_AMD:
            m_nPixelFormat = GL_ATC_RGB_AINTERP_AMD;
            m_nPixelType   = GL_NONE;
            nBytesPerBlock = 16;
            break;
        case GL_ETC1_RGB8_OES:
            m_nPixelFormat = GL_ETC1_RGB8_OES;
            m_nPixelType   = GL_NONE;
            nBytesPerBlock = 8;
            break;
        case GL_ETC1_RGB_A4_OES:
            m_nPixelFormat = GL_ETC1_RGB_A4_OES;
            m_nPixelType   = GL_NONE;
            nBytesPerBlock = 16;
            break;
        case GL_ETC1_RGB_AINTERP_OES:
            m_nPixelFormat = GL_ETC1_RGB_AINTERP_OES;
            m_nPixelType   = GL_NONE;
            nBytesPerBlock = 16;
            break;
        default:
            // Unsupported type
#if defined (_WIN32)
            CloseHandle( hFile );
#elif LINUX_OR_OSX
            fclose(fptr);
#endif
            return false;
    }

    m_nWidth     = Header.nWidth;
    m_nHeight    = Header.nHeight;
    m_nRowPitch  = 0;
    m_nNumLevels = Header.nNumLevels;

    m_nFlags     = IMG_COMPRESSED;

    // Compute size to account for mip maps
    m_nBaseSize     = 0;
    m_nMipChainSize = 0;
    m_nMinMipSize   = 0;
    UINT w = m_nWidth;
    UINT h = m_nHeight;
    for( UINT32 i=0; i<m_nNumLevels; i++ )
    {
        UINT nNumBlocks = ((w + 3) >> 2) * ((h + 3) >> 2);
        m_nMipChainSize += nNumBlocks * nBytesPerBlock;

        if( i == 0 )
            m_nBaseSize = nNumBlocks * nBytesPerBlock;
        if( i == m_nNumLevels-1 )
            m_nMinMipSize = nNumBlocks * nBytesPerBlock;

        if( w > 1 ) w >>= 1;
        if( h > 1 ) h >>= 1;
    }

    // Allocate memory for the encoded image 
    m_pData = new BYTE[m_nMipChainSize];
    if ( NULL == m_pData)
        return false;

#if defined (_WIN32)
    // Seek to the data offset
    SetFilePointer( hFile, Header.nDataOffset, NULL, FILE_BEGIN );

    // Read in the encoded image 
    ReadFile( hFile, (VOID*)m_pData, m_nMipChainSize, &nNumBytesRead, NULL );

    CloseHandle( hFile );
#elif LINUX_OR_OSX

    fseek(fptr, Header.nDataOffset, SEEK_SET);
    nNumBytesRead = fread(m_pData, m_nMipChainSize, 1, fptr);
#endif
    return true;
}


//-----------------------------------------------------------------------------
// Name: LoadUsingFreeImage()
// Desc: Attempts to load any image format supported by FreeImage.
//-----------------------------------------------------------------------------
bool CImage::LoadUsingFreeImage( const CHAR* strFilename )
{
    FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename( strFilename );
    if( format == FIF_UNKNOWN )
    {
        return false;
    }

    FIBITMAP* pBitmap = FreeImage_Load( format, strFilename );
    if( NULL == pBitmap )
    {
        return false;
    }

    FREE_IMAGE_TYPE ImageType = FreeImage_GetImageType( pBitmap );

    // Note: this currently only works for 32-bpp images
    UINT32 nBPP = FreeImage_GetBPP( pBitmap );

    if( 32 != nBPP )
    {
        FIBITMAP* pNewBitmap = FreeImage_ConvertTo32Bits( pBitmap );
        FreeImage_Unload( pBitmap );
        pBitmap = pNewBitmap;
        nBPP = FreeImage_GetBPP( pBitmap );
    }

    if( 32 != nBPP )
    {
        FreeImage_Unload( pBitmap );
        return false;
    }

    // Flip images if we need them in a different coordinate-system than FreeImage uses
    // FreeImage_FlipVertical( pBitmap );

    m_nPixelFormat  = GL_RGBA;
    m_nPixelType    = GL_UNSIGNED_BYTE;
    m_nWidth        = FreeImage_GetWidth( pBitmap );
    m_nHeight       = FreeImage_GetHeight( pBitmap );
    m_nRowPitch     = m_nWidth * ( nBPP / 8 );
    m_nBaseSize     = m_nRowPitch * m_nHeight;
    m_nMipChainSize = m_nRowPitch * m_nHeight;
    m_nMinMipSize   = m_nRowPitch * m_nHeight;
    m_nNumLevels    = 1;
    m_pData = new BYTE[m_nMipChainSize];

    FreeImage_ConvertToRawBits( m_pData, pBitmap, m_nRowPitch, nBPP, FI_RGBA_RED_MASK, 
                                                                     FI_RGBA_GREEN_MASK, 
                                                                     FI_RGBA_BLUE_MASK, TRUE );

    // BUGBUG: FreeImage won't respect the color channel masks, so we need to swizzle
    // the channels manually
    for( UINT32 x=0; x<m_nMipChainSize; x+=4 )
    {
        BYTE r = m_pData[x+0];
        BYTE g = m_pData[x+1];
        BYTE b = m_pData[x+2];
        BYTE a = m_pData[x+3];

        m_pData[x+0] = b;
        m_pData[x+1] = g;
        m_pData[x+2] = r;
        m_pData[x+3] = a;
    }

    FreeImage_Unload( pBitmap );

    return true;
}


//-----------------------------------------------------------------------------
// Name: LoadPFM()
// Desc: Attempts to load a PFM (portable float map) image
//-----------------------------------------------------------------------------
bool CImage::LoadPFM( const CHAR* strFilename )
{
    FILE* file = fopen( strFilename, "rb" );
    if( NULL == file )
        return false;

    CHAR strMagic[20];
    fscanf( file, "%10s", strMagic );
    if( 0 != strcmp( strMagic, "PF" ) )
    {
        fclose( file );
        return false;
    }
    
    UINT32 nWidth;
    UINT32 nHeight;
    FLOAT  fScale;
    fscanf( file, "%d %d %f\n", &nWidth, &nHeight, &fScale );
    
    FLOAT3* pData = new FLOAT3[ nWidth * nHeight ];
    fread( pData, nWidth*nHeight, sizeof(FLOAT3), file );

    // TODO: Investigate whether image data needs to be flipped vertically

    m_nPixelFormat  = GL_RGB;
    m_nPixelType    = GL_FLOAT;
    m_nWidth        = nWidth;
    m_nHeight       = nHeight;
    m_nRowPitch     = m_nWidth * sizeof(FLOAT3);
    m_nBaseSize     = m_nRowPitch * m_nHeight;
    m_nMipChainSize = m_nRowPitch * m_nHeight;
    m_nMinMipSize   = m_nRowPitch * m_nHeight;
    m_nNumLevels    = 1;
    m_pData         = (BYTE*)pData;

    return true;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: Resizes an image, an optionally applies a colorkey
//--------------------------------------------------------------------------------------
bool CImage::Resize( UINT32 nNewWidth, UINT32 nNewHeight )
{
    if( m_nWidth == nNewWidth && m_nHeight == nNewHeight )
        return true;

    // NOTE: This currently is hard-coded to work only on 32-bpp images!
    UINT32 nBPP = 32;

    FIBITMAP* pOldBitmap = FreeImage_ConvertFromRawBits( m_pData, m_nWidth, m_nHeight, m_nRowPitch, nBPP,
                                                         0xff000000, 0x00ff0000, 0x0000ff00, TRUE );

    FIBITMAP* pNewBitmap = FreeImage_Rescale( pOldBitmap, nNewWidth, nNewHeight, FILTER_BICUBIC );

    delete m_pData;

    m_nWidth        = nNewWidth;
    m_nHeight       = nNewHeight;
    m_nRowPitch     = m_nWidth * ( nBPP / 8 );
    m_nBaseSize     = m_nRowPitch * m_nHeight;
    m_nMipChainSize = m_nRowPitch * m_nHeight;
    m_nMinMipSize   = m_nRowPitch * m_nHeight;
    m_pData = new BYTE[m_nMipChainSize];

    FreeImage_ConvertToRawBits( m_pData, pNewBitmap, m_nRowPitch, nBPP, 0xff000000, 0x00ff0000, 0x0000ff00, TRUE );

    FreeImage_Unload( pOldBitmap );
    FreeImage_Unload( pNewBitmap );

    return true;
}


//--------------------------------------------------------------------------------------
// Name: Flip()
// Desc: Flips an image vertically
//--------------------------------------------------------------------------------------
bool CImage::Flip()
{
    // NOTE: This currently is hard-coded to work only on 32-bpp images!
    UINT32 nBPP = 32;

    FIBITMAP* pBitmap = FreeImage_ConvertFromRawBits( m_pData, m_nWidth, m_nHeight, m_nRowPitch, nBPP,
                                                      0xffffffff, 0xffffffff, 0xffffffff, TRUE );
    FreeImage_FlipVertical( pBitmap );
    FreeImage_ConvertToRawBits( m_pData, pBitmap, m_nRowPitch, nBPP, 0xffffffff, 0xffffffff, 0xffffffff, TRUE );
    FreeImage_Unload( pBitmap );
    return true;
}


//--------------------------------------------------------------------------------------
// Name: Rotate()
// Desc: Rotates an image by 0, 90, 180, or 270 degrees
//--------------------------------------------------------------------------------------
bool CImage::Rotate( UINT32 nAngle )
{
    if( 0 == nAngle )
        return true;

    // NOTE: This currently is hard-coded to work only on 32-bpp images!
    UINT32 nBPP = 32;

    // Create a rotated bitmap
    FIBITMAP* pBitmap = FreeImage_ConvertFromRawBits( m_pData, m_nWidth, m_nHeight, m_nRowPitch, nBPP,
                                                      0xffffffff, 0xffffffff, 0xffffffff, TRUE );
    FIBITMAP* pRotatedBitmap = FreeImage_RotateClassic( pBitmap, (DOUBLE)nAngle );
    m_nWidth  = FreeImage_GetWidth( pRotatedBitmap );
    m_nHeight = FreeImage_GetHeight( pRotatedBitmap );
    m_nRowPitch = m_nWidth * (nBPP/8);

    // Re-allocate memory, in case the size has changed
    delete[] m_pData;
    m_pData = new BYTE[m_nRowPitch*m_nHeight];

    // Write out the bits
    FreeImage_ConvertToRawBits( m_pData, pRotatedBitmap, m_nRowPitch, nBPP, 0xffffffff, 0xffffffff, 0xffffffff, TRUE );
    FreeImage_Unload( pRotatedBitmap );
    FreeImage_Unload( pBitmap );
    return true;
}


//--------------------------------------------------------------------------------------
// Name: ConvertToCompressedFormat()
// Desc: Compresses an image to the desired format
//--------------------------------------------------------------------------------------
bool CImage::ConvertToCompressedFormat( IMAGE_PIXEL_FORMAT nNewPixelFormat, IMAGE_PIXEL_TYPE nNewPixelType )
{
    // Check if the image is already in the desired format
    if( m_nPixelFormat == nNewPixelFormat && m_nPixelType == nNewPixelType )
        return true;

    // For compressed formats, the pixel type should be zero
    if( nNewPixelType != 0 )
        return false;

    // If the image is already compressed, then we can't do anything about it!
    if( m_nPixelType == 0 )
        return false;

    // Weed out unsupported formats
	unsigned int qFormat;
    switch( nNewPixelFormat )
    {
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT: qFormat = Q_FORMAT_S3TC_DXT1_RGB; break;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: qFormat = Q_FORMAT_S3TC_DXT1_RGBA; break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: qFormat = Q_FORMAT_S3TC_DXT3_RGBA; break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: qFormat = Q_FORMAT_S3TC_DXT5_RGBA; break;
        case GL_ATC_RGB_AMD: qFormat = Q_FORMAT_ATITC_RGB; break;
        case GL_ATC_RGBA_AMD: qFormat = Q_FORMAT_ATITC_RGBA; break;
#if defined( COMPILE_GLES3_FORMATS )
		case GL_ETC1_RGB8_OES: qFormat = Q_FORMAT_ETC1_RGB8; break;
        case GL_COMPRESSED_RGB8_ETC2: qFormat = Q_FORMAT_ETC2_RGB8; break;
        case GL_COMPRESSED_RGBA8_ETC2_EAC: qFormat = Q_FORMAT_ETC2_RGBA8; break;
		case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2: qFormat = Q_FORMAT_ETC2_RGB8_PUNCHTHROUGH_ALPHA1; break;
		case GL_COMPRESSED_SRGB8_ETC2: qFormat = Q_FORMAT_ETC2_SRGB8; break;
        case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC: qFormat = Q_FORMAT_ETC2_SRGB8_ALPHA8; break;
		case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2: qFormat = Q_FORMAT_ETC2_SRGB8_PUNCHTHROUGH_ALPHA1; break;
        case GL_COMPRESSED_SIGNED_R11_EAC: qFormat = Q_FORMAT_EAC_R_SIGNED; break;
        case GL_COMPRESSED_R11_EAC: qFormat = Q_FORMAT_EAC_R_UNSIGNED; break;
        case GL_COMPRESSED_SIGNED_RG11_EAC: qFormat = Q_FORMAT_EAC_RG_SIGNED; break;
        case GL_COMPRESSED_RG11_EAC: qFormat = Q_FORMAT_EAC_RG_UNSIGNED; break;
#endif

        default: return false;
    }

    // Make sure the source data is in 32-bpp RGBA format
    if(!ConvertToFixedPointFormat(GL_RGBA, GL_UNSIGNED_BYTE))
        return false;

	// Save original dimensions
    UINT32 nWidth = m_nWidth;
    UINT32 nHeight = m_nHeight;
    UINT32 nNumLevels = m_nNumLevels;

	// The original mipmap datastructure (required by TextureConverter)
	TQonvertImage originalMipmap;
	originalMipmap.nWidth = nWidth;
	originalMipmap.nHeight = nHeight;
	originalMipmap.nFormat = Q_FORMAT_RGBA_8UI;
	originalMipmap.pFormatFlags = NULL;
	originalMipmap.nDataSize = nWidth * nHeight * 4;
	originalMipmap.pData = NULL;

	// The compressed mipmap datastructure (required by TextureConverter)
	TQonvertImage compressedMipmap;
	compressedMipmap.nWidth = nWidth;
	compressedMipmap.nHeight = nHeight;
	compressedMipmap.nFormat = qFormat;
	compressedMipmap.pFormatFlags = NULL;
	compressedMipmap.nDataSize = 0;
	compressedMipmap.pData = NULL;

	// Compute the post-compression size for all mipmaps
    UINT32 nMipChainSize = 0;
	UINT32 *nMipSize = new UINT32[nNumLevels];

	for( UINT32 nMipLevel = 0; nMipLevel < nNumLevels; nMipLevel++ )
	{
		unsigned int nRet;
		nRet = Qonvert(&originalMipmap, &compressedMipmap);

		if (nRet != Q_SUCCESS || compressedMipmap.nDataSize == 0)
		{
			delete[] nMipSize;
			return false;
		}

		nMipSize[nMipLevel] = compressedMipmap.nDataSize;
		nMipChainSize += compressedMipmap.nDataSize;

		if( compressedMipmap.nWidth  > 1 ) compressedMipmap.nWidth >>= 1;
        if( compressedMipmap.nHeight > 1 ) compressedMipmap.nHeight >>= 1;
	}

	// Create buffer for all mipmap images
    BYTE* pMipChainData = new BYTE[nMipChainSize];

    // Compress the texture, one mip level at a time
	UINT32 nMipOffset = 0;

    for( UINT32 nMipLevel = 0; nMipLevel < nNumLevels; nMipLevel++ )
    {
		originalMipmap.nWidth = m_nWidth;
		originalMipmap.nHeight = m_nHeight;
		originalMipmap.nDataSize = m_nWidth * m_nHeight * 4;
		originalMipmap.pData = m_pData;

		compressedMipmap.nWidth = m_nWidth;
		compressedMipmap.nHeight = m_nHeight;
		compressedMipmap.nDataSize = nMipSize[nMipLevel];
		compressedMipmap.pData = pMipChainData + nMipOffset;

        unsigned int nRet;
		nRet = Qonvert(&originalMipmap, &compressedMipmap);

		if (nRet != Q_SUCCESS)
		{
			delete[] nMipSize;
			delete[] pMipChainData;

			return false;
		}

        nMipOffset += nMipSize[nMipLevel];
		UINT32 nNewWidth = ( m_nWidth > 1 ) ? m_nWidth >> 1 : 1;
        UINT32 nNewHeight = ( m_nHeight > 1 ) ? m_nHeight >> 1 : 1;

        Resize( nNewWidth, nNewHeight );
    }
    
    // Pack all the mip levels together
    delete[] m_pData;
    m_pData         = pMipChainData;
    m_nMipChainSize = nMipChainSize;
    m_nMinMipSize   = nMipSize[nNumLevels - 1];
    m_nBaseSize     = nMipSize[0];
    m_nPixelFormat  = nNewPixelFormat;
    m_nPixelType    = nNewPixelType;
    m_nWidth        = nWidth;
    m_nHeight       = nHeight;
    m_nRowPitch     = 0;
    m_nNumLevels    = nNumLevels;
    m_nFlags        = IMG_COMPRESSED;

	// Delete mip size table
	delete[] nMipSize;

	// Return successful
    return true;
}


bool CImage::ConvertFormat( IMAGE_PIXEL_FORMAT nNewPixelFormat, IMAGE_PIXEL_TYPE nNewPixelType )
{
    // Check if the image is already in the desired format
    if( m_nPixelFormat == nNewPixelFormat && m_nPixelType == nNewPixelType )
        return true;

	// For SRGBA8 textures, just assign the new format, no image conversion is needed.
	if( m_nPixelFormat == GL_RGBA && m_nPixelType == nNewPixelType && nNewPixelFormat == GL_SRGB8_ALPHA8 )
	{
		m_nPixelFormat = GL_SRGB8_ALPHA8;
		return true;
	}

    // If the image is already compressed, then we can't do anything with it!
    if( m_nPixelType == 0 )
        return false;

    // Convert to a compressed format

    switch( nNewPixelType )
    {
        // Compressed formats have a pixel type of 0
        case 0: 
            return ConvertToCompressedFormat( nNewPixelFormat, nNewPixelType );

        case GL_UNSIGNED_BYTE:
            return ConvertToFixedPointFormat( nNewPixelFormat, nNewPixelType );

        case GL_FLOAT:
            return ConvertToFloatingPointFormat( nNewPixelFormat, nNewPixelType );

        default:
            return false;
    }

    return false;
}

        
bool CImage::ConvertToFixedPointFormat( IMAGE_PIXEL_FORMAT nNewPixelFormat, IMAGE_PIXEL_TYPE nNewPixelType )
{
    // Check if the image is already in the desired format
    if( m_nPixelFormat == nNewPixelFormat && m_nPixelType == nNewPixelType )
        return true;

    // If the image is already compressed, then we can't do anything with it!
    if( m_nPixelType == 0 )
        return false;

    // Step 1: convert src pixels to GL_RGBA/GL_UNSIGNED_BYTE
    {
        BYTE4* pNewDataPtr = NULL;

        if( m_nPixelType == GL_UNSIGNED_BYTE )
        {
            // Nothing needs to be done
        }
        else if( m_nPixelType == GL_FLOAT )
        {
            BYTE4* pNewData = new BYTE4[ m_nWidth * m_nHeight ];
            pNewDataPtr = pNewData;
            
            if( m_nPixelFormat == GL_RGBA )
            {
                FLOAT4* pOldData = (FLOAT4*)m_pData;

                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = (BYTE)( ClampF(pOldData->r) * 255.0f );
                    pNewData->g = (BYTE)( ClampF(pOldData->g) * 255.0f );
                    pNewData->b = (BYTE)( ClampF(pOldData->b) * 255.0f );
                    pNewData->a = (BYTE)( ClampF(pOldData->a) * 255.0f );
                    pNewData++;
                    pOldData++;
                }
            }
            else if( m_nPixelFormat == GL_RGB )
            {
                FLOAT3* pOldData = (FLOAT3*)m_pData;

                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = (BYTE)( ClampF(pOldData->r) * 255.0f );
                    pNewData->g = (BYTE)( ClampF(pOldData->g) * 255.0f );
                    pNewData->b = (BYTE)( ClampF(pOldData->b) * 255.0f );
                    pNewData->a = 255;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( m_nPixelFormat == GL_LUMINANCE_ALPHA )
            {
                FLOAT2* pOldData = (FLOAT2*)m_pData;

                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = (BYTE)(pOldData->l * 255.0f);
                    pNewData->g = (BYTE)(pOldData->l * 255.0f);
                    pNewData->b = (BYTE)(pOldData->l * 255.0f);
                    pNewData->a = (BYTE)(pOldData->a * 255.0f);
                    pNewData++;
                    pOldData++;
                }
            }
            else if( m_nPixelFormat == GL_LUMINANCE )
            {
                FLOAT1* pOldData = (FLOAT1*)m_pData;

                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = (BYTE)(pOldData->l * 255.0f);
                    pNewData->g = (BYTE)(pOldData->l * 255.0f);
                    pNewData->b = (BYTE)(pOldData->l * 255.0f);
                    pNewData->a = 255;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( m_nPixelFormat == GL_ALPHA )
            {
                FLOAT1* pOldData = (FLOAT1*)m_pData;

                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = 0;
                    pNewData->g = 0;
                    pNewData->b = 0;
                    pNewData->a = (BYTE)(pOldData->a * 255.0f);
                    pNewData++;
                    pOldData++;
                }
            }
        }
        else if( m_nPixelType == GL_UNSIGNED_SHORT_4_4_4_4 || 
                 m_nPixelType == GL_UNSIGNED_SHORT_5_5_5_1 || 
                 m_nPixelType == GL_UNSIGNED_SHORT_5_6_5 )
        {
            BYTE4* pNewData = new BYTE4[ m_nWidth * m_nHeight ];
            pNewDataPtr = pNewData;

            USHORT nRedMask,   nRedShift,   nNumRedBits;
            USHORT nGreenMask, nGreenShift, nNumGreenBits;
            USHORT nBlueMask,  nBlueShift,  nNumBlueBits;
            USHORT nAlphaMask, nAlphaShift, nNumAlphaBits;
            
            if( m_nPixelType == GL_UNSIGNED_SHORT_4_4_4_4 )
            {
                nNumRedBits   = 4, nRedMask   = 0x0f00, nRedShift   = 8;
                nNumGreenBits = 4, nGreenMask = 0x00f0, nGreenShift = 4;
                nNumBlueBits  = 4, nBlueMask  = 0x000f, nBlueShift  = 0;
                nNumAlphaBits = 4, nAlphaMask = 0xf000, nAlphaShift = 12;
            }
            else if( m_nPixelType == GL_UNSIGNED_SHORT_5_5_5_1 )
            {
                nNumRedBits   = 5, nRedMask   = 0x7c00, nRedShift   = 10;
                nNumGreenBits = 5, nGreenMask = 0x03e0, nGreenShift = 5;
                nNumBlueBits  = 5, nBlueMask  = 0x001f, nBlueShift  = 0;
                nNumAlphaBits = 1, nAlphaMask = 0x8000, nAlphaShift = 15;
            }
            else // if( m_nPixelType == GL_UNSIGNED_SHORT_5_6_5 )
            {
                nNumRedBits   = 5, nRedMask   = 0xf800, nRedShift   = 10;
                nNumGreenBits = 6, nGreenMask = 0x07e0, nGreenShift = 5;
                nNumBlueBits  = 5, nBlueMask  = 0x001f, nBlueShift  = 0;
                nNumAlphaBits = 0, nAlphaMask = 0x0000, nAlphaShift = 0;
            }

            USHORT* pOldData = (USHORT*)m_pData;

            for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
            {
                // Isolate the bits, and pad out to an 8-bit channel
                UINT32 r = ( ( (*pOldData) & nRedMask   ) >> nRedShift   ) << (8-nNumRedBits   );
                UINT32 g = ( ( (*pOldData) & nGreenMask ) >> nGreenShift ) << (8-nNumGreenBits );
                UINT32 b = ( ( (*pOldData) & nBlueMask  ) >> nBlueShift  ) << (8-nNumBlueBits  );
                UINT32 a = ( ( (*pOldData) & nAlphaMask ) >> nAlphaShift ) << (8-nNumAlphaBits );

                // Replicate the padded bits
                pNewData->r = r | ( r >> nNumRedBits   );
                pNewData->g = g | ( g >> nNumGreenBits );
                pNewData->b = b | ( b >> nNumBlueBits  );
                
                // Special case for 1-bit alpha
                pNewData->a = ( nNumAlphaBits == 1 ) ? ( a ? 255 : 0 ) : a | ( a >> nNumAlphaBits );

                pNewData++;
                pOldData++;
            }
        }

        // Image data is now in GL_RGBA/GL_UNSIGNED_BYTE format
        if( pNewDataPtr )
        {
            delete[] m_pData;
            m_pData         = (BYTE*)pNewDataPtr;
            m_nRowPitch     = m_nWidth * sizeof(BYTE4);
            m_nBaseSize     = m_nRowPitch * m_nHeight;
            m_nMipChainSize = m_nRowPitch * m_nHeight;
            m_nMinMipSize   = m_nRowPitch * m_nHeight;
            m_nPixelFormat  = GL_RGBA;
            m_nPixelType    = GL_UNSIGNED_BYTE;
        }
    }

    // Step 2: Downgrade to the desired format
    {
        BYTE*  pNewDataPtr = NULL;
        UINT32 nSizeOfPixel;

        if( nNewPixelType == GL_UNSIGNED_SHORT_4_4_4_4 ||
            nNewPixelType == GL_UNSIGNED_SHORT_5_5_5_1 ||
            nNewPixelType == GL_UNSIGNED_SHORT_5_5_5_1 )
        {
            BYTE4*  pOldData = (BYTE4*)m_pData;
            USHORT* pNewData = new USHORT[ m_nWidth * m_nHeight ];
            pNewDataPtr  = (BYTE*)pNewData;
            nSizeOfPixel = sizeof(USHORT);

            USHORT nRedMask,   nRedShift,   nNumRedBits;
            USHORT nGreenMask, nGreenShift, nNumGreenBits;
            USHORT nBlueMask,  nBlueShift,  nNumBlueBits;
            USHORT nAlphaMask, nAlphaShift, nNumAlphaBits;

            if( nNewPixelType == GL_UNSIGNED_SHORT_4_4_4_4 )
            {
                nNumRedBits   = 4, nRedMask   = 0x0f00, nRedShift   = 8;
                nNumGreenBits = 4, nGreenMask = 0x00f0, nGreenShift = 4;
                nNumBlueBits  = 4, nBlueMask  = 0x000f, nBlueShift  = 0;
                nNumAlphaBits = 4, nAlphaMask = 0xf000, nAlphaShift = 12;
            }
            if( nNewPixelType == GL_UNSIGNED_SHORT_5_5_5_1 )
            {
                nNumRedBits   = 5, nRedMask   = 0x7c00, nRedShift   = 10;
                nNumGreenBits = 5, nGreenMask = 0x03e0, nGreenShift = 5;
                nNumBlueBits  = 5, nBlueMask  = 0x001f, nBlueShift  = 0;
                nNumAlphaBits = 1, nAlphaMask = 0x8000, nAlphaShift = 15;
            }
            if( nNewPixelType == GL_UNSIGNED_SHORT_5_5_5_1 )
            {
                nNumRedBits   = 5, nRedMask   = 0xf800, nRedShift   = 10;
                nNumGreenBits = 6, nGreenMask = 0x07e0, nGreenShift = 5;
                nNumBlueBits  = 5, nBlueMask  = 0x001f, nBlueShift  = 0;
                nNumAlphaBits = 0, nAlphaMask = 0x0000, nAlphaShift = 0;
            }

            for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
            {
                USHORT r = ( ( pOldData->r >> (8-nNumRedBits)   ) << nRedShift );
                USHORT g = ( ( pOldData->g >> (8-nNumGreenBits) ) << nGreenShift );
                USHORT b = ( ( pOldData->b >> (8-nNumBlueBits)  ) << nBlueShift );
                USHORT a = ( ( pOldData->a >> (8-nNumAlphaBits) ) << nAlphaShift );

                *pNewData = a | r | g | b;
                pNewData++;
                pOldData++;
            }
        }
        else if( nNewPixelType == GL_UNSIGNED_BYTE )
        {
            if( nNewPixelFormat == GL_RGBA )
            {
                // Nothing more needs to be done
            }
            else if( nNewPixelFormat == GL_RGB )
            {
                BYTE4* pOldData = (BYTE4*)m_pData;
                BYTE3* pNewData = new BYTE3[ m_nWidth * m_nHeight ];
                pNewDataPtr  = (BYTE*)pNewData;
                nSizeOfPixel = sizeof(BYTE3);
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = pOldData->r;
                    pNewData->g = pOldData->g;
                    pNewData->b = pOldData->b;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( nNewPixelFormat == GL_LUMINANCE_ALPHA )
            {
                BYTE4* pOldData = (BYTE4*)m_pData;
                BYTE2* pNewData = new BYTE2[ m_nWidth * m_nHeight ];
                pNewDataPtr  = (BYTE*)pNewData;
                nSizeOfPixel = sizeof(BYTE2);
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->l = pOldData->r;
                    pNewData->a = pOldData->a;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( nNewPixelFormat == GL_LUMINANCE )
            {
                BYTE4* pOldData = (BYTE4*)m_pData;
                BYTE1* pNewData = new BYTE1[ m_nWidth * m_nHeight ];
                pNewDataPtr  = (BYTE*)pNewData;
                nSizeOfPixel = sizeof(BYTE1);
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->l = pOldData->r;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( nNewPixelFormat == GL_ALPHA )
            {
                BYTE4* pOldData = (BYTE4*)m_pData;
                BYTE1* pNewData = new BYTE1[ m_nWidth * m_nHeight ];
                pNewDataPtr  = (BYTE*)pNewData;
                nSizeOfPixel = sizeof(BYTE1);
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->a = pOldData->a;
                    pNewData++;
                    pOldData++;
                }
            }
        }

        if( pNewDataPtr )
        {
            delete[] m_pData;
            m_pData         = pNewDataPtr;
            m_nRowPitch     = m_nWidth * nSizeOfPixel;
            m_nBaseSize     = m_nRowPitch * m_nHeight;
            m_nMipChainSize = m_nRowPitch * m_nHeight;
            m_nMinMipSize   = m_nRowPitch * m_nHeight;
            m_nPixelFormat  = nNewPixelFormat;
            m_nPixelType    = nNewPixelType;
        }
    }

    return true;
}


bool CImage::ConvertToFloatingPointFormat( IMAGE_PIXEL_FORMAT nNewPixelFormat, IMAGE_PIXEL_TYPE nNewPixelType )
{
    // Check if the image is already in the desired format
    if (m_nPixelFormat == nNewPixelFormat && m_nPixelType == nNewPixelType)
        return true;

    // If the image is already compressed, then we can't do anything with it!
    if (m_nPixelType == 0)
        return false;

    // This function only converts to GL_FLOAT types
    if (nNewPixelType != GL_FLOAT)
        return false;

    // Step 1: First upgrade the image to GL_RGBA/GL_FLOAT
    {
        BYTE* pNewDataPtr = NULL;

        if( m_nPixelType == GL_FLOAT )
        {
            if( m_nPixelFormat == GL_RGBA )
            {
                // Nothing to do
            }
            else if( m_nPixelFormat == GL_RGB )
            {
                FLOAT3* pOldData = (FLOAT3*)m_pData;
                FLOAT4* pNewData = new FLOAT4[ m_nWidth * m_nHeight ];
                pNewDataPtr = (BYTE*)pNewData;
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = pOldData->r;
                    pNewData->g = pOldData->g;
                    pNewData->b = pOldData->b;
                    pNewData->a = 1.0f;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( m_nPixelFormat == GL_LUMINANCE_ALPHA )
            {
                FLOAT2* pOldData = (FLOAT2*)m_pData;
                FLOAT4* pNewData = new FLOAT4[ m_nWidth * m_nHeight ];
                pNewDataPtr = (BYTE*)pNewData;
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = pOldData->l;
                    pNewData->g = pOldData->l;
                    pNewData->b = pOldData->l;
                    pNewData->a = pOldData->a;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( m_nPixelFormat == GL_LUMINANCE )
            {
                FLOAT1* pOldData = (FLOAT1*)m_pData;
                FLOAT4* pNewData = new FLOAT4[ m_nWidth * m_nHeight ];
                pNewDataPtr = (BYTE*)pNewData;
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = pOldData->l;
                    pNewData->g = pOldData->l;
                    pNewData->b = pOldData->l;
                    pNewData->a = 1.0f;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( m_nPixelFormat == GL_ALPHA )
            {
                FLOAT1* pOldData = (FLOAT1*)m_pData;
                FLOAT4* pNewData = new FLOAT4[ m_nWidth * m_nHeight ];
                pNewDataPtr = (BYTE*)pNewData;
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = 0.0f;
                    pNewData->g = 0.0f;
                    pNewData->b = 0.0f;
                    pNewData->a = pOldData->a;
                    pNewData++;
                    pOldData++;
                }
            }
        }
        else
        {
            // First, upscale to a full 32-bit image
            if (!ConvertFormat(GL_RGBA, GL_UNSIGNED_BYTE))
                return false;

            // Convert to float pixels
            BYTE4* pOldData = (BYTE4*)m_pData;
            FLOAT4* pNewData = new FLOAT4[ m_nWidth * m_nHeight ];
            
            for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
            {
                pNewData->r = pOldData->r / 255.0f;
                pNewData->g = pOldData->g / 255.0f;
                pNewData->b = pOldData->b / 255.0f;
                pNewData->a = pOldData->a / 255.0f;
                pNewData++;
                pOldData++;
            }
        }

        if( pNewDataPtr )
        {
            delete[] m_pData;
            m_pData         = pNewDataPtr;
            m_nRowPitch     = m_nWidth * sizeof(FLOAT4);
            m_nBaseSize     = m_nRowPitch * m_nHeight;
            m_nMipChainSize = m_nRowPitch * m_nHeight;
            m_nMinMipSize   = m_nRowPitch * m_nHeight;
            m_nPixelFormat  = GL_RGBA;
            m_nPixelType    = GL_FLOAT;
        }
    }

    // Step 2: downgrade to the desired pixel format
    {
        BYTE*  pNewDataPtr = NULL;
        UINT32 nSizeOfPixel;

        if( nNewPixelType == GL_FLOAT )
        {
            if( nNewPixelFormat == GL_RGBA )
            {
                // Nothing more needs to be done
            }
            else if( nNewPixelFormat == GL_RGB )
            {
                FLOAT4* pOldData = (FLOAT4*)m_pData;
                FLOAT3* pNewData = new FLOAT3[ m_nWidth * m_nHeight ];
                pNewDataPtr  = (BYTE*)pNewData;
                nSizeOfPixel = sizeof(FLOAT3);
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->r = pOldData->r;
                    pNewData->g = pOldData->g;
                    pNewData->b = pOldData->b;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( nNewPixelFormat == GL_LUMINANCE_ALPHA )
            {
                FLOAT4* pOldData = (FLOAT4*)m_pData;
                FLOAT2* pNewData = new FLOAT2[ m_nWidth * m_nHeight ];
                pNewDataPtr  = (BYTE*)pNewData;
                nSizeOfPixel = sizeof(FLOAT2);
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->l = pOldData->r;
                    pNewData->a = pOldData->a;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( nNewPixelFormat == GL_LUMINANCE )
            {
                FLOAT4* pOldData = (FLOAT4*)m_pData;
                FLOAT1* pNewData = new FLOAT1[ m_nWidth * m_nHeight ];
                pNewDataPtr  = (BYTE*)pNewData;
                nSizeOfPixel = sizeof(FLOAT1);
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->l = pOldData->r;
                    pNewData++;
                    pOldData++;
                }
            }
            else if( nNewPixelFormat == GL_ALPHA )
            {
                FLOAT4* pOldData = (FLOAT4*)m_pData;
                FLOAT1* pNewData = new FLOAT1[ m_nWidth * m_nHeight ];
                pNewDataPtr  = (BYTE*)pNewData;
                nSizeOfPixel = sizeof(FLOAT1);
                
                for( UINT32 i=0; i< m_nWidth * m_nHeight; i++ )
                {
                    pNewData->a = pOldData->a;
                    pNewData++;
                    pOldData++;
                }
            }
        }

        if( pNewDataPtr )
        {
            delete[] m_pData;
            m_pData         = pNewDataPtr;
            m_nRowPitch     = m_nWidth * nSizeOfPixel;
            m_nBaseSize     = m_nRowPitch * m_nHeight;
            m_nMipChainSize = m_nRowPitch * m_nHeight;
            m_nMinMipSize   = m_nRowPitch * m_nHeight;
            m_nPixelFormat  = nNewPixelFormat;
            m_nPixelType    = nNewPixelType;
        }
    }

    return true;
}


//--------------------------------------------------------------------------------------
// Name: LoadImage()
// Desc: 
//--------------------------------------------------------------------------------------
bool LoadImage( const CHAR* strFileName, const CHAR* strFormat,
                   UINT32 nWidth, UINT32 nHeight, BOOL bGenMipMaps,
                   BOOL bFlipImage, UINT32 nRotate,
                   CImage* pImage )
{
    if (!pImage->Load(strFileName))
    {
        return false;
    }

    if( pImage->m_nFlags & IMG_COMPRESSED )
    {
        // We can't override pre-compressed texture images
        if ((strFormat != NULL) || (nWidth != 0) || (nHeight != 0) ||
            (bGenMipMaps != FALSE) || (bFlipImage != FALSE))
          return false;
    }
    else
    {
        // Optionally flip image in Y-direction
		//
		// PGG December 13th 2010: Latest FreeImage upgrade requires that
		// images that aren't flipped be flipped.
		//
		if( !bFlipImage )
            pImage->Flip();

        // Optionally rotate the image
        if( nRotate )
            pImage->Rotate( nRotate );

        if( nWidth  == 0 ) nWidth  = pImage->m_nWidth;
        if( nHeight == 0 ) nHeight = pImage->m_nHeight;

        if( nWidth != pImage->m_nWidth || nHeight != pImage->m_nHeight )
        {
            if (!pImage->Resize( nWidth, nHeight ))
                return false;
        }

        if( bGenMipMaps )
        {
            UINT32 nMaxSize  = max( nWidth, nHeight );
            UINT32 nMaxLevel = (UINT32)(( logf( (FLOAT)nMaxSize ) / logf(2.0f) )+0.5f);
            pImage->m_nNumLevels = nMaxLevel+1;
            float temp = ( logf( (FLOAT)nMaxSize ) / logf(2.0f) )+0.5f;
        }

        // Convert to the desired format
        IMAGE_PIXEL_FORMAT nPixelFormat = pImage->m_nPixelFormat;
        IMAGE_PIXEL_TYPE   nPixelType   = pImage->m_nPixelType;
        if( strFormat )
        {
            UINT32 i;
            for( i=0; i<g_nNumPixelFormats; i++ )
            {
                if( !strcmp( strFormat, g_PixelFormats[i].strName ) )
                {
                    nPixelFormat = g_PixelFormats[i].nPixelFormat;
                    nPixelType   = g_PixelFormats[i].nPixelType;
                    break;
                }
            }
        }

        if (!pImage->ConvertFormat(nPixelFormat, nPixelType))
            return false;
    }

    return true;
}

