//--------------------------------------------------------------------------------------
// Author:                 QUALCOMM, Adreno SDK
// Desc: Port of ShaderX7 Cascaded Shadow Maps
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#include <FrmPlatform.h>

#include <OpenGLES/FrmGLES3.h>

#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <OpenGLES/FrmUtilsGLES.h>
#include "Scene.h"



//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Cascaded Shadow Maps" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
	m_nCurShaderIndex					 = HARD_ARRAY;

    m_hOverlayShader                     = 0;

    m_hDepthShader                       = 0;
    m_hDepthMVPLoc                       = 0;

    m_pLogoTexture                       = NULL;
    m_nCurMeshIndex                      = 0;

	m_Preview						     = false;

	m_OpenGLESVersion				     = GLES3;
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
        TopLeft.x, BottomRight.y,		0.0f, 0.0f,
        BottomRight.x, BottomRight.y,	1.0f, 0.0f,
        BottomRight.x, TopLeft.y,		1.0f, 1.0f,
		TopLeft.x, TopLeft.y,			0.0f, 1.0f,

		TopLeft.x, BottomRight.y,		0.0f, 0.0f,
        
    };

    glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, Quad );
    glEnableVertexAttribArray( 0 );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 5 );
    glDisableVertexAttribArray( 0 );
    glEnable( GL_DEPTH_TEST );
}
VOID CSample::DrawGround()
{
	static float dim = 100.f;
	const FLOAT32 Quad[] =
    {
        dim, -2.0f, -dim, 0.f,1.f,0.f,
		-dim, -2.0f, -dim, 0.f,1.f,0.f,
		-dim, -2.0f, dim, 0.f,1.f,0.f,

		-dim, -2.0f, dim, 0.f,1.f,0.f,
		dim, -2.0f, dim, 0.f,1.f,0.f,
		dim, -2.0f, -dim, 0.f,1.f,0.f,
        
    };

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 36, Quad, GL_DYNAMIC_DRAW);

    glVertexAttribPointer( 0, 3, GL_FLOAT, false, 24, 0 );
    glEnableVertexAttribArray( 0 );

	glVertexAttribPointer( 3, 3, GL_FLOAT, false, 24, Quad  + 3);
    glEnableVertexAttribArray( 3 );

	glDrawArrays( GL_TRIANGLES, 0, 6 );

	glDisableVertexAttribArray( 3 );
    glDisableVertexAttribArray( 0 );
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
// Name: CreateShadowMapFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CreateShadowMapFBO()
{
    //FreeShadowMapFBO();
	m_DebugRTs[0] = new FrameBufferObject;
	m_DebugRTs[0]->m_nHeight = sizeShadow*2;
	m_DebugRTs[0]->m_nWidth = sizeShadow*2;

    // Create shadow texture
    {   
		glGenTextures( 1, &m_DebugRTs[0]->m_hTexture );
        glBindTexture( GL_TEXTURE_2D, m_DebugRTs[0]->m_hTexture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
        
        glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, sizeShadow*2, sizeShadow*2, 
                      0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
    }

	glGenFramebuffers( 1, &m_DebugRTs[0]->m_hFrameBuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, m_DebugRTs[0]->m_hFrameBuffer );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DebugRTs[0]->m_hTexture, 0 );

	//ATI cards need you to specify no color attached
	GLenum noColorAttach = GL_NONE;
	glDrawBuffers( 1, &noColorAttach );


    glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_DebugRTs[0]->m_hTexture );
 
    CheckFrameBufferStatus();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


VOID CSample::CreateShadowMapFBOArray()
{
	m_DebugRTs[1] = new FrameBufferObject;
	m_DebugRTs[1]->m_nHeight = sizeShadow;
	m_DebugRTs[1]->m_nWidth = sizeShadow;

	glGenTextures( 4, &m_DebugRTs[1]->m_hTexture );
	glBindTexture( GL_TEXTURE_2D_ARRAY, m_DebugRTs[1]->m_hTexture );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    
    // Use glTexImage3D to initialize the 2D Texture Array
    glTexImage3D( GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, m_DebugRTs[1]->m_nWidth, m_DebugRTs[1]->m_nHeight,
                   4, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );

	// When we render we will bind each of the textures via glFramebufferTextureLayer() to this one buffer
	glGenFramebuffers( 1, &m_DebugRTs[1]->m_hFrameBuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, m_DebugRTs[1]->m_hFrameBuffer );
	glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DebugRTs[1]->m_hTexture, 0, 0);
	//glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DebugRTs[1]->m_hTexture, 0 );

	//ATI cards need you to specify no color attached
	GLenum noColorAttach = GL_NONE;
	glDrawBuffers( 1, &noColorAttach );

	CheckFrameBufferStatus();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
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
// Name: BeginFBO()
// Desc: Bind objects for rendering to the frame buffer object
//--------------------------------------------------------------------------------------
VOID CSample::BeginFBO( FrameBufferObject* pFBO, INT32 layer )
{
    glBindFramebuffer( GL_FRAMEBUFFER, pFBO->m_hFrameBuffer );
	glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pFBO->m_hTexture, 0, layer);
    glViewport( 0, 0, pFBO->m_nWidth, pFBO->m_nHeight );
}
//--------------------------------------------------------------------------------------
// Name: EndFBO()
// Desc: Restore rendering back to the primary frame buffer
//--------------------------------------------------------------------------------------
VOID CSample::EndFBO( FrameBufferObject* pFBO )
{
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( 0, 0, m_nWidth, m_nHeight );
}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------

BOOL CSample::InitShaders()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "g_vPositionOS", FRM_VERTEX_POSITION },
        { "g_vNormalOS",   FRM_VERTEX_NORMAL }
    };
    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

	if( FALSE == m_ShadowMan.initShaders())
	{
        return FALSE;
	}

	// Create the Texture shader
    {
        
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vVertex", FRM_VERTEX_POSITION }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Overlay30.vs",
                                                      "Samples/Shaders/Overlay30.fs",
                                                      &m_hOverlayShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/OverlayArrayFirstElement.vs",
                                                      "Samples/Shaders/OverlayArrayFirstElement.fs",
                                                      &m_hOverlayArrayShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
		
        m_hOverlayScreenSizeLoc = glGetUniformLocation( m_hOverlayShader, "g_vScreenSize" );
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
	{
		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Depth30.vs",
                                                      "Samples/Shaders/Depth30.fs",
                                                      &m_hDepthShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_hDepthMVPLoc = glGetUniformLocation( m_hDepthShader, "g_matModelViewProj" );
	}

	CreateShadowMapFBO();
	CreateShadowMapFBOArray();

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Initialize sample variables
    m_nCurMeshIndex  = CUBE_MESH;

    m_vSunDirection = FRMVECTOR3( 0.0f, 4.0f, 0.0f );

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/CascadedShadowMaps.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;

	//hack in a title for first model
	m_UserInterface.SetHeading("Hard Edges, using Texture 2D Array");

    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Model" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Rotate Sun CW about Z axis" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Rotate Sun CCW about Z axis"  );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Rotate Sun CW about X axis" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Rotate Sun CCW about X axis" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, "Change Shadow Filter" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, "Toggle Depth Buffer" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, "Change Cascade Split Method" );

     // Load the meshes
    if( FALSE == m_Meshes[ CUBE_MESH ].Load( "Samples/Meshes/Cube.mesh" ) )
        return FALSE;
	if( FALSE == m_Meshes[ CUBE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].Load( "Samples/Meshes/Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].Load( "Samples/Meshes/BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].Load( "Samples/Meshes/Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].Load( "Samples/Meshes/Rocket.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Initialize the camera and light perspective matrices
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 250.0f );

	// To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matCameraPersp.M(0,0) *= fAspect;
        m_matCameraPersp.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Setup the camera view matrix
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 0.0f, 10.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the light look at and up vectors
    m_vLightLookAt = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vLightUp     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );

    // Setup GL state
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 400.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matCameraPersp.M(0,0) *= fAspect;
        m_matCameraPersp.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Delete shader programs
    
    if( m_hOverlayShader ) glDeleteProgram( m_hOverlayShader );

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    // Free mesh
    FreeMeshes();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fTime        = m_Timer.GetFrameTime();
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

	mover.Update( m_Input, fTime);

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
    {
        m_UserInterface.AdvanceState();
    }

    // Move shadow left
    if( nButtons & INPUT_KEY_2 )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vSunDirection, matRotate );
        m_vSunDirection = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow right
    if( nButtons & INPUT_KEY_3 )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vSunDirection, matRotate );
        m_vSunDirection = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow up
    if( nButtons & INPUT_KEY_4 )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vSunDirection, matRotate );
        m_vSunDirection = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow down
    if( nButtons & INPUT_KEY_5 )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vSunDirection, matRotate );
        m_vSunDirection = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

	m_vSunDirection = FrmVector3Normalize( m_vSunDirection );

    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nCurMeshIndex = ( m_nCurMeshIndex + 1 ) % NUM_MESHES;
    }

	if( nPressedButtons & INPUT_KEY_6 )
    {
        m_nCurShaderIndex = ( m_nCurShaderIndex + 1 ) % NUM_SHADERS;
		m_ShadowSettings.setTechnique( Settings::TECHNIQUE_ATLAS );

		switch(m_nCurShaderIndex)
		{
		case HARD:
			m_UserInterface.SetHeading("Hard Edges");
		break;
		case PCF_SHOW_SPLITS:
			m_UserInterface.SetHeading("Show Cascade Splits with PCF");
		break;
		case PCF:
			m_UserInterface.SetHeading("PCF");
		break;
		case HARD_ARRAY:
			m_UserInterface.SetHeading("Hard Edges, using Texture 2D Array");
			m_ShadowSettings.setTechnique( Settings::TECHNIQUE_ARRAY );
		break;
		case ESM_OPTIMIZED:
			m_UserInterface.SetHeading("Exponential Shadow Maps using Optimized CSM");
		break;

		}
    }

	if( nPressedButtons & INPUT_KEY_7 )
    {
		m_Preview = !m_Preview;
	}

	if( nPressedButtons & INPUT_KEY_8 )
    {
		m_ShadowSettings.setFlickering( (m_ShadowSettings.getFlickering() + 1)% Settings::NUM_FLICKERING_MODES);
	}

	m_matCameraView = mover.GetInverseWorldMatrix();

    m_matCameraMeshModelView      = m_matCameraView;
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

	// Update shadow manager
	m_ShadowMan.update( &m_ShadowSettings, m_matCameraPersp, m_matCameraView, (float *)& m_vSunDirection );

	//update all the individual model matrices..
	for(int i = 0; i < MODEL_DIMENSION * MODEL_DIMENSION; ++i)
	{
		static float grid_spacing = 5.0f;

		float transX = grid_spacing * ((i % MODEL_DIMENSION) - (MODEL_DIMENSION/2.0f));
		float transZ = grid_spacing * ((i / MODEL_DIMENSION) - (MODEL_DIMENSION/2.0f));

		FRMVECTOR3 yAxis = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
		FRMVECTOR3 xAxis = FRMVECTOR3( 1.0f, 0.0f, 0.0f );
		FRMMATRIX4X4 matMeshRotate    = FrmMatrixRotate( fTime *(-1.f*(i%2)), yAxis );
		FRMMATRIX4X4 matMeshRotate2   = FrmMatrixRotate( fTime *(-1.f*(i%3)), xAxis );
		FRMMATRIX4X4 matMeshModel     = FrmMatrixTranslate( transX, 0.0f, transZ );
		matMeshModel                  = FrmMatrixMultiply( matMeshRotate, matMeshModel );
		m_ModelMtxs[i]                = FrmMatrixMultiply( matMeshRotate2, matMeshModel );
	}
}


//--------------------------------------------------------------------------------------
// Name: CheckFrameBufferStatus()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CheckFrameBufferStatus()
{
    GLenum nStatus;
    nStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch( nStatus )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            break;
        default:
            FrmAssert(0);
    }
}

//--------------------------------------------------------------------------------------
// Name: FreeMeshes()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeMeshes()
{
    for( INT32 nMeshId = 0; nMeshId < NUM_MESHES; ++nMeshId )
    {
        m_Meshes[ nMeshId ].Destroy(); 
    }
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
	FrameBufferObject* curRT = m_DebugRTs[1];

	if(m_ShadowSettings.getTechnique() == Settings::TECHNIQUE_ATLAS) 
	{
		curRT = m_DebugRTs[0];
	}

	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glUseProgram( m_hDepthShader );
	{
		BeginFBO( curRT );

		glClear( GL_DEPTH_BUFFER_BIT );

		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		glClearDepthf( 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		FRMMATRIX4X4 matCameraMeshModelViewProj;

		for(int i=0; i<4; ++i)
		{
			if( m_ShadowSettings.getTechnique() == Settings::TECHNIQUE_ATLAS )
			{
				glViewport(sizeShadow * (i % 2),sizeShadow * (i / 2),sizeShadow,sizeShadow);
			}else
			{
				BeginFBO( curRT, i );
				glClear( GL_DEPTH_BUFFER_BIT );
			}
			
			for(int k = 0; k < MODEL_DIMENSION * MODEL_DIMENSION; ++k)
			{
				matCameraMeshModelViewProj = FrmMatrixMultiply( m_ModelMtxs[k], m_ShadowMan.getModelviews()[i] );
				matCameraMeshModelViewProj = FrmMatrixMultiply( matCameraMeshModelViewProj, m_ShadowMan.getProjections()[i] );

				glUniformMatrix4fv( m_hDepthMVPLoc, 1, FALSE, (FLOAT32*)&matCameraMeshModelViewProj );

				m_Meshes[ m_nCurMeshIndex ].Render();
			}

			matCameraMeshModelViewProj   = FrmMatrixMultiply( m_ShadowMan.getModelviews()[i], m_ShadowMan.getProjections()[i] );

			glUniformMatrix4fv( m_hDepthMVPLoc, 1, FALSE, (FLOAT32*)&matCameraMeshModelViewProj );

			DrawGround();
		}

		EndFBO( curRT );
	}

	glUseProgram( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hShaderId);
	{
		glUniform3f( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hLightLoc , m_vSunDirection.x, m_vSunDirection.y, m_vSunDirection.z );

		//set up depth map
		glActiveTexture( GL_TEXTURE0 );

		glBindTexture( GL_TEXTURE_2D, m_DebugRTs[0]->m_hTexture );

		glUniform1i( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hShadowMapLoc , 0 );

		if( m_nCurShaderIndex == ESM_OPTIMIZED)
		{
			glUniform4fv( m_ShadowMan.m_hShaders[ ESM_OPTIMIZED ].m_hShadowSpheresLoc, 4, (FLOAT32*)m_ShadowMan.getSpheres() );
		}

		for(int i = 0; i < MODEL_DIMENSION * MODEL_DIMENSION; ++i)
		{
			m_matCameraMeshModelView      = FrmMatrixMultiply( m_ModelMtxs[i], m_matCameraView );
			m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
			m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

			glUniformMatrix4fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hMVPLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelViewProj );

			FRMMATRIX4X4 worldMtx = m_ModelMtxs[i];

			glUniformMatrix4fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hMWorldLoc, 1, FALSE, (FLOAT32*)&worldMtx );

			glUniformMatrix3fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hMNormalLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshNormal );

			glUniform3fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hOffsetsLoc, 4, (FLOAT32*)m_ShadowMan.getOffsets() );

			glUniformMatrix4fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hLightMVPLoc, 4, FALSE, (FLOAT32*)m_ShadowMan.getModelviewProjections() );

			m_Meshes[ m_nCurMeshIndex ].Render();
		}
		
		m_matCameraMeshModelView      =  m_matCameraView;
		m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
		m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

		glUniformMatrix4fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hMVPLoc, 1, FALSE, (FLOAT32*)&m_matCameraMeshModelViewProj );

		FRMMATRIX4X4 identity = FrmMatrixIdentity();
		FRMMATRIX3X3 normMat = FrmMatrixNormalOrthonormal( identity );
		glUniformMatrix4fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hMWorldLoc, 1, FALSE, (FLOAT32*)&identity );
		glUniformMatrix3fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hMNormalLoc, 1, FALSE, (FLOAT32*)&normMat );
		glUniform3fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hOffsetsLoc, 4, (FLOAT32*)m_ShadowMan.getOffsets() );

		glUniformMatrix4fv( m_ShadowMan.m_hShaders[ m_nCurShaderIndex ].m_hLightMVPLoc, 4, FALSE, (FLOAT32*)m_ShadowMan.getModelviewProjections() );

		DrawGround();
	}

	// Show render target previews
	if( m_Preview )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		if(m_ShadowSettings.getTechnique() == Settings::TECHNIQUE_ATLAS) 
		{
			FrmRenderTextureToScreen_GLES( -5, 5, 164, 164, curRT->m_hTexture,
                                       m_hOverlayShader, m_hOverlayScreenSizeLoc );
		}
		else
		{
			RenderTextureArrayToScreen_GLES( -5, 5, 164, 164, curRT->m_hTexture,
                                       m_hOverlayArrayShader, m_hOverlayScreenSizeLoc );
		}
	}

	// Update the timer
    m_Timer.MarkFrame();
    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
	

}

