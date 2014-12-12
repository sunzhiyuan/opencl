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
    return new CSample( "ParallaxMapping" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hParallaxMappingShader           = 0;
    m_hParallaxMappingMVPLoc           = 0;
    m_hParallaxMappingMLoc             = 0;
    m_hParallaxMappingNormalLoc        = 0;
    m_hParallaxMappingViewPositionLoc  = 0;
    m_hParallaxMappingLightPositionLoc = 0;
    m_hParallaxMappingTextureScaleLoc  = 0;
    m_hParallaxMappingBaseTextureLoc   = 0;
    m_hParallaxMappingBumpTextureLoc   = 0;
    m_hParallaxMappingParallaxLoc      = 0;
    m_hParallaxMappingScaleLoc         = 0;
    m_hParallaxMappingAmbientLoc       = 0;
    m_hParallaxMappingSpecularLoc      = 0;
    m_hParallaxMappingModeLoc          = 0;

    m_pLogoTexture                     = NULL;

    for( INT32 nTextureIndex = 0; nTextureIndex < NUM_TEXTURES; ++nTextureIndex )
    {
        m_pBaseTexture[ nTextureIndex ] = NULL;
        m_pBumpTexture[ nTextureIndex ] = NULL;
    }

    m_fParallax                        = 0.0f;
    m_fScale                           = 0.0f;

    m_nMode                            = 0;
    m_nTextureId                       = 0;

    m_fRotateX                         = 0.0f;
    m_fRotateY                         = 0.0f;

    m_bBumpMappingOn                   = FALSE;
    m_bParallaxMappingOn               = FALSE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the Stencil Volume shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vPositionOS",   FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL },
            { "g_vTexCoord", FRM_VERTEX_TEXCOORD0 },
            { "g_vTangentOS",  FRM_VERTEX_TANGENT },
            { "g_vBinormalOS", FRM_VERTEX_BINORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/ParallaxMapping.vs",
                                                      "Samples/Shaders/ParallaxMapping.fs",
                                                      &m_hParallaxMappingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_hParallaxMappingMVPLoc           = glGetUniformLocation( m_hParallaxMappingShader, "g_matModelViewProj" );
        m_hParallaxMappingMLoc             = glGetUniformLocation( m_hParallaxMappingShader, "g_matModelMatrix" );
        m_hParallaxMappingNormalLoc        = glGetUniformLocation( m_hParallaxMappingShader, "g_matNormal" );
        m_hParallaxMappingViewPositionLoc  = glGetUniformLocation( m_hParallaxMappingShader, "g_vViewPosition" );
        m_hParallaxMappingLightPositionLoc = glGetUniformLocation( m_hParallaxMappingShader, "g_vLightPosition" );
        m_hParallaxMappingTextureScaleLoc  = glGetUniformLocation( m_hParallaxMappingShader, "g_vTexScale" );
        m_hParallaxMappingBaseTextureLoc   = glGetUniformLocation( m_hParallaxMappingShader, "g_sBaseTexture" );
        m_hParallaxMappingBumpTextureLoc   = glGetUniformLocation( m_hParallaxMappingShader, "g_sBumpTexture" );
        m_hParallaxMappingParallaxLoc      = glGetUniformLocation( m_hParallaxMappingShader, "g_fParallax" );
        m_hParallaxMappingScaleLoc         = glGetUniformLocation( m_hParallaxMappingShader, "g_fScale" );
        m_hParallaxMappingAmbientLoc       = glGetUniformLocation( m_hParallaxMappingShader, "g_Material.vAmbient" );
        m_hParallaxMappingDiffuseLoc       = glGetUniformLocation( m_hParallaxMappingShader, "g_Material.vDiffuse" );
        m_hParallaxMappingSpecularLoc      = glGetUniformLocation( m_hParallaxMappingShader, "g_Material.vSpecular" );
        m_hParallaxMappingModeLoc          = glGetUniformLocation( m_hParallaxMappingShader, "g_nMode" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    m_vViewPosition  = FRMVECTOR3( 0.0f, 1.0f, 0.0f );

    m_vAmbient  = FRMVECTOR4( 0.12f, 0.12f, 0.12f, 1.0f );
    m_vDiffuse  = FRMVECTOR4( 0.8f,  0.8f,  0.8f,  1.0f );
    m_vSpecular = FRMVECTOR4( 0.3f,  0.3f,  0.3f,  30.0f );

    m_fParallax = 0.05f;
    m_fScale = 2.0f;

    m_bBumpMappingOn     = TRUE;
    m_bParallaxMappingOn = TRUE;

    m_nMode = PARALLAX_AND_BUMP_MODE;

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/ParallaxMapping.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Create the Base textures
    m_pBaseTexture[ PARQ_TEXTURE ]   = resource.GetTexture( "Parq Base Texture" );
    m_pBaseTexture[ ROCKY_TEXTURE ]  = resource.GetTexture( "Rocky Base Texture" );
    m_pBaseTexture[ BRICK1_TEXTURE ] = resource.GetTexture( "Brick1 Base Texture" );
    m_pBaseTexture[ BRICK2_TEXTURE ] = resource.GetTexture( "Brick2 Base Texture" );

    // Create the Bump textures
    m_pBumpTexture[ PARQ_TEXTURE ]   = resource.GetTexture( "Parq Bump Texture" );
    m_pBumpTexture[ ROCKY_TEXTURE ]  = resource.GetTexture( "Rocky Bump Texture" );
    m_pBumpTexture[ BRICK1_TEXTURE ] = resource.GetTexture( "Brick1 Bump Texture" );
    m_pBumpTexture[ BRICK2_TEXTURE ] = resource.GetTexture( "Brick2 Bump Texture" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_fParallax, "Parallax", "%4.3f" );
    m_UserInterface.AddFloatVariable( &m_vSpecular.x, "Specular", "%4.3f" );
    m_UserInterface.AddBoolVariable( &m_bBumpMappingOn, "Bump Mapping" );
    m_UserInterface.AddBoolVariable( &m_bParallaxMappingOn, "Parallax Mapping" );
    m_UserInterface.AddFloatVariable( &m_vViewPosition.y, "View Position" );
    m_UserInterface.AddFloatVariable( &m_fRotateX, "X-Rotation" );
    m_UserInterface.AddFloatVariable( &m_fRotateY, "Y-Rotation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Toggle Texture Set" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Toggle Bump Mapping" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Toggle Parallax Mapping" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Decrease Parallax" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Increase Parallax" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, "Decrease Specular Lighting" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, "Increase Specular Lighting" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, "Zoom In" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_9, "Zoom Out" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_DPAD, "Rotate Plane" );

     // Load the meshes
    if( FALSE == m_Floor.Load( "Samples/Meshes/Plane.mesh" ) )
        return FALSE;
    if( FALSE == m_Floor.MakeDrawable() )
        return FALSE;

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
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matPersp.M(0,0) *= fAspect;
        m_matPersp.M(1,1) *= fAspect;
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
    // Delete shader programs
    if( m_hParallaxMappingShader ) glDeleteProgram( m_hParallaxMappingShader );

    if( m_pLogoTexture ) m_pLogoTexture->Release();

    for( INT32 nTextureNum = 0; nTextureNum < NUM_TEXTURES; ++nTextureNum )
    {
        if( m_pBaseTexture ) m_pBaseTexture[ nTextureNum ]->Release();
        if( m_pBumpTexture ) m_pBumpTexture[ nTextureNum ]->Release();
    }

    m_Floor.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fTime        = m_Timer.GetFrameTime();
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    if( nPressedButtons & INPUT_KEY_0 )
    {
        m_UserInterface.AdvanceState();
    }

    // Change to the next base / bump texture set
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nTextureId = ( m_nTextureId + 1 ) % NUM_TEXTURES;
    }

    // Toggle bump mapping on/off
    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_nMode ^= 1;
        m_bBumpMappingOn = 1 - m_bBumpMappingOn;
    }

    // Toggle Parallax mapping on/off
    if( nPressedButtons & INPUT_KEY_3 )
    {
        m_nMode ^= 2;
        m_bParallaxMappingOn = 1 - m_bParallaxMappingOn;
    }

    // Decrease parallax
    if( nButtons & INPUT_KEY_4 )
    {
        m_fParallax -= 0.1f * fElapsedTime;
        if( m_fParallax < 0.0f )
        {
            m_fParallax = 0.0f;
        }
    }

    // Increase parallax
    if( nButtons & INPUT_KEY_5 )
    {
        m_fParallax += 0.1f * fElapsedTime;

        if( m_fParallax > 0.05f )
        {
            m_fParallax = 0.05f;
        }
    }

    // Decrease specular lighting
    if( nButtons & INPUT_KEY_6 )
    {
        m_vSpecular -= FRMVECTOR4( 0.4f, 0.4f, 0.4f, 0.0f ) * fElapsedTime;
        if( m_vSpecular.x < 0.0f )
        {
            m_vSpecular = FRMVECTOR4( 0.0f, 0.0f, 0.0f, m_vSpecular.a );
        }
    }

    // Increase specular lighting
    if( nButtons & INPUT_KEY_7 )
    {
        m_vSpecular += FRMVECTOR4( 0.4f, 0.4f, 0.4f, 0.0f ) * fElapsedTime;
        if( m_vSpecular.x > 1.0f )
        {
            m_vSpecular = FRMVECTOR4( 1.0f, 1.0f, 1.0f, m_vSpecular.a );
        }
    }

    // Zoom in
    if( nButtons & INPUT_KEY_8 )
    {
        m_vViewPosition.y -= fElapsedTime;
        if( m_vViewPosition.y < 0.8f )
        {
            m_vViewPosition.y = 0.8f;
        }
    }

    // Zoom out
    if( nButtons & INPUT_KEY_9 )
    {
        m_vViewPosition.y += fElapsedTime;
        if( m_vViewPosition.y > 2.0f )
        {
            m_vViewPosition.y = 2.0f;
        }
    }

    // Rotate plane CW along x-axis
    if( nButtons & INPUT_DPAD_UP )
    {
        m_fRotateX += fElapsedTime;
    }

    // Rotate plane CCW along y-axis
    if( nButtons & INPUT_DPAD_LEFT )
    {
        m_fRotateY -= fElapsedTime;
    }

    // Rotate plane CW along x-axis
    if( nButtons & INPUT_DPAD_RIGHT )
    {
        m_fRotateY += fElapsedTime;
    }

    // Rotate plane CCW along y-axis
    if( nButtons & INPUT_DPAD_DOWN )
    {
        m_fRotateX -= fElapsedTime;
    }

    // Setup the camera view matrix
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 0.0f, -1.0f );
    FRMMATRIX4X4 matView = FrmMatrixLookAtRH( m_vViewPosition, vLookAt, vUp );

    // Setup the floor's camera relative model view projection matrix
    FRMMATRIX4X4 matFloorRotateX    = FrmMatrixRotate( m_fRotateX, 1.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 matFloorRotateY    = FrmMatrixRotate( m_fRotateY, 0.0f, 0.0f, 1.0f );
    m_matFloorModel                 = FrmMatrixMultiply( matFloorRotateY, matFloorRotateX );
    FRMMATRIX4X4 matFloorModelView  = FrmMatrixMultiply( m_matFloorModel, matView );
    m_matFloorModelViewProj         = FrmMatrixMultiply( matFloorModelView, m_matPersp );
    m_matNormal                     = FrmMatrixNormal( m_matFloorModel );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    // Clear the backbuffer and depth-buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Render our floor with the parallax shader applied

    glUseProgram( m_hParallaxMappingShader );

    glUniformMatrix4fv( m_hParallaxMappingMVPLoc, 1, FALSE, (FLOAT32*) &m_matFloorModelViewProj );
    glUniformMatrix4fv( m_hParallaxMappingMLoc, 1, FALSE, (FLOAT32*) &m_matFloorModel );
    glUniformMatrix3fv( m_hParallaxMappingNormalLoc, 1, FALSE, (FLOAT32*) &m_matNormal );
    glUniform3f( m_hParallaxMappingLightPositionLoc, 0.0f, 1000.0, 0.0 );
    glUniform3f( m_hParallaxMappingViewPositionLoc, m_vViewPosition.x, m_vViewPosition.y, m_vViewPosition.z );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pBaseTexture[ m_nTextureId ]->m_hTextureHandle );
    glUniform1i( m_hParallaxMappingBaseTextureLoc, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_pBumpTexture[ m_nTextureId ]->m_hTextureHandle );
    glUniform1i( m_hParallaxMappingBumpTextureLoc, 1 );

    glUniform4fv( m_hParallaxMappingAmbientLoc,  1, (FLOAT32*) &m_vAmbient );
    glUniform4fv( m_hParallaxMappingDiffuseLoc,  1, (FLOAT32*) &m_vDiffuse );
    glUniform4fv( m_hParallaxMappingSpecularLoc, 1, (FLOAT32*) &m_vSpecular );

    glUniform1f( m_hParallaxMappingParallaxLoc, m_fParallax );
    glUniform1f( m_hParallaxMappingScaleLoc, m_fScale );
    glUniform1i( m_hParallaxMappingModeLoc, m_nMode );

    glUniform1f( m_hParallaxMappingTextureScaleLoc, 1.0f );

    m_Floor.Render();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

