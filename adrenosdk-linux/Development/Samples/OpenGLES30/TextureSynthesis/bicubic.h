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
#include "FrmUtils.h"

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


void Float32ToFloat16Array( FLOAT16 *dstF16, FLOAT32 *srcF32, INT32 n )
{
	for( int i=0; i<n; i++ )
		dstF16[i] = CompressFloatToHalfFloat( srcF32[i] );
}


// computes weights for cubic interpolation of the height map
FLOAT32 R( FLOAT32 x )
{
	FLOAT32 v, t;
	t  = FRM_MAX( 0.0f, x + 2.0f );
	v  = t * t * t;
	t  = FRM_MAX( 0.0f, x + 1.0f );
	v -= 4.0f * t * t * t;
	t  = FRM_MAX( 0.0f, x + 0.0f );
	v += 6.0f * t * t * t;
	t  = FRM_MAX( 0.0f, x - 1.0f );
	v -= 4.0f * t * t * t;
	v /= 6.0f;
	return v;
}

// create & compute the textures
INT32 generateBiCubicWeights( GLuint *pBicubicWeight03,
							GLuint *pBicubicWeight47,
							GLuint *pBicubicWeight8B,
							GLuint *pBicubicWeightCF )
{
	INT32	i, j;
/*
	if ( FAILED( pD3DDevice->CreateTexture(
		WEIGHT_TEX_RES, WEIGHT_TEX_RES, 1, 0, BICUBIC_FORMAT, D3DPOOL_MANAGED, (IDirect3DTexture9**)&bicubicWeight03, NULL ) ) )
		return -1;
	if ( FAILED( pD3DDevice->CreateTexture(
		WEIGHT_TEX_RES, WEIGHT_TEX_RES, 1, 0, BICUBIC_FORMAT, D3DPOOL_MANAGED, (IDirect3DTexture9**)&bicubicWeight47, NULL ) ) )
		return -1;
	if ( FAILED( pD3DDevice->CreateTexture(
		WEIGHT_TEX_RES, WEIGHT_TEX_RES, 1, 0, BICUBIC_FORMAT, D3DPOOL_MANAGED, (IDirect3DTexture9**)&bicubicWeight8B, NULL ) ) )
		return -1;
	if ( FAILED( pD3DDevice->CreateTexture(
		WEIGHT_TEX_RES, WEIGHT_TEX_RES, 1, 0, BICUBIC_FORMAT, D3DPOOL_MANAGED, (IDirect3DTexture9**)&bicubicWeightCF, NULL ) ) )
		return -1;
*/
	
	glGenTextures(1, pBicubicWeight03);
	glBindTexture( GL_TEXTURE_2D, *pBicubicWeight03 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	glGenTextures(1, pBicubicWeight47);
	glBindTexture( GL_TEXTURE_2D, *pBicubicWeight47 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	glGenTextures(1, pBicubicWeight8B);
	glBindTexture( GL_TEXTURE_2D, *pBicubicWeight8B );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	glGenTextures(1, pBicubicWeightCF);
	glBindTexture( GL_TEXTURE_2D, *pBicubicWeightCF );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glBindTexture( GL_TEXTURE_2D, 0 );

	// !hack!
	FRMVECTOR4  *rFunc		= new FRMVECTOR4[ WEIGHT_TEX_RES * WEIGHT_TEX_RES ];
	WORD		*rFunc16	= new WORD[ WEIGHT_TEX_RES * WEIGHT_TEX_RES * 4 ];
	FLOAT16		*rFunc16F	= new FLOAT16[ WEIGHT_TEX_RES * WEIGHT_TEX_RES * 4 ];

	for ( INT32 n = -1; n <= 2; n++ )
	{
		for ( j = 0; j < WEIGHT_TEX_RES; j++ )
			for ( i = 0; i < WEIGHT_TEX_RES; i++ )
			{
				FLOAT32 dx = (FLOAT32)(i+0) / (FLOAT32)(WEIGHT_TEX_RES), dy = (FLOAT32)(j+0) / (FLOAT32)(WEIGHT_TEX_RES);

				FRMVECTOR4 r;
				// m = -1..+2, n = -1
				r.x = R( -1.0f - dx ) * R( dy - (FLOAT32)n );
				r.y = R( +0.0f - dx ) * R( dy - (FLOAT32)n );
				r.z = R( +1.0f - dx ) * R( dy - (FLOAT32)n );
				r.w = R( +2.0f - dx ) * R( dy - (FLOAT32)n );

				Float32ToFloat16Array( &rFunc16F[ ( i + j * WEIGHT_TEX_RES ) * 4 ], (FLOAT32*)&r, 4 );

				rFunc16[ ( i + j * WEIGHT_TEX_RES ) * 4 + 0 ] = (WORD)(r.x * 65536.0f);
				rFunc16[ ( i + j * WEIGHT_TEX_RES ) * 4 + 1 ] = (WORD)(r.y * 65536.0f);
				rFunc16[ ( i + j * WEIGHT_TEX_RES ) * 4 + 2 ] = (WORD)(r.z * 65536.0f);
				rFunc16[ ( i + j * WEIGHT_TEX_RES ) * 4 + 3 ] = (WORD)(r.w * 65536.0f);

				rFunc[ i + j * WEIGHT_TEX_RES ] = r;
			}

#if BICUBIC_FORMAT == GL_RGBA32F
		void *src = rFunc;
		INT32  size = sizeof( FLOAT32 );
#elif BICUBIC_FORMAT == GL_RGBA16F
		void *src = rFunc16F;
		INT32  size = sizeof( WORD );
#elif BICUBIC_FORMAT == GL_RGBA16UI
		void *src = rFunc16;
		INT32  size = sizeof( WORD );
#endif

		GLuint *dst;	// poINT32er to texture handle
		switch ( n )
		{
		case -1: dst = pBicubicWeight03;	break;
		case +0: dst = pBicubicWeight47;	break;
		case +1: dst = pBicubicWeight8B;	break;
		case +2: dst = pBicubicWeightCF;	break;
		}

		/*
		LPDIRECT3DSURFACE9	lpSurface;
		dst->GetSurfaceLevel( 0, &lpSurface );
		
		RECT srcRect = { 0, 0, WEIGHT_TEX_RES, WEIGHT_TEX_RES };
		D3DXLoadSurfaceFromMemory( 
			lpSurface, NULL, NULL, src, BICUBIC_FORMAT, WEIGHT_TEX_RES * 4 * size,
			NULL, &srcRect, D3DX_FILTER_NONE, 0 );

		SAFE_RELEASE( lpSurface );
		*/
		
		glBindTexture( GL_TEXTURE_2D, *dst );
		
#if BICUBIC_FORMAT == GL_RGBA32F
		glTexImage2D( GL_TEXTURE_2D, 0, BICUBIC_FORMAT, WEIGHT_TEX_RES, WEIGHT_TEX_RES,
						0, GL_RGBA, GL_FLOAT, src);
#elif BICUBIC_FORMAT == GL_RGBA16F
		glTexImage2D( GL_TEXTURE_2D, 0, BICUBIC_FORMAT, WEIGHT_TEX_RES, WEIGHT_TEX_RES,
						0, GL_RGBA, GL_HALF_FLOAT, src);
#elif BICUBIC_FORMAT == GL_RGBA16UI
		glTexImage2D( GL_TEXTURE_2D, 0, BICUBIC_FORMAT, WEIGHT_TEX_RES, WEIGHT_TEX_RES,
						0, GL_RGBA, GL_UNSIGNED_SHORT, src);
#endif

		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	delete rFunc;
	delete rFunc16;
	delete rFunc16F;

	return 0;
}

#endif