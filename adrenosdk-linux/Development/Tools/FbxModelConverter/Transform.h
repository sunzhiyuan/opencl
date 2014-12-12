//-----------------------------------------------------------------------------
// 
//  Transform
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_TRANSFORM_H
#define MCE_FBX_TRANSFORM_H

#include "Quaternion.h"
#include "Vector3.h"

#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Transform
        {
            public:
                Vector3    Position;
                Quaternion Rotation;
                Vector3    Scale;

                Transform() : Position(), Rotation(), Scale( 1.0f, 1.0f, 1.0f )
                {

                }

                Transform( Vector3 position, Quaternion rotation, Vector3 scale ) : Position( position ), Rotation( rotation ), Scale( scale )
                {

                }
        };

        typedef std::vector< MCE::FBX::Transform > TransformArray;
    }
}

//-----------------------------------------------------------------------------

#endif