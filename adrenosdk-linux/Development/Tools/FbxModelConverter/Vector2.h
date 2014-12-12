//-----------------------------------------------------------------------------
// 
//  Vector2
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_VECTOR2_H
#define MCE_FBX_VECTOR2_H

#include <math.h>
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class Vector2
        {
            public:
                float X;
                float Y;

                Vector2() : X( 0.0f ), Y( 0.0f )
                {

                }

                Vector2( float x, float y ) : X( x ), Y( y )
                {

                }

                Vector2( double x, double y ) : X( (float)x ), Y( (float)y )
                {

                }

                void Set( float x, float y )
                {
                    X = x;
                    Y = y;
                }

                void Set( double x, double y )
                {
                    X = (float)x;
                    Y = (float)y;
                }
        };

        inline bool operator == ( const MCE::FBX::Vector2& a, const MCE::FBX::Vector2& b )
        {
            bool is_equal = ( a.X == b.X ) && ( a.Y == b.Y );
            return is_equal;
        }

        inline bool operator != ( const MCE::FBX::Vector2& a, const MCE::FBX::Vector2& b )
        {
            bool is_not_equal = ( a.X != b.X ) || ( a.Y != b.Y );
            return is_not_equal;
        }

        inline bool IsSimilar( const MCE::FBX::Vector2& a, const MCE::FBX::Vector2& b, float epsilon = 0.001f )
        {
            bool is_equal = ( fabs( a.X - b.X ) < epsilon ) && ( fabs( a.Y - b.Y ) < epsilon );
            return is_equal;
        }

        typedef std::vector< MCE::FBX::Vector2 > Vector2Array;
    }
}

//-----------------------------------------------------------------------------

#endif