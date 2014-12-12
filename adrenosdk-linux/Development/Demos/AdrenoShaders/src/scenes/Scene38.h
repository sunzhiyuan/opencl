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
// Name: struct MaterialShader38
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader38
{
    MaterialShader38();
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
    INT32   m_BumpTextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct DownsizeShader38
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct DownsizeShader38
{
    DownsizeShader38();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   StepSizeId;

	// members
    FRMVECTOR2 StepSize;
};


//--------------------------------------------------------------------------------------
// Name: struct BlurShader38
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlurShader38
{
    BlurShader38();
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
// Name: struct CombineShader38
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CombineShader38
{
    CombineShader38();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   SharpTextureId;
    INT32   BlurredTextureId;
    INT32   BloomIntensityId;
    INT32   ContrastId;
    INT32   SaturationId;
    INT32   ColorCorrectId;
    INT32   ColorAddId;

    // members
    FLOAT32     GaussSpread;
    FLOAT32     GaussRho;
    FLOAT32		BloomIntensity;
    FLOAT32		Contrast;
    FLOAT32		Saturation;
    FRMVECTOR3  ColorCorrect;
    FRMVECTOR3  ColorAdd;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject38
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject38
{
    SimpleObject38();
    VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
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
};


//--------------------------------------------------------------------------------------
// Name: class CSample38
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample38 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample38( const CHAR* strName );

private:    
    BOOL InitShaders();
    BOOL CheckFrameBufferStatus();
    VOID DrawObject( SimpleObject38* Object );
    VOID DrawScene();
    VOID GaussBlur( OffscreenRT4& TargetRT );
    VOID CreateBlurredImage();
    VOID DrawCombinedScene();
    VOID SetupGaussWeights();

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    BOOL                m_ShouldRotate;
    FRMVECTOR4          m_AmbientLight;
    SimpleObject38        m_Object;

    MaterialShader38      m_MaterialShader;
    DownsizeShader38      m_DownsizeShader;
    BlurShader38          m_BlurShader;
    CombineShader38       m_CombineShader;

    OffscreenRT4         m_SharpRT;
    OffscreenRT4         m_BlurredRT;
    OffscreenRT4         m_ScratchRT;
};

