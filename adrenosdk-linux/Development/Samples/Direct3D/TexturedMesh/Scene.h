//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
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
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample( const CHAR* strName );

private:
    struct AppConstantBuffer
    {
        FRMMATRIX4X4 matModelViewProj;        
    };

    FRMMATRIX4X4 m_matModelView;
    FRMMATRIX4X4 m_matProj;

    CFrmFontD3D m_Font;
    CFrmUserInterfaceD3D m_UserInterface;
    CFrmShaderProgramD3D m_shader;
    CFrmConstantBuffer* m_constantBuffer;
    CFrmTexture* m_pTexture;
    CFrmTexture* m_pLogoTexture;
    CFrmMesh m_mesh;
    CFrmTimer m_Timer;    
    AppConstantBuffer m_constantBufferData;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;
};


#endif // SCENE_H
