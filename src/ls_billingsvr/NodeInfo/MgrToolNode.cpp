#include "../stdafx.h"

#include "../MainProcess.h"
#include "../NodeInfo/GoodsManager.h"
#include "../ioProcessChecker.h"

#include "../Network/GameServer.h"
#include "../Network/ioPacketQueue.h"

#include "ServerNodeManager.h"

#include "../database/logdbclient.h"
#include "../Local/ioLocalManager.h"
#include "../Version.h"
#include "../ThreadPool/ioThreadPool.h"

#include "MgrToolNodeManager.h"
#include "MgrToolNode.h"
 
#include <strsafe.h>
#include <iostream>

extern CLog LOG;

bool MgrToolNode::m_bUseSecurity = false;
int  MgrToolNode::m_iSecurityOneSecRecv = 30;

void MgrToolNode::LoadHackCheckValue()
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "SECURITY" );

	if( kLoader.LoadInt( "ON", 0 ) == 1 )
		m_bUseSecurity = true;
	else
		m_bUseSecurity = false;

	m_iSecurityOneSecRecv = kLoader.LoadInt( "ONE_SEC_RCV", 30 );
}

MgrToolNode::MgrToolNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();

	if( m_bUseSecurity )
	{
		SetNS( new ServerSecurity );
	}
}

MgrToolNode::~MgrToolNode()
{	
}

void MgrToolNode::InitData()
{
	char szTempGUID[USER_GUID_NUM_PLUS_ONE]="";
	CreateGUID(szTempGUID, sizeof(szTempGUID) );
	m_szGUID = szTempGUID;
	m_szMgrToolIP.Clear();
	m_szID.Clear();
	m_billingIndex = -1;


}

void MgrToolNode::OnCreate()
{
	CConnectNode::OnCreate();
	InitData();

	if( m_bUseSecurity )
	{		
		ServerSecurity *pSS = (ServerSecurity *)m_pNS;
		if( pSS ) 
		{
			pSS->InitDoSAttack( m_iSecurityOneSecRecv );
			pSS->InitState( m_socket );		
		}
	}
	m_dwCurrentTime = TIMEGETTIME();
}

void MgrToolNode::OnDestroy()
{
	CConnectNode::OnDestroy();
}

bool MgrToolNode::CheckNS( CPacket &rkPacket )
{
	if( m_pNS == NULL ) return true;

	ServerSecurity *pSS = (ServerSecurity*)m_pNS;
	if( !pSS->IsCheckSum( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "MgrToolNode::CheckNS Check Sum Fail!! [0x%x]", rkPacket.GetPacketID() );
		ExceptionClose( 0 );
		return false;
	}

	if( !pSS->CheckState( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "MgrToolNode::CheckNS State Not Same Client:%d, Server:%d [0x%x]", rkPacket.GetState(), pSS->GetRcvState(), rkPacket.GetPacketID() );
		ExceptionClose( 0 );
		return false;
	}

	if( !pSS->UpdateReceiveCount() )
	{
		LOG.PrintTimeAndLog( 0, "MgrToolNode::CheckNS ONE SEC MANY PACKET(%d)!! [0x%x]", pSS->GetRcvCount(), rkPacket.GetPacketID() );
		ExceptionClose( 0 );
		return false;
	}

	return true;
}


int MgrToolNode::GetConnectType()
{
	return CONNECT_TYPE_MANAGERTOOL;
}

bool MgrToolNode::IsGhostSocket()
{
	if( TIMEGETTIME() - m_dwCurrentTime > 120000 )
	{
		m_dwCurrentTime = TIMEGETTIME();
		return true;
	}
	return false;
}

bool MgrToolNode::SendMessage( CPacket &rkPacket )
{
	g_ProcessChecker.UserSendMessage( rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( rkPacket, TRUE );
}

void MgrToolNode::SessionClose( BOOL safely )
{
	if(IsActive())
	{
		CPacket packet(ITPK_CLOSE_SESSION);
		ReceivePacket( packet );
	}
}

void MgrToolNode::CreateGUID(OUT char *szGUID, IN int iSize)
{
	char szLongGUID[MAX_PATH]="";

	GUID guid;
	CoCreateGuid(&guid);
	StringCbPrintf(szLongGUID,sizeof(szLongGUID), "%04X%04X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		HIWORD(guid.Data1), LOWORD(guid.Data1), guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	StringCbCopyN(szGUID, iSize, szLongGUID, iSize-1);
}


void MgrToolNode::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void MgrToolNode::OnRequestConnect( SP2Packet &rkPacket )
{
	int iBillingIndex = 0;
	rkPacket >> iBillingIndex;
	if( m_billingIndex < 0 )
		m_billingIndex = iBillingIndex;

	int iUserCount = g_UserInfoManager->GetUserInfoCount();
	

	SP2Packet kPacket( STPKM_ANSWER_BILLING_CONNECT );
	kPacket << m_billingIndex;
	kPacket << iUserCount;	//모니터링 툴 UserInfoCount 컬럼에 표시 

	SendMessage( kPacket );
}

//모니터링 툴에 유저 정보 보냄 
void MgrToolNode::OnRequestServerUserCount( SP2Packet &rkPacket )
{
	SP2Packet kPacket( STPKM_BILLING_USER );
	int iUserCount = g_UserInfoManager->GetUserInfoCount();
	kPacket << iUserCount;
	SendMessage( kPacket );
}

//빌링서버에서 요청시 (메시지 창으로 보여짐)
void MgrToolNode::OnRequestServerInfo( SP2Packet &rkPacket )
{
	int usingCount[4] = {0,};
	int remainCount[4] = {0,};
	g_RecvQueue.GetPoolCount( usingCount, remainCount );
	
	 BOOL bLogDB = TRUE;
	 if( g_LogDBClient.IsActive() )
	 {
		 bLogDB = TRUE;
	 }
	 else
	 {
		 bLogDB = FALSE;
	 }
 
	 BOOL bMgameBilllingServer = TRUE;
	 
	 if( g_MgameBillingServer.IsActive() )
		bMgameBilllingServer = TRUE;
	 else
		bMgameBilllingServer = FALSE;

	//WEMADE BUY SERVER INFO
	 BOOL bWeMaedeBuyServer = TRUE;
	if( g_WemadeBuyServer.IsActive() )
		bWeMaedeBuyServer = TRUE;
	else
		bWeMaedeBuyServer = FALSE;
		 
	//Tooniland SERVER INFO
	BOOL bToonilandBillingServer = TRUE;
	if( g_ToonilandBillingServer.IsActive() )
		bToonilandBillingServer = TRUE;
	else
		bToonilandBillingServer = FALSE;

	//Nexon SERVER INFO
	BOOL bNexonServer = TRUE;
	if( g_NexonSessionServer.IsActive() )
		bNexonServer = TRUE;
	else
		bNexonServer = FALSE;

	ioHashString strFile;
	ioHashString strFileName;
	strFile = STRFILEVER;
	strFileName = STRINTERNALNAME;

	SP2Packet kPacket( STPKM_SERVER_INFO_ACK );
	kPacket << BILLING_ANSWER;	//billing type 으로 보냄
	kPacket << m_billingIndex;
	kPacket << (DWORD)TIMEGETTIME();
	kPacket << g_App.GetPublicIP();
	kPacket << g_App.GetPort();
	kPacket << ThreadManager::GetInstance()->GetHandleCount();
	kPacket << g_ServerNodeManager.GetNodeSize();
	kPacket << g_ServerNodeManager.GetDestroyNodeSize();
	kPacket << g_ServerNodeManager.RemainderNode();
	kPacket << usingCount[0]; 
	kPacket << usingCount[1];
	kPacket << usingCount[2];
	kPacket << usingCount[3];
	kPacket << remainCount[0];
	kPacket << remainCount[1];
	kPacket << remainCount[2];
	kPacket << remainCount[3];
	kPacket << bLogDB;
	kPacket << bMgameBilllingServer;
	kPacket << bWeMaedeBuyServer;
	kPacket << bToonilandBillingServer;
	kPacket << bNexonServer;
	kPacket << g_UserInfoManager->GetMemoryPoolCount();
	kPacket << g_UserInfoManager->GetUserInfoCount();
	kPacket << g_ThreadPool.GetNodeSize(); 
	kPacket << g_ThreadPool.RemainderNode(); 
	kPacket << g_ThreadPool.GetActiveThreadCount(); 
	kPacket << g_ThreadPool.GetThreadCount();
	kPacket << strFile;
	kPacket << strFileName;

	SendMessage( kPacket );
}

void MgrToolNode::OnServerReloadINI( SP2Packet &rkPacket )
{
	BOOL reloadResult = FALSE;
	ioHashString publicIP = g_App.GetPublicIP();
	ioHashString mgrIP;
	rkPacket >> mgrIP;

	SP2Packet kPacket( STPKM_BILLINGSERVER_RELOAD_INI );
	

	LOG.PrintTimeAndLog( 0, "ReloadINI Start (Monitoring IP:%s Monitoring PORT:%d ) ",  mgrIP.c_str(), g_App.GetMgrPort() );

	if( !g_GoodsMgr.ReloadINI( "config/GoodsList.ini", "config/GoodsName.ini" ) )
	{
		reloadResult =  FALSE;
		LOG.PrintTimeAndLog( 0, "ReloadINI Fail (Monitoring IP:%s Monitoring PORT:%d )",  mgrIP.c_str(), g_App.GetMgrPort() );
	}
	else
	{
		reloadResult = TRUE;
		LOG.PrintTimeAndLog( 0, "ReloadINI Success (Monitoring IP:%s Monitoring PORT:%d )",  mgrIP.c_str(), g_App.GetMgrPort() );
	}
	
	kPacket << m_billingIndex;
	kPacket << reloadResult;

	SendMessage( kPacket );

}

//넥슨 pc 방 유저일 경우만 SendLogoutpacket 
void MgrToolNode::OnRequestNexonPCRoomLogout( SP2Packet &rkPacket )
{
	UserInfoManager::NexonUserInfoList* userInfos = g_UserInfoManager->GetUserInfoList();

	if(userInfos == NULL)
		return;

	POSITION pos = userInfos->GetHeadPosition();
	while( pos != NULL )
	{
		NexonUserInfo* tempUser = userInfos->GetAt( pos );
		
		if(tempUser == NULL)
		{
			LOG.PrintTimeAndLog(0,"%s Nexon PCRoom User Null", __FUNCTION__);		
		}
		else if( tempUser->pcRoomState == 1 )
		{
			g_NexonSessionServer.SendLogoutPacketForPCRoomUser( tempUser->userIndex );

			LOG.PrintTimeAndLog(0,"%s Nexon PCRoom LogOutUser(%s)", __FUNCTION__, tempUser->chanID.c_str());		


			g_NexonSessionServer.SendLoginPacket(tempUser->chanID.c_str(), 
											tempUser->privateID.c_str(), 
											tempUser->publicIP.c_str(), 
											tempUser->privateIP.c_str(),
											tempUser->chType);
			
			LOG.PrintTimeAndLog(0,"%s Nexon PCRoom LogInUser(%s)", __FUNCTION__, tempUser->chanID.c_str());		

		}

		userInfos->GetNext( pos );
	}
}

void MgrToolNode::PacketParsing( CPacket &packet )
{
	m_dwCurrentTime = TIMEGETTIME();

	SP2Packet &kPacket = (SP2Packet&)packet;

	FUNCTION_TIME_CHECKER( 100000.0f, kPacket.GetPacketID() );          // 0.1 초 이상 걸리면로그 남김

	switch( kPacket.GetPacketID() )
	{
	case ITPK_CLOSE_SESSION:
		OnClose( kPacket );
		break;
	
	case CTPKM_REQUEST_NUM_CONNECT:
		OnRequestConnect( kPacket );
		break;

	case CTPKM_SERVER_INFO_REQUEST:
		OnRequestServerInfo( kPacket );
		break;

	case CTPKM_BILLINGSERVER_USER_REQUEST:
		OnRequestServerUserCount( kPacket );		
		break;

	case CTPKM_BILLINGSERVER_RELOAD_INI:
		OnServerReloadINI( kPacket );
		break;

	case CTPKM_BILLINGSERVER_NEXONPCROOM_LOGOUT:
		OnRequestNexonPCRoomLogout( kPacket );
		break;



	default:
		LOG.PrintTimeAndLog( 0, "MgrToolNode::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
		break;
	}
}

void MgrToolNode::OnClose( SP2Packet &packet  )
{
	g_MgrTool.RemoveNode( this );
}
