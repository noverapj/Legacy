#include "stdafx.h"
#include "ioBroadCastRelayModule.h"
#include "../MainProcess.h"
#include "../Network/ioPacketQueue.h"
#include "../Network/GameServer.h"
#include "../EtcHelpFunc.h"
#include "../NodeInfo/ServerNode.h" 
#include "../NodeInfo/Room.h"
#include "../NodeInfo/RoomNodeManager.h"
#include "../NodeInfo/ioRelayGroupInfoMgr.h"
#include "../NodeInfo/ioRelayRoomInfoMgr.h"
#ifdef ANTIHACK
#include "../Network/ioSkillInfoMgr.h"
#endif

#define USER_MAX 3000 //kyg 추후 ini 셋팅으로 뺼지 고민 할것
#define ROOM_MAX 3000

#ifndef ANTIHACK
ioBroadCastRelayModule::ioBroadCastRelayModule(void) : m_inputCount(0), m_processCount(0), m_iRelayServerIndex(0xa), m_bUseRelayServer(FALSE)
{
	m_queueId = -1;
}	

ioBroadCastRelayModule::~ioBroadCastRelayModule(void)
{
}

BOOL ioBroadCastRelayModule::Init( int seed, int roomSeed, int threadCount)
{
	if( Help::IsServerRelayToTCP() )
		return TRUE;

	m_bufferPool.init(seed);

	InitQueues(threadCount);

	g_RelayGroupInfoMgr->InitData(roomSeed);

	if ( !Begin(threadCount) )
	{
		LOG.PrintTimeAndLog(0,"[error][udp]thread invalid");
		g_App.Shutdown(99, 16);
	}

	return (GetThread() == 0) ? FALSE : TRUE;
}

void ioBroadCastRelayModule::Run()
{
	CURRENTQUEUE* pQueue = GetMyqueue();
	//LOG.PrintTimeAndLog(0, "ThreadID : %d", GetCurrentThreadId());

	while(TRUE)
	{		
		RelayHeader* pRelayHeader = reinterpret_cast<RelayHeader*>(Dequeue(pQueue));
		if(pRelayHeader == NULL)
		{
			Sleep(1);
			continue;
		}

		switch(pRelayHeader->m_packetId)
		{
		case UDP_INSERTDATAPACKET:
			{
				InsertRoom(pRelayHeader);
			}
			break;
		case UDP_REMOVEPACKET:
			{
				RemoveRoom(pRelayHeader);
			}
			break;
		case UDP_SENDPACKET:
			{
				SendPacket(pRelayHeader,(DWORD)pQueue);		 
			}
			break;
		default:
			LOG.PrintTimeAndLog(0,"Error ioBroadCastRelayModule::Run()::ParsingPacket");
		}


		IncrementProcessCount();
	}

}

void ioBroadCastRelayModule::InsertRelayGroupReserve( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort,const ioHashString& publicID )
{
	if(IsRelayServerOn(pRoom->GetRoomIndex(), pRoom->RelayServerIndex()))
	{
		RelayServerInsertGroup(pRoom, dwUserIndex, rkIP, iPort, publicID);
	}
	else
	{
		LocalInsertRelayGroupReserve(pRoom, dwUserIndex, rkIP, iPort);
	} 
}

void ioBroadCastRelayModule::RemoveRelayGroupReserve( Room *pRoom, DWORD dwUserIndex )
{
	if(IsRelayServerOn(pRoom->GetRoomIndex(), pRoom->RelayServerIndex()))
	{
		RelayServerRemoveGroup(pRoom, dwUserIndex);
	}
	else 
	{
		LocalRemoveRelayGroupReserve(pRoom->GetRoomIndex(), dwUserIndex);
	}
}

BOOL ioBroadCastRelayModule::PushRelayPacket( sockaddr_in &client_addr, CPacket &kPacket )
{
	return LocalPushRelayPacekt(kPacket, client_addr);	 
}

void ioBroadCastRelayModule::InsertRoom( RelayHeader* pRelayHeader )
{
	g_RelayGroupInfoMgr->InsertRoom(pRelayHeader);
	m_bufferPool.Push(reinterpret_cast<char*>(pRelayHeader),sizeof(InsertData));
}

void ioBroadCastRelayModule::RemoveRoom( RelayHeader* pRelayHeader )
{
	g_RelayGroupInfoMgr->RemoveRoom(pRelayHeader);
	m_bufferPool.Push(reinterpret_cast<char*>(pRelayHeader),sizeof(RemoveData));
}

BOOL ioBroadCastRelayModule::SendRelayPacket( DWORD dwUserIndex, SP2Packet& rkPacket )
{
	return g_RelayGroupInfoMgr->SendRelayPacket(dwUserIndex,rkPacket);
}

BOOL ioBroadCastRelayModule::SendPacket( RelayHeader* pRelayHeader, DWORD dwQueueId)
{
	UDPPacket* pData = reinterpret_cast<UDPPacket*>(pRelayHeader);
	SP2Packet kPacket;

	sockaddr_in client_addr;
	memcpy(&client_addr, &pData->m_client_addr, sizeof(client_addr)); //수정 부분 
	kPacket.SetBufferCopy(pData->m_buffer, pData->m_size);

	PushPacket(pData);

	kPacket.SetPosBegin();

	if( !COMPARE( kPacket.GetPacketID(), CUPK_CONNECT, 0x5000 ) )
	{
		return FALSE;
	}

	// UDP Test
	if( kPacket.GetPacketID() == CUPK_TEST )
	{
		SP2Packet kRPacket = kPacket;
		DWORD dwIP = 0, dwPort = 0;
		kPacket  >> dwIP >> dwPort;
		//g_UDPNode.SendMessageByAddr( pData->m_client_addr, pData->m_buffer, pData->m_size );
		g_UDPNode.SendMessageByDWORDIP( dwIP, dwPort, kRPacket );
		return TRUE;

	}

	switch( kPacket.GetPacketID() )
	{
	case CUPK_CONNECT:                   // 잘못 왔음.
	case CUPK_SYNCTIME:
	case CUPK_RESERVE_ROOM_JOIN:
	case CUPK_CHECK_KING_PING:
#ifdef FLAG_MODE_BY_BCKIM
	case CUPK_CHECK_FLAG_PING: 	// 2018-03-15 by bckim, 깃발모드 추가
#endif // FLAG_MODE_BY_BCKIM
		return FALSE;
	}

	DWORD dwIP = 0, dwPort = 0;
	kPacket  >> dwIP >> dwPort;

	if( dwIP == 0 || dwPort == 0 )
	{
		DWORD dwUserIndex;
		kPacket >> dwUserIndex;

		int iCutSize = sizeof( DWORD ) * 2;     // IP / PORT

		SP2Packet kRelayPacket( kPacket.GetPacketID() );

		if( kPacket.GetDataSize() > iCutSize )
		{
			kRelayPacket.SetDataAddCreateUDP( 1, 1, (char*)kPacket.GetData() + iCutSize, kPacket.GetDataSize() - iCutSize );
		}

		if( !SendRelayPacket( dwUserIndex, kRelayPacket ) )    
		{ 
			kPacket << client_addr;
		}

		return TRUE;
	}
	else if( dwIP == g_App.GetDwordIP() && dwPort == (DWORD)g_App.GetCSPort() )
	{		 
		return FALSE;
	}

	g_UDPNode.SendMessageByDWORDIP( dwIP, dwPort, kPacket );

	return TRUE;
}


int ioBroadCastRelayModule::GetNodeSize()
{
	int sumCount = 0;
	for(int i=0; i<(int)m_queues.size(); i++)
	{	
#if(_USE_IOCP_)
		sumCount += m_queues[i]->GetCount();
#else						   
		sumCount += m_queues[i]->GetSize();
#endif
	}
	return sumCount;
}

void ioBroadCastRelayModule::LocalInsertRelayGroupReserve( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort )
{
	InsertData *pData = reinterpret_cast<InsertData*>(m_bufferPool.Get(sizeof(InsertData)));
	if(pData == NULL)
	{
		LOG.PrintTimeAndLog(0,"Error InsertRelayGroupReserve Buffer NULL \n");
		return;
	}

	pData->m_packetId		= UDP_INSERTDATAPACKET;
	pData->m_dwRoomIndex	= pRoom->GetRoomIndex();
	pData->m_dwUserIndex	= dwUserIndex;
	pData->m_iClientPort	= iPort;
	strcpy_s(pData->m_szPublicIP, rkIP.c_str());


	//	Enqueue(reinterpret_cast<DWORD>(pData),sizeof(InsertData));
	EnqueueByUser(dwUserIndex,pData);
	//LOG.PrintTimeAndLog(0, "TEST LocalInsertRelayGroupReserve : %d %d %d", pData->m_dwUserIndex, pData->m_dwRoomIndex, GetCurrentThreadId());	//유영재

	pRoom->SetRelayServerIndex(0);

	User* pUser = g_UserNodeManager.GetUserNode(dwUserIndex);
	if(pUser && pUser->IsRelayUse())
	{
		SP2Packet kPacket(STPK_ON_CONTROL);
		int iControlType = RC_NOTUSE_RELAYSVR;
		kPacket << iControlType;

		pUser->SendMessage(kPacket);
		pUser->SendUserLogOut();
	}

	IncementInputCount();
}

BOOL ioBroadCastRelayModule::LocalPushRelayPacekt( CPacket &kPacket, sockaddr_in & client_addr )
{
	UDPPacket* pData = reinterpret_cast<UDPPacket*>(m_bufferPool.Get(sizeof(UDPPacket)));
	if(pData == NULL)
	{
		//	LOG.PrintTimeAndLog(0,"Error PushRelayPacket Buffer NULL\n"); //RD에 추가됨
		return FALSE;
	}

	pData->m_buffer = reinterpret_cast<char*>(m_bufferPool.Get(kPacket.GetBufferSize()));
	if(pData->m_buffer == NULL)
	{
		m_bufferPool.Push((char*)pData,sizeof(UDPPacket)); //kyg 머지 
		//LOG.PrintTimeAndLog(0,"Error PushRelayPacket Buffer NULL\n");
		return FALSE;
	}

	pData->m_packetId	= UDP_SENDPACKET;
	pData->m_size		= kPacket.GetBufferSize();
	memcpy(&pData->m_client_addr, &client_addr, sizeof(client_addr));
	memcpy(pData->m_buffer, kPacket.GetBuffer(), kPacket.GetBufferSize());

	EnqueueByUser((DWORD)client_addr.sin_addr.S_un.S_addr + client_addr.sin_port,pData);

	IncementInputCount();
	return TRUE;
}

void ioBroadCastRelayModule::LocalRemoveRelayGroupReserve( DWORD dwRoomIndex, DWORD dwUserIndex )
{
	RemoveData *pData = reinterpret_cast<RemoveData*>(m_bufferPool.Get(sizeof(RemoveData)));
	if(pData == NULL)
	{
		//LOG.PrintTimeAndLog(0,"Error RemoveRelayGroupReserve Buffer NULL \n");//RD에 추가됨
		return;
	}

	pData->m_packetId		= UDP_REMOVEPACKET;
	pData->m_dwRoomIndex	= dwRoomIndex;
	pData->m_dwUserIndex	= dwUserIndex;
	//Enqueue(reinterpret_cast<DWORD>(pData),sizeof(RemoveData));
	EnqueueByUser(dwUserIndex,pData);
	IncementInputCount();

	//LOG.PrintTimeAndLog(0,"TEST RemoveRelayGroupReserve : %d %d", pData->m_dwUserIndex, pData->m_dwRoomIndex);//유영재
}

void ioBroadCastRelayModule::AddRelayServerInfo( ServerNode* pServerNode )
{
	m_relayServerNodes.AddTail( pServerNode );
}

BOOL ioBroadCastRelayModule::DelRelayServerInfo( ServerNode* pServerNode )
{
	POSITION pos = m_relayServerNodes.Find( pServerNode );
	if(pos != NULL)
	{
		m_relayServerNodes.RemoveAt(pos);
		return TRUE;
	}

	LOG.PrintTimeAndLog(0,"DelRelayServerInfo Fail");
	return FALSE;
}

BOOL ioBroadCastRelayModule::IsRelayServerOn(DWORD dwRoomIndex,int iRelayServerIndex)
{
	if(IsUsingRelayServer() == FALSE && iRelayServerIndex == 0)
		return FALSE;

	BOOL rtVal = FALSE;

	if(m_relayServerNodes.GetCount() != 0 )
	{
		rtVal = IsRelayServerMode();

		return rtVal;
	}
	else
	{
		return rtVal;
	}
}

BOOL ioBroadCastRelayModule::IsRelayServerOn()
{
	if(IsUsingRelayServer() == FALSE)
		return FALSE;

	BOOL rtVal = FALSE;

	if(m_relayServerNodes.GetCount() != 0)
	{
		rtVal = IsRelayServerMode();
	}

	return rtVal;
}

ServerNode* ioBroadCastRelayModule::GetRelayServer() //kyg 여긴 수정이 좀 필요할수도 있음 
{
	POSITION pos = m_relayServerNodes.GetHeadPosition();

	ServerNode* pServerNode = NULL;

	while(pos)
	{
		pServerNode = m_relayServerNodes.GetAt(pos);

		if(pServerNode == NULL)
			return pServerNode;

		if(pServerNode->RelayInfo().m_roomCount < ROOM_MAX && IsRelayServerMode(pServerNode) == TRUE)
		{
			return pServerNode;
		}

		m_relayServerNodes.GetNext(pos);

		pServerNode = NULL;
	}

	return pServerNode;
}

ServerNode* ioBroadCastRelayModule::GetRelayServer( const int relayServerID, BOOL ForceState )
{
	POSITION pos = m_relayServerNodes.GetHeadPosition();
	ServerNode* pServerNode = NULL;

	while(pos)
	{
		pServerNode = m_relayServerNodes.GetAt(pos);

		if(pServerNode == NULL)
			return pServerNode;

		if(pServerNode->RelayServerIndex() == relayServerID &&  IsRelayServerMode(pServerNode) == TRUE)
		{
			return pServerNode;
		}

		m_relayServerNodes.GetNext(pos);

		pServerNode = NULL;
	}

	return pServerNode;
}

ServerNode* ioBroadCastRelayModule::FindRelayServer( int relayServerIndex, BOOL ForceState )
{
	ServerNode* pServerNode = NULL;
	if(m_relayServerNodes.GetCount() != 0 )
	{
		pServerNode = GetRelayServer(relayServerIndex);

		if(pServerNode == NULL)
		{
			pServerNode = GetRelayServer();
		}
	}

	return pServerNode;
}

void ioBroadCastRelayModule::RelayServerInsertGroup(  Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort, const ioHashString& publicID )
{
	if(pRoom == NULL)
		return;

	ServerNode* pServerNode = NULL;

	if(pRoom->RelayServerIndex() == 0) // 로컬 노드라면 
	{
		pServerNode = GetRelayServer();
	}
	else // 로컬노드 아닌데 릴레이서버에도 없을때에는 새로운 릴레이 서버 등록 
	{
		pServerNode = FindRelayServer(pRoom->RelayServerIndex());
	}

	if(pServerNode == NULL)
	{
		LOG.PrintTimeAndLog(0,"RelayServerInsertGroup Error!! ServerNode Is NULL");
		return;
	}

	pRoom->SetRelayServerIndex( pServerNode->RelayServerIndex() );

	SendRelayInsertData* outData = reinterpret_cast<SendRelayInsertData*>(m_bufferPool.Get(sizeof(SendRelayInsertData)));
	if(outData == NULL)
	{
		LOG.PrintTimeAndLog(0,"RelayServerInsertGroup is Null");
		return;
	}

	outData->m_dwRoomIndex = pRoom->GetRoomIndex();
	outData->m_dwUserIndex = dwUserIndex;
	outData->m_iClientPort = iPort;
	strcpy_s(outData->m_szPublicIP,rkIP.c_str());
	strcpy_s(outData->m_szPublicID,publicID.c_str());

	SP2Packet kPacket(RSTPK_ON_CONTROL);
	int iControlType = RS_INSERT_GROUP;
	kPacket << iControlType;
	kPacket << (*outData);

	Debug("I:%s:%d(%d:%d)[%s]\n",outData->m_szPublicIP,outData->m_iClientPort,outData->m_dwRoomIndex,outData->m_dwUserIndex,outData->m_szPublicID);
	//pServerNode->AddRoom(dwRoomIndex);

	pServerNode->SendMessage( kPacket );

	m_bufferPool.Push(reinterpret_cast<char*>(outData),sizeof(SendRelayInsertData));
}

void ioBroadCastRelayModule::RelayServerRemoveGroup( Room *pRoom, DWORD dwUserIndex )
{
	RemoveData* outData = reinterpret_cast<RemoveData*>(m_bufferPool.Get(sizeof(RemoveData)));
	if(outData == NULL)
	{
		LOG.PrintTimeAndLog(0,"RelayServerRemoveGroup MemoryPool is NULL");
		return;
	}

	outData->m_dwRoomIndex = pRoom->GetRoomIndex();
	outData->m_dwUserIndex = dwUserIndex;

	SP2Packet kPacket(RSTPK_ON_CONTROL);
	int iControlType = RS_REMOVE_GROUP;
	kPacket << iControlType;
	kPacket << (*outData);

	Debug("R:(%d:%d)\n",outData->m_dwRoomIndex, outData->m_dwUserIndex);

	ServerNode* pServerNode = NULL;
	if(pRoom->RelayServerIndex() == 0) // 로컬 노드라면 에러 상황
	{
		LOG.PrintTimeAndLog(0,"Error Remove Group Server node Pointer is Null");
		pServerNode = GetRelayServer();
	}
	else 
	{
		pServerNode = GetRelayServer(pRoom->RelayServerIndex());
	}

	if(pServerNode == NULL) //릴레이 서버가 죽은 상황 
	{
		LOG.PrintTimeAndLog(0,"RelayServerInsertGroup Error!! ServerNode Is NULL");
		m_bufferPool.Push(reinterpret_cast<char*>(outData),sizeof(RemoveData));
		return;
	}

	pServerNode->SendMessage( kPacket );

	m_bufferPool.Push(reinterpret_cast<char*>(outData),sizeof(RemoveData));
}

void ioBroadCastRelayModule::MakeInfoPacket( SP2Packet& pk )
{
	int iRelayCount = m_relayServerNodes.GetCount();
	pk << iRelayCount;

	POSITION pos = m_relayServerNodes.GetHeadPosition();
	while(pos)
	{
		ServerNode* pServerNode = m_relayServerNodes.GetAt(pos);
		pk <<  pServerNode->RelayInfo(); 
		m_relayServerNodes.GetNext(pos);
	}
}

void ioBroadCastRelayModule::PushPacket( UDPPacket* pData )
{
	if(pData == NULL)
		return;

	m_bufferPool.Push(pData->m_buffer, pData->m_size);
	m_bufferPool.Push(reinterpret_cast<char*>(pData), sizeof(UDPPacket));
}

BOOL ioBroadCastRelayModule::IsRelayServerMode( ServerNode* node )
{
	BOOL rtVal = FALSE;

	if(node == NULL)
		return rtVal;

	rtVal = TRUE;
	// 
	// 	if( node->GetModeInfo() == MODE_RELAYSERVER || node->GetModeInfo() == MODE_RELAYGUARDIAN)
	// 	{
	// 		rtVal = TRUE;
	// 	}

	return rtVal;
}

BOOL ioBroadCastRelayModule::IsRelayServerMode()
{
	BOOL rtVal = FALSE;

	POSITION pos = m_relayServerNodes.GetHeadPosition();

	while(pos)
	{
		ServerNode* node = m_relayServerNodes.GetAt(pos);
		if(node)
		{
			rtVal = IsRelayServerMode(node);

			if(rtVal == TRUE)
				break;

		}
		m_relayServerNodes.GetNext(pos);
	}

	return rtVal;
}

/************************************************************************/
/* Queues 사용                                                                      */
/************************************************************************/

void ioBroadCastRelayModule::InitQueues( int iQueueCount )
{
	for(int i=0; i<iQueueCount; i++)
	{
		CURRENTQUEUE* queue = new CURRENTQUEUE;
		if(queue == NULL)
			return;
#if(_USE_IOCP_)
		queue->Startup(INFINITE);
#else
#endif
		m_queues.push_back(queue);
	}
}

CURRENTQUEUE* ioBroadCastRelayModule::GetMyqueue()
{
	long val = 0;
	//InterlockedExchange(&val,m_iQueueIndex);
	return Getqueue(InterlockedIncrement(&m_iQueueIndex));

	//return NULL;
}

CURRENTQUEUE* ioBroadCastRelayModule::GetqueueByUser( DWORD dwUserIndex )
{
	return Getqueue(dwUserIndex);
}

void ioBroadCastRelayModule::EnqueueByUser(DWORD dwUserIndex,void* pData)
{
	CURRENTQUEUE* queue = Getqueue(dwUserIndex);
	//	printf("EnqueueByUser : %u \n",queue);
	if(queue == NULL)
		LOG.PrintTimeAndLog(0,"error queue");
#if(_USE_IOCP_)
	queue->Enqueue((DWORD)pData,sizeof(pData));
#else
	queue->Enqueue(reinterpret_cast<NodeData*>(pData));
#endif
}

void* ioBroadCastRelayModule::Dequeue( CURRENTQUEUE* queue )
{
	if(queue)
		return queue->Dequeue();

	return NULL;
}

CURRENTQUEUE* ioBroadCastRelayModule::Getqueue( int queueId )
{
	queueId = queueId % m_queues.size();

	if(queueId >= (int)m_queues.size())
		return NULL;

	return m_queues[queueId];
}

void ioBroadCastRelayModule::GetProcessCounts( std::vector<int>& queueCounts )
{
	for(int i=0; i< (int)m_queues.size(); i++)
	{
		CURRENTQUEUE* queue = m_queues[i];
		if(queue)
		{
#if(_USE_IOCP_)
			queueCounts.push_back(queue->GetProcessCount());
#else
			queueCounts.push_back(queue->GetProcessCount());
#endif
		}
	}
}
#else

#include "ioGuardianMgr.h"
extern CLog CriticalLOG;

ioBroadCastRelayModule::ioBroadCastRelayModule(void) : m_inputCount(0), m_processCount(0), m_iRelayServerIndex(0xa), m_bUseRelayServer(FALSE)
{
	m_queueId = -1;
	m_iQueueIndex = 0;
	m_iRelayGroupIndex = -1;
}	

ioBroadCastRelayModule::~ioBroadCastRelayModule(void)
{
}

BOOL ioBroadCastRelayModule::Init( int seed, int roomSeed, int threadCount)
{
	if( Help::IsServerRelayToTCP() )
		return TRUE;

	// 인스턴스 만들려고..
	g_SkillInfoMgr;

	m_bufferPool.init(seed);

	InitQueues(threadCount);

	InitRelayGroups(threadCount, roomSeed);

	Begin(threadCount);

	return (GetThread() == 0) ? FALSE : TRUE;
}

void ioBroadCastRelayModule::Run()
{
	CURRENTQUEUE* pQueue = GetMyqueue();
	ioRelayGroupInfoMgr* pRelayGroupInfoMgr = GetMyRelayRoomInfo();

	if(pQueue == NULL || pRelayGroupInfoMgr == NULL) return;

	while(TRUE)
	{		
		ProcessPacket(pQueue, pRelayGroupInfoMgr);
	}
}

void ioBroadCastRelayModule::InsertRelayGroupReserve( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort,const ioHashString& publicID, DWORD dwUserSeed, DWORD dwNPCSeed, int iCoolType, int iModeType, int iRoomStyle, int iTeamType )
{
	LocalInsertRelayGroupReserve(pRoom, dwUserIndex, rkIP, iPort, dwUserSeed, dwNPCSeed, iCoolType, iModeType, iRoomStyle, iTeamType );
}

void ioBroadCastRelayModule::RemoveRelayGroupReserve( Room *pRoom, DWORD dwUserIndex )
{
	if(IsRelayServerOn(pRoom->GetRoomIndex(), pRoom->RelayServerIndex()))
	{
		RelayServerRemoveGroup(pRoom, dwUserIndex);
	}
	else 
	{
		LocalRemoveRelayGroupReserve(pRoom->GetRoomIndex(), dwUserIndex);
	}
}

BOOL ioBroadCastRelayModule::PushRelayPacket( sockaddr_in &client_addr, CPacket &kPacket )
{
	return LocalPushRelayPacekt(kPacket, client_addr);	 
}

void ioBroadCastRelayModule::InsertRoom( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfo )
{
	if(pRelayHeader)
	{
		InsertData* pInsetData = reinterpret_cast<InsertData*>(pRelayHeader);

		if(pRelayGroupInfo)
			pRelayGroupInfo->InsertRoom(pRelayHeader);

		m_bufferPool.Push(reinterpret_cast<char*>(pRelayHeader), sizeof(InsertData));
	}
}

void ioBroadCastRelayModule::RemoveRoom( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfo )
{
	if(pRelayHeader)
	{
		RemoveData* pRemoveData = reinterpret_cast<RemoveData*>(pRelayHeader);

		if(pRelayGroupInfo)
			pRelayGroupInfo->RemoveRoom(pRelayHeader);

		m_bufferPool.Push(reinterpret_cast<char*>(pRelayHeader), sizeof(RemoveData));
	}
}

int ioBroadCastRelayModule::GetNodeSize()
{
	int sumCount = 0;
	for(int i=0; i<m_queues.size(); i++)
	{	
#if(_USE_IOCP_)
		sumCount += m_queues[i]->GetCount();
#else						   
		sumCount += m_queues[i]->GetSize();
#endif
	}
	return sumCount;
}

void ioBroadCastRelayModule::LocalInsertRelayGroupReserve( Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort, DWORD dwUserSeed, DWORD dwNPCSeed, int iCoolType, int iModeType, int iRoomStyle, int iTeamType )
{
	InsertData *pData = reinterpret_cast<InsertData*>(m_bufferPool.Get(sizeof(InsertData)));
	if(pData == NULL)
	{
		LOG.PrintTimeAndLog(0,"Error InsertRelayGroupReserve Buffer NULL \n");
		return;
	}

	pData->m_packetId		= UDP_INSERTDATAPACKET;
	pData->m_dwRoomIndex	= pRoom->GetRoomIndex();
	pData->m_dwUserIndex	= dwUserIndex;
	pData->m_iClientPort	= iPort;
	pData->m_dwUserSeed	= dwUserSeed;
	pData->m_dwNPCSeed = dwNPCSeed;
	pData->m_iCoolType = iCoolType;
	pData->m_iModeType = iModeType;
	pData->m_iRoomStyle = iRoomStyle;
	pData->m_iTeamType = iTeamType;

	strcpy_s(pData->m_szPublicIP, rkIP.c_str());

	EnqueueByIndex( pData->m_dwRoomIndex, pData );
	LOG.PrintTimeAndLog( 0, "TEST - InsertGroup EnqueByIndex - roomIndex(%u)", pData->m_dwRoomIndex );

	IncementInputCount();
}

BOOL ioBroadCastRelayModule::LocalPushRelayPacekt( CPacket &kPacket, sockaddr_in & client_addr )
{
	UDPPacket* pData = reinterpret_cast<UDPPacket*>(m_bufferPool.Get(sizeof(UDPPacket)));
	if(pData == NULL)
	{
		return FALSE;
	}

	pData->m_buffer = reinterpret_cast<char*>(m_bufferPool.Get(kPacket.GetBufferSize()));
	if(pData->m_buffer == NULL)
	{
		m_bufferPool.Push((char*)pData,sizeof(UDPPacket)); //kyg 머지 
		return FALSE;
	}

	pData->m_packetId	= UDP_SENDPACKET;
	pData->m_size		= kPacket.GetBufferSize();
	memcpy(&pData->m_client_addr, &client_addr, sizeof(client_addr));
	memcpy(pData->m_buffer, kPacket.GetBuffer(), kPacket.GetBufferSize());
	DWORD dwIndex = GetRoomIndexByRelayPacket((SP2Packet&)kPacket);
	EnqueueByIndex(dwIndex, pData);

	IncementInputCount();
	return TRUE;
}

void ioBroadCastRelayModule::LocalRemoveRelayGroupReserve( DWORD dwRoomIndex, DWORD dwUserIndex )
{
	RemoveData *pData = reinterpret_cast<RemoveData*>(m_bufferPool.Get(sizeof(RemoveData)));
	if(pData == NULL)
	{
		return;
	}
	pData->m_packetId		= UDP_REMOVEPACKET;
	pData->m_dwRoomIndex	= dwRoomIndex;
	pData->m_dwUserIndex	= dwUserIndex;

	EnqueueByIndex(dwRoomIndex,pData);
	IncementInputCount();
}

void ioBroadCastRelayModule::AddRelayServerInfo( ServerNode* pServerNode )
{
	m_relayServerNodes.AddTail( pServerNode );
}

BOOL ioBroadCastRelayModule::DelRelayServerInfo( ServerNode* pServerNode )
{
	POSITION pos = m_relayServerNodes.Find( pServerNode );
	if(pos != NULL)
	{
		m_relayServerNodes.RemoveAt(pos);
		return TRUE;
	}

	LOG.PrintTimeAndLog(0,"DelRelayServerInfo Fail");
	return FALSE;
}

BOOL ioBroadCastRelayModule::IsRelayServerOn(DWORD dwRoomIndex,int iRelayServerIndex)
{
	if(IsUsingRelayServer() == FALSE && iRelayServerIndex == 0)
		return FALSE;

	BOOL rtVal = FALSE;

	if(m_relayServerNodes.GetCount() != 0 )
	{
		rtVal = IsRelayServerMode();

		return rtVal;
	}
	else
	{
		return rtVal;
	}
}

BOOL ioBroadCastRelayModule::IsRelayServerOn()
{
	if(IsUsingRelayServer() == FALSE)
		return FALSE;

	BOOL rtVal = FALSE;

	if(m_relayServerNodes.GetCount() != 0)
	{
		rtVal = IsRelayServerMode();
	}

	return rtVal;
}

ServerNode* ioBroadCastRelayModule::GetRelayServer() //kyg 여긴 수정이 좀 필요할수도 있음 
{
	POSITION pos = m_relayServerNodes.GetHeadPosition();

	ServerNode* pServerNode = NULL;

	while(pos)
	{
		pServerNode = m_relayServerNodes.GetAt(pos);

		if(pServerNode == NULL)
			return pServerNode;

		if(pServerNode->RelayInfo().m_roomCount < ROOM_MAX && IsRelayServerMode(pServerNode) == TRUE)
		{
			return pServerNode;
		}

		m_relayServerNodes.GetNext(pos);

		pServerNode = NULL;
	}

	return pServerNode;
}

ServerNode* ioBroadCastRelayModule::GetRelayServer( const int relayServerID, BOOL ForceState )
{
	POSITION pos = m_relayServerNodes.GetHeadPosition();
	ServerNode* pServerNode = NULL;

	while(pos)
	{
		pServerNode = m_relayServerNodes.GetAt(pos);

		if(pServerNode == NULL)
			return pServerNode;

		if(pServerNode->RelayServerIndex() == relayServerID &&  IsRelayServerMode(pServerNode) == TRUE)
		{
			return pServerNode;
		}

		m_relayServerNodes.GetNext(pos);

		pServerNode = NULL;
	}

	return pServerNode;
}

ServerNode* ioBroadCastRelayModule::FindRelayServer( int relayServerIndex, BOOL ForceState )
{
	ServerNode* pServerNode = NULL;
	if(m_relayServerNodes.GetCount() != 0 )
	{
		pServerNode = GetRelayServer(relayServerIndex);

		if(pServerNode == NULL)
		{
			pServerNode = GetRelayServer();
		}
	}

	return pServerNode;
}

void ioBroadCastRelayModule::RelayServerInsertGroup(  Room *pRoom, DWORD dwUserIndex, const ioHashString &rkIP, int iPort, const ioHashString& publicID )
{
	if(pRoom == NULL)
		return;

	ServerNode* pServerNode = NULL;

	if(pRoom->RelayServerIndex() == 0) // 로컬 노드라면 
	{
		pServerNode = GetRelayServer();
	}
	else // 로컬노드 아닌데 릴레이서버에도 없을때에는 새로운 릴레이 서버 등록 
	{
		pServerNode = FindRelayServer(pRoom->RelayServerIndex());
	}

	if(pServerNode == NULL)
	{
		LOG.PrintTimeAndLog(0,"RelayServerInsertGroup Error!! ServerNode Is NULL");
		return;
	}

	pRoom->SetRelayServerIndex( pServerNode->RelayServerIndex() );

	SendRelayInsertData* outData = reinterpret_cast<SendRelayInsertData*>(m_bufferPool.Get(sizeof(SendRelayInsertData)));
	if(outData == NULL)
	{
		LOG.PrintTimeAndLog(0,"RelayServerInsertGroup is Null");
		return;
	}

	outData->m_dwRoomIndex = pRoom->GetRoomIndex();
	outData->m_dwUserIndex = dwUserIndex;
	outData->m_iClientPort = iPort;
	strcpy_s(outData->m_szPublicIP,rkIP.c_str());
	strcpy_s(outData->m_szPublicID,publicID.c_str());

	SP2Packet kPacket(RSTPK_ON_CONTROL);
	int iControlType = RS_INSERT_GROUP;
	kPacket << iControlType;
	kPacket << (*outData);

	Debug("I:%s:%d(%d:%d)[%s]\n",outData->m_szPublicIP,outData->m_iClientPort,outData->m_dwRoomIndex,outData->m_dwUserIndex,outData->m_szPublicID);

	pServerNode->SendMessage( kPacket );

	m_bufferPool.Push(reinterpret_cast<char*>(outData),sizeof(SendRelayInsertData));
}

void ioBroadCastRelayModule::RelayServerRemoveGroup( Room *pRoom, DWORD dwUserIndex )
{
	RemoveData* outData = reinterpret_cast<RemoveData*>(m_bufferPool.Get(sizeof(RemoveData)));
	if(outData == NULL)
	{
		LOG.PrintTimeAndLog(0,"RelayServerRemoveGroup MemoryPool is NULL");
		return;
	}

	outData->m_dwRoomIndex = pRoom->GetRoomIndex();
	outData->m_dwUserIndex = dwUserIndex;

	SP2Packet kPacket(RSTPK_ON_CONTROL);
	int iControlType = RS_REMOVE_GROUP;
	kPacket << iControlType;
	kPacket << (*outData);

	Debug("R:(%d:%d)\n",outData->m_dwRoomIndex, outData->m_dwUserIndex);

	ServerNode* pServerNode = NULL;
	if(pRoom->RelayServerIndex() == 0) // 로컬 노드라면 에러 상황
	{
		LOG.PrintTimeAndLog(0,"Error Remove Group Server node Pointer is Null");
		pServerNode = GetRelayServer();
	}
	else 
	{
		pServerNode = GetRelayServer(pRoom->RelayServerIndex());
	}

	if(pServerNode == NULL) //릴레이 서버가 죽은 상황 
	{
		LOG.PrintTimeAndLog(0,"RelayServerInsertGroup Error!! ServerNode Is NULL");
		m_bufferPool.Push(reinterpret_cast<char*>(outData),sizeof(RemoveData));
		return;
	}

	pServerNode->SendMessage( kPacket );

	m_bufferPool.Push(reinterpret_cast<char*>(outData),sizeof(RemoveData));
}

void ioBroadCastRelayModule::MakeInfoPacket( SP2Packet& pk )
{
	int iRelayCount = m_relayServerNodes.GetCount();
	pk << iRelayCount;

	POSITION pos = m_relayServerNodes.GetHeadPosition();
	while(pos)
	{
		ServerNode* pServerNode = m_relayServerNodes.GetAt(pos);
		pk <<  pServerNode->RelayInfo(); 
		m_relayServerNodes.GetNext(pos);
	}
}

void ioBroadCastRelayModule::PushPacket( UDPPacket* pData )
{
	if(pData == NULL)
		return;

	m_bufferPool.Push(pData->m_buffer, pData->m_size);
	m_bufferPool.Push(reinterpret_cast<char*>(pData), sizeof(UDPPacket));
}

BOOL ioBroadCastRelayModule::IsRelayServerMode( ServerNode* node )
{
	BOOL rtVal = FALSE;

	if(node == NULL)
		return rtVal;

	rtVal = TRUE;

	return rtVal;
}

BOOL ioBroadCastRelayModule::IsRelayServerMode()
{
	BOOL rtVal = FALSE;

	POSITION pos = m_relayServerNodes.GetHeadPosition();

	while(pos)
	{
		ServerNode* node = m_relayServerNodes.GetAt(pos);
		if(node)
		{
			rtVal = IsRelayServerMode(node);

			if(rtVal == TRUE)
				break;

		}
		m_relayServerNodes.GetNext(pos);
	}

	return rtVal;
}

void ioBroadCastRelayModule::InsertRoomInfo( const DWORD dwUserIndex,const DWORD dwRoomIndex )
{	
	g_RelayRoomInfoMgr->InsertRoomInfo(dwUserIndex,dwRoomIndex);
}

void ioBroadCastRelayModule::RemoveRoomInfo( const DWORD dwUserIndex)
{
	if(dwUserIndex != 0)
	{
		g_RelayRoomInfoMgr->RemoveRoomInfo(dwUserIndex);
	}
}

DWORD ioBroadCastRelayModule::GetRoomIndexByUser( DWORD dwUserIndex )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", __FUNCTION__ );
	return g_RelayRoomInfoMgr->GetRoomIndexByUser(dwUserIndex);
}

/************************************************************************/
/* Queues 사용                                                                      */
/************************************************************************/

void ioBroadCastRelayModule::InitQueues( int iQueueCount )
{
	for(int i=0; i<iQueueCount; i++)
	{
		CURRENTQUEUE* queue = new CURRENTQUEUE;
		if(queue == NULL)
			return;
#if(_USE_IOCP_)
		queue->Startup(INFINITE);
#else
#endif
		m_queues.push_back(queue);
	}
}

CURRENTQUEUE* ioBroadCastRelayModule::GetMyqueue()
{
	long val = 0;
	return Getqueue(InterlockedIncrement(&m_iQueueIndex));

	//return NULL;
}

CURRENTQUEUE* ioBroadCastRelayModule::GetqueueByUser( DWORD dwUserIndex )
{
	return Getqueue(dwUserIndex);
}

void ioBroadCastRelayModule::EnqueueByIndex(DWORD dwUserIndex,void* pData)
{
	CURRENTQUEUE* queue = Getqueue(dwUserIndex);
	if(queue == NULL)
		LOG.PrintTimeAndLog(0,"error queue");
#if(_USE_IOCP_)
	queue->Enqueue((DWORD)pData,sizeof(pData));
#else
	queue->Enqueue(reinterpret_cast<NodeData*>(pData));
#endif
}

void* ioBroadCastRelayModule::Dequeue( CURRENTQUEUE* queue )
{
	if(queue)
		return queue->Dequeue();

	return NULL;
}

CURRENTQUEUE* ioBroadCastRelayModule::Getqueue( int queueId )
{
	queueId = queueId % m_queues.size();

	if(queueId >= m_queues.size())
		return NULL;

	return m_queues[queueId];
}

void ioBroadCastRelayModule::GetProcessCounts( std::vector<int>& queueCounts )
{
	for(int i=0; i< m_queues.size(); i++)
	{
		CURRENTQUEUE* queue = m_queues[i];
		if(queue)
		{
#if(_USE_IOCP_)
			queueCounts.push_back(queue->GetProcessCount());
#else
			queueCounts.push_back(queue->GetProcessCount());
#endif
		}
	}
}

void ioBroadCastRelayModule::InitRelayGroups( int iThreadCount, int iRoomSeed )
{
	ioINILoader iLoader("ls_config_game.ini");
	iLoader.SetTitle( "ANTIHACK" );

	m_iAntiWaitTime = iLoader.LoadInt( "AntiHackWaitTime", 1000 );
	m_fAntiErrorRate = iLoader.LoadFloat( "AntiHackErrorRate", 0.8f );

	m_iPenguinCount = iLoader.LoadInt( "AntiHackPenguinCount", 8 );
	m_iKickCount = iLoader.LoadInt( "AntiHackKickCount", 50 );
	m_iTimeDecrease = iLoader.LoadFloat( "AntiHackTimeDecrease", 30 );

	m_iSkillHackCount = iLoader.LoadInt( "AntiHackSkillCount", 5 );
	m_iSkillKickCount = iLoader.LoadInt( "AntiHackSkillKickCount", 15 );
	m_iSkillTimeDecrease = iLoader.LoadInt( "AntiHackSkillTimeDecrease", 30 );

	int iExceptCnt = iLoader.LoadInt( "AntiHackExceptSkillCnt", 0 );

	std::vector<int> vecID;

	char szKey[MAX_PATH]={0,};
	for( int i = 0; i < iExceptCnt; ++i )
	{
		sprintf( szKey, "AntiHackExceptID%d", i+1 );
		int iExceptID = iLoader.LoadInt( szKey, 0 );
		vecID.push_back( iExceptID );
	}


	for(int i=0; i<iThreadCount; i++)
	{
		ioRelayGroupInfoMgr* pRelayGroupMgr = new ioRelayGroupInfoMgr;

		if(pRelayGroupMgr)
			pRelayGroupMgr->InitData(iRoomSeed,m_iAntiWaitTime,m_fAntiErrorRate,
			m_iPenguinCount, m_iKickCount, m_iTimeDecrease, m_iSkillHackCount, m_iSkillKickCount, m_iSkillTimeDecrease, vecID );

		if( i == 0 )
			pRelayGroupMgr->OnRUDPRoomLog();

		m_relayGroups.push_back(pRelayGroupMgr);
	}
}

ioRelayGroupInfoMgr* ioBroadCastRelayModule::GetMyRelayRoomInfo()
{
	return m_relayGroups[InterlockedIncrement(&m_iRelayGroupIndex)];
}

ioRelayGroupInfoMgr* ioBroadCastRelayModule::GetMyRelayGroupInfoFromRoomIndex( DWORD dwRoomIndex )
{
	int iIndex = dwRoomIndex % m_relayGroups.size();

	return m_relayGroups[iIndex];
}

DWORD ioBroadCastRelayModule::GetRoomIndexByRelayPacket( SP2Packet& rkPacket )
{
	DWORD dwIP = 0, dwPort = 0, dwUserIndex = 0;
	DWORD dwRoomIndex = 0;

	rkPacket.SetPosBegin();

	switch(rkPacket.GetPacketID())
	{
	case CUPK_USE_SKILL:	
	default:
		{
			PACKET_GUARD_INT(rkPacket.Read(dwIP));
			PACKET_GUARD_INT(rkPacket.Read(dwPort));
			PACKET_GUARD_INT(rkPacket.Read(dwUserIndex));
			dwRoomIndex =  g_RelayRoomInfoMgr->GetRoomIndexByUser(dwUserIndex);
		}
		break;
	}

	rkPacket.SetPosBegin();

	return dwRoomIndex;
}

void ioBroadCastRelayModule::ProcessPacket( CURRENTQUEUE* pQueue, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	if(pQueue == NULL || pRelayGroupInfoMgr == NULL)
		return;

	__try
	{
		RelayHeader* pRelayHeader = reinterpret_cast<RelayHeader*>(Dequeue(pQueue));

		if(pRelayHeader == NULL)
		{
			Sleep(1);
			return;
		}

		switch(pRelayHeader->m_packetId)
		{
		case UDP_INSERTDATAPACKET:
			{
				printf(" UDP_INSERTDATAPACKET threadID:%d \n", GetCurrentThreadId());
				InsertRoom(pRelayHeader, pRelayGroupInfoMgr);
			}
			break;
		case UDP_REMOVEPACKET:
			{
				//printf(" UDP_REMOVEPACKET threadID:%d \n", GetCurrentThreadId());
				RemoveRoom(pRelayHeader, pRelayGroupInfoMgr);
			}
			break;
		case UDP_SENDPACKET:
			{
				//printf(" UDP_SENDPACKET threadID:%d \n", GetCurrentThreadId());
				ReceivePacket(pRelayHeader, pRelayGroupInfoMgr);
			}
			break;
		case UDP_UPDATEWINCNT:
			{
				//printf(" UDP_SENDPACKET threadID:%d \n", GetCurrentThreadId());
				UpdateWinCnt(pRelayHeader, pRelayGroupInfoMgr);
			}
			break;
		case UDP_UPDATESCORE:
			{
				//printf(" UDP_SENDPACKET threadID:%d \n", GetCurrentThreadId());
				UpdateWinCnt(pRelayHeader, pRelayGroupInfoMgr);
			}
			break;
		case UDP_TIMER:
			{
				UpdateTimer(pRelayHeader, pRelayGroupInfoMgr);
			}
			break;
		case UDP_ANTIHACK_RELOAD:
			{
				ReloadAntihack(pRelayHeader, pRelayGroupInfoMgr);
			}
			break;
		case UDP_UPDATE_SP_POTION:
			{
				UpdateSPPotion(pRelayHeader, pRelayGroupInfoMgr);
			}
			break;
		case UDP_USER_DIESTATE:
			{
				OnUpdateDieState(pRelayHeader,pRelayGroupInfoMgr);
			}
			break;
		default:
			LOG.PrintTimeAndLog(0,"Error ioBroadCastRelayModule::Run()::ParsingPacket(%d)", pRelayHeader->m_packetId);
		}

		IncrementProcessCount();
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioBroadCastRelayModule Crash!!" );
	}
}

BOOL ioBroadCastRelayModule::ReceivePacket( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	UDPPacket* pData = reinterpret_cast<UDPPacket*>(pRelayHeader);

	SP2Packet kPacket(false,PK_UDP,pData->m_size) ;

	sockaddr_in sockAddr;
	ExtractPacket( pData, kPacket, sockAddr );

	//여기서 체크 루틴
	if( !g_Guardian.PacketParsing( kPacket, pRelayGroupInfoMgr ) )
		return true;

	return Broadcast( kPacket, pRelayGroupInfoMgr );
}

BOOL ioBroadCastRelayModule::BroadcastPacket( DWORD dwUserIndex, SP2Packet& rkPacket, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	return pRelayGroupInfoMgr->BroadcastPacket(dwUserIndex, rkPacket);
}

void ioBroadCastRelayModule::ExtractPacket( UDPPacket* pData, SP2Packet &rkPacket, sockaddr_in &sockAddr )
{
	memcpy_s(&sockAddr, sizeof(sockAddr), &pData->m_client_addr, sizeof(sockAddr));

	rkPacket.SetBufferCopy(pData->m_buffer, pData->m_size);

	PushPacket(pData);
}

BOOL ioBroadCastRelayModule::Broadcast( SP2Packet &rkPacket, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	rkPacket.SetPosBegin();

	//일단 이렇게
	DWORD dwIP = 0, dwPort = 0;
	PACKET_GUARD_BOOL( rkPacket.Read(dwIP) );
	PACKET_GUARD_BOOL( rkPacket.Read(dwPort) );

	if( (dwIP == 0) || (dwPort == 0) )
	{
		int iCutSize = sizeof( DWORD ) * 2;     // IP + PORT

		DWORD dwUserIndex = 0;
		PACKET_GUARD_BOOL( rkPacket.Read(dwUserIndex) );

		SP2Packet kRelayPacket( rkPacket.GetPacketID(), false, PK_UDP, rkPacket.GetDataSize() - iCutSize );
		if( rkPacket.GetDataSize() > iCutSize )
		{
			kRelayPacket.SetDataAddCreateUDP( 1, 1, (char*)rkPacket.GetData() + iCutSize, rkPacket.GetDataSize() - iCutSize );
		}

		BroadcastPacket( dwUserIndex, kRelayPacket, pRelayGroupInfoMgr );
		return TRUE;
	}
	else if( dwIP == g_App.GetDwordIP() && dwPort == (DWORD)g_App.GetCSPort() )
	{		 
		return FALSE;
	}

	g_UDPNode.SendMessageByDWORDIP( dwIP, dwPort, rkPacket );
	return TRUE;
}

void ioBroadCastRelayModule::UpdateRelayGroupWin( DWORD dwRoomIndex, int iRedTeamWinCnt, int iBlueTeamWinCnt )
{
	UpdateRelayGroupWinCntData* pData = reinterpret_cast<UpdateRelayGroupWinCntData*>(m_bufferPool.Get(sizeof(UpdateRelayGroupWinCntData)));
	if(pData == NULL)
	{
		return;
	}

	pData->m_packetId = UDP_UPDATEWINCNT;
	pData->m_dwRoomIndex	 = dwRoomIndex;
	pData->m_iRedTeamWinCnt = iRedTeamWinCnt;
	pData->m_iBueTeamWinCnt = iBlueTeamWinCnt;

	EnqueueByIndex(dwRoomIndex, pData);
	IncementInputCount();

	//LOG.PrintTimeAndLog( 0, "TEST - UpdateRelayGroupWin roomIndex(%u), Win Red/Blue(%d/%d)", pData->m_dwRoomIndex, iRedTeamWinCnt, iBlueTeamWinCnt );
}

void ioBroadCastRelayModule::UpdateWinCnt( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	if(pRelayHeader)
	{
		UpdateRelayGroupWinCntData* pWinCntData = reinterpret_cast<UpdateRelayGroupWinCntData*>(pRelayHeader);

		if(pRelayGroupInfoMgr)
			pRelayGroupInfoMgr->UpdateRelayGroupWin(pWinCntData->m_dwRoomIndex,pWinCntData->m_iRedTeamWinCnt,pWinCntData->m_iBueTeamWinCnt);

		m_bufferPool.Push(reinterpret_cast<char*>(pWinCntData), sizeof(UpdateRelayGroupWinCntData));
	}
}

void ioBroadCastRelayModule::UpdateRelayGroupScore( DWORD dwRoomIndex, int iTeamType )
{
	UpdateRelayGroupScoreData* pData = reinterpret_cast<UpdateRelayGroupScoreData*>(m_bufferPool.Get(sizeof(UpdateRelayGroupScoreData)));
	if(pData == NULL)
	{
		return;
	}

	pData->m_packetId = UDP_UPDATESCORE;
	pData->m_dwRoomIndex	 = dwRoomIndex;
	pData->m_iTeamType = iTeamType;

	EnqueueByIndex(dwRoomIndex, pData);
	IncementInputCount();

	//LOG.PrintTimeAndLog( 0, "TEST - UpdateRelayGroupWin roomIndex(%u), Score TemaType(%d)", pData->m_dwRoomIndex, iTeamType );
}

void ioBroadCastRelayModule::UpdateScore( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	if(pRelayHeader)
	{
		UpdateRelayGroupScoreData* pScoreData = reinterpret_cast<UpdateRelayGroupScoreData*>(pRelayHeader);

		if(pRelayGroupInfoMgr)
			pRelayGroupInfoMgr->UpdateRelayGroupScore(pScoreData->m_dwRoomIndex,pScoreData->m_iTeamType);

		m_bufferPool.Push(reinterpret_cast<char*>(pScoreData), sizeof(UpdateRelayGroupScoreData));
	}
}

void ioBroadCastRelayModule::InsertTimerOperation()
{
	for(BYTE i=0; i< m_queues.size(); i++)
	{
		RelayHeader* pData = reinterpret_cast<RelayHeader*>(m_bufferPool.Get(sizeof(RelayHeader)));
		if(pData == NULL)
		{
			LOG.PrintTimeAndLog(0, "[error][UDP]InsertTimerOperation pool is empty");
			return;
		}

		pData->m_packetId		= UDP_TIMER;

		EnqueueByIndex(i, pData);
		IncementInputCount();
	}
}

void ioBroadCastRelayModule::UpdateTimer( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	if( pRelayHeader )
	{
		pRelayGroupInfoMgr->CheckAntiHackData();
		m_bufferPool.Push(reinterpret_cast<char*>(pRelayHeader), sizeof(pRelayHeader));
	}
}

void ioBroadCastRelayModule::OnAntihackReload()
{
	bool bChange = false;
	ioINILoader iLoader;
	iLoader.ReloadFile("ls_config_game.ini");
	iLoader.SetTitle( "ANTIHACK" );

	int iAntiWaitTime = iLoader.LoadInt( "AntiHackWaitTime", 1000 );
	float fAntiErrorRate = iLoader.LoadFloat( "AntiHackErrorRate", 0.8f );

	int iPenguinCount = iLoader.LoadInt( "AntiHackPenguinCount", 8 );
	int iKickCount = iLoader.LoadInt( "AntiHackKickCount", 50 );
	int iTimeDecrease = iLoader.LoadFloat( "AntiHackTimeDecrease", 30 );

	int iSkillHackCount = iLoader.LoadInt( "AntiHackSkillCount", 5 );
	int iSkillKickCount = iLoader.LoadInt( "AntiHackSkillKickCount", 15 );
	int iSkillTimeDecrease = iLoader.LoadInt( "AntiHackSkillTimeDecrease", 30 );	
	std::vector<int> vecID;

	if( m_fAntiErrorRate != fAntiErrorRate || m_iAntiWaitTime != iAntiWaitTime || m_iPenguinCount != iPenguinCount || m_iKickCount != iKickCount || 
		m_iTimeDecrease != iTimeDecrease || m_iSkillHackCount != iSkillHackCount || m_iSkillKickCount != iSkillKickCount || m_iSkillTimeDecrease != iSkillTimeDecrease )
	{
		int iExceptCnt = iLoader.LoadInt( "AntiHackExceptSkillCnt", 0 );

		bChange = true;		
		char szKey[MAX_PATH]={0,};
		for( int i = 0; i < iExceptCnt; ++i )
		{	
			sprintf( szKey, "AntiHackExceptID%d", i+1 );
			int iExceptID = iLoader.LoadInt( szKey, 0 );
			vecID.push_back( iExceptID );

			if( i >= 9 )
				break;
		}
	}



	if( bChange )
	{
		for(BYTE i=0; i< m_queues.size(); i++)
		{
			UpdateAntihackInfo* pData = reinterpret_cast<UpdateAntihackInfo*>(m_bufferPool.Get(sizeof(UpdateAntihackInfo)));
			if(pData == NULL)
			{
				LOG.PrintTimeAndLog(0, "[error][UDP]OnAntihackReload pool is empty");
				return;
			}

			pData->m_packetId = UDP_ANTIHACK_RELOAD;
			pData->fAntiErrorRate = fAntiErrorRate;
			pData->iAntiWaitTime = iAntiWaitTime;			
			pData->iPenguinCount = iPenguinCount;
			pData->iKickCount = iKickCount;
			pData->iTimeDecrease = iTimeDecrease;
			pData->iSkillHackCount = iSkillHackCount;
			pData->iSkillKickCount = iSkillKickCount;
			pData->iSkillTimeDecrease = iSkillTimeDecrease;

			int vSize = vecID.size();
			for( int j = 0; j < 10; ++j )
			{
				if( i < vSize )
					pData->iExceptSkillID[j] = vecID[j];
				else
					pData->iExceptSkillID[j] = 0;
			}



			EnqueueByIndex(i, pData);
			IncementInputCount();
		}

	}
}

void ioBroadCastRelayModule::ReloadAntihack( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	if( pRelayHeader )
	{
		UpdateAntihackInfo* pAntiInfo = reinterpret_cast<UpdateAntihackInfo*>(pRelayHeader);

		if(pRelayGroupInfoMgr)
			pRelayGroupInfoMgr->ReloadAntiHack(pAntiInfo->fAntiErrorRate, pAntiInfo->iAntiWaitTime, pAntiInfo->iPenguinCount, pAntiInfo->iKickCount, pAntiInfo->iTimeDecrease,
			pAntiInfo->iSkillHackCount, pAntiInfo->iSkillKickCount, pAntiInfo->iSkillTimeDecrease, pAntiInfo->iExceptSkillID );

		m_bufferPool.Push(reinterpret_cast<char*>(pRelayHeader), sizeof(UpdateAntihackInfo));
	}
}

void ioBroadCastRelayModule::UpdateSPPotion( DWORD dwRoomIndex, DWORD dwUserIndex )
{
	UpdateRelayGroupSpData* pData = reinterpret_cast<UpdateRelayGroupSpData*>(m_bufferPool.Get(sizeof(UpdateRelayGroupSpData)));
	if(pData == NULL)
	{
		return;
	}

	pData->m_packetId = UDP_UPDATE_SP_POTION;
	pData->m_dwRoomIndex	 = dwRoomIndex;
	pData->m_dwUserIndex = dwUserIndex;

	EnqueueByIndex(dwRoomIndex, pData);
	IncementInputCount();

	LOG.PrintTimeAndLog( 0, "TEST - UpdateSPPotion roomIndex(%u), dwUserIndex(%u)", dwRoomIndex, dwUserIndex );
}

void ioBroadCastRelayModule::UpdateSPPotion( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	if( pRelayHeader )
	{
		UpdateRelayGroupSpData* pSPInfo = reinterpret_cast<UpdateRelayGroupSpData*>(pRelayHeader);

		if(pRelayGroupInfoMgr)
			pRelayGroupInfoMgr->UpdateUserSPPotion( pSPInfo->m_dwRoomIndex, pSPInfo->m_dwUserIndex );

		m_bufferPool.Push(reinterpret_cast<char*>(pRelayHeader), sizeof(UpdateRelayGroupSpData));
	}
}

void ioBroadCastRelayModule::UpdateDieState( const DWORD dwRoomIndex, const DWORD dwUserIndex, const BOOL bDieState )
{
	UpdateDieStateData* pData = reinterpret_cast<UpdateDieStateData*>(m_bufferPool.Get(sizeof(UpdateDieStateData)));
	if(pData == NULL)
	{
		return;
	}

	pData->m_packetId = UDP_USER_DIESTATE;
	pData->m_dwRoomIndex = dwRoomIndex;
	pData->m_dwUserIndex	 = dwUserIndex;
	pData->m_bDieState = bDieState;

	EnqueueByIndex(dwRoomIndex, pData);

	IncementInputCount();
}

void ioBroadCastRelayModule::OnUpdateDieState( RelayHeader* pRelayHeader, ioRelayGroupInfoMgr* pRelayGroupInfoMgr )
{
	if(pRelayHeader)
	{
		UpdateDieStateData* pState = reinterpret_cast<UpdateDieStateData*>(pRelayHeader);

		if(pRelayGroupInfoMgr)
			pRelayGroupInfoMgr->UpdateDieState(pState->m_dwRoomIndex,pState->m_dwUserIndex,pState->m_bDieState);

		m_bufferPool.Push(reinterpret_cast<char*>(pState), sizeof(UpdateDieStateData));
	}
}

#endif

