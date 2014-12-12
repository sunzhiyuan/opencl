//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: Texture Synthesis
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmApplication.h>
#include <Direct3D/FrmFontD3D.h>
#include <Direct3D/FrmMesh.h>
#include <Direct3D/FrmPackedResourceD3D.h>
#include <Direct3D/FrmShader.h>
#include <Direct3D/FrmUserInterfaceD3D.h>
#include <Direct3D/FrmUtilsD3D.h>

#include <stdio.h>
#include "lod.h"
#include "surface.h"
#include "bicubic.h"
#include "quadtree.h"

#include "Scene.h"

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Texture Synthesis" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    // Initialize variables
    m_pLogoTexture                       = NULL;

    m_bShowRT                            = FALSE;
    
    m_pRenderTerrainConstantBuffer       = NULL;
    m_pResampleHeightConstantBuffer      = NULL;
    m_pComputeNormalsConstantBuffer      = NULL;
    m_pAddLayerConstantBuffer            = NULL;
    m_pOverlayConstantBuffer             = NULL;

    // Variables for Cached Procedural Textures
    
    m_flushTextures = 1;
    
    m_heightMap16 = 0;		// resampled height map to 513
    m_heightMap16Full = 0;	// full res height map

    m_pHeightMapTexture16 = NULL;	// texture storing full res height map
    m_pTexNoise = 0;			// texture for generating noise
    m_pTexDisplacement = 0;		// texture for generating height-displacements
    
    m_pBicubicWeight03 = 0;		// bi-cubic interpolation weights
    m_pBicubicWeight47 = 0;
    m_pBicubicWeight8B = 0;
    m_pBicubicWeightCF = 0;
       
    m_pVertexBuffer = 0;		// For caching texture tiles
    m_pIndexBuffer = 0;

    m_nTempQuads = 0;
	m_quadList = NULL;
	
}

//--------------------------------------------------------------------------------------
// Name: InitTerrainAndTextures()
// Desc: Initialize the terrain textures and geometry
//--------------------------------------------------------------------------------------
BOOL CSample::InitHeightMapAndTextures()
{
	//
	// load terrain
	//
	heightMapX = heightMapY = 513;
	heightMapXFull = heightMapYFull = 2049;

	FILE *f = NULL;
    fopen_s(&f, "Data\\rainier2049.raw", "rb" );

	if ( f == NULL ) return -1;

	m_heightMap16Full = new UINT16[ heightMapXFull * heightMapYFull ];
	fread( m_heightMap16Full, 1, heightMapXFull * heightMapYFull * sizeof( UINT16 ), f );
	fclose( f );


	// create low res heightmap for triangle mesh
	m_heightMap16 = new UINT16[ heightMapX * heightMapY ];

	int step = (heightMapXFull-1) / (heightMapX-1);
	for ( int y = 0; y < heightMapY; y++ )
		for ( int x = 0; x < heightMapX; x++ )
		{
			m_heightMap16[ x + y * heightMapX ] = 
				m_heightMap16Full[ min( heightMapXFull-1, x * step ) + min( heightMapYFull-1, y * step ) * heightMapXFull ];
		}
		
	//
	// create texturing
	// 
	createSurfaceNodes();
	
    
    	
	// create height map texture
	UINT16 *heightMapDouble = new UINT16[ (heightMapXFull-1)*(heightMapXFull-1)*2 ];
	for ( int j = 0; j < (heightMapXFull-1); j++ )
		for ( int i = 0; i < (heightMapXFull-1); i++ )
		{
			UINT16 h  = m_heightMap16Full[ i +     (heightMapXFull-2-j) * heightMapXFull ];
			UINT16 h2 = m_heightMap16Full[ i + 1 + (heightMapXFull-2-j) * heightMapXFull ];
			heightMapDouble[ (i + j * (heightMapXFull-1)) * 2 + 0 ] = h;
			heightMapDouble[ (i + j * (heightMapXFull-1)) * 2 + 1 ] = h2;
		}

    if ( FALSE == FrmCreateTexture( heightMapXFull-1, heightMapYFull-1, 1, DXGI_FORMAT_R16G16_UNORM,
                                    0, heightMapDouble, sizeof(UINT16) * (heightMapXFull-1)*(heightMapXFull-1)*2,
                                    &m_pHeightMapTexture16 ) )
    {
        return FALSE;
    }

    // Modify the sampler
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 0.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;
    D3DDevice()->CreateSamplerState( &samplerDesc, &(m_pHeightMapTexture16)->m_pSamplerState );
		
	
	delete[] heightMapDouble;
	
	// interpolation look-up textures (returns 0 if successful)
	if( generateBiCubicWeights( &m_pBicubicWeight03,
								&m_pBicubicWeight47,
								&m_pBicubicWeight8B,
								&m_pBicubicWeightCF ) )
		return FALSE;

	// create chunk LOD quadtree
	createQLOD( heightMapX, m_heightMap16, m_pHeightMapTexture16 );

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitIntermediateFBOs()
// Desc: Initialize the intermediate FBOs
//--------------------------------------------------------------------------------------
BOOL CSample::InitIntermediateFBOs()
{	
    // atlas textures (+ clear them)
	extern ATLAS atlas[ ATLAS_TEXTURES ];

	m_flushTextures = 1;

    // Setup all the render targets
    RENDER_TEXTURE* pRTs[MAX_LAYER_DEPTH + ATLAS_TEXTURES + 5];
    int r;
    for ( r = 0; r < MAX_LAYER_DEPTH; r++ )
    {
        pRTs[r] = &m_TexIntermediateHeightCoverageLayer[r];
    }
    for ( ; (r - MAX_LAYER_DEPTH) < ATLAS_TEXTURES; r++)
    {
        pRTs[r] = &atlas[r - MAX_LAYER_DEPTH].texture;
    }
    pRTs[r++] = &m_TexIntermediateColor;
    pRTs[r++] = &m_TexIntermediateColor2;
    pRTs[r++] = &m_TexIntermediateNormal;
    pRTs[r++] = &m_TexIntermediateHeightCoverage;
    pRTs[r++] = &m_TexDebug;
    
    for ( int i = 0; i < MAX_LAYER_DEPTH + ATLAS_TEXTURES + 5; i++ )
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        D3D11_SAMPLER_DESC samplerDesc;
    
        // Texture description
        ZeroMemory( &textureDesc, sizeof(textureDesc) );
        textureDesc.Width = RT_SIZE;
        textureDesc.Height = RT_SIZE;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;
        if (pRTs[i] == &m_TexIntermediateNormal || pRTs[i] == &m_TexIntermediateHeightCoverage)
            textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        // Render target description
        ZeroMemory( &renderTargetViewDesc, sizeof(renderTargetViewDesc) );
        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;
    
        // Shader resource view description
        ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
        shaderResourceViewDesc.Format = textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

        // Sampler description
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.MaxAnisotropy = 0;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.BorderColor[0] = 0.0f;
        samplerDesc.BorderColor[1] = 0.0f;
        samplerDesc.BorderColor[2] = 0.0f;
        samplerDesc.BorderColor[3] = 0.0f;
    
        // Create Texture 
        if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &pRTs[i]->m_pTexture ) ) )
        {
            return FALSE;
        }

        // Create Render Target View
        if ( FAILED(D3DDevice()->CreateRenderTargetView( pRTs[i]->m_pTexture.Get(), &renderTargetViewDesc, &pRTs[i]->m_pRenderTargetView ) ) )
        {
            return FALSE;
        }

        // Create Render Shader Resource View 0
        if ( FAILED(D3DDevice()->CreateShaderResourceView( pRTs[i]->m_pTexture.Get(), &shaderResourceViewDesc, &pRTs[i]->m_pShaderResourceView ) ) )
        {
            return FALSE;
        }    
        
        // Create Sampler State
        if ( FAILED(D3DDevice()->CreateSamplerState( &samplerDesc, &pRTs[i]->m_pSampler ) ) )
        {
            return FALSE;
        }
    }
    	
	// Clear all of the atlas textures
    for (int i = 0; i < ATLAS_TEXTURES; i++)
    {
        const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        D3DDeviceContext()->ClearRenderTargetView( atlas[i].texture.m_pRenderTargetView.Get(), clearColor );
    }

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the Texture shader
    {
        
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_OverlayShader.Compile( "OverlayVS.cso", "OverlayPS.cso",                                                     
                                               pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if ( FALSE == FrmCreateConstantBuffer(sizeof(m_OverlayConstantBufferData), &m_OverlayConstantBufferData, &m_pOverlayConstantBuffer) )
            return FALSE;        
    }
    
    // Create the RenderTerrain shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION",   FRM_VERTEX_POSITION,  DXGI_FORMAT_R32G32B32_FLOAT },
            { "TEXCOORD",   FRM_VERTEX_TEXCOORD0, DXGI_FORMAT_R32G32B32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_RenderTerrainShader.Compile( "RenderTerrainVS.cso", "RenderTerrainPS.cso",
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer(sizeof(m_RenderTerrainConstantBufferData), &m_RenderTerrainConstantBufferData,
                                             &m_pRenderTerrainConstantBuffer ) )
            return FALSE;


    }

    // Create the ResampleHeightBaseColor shader
    {    
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION",	FRM_VERTEX_POSITION,    DXGI_FORMAT_R32G32B32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD0,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD1,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD2,   DXGI_FORMAT_R32G32_FLOAT },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_ResampleHeightBaseColorShader.Compile( "ResampleHeightBaseColorVS.cso", "ResampleHeightBaseColorPS.cso",
                                                              pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer(sizeof(m_ResampleHeightConstantBufferData), &m_ResampleHeightConstantBufferData,
                                             &m_pResampleHeightConstantBuffer ) )
            return FALSE;
    }
    
    // Create the ResampleHeightMapColor shader
    {    
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION",	FRM_VERTEX_POSITION,    DXGI_FORMAT_R32G32B32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD0,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD1,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD2,   DXGI_FORMAT_R32G32_FLOAT },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_ResampleHeightMapColorShader.Compile( "ResampleHeightMapColorVS.cso", "ResampleHeightMapColorPS.cso",
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
    }
    
    // Create the ComputeNormals shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION",	FRM_VERTEX_POSITION,    DXGI_FORMAT_R32G32B32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD0,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD1,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD2,   DXGI_FORMAT_R32G32_FLOAT },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_ComputeNormalsShader.Compile( "ComputeNormalsVS.cso", "ComputeNormalsPS.cso",
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
        
        if( FALSE == FrmCreateConstantBuffer(sizeof(m_ComputeNormalsConstantBufferData), &m_ComputeNormalsConstantBufferData,
                                             &m_pComputeNormalsConstantBuffer ) )
            return FALSE;

    }
    
    // Create the AddLayer shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION",	FRM_VERTEX_POSITION,    DXGI_FORMAT_R32G32B32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD0,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD1,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD2,   DXGI_FORMAT_R32G32_FLOAT },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_AddLayerShader.Compile( "AddLayerVS.cso", "AddLayerPS.cso",
                                               pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer(sizeof(m_AddLayerConstantBufferData), &m_AddLayerConstantBufferData,
                                             &m_pAddLayerConstantBuffer ) )
            return FALSE;
    }
    
    // Create the ComputeLayerLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION",	FRM_VERTEX_POSITION,    DXGI_FORMAT_R32G32B32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD0,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD1,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD",	FRM_VERTEX_TEXCOORD2,   DXGI_FORMAT_R32G32_FLOAT },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_ComputeLayerLightingShader.Compile( "AddLayerVS.cso", "ComputeLayerLightingPS.cso",                                                      
                                                           pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
    }


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Initialize sample variables
    m_bShowRT = false;

    // Create the font
    if( FALSE == m_Font.Create( "Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceD3D resource;
    if( FALSE == resource.LoadFromFile( "Textures.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );
    
    // Create the terrain textures
    m_pTexNoise = resource.GetTexture( "Noise" );
    m_pTexDisplacement = resource.GetTexture( "Displacement" );
    m_pTexShadow = resource.GetTexture( "Shadow" );
    m_pTexOcclusion = resource.GetTexture( "Ambient" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Decrease Quality Level" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Increase Quality Level" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Decrease Exposure" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Increase Exposure" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Decrease Gamma Control" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, "Increase Gamma Control" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, "Toggle Terrain Type" );

	m_UserInterface.AddIntVariable( &g_qualitySelect, "Quality Level" );
	m_UserInterface.AddFloatVariable( &g_sliderExposure, "Exposure Control" );
	m_UserInterface.AddFloatVariable( &g_sliderGamma, "Gamma Control" );
	
	CURRENT_SURFACE_TYPE_STR = SURFACE_TYPE_STR[ g_surfaceSelect ];
	m_UserInterface.AddStringVariable( &CURRENT_SURFACE_TYPE_STR, "Surface Type" );

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
        
    // Initialize the intermediate textures/FBOs
    if( FALSE == InitIntermediateFBOs() )
		return FALSE;
        
    // Initialize the data for terrain rendering
    if( FALSE == InitHeightMapAndTextures() )
		return FALSE;

    // Initialize the camera and light perspective matrices
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 40000.0f );

    
    // camera setup
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 1000.0f, 7000.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    
    mover.SetPosition( vCameraPosition );
    
    m_camPos = FRMVECTOR4( vCameraPosition.x, vCameraPosition.y, vCameraPosition.z, 1.0f );
    m_camLookAt = FRMVECTOR4( vCameraLookAt.x, vCameraLookAt.y, vCameraLookAt.z, 1.0f );
    
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the model view proj
    m_matModelViewProj = FrmMatrixMultiply( m_matCameraView, m_matCameraPersp );

    // Render State
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.FrontCounterClockwise = TRUE; // Change the winding direction to match GL
    rdesc.ScissorEnable = TRUE;
    D3DDevice()->CreateRasterizerState(&rdesc, &m_RasterizerState);
    D3DDeviceContext()->RSSetState(m_RasterizerState.Get());
    
    rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);
    rdesc.ScissorEnable = TRUE;
    rdesc.CullMode = D3D11_CULL_NONE;
    D3DDevice()->CreateRasterizerState(&rdesc, &m_CullDisabledRasterizerState );

    // State blocks
    D3D11_DEPTH_STENCIL_DESC dsdesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DefaultDepthStencilState);
    dsdesc.DepthEnable = false;
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DisabledDepthStencilState);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 40000.0f );
    m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matCameraPersp.M(0,0) *= fAspect;
        m_matCameraPersp.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Delete shader programs
#ifdef TEMP
    if( m_hOverlayShader )          glDeleteProgram( m_hOverlayShader );
#endif

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();
    
    if( m_pTexNoise )			m_pTexNoise->Release();
    if( m_pTexDisplacement )	m_pTexDisplacement->Release();
    if( m_pTexShadow )			m_pTexShadow->Release();
    if( m_pTexOcclusion )		m_pTexOcclusion->Release();
    
    
    if ( m_pRenderTerrainConstantBuffer )  m_pRenderTerrainConstantBuffer->Release();
    if ( m_pResampleHeightConstantBuffer ) m_pResampleHeightConstantBuffer->Release();
    if ( m_pComputeNormalsConstantBuffer ) m_pComputeNormalsConstantBuffer->Release();
    if ( m_pAddLayerConstantBuffer )       m_pAddLayerConstantBuffer->Release();
    if ( m_pOverlayConstantBuffer )        m_pOverlayConstantBuffer->Release();

    // Delete Textures
    if( m_pHeightMapTexture16 ) m_pHeightMapTexture16->Release();
    if( m_pBicubicWeight03 )    m_pBicubicWeight03->Release();
    if( m_pBicubicWeight47 )    m_pBicubicWeight47->Release();
    if( m_pBicubicWeight8B )    m_pBicubicWeight8B->Release();
    if( m_pBicubicWeightCF )    m_pBicubicWeightCF->Release();
	
    
    // Delete Buffers
    if( m_pVertexBuffer ) m_pVertexBuffer->Release();
    if( m_pIndexBuffer )  m_pIndexBuffer->Release();

    // delete data
    if( m_heightMap16 != NULL )
		delete[] m_heightMap16;
	if( m_heightMap16Full != NULL )
		delete[] m_heightMap16Full;
    
    m_flushTextures = 1;

    destroyQLOD();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fTime        = m_Timer.GetFrameTime();
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );
    
    mover.Update( m_Input, 0.5f );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
    {
        m_UserInterface.AdvanceState();
    }

	if( nPressedButtons & INPUT_KEY_1 )
	{
		g_qualitySelect = max( g_qualitySelect - 1, 1 );
	}

	if( nPressedButtons & INPUT_KEY_2 )
	{
		g_qualitySelect = min( g_qualitySelect + 1, 10 );
	}

	if( nButtons & INPUT_KEY_3 )
	{
		g_sliderExposure -= 2.0f * fElapsedTime;
		g_sliderExposure = max( g_sliderExposure, 0.0f );
	}

	if( nButtons & INPUT_KEY_4 )
	{
		g_sliderExposure += 2.0f * fElapsedTime;
		g_sliderExposure = min( g_sliderExposure, 3.0f );
	}

	if( nButtons & INPUT_KEY_5 )
	{
		g_sliderGamma -= 2.0f * fElapsedTime;
		g_sliderGamma = max( g_sliderGamma, 0.0f );
	}

	if( nButtons & INPUT_KEY_6 )
	{
		g_sliderGamma += 2.0f * fElapsedTime;
		g_sliderGamma = min( g_sliderGamma, 3.0f );
	}

	if( nPressedButtons & INPUT_KEY_7 )
	{
		g_surfaceSelect = abs( 1 - g_surfaceSelect );
		CURRENT_SURFACE_TYPE_STR = SURFACE_TYPE_STR[ g_surfaceSelect ];
	}

    // Toggle display of debug render target
    if( nPressedButtons & INPUT_KEY_9 )
    {
        m_bShowRT = !m_bShowRT;
    }
}





//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
	static int lastSurfaceType = 0;
	static int lastQuality     = 5;

	extern void buildFrustum( FRMMATRIX4X4 &clip );
	
	// update the MVP
	FRMVECTOR3 vCameraPosition = mover.GetPosition();
	FRMVECTOR3 vCameraDirection = mover.GetDirection();
	FRMVECTOR3 vCameraLookAt = vCameraPosition + 100.0f*vCameraDirection;
	m_camPos = FRMVECTOR4( vCameraPosition.x, vCameraPosition.y, vCameraPosition.z, 1.0f );
    m_camLookAt = FRMVECTOR4( vCameraLookAt.x, vCameraLookAt.y, vCameraLookAt.z, 1.0f );
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, FRMVECTOR3(0.0f, 1.0f, 0.0f) );
    m_matModelViewProj = FrmMatrixMultiply( m_matCameraView, m_matCameraPersp );
 
	buildFrustum( m_matModelViewProj );

	if( lastSurfaceType != g_surfaceSelect || lastQuality != g_qualitySelect )
	{
		lastSurfaceType = g_surfaceSelect;
		lastQuality = g_qualitySelect;
		m_flushTextures = 1;
		createSurfaceNodes();
		rootSurfaceNode = surfaceTypes[ g_surfaceSelect ];
	}
	if( m_flushTextures )
		freeAllTexturesQLOD( 0, 0 );

	traverseQLOD( m_camPos, m_camLookAt, 0, 0 );
	m_flushTextures = 0;
	

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(RT_SIZE);
    viewport.Height = static_cast<float>(RT_SIZE);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);

	ComputeTextures();
	
	// Main pass

    // ensure the main framebuffer is bound	
    ID3D11RenderTargetView* pRTViews[2] = { m_windowRenderTargetView.Get(), NULL };
    D3DDeviceContext()->OMSetRenderTargets( 2, pRTViews, m_windowDepthStencilView.Get() );

    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    D3DDeviceContext()->RSSetViewports(1, &viewport);

    D3D11_RECT scissorRect;
    scissorRect.left = 0;
    scissorRect.right = m_nWidth;
    scissorRect.top = 0;
    scissorRect.bottom = m_nHeight;
    D3DDeviceContext()->RSSetScissorRects(1, &scissorRect);

    // Clear the backbuffer to a solid color, and reset the depth stencil.
    const float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_windowRenderTargetView.Get(), clearColor );
    D3DDeviceContext()->ClearDepthStencilView( m_windowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
	
	float t1 = 1.0f / g_sliderGamma, t2 = -g_sliderExposure;
	
    m_RenderTerrainShader.Bind();
	m_RenderTerrainConstantBufferData.gammaControl = t1;
    m_RenderTerrainConstantBufferData.exposureControl = t2;
	
	// set in renderQLOD, m_hRenderTerrainRtSizeVec4Loc;
    // set in renderQLOD, m_hRenderTerrainTexcoordBiasScaleVec4Loc;
    // set in renderQLOD, m_hRenderTerrainTerrainTextureLoc;
    
    m_RenderTerrainConstantBufferData.cameraPosition = m_camPos;
    m_RenderTerrainConstantBufferData.MatModelViewProj = m_matModelViewProj;
    
	renderQLOD( m_pRenderTerrainConstantBuffer, &m_RenderTerrainConstantBufferData );
	

	if( m_bShowRT )
    {
        extern ATLAS atlas[ ATLAS_TEXTURES ];

        D3DDeviceContext()->PSSetShaderResources( 0, 1,  atlas[10].texture.m_pShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, atlas[10].texture.m_pSampler.GetAddressOf() );        
        
        m_RenderTextureToScreen.RenderTextureToScreen( 15, 15, 512, 512, NULL,
                                      &m_OverlayShader, m_pOverlayConstantBuffer, &m_OverlayConstantBufferData,                                    
                                      offsetof(OverlayConstantBuffer, vScreenSize) );
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}



void CSample::AddQuad( TERRAINTILE *tile,  D3D11_RECT *pBoundingRect)
{
	m_quadList[ m_nTempQuads * 4 + 0 ].x = 0.0f; m_quadList[ m_nTempQuads * 4 + 0 ].y = 0.0f;
	m_quadList[ m_nTempQuads * 4 + 1 ].x = 0.0f; m_quadList[ m_nTempQuads * 4 + 1 ].y = 1.0f;
	m_quadList[ m_nTempQuads * 4 + 2 ].x = 1.0f; m_quadList[ m_nTempQuads * 4 + 2 ].y = 0.0f;
	m_quadList[ m_nTempQuads * 4 + 3 ].x = 1.0f; m_quadList[ m_nTempQuads * 4 + 3 ].y = 1.0f;

	for ( int i = 0; i < 4; i++ )
	{
		m_quadList[ m_nTempQuads * 4 + i ].z = (float)tile->tileSize;
		m_quadList[ m_nTempQuads * 4 + i ].coordHMBias1    = tile->coordHM.x;
		m_quadList[ m_nTempQuads * 4 + i ].coordHMBias2    = tile->coordHM.y;
		m_quadList[ m_nTempQuads * 4 + i ].coordHMScale1   = tile->coordHM.z;
		m_quadList[ m_nTempQuads * 4 + i ].coordHMScale2   = tile->coordHM.w;
		m_quadList[ m_nTempQuads * 4 + i ].coordTileBias1  = tile->coordTile.x;
		m_quadList[ m_nTempQuads * 4 + i ].coordTileBias2  = tile->coordTile.y;
		m_quadList[ m_nTempQuads * 4 + i ].coordTileScale1 = tile->coordTile.z;
		m_quadList[ m_nTempQuads * 4 + i ].coordTileScale2 = tile->coordTile.w;


        // Compute vertex position for bounding rect
        float vertPosX = m_quadList[ m_nTempQuads * 4 + i ].x * m_quadList[ m_nTempQuads * 4 + i ].coordTileScale1 + m_quadList[ m_nTempQuads * 4 + i].coordTileBias1;
        float vertPosY = m_quadList[ m_nTempQuads * 4 + i ].y * m_quadList[ m_nTempQuads * 4 + i ].coordTileScale2 + m_quadList[ m_nTempQuads * 4 + i].coordTileBias2;

        // Convert to viewport coordinates
        vertPosX = vertPosX * (float)RT_SIZE;
        vertPosY = (1.0f - vertPosY) * (float)RT_SIZE;

        // Keep track of bounding region of quads for scissor region
        if ( vertPosX < pBoundingRect->left )
            pBoundingRect->left = (LONG)vertPosX;
        if ( vertPosX > pBoundingRect->right )
            pBoundingRect->right = (LONG)vertPosX;
        if ( vertPosY < pBoundingRect->top )
            pBoundingRect->top = (LONG)vertPosY;
        if ( vertPosY > pBoundingRect->bottom )
            pBoundingRect->bottom = (LONG)vertPosY;


		float realSizeM = tile->worldSize / (float)(heightMapX-1) / (float)tile->tileSize * 15300.0f * 2.6777777f;
		float heightScale = 65536.0f * 0.1f;
		m_quadList[ m_nTempQuads * 4 + i ].scale1 = heightScale / realSizeM;
		m_quadList[ m_nTempQuads * 4 + i ].scale2 = realSizeM / heightScale;
	}

	m_nTempQuads ++;
}


VOID CSample::AddTextureLayer( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface, 
							RENDER_TEXTURE *srcColor, 
							RENDER_TEXTURE *srcHeight, 
							RENDER_TEXTURE *srcCoverage,
							RENDER_TEXTURE *dstColor,
							RENDER_TEXTURE *dstHeightCoverage )
{
	// setup  states for texture synthesis
    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 1 );

    
	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 0: read input heightmap (maybe modify height values here!)
	//         IN : height, coverage, current surface color
	//              noise textures
	//         OUT: intermediate color and height+coverage
	//
    m_pVertexBuffer->Bind( 0 );
    m_pIndexBuffer->Bind( 0 );
	
	
	// Bind the two render targets for Color and Height/Coverage
    ID3D11RenderTargetView* pRTViews[2] = { m_TexIntermediateColor.m_pRenderTargetView.Get(), 
                                            m_TexIntermediateHeightCoverage.m_pRenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 2,  pRTViews, NULL );


	// clear the render targets
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( pRTViews[0], clearColor );
    D3DDeviceContext()->ClearRenderTargetView( pRTViews[1], clearColor );
    

    m_ResampleHeightMapColorShader.Bind( );
    m_ResampleHeightConstantBufferData.heightMapSize = (float)(heightMapXFull - 1);
    m_pResampleHeightConstantBuffer->Update( &m_ResampleHeightConstantBufferData );
    m_pResampleHeightConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

    D3DDeviceContext()->PSSetShaderResources( 0, 1, srcColor->m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, srcColor->m_pSampler.GetAddressOf() );

    D3DDeviceContext()->PSSetShaderResources( 1, 1, srcHeight->m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 1, 1, srcHeight->m_pSampler.GetAddressOf() );
    
    D3DDeviceContext()->PSSetShaderResources( 2, 1, srcCoverage->m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 2, 1, srcCoverage->m_pSampler.GetAddressOf() );
    

    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_nTempQuads*2*3, sizeof(UINT16), 0 );

    ID3D11ShaderResourceView* unbind[4] = { NULL, NULL, NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 4, unbind );

    
	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 1: compute normals for heightmap
	//         IN : intermediate height map (if height values have been modified in step 0), otherwise input height
	//         OUT: intermediate normal
	//
	
	// Framebuffer is already bound, just need to change the color attachments
	//	( texIntermediateNormal and clear attachment 1 )
    pRTViews[0] = m_TexIntermediateNormal.m_pRenderTargetView.Get();
    pRTViews[1] = NULL;
    D3DDeviceContext()->OMSetRenderTargets( 2,  pRTViews, NULL );

    D3DDeviceContext()->DiscardView( m_TexIntermediateNormal.m_pRenderTargetView.Get() );

    m_ComputeNormalsShader.Bind( );
		
	FRMVECTOR4 rtSize = FRMVECTOR4( (float)RT_SIZE, 1.0f/(float)RT_SIZE, 0.0f, (float)quadsPerTile );
    m_ComputeNormalsConstantBufferData.rtSize = rtSize;
    m_pComputeNormalsConstantBuffer->Update( &m_ComputeNormalsConstantBufferData );
    m_pComputeNormalsConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

    D3DDeviceContext()->PSSetShaderResources( 0, 1, m_TexIntermediateHeightCoverage.m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, m_TexIntermediateHeightCoverage.m_pSampler.GetAddressOf() );

    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_nTempQuads*2*3, sizeof(UINT16), 0 );

    D3DDeviceContext()->PSSetShaderResources( 0, 4, unbind );


    D3DDeviceContext()->GenerateMips(  m_TexIntermediateHeightCoverage.m_pShaderResourceView.Get() );


	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 2: add texture layer
	//         IN : intermediate height + coverage, current surface color, noise, ...
	//         OUT: new surface color
	//              new height+coverage
	//
    pRTViews[0] = dstColor->m_pRenderTargetView.Get();
    pRTViews[1] = dstHeightCoverage->m_pRenderTargetView.Get();
    D3DDeviceContext()->OMSetRenderTargets( 2,  pRTViews, NULL );

    D3DDeviceContext()->DiscardView( dstColor->m_pRenderTargetView.Get() );
    D3DDeviceContext()->DiscardView( dstHeightCoverage->m_pRenderTargetView.Get() );

    m_AddLayerShader.Bind( );

    m_AddLayerConstantBufferData.noisePanning = surface.noisePan;
    m_AddLayerConstantBufferData.constraintAltitude = surface.tpAScaleBias;
    m_AddLayerConstantBufferData.constraintSlope = surface.tpSScaleBias;
    m_AddLayerConstantBufferData.materialColor = surface.spColor;
        
	FRMVECTOR4 attribVec( surface.spCoverage, surface.spFractalNoise, 
		surface.spBumpiness*0.25f, 
		surface.spMimicTerrain );

    m_AddLayerConstantBufferData.constraintAttrib = attribVec;
    m_pAddLayerConstantBuffer->Update( &m_AddLayerConstantBufferData );
    m_pAddLayerConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );
	
    D3DDeviceContext()->PSSetShaderResources( 0, 1, m_TexIntermediateNormal.m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, m_TexIntermediateNormal.m_pSampler.GetAddressOf() );

    m_pTexNoise->Bind( 1 );
    
    D3DDeviceContext()->PSSetShaderResources( 2, 1, m_TexIntermediateColor.m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 2, 1, m_TexIntermediateColor.m_pSampler.GetAddressOf() );


    D3DDeviceContext()->PSSetShaderResources( 3, 1, m_TexIntermediateHeightCoverage.m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 3, 1, m_TexIntermediateHeightCoverage.m_pSampler.GetAddressOf() );

    
    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_nTempQuads*2*3, sizeof(UINT16), 0 );

	// Unbind textures
    D3DDeviceContext()->PSSetShaderResources( 0, 4, unbind );
		
	// reset states
    D3DDeviceContext()->RSSetState( m_RasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 1 );
}


VOID CSample::AddTextureLayerRoot( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface, 
								RENDER_TEXTURE *dstColor, 
								RENDER_TEXTURE *dstHeightCoverage )
{
	// setup states for texture synthesis
    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 1 );

	const int VBO_SIZE = 1024*128;

	// initialize vertex buffer
	if( m_pVertexBuffer == 0 )
	{
        FrmCreateVertexBuffer( VBO_SIZE / sizeof(TILEVERTEX), sizeof(TILEVERTEX), NULL, &m_pVertexBuffer );
	}

	// Initialize and fill up index buffer
	if( m_pIndexBuffer == 0 )
	{
        UINT16* idx = new UINT16[1024*2*3];
        UINT16* pIdxStart = idx;
        for ( int i = 0; i < 1024; i++ )
		{
			*(idx++) = i * 4 + 0;
			*(idx++) = i * 4 + 1;
			*(idx++) = i * 4 + 3;
			*(idx++) = i * 4 + 0;
			*(idx++) = i * 4 + 3;
			*(idx++) = i * 4 + 2;
		}

        FrmCreateIndexBuffer(1024*2*3, sizeof(UINT16), pIdxStart, &m_pIndexBuffer );     

        delete [] pIdxStart;
	}

	// Fill up vertex buffer
	m_quadList = (TILEVERTEX *) m_pVertexBuffer->Map( D3D11_MAP_WRITE_DISCARD );
	

    // Initialize the scissor rect to largest possible values so
    // that it is modified by the AddQuad() method
    D3D11_RECT scissorRect;
    scissorRect.left = RT_SIZE;
    scissorRect.right = 0;
    scissorRect.top = RT_SIZE;
    scissorRect.bottom = 0;

	m_nTempQuads = 0;
	for ( int i = 0; i < (int)atlas->count; i++ )
		AddQuad( atlas->assign[ i ], &scissorRect );

    // Set the scissor based on the computed bounds.  This is a performance optimization
    // so that the driver knows what tile(s) are going to be modified
    D3DDeviceContext()->RSSetScissorRects(1, &scissorRect);

    m_pVertexBuffer->Unmap();
	
    m_pVertexBuffer->Bind( 0 );
    m_pIndexBuffer->Bind( 0 );
	   

	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 0: read original heightmap and resample to new resolution
	//         IN : tile->pHeightMapTexture (16-bit integer: x=height)
	//              lookup + displacement textures
	//         OUT: intermediate color and height+coverage
	//
    ID3D11RenderTargetView* pRTViews[2] = { dstColor->m_pRenderTargetView.Get(), 
                                            dstHeightCoverage->m_pRenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 2,  pRTViews, NULL );

    D3DDeviceContext()->DiscardView( dstColor->m_pRenderTargetView.Get() );
    D3DDeviceContext()->DiscardView( dstHeightCoverage->m_pRenderTargetView.Get() );
	
    m_ResampleHeightBaseColorShader.Bind();
    m_ResampleHeightConstantBufferData.heightMapSize = (float)(heightMapXFull - 1) ;
    m_ResampleHeightConstantBufferData.terrainBaseColor = terrainBaseColor;
	
	// surface attributes
	FRMVECTOR4 attribVec( surface.spCoverage, surface.spFractalNoise, 
		surface.spBumpiness*0.25f, 
		surface.spMimicTerrain );
    m_ResampleHeightConstantBufferData.constraintAttrib = attribVec;

    m_pResampleHeightConstantBuffer->Update( &m_ResampleHeightConstantBufferData );
    m_pResampleHeightConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

    atlas->assign[0]->pHeightMapTexture->Bind( 0 );
    m_pTexDisplacement->Bind( 1 );
    m_pBicubicWeight03->Bind( 2 );
    m_pBicubicWeight47->Bind( 3 );
    m_pBicubicWeight8B->Bind( 4 );
    m_pBicubicWeightCF->Bind( 5 );
    
    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,  m_nTempQuads*2*3, sizeof(UINT16), 0 );
    
    // Unbind textures
    ID3D11ShaderResourceView* unbind[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 6, unbind );
		
    // Reset States
	D3DDeviceContext()->RSSetState( m_RasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 1 );
}


VOID CSample::ComputeLightingLayer( ATLAS *atlas, RENDER_TEXTURE *srcColor, RENDER_TEXTURE *srcHeight, RENDER_TEXTURE *dstColor )
{
    // setup states for texture synthesis
    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 1 );

    m_pVertexBuffer->Bind( 0 );
    m_pIndexBuffer->Bind( 0);
	
	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 0: compute normals for heightmap
	//
    ID3D11RenderTargetView* pRTViews[2] = { m_TexIntermediateNormal.m_pRenderTargetView.Get(), 
                                            NULL };
    D3DDeviceContext()->OMSetRenderTargets( 2,  pRTViews, NULL );

    D3DDeviceContext()->DiscardView( m_TexIntermediateNormal.m_pRenderTargetView.Get() );

    m_ComputeNormalsShader.Bind( );
		
	FRMVECTOR4 rtSize = FRMVECTOR4( (float)RT_SIZE, 1.0f/(float)RT_SIZE, 0.0f, (float)quadsPerTile );
    m_ComputeNormalsConstantBufferData.rtSize = rtSize;
	m_pComputeNormalsConstantBuffer->Update( &m_ComputeNormalsConstantBufferData );
    m_pComputeNormalsConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

    D3DDeviceContext()->PSSetShaderResources( 0, 1, srcHeight->m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, srcHeight->m_pSampler.GetAddressOf() );

    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_nTempQuads*2*3, sizeof(UINT16), 0 );

    ID3D11ShaderResourceView* unbind[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 1, unbind );


    D3DDeviceContext()->GenerateMips(  m_TexIntermediateNormal.m_pShaderResourceView.Get() );

	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 1: compute lighting
    pRTViews[0] = dstColor->m_pRenderTargetView.Get();
    pRTViews[1] = NULL;
    D3DDeviceContext()->OMSetRenderTargets( 2,  pRTViews, NULL );
	
    m_ComputeLayerLightingShader.Bind( );
	
    m_pAddLayerConstantBuffer->Update( &m_AddLayerConstantBufferData );
    m_pAddLayerConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

	// noisePanning is unused, doesn't need to be set
	// glUniform4fv( m_hComputeLayerLightingNoisePanningVec4Loc, 1, (FLOAT *)&surface.noisePan );

    D3DDeviceContext()->PSSetShaderResources( 0, 1, m_TexIntermediateNormal.m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, m_TexIntermediateNormal.m_pSampler.GetAddressOf() );

    m_pTexShadow->Bind( 1 );
    m_pTexOcclusion->Bind( 2 );

    D3DDeviceContext()->PSSetShaderResources( 3, 1, srcColor->m_pShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 3, 1, srcColor->m_pSampler.GetAddressOf() );

   
    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_nTempQuads*2*3, sizeof(UINT16), 0 );

    D3DDeviceContext()->PSSetShaderResources( 0, 4, unbind );

    // Reset States
	D3DDeviceContext()->RSSetState( m_RasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 1 );

}

#define NO_MORE_ON_THIS_LEVEL	1
static RENDER_TEXTURE* lastHeightCoverage = NULL;

VOID CSample::ComputeTexturesRecursive( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface,
									INT32 depth, RENDER_TEXTURE *currentCoverage, INT32 additionalFlags)
{
	// if this is our first layer, we do not need to evaluate constraints, but resample the height map
	if ( depth == 0 )
	{
		AddTextureLayerRoot( atlas, surface, &m_TexIntermediateColor2, &m_TexIntermediateHeightCoverageLayer[ depth ] );
		lastHeightCoverage = &m_TexIntermediateHeightCoverageLayer[ depth ];
	} else
	// ok, just add another layer
	{
		AddTextureLayer( atlas, surface,
			&m_TexIntermediateColor2,								// src color
			lastHeightCoverage,										// src height	(but is a height and coverage texture...)
			&m_TexIntermediateHeightCoverageLayer[ depth - 1 ],	// src coverage (but is a height and coverage texture...)
			&m_TexIntermediateColor2,								// dst color
			&m_TexIntermediateHeightCoverageLayer[ depth ] );		// dst coverage (height and coverage)
		lastHeightCoverage = &m_TexIntermediateHeightCoverageLayer[ depth ];
	}

	// and don't forget child surface nodes
	for ( int i = 0; i < surface.nChilds; i++ )
	{
		int addFlags = 0;

		if ( i == surface.nChilds - 1 )
			if ( additionalFlags & NO_MORE_ON_THIS_LEVEL || depth == 0 )
				addFlags |= NO_MORE_ON_THIS_LEVEL;

		ComputeTexturesRecursive( atlas, *surface.child[ i ], depth + 1, &m_TexIntermediateHeightCoverageLayer[ depth - 1 ], addFlags );
	}
}

VOID CSample::ComputeTextures()
{
	extern ATLAS atlas[ ATLAS_TEXTURES ];

	terrainBaseColor = rootSurfaceNode->spColor;

	for ( int i = 0; i < ATLAS_TEXTURES; i++ )
	    if ( atlas[ i ].count > 0 )
	    {
		    ComputeTexturesRecursive( &atlas[ i ], *rootSurfaceNode );
		    ComputeLightingLayer( &atlas[ i ], 
				    &m_TexIntermediateColor2, lastHeightCoverage, &atlas[ i ].texture );
	    }
}
