//-----------------------------------------------------------------------------
// 
//  Animation
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_ANIMATION_H
#define MCE_FBX_ANIMATION_H

#include "AnimationType.h"
#include "Quaternion.h"
#include "TimeRange.h"
#include "Vector3.h"
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {

        class Animation
        {
            public:
                         Animation();
                virtual ~Animation();

                void                                    SetTimeRange    ( MCE::FBX::TimeRange time_range );
                void                                    SetTimeRange    ( int start_frame, int end_frame );

                MCE::FBX::AnimationType::AnimationTypes Type            () const;

                int                                     GetStartFrame   () const;
                int                                     GetEndFrame     () const;

                int                                     NumKeyFrames    () const;

            protected:
                Animation( MCE::FBX::AnimationType::AnimationTypes type );

                virtual void                            ResizeKeyframes ( int num_keyframes ) = 0;

            private:
                AnimationType::AnimationTypes m_type;
                int                           m_start_frame;
                int                           m_end_frame;
                int                           m_num_keyframes;
        };

        typedef std::vector< Animation* > AnimationArray;
    }
}

//-----------------------------------------------------------------------------

#endif