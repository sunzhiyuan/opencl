//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#ifndef         HEIGHTFIELD_H_INCLUDED
#define         HEIGHTFIELD_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include        <vector>

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  HeightField
//-----------------------------------------------------------------------------
//
//! Basic image-based height field
//
//-----------------------------------------------------------------------------
class HeightField
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new empty height field
                        HeightField         ();
    //! \brief  Destructor
                        ~HeightField        ();

    //! \brief  Creates the heigt data from the given parameter
    //! \param  [in] Pointer to the raw RGB image data
    //! \param  [in] The image width
    //! \param  [in] The image height
    //! \param  [in] The scale value to apply to each height
    //! \param  [in] x/y grid space to use for this field
    void                createFromRGBImage  ( const unsigned char* a_pImageData
                                            , const unsigned int a_imageWidth
                                            , const unsigned int a_imageHeight
                                            , const double a_heightScale
                                            , const double a_gridSpacing
                                            );

    //! \brief  Destroys the heigt data
    void                destroy             ();

    //
    // Accessors
    //

    //! \brief  Returns the field's dimension on the x-axis
    //! \return The field's dimension on the x-axis
    const unsigned int  getFieldWidth       () const;
    //! \brief  Returns the field's dimension on the y-axis
    //! \return The field's dimension on the y-axis
    const unsigned int  getFieldHeight      () const;
    //! \brief  Returns the heigt at the specified coordinates
    //! \return The height value at the specified coordinates
    const double        getHeight           ( const unsigned int a_x
                                            , const unsigned int a_y
                                            ) const;
    //! \brief  Returns the field's grid spacing
    //! \return The x/y grid space to use for this field
    const double        getGridSpacing      () const;

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a HeightField from the specified one
    //! \param  [in] The HeightField to copy from
                        HeightField         ( const HeightField& );
    //! \brief  Sets this HeightField to the specified one
    //! \param  [in] The HeightField to copy from
    //! \return The modified HeightField
    HeightField&        operator=           ( const HeightField& );

    //
    // Field data
    //

    std::vector< double >   m_heights;      //!< The field heights
    unsigned int            m_width;        //!< The field's dimension on the x-axis
    unsigned int            m_height;       //!< The field's dimension on the y-axis
    double                  m_gridSpacing;  //!< x/y grid space to use for this field

};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // HEIGHTFIELD_H_INCLUDED
