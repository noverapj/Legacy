#pragma once

class ioSpecialShopMgr : public Singleton< ioSpecialShopMgr >
{
protected:
	enum
	{
		SYNC_TIME = 5000,
	};

public:
	struct SpecialGoods
	{
		int nCode;
		int nCount;

		SpecialGoods()
		{
			nCode	= 0;
			nCount	= 0;
		}
	};

	typedef std::vector<SpecialGoods> SpecialGoodsList;

public:
	struct SpecialShopInfo
	{
		CTime	m_ShopOpenTime;
		CTime	m_ShopCloseTime;
	};
	typedef std::vector<SpecialShopInfo> SpecialShopInfoList;

protected:
	SpecialGoodsList	m_SpecialGoodsSyncList; //스페셜 상점 목록(수량 포함)
	SpecialShopInfoList	m_SpecialShopInfoList;	//스페셜 상점 시간정보

	bool				m_bSpecailShopSync;
	bool				m_bSpecailShopOpen;

	DWORD				m_dwCurrentTime;	

public:
	static ioSpecialShopMgr& GetSingleton();

public:
	virtual void Initialize();
	void LoadGoods( ioINILoader& rkLoader );
	void LoadPeriod( ioINILoader& rkLoader, CTime& rkTime, const char* szTitle );

public:
	const SpecialGoods* const GetConstSpecialGoods( int nCode );

protected:
	SpecialGoods* GetSpecialGoods( int nCode );

public:
	bool IsSpecialShopOpen();	
	bool IsSpecialSyncGoods( int nCode );
	int GetSpecialGoodsCount( int nCode );
	
public:
	const ioSpecialShopMgr::SpecialShopInfo* GetShopInfo();
	const CTime* GetOpenTime();
	const CTime* GetCloseTime();

public:
	void SyncGoodsList();

public:
	bool ProcessTCPPacket( SP2Packet &rkPacket );

public:
	void OnSpecialShopGoodsList( SP2Packet &rkPacket );
	void OnSpecialShopBuyGoodsCount( SP2Packet &rkPacket );

public:
	ioSpecialShopMgr();
	virtual ~ioSpecialShopMgr();
};

#define g_SpecialShopMgr ioSpecialShopMgr::GetSingleton()