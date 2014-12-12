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
const UINT32 MAX_BONES = 26;


//--------------------------------------------------------------------------------------
// Assigned slots for the shader constants
//--------------------------------------------------------------------------------------
struct SHADER_CONSTANTS
{
    INT32 m_slotWorldMatrix;
    INT32 m_slotViewMatrix;
    INT32 m_slotProjMatrix;

    INT32 m_slotMaterialDiffuse;
    INT32 m_slotMaterialSpecular;
    INT32 m_slotMaterialAmbient;
};


//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

    CSample( const CHAR* strName );

private:
    BOOL InitShaders();

    // Framework classes
    CFrmFontGLES          m_Font;
    CFrmTimer             m_Timer;
    CFrmUserInterfaceGLES m_UserInterface;
    CFrmTexture*          m_pLogoTexture;

    // Skinned mesh and animation data
    UINT32                m_nActiveMesh;
    CFrmMesh              m_Mesh[2];
    FRM_ANIMATION_SET*    m_pAnimationSet[2];

    // Skinning shader
    GLuint                m_hSkinningShader;
    SHADER_CONSTANTS      m_SkinningConstants;

    // Transforms
    FRMMATRIX4X4          m_matWorld;
    FRMMATRIX4X4          m_matView;
    FRMMATRIX4X4          m_matProj;
};


#endif // SCENE_H
