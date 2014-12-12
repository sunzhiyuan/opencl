//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once
#ifndef _LOD__H
#define _LOD__H

// a terrain-tile represents a square region of the terrain (original or coarser representation)
typedef struct
{
	FRMVECTOR3				center,				// the center and AABB of this terrain part
							aabbMin, 
							aabbMax;


	/*	Replacing this with vert and index buffer
		(thats all thats used from the D3DXMESH)
	LPD3DXMESH				pMesh;				// mesh for this terrain part
	DWORD					fvfMeshVertex, 
							sizePerVertex;
	*/

	GLuint					hVertexBuffer;				// handle to vertex buffer handle
	UINT32					vertexType;					// vertex data type
	UINT32					numVertexComponents;		// number of components per vertex
	UINT32					numTexcoordComponents;		// number of components per texture
	UINT32					vertexBufferSize;			// byte size of the vertex buffer
	UINT32					vertexBufferStride;			// stride from vertex to vertex (includes texcoord)
	UINT32					vertexBufferTexcoordOffset;	// offset to first Texcoord in the buffer
	
	GLuint					hIndexBuffer;		// handle to index buffer handle
	UINT32					indexType;			// index data type
	UINT32					indexBufferSize;	// byte size of index buffer
	UINT32					numIndices;			// total number of indices

	GLuint*					pTexture;			// texture containing surface color
	FRMVECTOR4				coordTile;			// bias + scale: 4 floats defining rectangle of corresponding texture area
	int						tileSize, 
							tileIdx;			// stores tile index (lower 24 bit) and texture atlas (upper 8 bit)
	int						worldSize;			// real size in world space
	int						intersectFrustum;	// guess what
	
	GLuint*					pHeightMapTexture;	// ptr to source height map
	FRMVECTOR4				coordHM;			// bias + scale to adress corresponding rectangle in the heightmap
}TERRAINTILE;

// number of atlas textures (each 512²)
#define ATLAS_TEXTURES	32

#define	MAX_TILE_TEXTURE_SIZE	512
#define ATLAS_MINIMUM_TILE_SIZE	32

// this is, what we store for each atlas texture
class FQTree;
typedef struct
{
	// quadtree for managing the tile placement
	FQTree				*layout;
	// its texture
	GLuint				texture;
	// pointers to the terrain tiles stored
	TERRAINTILE			*assign[ (MAX_TILE_TEXTURE_SIZE/ATLAS_MINIMUM_TILE_SIZE)*(MAX_TILE_TEXTURE_SIZE/ATLAS_MINIMUM_TILE_SIZE) ];
	// number of tiles
	DWORD				count;
}ATLAS;

// see lod.cpp
extern int	heightMapX, 
			heightMapY,
			heightMapXFull,
			heightMapYFull,
			quadsPerTile;

void	createQLOD( int heightMapSize, WORD *heightMap16, GLuint* heightMapTexture16 );
void	traverseQLOD( FRMVECTOR4 &camPos, FRMVECTOR4 &camDst, int node, int level );
void	freeAllTexturesQLOD( int node, int level );
void	renderQLOD( INT32 *pRtSizeLoc,
					INT32 *pTexCoordBiasLoc, INT32 *pTerrainTextureLoc );
void	destroyQLOD();

#endif