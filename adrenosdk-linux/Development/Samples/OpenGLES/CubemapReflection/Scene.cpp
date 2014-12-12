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


// Used to setup the six camera angles
struct CAMERA
{
    FRMVECTOR3 vPosition;
    FRMVECTOR3 vLookAt;
    FRMVECTOR3 vUp;
};

CAMERA g_CubemapCameras[6] = 
{
  //            vPosition              vLookAt                  vUp
  { FRMVECTOR3( 0, 0, 0 ), FRMVECTOR3( 1, 0, 0 ), FRMVECTOR3( 0,-1, 0 ) }, // +x face
  { FRMVECTOR3( 0, 0, 0 ), FRMVECTOR3(-1, 0, 0 ), FRMVECTOR3( 0,-1, 0 ) }, // -x face
  { FRMVECTOR3( 0, 0, 0 ), FRMVECTOR3( 0, 1, 0 ), FRMVECTOR3( 0, 0, 1 ) }, // +y face
  { FRMVECTOR3( 0, 0, 0 ), FRMVECTOR3( 0,-1, 0 ), FRMVECTOR3( 0, 0,-1 ) }, // -y face
  { FRMVECTOR3( 0, 0, 0 ), FRMVECTOR3( 0, 0, 1 ), FRMVECTOR3( 0,-1, 0 ) }, // +z face
  { FRMVECTOR3( 0, 0, 0 ), FRMVECTOR3( 0, 0,-1 ), FRMVECTOR3( 0,-1, 0 ) }  // -z face
};

// Size of the environment map
const UINT32 ENVMAP_SIZE = 128;


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "CubemapReflection" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hMainShader       = 0;
    m_hSatelliteShader  = 0;
    m_pSatelliteTexture = NULL;
    m_pPlaqueTexture    = NULL;
    m_pEnvironmentMap   = NULL;
    m_pLogoTexture      = NULL;
    m_vLightPosWorld    = FRMVECTOR3( -3.0, 0.5, 3.0 );
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the Main shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vTexCcordIN", FRM_VERTEX_TEXCOORD0 },
            { "g_vNormalOS", FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CubemapReflectionMain.vs",
                                                      "Samples/Shaders/CubemapReflectionMain.fs",
                                                      &m_hMainShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_locMainModelViewProjMatrix = glGetUniformLocation( m_hMainShader, "g_matModelViewProj" );
        m_locMainModelViewMatrix     = glGetUniformLocation( m_hMainShader, "g_matModelView" );
        m_locMainNormalMatrix        = glGetUniformLocation( m_hMainShader, "g_matNormal" );
        m_locMainRefMap              = glGetUniformLocation( m_hMainShader, "g_RefMap" );
        m_locMainEnvMap                 = glGetUniformLocation( m_hMainShader, "g_EnvMap" );
        m_locMainLightPos            = glGetUniformLocation( m_hMainShader, "g_vLightPos" );
    }

    // Create the Satellite shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS", FRM_VERTEX_NORMAL },
            { "g_vTexcoordIN", FRM_VERTEX_TEXCOORD0 }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CubemapReflectionSatellite.vs",
                                                      "Samples/Shaders/CubemapReflectionSatellite.fs",
                                                      &m_hSatelliteShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_locSatModelViewProjMatrix = glGetUniformLocation( m_hSatelliteShader, "g_matModelViewProj" );
        m_locSatModelViewMatrix     = glGetUniformLocation( m_hSatelliteShader, "g_matModelView" );
        m_locSatNormalMatrix        = glGetUniformLocation( m_hSatelliteShader, "g_matNormal" );
        m_locSatTex                 = glGetUniformLocation( m_hSatelliteShader, "g_BaseMap" );
        m_locSatLightPos            = glGetUniformLocation( m_hSatelliteShader, "g_vLightPos" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: FrmCreateCubeMap()
// Desc: Helper function to create a cube map
//--------------------------------------------------------------------------------------
BOOL FrmCreateCubeMap( UINT32 nSize, UINT32 nPixelType, UINT32* phCubeMap )
{
    UINT32 nPixelFormat;
    switch( nPixelType )
    {
        case GL_UNSIGNED_SHORT_5_6_5:
            nPixelFormat = GL_RGB;
            break;
        default:
            nPixelFormat = GL_RGBA;
            break;
    }
    glGenTextures( 1, phCubeMap );
    glBindTexture( GL_TEXTURE_CUBE_MAP, *phCubeMap );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    for( UINT32 i=0; i<6; i++ )
    {
        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, nPixelFormat,
                      nSize, nSize, 0, nPixelFormat, nPixelType, NULL );
    }
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: FrmCreateRenderBuffer()
// Desc: Helper function to create a render buffer
//--------------------------------------------------------------------------------------
BOOL FrmCreateRenderBuffer( UINT32 nWidth, UINT32 nHeight, UINT32 nPixelFormat,
                           UINT32* phRenderBuffer )
{
    glGenRenderbuffers( 1, phRenderBuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, *phRenderBuffer );
    glRenderbufferStorage( GL_RENDERBUFFER, nPixelFormat, nWidth, nHeight );
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
    if( FALSE == resource.LoadFromFile( "Samples/Textures/CubeMapReflection.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Create the textures
    m_pSatelliteTexture = resource.GetTexture( "TennisBall" );
    m_pPlaqueTexture    = resource.GetTexture( "Trophy_Plaque" );
    m_pEnvironmentMap   = resource.GetCubeMap( "BedroomCubeMap" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Load a mesh for the center of the screen
    if( FALSE == m_MainObject.Load( "Samples/Meshes/trophy.mesh" ) )
        return FALSE;
    if( FALSE == m_MainObject.MakeDrawable( &resource ) )
        return FALSE;
    
    // Load a mesh to use for the satellite objects
    if( FALSE == m_SatelliteObject.Load( "Samples/Meshes/tennisBall.mesh" ) )
        return FALSE;
    if( FALSE == m_SatelliteObject.MakeDrawable( &resource ) )
        return FALSE;

    // Create the reflection cube map 
    FrmCreateCubeMap( ENVMAP_SIZE, GL_UNSIGNED_SHORT_5_5_5_1, &m_hReflectionMap );

    // Create an FBO for offscreen rendering
    glGenFramebuffers( 1, &m_hFBOFramebufferID );

    // Create a depth-buffer for offscreen rendering
    FrmCreateRenderBuffer( ENVMAP_SIZE, ENVMAP_SIZE, GL_DEPTH_COMPONENT16,
                           &m_hFBODepthbufferID );

    FRMVECTOR3 vPosition = FRMVECTOR3( -0.2f, 0.25f, 1.0f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, -0.1f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matView = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );

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
    if( m_hMainShader )       glDeleteProgram( m_hMainShader );
    if( m_hSatelliteShader )  glDeleteProgram( m_hSatelliteShader );

    if( m_pLogoTexture )      m_pLogoTexture->Release();
    if( m_pSatelliteTexture ) m_pSatelliteTexture->Release();
    if( m_pPlaqueTexture )    m_pPlaqueTexture->Release();
    if( m_pEnvironmentMap )   m_pEnvironmentMap->Release();
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

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    // Position the main object
    m_matMainObject = FrmMatrixTranslate( 0.0f, -0.2f, 0.0f );

    // Rotate the satellite objects' position
    FLOAT32 fAngle0 = m_Timer.GetTime() * 3.0f;
    FLOAT32 fAngle1 = m_Timer.GetTime() * 6.0f;

    m_vSatellitePos[0].x = 0.2f * FrmSin( fAngle0 );
    m_vSatellitePos[0].y = 0.2f * FrmCos( fAngle0 );
    m_vSatellitePos[0].z = 0.2f * FrmSin( fAngle0 / 2.0f );
    m_matSatelliteObject[0] = FrmMatrixTranslate( 0.0f, -0.07f, 0.0f );
	FRMMATRIX4X4 matScale = FrmMatrixScale( 0.5f, 0.5f, 0.5f );
    m_matSatelliteObject[0] = FrmMatrixMultiply( m_matSatelliteObject[0], matScale );
	FRMMATRIX4X4 matRotate = FrmMatrixRotate( -fAngle0, 0.01f, 0.995f, 0.0f );
	m_matSatelliteObject[0] = FrmMatrixMultiply( m_matSatelliteObject[0], matRotate );
	FRMMATRIX4X4 matTranslate = FrmMatrixTranslate( m_vSatellitePos[0] );
	m_matSatelliteObject[0] = FrmMatrixMultiply( m_matSatelliteObject[0], matTranslate );

    m_vSatellitePos[1].x = 0.25f * FrmSin( fAngle1/4 );
    m_vSatellitePos[1].y = 0.25f * FrmCos( fAngle1/4 );
    m_vSatellitePos[1].z = 0.25f * FrmSin( (fAngle1/4) / 2.0f );
    m_matSatelliteObject[1] = FrmMatrixTranslate( 0.0f, -0.07f, 0.0f );
	matScale = FrmMatrixScale( 0.3f, 0.3f, 0.3f );
	m_matSatelliteObject[1] = FrmMatrixMultiply( m_matSatelliteObject[1], matScale );
	matRotate = FrmMatrixRotate( -fAngle0, 0.01f, 0.995f, 0.0f );
	m_matSatelliteObject[1] = FrmMatrixMultiply( m_matSatelliteObject[1], matRotate );
	matTranslate = FrmMatrixTranslate( m_vSatellitePos[1] );
	m_matSatelliteObject[1] = FrmMatrixMultiply( m_matSatelliteObject[1], matTranslate );
}


//--------------------------------------------------------------------------------------
// Name: RenderMainObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderMainObject( FRMMATRIX4X4 matModel, FRMMATRIX4X4 matView,
                                FRMMATRIX4X4 matProj )
{
    // Set the shader
    glUseProgram( m_hMainShader );

    // Set the texture
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_hReflectionMap );
    glUniform1i( m_locMainRefMap, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_pEnvironmentMap->m_hTextureHandle );
    glUniform1i( m_locMainEnvMap, 1 );

    // Set the matrices
    FRMMATRIX4X4 matModelView     = FrmMatrixMultiply( matModel, matView );
    FRMMATRIX4X4 matModelViewProj = FrmMatrixMultiply( matModelView, matProj );
    FRMMATRIX3X3 matNormal        = FrmMatrixNormal( matModelView );

    glUniformMatrix4fv( m_locMainModelViewProjMatrix, 1, FALSE, (FLOAT32*)&matModelViewProj );
    glUniformMatrix4fv( m_locMainModelViewMatrix,     1, FALSE, (FLOAT32*)&matModelView );
    glUniformMatrix3fv( m_locMainNormalMatrix,        1, FALSE, (FLOAT32*)&matNormal );

    FRMVECTOR4 vLightPosView = FrmVector3Transform( m_vLightPosWorld, matView );
    glUniform3fv( m_locMainLightPos, 1, (FLOAT32*) &vLightPosView );

    // Render the mesh
    m_MainObject.Render();
}


//--------------------------------------------------------------------------------------
// Name: RenderSatelliteObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderSatelliteObject( FRMMATRIX4X4 matModel, FRMMATRIX4X4 matView,
                                     FRMMATRIX4X4 matProj )
{
    // Set the texture
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_pSatelliteTexture->m_hTextureHandle );

    // Set the shader
    glUseProgram( m_hSatelliteShader );

    glUniform1i( m_locSatTex, 1 );

    // Set the matrices
    FRMMATRIX4X4 matModelView     = FrmMatrixMultiply( matModel, matView );
    FRMMATRIX4X4 matModelViewProj = FrmMatrixMultiply( matModelView, matProj );
    FRMMATRIX3X3 matNormal        = FrmMatrixNormal( matModelView );

    glUniformMatrix4fv( m_locSatModelViewProjMatrix, 1, FALSE, (FLOAT32*)&matModelViewProj );
    glUniformMatrix4fv( m_locSatModelViewMatrix,     1, FALSE, (FLOAT32*)&matModelView );
    glUniformMatrix3fv( m_locSatNormalMatrix,        1, FALSE, (FLOAT32*)&matNormal );

    FRMVECTOR4 vLightPosView = FrmVector3Transform( m_vLightPosWorld, matView );
    glUniform3fv( m_locSatLightPos, 1, (FLOAT32*) &vLightPosView );

    // Render the mesh
    m_SatelliteObject.Render();
}


//--------------------------------------------------------------------------------------
// Name: GenerateEnvironmentMap()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GenerateEnvironmentMap()
{
    // Set a 90 degree FOV
    FRMMATRIX4X4 matCubeMapProj;
    matCubeMapProj = FrmMatrixPerspectiveFovRH( FRM_PI/2.0f, 1.0f, 0.1f, 20.0f );

    // Bind the cube map texture
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_hReflectionMap );

    // Setup the FBO for rendering
    glBindFramebuffer( GL_FRAMEBUFFER, m_hFBOFramebufferID );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_RENDERBUFFER, m_hFBODepthbufferID );
    glViewport( 0, 0, ENVMAP_SIZE, ENVMAP_SIZE );
        
    // Render objects into a six faces of the cubemap
    for( UINT32 face = 0; face < 6; ++face )
    {
        // Use the cubemap face as the FBO destination texture
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                                m_hReflectionMap, 0 );

        // Clear it
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Set the camera for rendering into this face
        FRMMATRIX4X4 matCubeMapView;
        matCubeMapView = FrmMatrixLookAtRH( g_CubemapCameras[face].vPosition,
                                            g_CubemapCameras[face].vLookAt,
                                            g_CubemapCameras[face].vUp );

        // Render the objects
        RenderSatelliteObject( m_matSatelliteObject[0], matCubeMapView, matCubeMapProj );
        RenderSatelliteObject( m_matSatelliteObject[1], matCubeMapView, matCubeMapProj );
    }

    // Restore the original color and viewport
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glEnable( GL_DEPTH_TEST );

    // Render the satellite objects to the environment map
    GenerateEnvironmentMap();

    // Clear the main backbuffer and depth-buffer
    glClearColor( 0.3f, 0.3f, 0.5f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Render the scene
    RenderMainObject( m_matMainObject, m_matView, m_matProj );
    RenderSatelliteObject( m_matSatelliteObject[0], m_matView, m_matProj );
    RenderSatelliteObject( m_matSatelliteObject[1], m_matView, m_matProj );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

