

//--------------------------------------------------------------------------------------
// File: PointSpritesVS.hlsl
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
// Uniform shader constants 
cbuffer PointSpritesConstantBuffer
{
    float4x4 matModelViewProj;
    float    fTime;
};

struct VertexShaderInput
{
    float3 vPosition : POSITION;
    float3 vVelocity : TEXCOORD0;
    float4 vColor : COLOR;
    float  fStartTime : TEXCOORD1;
    float  fLifeSpan : TEXCOORD2;
    float  fInitialSize : TEXCOORD3;
    float  fSizeIncreaseRate : TEXCOORD4;
    float2 vTexCoord : TEXCOORD5;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float4 vOutColor : COLOR;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

    vso.vTexCoord = input.vTexCoord;

    float fAge = fTime - input.fStartTime;

    if( fAge <= input.fLifeSpan )
    {
        // Compute particle center to get size
        float3 vCenterPositionWS = input.vPosition + input.vVelocity * fAge;
        float4 vCenterPositionCS = mul( matModelViewProj, float4( vCenterPositionWS, 1.0 ) );

        float fDepthScaleFactor = 2.0 * vCenterPositionCS.z / vCenterPositionCS.w;        
        float fSize = input.fInitialSize + fAge * fDepthScaleFactor * input.fSizeIncreaseRate;

        // Extrude in object space.  Use the texture coordinate to determine which
        // direction to extrude the vertex
        float3 vPosition = input.vPosition;
        float2 vExtrudeDir = input.vTexCoord - 0.5f;
        vPosition.xy += vExtrudeDir * (fSize / 250.0);

        float3 vPositionWS = vPosition + input.vVelocity * fAge;
        float4 vPositionCS = mul( matModelViewProj, float4( vPositionWS, 1.0 ) );
        
        vso.vPosition = vPositionCS;
                
        float fAlpha = input.vColor.a * ( 1.0 - ( fAge / input.fLifeSpan ) );
        vso.vOutColor.rgb = fAlpha * input.vColor.rgb;
        vso.vOutColor.a   = fAlpha;
    }
    else
    {
        vso.vPosition = float4( 1000.0, 0.0, 0.0, 0.0 ); // Move dead particles off screen
        vso.vOutColor = float4( 0.0, 0.0, 0.0, 0.0 );
    }

    return vso;
}


