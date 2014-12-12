//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Scene49.h"


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader49::MaterialShader49()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader49::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name: CSample49()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample49::CSample49( const CHAR* strName ) : CScene( strName )
{
    m_pLogoTexture = NULL;
    m_HDRTexture[0] = m_HDRTexture[1] = NULL;
    m_Exposure = 1.0f;

    m_nWidth = 800;
    m_nHeight = 480;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample49::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/49_Textures.pak" ) )
        return FALSE;

    m_HDRTexture[0] = resource.GetTexture( "HDRTexture1" );
    m_HDRTexture[1] = resource.GetTexture( "HDRTexture2" );


    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_Exposure, (char *)"Exposure" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Decrease exposure" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Increase exposure" );


	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample49::InitShaders()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "g_PositionIn", FRM_VERTEX_POSITION }
    };
    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);


    // Create the RGB pass through shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader49.vs", "Demos/AdrenoShaders/Shaders/MaterialShader49.fs",
            &m_MaterialShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_MaterialShader.TextureId = glGetUniformLocation( m_MaterialShader.ShaderId, "g_Texture" );
        m_MaterialShader.ExposureId = glGetUniformLocation( m_MaterialShader.ShaderId, "g_Exposure" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample49::Resize()
{
    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample49::Destroy()
{
    // Free shader memory
    m_MaterialShader.Destroy();

    // Free texture memory
    if( m_pLogoTexture ) m_pLogoTexture->Release();
    if( m_HDRTexture[0] ) m_HDRTexture[0]->Release();
    if( m_HDRTexture[1] ) m_HDRTexture[1]->Release();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample49::Update()
{
    // get the current time
    FLOAT32 ElapsedTime = m_Timer.GetFrameElapsedTime();

    // process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();


    if( nButtons & INPUT_KEY_1 )
    {
        m_Exposure -= 3.0f * ElapsedTime;
        m_Exposure = max( 0.1f, m_Exposure );
    }

    if( nButtons & INPUT_KEY_2 )
    {
        m_Exposure += 3.0f * ElapsedTime;
        m_Exposure = min( 3.0f, m_Exposure );
    }

}


//--------------------------------------------------------------------------------------
// Name: DrawRGB()
// Desc: RGB pass through mode
//--------------------------------------------------------------------------------------
// VOID CSample49::DrawRGB()
// {
//     glBindFramebuffer( GL_FRAMEBUFFER, 0 );
//     glViewport( 0, 0, m_nWidth, m_nHeight );
// 
//     glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
//     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
// 
//     glCullFace( GL_BACK );
//     glFrontFace( GL_CW );
//     glDisable( GL_DEPTH_TEST );
//     glDisable( GL_BLEND );
// 
//     glUseProgram( m_RGBPassThroughShader.ShaderId );
//     SetTexture( m_RGBPassThroughShader.TextureId, m_ColorTexture->m_hTextureHandle, 0 );
// 
//     RenderScreenAlignedQuad();
// }


//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample49::DrawScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glCullFace( GL_BACK );
    glFrontFace( GL_CW );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );


    glUseProgram( m_MaterialShader.ShaderId );
    glUniform1f( m_MaterialShader.ExposureId, m_Exposure );

    FRMVECTOR2 CardSize = FRMVECTOR2( 0.4f, 0.8f );
    FRMVECTOR2 CardPos1 = FRMVECTOR2( -0.475f, 0.0f );
    SetTexture( m_MaterialShader.TextureId, m_HDRTexture[0]->m_hTextureHandle, 0 );
    RenderScreenAlignedQuad( CardPos1 - CardSize, CardPos1 + CardSize );

    FRMVECTOR2 CardPos2 = FRMVECTOR2( 0.475f, 0.0f );
    SetTexture( m_MaterialShader.TextureId, m_HDRTexture[1]->m_hTextureHandle, 0 );
    RenderScreenAlignedQuad( CardPos2 - CardSize, CardPos2 + CardSize );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample49::Render()
{
    DrawScene();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );

    // Put the state back
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glFrontFace( GL_CCW );
}
