//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once
#ifndef BICUBIC__H
#define BICUBIC__H

#include "FrmMath.h"

// resolution of the lookup tables
#define WEIGHT_TEX_RES	128

// choose one of the following options for 16-bit integer, 16-bit float oder 32-bit float (first and last work fine)
//#define BICUBIC_FORMAT	GL_RGBA16UI
#define BICUBIC_FORMAT	GL_RGBA16F
//#define BICUBIC_FORMAT	GL_RGBA32F

typedef unsigned short FLOAT16;

// From Ryan's CompressedMesh source
FLOAT16 CompressFloatToHalfFloat( float v )
{
    UINT32 src = *(unsigned long*)(&v);
    UINT32 sign = src >> 31;

    // Extract mantissa
    UINT32 mantissa = src  &  (1 << 23) - 1;

    // extract & convert exp bits
    long exp = (long)( src >> 23  &  0xFF ) - 127;
    if( exp > 16 )
    {
        // largest possible number...
        exp = 16;
        mantissa = (1 << 23) - 1;
    }
    else if( exp <= -15 )
    {
        // handle wraparound of shifts (done mod 64 on PPC)
        // All float32 denorm/0 values map to float16 0
        if( exp <= - 14 - 24 )
            mantissa = 0;
        else
            mantissa = ( mantissa | 1 << 23 )  >>  (-exp - 14);
        exp = -15;
    }

    return (FLOAT16)(sign<<15) | (FLOAT16)((exp+15)<<10) | (FLOAT16)(mantissa>>13);
}


void Float32ToFloat16Array( FLOAT16 *dstF16, FLOAT32 *srcF32, INT n )
{
	for( int i=0; i<n; i++ )
		dstF16[i] = CompressFloatToHalfFloat( srcF32[i] );
}


// computes weights for cubic interpolation of the height map
float R( float x )
{
	float v, t;
	t  = max( 0.0f, x + 2.0f );
	v  = t * t * t;
	t  = max( 0.0f, x + 1.0f );
	v -= 4.0f * t * t * t;
	t  = max( 0.0f, x + 0.0f );
	v += 6.0f * t * t * t;
	t  = max( 0.0f, x - 1.0f );
	v -= 4.0f * t * t * t;
	v /= 6.0f;
	return v;
}

// create & compute the textures
int generateBiCubicWeights( CFrmTexture **ppBicubicWeight03,
							CFrmTexture **ppBicubicWeight47,
							CFrmTexture **ppBicubicWeight8B,
							CFrmTexture **ppBicubicWeightCF )
{
	int	i, j;
   
	// !hack!
	FRMVECTOR4  *rFunc		= new FRMVECTOR4[ WEIGHT_TEX_RES * WEIGHT_TEX_RES ];
	WORD		*rFunc16	= new WORD[ WEIGHT_TEX_RES * WEIGHT_TEX_RES * 4 ];
	FLOAT16		*rFunc16F	= new FLOAT16[ WEIGHT_TEX_RES * WEIGHT_TEX_RES * 4 ];

	for ( int n = -1; n <= 2; n++ )
	{
		for ( j = 0; j < WEIGHT_TEX_RES; j++ )
			for ( i = 0; i < WEIGHT_TEX_RES; i++ )
			{
				float dx = (float)(i+0) / (float)(WEIGHT_TEX_RES), dy = (float)(j+0) / (float)(WEIGHT_TEX_RES);

				FRMVECTOR4 r;
				// m = -1..+2, n = -1
				r.x = R( -1.0f - dx ) * R( dy - (float)n );
				r.y = R( +0.0f - dx ) * R( dy - (float)n );
				r.z = R( +1.0f - dx ) * R( dy - (float)n );
				r.w = R( +2.0f - dx ) * R( dy - (float)n );

				Float32ToFloat16Array( &rFunc16F[ ( i + j * WEIGHT_TEX_RES ) * 4 ], (FLOAT*)&r, 4 );

				rFunc16[ ( i + j * WEIGHT_TEX_RES ) * 4 + 0 ] = (WORD)(r.x * 65536.0f);
				rFunc16[ ( i + j * WEIGHT_TEX_RES ) * 4 + 1 ] = (WORD)(r.y * 65536.0f);
				rFunc16[ ( i + j * WEIGHT_TEX_RES ) * 4 + 2 ] = (WORD)(r.z * 65536.0f);
				rFunc16[ ( i + j * WEIGHT_TEX_RES ) * 4 + 3 ] = (WORD)(r.w * 65536.0f);

				rFunc[ i + j * WEIGHT_TEX_RES ] = r;
			}

		void *src = rFunc16F;
		int  size = sizeof( WORD );

		CFrmTexture **dst = 0;	// pointer to texture handle
		switch ( n )
		{
		case -1: dst = ppBicubicWeight03;	break;
		case +0: dst = ppBicubicWeight47;	break;
		case +1: dst = ppBicubicWeight8B;	break;
		case +2: dst = ppBicubicWeightCF;	break;
		}

		FrmCreateTexture( WEIGHT_TEX_RES, WEIGHT_TEX_RES, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
                          0, src,  WEIGHT_TEX_RES * WEIGHT_TEX_RES * 4 * sizeof( WORD ), dst );

        // Modify the sampler state            
        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.MaxAnisotropy = 0;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.BorderColor[0] = 0.0f;
        samplerDesc.BorderColor[1] = 0.0f;
        samplerDesc.BorderColor[2] = 0.0f;
        samplerDesc.BorderColor[3] = 0.0f;
        D3DDevice()->CreateSamplerState( &samplerDesc, &(*dst)->m_pSamplerState );
    }

	delete rFunc;
	delete rFunc16;
	delete rFunc16F;

	return 0;
}

#endif