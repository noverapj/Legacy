#pragma once

#include "EncryptValue.h"

class ioMedalItemInfoManager : public Singleton< ioMedalItemInfoManager >
{
public:
	enum 
	{
		MAX_SLOT_NUM = 6,
	};
	enum SubMedalType
	{
		MEDALTYPE_NORMAL = 1,
		MEDALTYPE_CUSTOM = 2,
	};

public:
	struct ItemInfo
	{
		int		      m_iItemType;
		int           m_iLimitLevel;
		DWORDVec      m_vUseClassTypeVec;
		CEncrypt<int> m_iCharGrowth[MAX_CHAR_GROWTH];
		CEncrypt<int> m_iItemGrowth[MAX_ITEM_GROWTH];
		ioHashString  m_sName;
		ioHashString  m_sIcon;
		ioHashString  m_sSubIcon;
		int           m_iManual;
		int           m_iSellPeso;

		int m_iSubMedalType;
		int m_iSelectStat;
		int m_iMinStatPoint;
		int m_iMaxStatPoint;

		ItemInfo()
		{
 			m_iItemType   = 0;
 			m_iLimitLevel = 0;
 			m_vUseClassTypeVec.clear(); // empty면 모든 캐릭터 장착가능
 
 			for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
 				m_iCharGrowth[i] = 0;
 			for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
 				m_iItemGrowth[i] = 0;

			m_iManual   = 0;
			m_iSellPeso = 0;
			m_iSubMedalType = MEDALTYPE_NORMAL;
			m_iSelectStat = 0;
			m_iMinStatPoint = 0;
			m_iMaxStatPoint = 0;
		}
	};
protected:
	struct SlotInfo
	{
		int m_iLevelOver;
		int m_iSlotNum;

		SlotInfo()
		{
			m_iLevelOver = 0;
			m_iSlotNum    = 0;
		}
	};

protected:
	typedef std::vector< SlotInfo > vSlotInfoVec;

protected:
	vSlotInfoVec    m_vSlotInfoVec;
	ItemInfo       *m_pItemInfoArray; // 암호화 변수는 push_back이 안되므로 vector대신 포인터를 사용
	int             m_iMaxItemArray;

	CEncrypt<int>   m_iItemSellDevideMin;

protected:
	void Clear();

public:
	void LoadINI();

	DWORD GetLevelLimit( int iItemType );
	int   GetSlotNum( int iCurLevel );
	int   GetLevelOver( int iSlotNum );
	bool  IsRight( int iItemType, int iClassType );

	const ItemInfo *GetItemInfo( int iItemType ); 
	int   GetMedalItemGrowth( int iItemType, bool bCharGrowth, int iArray );

	int GetSellPeso( int iItemType );
	inline int GetSellPesoByMinute() { return m_iItemSellDevideMin; }

public:
	static ioMedalItemInfoManager& GetSingleton();

public:
	ioMedalItemInfoManager(void);
	virtual ~ioMedalItemInfoManager(void);
};

#define g_MedalItemMgr ioMedalItemInfoManager::GetSingleton()

