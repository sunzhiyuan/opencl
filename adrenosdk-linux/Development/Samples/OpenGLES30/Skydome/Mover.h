//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//Simple camera mover class
#include "FrmMath.h"
#include "FrmInput.h"

#ifndef _MOVER_H_
#define _MOVER_H_

class Mover
{
private:
	void DampenVel()
	{
		if((CurVel.x*CurVel.x)+(CurVel.y*CurVel.y)+(CurVel.z*CurVel.z) > 0.0001)//mag2 > 0.0001
		{
			CurVel *= 0.8f;
		}else
		{
			CurVel = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
		}
	}
public:
	Mover() : SpeedInc(0.0999f)
	{
		CurVel = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
		WorldPose = FRMVECTOR3( 0.0f, 0.0f, 0.0f );

		Spin = FRMVECTOR2(0.f,0.f);

		FirstMouseUpdate = true;

		InverseWorldMatrix = FrmMatrixIdentity();
	}
	void SetPosition(const FRMVECTOR3& in)
	{
		WorldPose = -in;
	}
	void Update( FRM_INPUT& input, FLOAT32 dt)
	{
		//damp vel 
		DampenVel();

		// Process input
		UINT32 nButtons;
		UINT32 nPressedButtons;
		FrmGetInput( &input, &nButtons, &nPressedButtons );

		FRMVECTOR2 delta = FRMVECTOR2(0.f, 0.f);
		UINT32 pntState;
		FRMVECTOR2 newMousePose;
		input.GetPointerState( &pntState, &newMousePose );
		if( pntState & FRM_INPUT::POINTER_DOWN )
		{
			delta = newMousePose - MousePose;//compute mouse movement
			MousePose = newMousePose;

			if(FirstMouseUpdate)//grab mouse pose first hit
			{
				FirstMouseUpdate = false;
				return;
			}

		}else
		{
			FirstMouseUpdate = true;
		}

		delta *= 0.6f;//damp spin rate

		Spin += delta;

		const float piOn2 = 1.57079633f;
		if(Spin.y > piOn2)
			Spin.y = piOn2;
		if(Spin.y < -piOn2)
			Spin.y = -piOn2;


		//seems silly to do full rotation axis (simple axis alligned Eular rotations would do fine)
		FRMVECTOR3 yAxis = FRMVECTOR3(0.0f,1.0f,0.0f);
		FRMMATRIX4X4 roty = FrmMatrixRotate( Spin.x, yAxis);
		FRMVECTOR3 xAxis = FRMVECTOR3( roty.M(0,0), roty.M(1,0), roty.M(2,0));
		FRMMATRIX4X4 rotx = FrmMatrixRotate( -Spin.y , xAxis);//FRMVECTOR3(1.0f,0.0f,0.0f));

		FRMMATRIX4X4 rot = FrmMatrixMultiply( rotx , roty );

		if( nButtons & FRM_INPUT::DPAD_UP )
		{
			CurVel -= FRMVECTOR3( rot.M(0,2), rot.M(1,2), rot.M(2,2)) * SpeedInc;
		}
		if( nButtons & FRM_INPUT::DPAD_DOWN )
		{
			CurVel += FRMVECTOR3( rot.M(0,2), rot.M(1,2), rot.M(2,2)) * SpeedInc;
		}
		if( nButtons & FRM_INPUT::DPAD_RIGHT )
		{
			CurVel += FRMVECTOR3( rot.M(0,0), rot.M(1,0), rot.M(2,0)) * SpeedInc;
		}
		if( nButtons & FRM_INPUT::DPAD_LEFT )
		{
			CurVel -= FRMVECTOR3( rot.M(0,0), rot.M(1,0), rot.M(2,0)) * SpeedInc;
		}

		WorldPose += (CurVel * dt);
		FRMVECTOR3 negWorldPose = -WorldPose;
		InverseWorldMatrix = FrmMatrixTranslate( negWorldPose );
	
		InverseWorldMatrix = FrmMatrixMultiply( InverseWorldMatrix, rot );

	}
	const FRMMATRIX4X4& GetInverseWorldMatrix() { return InverseWorldMatrix; }

private:

	FRMMATRIX4X4 InverseWorldMatrix;
	FRMVECTOR2 MousePose; 
	FRMVECTOR2 Spin; 
	FRMVECTOR3 WorldPose;
	FRMVECTOR3 CurVel;
	bool FirstMouseUpdate;
	const float SpeedInc;

};
#endif
