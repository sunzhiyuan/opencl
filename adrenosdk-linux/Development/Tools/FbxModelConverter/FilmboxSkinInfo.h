//-----------------------------------------------------------------------------
// 
//  FilmboxSkinInfo
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FILMBOX_SKIN_INFO_H
#define MCE_FILMBOX_SKIN_INFO_H

#include "FilmboxBone.h"
#include "FilmboxSkinWeight.h"
#include <fbxsdk.h>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        struct FilmboxSkinInfo
        {
            KFbxNode*               Node;
            KFbxMesh*               Mesh;
            bool                    IsSkinned;
            FilmboxBoneArray        Bones;
            FilmboxMeshWeightArray  MeshWeights;
            int                     NumWeightsPerVertex;

            FilmboxSkinInfo() : Node( NULL ), Mesh( NULL ), IsSkinned( false ), Bones(), MeshWeights(), NumWeightsPerVertex( 0 )
            {

            }
        };

        typedef std::vector< FilmboxSkinInfo > FilmboxSkinInfoArray;
    }
}

//-----------------------------------------------------------------------------

#endif