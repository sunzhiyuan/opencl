//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#ifndef         STATICTERRAIN_H_INCLUDED
#define         STATICTERRAIN_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <vector>

class           HeightField;

//-----------------------------------------------------------------------------
//  StaticTerrain
//-----------------------------------------------------------------------------
//
//! Basic static landscape mesh
//
//-----------------------------------------------------------------------------
class StaticTerrain
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new empty landscape
                        StaticTerrain   ();
    //! \brief  Destructor
                        ~StaticTerrain  ();

    //! \brief  Creates the landscape from the given height field
    //! \param  [in] The height field to use
    void                create          ( const HeightField& a_heightField );

	void GenTerrainAndNormalTextures( const unsigned char* a_terrainMap
												, const int a_terrainMapWidth
												, const int a_terrainMapHeight
												, const unsigned char* a_normalMap
												, const int a_normalMapWidth
												, const int a_normalMapHeight);
    //! \brief  Destroys the landscape
    void                destroy         ();

    //
    // Accessors
    //

    //! \brief  Returns the landscape vertices
    //! \return The landscape vertices
    const FRMVECTOR3*     getVertices     () const;
    //! \brief  Returns the landscape texture coordinates
    //! \return The landscape texture coordinates
    const FRMVECTOR3*     getTexCoords    () const;
    //! \brief  Returns the landscape triangle strip indices
    //! \return The landscape indices
    const unsigned int* getIndices      () const;

    //! \brief  Returns the landscape triangle strip index count
    //! \return The index count
    const size_t        getIndexCount   () const;
    //! \brief  Returns the landscape total width
    //! \return The total width
    const double        getTotalWidth   () const;
    //! \brief  Returns the landscape total height
    //! \return The total height
    const double        getTotalHeight  () const;

	const unsigned int  getTerrainMap() const {return m_terrainMap;}
	const unsigned int  getNormalMap() const {return m_normalMap;}

	const unsigned int getVertexCount() const {return m_vertices.size();}
	const unsigned int getTexCoordCount() const {return m_texCoords.size();}

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a StaticTerrain from the specified one
    //! \param  [in] The StaticTerrain to copy from
                        StaticTerrain   ( const StaticTerrain& );
    //! \brief  Sets this StaticTerrain to the specified one
    //! \param  [in] The StaticTerrain to copy from
    //! \return The modified StaticTerrain
    StaticTerrain&      operator=       ( const StaticTerrain& );

    //
    // Terrain data
    //

    std::vector< FRMVECTOR3 >     m_vertices;  //!< Terrain vertices
    std::vector< FRMVECTOR3 >     m_texCoords; //!< Terrain texture coordinates
    std::vector< unsigned int > m_indices;   //!< Terrain indices

    double m_totalWidth;    // Total x-dimension of the terrain
    double m_totalHeight;   // Total y-dimension of the terrain

	unsigned int            m_terrainMap;               //!< The terrain texture map
    unsigned int            m_normalMap;                //!< The terrain normal map

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // STATICTERRAIN_H_INCLUDED
