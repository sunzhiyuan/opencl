//-----------------------------------------------------------------------------
// 
//  SkinWeight
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_SKIN_WEIGHT_H
#define MCE_FBX_SKIN_WEIGHT_H

#include "Base.h"
#include <math.h>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class SkinWeight
        {
            public:
                uint32 BoneIndex;
                float  Weight;

                SkinWeight() : BoneIndex( 0 ), Weight( 0.0f )
                {

                }

                SkinWeight( uint32 bone_index, float weight ) : BoneIndex( bone_index ), Weight( weight )
                {

                }

                SkinWeight( uint32 bone_index, double weight ) : BoneIndex( bone_index ), Weight( (float)weight )
                {

                }

                void Set( uint32 bone_index, float weight )
                {
                    BoneIndex = bone_index;
                    Weight    = weight;
                }

                void Set( uint32 bone_index, double weight )
                {
                    BoneIndex = bone_index;
                    Weight    = (float)weight;
                }
        };

        inline bool operator == ( const MCE::FBX::SkinWeight& a, const MCE::FBX::SkinWeight& b )
        {
            bool is_equal = ( a.BoneIndex == b.BoneIndex ) && ( a.Weight == b.Weight );
            return is_equal;
        }

        inline bool operator != ( const MCE::FBX::SkinWeight& a, const MCE::FBX::SkinWeight& b )
        {
            bool is_not_equal = ( a.BoneIndex != b.BoneIndex ) || ( a.Weight != b.Weight );
            return is_not_equal;
        }

        inline bool IsSimilar( const MCE::FBX::SkinWeight& a, const MCE::FBX::SkinWeight& b, float epsilon = 0.001f )
        {
            bool is_equal = ( a.BoneIndex == b.BoneIndex ) && ( fabs( a.Weight - b.Weight ) < epsilon );
            return is_equal;
        }

        typedef std::vector< MCE::FBX::SkinWeight > SkinWeightArray;
    }
}

//-----------------------------------------------------------------------------

#endif