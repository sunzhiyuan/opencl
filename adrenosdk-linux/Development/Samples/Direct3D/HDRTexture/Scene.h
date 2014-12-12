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
// Name: struct MaterialShader49
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader
{
	MaterialShader();
	VOID Destroy();

	// shader
	CFrmShaderProgramD3D  Shader;

	// shader variables
	INT32   TextureId;
	INT32   ExposureId;
};

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
	enum ColorSpace
	{
		COLORSPACE_RGB_PASSTHROUGH,
		COLORSPACE_CIE,
		COLORSPACE_HSV,
		COLORSPACE_L16LUV,
		MAX_COLORSPACES
	};

public:
	virtual BOOL Initialize();
	virtual BOOL Resize();
	virtual VOID Destroy();
	virtual VOID Update();
	virtual VOID Render();

	CSample( const CHAR* strName );
	
	VOID RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight );

private:    
	BOOL InitShaders();
	VOID DrawScene();
	
	CFrmPackedResourceD3D   resource;

    CFrmFontD3D             m_Font;
	CFrmTimer               m_Timer;
	CFrmUserInterfaceD3D    m_UserInterface;
	CFrmTexture*            m_pLogoTexture;

	CFrmTexture*            m_HDRTexture[2];
    
    CFrmVertexBuffer*       m_pQuadVertexBuffer;
    CFrmIndexBuffer*        m_pQuadIndexBuffer;
    
	MaterialShader          m_MaterialShader;
	FLOAT32                 m_Exposure;

    struct HDRTextureConstantBuffer
    {
        FLOAT32 Exposure;
        FRMVECTOR3 Padding; // Pad to multiple of 16-bytes
    };
    HDRTextureConstantBuffer m_HDRTextureConstantBufferData;
    CFrmConstantBuffer*      m_pHDRTextureConstantBuffer;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DefaultDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DisabledDepthStencilState;

};


#endif // SCENE_H
