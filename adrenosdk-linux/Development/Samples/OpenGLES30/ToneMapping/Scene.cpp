//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>

#include <OpenGLES/FrmGLES3.h>

#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include "Scene.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader::MaterialShader()
{
	ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader::Destroy()
{
	if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
DownsizeShader::DownsizeShader()
{
	ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID DownsizeShader::Destroy()
{
	if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID AdaptLuminanceShader::Destroy()
{
	if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
AdaptLuminanceShader::AdaptLuminanceShader()
{
	ShaderId = 0;
	//ElapsedTime = 1.0f;
	//Lambda = 0.1f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
LuminanceShader::LuminanceShader()
{
	ShaderId = 0;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID LuminanceShader::Destroy()
{
	if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BrightPassShader::BrightPassShader()
{
	ShaderId = 0;
	MiddleGray = 0.18f;
	BrightPassWhite = 1.0f;
	BrightPassThreshold = 0.7f;
	BrightPassOffset = 0.4f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BrightPassShader::Destroy()
{
	if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader::CombineShader()
{
	ShaderId = 0;

	GaussSpread = 1.0f;
	GaussRho = 1.0f;
	MiddleGray = 0.18f;
	White = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CombineShader::Destroy()
{
	if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BlurShader::BlurShader()
{
	ShaderId = 0;
	StepSize = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BlurShader::Destroy()
{
	if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PreviewShader::PreviewShader()
{
	ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PreviewShader::Destroy()
{
	if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject::SimpleObject()
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
VOID SimpleObject::Update( FLOAT32 ElapsedTime, BOOL ShouldRotate )
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
VOID SimpleObject::Destroy()
{
}

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Tone Mapping" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_pLogoTexture = NULL;
  //  m_pTexture     = NULL;

	m_SharpRT     = NULL;
	m_BlurredRT     = NULL;
	m_ScratchRT     = NULL;
	m_64x64RT     = NULL;
	m_16x16RT     = NULL;
	m_4x4RT     = NULL;
	m_1x1RT     = NULL;
	m_LastAverageLumRT     = NULL;
	m_PreviousAverageLumRT     = NULL;
	m_BrightPassRT     = NULL;
	m_QuarterRT     = NULL;
	
	m_ShouldRotate		= TRUE;

	m_OpenGLESVersion = GLES3;

}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/MaterialShader30.vs", "Samples/Shaders/MaterialShader30.fs",
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/DownsizeShader30.vs", "Samples/Shaders/DownsizeShader30.fs",
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/GenerateLuminance30.vs", "Samples/Shaders/GenerateLuminance30.fs",
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/AverageLuminance30.vs", "Samples/Shaders/AverageLuminance30.fs",
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/AdaptLuminance30.vs", "Samples/Shaders/AdaptLuminance30.fs",
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/BrightPass30.vs", "Samples/Shaders/BrightPass30.fs",
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Copy30.vs", "Samples/Shaders/Copy30.fs",
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/BlurShader30.vs", "Samples/Shaders/BlurShader30.fs",
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CombineShaderToneMapping30.vs", "Samples/Shaders/CombineShaderToneMapping30.fs",
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PreviewShader30.vs", "Samples/Shaders/PreviewShader30.fs",
			&m_PreviewShader.ShaderId, pShaderAttributes,
			nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
		m_PreviewShader.ColorTextureId = glGetUniformLocation( m_PreviewShader.ShaderId, "g_ColorTexture" );
	}
	
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CheckFrameBufferStatus()
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
// Name: CreateFBO()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateFBO( UINT32 nWidth, UINT32 nHeight, UINT32 nFormat, UINT32 nType,
						UINT32 nInternalFormat, FrameBufferObject** ppFBO )
{
	(*ppFBO) = new FrameBufferObject;
	(*ppFBO)->m_nWidth  = nWidth;
	(*ppFBO)->m_nHeight = nHeight;
	
	glGenFramebuffers( 1, &(*ppFBO)->m_hFrameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, (*ppFBO)->m_hFrameBuffer );

	// Create an offscreen texture
	glGenTextures( 1, &(*ppFBO)->m_hTexture );
	glBindTexture( GL_TEXTURE_2D, (*ppFBO)->m_hTexture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, nInternalFormat, nWidth, nHeight, 0, nFormat, nType, NULL );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (*ppFBO)->m_hTexture, 0 );
	
	glGenTextures( 1, &(*ppFBO)->m_hDepthTexture);
	glBindTexture( GL_TEXTURE_2D, (*ppFBO)->m_hDepthTexture);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, nWidth, nHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, (*ppFBO)->m_hDepthTexture, 0);
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, (*ppFBO)->m_hDepthTexture, 0);


	if( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus( GL_FRAMEBUFFER ) )
		return FALSE;

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: DestroyFBO()
// Desc: Destroy the FBO
//--------------------------------------------------------------------------------------
VOID CSample::DestroyFBO( FrameBufferObject* pFBO )
{
	glDeleteTextures( 1, &pFBO->m_hTexture );
	glDeleteFramebuffers( 1, &pFBO->m_hFrameBuffer );
	glDeleteTextures( 1, &pFBO->m_hDepthTexture );
	delete pFBO;
}


//--------------------------------------------------------------------------------------
// Name: BeginFBO()
// Desc: Bind objects for rendering to the frame buffer object
//--------------------------------------------------------------------------------------
VOID CSample::BeginFBO( FrameBufferObject* pFBO )
{
	glBindFramebuffer( GL_FRAMEBUFFER, pFBO->m_hFrameBuffer );
	glViewport( 0, 0, pFBO->m_nWidth, pFBO->m_nHeight );
}


//--------------------------------------------------------------------------------------
// Name: EndFBO()
// Desc: Restore rendering back to the primary frame buffer
//--------------------------------------------------------------------------------------
VOID CSample::EndFBO( FrameBufferObject* pFBO )
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( 0, 0, pFBO->m_nWidth, pFBO->m_nHeight );
}

//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad
// Desc: Draws a full-screen rectangle
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad()
{
	glDisable( GL_DEPTH_TEST );

	// There is currently a bug in Qualcomm's shader compiler in that it can't
	// negate a value.  Therefore, change the shader and the order of the 
	// quad here.
	const FLOAT32 Quad[] =
	{
		+1.0, +1.0f, 1.0f, 1.0f,
		-1.0, +1.0f, 0.0f, 1.0f,
		+1.0, -1.0f, 1.0f, 0.0f,
		-1.0, -1.0f, 0.0f, 0.0f,
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 16, Quad, GL_DYNAMIC_DRAW);

	glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, 0 );
	glEnableVertexAttribArray( 0 );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	glDisableVertexAttribArray( 0 );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
	m_ShouldRotate = TRUE;
	m_Preview = false;

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    if( FALSE == g_RoomResources.LoadFromFile( "Samples/Textures/ToneMapping.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = g_RoomResources.GetTexture( "Logo" );
    
	// Load the packed resources
	//if( FALSE == g_RoomResources.LoadFromFile( "Media/Room.pak" ) )
	//	return FALSE;

	// Setup the user interface
	if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
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

	if( FALSE == m_RoomMesh.Load( "Samples/Meshes/Map3.mesh" ) )
		return FALSE;

	m_CameraPos = FRMVECTOR3( 6.2f, 2.0f, 0.0f );
	float Split = 1.1f;
	float yAdjust = -0.05f;
 	m_Object.ModelScale = 1.0f;
		
	if( FALSE == m_RoomMesh.MakeDrawable( &g_RoomResources ) )
		return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
        
	m_AmbientLight = FRMVECTOR4( 0.02f, 0.02f, 0.02f, 0.02f );
        
	// Set up the objects
	m_Object.Drawable = &m_RoomMesh;
	m_Object.Position.y += yAdjust;
    

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
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
/*
	if( FALSE == CreateFBO( m_nWidth, m_nHeight, GL_RGBA, GL_FLOAT, GL_RGBA16F, &m_SharpRT ) )
		return FALSE;
	if( FALSE == CreateFBO( m_nWidth / 2, m_nHeight / 2, GL_RGBA, GL_FLOAT, GL_RGBA16F, &m_QuarterRT ) )
		return FALSE;
	if( FALSE == CreateFBO( m_nWidth / 2, m_nHeight / 2, GL_RGBA, GL_FLOAT, GL_RGBA16F, &m_BlurredRT ) )
		return FALSE;
	if( FALSE == CreateFBO( m_nWidth / 2, m_nHeight / 2, GL_RGBA, GL_FLOAT, GL_RGBA16F, &m_ScratchRT ) )
		return FALSE;
	if( FALSE == CreateFBO( 64, 64, GL_RED, GL_FLOAT, GL_R32F, &m_64x64RT ) )
		return FALSE;
	if( FALSE == CreateFBO( 16, 16, GL_RED, GL_FLOAT, GL_R32F, &m_16x16RT ) )
		return FALSE;
	if( FALSE == CreateFBO( 4, 4, GL_RED, GL_FLOAT, GL_R32F, &m_4x4RT ) )
		return FALSE;
	if( FALSE == CreateFBO( 1, 1, GL_RED, GL_FLOAT, GL_R32F, &m_1x1RT ) )
		return FALSE;
	if( FALSE == CreateFBO( 1, 1, GL_RED, GL_FLOAT, GL_R32F, &m_PreviousAverageLumRT ) )
		return FALSE;
	if( FALSE == CreateFBO( 1, 1, GL_RED, GL_FLOAT, GL_R32F, &m_LastAverageLumRT ) )
		return FALSE;
	if( FALSE == CreateFBO( m_nWidth / 2, m_nHeight / 2, GL_RGBA, GL_FLOAT, GL_RGBA16F, &m_BrightPassRT ) )
		return FALSE;
*/
		
	// Trying out sRGB
	if( FALSE == CreateFBO( m_nWidth, m_nHeight, GL_RGBA, GL_UNSIGNED_BYTE, GL_SRGB8_ALPHA8, &m_SharpRT ) )
		return FALSE;
	if( FALSE == CreateFBO( m_nWidth / 2, m_nHeight / 2, GL_RGBA, GL_UNSIGNED_BYTE, GL_SRGB8_ALPHA8, &m_QuarterRT ) )
		return FALSE;
	if( FALSE == CreateFBO( m_nWidth / 2, m_nHeight / 2, GL_RGBA, GL_UNSIGNED_BYTE, GL_SRGB8_ALPHA8, &m_BlurredRT ) )
		return FALSE;
	if( FALSE == CreateFBO( m_nWidth / 2, m_nHeight / 2, GL_RGBA, GL_UNSIGNED_BYTE, GL_SRGB8_ALPHA8, &m_ScratchRT ) )
		return FALSE;
	if( FALSE == CreateFBO( 64, 64, GL_RED, GL_FLOAT, GL_R32F, &m_64x64RT ) )
		return FALSE;
	if( FALSE == CreateFBO( 16, 16, GL_RED, GL_FLOAT, GL_R32F, &m_16x16RT ) )
		return FALSE;
	if( FALSE == CreateFBO( 4, 4, GL_RED, GL_FLOAT, GL_R32F, &m_4x4RT ) )
		return FALSE;
	if( FALSE == CreateFBO( 1, 1, GL_RED, GL_FLOAT, GL_R32F, &m_1x1RT ) )
		return FALSE;
	if( FALSE == CreateFBO( 1, 1, GL_RED, GL_FLOAT, GL_R32F, &m_PreviousAverageLumRT ) )
		return FALSE;
	if( FALSE == CreateFBO( 1, 1, GL_RED, GL_FLOAT, GL_R32F, &m_LastAverageLumRT ) )
		return FALSE;
	if( FALSE == CreateFBO( m_nWidth / 2, m_nHeight / 2, GL_RGBA, GL_UNSIGNED_BYTE, GL_SRGB8_ALPHA8, &m_BrightPassRT ) )
		return FALSE;

	m_BlurShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_BlurredRT->m_nWidth, 1.0f / (FLOAT32)m_BlurredRT->m_nHeight );

	BeginFBO(m_LastAverageLumRT);
	
	glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT);

	EndFBO(m_LastAverageLumRT);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
//    if( m_pTexture )     m_pTexture->Release();
    if( m_pLogoTexture ) m_pLogoTexture->Release();

	DestroyFBO( m_SharpRT);
	DestroyFBO( m_BlurredRT);
	DestroyFBO( m_ScratchRT);
	DestroyFBO(m_64x64RT);
	DestroyFBO(m_16x16RT);
	DestroyFBO(m_4x4RT);
	DestroyFBO(m_1x1RT);
	DestroyFBO(m_LastAverageLumRT);
	DestroyFBO(m_PreviousAverageLumRT);
	DestroyFBO(m_BrightPassRT);
	DestroyFBO(m_QuarterRT);
	
	// release resource
	m_RoomMesh.Destroy();
	g_Resource.Destroy();
	g_RoomResources.Destroy();
	
	// Free objects
	m_Object.Destroy();
	
	// Free shader memory
	m_MaterialShader.Destroy();
	m_CombineShader.Destroy();
	m_DownsizeShader.Destroy();
	m_BlurShader.Destroy();
	m_LuminanceShader.Destroy();
	m_AvgLuminanceShader.Destroy();
	m_CopyRTShader.Destroy();

	m_PreviewShader.Destroy();
}

//--------------------------------------------------------------------------------------
// Name: GetCameraPos27()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos( FLOAT32 fTime )
{
	FRMVECTOR3 BaseOffset = FRMVECTOR3( -0.5f, 2.0f, 0.0f );
	FRMVECTOR3 CurPosition = FRMVECTOR3( 3.0f * FrmCos( 0.5f * fTime ), 0.0f, 0.0f );
	return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
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
		m_BrightPassShader.BrightPassThreshold = min( 100.0f, m_BrightPassShader.BrightPassThreshold );
	}

	if( nButtons & INPUT_KEY_3 )
	{
		m_BrightPassShader.BrightPassOffset -= 2.0f * ElapsedTime;
		m_BrightPassShader.BrightPassOffset = max( 0.0f, m_BrightPassShader.BrightPassOffset );
	}

	if( nButtons & INPUT_KEY_4 )
	{
		m_BrightPassShader.BrightPassOffset += 2.0f * ElapsedTime;
		m_BrightPassShader.BrightPassOffset = min( 100.0f, m_BrightPassShader.BrightPassOffset );
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

	m_CameraPos   = GetCameraPos( TotalTime );
	LookPos       = m_CameraPos + FRMVECTOR3( 1.0f, 0.0f, 0.0f );
	m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
	m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

	m_AdaptLuminanceShader.ElapsedTime = ElapsedTime;
	m_AdaptLuminanceShader.Lambda = 17000.0f;

	// update objects
	m_Object.Update( ElapsedTime, m_ShouldRotate );
}

//--------------------------------------------------------------------------------------
// Name: GaussianDistribution()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 GaussianDistribution( FLOAT32 X, FLOAT32 Y, FLOAT32 Rho )
{
	FLOAT32 Gauss = 1.0f / sqrtf( 2.0f * FRM_PI * Rho * Rho );
	Gauss *= expf( -( X * X + Y * Y ) / ( 2.0f * Rho * Rho ) );

	return Gauss;
}

//--------------------------------------------------------------------------------------
// Name: SetupGaussWeights()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetupGaussWeights()
{
	FLOAT32 GaussSum = 0.0f;
	for( int i = 0; i < 4; i++ )
	{
		// normalize to -1..1
		FLOAT32 X = ( FLOAT32( i ) / 3.0f );
		X = ( X - 0.5f ) * 2.0f;

		// spread is tunable
		X *= m_CombineShader.GaussSpread;

		m_BlurShader.GaussWeight[i] = GaussianDistribution( X, 0.0f, m_CombineShader.GaussRho );

		GaussSum += m_BlurShader.GaussWeight[i];
	}

	m_BlurShader.GaussInvSum = ( 1.0f / GaussSum );
}

//--------------------------------------------------------------------------------------
// Name: SetTexture()
// Desc: Prepares a texture for a draw call
//--------------------------------------------------------------------------------------
VOID CSample::SetTexture( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot )
{
	glActiveTexture( GL_TEXTURE0 + SamplerSlot );
	glBindTexture( GL_TEXTURE_2D, TextureHandle );
	glUniform1i( ShaderVar, SamplerSlot );
}


//--------------------------------------------------------------------------------------
// Name: GaussBlur()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GaussBlur( FrameBufferObject* InputTargetRT, FrameBufferObject* BlurredTargetRT )
{
	// Blur in the X direction
	{
		BeginFBO( m_ScratchRT );

		glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDisable( GL_CULL_FACE );
		glDisable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );

		FRMVECTOR2 StepSizeX = FRMVECTOR2( m_BlurShader.StepSize.x * m_CombineShader.GaussSpread, 0.0f );
		glUseProgram( m_BlurShader.ShaderId );
		glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
		glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
		glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeX.v );
		SetTexture( m_BlurShader.ColorTextureId, InputTargetRT->m_hTexture, 0 );

		RenderScreenAlignedQuad();

		EndFBO( m_ScratchRT );
	}

	// Then blur in the Y direction
	{
		BeginFBO( BlurredTargetRT );

		glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDisable( GL_CULL_FACE );
		glDisable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );

		FRMVECTOR2 StepSizeY = FRMVECTOR2( 0.0f, m_BlurShader.StepSize.y * m_CombineShader.GaussSpread );
		glUseProgram( m_BlurShader.ShaderId );
		glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
		glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
		glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeY.v );
		SetTexture( m_BlurShader.ColorTextureId, m_ScratchRT->m_hTexture, 0 );

		RenderScreenAlignedQuad();

		EndFBO( BlurredTargetRT );
	}
}

//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::DrawObject( SimpleObject* Object )
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

//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad
// Desc: Draws a full-screen rectangle
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight )
{
    glDisable( GL_DEPTH_TEST );

    // There is currently a bug in Qualcomm's shader compiler in that it can't
    // negate a value.  Therefore, change the shader and the order of the 
    // quad here.
    const FLOAT32 Quad[] =
    {
/*
        BottomRight.x, TopLeft.y, 1.0f, 0.0f,
        TopLeft.x, TopLeft.y, 0.0f, 0.0f,
        BottomRight.x, BottomRight.y, 1.0f, 1.0f,
        TopLeft.x, BottomRight.y, 0.0f, 1.0f,
       
        -1.0, -1.0f, 0.0f, 1.0f,
        +1.0, -1.0f, 1.0f, 1.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
        +1.0, +1.0f, 1.0f, 0.0f,  
*/

        TopLeft.x, BottomRight.y,		0.0f, 1.0f,
        BottomRight.x, BottomRight.y,	1.0f, 1.0f,
        TopLeft.x, TopLeft.y,			0.0f, 0.0f,
        BottomRight.x, TopLeft.y,		1.0f, 0.0f,
        
    };

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 16, Quad, GL_DYNAMIC_DRAW);

	glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, 0 );

    glEnableVertexAttribArray( 0 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glDisableVertexAttribArray( 0 );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

    glEnable( GL_DEPTH_TEST );
}

VOID CSample::PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle )
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
VOID CSample::DrawScene()
{
	BeginFBO( m_SharpRT );

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepthf( 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );
	glFrontFace( GL_CW );

	DrawObject( &m_Object );

	EndFBO( m_SharpRT );
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Generate64x64Lum( FrameBufferObject* SourceRT )
{
	BeginFBO( m_64x64RT );

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT);
	glDisable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( m_LuminanceShader.ShaderId );
	SetTexture( m_LuminanceShader.ColorTextureId, SourceRT->m_hTexture, 0 );
	glUniform2f( m_LuminanceShader.StepSizeId, 1.0f / SourceRT->m_nWidth, 1.0f / SourceRT->m_nHeight);

	RenderScreenAlignedQuad();

	EndFBO( m_64x64RT );
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Generate16x16Lum( FrameBufferObject* SourceRT )
{
	BeginFBO( m_16x16RT );

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT);
	glDisable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( m_AvgLuminanceShader.ShaderId );
	SetTexture( m_AvgLuminanceShader.ColorTextureId, SourceRT->m_hTexture, 0 );
	glUniform2f( m_AvgLuminanceShader.StepSizeId, 1.0f / SourceRT->m_nWidth, 1.0f / SourceRT->m_nHeight);

	RenderScreenAlignedQuad();

	EndFBO( m_16x16RT );
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Generate4x4Lum( FrameBufferObject* SourceRT )
{
	BeginFBO( m_4x4RT );

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT);
	glDisable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( m_AvgLuminanceShader.ShaderId );
	SetTexture( m_AvgLuminanceShader.ColorTextureId, SourceRT->m_hTexture, 0 );
	glUniform2f( m_AvgLuminanceShader.StepSizeId, 1.0f / SourceRT->m_nWidth, 1.0f / SourceRT->m_nHeight);

	RenderScreenAlignedQuad();

	EndFBO( m_4x4RT );
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Generate1x1Lum( FrameBufferObject* SourceRT )
{
	BeginFBO( m_1x1RT );

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT);
	glDisable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( m_AvgLuminanceShader.ShaderId );
	SetTexture( m_AvgLuminanceShader.ColorTextureId, SourceRT->m_hTexture, 0 );
	glUniform2f(m_AvgLuminanceShader.StepSizeId, 1.0f / SourceRT->m_nWidth, 1.0f / SourceRT->m_nHeight);

	RenderScreenAlignedQuad();

	EndFBO( m_1x1RT );
}

//--------------------------------------------------------------------------------------
// Name: GenerateAdaptLum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GenerateAdaptLum( FrameBufferObject* CurrLumRT, FrameBufferObject* PrevLumRT )
{
	BeginFBO(m_LastAverageLumRT);

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT);
	glDisable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( m_AdaptLuminanceShader.ShaderId );
	SetTexture( m_AdaptLuminanceShader.CurrLumTextureId, CurrLumRT->m_hTexture, 0 );
	SetTexture( m_AdaptLuminanceShader.PrevLumTextureId, PrevLumRT->m_hTexture, 1 );
	glUniform1f( m_AdaptLuminanceShader.ElapsedTimeId, m_AdaptLuminanceShader.ElapsedTime );
	glUniform1f( m_AdaptLuminanceShader.LambdaId, m_AdaptLuminanceShader.Lambda);

	RenderScreenAlignedQuad();

	EndFBO(m_LastAverageLumRT);
}

//--------------------------------------------------------------------------------------
// Name: Copy1x1Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Copy1x1Lum( FrameBufferObject* CopyToLumRT, FrameBufferObject* CopyFromLumRT )
{
	BeginFBO(CopyToLumRT);

	glDisable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( m_CopyRTShader.ShaderId );
	SetTexture( m_CopyRTShader.ColorTextureId, CopyFromLumRT->m_hTexture, 0 );

	RenderScreenAlignedQuad();

	EndFBO(CopyToLumRT);
}

//--------------------------------------------------------------------------------------
// Name: BrightPass()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::BrightPass( FrameBufferObject* CopyToLumRT, FrameBufferObject* ScratchRT, FrameBufferObject* currLumRT )
{
	BeginFBO(CopyToLumRT);

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT);
	glDisable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( m_BrightPassShader.ShaderId );
	SetTexture( m_BrightPassShader.ScratchTextureId, ScratchRT->m_hTexture, 0 );
	SetTexture( m_BrightPassShader.CurrLumTextureId, currLumRT->m_hTexture, 1 );
	glUniform1f( m_BrightPassShader.MiddleGrayId, m_BrightPassShader.MiddleGray );
	glUniform1f( m_BrightPassShader.BrightPassWhiteId, m_BrightPassShader.BrightPassWhite );
	glUniform1f( m_BrightPassShader.BrightPassThresholdId, m_BrightPassShader.BrightPassThreshold );
	glUniform1f( m_BrightPassShader.BrightPassOffsetId, m_BrightPassShader.BrightPassOffset );

	RenderScreenAlignedQuad();

	EndFBO(CopyToLumRT);
}


//--------------------------------------------------------------------------------------
// Name: CreateBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CreateBlurredImage()
{
	// Part 1: downsize the framebuffer to a smaller render target
	{
		BeginFBO(m_QuarterRT);

		// Render a full-screen quad
		glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDisable( GL_CULL_FACE );
		glDisable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );

		glUseProgram( m_DownsizeShader.ShaderId );
		SetTexture( m_DownsizeShader.ColorTextureId, m_SharpRT->m_hTexture, 0 );
		glUniform2f( m_DownsizeShader.StepSizeId, 1.0f / m_SharpRT->m_nWidth, 1.0f / m_SharpRT->m_nHeight);

		RenderScreenAlignedQuad();

		EndFBO(m_QuarterRT);
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
VOID CSample::DrawCombinedScene()
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( 0, 0, m_nWidth, m_nHeight );

	glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	glUseProgram( m_CombineShader.ShaderId );
	SetTexture( m_CombineShader.SharpTextureId, m_SharpRT->m_hTexture, 0 );
	SetTexture( m_CombineShader.CurrLumTextureId, m_LastAverageLumRT->m_hTexture, 3 );
	SetTexture( m_CombineShader.BloomTextureId, m_BlurredRT->m_hTexture, 4 );
	glUniform1f(m_CombineShader.MiddleGrayId, m_CombineShader.MiddleGray );
	glUniform1f(m_CombineShader.WhiteId, m_CombineShader.White);

	RenderScreenAlignedQuad();
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
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

		PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_BrightPassRT->m_hTexture );
		xPos += ( Width + 0.1f );

		PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_64x64RT->m_hTexture );
		xPos += ( Width + 0.05f );
		PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_16x16RT->m_hTexture );
		xPos += ( Width + 0.05f );
		PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_4x4RT->m_hTexture );
		xPos += ( Width + 0.05f );
		PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_1x1RT->m_hTexture );
		xPos += ( Width + 0.1f );


		PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_LastAverageLumRT->m_hTexture );
		xPos += ( Width + 0.05f );
		PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_PreviousAverageLumRT->m_hTexture );
	}

	// Update the timer
	m_Timer.MarkFrame();

	// Render the user interface
	m_UserInterface.Render( m_Timer.GetFrameRate() );
}
