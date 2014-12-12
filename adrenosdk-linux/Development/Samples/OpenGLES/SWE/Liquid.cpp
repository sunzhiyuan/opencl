//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>

#include "Crossplatform.h"
#include "Liquid.h"


#include <math.h>
#include <stdio.h>
//#include <string.h>
#if OSX

#include <stdlib.h>

#else

#include <malloc.h>

#endif


#define height(x,y,i) m_height[x + (y)*m_xres + (i)*m_xres*m_yres]
#define norm(x,y,i) m_norm[i + (x)*4 + (y)*4*m_xres]
#define vx(x,y,i) m_vx[x + (y)*m_xres + (i)*m_xres*m_yres]
#define vy(x,y,i) m_vy[x + (y)*m_xres + (i)*m_xres*m_yres]
#define f(g,x,y,i) g[x + (y)*m_xres + (i)*m_xres*m_yres]



//  value location reference   
//         _______ 
//        |       |
//        |       |
//    vx ->   h   |
//        |       |
//        |___^___|
//           vy
//           



Liquid::Liquid()
{
	m_height = m_vx = m_vy = 0;
	m_cur = m_xres = m_yres = 0;
	m_norm = 0;
	m_nex = 1;
	m_grav = 1.0f;
	m_recipxdelt = 1.0f;
	m_recipydelt = 1.0f;
	m_iframe = 0;
}

Liquid::~Liquid()
{
}

void Liquid::Destroy()
{
	free(m_height);
	free(m_vx);
	free(m_vy);
}

void Liquid::Init (int xres, int yres)
{
	m_xres = xres;
	m_yres = yres;

	m_height = (float*) malloc(2*xres*yres*sizeof(float));
	m_vx = (float*) malloc(2*xres*yres*sizeof(float));
	m_vy = (float*) malloc(2*xres*yres*sizeof(float));
	m_norm = (char*) malloc(4*xres*yres);

	m_xoffset = 0.15f;
	m_xscale = 0.7f;
	m_yoffset = 0.15f;
	m_yscale = 0.7f;

}

void Liquid::Init (int xres, int yres, float * pHeight, float * pVx, float * pVy, char* norm)
{
	// todo: add some error checking
	m_xres = xres;
	m_yres = yres;
	m_norm = norm;

	m_height = pHeight;
	m_vx = pVx;
	m_vy = pVy;

}


void Liquid::Init (int xres, int yres, char* norm)
{	// todo: add some error checking
	m_xres = xres;
	m_yres = yres;

	m_norm = norm;

	m_height = (float*) malloc(2*xres*yres*sizeof(float));
	m_vx = (float*) malloc(2*xres*yres*sizeof(float));
	m_vy = (float*) malloc(2*xres*yres*sizeof(float));

}


float Liquid::blerp ( float * pField, float x, float y)
{
	int i = (int) x;
	int j = (int) y;
	float xint = x - (float) i;
	float yint = y - (float) j;

	return yint * (xint * f(pField, i+1, j+1, m_cur) + (1.f - xint) * f(pField, i, j+1, m_cur))
		+ (1.f - yint) * (xint * f(pField, i+1, j, m_cur) + (1.f - xint) * f(pField, i, j, m_cur));
}


void Liquid::Advect(float tdelt, float* pField, float dx, float dy)
{
	float xv,yv, p, q;

	for (int i = 1; i < m_xres-1; i++)
	{
		for (int j = 1; j < m_yres-1; j++)
		{
			p = (float) i;
			q = (float) j;
			xv = p - tdelt * blerp (m_vx, p + dx, q);  // todo: make 2 element lerp
			yv = q - tdelt * blerp (m_vy, p, q + dy);
			f(pField, i, j, m_nex) = blerp (pField, xv , yv );
		}
	}

}

void Liquid::UpdateHeight(float tdelt)
{

	for (int i = 1; i < m_xres-1; i++)
	{
		for (int j = 1; j < m_yres-1; j++)
		{
			height(i,j,m_nex) -= height(i,j,m_nex) * tdelt *
				((vx(i+1,j,m_nex) - vx(i,j,m_nex)) * m_recipxdelt + (vy(i,j+1,m_nex) - vy(i,j,m_nex)) * m_recipydelt);
		}
	}

}

void Liquid::UpdateBoundaries(float tdelt)
{

	for (int i = 0; i < m_xres; i++)
	{
		height(i,0,m_nex) = height (i,1,m_cur);
		height(i,m_yres-1,m_nex) = height (i,m_yres-2,m_cur);
		vx(i,0,m_nex) = 0.0f;
		vx(i,m_yres-1,m_nex) = 0.0f;
		vy(i,0,m_nex) = 0.0f;
		vy(i,m_yres-1,m_nex) = 0.0f;
	}

	for (int j = 0; j < m_yres; j++)
	{
		height(0,j,m_nex) = height (1,j,m_cur);
		height(m_xres-1,j,m_nex) = height (m_xres-2,j,m_cur);
		vx(1,j,m_nex) = 0.0f;
		vx(m_xres-1,j,m_nex) = 0.0f;
		vy(0,j,m_nex) = 0.0f;
		vy(m_xres-1,j,m_nex) = 0.0f;
	}

}

void Liquid::UpdateVelocities(float tdelt)
{

	for (int i = 2; i < m_xres-1; i++)
	{
		for (int j = 1; j < m_yres-1; j++)
		{
			vx(i,j,m_nex) += m_grav * tdelt * (height(i-1,j,m_nex) - height (i,j,m_nex)) * m_recipxdelt;
		}
	}

	for (int i = 1; i < m_xres-1; i++)
	{
		for (int j = 2; j < m_yres-1; j++)
		{
			vy(i,j,m_nex) += m_grav * tdelt * (height(i,j-1,m_nex) - height (i,j,m_nex)) * m_recipydelt;
		}
	}

}

void Liquid::CalcNormals()
{
	float nm[3];
	m_volume = 0.f;

	
	for (int i = 1; i < m_xres-1; i++)
	{
		for (int j = 1; j < m_yres-1; j++)
		{
			nm[0] = m_recipxdelt * (height(i+1,j,m_cur) - height(i-1,j,m_cur));
			nm[1] = m_recipydelt * (height(i,j+1,m_cur) - height(i,j-1,m_cur));
			nm[2] = sqrt (1.0f - nm[0] * nm[0] - nm[1] * nm[1]);

			for (int c=0; c<3; c++) norm(i,j,c) = (char) (256. * (nm[c] + 0.5f));

			m_volume += height(i,j,m_cur);

			// refraction
			float rind = 0.5; // 0.5;  // for now
			float dist = 0.5;  // for now
			float a = rind *rind;

			float b = sqrt (1.f - a + a * nm[2] *nm[2] );
			float refract[3];
			for (int c=0; c<3; c++) refract[c] = nm[2] * nm[c];
			refract[2] -= 1.0;
			for (int c=0; c<3; c++) refract[c] = -b * nm[c] + rind * refract[c];

			float refx = (float) i / (float) m_xres + dist * refract[0];
			float refy = (float) j / (float) m_yres + dist * refract[1];

			// reflection
			// eye position is at 0.5, 0.5, 1.0, for now
			float eye[4];
			eye[0] = (float) i / (float) m_xres - 0.5f;
			eye[1] = (float) j / (float) m_yres - 0.5f;
			eye[2] = -0.125f;
			eye[3] = 0.f;

			float eyen = sqrt (eye[0]*eye[0] + eye[1]*eye[1] + eye[2]*eye[2]);
			for (int c=0; c<3; c++) eye[c] /= eyen;

			eye[0] = 0.f;
			eye[1] = 0.f;
			eye[2] = -1.f;
			eye[3] = 0.f;

			// calc reflection vector
			float ndotl = eye[0]*nm[0] + eye[1]*nm[1] + eye[2]*nm[2];    // dot (eye, normal);
			float reflect[4];
			for (int c=0; c<3; c++) reflect[c] = 2.f * ndotl * nm[c] - eye[c];


			// spherical coordinates
			float m = 2.f * sqrt (reflect[0] * reflect[0] + reflect[1] * reflect[1] + (1.0f - reflect[2])*(1.0f - reflect[2]));
			m = 1.f/ m;
			float sphx = reflect[0] * m + 0.5f;
			float sphy = reflect[1] * m + 0.5f;

			norm(i,j,0) = (char) (256. *sphx);
			norm(i,j,1) = (char) (256. *sphy);
			norm(i,j,2) = (char) (256. *refx);
			norm(i,j,3) = (char) (256. *refy);
		}
	}

	
	// readjust volume
	float maxLeak = 0.1f;
	if (ABS(m_initVolume - m_volume) > maxLeak)
	{
		float pc = 1.f / ((float) (m_xres -1) * (float) (m_yres -1));
		pc = (m_initVolume - m_volume) * pc;
			
		for (int i = 0; i < m_xres-1; i++)
		{
			for (int j = 0; j < m_yres-1; j++)
			{
				height(i+1,j,m_cur) += pc;
			}
		}
	}
	
}

void Liquid::Step(int iter)
{
	float delta_time = 0.1f;  //

	for (int i=0; i<iter; i++)
	{
		Advect (delta_time, m_height, 0.5f, 0.5f);
		Advect (delta_time, m_vx, 0.f, 0.5f);
		Advect (delta_time, m_vy, 0.5f, 0.f);
		UpdateBoundaries (delta_time);
		UpdateHeight (delta_time);
		UpdateVelocities (delta_time);

		int tmp = m_cur;  m_cur = m_nex;  m_nex = tmp; // swap current fields
	}

	CalcNormals();
	m_iframe++;
}


void Liquid::ResetValues ()
{
	m_cur = 0;
	m_nex = 1;
	m_initVolume = 0.f;

	for (int i = 0; i < m_xres; i++)
	{
		for (int j = 0; j < m_yres; j++)
		{
			height(i,j, 0) = .2f;
			height(i,j, 1) = .2f;
			if ((i <= (m_xres / 2)) && (i >= (2*m_xres / 7)) && (j <= (m_yres / 2)) && (j >= (2*m_yres / 7)))
			{
				height(i,j, 0) = .2f; // 1.0f;
			}
			vx(i,j, 0) = 0.0;
			vy(i,j, 0) = 0.0;
			vx(i,j, 1) = 0.0;
			vy(i,j, 1) = 0.0;

			if ((i>0) && (i < m_xres - 1) && (j>0) && (j < m_yres - 1)) m_initVolume += height(i,j, 0);
		}
	}

	float nm[3];

	for (int i = 1; i < m_xres-2; i++)
	{
		for (int j = 1; j < m_yres-2; j++)
		{
			nm[0] = (float) i / (float)m_xres;
			nm[1] = (float) j / (float)m_yres;
			nm[2] = sqrt (1.0f - nm[0] * nm[0] + nm[1] * nm[1]);
			
			for (int c=0; c<3; c++) norm(i,j,c) = (char) (256. * nm[c]);
		}
	}
}


void Liquid::Finger (float x, float y)
{
	int i = (int) (x * (float) m_xres);
	int j = (int) (y * (float) m_yres);

	if ((i < (m_xres - 1)) && (i > 0) && (j < (m_yres - 1)) && (j > 1))
	{
		float disp = 0.5f;  // for now
		if (height(i,j, m_cur) < disp) disp = height(i,j, m_cur);

		 // todo: boundary?
		height(i,j, m_cur) -= disp;

		vx(i,j, m_cur) -= disp * 0.25f;
		vy(i,j, m_cur) -= disp * 0.25f;
		vx(i+1,j, m_cur) += disp * 0.25f;
		vy(i,j+1, m_cur) += disp * 0.25f;
		
		height(i+1, j, m_cur) += disp * 0.25f;
		height(i, j+1, m_cur) += disp * 0.25f;
		height(i-1, j, m_cur) += disp * 0.25f;
		height(i, j-1, m_cur) += disp * 0.25f;
	}

}

const void* Liquid::CurrentHeight()
{
	void* pHeight;
	pHeight = (void*) &m_height[m_cur * m_xres * m_yres];
	return (const_cast <const void *> (pHeight));
}

const void* Liquid::CurrentNorm()
{
	void* pNorm;
	pNorm = (void*) m_norm;
	return (const_cast <const void *> (pNorm));
}

float Liquid::Volume()
{
	float volume = 0.f;

	
	for (int i = 1; i < m_xres-1; i++)
	{
		for (int j = 1; j < m_yres-1; j++)
		{
			volume += height(i,j,m_cur);
		}
	}
	return volume;
}