

//--------------------------------------------------------------------------------------
// File: PointSprites30.glsl
// Desc: Shaders for the Particle System sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

[PointSprites30VS]

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
#version 300 es

#include "CommonVS.glsl"

// Uniform shader constants 
uniform   mat4   g_matModelViewProj;
uniform   float  g_fTime;

// Input per-vertex attributes
in vec3   g_vPosition;
in vec3   g_vVelocity;
in vec4   g_vColor;
in float  g_fStartTime;
in float  g_fLifeSpan;
in float  g_fInitialSize;
in float  g_fSizeIncreaseRate;

// Varying per-vertex outputs
out   vec4   g_vOutColor;

void main()
{
    float fAge = g_fTime - g_fStartTime;

    if( fAge <= g_fLifeSpan )
    {
        vec3 vPositionWS = g_vPosition + g_vVelocity * fAge;
        vec4 vPositionCS = g_matModelViewProj * vec4( vPositionWS, 1.0 );        
        gl_Position = vPositionCS;
        
        float fDepthScaleFactor = 2.0 * vPositionCS.z / vPositionCS.w;
        gl_PointSize    = g_fInitialSize + fAge * fDepthScaleFactor * g_fSizeIncreaseRate;

        float fAlpha = g_vColor.a * ( 1.0 - ( fAge / g_fLifeSpan ) );
        g_vOutColor.rgb = fAlpha * g_vColor.rgb;
        g_vOutColor.a   = fAlpha;
    }
    else
    {
        gl_Position = vec4( 1000.0, 0.0, 0.0, 0.0 ); // Move dead particles off screen
        gl_PointSize = 0.0;
    }	
}

[PointSprites30FS]

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
#version 300 es

#include "CommonFS.glsl"

uniform sampler2D g_TextureSampler;

in vec4      g_vOutColor;
out vec4 g_FragColor;


void main()
{
   float fIntensity = texture( g_TextureSampler, gl_PointCoord.xy ).r;   
    
   g_FragColor = fIntensity * g_vOutColor;	
	
}

