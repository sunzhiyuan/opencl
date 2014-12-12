//--------------------------------------------------------------------------------------
// File: PreethamScatter.glsl
// Desc: 
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



uniform vec3    SunDir;
uniform vec2    a_thetaSun;
uniform vec3    a_zenithDirection;
uniform vec3    a_zenithColor;
uniform vec3    a_A;
uniform vec3    a_B;
uniform vec3    a_C;
uniform vec3    a_D;
uniform vec3    a_E;
uniform mat3    a_colorConvMat;
uniform float   a_exposure; 

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;

attribute vec3 g_vPositionOS;
attribute vec3 g_vTexCoord;

varying		vec3 g_vVSTexCoord;
varying     vec3 LightVec;

varying vec3	vColor;

void main()
{
	vec3 vertex        = vec3( g_matModelView * vec4(g_vPositionOS, 0.0) ).xyz;
    vec3 direction     = normalize( vertex );
	
	vec3 sunVec = vec3( g_matModelView * vec4(SunDir, 0.0) ).xyz;
	sunVec = normalize( sunVec );
	
	float theta     = abs(dot( a_zenithDirection, direction ));
	float gamma     = abs(dot( sunVec, direction )); 
	float cos2gamma = gamma * gamma;
	gamma		    = acos( gamma ); 
	                                                                                
	//                                                                     
	// Compute sky chromaticity values and sky luminance                           
	//                                                                   

	vec3 num = (1.0 + a_A * exp( a_B / theta )) * (1.0 + a_C * exp( a_D * gamma ) + a_E * cos2gamma);
	vec3 den = (1.0 + a_A * exp( a_B )) * (1.0 + a_C * exp( a_D * a_thetaSun.x ) + a_E * a_thetaSun.y);//a_thetaSun.y);
	vec3 xyY = num / den * a_zenithColor; 
	
	//
	// Exposure function for luminance
	//                                                                                                     
 
	xyY.z = 1.0 - exp( -a_exposure * xyY.z );                                                              
                                                                                                            
	//                                                                                                     
	// Convert CIE xyY to CIE XYZ                                                                          
	//                                                                                                     
	                                                                                                       
	vec3 XYZ;                                                                                           
	                                                                                                         
	XYZ.x = (xyY.x / xyY.y) * xyY.z;
	XYZ.y = xyY.z;
	XYZ.z = ((1.0 - xyY.x - xyY.y) / xyY.y) * xyY.z;                                                        


// XYZ -> RGB conversion
	// The official XYZ to sRGB conversion matrix is (following ITU-R BT.709)
	//  3.2410 -1.5374 -0.4986
	// -0.9692  1.8760  0.0416
	//  0.0556 -0.2040  1.0570		
	const mat3 XYZ2RGB  = mat3( 2.5651, -1.1665, -0.3986,
							   -1.0217,  1.9777,  0.0439, 
							    0.0753, -0.2543,  1.1892);
							    
	vColor = XYZ2RGB * XYZ; // a_colorConvMat * XYZ;
	
	//                                                                                                    
	// Transform vertex
	//                                                                                                      

    vec4 vPositionCS = g_matModelViewProj * vec4( g_vPositionOS, 1.);


    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    
    LightVec = sunVec;
    g_vVSTexCoord = g_vTexCoord;
    
}


//--------------------------------------------------------------------------------------
// The fragment shader
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



uniform vec3    SunDir;
uniform vec2    a_thetaSun;
uniform vec3    a_zenithDirection;
uniform vec3    a_zenithColor;
uniform vec3    a_A;
uniform vec3    a_B;
uniform vec3    a_C;
uniform vec3    a_D;
uniform vec3    a_E;
uniform mat3    a_colorConvMat;
uniform float   a_exposure; 

uniform vec2    a_bins;
uniform vec2    a_alpha;
uniform vec3    a_S0Mie[36];
uniform vec3    a_S0Ray[36];
uniform vec3    a_constants;
uniform vec3    a_C1plusC2;
uniform vec3    a_betaMie;
uniform vec3    a_betaRay;

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;

attribute vec3 g_vPositionOS;
attribute vec3 g_vTexCoord;

varying		vec3 g_vVSTexCoord;
varying     vec3 LightVec;

varying vec3	Inscatter;
varying vec3	Extinction;

float evalFunc( float a_B, float a_s )                                                                      
{
	float result; 
	
	if( abs( a_B * a_s ) < 0.01 ) 
		result = a_s;
	else
		result = (1.0 - exp( -a_B * a_s )) / a_B;
		
		return result;
}

void main()
{
	const float epsilon = .0001;

	vec3 vertex        = g_vPositionOS.xyz;//vec3( g_matModelView * vec4(g_vPositionOS, 1.0) ).xyz;
    vec3 direction     = normalize( vertex );
    
    vec3 sunVec = SunDir.xyz;//vec3( g_matModelView * vec4(SunDir, 0.0) ).xyz;
	sunVec = normalize( sunVec );
	
    float thetav = ((-direction.z - 1.0) / 2.0) * a_bins.x - epsilon;
    thetav = max( thetav, 0.0 );  
    
    vec2 dirTemp   = normalize( vec2( direction.x, direction.z ) );
                                                                                                  
	if( dirTemp.y < 0.0 )                                                                             
		dirTemp.x = -(dirTemp.x + 2.0);                                                                 
                                                                                                        
        float phiv = ((-dirTemp.x - 1.0) / 4.0) * a_bins.x - epsilon;                                         
              phiv = max( phiv, 0.0 );                                                                         
                                                                                                              
        float i = thetav;                                                                                        
        float u = thetav - i;                                                                                 
                                                                                                              
		float j = phiv;                                                                                        
        float v = phiv - j;                                                                                   
                                                                                                               
        ivec4 indices;                                                                                           
        indices.x = int(i * a_bins.y + j);                                                                           
        indices.y = int((i + 1.0) * a_bins.y + j);                                                                     
        indices.z = int(i * a_bins.y + j + 1.0);                                                                       
        indices.w = int((i + 1.0) * a_bins.y + j + 1.0);                                                                 
                                                                                                                
        vec4 factors;                                                                                         
        factors.x = (1.0 - u) * (1.0 - v);                                                                      
        factors.y = u * (1.0 - v);                                                                               
        factors.z = (1.0 - u) * v;                                                                               
        factors.w = u * v;                                                                                       
                                                                                                     
        vec3 S0Mie = factors.x * a_S0Mie[ indices.x ]                                                          
                    + factors.y * a_S0Mie[ indices.y ]                                                          
                    + factors.z * a_S0Mie[ indices.z ]                                                          
                    + factors.w * a_S0Mie[ indices.w ];                                                         
        vec3 S0Ray = factors.x * a_S0Ray[ indices.x ]                                                          
                    + factors.y * a_S0Ray[ indices.y ]                                                          
                    + factors.z * a_S0Ray[ indices.z ]                                                          
                    + factors.w * a_S0Ray[ indices.w ];                                                         
                                                                                                                
       float s = length( vertex );                                                                             
                                                                                                                
       float B1 = a_alpha.x * direction.z;                                                                      
       float B2 = a_alpha.y * direction.z;                                                                      
                                                                                                                
       vec3 IMie = a_constants.x * ((1.0 - exp( -(B1 + a_C1plusC2) * s) ) / (B1 + a_C1plusC2));               
       vec3 IRay = a_constants.y * ((1.0 - exp( -(B2 + a_C1plusC2) * s) ) / (B2 + a_C1plusC2));               
                                                                                                                
       vec3 Lin = S0Mie * IMie + S0Ray * IRay;                                                                
                                                                                                                
       Lin *= a_constants.z;                                                                                    
                                                                                                                
       //                                                                                                       
       // Extinction                                                                                            
       //                                                                                                       
                                                                                                                                                                                                       
       vec3 Fex = exp( -a_betaMie * a_constants.x * evalFunc( B1, s ) ) *                                     
                    exp( -a_betaRay * a_constants.y * evalFunc( B2, s ) );                                      
        
        
      const mat3 XYZ2RGB  = mat3( 2.5651, -1.1665, -0.3986,
							   -1.0217,  1.9777,  0.0439, 
							    0.0753, -0.2543,  1.1892);
							    
	//  vColor = XYZ2RGB * XYZ; // a_colorConvMat * XYZ;
	
	  //                                                                                                    
	  // Transform vertex
	  //                                                                                                      

      vec4 vPositionCS = g_matModelViewProj * vec4( g_vPositionOS, 1.0);


      // Pass everything off to the fragment shader
      gl_Position  = vPositionCS;
    
      LightVec = (g_matModelView * vec4( sunVec, 0.0 )).xyz;
      g_vVSTexCoord = g_vTexCoord;
      
      Inscatter = XYZ2RGB * Lin;
   //Inscatter *= vec3(0.1, 0.1,0.1);
      Extinction = XYZ2RGB * Fex;
    //  Extinction *= vec3(1.5, 1.5, 1.5);

}
