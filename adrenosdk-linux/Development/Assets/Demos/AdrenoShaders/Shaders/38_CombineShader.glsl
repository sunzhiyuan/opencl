//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CombineShader38VS]
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
[CombineShader38FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_SharpTexture;
uniform sampler2D g_BlurredTexture;
uniform float g_BloomIntensity;
uniform float g_Contrast;
uniform float g_Saturation;
uniform vec3 g_ColorCorrect;
uniform vec3 g_ColorAdd;


void main()
{
    vec4 FocusPixel = texture2D( g_SharpTexture, g_TexCoord );
    vec4 BlurPixel = texture2D( g_BlurredTexture, g_TexCoord );
    
    vec4 Color = clamp(FocusPixel + (BlurPixel * g_BloomIntensity), 0.0, 1.0);

	// color filters
	Color.xyz = Color.xyz - g_Contrast * (Color.xyz - 1.0) * Color.xyz *(Color.xyz - 0.5);
	
	// saturation
	float Lum = dot(Color.xyz, vec3(0.2126, 0.7152, 0.0722));
	Color.xyz = mix(vec3(Lum, Lum, Lum), Color.xyz, g_Saturation);
	
	// color correction
	Color.xyz = Color.xyz * g_ColorCorrect * vec3(2.0,2.0,2.0) + g_ColorAdd;
    
    gl_FragColor = Color;
}
