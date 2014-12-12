//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: Geometry Compression
// Author:                 QUALCOMM, Adreno SDK
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



//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Compressed Geometry" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
	for(int i=0;i<NUM_SHADER;++i)
	{
		m_CompressionShaders[i]			 = 0;
	}
	m_nCurShaderIndex					 = 0;
	m_MaterialDiffuse					 = FRMVECTOR3(0.6f, 0.1f, 0.1f);
    m_hModelViewMatrixLoc                = 0;
	m_hCTMatrixLoc						 = 0;
    m_hModelViewProjMatrixLoc            = 0;
    m_hNormalMatrixLoc                   = 0;
    m_hLightPositionLoc                  = 0;
    m_hMaterialAmbientLoc                = 0;
    m_hMaterialDiffuseLoc                = 0;
    m_hMaterialSpecularLoc               = 0;

    m_hOverlayShader                     = 0;

    m_hDepthShader                       = 0;
    m_hDepthMVPLoc                       = 0;

    m_pLogoTexture                       = NULL;
    m_nCurMeshIndex                      = 0;

    m_hDiffuseSpecularTexId              = 0;
    m_hDiffuseSpecularBufId              = 0;
    m_hDiffuseSpecularDepthBufId         = 0;

    m_nCurMeshIndex                      = 0;

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

    if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CompressedGeometryF16.vs",
                                                  "Samples/Shaders/CompressedGeometryF16.fs",
                                                  &m_CompressionShaders[COMPRESS_F16],
	                                            pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CompressedGeometry101012.vs",
                                                  "Samples/Shaders/CompressedGeometry101012.fs",
                                                  &m_CompressionShaders[COMPRESS_101012],
	                                            pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CompressedGeometry24.vs",
                                                  "Samples/Shaders/CompressedGeometry24.fs",
                                                  &m_CompressionShaders[COMPRESS_24],
	                                            pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CompressedGeometry8.vs",
                                                  "Samples/Shaders/CompressedGeometry8.fs",
                                                  &m_CompressionShaders[COMPRESS_8],
	                                            pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
    // Make a record of the location for each shader constant
	for(int i=0;i<NUM_SHADER;++i)
	{
		m_hModelViewMatrixLoc     = glGetUniformLocation( m_CompressionShaders[i],  "g_matModelView" );
		m_hCTMatrixLoc			  = glGetUniformLocation( m_CompressionShaders[i],  "g_matCT" );
		m_hModelViewProjMatrixLoc = glGetUniformLocation( m_CompressionShaders[i],  "g_matModelViewProj" );
		m_hNormalMatrixLoc        = glGetUniformLocation( m_CompressionShaders[i],  "g_matNormal" );
		m_hLightPositionLoc       = glGetUniformLocation( m_CompressionShaders[i],  "g_vLightPos" );
		m_hMaterialAmbientLoc     = glGetUniformLocation( m_CompressionShaders[i],  "g_Material.vAmbient" );
		m_hMaterialDiffuseLoc     = glGetUniformLocation( m_CompressionShaders[i],  "g_Material.vDiffuse" );
		m_hMaterialSpecularLoc    = glGetUniformLocation( m_CompressionShaders[i],  "g_Material.vSpecular" );

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

    m_vLightPosition = FRMVECTOR3( 0.0f, 8.0f, 0.01f );

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/CompressedGeometry.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;

	//hack in a title for first model
	m_UserInterface.SetHeading("Compressed using 16 bit Compression Transform");

    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Model" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Next Shader Compression Type" );

	m_CurMeshArray = m_MeshesF16;
    // Load the meshes
    if( FALSE == m_MeshesF16[ CUBE_MESH ].Load( "Samples/Meshes/Cube.mesh" ) )
        return FALSE;
	if( FALSE == m_MeshesF16[ CUBE_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshesF16[ SPHERE_MESH ].Load( "Samples/Meshes/Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshesF16[ SPHERE_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshesF16[ BUMPY_SPHERE_MESH ].Load( "Samples/Meshes/BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshesF16[ BUMPY_SPHERE_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshesF16[ TORUS_MESH ].Load( "Samples/Meshes/Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshesF16[ TORUS_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshesF16[ ROCKET_MESH ].Load( "Samples/Meshes/Rocket.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshesF16[ ROCKET_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;
	if( FALSE == m_Meshes101012[ CUBE_MESH ].Load( "Samples/Meshes/Cube.mesh" ) )
        return FALSE;
	if( FALSE == m_Meshes101012[ CUBE_MESH ].CompressionTransformPosition101012MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes101012[ SPHERE_MESH ].Load( "Samples/Meshes/Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes101012[ SPHERE_MESH ].CompressionTransformPosition101012MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes101012[ BUMPY_SPHERE_MESH ].Load( "Samples/Meshes/BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes101012[ BUMPY_SPHERE_MESH ].CompressionTransformPosition101012MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes101012[ TORUS_MESH ].Load( "Samples/Meshes/Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes101012[ TORUS_MESH ].CompressionTransformPosition101012MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes101012[ ROCKET_MESH ].Load( "Samples/Meshes/Rocket.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes101012[ ROCKET_MESH ].CompressionTransformPosition101012MakeDrawable() )
        return FALSE;
	if( FALSE == m_Meshes24[ CUBE_MESH ].Load( "Samples/Meshes/Cube.mesh" ) )
        return FALSE;
	if( FALSE == m_Meshes24[ CUBE_MESH ].SlidingCompressionTransformPosition24bitbitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes24[ SPHERE_MESH ].Load( "Samples/Meshes/Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes24[ SPHERE_MESH ].SlidingCompressionTransformPosition24bitbitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes24[ BUMPY_SPHERE_MESH ].Load( "Samples/Meshes/BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes24[ BUMPY_SPHERE_MESH ].SlidingCompressionTransformPosition24bitbitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes24[ TORUS_MESH ].Load( "Samples/Meshes/Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes24[ TORUS_MESH ].SlidingCompressionTransformPosition24bitbitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes24[ ROCKET_MESH ].Load( "Samples/Meshes/Rocket.mesh" ) )
        return FALSE;
	if( FALSE == m_Meshes24[ ROCKET_MESH ].SlidingCompressionTransformPosition24bitbitMakeDrawable() )
        return FALSE;
	if( FALSE == m_Meshes8[ CUBE_MESH ].Load( "Samples/Meshes/Cube.mesh" ) )
        return FALSE;
	if( FALSE == m_Meshes8[ CUBE_MESH ].CompressionTransformPosition8bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes8[ SPHERE_MESH ].Load( "Samples/Meshes/Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes8[ SPHERE_MESH ].CompressionTransformPosition8bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes8[ BUMPY_SPHERE_MESH ].Load( "Samples/Meshes/BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes8[ BUMPY_SPHERE_MESH ].CompressionTransformPosition8bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes8[ TORUS_MESH ].Load( "Samples/Meshes/Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes8[ TORUS_MESH ].CompressionTransformPosition8bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes8[ ROCKET_MESH ].Load( "Samples/Meshes/Rocket.mesh" ) )
        return FALSE;
	if( FALSE == m_Meshes8[ ROCKET_MESH ].CompressionTransformPosition8bitMakeDrawable() )
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
	for(int i=0;i<NUM_SHADER;++i)
	{
		if( m_CompressionShaders[i] ) 
			glDeleteProgram( m_CompressionShaders[i] );
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

	// Change to next shader
    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_nCurShaderIndex = ( m_nCurShaderIndex + 1 ) % NUM_SHADER;

		switch(m_nCurShaderIndex)
		{
			case COMPRESS_F16:

				m_UserInterface.SetHeading("Compressed using 16 bit Compression Transform");
				m_CurMeshArray = m_MeshesF16;
				m_MaterialDiffuse = FRMVECTOR3(0.6f, 0.1f, 0.1f);
			break;
			case COMPRESS_101012:

				m_UserInterface.SetHeading("Compressed using 101012 Compression Transform");
				m_CurMeshArray = m_Meshes101012;
				m_MaterialDiffuse = FRMVECTOR3(0.1f, 0.5f, 0.1f);
			break;
			case COMPRESS_24:

				m_UserInterface.SetHeading("Compressed using Sliding 24bit Compression Transform");
				m_CurMeshArray = m_Meshes24;
				m_MaterialDiffuse = FRMVECTOR3(0.1f, 0.1f, 0.6f);
			break;
			case COMPRESS_8:

				m_UserInterface.SetHeading("Compressed using 8 bit Compression Transform");
				m_CurMeshArray = m_Meshes8;
				m_MaterialDiffuse = FRMVECTOR3(0.6f, 0.6f, 0.6f);
			break;
		}
		
    }
    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nCurMeshIndex = ( m_nCurMeshIndex + 1 ) % NUM_MESHES;
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
        m_MeshesF16[ nMeshId ].Destroy(); 
		m_Meshes101012[ nMeshId ].Destroy(); 
		m_Meshes24[ nMeshId ].Destroy(); 
		m_Meshes8[ nMeshId ].Destroy(); 
    }
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( m_CompressionShaders[ m_nCurShaderIndex ] );

	//grab the compression transfom matrix
	FRMMATRIX4X4 ct_mat = FRMMATRIX4X4(m_CurMeshArray[ m_nCurMeshIndex ].GetCTMatrix());
	ct_mat = FrmMatrixInverse(ct_mat);
	ct_mat = FrmMatrixTranspose(ct_mat);

    // Render the mesh
    glUniform4f( m_hMaterialAmbientLoc, 0.0f, 0.0f, 0.0f, 1.0f );
	glUniform4f( m_hMaterialDiffuseLoc, m_MaterialDiffuse.x, m_MaterialDiffuse.y, m_MaterialDiffuse.z, 1.0);

    glUniform4f( m_hMaterialSpecularLoc, 1.0f, 0.6f, 0.65f, 10.0f );
    glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelView );
	glUniformMatrix4fv( m_hCTMatrixLoc, 1, FALSE, (FLOAT32*)&ct_mat );
    glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelViewProj );
    glUniformMatrix3fv( m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshNormal );
    glUniform3f( m_hLightPositionLoc, m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z );
    m_CurMeshArray[ m_nCurMeshIndex ].Render();
			
	// Update the timer
    m_Timer.MarkFrame();
    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
	

}

