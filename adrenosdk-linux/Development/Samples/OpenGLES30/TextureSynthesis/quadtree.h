//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once
#ifndef TEXTURE_QUADTREE__H
#define TEXTURE_QUADTREE__H

// choose different atlas sizes here
/*
#define TREE_MAX_RESOLUTION	1024
#define TREE_MAX_LEVELS		6

// SUM_i=0'(TREE_MAX_LEVELS-1)( 4^i )
#define TREE_NODES			1365
*/

#define TREE_MAX_RESOLUTION	512
#define TREE_MAX_LEVELS		5

// SUM_i=0'(TREE_MAX_LEVELS-1)( 4^i )
#define TREE_NODES			341

static	const	INT32		levelOffset[ 6 ] = { 0, 1,  5, 21,  85,  341 };
static	const	INT32		levelNodes [ 6 ] = { 1, 4, 16, 64, 256, 1024 };
static			INT32		levelFree  [ 6 ];


#define NODE_FREE		0
#define NODE_OCC		1
#define NODE_CHILD_OCC	2
#define NODE_PARENT_OCC	3

typedef struct _MFLAT_TREE
{
	INT32			free[ TREE_NODES ];
	INT32			tile[ TREE_NODES ];
}FLAT_TREE;

// this class handles the tile placement within atlas textures
class FQTree
{
	private:
		FLAT_TREE	*mt;
	public:
		FQTree()
		{
            mt = new FLAT_TREE;
            memset(mt->free, 0, sizeof(INT32)*TREE_NODES);
		};

		~FQTree()
		{
			delete mt;
		}

		INT32	treeScan( INT32 size, INT32 *levelIdx, INT32 *tileIdx, INT32 *quality, FRMVECTOR4 *coordTile );
		INT32	treeAllocate( INT32 size, INT32 tileIdx );
		INT32	treeFree( INT32 size, INT32 tileIdx );
};


#endif