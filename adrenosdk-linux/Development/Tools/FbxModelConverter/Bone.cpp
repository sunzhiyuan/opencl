//-----------------------------------------------------------------------------
// 
//  Bone
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "Bone.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        Bone::Bone()
        : MCE::FBX::SceneObject( MCE::FBX::SceneObjectType::BONE )
        , m_inverse_bind_pose()
        {

        }

        //-----------------------------------------------------------------------------

        Bone::~Bone()
        {

        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Transform& Bone::GetInverseBindPose()
        {
            return m_inverse_bind_pose;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Transform& Bone::GetInverseBindPose() const
        {
            return m_inverse_bind_pose;
        }
    }
}
