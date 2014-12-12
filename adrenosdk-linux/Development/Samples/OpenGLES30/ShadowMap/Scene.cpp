//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: Shadow Mapping
//
// Pros:
// - Can be applied to non-planar surfaces (press 4 to see this in action)
// - Supports self-shadowing
//
// Cons:
// - Do not produce soft-shadows, although PCF helps with a perfomance cost
//   (press 7 to turn on PCF)
//
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
    return new CSample( "ShadowMap" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    // Initialize variables
    m_hShadowMapShader                   = 0;
    m_hShadowMapModelViewProjLoc         = 0;
    m_hShadowMapShadowMatrixLoc          = 0;
    m_hShadowMapScreenCoordMatrixLoc     = 0;
    m_hShadowMapShadowMapLoc             = 0;
    m_hShadowMapDiffuseSpecularMapLoc    = 0;
    m_hShadowMapAmbientLoc               = 0;

    m_hShadowMapPCFShader                = 0;
    m_hShadowMapPCFModelViewProjLoc      = 0;
    m_hShadowMapPCFShadowMatrixLoc       = 0;
    m_hShadowMapPCFScreenCoordMatrixLoc  = 0;
    m_hShadowMapPCFShadowMapLoc          = 0;
    m_hShadowMapPCFDiffuseSpecularMapLoc = 0;
    m_hShadowMapPCFAmbientLoc            = 0;
    m_hShadowMapPCFEpsilonLoc            = 0;

    m_hPerPixelLightingShader            = 0;
    m_hModelViewMatrixLoc                = 0;
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

    m_nCurFloorIndex                     = 0;
    m_nCurMeshIndex                      = 0;

    m_nShadowMapFBOTextureWidth          = 0;
    m_nShadowMapFBOTextureHeight         = 0;
    m_nShadowMapFBOTextureExp            = 0;
    m_hShadowMapBufId                    = 0;
    m_hShadowMapTexId                    = 0;

    m_hDiffuseSpecularTexId              = 0;
    m_hDiffuseSpecularBufId              = 0;
    m_hDiffuseSpecularDepthBufId         = 0;

    m_bShowShadowMap                     = TRUE;
    m_bUsePCF                            = TRUE;

	m_OpenGLESVersion				     = GLES3;

	m_Initialize						 = FALSE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the ShadowMap shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex",   FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/ShadowMap30.vs",
                                                      "Samples/Shaders/ShadowMap30.fs",
                                                      &m_hShadowMapShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hShadowMapModelViewProjLoc      = glGetUniformLocation( m_hShadowMapShader, "g_matModelViewProj" );
        m_hShadowMapShadowMatrixLoc       = glGetUniformLocation( m_hShadowMapShader, "g_matShadow" );
        m_hShadowMapScreenCoordMatrixLoc  = glGetUniformLocation( m_hShadowMapShader, "g_matScreenCoord" );
        m_hShadowMapShadowMapLoc          = glGetUniformLocation( m_hShadowMapShader, "g_sShadowMap" );
        m_hShadowMapDiffuseSpecularMapLoc = glGetUniformLocation( m_hShadowMapShader, "g_sDiffuseSpecularMap" );
        m_hShadowMapAmbientLoc            = glGetUniformLocation( m_hShadowMapShader, "g_vAmbient" );
    }

    // Create the ShadowMap PCF shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex",   FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/ShadowMap30.vs",
                                                      "Samples/Shaders/ShadowMap30_2x2PCF.fs",
                                                      &m_hShadowMapPCFShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hShadowMapPCFModelViewProjLoc      = glGetUniformLocation( m_hShadowMapPCFShader, "g_matModelViewProj" );
        m_hShadowMapPCFShadowMatrixLoc       = glGetUniformLocation( m_hShadowMapPCFShader, "g_matShadow" );
        m_hShadowMapPCFScreenCoordMatrixLoc  = glGetUniformLocation( m_hShadowMapPCFShader, "g_matScreenCoord" );
        m_hShadowMapPCFShadowMapLoc          = glGetUniformLocation( m_hShadowMapPCFShader, "g_sShadowMap" );
        m_hShadowMapPCFDiffuseSpecularMapLoc = glGetUniformLocation( m_hShadowMapPCFShader, "g_sDiffuseSpecularMap" );
        m_hShadowMapPCFAmbientLoc            = glGetUniformLocation( m_hShadowMapPCFShader, "g_vAmbient" );
        m_hShadowMapPCFEpsilonLoc            = glGetUniformLocation( m_hShadowMapPCFShader, "g_fEpsilon" );
    }


    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PerPixelLighting30.vs",
                                                      "Samples/Shaders/PerPixelLighting30.fs",
                                                      &m_hPerPixelLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hModelViewMatrixLoc     = glGetUniformLocation( m_hPerPixelLightingShader,  "g_matModelView" );
        m_hModelViewProjMatrixLoc = glGetUniformLocation( m_hPerPixelLightingShader,  "g_matModelViewProj" );
        m_hNormalMatrixLoc        = glGetUniformLocation( m_hPerPixelLightingShader,  "g_matNormal" );
        m_hLightPositionLoc       = glGetUniformLocation( m_hPerPixelLightingShader,  "g_vLightPos" );
        m_hMaterialAmbientLoc     = glGetUniformLocation( m_hPerPixelLightingShader,  "g_Material.vAmbient" );
        m_hMaterialDiffuseLoc     = glGetUniformLocation( m_hPerPixelLightingShader,  "g_Material.vDiffuse" );
        m_hMaterialSpecularLoc    = glGetUniformLocation( m_hPerPixelLightingShader,  "g_Material.vSpecular" );
    }

    // Create the Texture shader
    {
        
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Overlay30.vs",
                                                      "Samples/Shaders/Overlay30.fs",
                                                      &m_hOverlayShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_hOverlayScreenSizeLoc = glGetUniformLocation( m_hOverlayShader, "g_vScreenSize" );
    }

    // Create the Depth shader
    {
        
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Depth30.vs",
                                                      "Samples/Shaders/Depth30.fs",
                                                      &m_hDepthShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_hDepthMVPLoc = glGetUniformLocation( m_hDepthShader, "g_matModelViewProj" );
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
    m_bShowShadowMap = TRUE;
    m_vLightPosition = FRMVECTOR3( 0.0f, 8.0f, 0.01f );

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/ShadowMap30.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );
    m_UserInterface.AddIntVariable( &m_nShadowMapFBOTextureWidth , "Shadow Map Size", "%d" );
    m_UserInterface.AddBoolVariable( &m_bUsePCF , "PCF On", "True", "False" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Model" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Next Floor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Decrease Shadow Map Size" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Increase Shadow Map Size" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Toggle PCF" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, "Toggle Shadow Map Display" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_DPAD, "Rotate light" );

     // Load the meshes
    if( FALSE == m_Floor[ PLANE_FLOOR ].Load( "Samples/Meshes/Plane.mesh" ) )
        return FALSE;
    if( FALSE == m_Floor[ PLANE_FLOOR ].MakeDrawable() )
        return FALSE;

    if( FALSE == m_Floor[ TERRAIN_FLOOR ].Load( "Samples/Meshes/Terrain.mesh" ) )
        return FALSE;
    if( FALSE == m_Floor[ TERRAIN_FLOOR ].MakeDrawable() )
        return FALSE;

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

    // Setup the FBOs
    m_nShadowMapFBOTextureExp    = 7;
    m_nShadowMapFBOTextureWidth  = 2 << m_nShadowMapFBOTextureExp;
    m_nShadowMapFBOTextureHeight = 2 << m_nShadowMapFBOTextureExp;
    CreateShadowMapFBO();
    CreateLightingFBO();

    // Setup the camera view matrix
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 5.0f, 10.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, -2.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the floor's camera relative model view projection matrix
    FRMMATRIX4X4 matFloorScale            = FrmMatrixScale( 5.0f, 5.0f, 5.0f );
    FRMMATRIX4X4 matFloorTranslate        = FrmMatrixTranslate( 0.0f, -0.5f, 0.0f );
    m_matFloorModel                       = FrmMatrixMultiply( matFloorScale, matFloorTranslate );
    m_matCameraFloorModelView             = FrmMatrixMultiply( m_matFloorModel, m_matCameraView );
    m_matCameraFloorModelViewProj         = FrmMatrixMultiply( m_matCameraFloorModelView, m_matCameraPersp );
    m_matCameraFloorNormal                = FrmMatrixNormal( m_matCameraFloorModelView );

    // Setup the light look at and up vectors
    m_vLightLookAt = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vLightUp     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );

    // Create our scale and bias matrix that is used to convert the coordinates of vertices that are multiplied by a 
    // model view projection matrix from the range [-1, 1] to the range [0, 1], so that they can be used for texture lookups
    m_matScaleAndBias.M( 0, 0 ) = 0.5f; m_matScaleAndBias.M( 0, 1 ) = 0.0f; m_matScaleAndBias.M( 0, 2 ) = 0.0f; m_matScaleAndBias.M( 0, 3 ) = 0.0f;  
    m_matScaleAndBias.M( 1, 0 ) = 0.0f; m_matScaleAndBias.M( 1, 1 ) = 0.5f; m_matScaleAndBias.M( 1, 2 ) = 0.0f; m_matScaleAndBias.M( 1, 3 ) = 0.0f;  
    m_matScaleAndBias.M( 2, 0 ) = 0.0f; m_matScaleAndBias.M( 2, 1 ) = 0.0f; m_matScaleAndBias.M( 2, 2 ) = 0.5f; m_matScaleAndBias.M( 2, 3 ) = 0.0f;  
    m_matScaleAndBias.M( 3, 0 ) = 0.5f; m_matScaleAndBias.M( 3, 1 ) = 0.5f; m_matScaleAndBias.M( 3, 2 ) = 0.5f; m_matScaleAndBias.M( 3, 3 ) = 1.0f;  

    // Setup GL state
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

	// Initialization complete
	m_Initialize = TRUE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
	if (!m_Initialize)
	{
		return FALSE;
	}

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

    CreateShadowMapFBO();
    CreateLightingFBO();

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Delete shader programs
    if( m_hShadowMapShader )        glDeleteProgram( m_hShadowMapShader );
    if( m_hShadowMapPCFShader )     glDeleteProgram( m_hShadowMapPCFShader );
    if( m_hPerPixelLightingShader ) glDeleteProgram( m_hPerPixelLightingShader );
    if( m_hOverlayShader )          glDeleteProgram( m_hOverlayShader );
    if( m_hDepthShader )            glDeleteProgram( m_hDepthShader );

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    // Free mesh and FBO resources
    FreeMeshes();
    FreeShadowMapFBO();
    FreeLightingFBO();
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

    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nCurMeshIndex = ( m_nCurMeshIndex + 1 ) % NUM_MESHES;
    }

    // Change to next floor mesh
    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_nCurFloorIndex = ( m_nCurFloorIndex + 1 ) % NUM_FLOORS;
    }

    // Lower shadow map size
    if( nPressedButtons & INPUT_KEY_3 )
    {
        if( m_nShadowMapFBOTextureExp > 5 )
        {
            m_nShadowMapFBOTextureExp--;
            m_nShadowMapFBOTextureWidth  = 2 << m_nShadowMapFBOTextureExp;
            m_nShadowMapFBOTextureHeight = 2 << m_nShadowMapFBOTextureExp; 
            CreateShadowMapFBO();
        }
    }

    // Increase shadow map size
    if( nPressedButtons & INPUT_KEY_4 )
    {
        if( m_nShadowMapFBOTextureExp < 8 )
        {
            m_nShadowMapFBOTextureExp++;
            m_nShadowMapFBOTextureWidth  = 2 << m_nShadowMapFBOTextureExp;
            m_nShadowMapFBOTextureHeight = 2 << m_nShadowMapFBOTextureExp; 
            CreateShadowMapFBO();
        }
    }

    // Toggle PCF
    if( nPressedButtons & INPUT_KEY_5 )
    {
        m_bUsePCF = !m_bUsePCF;
    }

    // Toggle display of shadow map
    if( nPressedButtons & INPUT_KEY_6 )
    {
        m_bShowShadowMap = !m_bShowShadowMap;
    }

    // Setup the mesh's camera relative model view, model view projection, and normal matrices
	FRMVECTOR3 yAxis = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
	FRMVECTOR3 xAxis = FRMVECTOR3( 1.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 matMeshRotate    = FrmMatrixRotate( fTime, yAxis );
    FRMMATRIX4X4 matMeshRotate2   = FrmMatrixRotate( fTime, xAxis );
    FRMMATRIX4X4 matMeshModel     = FrmMatrixTranslate( 0.0f, 2.0f, 0.0f );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate, matMeshModel );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate2, matMeshModel );
    m_matCameraMeshModelView      = FrmMatrixMultiply( matMeshModel, m_matCameraView );
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

    // Setup the floor and mesh's light relative model view projection matrices
    FRMMATRIX4X4 matLightView          = FrmMatrixLookAtRH( m_vLightPosition, m_vLightLookAt, m_vLightUp );
    FRMMATRIX4X4 matLightModelViewProj = FrmMatrixMultiply( matLightView, m_matLightPersp );
    m_matLightFloorModelViewProj       = FrmMatrixMultiply( m_matFloorModel, matLightModelViewProj );
    m_matLightMeshModelViewProj        = FrmMatrixMultiply( matMeshModel, matLightModelViewProj );

    // Setup our matrices that are used to convert vertices into shadow map lookup coordinates
    m_matFloorShadowMatrix = FrmMatrixMultiply( m_matLightFloorModelViewProj, m_matScaleAndBias );
    m_matMeshShadowMatrix = FrmMatrixMultiply( m_matLightMeshModelViewProj, m_matScaleAndBias );

    // Setup our matrices that are used to convert vertices into diffuse/specular map lookup coordinates
    m_matFloorScreenCoordMatrix = FrmMatrixMultiply( m_matCameraFloorModelViewProj, m_matScaleAndBias );
    m_matMeshScreenCoordMatrix = FrmMatrixMultiply( m_matCameraMeshModelViewProj, m_matScaleAndBias );
}


//--------------------------------------------------------------------------------------
// Name: CheckFrameBufferStatus()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CheckFrameBufferStatus()
{
	GLenum nStatus;
    nStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch( nStatus )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return TRUE;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            return FALSE;
        default:
            FrmAssert(0);
    }

    return FALSE;
}


//--------------------------------------------------------------------------------------
// Name: CreateShadowMapFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CreateShadowMapFBO()
{
    FreeShadowMapFBO();

    // Create shadow texture
    {   
        glGenTextures( 1, &m_hShadowMapTexId );
        glBindTexture( GL_TEXTURE_2D, m_hShadowMapTexId );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
        
        // Setup hardware comparison
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        
        glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                  m_nShadowMapFBOTextureWidth, m_nShadowMapFBOTextureHeight, 
                  0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL );
    }

    glGenFramebuffers( 1, &m_hShadowMapBufId );
    glBindFramebuffer( GL_FRAMEBUFFER, m_hShadowMapBufId );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_hShadowMapTexId, 0 );


	//ATI cards need you to specify no color attached
	GLenum noColorAttach = GL_NONE;
	glDrawBuffers( 1, &noColorAttach );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_hShadowMapTexId );
 
    CheckFrameBufferStatus();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


//--------------------------------------------------------------------------------------
// Name: FreeMeshes()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeMeshes()
{
    for( INT32 nFloorMeshId = 0; nFloorMeshId < NUM_FLOORS; ++nFloorMeshId )
    {
        m_Floor[ nFloorMeshId ].Destroy();
    }

    for( INT32 nMeshId = 0; nMeshId < NUM_MESHES; ++nMeshId )
    {
        m_Meshes[ nMeshId ].Destroy(); 
    }
}


//--------------------------------------------------------------------------------------
// Name: FreeShadowMapFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeShadowMapFBO()
{
    if( m_hShadowMapBufId )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, m_hShadowMapBufId );
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    }
    if( m_hShadowMapBufId ) glDeleteFramebuffers( 1, &m_hShadowMapBufId );
    if( m_hShadowMapTexId ) glDeleteTextures( 1, &m_hShadowMapTexId );
}


//--------------------------------------------------------------------------------------
// Name: GetShadowMapFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GetShadowMapFBO()
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_hShadowMapBufId );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_hShadowMapTexId, 0 );
    glViewport( 0, 0, m_nShadowMapFBOTextureWidth, m_nShadowMapFBOTextureHeight );
}


//--------------------------------------------------------------------------------------
// Name: CreateLightingFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CreateLightingFBO()
{
    FreeLightingFBO();

    glGenRenderbuffers( 1, &m_hDiffuseSpecularDepthBufId );
    glBindRenderbuffer( GL_RENDERBUFFER, m_hDiffuseSpecularDepthBufId );    
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_nWidth, m_nHeight );

    glGenFramebuffers( 1, &m_hDiffuseSpecularBufId );
    glBindFramebuffer( GL_FRAMEBUFFER, m_hDiffuseSpecularBufId );

    glGenTextures( 1, &m_hDiffuseSpecularTexId );
    glBindTexture( GL_TEXTURE_2D, m_hDiffuseSpecularTexId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_nWidth, m_nHeight, 0, GL_RGB, 
                  GL_UNSIGNED_BYTE, NULL );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_hDiffuseSpecularTexId );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hDiffuseSpecularTexId, 0 );

    CheckFrameBufferStatus();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


//--------------------------------------------------------------------------------------
// Name: FreeLightingFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeLightingFBO()
{
    if( m_hDiffuseSpecularBufId )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, m_hDiffuseSpecularBufId );
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    }

    if( m_hDiffuseSpecularBufId )      glDeleteFramebuffers( 1, &m_hDiffuseSpecularBufId );
    if( m_hDiffuseSpecularTexId )      glDeleteTextures( 1, &m_hDiffuseSpecularTexId );
    if( m_hDiffuseSpecularDepthBufId ) glDeleteRenderbuffers( 1, &m_hDiffuseSpecularDepthBufId );
}


//--------------------------------------------------------------------------------------
// Name: GetLightingFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GetLightingFBO()
{
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_hDiffuseSpecularTexId );
    glBindFramebuffer( GL_FRAMEBUFFER, m_hDiffuseSpecularBufId );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hDiffuseSpecularTexId, 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_hDiffuseSpecularDepthBufId );
    glViewport( 0, 0, m_nWidth, m_nHeight );
}


//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad()
{
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    const FLOAT32 Quad[] =
    {
        -1.0, -1.0f, 0.0f, 1.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
        +1.0, -1.0f, 1.0f, 1.0f,
        +1.0, +1.0f, 1.0f, 0.0f,
    };


	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 16, Quad, GL_DYNAMIC_DRAW);
	glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, 0 );
	glEnableVertexAttribArray( 0 );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glDisableVertexAttribArray( 0 );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


    glDisable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name: RenderShadowMapToFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderShadowMapToFBO()
{
    GetShadowMapFBO();
    glDisable( GL_CULL_FACE );

    glClear( GL_DEPTH_BUFFER_BIT );

    glEnable( GL_POLYGON_OFFSET_FILL );
	
#if defined(_WIN32)
	glPolygonOffset( 2.0f, 100.0f );
#elif LINUX 
	glPolygonOffset( 2.0f, 100.0f );
#else
    glPolygonOffset( 0.1f, 1.0f );
#endif

    // Render the floor along with its depth
    glUseProgram( m_hDepthShader );
    glUniformMatrix4fv( m_hDepthMVPLoc, 1, FALSE, (FLOAT32*) &m_matLightFloorModelViewProj );
    m_Floor[ m_nCurFloorIndex ].Render();

    // Draw the Mesh
    glUniformMatrix4fv( m_hDepthMVPLoc, 1, FALSE, (FLOAT32*) &m_matLightMeshModelViewProj );
    m_Meshes[ m_nCurMeshIndex ].Render();    

    glDisable( GL_POLYGON_OFFSET_FILL );

    glEnable( GL_CULL_FACE );
}


//--------------------------------------------------------------------------------------
// Name: RenderLightingToFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderLightingToFBO()
{
    GetLightingFBO();

    glClear( GL_DEPTH_BUFFER_BIT );

    glUseProgram( m_hPerPixelLightingShader );

    // Render the floor
    glUniform4f( m_hMaterialAmbientLoc, 0.0f, 0.0f, 0.0f, 1.0f );
    glUniform4f( m_hMaterialDiffuseLoc, 0.5f, 0.5f, 0.8f, 1.0f );
    glUniform4f( m_hMaterialSpecularLoc, 0.0f, 0.0f, 0.0f, 0.0f );
    glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraFloorModelView );
    glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraFloorModelViewProj );
    glUniformMatrix3fv( m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraFloorNormal );
    glUniform3f( m_hLightPositionLoc, m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z );
    m_Floor[ m_nCurFloorIndex ].Render();

    // Render the mesh
    glUniform4f( m_hMaterialAmbientLoc, 0.0f, 0.0f, 0.0f, 1.0f );
    glUniform4f( m_hMaterialDiffuseLoc, 0.6f, 0.1f, 0.1f, 1.0f );
    glUniform4f( m_hMaterialSpecularLoc, 1.0f, 0.6f, 0.65f, 10.0f );
    glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelView );
    glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelViewProj );
    glUniformMatrix3fv( m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshNormal );
    glUniform3f( m_hLightPositionLoc, m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z );
    m_Meshes[ m_nCurMeshIndex ].Render();
}


//--------------------------------------------------------------------------------------
// Name: RenderBlendedLightingAndShadowMap()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderBlendedLightingAndShadowMap()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    if( m_bUsePCF )
    {
        glUseProgram( m_hShadowMapPCFShader );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, m_hShadowMapTexId );
        glUniform1i( m_hShadowMapPCFShadowMapLoc, 0 );

        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, m_hDiffuseSpecularTexId );
        glUniform1i( m_hShadowMapPCFDiffuseSpecularMapLoc, 1 );
        glUniform3f( m_hShadowMapPCFAmbientLoc, 0.2f, 0.2f, 0.2f );
        glUniform1f( m_hShadowMapPCFEpsilonLoc, 0.0035f );

        glUniformMatrix4fv( m_hShadowMapPCFModelViewProjLoc, 1, FALSE, (FLOAT32*) &m_matCameraFloorModelViewProj );
        glUniformMatrix4fv( m_hShadowMapPCFShadowMatrixLoc, 1, FALSE, (FLOAT32*) &m_matFloorShadowMatrix );
        glUniformMatrix4fv( m_hShadowMapPCFScreenCoordMatrixLoc, 1, FALSE, (FLOAT32*) &m_matFloorScreenCoordMatrix );
        m_Floor[ m_nCurFloorIndex ].Render();

        glUniformMatrix4fv( m_hShadowMapPCFModelViewProjLoc, 1, FALSE, (FLOAT32*) &m_matCameraMeshModelViewProj );
        glUniformMatrix4fv( m_hShadowMapPCFShadowMatrixLoc, 1, FALSE, (FLOAT32*) &m_matMeshShadowMatrix );
        glUniformMatrix4fv( m_hShadowMapPCFScreenCoordMatrixLoc, 1, FALSE, (FLOAT32*) &m_matMeshScreenCoordMatrix );
        m_Meshes[ m_nCurMeshIndex ].Render();
    }
    else
    {
        glUseProgram( m_hShadowMapShader );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, m_hShadowMapTexId );
        glUniform1i( m_hShadowMapShadowMapLoc, 0 );

        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, m_hDiffuseSpecularTexId );
        glUniform1i( m_hShadowMapDiffuseSpecularMapLoc, 1 );
        glUniform3f( m_hShadowMapAmbientLoc, 0.2f, 0.2f, 0.2f );

        glUniformMatrix4fv( m_hShadowMapModelViewProjLoc, 1, FALSE, (FLOAT32*) &m_matCameraFloorModelViewProj );
        glUniformMatrix4fv( m_hShadowMapShadowMatrixLoc, 1, FALSE, (FLOAT32*) &m_matFloorShadowMatrix );
        glUniformMatrix4fv( m_hShadowMapScreenCoordMatrixLoc, 1, FALSE, (FLOAT32*) &m_matFloorScreenCoordMatrix );
        m_Floor[ m_nCurFloorIndex ].Render();

        glUniformMatrix4fv( m_hShadowMapModelViewProjLoc, 1, FALSE, (FLOAT32*) &m_matCameraMeshModelViewProj );
        glUniformMatrix4fv( m_hShadowMapShadowMatrixLoc, 1, FALSE, (FLOAT32*) &m_matMeshShadowMatrix );
        glUniformMatrix4fv( m_hShadowMapScreenCoordMatrixLoc, 1, FALSE, (FLOAT32*) &m_matMeshScreenCoordMatrix );
        m_Meshes[ m_nCurMeshIndex ].Render();
    }
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    RenderShadowMapToFBO();
    RenderLightingToFBO();
	RenderBlendedLightingAndShadowMap();

    if( m_bShowShadowMap )
    {
		// must turn off texture comparison, otherwise rendering the texture
		//	to screen will be wrong
		glBindTexture( GL_TEXTURE_2D, m_hShadowMapTexId );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glBindTexture( GL_TEXTURE_2D, 0 );
    
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        FrmRenderTextureToScreen_GLES( 0, 0, m_nShadowMapFBOTextureWidth, m_nShadowMapFBOTextureHeight, m_hShadowMapTexId,
                                       m_hOverlayShader, m_hOverlayScreenSizeLoc );
    
		// Turn comparison back on for hardware shadow map comparison
		glBindTexture( GL_TEXTURE_2D, m_hShadowMapTexId );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glBindTexture( GL_TEXTURE_2D, 0 );
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

