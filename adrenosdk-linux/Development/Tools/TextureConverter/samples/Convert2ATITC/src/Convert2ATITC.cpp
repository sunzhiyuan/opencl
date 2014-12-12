/*=============================================================================
FILE: Convert2ATITC.c 

DESCRIPTION: 
    Convert an image file into an ATI TC compressed texture file (ATITC)
  
    This sample shows how to use Qualcomm's Texture Conversion and Compression 
    Library "TextureConverter" to convert an image file into a texture that
    is optimized for any of Qualcomm's 3D Graphics Platforms.
    
    The source code sample demonstrates how to create a command line utility
    to convert an image file into a raw ATITC compressed texture file.

    This sample can easily be extended or can serve to show how to integrate
    TextureConverter into a content creation pipeline.

	Uses FreeImage to convert into (ensure that freeimage.dll is included in
	the same directory as the .exe).

USAGE:
    convert2ATITC.exe <infile> <outfile> <outwidth> <outheight> 

    <infile>    defaults to "texture_128x128_rgba.tga"
    <outfile>   defaults to "texture_128x128_rgba_atitc_mipmapped.raw"
   	<outwidth>  defaults to 0 (uses the width of <inputfile>
	<outheight> defaults to 0 (uses the width of <inputfile>

	NOTE:		Other arguments, such as output type, compression scale, verbose mode etc. 
				will be added in the future

	AUTHOR:		QUALCOMM
                Copyright (c) 2007,2009,2011 QUALCOMM Incorporated.
                All Rights Reserved.
                QUALCOMM Proprietary/GTDR
=============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#ifdef _WIN32
#include <conio.h>
#endif

#include <FreeImage.h>

#include <TextureConverter.h>

//=============================================================================
// MACROS
//=============================================================================

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

//=============================================================================
// ERROR HANDLING FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
void Error(const char* pszErrorString)
//-----------------------------------------------------------------------------
{
    if (pszErrorString != NULL)
        fprintf(stderr, "ERROR: %s\n", pszErrorString);

#ifdef _WIN32
    fprintf(stderr, "\n<press a key>\n");
    _getch();
#endif

    exit(0);
}

//-----------------------------------------------------------------------------
void UsageMessage(const char* pszErrorString)
//-----------------------------------------------------------------------------
{
    if (pszErrorString != NULL)
	{
        fprintf(stderr, "%s\n\n", pszErrorString);
	}

	fprintf(stderr, "Compress an image file to an ATITC compressed (RAW) output file.\n\n");
    fprintf(stderr, "Usage: \n   convert2ATITC.exe [<infile> <outfile> <outwidth> <outheight> <comparefile>]\n\n");
	fprintf(stderr, "<outwidth> and <outheight> (optional)\n   Allow for changing the dimensions of <outfile>, handy for mipmaps.\n");
	fprintf(stderr, "   Both default to 0, which will use the dimensions of <infile>.\n\n");
	fprintf(stderr, "<comparefile> (optional)\n   Creates a tga file from the compressed outfile for easy viewing (optional).\n");
	fprintf(stderr, "\n");

    Error(NULL);
}

//=============================================================================
// HELPER FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
TQonvertImage* CreateEmptyTexture()
//-----------------------------------------------------------------------------
{
    TQonvertImage* pTexture;

    pTexture = (TQonvertImage*)malloc(sizeof(TQonvertImage));
    memset(pTexture, 0, sizeof(TQonvertImage));
    pTexture->pFormatFlags = (TFormatFlags*)malloc(sizeof(TFormatFlags));
    memset(pTexture->pFormatFlags, 0, sizeof(TFormatFlags));

    return pTexture;
}

//-----------------------------------------------------------------------------
void FreeTexture(TQonvertImage* pTexture)
//-----------------------------------------------------------------------------
{
    free(pTexture->pFormatFlags);
    free(pTexture->pData);
	free(pTexture);
}

//=============================================================================
// MAIN FUNCTIONS
//=============================================================================

//-----------------------------------------------------------------------------
bool LoadImage( const char* strFilename, TQonvertImage* pTexture, int *BPP)
//-----------------------------------------------------------------------------
{
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename( strFilename );

	if( format == FIF_UNKNOWN )
	{
		return false;
	}

	FIBITMAP *pBitmap = NULL;

	if(FreeImage_FIFSupportsReading(format))   
	{
		pBitmap = FreeImage_Load( format, strFilename );
	}

	if( NULL == pBitmap )
	{
		return false;
	}

	FREE_IMAGE_TYPE ImageType = FreeImage_GetImageType( pBitmap );

	int nBPP = FreeImage_GetBPP( pBitmap );

	// NOTE: We must always load an image into a 32 bit format to convert from.
	// Therefore, cache the input BPP so that we can specify how to compress the 
	// output ATITC file to (either RGB or RGBA)
	*BPP = nBPP;
	
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

	pTexture->nFormat      = Q_FORMAT_RGBA_8UI;	// always set to an RGBA input
	pTexture->nWidth       = FreeImage_GetWidth( pBitmap );
	pTexture->nHeight      = FreeImage_GetHeight( pBitmap );
	int      m_RowPitch    = pTexture->nWidth * ( nBPP / 8 );
	pTexture->nDataSize    = m_RowPitch * pTexture->nHeight;
	pTexture->pData        = (unsigned char*)malloc(pTexture->nDataSize);

	FreeImage_ConvertToRawBits( pTexture->pData, 
								pBitmap, 
								m_RowPitch, 
								nBPP, 
								FI_RGBA_RED_MASK, 
								FI_RGBA_GREEN_MASK, 
								FI_RGBA_BLUE_MASK,
								TRUE );

	FreeImage_Unload( pBitmap );

	// FreeImage outputs only in BGRA format, so we need to do this conversion to RGBA
	unsigned char* pRed  = pTexture->pData + 2;
	unsigned char* pBlue = pTexture->pData + 0;

	for (unsigned int n = 0; n < pTexture->nWidth * pTexture->nHeight; ++n)
	{
		*pRed  = *pRed ^ *pBlue;
		*pBlue = *pRed ^ *pBlue;
		*pRed  = *pRed ^ *pBlue;

		 pRed  += 4;
		 pBlue += 4;
	}

	return true;
}

//-----------------------------------------------------------------------------
void CompressToATITC(TQonvertImage* pTextureIn, TQonvertImage* pTextureOut, 
					 unsigned int Width, unsigned int Height, int BPP)
//-----------------------------------------------------------------------------
{
    unsigned int nRet;

    //Leaving pData = NULL will request that the Qonvert library compute nDataSize.
	pTextureOut->nWidth       = Width;
	pTextureOut->nHeight      = Height;
	pTextureOut->nFormat      = BPP == 24 ? Q_FORMAT_ATITC_RGB : Q_FORMAT_ATITC_RGBA;
	pTextureOut->nDataSize    = 0;
	pTextureOut->pData        = NULL; 

	//first pass is to get the nDataSize of the output buffer
	nRet = Qonvert(pTextureIn, pTextureOut);

	if (nRet != Q_SUCCESS || pTextureOut->nDataSize == 0)
	{
		Error("Error: Occurred in TextureConverter library, possibly due to unsupported conversion options");
	}

	//create the output buffer
	pTextureOut->pData = (unsigned char*)malloc(pTextureOut->nDataSize);

    // Add a TFormatFlags to specify special processing that should be done on image
	TFormatFlags formatFlags;
    memset(&formatFlags, 0, sizeof(TFormatFlags));

	// If desired, flip the image here in X and/or Y
	formatFlags.nFlipX = 0;
    formatFlags.nFlipY = 0;

    pTextureIn->pFormatFlags = &formatFlags;

	nRet = Qonvert(pTextureIn, pTextureOut);

	pTextureIn->pFormatFlags = NULL;

	if (nRet != Q_SUCCESS)
	{
		Error("Error: Occurred in second pass through TextureConverter library, panic!");
	}
}


//-----------------------------------------------------------------------------
void DecompressFromATITC(TQonvertImage* pTextureIn, 
						 TQonvertImage* pTextureOut, 
						 unsigned int Width, unsigned int Height, int BPP)
//-----------------------------------------------------------------------------
{
	unsigned int nRet;

	// Leaving pData = NULL will request that the Qonvert library compute nDataSize.
	pTextureOut->nWidth       = Width;
	pTextureOut->nHeight      = Height;
	pTextureOut->nFormat      = BPP == 24 ? Q_FORMAT_RGB_888 : Q_FORMAT_RGBA_8UI;
	pTextureOut->nDataSize    = 0;
	pTextureOut->pData        = NULL; 

	// first pass is to get the nDataSize of the output buffer
	nRet = Qonvert(pTextureIn, pTextureOut);

	if (nRet != Q_SUCCESS || pTextureOut->nDataSize == 0)
	{
		Error("Error: Occurred in TextureConverter library, possibly due to unsupported conversion options");
	}

	//create the output buffer
	pTextureOut->pData = (unsigned char*)malloc(pTextureOut->nDataSize);

	nRet = Qonvert(pTextureIn, pTextureOut);

	if (nRet != Q_SUCCESS)
	{
		Error("Error: Occurred in second pass through TextureConverter library, panic!");
	}
}

//-----------------------------------------------------------------------------
void SaveImageTGA( const char* pszOutputFileName, TQonvertImage* pTexture, int BPP)
//-----------------------------------------------------------------------------
{
	if(!pszOutputFileName || !pTexture)
	{
		Error("Error: Input to SaveImageTGA not valid.");
		return;
	}

	int bytesPerPixel = ( BPP / 8 );
	int RowPitch = pTexture->nWidth * bytesPerPixel;

	// FreeImage assumes BGRA format, so we need to do this conversion
	unsigned char* pRed  = pTexture->pData + 2;
	unsigned char* pBlue = pTexture->pData + 0;

	for( unsigned int n = 0; n < pTexture->nWidth * pTexture->nHeight; ++n )
	{
		*pRed  = *pRed ^ *pBlue;
		*pBlue = *pRed ^ *pBlue;
		*pRed  = *pRed ^ *pBlue;

		 pRed  += bytesPerPixel;
		 pBlue += bytesPerPixel;
	}
	
	FIBITMAP *pBitmap = FreeImage_ConvertFromRawBits( 
									pTexture->pData, 
									pTexture->nWidth, 
									pTexture->nHeight, 
									RowPitch, 
									BPP, 
									FI_RGBA_RED_MASK, 
									FI_RGBA_GREEN_MASK, 
									FI_RGBA_BLUE_MASK, 
									TRUE );

	FreeImage_Save(FIF_TARGA, pBitmap, pszOutputFileName);

	FreeImage_Unload( pBitmap );
}

//-----------------------------------------------------------------------------
void SaveImageRAW(const char* pszOutputFileName, TQonvertImage* pTexture)
//-----------------------------------------------------------------------------
{
    FILE* pFileOut = NULL;
    
#ifdef _WIN32
    fopen_s(&pFileOut, pszOutputFileName, "wb");
#else
    pFileOut = fopen(pszOutputFileName, "wb");
#endif

    if (pFileOut == NULL || pTexture == NULL)
    {
		Error("ERROR: Writing output file failed.");
		return;
	}

    fwrite(pTexture->pData, sizeof(unsigned char), pTexture->nDataSize, pFileOut);

    fclose(pFileOut);
}

//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
//-----------------------------------------------------------------------------
{
// set to 1 to use test images for input
#if 0	
	char*           pszInputFileName     = "texture_128x128_rgb.tga";
	char*           pszOutputFileName    = "texture_128x128_rgb_atitc.raw";
	char*           pszCompareFileName   = "texture_compare.tga";
#else
	char*           pszInputFileName     = NULL;
	char*           pszOutputFileName    = NULL;
	char*           pszCompareFileName   = NULL;
#endif

	TQonvertImage*  pInputTexture        = CreateEmptyTexture();
    TQonvertImage*  pOutputTexture       = CreateEmptyTexture();
	TQonvertImage*  pCompareTexture      = CreateEmptyTexture();

	unsigned int    Width  = 0;
	unsigned int    Height = 0;

	// Use this to carry through the Bits Per Pixel size (either 24 or 32)
	int BPP = 0;

    // check command line arguments. 
	fprintf(stderr, "Image to ATITC converter (c)2011 QUALCOMM Incorporated\n\n");

    if (argc > 1)
	{
    	pszInputFileName =	argv[1];
	}
    if (argc > 2)
    {
	    pszOutputFileName = argv[2];
    }
	if (argc > 3)
	{
		Width =	atoi(argv[3]);
	}
	if (argc > 4)
	{
		Height = atoi(argv[4]);
	}

	if (argc > 5)
	{
		pszCompareFileName = argv[5];
	}

	// Load the input texture
	if(LoadImage(pszInputFileName, pInputTexture, &BPP) == false)
	{
		UsageMessage("ERROR: Input image could not be loaded.");
	}

	// Cap dimensions to be the same as input image
	if(Width <= 0)
	{
		Width = pInputTexture->nWidth;
	}

	if (Height <= 0) 
	{
		Height = pInputTexture->nHeight;
	}

    // Compress the input texture into the output texture
	CompressToATITC(pInputTexture, pOutputTexture, Width, Height, BPP);

    // Write the texture to the RAW output file
    SaveImageRAW(pszOutputFileName, pOutputTexture);

	// Optional: Write compressed file to a Targa file for viewing
	if(pszCompareFileName != NULL)
	{
		DecompressFromATITC(pOutputTexture, pCompareTexture, Width, Height, BPP);

		// Save image to tga
		SaveImageTGA(pszCompareFileName, pCompareTexture, BPP);
	}

	// Cleanup
    FreeTexture(pInputTexture);
	FreeTexture(pOutputTexture);
	FreeTexture(pCompareTexture);

    return 0;
}
