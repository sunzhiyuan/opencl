//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#ifndef __BOUNDS_H__
#define __BOUNDS_H__

#include "FrmMath.h"

/*
 */
class BoundSphere;
class BoundBox;

/******************************************************************************\
*
* BoundSphere
*
\******************************************************************************/

/*
 */
class BoundSphere {
		
	public:
		
		BoundSphere();
		explicit BoundSphere(  BoundBox &bb);
		virtual ~BoundSphere();
		
		void init();

		void setTransform(FRMMATRIX4X4 &transform);
		
		void set(FRMVECTOR3 &cent, float rad);
		void set(  BoundBox &bb);
		// expand
		void expand(FRMVECTOR3 &point);
	
		void expand(  BoundBox &bb);
		
		// inside bounds
		int inside(  BoundBox &bb)  ;

		// parameters
		inline int isValid() { return valid; }
		inline FRMVECTOR3 &getCenter() { return center; }
		inline float getRadius() { return radius; }
		
	private:
		
		int valid;
		FRMVECTOR3 center;	// bound sphere center
		float radius;	// bound sphere radius
};

/******************************************************************************\
*
* BoundBox
*
\******************************************************************************/

/*
 */
class BoundBox {
		
	public:
		
		BoundBox();
		~BoundBox();
		// expand
		void expand(FRMVECTOR3 &point);

		// parameters
		inline int isValid(){ return valid; }
		FRMVECTOR3 getMin() { return min; }
		FRMVECTOR3 getMax() { return max; }
		void set(FRMVECTOR3 &min_, FRMVECTOR3 &max_) ;
		
	private:
		
		int valid;
		FRMVECTOR3 min;		// bound box minimum
		FRMVECTOR3 max;		// bound box maximum
};


#endif /* __BOUNDS_H__ */
