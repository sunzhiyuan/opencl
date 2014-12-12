//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#include "FrmPlatform.h"
#include "FrmMath.h"
#include "Bounds.h"
#include "Settings.h"
#include "Shadows.h"

#include "../BasicMesh.h"

#include <OpenGLES/FrmGLES3.h>
#include <OpenGLES/FrmShader.h>

/*
 */
Shadows::Shadows() {
	
	size = 512;
	
	time = 0.0f;
	direction = FRMVECTOR3(0.0f, 1.0f,0.0f);
	
	technique = -1;
	shadows = -1;
}

Shadows::~Shadows() {
	
	destroy();
}

bool Shadows::initShaders() {
	
	FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "g_vPositionOS", FRM_VERTEX_POSITION },
        { "g_vNormalOS",   FRM_VERTEX_NORMAL }
    };

    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CSM.vs",
                                                  "Samples/Shaders/Hard.fs",
												  &m_hShaders[HARD].m_hShaderId,pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CSM.vs",
                                                  "Samples/Shaders/PCFShowSplits.fs",
												  &m_hShaders[PCF_SHOW_SPLITS].m_hShaderId,pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CSM.vs",
                                                  "Samples/Shaders/PCF.fs",
												  &m_hShaders[PCF].m_hShaderId,pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
	if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CSM.vs",
                                                  "Samples/Shaders/HardArray.fs",
												  &m_hShaders[HARD_ARRAY].m_hShaderId,pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}
		if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/CSMOptimized.vs",
                                                  "Samples/Shaders/OptimizedExp.fs",
												  &m_hShaders[ESM_OPTIMIZED].m_hShaderId,pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

	for(int i=0; i<NUM_SHADERS; ++i)
	{
		m_hShaders[i].m_hMVPLoc = glGetUniformLocation( m_hShaders[i].m_hShaderId, "g_matModelViewProj" );
		m_hShaders[i].m_hMWorldLoc = glGetUniformLocation( m_hShaders[i].m_hShaderId, "g_matWorld" );
		m_hShaders[i].m_hLightMVPLoc = glGetUniformLocation( m_hShaders[i].m_hShaderId, "modelviewprojections" );
		m_hShaders[i].m_hOffsetsLoc = glGetUniformLocation( m_hShaders[i].m_hShaderId, "offsets" );
		m_hShaders[i].m_hLightLoc = glGetUniformLocation( m_hShaders[i].m_hShaderId, "light" );
		m_hShaders[i].m_hMNormalLoc = glGetUniformLocation( m_hShaders[i].m_hShaderId, "g_matNormal" );
		m_hShaders[i].m_hShadowMapLoc = glGetUniformLocation( m_hShaders[i].m_hShaderId, "s_texture_8" );
	}

	m_hShaders[ESM_OPTIMIZED].m_hShadowSpheresLoc = glGetUniformLocation( m_hShaders[ESM_OPTIMIZED].m_hShaderId, "gShadowSpheres" );

	return TRUE;
}
/******************************************************************************\
*
* Render destroy
*
\******************************************************************************/

/*
 */
void Shadows::destroy() {
	
	technique = -1;
	shadows = -1;
	
	for(int i = 0; i < NUM_SHADERS; i++) {
		glDeleteProgram(m_hShaders[i].m_hShaderId);
	}
}

//Note: Due to issues with the FRMMATRIX inverse, I'm dumping my own here (passed as float ptrs)
//Note that ret must already be allocated to 16 floats before calling this.
float *inverse(float *ret,const float *m) {
	float src[16];
	float temp[12];
	src[0]  = m[0];
	src[1]  = m[4];
	src[2]  = m[8];
	src[3]  = m[12];
	src[4]  = m[1];
	src[5]  = m[5];
	src[6]  = m[9];
	src[7]  = m[13];
	src[8]  = m[2];
	src[9]  = m[6];
	src[10] = m[10];
	src[11] = m[14];
	src[12] = m[3];
	src[13] = m[7];
	src[14] = m[11];
	src[15] = m[15];
	temp[0]  = src[10] * src[15];
	temp[1]  = src[11] * src[14];
	temp[2]  = src[9]  * src[15];
	temp[3]  = src[11] * src[13];
	temp[4]  = src[9]  * src[14];
	temp[5]  = src[10] * src[13];
	temp[6]  = src[8]  * src[15];
	temp[7]  = src[11] * src[12];
	temp[8]  = src[8]  * src[14];
	temp[9]  = src[10] * src[12];
	temp[10] = src[8]  * src[13];
	temp[11] = src[9]  * src[12];
	ret[0]  = temp[0] * src[5] + temp[3] * src[6] + temp[4]  * src[7];
	ret[0] -= temp[1] * src[5] + temp[2] * src[6] + temp[5]  * src[7];
	ret[1]  = temp[1] * src[4] + temp[6] * src[6] + temp[9]  * src[7];
	ret[1] -= temp[0] * src[4] + temp[7] * src[6] + temp[8]  * src[7];
	ret[2]  = temp[2] * src[4] + temp[7] * src[5] + temp[10] * src[7];
	ret[2] -= temp[3] * src[4] + temp[6] * src[5] + temp[11] * src[7];
	ret[3]  = temp[5] * src[4] + temp[8] * src[5] + temp[11] * src[6];
	ret[3] -= temp[4] * src[4] + temp[9] * src[5] + temp[10] * src[6];
	ret[4]  = temp[1] * src[1] + temp[2] * src[2] + temp[5]  * src[3];
	ret[4] -= temp[0] * src[1] + temp[3] * src[2] + temp[4]  * src[3];
	ret[5]  = temp[0] * src[0] + temp[7] * src[2] + temp[8]  * src[3];
	ret[5] -= temp[1] * src[0] + temp[6] * src[2] + temp[9]  * src[3];
	ret[6]  = temp[3] * src[0] + temp[6] * src[1] + temp[11] * src[3];
	ret[6] -= temp[2] * src[0] + temp[7] * src[1] + temp[10] * src[3];
	ret[7]  = temp[4] * src[0] + temp[9] * src[1] + temp[10] * src[2];
	ret[7] -= temp[5] * src[0] + temp[8] * src[1] + temp[11] * src[2];
	temp[0]  = src[2] * src[7];
	temp[1]  = src[3] * src[6];
	temp[2]  = src[1] * src[7];
	temp[3]  = src[3] * src[5];
	temp[4]  = src[1] * src[6];
	temp[5]  = src[2] * src[5];
	temp[6]  = src[0] * src[7];
	temp[7]  = src[3] * src[4];
	temp[8]  = src[0] * src[6];
	temp[9]  = src[2] * src[4];
	temp[10] = src[0] * src[5];
	temp[11] = src[1] * src[4];
	ret[8]   = temp[0]  * src[13] + temp[3]  * src[14] + temp[4]  * src[15];
	ret[8]  -= temp[1]  * src[13] + temp[2]  * src[14] + temp[5]  * src[15];
	ret[9]   = temp[1]  * src[12] + temp[6]  * src[14] + temp[9]  * src[15];
	ret[9]  -= temp[0]  * src[12] + temp[7]  * src[14] + temp[8]  * src[15];
	ret[10]  = temp[2]  * src[12] + temp[7]  * src[13] + temp[10] * src[15];
	ret[10] -= temp[3]  * src[12] + temp[6]  * src[13] + temp[11] * src[15];
	ret[11]  = temp[5]  * src[12] + temp[8]  * src[13] + temp[11] * src[14];
	ret[11] -= temp[4]  * src[12] + temp[9]  * src[13] + temp[10] * src[14];
	ret[12]  = temp[2]  * src[10] + temp[5]  * src[11] + temp[1]  * src[9];
	ret[12] -= temp[4]  * src[11] + temp[0]  * src[9]  + temp[3]  * src[10];
	ret[13]  = temp[8]  * src[11] + temp[0]  * src[8]  + temp[7]  * src[10];
	ret[13] -= temp[6]  * src[10] + temp[9]  * src[11] + temp[1]  * src[8];
	ret[14]  = temp[6]  * src[9]  + temp[11] * src[11] + temp[3]  * src[8];
	ret[14] -= temp[10] * src[11] + temp[2]  * src[8]  + temp[7]  * src[9];
	ret[15]  = temp[10] * src[10] + temp[4]  * src[8]  + temp[9]  * src[9];
	ret[15] -= temp[8]  * src[9]  + temp[11] * src[10] + temp[5]  * src[8];
	float det = src[0] * ret[0] + src[1] * ret[1] + src[2] * ret[2] + src[3] * ret[3];
	float idet = 1.0f / det;
	ret[0]  *= idet;
	ret[1]  *= idet;
	ret[2]  *= idet;
	ret[3]  *= idet;
	ret[4]  *= idet;
	ret[5]  *= idet;
	ret[6]  *= idet;
	ret[7]  *= idet;
	ret[8]  *= idet;
	ret[9]  *= idet;
	ret[10] *= idet;
	ret[11] *= idet;
	ret[12] *= idet;
	ret[13] *= idet;
	ret[14] *= idet;
	ret[15] *= idet;
	return ret;
}
void ortho(float* ret, float l,float r,float b,float t,float n,float f) {
	float rl = r - l;
	float tb = t - b;
	float fn = f - n;
	ret[0] = 2.0f / rl; ret[4] = 0.0f;      ret[8] = 0.0f;        ret[12] = -(r + l) / rl;
	ret[1] = 0.0f;      ret[5] = 2.0f / tb; ret[9] = 0.0f;        ret[13] = -(t + b) / tb;
	ret[2] = 0.0f;      ret[6] = 0.0f;      ret[10] = -2.0f / fn; ret[14] = -(f + n) / fn;
	ret[3] = 0.0f;      ret[7] = 0.0f;      ret[11] = 0.0f;       ret[15] = 1.0f;
}

/*
 */
void Shadows::update(Settings *settings, FRMMATRIX4X4& projIn, FRMMATRIX4X4& ModelviewIn, float* fv3SunDirIn) {
	
	direction = FRMVECTOR3(fv3SunDirIn[0], fv3SunDirIn[1], fv3SunDirIn[2]);
	
	// update pssm matrices
	float znear = 1.0f;
	float zfar = 250.0f;
	float shadowRange = 200.0f;
	float shadow_distribute = 0.1f;//25f;

	FRMMATRIX4X4 invProj;
	inverse( (float *)&invProj, (float *)&projIn );

	//hack in a fast inverse
	FRMMATRIX4X4 imodelview;
	inverse((float *)& imodelview, (float *)& ModelviewIn);//inverse(mat4(fMat4ModelviewIn));//inverse(camera->getModelview());

	// ortho basis (incorrect solution for vertical direction)
	FRMVECTOR3 zAxis = FRMVECTOR3(0.0f,0.0f,1.0f);
	FRMVECTOR3 side = FrmVector3Cross(zAxis, direction);
	side = FrmVector3Normalize (side);
	FRMVECTOR3 up = FrmVector3Cross(direction, side);
	up = FrmVector3Normalize(up);
	//vec3 side = normalize(cross(direction, vec3(0.0f,0.0f,1.0f)));
	//vec3 up = normalize(cross(direction, side));
	
	// bound frustum points
	FRMVECTOR3 points[8] = {
		FRMVECTOR3(-1.0f,-1.0f,-1.0f), FRMVECTOR3(1.0f,-1.0f,-1.0f), FRMVECTOR3(-1.0f,1.0f,-1.0f), FRMVECTOR3(1.0f,1.0f,-1.0f),
		FRMVECTOR3(-1.0f,-1.0f, 1.0f), FRMVECTOR3(1.0f,-1.0f, 1.0f), FRMVECTOR3(-1.0f,1.0f, 1.0f), FRMVECTOR3(1.0f,1.0f, 1.0f),
	};
	
	// world space bound frustum points
	for(int i = 0; i < 8; i++) {
		FRMVECTOR4 point = FRMVECTOR4( points[i].x, points[i].y, points[i].z, 1.0f );
		point = FrmVector4Transform( point, invProj );
		points[i] = FRMVECTOR3( point.x, point.y, point.z ) / point.w;
	}
	
	// world space bound frustum directions
	FRMVECTOR3 directions[4];
	for(int i = 0; i < 4; i++) {
		directions[i] = points[i + 4] - points[i];
		directions[i] = FrmVector3Normalize( directions[i] );
	}
	
	// split bound frustum
	for(int i = 0; i < 4; i++) {
		
		float k0 = (float)(i + 0) / 4;
		float k1 = (float)(i + 1) / 4;
		float fmin = FrmLerp(shadow_distribute, znear * powf(zfar / znear,k0),znear + (zfar - znear) * k0);
		float fmax = FrmLerp(shadow_distribute, znear * powf(zfar / znear,k1),znear + (zfar - znear) * k1);
		
		// none flickering removal
		if( settings->getFlickering() == Settings::FLICKERING_NONE) 
		{
			
			BoundBox bb;
			for(int j = 0; j < 4; j++) {
				FRMVECTOR3 v;
				v = points[j] + (directions[j] * fmin);
				bb.expand(v);
				v = points[j] + (directions[j] * fmax);
				bb.expand(v);
			}
			BoundSphere bs(bb);
			

			//save out radius squared
			spheres[i] = FRMVECTOR4( FrmVector3TransformCoord( bs.getCenter(), imodelview), bs.getRadius() * bs.getRadius());

			FRMVECTOR3 target = FrmVector3TransformCoord( bs.getCenter(), imodelview );
			
			ortho( (float*)&projections[i], bs.getRadius(),-bs.getRadius(),bs.getRadius(),-bs.getRadius(), 1.1f, shadowRange);
			FRMVECTOR3 position = target + (direction * shadowRange / 2.0f);
			FRMVECTOR3 lookat = target - (direction * shadowRange / 2.0f);
			modelviews[i] = FrmMatrixLookAtRH(position,lookat,up);
			
		}
		// exact flickering removal
		else
		{
			BoundBox bb;
			for(int j = 0; j < 4; j++) {
				FRMVECTOR3 v;
				v = points[j] + (directions[j] * fmin);
				bb.expand(v);
				v = points[j] + (directions[j] * fmax);
				bb.expand(v);
			}
			BoundSphere bs(bb);

			//save out radius squared
			spheres[i] = FRMVECTOR4( FrmVector3TransformCoord( bs.getCenter(), imodelview ), bs.getRadius() * bs.getRadius());
			
			float half_size = size / 2.0f;

			FRMVECTOR3 target = FrmVector3TransformCoord( bs.getCenter(), imodelview );

			float x = ceilf( FrmVector3Dot(target,up) * half_size / bs.getRadius()) * bs.getRadius() / half_size;
			float y = ceilf( FrmVector3Dot(target,side) * half_size / bs.getRadius()) * bs.getRadius() / half_size;
			target = up * x + side * y + (direction * FrmVector3Dot(target,direction));

			ortho( (float*)&projections[i],bs.getRadius(),-bs.getRadius(),bs.getRadius(),-bs.getRadius(), 1.1f, shadowRange);
			FRMVECTOR3 position = target + (direction * shadowRange / 2.0f);
			FRMVECTOR3 lookat = target - (direction * shadowRange / 2.0f);
			modelviews[i] = FrmMatrixLookAtRH(position,lookat,up);
		}

		int flipped = 0;

		modelviewprojections[i] = FrmMatrixMultiply( modelviews[i], projections[i]);

		if(settings->getTechnique() == Settings::TECHNIQUE_ATLAS) 
		{
			FRMMATRIX4X4 scale = FrmMatrixScale(0.25f,(flipped) ? -0.25f : 0.25f,0.5f);
			FRMMATRIX4X4 trans1 = FrmMatrixTranslate(0.25f,0.25f,0.5f);
			FRMMATRIX4X4 trans2 = FrmMatrixTranslate(0.5f * (i % 2),0.5f * (i / 2),0.0f);
			modelviewprojections[i] = FrmMatrixMultiply( modelviewprojections[i], scale );
			modelviewprojections[i] = FrmMatrixMultiply( modelviewprojections[i], trans1 );
			modelviewprojections[i] = FrmMatrixMultiply( modelviewprojections[i], trans2 );
		}
		else 
		{
			FRMMATRIX4X4 scale = FrmMatrixScale(0.5f,(flipped) ? -0.5f : 0.5f,0.5f);
			FRMMATRIX4X4 trans = FrmMatrixTranslate(0.5f,0.5f,0.5f);
			modelviewprojections[i] = FrmMatrixMultiply( modelviewprojections[i], scale );
			modelviewprojections[i] = FrmMatrixMultiply( modelviewprojections[i], trans );

		}

		FRMMATRIX4X4 invModel;

		inverse((float *)& invModel, (float *)& modelviews[i]);
		offsets[i] = FRMVECTOR3( invModel.M(3, 0), invModel.M(3, 1), invModel.M(3, 2));
	}
}



/******************************************************************************\
*
* Parameters
*
\******************************************************************************/

/*
 */
FRMVECTOR3 &Shadows::getDirection(){
	return direction;
}

/*
 */
FRMMATRIX4X4 *Shadows::getProjections() {
	return projections;
}

FRMMATRIX4X4 *Shadows::getModelviews() {
	return modelviews;
}

FRMMATRIX4X4 *Shadows::getModelviewProjections() {
	return modelviewprojections;
}

FRMVECTOR3 *Shadows::getOffsets() {
	return offsets;
}

/*
 */

