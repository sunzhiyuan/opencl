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
#include "MotionBlur.h"
#include "Scene.h"


FLOAT32 g_pCube[] =
{
    // FRONT
    -0.5f, -0.5f, 0.5f,
     0.5f, -0.5f, 0.5f,
    -0.5f,  0.5f, 0.5f,
     0.5f,  0.5f, 0.5f,

    // BACK
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,

    // LEFT
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // RIGHT
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,

    // TOP
    -0.5f, 0.5f,  0.5f,
     0.5f, 0.5f,  0.5f,
    -0.5f, 0.5f, -0.5f,
     0.5f, 0.5f, -0.5f,
    
    // BOTTOM
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f
};

FLOAT32 g_pColor[] =
{
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f
};


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "MotionBlur" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hConstantShader = NULL;
    m_hTextureShader  = NULL;
    m_pLogoTexture    = NULL;
    m_pMotionBlur	  = NULL;

    m_fStartTime   = FrmGetTime();
    m_fCurrentTime = 0.0f;

    m_nFBOTextureWidth   = 256;
    m_nFBOTextureHeight  = 256;

    m_pMotionBlur        = NULL;
    m_bMotionBlurOn      = TRUE;

    m_fSpeed             = 2.0f;
    m_fShutterDuration   = 0.025f;
    m_fRotTotal          = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the Texture shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex", 0 }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Texture.vs",
                                                      "Samples/Shaders/TextureAlpha.fs",
                                                      &m_hTextureShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_nTextureAlphaSlot = glGetUniformLocation( m_hTextureShader, "g_fAlpha" );
    }

    // Create the Constant shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        { 
            { "g_vVertex", 0 },
            { "g_vColor",  1 }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Constant.vs",
                                                      "Samples/Shaders/Constant.fs",
                                                      &m_hConstantShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
        m_nConstantMVPSlot = glGetUniformLocation( m_hConstantShader,  "g_mModelViewProjectionMatrix" );
    }

    return TRUE;
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
    if( FALSE == resource.LoadFromFile( "Samples/Textures/MotionBlur.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_bMotionBlurOn, "Motion Blur", "On", "Off" );
    m_UserInterface.AddFloatVariable( &m_fSpeed, "Speed", "%4.2f" );
    m_UserInterface.AddFloatVariable( &m_fShutterDuration, "Shutter Duration", "%4.4f sec" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Decrease Speed" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Increase Speed" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Decrease Shutter Duration" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Increase Shutter Duration" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Toggle Motion Blur (on/off)" );

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Setup the view
    FRMVECTOR3 vPosition   = FRMVECTOR3( 0.0f, 0.0f, 4.0f );
    FRMVECTOR3 vLookAt     = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vUp         = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matView = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );

    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_mPerspectiveMatrix = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_mPerspectiveMatrix.M(0,0) *= fAspect;
        m_mPerspectiveMatrix.M(1,1) *= fAspect;

        // Rotate the cube to better use the space
		FRMVECTOR3 vZ = FRMVECTOR3(0.0f, 0.0f, 1.0f);
		FRMMATRIX4X4 mRotate = FrmMatrixRotate( FRM_PI/2, vZ );
		m_mPerspectiveMatrix = FrmMatrixMultiply( mRotate, m_mPerspectiveMatrix );
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Create the motion blur class
    delete m_pMotionBlur;
    m_pMotionBlur = new CMotionBlur( m_hTextureShader, m_nTextureAlphaSlot,
                                     WrapperRenderBlurredScene, this, m_fShutterDuration, 
                                     m_nWidth, m_nHeight, m_nFBOTextureWidth, m_nFBOTextureHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Release shaders
    if( m_hConstantShader ) glDeleteProgram( m_hConstantShader );
    if( m_hTextureShader )  glDeleteProgram( m_hTextureShader );

    // Release textures
    if( m_pLogoTexture )     m_pLogoTexture->Release();

    delete m_pMotionBlur;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the frame elapsed time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    // Adjust the speed
    if( nButtons & INPUT_KEY_1 )
        m_fSpeed -= 0.5f * fElapsedTime;

    if( nButtons & INPUT_KEY_2 )
        m_fSpeed += 0.5f * fElapsedTime;

    // Adjust the shutter duration
    if( nButtons & INPUT_KEY_3 )
    {
        m_fShutterDuration -= 0.01f * fElapsedTime;
        if( m_fShutterDuration < 0.001f )
        {
            m_fShutterDuration = 0.001f;
        }
        m_pMotionBlur->SetShutterDuration( m_fShutterDuration );
    }
    if( nButtons & INPUT_KEY_4 )
    {
        m_fShutterDuration += 0.01f * fElapsedTime;
        m_pMotionBlur->SetShutterDuration( m_fShutterDuration );
    }

    // Toggle the motion blur effect
    if( nPressedButtons & INPUT_KEY_5 )
        m_bMotionBlurOn = !m_bMotionBlurOn;
}


//--------------------------------------------------------------------------------------
// Name: RenderCube()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderCube()
{
    FLOAT32 fPrevFrameTime = m_fCurrentTime;
    m_fCurrentTime = FrmGetTime();

    // Rotate the object
    m_fRotTotal += m_fSpeed * ( m_fCurrentTime - fPrevFrameTime );

    FRMMATRIX4X4 matTrans1  = FrmMatrixTranslate( 0.0f, 0.6f * FrmSin( m_fRotTotal * 2.0f ), 0.0f );
	FRMVECTOR3 vTmp;
	vTmp = FRMVECTOR3(0.0f, 0.0f, 1.0f);
	FRMMATRIX4X4 matRotate1 = FrmMatrixRotate( m_fRotTotal *  1.0f, vTmp );
	vTmp = FRMVECTOR3(1.0f, 0.0f, 0.0f);
	FRMMATRIX4X4 matRotate2 = FrmMatrixRotate( m_fRotTotal *  3.0f, vTmp );
	vTmp = FRMVECTOR3(0.0f, 1.0f, 0.0f);
	FRMMATRIX4X4 matRotate3 = FrmMatrixRotate( m_fRotTotal *  1.5f, vTmp );
	vTmp = FRMVECTOR3(1.0f, 0.0f, 1.0f);
	FRMMATRIX4X4 matRotate4 = FrmMatrixRotate( m_fRotTotal * -1.0f, vTmp );

    FRMMATRIX4X4 matModel     = FrmMatrixMultiply( matRotate4,   matTrans1 );
    matModel                  = FrmMatrixMultiply( matRotate3,   matModel );
    matModel                  = FrmMatrixMultiply( matRotate2,   matModel );
    matModel                  = FrmMatrixMultiply( matRotate1,   matModel );
    FRMMATRIX4X4 matModelView = FrmMatrixMultiply( matModel,     m_matView );
    m_matModelViewProj        = FrmMatrixMultiply( matModelView, m_mPerspectiveMatrix );

    glEnable( GL_DEPTH_TEST );

    glUseProgram( m_hConstantShader );
    glUniformMatrix4fv( m_nConstantMVPSlot, 1, FALSE, (FLOAT32*)&m_matModelViewProj );

    glVertexAttribPointer( 0, 3, GL_FLOAT, 0, 0, g_pCube );
    glEnableVertexAttribArray( 0 );

    glVertexAttribPointer( 1, 4, GL_FLOAT, 0, 0, g_pColor );
    glEnableVertexAttribArray( 1 );

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glDrawArrays( GL_TRIANGLE_STRIP, 4, 4 );

    glDrawArrays( GL_TRIANGLE_STRIP,  8, 4 );
    glDrawArrays( GL_TRIANGLE_STRIP, 12, 4 );

    glDrawArrays( GL_TRIANGLE_STRIP, 16, 4 );
    glDrawArrays( GL_TRIANGLE_STRIP, 20, 4 );

    glDisableVertexAttribArray( 1 );
    glDisableVertexAttribArray( 0 );

    glDisable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name: RenderBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderBlurredScene()
{
    // Clear the backbuffer and depth-buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    RenderCube();
}


//--------------------------------------------------------------------------------------
// Name: WrapperRenderBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::WrapperRenderBlurredScene( VOID* pScene )
{
    CSample* pCSampleScene = (CSample*)pScene;
    pCSampleScene->RenderBlurredScene();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );

    if( m_bMotionBlurOn )
    {
        m_pMotionBlur->Apply();
    }
    else
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        RenderBlurredScene();
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

