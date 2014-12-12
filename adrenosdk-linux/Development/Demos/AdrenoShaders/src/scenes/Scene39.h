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
// Name: struct MaterialShader39
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader39
{
    MaterialShader39();
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
// Name: struct DownsizeShader39
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct DownsizeShader39
{
    DownsizeShader39();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   StepSizeId;

    FRMVECTOR2 StepSize;
};

//--------------------------------------------------------------------------------------
// Name: struct LuminanceShader39
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct LuminanceShader39
{
    LuminanceShader39();
    VOID Destroy();

    // shader
    UINT32  ShaderId;
    INT32   StepSizeId;

    // members
    FRMVECTOR2 StepSize;
    INT32   ColorTextureId;
};

//--------------------------------------------------------------------------------------
// Name: struct BrightPassShader39
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BrightPassShader39
{
    BrightPassShader39();
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
// Name: struct AdaptLuminanceShader39
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct AdaptLuminanceShader39
{
    AdaptLuminanceShader39();
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
// Name: struct BlurShader39
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlurShader39
{
    BlurShader39();
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
// Name: struct PreviewShader39
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PreviewShader39
{
    PreviewShader39();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
};

//--------------------------------------------------------------------------------------
// Name: struct CombineShader39
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CombineShader39
{
    CombineShader39();
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
    INT32   StreakTexture1Id;
    INT32   StreakTexture2Id;

    // members
	float MiddleGray;
	float White;
};


//--------------------------------------------------------------------------------------
// Name: struct LightStreakShader39
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct LightStreakShader39
{
    LightStreakShader39();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   WeightsId;
    INT32   StreakDirId;
    INT32   StreakLengthId;
    INT32   StepSizeId;

    // members
    FLOAT32 StreakDir;
    FLOAT32 StreakLength;
    FRMVECTOR2 StepSize;
};


//--------------------------------------------------------------------------------------
// Name: struct SimpleObject39
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject39
{
    SimpleObject39();
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
// Name: class CSample39
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample39 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample39( const CHAR* strName );

private:    
    BOOL InitShaders();
    BOOL CheckFrameBufferStatus();
    VOID DrawObject( SimpleObject39* Object );
    VOID DrawScene();
    VOID GaussBlur( OffscreenRT4& InputTargetRT, OffscreenRT4& BlurredTargetRT );
    VOID LightStreak( OffscreenRT4& OutputRT, OffscreenRT4& InputRT );
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
    SimpleObject39        m_Object;

    MaterialShader39      m_MaterialShader;
    DownsizeShader39      m_DownsizeShader;
    BlurShader39          m_BlurShader;
    CombineShader39       m_CombineShader;
    LuminanceShader39     m_LuminanceShader;
    LuminanceShader39     m_AvgLuminanceShader;
    AdaptLuminanceShader39 m_AdaptLuminanceShader;
    LuminanceShader39     m_CopyRTShader;
    BrightPassShader39    m_BrightPassShader;
    PreviewShader39       m_PreviewShader;
    LightStreakShader39   m_LightStreakShader;

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

