#include "stdafx.h"

#include "ioModeItemManager.h"

#include "ioModeItem.h"
#include "ioShuffleStarModeItem.h"
#include "ioShuffleBuffItem.h"
#include "ioHunterCoinModeItem.h"

template<> ioModeItemManager* Singleton< ioModeItemManager >::ms_Singleton = 0;
ioModeItemManager::ioModeItemManager()
{
	m_pPlayStage = NULL;
	m_ModeItemMap.clear();
}

ioModeItemManager::~ioModeItemManager()
{
	ClearModeItemList();
}

ioModeItemManager& ioModeItemManager::GetSingeton()
{
	return Singleton< ioModeItemManager >::GetSingleton();
}

void ioModeItemManager::ClearModeItemList()
{
	ModeItemMap_iter iter;
	for( iter=m_ModeItemMap.begin() ; iter!=m_ModeItemMap.end() ; ++iter )
	{
		SAFEDELETE(iter->second);
	}
	m_ModeItemMap.clear();
}

void ioModeItemManager::LoadItemList()
{
	ioINILoader kLoader( "config/sp2_mode_item.ini" );
	
	char szTitle[MAX_PATH] = "";

	kLoader.SetTitle( "common" );
	int iCnt = kLoader.LoadInt( "mode_item_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf_e( szTitle, "mode_item%d", i+1 );
		kLoader.SetTitle( szTitle );

		ioModeItem *pItem = CreateModeItemTemplete( kLoader );
		if( pItem )
		{
			pItem->SetPlayStage( m_pPlayStage );
			pItem->LoadProperty( kLoader );
			
			if( m_ModeItemMap.find( pItem->GetTypeID() ) == m_ModeItemMap.end() )
			{
				m_ModeItemMap.insert( ModeItemMap::value_type( pItem->GetTypeID(), pItem ) );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioModeItemManager::LoadItemList - %s Already Exist", pItem->GetTypeID() );
				SAFEDELETE( pItem );
			}
		}
	}
}

ioModeItem *ioModeItemManager::CreateModeItemTemplete( ioINILoader &rkLoader )
{
	int iTypeID = rkLoader.LoadInt_e( "item_type", 0 );

	switch( iTypeID )
	{
	case MIT_SHUFFLE_STAR:
		return new ioShuffleStarModeItem;
		break;
	case MIT_BUFF:
		return new ioShuffleBuffItem;
		break;
	case MIT_HUNTER_COIN:
		return new ioHunterCoinModeItem;
		break;
	default:
		LOG.PrintTimeAndLog( 0, "ioModeItemManager::CreateModeItem - %d UnknownType", iTypeID );
	}

	return NULL;
}

ioModeItem *ioModeItemManager::CreateModeItem( int iType )
{
	ModeItemMap_iter iter = m_ModeItemMap.find( iType );
	if( iter != m_ModeItemMap.end() )
	{
		return iter->second->Clone();
	}

	LOG.PrintTimeAndLog( 0, "ioModeItemManager::CreateModeItem - %d Not Exist Skill", iType );
	return NULL;
}
