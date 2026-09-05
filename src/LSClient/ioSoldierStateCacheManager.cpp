#include "stdafx.h"

#include "ioSoldierStateCacheManager.h"
#include "ItemDefine.h"

template<> ioSoldierCacheManager* Singleton< ioSoldierCacheManager >::ms_Singleton = 0;

ioSoldierCacheManager::ioSoldierCacheManager()
{
}

ioSoldierCacheManager::~ioSoldierCacheManager()
{
	Finalize();
}

ioSoldierCacheManager& ioSoldierCacheManager::GetSingleton()
{
	return Singleton< ioSoldierCacheManager >::GetSingleton();
}

void ioSoldierCacheManager::Initialize()
{
	Finalize();
}

void ioSoldierCacheManager::Finalize()
{
	m_SoldierCacheList.clear();

	for( SoldierCacheList::iterator iter = m_SoldierCacheList.begin(); iter != m_SoldierCacheList.end(); iter++ )
	{
		SoldierCache* pCache = *iter;
		if( pCache )
		{
			SAFEDELETE( pCache );
		}
	}
}

SoldierCache* ioSoldierCacheManager::GetCache( const ioHashString& szOwner, SoldierCache::CacheType eType )
{
	for( SoldierCacheList::iterator iter = m_SoldierCacheList.begin(); iter != m_SoldierCacheList.end(); iter++ )
	{
		SoldierCache* pCache = *iter;
		if( pCache && pCache->GetType() == eType && pCache->GetOwner() == szOwner )
		{
			return pCache;
		}
	}

	return NULL;
}

SoldierCache* ioSoldierCacheManager::NewCache( const ioHashString& szOwner, SoldierCache::CacheType eType )
{
	SoldierCache* pCache = GetCache( szOwner, eType );
	if( pCache )
		return pCache;
	
	switch( eType )
	{
	case SoldierCache::CT_RESSEARCHER:
		pCache = new ioResearcherCache( szOwner );
	}

	m_SoldierCacheList.push_back( pCache );
	return pCache;
}