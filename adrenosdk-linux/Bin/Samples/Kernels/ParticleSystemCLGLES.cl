//--------------------------------------------------------------------------------------
// File: ParticleSystemCLGLES.cl
// Desc: Emits particles into buffer used as OpenGL ES VBO
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// Particle structure
typedef struct
{
    float4 vPosition;
    float4 vVelocity;
    float4 vColor;
    float  fStartTime;
    float  fLifeSpan;
    float  fInitialSize;
    float  fSizeIncreaseRate;
} Particle  __attribute__ ((aligned (16))); 

// Particle constants
typedef struct
{
    float4 vPosition;
    float4 vVelocity;
    float fEmitterRadius;
    float fEmitterSpread;
    float fInitialLifeSpan;
    float fLifeSpanVariance;
    float fSpeedVariance;
    float fInitialSize;
    float fSizeIncreaseRate;
    float fInitialSizeVariance;
    float fSizeIncreaseRateVariance;
    float fInitialOpacity;
    float fOpacityVariance;
    float fTime;
    float4 vColor;
    float fEmissionRate;
    float fPadding[3];

} ParticleConstants  __attribute__ ((aligned (16)));

#define FUINT_MAX (4294967295.0f)
#define DEG_TO_RAD (0.0174532925f)

// Generate a random uint
uint randhash(uint seed)
{
    uint i=(seed^12345391u)*2654435769u;
    i^=(i<<6u)^(i>>26u);
    i*=2654435769u;
    i+=(i<<5u)^(i>>12u);
    return i;
}

// Generate a random float [0.0, 1.0]
float randhashf(uint seed)
{
    return ((float)randhash(seed)) / FUINT_MAX;
}

//--------------------------------------------------------------------------------------
// Name: ApplyVariance()
// Desc: Applies the specified amount of random variance to the specified value.
// Ported to GLSL
//--------------------------------------------------------------------------------------
float ApplyVariance( uint seed, float fValue, float fVariance )
{
    return (fValue - 0.5 * fVariance + randhashf(seed) * fVariance);
}

//--------------------------------------------------------------------------------------
// Name: EmitParticles()
// Desc: Emit particles into a buffer used as an OpenGL ES VBO
//--------------------------------------------------------------------------------------
__kernel void EmitParticles( __global Particle* pInput,
                             __global Particle* pOutput,
                             __constant ParticleConstants* pUniforms )
{
    uint seed = ((uint)(pUniforms->fTime * 1000.0f)) + get_global_id(0);
    int i = get_global_id(0);
    __global Particle* pIn = &pInput[i];
    __global Particle* pOut = &pOutput[i];

    if( ( ( pIn->fStartTime + pIn->fLifeSpan >= pUniforms->fTime * 0.001f ) ||
        ( pIn->fLifeSpan < 0.0 ) ) &&
        ( randhashf(seed++) < pUniforms->fEmissionRate ) )
    {
        float fAngle = DEG_TO_RAD * pUniforms->fEmitterSpread * ( randhashf(seed++) - 0.5 );
        float cosTerm = cos(fAngle);
        float sinTerm = sin(fAngle);

        float4 vVelocity;
        vVelocity.x = cosTerm * pUniforms->vVelocity.x + sinTerm * pUniforms->vVelocity.y;
        vVelocity.y = -sinTerm * pUniforms->vVelocity.x + cosTerm * pUniforms->vVelocity.y;
        vVelocity.z = pUniforms->vVelocity.z;
        vVelocity.w = 0.0f;

        
        float4 randVec;
        randVec.x = 2.0f * randhashf(seed++) - 1.0f;
        randVec.y = 2.0f * randhashf(seed++) - 1.0f;
        randVec.z = 2.0f * randhashf(seed++) - 1.0f;
        randVec.w = 0.0f;

        randVec = pUniforms->fEmitterRadius * normalize(randVec);
        pOut->vPosition = pUniforms->vPosition + randVec;
        
        pOut->vVelocity = vVelocity * ApplyVariance( seed++, 1.0f, pUniforms->fSpeedVariance );
        pOut->vColor = pUniforms->vColor;
        pOut->vColor.w = ApplyVariance( seed++, pUniforms->fInitialOpacity, pUniforms->fOpacityVariance );
        pOut->fStartTime = pUniforms->fTime;
        pOut->fLifeSpan = ApplyVariance( seed++, pUniforms->fInitialLifeSpan, pUniforms->fLifeSpanVariance );
        pOut->fInitialSize = ApplyVariance( seed++, pUniforms->fInitialSize, pUniforms->fInitialSizeVariance );
        pOut->fSizeIncreaseRate = ApplyVariance( seed++, pUniforms->fSizeIncreaseRate, pUniforms->fSizeIncreaseRateVariance );
    }
    else
    {
        // Pass through data because not re-emitting particle
        pOut->vPosition = pIn->vPosition;
        pOut->vVelocity = pIn->vVelocity;
        pOut->vColor = pIn->vColor;
        pOut->fStartTime = pIn->fStartTime;
        pOut->fLifeSpan = pIn->fLifeSpan;
        pOut->fInitialSize = pIn->fInitialSize;
        pOut->fSizeIncreaseRate = pIn->fSizeIncreaseRate;
    }
}
