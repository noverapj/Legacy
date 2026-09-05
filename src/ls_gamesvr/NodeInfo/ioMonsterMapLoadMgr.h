

#ifndef _ioMonsterMapLoadMgr_h_
#define _ioMonsterMapLoadMgr_h_

#include "../Util/Singleton.h"
#include "MonsterSurvivalMode.h"

class ioMonsterMapLoadMgr : public Singleton< ioMonsterMapLoadMgr >
{
protected:
	typedef std::map< DWORD, TurnData > TurnDataMap;
	TurnDataMap m_TurnDataMap;

protected:    // Monster Drop Item
	struct MonsterDropItem
	{
		ITEM_DATA m_item_data;
		DWORD     m_dwRateValue;
		MonsterDropItem()
		{
			m_item_data.Initialize();
			m_dwRateValue = 0;
		}
	};
	typedef std::vector< MonsterDropItem > vMonsterDropItem;
	struct MonsterDropItemList
	{
		vMonsterDropItem m_ItemList;
		DWORD            m_dwRandSeed;
		MonsterDropItemList()
		{
			m_dwRandSeed = 0;
		}
	};
	typedef std::map< DWORD, MonsterDropItemList > MonsterDropItemMap;
	MonsterDropItemMap m_DropItemTable;
	IORandom           m_DropItemRandom;

protected:   
	struct MonsterDropRewardItemList
	{
		DWORD m_dwRandSeed;
		vMonsterDropRewardItem m_ItemList;
		MonsterDropRewardItemList()
		{
			m_dwRandSeed = 0;
		}
	};
	typedef std::map< DWORD, MonsterDropRewardItemList > MonsterDropRewardItemMap;
	MonsterDropRewardItemMap m_DropRewardItemTable;
	IORandom           m_DropRewardItemRandom;

protected:
	struct MonsterCreateTable
	{
		DWORDVec m_MonsterRandomCode;
		MonsterRecordList m_vNormalMonster;
		MonsterRecord m_BossMonster;
	};
	typedef std::map< DWORD, MonsterCreateTable > MonsterCreateMap;
	MonsterCreateMap m_MonsterCreateTable;

protected:
	struct MonsterRandCreateTable
	{
		DWORD m_dwRondomSeed;
		MonsterRecordList m_vMonster;
		MonsterRandCreateTable()
		{
			m_dwRondomSeed = 0;
		}
	};
	typedef std::map< DWORD, MonsterRandCreateTable > MonsterRandCreateMap;
	MonsterRandCreateMap m_MonsterRandCreateTable;
	IORandom             m_MonsterRandCreateRandom;

protected:  // 주사위
	struct MonsterDicePresent
	{
		DWORD m_dwRandValue;

		short m_iPresentType;
		short m_iPresentState;
		short m_iPresentMent;
		int   m_iPresentPeriod;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		MonsterDicePresent()
		{
			m_dwRandValue = 0;
			m_iPresentType = m_iPresentState = m_iPresentMent = 0;
			m_iPresentPeriod = m_iPresentValue1 = m_iPresentValue2 = 0;
		}
	};
	typedef std::vector< MonsterDicePresent > MonsterDicePresentList;
	struct MonsterDiceTable
	{
		DWORD m_dwAllRandSeed;
		MonsterDicePresentList m_PresentList;
		MonsterDiceTable()
		{
			m_dwAllRandSeed = 0;
		}
	};
	typedef std::map< DWORD, MonsterDiceTable > MonsterDiceTableMap;
	MonsterDiceTableMap m_MonsterDiceTableMap;
	IORandom            m_MonsterDiceRandom;

	IntVec m_vDamageDiceValue; 

protected:	// 보물 카드
	struct RandValueData
	{
		DWORD m_dwRandValue;
		int   m_iRandData;
		RandValueData()
		{
			m_dwRandValue = 0;
			m_iRandData   = 0;
		}
	};
	typedef std::vector< RandValueData > RandValueDataVec;
	struct TreasureCardExtraItemRand
	{
		DWORD m_dwAllLimitDateSeed;
		DWORD m_dwAllReinforceSeed;
		RandValueDataVec m_LimitDateList;
		RandValueDataVec m_ReinforceList;
		TreasureCardExtraItemRand()
		{
			m_dwAllLimitDateSeed = 0;
			m_dwAllReinforceSeed = 0;
		}
	};
	typedef std::map< DWORD, TreasureCardExtraItemRand > TreasureCardExtraItemRandMap;
	TreasureCardExtraItemRandMap m_TreasureCardExtraItemRandMap;
	IORandom					 m_TreasureCardExtraItemRandom;

	struct MonsterTreasureCard
	{
		DWORD m_dwRandValue;

		short m_iPresentType;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		MonsterTreasureCard()
		{
			m_dwRandValue = 0;
			m_iPresentType = 0;
			m_iPresentValue1 = m_iPresentValue2 = 0;
		}
	};
	typedef std::vector< MonsterTreasureCard > MonsterTreasureCardList;
	struct MonsterTreasureCardTable
	{
		DWORD m_dwAllRandSeed;

		// 공용
		short        m_iPresentState;
		short        m_iPresentMent;
		int          m_iPresentPeriod;

		MonsterTreasureCardList m_CardList;
		MonsterTreasureCardTable()
		{
			m_dwAllRandSeed = 0;
			m_iPresentState = m_iPresentMent = 0;
			m_iPresentPeriod = 0;
		}
	};
	typedef std::map< DWORD, MonsterTreasureCardTable > MonsterTreasureCardMap;
	MonsterTreasureCardMap m_MonsterTreasureCardMap;
	IORandom               m_MonsterTreasureCardRandom;
  
protected:
	void ClearData();
	void LoadTurnData();
	void LoadMonsterTable();
	void LoadRandMonsterTable();
	void LoadDropItemData();
	void LoadDropRewardItemData();
	void LoadMonsterDiceTable();
	void LoadMonsterTreasureCardTable();

public:
	void LoadMapData();

public:
	bool GetTurnData( DWORD dwIndex, TurnData &rkTurnData, int iHighTurn, int iLowTurn, int &rNameCount, bool bAutoDropItem = true );
	bool GetRandMonster( DWORD dwTable, MonsterRecordList &rkMonster, int &rNameCount, int iMonsterCount, bool bFieldUniqueLive, bool bAutoDropItem = true );
	void GetMonsterDropItem( DWORD dwTableIndex, ITEM_DATA &rkItem );
	void GetMonsterDropRewardItem( DWORD dwTableIndex, MonsterDropRewardItem &rkReturnItem );
	void GetMonsterDicePresent( DWORD dwTableIndex, ioHashString &rkSendID, short &rPresentType, short &rPresentState, 
								short &rPresentMent, int &rPresentPeriod, int &rPresentValue1, int &rPresentValue2 );
	void GetMonsterDicePresent( DWORD dwTableIndex, int nSubIdx, ioHashString &rkSendID, short &rPresentType, short &rPresentState, 
		short &rPresentMent, int &rPresentPeriod, int &rPresentValue1, int &rPresentValue2 );
	int  GetMonsterDiceRate( int iDamageRank );
	void GetTreasureCardPresent( DWORD dwTableIndex, ioHashString &rkSendID, short &rPresentType, short &rPresentState, 
							 	 short &rPresentMent, int &rPresentPeriod, int &rPresentValue1, int &rPresentValue2 );
	int  GetTreasureCardPresentValue2( short iPresentType, int iPresentValue2 );

public:
	static ioMonsterMapLoadMgr& GetSingleton();

public:
	ioMonsterMapLoadMgr();
	virtual ~ioMonsterMapLoadMgr();
};

#define g_MonsterMapLoadMgr ioMonsterMapLoadMgr::GetSingleton()

#endif