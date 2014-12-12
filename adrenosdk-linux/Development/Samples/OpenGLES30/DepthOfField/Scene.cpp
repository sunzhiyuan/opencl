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

#include <OpenGLES/FrmGLES3.h>  // OpenGL ES 3 headers here

#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include "Scene.h"

#define THERMALVISUALIZER

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
CombineShader::CombineShader()
{
	ShaderId = 0;

	FocalDistRange.x = 1.55f;
	FocalDistRange.y = 3.0f;
	GaussSpread = 3.0f;
	GaussRho = 1.0f;
#ifndef THERMALVISUALIZER	
	ThermalColors = FALSE;
#endif	
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
SimpleObject::SimpleObject()
{
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
    return new CSample( "DepthOfField" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_pLogoTexture = NULL;
    m_pTexture     = NULL;

	g_pSharpFBO			= NULL;
	g_pBlurFBO			= NULL;
	g_pScratchFBO		= NULL;
	
	m_ShouldRotate		= TRUE;

	m_OpenGLESVersion	= GLES3;
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

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CombineShader30.vs", "Samples/Shaders/CombineShader30.fs",
			&m_CombineShader.ShaderId, pShaderAttributes,
			nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
		m_CombineShader.SharpTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_SharpTexture" );
		m_CombineShader.BlurredTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_BlurredTexture" );
		m_CombineShader.DepthTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_DepthTexture" );
		m_CombineShader.NearQId = glGetUniformLocation( m_CombineShader.ShaderId, "g_NearQ" );
		m_CombineShader.FocalDistRangeId = glGetUniformLocation( m_CombineShader.ShaderId, "g_FocalDistRange" );
#ifndef THERMALVISUALIZER	
		m_CombineShader.ThermalColorsId = glGetUniformLocation( m_CombineShader.ShaderId, "g_ThermalColors" );
#endif		
	}
	
    return TRUE;
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
    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    if (FALSE == g_RoomResources.LoadFromFile("Samples/Textures/DepthOfField30.pak"))
        return FALSE;

    m_pLogoTexture = g_RoomResources.GetTexture("Logo"); 

	// Setup the user interface
	if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
		return FALSE;
	m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
		m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
	m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
	m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
	m_UserInterface.AddFloatVariable( &m_CombineShader.GaussSpread, (char *)"Gauss filter spread" );
	m_UserInterface.AddFloatVariable( &m_CombineShader.FocalDistRange.x, (char *)"Focal distance" );
	m_UserInterface.AddFloatVariable( &m_CombineShader.FocalDistRange.y, (char *)"Focal range" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
//	m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Toggle thermal visualizer" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Decrease focal distance" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Increase focal distance" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decrease focal range" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Increase focal range" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease gauss spread" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase gauss spread" );
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
        
	m_AmbientLight = FRMVECTOR4( 0.1f, 0.1f, 0.1f, 0.0f );
        
	// Set up the objects
	m_Object.Drawable = &m_RoomMesh;
	m_Object.Position.y += yAdjust;
    
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

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

	// Q = Zf / Zf - Zn
	m_CombineShader.NearFarQ.x = zNear;
	m_CombineShader.NearFarQ.y = zFar;
	m_CombineShader.NearFarQ.z = ( zFar / ( zFar - zNear ) );

	// To keep sizes consistent, scale the projection matrix in landscape oriention
	if( fAspect > 1.0f )
	{
		m_matProj.M(0,0) *= fAspect;
		m_matProj.M(1,1) *= fAspect;
	}

	// Initialize the viewport
	glViewport( 0, 0, m_nWidth, m_nHeight );

    // Setup our FBOs
    m_nOutOfFocusFBOWidth  = m_nWidth / 2;
    m_nOutOfFocusFBOHeight = m_nHeight / 2;

	if( FALSE == CreateFBO( m_nWidth, m_nHeight, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, &g_pSharpFBO ) )
		return FALSE;

	if( FALSE == CreateFBO( m_nOutOfFocusFBOWidth, m_nOutOfFocusFBOHeight, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, &g_pBlurFBO ) )
		return FALSE;

	if( FALSE == CreateFBO( m_nOutOfFocusFBOWidth, m_nOutOfFocusFBOHeight, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, &g_pScratchFBO ) )
		return FALSE;
		
	m_BlurShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_pTexture )     m_pTexture->Release();
    if( m_pLogoTexture ) m_pLogoTexture->Release();

	DestroyFBO( g_pSharpFBO );
	DestroyFBO( g_pBlurFBO );
	DestroyFBO( g_pScratchFBO );
	
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
}

//--------------------------------------------------------------------------------------
// Name: GetCameraPos27()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos( FLOAT32 fTime )
{
	FRMVECTOR3 BaseOffset = FRMVECTOR3( 4.5f, 2.0f, 0.0f );
	FRMVECTOR3 CurPosition = FRMVECTOR3( 1.7f * FrmCos( 0.25f * fTime ), 0.5f * FrmSin( 0.15f * fTime ), -1.9f * FrmSin( 0.25f * fTime ) );
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
#ifndef THERMALVISUALIZER	
	if( nPressedButtons & INPUT_KEY_1 )
	{
		m_CombineShader.ThermalColors = !m_CombineShader.ThermalColors;
	}
#endif

	if( nButtons & INPUT_KEY_2 )
	{
		// decrease focal distance
		FLOAT32 MIN_FOCAL_DIST = 0.5f; // actually 0.25, since FocalDist gets squared
		m_CombineShader.FocalDistRange.x -= 1.5f * ElapsedTime;
		m_CombineShader.FocalDistRange.x = max( MIN_FOCAL_DIST, m_CombineShader.FocalDistRange.x );
	}

	if( nButtons & INPUT_KEY_3 )
	{
		// increase focal distance
		FLOAT32 MAX_FOCAL_DIST = 3.5f; // actually 6.25, since FocalDist gets squared
		m_CombineShader.FocalDistRange.x += 1.5f * ElapsedTime;
		m_CombineShader.FocalDistRange.x = min( MAX_FOCAL_DIST, m_CombineShader.FocalDistRange.x );
	}

	if( nButtons & INPUT_KEY_4 )
	{
		// decrease range of clarity
		FLOAT32 MIN_FOCAL_RANGE = 0.4f;
		m_CombineShader.FocalDistRange.y -= 2.0f * ElapsedTime;
		m_CombineShader.FocalDistRange.y = max( MIN_FOCAL_RANGE, m_CombineShader.FocalDistRange.y );
	}

	if( nButtons & INPUT_KEY_5 )
	{
		// increase range of clarity
		FLOAT32 MAX_FOCAL_RANGE = 5.0f;
		m_CombineShader.FocalDistRange.y += 2.0f * ElapsedTime;
		m_CombineShader.FocalDistRange.y = min( MAX_FOCAL_RANGE, m_CombineShader.FocalDistRange.y );
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
	FRMVECTOR3 LookPos = GetCameraPos( TotalTime + 0.05f );
	m_CameraPos   = GetCameraPos( TotalTime );
	m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
	m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

	// update objects
	m_Object.Update( ElapsedTime, m_ShouldRotate );
}

//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::DrawScene()
{
	BeginFBO( g_pSharpFBO );

	glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
	glClearDepthf( 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );
	glFrontFace( GL_CW );

	DrawObject( &m_Object );

	EndFBO( g_pSharpFBO );
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
// Name: GaussBlur()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GaussBlur( FrameBufferObject* TargetRT )
{
	// Blur in the X direction
	{
		BeginFBO( g_pScratchFBO );

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
		SetTexture( m_BlurShader.ColorTextureId, TargetRT->m_hTexture, 0 );

		RenderScreenAlignedQuad();

		EndFBO( g_pScratchFBO );
	}

	// Then blur in the Y direction
	{
		BeginFBO( TargetRT );

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
		SetTexture( m_BlurShader.ColorTextureId, g_pScratchFBO->m_hTexture, 0 );

		RenderScreenAlignedQuad();

		EndFBO( TargetRT );
	}
}

//--------------------------------------------------------------------------------------
// Name: CreateBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CreateBlurredImage()
{
	// Part 1: downsize the framebuffer to a smaller render target
	{
		BeginFBO( g_pBlurFBO );

		// Render a full-screen quad
		glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDisable( GL_CULL_FACE );
		glDisable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );

		glUseProgram( m_DownsizeShader.ShaderId );
		SetTexture( m_DownsizeShader.ColorTextureId, g_pSharpFBO->m_hTexture, 0 );

		RenderScreenAlignedQuad();

		EndFBO( g_pBlurFBO );
	}

	// Part 2: blur
	GaussBlur( g_pBlurFBO );
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
	glClear( GL_COLOR_BUFFER_BIT );
	glDisable( GL_CULL_FACE );
	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );

	FRMVECTOR2 FocalDistRange;
	FocalDistRange.x = pow( m_CombineShader.FocalDistRange.x, 2.0f );
	FocalDistRange.y = ( 1.0f / max( 0.00001f, m_CombineShader.FocalDistRange.y ) );

	glUseProgram( m_CombineShader.ShaderId );
	SetTexture( m_CombineShader.SharpTextureId, g_pSharpFBO->m_hTexture, 0 );
	SetTexture( m_CombineShader.BlurredTextureId, g_pBlurFBO->m_hTexture, 1 );
	SetTexture( m_CombineShader.DepthTextureId, g_pSharpFBO->m_hDepthTexture, 2 );
	glUniform2f( m_CombineShader.NearQId, m_CombineShader.NearFarQ.x, m_CombineShader.NearFarQ.z );
	glUniform2fv( m_CombineShader.FocalDistRangeId, 1, FocalDistRange.v );
#ifndef THERMALVISUALIZER	
	glUniform1i( m_CombineShader.ThermalColorsId, m_CombineShader.ThermalColors );
#endif	

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

	// Clean up state
	//  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	glFrontFace( GL_CCW );

	// Update the timer
	m_Timer.MarkFrame();

	// Render the user interface
	m_UserInterface.Render( m_Timer.GetFrameRate() );
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


