//-----------------------------------------------------------------------------
// 
//  Vector3
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_VECTOR3_H
#define MCE_FBX_VECTOR3_H

#include <math.h>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Vector3
        {
            public:
                float X;
                float Y;
                float Z;

                Vector3() : X( 0.0f ), Y( 0.0f ), Z( 0.0f )
                {

                }

                Vector3( float x, float y, float z ) : X( x ), Y( y ), Z( z )
                {

                }

                Vector3( double x, double y, double z ) : X( (float)x ), Y( (float)y ), Z( (float)z )
                {

                }

                void Set( float x, float y, float z )
                {
                    X = x;
                    Y = y;
                    Z = z;
                }

                void Set( double x, double y, double z )
                {
                    X = (float)x;
                    Y = (float)y;
                    Z = (float)z;
                }
        };

        inline MCE::FBX::Vector3 operator / ( float a, const MCE::FBX::Vector3& b )
        {
            return MCE::FBX::Vector3( a / b.X, a / b.Y, a / b.Z );
        }

        inline MCE::FBX::Vector3 operator - ( const MCE::FBX::Vector3& v )
        {
            return MCE::FBX::Vector3( -v.X, -v.Y, -v.Z );
        }

        inline bool operator == ( const MCE::FBX::Vector3& a, const MCE::FBX::Vector3& b )
        {
            bool is_equal = ( a.X == b.X ) && ( a.Y == b.Y ) && ( a.Z == b.Z );
            return is_equal;
        }

        inline bool operator != ( const MCE::FBX::Vector3& a, const MCE::FBX::Vector3& b )
        {
            bool is_not_equal = ( a.X != b.X ) || ( a.Y != b.Y ) || ( a.Z != b.Z );
            return is_not_equal;
        }
        
        inline bool IsSimilar( const MCE::FBX::Vector3& a, const MCE::FBX::Vector3& b, float epsilon = 0.001f )
        {
            bool is_equal = ( fabs( a.X - b.X ) < epsilon ) && ( fabs( a.Y - b.Y ) < epsilon ) && ( fabs( a.Z - b.Z ) < epsilon );
            return is_equal;
        }

        typedef std::vector< MCE::FBX::Vector3 > Vector3Array;
    }
}

//-----------------------------------------------------------------------------

#endif