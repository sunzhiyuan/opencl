//-----------------------------------------------------------------------------
// 
//  TimeRange
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_TIME_RANGE_H
#define MCE_FBX_TIME_RANGE_H

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class TimeRange
        {
            public:
                int StartFrame;
                int EndFrame;

                TimeRange() : StartFrame( 0 ), EndFrame( 0 )
                {

                }

                TimeRange( int start_frame, int end_frame ) : StartFrame( start_frame ), EndFrame( end_frame )
                {

                }

                void Set( int start_frame, int end_frame )
                {
                    StartFrame = start_frame;
                    EndFrame   = end_frame;
                }
        };
    }
}

//-----------------------------------------------------------------------------

#endif