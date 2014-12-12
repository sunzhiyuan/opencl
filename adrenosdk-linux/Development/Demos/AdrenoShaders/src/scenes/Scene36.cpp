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
#include "Scene36.h"


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader36::MaterialShader36()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader36::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
DownsizeShader36::DownsizeShader36()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID DownsizeShader36::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}




//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID AdaptLuminanceShader36::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
AdaptLuminanceShader36::AdaptLuminanceShader36()
{
    ShaderId = 0;
	ElapsedTime = 1.0f;
	Lambda = 0.1f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
LuminanceShader36::LuminanceShader36()
{
    ShaderId = 0;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID LuminanceShader36::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BrightPassShader36::BrightPassShader36()
{
    ShaderId = 0;
	MiddleGray = 0.18f;
	BrightPassWhite = 1.0f;
	BrightPassThreshold = 0.1f;
	BrightPassOffset = 2.0f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BrightPassShader36::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader36::CombineShader36()
{
    ShaderId = 0;

    GaussSpread = 3.0f;
    GaussRho = 1.0f;
	MiddleGray = 0.18f;
	White = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CombineShader36::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BlurShader36::BlurShader36()
{
    ShaderId = 0;
    StepSize = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BlurShader36::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PreviewShader36::PreviewShader36()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PreviewShader36::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject36::SimpleObject36()
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
VOID SimpleObject36::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject36::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample36()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample36::CSample36( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 800;
    m_nHeight = 480;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample36::CheckFrameBufferStatus()
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
BOOL CSample36::Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;
    m_Preview = false;

    // Create the font
    // Load the packed resources
    // NOTE SAME TEXTURES AS 27
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/27_Textures.pak" ) )
        return FALSE;


    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_BrightPassShader.BrightPassThreshold, (char *)"Bright pass threshold" );
    m_UserInterface.AddFloatVariable( &m_BrightPassShader.BrightPassOffset, (char *)"Bright pass offset" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.GaussSpread, (char *)"Gauss filter spread" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Decrease bright pass threshold" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Increase bright pass threshold" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Decrease bright pass offset" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Increase bright pass offset" );
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

    m_AmbientLight = FRMVECTOR4( 0.01f, 0.01f, 0.01f, 0.0f );

    // Set up the objects
    m_Object.Drawable = &m_Mesh;
    m_Object.Position.y += yAdjust;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample36::InitShaders()
{
    // Create the material shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT },
            { "g_BinormalIn", FRM_VERTEX_BINORMAL }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader36.vs", "Demos/AdrenoShaders/Shaders/MaterialShader36.fs",
            &m_MaterialShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_MaterialShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_MaterialShader.ShaderId, "g_MatModelViewProj" );
        m_MaterialShader.m_ModelMatrixId         = glGetUniformLocation( m_MaterialShader.ShaderId, "g_MatModel" );
        m_MaterialShader.m_LightPositionId       = glGetUniformLocation( m_MaterialShader.ShaderId, "g_LightPos" );
        m_MaterialShader.m_EyePositionId         = glGetUniformLocation( m_MaterialShader.ShaderId, "g_EyePos" );
        m_MaterialShader.m_AmbientColorId        = glGetUniformLocation( m_MaterialShader.ShaderId, "g_AmbientColor" );
        m_MaterialShader.m_DiffuseTextureId      = glGetUniformLocation( m_MaterialShader.ShaderId, "g_DiffuseTexture" );
        m_MaterialShader.m_BumpTextureId         = glGetUniformLocation( m_MaterialShader.ShaderId, "g_BumpTexture" );
    }

    // downsize shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/DownsizeShader36.vs", "Demos/AdrenoShaders/Shaders/DownsizeShader36.fs",
            &m_DownsizeShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_DownsizeShader.ColorTextureId = glGetUniformLocation( m_DownsizeShader.ShaderId, "g_ColorTexture" );
        m_DownsizeShader.StepSizeId = glGetUniformLocation( m_DownsizeShader.ShaderId, "g_StepSize" );
    }

    // luminance shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/GenerateLuminance36.vs", "Demos/AdrenoShaders/Shaders/GenerateLuminance36.fs",
            &m_LuminanceShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_LuminanceShader.ColorTextureId = glGetUniformLocation( m_LuminanceShader.ShaderId, "g_ColorTexture" );
        m_LuminanceShader.StepSizeId = glGetUniformLocation( m_LuminanceShader.ShaderId, "g_StepSize" );
    }

    // average luminance shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/AverageLuminance36.vs", "Demos/AdrenoShaders/Shaders/AverageLuminance36.fs",
            &m_AvgLuminanceShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_AvgLuminanceShader.ColorTextureId = glGetUniformLocation( m_AvgLuminanceShader.ShaderId, "g_ColorTexture" );
        m_AvgLuminanceShader.StepSizeId = glGetUniformLocation( m_AvgLuminanceShader.ShaderId, "g_StepSize" );
    }

    // adapt luminance shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/AdaptLuminance36.vs", "Demos/AdrenoShaders/Shaders/AdaptLuminance36.fs",
            &m_AdaptLuminanceShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_AdaptLuminanceShader.CurrLumTextureId = glGetUniformLocation( m_AdaptLuminanceShader.ShaderId, "g_CurrentLuminance" );
        m_AdaptLuminanceShader.PrevLumTextureId = glGetUniformLocation( m_AdaptLuminanceShader.ShaderId, "g_PreviousLuminance" );
        m_AdaptLuminanceShader.ElapsedTimeId = glGetUniformLocation( m_AdaptLuminanceShader.ShaderId, "ElapsedTime" );
        m_AdaptLuminanceShader.LambdaId = glGetUniformLocation( m_AdaptLuminanceShader.ShaderId, "Lambda" );
	}

    // bright pass shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BrightPass36.vs", "Demos/AdrenoShaders/Shaders/BrightPass36.fs",
            &m_BrightPassShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_BrightPassShader.ScratchTextureId = glGetUniformLocation( m_BrightPassShader.ShaderId, "g_ScratchTexture" );
        m_BrightPassShader.CurrLumTextureId = glGetUniformLocation( m_BrightPassShader.ShaderId, "g_CurrentLuminance" );
        m_BrightPassShader.MiddleGrayId = glGetUniformLocation( m_BrightPassShader.ShaderId, "MiddleGray" );
        m_BrightPassShader.BrightPassWhiteId = glGetUniformLocation( m_BrightPassShader.ShaderId, "BrightPassWhite" );
        m_BrightPassShader.BrightPassThresholdId = glGetUniformLocation( m_BrightPassShader.ShaderId, "BrightPassThreshold" );
        m_BrightPassShader.BrightPassOffsetId = glGetUniformLocation( m_BrightPassShader.ShaderId, "BrightPassOffset" );
	}


    // copy render target shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CopyRT36.vs", "Demos/AdrenoShaders/Shaders/CopyRT36.fs",
            &m_CopyRTShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CopyRTShader.ColorTextureId = glGetUniformLocation( m_CopyRTShader.ShaderId, "g_ColorTexture" );
    }

    // blur shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BlurShader36.vs", "Demos/AdrenoShaders/Shaders/BlurShader36.fs",
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

    // combine shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CombineShader36.vs", "Demos/AdrenoShaders/Shaders/CombineShader36.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.SharpTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_SharpTexture" );
        m_CombineShader.CurrLumTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_CurrentLuminance" );
        m_CombineShader.BloomTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_BloomTexture" );
	    m_CombineShader.MiddleGrayId = glGetUniformLocation( m_CombineShader.ShaderId, "MiddleGray" );
        m_CombineShader.WhiteId = glGetUniformLocation( m_CombineShader.ShaderId, "White" );
    }

    // preview shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PreviewShader36.vs", "Demos/AdrenoShaders/Shaders/PreviewShader36.fs",
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
BOOL CSample36::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 86 );
    FLOAT32 zNear = 0.25f;
    FLOAT32 zFar = 20.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, fAspect, zNear, zFar );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    m_SharpRT.CreateRT( m_nWidth / 2, m_nHeight / 2, TRUE );
	m_QuarterRT.CreateRT( m_nWidth / 4, m_nHeight / 4, FALSE );
    m_BlurredRT.CreateRT( m_nWidth / 4, m_nHeight / 4, FALSE );
    m_ScratchRT.CreateRT( m_nWidth / 4, m_nHeight / 4, FALSE );
	m_64x64RT.CreateRT( 64, 64, FALSE);
	m_16x16RT.CreateRT( 16, 16, FALSE);
	m_4x4RT.CreateRT( 4, 4, FALSE);
	m_1x1RT.CreateRT( 1, 1, FALSE);
	m_PreviousAverageLumRT.CreateRT( 1, 1, FALSE);
	m_LastAverageLumRT.CreateRT( 1, 1, FALSE);
    m_BrightPassRT.CreateRT( m_nWidth / 4, m_nHeight / 4, FALSE );

	m_BlurShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );
	m_LuminanceShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );
	m_AvgLuminanceShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );
    m_DownsizeShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );

    m_LastAverageLumRT.SetFramebuffer();
    glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT);

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::Destroy()
{
    // Free shader memory
    m_MaterialShader.Destroy();
    m_CombineShader.Destroy();
    m_DownsizeShader.Destroy();
    m_BlurShader.Destroy();
    m_LuminanceShader.Destroy();
    m_AvgLuminanceShader.Destroy();
    m_CopyRTShader.Destroy();

    m_PreviewShader.Destroy();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos36()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos36( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( -0.5f, 2.0f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 3.0f * FrmCos( 0.5f * fTime ), 0.0f, 0.0f );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::Update()
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

    // toggle rotation
    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_ShouldRotate = 1 - m_ShouldRotate;
    }

    // toggle previews
    if( nPressedButtons & INPUT_KEY_9 )
    {
        m_Preview = 1 - m_Preview;
    }

    if( nButtons & INPUT_KEY_1 )
    {
        m_BrightPassShader.BrightPassThreshold -= 2.0f * ElapsedTime;
        m_BrightPassShader.BrightPassThreshold = max( 0.0f, m_BrightPassShader.BrightPassThreshold );
    }

    if( nButtons & INPUT_KEY_2 )
    {
        m_BrightPassShader.BrightPassThreshold += 2.0f * ElapsedTime;
        m_BrightPassShader.BrightPassThreshold = min( 2.0f, m_BrightPassShader.BrightPassThreshold );
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_BrightPassShader.BrightPassOffset -= 2.0f * ElapsedTime;
        m_BrightPassShader.BrightPassOffset = max( 0.0f, m_BrightPassShader.BrightPassOffset );
    }

    if( nButtons & INPUT_KEY_4 )
    {
        m_BrightPassShader.BrightPassOffset += 2.0f * ElapsedTime;
        m_BrightPassShader.BrightPassOffset = min( 2.0f, m_BrightPassShader.BrightPassOffset );
    }

    // update light position
    m_LightPos = FRMVECTOR3( 5.3f, 2.0f, 0.0f );

    // animate the camera
    static FLOAT32 TotalTime = 0.0f;
    if( m_ShouldRotate )
    {
        static FLOAT32 CameraSpeed = 1.0f;
        TotalTime += ElapsedTime * CameraSpeed;
    }

    // build camera transforms
    FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 LookPos;

    m_CameraPos   = GetCameraPos36( TotalTime );
    LookPos       = m_CameraPos + FRMVECTOR3( 1.0f, 0.0f, 0.0f );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

	m_AdaptLuminanceShader.ElapsedTime = 16.0f;
	m_AdaptLuminanceShader.Lambda = 0.09f;

    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: GaussianDistribution36()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 GaussianDistribution36( FLOAT32 X, FLOAT32 Y, FLOAT32 Rho )
{
    FLOAT32 Gauss = 1.0f / sqrtf( 2.0f * FRM_PI * Rho * Rho );
    Gauss *= expf( -( X * X + Y * Y ) / ( 2.0f * Rho * Rho ) );

    return Gauss;
}


//--------------------------------------------------------------------------------------
// Name: SetupGaussWeights()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::SetupGaussWeights()
{
    FLOAT32 GaussSum = 0.0f;
    for( int i = 0; i < 4; i++ )
    {
        // normalize to -1..1
        FLOAT32 X = ( FLOAT32( i ) / 3.0f );
        X = ( X - 0.5f ) * 2.0f;

        // spread is tunable
        X *= m_CombineShader.GaussSpread;

        m_BlurShader.GaussWeight[i] = GaussianDistribution36( X, 0.0f, m_CombineShader.GaussRho );

        GaussSum += m_BlurShader.GaussWeight[i];
    }

    m_BlurShader.GaussInvSum = ( 1.0f / GaussSum );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::DrawObject( SimpleObject36* Object )
{
    FRMMATRIX4X4    MatModel;
    FRMMATRIX4X4    MatModelView;
    FRMMATRIX4X4    MatModelViewProj;
    FRMMATRIX4X4    MatNormal;
    FRMVECTOR3      Eye = m_CameraPos;

    if ( !Object || !Object->Drawable )
        return;

    MatModel         = Object->ModelMatrix;
    MatModelView     = FrmMatrixMultiply( MatModel, m_matView );
    MatModelViewProj = FrmMatrixMultiply( MatModelView, m_matProj );
    MatNormal        = FrmMatrixInverse( MatModel );

    glUseProgram( m_MaterialShader.ShaderId );
    glUniformMatrix4fv( m_MaterialShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_MaterialShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniform3fv( m_MaterialShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_MaterialShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_MaterialShader.m_AmbientColorId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform1i( m_MaterialShader.m_DiffuseTextureId, 0 );
    glUniform1i( m_MaterialShader.m_BumpTextureId, 1 );

    Object->Drawable->Render();
}


VOID CSample36::PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle )
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
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::DrawScene()
{
    m_SharpRT.SetFramebuffer();

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepthf( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glFrontFace( GL_CW );

    DrawObject( &m_Object );
    
	m_SharpRT.DetachFramebuffer();         
}


//--------------------------------------------------------------------------------------
// Name: GaussBlur()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::GaussBlur( OffscreenRT4& InputTargetRT, OffscreenRT4& BlurredTargetRT )
{
    // Blur in the X direction
    {
        m_ScratchRT.SetFramebuffer();

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeX = FRMVECTOR2( (1.0f / InputTargetRT.Width) * m_CombineShader.GaussSpread, 0.0f );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeX.v );
        SetTexture( m_BlurShader.ColorTextureId, InputTargetRT.TextureHandle, 0 );

        RenderScreenAlignedQuad();
        
	 	m_ScratchRT.DetachFramebuffer();         
    }

    // Then blur in the Y direction
    {
        BlurredTargetRT.SetFramebuffer();

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeY = FRMVECTOR2( 0.0f,  (1.0f / InputTargetRT.Height) * m_CombineShader.GaussSpread );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeY.v );
        SetTexture( m_BlurShader.ColorTextureId, m_ScratchRT.TextureHandle, 0 );

        RenderScreenAlignedQuad();
        
	 	BlurredTargetRT.DetachFramebuffer();         
    }
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::Generate64x64Lum( OffscreenRT4& TargetRT )
{
        m_64x64RT.SetFramebuffer();

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT);
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_LuminanceShader.ShaderId );
        SetTexture( m_LuminanceShader.ColorTextureId, TargetRT.TextureHandle, 0 );
        glUniform2f( m_LuminanceShader.StepSizeId, 1.0f / TargetRT.Width, 1.0f / TargetRT.Height);

        RenderScreenAlignedQuad();
        
	 	m_64x64RT.DetachFramebuffer();         
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::Generate16x16Lum( OffscreenRT4& TargetRT )
{
        m_16x16RT.SetFramebuffer();

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT);
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_AvgLuminanceShader.ShaderId );
        SetTexture( m_AvgLuminanceShader.ColorTextureId, TargetRT.TextureHandle, 0 );
        glUniform2f( m_AvgLuminanceShader.StepSizeId, 1.0f / TargetRT.Width, 1.0f / TargetRT.Height);

        RenderScreenAlignedQuad();
        
	 	m_16x16RT.DetachFramebuffer();         
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::Generate4x4Lum( OffscreenRT4& TargetRT )
{
        m_4x4RT.SetFramebuffer();

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT);
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_AvgLuminanceShader.ShaderId );
        SetTexture( m_AvgLuminanceShader.ColorTextureId, TargetRT.TextureHandle, 0 );
        glUniform2f( m_AvgLuminanceShader.StepSizeId, 1.0f / TargetRT.Width, 1.0f / TargetRT.Height);

        RenderScreenAlignedQuad();
        
	 	m_4x4RT.DetachFramebuffer();         
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::Generate1x1Lum( OffscreenRT4& TargetRT )
{
        m_1x1RT.SetFramebuffer();

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT);
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_AvgLuminanceShader.ShaderId );
        SetTexture( m_AvgLuminanceShader.ColorTextureId, TargetRT.TextureHandle, 0 );
        glUniform2f(m_AvgLuminanceShader.StepSizeId, 1.0f / TargetRT.Width, 1.0f / TargetRT.Height);

        RenderScreenAlignedQuad();
        
	 	m_1x1RT.DetachFramebuffer();         
}

//--------------------------------------------------------------------------------------
// Name: GenerateAdaptLum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::GenerateAdaptLum( OffscreenRT4& CurrLumRT, OffscreenRT4& PrevLumRT )
{
		m_LastAverageLumRT.SetFramebuffer();

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT);
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_AdaptLuminanceShader.ShaderId );
        SetTexture( m_AdaptLuminanceShader.CurrLumTextureId, CurrLumRT.TextureHandle, 0 );
        SetTexture( m_AdaptLuminanceShader.PrevLumTextureId, PrevLumRT.TextureHandle, 1 );
        glUniform1f( m_AdaptLuminanceShader.ElapsedTimeId, m_AdaptLuminanceShader.ElapsedTime );
        glUniform1f( m_AdaptLuminanceShader.LambdaId, m_AdaptLuminanceShader.Lambda);

        RenderScreenAlignedQuad();
        
	 	m_LastAverageLumRT.DetachFramebuffer();         
}

//--------------------------------------------------------------------------------------
// Name: Copy1x1Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::Copy1x1Lum( OffscreenRT4& CopyToLumRT, OffscreenRT4& CopyFromLumRT )
{
        CopyToLumRT.SetFramebuffer();

		glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_CopyRTShader.ShaderId );
        SetTexture( m_CopyRTShader.ColorTextureId, CopyFromLumRT.TextureHandle, 0 );

		RenderScreenAlignedQuad();
		
	 	CopyToLumRT.DetachFramebuffer();         
}

//--------------------------------------------------------------------------------------
// Name: BrightPass()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::BrightPass( OffscreenRT4& CopyToLumRT, OffscreenRT4& ScratchRT, OffscreenRT4& currLumRT )
{
        CopyToLumRT.SetFramebuffer();

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT);
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_BrightPassShader.ShaderId );
        SetTexture( m_BrightPassShader.ScratchTextureId, ScratchRT.TextureHandle, 0 );
        SetTexture( m_BrightPassShader.CurrLumTextureId, currLumRT.TextureHandle, 1 );
        glUniform1f( m_BrightPassShader.MiddleGrayId, m_BrightPassShader.MiddleGray );
        glUniform1f( m_BrightPassShader.BrightPassWhiteId, m_BrightPassShader.BrightPassWhite );
        glUniform1f( m_BrightPassShader.BrightPassThresholdId, m_BrightPassShader.BrightPassThreshold );
        glUniform1f( m_BrightPassShader.BrightPassOffsetId, m_BrightPassShader.BrightPassOffset );

		RenderScreenAlignedQuad();
		
	 	CopyToLumRT.DetachFramebuffer();         		
}


//--------------------------------------------------------------------------------------
// Name: CreateBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::CreateBlurredImage()
{
    // Part 1: downsize the framebuffer to a smaller render target
    {
        m_QuarterRT.SetFramebuffer();

        // Render a full-screen quad
        glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_DownsizeShader.ShaderId );
        SetTexture( m_DownsizeShader.ColorTextureId, m_SharpRT.TextureHandle, 0 );
        glUniform2f( m_DownsizeShader.StepSizeId, 1.0f / m_SharpRT.Width, 1.0f / m_SharpRT.Height);

        RenderScreenAlignedQuad();
        
	 	m_QuarterRT.DetachFramebuffer();                 
    }

    // Part 2: blur
    GaussBlur( m_QuarterRT, m_BlurredRT );

	// generate 64x64 luminance render target
	Generate64x64Lum( m_BlurredRT );

	// generate 16x16 luminance render target
	Generate16x16Lum( m_64x64RT );

	// generate 4x4 luminance render target
	Generate4x4Lum( m_16x16RT );

	// generate 1x1 luminance render target
	Generate1x1Lum( m_4x4RT );

	// adapt luminance
	GenerateAdaptLum( m_1x1RT, m_PreviousAverageLumRT );

	// copy the result of this operation into another 1x1 render target to store it for the next frame
	Copy1x1Lum( m_PreviousAverageLumRT, m_LastAverageLumRT );

	// bright pass filter
	BrightPass( m_BrightPassRT, m_QuarterRT, m_LastAverageLumRT );

	// bloom
	GaussBlur( m_BrightPassRT, m_QuarterRT );
    GaussBlur( m_QuarterRT, m_BlurredRT );
}


//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::DrawCombinedScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    glUseProgram( m_CombineShader.ShaderId );
    SetTexture( m_CombineShader.SharpTextureId, m_SharpRT.TextureHandle, 0 );
    SetTexture( m_CombineShader.CurrLumTextureId, m_LastAverageLumRT.TextureHandle, 3 );
    SetTexture( m_CombineShader.BloomTextureId, m_BlurredRT.TextureHandle, 4 );
    glUniform1f(m_CombineShader.MiddleGrayId, m_CombineShader.MiddleGray );
    glUniform1f(m_CombineShader.WhiteId, m_CombineShader.White);

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample36::Render()
{
    // Draw regular stuff to the framebuffer
    DrawScene();

    // Downsize framebuffer and blur
    CreateBlurredImage();

    // Recombine scene with depth of field
    DrawCombinedScene();

    // Show render target previews
    if( m_Preview )
    {
        FLOAT32 xPos = -0.95f;
        FLOAT32 yPos = -0.95f;
        FLOAT32 Width = 0.2f;
        FLOAT32 Height = 0.2f;

        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_BrightPassRT.TextureHandle );
        xPos += ( Width + 0.1f );

        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_64x64RT.TextureHandle );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_16x16RT.TextureHandle );
	    xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_4x4RT.TextureHandle );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_1x1RT.TextureHandle );
        xPos += ( Width + 0.1f );

        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_LastAverageLumRT.TextureHandle );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_PreviousAverageLumRT.TextureHandle );
	}

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
