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
#include "Scene14.h"



//--------------------------------------------------------------------------------------
// Name: Clear()
// Desc: Clears render target data
//--------------------------------------------------------------------------------------
VOID SOffscreenRT::Clear()
{
    BufferHandle = NULL;
    TextureHandle = NULL;
    DepthHandle = NULL;
    Width = 0;
    Height = 0;
}


//--------------------------------------------------------------------------------------
// Name: CSample14()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample14::CSample14( const CHAR* strName ) : CScene( strName )
{
    m_ToonShader.ShaderId = 0;
    m_NormalShader.ShaderId = 0;
    m_fRotateTime = 0.0f;
    m_OutlineThickness = 0.0004f;
    m_SlopeBias = 3.1f;

    m_nWidth = 400;
    m_nHeight = 240;

    ClearOffscreenRTs();
}


//--------------------------------------------------------------------------------------
// Name: ClearOffscreenRTs()
// Desc: Clears the variables used for off-screen render targets
//--------------------------------------------------------------------------------------
VOID CSample14::ClearOffscreenRTs()
{
    for( int i = 0; i < MAX_RTS; i++ )
    {
        m_OffscreenRT[i].Clear();
    }
}


//--------------------------------------------------------------------------------------
// Name: FreeOffscreenRTs()
// Desc: Destroys all off-screen render tarets
//--------------------------------------------------------------------------------------
VOID CSample14::FreeOffscreenRTs()
{
    for( int i = 0; i < MAX_RTS; i++ )
    {
        FreeOffscreenRT( i );
    }
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample14::InitShaders()
{
    // toon shader
	{
		FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
		{ 
			{ "g_vPositionOS", FRM_VERTEX_POSITION },
			{ "g_vNormalOS",   FRM_VERTEX_NORMAL }
		};
		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

		if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/ToonShader14.vs", "Demos/AdrenoShaders/Shaders/ToonShader14.fs",
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
	}

    // normal shader
	{
		FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
		{ 
			{ "g_vPositionOS", FRM_VERTEX_POSITION },
			{ "g_vNormalOS",   FRM_VERTEX_NORMAL }
		};
		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

		if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/NormalShader14.vs", "Demos/AdrenoShaders/Shaders/NormalShader14.fs",
			&m_NormalShader.ShaderId,
			pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
		m_NormalShader.ModelViewMatrixLoc     = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matModelView" );
		m_NormalShader.ModelViewProjMatrixLoc = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matModelViewProj" );
		m_NormalShader.NormalMatrixLoc        = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matNormal" );
		m_NormalShader.LightPositionLoc       = glGetUniformLocation( m_NormalShader.ShaderId,  "g_vLightPos" );
		m_NormalShader.MaterialAmbientLoc     = glGetUniformLocation( m_NormalShader.ShaderId,  "g_Material.vAmbient" );
		m_NormalShader.MaterialDiffuseLoc     = glGetUniformLocation( m_NormalShader.ShaderId,  "g_Material.vDiffuse" );
		m_NormalShader.MaterialSpecularLoc    = glGetUniformLocation( m_NormalShader.ShaderId,  "g_Material.vSpecular" );
	}

    // combine shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CombineShader14.vs", "Demos/AdrenoShaders/Shaders/CombineShader14.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.NormalTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_NormalTexture" );
        m_CombineShader.ToonTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_ToonTexture" );
        m_CombineShader.SlopeBias = glGetUniformLocation( m_CombineShader.ShaderId, "g_SlopeBias" );
        m_CombineShader.StepSizeId = glGetUniformLocation( m_CombineShader.ShaderId, "g_StepSize" );
    }

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: SetFramebuffer
// Desc: Sets a custom RT as the main framebuffer for drawing
//--------------------------------------------------------------------------------------
VOID CSample14::SetFramebuffer( UINT32 RenderTargetId )
{
    if (RenderTargetId < MAX_RTS)
    {
        SOffscreenRT* rt = &m_OffscreenRT[RenderTargetId];

        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, rt->TextureHandle );
        glBindFramebuffer( GL_FRAMEBUFFER, rt->BufferHandle );

        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->TextureHandle, 0 );
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rt->DepthHandle );

        glViewport( 0, 0, rt->Width, rt->Height );
    }
}


//--------------------------------------------------------------------------------------
// Name: CreateOffscreenRT
// Desc: Creates a render target for drawing to
//--------------------------------------------------------------------------------------
BOOL CSample14::CreateOffscreenRT( UINT32 RenderTargetId, UINT32 Width, UINT32 Height )
{
    if (RenderTargetId < MAX_RTS)
    {
        SOffscreenRT* rt = &m_OffscreenRT[RenderTargetId];

        // Free the RT, in case it was previously created
        FreeOffscreenRT( RenderTargetId );

        glGenRenderbuffers( 1, &rt->DepthHandle );
        glBindRenderbuffer( GL_RENDERBUFFER, rt->DepthHandle );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, Width, Height );

        glGenFramebuffers( 1, &rt->BufferHandle );
        glBindFramebuffer( GL_FRAMEBUFFER, rt->BufferHandle );

        glGenTextures( 1, &rt->TextureHandle );
        glBindTexture( GL_TEXTURE_2D, rt->TextureHandle );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, rt->TextureHandle );
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->TextureHandle, 0 );
        
        if( FALSE == CheckFrameBufferStatus() )
            return FALSE;

        glBindFramebuffer( GL_FRAMEBUFFER, 0 );

        rt->Width = Width;
        rt->Height = Height;

        return TRUE;
    }

    return FALSE;
}


//--------------------------------------------------------------------------------------
// Name: FreeOffscreenRT
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample14::FreeOffscreenRT( UINT32 RenderTargetId )
{
    if( RenderTargetId < MAX_RTS )
    {
        SOffscreenRT* rt = &m_OffscreenRT[RenderTargetId];

        if( rt->BufferHandle )
        {
            glBindFramebuffer( GL_FRAMEBUFFER, rt->BufferHandle );
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
            glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        }
        
        if( rt->BufferHandle )
            glDeleteFramebuffers( 1, &rt->BufferHandle );
        if( rt->TextureHandle )
            glDeleteTextures( 1, &rt->TextureHandle );
        if( rt->DepthHandle )
            glDeleteRenderbuffers( 1, &rt->DepthHandle );
    }
}



//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample14::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_bShouldRotate = TRUE;

    // load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/14_Textures.pak" ) )
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
    m_UserInterface.AddFloatVariable( &m_OutlineThickness , (char *)"Line thickness", (char *)"%3.4f" );
    m_UserInterface.AddFloatVariable( &m_SlopeBias , (char *)"Edge detection", (char *)"%3.4f" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Decrease line thickness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Increase line thickness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Decrease edge detection" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Increase edge detection" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Toggle Rotation" );

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
BOOL CSample14::Resize()
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

    CreateOffscreenRT( RT_NORMALS, m_nWidth / 2, m_nHeight / 2);
    CreateOffscreenRT( RT_TOON, m_nWidth / 2, m_nHeight / 2);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample14::Destroy()
{
    // Free shader memory
    if( m_ToonShader.ShaderId ) glDeleteProgram( m_ToonShader.ShaderId );
    if( m_NormalShader.ShaderId ) glDeleteProgram( m_NormalShader.ShaderId );

    // Free texture memory
    if( m_ToonShader.ColorRampTexture ) m_ToonShader.ColorRampTexture->Release();

    // Free mesh memory
    m_Mesh.Destroy();

    FreeOffscreenRTs();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample14::Update()
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
        if( m_OutlineThickness >= 0.0f )
        {
            m_OutlineThickness -= 0.005f * fElapsedTime;
            m_OutlineThickness = max( m_OutlineThickness, 0.0f );
        }
    }

    // Increase outline thickness
    if( nButtons & INPUT_KEY_2 )
    {
        float MAX_OUTLINE_THICKNESS = 0.003f;
        if( m_OutlineThickness <= MAX_OUTLINE_THICKNESS )
        {
            m_OutlineThickness += 0.005f * fElapsedTime;
            m_OutlineThickness = min( m_OutlineThickness, MAX_OUTLINE_THICKNESS );
        }
    }

    // Decrease edge detection bias
    if( nButtons & INPUT_KEY_3 )
    {
        if( m_SlopeBias >= 0.0f )
        {
            m_SlopeBias -= 1.0f * fElapsedTime;
            m_SlopeBias = max( m_SlopeBias, 0.0f );
        }
    }

    // Increase edge detection bias
    if( nButtons & INPUT_KEY_4 )
    {
        float MAX_SLOPE_BIAS = 5.0f;
        if( m_SlopeBias <= MAX_SLOPE_BIAS )
        {
            m_SlopeBias += 1.0f * fElapsedTime;
            m_SlopeBias = min( m_SlopeBias, MAX_SLOPE_BIAS );
        }
    }

    // Toggle rotation
    if( nPressedButtons & INPUT_KEY_5 )
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
// Name: RenderSceneNormals()
// Desc: Draws the scene normals to an off-screen render target
//--------------------------------------------------------------------------------------
VOID CSample14::RenderSceneNormals()
{
    // Clear the frame
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set shader variables
    glUseProgram( m_NormalShader.ShaderId );
    glUniform4f( m_NormalShader.MaterialAmbientLoc, 0.8f, 0.2f, 0.2f, 1.0f );
    glUniform4f( m_NormalShader.MaterialDiffuseLoc, 0.3f, 0.3f, 0.8f, 1.0f );
    glUniform4f( m_NormalShader.MaterialSpecularLoc, 0.65f, 0.6f, 1.0f, 10.0f );
    glUniformMatrix4fv( m_NormalShader.ModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelView );
    glUniformMatrix4fv( m_NormalShader.ModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniformMatrix3fv( m_NormalShader.NormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matNormal );
    glUniform3f( m_NormalShader.LightPositionLoc, 0.0f, 1.0f, 1.0f );

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

        glUniformMatrix4fv( m_NormalShader.ModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&matModelView );
        glUniformMatrix4fv( m_NormalShader.ModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&matModelViewProj );
        glUniformMatrix3fv( m_NormalShader.NormalMatrixLoc, 1, FALSE, (FLOAT32*)&matNormal );

        // Draw
        m_Mesh.Render();
    }
}


//--------------------------------------------------------------------------------------
// Name: RenderSceneToon()
// Desc: Draws the scene normals to an off-screen render target
//--------------------------------------------------------------------------------------
VOID CSample14::RenderSceneToon()
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
// Name: RenderCombinedScene()
// Desc: The final draw pass
//--------------------------------------------------------------------------------------
VOID CSample14::RenderCombinedScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glUseProgram( m_CombineShader.ShaderId );
    SetTexture( m_CombineShader.NormalTextureId, m_OffscreenRT[RT_NORMALS].TextureHandle, 0 );
    SetTexture( m_CombineShader.ToonTextureId, m_OffscreenRT[RT_TOON].TextureHandle, 1 );
    glUniform1f( m_CombineShader.StepSizeId, m_OutlineThickness );
    glUniform1f( m_CombineShader.SlopeBias, m_SlopeBias );

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the scene
//--------------------------------------------------------------------------------------
VOID CSample14::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    // Draw scene normals
    SetFramebuffer( RT_NORMALS );
    RenderSceneNormals();

    // Draw the scene with toon shading
    SetFramebuffer( RT_TOON );
    RenderSceneToon();

    // Finally, combine the two
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );
    RenderCombinedScene();


    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
