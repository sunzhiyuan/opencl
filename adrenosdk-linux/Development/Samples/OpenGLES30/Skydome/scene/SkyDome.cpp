//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#include		"FrmPlatform.h"
#include        "SkyDome.hpp"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
SkyDome::SkyDome()
{
	 m_sunDirection = FRMVECTOR3( 0.80f, 0.80f, 0.80f );
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
SkyDome::~SkyDome()
{
    destroy();
}

//-----------------------------------------------------------------------------
//  create
//-----------------------------------------------------------------------------
void SkyDome::create( const double a_radius
                    , const double a_height
                    , const int a_rings
                    , const int a_segments
					, const double lowered
                    )
{
    // Destroy current instance

    m_vertices.clear();
    m_indices .clear();

    // Clamp arguments

	const double    dEpsilon      =   1.0e-5;
	const double    dPI           =   3.14159265358979323846;
	const double    dDegToRad     =   dPI / 180.0;

    double radius = max( a_radius, dEpsilon );
    double height = max( a_height, dEpsilon );

    int rings    = max( a_rings, 1 ) + 1;
    int segments = max( a_segments, 4 ) + 1;

    // Init vertex data container

    m_vertices.reserve( rings * (segments + 1) );

    // Fill vertex data container

    for( int y = 0; y <= segments; ++y )
    {
        double theta = static_cast< double >( y ) / segments * 360.0 * dDegToRad;

        for( int x = rings - 1; x >= 0; --x )
        {
            double phi = static_cast< double >( x ) / rings * 90.0 * dDegToRad;

            m_vertices.push_back( FRMVECTOR3  ( static_cast< float >( sin( phi ) * cos( theta ) * radius )
										    , static_cast< float >( (cos( phi ) * height ) - lowered) 
											, static_cast< float >( sin( phi ) * sin( theta ) * radius )));
                                           // , static_cast< float >( sin( phi ) * sin( theta ) * radius )
                                           // , static_cast< float >( cos( phi ) * height ) ) );
        }
    }

    // Init index container

    m_indices.reserve( segments * rings * 2 );

    // Fill index container

    bool leftToRight = false;

    for(int y = 0; y < segments; ++y )
    {
        if( true == leftToRight )
        {
            for( int x = 0; x < rings; ++x )
            {
                m_indices.push_back( (y + 1) * rings + x );
                m_indices.push_back(  y      * rings + x );
            }
        }
        else
        {
            for( int x = rings - 1; x >= 0; --x )
            {
                m_indices.push_back(  y      * rings + x );
                m_indices.push_back( (y + 1) * rings + x );
            }
        }

        leftToRight = !leftToRight;
    }
}

//-----------------------------------------------------------------------------
//  destroy
//-----------------------------------------------------------------------------
void SkyDome::destroy()
{
    // Reset members

    m_vertices.clear();
    m_indices .clear();
}

//-----------------------------------------------------------------------------
//  getVertices
//-----------------------------------------------------------------------------
const FRMVECTOR3* SkyDome::getVertices() const
{
    return &m_vertices.front();
}

//-----------------------------------------------------------------------------
//  getIndices
//-----------------------------------------------------------------------------
const unsigned int* SkyDome::getIndices() const
{
    return &m_indices.front();
}

//-----------------------------------------------------------------------------
//  getIndexCount
//-----------------------------------------------------------------------------
const size_t SkyDome::getIndexCount() const
{
    return m_indices.size();
}
    
//-----------------------------------------------------------------------------
//  setSunDirection
//-----------------------------------------------------------------------------
void SkyDome::setSunDirection( const FRMVECTOR3& a_direction )
{
    m_sunDirection = a_direction;
}

//-----------------------------------------------------------------------------
//  getSunDirection
//-----------------------------------------------------------------------------
FRMVECTOR3 SkyDome::getSunDirection() const
{
    return m_sunDirection;
}
