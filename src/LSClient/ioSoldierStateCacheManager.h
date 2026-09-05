#pragma once
#include "../io3DEngine/ioHashString.h"

// 용병 캐쉬(상태 혹은 용병 교체 후에도 남아있어야 할 데이터 등) 데이터
class SoldierCache
{
public:
	enum CacheType
	{
		CT_NONE,		
		CT_RESSEARCHER,
	};

protected:
	ioHashString m_szOwner;

public:
	virtual ioHashString GetOwner(){ return m_szOwner; }
	virtual SoldierCache::CacheType GetType(){ return CT_NONE; }

public:
	SoldierCache()
	{
	};

	SoldierCache( ioHashString szOwner ) : m_szOwner (szOwner)
	{
	};

	virtual ~SoldierCache()
	{
	};
};

typedef std::vector<SoldierCache*> SoldierCacheList;

class ioSoldierCacheManager : public Singleton< ioSoldierCacheManager >
{
protected:
	SoldierCacheList m_SoldierCacheList;

public:
	void Initialize(); 
	void Finalize();

public:
	static ioSoldierCacheManager& GetSingleton();

	SoldierCache* GetCache( const ioHashString& szOwner, SoldierCache::CacheType eType );
	SoldierCache* NewCache( const ioHashString& szOwner, SoldierCache::CacheType eType );

public:
	ioSoldierCacheManager();
	virtual ~ioSoldierCacheManager();
};

#define g_SoldierCacheManager ioSoldierCacheManager::GetSingleton()

