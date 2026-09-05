
#include "stdafx.h"

#include "ioDummyCharAniEventHandler.h"

ioDummyCharAniEventHandler::ioDummyCharAniEventHandler()
{
	m_pChar = NULL;
}

ioDummyCharAniEventHandler::~ioDummyCharAniEventHandler()
{
	m_ShopEffectList.clear();
}

void ioDummyCharAniEventHandler::SetCreateChar( ioDummyChar *pChar )
{
	m_pChar = pChar;
}

bool ioDummyCharAniEventHandler::EventHandling( const AniEventTime *pEvent, const ioAnimation *pNotifier )
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

	return false;
}

void ioDummyCharAniEventHandler::OnAutoEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	m_pChar->AttachEffect( pEvent->szSubInfo, NULL );
}

void ioDummyCharAniEventHandler::OnShopEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier )
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

void ioDummyCharAniEventHandler::OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier )
{
	// Hide ¸Þ½¬ º¹±Í
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

	// Hide ¸Þ½¬¿¡ µû¸¥ ÀÌÆåÆ® º¹±Í
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

	// ShopEffect Á¦°Å
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
}
