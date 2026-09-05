#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../NodeInfo/UserCopyNode.h"
#include "../NodeInfo/UserNodeManager.h"
#include "../NodeInfo/ioItemInfoManager.h"
#include "../NodeInfo/ioInventory.h"
#include "../NodeInfo/ioDecorationPrice.h"
#include "../DataBase/DBClient.h"

#include ".\billingrelayserver.h"
#include "..\Local\ioLocalParent.h"
#include "..\NodeInfo\ServerNodeManager.h"

#define BILLING_RECONNECT_USER_COUNT 50

BillingRelayServer *BillingRelayServer::sg_Instance = NULL;
BillingRelayServer::BillingRelayServer( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	m_iBillingPort = 0;
	m_dwCurrentTime= 0;
	m_bReconnectState = false;
}

BillingRelayServer::~BillingRelayServer(void)
{
}

BillingRelayServer &BillingRelayServer::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_game.ini" );
		kLoader.SetTitle( "BillingRelay Session" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", MAX_BUFFER );

		sg_Instance = new BillingRelayServer( INVALID_SOCKET, iSendBufferSize, MAX_BUFFER * 2 );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][billing]session sendBuffer size : [%d]", iSendBufferSize );
	}
	return *sg_Instance;
}

void BillingRelayServer::ReleaseInstance()
{		
	SAFEDELETE( sg_Instance );
}

bool BillingRelayServer::ConnectTo()
{ 
	if( g_ServerNodeManager.GetServerIndex() == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BillingRelayServer::ConnectTo ServerIndex is empty" );
		return false;
	}

	// 서버선택
	GenerateBillingServerInfo();

	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BillingRelayServer::ConnectTo socket %d[%s:%d]", GetLastError(), m_szBillingIP.c_str(), m_iBillingPort );
		return false;
	}
	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( m_szBillingIP.c_str() );
	serv_addr.sin_port			= htons( m_iBillingPort );

	if( ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) ) != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BillingRelayServer::ConnectTo connect %d[%s:%d]", GetLastError(), m_szBillingIP.c_str(), m_iBillingPort );
		closesocket(socket);
		return false;
	}

	g_iocp.AddHandleToIOCP( (HANDLE)socket, (DWORD)this );
	CConnectNode::SetSocket( socket );

	OnCreate();
	AfterCreate();
	//SendUserInfo();//BillingServer가 준비가 되면 보내주기로함 
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][billing]OnConnect : [%s] [%d] [%d]", m_szBillingIP.c_str(), m_iBillingPort, 0 );
	return true;
}

void BillingRelayServer::InitData()
{
	m_dwCurrentTime = 0;
}

void BillingRelayServer::OnCreate()
{
	CConnectNode::OnCreate();
	InitData();

	m_dwCurrentTime = TIMEGETTIME();

	SP2Packet kPacket( BSTPK_SERVER_IPPORT );
	PACKET_GUARD_VOID( kPacket.Write(g_App.GetPrivateIP()) );
	PACKET_GUARD_VOID( kPacket.Write(g_App.GetCSPort()) );
	PACKET_GUARD_VOID( kPacket.Write(g_ServerNodeManager.GetServerIndex()) );
	SendMessage( kPacket );
}

void BillingRelayServer::OnDestroy()
{
	CConnectNode::OnDestroy();
	SetReconnectState(true);
	g_CriticalError.CheckBillingServerDisconnect();
}

void BillingRelayServer::SessionClose(BOOL safely)
{
	if(!safely)
	{
		g_CriticalError.CheckBillingServerExceptionDisconnect( GetLastError() );
	}

	CPacket packet(BSTPK_CLOSE);
	ReceivePacket( packet );
}

bool BillingRelayServer::SendMessage( CPacket &rkPacket )
{
	g_ProcessChecker.BillingRelayServerSendMessage( rkPacket.GetPacketID(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( rkPacket );
}

bool BillingRelayServer::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int BillingRelayServer::GetConnectType()
{
	return CONNECT_TYPE_BILLING_RELAY_SERVER;
}


void BillingRelayServer::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void BillingRelayServer::ProcessTime()
{	
	if( g_App.IsWantExit() ) return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( TIMEGETTIME() - m_dwCurrentTime > UPDATE_TIME )       
	{
		if( !IsActive() )
			ConnectTo();
		m_dwCurrentTime = TIMEGETTIME();
	}
}

void BillingRelayServer::ProcessFlush()
{
	if( g_App.IsWantExit() )
		return;

	if( ! IsActive() )
		return;
	if( GetSocket() == INVALID_SOCKET )
		return;

	CSendIO::FlushSendBuffer();
}

void BillingRelayServer::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	FUNCTION_TIME_CHECKER( 500000.0f, kPacket.GetPacketID() );          // 0.5 초 이상 걸리면로그 남김

	OnBillingPacketParsing( kPacket );
}

void BillingRelayServer::OnBillingPacketParsing( SP2Packet &rkPacket )
{
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID = ""; // PrintTimeAndLog에서 NULL이 나오지 않게 ""
	UserParent   *pUserParent = NULL;
	ioHashString szUserGUID = "";
 
	switch(rkPacket.GetPacketID())
	{
	case BSTPK_CLOSE:
		OnClose();
		return;
	case BSTPK_REQUEST_USERINFO:
		SendUserInfo();
		return;
	}
	if( rkPacket.GetPacketID() == BSTPK_LOGIN_RESULT || 
		rkPacket.GetPacketID() == BSTPK_USER_INFO_RESULT ||
		rkPacket.GetPacketID() == BSTPK_AUTOUPGRADE_LOGIN_RESULT ||
		rkPacket.GetPacketID() == BSTPK_OTP_RESULT ||
		rkPacket.GetPacketID() == BSTPK_IPBONUS_RESULT )
	{
		PACKET_GUARD_VOID( rkPacket.Read(szPrivateID) );
		pUserParent =  g_UserNodeManager.GetGlobalUserNodeByPrivateID( szPrivateID );
	}
	else if( rkPacket.GetPacketID() == BSTPK_DAUM_SHUTDOWN_CHECK )
	{
		PACKET_GUARD_VOID( rkPacket.Read(szUserGUID) );
		pUserParent = g_UserNodeManager.GetUserNodeByGUID( szUserGUID );
		if( !pUserParent )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][daum]Adult chek target is none");
			return;
		}
	}
	else
	{
		PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
		pUserParent =  g_UserNodeManager.GetGlobalUserNode( dwUserIndex );
	}

	if( !pUserParent )
	{
		PACKET_GUARD_VOID( rkPacket.Read(szBillingGUID) );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BillingRelayServer::OnBillingPacketParsing Not User: %d:%s:%s:0x%x", dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), rkPacket.GetPacketID() );
		
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			rkPacket.SetPosBegin();
			pLocal->SendCancelCash( rkPacket );
		}
		//hryoon test

		//User *temp = new User();
		//temp->OnTimeGateRequest(rkPacket);
		//delete temp;
		
		return;
	}

	// Move Server
	if( !pUserParent->IsUserOriginal() ) 
	{
		PACKET_GUARD_VOID( rkPacket.Read(szBillingGUID) );
		
		DWORD dwSSMsgType = 0;
		switch( rkPacket.GetPacketID() )
		{
		case BSTPK_GET_CASH_RESULT:
			dwSSMsgType = SSTPK_GET_CASH_RESULT;
			break;
		case BSTPK_OUTPUT_CASH_RESULT:
			dwSSMsgType = SSTPK_OUTPUT_CASH_RESULT;
			break;
		case BSTPK_LOGIN_RESULT:
			dwSSMsgType = SSTPK_BILLING_LOGIN_RESULT;
			break;
		case BSTPK_REFUND_CASH_RESULT:
			dwSSMsgType = SSTPK_BILLING_REFUND_CASH_RESULT;
			break;
		case BSTPK_USER_INFO_RESULT:
			dwSSMsgType = SSTPK_BILLING_USER_INFO_RESULT;
			break;
		case BSTPK_AUTOUPGRADE_LOGIN_RESULT:
			dwSSMsgType = SSTPK_BILLING_AUTOUPGRADE_LOGIN_RESULT;
			break;
		case BSTPK_PCROOM_RESULT:
			dwSSMsgType = SSTPK_BILLING_PCROOM_RESULT;
			break;
		case BSTPK_OTP_RESULT:
			dwSSMsgType = SSTPK_BILLING_OTP_RESULT;
			break;
		case BSTPK_GET_MILEAGE_RESULT:
			dwSSMsgType = SSTPK_BILLING_GET_MILEAGE_RESULT;
			break;
		case BSTPK_ADD_MILEAGE_RESULT:
			dwSSMsgType = SSTPK_BILLING_ADD_MILEAGE_RESULT;
			break;
		case BSTPK_IPBONUS_RESULT:
			dwSSMsgType = SSTPK_BILLING_IPBONUS_RESULT;
			break;
		case BSTPK_ADD_CASH_RESULT:
			dwSSMsgType = SSTPK_BILLING_ADD_CASH_RESULT;
			break;
		case BSTPK_FILL_CASH_URL_RESULT:
			dwSSMsgType = SSTPK_BILLING_FILL_CASH_URL_RESULT;
			break;
		case BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT:
			dwSSMsgType = SSTPK_BILLING_SUBSCRIPTION_RETRACT_CHECK_RESULT;
			break;
		case BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT:
			dwSSMsgType = SSTPK_BILLING_SUBSCRIPTION_RETRACT_RESULT;
			break;
		case BSTPK_SESSION_CONTROL_RESULT:
			dwSSMsgType = SSTPK_SESSION_CONTROL_RESULT;
			break;
		case BSTPK_TIMEOUT_BILLINGGUID:
			dwSSMsgType = SSTPK_TIMEOUT_BILLINGGUID;
			break;
		case BSTPK_REQUEST_TIME_CASH_RESULT:
			dwSSMsgType = SSTPK_REQUEST_TIME_CASH_RESULT;
			break;
		default:
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BillingRelayServer::OnBillingPacketParsing SS Msg Type is Error : %s:%s:0x%x", pUserParent->GetPublicID().c_str(), szBillingGUID.c_str(), rkPacket.GetPacketID() );
			return;
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BillingRelayServer::OnBillingPacketParsing Send Other Server : %s:%s:0x%x:0x%x", pUserParent->GetPublicID().c_str(), szBillingGUID.c_str(), rkPacket.GetPacketID(), dwSSMsgType );

		UserCopyNode *pUser = static_cast<UserCopyNode*> ( pUserParent );
		SP2Packet kPacket( dwSSMsgType );
		kPacket.SetDataAdd( (char*) rkPacket.GetData(), rkPacket.GetDataSize() );
		pUser->SendMessage( kPacket );
		return;
	}

	// This Server
	User *pUser = static_cast< User* > ( pUserParent );

	switch( rkPacket.GetPacketID() )
	{
	case BSTPK_GET_CASH_RESULT:
		pUser->OnBillingGetCash( rkPacket );
		break;
	case BSTPK_OUTPUT_CASH_RESULT:
		pUser->OnBillingOutputCash( rkPacket );
		break;
	case BSTPK_LOGIN_RESULT:
		pUser->OnBillingLogin( rkPacket );
		break;
	case BSTPK_REFUND_CASH_RESULT:
		pUser->OnBillingRefundCash( rkPacket );
		break;
	case BSTPK_USER_INFO_RESULT:
		pUser->OnBillingUserInfo( rkPacket );
		break;
	case BSTPK_AUTOUPGRADE_LOGIN_RESULT:
		pUser->OnBillingAutoUpgradeLogin( rkPacket );
		break;
	case BSTPK_PCROOM_RESULT:
		pUser->OnBillingPCRoom( rkPacket );
		break;
	case BSTPK_OTP_RESULT:
		pUser->OnBillingAutoUpgradeOTP( rkPacket );
		break;
	case BSTPK_GET_MILEAGE_RESULT:
		pUser->OnBillingGetMileage( rkPacket );
		break;
	case BSTPK_ADD_MILEAGE_RESULT:
		pUser->OnBillingAddMileage( rkPacket );
		break;
	case BSTPK_IPBONUS_RESULT:
		pUser->OnBillingIPBonus( rkPacket );
		break;
	case BSTPK_ADD_CASH_RESULT:
		pUser->OnBillingAddCash( rkPacket );
		break;
	case BSTPK_FILL_CASH_URL_RESULT:
		pUser->OnBillingFillCashUrl( rkPacket );
		break;
	case BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT:
		pUser->OnBillingSubscriptionRetractCheck( rkPacket );
		break;
	case BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT:
		pUser->OnBillingSubscriptionRetract( rkPacket );
		break;
	case BSTPK_SESSION_CONTROL_RESULT:
		pUser->OnSessionControl( rkPacket );
		break;
	case BSTPK_TIMEOUT_BILLINGGUID:
		pUser->OnBillingTimeoutBillingGUID( rkPacket );
		break;
	case BSTPK_DAUM_SHUTDOWN_CHECK:
		pUser->OnDaumAdultCheckRsp( rkPacket );
		break;
	case BSTPK_REQUEST_TIME_CASH_RESULT:
		pUser->OnBillingTimeCashResult(rkPacket);
		break;
	default:
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BillingRelayServer::OnBillingPacketParsing Billing Msg Type is Error : %s:%s:0x%x", pUserParent->GetPublicID().c_str(), szBillingGUID.c_str(), rkPacket.GetPacketID() );
		break;
	}
}

void BillingRelayServer::GenerateBillingServerInfo()
{
	int iIndex = g_ServerNodeManager.GetServerIndex() % m_vServerIP.size();

	m_szBillingIP	= m_vServerIP[iIndex].c_str();
	m_iBillingPort	= m_vServerPort[iIndex];
}

void BillingRelayServer::SetBillingServerInfo(std::vector<std::string>& vServerIP, std::vector<int>& vServerPort)
{
	m_vServerIP		= vServerIP;
	m_vServerPort	= vServerPort;
}

void BillingRelayServer::OnClose()
{
	OnDestroy();
}

void BillingRelayServer::SendUserInfo()
{
	ioHashString publicIP;
	ioHashString privateIP;
	ioHashString privateID;
	ioHashString chanID;
	static vUser vUserInfos;
	vUserInfos.clear();

	if(GetReconnectState())
	{
		int userMax = 0;
		int userCount = 0;

		userMax = g_UserNodeManager.GetNodeSize();
		LOG.PrintTimeAndLog(0,"Send UserInfoSessionServer Count :%d",userMax);
		int loopCount = 0;
		int iStart = 0;
		while(1)
		{
			SP2Packet pk(BSTPK_SESSION_CONTROL);
			PACKET_GUARD_VOID( pk.Write(NexonReconnect) );

			loopCount = BILLING_RECONNECT_USER_COUNT;

			if((userMax - userCount) < BILLING_RECONNECT_USER_COUNT)
				loopCount = userMax - userCount;

			PACKET_GUARD_VOID( pk.Write(loopCount) );
			
			g_UserNodeManager.GetUserInfos(vUserInfos, iStart, iStart + loopCount);

			for(int i =0; i< (int)vUserInfos.size(); i++)//BILLING_RECONNECT_USER_COUNT 만큼 묶어서 보냄 
			{
				User* userInfo = vUserInfos[i];
				
				if(userInfo)
				{
					publicIP = userInfo->GetPublicIP();
					privateIP = userInfo->GetPrivateIP();
					privateID = userInfo->GetPrivateID();
					chanID = userInfo->GetChannelingUserNo();

					pk.Write(userInfo->GetChannelingType());
					pk.Write(userInfo->GetUserIndex());
					pk.Write(chanID);
					pk.Write(publicIP);
					pk.Write(privateIP);
					pk.Write(privateID);
					pk.Write(g_ServerNodeManager.GetServerIndex()); // 이건 고민중 
				}
				else
					return;

				userCount++;
			}
			iStart += loopCount;

			if(loopCount < BILLING_RECONNECT_USER_COUNT)
				break;

			SendMessage(pk);
		}
	}
}
