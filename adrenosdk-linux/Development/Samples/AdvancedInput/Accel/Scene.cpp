//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
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
    return new CSample( "Accel" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hTextureShader     = 0;
    m_pLogoTexture       = NULL;
    m_pTexture           = NULL;
	m_pTexture2          = NULL;
	m_bCalibrateOn       = FALSE;
	m_bShowSensorValues  = FALSE;
	m_X				     = 0.0f;
	m_Y				     = 0.0f;

}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
	// Create the shader for rendering objects
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
    if( FALSE == resource.LoadFromFile( "Samples/Textures/Accel.pak" ) )
        return FALSE;

    // Create the textures for objects to render
    m_pTexture  = resource.GetTexture( "Bowling Pin" );
    m_pTexture2 = resource.GetTexture( "Floor Texture" );

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

	// Setup the user interface
	if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
		return FALSE;
	m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
		m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
	m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
	
	// Visual variables
	m_UserInterface.AddBoolVariable( &m_bShowSensorValues , "Show Sensor Values", "True", "False" );

	// Help keys
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Toggle Show Sensor Values" );

    // Load the meshes
	if( FALSE == m_Mesh.Load( "Samples/Meshes/BowlingPin.mesh" ) )
		return FALSE;

	if( FALSE == m_Mesh.MakeDrawable( &resource ) )
		return FALSE;

	if( FALSE == m_Mesh2.Load( "Samples/Meshes/Plane.mesh" ) )
		return FALSE;

	if( FALSE == m_Mesh2.MakeDrawable( &resource ) )
		return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
	// Enable standard OpenGL states
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
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/6.0f, fAspect, 0.1f, 10.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
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
    if( m_hTextureShader )  
		glDeleteProgram( m_hTextureShader );

    if( m_pTexture )     
		m_pTexture->Release();

	if( m_pTexture2 )     
		m_pTexture2->Release();

    if( m_pLogoTexture ) 
		m_pLogoTexture->Release();
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

	// Toggle Show Sensor values
	if( nPressedButtons & INPUT_KEY_1 )
		m_bShowSensorValues = !m_bShowSensorValues;

	// Turn on / off calibration with pointer press / or screen touch
	if(m_Input.m_nPointerState & FRM_INPUT::POINTER_DOWN)
	{
		m_bCalibrateOn = TRUE;
	}
	else
	{
		m_bCalibrateOn = FALSE;
	}

	//----------------
	// SENSOR INPUT

	// Get Accelerometer values
	FRMVECTOR3 &ASmooth    = m_Input.m_AccelSmooth;
	FRMVECTOR3 &AVelocity  = m_Input.m_AccelVelocity;
	
	m_Input.m_AccelHPFK = 0.9f;	// Setting filter smoothes out Accelerometer but reduces accuracy and 
								// creates a slower response

	if(m_Input.m_AccelSupported == TRUE) 
	{
		// Calibration for this example consists of removing the resting component of the 
		// accelerometer (so that we can center the camera on the scene)
		if(m_bCalibrateOn == TRUE)
		{
			m_X = 0.0f;
			m_Y = 0.0f;
			AVelocity.v[0] = 0.0f;
			AVelocity.v[1] = 0.0f;
			AVelocity.v[2] = 0.0f;
		}
	}

	// Change the x and y position of the camera based on the accelerometer values
	m_X +=  AVelocity.v[0] * 0.05f;  // Scale velocity to keep camera on objects
	m_Y += -AVelocity.v[1] * 0.05f;  // ""

	//----------------
	// CAMERA

	FRMVECTOR3 vPosition = FRMVECTOR3( m_X, 1.5f, (m_Y - 0.2f ));
    FRMVECTOR3 vLookAt   = FRMVECTOR3( m_X, 0.0f, (m_Y - 0.2f ));
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 0.0f, -1.0f );

    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

	//----------------
	// OBJECT(S)

	// Scale object
	FRMMATRIX4X4 matTranslate = FrmMatrixTranslate( 0.0f, 0.0f, 0.0f );
	FRMMATRIX4X4 matScale     = FrmMatrixScale( 0.5f, 0.5f, 0.5f );

	// Build the matrices
    m_matModel = matTranslate;
    m_matModel = FrmMatrixMultiply( matScale, m_matModel );
}


//--------------------------------------------------------------------------------------
// Name: RenderScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderScene()
{
    // Set the clear color and the buffers to clear out
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glUseProgram( m_hTextureShader );

    glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );

    //                                  X      Z
    FLOAT32 BowlingPinPositions[] = {
//									   0.0f,  0.0f,
                                      -0.1f, -0.1f,
                                       0.1f, -0.1f,
                                      -0.2f, -0.2f,
                                       0.0f, -0.2f,
                                       0.2f, -0.2f,
//                                    -0.3f, -0.3f,
                                      -0.1f, -0.3f,
                                       0.1f, -0.3f,
//                                     0.3f, -0.3f 
									};

    for( INT32 i = 0; i < 7; ++i )
    {
		FRMMATRIX4X4 mTranslate = FrmMatrixTranslate( 1.2f * BowlingPinPositions[ 2 * i ], 0.0f, 2.0f * BowlingPinPositions[ 2 * i + 1 ] );
		FRMMATRIX4X4 matModel = FrmMatrixMultiply( mTranslate, m_matModel ); 
        m_matModelView     = FrmMatrixMultiply( matModel,       m_matView );
        m_matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );

        glUniformMatrix4fv( m_hTextureModelViewMatrixLoc,     1, FALSE, (FLOAT32*)&m_matModelView );
        glUniformMatrix4fv( m_hTextureModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );

        m_Mesh.Render();

		// This is the right offset for the floor, so render this now
		if(i == 3)
		{
			glBindTexture( GL_TEXTURE_2D, m_pTexture2->m_hTextureHandle );
			m_Mesh2.Render();
			glBindTexture( GL_TEXTURE_2D, m_pTexture->m_hTextureHandle );
		}
    }
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    RenderScene();
	m_Timer.MarkFrame();
	m_UserInterface.Render( m_Timer.GetFrameRate() );
	RenderSensorValues();

}


//--------------------------------------------------------------------------------------
// Name: RenderSensorValues()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderSensorValues()
{
	// Show calibration message if on
	{
		static int progressCountInit = 30;
		static int progressCount = progressCountInit;

		if(m_bCalibrateOn == TRUE)
		{
			if(progressCount > 10)
			{
				CHAR strCalibrate[64];
				FrmSprintf(strCalibrate, 64, "CALIBRATING ACCELEROMETER, KEEP DEVICE STILL . . .");
				m_Font.SetScaleFactors( 1.0f, 1.0f );
				m_Font.Begin();
				m_Font.DrawText( 30, 100, FRMCOLOR_YELLOW, strCalibrate);
				m_Font.End();
			}

			progressCount--; 
			if (progressCount <= 0) 
				progressCount = progressCountInit;

		}
		else
		{
			progressCount = progressCountInit;
		}
	}

	// Show data values of sensor if on
	if(m_bShowSensorValues == TRUE)
	{
		FRMVECTOR3 &ARaw    = m_Input.m_AccelRaw;
		FRMVECTOR3 &ASmooth = m_Input.m_AccelSmooth;
		FRMVECTOR3 &AVel    = m_Input.m_AccelVelocity;

		CHAR strAccel[64];

		m_Font.SetScaleFactors( 1.0f, 1.0f );

		m_Font.Begin();

		FrmSprintf(strAccel, 64, "RAW X: %1.5f", ARaw.v[0]);
		m_Font.DrawText( 10, 600, FRMCOLOR_WHITE, strAccel);
		FrmSprintf(strAccel, 64, "RAW Y: %1.5f", ARaw.v[1]);
		m_Font.DrawText( 10, 625, FRMCOLOR_WHITE, strAccel);
		FrmSprintf(strAccel, 64, "RAW Z: %1.5f", ARaw.v[2]);
		m_Font.DrawText( 10, 650, FRMCOLOR_WHITE, strAccel);

		FrmSprintf(strAccel, 64, "SMOOTH X: %1.5f", ASmooth.v[0]);
		m_Font.DrawText( 160, 600, FRMCOLOR_WHITE, strAccel);
		FrmSprintf(strAccel, 64, "SMOOTH Y: %1.5f", ASmooth.v[1]);
		m_Font.DrawText( 160, 625, FRMCOLOR_WHITE, strAccel);
		FrmSprintf(strAccel, 64, "SMOOTH Z: %1.5f", ASmooth.v[2]);
		m_Font.DrawText( 160, 650, FRMCOLOR_WHITE, strAccel);

		FrmSprintf(strAccel, 64, "VEL X: %1.5f", AVel.v[0]);
		m_Font.DrawText( 340, 600, FRMCOLOR_WHITE, strAccel);
		FrmSprintf(strAccel, 64, "VEL Y: %1.5f", AVel.v[1]);
		m_Font.DrawText( 340, 625, FRMCOLOR_WHITE, strAccel);
		FrmSprintf(strAccel, 64, "VEL Z: %1.5f", AVel.v[2]);
		m_Font.DrawText( 340, 650, FRMCOLOR_WHITE, strAccel);

		m_Font.End();
	}
}