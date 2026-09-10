#pragma once
#include "NewShopDataDefine.h"

class NewShopCache
{
public:
	DWORD			dwTab;
	DWORD			dwSamllTab;
	int				eNewShopFilter;

	NewShopCache()
	{
		dwTab			= 0;
		dwSamllTab		= 0;
		eNewShopFilter	= 0;
	}
	virtual ~NewShopCache(){}
};
typedef std::vector<NewShopCache> NewShopCacheList;

struct  NewShopDataCache : public NewShopCache
{
public:
	vNewShopItemInfoVec	m_ShopDataList;
	int					m_SexType;

	NewShopDataCache()
	{
		m_SexType = 0;
	}
};
typedef std::vector<NewShopDataCache> NewShopDataCacheList;

class ioShopDataCache : public Singleton< ioShopDataCache >
{
protected:
	NewShopDataCacheList m_NewShopDataCacheList;

public:
	static ioShopDataCache& GetSingleton();

public:
	void AddCash( NewShopDataCache& Cache );
	void RemoveCache( DWORD dwTabID, DWORD dwSamllTabID, int iSexType );

	NewShopDataCache* GetCash( IN DWORD dwTab, IN DWORD dwSubTab, int iSexType = 0 );

public:
	void ClearFilterCache();

public:
	ioShopDataCache();
	~ioShopDataCache();
};

#define g_ShopData ioShopDataCache::GetSingleton()