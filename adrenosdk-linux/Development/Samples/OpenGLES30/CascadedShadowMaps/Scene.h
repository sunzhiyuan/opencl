//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: Port of ShaderX7 Practical CSM
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "BasicMesh.h"
#include "Mover.h"
#include "CSM/Shadows.h"
#include "CSM/Settings.h"

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

// enum of meshes
enum { CUBE_MESH = 0,
       SPHERE_MESH,
       BUMPY_SPHERE_MESH,
       TORUS_MESH,
       ROCKET_MESH,
       NUM_MESHES };

struct PreviewShader
{
	PreviewShader()
	{
		ShaderId = 0;
	}
	VOID Destroy()
	{
		if( ShaderId ) glDeleteProgram( ShaderId );
	}

	// shader
	UINT32  ShaderId;

	// shader variables
	INT32   ColorTextureId;
};
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

	// Our own version of FrmRenderTextureToScreen_GLES(..) only for texture arrays
	VOID RenderTextureArrayToScreen_GLES( FLOAT32 sx, FLOAT32 sy, FLOAT32 w, FLOAT32 h,
										UINT32 hTexture,
										INT32 hShaderProgram, INT32 hScreenSizeLoc )
	{
		struct { GLint x, y, width, height; } viewport;
		glGetIntegerv( GL_VIEWPORT, (GLint*)&viewport );

		FLOAT32 fTextureWidth  = w;
		FLOAT32 fTextureHeight = h;
		FLOAT32 fScreenWidth   = (FLOAT32)viewport.width;
		FLOAT32 fScreenHeight  = (FLOAT32)viewport.height;

		if( sx < 0.0f ) sx += fScreenWidth  - fTextureWidth;
		if( sy < 0.0f ) sy += fScreenHeight - fTextureHeight;

		sx = FrmFloor( sx );
		sy = FrmFloor( sy );

		// Set the geoemtry
		FRMVECTOR4 vQuad[] = 
		{ 
			//          Screenspace x     Screenspace y      tu    tv
			FRMVECTOR4( sx,               sy,                0.0f, 1.0f ),
			FRMVECTOR4( sx,               sy+fTextureHeight, 0.0f, 0.0f ),
			FRMVECTOR4( sx+fTextureWidth, sy+fTextureHeight, 1.0f, 0.0f ),
			FRMVECTOR4( sx+fTextureWidth, sy,                1.0f, 1.0f ),
		};
		glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, vQuad );
		glEnableVertexAttribArray( 0 );

		// Set the texture
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D_ARRAY, hTexture );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );

		// Set the shader program
		glUseProgram( hShaderProgram );
		glUniform2f( hScreenSizeLoc, fScreenWidth, fScreenHeight );

		// Draw the quad
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );

		glEnable( GL_DEPTH_TEST );
		glDisable( GL_BLEND );
	}

	VOID DrawGround();
	VOID RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight );

	VOID PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle );

	VOID SetTexture( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot );

	BOOL CreateFBO( UINT32 nWidth, UINT32 nHeight, UINT32 nFormat, UINT32 nType, UINT32 nInternalFormat, FrameBufferObject** ppFBO );

	VOID DestroyFBO( FrameBufferObject* pFBO );

	VOID CreateShadowMapFBO();
	VOID CreateShadowMapFBOArray();

	VOID BeginFBO( FrameBufferObject* pFBO );
	VOID BeginFBO( FrameBufferObject* pFBO, INT32 layer );

	VOID EndFBO( FrameBufferObject* pFBO );

    BOOL InitShaders();

    VOID CheckFrameBufferStatus();

    VOID FreeMeshes();

    VOID RenderScreenAlignedQuad();

    VOID RenderProjectedShadowMapToFBO();
    VOID RenderBlendedLightingAndShadowMap();

    CFrmFontGLES           m_Font;
    CFrmUserInterfaceGLES  m_UserInterface;
    CFrmTimer              m_Timer;

	static const int   MODEL_DIMENSION = 5;
	static const int sizeShadow = 512;

    // Declare shader variables
    UINT32             m_hOverlayShader;
	UINT32			   m_hOverlayArrayShader;

    UINT32             m_hOverlayScreenSizeLoc;

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;

    // Declare transformation related variables
    FRMMATRIX4X4       m_matCameraPersp;
    FRMMATRIX4X4       m_matCameraView;

    FRMMATRIX4X4       m_matCameraMeshModelView;
    FRMMATRIX4X4       m_matCameraMeshModelViewProj;
    FRMMATRIX3X3       m_matCameraMeshNormal;

	FRMMATRIX4X4	   m_ModelMtxs [ MODEL_DIMENSION * MODEL_DIMENSION ];

    FRMVECTOR3         m_vLightLookAt;
    FRMVECTOR3         m_vLightUp;

    // Declare user controled variables
    FRMVECTOR3         m_vSunDirection;
	PreviewShader      m_PreviewShader;

	BasicMesh		   m_Meshes[ NUM_MESHES ];

    UINT32             m_nCurMeshIndex;
	UINT32             m_nCurShaderIndex;

	Mover mover;//camera mover

	Shadows			   m_ShadowMan; //shadow manager
	Settings		   m_ShadowSettings;

	UINT32             m_hDepthShader;
    INT32              m_hDepthMVPLoc;

	bool			   m_Preview;

	FrameBufferObject*         m_DebugRTs[2];

};


#endif // SCENE_H
