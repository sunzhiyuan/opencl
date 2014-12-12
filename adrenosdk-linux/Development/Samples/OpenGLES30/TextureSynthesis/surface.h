//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once
#ifndef _SURFACE__H
#define _SURFACE__H

#include <ctype.h>

// this structure holds our surface parameters
typedef struct _PROCTEX_SURFACEPARAM
{
	char	name[ 256 ];

	// appearance Parameters
	DWORD	spRandomSeed1;
	float	spBumpiness;

	//float	spScaleBump;
	FRMVECTOR4	spColor;

	// Distribution Parameters
	DWORD	spRandomSeed2;
	float	spCoverage,
			spFractalNoise,
			spMimicTerrain,
			spAltitudeMin,
			spAltitudeMax,
			spAltitudeFuzzinessMin,
			spAltitudeFuzzinessMax,
			spSlopeMin,
			spSlopeMax,
			spSlopeFuzzinessMin, 
			spSlopeFuzzinessMax, 
			spBluriness;

	float	flagAltLimitMin, 
			flagAltLimitMax;
	float	flagSlopeLimitMin, 
			flagSlopeLimitMax;

	// temporary parameters
	int		nodeID;
	float	tpAC1, tpAC2, tpAM;
	float	tpSC1, tpSC2, tpSM;

	FRMVECTOR4	tpAScaleBias, tpSScaleBias;
	FRMVECTOR4 noisePan;
	
	int		nChilds;
	_PROCTEX_SURFACEPARAM *child[ 8 ];
}PROCTEX_SURFACEPARAM;

PROCTEX_SURFACEPARAM	*rootSurfaceNode;
PROCTEX_SURFACEPARAM	*surfaceTypes[ 2 ];
FRMVECTOR4				terrainBaseColor;

PROCTEX_SURFACEPARAM	*newSurfaceNode()
{
	PROCTEX_SURFACEPARAM *node = new PROCTEX_SURFACEPARAM;

	node->nChilds = 0;

	node->spRandomSeed1 = 0;
	node->spRandomSeed2 = 0;
	node->spBumpiness   = 0.0f;

	node->spColor		= FRMVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );
	node->noisePan		= FRMVECTOR4( rand() / 32768.0f, rand() / 32768.0f, 0.0f, 0.0f );

	// Distribution Parameters
	node->spCoverage		  = 
	node->spFractalNoise	  = 
	node->spMimicTerrain	  = 
	node->spAltitudeMin		  =
	node->spAltitudeMax		  =
	node->spAltitudeFuzzinessMin =
	node->spAltitudeFuzzinessMax =
	node->spSlopeMin		  =
	node->spSlopeMax		  =
	node->spSlopeFuzzinessMin =
	node->spSlopeFuzzinessMax =
	node->spBluriness         = 0.0f;

	return node;
}

// the following methods are used to read Terragen surface maps

void	readTERMChunk( FRM_FILE *fp, PROCTEX_SURFACEPARAM *chunk, int chunkSize = 0, int level = 0 );
void	readDENSChunk( FRM_FILE *fp, PROCTEX_SURFACEPARAM *chunk, int chunkSize = 0, int level = 0 );

void	readSRFLChunk( FRM_FILE *fp, PROCTEX_SURFACEPARAM *chunk, int chunkSize = 0, int level = 0 )
{
	DWORD	id, length;
	char	buf[ 256 ];

	do {

		FrmFile_Read(fp, &id, sizeof(DWORD));
		FrmFile_Read(fp, &length, sizeof(DWORD));
		if ( length & 3 ) length = ( length & ~3 ) + 4;
		((char*)&id)[0] = toupper( ((char*)&id)[0] );
		((char*)&id)[1] = toupper( ((char*)&id)[1] );
		((char*)&id)[2] = toupper( ((char*)&id)[2] );
		((char*)&id)[3] = toupper( ((char*)&id)[3] );

		chunkSize -= 8 + length;

		switch ( id )
		{
			// SRFL: child layer
			case 0x4c465253:
				if ( level == 0 )
				{
					readSRFLChunk( fp, chunk, length, level + 1 );
					return;
				} else
				{
					chunk->child[ chunk->nChilds ] = newSurfaceNode();
					readSRFLChunk( fp, chunk->child[ chunk->nChilds ], length, level + 1 );
					chunk->nChilds ++;
				}
				break;
			// NAME: name chunk
			case 0x454d414e:
				FrmFile_Read(fp, buf, length);
				strncpy( chunk->name, buf, 255 );
				break;
			// TERM: term chunk
			case 0x4d524554:
				readTERMChunk( fp, chunk, length, level );
				break;
			// DENS: dens chunk
			case 0x534e4544:
				readDENSChunk( fp, chunk, length, level );
				break;
			default:
				FrmFile_Read(fp, buf, length);
				break;
		};

	} while ( chunkSize > 0 );
}


void	readTERMChunk( FRM_FILE *fp, PROCTEX_SURFACEPARAM *chunk, int chunkSize, int level )
{
	DWORD	id, length;
	WORD	r, g, b;

	//Guessing that we won't need more than 256 for this array...
	char temp[256];

	do {

		FrmFile_Read(fp, &id, sizeof(DWORD));
		FrmFile_Read(fp, &length, sizeof(DWORD));
		if ( length & 3 ) length = ( length & ~3 ) + 4;
		((char*)&id)[0] = toupper( ((char*)&id)[0] );
		((char*)&id)[1] = toupper( ((char*)&id)[1] );
		((char*)&id)[2] = toupper( ((char*)&id)[2] );
		((char*)&id)[3] = toupper( ((char*)&id)[3] );

		chunkSize -= 8 + length;

		switch ( id )
		{
			// DIFC: diffuse color chunk
			case 0x43464944:
				FrmFile_Read(fp, &r, sizeof(WORD));
				FrmFile_Read(fp, &g, sizeof(WORD));
				FrmFile_Read(fp, &b, sizeof(WORD));
				FrmFile_Read(fp, temp, length-3*sizeof( WORD ));

				chunk->spColor = FRMVECTOR4( (float)r, (float)g, (float)b, 0.0f ) / 256.0f;
				break;

			// BMAM: bump map amount chunk
			case 0x4d414d42:
				FrmFile_Read(fp, &chunk->spBumpiness, sizeof(float));
				break;

			// MTER: mimic terrain chunk
			case 0x5245544d:
				FrmFile_Read(fp, &chunk->spMimicTerrain, sizeof(float));
				break;

			// BMTX: bump map texture settings chunk
			case 0x58544d42:
				FrmFile_Read(fp, temp, length);
				break;

			default:
				FrmFile_Read(fp, temp, length);
				break;
		};

	} while ( chunkSize > 0 );
}


void	readDENSChunk( FRM_FILE *fp, PROCTEX_SURFACEPARAM *chunk, int chunkSize, int level )
{
	DWORD	id, length;

	//Guessing that we won't need more than 256 for this array...
	char temp[256];

	do {

		FrmFile_Read(fp, &id, sizeof(DWORD));
		FrmFile_Read(fp, &length, sizeof(DWORD));
		if ( length & 3 ) length = ( length & ~3 ) + 4;
		((char*)&id)[0] = toupper( ((char*)&id)[0] );
		((char*)&id)[1] = toupper( ((char*)&id)[1] );
		((char*)&id)[2] = toupper( ((char*)&id)[2] );
		((char*)&id)[3] = toupper( ((char*)&id)[3] );

		chunkSize -= 8 + length;

		switch ( id )
		{
			// VARI: fractal noise chunk
			case 0x49524156:
				FrmFile_Read(fp, &chunk->spFractalNoise, sizeof(float));
				break;

			// COVR: coverage chunk
			case 0x52564f43:
				FrmFile_Read(fp, &chunk->spCoverage, sizeof(float));
				break;

			// ALTE: altitude effects chunk
			case 0x45544c41:
				FrmFile_Read(fp, &chunk->flagAltLimitMin, sizeof(float));
				FrmFile_Read(fp, &chunk->flagAltLimitMax, sizeof(float));
				break;

			// ALTL: altitude limits chunk
			case 0x4c544c41:
				FrmFile_Read(fp, &chunk->spAltitudeMin, sizeof(float));
				FrmFile_Read(fp, &chunk->spAltitudeMax, sizeof(float));
				break;

			// ALTF: altitude fuzziness chunk
			case 0x46544c41:
				FrmFile_Read(fp, &chunk->spAltitudeFuzzinessMin, sizeof(float));
				FrmFile_Read(fp, &chunk->spAltitudeFuzzinessMax, sizeof(float));
				break;

			// SLPE: slope effects chunk
			case 0x45504c53:
				FrmFile_Read(fp, &chunk->flagSlopeLimitMin, sizeof(float));
				FrmFile_Read(fp, &chunk->flagSlopeLimitMax, sizeof(float));
				break;

			// SLPL: slope limits chunk
			case 0x4c504c53:
				FrmFile_Read(fp, &chunk->spSlopeMin, sizeof(float));
				FrmFile_Read(fp, &chunk->spSlopeMax, sizeof(float));
				break;

			// SLPF: slope fuzziness chunk
			case 0x46504c53:
				FrmFile_Read(fp, &chunk->spSlopeFuzzinessMin, sizeof(float));
				FrmFile_Read(fp, &chunk->spSlopeFuzzinessMax, sizeof(float));
				break;

			// SMSZ: smoothing size chunk
			case 0x5a534d53:
				FrmFile_Read(fp, &chunk->spBluriness, sizeof(float));
				break;


			// VART: variation texture settings chunk
			case 0x54524156:
				FrmFile_Read(fp, temp, length);
				break;
			// DNSP: plugin list chunk
			case 0x444e5350:
				FrmFile_Read(fp, temp, length);
				break;

			default:
				FrmFile_Read(fp, temp, length);
				break;
		};

	} while ( chunkSize > 0 );
}

// compute shader-friendly texturing constraints
void	ptPrepareMaterialNode( PROCTEX_SURFACEPARAM *sp )
{
	float m, n;

	extern float terrainMaxElevation, terrainExtend;

	sp->spAltitudeMax = sp->spAltitudeMax * 30.0f / terrainMaxElevation;
	sp->spAltitudeMin = sp->spAltitudeMin * 30.0f / terrainMaxElevation;
	sp->spAltitudeFuzzinessMin = sp->spAltitudeFuzzinessMin * 30.0f / terrainMaxElevation;
	sp->spAltitudeFuzzinessMax = sp->spAltitudeFuzzinessMax * 30.0f / terrainMaxElevation;

	sp->spAltitudeFuzzinessMin *= 1.0f;
	sp->spAltitudeFuzzinessMax *= 1.0f;
	sp->spSlopeFuzzinessMin *= 1.0f;
	sp->spSlopeFuzzinessMax *= 1.0f;

	if ( sp->flagAltLimitMax < 0.5f )
		sp->spAltitudeMax = 2.0f;
	if ( sp->flagAltLimitMin < 0.5f )
		sp->spAltitudeMin = -1.0f;
	if ( sp->flagSlopeLimitMax < 0.5f )
		sp->spSlopeMax = 100.0f;
	if ( sp->flagSlopeLimitMin < 0.5f )
		sp->spSlopeMin = -1.0f;


	// Altitude

	// right side
	m = -1.0f / ( 2.0f * sp->spAltitudeFuzzinessMax );
	n = -m * ( sp->spAltitudeMax + sp->spAltitudeFuzzinessMax );

	sp->tpAScaleBias.x = m;
	sp->tpAScaleBias.y = n;

	// left side
	m = +1.0f / ( 2.0f * sp->spAltitudeFuzzinessMin );
	n = -m * ( sp->spAltitudeMin - sp->spAltitudeFuzzinessMin );

	sp->tpAScaleBias.z = m;
	sp->tpAScaleBias.w = n;


	// Slope

	// right side
	m = -1.0f / ( 2.0f * sp->spSlopeFuzzinessMax*0.5f );
	n = -m * ( sp->spSlopeMax + sp->spSlopeFuzzinessMax );

	sp->tpSScaleBias.x = m;
	sp->tpSScaleBias.y = n;

	// left side
	m = +1.0f / ( 2.0f * sp->spSlopeFuzzinessMin*0.5f );
	n = -m * ( sp->spSlopeMin - sp->spSlopeFuzzinessMin );

	sp->tpSScaleBias.z = m;
	sp->tpSScaleBias.w = n;
}

void	prepareSurfaceNodes( PROCTEX_SURFACEPARAM *node )
{
	ptPrepareMaterialNode( node );
	for ( int i = 0; i < node->nChilds; i++ )
		prepareSurfaceNodes( node->child[ i ] );
}

void	loadTGSurfaceMap( char *filename, PROCTEX_SURFACEPARAM **root )
{
	FRM_FILE *fp;
	char header[16];

	*root = newSurfaceNode();
	
	if(FrmFile_Open(filename, FRM_FILE_READ, &fp))
	{
		// Header: TERRAGEN & SURFMAP2
		FrmFile_Read(fp, header, 16);

		readSRFLChunk(fp, *root);
		FrmFile_Close(fp, FRM_FILE_READ);

		prepareSurfaceNodes( *root );
	}
}

void	createSurfaceNodes()
{
	loadTGSurfaceMap( "Samples/Misc/TextureSynthesis/demo1.srf", &surfaceTypes[ 0 ] );
	loadTGSurfaceMap( "Samples/Misc/TextureSynthesis/demo2.srf", &surfaceTypes[ 1 ] );
		
	rootSurfaceNode = surfaceTypes[ 0 ];
}

#endif

