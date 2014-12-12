//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: Skydome
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Mover.h"
#include "Scatter\HoffmanScatter.hpp"
#include "Scene\SkyDome.hpp"
#include "Scene\StaticTerrain.hpp"

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

	float DecreaseParam(float in) 
	{
		in -= 0.0001f;
		if(in < 0.0f)
			in = 0.f;

		return in;
	}
	float IncreaseParam(float in) 
	{
		in += 0.0001f;
		if(in > 4.0f)
			in = 4.f;

		return in;
	}
    BOOL InitShaders();

    CFrmFontD3D            m_Font;
    CFrmUserInterfaceD3D   m_UserInterface;
    CFrmTimer              m_Timer;

    // Declare shader variables

	float			   m_shaderParamsArray[10];

    INT32              m_hModelViewMatrixLoc;
    INT32              m_hModelViewProjMatrixLoc;
    INT32              m_hLightPositionLoc;
    INT32              m_hMaterialAmbientLoc;
    INT32              m_hMaterialDiffuseLoc;
    INT32              m_hMaterialSpecularLoc;    

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;
	UINT32             m_nCurShaderIndex;

    // Declare transformation related variables
    FRMMATRIX4X4       m_matCameraPersp;

    FRMMATRIX4X4       m_matCameraView;

    FRMMATRIX4X4       m_matCameraMeshModelView;
    FRMMATRIX4X4       m_matCameraMeshModelViewProj;
    FRMMATRIX3X3       m_matCameraMeshNormal;

    FRMVECTOR3         m_vLightLookAt;
    FRMVECTOR3         m_vLightUp;

    // Declare user controled variables
    FRMVECTOR3         m_vLightPosition;

	Mover mover;//camera mover

	HoffmanScatter	   m_HoffmanScatter;
	SkyDome			   m_SkyDome;
	StaticTerrain      m_terrain;

	bool			   m_usingHoffman;//if false, use Preetham

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;

};


#endif // SCENE_H
