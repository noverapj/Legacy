

#include "stdafx.h"

#include "ioCameraController.h"
#include "ioMath.h"

ioCameraInput::ioCameraInput()
{
	m_fXMoveAmt = 0.0f;
	m_fYMoveAmt = 0.0f;
	m_fWheelAmt = 0.0f;
}

ioCameraInput::~ioCameraInput()
{
}

ioCameraController::ioCameraController() 
: m_vPos( 0.0f, 0.0f, 0.0f )
{
	m_fHead	 = 0.0f;
	m_fPitch = 0.0f;
	m_fRoll	 = 0.0f;
}

ioCameraController::~ioCameraController()
{
}

void ioCameraController::SetRollValue( float fRoll )
{
	m_fRoll = ioMath::ArrangeHead( fRoll );
}

void ioCameraController::SetPosition( const D3DXVECTOR3 &vPos )
{
	m_vPos = vPos;
}

void ioCameraController::SetDirection( float fHead, float fPitch, float fRoll )
{
	m_fHead	 = fHead;
	m_fPitch = fPitch;
	m_fRoll  = fRoll;
}

void ioCameraController::GetExtraInfoText( char *szBuf, int iLen ) const
{
	memset( szBuf, 0, iLen );
	strncpy( szBuf, "Extra Info - Nothing", iLen-1 );
}