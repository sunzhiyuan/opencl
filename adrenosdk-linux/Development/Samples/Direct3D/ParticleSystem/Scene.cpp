//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
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
#include "Scene.h"


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "ParticleSystem" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_pParticleTexture = NULL;
    m_pLogoTexture     = NULL;

    m_pPointSpriteConstantBuffer = NULL;
    m_pParticleVertexBuffer      = NULL;
    m_pParticleIndexBuffer       = NULL;

    m_fParticleTimeX   = 0.0f;
    m_fParticleTimeY   = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name: InitParticles()
// Desc: Initializes the particles
//--------------------------------------------------------------------------------------
static BOOL InitParticles( PARTICLE* pParticles, CFrmVertexBuffer** ppVertBuffer, CFrmIndexBuffer** ppIndexBuffer )
{
    for( UINT32 i = 0; i < NUM_PARTICLES * 4; i++ )
    {
        pParticles[i].m_vPosition         =  FRMVECTOR3( 0.0f, 0.0f, 0.0f );
        pParticles[i].m_vVelocity         =  FRMVECTOR3( 0.0f, 0.0f, 0.0f );
        pParticles[i].m_vColor            =  FRMVECTOR4( 1.0, 0.0, 0.0, 0.0 );
        pParticles[i].m_fInitialSize      =  0.0f;
        pParticles[i].m_fSizeIncreaseRate =  0.0f;
        pParticles[i].m_fStartTime        =  0.0f;
        pParticles[i].m_fLifeSpan         =  -1.0f;

  
        // Initialize texture coordinates statically
        if (i % 4 <= 1)
        {
            pParticles[i].m_vTexCoord.x = 0.0f;
            pParticles[i].m_vTexCoord.y = (i % 4) == 0 ? 1.0f : 0.0f;
        }
        else
        {
            pParticles[i].m_vTexCoord.x = 1.0f;
            pParticles[i].m_vTexCoord.y = (i % 4) == 2 ? 0.0f : 1.0f;
        }
    }

    // Create VB/IB
    if ( FALSE == FrmCreateVertexBuffer( NUM_PARTICLES * 4, sizeof(PARTICLE), NULL, ppVertBuffer) )
        return FALSE;

    // Create indices for drawing as triangle list
    UINT32 *pIndices = new UINT32[NUM_PARTICLES * 6];
    for (int i = 0; i < NUM_PARTICLES; i ++)
    {
        pIndices[i * 6 + 0] = i * 4 + 0;
        pIndices[i * 6 + 1] = i * 4 + 1;
        pIndices[i * 6 + 2] = i * 4 + 2;
        pIndices[i * 6 + 3] = i * 4 + 0;
        pIndices[i * 6 + 4] = i * 4 + 2;
        pIndices[i * 6 + 5] = i * 4 + 3;
    }

    if ( FALSE == FrmCreateIndexBuffer( NUM_PARTICLES * 6, sizeof(UINT32), pIndices, ppIndexBuffer ) )
        return FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitEmitter()
// Desc: Initializes the particle emitter
//--------------------------------------------------------------------------------------
VOID CParticleEmitter::Init()
{
    m_iParticleIndex     = 0;

    // Play around with the values for different effects
    m_fEmissionRate             = 10000.0f; // 10000 particles per second
    m_vPosition                 = FRMVECTOR3( 0.0f, 0.0f, -0.0f );
    m_vVelocity                 = FRMVECTOR3( -1.0f, 0.0f, 0.0f );
    m_fEmitterRadius            = 0.01f;
    m_fEmitterSpread            = 20.0f; // Emit within X degrees of the direction vector
    m_fInitialLifeSpan          = 5.5f;  // Particle lifetime in seconds
    m_fLifeSpanVariance         = 1.5f;  
    m_fSpeedVariance            = 0.5f;
    m_fInitialSize              = 25.0f;
    m_fInitialSizeVariance      = 5.0f;
    m_fSizeIncreaseRate         = 40.0f;
    m_fSizeIncreaseRateVarience = 10.0f;
    m_fInitialOpacity           = 1.0f;
    m_fOpacityVariance          = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the point sprite shader
    {

        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "POSITION", FRM_VERTEX_POSITION,  DXGI_FORMAT_R32G32B32_FLOAT },
            { "TEXCOORD", FRM_VERTEX_TEXCOORD0, DXGI_FORMAT_R32G32B32_FLOAT },
            { "COLOR",    FRM_VERTEX_COLOR0,    DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD", FRM_VERTEX_TEXCOORD1, DXGI_FORMAT_R32_FLOAT },
            { "TEXCOORD", FRM_VERTEX_TEXCOORD2, DXGI_FORMAT_R32_FLOAT },
            { "TEXCOORD", FRM_VERTEX_TEXCOORD3, DXGI_FORMAT_R32_FLOAT },
            { "TEXCOORD", FRM_VERTEX_TEXCOORD4, DXGI_FORMAT_R32_FLOAT },
            { "TEXCOORD", FRM_VERTEX_TEXCOORD5, DXGI_FORMAT_R32G32_FLOAT },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_ParticleShader.Compile( "PointSpritesVS.cso", "PointSpritesPS.cso",                                                      
                                               pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer( sizeof(m_PointSpriteConstantBufferData), &m_PointSpriteConstantBufferData,
                                              &m_pPointSpriteConstantBuffer ) )
            return FALSE;
    }



    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: UpdateEmitterPosition()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CParticleEmitter::UpdateEmitterPosition( const FRMVECTOR3& vPosition )
{
    m_vPosition = vPosition;
}


//--------------------------------------------------------------------------------------
// Name: UpdateEmitterVelocity()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CParticleEmitter::UpdateEmitterVelocity( const FRMVECTOR3& vVelocity )
{
    m_vVelocity = vVelocity;
}


//--------------------------------------------------------------------------------------
// Name: GetNextParticle()
// Desc: 
//--------------------------------------------------------------------------------------
PARTICLE* CParticleEmitter::GetNextParticle( PARTICLE* pParticles )
{
    m_iParticleIndex = ( m_iParticleIndex + 1 ) % NUM_PARTICLES;
    return &pParticles[m_iParticleIndex * 4];
}
                                       
                                       
//--------------------------------------------------------------------------------------
// Name: EmitParticles()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CParticleEmitter::EmitParticles( PARTICLE* pParticles, FLOAT32 fElapsedTime,
                                      FLOAT32 fCurrentTime, FRMCOLOR Colors[NUM_COLORS],
                                      FLOAT32 fTimePerColor,
									  EmitParticleShader *epShader,
									  void *srcBuffer, void *dstBuffer,
									  FRM_VERTEX_ELEMENT* pVertexLayout,
									  UINT32 nVertexSize, BOOL bUseTransformFeedback )
{
    // Determine the color of the particles to be emitted
    FLOAT32  t = ( fCurrentTime / fTimePerColor ) - FrmFloor( fCurrentTime / fTimePerColor );
    UINT32   nColorIndex = (INT32)FrmFloor( fCurrentTime / fTimePerColor ) % NUM_COLORS;
    FRMCOLOR vColor1 = Colors[ nColorIndex ];
    FRMCOLOR vColor2 = Colors[(nColorIndex+1) % NUM_COLORS ];
    FRMCOLOR vColor  = FrmLerp( t, vColor1, vColor2 );

    m_fLeftOverParticles += m_fEmissionRate * fElapsedTime;

	if( !bUseTransformFeedback )
	{
		// Create the needed number of particles based upon our emission rate.
		for( INT32 i = 0; i < (INT32)NUM_PARTICLES; i++ )
		{
			// Get the next particle
			PARTICLE* pParticle = GetNextParticle( pParticles );

			if( ( ( pParticle->m_fStartTime + pParticle->m_fLifeSpan >= fCurrentTime * 0.001f ) || 
				( pParticle->m_fLifeSpan < 0.0f ) ) &&
				( FrmRand() < m_fLeftOverParticles/NUM_PARTICLES ) )
			{
				FLOAT32      fAngle            = m_fEmitterSpread * ( FrmRand() - 0.5f );
				FRMMATRIX4X4 matRotate         = FrmMatrixRotate( FrmRadians(fAngle), 0.0f, 0.0f, -1.0f );
				FRMVECTOR3   vVelocity         = FrmVector3TransformCoord( m_vVelocity, matRotate );

				pParticle->m_vPosition         = m_vPosition + FrmSphrand( m_fEmitterRadius );
				pParticle->m_vVelocity         = vVelocity * ApplyVariance( 1.0f, m_fSpeedVariance );
				pParticle->m_vColor            = FRMVECTOR4( vColor.r/255.0f, vColor.g/255.0f, vColor.b/255.0f, vColor.a/255.0f );
				pParticle->m_vColor.a          = ApplyVariance( m_fInitialOpacity, m_fOpacityVariance );
				pParticle->m_fStartTime        = fCurrentTime;
				pParticle->m_fLifeSpan         = ApplyVariance( m_fInitialLifeSpan, m_fLifeSpanVariance );
				pParticle->m_fInitialSize      = ApplyVariance( m_fInitialSize, m_fInitialSizeVariance );
				pParticle->m_fSizeIncreaseRate = ApplyVariance( m_fSizeIncreaseRate, m_fSizeIncreaseRateVarience );

                // Copy to the other three vertices
                for(int v = 1; v < 4; v++)
                {
                    pParticle[v].m_vPosition         = pParticle->m_vPosition;
				    pParticle[v].m_vVelocity         = pParticle->m_vVelocity;
				    pParticle[v].m_vColor            = pParticle->m_vColor;
				    pParticle[v].m_fStartTime        = pParticle->m_fStartTime;
				    pParticle[v].m_fLifeSpan         = pParticle->m_fLifeSpan;
				    pParticle[v].m_fInitialSize      = pParticle->m_fInitialSize;
				    pParticle[v].m_fSizeIncreaseRate = pParticle->m_fSizeIncreaseRate;
                }
			}
		}
	}
	else
	{
#ifdef TEMP
		// Using transform feedback to do particle emission

		// Set up the shader
		glUseProgram( epShader->ShaderID );
		glUniform3fv( epShader->slotvPosition, 1, (FLOAT32 *)&m_vPosition );
		glUniform3fv( epShader->slotvVelocity, 1, (FLOAT32 *)&m_vVelocity );
		glUniform1f( epShader->slotfEmitterSpread, m_fEmitterSpread );
		glUniform1f( epShader->slotfEmitterRadius, m_fEmitterRadius );
		glUniform1f( epShader->slotfSpeedVariance, m_fSpeedVariance );
		glUniform1f( epShader->slotfInitialOpacity, m_fInitialOpacity );
		glUniform1f( epShader->slotfOpacityVariance, m_fOpacityVariance );
		glUniform1f( epShader->slotfInitialLifeSpan, m_fInitialLifeSpan );
		glUniform1f( epShader->slotfLifeSpanVariance, m_fLifeSpanVariance );
		glUniform1f( epShader->slotfInitialSize, m_fInitialSize );
		glUniform1f( epShader->slotfInitialSizeVariance, m_fInitialSizeVariance );
		glUniform1f( epShader->slotfSizeIncreaseRate, m_fSizeIncreaseRate );
		glUniform1f( epShader->slotfSizeIncreaseRateVariance, m_fSizeIncreaseRateVarience );
		glUniform1f( epShader->slotfTime, fCurrentTime );
		glUniform4f( epShader->slotvColor, vColor.r/255.0f, vColor.g/255.0f, vColor.b/255.0f, vColor.a/255.0f );
		glUniform1f( epShader->slotfEmissionRate, m_fLeftOverParticles/NUM_PARTICLES );

		glEnable( GL_RASTERIZER_DISCARD );

		// set source buffer
		glBindBuffer( GL_ARRAY_BUFFER, *srcBuffer );
		FrmSetVertexLayout( pVertexLayout, nVertexSize, 0 );
	    
		// set destination buffer
		glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, *dstBuffer );
		
		// Perfom transform feedback
		glBeginTransformFeedback( GL_POINTS );
		glDrawArrays( GL_POINTS, 0, NUM_PARTICLES );
		glEndTransformFeedback();

		glDisable( GL_RASTERIZER_DISCARD );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0 );
#endif
	}

	m_fLeftOverParticles -= FrmFloor( m_fLeftOverParticles );
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    m_Colors[0]      = FRMCOLOR( 1.0f, 0.5f, 0.5f );
    m_Colors[1]      = FRMCOLOR( 1.0f, 1.0f, 0.5f );
    m_Colors[2]      = FRMCOLOR( 0.5f, 1.0f, 0.5f );
    m_Colors[3]      = FRMCOLOR( 0.5f, 1.0f, 1.0f );
    m_Colors[4]      = FRMCOLOR( 0.5f, 0.5f, 1.0f );
    m_Colors[5]      = FRMCOLOR( 1.0f, 0.5f, 1.0f );
    m_fXFactor       = 0.15f;
    m_fYFactor       = 0.10f;
    m_fParticleSpeed = 0.35f;
    m_fTimePerColor  = 1.0f;
    m_bUseSrcAlpha   = TRUE;

    if ( FALSE == InitParticles( m_Particles, &m_pParticleVertexBuffer, &m_pParticleIndexBuffer ) )
        return FALSE;
	    
    for( UINT32 i = 0; i < NUM_EMITTERS; ++i )
        m_ParticleEmitters[i].Init();

    // Create the font
    if( FALSE == m_Font.Create( "Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceD3D resource;
    if( FALSE == resource.LoadFromFile( "Textures.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Create the particle texture
    m_pParticleTexture = resource.GetTexture( "Particle" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_fXFactor , "X Factor", "%4.2f" );
    m_UserInterface.AddFloatVariable( &m_fYFactor , "Y Factor", "%4.2f" );
    m_UserInterface.AddFloatVariable( &m_fParticleSpeed , "Speed", "%4.2f" );
    m_UserInterface.AddBoolVariable( &m_bUseSrcAlpha , "Use Source Alpha", "Yes", "No" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0,   "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1,   "Decrease X Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2,   "Increase X Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3,   "Decrease Y Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4,   "Increase Y Factor" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5,   "Decrease Speed" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6,   "Increase Speed" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7,   "Toggle Blend Mode" );

    // Create the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.FrontCounterClockwise = TRUE; // Change the winding direction to match GL    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_RasterizerState);
    D3DDeviceContext()->RSSetState(m_RasterizerState.Get());

    D3D11_DEPTH_STENCIL_DESC dsdesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
    dsdesc.DepthEnable = false;
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DisabledDepthStencilState);
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );

    D3D11_BLEND_DESC bdesc = CD3D11_BLEND_DESC(D3D11_DEFAULT);
    bdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bdesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bdesc.RenderTarget[0].BlendEnable = TRUE;
    D3DDevice()->CreateBlendState(&bdesc, &m_SrcAlphaBlendState);

    bdesc = CD3D11_BLEND_DESC(D3D11_DEFAULT);
    bdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bdesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    bdesc.RenderTarget[0].BlendEnable = TRUE;
    D3DDevice()->CreateBlendState(&bdesc, &m_BlendState);
   

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 1000.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    FRMMATRIX4X4 m_matView;
    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.0f, 5.0f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matView = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matProj = FrmMatrixMultiply( m_matView, m_matProj );

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
    if( m_pPointSpriteConstantBuffer ) m_pPointSpriteConstantBuffer->Release();

    if( m_pParticleVertexBuffer ) m_pParticleVertexBuffer->Release();
    if( m_pParticleIndexBuffer ) m_pParticleIndexBuffer->Release();

    if( m_pParticleTexture )  m_pParticleTexture->Release();
    if( m_pLogoTexture )      m_pLogoTexture->Release();
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
    {
        UINT32 nButtons;
        UINT32 nPressedButtons;
        FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

        // Toggle user interface
        if( nPressedButtons & INPUT_KEY_0 )
            m_UserInterface.AdvanceState();

        if( nButtons & INPUT_KEY_1 )
            m_fXFactor -= m_fXFactor * fElapsedTime;

        if( nButtons & INPUT_KEY_2 )
            m_fXFactor += m_fXFactor * fElapsedTime;

        if( nButtons & INPUT_KEY_3 )
            m_fYFactor -= m_fYFactor * fElapsedTime;

        if( nButtons & INPUT_KEY_4 )
            m_fYFactor += m_fYFactor * fElapsedTime;

        if( nButtons & INPUT_KEY_5 )
            m_fParticleSpeed -= m_fParticleSpeed * fElapsedTime;

        if( nButtons & INPUT_KEY_6 )
            m_fParticleSpeed += m_fParticleSpeed * fElapsedTime;

        if( nPressedButtons & INPUT_KEY_7 )
            m_bUseSrcAlpha = !m_bUseSrcAlpha;
    }

    // Update the emitter position to do an interesting effect
    m_fParticleTimeX += fElapsedTime * m_fXFactor;
    m_fParticleTimeY += fElapsedTime * m_fYFactor;

    FRMVECTOR3 vEmitterPosition;
    vEmitterPosition.x = FrmSin( m_fParticleTimeX * 2*FRM_PI );
    vEmitterPosition.y = FrmCos( m_fParticleTimeY * 2*FRM_PI );
    vEmitterPosition.z = 0.0f;

    m_ParticleEmitters[0].UpdateEmitterPosition( vEmitterPosition );

    // Update the emitter direction to do an interesting effect
    FRMVECTOR3 vEmitterDirection;
    vEmitterDirection.x = FrmCos( fTime * 2*FRM_PI );
    vEmitterDirection.y = FrmSin( fTime * 2*FRM_PI );
    vEmitterDirection.z = 0.0f; 
    vEmitterDirection   = FrmVector3Normalize( vEmitterDirection );
    m_ParticleEmitters[0].UpdateEmitterVelocity( m_fParticleSpeed * vEmitterDirection );
	
    // Emit some new particles into the m_Particles buffer
    m_ParticleEmitters[0].EmitParticles( m_Particles, fElapsedTime, fTime, 
                                         m_Colors, m_fTimePerColor, NULL,
										 NULL, NULL,
										 NULL, sizeof(PARTICLE), FALSE );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    // Bind the backbuffer as a render target
    D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), m_windowDepthStencilView.Get() );

    // Clear the backbuffer to a solid color, and reset the depth stencil.
    const float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_windowRenderTargetView.Get(), clearColor );
    D3DDeviceContext()->ClearDepthStencilView( m_windowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

    // Draw the particles
    {
        float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT32 sampleMask = 0xffffffff;
        
        if( m_bUseSrcAlpha )
        {
            D3DDeviceContext()->OMSetBlendState( m_SrcAlphaBlendState.Get(), blendFactor, sampleMask );
        }
        else
        {
            D3DDeviceContext()->OMSetBlendState( m_BlendState.Get(), blendFactor, sampleMask );
        }

        m_pParticleTexture->Bind( 0 );
        m_PointSpriteConstantBufferData.matModelViewProj = m_matProj;
        m_PointSpriteConstantBufferData.fTime = m_Timer.GetFrameTime();
        m_pPointSpriteConstantBuffer->Update( &m_PointSpriteConstantBufferData );
        m_pPointSpriteConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS );

		
        m_ParticleShader.Bind();

        m_pParticleIndexBuffer->Bind( 0 );
            
        // Update the particles
        VOID* pBuf = m_pParticleVertexBuffer->Map(D3D11_MAP_WRITE_DISCARD);
        FrmMemcpy(pBuf, &m_Particles[0], sizeof(m_Particles));
        m_pParticleVertexBuffer->Unmap();

        // Bind the VB
        m_pParticleVertexBuffer->Bind( 0 );

        // Draw the particles
        FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, NUM_PARTICLES * 6, sizeof(UINT32), 0 );
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

