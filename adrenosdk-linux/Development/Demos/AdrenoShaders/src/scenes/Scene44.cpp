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
#include "Scene44.h"


//--------------------------------------------------------------------------------------
// Name: GaussianDistribution44()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 GaussianDistribution44( FLOAT32 X, FLOAT32 Y, FLOAT32 Rho )
{
    FLOAT32 Gauss = 1.0f / sqrtf( 2.0f * FRM_PI * Rho * Rho );
    Gauss *= expf( -( X * X + Y * Y ) / ( 2.0f * Rho * Rho ) );

    return Gauss;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
NormalShader44::NormalShader44()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID NormalShader44::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PreviewShader44::PreviewShader44()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PreviewShader44::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
AOShader44::AOShader44()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID AOShader44::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BlurShader44::BlurShader44()
{
    ShaderId = 0;
    StepSize = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BlurShader44::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader44::CombineShader44()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CombineShader44::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject44::SimpleObject44()
{
    ModelScale = 1.0f;
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    BumpTexture = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject44::Update( FLOAT ElapsedTime, FRMMATRIX4X4& ViewMat, FRMMATRIX4X4& ProjMat, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );

    // Build the matrices
    ModelView     = FrmMatrixMultiply( ModelMatrix, ViewMat );
    ModelViewProj = FrmMatrixMultiply( ModelView, ProjMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject44::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample44()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample44::CSample44( const CHAR* strName ) : CScene( strName )
{
    m_JitterTexture = NULL;
    m_ViewPositionScalar = 10.0f;
    m_GaussSpread = 1.05f;
    m_GaussRho = 1.0f;
    m_AOIntensity = 3.5f;
    m_AOEdgeFinder = 16.67f;
    m_JitterAmount = 1.5f;


    m_nWidth = 800;
    m_nHeight = 600;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample44::CheckFrameBufferStatus()
{
    switch( glCheckFramebufferStatus(GL_FRAMEBUFFER) )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return TRUE;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            return FALSE;
    }
    FrmAssert(0);
    return FALSE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample44::Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;
    m_PreviewRT = FALSE;

    // Create the font
    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/44_Textures.pak" ) )
        return FALSE;

    // A jitter texture for AO
    m_JitterTexture = resource.GetTexture( "Jitter" );


    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_AOIntensity, (char *)"AO Intensity" );
    m_UserInterface.AddFloatVariable( &m_AOEdgeFinder, (char *)"AO edge highlight" );
    m_UserInterface.AddFloatVariable( &m_JitterAmount, (char *)"Jitter amount" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"AO intensity -" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"AO intensity +" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"AO edge -" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"AO edge +" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Jitter -" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Jitter +" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Map3.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 6.2f, 2.0f, 0.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    // Set up the objects
    m_Object.Drawable = &m_Mesh;
    m_Object.Position.y += yAdjust;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: SetupGaussWeights()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample44::SetupGaussWeights()
{
    FLOAT32 GaussSum = 0.0f;
    for( int i = 0; i < 4; i++ )
    {
        // normalize to -1..1
        FLOAT32 X = ( FLOAT32( i ) / 3.0f );
        X = ( X - 0.5f ) * 2.0f;

        // spread is tunable
        X *= m_GaussSpread;

        m_BlurShader.GaussWeight[i] = GaussianDistribution44( X, 0.0f, m_GaussRho );

        GaussSum += m_BlurShader.GaussWeight[i];
    }

    m_BlurShader.GaussInvSum = ( 1.0f / GaussSum );
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample44::InitShaders()
{
    // Normals shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_NormalIn",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/NormalShader44.vs", "Demos/AdrenoShaders/Shaders/NormalShader44.fs",
            &m_NormalShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_NormalShader.ModelViewProjMatrixId = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matModelViewProj" );
        m_NormalShader.ModelViewMatrixId     = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matModelView" );
    }

    // AO shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/AOShader44.vs", "Demos/AdrenoShaders/Shaders/AOShader44.fs",
            &m_AOShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_AOShader.NormalTextureId  = glGetUniformLocation( m_AOShader.ShaderId, "g_NormalTexture" );
        m_AOShader.DepthTextureId   = glGetUniformLocation( m_AOShader.ShaderId, "g_DepthTexture" );
        m_AOShader.JitterTextureId  = glGetUniformLocation( m_AOShader.ShaderId, "g_JitterTexture" );
        m_AOShader.ProjInvId        = glGetUniformLocation( m_AOShader.ShaderId, "g_ProjInv" );
        m_AOShader.StepSizeId       = glGetUniformLocation( m_AOShader.ShaderId, "g_StepSize" );
        m_AOShader.AOIntensityId    = glGetUniformLocation( m_AOShader.ShaderId, "g_AOIntensity" );
        m_AOShader.AOEdgeId         = glGetUniformLocation( m_AOShader.ShaderId, "g_AOEdgeFinder" );
        m_AOShader.JitterAmountId   = glGetUniformLocation( m_AOShader.ShaderId, "g_JitterAmount" );
    }

    // Blur shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BlurShader44.vs", "Demos/AdrenoShaders/Shaders/BlurShader44.fs",
            &m_BlurShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_BlurShader.ColorTextureId = glGetUniformLocation( m_BlurShader.ShaderId, "g_ColorTexture" );
        m_BlurShader.GaussWeightId = glGetUniformLocation( m_BlurShader.ShaderId, "g_GaussWeight" );
        m_BlurShader.GaussInvSumId = glGetUniformLocation( m_BlurShader.ShaderId, "g_GaussInvSum" );
        m_BlurShader.StepSizeId = glGetUniformLocation( m_BlurShader.ShaderId, "g_StepSize" );

        SetupGaussWeights();
    }

    // Combine shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CombineShader44.vs", "Demos/AdrenoShaders/Shaders/CombineShader44.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.AOTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_AOTexture" );
    }

    // Preview shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PreviewShader44.vs", "Demos/AdrenoShaders/Shaders/PreviewShader44.fs",
            &m_PreviewShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_PreviewShader.ColorTextureId = glGetUniformLocation( m_PreviewShader.ShaderId, "g_ColorTexture" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample44::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 86 );
    FLOAT32 zNear = 0.25f;
    FLOAT32 zFar = 20.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, fAspect, zNear, zFar );

    INT32 AOWidth = m_nWidth / 4;
    INT32 AOHeight = m_nHeight / 4;
    m_AOShader.ProjInv = FrmMatrixInverse( m_matProj );
    m_AOShader.StepSize = FRMVECTOR2( 1.0f / FLOAT32( AOWidth ), 1.0f / FLOAT32( AOHeight ) );
    m_BlurShader.StepSize = m_AOShader.StepSize;


    // To keep sizes consistent, scale the projection matrix in landscape orientation
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    m_NormalRT.CreateRT( AOWidth, AOHeight, TRUE );
    m_AORT.CreateRT( AOWidth, AOHeight, FALSE );
    m_ScratchRT.CreateRT( AOWidth, AOHeight, FALSE );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample44::Destroy()
{
    // Free shader memory
    m_NormalShader.Destroy();
    m_AOShader.Destroy();
    m_PreviewShader.Destroy();
    m_CombineShader.Destroy();
    m_BlurShader.Destroy();


    // Free texture memory
    if( m_JitterTexture ) m_JitterTexture->Release();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: GaussBlur()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample44::GaussBlur( OffscreenRT4& TargetRT )
{
    // Blur in the X direction
    {
        m_ScratchRT.SetFramebuffer();

        glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeX = FRMVECTOR2( m_BlurShader.StepSize.x * m_GaussSpread, 0.0f );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeX.v );
        SetTexture( m_BlurShader.ColorTextureId, TargetRT.TextureHandle, 0 );

        RenderScreenAlignedQuad();

        m_ScratchRT.DetachFramebuffer();
    }

    // Then blur in the Y direction
    {
        TargetRT.SetFramebuffer();

        glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeY = FRMVECTOR2( 0.0f, m_BlurShader.StepSize.y * m_GaussSpread );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeY.v );
        SetTexture( m_BlurShader.ColorTextureId, m_ScratchRT.TextureHandle, 0 );

        RenderScreenAlignedQuad();

        TargetRT.DetachFramebuffer();
    }
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos44()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos44( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( 4.5f, 2.0f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 1.7f * FrmCos( 0.25f * fTime ), 0.5f * FrmSin( 0.15f * fTime ), -1.9f * FrmSin( 0.25f * fTime ) );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample44::Update()
{
    // get the current time
    FLOAT32 ElapsedTime = m_Timer.GetFrameElapsedTime();

    // process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();


    if( nButtons & INPUT_KEY_2 )
    {
        m_AOIntensity -= 5.0f * ElapsedTime;
        m_AOIntensity = max( 0.25f, m_AOIntensity );
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_AOIntensity += 5.0f * ElapsedTime;
        m_AOIntensity = min( 5.0f, m_AOIntensity );
    }

    if( nButtons & INPUT_KEY_4 )
    {
        m_AOEdgeFinder -= 50.0f * ElapsedTime;
        m_AOEdgeFinder = max( 0.0f, m_AOEdgeFinder );
    }

    if( nButtons & INPUT_KEY_5 )
    {
        m_AOEdgeFinder += 50.0f * ElapsedTime;
        m_AOEdgeFinder = min( 50.0f, m_AOEdgeFinder );
    }

    if( nButtons & INPUT_KEY_6 )
    {
        m_JitterAmount -= 2.0f * ElapsedTime;
        m_JitterAmount = max( 0.1f, m_JitterAmount );
    }

    if( nButtons & INPUT_KEY_7 )
    {
        m_JitterAmount += 2.0f * ElapsedTime;
        m_JitterAmount = min( 2.0f, m_JitterAmount );
    }

    // toggle rotation
    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_ShouldRotate = 1 - m_ShouldRotate;
    }

    // toggle RT preview
    if( nPressedButtons & INPUT_KEY_9 )
    {
        m_PreviewRT = 1 - m_PreviewRT;
    }

    // update light position
    m_LightPos = FRMVECTOR3( 3.3f, 2.0f, -0.42f );

    // animate the camera
    static FLOAT32 TotalTime = 0.0f;
    if( m_ShouldRotate )
    {
        static FLOAT32 CameraSpeed = 1.0f;
        TotalTime += ElapsedTime * CameraSpeed;
    }

    // build camera transforms
    FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_CameraPos = FRMVECTOR3( 5.5f, 2.0f, 2.0f );
    FRMVECTOR3 LookPos = m_CameraPos + FRMVECTOR3( -0.5f, -0.3f, -1.0f );


    // Move cam around a little
    LookPos.x += FrmCos( TotalTime * 0.5f ) * 0.2f;


    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // update object
    m_Object.Update( ElapsedTime, m_matView, m_matProj, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: DrawNormalsAndDepth()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample44::DrawNormalsAndDepth()
{
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glFrontFace( GL_CW );
    glDepthFunc( GL_LESS );
    glDepthMask( TRUE );

    m_NormalRT.SetFramebuffer();

    // Clear color, depth
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepthf( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set shader variables
    glUseProgram( m_NormalShader.ShaderId );
    glUniformMatrix4fv( m_NormalShader.ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&m_Object.ModelViewProj );
    glUniformMatrix4fv( m_NormalShader.ModelViewMatrixId, 1, FALSE, (FLOAT32*)&m_Object.ModelView );

    m_Object.Drawable->Render();

    m_NormalRT.DetachFramebuffer();
    glDepthMask( FALSE );
}


//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample44::DrawAO()
{
    // Part 1: create AO
    {
        m_AORT.SetFramebuffer();

        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_AOShader.ShaderId );
        SetTexture( m_AOShader.NormalTextureId, m_NormalRT.TextureHandle, 0 );
        SetTexture( m_AOShader.DepthTextureId, m_NormalRT.DepthTextureHandle, 1 );
        SetTexture( m_AOShader.JitterTextureId, m_JitterTexture->m_hTextureHandle, 2 );
        glUniformMatrix4fv( m_AOShader.ProjInvId, 1, FALSE, (FLOAT32*)&m_AOShader.ProjInv );
        glUniform2fv( m_AOShader.StepSizeId, 1, (FLOAT32*)&m_AOShader.StepSize.v );
        glUniform1f( m_AOShader.AOIntensityId, m_AOIntensity );
        glUniform1f( m_AOShader.AOEdgeId, m_AOEdgeFinder );
        glUniform1f( m_AOShader.JitterAmountId, m_JitterAmount * 0.1f );

        RenderScreenAlignedQuad();

        m_AORT.DetachFramebuffer();
    }

    // Part 2: blur
    GaussBlur( m_AORT );
    GaussBlur( m_AORT );
}


//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample44::DrawCombinedScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    glUseProgram( m_CombineShader.ShaderId );
    SetTexture( m_CombineShader.AOTextureId, m_AORT.TextureHandle, 0 );

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: PreviewRT()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample44::PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle )
{
    TopLeft.y = -TopLeft.y;
    Dims.y = -Dims.y;

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    glUseProgram( m_PreviewShader.ShaderId );
    SetTexture( m_PreviewShader.ColorTextureId, TextureHandle, 0 );

    RenderScreenAlignedQuad( TopLeft, TopLeft + Dims );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample44::Render()
{
    // Fill up normal RT and depth
    DrawNormalsAndDepth();

    // Calculate AO
    DrawAO();

    // Put everything together
    DrawCombinedScene();


    // Preview RTs
    if( m_PreviewRT )
    {
        FLOAT32 xPos = -0.975f;
        FLOAT32 yPos = -0.95f;
        FLOAT32 Width = 0.3f;
        FLOAT32 Height = 0.3f;
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_NormalRT.TextureHandle );
        xPos += ( Width + 0.025f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_NormalRT.DepthTextureHandle );
        xPos += ( Width + 0.025f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_AORT.TextureHandle );
    }

    // Put the state back
    glDepthMask( TRUE );
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glFrontFace( GL_CCW );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
