#pragma once

#include "ServerNode.h"
#include <list>

using namespace std;

const int MAX_SERVERNODE = 512;

typedef struct tagSortServerNode
{
	ServerNode *m_pNode;
	int         m_iSortPoint;
	tagSortServerNode()
	{
		m_pNode  = NULL;
		m_iSortPoint = 0;
	}
}SortServerNode;
typedef vector< SortServerNode > vSortServerNode;
typedef vSortServerNode::iterator vSortServerNode_iter;

class ServerNodeSort
{
public:
	bool operator()( const SortServerNode &lhs , const SortServerNode &rhs ) const
	{
		if( lhs.m_iSortPoint < rhs.m_iSortPoint )
			return true;
		return false;
	}
};

typedef vector< ServerNode * > vServerNode;
typedef vServerNode::iterator vServerNode_iter;

// tool server info
typedef struct tagToolServerInfo
{
	ioHashString m_szPublicIP;
	ioHashString m_szPrivateIP;
	int          m_iClientPort;	

	tagToolServerInfo()
	{
		m_iClientPort = 0;
	}
}ToolServerInfo;

class ToolServerInfoCompare
{
public:
	bool operator()(const ToolServerInfo *lhs , const ToolServerInfo *rhs) const
	{
		int iCmpValue = _stricmp( lhs->m_szPrivateIP.c_str(), rhs->m_szPrivateIP.c_str() );
		if( iCmpValue < 0 ) 
			return true; 
		else if( iCmpValue == 0 ) 
		{ 
			if( lhs->m_iClientPort < rhs->m_iClientPort )
				return true; 
			else 
				return false; 
		} 
		else 
			return false;
	}
};

typedef set< ToolServerInfo*, ToolServerInfoCompare > sToolServerInfo;

//
class ServerNodeManager : public SuperParent
{
protected:
	static ServerNodeManager *sg_Instance;
	vServerNode	              m_vServerNode;
	MemPooler<ServerNode>	  m_MemNode;

	DWORD					  m_dwCurrentTime;
	DWORD                     m_dwCheckServerToServerConnectTime;
	sToolServerInfo           m_sToolServerInfo;

	typedef std::list<DWORD> SERVERINDEXES;
	SERVERINDEXES			  m_vServerIndexes;

public:
	static ServerNodeManager &GetInstance();
	static void ReleaseInstance();

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();

	void CreateServerIndexes();

public:
	ServerNode *CreateServerNode( SOCKET s );
	BOOL CreateServerIndex(DWORD& dwServerIndex);
	void DeleteServerIndex(DWORD dwServerIndex);
	void DestroyServerIndex(const DWORD dwServerIndex);

public:
	void AddServerNode( ServerNode *pNewNode );
	void RemoveNode( ServerNode *pServerNode );
	int RemainderNode()	{ return m_MemNode.GetCount(); }
	int GetNodeSize()	{ return m_vServerNode.size(); }
	DWORD GetMaxUserCount();
	DWORD GetMaxUserCountByChannelingType( ChannelingType eChannelingType );
	int GetServerConnectCount( DWORD dwServerIndex );
	
public:
	void CheckServerToServerConnect();
	void CheckServerPing();

public:
	ServerNode *GetServerNode();
	ServerNode *GetServerNode( DWORD dwServerIndex );

public:
	void ProcessServerNode();
	void ServerNode_SendBufferFlush();

public:
	void FillServerInfo( SP2Packet &rkPacket );
	void FillMainServerInfo( SP2Packet& rkPacket );
	void FillTotalServerUserPos( SP2Packet &rkPacket );
	bool InsertToolServerInfo( ToolServerInfo *pInfo );
	void FillToolServerInfo( SP2Packet &rkPacket );

public:
	void SendMessageAllNode( SP2Packet &rkPacket, const DWORD dwServerIndex = 0xFFFFFFFF );
	bool SendMessageNode( int iServerIndex, SP2Packet &rkPacket );
	bool SendMessageArray( int iServerArray, SP2Packet &rkPacket );
	bool SendMessageIP( ioHashString &rszIP, SP2Packet &rkPacket );
	bool SendMessageIPnPort( ioHashString& rszIP, int port, SP2Packet& rkPacket );
	void SendServerList( ServerNode *pServerNode );
	void SendMessage( SP2Packet &packet );

public:       //DB RESULT
	bool GlobalQueryParse( SP2Packet &rkPacket );

public:
	void OnResultPingPong( CQueryResultData *query_data );
	void OnResultInsertGameServerInfo( CQueryResultData *query_data );
	void OnResultSelectItemBuyCnt( CQueryResultData *query_data );
	void OnResultSelectTotalRegUser( CQueryResultData *query_data );
	void OnResultSelectGuildInfoList( CQueryResultData *query_data );
	
	void OnResultSelectCampData( CQueryResultData *query_data );
	void OnResultSelectCampSpecialUserCount( CQueryResultData *query_data );

	// Trade
	void OnResultSelectTradeInfoList( CQueryResultData *query_data );
	void OnResultTradeItemDelete( CQueryResultData *query_data );

	// Tournament
	void OnResultSelectTournamentData( CQueryResultData *query_data );
	void OnResultSelectTournamentCustomData( CQueryResultData *query_data );
	void OnResultSelectTournamentTeamList( CQueryResultData *query_data );
	void OnResultInsertTournamentWinnerHistory( CQueryResultData *query_data );
	void OnResultSelectTournamentCustomInfo( CQueryResultData *query_data );
	void OnResultSelectTournamentCustomRound( CQueryResultData *query_data );
	void OnResultSelectTournamentConfirmUserList( CQueryResultData *query_data );
	void OnResultSelectTournamentPrevChampInfo( CQueryResultData *query_data );

	void OnResultInsertUserBlock( CQueryResultData *query_data );
	void OnResultInsertChangeUserBlock( CQueryResultData *query_data );
	void OnResultSelectGoodsBuyCount( CQueryResultData *query_data );
	void OnResultUpdateGoodsBuyCount( CQueryResultData *query_data );

	void OnResultSelectPracticeInfoList( CQueryResultData *query_data );
	void OnResultPractice_Index_RankPresent( CQueryResultData *query_data );
	void OnResultPractice_BlockUserList( CQueryResultData *query_data );

private:     	/* Singleton Class */
	ServerNodeManager();
	virtual ~ServerNodeManager();
};

#define g_ServerNodeManager ServerNodeManager::GetInstance()