//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: Texture Synthesis
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>

#include <OpenGLES/FrmGLES3.h>

#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <OpenGLES/FrmUtilsGLES.h>
#include <FrmUtils.h>
#include <FrmFile.h>

#include <stdio.h>
#include "lod.h"
#include "surface.h"
#include "bicubic.h"
#include "quadtree.h"

#include "Scene.h"

extern ATLAS atlas[ ATLAS_TEXTURES ];

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Texture Synthesis" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    // Initialize variables
    m_hOverlayShader                     = 0;

    m_pLogoTexture                       = NULL;

    m_bShowRT                     = FALSE;
    
    
    // Variables for Cached Procedural Textures
    
    m_flushTextures = 1;
    
    m_heightMap16 = 0;		// resampled height map to 513
    m_heightMap16Full = 0;	// full res height map
    
    m_hHeightMapTexture16 = 0;	// texture storing full res height map
    m_pTexNoise = 0;			// texture for generating noise
    m_pTexDisplacement = 0;		// texture for generating height-displacements
    
    m_hBicubicWeight03 = 0;		// bi-cubic interpolation weights
    m_hBicubicWeight47 = 0;
    m_hBicubicWeight8B = 0;
    m_hBicubicWeightCF = 0;
    
    m_pTexShadow = 0;			// shadow and occlusion textures
    m_pTexOcclusion = 0;
    
	m_hVertexArrayObject = 0;
    m_hVertexBuffer = 0;		// For caching texture tiles
    m_hIndexBuffer = 0;
    
    m_nTempQuads = 0;
	m_quadList = NULL;
	
	DrawBuffers1[0] = GL_COLOR_ATTACHMENT0;
	
	DrawBuffers2[0] = GL_COLOR_ATTACHMENT0;
	DrawBuffers2[1] = GL_COLOR_ATTACHMENT1;

	m_OpenGLESVersion = GLES3;

}

//--------------------------------------------------------------------------------------
// Name: InitTerrainAndTextures()
// Desc: Initialize the terrain textures and geometry
//--------------------------------------------------------------------------------------
BOOL CSample::InitHeightMapAndTextures()
{
	//
	// load terrain
	//
	FRM_FILE *fp;
	heightMapX = heightMapY = 513;
	heightMapXFull = heightMapYFull = 2049;

	if(!FrmFile_Open("Samples/Misc/TextureSynthesis/rainier2049.raw", FRM_FILE_READ, &fp))
		return -1;

	m_heightMap16Full = new UINT16[ heightMapXFull * heightMapYFull ];
	FrmFile_Read(fp, m_heightMap16Full, heightMapXFull * heightMapYFull * sizeof( UINT16 ));
	FrmFile_Close(fp,FRM_FILE_READ);


	// create low res heightmap for triangle mesh
	m_heightMap16 = new UINT16[ heightMapX * heightMapY ];

	int step = (heightMapXFull-1) / (heightMapX-1);
	for ( int y = 0; y < heightMapY; y++ )
		for ( int x = 0; x < heightMapX; x++ )
		{
			m_heightMap16[ x + y * heightMapX ] = 
				m_heightMap16Full[ FRM_MIN( heightMapXFull-1, x * step ) + FRM_MIN( heightMapYFull-1, y * step ) * heightMapXFull ];
		}
		
	//
	// create texturing
	// 
	createSurfaceNodes();
	
	
	// create height map texture
	UINT16 *heightMapDouble = new UINT16[ (heightMapXFull-1)*(heightMapXFull-1)*2 ];
	for ( int j = 0; j < (heightMapXFull-1); j++ )
		for ( int i = 0; i < (heightMapXFull-1); i++ )
		{
			UINT16 h  = m_heightMap16Full[ i +     (heightMapXFull-2-j) * heightMapXFull ];
			UINT16 h2 = m_heightMap16Full[ i + 1 + (heightMapXFull-2-j) * heightMapXFull ];
			heightMapDouble[ (i + j * (heightMapXFull-1)) * 2 + 0 ] = h;
			heightMapDouble[ (i + j * (heightMapXFull-1)) * 2 + 1 ] = h2;
		}
	
	glGenTextures( 1, &m_hHeightMapTexture16 );
	glBindTexture( GL_TEXTURE_2D, m_hHeightMapTexture16 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RG16UI, (heightMapXFull-1), (heightMapYFull-1),
					0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, heightMapDouble );
	glBindTexture( GL_TEXTURE_2D, 0 );
		
		
	/*FLOAT *heightMapDouble = new FLOAT[ (heightMapXFull-1)*(heightMapXFull-1)*2 ];
	for ( int j = 0; j < (heightMapXFull-1); j++ )
		for ( int i = 0; i < (heightMapXFull-1); i++ )
		{
			FLOAT h  = m_heightMap16Full[ i +     (heightMapXFull-2-j) * heightMapXFull ];
			FLOAT h2 = m_heightMap16Full[ i + 1 + (heightMapXFull-2-j) * heightMapXFull ];
			heightMapDouble[ (i + j * (heightMapXFull-1)) * 2 + 0 ] = h;
			heightMapDouble[ (i + j * (heightMapXFull-1)) * 2 + 1 ] = h2;
		}
	
	glGenTextures( 1, &m_hHeightMapTexture16 );
	glBindTexture( GL_TEXTURE_2D, m_hHeightMapTexture16 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RG32F, (heightMapXFull-1), (heightMapYFull-1),
					0, GL_RG, GL_FLOAT, heightMapDouble );
	glBindTexture( GL_TEXTURE_2D, 0 );*/
	
	delete[] heightMapDouble;
	
	// interpolation look-up textures (returns 0 if successful)
	if( generateBiCubicWeights( &m_hBicubicWeight03,
								&m_hBicubicWeight47,
								&m_hBicubicWeight8B,
								&m_hBicubicWeightCF ) )
		return FALSE;
		
	// create chunk LOD quadtree
	createQLOD( heightMapX, m_heightMap16, &m_hHeightMapTexture16 );

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitIntermediateFBOs()
// Desc: Initialize the intermediate FBOs
//--------------------------------------------------------------------------------------
BOOL CSample::InitIntermediateFBOs()
{
	// Instead of creating a lot of separate FBOs, one FBO will be created
	// and when the different textures get bound, we just switch the color attachments
	
	m_flushTextures = 1;
	
	
	// Create FrameBuffer
	glGenFramebuffers( 1, &m_hColorHeightCoverageFBO );
	
	// coverage textures
	glGenTextures( MAX_LAYER_DEPTH, &m_hTexIntermediateHeightCoverageLayer[0] );
	for( int i = 0; i < MAX_LAYER_DEPTH; i++ )
	{
		glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateHeightCoverageLayer[i] );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, RT_SIZE, RT_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	}
	
	
	// surface color textures 
	glGenTextures( 1, &m_hTexIntermediateColor );
	glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateColor );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, RT_SIZE, RT_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	
	glGenTextures( 1, &m_hTexIntermediateColor2 );
	glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateColor2 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, RT_SIZE, RT_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	
	glGenTextures( 1, &m_hTexIntermediateNormal );
	glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateNormal );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, RT_SIZE, RT_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	
	glGenTextures( 1, &m_hTexIntermediateHeightCoverage );
	glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateHeightCoverage );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, RT_SIZE, RT_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	
	glGenTextures( 1, &m_hTexDebug );
	glBindTexture( GL_TEXTURE_2D, m_hTexDebug );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, RT_SIZE, RT_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	
	
	// atlas textures (+ clear them)

	
	// to clear the textures, temporarily use the framebuffer that was just created
	// we can bind the texture as the color buffer, then just call glClear(...)
	
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glBindFramebuffer( GL_FRAMEBUFFER, m_hColorHeightCoverageFBO );
	
	for( int i = 0; i < ATLAS_TEXTURES; i++ )
	{	
		glGenTextures( 1, &atlas[i].texture );
		glBindTexture( GL_TEXTURE_2D, atlas[i].texture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_MIRRORED_REPEAT ); //GL_CLAMP_TO_EDGE );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, RT_SIZE, RT_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
		
		// Bind this texture to the frame buffer, so we can clear it
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, atlas[i].texture, 0 );
		
		// clear the texture
		glClear( GL_COLOR_BUFFER_BIT );
	}
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	
	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the Texture shader
    {
        
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Overlay30.vs",
                                                      "Samples/Shaders/Overlay30.fs",
                                                      &m_hOverlayShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_hOverlayScreenSizeLoc = glGetUniformLocation( m_hOverlayShader, "g_vScreenSize" );
    }
    
    // Create the RenderTerrain shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn",   FRM_VERTEX_POSITION },
            { "g_TexCoordIn",   FRM_VERTEX_TEXCOORD0 }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/RenderTerrain.vs",
                                                      "Samples/Shaders/RenderTerrain.fs",
                                                      &m_hRenderTerrainShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
		m_hRenderTerrainGammaControlFloatLoc	= glGetUniformLocation( m_hRenderTerrainShader, "gammaControl" );
		m_hRenderTerrainExposureControlFloatLoc	= glGetUniformLocation( m_hRenderTerrainShader, "exposureControl" );
		m_hRenderTerrainRtSizeVec4Loc			= glGetUniformLocation( m_hRenderTerrainShader, "rtSize" );
		m_hRenderTerrainTexcoordBiasScaleVec4Loc= glGetUniformLocation( m_hRenderTerrainShader, "texCoordBiasScale" );
		m_hRenderTerrainCameraPositionVec4Loc	= glGetUniformLocation( m_hRenderTerrainShader, "cameraPosition" );
		m_hRenderTerrainMatModelViewProjMat4Loc = glGetUniformLocation( m_hRenderTerrainShader, "g_MatModelViewProj" );
		m_hRenderTerrainTerrainTextureLoc		= glGetUniformLocation( m_hRenderTerrainShader, "terrainTexture" );
    }
    
    // Create the ResampleHeightBaseColor shader
    {    
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn",	FRM_VERTEX_POSITION },
            { "g_CoordHMIn",	FRM_VERTEX_TEXCOORD0 },
            { "g_CoordTileIn",	FRM_VERTEX_TEXCOORD1 },
            { "g_ScaleIn",		FRM_VERTEX_TEXCOORD2 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/ResampleHeightBaseColor.vs",
                                                      "Samples/Shaders/ResampleHeightBaseColor.fs",
                                                      &m_hResampleHeightBaseColorShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
		m_hResampleHeightBaseColorHeightMapSizeFloatLoc	= glGetUniformLocation( m_hResampleHeightBaseColorShader, "heightMapSize" );
		m_hResampleHeightBaseColorTerrainBaseColorVec4Loc = glGetUniformLocation( m_hResampleHeightBaseColorShader, "terrainBaseColor" );
		m_hResampleHeightBaseColorConstraintAttribVec4Loc = glGetUniformLocation( m_hResampleHeightBaseColorShader, "constraintAttrib" );
		m_hResampleHeightBaseColorHeightMapLoc			= glGetUniformLocation( m_hResampleHeightBaseColorShader, "heightMap" );
		m_hResampleHeightBaseColorNoiseMapLoc			= glGetUniformLocation( m_hResampleHeightBaseColorShader, "noiseMap" );
		m_hResampleHeightBaseColorBicubicWeight03Loc	= glGetUniformLocation( m_hResampleHeightBaseColorShader, "bicubicWeight03" );
		m_hResampleHeightBaseColorBicubicWeight47Loc	= glGetUniformLocation( m_hResampleHeightBaseColorShader, "bicubicWeight47" );
		m_hResampleHeightBaseColorBicubicWeight8BLoc	= glGetUniformLocation( m_hResampleHeightBaseColorShader, "bicubicWeight8B" );
		m_hResampleHeightBaseColorBicubicWeightCFLoc	= glGetUniformLocation( m_hResampleHeightBaseColorShader, "bicubicWeightCF" );
    }
    
    // Create the ResampleHeightMapColor shader
    {    
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn",	FRM_VERTEX_POSITION },
            { "g_CoordHMIn",	FRM_VERTEX_TEXCOORD0 },
            { "g_CoordTileIn",	FRM_VERTEX_TEXCOORD1 },
            { "g_ScaleIn",		FRM_VERTEX_TEXCOORD2 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/ResampleHeightMapColor.vs",
                                                      "Samples/Shaders/ResampleHeightMapColor.fs",
                                                      &m_hResampleHeightMapColorShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
		m_hResampleHeightMapColorHeightMapSizeFloatLoc	= glGetUniformLocation( m_hResampleHeightMapColorShader, "heightMapSize" );
		m_hResampleHeightMapColorColorMapLoc			= glGetUniformLocation( m_hResampleHeightMapColorShader, "colorMap" );
		m_hResampleHeightMapColorHeightTextureLoc		= glGetUniformLocation( m_hResampleHeightMapColorShader, "heightTexture" );
		m_hResampleHeightMapColorCoverageTextureLoc		= glGetUniformLocation( m_hResampleHeightMapColorShader, "coverageTexture" );
    }
    
    // Create the ComputeNormals shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn",	FRM_VERTEX_POSITION },
            { "g_CoordHMIn",	FRM_VERTEX_TEXCOORD0 },
            { "g_CoordTileIn",	FRM_VERTEX_TEXCOORD1 },
            { "g_ScaleIn",		FRM_VERTEX_TEXCOORD2 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/ComputeNormals.vs",
                                                      "Samples/Shaders/ComputeNormals.fs",
                                                      &m_hComputeNormalsShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
		m_hComputeNormalsRtSizeVec4Loc = glGetUniformLocation( m_hComputeNormalsShader, "rtSize" );
		m_hComputeNormalsHeightMapLoc  = glGetUniformLocation( m_hComputeNormalsShader, "heightMap" );
    }
    
    // Create the AddLayer shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn",	FRM_VERTEX_POSITION },
            { "g_CoordHMIn",	FRM_VERTEX_TEXCOORD0 },
            { "g_CoordTileIn",	FRM_VERTEX_TEXCOORD1 },
            { "g_ScaleIn",		FRM_VERTEX_TEXCOORD2 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/AddLayer.vs",
                                                      "Samples/Shaders/AddLayer.fs",
                                                      &m_hAddLayerShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hAddLayerNoisePanningVec4Loc		= glGetUniformLocation( m_hAddLayerShader, "noisePanning" );
		m_hAddLayerConstraintAttribVec4Loc	= glGetUniformLocation( m_hAddLayerShader, "constraintAttrib" );
		m_hAddLayerConstraintSlopeVec4Loc	= glGetUniformLocation( m_hAddLayerShader, "constraintSlope" );
		m_hAddLayerConstraintAltitudeVec4Loc= glGetUniformLocation( m_hAddLayerShader, "constraintAltitude" );
		m_hAddLayerMaterialColorVec4Loc		= glGetUniformLocation( m_hAddLayerShader, "materialColor" );
		m_hAddLayerNormalMapLoc				= glGetUniformLocation( m_hAddLayerShader, "normalMap" );
		m_hAddLayerNoiseMapLoc				= glGetUniformLocation( m_hAddLayerShader, "noiseMap" );
		m_hAddLayerColorMapLoc				= glGetUniformLocation( m_hAddLayerShader, "colorMap" );
		m_hAddLayerHeightCoverageTextureLoc	= glGetUniformLocation( m_hAddLayerShader, "heightCoverageTexture" );
    }
    
    // Create the ComputeLayerLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn",	FRM_VERTEX_POSITION },
            { "g_CoordHMIn",	FRM_VERTEX_TEXCOORD0 },
            { "g_CoordTileIn",	FRM_VERTEX_TEXCOORD1 },
            { "g_ScaleIn",		FRM_VERTEX_TEXCOORD2 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/AddLayer.vs",
                                                      "Samples/Shaders/ComputeLayerLighting.fs",
                                                      &m_hComputeLayerLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
		//m_hComputeLayerLightingNoisePanningVec4Loc = glGetUniformLocation( m_hComputeLayerLightingShader, "noisePanning" );
		//m_hComputeLayerLightingNormalMapLoc		   = glGetUniformLocation( m_hComputeLayerLightingShader, "normalMap" );
		//m_hComputeLayerLightingShadowTextureLoc	   = glGetUniformLocation( m_hComputeLayerLightingShader, "shadowTexture" );
		//m_hComputeLayerLightingOcclusionTextureLoc = glGetUniformLocation( m_hComputeLayerLightingShader, "occlusionTexture" );
		//m_hComputeLayerLightingColorMapLoc		   = glGetUniformLocation( m_hComputeLayerLightingShader, "colorMap" );

		const GLchar *uniformNames[] = {"noisePanning",
										"normalMap",
										"shadowTexture",
										"occlusionTexture",
										"colorMap" };

		GLuint uniformLocs[5];
		glGetUniformIndices( m_hComputeLayerLightingShader, 5, uniformNames, &uniformLocs[0] );
		
		m_hComputeLayerLightingNoisePanningVec4Loc	= uniformLocs[0];
		m_hComputeLayerLightingNormalMapLoc			= uniformLocs[1];
		m_hComputeLayerLightingShadowTextureLoc		= uniformLocs[2];
		m_hComputeLayerLightingOcclusionTextureLoc	= uniformLocs[3];
		m_hComputeLayerLightingColorMapLoc			= uniformLocs[4];
    }


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Initialize sample variables
    m_bShowRT = false;

    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/TextureSynthesis.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );
    
    // Create the terrain textures
    m_pTexNoise = resource.GetTexture( "Noise" );
    m_pTexDisplacement = resource.GetTexture( "Displacement" );
    m_pTexShadow = resource.GetTexture( "Shadow" );
    m_pTexOcclusion = resource.GetTexture( "Ambient" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Decrease Quality Level" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Increase Quality Level" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Decrease Exposure" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Increase Exposure" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Decrease Gamma Control" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, "Increase Gamma Control" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, "Toggle Terrain Type" );

	m_UserInterface.AddIntVariable( &g_qualitySelect, "Quality Level" );
	m_UserInterface.AddFloatVariable( &g_sliderExposure, "Exposure Control" );
	m_UserInterface.AddFloatVariable( &g_sliderGamma, "Gamma Control" );
	
	CURRENT_SURFACE_TYPE_STR = SURFACE_TYPE_STR[ g_surfaceSelect ];
	m_UserInterface.AddStringVariable( &CURRENT_SURFACE_TYPE_STR, "Surface Type" );

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
        
    // Initialize the intermediate textures/FBOs
    if( FALSE == InitIntermediateFBOs() )
		return FALSE;
        
    // Initialize the data for terrain rendering
    if( FALSE == InitHeightMapAndTextures() )
		return FALSE;

    // Initialize the camera and light perspective matrices
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 40000.0f );

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );
    
    // camera setup
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 1000.0f, 7000.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    
    mover.SetPosition( vCameraPosition );
    
    m_camPos = FRMVECTOR4( vCameraPosition.x, vCameraPosition.y, vCameraPosition.z, 1.0f );
    m_camLookAt = FRMVECTOR4( vCameraLookAt.x, vCameraLookAt.y, vCameraLookAt.z, 1.0f );
    
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the model view proj
    m_matModelViewProj = FrmMatrixMultiply( m_matCameraView, m_matCameraPersp );

    // Setup GL state
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 40000.0f );
    m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matCameraPersp.M(0,0) *= fAspect;
        m_matCameraPersp.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Delete shader programs
    if( m_hOverlayShader )          glDeleteProgram( m_hOverlayShader );

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();
    
    if( m_pTexNoise )			m_pTexNoise->Release();
    if( m_pTexDisplacement )	m_pTexDisplacement->Release();
    if( m_pTexShadow )			m_pTexShadow->Release();
    if( m_pTexOcclusion )		m_pTexOcclusion->Release();
    
    // Delete Textures
    if( glIsTexture( m_hHeightMapTexture16 ) )
		glDeleteTextures( 1, &m_hHeightMapTexture16 );
	if( glIsTexture( m_hBicubicWeight03 ) )
		glDeleteTextures( 1, &m_hBicubicWeight03 );
	if( glIsTexture( m_hBicubicWeight47 ) )
		glDeleteTextures( 1, &m_hBicubicWeight47 );
	if( glIsTexture( m_hBicubicWeight8B ) )
		glDeleteTextures( 1, &m_hBicubicWeight8B );
	if( glIsTexture( m_hBicubicWeightCF ) )
		glDeleteTextures( 1, &m_hBicubicWeightCF );
	
    
    // Delete Buffers
    if( glIsBuffer( m_hVertexBuffer ) ) glDeleteBuffers( 1, &m_hVertexBuffer );
    if( glIsBuffer( m_hIndexBuffer ) )	glDeleteBuffers( 1, &m_hIndexBuffer );
    
    // delete data
    if( m_heightMap16 != NULL )
		delete[] m_heightMap16;
	if( m_heightMap16Full != NULL )
		delete[] m_heightMap16Full;
    
    m_flushTextures = 1;
    destroyQLOD();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fTime        = m_Timer.GetFrameTime();
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );
    
    mover.Update( m_Input, 0.5f );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
    {
        m_UserInterface.AdvanceState();
    }

	if( nPressedButtons & INPUT_KEY_1 )
	{
		g_qualitySelect = FRM_MAX( g_qualitySelect - 1, 1 );
	}

	if( nPressedButtons & INPUT_KEY_2 )
	{
		g_qualitySelect = FRM_MIN( g_qualitySelect + 1, 10 );
	}

	if( nButtons & INPUT_KEY_3 )
	{
		g_sliderExposure -= 2.0f * fElapsedTime;
		g_sliderExposure = FRM_MAX( g_sliderExposure, 0.0f );
	}

	if( nButtons & INPUT_KEY_4 )
	{
		g_sliderExposure += 2.0f * fElapsedTime;
		g_sliderExposure = FRM_MIN( g_sliderExposure, 3.0f );
	}

	if( nButtons & INPUT_KEY_5 )
	{
		g_sliderGamma -= 2.0f * fElapsedTime;
		g_sliderGamma = FRM_MAX( g_sliderGamma, 0.0f );
	}

	if( nButtons & INPUT_KEY_6 )
	{
		g_sliderGamma += 2.0f * fElapsedTime;
		g_sliderGamma = FRM_MIN( g_sliderGamma, 3.0f );
	}

	if( nPressedButtons & INPUT_KEY_7 )
	{
		g_surfaceSelect = abs( 1 - g_surfaceSelect );
		CURRENT_SURFACE_TYPE_STR = SURFACE_TYPE_STR[ g_surfaceSelect ];
	}

    // Toggle display of debug render target
    if( nPressedButtons & INPUT_KEY_9 )
    {
        m_bShowRT = !m_bShowRT;
    }
}


//--------------------------------------------------------------------------------------
// Name: CheckFrameBufferStatus()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CheckFrameBufferStatus()
{
    GLenum nStatus;
    nStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch( nStatus )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            break;
        default:
            FrmAssert(0);
    }
}


//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad()
{
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    const FLOAT32 Quad[] =
    {
        -1.0, -1.0f, 0.0f, 1.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
        +1.0, -1.0f, 1.0f, 1.0f,
        +1.0, +1.0f, 1.0f, 0.0f,
    };

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 16, Quad, GL_DYNAMIC_DRAW);

	glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, 0 );
    glEnableVertexAttribArray( 0 );

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glDisableVertexAttribArray( 0 );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

    glDisable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
	static int lastSurfaceType = 0;
	static int lastQuality     = 5;

	extern void buildFrustum( FRMMATRIX4X4 &clip );
	
	// update the MVP
	FRMVECTOR3 vCameraPosition = mover.GetPosition();
	FRMVECTOR3 vCameraDirection = mover.GetDirection();
	FRMVECTOR3 vCameraLookAt = vCameraPosition + 100.0f*vCameraDirection;
	m_camPos = FRMVECTOR4( vCameraPosition.x, vCameraPosition.y, vCameraPosition.z, 1.0f );
    m_camLookAt = FRMVECTOR4( vCameraLookAt.x, vCameraLookAt.y, vCameraLookAt.z, 1.0f );
    FRMVECTOR3 upVec = FRMVECTOR3(0.0f, 1.0f, 0.0f);
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, upVec);
    m_matModelViewProj = FrmMatrixMultiply( m_matCameraView, m_matCameraPersp );
 
	buildFrustum( m_matModelViewProj );

	if( lastSurfaceType != g_surfaceSelect || lastQuality != g_qualitySelect )
	{
		lastSurfaceType = g_surfaceSelect;
		lastQuality = g_qualitySelect;
		m_flushTextures = 1;
		createSurfaceNodes();
		rootSurfaceNode = surfaceTypes[ g_surfaceSelect ];
	}
	
	if( m_flushTextures )
		freeAllTexturesQLOD( 0, 0 );

	traverseQLOD( m_camPos, m_camLookAt, 0, 0 );
	m_flushTextures = 0;
	
	glViewport( 0, 0, RT_SIZE, RT_SIZE );
	ComputeTextures();
	
	
	
	// Main pass
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
	
	
	// ensure the main framebuffer is bound
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( 0, 0, m_nWidth, m_nHeight );
	
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	float t1 = 1.0f / g_sliderGamma, t2 = -g_sliderExposure;
	
	glUseProgram( m_hRenderTerrainShader );
	
	glUniform1f( m_hRenderTerrainGammaControlFloatLoc, t1 );
	glUniform1f( m_hRenderTerrainExposureControlFloatLoc, t2 );
	
	// set in renderQLOD, m_hRenderTerrainRtSizeVec4Loc;
    // set in renderQLOD, m_hRenderTerrainTexcoordBiasScaleVec4Loc;
    // set in renderQLOD, m_hRenderTerrainTerrainTextureLoc;
    
    glUniform4fv( m_hRenderTerrainCameraPositionVec4Loc, 1, (FLOAT32 *)&m_camPos );
    glUniformMatrix4fv( m_hRenderTerrainMatModelViewProjMat4Loc, 1, GL_FALSE,
						(FLOAT32 *)&m_matModelViewProj );
	
	renderQLOD( &m_hRenderTerrainRtSizeVec4Loc,
				&m_hRenderTerrainTexcoordBiasScaleVec4Loc,
				&m_hRenderTerrainTerrainTextureLoc );
	
	/*
	
	// Use this to copy a texture to the debug texture
	static bool doOnce = false;
    if( !doOnce )
    {
		glBindFramebuffer( GL_FRAMEBUFFER, m_hColorHeightCoverageFBO );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  *dstHeightCoverage, 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  0, 0 );
		CheckFrameBufferStatus();
    
		doOnce = true;
		// before unbinding the fbo, copy the texture
		glBindTexture( GL_TEXTURE_2D, m_hTexDebug );
		glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, RT_SIZE, RT_SIZE, 0 );
	}
	*/

	/*
	if( m_bShowRT )
    {
		extern ATLAS atlas[ ATLAS_TEXTURES ];
    
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		FrmRenderTextureToScreen_GLES( 15, 15, 512, 512, atlas[10].texture,
                                       m_hOverlayShader, m_hOverlayScreenSizeLoc );
    }
	*/

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}



void CSample::AddQuad( TERRAINTILE *tile )
{
	m_quadList[ m_nTempQuads * 4 + 0 ].x = 0.0f; m_quadList[ m_nTempQuads * 4 + 0 ].y = 0.0f;
	m_quadList[ m_nTempQuads * 4 + 1 ].x = 0.0f; m_quadList[ m_nTempQuads * 4 + 1 ].y = 1.0f;
	m_quadList[ m_nTempQuads * 4 + 2 ].x = 1.0f; m_quadList[ m_nTempQuads * 4 + 2 ].y = 0.0f;
	m_quadList[ m_nTempQuads * 4 + 3 ].x = 1.0f; m_quadList[ m_nTempQuads * 4 + 3 ].y = 1.0f;

	for ( int i = 0; i < 4; i++ )
	{
		m_quadList[ m_nTempQuads * 4 + i ].z = (float)tile->tileSize;
		m_quadList[ m_nTempQuads * 4 + i ].coordHMBias1    = tile->coordHM.x;
		m_quadList[ m_nTempQuads * 4 + i ].coordHMBias2    = tile->coordHM.y;
		m_quadList[ m_nTempQuads * 4 + i ].coordHMScale1   = tile->coordHM.z;
		m_quadList[ m_nTempQuads * 4 + i ].coordHMScale2   = tile->coordHM.w;
		m_quadList[ m_nTempQuads * 4 + i ].coordTileBias1  = tile->coordTile.x;
		m_quadList[ m_nTempQuads * 4 + i ].coordTileBias2  = tile->coordTile.y;
		m_quadList[ m_nTempQuads * 4 + i ].coordTileScale1 = tile->coordTile.z;
		m_quadList[ m_nTempQuads * 4 + i ].coordTileScale2 = tile->coordTile.w;
		
		float realSizeM = tile->worldSize / (float)(heightMapX-1) / (float)tile->tileSize * 15300.0f * 2.6777777f;
		float heightScale = 65536.0f * 0.1f;
		m_quadList[ m_nTempQuads * 4 + i ].scale1 = heightScale / realSizeM;
		m_quadList[ m_nTempQuads * 4 + i ].scale2 = realSizeM / heightScale;
	}

	m_nTempQuads ++;
}


VOID CSample::AddTextureLayer( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface, 
							GLuint *srcColor, 
							GLuint *srcHeight, 
							GLuint *srcCoverage,
							GLuint *dstColor,
							GLuint *dstHeightCoverage )
{
	// setup GL states for texture synthesis
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glDepthFunc( GL_ALWAYS );
    
	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 0: read input heightmap (maybe modify height values here!)
	//         IN : height, coverage, current surface color
	//              noise textures
	//         OUT: intermediate color and height+coverage
	//
	glBindBuffer( GL_ARRAY_BUFFER, m_hVertexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_hIndexBuffer );
	
	glVertexAttribPointer( FRM_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(0*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_POSITION );
	
	glVertexAttribPointer( FRM_VERTEX_TEXCOORD0, 4, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(3*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_TEXCOORD0 );
	
	glVertexAttribPointer( FRM_VERTEX_TEXCOORD1, 4, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(7*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_TEXCOORD1 );
	
	glVertexAttribPointer( FRM_VERTEX_TEXCOORD2, 2, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(11*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_TEXCOORD2 );
	
	// Bind the two render targets for Color and Height/Coverage
	glBindFramebuffer( GL_FRAMEBUFFER, m_hColorHeightCoverageFBO );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  m_hTexIntermediateColor, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  m_hTexIntermediateHeightCoverage, 0 );
	glDrawBuffers(2, DrawBuffers2);

	// clear the render targets
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	glUseProgram( m_hResampleHeightMapColorShader );
	
	glUniform1f( m_hResampleHeightMapColorHeightMapSizeFloatLoc, (FLOAT32)(heightMapXFull - 1) );

	glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, *srcColor );
    glUniform1i( m_hResampleHeightMapColorColorMapLoc, 0 );
    
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, *srcHeight );
    glUniform1i( m_hResampleHeightMapColorHeightTextureLoc, 1 );
    
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, *srcCoverage );
    glUniform1i( m_hResampleHeightMapColorCoverageTextureLoc, 2 );

    glDrawElements( GL_TRIANGLES, m_nTempQuads*2*3, GL_UNSIGNED_SHORT, 0 );

	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 1: compute normals for heightmap
	//         IN : intermediate height map (if height values have been modified in step 0), otherwise input height
	//         OUT: intermediate normal
	//
	
	// Framebuffer is already bound, just need to change the color attachments
	//	( texIntermediateNormal and clear attachment 1 )
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  m_hTexIntermediateNormal, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  0, 0 );
	glDrawBuffers(1, DrawBuffers1);
	
	glUseProgram( m_hComputeNormalsShader );
	
	FRMVECTOR4 rtSize = FRMVECTOR4( (FLOAT32)RT_SIZE, 1.0f/(FLOAT32)RT_SIZE, 0.0f, (FLOAT32)quadsPerTile );
	glUniform4fv( m_hComputeNormalsRtSizeVec4Loc, 1, (FLOAT32 *)&rtSize );
	
	glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateHeightCoverage );
    glUniform1i( m_hComputeNormalsHeightMapLoc, 0 );
    
    glDrawElements( GL_TRIANGLES, m_nTempQuads*2*3, GL_UNSIGNED_SHORT, 0 );

	glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateNormal );
	glGenerateMipmap( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );

	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 2: add texture layer
	//         IN : intermediate height + coverage, current surface color, noise, ...
	//         OUT: new surface color
	//              new height+coverage
	//
	
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  *dstColor, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  *dstHeightCoverage, 0 );
	
	glDrawBuffers(2, DrawBuffers2);
	
	glUseProgram( m_hAddLayerShader );
    
    glUniform4fv( m_hAddLayerNoisePanningVec4Loc,		1, (FLOAT32 *)&surface.noisePan );
	glUniform4fv( m_hAddLayerConstraintAltitudeVec4Loc, 1, (FLOAT32 *)&surface.tpAScaleBias );
	glUniform4fv( m_hAddLayerConstraintSlopeVec4Loc,	1, (FLOAT32 *)&surface.tpSScaleBias );
	glUniform4fv( m_hAddLayerMaterialColorVec4Loc,		1, (FLOAT32 *)&surface.spColor );
	
	FRMVECTOR4 attribVec( surface.spCoverage, surface.spFractalNoise, 
		surface.spBumpiness*0.25f, 
		surface.spMimicTerrain );

	glUniform4fv( m_hAddLayerConstraintAttribVec4Loc, 1, (FLOAT32 *)&attribVec );
	
	glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateNormal );
    glUniform1i( m_hAddLayerNormalMapLoc, 0 );
    
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_pTexNoise->m_hTextureHandle );
    glUniform1i( m_hAddLayerNoiseMapLoc, 1 );
    
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateColor );
    glUniform1i( m_hAddLayerColorMapLoc, 2 );
    
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateHeightCoverage );
    glUniform1i( m_hAddLayerHeightCoverageTextureLoc, 3 );

	glDrawElements( GL_TRIANGLES, m_nTempQuads*2*3, GL_UNSIGNED_SHORT, 0 );
	
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  0, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  0, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// Disable attrib pointers and unbind the vert/index buffers
	glDisableVertexAttribArray( FRM_VERTEX_POSITION );
	glDisableVertexAttribArray( FRM_VERTEX_TEXCOORD0 );
	glDisableVertexAttribArray( FRM_VERTEX_TEXCOORD1 );
	glDisableVertexAttribArray( FRM_VERTEX_TEXCOORD2 );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	
	// reset GL states
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
}


VOID CSample::AddTextureLayerRoot( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface, 
								GLuint *dstColor, 
								GLuint *dstHeightCoverage )
{
	// setup GL states for texture synthesis
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glDepthFunc( GL_ALWAYS );

	const int VBO_SIZE = 1024*128;

	if (m_hVertexArrayObject == 0)
	{
		glGenVertexArrays(1, &m_hVertexArrayObject);
	}

	// initialize vertex buffer
	if( m_hVertexBuffer == 0 )
	{
		//FrmCreateVertexBuffer( VBO_SIZE, 1, 0, &m_hVertexBuffer );
		
		glGenBuffers( 1, &m_hVertexBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, m_hVertexBuffer );
		glBufferData( GL_ARRAY_BUFFER, VBO_SIZE, NULL, GL_DYNAMIC_DRAW );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
	/*HRESULT hr;
	if ( quadVB == NULL )
	{
		pD3DDevice->CreateVertexBuffer( 1024*128,D3DUSAGE_WRITEONLY,
			D3DFVF_XYZW|D3DFVF_TEX5,
			D3DPOOL_MANAGED, &quadVB, NULL );
	}*/

	// Initialize and fill up index buffer
	if( m_hIndexBuffer == 0 )
	{
		//FrmCreateIndexBuffer( 1024*2*3, sizeof(UINT16), 0, &m_hIndexBuffer );
		glGenBuffers( 1, &m_hIndexBuffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_hIndexBuffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, 1024*2*3*sizeof(UINT16), 0, GL_DYNAMIC_DRAW );
		
		UINT16 *idx;
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_hIndexBuffer );
		idx = (UINT16 *)glMapBufferRange( GL_ELEMENT_ARRAY_BUFFER, 0, 1024*2*3*sizeof(UINT16), GL_MAP_WRITE_BIT );
		
		for ( int i = 0; i < 1024; i++ )
		{
			*(idx++) = i * 4 + 0;
			*(idx++) = i * 4 + 1;
			*(idx++) = i * 4 + 3;
			*(idx++) = i * 4 + 0;
			*(idx++) = i * 4 + 3;
			*(idx++) = i * 4 + 2;
		}
		
		glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}


	glBindVertexArray(m_hVertexArrayObject);

	// Fill up vertex buffer
	glBindBuffer( GL_ARRAY_BUFFER, m_hVertexBuffer );
	m_quadList = (TILEVERTEX *)glMapBufferRange( GL_ARRAY_BUFFER, 0, VBO_SIZE, GL_MAP_WRITE_BIT );
	
		m_nTempQuads = 0;
		for ( int i = 0; i < (int)atlas->count; i++ )
			AddQuad( atlas->assign[ i ] );
	
	glUnmapBuffer( GL_ARRAY_BUFFER );

	glBindBuffer( GL_ARRAY_BUFFER, m_hVertexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_hIndexBuffer );
	
	glVertexAttribPointer( FRM_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(0*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_POSITION );
	
	glVertexAttribPointer( FRM_VERTEX_TEXCOORD0, 4, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(3*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_TEXCOORD0 );
	
	glVertexAttribPointer( FRM_VERTEX_TEXCOORD1, 4, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(7*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_TEXCOORD1 );
	
	glVertexAttribPointer( FRM_VERTEX_TEXCOORD2, 2, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(11*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_TEXCOORD2 );

	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 0: read original heightmap and resample to new resolution
	//         IN : tile->pHeightMapTexture (16-bit integer: x=height)
	//              lookup + displacement textures
	//         OUT: intermediate color and height+coverage
	//
	
	// Bind the two render targets for Color and Height/Coverage
	glBindFramebuffer( GL_FRAMEBUFFER, m_hColorHeightCoverageFBO );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  *dstColor, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  *dstHeightCoverage, 0 );
	glDrawBuffers(2, DrawBuffers2);

	glUseProgram( m_hResampleHeightBaseColorShader );
	
	glUniform1f( m_hResampleHeightBaseColorHeightMapSizeFloatLoc, (FLOAT32)(heightMapXFull - 1) );
	glUniform4fv( m_hResampleHeightBaseColorTerrainBaseColorVec4Loc, 1, (FLOAT32 *)&terrainBaseColor );
	
	// surface attributes
	FRMVECTOR4 attribVec( surface.spCoverage, surface.spFractalNoise, 
		surface.spBumpiness*0.25f, 
		surface.spMimicTerrain );
	glUniform4fv( m_hResampleHeightBaseColorConstraintAttribVec4Loc, 1, (FLOAT32 *)&attribVec );

	glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, *atlas->assign[0]->pHeightMapTexture );
    glUniform1i( m_hResampleHeightBaseColorHeightMapLoc, 0 );
    
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_pTexDisplacement->m_hTextureHandle );
    glUniform1i( m_hResampleHeightBaseColorNoiseMapLoc, 1 );
    
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, m_hBicubicWeight03 );
    glUniform1i( m_hResampleHeightBaseColorBicubicWeight03Loc, 2 );
    
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, m_hBicubicWeight47 );
    glUniform1i( m_hResampleHeightBaseColorBicubicWeight47Loc, 3 );
    
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, m_hBicubicWeight8B );
    glUniform1i( m_hResampleHeightBaseColorBicubicWeight8BLoc, 4 );
    
    glActiveTexture( GL_TEXTURE5 );
    glBindTexture( GL_TEXTURE_2D, m_hBicubicWeightCF );
    glUniform1i( m_hResampleHeightBaseColorBicubicWeightCFLoc, 5 );

    glDrawElements( GL_TRIANGLES, m_nTempQuads*2*3, GL_UNSIGNED_SHORT, 0 );
    
	// unbind framebuffers, vertex buffers, index buffers
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  0, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  0, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// Disable attrib pointers and unbind the vert/index buffers
	glDisableVertexAttribArray( FRM_VERTEX_POSITION );
	glDisableVertexAttribArray( FRM_VERTEX_TEXCOORD0 );
	glDisableVertexAttribArray( FRM_VERTEX_TEXCOORD1 );
	glDisableVertexAttribArray( FRM_VERTEX_TEXCOORD2 );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	
	// reset GL states
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
}


VOID CSample::ComputeLightingLayer( ATLAS *atlas, GLuint *srcColor, GLuint *srcHeight, GLuint *dstColor )
{
    FrmLogMessage("Inside ComputeLightingLayer");

	// setup GL states for texture synthesis
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glDepthFunc( GL_ALWAYS );

	glBindBuffer( GL_ARRAY_BUFFER, m_hVertexBuffer );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_hIndexBuffer );
	
	glVertexAttribPointer( FRM_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(0*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_POSITION );
	
	glVertexAttribPointer( FRM_VERTEX_TEXCOORD0, 4, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(3*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_TEXCOORD0 );
	
	glVertexAttribPointer( FRM_VERTEX_TEXCOORD1, 4, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(7*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_TEXCOORD1 );
	
	glVertexAttribPointer( FRM_VERTEX_TEXCOORD2, 2, GL_FLOAT, GL_FALSE, sizeof( TILEVERTEX ), (VOID *)(11*sizeof(FLOAT32)) );
	glEnableVertexAttribArray( FRM_VERTEX_TEXCOORD2 );

	glBindFramebuffer( GL_FRAMEBUFFER, m_hColorHeightCoverageFBO );

	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 0: compute normals for heightmap
	//
	
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  m_hTexIntermediateNormal, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  0, 0 );
	glDrawBuffers(1, DrawBuffers1);
	
	glUseProgram( m_hComputeNormalsShader );
	
	FRMVECTOR4 rtSize = FRMVECTOR4( (FLOAT32)RT_SIZE, 1.0f/(FLOAT32)RT_SIZE, 0.0f, (FLOAT32)quadsPerTile );
	glUniform4fv( m_hComputeNormalsRtSizeVec4Loc, 1, (FLOAT32 *)&rtSize );
	
	glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, *srcHeight );
    glUniform1i( m_hComputeNormalsHeightMapLoc, 0 );
    
    glDrawElements( GL_TRIANGLES, m_nTempQuads*2*3, GL_UNSIGNED_SHORT, 0 );

	glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateNormal );
	glGenerateMipmap( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );

	/////////////////////////////////////////////////////////////////////////////////////////////
	// STEP 1: compute lighting
	
	
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  *dstColor, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  0, 0 );
	glDrawBuffers(1, DrawBuffers1);
	
	glUseProgram( m_hComputeLayerLightingShader );
    
    // noisePanning is unused, doesn't need to be set
	// glUniform4fv( m_hComputeLayerLightingNoisePanningVec4Loc, 1, (FLOAT *)&surface.noisePan );
	
	glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_hTexIntermediateNormal );
    glUniform1i( m_hComputeLayerLightingNormalMapLoc, 0 );
    
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_pTexShadow->m_hTextureHandle );
    glUniform1i( m_hComputeLayerLightingShadowTextureLoc, 1 );
    
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, m_pTexOcclusion->m_hTextureHandle );
    glUniform1i( m_hComputeLayerLightingOcclusionTextureLoc, 2 );
    
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, *srcColor );
    glUniform1i( m_hComputeLayerLightingColorMapLoc, 3 );

	glDrawElements( GL_TRIANGLES, m_nTempQuads*2*3, GL_UNSIGNED_SHORT, 0 );
	
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  0, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,  0, 0 );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	// Disable attrib pointers and unbind the vert/index buffers
	glDisableVertexAttribArray( FRM_VERTEX_POSITION );
	glDisableVertexAttribArray( FRM_VERTEX_TEXCOORD0 );
	glDisableVertexAttribArray( FRM_VERTEX_TEXCOORD1 );
	glDisableVertexAttribArray( FRM_VERTEX_TEXCOORD2 );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}


#define NO_MORE_ON_THIS_LEVEL	1
static GLuint* lastHeightCoverage = NULL;

VOID CSample::ComputeTexturesRecursive( ATLAS *atlas, PROCTEX_SURFACEPARAM &surface,
									INT32 depth, GLuint *currentCoverage, INT32 additionalFlags)
{
	// if this is our first layer, we do not need to evaluate constraints, but resample the height map
	if ( depth == 0 )
	{
		AddTextureLayerRoot( atlas, surface, &m_hTexIntermediateColor2, &m_hTexIntermediateHeightCoverageLayer[ depth ] );
		lastHeightCoverage = &m_hTexIntermediateHeightCoverageLayer[ depth ];
	} else
	// ok, just add another layer
	{
		AddTextureLayer( atlas, surface,
			&m_hTexIntermediateColor2,								// src color
			lastHeightCoverage,										// src height	(but is a height and coverage texture...)
			&m_hTexIntermediateHeightCoverageLayer[ depth - 1 ],	// src coverage (but is a height and coverage texture...)
			&m_hTexIntermediateColor2,								// dst color
			&m_hTexIntermediateHeightCoverageLayer[ depth ] );		// dst coverage (height and coverage)
		lastHeightCoverage = &m_hTexIntermediateHeightCoverageLayer[ depth ];
	}

	// and don't forget child surface nodes
	for ( int i = 0; i < surface.nChilds; i++ )
	{
		int addFlags = 0;

		if ( i == surface.nChilds - 1 )
			if ( additionalFlags & NO_MORE_ON_THIS_LEVEL || depth == 0 )
				addFlags |= NO_MORE_ON_THIS_LEVEL;

		ComputeTexturesRecursive( atlas, *surface.child[ i ], depth + 1, &m_hTexIntermediateHeightCoverageLayer[ depth - 1 ], addFlags );
	}
}


VOID CSample::ComputeTextures()
{
	//extern ATLAS atlas[ ATLAS_TEXTURES ];
    if (!rootSurfaceNode)
    {
        FrmLogMessage("ComputeTextures::rootSurfaceNode is NULL");
        return;
    }

	terrainBaseColor = rootSurfaceNode->spColor;

	for ( int i = 0; i < ATLAS_TEXTURES; i++ )
    {
        if ( atlas[ i ].count > 0 )
        {      
            ComputeTexturesRecursive( &atlas[ i ], *rootSurfaceNode );
            ComputeLightingLayer( &atlas[ i ], 
                &m_hTexIntermediateColor2, lastHeightCoverage, &atlas[ i ].texture );
        }
    }
}
