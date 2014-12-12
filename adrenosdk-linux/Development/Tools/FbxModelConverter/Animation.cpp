//-----------------------------------------------------------------------------
// 
//  Animation
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "Animation.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        //-----------------------------------------------------------------------------

        Animation::Animation()
        : m_type( MCE::FBX::AnimationType::INVALID )
        , m_start_frame( 0 )
        , m_end_frame( 0 )
        , m_num_keyframes( 0 )
        {

        }

        //-----------------------------------------------------------------------------

        Animation::Animation( MCE::FBX::AnimationType::AnimationTypes type )
        : m_type( type )
        , m_start_frame( 0 )
        , m_end_frame( 0 )
        , m_num_keyframes( 0 )
        {

        }

        //-----------------------------------------------------------------------------

        Animation::~Animation()
        {

        }

        //-----------------------------------------------------------------------------

        void Animation::SetTimeRange( MCE::FBX::TimeRange time_range )
        {
            SetTimeRange( time_range.StartFrame, time_range.EndFrame );
        }

        //-----------------------------------------------------------------------------

        void Animation::SetTimeRange( int start_frame, int end_frame )
        {
            m_start_frame   = start_frame;
            m_end_frame     = end_frame;
            m_num_keyframes = m_end_frame - m_start_frame + 1; // Add one to make the number of keys inclusive of both end points

            ResizeKeyframes( m_num_keyframes );
        }

        //-----------------------------------------------------------------------------

        MCE::FBX::AnimationType::AnimationTypes Animation::Type() const
        {
            return m_type;
        }

        //-----------------------------------------------------------------------------

        int Animation::GetStartFrame() const
        {
            return m_start_frame;
        }

        //-----------------------------------------------------------------------------

        int Animation::GetEndFrame() const
        {
            return m_end_frame;
        }

        //-----------------------------------------------------------------------------

        int Animation::NumKeyFrames() const
        {
            return m_num_keyframes;
        }
    }
}
