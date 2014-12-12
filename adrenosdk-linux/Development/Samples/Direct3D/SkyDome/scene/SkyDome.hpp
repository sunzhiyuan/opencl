//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#ifndef         SKYDOME_H_INCLUDED
#define         SKYDOME_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <vector>
#include		"FrmMath.h"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  SkyDome
//-----------------------------------------------------------------------------
//
//! Sky dome hemisphere and associated scatter class
//
//-----------------------------------------------------------------------------
class SkyDome
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new sky dome
                        SkyDome         ();
    //! \brief  Destructor
                        ~SkyDome        ();

    //! \brief  Creates the sky dome from the given parameter
    //! \param  [in] The hemisphere radius
    //! \param  [in] The hemisphere height
    //! \param  [in] The number of rings for the hemisphere
    //! \param  [in] The number of segmants for the hemisphere
    void                create          ( const double a_radius
                                        , const double a_height
                                        , const int a_rings
                                        , const int a_segments
										, const double lowered
                                        );
    //! \brief  Destroys the sky dome
    void                destroy         ();

    //
    // Accessors
    //

    //! \brief  Returns the sky dome vertices
    //! \return The sky dome vertices
    const FRMVECTOR3*     getVertices     () const;
    
    //! \brief  Returns the number of sky dome vertices
    //! \return The number of sky dome vertices
    const size_t        getVertexCount   () const;

    
    //! \brief  Returns the sky dome triangle strip indices
    //! \return The sky dome indices
    const unsigned int* getIndices      () const;
    //! \brief  Returns the sky dome triangle strip index count
    //! \return The index count
    const size_t        getIndexCount   () const;

    //! \brief  Sets the current sun direction
    //! \param  [in] The sun direction to set
    void                setSunDirection ( const FRMVECTOR3& a_direction );
    //! \brief  Returns the current sun direction
    //! \return The current sun direction
    FRMVECTOR3     getSunDirection () const;

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a SkyDome from the specified one
    //! \param  [in] The SkyDome to copy from
                        SkyDome         ( const SkyDome& );
    //! \brief  Sets this SkyDome to the specified one
    //! \param  [in] The SkyDome to copy from
    //! \return The modified SkyDome
    SkyDome&            operator=       ( const SkyDome& );

    //
    // Dome data
    //

    std::vector< FRMVECTOR3 >     m_vertices;     //!< Hemisphere vertices
    std::vector< unsigned int >   m_indices;      //!< Hemisphere indices
    FRMVECTOR3                    m_sunDirection; //!< Current sun direction

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // SKYDOME_H_INCLUDED
