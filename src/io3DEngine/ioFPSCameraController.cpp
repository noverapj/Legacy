

#include "stdafx.h"

#include "ioMath.h"
#include "ioCameraController.h"
#include "ioFPSCameraController.h"
#include "HelpFunc.h"

ioFPSCameraInput::ioFPSCameraInput()
{
	m_bForward = m_bBack = m_bLeft = m_bRight = m_bMiddle = false;
	m_bLeftRot = m_bRightRot = m_bUp = m_bDown = m_bSpeedUp = false;
}

ioFPSCameraInput::~ioFPSCameraInput()
{
}

ioFPSCameraController::ioFPSCameraController()
{
	m_fForwardSpeed  = 100.0f;
	m_fStrafeSpeed   = 100.0f;
	m_fUpSpeed		 = 50.0f;
	m_fRotSpeed		 = 60.0f;
	m_fSpeedUpFactor = 4.0f;
}

ioFPSCameraController::~ioFPSCameraController()
{
}

void ioFPSCameraController::Update( ioCameraInput *pInput )
{
	ioFPSCameraInput *pFPSInput = ToFPSInput( pInput );
	if( !pFPSInput )	return;

	if( pFPSInput->m_bMiddle )		// m_fPitch
	{
		m_fHead -= pFPSInput->m_fXMoveAmt * 0.15f;
		m_fHead = ioMath::ArrangeHead( m_fHead );

		m_fPitch -= pFPSInput->m_fYMoveAmt * 0.1f;
		if( m_fPitch >= 89.0f )	 m_fPitch = 89.0f;
		if( m_fPitch <= -89.0f ) m_fPitch = -89.0f;
	}

	if( pFPSInput->m_bLeftRot || pFPSInput->m_bRightRot )
	{
		// m_fHead
		if( pFPSInput->m_bLeftRot )
		{
			m_fHead -= m_fRotSpeed * 0.01f;
		}
		else if( pFPSInput->m_bRightRot )
		{
			m_fHead += m_fRotSpeed * 0.01f;
		}

		m_fHead = ioMath::ArrangeHead( m_fHead );
	}	

	float dt = 0.1f;
	if( pFPSInput->m_bSpeedUp )
		dt *= m_fSpeedUpFactor;

	float dx, dz;
	dx = sinf( DEGtoRAD(m_fHead) );
	dz = cosf( DEGtoRAD(m_fHead) );

	if( pFPSInput->m_bUp )
		m_vPos.y += m_fUpSpeed * dt;

	if( pFPSInput->m_bDown )
		m_vPos.y -= m_fUpSpeed * dt;

	if( pFPSInput->m_bForward )
	{
		m_vPos.x += m_fForwardSpeed * dx * dt;
		m_vPos.z += m_fForwardSpeed * dz * dt;
	}

	if( pFPSInput->m_bBack )
	{
		m_vPos.x -= m_fForwardSpeed * dx * dt;
		m_vPos.z -= m_fForwardSpeed * dz * dt;
	}

	if( pFPSInput->m_bRight )
	{
		m_vPos.x += m_fStrafeSpeed * dz * dt;
		m_vPos.z -= m_fStrafeSpeed * dx * dt;
	}

	if( pFPSInput->m_bLeft )
	{
		m_vPos.x -= m_fStrafeSpeed * dz * dt;
		m_vPos.z += m_fStrafeSpeed * dx * dt;
	}
}

void ioFPSCameraController::CreateViewMatrix( D3DXMATRIX *pViewMat ) const
{
	float fHeadR, fPitchR, fRollR;
	fHeadR  = DEGtoRAD( m_fHead );
	fPitchR = DEGtoRAD( m_fPitch );
	fRollR  = DEGtoRAD( m_fRoll );

	D3DXMATRIX matTrans;
	D3DXMatrixTranslation( &matTrans, -m_vPos.x, -m_vPos.y, -m_vPos.z );

	D3DXMATRIX matRotH, matRotP, matRotR;
	D3DXMatrixRotationY( &matRotH, -fHeadR );
	D3DXMatrixRotationX( &matRotP, -fPitchR );
	D3DXMatrixRotationZ( &matRotR, -fRollR );

	*pViewMat = matTrans * matRotH * matRotP * matRotR;
}

const char* ioFPSCameraController::GetName()
{
	return "FPS Camera";
}

D3DXVECTOR3 ioFPSCameraController::GetDirection() const
{
	D3DXQUATERNION qtRot = ioMath::MakeQuaternionByDegree( m_fHead, m_fPitch, m_fRoll );

	return qtRot * ioMath::UNIT_Z;
}

void ioFPSCameraController::SetSpeed( float fForward, float fStrafe, float fUp, float fRot )
{
	m_fForwardSpeed = fForward;
	m_fStrafeSpeed  = fStrafe;
	m_fUpSpeed	    = fUp;
	m_fRotSpeed		= fRot;
}
