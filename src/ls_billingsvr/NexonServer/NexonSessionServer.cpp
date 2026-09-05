#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "NexonSessionServer.h"
#include "../Util/Ringbuffer.h"
#include "../channeling/iochannelingnodemanager.h"
#include "../channeling/ioChannelingNodeNexonSession.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalManager.h"
#include "../Network/ConnectAssist.h"
#include "../NodeInfo/UserInfoManager.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../NexonDefine.h"
#include "../EtcHelpFunc.h"
#include <atltime.h>

extern CLog LOG;

BOOL tokenize(const std::string str, const std::string& delimiters, std::vector<std::string>& tokens)
{
	tokens.clear();

	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	std::string token;
	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		token = str.substr(lastPos, pos - lastPos);
		tokens.push_back(token);

		lastPos = str.find_first_not_of(delimiters, pos);

		pos = str.find_first_of(delimiters, lastPos);
	}
	return (tokens.size() > 0) ? TRUE : FALSE;
}

NexonSessionServer *NexonSessionServer::sg_Instance = NULL;
NexonSessionServer::NexonSessionServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	m_bFirst = true;
	InitData();
	ZeroMemory(m_szServerIP,MAX_PATH);
	m_iSSPort = 0;
	m_nexonDomainSn = 0;
}

NexonSessionServer::~NexonSessionServer()
{	
}

NexonSessionServer &NexonSessionServer::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "BillingRelayServerInfo.ini" );
		kLoader.SetTitle( "NexonSessionServer Buffer" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		
		sg_Instance = new NexonSessionServer( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );
	}
	return *sg_Instance;
}

void NexonSessionServer::ReleaseInstance()
{		
	SAFEDELETE( sg_Instance );
}

bool NexonSessionServer::ConnectTo( bool bStart )
{
	char szSessionURL[MAX_PATH] = "";
	ioHashStringVec vIPList;
	vIPList.clear();

	const char* szINI = g_App.GetINI().c_str();

	m_nexonDomainSn = GetPrivateProfileInt("DEFAULT", "NexonSessionDomainSn", 2,szINI);

	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NETWORK" );

	kLoader.LoadString( "NexonSessionServerURL", "", szSessionURL, MAX_PATH );
	if( 0 != lstrlen(szSessionURL) )
	{
		if( !Help::GetHostByName(vIPList, szSessionURL) )
			return false;

		strcpy_s(m_szServerIP, sizeof(m_szServerIP), vIPList[0].c_str());
		if( !m_szServerIP )
		{
			LOG.PrintTimeAndLog(0,"m_szServerIP Is NULL ");
			return false;
		}
	}
	else
	{
		kLoader.LoadString( "NexonSessionServerIP", "", m_szServerIP, MAX_PATH );
	}

	int m_iSSPort = kLoader.LoadInt( "NexonSessionServerPORT", -1 );

	if(m_iSSPort == -1)
	{
		LOG.PrintTimeAndLog(0,"Error %s Port Is NULL ",__FUNCTION__);
		return false;
	}


	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "%s fail socket %d[%s:%d]", __FUNCTION__, GetLastError(), m_szServerIP, m_iSSPort );
		return false;
	}
	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( m_szServerIP );
	serv_addr.sin_port			= htons( m_iSSPort );
 
 
	int retval = ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) );
	if( retval != 0 ) 
	{
		DWORD dwError = GetLastError();
		if( dwError != WSAEWOULDBLOCK )
		{
			LOG.PrintTimeAndLog( 0, "[warning][nexon]%s fail connect Errcode(%d)[%s:%d]", __FUNCTION__,dwError, m_szServerIP, m_iSSPort );
			bool reuse = true;
			::setsockopt(GetSocket(),SOL_SOCKET,SO_REUSEADDR,(TCHAR*)&reuse,sizeof(reuse));
			closesocket(socket);
			return false;
		}
	}

	// block
	CConnectNode::SetSocket( socket );

	return true;
}

void NexonSessionServer::InitData()
{
	m_dwCurrentTime = 0;
	m_bSendAlive    = false;
	SetActive(false);
//	SetSyncState(false);

	m_bIsInit = false;

	
}

void NexonSessionServer::OnCreate()
{
	InitData();

	CConnectNode::OnCreate();

	m_dwCurrentTime = TIMEGETTIME();
}

bool NexonSessionServer::AfterCreate()
{
	bool state = false;

	g_iocp.AddHandleToIOCP( (HANDLE)GetSocket(), (DWORD)this );
	LOG.PrintTimeAndLog( 0, "[info][nexon]NexonSessionServerOnConnect (IP:%s PORT:%d RESULT:%d)", m_szServerIP, m_iSSPort, 0 );

	state = CConnectNode::AfterCreate();
	 
	//패킷전송 
	SendInitPacket(m_bFirst);
	
	m_bFirst = false;

	return state;
}

void NexonSessionServer::OnDestroy()
{
	CConnectNode::OnDestroy();

	SetInitState(false);
	LOG.PrintTimeAndLog( 0, "[warning][nexon]Disconnect NexonSessionServer Server!" );
}

void hexdump(const void * buf, size_t size)
{
	const uchar * cbuf = (const uchar *) buf;
	const ulong BYTES_PER_LINE = 16;
	ulong offset, minioffset;

	for (offset = 0; offset < size; offset += BYTES_PER_LINE)
	{
		// OFFSETXX  xx xx xx xx xx xx xx xx  xx xx . . .
		//     . . . xx xx xx xx xx xx   abcdefghijklmnop
		printf("%08x  ", cbuf + offset);
		for (minioffset = offset;
			minioffset < offset + BYTES_PER_LINE;
			minioffset++)
		{
			if (minioffset - offset == (BYTES_PER_LINE / 2)) {
				printf(" ");
			}

			if (minioffset < size) {
				printf("%02x ", cbuf[minioffset]);
			} else {
				printf("   ");
			}
		}
		printf("  ");

		for (minioffset = offset;
			minioffset < offset + BYTES_PER_LINE;
			minioffset++)
		{
			if (minioffset >= size)
				break;

			if (cbuf[minioffset] < 0x20 ||
				cbuf[minioffset] > 0x7e)
			{
				printf(".");
			} else {
				printf("%c", cbuf[minioffset]);
			}
		}
		printf("\n");
	}
}

bool NexonSessionServer::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false;

	ThreadSync ts(this);
	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
#if(_TEST)
	hexdump(SendPacket.GetBuffer() + SendPacket.GetCurPos(),SendPacket.GetBufferSize() - SendPacket.GetCurPos());
#endif
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );
}

bool NexonSessionServer::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int NexonSessionServer::GetConnectType()
{
	return CONNECT_TYPE_NEXON_SESSION_SERVER;
}

void NexonSessionServer::ProcessTime()
{	
	if( g_App.IsWantExit() ) return;

	if( TIMEGETTIME() - m_dwCurrentTime > UPDATE_TIME*2 )       
	{
		static int iCnt = 0;
		if( !IsActive() )
		{
 
		}
		else
		{	
 
			if( m_bSendAlive )
			{
				LOG.PrintTimeAndLog( 0, "%s Nexon Session Server No Answer.", __FUNCTION__  );
				g_LogDBClient.OnInsertBillingServerError( CNT_NEXONSESSION, BILLING_ERROR_LOG_NO_ANSWER,"Billing server no answer." );
				//SessionClose(); //kyg 일단 끊지 않음 
				//OnDestroy();
				m_bSendAlive = false; // 초기화
			}
			else
			{
				m_bSendAlive = true;
			}
 
		}
		
		m_dwCurrentTime = TIMEGETTIME();
	}
}

void NexonSessionServer::SessionClose(BOOL safely)
{
	CPacket packet(BWTPK_CLOSE);
	ReceivePacket( packet );
}

void NexonSessionServer::DispatchReceive(CPacket& packet, DWORD bytesTransferred)
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

#if(_TEST)
	hexdump(m_recvIO.GetBuffer(),bytesTransferred);
#endif

	int loopCount = 0;
	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		m_recvIO.GetBuffer(&m_packetParse,sizeof(m_packetParse),0);
		m_packetParse.Ntohl();

		SP2Packet recvPacket( m_packetParse.PacketType );

		int sizeofPacketParse = m_packetParse.size + sizeof(NexonPacketHeader) - sizeof(BYTE);//마지막 BYTE를 뺴주는건 넥슨 헤더에는 페킷타입은 들어가있지 않음 
			
		recvPacket.SetDataAdd( (char*)m_recvIO.GetBuffer(), min(sizeofPacketParse, (int)m_recvIO.GetBytesTransferred() ), true );

		if( recvPacket.IsValidPacket() == true && (int)m_recvIO.GetBytesTransferred() >= sizeofPacketParse )
		{

			if( !CheckNS( recvPacket ) ) return;

			ReceivePacket( recvPacket );

			m_recvIO.AfterReceive( sizeofPacketParse );

		}
		else 
		{
			//m_recvIO.InitRecvIO();
			LOG.PrintTimeAndLog(0,"DispatchReceive Error Init RecvIO Size :%d PacketID : %x",bytesTransferred,m_packetParse.PacketType);
			break;
		}

		if(loopCount > 200)
		{
			LOG.PrintTimeAndLog(0,"Error %s LoopCountOver(%d:%d)",__FUNCTION__,m_packetParse.size,bytesTransferred);
			m_recvIO.AfterReceive(bytesTransferred);
			break;
		}
		loopCount++;
	}

	WaitForPacketReceive();
}

void NexonSessionServer::OnClose( SP2Packet &rkPacket )
{
	if(IsActive())
	{
		OnDestroy();

		g_UserInfoManager->SetOnSyncState(false);

		g_ConnectAssist.PushNode(this);
	}
	LOG.PrintTimeAndLog(0,"NexonSessionServer OnClose ");
}


void NexonSessionServer::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void NexonSessionServer::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	__try
	{
		switch( kPacket.GetPacketID() )
		{
		case BWTPK_CLOSE:
			OnClose( kPacket );
			break;
		case NEXON_INITIALIZE:
			OnInitialzie( kPacket );
			break;
		case NEXON_LOGIN:
			OnLogin( kPacket );
			break;
		case NEXON_TERMINATE:
			OnTerminate( kPacket );
			break;
		case NEXON_MESSAGE:
			OnMessage( kPacket );
			break;
		case NEXON_SYNC:
			OnSync( kPacket );
			break;
		case NEXON_ALIVE:
			OnAlive( kPacket );
			break;

		default:
			LOG.PrintTimeAndLog( 0, "NexonSessionServer::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
			break;
		}
	}
	__except(1)
	{
		LOG.PrintTimeAndLog(0,"%s Error PacketID : 0x%x",__FUNCTION__,kPacket.GetPacketID());
	}
}

bool NexonSessionServer::SendInitPacket( bool bFirst )
{
	NexonReInitialize initPacket;

	initPacket.SetInfo("LostSaga");
	initPacket.DomainSn = m_nexonDomainSn; //포트별 변경 필요 
	initPacket.Htonl();

	m_packet.SetPosBegin();

	m_packet.Write(initPacket);
	SendMessage(m_packet);
	bFirst = false;

	return true;
}

void NexonSessionServer::OnInitialzie( SP2Packet& rkPacket )
{
	OnNexonInitialize onInit;
	rkPacket.Read(onInit);
	onInit.Ntohl();

	switch(onInit.Result)
	{
	case 0:
		{
			SetInitState(true);
			LOG.PrintTimeAndLog(0,"OnInitialzie Success!(%d) ",onInit.Result);

			g_ServerNodeManager.SendLoginInfo(); 
		}
		break;
	case 1:
		{
			SetInitState(false);
			//재접속 시도 
			m_bFirst = true;
			SessionClose();
		}
		break;
	case 2://인증되지 않은 세션 서버
		{
			 
			SetInitState(false);
			LOG.PrintTimeAndLog(0,"%s Not Available SessionServer !! ",__FUNCTION__);
			//재접속 시도 
			m_bFirst = true;
			SessionClose();
		}
		break;
	case 99:
		{
			LOG.PrintTimeAndLog(0,"%s Unknown Error Reconnect ",__FUNCTION__);
			SessionClose();
		}
		break;
	}
	
	LOG.PrintTimeAndLog(0,"%s %d",__FUNCTION__,onInit.Result);
}


void NexonSessionServer::OnLogin( SP2Packet& rkPacket )
{
	OnNexonLogin onLogin;
	NexonPolicyResult OnPolicy;

	rkPacket.Read(onLogin);
	onLogin.Ntohl();

	NexonUserInfo* userInfo = NULL;

	if(onLogin.SessionNo)
	{
		userInfo = g_UserInfoManager->GetUSerInfoByChanID(onLogin.UserId);
		//LOG.PrintTimeAndLog(0,"OnLogin (%s)" ,onLogin.UserId.c_str());
	}

	if(userInfo == NULL )
	{
		SendLogoutPacket(onLogin.UserId.c_str(), onLogin.SessionNo);
		LOG.PrintTimeAndLog(0,"%s LogOutUser userSeeionNois NULL(%s)", __FUNCTION__, onLogin.UserId.c_str());
		return;
	}

	bool rtVal =false;
	int errcode = 0;
	DWORD timecode = 0;

	userInfo->sessionNo = onLogin.SessionNo; //코드정리

	switch(onLogin.AuthorizeResult)
	{
	case 0:
		{// Forbidden	// 접속자 차단 로직 구현	// 로그아웃 패킷 안보냄
			
			rtVal = OptionCheck(onLogin.Option, userInfo, onLogin.Argument, true);

			if(rtVal == false)
				SendClosePacket(true, NexonForbidden, userInfo);
		}
		break;
	case 1: // Allowed
		{
			bool isAlow = true;
			
			if(onLogin.PropertyCount > 0)
			{
				if(GetPropertyResult(rkPacket,OnPolicy,onLogin.PropertyCount))
				{
					isAlow = true;
 
					parseDiscription(errcode, timecode, OnPolicy.PolicyDisciption.c_str());	

					if(timecode)
						SendClosePacket(false, NexonSelctShutdown, userInfo, errcode, timecode); //시간만 알려주기 위함 
				}
				else//강퇴 //정책 적용이 되면 무조건 강퇴임 
				{
					isAlow = false;
			
					parseDiscription(errcode, timecode, OnPolicy.PolicyDisciption.c_str());		

					SelectShutdownReasonPacket(true, errcode, timecode, userInfo);

					LOG.PrintTimeAndLog(0,"%s Send ShutDown User:%s (%s)",__FUNCTION__, userInfo->privateID.c_str(), OnPolicy.PolicyDisciption.c_str());
				}
			}
			if(isAlow)
				OptionCheck(onLogin.Option, userInfo, onLogin.Argument, false);

			SendPCRoomPacket(userInfo, onLogin, OnPolicy.PCRoomNo);
		}
		break;
	case 2: // Trial //일반 유저로 처리 
		{
			//그냥 넘김 
			userInfo->sessionNo = onLogin.SessionNo;
			bool isAlow = true;
			if(onLogin.PropertyCount > 0)
			{
				if(GetPropertyResult(rkPacket,OnPolicy,onLogin.PropertyCount))
				{
					//kyg 일반유저라 프리미엄일수가없음.. or 과금이 다된 피시방임..
					parseDiscription(errcode, timecode, OnPolicy.PolicyDisciption.c_str());		

					if(timecode)
						SendClosePacket(false, NexonSelctShutdown, userInfo, errcode, timecode); //시간만 알려주기 위함 
				}
				else//강퇴 
				{
					isAlow = false;

					if(userInfo->chType == CNT_NEXON) //혹시 몰라 검색 한번더 ..
					{
						int errcode = 0;
						DWORD timecode = 0;

						parseDiscription(errcode,timecode,OnPolicy.PolicyDisciption.c_str());		

						SelectShutdownReasonPacket(true,errcode,timecode,userInfo); 

						LOG.PrintTimeAndLog(0,"%s Send ShutDown User:%s errcode(%d) (%s)", __FUNCTION__, userInfo->privateID.c_str(), errcode, OnPolicy.PolicyDisciption.c_str());
					}
				}
			}
			if(isAlow)
				OptionCheck(onLogin.Option, userInfo, onLogin.Argument, false);
		}
		break;
	case 3: // Terminate// 접속자 차단 로직 구현	// 로그아웃 패킷  을 반드시 보냄
		{		 
			rtVal = OptionCheck(onLogin.Option,userInfo,onLogin.Argument,true);

			if(rtVal == false)
				SendClosePacket(true, NexonForbidden, userInfo);
		}
		break;
	}

	LOG.PrintTimeAndLog(0,"OnLogin [Property:%d::%d][Authresult:%d]::%I64u::%s::argument:%d",
		onLogin.PropertyCount,
		OnPolicy.PolicyNo,
		onLogin.AuthorizeResult,
		onLogin.SessionNo,
		onLogin.UserId.c_str(),
		onLogin.Argument);
}

void NexonSessionServer::OnTerminate( SP2Packet& rkPacket )
{
	OnNexonTerminate onTerminate;
	NexonPolicyResult OnPolicy;
	int errcode = 0;
	DWORD timecode = 0;

	rkPacket.Read(onTerminate);
	onTerminate.Ntohl();

	NexonUserInfo* userInfo = g_UserInfoManager->GetUSerInfoBySessionNo(onTerminate.SessionNo);

	if(userInfo == NULL)
	{
		LOG.PrintTimeAndLog(0,"OnTerminate UserInfos IS NULL(%I64u(%s)", onTerminate.SessionNo, onTerminate.UserId.c_str());//l(소문자엘) -> I 
		return;
	}

	bool rtVal = false;

	if(userInfo)
	{
		if(onTerminate.PropertyCount > 0) //차단 정책이 최 우선임
		{ 
			if(GetPropertyResult(rkPacket,OnPolicy,onTerminate.PropertyCount))
			{
				//pc방 아님으로 암것도 할 필요 없음..(로그인아님)
			}
			else //강퇴 
			{
				//LOG.PrintTimeAndLog(0,"OnTerminate PolicyParse ");
		
				parseDiscription(errcode, timecode, OnPolicy.PolicyDisciption.c_str());

				SelectShutdownReasonPacket(true, errcode, timecode, userInfo);

				LOG.PrintTimeAndLog(0,"OnTerminate Send ShutDown User:%s errCode(%d)",userInfo->privateID.c_str(),errcode);
				return;
			}
		}
		//프로퍼티카운트가 0 일떄는 옵션 체크를함 .

		rtVal = OptionCheck(onTerminate.Option,userInfo,0);

		if(rtVal == false) // unkonwon 에러 메시지패킷에는 해당하지않음 
		{
			SendClosePacket(true,NexonUnknownShutDown,userInfo);
		}

		LOG.PrintTimeAndLog(0,"OnTerminate userID:%s properyCount:%d Option:%d Errcode :%d",
			userInfo->privateID.c_str(),
			onTerminate.PropertyCount,
			onTerminate.Option,
			errcode);
	}
 
	LOG.PrintTimeAndLog(0,"OnTerminate sessionNo:%I64u::nexonuserID %s::optionVal : %d",
		onTerminate.SessionNo,
		onTerminate.UserId.c_str(),
		onTerminate.Option);
}

void NexonSessionServer::OnMessage( SP2Packet& rkPacket )
{
	OnNexonMessage onMessage;
	rkPacket.Read(onMessage);
	onMessage.Ntohl();
	
	NexonUserInfo* userInfo = g_UserInfoManager->GetUSerInfoBySessionNo(onMessage.SessionNo);

	if(userInfo)
		OptionCheck(onMessage.Option, userInfo, onMessage.Argument);
	else
		LOG.PrintTimeAndLog(0,"OnMessage UNKONOWN %s Option:%d Arguement:%d",onMessage.UserId.c_str(), onMessage.Option, onMessage.Argument);

	LOG.PrintTimeAndLog(0,"OnMessage user: %s option(%d) Arguemnt(%d)",onMessage.UserId.c_str(), onMessage.Option, onMessage.Argument);
}

void NexonSessionServer::OnSync( SP2Packet& rkPacket )
{
 	OnNexonSynchronize onSync;
	rkPacket.Read(onSync);
	onSync.Ntohl();

	if(onSync.IsMonitoring == 0)
	{
		if(onSync.Count)
		{
			OnNexonSyncUser nexonUser;

			for(DWORD i =0; i< onSync.Count; ++i)
			{
				nexonUser.Init();

				rkPacket >> nexonUser.SessionNo;
				rkPacket >> nexonUser.userIdLength;
				rkPacket.SetHashString(nexonUser.userId, nexonUser.userIdLength);

				nexonUser.Ntohl();
				m_users.push_back(nexonUser);
			}		
		}
		else
		{
			LOG.PrintTimeAndLog(0,"%s Sync Finish Send LoginPacket",__FUNCTION__);
			//SendLoginPacket 
			g_UserInfoManager->SetOnSyncState(true);
			SendReLoginPacket();

		}
		SendSyncPacket(onSync.IsMonitoring);
	}
 	else if(onSync.IsMonitoring == 1)
	{
		if(onSync.Count)
		{
			OnNexonSyncUser nexonUser;

			for(DWORD i =0; i< onSync.Count; ++i)
			{
				nexonUser.Init();

				rkPacket >> nexonUser.SessionNo;
				rkPacket >> nexonUser.userIdLength;
				rkPacket.SetHashString(nexonUser.userId, nexonUser.userIdLength);

				nexonUser.Ntohl();
				m_users.push_back(nexonUser);
			}		

			SendSyncPacket(onSync.IsMonitoring);
		}
		g_UserInfoManager->SetOnSyncState(true);
	}
	LOG.PrintTimeAndLog(0,"%s %d", __FUNCTION__, onSync.Count);
}

void NexonSessionServer::OnAlive( SP2Packet& rkPacket )
{
	//NexonAlive onAlvie;
	//rkPacket.Read(onAlvie);
}

void NexonSessionServer::SendAlivePacket()
{
	//kyg send alive
	if(GetInitState())
	{	
		m_onAlive.Init();
		m_onAlive.Htonl();

		m_packet.SetPosBegin();
		m_packet.Write(m_onAlive);

		SendMessage(m_packet);
	}
}

bool NexonSessionServer::SendReLoginPacket()
{
	if(g_UserInfoManager->GetOnSyncState())
	{
		RELOGINUSERS::iterator pos = m_nexonReLoginUsers.begin();		
		while ( pos != m_nexonReLoginUsers.end() )
		{
			NexonLogin& tempUser = ( *pos );
			m_packet.SetPosBegin();
			m_packet.Write(tempUser);
			SendMessage(m_packet);
			LOG.PrintTimeAndLog(0, "%s:%s", __FUNCTION__, tempUser.UserId);
			pos++;
		}
		m_nexonReLoginUsers.clear();

		OTHERUSERS::iterator otherPos = m_otherReLoginUsers.begin();
		while( otherPos != m_otherReLoginUsers.end() )
		{
			OtherLogin &tempUser = ( *otherPos );
			m_packet.SetPosBegin();
			m_packet.Write(tempUser);
			SendMessage(m_packet);
			LOG.PrintTimeAndLog(0, "%s:%s", __FUNCTION__, tempUser.UserId);
			otherPos++;
		}
		m_otherReLoginUsers.clear();
	}

	return true;
}

bool NexonSessionServer::GetPropertyResult( SP2Packet& rkPacket,NexonPolicyResult& OnPolicy, BYTE propertyCount)
{
	WORD propertyNo = 0;
	unsigned char policyCount = 0;
	bool rtVal = true;
	for(int i=0; i<propertyCount; ++i)
	{
		rkPacket >> propertyNo;
		propertyNo = ntohs(propertyNo);

		switch(propertyNo)
		{
		case NEXON_POLICY_SHUTDOWN:
			{
				rkPacket >> policyCount;
				if(policyCount)
				{
					rtVal= ParsePolicyPacket(rkPacket, OnPolicy, policyCount);
				}
			}
			break;
		case NEXOM_LOGIN_PROPERTY_PCROOMNO:
			{
				DWORD pcRoomNo = 0;
				rkPacket >> pcRoomNo;
				pcRoomNo = ntohl(pcRoomNo);
				OnPolicy.PCRoomNo = pcRoomNo;
			}
			break;

		case NEXON_LOGIN_PROPERTY_ISPCROOM:
		case 11: //피시방 우편번호
			{
				BYTE isValid = 0;
				rkPacket >> isValid;

				LOG.PrintTimeAndLog(0,"(%d)Vaildate PCRoom/PCRoom Level(%d)",propertyNo,isValid);
			}
			break;

		case 9://피시방 상호명
		case 10: //피시방 우편번호
			{
				BYTE strLength = 0;
				ioHashString pcRoomName;
				rkPacket >> strLength;
				rkPacket.SetHashString(pcRoomName,strLength);
				LOG.PrintTimeAndLog(0,"(%d)PCRoom Name/PostCode : %s(%d)",propertyNo,pcRoomName.c_str(),strLength);
			}
			break;

		}
	 
	}
	return rtVal;
}

bool NexonSessionServer::SendLogoutPacket( const uint64_t sessionNo )
{
	NexonUserInfo* userInfo = g_UserInfoManager->GetUSerInfoBySessionNo(sessionNo);

	if(userInfo)
	{
		DeleteReLoginUser( userInfo->chanID.c_str() );

		if(!GetInitState())
		{
			LOG.PrintTimeAndLog(0,"%s InitSateError",__FUNCTION__);
			return false;
		}

		NexonLogOut logout;
		logout.SetInfo(userInfo->chanID.c_str(),userInfo->sessionNo);
		logout.Htonl();

		m_packet.SetPosBegin();
		m_packet.Write(logout);

		bool rtVal = SendMessage(m_packet);

		g_UserInfoManager->DelUserInfo(userInfo);

		LOG.PrintTimeAndLog(0,"%s Send[%s]::[%s]",__FUNCTION__,userInfo->chanID.c_str(),userInfo->privateID.c_str());
		return rtVal;
	}
	else
		LOG.PrintTimeAndLog(0,"%s Error UserInfo is NULL",__FUNCTION__);

	return false;
}

bool NexonSessionServer::SendLogoutPacket( const DWORD userIndex )
{
	NexonUserInfo* userInfo = g_UserInfoManager->GetUserInfoByUserIndex(userIndex);

	if(userInfo)
	{
		DeleteReLoginUser( userInfo->chanID.c_str() );

		if(!GetInitState())
		{
			LOG.PrintTimeAndLog(0,"%s InitSateError",__FUNCTION__);
			return false;
		}
		
		NexonLogOut logout;
		 
		logout.SetInfo(userInfo->chanID.c_str(),userInfo->sessionNo);
		logout.Htonl();

		m_packet.SetPosBegin();
		m_packet.Write(logout);

		LOG.PrintTimeAndLog(0,"SendLogoutPacket (%s)",userInfo->privateID.c_str());

		bool rtVal = SendMessage(m_packet);

		g_UserInfoManager->DelUserInfo(userInfo);

		return rtVal;
	}
	LOG.PrintTimeAndLog(0,"SendLogoutPacket userIndex is Null(%d)",userIndex);
	return true;
}

bool NexonSessionServer::SendLogoutPacketForPCRoomUser(const DWORD userIndex)
{
	NexonUserInfo* userInfo = g_UserInfoManager->GetUserInfoByUserIndex(userIndex);

	if(userInfo)
	{
		DeleteReLoginUser( userInfo->chanID.c_str() );

		if(!GetInitState())
		{
			LOG.PrintTimeAndLog(0,"%s InitSateError",__FUNCTION__);
			return false;
		}

		NexonLogOut logout;
 
		logout.SetInfo(userInfo->chanID.c_str(),userInfo->sessionNo);
		logout.Htonl();

		m_packet.SetPosBegin();
		m_packet.Write(logout);

		LOG.PrintTimeAndLog(0,"SendLogoutPacketForPCRoomUser (%s)",userInfo->privateID.c_str());

		bool rtVal = SendMessage(m_packet);
		return rtVal;
	}
	LOG.PrintTimeAndLog(0,"SendLogoutPacketForPCRoomUser userIndex is Null(%d)",userIndex);
	return true;
}

bool NexonSessionServer::SendLogoutPacket( const char* chanID, const uint64_t sessionNo )
{
	NexonLogOut logout;
	logout.SetInfo(chanID,sessionNo);
	logout.Htonl();

	m_packet.SetPosBegin();
	m_packet.Write(logout);

	return SendMessage(m_packet);

}

bool NexonSessionServer::SendLogoutPacketByUserInfo( const NexonUserInfo* userInfo )
{
	if(userInfo == NULL)
		return false;

	DeleteReLoginUser( userInfo->chanID.c_str() );

	if(!GetInitState())
	{
		LOG.PrintTimeAndLog(0,"%s InitSateError",__FUNCTION__);
		return false;
	}
	
	NexonLogOut logout;
	logout.SetInfo(userInfo->chanID.c_str(),userInfo->sessionNo);
	logout.Htonl();

	m_packet.SetPosBegin();
	m_packet.Write(logout);

	bool rtVAl = SendMessage(m_packet);

	LOG.PrintTimeAndLog(0,"SendLogoutPacketByUserInfo by UserInfo %s",userInfo->privateID.c_str());

	return rtVAl; 
}

bool NexonSessionServer::SendLoginPacket(const char* userID, const char* privateID, const char* publicIP, const char* privateIP, const int iChanType)
{
	bool rtVal = false;
 
	if(iChanType == CNT_NEXON )
	{
		NexonLogin login;
		
		login.SetInfo(userID,inet_addr(publicIP),privateID,inet_addr(privateIP));
		login.Htonl();

		m_packet.SetPosBegin();
		m_packet.Write(login);

		rtVal =  SendMessage(m_packet);

		if(!rtVal || !GetInitState())
		{
			LOG.PrintTimeAndLog(0,"Error SendLoginPacket :%s",userID);
			bool bFind = false;
			
			RELOGINUSERS::iterator pos;
			for( pos = m_nexonReLoginUsers.begin(); pos != m_nexonReLoginUsers.end(); ++pos )
			{
				NexonLogin &tempUser = ( *pos );
				if( tempUser.UserId == login.UserId )
				{
					bFind = true;
					break;
				}
			}
			if(bFind == false)
				m_nexonReLoginUsers.push_back(login);
		}
	}
	else
	{
		OtherLogin login;

		login.SetInfo(userID,inet_addr(publicIP),privateID,inet_addr(privateIP));
		login.Htonl();

		m_packet.SetPosBegin();
		m_packet.Write(login);

		rtVal = SendMessage(m_packet);

		if(!rtVal || !GetInitState())
		{
			bool bFind = false;
			LOG.PrintTimeAndLog(0,"Error OrtherSendLoginPacket :%s",userID);

			OTHERUSERS::iterator pos;
			for( pos = m_otherReLoginUsers.begin(); pos != m_otherReLoginUsers.end(); ++pos )
			{
				OtherLogin &tempOtherUser = ( *pos );
				if( tempOtherUser.UserId == login.UserId )
				{
					bFind = true;
					break;
				}
			}
			if(bFind == false)
				m_otherReLoginUsers.push_back(login);
		}
	}	

	if(!GetInitState())
	{
		LOG.PrintTimeAndLog(0,"%s InitSateError",__FUNCTION__);

		return false;
	}
	return rtVal;
}

int NexonSessionServer::SendSyncPacket(BYTE monitorState)
{
	if(!GetInitState())
	{
		LOG.PrintTimeAndLog(0,"SendSyncPacket InitSateError");
		return -1;
	}
 
	if(!m_users.empty())
	{
		NexonSynchronize SyncPacket;
		SyncPacket.SetInfo(monitorState,m_users.size());

		SyncPacket.size += sizeof(NexonSendSyncUser) * m_users.size();

		SyncPacket.Htonl();

		m_packet.SetPosBegin();
		m_packet.Write(SyncPacket);

		//이제 카운트만큼 데이터 붙여주면 됨 

		for(int i=0; i< (int)m_users.size(); i++)
		{

			NexonSendSyncUser user;
			
			NexonUserInfo* userInfo = g_UserInfoManager->GetUSerInfoBySessionNo(m_users[i].SessionNo); //kyg 수정세션으로 수조ㅓㅇ 

			if(userInfo == NULL)
			{
				userInfo = g_UserInfoManager->GetUSerInfoByChanID(m_users[i].userId.c_str());
			}
			
			if(userInfo)
			{
				user.SetInfo(m_users[i].SessionNo,1);
				LOG.PrintTimeAndLog(0,"SendSyncPacket Send Sync User:%s",userInfo->privateID.c_str());
			}
			else
			{
				user.SetInfo(m_users[i].SessionNo,0);
			}

			user.Htonl();

			m_packet.Write(user);
		}

		if(!m_users.empty())
			SendMessage(m_packet);

		m_users.clear();
	}
	return 0;//일단 0 
}

void NexonSessionServer::SendClosePacket( bool nNow, int reson,NexonUserInfo* userInfo, int errcode, DWORD time )
{
	if(!GetInitState())
	{
		LOG.PrintTimeAndLog(0,"%s InitSateError",__FUNCTION__);
		return;
	}

	if(userInfo)
	{
		if(!userInfo->serverIP.IsEmpty())
		{
			SP2Packet pk(BSTPK_SESSION_CONTROL_RESULT);

			pk << userInfo->userIndex;
			pk << userInfo->privateID; // billguid랑 같음
			pk << reson;
			pk << errcode;
			pk << time;
			pk << nNow;
	
			      
			if ( !g_ServerNodeManager.SendMessageIP(userInfo->serverIP,userInfo->serverPort,pk) )
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s", "NexonSessionServer::SendClosePacket", userInfo->userIndex, userInfo->privateID.c_str() );
			}
		}
	}
}

void NexonSessionServer::parseDiscription( int& errCode, DWORD& time ,std::string parseString )
{
	std::string values = parseString;
	std::string delimiter = "&";
	std::vector<std::string> tokens;
	std::vector<std::string> seconTokens;

	if(parseString.length() == 0)
	{
		//LOG.PrintTimeAndLog(0,"ParseString IS NULL");
		return;
	}
	tokenize(parseString,delimiter,tokens);

	delimiter = "=";
	for(int i=0; i< (int)tokens.size(); ++i)
	{
		if(!tokens.empty())
		{
			seconTokens.clear();

			tokenize(tokens[i],delimiter,seconTokens);
			if(seconTokens.size() >= 2)
			{
				if(strcmp(seconTokens[0].c_str(),"time") == 0)
					time = atoi(seconTokens[1].c_str());
				else if(strcmp(seconTokens[0].c_str(),"error") == 0)
					errCode = atoi(seconTokens[1].c_str());
					
			}
// 			else
// 				LOG.PrintTimeAndLog(0,"ParseString Unmatch(%s)",parseString.c_str());
		}
	}
	LOG.PrintTimeAndLog(0,"ParseString time :%d erocdoe %d",time,errCode);
}

void NexonSessionServer::SelectShutdownReasonPacket( bool bState, int errCode, DWORD timeCode ,NexonUserInfo* userInfo )
{
	if( !userInfo ) return;

	switch(errCode)
	{
	case 1: // 나이오류 
		SendClosePacket(bState,NexonAgeError,userInfo);
		break;
	case 4://셀렉트 셧다운
		SendClosePacket(bState,NexonSelctShutdown,userInfo,errCode,timeCode);
		break;
	case 3://셧다운
		SendClosePacket(bState,NexonShutDown,userInfo,errCode);
		break;
	case 2://NexonNotNexonUser
		SendClosePacket(bState,NexonNotNexonUser,userInfo);
		break;
	case 99://unknown
	default:
		SendClosePacket(bState,NexonUnknownShutDown,userInfo);
		break;
	}
}

bool NexonSessionServer::OptionCheck( BYTE option, NexonUserInfo* userInfo, int argument, bool bLogin )
{
	if(userInfo)
	{
		switch(option)
		{
		case 1://프리미엄 PC방아님
			SendClosePacket(bLogin,NexonAddressNotAllowed,userInfo);
			return true;
		case 2://동일 IP로 여러명 접속 담당자에게 연락	 
			SendClosePacket(bLogin,NexonIPMaxError,userInfo);
			return true;
		case 5://최대 접속 ID수 초과 
			SendClosePacket(bLogin,NexonIDMaxError,userInfo);
			return true;
		case 13://체험ID 
			return true;
		case 17://로그인시 정량제 시간 남음 int argument // terminate 는 argument 0 
			if(argument == 0) //termiante때는 메시지 보냄 
				SendClosePacket(bLogin,NexonWelcomeMessage,userInfo,0,argument);

			LOG.PrintTimeAndLog(0,"OptionCheck Option SUCESS(%s) %d %d",userInfo->privateID.c_str(),option,argument);
			break;
		case 32:
			SendClosePacket(bLogin,NexonWelcomeMessage,userInfo,0,argument);

			LOG.PrintTimeAndLog(0,"OptionCheck Option SUCESS(%s) %d %d",userInfo->privateID.c_str(),option,argument);
			return true;
		case 27://접속이 차단된 PC방임 
		case 28:
			SendClosePacket(bLogin,NexonPCRoomBlocked,userInfo); 
			return true;
		case 99:
			return false;
		}
	}
	return false;
}

void NexonSessionServer::CheckSessionServer()
{
	CTime time(CTime::GetCurrentTime());

	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "NexonSessionCheck" );

	int hour = kLoader.LoadInt( "Hour", 0 ); //am 12시임 
	int minute = kLoader.LoadInt( "Minute", 0 );

	if( time.GetHour() == hour && time.GetMinute() >= minute )
	{
		if( GetInitState() == false && IsActive() == false ) 
		{
			std::vector<NexonUserInfo*> delUserInfos;
			UserInfoManager::NexonUserInfoList* userInfos = g_UserInfoManager->GetUserInfoList();

			if(userInfos == NULL)
				return;

			POSITION pos = userInfos->GetHeadPosition();
		 
			while(pos)
			{
				NexonUserInfo* userInfo = userInfos->GetAt(pos);
				if(userInfo)
				{
					if( userInfo->chType == CNT_NEXON )
					{
						SendClosePacket(true,NexonSessionServerError,userInfo);
						delUserInfos.push_back(userInfo);
					}
				}
				userInfos->GetNext(pos);
			}
				
			LOG.PrintTimeAndLog(0,"CheckSessionServer Send NexonSessionServer Errror Count(%d)",delUserInfos.size());

			for(int i=0; i < (int)delUserInfos.size(); ++i)
			{
				NexonUserInfo* userInfo = delUserInfos[i];
				if(userInfo)
					g_UserInfoManager->DelUserInfo(userInfo);
			}
		}
	}
}

void NexonSessionServer::DeleteReLoginUser( const char* szUserId )
{
	RELOGINUSERS::iterator iter = m_nexonReLoginUsers.begin();
	while ( iter != m_nexonReLoginUsers.end() )
	{
		NexonLogin& tempUser = ( *iter );
		
		if(strcmp(tempUser.UserId.c_str(), szUserId) == 0)
		{
			//삭제
			m_nexonReLoginUsers.erase( iter );	
			return;
		}
		else 
		{
			iter++;
		}
	}

	OTHERUSERS::iterator otherIter = m_otherReLoginUsers.begin();
	while( otherIter != m_otherReLoginUsers.end() )
	{
		OtherLogin &tempOtherUser = ( *otherIter );
		
		if(strcmp(tempOtherUser.UserId.c_str(), szUserId) == 0)
		{
			//삭제
			m_otherReLoginUsers.erase( otherIter );
			return;
		}
		else 
		{
			otherIter++;
		}
	}
}

void NexonSessionServer::SendPCRoomPacket( NexonUserInfo* userInfo, OnNexonLogin &onLogin, DWORD pcRoomNo)
{
	if( !userInfo) return;

	SP2Packet kPacket( BSTPK_SESSION_CONTROL_RESULT );

	if(pcRoomNo == 0)
		pcRoomNo = 1;

	kPacket << userInfo->userIndex;
	kPacket << onLogin.UserId;
	kPacket << NexonOnPCRoom;
	kPacket << pcRoomNo;
	kPacket << pcRoomNo;

	if ( !g_ServerNodeManager.SendMessageIP(userInfo->serverIP,userInfo->serverPort,kPacket) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s", "NexonSessionServer::SendPCRoomPacket", userInfo->userIndex, onLogin.UserId.c_str() );
		return;
	}

	userInfo->pcRoomState = 1;		//130906 임시 추가
	LOG.PrintTimeAndLog(0,"SendPCRoomPacket Send PCRoom User:%s(%d)",userInfo->privateID.c_str(),pcRoomNo);
}

bool NexonSessionServer::ParsePolicyPacket( SP2Packet &rkPacket, NexonPolicyResult &OnPolicy, BYTE policyCount )
{
	rkPacket >> OnPolicy.PolicyNo;

	switch(OnPolicy.PolicyNo)
	{
	case NEXON_LOGIN_PROPERTY_POLICY_VALUE:
		{
			rkPacket >> OnPolicy.PolicyNameLength;

			if(OnPolicy.PolicyNameLength)
				rkPacket.SetHashString( OnPolicy.PolicyName,OnPolicy.PolicyNameLength);

			rkPacket >> OnPolicy.PolicyResult;
			rkPacket >> OnPolicy.PolicyDisLength;

			if(OnPolicy.PolicyDisLength)
				rkPacket.SetHashString( OnPolicy.PolicyDisciption,OnPolicy.PolicyDisLength);

			// LOG.PrintTimeAndLog(0,"ParsePolicyPacket Result:%d PolicyDiscription :%s(%d)",OnPolicy.PolicyResult,OnPolicy.PolicyDisciption.c_str(),OnPolicy.PolicyNameLength);

			if(OnPolicy.PolicyResult != 0) //1이면 무조건 강퇴해야함
			{
				return false;
			}
		}
		break;

	default:
		LOG.PrintTimeAndLog(0," ParsePolicyPacket UnKnown PolicyNum");
		break;
	}

	return true;
}

  