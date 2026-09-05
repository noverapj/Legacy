

#include "stdafx.h"

#include "ioNode.h"
#include "ioMovableObject.h"
#include "ioCameraController.h"
#include "ioTargetLookAtCameraController.h"

#include "HelpFunc.h"
#include "ioMath.h"
#include "ioRay.h"

#include "ioFrameTimer.h"

ioTargetLookAtCameraController::ioTargetLookAtCameraController()
{
	m_pTargetObj = NULL;

	m_vLookAt = ioMath::VEC3_ZERO;
	m_vPreTargetPos = D3DXVECTOR3( 0.0f, 0.0f, 400.0f );

	m_fMinDist	= 100.0f;
	m_fMaxDist	=FLOAT1000;
	m_fCurDist	= 400.0f;
	m_fPreDist  = 400.0f;

	m_fSpringConst = 2.5f;
	m_fDampConst = 0.25f;

	m_fRotSpringConst = 5.0f;
	m_fRotDampConst = 0.25f;

	m_fFwdSpeed = 50.0f;
	m_fRotSpeed	= 4.0f;

	m_fTargetHead  = 0.0f;
	m_fTargetPitch = 0.0f;

	m_fCurBottomHeight = 0.0f;
	m_fDefaultHeight  =FLOAT1000;

	m_bRepairDefault = false;
}

ioTargetLookAtCameraController::~ioTargetLookAtCameraController()
{
}

void ioTargetLookAtCameraController::SetMinMaxDistance( float fMinDist, float fMaxDist )
{
	m_fMinDist = fMinDist;
	m_fMaxDist = fMaxDist;
	
	m_fCurDist = max( m_fMinDist, min( m_fCurDist, m_fMaxDist ) );
	m_fPreDist = m_fCurDist;
}

void ioTargetLookAtCameraController::SetSpeed( float fForward, float fRot )
{
	m_fFwdSpeed = fForward;
	m_fRotSpeed = fRot;
}

void ioTargetLookAtCameraController::SetSpringConstants( float fSpringConst, float fDampConst )
{
	m_fSpringConst = fSpringConst;
	m_fDampConst   = fDampConst;
}

void ioTargetLookAtCameraController::SetRotSpringConstants( float fSpringConst, float fDampConst )
{
	m_fRotSpringConst = fSpringConst;
	m_fRotDampConst   = fDampConst;
}

void ioTargetLookAtCameraController::SetLookAtTarget( ioMovableObject *pObj,
													  float fDefalutHeight,
													  float fCurBottomHeight,
													  float fCurDist,
													  const D3DXVECTOR3 &vOffset )
{
	m_pTargetObj = pObj;

	m_fCurDist = max( m_fMinDist, min( fCurDist, m_fMaxDist ) );
	m_fPreDist = m_fCurDist;

	m_vLookAtOffset = vOffset;
	m_fDefaultHeight   = fDefalutHeight;
	m_fCurBottomHeight = fCurBottomHeight;

	m_vLookAt = GetLookAtByObjPos();
	m_vPreTargetPos = m_vPos = GetDefaultBackPos();

	UpdateAngleByDir();

	m_fTargetHead  = m_fHead;
	m_fTargetPitch = m_fPitch;
}

D3DXVECTOR3 ioTargetLookAtCameraController::GetLookAtByObjPos() const
{
	D3DXVECTOR3 vLookAt;
	vLookAt = m_pTargetObj->GetParentNode()->GetDerivedOrientation() * m_vLookAtOffset;
	vLookAt += m_pTargetObj->GetParentNode()->GetDerivedPosition();

	return vLookAt;
}

float ioTargetLookAtCameraController::LimitPitch( float fPitch )
{
	if( fPitch >= 70.0f )  fPitch = 70.0f;
	if( fPitch <= -70.0f ) fPitch = -70.0f;

	return fPitch;
}

void ioTargetLookAtCameraController::Update( ioCameraInput *pInput )
{
	UpdatePos( pInput );
	UpdateAngleByDir();
}

void ioTargetLookAtCameraController::SetObjMoveDir( const D3DXVECTOR3 &vMoveDir )
{
	D3DXMATRIX matNextDir;
	ioMath::CalcDirectionMatrix( matNextDir, -vMoveDir );

	D3DXQUATERNION qtDir;
	D3DXQuaternionRotationMatrix( &qtDir, &matNextDir );

	m_vRepairDir = qtDir * ioMath::UNIT_Z;
	m_bRepairDefault = true;
}

void ioTargetLookAtCameraController::SetCurDist( float fDist )
{
	m_fCurDist = max( min( fDist, m_fMaxDist ), m_fMinDist );
	m_fPreDist = m_fCurDist;
}

void ioTargetLookAtCameraController::SetCurBottomHeight( float fHeight )
{
	m_fCurBottomHeight = fHeight;
}

void ioTargetLookAtCameraController::SetDefaultHeight( float fHeight )
{
	m_fDefaultHeight = fHeight;
}

void ioTargetLookAtCameraController::UpdatePos( ioCameraInput *pInput )
{
	m_vLookAt = GetLookAtByObjPos();

	if( pInput->m_fWheelAmt != 0.0f )
	{
		ZoomToLookAt( pInput->m_fWheelAmt );
		m_bRepairDefault = false;
	}

	float fRealDist = D3DXVec3Length( &(m_vLookAt - m_vPos) );
	float fSpringDist = SpringDistDamp( m_fSpringConst * 2.0f,
										m_fDampConst * 2.0f,
										fRealDist,
										m_fCurDist,
										m_fPreDist );

	fRealDist -= fSpringDist;
	fRealDist = max( m_fMinDist, min( fRealDist, m_fMaxDist ) );
	if( fabs( fRealDist - m_fCurDist ) < 1.0f )
		fRealDist = m_fCurDist;

	if( pInput->m_fXMoveAmt != 0.0f || pInput->m_fYMoveAmt != 0.0f )
	{
		float fRotHead, fRotPitch;
		fRotHead  = pInput->m_fXMoveAmt * m_fRotSpeed * 0.1f;
		fRotPitch = pInput->m_fYMoveAmt * m_fRotSpeed * 0.07f;

		m_fTargetHead  = ioMath::ArrangeHead( m_fTargetHead - fRotHead );
		m_fTargetPitch = LimitPitch( m_fTargetPitch - fRotPitch );

		m_bRepairDefault = false;
	}

	D3DXVECTOR3 vNewPos;
	D3DXVECTOR3 vTargetPos;
	if( m_bRepairDefault )
	{
		vTargetPos = m_vLookAt + m_vRepairDir * fRealDist;
		vTargetPos.y = m_fCurBottomHeight + m_fDefaultHeight;

		vNewPos = SpringMoveDamp( m_fSpringConst,
								  m_fDampConst,
								  m_vPos,
								  vTargetPos,
								  m_vPreTargetPos );

		D3DXVECTOR3 vCamDir = m_vLookAt - vNewPos;
		D3DXVec3Normalize( &vCamDir, &vCamDir );
	
		D3DXVECTOR3 vSide, vUp( 0.0f, 1.0f, 0.0f );
		D3DXVec3Cross( &vSide, &vUp, &vCamDir );

		D3DXVECTOR3 vRayDir;
		D3DXVec3Cross( &vRayDir, &vSide, &vUp );

		ioRay kSightRay( vNewPos, vRayDir );
		ioSphere kSphere( m_vLookAt, fRealDist );

		D3DXVECTOR3 vPoint[2];
		if( ioMath::FindIntersection( kSightRay, kSphere, vPoint, true ) )
		{
			m_vPos = vPoint[0];
		}
		else
		{
			vNewPos -= m_vLookAt;
			D3DXVec3Normalize( &vNewPos, &vNewPos );
			m_vPos = m_vLookAt + vNewPos * fRealDist;
		}
	}
	else
	{
		D3DXQUATERNION qtRot;
		qtRot = ioMath::MakeQuaternionByDegree( m_fTargetHead,
												m_fTargetPitch,
												0.0f );

		vTargetPos = m_vLookAt + qtRot * D3DXVECTOR3(0.0f, 0.0f, fRealDist);

		vNewPos = SpringMoveDamp( m_fRotSpringConst,
								  m_fRotDampConst,
								  m_vPos,
								  vTargetPos,
								  m_vPreTargetPos );

		D3DXVECTOR3 vNewDir = vNewPos - m_vLookAt;
		D3DXVec3Normalize( &vNewDir, &vNewDir );
		m_vPos = m_vLookAt + vNewDir * fRealDist;
	}

	m_fPreDist = fRealDist;
	m_vPreTargetPos = vTargetPos;
}

void ioTargetLookAtCameraController::UpdateAngleByDir()
{
	D3DXVECTOR3 vDir = m_vPos - m_vLookAt;
	D3DXVec3Normalize( &vDir, &vDir );

	float fHeadR = atan2f( vDir.x, vDir.z );
	m_fHead = ioMath::ArrangeHead( RADtoDEG( fHeadR ) );
	
	D3DXVECTOR3 vXZProj;
	vXZProj.x = vDir.x;
	vXZProj.y = 0.0f;
	vXZProj.z = vDir.z;
	D3DXVec3Normalize( &vXZProj, &vXZProj );

	float fPitchR = acos( D3DXVec3Dot( &vDir, &vXZProj ) );
	m_fPitch = LimitPitch( RADtoDEG( fPitchR ) );

	if( vDir.y >= 0.0f )
	{
		m_fPitch = -m_fPitch;
	}

	if( m_bRepairDefault )
	{
		m_fTargetHead  = m_fHead;
		m_fTargetPitch = m_fPitch;
	}
}

float ioTargetLookAtCameraController::SpringDistDamp( float fSpringConst,
													  float fDampConst,
													  float fCurDist,
													  float fTargetDist,
													  float fPreDist )
{
	float fDisp = fCurDist - fTargetDist;
	if( fabsf( fDisp ) <= 1.0f )	return -fDisp;

	float fForceMag = fSpringConst * fDisp +
					  fDampConst * ( fPreDist - fCurDist );

	return fForceMag * g_FrameTimer.GetSecPerFrame();
}

D3DXVECTOR3 ioTargetLookAtCameraController::GetDefaultBackPos() const
{
	D3DXQUATERNION	qtRot;
	qtRot = m_pTargetObj->GetParentNode()->GetDerivedOrientation();

	D3DXVECTOR3 vDefaultPos;
	vDefaultPos = m_vLookAt + qtRot * D3DXVECTOR3( 0.0f, 0.0f, m_fCurDist );
	vDefaultPos.y = m_fCurBottomHeight + m_fDefaultHeight;

	return vDefaultPos;	
}

D3DXVECTOR3 ioTargetLookAtCameraController::SpringMoveDamp( float fSpringConst,
															float fDampConst, 
														    const D3DXVECTOR3 &vCurPos,
															const D3DXVECTOR3 &vTargetPos,
															const D3DXVECTOR3 &vPreTargetPos )
{
	float fDeltaTime = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vDisp, vVelocity;

	vDisp = vCurPos - vTargetPos;
	vVelocity = ( vPreTargetPos - vTargetPos ) * fDeltaTime;

	float	fDispLength;

	fDispLength = D3DXVec3Length( &vDisp );
	if( fDispLength < 1.0f )
	{
		return vTargetPos;
	}

	float fForceMag;
	fForceMag = fSpringConst * ( 1.0f - fDispLength ) +
				fDampConst * ( D3DXVec3Dot( &vDisp, &vVelocity ) / fDispLength );

	D3DXVECTOR3 vLookToCur, vLookToNext;
	vLookToCur  = vCurPos - m_vLookAt;
	vLookToNext = vTargetPos - m_vLookAt;

	D3DXVECTOR3 vCurNextCenter;
	D3DXVec3Normalize( &vCurNextCenter, &(vLookToCur + vLookToNext ) );
	vCurNextCenter *= D3DXVec3Length( &vLookToNext );

	D3DXVECTOR3 vToCurNrm, vToNextNrm;

	D3DXVec3Normalize( &vToCurNrm, &vLookToCur );
	D3DXVec3Normalize( &vToNextNrm, &vLookToNext );

	if( D3DXVec3Dot( &vToCurNrm, &vToNextNrm ) < 0.99f )
	{
		vDisp = vCurNextCenter - vLookToCur;

		D3DXVec3Normalize( &vDisp, &vDisp );
		vDisp *= fabs( fForceMag * fDeltaTime );
	}
	else
	{
		D3DXVec3Normalize( &vDisp, &vDisp );
		vDisp *= fForceMag * fDeltaTime;
	}

	return vCurPos + vDisp;
}

void ioTargetLookAtCameraController::ZoomToLookAt( float fAmt )
{
	float fMove = fAmt * m_fFwdSpeed;

	if( m_fCurDist + fMove < m_fMinDist )
		fMove = m_fMinDist - m_fCurDist;
	else if( m_fCurDist + fMove > m_fMaxDist )
		fMove = m_fMaxDist - m_fCurDist;

	m_fCurDist += fMove;
}

void ioTargetLookAtCameraController::CreateViewMatrix( D3DXMATRIX *pViewMat ) const
{
	D3DXMatrixLookAtLH( pViewMat,
						&m_vPos,
						&m_vLookAt,
						&ioMath::UNIT_Y );
}

void ioTargetLookAtCameraController::GetExtraInfoText( char *szBuf, int iLen ) const
{
	D3DXQUATERNION qt = m_pTargetObj->GetParentNode()->GetDerivedOrientation();

	float fHead = ioMath::QuaterToYaw( qt );
	fHead = ioMath::ArrangeHead( RADtoDEG( fHead ) );

	char szTemp[MAX_PATH];
	memset( szBuf, 0, iLen );
	sprintf( szTemp, "Head : %.2f Pitch : %.2f, ObjHead:%.2f", m_fHead, m_fPitch, fHead );
	strncpy( szBuf, szTemp, iLen-1 );
}

void ioTargetLookAtCameraController::SetPosition( const D3DXVECTOR3 &vPos )
{
	m_vPos = m_vPreTargetPos = vPos;
}

D3DXVECTOR3 ioTargetLookAtCameraController::GetDirection() const
{
	D3DXVECTOR3 vDir = m_vLookAt - m_vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	return vDir;
}

const char* ioTargetLookAtCameraController::GetName()
{
	return "Target LookAt Camera";
}
