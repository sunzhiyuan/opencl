//-----------------------------------------------------------------------------
// 
//  QuaternionAnimation
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "QuaternionAnimation.h"
#include "Base.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        QuaternionAnimation::QuaternionAnimation()
        : MCE::FBX::Animation( MCE::FBX::AnimationType::QUATERNION )
        , m_keyframes()
        {

        }

        //-----------------------------------------------------------------------------

        QuaternionAnimation::~QuaternionAnimation()
        {

        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Quaternion& QuaternionAnimation::GetKeyframe( int index )
        {
            FBX_ASSERT( ( index >= 0 ) && ( index < NumKeyFrames() ), "Index out of bounds" );
            MCE::FBX::Quaternion& value = m_keyframes[ index ];
            return value;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Quaternion& QuaternionAnimation::GetKeyframe( int index ) const
        {
            FBX_ASSERT( ( index >= 0 ) && ( index < NumKeyFrames() ), "Index out of bounds" );
            const MCE::FBX::Quaternion& value = m_keyframes[ index ];
            return value;
        }

        //-----------------------------------------------------------------------------

        void QuaternionAnimation::SetKeyframe( int index, const MCE::FBX::Quaternion& value )
        {
            FBX_ASSERT( ( index >= 0 ) && ( index < NumKeyFrames() ), "Index out of bounds" );
            m_keyframes[ index ] = value;
        }

        //-----------------------------------------------------------------------------

        void QuaternionAnimation::ResizeKeyframes( int num_keyframes )
        {
            m_keyframes.resize( num_keyframes );
        }
    }
}
