
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[HoffmanScatter30VS]

#version 300 es

#include "CommonVS.glsl"

uniform vec3	EyePos;
uniform vec3    SunDir;
uniform vec3    BetaRPlusBetaM;
uniform vec3    HGg;
uniform vec3    BetaDashR;
uniform vec3    BetaDashM;
uniform vec3    OneOverBetaRPlusBetaM;
uniform vec4    Multipliers;
uniform vec4    SunColorAndIntensity;

uniform  mediump mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;

in vec3 g_vPositionOS;
in vec3 g_vTexCoord;

out		vec3 g_vVSTexCoord;
out     vec3 LightVec;

out vec3	Lin;
out vec3	Fex;


void main()
{
    //          
    // Calculate view direction and distance    
	//                                                                                                 
	vec3 vertex        = vec3( g_matModelView * vec4(g_vPositionOS, 1.0) ).xyz;
    vec3 viewDirection = normalize( vertex );
    float  distance      = length( vertex );

	//                                                                               
	// Angle between sun direction and view direction                               
	//                                                        
	vec3 sunDirection = normalize( vec4( g_matModelView * vec4(SunDir, 0.0) ).xyz );    
	float  theta        = dot( sunDirection, viewDirection );                 

	//                                                
	// Phase1 and Phase2                                   
	//                                                               

	float phase1 = 1.0 + theta * theta;                                     
	float phase2 = pow( sqrt( HGg.y - HGg.z * theta ), 3.0 ) * HGg.x;       

	//                                             
	// Extinction term                                                         
	//                               

	vec3 extinction      = exp( -BetaRPlusBetaM * distance );                       
	vec3 totalExtinction = extinction * Multipliers.yzw;                            

	//                                                                                 
	// Inscattering term                                                   
	//                                                                    

	vec3 betaRay = BetaDashR * phase1;                          
	vec3 betaMie = BetaDashM * phase2;                  

	vec3 inscatter = (betaRay + betaMie) * OneOverBetaRPlusBetaM * (1.0 - extinction);

	//                                                                 
	// Apply inscattering contribution factors               
	//                                                             

	inscatter *= Multipliers.x;           

	//                                              
	// Scale with sun color & intensity      
	//                                                    

	inscatter       *= SunColorAndIntensity.xyz * SunColorAndIntensity.w;         
	totalExtinction *= SunColorAndIntensity.xyz * SunColorAndIntensity.w;      

	Lin = inscatter;
	Fex = totalExtinction;
	
	//                                                                  
	// Transform vertex and set its colors                    
	//                                                                                                       
    vec4 vPositionES = g_matModelView     * vec4( g_vPositionOS, 1.);
    vec4 vPositionCS = g_matModelViewProj * vec4( g_vPositionOS, 1.);


    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    
    LightVec = sunDirection;
    g_vVSTexCoord = g_vTexCoord;
    

}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[HoffmanScatter30FS]
#version 300 es
#include "CommonFS.glsl"


in vec3	Lin;
in vec3	Fex;

out vec4 g_FragColor;

void main()
{
	g_FragColor.rgba  = vec4(Lin + vec3(0.01,0.,0.2) , 1.) ;
}
