//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: Shadow Volume
//
// Pros:
// - Can be applied to non-planar surfaces (press 4 to see this in action)
// - Supports self-shadowing
//
// Cons:
// - Do not produce soft-shadows
//
// Comment:
// This implementation uses a directional light source, the algorithm can also be
// implemented with a point light source with a performance cost.
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
#include <OpenGLES/FrmUtilsGLES.h>
#include "Scene.h"


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "ShadowVolume" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hShadowVolumeShader            = 0;
    m_hShadowVolumeLightDirectionLoc = 0;
    m_hShadowVolumeNormalLoc         = 0;
    m_hShadowVolumeModelViewProjLoc  = 0;
    m_hShadowVolumeViewProjLoc       = 0;

    m_hPerPixelLightingShader        = 0;
    m_hModelViewMatrixLoc            = 0;
    m_hModelViewProjMatrixLoc        = 0;
    m_hNormalMatrixLoc               = 0;
    m_hLightPositionLoc              = 0;
    m_hMaterialAmbientLoc            = 0;
    m_hMaterialDiffuseLoc            = 0;
    m_hMaterialSpecularLoc           = 0;

    m_hConstantShader                = 0;
    m_hConstantMVPLoc                = 0;
    m_hConstantColorLoc              = 0;

    m_pLogoTexture                   = NULL;

    m_nCurFloorIndex                 = 0;
    m_nCurMeshIndex                  = 0;

    m_bVisualizeShadowVolume         = FALSE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the Shadow Volume shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vVertex", FRM_VERTEX_POSITION },
            { "g_vNormal", FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/ShadowVolume.vs",
                                                      "Samples/Shaders/ShadowVolume.fs",
                                                      &m_hShadowVolumeShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_hShadowVolumeLightDirectionLoc = glGetUniformLocation( m_hShadowVolumeShader, "g_vLightDirection" );
        m_hShadowVolumeNormalLoc         = glGetUniformLocation( m_hShadowVolumeShader, "g_matNormal" );
        m_hShadowVolumeModelViewProjLoc  = glGetUniformLocation( m_hShadowVolumeShader, "g_matModelViewProj" );
        m_hShadowVolumeViewProjLoc       = glGetUniformLocation( m_hShadowVolumeShader, "g_matViewProj" );
    }

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
    m_nCurMeshIndex   = CUBE_MESH;
    m_vLightDirection = FRMVECTOR4( 0.0f, -1.0f, 0.0f, 0.0f );
    m_vLightPosition  = FRMVECTOR3( 0.0f, 8.0f, 0.01f );

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/ShadowVolume.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
		                        m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Model" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Next Floor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Visualize Shadow Volume" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_UP, "Rotate light CCW about x-axis" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_DOWN, "Rotate light CW about x-axis" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_LEFT, "Rotate light CCW about z-axis" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_RIGHT, "Rotate light CW about z-axis" );

     // Load the meshes
    if( FALSE == m_Floor[ PLANE_FLOOR ].Load( "Samples/Meshes/Plane.mesh" ) )
        return FALSE;
    if( FALSE == m_Floor[ PLANE_FLOOR ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_FloorEdges[ PLANE_FLOOR ].Load( "Samples/Meshes/PlaneEdges.mesh" ) )
        return FALSE;
    if( FALSE == m_FloorEdges[ PLANE_FLOOR ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Floor[ TERRAIN_FLOOR ].Load( "Samples/Meshes/Terrain.mesh" ) )
        return FALSE;
    if( FALSE == m_Floor[ TERRAIN_FLOOR ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_FloorEdges[ TERRAIN_FLOOR ].Load( "Samples/Meshes/TerrainEdges.mesh" ) )
        return FALSE;
    if( FALSE == m_FloorEdges[ TERRAIN_FLOOR ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ CUBE_MESH ].Load( "Samples/Meshes/Cube.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ CUBE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshEdges[ CUBE_MESH ].Load( "Samples/Meshes/CubeEdges.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshEdges[ CUBE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].Load( "Samples/Meshes/Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshEdges[ SPHERE_MESH ].Load( "Samples/Meshes/SphereEdges.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshEdges[ SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].Load( "Samples/Meshes/BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshEdges[ BUMPY_SPHERE_MESH ].Load( "Samples/Meshes/BumpySphereEdges.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshEdges[ BUMPY_SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].Load( "Samples/Meshes/Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshEdges[ TORUS_MESH ].Load( "Samples/Meshes/TorusEdges.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshEdges[ TORUS_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].Load( "Samples/Meshes/Rocket.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshEdges[ ROCKET_MESH ].Load( "Samples/Meshes/RocketEdges.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshEdges[ ROCKET_MESH ].MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );

    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Setup the floor's camera relative model view, model view projection, and normal matrices

    FRMMATRIX4X4 matFloorScale     = FrmMatrixScale( 5.0f, 5.0f, 5.0f );
    FRMMATRIX4X4 matFloorTranslate = FrmMatrixTranslate( 0.0f, -0.1f, 0.0f );

    m_vPosition        = FRMVECTOR3( 0.0f, 4.0f, 10.0f );
    FRMVECTOR3 vLookAt = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vUp     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matView          = FrmMatrixLookAtRH( m_vPosition, vLookAt, vUp );

    FRMMATRIX4X4 m_matFloorModel    = FrmMatrixMultiply( matFloorScale, matFloorTranslate );
    m_matFloorModelView             = FrmMatrixMultiply( m_matFloorModel,     m_matView );
    m_matFloorModelViewProj         = FrmMatrixMultiply( m_matFloorModelView, m_matPersp );
    m_matFloorWorldNormal           = FrmMatrixNormal( m_matFloorModel );
    m_matFloorEyeNormal             = FrmMatrixNormal( m_matFloorModelView );

    // Setup GL state
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glEnable( GL_CULL_FACE );

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
    if( m_hShadowVolumeShader )    glDeleteProgram( m_hShadowVolumeShader );
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
        m_vLightDirection = FrmVector4Transform( m_vLightDirection, matRotate );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow right
    if( nButtons & INPUT_DPAD_RIGHT )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        m_vLightDirection = FrmVector4Transform( m_vLightDirection, matRotate );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow down
    if( nButtons & INPUT_DPAD_DOWN )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        m_vLightDirection = FrmVector4Transform( m_vLightDirection, matRotate );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow up
    if( nButtons & INPUT_DPAD_UP )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        m_vLightDirection = FrmVector4Transform( m_vLightDirection, matRotate );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nCurMeshIndex = ( m_nCurMeshIndex + 1 ) % NUM_MESHES;
    }

    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_nCurFloorIndex = ( m_nCurFloorIndex + 1 ) % NUM_FLOORS;
    }

    // Toggle shadow volume visualization
    if( nPressedButtons & INPUT_KEY_3 )
    {
        m_bVisualizeShadowVolume = 1 - m_bVisualizeShadowVolume;
    }

    // Setup the mesh's camera relative model view, model view projection, and normal matrices
	
	FRMVECTOR3 vTmp = FRMVECTOR3(0.0f, 1.0f, 0.0f);
    FRMMATRIX4X4 matMeshRotate  = FrmMatrixRotate( fTime, vTmp );
	vTmp = FRMVECTOR3(1.0f, 0.0f, 0.0f);
	FRMMATRIX4X4 matMeshRotate2 = FrmMatrixRotate( fTime, vTmp );
    FRMMATRIX4X4 matMeshModel   = FrmMatrixTranslate( 0.0f, 2.0f, 0.0f );
    matMeshModel                = FrmMatrixMultiply( matMeshRotate,      matMeshModel );
    m_matMeshModel              = FrmMatrixMultiply( matMeshRotate2,     matMeshModel );
    m_matMeshModelView          = FrmMatrixMultiply( m_matMeshModel,     m_matView );
    m_matMeshModelViewProj      = FrmMatrixMultiply( m_matMeshModelView, m_matPersp );
    m_matMeshWorldNormal        = FrmMatrixNormal( m_matMeshModel );
    m_matMeshEyeNormal          = FrmMatrixNormal( m_matMeshModelView );
    m_matViewProj               = FrmMatrixMultiply( m_matView, m_matPersp );
}


VOID CSample::RenderScreenAlignedQuad()
{
    const FLOAT32 Quad[] =
    {
        -1.0, -1.0f, 0.0f, 1.0f,
        -1.0, +1.0f, 0.0f, 1.0f,
        +1.0, +1.0f, 0.0f, 1.0f,
        +1.0, -1.0f, 0.0f, 1.0f,
    };

    glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, Quad );
    glEnableVertexAttribArray( 0 );

    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );

    glDisableVertexAttribArray( 0 );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glStencilMask( 0xFFFFFFFF );
    glEnable( GL_DEPTH_TEST );

    // Shared lighting parameters
    glUseProgram( m_hPerPixelLightingShader );
    glUniform4f( m_hMaterialAmbientLoc, 0.2f, 0.2f, 0.2f, 1.0f );
    glUniform3f( m_hLightPositionLoc, m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z );

    // Render the floor along with its depth
    glUniform4f( m_hMaterialDiffuseLoc, 0.5f, 0.5f, 0.8f, 1.0f );
    glUniform4f( m_hMaterialSpecularLoc, 0.0f, 0.0f, 0.0f, 0.0f );
    glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matFloorModelView );
    glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matFloorModelViewProj );
    glUniformMatrix3fv( m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matFloorEyeNormal );
    m_Floor[ m_nCurFloorIndex ].Render();

    // Render the mesh along with its depth
    glUniform4f( m_hMaterialDiffuseLoc, 0.8f, 0.3f, 0.3f, 1.0f );
    glUniform4f( m_hMaterialSpecularLoc, 1.0f, 0.6f, 0.65f, 10.0f );
    glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matMeshModelView );
    glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matMeshModelViewProj );
    glUniformMatrix3fv( m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matMeshEyeNormal );
    m_Meshes[ m_nCurMeshIndex ].Render();

    // Draw shadow volume using Carmack's reverse

    glUseProgram( m_hShadowVolumeShader );

    glDisable( GL_CULL_FACE );

    if( !m_bVisualizeShadowVolume )
    {
        glEnable( GL_STENCIL_TEST );

        glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
        glDepthMask( GL_FALSE );

        glStencilFunc( GL_ALWAYS, 0, 0xFFFFFFFF );
        glStencilOpSeparate( GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP );
        glStencilOpSeparate( GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP );

        glDepthFunc( GL_GREATER );
    }

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.0f, 100.0f);

    // Setup shared shadow volume parameters
    glUniform4fv( m_hShadowVolumeLightDirectionLoc, 1, (FLOAT32*) &m_vLightDirection );
    glUniformMatrix4fv( m_hShadowVolumeViewProjLoc, 1, FALSE, 
                       (FLOAT32*) &m_matViewProj );

    // Render mesh shadow volume
    glUniformMatrix3fv( m_hShadowVolumeNormalLoc, 1, FALSE, 
                       (FLOAT32*) &m_matMeshWorldNormal );    
    glUniformMatrix4fv( m_hShadowVolumeModelViewProjLoc, 1, FALSE, 
                       (FLOAT32*) &m_matMeshModelViewProj );
    m_MeshEdges[ m_nCurMeshIndex ].Render();

    // Render floor shadow volume
    glUniformMatrix3fv( m_hShadowVolumeNormalLoc, 1, FALSE, 
                       (FLOAT32*) &m_matFloorWorldNormal );    
    glUniformMatrix4fv( m_hShadowVolumeModelViewProjLoc, 1, FALSE, 
                       (FLOAT32*) &m_matFloorModelViewProj );
    m_FloorEdges[ m_nCurFloorIndex ].Render();

    glDisable(GL_POLYGON_OFFSET_FILL);

    if( !m_bVisualizeShadowVolume )
    {
        glDepthFunc( GL_LEQUAL );
        glStencilFunc( GL_NOTEQUAL, 0, 0xFFFFFFFF );
        glStencilOp( GL_ZERO, GL_KEEP, GL_KEEP );

        glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
        glDepthMask( GL_TRUE );

        glDisable( GL_DEPTH_TEST );
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

        // Render over the screen space covered by the geometry in the shadow color
        glUseProgram( m_hConstantShader );
        glUniformMatrix4fv( m_hConstantMVPLoc, 1, FALSE,
                           (FLOAT32*) (FLOAT32*)&m_matFloorModelViewProj );
        glUniform4f( m_hConstantColorLoc, 0.2f, 0.2f, 0.2f, 1.0f );
        m_Floor[ m_nCurFloorIndex ].Render();

        glUniformMatrix4fv( m_hConstantMVPLoc, 1, FALSE,
                           (FLOAT32*) &m_matMeshModelViewProj );
        m_Meshes[ m_nCurMeshIndex ].Render();

        glDisable( GL_BLEND );
        glEnable( GL_DEPTH_TEST );
        glDisable( GL_STENCIL_TEST );
    }

    glEnable( GL_CULL_FACE );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

