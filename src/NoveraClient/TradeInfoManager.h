#pragma once

#include "EncryptValue.h"

///////////////////////////////////////////////////////////////////////////

#define MAX_TRADE_ITEM_LIST	 20

struct TradeItemInfo
{
	DWORD		m_dwRegisterUserIndex;
	DWORD		m_dwTradeIndex;
	DWORD		m_dwItemType;
	DWORD		m_dwItemMagicCode;
	DWORD		m_dwItemValue;
	DWORD		m_dwItemMaleCustom;
	DWORD		m_dwItemFemaleCustom;

	__int64		m_iItemPrice;

	DWORD		m_dwRegisterStartDate;
	DWORD		m_dwRegisterEndDate;
	DWORD		m_dwPeriodTime;

	ioHashString m_szRegisterUserNick;

	TradeItemInfo()
	{
		Init();
	};

	void Init()
	{
		m_dwRegisterUserIndex	= 0;

		m_dwTradeIndex			= -1;

		m_dwItemType			= 0;
		m_dwItemMagicCode		= 0;
		m_dwItemValue			= 0;
		m_dwItemMaleCustom		= 0;
		m_dwItemFemaleCustom	= 0;

		m_iItemPrice			= 0;

		m_dwRegisterStartDate	= 0;
		m_dwRegisterEndDate		= 0;

		m_dwPeriodTime			= 0;

		m_szRegisterUserNick	= "";
	};

};
typedef std::vector<TradeItemInfo> TradeItemInfoList;


class TradeHighGoldSort : public std::binary_function< const TradeItemInfo& , const TradeItemInfo&, bool >
{
public:
	bool operator() ( const TradeItemInfo &lhs, const TradeItemInfo &rhs ) const
	{
		if( lhs.m_iItemPrice > rhs.m_iItemPrice )
		{
			return true;
		}
		return false;
	}
};

class TradeLowGoldSort : public std::binary_function< const TradeItemInfo& , const TradeItemInfo&, bool >
{
public:
	bool operator() ( const TradeItemInfo &lhs, const TradeItemInfo &rhs ) const
	{
		if( lhs.m_iItemPrice < rhs.m_iItemPrice )
		{
			return true;
		}
		return false;
	}
};

class TradeInfoManager : public Singleton< TradeInfoManager >
{
protected:
	CEncrypt<__int64>	m_iRegisterLimitPeso;
	int					m_iTradePeriod;
	CEncrypt<int>		m_iTradeStateChangeLevel;

	int					m_iRegisterLimitLevel;
	int					m_iItemBuyLimitLevel;

	float				m_fRegisterTex;
	float				m_fBuyTex;

	float				m_fPCRoomRegisterTex;
	float				m_fPCRoomBuyTex;
	
	DWORD				m_dwTradeReqTime;

protected:
	TradeItemInfoList	m_vTradeItemInfoList;
	DWORD				m_dwCurrTradeReqTime;

public:
	static TradeInfoManager& GetSingleton();

public:
	void LoadTradeInfo();

	inline __int64 GetRegisterLimitPeso() const { return m_iRegisterLimitPeso; }

	inline int GetTradePeriod() const { return m_iTradePeriod; }
	inline int GetTradeStateChangeLevel() const { return m_iTradeStateChangeLevel; }

	inline int GetRegisterLimitLv() const { return m_iRegisterLimitLevel; }
	inline int GetItemBuyLimitLv() const { return m_iItemBuyLimitLevel; }
	inline float GetRegisterTexRate() const { return m_fRegisterTex; }
	inline float GetBuyTexRate() const { return m_fBuyTex; }

	inline float GetPCRoomRegisterTexRate() const { return m_fPCRoomRegisterTex; }
	inline float GetPCRoomBuyTexRate() const { return m_fPCRoomBuyTex; }

public:
	void ClearTradeList();
	void SyncTradeList( bool bMust = false );
	void ApplyTradeList( SP2Packet& rkPacket );

public:
	void GetTradeItemList( TradeItemInfoList& TradeList );
	void GetTradeItemListByUser( TradeItemInfoList& TradeList, DWORD UserIndex );
	
public:
	void AddTradeItem( const TradeItemInfo& Info );
	void RemoveTradeItem( DWORD dwTradeIdx );

public:
	TradeInfoManager();
	virtual ~TradeInfoManager();
};

#define g_TradeInfoMgr TradeInfoManager::GetSingleton()

