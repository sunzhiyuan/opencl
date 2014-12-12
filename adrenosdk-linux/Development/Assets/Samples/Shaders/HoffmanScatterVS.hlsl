
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




cbuffer HoffmanScatterConstantBuffer
{
    float4x4 matModelView;
    float4x4 matModelViewProj;

    float4 EyePos;
    float4 SunDir;
    float4 BetaRPlusBetaM;
    float4 HGg;
    float4 BetaDashR;
    float4 BetaDashM;
    float4 OneOverBetaRPlusBetaM;
    float4 Multipliers;
    float4 SunColorAndIntensity;	
};

struct VertexShaderInput
{
    float3 vPositionOS : POSITION;
    float3 vTexCoord : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 vVertexPos : SV_POSITION;
    float3 vVSTexCoord : TEXCOORD0;
    float3 LightVec : TEXCOORD1;
    float3 Lin : TEXCOORD2;
    float3 Fex : TEXCOORD3;
};


PixelShaderInput main( VertexShaderInput input )
{
    PixelShaderInput vso;

    //          
    // Calculate view direction and distance    
    //                                                                                                 
    float3 vertex        = float3( mul(matModelView, float4(input.vPositionOS, 1.0) ).xyz );
    float3 viewDirection = normalize( vertex );
    float  distance      = length( vertex );

    //                                                                               
    // Angle between sun direction and view direction                               
    //                                                        
    float3 sunDirection = normalize( mul(matModelView, float4(SunDir.xyz, 0.0) ).xyz );    
    float  theta        = dot( sunDirection, viewDirection );                 

    //                                                
    // Phase1 and Phase2                                   
    //                                                               

    float phase1 = 1.0 + theta * theta;                                     
    float phase2 = pow( sqrt( HGg.y - HGg.z * theta ), 3.0 ) * HGg.x;       

    //                                             
    // Extinction term                                                         
    //                               

    float3 extinction      = exp( -BetaRPlusBetaM.xyz * distance );                       
    float3 totalExtinction = extinction * Multipliers.yzw;                            

    //                                                                                 
    // Inscattering term                                                   
    //                                                                    

    float3 betaRay = BetaDashR.xyz * phase1;                          
    float3 betaMie = BetaDashM.xyz * phase2;                  

    float3 inscatter = (betaRay + betaMie) * OneOverBetaRPlusBetaM.xyz * (1.0 - extinction);

    //                                                                 
    // Apply inscattering contribution factors               
    //                                                             

    inscatter *= Multipliers.x;           

    //                                              
    // Scale with sun color & intensity      
    //                                                    

    inscatter       *= SunColorAndIntensity.xyz * SunColorAndIntensity.w;         
    totalExtinction *= SunColorAndIntensity.xyz * SunColorAndIntensity.w;      

    vso.Lin = inscatter;
    vso.Fex = totalExtinction;
	
    //                                                                  
    // Transform vertex and set its colors                    
    //                                                                                                       
    float4 vPositionES = mul( matModelView, float4( input.vPositionOS, 1.) );
    float4 vPositionCS = mul( matModelViewProj, float4( input.vPositionOS, 1.) );


    // Pass everything off to the fragment shader
    vso.vVertexPos  = vPositionCS;
    
    vso.LightVec = sunDirection;
    vso.vVSTexCoord = input.vTexCoord;
    
    return vso;
}

