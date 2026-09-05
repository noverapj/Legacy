#ifndef _TradeNodeManager_h_
#define _TradeNodeManager_h_

#include "TradeNode.h"
using namespace std;

class ServerNode;

typedef vector< TradeNode * > vTradeNode;
typedef std::map<DWORD, TradeNode*> mapTradeNode;

enum { MAX_SYNC_ITEM = 200, };

class TradeSort
{
public:
	bool operator()( const TradeNode *lhs , const TradeNode *rhs ) const
	{
		if( lhs->GetRegisterDate1() > rhs->GetRegisterDate1() )
		{
			return true;
		}
		else if( lhs->GetRegisterDate1() == rhs->GetRegisterDate1() )
		{
			if( lhs->GetRegisterDate2() > rhs->GetRegisterDate2() ) 
				return true;
		}
		return false;
	}
};

class TradeNodeManager : public SuperParent
{
protected:
	static TradeNodeManager *sg_Instance;

protected:
	mapTradeNode m_mapTradeNode;
	mapTradeNode m_mapWaitTradeNode;
	mapTradeNode m_mapTimeWaitTradeNode;

	DWORD m_dwUpdateCheckTime;

public:
	static TradeNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	TradeNode *CreateTradeNode( SP2Packet &rkPacket );
	TradeNode *CreateTradeNode( CQueryResultData *pQueryData );

public:
	int GetNodeSize() { return m_mapTradeNode.size(); }

	bool RemoveTradeItem( DWORD dwTradeIndex );
	TradeNode *GetTradeNode( DWORD dwTradeIndex );

	bool RemoveWaitTradeItem( DWORD dwTradeIndex );
	TradeNode *GetWaitTradeNode( DWORD dwTradeIndex );

	bool RemoveTimeWaitTradeItem( DWORD dwTradeIndex );
	TradeNode *GetTimeWaitTradeNode( DWORD dwTradeIndex );

public:
	void SendCurList( ServerNode *pServerNode, SP2Packet &rkPacket );
	
	void OnGetTradeItemInfo( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex );
	void OnTradeItemFail( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex );
	void OnTradeItemDel( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex );

	void OnGetTradeCancelInfo( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex );
	void OnGetTradeCancelFail( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwServerIndex );

	void SendTimeOutItemInfo( DWORD dwTradeIndex, DWORD dwServerIndex );

	void ProcessUpdateTrade();

	// 메인서버가 처음 보내는 거래소 아이템 전체 리스트 패킷
	void SendAllTradeItem();

	// 메인서버가 보내는 거래소 아이템 추가 패킷
	void SendAddTradeItem( DWORD dwUseIndex, DWORD dwTradeIndex, DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue
		, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iItemPrice, DWORD dwRegisterDate1, DWORD dwRegisterDate2 
		, DWORD dwRegisterPeriod, ioHashString szRegistUserNick );

	// 메인서버가 보내는 거래소 아이템 삭제 패킷
	void SendDelTradeItem( DWORD dwTradeIndex );

private:     	/* Singleton Class */
	TradeNodeManager();
	virtual ~TradeNodeManager();
};

#define g_TradeNodeManager TradeNodeManager::GetInstance()

#endif
