

//--------------------------------------------------------------------------------------
// File: PCFPS.hlsl
// Desc: Pixel shader for CascadedShadowMaps (PCF)
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#define PCF 1
//#define SHOW_SPLITS 1

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
    float4 vTexCoord[6] : TEXCOORD0;
};

half4 main(PixelShaderInput input) : SV_TARGET
{
    
    half shadow = 0.0;
    half4 color = input.vTexCoord[0];
    
    /////////////////////////////////
    // independent SHOW_SPLITS
    ////////////////////////////////
        
    // select split
    half sample_distance = 0.0;
    half2 sample_texcoord = input.vTexCoord[1].xy / input.vTexCoord[1].w;
    if(max(abs(sample_texcoord.x - 0.25),abs(sample_texcoord.y - 0.25)) < 0.24) {
        sample_distance = input.vTexCoord[5].x;
        #ifdef SHOW_SPLITS
            color = half4(1.0,0.0,0.0,1.0);
        #endif
    } else {
        sample_texcoord = input.vTexCoord[2].xy / input.vTexCoord[2].w;
        if(max(abs(sample_texcoord.x - 0.75),abs(sample_texcoord.y - 0.25)) < 0.24) {
            sample_distance = input.vTexCoord[5].y;
            #ifdef SHOW_SPLITS
                color = half4(0.0,1.0,0.0,1.0);
            #endif
        } else {
            sample_texcoord = input.vTexCoord[3].xy / input.vTexCoord[3].w;
            if(max(abs(sample_texcoord.x - 0.25),abs(sample_texcoord.y - 0.75)) < 0.24) {
                sample_distance = input.vTexCoord[5].z;
                #ifdef SHOW_SPLITS
                    color = half4(0.0,0.0,1.0,1.0);
                #endif
            } else {
                sample_texcoord = input.vTexCoord[4].xy / input.vTexCoord[4].w;
                if(max(abs(sample_texcoord.x - 0.75),abs(sample_texcoord.y - 0.75)) < 0.24) {
                    sample_distance = input.vTexCoord[5].w;
                    #ifdef SHOW_SPLITS
                        color = half4(1.0,1.0,1.0,1.0);
                    #endif
                } else {
                    shadow = 1.0;
                }
            }
        }
    }
    // calculate shadow
    if(shadow == 0.0) {
        half filterSize = 2.0f;
        half startOffset = (filterSize - 1.0f) / 2.0f;
        half texOffset = 1.0f / 1024.0f;

        half2 texCoord = sample_texcoord - (startOffset * texOffset);
        [unroll]
        for(int i = 0; i < filterSize; i++) 
        {
            [unroll]
            for(int j = 0; j < filterSize; j++) 
            {
                half4 sample = tTexture_8.Sample(sTexture_8, texCoord + half2(i*texOffset, j*texOffset));
                // 1 - (z/w) in texture, so convert to (z/w)
                sample = 1.0 - sample;                    
                shadow += half(sample.x > sample_distance - fBias);
            }
        }
        shadow /= (filterSize * filterSize);

    }
            
    return shadow * color;
}