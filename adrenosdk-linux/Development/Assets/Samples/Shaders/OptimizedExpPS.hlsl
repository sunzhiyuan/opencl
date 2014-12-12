

//--------------------------------------------------------------------------------------
// File: OptimizedExp.hlsl
// Desc: Optimized pixel shader for CascadedShadowMaps
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

/******************************************************************************\
*
* Fragment shader
*
\******************************************************************************/
cbuffer CSMConstantBuffer
{
    float4x4 matModelViewProj;
    float4x4 matWorld;
    float4x4 modelviewprojections[4];
    float4x4 matNormal;
    float4   offsets[4];
    float4   shadowSpheres[4];
    float4   light;    
    float    fBias;
};

Texture2D tTexture_8 : register(t0);
SamplerState sTexture_8 : register(s0);


struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
    float4 vWorldSpace : TEXCOORD0;
};


half4 main(PixelShaderInput input) : SV_TARGET
{
    
// 1. calculate distance between two points
// 2. compare to the radius. If smaller it is in the sphere
// Original equation
// inside = (sqrt((WorldSpace.x - CenterOfSphere.x)^2 + (WorldSpace.y - CenterOfSphere.y)^2 + (WorldSpace.z - CenterOfSphere.z)^2) < RadiusOfSphere
// The equation is simplified like this
// inside = ((CenterOfSphere.x - WorldSpace.x)^2 + (CenterOfSphere.y - WorldSpace.y)^2 + (CenterOfSphere.z - WorldSpace.z)^2) < RadiusOfSphere^2
// For certain hardware platforms those are too many constants. Check out [Valient] for a solution that only utilizes texture coordinate registers

    half4 Dist;
    Dist.x = dot((input.vWorldSpace.xyz - shadowSpheres[0].xyz), (input.vWorldSpace.xyz - shadowSpheres[0].xyz));
    Dist.y = dot((input.vWorldSpace.xyz - shadowSpheres[1].xyz), (input.vWorldSpace.xyz - shadowSpheres[1].xyz));
    Dist.z = dot((input.vWorldSpace.xyz - shadowSpheres[2].xyz), (input.vWorldSpace.xyz - shadowSpheres[2].xyz));
    Dist.w = dot((input.vWorldSpace.xyz - shadowSpheres[3].xyz), (input.vWorldSpace.xyz - shadowSpheres[3].xyz));

// whatever comes first, pick that
// this is distance < radius
// to make this work with the simplified equation, radius comes in as radius ^2 in the w channel
    int mapToUse = (Dist.x < shadowSpheres[0].w) ? 0 :
                   (Dist.y < shadowSpheres[1].w) ? 1 :
                   (Dist.z < shadowSpheres[2].w) ? 2 :
                   (Dist.w < shadowSpheres[3].w) ? 3 : 4;//put back.. 4;

//just to debug..
    half4 colors[5];
    colors[0] = half4(1.0, 0.0, 0.0, 1.0);
    colors[1] = half4(0.0, 1.0, 0.0, 1.0);
    colors[2] = half4(1.0, 1.0, 0.0, 1.0);
    colors[3] = half4(0.0, 0.0, 1.0, 1.0);
    colors[4] = half4(1.0, 0.0, 1.0, 1.0);
    
    half shadow;
    
    //this is to get around if(mapToUse == 4) shadow = 1.0
    shadow = half(clamp( mapToUse - 3, 0, 1));
    clamp( mapToUse, 0, 3);
    
    // Get pixel depth from the point of view from the light camera
    half4 pos = mul(modelviewprojections[mapToUse], half4(input.vWorldSpace.xyz, 1.0));// mul(half4(WorldSpace.xyz, 1.0f), LightMatrix);

    // fetch shadow map depth value
    half depth = tTexture_8.Sample( sTexture_8, pos.xy).x;
    // 1 - (z/w) in texture, so convert to (z/w)
    depth = 1.0 - depth;
        

//	if(shadow == 0.0)
//		shadow = half(depth > pos.z - 0.004);

    if(shadow == 0.0)//Exponential Shadow Map
        shadow = clamp(2.0 - exp((pos.z - depth) * 100.0),0.0,1.0);

    return colors[ mapToUse ] * input.vColor * shadow;
}
