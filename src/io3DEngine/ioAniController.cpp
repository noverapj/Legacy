

#include "StdAfx.h"

#include "ioSphere.h"
#include "ioOrientBox.h"

#include "ioResource.h"
#include "ioRcSkeleton.h"

#include "ioRenderable.h"
#include "ioRenderOperation.h"
#include "ioMovableObject.h"

#include "ioNode.h"
#include "ioSceneNode.h"

#include "ioSkeleton.h"
#include "ioAnimation.h"
#include "ioAnimateFX.h"
#include "ioAniController.h"

#include "ioEntityGroup.h"
#include "ioAniEventHandler.h"

#include "ioAnimateFXManager.h"
#include "ioAnimationManager.h"

#include "ioFrameTimer.h"

#include "HelpFunc.h"
#include "ErrorReport.h"

bool ioAniController::m_bGuaranteeAtLeastOneAnimation = false;

ioAniController::ioAniController( ioEntityGroup *pParent )
{
	m_pParent = pParent;
	m_pEventHandler = new ioAniEventHandler;
	m_pEventHandler->SetOwner( pParent );

	m_bAnimationStop = false;
}

ioAniController::~ioAniController()
{
	AnimateFXList::iterator iterFX;
	for( iterFX=m_AnimateFXList.begin() ; iterFX!=m_AnimateFXList.end() ; ++iterFX )
	{
		delete *iterFX;
	}
	m_AnimateFXList.clear();

	ioAniList::iterator iter;
	for( iter=m_ActionList.begin() ; iter!=m_ActionList.end() ; ++iter )
	{
		delete *iter;
	}
	m_ActionList.clear();

	for( iter=m_LoopList.begin() ; iter!=m_LoopList.end() ; ++iter )
	{
		delete *iter;
	}
	m_LoopList.clear();

	SAFEDELETE( m_pEventHandler );
}

void ioAniController::FindActionAniInPlay( int id, ioAniList &vAniList )
{
	if( COMPARE( id, 0, m_pParent->GetAnimationCnt() ) )
	{
		FindActionAniInPlay( m_pParent->GetAnimationName( id ),
							 vAniList );

		return;
	}

	LOG.PrintTimeAndLog( 0, "ioAniController::FindActionAniInPlay - over (%d/%d)",
							 id, m_pParent->GetAnimationCnt() );
}

void ioAniController::FindActionAniInPlay( const ioHashString &kAniName, ioAniList &vAniList )
{
	ioAniList::iterator iter;
	for( iter=m_ActionList.begin() ; iter!=m_ActionList.end() ; ++iter )
	{
		if( (*iter)->GetName() == kAniName )
		{
			vAniList.push_back( *iter );
		}
	}
}

ioAnimation* ioAniController::FindLoopAniInPlay( const ioHashString &rkName )
{
	ioAniList::iterator iter;
	for( iter=m_LoopList.begin() ; iter!=m_LoopList.end() ; ++iter )
	{
		if( (*iter)->GetName() == rkName )
			return *iter;
	}

	return NULL;
}

ioAnimation* ioAniController::FindLoopAniInPlay( int id )
{
	if( !COMPARE( id, 0, m_pParent->GetAnimationCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioAniController::FindLoopAniInPlay - over (%d/%d)",
								id, m_pParent->GetAnimationCnt() );
		return NULL;
	}

	const ioHashString &rkName = m_pParent->GetAnimationName( id );

	ioAniList::iterator iter;
	for( iter=m_LoopList.begin() ; iter!=m_LoopList.end() ; ++iter )
	{
		if( (*iter)->GetName() == rkName )
			return *iter;
	}

	return NULL;
}

void ioAniController::StopAllPlayingAnimation()
{
	if( m_bAnimationStop )
		return;

	ioAniList::iterator iter = m_ActionList.begin();
	for( ; iter!=m_ActionList.end() ; ++iter )
	{
		(*iter)->StopAni();
		NotifyClearEvent( *iter );
	}

	iter = m_LoopList.begin();
	for( ; iter!=m_LoopList.end() ; ++iter )
	{
		(*iter)->StopAni();
		NotifyClearEvent( *iter );
	}

	m_bAnimationStop = true;
}

void ioAniController::ResumeAllPlayingAnimation()
{
	if( !m_bAnimationStop )
		return;

	ioAniList::iterator iter = m_ActionList.begin();
	for( ; iter!=m_ActionList.end() ; ++iter )
	{
		(*iter)->ResumeAni();
	}

	iter = m_LoopList.begin();
	for( ; iter!=m_LoopList.end() ; ++iter )
	{
		(*iter)->ResumeAni();
	}

	m_bAnimationStop = false;
}

void ioAniController::ExecuteAction( int id,
								     float fFadeIn,
									 float fFadeOut,
									 float fWeight,
									 float fTimeFactor,
									 float fWaitTime,
									 float fPreDelay,
									 bool bHoldLastFrame,
									 bool bUseFX )
{
	ioAnimation *pActionAni = g_AnimationMgr.CreateInstance( m_pParent->GetAnimationName( id ) );
	if( !pActionAni )	return;

	pActionAni->NotifyAttached( this );
	pActionAni->BuildAniTrackList( m_pParent->GetSkeleton() );

	pActionAni->ExecuteAni( fWeight,
							fTimeFactor,
							fFadeIn,
							fFadeOut,
							fWaitTime,
							fPreDelay,
							bHoldLastFrame );

	m_ActionList.push_front( pActionAni );

	CheckGuaranteeOut();
/*
	if( bUseFX && g_AnimateFXMgr.HasFxFileSet() )
	{
		ioAnimateFX *pAniFX = g_AnimateFXMgr.CreateFX( pActionAni->GetName(), true );
		if( pAniFX )
		{
			pAniFX->Initialize( fTimeFactor, m_pParent->GetSkeleton() );
			m_AnimateFXList.push_back( pAniFX );
			m_pParent->GetParentSceneNode()->AttachObject( pAniFX );
		}
	}
*/

	// юс╫ц :
	if( pActionAni->GetState() == ioAnimation::AS_PLAY )
	{
		float fTimePerSec = g_FrameTimer.GetSecPerFrame();
		UpdateSkeleton( fTimePerSec );
	}
}

void ioAniController::ClearAction( int id, float fFadeOut )
{
	ioAniList vAniList;
	FindActionAniInPlay( id, vAniList );

	if( !vAniList.empty() )
	{
		ioAniList::iterator iter = vAniList.begin();
		for( ; iter!=vAniList.end() ; ++iter )
		{
			(*iter)->ForceEndAction( fFadeOut );
			NotifyClearEvent( *iter );
		}
	}
}

void ioAniController::ExecuteLoop( int id,
								   float fFadeIn,
								   float fWeight,
								   float fTimeFactor,
								   float fWaitTime )
{
	if( !COMPARE( id, 0, m_pParent->GetAnimationCnt() ) )
		return;

	ioAnimation *pLoopAni = FindLoopAniInPlay( id );
	if( !pLoopAni )
	{
		pLoopAni = g_AnimationMgr.CreateInstance( m_pParent->GetAnimationName(id) );
		if( pLoopAni )
		{
			pLoopAni->NotifyAttached( this );
			pLoopAni->BuildAniTrackList( m_pParent->GetSkeleton() );
			m_LoopList.push_front( pLoopAni );
		}
	}

	if( pLoopAni )
	{
		pLoopAni->StartLoop( fWeight, fFadeIn, fTimeFactor, fWaitTime );
		CheckGuaranteeOut();
	}
}

void ioAniController::ExecuteLoopInMid( int id,
										float fStartTime,
										float fFadeIn,
										float fWeight,
										float fTimeFactor )
{
	if( !COMPARE( id, 0, m_pParent->GetAnimationCnt() ) )
		return;

	ioAnimation *pLoopAni = FindLoopAniInPlay( id );
	if( !pLoopAni )
	{
		pLoopAni = g_AnimationMgr.CreateInstance( m_pParent->GetAnimationName(id) );
		if( pLoopAni )
		{
			pLoopAni->NotifyAttached( this );
			pLoopAni->BuildAniTrackList( m_pParent->GetSkeleton() );
			m_LoopList.push_front( pLoopAni );
		}
	}

	if( pLoopAni )
	{
		pLoopAni->StartLoopInMid( fStartTime, fWeight, fFadeIn, fTimeFactor );
		CheckGuaranteeOut();
	}
}

void ioAniController::ClearLoop( int id, float fFadeOut, bool bAlsoWait )
{
	ioAnimation *pLoopAni = FindLoopAniInPlay( id );
	if( pLoopAni )
	{
		pLoopAni->EndLoop( fFadeOut, bAlsoWait );

		if( bAlsoWait )
		{
			NotifyClearEvent( pLoopAni );
		}
	}
}


void ioAniController::ClearAniEvent()
{
	if( m_pEventHandler )
	{
		m_pEventHandler->ClearAniEvent();
	}
}

void ioAniController::ClearAniEffect()
{
	if( m_pEventHandler )
	{
		m_pEventHandler->ClearAniEffect();
	}
}

void ioAniController::ClearAllAction( float fFadeOut, bool bAlsoWait )
{
	ioAniList::iterator iter;
	for( iter=m_ActionList.begin() ; iter!=m_ActionList.end() ; ++iter )
	{
		ioAnimation *pActionAni = *iter;
		if( bAlsoWait || !pActionAni->IsWaiting() )
		{
			pActionAni->ForceEndAction( fFadeOut );
			NotifyClearEvent( pActionAni );
		}
	}
}

void ioAniController::ClearAllLoop( float fFadeOut, bool bAlsoWait )
{
	ioAniList::iterator iter;
	for( iter = m_LoopList.begin() ; iter!=m_LoopList.end() ; ++iter )
	{
		(*iter)->EndLoop( fFadeOut, bAlsoWait );

		if( bAlsoWait )
		{
			ioAnimation *pLoopAni = *iter;
			NotifyClearEvent( pLoopAni );
		}
	}
}

bool ioAniController::ClearAllLoopExceptOne( int iExceptID, float fFadeOut, bool bWaitAlso )
{
	ioAnimation *pPlayAni = FindLoopAniInPlay( iExceptID );
	if( pPlayAni )
	{
		ioAniList::iterator iter;
		for( iter=m_LoopList.begin() ; iter!=m_LoopList.end() ; ++iter )
		{
			ioAnimation *pLoopAni = *iter;

			if( pLoopAni != pPlayAni )
			{
				pLoopAni->EndLoop( fFadeOut, bWaitAlso );
			}
		}

		return true;
	}
	else
	{
		ClearAllLoop( fFadeOut, bWaitAlso );
	}

	return false;
}

void ioAniController::Update( float fTime )
{
	if( m_bAnimationStop )
		return;

	UpdateAnimation( fTime );
	ClearEndedAnimation();

	UpdateSkeleton( fTime );
	UpdateAnimateFX( fTime );
}

void ioAniController::UpdateAnimation( float fTime )
{
	if( !m_ActionList.empty() )
	{
		ioAniList::iterator iter = m_ActionList.begin();
		for( ; iter!=m_ActionList.end(); ++iter )
		{
			(*iter)->Update( fTime );
		}
	}

	if( !m_LoopList.empty() )
	{
		ioAniList::iterator iter = m_LoopList.begin();
		for( ; iter!=m_LoopList.end(); ++iter )
		{
			(*iter)->Update( fTime );
		}
	}
}

void ioAniController::UpdateSkeleton( float fTime )
{
	ioSkeleton	*pSkeleton = m_pParent->GetSkeleton();
	if( !pSkeleton )	return;

	pSkeleton->ClearBlend();

	// Action Animation	
	if( !m_ActionList.empty() )
	{
		ioAniList::iterator iter = m_ActionList.begin();
		for( ; iter!=m_ActionList.end() ; ++iter )
		{
			(*iter)->BlendSkeleton( pSkeleton );
		}

		pSkeleton->LockBlend();
	}

	// Loop Animation
	if( !m_LoopList.empty() )
	{
		ioAniList::iterator iter = m_LoopList.begin();
		for( ; iter!=m_LoopList.end() ; ++iter )
		{
			(*iter)->BlendSkeleton( pSkeleton );
		}

		pSkeleton->LockBlend();
	}

	pSkeleton->UpdateSkeleton( fTime );
}

void ioAniController::UpdateAnimateFX( float fTime )
{
	if( m_AnimateFXList.empty() )
		return;

	AnimateFXList::iterator iter = m_AnimateFXList.begin();
	while( iter != m_AnimateFXList.end() )
	{
		ioAnimateFX *pFX = *iter;

		if( pFX->UpdateFX( fTime, m_pParent->GetSkeleton() ) )
		{
			++iter;
		}
		else
		{
			iter = m_AnimateFXList.erase( iter );
			delete pFX;
		}
	}
}

void ioAniController::ClearEndedAnimation()
{
	ioAnimation *pCurAni = NULL;

	ioAniList::iterator iter = m_LoopList.begin();
	while( iter != m_LoopList.end() )
	{
		pCurAni = *iter;

		if( pCurAni->GetState() != ioAnimation::AS_END )
		{
			++iter;
		}
		else
		{
			NotifyEndEvent( pCurAni );

			iter = m_LoopList.erase( iter );
			delete pCurAni;
		}
	}

	bool bGuanranteeLeastAnimation = false;
	if( m_bGuaranteeAtLeastOneAnimation && m_LoopList.empty() )
	{
		bGuanranteeLeastAnimation = true;
	}

	bool bAllBeforeGuarantee = true;

	iter = m_ActionList.begin();
	while( iter != m_ActionList.end() )
	{
		pCurAni = *iter;

		ioAnimation::AniState eState = pCurAni->GetState();
		if( eState < ioAnimation::AS_END )
		{
			bAllBeforeGuarantee = false;
			++iter;
		}
		else
		{
			switch( eState )
			{
			case ioAnimation::AS_END:
				NotifyEndEvent( pCurAni );

				if( bGuanranteeLeastAnimation && bAllBeforeGuarantee )
				{
					pCurAni->SetGuaranteeState();
					++iter;
				}
				else
				{
					iter = m_ActionList.erase( iter );
					delete pCurAni;
				}
				break;
			case ioAnimation::AS_ACT_GUARANTEE:
			case ioAnimation::AS_ACT_GUARANTEE_OUT:
				++iter;
				break;
			case ioAnimation::AS_ACT_GUARANTEE_END:
				iter = m_ActionList.erase( iter );
				delete pCurAni;
				break;
			}
		}
	}
}

void ioAniController::CheckGuaranteeOut()
{
	if( !m_bGuaranteeAtLeastOneAnimation )
		return;

	ioAniList::iterator iter;
	for( iter=m_ActionList.begin() ; iter!=m_ActionList.end() ; ++iter )
	{
		if( (*iter)->GetState() == ioAnimation::AS_ACT_GUARANTEE )
		{
			(*iter)->SetGuaranteeOut();
		}
	}
}

void ioAniController::SetAniEventHandler( ioAniEventHandler *pHandler )
{
	SAFEDELETE( m_pEventHandler );

	if( pHandler )
	{
		m_pEventHandler = pHandler;
		m_pEventHandler->SetOwner( m_pParent );
	}
}

void ioAniController::NotifyAniEvent( const AniEventTime *pEvent,
									  const ioAnimation *pNotifier )
{
	if( m_pEventHandler )
	{
		m_pEventHandler->EventHandling( pEvent, pNotifier );
	}
}

void ioAniController::ChangeAniEvent( const AniEventTime *pEvent, ioAnimation *pNotifier )
{
	if( m_pEventHandler )
	{
		m_pEventHandler->ChangeAniEventHandling( pEvent, pNotifier );
	}
}

void ioAniController::HideMeshExceptionCheck( const ioHashString &szMeshName )
{
	if( m_pEventHandler )
	{
		m_pEventHandler->HideMeshExceptionCheck( szMeshName );
	}
}

void ioAniController::NotifyEndEvent( const ioAnimation *pNotifier )
{
	if( pNotifier->IsNeedEndEventNotify() )
	{
		AniEventTime kEvent( 0.0f, "end_animation", "" );
		NotifyAniEvent( &kEvent, pNotifier );
	}
}

void ioAniController::NotifyClearEvent( const ioAnimation *pNotifier )
{
	AniEventTime kEvent( 0.0f, "clear_animation", "" );
	NotifyAniEvent( &kEvent, pNotifier );
}

ioAnimation* ioAniController::GetPlayingActionByOrder( int iOrder )
{
	if( COMPARE( iOrder, 0, GetPlayingActionCnt() ) )
	{
		ioAniList::iterator iter = m_ActionList.begin();
		std::advance( iter, iOrder );
		return *iter;
	}

	return NULL;
}

ioAnimation* ioAniController::GetPlayingLoopByOrder( int iOrder )
{
	if( COMPARE( iOrder, 0, GetPlayingLoopCnt() ) )
	{
		ioAniList::iterator iter = m_LoopList.begin();
		std::advance( iter, iOrder );
		return *iter;
	}

	return NULL;
}

