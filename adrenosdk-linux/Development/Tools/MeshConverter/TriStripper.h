//--------------------------------------------------------------------------------------
// Name: TriStripper.h
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once


//--------------------------------------------------------------------------------------
// Name: EstimateNumCacheMisses()
// Desc: Estimates the number of vertex cahce misses for a given set of indices.
//--------------------------------------------------------------------------------------
UINT32 EstimateNumCacheMisses( UINT32  nCacheSize, 
                               UINT16* pInIndices16, UINT32 nNumIndices,
                               BOOL    bIsTriStrip );


//--------------------------------------------------------------------------------------
// Name: StripifyMesh()
// Desc: Performs a time-consuming algorithm to generate triangle strips.
//--------------------------------------------------------------------------------------
BOOL StripifyMesh( UINT32   nCacheSize,
                   UINT16*  pInIndices16, UINT32 nNumIndices,
                   UINT16** ppOutStripIndices16, UINT32* pnOutNumIndices );


//--------------------------------------------------------------------------------------
// Name: OptimizeVertexOrder()
// Desc: Reorders vertices (and their indices) to minimize memory page misses.
//--------------------------------------------------------------------------------------
VOID OptimizeVertexOrder( UINT32  nVertexSize,
                          VOID*   pInOutVertices, UINT32 nNumVertices,
                          UINT16* pInOutIndices,  UINT32 nNumIndices );

