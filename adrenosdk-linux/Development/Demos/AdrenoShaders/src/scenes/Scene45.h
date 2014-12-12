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
// Name: struct MaterialShader45
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader45
{
    MaterialShader45();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   m_ModelMatrixId;
    INT32   m_ModelViewProjMatrixId;
    INT32   m_LightPositionId;
    INT32   m_EyePositionId;

    INT32   m_AmbientColorId;
    INT32   m_DiffuseTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct NormalShader45
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct NormalShader45
{
    NormalShader45();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ModelViewProjMatrixId;
    INT32   ModelViewMatrixId;
};


//--------------------------------------------------------------------------------------
// Name: struct BounceShader45
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BounceShader45
{
    BounceShader45();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   NormalTextureId;
    INT32   DepthTextureId;
    INT32   JitterTextureId;
    INT32   ColorTextureId;
    INT32   ProjInvId;
    INT32   StepSizeId;
    INT32   AOIntensityId;
    INT32   AOEdgeId;
    INT32   JitterAmountId;
    INT32   BounceIntensityId;

    // members
    FRMVECTOR2      StepSize;
    FRMMATRIX4X4    ProjInv;
};


//--------------------------------------------------------------------------------------
// Name: struct BlurShader45
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlurShader45
{
    BlurShader45();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   GaussWeightId;
    INT32   GaussInvSumId;
    INT32   StepSizeId;

    // members
    FLOAT32 GaussWeight[4];
    FLOAT32 GaussInvSum;
    FRMVECTOR2 StepSize;
};


//--------------------------------------------------------------------------------------
// Name: struct CombineShader45
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CombineShader45
{
    CombineShader45();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   AOTextureId;
    INT32   ColorTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct PreviewShader45
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PreviewShader45
{
    PreviewShader45();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject45
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject45
{
    SimpleObject45();
    VOID Update( FLOAT ElapsedTime, FRMMATRIX4X4& ViewMat, FRMMATRIX4X4& ProjMat, BOOL ShouldRotate = TRUE );
    VOID Destroy();

    FLOAT32         ModelScale;
    FRMVECTOR3      Position;
    FLOAT32         RotateTime;
    CFrmMesh*       Drawable;
    CFrmTexture*    DiffuseTexture;
    CFrmTexture*    BumpTexture;
    FRMVECTOR4      DiffuseColor;

    // constructed in Update() for rendering
    FRMMATRIX4X4    ModelMatrix;
    FRMMATRIX4X4    ModelView;
    FRMMATRIX4X4    ModelViewProj;
};


//--------------------------------------------------------------------------------------
// Name: class CSample45
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample45 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample45( const CHAR* strName );

private:    
    BOOL InitShaders();
    BOOL CheckFrameBufferStatus();
    VOID SetupGaussWeights();
    VOID GaussBlur( OffscreenRT4& TargetRT );
    VOID DrawObject( SimpleObject45* Object );
    VOID PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle );
    VOID DrawColor();
    VOID DrawNormalsAndDepth();
    VOID DrawAO();
    VOID DrawCombinedScene();

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;
    BOOL                    m_ShouldRotate;
    BOOL                    m_PreviewRT;
    SimpleObject45            m_Object;
    FLOAT32                 m_ViewPositionScalar;
    FLOAT32                 m_GaussSpread;
    FLOAT32                 m_GaussRho;
    FLOAT32                 m_AOEdgeFinder;
    FLOAT32                 m_AOIntensity;
    FLOAT32                 m_JitterAmount;
    FLOAT32                 m_BounceIntensity;
    CFrmTexture*            m_JitterTexture;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    OffscreenRT4         m_ColorRT;
    OffscreenRT4         m_NormalRT;
    OffscreenRT4         m_AORT;
    OffscreenRT4         m_ScratchRT;

    MaterialShader45      m_MaterialShader;
    NormalShader45        m_NormalShader;
    BounceShader45        m_BounceShader;
    BlurShader45          m_BlurShader;
    CombineShader45       m_CombineShader;
    PreviewShader45       m_PreviewShader;
};
