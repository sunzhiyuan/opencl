//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <FrmFile.h>
#include <FrmNoise.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <OpenGLES/FrmUtilsGLES.h>
#include "Scene.h"
#include <stdio.h>

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "ImageEffects" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_strEffectName = g_pEffectNames[ EFFECT_NONE ];

    m_nEffectIndex = 0;
    m_bShowLine    = FALSE;

    m_hTextureShader             = 0;
    m_hStaticShader              = 0;
    m_hNoiseShader               = 0;
    m_hBlurShader                = 0;
    m_hSharpenShader             = 0;
    m_hSepiaShader               = 0;
    m_hSobelShader               = 0;
    m_hEmbossShader              = 0;
    m_hLensShader                = 0;
    m_hWarpShader                = 0;

    m_fStepSize     = 1.0f / 512.0f;
    m_fHalfStepSize = m_fStepSize / 2.0f;

    m_pTexture         = NULL;
    m_pTextureWrap     = NULL;
    m_pSepiaTexture    = NULL;
    m_pNoiseTexture2D  = NULL;
    m_pRandomTexture3D = NULL;
    m_pNoiseTexture3D  = NULL;
    m_pLogoTexture     = NULL;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "g_vVertex", 0 }
    };
    const UINT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

    // Create the Texture shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Texture.vs",
                                                      "Samples/Shaders/Texture.fs",
                                                      &m_hTextureShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
    }

    // Create the Noise shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Noise.vs",
                                                      "Samples/Shaders/Noise.fs",
                                                      &m_hNoiseShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_nNoiseImageTextureSlot  = glGetUniformLocation( m_hNoiseShader, "g_sImageTexture" );
        m_nNoiseTextureSlot       = glGetUniformLocation( m_hNoiseShader, "g_sNoiseTexture" );
        m_nNoiseScaleSlot         = glGetUniformLocation( m_hNoiseShader, "g_fNoiseScale" );
    }

    // Create the Blur shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Blur.vs",
                                                      "Samples/Shaders/Blur.fs",
                                                      &m_hBlurShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_nBlurStepSizeXSlot = glGetUniformLocation( m_hBlurShader, "g_fStepSizeX" );
        m_nBlurStepSizeYSlot = glGetUniformLocation( m_hBlurShader, "g_fStepSizeY" );
    }

    // Create the Sharpen shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Sharpen.vs",
                                                      "Samples/Shaders/Sharpen.fs",
                                                      &m_hSharpenShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_nSharpenStepSizeXSlot = glGetUniformLocation( m_hSharpenShader, "g_fStepSizeX" );
        m_nSharpenStepSizeYSlot = glGetUniformLocation( m_hSharpenShader, "g_fStepSizeY" );
    }

    // Create the Static shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Static.vs",
                                                      "Samples/Shaders/Static.fs",
                                                      &m_hStaticShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_nStaticImageTextureSlot  = glGetUniformLocation( m_hStaticShader, "g_sImageTexture" );
        m_nStaticRandomTextureSlot = glGetUniformLocation( m_hStaticShader, "g_sRandomTexture" );
        m_nStaticScaleSlot         = glGetUniformLocation( m_hStaticShader, "g_fStaticScale" );
        m_nStaticFrequencySlot     = glGetUniformLocation( m_hStaticShader, "g_fFrequency" );
        m_nStaticTimeSlot          = glGetUniformLocation( m_hStaticShader, "g_fTime" );
    }

    // Create the Sepia shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Sepia.vs",
                                                      "Samples/Shaders/Sepia.fs",
                                                      &m_hSepiaShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_nSepiaImageTextureSlot  = glGetUniformLocation( m_hSepiaShader, "g_sImageTexture" );
        m_nSepiaLookupTextureSlot = glGetUniformLocation( m_hSepiaShader, "g_sLookupTexture" );
    }

    // Create the Sobel shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Sobel.vs",
                                                      "Samples/Shaders/Sobel.fs",
                                                      &m_hSobelShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_nSobelHalfStepSizeXSlot = glGetUniformLocation( m_hSobelShader, "g_fHalfStepSizeX" );
        m_nSobelHalfStepSizeYSlot = glGetUniformLocation( m_hSobelShader, "g_fHalfStepSizeY" );
    }

    // Create the Emboss shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Emboss.vs",
                                                      "Samples/Shaders/Emboss.fs",
                                                      &m_hEmbossShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_nEmbossHalfStepSizeXSlot = glGetUniformLocation( m_hEmbossShader, "g_fHalfStepSizeX" );
        m_nEmbossHalfStepSizeYSlot = glGetUniformLocation( m_hEmbossShader, "g_fHalfStepSizeY" );
    }

    // Create the Lens shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Lens.vs",
                                                      "Samples/Shaders/Lens.fs",
                                                      &m_hLensShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
        m_nLensPositionSlot = glGetUniformLocation( m_hLensShader, "g_vPosition" );
    }

    // Create the Warp shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Warp.vs",
                                                      "Samples/Shaders/Warp.fs",
                                                      &m_hWarpShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
        m_nWarpImageTextureSlot = glGetUniformLocation( m_hWarpShader, "g_sImageTexture" );
        m_nWarpNoiseTextureSlot = glGetUniformLocation( m_hWarpShader, "g_sNoiseTexture" );
        m_nWarpTimeSlot         = glGetUniformLocation( m_hWarpShader, "g_fTime" );
        m_nWarpScaleSlot        = glGetUniformLocation( m_hWarpShader, "g_fWarpScale" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: CreateRandomTexture3D()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateRandomTexture3D( UINT32 nDim )
{
    UINT32 nNumTexels = nDim * nDim * nDim;
    UINT8* pTexels    = new UINT8[ nNumTexels ];
    UINT32 nIndex     = 0;

    // Create a 3D texture of random values
    for( UINT32 nDepth = 0; nDepth < nDim; ++nDepth )
    {
        UINT32* pFilePixels = new UINT32[ nDim * nDim ];
        BYTE*   pPixelWriteData = (BYTE*)pFilePixels;
    
        for( UINT32 nCol = 0; nCol < nDim; ++nCol )
        {
            for( UINT32 nRow = 0; nRow < nDim; ++nRow )
            {
                UINT8 nRandNum = (UINT8)( 256.0f * FrmRand() );
                pTexels[ nIndex++ ] = nRandNum;

                *pPixelWriteData++ = nRandNum;
                *pPixelWriteData++ = 0;
                *pPixelWriteData++ = 0;
                *pPixelWriteData++ = 255;
            }
        }

        CHAR strFileName[80];
        FrmSprintf( strFileName, 80, "Random3D_%03d.tga", nDepth );
        FrmSaveImageAsTGA( strFileName, nDim, nDim, pFilePixels );
        delete[] pFilePixels; 
    }

    BOOL bResult = FrmCreateTexture3D( nDim, nDim, nDim, 1, GL_LUMINANCE,
                                       GL_UNSIGNED_BYTE, 0, pTexels, nNumTexels * sizeof(UINT8),
                                       &m_pRandomTexture3D );
    glTexParameteri( GL_TEXTURE_3D_OES, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    delete[] pTexels;
    return bResult;
}


//--------------------------------------------------------------------------------------
// Name: CreateNoiseTexture2D()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateNoiseTexture2D( UINT32 nDim )
{
    UINT32 nNumTexels = nDim * nDim;
    UINT8* pTexels    = new UINT8[ nNumTexels ];
    UINT32 nIndex     = 0;

    CFrmPerlin NoiseGenerator;

    CHAR strFileName[80];
    FrmSprintf( strFileName, 80, "Noise2D.pfm" );

#ifndef UNDER_CE
    FRM_FILE* pFile;
#else
    FILE* pFile;
#endif // UNDER_CE
	FrmFile_Open( strFileName, FRM_FILE_WRITE, &pFile );

    CHAR strPfmHeader[80];
    FrmSprintf( strPfmHeader, 80, "PF\n%d %d\n1.0\n", nDim, nDim );
    FrmFile_Write( pFile, strPfmHeader, FrmStrlen(strPfmHeader) );

    for( UINT32 nCol = 0; nCol < nDim; ++nCol )
    {
        for( UINT32 nRow = 0; nRow < nDim; ++nRow )
        {
            FLOAT32 fNoise = NoiseGenerator.Turbulence2D( (FLOAT32)nRow / (FLOAT32)nDim,
                                                          (FLOAT32)nCol / (FLOAT32)nDim,
                                                          50.0f, 4 );

            fNoise = 0.5f * fNoise + 0.5f; 
            FrmFile_Write( pFile, &fNoise, sizeof(FLOAT32) );
            FrmFile_Write( pFile, &fNoise, sizeof(FLOAT32) );
            FrmFile_Write( pFile, &fNoise, sizeof(FLOAT32) );

            pTexels[ nIndex++ ] = (UINT8)( 256.0f * fNoise );
        }
    }

    FrmFile_Close( pFile );

    BOOL bResult = FrmCreateTexture( nDim, nDim, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE, 
                                     0, pTexels, nNumTexels * sizeof(UINT8),
                                     &m_pNoiseTexture2D );
    delete[] pTexels;
    return bResult;
}


//--------------------------------------------------------------------------------------
// Name: CreateNoiseTexture3D()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateNoiseTexture3D( UINT32 nDim )
{
    UINT32 nNumTexels = nDim * nDim * nDim;
    UINT8* pTexels = new UINT8[ nNumTexels * 2 ];
    UINT32 nIndex  = 0;

    CFrmPerlin NoiseGenerator;
    const FLOAT32 fFreq   = 2.0f;
    const FLOAT32 fPeriod1 = 0.0f;
    const FLOAT32 fPeriod2 = 7.2f; // Add some random period

    
    // Create one depth slice at a time
    for( UINT32 nDepth = 0; nDepth < nDim; ++nDepth )
    {
        UINT32* pFilePixels = new UINT32[ nDim * nDim ];
        BYTE*   pPixelWriteData = (BYTE*)pFilePixels;
    
        for( UINT32 nCol = 0; nCol < nDim; ++nCol )
        {
            for( UINT32 nRow = 0; nRow < nDim; ++nRow )
            {
                FLOAT32 fNoise1 = NoiseGenerator.TileableTurbulence3D( ( (FLOAT32)nRow   + fPeriod1 ) / (FLOAT32)nDim,
                                                                       ( (FLOAT32)nCol   + fPeriod1 ) / (FLOAT32)nDim,
                                                                       ( (FLOAT32)nDepth + fPeriod1 ) / (FLOAT32)nDim,
                                                                       1.0f, 1.0f, 1.0f,
                                                                       2.50f, 3, 0.8f, 0.5f );
                FLOAT32 fNoise2 = NoiseGenerator.TileableTurbulence3D( ( (FLOAT32)nRow   + fPeriod2 ) / (FLOAT32)nDim,
                                                                       ( (FLOAT32)nCol   + fPeriod2 ) / (FLOAT32)nDim,
                                                                       ( (FLOAT32)nDepth + fPeriod2 ) / (FLOAT32)nDim,
                                                                       1.0f, 1.0f, 1.0f,
                                                                       2.50f, 3, 0.8f, 0.5f );
                UINT8 nTexel1 = (UINT8)( 256.0f * ( 0.5 * fNoise1 + 0.5f ) );
                UINT8 nTexel2 = (UINT8)( 256.0f * ( 0.5 * fNoise2 + 0.5f ) );

                pTexels[ nIndex++ ] = nTexel1;
                pTexels[ nIndex++ ] = nTexel2;

                *pPixelWriteData++ = nTexel1;
                *pPixelWriteData++ = 0;
                *pPixelWriteData++ = 0;
                *pPixelWriteData++ = nTexel2;
            }
        }

        CHAR strFileName[80];
        FrmSprintf( strFileName, 80, "Noise3D_%03d.tga", nDepth );
        FrmSaveImageAsTGA( strFileName, nDim, nDim, pFilePixels );
        delete[] pFilePixels; 
    }

    BOOL bResult = FrmCreateTexture3D( nDim, nDim, nDim, 1, GL_LUMINANCE_ALPHA,
                                       GL_UNSIGNED_BYTE, 0, pTexels, nNumTexels * 2 * sizeof(UINT8),
                                       &m_pNoiseTexture3D );
    delete[] pTexels;
    return bResult;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{



    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;



    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/ImageEffects.pak" ) )
        return FALSE;


    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Create the sample image texture
    m_pTexture = resource.GetTexture( "Sample Image" );


    // Create the sample image texture
    m_pTextureWrap = resource.GetTexture( "Sample Image Wrap" );


    // Create the sepia texture
    m_pSepiaTexture = resource.GetTexture( "Sepia" );


    // Create the 3D random texture
#ifdef CREATE_TEXTURES
    CreateRandomTexture3D( 64 );
    CreateNoiseTexture2D( 128 );
    CreateNoiseTexture3D( 64 );
#else
    m_pRandomTexture3D = resource.GetTexture3D( "Random3D" );
    m_pNoiseTexture2D  = resource.GetTexture( "Noise2D" );
    m_pNoiseTexture3D  = resource.GetTexture3D( "Noise3D" );
#endif



    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;

    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddStringVariable( &m_strEffectName , "Effect" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Previous Effect" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Next Effect" );



    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    

    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Delete shader programs
    if( m_hTextureShader ) glDeleteProgram( m_hTextureShader );
    if( m_hStaticShader )  glDeleteProgram( m_hStaticShader );
    if( m_hNoiseShader )   glDeleteProgram( m_hNoiseShader );
    if( m_hBlurShader )    glDeleteProgram( m_hBlurShader );
    if( m_hSharpenShader ) glDeleteProgram( m_hSharpenShader );
    if( m_hSepiaShader )   glDeleteProgram( m_hSepiaShader );
    if( m_hSobelShader )   glDeleteProgram( m_hSobelShader );
    if( m_hEmbossShader )  glDeleteProgram( m_hEmbossShader );
    if( m_hLensShader )    glDeleteProgram( m_hLensShader );
    if( m_hWarpShader )    glDeleteProgram( m_hWarpShader );

    // Release textures
    if( m_pTexture )         m_pTexture->Release();
    if( m_pTextureWrap )     m_pTextureWrap->Release();
    if( m_pSepiaTexture )    m_pSepiaTexture->Release();
    if( m_pNoiseTexture2D )  m_pNoiseTexture2D->Release();
    if( m_pRandomTexture3D ) m_pRandomTexture3D->Release();
    if( m_pNoiseTexture3D )  m_pNoiseTexture3D->Release();
    if( m_pLogoTexture )     m_pLogoTexture->Release();

    // Release font
    m_Font.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    m_fElapsedTime = m_Timer.GetTime();

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nEffectIndex = ( m_nEffectIndex - 1 + NUM_EFFECTS ) % NUM_EFFECTS;
        m_strEffectName = g_pEffectNames[ m_nEffectIndex ];
    }

    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_nEffectIndex = ( m_nEffectIndex + 1 + NUM_EFFECTS ) % NUM_EFFECTS;
        m_strEffectName = g_pEffectNames[ m_nEffectIndex ];
    }
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetTextureShader()
{
    glUseProgram( m_hTextureShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetNoiseShader()
{
    glUseProgram( m_hNoiseShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );
    glUniform1i( m_nNoiseImageTextureSlot, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_pNoiseTexture2D->m_hTextureHandle );
    glUniform1i( m_nNoiseTextureSlot, 1 );

    glUniform1f( m_nNoiseScaleSlot, 0.06f );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetBlurShader()
{
    glUseProgram( m_hBlurShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );

    glUniform1f( m_nBlurStepSizeXSlot, m_fStepSize * 2.0f );
    glUniform1f( m_nBlurStepSizeYSlot, m_fStepSize * 2.0f );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetSharpenShader()
{
    glUseProgram( m_hSharpenShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );

    glUniform1f( m_nSharpenStepSizeXSlot, m_fStepSize );
    glUniform1f( m_nSharpenStepSizeYSlot, m_fStepSize );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetStaticShader()
{
    glUseProgram( m_hStaticShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );
    glUniform1i( m_nStaticImageTextureSlot, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_3D_OES, m_pRandomTexture3D->m_hTextureHandle );
    glUniform1i( m_nStaticRandomTextureSlot, 1 );

    glUniform1f( m_nStaticScaleSlot, 0.2f );
    glUniform1f( m_nStaticFrequencySlot, 4.0f );

    glUniform1f( m_nStaticTimeSlot, m_fElapsedTime );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
 VOID CSample::SetSepiaShader()
 {
    glUseProgram( m_hSepiaShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );
    glUniform1i( m_nSepiaImageTextureSlot, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_pSepiaTexture->m_hTextureHandle );
    glUniform1i( m_nSepiaLookupTextureSlot, 1 );
 }


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetSobelShader()
 {
    glUseProgram( m_hSobelShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );

    glUniform1f( m_nSobelHalfStepSizeXSlot, m_fHalfStepSize );
    glUniform1f( m_nSobelHalfStepSizeYSlot, m_fHalfStepSize );
 }


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetEmbossShader()
{
    glUseProgram( m_hEmbossShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );

    glUniform1f( m_nEmbossHalfStepSizeXSlot, m_fHalfStepSize );
    glUniform1f( m_nEmbossHalfStepSizeYSlot, m_fHalfStepSize );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetLensShader()
{
    glUseProgram( m_hLensShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );

    FLOAT32 fRate = m_fElapsedTime * 1.5f;
    FRMVECTOR2 vLensPos = 0.6f * FRMVECTOR2( FrmCos( fRate ), FrmSin( fRate ) );
    glUniform2fv( m_nLensPositionSlot, 1, ( FLOAT32* )&vLensPos );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetWarpShader()
{
    glUseProgram( m_hWarpShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureWrap->m_hTextureHandle );
    glUniform1i( m_nWarpImageTextureSlot, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_3D_OES, m_pNoiseTexture3D->m_hTextureHandle );
    glUniform1i( m_nWarpNoiseTextureSlot, 1 );

    FLOAT32 fRate = m_fElapsedTime * 0.3f;
    glUniform1f( m_nWarpTimeSlot, fRate ); 

    glUniform1f( m_nWarpScaleSlot, 0.4f );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glDisable( GL_DEPTH_TEST );

    // Draw the scene
    {
        // Set the texture and blend state
        glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );
        glDisable( GL_BLEND );

        // Set the shader
        switch( m_nEffectIndex )
        {
            case EFFECT_NONE:
                SetTextureShader();
                break;
            case EFFECT_NOISE:
                SetNoiseShader();
                break;
            case EFFECT_BLUR:
                SetBlurShader();
                break;
            case EFFECT_SHARPEN:
                SetSharpenShader();
                break;
            case EFFECT_STATIC:
                SetStaticShader();
                break;
            case EFFECT_SEPIA:
                SetSepiaShader();
                break;
            case EFFECT_SOBEL:
                SetSobelShader();
                break;
            case EFFECT_EMBOSS:
                SetEmbossShader();
                break;
            case EFFECT_LENS:
                SetLensShader();
                break;
            case EFFECT_WARP:
                SetWarpShader();
                break;
        }

        FLOAT32 fAspectRatio = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
        FLOAT32 w_div_2 = ( fAspectRatio < 1.0f ) ? 1.0f : 1.0f / fAspectRatio;
        FLOAT32 h_div_2 = ( fAspectRatio < 1.0f ) ? fAspectRatio : 1.0f;
        FLOAT32 Quad[] =
        {
            -w_div_2, -h_div_2, 0.0f, 1.0f,
            +w_div_2, -h_div_2, 1.0f, 1.0f,
            +w_div_2, +h_div_2, 1.0f, 0.0f,
            -w_div_2, +h_div_2, 0.0f, 0.0f,
        };
        glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, Quad );
        glEnableVertexAttribArray( 0 );

        // Draw the textured quad
        glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

