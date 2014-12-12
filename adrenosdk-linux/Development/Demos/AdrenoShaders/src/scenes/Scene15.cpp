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
#include "Scene15.h"


//--------------------------------------------------------------------------------------
// Name: SOffscreenRT15()
// Desc: Constructor
//--------------------------------------------------------------------------------------
SOffscreenRT15::SOffscreenRT15()
{
    Initialized = FALSE;
}

//--------------------------------------------------------------------------------------
// Name: Clear()
// Desc: Clears render target data
//--------------------------------------------------------------------------------------
VOID SOffscreenRT15::Clear()
{
    BufferHandle = NULL;
    TextureHandle = NULL;
    DepthHandle = NULL;
    Width = 0;
    Height = 0;
}


//--------------------------------------------------------------------------------------
// Name: SHatchShader15()
// Desc: Constructor
//--------------------------------------------------------------------------------------
SHatchShader15::SHatchShader15()
{
    ShaderId = NULL;
    HatchTexture[0] = NULL;
    HatchTexture[1] = NULL;
    DiffuseTexture = NULL;
    HatchTextureId[0] = 0;
    HatchTextureId[1] = 0;
    DiffuseTextureId = 0;
}


//--------------------------------------------------------------------------------------
// Name: CSample15()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample15::CSample15( const CHAR* strName ) : CScene( strName )
{
    m_AlternateStyle = false;
    m_fRotateTime = -10.0f;
    m_OutlineThickness = 0.0004f;
    m_SlopeBias = 1.5837f;
    m_AmbientLight = 0.0f;

    m_nWidth = 500;
    m_nHeight = 500;

    ClearOffscreenRTs();
}


//--------------------------------------------------------------------------------------
// Name: ClearOffscreenRTs()
// Desc: Clears the variables used for off-screen render targets
//--------------------------------------------------------------------------------------
VOID CSample15::ClearOffscreenRTs()
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
VOID CSample15::FreeOffscreenRTs()
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
BOOL CSample15::InitShaders()
{
    // hatch shader A
	{
		FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
		{ 
			{ "g_vPositionOS", FRM_VERTEX_POSITION },
			{ "g_vNormalOS",   FRM_VERTEX_NORMAL },
            { "g_vTexCoord",   FRM_VERTEX_TEXCOORD0 }
		};
		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

		if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/HatchShader.vs", "Demos/AdrenoShaders/Shaders/HatchShader.fs",
			&m_HatchShaderA.ShaderId,
			pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
		m_HatchShaderA.ModelViewMatrixLoc     = glGetUniformLocation( m_HatchShaderA.ShaderId,  "g_matModelView" );
		m_HatchShaderA.ModelViewProjMatrixLoc = glGetUniformLocation( m_HatchShaderA.ShaderId,  "g_matModelViewProj" );
		m_HatchShaderA.NormalMatrixLoc        = glGetUniformLocation( m_HatchShaderA.ShaderId,  "g_matNormal" );
		m_HatchShaderA.LightPositionId        = glGetUniformLocation( m_HatchShaderA.ShaderId,  "g_LightPos" );
        m_HatchShaderA.AmbientId              = glGetUniformLocation( m_HatchShaderA.ShaderId,  "g_Ambient" );

		m_HatchShaderA.HatchTextureId[0]      = glGetUniformLocation( m_HatchShaderA.ShaderId,  "g_HatchTexture1" );
        m_HatchShaderA.HatchTextureId[1]      = glGetUniformLocation( m_HatchShaderA.ShaderId,  "g_HatchTexture2" );
        m_HatchShaderA.DiffuseTextureId       = glGetUniformLocation( m_HatchShaderA.ShaderId,  "g_DiffuseTexture" );
	}

    // hatch shader B
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL },
            { "g_vTexCoord",   FRM_VERTEX_TEXCOORD0 }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/HatchShaderB.vs", "Demos/AdrenoShaders/Shaders/HatchShaderB.fs",
            &m_HatchShaderB.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_HatchShaderB.ModelViewMatrixLoc     = glGetUniformLocation( m_HatchShaderB.ShaderId,  "g_matModelView" );
        m_HatchShaderB.ModelViewProjMatrixLoc = glGetUniformLocation( m_HatchShaderB.ShaderId,  "g_matModelViewProj" );
        m_HatchShaderB.NormalMatrixLoc        = glGetUniformLocation( m_HatchShaderB.ShaderId,  "g_matNormal" );
        m_HatchShaderB.LightPositionId        = glGetUniformLocation( m_HatchShaderB.ShaderId,  "g_LightPos" );
        m_HatchShaderB.AmbientId              = glGetUniformLocation( m_HatchShaderB.ShaderId,  "g_Ambient" );

        m_HatchShaderB.HatchTextureId[0]      = glGetUniformLocation( m_HatchShaderB.ShaderId,  "g_HatchTexture1" );
        m_HatchShaderB.HatchTextureId[1]      = glGetUniformLocation( m_HatchShaderB.ShaderId,  "g_HatchTexture2" );
        m_HatchShaderB.DiffuseTextureId       = glGetUniformLocation( m_HatchShaderB.ShaderId,  "g_DiffuseTexture" );
    }

    // normal shader
	{
		FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
		{ 
			{ "g_vPositionOS", FRM_VERTEX_POSITION },
			{ "g_vNormalOS",   FRM_VERTEX_NORMAL }
		};
		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

		if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/NormalShader15.vs", "Demos/AdrenoShaders/Shaders/NormalShader15.fs",
			&m_NormalShader.ShaderId,
			pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
		m_NormalShader.ModelViewMatrixLoc     = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matModelView" );
		m_NormalShader.ModelViewProjMatrixLoc = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matModelViewProj" );
		m_NormalShader.NormalMatrixLoc        = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matNormal" );
		m_NormalShader.LightPositionId        = glGetUniformLocation( m_NormalShader.ShaderId,  "g_vLightPos" );
	}

    // combine shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CombineShader15.vs", "Demos/AdrenoShaders/Shaders/CombineShader15.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.NormalTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_NormalTexture" );
        m_CombineShader.HatchTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_HatchTexture" );
        m_CombineShader.SlopeBiasId = glGetUniformLocation( m_CombineShader.ShaderId, "g_SlopeBias" );
        m_CombineShader.StepSizeId = glGetUniformLocation( m_CombineShader.ShaderId, "g_StepSize" );
    }

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: SetFramebuffer
// Desc: Sets a custom RT as the main framebuffer for drawing
//--------------------------------------------------------------------------------------
VOID CSample15::SetFramebuffer( UINT32 RenderTargetId )
{
    if (RenderTargetId < MAX_RTS)
    {
        SOffscreenRT15* rt = &m_OffscreenRT[RenderTargetId];

        FrmAssert( rt->Initialized );

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
BOOL CSample15::CreateOffscreenRT( UINT32 RenderTargetId, UINT32 Width, UINT32 Height )
{
    if (RenderTargetId < MAX_RTS)
    {
        SOffscreenRT15* rt = &m_OffscreenRT[RenderTargetId];

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
        rt->Initialized = TRUE;

        return TRUE;
    }

    return FALSE;
}


//--------------------------------------------------------------------------------------
// Name: FreeOffscreenRT
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample15::FreeOffscreenRT( UINT32 RenderTargetId )
{
    if( RenderTargetId < MAX_RTS )
    {
        SOffscreenRT15* rt = &m_OffscreenRT[RenderTargetId];

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
BOOL CSample15::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_bShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/15_Textures.pak" ) )
        return FALSE;

    // Grab hatch patterns
    m_HatchShaderA.HatchTexture[0] = resource.GetTexture( "Hatch1" );
    m_HatchShaderA.HatchTexture[1] = resource.GetTexture( "Hatch2" );
    m_HatchShaderB.HatchTexture[0] = resource.GetTexture( "Hatch3" );
    m_HatchShaderB.HatchTexture[1] = resource.GetTexture( "Hatch4" );
    m_HatchShaderA.DiffuseTexture = m_HatchShaderB.DiffuseTexture = resource.GetTexture( "RayGunLuminance" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_bShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_OutlineThickness , (char *)"Line thickness", (char *)"%3.4f" );
    m_UserInterface.AddFloatVariable( &m_SlopeBias , (char *)"Edge detection", (char *)"%3.4f" );
    m_UserInterface.AddFloatVariable( &m_AmbientLight , (char *)"Ambient light", (char *)"%3.4f" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Decrease line thickness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Increase line thickness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Decrease edge detection" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Increase edge detection" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Decrease ambient light" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Increase ambient light" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Switch hatch style" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/RayGun.mesh" ) )
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
BOOL CSample15::Resize()
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

    CreateOffscreenRT( RT_HATCH, m_nWidth / 2, m_nHeight / 2);
    CreateOffscreenRT( RT_NORMALS, m_nWidth / 2, m_nHeight / 2);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample15::Destroy()
{
    // Free shader memory
    if( m_HatchShaderA.ShaderId ) glDeleteProgram( m_HatchShaderA.ShaderId );
    if( m_HatchShaderB.ShaderId ) glDeleteProgram( m_HatchShaderB.ShaderId );
    if( m_NormalShader.ShaderId ) glDeleteProgram( m_NormalShader.ShaderId );

    // Free texture memory
    if( m_HatchShaderA.HatchTexture[0] ) m_HatchShaderA.HatchTexture[0]->Release();
    if( m_HatchShaderA.HatchTexture[1] ) m_HatchShaderA.HatchTexture[1]->Release();
    if( m_HatchShaderA.DiffuseTexture )  m_HatchShaderA.DiffuseTexture->Release();
    if( m_HatchShaderB.HatchTexture[0] ) m_HatchShaderB.HatchTexture[0]->Release();
    if( m_HatchShaderB.HatchTexture[1] ) m_HatchShaderB.HatchTexture[1]->Release();
    // note: m_HatchShaderB.DiffuseTexture is shared with m_HatchShaderA, so don't destroy

    // Free mesh memory
    m_Mesh.Destroy();

    FreeOffscreenRTs();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample15::Update()
{
    // Get the current time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();
    if( m_bShouldRotate )
    {
        m_fRotateTime += ( fElapsedTime * 0.75f );
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
            m_SlopeBias -= 5.0f * fElapsedTime;
            m_SlopeBias = max( m_SlopeBias, 0.0f );
        }
    }

    // Increase edge detection bias
    if( nButtons & INPUT_KEY_4 )
    {
        float MAX_SLOPE_BIAS = 5.0f;
        if( m_SlopeBias <= MAX_SLOPE_BIAS )
        {
            m_SlopeBias += 5.0f * fElapsedTime;
            m_SlopeBias = min( m_SlopeBias, MAX_SLOPE_BIAS );
        }
    }

    // Decrease ambient light
    if( nButtons & INPUT_KEY_5 )
    {
        if( m_AmbientLight >= 0.0f )
        {
            m_AmbientLight -= 1.0f * fElapsedTime;
            m_AmbientLight = max( m_AmbientLight, 0.0f );
        }
    }

    // Increase ambient light
    if( nButtons & INPUT_KEY_6 )
    {
        float MAX_AMBIENT_LIGHT = 0.8f;
        if( m_AmbientLight <= MAX_AMBIENT_LIGHT )
        {
            m_AmbientLight += 1.0f * fElapsedTime;
            m_AmbientLight = min( m_AmbientLight, MAX_AMBIENT_LIGHT );
        }
    }

    // Toggle shader
    if( nPressedButtons & INPUT_KEY_7 )
    {
        m_AlternateStyle = 1 - m_AlternateStyle;

        // set presets
        if (m_AlternateStyle)
        {
            m_AmbientLight = 0.1f;
        }
        else
        {
            m_AmbientLight = 0.0f;
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
VOID CSample15::RenderSceneNormals()
{
    // Clear the frame
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set shader variables
    glUseProgram( m_NormalShader.ShaderId );
    glUniformMatrix4fv( m_NormalShader.ModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelView );
    glUniformMatrix4fv( m_NormalShader.ModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniformMatrix3fv( m_NormalShader.NormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matNormal );
    glUniform3f( m_NormalShader.LightPositionId, 0.0f, 1.0f, 1.0f );

    // Draw
    m_Mesh.Render();
}


//--------------------------------------------------------------------------------------
// Name: RenderSceneHatch()
// Desc: Draws the scene hatch shaded
//--------------------------------------------------------------------------------------
VOID CSample15::RenderSceneHatch()
{
    // Clear the frame
    FRMVECTOR4 WhiteColor = FRMVECTOR4( 0.95f, 0.95f, 0.95f, 1.0f );

    SHatchShader15* HatchShader = &m_HatchShaderB;
    if (m_AlternateStyle)
    {
        HatchShader = &m_HatchShaderA;
    }
    else
    {
        WhiteColor = FRMVECTOR4( 0.7216f, 0.7098f, 0.6902f, 1.0f );
        WhiteColor *= ( 0.5f * 0.6f + 0.35f );
        WhiteColor.w = 1.0f;
    }

    glClearColor( WhiteColor.x, WhiteColor.y, WhiteColor.z, WhiteColor.w );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    // Set shader variables
    glUseProgram( HatchShader->ShaderId );
    glUniformMatrix4fv( HatchShader->ModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelView );
    glUniformMatrix4fv( HatchShader->ModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniformMatrix3fv( HatchShader->NormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matNormal );
    glUniform1f( HatchShader->AmbientId, m_AmbientLight );
    FRMVECTOR3 LightVector;
    LightVector.x = -1.0f;
    LightVector.y = 1.0f;
    LightVector.z = 1.0f;
	FRMVECTOR3 LightPosition = FrmVector3Normalize(LightVector);
    glUniform3f( HatchShader->LightPositionId, LightPosition.x, LightPosition.y, LightPosition.z );

    SetTexture( HatchShader->DiffuseTextureId, HatchShader->DiffuseTexture->m_hTextureHandle, 0 );
    SetTexture( HatchShader->HatchTextureId[0], HatchShader->HatchTexture[0]->m_hTextureHandle, 1 );
    SetTexture( HatchShader->HatchTextureId[1], HatchShader->HatchTexture[1]->m_hTextureHandle, 2 );

    // Draw
    m_Mesh.Render();
}


//--------------------------------------------------------------------------------------
// Name: RenderCombinedScene()
// Desc: The final draw pass
//--------------------------------------------------------------------------------------
VOID CSample15::RenderCombinedScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glUseProgram( m_CombineShader.ShaderId );
    SetTexture( m_CombineShader.NormalTextureId, m_OffscreenRT[RT_NORMALS].TextureHandle, 0 );
    SetTexture( m_CombineShader.HatchTextureId, m_OffscreenRT[RT_HATCH].TextureHandle, 1 );
    glUniform1f( m_CombineShader.StepSizeId, m_OutlineThickness );
    glUniform1f( m_CombineShader.SlopeBiasId, m_SlopeBias );

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the scene
//--------------------------------------------------------------------------------------
VOID CSample15::Render()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Draw scene normals
    SetFramebuffer( RT_NORMALS );
    RenderSceneNormals();

    // Draw the scene with hatch shading
    SetFramebuffer( RT_HATCH );
    RenderSceneHatch();

    // Finally, combine the two
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );
    RenderCombinedScene();


    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
