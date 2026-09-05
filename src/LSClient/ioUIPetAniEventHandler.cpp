

#include "stdafx.h"

#include <strsafe.h>

#include "GUI/ioPetView.h"
#include "ioUIPetAniEventHandler.h"

ioUIPetAniEventHandler::ioUIPetAniEventHandler()
{
	m_pChar = NULL;
}

ioUIPetAniEventHandler::~ioUIPetAniEventHandler()
{
	m_ShopEffectList.clear();
	m_ShopSoundList.clear();
}

void ioUIPetAniEventHandler::SetCreatePet( ioPetView *pChar )
{
	m_pChar = pChar;
}

bool ioUIPetAniEventHandler::EventHandling( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( ioAniEventHandler::EventHandling( pEvent, pNotifier ) )
		return true;

	if( pEvent->szEventType == "auto_effect" )
	{
		OnAutoEffect( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "shop_effect" )
	{
		OnShopEffect( pEvent, pNotifier );
		return true;
	}
	else if( pEvent->szEventType == "shop_sound" )
	{
		OnShopSound( pEvent, pNotifier );
		return true;
	}
	return false;
}

void ioUIPetAniEventHandler::OnAutoEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	m_pChar->AttachEffect( pEvent->szSubInfo, NULL );
}

void ioUIPetAniEventHandler::OnShopEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	if( !m_pChar ) return;

	AniEventInfoList::iterator iter = m_ShopEffectList.begin();
	while( iter != m_ShopEffectList.end() )
	{
		if( (*iter).m_EventName == pEvent->szSubInfo )
		{
			m_pChar->EndEffect( pEvent->szSubInfo, false );
			m_ShopEffectList.erase( iter );
			break;
		}
		else
		{
			++iter;
		}
	}

	m_pChar->AttachEffect( pEvent->szSubInfo, NULL );

	AniEventInfo kInfo;
	kInfo.m_AniName = pNotifier->GetName();
	kInfo.m_EventName = pEvent->szSubInfo;

	m_ShopEffectList.push_back( kInfo );
}

void ioUIPetAniEventHandler::OnShopSound( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	AniSoundEventInfoList::iterator iter = m_ShopSoundList.begin();
	while( iter != m_ShopSoundList.end() )
	{
		if( (*iter).m_EventName == pEvent->szSubInfo )
		{
			g_SoundMgr.StopSound( (*iter).m_EventName, (*iter).m_SoundID );
			//g_SoundMgr.StopSound( (*iter).m_EventName, 0 );
			iter = m_ShopSoundList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	UniqueObjID eSoundID = g_SoundMgr.PlaySound( pEvent->szSubInfo, m_pOwner->GetParentSceneNode() );

	AniSoundEventInfo kInfo;
	kInfo.m_AniName = pNotifier->GetName();
	kInfo.m_EventName = pEvent->szSubInfo;
	kInfo.m_SoundID = eSoundID;

	m_ShopSoundList.push_back( kInfo );
}

void ioUIPetAniEventHandler::OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	// Hide 메쉬 복귀
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

	// Hide 메쉬에 따른 이펙트 복귀
	if( !m_HideMeshLinkEffectList.empty() )
	{
		AniEventInfoList::iterator iter = m_HideMeshLinkEffectList.begin();
		while( iter != m_HideMeshLinkEffectList.end() )
		{
			AniEventInfo kInfo = *iter;

			if( kInfo.m_AniName == pNotifier->GetName() )
			{
				m_pChar->AttachEffect( kInfo.m_EventName, NULL );
				iter = m_HideMeshLinkEffectList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	// ShopEffect 제거
	if( !m_ShopEffectList.empty() )
	{
		AniEventInfoList::iterator iter = m_ShopEffectList.begin();
		while( iter != m_ShopEffectList.end() )
		{
			AniEventInfo kInfo = *iter;

			if( kInfo.m_AniName == pNotifier->GetName() )
			{
				m_pOwner->EndEffect( kInfo.m_EventName, false );
				iter = m_ShopEffectList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	// ShopSound 제거
	if( !m_ShopSoundList.empty() )
	{
		AniSoundEventInfoList::iterator iter = m_ShopSoundList.begin();
		while( iter != m_ShopSoundList.end() )
		{
			AniSoundEventInfo kInfo = *iter;

			if( kInfo.m_AniName == pNotifier->GetName() )
			{
				g_SoundMgr.StopSound( kInfo.m_EventName, 0 );
				iter = m_ShopSoundList.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}
}