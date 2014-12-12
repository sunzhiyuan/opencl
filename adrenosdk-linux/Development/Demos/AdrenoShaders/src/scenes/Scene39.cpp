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
#include "Scene39.h"


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader39::MaterialShader39()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader39::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
DownsizeShader39::DownsizeShader39()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID DownsizeShader39::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}




//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID AdaptLuminanceShader39::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
AdaptLuminanceShader39::AdaptLuminanceShader39()
{
    ShaderId = 0;
	ElapsedTime = 1.0f;
	Lambda = 0.1f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
LuminanceShader39::LuminanceShader39()
{
    ShaderId = 0;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID LuminanceShader39::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BrightPassShader39::BrightPassShader39()
{
    ShaderId = 0;
	MiddleGray = 0.18f;
	BrightPassWhite = 1.0f;
	BrightPassThreshold = 3.0f;
	BrightPassOffset = 3.0f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BrightPassShader39::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader39::CombineShader39()
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
VOID CombineShader39::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BlurShader39::BlurShader39()
{
    ShaderId = 0;
    StepSize = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BlurShader39::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
LightStreakShader39::LightStreakShader39()
{
    ShaderId = 0;
    StepSize = FRMVECTOR2( 1.0f, 1.0f );

    StreakDir = 1.18f;
    StreakLength = 6.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID LightStreakShader39::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PreviewShader39::PreviewShader39()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PreviewShader39::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject39::SimpleObject39()
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
VOID SimpleObject39::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
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
VOID SimpleObject39::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample39()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample39::CSample39( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 800;
    m_nHeight = 480;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample39::CheckFrameBufferStatus()
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
BOOL CSample39::Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;
    m_Preview = FALSE;

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
    m_UserInterface.AddFloatVariable( &m_LightStreakShader.StreakDir, (char *)"Streak direction" );
    m_UserInterface.AddFloatVariable( &m_LightStreakShader.StreakLength, (char *)"Streak length" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Rotate streak dir +" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Rotate streak dir -" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Decrease streak length" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Increase streak length" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_9, (char *)"Preview bright pass RT" );

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
BOOL CSample39::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader39.vs", "Demos/AdrenoShaders/Shaders/MaterialShader39.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/DownsizeShader39.vs", "Demos/AdrenoShaders/Shaders/DownsizeShader39.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/GenerateLuminance39.vs", "Demos/AdrenoShaders/Shaders/GenerateLuminance39.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/AverageLuminance39.vs", "Demos/AdrenoShaders/Shaders/AverageLuminance39.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/AdaptLuminance39.vs", "Demos/AdrenoShaders/Shaders/AdaptLuminance39.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BrightPass39.vs", "Demos/AdrenoShaders/Shaders/BrightPass39.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CopyRT39.vs", "Demos/AdrenoShaders/Shaders/CopyRT39.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BlurShader39.vs", "Demos/AdrenoShaders/Shaders/BlurShader39.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CombineShader39.vs", "Demos/AdrenoShaders/Shaders/CombineShader39.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.SharpTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_SharpTexture" );
        m_CombineShader.CurrLumTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_CurrentLuminance" );
        m_CombineShader.StreakTexture1Id = glGetUniformLocation( m_CombineShader.ShaderId, "g_StreakTexture1" );
        m_CombineShader.StreakTexture2Id = glGetUniformLocation( m_CombineShader.ShaderId, "g_StreakTexture2" );
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PreviewShader39.vs", "Demos/AdrenoShaders/Shaders/PreviewShader39.fs",
            &m_PreviewShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_PreviewShader.ColorTextureId = glGetUniformLocation( m_PreviewShader.ShaderId, "g_ColorTexture" );
    }

    // light streak shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/LightStreakShader39.vs", "Demos/AdrenoShaders/Shaders/LightStreakShader39.fs",
            &m_LightStreakShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_LightStreakShader.ColorTextureId = glGetUniformLocation( m_LightStreakShader.ShaderId, "g_ColorTexture" );
        m_LightStreakShader.WeightsId = glGetUniformLocation( m_LightStreakShader.ShaderId, "g_Weights" );
        m_LightStreakShader.StreakDirId = glGetUniformLocation( m_LightStreakShader.ShaderId, "g_StreakDir" );
        m_LightStreakShader.StreakLengthId = glGetUniformLocation( m_LightStreakShader.ShaderId, "g_StreakLength" );
        m_LightStreakShader.StepSizeId = glGetUniformLocation( m_LightStreakShader.ShaderId, "g_StepSize" );

        SetupGaussWeights();
    }


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample39::Resize()
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
    m_LightStreakShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );
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
VOID CSample39::Destroy()
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
// Name: GetCameraPos39()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos39( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( 4.5f, 2.0f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 1.7f * FrmCos( 0.25f * fTime ), 0.5f * FrmSin( 0.15f * fTime ), -1.9f * FrmSin( 0.25f * fTime ) );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample39::Update()
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
        m_LightStreakShader.StreakDir -= FRM_PI * ElapsedTime;
        while( m_LightStreakShader.StreakDir < 0.0f )
        {
            m_LightStreakShader.StreakDir += FRM_PI;
        }
    }

    if( nButtons & INPUT_KEY_2 )
    {
        m_LightStreakShader.StreakDir += FRM_PI * ElapsedTime;
        while( m_LightStreakShader.StreakDir > FRM_PI )
        {
            m_LightStreakShader.StreakDir -= FRM_PI;
        }
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_LightStreakShader.StreakLength -= 6.0f * ElapsedTime;
        m_LightStreakShader.StreakLength = max( 0.0f, m_LightStreakShader.StreakLength );
    }

    if( nButtons & INPUT_KEY_4 )
    {
        m_LightStreakShader.StreakLength += 6.0f * ElapsedTime;
        m_LightStreakShader.StreakLength = min( 6.0f, m_LightStreakShader.StreakLength );
    }

    // update light position
    m_LightPos = FRMVECTOR3( 3.3f, 2.0f, -0.42f );

    // animate the camera
    static FLOAT32 TotalTime = 0.0f;
    if( m_ShouldRotate )
    {
        static FLOAT32 CameraSpeed = 0.5f;
        TotalTime += ElapsedTime * CameraSpeed;
    }

    // build camera transforms
    FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 LookPos = GetCameraPos39( TotalTime + 0.05f );
    m_CameraPos   = GetCameraPos39( TotalTime );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

	m_AdaptLuminanceShader.ElapsedTime = 16.0f;
	m_AdaptLuminanceShader.Lambda = 0.001f;

    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: GaussianDistribution39()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 GaussianDistribution39( FLOAT32 X, FLOAT32 Y, FLOAT32 Rho )
{
    FLOAT32 Gauss = 1.0f / sqrtf( 2.0f * FRM_PI * Rho * Rho );
    Gauss *= expf( -( X * X + Y * Y ) / ( 2.0f * Rho * Rho ) );

    return Gauss;
}


//--------------------------------------------------------------------------------------
// Name: SetupGaussWeights()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample39::SetupGaussWeights()
{
    FLOAT32 GaussSum = 0.0f;
    for( int i = 0; i < 4; i++ )
    {
        // normalize to -1..1
        FLOAT32 X = ( FLOAT32( i ) / 3.0f );
        X = ( X - 0.5f ) * 2.0f;

        // spread is tunable
        X *= m_CombineShader.GaussSpread;

        m_BlurShader.GaussWeight[i] = GaussianDistribution39( X, 0.0f, m_CombineShader.GaussRho );

        GaussSum += m_BlurShader.GaussWeight[i];
    }

    m_BlurShader.GaussInvSum = ( 1.0f / GaussSum );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample39::DrawObject( SimpleObject39* Object )
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


VOID CSample39::PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle )
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
VOID CSample39::DrawScene()
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
VOID CSample39::GaussBlur( OffscreenRT4& InputTargetRT, OffscreenRT4& BlurredTargetRT )
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

        FRMVECTOR2 StepSizeY = FRMVECTOR2( 0.0f, (1.0f / m_ScratchRT.Height) * m_CombineShader.GaussSpread );
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
VOID CSample39::Generate64x64Lum( OffscreenRT4& TargetRT )
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
VOID CSample39::Generate16x16Lum( OffscreenRT4& TargetRT )
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
VOID CSample39::Generate4x4Lum( OffscreenRT4& TargetRT )
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
VOID CSample39::Generate1x1Lum( OffscreenRT4& TargetRT )
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
VOID CSample39::GenerateAdaptLum( OffscreenRT4& CurrLumRT, OffscreenRT4& PrevLumRT )
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
VOID CSample39::Copy1x1Lum( OffscreenRT4& CopyToLumRT, OffscreenRT4& CopyFromLumRT )
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
VOID CSample39::BrightPass( OffscreenRT4& CopyToLumRT, OffscreenRT4& ScratchRT, OffscreenRT4& currLumRT )
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
// Name: LightStreak()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample39::LightStreak( OffscreenRT4& OutputRT, OffscreenRT4& InputRT )
{
    OutputRT.SetFramebuffer();

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    FRMVECTOR2 StreakDir = FRMVECTOR2( FrmCos( m_LightStreakShader.StreakDir ), FrmSin( m_LightStreakShader.StreakDir ) );
    FRMVECTOR4 StreakWeights = FRMVECTOR4( 1.0f, 0.75f, 0.5f, 0.25f );

    glUseProgram( m_LightStreakShader.ShaderId );
    glUniform4fv( m_LightStreakShader.WeightsId, 1, StreakWeights.v );
    glUniform2fv( m_LightStreakShader.StreakDirId, 1, StreakDir.v );
    glUniform1f( m_LightStreakShader.StreakLengthId, m_LightStreakShader.StreakLength / 4.0f );
    glUniform2f( m_LightStreakShader.StepSizeId, (1.0f / InputRT.Width), (1.0f / InputRT.Height));
    SetTexture( m_LightStreakShader.ColorTextureId, InputRT.TextureHandle, 0 );

    RenderScreenAlignedQuad();
    
	OutputRT.DetachFramebuffer();                     
}


//--------------------------------------------------------------------------------------
// Name: CreateBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample39::CreateBlurredImage()
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


	// generate 64x64 luminance render target
	Generate64x64Lum( m_QuarterRT );

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

    // apply light streaks
    LightStreak( m_QuarterRT, m_BrightPassRT );
    LightStreak( m_ScratchRT, m_QuarterRT );
    LightStreak( m_QuarterRT, m_ScratchRT );

    // now streak again, 90 degrees off
    m_LightStreakShader.StreakDir += FRM_PI * 0.5f;
    LightStreak( m_BlurredRT, m_BrightPassRT );
    LightStreak( m_ScratchRT, m_BlurredRT );
    LightStreak( m_BlurredRT, m_ScratchRT );
    m_LightStreakShader.StreakDir -= FRM_PI * 0.5f;
}


//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample39::DrawCombinedScene()
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
    SetTexture( m_CombineShader.CurrLumTextureId, m_LastAverageLumRT.TextureHandle, 1 );
    SetTexture( m_CombineShader.StreakTexture1Id, m_QuarterRT.TextureHandle, 2 );
    SetTexture( m_CombineShader.StreakTexture2Id, m_BlurredRT.TextureHandle, 3 );
    glUniform1f(m_CombineShader.MiddleGrayId, m_CombineShader.MiddleGray );
    glUniform1f(m_CombineShader.WhiteId, m_CombineShader.White);

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample39::Render()
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
        FLOAT32 Width = 0.4f;
        FLOAT32 Height = 0.4f;

        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_BrightPassRT.TextureHandle );
        xPos += ( Width + 0.1f );
	}

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
