//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H


#define NUM_PARTICLES 2000
#define NUM_EMITTERS  1
#define NUM_COLORS    6


// Transform Feedback Particle Shader related data
struct EmitParticleShader
{
	GLuint	ShaderID;
	GLint 	slotvPosition;
	GLint 	slotvVelocity;
	GLint 	slotfEmitterSpread;
	GLint 	slotfEmitterRadius;
	GLint 	slotfSpeedVariance;
	GLint 	slotfInitialOpacity;
	GLint 	slotfOpacityVariance;
	GLint 	slotfInitialLifeSpan;
	GLint 	slotfLifeSpanVariance;
	GLint 	slotfInitialSize;
	GLint 	slotfInitialSizeVariance;
	GLint 	slotfSizeIncreaseRate;
	GLint 	slotfSizeIncreaseRateVariance;
	GLint 	slotfTime;
	GLint 	slotvColor;
	GLint	slotfEmissionRate;
};

//--------------------------------------------------------------------------------------
// Name: struct PARTICLE
// Desc: Attributes of an individual particle
//--------------------------------------------------------------------------------------
struct PARTICLE
{
    FRMVECTOR3 m_vPosition;          // The centers of the point sprite particles
    FRMVECTOR3 m_vVelocity;          // The velocities of the particles
    FRMVECTOR4 m_vColor;             // The colors of the particles
    FLOAT32    m_fStartTime;         // The times at which particles were born in seconds
    FLOAT32    m_fLifeSpan;          // How long in seconds particles should live for
    FLOAT32    m_fInitialSize;       // The initial size of the particles
    FLOAT32    m_fSizeIncreaseRate;  // The rate at which the particles size increases
};


//--------------------------------------------------------------------------------------
// Name: class CParticleEmitter
// Desc: Class for a particle emitter
//--------------------------------------------------------------------------------------
class CParticleEmitter
{
    FLOAT32    m_fEmissionRate;        // The number of particles to emit per second
    FLOAT32    m_fLeftOverParticles;   // Can't create 0.63 of a particle, so wait until the total is over 1
    FRMVECTOR3 m_vPosition;            // The center position of the emitter
    FRMVECTOR3 m_vVelocity;            // The direction and speed particles should be emitted in
    FLOAT32    m_fEmitterRadius;       // The radius of the emitter, the emitter is a sphere centered at position
    FLOAT32    m_fEmitterSpread;       // Degrees of spreed (0-360)
    FLOAT32    m_fInitialLifeSpan;     // How long particles that are emitted will live for
    FLOAT32    m_fLifeSpanVariance;    // A random variance that is added to the life span [-0.5*lsv, 0.5*lsv]
    FLOAT32    m_fSpeedVariance;       // A random variance that is added to the speed [-0.5*sv, 0.5*sv]
    FLOAT32    m_fInitialSize;         // The size that the particles are emitted with
    FLOAT32    m_fSizeIncreaseRate;    // The rate at which the particles grow in size
    FLOAT32    m_fInitialSizeVariance; // The random variance that is added to the initial size
    FLOAT32    m_fSizeIncreaseRateVarience; // The varience of the rate of particle growth
    FLOAT32    m_fInitialOpacity;      // The opacity with which particles are emitted with
    FLOAT32    m_fOpacityVariance;     // A random variance that is added to the opacity [-0.5ov, 0.5ov]
    INT32      m_iParticleIndex;       // Current particle index

public:
    VOID      Init();
    PARTICLE* GetNextParticle( PARTICLE* pParticles );
    
    VOID UpdateEmitterPosition( const FRMVECTOR3& vPosition );
    VOID UpdateEmitterVelocity( const FRMVECTOR3& vVelocity );
    VOID EmitParticles( PARTICLE* pParticles, FLOAT32 fElapsedTime, FLOAT32 fCurrentTime,
                        FRMCOLOR Colors[NUM_COLORS], FLOAT32 fTimePerColor,
						EmitParticleShader *epShader,
						GLuint *srcBuffer, GLuint *srcVAO, GLuint *dstBuffer,
						FRM_VERTEX_ELEMENT* pVertexLayout,
                        UINT32 nVertexSize, BOOL bUseTransformFeedback, GLuint Query );
};


//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:
    CSample( const CHAR* strName );

    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

private:
    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    // Shader related data
    GLuint             m_hParticleShader;
    GLint              m_slotModelViewProjMatrix;
    GLint              m_slotTime;

    CFrmTexture*       m_pLogoTexture;
    CFrmTexture*       m_pParticleTexture;

    FRM_VERTEX_ELEMENT m_ParticleVertexLayout[16];

    FRMMATRIX4X4       m_matProj;

    // Particle related data
    CParticleEmitter   m_ParticleEmitters[NUM_EMITTERS];
    PARTICLE           m_Particles[NUM_PARTICLES];
    FRMCOLOR           m_Colors[NUM_COLORS];
    FLOAT32            m_fTimePerColor;
    FLOAT32            m_fParticleTimeX;
    FLOAT32            m_fParticleTimeY;

	// Transform Feedback Particle related data
	GLuint			   m_hQuery;
	GLuint			   m_hParticleVAOs[2];
	GLuint			   m_hParticleBuffers[2];
	EmitParticleShader m_emitParticleShader;
	INT32			   m_srcBuffer;
	INT32			   m_dstBuffer;

    // User options
    FLOAT32            m_fXFactor;
    FLOAT32            m_fYFactor;
    FLOAT32            m_fParticleSpeed;
    BOOL               m_bUseSrcAlpha;
	BOOL               m_bUseTransformFeedback;

    BOOL InitShaders();
};


#endif // SCENE_H
