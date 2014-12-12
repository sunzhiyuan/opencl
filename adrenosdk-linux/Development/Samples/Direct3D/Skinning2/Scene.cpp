//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#include <FrmPlatform.h>
#include <FrmApplication.h>
#include <Direct3D/FrmFontD3D.h>
#include <Direct3D/FrmMesh.h>
#include <Direct3D/FrmShader.h>
#include <Direct3D/FrmPackedResourceD3D.h>
#include <Direct3D/FrmUserInterfaceD3D.h>
#include <Direct3D/FrmUtilsD3D.h>
#include <Model/FrmModelIO.h>
#include "Scene.h"


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Skinning2" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_pModel = NULL;
    m_pAnim = NULL;

    m_pLogoTexture = NULL;
    m_pModelTexture = NULL;

    m_ppVertexBuffer = NULL;
    m_ppIndexBuffer = NULL;

    m_vertexFormatMap = NULL;

    m_cameraPosition = FRMVECTOR3( 10.0f, 15.0f, 25.0f );
    m_cameraLookAt = FRMVECTOR3( 0.0f, 5.0f, 0.0f );
    m_cameraUp = FRMVECTOR3( 0.0f, 1.0f, 0.0f );

    m_lightPosition = FRMVECTOR3( 50.0f, 50.0f, 50.0f );

    m_curAnimFrame = 0;
    m_lastAnimFrame = 0;
    m_frameWeight = 1.0f;

    m_animStartIndex = 0;
    m_animLength = 0;

    m_speedFactor = 1.0f;

    m_pSkinningConstantBuffer = NULL;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Create the font
    if( FALSE == m_Font.Create( "Tuffy12.pak" ) )
    {
        return FALSE;
    }

    // Load the packed resources
    CFrmPackedResourceD3D resource;
    if( FALSE == resource.LoadFromFile( "Textures.pak" ) )
    {
        return FALSE;
    }

    // Load the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );
    if ( !m_pLogoTexture )
    {
        return FALSE;
    }

    m_pNormalMapping = resource.GetTexture( "blinn2" );
    if ( !m_pNormalMapping )
    {
        return FALSE;
    }

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
    {
        return FALSE;
    }

    // Enter the help text
    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_speedFactor , "Speed", "x%3.2f" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Speed up animation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Slow down animation" );

    // Load model
    m_pModel = Adreno::FrmLoadModelFromFile( "scorpion.model" );
    if( m_pModel == NULL )
    {
        return FALSE;
    }

    // Load animation
    m_pAnim = Adreno::FrmLoadAnimationFromFile( "scorpion.anim" );
    if( m_pAnim == NULL )
    {
        return FALSE;
    }

    // Pre-process all bone transformations
    if ( !SetupBoneTransform( m_pModel, m_pAnim ) )
    {
        return FALSE;
    }

    // Get rid of unused bones through remapping
    if ( !RemapBoneIndices( m_pModel, m_pAnim ) )
    {
        return FALSE;
    }

    // Load model textures
    if ( m_pModel->NumMaterials > 0 )
    {
        m_pModelTexture = new CFrmTexture*[m_pModel->NumMaterials];
        memset( m_pModelTexture, 0, sizeof(CFrmTexture*) * m_pModel->NumMaterials );

        for ( INT32 materialIndex = 0; materialIndex < m_pModel->NumMaterials; ++materialIndex )
        {
            Adreno::Material* pMaterial = m_pModel->Materials + materialIndex;

            m_pModelTexture[materialIndex] = resource.GetTexture( pMaterial->Id.Name );

            if ( !m_pModelTexture[materialIndex] )
            {
                return FALSE;
            }
        }
    }

    // Create vertex and index buffers, and map vertex format
    m_ppVertexBuffer = new CFrmVertexBuffer*[m_pModel->NumMeshes];
    memset( m_ppVertexBuffer, 0, sizeof(CFrmVertexBuffer*) * m_pModel->NumMeshes );

    m_ppIndexBuffer = new CFrmIndexBuffer*[m_pModel->NumMeshes];
    memset( m_ppIndexBuffer, 0, sizeof(CFrmIndexBuffer*) * m_pModel->NumMeshes );

    m_vertexFormatMap = new VERTEX_FORMAT_MAP[m_pModel->NumMeshes];
    memset( m_vertexFormatMap, 0, sizeof(VERTEX_FORMAT_MAP) * m_pModel->NumMeshes );
    
    for ( INT32 meshIndex = 0; meshIndex < m_pModel->NumMeshes; ++meshIndex )
    {
        Adreno::Mesh* pMesh = m_pModel->Meshes + meshIndex;

        if( !FrmCreateVertexBuffer( pMesh->Vertices.NumVerts, pMesh->Vertices.Format.Stride, pMesh->Vertices.Buffer, &m_ppVertexBuffer[meshIndex] ) )
        {
            return FALSE;
        }

        if( !FrmCreateIndexBuffer( pMesh->Indices.NumIndices, sizeof( UINT32 ), pMesh->Indices.Indices, &m_ppIndexBuffer[meshIndex] ) )
        {
            return FALSE;
        }

        m_vertexFormatMap[meshIndex].position = GetPropertyIndexFromName( pMesh, "position" );
        m_vertexFormatMap[meshIndex].normal = GetPropertyIndexFromName( pMesh, "normal" );
        m_vertexFormatMap[meshIndex].tangent = GetPropertyIndexFromName( pMesh, "tangent" );
        m_vertexFormatMap[meshIndex].binormal = GetPropertyIndexFromName( pMesh, "binormal" );
        m_vertexFormatMap[meshIndex].boneIndex = GetPropertyIndexFromName( pMesh, "skinindex" );
        m_vertexFormatMap[meshIndex].boneWeight = GetPropertyIndexFromName( pMesh, "skinweight" );
        m_vertexFormatMap[meshIndex].texCoord = GetPropertyIndexFromName( pMesh, "texcoord" );
    }

    // Initialize the shaders
    if( FALSE == InitSkinningShader() )
    {
        return FALSE;
    }

    // Specify animation range to play
    m_animStartIndex = 10;
    m_animLength = 110;

    // Render state blocks
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.FrontCounterClockwise = TRUE; // Change the winding direction to match GL    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_RasterizerState);
    D3DDeviceContext()->RSSetState(m_RasterizerState.Get());


    // Successful
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32) m_nWidth / (FLOAT32) m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 0.1f, 200.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Set the viewport
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
// Name: Update()
// Desc: Use the timer and input to update objects in the scene
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    static UINT32 nAnimTicks = 0;

    // Get the current time
    FLOAT32 fTime = m_Timer.GetFrameTime();
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

    // Update animation ticks
    nAnimTicks += (UINT32) ( fElapsedTime * FRM_ANIMATION_TICKS_PER_SEC );

    // Speed up animation
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_speedFactor += SPEED_STEP;
        if ( m_speedFactor > MAX_SPEED )
        {
            m_speedFactor = MAX_SPEED;
        }
    }

    // Slow down animation
    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_speedFactor -= SPEED_STEP;
        if ( m_speedFactor < MIN_SPEED )
        {
            m_speedFactor = MIN_SPEED;
        }
    }

    // Only update animation if we have speed
    if ( m_speedFactor > 0.0f )
    {
        // Get current ticks per frame
        UINT32 ticksPerFrame = ( UINT32 ) ( TICKS_PER_FRAME / m_speedFactor );

        // Compensate tick count for change in speed
        if( nPressedButtons & ( INPUT_KEY_1 | INPUT_KEY_2 ) )
        {
            nAnimTicks = ( m_lastAnimFrame - m_animStartIndex ) * ticksPerFrame + nAnimTicks % ticksPerFrame;
        }

        // Figure the current animation frame
        UINT32 numFrames = ( m_animLength == 0 ) ? m_pAnim->NumFrames - m_animStartIndex : m_animLength;
        UINT32 nAnimFrame = nAnimTicks / ticksPerFrame;
        m_lastAnimFrame = m_animStartIndex + nAnimFrame % numFrames;
        m_curAnimFrame = ( m_lastAnimFrame != m_animStartIndex + numFrames - 1 ) ? m_lastAnimFrame + 1 : m_animStartIndex;
        m_frameWeight = ( FLOAT32 ) ( nAnimTicks - nAnimFrame * ticksPerFrame ) / ticksPerFrame;
    }
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: Render the scene
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{    
    // Bind the backbuffer as a render target
    D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), m_windowDepthStencilView.Get() );

    // Clear the backbuffer to a solid color, and reset the depth stencil.
    const float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_windowRenderTargetView.Get(), clearColor );
    D3DDeviceContext()->ClearDepthStencilView( m_windowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

    // Draw the skinned mesh
    DrawSkinnedMesh();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroy objects created by the sample
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Delete vertex buffers
    if( m_ppVertexBuffer )
    {
        for ( int i = 0; i < m_pModel->NumMeshes; i++ )
        {
            if ( m_ppVertexBuffer[i] ) m_ppVertexBuffer[i]->Release();
        }

        delete[] m_ppVertexBuffer;
    }

    // Delete index buffers
    if( m_ppIndexBuffer )
    {
        for ( int i = 0; i < m_pModel->NumMeshes; i++ )
        {
            if ( m_ppIndexBuffer[i] ) m_ppIndexBuffer[i]->Release();
        }
        
        delete[] m_ppIndexBuffer;
    }

    // Delete vertex format map
    if( m_vertexFormatMap )
    {
        delete[] m_vertexFormatMap;
    }

    // Release model textures
    if( m_pModelTexture )
    {
        for ( INT32 materialIndex = 0; materialIndex < m_pModel->NumMaterials; ++materialIndex )
        {
            m_pModelTexture[materialIndex]->Release();
        }

        delete[] m_pModelTexture;
    }

    // Destroy model
    if ( m_pModel )
    {
        Adreno::FrmDestroyLoadedModel( m_pModel );
    }

    // Destroy animation
    if ( m_pAnim )
    {
        Adreno::FrmDestroyLoadedAnimation( m_pAnim );
    }

    // Release logo texture
    if( m_pLogoTexture )
    {
        m_pLogoTexture->Release();
    }

    if ( m_pSkinningConstantBuffer )
    {
        m_pSkinningConstantBuffer->Release();
    }
}


//--------------------------------------------------------------------------------------
// Name: InitSkinningShader()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitSkinningShader()
{
    // Shader attributes
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "POSITION",       FRM_VERTEX_POSITION,    DXGI_FORMAT_R32G32B32_FLOAT },
        { "NORMAL",         FRM_VERTEX_NORMAL,      DXGI_FORMAT_R32G32B32_FLOAT },
        { "TANGENT",        FRM_VERTEX_TANGENT,     DXGI_FORMAT_R32G32B32_FLOAT },
        { "BINORMAL",       FRM_VERTEX_BINORMAL,    DXGI_FORMAT_R32G32B32_FLOAT },        
        { "BLENDINDICES",   FRM_VERTEX_BONEINDICES, DXGI_FORMAT_R8G8B8A8_UINT },
        { "BLENDWEIGHT",    FRM_VERTEX_BONEWEIGHTS, DXGI_FORMAT_R32_FLOAT },
        { "TEXCOORD",       FRM_VERTEX_TEXCOORD0,   DXGI_FORMAT_R32G32_FLOAT },
    };

    // The number of shader attributes
    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

    // Compile shader
    if( FALSE == m_SkinningShader.Compile( "Skinning2VS.cso", "Skinning2PS.cso", pShaderAttributes, nNumShaderAttributes ) )
    {
        return FALSE;
    }

    if( FALSE == FrmCreateConstantBuffer( sizeof(m_SkinningConstantBufferData), &m_SkinningConstantBufferData, &m_pSkinningConstantBuffer ) )
    {
        return FALSE;
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: SetupBoneTransform()
// Desc: Pre-process all bone transforms and flatten the bones hierarchy
//--------------------------------------------------------------------------------------
BOOL CSample::SetupBoneTransform( Adreno::Model* pModel, Adreno::Animation* pAnim )
{
    // If there is a mismatch, return false
    if ( pModel->NumJoints != pAnim->NumTracks )
    {
        return FALSE;
    }

    // Process each of the bones
    for ( INT32 boneIndex = 0; boneIndex < pModel->NumJoints; ++boneIndex )
    {
        Adreno::AnimationTrack* pTrack = m_pAnim->Tracks + boneIndex;

        // If there are frames, process them, in addition to the default transform
        if ( pTrack->NumKeyframes > 0 )
        {
            Adreno::Transform transform;

            // For each frame, calculate and save the final transform for the bone
            for ( INT32 frameIndex = 0; frameIndex < pAnim->NumFrames; ++frameIndex )
            {
                GetBoneTransform( pModel, pAnim, boneIndex, frameIndex, transform );
                pTrack->Keyframes[frameIndex] = transform;
            }

            // Do the same for the default transform
            GetBoneTransform( pModel, pAnim, boneIndex, -1, transform );
            pModel->Joints[boneIndex].Transform = transform;
        }
        else
        {
            // Calculate and save the final default transform
            Adreno::Transform transform;
            GetBoneTransform( pModel, pAnim, boneIndex, -1, transform );
            pModel->Joints[boneIndex].Transform = transform;
        }

        // Flatten the bone hierarchy
        pModel->Joints[boneIndex].ParentIndex = -1;
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: RemapBoneIndices()
// Desc: Generates a table of the bones that are used by the animation
//--------------------------------------------------------------------------------------
BOOL CSample::RemapBoneIndices( Adreno::Model* pModel, Adreno::Animation* pAnim )
{
    // Return false if number of total bones is greater than supported
    if ( pModel->NumJoints > TOTAL_BONES )
    {
        return FALSE;
    }

    // Temporary buffer
    UINT8 processedBone[TOTAL_BONES];
    memset( processedBone, 0xFF, sizeof( processedBone ) );

    // The number of bones remapped
    m_boneRemapCount = 0;

    // Process each mesh
    for ( INT32 meshIndex = 0; meshIndex < m_pModel->NumMeshes; ++meshIndex )
    {
        Adreno::Mesh* pMesh = pModel->Meshes + meshIndex;

        // Find the index for skinindex and skinweight properties
        INT32 skinIndexProperty = GetPropertyIndexFromName( pMesh, "skinindex" );
        INT32 skinWeightProperty = GetPropertyIndexFromName( pMesh, "skinweight" );

        if ( skinIndexProperty < 0 || skinWeightProperty < 0 )
        {
            continue;
        }

        // The number of bones that can influence a vertex
        UINT32 numberOfBones = pMesh->Vertices.Format.Properties[skinIndexProperty].NumValues();

        // Look at each vertex of the mesh
        for ( UINT32 vertexIndex = 0, vertexOffset = 0; vertexIndex < pMesh->Vertices.NumVerts; ++vertexIndex, vertexOffset += pMesh->Vertices.Format.Stride )
        {
            UINT32* pBoneIndex = ( UINT32* ) ( pMesh->Vertices.Buffer + vertexOffset + pMesh->Vertices.Format.Properties[skinIndexProperty].Offset );
            FLOAT32* pBoneWeight = ( FLOAT32* ) ( pMesh->Vertices.Buffer + vertexOffset + pMesh->Vertices.Format.Properties[skinWeightProperty].Offset );

            // Add each bone that transform vertices to the table
            for ( UINT32 boneCount = 0; boneCount < numberOfBones; ++boneCount )
            {
                UINT32 boneIndex = *( pBoneIndex + boneCount );
                FLOAT32 boneWeight = *( pBoneWeight + boneCount );

                // Ignore bones that have no influence (weight is zero)
                if ( boneWeight != 0.0f )
                {
                    if ( processedBone[boneIndex] == 0xFF )
                    {
                        if ( m_boneRemapCount >= MAX_BONES )
                        {
                            return FALSE;
                        }
                        
                        processedBone[boneIndex] = m_boneRemapCount;
                        m_boneRemap[m_boneRemapCount++] = boneIndex;
                    }
                    
                    *( pBoneIndex + boneCount ) = processedBone[boneIndex];
                }
            }
        }
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: GetPropertyIndexFromName()
// Desc: Returns index for a given property name
//--------------------------------------------------------------------------------------
INT32 CSample::GetPropertyIndexFromName( Adreno::Mesh* pMesh, char* propertyName )
{
    Adreno::NamedId propertyNameId( propertyName );

    for ( INT32 propertyIndex = 0; propertyIndex < pMesh->Vertices.Format.NumProperties; ++propertyIndex )
    {
        if ( pMesh->Vertices.Format.Properties[propertyIndex].Usage == propertyNameId )
        {
            return propertyIndex;
        }
    }

    return -1;
}


//--------------------------------------------------------------------------------------
// Name: GetBoneTransform()
// Desc: Get final transform for a bone
//--------------------------------------------------------------------------------------
void CSample::GetBoneTransform( Adreno::Model* pModel, Adreno::Animation* pAnim, INT32 boneIndex, INT32 frameIndex, Adreno::Transform& transform )
{
    Adreno::Joint* pJoint = m_pModel->Joints + boneIndex;
    Adreno::AnimationTrack* pTrack = m_pAnim->Tracks + boneIndex;
    
    if ( frameIndex >= 0 && frameIndex < pTrack->NumKeyframes )
    {
        transform = *( pTrack->Keyframes + frameIndex );
    }
    else
    {
        transform = pJoint->Transform;
    }

    if (pJoint->ParentIndex != -1)
    {
        Adreno::Transform parentTransform;
        GetBoneTransform( pModel, pAnim, pJoint->ParentIndex, frameIndex, parentTransform );

        FRMMATRIX4X4 parentRotation = FrmMatrixRotate( parentTransform.Rotation );
        transform.Position = FrmVector3TransformCoord( transform.Position, parentRotation );
        transform.Rotation = FrmQuaternionMultiply( transform.Rotation, parentTransform.Rotation );
        transform.Position += parentTransform.Position;
    }
}


//--------------------------------------------------------------------------------------
// Name: DrawSkinnedMesh()
// Desc: Draw the skinned mesh
//--------------------------------------------------------------------------------------
VOID CSample::DrawSkinnedMesh()
{
    m_SkinningShader.Bind();

    // Create the view matrix
    m_matView = FrmMatrixLookAtRH( m_cameraPosition, m_cameraLookAt, m_cameraUp );

    // Set view and projection matrices
    m_SkinningConstantBufferData.g_matView = m_matView;
    m_SkinningConstantBufferData.g_matProj = m_matProj;

    // Set camera and light positions
    m_SkinningConstantBufferData.g_cameraPos = FRMVECTOR4( m_cameraPosition, 1.0 );
    m_SkinningConstantBufferData.g_lightPos = FRMVECTOR4( m_lightPosition, 1.0 );

    // Set material color properties 
    FRMVECTOR4 vAmbient( 0.05f, 0.05f, 0.05f, 1.0f );
    FRMVECTOR4 vDiffuse( 1.0f, 0.5f, 0.5f, 1.0f );
    FRMVECTOR4 vSpecular( 0.5f, 0.5f, 0.5f, 1.0f );
    FLOAT32 fShininess = 16.0f;

    m_SkinningConstantBufferData.g_vAmbient = vAmbient;
    m_SkinningConstantBufferData.g_vDiffuse = vDiffuse;
    m_SkinningConstantBufferData.g_vSpecular = vSpecular;
    m_SkinningConstantBufferData.g_fShininess = fShininess;
    
    // Render each of the meshes
    for ( INT32 meshIndex = 0; meshIndex < m_pModel->NumMeshes; ++meshIndex )
    {
        Adreno::Mesh* pMesh = m_pModel->Meshes + meshIndex;

        // Set vertex buffer
        m_ppVertexBuffer[meshIndex]->Bind( 0 );
                
        // Set index buffer
        m_ppIndexBuffer[meshIndex]->Bind( 0 );

        // Prepare this frame's transforms for each of the bones
        for( UINT32 boneIndex = 0; boneIndex < m_boneRemapCount; ++boneIndex )
        {
            FRMMATRIX4X4 matBoneMatrix = FrmMatrixIdentity();

            // Get the current and last transforms
            Adreno::Joint* pJoint = m_pModel->Joints + m_boneRemap[boneIndex];
            Adreno::AnimationTrack* pTrack = m_pAnim->Tracks + m_boneRemap[boneIndex];

            Adreno::Transform* pCurTransform;
            Adreno::Transform* pLastTransform;

            if ( pTrack->NumKeyframes >= 0 )
            {
                pCurTransform = pTrack->Keyframes + m_curAnimFrame;
                pLastTransform = pTrack->Keyframes + m_lastAnimFrame;
            }
            else
            {
                pCurTransform = &pJoint->Transform;
                pLastTransform = &pJoint->Transform;
            }

            // Interpolate between the last and current transforms
            Adreno::Transform transform;
            transform.Position = FrmVector3Lerp( pLastTransform->Position, pCurTransform->Position, m_frameWeight );
            transform.Rotation = FrmVector4SLerp( pLastTransform->Rotation, pCurTransform->Rotation, m_frameWeight );

            // Apply inverse bind transform to final matrix
            FRMMATRIX4X4 matInverseBindPosition = FrmMatrixTranslate( pJoint->InverseBindPose.Position );
            FRMMATRIX4X4 matInverseBindRotation = FrmMatrixRotate( pJoint->InverseBindPose.Rotation );

            matBoneMatrix = matInverseBindRotation;
            matBoneMatrix = FrmMatrixMultiply( matBoneMatrix, matInverseBindPosition );

            // Apply interpolated transform to final matrix
            FRMMATRIX4X4 matBonePosition = FrmMatrixTranslate( transform.Position );
            FRMMATRIX4X4 matBoneRotation = FrmMatrixRotate( transform.Rotation );

            matBoneMatrix = FrmMatrixMultiply( matBoneMatrix, matBoneRotation );
            matBoneMatrix = FrmMatrixMultiply( matBoneMatrix, matBonePosition );

            // Place it in a 4x3 matrix
            matBoneMatrix = FrmMatrixTranspose( matBoneMatrix );
            memcpy( &m_SkinningConstantBufferData.g_matWorldArray[boneIndex * 3], &matBoneMatrix, sizeof(FRMMATRIX4X3) );
        }

        // Update constant buffer
        m_pSkinningConstantBuffer->Update( &m_SkinningConstantBufferData );
        m_pSkinningConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

        // Render each mesh surface
        for ( UINT32 surfaceIndex = 0; surfaceIndex < pMesh->Surfaces.NumSurfaces; ++surfaceIndex )
        {
            Adreno::MeshSurface* pSurface = pMesh->Surfaces.Surfaces + surfaceIndex;

            // Set the material for the surface
            m_pModelTexture[pSurface->MaterialId]->Bind( 0 );

            // Set the normal mapping for the surface
            m_pNormalMapping->Bind( 1 );

            // Draw the surface
            FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pSurface->NumTriangles * 3, sizeof(UINT32), pSurface->StartIndex * sizeof(UINT32) );
        }
    }
}
