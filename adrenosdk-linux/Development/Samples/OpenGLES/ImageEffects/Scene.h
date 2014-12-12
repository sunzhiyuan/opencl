//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H


// The various image effects
enum EffectType
{
    EFFECT_NONE = 0,
    EFFECT_NOISE,
    EFFECT_BLUR,
    EFFECT_SHARPEN,
    EFFECT_STATIC,
    EFFECT_SEPIA,
    EFFECT_SOBEL,
    EFFECT_EMBOSS,
    EFFECT_LENS,
    EFFECT_WARP,
    NUM_EFFECTS
};


//--------------------------------------------------------------------------------------
// Effect names
//--------------------------------------------------------------------------------------
const CHAR* g_pEffectNames[ NUM_EFFECTS ] = 
{ 
    "No Effect",
    "Noise",
    "Blur",
    "Sharpen",
    "Static",
    "Sepia",
    "Sobel",
    "Emboss",
    "Lens",
    "Warp"
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
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

private:
    BOOL InitShaders();

    BOOL CreateRandomTexture3D( UINT32 nDim );
    BOOL CreateNoiseTexture2D( UINT32 nDim );
    BOOL CreateNoiseTexture3D( UINT32 nDim );

    VOID SetTextureShader();
    VOID SetNoiseShader();
    VOID SetBlurShader();
    VOID SetSharpenShader();
    VOID SetStaticShader();
    VOID SetSepiaShader();
    VOID SetSobelShader();
    VOID SetEmbossShader();
    VOID SetLensShader();
    VOID SetWarpShader();

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    FLOAT32                m_fElapsedTime;
    CFrmUserInterfaceGLES  m_UserInterface;

    GLuint             m_hTextureShader;

    GLuint             m_hStaticShader;
    GLint              m_nStaticImageTextureSlot;
    GLint              m_nStaticRandomTextureSlot;
    GLint              m_nStaticScaleSlot;
    GLint              m_nStaticFrequencySlot;
    GLint              m_nStaticTimeSlot;

    GLuint             m_hNoiseShader;
    GLint              m_nNoiseImageTextureSlot;
    GLint              m_nNoiseTextureSlot;
    GLint              m_nNoiseScaleSlot;

    GLuint             m_hBlurShader;
    GLint              m_nBlurStepSizeXSlot;
    GLint              m_nBlurStepSizeYSlot;

    GLuint             m_hSharpenShader;
    GLint              m_nSharpenStepSizeXSlot;
    GLint              m_nSharpenStepSizeYSlot;

    GLuint             m_hSepiaShader;
    GLint              m_nSepiaImageTextureSlot;
    GLint              m_nSepiaLookupTextureSlot;

    GLuint             m_hSobelShader;
    GLint              m_nSobelHalfStepSizeXSlot;
    GLint              m_nSobelHalfStepSizeYSlot;

    GLuint             m_hEmbossShader;
    GLint              m_nEmbossHalfStepSizeXSlot;
    GLint              m_nEmbossHalfStepSizeYSlot;

    GLuint             m_hLensShader;
    GLint              m_nLensPositionSlot;

    GLuint             m_hWarpShader;
    GLint              m_nWarpImageTextureSlot;
    GLint              m_nWarpNoiseTextureSlot;
    GLint              m_nWarpTimeSlot;
    GLint              m_nWarpScaleSlot;

    const CHAR*        m_strEffectName;
    UINT32             m_nEffectIndex;
    BOOL               m_bShowLine;

    CFrmTexture*       m_pTexture;
    CFrmTexture*       m_pTextureWrap;
    CFrmTexture*       m_pSepiaTexture;
    CFrmTexture*       m_pNoiseTexture2D;

    CFrmTexture3D*     m_pRandomTexture3D;
    CFrmTexture3D*     m_pNoiseTexture3D;

    FLOAT32            m_fStepSize;
    FLOAT32            m_fHalfStepSize;

    CFrmTexture*       m_pLogoTexture;
};


#endif // SCENE_H
