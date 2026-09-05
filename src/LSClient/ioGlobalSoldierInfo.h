#pragma once

#include "CharacterInfo.h"

class SP2Packet;
class ioGlobalSoldierInfo : public Singleton< ioGlobalSoldierInfo >
{
protected:
	// 용병 정보
	typedef std::map< DWORD, CHARACTER > SoldierDecMap;
	SoldierDecMap m_vSoldierMap;
	CHARACTER m_DefCharInfo;

	// 용병 아이템 정보
	struct ItemCode 
	{
		DWORD m_dwItemCode[MAX_INVENTORY];
		DWORD m_dwItemMaleCustom[MAX_INVENTORY];
		DWORD m_dwItemFemaleCustom[MAX_INVENTORY];
		ItemCode()
		{
			memset( m_dwItemCode, 0, sizeof( m_dwItemCode ) );
			memset( m_dwItemMaleCustom, 0, sizeof( m_dwItemMaleCustom ) );
			memset( m_dwItemFemaleCustom, 0, sizeof( m_dwItemFemaleCustom ) );
		}
	};
	typedef std::map< DWORD, ItemCode > SoldierItemMap;
	SoldierItemMap m_vSoldierItemMap;

	// 시상식에 랜덤으로 표시할 용병 수집
	SoldierDecMap m_vAwardSoldierMap;

protected:
	bool IsExceptionSyncItem( DWORD dwItemCode );

public:
	CHARACTER GetSoldierData( const ioHashString &rkName, DWORD dwCharIndex, int iClassType, int iKindred, int iSex );
	CHARACTER GetSoldierData( const ioHashString &rkName, DWORD dwCharIndex );
	DWORD     GetSoldierItemCode( const ioHashString &rkName, DWORD dwCharIndex, int iSlot );
	DWORD     GetSoldierItemMaleCustom( const ioHashString &rkName, DWORD dwCharIndex, int iSlot );
	DWORD     GetSoldierItemFemaleCustom( const ioHashString &rkName, DWORD dwCharIndex, int iSlot );
	CHARACTER GetAwardRandSoldier( int iSeed );

public:
	void SyncSoldierData( DWORD dwCharIndex, CHARACTER kCharInfo );
	void SyncSoldierItem( DWORD dwCharIndex, const ITEM_DATA* pItem );

public:
	void ClearAwardSoldierMap();

public:
	static ioGlobalSoldierInfo& GetSingleton();

public:
	ioGlobalSoldierInfo();
	virtual ~ioGlobalSoldierInfo();
};

#define g_GlobalSoldierInfo ioGlobalSoldierInfo::GetSingleton()

