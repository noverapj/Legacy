#pragma once


#include "ioEtcItem.h"

class ioEtcItemManager : public Singleton< ioEtcItemManager >
{
public:
	enum
	{
		TYPE_GROUP_GAP      = 100000,
		CLASS_ETC_ITEM_TYPE = 1, 
		USE_TYPE_POS        = 1000000, 
		FIRST_TYPE_POS      = 100000, 
		SECOND_TYPE_POS     = 100,
	};
protected:
	typedef std::map< DWORD, ioEtcItem* > EtcItemMap;
	EtcItemMap m_EtcItemMap;

public:
	void LoadEtcItem();

protected:
	ioEtcItem* CreateEtcItem( DWORD dwType );
	bool  AddEtcItem( ioEtcItem *pInfo );
	void  ClearEtcItemList();
	DWORD GetEtcItemTypeExceptClass( DWORD dwType );

public:
	ioEtcItem* FindEtcItem( DWORD dwType );
	ioEtcItem* GetEtcItemByIdx( int iIdx ) const;
	ioEtcItem* FindEtcItemMacro( const char *szMacro );
	void FindAllEtcItemMacro( const char *szMacro, IntVec& TypeList );

	void GetEtcItemList( int iGroupIdx, EtcItemList &rkList );
	void Init( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );

public:
	inline int GetEtcItemCount() const { return m_EtcItemMap.size(); }

public:
	bool IsBlockEtcItem( DWORD dwType );
	bool IsFishingGroundEtcItem( DWORD dwType );
	bool IsFishingRodEtcItem( DWORD dwType );
	bool IsFishingBaitEtcItem( DWORD dwType );
	ioEtcItem* FindEtcItemBlockMacro( const char *szMacro );

	int GetEtcItemRecommandedCnt();

	bool IsFishingGearEtcItem( DWORD dwType );

public:
	static ioEtcItemManager& GetSingeton();

public:
	ioEtcItemManager();
	virtual ~ioEtcItemManager();
};

#define g_EtcItemMgr ioEtcItemManager::GetSingleton()

