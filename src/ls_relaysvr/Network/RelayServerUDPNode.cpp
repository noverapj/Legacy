#include "stdafx.h"
#include "RelayServerUDPNode.h"
#include "SP2Packet.h"


RelayServerUDPNode::RelayServerUDPNode(void)
{
	Init();
	InitMemoryPool();
}

RelayServerUDPNode::~RelayServerUDPNode(void)
{
}

void RelayServerUDPNode::Init()
{
	SetActive( true );
	SetNS(new ioUDPSecurity);
	m_packet = new SP2Packet;
	m_inputCount = 0;
	m_processCount = 0;
	m_SendLogicCount = 0;
	m_maxSize = 0;
	m_dwNodeGhostCheckTime = 0;
}

void RelayServerUDPNode::InitMemoryPool()
{
	m_UserInfoPool.CreatePool(g_Config()->GetUserInfoMax(),20000);
}

void RelayServerUDPNode::Destroy()
{
	for(UINT i=0; i<m_recvInfos.size(); i++)
	{
		bool reuse = true;
		::setsockopt(m_recvInfos[i]->fd,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(reuse));
		CloseConnection(i);
	}
}

void RelayServerUDPNode::SessionClose( int index )
{
	if(IsActive())
	{

	}
}

void RelayServerUDPNode::ReceivePacket( CPacket &packet, int index )
{
	char rcv_ip[16] = "";
	int port = 0;

	port = MakeIpAddres(rcv_ip, m_recvInfos[index], port);

	SendLogic(m_recvInfos[index], packet);
}

void RelayServerUDPNode::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;
	FUNCTION_TIME_CHECKER( 100000.0f, kPacket.GetPacketID() );  

	char ipAddr[STR_IP_MAX];
	int port;
	sockaddr_in addr;

	IncrementProcessCount();

	kPacket.GetSockAddress(addr);

	SetCurrentIndex(addr);
	//Debug("CurrentInfo :%d\n",m_currentIndex);

	
	MakeIpAddres(ipAddr,addr,port);

	switch(kPacket.GetPacketID())
	{
	case Protocols::CUPK_CONNECT :
		{
			OnConnect(kPacket, ipAddr, port);
			Debug("CUDPCONNECT :%s:%d\n",ipAddr,port);
		}
		break;

	case Protocols::CUPK_SYNCTIME: // 릴레이 서버에서 
		{
			OnSyncTime(kPacket, ipAddr, port);
		}
		break;

	case Protocols::CUPK_RESERVE_ROOM_JOIN: // 게임 서버에게 넘김 넘기고 
		{
			OnReserveRoomJoin(kPacket, ipAddr, port);	 
		}
		break;

	case Protocols::CUPK_CHECK_KING_PING: //쓰지않는 핑 
		{  
		}
		break;
#if 1
	case 0x4500: // 테스트용 
		{
			SP2Packet rpk(0x4005);
			// Debug("R:%d",packet.GetBufferSize());
			 SendMessage(ipAddr,port,rpk);
		}
		break;
#endif 
	default:
		//여기서 서버 인덱스에 따라 
		{
			OnDefaultPacket(kPacket);
		}
		break;
	}
}

BOOL RelayServerUDPNode::SetNetworkSecurity(int i)
{
	SetNS(new ioUDPSecurity,i);
	return TRUE;
}

int RelayServerUDPNode::GetConnectType()
{
	return 0;
}

void RelayServerUDPNode::OnSyncTime( SP2Packet & kPacket, char * ipAddr, int port )
{
	char publicID[PUBLICID_MAX];
	kPacket >> publicID;

	UserInfo* userInfo = GetUserInfo(publicID);

	if(userInfo)
	{
		SendChangeIPMessage(userInfo, ipAddr, port,publicID);
	}
	else
	{
		Debug("OnSyncTime GetUserInfo Fail!!(%s)\n",publicID);
		LOG.PrintTimeAndLog(0,"OnSyncTime GetUserInfo Fail!!(%s)",publicID);
		return;
	}

	WORD client_time = 0;
	int send_total_index = -1;
	bool bNowWorkPing = false;

	kPacket >> client_time;
	kPacket >> send_total_index;
	kPacket >> bNowWorkPing;

	DWORD current_time = TIMEGETTIME();
	DWORD gap_time = current_time - userInfo->m_sync_time;

	OnCheckPingStep( userInfo, client_time );

	if( send_total_index > 0 )
	{
		if( userInfo->m_ping_total_send_index == send_total_index )
		{
			HackLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ERROR : %s ping duplicated(%d:%d:%d)",
				userInfo->m_szPublicID,
				userInfo->m_ping_total_send_index,
				send_total_index,
				gap_time );
			return;
		}
		else if( userInfo->m_ping_total_send_index > send_total_index )
		{
			HackLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ERROR : %s ping out of ordering(%d:%d:%d)",
				userInfo->m_szPublicID,
				userInfo->m_ping_total_send_index,
				send_total_index,
				gap_time );
			return;
		}
	}
	GameServerNode* node = g_ServerConnectMgr()->GetServerNodeByID(userInfo->m_serverID);

	if(node == NULL)
	{
		LOG.PrintTimeAndLog(0,"Error OnSynceServer Is NULL");
		Debug("Error OnSynceServer Is NULL");
	}

	SP2Packet pk(Protocols::SUPK_SYNCTIME);

	pk << node->WholeChatState() << current_time;

	 SendCurrentPortMessage( userInfo->m_szPublicIP, userInfo->m_iClientPort, m_currentIndex, pk );

	if( !userInfo->m_first_heart_beat )
	{
		if( userInfo->m_iRoomState == 1 && userInfo->m_dwSpeedHackQuizLimitTime == 0 ) // 입장데이터밖에없을탠데.. 어떻게 검사해야하나 ? udpnode와 gameservernode간의 데이터 공유가 필요함 .
		{
			// 스피드핵 점검
			if( gap_time < 200 )
			{
				if(userInfo->m_ping_total_send_index != 0)
				{
					HackLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ERROR : %s Ping Gap(%d) too small, (%d/%d)",
						userInfo->m_szPublicID,
						gap_time,
						userInfo->m_ping_total_send_index,
						send_total_index );
				}
			}
			else if( gap_time < HackCheck::SH_LessCheckTime() )
			{
				if( userInfo->m_prev_over_ping_time > 0 )
				{
					DWORD dwDoubleGapTime = ( userInfo->m_prev_over_ping_time + SYNC_TIME_GAP + gap_time ) / 2;

					if( dwDoubleGapTime < HackCheck::SH_LessCheckTime() )
					{
						userInfo->m_ping_less_error_count++;
						userInfo->m_total_ping_error_count++;
					}
				}
				else
				{
					userInfo->m_ping_less_error_count++;
					userInfo->m_total_ping_error_count++;
				}
			}
			else if( gap_time > HackCheck::SH_OverCheckTime() )
			{
				// 중간에 핑이 소실되지 않았다.
				if( userInfo->m_ping_total_send_index + 1 == send_total_index  && !bNowWorkPing )
				{
					userInfo->m_ping_over_error_count++;
					userInfo->m_total_ping_error_count++;
				}
				else
				{
					// 중간 핑소실로 인한 것이라서 봐준다..
				}
			}
			else	// 정상 범위
			{
				userInfo->m_ping_less_error_count = 0;
				userInfo->m_ping_over_error_count = 0;
			}			
		}
	}
	else	// 접속이후 최초 PingMessage..
	{
		userInfo->m_ping_less_error_count = 0;
		userInfo->m_ping_over_error_count = 0;
		userInfo->m_prev_over_ping_time = 0;
		userInfo->m_first_heart_beat = false;
	}

	userInfo->m_sync_time = current_time;
	userInfo->m_ping_total_send_index = send_total_index;

	if( gap_time > SYNC_TIME_GAP )
		userInfo->m_prev_over_ping_time = gap_time - SYNC_TIME_GAP;
	else
		userInfo->m_prev_over_ping_time = 0;

	if( userInfo->m_iRoomState == 1 && userInfo->m_dwSpeedHackQuizLimitTime == 0 )
	{
		if( CheckHackCount( userInfo, gap_time ) )
		{
			userInfo->m_SpeedHackQuiz = HackCheck::GenerateProblem( HackCheck::HT_SPEED );
			userInfo->m_dwSpeedHackQuizLimitTime  = TIMEGETTIME() + HackCheck::ServerAnswerTime( HackCheck::HT_SPEED );
			userInfo->m_iCurSpeedHackAnswerChance = 0;

			SP2Packet kPacket( Protocols::RSPTK_ON_CONTROL );
			int ctype = ControlTypes::RS_HACK_ANNOUNCE;
			kPacket << ctype;
			kPacket << userInfo->m_dwUserIndex;
			kPacket << (int)HackCheck::HT_SPEED;
			kPacket << HackCheck::MaxAnswerChance( HackCheck::HT_SPEED );
			kPacket << HackCheck::ClientAnswerTime( HackCheck::HT_SPEED );
			kPacket << userInfo->m_SpeedHackQuiz.m_iFirstOperand;
			kPacket << userInfo->m_SpeedHackQuiz.m_iSecondOperand;
			kPacket << (int)userInfo->m_SpeedHackQuiz.m_Operator;
			node->SendMessage( kPacket );
			Debug("HACK::userindex:%d FirstOperand:%d\n",userInfo->m_dwUserIndex,userInfo->m_SpeedHackQuiz.m_iFirstOperand);

			userInfo->m_ping_less_error_count  = 0;
			userInfo->m_ping_over_error_count  = 0;
			userInfo->m_total_ping_error_count = 0;
		}

	}
	//Debug("OnUserSync ping Step : %d\n",userInfo->m_dwPingStep);
}

void RelayServerUDPNode::OnConnect( SP2Packet & kPacket, char * ipAddr, int port )
{
	char publicID[PUBLICID_MAX];
	kPacket >> publicID;

	UserInfo* userInfo = GetUserInfo(publicID);

	if(userInfo)
	{
		userInfo->m_sync_time = TIMEGETTIME();
		userInfo->m_ping_total_send_index = 0;

		if(strcmp(ipAddr,userInfo->m_szPublicIP) != 0 || port != userInfo->m_iClientPort)
			SendChangeIPMessage(userInfo, ipAddr, port,publicID);
		//		LOG.PrintTimeAndLog(0,"OnConnect :%s (%s:%d)",userInfo->m_szPublicID,userInfo->m_szPublicIP,userInfo->m_iClientPort);
	}
	else
	{
		Debug("OnConnect GetUserInfo Fail!!(%s)\n",publicID);
		LOG.PrintTimeAndLog(0,"OnConnect GetUserInfo Fail!!(%s)",publicID);
	}

	SP2Packet kReturn(Protocols::SUPK_CONNECT);
	SendCurrentPortMessage(ipAddr,port,m_currentIndex,kReturn);
}

void RelayServerUDPNode::OnDefaultPacket( SP2Packet &kPacket )
{
	DWORD dwIP, dwPort;

	kPacket >> dwIP >> dwPort;

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
		SendRelayPacket( dwUserIndex, kRelayPacket );
		return;
	}
	else if( dwIP == g_Config()->GetDWIP() && dwPort == (DWORD)g_Config()->GetPort() ) 
		return;

	char szIP[16] = "";

	sprintf_s( szIP, "%d.%d.%d.%d", (dwIP & 0xff000000)>>24, (dwIP & 0x00ff0000)>>16, (dwIP & 0x0000ff00)>>8,	(dwIP & 0xff) );
	Debug("Send :%s:%d\n",szIP,dwPort);

	g_UDPNode()->SendCurrentPortMessage( szIP, dwPort, m_currentIndex, kPacket);

}

void RelayServerUDPNode::OnReserveRoomJoin( SP2Packet & kPacket, char * ipAddr, int port )
{
	char publicID[PUBLICID_MAX];
	int roomIndex = 0;

	kPacket >> publicID;
	kPacket >> roomIndex;

	UserInfo* userInfo = GetUserInfo(publicID);

	if(userInfo)
		SendChangeIPMessage(userInfo, ipAddr, port,publicID);

	else
	{
		LOG.PrintTimeAndLog(0,"RS_RESERVER_ROOM_JOIN Error UserID Not Found(%s)",publicID);
		return;
	}

	SP2Packet pk(Protocols::RSPTK_ON_CONTROL);
	int ctype = ControlTypes::RS_RESERVER_ROOM_JOIN;

	pk << ctype;
	pk << userInfo->m_dwUserIndex;
	pk << roomIndex;

	GameServerNode* node = g_ServerConnectMgr()->GetServerNodeByID(userInfo->m_serverID);

	if(node)
		node->SendMessage(pk);
	else
		LOG.PrintTimeAndLog(0,"RS_RESERVER_ROOM_JOIN Error Server ID Not Found(%d:%s)",userInfo->m_serverID,publicID);
}


void RelayServerUDPNode::OnCheckKingPing(SP2Packet& kPacket, const char* ipAddr, const int port)
{
	char publicID[PUBLICID_MAX];
	kPacket >> publicID;

	UserInfo* userInfo = GetUserInfo(publicID);

	if(userInfo)
		SendChangeIPMessage(userInfo, ipAddr, port,publicID);

	else
		LOG.PrintTimeAndLog(0,"OnCheckKingPing public Not Found(%s)",publicID);
}

void RelayServerUDPNode::OnCheckPingStep( UserInfo* userInfo, DWORD dwClientTime )
{
	if( userInfo->m_first_heart_beat || dwClientTime == 0 )
	{
		userInfo->m_dwPingStep = 0;
	}
	else
	{
		int iGapTime = abs( (int)TIMEGETTIME() - (int)dwClientTime );
		userInfo->m_dwPingStep = iGapTime / PING_MS_VALUE;
	}	
}

bool RelayServerUDPNode::CheckHackCount( UserInfo* userData, DWORD dwCurGap )
{
	if( HackCheck::SH_LessCount() > 0 )
	{
		if( userData->m_ping_less_error_count >= HackCheck::SH_LessCount() )
		{
			PrintHackLog( userData, dwCurGap );
			return true;
		}
	}

	if( HackCheck::SH_OverCount() > 0 )
	{
		if( userData->m_ping_over_error_count >= HackCheck::SH_OverCount() )
		{
			PrintHackLog( userData, dwCurGap );
			return true;
		}
	}

	if( HackCheck::SH_LessOverCount() > 0 )
	{
		if( userData->m_ping_less_error_count + userData->m_ping_over_error_count >= HackCheck::SH_LessOverCount() )
		{
			PrintHackLog( userData, dwCurGap );
			return true;
		}
	}

	if( HackCheck::SH_TotalCount() > 0 )
	{
		if( userData->m_total_ping_error_count >= HackCheck::SH_TotalCount() )
		{
			PrintHackLog( userData, dwCurGap );
			return true;
		}
	}

	return false;

}

void RelayServerUDPNode::PrintHackLog( UserInfo* userData, DWORD dwCurGap )
{
	HackLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "HACK : %s(%d) Less:%d, Over:%d, Total:%d", 
		userData->m_szPublicID,
		dwCurGap,
		userData->m_ping_less_error_count,
		userData->m_ping_over_error_count,
		userData->m_total_ping_error_count );
}

void RelayServerUDPNode::SendTimeMessage( UserInfo * userData, int sendTime )
{
	SP2Packet pk(Protocols::RSPTK_ON_CONTROL);
	int ctype = ControlTypes::RS_USER_GHOST;

	pk << ctype;
	pk << userData->m_dwUserIndex;
	pk << sendTime;

	GameServerNode* node = g_ServerConnectMgr()->GetServerNodeByID(userData->m_serverID);
	if(node)
		node->SendMessage(pk);
}

void RelayServerUDPNode::GhostCheck() // ini 뺄것 생각할것
{
	 //5초에 한번씩 
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	std::vector<DWORD> removeUserInfos;
	USERINFO::iterator pos = m_userInfoMap.begin();

	while(pos != m_userInfoMap.end())
	{
		UserInfo *userData = (*pos).second;

		if(userData == NULL) break;

		if( userData->m_sync_time <= 0 ) //첫접속 이후 Syntime을 보내지않음 
		{
			userData->m_firstState++;
			if(userData->m_firstState >= 30) //어떻게 처리할지 //일단은 접속 종료시킴 
			{
				SendTimeMessage(userData,TimeTypes::MIN_5);
			}
			pos++;
			continue;
		}

		DWORD checkTime = TIMEGETTIME() - userData->m_sync_time;
		userData->m_firstState = 0;
		if( checkTime >= TimeTypes::SEC_30 && checkTime < TimeTypes::SEC_60)
		{
		}

		else if( checkTime >= TimeTypes::SEC_60 && checkTime < TimeTypes::SEC_90)
		{
			Debug("SEC_60 SendUserCloseMessage[id:%s]\n",userData->m_szPublicID);
			LOG.PrintTimeAndLog(0,"SEC_60 SendUserCloseMessage[id:%s]\n",userData->m_szPublicID);

			SendTimeMessage(userData,TimeTypes::SEC_60);
		}

		else if(checkTime >= TimeTypes::SEC_90 && checkTime < TimeTypes::MIN_5)
		{
			Debug("SEC_90 SendUserCloseMessage[id:%s]\n",userData->m_szPublicID);
			LOG.PrintTimeAndLog(0,"SEC_90 SendUserCloseMessage[id:%s]\n",userData->m_szPublicID);

			SendTimeMessage(userData,TimeTypes::SEC_90);
		}

		else if(checkTime >= TimeTypes::MIN_5)
		{
			Debug("MIN_5 SendUserCloseMessage[id:%s]\n",userData->m_szPublicID);
			LOG.PrintTimeAndLog(0,"MIN_5 SendUserCloseMessage[id:%s]\n",userData->m_szPublicID);

			SendTimeMessage(userData,TimeTypes::MIN_5);
			removeUserInfos.push_back(userData->m_dwUserIndex);

		}
		pos++;
	}
	m_dwNodeGhostCheckTime = TIMEGETTIME();

	for(UINT i=0; i<removeUserInfos.size(); ++i)
	{
		DelUserInfo(removeUserInfos[i]);
	}
}

void RelayServerUDPNode::ModePingCheck()
{
	USERINFO::iterator pos = m_userInfoMap.begin();

	while(pos != m_userInfoMap.end())
	{
		 UserInfo* userData = (*pos).second;

		 if(userData == NULL)  break;

		 switch(userData->m_modeType)
		 {
		 case ModeTypes::MT_KING:
			 {
				 userData->m_iKingPing++;

				 if(userData->m_iKingPing > 6) // 30초 이상임 이것 셋팅 해야할것 
				 {
					 SP2Packet pk(Protocols::RSPTK_ON_CONTROL);

					 int ctype = ControlTypes::RS_USER_MODE_PING_ROW;

					 pk << userData->m_dwUserIndex;

					 GameServerNode* node = g_ServerConnectMgr()->GetServerNodeByID(userData->m_serverID);

					 if(node)
						 node->SendMessage(pk);

				 }
			 }
			 break;
		 }
		pos++;
	}
}

BOOL RelayServerUDPNode::SendMessage( const char* ip,int port, CPacket& rkPacket )
{
	return UDPNode::SendMessage(ip,port,rkPacket);
}

BOOL RelayServerUDPNode::SendMessage( const char* ip,int port, const char* buffer, const int size )
{
	return UDPNode::SendMessage(ip,port,buffer,size);
}

void RelayServerUDPNode::SendLogic( const UDPIoInfo* recvinfo, CPacket &packet )
{
	SP2Packet& rpacket = static_cast<SP2Packet&>(packet);
	rpacket << recvinfo->addr;
	g_Queue()->InsertQueue( (DWORD)this, packet, PK_QUEUE_UDP );
}

int RelayServerUDPNode::MakeIpAddres( char* rcv_ip, sockaddr_in& addr, int& port )
{
	sprintf_s(rcv_ip, 16,"%d.%d.%d.%d",
		addr.sin_addr.s_net,
		addr.sin_addr.s_host, 
		addr.sin_addr.s_lh,
		addr.sin_addr.s_impno );
	port = ntohs(addr.sin_port );	return port;
}

int RelayServerUDPNode::MakeIpAddres( char* rcv_ip, UDPIoInfo* recvInfo, int& port )
{
	MakeIpAddres(rcv_ip,recvInfo->addr,port); return port;
}

void RelayServerUDPNode::SendChangeIPMessage( UserInfo* userData, const char* ipAddr, const int port, char* publicID)
{
	if(userData)
	{
		if(strcmp(ipAddr,userData->m_szPublicIP) != 0 || port != userData->m_iClientPort)
		{

			SP2Packet pk(Protocols::RSPTK_ON_CONTROL);
			int ctype = ControlTypes::RS_CHANGE_ADDR;

			char sendPublicID[PUBLICID_MAX];
			char sendIpaddr[STR_IP_MAX];
			strcpy_s(sendPublicID,publicID);
			strcpy_s(sendIpaddr,ipAddr);

			pk << ctype;
			pk << sendPublicID;
			pk << sendIpaddr;
			pk << port;
			pk << userData->m_dwUserIndex;

			GameServerNode* node = NULL;
			node = g_ServerConnectMgr()->GetServerNodeByID(userData->m_serverID);

			if(node)
			{
				node->SendMessage(pk);
				node->ChangeUserAddr(ipAddr,port,userData->m_dwUserIndex);
			}
			else
				LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"SendChangeIPMessage::NotFoundServerNode!!");

			strcpy_s(userData->m_szPublicIP,ipAddr);
			userData->m_iClientPort = port;

			LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"SendIPChangeMessage(%s)[%s:%d]",userData->m_szPublicID,userData->m_szPublicIP,userData->m_iClientPort);

		}
	}
	else
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,"SendChangeIPMessage::GetServerNodeByID Error!!");
	}
}

void RelayServerUDPNode::SendRelayPacket( DWORD dwUserIndex,SP2Packet& spPacket )
{
	int serverID = GetServerID(dwUserIndex);

	GameServerNode* node = NULL;

	if(serverID != -1)
		node = g_ServerConnectMgr()->GetServerNodeByID(serverID);

	if(node)
	{
		RelayGroup* relayGroup = node->GetRelayGroupByUser(dwUserIndex);

		if(relayGroup)
		{
			RelayGroup::RelayGroups* users = relayGroup->GetUserLists();

			if(users == NULL) return;
			
			for(unsigned int i=0; i<users->size(); ++i)
			{
				UserData& userData = users->at(i);

				if(userData.m_dwUserIndex == dwUserIndex)
					continue;

				//printf("(0x%x)SendRelayPacket :%s:%d\n",spPacket.GetPacketID(),users[i].m_szPublicIP,users[i].m_iClientPort);
				g_State()->IncrementTestCount();
				g_UDPNode()->SendCurrentPortMessage(userData.m_szPublicIP  ,userData.m_iClientPort, m_currentIndex, spPacket);
			}

			return;
		}
	}
}

int RelayServerUDPNode::GetServerID( char *publicID )
{
	auto pos = m_userInfoMap.begin();
	while(pos != m_userInfoMap.end())
	{
		UserInfo* userData = (*pos).second;

		if(userData == NULL) return -1;

		if(strcmp(userData->m_szPublicID,publicID) == 0)
		{
			return userData->m_serverID;
		}
		pos++;
	}
	return -1;
}

int RelayServerUDPNode::GetServerID( int userIndex )
{
	UserInfo* userInfo = GetUserInfo(userIndex);
	if(userInfo)
	{
		return userInfo->m_serverID;
	}
	else
		return -1;
}

UserInfo* RelayServerUDPNode::GetUserInfo( const char* publicID)
{
	USERINFO::iterator pos = m_userInfoMap.begin();

	while(pos != m_userInfoMap.end())
	{
		UserInfo* userData = (*pos).second;

		if(userData == NULL) return NULL;

		if(strcmp(userData->m_szPublicID,publicID) == 0)
		{ 
			return userData;
		}
		pos++;
	}
	return NULL;
}

UserInfo* RelayServerUDPNode::GetUserInfo( DWORD userIndex )
{
	USERINFO::iterator pos = m_userInfoMap.find(userIndex);
	if(pos != m_userInfoMap.end())
	{
		return (*pos).second;
	}
	else 
		return NULL;

}

BOOL RelayServerUDPNode::InsertUserInfo(const int userIndex, const char* publicID, const int serverIndex, const char* ipAddr, const int port )
{//hash_map 으로 만들어 userid를 hash 한후 사용하는것 검토할것 

	UserInfo* userInfo = GetUserInfo(userIndex);
	if(userInfo)
	{
		//data 있음 
		if(strcmp(userInfo->m_szPublicIP,ipAddr) != 0 || userInfo->m_iClientPort != port)
		{
			strcpy_s(userInfo->m_szPublicIP,ipAddr);
			strcpy_s(userInfo->m_szPublicID,publicID);
			userInfo->m_iClientPort = port;
			userInfo->m_serverID = serverIndex;
			return FALSE;
		}
	}
	else//data 없으므로 추가 
	{
		UserInfo* newUserInfo = CreateUserInfo();
	 
		//널체크 추가 
		ZeroMemory(newUserInfo,sizeof(UserInfo));
		strcpy_s(newUserInfo->m_szPublicID,publicID);
		newUserInfo->m_dwUserIndex = userIndex;
		strcpy_s(newUserInfo->m_szPublicIP,ipAddr);
		newUserInfo->m_iClientPort = port;
		newUserInfo->m_serverID = serverIndex;
		m_userInfoMap.insert(USERINFO::value_type(userIndex,newUserInfo));
	}
	return TRUE;
}

BOOL RelayServerUDPNode::SetUserRoomInfo( const DWORD userIndex , short roomSate)
{
	UserInfo* userInfo = GetUserInfo(userIndex);
	if(userInfo)
	{
		userInfo->m_iRoomState = roomSate;
	}

	else //릴레이 서버가 
	{
	}
	return FALSE;
}

BOOL RelayServerUDPNode::SetUserRoomInfo( const UserData& userData, short roomState, int serverIndex )
{
	UserInfo* oldUserInfo = GetUserInfo(userData.m_dwUserIndex);
	if(oldUserInfo)
	{
		oldUserInfo->m_iRoomState = roomState;
		oldUserInfo->m_modeType = userData.m_modeType;
		oldUserInfo->m_serverID = serverIndex;
		strcpy_s(oldUserInfo->m_szPublicID,userData.m_szPublicID);
		strcpy_s(oldUserInfo->m_szPublicIP,userData.m_szPublicIP);
		oldUserInfo->m_iClientPort = userData.m_iClientPort;
	}
	else //릴레이 서버가 재접속한 경우일수도 있음 
	{ 

		UserInfo* userInfo = CreateUserInfo();

		if(userInfo == NULL)
		{
			LOG.PrintTimeAndLog(0,"SetUserRoomInfo::Error UserInfo is NULL");
			return FALSE;
		}
 
		ZeroMemory(userInfo,sizeof(UserInfo));

		userInfo->m_dwUserIndex = userData.m_dwUserIndex;
		userInfo->m_iClientPort = userData.m_iClientPort;
		strcpy_s(userInfo->m_szPublicID,userData.m_szPublicID);
		strcpy_s(userInfo->m_szPublicIP,userData.m_szPublicIP);
		userInfo->m_modeType = userData.m_modeType;
		userInfo->m_serverID = serverIndex;
		userInfo->m_iRoomState  = roomState;

		m_userInfoMap.insert(USERINFO::value_type(userInfo->m_dwUserIndex,userInfo));
	}
	return FALSE;
}

int RelayServerUDPNode::DelUserInfoByServerID( const int serverIndex )
{
	int resultCount = 0;
	USERINFO::iterator pos = m_userInfoMap.begin();

	while(pos != m_userInfoMap.end())
	{
		UserInfo* userInfo = (*pos).second;

		if(userInfo == NULL)
			break;

		if(userInfo->m_serverID == serverIndex)
		{
			resultCount++;

			PushUserInfo(userInfo);
			pos = m_userInfoMap.erase(pos);
			continue;
		}
		resultCount++;
		pos++;
	}
	return resultCount;
}

void RelayServerUDPNode::PushUserInfo( UserInfo* userInfo )
{
	ZeroMemory(userInfo,sizeof(UserInfo));
	m_UserInfoPool.Push(userInfo);
}

BOOL RelayServerUDPNode::DelUserInfo( const DWORD userIndex )
{
	USERINFO::iterator pos = m_userInfoMap.find(userIndex);

	if(pos != m_userInfoMap.end())
	{
		UserInfo* userInfo = (*pos).second;

		if(userInfo == NULL) return FALSE;

		PushUserInfo(userInfo); 
		m_userInfoMap.erase(pos);		
		return TRUE;
	}
	else
		return FALSE;
}

BOOL RelayServerUDPNode::DelUserInfo( const DWORD userIndex, const int serverIndex )
{
	USERINFO::iterator pos = m_userInfoMap.find(userIndex);

	if(pos != m_userInfoMap.end())
	{
		UserInfo* userInfo = (*pos).second;

		if(userInfo == NULL) return FALSE;

		if(userInfo->m_serverID == serverIndex)
		{
			Debug("DelUserInfo(%s) ServerIndex : %d\n",userInfo->m_szPublicID,serverIndex);
			LOG.PrintTimeAndLog(0,"DelUserInfo(%s) ServerIndex : %d\n",userInfo->m_szPublicID,serverIndex);

			PushUserInfo(userInfo); 
			m_userInfoMap.erase(pos);		
			return TRUE;
		}
	}
	else
		return FALSE;

	return FALSE;
}

UserInfo* RelayServerUDPNode::CreateUserInfo()
{
	UserInfo* userInfo = m_UserInfoPool.Pop();

	if(userInfo == NULL)
	{
		userInfo = new UserInfo;
		LOG.PrintTimeAndLog(0,"userInfo NULL (CreateUserInfo (oldSize :%d)) ",m_UserInfoPool.GetCount());
	}

	ZeroMemory(userInfo,sizeof(UserInfo));
	return userInfo;
}

void RelayServerUDPNode::SetCurrentIndex( sockaddr_in& addr )
{
	m_currentIndex = 0;

	for(UINT i=0; i<m_recvInfos.size(); ++i)
	{
		if(memcmp(&m_recvInfos[i]->addr,&addr,sizeof(addr)) == 0)
		{
			m_currentIndex = i;
		}
	}
}



