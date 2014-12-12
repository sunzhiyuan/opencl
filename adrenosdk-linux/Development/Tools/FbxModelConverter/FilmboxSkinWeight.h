//-----------------------------------------------------------------------------
// 
//  FilmboxSkinWeight
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FILMBOX_SKIN_WEIGHT_H
#define MCE_FILMBOX_SKIN_WEIGHT_H

#include <fbxsdk.h>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        struct FilmboxSkinWeight
        {
            KFbxNode* BoneNode;
            float     Weight;

            FilmboxSkinWeight() : BoneNode( NULL ), Weight( 0.0f )
            {

            }

            FilmboxSkinWeight( KFbxNode* bone_node, float weight ) : BoneNode( bone_node ), Weight( weight )
            {

            }
        };

        typedef std::vector< FilmboxSkinWeight >        FilmboxVertexWeightArray;
        typedef std::vector< FilmboxVertexWeightArray > FilmboxMeshWeightArray;
    }
}

//-----------------------------------------------------------------------------

#endif