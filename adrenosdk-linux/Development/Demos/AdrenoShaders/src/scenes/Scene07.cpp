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
#include "Scene07.h"


//--------------------------------------------------------------------------------------
// Name: CSample07()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample07::CSample07( const CHAR* strName ) : CScene( strName )
{
    m_hCubeMapLightingShader =	NULL;
	m_pLightingCubeMap	  =	NULL;

    m_fRotateTime = 0.0f;

    m_nWidth          = 600;
    m_nHeight         = 800;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample07::InitShaders()
{
    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CubeMapLighting.vs",
                                                      "Demos/AdrenoShaders/Shaders/CubeMapLighting.fs",
                                                      &m_hCubeMapLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hModelViewMatrixLoc     = glGetUniformLocation( m_hCubeMapLightingShader,  "g_matModelView" );
        m_hModelViewProjMatrixLoc = glGetUniformLocation( m_hCubeMapLightingShader,  "g_matModelViewProj" );
        m_hNormalMatrixLoc        = glGetUniformLocation( m_hCubeMapLightingShader,  "g_matNormal" );
        m_hLightPositionLoc       = glGetUniformLocation( m_hCubeMapLightingShader,  "g_vLightPos" );
        m_hMaterialAmbientLoc     = glGetUniformLocation( m_hCubeMapLightingShader,  "g_Material.vAmbient" );
        m_hMaterialDiffuseLoc     = glGetUniformLocation( m_hCubeMapLightingShader,  "g_Material.vDiffuse" );
        m_hMaterialSpecularLoc    = glGetUniformLocation( m_hCubeMapLightingShader,  "g_Material.vSpecular" );
        m_hCubeMapLighting        = glGetUniformLocation( m_hCubeMapLightingShader,  "g_CubeMapLighting" );
    }

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample07::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_bShouldRotate = TRUE;
    m_fDeformSize   = 0.0030f;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/07_Textures.pak" ) )
        return FALSE;

	// fetch cube map
    m_pLightingCubeMap = resource.GetCubeMap( "BedroomCubeMap" );
    if(m_pLightingCubeMap == NULL)
    {
        FrmLogMessage("Unable to create cube map texture");
        return FALSE;
    }

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;

    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_fDeformSize , (char *)"Normal Deform Size", (char *)"%3.4f" );
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
BOOL CSample07::Resize()
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
VOID CSample07::Destroy()
{
    // Free shader memory
    if( m_hCubeMapLightingShader ) glDeleteProgram( m_hCubeMapLightingShader );
    
	// Free texture memory
    if( m_pLightingCubeMap ) m_pLightingCubeMap->Release();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample07::Update()
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
    m_matNormal        = FrmMatrixNormal( m_matModelView );
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample07::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUseProgram( m_hCubeMapLightingShader );
	glUniform4f( m_hMaterialAmbientLoc, 0.8f, 0.2f, 0.2f, 1.0f );
	glUniform4f( m_hMaterialDiffuseLoc, 0.3f, 0.3f, 0.8f, 1.0f );
	glUniform4f( m_hMaterialSpecularLoc, 0.65f, 0.6f, 1.0f, 10.0f );
	glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelView );
	glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
	glUniformMatrix3fv( m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matNormal );
	glUniform3f( m_hLightPositionLoc, 2.0f, 1.0f, 1.0f );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_pLightingCubeMap->m_hTextureHandle );
    glUniform1i( m_hCubeMapLighting, 0 );

	m_Mesh.Render();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

