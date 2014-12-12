//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
//
//   Liquid
//

#define ABS(a)	   (((a) < 0) ? -(a) : (a))

class Liquid
{
public:
	Liquid();
	~Liquid();

	void Init (int xres, int yres);
	void Init (int xres, int yres, float * pHeight, float * pVx, float * pVy, char* norm);
	void Init (int xres, int yres, char* norm);
	void Destroy ();
	void ResetValues();
	void Advect(float tdelt, float* pField, float dx, float dy);
	void UpdateHeight(float tdelt);
	void UpdateVelocities(float tdelt);
	void UpdateBoundaries(float tdelt);
	void CalcNormals();
	void Finger(float x, float y);
	float blerp(float* pField, float x, float y);
	void Step(int iter = 0);
	const void* CurrentHeight();
	const void* CurrentNorm();
	float Volume();

	int			m_cur;
	int			m_xres;
	int			m_yres;
	float		m_volume;
	float		m_initVolume;
	float		m_test;
	long		m_iframe;

	float		m_xoffset;
	float		m_yoffset;
	float		m_xscale;
	float		m_yscale;

private:

	int			m_nex;

	float *		m_height;
	float *		m_vx;
	float *		m_vy;
	char  *		m_norm;

	float		m_grav;
	float		m_recipxdelt;
	float		m_recipydelt;

};