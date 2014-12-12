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
// Name: struct NormalShader44
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct NormalShader44
{
    NormalShader44();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ModelViewProjMatrixId;
    INT32   ModelViewMatrixId;
};


//--------------------------------------------------------------------------------------
// Name: struct AOShader44
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct AOShader44
{
    AOShader44();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   NormalTextureId;
    INT32   DepthTextureId;
    INT32   JitterTextureId;
    INT32   ProjInvId;
    INT32   StepSizeId;
    INT32   AOIntensityId;
    INT32   AOEdgeId;
    INT32   JitterAmountId;

    // members
    FRMVECTOR2      StepSize;
    FRMMATRIX4X4    ProjInv;
};


//--------------------------------------------------------------------------------------
// Name: struct BlurShader44
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlurShader44
{
    BlurShader44();
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
// Name: struct CombineShader44
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CombineShader44
{
    CombineShader44();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   AOTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct PreviewShader44
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PreviewShader44
{
    PreviewShader44();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject44
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject44
{
    SimpleObject44();
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
// Name: class CSample44
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample44 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample44( const CHAR* strName );

private:    
    BOOL InitShaders();
    BOOL CheckFrameBufferStatus();
    VOID SetupGaussWeights();
    VOID GaussBlur( OffscreenRT4& TargetRT );
    VOID PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle );
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
    SimpleObject44            m_Object;
    FLOAT32                 m_ViewPositionScalar;
    FLOAT32                 m_GaussSpread;
    FLOAT32                 m_GaussRho;
    FLOAT32                 m_AOEdgeFinder;
    FLOAT32                 m_AOIntensity;
    FLOAT32                 m_JitterAmount;
    CFrmTexture*            m_JitterTexture;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    OffscreenRT4         m_NormalRT;
    OffscreenRT4         m_AORT;
    OffscreenRT4         m_ScratchRT;

    NormalShader44        m_NormalShader;
    AOShader44            m_AOShader;
    BlurShader44          m_BlurShader;
    CombineShader44       m_CombineShader;
    PreviewShader44       m_PreviewShader;
};
