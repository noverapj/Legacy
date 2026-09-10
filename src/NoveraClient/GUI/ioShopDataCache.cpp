#include "stdafx.h"

#include "ioShopDataCache.h"

template<> ioShopDataCache* Singleton< ioShopDataCache >::ms_Singleton = 0;
ioShopDataCache& ioShopDataCache::GetSingleton()
{
	return Singleton< ioShopDataCache >::GetSingleton();
}

ioShopDataCache::ioShopDataCache()
{

}

ioShopDataCache::~ioShopDataCache()
{

}

void ioShopDataCache::AddCash( NewShopDataCache& Cache )
{
	RemoveCache( Cache.dwTab, Cache.dwSamllTab, Cache.m_SexType );
	m_NewShopDataCacheList.push_back( Cache );
}

void ioShopDataCache::RemoveCache( DWORD dwTabID, DWORD dwSamllTabID, int iSexType )
{
	for( NewShopDataCacheList::iterator iter = m_NewShopDataCacheList.begin(); iter != m_NewShopDataCacheList.end(); )
	{
		NewShopDataCache& Cache = *iter;
		if( Cache.dwTab == dwTabID && Cache.dwSamllTab == dwSamllTabID && Cache.m_SexType == iSexType )
		{
			iter = m_NewShopDataCacheList.erase( iter );
		}
		else
		{
			 ++iter;
		}
	}
}

NewShopDataCache* ioShopDataCache::GetCash( IN DWORD dwTab, IN DWORD dwSubTab, int iSexType /* = 0 */ )
{
	for( NewShopDataCacheList::iterator iter = m_NewShopDataCacheList.begin(); iter != m_NewShopDataCacheList.end(); ++iter )
	{
		NewShopDataCache& Cache = *iter;
		if( Cache.dwTab == dwTab && Cache.dwSamllTab == dwSubTab && Cache.m_SexType == iSexType )
		{
			return &Cache;
		}
	}

	return NULL;
}

void ioShopDataCache::ClearFilterCache()
{
	for( NewShopDataCacheList::iterator iter = m_NewShopDataCacheList.begin(); iter != m_NewShopDataCacheList.end(); ++iter )
	{
		NewShopDataCache& Cache = *iter;
		Cache.eNewShopFilter = NSF_NONE;
	}
}