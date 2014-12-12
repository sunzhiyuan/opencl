//--------------------------------------------------------------------------------------
// File: FontMaker.cpp
// Desc: A tool to build font files for use in the Snapdragon SDK samples
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>


//--------------------------------------------------------------------------------------
// Magic ID to identify font files
//--------------------------------------------------------------------------------------
const UINT32 FRM_FONT_FILE_MAGIC_ID = ('F'<<0)|('O'<<8)|('N'<<16)|('T'<<24);


//--------------------------------------------------------------------------------------
// Name: struct FRM_FONT_GLYPH
// Desc: Texture coordinates and ABC widths needed to render a single glyph. The font
//       data file contains an array of these structures.
//--------------------------------------------------------------------------------------
struct FRM_FONT_GLYPH
{
    UINT16 u0, v0;
    UINT16 u1, v1;
    INT16  nOffset;
    INT16  nWidth;
    INT16  nAdvance;
};


//--------------------------------------------------------------------------------------
// Name: class CFontMaker
// Desc: The workhouse class that builds the font texture and data files
//--------------------------------------------------------------------------------------
class CFontMaker
{
public:
    HRESULT Create( LOGFONT* pLogFont, BOOL bOutline );
    
    HRESULT SaveImageFile( const CHAR* strFileName );
    HRESULT SaveDataFile( const CHAR* strFileName );
    HRESULT SaveResourceFile( const CHAR* strFileName,
                              const CHAR* strFontFilePrefix, BOOL bCompressed );

    CFontMaker();
    ~CFontMaker();

protected:
    FRM_FONT_GLYPH m_pGlyphs[256];

    UINT32  m_nFontHeight;
    
    UINT32  m_nImageWidth;
    UINT32  m_nImageHeight;
    UINT32* m_pImageBits;
};


//--------------------------------------------------------------------------------------
// Name: CFontMaker()
// Desc: The constructor
//--------------------------------------------------------------------------------------
CFontMaker::CFontMaker()
{
    m_nImageWidth  = 512;
    m_nImageHeight = 2048;
    m_pImageBits   = NULL;
}


//--------------------------------------------------------------------------------------
// Name: ~CFontMaker()
// Desc: The destructor
//--------------------------------------------------------------------------------------
CFontMaker::~CFontMaker()
{
    delete[] m_pImageBits;
}


//--------------------------------------------------------------------------------------
// Name: SaveDataFile()
// Desc: Creates the font data file
//--------------------------------------------------------------------------------------
HRESULT CFontMaker::SaveDataFile( const CHAR* strFileName )
{
    // Create the file
    FILE* file = fopen( strFileName, "wb" );
    if( NULL == file )
        return E_FAIL;

    // Write out the magic ID
    UINT32 nMagicID = FRM_FONT_FILE_MAGIC_ID;
    fwrite( &nMagicID, sizeof(nMagicID), 1, file );

    // Write out the font height
    fwrite( &m_nFontHeight, sizeof(m_nFontHeight), 1, file );

    // Write out the array of font glyphs
    fwrite( m_pGlyphs, sizeof(FRM_FONT_GLYPH), 256, file );

    fclose( file );
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: SaveResourceFile()
// Desc: Create the xml file use to pack font resources with the ResourcePacker tool
//--------------------------------------------------------------------------------------
HRESULT CFontMaker::SaveResourceFile( const CHAR* strFileName,
                                      const CHAR* strFontFilePrefix,
                                      BOOL bCompressed )
{
    // Create the file
    FILE* file = fopen( strFileName, "w" );
    if( NULL == file )
        return E_FAIL;

    // Write out the XML header
    fprintf( file, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n" );
    fprintf( file, "\n" );
    
    // Write out the font image file as a texture resource
    fprintf( file, "<Texture\n" );
    fprintf( file, "    ResourceID  = \"FontTexture\"\n" );
    fprintf( file, "    SrcFileName = \"%s.tga\"\n", strFontFilePrefix );
    if( bCompressed )
        fprintf( file, "    DstFormat   = \"ATC\"\n" );
    fprintf( file, "/>\n" );
    fprintf( file, "\n" );

    // Write out the font data file as a binary data resource
    fprintf( file, "<BinaryData\n" );
    fprintf( file, "    ResourceID  = \"FontData\"\n" );
    fprintf( file, "    SrcFileName = \"%s.font\"\n", strFontFilePrefix );
    fprintf( file, "/>\n" );

    fclose( file );
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: WriteTargaFile()
// Desc: Helper function to save a 32-bit image as a .TGA file
//--------------------------------------------------------------------------------------
HRESULT WriteTargaFile( const CHAR* strFileName, UINT32 nWidth, UINT32 nHeight, 
                        UINT32* pPixels )
{
    // Description of a .TGA file header
    struct TARGA_HEADER
    {
        UINT8  IDLength;
        UINT8  ColormapType;
        UINT8  ImageType;
        UINT8  ColormapSpecification[5];
        UINT16 XOrigin;
        UINT16 YOrigin;
        UINT16 ImageWidth;
        UINT16 ImageHeight;
        UINT8  PixelDepth;
        UINT8  ImageDescriptor;
    };

    // Create the file
    FILE* file = fopen( strFileName, "wb" );
    if( NULL == file )
        return E_FAIL;

    // Write the .TGA file header
    TARGA_HEADER header;
    ZeroMemory( &header, sizeof(header) );
    header.IDLength        = 0;
    header.ImageType       = 2;
    header.ImageWidth      = nWidth;
    header.ImageHeight     = nHeight;
    header.PixelDepth      = 32;
    header.ImageDescriptor = 0x28;
    fwrite( &header, sizeof(header), 1, file );

    // Write the 32-bit pixels
    fwrite( pPixels, sizeof(pPixels[0]), nHeight * nWidth, file );

    fclose( file );
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: SaveImageFile()
// Desc: Creates the font image file, saving it as a 32-bit .TGA file
//--------------------------------------------------------------------------------------
HRESULT CFontMaker::SaveImageFile( const CHAR* strFileName )
{
    // First, convert the image to final colors.
    // Input is red text with a black outline, on a blue backround
    // Output is white, outlined text on a transparent black backgroun
    UINT32* pPixels = new UINT32[m_nImageWidth*m_nImageHeight];

    for( UINT32 i=0; i<m_nImageWidth*m_nImageHeight; i++ )
    {
        UINT8 r = ( m_pImageBits[i] >> 16L ) & 0x000000ff;
        UINT8 g = ( m_pImageBits[i] >>  8L ) & 0x000000ff;
        UINT8 b = ( m_pImageBits[i] >>  0L ) & 0x000000ff;

        UINT8 lum;
        UINT8 a;

        if( g == 0xff )
        {
            // Replace a green background with a dark gray outline
            // This is useful for editting the font in an image editor
            lum = 0x40;
            a   = 0;
        }
        else
        {
            // Use the red channel as luminance, 
            // And the blue channel as transparency
            lum = r;
            a   = 0xff - b;
        }

        // Down sample each value to 4-bits (to better control texture compression)
        lum = ((lum>>4)<<4) | ((lum>>4)<<0);
        a   = (  (a>>4)<<4) | (  (a>>4)<<0);

        // Store the adjusted pixel
        pPixels[i] = (a<<24L) | (lum<<16L) | (lum<<8L) | (lum<<0L);
    }

    // Save the image as a 32-bit TGA file
    HRESULT hr = WriteTargaFile( strFileName, m_nImageWidth, m_nImageHeight, pPixels );
    delete[] pPixels;
    return hr;
}


//--------------------------------------------------------------------------------------
// Name: Create()
// Desc: Create the font internal data structures and image data. This function creates
//       the font, and uses it to render all ASCII characters [0..255] into a bitmap.
//       Meanwhile, it records position and spacing information for each glyph.
//--------------------------------------------------------------------------------------
HRESULT CFontMaker::Create( LOGFONT* pLogFont, BOOL bOutline )
{
    // Create the font
    HFONT hFont = CreateFontIndirect( pLogFont );

    // Create a bitmp on which to render the font glyphs
    HDC hDC = CreateCompatibleDC( NULL );
    HBITMAP hBitmap = CreateBitmap( m_nImageWidth, m_nImageHeight, 1, 32, NULL );
    HRGN hRgn = CreateRectRgn( 0, 0, m_nImageWidth, m_nImageHeight );
    SelectObject( hDC, hBitmap );
    SelectClipRgn( hDC, hRgn );

    // Set text drawing properties
    SelectObject( hDC, hFont );
    SetTextAlign( hDC, TA_LEFT|TA_TOP|TA_UPDATECP );
    SetMapMode( hDC, MM_TEXT );
    SetBkMode( hDC, TRANSPARENT );
    SetBkColor( hDC, RGB(0,0,255) );

    // Clear the background (to GREEN, for later image processing)
    RECT rect;
    SetRect( &rect, 0, 0, m_nImageWidth, m_nImageHeight );
    HBRUSH hBrush = (HBRUSH)CreateSolidBrush( RGB(0,255,0) );
    FillRect( hDC, &rect, hBrush );
    DeleteObject( hBrush );

    // Set up a brush for later use (to BLUE, for later image processing)
    hBrush = (HBRUSH)CreateSolidBrush( RGB(0,0,255) );

    // Store the height of the font
    SIZE size;
    GetTextExtentPoint32W( hDC, L"A", 1, &size );
    m_nFontHeight = size.cy;

    // Intial location (x,y) of each rendered glyph
    UINT32 x = 1;
    UINT32 y = 1;

    // Add some padding around each rendered glyph
    UINT32 nFontPadding = 1;
    if( bOutline )
        nFontPadding += 1;

    // Now, render all the entire character set of glyphs into the bitmap
    for( UINT32 i=0; i<256; i++ )
    {
        // Set up a string, for use in the text functions below
        WCHAR str[2] = L"A";
        str[0] = (WCHAR)i;

        // Handle unprintable characters
        WORD wGlyphIndex = 0;
        GetGlyphIndicesW( hDC, str, 1, &wGlyphIndex, GGI_MARK_NONEXISTING_GLYPHS );
        if( wGlyphIndex == 0xffff )
        {
            if( i == 0 )
                str[0] = (WCHAR)0xffff;
            else if( i >= 128 && i < 160 )
                str[0] = (WCHAR)0x0000;
            else
            {
                ZeroMemory( &m_pGlyphs[i], sizeof(m_pGlyphs[i]) );
                continue;
            }
        }

        // For printable characters, get the ABC widths
        ABC abc;
        if( str[0] )
        {
            GetCharABCWidthsW( hDC, str[0], str[0], &abc );
            abc.abcA -= nFontPadding;
            abc.abcB += nFontPadding + nFontPadding;
            abc.abcC -= nFontPadding;
        }
        else
        {
            // Else, reserve space for a custom glpyh
            abc.abcA = 0;
            abc.abcB = m_nFontHeight;
            abc.abcC = 0;

            abc.abcA -= 1;
            abc.abcB += 2;
            abc.abcC -= 1;
        }

        // The width and height of the bounding rectangle
        int w = abc.abcB;
        int h = m_nFontHeight + nFontPadding;

        // If we wrap off the right edge of the bitmap, go to the next line
        if( x + w + 1 >= (int)m_nImageWidth )
        {
            x  = 1;
            y += h + 1;

            // If we run out of room, cry foul
            assert( y + h + 1 < m_nImageHeight );
        }

        // Compute coordinates of where to draw the glyph
        INT32 sx = x;
        INT32 sy = y + 1;

        if( abc.abcA >= 0 )
            x += abc.abcA;
        else
            sx -= abc.abcA;

        // Save off the glyph data structure
        m_pGlyphs[i].u0       = (UINT16)(x+0);
        m_pGlyphs[i].v0       = (UINT16)(y+0);
        m_pGlyphs[i].u1       = (UINT16)(x+w);
        m_pGlyphs[i].v1       = (UINT16)(y+h);
        m_pGlyphs[i].nOffset  = (INT16)(abc.abcA);
        m_pGlyphs[i].nWidth   = (INT16)(abc.abcB);
        m_pGlyphs[i].nAdvance = (INT16)(abc.abcB + abc.abcC);

        // Draw a solid background (in BLUE, for later image processing)
        rect.left   = x;
        rect.top    = y;
        rect.right  = x + w;
        rect.bottom = y + h;
        FillRect( hDC, &rect, hBrush );

        if( str[0] )
        {
            // Draw the text outline (in BLACK, for later image processing)
            if( bOutline )
            {
                SetTextColor( hDC, RGB(0,0,0) );
                MoveToEx( hDC, sx-1, sy-1, NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
                MoveToEx( hDC, sx-1, sy-0, NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
                MoveToEx( hDC, sx-1, sy+1, NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
                MoveToEx( hDC, sx-0, sy-1, NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
                MoveToEx( hDC, sx-0, sy-0, NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
                MoveToEx( hDC, sx-0, sy+1, NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
                MoveToEx( hDC, sx+1, sy-1, NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
                MoveToEx( hDC, sx+1, sy-0, NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
                MoveToEx( hDC, sx+1, sy+1, NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
            }

            // Draw the text (in RED, for later image processing)
            SetTextColor( hDC, RGB(255,0,0) );
            MoveToEx( hDC, sx, sy,   NULL ); ExtTextOutW( hDC, 0, 0, ETO_OPAQUE, NULL, str, 1, NULL );
        }
        else
        {
            // Reserve space for a custom glpyh
            DrawFocusRect( hDC, &rect );
        }

        // Advance the cursor to the next position
        x += w + 1;
    }

    // Access the image bits
    m_pImageBits = new UINT32[ m_nImageWidth * m_nImageHeight ];
    GetBitmapBits( hBitmap, m_nImageWidth*m_nImageHeight*sizeof(UINT32), m_pImageBits );

    // Round up the image size to the smallest power-of-two size that fits the image
    m_nImageHeight = y + m_nFontHeight + nFontPadding + 1;
    if(      m_nImageHeight > 1024 ) m_nImageHeight = 2048;
    else if( m_nImageHeight >  512 ) m_nImageHeight = 1024;
    else if( m_nImageHeight >  256 ) m_nImageHeight =  512;
    else if( m_nImageHeight >  128 ) m_nImageHeight =  256;
    else if( m_nImageHeight >   64 ) m_nImageHeight =  128;
    else                             m_nImageHeight =   64;

    // Cleanup
    DeleteObject( hBrush );
    DeleteObject( hBitmap );
    SelectClipRgn( hDC, NULL );
    DeleteObject( hRgn );
    DeleteDC( hDC );
    DeleteObject( hFont );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: main()
// Desc: Entry point for the tool.
//--------------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    LOGFONT LogFont;
    CHAR    strFont[MAX_PATH] = "";
    LONG    nHeight           = 0;
    BOOL    bOutline          = TRUE;
    BOOL    bCompressed       = TRUE;
    CHAR    strOutputFontImageFileName[MAX_PATH] = "";
    CHAR    strOutputFontDataFileName[MAX_PATH] = "";
    CHAR    strOutputFontResourceFileName[MAX_PATH] = "";

    // If no command line args were specified, display the ChooseFont common dialog
    if( argc == 1 )
    {
        CHOOSEFONT cf;
        ZeroMemory( &cf, sizeof(cf) );
        cf.lStructSize = sizeof(cf);
        cf.hwndOwner = NULL;
        cf.lpLogFont = &LogFont;
        cf.rgbColors;
        cf.Flags     = CF_SCREENFONTS | CF_TTONLY;

        // Call the ChooseFont dialog to build up the LOGFONT structure we want
        if( FALSE == ChooseFont( &cf ) )
            return 0;

        strcpy( strFont, LogFont.lfFaceName );
        nHeight = cf.iPointSize / 10;
    }
    else
    {
        // Parse the command line arguments
        BOOL  bBold       = FALSE;
        BOOL  bItalic     = FALSE;
        BOOL  bPrintUsage = FALSE;
        BOOL  bPrintLogo  = TRUE;

        // The first argument is the font name
        if( argc > 1 )
            strcpy( strFont, argv[1] );

        // The second argument should be the font height
        if( argc > 2 )
            nHeight = atoi( argv[2] );

        // Read in the remaining arguments
        for( INT i=3; i<argc; i++ )
        {
            CHAR* strArg = argv[i];

            if( strArg[0] == '-' || strArg[0] == '/' )
            {
                if( strArg[1] == 'b' ) // To make the font bold
                {
                    bBold = TRUE;
                    continue;
                }
                else if( strArg[1] == 'i' ) // To italicize the font
                {
                    bItalic = TRUE;
                    continue;
                }
                else if( strArg[1] == 'o' ) // To specify an output file
                {
                    strcpy( strOutputFontImageFileName, argv[i] );
                    strcpy( strOutputFontDataFileName,  argv[i] );
                    strcpy( strOutputFontResourceFileName,  argv[i] );
                    i++;
                    continue;
                }
                else if( !stricmp( strArg+1, "nologo" ) ) // To suppress the logo
                {
                    bPrintLogo = FALSE;
                    continue;
                }
            }

            bPrintUsage = TRUE;
            break;
        }

        // Make sure we've got everything we need
        if( strFont[0] == 0 || nHeight < 1 )
            bPrintUsage = TRUE;

        // Print the usage
        if( bPrintUsage )
        {
            printf( "Usage:\n" );
            printf( "  FontMaker FontName [-h FontHeight] [-b] [i] [-o OutputFile] [-nologo]\n" );
            printf( "     FontName        The font face name (e.g. 'Arial').\n" );
            printf( "     FontHeight      Height of the font.\n" );
            printf( "     -b              Bold the font.\n" );
            printf( "     -i              Italicize the font.\n" );
            printf( "     -o OutputFile   The output file prefix.\n" );
            printf( "     -nologo         Suppress printing the logo.\n" );
            printf( "\n" );
            printf( "The specified font will be rendered into a texture image, saved as\n" );
            printf( "OUTPUTFILE.TGA. Additionally, per-glyph information needed to render\n" );
            printf( "text in-game using the font is saved in OUTPUTFILE.FONT.\n" );
            return 1;
        }

        // Print logo
        if( bPrintLogo )
            printf( "Adreno Font Maker\n" );

        // Build the LOGFONT structure
        ZeroMemory( &LogFont, sizeof(LogFont) );
        LogFont.lfHeight         = -( nHeight * 96 ) / 72;
        LogFont.lfWeight         = bBold ? 700 : 400;
        LogFont.lfItalic         = bItalic ? 255 : 0;
        LogFont.lfQuality        = ANTIALIASED_QUALITY;
        strcpy( LogFont.lfFaceName, strFont );
    }

    // If the output files were not specified above, build them
    CHAR strFontFilePrefix[80];
    sprintf( strFontFilePrefix, "%s%d", strFont, nHeight );
    if( strOutputFontImageFileName[0] == 0 )
    {
        sprintf( strOutputFontImageFileName,    "%s", strFontFilePrefix );
        sprintf( strOutputFontDataFileName,     "%s", strFontFilePrefix );
        sprintf( strOutputFontResourceFileName, "%s", strFontFilePrefix );
    }
    strcat( strOutputFontImageFileName,    ".tga" );
    strcat( strOutputFontDataFileName,     ".font" );
    strcat( strOutputFontResourceFileName, ".xml" );

    // Create the font data structures and image
    CFontMaker Font;
    if( FAILED( Font.Create( &LogFont, bOutline ) ) )
        return -1;

    // Save the font's image file
    if( FAILED( Font.SaveImageFile( strOutputFontImageFileName ) ) )
        return -1;

    // Save the font's data file
    if( FAILED( Font.SaveDataFile( strOutputFontDataFileName ) ) )
        return -1;

    // Save the font's resource file
    if( FAILED( Font.SaveResourceFile( strOutputFontResourceFileName,
                                       strFontFilePrefix, bCompressed ) ) )
        return -1;

    return 0;
}

