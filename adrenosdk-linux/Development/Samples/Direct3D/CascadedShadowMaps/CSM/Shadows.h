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

/*
 */
enum { HARD = 0,
       PCF_SHOW_SPLITS,
       PCF,
       ESM_OPTIMIZED,
       NUM_SHADERS };

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

        CFrmShaderProgramD3D		m_Shaders[NUM_SHADERS];
        
        struct CSMConstantBuffer
        {
            FRMMATRIX4X4 matModelViewProj;
            FRMMATRIX4X4 matWorld;
            FRMMATRIX4X4 modelviewprojections[4];
            FRMMATRIX4X4 matNormal;
            FRMVECTOR4   offsets[4];
            FRMVECTOR4   shadowsSpheres[4];
            FRMVECTOR4   light;            
            FLOAT32      fBias;
            FRMVECTOR3   Padding; // Pad to multiple of 16-bytes
        };

        CFrmConstantBuffer  *m_pCSMConstantBuffer;
        CSMConstantBuffer   m_CSMConstantBufferData;
};


#endif /* __SHADOWS_H__ */
