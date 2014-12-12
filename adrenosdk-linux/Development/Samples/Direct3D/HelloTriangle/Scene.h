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
        FRMMATRIX4X4 Perspective;
        FRMMATRIX4X4 View;
    };

    CFrmShaderProgramD3D m_shader;
    CFrmVertexBuffer *m_vertexBuffer;
    CFrmIndexBuffer *m_indexBuffer;
    CFrmConstantBuffer *m_constantBuffer;
    CFrmTexture *m_texture;
    CFrmTimer m_Timer;    
    AppConstantBuffer m_constantBufferData;
};


#endif // SCENE_H
