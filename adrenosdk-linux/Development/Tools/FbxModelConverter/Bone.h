//-----------------------------------------------------------------------------
// 
//  Bone
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_BONE_H
#define MCE_FBX_BONE_H

#include "SceneObject.h"
#include "Transform.h"
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class FilmboxNode;

        class Bone : public SceneObject
        {
            public:
                         Bone();
                virtual ~Bone();

                MCE::FBX::Transform&        GetInverseBindPose  ();
                const MCE::FBX::Transform&  GetInverseBindPose  () const;

            private:
                MCE::FBX::Transform m_inverse_bind_pose;
        };

        typedef std::vector< Bone* > BoneArray;
    }
}

//-----------------------------------------------------------------------------

#endif