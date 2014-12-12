//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: Projective Shadows
//
// Pros:
// - Computationally inexpensive
//
// Cons:
// - Can only be applied to planar surfaces
// - No self-shadowing (rotate the light below the ground plane to see this problem)
// - False shadows (press 5 to see a situaution that creates a false shadow)
// - Do not produce soft-shadows
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
    return new CSample( "ProjectiveShadows" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    // Initialize variables
    m_hPerPixelLightingShader = 0;
    m_hModelViewMatrixLoc     = 0;
    m_hModelViewProjMatrixLoc = 0;
    m_hNormalMatrixLoc        = 0;
    m_hLightPositionLoc       = 0;
    m_hMaterialAmbientLoc     = 0;
    m_hMaterialDiffuseLoc     = 0;
    m_hMaterialSpecularLoc    = 0;

    m_hConstantShader         = 0;
    m_hConstantMVPLoc         = 0;
    m_hConstantColorLoc       = 0;

    m_pLogoTexture            = NULL;

    m_nCurMeshIndex           = 0;

    m_bUseStencil             = FALSE;
    m_bShowFalseShadow        = FALSE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PerPixelLighting.vs",
                                                      "Samples/Shaders/PerPixelLighting.fs",
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

    // Create the Constant shader
    {
        
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vVertex", FRM_VERTEX_POSITION },
			{ "g_vColor",  FRM_VERTEX_COLOR0 }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Constant.vs",
                                                      "Samples/Shaders/Constant.fs",
                                                      &m_hConstantShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_hConstantMVPLoc   = glGetUniformLocation( m_hConstantShader, "g_mModelViewProjectionMatrix" );
        m_hConstantColorLoc = glGetUniformLocation( m_hConstantShader, "g_vColor" );
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
    m_vLightPosition = FRMVECTOR3( 0.0f, 8.0f, 0.0f );
    m_vPlane         = FRMVECTOR4( 0.0f, 1.0f, 0.0f, 0.0f );
    m_bUseStencil    = TRUE;

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/ProjectiveShadows.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );
    m_UserInterface.AddFloatVariable( &m_vLightPosition.x, "Light Position" );
    m_UserInterface.AddBoolVariable( &m_bUseStencil, "Use Stencil Buffer" );
    m_UserInterface.AddBoolVariable( &m_bShowFalseShadow, "Show False Shadow" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Mesh" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Toggle Stencil Buffer" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Toggle False Shadow" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_UP, "Rotate light CCW about x-axis" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_DOWN, "Rotate light CW about x-axis" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_LEFT, "Rotate light CCW about z-axis" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_RIGHT, "Rotate light CW about z-axis" );

     // Load the meshes
    if( FALSE == m_Floor.Load( "Samples/Meshes/Plane.mesh" ) )
        return FALSE;
    if( FALSE == m_Floor.MakeDrawable() )
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

    // Initialize the camera perspective matrix
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Setup the camera view matrix
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 4.0f, 10.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matCameraView            = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the floor's camera relative model view projection matrix
    FRMMATRIX4X4 matFloorScale            = FrmMatrixScale( 5.0f, 5.0f, 5.0f );
    FRMMATRIX4X4 matFloorTranslate        = FrmMatrixTranslate( 0.0f, -0.1f, 0.0f );
    FRMMATRIX4X4 matFloorModel            = FrmMatrixMultiply( matFloorScale, matFloorTranslate );
    m_matCameraFloorModelView             = FrmMatrixMultiply( matFloorModel, m_matCameraView );
    m_matCameraFloorModelViewProj         = FrmMatrixMultiply( m_matCameraFloorModelView, m_matCameraPersp );
    m_matCameraFloorNormal                = FrmMatrixNormal( m_matCameraFloorModelView );

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
    if( m_hPerPixelLightingShader ) glDeleteProgram( m_hPerPixelLightingShader );
    if( m_hConstantShader )         glDeleteProgram( m_hConstantShader );

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();
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

    // Move shadow down
    if( nButtons & INPUT_DPAD_DOWN )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow up
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

    // Toggle the use of the stencil buffer to clip the projective shadows to the ground plane
    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_bUseStencil = 1 - m_bUseStencil;
    }

    // Shows an example of a false shadow that occurs when the occluder is located below the ground plane
    if( nPressedButtons & INPUT_KEY_3 )
    {
        m_bShowFalseShadow = 1 - m_bShowFalseShadow;
    }

    // Setup the mesh's camera relative model view, model view projection, and normal matrices
    FRMVECTOR3 vTmp = FRMVECTOR3(0.0f, 1.0f, 0.0f);
	FRMMATRIX4X4 matMeshRotate    = FrmMatrixRotate( fTime, vTmp );
	vTmp = FRMVECTOR3(1.0f, 0.0f, 0.0f);
	FRMMATRIX4X4 matMeshRotate2   = FrmMatrixRotate( fTime, vTmp );
    FRMMATRIX4X4 matMeshTranslate;
    
    if( m_bShowFalseShadow )
    {
        matMeshTranslate = FrmMatrixTranslate( 0.0f, -3.0f, 0.0f );
    }
    else
    {
        matMeshTranslate = FrmMatrixTranslate( 0.0f, 2.0f, 0.0f );
    }

    FRMMATRIX4X4 matMeshModel     = FrmMatrixMultiply( matMeshRotate, matMeshTranslate );
    m_matMeshModel                = FrmMatrixMultiply( matMeshRotate2, matMeshModel );
    m_matCameraMeshModelView      = FrmMatrixMultiply( m_matMeshModel, m_matCameraView );
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );
}


//--------------------------------------------------------------------------------------
// Name: RenderFloor()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderFloor()
{
    glUseProgram( m_hPerPixelLightingShader );
    glUniform4f( m_hMaterialAmbientLoc, 0.2f, 0.2f, 0.2f, 1.0f );
    glUniform4f( m_hMaterialDiffuseLoc, 0.5f, 0.5f, 0.8f, 1.0f );
    glUniform4f( m_hMaterialSpecularLoc, 0.0f, 0.0f, 0.0f, 0.0f );
    glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraFloorModelView );
    glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraFloorModelViewProj );
    glUniformMatrix3fv( m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraFloorNormal );
    glUniform3f( m_hLightPositionLoc, m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z );

    m_Floor.Render();
}


//--------------------------------------------------------------------------------------
// Name: RenderMesh()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderMesh()
{
    glUseProgram( m_hPerPixelLightingShader );
    glUniform4f( m_hMaterialAmbientLoc, 0.2f, 0.2f, 0.2f, 1.0f );
    glUniform4f( m_hMaterialDiffuseLoc, 0.6f, 0.1f, 0.1f, 1.0f );
    glUniform4f( m_hMaterialSpecularLoc, 1.0f, 0.6f, 0.65f, 10.0f );
    glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelView );
    glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelViewProj );
    glUniformMatrix3fv( m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshNormal );
    glUniform3f( m_hLightPositionLoc, m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z );

    m_Meshes[ m_nCurMeshIndex ].Render();
}


//--------------------------------------------------------------------------------------
// Name: RenderProjectiveShadows()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderProjectiveShadows()
{
    FRMMATRIX4X4 matShadow;

    // Calculate the dot-product between the plane and the light's position
    FLOAT32 fLightW = 1.0f;
    FLOAT32 fDot = m_vPlane.x * m_vLightPosition.x + 
                   m_vPlane.y * m_vLightPosition.y + 
                   m_vPlane.z * m_vLightPosition.z + 
                   m_vPlane.w * fLightW;

    // First column
    matShadow.M( 0, 0 ) = fDot - m_vLightPosition.x * m_vPlane.x;
    matShadow.M( 1, 0 ) = 0.0f - m_vLightPosition.x * m_vPlane.y;
    matShadow.M( 2, 0 ) = 0.0f - m_vLightPosition.x * m_vPlane.z;
    matShadow.M( 3, 0 ) = 0.0f - m_vLightPosition.x * m_vPlane.w;

    // Second column
    matShadow.M( 0, 1 ) = 0.0f - m_vLightPosition.y * m_vPlane.x;
    matShadow.M( 1, 1 ) = fDot - m_vLightPosition.y * m_vPlane.y;
    matShadow.M( 2, 1 ) = 0.0f - m_vLightPosition.y * m_vPlane.z;
    matShadow.M( 3, 1 ) = 0.0f - m_vLightPosition.y * m_vPlane.w;

    // Third column
    matShadow.M( 0, 2 ) = 0.0f - m_vLightPosition.z * m_vPlane.x;
    matShadow.M( 1, 2 ) = 0.0f - m_vLightPosition.z * m_vPlane.y;
    matShadow.M( 2, 2 ) = fDot - m_vLightPosition.z * m_vPlane.z;
    matShadow.M( 3, 2 ) = 0.0f - m_vLightPosition.z * m_vPlane.w;

    // Fourth column
    matShadow.M( 0, 3 ) = 0.0f - fLightW * m_vPlane.x;
    matShadow.M( 1, 3 ) = 0.0f - fLightW * m_vPlane.y;
    matShadow.M( 2, 3 ) = 0.0f - fLightW * m_vPlane.z;
    matShadow.M( 3, 3 ) = fDot - fLightW * m_vPlane.w;

	FRMMATRIX4X4 mTmp = FrmMatrixMultiply(m_matMeshModel, matShadow);
	mTmp = FrmMatrixMultiply(mTmp, m_matCameraView);
    matShadow = FrmMatrixMultiply( mTmp, m_matCameraPersp );

    glDisable( GL_DEPTH_TEST );

    glUseProgram( m_hConstantShader );
    glUniformMatrix4fv( m_hConstantMVPLoc, 1, FALSE, (FLOAT32*)&matShadow );
    glUniform4f( m_hConstantColorLoc, 0.2f, 0.2f, 0.2f, 1.0f );

    m_Meshes[ m_nCurMeshIndex ].Render();

    glEnable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    if( m_bUseStencil )
    {
        glEnable( GL_STENCIL_TEST );
        glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFF );
        glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

        RenderFloor();

        glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFF );
        glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

        RenderProjectiveShadows();
        glDisable( GL_STENCIL_TEST );

        RenderMesh();
    }
    else
    {
        RenderFloor();
        RenderProjectiveShadows();
        RenderMesh();
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

