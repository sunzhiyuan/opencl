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
#include "Scene50.h"


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
RGBPassThroughShader::RGBPassThroughShader()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID RGBPassThroughShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}



//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
HSVShader::HSVShader()
{
    RGBtoHSVShaderId = 0;
    HSVtoRGBShaderId = 0;
    SaturationScale = 1.0f;
    HueBias = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID HSVShader::Destroy()
{
    if( RGBtoHSVShaderId ) glDeleteProgram( RGBtoHSVShaderId );
    if( HSVtoRGBShaderId ) glDeleteProgram( HSVtoRGBShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CIEShader::CIEShader()
{
    RGBtoCIEShaderId = 0;
    CIEtoRGBShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CIEShader::Destroy()
{
    if( RGBtoCIEShaderId ) glDeleteProgram( RGBtoCIEShaderId );
    if( CIEtoRGBShaderId ) glDeleteProgram( CIEtoRGBShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
L16Shader::L16Shader()
{
    RGBtoL16ShaderId = 0;
    L16toRGBShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID L16Shader::Destroy()
{
    if( RGBtoL16ShaderId ) glDeleteProgram( RGBtoL16ShaderId );
    if( L16toRGBShaderId ) glDeleteProgram( L16toRGBShaderId );
}


//--------------------------------------------------------------------------------------
// Name: CSample50()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample50::CSample50( const CHAR* strName ) : CScene( strName )
{
    m_pLogoTexture = NULL;
    m_ColorTexture = NULL;
    m_ColorSpace = COLORSPACE_RGB_PASSTHROUGH;

    // This matches the aspect ratio of the crayons sample image (512/282)
    m_nWidth = 800;
    m_nHeight = 441;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample50::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    // We need the Font for later
    m_pFont = &m_Font;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/50_Textures.pak" ) )
        return FALSE;

    m_ColorTexture = resource.GetTexture( "ColorPic" );


    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Previous color space" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Next color space" );


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
BOOL CSample50::InitShaders()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "g_PositionIn", FRM_VERTEX_POSITION }
    };
    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);


    // Create the RGB pass through shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PassThrough.vs", "Demos/AdrenoShaders/Shaders/RGBPassThrough.fs",
            &m_RGBPassThroughShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_RGBPassThroughShader.TextureId = glGetUniformLocation( m_RGBPassThroughShader.ShaderId, "g_Texture" );
    }

    // Create the CIE shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PassThrough.vs", "Demos/AdrenoShaders/Shaders/RGBtoCIE.fs",
            &m_CIEShader.RGBtoCIEShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PassThrough.vs", "Demos/AdrenoShaders/Shaders/CIEtoRGB.fs",
            &m_CIEShader.CIEtoRGBShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CIEShader.TextureId = glGetUniformLocation( m_CIEShader.RGBtoCIEShaderId, "g_Texture" );
        m_CIEShader.CIETextureId = glGetUniformLocation( m_CIEShader.CIEtoRGBShaderId, "g_CIETexture" );
    }

    // Create the HSV shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PassThrough.vs", "Demos/AdrenoShaders/Shaders/RGBtoHSV.fs",
            &m_HSVShader.RGBtoHSVShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PassThrough.vs", "Demos/AdrenoShaders/Shaders/HSVtoRGB.fs",
            &m_HSVShader.HSVtoRGBShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_HSVShader.TextureId = glGetUniformLocation( m_HSVShader.RGBtoHSVShaderId, "g_Texture" );
        m_HSVShader.HSVTextureId = glGetUniformLocation( m_HSVShader.HSVtoRGBShaderId, "g_HSVTexture" );
        m_HSVShader.HSVParamsId = glGetUniformLocation( m_HSVShader.HSVtoRGBShaderId, "g_HSVParams" );
    }

    // Create the L16 shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PassThrough.vs", "Demos/AdrenoShaders/Shaders/RGBtoL16.fs",
            &m_L16Shader.RGBtoL16ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PassThrough.vs", "Demos/AdrenoShaders/Shaders/L16toRGB.fs",
            &m_L16Shader.L16toRGBShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_L16Shader.TextureId = glGetUniformLocation( m_L16Shader.RGBtoL16ShaderId, "g_Texture" );
        m_L16Shader.L16TextureId = glGetUniformLocation( m_L16Shader.L16toRGBShaderId, "g_L16Texture" );
    }


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample50::Resize()
{
    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    m_Backbuffer.CreateRT( m_nWidth, m_nHeight, TRUE );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample50::Destroy()
{
    // Free shader memory
    m_RGBPassThroughShader.Destroy();
    m_CIEShader.Destroy();
    m_HSVShader.Destroy();
    m_L16Shader.Destroy();

    // Free texture memory
    if( m_pLogoTexture ) m_pLogoTexture->Release();
    if( m_ColorTexture ) m_ColorTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: ResetSettings()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample50::ResetSettings()
{
    m_HSVShader.SaturationScale = 1.0f;
    m_HSVShader.HueBias = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name: ProcessHSVInput()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample50::ProcessHSVInput( FLOAT32 ElapsedTime, UINT32 Buttons, UINT32 PressedButtons )
{
    // Saturation scale
    if( Buttons & INPUT_KEY_3 )
    {
        m_HSVShader.SaturationScale -= ( 2.0f * ElapsedTime );
        m_HSVShader.SaturationScale = max( 0.0f, m_HSVShader.SaturationScale );
    }
    if( Buttons & INPUT_KEY_4 )
    {
        m_HSVShader.SaturationScale += ( 2.0f * ElapsedTime );
        m_HSVShader.SaturationScale = min( 2.0f, m_HSVShader.SaturationScale );
    }

    // Hue bias
    if( Buttons & INPUT_KEY_5 )
    {
        m_HSVShader.HueBias -= ( 1.0f * ElapsedTime );
        while( m_HSVShader.HueBias < 0.0f )
            m_HSVShader.HueBias += 1.0f;
    }
    if( Buttons & INPUT_KEY_6 )
    {
        m_HSVShader.HueBias += ( 1.0f * ElapsedTime );
        while( m_HSVShader.HueBias >= 1.0f )
            m_HSVShader.HueBias -= 1.0f;
    }

    if( PressedButtons & INPUT_KEY_7 )
    {
        ResetSettings();
    }
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample50::Update()
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

    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_ColorSpace--;
        if( m_ColorSpace < 0 )
            m_ColorSpace = ( MAX_COLORSPACES - 1 );

        ResetSettings();
    }

    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_ColorSpace++;
        if( m_ColorSpace >= MAX_COLORSPACES )
            m_ColorSpace = 0;

        ResetSettings();
    }


    switch( m_ColorSpace )
    {
        case COLORSPACE_HSV:
            ProcessHSVInput( ElapsedTime, nButtons, nPressedButtons );
            break;

        default:
            break;
    }
}


//--------------------------------------------------------------------------------------
// Name: DisplayColorSpaceName()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample50::DisplayColorSpaceName()
{
    m_pFont->Begin();
    m_pFont->SetScaleFactors( 1.0f, 1.0f );

    char* ColorSpaceText = (char *)"UNSUPPORTED";
    switch( m_ColorSpace )
    {
        case COLORSPACE_RGB_PASSTHROUGH:
            ColorSpaceText = (char *)"1. RGB Pass-through";
            break;

        case COLORSPACE_CIE:
            ColorSpaceText = (char *)"2. CIE";
            break;

        case COLORSPACE_HSV:
            ColorSpaceText = (char *)"3. HSV";
            break;

        case COLORSPACE_L16LUV:
            ColorSpaceText = (char *)"4. L16Luv";
            break;
    }


    // Want a semi-centered position, left justified.  Let's use the centered position
    // of the longest string in the list for everyone.
    FLOAT32 tx, ty;
    tx = ( (FLOAT32)m_nWidth * 0.5f ) - ( m_pFont->GetTextWidth( "1. RGB Pass-through" ) * 0.5f );
    ty = (FLOAT32)m_nHeight - m_pFont->GetTextHeight() * 3.0f;

    m_pFont->DrawText( tx, ty, FRMCOLOR_YELLOW, ColorSpaceText );

    m_pFont->SetScaleFactors( 1.0f, 1.0f );
    m_pFont->End();
}


//--------------------------------------------------------------------------------------
// Name: DisplayColorSpaceHelp()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample50::DisplayColorSpaceHelp()
{
    m_pFont->Begin();
    m_pFont->SetScaleFactors( 0.75f, 0.75f );

    FLOAT32 tx, ty;
    tx = ( (FLOAT32)m_nWidth ) - ( m_pFont->GetTextWidth( "0" ) * 20.0f );
    ty = 50.0f;

    switch( m_ColorSpace )
    {
        case COLORSPACE_HSV:
            {
                char buf[1024];
                sprintf( buf, "Saturation scale: %.02f", m_HSVShader.SaturationScale );
                m_pFont->DrawText( tx, ty, FRMCOLOR_WHITE, buf );

                ty += m_pFont->GetTextHeight();
                sprintf( buf, "Hue bias: %.02f", m_HSVShader.HueBias );
                m_pFont->DrawText( tx, ty, FRMCOLOR_WHITE, buf );


                ty += m_pFont->GetTextHeight() * 2.0f;
                m_pFont->DrawText( tx, ty, FRMCOLOR_CYAN, "keys:" );
                ty += m_pFont->GetTextHeight();
                m_pFont->DrawText( tx, ty, FRMCOLOR_CYAN, "3/4 = saturation scale" );
                ty += m_pFont->GetTextHeight();
                m_pFont->DrawText( tx, ty, FRMCOLOR_CYAN, "5/6 = hue bias" );
                ty += m_pFont->GetTextHeight();
                m_pFont->DrawText( tx, ty, FRMCOLOR_CYAN, "7 = reset settings" );
            }
            break;

        default:
            break;
    }

    m_pFont->SetScaleFactors( 1.0f, 1.0f );
    m_pFont->End();
}


//--------------------------------------------------------------------------------------
// Name: DrawRGB()
// Desc: RGB pass through mode
//--------------------------------------------------------------------------------------
VOID CSample50::DrawRGB()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glCullFace( GL_BACK );
    glFrontFace( GL_CW );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    glUseProgram( m_RGBPassThroughShader.ShaderId );
    SetTexture( m_RGBPassThroughShader.TextureId, m_ColorTexture->m_hTextureHandle, 0 );

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: DrawCIE()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample50::DrawCIE()
{
    glCullFace( GL_BACK );
    glFrontFace( GL_CW );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    // Draw to back buffer in CIE format
    {
        m_Backbuffer.SetFramebuffer();

        glUseProgram( m_CIEShader.RGBtoCIEShaderId );
        SetTexture( m_CIEShader.TextureId, m_ColorTexture->m_hTextureHandle, 0 );

        RenderScreenAlignedQuad();
    }

    // Now draw back to the screen in RGB
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glViewport( 0, 0, m_nWidth, m_nHeight );

        glUseProgram( m_CIEShader.CIEtoRGBShaderId );
        SetTexture( m_CIEShader.CIETextureId, m_Backbuffer.TextureHandle, 0 );

        RenderScreenAlignedQuad();
    }
}


//--------------------------------------------------------------------------------------
// Name: DrawHSV()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample50::DrawHSV()
{
    glCullFace( GL_BACK );
    glFrontFace( GL_CW );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    // Draw to back buffer in HSV format
    {
        m_Backbuffer.SetFramebuffer();

        glUseProgram( m_HSVShader.RGBtoHSVShaderId );
        SetTexture( m_HSVShader.TextureId, m_ColorTexture->m_hTextureHandle, 0 );

        RenderScreenAlignedQuad();
    }

    // Now draw back to the screen in RGB
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glViewport( 0, 0, m_nWidth, m_nHeight );

        float SaturationScale = ( m_HSVShader.SaturationScale < 1.0f ) ? m_HSVShader.SaturationScale : pow( m_HSVShader.SaturationScale, 3.0f );

        glUseProgram( m_HSVShader.HSVtoRGBShaderId );
        SetTexture( m_HSVShader.HSVTextureId, m_Backbuffer.TextureHandle, 0 );
        glUniform2f( m_HSVShader.HSVParamsId, m_HSVShader.HueBias, SaturationScale );

        RenderScreenAlignedQuad();
    }
}


//--------------------------------------------------------------------------------------
// Name: DrawL16()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample50::DrawL16()
{
    glCullFace( GL_BACK );
    glFrontFace( GL_CW );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    // Draw to back buffer in L16Luv format
    {
        m_Backbuffer.SetFramebuffer();

        glUseProgram( m_L16Shader.RGBtoL16ShaderId );
        SetTexture( m_L16Shader.TextureId, m_ColorTexture->m_hTextureHandle, 0 );

        RenderScreenAlignedQuad();
    }

    // Now draw back to the screen in RGB
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glViewport( 0, 0, m_nWidth, m_nHeight );

        glUseProgram( m_L16Shader.L16toRGBShaderId );
        SetTexture( m_L16Shader.L16TextureId, m_Backbuffer.TextureHandle, 0 );
 
        RenderScreenAlignedQuad();
    }
}


//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample50::DrawScene()
{
    switch( m_ColorSpace )
    {
        case COLORSPACE_CIE:
            DrawCIE();
            break;

        case COLORSPACE_HSV:
            DrawHSV();
            break;

        case COLORSPACE_L16LUV:
            DrawL16();
            break;

        case COLORSPACE_RGB_PASSTHROUGH:
        default:
            DrawRGB();
    }
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample50::Render()
{
    DrawScene();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );

    DisplayColorSpaceName();
    DisplayColorSpaceHelp();

    // Put the state back
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glFrontFace( GL_CCW );
}
