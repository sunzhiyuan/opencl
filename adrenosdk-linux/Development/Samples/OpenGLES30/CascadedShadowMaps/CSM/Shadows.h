//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#ifndef __SHADOWS_H__
#define __SHADOWS_H__

class Settings;

class BasicMesh;

enum { HARD = 0,
       PCF_SHOW_SPLITS,
	   PCF,
	   HARD_ARRAY,
	   ESM_OPTIMIZED,
       NUM_SHADERS };

typedef struct ShadowShader
{
	UINT32             m_hShaderId;
	INT32              m_hMVPLoc;
	INT32			   m_hMWorldLoc;
	INT32              m_hLightMVPLoc;//array of mat4 modelviewprojections[4];
	INT32              m_hOffsetsLoc;//array of vec3 offsets[4];
	INT32              m_hLightLoc;
	INT32			   m_hMNormalLoc;
	INT32			   m_hShadowMapLoc;

	INT32			   m_hShadowSpheresLoc;//array of vec4 spheres[4], xyz world space center, w R^2;

} SHADOWSHADER;

class Shadows {
		
	public:
		
		Shadows();
		virtual ~Shadows();

		bool initShaders();
		
		void destroy();
		
		void update(Settings *settings, FRMMATRIX4X4& invProj, FRMMATRIX4X4& ModelviewIn,   float* fv3SunDir);
		
		FRMVECTOR3 &getDirection()  ;
		
		FRMMATRIX4X4 *getProjections()  ;
		FRMMATRIX4X4 *getModelviews()  ;
		FRMMATRIX4X4 *getModelviewProjections()  ;
		FRMVECTOR3 *getOffsets()  ;
		
		FRMVECTOR4 *getSpheres() { return spheres; }
		
	private:
		
		int size;						// shadow map size
		
		float time;						// light time
		FRMVECTOR3 direction;					// light direction
		
		FRMMATRIX4X4 projections[4];			// projection matrices
		FRMMATRIX4X4 modelviews[4];				// modelview matrices
		FRMMATRIX4X4 modelviewprojections[4];	// modelviewprojection matrices

		FRMVECTOR3 offsets[4];				// modelview offsets
		
		FRMVECTOR4 spheres[4];				//xyz -center in world coords, w is radius squared. 
		
		int technique;					// last technique
		int shadows;					// last shadows
		

	public:

		SHADOWSHADER		m_hShaders[NUM_SHADERS];

};

#endif /* __SHADOWS_H__ */