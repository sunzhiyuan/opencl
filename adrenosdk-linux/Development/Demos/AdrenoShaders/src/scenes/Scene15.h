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
// Name: struct SOffscreenRT15
// Desc: Stores information and handles for off-screen render targets
//--------------------------------------------------------------------------------------
struct SOffscreenRT15
{
    SOffscreenRT15();
    VOID Clear();

    UINT32  BufferHandle;       // GL_FRAMEBUFFER
    UINT32  TextureHandle;      // GL_TEXTURE_2D
    UINT32  DepthHandle;
    UINT32  Width;
    UINT32  Height;
    BOOL    Initialized;
};


//--------------------------------------------------------------------------------------
// Name: struct SHatchShader15
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct SHatchShader15
{
    SHatchShader15();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ModelViewMatrixLoc;
    INT32   ModelViewProjMatrixLoc;
    INT32   NormalMatrixLoc;
    INT32   LightPositionId;
    INT32   AmbientId;
    INT32   HatchTextureId[2];
    INT32   DiffuseTextureId;

    // textures
    CFrmTexture* HatchTexture[2];
    CFrmTexture* DiffuseTexture;
};


//--------------------------------------------------------------------------------------
// Name: struct SNormalShader15
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct SNormalShader15
{
    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ModelViewMatrixLoc;
    INT32   ModelViewProjMatrixLoc;
    INT32   NormalMatrixLoc;
    INT32   LightPositionId;
};


//--------------------------------------------------------------------------------------
// Name: struct SCombineShader15
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct SCombineShader15
{
    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   NormalTextureId;
    INT32   HatchTextureId;
    INT32   StepSizeId;
    INT32   SlopeBiasId;
};


//--------------------------------------------------------------------------------------
// Name: class CSample15
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample15 : public CScene
{
    // Off-screen render targets used by the app
    enum
    {
        RT_NORMALS,
        RT_HATCH,
        MAX_RTS
    };


public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample15( const CHAR* strName );

private:
    BOOL InitShaders();

    VOID ClearOffscreenRTs();
    BOOL CreateOffscreenRT( UINT32 RenderTargetId, UINT32 Width, UINT32 Height );
    VOID FreeOffscreenRTs();
    VOID SetFramebuffer( UINT32 RenderTargetId );
    VOID FreeOffscreenRT( UINT32 RenderTargetId );

    VOID RenderSceneNormals();
    VOID RenderSceneHatch();
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

    SOffscreenRT15       m_OffscreenRT[MAX_RTS];

    // Shader related data
    INT32              m_hConstantModelViewProjMatrixLoc;
    INT32              m_hConstantColorLoc;
    INT32              m_hConstantDeformSize;
    FLOAT32            m_OutlineThickness;
    FLOAT32            m_SlopeBias;
    FLOAT32            m_AmbientLight;

    SHatchShader15       m_HatchShaderA;
    SHatchShader15       m_HatchShaderB;
    SNormalShader15      m_NormalShader;
    SCombineShader15     m_CombineShader;

    BOOL               m_AlternateStyle;
    BOOL               m_bShouldRotate;
    FLOAT              m_fRotateTime;
};

