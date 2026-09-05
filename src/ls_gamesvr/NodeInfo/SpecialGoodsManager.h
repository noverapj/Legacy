#pragma once

#include "../Util/Singleton.h"
class SpecailGoodsManager : public Singleton< SpecailGoodsManager >
{
public:
	struct SpecialGoods
	{
		int iType;
		int iLimitCount;

		SpecialGoods()
		{
			Init();
		}

		void Init()
		{
			iType		= 0;
			iLimitCount = 0;
		}
	};

public:
	SpecailGoodsManager();
	virtual ~SpecailGoodsManager();

	void Init();
	void Destroy();

public:
	static SpecailGoodsManager& GetSingleton();

public:
	inline bool IsOpen() { return m_bOpen; }
	inline void SetOpen( bool bOpen ) { m_bOpen = bOpen; }

	inline void ClearSpecialGoodsList() { m_vSpecialGoodsList.clear(); }
	void SetSpecialGoodsList(SP2Packet& kPacket);

	bool IsPossibleBuy(DWORD dwItemType);

	void ChangeState(bool bState);
	void ChangeCurGoodsCount(DWORD dwGoodsCode, int iCurCount);

	void FillGoodsList( SP2Packet& kPacket );
protected:
	typedef std::vector<SpecialGoods> vSpecialGoodsList;

protected:
	bool m_bOpen;
	vSpecialGoodsList m_vSpecialGoodsList;

	// popup¿ë
	//////////////////////////////////////////////////////////////////////////
	enum { ePopupConditionTotalCash = 0, ePopupConditionMonthCash, ePopupConditionCurCash, ePopupConditionGrade, ePopupConditionPlayTime, ePopupConditionMax };

	struct sPopupItemInfo
	{
		bool bActive;

		int iIndex;
		int iPresentType;
		int iPresentValue1;
		int iPresentValue2;
		int iCash;
		int iPrevCash;
		int iDisCount;

		DWORD dwConditionMin[ePopupConditionMax];
		DWORD dwConditionMax[ePopupConditionMax];
	};

	std::vector<sPopupItemInfo> m_vecPopupItemInfo;
	bool IsCompareMinMax( DWORD dwMin, DWORD dwMax, DWORD dwValue );

public:

	void ReloadINI();
	void GetPopupStoreIndex( std::vector<int>& iUseVec, OUT std::vector<int>& outSendVec, DWORD* dwArray );
	bool GetBuyPopupItem( int iIndex, OUT int& iPresentType, OUT int& iPresentValue1, OUT int& iPresentValue2, OUT int& iNeedCash );
};

#define g_SpecialGoodsMgr SpecailGoodsManager::GetSingleton()