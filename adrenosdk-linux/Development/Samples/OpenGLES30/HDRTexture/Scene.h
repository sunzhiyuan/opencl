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
	UINT32  ShaderId;

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
	VOID SetTexture( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot );

private:    
	BOOL InitShaders();
	VOID DrawScene();
	
	CFrmPackedResourceGLES resource;

	CFrmFontGLES            m_Font;
	CFrmTimer               m_Timer;
	CFrmUserInterfaceGLES   m_UserInterface;
	CFrmTexture*            m_pLogoTexture;
	GLuint                  m_HDRTexture[2];

	CFrmTexture*            m_OldHDRTexture[2];

	MaterialShader          m_MaterialShader9995;
	MaterialShader          m_MaterialShaderOld;
	FLOAT32                 m_Exposure;

	bool					m_9995Shader;
};


#endif // SCENE_H
