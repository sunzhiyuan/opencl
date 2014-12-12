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
#include <Direct3D/FrmShader.h>
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
, m_pHoffmanScatterConstantBuffer(NULL)
, m_pSkyLightVertexBuffer(NULL)
, m_pSkyLightIndexBuffer(NULL)
, m_pTerrainVertexBuffer(NULL)
, m_pTerrainIndexBuffer(NULL)
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
{
    if ( m_pHoffmanScatterConstantBuffer ) m_pHoffmanScatterConstantBuffer->Release();
    
    if ( m_pSkyLightVertexBuffer ) m_pSkyLightVertexBuffer->Release();
    if ( m_pSkyLightIndexBuffer ) m_pSkyLightIndexBuffer->Release();

    if ( m_pTerrainVertexBuffer ) m_pTerrainVertexBuffer->Release();
    if ( m_pTerrainIndexBuffer ) m_pTerrainIndexBuffer->Release();

}

//-----------------------------------------------------------------------------
//  create
//-----------------------------------------------------------------------------
bool HoffmanScatter::create()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    {
        { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32_FLOAT },
        { "TEXCOORD", FRM_VERTEX_TEXCOORD0, DXGI_FORMAT_R32G32B32_FLOAT }
    };
    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

    // Create sky and terrain shader programs
	if( FALSE == m_HoffmanScatterShader.Compile( "HoffmanScatterVS.cso", "HoffmanScatterPS.cso", pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

    if ( FALSE == FrmCreateConstantBuffer(sizeof(m_HoffmanScatterConstantBufferData), &m_HoffmanScatterConstantBufferData, &m_pHoffmanScatterConstantBuffer) )
    {
        return FALSE;
    }


    m_validSkyLight = true;
    m_validAerialPerspective = true;


    if( FALSE == m_HoffmanScatterTerrainShader.Compile( "HoffmanScatterVS.cso", "HoffmanScatterTerrainPS.cso", pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

    D3D11_DEPTH_STENCIL_DESC dsdesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DefaultDepthStencilState);
    dsdesc.DepthEnable = false;
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DisabledDepthStencilState);


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
                                   , const float* a_pVertices
                                   , const unsigned int a_vertexCount
                                   , const unsigned int* a_pIndices
                                   , const unsigned int a_indexCount
                                   )
{
    if ( m_pSkyLightVertexBuffer == NULL )
    {
        // Copy to a single buffer
        float* vertBufData = new float [ 6 * a_vertexCount ] ;

        for (unsigned int i = 0; i < a_vertexCount; i++)
        {
            vertBufData[i * 6 + 0] = a_pVertices[i * 3 + 0];
            vertBufData[i * 6 + 1] = a_pVertices[i * 3 + 1];
            vertBufData[i * 6 + 2] = a_pVertices[i * 3 + 2];
            vertBufData[i * 6 + 3] = 0.0f;
            vertBufData[i * 6 + 4] = 0.0f;
            vertBufData[i * 6 + 5] = 0.0f;
        }

        FrmCreateVertexBuffer( a_vertexCount, sizeof(float) * 6, vertBufData, &m_pSkyLightVertexBuffer );
        FrmCreateIndexBuffer( a_indexCount, sizeof(unsigned int), (void*) a_pIndices, &m_pSkyLightIndexBuffer );

        delete [] vertBufData;
    }
    
    // Compute thetaS dependencies

    FRMVECTOR3 vecZenith( 0.0f, 1.0f, 0.0f );

    float   fThetaS = FrmVector3Dot( a_sunDirection, vecZenith );
            fThetaS = acosf( fThetaS );

    computeAttenuation( fThetaS );

    // Set shader constants
    m_HoffmanScatterShader.Bind();
    setShaderConstants( a_sunDirection );
    m_pHoffmanScatterConstantBuffer->Update(&m_HoffmanScatterConstantBufferData);
    m_pHoffmanScatterConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS);

    // Draw sky dome
    m_pSkyLightVertexBuffer->Bind( 0 );
    m_pSkyLightIndexBuffer->Bind( 0 );

    
    // Disable depth test
    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 1 );

    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, a_indexCount, sizeof(unsigned int), 0 );

    // Enable depth
    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 1 );
}

void HoffmanScatter::renderTerrain( FRMVECTOR3& a_sunDirection
                                   , const float* a_pVertices
                                   , const unsigned int* a_pIndices
								   , const float* a_pTexCoords
                                   , const unsigned int a_vertexCount
                                   , const unsigned int a_indexCount
                                   , CFrmTexture* a_terrainMap
                                   , CFrmTexture* a_normalMap
								   )
{

    if ( m_pTerrainVertexBuffer == NULL )
    {
        // Copy to a single buffer
        float* vertBufData = new float [ 6 * a_vertexCount ] ;

        for (unsigned int i = 0; i < a_vertexCount; i++)
        {
            vertBufData[i * 6 + 0] = a_pVertices[i * 3 + 0];
            vertBufData[i * 6 + 1] = a_pVertices[i * 3 + 1];
            vertBufData[i * 6 + 2] = a_pVertices[i * 3 + 2];
            vertBufData[i * 6 + 3] = a_pTexCoords[i * 3 + 0];
            vertBufData[i * 6 + 4] = a_pTexCoords[i * 3 + 1];
            vertBufData[i * 6 + 5] = a_pTexCoords[i * 3 + 2];
        }

        FrmCreateVertexBuffer( a_vertexCount, sizeof(float) * 6, vertBufData, &m_pTerrainVertexBuffer );
        FrmCreateIndexBuffer( a_indexCount, sizeof(unsigned int), (void*) a_pIndices, &m_pTerrainIndexBuffer );

        delete [] vertBufData;
    }

    // Compute thetaS dependencies

    FRMVECTOR3 vecZenith( 0.0f, 1.0f, 0.0f );

    float   fThetaS = FrmVector3Dot( a_sunDirection, vecZenith );
            fThetaS = acosf( fThetaS );

    computeAttenuation( fThetaS );

    // Set shader constants
    m_HoffmanScatterTerrainShader.Bind();
    setShaderConstants(a_sunDirection);

    m_pHoffmanScatterConstantBuffer->Update(&m_HoffmanScatterConstantBufferData);
    m_pHoffmanScatterConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS);

    m_pTerrainVertexBuffer->Bind( 0 );
    m_pTerrainIndexBuffer->Bind( 0 );


	//Set up base and normal map
    a_terrainMap->Bind( 0 );
    a_normalMap->Bind( 1 );

    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, a_indexCount, sizeof(unsigned int), 0);	
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

    m_HoffmanScatterConstantBufferData.EyePos = FRMVECTOR4(1.0f, 1.0f, 1.0f, 1.0);
    m_HoffmanScatterConstantBufferData.SunDir = FRMVECTOR4(a_sunDirection, 1.0);
    m_HoffmanScatterConstantBufferData.BetaRPlusBetaM = FRMVECTOR4(vecBetaRM, 1.0);
    m_HoffmanScatterConstantBufferData.HGg = FRMVECTOR4(vecG, 1.0);
    m_HoffmanScatterConstantBufferData.BetaDashR = FRMVECTOR4(vecBetaDashR, 1.0);
    m_HoffmanScatterConstantBufferData.BetaDashM = FRMVECTOR4(vecBetaDashM, 1.0);
    m_HoffmanScatterConstantBufferData.OneOverBetaRPlusBetaM = FRMVECTOR4(vecOneOverBetaRM, 1.0);
    m_HoffmanScatterConstantBufferData.Multipliers = vecTermMulitpliers;
    m_HoffmanScatterConstantBufferData.SunColorAndIntensity = m_sunColorAndIntensity;

	m_HoffmanScatterConstantBufferData.matModelView = m_matCameraMeshModelView;
	m_HoffmanScatterConstantBufferData.matModelViewProj = m_matCameraMeshModelViewProj;
}
