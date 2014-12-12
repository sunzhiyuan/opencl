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
    return new CSample( "NormalMapping" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_bUseNormalMapping = TRUE;
    m_bCameraPaused     = FALSE;
    m_fCameraSpeed      = 2.0f;
    m_fCameraSplinePos  = 0.0f;

    m_hNormalMappingShader   = 0;
    m_hDefaultLightingShader = 0;
    m_pLogoTexture      = NULL;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the room shader for doing normal mapping
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "position", FRM_VERTEX_POSITION },
            { "texCoord", FRM_VERTEX_TEXCOORD0 },
            { "tangent",  FRM_VERTEX_TANGENT },
            { "binormal", FRM_VERTEX_BINORMAL },
            { "normal",   FRM_VERTEX_NORMAL },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/NormalMapping.vs",
                                                      "Samples/Shaders/NormalMapping.fs",
                                                      (GLuint*)&m_hNormalMappingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_MVPLoc[0]      = glGetUniformLocation( m_hNormalMappingShader, "matModelViewProj" );
        m_LightPosLoc[0] = glGetUniformLocation( m_hNormalMappingShader, "vLightPos" );
        m_CamPosLoc[0]   = glGetUniformLocation( m_hNormalMappingShader, "vCamPos" );
        m_AmbientLoc[0]  = glGetUniformLocation( m_hNormalMappingShader, "fAmbient" );
        m_BaseLoc[0]     = glGetUniformLocation( m_hNormalMappingShader, "BaseTexture" );
        m_BumpLoc[0]     = glGetUniformLocation( m_hNormalMappingShader, "BumpTexture" );
    }

    // Create the room shader for doing default lighting (no normal mapping)
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "position", FRM_VERTEX_POSITION },
            { "texCoord", FRM_VERTEX_TEXCOORD0 },
            { "tangent",  FRM_VERTEX_TANGENT },
            { "binormal", FRM_VERTEX_BINORMAL },
            { "normal",   FRM_VERTEX_NORMAL },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/NoNormalMapping.vs",
                                                      "Samples/Shaders/NoNormalMapping.fs",
                                                      (GLuint*)&m_hDefaultLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_MVPLoc[1]      = glGetUniformLocation( m_hDefaultLightingShader, "matModelViewProj" );
        m_LightPosLoc[1] = glGetUniformLocation( m_hDefaultLightingShader, "vLightPos" );
        m_CamPosLoc[1]   = glGetUniformLocation( m_hDefaultLightingShader, "vCamPos" );
        m_AmbientLoc[1]  = glGetUniformLocation( m_hDefaultLightingShader, "fAmbient" );
        m_BaseLoc[1]     = glGetUniformLocation( m_hDefaultLightingShader, "BaseTexture" );
        m_BumpLoc[1]     = glGetUniformLocation( m_hDefaultLightingShader, "BumpTexture" );
    }

    return TRUE;
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

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/NormalMapping.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_bUseNormalMapping, "Normal Mapping", "On", "Off" );
    m_UserInterface.AddFloatVariable( &m_fCameraSpeed, "Speed", "%3.2f" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Toggle Normal Mapping" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Pause the Camera" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Decrease Camera Speed" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Increase Camera Speed" );

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
    // Load the room mesh
    if( FALSE == m_RoomMesh.Load( "Samples/Meshes/Map3.mesh" ) )
        return FALSE;

    // Create vertex buffers and bind textures for the mesh
    if( FALSE == m_RoomMesh.MakeDrawable( &resource ) )
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
    m_matProj = FrmMatrixPerspectiveFovRH( 1.5f, fAspect, 0.25f, 20.0f );

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
    if( m_pLogoTexture ) m_pLogoTexture->Release();
    
    if( m_hNormalMappingShader )   glDeleteProgram( m_hNormalMappingShader );
    if( m_hDefaultLightingShader ) glDeleteProgram( m_hDefaultLightingShader );

    m_pLogoTexture           = NULL;
    m_hNormalMappingShader   = 0;
    m_hDefaultLightingShader = 0;
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
                       2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
                       0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Update the camera position
    if( FALSE == m_bCameraPaused )
        m_fCameraSplinePos += m_fCameraSpeed * fElapsedTime;
    m_vCameraPos = GetCameraPos( m_fCameraSplinePos );
    FRMVECTOR3 vLookPos = GetCameraPos( m_fCameraSplinePos + 0.01f);

    // Handle input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    if( nPressedButtons & INPUT_KEY_1 )
        m_bUseNormalMapping = !m_bUseNormalMapping;
    if( nPressedButtons & INPUT_KEY_2 )
        m_bCameraPaused = !m_bCameraPaused;
    if( nButtons & INPUT_KEY_3 )
        m_fCameraSpeed -= m_bCameraPaused ? 0.0f : fElapsedTime;
    if( nButtons & INPUT_KEY_4 )
        m_fCameraSpeed += m_bCameraPaused ? 0.0f : fElapsedTime;

	FRMVECTOR3 vUp(0,1,0);

    // Build the matrices
    FRMMATRIX4X4 matModel     = FrmMatrixIdentity();
    FRMMATRIX4X4 matView      = FrmMatrixLookAtRH( m_vCameraPos, vLookPos, vUp );
    FRMMATRIX4X4 matModelView = FrmMatrixMultiply( matModel, matView );
    m_matModelViewProj = FrmMatrixMultiply( matModelView, m_matProj );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderScene( const FRMVECTOR3& vLightPos, FLOAT32 fAmbientLighting )
{
    glFrontFace( GL_CCW );
    
    if( m_bUseNormalMapping )
        glUseProgram( m_hNormalMappingShader );
    else
        glUseProgram( m_hDefaultLightingShader );

    glUniformMatrix4fv( m_MVPLoc[!m_bUseNormalMapping], 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniform3fv( m_LightPosLoc[!m_bUseNormalMapping], 1, (FLOAT32*)&vLightPos );
    glUniform3fv( m_CamPosLoc[!m_bUseNormalMapping], 1, (FLOAT32*)&m_vCameraPos );
    glUniform1f( m_AmbientLoc[!m_bUseNormalMapping], fAmbientLighting );
    glUniform1i( m_BaseLoc[!m_bUseNormalMapping], 0 );
    glUniform1i( m_BumpLoc[!m_bUseNormalMapping], 1 );

    // Render the mesh
    m_RoomMesh.Render();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    // Set render states
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_FRONT );
    glDisable( GL_BLEND );

    // Clear the depth-buffer
    glClear( GL_DEPTH_BUFFER_BIT );

    // Draw the room 
    static FRMVECTOR3 vLightPos = FRMVECTOR3( +3.3f, 2.0f, -0.42f ); 
    RenderScene( vLightPos, 0.15f );

    // Restore state
    glDisable( GL_CULL_FACE );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

