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

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture0;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTexture0RenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture0ShaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pTexture0Sampler;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthTexture;    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
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
	CFrmShaderProgramD3D  Shader;

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
// Name: struct DownsizeShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct DownsizeShader
{
	DownsizeShader();
	VOID Destroy();

	// shader
	CFrmShaderProgramD3D  Shader;	

    struct DownsizeShaderConstantBuffer
    {
        FRMVECTOR2 g_StepSize;
        FRMVECTOR2 Padding; // Pad to multiple of 16-bytes
    };

    DownsizeShaderConstantBuffer m_DownsizeShaderConstantBufferData;
    CFrmConstantBuffer*          m_pDownsizeShaderConstantBuffer;

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
	CFrmShaderProgramD3D  Shader;	

    struct LuminanceShaderConstantBuffer
    {
        FRMVECTOR2 g_StepSize;
        FRMVECTOR2 Padding; // Pad to multiple of 16-bytes
    };

    LuminanceShaderConstantBuffer   m_LuminanceShaderConstantBufferData;
    CFrmConstantBuffer*             m_pLuminanceShaderConstantBuffer;
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
	CFrmShaderProgramD3D Shader;
    struct BrightPassConstantBuffer
    {
        FLOAT32 MiddleGray;
        FLOAT32 BrightPassWhite;
        FLOAT32 BrightPassThreshold;
        FLOAT32 BrightPassOffset;
    };
    BrightPassConstantBuffer m_BrightPassConstantBufferData;
    CFrmConstantBuffer*      m_pBrightPassConstantBuffer;

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
    CFrmShaderProgramD3D Shader;

    struct AdadptLuminanceConstantBuffer
    {
        FLOAT32 ElapsedTime;
        FLOAT32 Lambda;
        FRMVECTOR2 Padding; // Pad to multiple of 16-bytes
    };

    AdadptLuminanceConstantBuffer m_AdaptLuminanceConstantBufferData;
    CFrmConstantBuffer*           m_pAdaptLuminanceConstantBuffer;
	
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
// Name: struct PreviewShader36
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct PreviewShader
{
	PreviewShader();
	VOID Destroy();

	// shader
	CFrmShaderProgramD3D  Shader;	
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
	CFrmShaderProgramD3D Shader;
    struct CombineShaderConstantBuffer
    {
        FLOAT32 MiddleGray;
        FLOAT32 White;
        FRMVECTOR2 Padding; // Pad to multiple of 16-bytes
    };
    CombineShaderConstantBuffer m_CombineShaderConstantBufferData;
    CFrmConstantBuffer*         m_pCombineShaderConstantBuffer;

	// members
    FLOAT32 GaussSpread;
    FLOAT32 GaussRho;
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


    BOOL CreateFBO( UINT32 nWidth, UINT32 nHeight, DXGI_FORMAT nFormat, D3D11_FILTER nFilter, BOOL bDepth, FrameBufferObject** ppFBO );	
	VOID DestroyFBO( FrameBufferObject* pFBO );
	VOID BeginFBO( FrameBufferObject* pFBO );
	VOID EndFBO( FrameBufferObject* pFBO );
	
	VOID RenderScreenAlignedQuad();
	VOID RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight );
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
	VOID PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, FrameBufferObject* pTexture );

	VOID SetupGaussWeights();
	
	BOOL                    m_Preview;

	CFrmFontD3D             m_Font;
	CFrmTimer               m_Timer;
	CFrmUserInterfaceD3D    m_UserInterface;
	CFrmMesh                m_Mesh;
	FRMVECTOR3              m_CameraPos;
	FRMVECTOR3              m_LightPos;
    FLOAT32                 m_fDownsizeRatio;

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
	CFrmPackedResourceD3D   g_Resource;
	CFrmPackedResourceD3D   g_RoomResources;

    CFrmVertexBuffer*   m_pQuadVertexBuffer;
    CFrmVertexBuffer*   m_pDynamicQuadVertexBuffer;
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
