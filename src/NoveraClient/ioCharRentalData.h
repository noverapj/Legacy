#pragma once

#include "CharacterInfo.h"
#include "ioUserExtraItem.h"
#include "ioUserMedalItem.h"

struct RentalData
{
	ioHashString m_szOwnerID;

	int       m_dwCharIndex;
	int       m_iClassLevel;
	ITEM_DATA m_EquipItem[MAX_INVENTORY];
	MedalSyncInfoVec  m_EquipMedal;
	BYTE      m_CharGrowth[MAX_CHAR_GROWTH];
	BYTE      m_ItemGrowth[MAX_CHAR_GROWTH];
	CostumeEquipInfo m_CostumeEquipInfo[MAX_INVENTORY];
	AccessoryEquipInfo m_AccessoryEquipInfo[MAX_INVENTORY];

	RentalData()
	{
		Initialize();
	}

	void Initialize()
	{
		m_szOwnerID.Clear();

		m_dwCharIndex = 0;
		m_iClassLevel = 0;

		int i = 0;
		for(i = 0;i < MAX_INVENTORY;i++)
			m_EquipItem[i].Initialize();
		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			m_CharGrowth[i] = 0;
		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			m_ItemGrowth[i] = 0;
		for(i = 0;i < MAX_INVENTORY;i++)
			m_CostumeEquipInfo[i].Init();
		for(i = 0;i < MAX_ACC_INVENTORY;i++)
			m_AccessoryEquipInfo[i].Init();

		m_EquipMedal.clear();
	}

	void SetOwnerID( const ioHashString &rkName ){ m_szOwnerID = rkName; }

	void ApplyData( SP2Packet &rkPacket )
	{
		rkPacket >> m_dwCharIndex;
		rkPacket >> m_iClassLevel;

		int i = 0;
		for(i = 0;i < MAX_INVENTORY;i++)
		{
			rkPacket >> m_EquipItem[i].m_item_code >> m_EquipItem[i].m_item_reinforce;
			rkPacket >> m_EquipItem[i].m_item_male_custom >> m_EquipItem[i].m_item_female_custom;
		}

		int iMedalSize;
		rkPacket >> iMedalSize;
		for(i = 0;i < iMedalSize;i++)
		{
			MedalSyncInfo cInfo;
			rkPacket >> cInfo.m_iItemType;
			m_EquipMedal.push_back( cInfo );
		}
		int iMaxCustomMedal;
		rkPacket >> iMaxCustomMedal;
		for( i = 0; i < iMaxCustomMedal; ++i )
		{
			MedalSyncInfo cInfo;
			rkPacket >> cInfo.m_iItemType >> cInfo.m_iCustomIndex;
			for( int j = 0; j < 8; j++ )
				rkPacket >> cInfo.m_iStat[j];

			m_EquipMedal.push_back( cInfo );
		}

		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			rkPacket >> m_CharGrowth[i];

		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			rkPacket >> m_ItemGrowth[i];
	}

	void ApplyCostumeData( const CostumeEquipInfo sCostumeEquipInfo[MAX_INVENTORY] )
	{
		for ( int i=0; i<MAX_INVENTORY; i++ )
			m_CostumeEquipInfo[i] = sCostumeEquipInfo[i];
	}

	void ApplyAccessoryData( const AccessoryEquipInfo sAccessoryEquipInfo[MAX_ACC_INVENTORY] )
	{
		for ( int i=0; i<MAX_ACC_INVENTORY; i++ )
			m_AccessoryEquipInfo[i] = sAccessoryEquipInfo[i];
	}
};
typedef std::vector< RentalData > RentalDataList;

class ioCharRentalData
{
protected:	
	RentalDataList m_RentalDataList;
    
public:
	void Initialize();

public:
	const ioHashString &GetOwnerName( const DWORD dwCharIndex );
	int GetClassLevel( const DWORD dwCharIndex );
	void GetEquipItem( const DWORD dwCharIndex, ITEM_DATA &rkEquipItem, int iSlot );
	bool GetEquipExtraItem( const DWORD dwCharIndex, EXTRAITEMSLOT &rkExtraItem, int iSlot );
	void GetEquipMedal( const DWORD dwCharIndex, MedalSyncInfoVec &rkEquipMedal );
	void GetCharGrowth( const DWORD dwCharIndex, BYTE &rkCharGrowth, int iSlot );
	void GetItemGrowth( const DWORD dwCharIndex, BYTE &rkItemGrowth, int iSlot );
	bool GetEquipCostume( const DWORD dwCharIndex, CostumeSlot &sCostumeSlot, int nSlot );
	bool GetEquipAccessory( const DWORD dwCharIndex, AccessorySlot &sAccessorySlot, int iSlot );

public:
	void EquipItem( CHARACTERDATA &rkCharData, bool bBulletInit = true );
	void EquipCostume( CHARACTERDATA &rkCharData ); 
	void EquipAccessory( CHARACTERDATA &rkCharData ); 

public:
	void InsertRentalData( RentalData &rkRentalData );
	void DeleteRentalData( const DWORD dwCharIndex );

public:
	ioCharRentalData();
	virtual ~ioCharRentalData();
};

