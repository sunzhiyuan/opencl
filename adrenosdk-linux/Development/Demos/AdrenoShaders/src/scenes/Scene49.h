//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#pragma once

// The Parent class
#include "../scene.h"


//--------------------------------------------------------------------------------------
// Name: struct MaterialShader49
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct MaterialShader49
{
    MaterialShader49();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   TextureId;
    INT32   ExposureId;
};


//--------------------------------------------------------------------------------------
// Name: class CSample49
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample49 : public CScene
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
    virtual BOOL Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture);
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample49( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawScene();

    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmTexture*            m_pLogoTexture;
    CFrmTexture*            m_HDRTexture[2];
    MaterialShader49          m_MaterialShader;
    FLOAT32                 m_Exposure;
};

