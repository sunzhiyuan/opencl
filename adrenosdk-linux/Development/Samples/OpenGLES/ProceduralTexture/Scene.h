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

// The procedural texture types
enum PTType
{
    PT_CHECKER = 0,
    PT_WOOD,
    PT_MARBLE,
    PT_CLOUDS,
    NUM_PTS
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
    BOOL InitShaders();

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    BOOL               m_bShouldRotate;
    FLOAT32            m_fRotateTime;

    CFrmMesh           m_Mesh;

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormal;

    CFrmTexture*       m_pLogoTexture;

    // Shader related data
    UINT32             m_hCheckerShader;
    UINT32             m_nCheckerModelViewProjectionMatrixSlot;
    UINT32             m_nCheckerColor1Slot;
    UINT32             m_nCheckerColor2Slot;
    UINT32             m_nCheckerFrequencySlot;

    UINT32             m_hWoodShader;
    UINT32             m_nWoodModelViewProjectionMatrixSlot;
    UINT32             m_nWoodModelViewMatrixSlot;
    UINT32             m_nWoodNormalMatrixSlot;
    UINT32             m_nWoodScaleSlot;
    UINT32             m_nWoodKdSlot;
    UINT32             m_nWoodKsSlot;
    UINT32             m_nWoodDarkWoodColorSlot;
    UINT32             m_nWoodLightWoodColorSlot;
    UINT32             m_nWoodFrequencySlot;
    UINT32             m_nWoodNoiseScaleSlot;
    UINT32             m_nWoodRingScaleSlot;
    UINT32             m_nWoodLightDirSlot;

    UINT32             m_hMarbleShader;
    UINT32             m_nMarbleModelViewProjectionMatrixSlot;
    UINT32             m_nMarbleModelViewMatrixSlot;
    UINT32             m_nMarbleNormalMatrixSlot;
    UINT32             m_nMarbleScaleSlot;
    UINT32             m_nMarbleKdSlot;
    UINT32             m_nMarbleKsSlot;
    UINT32             m_nMarbleColor1Slot;
    UINT32             m_nMarbleColor2Slot;
    UINT32             m_nMarbleTurbScaleSlot;
    UINT32             m_nMarbleLightDirSlot;

    UINT32             m_hCloudsShader;
    UINT32             m_nCloudsModelViewProjectionMatrixSlot;
    UINT32             m_nCloudsScaleSlot;
    UINT32             m_nCloudsIntensitySlot;
    UINT32             m_nCloudsColor1Slot;
    UINT32             m_nCloudsColor2Slot;

    CFrmTexture3D*     m_pNoiseTexture;
    CFrmTexture3D*     m_pTurbTexture;

    UINT32             m_nPTIndex;
    FLOAT32            m_fCheckerFrequency;
    FLOAT32            m_fWoodFrequency;
    FLOAT32            m_MarbleTurbScale;

    // Shader related data
    UINT32             m_hProceduralShader;
    UINT32             m_modelViewMatrixLoc;
    UINT32             m_color1Loc;
    UINT32             m_color2Loc;
    UINT32             m_frequencyLoc;

    FRMVECTOR4         m_TextureColor1;
    FRMVECTOR4         m_TextureColor2;
    FLOAT32            m_fTextureFrequency;

    BOOL CreateNoiseTexture3D( UINT32 nDim );
    BOOL CreateTurbTexture3D( UINT32 nDim );

    VOID SetCheckerShader();
    VOID SetWoodShader();
    VOID SetMarbleShader();
    VOID SetCloudsShader();
};


#endif // SCENE_H
