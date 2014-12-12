//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once

// The Parent class
#include "../scene.h"


//--------------------------------------------------------------------------------------
// Name: struct SOffscreenRT
// Desc: Stores information and handles for off-screen render targets
//--------------------------------------------------------------------------------------
struct SOffscreenRT
{
    VOID Clear();

    UINT32  BufferHandle;       // GL_FRAMEBUFFER
    UINT32  TextureHandle;      // GL_TEXTURE_2D
    UINT32  DepthHandle;
    UINT32  Width;
    UINT32  Height;
};


//--------------------------------------------------------------------------------------
// Name: struct SToonShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct SToonShader
{
    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ModelViewMatrixLoc;
    INT32   ModelViewProjMatrixLoc;
    INT32   NormalMatrixLoc;
    INT32   LightPositionLoc;
    INT32   MaterialAmbientLoc;
    INT32   MaterialDiffuseLoc;
    INT32   MaterialSpecularLoc;
    INT32   ColorRampId;

    // textures
    CFrmTexture*    ColorRampTexture;
};


//--------------------------------------------------------------------------------------
// Name: struct SNormalShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct SNormalShader
{
    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ModelViewMatrixLoc;
    INT32   ModelViewProjMatrixLoc;
    INT32   NormalMatrixLoc;
    INT32   LightPositionLoc;
    INT32   MaterialAmbientLoc;
    INT32   MaterialDiffuseLoc;
    INT32   MaterialSpecularLoc;
};


//--------------------------------------------------------------------------------------
// Name: struct SCombineShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct SCombineShader
{
    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   NormalTextureId;
    INT32   ToonTextureId;
    INT32   StepSizeId;
    INT32   SlopeBias;
};


//--------------------------------------------------------------------------------------
// Name: class CSample14
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample14 : public CScene
{
    // Off-screen render targets used by the app
    enum
    {
        RT_NORMALS,
        RT_TOON,
        MAX_RTS
    };


public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample14( const CHAR* strName );

private:
    BOOL InitShaders();

    VOID ClearOffscreenRTs();
    BOOL CreateOffscreenRT( UINT32 RenderTargetId, UINT32 Width, UINT32 Height );
    VOID FreeOffscreenRTs();
    VOID SetFramebuffer( UINT32 RenderTargetId );
    VOID FreeOffscreenRT( UINT32 RenderTargetId );

    VOID RenderSceneNormals();
    VOID RenderSceneToon();
    VOID RenderCombinedScene();

    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    CFrmMesh           m_Mesh;

    FRMMATRIX4X4       m_matModel;
    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormal;

    SOffscreenRT       m_OffscreenRT[MAX_RTS];

    // Shader related data
    INT32              m_hConstantModelViewProjMatrixLoc;
    INT32              m_hConstantColorLoc;
    INT32              m_hConstantDeformSize;
    FLOAT32            m_OutlineThickness;
    FLOAT32            m_SlopeBias;

    SToonShader        m_ToonShader;
    SNormalShader      m_NormalShader;
    SCombineShader     m_CombineShader;

    BOOL               m_bShouldRotate;
    FLOAT              m_fRotateTime;
};

