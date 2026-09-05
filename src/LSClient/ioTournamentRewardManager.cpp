
#include "stdafx.h"

#include "ioTournamentRewardManager.h"

template<> TournamentRewardManager* Singleton< TournamentRewardManager >::ms_Singleton = 0;

TournamentRewardManager::TournamentRewardManager()
{
}

TournamentRewardManager::~TournamentRewardManager()
{
	DestroyRewardItem();
}

void TournamentRewardManager::Initialize()
{
	DestroyRewardItem();

	ioINILoader_e kLoader( "config/sp2_custom_tournament_reward.ini" );

	char szBuf[MAX_PATH];
	int iMaxCount = kLoader.LoadInt( "common", "max", 0 );
	for( int i = 1 ; i <= iMaxCount ; ++i )
	{
		TournamentRewardItem* pItem = new TournamentRewardItem;
		wsprintf_e( szBuf, "rewarditem%d", i );
		kLoader.SetTitle( szBuf );
		
		DWORD dwType = kLoader.LoadInt_e( "etc_item_type", 0 );

		kLoader.LoadString_e( "title", "", szBuf, MAX_PATH );
		pItem->m_szRewardItemName = szBuf;
				
		int iPoint = kLoader.LoadInt_e( "reward_item_pirce", 0 );
		pItem->m_dwRewardItemPirce = iPoint;

		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( dwType );
		if( pEtcItem )
		{
			pItem->m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
			pItem->m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );
		}

		m_TournamentRewardItemMap.insert( TournamentRewardItemMap::value_type( dwType, pItem ) );
	}
}

const TournamentRewardItem* TournamentRewardManager::FindRewardItem( DWORD dwType ) const 
{
	TournamentRewardItemMap::const_iterator iter = m_TournamentRewardItemMap.find( dwType );
	if( iter != m_TournamentRewardItemMap.end() )
	{
		return iter->second;
	}

	return NULL;
}

void TournamentRewardManager::DestroyRewardItem()
{
	TournamentRewardItemMap::const_iterator iter = m_TournamentRewardItemMap.begin();
	for( ; iter != m_TournamentRewardItemMap.end(); ++iter )
	{
		TournamentRewardItem* pItem = iter->second;
		if( pItem )		
		{
			SAFEDELETE( pItem->m_pIcon );
			SAFEDELETE( pItem->m_pSubIcon );
		}
		SAFEDELETE( pItem );		
	}
	m_TournamentRewardItemMap.clear();
}

TournamentRewardManager& TournamentRewardManager::GetSingleton()
{
	return Singleton< TournamentRewardManager >::GetSingleton();
}
