//--------------------------------------------------------------------------------------
// Author:                 QUALCOMM, Adreno SDK
// Desc: Physically Based Lighting
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>

#include <OpenGLES/FrmGLES3.h>  // OpenGL ES 3 headers here

#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <OpenGLES/FrmUtilsGLES.h>
#include "Scene.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Physically Bassed Lighting" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
	m_nCurShaderIndex					 = 0;

    m_hOverlayShader                     = 0;

    m_hDepthShader                       = 0;
    m_hDepthMVPLoc                       = 0;

    m_pLogoTexture                       = NULL;
    m_nCurMeshIndex                      = 0;

    m_hDiffuseSpecularTexId              = 0;
    m_hDiffuseSpecularBufId              = 0;
    m_hDiffuseSpecularDepthBufId         = 0;

	m_SkyColor							 = FRMVECTOR3(0.9f, 0.1f, 0.1f);
	m_GroundColor						 = FRMVECTOR3(0.1f, 0.1f, 0.1f);
	m_MaterialSpecular 				     = FRMVECTOR3(0.6f, 0.5f, 0.5f);
	m_MaterialDiffuse 					 = FRMVECTOR3(0.9f, 0.3f, 0.3f);
	m_LightColor 						 = FRMVECTOR3(0.9f, 0.9f, 0.9f);
	m_MaterialGlossiness				 = 0.4f;

	m_OpenGLESVersion				     = GLES3;

}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "g_vPositionOS", FRM_VERTEX_POSITION },
        { "g_vNormalOS",   FRM_VERTEX_NORMAL }
    };
    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PhysicallyBasedLighting30.vs",
                                                  "Samples/Shaders/CookTorrance30.fs",
                                                  &m_Shaders[COOK_TORRANCE], pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PhysicallyBasedLighting30.vs",
                                                  "Samples/Shaders/CookTorranceFull30.fs",
                                                  &m_Shaders[COOK_TORRANCE_FULL], pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PhysicallyBasedLighting30.vs",
                                                  "Samples/Shaders/KemenSzirmayKalos30.fs",
                                                  &m_Shaders[KEMEN_SZIRMAY_KALOS], pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
    if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PhysicallyBasedLighting30.vs",
                                                  "Samples/Shaders/PhysicallyBasedLightingOptimized30.fs",
                                                  &m_Shaders[OPTIMIZED], pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
	 if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PhysicallyBasedLighting30.vs",
                                                  "Samples/Shaders/BlinnPhong30.fs",
                                                  &m_Shaders[BLINN_PHONG], pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

	for(int i=0; i<NUM_SHADERS; ++i)
	{
		m_Uniforms[i].m_hModelViewMatrixLoc     = glGetUniformLocation( m_Shaders[i],  "g_matModelView" );
		m_Uniforms[i].m_hModelViewProjMatrixLoc = glGetUniformLocation( m_Shaders[i],  "g_matModelViewProj" );
		m_Uniforms[i].m_hNormalMatrixLoc        = glGetUniformLocation( m_Shaders[i],  "g_matNormal" );
		m_Uniforms[i].m_hLightPositionLoc       = glGetUniformLocation( m_Shaders[i],  "g_vLightPos" );
		m_Uniforms[i].m_SkyColorLoc			    = glGetUniformLocation( m_Shaders[i],  "g_SkyColor" );
		m_Uniforms[i].m_GroundColorLoc          = glGetUniformLocation( m_Shaders[i],  "g_GroundColor" );
		m_Uniforms[i].m_MaterialSpecularLoc	    = glGetUniformLocation( m_Shaders[i],  "g_MaterialSpecular" );
		m_Uniforms[i].m_MaterialDiffuseLoc	    = glGetUniformLocation( m_Shaders[i],  "g_MaterialDiffuse" );
		m_Uniforms[i].m_LightColorLoc		    = glGetUniformLocation( m_Shaders[i],  "g_LightColor" );
		m_Uniforms[i].m_MaterialGlossinessLoc   = glGetUniformLocation( m_Shaders[i],  "g_MaterialGlossiness" );
	}

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Initialize sample variables
    m_nCurMeshIndex  = CUBE_MESH;

    m_vLightPosition = FRMVECTOR3( 0.0f, 4.0f, 0.0f );

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/PhysicallyBasedLighting.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;

	//hack in a title for first model
	m_UserInterface.SetHeading("Cook Torrance");

    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Model" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Next Shader" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Increase Material Glossinesss" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Decrease Material Glossinesss" );

     // Load the meshes
    if( FALSE == m_Meshes[ CUBE_MESH ].Load( "Samples/Meshes/Cube.mesh" ) )
        return FALSE;
	if( FALSE == m_Meshes[ CUBE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].Load( "Samples/Meshes/Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].Load( "Samples/Meshes/BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].Load( "Samples/Meshes/Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].Load( "Samples/Meshes/Rocket.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Initialize the camera and light perspective matrices
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );
    m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Setup the camera view matrix
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 0.0f, 10.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the light look at and up vectors
    m_vLightLookAt = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vLightUp     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );

    // Setup GL state
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );
    m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matCameraPersp.M(0,0) *= fAspect;
        m_matCameraPersp.M(1,1) *= fAspect;
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
	for(int i=0;i<NUM_SHADERS;++i)
	{
		if( m_Shaders[i] ) 
			glDeleteProgram( m_Shaders[i] );
	}
    
    if( m_hOverlayShader ) glDeleteProgram( m_hOverlayShader );

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    // Free mesh
    FreeMeshes();
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
    {
        m_UserInterface.AdvanceState();
    }

    // Move shadow left
    if( nButtons & INPUT_DPAD_LEFT )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow right
    if( nButtons & INPUT_DPAD_RIGHT )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow up
    if( nButtons & INPUT_DPAD_DOWN )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow down
    if( nButtons & INPUT_DPAD_UP )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

	m_MaterialDiffuse = FRMVECTOR3(0.6f, 0.1f, 0.1f);
	
    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nCurMeshIndex = ( m_nCurMeshIndex + 1 ) % NUM_MESHES;
    }

	if( nPressedButtons & INPUT_KEY_2 )
    {
        m_nCurShaderIndex = ( m_nCurShaderIndex + 1 ) % NUM_SHADERS;
		switch(m_nCurShaderIndex)
		{
		case COOK_TORRANCE:
			m_UserInterface.SetHeading("Cook Torrance");
		break;
		case COOK_TORRANCE_FULL:
			m_UserInterface.SetHeading("Cook Torrance Full");
		break;
		case KEMEN_SZIRMAY_KALOS:
			m_UserInterface.SetHeading("Kemen Szirmay Kalos");
		break;
		case OPTIMIZED:
			m_UserInterface.SetHeading("Optimized");
		break;
		case BLINN_PHONG:
			m_UserInterface.SetHeading("Blinn Phong");
		break;
		default:
				FrmAssert(0);
		}
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_MaterialGlossiness += 0.01f;
    }
	if( nButtons & INPUT_KEY_4 )
    {
        m_MaterialGlossiness -= 0.01f;

		m_MaterialGlossiness = max(m_MaterialGlossiness, 0.0f);
    }

    // Setup the mesh's camera relative model view, model view projection, and normal matrices
	FRMVECTOR3 yAxis = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
	FRMVECTOR3 xAxis = FRMVECTOR3( 1.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 matMeshRotate    = FrmMatrixRotate( fTime, yAxis );
    FRMMATRIX4X4 matMeshRotate2   = FrmMatrixRotate( fTime, xAxis );
    FRMMATRIX4X4 matMeshModel     = FrmMatrixTranslate( 0.0f, 0.0f, 0.0f );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate, matMeshModel );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate2, matMeshModel );

    m_matCameraMeshModelView      = FrmMatrixMultiply( matMeshModel, m_matCameraView );
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );
}


//--------------------------------------------------------------------------------------
// Name: CheckFrameBufferStatus()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CheckFrameBufferStatus()
{
    GLenum nStatus;
    nStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch( nStatus )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            break;
        default:
            FrmAssert(0);
    }
}

//--------------------------------------------------------------------------------------
// Name: FreeMeshes()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeMeshes()
{
    for( INT32 nMeshId = 0; nMeshId < NUM_MESHES; ++nMeshId )
    {
        m_Meshes[ nMeshId ].Destroy(); 
    }
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( m_Shaders[ m_nCurShaderIndex ]);

    // Render the mesh

    glUniformMatrix4fv( m_Uniforms[ m_nCurShaderIndex ].m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelView );
    glUniformMatrix4fv( m_Uniforms[ m_nCurShaderIndex ].m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelViewProj );
    glUniformMatrix3fv( m_Uniforms[ m_nCurShaderIndex ].m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshNormal );
    glUniform3f( m_Uniforms[ m_nCurShaderIndex ].m_hLightPositionLoc, m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z );

	glUniform3fv( m_Uniforms[ m_nCurShaderIndex ].m_SkyColorLoc, 1, (FLOAT32*)&m_SkyColor );
	glUniform3fv( m_Uniforms[ m_nCurShaderIndex ].m_GroundColorLoc, 1, (FLOAT32*)&m_GroundColor );
	glUniform3fv( m_Uniforms[ m_nCurShaderIndex ].m_MaterialSpecularLoc, 1, (FLOAT32*)&m_MaterialSpecular );
	glUniform3fv( m_Uniforms[ m_nCurShaderIndex ].m_MaterialDiffuseLoc, 1, (FLOAT32*)&m_MaterialDiffuse );
	glUniform3fv( m_Uniforms[ m_nCurShaderIndex ].m_LightColorLoc, 1, (FLOAT32*)&m_LightColor );
	glUniform1f( m_Uniforms[ m_nCurShaderIndex ].m_MaterialGlossinessLoc, m_MaterialGlossiness );

    m_Meshes[ m_nCurMeshIndex ].Render();
			
	// Update the timer
    m_Timer.MarkFrame();
    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

