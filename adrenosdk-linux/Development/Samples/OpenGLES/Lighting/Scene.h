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

    CFrmFontGLES          m_Font;
    CFrmTimer             m_Timer;
    CFrmUserInterfaceGLES m_UserInterface;

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormal;
    CFrmMesh           m_Mesh;

    CFrmArcBall        m_ArcBall;
    FRMVECTOR4         m_qMeshRotation;
    FRMVECTOR4         m_qRotationDelta;

    CFrmTexture*       m_pLogoTexture;

    BOOL               m_bPerPixelLighting;
    const CHAR*        m_strLightingType;
    FRMVECTOR3         m_vLightPosition;
    INT32              m_nMaterialIndex;
    const CHAR*        m_strMaterialName;

    GLuint             m_hPerPixelLightingShader;
    GLint              m_hPPLModelViewMatrixLoc;
    GLint              m_hPPLModelViewProjMatrixLoc;
    GLint              m_hPPLNormalMatrixLoc;
    GLint              m_hPPLLightPositionLoc;
    GLint              m_hPPLMaterialAmbientLoc;
    GLint              m_hPPLMaterialDiffuseLoc;
    GLint              m_hPPLMaterialSpecularLoc;

    GLuint             m_hPerVertexLightingShader;
    GLint              m_hPVLModelViewMatrixLoc;
    GLint              m_hPVLModelViewProjMatrixLoc;
    GLint              m_hPVLNormalMatrixLoc;
    GLint              m_hPVLLightPositionLoc;
    GLint              m_hPVLMaterialAmbientLoc;
    GLint              m_hPVLMaterialDiffuseLoc;
    GLint              m_hPVLMaterialSpecularLoc;
};


#endif // SCENE_H
