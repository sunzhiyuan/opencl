//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#include "FrmPlatform.h"
#include "HoffmanScatter.hpp"
#include <OpenGLES/FrmShader.h>
#include "FrmMath.h"

	static const double    dEpsilon      =   1.0e-5;
	static const double    dPI           =   3.14159265358979323846;
	static const double    dDegToRad     =   dPI / 180.0;

//-----------------------------------------------------------------------------
//  Default constructor
//-----------------------------------------------------------------------------
HoffmanScatter::HoffmanScatter()
: m_HGg( 0.5406f )
, m_inscatteringMultiplier( 0.087006f )//1.0f )
, m_betaRayMultiplier( 0.2f )//1.0f )
, m_betaMieMultiplier( 0.02399f )//1.0f )
, m_sunIntensity( 1.0f )
, m_turbitity( 0.2140f )//2.0f )
{
    const float n = 1.0003f;
    const float N = 2.545e25f;
    const float pn = 0.035f;

    float fLambda[3], fLambda2[3], fLambda4[3];

    fLambda[0]  = 1.0f / 650e-9f;   // red
    fLambda[1]  = 1.0f / 570e-9f;   // green
    fLambda[2]  = 1.0f / 475e-9f;   // blue

    for( int i = 0; i < 3; ++i )
    {
        fLambda2[i] = fLambda[i]*fLambda[i];
        fLambda4[i] = fLambda2[i]*fLambda2[i];
    }

    FRMVECTOR3 vLambda2( fLambda2[0], fLambda2[1], fLambda2[2] ); 
    FRMVECTOR3 vLambda4( fLambda4[0], fLambda4[1], fLambda4[2] ); 

    // Rayleigh scattering constants

    const float fTemp = (float)(dPI * dPI * (n * n - 1.0f) * (n * n - 1.0f) * (6.0f + 3.0f * pn) / (6.0f - 7.0f * pn) / N);
    const float fBeta = (float)(8.0f * fTemp * dPI / 3.0f);
 
    m_betaRay = fBeta * vLambda4;

    const float fBetaDash = fTemp / 2.0f;

    m_betaDashRay = fBetaDash * vLambda4;

    // Mie scattering constants

    const float T       = 2.0f;
    const float c       = (6.544f * T - 6.51f) * 1e-17f;
    const float fTemp2  = (float)(0.434f * c * (2.0f * dPI) * (2.0f* dPI) * 0.5f);

    m_betaDashMie = fTemp2 * vLambda2;

    const float K[3]    = {0.685f, 0.679f, 0.670f};
    const float fTemp3  = (float)( 0.434f * c * dPI * (2.0f * dPI) * (2.0f * dPI));

    FRMVECTOR3 vBetaMieTemp( K[0] * fLambda2[0], K[1] * fLambda2[1], K[2] * fLambda2[2] );

    m_betaMie = fTemp3 * vBetaMieTemp;
}

//-----------------------------------------------------------------------------
//  Destructor
//-----------------------------------------------------------------------------
HoffmanScatter::~HoffmanScatter()
{}

//-----------------------------------------------------------------------------
//  create
//-----------------------------------------------------------------------------
bool HoffmanScatter::create()
{
    // Create sky and terrain shader programs

	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/HoffmanScatter30.vs",
                                                  "Samples/Shaders/HoffmanScatter30.fs",
                                                  &m_HoffmanScatterShader,
												  NULL, 0 ) )
	{
        return FALSE;
	}

    m_validSkyLight = true;
    m_validAerialPerspective = true;

	m_cgPosition = glGetAttribLocation( m_HoffmanScatterShader, "g_vPositionOS" );
    m_cgEyePos                  = glGetUniformLocation( m_HoffmanScatterShader, "EyePos" );
    m_cgSunDir                  = glGetUniformLocation( m_HoffmanScatterShader, "SunDir" );
    m_cgBetaRPlusBetaM          = glGetUniformLocation( m_HoffmanScatterShader, "BetaRPlusBetaM" );
    m_cgHGg                     = glGetUniformLocation( m_HoffmanScatterShader, "HGg" );
    m_cgBetaDashR               = glGetUniformLocation( m_HoffmanScatterShader, "BetaDashR" );
    m_cgBetaDashM               = glGetUniformLocation( m_HoffmanScatterShader, "BetaDashM" );
    m_cgOneOverBetaRPlusBetaM   = glGetUniformLocation( m_HoffmanScatterShader, "OneOverBetaRPlusBetaM" );
    m_cgMultipliers             = glGetUniformLocation( m_HoffmanScatterShader, "Multipliers" );
    m_cgSunColorAndIntensity    = glGetUniformLocation( m_HoffmanScatterShader, "SunColorAndIntensity" );
	m_cgMVP                      = glGetUniformLocation( m_HoffmanScatterShader, "g_matModelViewProj" );
    m_cgMV                      = glGetUniformLocation( m_HoffmanScatterShader, "g_matModelView" );

	//
	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/HoffmanScatter30.vs",
                                                  "Samples/Shaders/HoffmanScatterTerrain30.fs",
                                                  &m_HoffmanScatterTerrainShader,
												  NULL, 0 ) )
	{
        return FALSE;
	}

	m_cgPositionTerrain				   = glGetAttribLocation( m_HoffmanScatterTerrainShader, "g_vPositionOS" );
	m_cgTexCoordTerrain				   = glGetAttribLocation( m_HoffmanScatterTerrainShader, "g_vTexCoord" );

	m_hBaseTerrain					   = glGetUniformLocation( m_HoffmanScatterTerrainShader, "g_sBaseTexture" );
    m_hNormalTerrain				   = glGetUniformLocation( m_HoffmanScatterTerrainShader, "g_sNormTexture" );

    m_cgEyePosTerrain                  = glGetUniformLocation( m_HoffmanScatterTerrainShader, "EyePos" );
    m_cgSunDirTerrain                  = glGetUniformLocation( m_HoffmanScatterTerrainShader, "SunDir" );
    m_cgBetaRPlusBetaMTerrain          = glGetUniformLocation( m_HoffmanScatterTerrainShader, "BetaRPlusBetaM" );
    m_cgHGgTerrain                     = glGetUniformLocation( m_HoffmanScatterTerrainShader, "HGg" );
    m_cgBetaDashRTerrain               = glGetUniformLocation( m_HoffmanScatterTerrainShader, "BetaDashR" );
    m_cgBetaDashMTerrain               = glGetUniformLocation( m_HoffmanScatterTerrainShader, "BetaDashM" );
    m_cgOneOverBetaRPlusBetaMTerrain   = glGetUniformLocation( m_HoffmanScatterTerrainShader, "OneOverBetaRPlusBetaM" );
    m_cgMultipliersTerrain             = glGetUniformLocation( m_HoffmanScatterTerrainShader, "Multipliers" );
    m_cgSunColorAndIntensityTerrain    = glGetUniformLocation( m_HoffmanScatterTerrainShader, "SunColorAndIntensity" );
	m_cgMVPTerrain                     = glGetUniformLocation( m_HoffmanScatterTerrainShader, "g_matModelViewProj" );
    m_cgMVTerrain                      = glGetUniformLocation( m_HoffmanScatterTerrainShader, "g_matModelView" );

	
	return TRUE;
	return TRUE;
}

//-----------------------------------------------------------------------------
//  setSunIntensity
//-----------------------------------------------------------------------------
void HoffmanScatter::setSunIntensity( const float a_value )
{
    m_sunIntensity = a_value;
}

//-----------------------------------------------------------------------------
//  setMie
//-----------------------------------------------------------------------------
void HoffmanScatter::setMie( const float a_value )
{
    m_betaMieMultiplier = a_value;
}

//-----------------------------------------------------------------------------
//  setRayleigh
//-----------------------------------------------------------------------------
void HoffmanScatter::setRayleigh( const float a_value )
{
    m_betaRayMultiplier = a_value;
}

//-----------------------------------------------------------------------------
//  setHg
//-----------------------------------------------------------------------------
void HoffmanScatter::setHg( const float a_value )
{
    m_HGg = a_value;
}

//-----------------------------------------------------------------------------
//  setInscattering
//-----------------------------------------------------------------------------
void HoffmanScatter::setInscattering( const float a_value )
{
    m_inscatteringMultiplier = a_value;
}

//-----------------------------------------------------------------------------
//  setTurbidity
//-----------------------------------------------------------------------------
void HoffmanScatter::setTurbidity( const float a_value )
{
    m_turbitity = a_value;
}

//-----------------------------------------------------------------------------
//  isSkyLightValid
//-----------------------------------------------------------------------------
const bool HoffmanScatter::isSkyLightValid() const
{
    return m_validSkyLight;
}

//-----------------------------------------------------------------------------
//  isAerialPerspectiveValid
//-----------------------------------------------------------------------------
const bool HoffmanScatter::isAerialPerspectiveValid() const
{
    return m_validAerialPerspective;
}
//-----------------------------------------------------------------------------
//  renderSkyLight
//-----------------------------------------------------------------------------
void HoffmanScatter::renderSkyLight( FRMVECTOR3& a_sunDirection                                   
								   , const unsigned int vao
                                   , const unsigned int vbo
                                   , const unsigned int ibo
								   , const unsigned int a_indexCount
                                   )
{
    // Compute thetaS dependencies

    FRMVECTOR3 vecZenith( 0.0f, 1.0f, 0.0f );

    float   fThetaS = FrmVector3Dot( a_sunDirection, vecZenith );
            fThetaS = acosf( fThetaS );

    computeAttenuation( fThetaS );

    // Set shader constants
	glUseProgram( m_HoffmanScatterShader );
    setShaderConstants( a_sunDirection );

    // Draw sky dome
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	
	glEnableVertexAttribArray( m_cgPosition );
	glVertexAttribPointer( m_cgPosition , 3, GL_FLOAT, 0, 0, 0 );	

    glDisable( GL_DEPTH_TEST );

	glDrawElements          ( GL_TRIANGLE_STRIP, //GL_LINES,
				a_indexCount, GL_UNSIGNED_INT, 0 );
    glEnable( GL_DEPTH_TEST );

	glDisableVertexAttribArray( m_cgPosition );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void HoffmanScatter::renderTerrain( FRMVECTOR3& a_sunDirection
                                   , const float* a_pVertices
								   , const unsigned int a_pVertexCount
                                   , const unsigned int* a_pIndices
								   , const float* a_pTexCoords
                                   , const unsigned int a_indexCount
                                   , const unsigned int a_terrainMap
                                   , const unsigned int a_normalMap
								   )
{
    // Compute thetaS dependencies

    FRMVECTOR3 vecZenith( 0.0f, 1.0f, 0.0f );

    float   fThetaS = FrmVector3Dot( a_sunDirection, vecZenith );
            fThetaS = acosf( fThetaS );

    computeAttenuation( fThetaS );

    // Set shader constants
	glUseProgram( m_HoffmanScatterTerrainShader );
    setShaderConstants(a_sunDirection );

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo[2];
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * a_pVertexCount, a_pVertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray( m_cgPositionTerrain );
	glVertexAttribPointer( m_cgPositionTerrain , 3, GL_FLOAT, 0, 0, 0 );
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * a_pVertexCount, a_pTexCoords, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray( m_cgTexCoordTerrain );
	glVertexAttribPointer( m_cgTexCoordTerrain , 3, GL_FLOAT, 0, 0, 0 );

	
	//Set up base and normal map
	glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, a_terrainMap );
    glUniform1i( m_hBaseTerrain, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, a_normalMap );
    glUniform1i( m_hNormalTerrain, 1 );

//	glDisable( GL_CULL_FACE );

	glDrawElements          ( GL_TRIANGLE_STRIP, //GL_LINES, //
				a_indexCount, GL_UNSIGNED_INT, a_pIndices );

	glDisableVertexAttribArray( m_cgTexCoordTerrain );
	glDisableVertexAttribArray( m_cgPositionTerrain );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

//*********************************************************************************************************************
//*********************************************************************************************************************
//**    Private methods
//*********************************************************************************************************************
//*********************************************************************************************************************

//-----------------------------------------------------------------------------
//  computeAttenuation
//-----------------------------------------------------------------------------
void HoffmanScatter::computeAttenuation( const float a_theta )
{
    float fBeta = 0.04608365822050f * m_turbitity - 0.04586025928522f;
    float fTauR, fTauA, fTau[3];
    float m = (float)(1.0f / (cosf( a_theta ) + 0.15f * powf( 93.885f - a_theta / (float)dPI * 180.0f, -1.253f )));  // Relative Optical Mass
    float fLambda[3] = {0.65f, 0.57f, 0.475f }; 

    for( int i = 0; i < 3; ++i )
    {
        // Rayleigh Scattering
        // lambda in um.

        fTauR = expf( -m * 0.008735f * powf( fLambda[i], float(-4.08f) ) );

        // Aerosal (water + dust) attenuation
        // beta - amount of aerosols present 
        // alpha - ratio of small to large particle sizes. (0:4,usually 1.3)

        const float fAlpha = 1.3f;
        fTauA = expf(-m * fBeta * powf(fLambda[i], -fAlpha));  // lambda should be in um


        fTau[i] = fTauR * fTauA; 

    }

    m_sunColorAndIntensity = FRMVECTOR4( fTau[0], fTau[1], fTau[2], m_sunIntensity * 100.0f );
}

//-----------------------------------------------------------------------------
//  setShaderConstants
//-----------------------------------------------------------------------------
void HoffmanScatter::setShaderConstants( FRMVECTOR3& a_sunDirection)
{
    float fReflectance = 0.1f;

    FRMVECTOR3 vecBetaR           ( m_betaRay * m_betaRayMultiplier );
    FRMVECTOR3 vecBetaDashR       ( m_betaDashRay * m_betaRayMultiplier );
    FRMVECTOR3 vecBetaM           ( m_betaMie * m_betaMieMultiplier );
    FRMVECTOR3 vecBetaDashM       ( m_betaDashMie * m_betaMieMultiplier );
    FRMVECTOR3 vecBetaRM          ( vecBetaR + vecBetaM );
    FRMVECTOR3 vecOneOverBetaRM   ( 1.0f / vecBetaRM.x, 1.0f / vecBetaRM.y, 1.0f / vecBetaRM.z );
    FRMVECTOR3 vecG               ( 1.0f - m_HGg * m_HGg, 1.0f + m_HGg * m_HGg, 2.0f * m_HGg );
    FRMVECTOR4 vecTermMulitpliers ( m_inscatteringMultiplier, 0.138f * fReflectance, 0.113f * fReflectance, 0.08f * fReflectance );


	glUniform3f			 ( m_cgEyePos, 1.0f, 1.0f, 1.0f);
	glUniform3fv		 ( m_cgSunDir, 1, (float *)& a_sunDirection );
    glUniform3fv         ( m_cgBetaRPlusBetaM, 1, (float *)& vecBetaRM );
    glUniform3fv         ( m_cgHGg, 1, (float *)& vecG );
    glUniform3fv         ( m_cgBetaDashR, 1, (float *)& vecBetaDashR );
    glUniform3fv         ( m_cgBetaDashM, 1, (float *)& vecBetaDashM );
    glUniform3fv         ( m_cgOneOverBetaRPlusBetaM, 1, (float *)& vecOneOverBetaRM );
    glUniform4fv         ( m_cgMultipliers, 1, (float *)& vecTermMulitpliers);
    glUniform4fv         ( m_cgSunColorAndIntensity, 1, (float *)& m_sunColorAndIntensity );

	glUniformMatrix4fv   ( m_cgMV, 1, FALSE, (float *) &m_matCameraMeshModelView );
	glUniformMatrix4fv   ( m_cgMVP, 1, FALSE, (float *) &m_matCameraMeshModelViewProj );

//and for the terrain..
	glUniform3f		( m_cgEyePosTerrain, 1.0f, 1.0f, 1.0f);
	glUniform3fv		 ( m_cgSunDirTerrain, 1, (float *)& a_sunDirection);
    glUniform3fv         ( m_cgBetaRPlusBetaMTerrain, 1, (float *)& vecBetaRM );
    glUniform3fv         ( m_cgHGgTerrain, 1, (float *)& vecG );
    glUniform3fv         ( m_cgBetaDashRTerrain, 1, (float *)& vecBetaDashR );
    glUniform3fv         ( m_cgBetaDashMTerrain, 1, (float *)& vecBetaDashM );
    glUniform3fv         ( m_cgOneOverBetaRPlusBetaMTerrain, 1, (float *)& vecOneOverBetaRM );
    glUniform4fv         ( m_cgMultipliersTerrain, 1, (float *)& vecTermMulitpliers  );
    glUniform4fv         ( m_cgSunColorAndIntensityTerrain, 1, (float *)& m_sunColorAndIntensity );

	glUniformMatrix4fv   ( m_cgMVTerrain, 1, FALSE, (float *) &m_matCameraMeshModelView );
	glUniformMatrix4fv   ( m_cgMVPTerrain, 1, FALSE, (float *) &m_matCameraMeshModelViewProj );
}
