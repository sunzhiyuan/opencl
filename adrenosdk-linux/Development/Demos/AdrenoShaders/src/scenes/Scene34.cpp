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
#include "Scene34.h"


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader34::MaterialShader34()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader34::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
DownsizeShader34::DownsizeShader34()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID DownsizeShader34::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}




//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID AdaptLuminanceShader34::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
AdaptLuminanceShader34::AdaptLuminanceShader34()
{
    ShaderId = 0;
	ElapsedTime = 1.0f;
	Lambda = 0.1f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
LuminanceShader34::LuminanceShader34()
{
    ShaderId = 0;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID LuminanceShader34::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BrightPassShader34::BrightPassShader34()
{
    ShaderId = 0;
	MiddleGray = 0.18f;
	BrightPassWhite = 1.0f;
	BrightPassThreshold = 1.9f;
	BrightPassOffset = 0.3f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BrightPassShader34::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader34::CombineShader34()
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
VOID CombineShader34::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BlurShader34::BlurShader34()
{
    ShaderId = 0;
    StepSize = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BlurShader34::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PreviewShader34::PreviewShader34()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PreviewShader34::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject34::SimpleObject34()
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
VOID SimpleObject34::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
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
VOID SimpleObject34::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample34()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample34::CSample34( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 800;
    m_nHeight = 480;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample34::CheckFrameBufferStatus()
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
BOOL CSample34::Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture)
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
    m_UserInterface.AddFloatVariable( &m_CombineShader.GaussSpread, (char *)"Gauss filter spread" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease gauss spread" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase gauss spread" );
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

    m_AmbientLight = FRMVECTOR4( 0.1f, 0.1f, 0.1f, 0.0f );

    // Set up the objects
    m_Object.Drawable = &m_Mesh;
    m_Object.Position.y += yAdjust;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample34::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader34.vs", "Demos/AdrenoShaders/Shaders/MaterialShader34.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/DownsizeShader34.vs", "Demos/AdrenoShaders/Shaders/DownsizeShader34.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/GenerateLuminance34.vs", "Demos/AdrenoShaders/Shaders/GenerateLuminance34.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/AverageLuminance34.vs", "Demos/AdrenoShaders/Shaders/AverageLuminance34.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/AdaptLuminance34.vs", "Demos/AdrenoShaders/Shaders/AdaptLuminance34.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BrightPass34.vs", "Demos/AdrenoShaders/Shaders/BrightPass34.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CopyRT34.vs", "Demos/AdrenoShaders/Shaders/CopyRT34.fs",
            &m_CopyRTShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CopyRTShader.ColorTextureId = glGetUniformLocation( m_CopyRTShader.ShaderId, "g_ColorTexture" );
      //  m_AvgLuminanceShader.StepSizeId = glGetUniformLocation( m_CopyRTShader.ShaderId, "g_StepSize" );
    }

    // blur shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BlurShader34.vs", "Demos/AdrenoShaders/Shaders/BlurShader34.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CombineShader34.vs", "Demos/AdrenoShaders/Shaders/CombineShader34.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.SharpTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_SharpTexture" );
        m_CombineShader.BlurredTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_BlurredTexture" );
        m_CombineShader.DepthTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_DepthTexture" );
        m_CombineShader.NearQId = glGetUniformLocation( m_CombineShader.ShaderId, "g_NearQ" );
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PreviewShader34.vs", "Demos/AdrenoShaders/Shaders/PreviewShader34.fs",
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
BOOL CSample34::Resize()
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
VOID CSample34::Destroy()
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
// Name: GetCameraPos34()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos34( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( 4.5f, 2.0f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 1.7f * FrmCos( 0.25f * fTime ), 0.5f * FrmSin( 0.15f * fTime ), -1.9f * FrmSin( 0.25f * fTime ) );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample34::Update()
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

    if( nButtons & INPUT_KEY_6 )
    {
        m_CombineShader.GaussSpread -= 2.0f * ElapsedTime;
        m_CombineShader.GaussSpread = max( 0.0f, m_CombineShader.GaussSpread );
    }

    if( nButtons & INPUT_KEY_7 )
    {
        m_CombineShader.GaussSpread += 2.0f * ElapsedTime;
        m_CombineShader.GaussSpread = min( 3.0f, m_CombineShader.GaussSpread );
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
    FRMVECTOR3 LookPos = GetCameraPos34( TotalTime + 0.05f );
    m_CameraPos   = GetCameraPos34( TotalTime );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

	m_AdaptLuminanceShader.ElapsedTime = 20.0f;

    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: GaussianDistribution34()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 GaussianDistribution34( FLOAT32 X, FLOAT32 Y, FLOAT32 Rho )
{
    FLOAT32 Gauss = 1.0f / sqrtf( 2.0f * FRM_PI * Rho * Rho );
    Gauss *= expf( -( X * X + Y * Y ) / ( 2.0f * Rho * Rho ) );

    return Gauss;
}


//--------------------------------------------------------------------------------------
// Name: SetupGaussWeights()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample34::SetupGaussWeights()
{
    FLOAT32 GaussSum = 0.0f;
    for( int i = 0; i < 4; i++ )
    {
        // normalize to -1..1
        FLOAT32 X = ( FLOAT32( i ) / 3.0f );
        X = ( X - 0.5f ) * 2.0f;

        // spread is tunable
        X *= m_CombineShader.GaussSpread;

        m_BlurShader.GaussWeight[i] = GaussianDistribution34( X, 0.0f, m_CombineShader.GaussRho );

        GaussSum += m_BlurShader.GaussWeight[i];
    }

    m_BlurShader.GaussInvSum = ( 1.0f / GaussSum );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample34::DrawObject( SimpleObject34* Object )
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


VOID CSample34::PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle )
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
VOID CSample34::DrawScene()
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
VOID CSample34::GaussBlur( OffscreenRT4& InputTargetRT, OffscreenRT4& BlurredTargetRT )
{
    // Blur in the X direction
    {
        m_ScratchRT.SetFramebuffer();
        
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

       // FRMVECTOR2 StepSizeX = FRMVECTOR2( m_BlurShader.StepSize.x * m_CombineShader.GaussSpread, 0.0f );
        FRMVECTOR2 StepSizeX = FRMVECTOR2( (1.0f / InputTargetRT.Width) * m_CombineShader.GaussSpread, 0.0f );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeX.v );
        SetTexture( m_BlurShader.ColorTextureId, InputTargetRT.TextureHandle, 0 );

        RenderScreenAlignedQuad();
        
        m_ScratchRT.DetachFramebuffer();
    }

    // regular blur instead of Gauss blur
    {
        BlurredTargetRT.SetFramebuffer();

        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeY = FRMVECTOR2( (1.0f / InputTargetRT.Width), (1.0f / InputTargetRT.Height) );
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
VOID CSample34::Generate64x64Lum( OffscreenRT4& TargetRT )
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
VOID CSample34::Generate16x16Lum( OffscreenRT4& TargetRT )
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
VOID CSample34::Generate4x4Lum( OffscreenRT4& TargetRT )
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
VOID CSample34::Generate1x1Lum( OffscreenRT4& TargetRT )
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
VOID CSample34::GenerateAdaptLum( OffscreenRT4& CurrLumRT, OffscreenRT4& PrevLumRT )
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
VOID CSample34::Copy1x1Lum( OffscreenRT4& CopyToLumRT, OffscreenRT4& CopyFromLumRT )
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
VOID CSample34::BrightPass( OffscreenRT4& CopyToLumRT, OffscreenRT4& ScratchRT, OffscreenRT4& currLumRT )
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
        glUniform1f(m_BrightPassShader.MiddleGrayId, m_BrightPassShader.MiddleGray );
        glUniform1f(m_BrightPassShader.BrightPassWhiteId, m_BrightPassShader.BrightPassWhite);
        glUniform1f(m_BrightPassShader.BrightPassThresholdId, m_BrightPassShader.BrightPassThreshold);
        glUniform1f(m_BrightPassShader.BrightPassOffsetId, m_BrightPassShader.BrightPassOffset);

		RenderScreenAlignedQuad();
		
		CopyToLumRT.DetachFramebuffer();
}


//--------------------------------------------------------------------------------------
// Name: CreateBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample34::CreateBlurredImage()
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
        glUniform2f( m_DownsizeShader.StepSizeId, (1.0f / m_SharpRT.Width), (1.0f / m_SharpRT.Height));

        RenderScreenAlignedQuad();
        
        m_QuarterRT.DetachFramebuffer();
    }


    // Part 2: blur
    GaussBlur( m_QuarterRT, m_BlurredRT );

	// generate 64x64 luminance render target
	Generate64x64Lum(m_BlurredRT);

	// generate 16x16 luminance render target
	Generate16x16Lum(m_64x64RT);

	// generate 4x4 luminance render target
	Generate4x4Lum(m_16x16RT);

	// generate 1x1 luminance render target
	Generate1x1Lum(m_4x4RT);

	// adapt luminance
	GenerateAdaptLum(m_1x1RT, m_PreviousAverageLumRT);

	// copy the result of this operation into another 1x1 render target to store it for the next frame
	Copy1x1Lum(m_PreviousAverageLumRT, m_LastAverageLumRT);

	// bright pass filter
	BrightPass( m_BrightPassRT, m_QuarterRT, m_LastAverageLumRT);

	// bloom
	GaussBlur( m_BrightPassRT, m_QuarterRT);
}


//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample34::DrawCombinedScene()
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
    SetTexture( m_CombineShader.BlurredTextureId, m_BlurredRT.TextureHandle, 1 );
    SetTexture( m_CombineShader.DepthTextureId, m_SharpRT.DepthTextureHandle, 2 );
    SetTexture( m_CombineShader.CurrLumTextureId, m_LastAverageLumRT.TextureHandle, 3 );
    SetTexture( m_CombineShader.BloomTextureId, m_BrightPassRT.TextureHandle, 4 );
    glUniform1f(m_CombineShader.MiddleGrayId, m_CombineShader.MiddleGray );
    glUniform1f(m_CombineShader.WhiteId, m_CombineShader.White);

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample34::Render()
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
        yPos += ( Height + 0.05f );
/*        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_BrightPassRT.TextureHandle );
  		xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_BlurredRT.TextureHandle );
        yPos -= ( Height + 0.05f );
  		xPos -= ( Width + 0.05f );


        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_64x64RT.TextureHandle );
		xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_16x16RT.TextureHandle );
	    xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_4x4RT.TextureHandle );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_1x1RT.TextureHandle );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_LastAverageLumRT.TextureHandle );
        yPos += ( Height + 0.05f );
 */       
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height), m_PreviousAverageLumRT.TextureHandle );
	}

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
