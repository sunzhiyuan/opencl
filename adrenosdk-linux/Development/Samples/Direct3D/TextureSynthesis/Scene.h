//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: Texture Synthesis
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H

#include "Mover.h"
       
// used for batching texture tiles to be updated
typedef struct
{
	float x, y, z;
	float coordHMBias1, coordHMBias2,
		  coordHMScale1, coordHMScale2,
		  coordTileBias1, coordTileBias2,
		  coordTileScale1, coordTileScale2,
		  scale1, scale2;
}TILEVERTEX;


// maximum depth of surface layer tree!
#define MAX_LAYER_DEPTH		4

// max render target size
static	UINT16				RT_SIZE = TREE_MAX_RESOLUTION;


// Globals that will likely need to be moved
float g_sliderGamma = 1.8f, g_sliderExposure = 1.0f;
int g_qualitySelect = 5;
int g_surfaceSelect = 0;

const char *SURFACE_TYPE_STR[2] = { "Rock, Sand and Grass", "Snow Covered Terrain" };
const char *CURRENT_SURFACE_TYPE_STR;

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample( const CHAR* strName );

private:
    BOOL InitShaders();

    // Function calls related to terrain texture caching
    BOOL InitHeightMapAndTextures();
    BOOL InitIntermediateFBOs();
    
    // Calls for Texture Synthesis Processing and Rendering
    VOID AddQuad( TERRAINTILE *tile, D3D11_RECT *pBoundingRect );
    VOID AddTextureLayerRoot( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface, 
								RENDER_TEXTURE *dstColor, 
								RENDER_TEXTURE *dstHeightCoverage );
    VOID AddTextureLayer( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface, 
							RENDER_TEXTURE *srcColor, 
							RENDER_TEXTURE *srcHeight, 
							RENDER_TEXTURE *srcCoverage,
							RENDER_TEXTURE *dstColor,
							RENDER_TEXTURE *dstHeightCoverage );
	
	VOID ComputeLightingLayer( ATLAS *atlas, RENDER_TEXTURE *srcColor, RENDER_TEXTURE *srcHeight, RENDER_TEXTURE *dstColor );
    VOID ComputeTextures();
    VOID ComputeTexturesRecursive( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface,
									INT32 depth=0, RENDER_TEXTURE *currentCoverage=NULL, INT32 additionalFlags = 0);

    CFrmFontD3D            m_Font;
    CFrmUserInterfaceD3D   m_UserInterface;
    CFrmTimer              m_Timer;

    // Declare shader variables
    struct OverlayConstantBuffer
    {
        FRMVECTOR4 vScreenSize;
    };

    CFrmShaderProgramD3D   m_OverlayShader;    
    OverlayConstantBuffer  m_OverlayConstantBufferData;
    CFrmConstantBuffer*    m_pOverlayConstantBuffer;
    
    CFrmRenderTextureToScreenD3D m_RenderTextureToScreen;

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;

    // Declare transformation related variables
	FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX4X4       m_matCameraPersp;
    FRMMATRIX4X4       m_matLightPersp;

    FRMMATRIX4X4       m_matCameraView;
    FRMMATRIX4X4       m_matCameraFloorModelView;
    FRMMATRIX4X4       m_matCameraFloorModelViewProj;
    FRMMATRIX3X3       m_matCameraFloorNormal;

    FRMMATRIX4X4       m_matCameraMeshModelView;
    FRMMATRIX4X4       m_matCameraMeshModelViewProj;
    FRMMATRIX3X3       m_matCameraMeshNormal;

    FRMMATRIX4X4       m_matFloorModel;

    FRMVECTOR3         m_vLightLookAt;
    FRMVECTOR3         m_vLightUp;

    FRMMATRIX4X4       m_matLightModelViewProj;
    FRMMATRIX4X4       m_matLightFloorModelViewProj;
    FRMMATRIX4X4       m_matLightMeshModelViewProj;

    FRMMATRIX4X4       m_matFloorShadowMatrix;
    FRMMATRIX4X4       m_matMeshShadowMatrix;

    FRMMATRIX4X4       m_matFloorScreenCoordMatrix;
    FRMMATRIX4X4       m_matMeshScreenCoordMatrix;

    FRMMATRIX4X4       m_matScaleAndBias;
    
    // Declare camera position variables
    FRMVECTOR4		m_camPos;
    FRMVECTOR4		m_camLookAt;

    // Declare user controlled variables
    BOOL            m_bShowRT;
    
    // Variables for procedural terrain texture caching
    INT32			m_flushTextures;
    
    UINT16*			m_heightMap16;		// resampled height map to 513
    UINT16*			m_heightMap16Full;	// full res height map
    
    CFrmTexture*    m_pHeightMapTexture16; // texture storing full res height map
    CFrmTexture*    m_pBicubicWeight03;    // bi-cubic interpolation weights
    CFrmTexture*    m_pBicubicWeight47;
    CFrmTexture*    m_pBicubicWeight8B;
    CFrmTexture*    m_pBicubicWeightCF;
    
    CFrmTexture*	m_pTexNoise;			// texture for generating noise
    CFrmTexture*	m_pTexDisplacement;		// texture for generating height-displacements
    CFrmTexture*	m_pTexShadow;			// shadow and occlusion textures
    CFrmTexture*	m_pTexOcclusion;

    CFrmVertexBuffer* m_pVertexBuffer;		// For caching texture tiles
    CFrmIndexBuffer*  m_pIndexBuffer;
    
    // intermediate render targets/textures for the procedural textures
	RENDER_TEXTURE  m_TexIntermediateColor, 
					m_TexIntermediateColor2,
					m_TexIntermediateNormal, 
					m_TexIntermediateHeightCoverage;
	RENDER_TEXTURE  m_TexIntermediateHeightCoverageLayer[ MAX_LAYER_DEPTH ];
	
	RENDER_TEXTURE	m_TexDebug;
	
    // Variables for batching texture quads to be updated
    INT32			m_nTempQuads;
	TILEVERTEX		*m_quadList;
    
    // Shaders for procedural terrain texture caching
    CFrmShaderProgramD3D        m_RenderTerrainShader;
    RenderTerrainConstantBuffer m_RenderTerrainConstantBufferData;
    CFrmConstantBuffer*         m_pRenderTerrainConstantBuffer;
    
    struct ResampleHeightConstantBuffer
    {
        FRMVECTOR4 terrainBaseColor;
        FRMVECTOR4 constraintAttrib;
        FLOAT32 heightMapSize;
        FRMVECTOR3 Padding; // Pad to multiple of 16-bytes
    };
    CFrmShaderProgramD3D         m_ResampleHeightBaseColorShader;    
    ResampleHeightConstantBuffer m_ResampleHeightConstantBufferData;
    CFrmConstantBuffer*          m_pResampleHeightConstantBuffer;
    
    CFrmShaderProgramD3D         m_ResampleHeightMapColorShader;
    
    struct ComputeNormalsConstantBuffer
    {
        FRMVECTOR4 rtSize;
    };
    CFrmShaderProgramD3D         m_ComputeNormalsShader;    
    ComputeNormalsConstantBuffer m_ComputeNormalsConstantBufferData;
    CFrmConstantBuffer*          m_pComputeNormalsConstantBuffer;
    
    struct AddLayerConstantBuffer
    {
        FRMVECTOR4 noisePanning;
        FRMVECTOR4 constraintAttrib;
        FRMVECTOR4 constraintSlope;
        FRMVECTOR4 constraintAltitude;
        FRMVECTOR4 materialColor;
    };
    CFrmShaderProgramD3D         m_AddLayerShader;
    AddLayerConstantBuffer       m_AddLayerConstantBufferData;
    CFrmConstantBuffer*          m_pAddLayerConstantBuffer;
    
    CFrmShaderProgramD3D      m_ComputeLayerLightingShader;
    
    Mover mover;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_CullDisabledRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DefaultDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DisabledDepthStencilState;

};


#endif // SCENE_H
