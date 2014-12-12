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


//--------------------------------------------------------------------------------------
// We are going to blend between the sharp and out of focus FBOs based upon
// the distance an object is from the viewer.
//--------------------------------------------------------------------------------------
// enum { SHARP_FBO=0, OUT_OF_FOCUS_FBO, NUM_FBOS };

//--------------------------------------------------------------------------------------
// Name: struct MaterialShader
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
// Name: struct DownsizeShader
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
};


//--------------------------------------------------------------------------------------
// Name: struct BlurShader
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
// Name: struct CombineShader
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
	INT32   BlurredTextureId;
	INT32   DepthTextureId;
	INT32   NearQId;
	INT32   FocalDistRangeId;
	INT32   ThermalColorsId;

	// members
	FRMVECTOR3  NearFarQ;
	FRMVECTOR2  FocalDistRange;
	FLOAT32     GaussSpread;
	FLOAT32     GaussRho;
	BOOL        ThermalColors;
};

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
// Name: struct SimpleObject27
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject
{
	SimpleObject();
	VOID Update( FLOAT32 ElapsedTime, BOOL ShouldRotate = TRUE );
	VOID Destroy();

	FRMVECTOR3      Position;
	FLOAT32         RotateTime;
	CFrmMesh*       Drawable;
	CFrmTexture*    DiffuseTexture;
	CFrmTexture*    BumpTexture;
	FRMVECTOR4      DiffuseColor;

	// constructed in Update() for rendering
	FRMMATRIX4X4    ModelMatrix;
	
	FLOAT32 ModelScale;
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
	
	VOID DrawScene();
	VOID DrawObject( SimpleObject* Object );
	VOID SetupGaussWeights();
    
    VOID RenderScreenAlignedQuad();

    VOID RenderSharpSceneAndBlurinessToFBO();
    VOID RenderBlurredSceneToFBO();
    VOID RenderBlendedScene();
    
	VOID SetTexture( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot );
	VOID GaussBlur( FrameBufferObject* TargetRT );
	VOID CreateBlurredImage();
	VOID DrawCombinedScene();

    

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    FRMMATRIX4X4       m_matModel;
    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;

    CFrmMesh           m_Mesh;
	CFrmMesh           m_RoomMesh;

	FRMVECTOR3         m_CameraPos;
	FRMVECTOR3         m_LightPos;

    CFrmTexture*       m_pLogoTexture;

    INT32              m_hTextureModelViewMatrixLoc;
    INT32              m_hTextureModelViewProjMatrixLoc;
    CFrmTexture*       m_pTexture;

    UINT32             m_hGaussianShader;
    INT32              m_hGaussianStepSizeLoc;

    UINT32             m_hDepthOfFieldShader;
    INT32              m_hDepthOfFieldSharpImageLoc;
    INT32              m_hDepthOfFieldBlurredImageLoc;
    INT32              m_hDepthOfFieldRangeLoc;
    INT32              m_hDepthOfFieldFocusLoc;

    INT32              m_nOutOfFocusFBOWidth;
    INT32              m_nOutOfFocusFBOHeight;
    
	FrameBufferObject* g_pSharpFBO;
	FrameBufferObject* g_pBlurFBO;
	FrameBufferObject* g_pScratchFBO;

		
	CFrmPackedResourceGLES g_Resource;
	CFrmPackedResourceGLES g_RoomResources;
	
	SimpleObject        m_Object;
	BOOL                m_ShouldRotate;
	FRMVECTOR4          m_AmbientLight;
	
	MaterialShader      m_MaterialShader;
	DownsizeShader      m_DownsizeShader;
	BlurShader          m_BlurShader;
	CombineShader       m_CombineShader;
};



#endif // SCENE_H
