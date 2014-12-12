//-----------------------------------------------------------------------------
// 
//  AnimationTrack
//	
//  Copyright (c) 2011 QUALCOMM Incorporated.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#include "FrmAnimationTrack.h"

//-----------------------------------------------------------------------------

Adreno::AnimationTrack::AnimationTrack()
: Id()
, NumKeyframes( 0 )
, Keyframes( NULL )
{

}

//-----------------------------------------------------------------------------

Adreno::AnimationTrack::~AnimationTrack()
{
    DeleteKeyframes();
}

//-----------------------------------------------------------------------------

void Adreno::AnimationTrack::SetName( const char* name )
{
    Id.SetName( name );
}

//-----------------------------------------------------------------------------

void Adreno::AnimationTrack::ResizeKeyframes( int num_keyframes )
{
    DeleteKeyframes();
    
    NumKeyframes = num_keyframes;
    
    if( num_keyframes > 0 )
    {
        Keyframes = new Adreno::Transform[ num_keyframes ];
    }
}

//-----------------------------------------------------------------------------

void Adreno::AnimationTrack::DeleteKeyframes()
{
    if( Keyframes )
    {
        delete[] Keyframes;
        Keyframes = NULL;
    }
}
