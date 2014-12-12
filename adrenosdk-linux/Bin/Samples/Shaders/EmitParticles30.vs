

//--------------------------------------------------------------------------------------
// File: PointSprites.glsl
// Desc: Shaders for the PointSprites sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

#version 300 es

//--------------------------------------------------------------------------------------
// File: CommonVS.glsl
// Desc: Useful common shader code for vertex shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



#define ATTR_VPOSITION			0
#define ATTR_VVELOCITY			1
#define ATTR_VCOLOR				2
#define ATTR_FSTARTTIME			3
#define ATTR_FLIFESPAN			4
#define ATTR_FINITIALSIZE		5
#define ATTR_FSIZEINCREASERATE	6

// Uniform shader constants (Emitter properties)
uniform vec3	u_vPosition;
uniform vec3	u_vVelocity;
uniform float	u_fEmitterSpread;
uniform float	u_fEmitterRadius;
uniform float	u_fSpeedVariance;
uniform float	u_fInitialOpacity;
uniform float	u_fOpacityVariance;
uniform float	u_fInitialLifeSpan;
uniform float	u_fLifeSpanVariance;
uniform float	u_fInitialSize;
uniform float	u_fInitialSizeVariance;
uniform float	u_fSizeIncreaseRate;
uniform float	u_fSizeIncreaseRateVariance;
uniform float	u_fTime;
uniform vec4	u_vColor;
uniform float	u_fEmissionRate;

// Input per-particle attributes
layout(location = ATTR_VPOSITION)			in vec3   g_vPosition;
layout(location = ATTR_VVELOCITY)			in vec3   g_vVelocity;
layout(location = ATTR_VCOLOR)				in vec4   g_vColor;
layout(location = ATTR_FSTARTTIME)			in float  g_fStartTime;
layout(location = ATTR_FLIFESPAN)			in float  g_fLifeSpan;
layout(location = ATTR_FINITIALSIZE)		in float  g_fInitialSize;
layout(location = ATTR_FSIZEINCREASERATE)	in float  g_fSizeIncreaseRate;

// Varying per-particle outputs
out vec3   o_vPosition;
out vec3   o_vVelocity;
out vec4   o_vColor;
out float  o_fStartTime;
out float  o_fLifeSpan;
out float  o_fInitialSize;
out float  o_fSizeIncreaseRate;

out vec4 glPosition;

const float UINT_MAX = 4294967295.0;
const float DEG_TO_RAD = 0.0174532925;

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
    return float(randhash(seed)) / UINT_MAX;
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

void main()
{
	uint seed = uint(u_fTime * 1000.0) + uint(gl_VertexID);
	
	if( randhashf(seed++) < u_fEmissionRate )
	{
		float fAngle = DEG_TO_RAD * u_fEmitterSpread * ( randhashf(seed++) - 0.5 );
		float cosTerm = cos(fAngle);
		float sinTerm = sin(fAngle);

		mat3 rotateZ = mat3( cosTerm, sinTerm, 0,
							-sinTerm, cosTerm, 0,
							0, 0, 1 );
		vec3 vVelocity = rotateZ*u_vVelocity;
		
		vec3 randVec;
		randVec.x = 2.0 * randhashf(seed++) - 1.0;
		randVec.y = 2.0 * randhashf(seed++) - 1.0;
		randVec.z = 2.0 * randhashf(seed++) - 1.0;
		randVec = u_fEmitterRadius * normalize(randVec);

		o_vPosition = u_vPosition + randVec;
		o_vVelocity = vVelocity * ApplyVariance( seed++, 1.0, u_fSpeedVariance );
		o_vColor = u_vColor;
		o_vColor.a = ApplyVariance( seed++, u_fInitialOpacity, u_fOpacityVariance );
		o_fStartTime = u_fTime;
		o_fLifeSpan = ApplyVariance( seed++, u_fInitialLifeSpan, u_fLifeSpanVariance );
		o_fInitialSize = ApplyVariance( seed++, u_fInitialSize, u_fInitialSizeVariance );
		o_fSizeIncreaseRate = ApplyVariance( seed++, u_fSizeIncreaseRate, u_fSizeIncreaseRateVariance );
	}
	else
	{
		// Pass through data because not re-emitting particle
		o_vPosition			= g_vPosition;
		o_vVelocity			= g_vVelocity;
		o_vColor			= g_vColor;
		o_fStartTime		= g_fStartTime;
		o_fLifeSpan			= g_fLifeSpan;
		o_fInitialSize		= g_fInitialSize;
		o_fSizeIncreaseRate	= g_fSizeIncreaseRate;
	}
	
	gl_Position = vec4(o_vPosition, 1.0);
}

