//--------------------------------------------------------------------------------------
// Author:                 QUALCOMM, Adreno SDK
// Desc: Port of ShaderX7 Cascaded Shadow Maps
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmApplication.h>
#include <Direct3D/FrmFontD3D.h>
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
    return new CSample( "Cascaded Shadow Maps" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_nCurShaderIndex					 = PCF;

    m_pOverlayConstantBuffer             = NULL;
    m_pDepthConstantBuffer               = NULL;
    m_pGroundVertexBuffer                = NULL;
    m_pGroundIndexBuffer                 = NULL;
    
    m_pLogoTexture                       = NULL;
    m_nCurMeshIndex                      = 0;

    m_Preview						     = false;

    // Set shadow map Bias
    m_ShadowMan.m_CSMConstantBufferData.fBias = -0.0163f;
}

VOID CSample::DrawGround()
{
    static float dim = 100.f;
    
    // Allocate on first use
    if ( m_pGroundVertexBuffer == NULL )
    {
        FLOAT32 Quad[] =
        {
            dim, -2.0f, -dim, 0.f,1.f,0.f,
            -dim, -2.0f, -dim, 0.f,1.f,0.f,
            -dim, -2.0f, dim, 0.f,1.f,0.f,

            -dim, -2.0f, dim, 0.f,1.f,0.f,
            dim, -2.0f, dim, 0.f,1.f,0.f,
            dim, -2.0f, -dim, 0.f,1.f,0.f,
        
        };
        UINT32 indices[6] = { 0, 1, 2, 3, 4, 5 };

        FrmCreateVertexBuffer( 6, sizeof(FLOAT32) * 6, &Quad[0], &m_pGroundVertexBuffer );
        FrmCreateIndexBuffer( 6, sizeof(UINT32), &indices[0], &m_pGroundIndexBuffer );

    }

    m_pGroundVertexBuffer->Bind( 0 );
    m_pGroundIndexBuffer->Bind( 0 );

    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6, sizeof(UINT32), 0 );
}


//--------------------------------------------------------------------------------------
// Name: CreateShadowMapFBO()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateShadowMapFBO()
{
    m_pFBO = new FrameBufferObject;
    m_pFBO->m_nHeight = sizeShadow*2;
    m_pFBO->m_nWidth = sizeShadow*2;
    
    // Create shadow texture
    {   
        
        // Texture description - render to a 32-bit float color texture
        D3D11_TEXTURE2D_DESC textureDesc;        
        ZeroMemory( &textureDesc, sizeof(textureDesc) );
        textureDesc.Width = sizeShadow*2;
        textureDesc.Height = sizeShadow*2;
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
        if ( FAILED( D3DDevice()->CreateTexture2D( &textureDesc, NULL, &m_pFBO->m_pTexture ) ) )
            return FALSE;

        // Create the render target view
        if ( FAILED( D3DDevice()->CreateRenderTargetView( m_pFBO->m_pTexture.Get(),
                                                          &renderTargetViewDesc, 
                                                          &m_pFBO->m_pTextureRenderTargetView ) ) )
            return FALSE;

        // Create the shader resource view
        if ( FAILED( D3DDevice()->CreateShaderResourceView( m_pFBO->m_pTexture.Get(),
                                                            &shaderResourceViewDesc,
                                                            &m_pFBO->m_pTextureShaderResourceView ) ) )
            return FALSE;
        
        // Create the sampler
        if ( FAILED(D3DDevice()->CreateSamplerState( &samplerDesc, &m_pFBO->m_pTextureSampler) ) )
            return FALSE;
        
        // Create a depth/stencil texture for the depth attachment
        textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &m_pFBO->m_pDepthTexture ) ) )
            return FALSE;

        // Create depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
        depthStencilViewDesc.Format = textureDesc.Format;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;
    
        if ( FAILED(D3DDevice()->CreateDepthStencilView( m_pFBO->m_pDepthTexture.Get(), 
                                                         &depthStencilViewDesc, 
                                                         &m_pFBO->m_pDepthStencilView ) ) )
            return FALSE;    

    }

    return TRUE;
}




//--------------------------------------------------------------------------------------
// Name: BeginFBO()
// Desc: Bind objects for rendering to the frame buffer object
//--------------------------------------------------------------------------------------
VOID CSample::BeginFBO( FrameBufferObject* pFBO )
{

    ID3D11RenderTargetView* pRTViews[1] = { pFBO->m_pTextureRenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 1, pRTViews, pFBO->m_pDepthStencilView.Get() );
    
    // Set the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(pFBO->m_nWidth);
    viewport.Height = static_cast<float>(pFBO->m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);
}

//--------------------------------------------------------------------------------------
// Name: EndFBO()
// Desc: Restore rendering back to the primary frame buffer
//--------------------------------------------------------------------------------------
VOID CSample::EndFBO( FrameBufferObject* pFBO )
{
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
}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------

BOOL CSample::InitShaders()
{    
    if( FALSE == m_ShadowMan.initShaders())
    {
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

    // preview shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_PreviewShader.Compile( "PreviewShaderVS.cso", "PreviewShaderPS.cso", 
                                              pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;		
    }
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

    if ( FALSE == CreateShadowMapFBO() )
        return FALSE;

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

    m_vSunDirection = FRMVECTOR3( 0.0f, 4.0f, 0.0f );

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

    //hack in a title for first model
    m_UserInterface.SetHeading("PCF");

    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Model" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Rotate Sun CW about Z axis" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Rotate Sun CCW about Z axis"  );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Rotate Sun CW about X axis" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Rotate Sun CCW about X axis" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, "Change Shadow Filter" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, "Toggle Depth Buffer" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, "Change Cascade Split Method" );

     // Load the meshes
    if( FALSE == m_Meshes[ CUBE_MESH ].Load( "Cube.mesh" ) )//
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
    ////////////

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Initialize the camera and light perspective matrices
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 250.0f );


    // Setup the camera view matrix
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 0.0f, 10.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the light look at and up vectors
    m_vLightLookAt = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vLightUp     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );

    // Initial state
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.CullMode = D3D11_CULL_NONE; // Do not CULL in this example
    rdesc.FrontCounterClockwise = TRUE; // Change the winding direction to match GL    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_RasterizerState);
    D3DDeviceContext()->RSSetState(m_RasterizerState.Get());

    D3D11_DEPTH_STENCIL_DESC dsdesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
    dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DepthLEqualStencilState);
    dsdesc.DepthEnable = false;
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
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 400.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matCameraPersp.M(0,0) *= fAspect;
        m_matCameraPersp.M(1,1) *= fAspect;
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Delete shader programs
    if ( m_pOverlayConstantBuffer ) m_pOverlayConstantBuffer->Release();
    if ( m_pDepthConstantBuffer ) m_pDepthConstantBuffer->Release();

    if ( m_pGroundVertexBuffer ) m_pGroundVertexBuffer->Release();
    if ( m_pGroundIndexBuffer ) m_pGroundIndexBuffer->Release();

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    // Free mesh
    FreeMeshes();

    delete m_pFBO;
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

    mover.Update( m_Input, fTime);

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
    {
        m_UserInterface.AdvanceState();
    }

    // Move shadow left
    if( nButtons & INPUT_KEY_2 )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vSunDirection, matRotate );
        m_vSunDirection = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow right
    if( nButtons & INPUT_KEY_3 )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vSunDirection, matRotate );
        m_vSunDirection = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow up
    if( nButtons & INPUT_KEY_4 )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vSunDirection, matRotate );
        m_vSunDirection = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow down
    if( nButtons & INPUT_KEY_5 )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vSunDirection, matRotate );
        m_vSunDirection = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }


    m_vSunDirection = FrmVector3Normalize( m_vSunDirection );

    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nCurMeshIndex = ( m_nCurMeshIndex + 1 ) % NUM_MESHES;
    }

    

    if( nPressedButtons & INPUT_KEY_6 )
    {
        m_nCurShaderIndex = ( m_nCurShaderIndex + 1 ) % NUM_SHADERS;
        m_ShadowSettings.setTechnique( Settings::TECHNIQUE_ATLAS );

        switch(m_nCurShaderIndex)
        {
        case HARD:
            m_UserInterface.SetHeading("Hard Edges");
        break;
        case PCF_SHOW_SPLITS:
            m_UserInterface.SetHeading("Show Cascade Splits with PCF");
        break;
        case PCF:
            m_UserInterface.SetHeading("PCF");
        break;
        case ESM_OPTIMIZED:
            m_UserInterface.SetHeading("Exponential Shadow Maps using Optimized CSM");
        break;

        }
    }

    if( nPressedButtons & INPUT_KEY_7 )
    {
        m_Preview = !m_Preview;
    }

    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_ShadowSettings.setFlickering( (m_ShadowSettings.getFlickering() + 1)% Settings::NUM_FLICKERING_MODES);
    }



    m_matCameraView = mover.GetInverseWorldMatrix();

    m_matCameraMeshModelView      = m_matCameraView;
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );



    // Update shadow manager
    m_ShadowMan.update( &m_ShadowSettings, m_matCameraPersp, m_matCameraView, (float *)& m_vSunDirection );


    //update all the individual model matrices..
    for(int i = 0; i < MODEL_DIMENSION * MODEL_DIMENSION; ++i)
    {
        static float grid_spacing = 5.0f;

        float transX = grid_spacing * ((i % MODEL_DIMENSION) - (MODEL_DIMENSION/2.0f));
        float transZ = grid_spacing * ((i / MODEL_DIMENSION) - (MODEL_DIMENSION/2.0f));


        FRMMATRIX4X4 matMeshRotate    = FrmMatrixRotate( fTime *(-1.f*(i%2)), FRMVECTOR3( 0.0f, 1.0f, 0.0f ) );
        FRMMATRIX4X4 matMeshRotate2   = FrmMatrixRotate( fTime *(-1.f*(i%3)), FRMVECTOR3( 1.0f, 0.0f, 0.0f ) );
        FRMMATRIX4X4 matMeshModel     = FrmMatrixTranslate( transX, 0.0f, transZ );
        matMeshModel                  = FrmMatrixMultiply( matMeshRotate, matMeshModel );
        m_ModelMtxs[i]                = FrmMatrixMultiply( matMeshRotate2, matMeshModel );
    }

}



//--------------------------------------------------------------------------------------
// Name: FreeMeshes()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeMeshes()
{
    for( INT32 nMeshId = 0; nMeshId < NUM_MESHES; ++nMeshId )
    {
        m_Meshes[ nMeshId ].Destroy(); 
    }
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    FrameBufferObject* curRT = m_pFBO;
    
    m_DepthShader.Bind();	
    {

        BeginFBO( curRT );

        const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        D3DDeviceContext()->ClearRenderTargetView( curRT->m_pTextureRenderTargetView.Get(), clearColor );
        D3DDeviceContext()->ClearDepthStencilView( curRT->m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

        FRMMATRIX4X4 matCameraMeshModelViewProj;

        for(int i=0; i<4; ++i)
        {
            // Set viewport for the atlas
            D3D11_VIEWPORT viewport;
            viewport.TopLeftX = static_cast<float>(sizeShadow * (i % 2));
            viewport.TopLeftY = static_cast<float>(sizeShadow * (i / 2));
            viewport.Width = static_cast<float>(sizeShadow);
            viewport.Height = static_cast<float>(sizeShadow);
            viewport.MinDepth = D3D11_MIN_DEPTH;
            viewport.MaxDepth = D3D11_MAX_DEPTH;
            D3DDeviceContext()->RSSetViewports(1, &viewport);
            
            for(int k = 0; k < MODEL_DIMENSION * MODEL_DIMENSION; ++k)
            {

                matCameraMeshModelViewProj = FrmMatrixMultiply( FrmMatrixMultiply( m_ModelMtxs[k], m_ShadowMan.getModelviews()[i]), m_ShadowMan.getProjections()[i] );
                m_DepthConstantBufferData.matModelViewProj = matCameraMeshModelViewProj;
                m_pDepthConstantBuffer->Update( &m_DepthConstantBufferData );
                m_pDepthConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS );
                
                m_Meshes[ m_nCurMeshIndex ].Render( &m_DepthShader );
            }

            matCameraMeshModelViewProj   = FrmMatrixMultiply( m_ShadowMan.getModelviews()[i], m_ShadowMan.getProjections()[i] );            
            m_DepthConstantBufferData.matModelViewProj = matCameraMeshModelViewProj;
            m_pDepthConstantBuffer->Update( &m_DepthConstantBufferData );
            m_pDepthConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS );

            DrawGround();

        }

        EndFBO( curRT );
    }

    // Clear the backbuffer, and reset the depth stencil.
    const float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_windowRenderTargetView.Get(), clearColor );
    D3DDeviceContext()->ClearDepthStencilView( m_windowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

    m_ShadowMan.m_Shaders[ m_nCurShaderIndex ].Bind();	
    {
        m_ShadowMan.m_CSMConstantBufferData.light = FRMVECTOR4( m_vSunDirection, 1.0 );
        
        //set up depth map
        D3DDeviceContext()->PSSetShaderResources( 0, 1, curRT->m_pTextureShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, curRT->m_pTextureSampler.GetAddressOf() );


        if( m_nCurShaderIndex == ESM_OPTIMIZED)
        {            
            for (int s = 0; s < 4; s++)
            {
                m_ShadowMan.m_CSMConstantBufferData.shadowsSpheres[s] = m_ShadowMan.getSpheres()[s];
            }
        }

        for(int i = 0; i < MODEL_DIMENSION * MODEL_DIMENSION; ++i)
        {
            m_matCameraMeshModelView      = FrmMatrixMultiply( m_ModelMtxs[i], m_matCameraView );
            m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
            m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

            m_ShadowMan.m_CSMConstantBufferData.matModelViewProj = m_matCameraMeshModelViewProj;
            
            FRMMATRIX4X4 worldMtx = m_ModelMtxs[i];

            m_ShadowMan.m_CSMConstantBufferData.matWorld = worldMtx;

            m_ShadowMan.m_CSMConstantBufferData.matNormal = FRMMATRIX4X4( m_matCameraMeshNormal );

            for (int j = 0; j < 4; j++)
            {
                m_ShadowMan.m_CSMConstantBufferData.offsets[j] = FRMVECTOR4( m_ShadowMan.getOffsets()[j], 1.0 );
                m_ShadowMan.m_CSMConstantBufferData.modelviewprojections[j] = m_ShadowMan.getModelviewProjections()[j];
            }

            m_ShadowMan.m_pCSMConstantBuffer->Update( &m_ShadowMan.m_CSMConstantBufferData );
            m_ShadowMan.m_pCSMConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS |  CFrmConstantBuffer::BindFlagPS);
            
            m_Meshes[ m_nCurMeshIndex ].Render(  &m_ShadowMan.m_Shaders[ m_nCurShaderIndex ] );
        }
        
        m_matCameraMeshModelView      = m_matCameraView;
        m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
        m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

        m_ShadowMan.m_CSMConstantBufferData.matModelViewProj = m_matCameraMeshModelViewProj;

        FRMMATRIX3X3 normMat = FrmMatrixNormalOrthonormal( FrmMatrixIdentity() );
        m_ShadowMan.m_CSMConstantBufferData.matWorld = FrmMatrixIdentity();
        m_ShadowMan.m_CSMConstantBufferData.matNormal = FRMMATRIX4X4( normMat );
        for (int j = 0; j < 4; j++)
        {
            m_ShadowMan.m_CSMConstantBufferData.offsets[j] = FRMVECTOR4( m_ShadowMan.getOffsets()[j], 1.0 );
            m_ShadowMan.m_CSMConstantBufferData.modelviewprojections[j] = m_ShadowMan.getModelviewProjections()[j];
        }

        m_ShadowMan.m_pCSMConstantBuffer->Update( &m_ShadowMan.m_CSMConstantBufferData );
        m_ShadowMan.m_pCSMConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS |  CFrmConstantBuffer::BindFlagPS);

        DrawGround();
        
    }

    // Show render target previews
    if( m_Preview )
    {
        D3DDeviceContext()->PSSetShaderResources( 0, 1, curRT->m_pTextureShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, curRT->m_pTextureSampler.GetAddressOf() );

        m_RenderTextureToScreen.RenderTextureToScreen( -5, 5, 164, 164, NULL,
                                    &m_OverlayShader, m_pOverlayConstantBuffer, &m_OverlayConstantBufferData,                                    
                                    offsetof(OverlayConstantBuffer, vScreenSize) );
    }		

    // Update the timer
    m_Timer.MarkFrame();
    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
    

}

