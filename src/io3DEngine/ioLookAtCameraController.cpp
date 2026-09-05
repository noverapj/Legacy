

#include "stdafx.h"

#include "HelpFunc.h"
#include "ioMath.h"

#include "ioCameraController.h"
#include "ioLookAtCameraController.h"

#define PITCH_LIMIT	89.5f

ioLookAtCameraInput::ioLookAtCameraInput()
{
	m_bForward = m_bBack = m_bLeft = m_bRight = false;
	m_bZoom = m_bMiddle = false;
}

ioLookAtCameraInput::~ioLookAtCameraInput()
{
}

ioLookAtCameraController::ioLookAtCameraController()
{
	m_fForwardSpeed = 50.0f;
	m_fStrafeSpeed  = 50.0f;
	m_fRotSpeed = 4.0f;

	m_fMinDistance = 100.0f;
	m_vLookAt.x = m_vLookAt.y = m_vLookAt.z = m_fMinDistance;
}

ioLookAtCameraController::~ioLookAtCameraController()
{
}

void ioLookAtCameraController::Update( ioCameraInput *pInput )
{
	ioLookAtCameraInput *pLookAt = ToLookAtInput( pInput );
	if( !pLookAt )	return;

	float dt = 0.1f;

	float xMoveAmt, yMoveAmt;
	xMoveAmt = pLookAt->m_fXMoveAmt;
	yMoveAmt = pLookAt->m_fYMoveAmt;

	if( !pLookAt->m_bZoom && pLookAt->m_bMiddle )
	{
		if( xMoveAmt != 0.0f || yMoveAmt != 0.0f )
		{
			if( fabs( xMoveAmt ) < fabs( yMoveAmt ) )
				xMoveAmt = 0.0f;
			else
				yMoveAmt = 0.0f;

			Rotate( dt, xMoveAmt, yMoveAmt );
		}
	}

	if( pLookAt->m_bZoom )
	{
		ZoomToLookAt( dt, -yMoveAmt );
	}

	MoveInLocal( pLookAt, dt );
}

void ioLookAtCameraController::Rotate( float dt, float fXAmt, float fYAmt )
{
	m_fHead  += fXAmt * m_fRotSpeed * dt;
	m_fPitch += fYAmt * m_fRotSpeed * dt;

	m_fHead = ioMath::ArrangeHead( m_fHead );
	m_fPitch = MaxMinPitchLimit( m_fPitch );

	float fLength = D3DXVec3Length( &(m_vLookAt - m_vPos) );
	m_vPos = m_vLookAt - CalcDirectionVector( m_fHead, m_fPitch ) * fLength;
}

void ioLookAtCameraController::ZoomToLookAt( float dt, float fAmt )
{
	float fMove = dt * fAmt * m_fForwardSpeed;

	D3DXVECTOR3	vDir;
	vDir = m_vLookAt - m_vPos;
	float fDistance = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	if( fDistance - fMove < m_fMinDistance )
		fMove = fDistance - m_fMinDistance;

	m_vPos += vDir * fMove;
}

void ioLookAtCameraController::MoveInLocal( ioLookAtCameraInput *pInput, float dt )
{
	if( pInput->m_bForward )
	{
		m_vLookAt.y += m_fForwardSpeed * dt;
		m_vPos.y += m_fForwardSpeed * dt;
	}

	if( pInput->m_bBack )
	{
		m_vLookAt.y -= m_fForwardSpeed * dt;
		m_vPos.y -= m_fForwardSpeed * dt;
	}

	if( pInput->m_bRight || pInput->m_bLeft )
	{
		D3DXVECTOR3 vRight;
		D3DXVECTOR3 vDir = GetDirection();
		D3DXVec3Cross( &vRight, &ioMath::UNIT_Y, &vDir );

		vRight.y = 0.0f;
		D3DXVec3Normalize( &vRight, &vRight );

		if( pInput->m_bRight )
		{
			m_vLookAt += vRight * m_fStrafeSpeed * dt;
			m_vPos += vRight * m_fStrafeSpeed * dt;
		}

		if( pInput->m_bLeft )
		{
			m_vLookAt -= vRight * m_fStrafeSpeed * dt;
			m_vPos -= vRight * m_fStrafeSpeed * dt;
		}
	}
}

void ioLookAtCameraController::SetMinDistance( float fDistance )
{
	m_fMinDistance = fDistance;
	D3DXVECTOR3	vDir = m_vLookAt - m_vPos;

	float fOldDistance = D3DXVec3Length( &vDir );
	if( fOldDistance < m_fMinDistance )
	{
		D3DXVec3Normalize( &vDir, &vDir );
		m_vPos += vDir * ( m_fMinDistance - fOldDistance );
	}
}

void ioLookAtCameraController::SetPosition( const D3DXVECTOR3 &vPos )
{
	D3DXVECTOR3 vDir;
	D3DXVec3Normalize( &vDir, &(m_vLookAt - vPos) );

	D3DXQUATERNION qtDirection;
	ioMath::CalcDirectionQuaternion( qtDirection, vDir );

	float fHeadR, fPitchR, fRollR;
	ioMath::QuaterToEuler( qtDirection, fHeadR, fPitchR, fRollR );

	m_fHead = RADtoDEG( fHeadR );
	m_fHead = ioMath::ArrangeHead( m_fHead );

	m_fPitch = RADtoDEG( fPitchR );
	m_fPitch = MaxMinPitchLimit( m_fPitch );

	m_vPos = vPos;
}

void ioLookAtCameraController::SetLookAt( const D3DXVECTOR3 &vLookAt )
{
	D3DXVECTOR3 vDir;
	D3DXVec3Normalize( &vDir, &(vLookAt - m_vPos) );

	D3DXQUATERNION qtDirection;
	ioMath::CalcDirectionQuaternion( qtDirection, vDir );

	float fHeadR, fPitchR, fRollR;
	ioMath::QuaterToEuler( qtDirection, fHeadR, fPitchR, fRollR );

	m_fHead = RADtoDEG( fHeadR );
	m_fHead = ioMath::ArrangeHead( m_fHead );

	m_fPitch = RADtoDEG( fPitchR );
	m_fPitch = MaxMinPitchLimit( m_fPitch );

	m_vLookAt = vLookAt;
}

void ioLookAtCameraController::CreateViewMatrix( D3DXMATRIX *pViewMat ) const
{
	D3DXQUATERNION qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, 0.0f, DEGtoRAD(m_fRoll) );

	D3DXVECTOR3 vUp = qtExtraRot * ioMath::UNIT_Y;
	D3DXVec3Normalize( &vUp, &vUp );

	D3DXMatrixLookAtLH( pViewMat,
						&m_vPos,
						&m_vLookAt,
						&vUp );
}

const char* ioLookAtCameraController::GetName()
{
	return "LookAt Camera";
}

void ioLookAtCameraController::GetExtraInfoText( char *szBuf, int iLen ) const
{
	char szTemp[MAX_PATH];

	memset( szBuf, 0, iLen );
	sprintf( szTemp, "Look At: %.2f, %.2f, %.2f", m_vLookAt.x, m_vLookAt.y, m_vLookAt.z );
	strncpy( szBuf, szTemp, iLen-1 );
}

void ioLookAtCameraController::SetSpeed( float fForward, float fStrafe, float fRot )
{
	m_fForwardSpeed = fForward;
	m_fStrafeSpeed  = fStrafe;
	m_fRotSpeed = fRot;
}

void ioLookAtCameraController::GetSpeed( float& fForward, float& fStrafe, float& fRot )
{
	fForward = m_fForwardSpeed;
	fStrafe = m_fStrafeSpeed;
	fRot = m_fRotSpeed;
}

D3DXVECTOR3 ioLookAtCameraController::CalcDirectionVector( float fHeadD, float fPitchD )
{
	D3DXQUATERNION qtRot = ioMath::MakeQuaternionByDegree( fHeadD, fPitchD, 0.0f );

	return qtRot * ioMath::UNIT_Z;
}

float ioLookAtCameraController::MaxMinPitchLimit( float fPitchD )
{
	if( fPitchD > PITCH_LIMIT )
		fPitchD = PITCH_LIMIT;

	if( fPitchD < -PITCH_LIMIT )
		fPitchD = -PITCH_LIMIT;

	return fPitchD;
}

D3DXVECTOR3 ioLookAtCameraController::GetDirection() const
{
	D3DXVECTOR3 vDir = m_vLookAt - m_vPos;
	D3DXVec3Normalize( &vDir, &vDir	);

	return vDir;
}