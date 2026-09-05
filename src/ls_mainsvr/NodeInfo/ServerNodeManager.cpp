#include "../stdafx.h"

#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../MainProcess.h"
#include "../ioProcessChecker.h"
#include "../ServerCloseManager.h"
#include "../Shutdown.h"

#include "EventGoodsManager.h"
#include "CampManager.h"
#include "GuildNodeManager.h"
#include "TradeNodeManager.h"
#include "ServerNodeManager.h"
#include "TournamentManager.h"
#include "MgrToolNodeManager.h"
#include "PracticeNode.h"
#include "PracticeNodeManager.h"
#include "../EtcHelpFunc.h"

extern CLog TradeLOG;
extern CLog MonitorLOG;
extern CLog WemadeLOG;
extern CLog OperatorLOG;

ServerNodeManager *ServerNodeManager::sg_Instance = NULL;

ServerNodeManager::ServerNodeManager() : m_dwCurrentTime(0), m_dwCheckServerToServerConnectTime(0)
{
	CreateServerIndexes();
}

ServerNodeManager::~ServerNodeManager()
{
	m_vServerNode.clear();
}

ServerNodeManager &ServerNodeManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new ServerNodeManager;

	return *sg_Instance;
}

void ServerNodeManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ServerNodeManager::InitMemoryPool()
{
	ioINILoader kLoader( "ls_config_main.ini" );
	kLoader.SetTitle( "Server Session Buffer" );
	int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
	int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
	kLoader.SetTitle( "MemoryPool" );
	int iMaxServer = kLoader.LoadInt( "server_pool", 10 );

	m_MemNode.CreatePool(0, MAX_SERVERNODE, FALSE);
	for(int i = 0;i < iMaxServer;i++)
	{
		m_MemNode.Push( new ServerNode( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize ) );
	}
}

void ServerNodeManager::ReleaseMemoryPool()
{
	vServerNode_iter iter, iEnd;
	iEnd = m_vServerNode.end();
	for(iter = m_vServerNode.begin();iter != iEnd;++iter)
	{
		ServerNode *pServerNode = *iter;
		pServerNode->OnDestroy();
		m_MemNode.Push( pServerNode );
	}	
	m_vServerNode.clear();
	m_MemNode.DestroyPool();

	for(sToolServerInfo::iterator iter = m_sToolServerInfo.begin(); iter != m_sToolServerInfo.end(); ++iter)
	{
		ToolServerInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		SAFEDELETE( pInfo );
	}
	m_sToolServerInfo.clear();
}

void ServerNodeManager::CreateServerIndexes()
{
	for(DWORD dwServerIndex = 1 ; dwServerIndex <=  MAX_SERVERNODE ; dwServerIndex++)
	{
		m_vServerIndexes.push_back( dwServerIndex );
	}
}

ServerNode *ServerNodeManager::CreateServerNode(SOCKET s)
{
	ServerNode *newNode = m_MemNode.Remove();
	if( !newNode )
	{
		LOG.PrintTimeAndLog(0,"ServerNodeManager::CreateServerNode MemPool Zero!");
		return NULL;
	}

	newNode->SetSocket(s);
	newNode->OnCreate();

	// 아이템 가격 정보 로드
	g_DBClient.OnSelectItemBuyCnt();
	return newNode;
}

BOOL ServerNodeManager::CreateServerIndex(DWORD& dwServerIndex)
{
	dwServerIndex = 0;
	if(m_vServerIndexes.size() > 0)
	{
		dwServerIndex = m_vServerIndexes.front();
		m_vServerIndexes.pop_front();

		return TRUE;
	}
	return FALSE;
}

void ServerNodeManager::DeleteServerIndex(DWORD dwServerIndex)
{
	if( 0 != dwServerIndex )
	{
		// 메인서버 재시작시 인덱스가 초기화 되므로 이미 있는 인덱스인지 확인후 추가한다
		SERVERINDEXES::iterator it = std::find( m_vServerIndexes.begin(), m_vServerIndexes.end(),  dwServerIndex );
		if( it != m_vServerIndexes.end() )
		{
			m_vServerIndexes.erase( it );
		}
	}
}

void ServerNodeManager::DestroyServerIndex(const DWORD dwServerIndex)
{
	if( 0 != dwServerIndex )
	{
		// 메인서버 재시작시 인덱스가 초기화 되므로 이미 있는 인덱스인지 확인후 추가한다
		SERVERINDEXES::iterator it = std::find( m_vServerIndexes.begin(), m_vServerIndexes.end(),  dwServerIndex );
		if( it == m_vServerIndexes.end() )
		{
			m_vServerIndexes.push_front( dwServerIndex );
		}
	}
}

void ServerNodeManager::AddServerNode( ServerNode *pNewNode )
{
	m_vServerNode.push_back( pNewNode );

	// 게임 서버가 켜질 시 거래소 아이템을 동기화 해준다.
	g_TradeNodeManager.SendAllTradeItem();
}

void ServerNodeManager::RemoveNode( ServerNode *pServerNode )
{
	DWORD dwServerIndex = pServerNode->GetServerIndex();

	WemadeLOG.PrintTimeAndLog( 0, "Server_Node : Disconnect: (%d-%s[%s])", pServerNode->GetSocket(), pServerNode->GetPrivateIP().c_str(), pServerNode->GetPublicIP().c_str() );
	vServerNode_iter iter = std::find( m_vServerNode.begin(), m_vServerNode.end(), pServerNode );
	if( iter != m_vServerNode.end() )
	{
		ServerNode *pServerNode = *iter;
		pServerNode->OnDestroy();
		m_vServerNode.erase( iter );
		m_MemNode.Push( pServerNode );

		DestroyServerIndex( dwServerIndex );
	}	

	if( m_vServerNode.empty() )
	{
		if( g_ServerCloseMgr.IsServerClose() )
		{
			FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김
			//모든 서버와 연결이 종료되었다.
			g_MainProc.Shutdown( SHUTDOWN_QUICK );
		}
	}
}

ServerNode *ServerNodeManager::GetServerNode()
{
	if( m_vServerNode.empty() ) return NULL;

	int iSize = m_vServerNode.size();
	int iRand = rand() % iSize;

	if( COMPARE( iRand, 0, iSize ) )
	{
		return m_vServerNode[iRand];
	}

	return NULL;
}

ServerNode *ServerNodeManager::GetServerNode( DWORD dwServerIndex )
{
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;
	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		if( item->GetServerIndex() == dwServerIndex )
		{
			return item;
		}		
	}
	return NULL;
}

void ServerNodeManager::ProcessServerNode()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( m_dwCurrentTime == 0 )
		m_dwCurrentTime = TIMEGETTIME();

	if( TIMEGETTIME() - m_dwCurrentTime < 20000 ) return;

	m_dwCurrentTime = TIMEGETTIME();
	
	// 서버에게 핑 전송 (20초 ~ 40초마다 전송)
	CheckServerPing();

	// 서버간 연결이 종료된 서버를 찾아서 종료시킨다. (1분 ~ 1분 10초마다 체크)
	CheckServerToServerConnect();
}

void ServerNodeManager::ServerNode_SendBufferFlush()
{
	if( m_vServerNode.empty() == false )
	{
		vector< ServerNode* >::iterator	iter	= m_vServerNode.begin();
		vector< ServerNode* >::iterator	iterEnd	= m_vServerNode.end();

		for( iter ; iter != iterEnd ; ++iter )
		{
			ServerNode* pServerNode = (*iter);

			if( ! pServerNode->IsActive() )
				continue;
			if( pServerNode->GetSocket() == INVALID_SOCKET )
				continue;

			pServerNode->FlushSendBuffer();
		}
	}
}

DWORD ServerNodeManager::GetMaxUserCount()
{
	DWORD dwUserCount = 0;
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;
	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		dwUserCount += item->GetUserCount();	
	}
	return dwUserCount;
}

DWORD ServerNodeManager::GetMaxUserCountByChannelingType( ChannelingType eChannelingType )
{
	DWORD dwUserCount = 0;
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;
	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		dwUserCount += item->GetUserCountByChannelingType( eChannelingType );	
	}
	return dwUserCount;
}

int ServerNodeManager::GetServerConnectCount( DWORD dwServerIndex )
{
	int iReturnCount = 0;
	vServerNode_iter iter, iEnd;	
	iEnd = m_vServerNode.end();
	for(iter = m_vServerNode.begin();iter != iEnd;++iter)
	{
		ServerNode *pServerNode = *iter;
		if( pServerNode->IsConnectServerIndex( dwServerIndex ) )
			iReturnCount++;
	}	
	return iReturnCount;
}

void ServerNodeManager::CheckServerToServerConnect()
{
	if( m_dwCheckServerToServerConnectTime == 0 )
		m_dwCheckServerToServerConnectTime = TIMEGETTIME();

	if( TIMEGETTIME() - m_dwCheckServerToServerConnectTime < 60000 )
		return;

	m_dwCheckServerToServerConnectTime = TIMEGETTIME();

	vSortServerNode vSortNode;
	vServerNode_iter iter, iEnd;	
	iEnd = m_vServerNode.end();
	for(iter = m_vServerNode.begin();iter != iEnd;++iter)
	{
		SortServerNode kNode;
		kNode.m_pNode = *iter;
		if( kNode.m_pNode == NULL ) continue;

		kNode.m_iSortPoint = GetServerConnectCount( kNode.m_pNode->GetServerIndex() );
		vSortNode.push_back( kNode );
	}	

	if( vSortNode.empty() )
	{
		LOG.PrintTimeAndLog( 0, "There are no connected server" );
		return;
	}

	std::sort( vSortNode.begin(), vSortNode.end(), ServerNodeSort() );
	
	SortServerNode kLowConnectNode = vSortNode[0];
	if( kLowConnectNode.m_iSortPoint >= (int)m_vServerNode.size() )
	{
		int iSize = vSortNode.size();
		for(int i = 0;i < iSize;i++)
		{
			ServerNode *pNode = vSortNode[i].m_pNode;
			if( pNode )
				pNode->InitDisconnectCheckCount();
		}
		LOG.PrintTimeAndLog( 0, "%d servers are connected", kLowConnectNode.m_iSortPoint );
	}
	else
	{
		ServerNode *pDisconnectNode = kLowConnectNode.m_pNode;
		if( pDisconnectNode )
		{
			if( pDisconnectNode->IsDisconnectCheckOver() )
			{
				LOG.PrintTimeAndLog( 0, "서버간 연결이 종료된 서버 종료 신호 전송(%d) : %s:%d", kLowConnectNode.m_iSortPoint, pDisconnectNode->GetPrivateIP().c_str(), pDisconnectNode->GetClientPort() );
#if defined ( SRC_OVERSEAS ) || defined ( SRC_NA ) || defined ( SRC_TH ) || defined( SRC_LATIN ) ||  defined( SRC_TW ) ||  defined ( SRC_ID )  || defined ( SRC_SEA )
				/*SP2Packet kPacket( MSTPK_LOW_CONNECT_SERVER_EXIT );
				kPacket << kLowConnectNode.m_iSortPoint;
				pDisconnectNode->SendMessage( kPacket );*/
#else
				SP2Packet kPacket( MSTPK_LOW_CONNECT_SERVER_EXIT );
				kPacket << kLowConnectNode.m_iSortPoint;
				pDisconnectNode->SendMessage( kPacket );
#endif 
			}		
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "CheckServerToServerConnect NULL ServerNode" );
		}
	}
}

void ServerNodeManager::CheckServerPing()
{
	vServerNode_iter iter, iEnd;	
	iEnd = m_vServerNode.end();
	for(iter = m_vServerNode.begin();iter != iEnd;++iter)
	{
		ServerNode *pServer = *iter;
		if( pServer == NULL ) continue;

		pServer->SendPingCheck();
	}	
}

void ServerNodeManager::FillServerInfo( SP2Packet &rkPacket )
{
	rkPacket << (int)m_vServerNode.size() + 1;
	vServerNode_iter iter, iEnd;	
	iEnd = m_vServerNode.end();
	for(iter = m_vServerNode.begin();iter != iEnd;++iter)
	{
		ServerNode *pServerNode = *iter;
		pServerNode->FillServerInfo( rkPacket );
	}

	// 추가. (MainServer)
	FillMainServerInfo( rkPacket );
}

void ServerNodeManager::FillMainServerInfo( SP2Packet& rkPacket )
{
	// Main서버 정보
	rkPacket << g_MainProc.GetPublicIP() << g_MainProc.GetPort()
			<< g_ServerNodeManager.GetNodeSize()
			<< 0 << 0 << 0 << 0 << 0 << 0;
}

void ServerNodeManager::FillTotalServerUserPos( SP2Packet &rkPacket )
{
	DWORD dwHeadquartersUserCount = 0;		//본부 유저
	DWORD dwSafetySurvivalRoomUserCount = 0;//초보서바이벌 훈련 유저
	DWORD dwPlazaUserCount = 0;				//광장 유저
	DWORD dwBattleRoomUserCount = 0;		//전투 유저
	DWORD dwBattleRoomPlayUserCount = 0;	//전투 플레이 유저
	DWORD dwLadderBattlePlayUserCount= 0;    //길드전 유저
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;
	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		dwHeadquartersUserCount += item->GetUserCount();	
		dwSafetySurvivalRoomUserCount += item->GetSafetySurvivalRoomUserCount();
		dwPlazaUserCount += item->GetPlazaUserCount();
		dwBattleRoomUserCount += item->GetBattleRoomUserCount();
		dwBattleRoomPlayUserCount += item->GetBattleRoomPlayUserCount();
		dwLadderBattlePlayUserCount += item->GetLadderBattlePlayUserCount();
	}
	// 최대 접속 인원 - (룸유저+광장유저+전투플레이유저)
	if( dwHeadquartersUserCount >= (dwPlazaUserCount + dwBattleRoomPlayUserCount) )
		dwHeadquartersUserCount = dwHeadquartersUserCount - (dwPlazaUserCount + dwBattleRoomPlayUserCount);
	else
		dwHeadquartersUserCount = 0;
	rkPacket << dwHeadquartersUserCount << dwSafetySurvivalRoomUserCount << dwPlazaUserCount << dwBattleRoomUserCount << dwLadderBattlePlayUserCount;
}

bool ServerNodeManager::InsertToolServerInfo( ToolServerInfo *pInfo )
{
	sToolServerInfo::iterator iter = m_sToolServerInfo.find( pInfo );
	if( iter != m_sToolServerInfo.end() )
		return false;

	m_sToolServerInfo.insert( pInfo );

	return true;
}

void ServerNodeManager::FillToolServerInfo( SP2Packet &rkPacket )
{
	rkPacket << (int)m_sToolServerInfo.size();
	for(sToolServerInfo::iterator iter = m_sToolServerInfo.begin(); iter != m_sToolServerInfo.end(); ++iter)
	{
		ToolServerInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		rkPacket << pInfo->m_szPublicIP;
		rkPacket << pInfo->m_szPrivateIP;
		rkPacket << pInfo->m_iClientPort;
	}

	// main 서버
	rkPacket << g_MainProc.GetPublicIP() << g_MainProc.GetPort();
}

void ServerNodeManager::SendMessageAllNode( SP2Packet &rkPacket, const DWORD dwServerIndex )
{
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;
	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		if( item->GetServerIndex() != dwServerIndex)
			item->SendMessage( rkPacket );
	}
}

bool ServerNodeManager::SendMessageNode( int iServerIndex, SP2Packet &rkPacket )
{
	ServerNode *pNode = GetServerNode( iServerIndex );
	if( pNode )
		return pNode->SendMessage( rkPacket );
	return false;
}

bool ServerNodeManager::SendMessageArray( int iServerArray, SP2Packet &rkPacket )
{
	if( !COMPARE( iServerArray, 0, (int)m_vServerNode.size() ) )
		return false;

    vServerNode_iter iter = m_vServerNode.begin() + iServerArray;
	ServerNode *pNode = *iter;
	if( !pNode )
		return false;
	
	return pNode->SendMessage( rkPacket );
}

bool ServerNodeManager::SendMessageIP( ioHashString &rszIP, SP2Packet &rkPacket )
{
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;
	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		if( item->GetPrivateIP() == rszIP )
		{
			return item->SendMessage( rkPacket );
		}		
	}
	return false;
}

void ServerNodeManager::SendMessage( SP2Packet &packet )
{
	if(m_vServerNode.size() > 0)
	{
		int index = rand() % m_vServerNode.size();
		ServerNode* serverNode = m_vServerNode[index];
		if(serverNode)
		{
			serverNode->SendMessage( packet );
		}
	}
}

bool ServerNodeManager::SendMessageIPnPort( ioHashString& rszIP, int port, SP2Packet& rkPacket )
{
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;

	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		if( item->GetPrivateIP() == rszIP && item->GetClientPort() == port )
		{
			return item->SendMessage( rkPacket );
		}		
	}
	return false;
}

void ServerNodeManager::SendServerList( ServerNode *pServerNode )
{
	if( pServerNode == NULL ) return;

	const int iSendListSize = 40;
	int iMaxServerCount = (int)m_vServerNode.size();                    
	for(int iStartArray = 0;iStartArray < iMaxServerCount;)
	{
		int iLoop = iStartArray;
		int iSendSize = min( iMaxServerCount - iStartArray, iSendListSize );
		SP2Packet kPacket( MSTPK_ALL_SERVER_LIST );
		kPacket << iSendSize;	
		for(;iLoop < iStartArray + iSendSize;iLoop++)
		{
			ServerNode *pServer = m_vServerNode[iLoop];
			kPacket << pServer->GetServerIndex();
			kPacket << pServer->GetPrivateIP();
			kPacket << pServer->GetServerPort();
		}		
		// 패킷 종료
		kPacket << (bool)(iLoop >= iMaxServerCount);
		pServerNode->SendMessage( kPacket );		

		iStartArray = iLoop;
	}	
}

bool ServerNodeManager::GlobalQueryParse( SP2Packet &rkPacket )
{
	CQueryResultData query_data;
	rkPacket >> query_data;

	g_PacketChecker.QueryPacket( query_data.GetMsgType() );	
	
	FUNCTION_TIME_CHECKER( 100000.0f, query_data.GetMsgType() );          // 0.1 초 이상 걸리면로그 남김

	switch( query_data.GetMsgType() )
	{
	case DBAGENT_GAME_PINGPONG:
		OnResultPingPong( &query_data );
		return true;
	case DBAGENT_LOG_PINGPONG:
		return true;
	case DBAGENT_SERVER_INFO_SET:
		OnResultInsertGameServerInfo( &query_data );
		return true;
	case DBAGENT_ITEM_BUYCNT_SET:
		OnResultSelectItemBuyCnt( &query_data );
		return true;
	case DBAGENT_TOTAL_REG_USER_SET:
		OnResultSelectTotalRegUser( &query_data );
		return true;
	case DBAGENT_GUILD_INFO_GET:
		OnResultSelectGuildInfoList( &query_data );
		return true;
	case DBAGENT_CAMP_DATA_GET:
		OnResultSelectCampData( &query_data );
		return true;
	case DBAGENT_CAMP_SPECIAL_USER_COUNT_GET:
		OnResultSelectCampSpecialUserCount( &query_data );
		return true;
	case DBAGENT_TRADE_INFO_GET:
		OnResultSelectTradeInfoList( &query_data );
		return true;
	case DBAGENT_TRADE_DELETE:
		OnResultTradeItemDelete( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_DATA_GET:
		OnResultSelectTournamentData( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_CUSTOM_DATA_GET:
		OnResultSelectTournamentCustomData( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_TEAM_LIST_GET:
		OnResultSelectTournamentTeamList( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_WINNER_HISTORY_SET:
		OnResultInsertTournamentWinnerHistory( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_CUSTOM_INFO_GET:
		OnResultSelectTournamentCustomInfo( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_CUSTOM_ROUND_GET:
		OnResultSelectTournamentCustomRound( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_CONFIRM_USER_LIST:
		OnResultSelectTournamentConfirmUserList( &query_data );
		return true;
	case DBAGENT_TOURNAMENT_PREV_CAMP_INFO_GET:
		OnResultSelectTournamentPrevChampInfo( &query_data );
		return true;
	case DBAGENT_USERBLOCK_SET:
		OnResultInsertUserBlock( &query_data );
		return true;
	case DBAGENT_CHANGE_USERBLOCK_SET:
		OnResultInsertChangeUserBlock( &query_data );
		return true;
	case DBAGENT_EVENTSHOP_BUYCOUNT_GET:
		OnResultSelectGoodsBuyCount( &query_data );
		return true;
	case DBAGENT_EVENTSHOP_BUYCOUNT_SET:
		OnResultUpdateGoodsBuyCount( &query_data );
		return true;
	case DBAGENT_EVENTSHOP_BUYCOUNT_DEL:
		return true;

	case DBAGENT_PRACTICE_BLOCKUSER_LIST:
		OnResultPractice_BlockUserList( &query_data );
		return true;

	case DBAGENT_PRACTICE_TOTAL_INFO_GET:
		OnResultSelectPracticeInfoList( &query_data );
		return true;
	case DBAGENT_PRACTICE_INDEX_RANK_PRESENT:
		OnResultPractice_Index_RankPresent( &query_data );
		return true;
	}

	return false;
}

void ServerNodeManager::OnResultPingPong( CQueryResultData *query_data )
{
	DWORD dwLastPing;
	if(!query_data->GetValue( dwLastPing ))	return;

	DWORD dwElapse = (TIMEGETTIME() - dwLastPing) / 2;
	Debug("DB Ping : %lu ms\n", dwElapse );
}

void ServerNodeManager::OnResultInsertGameServerInfo( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultInsertGameServerInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}	
}

void ServerNodeManager::OnResultSelectItemBuyCnt( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectItemBuyCnt Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	SP2Packet kPacket( MSTPK_CLASS_PRICE_INFO );
	kPacket << *query_data;
	SendMessageAllNode( kPacket );
}

void ServerNodeManager::OnResultSelectTotalRegUser( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectTotalRegUser Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	
	bool bServerDown;
	if(!query_data->GetValue( bServerDown ))	return;

	int iTotalRegUser;
	if(!query_data->GetValue( iTotalRegUser ))	return;

	g_MainProc.SetTotalRegUser( iTotalRegUser );
	LOG.PrintTimeAndLog( 0, "OnResultSelectTotalRegUser : %d - ServerDown(%d)", iTotalRegUser, (int)bServerDown );

	if( !bServerDown )
	{
		SP2Packet kPacket( MSTPK_TOTAL_REG_USER_CNT );
		kPacket << iTotalRegUser;
		SendMessageAllNode( kPacket );
	}	
}

void ServerNodeManager::OnResultSelectGuildInfoList( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectGuildInfoList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int   iCount = 0;
	DWORD dwLastIndex = GUILD_LOAD_START_INDEX;
	while( query_data->IsExist() )
	{
		GuildNode *pGuildNode = g_GuildNodeManager.CreateGuildNode( query_data );
		if( pGuildNode )
		{
			dwLastIndex = pGuildNode->GetGuildIndex();
		}
		iCount++;
	}

	if( iCount >= GUILD_MAX_LOAD_LIST )
	{
		g_DBClient.OnSelectGuildInfoList( dwLastIndex, GUILD_MAX_LOAD_LIST );
	}	
	else
	{
		g_GuildNodeManager.SortGuildRankAll();
	}
}

void ServerNodeManager::OnResultSelectCampData( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectCampData Result FAILED! :%d",query_data->GetResultType());
		return;
	}
	
	int iBlueCampPoint = 0 ,iBlueCampTodayPoint = 0, iBlueCampBonusPoint = 0;
	int iRedCampPoint = 0 ,iRedCampTodayPoint = 0, iRedCampBonusPoint = 0;
	
	if( query_data->GetResultCount() > 0 )
	{
		if(!query_data->GetValue( iBlueCampPoint ))			return;
		if(!query_data->GetValue( iBlueCampTodayPoint ))	return;
		if(!query_data->GetValue( iBlueCampBonusPoint ))	return;
		if(!query_data->GetValue( iRedCampPoint ))			return;
		if(!query_data->GetValue( iRedCampTodayPoint ))		return;
		if(!query_data->GetValue( iRedCampBonusPoint ))		return;
	}

	g_CampMgr.DBToCampData( iBlueCampPoint, iBlueCampTodayPoint, iBlueCampBonusPoint,
							iRedCampPoint, iRedCampTodayPoint, iRedCampBonusPoint );
}

void ServerNodeManager::OnResultSelectCampSpecialUserCount( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectCampSpecialUserCount Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int iCampType = 0, iCampSpecialEntry = 0;
	if(!query_data->GetValue( iCampType ))			return;
	if(!query_data->GetValue( iCampSpecialEntry ))	return;

	g_CampMgr.DBToCampSpecialUserCount( iCampType, iCampSpecialEntry );
}

void ServerNodeManager::OnResultSelectTradeInfoList( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectTradeInfoList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int   iCount = 0;
	DWORD dwLastIndex = TRADE_START_LAST_INDEX;
	while( query_data->IsExist() )
	{
		TradeNode *pTradeNode = g_TradeNodeManager.CreateTradeNode( query_data );
		if( pTradeNode )
		{
			dwLastIndex = pTradeNode->GetTradeIndex();
		}
		iCount++;
	}

	if( iCount >= TRADE_MAX_LOAD_LIST )
	{
		g_DBClient.OnSelectTradeItemInfo( dwLastIndex, TRADE_MAX_LOAD_LIST );
	}	

	// DB 에서 처음 읽고 나서 동기화를 시켜준다.
	g_TradeNodeManager.SendAllTradeItem();
}

void ServerNodeManager::OnResultTradeItemDelete( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultTradeItemDelete Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwTradeIndex=0, dwServerIndex=0;
	if(!query_data->GetValue( dwTradeIndex ))	return;
	if(!query_data->GetValue( dwServerIndex ))	return;

	g_TradeNodeManager.SendTimeOutItemInfo( dwTradeIndex, dwServerIndex );
}

void ServerNodeManager::OnResultSelectTournamentData( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectTournamentData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int   iCount = 0;
	DWORD dwLastIndex = TOURNAMENT_LOAD_START_INDEX;
	while( query_data->IsExist() )
	{
		TournamentNode *pNode = g_TournamentManager.CreateTournamentNode( query_data );
		if( pNode )
		{
			dwLastIndex = pNode->GetIndex();			
		}
		iCount++;
	}

	if( iCount >= TOURNAMENT_MAX_LOAD_LIST )
	{
		g_DBClient.OnSelectTournamentData( dwLastIndex, TOURNAMENT_MAX_LOAD_LIST );
	}	
	else
	{
		g_TournamentManager.CreateCompleteSort();

		// 토너먼트 팀 정보
		g_DBClient.OnSelectTournamentTeamList( TOURNAMENT_LOAD_START_INDEX, TOURNAMENT_TEAM_MAX_LOAD_LIST );
	}
}

void ServerNodeManager::OnResultSelectTournamentCustomData( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectTournamentCustomData Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwTourIndex=0, dwUserIndex=0, dwServerIndex=0;
	if(!query_data->GetValue( dwTourIndex ))		return;
	if(!query_data->GetValue( dwUserIndex ))		return;
	if(!query_data->GetValue( dwServerIndex ))		return;

	TournamentNode *pNode = g_TournamentManager.CreateTournamentNode( query_data );
	if( pNode )
	{
		if( pNode->GetIndex() == dwTourIndex )
		{
			//
			SP2Packet kPacket( MSTPK_TOURNAMENT_CUSTOM_CREATE );
			kPacket << dwUserIndex << dwTourIndex;
			if( g_ServerNodeManager.SendMessageNode( dwServerIndex, kPacket ) == false )
			{
				LOG.PrintTimeAndLog( 0, "OnResultSelectTournamentCustomData : None ServerNode : %d", dwServerIndex );
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "OnResultSelectTournamentCustomData : MissMatch Data : %d - %d", dwTourIndex, pNode->GetIndex() );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "OnResultSelectTournamentCustomData : None Data" );
	}
}

void ServerNodeManager::OnResultSelectTournamentTeamList( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectTournamentTeamList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	int   iCount = 0;
	DWORD dwLastIndex = TOURNAMENT_LOAD_START_INDEX;
	while( query_data->IsExist() )
	{
		TournamentTeamNode *pTeam = g_TournamentManager.CreateTournamentTeamNode( query_data );
		if( pTeam )
		{
			dwLastIndex = pTeam->GetTeamIndex();
		}
		iCount++;
	}

	if( iCount >= TOURNAMENT_TEAM_MAX_LOAD_LIST )
	{
		g_DBClient.OnSelectTournamentTeamList( dwLastIndex, TOURNAMENT_TEAM_MAX_LOAD_LIST );
	}	
	else
	{
		g_TournamentManager.CreateTeamCompleteRound();
	}
}

void ServerNodeManager::OnResultInsertTournamentWinnerHistory( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultInsertTournamentWinnerHistory Result FAILED! :%d",query_data->GetResultType());
		return;
	}	
}

void ServerNodeManager::OnResultSelectTournamentCustomInfo( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectTournamentCustomInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}	

	g_TournamentManager.ApplyTournamentInfoDB( query_data );
}

void ServerNodeManager::OnResultSelectTournamentCustomRound( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectTournamentCustomRound Result FAILED! :%d",query_data->GetResultType());
		return;
	}	

	g_TournamentManager.ApplyTournamentRoundDB( query_data );
}

void ServerNodeManager::OnResultSelectTournamentConfirmUserList( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectTournamentConfirmUserList Result FAILED! :%d",query_data->GetResultType());
		return;
	}	

	g_TournamentManager.ApplyTournamentConfirmUserListDB( query_data );
}

void ServerNodeManager::OnResultSelectTournamentPrevChampInfo( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectTournamentPrevChampInfo Result FAILED! :%d",query_data->GetResultType());
		return;
	}	

	g_TournamentManager.ApplyTournamentPrevChampInfoDB( query_data );
}

void ServerNodeManager::OnResultInsertUserBlock( CQueryResultData *query_data )
{
	if( FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultInsertUserBlock Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	g_MgrTool.ApplyUserBlockDB( query_data );
}

void ServerNodeManager::OnResultInsertChangeUserBlock( CQueryResultData *query_data )
{
	if( FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultInsertUserBlock Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	g_MgrTool.ApplyChangeUserBlockDB( query_data );
}

void ServerNodeManager::OnResultSelectGoodsBuyCount( CQueryResultData *query_data )
{
	if( FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectGoodsBuyCount Result FAILED! :%d", query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex=0, dwGoodsIndex=0;
	BYTE byType=0, byCount=0;

	// 새로운 아이템으로 추가
	int iCount = 0;
	while( query_data->IsExist() )
	{
		if(!query_data->GetValue( dwUserIndex ))		return;
		if(!query_data->GetValue( byType ))				return;
		if(!query_data->GetValue( dwGoodsIndex ))		return;
		if(!query_data->GetValue( byCount ))			return;

		g_EventGoodsMgr.ApplyUserBuyData(dwUserIndex, dwGoodsIndex, byCount);

		LOG.PrintTimeAndLog(0, "Loading eventGoods : type[%d] userindex[%d] goods[%d] count[%d]", byType, dwUserIndex, dwGoodsIndex, byCount);
		++iCount;
	}

	if(iCount == g_EventGoodsMgr.m_PagingSize )
	{
		g_DBClient.OnSelectGoodsBuyCount(g_EventGoodsMgr.m_PagingSize, ++g_EventGoodsMgr.m_Page);
	}
	else
	{
		LOG.PrintTimeAndLog(0, "Loading eventGoods completed : [%d]", g_EventGoodsMgr.m_Page);
	}
}

void ServerNodeManager::OnResultUpdateGoodsBuyCount( CQueryResultData *query_data )
{
	if( FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultUpdateGoodsBuyCount Result FAILED! :%d", query_data->GetResultType());
		return;
	}

}

void ServerNodeManager::OnResultSelectPracticeInfoList( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultSelectPracticeInfoList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;
	int iPracticeIDX = 0, iPracticeTime = 0;
	DBTIMESTAMP dts;

	int   iCount = 0;
	DWORD dwLastIndex = PRACTICE_LOAD_START_INDEX;
	char szUserNick[ID_NUM_PLUS_ONE] = "";
	while( query_data->IsExist() )
	{
		if(!query_data->GetValue( iPracticeIDX ))		return;
		if(!query_data->GetValue( dwUserIndex ))		return;
		if(!query_data->GetValue( szUserNick, ID_NUM_PLUS_ONE ))			break;
		if(!query_data->GetValue( iPracticeTime ))		return;
		if(!query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ))		return;
		
		CTime tUpdate_time = CTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
		CPracticeNode* pkPracticeNode = g_PracticeNodeManager.CreatePracticeNode(iPracticeIDX);
		if(pkPracticeNode == NULL)
		{
			continue;
		}
		pkPracticeNode->CreatePracticer(dwUserIndex, szUserNick, iPracticeTime, tUpdate_time);

		iCount++;
	}

	if( iCount >= PRACTICE_MAX_LOAD_LIST )
	{
		g_DBClient.OnSelectPracticeInfoList(dwUserIndex, iPracticeIDX, PRACTICE_MAX_LOAD_LIST );
	}	
	else
	{
		g_PracticeNodeManager.SortRankAll(true);
	}
}

void ServerNodeManager::OnResultPractice_Index_RankPresent( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultPractice_Index_RankPresent Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	g_PracticeNodeManager.ProcessState(CPracticeNodeManager::ST_DATA_INIT);
}

void ServerNodeManager::OnResultPractice_BlockUserList( CQueryResultData *query_data )
{
	if(FAILED(query_data->GetResultType()))
	{
		LOG.PrintTimeAndLog(0,"DB OnResultPractice_BlockUserList Result FAILED! :%d",query_data->GetResultType());
		return;
	}

	DWORD dwUserIndex = 0;

	int   iCount = 0;
	DWORD dwLastIndex = PRACTICE_LOAD_START_INDEX;
	char szUserNick[ID_NUM_PLUS_ONE] = "";
	while( query_data->IsExist() )
	{
		if(!query_data->GetValue( dwUserIndex ))		return;
		
		OperatorLOG.PrintTimeAndLog(0, "OnResultPractice_BlockUserList Kick - %d", dwUserIndex);
		g_PracticeNodeManager.BlockUserDelete(dwUserIndex);

		iCount++;
	}
	
	if( iCount > 0)
	{
		g_PracticeNodeManager.SortRankAll(false);
	}	
	OperatorLOG.PrintTimeAndLog(0, "OnResultPractice_BlockUserList Count - %d:%d", dwUserIndex, iCount);
}