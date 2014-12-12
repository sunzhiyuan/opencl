//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H


//--------------------------------------------------------------------------------------
// The max number of bones is determined by the max number of vertex shader constants
// allocated towards the palette of bone matrices. Note that the mesh must have been
// created with this number in mind!
//--------------------------------------------------------------------------------------
const UINT32 MAX_BONES = 80;


//--------------------------------------------------------------------------------------
// The total number of bones that a model and animation file can have. Note that out of 
// TOTAL_BONES, only MAX_BONES can have vertices attached in order to process skinning
// in the GPU.
//--------------------------------------------------------------------------------------
const UINT32 TOTAL_BONES = 256;


//--------------------------------------------------------------------------------------
// The number of ticks per animation frame
//--------------------------------------------------------------------------------------
const UINT32 TICKS_PER_FRAME = 150;


//--------------------------------------------------------------------------------------
// The speed modification constants
//--------------------------------------------------------------------------------------
const FLOAT32 SPEED_STEP = 0.1f;
const FLOAT32 MIN_SPEED = 0.0f;
const FLOAT32 MAX_SPEED = 5.0f;



//--------------------------------------------------------------------------------------
// Vertex format map structure
//--------------------------------------------------------------------------------------
struct VERTEX_FORMAT_MAP
{
    UINT32 position;
    UINT32 normal;
    UINT32 tangent;
    UINT32 binormal;
    UINT32 boneIndex;
    UINT32 boneWeight;
    UINT32 texCoord;
};


//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:

    CSample( const CHAR* strName );

    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

private:

    BOOL InitSkinningShader();
    
    BOOL SetupBoneTransform( Adreno::Model* pModel, Adreno::Animation* pAnim );
    BOOL RemapBoneIndices( Adreno::Model* pModel, Adreno::Animation* pAnim );

    INT32 GetPropertyIndexFromName( Adreno::Mesh* pMesh, char* propertyName );
    void GetBoneTransform( Adreno::Model* pModel, Adreno::Animation* pAnim, INT32 boneIndex, INT32 frameIndex, Adreno::Transform& transform );

    VOID DrawSkinnedMesh();

private:

    // Framework classes
    CFrmFontD3D           m_Font;
    CFrmTimer             m_Timer;
    CFrmUserInterfaceD3D  m_UserInterface;

    // Camera and light information
    FRMVECTOR3            m_cameraPosition;
    FRMVECTOR3            m_cameraLookAt;
    FRMVECTOR3            m_cameraUp;
    FRMVECTOR3            m_lightPosition;

    // Skinning shader
    CFrmShaderProgramD3D  m_SkinningShader;
    struct SkinningConstantBuffer
    {
        FRMVECTOR4   g_matWorldArray[MAX_BONES * 3];
        FRMMATRIX4X4 g_matView;
        FRMMATRIX4X4 g_matProj;
        FRMVECTOR4   g_lightPos;
        FRMVECTOR4   g_cameraPos;
        FRMVECTOR4   g_vAmbient;
        FRMVECTOR4   g_vDiffuse;
        FRMVECTOR4   g_vSpecular;
        FLOAT32      g_fShininess;
        FRMVECTOR3   Padding; // Pad to multiple of 16-bytes
    };
    SkinningConstantBuffer  m_SkinningConstantBufferData;
    CFrmConstantBuffer*     m_pSkinningConstantBuffer;

    // Transforms
    FRMMATRIX4X4          m_matWorld;
    FRMMATRIX4X4          m_matView;
    FRMMATRIX4X4          m_matProj;

    // Model and animation
    Adreno::Model*        m_pModel;
    Adreno::Animation*    m_pAnim;

    // Textures
    CFrmTexture*          m_pLogoTexture;
    CFrmTexture*          m_pNormalMapping;
    CFrmTexture**         m_pModelTexture;

    // Vertex/Index Buffers
    CFrmVertexBuffer**    m_ppVertexBuffer;
    CFrmIndexBuffer**     m_ppIndexBuffer;

    // Vertex format
    VERTEX_FORMAT_MAP*    m_vertexFormatMap;

    // Bone remapping
    UINT32                m_boneRemap[MAX_BONES];
    UINT32                m_boneRemapCount;

    // Animation frame
    INT32                 m_curAnimFrame;
    INT32                 m_lastAnimFrame;
    FLOAT32               m_frameWeight;

    // Animation speed and range
    FLOAT32               m_speedFactor;
    UINT32                m_animStartIndex;
    UINT32                m_animLength;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;

};


#endif // SCENE_H
