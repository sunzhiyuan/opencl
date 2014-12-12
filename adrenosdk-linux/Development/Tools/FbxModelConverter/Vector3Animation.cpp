//-----------------------------------------------------------------------------
// 
//  Vector3Animation
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "Vector3Animation.h"
#include "Base.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        Vector3Animation::Vector3Animation()
        : MCE::FBX::Animation( MCE::FBX::AnimationType::VECTOR3 )
        , m_keyframes()
        {

        }

        //-----------------------------------------------------------------------------

        Vector3Animation::~Vector3Animation()
        {

        }

        //-----------------------------------------------------------------------------

        MCE::FBX::Vector3& Vector3Animation::GetKeyframe( int index )
        {
            FBX_ASSERT( ( index >= 0 ) && ( index < NumKeyFrames() ), "Index out of bounds" );
            MCE::FBX::Vector3& value = m_keyframes[ index ];
            return value;
        }

        //-----------------------------------------------------------------------------

        const MCE::FBX::Vector3& Vector3Animation::GetKeyframe( int index ) const
        {
            FBX_ASSERT( ( index >= 0 ) && ( index < NumKeyFrames() ), "Index out of bounds" );
            const MCE::FBX::Vector3& value = m_keyframes[ index ];
            return value;
        }

        //-----------------------------------------------------------------------------

        void Vector3Animation::SetKeyframe( int index, const MCE::FBX::Vector3& value )
        {
            FBX_ASSERT( ( index >= 0 ) && ( index < NumKeyFrames() ), "Index out of bounds" );
            m_keyframes[ index ] = value;
        }

        //-----------------------------------------------------------------------------

        void Vector3Animation::ResizeKeyframes( int num_keyframes )
        {
            m_keyframes.resize( num_keyframes );
        }
    }
}
