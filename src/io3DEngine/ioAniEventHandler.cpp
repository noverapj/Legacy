

#include "stdafx.h"

#include "ioAniEventHandler.h"
#include "ioEntityGroup.h"
#include "ioStringConverter.h"

#include "ioEntity.h"
#include "ioMeshTrailer.h"
#include "ioAnimation.h"
#include "ioSkeleton.h"

#include "ioFrameTimer.h"

#include "ioSoundManager.h"

ioAniEventHandler::ioAniEventHandler()
{
	m_pOwner = NULL;
}

ioAniEventHandler::~ioAniEventHandler()
{
	m_HideMeshList.clear();
}

void ioAniEventHandler::SetOwner( ioEntityGroup *pOwner )
{
	m_pOwner = pOwner;
}

bool ioAniEventHandler::EventHandling( const AniEventTime *pEvent,
									   const ioAnimation *pNotifier )
{
	if( !m_pOwner )	return false;

	if( pEvent->szEventType == "auto_sound" )
	{
		OnAutoSound( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "trail_s" )
	{
		OnTrailStart( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "trail_e" )
	{
		OnTrailEnd( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "end_animation" )
	{
		OnAnimationEnd( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "clear_animation" )
	{
		OnAnimationClear( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "show_mesh" )
	{
		OnShowMesh( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "hide_mesh" )
	{
		OnHideMesh( pEvent, pNotifier );
		return true;
	}

	return false;
}

bool ioAniEventHandler::ChangeAniEventHandling( const AniEventTime *pEvent, ioAnimation *pChangeAni )
{
	if( !m_pOwner )	return false;

	if( pEvent->szEventType == "jump_anitime" )
	{
		pChangeAni->SetCurTime( atof( pEvent->szSubInfo.c_str() ) );
		pChangeAni->Update( 0.0f );
		return true;
	}

	return false;
}

void ioAniEventHandler::OnAutoSound( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	g_SoundMgr.PlaySound( pEvent->szSubInfo, m_pOwner->GetParentSceneNode() );
}

void ioAniEventHandler::OnTrailStart( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );

	ioHashString szTexture;
	if( vParam.size() >= 1 )
		szTexture = vParam[0].c_str();

	FXRenderType eRenderType = FRT_SRCALPHA;
	if( vParam.size() >= 2 )
		eRenderType = ioStringConverter::ParseFXRenderType( vParam[1].c_str() );

	int iMaxSegment = 100;
	if( vParam.size() >= 3 )
		iMaxSegment = ioStringConverter::ParseInt( vParam[2].c_str() );

	float fMaxTick = 100.0f;
	if( vParam.size() >= 4 )
		fMaxTick = ioStringConverter::ParseFloat( vParam[3].c_str() );

	// 5번째는 부위설정..

	float fSampleTick = 200.0f;
	if( vParam.size() >= 6 )
		fSampleTick = ioStringConverter::ParseFloat( vParam[5].c_str() );

	bool bDepthCheck = true;
	if( vParam.size() >= 7 )
		bDepthCheck = ioStringConverter::ParseBool( vParam[6].c_str() );

	int iCtrlStart = 0;
	int iCtrlEnd = INT_MAX;
	if( vParam.size() >= 8 )
	{
		// vParam은 ::Split 내부의 static변수의 참조인데, Split내부에서 시작시 지워버리기때문에 삭제되버려서 임시로 복사
		std::string szCtrlParam = vParam[7];

		const StringVector &vCtrl = ioStringConverter::Split( szCtrlParam, "-", 1 );
		if( vCtrl.size() == 2 )
		{
			iCtrlStart = ioStringConverter::ParseInt( vCtrl[0].c_str() );
			iCtrlEnd   = ioStringConverter::ParseInt( vCtrl[1].c_str() ) + 1;
		}
	}
	
	float fGlowness = 2.0f;
	if( vParam.size() >= 9 )
		fGlowness = ioStringConverter::ParseFloat( vParam[8].c_str() );

	float fIntensity = 5.0f;
	if( vParam.size() >= 10 )
		fIntensity = ioStringConverter::ParseFloat( vParam[9].c_str() );

	int iEntityCnt = m_pOwner->GetNumEntities();
	for( int i=0 ; i<iEntityCnt ; i++ )
	{
		ioEntity *pEntity = m_pOwner->GetEntity( i );
		if( !pEntity || pEntity->GetControlPointCnt() == 0 )
			continue;

		ConstCtrlPointList rkStart = pEntity->GetControlPointList( "trailstart" );
		if( rkStart.empty() )	continue;

		ConstCtrlPointList rkEnd = pEntity->GetControlPointList( "trailend" );
		if( rkEnd.empty() || rkStart.size() != rkEnd.size() )
		{
			LOG.PrintTimeAndLog( 0, "OnTrailStart - %s trailstart - trailend not match",
									pEntity->GetMeshName().c_str() );
			continue;
		}

		ConstCtrlPointList::iterator iStart, iEnd;
		iStart = rkStart.begin(), iEnd = rkEnd.begin();
		for( int iIdx = 0 ; iStart != rkStart.end() ; ++iStart, ++iEnd, iIdx++ )
		{
			if( !COMPARE( iIdx, iCtrlStart, iCtrlEnd ) )
				continue;

			ioMeshTrailer *pTrailer = m_pOwner->CreateTrailer();
			if( !pTrailer ) continue;

			pTrailer->SetTexture( szTexture );
			pTrailer->SetRenderType( eRenderType );
			pTrailer->SetIntensity( fIntensity );
			pTrailer->SetGlowness( fGlowness );
			pTrailer->BuildMaterial();

			pTrailer->SetDepthCheck( bDepthCheck );

			pTrailer->SetMaxTickTime( fMaxTick );
			pTrailer->SetMaxSegment( iMaxSegment );
			pTrailer->SetStartEndPoint( *iStart, *iEnd );

			bool bDataOk = pTrailer->GenerateTrailData( m_pOwner->GetSkeleton()->GetName(),
														pEntity->GetMeshName(),
														pNotifier,
														iIdx,
														FLOAT1000 / fSampleTick,
														pEvent->fEventTime );

			if( !bDataOk )
			{
				m_pOwner->DestroyTrailer( pTrailer );
				pTrailer = NULL;	// 주의!!
			}
		}
	}

	pNotifier->SetEndEventNotify();
}

void ioAniEventHandler::OnTrailEnd( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	m_pOwner->SetGraceCloseAllTrailer();
}

void ioAniEventHandler::OnAnimationEnd( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	m_pOwner->SetGraceCloseAllTrailer();

	/*
	if( !m_HideMeshList.empty() )
	{
		ioHashStringVec::iterator iter;
		for( iter=m_HideMeshList.begin() ; iter!=m_HideMeshList.end() ; ++iter )
		{
			m_pOwner->SetEntityVisible( *iter, true );
		}
		m_HideMeshList.clear();
	}
	*/

	OnAnimationClear( pEvent, pNotifier );
}

void ioAniEventHandler::OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_HideMeshList.empty() )
	{
		AniEventInfoList::iterator iter = m_HideMeshList.begin();

		while( iter != m_HideMeshList.end() )
		{
			AniEventInfo kInfo = *iter;

			if( kInfo.m_AniName == pNotifier->GetName() )
			{
				m_pOwner->SetEntityVisible( kInfo.m_EventName, true );
				iter = m_HideMeshList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}
}

void ioAniEventHandler::OnShowMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	m_pOwner->SetEntityVisible( pEvent->szSubInfo, true );

	if( !m_HideMeshList.empty() )
	{
		AniEventInfoList::iterator iter;

		for( iter=m_HideMeshList.begin(); iter != m_HideMeshList.end(); ++iter )
		{
			AniEventInfo kInfo = *iter;

			if( pEvent->szSubInfo == kInfo.m_EventName )
				break;
		}

		if( iter != m_HideMeshList.end() )
		{
			m_HideMeshList.erase( iter );
		}
	}
}

void ioAniEventHandler::OnHideMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( pEvent->szSubInfo.IsEmpty() )
		return;

	const StringVector &vParam = ioStringConverter::Split( pEvent->szSubInfo.c_str(), " " );

	bool bShowMust = true;
	if( vParam.size() == 2 )
	{
		bShowMust = ioStringConverter::ParseBool( vParam[1].c_str() );
	}

	ioHashString szMeshName = vParam[0].c_str();
	m_pOwner->SetEntityVisible( szMeshName, false );

	if( bShowMust )
	{
		bool bFind = false;
		AniEventInfoList::iterator iter = m_HideMeshList.begin();
		while( iter != m_HideMeshList.end() )
		{
			if( (*iter).m_EventName == szMeshName )
			{
				bFind = true;
				break;
			}
			else
			{
				++iter;
			}
		}

		if( bFind )
		{
			(*iter).m_AniName = pNotifier->GetName();
			return;
		}

		AniEventInfo kInfo;
		kInfo.m_AniName = pNotifier->GetName();
		kInfo.m_EventName = szMeshName;

		m_HideMeshList.push_back( kInfo );
		pNotifier->SetEndEventNotify();
	}
}

// hide_mesh 테그로 숨겨진 메쉬가 에니메이션 외적으로 다시 숨겨질 경우
// 리스트에서 제거하여 에니메이션에서 컨트롤하지 않게한다.
void ioAniEventHandler::HideMeshExceptionCheck( const ioHashString &szMeshName )
{
	if( m_HideMeshList.empty() )
		return;

	AniEventInfoList::iterator iter = m_HideMeshList.begin();
	while( iter != m_HideMeshList.end() )
	{
		if( (*iter).m_EventName == szMeshName )
			iter = m_HideMeshList.erase( iter );
		else
			++iter;
	}
}

void ioAniEventHandler::ClearAniEvent()
{

}

void ioAniEventHandler::ClearAniEffect()
{

}