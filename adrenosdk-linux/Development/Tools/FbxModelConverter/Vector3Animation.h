//-----------------------------------------------------------------------------
// 
//  Vector3Animation
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_VECTOR3_ANIMATION_H
#define MCE_FBX_VECTOR3_ANIMATION_H

#include "Animation.h"
#include "Vector3.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {

        class Vector3Animation : public MCE::FBX::Animation
        {
            public:
                         Vector3Animation();
                virtual ~Vector3Animation();

                MCE::FBX::Vector3&          GetKeyframe     ( int index );
                const MCE::FBX::Vector3&    GetKeyframe     ( int index ) const;
                void                        SetKeyframe     ( int index, const MCE::FBX::Vector3& value );

            protected:
                virtual void                ResizeKeyframes ( int num_keyframes );

            private:
                Vector3Array m_keyframes;
        };
    }
}

//-----------------------------------------------------------------------------

#endif