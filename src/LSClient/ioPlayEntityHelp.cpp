#include "stdafx.h"
#include "ioPlayEntityHelp.h"

namespace ioPlayEntityHelp
{
	void CheckOrientation( IN const D3DXVECTOR3 &vDir, OUT D3DXQUATERNION& qtRot )
	{
		D3DXVECTOR3 vUp = ioMath::UNIT_Y;
		D3DXVECTOR3 vSide, vFront;

		D3DXVec3Cross( &vSide, &vUp, &vDir );
		D3DXVec3Cross( &vFront, &vSide, &vUp );
		D3DXVec3Normalize( &vSide, &vSide );
		D3DXVec3Normalize( &vFront, &vFront );

		float fYaw = D3DXVec3Dot( &vFront, &-ioMath::UNIT_Z );
		float fPitch = D3DXVec3Dot( &vFront, &vDir );
		float fCheckYaw = D3DXVec3Dot( &vDir, &ioMath::UNIT_X );
		float fCheckPitch = D3DXVec3Dot( &vDir, &ioMath::UNIT_Y );

		if( fPitch >= FLOAT1 )
		{
			ioMath::CalcDirectionQuaternion( qtRot, -vDir );
		}
		else if( fPitch <= -FLOAT1 )
		{
			float fYawAngle = acos(fYaw);
			float fPitchAngle = acos(fPitch);
			D3DXQuaternionRotationYawPitchRoll( &qtRot, fYawAngle, fPitchAngle, 0.0f );
		}
		else
		{
			float fYawAngle;
			if( fYaw >= FLOAT1 )
			{
				fYawAngle = 0.0f;
			}
			else
			{
				fYawAngle = acos(fYaw);
				if( fCheckYaw > 0.0f )
					fYawAngle *= -1;
			}

			float fPitchAngle = acos(fPitch);
			if( fPitch >= FLOAT1 )
			{
				fPitchAngle = 0.0f;
			}
			else
			{
				fPitchAngle = acos(fPitch);
				if( fCheckPitch < 0.0f )
					fPitchAngle *= -1;
			}

			D3DXQuaternionRotationYawPitchRoll( &qtRot, fYawAngle, fPitchAngle, 0.0f );
		}
	}

	void RotateMoveDir( IN float fAngleH, IN float fAngleV, OUT D3DXVECTOR3& vDir, OUT D3DXQUATERNION& qtRot )
	{
		fAngleV = ioMath::ArrangeHead( fAngleV );
		fAngleH = ioMath::ArrangeHead( fAngleH );		

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		//Vertical
		if( fAngleV != 0.0f )
		{
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
			vDir = qtRot * vDir;			
		}

		//Horizon
		if( fAngleH != 0.0f )
		{
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
			vDir = qtRot * vDir;
		}
		ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	}

	void MoveOffset( IN const D3DXVECTOR3& vOffset, IN const D3DXQUATERNION& qtTargetDir, OUT D3DXVECTOR3& vOutOffset )
	{
		D3DXVECTOR3 vDir = qtTargetDir * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vSide;
		D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );
		vOutOffset		+= vDir * vOffset.z;
		vOutOffset.y	+= vOffset.y;
		vOutOffset		+= vSide * vOffset.x;
	}
}