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
	CFrmShaderProgramD3D Shader;

	// Constant data
    struct MaterialConstantBuffer
    {
        FRMMATRIX4X4 MatModelViewProj;
        FRMMATRIX4X4 MatModel;
        FRMVECTOR4   LightPos;
        FRMVECTOR4   EyePos;
        FRMVECTOR4   AmbientColor;
    };

    MaterialConstantBuffer m_MaterialConstantBufferData;
    CFrmConstantBuffer*    m_pMaterialConstantBuffer;

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
	CFrmShaderProgramD3D Shader;	

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
    CFrmShaderProgramD3D Shader;
	
	// Constant data    
    struct BlurConstantBuffer
    {
        FLOAT32    GaussWeight[4];
        FRMVECTOR2 StepSize;    
        FLOAT32    GaussInvSum;
        FLOAT32    Padding; // Must be 16-byte aligned
    };

    FRMVECTOR2 StepSize;    
	
    BlurConstantBuffer  m_BlurConstantBufferData;
    CFrmConstantBuffer* m_pBlurConstantBuffer;
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
	CFrmShaderProgramD3D Shader;

	// shader variables	
    struct CombineConstantBuffer
    {
        FRMVECTOR2 NearQ;
        FRMVECTOR2 FocalDistRange;
        BOOL ThermalColors;
        FRMVECTOR3 Padding; // Must be 16-byte aligned
    };

    CombineConstantBuffer m_CombineConstantBufferData;
    CFrmConstantBuffer*   m_pCombineConstantBuffer;

	// members
	FRMVECTOR3  NearFarQ;
    FRMVECTOR2 FocalDistRange;        
	FLOAT32     GaussSpread;
	FLOAT32     GaussRho;	
};

// Wrapper for FBO objects and properties
struct FrameBufferObject
{
	UINT32 m_nWidth;
	UINT32 m_nHeight;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture0;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTexture0RenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture0ShaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pTexture0Sampler;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTexture1RenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture1ShaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pTexture1Sampler;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthTexture;    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
};

//--------------------------------------------------------------------------------------
// Name: struct SimpleObject27
// Desc: A simple drawable object
//--------------------------------------------------------------------------------------
struct SimpleObject
{
	SimpleObject();
	VOID Update( FLOAT ElapsedTime, BOOL ShouldRotate = TRUE );
	VOID Destroy();

	FRMVECTOR3      Position;
	FLOAT32         RotateTime;
	CFrmMesh*       Drawable;
	CFrmTexture*    DiffuseTexture;
	CFrmTexture*    BumpTexture;
	FRMVECTOR4      DiffuseColor;

	// constructed in Update() for rendering
	FRMMATRIX4X4    ModelMatrix;
	
	float ModelScale;
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

	BOOL CreateFBO( UINT32 nWidth, UINT32 nHeight, BOOL bMRT, BOOL bDepth, FrameBufferObject** ppFBO );
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
    
	VOID GaussBlur( FrameBufferObject* TargetRT );
	VOID CreateBlurredImage();
	VOID DrawCombinedScene();

    

    CFrmFontD3D            m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceD3D   m_UserInterface;

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

		
	CFrmPackedResourceD3D g_Resource;
	CFrmPackedResourceD3D g_RoomResources;
	
	SimpleObject        m_Object;
	BOOL                m_ShouldRotate;
	FRMVECTOR4          m_AmbientLight;
	
	MaterialShader      m_MaterialShader;
	DownsizeShader      m_DownsizeShader;
	BlurShader          m_BlurShader;
	CombineShader       m_CombineShader;

    CFrmVertexBuffer*   m_pQuadVertexBuffer;
    CFrmIndexBuffer*    m_pQuadIndexBuffer;
    
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_DefaultRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_CullDisabledRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DefaultDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DisabledDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11BlendState>         m_DisabledBlendState;
    Microsoft::WRL::ComPtr<ID3D11BlendState>         m_EnabledBlendState;
    
};



#endif // SCENE_H
