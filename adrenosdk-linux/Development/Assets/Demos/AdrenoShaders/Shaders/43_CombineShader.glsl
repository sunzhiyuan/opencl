//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CombineShader43VS]
#include "CommonVS.glsl"

attribute vec4 g_Vertex;

varying vec2 g_TexCoord;

void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CombineShader43FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;
//uniform vec2 g_StepSize;
uniform sampler2D g_SharpTexture;
uniform float g_WarpAmount;

void main()
{
	// this is the center point we want to warp
	// if you want to mimic an huge explosion this point should be where the explosion happens in 
	// screen-space
	vec2 warpingPoint = vec2(.5, .5); 
	
	// center the texture coordinates around the warping point
	vec2 currentTexCoord = g_TexCoord - warpingPoint; 
	
	// measure the distance to the center
	float dist = length(currentTexCoord); 
   
    // how much sphere should the warp have?
	float amount = g_WarpAmount;
	
	// use a power curve to warp the texture coordinates
    // by adding the warping point, move the texture coordinate back into 
    // its original texture space
    // 0.7 is a magic value
	vec2 newTexCoord = .7 * pow(dist, amount) * currentTexCoord + warpingPoint;
  
	// fetch with new texture coordinates
	vec4 FocusPixel = texture2D( g_SharpTexture, newTexCoord);
   
    gl_FragColor = FocusPixel;
}
