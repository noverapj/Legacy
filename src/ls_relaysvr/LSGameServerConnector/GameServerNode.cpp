#include "StdAfx.h"
#include "GameServerNode.h"


GameServerNode::GameServerNode() : CConnectNode(INVALID_SOCKET,DEFAULT_BUFFER,DEFAULT_BUFFER*2+1)
{
	InitData();
}

GameServerNode::GameServerNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode(s,dwSendBufSize,dwRecvBufSize)
{
	InitData();
}

GameServerNode::~GameServerNode(void)
{

}

void GameServerNode::InitData()
{
	m_relayServerIndex = 0;
	m_serverIndex = 0;
	m_currentTime = 0;
	m_maxuserCount = 0;
	m_roomMaxCount = 0;
	m_currentUserCount= 0;
	m_wholeChatState = false;
	m_mineId = 0;
	m_ipAddr = "";
	m_port = 0;
	m_sendServerId = 0;
}

void GameServerNode::ReleaseData()
{
	POSITION pos = m_relayGroups.GetHeadPosition();
	while(pos)
	{
		RelayGroup* relayGroup = m_relayGroups.GetAt(pos);

		if(relayGroup)
		{
			m_relayGroupPool.Push(relayGroup);
			m_relayGroups.GetNext(pos);
		}
	}

	while(1)
	{
		RelayGroup* relayGroup = m_relayGroupPool.Pop();

		if(relayGroup)
			delete relayGroup;

		else
		{
			LOG.PrintTimeAndLog(0,"Destory Pool Success");
			break;
		}

	}
	m_relayGroupPool.DestroyPool();
	 
	m_relayGroups.RemoveAll();	
}

bool GameServerNode::ConnectTo(std::string serverIP,int& serverPort)
{
	if(SetServerAddress(serverIP, serverPort) == false)
		return false;

	if(::connect(GetSocket(),(sockaddr*)&m_serverAddress,sizeof(m_serverAddress)) != 0)
	{
		LOG.PrintTimeAndLog(0,"LsConnector::Connect to socket %d[%s:%d]",GetLastError(), serverIP.c_str(), serverPort);
		bool reuse = true;
		::setsockopt(GetSocket(),SOL_SOCKET,SO_REUSEADDR,(TCHAR*)&reuse,sizeof(reuse));
		closesocket(GetSocket());
		return false;
	}

	LOG.PrintTimeAndLog(0,"GameServer OnConnect (IP : %s port %d)", serverIP.c_str(), serverPort);
	return true;
}

void GameServerNode::OnCreate()
{
	CConnectNode::OnCreate();

	m_currentTime = TIMEGETTIME();
	m_roomMaxCount = g_Config()->GetRoomMaxCount();//접속 될때마다 하니 부하가 좀있음 이걸 초기에 다 해버리는 방법?? 아니면 버퍼 풀을 싱글톤으로?
	
    for(int i=0; i<m_roomMaxCount * 2; ++i)
    {
    	RelayGroup* relayGroup = InitRelayGroup();

		if(relayGroup)
    		m_relayGroupPool.Push(relayGroup);
   	}	
}

bool GameServerNode::AfterCreate()
{
	g_Iocp()->AddHandleToIOCP( (HANDLE)GetSocket(), (DWORD)this );

	CConnectNode::SetSocket( GetSocket() );

	return CConnectNode::AfterCreate();
}

void GameServerNode::OnDestroy()
{
	ReleaseData();

	CConnectNode::OnDestroy();

	LOG.PrintTimeAndLog(0,"GameServerNode : Disconnect");	
}

void GameServerNode::SessionClose( BOOL safely/* =TRUE */ )
{
	if(IsActive())
	{
		CPacket packet( Protocols::ITPK_CLOSE_SESSION );
		ReceivePacket( packet );
	}
}

bool GameServerNode::SendMessage(CPacket& rkPacket)
{
	return CConnectNode::SendMessage(rkPacket);
}

void GameServerNode::ReceivePacket( CPacket &packet )
{
	g_Queue()->InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void GameServerNode::PacketParsing( CPacket &packet )
{
	FUNCTION_TIME_CHECKER( 100000.0f, packet.GetPacketID() );  

	SP2Packet &kPacket = reinterpret_cast<SP2Packet&>(packet);

	switch(kPacket.GetPacketID())
	{
	case Protocols::ITPK_CLOSE_SESSION:
		{
			OnClose((SP2Packet&)packet);
		}
		break;

	case Protocols::RSPTK_ON_CONNECT:
		{
			OnConnect(kPacket);
		}
		break;

	case Protocols::RSPTK_ON_CONTROL:
		{
			OnRelayControl(kPacket);		
		}
		break;
	}
}

bool GameServerNode::CheckNS( CPacket &rkPacket )
{
	return true;
}

int GameServerNode::GetConnectType()
{
	return NodeTypes::CONNECTOR;
}

void GameServerNode::OnConnect( SP2Packet & kPacket )
{
	kPacket >> m_wholeChatState;
	kPacket >> m_relayServerIndex;
	kPacket >> m_serverIndex;

	GetPeerIP(m_public_ip,16,m_port);

	LOG.PrintTimeAndLog(0,"(WholechatState %s)OnConnect RelayServerIndex : %d GameServerIndex : %d(%s:%d)",m_wholeChatState ? "TRUE":"FALSE",m_relayServerIndex,m_serverIndex,m_public_ip,m_port);
}

void GameServerNode::OnClose( SP2Packet &packet )
{	
	g_UDPNode()->DelUserInfoByServerID(m_serverIndex);
	g_ServerConnectMgr()->DelClient(this);
	OnDestroy();
}

void GameServerNode::OnRelayControl( SP2Packet & kPacket )
{
	int cType;
	kPacket >> cType;

	switch(cType)
	{
	case ControlTypes::RS_INFO:
		{
			printf("RS_INFO\n");
		}
		break;

	case ControlTypes::RS_ADD_USER:
		{
			AddUser(kPacket); //안쓰는 기능 
		}
		break;

	case ControlTypes::RS_INSERT_GROUP:
		{
		 
			SendRelayInsertData insertData;
			kPacket >> insertData;

			Debug("(S:%d)InSertRoom User: %d Room :%d\n",m_serverIndex,insertData.m_dwUserIndex,insertData.m_dwRoomIndex);
			LOG.PrintTimeAndLog(0,"(S:%d)InSertRoom User: %d Room :%d\n",m_serverIndex,insertData.m_dwUserIndex,insertData.m_dwRoomIndex);

			InsertRoom(insertData);
		}
		break;

	case ControlTypes::RS_REMOVE_GROUP: // USerIndex == 0 이면 방삭제 
		{
			RemoveData removeData;
			kPacket >> removeData;

			LOG.PrintTimeAndLog(0,"(S:%d)RS_REMOVE_GROUP User: %d Room :%d\n",m_serverIndex,removeData.m_dwUserIndex,removeData.m_dwRoomIndex);
			Debug("(S:%d)RS_REMOVE_GROUP User: %d Room :%d\n",m_serverIndex,removeData.m_dwUserIndex,removeData.m_dwRoomIndex);
	
			RemoveRoom(removeData);
		}
		break;

	case ControlTypes::RS_SEND_PACKET:
		{
			printf("RS_SEND_PACKET\n");
		}
		break;

	case ControlTypes::RS_DEL_USER:
		{
			OnDelUser(kPacket);
		}
		break;

	case ControlTypes::RS_WHOLECHAT_STATE:
		{
			kPacket >> m_wholeChatState;
		}
		break;

	case ControlTypes::RS_CHANGE_NICKNAME:
		{
			OnChangeNickName(kPacket);
		}
		break;
	}
}

void GameServerNode::AddUser( SP2Packet & kPacket ) //안쓰는 기능 
{
	int serverIndex;
	int userIndex;
	char publicID[PUBLICID_MAX];
	char public_IP[STR_IP_MAX];       //사용자 외부 아이피
	int  clientport;     //사용자 내부 아이피

	kPacket >> serverIndex;
	m_serverIndex = serverIndex;
	kPacket >> publicID;
	kPacket>> public_IP;
	kPacket >> clientport;
	kPacket >> userIndex;

	//받은 다음애 게임서버에게 AcK를 준후 그 이후에 게임서버에서 릴레이 서버의 정보를 주도록 해야함 
	g_UDPNode()->InsertUserInfo(userIndex,publicID,serverIndex,public_IP,clientport);

	SendOnAddUser(publicID); 

}

void GameServerNode::OnDelUser( SP2Packet & kPacket )
{
	DWORD userIndex;
	kPacket >> userIndex;

	Debug("DelUser(%d)\n",userIndex);

	g_UDPNode()->DelUserInfo(userIndex,	m_serverIndex);

	RelayGroup* relayGroup = GetRelayGroupByUser(userIndex);

	if(relayGroup == NULL) 	return; 

	RelayGroup::RelayGroups* users = relayGroup->GetUserLists();

	if(users == NULL)
	{
		LOG.PrintTimeAndLog(0,"Error OnDelUser users NULL");
		return;
	}
	
	for(int i = 0; i < users->size(); ++i)
	{
		UserData& userData = users->at(i);

		if(userData.m_dwUserIndex == userIndex)
		{
			users->erase(users->begin() + i);
			break;
		}
	}
}

void GameServerNode::OnChangeNickName( SP2Packet & kPacket )
{
	DWORD userIndex;
	char newPublicID[PUBLICID_MAX];
	kPacket >> userIndex;
	kPacket >> newPublicID;
	UserInfo* uData = g_UDPNode()->GetUserInfo(userIndex);
	strcpy_s(uData->m_szPublicID,newPublicID);
}

void GameServerNode::SendOnAddUser( char * publicID )
{
	SP2Packet pk(Protocols::RSPTK_ON_CONTROL);
	int rctype = ControlTypes::RS_ON_ADD_USER;

	pk << rctype;
	pk << publicID;

	SendMessage(pk);
}

void GameServerNode::ChangeUserAddr( const char* ipAddr, int port, DWORD userIndex )
{
	RelayGroup* relayGroup = GetRelayGroupByUser(userIndex);
	if(relayGroup == NULL)
	{
		LOG.PrintTimeAndLog(0,"ChangeUserAddr::Error UserIndex %d (%s:%d)",userIndex,ipAddr,port);
		return;
	}

	RelayGroup::RelayGroups* users = relayGroup->GetUserLists();
	
	if(users == NULL) 
	{
		LOG.PrintTimeAndLog(0,"ChangeUserAddr::Error Users is NULL");
		return;
	}

	for(UINT i=0; i<users->size(); ++i)
	{
		UserData& userData = users->at(i);

		if(userData.m_dwUserIndex == userIndex)
		{
			if(userData.m_iClientPort != port )
				LOG.PrintTimeAndLog(0,"CahngeUser Addr Orig :%s:%d to New Addr : %s:%d",userData.m_szPublicIP,userData.m_iClientPort,ipAddr,port);

			strcpy_s(userData.m_szPublicIP,ipAddr);
			userData.m_iClientPort= port;

			break;
		}
	}
}

bool GameServerNode::SetServerAddress( std::string &serverIP, int& serverPort )
{
	m_ipAddr	= serverIP;
	m_port		= serverPort;
	SOCKET fd = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	SetSocket(fd);
	
	if(fd == INVALID_SOCKET)
	{
		LOG.PrintTimeAndLog(0,"LsConnector::Connect to socket %d[%s:%d]",GetLastError(), serverIP.c_str(), serverPort);
		return false;
	}

	ZeroMemory(&m_serverAddress,sizeof(m_serverAddress));
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_addr.s_addr = inet_addr(serverIP.c_str());
	m_serverAddress.sin_port = htons(serverPort);

	return true;
}

RelayGroup* GameServerNode::InitRelayGroup()
{
	RelayGroup* rgTmp = new RelayGroup;

	rgTmp->m_dwRoomIndex = -1;
	rgTmp->m_RelayUserList.reserve(16);

	return rgTmp;
}

#if 0

BOOL GameServerNode::SendRelayPacket( DWORD dwUserIndex,SP2Packet& sppacket )
{
	RelayGroup* rgTmp = GetRelayGroupByUser( dwUserIndex );

	if(rgTmp)
	{
		RelayGroup::RelayGroups* users = rgTmp->GetUserLists();

		for(unsigned int i=0; i<users->size(); ++i)
		{
			UserData& userData = users->at(i);
			if(userData.m_dwUserIndex == dwUserIndex)
			{
				continue;
			}

			g_UDPNode()->SendMessage(userData.m_szPublicIP  ,userData.m_iClientPort,sppacket);
		}

		return TRUE;
	}

	return FALSE; //추가 부분 
}
#endif

RelayGroup* GameServerNode::GetRelayGroupByRoom( DWORD dwRoomIndex )
{
	POSITION pos = m_relayGroups.GetHeadPosition();

	while(pos)
	{
		RelayGroup* relayGroup = m_relayGroups.GetAt(pos);

		if(relayGroup)
		{
			if(relayGroup->m_dwRoomIndex == dwRoomIndex)
			{
				return relayGroup;
			}
			m_relayGroups.GetNext(pos);
		}
	}

	return NULL;
}

RelayGroup* GameServerNode::GetRelayGroupByUser( DWORD dwUserIndex ) //유저 검색 
{
	POSITION pos = m_relayGroups.GetHeadPosition();

	while(pos)
	{
		RelayGroup* relayGroup = m_relayGroups.GetAt(pos);
		if(relayGroup == NULL) return NULL;

		RelayGroup::RelayGroups* users = relayGroup->GetUserLists();
		if(users == NULL) return NULL;

		for(unsigned int i=0; i<users->size(); ++i)
		{
			UserData& user = users->at(i);
			if(user.m_dwUserIndex == dwUserIndex)
			{
				return relayGroup;
			}
		}
		m_relayGroups.GetNext(pos);
	}
	return NULL;
}

RelayGroup* GameServerNode::CreateRelayGroup()
{
	RelayGroup* relayGroup = m_relayGroupPool.Pop();

	if(relayGroup)
	{
		relayGroup->Init();
		return relayGroup;
	}
	else
	{
		relayGroup = new RelayGroup;
		relayGroup->Init();
		LOG.PrintTimeAndLog(0,"CreateRelayGroup NULL");
	}

	return relayGroup;
}

void GameServerNode::InsertRoom( SendRelayInsertData& inData )
{
	RelayGroup *relayGroup = GetRelayGroupByRoom( inData.m_dwRoomIndex );

	if( relayGroup )
	{
		bool bChange = false;

		RelayGroup::RelayGroups* users = relayGroup->GetUserLists();
		if(users == NULL) return;

		for(unsigned int i=0; i<users->size(); i++)
		{
			UserData &rkUser = users->at(i);
			if( rkUser.m_dwUserIndex == inData.m_dwUserIndex )
			{
				bChange = true;   // 바뀌었다.
				strcpy_s(rkUser.m_szPublicIP,inData.m_szPublicIP);
				rkUser.m_iClientPort= inData.m_iClientPort;
				strcpy_s(rkUser.m_szPublicID,inData.m_szPublicID);

				g_UDPNode()->SetUserRoomInfo(rkUser,1,m_serverIndex);
			
				break;
			}
		}

		if( !bChange )
		{
			// 유저 입장
			UserData kUser;
			kUser.m_dwUserIndex = inData.m_dwUserIndex;
			strcpy_s(kUser.m_szPublicIP, inData.m_szPublicIP);
			kUser.m_iClientPort = inData.m_iClientPort; 
			strcpy_s(kUser.m_szPublicID, inData.m_szPublicID);

			relayGroup->m_RelayUserList.push_back( kUser ); //아이피가 안들어감 

			g_UDPNode()->SetUserRoomInfo(kUser,1,m_serverIndex);
		
		}
	}	

	else
	{
		// 생성
		relayGroup = CreateRelayGroup();
		if( relayGroup )
		{
			relayGroup->m_dwRoomIndex = inData.m_dwRoomIndex;

			UserData kUser;
			kUser.m_dwUserIndex = inData.m_dwUserIndex;
			strcpy_s(kUser.m_szPublicIP , inData.m_szPublicIP);
			strcpy_s(kUser.m_szPublicID, inData.m_szPublicID);
			kUser.m_iClientPort = inData.m_iClientPort;

			relayGroup->m_RelayUserList.push_back( kUser );

			m_relayGroups.AddTail( relayGroup );

			g_UDPNode()->SetUserRoomInfo(kUser,1,m_serverIndex);
		}
		else
			LOG.PrintTimeAndLog(0,"CreateRelayGroup Fail MemoryPool Zero");
	 
		g_State()->IncrementRoomCount();
	}
	
	Debug("(S:%d)RS_ON_ADD_USER :: %s\n",m_serverIndex,inData.m_szPublicID);
	LOG.PrintTimeAndLog(0,"(S:%d)RS_ON_ADD_USER :: %s\n",m_serverIndex,inData.m_szPublicID);

	SendOnAddUser(inData.m_szPublicID);
}

void GameServerNode::RemoveRoom( RemoveData& rmData )
{
	if( rmData.m_dwUserIndex == 0 )
	{
		// 룸 통째로 삭제
		RemoveRelayGroup( rmData.m_dwRoomIndex );
	}
	else
	{
		// 유저만 삭제
		RelayGroup *relayGroup = GetRelayGroupByRoom( rmData.m_dwRoomIndex );

		if(relayGroup == NULL) 	return;

		RelayGroup::RelayGroups* users = relayGroup->GetUserLists();
		for(unsigned int i=0; i<users->size(); ++i)
		{
			UserData& userTmp = users->at(i);
			if( userTmp.m_dwUserIndex == rmData.m_dwUserIndex )
			{		 
				users->erase(users->begin() + i);
				//kyg 여기에 udpnode의 유저중 룸삭제 
				g_UDPNode()->SetUserRoomInfo(rmData.m_dwUserIndex,0);
				break;
			}
		}
	}
}

void GameServerNode::RemoveRelayGroup( DWORD dwRoomIndex )
{
	POSITION pos = m_relayGroups.GetHeadPosition();

	while(pos)
	{
		RelayGroup *rgtmp = m_relayGroups.GetAt(pos);
		if( rgtmp->m_dwRoomIndex == dwRoomIndex )
		{
			rgtmp->m_dwRoomIndex = 0;

			rgtmp->m_RelayUserList.clear();
			m_relayGroups.RemoveAt(pos);

			m_relayGroupPool.Push(rgtmp);
			return;
		}
		m_relayGroups.GetNext(pos);
	}

	LOG.PrintTimeAndLog(0,"(%d)Error RemoveRelayGroup Fail(%d)",m_serverIndex,dwRoomIndex);
}




