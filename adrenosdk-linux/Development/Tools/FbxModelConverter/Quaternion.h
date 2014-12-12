//-----------------------------------------------------------------------------
// 
//  Quaternion
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_QUATERNION_H
#define MCE_FBX_QUATERNION_H

#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Quaternion
        {
            public:
                float X;
                float Y;
                float Z;
                float W;

                Quaternion() : X( 0.0f ), Y( 0.0f ), Z( 0.0f ), W( 1.0f )
                {

                }

                Quaternion( float x, float y, float z, float w ) : X( x ), Y( y ), Z( z ), W( w )
                {

                }

                Quaternion( double x, double y, double z, double w ) : X( (float)x ), Y( (float)y ), Z( (float)z ), W( (float)w )
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
                
                Quaternion Inverse() const
                {
                    return Quaternion( -X, -Y, -Z, W );
                }
        };

        inline bool operator == ( const MCE::FBX::Quaternion& a, const MCE::FBX::Quaternion& b )
        {
            bool is_equal = ( a.X == b.X ) && ( a.Y == b.Y ) && ( a.Z == b.Z ) && ( a.W == b.W );
            return is_equal;
        }

        typedef std::vector< MCE::FBX::Quaternion > QuaternionArray;
    }
}

//-----------------------------------------------------------------------------

#endif