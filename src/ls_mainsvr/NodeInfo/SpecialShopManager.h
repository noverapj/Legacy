#pragma once

#include <vector>
#include <map>

class ServerNode;

class SpecialShopManager 
{
public:
	struct SellDate
	{
		int   m_iTabInfo;
		DWORD m_dwStartDate;
		DWORD m_dwEndDate;

		SellDate()
		{
			m_iTabInfo	  = 0;
			m_dwStartDate = 0;
			m_dwEndDate	  = 0;
		}
	};

	struct GoodsInfo
	{
		DWORD m_dwGoodsCode;
		int m_iCurGoodsCount;
		int m_iOriginCount;
		int m_iBuyReserveCount;
		int m_iUserBuyLimitCount;
		int m_iRefillCount;
		bool m_bChangeFlag;

		GoodsInfo()
		{
			m_dwGoodsCode		= 0;
			m_iCurGoodsCount	= 0;
			m_iOriginCount		= 0;
			m_iBuyReserveCount	= 0;
			m_iUserBuyLimitCount= 0;
			m_iRefillCount		= 0;
			m_bChangeFlag		= false;
		}
	};

	struct UserBuyInfo
	{
		DWORD m_dwGoodsCode;
		int m_iBuyCount;

		UserBuyInfo()
		{
			m_dwGoodsCode	= 0;
			m_iBuyCount		= 0;
		}
	};

public:
	SpecialShopManager();
	virtual ~SpecialShopManager();

	void Init();
	void Destroy();

	void LoadINI();

public:
	static SpecialShopManager &GetInstance();

public:
	inline bool IsOpen() { return m_bOpen; }
	inline void ChangeOpenInfo( bool bOpen ) { m_bOpen = bOpen; }

	void FillCurSellGoodsInfo(SP2Packet& kPacket);
	void ChangeCurGoodsInfo(DWORD dwStartDate);

	void CheckShopState();

	int	GetUserBuyCount(DWORD dwUserIndex, DWORD dwItemCode);
	void UpdateUserBuyInfo(DWORD dwUserIndex, DWORD dwItemCode);

	void SendGoodsUpdateInfo();

	void RenewalGoodsCount(const DWORD dwItemCode, int iItemCount);

public:
	void BuySpecialShopGoods( ServerNode *pServerNode, SP2Packet &rkPacket );
	void BuyResultSpecialShopGoods( ServerNode *pServerNode, SP2Packet &rkPacket );

	void SetActiveTab(int val) { m_iActiveTab = val; }
	int GetActiveTab()	{ return m_iActiveTab; }

protected:
	static SpecialShopManager *sg_Instance;

protected:
	typedef std::vector<SellDate> vGoodsSellDate;
	typedef std::vector<GoodsInfo> vGoodsInfo;
	typedef std::vector<UserBuyInfo> vUserBuyInfo;

	typedef std::map<DWORD, vGoodsInfo> mAllGoodsInfo;
	typedef std::map<DWORD, vUserBuyInfo> mUserBuyInfo;
	
protected:
	bool m_bOpen;
	int m_iActiveTab;

	vGoodsInfo m_vCurSellGoodsInfo;
	vGoodsSellDate m_vGoodsSellDate;
	mAllGoodsInfo m_mAllGoodsInfo;		// <판매 시작 값, 해당 판매 상품들>
	mUserBuyInfo m_mUserBuyInfo;		// <유저 index, 구매한 상품별 체크>

	DWORD	m_dwActiveDate;

	int m_iRefillCycleDay;
	int m_iRefillCycleHour;

};

#define g_SpecialShopManager SpecialShopManager::GetInstance()