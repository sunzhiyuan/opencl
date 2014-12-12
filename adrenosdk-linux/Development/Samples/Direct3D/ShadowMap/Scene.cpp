//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: Shadow Mapping
//
// Pros:
// - Can be applied to non-planar surfaces (press 4 to see this in action)
// - Supports self-shadowing
//
// Cons:
// - Do not produce soft-shadows, although PCF helps with a perfomance cost
//   (press 7 to turn on PCF)
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
#include "Scene.h"


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "ShadowMap" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    // Initialize variables
    m_pLogoTexture                       = NULL;

    m_pShadowMapConstantBuffer           = NULL;
    m_pPerPixelLightingConstantBuffer    = NULL;
    m_pDepthConstantBuffer               = NULL;
    m_pOverlayConstantBuffer             = NULL;

    m_nCurFloorIndex                     = 0;
    m_nCurMeshIndex                      = 0;

    m_nShadowMapFBOTextureWidth          = 0;
    m_nShadowMapFBOTextureHeight         = 0;
    m_nShadowMapFBOTextureExp            = 0;

    m_nCurMeshIndex                      = 0;
    m_bShowShadowMap                     = FALSE;
    m_bUsePCF                            = TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the ShadowMap shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION",   FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_ShadowMapShader.Compile( "ShadowMapVS.cso", "ShadowMapPS.cso",
                                                 pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer( sizeof(m_ShadowMapConstantBufferData), &m_ShadowMapConstantBufferData,
                                              &m_pShadowMapConstantBuffer ) )
            return FALSE;
    }

    // Create the ShadowMap PCF shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION",   FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_ShadowMapPCFShader.Compile( "ShadowMapVS.cso", "ShadowMap2x2PCFPS.cso",                                                      
                                                   pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

    }

    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "NORMAL",   FRM_VERTEX_NORMAL, DXGI_FORMAT_R32G32B32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_PerPixelLightingShader.Compile( "PerPixelLightingVS.cso", "PerPixelLightingPS.cso",
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer( sizeof(m_PerPixelLightingConstantBufferData), &m_PerPixelLightingConstantBufferData,
                                              &m_pPerPixelLightingConstantBuffer ) )
            return FALSE;        
    }

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

    // Create the Depth shader
    {
        
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == m_DepthShader.Compile( "DepthVS.cso", "DepthPS.cso",
                                            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if ( FALSE == FrmCreateConstantBuffer(sizeof(m_DepthConstantBufferData), &m_DepthConstantBufferData, &m_pDepthConstantBuffer) )
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
    m_nCurMeshIndex  = CUBE_MESH;
    m_bShowShadowMap = TRUE;
    m_vLightPosition = FRMVECTOR3( 0.0f, 8.0f, 0.01f );

    // Create the font
    if( FALSE == m_Font.Create( "Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceD3D resource;
    if( FALSE == resource.LoadFromFile( "Textures.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );
    m_UserInterface.AddIntVariable( &m_nShadowMapFBOTextureWidth , "Shadow Map Size", "%d" );
    m_UserInterface.AddBoolVariable( &m_bUsePCF , "PCF On", "True", "False" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Model" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Next Floor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Decrease Shadow Map Size" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Increase Shadow Map Size" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Toggle PCF" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, "Toggle Shadow Map Display" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_DPAD, "Rotate light" );

     // Load the meshes
    if( FALSE == m_Floor[ PLANE_FLOOR ].Load( "Plane.mesh" ) )
        return FALSE;
    if( FALSE == m_Floor[ PLANE_FLOOR ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Floor[ TERRAIN_FLOOR ].Load( "Terrain.mesh" ) )
        return FALSE;
    if( FALSE == m_Floor[ TERRAIN_FLOOR ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ CUBE_MESH ].Load( "Cube.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ CUBE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].Load( "Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].Load( "BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].Load( "Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].Load( "Rocket.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Initialize the camera and light perspective matrices
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );
    m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // Setup the FBOs
    m_nShadowMapFBOTextureExp    = 7;
    m_nShadowMapFBOTextureWidth  = 2 << m_nShadowMapFBOTextureExp;
    m_nShadowMapFBOTextureHeight = 2 << m_nShadowMapFBOTextureExp;
    
    if( FALSE == CreateShadowMapFBO() )
        return FALSE;

    if( FALSE == CreateLightingFBO() )
        return FALSE;

    // Setup the camera view matrix
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 4.0f, 10.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the floor's camera relative model view projection matrix
    FRMMATRIX4X4 matFloorScale            = FrmMatrixScale( 5.0f, 5.0f, 5.0f );
    FRMMATRIX4X4 matFloorTranslate        = FrmMatrixTranslate( 0.0f, -0.1f, 0.0f );
    m_matFloorModel                       = FrmMatrixMultiply( matFloorScale, matFloorTranslate );
    m_matCameraFloorModelView             = FrmMatrixMultiply( m_matFloorModel, m_matCameraView );
    m_matCameraFloorModelViewProj         = FrmMatrixMultiply( m_matCameraFloorModelView, m_matCameraPersp );
    m_matCameraFloorNormal                = FrmMatrixNormal( m_matCameraFloorModelView );

    // Setup the light look at and up vectors
    m_vLightLookAt = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vLightUp     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );

    // Create our scale and bias matrix that is used to convert the coordinates of vertices that are multiplied by a 
    // model view projection matrix from the range [-1, 1] to the range [0, 1], so that they can be used for texture lookups
    m_matScaleAndBias.M( 0, 0 ) = 0.5f; m_matScaleAndBias.M( 0, 1 ) = 0.0f; m_matScaleAndBias.M( 0, 2 ) = 0.0f; m_matScaleAndBias.M( 0, 3 ) = 0.0f;  
    m_matScaleAndBias.M( 1, 0 ) = 0.0f; m_matScaleAndBias.M( 1, 1 ) = -0.5f; m_matScaleAndBias.M( 1, 2 ) = 0.0f; m_matScaleAndBias.M( 1, 3 ) = 0.0f;  
    m_matScaleAndBias.M( 2, 0 ) = 0.0f; m_matScaleAndBias.M( 2, 1 ) = 0.0f; m_matScaleAndBias.M( 2, 2 ) = 0.5f; m_matScaleAndBias.M( 2, 3 ) = 0.0f;  
    m_matScaleAndBias.M( 3, 0 ) = 0.5f; m_matScaleAndBias.M( 3, 1 ) = 0.5f; m_matScaleAndBias.M( 3, 2 ) = 0.5f; m_matScaleAndBias.M( 3, 3 ) = 1.0f;  

    // State blocks
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.FrontCounterClockwise = TRUE; // Change the winding direction to match GL    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_DefaultRasterizerState);
    D3DDeviceContext()->RSSetState(m_DefaultRasterizerState.Get());

    rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);   
    rdesc.CullMode = D3D11_CULL_NONE;
    rdesc.DepthBias = 200;
    rdesc.SlopeScaledDepthBias = 2.0f; 
    D3DDevice()->CreateRasterizerState(&rdesc, &m_PolygonOffsetRasterizerState);

    D3D11_DEPTH_STENCIL_DESC dsdesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
    dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DepthLEqualStencilState);
    D3DDeviceContext()->OMSetDepthStencilState( m_DepthLEqualStencilState.Get(), 0 );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );
    m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matCameraPersp.M(0,0) *= fAspect;
        m_matCameraPersp.M(1,1) *= fAspect;
    }

    if ( FALSE == CreateShadowMapFBO() )
        return FALSE;
    if ( FALSE == CreateLightingFBO() )
        return FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if ( m_pShadowMapConstantBuffer ) m_pShadowMapConstantBuffer->Release();
    if ( m_pOverlayConstantBuffer ) m_pOverlayConstantBuffer->Release();
    if ( m_pPerPixelLightingConstantBuffer ) m_pPerPixelLightingConstantBuffer->Release();
    if ( m_pDepthConstantBuffer ) m_pDepthConstantBuffer->Release();

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    // Free mesh and FBO resources
    FreeMeshes();
    FreeShadowMapFBO();
    FreeLightingFBO();
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

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
    {
        m_UserInterface.AdvanceState();
    }

    // Move shadow left
    if( nButtons & INPUT_DPAD_LEFT )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow right
    if( nButtons & INPUT_DPAD_RIGHT )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow up
    if( nButtons & INPUT_DPAD_DOWN )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow down
    if( nButtons & INPUT_DPAD_UP )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nCurMeshIndex = ( m_nCurMeshIndex + 1 ) % NUM_MESHES;
    }

    // Change to next floor mesh
    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_nCurFloorIndex = ( m_nCurFloorIndex + 1 ) % NUM_FLOORS;
    }

    // Lower shadow map size
    if( nPressedButtons & INPUT_KEY_3 )
    {
        if( m_nShadowMapFBOTextureExp > 5 )
        {
            m_nShadowMapFBOTextureExp--;
            m_nShadowMapFBOTextureWidth  = 2 << m_nShadowMapFBOTextureExp;
            m_nShadowMapFBOTextureHeight = 2 << m_nShadowMapFBOTextureExp; 
            CreateShadowMapFBO();
        }
    }

    // Increase shadow map size
    if( nPressedButtons & INPUT_KEY_4 )
    {
        if( m_nShadowMapFBOTextureExp < 8 )
        {
            m_nShadowMapFBOTextureExp++;
            m_nShadowMapFBOTextureWidth  = 2 << m_nShadowMapFBOTextureExp;
            m_nShadowMapFBOTextureHeight = 2 << m_nShadowMapFBOTextureExp; 
            CreateShadowMapFBO();
        }
    }

    // Toggle PCF
    if( nPressedButtons & INPUT_KEY_5 )
    {
        m_bUsePCF = 1 - m_bUsePCF;
    }

    // Toggle display of shadow map
    if( nPressedButtons & INPUT_KEY_6 )
    {
        m_bShowShadowMap = !m_bShowShadowMap;
    }

    // Setup the mesh's camera relative model view, model view projection, and normal matrices
    FRMMATRIX4X4 matMeshRotate    = FrmMatrixRotate( fTime, FRMVECTOR3( 0.0f, 1.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshRotate2   = FrmMatrixRotate( fTime, FRMVECTOR3( 1.0f, 0.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshModel     = FrmMatrixTranslate( 0.0f, 2.0f, 0.0f );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate, matMeshModel );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate2, matMeshModel );
    m_matCameraMeshModelView      = FrmMatrixMultiply( matMeshModel, m_matCameraView );
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

    // Setup the floor and mesh's light relative model view projection matrices
    FRMMATRIX4X4 matLightView          = FrmMatrixLookAtRH( m_vLightPosition, m_vLightLookAt, m_vLightUp );
    FRMMATRIX4X4 matLightModelViewProj = FrmMatrixMultiply( matLightView, m_matLightPersp );
    m_matLightFloorModelViewProj       = FrmMatrixMultiply( m_matFloorModel, matLightModelViewProj );
    m_matLightMeshModelViewProj        = FrmMatrixMultiply( matMeshModel, matLightModelViewProj );

    // Setup our matrices that are used to convert vertices into shadow map lookup coordinates
    m_matFloorShadowMatrix = FrmMatrixMultiply( m_matLightFloorModelViewProj, m_matScaleAndBias );
    m_matMeshShadowMatrix = FrmMatrixMultiply( m_matLightMeshModelViewProj, m_matScaleAndBias );

    // Setup our matrices that are used to convert vertices into diffuse/specular map lookup coordinates
    m_matFloorScreenCoordMatrix = FrmMatrixMultiply( m_matCameraFloorModelViewProj, m_matScaleAndBias );
    m_matMeshScreenCoordMatrix = FrmMatrixMultiply( m_matCameraMeshModelViewProj, m_matScaleAndBias );
}


//--------------------------------------------------------------------------------------
// Name: CreateShadowMapFBO()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateShadowMapFBO()
{
    FreeShadowMapFBO();

    // Texture description - render to a 32-bit float color texture that stores depth
    D3D11_TEXTURE2D_DESC textureDesc;        
    ZeroMemory( &textureDesc, sizeof(textureDesc) );
    textureDesc.Width = m_nShadowMapFBOTextureWidth;
    textureDesc.Height = m_nShadowMapFBOTextureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Render target view
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        
    // Sampler description
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
        
    // Create a depth/stencil texture for the depth attachment
    if ( FAILED( D3DDevice()->CreateTexture2D( &textureDesc, NULL, &m_pShadowTexture ) ) )
        return FALSE;

    // Create the render target view
    if ( FAILED( D3DDevice()->CreateRenderTargetView( m_pShadowTexture.Get(), &renderTargetViewDesc, &m_pShadowTextureRenderTargetView ) ) )
        return FALSE;

    // Create the shader resource view
    if ( FAILED( D3DDevice()->CreateShaderResourceView( m_pShadowTexture.Get(), &shaderResourceViewDesc, &m_pShadowTextureShaderResourceView ) ) )
        return FALSE;
        
    // Create the sampler
    if ( FAILED(D3DDevice()->CreateSamplerState( &samplerDesc, &m_pShadowTextureSampler ) ) )
        return FALSE;
        
    // Create a depth/stencil texture for the depth attachment
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &m_pShadowDepthTexture ) ) )
        return FALSE;

    // Create depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = textureDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    
    if ( FAILED(D3DDevice()->CreateDepthStencilView( m_pShadowDepthTexture.Get(), &depthStencilViewDesc, &m_pShadowDepthStencilView ) ) )
        return FALSE;    

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: FreeMeshes()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeMeshes()
{
    for( INT32 nFloorMeshId = 0; nFloorMeshId < NUM_FLOORS; ++nFloorMeshId )
    {
        m_Floor[ nFloorMeshId ].Destroy();
    }

    for( INT32 nMeshId = 0; nMeshId < NUM_MESHES; ++nMeshId )
    {
        m_Meshes[ nMeshId ].Destroy(); 
    }
}


//--------------------------------------------------------------------------------------
// Name: FreeShadowMapFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeShadowMapFBO()
{
    m_pShadowTexture.Reset();
    m_pShadowTextureRenderTargetView.Reset();
    m_pShadowTextureShaderResourceView.Reset();
    m_pShadowTextureSampler.Reset();
    m_pShadowDepthTexture.Reset();
    m_pShadowDepthStencilView.Reset();
}


//--------------------------------------------------------------------------------------
// Name: GetShadowMapFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GetShadowMapFBO()
{
    ID3D11RenderTargetView* pRTViews[1] = { m_pShadowTextureRenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 1, pRTViews, m_pShadowDepthStencilView.Get() );
    
    // Set the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nShadowMapFBOTextureWidth);
    viewport.Height = static_cast<float>(m_nShadowMapFBOTextureHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);
}


//--------------------------------------------------------------------------------------
// Name: CreateLightingFBO()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateLightingFBO()
{
    FreeLightingFBO();

    // Texture description - render to a 32-bit RGBA color texture
    D3D11_TEXTURE2D_DESC textureDesc;        
    ZeroMemory( &textureDesc, sizeof(textureDesc) );
    textureDesc.Width = m_nWidth;
    textureDesc.Height = m_nHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Render target view
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        
    // Sampler description
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
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
        
    // Create a depth/stencil texture for the depth attachment
    if ( FAILED( D3DDevice()->CreateTexture2D( &textureDesc, NULL, &m_pDiffuseSpecularTexture ) ) )
        return FALSE;

    // Create the render target view
    if ( FAILED( D3DDevice()->CreateRenderTargetView( m_pDiffuseSpecularTexture.Get(), &renderTargetViewDesc, &m_pDiffuseSpecularTextureRenderTargetView ) ) )
        return FALSE;

    // Create the shader resource view
    if ( FAILED( D3DDevice()->CreateShaderResourceView( m_pDiffuseSpecularTexture.Get(), &shaderResourceViewDesc, &m_pDiffuseSpecularTextureShaderResourceView ) ) )
        return FALSE;
        
    // Create the sampler
    if ( FAILED(D3DDevice()->CreateSamplerState( &samplerDesc, &m_pDiffuseSpecularTextureSampler ) ) )
        return FALSE;
        
    // Create a depth/stencil texture for the depth attachment
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &m_pDiffuseSpecularDepthTexture ) ) )
        return FALSE;

    // Create depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = textureDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    
    if ( FAILED(D3DDevice()->CreateDepthStencilView( m_pDiffuseSpecularDepthTexture.Get(), &depthStencilViewDesc, &m_pDiffuseSpecularDepthStencilView ) ) )
        return FALSE;    



    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: FreeLightingFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeLightingFBO()
{
    m_pDiffuseSpecularTexture.Reset();
    m_pDiffuseSpecularTextureRenderTargetView.Reset();
    m_pDiffuseSpecularTextureShaderResourceView.Reset();
    m_pDiffuseSpecularTextureSampler.Reset();
    m_pDiffuseSpecularDepthTexture.Reset();
    m_pDiffuseSpecularDepthStencilView.Reset();
}


//--------------------------------------------------------------------------------------
// Name: GetLightingFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GetLightingFBO()
{
    ID3D11RenderTargetView* pRTViews[1] = { m_pDiffuseSpecularTextureRenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 1, pRTViews, m_pDiffuseSpecularDepthStencilView.Get() );
    
    // Set the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);

}


//--------------------------------------------------------------------------------------
// Name: RenderShadowMapToFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderShadowMapToFBO()
{
    GetShadowMapFBO();

    
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_pShadowTextureRenderTargetView.Get(), clearColor );
    D3DDeviceContext()->ClearDepthStencilView( m_pShadowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

    D3DDeviceContext()->RSSetState( m_PolygonOffsetRasterizerState.Get() );
           
    // Render the floor along with its depth
    m_DepthShader.Bind();
    m_DepthConstantBufferData.matModelViewProj = m_matLightFloorModelViewProj;
    m_pDepthConstantBuffer->Update( &m_DepthConstantBufferData );
    m_pDepthConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS );
    m_Floor[ m_nCurFloorIndex ].Render( &m_DepthShader );

    // Draw the Mesh
    m_DepthConstantBufferData.matModelViewProj = m_matLightMeshModelViewProj;
    m_pDepthConstantBuffer->Update( &m_DepthConstantBufferData );
    m_pDepthConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS );
    m_Meshes[ m_nCurMeshIndex ].Render( &m_DepthShader );    

    D3DDeviceContext()->RSSetState( m_DefaultRasterizerState.Get() );    
}


//--------------------------------------------------------------------------------------
// Name: RenderLightingToFBO()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderLightingToFBO()
{
    GetLightingFBO();

    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_pDiffuseSpecularTextureRenderTargetView.Get(), clearColor );
    D3DDeviceContext()->ClearDepthStencilView( m_pDiffuseSpecularDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );


    m_PerPixelLightingShader.Bind();
    
    // Render the floor
    m_PerPixelLightingConstantBufferData.vAmbient = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
    m_PerPixelLightingConstantBufferData.vDiffuse = FRMVECTOR4( 0.5f, 0.5f, 0.8f, 1.0f );
    m_PerPixelLightingConstantBufferData.vSpecular = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );
    m_PerPixelLightingConstantBufferData.matModelView = m_matCameraFloorModelView;
    m_PerPixelLightingConstantBufferData.matModelViewProj = m_matCameraFloorModelViewProj;
    m_PerPixelLightingConstantBufferData.matNormal = m_matCameraFloorNormal;
    m_PerPixelLightingConstantBufferData.vLightPos = FRMVECTOR4( m_vLightPosition, 1.0f );  
    m_pPerPixelLightingConstantBuffer->Update( &m_PerPixelLightingConstantBufferData );
    m_pPerPixelLightingConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );
    m_Floor[ m_nCurFloorIndex ].Render( &m_PerPixelLightingShader );

    // Render the mesh
    m_PerPixelLightingConstantBufferData.vAmbient = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
    m_PerPixelLightingConstantBufferData.vDiffuse = FRMVECTOR4( 0.6f, 0.1f, 0.1f, 1.0f );
    m_PerPixelLightingConstantBufferData.vSpecular = FRMVECTOR4( 1.0f, 0.6f, 0.65f, 10.0f );
    m_PerPixelLightingConstantBufferData.matModelView = m_matCameraMeshModelView;
    m_PerPixelLightingConstantBufferData.matModelViewProj = m_matCameraMeshModelViewProj;
    m_PerPixelLightingConstantBufferData.matNormal = m_matCameraMeshNormal;
    m_PerPixelLightingConstantBufferData.vLightPos = FRMVECTOR4( m_vLightPosition, 1.0f );
    m_pPerPixelLightingConstantBuffer->Update( &m_PerPixelLightingConstantBufferData );
    m_pPerPixelLightingConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );    
    m_Meshes[ m_nCurMeshIndex ].Render( &m_PerPixelLightingShader );
}


//--------------------------------------------------------------------------------------
// Name: RenderBlendedLightingAndShadowMap()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderBlendedLightingAndShadowMap()
{
    CFrmShaderProgramD3D* pShadowMapShader = NULL;

    // Reset to default window framebuffer
    ID3D11RenderTargetView* pRTViews[1] = { m_windowRenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 1, pRTViews, m_windowDepthStencilView.Get() );

    // Reset the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);

    // Bind shadow map to texture 0
    D3DDeviceContext()->PSSetShaderResources( 0, 1, m_pShadowTextureShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, m_pShadowTextureSampler.GetAddressOf() );

    // Bind diffuse/specular map to texture 1
    D3DDeviceContext()->PSSetShaderResources( 1, 1, m_pDiffuseSpecularTextureShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 1, 1, m_pDiffuseSpecularTextureSampler.GetAddressOf() );

    if( m_bUsePCF )
    {
        pShadowMapShader = &m_ShadowMapPCFShader;
    }
    else
    {
        pShadowMapShader = &m_ShadowMapShader;
    }

    pShadowMapShader->Bind();    
    m_ShadowMapConstantBufferData.fEpsilon = 0.0035f;
    m_ShadowMapConstantBufferData.vAmbient = FRMVECTOR4( 0.2f, 0.2f, 0.2f, 1.0f );
    m_ShadowMapConstantBufferData.matModelViewProj = m_matCameraFloorModelViewProj;
    m_ShadowMapConstantBufferData.matShadow = m_matFloorShadowMatrix;
    m_ShadowMapConstantBufferData.matScreenCoord = m_matFloorScreenCoordMatrix;
    m_pShadowMapConstantBuffer->Update( &m_ShadowMapConstantBufferData );
    m_pShadowMapConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );
    m_Floor[ m_nCurFloorIndex ].Render( pShadowMapShader );

        
    m_ShadowMapConstantBufferData.matModelViewProj = m_matCameraMeshModelViewProj;
    m_ShadowMapConstantBufferData.matShadow = m_matMeshShadowMatrix;
    m_ShadowMapConstantBufferData.matScreenCoord = m_matMeshScreenCoordMatrix;
    m_pShadowMapConstantBuffer->Update( &m_ShadowMapConstantBufferData );
    m_pShadowMapConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );
    m_Meshes[ m_nCurMeshIndex ].Render( pShadowMapShader );

    // Unbind
    ID3D11ShaderResourceView* noTex[2] = { NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 2, noTex );        
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    RenderShadowMapToFBO();
    RenderLightingToFBO();

    // Bind the backbuffer as a render target
    D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), m_windowDepthStencilView.Get() );

    // Clear the backbuffer to a solid color, and reset the depth stencil.
    const float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_windowRenderTargetView.Get(), clearColor );
    D3DDeviceContext()->ClearDepthStencilView( m_windowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

    if( m_bShowShadowMap )
    {
        D3DDeviceContext()->PSSetShaderResources( 0, 1, m_pShadowTextureShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, m_pShadowTextureSampler.GetAddressOf() );

        m_RenderTextureToScreen.RenderTextureToScreen( -5, 5, 64, 64, NULL,
                                      &m_OverlayShader, m_pOverlayConstantBuffer, &m_OverlayConstantBufferData,                                    
                                      offsetof(OverlayConstantBuffer, vScreenSize) );

        // Unbind
        ID3D11ShaderResourceView* noTex[1] = { NULL };
        D3DDeviceContext()->PSSetShaderResources( 0, 1, noTex );        
    }

    RenderBlendedLightingAndShadowMap();


    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

