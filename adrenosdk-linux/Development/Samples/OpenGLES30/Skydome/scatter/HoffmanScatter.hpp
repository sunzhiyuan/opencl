//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef         HOFFMANSCATTER_H_INCLUDED
#define         HOFFMANSCATTER_H_INCLUDED

#ifdef          _MSC_VER
#if             _MSC_VER > 1000
#pragma once
#endif
#endif

#include		"FrmMath.h"

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

//-----------------------------------------------------------------------------
//  HoffmanScatter
//-----------------------------------------------------------------------------
//
//! Implements the scattering model as described by Hoffman et al.
//
//-----------------------------------------------------------------------------
class HoffmanScatter
{

public:

    //
    // Con-/Destruction
    //

    //! \brief  Constructs a new HoffmanScatter
                    HoffmanScatter          ();
    //! \brief  Destructor
                    ~HoffmanScatter         ();

    //! \brief  Initializes the Cg shader
    bool            create                  ();

    //
    // Accessors
    //

    //! \brief  Sets the current sun intensity
    //! \param  [in] The sun intensity to set
    void            setSunIntensity         ( const float a_value );
    //! \brief  Sets the current Mie scattering coefficient
    //! \param  [in] The Mie scattering coefficient to set
    void            setMie                  ( const float a_value );
    //! \brief  Sets the current Rayleigh scattering coefficient
    //! \param  [in] The Rayleigh scattering coefficient to set
    void            setRayleigh             ( const float a_value );
    //! \brief  Sets the current Henyey Greenstein g-value
    //! \param  [in] The Henyey Greenstein g-value to set
    void            setHg                   ( const float a_value );
    //! \brief  Sets the current inscattering factor
    //! \param  [in] The inscattering factor to set
    void            setInscattering         ( const float a_value );
    //! \brief  Sets the current turbiditiy
    //! \param  [in] The turbiditiy to set
    void            setTurbidity            ( const float a_value );

	float getHg() const { return m_HGg;}
	float getInscattering() const { return m_inscatteringMultiplier;}
	float getMie() const { return m_betaMieMultiplier;}
	float getTurbidity() const { return m_turbitity;}
    //
    // Interface IScatter implementation
    //

    //! \brief  Returns whether the skylight renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    const bool      isSkyLightValid         () const;
    //! \brief  Returns whether the aerial perspective renderer is valid
    //! \return The method returns either of the following values
    //! \retval true    | The renderer is valid
    //! \retval false   | The renderer is not valid
    const bool      isAerialPerspectiveValid() const;

    //! \brief  Renders skydome and skylight
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    //! \param  [in] The sky dome vertices
    //! \param  [in] The sky dome indices
    //! \param  [in] The sky dome index count
    void            renderSkyLight          ( FRMVECTOR3& a_sunDirection                                            
											, const unsigned int vao
                                            , const unsigned int vbo
                                            , const unsigned int ibo
											, const unsigned int a_indexCount
                                            );

	//! \brief  Renders terrain
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    //! \param  [in] The terrain vertices
    //! \param  [in] The terrain indices
    //! \param  [in] The terrain index count
    void            renderTerrain          ( FRMVECTOR3& a_sunDirection
                                            , const float* a_pVertices
											, const unsigned int a_pVertexCount
                                            , const unsigned int* a_pIndices
											, const float* a_pTexCoords
                                            , const unsigned int a_indexCount
											, const unsigned int a_terrainMap
                                            , const unsigned int a_normalMap
                                            );

	void SetViewAndProjMat(const FRMMATRIX4X4& mvIn, const FRMMATRIX4X4& mvpIn)
	{
		m_matCameraMeshModelViewProj = mvpIn;
		m_matCameraMeshModelView = mvIn;
	}

private:

    //
    // Construction and assignment
    //

    //! \brief  Creates a HoffmanScatter from the specified one
    //! \param  [in] The HoffmanScatter to copy from
    HoffmanScatter          ( const HoffmanScatter& );
    //! \brief  Sets this HoffmanScatter to the specified one
    //! \param  [in] The HoffmanScatter to copy from
    //! \return The modified HoffmanScatter
    HoffmanScatter& operator=               ( const HoffmanScatter& );

    //
    // Helper
    //

    //! \brief  Computes the sun attenuation for the specified sun angle
    //! \param  [in] The sun angle to use
    void            computeAttenuation      ( const float a_theta );
    //! \brief  Updates the shader constants
    //! \param  [in] The camera to use
    //! \param  [in] The current sun direction
    void            setShaderConstants      ( FRMVECTOR3& a_sunDirection );

    //
    // Atmosphere data
    //

    float       m_HGg;                      //!< g value in Henyey Greenstein approximation function
    float       m_inscatteringMultiplier;   //!< Multiply inscattering term with this factor
    float       m_betaRayMultiplier;        //!< Multiply Rayleigh scattering coefficient with this factor
    float       m_betaMieMultiplier;        //!< Multiply Mie scattering coefficient with this factor
    float       m_sunIntensity;             //!< Current sun intensity
    float       m_turbitity;                //!< Current turbidity

    FRMVECTOR3    m_betaRay;                  //!< Rayleigh total scattering coefficient
    FRMVECTOR3    m_betaDashRay;              //!< Rayleigh angular scattering coefficient without phase term
    FRMVECTOR3    m_betaMie;                  //!< Mie total scattering coefficient
    FRMVECTOR3    m_betaDashMie;              //!< Mie angular scattering coefficient without phase term
    FRMVECTOR4    m_sunColorAndIntensity;     //!< Sun color and intensity packed for the shader

    //
    // Cg data
    //

    static char s_vertexShaderSource[];            //!< Shared Hoffman vertex shader source
    static char s_skyFragmentShaderSource[];       //!< Sky fragment shader source
    static char s_terrainFragmentShaderSource[];   //!< Terrain fragment shader source

	UINT32 m_HoffmanScatterShader;
	UINT32 m_HoffmanScatterTerrainShader;
    UINT32   m_cgTerrainFragmentShader;  //!< Terrain fragment shader

    INT32 m_cgPosition;               //!< Shared vertex shader position parameter
    INT32 m_cgEyePos;                 //!< Shared vertex shader eye position parameter
    INT32 m_cgSunDir;                 //!< Shared vertex shader sun direction parameter
    INT32 m_cgBetaRPlusBetaM;         //!< Shared vertex shader scattering coefficients parameter
    INT32 m_cgHGg;                    //!< Shared vertex shader Henyey Greenstein parameter
    INT32 m_cgBetaDashR;              //!< Shared vertex shader scattering coefficients parameter
    INT32 m_cgBetaDashM;              //!< Shared vertex shader scattering coefficients parameter
    INT32 m_cgOneOverBetaRPlusBetaM;  //!< Shared vertex shader scattering coefficients parameter
    INT32 m_cgMultipliers;            //!< Shared vertex shader multiplier parameter
    INT32 m_cgSunColorAndIntensity;   //!< Shared vertex shader sun color/intensity parameter
    INT32 m_cgMVP;                    //!< Shared vertex shader model/view/projection matrix parameter
    INT32 m_cgMV;                     //!< Shared vertex shader model/view matrix parameter

	INT32 m_cgPositionTerrain;               //!< Shared vertex shader position parameter
    INT32 m_cgTexCoordTerrain;

	INT32 m_hBaseTerrain;		  //!< texture sampler location
	INT32 m_hNormalTerrain;		  //!< texture sampler location
	
    INT32 m_cgEyePosTerrain;                 //!< Shared vertex shader eye position parameter
    INT32 m_cgSunDirTerrain;                 //!< Shared vertex shader sun direction parameter
    INT32 m_cgBetaRPlusBetaMTerrain;         //!< Shared vertex shader scattering coefficients parameter
    INT32 m_cgHGgTerrain;                    //!< Shared vertex shader Henyey Greenstein parameter
    INT32 m_cgBetaDashRTerrain;              //!< Shared vertex shader scattering coefficients parameter
    INT32 m_cgBetaDashMTerrain;              //!< Shared vertex shader scattering coefficients parameter
    INT32 m_cgOneOverBetaRPlusBetaMTerrain;  //!< Shared vertex shader scattering coefficients parameter
    INT32 m_cgMultipliersTerrain;            //!< Shared vertex shader multiplier parameter
    INT32 m_cgSunColorAndIntensityTerrain;   //!< Shared vertex shader sun color/intensity parameter
    INT32 m_cgMVPTerrain;                    //!< Shared vertex shader model/view/projection matrix parameter
    INT32 m_cgMVTerrain;                     //!< Shared vertex shader model/view matrix parameter

    INT32 m_cgLightVector;            //!< Terrain fragment shader light vector parameter
    INT32 m_cgTerrainMap;             //!< Terrain fragment shader texture map parameter
    INT32 m_cgNormalMap;              //!< Terrain fragment shader normal map parameter

    bool        m_validSkyLight;            //!< Flag indicating whether the skylight shader is loadable
    bool        m_validAerialPerspective;   //!< Flag indicating whether the airlight shader is loadable

	FRMMATRIX4X4 m_matCameraMeshModelViewProj;
	FRMMATRIX4X4 m_matCameraMeshModelView;
};

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

#endif // HOFFMANSCATTER_H_INCLUDED
