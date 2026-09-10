#pragma once


class SP2Packet;
class ioINILoader;

#define EVENT_GOODS_INFINITY_COUNT         -1
class ioEventGoodsMgr : public Singleton< ioEventGoodsMgr >
{	
protected:
	enum
	{
		SYNC_TIME = 10000, 
	};

protected:    // 상품
	struct GoodsDatabase
	{
		// 유효 인덱스
		DWORD m_dwGoodsIndex;         

		// 상품 정보 - 선물과 동일하다
		short m_iPresentType;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		int   m_iPresentValue3;
		int   m_iPresentValue4;

		// UI
		int   m_iManualIndex;
		int   m_iGoodsOrder;
		int   m_iGoodsMark;
		GoodsDatabase()
		{
			m_dwGoodsIndex = 0;
			m_iPresentType = 0;
			m_iPresentValue1 = m_iPresentValue2 = 0;
			m_iPresentValue3 = m_iPresentValue4 = 0;

			m_iManualIndex = m_iGoodsOrder = m_iGoodsMark = 0;
		}
	};
	typedef std::vector< GoodsDatabase > vGoodsDatabase;
	vGoodsDatabase m_GoodsDatabase;

protected:
	struct GoodsData
	{
		int m_iGoodsCount;
		DWORD m_dwNeedEtcItem;
		int m_iNeedEtcItemCount;
		GoodsDatabase m_Database;
		GoodsData()
		{
			m_dwNeedEtcItem = 0;
			m_iGoodsCount = m_iNeedEtcItemCount = 0;
		}
	};
	typedef std::vector< GoodsData > vGoodsData;
	vGoodsData m_GoodsDataList;

protected:
	DWORD m_dwCurrentTime;
	bool  m_bSyncEventShop;
	bool  m_bEventShopOpen;
	bool  m_bCloverShopOpen;
	bool  m_bMileageShopOpen;

public:
	void LoadEventGoodsInfo();

public:
	void SyncGoodsList();
	bool IsSyncEventShop();

public:
	void ApplyGoodsList( SP2Packet &rkPacket );

public:
	bool IsEventShopOpen(){ return m_bEventShopOpen; }
	void EventShopClose(); 

public:
	bool IsCloverShopOpen(){ return m_bCloverShopOpen; }
	void CloverShopClose();

public:
	bool IsMileageShopOpen(){ return m_bMileageShopOpen; }
	void MileageShopClose();

public:
	int  GetMaxGoods(){ return m_GoodsDataList.size(); }
	void GetGoods( int iArray, DWORD &rGoodsIndex, int &rGoodsCount, DWORD &rdwNeedEtcItem, int &rNeedEtcItemCount, 
				   short &rPresentType, int &rPresentValue1, int &rPresentValue2, int &rPresentValue3, int &rPresentValue4, int &rManualIndex, int &rGoodsOrder, int &rGoodsMark );
public:
	void BuyGoodsComplete( DWORD dwGoodsIndex, DWORD dwEtcItem, int iNeedCoin );

public:
	static ioEventGoodsMgr& GetSingleton();

public:
	ioEventGoodsMgr();
	virtual ~ioEventGoodsMgr();
};

#define g_EventGoodsMgr ioEventGoodsMgr::GetSingleton()

