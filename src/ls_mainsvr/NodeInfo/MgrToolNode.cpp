#include "../stdafx.h"

#include "../MainProcess.h"
#include "../ioProcessChecker.h"

#include "../Network/GameServer.h"
#include "../Network/ioPacketQueue.h"

#include "ServerNodeManager.h"
#include "EventGoodsManager.h"
#include "MgrToolNodeManager.h"
#include "MgrToolNode.h"
#include "../Shutdown.h"
#include "../ServerCloseManager.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "SpecialShopManager.h"
#include "../NodeInfo/MatchNodeManager.h"
#include <strsafe.h>
#include <iostream>
using namespace std;

extern CLog OperatorLOG;

bool MgrToolNode::m_bUseSecurity = false;
int  MgrToolNode::m_iSecurityOneSecRecv = 30;

void MgrToolNode::LoadHackCheckValue()
{
	ioINILoader kLoader( "ls_config_main.ini" );
	kLoader.SetTitle( "SECURITY" );

	if( kLoader.LoadInt( "ON", 0 ) == 1 )
		m_bUseSecurity = true;
	else
		m_bUseSecurity = false;

	m_iSecurityOneSecRecv = kLoader.LoadInt( "ONE_SEC_RCV", 30 );
}

MgrToolNode::MgrToolNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	static DWORD dwIndexCount = 0;
	m_dwIndex = dwIndexCount++;

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

void MgrToolNode::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
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
		OnRequestNumConnect( kPacket );
		break;
	case CTPKM_ANNOUNCE: // 미사용
		OnAnnounce( kPacket );
		break;
	case CTPKM_UPDATE_CLIENT_VERSION:
		OnUpdateClientVersion( kPacket );
		break;
	case CTPKM_LOAD_CS3_FILE:
		OnLoadCS3File( kPacket );
		break;
	case CTPKM_MONITOR_IP:
		OnMonitorIP( kPacket );
		break;
	case CTPKM_CS3_FILE_VERSION:
		OnCS3FileVersion( kPacket );
		break;
	case CTPKM_SERVER_INFO_REQUEST:
		OnServerInfoReq( kPacket );
		break;
	case CTPKM_MAINSERVER_EXIT:
		OnMainServerExit( kPacket );
		break;
	case CTPKM_MAINSERVER_QUICKALLEXIT:
	case CTPKM_MAINSERVER_SAFETYALLEXIT:
		OnGameServerExit( kPacket );
		break;
	case CTPKM_MAINSERVER_RELOADCLOSEINFO:
		OnReloadCloseINI( kPacket );
		break;
	case CTPKM_MAINSERVER_DBAGENT_EXTEND: // 미사용
		g_MainProc.SetDBAgentExtend( true );
		break;
	case CTPKM_MAINSERVER_GAMESERVER_OPTION: 
		g_MainProc.SetGameServerOption( true );
		break;
	case CTPKM_MAINSERVER_GAMESERVER_RELOADINI:
		OnReloadGameServerINI( kPacket );
		break;
	case CTPKM_MAINSERVER_EXTRAITEM_INI: // 미사용
		g_MainProc.SetExtraItemGrowthCatalyst( true );
		break;
	case CTPKM_MAINSERVER_EVENTSHOP_INI: // 미사용
		g_MainProc.SetEventShopReload( true );
		break;
	case CTPKM_MAINSERVER_GS_SETNAGLE_REFCOUNT:
		OnGameServerSetNagleRefCount( kPacket );
		break;
	case CTPKM_MAINSERVER_GS_SETNAGLE_TIME:
		OnGameServerSetNagleTime( kPacket );
		break;
	case CTPKM_GAMESERVER_PROTOCAL:
		OnReciveToolDataSendGameServer( kPacket );
		break;
	case CTPKM_MAINSERVER_ADMINCOMMAND :
		OnAdminCommand( kPacket );
		break;
	case CTPKM_MAINSERVER_RESETEVENTSHOP :
		OnResetEventShop( kPacket );
		break;
	case CTPKM_WHITELIST_REQUEST :
		OnWhiteListReq( kPacket );
		break;
	case CTPKM_MAINSERVER_RESETBINGONUMBER :
		OnResetBingoNumber( kPacket );
		break;
	case CTPKM_MAINSERVER_RESET_OLDMISSIONDATA :
		OnResetOldMissionData( kPacket );
		break;

	case CTPKM_MTOOL_GAMESERVER_PROTOCAL:
		OnMToolReciveToolDataSendGameServer( kPacket );
		break;

	case CTPKM_MTOOL_MAINSERVER_EXIT:
		OnMToolMainServerExit( kPacket );
		break;
	case CTPKM_MTOOL_MAINSERVER_QUICKALLEXIT:
	case CTPKM_MTOOL_MAINSERVER_SAFETYALLEXIT:
		OnMToolGameServerExit( kPacket );
		break;

	case CTPKM_MTOOL_MAINSERVER_GAMESERVER_RELOADINI:
		OnMToolReloadGameServerINI( kPacket );
		break;

	case CTPKM_MTOOL_MAINSERVER_RELOADCLOSEINFO:
		OnMToolReloadCloseINI( kPacket );
		break;

// 해외용 서버 라이센스 체크
#if defined( SRC_OVERSEAS )
	case CTPKM_LICENSE_REQUEST :
		OnRequestLicense( kPacket );
		break;
#endif

	default:
		LOG.PrintTimeAndLog( 0, "MgrToolNode::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
		break;
	}
}

void MgrToolNode::OnClose( SP2Packet &packet  )
{
	g_MgrTool.RemoveNode( this );
}

void MgrToolNode::OnRequestNumConnect( SP2Packet &rkPacket )
{
	//서버 정보 전부 전송
	SP2Packet kPacket( STPKM_ANSWER_NUM_CONNECT );
	g_ServerNodeManager.FillServerInfo( kPacket );
	SendMessage( kPacket );
}

void MgrToolNode::OnAnnounce( SP2Packet &rkPacket )
{
	//SP2Packet kPacket( MSTPK_ADMIN );
	//kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	//g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void MgrToolNode::OnUpdateClientVersion( SP2Packet &rkPacket )
{
	int iClientNumber = 0;
	bool bUseClientVersion = false;
	int  iClientVersion    = 0;
	rkPacket >> iClientNumber;
	rkPacket >> bUseClientVersion;
	if( bUseClientVersion )
		rkPacket >> iClientVersion;

	g_MainProc.SaveClientVersion( bUseClientVersion, iClientVersion );

	SP2Packet kPacket( MSTPK_UPDATE_CLIENT_VERSION );
	kPacket << iClientNumber;
	kPacket << GetGUID();
	kPacket << bUseClientVersion;
	if( bUseClientVersion )
		kPacket << iClientVersion;

	g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void MgrToolNode::OnLoadCS3File( SP2Packet &rkPacket )
{
	ioHashString szIP;
	int iVersion = 0;
	int iChange  = 0;
	rkPacket >> szIP;
	rkPacket >> iVersion;
	rkPacket >> iChange;

	SP2Packet kPacket( MSTPK_LOAD_CS3_FILE );
	kPacket << GetGUID();
	kPacket << iVersion;
	kPacket << iChange;
	if( szIP.IsEmpty() )
		g_ServerNodeManager.SendMessageAllNode( kPacket );
	else
		g_ServerNodeManager.SendMessageIP( szIP, kPacket );

	LOG.PrintTimeAndLog( 0, "MgrToolNode::OnLoadCS3File : %s:%s:%d:%s:%d", GetGUID().c_str(), m_szMgrToolIP.c_str(), iVersion, szIP.c_str(), iChange );
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

void MgrToolNode::OnMonitorIP( SP2Packet &rkPacket )
{
	rkPacket >> m_szMgrToolIP;

	SP2Packet kPacket( STPKM_ALL_SERVER_INFO );
	kPacket << g_MainProc.IsUseManageToolPrivateIP();
	g_ServerNodeManager.FillToolServerInfo( kPacket );
	SendMessage( kPacket );

	//OperatorLOG.PrintTimeAndLog(0, "MONITOR : [%s]", m_szMgrToolIP.c_str());
}

void MgrToolNode::OnCS3FileVersion( SP2Packet &rkPacket )
{
	ioHashString szIP;
	rkPacket >> szIP;

	SP2Packet kPacket( MSTPK_CS3_FILE_VERSION );
	kPacket << GetGUID();
	if( szIP.IsEmpty() )
		g_ServerNodeManager.SendMessageAllNode( kPacket );
	else
		g_ServerNodeManager.SendMessageIP( szIP, kPacket );


	LOG.PrintTimeAndLog( 0, "MgrToolNode::OnCS3FileVersion : %s:%s:%s", GetGUID().c_str(), m_szMgrToolIP.c_str(), szIP.c_str() );
}

void MgrToolNode::OnServerInfoReq( SP2Packet &rkPacket )
{
	ioHashString	szIP, szGUID;
	int port = 0;
	rkPacket >> szIP >> port;

	if( szIP.c_str() == NULL || port == 0 )
	{
		LOG.PrintTimeAndLog( 0, "RETURN] ip : %s, port : %d", szIP, port );
		return;
	}

	// Check
	if( strcmp( szIP.c_str(), g_MainProc.GetPublicIP().c_str() ) == 0 && port == g_MainProc.GetPort() )
	{
		// MainServer

		SP2Packet kPacket( STPKM_SERVER_INFO_ACK );
		MAINSERVERINFO	info;
		g_MainProc.DrawModule( info );
		kPacket << (int)eServerType_MainServer;
		kPacket << info;

		SendMessage( kPacket );
	}
	else
	{
		// GameServer
		szGUID = GetGUID();

		SP2Packet packet( MSTPK_SERVER_INFO_REQ );
		packet << szGUID;
		g_ServerNodeManager.SendMessageIPnPort( szIP, port, packet );
	}
}

void MgrToolNode::OnMainServerExit( SP2Packet &rkPacket )
{
	if(GetID().IsEmpty()) return;

	OperatorLOG.PrintTimeAndLog(0, "CMD_MAINEXIT : [%s(%s)]", GetID().c_str(), m_szMgrToolIP.c_str());

	g_MainProc.Shutdown( SHUTDOWN_QUICK );
}

void MgrToolNode::OnGameServerExit( SP2Packet &rkPacket )
{
	if(GetID().IsEmpty()) return;

	DWORD dwType = 0;
	rkPacket >> dwType;

	OperatorLOG.PrintTimeAndLog(0, "CMD_GAMEEXIT : [%s(%s)] [%d]", GetID().c_str(), m_szMgrToolIP.c_str(), dwType);

	g_MainProc.AllServerExit( dwType, false );
}

void MgrToolNode::OnReloadCloseINI( SP2Packet &rkPacket )
{
	if(GetID().IsEmpty()) return;

	OperatorLOG.PrintTimeAndLog(0, "CMD_RELOADCLOSEINI : [%s(%s)]", GetID().c_str(), m_szMgrToolIP.c_str());

	g_ServerCloseMgr.LoadCloseServerInfo();
}

void MgrToolNode::OnReloadGameServerINI( SP2Packet &rkPacket )
{
	if(GetID().IsEmpty()) return;

	OperatorLOG.PrintTimeAndLog(0, "CMD_RELOADGAMEINI : [%s(%s)]", GetID().c_str(), m_szMgrToolIP.c_str());

	ioINILoader kLoader;
	if( kLoader.ReloadFile( "config/game_server_reload_list.ini" ) )
	{
		LOG.PrintTimeAndLog(0, "%s - INI file reload sucess", __FUNCTION__ );
	}
	else
	{
		LOG.PrintTimeAndLog(0, "%s - INI file reload failed!!", __FUNCTION__ );
	}

	kLoader.SetTitle( "common" );

	DWORDVec vINI;
	vINI.clear();

	int iMaxINI = kLoader.LoadInt( "max_ini", 0 );
	SP2Packet kPacket( MSTPK_GAME_SERVER_RELOAD_INI );
	kPacket << iMaxINI;
	for(int i = 0;i < iMaxINI;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "ini_type%d", i + 1 );
		int iKey = kLoader.LoadInt( szKey, -1 );
		kPacket << iKey;
		vINI.push_back( iKey );
	}
	g_ServerNodeManager.SendMessageAllNode( kPacket );	

	// 메인서버의 ini reload
	int icount = (int)vINI.size();
	for(int i=0 ; i<icount ; ++i)
	{
		switch( vINI[i] )
		{
		case RELOAD_RANKING:
			{
				g_MatchNodeManager.LoadINI();
			}
			break;
		}
	}
}

void MgrToolNode::OnGameServerSetNagleRefCount( SP2Packet& rkPacket )
{
	int refCount = 0;
	rkPacket >> refCount;

	SP2Packet	kPacket( MSTPK_GAMESERVER_SETNAGLE_REFCOUNT );
	kPacket << refCount;
	g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void MgrToolNode::OnGameServerSetNagleTime( SP2Packet& rkPacket )
{
	uint32 nagleTime = 0;
	rkPacket >> nagleTime;

	SP2Packet	kPacket( MSTPK_GAMESERVER_SETNAGLE_TIME );
	kPacket << nagleTime;
	g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void MgrToolNode::OnReciveToolDataSendGameServer( SP2Packet& rkPacket )
{
	if(GetID().IsEmpty()) return;

	ioHashString szIP, szGUID;
	int port = 0;
	DWORD protocol = 0;
	rkPacket >> szIP >> port >> protocol;
	BOOL bRelayServerState = FALSE;

	szGUID = GetGUID();

	switch( protocol )
	{
	//case CTPKM_GAMESERVER_RELOAD_HACK_CONSTANT:		protocal = MSTPK_GAMESERVER_RELOAD_HACK_CONSTANT;	break;
	//case CTPKM_GAMESERVER_RELOAD_USER_DISPERSION:	protocal = MSTPK_GAMESERVER_RELOAD_USER_DISPERSION;	break;
	//case CTPKM_GAMESERVER_RELOAD_PROCESS_INI:		protocal = MSTPK_GAMESERVER_RELOAD_PROCESS_INI;		break;
	//case CTPKM_GAMESERVER_RELOAD_INI_CLASS:			protocal = MSTPK_GAMESERVER_RELOAD_INI_CLASS;		break;
	//case CTPKM_GAMESERVER_RELOAD_INI_DECO:			protocal = MSTPK_GAMESERVER_RELOAD_INI_DECO;		break;
	//case CTPKM_GAMESERVER_RELOAD_INI_ETC:			protocal = MSTPK_GAMESERVER_RELOAD_INI_ETC;			break;
	//case CTPKM_GAMESERVER_RELOAD_INI_EVENT:			protocal = MSTPK_GAMESERVER_RELOAD_INI_EVENT;		break;
	//case CTPKM_GAMESERVER_RELOAD_QUEST_INI:			protocal = MSTPK_GAMESERVER_RELOAD_QUEST_INI;		break;
	//case CTPKM_GAMESERVER_RELOAD_MODE_INI:			protocal = MSTPK_GAMESERVER_RELOAD_MODE_INI;		break;
	//case CTPKM_GAMESERVER_RELOAD_PRESENT_INI:		protocal = MSTPK_GAMESERVER_RELOAD_PRESENT_INI;		break;
	//case CTPKM_GAMESERVER_RELOAD_FISHING_INI:		protocal = MSTPK_GAMESERVER_RELOAD_FISHING_INI;		break;
	//case CTPKM_GAMESERVER_RELOAD_EXCA:				protocal = MSTPK_GAMESERVER_RELOAD_EXCA;			break;
	//case CTPKM_GAMESERVER_RELOAD_ITEM_COMPOUND_INI:	protocal = MSTPK_GAMESERVER_RELOAD_ITEM_COMPOUND_INI;	break;
	//case CTPKM_GAMESERVER_RELOAD_EXTRAITEM_INI:		protocal = MSTPK_GAMESERVER_RELOAD_EXTRAITEM_INI;		break;
	//case CTPKM_GAMESERVER_RELOAD_TRADE_INI:			protocal = MSTPK_GAMESERVER_RELOAD_TRADE_INI;			break;
	//case CTPKM_GAMESERVER_RELOAD_LEVELMATCH_INI:	protocal = MSTPK_GAMESERVER_RELOAD_LEVELMATCH_INI;		break;
	//case CTPKM_GAMESERVER_RELOAD_ITEM_INIT_CONTROL:	protocal = MSTPK_GAMESERVER_RELOAD_ITEM_INIT_CONTROL;	break;
	//case CTPKM_GAMESERVER_RELOAD_CONFIG_INI:		protocal = MSTPK_GAMESERVER_RELOAD_CONFIG_INI;			break;
	case CTPKM_GAMESERVER_QUICK_EXIT:				protocol = MSTPK_GAMESERVER_QUICK_EXIT;					break;
	case CTPKM_GAMESERVER_SAFETY_EXIT:				protocol = MSTPK_GAMESERVER_SAFETY_EXIT;				break;
	case CTPKM_GAMESERVER_CHANGE_RELAYSVR_STATE:  
		{
			protocol = MSTPK_GAMESERVER_CHANGE_RELAYSERVER_STATE;
			
			rkPacket >> bRelayServerState;	
		}
		break;
	}

	SP2Packet kPacket( protocol );
	kPacket << szGUID;
	if(protocol == MSTPK_GAMESERVER_CHANGE_RELAYSERVER_STATE)
	{
		kPacket << bRelayServerState;
	}


	g_ServerNodeManager.SendMessageIPnPort( szIP, port, kPacket );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_RELAY : [%s] [%lu]", GetID().c_str(), protocol);
}

void MgrToolNode::OnAdminCommand( SP2Packet& rkPacket )
{
	// 관리자툴에서 사용하는 프로토콜
	int iType = 0;
	rkPacket >> iType;

	switch( iType )
	{
	case ADMINCOMMAND_KICK :
		OnAdminKickUser( iType, rkPacket );
		break;

	case ADMINCOMMAND_ANNOUNCE :
		OnAdminAnnounce( iType, rkPacket );
		break;

	case ADMINCOMMAND_ITEMINSERT :
		OnAdminItemInsert( iType, rkPacket );
		break;

	case ADMINCOMMAND_EVENTINSERT :
		OnAdminEventInsert( iType, rkPacket );
		break;

	case ADMINCOMMAND_AUTH :
		OnAdminAuth( iType, rkPacket );
		break;

	case ADMINCOMMAND_USERBLOCK:
		OnAdminUserBlock( iType, rkPacket );
		break;

	case ADMINCOMMAND_USERUNBLOCK:
		OnAdminUserUnblock( iType, rkPacket );
		break;

	case ADMINCOMMAND_SECRETSHOP:
		OnAdminRenewalSecretShop(rkPacket);
		break;

	case ADMINCOMMAND_COMPENSATION:
		OnAdminRegistCompensation(rkPacket);
		break;
	case ADMINCOMMAND_PRACTICE:
		OnAdminPracticeKickUserDelete(rkPacket);
		break;


	case ADMINCOMMAND_MTOOL_AUTH :
		OnAdminMToolAuth( iType, rkPacket );
		break;

	case ADMINCOMMAND_MTOOL_KICK :
		OnAdminMToolKickUser( iType, rkPacket );
		break;

	case ADMINCOMMAND_MTOOL_USERBLOCK:
		OnAdminMToolUserBlock( iType, rkPacket );
		break;

	case ADMINCOMMAND_MTOOL_USERUNBLOCK:
		OnAdminMToolUserUnblock( iType, rkPacket );
		break;

	case ADMINCOMMAND_MTOOL_ANNOUNCE :
		OnAdminMToolAnnounce( iType, rkPacket );
		break;

	case ADMINCOMMAND_MTOOL_ITEMINSERT :
		OnAdminMToolItemInsert( iType, rkPacket );
		break;

	case ADMINCOMMAND_MTOOL_EVENTINSERT :
		OnAdminMToolEventInsert( iType, rkPacket );
		break;
	case ADMINCOMMAND_MTOOL_PRACTICE:
		OnAdminMToolPracticeKickUserDelete(rkPacket);
		break;

	}
}

void MgrToolNode::OnResetEventShop( SP2Packet& rkPacket )
{
	// 테스트로 사용하는 이벤트상점의 데이타 초기화
	// type 1 : event count, 2 : clover count, 3: mileage count 4 : clear log
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case 0 :
		g_EventGoodsMgr.ResetBuyLog();
		break;

	case 1:
		g_EventGoodsMgr.ResetBuyCount( EventGoodsManager::ST_EVENT );
		break;

	case 2 :
		g_EventGoodsMgr.ResetBuyCount( EventGoodsManager::ST_CLOVER );
		break;

	case 3 :
		g_EventGoodsMgr.ResetBuyCount( EventGoodsManager::ST_MILEAGE );
		break;

	
	}
}

void MgrToolNode::OnResetBingoNumber( SP2Packet& rkPacket )
{
	OperatorLOG.PrintTimeAndLog(0, "Reset All BingoNumber : [%s(%s)]", GetID().c_str(), m_szMgrToolIP.c_str() );
	g_DBClient.OnResetBingoNumber();
}

void MgrToolNode::OnResetOldMissionData( SP2Packet& rkPacket )
{
	CTime kCurTime = CTime::GetCurrentTime();
	int iDay = 0, iHour= 0, iMin = 0, iSec = 0;
	iDay	= kCurTime.GetDay();
	iHour	= kCurTime.GetHour();
	iMin	= kCurTime.GetMinute();
	iSec	= kCurTime.GetSecond();
	CTimeSpan kSpanTime( iDay + 7, iHour, iMin, iSec ); //매월 초의 날에 -7일
	kCurTime -= kSpanTime;
	OperatorLOG.PrintTimeAndLog(0, "Reset Old Mission Data : [%s(%s)] - Delete Date Base %d-%d-%d",
		GetID().c_str(), m_szMgrToolIP.c_str() , kCurTime.GetYear(), kCurTime.GetMonth(), kCurTime.GetDay()  );
	g_DBClient.OnResetOldMissionData( kCurTime.GetYear(), kCurTime.GetMonth(), kCurTime.GetDay() );
}

void MgrToolNode::OnWhiteListReq( SP2Packet& rkPacket )
{
	BOOL bWhiteList = 0;
	rkPacket >> bWhiteList;

	OperatorLOG.PrintTimeAndLog(0, "CMD_WHITELIST : [%s(%s)] [%d]", GetID().c_str(), m_szMgrToolIP.c_str(), bWhiteList);

	SP2Packet kPacket( MSTPK_WHITELIST_REQUEST );
	kPacket << bWhiteList;
	g_ServerNodeManager.SendMessageAllNode( kPacket );
}


bool Authenticate( const char* section, ioHashString& szUserID, ioHashString& szPassword )
{
	char szTemp[64] = {};

	ioINILoader kLoader;
	if( kLoader.ReloadFile( "ls_auth.ini" ) )
	{
		LOG.PrintTimeAndLog(0, "%s - INI file reload sucess", __FUNCTION__ );
	}
	else
	{
		LOG.PrintTimeAndLog(0, "%s - INI file reload failed!!", __FUNCTION__ );
	}

	kLoader.LoadString( section, szUserID.c_str(), "", szTemp, sizeof(szTemp) );
	if(szPassword == szTemp)
	{
		return true;
	}
	return false;
}

void MgrToolNode::SetID(ioHashString& szID)
{
	m_szID = szID;
}

int MgrToolNode::AdminIDCheck(ioHashString& szUserID, ioHashString& szPassword)
{
	AdminLevels level = ADMIN_LEVEL_NONE;
	if(Authenticate("administrator", szUserID, szPassword))
	{
		return ADMIN_LEVEL_ADMINISTRATOR;
	}
	else if(Authenticate("operator", szUserID, szPassword))
	{
		return ADMIN_LEVEL_OPERATOR;
	}
	else if(Authenticate("developer", szUserID, szPassword))
	{
		return ADMIN_LEVEL_DEVELOPER;
	}

	return ADMIN_LEVEL_NONE;
}

void MgrToolNode::OnAdminKickUser( const int iType, SP2Packet& rkPacket )
{
	if(GetID().IsEmpty()) return;

	// 닉네임
	ioHashString szPublicID;
	rkPacket >> szPublicID;

	SP2Packet kPacket( MSTPK_ADMIN );
	kPacket << iType << szPublicID;
	g_ServerNodeManager.SendMessageAllNode( kPacket );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_KICK : [%s] [%s]", GetID().c_str(), szPublicID.c_str());
}

void MgrToolNode::OnAdminAnnounce( const int iType, SP2Packet& rkPacket )
{
	if(GetID().IsEmpty()) return;

	// 공지사항 공지타입 닉네임
	ioHashString szAnnounce, szPublicID;
	int iMsgType;

	rkPacket >> szAnnounce >> iMsgType;
	if(iMsgType != ANNOUNCE_TYPE_ALL)
	{
		rkPacket >> szPublicID;
	}

	SP2Packet kPacket( MSTPK_ADMIN );
	kPacket << iType << szAnnounce << iMsgType;
	if(iMsgType != ANNOUNCE_TYPE_ALL)
	{
		kPacket << szPublicID;
	}
	g_ServerNodeManager.SendMessageAllNode( kPacket );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_ANNOUNCE : [%s] [%s] [%s]", GetID().c_str(), szPublicID.c_str(), szAnnounce.c_str());
}

void MgrToolNode::OnAdminItemInsert( const int iType, SP2Packet& rkPacket )
{
	if(GetID().IsEmpty()) return;

	// 닉네임 선물타입 value1 value2
	int iPresentType = 0, iPresentValue1 = 0, iPresentValue2 = 0;
	int iPresentMent = 1;
	int iPublicIDState = 0;

	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );
	PACKET_GUARD_VOID( rkPacket.Read(iPresentType) );
	PACKET_GUARD_VOID( rkPacket.Read(iPresentValue1) );
	PACKET_GUARD_VOID( rkPacket.Read(iPresentValue2) );
	PACKET_GUARD_VOID( rkPacket.Read(iPresentMent) );
	PACKET_GUARD_VOID( rkPacket.Read(iPublicIDState) );

	SP2Packet kPacket( MSTPK_ADMIN );
	PACKET_GUARD_VOID( kPacket.Write(iType) );
	PACKET_GUARD_VOID( kPacket.Write(szPublicID) );
	PACKET_GUARD_VOID( kPacket.Write(iPresentType) );
	PACKET_GUARD_VOID( kPacket.Write(iPresentValue1) );
	PACKET_GUARD_VOID( kPacket.Write(iPresentValue2) );
	PACKET_GUARD_VOID( kPacket.Write(iPresentMent) );
	PACKET_GUARD_VOID( kPacket.Write(iPublicIDState) );

	ServerNode* pServerNode = g_ServerNodeManager.GetServerNode();
	if(pServerNode)
	{
		pServerNode->SendMessage( kPacket );

		OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_ITEMINSERT : success [%s] [%s] [%d] [%d] [%d]", GetID().c_str(), szPublicID.c_str(), iPresentType, iPresentValue1, iPresentValue2);
	}
	else
	{
		OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_ITEMINSERT : failed [%s] [%s] [%d] [%d] [%d]", GetID().c_str(), szPublicID.c_str(), iPresentType, iPresentValue1, iPresentValue2);
	}
}

void MgrToolNode::OnAdminEventInsert( const int iType, SP2Packet& rkPacket )
{
	if(GetID().IsEmpty()) return;

	//// 이벤트갯수 이벤트정보...
	int iValueCount = 0, iValues[64] = {};
	rkPacket >> iValueCount;

	SP2Packet kPacket( MSTPK_ADMIN );
	kPacket.SetDataAdd( (char*)rkPacket.GetData(), rkPacket.GetDataSize() );
	g_ServerNodeManager.SendMessageAllNode( kPacket );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_EVENTINSERT : [%s] count(%d)", GetID().c_str(), iValueCount);
	for(int i = 0 ; i < iValueCount ; i++)
	{
		rkPacket >> iValues[i];
		OperatorLOG.PrintTimeAndLog(0, "values : %d", iValues[i]);
	}
}

void MgrToolNode::OnAdminUserBlock( const int iType, SP2Packet& rkPacket )
{
	if(GetID().IsEmpty()) return;

	ioHashString szPublicID;
	ioHashString szReason;
	ioHashString szNote;
	int64		 iTime;
	int			 iBlockLevel;
	
	rkPacket >> szPublicID;
	rkPacket >> szReason;
	rkPacket >> szNote;
	rkPacket >> iTime;
	rkPacket >> iBlockLevel;

	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan spanTime(iTime);
	CTime limitTime = curTime + spanTime;
	
	ServerNode* pServerNode = g_ServerNodeManager.GetServerNode();

	if( pServerNode )
	{
		SP2Packet kPacket( MSTPK_ADMIN );
		kPacket << iType;
		kPacket << szPublicID;
		kPacket << iBlockLevel;
		pServerNode->SendMessage( kPacket );
	}

	g_DBClient.OnUserBlock( szPublicID, iBlockLevel, limitTime, m_szID, m_szMgrToolIP, szReason, szNote, GetIndex() );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_USERBLOCK : [%s] %s : %s / %d-%d-%d, %d:%d - %d", GetID().c_str(),
		szPublicID.c_str(), szNote.c_str(), limitTime.GetYear(), limitTime.GetMonth(), limitTime.GetDay(), limitTime.GetHour(), limitTime.GetMinute(), iBlockLevel);
}

void MgrToolNode::OnAdminUserUnblock( const int iType, SP2Packet& rkPacket )
{
	if(GetID().IsEmpty()) return;

	ioHashString szPublicID;
	ioHashString szReason;
	ioHashString szNote;
	
	rkPacket >> szPublicID;
	rkPacket >> szReason;
	rkPacket >> szNote;

	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan spanTime(3600*24);

	CTime yesterdayTime = curTime - spanTime;
	
	g_DBClient.OnUserBlock( szPublicID, 0, yesterdayTime, m_szID, m_szMgrToolIP, szReason, szNote, GetIndex() );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_USERUNBLOCK : [%s] %s ", GetID().c_str(), szPublicID.c_str() );
}


void MgrToolNode::OnAdminAuth( const int iType, SP2Packet& rkPacket )
{
	// 아이디 비번
	ioHashString szUserID, szPassword;
	rkPacket >> szUserID >> szPassword;
	if(szUserID.IsEmpty())		return;
	if(szPassword.IsEmpty())	return;
	
	bool bResult = false;
	AdminLevels level = ADMIN_LEVEL_NONE;

	ioHashString szID = GetID();
	if(szID.IsEmpty())
	{
		if(Authenticate("administrator", szUserID, szPassword))
		{
			bResult = true;
			level	= ADMIN_LEVEL_ADMINISTRATOR;
			SetID(szUserID);
		}
		else if(Authenticate("operator", szUserID, szPassword))
		{
			bResult = true;
			level	= ADMIN_LEVEL_OPERATOR;
			SetID(szUserID);
		}
		else if(Authenticate("developer", szUserID, szPassword))
		{
			bResult = true;
			level	= ADMIN_LEVEL_DEVELOPER;
			SetID(szUserID);
		}
	}

	SP2Packet kPacket( STPKM_ADMIN_RESPONSE );
	kPacket << iType << bResult << (int)level; 
	SendMessage( kPacket );

	char szIP[256];
	int iPort;
	GetPeerIP(szIP, sizeof(szIP), iPort);

	if( bResult )
	{
		OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_AUTH : success [%s(%s)] [%s] [%s]", GetID().c_str(), szIP, szUserID.c_str(), szPassword.c_str());
	}
	else
	{
		OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_AUTH : failed [%s(%s)] [%s] [%s]", GetID().c_str(), szIP, szUserID.c_str(), szPassword.c_str());
	}
}

void MgrToolNode::OnAdminRenewalSecretShop(SP2Packet& rkPacket )
{
	DWORD dwItemCode	= 0;
	int iItemCount		= 0;

	rkPacket >> dwItemCode >> iItemCount;

	if( !g_SpecialShopManager.IsOpen() )
		return;

	g_SpecialShopManager.RenewalGoodsCount(dwItemCode, iItemCount);
}

void MgrToolNode::ApplyUserBlockDB( ioHashString& szPublicID, LONG lSuccess )
{
	if( szPublicID.IsEmpty() ) return;

	SP2Packet kPacket( STPKM_ADMIN_RESPONSE );
	kPacket << ADMINCOMMAND_USERBLOCK << szPublicID << lSuccess;

	CConnectNode::SendMessage( kPacket );
}

void MgrToolNode::ApplyChangeUserBlockDB(int ClientNumber,  ioHashString& szPublicID, LONG lSuccess )
{
	if( szPublicID.IsEmpty() ) return;

	SP2Packet kPacket( STPKM_ADMIN_RESPONSE );
	kPacket << ADMINCOMMAND_MTOOL_USERBLOCK << ClientNumber << szPublicID << lSuccess;

	CConnectNode::SendMessage( kPacket );
}

void MgrToolNode::OnAdminRegistCompensation(SP2Packet& rkPacket )
{
	int iItemType		= 0;
	DWORD dwItemCode	= 0;
	int iValue			= 0;
	__int64 dwEndDate	= 0;

	rkPacket >> iItemType >> dwItemCode >> iValue >> dwEndDate;

	//게임서버에 Noti
	SP2Packet kPacket(MSTPK_REQ_REGIST_COMPENSATION);
	kPacket << iItemType << dwItemCode << iValue << dwEndDate;

	g_ServerNodeManager.SendMessageAllNode(kPacket);
}

void MgrToolNode::OnAdminPracticeKickUserDelete(SP2Packet& rkPacket )
{
	if(GetID().IsEmpty()) return;

	// 닉네임
	ioHashString szPublicID;
	rkPacket >> szPublicID;

	OperatorLOG.PrintTimeAndLog(0, "PracticeKickUserDelete %s", szPublicID.c_str());
	g_DBClient.OnPractice_BlockUserList(szPublicID.c_str());
}

// 해외용 서버 라이센스 체크
#if defined( SRC_OVERSEAS )
//게임서버에 라이센스 요청함
void MgrToolNode::OnRequestLicense( SP2Packet& rkPacket )
{
	SP2Packet kPacket( MSTPK_LICENSE_REQUEST );


	g_ServerNodeManager.SendMessageAllNode( kPacket );	//모든 게임서버에 라이센스 요청보냄
}
#endif


void MgrToolNode::OnAdminMToolAuth( const int iType, SP2Packet& rkPacket )
{
	// 아이디 비번
	int ClientNumber;
	ioHashString szUserID, szPassword;
	rkPacket >> ClientNumber >> szUserID >> szPassword;
	if(szUserID.IsEmpty())		return;
	if(szPassword.IsEmpty())	return;
	
	bool bResult = false;
	AdminLevels level = (AdminLevels)AdminIDCheck(szUserID, szPassword);

	SP2Packet kPacket( STPKM_ADMIN_RESPONSE );
	kPacket << iType << ClientNumber << bResult << (int)level; 
	SendMessage( kPacket );

	char szIP[256];
	int iPort;
	GetPeerIP(szIP, sizeof(szIP), iPort);

	if( bResult )
	{
		OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_AUTH : success [%s(%s)] [%s] [%s]", szUserID.c_str(), szIP, szUserID.c_str(), szPassword.c_str());
	}
	else
	{
		OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_AUTH : failed [%s(%s)] [%s] [%s]", szUserID.c_str(), szIP, szUserID.c_str(), szPassword.c_str());
	}
}

void MgrToolNode::OnAdminMToolKickUser( const int iType, SP2Packet& rkPacket )
{
	ioHashString szUserID, szPassword;
	ioHashString szPublicID;
	rkPacket >> szUserID >> szPassword;
	rkPacket >> szPublicID;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	SP2Packet kPacket( MSTPK_ADMIN );
	kPacket << ADMINCOMMAND_KICK << szPublicID;
	g_ServerNodeManager.SendMessageAllNode( kPacket );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_MTOOL_KICK : [%s] [%s]", GetID().c_str(), szPublicID.c_str());
}

void MgrToolNode::OnAdminMToolUserBlock( const int iType, SP2Packet& rkPacket )
{
	int ClientNumber;
	ioHashString szUserID, szPassword;
	ioHashString szPublicID;
	ioHashString szReason;
	ioHashString szNote;
	int64		 iTime;
	int			 iBlockLevel;
	
	rkPacket >> ClientNumber >> szUserID >> szPassword;
	rkPacket >> szPublicID;
	rkPacket >> szReason;
	rkPacket >> szNote;
	rkPacket >> iTime;
	rkPacket >> iBlockLevel;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan spanTime(iTime);
	CTime limitTime = curTime + spanTime;
	
	ServerNode* pServerNode = g_ServerNodeManager.GetServerNode();

	if( pServerNode )
	{
		SP2Packet kPacket( MSTPK_ADMIN );
		kPacket << ADMINCOMMAND_USERBLOCK;
		kPacket << szPublicID;
		kPacket << iBlockLevel;
		pServerNode->SendMessage( kPacket );
	}

	g_DBClient.OnChangeUserBlock(ClientNumber, szPublicID, iBlockLevel, limitTime, m_szID, m_szMgrToolIP, szReason, szNote, GetIndex() );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_MTOOL_USERBLOCK : [%s] %s : %s / %d-%d-%d, %d:%d - %d", szUserID.c_str(),
		szPublicID.c_str(), szNote.c_str(), limitTime.GetYear(), limitTime.GetMonth(), limitTime.GetDay(), limitTime.GetHour(), limitTime.GetMinute(), iBlockLevel);
}

void MgrToolNode::OnAdminMToolUserUnblock( const int iType, SP2Packet& rkPacket )
{
	int ClientNumber;
	ioHashString szUserID, szPassword;
	ioHashString szPublicID;
	ioHashString szReason;
	ioHashString szNote;
	
	rkPacket >> ClientNumber >> szUserID >> szPassword;
	rkPacket >> szPublicID;
	rkPacket >> szReason;
	rkPacket >> szNote;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	CTime curTime = CTime::GetCurrentTime();
	CTimeSpan spanTime(3600*24);

	CTime yesterdayTime = curTime - spanTime;
	
	g_DBClient.OnChangeUserBlock(ClientNumber,  szPublicID, 0, yesterdayTime, m_szID, m_szMgrToolIP, szReason, szNote, GetIndex() );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_MTOOL_USERUNBLOCK : [%s] %s ", szUserID.c_str(), szPublicID.c_str() );
}

void MgrToolNode::OnAdminMToolAnnounce( const int iType, SP2Packet& rkPacket )
{
	// 공지사항 공지타입 닉네임
	ioHashString szUserID, szPassword;
	ioHashString szAnnounce, szPublicID;
	int iMsgType;

	rkPacket >> szUserID >> szPassword;
	rkPacket >> szAnnounce >> iMsgType;
	if(iMsgType != ANNOUNCE_TYPE_ALL)
	{
		rkPacket >> szPublicID;
	}

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	SP2Packet kPacket( MSTPK_ADMIN );
	kPacket << ADMINCOMMAND_ANNOUNCE << szAnnounce << iMsgType;
	if(iMsgType != ANNOUNCE_TYPE_ALL)
	{
		kPacket << szPublicID;
	}
	g_ServerNodeManager.SendMessageAllNode( kPacket );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_MTOOL_ANNOUNCE : [%s] [%s] [%s]", GetID().c_str(), szPublicID.c_str(), szAnnounce.c_str());
}

void MgrToolNode::OnAdminMToolItemInsert( const int iType, SP2Packet& rkPacket )
{
	// 닉네임 선물타입 value1 value2
	ioHashString szUserID, szPassword;
	int iPresentType = 0, iPresentValue1 = 0, iPresentValue2 = 0;
	int iPresentMent = 1;
	int iPublicIDState = 0;

	ioHashString szPublicID;

	PACKET_GUARD_VOID( rkPacket.Read(szUserID) );
	PACKET_GUARD_VOID( rkPacket.Read(szPassword) );
	PACKET_GUARD_VOID( rkPacket.Read(szPublicID) );
	PACKET_GUARD_VOID( rkPacket.Read(iPresentType) );
	PACKET_GUARD_VOID( rkPacket.Read(iPresentValue1) );
	PACKET_GUARD_VOID( rkPacket.Read(iPresentValue2) );
	PACKET_GUARD_VOID( rkPacket.Read(iPresentMent) );
	PACKET_GUARD_VOID( rkPacket.Read(iPublicIDState) );

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	SP2Packet kPacket( MSTPK_ADMIN );
	PACKET_GUARD_VOID( kPacket.Write(ADMINCOMMAND_ITEMINSERT) );
	PACKET_GUARD_VOID( kPacket.Write(szPublicID) );
	PACKET_GUARD_VOID( kPacket.Write(iPresentType) );
	PACKET_GUARD_VOID( kPacket.Write(iPresentValue1) );
	PACKET_GUARD_VOID( kPacket.Write(iPresentValue2) );
	PACKET_GUARD_VOID( kPacket.Write(iPresentMent) );
	PACKET_GUARD_VOID( kPacket.Write(iPublicIDState) );

	ServerNode* pServerNode = g_ServerNodeManager.GetServerNode();
	if(pServerNode)
	{
		pServerNode->SendMessage( kPacket );

		OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_MTOOL_ITEMINSERT : success [%s] [%s] [%d] [%d] [%d]", GetID().c_str(), szPublicID.c_str(), iPresentType, iPresentValue1, iPresentValue2);
	}
	else
	{
		OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_MTOOL_ITEMINSERT : failed [%s] [%s] [%d] [%d] [%d]", GetID().c_str(), szPublicID.c_str(), iPresentType, iPresentValue1, iPresentValue2);
	}
}

void MgrToolNode::OnAdminMToolEventInsert( const int iType, SP2Packet& rkPacket )
{
	//// 이벤트갯수 이벤트정보...
	ioHashString szUserID, szPassword;
	int iValueCount = 0, iValues[64] = {};
	rkPacket >> szUserID >> szPassword;
	rkPacket >> iValueCount;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	for(int i = 0 ; i < iValueCount ; i++)
	{
		rkPacket >> iValues[i];
		OperatorLOG.PrintTimeAndLog(0, "values : %d", iValues[i]);
	}

	SP2Packet kPacket( MSTPK_ADMIN );
	kPacket << ADMINCOMMAND_EVENTINSERT ;
	kPacket << iValueCount;
	for(int i = 0 ; i < iValueCount ; i++)
	{
		kPacket << iValues[i];
	}
	g_ServerNodeManager.SendMessageAllNode( kPacket );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_MTOOL_EVENTINSERT : [%s] count(%d)", szUserID.c_str(), iValueCount);
}

void MgrToolNode::OnAdminMToolPracticeKickUserDelete(SP2Packet& rkPacket )
{
	ioHashString szUserID, szPassword;
	ioHashString szPublicID;
	rkPacket >> szUserID >> szPassword;
	rkPacket >> szPublicID;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_MTOOL_PracticeKickUserDelete %s", szPublicID.c_str());
	g_DBClient.OnPractice_BlockUserList(szPublicID);
}



void MgrToolNode::OnMToolReciveToolDataSendGameServer( SP2Packet& rkPacket )
{
	ioHashString szUserID, szPassword;
	ioHashString szIP, szGUID;
	int port = 0;
	DWORD protocol = 0;
	rkPacket >> szUserID >> szPassword;
	rkPacket >> szIP >> port >> protocol;
	BOOL bRelayServerState = FALSE;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	szGUID = GetGUID();

	switch( protocol )
	{
	case CTPKM_GAMESERVER_QUICK_EXIT:				protocol = MSTPK_GAMESERVER_QUICK_EXIT;					break;
	case CTPKM_GAMESERVER_SAFETY_EXIT:				protocol = MSTPK_GAMESERVER_SAFETY_EXIT;				break;
	case CTPKM_GAMESERVER_CHANGE_RELAYSVR_STATE:  
		{
			protocol = MSTPK_GAMESERVER_CHANGE_RELAYSERVER_STATE;
			
			rkPacket >> bRelayServerState;	
		}
		break;
	}

	SP2Packet kPacket( protocol );
	kPacket << szGUID;
	if(protocol == MSTPK_GAMESERVER_CHANGE_RELAYSERVER_STATE)
	{
		kPacket << bRelayServerState;
	}


	g_ServerNodeManager.SendMessageIPnPort( szIP, port, kPacket );

	OperatorLOG.PrintTimeAndLog(0, "ADMINCOMMAND_RELAY : [%s] [%lu]", szUserID.c_str(), protocol);
}

void MgrToolNode::OnMToolMainServerExit( SP2Packet &rkPacket )
{
	ioHashString szUserID, szPassword;
	rkPacket >> szUserID >> szPassword;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	OperatorLOG.PrintTimeAndLog(0, "CMD_MAINEXIT : [%s(%s)]", GetID().c_str(), m_szMgrToolIP.c_str());

	g_MainProc.Shutdown( SHUTDOWN_QUICK );
}

void MgrToolNode::OnMToolGameServerExit( SP2Packet &rkPacket )
{
	ioHashString szUserID, szPassword;
	DWORD dwType = 0;
	rkPacket >> szUserID >> szPassword;
	rkPacket >> dwType;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	OperatorLOG.PrintTimeAndLog(0, "CMD_GAMEEXIT : [%s(%s)] [%d]", GetID().c_str(), m_szMgrToolIP.c_str(), dwType);

	g_MainProc.AllServerExit( dwType, false );
}

void MgrToolNode::OnMToolReloadGameServerINI( SP2Packet &rkPacket )
{
	ioHashString szUserID, szPassword;
	rkPacket >> szUserID >> szPassword;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	OperatorLOG.PrintTimeAndLog(0, "CMD_RELOADGAMEINI : [%s(%s)]", GetID().c_str(), m_szMgrToolIP.c_str());

	ioINILoader kLoader;
	if( kLoader.ReloadFile( "config/game_server_reload_list.ini" ) )
	{
		LOG.PrintTimeAndLog(0, "%s - INI file reload sucess", __FUNCTION__ );
	}
	else
	{
		LOG.PrintTimeAndLog(0, "%s - INI file reload failed!!", __FUNCTION__ );
	}

	kLoader.SetTitle( "common" );

	DWORDVec vINI;
	vINI.clear();

	int iMaxINI = kLoader.LoadInt( "max_ini", 0 );
	SP2Packet kPacket( MSTPK_GAME_SERVER_RELOAD_INI );
	kPacket << iMaxINI;
	for(int i = 0;i < iMaxINI;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "ini_type%d", i + 1 );
		int iKey = kLoader.LoadInt( szKey, -1 );
		kPacket << iKey;
		vINI.push_back( iKey );
	}
	g_ServerNodeManager.SendMessageAllNode( kPacket );	

	// 메인서버의 ini reload
	int icount = (int)vINI.size();
	for(int i=0 ; i<icount ; ++i)
	{
		switch( vINI[i] )
		{
		case RELOAD_RANKING:
			{
				g_MatchNodeManager.LoadINI();
			}
			break;
		}
	}
}

void MgrToolNode::OnMToolReloadCloseINI( SP2Packet &rkPacket )
{
	ioHashString szUserID, szPassword;
	rkPacket >> szUserID >> szPassword;

	if(ADMIN_LEVEL_NONE == AdminIDCheck(szUserID, szPassword))
	{
		return ;
	}

	OperatorLOG.PrintTimeAndLog(0, "CMD_RELOADCLOSEINI : [%s(%s)]", GetID().c_str(), m_szMgrToolIP.c_str());

	g_ServerCloseMgr.LoadCloseServerInfo();
}
