#pragma once

#include <boost/unordered/unordered_map.hpp>
#include "../BoostPooler.h"

class ServerNode;

#define EVENT_GOODS_INFINITY_COUNT         -1
#define GOODS_INDEX_HALF_DATA              100000


class EventGoodsManager : public SuperParent
{
protected:
	static EventGoodsManager *sg_Instance;

protected:
	DWORD m_dwCurrentTime;

protected:
	enum ShopState
	{
		SS_OPEN = 0,
		SS_CLOSE,
	};
	ShopState	   m_eEventShopState;
	ShopState      m_eCloverShopState;
	ShopState      m_eMileageShopState;
	ioHashString   m_szEventShopState;
	ioHashString   m_szCloverShopState;
	ioHashString   m_szMileageShopState;

	bool m_bEventGoodsChange;
	bool m_bCloverGoodsChange;

public:
	int m_Page;
	int m_PagingSize;

	int GetPage()		{ return m_Page; }
	int GetPagingSize()	{ return m_PagingSize; }

	enum ShopType
	{
		ST_EVENT	= 0,
		ST_CLOVER,
		ST_MILEAGE,
		ST_END,
	};

	class UserBuyData : public BoostPooler<UserBuyData>
	{
	public:
		UserBuyData()
		{}

	public:
		void Clear()
		{
			m_EventShop.clear();
			m_CloverShop.clear();
		}
		int GetCount(const int iShopType, const DWORD m_dwGoodsIndex)
		{
			if(ST_EVENT == iShopType)
			{
				BUYTABLE::iterator it = m_EventShop.find(m_dwGoodsIndex);
				if(it != m_EventShop.end())
				{
					return it->second;
				}
				return 0;
			}
			else
			{
				BUYTABLE::iterator it = m_CloverShop.find(m_dwGoodsIndex);
				if(it != m_CloverShop.end())
				{
					return it->second;
				}
				return 0;
			}
		}
		void SetCount(const int iShopType, const DWORD m_dwGoodsIndex, const int iCount)
		{
			if(ST_EVENT == iShopType)
			{
				m_EventShop[m_dwGoodsIndex] = iCount;
			}
			else
			{
				m_CloverShop[m_dwGoodsIndex] = iCount;
			}
		}

	protected:
		typedef boost::unordered_map<DWORD,int> BUYTABLE;
		BUYTABLE m_EventShop;
		BUYTABLE m_CloverShop;
	};

protected:    // 상품
	struct GoodsData
	{
		// 상품 정보 - 선물과 동일하다.
		ioHashString m_szSendID;
		short m_iPresentType;
		bool  m_bPresentAlarm;
		int   m_iPresentMent;
		int   m_iPresentPeriod;	
		int   m_iPresentValue1;
		int   m_iPresentValue2;	
		int   m_iPresentValue3;
		int   m_iPresentValue4;
		bool  m_bPresentDirect;

		// 상품 정보 - 수량 / 구매 조건 등등..
		char  m_szINITitle[MAX_PATH]; // INI 타이틀
		DWORD m_dwGoodsIndex;         // 유효 인덱스
		int   m_iGoodsCount;          // 수량 - 0이 되어도 리스트에서 삭제하지 않는다.
		int   m_iOriginCount;         // 수량 - 최초 수량
		int   m_iBuyReserve;          // 구매 예약 개수
		DWORD m_dwNeedEtcItem;        // 필요한 코인 타입
		int   m_iNeedEtcItemCount;    // 필요한 코인 개수
		int   m_iUserBuyCount;        // 유저당 구매 개수

		// 판매 기간
		DWORD  m_dwStartDate;
		DWORD  m_dwEndDate;

		// 상품 타입
		ShopType m_eShopType;

		GoodsData()
		{
			m_iPresentType = 0;
			m_bPresentAlarm = false;
			m_iPresentMent = m_iPresentPeriod = m_iPresentValue1 = m_iPresentValue2 = m_iPresentValue3 = m_iPresentValue4 = 0;
			m_bPresentDirect= false;			

			m_dwGoodsIndex = m_dwNeedEtcItem = 0;
			memset( m_szINITitle, 0, sizeof( m_szINITitle ) );
			m_iGoodsCount = m_iOriginCount = m_iBuyReserve = m_iNeedEtcItemCount = m_iUserBuyCount = 0;

			m_dwStartDate = m_dwEndDate = 0;

			m_eShopType = ST_EVENT;
		}

		bool IsDateLimit(){ return (m_dwStartDate != 0); }
	};
	typedef std::vector< GoodsData > vGoodsData;
	vGoodsData m_GoodsList;

protected:
	UserBuyData* GetUserBuyData(const DWORD dwUserIndex)
	{
		USERBUYTABLE::iterator it = m_UserBuyList.find(dwUserIndex);
		if(it != m_UserBuyList.end())
		{
			return it->second;
		}
		return NULL;
	}

	typedef boost::unordered_map<DWORD,UserBuyData*> USERBUYTABLE;
	USERBUYTABLE m_UserBuyList;

protected:
	struct SaveData
	{
		ioHashString m_FileName;
		ioHashString m_Title;
		ioHashString m_Key;
		int          m_Value;
		SaveData()
		{
			m_Value = 0;
		}
	};
	typedef std::vector< SaveData > SaveDataVec;
	SaveDataVec m_SaveDataList;

protected:
	void InsertSaveData( const ioHashString &rkFileName, const ioHashString &rkTitle, const ioHashString &rkKey, int kValue );

public:
	void LoadINIData( bool bFirstLoad );
	void LoadGoodsList( ioINILoader &rkLoader, ShopType eShopType );
	void LoadUserBuyLog();

public:
	void Process();

public:
	void ApplyUserBuyData(const DWORD dwUserIndex, const DWORD dwGoodsIndex, const int iCount);
	bool InsertUserBuyData(const DWORD dwUserIndex, const DWORD dwGoodsIndex, const int iCount, const bool bLoading = true);
	int  GetUserBuyCount()	{ return (int)m_UserBuyList.size(); }
	int  GetUserBuyCount(const DWORD dwGoodsIndex, const DWORD dwUserIndex );
	void AddUserBuyCount( DWORD dwGoodsIndex, DWORD dwUserIndex );
	void ClearUserBuyCount();

public:
	void SetEventShopState( int iState );
	bool IsEventShopOpen(){ return (m_eEventShopState == SS_OPEN); }
	const ioHashString &GetEventShopState(){ return m_szEventShopState; }

	void SetCloverShopState( int iState );
	bool IsCloverShopOpen(){ return (m_eCloverShopState == SS_OPEN); }
	const ioHashString &GetCloverShopState(){ return m_szCloverShopState; }


	void SetMileageShopState( int iState );
	bool IsMileageShopOpen(){ return (m_eMileageShopState == SS_OPEN); }
	const ioHashString &GetMileageShopState(){ return m_szMileageShopState; }

public:
	void SaveGoodsCount( const int iShopType, const char *szTitle, int iGoodsCount );

public:
	void OnEventShopGoodsList( ServerNode *pServerNode, SP2Packet &rkPacket );
	bool _OnShopCloseCheck( int iShopType );
	void OnEventShopGoodsBuy( ServerNode *pServerNode, SP2Packet &rkPacket );
	void OnEventShopGoodsBuyResult( ServerNode *pServerNode, SP2Packet &rkPacket );

public:
	int GetSaveDataCount(){ return m_SaveDataList.size(); }
	void SaveDataOneWrite();
	void SaveDataAllWrite();

	void ResetBuyCount( const ShopType eShopType );
	void ResetBuyLog();
	void InitUserBuyCount();

public:
	static EventGoodsManager &GetInstance();
	static void ReleaseInstance();

private:     	/* Singleton Class */
	EventGoodsManager();
	virtual ~EventGoodsManager();
};

#define g_EventGoodsMgr EventGoodsManager::GetInstance()
