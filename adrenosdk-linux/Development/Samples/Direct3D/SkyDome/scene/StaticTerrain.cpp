//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "FrmPlatform.h"
#include "FrmMath.h"
#include "StaticTerrain.hpp"
#include "HeightField.hpp"

//*************************************************************************************************
//*************************************************************************************************
//**    Public methods
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
StaticTerrain::StaticTerrain()
: m_totalWidth( 0.0 ), m_totalHeight( 0.0 )
, m_pTerrainMap(0), m_pNormalMap(0)
{}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
StaticTerrain::~StaticTerrain()
{
    destroy();    
}

//-----------------------------------------------------------------------------
//  create
//-----------------------------------------------------------------------------
void StaticTerrain::create( const HeightField& a_heightField )
{
    // Destroy current instance

    destroy();

    // Init vertex and normal data container

    unsigned int vertexCount = a_heightField.getFieldWidth() * a_heightField.getFieldHeight();

    m_vertices .reserve( vertexCount );
    m_texCoords.reserve( vertexCount );

    // Fill vertex and texcoord data container

    for( unsigned int y = 0; y < a_heightField.getFieldHeight(); ++y )
    {
        for( unsigned int x = 0; x < a_heightField.getFieldWidth(); ++x )
        {
            m_vertices.push_back( FRMVECTOR3  ( static_cast< float >( x * a_heightField.getGridSpacing() )
										    , static_cast< float >( a_heightField.getHeight( x, y ) ) 
											, static_cast< float >( y * a_heightField.getGridSpacing() )));
                                           // , static_cast< float >( y * a_heightField.getGridSpacing() )
                                           // , static_cast< float >( a_heightField.getHeight( x, y ) ) ) );

            m_texCoords.push_back( FRMVECTOR3 ( static_cast< float >( x ) / a_heightField.getFieldWidth()
                                            , static_cast< float >( y ) / a_heightField.getFieldHeight()
                                            , 0.0 ) );
        }
    }

    // Init index container

    m_indices.reserve( (a_heightField.getFieldHeight() - 1) * a_heightField.getFieldWidth() * 2 );

    // Fill index container

    bool leftToRight = true;

    for(unsigned int y = 0; y < a_heightField.getFieldHeight() - 1; ++y )
    {
        if( true == leftToRight )
        {
            for( unsigned int x = 0; x < a_heightField.getFieldWidth(); ++x )
            {
                m_indices.push_back( (y + 1) * a_heightField.getFieldWidth() + x );
                m_indices.push_back(  y      * a_heightField.getFieldWidth() + x );
            }
        }
        else
        {
            for( int x = a_heightField.getFieldWidth() - 1; x >= 0; --x )
            {
                m_indices.push_back(  y      * a_heightField.getFieldWidth() + x );
                m_indices.push_back( (y + 1) * a_heightField.getFieldWidth() + x );
            }
        }

        leftToRight = !leftToRight;
    }

    // Set terrain dimensions

    m_totalWidth  = a_heightField.getFieldWidth() * a_heightField.getGridSpacing();
    m_totalHeight = a_heightField.getFieldHeight() * a_heightField.getGridSpacing();
}
void StaticTerrain::GenTerrainAndNormalTextures( const unsigned char* a_terrainMap
												, const int a_terrainMapWidth
												, const int a_terrainMapHeight
												, const unsigned char* a_normalMap
												, const int a_normalMapWidth
												, const int a_normalMapHeight)

{
    // Create terrain textures
    FrmCreateTexture( a_terrainMapWidth, a_terrainMapHeight, 1,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0,
        (void*)a_terrainMap, a_terrainMapWidth * a_terrainMapHeight * 4,
        &m_pTerrainMap);

     FrmCreateTexture( a_normalMapWidth, a_normalMapHeight, 1,
        DXGI_FORMAT_R8G8B8A8_UNORM, 0,
        (void*)a_normalMap, a_normalMapWidth * a_normalMapHeight * 4,
        &m_pNormalMap);
}

//-----------------------------------------------------------------------------
//  destroy
//-----------------------------------------------------------------------------
void StaticTerrain::destroy()
{
    // Reset members

    m_vertices .clear();
    m_texCoords.clear();
    m_indices  .clear();

    m_totalWidth  = 0.0;
    m_totalHeight = 0.0;

    if (m_pTerrainMap) m_pTerrainMap->Release();
    m_pTerrainMap = NULL;

    if (m_pNormalMap) m_pNormalMap->Release();
    m_pNormalMap = NULL;

}

//-----------------------------------------------------------------------------
//  getVertices
//-----------------------------------------------------------------------------
const FRMVECTOR3* StaticTerrain::getVertices() const
{
    return &m_vertices.front();
}

//-----------------------------------------------------------------------------
//  getTexCoords
//-----------------------------------------------------------------------------
const FRMVECTOR3* StaticTerrain::getTexCoords() const
{
    return &m_texCoords.front();
}

//-----------------------------------------------------------------------------
//  getIndices
//-----------------------------------------------------------------------------
const unsigned int* StaticTerrain::getIndices() const
{
    return &m_indices.front();
}

//-----------------------------------------------------------------------------
//  getIndexCount
//-----------------------------------------------------------------------------
const size_t StaticTerrain::getIndexCount() const
{
    return m_indices.size();
}

//-----------------------------------------------------------------------------
//  getIndexCount
//-----------------------------------------------------------------------------
const size_t StaticTerrain::getVertexCount() const
{
    return m_vertices.size();
}
    

//-----------------------------------------------------------------------------
//  getTotalWidth
//-----------------------------------------------------------------------------
const double StaticTerrain::getTotalWidth() const
{
    return m_totalWidth;
}

//-----------------------------------------------------------------------------
//  getTotalHeight
//-----------------------------------------------------------------------------
const double StaticTerrain::getTotalHeight() const
{
    return m_totalHeight;
}
