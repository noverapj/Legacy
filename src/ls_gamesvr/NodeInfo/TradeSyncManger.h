#ifndef __TradeSyncManger_h__
#define __TradeSyncManger_h__

#include "../Util/Singleton.h"

enum { MAX_SEND_ITEM = 200, };

// 아이템 리스트 전송 타입 
enum
{
	TRADE_ITEM_LIST_FIRST	= 0,	// 처음 전송되는 아이템 리스트
	TRADE_ITEM_LIST_SENDING	= 1,	// 전송 중인 아이템 리스트
	TRADE_ITEM_LIST_LAST	= 2,	// 마지막 아이템 리스트
};

class TradeSyncManager : public Singleton< TradeSyncManager > 
{
protected:
	struct TradeSyncInfo
	{
		DWORD m_dwUserIndex;
		DWORD m_dwTradeIndex;
		DWORD m_dwItemType;
		DWORD m_dwItemMagicCode;
		DWORD m_dwItemValue;
		
		DWORD m_dwItemMaleCustom;
		DWORD m_dwItemFemaleCustom;
		__int64 m_iItemPrice;
		DWORD m_dwRegisterDate1;
		DWORD m_dwRegisterDate2;

		DWORD m_dwRegisterPeriod;
		ioHashString m_szRegisterUserNick;

		TradeSyncInfo()
		{
			m_dwUserIndex = 0;
			m_dwTradeIndex = 0;
			m_dwItemType = 0;
			m_dwItemMagicCode = 0;
			m_dwItemValue = 0;

			m_dwItemMaleCustom = 0;
			m_dwItemFemaleCustom = 0;
			m_iItemPrice = 0;
			m_dwRegisterDate1 = 0;
			m_dwRegisterDate2 = 0;

			m_dwRegisterPeriod = 0;
			m_szRegisterUserNick = "";
		}
	};
	
 	typedef std::vector< TradeSyncInfo > vTradeSyncInfo;
	typedef std::map< DWORD, TradeSyncInfo > mapTradeSyncInfo;
	mapTradeSyncInfo m_mapTradeSyncInfo;	// 게임 서버 메모리 상에 들고 있는 아이템 리스트

public:
	TradeSyncManager();
	virtual ~TradeSyncManager();

	void Init();
	void Destroy();

public:
	// 클라 요청에 대한 응답
	void SendTradeItemList( User *pUser );

	// 메인서버로 부터 받는 거래소 아이템 전체 리스트
	void RecvAllTradeItem( SP2Packet& rkPacket );
	
	// 메인 서버로 부터 받은 거래소 아이템 추가 형태
	void RecvAddTradeItem( SP2Packet& rkPacket );

	// 메인 서버로 부터 받은 거래소 아이템 삭제 형태
	void RecvDelTradeItem( SP2Packet& rkPacket );

public:
	static TradeSyncManager& GetSingleton();
};

#define g_TradeSyncMgr TradeSyncManager::GetSingleton()

#endif