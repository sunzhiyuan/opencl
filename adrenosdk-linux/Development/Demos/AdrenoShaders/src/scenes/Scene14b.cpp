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
#include "Scene14b.h"


//--------------------------------------------------------------------------------------
// Name: CSample14b()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample14b::CSample14b( const CHAR* strName ) : CScene( strName )
{
    m_ToonShader.ShaderId = NULL;

    m_fRotateTime = 0.0f;

    m_nWidth = 500;
    m_nHeight = 500;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample14b::InitShaders()
{
    // toon shader
	{
		FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
		{ 
			{ "g_vPositionOS", FRM_VERTEX_POSITION },
			{ "g_vNormalOS",   FRM_VERTEX_NORMAL }
		};
		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

		if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/ToonShader14b.vs", "Demos/AdrenoShaders/Shaders/ToonShader14b.fs",
			&m_ToonShader.ShaderId,
			pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
		m_ToonShader.ModelViewMatrixLoc     = glGetUniformLocation( m_ToonShader.ShaderId,  "g_matModelView" );
		m_ToonShader.ModelViewProjMatrixLoc = glGetUniformLocation( m_ToonShader.ShaderId,  "g_matModelViewProj" );
		m_ToonShader.NormalMatrixLoc        = glGetUniformLocation( m_ToonShader.ShaderId,  "g_matNormal" );
		m_ToonShader.LightPositionLoc       = glGetUniformLocation( m_ToonShader.ShaderId,  "g_vLightPos" );
		m_ToonShader.MaterialAmbientLoc     = glGetUniformLocation( m_ToonShader.ShaderId,  "g_Material.vAmbient" );
		m_ToonShader.MaterialDiffuseLoc     = glGetUniformLocation( m_ToonShader.ShaderId,  "g_Material.vDiffuse" );
		m_ToonShader.MaterialSpecularLoc    = glGetUniformLocation( m_ToonShader.ShaderId,  "g_Material.vSpecular" );
		m_ToonShader.ColorRampId            = glGetUniformLocation( m_ToonShader.ShaderId,  "g_ColorRamp" );
        m_ToonShader.EdgeBiasId             = glGetUniformLocation( m_ToonShader.ShaderId,  "g_EdgeBias" );
	}

    m_EdgeBias = 3.58f;

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample14b::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_bShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/14b_Textures.pak" ) )
        return FALSE;

    // load the color ramp for the toon shader
    m_ToonShader.ColorRampTexture = resource.GetTexture( "ToonColorRamp" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_bShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_EdgeBias , (char *)"Edge detection", (char *)"%3.4f" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Decrease edge detection" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Increase edge detection" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Teapot.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    glEnable( GL_DEPTH_TEST );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample14b::Resize()
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
VOID CSample14b::Destroy()
{
    // Free shader memory
    if( m_ToonShader.ShaderId ) glDeleteProgram( m_ToonShader.ShaderId );

    // Free texture memory
    if( m_ToonShader.ColorRampTexture ) m_ToonShader.ColorRampTexture->Release();


    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample14b::Update()
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

    // Decrease outline thickness
    if( nButtons & INPUT_KEY_1 )
    {
        if( m_EdgeBias >= 0.0f )
        {
            m_EdgeBias -= 5.0f * fElapsedTime;
            m_EdgeBias = max( m_EdgeBias, 0.0f );
        }
    }

    // Increase outline thickness
    if( nButtons & INPUT_KEY_2 )
    {
        float MAX_OUTLINE_THICKNESS = 10.0f;
        if( m_EdgeBias <= MAX_OUTLINE_THICKNESS )
        {
            m_EdgeBias += 5.0f * fElapsedTime;
            m_EdgeBias = min( m_EdgeBias, MAX_OUTLINE_THICKNESS );
        }
    }

    // Toggle rotation
    if( nPressedButtons & INPUT_KEY_3 )
    {
        m_bShouldRotate = 1 - m_bShouldRotate;
    }

    // Rotate the object
    FRMMATRIX4X4 matRotate1 = FrmMatrixRotate( m_fRotateTime, 0.0f, 1.0f, 0.0f );
    FRMMATRIX4X4 matRotate2 = FrmMatrixRotate( 0.3f,  1.0f, 0.0f, 0.0f );
    static FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.2f, 0.4f );
    static FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.1f, 0.0f );

    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // Build the matrices
    m_matModel         = FrmMatrixMultiply( matRotate2,     matRotate1 );
    m_matModelView     = FrmMatrixMultiply( m_matModel,     m_matView );
    m_matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_matNormal        = FrmMatrixNormal( m_matModelView );
}


//--------------------------------------------------------------------------------------
// Name: RenderSceneToon()
// Desc: Draws the full scene toon shaded with outlines
//--------------------------------------------------------------------------------------
VOID CSample14b::RenderSceneToonAndOutline()
{
    // Clear the frame
    glClearColor( 0.4f, 0.8f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set shader variables
    glUseProgram( m_ToonShader.ShaderId );
    glUniform4f( m_ToonShader.MaterialAmbientLoc, 0.8f, 0.2f, 0.2f, 1.0f );
    glUniform4f( m_ToonShader.MaterialDiffuseLoc, 0.3f, 0.3f, 0.8f, 1.0f );
    glUniform4f( m_ToonShader.MaterialSpecularLoc, 0.65f, 0.6f, 1.0f, 10.0f );
    glUniformMatrix4fv( m_ToonShader.ModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelView );
    glUniformMatrix4fv( m_ToonShader.ModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniformMatrix3fv( m_ToonShader.NormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matNormal );
    FRMVECTOR3 LightVector;
    LightVector.x = -3.0f;
    LightVector.y = 3.0f;
    LightVector.z = 1.0f;
	FRMVECTOR3 NormLight = FrmVector3Normalize(LightVector);
    glUniform3f( m_ToonShader.LightPositionLoc, NormLight.x, NormLight.y, NormLight.z);
    SetTexture( m_ToonShader.ColorRampId, m_ToonShader.ColorRampTexture->m_hTextureHandle, 0 );
    glUniform1f( m_ToonShader.EdgeBiasId, m_EdgeBias );

    // Draw
    m_Mesh.Render();


    // Draw some more teapots
    for( int i = 0; i < 10; i++ )
    {
        float t = (float)i * 0.1f * 3.141529f * 2.0f;
        FRMMATRIX4X4 matModel = FrmMatrixTranslate( cos(t) * 0.5f, 0.0f, sin(t) * 0.5f );

        matModel = FrmMatrixMultiply( matModel, m_matModel );

        FRMMATRIX4X4 matModelView = FrmMatrixMultiply( matModel, m_matView );
        FRMMATRIX4X4 matModelViewProj = FrmMatrixMultiply( matModelView, m_matProj );
        FRMMATRIX3X3 matNormal = FrmMatrixNormal( matModelView );

        glUniformMatrix4fv( m_ToonShader.ModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&matModelView );
        glUniformMatrix4fv( m_ToonShader.ModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&matModelViewProj );
        glUniformMatrix3fv( m_ToonShader.NormalMatrixLoc, 1, FALSE, (FLOAT32*)&matNormal );

        // Draw
        m_Mesh.Render();
    }
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the scene
//--------------------------------------------------------------------------------------
VOID CSample14b::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    // Draw the scene with toon shading and outlines in one pass
    RenderSceneToonAndOutline();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
