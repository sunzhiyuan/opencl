
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
#include <FrmFile.h>
#include <FrmNoise.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <OpenGLES/FrmUtilsGLES.h>
#include "Scene.h"
#include "Liquid.h"


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Fluid Simulation" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hSWEShader                 = 0;

    m_fStepSize     = 1.0f / 512.0f;
    m_fHalfStepSize = m_fStepSize / 2.0f;
 
    m_pBaseTexture     = NULL;
    m_pEnvTexture     = NULL;
    m_pNormalTexture     = NULL;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "g_vVertex", 0 }
    };
     
	const UINT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

	 // Create the swe shader
    {
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/SWE.vs",
                                                      "Samples/Shaders/SWE.fs",
                                                      &m_hSWEShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
        m_nSWEBaseTextureSlot = glGetUniformLocation( m_hSWEShader, "basemap" );
        m_nSWEEnvTextureSlot = glGetUniformLocation( m_hSWEShader, "envmap" );
        m_nSWENormalTextureSlot = glGetUniformLocation( m_hSWEShader, "normalmap" );
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

    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/SWE.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    m_pBaseTexture = resource.GetTexture( "Marble" );
    m_pEnvTexture = resource.GetTexture( "Env" );
    m_pNormalTexture = resource.GetTexture( "Normal" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Reset Simulation" );

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );

	// initialize simulation
	m_pLiquid = new Liquid();

#ifdef UNDER_CE
	m_pLiquid->Init(15, 20);
	m_pLiquid->ResetValues();
	m_pLiquid->Finger(0.7,0.3);
#else
	m_pLiquid->Init(24, 32);
	m_pLiquid->ResetValues();
    m_UserInterface.AddTextString( "Click mouse to start", 1.f, -1.f);
#endif // UNDER_CE

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
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
    if( m_hSWEShader )    glDeleteProgram( m_hSWEShader );

    // Release textures
    if( m_pLogoTexture )     m_pLogoTexture->Release();
	if( m_pBaseTexture )      m_pBaseTexture->Release();
	if( m_pEnvTexture )       m_pEnvTexture->Release();

    // Release font
    m_Font.Destroy();

	if (m_pLiquid)	m_pLiquid->Destroy();

}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    m_fElapsedTime = m_Timer.GetTime();

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    if( nPressedButtons & INPUT_KEY_1 )
    {
		m_pLiquid->ResetValues();
	}

	 UINT32 nPointerState;
	 FRMVECTOR2 vPointerPosition;
	 m_Input.GetPointerState(&nPointerState, &vPointerPosition);

	 if (nPointerState & FRM_INPUT::POINTER_DOWN)
	 {
		 float fx, fy;
		 fx = m_pLiquid->m_xoffset + (((vPointerPosition.x * 0.5f) + 0.5f) * m_pLiquid->m_xscale);
		 fy = m_pLiquid->m_yoffset + (((vPointerPosition.y * 0.5f) + 0.5f) * m_pLiquid->m_yscale);

		 m_pLiquid->Finger(fx, fy);
	 }

	m_pLiquid->Step(10);

}

//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetSWEShader()
{
    glUseProgram( m_hSWEShader );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pBaseTexture->m_hTextureHandle );
    glUniform1i( m_nSWEBaseTextureSlot, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_pEnvTexture->m_hTextureHandle );
    glUniform1i( m_nSWEEnvTextureSlot, 1 );

    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, m_pNormalTexture->m_hTextureHandle );
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_pLiquid->m_xres, m_pLiquid->m_yres, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pLiquid->CurrentNorm());
    
	glUniform1i( m_nSWENormalTextureSlot, 2 );

}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glDisable( GL_DEPTH_TEST );

    // Draw the scene
    {
        glDisable( GL_BLEND );

		SetSWEShader();
        

        FLOAT32 fAspectRatio = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
        FLOAT32 w_div_2 = 1.0f;
        FLOAT32 h_div_2 = 1.0f;
		
		float xmn = m_pLiquid->m_xoffset;
		float xmx = 1.f - m_pLiquid->m_xoffset;
		float ymn = m_pLiquid->m_yoffset;
		float ymx = 1.f - m_pLiquid->m_yoffset;

        FLOAT32 Quad[] =
        {
            -w_div_2, -h_div_2, xmn, ymx,
            +w_div_2, -h_div_2, xmx, ymx,
            +w_div_2, +h_div_2, xmx, ymn,
            -w_div_2, +h_div_2, xmn, ymn,
       };        
		glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, Quad );
        glEnableVertexAttribArray( 0 );

        // Draw the textured quad
        glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

