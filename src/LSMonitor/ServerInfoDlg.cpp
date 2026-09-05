// ServerInfoDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "LSMonitorDlg.h"
#include ".\serverinfodlg.h"


// CServerInfoDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerInfoDlg, CDialog)
CServerInfoDlg::CServerInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerInfoDlg::IDD, pParent)
	, m_szEdit(_T(""))
{
	m_type = 0;
}

CServerInfoDlg::~CServerInfoDlg()
{
}

void CServerInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_szEdit);
}


BEGIN_MESSAGE_MAP(CServerInfoDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CServerInfoDlg::OnBnClickedButtonRefresh)
END_MESSAGE_MAP()


// CServerInfoDlg 메시지 처리기입니다.

void CServerInfoDlg::PrintMainserverTextOut( SP2Packet& rPacket )
{
	m_type = eServerType_MainServer;
	SetDlgItemText( IDC_STATIC_INFO, "MainServer Info :" );

	// Declare)
	char temp[ 1024 ] = { 0, };
	m_szEdit.Empty();

	// Struct
	MAINSERVERINFO	info;
	rPacket >> info;

	//GLOBAL TIME
	sprintf( temp, "GLOBAL TIME : %d", info.dwGlobalTime );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );
		
	//Network Info
	sprintf( temp, "SERVER IP: %s:%s PORT: %d", info.szPublicIP, info.szPrivateIP, info.iPort );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//Thread Info
	sprintf( temp, "THREAD COUNT: %d", info.ThreadCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );
	
	//Connect Client Info
	sprintf( temp, "JOIN SERVER: %d        ", info.JoinServerCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//Remainder MemPool Info
	sprintf( temp, "REMAINDER MEMPOOL: %d ServerMemCount", info.RemainderMemPoolCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//RECV QUEUE
	sprintf( temp, "RECV PACKET: %d(64), %d(256), %d(1024), %d(big) QUEUE", info.RecvQueuePacketCount[0], info.RecvQueuePacketCount[1],
		info.RecvQueuePacketCount[2], info.RecvQueuePacketCount[3] );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//Remainder MemPool Info
	sprintf( temp, "REMAINDER MEMPOOL: %d(64), %d(256), %d(1024), %d(big) PacketMemCount", info.RecvQueueRemainderCount[0], info.RecvQueueRemainderCount[1],
		info.RecvQueueRemainderCount[2], info.RecvQueueRemainderCount[3] );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//DB AGENT SERVER INFO
	sprintf( temp, "DB AGENT IP: %s PORT: %d", info.szDBAgentIP, info.DBAgentPort );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//GUILD INFO
	sprintf( temp, "MAX GUILD: %d개 - MAX UPDATE GUILD: %d개", info.MaxGuildCount, info.MaxUpdateGuild );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//CAMP INFO
	sprintf( temp, "진영전 : %s", info.szCampStringHelp );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//Trade Info
	sprintf( temp, "MAX TradeItem: %d개", info.MaxTradeItemCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//Event Shop Info
	sprintf( temp, "Event Shop : %s(%d Save Reserve)", info.szEventShopState, info.EventGoodsSaveDataCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	//MANAGER TOOL
	sprintf( temp, "MAX Tool CONNECT : %d", info.MaxToolConnectCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	// Client Version
	sprintf( temp, "CLIENT VERSION: %d:%d ", (int)info.bUseClientVersion, info.iClientVersion );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	// Main Server Version
	sprintf( temp, "MAIN SERVER VERSION: %s | %s", info.szMainServerVersion, info.szMainServerName );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	// Main Server Log
	sprintf( temp, "Log Buffer Using : %d ( max : %d ) Remain Count : %d / Log Drop Count : %d", info.usingLogCount, info.maxUsingLogCount, info.remainLogCount, info.dropLogCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	UpdateData( FALSE );
}

void CServerInfoDlg::PrintGameServerTextOut( SP2Packet& rPacket )
{
	m_type = eServerType_GameServer;
	SetDlgItemText( IDC_STATIC_INFO, "GameServer Info :" );

	// Declare)
	char temp[ 1024 ] = { 0, };
	m_szEdit.Empty();

	// Struct
	GAMESERVERINFO	info;
	rPacket >> info;

	sprintf( temp, "GLOBAL TIME : %d", info.dwGlobalTime );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "SERVERIP:%s CSPORT:%d SSPORT:%d MSPORT:%d", info.szPublicIP, info.csPort, info.SSPort, info.MSPort );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "THREAD COUNT: %d", info.ThreadCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "JOIN CLIENT: %d(%d) Client", info.NodeSize, info.CopyNodeSize );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "REMAINDER MEMPOOL: %d UserMemCount", info.RemainderNode );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "ROOM CREATE: %d(%d:%d) - %d(%d:%d) ROOM", info.RoomNodeSize, info.PlazaNodeSize, info.HeapQuartersNodeSize
		, info.CopyRoomNodeSize, info.CopyPlazaNodeSize, info.CopyHeapQuartersNodeSize );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "REMAINDER MEMPOOL: %d RoomMemCount", info.RommRemainderNode );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "BATTLEROOM CREATE: %d(%d) ROOM LimitMinRoom(%d)", info.BattleRoomNodeSize, info.BattleRoomCopyNodeSize, info.PartyLevelCheckMinRoom );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "REMAINDER MEMPOOL: %d BattleRoomMemCount", info.BattleRoomRemainderNode );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "LADDERTEAM CREATE: %d(%d) - IsBattle(%d)", info.LadderTeamNodeSize, info.LadderTeamCopyNodeSize, info.LadderTeamCampBattlePlay );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "REMAINDER MEMPOOL: %d LadderTeamMemCount", info.LadderTeamRemainderNode );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "RECV PACKET: %d(64) %d(256) %d(1024) %d(big) %d(udp) QUEUE", info.RecvQueueNodeSize[ 0 ], info.RecvQueueNodeSize[ 1 ],
		info.RecvQueueNodeSize[ 2 ], info.RecvQueueNodeSize[ 3 ], info.BroadCastUDPnRelayNodeSize );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "REMAINDER MEMPOOL: %d(64) %d(256) %d(1024) %d(big) %d(udp) PacketMemCount", info.RecvQueueRemainderNodeSize[ 0 ], info.RecvQueueRemainderNodeSize[ 1 ], info.RecvQueueRemainderNodeSize[ 2 ],
		info.RecvQueueRemainderNodeSize[ 3 ], info.RecvQueueRemainderNodeSize[ 4 ], info.BroadCastUDPnRelayNodeSize );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "DB AGENT %d Connection", info.DBClientNodeSize );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "GAME SERVER VERSION: %s | %s", info.szSTRFILEVER, info.szSTRINTERNALNAME );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "GAME SERVER ID: %s", info.szGameServerID );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "GAME SERVER NAME: %s", info.szGameServerName );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "LOGDB AGENT IP: %s PORT: %d", info.szLogDBIP, info.LogDBPort );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "HACKCHECK : Min:%d, Max:%d, Less:%d, Over:%d, LessOver:%d, Total:%d", info.HackCheckMin
		, info.HackCheckMax, info.HackCheckLess, info.HackCheckOver, info.HackCheckLessOver, info.HackCheckTotal );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "CLIENT VERSION: %d:%d", info.IsClientVersion, info.GetClientVersion );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "Channel CREATE: %d(%d) : %d Copy Channel", info.ChannelNodeSize, info.ChannelRemainderSize, info.ChannelCopyNodeSize );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "MEMO %d", info.MemoNodeSize );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "SERVER CONNECT %d : [%d](%d)", info.GetServerIndex, info.ServerNodeSize, info.ServerRemainderNodeSize );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	if( info.IsMainServerActive )
	{
		sprintf( temp, "MAIN SERVER    CONNECT %s:%d", info.MainServerIP, info.MainServerPort );
	}
	else
	{
		sprintf( temp, "MAIN SERVER DISCONNECT %s:%d", info.MainServerIP, info.MainServerPort );
	}
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "UDP TRANSFER COUNT : %I64d(%I64d:%I64d)", info.UDPTransferCount, info.UDPTransferTCPCount, info.UDPTransferTCPSendCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	if( info.IsBillingRelayServerActive )
	{
		sprintf( temp, "BILLING RELAY SERVER    CONNECT %s:%d", info.BillingIP, info.BillingPort );
	}
	else
	{
		sprintf( temp, "BILLING RELAY SERVER DISCONNECT %s:%d", info.BillingIP, info.BillingPort );
	}
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	// XTrap
	sprintf( temp, "XTRAP CS3 VERSION %s", info.XtrapVersion );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );
	
	if( info.LicenseDate )
	{
		sprintf( temp, "EXPIRATION DATES : %d", info.LicenseDate );
		m_szEdit.Append( temp );
		m_szEdit.Append( "\r\n" );
	}

	// sendBuffer
	sprintf( temp, "SendBuffer Using : %d ( max:%d ), Remain Count : %d", info.sendBufferUsingCnt, info.sendBufferMaxCnt, info.sendBufferRemainCnt );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	// log
	sprintf( temp, "Log Buffer Using : %d ( max : %d ) Remain Count : %d / Log Drop Count : %d", info.usingLogCount, info.maxUsingLogCount, info.remainLogCount, info.dropLogCount );
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	if( info.m_bReserveLogout )
	{
		sprintf( temp, "EXIT AFTER %03d S", info.remainSecond );
		m_szEdit.Append( temp );
		m_szEdit.Append( "\r\n" );
	}

	UpdateData( FALSE );
}

void CServerInfoDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//delete this;
	//CDialog::PostNcDestroy();
}


void CServerInfoDlg::OnBnClickedButtonRefresh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( AfxGetApp() )
 	{
 		CLSMonitorDlg *pWnd = (CLSMonitorDlg*)AfxGetApp()->GetMainWnd();
 		if(pWnd)
		{
			switch( m_type )
			{
			case eServerType_MainServer:
				pWnd->OnMainServerInfoDrawReq();
				break;

			case eServerType_GameServer:
				pWnd->OnGameserverInfodraw();
				break;
			case eServerType_LoginPanel:
				pWnd->OnLoginserverLoginserverinFill();
				break;
			}
		}
 	}
}

void CServerInfoDlg::PrintLoginServerTextOut( SP2Packet& rPacket )
{
	m_type = eServerType_LoginPanel;
	LoginServerInfo_ stdata;
	rPacket >> stdata;

	char temp[ 1024 ] = { 0, };
	m_szEdit.Empty();
	SetDlgItemText( IDC_STATIC_INFO, "LoginServer Info :" );
 

	sprintf( temp, "LoginServer(%s:%d)",stdata.ipaddr,stdata.port);
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "AccetCount:%d(%d)",stdata.acceptcountpersec,stdata.acceptcount);
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "CloseCount:%d",stdata.closecount);
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "ClientPoolCount:%d",stdata.clientpoolcount);
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "OPPoolCount:%d",stdata.oppoolcount);
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "PacketQueCount:%d",stdata.packetquecount);
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "serverconnectpool:%d",stdata.serverconnectorpoolcount);
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "ServerconnectCount:%d",stdata.serverconnectcount);
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );

	sprintf( temp, "userinfoCount:%d",stdata.userinfocount);
	m_szEdit.Append( temp );
	m_szEdit.Append( "\r\n" );
 
	int max;
	rPacket >> max;
	int usercount;
	int sendcount;
	int serverid;
	int serverstate;
	for(int i=0; i<max; ++i)
	{
		rPacket >> serverid;
		rPacket >> usercount;
		rPacket >> sendcount;
		rPacket >> serverstate;
		sprintf( temp, "Server(%d)::usercount:%d::sendcount::%d state::%x",serverid,usercount,sendcount,serverstate);
		m_szEdit.Append( temp );
		m_szEdit.Append( "\r\n" );
	}


	m_szEdit.Append( "\r\n" );
	m_szEdit.Append( "\r\n" );
	UpdateData( FALSE );
}


BOOL CServerInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if(GetKeyState(VK_F5) < 0)
			{
				OnBnClickedButtonRefresh();
			}
		}
		break;

	}

 

	return CDialog::PreTranslateMessage(pMsg);
}
