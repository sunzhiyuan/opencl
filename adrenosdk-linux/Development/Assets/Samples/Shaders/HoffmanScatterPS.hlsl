
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

struct PixelShaderInput
{
    float4 vVertexPos : SV_POSITION;
    float3 vVSTexCoord : TEXCOORD0;
    float3 LightVec : TEXCOORD1;
    float3 Lin : TEXCOORD2;
    float3 Fex : TEXCOORD3;
};


float4 main( PixelShaderInput input ) : SV_TARGET
{
    return float4(input.Lin + float3(0.01,0.,0.2) , 1.) ;
}

