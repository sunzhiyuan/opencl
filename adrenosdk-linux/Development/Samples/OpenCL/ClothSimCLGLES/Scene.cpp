//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <CL/cl_gl.h>
#include <FrmComputeGraphicsApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <OpenGLES/FrmUtilsGLES.h>

#include <OpenCL/FrmKernel.h>
#include <FrmUtils.h>
#include "Scene.h"



//--------------------------------------------------------------------------------------
// Name: FrmCreateComputeGraphicsApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeGraphicsApplication* FrmCreateComputeGraphicsApplicationInstance()
{
    return new CSample( "ClothSimCLGLES" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeGraphicsApplication( strName )
{
    m_hTextureShader           = 0;
    m_hClothShader  = 0;
    m_qMeshRotation     = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
    m_qRotationDelta    = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
    m_vLightPosition.x  = -5.0f;
    m_vLightPosition.y  = 1.25f;
    m_vLightPosition.z  = 1.0f;

    m_pLogoTexture = NULL;
    m_pNormalTexture = NULL;
    m_pDiffuseTexture = NULL;

    m_bUseOpenCL = TRUE;
    m_ClothSim.SetUseOpenCL(m_bUseOpenCL ? true : false);
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
            { "g_vNormalOS",   FRM_VERTEX_NORMAL },
            { "g_vTexCoord",   FRM_VERTEX_TEXCOORD0 },
            { "g_vTangent",    FRM_VERTEX_TANGENT },
            { "g_vBitangent",  FRM_VERTEX_BINORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Cloth.vs",
                                                      "Samples/Shaders/Cloth.fs",
                                                      &m_hClothShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hDiffuseMapLoc          = glGetUniformLocation( m_hClothShader, "g_DiffuseMap" );
        m_hNormalMapLoc           = glGetUniformLocation( m_hClothShader, "g_NormalMap" );
        m_hModelMatrixLoc         = glGetUniformLocation( m_hClothShader, "g_matModel" );
        m_hModelViewMatrixLoc     = glGetUniformLocation( m_hClothShader, "g_matModelView" );
        m_hModelViewProjMatrixLoc = glGetUniformLocation( m_hClothShader, "g_matModelViewProj" );
        m_hNormalMatrixLoc        = glGetUniformLocation( m_hClothShader, "g_matNormal" );
        m_hLightPositionLoc       = glGetUniformLocation( m_hClothShader, "g_vLightPos" );
        m_hMaterialAmbientLoc     = glGetUniformLocation( m_hClothShader, "g_Material.vAmbient" );
        m_hMaterialDiffuseLoc     = glGetUniformLocation( m_hClothShader, "g_Material.vDiffuse" );
        m_hMaterialSpecularLoc    = glGetUniformLocation( m_hClothShader, "g_Material.vSpecular" );
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    if( FALSE == m_ClothSim.Initialize( m_context, m_devices[0] ) )
        return FALSE;

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/ClothSimCLGLES.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );
    m_pDiffuseTexture = resource.GetTexture( "Banner_D" );
    m_pNormalTexture = resource.GetTexture( "Banner_N" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_bUseOpenCL, "Use OpenCL" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Toggle Use OpenCL Simulation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Add Wind" );

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.1f, 5.0f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );

    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 0.1f, 10.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Resize the arc ball control
    m_ArcBall.Resize( m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_hClothShader )  glDeleteProgram( m_hClothShader );
    if( m_hTextureShader )           glDeleteProgram( m_hTextureShader );

    if( m_pLogoTexture )    m_pLogoTexture->Release();
    if( m_pDiffuseTexture ) m_pDiffuseTexture->Release();
    if( m_pNormalTexture )  m_pNormalTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Update the Cloth simulation
    FLOAT32 fClothElapsedTime = fElapsedTime;

    // Clamp the tick of the cloth simulation to 1/10th of a second so
    // the simulation does not pull the cloth to the point it becomes
    // unstable
    if( fClothElapsedTime > 0.1f )
        fClothElapsedTime = 0.1f;
    m_ClothSim.Tick( fClothElapsedTime );
    m_ClothSim.UpdateVBOs();

    // Process input
    {
        // Get button input since the last frame
        UINT32 nButtons;
        UINT32 nPressedButtons;
        m_Input.GetButtonState( &nButtons, &nPressedButtons );

        // Get pointer input since the last frame
        UINT32     nPointerState;
        FRMVECTOR2 vPointerPosition;
        m_Input.GetPointerState( &nPointerState, &vPointerPosition );

        // Conditionally advance the UI state
        m_UserInterface.HandleInput( m_nWidth, m_nHeight, nPressedButtons,
                                     nPointerState, vPointerPosition );

        // Use the pointer input to rotate the object
        FRMVECTOR4 qArcBallDelta = m_ArcBall.HandleInput( vPointerPosition, nPointerState & FRM_INPUT::POINTER_DOWN );
        if( m_ArcBall.IsDragging() )
            m_qRotationDelta = qArcBallDelta;
        FRMVECTOR4 qDelta = nPointerState ? qArcBallDelta : m_qRotationDelta;

        qDelta = FrmVector4Normalize( qDelta );
        m_qMeshRotation = FrmQuaternionMultiply( m_qMeshRotation, qDelta );

        // Move the light up/down/left/right
        if( nButtons & FRM_INPUT::DPAD_UP )
            m_vLightPosition.y = FrmMin( +5.0f, m_vLightPosition.y + 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_DOWN )
            m_vLightPosition.y = FrmMax( -5.0f, m_vLightPosition.y - 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_LEFT )
            m_vLightPosition.x = FrmMax( -5.0f, m_vLightPosition.x - 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_RIGHT )
            m_vLightPosition.x = FrmMin( +5.0f, m_vLightPosition.x + 5.0f * fElapsedTime );

        if( nPressedButtons & FRM_INPUT::KEY_1 )
        {
            m_bUseOpenCL = !m_bUseOpenCL;
            m_ClothSim.SetUseOpenCL( m_bUseOpenCL ? true : false );
        }

        if( nButtons & FRM_INPUT::KEY_2 )
        {
            FRMVECTOR3 cameraPos = FRMVECTOR3( 0.0f, 0.1f, 5.0f );
            m_ClothSim.LaunchSphere(0.5f, 0.5f, m_matModelViewProj, cameraPos );
        }
    }

    // Orientate the object
    FRMMATRIX4X4 matRotate    = FrmMatrixRotate( m_qMeshRotation );
    FRMMATRIX4X4 matTranslate = FrmMatrixTranslate( 0.0f, -0.07f, 0.0f );

    // Build the matrices
    m_matModel            = FrmMatrixMultiply( matTranslate,   matRotate );
    m_matModelView        = FrmMatrixMultiply( m_matModel,       m_matView );
    m_matModelViewProj    = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_matNormal           = FrmMatrixNormal( m_matModelView );

}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    // Clear the color and depth-buffer
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( m_hClothShader );

    FRMVECTOR4 vAmbient = FRMVECTOR4( 0.0f, 0.1f, 0.25f, 1.0f );
    FRMVECTOR4 vDiffuse = FRMVECTOR4( 0.0f, 0.4f, 1.0f, 1.0f );
    FRMVECTOR4 vSpecular = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 20.0f );

    glUniform4fv( m_hMaterialAmbientLoc,  1, (FLOAT32*)&vAmbient );
    glUniform4fv( m_hMaterialDiffuseLoc,  1, (FLOAT32*)&vDiffuse );
    glUniform4fv( m_hMaterialSpecularLoc, 1, (FLOAT32*)&vSpecular );
    glUniformMatrix4fv( m_hModelMatrixLoc,         1, FALSE, (FLOAT32*)&m_matModel );
    glUniformMatrix4fv( m_hModelViewMatrixLoc,     1, FALSE, (FLOAT32*)&m_matModelView );
    glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniformMatrix3fv( m_hNormalMatrixLoc,        1, FALSE, (FLOAT32*)&m_matNormal );
    glUniform3fv( m_hLightPositionLoc, 1, (FLOAT32*)&m_vLightPosition );

    glUniform1i( m_hDiffuseMapLoc, 0 );
    glUniform1i( m_hNormalMapLoc, 1 );
    FrmSetTexture( 0, m_pDiffuseTexture->m_hTextureHandle );
    FrmSetTexture( 1, m_pNormalTexture->m_hTextureHandle );

    m_ClothSim.SubmitDraw();

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, 0 );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, 0 );

    //m_Mesh.Render();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

