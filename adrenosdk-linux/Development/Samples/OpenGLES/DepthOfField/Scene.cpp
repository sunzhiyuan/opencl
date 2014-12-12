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
    m_hTextureShader  = 0;
    m_pLogoTexture = NULL;
    m_pTexture     = NULL;

    m_fRange       = 4.0f;
    m_fFocus       = 0.8f;

    m_pBufferId[SHARP_FBO]      = NULL;
    m_pTextureId[SHARP_FBO]     = NULL;
    m_hDepthbufferId[SHARP_FBO] = NULL;

    m_pBufferId[OUT_OF_FOCUS_FBO]      = NULL;
    m_pTextureId[OUT_OF_FOCUS_FBO]     = NULL;
    m_hDepthbufferId[OUT_OF_FOCUS_FBO] = NULL;
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
            { "g_vTexCoord",   FRM_VERTEX_TEXCOORD0 }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/DOFTexture.vs",
                                                      "Samples/Shaders/DOFTexture.fs",
                                                      &m_hTextureShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hTextureModelViewMatrixLoc     = glGetUniformLocation( m_hTextureShader, "g_matModelView" );
        m_hTextureModelViewProjMatrixLoc = glGetUniformLocation( m_hTextureShader, "g_matModelViewProj" );
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
        m_hGaussianStepSizeLoc = glGetUniformLocation( m_hGaussianShader, "g_vStepSize" );
    }


    // Create our depth of field shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/DOF.vs",
                                                      "Samples/Shaders/DOF.fs",
                                                      &m_hDepthOfFieldShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hDepthOfFieldSharpImageLoc   = glGetUniformLocation( m_hDepthOfFieldShader, "g_sSharpImage" );
        m_hDepthOfFieldBlurredImageLoc = glGetUniformLocation( m_hDepthOfFieldShader, "g_sBlurredImage" );
        m_hDepthOfFieldRangeLoc        = glGetUniformLocation( m_hDepthOfFieldShader, "g_fRange" );
        m_hDepthOfFieldFocusLoc        = glGetUniformLocation( m_hDepthOfFieldShader, "g_fFocus" );
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
BOOL CSample::CreateFBO( UINT32 nFBOId, UINT32 nWidth, UINT32 nHeight )
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
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    
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
VOID CSample::FreeFBO( UINT32 nFBOId )
{
    if( m_pBufferId[ nFBOId ] )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferId[ nFBOId ] );
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    }
    
    if( m_pBufferId[ nFBOId ] )      glDeleteFramebuffers( 1, &m_pBufferId[ nFBOId ] );
    if( m_pTextureId[ nFBOId ] )     glDeleteTextures( 1, &m_pTextureId[ nFBOId ] );
    if( m_hDepthbufferId[ nFBOId ] ) glDeleteRenderbuffers( 1, &m_hDepthbufferId[ nFBOId ] );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GetFBO( UINT32 nFBOId, UINT32 nWidth, UINT32 nHeight )
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

    const FLOAT32 Quad[] =
    {
        +1.0, -1.0f, 1.0f, 1.0f,
        -1.0, -1.0f, 0.0f, 1.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
        +1.0, +1.0f, 1.0f, 0.0f,
    };

    glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, Quad );
    glEnableVertexAttribArray( 0 );

    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );

    glDisableVertexAttribArray( 0 );

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

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/DepthOfField.pak" ) )
        return FALSE;

    // Create the bowling pin texture
    m_pTexture = resource.GetTexture( "Bowling Pin" );

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_fRange , "Range", "%3.2f" );
    m_UserInterface.AddFloatVariable( &m_fFocus , "Focus", "%3.3f" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Decrease Range" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Increase Range" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Decrease Focus" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Increase Focus" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Samples/Meshes/BowlingPin.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
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
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/6.0f, fAspect, 0.1f, 1.2f );

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
    CreateFBO( SHARP_FBO, m_nWidth, m_nHeight );
    CreateFBO( OUT_OF_FOCUS_FBO, m_nOutOfFocusFBOWidth, m_nOutOfFocusFBOHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_hTextureShader )  glDeleteProgram( m_hTextureShader );

    if( m_pTexture )     m_pTexture->Release();
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    FreeFBO( SHARP_FBO );
    FreeFBO( OUT_OF_FOCUS_FBO );
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

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    if( nButtons & INPUT_KEY_1 )
    {
        if( m_fRange >= 0.1f )
            m_fRange -= fElapsedTime;
    }
    if( nButtons & INPUT_KEY_2 )
    {
        m_fRange += fElapsedTime;
    }
    if( nButtons & INPUT_KEY_3 )
    {
        if( m_fFocus >= 0.3f )
            m_fFocus -= 0.1f * fElapsedTime;
    }
    if( nButtons & INPUT_KEY_4 )
    {
        if( m_fFocus < 1.99f )
            m_fFocus += 0.1f * fElapsedTime;
    }

    // Rotate the object
    FRMMATRIX4X4 matTranslate = FrmMatrixTranslate( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3   vX = FRMVECTOR3( 1.0f, 0.0f, 0.0f );
	FRMVECTOR3   vY = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
	FRMMATRIX4X4 matRotate1   = FrmMatrixRotate( fTime, vY );
    FRMMATRIX4X4 matRotate2   = FrmMatrixRotate( 0.3f,  vX );
    FRMMATRIX4X4 matScale     = FrmMatrixScale( 0.5f, 0.5f, 0.5f );

    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.5f, 0.8f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.16f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // Build the matrices
    m_matModel = matTranslate;
    m_matModel = FrmMatrixMultiply( matScale, m_matModel );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderSharpSceneAndBlurinessToFBO()
{
    GetFBO( SHARP_FBO, m_nWidth, m_nHeight );

    // Need to create an alpha value which results in the depth of field for the background being 1
    FLOAT32 fAlpha = m_fFocus - 1.0f / m_fRange;

    glClearColor( 0.5f, 0.5f, 0.5f, fAlpha );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glUseProgram( m_hTextureShader );

    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );

    //                                  X      Z
    FLOAT32 BowlingPinPositions[] = {  0.0f,  0.0f,
                                      -0.1f, -0.1f,
                                       0.1f, -0.1f,
                                      -0.2f, -0.2f,
                                       0.0f, -0.2f,
                                       0.2f, -0.2f,
                                      -0.3f, -0.3f,
                                      -0.1f, -0.3f,
                                       0.1f, -0.3f,
                                       0.3f, -0.3f };

    for( INT32 i = 0; i < 10; ++i )
    {
		FRMMATRIX4X4 mTranslate = FrmMatrixTranslate( 1.2f * BowlingPinPositions[ 2 * i ], 0.0f, 2.0f * BowlingPinPositions[ 2 * i + 1 ] );
		FRMMATRIX4X4 matModel = FrmMatrixMultiply( mTranslate, m_matModel ); 
        m_matModelView     = FrmMatrixMultiply( matModel,       m_matView );
        m_matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );

        glUniformMatrix4fv( m_hTextureModelViewMatrixLoc,     1, FALSE, (FLOAT32*)&m_matModelView );
        glUniformMatrix4fv( m_hTextureModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );

        m_Mesh.Render();
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderBlurredSceneToFBO()
{
    GetFBO( OUT_OF_FOCUS_FBO, m_nOutOfFocusFBOWidth, m_nOutOfFocusFBOHeight );

    glUseProgram( m_hGaussianShader );
    glUniform2f( m_hGaussianStepSizeLoc, 1.0f / m_nOutOfFocusFBOWidth, 0.0f );
    
    glBindTexture( GL_TEXTURE_2D, m_pTextureId[ SHARP_FBO ] );

    RenderScreenAlignedQuad();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    GetFBO( OUT_OF_FOCUS_FBO, m_nOutOfFocusFBOWidth, m_nOutOfFocusFBOHeight );

    glUniform2f( m_hGaussianStepSizeLoc, 0.0f, 1.0f / m_nOutOfFocusFBOHeight );

    RenderScreenAlignedQuad();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderBlendedScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glViewport( 0, 0, m_nWidth, m_nHeight );

    glUseProgram( m_hDepthOfFieldShader );

    glUniform1f( m_hDepthOfFieldRangeLoc, m_fRange );
    glUniform1f( m_hDepthOfFieldFocusLoc, m_fFocus );

    glActiveTexture( GL_TEXTURE0 );
    glUniform1i( m_hDepthOfFieldSharpImageLoc, 0 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureId[ SHARP_FBO ] );

    glActiveTexture( GL_TEXTURE1 );
    glUniform1i( m_hDepthOfFieldBlurredImageLoc, 1 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureId[ OUT_OF_FOCUS_FBO ] );

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    RenderSharpSceneAndBlurinessToFBO();
    RenderBlurredSceneToFBO();
    RenderBlendedScene();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

