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
// Name: struct MaterialShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader36
{
    MaterialShader36();
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
// Name: struct DownsizeShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct DownsizeShader36
{
    DownsizeShader36();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   StepSizeId;

    FRMVECTOR2 StepSize;
};

//--------------------------------------------------------------------------------------
// Name: struct DownsizeShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct LuminanceShader36
{
    LuminanceShader36();
    VOID Destroy();

    // shader
    UINT32  ShaderId;
    INT32   StepSizeId;

    // members
    FRMVECTOR2 StepSize;
    INT32   ColorTextureId;
};

//--------------------------------------------------------------------------------------
// Name: struct DownsizeShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BrightPassShader36
{
    BrightPassShader36();
    VOID Destroy();


    // shader
    UINT32  ShaderId;
    UINT32  MiddleGrayId;
    UINT32  BrightPassWhiteId;
    UINT32  BrightPassThresholdId;
    UINT32  BrightPassOffsetId;
    INT32   ScratchTextureId;
    INT32   CurrLumTextureId;

    // members
	float MiddleGray;
	float BrightPassWhite;
	float BrightPassThreshold;
	float BrightPassOffset;
};

//--------------------------------------------------------------------------------------
// Name: struct DownsizeShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct AdaptLuminanceShader36
{
    AdaptLuminanceShader36();
    VOID Destroy();

    // shader
    UINT32  ShaderId;
    INT32   StepSizeId;
    INT32   CurrLumTextureId;
    INT32   PrevLumTextureId;
    INT32   ElapsedTimeId;
    INT32   LambdaId;

	// members
	float ElapsedTime;
	float Lambda;
};

//--------------------------------------------------------------------------------------
// Name: struct BlurShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlurShader36
{
    BlurShader36();
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
// Name: struct PreviewShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PreviewShader36
{
    PreviewShader36();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
};

//--------------------------------------------------------------------------------------
// Name: struct CombineShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CombineShader36
{
    CombineShader36();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   SharpTextureId;

    // members
    FLOAT32     GaussSpread;
    FLOAT32     GaussRho;

    UINT32  MiddleGrayId;
    UINT32  WhiteId;
    INT32   CurrLumTextureId;
    INT32   BloomTextureId;

    // members
	float MiddleGray;
	float White;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject36
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject36
{
    SimpleObject36();
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
// Name: class CSample36
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample36 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample36( const CHAR* strName );

private:    
    BOOL InitShaders();
    BOOL CheckFrameBufferStatus();
    VOID DrawObject( SimpleObject36* Object );
    VOID DrawScene();
    VOID GaussBlur( OffscreenRT4& InputTargetRT, OffscreenRT4& BlurredTargetRT );
	VOID Generate64x64Lum( OffscreenRT4& TargetRT );
	VOID Generate16x16Lum( OffscreenRT4& TargetRT );
	VOID Generate4x4Lum( OffscreenRT4& TargetRT );
	VOID Generate1x1Lum( OffscreenRT4& TargetRT );
	VOID GenerateAdaptLum( OffscreenRT4& CurrLumRT, OffscreenRT4& PrevLumRT );
	VOID Copy1x1Lum( OffscreenRT4& CopyToLumRT, OffscreenRT4& CopyFromLumRT );
	VOID BrightPass( OffscreenRT4& CopyToLumRT, OffscreenRT4& ScratchRT, OffscreenRT4& currLumRT );

    VOID CreateBlurredImage();
    VOID DrawCombinedScene();
	VOID PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle );

    VOID SetupGaussWeights();

    BOOL                    m_Preview;

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
    SimpleObject36        m_Object;

    MaterialShader36      m_MaterialShader;
    DownsizeShader36      m_DownsizeShader;
    BlurShader36          m_BlurShader;
    CombineShader36       m_CombineShader;
    LuminanceShader36     m_LuminanceShader;
    LuminanceShader36     m_AvgLuminanceShader;
    AdaptLuminanceShader36 m_AdaptLuminanceShader;
    LuminanceShader36     m_CopyRTShader;
    BrightPassShader36    m_BrightPassShader;

    PreviewShader36       m_PreviewShader;

    OffscreenRT4         m_SharpRT;
    OffscreenRT4         m_BlurredRT;
    OffscreenRT4         m_ScratchRT;
    OffscreenRT4         m_64x64RT;
    OffscreenRT4         m_16x16RT;
    OffscreenRT4         m_4x4RT;
    OffscreenRT4         m_1x1RT;
    OffscreenRT4         m_LastAverageLumRT;
    OffscreenRT4         m_PreviousAverageLumRT;
    OffscreenRT4         m_BrightPassRT;
    OffscreenRT4         m_QuarterRT;
};
