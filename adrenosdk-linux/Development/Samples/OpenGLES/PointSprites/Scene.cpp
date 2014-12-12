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
    return new CSample( "PointSprites" );
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
}


//--------------------------------------------------------------------------------------
// Name: InitParticles()
// Desc: Initializes the particles
//--------------------------------------------------------------------------------------
static VOID InitParticles( PARTICLE* pParticles )
{
    for( UINT32 i = 0; i < NUM_PARTICLES; i++ )
    {
        pParticles[i].m_vPosition         =  FRMVECTOR3( 0.0f, 0.0f, 0.0f );
        pParticles[i].m_vVelocity         =  FRMVECTOR3( 0.0f, 0.0f, 0.0f );
        pParticles[i].m_vColor            =  0xff000000;
        pParticles[i].m_fInitialSize      =  0.0f;
        pParticles[i].m_fSizeIncreaseRate =  0.0f;
        pParticles[i].m_fStartTime        =  0.0f;
        pParticles[i].m_fLifeSpan         = -1.0f;
    }
}


//--------------------------------------------------------------------------------------
// Name: InitEmitter()
// Desc: Initializes the particle emitter
//--------------------------------------------------------------------------------------
VOID CParticleEmitter::Init()
{
    m_iParticleIndex     = 0;

    // Play around with the values for different effects
    m_fEmissionRate             = 100.0f; // 100 particles per second
    m_vPosition                 = FRMVECTOR3( 0.0f, 0.0f, -0.0f );
    m_vVelocity                 = FRMVECTOR3( -1.0f, 0.0f, 0.0f );
    m_fEmitterRadius            = 0.01f;
    m_fEmitterSpread            = 20.0f; // Emit within X degrees of the direction vector
    m_fInitialLifeSpan          = 1.5f;  // Particle lifetime in seconds
    m_fLifeSpanVariance         = 1.5f;  
    m_fSpeedVariance            = 0.5f;
    m_fInitialSize              = 25.0f;
    m_fInitialSizeVariance      = 5.0f;
    m_fSizeIncreaseRate         = 40.0f;
    m_fSizeIncreaseRateVarience = 20.0f;
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
            { 2, 4, GL_UNSIGNED_BYTE, TRUE,  4*sizeof(BYTE)    }, // m_vColor
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PointSprites.vs",
                                                      "Samples/Shaders/PointSprites.fs",
                                                      &m_hParticleShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_slotModelViewProjMatrix = glGetUniformLocation( m_hParticleShader,  "g_matModelViewProj" );
        m_slotTime                = glGetUniformLocation( m_hParticleShader,  "g_fTime" );
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
                                      FLOAT32 fTimePerColor )
{
    // Determine the color of the particles to be emitted
    FLOAT32  t = ( fCurrentTime / fTimePerColor ) - FrmFloor( fCurrentTime / fTimePerColor );
    UINT32   nColorIndex = (INT32)FrmFloor( fCurrentTime / fTimePerColor ) % NUM_COLORS;
    FRMCOLOR vColor1 = Colors[ nColorIndex ];
    FRMCOLOR vColor2 = Colors[(nColorIndex+1) % NUM_COLORS ];
    FRMCOLOR vColor  = FrmLerp( t, vColor1, vColor2 );

    m_fLeftOverParticles += m_fEmissionRate * fElapsedTime;

    // Create the needed number of particles based upon our emission rate.
    for( INT32 i = 0; i < (INT32)m_fLeftOverParticles; i++ )
    {
        // Get the next particle
        PARTICLE* pParticle = GetNextParticle( pParticles );

        if( ( pParticle->m_fStartTime + pParticle->m_fLifeSpan >= fCurrentTime * 0.001f ) || 
            ( pParticle->m_fLifeSpan < 0.0f ) )
        {
            FLOAT32      fAngle            = m_fEmitterSpread * ( FrmRand() - 0.5f );
            FRMMATRIX4X4 matRotate         = FrmMatrixRotate( FrmRadians(fAngle), 0.0f, 0.0f, -1.0f );
            FRMVECTOR3   vVelocity         = FrmVector3TransformCoord( m_vVelocity, matRotate );

            pParticle->m_vPosition         = m_vPosition + FrmSphrand( m_fEmitterRadius );
            pParticle->m_vVelocity         = vVelocity * ApplyVariance( 1.0f, m_fSpeedVariance );
            pParticle->m_vColor            = vColor;
            pParticle->m_vColor.a          = (UINT8)( 255 * ApplyVariance( m_fInitialOpacity, m_fOpacityVariance ) );
            pParticle->m_fStartTime        = fCurrentTime;
            pParticle->m_fLifeSpan         = ApplyVariance( m_fInitialLifeSpan, m_fLifeSpanVariance );
            pParticle->m_fInitialSize      = ApplyVariance( m_fInitialSize, m_fInitialSizeVariance );
            pParticle->m_fSizeIncreaseRate = ApplyVariance( m_fSizeIncreaseRate, m_fSizeIncreaseRateVarience );
        }
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

    InitParticles( m_Particles );
    
    for( UINT32 i = 0; i < NUM_EMITTERS; ++i )
        m_ParticleEmitters[i].Init();

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/PointSprites.pak" ) )
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
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0,   "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1,   "Decrease X Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2,   "Increase X Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3,   "Decrease Y Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4,   "Increase Y Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5,   "Decrease Speed" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6,   "Increase Speed" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7,   "Toggle Blend Mode" );

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
                                         m_Colors, m_fTimePerColor );
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

        FrmSetVertexLayout( m_ParticleVertexLayout, sizeof(PARTICLE), m_Particles );
        glDrawArrays( GL_POINTS, 0, NUM_PARTICLES );
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

