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

    VOID CheckFrameBufferStatus();


    VOID RenderScreenAlignedQuad();
    
    // Function calls related to terrain texture caching
    BOOL InitHeightMapAndTextures();
    BOOL InitIntermediateFBOs();
    
    // Calls for Texture Synthesis Processing and Rendering
    VOID AddQuad( TERRAINTILE *tile );
    VOID AddTextureLayerRoot( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface, 
								GLuint *dstColor, 
								GLuint *dstHeightCoverage );
    VOID AddTextureLayer( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface, 
							GLuint *srcColor, 
							GLuint *srcHeight, 
							GLuint *srcCoverage,
							GLuint *dstColor,
							GLuint *dstHeightCoverage );
	
	VOID ComputeLightingLayer( ATLAS *atlas, GLuint *srcColor, GLuint *srcHeight, GLuint *dstColor );
    VOID ComputeTextures();
    VOID ComputeTexturesRecursive( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface,
									INT32 depth=0, GLuint *currentCoverage=NULL, INT32 additionalFlags = 0);

    CFrmFontGLES           m_Font;
    CFrmUserInterfaceGLES  m_UserInterface;
    CFrmTimer              m_Timer;

    // Declare shader variables
    UINT32             m_hOverlayShader;
    UINT32             m_hOverlayScreenSizeLoc;

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;

    // Declare FBO related variables
    INT32              m_nShadowMapFBOTextureWidth;
    INT32              m_nShadowMapFBOTextureHeight;
    UINT32             m_nShadowMapFBOTextureExp;
    UINT32             m_hShadowMapBufId;
    UINT32             m_hShadowMapTexId;

    UINT32             m_hDiffuseSpecularTexId;
    UINT32             m_hDiffuseSpecularBufId;
    UINT32             m_hDiffuseSpecularDepthBufId;

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
    
    GLuint			m_hHeightMapTexture16;	// texture storing full res height map
    GLuint			m_hBicubicWeight03;		// bi-cubic interpolation weights
    GLuint			m_hBicubicWeight47;
    GLuint			m_hBicubicWeight8B;
    GLuint			m_hBicubicWeightCF;
    
    CFrmTexture*	m_pTexNoise;			// texture for generating noise
    CFrmTexture*	m_pTexDisplacement;		// texture for generating height-displacements
    CFrmTexture*	m_pTexShadow;			// shadow and occlusion textures
    CFrmTexture*	m_pTexOcclusion;
    
	GLuint			m_hVertexArrayObject;
    GLuint			m_hVertexBuffer;		// For caching texture tiles
    GLuint			m_hIndexBuffer;
    
    // intermediate render targets/textures for the procedural textures
	GLuint			m_hTexIntermediateColor, 
					m_hTexIntermediateColor2,
					m_hTexIntermediateNormal, 
					m_hTexIntermediateHeightCoverage;
	GLuint			m_hTexIntermediateHeightCoverageLayer[ MAX_LAYER_DEPTH ];
	
	GLuint			m_hTexDebug;
	
	GLuint			m_hColorHeightCoverageFBO;
	
	GLenum DrawBuffers1[1];
	GLenum DrawBuffers2[2];
    
    // Variables for batching texture quads to be updated
    INT32			m_nTempQuads;
	TILEVERTEX		*m_quadList;
    
    // Shaders for procedural terrain texture caching
    UINT32		m_hRenderTerrainShader;
    INT32		m_hRenderTerrainGammaControlFloatLoc;
    INT32		m_hRenderTerrainExposureControlFloatLoc;
    INT32		m_hRenderTerrainRtSizeVec4Loc;
    INT32		m_hRenderTerrainTexcoordBiasScaleVec4Loc;
    INT32		m_hRenderTerrainCameraPositionVec4Loc;
    INT32		m_hRenderTerrainMatModelViewProjMat4Loc;
    INT32		m_hRenderTerrainTerrainTextureLoc;
    
    UINT32      m_hResampleHeightBaseColorShader;
    INT32       m_hResampleHeightBaseColorHeightMapSizeFloatLoc;
    INT32       m_hResampleHeightBaseColorTerrainBaseColorVec4Loc;
    INT32       m_hResampleHeightBaseColorConstraintAttribVec4Loc;
    INT32		m_hResampleHeightBaseColorHeightMapLoc;
    INT32		m_hResampleHeightBaseColorNoiseMapLoc;
    INT32		m_hResampleHeightBaseColorBicubicWeight03Loc;
    INT32		m_hResampleHeightBaseColorBicubicWeight47Loc;
    INT32		m_hResampleHeightBaseColorBicubicWeight8BLoc;
    INT32		m_hResampleHeightBaseColorBicubicWeightCFLoc;
    
    UINT32      m_hResampleHeightMapColorShader;
    INT32       m_hResampleHeightMapColorHeightMapSizeFloatLoc;
    INT32		m_hResampleHeightMapColorColorMapLoc;
    INT32		m_hResampleHeightMapColorHeightTextureLoc;
    INT32		m_hResampleHeightMapColorCoverageTextureLoc;
    
    UINT32      m_hComputeNormalsShader;
    INT32       m_hComputeNormalsRtSizeVec4Loc;
    INT32		m_hComputeNormalsHeightMapLoc;
    
    UINT32      m_hAddLayerShader;
    INT32       m_hAddLayerNoisePanningVec4Loc;
    INT32       m_hAddLayerConstraintAttribVec4Loc;
    INT32       m_hAddLayerConstraintSlopeVec4Loc;
    INT32       m_hAddLayerConstraintAltitudeVec4Loc;
    INT32       m_hAddLayerMaterialColorVec4Loc;
    INT32		m_hAddLayerNormalMapLoc;
    INT32		m_hAddLayerNoiseMapLoc;
    INT32		m_hAddLayerColorMapLoc;
    INT32		m_hAddLayerHeightCoverageTextureLoc;
    
    UINT32      m_hComputeLayerLightingShader;
    INT32       m_hComputeLayerLightingNoisePanningVec4Loc;
    INT32		m_hComputeLayerLightingNormalMapLoc;
    INT32		m_hComputeLayerLightingShadowTextureLoc;
    INT32		m_hComputeLayerLightingOcclusionTextureLoc;
    INT32		m_hComputeLayerLightingColorMapLoc;
    
    Mover mover;
};


#endif // SCENE_H
