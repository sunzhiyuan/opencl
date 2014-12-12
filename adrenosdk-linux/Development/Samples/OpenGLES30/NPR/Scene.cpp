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


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "NPR" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_fRotateTime = 0.0f;

    m_hCelShadingShader    = 0;

    m_pLogoTexture         = NULL;
    m_pCelShadingTexture   = NULL;

    m_hCelModelViewProjMatrixLoc = 0;
    m_hCelNormalMatrixLoc        = 0;
    m_hCelLightPosLoc            = 0;
    m_hCelTextureLoc             = 0;
    m_hCelShadingLoc             = 0;
    m_hCelAmbientLoc             = 0;

    m_hNDModelViewProjMatrixLoc  = 0;
    m_hNDNormalMatrixLoc         = 0;

    m_hOutlineHalfStepXSizeLoc = 0;
    m_hOutlineHalfStepYSizeLoc = 0;
    m_fOutlineHalfStepSizeX    = 0.0f;
    m_fOutlineHalfStepSizeY    = 0.0f;

    m_fOutlineWidth  = 0.0f;

    m_hBlendShader      = 0;
    m_hBlendTexture1Loc = 0;
    m_hBlendTexture2Loc = 0;

    m_fAmbient = 0;

    m_vLightPosition.x  = 1.0f;
    m_vLightPosition.y  = 1.0f;
    m_vLightPosition.z  = 1.0f;

    m_fAmbient = 0.2f;

    m_hCelShadingShader     = NULL;
    m_hBlendShader          = NULL;
    m_pCelShadingTexture    = NULL;
    m_pLogoTexture 	        = NULL;
    m_pRayGunTexture        = NULL;

    m_pBufferIds[CEL_SHADED_FBO]     = NULL;
    m_pTextureIds[CEL_SHADED_FBO]    = NULL;
    
    m_pBufferIds[NORMAL_AND_DEPTH_FBO]     = NULL;
    m_pTextureIds[NORMAL_AND_DEPTH_FBO]    = NULL;
    
    m_pBufferIds[OUTLINE_FBO]     = NULL;
    m_pTextureIds[OUTLINE_FBO]    = NULL;
    
    m_hDepthbufferId = NULL;
    
    m_hFrameBufferNameId = NULL;
    
	m_OpenGLESVersion				     = GLES3;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the cel-shading shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex",   FRM_VERTEX_POSITION },
            { "g_vNormal", FRM_VERTEX_NORMAL },
            { "g_vTexCoord", FRM_VERTEX_TEXCOORD0 }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CelShading30.vs",
                                                      "Samples/Shaders/CelShading30.fs",
                                                      &m_hCelShadingShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hCelNormalMatrixLoc        = glGetUniformLocation( m_hCelShadingShader, "g_matNormal" );
        m_hCelModelViewProjMatrixLoc = glGetUniformLocation( m_hCelShadingShader, "g_matModelViewProj" );
        m_hCelLightPosLoc            = glGetUniformLocation( m_hCelShadingShader, "g_vLightPos" );
        m_hCelTextureLoc             = glGetUniformLocation( m_hCelShadingShader, "g_sTexture" );
        m_hCelShadingLoc             = glGetUniformLocation( m_hCelShadingShader, "g_sCelShadingTexture" );
        m_hCelAmbientLoc             = glGetUniformLocation( m_hCelShadingShader, "g_vAmbient" );
    }

    // Create our Blend FBO shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Blend30.vs",
                                                      "Samples/Shaders/BlendMultiplicative30.fs",
                                                      &m_hBlendShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hBlendTexture1Loc			= glGetUniformLocation( m_hBlendShader, "g_sImageTexture1" );
        m_hBlendTexture2Loc			= glGetUniformLocation( m_hBlendShader, "g_sImageTexture2" );
		m_hOutlineHalfStepXSizeLoc	= glGetUniformLocation( m_hBlendShader, "g_fHalfStepSizeX" );
		m_hOutlineHalfStepYSizeLoc	= glGetUniformLocation( m_hBlendShader, "g_fHalfStepSizeY" );
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


BOOL CSample::FreeDrawBuffers()
{
	if( m_pBufferIds[ CEL_SHADED_FBO ] )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferIds[ CEL_SHADED_FBO ] );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	
		glDeleteFramebuffers( 1, &m_pBufferIds[ CEL_SHADED_FBO ] );
	}	
	
	if( m_pTextureIds[ CEL_SHADED_FBO ] ) glDeleteTextures( 1, &m_pTextureIds[ CEL_SHADED_FBO ] );
	
	if( m_pBufferIds[ NORMAL_AND_DEPTH_FBO ] )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferIds[ NORMAL_AND_DEPTH_FBO ] );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0 );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		glDeleteFramebuffers( 1, &m_pBufferIds[ NORMAL_AND_DEPTH_FBO ] );
	}	

	if( m_pTextureIds[ NORMAL_AND_DEPTH_FBO ] ) glDeleteTextures( 1, &m_pTextureIds[ NORMAL_AND_DEPTH_FBO ] );

	if( m_hDepthbufferId)
	{
		glBindFramebuffer( GL_FRAMEBUFFER, m_hDepthbufferId );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		glDeleteFramebuffers( 1, &m_hDepthbufferId);
	}	

	if( m_hDepthbufferId) glDeleteTextures( 1, &m_hDepthbufferId);

	
	return TRUE;
}

BOOL CSample::CreateDrawBuffers()
{
	FreeDrawBuffers();
	
	glGenFramebuffers(1, &m_hFrameBufferNameId);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hFrameBufferNameId);
	
	glGenTextures( 1, &( m_pTextureIds[ CEL_SHADED_FBO ] ) );
	glBindTexture( GL_TEXTURE_2D, m_pTextureIds[ CEL_SHADED_FBO ] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_nFBOTextureWidth, m_nFBOTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTextureIds[ CEL_SHADED_FBO ], 0 );
	
	glGenTextures( 1, &( m_pTextureIds[ NORMAL_AND_DEPTH_FBO ] ) );
	glBindTexture( GL_TEXTURE_2D, m_pTextureIds[ NORMAL_AND_DEPTH_FBO ] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_nFBOTextureWidth, m_nFBOTextureHeight,0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_pTextureIds[ NORMAL_AND_DEPTH_FBO ], 0 );

	glGenTextures( 1, &m_hDepthbufferId);
	glBindTexture( GL_TEXTURE_2D, m_hDepthbufferId);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_nFBOTextureWidth, m_nFBOTextureHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL );

	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_hDepthbufferId, 0);
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_hDepthbufferId, 0);

	if( FALSE == CheckFrameBufferStatus() )
		return FALSE;
	

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    m_bShouldRotate = TRUE;

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/NPR30.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Create the cel shading texture
    m_pCelShadingTexture = resource.GetTexture( "Cel-Shading" );

    // Create the ray gun texture
    m_pRayGunTexture = resource.GetTexture( "RayGun" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_fOutlineWidth , "Outline Width", "%3.2f" );
    m_UserInterface.AddFloatVariable( &m_vLightPosition.x, "Light Pos", "%4.2f" );
    m_UserInterface.AddFloatVariable( &m_fAmbient, "Ambient Light", "%4.2f" );
    m_UserInterface.AddBoolVariable( &m_bShouldRotate, "Should Rotate" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Decrease Outline Width" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Increase Outline Width" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Decrease Ambient Light" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Increase Ambient Light" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Toggle Rotation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_DPAD, "Move Light" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Samples/Meshes/RayGun.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
    glClearColor( 0.5f, 0.5f, 0.5f, 0.0f );
    glEnable( GL_DEPTH_TEST );

    Resize();

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
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

    // Setup our depth buffer that will be shared by our various offscreen buffers
    m_nFBOTextureWidth  = m_nWidth;
    m_nFBOTextureHeight = m_nHeight;

    m_fOutlineWidth = 0.3f;
    m_fOutlineHalfStepSizeX = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureWidth ) / 2.0f;
    m_fOutlineHalfStepSizeY = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureHeight ) / 2.0f;

    // Setup our FBOs
    CreateDrawBuffers();

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Free shader memory
    if( m_hCelShadingShader )    glDeleteProgram( m_hCelShadingShader );
    if( m_hBlendShader )         glDeleteProgram( m_hBlendShader );

    // Free texture memory
    if( m_pCelShadingTexture ) m_pCelShadingTexture->Release();
    if( m_pLogoTexture )	   m_pLogoTexture->Release();
    if( m_pRayGunTexture )     m_pRayGunTexture->Release();

    // Free mesh memory
    m_Mesh.Destroy();
    
	FreeDrawBuffers();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
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

    if( nButtons & INPUT_KEY_1 )
    {
        m_fOutlineWidth = FRM_MAX( m_fOutlineWidth - 0.1f * fElapsedTime, 0.0f );
        m_fOutlineHalfStepSizeX = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureWidth  ) / 2.0f;
        m_fOutlineHalfStepSizeY = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureHeight ) / 2.0f;
    }

    if( nButtons & INPUT_KEY_2 )
    {
        m_fOutlineWidth = FRM_MIN( m_fOutlineWidth + 0.1f * fElapsedTime, 0.30f );
        m_fOutlineHalfStepSizeX = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureWidth  ) / 2.0f;
        m_fOutlineHalfStepSizeY = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureHeight ) / 2.0f;
    }

    if( nButtons & INPUT_KEY_3 )
    {
        if( m_fAmbient > 0.01f )
            m_fAmbient -= 0.2f * fElapsedTime;
    }

    if( nButtons & INPUT_KEY_4 )
    {
        if( m_fAmbient < 0.99f )
            m_fAmbient += 0.2f * fElapsedTime;
    }

    if( nPressedButtons & INPUT_KEY_5 )
    {
        m_bShouldRotate = 1 - m_bShouldRotate;
    }

    if( nButtons & INPUT_DPAD_LEFT )
    {
        // Move the light to the left
        m_vLightPosition.x -= 4.0f * fElapsedTime;
        if( m_vLightPosition.x < -5.0f )
            m_vLightPosition.x = -5.0f;
    }

    if( nButtons & INPUT_DPAD_RIGHT )
    {
        // Move the light to the right
        m_vLightPosition.x += 4.0f * fElapsedTime;
        if( m_vLightPosition.x > 5.0f )
            m_vLightPosition.x = 5.0f;
    }

    if( nButtons & INPUT_DPAD_DOWN )
    {
        // Move the light to the left
        m_vLightPosition.y -= 4.0f * fElapsedTime;
        if( m_vLightPosition.y < -5.0f )
            m_vLightPosition.y = -5.0f;
    }

    if( nButtons & INPUT_DPAD_UP )
    {
        // Move the light to the right
        m_vLightPosition.y += 4.0f * fElapsedTime;
        if( m_vLightPosition.y > 5.0f )
            m_vLightPosition.y = 5.0f;
    }

    // Rotate the object
    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.30f, 0.5f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.10f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );
    FRMMATRIX4X4 matModel = FrmMatrixRotate(m_fRotateTime, vUp);

    // Build the matrices
    m_matModelView     = FrmMatrixMultiply(matModel,       m_matView);
    m_matModelViewProj = FrmMatrixMultiply(m_matModelView, m_matProj);

    m_matNormalMatrix = FrmMatrixNormal(m_matModelView);
}



//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad()
{
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    const FLOAT32 Quad[] =
    {
        -1.0, -1.0f, 0.0f, 1.0f,
        +1.0, -1.0f, 1.0f, 1.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
        +1.0, +1.0f, 1.0f, 0.0f,
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

    glDisable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderSceneCelShadedDepthAndNormalsToFBO()
{
    // set the DrawBuffers framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_hFrameBufferNameId);

    glViewport( 0, 0, m_nWidth, m_nHeight );

    GLenum DrawBuffers[2];
    DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    DrawBuffers[1] = GL_COLOR_ATTACHMENT1;

    glDrawBuffers(2, DrawBuffers);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( m_hCelShadingShader );

    glActiveTexture( GL_TEXTURE0 );
    glUniform1i( m_hCelShadingLoc, 0 );
    glBindTexture( GL_TEXTURE_2D, m_pCelShadingTexture->m_hTextureHandle );

    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( m_hCelTextureLoc, 1 );
    glBindTexture( GL_TEXTURE_2D, m_pRayGunTexture->m_hTextureHandle );

    glUniformMatrix3fv( m_hCelNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matNormalMatrix );
    glUniformMatrix4fv( m_hCelModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniform3fv( m_hCelLightPosLoc, 1, (FLOAT32*)&m_vLightPosition );
    glUniform3f( m_hCelAmbientLoc, m_fAmbient, m_fAmbient, m_fAmbient );

    m_Mesh.Render();

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}



//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------

VOID CSample::RenderOutline()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glUseProgram( m_hBlendShader );
    
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureIds[ CEL_SHADED_FBO ] );
    glUniform1i( m_hBlendTexture1Loc, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureIds[ NORMAL_AND_DEPTH_FBO ] );
    glUniform1i( m_hBlendTexture2Loc, 1 );
    
	glUniform1f( m_hOutlineHalfStepXSizeLoc, m_fOutlineHalfStepSizeX * 4.0f);
	glUniform1f( m_hOutlineHalfStepYSizeLoc, m_fOutlineHalfStepSizeY * 4.0f);

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Clear the backbuffer and depth-buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    RenderSceneCelShadedDepthAndNormalsToFBO();
    RenderOutline();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

