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

#include "Scene01.h"


//--------------------------------------------------------------------------------------
// Name: CSample01()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample01::CSample01( const CHAR* strName ) : CScene( strName )
{
    m_hPerPixelLightingShader =	NULL;

    m_fRotateTime = 0.0f;

    m_nWidth          = 600;
    m_nHeight         = 800;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample01::InitShaders()
{
    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/Ambient.vs",
                                                      "Demos/AdrenoShaders/Shaders/Ambient.fs",
                                                      &m_hPerPixelLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hModelViewMatrixLoc     = glGetUniformLocation( m_hPerPixelLightingShader,  "g_matModelView" );
        m_hModelViewProjMatrixLoc = glGetUniformLocation( m_hPerPixelLightingShader,  "g_matModelViewProj" );
        m_hMaterialAmbientLoc     = glGetUniformLocation( m_hPerPixelLightingShader,  "g_Material.vAmbient" );
    }

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample01::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_bShouldRotate = TRUE;
    m_fDeformSize   = 0.0030f;

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_bShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decr Normal Deformation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Incr Normal Deformation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Teapot.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
    glEnable( GL_DEPTH_TEST );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample01::Resize()
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
VOID CSample01::Destroy()
{
    // Free shader memory
    if( m_hPerPixelLightingShader ) 
        glDeleteProgram( m_hPerPixelLightingShader );

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample01::Update()
{
    // Get the current time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();
    if( m_bShouldRotate )
    {
        m_fRotateTime += fElapsedTime;
    }


    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    // Decrease normal deformation
    if( nButtons & INPUT_KEY_4 )
    {
        if( m_fDeformSize >= 0.0f )
        {
            m_fDeformSize -= 0.004f * fElapsedTime;
        }
    }

    // Increase normal deformation
    if( nButtons & INPUT_KEY_5 )
    {
        if( m_fDeformSize <= 0.0040f )
        {
            m_fDeformSize += 0.004f * fElapsedTime;
        }
    }

    // Toggle rotation
    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_bShouldRotate = 1 - m_bShouldRotate;
    }

    // Rotate the object
    FRMMATRIX4X4 matRotate1 = FrmMatrixRotate( m_fRotateTime, 0.0f, 1.0f, 0.0f );
    FRMMATRIX4X4 matRotate2 = FrmMatrixRotate( 0.3f,  1.0f, 0.0f, 0.0f );

    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.20f, 0.5f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.07f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // Build the matrices
    FRMMATRIX4X4 matModel;
    matModel           = FrmMatrixMultiply( matRotate2,     matRotate1 );
    m_matModelView     = FrmMatrixMultiply( matModel,       m_matView );
    m_matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample01::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUseProgram( m_hPerPixelLightingShader );
	glUniform4f( m_hMaterialAmbientLoc, 1.0f, 1.0f, 1.0f, 1.0f );
	glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelView );
	glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );

	m_Mesh.Render();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

