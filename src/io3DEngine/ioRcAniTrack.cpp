
#include "StdAfx.h"

#include "ioMath.h"
#include "HelpFunc.h"

#include "ioRcAniTrack.h"

ioRcAniTrack::ioRcAniTrack()
{
	m_fWeight = 1.0f;
}

ioRcAniTrack::ioRcAniTrack( const ioHashString &szBipedName ) 
: m_BipedName( szBipedName )
{
	m_fWeight = 1.0f;
}

ioRcAniTrack::~ioRcAniTrack()
{
}

void ioRcAniTrack::ReserveFrameList( int iFrameCount )
{
	m_vKeyTimeList.reserve( iFrameCount );
	m_vKeyFrameList.reserve( iFrameCount );
}

void ioRcAniTrack::AddKeyFrame( float fTime,
								const D3DXVECTOR3 &vTrans,
								const D3DXQUATERNION &qtRot )
{
	m_vKeyTimeList.push_back( fTime );
	m_vKeyFrameList.push_back( KeyFrame( qtRot, vTrans ) );
}

int ioRcAniTrack::GetInterpolatedFrame( float fTime,
										int iHintFrame,
										D3DXVECTOR3 &vTrans,
										D3DXQUATERNION &qtRot ) const
{
	int iFrame = GetFrameIndex( fTime, iHintFrame );
	int iFrameCount = GetFrameCount();

	if( 0 < iFrame && iFrame < iFrameCount )
	{
		--iFrame;

		float fBeforeTime = m_vKeyTimeList[iFrame];
		float fAfterTime  = m_vKeyTimeList[iFrame+1];
		float fBlend = ( fTime - fBeforeTime ) / ( fAfterTime - fBeforeTime );

		//UJ 130911, 최적화: KeyFrame 복사가 느리므로 포인터를 사용
		//KeyFrame kBeforeFrame = m_vKeyFrameList[iFrame];
		//KeyFrame kAfterFrame  = m_vKeyFrameList[iFrame+1];
		const KeyFrame* pkBeforeFrame = &m_vKeyFrameList[iFrame];
		const KeyFrame* pkAfterFrame  = &m_vKeyFrameList[iFrame+1];

		qtRot  = ioMath::Slerp( fBlend, pkBeforeFrame->m_qtRot, pkAfterFrame->m_qtRot );
		vTrans = ioMath::Lerp( fBlend, pkBeforeFrame->m_vTrans, pkAfterFrame->m_vTrans );
	}
	else if( iFrame == 0 )
	{
		qtRot  = m_vKeyFrameList[iFrame].m_qtRot;
		vTrans = m_vKeyFrameList[iFrame].m_vTrans;
	}
	else if( iFrame == iFrameCount )
	{
		--iFrame;

		qtRot  = m_vKeyFrameList[iFrame].m_qtRot;
		vTrans = m_vKeyFrameList[iFrame].m_vTrans;
	}

	return iFrame;
}

int  ioRcAniTrack::GetNoInterpolatedFrame( float fTime,
		 								   int iHintFrame,
										   D3DXVECTOR3 &vTrans,
										   D3DXQUATERNION &qtRot ) const
{
	int iFrame = GetFrameIndex( fTime, iHintFrame );
	if( iFrame == GetFrameCount() )
		--iFrame;

	qtRot  = m_vKeyFrameList[iFrame].m_qtRot;
	vTrans = m_vKeyFrameList[iFrame].m_vTrans;

	return iFrame;
}

int ioRcAniTrack::GetFrameIndex( float fTime, int iHintFrame ) const
{
	int iMaxFrameCount = GetFrameCount();
	if( iMaxFrameCount <= 1 )
		return 0;

	if( !COMPARE( iHintFrame, 0, iMaxFrameCount ) )
		iHintFrame = max( 0, min( iHintFrame, iMaxFrameCount-1 ) );

	int iFrame = 0;
	if( m_vKeyTimeList[iHintFrame] < fTime )
	{
		//UJ 130903, 최적화: iHintFrame은 이미 비교했기때문에 +1해서 for에 넘겨서 그다음부터 비교해도됨 --start
		//iFrame = iHintFrame;
		iFrame = iHintFrame+1;
		//UJ --end
	}

	for( ; iFrame<iMaxFrameCount ; iFrame++ )
	{
		if( m_vKeyTimeList[iFrame] > fTime )
			return iFrame;
	}

	return iFrame;
}

float ioRcAniTrack::GetKeyTime( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetFrameCount() ) )
		return m_vKeyTimeList[iIndex];

	LOG.PrintTimeAndLog( 0, "ioRcAniTrack::GetKeyTime - overflow %d/%d", iIndex, GetFrameCount() );

	return 0.0f;
}

D3DXQUATERNION ioRcAniTrack::GetKeyOrientation( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetFrameCount() ) )
		return m_vKeyFrameList[iIndex].m_qtRot;

	LOG.PrintTimeAndLog( 0, "ioRcAniTrack::GetKeyOrientation - overflow %d/%d", iIndex, GetFrameCount() );

	return ioMath::QUAT_IDENTITY;
}

D3DXVECTOR3 ioRcAniTrack::GetKeyPosition( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetFrameCount() ) )
		return m_vKeyFrameList[iIndex].m_vTrans;

	LOG.PrintTimeAndLog( 0, "ioRcAniTrack::GetKeyPosition - overflow %d/%d", iIndex, GetFrameCount() );

	return ioMath::VEC3_ZERO;
}