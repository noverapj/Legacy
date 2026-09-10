
#include "stdafx.h"

#include "ioFieldRewardItemMgr.h"

template<> ioFieldRewardItemMgr* Singleton< ioFieldRewardItemMgr >::ms_Singleton = 0;
ioFieldRewardItemMgr::ioFieldRewardItemMgr()
{
}

ioFieldRewardItemMgr::~ioFieldRewardItemMgr()
{
	RewardDataMap::iterator iCreate = m_RewardDataMap.begin();
	for(; iCreate != m_RewardDataMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_RewardDataMap.clear();
}

ioFieldRewardItemMgr& ioFieldRewardItemMgr::GetSingleton()
{
	return Singleton< ioFieldRewardItemMgr >::GetSingleton();
}

void ioFieldRewardItemMgr::LoadINIData()
{
	{
		ioINILoader_e kLoader( "config/sp2_field_reward_item.ini" );
		kLoader.SetTitle_e( "common" );

		int iMaxItem = kLoader.LoadInt_e( "max_item", 0 );
		for(int i = 0;i < iMaxItem;i++)
		{
			RewardData *pRewardData = new RewardData;

			char szTitle[MAX_PATH] = "";
			sprintf_e( szTitle, "item%d", i + 1 );
			kLoader.SetTitle( szTitle );

			char szBuf[MAX_PATH] = "";
			kLoader.LoadString_e( "mesh_name", "", szBuf, MAX_PATH );
			pRewardData->m_MeshName = szBuf;

			kLoader.LoadString_e( "icon_name", "", szBuf, MAX_PATH );
			pRewardData->m_IconName = szBuf;

			m_RewardDataMap.insert( RewardDataMap::value_type( i + 1, pRewardData ) );
		}
	}
}

const ioHashString &ioFieldRewardItemMgr::GetMeshName( DWORD dwIndex )
{
	RewardDataMap::iterator iter = m_RewardDataMap.find( dwIndex );
	if( iter != m_RewardDataMap.end() )
		return iter->second->m_MeshName;

	static ioHashString kNoneReturn;
	return kNoneReturn;
}

const ioHashString &ioFieldRewardItemMgr::GetIconName( DWORD dwIndex )
{
	RewardDataMap::iterator iter = m_RewardDataMap.find( dwIndex );
	if( iter != m_RewardDataMap.end() )
		return iter->second->m_IconName;

	static ioHashString kNoneReturn;
	return kNoneReturn;
}