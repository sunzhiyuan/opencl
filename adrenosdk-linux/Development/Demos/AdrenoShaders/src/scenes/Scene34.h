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
// Name: struct MaterialShader34
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader34
{
    MaterialShader34();
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
// Name: struct DownsizeShader34
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct DownsizeShader34
{
    DownsizeShader34();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
    INT32   StepSizeId;

    FRMVECTOR2 StepSize;
};

//--------------------------------------------------------------------------------------
// Name: struct LuminanceShader34
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct LuminanceShader34
{
    LuminanceShader34();
    VOID Destroy();

    // shader
    UINT32  ShaderId;
    INT32   StepSizeId;

    // members
    FRMVECTOR2 StepSize;
    INT32   ColorTextureId;
};

//--------------------------------------------------------------------------------------
// Name: struct BrightPassShader34
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BrightPassShader34
{
    BrightPassShader34();
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
// Name: struct AdaptLuminanceShader34
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct AdaptLuminanceShader34
{
    AdaptLuminanceShader34();
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
// Name: struct BlurShader34
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlurShader34
{
    BlurShader34();
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
// Name: struct PreviewShader34
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PreviewShader34
{
    PreviewShader34();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   ColorTextureId;
};

//--------------------------------------------------------------------------------------
// Name: struct CombineShader34
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CombineShader34
{
    CombineShader34();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   SharpTextureId;
    INT32   BlurredTextureId;
    INT32   DepthTextureId;
    INT32   NearQId;

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
// Name: struct SimpleObject34
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject34
{
    SimpleObject34();
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
// Name: class CSample34
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample34 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample34( const CHAR* strName );

private:    
    BOOL InitShaders();
    BOOL CheckFrameBufferStatus();
    VOID DrawObject( SimpleObject34* Object );
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
    SimpleObject34        m_Object;

    MaterialShader34      m_MaterialShader;
    DownsizeShader34      m_DownsizeShader;
    BlurShader34          m_BlurShader;
    CombineShader34       m_CombineShader;
    LuminanceShader34     m_LuminanceShader;
    LuminanceShader34     m_AvgLuminanceShader;
    AdaptLuminanceShader34 m_AdaptLuminanceShader;
    LuminanceShader34     m_CopyRTShader;
    BrightPassShader34    m_BrightPassShader;

    PreviewShader34       m_PreviewShader;

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

