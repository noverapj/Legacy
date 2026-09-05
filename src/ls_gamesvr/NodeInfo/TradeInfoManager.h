

#ifndef _TradeInfoManager_h_
#define _TradeInfoManager_h_

#include "../Util/Singleton.h"


//////////////////////////////////////////////////////////////////////////////////////////

class TradeInfoManager : public Singleton< TradeInfoManager >
{
protected:
	int m_iTradePeriod;
	int m_iRegisterLimitLevel;
	int m_iItemBuyLimitLevel;

	float m_fRegisterTex;
	float m_fBuyTex;

	float m_fPCRoomBuyTex;
	float m_fPCRoomRegisterTex;

public:
	void CheckNeedReload();
	void LoadTradeInfo();

public:
	inline int GetTradePeriod() const { return m_iTradePeriod; }

	inline int GetRegisterLimitLv() const { return m_iRegisterLimitLevel; }
	inline int GetItemBuyLimitLv() const { return m_iItemBuyLimitLevel; }
	inline float GetRegisterTexRate() const { return m_fRegisterTex; }
	inline float GetBuyTexRate() const { return m_fBuyTex; }
	inline float GetPCRoomBuyTexRate() const { return m_fPCRoomBuyTex; }
	inline float GetPCRoomRegisterTexRate() const { return m_fPCRoomRegisterTex; }

public:
	static TradeInfoManager& GetSingleton();

public:
	TradeInfoManager();
	virtual ~TradeInfoManager();
};

#define g_TradeInfoMgr TradeInfoManager::GetSingleton()

#endif
