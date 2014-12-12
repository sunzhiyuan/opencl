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
// Name: struct RGBPassThroughShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct RGBPassThroughShader
{
    RGBPassThroughShader();
    VOID Destroy();

    // shader
    UINT32  ShaderId;

    // shader variables
    INT32   TextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct CIEShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct CIEShader
{
    CIEShader();
    VOID Destroy();

    // RGB to CIE
    UINT32  RGBtoCIEShaderId;
    INT32   TextureId;

    // CIE to RGB
    UINT32  CIEtoRGBShaderId;
    INT32   CIETextureId;
};


//--------------------------------------------------------------------------------------
// Name: struct HSVShader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct HSVShader
{
    HSVShader();
    VOID Destroy();

    // RGB to HSV
    UINT32  RGBtoHSVShaderId;
    INT32   TextureId;

    // HSV to RGB
    UINT32  HSVtoRGBShaderId;
    INT32   HSVTextureId;
    INT32   HSVParamsId;
    float   SaturationScale;
    float   HueBias;
};


//--------------------------------------------------------------------------------------
// Name: struct L16Shader
// Desc: Contains shader and shader variable handles
//--------------------------------------------------------------------------------------
struct L16Shader
{
    L16Shader();
    VOID Destroy();

    // RGB to CIE
    UINT32  RGBtoL16ShaderId;
    INT32   TextureId;

    // CIE to RGB
    UINT32  L16toRGBShaderId;
    INT32   L16TextureId;
};


//--------------------------------------------------------------------------------------
// Name: class CSample50
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample50 : public CScene
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

    CSample50( const CHAR* strName );

private:    
    BOOL InitShaders();
    VOID DrawScene();
    VOID DisplayColorSpaceName();
    VOID DisplayColorSpaceHelp();
    VOID ProcessHSVInput( FLOAT32 ElapsedTime, UINT32 Buttons, UINT32 PressedButtons );
    VOID ResetSettings();
    VOID DrawRGB();
    VOID DrawHSV();
    VOID DrawCIE();
    VOID DrawL16();

    OffscreenRT4             m_Backbuffer;
    CFrmFontGLES*            m_pFont;
    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;
    CFrmTexture*            m_pLogoTexture;
    INT32                   m_ColorSpace;
    CFrmTexture*            m_ColorTexture;
    RGBPassThroughShader    m_RGBPassThroughShader;
    CIEShader               m_CIEShader;
    HSVShader               m_HSVShader;
    L16Shader               m_L16Shader;
};
