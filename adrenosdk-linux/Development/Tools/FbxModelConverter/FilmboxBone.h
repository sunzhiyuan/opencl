//-----------------------------------------------------------------------------
// 
//  FilmboxBone
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FILMBOX_BONE_H
#define MCE_FILMBOX_BONE_H

#include <fbxsdk.h>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        struct FilmboxBone
        {
            KFbxNode*   BoneNode;
            KFbxXMatrix InverseBindPose;

            FilmboxBone() : BoneNode( NULL ), InverseBindPose()
            {
                InverseBindPose.SetIdentity();
            }

            FilmboxBone( KFbxNode* bone_node, KFbxXMatrix inverse_bind_pose ) : BoneNode( bone_node ), InverseBindPose( inverse_bind_pose )
            {

            }
        };

        typedef std::vector< FilmboxBone > FilmboxBoneArray;
    }
}

//-----------------------------------------------------------------------------

#endif