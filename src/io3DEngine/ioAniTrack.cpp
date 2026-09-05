

#include "stdafx.h"

#include "ioRcAniTrack.h"
#include "ioAniTrack.h"

ioAniTrack::ioAniTrack( const ioRcAniTrack *pRcTrack )
{
	m_pRcTrack = pRcTrack;
	m_iPreFrameIndex = 0;
	m_iBipedID = 0;

	m_fWeight = m_pRcTrack->GetWeight();
}

ioAniTrack::~ioAniTrack()
{
}

void ioAniTrack::GetInterpolatedFrame( float fCurTime,
									   D3DXVECTOR3 &vTrans,
									   D3DXQUATERNION &qtRot )
{
	m_iPreFrameIndex = m_pRcTrack->GetInterpolatedFrame( fCurTime,
														 m_iPreFrameIndex,
														 vTrans,
														 qtRot );
}

void ioAniTrack::GetNoInterpolatedFrame( float fCurTime,
										 D3DXVECTOR3 &vTrans,
										 D3DXQUATERNION &qtRot )
{
	m_iPreFrameIndex = m_pRcTrack->GetNoInterpolatedFrame( fCurTime,
														   m_iPreFrameIndex,
														   vTrans,
														   qtRot );
}

const ioHashString& ioAniTrack::GetBipedName() const
{
	return m_pRcTrack->GetRcBipedName();
}
