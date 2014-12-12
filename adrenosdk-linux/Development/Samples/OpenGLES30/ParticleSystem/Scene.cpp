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
    return new CSample( "ParticleSystem" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hParticleShader  = 0;
    m_pParticleTexture = NULL;
    m_pLogoTexture     = NULL;

    m_fParticleTimeX   = 0.0f;
    m_fParticleTimeY   = 0.0f;

	m_OpenGLESVersion  = GLES3;
}


//--------------------------------------------------------------------------------------
// Name: InitParticles()
// Desc: Initializes the particles
//--------------------------------------------------------------------------------------
static VOID InitParticles( PARTICLE* pParticles, GLuint *pParticleBuffers, GLuint *pParticleVAOs, GLuint *pQuery)
{
    for( UINT32 i = 0; i < NUM_PARTICLES; i++ )
    {
        pParticles[i].m_vPosition         =  FRMVECTOR3( 0.0f, 0.0f, 0.0f );
        pParticles[i].m_vVelocity         =  FRMVECTOR3( 0.0f, 0.0f, 0.0f );
        pParticles[i].m_vColor            =  FRMVECTOR4( 1.0, 0.0, 0.0, 0.0 );
        pParticles[i].m_fInitialSize      =  0.0f;
        pParticles[i].m_fSizeIncreaseRate =  0.0f;
        pParticles[i].m_fStartTime        =  0.0f;
        pParticles[i].m_fLifeSpan         =  -1.0f;
    }

	glGenQueries(1, pQuery);

	glGenVertexArrays(2, pParticleVAOs);

	// Create and initialize both Particle buffers to this particle data
	glGenBuffers(2, pParticleBuffers );

	glBindBuffer( GL_ARRAY_BUFFER, pParticleBuffers[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof(PARTICLE)*NUM_PARTICLES, pParticles, GL_STREAM_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	glBindBuffer( GL_ARRAY_BUFFER, pParticleBuffers[1] );	
	glBufferData( GL_ARRAY_BUFFER, sizeof(PARTICLE)*NUM_PARTICLES, pParticles, GL_STREAM_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}


//--------------------------------------------------------------------------------------
// Name: InitEmitter()
// Desc: Initializes the particle emitter
//--------------------------------------------------------------------------------------
VOID CParticleEmitter::Init()
{
    m_iParticleIndex     = 0;

    // Play around with the values for different effects
    m_fEmissionRate             = 10000.0f; // 10000 particles per second
    m_vPosition                 = FRMVECTOR3( 0.0f, 0.0f, -0.0f );
    m_vVelocity                 = FRMVECTOR3( -1.0f, 0.0f, 0.0f );
    m_fEmitterRadius            = 0.01f;
    m_fEmitterSpread            = 20.0f; // Emit within X degrees of the direction vector
    m_fInitialLifeSpan          = 5.5f;  // Particle lifetime in seconds
    m_fLifeSpanVariance         = 1.5f;  
    m_fSpeedVariance            = 0.5f;
    m_fInitialSize              = 25.0f;
    m_fInitialSizeVariance      = 5.0f;
    m_fSizeIncreaseRate         = 40.0f;
    m_fSizeIncreaseRateVarience = 10.0f;
    m_fInitialOpacity           = 1.0f;
    m_fOpacityVariance          = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the point sprite shader
    {
        // Create the vertex layout for the particles
        FRM_VERTEX_ELEMENT VertexLayout[] = 
        {
            { 0, 3, GL_FLOAT,         FALSE, 3*sizeof(FLOAT32) }, // m_vPosition
            { 1, 3, GL_FLOAT,         FALSE, 3*sizeof(FLOAT32) }, // m_vVelocity
            { 2, 4, GL_FLOAT,		  FALSE, 4*sizeof(FLOAT32) }, // m_vColor
            { 3, 1, GL_FLOAT,         FALSE, 1*sizeof(FLOAT32) }, // m_fStartTime
            { 4, 1, GL_FLOAT,         FALSE, 1*sizeof(FLOAT32) }, // m_fLifeSpan
            { 5, 1, GL_FLOAT,         FALSE, 1*sizeof(FLOAT32) }, // m_fInitialSize
            { 6, 1, GL_FLOAT,         FALSE, 1*sizeof(FLOAT32) }, // m_fFinalSize
            NULL
        };
        FrmMemcpy( m_ParticleVertexLayout, VertexLayout, sizeof(VertexLayout) );

        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vPosition",         0 },
            { "g_vVelocity",         1 },
            { "g_vColor",            2 },
            { "g_fStartTime",        3 },
            { "g_fLifeSpan",         4 },
            { "g_fInitialSize",      5 },
            { "g_fSizeIncreaseRate", 6 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PointSprites30.vs",
                                                      "Samples/Shaders/PointSprites30.fs",
                                                      &m_hParticleShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_slotModelViewProjMatrix = glGetUniformLocation( m_hParticleShader,  "g_matModelViewProj" );
        m_slotTime                = glGetUniformLocation( m_hParticleShader,  "g_fTime" );
    }


	{
		FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vPosition",         0 },
            { "g_vVelocity",         1 },
            { "g_vColor",            2 },
            { "g_fStartTime",        3 },
            { "g_fLifeSpan",         4 },
            { "g_fInitialSize",      5 },
            { "g_fSizeIncreaseRate", 6 },
        };

		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

		const char* varyings[7] = {
			"o_vPosition",
			"o_vVelocity",
			"o_vColor",
			"o_fStartTime",
			"o_fLifeSpan",
			"o_fInitialSize",
			"o_fSizeIncreaseRate"
		};

		m_emitParticleShader.ShaderID = glCreateProgram();
		GLuint hVertexShader  = glCreateShader( GL_VERTEX_SHADER );
		GLuint hFragmentShader  = glCreateShader( GL_FRAGMENT_SHADER );
		
		if( FALSE == FrmLoadShaderObjectFromFile( "Samples/Shaders/EmitParticles30.vs", hVertexShader ) )
			return FALSE;
		if( FALSE == FrmLoadShaderObjectFromFile( "Samples/Shaders/EmitParticles30.fs", hFragmentShader ) )
			return FALSE;
		glAttachShader( m_emitParticleShader.ShaderID, hVertexShader );
		glAttachShader( m_emitParticleShader.ShaderID, hFragmentShader );

		// Bind the attributes to the shader program
		FrmBindShaderAttributes( m_emitParticleShader.ShaderID, &pShaderAttributes[0], nNumShaderAttributes );

		// setup transform feedback varyings
		glTransformFeedbackVaryings( m_emitParticleShader.ShaderID, 7, varyings, GL_INTERLEAVED_ATTRIBS );

		// finally, link the program
		if( FALSE == FrmLinkShaderProgram( m_emitParticleShader.ShaderID ) )
			return FALSE;

		// get the uniform locations
		m_emitParticleShader.slotvPosition				= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_vPosition" );
		m_emitParticleShader.slotvVelocity				= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_vVelocity" );
		m_emitParticleShader.slotfEmitterSpread			= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fEmitterSpread" );
		m_emitParticleShader.slotfEmitterRadius			= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fEmitterRadius" );
		m_emitParticleShader.slotfSpeedVariance			= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fSpeedVariance" );
		m_emitParticleShader.slotfInitialOpacity		= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fInitialOpacity" );
		m_emitParticleShader.slotfOpacityVariance		= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fOpacityVariance" );
		m_emitParticleShader.slotfInitialLifeSpan		= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fInitialLifeSpan" );
		m_emitParticleShader.slotfLifeSpanVariance		= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fLifeSpanVariance" );
		m_emitParticleShader.slotfInitialSize			= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fInitialSize" );
		m_emitParticleShader.slotfInitialSizeVariance	= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fInitialSizeVariance" );
		m_emitParticleShader.slotfSizeIncreaseRate		= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fSizeIncreaseRate" );
		m_emitParticleShader.slotfSizeIncreaseRateVariance= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fSizeIncreaseRateVariance" );
		m_emitParticleShader.slotfTime					= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fTime" );
		m_emitParticleShader.slotvColor					= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_vColor" );
		m_emitParticleShader.slotfEmissionRate			= glGetUniformLocation( m_emitParticleShader.ShaderID, "u_fEmissionRate" );
	}

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: UpdateEmitterPosition()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CParticleEmitter::UpdateEmitterPosition( const FRMVECTOR3& vPosition )
{
    m_vPosition = vPosition;
}


//--------------------------------------------------------------------------------------
// Name: UpdateEmitterVelocity()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CParticleEmitter::UpdateEmitterVelocity( const FRMVECTOR3& vVelocity )
{
    m_vVelocity = vVelocity;
}


//--------------------------------------------------------------------------------------
// Name: GetNextParticle()
// Desc: 
//--------------------------------------------------------------------------------------
PARTICLE* CParticleEmitter::GetNextParticle( PARTICLE* pParticles )
{
    m_iParticleIndex = ( m_iParticleIndex + 1 ) % NUM_PARTICLES;
    return &pParticles[m_iParticleIndex];
}
                                       
                                       
//--------------------------------------------------------------------------------------
// Name: EmitParticles()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CParticleEmitter::EmitParticles( PARTICLE* pParticles, FLOAT32 fElapsedTime,
                                      FLOAT32 fCurrentTime, FRMCOLOR Colors[NUM_COLORS],
                                      FLOAT32 fTimePerColor,
									  EmitParticleShader *epShader,
									  GLuint *srcBuffer, GLuint *pVAOs, GLuint *dstBuffer,
									  FRM_VERTEX_ELEMENT* pVertexLayout,
									  UINT32 nVertexSize, BOOL bUseTransformFeedback, GLuint Query )
{
    // Determine the color of the particles to be emitted
    FLOAT32  t = ( fCurrentTime / fTimePerColor ) - FrmFloor( fCurrentTime / fTimePerColor );
    UINT32   nColorIndex = (INT32)FrmFloor( fCurrentTime / fTimePerColor ) % NUM_COLORS;
    FRMCOLOR vColor1 = Colors[ nColorIndex ];
    FRMCOLOR vColor2 = Colors[(nColorIndex+1) % NUM_COLORS ];
    FRMCOLOR vColor  = FrmLerp( t, vColor1, vColor2 );

    m_fLeftOverParticles += m_fEmissionRate * fElapsedTime;

	if( !bUseTransformFeedback )
	{
		// Create the needed number of particles based upon our emission rate.
		for( INT32 i = 0; i < (INT32)NUM_PARTICLES; i++ )
		{
			// Get the next particle
			PARTICLE* pParticle = GetNextParticle( pParticles );

			if( FrmRand() < m_fLeftOverParticles / NUM_PARTICLES )
			{
				FLOAT32      fAngle            = m_fEmitterSpread * ( FrmRand() - 0.5f );
				FRMMATRIX4X4 matRotate         = FrmMatrixRotate( FrmRadians(fAngle), 0.0f, 0.0f, -1.0f );
				FRMVECTOR3   vVelocity         = FrmVector3TransformCoord( m_vVelocity, matRotate );

				pParticle->m_vPosition         = m_vPosition + FrmSphrand( m_fEmitterRadius );
				pParticle->m_vVelocity         = vVelocity * ApplyVariance( 1.0f, m_fSpeedVariance );
				pParticle->m_vColor            = FRMVECTOR4( vColor.r/255.0f, vColor.g/255.0f, vColor.b/255.0f, vColor.a/255.0f );
				pParticle->m_vColor.a          = ApplyVariance( m_fInitialOpacity, m_fOpacityVariance );
				pParticle->m_fStartTime        = fCurrentTime;
				pParticle->m_fLifeSpan         = ApplyVariance( m_fInitialLifeSpan, m_fLifeSpanVariance );
				pParticle->m_fInitialSize      = ApplyVariance( m_fInitialSize, m_fInitialSizeVariance );
				pParticle->m_fSizeIncreaseRate = ApplyVariance( m_fSizeIncreaseRate, m_fSizeIncreaseRateVarience );
			}
		}
	}
	else
	{
		// Using transform feedback to do particle emission

		// Set up the shader
		glUseProgram( epShader->ShaderID );
		glUniform3fv( epShader->slotvPosition, 1, (FLOAT32 *)&m_vPosition );
		glUniform3fv( epShader->slotvVelocity, 1, (FLOAT32 *)&m_vVelocity );
		glUniform1f( epShader->slotfEmitterSpread, m_fEmitterSpread );
		glUniform1f( epShader->slotfEmitterRadius, m_fEmitterRadius );
		glUniform1f( epShader->slotfSpeedVariance, m_fSpeedVariance );
		glUniform1f( epShader->slotfInitialOpacity, m_fInitialOpacity );
		glUniform1f( epShader->slotfOpacityVariance, m_fOpacityVariance );
		glUniform1f( epShader->slotfInitialLifeSpan, m_fInitialLifeSpan );
		glUniform1f( epShader->slotfLifeSpanVariance, m_fLifeSpanVariance );
		glUniform1f( epShader->slotfInitialSize, m_fInitialSize );
		glUniform1f( epShader->slotfInitialSizeVariance, m_fInitialSizeVariance );
		glUniform1f( epShader->slotfSizeIncreaseRate, m_fSizeIncreaseRate );
		glUniform1f( epShader->slotfSizeIncreaseRateVariance, m_fSizeIncreaseRateVarience );
		glUniform1f( epShader->slotfTime, fCurrentTime );
		glUniform4f( epShader->slotvColor, vColor.r/255.0f, vColor.g/255.0f, vColor.b/255.0f, vColor.a/255.0f );
		glUniform1f( epShader->slotfEmissionRate, m_fLeftOverParticles/NUM_PARTICLES );


		glEnable( GL_RASTERIZER_DISCARD );

		glBindVertexArray(pVAOs[0]);
		// set source buffer
		glBindBuffer( GL_ARRAY_BUFFER, *srcBuffer );		
		FrmSetVertexLayout( pVertexLayout, nVertexSize, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );	
		glBindVertexArray(0);

		// set destination buffer
		glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, *dstBuffer );
		
		glBindVertexArray(pVAOs[0]);

		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, Query); 

		// Perfom transform feedback
		glBeginTransformFeedback( GL_POINTS );

		glDrawArrays( GL_POINTS, 0, NUM_PARTICLES );
		glEndTransformFeedback();

		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN); 

		glDisable( GL_RASTERIZER_DISCARD );

		glBindVertexArray(0);
		glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0 );
	}

	m_fLeftOverParticles -= FrmFloor( m_fLeftOverParticles );
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    m_Colors[0]      = FRMCOLOR( 1.0f, 0.5f, 0.5f );
    m_Colors[1]      = FRMCOLOR( 1.0f, 1.0f, 0.5f );
    m_Colors[2]      = FRMCOLOR( 0.5f, 1.0f, 0.5f );
    m_Colors[3]      = FRMCOLOR( 0.5f, 1.0f, 1.0f );
    m_Colors[4]      = FRMCOLOR( 0.5f, 0.5f, 1.0f );
    m_Colors[5]      = FRMCOLOR( 1.0f, 0.5f, 1.0f );
    m_fXFactor       = 0.15f;
    m_fYFactor       = 0.10f;
    m_fParticleSpeed = 0.35f;
    m_fTimePerColor  = 1.0f;
    m_bUseSrcAlpha   = TRUE;
	m_bUseTransformFeedback = TRUE;

    InitParticles( m_Particles, &m_hParticleBuffers[0], &m_hParticleVAOs[0], &m_hQuery );
	
	// Initialize buffer indices
	m_srcBuffer = 0;
	m_dstBuffer = 1;
    
    for( UINT32 i = 0; i < NUM_EMITTERS; ++i )
        m_ParticleEmitters[i].Init();

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/ParticleSystem.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Create the particle texture
    m_pParticleTexture = resource.GetTexture( "Particle" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_fXFactor , "X Factor", "%4.2f" );
    m_UserInterface.AddFloatVariable( &m_fYFactor , "Y Factor", "%4.2f" );
    m_UserInterface.AddFloatVariable( &m_fParticleSpeed , "Speed", "%4.2f" );
    m_UserInterface.AddBoolVariable( &m_bUseSrcAlpha , "Use Source Alpha", "Yes", "No" );
	m_UserInterface.AddBoolVariable( &m_bUseTransformFeedback , "Use Transform Feedback", "Yes", "No" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0,   "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1,   "Decrease X Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2,   "Increase X Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3,   "Decrease Y Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4,   "Increase Y Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5,   "Decrease Speed" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6,   "Increase Speed" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7,   "Toggle Blend Mode" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_8,   "Toggle Transform Feedback vs CPU Particles" );

    // Create the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 1000.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    FRMMATRIX4X4 m_matView;
    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.0f, 5.0f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matView = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matProj = FrmMatrixMultiply( m_matView, m_matProj );

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
    if( m_hParticleShader )	  glDeleteProgram( m_hParticleShader );

    if( m_pParticleTexture )  m_pParticleTexture->Release();
    if( m_pLogoTexture )      m_pLogoTexture->Release();
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
    {
        UINT32 nButtons;
        UINT32 nPressedButtons;
        FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

        // Toggle user interface
        if( nPressedButtons & INPUT_KEY_0 )
            m_UserInterface.AdvanceState();

        if( nButtons & INPUT_KEY_1 )
            m_fXFactor -= m_fXFactor * fElapsedTime;

        if( nButtons & INPUT_KEY_2 )
            m_fXFactor += m_fXFactor * fElapsedTime;

        if( nButtons & INPUT_KEY_3 )
            m_fYFactor -= m_fYFactor * fElapsedTime;

        if( nButtons & INPUT_KEY_4 )
            m_fYFactor += m_fYFactor * fElapsedTime;

        if( nButtons & INPUT_KEY_5 )
            m_fParticleSpeed -= m_fParticleSpeed * fElapsedTime;

        if( nButtons & INPUT_KEY_6 )
            m_fParticleSpeed += m_fParticleSpeed * fElapsedTime;

        if( nPressedButtons & INPUT_KEY_7 )
            m_bUseSrcAlpha = !m_bUseSrcAlpha;

		if( nPressedButtons & INPUT_KEY_8 )
            m_bUseTransformFeedback = !m_bUseTransformFeedback;
    }

    // Update the emitter position to do an interesting effect
    m_fParticleTimeX += fElapsedTime * m_fXFactor;
    m_fParticleTimeY += fElapsedTime * m_fYFactor;

    FRMVECTOR3 vEmitterPosition;
    vEmitterPosition.x = FrmSin( m_fParticleTimeX * 2*FRM_PI );
    vEmitterPosition.y = FrmCos( m_fParticleTimeY * 2*FRM_PI );
    vEmitterPosition.z = 0.0f;

    m_ParticleEmitters[0].UpdateEmitterPosition( vEmitterPosition );

    // Update the emitter direction to do an interesting effect
    FRMVECTOR3 vEmitterDirection;
    vEmitterDirection.x = FrmCos( fTime * 2*FRM_PI );
    vEmitterDirection.y = FrmSin( fTime * 2*FRM_PI );
    vEmitterDirection.z = 0.0f; 
    vEmitterDirection   = FrmVector3Normalize( vEmitterDirection );
    m_ParticleEmitters[0].UpdateEmitterVelocity( m_fParticleSpeed * vEmitterDirection );
	
    // Emit some new particles into the m_Particles buffer
    m_ParticleEmitters[0].EmitParticles( m_Particles, fElapsedTime, fTime, 
                                         m_Colors, m_fTimePerColor, &m_emitParticleShader,
										 &m_hParticleBuffers[m_srcBuffer], &m_hParticleVAOs[0], &m_hParticleBuffers[m_dstBuffer],
										 &m_ParticleVertexLayout[0], sizeof(PARTICLE), m_bUseTransformFeedback, m_hQuery);

	if( m_bUseTransformFeedback )
	{
		m_srcBuffer = m_dstBuffer;
		m_dstBuffer = abs(1 - m_dstBuffer);
	}
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    // Clear the backbuffer
    glClearColor( 0.0f, 0.0f, 0.5f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    // Draw the particles
    {
        glDisable( GL_DEPTH_TEST );
    
        if( m_bUseSrcAlpha )
        {
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        }
        else
        {
            glBlendFunc( GL_SRC_ALPHA, GL_ONE );
        }
        glEnable( GL_BLEND );
        glBindTexture( GL_TEXTURE_2D, m_pParticleTexture->m_hTextureHandle );

        glUseProgram( m_hParticleShader );
        glUniformMatrix4fv( m_slotModelViewProjMatrix, 1, FALSE, (FLOAT32*)&m_matProj );
        glUniform1f( m_slotTime, m_Timer.GetFrameTime() );

		if( !m_bUseTransformFeedback )
		{
			FrmSetVertexLayout( m_ParticleVertexLayout, sizeof(PARTICLE), m_Particles );
			glDrawArrays( GL_POINTS, 0, NUM_PARTICLES );
		}
		else
		{
			GLuint PrimitivesWritten = 0;
			glGetQueryObjectuiv(m_hQuery, GL_QUERY_RESULT, &PrimitivesWritten);

			glBindVertexArray(m_hParticleVAOs[1]);
			glBindBuffer( GL_ARRAY_BUFFER, m_hParticleBuffers[ m_dstBuffer ]);			
			FrmSetVertexLayout( m_ParticleVertexLayout, sizeof(PARTICLE), 0 );
			glBindVertexArray(0);
			glBindBuffer( GL_ARRAY_BUFFER, 0);		

			glBindVertexArray(m_hParticleVAOs[1]);
			glDrawArrays( GL_POINTS, 0, NUM_PARTICLES );
			glBindVertexArray(0);
		}
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

