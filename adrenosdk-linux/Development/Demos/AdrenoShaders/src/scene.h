//=============================================================================
// FILE: scene.h
//
// Create and render a scene
//
// AUTHOR:          QUALCOMM
//
//                  Copyright (c) 2008 QUALCOMM Incorporated.
//                              All Rights Reserved.
//                          QUALCOMM Proprietary/GTDR
//=============================================================================

#pragma once

// Common Sample Includes
#include "common.h"

// Framework includes
#include "FrmInput.h"
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>



//=============================================================================
// CONSTANTS
//=============================================================================
#define ROTATE_SPEED        0.025f


#define TIME_ONE_SECOND	    1000
#define PI                  3.141592653589793238462f
#define TWO_PI              6.283185307179586476925f

//=============================================================================
// HELPER MACROS
//=============================================================================
#define SETVEC3(P, X, Y, Z)     {P[0] = X; P[1] = Y; P[2] = Z;}
#define SETVEC4(P, X, Y, Z, W)  {P[0] = X; P[1] = Y; P[2] = Z; P[3] = W;}

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

//=============================================================================
// TYPEDEFS
//=============================================================================

//--------------------------------------------------------------------------------------
// Name: struct OffscreenRT
// Desc: Stores information and handles for off-screen render targets
//--------------------------------------------------------------------------------------
struct OffscreenRT
{
    OffscreenRT();
    ~OffscreenRT();
    BOOL CreateRT( UINT32 DesiredWidth, UINT32 DesiredHeight );
    VOID FreeRT();
    VOID SetFramebuffer();

    UINT32  BufferHandle;       // GL_FRAMEBUFFER
    UINT32  TextureHandle;      // GL_TEXTURE_2D
    UINT32  DepthTextureHandle;
    UINT32  Width;
    UINT32  Height;
};

//--------------------------------------------------------------------------------------
// Name: struct OffscreenRT4
// Desc: Stores information and handles for off-screen render targets
//--------------------------------------------------------------------------------------
struct OffscreenRT4
{
    OffscreenRT4();
    ~OffscreenRT4();
    BOOL CreateRT( UINT32 DesiredWidth, UINT32 DesiredHeight, BOOL CreateDepthBuffer, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE );
    VOID ShareDepthBuffer( UINT32 SourceDepthTexture );
    VOID FreeRT();
    VOID SetFramebuffer();
    VOID DetachFramebuffer();

    // frame buffer object
    UINT32  BufferHandle;

    // re-using buffers as samplers
    UINT32  TextureHandle;
    UINT32  DepthTextureHandle;
    BOOL    SharedDepthTexture;

    UINT32  Width;
    UINT32  Height;
};




//=============================================================================
// CScene Class
//=============================================================================
class CScene 
{
public:
    CScene(const CHAR* strName);
    virtual ~CScene();

     virtual BOOL Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture) = 0;
    virtual BOOL Resize() { return TRUE; }
    virtual VOID Destroy() = 0;
    virtual VOID Update() = 0;
    virtual VOID Render() = 0;

    // Functions common to multiple scenes
    BYTE*           LoadTGA( const CHAR* strFileName, UINT32* pWidth, UINT32* pHeight, UINT32* nFormat );
    VOID            SetTexture( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot );
    VOID            CheckOpenGL();
    BOOL            LoadCubeMap(const CHAR* ImageFileName);
    BOOL            CheckFrameBufferStatus();
    VOID            RenderScreenAlignedQuad();
	VOID			RenderScreenAlignedQuadInv();
    VOID            RenderScreenAlignedQuad( FRMVECTOR2& ScreenPos, const FLOAT32 Size );
    VOID            RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight );



private:

public:

    const CHAR* m_strName;
    INT32       m_nWidth;
    INT32       m_nHeight;


    FRM_INPUT   m_Input;

    GLuint      m_CubemapHandle;

};

