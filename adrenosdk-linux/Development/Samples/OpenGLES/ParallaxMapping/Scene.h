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

// enum of floor textures
enum { PARQ_TEXTURE=0,
       ROCKY_TEXTURE,
       BRICK1_TEXTURE,
       BRICK2_TEXTURE,
       NUM_TEXTURES };

// enum of texture mapping modes
enum { TEXTURE_MODE=0,
       BUMP_MODE,
       PARALLAX_MODE,
       PARALLAX_AND_BUMP_MODE,
       NUM_MODES };

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
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

private:
    BOOL InitShaders();

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    UINT32             m_hParallaxMappingShader;
    INT32              m_hParallaxMappingMVPLoc;
    INT32              m_hParallaxMappingMLoc;
    INT32              m_hParallaxMappingNormalLoc;
    INT32              m_hParallaxMappingViewPositionLoc;
    INT32              m_hParallaxMappingLightPositionLoc;
    INT32              m_hParallaxMappingTextureScaleLoc;
    INT32              m_hParallaxMappingBaseTextureLoc;
    INT32              m_hParallaxMappingBumpTextureLoc;
    INT32              m_hParallaxMappingParallaxLoc;
    INT32              m_hParallaxMappingScaleLoc;
    INT32              m_hParallaxMappingAmbientLoc;
    INT32              m_hParallaxMappingDiffuseLoc;
    INT32              m_hParallaxMappingSpecularLoc;
    INT32              m_hParallaxMappingModeLoc;

    CFrmTexture*       m_pLogoTexture;
    CFrmTexture*       m_pBaseTexture[ NUM_TEXTURES ];
    CFrmTexture*       m_pBumpTexture[ NUM_TEXTURES ];

    CFrmMesh           m_Floor;

    FRMMATRIX4X4       m_matPersp;
    FRMMATRIX4X4       m_matFloorModel;
    FRMMATRIX4X4       m_matFloorModelViewProj;
    FRMMATRIX3X3       m_matNormal;

    FRMVECTOR3         m_vViewPosition;

    FRMVECTOR4         m_vAmbient;
    FRMVECTOR4         m_vDiffuse;
    FRMVECTOR4         m_vSpecular;

    FLOAT32            m_fParallax;
    FLOAT32            m_fScale;

    INT32              m_nMode;
    INT32              m_nTextureId;

    FLOAT32            m_fRotateX;
    FLOAT32            m_fRotateY;

    BOOL               m_bBumpMappingOn;
    BOOL               m_bParallaxMappingOn;
};


#endif // SCENE_H
