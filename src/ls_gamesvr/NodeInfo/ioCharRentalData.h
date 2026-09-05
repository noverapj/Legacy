
#ifndef _ioCharRentalData_h_
#define _ioCharRentalData_h_

#include "NodeHelpStructDefine.h"
#include "ioUserGrowthLevel.h"
#include "CustomMedal.h"

struct RentalData
{
	ioHashString m_szOwnerName;

	int       m_dwCharIndex;
	int       m_iClassLevel;
	ITEM_DATA m_EquipItem[MAX_CHAR_DBITEM_SLOT];
	DWORDVec  m_EquipMedal;
	//IntOfTwoVec vEquipCustomMedal;  // index, code
	IntOfStatVec vEquipCustomMedalData;  
	
	BYTE      m_CharGrowth[MAX_CHAR_GROWTH];
	BYTE      m_ItemGrowth[MAX_ITEM_GROWTH];

	int       m_CustomStat[CustomMedal::MEDAL_STAT];

	RentalData()
	{
		Initialize();
	}

	void Initialize()
	{
		m_dwCharIndex = 0;
		m_iClassLevel = 0;

		int i = 0;
		for(i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
			m_EquipItem[i].Initialize();
		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			m_CharGrowth[i] = 0;
		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			m_ItemGrowth[i] = 0;

		m_EquipMedal.clear();
		vEquipCustomMedalData.clear();

	}

	void FillData( SP2Packet &rkPacket )
	{
		rkPacket << m_dwCharIndex;
		rkPacket << m_iClassLevel;

		int i = 0;
		for(i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
		{
			rkPacket << m_EquipItem[i].m_item_code << m_EquipItem[i].m_item_reinforce;
			rkPacket << m_EquipItem[i].m_item_male_custom << m_EquipItem[i].m_item_female_custom;
		}
   
		rkPacket << (int)m_EquipMedal.size();
		for(i = 0;i < (int)m_EquipMedal.size();i++)
			rkPacket << m_EquipMedal[i];

		

		//Ä¿½ºÅÒ ¸Þ´Þ
		rkPacket << (int)vEquipCustomMedalData.size();

		/*if( (int)vEquipCustomMedalData.size() == 0 )
		{
			rkPacket << (int)0;
			rkPacket << (int)0;
		}
		*/
		for(i = 0;i < (int)vEquipCustomMedalData.size();i++)
		{
			if( vEquipCustomMedalData[i].index != 0 && vEquipCustomMedalData[i].itemCode != 0 )
			{
				rkPacket << vEquipCustomMedalData[i].itemCode;
				rkPacket << vEquipCustomMedalData[i].index;
				rkPacket << vEquipCustomMedalData[i].stat1;
				rkPacket << vEquipCustomMedalData[i].stat2;
				rkPacket << vEquipCustomMedalData[i].stat3;
				rkPacket << vEquipCustomMedalData[i].stat4;
				rkPacket << vEquipCustomMedalData[i].stat5;
				rkPacket << vEquipCustomMedalData[i].stat6;
				rkPacket << vEquipCustomMedalData[i].stat7;
				rkPacket << vEquipCustomMedalData[i].stat8;
			}
			else
			{
				rkPacket << (int)0;
				rkPacket << (int)0;
			}
		}

		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			rkPacket << m_CharGrowth[i];

		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			rkPacket << m_ItemGrowth[i];
		
	}

	void ApplyData( SP2Packet &rkPacket )
	{
		rkPacket >> m_dwCharIndex;
		rkPacket >> m_iClassLevel;

		int i = 0;
		for(i = 0;i < MAX_CHAR_DBITEM_SLOT;i++)
		{
			rkPacket >> m_EquipItem[i].m_item_code >> m_EquipItem[i].m_item_reinforce;
			rkPacket >> m_EquipItem[i].m_item_male_custom >> m_EquipItem[i].m_item_female_custom;
		}

		int iMedalSize;
		rkPacket >> iMedalSize;
		for(i = 0;i < iMedalSize;i++)
		{
			int iMedalCode;
			rkPacket >> iMedalCode;
			m_EquipMedal.push_back( iMedalCode );
		}

		int iSize = 0;
		rkPacket >> iSize;

		//if( iSize == 0 )
		//{
		//	int i, j = 0;
		//	//rkPacket >> i >> j;
		//}

		for(i = 0; i < iSize ;i++)
		{
			int iCode = 0, iIndex = 0;
			IntOfStat stInfo;
			
			rkPacket >> iCode;
			rkPacket >> iIndex;

			stInfo.index = iIndex;
			stInfo.itemCode = iCode;

			if( iIndex != 0 && iCode != 0 )
			{
				rkPacket >> stInfo.stat1;
				rkPacket >> stInfo.stat2;
				rkPacket >> stInfo.stat3;
				rkPacket >> stInfo.stat4;
				rkPacket >> stInfo.stat5;
				rkPacket >> stInfo.stat6;
				rkPacket >> stInfo.stat7;
				rkPacket >> stInfo.stat8;
				vEquipCustomMedalData.push_back( stInfo );
			}
		}

		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			rkPacket >> m_CharGrowth[i];

		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			rkPacket >> m_ItemGrowth[i];

		/*int iCustomMedalSize = 0;
		rkPacket >> iCustomMedalSize;
		for( int i = 0; i< iCustomMedalSize; i++)
		{
			IntOfTwo stInfo;
			rkPacket >> stInfo.value2 >> stInfo.value1;
			vEquipCustomMedal.push_back( stInfo );
		}

		for(i = 0;i < CustomMedal::MEDAL_STAT;i++)
			rkPacket >> m_CustomStat[i];*/
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
	int GetClassLevel( const DWORD dwCharIndex );
	void GetEquipItem( const DWORD dwCharIndex, ITEM_DATA &rkEquipItem, int iSlot );
	void GetEquipMedal( const DWORD dwCharIndex, IntVec &rkEquipMedal );
	void GetEquipCustomMedal( const DWORD dwCharIndex, IntOfStatVec &rkEquipMedal );
	void GetCharGrowth( const DWORD dwCharIndex, BYTE &rkCharGrowth, int iSlot );
	void GetItemGrowth( const DWORD dwCharIndex, BYTE &rkItemGrowth, int iSlot );
	RentalData &GetRentalData( const DWORD dwCharIndex );

public:
	void InsertRentalData( const ioHashString &rkOwnerName, RentalData &rkRentalData );
	void DeleteRentalData( const DWORD dwCharIndex );

public:
	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket );

public:
	bool FillGrowthData( const DWORD dwCharIndex, SP2Packet &rkPacket );
	bool FillGrowthCustomData( const DWORD dwCharIndex, SP2Packet &rkPacket );


public:
	ioCharRentalData();
	virtual ~ioCharRentalData();
};

#endif