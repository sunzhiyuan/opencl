//-----------------------------------------------------------------------------
// 
//  Vector4
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_VECTOR4_H
#define MCE_FBX_VECTOR4_H

#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Vector4
        {
            public:
                float X;
                float Y;
                float Z;
                float W;

                Vector4() : X( 0.0f ), Y( 0.0f ), Z( 0.0f ), W( 0.0f )
                {

                }

                Vector4( float x, float y, float z, float w ) : X( x ), Y( y ), Z( z ), W( w )
                {

                }

                Vector4( double x, double y, double z, double w ) : X( (float)x ), Y( (float)y ), Z( (float)z ), W( (float)w )
                {

                }

                void Set( float x, float y, float z, float w )
                {
                    X = x;
                    Y = y;
                    Z = z;
                    W = w;
                }

                void Set( double x, double y, double z, double w )
                {
                    X = (float)x;
                    Y = (float)y;
                    Z = (float)z;
                    W = (float)w;
                }
        };

        inline bool operator == ( const MCE::FBX::Vector4& a, const MCE::FBX::Vector4& b )
        {
            bool is_equal = ( a.X == b.X ) && ( a.Y == b.Y ) && ( a.Z == b.Z ) && ( a.W == b.W );
            return is_equal;
        }

        inline bool operator != ( const MCE::FBX::Vector4& a, const MCE::FBX::Vector4& b )
        {
            bool is_not_equal = ( a.X != b.X ) || ( a.Y != b.Y ) || ( a.Z != b.Z ) || ( a.W != b.W );
            return is_not_equal;
        }

        typedef std::vector< MCE::FBX::Vector4 > Vector4Array;
    }
}

//-----------------------------------------------------------------------------

#endif