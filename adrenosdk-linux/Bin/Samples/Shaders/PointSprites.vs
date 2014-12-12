

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



// Uniform shader constants 
uniform   mat4   g_matModelViewProj;
uniform   float  g_fTime;

// Input per-vertex attributes
attribute vec3   g_vPosition;
attribute vec3   g_vVelocity;
attribute vec4   g_vColor;
attribute float  g_fStartTime;
attribute float  g_fLifeSpan;
attribute float  g_fInitialSize;
attribute float  g_fSizeIncreaseRate;

// Varying per-vertex outputs
varying   vec4   g_vOutColor;


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


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
