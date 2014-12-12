//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include "Scene.h"

#if defined(__linux__)
#include <stdio.h>
#include <unistd.h>
#endif



//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Bloom" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hConstantShader         =	NULL;
    m_hPerPixelLightingShader =	NULL;
    m_hGaussianShader         =	NULL;
    m_hBlendShader            =	NULL;
    m_hTextureShader          =	NULL;
    m_pLogoTexture			  =	NULL;

    m_pBufferId[MESH_FBO]       = NULL;
    m_pTextureId[MESH_FBO]      = NULL;
    m_hDepthbufferId[MESH_FBO]  = NULL;

    m_pBufferId[BLOOM_FBO]      = NULL;
    m_pTextureId[BLOOM_FBO]     = NULL;
    m_hDepthbufferId[BLOOM_FBO] = NULL;

    m_fRotateTime = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PerPixelLighting.vs",
                                                      "Samples/Shaders/PerPixelLighting.fs",
                                                      &m_hPerPixelLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hModelViewMatrixLoc     = glGetUniformLocation( m_hPerPixelLightingShader,  "g_matModelView" );
        m_hModelViewProjMatrixLoc = glGetUniformLocation( m_hPerPixelLightingShader,  "g_matModelViewProj" );
        m_hNormalMatrixLoc        = glGetUniformLocation( m_hPerPixelLightingShader,  "g_matNormal" );
        m_hLightPositionLoc       = glGetUniformLocation( m_hPerPixelLightingShader,  "g_vLightPos" );
        m_hMaterialAmbientLoc     = glGetUniformLocation( m_hPerPixelLightingShader,  "g_Material.vAmbient" );
        m_hMaterialDiffuseLoc     = glGetUniformLocation( m_hPerPixelLightingShader,  "g_Material.vDiffuse" );
        m_hMaterialSpecularLoc    = glGetUniformLocation( m_hPerPixelLightingShader,  "g_Material.vSpecular" );
    }

    // Create our Constant FBO shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex", FRM_VERTEX_POSITION },
            { "g_vNormal", FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/NormalDeformer.vs",
                                                      "Samples/Shaders/Constant.fs",
                                                      &m_hConstantShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hConstantModelViewProjMatrixLoc = glGetUniformLocation( m_hConstantShader, "g_mModelViewProjectionMatrix" );
        m_hConstantColorLoc               = glGetUniformLocation( m_hConstantShader, "g_vColor" );
        m_hConstantDeformSize             = glGetUniformLocation( m_hConstantShader, "g_fSize" );
    }

    // Create our Gaussian FBO shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Gaussian.vs",
                                                      "Samples/Shaders/Gaussian.fs",
                                                      &m_hGaussianShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hGaussianStepSize = glGetUniformLocation( m_hGaussianShader, "g_vStepSize" );
    }

    // Create our Blend FBO shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Blend.vs",
                                                      "Samples/Shaders/BlendAdditive.fs",
                                                      &m_hBlendShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hBlendTexture1Loc = glGetUniformLocation( m_hBlendShader, "g_sImageTexture1" );
        m_hBlendTexture2Loc = glGetUniformLocation( m_hBlendShader, "g_sImageTexture2" );
    }

    // Create our Texture FBO shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Texture.vs",
                                                      "Samples/Shaders/Texture.fs",
                                                      &m_hTextureShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;
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
BOOL CSample::CreateFBO( INT32 nFBOId, INT32 nWidth, INT32 nHeight )
{
    // Free the FBO, in case it was previously created
    FreeFBO( nFBOId );

    glGenRenderbuffers( 1, &m_hDepthbufferId[ nFBOId ] );
    glBindRenderbuffer( GL_RENDERBUFFER, m_hDepthbufferId[ nFBOId ] );    
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, nWidth, nHeight );

    glGenFramebuffers( 1, &m_pBufferId[ nFBOId ] );
    glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferId[ nFBOId ] );

    glGenTextures( 1, &( m_pTextureId[ nFBOId ] ) );
    glBindTexture( GL_TEXTURE_2D, m_pTextureId[ nFBOId ] );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight,
                  0, GL_RGB, GL_UNSIGNED_BYTE, NULL );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureId[ nFBOId ] );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTextureId[ nFBOId ], 0 );

    if( FALSE == CheckFrameBufferStatus() )
        return FALSE;

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeFBO( INT32 nFBOId )
{
	if( m_pBufferId[ nFBOId ] )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferId[ nFBOId ] );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	    glDeleteFramebuffers( 1, &m_pBufferId[ nFBOId ] );
	}
	if( m_pBufferId[ nFBOId ] )      glDeleteFramebuffers( 1, &m_pBufferId[ nFBOId ] );
	if( m_pTextureId[ nFBOId ] )     glDeleteTextures( 1, &m_pTextureId[ nFBOId ] );
	if( m_hDepthbufferId[ nFBOId ] ) glDeleteRenderbuffers( 1, &m_hDepthbufferId[ nFBOId ] );
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    m_bBloomOn      = TRUE;
    m_bShouldRotate = TRUE;
    m_fDeformSize   = 0.0030f;
    m_fBlurSize     = 1.0f;

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/Bloom.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddIntVariable( &m_nBloomFBOTextureWidth , "Bloom FBO Size", "%d" );
    m_UserInterface.AddBoolVariable( &m_bBloomOn , "Bloom On", "True", "False" );
    m_UserInterface.AddFloatVariable( &m_fDeformSize , "Normal Deform Size", "%3.4f" );
    m_UserInterface.AddFloatVariable( &m_fBlurSize , "Blur Size", "%3.4f" );
    m_UserInterface.AddBoolVariable( &m_bShouldRotate, "Should Rotate" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Toggle Bloom" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Decr Bloom FBO Size" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Incr Bloom FBO Size" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Decr Normal Deformation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Incr Normal Deformation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, "Decr Blur Size" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, "Incr Blur Size" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, "Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Samples/Meshes/Teapot.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
    glEnable( GL_DEPTH_TEST );

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

    // Setup our FBOs
    m_nBloomFBOTextureExp    = 6;
    m_nBloomFBOTextureWidth  = 2 << m_nBloomFBOTextureExp;
    m_nBloomFBOTextureHeight = 2 << m_nBloomFBOTextureExp;
    CreateFBO( BLOOM_FBO, m_nBloomFBOTextureWidth, m_nBloomFBOTextureHeight );
    CreateFBO( MESH_FBO, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Free shader memory
    if( m_hConstantShader )         glDeleteProgram( m_hConstantShader );
    if( m_hPerPixelLightingShader ) glDeleteProgram( m_hPerPixelLightingShader );
    if( m_hGaussianShader )         glDeleteProgram( m_hGaussianShader );
    if( m_hBlendShader )            glDeleteProgram( m_hBlendShader );
    if( m_hTextureShader )          glDeleteProgram( m_hTextureShader );

    // Free texture memory
    if( m_pLogoTexture )	        m_pLogoTexture->Release();

    // Free mesh memory
    m_Mesh.Destroy();

    // Free FBO info
    FreeFBO( MESH_FBO );
    FreeFBO( BLOOM_FBO );
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

    // Toggle Bloom
    if( nPressedButtons & INPUT_KEY_1 )
        m_bBloomOn = !m_bBloomOn;

    // Lower bloom FBO size
    if( nPressedButtons & INPUT_KEY_2 )
    {
        if( m_nBloomFBOTextureExp > 4 )
        {
            m_nBloomFBOTextureExp--;
            m_nBloomFBOTextureWidth  = 2 << m_nBloomFBOTextureExp;
            m_nBloomFBOTextureHeight = 2 << m_nBloomFBOTextureExp; 
            CreateFBO( BLOOM_FBO, m_nBloomFBOTextureWidth, m_nBloomFBOTextureHeight );
        }
    }

    // Increase bloom FBO size
    if( nPressedButtons & INPUT_KEY_3 )
    {
        if( m_nBloomFBOTextureExp < 7 )
        {
            m_nBloomFBOTextureExp++;
            m_nBloomFBOTextureWidth  = 2 << m_nBloomFBOTextureExp;
            m_nBloomFBOTextureHeight = 2 << m_nBloomFBOTextureExp; 
            CreateFBO( BLOOM_FBO, m_nBloomFBOTextureWidth, m_nBloomFBOTextureHeight );
        }
    }

    // Decrease normal deformation
    if( nButtons & INPUT_KEY_4 )
    {
        if( m_fDeformSize >= 0.0f )
        {
            m_fDeformSize -= 0.004f * fElapsedTime;
        }
    }

    // Increase normal deformation
    if( nButtons & INPUT_KEY_5 )
    {
        if( m_fDeformSize <= 0.0040f )
        {
            m_fDeformSize += 0.004f * fElapsedTime;
        }
    }

    // Decrease blur size
    if( nButtons & INPUT_KEY_6 )
    {
        if( m_fBlurSize >= 0.0f )
        {
            m_fBlurSize -= fElapsedTime;
        }
    }

    // Increase blur size
    if( nButtons & INPUT_KEY_7 )
    {
        if( m_fBlurSize <= 10.0f )
        {
            m_fBlurSize += fElapsedTime;
        }
    }

    // Toggle rotation
    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_bShouldRotate = 1 - m_bShouldRotate;
    }

    // Rotate the object
	FRMVECTOR3 vX(1,0,0);
	FRMVECTOR3 vY(0,1,0);
	FRMMATRIX4X4 matRotate1 = FrmMatrixRotate( m_fRotateTime, vY );
    FRMMATRIX4X4 matRotate2 = FrmMatrixRotate( 0.3f,  vX );

    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.20f, 0.5f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.07f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // Build the matrices
    FRMMATRIX4X4 matModel;
    matModel           = FrmMatrixMultiply( matRotate2,     matRotate1 );
    m_matModelView     = FrmMatrixMultiply( matModel,       m_matView );
    m_matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_matNormal        = FrmMatrixNormal( m_matModelView );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GetFBO( INT32 nFBOId, INT32 nWidth, INT32 nHeight )
{
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureId[ nFBOId ] );
    glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferId[ nFBOId ] );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTextureId[ nFBOId ], 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_hDepthbufferId[ nFBOId ] );
    glViewport( 0, 0, nWidth, nHeight );
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
        +1.0, +1.0f, 1.0f, 0.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
   };

    glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, Quad );
    glEnableVertexAttribArray( 0 );

    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );

    glDisableVertexAttribArray( 0 );

    glDisable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderBloomFBO()
{
	FRMVECTOR4 colorLocVec = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );

    GetFBO( BLOOM_FBO, m_nBloomFBOTextureWidth, m_nBloomFBOTextureHeight );

    glClearColor( 1.0f, 1.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    glDisable( GL_DEPTH_TEST );

    glUseProgram( m_hConstantShader );
    glUniformMatrix4fv( m_hConstantModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniform4fv( m_hConstantColorLoc, 1, (FLOAT32*) &colorLocVec );
    glUniform1f( m_hConstantDeformSize, -m_fDeformSize );

    m_Mesh.Render();

    glEnable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderMeshFBO()
{
    GetFBO( MESH_FBO, m_nWidth, m_nHeight );    

    glClearColor( 1.0f, 1.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( m_hPerPixelLightingShader );
    glUniform4f( m_hMaterialAmbientLoc, 0.2f, 0.2f, 0.2f, 1.0f );
    glUniform4f( m_hMaterialDiffuseLoc, 0.3f, 0.3f, 0.8f, 1.0f );
    glUniform4f( m_hMaterialSpecularLoc, 0.65f, 0.6f, 1.0f, 10.0f );
    glUniformMatrix4fv( m_hModelViewMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelView );
    glUniformMatrix4fv( m_hModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniformMatrix3fv( m_hNormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matNormal );
    glUniform3f( m_hLightPositionLoc, 0.0f, 1.0f, 1.0f );

    m_Mesh.Render();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderBlurredBloomFBO()
{
    GetFBO( BLOOM_FBO, m_nBloomFBOTextureWidth, m_nBloomFBOTextureHeight );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureId[ BLOOM_FBO ] );

    glUseProgram( m_hGaussianShader );
    glUniform2f( m_hGaussianStepSize, m_fBlurSize / m_nBloomFBOTextureWidth, 0.0f );

    RenderScreenAlignedQuad();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    GetFBO( BLOOM_FBO, m_nBloomFBOTextureWidth, m_nBloomFBOTextureHeight );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureId[ BLOOM_FBO ] );

    glUniform2f( m_hGaussianStepSize, 0.0f, m_fBlurSize / m_nBloomFBOTextureWidth );

    RenderScreenAlignedQuad();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderCompositedBloom()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    if( m_bBloomOn )
    {
        glUseProgram( m_hBlendShader );

        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, m_pTextureId[ MESH_FBO ] );
        glUniform1i( m_hBlendTexture1Loc, 0 );

        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, m_pTextureId[ BLOOM_FBO ] );
        glUniform1i( m_hBlendTexture2Loc, 1 );
    }
    else
    {
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, m_pTextureId[ MESH_FBO ] );

        glUseProgram( m_hTextureShader );
    }

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    RenderMeshFBO();

    if( m_bBloomOn == TRUE )
    {
        RenderBloomFBO();
        RenderBlurredBloomFBO();
    }

    RenderCompositedBloom();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

