//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#pragma once

// The Parent class
#include "../scene.h"

//--------------------------------------------------------------------------------------
// Name: struct MaterialShader41
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader41
{
    MaterialShader41();
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
// Name: struct DownsizeShader41
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct DownsizeShader41
{
    DownsizeShader41();
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
// Name: struct BlurShader41
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct BlurShader41
{
    BlurShader41();
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
// Name: struct CombineShader41
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CombineShader41
{
    CombineShader41();
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
    INT32   Rand3DTextureId;
    INT32   StaticFrequencySlotId;
    INT32   StaticTimeSlotId;


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
// Name: struct SimpleObject41
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject41
{
    SimpleObject41();
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
// Name: struct OffscreenRT41
// Desc: Stores information and handles for off-screen render targets
//--------------------------------------------------------------------------------------
struct OffscreenRT41
{
    OffscreenRT41();
    ~OffscreenRT41();
    BOOL CreateRT( UINT32 DesiredWidth, UINT32 DesiredHeight, BOOL CreateDepthBuffer );
    VOID FreeRT();
    VOID SetFramebuffer();

    // frame buffer object
    UINT32  BufferHandle;

    // re-using buffers as samplers
    UINT32  TextureHandle;
    UINT32  DepthTextureHandle;

    UINT32  Width;
    UINT32  Height;
};


//--------------------------------------------------------------------------------------
// Name: class CSample41
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample41 : public CScene
{
    
public:
    virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample41( const CHAR* strName );

private:    
    BOOL InitShaders();
    BOOL CheckFrameBufferStatus();
    VOID DrawObject( SimpleObject41* Object );
    VOID DrawScene();
    VOID GaussBlur( OffscreenRT41& TargetRT );
    VOID CreateBlurredImage();
    VOID DrawCombinedScene();
    VOID RenderScreenAlignedQuad41();
    VOID SetupGaussWeights();
    BOOL CreateRandomTexture3D( UINT32 nDim );


    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
	CFrmTexture3D*			m_p3DRandomTexture;
    CFrmMesh                m_Mesh;
    FRMVECTOR3              m_CameraPos;
    FRMVECTOR3              m_LightPos;

    FRMMATRIX4X4        m_matView;
    FRMMATRIX4X4        m_matProj;
    FRMMATRIX4X4        m_matViewProj;

    BOOL                m_ShouldRotate;
    FRMVECTOR4          m_AmbientLight;
    SimpleObject41        m_Object;

    MaterialShader41      m_MaterialShader;
    DownsizeShader41      m_DownsizeShader;
    BlurShader41          m_BlurShader;
    CombineShader41       m_CombineShader;

    OffscreenRT41         m_SharpRT;
    OffscreenRT41         m_BlurredRT;
    OffscreenRT41         m_ScratchRT;

    FLOAT32             m_fElapsedTime;
};
