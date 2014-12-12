//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H

// Wrapper for FBO objects and properties
struct FrameBufferObject
{
	UINT32 m_nWidth;
	UINT32 m_nHeight;

	UINT32 m_hFrameBuffer;
	UINT32 m_hDepthTexture;
	UINT32 m_hTexture;
};


//--------------------------------------------------------------------------------------
// Name: struct MaterialShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader
{
	MaterialShader();
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
struct DownsizeShader
{
	DownsizeShader();
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
struct LuminanceShader
{
	LuminanceShader();
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
struct BrightPassShader
{
	BrightPassShader();
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
struct AdaptLuminanceShader
{
	AdaptLuminanceShader();
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
struct BlurShader
{
	BlurShader();
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
struct PreviewShader
{
	PreviewShader();
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
struct CombineShader
{
	CombineShader();
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
struct SimpleObject
{
	SimpleObject();
	VOID Update( FLOAT32 ElapsedTime, BOOL ShouldRotate = TRUE );
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
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:
    CSample( const CHAR* strName );

    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

private:
    BOOL InitShaders();

    BOOL CheckFrameBufferStatus();
	
	BOOL CreateFBO( UINT32 nWidth, UINT32 nHeight, UINT32 nFormat, UINT32 nType, UINT32 nInternalFormat, FrameBufferObject** ppFBO );
	VOID DestroyFBO( FrameBufferObject* pFBO );
	VOID BeginFBO( FrameBufferObject* pFBO );
	VOID EndFBO( FrameBufferObject* pFBO );
	
	VOID RenderScreenAlignedQuad();
	VOID RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight );
	VOID SetTexture( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot );

	VOID DrawScene();
	VOID DrawObject( SimpleObject* Object );
	
	VOID GaussBlur( FrameBufferObject* InputTargetRT, FrameBufferObject* BlurredTargetRT );
	VOID Generate64x64Lum( FrameBufferObject* TargetRT );
	VOID Generate16x16Lum( FrameBufferObject* TargetRT );
	VOID Generate4x4Lum( FrameBufferObject* TargetRT );
	VOID Generate1x1Lum( FrameBufferObject* TargetRT );
	VOID GenerateAdaptLum( FrameBufferObject* CurrLumRT, FrameBufferObject* PrevLumRT );
	VOID Copy1x1Lum( FrameBufferObject* CopyToLumRT, FrameBufferObject* CopyFromLumRT );
	VOID BrightPass( FrameBufferObject* CopyToLumRT, FrameBufferObject* ScratchRT, FrameBufferObject* currLumRT );

	VOID CreateBlurredImage();
	VOID DrawCombinedScene();
	VOID PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle );

	VOID SetupGaussWeights();
	
	BOOL                    m_Preview;

	CFrmFontGLES            m_Font;
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
	SimpleObject        m_Object;

	MaterialShader      m_MaterialShader;
	DownsizeShader      m_DownsizeShader;
	BlurShader          m_BlurShader;
	CombineShader       m_CombineShader;
	LuminanceShader     m_LuminanceShader;
	LuminanceShader     m_AvgLuminanceShader;
	AdaptLuminanceShader m_AdaptLuminanceShader;
	LuminanceShader     m_CopyRTShader;
	BrightPassShader    m_BrightPassShader;

	PreviewShader       m_PreviewShader;

	FrameBufferObject*         m_SharpRT;
	FrameBufferObject*         m_BlurredRT;
	FrameBufferObject*         m_ScratchRT;
	FrameBufferObject*         m_64x64RT;
	FrameBufferObject*         m_16x16RT;
	FrameBufferObject*         m_4x4RT;
	FrameBufferObject*         m_1x1RT;
	FrameBufferObject*         m_LastAverageLumRT;
	FrameBufferObject*         m_PreviousAverageLumRT;
	FrameBufferObject*         m_BrightPassRT;
	FrameBufferObject*         m_QuarterRT;
	
	CFrmTexture*			m_pLogoTexture;

	CFrmMesh           m_RoomMesh;
	CFrmPackedResourceGLES g_Resource;
	CFrmPackedResourceGLES g_RoomResources;
};



#endif // SCENE_H
