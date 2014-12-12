//-----------------------------------------------------------------------------
// 
//  Color
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_COLOR_H
#define MCE_FBX_COLOR_H

#include "Base.h"
#include <vector>

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        struct Color
        {
            public:
                uint8 R;
                uint8 G;
                uint8 B;
                uint8 A;

                Color() : R( 255 ), G( 255 ), B( 255 ), A( 255 )
                {

                }

                Color( uint8 r, uint8 g, uint8 b ) : R( r ), G( g ), B( b ), A( 255 )
                {

                }

                Color( uint8 r, uint8 g, uint8 b, uint8 a ) : R( r ), G( g ), B( b ), A( a )
                {

                }

                Color( int r, int g, int b, int a ) : R( (uint8)r ), G( (uint8)g ), B( (uint8)b ), A( (uint8)a )
                {

                }

                Color( float r, float g, float b, float a ) : R( uint8( r * 255.0f ) ), G( uint8( g * 255.0f ) ), B( uint8( b * 255.0f ) ), A( uint8( a * 255.0f ) )
                {

                }

                Color( double r, double g, double b, double a ) : R( uint8( r * 255.0 ) ), G( uint8( g * 255.0 ) ), B( uint8( b * 255.0 ) ), A( uint8( a * 255.0 ) )
                {

                }

                uint32 AsUint32()
                {
                    uint32 color = *((uint32*)this);
                    return color;
                }
        };

        inline bool operator == ( const MCE::FBX::Color& a, const MCE::FBX::Color& b )
        {
            bool is_equal = ( a.R == b.R ) && ( a.G == b.G ) && ( a.B == b.B ) && ( a.A == b.A );
            return is_equal;
        }

        inline bool operator != ( const MCE::FBX::Color& a, const MCE::FBX::Color& b )
        {
            bool is_not_equal = ( a.R != b.R ) || ( a.G != b.G ) || ( a.B != b.B ) || ( a.A != b.A );
            return is_not_equal;
        }

        typedef std::vector< MCE::FBX::Color > ColorArray;
    }
}

//-----------------------------------------------------------------------------

#endif // MCE_FBX_COLOR_H