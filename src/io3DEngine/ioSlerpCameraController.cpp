

#include "stdafx.h"

#include "ioFrameTimer.h"

#include "ioCameraController.h"
#include "ioSlerpCameraController.h"
#include "ioMath.h"

ioSlerpCameraController::ioSlerpCameraController()
{
}

ioSlerpCameraController::~ioSlerpCameraController()
{
}

void ioSlerpCameraController::SetLookAt( const D3DXVECTOR3 &vLookAt )
{
	m_vLookAt = vLookAt;
}

void ioSlerpCameraController::AddNextTarget( const D3DXVECTOR3 &vPos,
											 const D3DXVECTOR3 &vLookAt,
											 int iTime )
{
	SlerpTarget kTarget;
	kTarget.vPosition = vPos;
	kTarget.vLookAt	  = vLookAt;
	kTarget.iTime     = max( iTime, 1 );

	m_vSlerpList.push_back( kTarget );
}

bool ioSlerpCameraController::IsNowSlerping() const
{
	if( m_vSlerpList.empty() )
		return false;

	return true;
}

void ioSlerpCameraController::ClearSlerping()
{
	m_vSlerpList.clear();
}

void ioSlerpCameraController::Update( ioCameraInput *pInput )
{
	if( !IsNowSlerping() )	return;

	int iCurTimeGap = g_FrameTimer.GetCurFrameGap();
	int iTotalGap = 0;
	int iPopCnt = 0;

	SlerpTargetList::iterator iter;
	for( iter=m_vSlerpList.begin(); iter!=m_vSlerpList.end() ; ++iter, iPopCnt++ )
	{
		iTotalGap += iter->iTime;

		if( iTotalGap > iCurTimeGap )
			break;
	}

	if( iTotalGap <= iCurTimeGap )	// 남은 Target 갯수가 모자르거나 끝에 딱 걸림
	{
		SlerpTarget &rkLast = m_vSlerpList.back();
		m_vPos = rkLast.vPosition;
		m_vLookAt = rkLast.vLookAt;

		m_vSlerpList.clear();
	}
	else
	{
		do
		{
			SlerpTarget &kCurTarget = m_vSlerpList.front();

			if( iCurTimeGap >= kCurTarget.iTime )
			{
				m_vPos = kCurTarget.vPosition;
				m_vLookAt = kCurTarget.vLookAt;

				iCurTimeGap -= kCurTarget.iTime;
				m_vSlerpList.pop_front();
			}
			else
			{
				float fSlerpRate = (float)iCurTimeGap/(float)kCurTarget.iTime;
				m_vPos    = ioMath::Lerp( fSlerpRate, m_vPos, kCurTarget.vPosition );
				m_vLookAt = ioMath::Lerp( fSlerpRate, m_vLookAt, kCurTarget.vLookAt );

				kCurTarget.iTime -= iCurTimeGap;
			}

			iPopCnt--;
		} while( iPopCnt >= 0 );
	}
}

void ioSlerpCameraController::CreateViewMatrix( D3DXMATRIX *pViewMat ) const
{
	D3DXMatrixLookAtLH( pViewMat,
						&m_vPos,
						&m_vLookAt,
						&ioMath::UNIT_Y );
}

D3DXVECTOR3 ioSlerpCameraController::GetDirection() const
{
	D3DXVECTOR3 vDir = m_vLookAt - m_vPos;
	D3DXVec3Normalize( &vDir, &vDir	);

	return vDir;
}

const char* ioSlerpCameraController::GetName()
{
	return "Slerp Camera";
}