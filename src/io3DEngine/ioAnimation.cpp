

#include "stdafx.h"

#include "ioResource.h"
#include "ioRcAniTrack.h"
#include "ioRcAnimation.h"

#include "ioAnimation.h"
#include "ioResourceManager.h"
#include "ioAnimationManager.h"

#include "ioAniController.h"
#include "ioSkeleton.h"
#include "ioAniTrack.h"

#include "ioFrameTimer.h"

#define GUARANTEE_OUTTIME 100.0f

ioAnimation::ioAnimation( ioRcAnimation *pRcAnimation )
{
	m_pRcAnimation = pRcAnimation;
	m_pAniCtrl = NULL;

	m_fFullTime = m_pRcAnimation->GetFullTime();
	m_fWaitTime = 0.0f;

	m_fPreTime  = 0.0f;
	m_fCurTime  = 0.0f;
	m_fMidStartTime = 0.0f;

	m_fCurWeight	= 0.0f;
	m_fTargetWeight = 0.0f;
	m_fTimeFactor	= 1.0f;

	m_eAniType	= AT_ACTION;
	m_eAniState = AS_WAIT;

	m_fFadeIn = 0.0f;
	m_fFadeOut = 0.0f;

	m_fActPreDelay = 0.0f;
	m_fActPreDelayPast = 0.0f;

	m_fActForceOut = 0.0f;
	m_fLoopFadeOut = 0.0f;

	m_fGuaranteeOut = 0.0f;

	m_bAniStop = false;
	m_bHoldLastFrame = false;
	m_bNeedEndNotify = false;
}

ioAnimation::~ioAnimation()
{
	ClearAniTrackList();

	if( m_pRcAnimation )
	{
		ioAnimationManager::GetSingleton().Destroy( m_pRcAnimation );
		m_pRcAnimation = NULL;
	}
}

void ioAnimation::NotifyAttached( ioAniController *pParent )
{
	m_pAniCtrl = pParent;
}

void ioAnimation::BuildAniTrackList( ioSkeleton *pSkel )
{
	if( !m_pRcAnimation || !pSkel )
		return;

	ClearAniTrackList();

	int iTrackCount = m_pRcAnimation->GetTrackCount();
	m_AniTrackList.reserve( iTrackCount );

	for( int i=0 ; i<iTrackCount ; i++ )
	{
		ioRcAniTrack *pRcAniTrack = m_pRcAnimation->GetAniTrack(i);
		if( !pRcAniTrack ) continue;

		ioAniTrack *pTrack = new ioAniTrack( pRcAniTrack );
		if( pTrack )
		{
			int iBipedIndex = pSkel->GetBipedIndex( pTrack->GetBipedName() );
			pTrack->SetBipedID( iBipedIndex );

			m_AniTrackList.push_back( pTrack );
		}
	}
}

void ioAnimation::Update( float fTime )
{
	if( m_bAniStop )	return;

	fTime *= m_fTimeFactor;

	switch( m_eAniType )
	{
	case AT_ACTION:
		UpdateActionAni( fTime );
		break;
	case AT_LOOP:
		UpdateLoopAni( fTime );
		break;
	}
}

void ioAnimation::BlendSkeleton( ioSkeleton *pSkeleton )
{
	if( IsWaiting() ) return;

	D3DXVECTOR3 vTrans;
	D3DXQUATERNION qtRot;

	int iBipedCount = pSkeleton->GetBipedCnt();
	bool bResult = false;

	AniTrackList::iterator iter;
	for( iter=m_AniTrackList.begin() ; iter!=m_AniTrackList.end() ; ++iter )
	{
		ioAniTrack *pTrack = *iter;

		if( COMPARE( pTrack->GetBipedID(), 0, iBipedCount ) )
		{
			bResult = true;
			pTrack->GetInterpolatedFrame( m_fCurTime, vTrans, qtRot );

			pSkeleton->BlendBiped( pTrack->GetBipedID(),
								   pTrack->GetWeight() * m_fCurWeight,
								   vTrans,
								   qtRot );
		}
	}
}

void ioAnimation::UpdateAniEvent()
{
	if( !m_pAniCtrl || !m_pRcAnimation )
		return;

	if( m_eAniType == AT_ACTION )
	{
		if( m_fPreTime < m_fCurTime )
		{
			const AniEventList &rkEventList = m_pRcAnimation->GetEventList();

			AniEventList::const_iterator iter;
			for( iter = rkEventList.begin() ; iter!=rkEventList.end() ; ++iter )
			{
				if( COMPARE( iter->fEventTime, m_fPreTime, m_fCurTime ) )
				{
					if( m_eAniState != AS_ACT_FORCE_OUT )
						m_pAniCtrl->NotifyAniEvent( &(*iter), this );

					m_pAniCtrl->ChangeAniEvent( &(*iter), this );
				}
			}
		}
	}
	else	// AT_LOOP
	{
		const AniEventList &rkEventList = m_pRcAnimation->GetEventList();

		AniEventList::const_iterator iter;
		for( iter = rkEventList.begin() ; iter!=rkEventList.end() ; ++iter )
		{
			if( m_fPreTime < m_fCurTime )
			{
				if( COMPARE( iter->fEventTime, m_fPreTime, m_fCurTime ) )
				{
					if( m_eAniState	!= AS_FADE_OUT )
						m_pAniCtrl->NotifyAniEvent( &(*iter), this );

					//내정보 애니메이션 이벤트 때문에 추가 (jump_anitime 이벤트)
					m_pAniCtrl->ChangeAniEvent( &(*iter), this );
				}
			}
			else if( m_fPreTime > m_fCurTime )
			{
				if( COMPARE( iter->fEventTime, m_fPreTime, m_fFullTime ) ||
					COMPARE( iter->fEventTime, 0.0f, m_fCurTime ) )
				{
					if( m_eAniState	!= AS_FADE_OUT )
						m_pAniCtrl->NotifyAniEvent( &(*iter), this );
				}
			}
			// m_fPreTime == m_fCurTime 인 경우는 처리안함 ( 캐릭터툴에서 정지동작시 무한 이펙트 발생때문..)
		}
	}
}

void ioAnimation::ExecuteAni( float fWeight,
							  float fTimeFactor,
							  float fFadeIn,
							  float fFadeOut,
							  float fWaitTime,
							  float fPreDelay,
							  bool bHoldLastFrame )
{
	m_eAniType = AT_ACTION;

	if( fWaitTime > 0.0f )
	{
		m_eAniState = AS_WAIT;
	}
	else
	{
		m_eAniState = AS_FADE_IN;
	}

	m_fWaitTime = fWaitTime * fTimeFactor;
	m_fCurTime  = 0.0f;

	m_fCurWeight	= 0.0f;
	m_fTargetWeight = fWeight;

	// 임시 :
	if( fWaitTime == 0.0f && fFadeIn == 0 )
	{
		m_eAniState = AS_PLAY;
		m_fCurWeight = m_fTargetWeight;
	}

	m_fTimeFactor   = fTimeFactor;

	m_fFadeIn  = fFadeIn;
	m_fFadeOut = fFadeOut;

	m_fActPreDelay = fPreDelay;
	m_fActPreDelayPast = 0.0f;

	m_bHoldLastFrame = bHoldLastFrame;
}

void ioAnimation::ForceEndAction( float fFadeOut )
{
	if( m_eAniState == AS_ACT_GUARANTEE ||
		m_eAniState == AS_ACT_GUARANTEE_OUT ||
		m_eAniState == AS_ACT_GUARANTEE_END )
	{
		return;
	}

	if( m_eAniState == AS_ACT_FORCE_OUT && m_fActForceOut < fFadeOut )
		return;

	m_eAniState	   = AS_ACT_FORCE_OUT;
	m_fFadeOut     = fFadeOut;
	m_fActForceOut = fFadeOut;

	// 임시 :
	if( fFadeOut == 0 )
	{
		m_fActForceOut = 0.0f;
		m_fFadeOut   = 0.0f;
		m_fCurWeight = 0.0f;
		
		m_eAniState	= AS_END;
	}

	m_bAniStop = false;
	m_bHoldLastFrame = false;
}

void ioAnimation::SetGuaranteeState()
{
	m_eAniState  = AS_ACT_GUARANTEE;
	m_fPreTime   = m_fFullTime;
	m_fCurTime   = m_fFullTime;
	m_fCurWeight = FLOAT05;
}

void ioAnimation::SetGuaranteeOut()
{
	m_eAniState = AS_ACT_GUARANTEE_OUT;
	m_fGuaranteeOut = GUARANTEE_OUTTIME;
}

void ioAnimation::UpdateActionAni( float fTime )
{
	if( m_eAniState == AS_ACT_GUARANTEE )
	{
		return;
	}
	else if( m_eAniState == AS_ACT_GUARANTEE_OUT )
	{
		m_fGuaranteeOut -= fTime;

		if( m_fGuaranteeOut > 0.0f )
		{
			m_fCurWeight = ( m_fGuaranteeOut / GUARANTEE_OUTTIME ) * FLOAT05;
			m_fCurWeight = max( m_fCurWeight, 0.1f );
		}
		else
		{
			m_fCurWeight = 0.0f;
			m_eAniState = AS_ACT_GUARANTEE_END;
		}

		return;
	}
	else if( m_eAniState == AS_ACT_FORCE_OUT )
	{
		float fCurWeight;
		if( m_fCurTime < m_fFadeIn )
		{
			fCurWeight = (m_fCurTime/m_fFadeIn) * m_fTargetWeight;
		}
		else
		{
			fCurWeight = m_fTargetWeight;
		}

		m_fActForceOut -= fTime;

		if( m_fActForceOut > 0.0f )
		{
			m_fCurWeight = ( m_fActForceOut/m_fFadeOut ) * fCurWeight;
		}
		else
		{
			m_fActForceOut = 0.0f;
			m_fCurWeight = 0.0f;
			m_eAniState = AS_END;
		}

		return;
	}

	if( m_eAniState == AS_WAIT )
	{
		m_fWaitTime -= fTime;

		if( m_fWaitTime <= 0.0f )
		{
			m_eAniState = AS_FADE_IN;
			fTime = -m_fWaitTime;
			m_fWaitTime = 0.0f;
			m_fCurTime = 0.0f;
		}
		else
		{
			return;
		}
	}

	m_fPreTime = m_fCurTime;

	if( m_fActPreDelayPast < m_fActPreDelay )
	{
		m_fActPreDelayPast += fTime / m_fTimeFactor;
		if( m_fActPreDelayPast > m_fActPreDelay )
		{
			m_fCurTime = m_fActPreDelayPast - m_fActPreDelay;
			m_fActPreDelayPast = m_fActPreDelay;
		}
	}
	else
	{
		m_fCurTime += fTime;
	}

	UpdateAniEvent();

	if( m_eAniState == AS_FADE_IN )
	{
		if( m_fCurTime < m_fFadeIn )
		{
			m_fCurWeight = ( m_fActPreDelayPast + m_fCurTime ) /
						   ( m_fActPreDelay + m_fFadeIn ) * m_fTargetWeight;
		}
		else
		{
			m_eAniState = AS_PLAY;
			m_fCurWeight = m_fTargetWeight;
		}
	}

	if( m_eAniState == AS_PLAY )
	{
		if( m_fCurTime >= m_fFullTime - m_fFadeOut )
		{
			m_eAniState = AS_FADE_OUT;
		}
	}

	if( m_eAniState == AS_FADE_OUT )
	{
		if( m_bHoldLastFrame )
		{
			if( m_fCurTime > m_fFullTime )
			{
				m_fCurTime = m_fFullTime;
			}
		}
		else
		{
			if( m_fCurTime < m_fFullTime )
			{
				m_fCurWeight = ( m_fFullTime - m_fCurTime ) / m_fFadeOut * m_fTargetWeight;
			}
			else
			{
				m_fCurTime   = m_fFullTime;
				m_fFadeOut   = 0.0f;
				m_fCurWeight = 0.0f;
				m_eAniState  = AS_END;
			}
		}
	}
}

void ioAnimation::UpdateLoopAni( float fTime )
{
	if( m_eAniState == AS_WAIT )
	{
		m_fWaitTime -= fTime;
		if( m_fWaitTime <= 0.0f )
		{
			m_eAniState = AS_FADE_IN;
			fTime += m_fWaitTime;
			m_fWaitTime = 0.0f;
			m_fCurTime = 0.0f;
		}
		else
		{
			return;
		}
	}

	m_fPreTime = m_fCurTime;
	m_fCurTime += fTime;

	if( m_eAniState == AS_FADE_IN )
	{
		float fTimePast = 0.0f;
		if( m_fCurTime >= m_fMidStartTime )
		{
			fTimePast = m_fCurTime - m_fMidStartTime;
		}
		else
		{
			fTimePast = m_fCurTime + m_fFullTime - m_fMidStartTime;
		}

		if( fTimePast < m_fFadeIn )
		{
			m_fCurWeight = (fTimePast/m_fFadeIn) * m_fTargetWeight;
		}
		else
		{
			m_eAniState = AS_PLAY;
			m_fCurWeight = m_fTargetWeight;
			m_fMidStartTime = 0.0f;
		}
	}

	if( m_fCurTime > m_fFullTime )
	{
		m_fCurTime -= m_fFullTime;
	}

	UpdateAniEvent();

	if( m_eAniState == AS_FADE_OUT )
	{
		m_fFadeOut -= fTime;

		if( m_fFadeOut > 0.0f )
		{
			m_fCurWeight = (m_fFadeOut/m_fLoopFadeOut) * m_fTargetWeight;
		}
		else
		{
			m_fFadeOut   = 0.0f;
			m_fCurWeight = 0.0f;
			m_eAniState  = AS_END;
		}
	}
}

void ioAnimation::ClearAniTrackList()
{
	AniTrackList::iterator iter;
	for( iter=m_AniTrackList.begin() ; iter!=m_AniTrackList.end() ; ++iter )
	{
		delete *iter;
	}

	m_AniTrackList.clear();
}

void ioAnimation::StartLoop( float fWeight, float fFadeIn, float fTimeFactor, float fWaitTime )
{
	m_eAniType = AT_LOOP;
	
	if( m_eAniState == AS_WAIT )	// 초기 생성
	{
		if( fWaitTime > 0.0f )
		{
			m_eAniState = AS_WAIT;
		}
		else
		{
			m_eAniState = AS_FADE_IN;
		}

		m_fWaitTime	= fWaitTime * fTimeFactor;
		m_fCurTime  = 0.0f;
		m_fCurWeight = 0.0f;
		m_fFadeIn  = fFadeIn;
	}
	else if( m_eAniState == AS_PLAY || m_eAniState == AS_FADE_OUT )
	{
		/*
		m_eAniState  = AS_PLAY;
		m_fCurWeight = fWeight;
		*/

		m_eAniState = AS_FADE_IN;
		m_fWaitTime	= fWaitTime * fTimeFactor;
		m_fCurTime  = 0.0f;
		m_fCurWeight = 0.0f;
		m_fFadeIn  = fFadeIn;
	}

	/*
	else if( m_eAniState == AS_PLAY || (m_eAniState == AS_FADE_OUT && m_fFadeOut > 0.0f) )
	{
		m_eAniState  = AS_PLAY;
		m_fCurWeight = fWeight;
	}
	else if( m_eAniState == AS_FADE_OUT && m_fFadeOut <= 0.0f )
	{
		m_eAniState = AS_FADE_IN;
		m_fWaitTime	= fWaitTime * fTimeFactor;
		m_fCurTime  = 0.0f;
		m_fCurWeight = 0.0f;
		m_fFadeIn  = fFadeIn;
	}
	*/

	m_fMidStartTime = 0.0f;
	m_fTargetWeight = fWeight;
	m_fTimeFactor   = fTimeFactor;
	m_fFadeOut = 0.0f;
	m_fLoopFadeOut = 0.0f;
}

void ioAnimation::StartLoopInMid( float fStartTime,
								  float fWeight,
								  float fFadeIn,
								  float fTimeFactor )
{
	m_eAniType = AT_LOOP;
	m_eAniState = AS_FADE_IN;

	m_fWaitTime	= 0.0f;
	m_fCurTime  = fStartTime;
	m_fMidStartTime = fStartTime;

	m_fCurWeight = 0.0f;
	m_fFadeIn  = fFadeIn;

	m_fTargetWeight = fWeight;
	m_fTimeFactor   = fTimeFactor;
	m_fFadeOut = 0.0f;
	m_fLoopFadeOut = 0.0f;
}

void ioAnimation::EndLoop( float fFadeOut, bool bAlsoWait )
{
	if(m_eAniState != AS_FADE_OUT )
	{
		m_eAniState	= AS_FADE_OUT;

		if( !bAlsoWait || (m_fWaitTime == 0.0f) )
		{
			m_fFadeOut	   = fFadeOut;
			m_fLoopFadeOut = max( 1.0f, fFadeOut );
		}
		else
		{
			m_fFadeOut	   = 0.0f;
			m_fLoopFadeOut = 1.0f;
		}

		m_fMidStartTime = 0.0f;
		m_fTimeFactor = 1.0f;
	}
	/*
	else
	{
		if( !bAlsoWait || (m_fWaitTime == 0.0f) )
		{
			m_fFadeOut	   = fFadeOut;
			m_fLoopFadeOut = max( 1.0f, fFadeOut );
		}
		else
		{
			m_fFadeOut	   = 0.0f;
			m_fLoopFadeOut = 1.0f;
		}
	}
	*/

	m_bAniStop = false;
}

void ioAnimation::SetCurTime( float fTime )
{
	m_fCurTime = max( 0, min( fTime, m_fFullTime ) );
}

void ioAnimation::ForceWeight( float fWeight )
{
	m_fTargetWeight = fWeight;

	if( m_eAniState == AS_PLAY )
	{
		m_fCurWeight = fWeight;
	}
}

float ioAnimation::GetTrackWeight( int iTrackID ) const
{
	if( COMPARE( iTrackID, 0, GetTrackCount() ) )
		return m_AniTrackList[iTrackID]->GetWeight();

	return 0.0f;	// error
}

float ioAnimation::GetTrackWeight( const ioHashString &kName ) const
{
	ioAniTrack *pTrack = GetAniTrack( kName );
	if( pTrack )
		return pTrack->GetWeight();

	return 0.0f;	// error
}

ioAniTrack* ioAnimation::GetAniTrack( int iTrackID ) const
{
	if( COMPARE( iTrackID, 0, GetTrackCount() ) )
		return m_AniTrackList[iTrackID];

	return NULL;
}

ioAniTrack* ioAnimation::GetAniTrack( const ioHashString &rkBipedName ) const
{
	AniTrackList::const_iterator iter;
	for( iter=m_AniTrackList.begin() ; iter!=m_AniTrackList.end() ; ++iter )
	{
		if( (*iter)->GetBipedName() == rkBipedName )
			return *iter;
	}

	return NULL;
}

const ioHashString& ioAnimation::GetName() const
{
	return m_pRcAnimation->GetName();
}

int ioAnimation::GetEventCnt() const
{
	return m_pRcAnimation->GetEventCnt();
}

float ioAnimation::GetEventTime( const ioHashString &szEventType ) const
{
	return m_pRcAnimation->GetEventTime( szEventType );
}
