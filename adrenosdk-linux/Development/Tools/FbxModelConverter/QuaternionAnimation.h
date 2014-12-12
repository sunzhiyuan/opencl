//-----------------------------------------------------------------------------
// 
//  QuaternionAnimation
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_QUATERNION_ANIMATION_H
#define MCE_FBX_QUATERNION_ANIMATION_H

#include "Animation.h"
#include "Quaternion.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {

        class QuaternionAnimation : public MCE::FBX::Animation
        {
            public:
                         QuaternionAnimation();
                virtual ~QuaternionAnimation();

                MCE::FBX::Quaternion&       GetKeyframe     ( int index );
                const MCE::FBX::Quaternion& GetKeyframe     ( int index ) const;
                void                        SetKeyframe     ( int index, const MCE::FBX::Quaternion& value );

            protected:
                virtual void                ResizeKeyframes ( int num_keyframes );

            private:
                QuaternionArray m_keyframes;
        };
    }
}

//-----------------------------------------------------------------------------

#endif