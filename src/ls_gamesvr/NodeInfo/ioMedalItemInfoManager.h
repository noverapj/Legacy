#ifndef __ioMedalItemInfoManager_h__
#define __ioMedalItemInfoManager_h__

#include "../Util/Singleton.h"
#include "ioUserGrowthLevel.h"

class ioMedalItemInfoManager : public Singleton< ioMedalItemInfoManager >
{
public:
	enum 
	{
		MAX_SLOT_NUM = 6,
	};
protected:
	struct ItemInfo
	{
		int m_iItemType;
		int m_iLimitLevel;
		int m_iSellPeso;
		DWORDVec m_vUseClassTypeVec;
		int m_iCharGrowth[MAX_CHAR_GROWTH];
		int m_iItemGrowth[MAX_ITEM_GROWTH];

		//HYROON 20170206 커스튬메달
		int m_iSubMedalType;
		int m_iEnableSelectStat;
		int m_iTotalPoint;
		int m_iMinStatPoint;
		int m_iMaxStatPoint;

		ItemInfo()
		{
			m_iItemType   = 0;
			m_iLimitLevel = 0;
			m_iSellPeso   = 0;
			m_vUseClassTypeVec.clear(); // empty면 모든 캐릭터 장착가능

			for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
				m_iCharGrowth[i] = 0;
			for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
				m_iItemGrowth[i] = 0;

			m_iSubMedalType		 = 0;
			m_iEnableSelectStat  = 0;
			m_iTotalPoint		 = 0;
			m_iMinStatPoint		 = 0;
			m_iMaxStatPoint		 = 0;

		}
	};

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
	typedef std::vector< ItemInfo > vItemInfoVec;
	typedef std::vector< SlotInfo > vSlotInfoVec;

protected:
	vSlotInfoVec m_vSlotInfoVec;
	vItemInfoVec m_vItemInfoVec;
	int          m_iItemSellDevideMin;
	bool		 m_bINILoading;
	
protected:
	const ItemInfo *GetItemInfo( int iItemType ); 
	void Clear();
	void LoadMedalItemInfo( ioINILoader &rkLoader );

public:
	void LoadINI();

	DWORD GetLevelLimit( int iItemType );
	int   GetSlotNum( int iCurLevel );
	bool  IsRight( int iItemType, int iClassType );
	int   GetMedalItemGrowth( int iItemType, bool bCharGrowth, int iArray );
	int   GetSellPeso( int iItemType );
	int   GetSellPesoByMinute() { return m_iItemSellDevideMin; }
	
	int   GetSubMedalType( int iItemType );
	int   GetMinStatPoint( int iItemType );
	int   GetMaxStatPoint( int iItemType );
	int   GetSelectStat( int iItemType );
	int	  GetTotalPoint( int iItemType );

public:
	static ioMedalItemInfoManager& GetSingleton();

public:
	ioMedalItemInfoManager(void);
	virtual ~ioMedalItemInfoManager(void);
};

#define g_MedalItemMgr ioMedalItemInfoManager::GetSingleton()

#endif // __ioMedalItemInfoManager_h__