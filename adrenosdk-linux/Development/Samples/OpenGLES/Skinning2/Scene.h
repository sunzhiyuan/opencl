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
// Defining our own custom vertex property constants
//--------------------------------------------------------------------------------------
enum FRM_VERTEX_USAGE_CUSTOM
{
	FRM_VERTEX_CUSTOM_BONEINDEX1 = FRM_VERTEX_BONEINDICES,
	FRM_VERTEX_CUSTOM_BONEINDEX2 = FRM_VERTEX_COLOR0,
	FRM_VERTEX_CUSTOM_BONEINDEX3 = FRM_VERTEX_COLOR1,
};


//--------------------------------------------------------------------------------------
// Assigned slots for the shader constants
//--------------------------------------------------------------------------------------
struct SHADER_CONSTANTS
{
    INT32 m_slotWorldMatrix;
    INT32 m_slotViewMatrix;
    INT32 m_slotProjMatrix;
	INT32 m_slotLightPosition;
	INT32 m_slotCameraPosition;
	INT32 m_slotMaterialAmbient;
    INT32 m_slotMaterialDiffuse;
    INT32 m_slotMaterialSpecular;
	INT32 m_slotMaterialShininess;
};


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
    CFrmFontGLES          m_Font;
    CFrmTimer             m_Timer;
    CFrmUserInterfaceGLES m_UserInterface;

	// Camera and light information
    FRMVECTOR3            m_cameraPosition;
	FRMVECTOR3            m_cameraLookAt;
	FRMVECTOR3            m_cameraUp;
	FRMVECTOR3            m_lightPosition;

    // Skinning shader
    GLuint                m_hSkinningShader;
    SHADER_CONSTANTS      m_SkinningConstants;

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

	// OpenGL Buffers
	GLuint*               m_hVertexBuffer;
	GLuint*               m_hIndexBuffer;

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
};


#endif // SCENE_H
