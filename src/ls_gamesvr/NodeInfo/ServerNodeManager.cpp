#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../Network/iocpHandler.h"
#include "../DataBase/DBClient.h"
#include "../MainServerNode/MainServerNode.h"
#include "BattleRoomManager.h"
#include "RoomNodeManager.h"
#include "ShuffleRoomManager.h"
#include "UserNodeManager.h"
#include "ServerNode.h"
#include "ServerNodeManager.h"
#include <algorithm>

ServerNodeManager *ServerNodeManager::sg_Instance = NULL;

ServerNodeManager::ServerNodeManager() : m_dwServerIndex(0), m_iMaxNodes(0), m_iDisperseCount(5), m_dwServerRoomCreateClosePingTime( SERVER_ROOM_CREATE_CLOSE_TIME ), m_bBlockState(false), m_iPartitionIndex(0)
{
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
	SAFEDELETE(sg_Instance);
}

void ServerNodeManager::InitMemoryPool()
{
	ioINILoader kLoader( "ls_config_game.ini" );
	int iMaxServerNode = kLoader.LoadInt( "MemoryPool", "server_pool", 16 );
	int iSendBufferSize = kLoader.LoadInt( "Server Session", "SendBufferSize", MAX_BUFFER );	
		
	// MemPooler
	m_MemNode.CreatePool( 0, 256, FALSE );  // 2013-01-28 신영욱, 서버풀은 자동증가 되게 함
	for(int i = 0;i < iMaxServerNode;i++)
	{
		m_MemNode.Push( new ServerNode( INVALID_SOCKET, iSendBufferSize, MAX_BUFFER * 2 ) );
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][memorypool]Server session send buffer : [%d]", iSendBufferSize );

	LoadINI();
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
}

void ServerNodeManager::LoadINI()
{
	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_user_dispersion.ini" );
	
	kLoader.SetTitle( "info" );
	m_dwServerRoomCreateClosePingTime = kLoader.LoadInt( "room_create_close_ping_time", SERVER_ROOM_CREATE_CLOSE_TIME );
}

ServerNode *ServerNodeManager::CreateServerNode(SOCKET s)
{
	ServerNode *newNode = (ServerNode*)m_MemNode.Pop();
	if( !newNode )
	{
		LOG.PrintTimeAndLog(0,"ServerNodeManager::CreateServerNode MemPool Zero!");
		return NULL;
	}

	g_iocp.AddHandleToIOCP((HANDLE)s,(DWORD)newNode);

	newNode->SetSocket(s);
	newNode->OnCreate();
	newNode->SetNodeRole( ServerNode::NODE_ROLE_SERVER );
	newNode->RequestJoin();
	return newNode;
}

void ServerNodeManager::AddServerNode( ServerNode *pNewNode )
{
	m_vServerNode.push_back( pNewNode );
}

void ServerNodeManager::CalculateMaxNode()
{
	// 게임서버노드의 맥스값을 알아온다. 게임디비에이전트 분산에 사용하기 위함
	int iCount = 0;
	for(vServerNode::iterator it = m_vServerNode.begin() ; it != m_vServerNode.end() ; ++it)
	{
		ServerNode *pServerNode = *it;
		if(!pServerNode) continue;

		if( pServerNode->IsGameNode() )
		{
			++iCount;
		}
	}

	if(iCount > GetMaxServerNodes())
	{
		m_iMaxNodes = iCount;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][server]Max node count : [%d]", GetMaxServerNodes() );
}

int ServerNodeManager::CalculatePartition(const int iPartitionCount, const int iServerIndex)
{
	int iServerCount = m_iMaxNodes + 1;
	int iCountPerPartition = iServerCount / iPartitionCount;
	if((iServerCount % iPartitionCount) != 0 )
		++iCountPerPartition;

	int iIndex=0;
	int iSegment = iCountPerPartition;
	if( iSegment > iServerIndex)
		return 0;

	while(iSegment < iServerIndex)
	{
		++iIndex;
		if((iSegment+iCountPerPartition) > iServerIndex)
		{
			break;
		}
		iSegment += iCountPerPartition;
	}

	return iIndex;
}

void ServerNodeManager::CollectMyPartitions()
{
	m_vServerIndex.clear();

	// 같은 채널의 서버목록 수집
	for(vServerNode::iterator it = m_vServerNode.begin() ; it != m_vServerNode.end() ; ++it)
	{
		ServerNode *pServerNode = *it;
		if(!pServerNode || !pServerNode->IsGameNode()) continue;
		
		if(pServerNode->GetPartitionIndex() == GetPartitionIndex())
		{
			m_vServerIndex.push_back( pServerNode->GetServerIndex() );
		}
	}
}

bool ServerNodeManager::WasMyPartition( const int iServerIndex )
{
	for(vServerIndex::iterator it = m_vServerIndex.begin() ; it != m_vServerIndex.end() ; ++it)
	{
		if(*it == iServerIndex) return true;
	}
	return false;
}

void ServerNodeManager::RearrangePartition(const int iPartitionCount)
{
	Debug("- Rearrange Partition : %d\n", iPartitionCount);

	// 나와 같은 파티션서버 목록수집
	CollectMyPartitions();

	// 자신의 파티션변경
	int iPartitionIndex = CalculatePartition( iPartitionCount, GetServerIndex() );
	SetPartitionIndex( iPartitionIndex );
	Debug( "[%d] My, Partition Index : %d\n", GetServerIndex(), iPartitionIndex );

	// 다른 서버의 파티션변경
	for(vServerNode::iterator it = m_vServerNode.begin() ; it != m_vServerNode.end() ; ++it)
	{
		ServerNode *pServerNode = *it;
		if(!pServerNode || !pServerNode->IsGameNode()) continue;
		
		int iPartitionIndex = CalculatePartition(iPartitionCount, pServerNode->GetServerIndex());
		pServerNode->SetPartitionIndex( iPartitionIndex );
		Debug("[%d] Other, Partition Index : %d\n", pServerNode->GetServerIndex(), iPartitionIndex);
	}

	// 파티션간의 병합/분리 작업
	for(vServerNode::iterator it = m_vServerNode.begin() ; it != m_vServerNode.end() ; ++it)
	{
		ServerNode *pServerNode = *it;
		if(!pServerNode || !pServerNode->IsGameNode()) continue;

		if(pServerNode->GetPartitionIndex() != GetPartitionIndex())
		{
			// 다른 파티션에 존재하는 서버의 방정보 삭제
			pServerNode->RemoveBattleRoomCopyNodeAll();
			pServerNode->RemoveRoomCopyNodeAll();
		}
		else
		{
			if( !WasMyPartition(pServerNode->GetServerIndex()) )
			{
				// 방정보 동기화
				g_RoomNodeManager.ConnectServerNodeSync( pServerNode );
				g_BattleRoomManager.ConnectServerNodeSync( pServerNode );
			}
		}
	}
}

void ServerNodeManager::CreateClientNode( DWORD dwServerIndex, SOCKET s )
{
	ServerNode *newNode = (ServerNode*)m_MemNode.Pop();
	if( !newNode )
	{
		LOG.PrintTimeAndLog(0,"ServerNodeManager::CreateServerNode MemPool Zero!");
		return;
	}

	g_iocp.AddHandleToIOCP((HANDLE)s,(DWORD)newNode);

	AddServerNode( newNode );

	newNode->SetSocket(s);
	newNode->OnCreate();
	if(newNode->AfterCreate())
	{
		newNode->SetServerIndex( dwServerIndex );
		newNode->SetNodeRole( ServerNode::NODE_ROLE_CLIENT );
	}
	else
	{
		newNode->SessionClose(false);
	}
}

bool ServerNodeManager::ConnectTo( DWORD dwServerIndex, const char *ServerIP, int iSSPort )
{
	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNodeManager::ConnectTo socket %d[%s:%d]", GetLastError(), ServerIP, iSSPort );
		return false;
	}
	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( ServerIP );
	serv_addr.sin_port			= htons( iSSPort );

	LOOP_GUARD();
	int iConnectCount = 0;
	while( ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) ) != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ServerNodeManager::ConnectTo connect Failed(%d) : %d[%s:%d]", iConnectCount++, GetLastError(), ServerIP, iSSPort );
		if( iConnectCount >= 10 )
			return false;
		Sleep( 1000 );        // 1초뒤에 다시 시도
	}	
	LOOP_GUARD_CLEAR();

	CreateClientNode( dwServerIndex, socket );
	return true;
}

void ServerNodeManager::RemoveNode( ServerNode *pServerNode )
{
	for(vServerNode::iterator it = m_vServerNode.begin() ; it != m_vServerNode.end() ; ++it)
	{
		ServerNode* node  = *it;
		if(node && (node == pServerNode))
		{
			m_vServerNode.erase(it);
			m_MemNode.Push( node );
			break;
		}
	}
}

void ServerNodeManager::RemoveNode( const int iServerIndex )
{
	for(vServerNode::iterator it = m_vServerNode.begin() ; it != m_vServerNode.end() ; ++it)
	{
		ServerNode* node  = *it;
		if(node && (node->GetServerIndex() == iServerIndex))
		{
			m_vServerNode.erase(it);

			node->OnDestroy();
			m_MemNode.Push( node );
			break;
		}
	}
}
//
//int ServerNodeManager::ExceptionRemoveUserNode( DWORD dwIndex )
//{
//	int iReturnCnt = 0;
//	LOOP_GUARD();
//	vServerNode_iter iter = m_vServerNode.begin();
//	vServerNode_iter iter_Prev;
//	while( iter != m_vServerNode.end() )
//	{
//		iter_Prev = iter++;
//		ServerNode *pNode = *iter_Prev;
//		iReturnCnt += pNode->ExceptionRemoveUserNode( dwIndex );
//	}
//	LOOP_GUARD_CLEAR();
//	return iReturnCnt;
//}

bool ServerNodeManager::IsConnectWorkComplete()
{
	if( !m_vServerNode.empty() )
	{
		LOOP_GUARD();
		vServerNode_iter iter = m_vServerNode.begin();
		vServerNode_iter iter_Prev;
		while( iter != m_vServerNode.end() )
		{
			iter_Prev = iter++;
			ServerNode *item = *iter_Prev;
			if( !item->IsConnectState() ) continue;

			if( !item->IsConnectWorkComplete() )
				return false;
		}
		LOOP_GUARD_CLEAR();
	}
	g_DBClient.OnUpdateServerOn( g_App.GetGameServerID() );
	return true;
}

void ServerNodeManager::SetServerIndex( DWORD dwServerIndex )
{
	m_dwServerIndex = dwServerIndex;
}

void ServerNodeManager::SetBlockState(const bool bBlockState)
{
	m_bBlockState = bBlockState;
	g_BattleRoomManager.SetBlockFlag( bBlockState );
	g_ShuffleRoomManager.SetBlockFlag( bBlockState );
	g_RoomNodeManager.SetBlockFlag( bBlockState );
}

void ServerNodeManager::SetPartitionIndex(const int iPartitionIndex)
{
	m_iPartitionIndex = iPartitionIndex;
}

bool ServerNodeManager::IsAfford()
{
	if( IsBlocked() ) return false;
	if( !g_RoomNodeManager.IsAfford() ) return false;
	if( !g_UserNodeManager.IsAfford() ) return false;

	return true;
}

void ServerNodeManager::GetServerNodes(bool bAll, vServerIndex& vServerIndexes)
{
	vServerIndexes.clear();

	for(vServerNode_iter it = m_vServerNode.begin() ; it != m_vServerNode.end() ; ++it)
	{
		ServerNode* pServerNode = *it;
		if(!pServerNode || !pServerNode->IsGameNode()) continue;

		if(bAll)
		{
			vServerIndexes.push_back( pServerNode->GetServerIndex() );
		}
		else
		{
			if(!pServerNode->IsBlocked() && (pServerNode->GetServerIndex() != 0))
			{
				vServerIndexes.push_back( pServerNode->GetServerIndex() );
			}
		}
	}
}

ServerNode *ServerNodeManager::GetServerNode( DWORD dwServerIndex )
{
	LOOP_GUARD();
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
	LOOP_GUARD_CLEAR();
	return NULL;
}

ServerNode *ServerNodeManager::GetUserIndexToServerNode( DWORD dwUserIndex )
{
	LOOP_GUARD();
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;
	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		if( item->GetUserNode( dwUserIndex ) )
		{
			return item;
		}		
	}
	LOOP_GUARD_CLEAR();
	return NULL;
}


typedef pair<int, int>			COUNTERSET;
typedef std::vector<COUNTERSET> COUNTERS;

// 정렬함수
bool CounterSort(const COUNTERSET& lhs, const COUNTERSET& rhs) 
{ 
	return lhs.second < rhs.second; 
} 

int GetSortedServerIndex(COUNTERS& Counters, const int iDisperseCount)
{
	if( (int)Counters.size() > iDisperseCount)
	{
		// 정렬
		std::sort( Counters.begin(), Counters.end(), CounterSort );
	
		for(COUNTERS::iterator it =  Counters.begin() ; it != Counters.end() ; ++it)
		{
			//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "@ 서버노드 정렬 : %d, %d", (*it).first, (*it).second );
		}
	}
	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "@ Disperse Count : %d", iDisperseCount );

	// 분산개수에서 랜덤으로 선택
	int iIndex = rand() % iDisperseCount;
	COUNTERSET CounterSet = Counters[iIndex];

	int iServerIndex = CounterSet.first;
	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "-> 서버노드 선택 : %d, %d", CounterSet.first, CounterSet.second );
	return (iServerIndex);
}

bool ServerNodeManager::GetSelectPlazaServer( int iPlazaCount, DWORD& dwServerIndex )
{
	COUNTERS Counters;
	if( IsAfford() )
	{
		Counters.push_back( make_pair(0, iPlazaCount) );
	}

	for(vServerNode_iter iter = m_vServerNode.begin() ; iter != m_vServerNode.end() ; ++iter)
	{
		ServerNode *pServerNode = *iter;
		if( !pServerNode || !pServerNode->IsGameNode() || pServerNode->IsBusy() ) continue;

		Counters.push_back( make_pair(pServerNode->GetServerIndex(), pServerNode->GetPlazaRoomCount()) );
	}

	if(Counters.size() > 0)
	{
		int iMinimum		= ((int)Counters.size() < GetDisperseCount()) ? Counters.size() : GetDisperseCount();
		int iDisperseCount	= Counters.size() * 0.1;
		iDisperseCount		= (iDisperseCount < iMinimum) ? iMinimum : iDisperseCount;

		dwServerIndex = GetSortedServerIndex( Counters, iDisperseCount );
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "@ 디버그_광장 생성, ServerIndex : %lu", dwServerIndex );
		return true;
	}
	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "@ 디버그_광장 생성, Failed" );
	return false;
}

bool ServerNodeManager::GetSelectBattleRoomServer( int iBattleRoomCount, DWORD& dwServerIndex )
{
	COUNTERS Counters;
	if( IsAfford() )
	{
		Counters.push_back( make_pair(0, iBattleRoomCount) );
	}

	for(vServerNode_iter iter = m_vServerNode.begin() ; iter != m_vServerNode.end() ; ++iter)
	{
		ServerNode *pServerNode = *iter;
		if( !pServerNode || !pServerNode->IsGameNode() || pServerNode->IsBusy() ) continue;

		Counters.push_back( make_pair(pServerNode->GetServerIndex(), pServerNode->GetBattleRoomCount()) );
	}

	if(Counters.size() > 0)
	{
		int iMinimum		= ((int)Counters.size() < GetDisperseCount()) ? Counters.size() : GetDisperseCount();
		int iDisperseCount	= Counters.size() * 0.1;
		iDisperseCount		= (iDisperseCount < iMinimum) ? iMinimum : iDisperseCount;

		dwServerIndex = GetSortedServerIndex( Counters, iDisperseCount );
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "@ 디버그_전투방 생성, ServerIndex : %lu", dwServerIndex );
		return true;
	}

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "@ 디버그_전투방 생성, Failed" );
	return false;
}

bool ServerNodeManager::GetSelectLadderTeamServer( int iLadderTeamCount, bool bHeroMode, DWORD& dwServerIndex )
{
	COUNTERS Counters;
	if( IsAfford() )
	{
		Counters.push_back( make_pair(0, iLadderTeamCount) );
	}

	for(vServerNode_iter iter = m_vServerNode.begin() ; iter != m_vServerNode.end() ; ++iter)
	{
		ServerNode *pServerNode = *iter;
		if( !pServerNode || !pServerNode->IsGameNode() || pServerNode->IsBusy() ) continue;

		Counters.push_back( make_pair(pServerNode->GetServerIndex(), pServerNode->GetLadderTeamCount(bHeroMode)) );
	}

	if(Counters.size() > 0)
	{
		int iMinimum		= ((int)Counters.size() < GetDisperseCount()) ? Counters.size() : GetDisperseCount();
		int iDisperseCount	= Counters.size() * 0.1;
		iDisperseCount		= (iDisperseCount < iMinimum) ? iMinimum : iDisperseCount;

		dwServerIndex = GetSortedServerIndex( Counters, iDisperseCount );
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "@ 디버그_래더팀 생성, ServerIndex : %lu", dwServerIndex );
		return true;
	}

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "@ 디버그_래더팀 생성, Failed" );
	return false;
}

bool ServerNodeManager::GetSelectShuffleRoomServer( int iShuffleRoomCount, DWORD& dwServerIndex )
{
	COUNTERS Counters;
	if( IsAfford() )
	{
		Counters.push_back( make_pair(0, iShuffleRoomCount) );
	}

	for(vServerNode_iter iter = m_vServerNode.begin() ; iter != m_vServerNode.end() ; ++iter)
	{
		ServerNode *pServerNode = *iter;
		if( !pServerNode || !pServerNode->IsGameNode() || pServerNode->IsBusy() ) continue;

		Counters.push_back( make_pair(pServerNode->GetServerIndex(), pServerNode->GetShuffleRoomCount()) );
	}

	if(Counters.size() > 0)
	{
		int iMinimum		= ((int)Counters.size() < GetDisperseCount()) ? Counters.size() : GetDisperseCount();
		int iDisperseCount	= Counters.size() * 0.1;
		iDisperseCount		= (iDisperseCount < iMinimum) ? iMinimum : iDisperseCount;

		dwServerIndex = GetSortedServerIndex( Counters, iDisperseCount );
		return true;
	}

	return false;
}

void ServerNodeManager::ProcessPing()
{
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;
	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		if( !item->IsConnectState() ) continue;

		item->ProcessPing();
	}
}

void ServerNodeManager::ProcessFlush()
{
	if( m_vServerNode.empty() == false )
	{
		vector< ServerNode* >::iterator	iter	= m_vServerNode.begin();
		vector< ServerNode* >::iterator	iterEnd	= m_vServerNode.end();

		LOOP_GUARD();

		for( iter ; iter != iterEnd ; ++iter )
		{
			ServerNode* pServerNode = (*iter);

			if( !pServerNode->IsActive() )			continue;
			if( !pServerNode->IsConnectState() )	continue;
			if( pServerNode->GetSocket() == INVALID_SOCKET )	continue;

			pServerNode->FlushSendBuffer();
		}//for

		LOOP_GUARD_CLEAR();
	}
}

void ServerNodeManager::SendMessageAllNode( SP2Packet &rkPacket, const DWORD dwServerIndex )
{
	LOOP_GUARD();
	vServerNode_iter iter = m_vServerNode.begin();
	vServerNode_iter iter_Prev;
	while( iter != m_vServerNode.end() )
	{
		iter_Prev = iter++;
		ServerNode *item = *iter_Prev;
		if( !item->IsConnectState() || !item->IsGameNode() ) continue;
		if( item->GetServerIndex() != dwServerIndex ) 
		{
			item->SendMessage( rkPacket );
		}
	}
	LOOP_GUARD_CLEAR();
}

void ServerNodeManager::SendMessageToPartitions( SP2Packet &rkPacket )
{
	for(vServerNode_iter iter = m_vServerNode.begin() ; iter != m_vServerNode.end() ; ++iter)
	{
		ServerNode *item = *iter;
		if( !item->IsConnectState() || !item->IsGameNode() ) continue;
		if( (GetPartitionIndex() == 0)  || (item->GetPartitionIndex() == GetPartitionIndex()) ) 
		{
			item->SendMessage( rkPacket );
		}
	}
}

bool ServerNodeManager::SendMessageNode( int iServerIndex, SP2Packet &rkPacket )
{
	ServerNode *pNode = GetServerNode( iServerIndex );
	if( pNode && pNode->IsConnectState() )
		return pNode->SendMessage( rkPacket );
	return false;
}

bool ServerNodeManager::SendMessageArray( int iServerArray, SP2Packet &rkPacket )
{
	if( !COMPARE( iServerArray, 0, (int)m_vServerNode.size() ) )
		return false;

    vServerNode_iter iter = m_vServerNode.begin() + iServerArray;
	ServerNode *pNode = *iter;
	if( !pNode || pNode->IsDisconnectState() )	return false;
	if( !pNode->IsGameNode())					return false;
	
	return pNode->SendMessage( rkPacket );
}

void ServerNodeManager::FillAllServerIndex( SP2Packet &rkPacket )
{
	rkPacket << (int)m_vServerNode.size();

	LOOP_GUARD();
	vServerNode_iter iter = m_vServerNode.begin();
	while( iter != m_vServerNode.end() )
	{
		ServerNode *item = *iter++;
		rkPacket << item->GetServerIndex();
	}
	LOOP_GUARD_CLEAR();
}

void ServerNodeManager::SendMessageRelay( SP2Packet& rkPacket )
{
	for(int i=0; i< (int)m_vServerNode.size(); ++i)
	{
		if( m_vServerNode[i]->IsRelayNode() )
		{
			m_vServerNode[i]->SendMessage( rkPacket );
		}
	}
}

