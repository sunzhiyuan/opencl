//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#include		"FrmPlatform.h"
#include        "HeightField.hpp"

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
HeightField::HeightField()
: m_width( 0 ), m_height( 0 ), m_gridSpacing( 0.0 )
{
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
HeightField::~HeightField()
{
    destroy();
}

//-----------------------------------------------------------------------------
//  createFromRGBImage
//-----------------------------------------------------------------------------
void HeightField::createFromRGBImage( const unsigned char* a_pImageData
                                    , const unsigned int a_imageWidth
                                    , const unsigned int a_imageHeight
                                    , const double a_heightScale
                                    , const double a_gridSpacing
                                    )
{
    // Check image pointer

    if( NULL == a_pImageData )
        return;

    // Destroy current instance

    destroy();

    // Initialize height container

    m_heights.resize( a_imageWidth * a_imageHeight );

    // Initialize members

    m_width         = a_imageWidth;
    m_height        = a_imageHeight;
    m_gridSpacing   = a_gridSpacing;

    // Create height values

    for( unsigned int y = 0; y < m_height; ++y )
    {
        for( unsigned int x = 0; x < m_width; ++x )
        {
            unsigned int offset = y * m_width * 3 + x * 3;
            unsigned char pixel = (a_pImageData[ offset + 0 ] + a_pImageData[ offset + 1 ] + a_pImageData[ offset + 2 ]) / 3;

            m_heights[ y * m_width + x ] = static_cast< double >( pixel ) * a_heightScale;
        }
    }
}

//-----------------------------------------------------------------------------
//  destroy
//-----------------------------------------------------------------------------
void HeightField::destroy()
{
    // Reset members

    m_heights       .clear();
    m_width         = 0;
    m_height        = 0;
    m_gridSpacing   = 0.0;
}

//-----------------------------------------------------------------------------
//  getFieldWidth
//-----------------------------------------------------------------------------
const unsigned int HeightField::getFieldWidth() const
{
    return m_width;
}

//-----------------------------------------------------------------------------
//  getFieldHeight
//-----------------------------------------------------------------------------
const unsigned int HeightField::getFieldHeight() const
{
    return m_height;
}

//-----------------------------------------------------------------------------
//  getHeight
//-----------------------------------------------------------------------------
const double HeightField::getHeight( const unsigned int a_x, const unsigned int a_y ) const
{
    return m_heights[ a_y * m_width + a_x ];
}

//-----------------------------------------------------------------------------
//  getGridSpacing
//-----------------------------------------------------------------------------
const double HeightField::getGridSpacing() const
{
    return m_gridSpacing;
}
