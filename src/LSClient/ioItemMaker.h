#pragma once

#include "ioItem.h"

class ioINILoader;
class ioExtendDash;
class ioExtendJump;
class ioCounterAttack;
class ioPlayStage;

class ioItemMaker : public Singleton< ioItemMaker >
{
protected:
	ioPlayStage* m_pPlayStage;

	typedef std::map< ioHashString, ioItem* > ItemNameMap;
	typedef std::map< DWORD, ioItem* > ItemCodeMap;
	typedef std::map< DWORD, int > ItemCreateMap;

	ItemNameMap m_ItemNameMap;	// 검색용
	ItemCodeMap m_ItemCodeMap;	// 검색용
	ItemVector m_ItemList;	// 메모리 관리용

	ItemCreateMap m_ItemCreateMap;	// 생성된 녀석들 검사용

	DWORD m_dwNextCreateIndex;

	// 아이템 모두 드랍시 사용
	float m_fItemDropRange;

	// 테스트용
	CEncrypt<bool> m_bUseBaseEquipMesh;

public:
	void LoadItemInfo();
	
	void SetPlayStage( ioPlayStage *pStage );
	ioPlayStage* GetPlayStage();

protected:
	void LoadBaseItemList( const char *szFileName );
	void LoadItemList( const char *szFileName );

	void ClearAllItemInfo();

public:
	ioItem* CreateItem( DWORD dwCode );
	ioItem* CreateItem( const ioHashString &rkName );

	ioExtendDash* CreateExtendDash( ioINILoader &rkLoader );
	ioExtendJump* CreateExtendJump( ioINILoader &rkLoader );
	ioCounterAttack* CreateCounterAttack( ioINILoader &rkLoader );

public:
	void SetNextCreateIndex( DWORD dwIndex );
	DWORD GetNextCreateIndex();

protected:
	ioItem* CreateItemTemplete( ioItem::ItemType eType, ioINILoader &rkLoader );
	ioItem* CreateWeaponItemTemplete( ioINILoader &rkLoader );
	ioItem* CreateWearItemTemplete( ioINILoader &rkLoader );
	ioItem* CreateObjectItemTemplete( ioINILoader &rkLoader );

	void AddNewItem( ioItem *pItem );

	ioItem* CreateCloneItem( ioItem *pSrc );

public:
	void NotifyItemDestroyed( DWORD dwItemCode );

protected:
	void IncreaseItemCreateCnt( DWORD dwItemCode );
	int GetCreatedCnt( DWORD dwItemCode ) const;

public:
	int GetNumItemTemplete() const;

	const ioItem* GetItemConst( DWORD dwCode ) const;
	const ioItem* GetItemConst( DWORD dwCode, const ioHashString& szFuncName ) const;
	const ioItem* GetItemConst( const ioHashString &rkName, const ioHashString& szFuncName ) const;
	
public:
	DWORDVec GetSetItemList( DWORD dwSetCode );
	const ioHashString& GetItemLargeIcon( DWORD dwItemCode, DWORD dwType ) const;

public:	// for test..
	const ioItem* GetWeaponRandom( bool bIncludeNoItem ) const;
	const ioItem* GetArmorRandom( bool bIncludeNoItem ) const;
	const ioItem* GetCloakRandom( bool bIncludeNoItem ) const;
	const ioItem* GetHelmetRandom( bool bIncludeNoItem ) const;

protected:
	int PickRandomItemNum( int iItemCnt, bool bIncludeNoItem ) const;
	bool CheckCanRandomSupply( ioItem *pItem, bool bIncludeLimitItem ) const;

public:
	bool IsItem( DWORD dwCode );

public:
	inline float GetItemDropRange() const { return m_fItemDropRange; }
	inline bool IsBaseEquipMesh() const { return m_bUseBaseEquipMesh; }

public:
	static ioItemMaker& GetSingleton();

public:
	ioItemMaker();
	virtual ~ioItemMaker();
};

#define g_ItemMaker ioItemMaker::GetSingleton()
