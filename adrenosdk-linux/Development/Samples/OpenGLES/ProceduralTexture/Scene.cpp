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
#include <FrmNoise.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include "Scene.h"


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "ProceduralTexture" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_fRotateTime = 0.0f;

    m_pLogoTexture = NULL;

    m_hCheckerShader = 0;
    m_nCheckerModelViewProjectionMatrixSlot = 0;
    m_nCheckerColor1Slot = 0;
    m_nCheckerColor2Slot = 0;
    m_nCheckerFrequencySlot = 0;

    m_hWoodShader = 0;
    m_nWoodModelViewProjectionMatrixSlot = 0;
    m_nWoodKdSlot = 0;
    m_nWoodKsSlot = 0;
    m_nWoodDarkWoodColorSlot = 0;
    m_nWoodLightWoodColorSlot = 0;
    m_nWoodFrequencySlot = 0;
    m_nWoodNoiseScaleSlot = 0;
    m_nWoodLightDirSlot = 0;

    m_hMarbleShader = 0;
    m_nMarbleModelViewProjectionMatrixSlot = 0;
    m_nMarbleKdSlot = 0;
    m_nMarbleKsSlot = 0;
    m_nMarbleColor1Slot = 0;
    m_nMarbleColor2Slot = 0;
    m_nMarbleTurbScaleSlot = 0;
    m_nMarbleLightDirSlot = 0;

    m_hCloudsShader = 0;
    m_nCloudsModelViewProjectionMatrixSlot = 0;
    m_nCloudsScaleSlot = 0;
    m_nCloudsIntensitySlot = 0;
    m_nCloudsColor1Slot = 0;
    m_nCloudsColor2Slot = 0;

    m_pNoiseTexture = 0;
    m_pTurbTexture  = 0;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the checker shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex",   FRM_VERTEX_POSITION }
        };
        const UINT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Checker.vs",
                                                      "Samples/Shaders/Checker.fs",
                                                      &m_hCheckerShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_nCheckerModelViewProjectionMatrixSlot = glGetUniformLocation( m_hCheckerShader,  "g_matModelViewProj" );
        m_nCheckerColor1Slot                    = glGetUniformLocation( m_hCheckerShader,  "g_vColor1" );
        m_nCheckerColor2Slot                    = glGetUniformLocation( m_hCheckerShader,  "g_vColor2" );
        m_nCheckerFrequencySlot                 = glGetUniformLocation( m_hCheckerShader,  "g_fFrequency" );
    }

    // Create the wood shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex",   FRM_VERTEX_POSITION },
            { "g_vNormalES", FRM_VERTEX_NORMAL }
        };
        const UINT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Wood.vs",
                                                      "Samples/Shaders/Wood.fs",
                                                      &m_hWoodShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;
            
        m_nWoodModelViewProjectionMatrixSlot = glGetUniformLocation( m_hWoodShader, "g_matModelViewProj" );
        m_nWoodModelViewMatrixSlot           = glGetUniformLocation( m_hWoodShader, "g_matModelView" );
        m_nWoodNormalMatrixSlot              = glGetUniformLocation( m_hWoodShader, "g_matNormal" );
        m_nWoodScaleSlot                     = glGetUniformLocation( m_hWoodShader, "g_fScale" );
        m_nWoodKdSlot                        = glGetUniformLocation( m_hWoodShader, "g_Kd" );
        m_nWoodKsSlot                        = glGetUniformLocation( m_hWoodShader, "g_Ks" );
        m_nWoodDarkWoodColorSlot             = glGetUniformLocation( m_hWoodShader, "g_vDarkWoodColor" );
        m_nWoodLightWoodColorSlot            = glGetUniformLocation( m_hWoodShader, "g_vLightWoodColor" );
        m_nWoodFrequencySlot                 = glGetUniformLocation( m_hWoodShader, "g_fFrequency" );
        m_nWoodNoiseScaleSlot                = glGetUniformLocation( m_hWoodShader, "g_fNoiseScale" );
        m_nWoodRingScaleSlot                 = glGetUniformLocation( m_hWoodShader, "g_fRingScale" );
        m_nWoodLightDirSlot                  = glGetUniformLocation( m_hWoodShader, "g_vLightDir" );
    }

    // Create the marble shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex",   FRM_VERTEX_POSITION },
            { "g_vNormalES", FRM_VERTEX_NORMAL }
        };
        const UINT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Marble.vs",
                                                      "Samples/Shaders/Marble.fs",
                                                      &m_hMarbleShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;
            
        m_nMarbleModelViewProjectionMatrixSlot = glGetUniformLocation( m_hMarbleShader, "g_matModelViewProj" );
        m_nMarbleModelViewMatrixSlot           = glGetUniformLocation( m_hMarbleShader, "g_matModelView" );
        m_nMarbleNormalMatrixSlot              = glGetUniformLocation( m_hMarbleShader, "g_matNormal" );
        m_nMarbleScaleSlot                     = glGetUniformLocation( m_hMarbleShader, "g_fScale" );
        m_nMarbleKdSlot                        = glGetUniformLocation( m_hMarbleShader, "g_Kd" );
        m_nMarbleKsSlot                        = glGetUniformLocation( m_hMarbleShader, "g_Ks" );
        m_nMarbleColor1Slot                    = glGetUniformLocation( m_hMarbleShader, "g_vColor1" );
        m_nMarbleColor2Slot                    = glGetUniformLocation( m_hMarbleShader, "g_vColor2" );
        m_nMarbleTurbScaleSlot                 = glGetUniformLocation( m_hMarbleShader, "g_fTurbScale" );
        m_nMarbleLightDirSlot                  = glGetUniformLocation( m_hMarbleShader, "g_vLightDir" );
    }

    // Create the clouds shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex",   FRM_VERTEX_POSITION },
        };
        const UINT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Clouds.vs",
                                                      "Samples/Shaders/Clouds.fs",
                                                      &m_hCloudsShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;

            m_nCloudsModelViewProjectionMatrixSlot = glGetUniformLocation( m_hCloudsShader, "g_matModelViewProj" );
            m_nCloudsScaleSlot                     = glGetUniformLocation( m_hCloudsShader, "g_fScale" );
            m_nCloudsIntensitySlot                 = glGetUniformLocation( m_hCloudsShader, "g_fIntensity" );
            m_nCloudsColor1Slot                    = glGetUniformLocation( m_hCloudsShader, "g_vColor1" );
            m_nCloudsColor2Slot                    = glGetUniformLocation( m_hCloudsShader, "g_vColor2" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: CreateNoiseTexture3D()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateNoiseTexture3D( UINT32 nDim )
{
    UINT32   nNumTexels = nDim * nDim * nDim;
    FLOAT32* pTexels  = new FLOAT32[ nNumTexels ];
    UINT32   nIndex   = 0;

    CFrmPerlin NoiseGenerator;

    for( UINT32 nDepth = 0; nDepth < nDim; ++nDepth )
    {
        for( UINT32 nCol = 0; nCol < nDim; ++nCol )
        {
            for( UINT32 nRow = 0; nRow < nDim; ++nRow )
            {
                FLOAT32 fNoise = NoiseGenerator.TileableNoise3D( (FLOAT32)nRow   / (FLOAT32)nDim,
                                                                 (FLOAT32)nCol   / (FLOAT32)nDim,
                                                                 (FLOAT32)nDepth / (FLOAT32)nDim,
                                                                 1.0f, 1.0f, 1.0f );
                pTexels[ nIndex++ ] = fNoise;
            }
        }
    }

    BOOL bResult = FrmCreateTexture3D( nDim, nDim, nDim, 1, GL_LUMINANCE, 
                                       GL_FLOAT, 0, pTexels, nNumTexels * sizeof(FLOAT32), 
                                       &m_pNoiseTexture );
    delete [] pTexels;
    return bResult;
}


//--------------------------------------------------------------------------------------
// Name: CreateTurbTexture3D()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateTurbTexture3D( UINT32 nDim )
{
    UINT32   nNumTexels = nDim * nDim * nDim;
    FLOAT32* pTexels    = new FLOAT32[ nNumTexels ];
    UINT32   nIndex     = 0;

    CFrmPerlin NoiseGenerator;

    for( UINT32 nDepth = 0; nDepth < nDim; ++nDepth )
    {
        for( UINT32 nCol = 0; nCol < nDim; ++nCol )
        {
            for( UINT32 nRow = 0; nRow < nDim; ++nRow )
            {
                FLOAT32 fNoise = NoiseGenerator.TileableTurbulence3D( (FLOAT32)nRow   / (FLOAT32)nDim,
                                                                      (FLOAT32)nCol   / (FLOAT32)nDim,
                                                                      (FLOAT32)nDepth / (FLOAT32)nDim, 
                                                                      1.0f, 1.0f, 1.0f, 
                                                                      2.50f, 3, 0.8f, 0.5f );
                pTexels[ nIndex++ ] = fNoise;
            }
        }
    }

    BOOL bResult = FrmCreateTexture3D( nDim, nDim, nDim, 1, GL_LUMINANCE, 
                                       GL_FLOAT, 0, pTexels, nNumTexels * sizeof(FLOAT32), 
                                       &m_pTurbTexture );
    delete [] pTexels;
    return bResult;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    m_bShouldRotate = TRUE;
    m_fCheckerFrequency = 30.0f;
    m_fWoodFrequency    = 10.0f;
    m_MarbleTurbScale   = 50.0f;
    m_nPTIndex = PT_CHECKER;

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/ProceduralTexture.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Create the 3D noise texture
    CreateNoiseTexture3D( 128 );

    // Create the 3D turbulence texture
    CreateTurbTexture3D( 64 );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_fCheckerFrequency, "Checker Freq", "%4.2f" );
    m_UserInterface.AddFloatVariable( &m_fWoodFrequency, "Wood Freq", "%4.2f" );
    m_UserInterface.AddFloatVariable( &m_MarbleTurbScale, "Marble Scale", "%4.2f" );
    m_UserInterface.AddBoolVariable( &m_bShouldRotate, "Should Rotate" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Decr Frequency" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Incr Frequency" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Next Procedural Texture" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Toggle Rotation" );
    // Create the mesh
    if( FALSE == m_Mesh.Load( "Samples/Meshes/Bunny.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 0.1f, 10.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

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
    // Free shader memory
    if( m_hCheckerShader ) glDeleteProgram( m_hCheckerShader );
    if( m_hWoodShader )    glDeleteProgram( m_hWoodShader );

    // Free texture memory
    if( m_pLogoTexture )   m_pLogoTexture->Release();
    if( m_pNoiseTexture )  m_pNoiseTexture->Release();
    if( m_pTurbTexture )   m_pTurbTexture->Release();

     // Free mesh memory
    m_Mesh.Destroy();

}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();
    if( m_bShouldRotate )
    {
        m_fRotateTime += fElapsedTime;
    }

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    // Lower frequency
    if( nButtons & INPUT_KEY_1 )
    {
        switch( m_nPTIndex )
        {
            case PT_CHECKER:
                m_fCheckerFrequency -= 4.0f * fElapsedTime;
                break;
            case PT_WOOD:
                m_fWoodFrequency -= 4.0f * fElapsedTime;
                break;
            case PT_MARBLE:
                m_MarbleTurbScale -= 4.0f * fElapsedTime;
                break;
        }
    }

    // Increase frequency
    if( nButtons & INPUT_KEY_2 )
    {
        switch( m_nPTIndex )
        {
            case PT_CHECKER:
                m_fCheckerFrequency += 4.0f * fElapsedTime;
                break;
            case PT_WOOD:
                m_fWoodFrequency += 4.0f * fElapsedTime;
                break;
            case PT_MARBLE:
                m_MarbleTurbScale += 4.0f * fElapsedTime;
                break;
        }
    }

    // Increment shader index
    if( nPressedButtons & INPUT_KEY_3 )
        m_nPTIndex = ( m_nPTIndex + 1 ) % NUM_PTS;

    // Toggle rotation
    if( nPressedButtons & INPUT_KEY_4 )
        m_bShouldRotate = 1 - m_bShouldRotate;

    // Rotate the object
    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.30f, 0.5f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.10f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    FRMMATRIX4X4 matModel = FrmMatrixRotate( m_fRotateTime, vUp );

    // Build the matrices
    m_matModelView     = FrmMatrixMultiply( matModel, m_matView );
    m_matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_matNormal        = FrmMatrixNormal( m_matModelViewProj );
}


//--------------------------------------------------------------------------------------
// Name: SetCheckerShader()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetCheckerShader()
{
	FRMVECTOR4 color1Vec = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
	FRMVECTOR4 color2Vec = FRMVECTOR4( 1.0f, 0.0f, 0.0f, 1.0f );

    glUseProgram( m_hCheckerShader );

    glUniformMatrix4fv( m_nCheckerModelViewProjectionMatrixSlot, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniform4fv( m_nCheckerColor1Slot, 1, (FLOAT32*)&color1Vec );
    glUniform4fv( m_nCheckerColor2Slot, 1, (FLOAT32*)&color2Vec );
    glUniform1f( m_nCheckerFrequencySlot, m_fCheckerFrequency );
}


//--------------------------------------------------------------------------------------
// Name: SetWoodShader()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetWoodShader()
{
	FRMVECTOR4 darkWoodColorVec = FRMVECTOR4( 0.44f, 0.210526f, 0.0f, 1.0f );
	FRMVECTOR4 lightWoodColorVec = FRMVECTOR4( 0.917293f, 0.503759f, 0.12782f, 1.0f );
	FRMVECTOR4 lightWoodDirVec = FRMVECTOR4( 0.0f, -1.0f, 0.0f, 1.0f );

    glUseProgram( m_hWoodShader );

    glUniformMatrix4fv( m_nWoodModelViewProjectionMatrixSlot, 1, FALSE, (FLOAT32*)&m_matModelViewProj );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_3D_OES, m_pNoiseTexture->m_hTextureHandle );

    glUniformMatrix4fv( m_nWoodModelViewMatrixSlot, 1, FALSE, (FLOAT32*)&m_matModelView );
    glUniformMatrix3fv( m_nWoodNormalMatrixSlot, 1, FALSE, (FLOAT32*)&m_matNormal );
    glUniform1f( m_nWoodScaleSlot, 10.0f );//;8.0f );
    glUniform1f( m_nWoodKdSlot, 0.89f );
    glUniform1f( m_nWoodKsSlot, 0.66f );
    glUniform4fv( m_nWoodDarkWoodColorSlot, 1, (FLOAT32*)&darkWoodColorVec );
    glUniform4fv( m_nWoodLightWoodColorSlot, 1, (FLOAT32*)&lightWoodColorVec );
    glUniform1f( m_nWoodFrequencySlot, m_fWoodFrequency );
    glUniform1f( m_nWoodNoiseScaleSlot, 20.0f );
    glUniform1f( m_nWoodRingScaleSlot, 2.2f );
    glUniform4fv( m_nWoodLightDirSlot, 1, (FLOAT32*)&lightWoodDirVec );
}


//--------------------------------------------------------------------------------------
// Name: SetMarbleShader()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetMarbleShader()
{
	FRMVECTOR4 marbleColor1Vec = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
	FRMVECTOR4 marbleColor2Vec = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
	FRMVECTOR4 lightMarbleDirVec = FRMVECTOR4( 0.0f, -1.0f, 0.0f, 1.0f );

    glUseProgram( m_hMarbleShader );

    glUniformMatrix4fv( m_nMarbleModelViewProjectionMatrixSlot, 1, FALSE, (FLOAT32*)&m_matModelViewProj );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_3D_OES, m_pTurbTexture->m_hTextureHandle );

    glUniformMatrix4fv( m_nMarbleModelViewMatrixSlot, 1, FALSE, (FLOAT32*)&m_matModelView );
    glUniformMatrix3fv( m_nMarbleNormalMatrixSlot, 1, FALSE, (FLOAT32*)&m_matNormal );
    glUniform1f( m_nMarbleScaleSlot, 20.0f ); //43 //20.0f );
    glUniform1f( m_nMarbleKdSlot, 0.89f );
    glUniform1f( m_nMarbleKsSlot, 0.66f );
    glUniform4fv( m_nMarbleColor1Slot, 1, (FLOAT32*)&marbleColor1Vec );
    glUniform4fv( m_nMarbleColor2Slot, 1, (FLOAT32*)&marbleColor2Vec );
    glUniform1f( m_nMarbleTurbScaleSlot, m_MarbleTurbScale );//12
    glUniform4fv( m_nMarbleLightDirSlot, 1, (FLOAT32*)&lightMarbleDirVec );
}


//--------------------------------------------------------------------------------------
// Name: SetCloudsShader()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetCloudsShader()
{
	FRMVECTOR3 cloudsColor1Vec = FRMVECTOR3( 0.0f, 0.0f, 0.5f );
	FRMVECTOR3 cloudsColor2Vec = FRMVECTOR3( 1.0f, 1.0f, 1.0f );

    glUseProgram( m_hCloudsShader );

    glUniformMatrix4fv( m_nCloudsModelViewProjectionMatrixSlot, 1, FALSE, (FLOAT32*)&m_matModelViewProj );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_3D_OES, m_pTurbTexture->m_hTextureHandle );

    glUniform1f( m_nCloudsScaleSlot, 25.0f );
    glUniform1f( m_nCloudsIntensitySlot, 1.5f + 0.5f * sinf( m_Timer.GetTime() * 2.0f ) );

    glUniform3fv( m_nCloudsColor1Slot, 1, (FLOAT32*)&cloudsColor1Vec );
    glUniform3fv( m_nCloudsColor2Slot, 1, (FLOAT32*)&cloudsColor2Vec );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Clear the backbuffer and depth-buffer
    glClearColor( 0.0f, 0.0f, 0.5f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Draw the scene
    {
        // Set the shader
        switch( m_nPTIndex )
        {
            case PT_CHECKER:
                SetCheckerShader();
                break;
            case PT_WOOD:
                SetWoodShader();
                break;
            case PT_MARBLE:
                SetMarbleShader();
                break;
            case PT_CLOUDS:
                SetCloudsShader();
                break;
        }

        m_Mesh.Render();
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

