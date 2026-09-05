// LogDBClient.cpp: implementation of the LogDBClient class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "LogDBClient.h"

#include "../MainProcess.h"
#include "../ioProcessChecker.h"
#include "../QueryData/QueryResultData.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../WemadeLog/WemadeLog.h"
#include <strsafe.h>

extern CLog TradeLOG;
extern bool tokenize(const std::string& str, const std::string& delimiters, std::vector<std::string>& tokens);

LogDBClient *LogDBClient::sg_Instance = NULL;
int LogDBClient::m_iUserCntSendDelay = 10;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogDBClient::LogDBClient( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	m_dwCurrentTime	= 0;
	m_iDBAgentPort	= 0;
	m_DBAgentIP		= "";
}

LogDBClient::~LogDBClient()
{

}

LogDBClient &LogDBClient::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_main.ini" );
		kLoader.SetTitle( "GameLogDB Buffer" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		sg_Instance = new LogDBClient( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );

		int iUserCntSendDelay = kLoader.LoadInt( "CCU_sync_time", 5 );
		sg_Instance->SetUserCntSendDelay(iUserCntSendDelay);

		LOG.PrintTimeAndLog( 0, "CCU sync Time: %d", iUserCntSendDelay );
	}
	return *sg_Instance;
}

void LogDBClient::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}

bool LogDBClient::ConnectTo()
{
	ioINILoader kLoader( "../global_define.ini" );
	kLoader.SetTitle( "DBA_log" );

	char szValue[MAX_PATH];
	kLoader.LoadString( "1", "", szValue, MAX_PATH );
	if( strcmp( szValue, "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "LogDBClient::ConnectTo IP is empty" );
		return false;
	}

	std::string values = szValue;
	std::string delimiter = ":";
	std::vector<std::string> tokens;
	tokenize(values, delimiter, tokens);
	if(tokens.size() != 2) return false;

	m_DBAgentIP		= tokens[0].c_str();
	m_iDBAgentPort	= atoi(tokens[1].c_str());

	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( 0, "LogDBClient::ConnectTo socket %d[%s:%d]", GetLastError(), m_DBAgentIP.c_str(), m_iDBAgentPort );
		return false;
	}

	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( m_DBAgentIP.c_str() );
	serv_addr.sin_port			= htons( m_iDBAgentPort );
	if( ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) ) != 0 )
	{
		LOG.PrintTimeAndLog( 0, "LogDBClient::ConnectTo connect %d[%s:%d]", GetLastError(), m_DBAgentIP.c_str(), m_iDBAgentPort );
		return false;
	}

	m_DBAgentIP		= m_DBAgentIP.c_str();
	m_iDBAgentPort	= m_iDBAgentPort;

	g_iocp.AddHandleToIOCP( (HANDLE)socket, (DWORD)this );
	CConnectNode::SetSocket( socket );

	OnCreate();	
	AfterCreate();
	LOG.PrintTimeAndLog( 0, "OnConnect (IP:%s PORT:%d RESULT:%d)", m_DBAgentIP.c_str(), m_iDBAgentPort, 0 );
	return true;
}

void LogDBClient::OnCreate()
{
	CConnectNode::OnCreate();
	
	m_dwCurrentTime = TIMEGETTIME();
}

void LogDBClient::OnDestroy()
{
	CConnectNode::OnDestroy();
	LOG.PrintTimeAndLog( 0, "LogDBClient : Disconnect" );
}

bool LogDBClient::SendMessage( CPacket &rkPacket )
{
	g_ProcessChecker.LogDBServerSendMessage( rkPacket.GetBufferSize() );
	if( !CConnectNode::SendMessage( rkPacket ) )
	{
		LOG.PrintTimeAndLog(0, "LogDBClient send failed : packet(%d), error(%lu)", rkPacket.GetPacketID(), GetLastError());
		return false;
	}
	return true;
}

bool LogDBClient::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int LogDBClient::GetConnectType()
{
	return CONNECT_TYPE_LOGDB_SERVER;
}

void LogDBClient::SessionClose( BOOL safely )
{
	if(IsActive())
	{
		CPacket packet(ITPK_CLOSE_SESSION);
		ReceivePacket( packet );
	}
}

void LogDBClient::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void LogDBClient::OnClose( SP2Packet &packet )
{
	OnDestroy();
}

void LogDBClient::ProcessTime()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if(TIMEGETTIME() - m_dwCurrentTime < 60000) return; // 1분 확인
	
	if( !IsActive() )
	{
		ConnectTo();
	}
	m_dwCurrentTime = TIMEGETTIME();
	OnPing();

	static int i1minsCnt = 0;
	i1minsCnt++;

	int iDelay = sg_Instance->GetUserCntSendDelay();
	LOG.PrintTimeAndLog( 0, "LogDBClient ccu %d", iDelay );

	if( i1minsCnt > iDelay ) // 5분
	{
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_WEMADEBUY ), CNT_WEMADEBUY );
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_MGAME ), CNT_MGAME );
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_DAUM ), CNT_DAUM );
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_BUDDY ), CNT_BUDDY );
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_NAVER ), CNT_NAVER );
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_TOONILAND ), CNT_TOONILAND );
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_NEXON ), CNT_NEXON );
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_HANGAME ), CNT_HANGAME );
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_VALOFE ), CNT_VALOFE );
#ifdef SRC_NA //북미인 경우만 스팀 ccu DB 에 저장함
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_STEAM ), CNT_STEAM );
#endif
#ifdef SRC_TW
		OnInsertConnectUser( g_ServerNodeManager.GetMaxUserCountByChannelingType( CNT_HAPPYTUK ), CNT_HAPPYTUK );
#endif //

		i1minsCnt = 0;
	}
}

void LogDBClient::LogDBNode_SendBufferFlush()
{
	if( ! IsActive() )
		return;
	if( GetSocket() == INVALID_SOCKET )
		return;

	CSendIO::FlushSendBuffer();
}

void LogDBClient::PacketParsing( CPacket &packet )
{
	switch(packet.GetPacketID())
	{
	case ITPK_CLOSE_SESSION:
		OnClose( (SP2Packet&)packet );
		break;
	}	
}

ValueType LogDBClient::GetValueType(VariableType nType,int len)
{
	ValueType vt;
	vt.type = nType;
	vt.size = len;
	return vt;
}

void LogDBClient::OnPing()
{
	const int queryId = 1; // 실제로는 존재하지 않는 쿼리

	DWORD dwCurrent = TIMEGETTIME();

	Reset();

	m_Query.SetReturnData( &dwCurrent, sizeof(DWORD) );

	m_Query.SetData( 
		0, 
		_RESULT_CHECK, 
		DBAGENT_LOG_PINGPONG, 
		_UPDATEDB,
		queryId, 
		m_FT, 
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}
//////////////////////////////////////////////////////////////////////////
// SEND
//////////////////////////////////////////////////////////////////////////
void LogDBClient::Reset()
{
	m_FT.Reset();
	m_VT.clear();
	m_Query.Clear();
}

void LogDBClient::OnInsertConnectUser( int iConnectUser, ChannelingType eChannelingType )
{
	//StringCbPrintf(str_query, sizeof(str_query), "exec game_log_concurrent 0, '%s', %d, 'Main', %d, %d" , g_App.GetPublicIP().c_str(), g_App.GetPort(), iConnectUser, (int)eChannelingType );

	char serverName[16] = "Main";

	const int queryId = 165;

	Reset();
	
	m_FT.Begin( WE_LOG_CONCURRENT );
	m_FT.Append( (uint64)0 );
	m_FT.Append( g_MainProc.GetPublicIP().c_str(), 32, TRUE );
	m_FT.Append( (uint32)g_MainProc.GetPort() );
	m_FT.Append( serverName, strlen(serverName), TRUE );
	m_FT.Append( (uint32)iConnectUser );
	m_FT.Append( (uint16)eChannelingType );

	m_Query.SetData( 
		0, 
		_RESULT_NAUGHT, 
		DBAGENT_CONNECT_USER_SET, 
		_INSERTDB, 
		queryId,
		m_FT,
		m_VT );

	SP2Packet kPacket(DTPK_QUERY);
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void LogDBClient::OnInsertTrade( DWORD dwUserIndex,
								const ioHashString &rszPublicID,
								DWORD dwTradeIndex,
								DWORD dwMagicCode,
								DWORD dwValue,
								__int64 iItemPrice,
								TradeSysType eType,
								const char *szPublicIP,
								const char *szNote )
{
	if( rszPublicID.IsEmpty() )
	{
		TradeLOG.PrintTimeAndLog( 0, "%s PublicID is empty. (%d:%d)", __FUNCTION__ , dwUserIndex, dwTradeIndex );
		return;
	}

	if( rszPublicID == NULL )
	{
		TradeLOG.PrintTimeAndLog( 0, "%s IP is NULL. (%d:%d)", __FUNCTION__ , dwUserIndex, dwTradeIndex );
		return;
	}

	const int NOTE_SISZE = 100;
	char szShortNote[NOTE_SISZE]="";
	StringCbCopyN( szShortNote, sizeof( szShortNote ), szNote, NOTE_SISZE-1 );

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_trade_add %d,'%s',%d,%d,%I64d,%d,'%s','%s'",
	//	dwUserIndex, rszPublicID.c_str(), dwMagicCode, dwValue, iItemPrice, eType, szPublicIP, szShortNote );

	const int queryId = 1011;

	Reset();

	m_FT.Begin( WE_LOG_DATA_TRADE );
	m_FT.Append( dwUserIndex );
	m_FT.Append( rszPublicID.c_str(), 32, TRUE );
	m_FT.Append( dwMagicCode );
	m_FT.Append( dwValue );
	m_FT.Append( (uint64)iItemPrice );
	m_FT.Append( (uint32)eType );
	m_FT.Append( szPublicIP, 32, TRUE );
	m_FT.Append( szShortNote, 100, TRUE );

	m_Query.SetData( 
		0, 
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB,
		queryId,
		m_FT,
		m_VT );

	SP2Packet kPacket(DTPK_QUERY);
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}


void LogDBClient::OnInsertTournamentRewardSet( DWORD dwTourIndex, const ioHashString &rkOwnerID, DWORD dwOwnerIndex, BYTE TourPos, DWORD dwReward1, DWORD dwReward2, DWORD dwReward3, DWORD dwReward4 )
{
	//exec log_data_league_present_add 유저인덱스, '닉네임', 리그인덱스, 라운드, 선물코드1, 선물코드2, 선물코드3, 선물코드4

	const int queryId = 1021;

	Reset();

	m_FT.Begin( WE_LOG_DATA_LEAGUE_PRESENT );
	m_FT.Append( dwOwnerIndex );
	m_FT.Append( rkOwnerID.c_str(), ID_NUM_PLUS_ONE, TRUE );
	m_FT.Append( dwTourIndex );
	m_FT.Append( (uint8)TourPos );
	m_FT.Append( dwReward1 );
	m_FT.Append( dwReward2 );
	m_FT.Append( dwReward3 );
	m_FT.Append( dwReward4 );

	m_Query.SetData( 0, _RESULT_DESTROY, LOGDBAGENT_SET, _INSERTDB, queryId, m_FT, m_VT );

	SP2Packet kPacket(DTPK_QUERY);
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}