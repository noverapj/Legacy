// DBClient.cpp: implementation of the DBClient class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "DBClient.h"

#include "../MainProcess.h"
#include "../ioProcessChecker.h"
#include "../QueryData/QueryResultData.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../EtcHelpFunc.h"


#include <strsafe.h>


DBClient *DBClient::sg_Instance = NULL;

extern bool tokenize(const std::string& str, const std::string& delimiters, std::vector<std::string>& tokens);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DBClient::DBClient( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	m_dwCurrentTime   = 0;
	m_iClassPriceTime = 0;
	m_iDBAgentThreadID= 0;
	m_iDBAgentPort    = 0;
	m_bOnceRun		  = false;
}

DBClient::~DBClient()
{
}

DBClient &DBClient::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_main.ini" );
		kLoader.SetTitle( "GameDB Buffer" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", 16384 );
		int iRecvBufferSize = kLoader.LoadInt( "RecvBufferSize", 16384 );
		sg_Instance = new DBClient( INVALID_SOCKET, iSendBufferSize, iRecvBufferSize );
	}
	return *sg_Instance;
}

void DBClient::ReleaseInstance()
{		
	SAFEDELETE(sg_Instance);
}

bool DBClient::ConnectTo()
{
	ioINILoader kLoader( "../global_define.ini" );
	kLoader.SetTitle( "DBA_game" );

	char szValue[MAX_PATH];
	kLoader.LoadString( "1", "", szValue, MAX_PATH );
	if( strcmp( szValue, "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "DBClient::ConnectTo IP is empty" );
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
		LOG.PrintTimeAndLog( 0, "DBClient::ConnectTo socket %d[%s:%d]", GetLastError(), m_DBAgentIP.c_str(), m_iDBAgentPort );
		return false;
	}

	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( m_DBAgentIP.c_str() );
	serv_addr.sin_port			= htons( m_iDBAgentPort );
	if( ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) ) != 0 )
	{
		LOG.PrintTimeAndLog( 0, "DBClient::ConnectTo connect %d[%s:%d]", GetLastError(), m_DBAgentIP.c_str(), m_iDBAgentPort );
		return false;
	}

	g_iocp.AddHandleToIOCP( (HANDLE)socket, (DWORD)this );
	CConnectNode::SetSocket( socket );

	kLoader.SetTitle( "ClassPrice" );
	m_iClassPriceTime = kLoader.LoadInt( "LoadTime", 10 );

	OnCreate();	
	AfterCreate();
	LOG.PrintTimeAndLog( 0, "OnConnect (IP:%s PORT:%d RESULT:%d)", m_DBAgentIP.c_str(), m_iDBAgentPort, 0 );
	return true;
}

void DBClient::OnCreate()
{
	CConnectNode::OnCreate();

	m_dwCurrentTime = TIMEGETTIME();
}

void DBClient::OnDestroy()
{
	CConnectNode::OnDestroy();
	LOG.PrintTimeAndLog( 0, "DBClient : Disconnect" );
}

ValueType DBClient::GetValueType(VariableType nType,int len)
{
	ValueType vt;
	vt.type = nType;
	vt.size = len;
	return vt;
}

bool DBClient::SendMessage( CPacket &rkPacket )
{
	g_ProcessChecker.DBServerSendMessage( rkPacket.GetBufferSize() );
	if( !CConnectNode::SendMessage( rkPacket, TRUE ) )
	{
		LOG.PrintTimeAndLog(0, "DBClient send failed : packet(%d), error(%lu)", rkPacket.GetPacketID(), GetLastError());
		return false;
	}
	return true;
}

bool DBClient::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int DBClient::GetConnectType()
{
	return CONNECT_TYPE_GAMEDB_SERVER;
}

void DBClient::SessionClose( BOOL safely )
{
	if(IsActive())
	{
		CPacket packet(ITPK_CLOSE_SESSION);
		ReceivePacket( packet );
	}
}

void DBClient::ReceivePacket( CPacket &packet )
{
	switch( packet.GetPacketID() )
	{
	case DTPK_QUERY:
		g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_QUERY );
		break;
	default:
		break;
	}
}

void DBClient::PacketParsing( CPacket &packet )
{
	switch(packet.GetPacketID())
	{
	case ITPK_CLOSE_SESSION:
		OnClose( (SP2Packet&)packet );
		break;
	}	
}

void DBClient::OnPing()
{
	const int iQueryID = 1; // 실제로는 존재하지 않는 쿼리

	DWORD dwCurrent = TIMEGETTIME();

	Reset();

	m_Query.SetReturnData( &dwCurrent, sizeof(DWORD) );
	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GAME_PINGPONG, 
		_UPDATEDB,
		iQueryID, 
		m_FT, 
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnClassPriceInfo()
{
	static int iCurMinute = 0;
	iCurMinute++;

	if( iCurMinute > m_iClassPriceTime )
	{
		iCurMinute = 0;
		OnSelectItemBuyCnt();
	}
}

void DBClient::OnTotalRegUserInfo()
{
	static int iCheckTime = 0;

	CTime curTime = CTime::GetCurrentTime();

	CTimeSpan minusTime( 0, 4, 30, 0 );
	CTime FatigueTime = curTime - minusTime; // 0:00 ~ 23:59가 아니라 4:30 ~ 4:29임.	
	DWORD dwCurTime	= ( FatigueTime.GetYear() * 10000 ) + ( FatigueTime.GetMonth() * 100 ) + FatigueTime.GetDay();
	if( dwCurTime != iCheckTime )
		iCheckTime = dwCurTime;	
	else 
		return;

	OnSelectTotalRegUser();
}

void DBClient::ProcessTime()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( !IsActive() )
	{
		if( g_MainProc.IsTestZone() )
			ConnectTo();
		else
			::PostQuitMessage( 0 );
	}

	if(TIMEGETTIME() - m_dwCurrentTime < 60000) return; // 1분 확인

	OnPing();
	OnClassPriceInfo();
	OnTotalRegUserInfo();
	OnDeleteOldMissionData();
	m_dwCurrentTime = TIMEGETTIME();
}

//////////////////////////////////////////////////////////////////////////
// SEND
//////////////////////////////////////////////////////////////////////////
void DBClient::Reset()
{
	m_FT.Reset();
	m_VT.clear();
	m_Query.Clear();
}

void DBClient::OnClose( SP2Packet &packet )
{
	OnDestroy();
}

void DBClient::OnInsertGameServerInfo( const int64 serverId, const ioHashString &szIP, const int iSSPort, const ioHashString &szName, const int iCSPort )
{
	if(0 == serverId)
	{
		LOG.PrintTimeAndLog(0, "Error - OnInsertGameServerInfo - GameServerID is empty");
		return;
	}

	//char str_query[MAX_PATH * 2] = "";
	//wsprintf(str_query,"exec game_server_add %s, '%s', %d, '%s', %d", szGameServerID.c_str(),  szIP.c_str(), iSSPort, szName.c_str(), iCSPort );

	const int iQueryID = 2000;

	Reset();

	m_FT.Write( serverId );
	m_FT.Write( (const uint8*)szIP.c_str(), 32, TRUE );
	m_FT.Write( (uint32)iSSPort );
	m_FT.Write( (const uint8*)szName.c_str(), 32, TRUE );
	m_FT.Write( (uint32)iCSPort );
	
	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_SERVER_INFO_SET, 
		_INSERTDB,
		iQueryID, 
		m_FT, 
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectItemBuyCnt()
{
	static const int iItemBuyCount = 150;
	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec game_price_class_get_data" );

	const int iQueryID = 2001;
	// GameServer iQueryID = 6; 과 같음

	Reset();
	m_VT.push_back(GetValueType(vLONG,sizeof(LONG)));         //용병 번호
	m_VT.push_back(GetValueType(vLONG,sizeof(LONG)));         //가격
	//for(int i = 0;i < iItemBuyCount;i++)
	//{
	//	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );         //가격
	//}

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_ITEM_BUYCNT_SET, 
		_SELECTDB,
		2001, 
		m_FT,
		m_VT );

	SP2Packet kPacket(DTPK_QUERY);
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectTotalRegUser( bool bServerDown )
{
	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec game_member_total_count" );

	const int iQueryID = 2002;

	Reset();

	m_Query.SetReturnData( &bServerDown, sizeof(bool) );
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );

	m_Query.SetData( 
		m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TOTAL_REG_USER_SET, 
		_SELECTDB,
		iQueryID, 
		m_FT, 
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectGuildInfoList( int iGuildIDX, int iSelectCount )
{
	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec game_guild_get_list %d, %d", iSelectCount, iGuildIDX );

	const int iQueryID = 2003;

	Reset();

	m_FT.Write( (uint32)iSelectCount );
	m_FT.Write( (uint32)iGuildIDX );

	// 기본 정보
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				 //길드 인덱스
	m_VT.push_back( GetValueType( vChar, GUILD_NAME_NUM_PLUS_ONE ) );    //길드 이름
	m_VT.push_back( GetValueType( vChar, GUILD_TITLE_NUMBER_PLUS_ONE ) );//길드 소개
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 마크
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 포인트
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 인원 제한
	m_VT.push_back( GetValueType( vTimeStamp,sizeof(DBTIMESTAMP)));		 //길드 생성일
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 현재 인원
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 금일 획득 포인트
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );               //길드 레벨

	// 길드 전적
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //승
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //패
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //킬
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) ); //데스

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_GUILD_INFO_GET, 
		_SELECTEX1DB,
		iQueryID, 
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnDeleteGuildEntryDelayMember( DWORD dwGuildIndex )
{
	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_guild_join_init %d", dwGuildIndex );

	const int iQueryID = 2004;

	Reset();

	m_FT.Write( dwGuildIndex );
	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_ENTRY_DELAY_MEMBER_DEL, 
		_DELETEDB, 
		iQueryID, 
		m_FT, 
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnDeleteGuild( DWORD dwGuildIndex )
{
	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_guild_delete %d", dwGuildIndex );

	const int iQueryID = 2005;

	Reset();

	m_FT.Write( dwGuildIndex ); // dwGuildIndex

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_DELETE_DEL, 
		_DELETEDB, 
		iQueryID, 
		m_FT, 
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnUpdateGuildRecord( DWORD dwGuildIndex, int iWinCount, int iLoseCount, int iKillCount, int iDeathCount )
{
	// '1'은 고정 타입 값이다.
	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_guild_battle_record_save %d, 1, %d, %d, %d, %d", 
	//				  dwGuildIndex, iWinCount, iLoseCount, iKillCount, iDeathCount );

	const int iQueryID = 2006;

	Reset();

	m_FT.Write( dwGuildIndex );
	m_FT.Write( (uint32)(1) );
	m_FT.Write( (uint32)iWinCount );
	m_FT.Write( (uint32)iLoseCount );
	m_FT.Write( (uint32)iKillCount );
	m_FT.Write( (uint32)iDeathCount );

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_RECORD_UPD, 
		_UPDATEDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnUpdateGuildRanking( DWORD dwGuildIndex, DWORD dwRank, DWORD dwGuildPoint, DWORD dwCurGuildPoint, DWORD dwGuildLevel, DWORD dwMaxEntry )
{
	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_guild_ranking_point_save %d, %d, %d, %d, %d, %d", 
	//				  dwGuildIndex, dwRank, dwGuildPoint, dwCurGuildPoint, dwGuildLevel, dwMaxEntry );

	const int iQueryID = 2007;

	Reset();

	m_FT.Write( dwGuildIndex );
	m_FT.Write( dwRank );
	m_FT.Write( dwGuildPoint );
	m_FT.Write( dwCurGuildPoint );
	m_FT.Write( dwGuildLevel );
	m_FT.Write( dwMaxEntry );

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_GUILD_RANKING_UPD, 
		_UPDATEDB,
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectCampData()
{
	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec game_region_get_point" );

	const int iQueryID = 2008;

	Reset();

	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );              // 블루 진영 포인트
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );              // 블루 진영 투데이 포인트
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );              // 블루 진영 보너스 포인트
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );              // 레드 진영 포인트
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );              // 레드 진영 투데이 포인트
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );              // 레드 진영 보너스 포인트

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CAMP_DATA_GET, 
		_SELECTDB,
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnUpdateCampData( int iBlueCampPoint, int iBlueCampTodayPoint, int iBlueCampBluePoint, int iRedCampPoint, int iRedCampTodayPoint, int iRedCampBluePoint )
{
	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_region_save %d, %d, %d, %d, %d, %d", 
	//				  iBlueCampPoint, iBlueCampTodayPoint, iBlueCampBluePoint, 
	//				  iRedCampPoint, iRedCampTodayPoint, iRedCampBluePoint );

	const int iQueryID = 2009;

	Reset();

	m_FT.Write( (uint32)iBlueCampPoint );
	m_FT.Write( (uint32)iBlueCampTodayPoint );
	m_FT.Write( (uint32)iBlueCampBluePoint );
	m_FT.Write( (uint32)iRedCampPoint );
	m_FT.Write( (uint32)iRedCampTodayPoint );
	m_FT.Write( (uint32)iRedCampBluePoint );

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CAMP_DATA_UPD, 
		_UPDATEDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectCampSpecialUserCount( int iCampType )
{
	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec game_region_player_count %d", iCampType );

	const int iQueryID = 2010;

	Reset();

	m_FT.Write( (uint32)iCampType );
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );				// N포인트 이상 획득한 유저 카운트
	m_Query.SetReturnData( &iCampType, sizeof(int) );

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_CAMP_SPECIAL_USER_COUNT_GET, 
		_SELECTDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnInitCampSeason()
{
	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_region_season_end_data_init" );

	const int iQueryID = 2011;

	Reset();

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CAMP_SEASON_UPD, 
		_UPDATEDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnEndCampSeasonProcess()
{
	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_region_season_end" );

	const int iQueryID = 2012;

	Reset();

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CAMP_SEASON_END_PORCESS_UPD, 
		_UPDATEDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnEndCampSeasonServerClose()
{
	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_region_season_end_server_close" );

	const int iQueryID = 2013;

	Reset();

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CAMP_SEASON_END_SERVER_CLOSE_UPD, 
		_UPDATEDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnEndCampSeasonFirstBackup()
{
	LOG.PrintTimeAndLog( 0, "DB OnEndCampSeasonFirstBackup Call" );

	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_region_backup_point" );

	const int iQueryID = 2014;

	Reset();

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CAMP_SEASON_END_FIRST_BACKUP_SET, 
		_INSERTDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnEndCampSeasonLastBackup()
{
	LOG.PrintTimeAndLog( 0, "DB OnEndCampSeasonLastBackup Call" );

	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_region_backup_compen" );

	const int iQueryID = 2015;

	Reset();

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_CAMP_SEASON_END_LAST_BACKUP_SET, 
		_INSERTDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnHeroExpertMinusToZero()
{
	LOG.PrintTimeAndLog( 0, "DB OnHeroExpertMinusToZero Call" );

	//char szQuery[MAX_PATH] = "";
	//sprintf_s( szQuery, "exec game_game_sexp_deduct_init" );

	const int iQueryID = 2016;

	Reset();

	m_Query.SetData( 
		m_iDBAgentThreadID, 
		_RESULT_DESTROY, 
		DBAGENT_HERO_EXPERT_MINUS_TO_ZERO_UPD, 
		_UPDATEDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

// 임시 : 거래소
void DBClient::OnSelectTradeItemInfo( int iIndex, int iSelectCount )
{
	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec game_trade_get_list %d, %d", iSelectCount, iIndex );

	const int iQueryID = 2017;

	Reset();

	m_FT.Write( (uint32)iSelectCount );
	m_FT.Write( (uint32)iIndex );

	// 기본 정보
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 거래품 인덱스
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 거래품 등록유저 인덱스
	m_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );			// 거래품 등록유저 닉네임

	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 거래품 타입
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 거래품 구분자
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 거래품 value
	m_VT.push_back( GetValueType( vINT64, sizeof(__int64) ) );			// 거래품 거래가격

	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 거래품 남성 치장
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 거래품 여성 치장

	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 거래품 등록기간
	m_VT.push_back(GetValueType(vTimeStamp,sizeof(DBTIMESTAMP)));		// 거래품 등록시간

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TRADE_INFO_GET, 
		_SELECTEX1DB,
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnTradeItemDelete( DWORD dwTradeIndex, DWORD dwServerIndex )
{
	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec game_trade_delete %d", dwTradeIndex );

	const int iQueryID = 2018;

	Reset();

	m_FT.Write( dwTradeIndex );

	m_Query.SetReturnData( &dwTradeIndex, sizeof(DWORD) );
	m_Query.SetReturnData( &dwServerIndex, sizeof(DWORD) );
	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_TRADE_DELETE, 
		_DELETEDB, 
		iQueryID,
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}


void DBClient::OnSelectTournamentData( DWORD dwIndex, int iSelectCount )
{
	// game_league_list %d, %d
	const int iQueryID = 2019;

	Reset();

	m_FT.Write( iSelectCount );
	m_FT.Write( dwIndex );

	// 기본 정보
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 토너먼트 인덱스
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 생성 유저 인덱스
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 시작일
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 종료일
	m_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );              // 타입
	m_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );				// 상태

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_DATA_GET, _SELECTEX1DB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectTournamentCustomData( DWORD dwIndex, DWORD dwUserIndex, DWORD dwServerIndex )
{
	// game_league_list %d, %d
	const int iQueryID = 2019;

	Reset();

	int iSelectCount = 1;
	m_FT.Write( iSelectCount );
	m_FT.Write( dwIndex + 1 );         // 쿼리가 idx < 으로 처리되므로 +1된 인덱스를 넣어주워 원하는 대회를 가지고 온다.

	// 기본 정보
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 토너먼트 인덱스
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 생성 유저 인덱스
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 시작일
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				// 종료일
	m_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );              // 타입
	m_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );				// 상태

	m_Query.SetReturnData( &dwIndex, sizeof( DWORD ) );
	m_Query.SetReturnData( &dwUserIndex, sizeof( DWORD ) );
	m_Query.SetReturnData( &dwServerIndex, sizeof( DWORD ) );
	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CUSTOM_DATA_GET, _SELECTEX1DB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnUpdateTournamentData( DWORD dwIndex, DWORD dwStartDate, DWORD dwEndDate, BYTE Type, BYTE State )
{
	// game_league_list_save %d, %d, %d, %d, %d
	const int iQueryID = 2020;

	Reset();

	m_FT.Write( dwIndex );
	m_FT.Write( dwStartDate );
	m_FT.Write( dwEndDate );
	m_FT.Write( Type );
	m_FT.Write( State );

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_DATA_UPD, _UPDATEDB, iQueryID, m_FT,	m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnDeleteTournamentTeam( DWORD dwTeamIndex )
{
	// game_league_team_delete INT
	const int iQueryID = 2024;

	Reset();

	m_FT.Write( dwTeamIndex );

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_TEAM_DEL, _DELETEDB, iQueryID, m_FT,	m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnDeleteTournamentAllTeam( DWORD dwTourIndex )
{
	// game_league_team_delete_all INT
	const int iQueryID = 2027;

	Reset();

	m_FT.Write( dwTourIndex );

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_DESTROY, DBAGNET_TOURNAMENT_ALL_DEL, _DELETEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnUpdateTournamentBackUP( DWORD dwTourIndex )
{
	// game_league_backup 리그인덱스
	const int iQueryID = 2029;

	Reset();

	m_FT.Write( dwTourIndex );

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_DESTROY, DBAGNET_TOURNAMENT_BACKUP, _UPDATEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectTournamentTeamList( DWORD dwTeamIndex, int iSelectCount )
{
	// game_league_team_list INT INT
	const int iQueryID = 2025;

	Reset();

	m_FT.Write( iSelectCount );
	m_FT.Write( dwTeamIndex );

	// - 팀인덱스, 리그인덱스, 팀이름, 팀장인덱스, 리그포지션, 팀맥스카운트, 응원포인트, 토너먼트위치, 진영포인트, 진영타입, 시작 포지션
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );	
	m_VT.push_back( GetValueType( vChar, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE ) );				
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				
	m_VT.push_back( GetValueType( vSHORT, sizeof(SHORT) ) );	
	m_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );      
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );		
	m_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );      
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );		
	m_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );				
	m_VT.push_back( GetValueType( vSHORT, sizeof(SHORT) ) );			

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_TEAM_LIST_GET, _SELECTEX1DB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}}

void DBClient::OnUpdateTournamentTeamPointSave( DWORD dwTeamIndex, SHORT Position, SHORT StartPosition, BYTE TourPos, int iLadderPoint )
{
	// game_league_team_point_save 팀인덱스, 진영포인트, 토너먼트위치, 리그포지션
	const int iQueryID = 2036;

	Reset();

	m_FT.Write( dwTeamIndex );
	m_FT.Write( iLadderPoint );
	m_FT.Write( (uint8)TourPos );
	m_FT.Write( (uint16)Position );
	m_FT.Write( (uint16)StartPosition );

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_TEAM_POINT_SAVE_UPD, _UPDATEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnInsertTournamentWinnerHistory( const ioHashString &rkTitle, DWORD dwStartDate, DWORD dwEndDate, DWORD dwTeamIndex, const ioHashString &rkTeamName, const ioHashString &rkCampName, int iCampPos )
{
	// game_league_winner_history INT INT INT STR INT
	const int iQueryID = 2038;

	Reset();

	m_FT.Write( (const uint8*)rkTitle.c_str(), 20, TRUE ); 
	m_FT.Write( dwStartDate );
	m_FT.Write( dwEndDate );
	m_FT.Write( dwTeamIndex );
	m_FT.Write( (const uint8*)rkTeamName.c_str(), 20, TRUE );
	m_FT.Write( (const uint8*)rkCampName.c_str(), 20, TRUE ); 
	m_FT.Write( iCampPos );

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_SERVER_INFO_SET, _INSERTDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnInsertTournamentRewardAdd( DWORD dwStartDate, DWORD dwTourIndex, short StartTeamCount, short sCheerTeamAdjust, short sCheerUserAdjust )
{
	LOG.PrintTimeAndLog( 0, "DB OnInsertTournamentRewardAdd Call" );

	// game_league_reward_add INT INT INT16 INT16 INT16
	const int iQueryID = 2039;

	Reset();

	m_FT.Write( dwStartDate );
	m_FT.Write( dwTourIndex );
	m_FT.Write( StartTeamCount );
	m_FT.Write( sCheerTeamAdjust );
	m_FT.Write( sCheerUserAdjust );
	
	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_REWARD_ADD, _INSERTDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectTournamentCustomInfo( DWORD dwTourIndex )
{
	LOG.PrintTimeAndLog( 0, "DB OnSelectTournamentCustomInfo Call %d", dwTourIndex );
	//2045=game_league_info_list INT
	const int iQueryID = 2045;
	
	Reset();

	m_FT.Write( dwTourIndex );

	// - 대회 정보 인덱스 : int / 리그명 : nvarchar(20) / 맥스라운드 : smallint / 배너1 : int	/ 배너2 : int / 모드타입 : int / 팀최대인원 : tinyint / 대회실행타입 : tinyint / 공지 : nvarchar(512) / 모집기간 / 대기기간
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );	
	m_VT.push_back( GetValueType( vChar, TOURNAMENT_NAME_NUM_PLUS_ONE ) );				
	m_VT.push_back( GetValueType( vSHORT, sizeof(SHORT) ) );	
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );		
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );		
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );		
	m_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );      
	m_VT.push_back( GetValueType( vChar, sizeof(BYTE) ) );      
	m_VT.push_back( GetValueType( vChar, TOURNAMENT_ANNOUNCE_NUM_PLUS_ONE ) );				
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );		
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );		

	m_Query.SetReturnData( &dwTourIndex, sizeof(int) );
	
	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CUSTOM_INFO_GET, _SELECTDB, iQueryID, m_FT,	m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectTournamentCustomRound( DWORD dwTourIndex, DWORD dwTourInfoIndex )
{
	LOG.PrintTimeAndLog( 0, "DB OnSelectTournamentCustomRound Call %d", dwTourInfoIndex );
	//2046=game_league_round_data INT
	const int iQueryID = 2046;

	Reset();

	m_FT.Write( dwTourInfoIndex );

	//  - 라운드인덱스 : int / 라운드1시간 : int / 라운드1선물1 : int / 라운드1선물2 : int / 라운드1선물3 : int / 라운드1선물4 : int x TOURNAMENT_CUSTOM_MAX_ROUND 
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );	
	for(int i = 0;i < TOURNAMENT_CUSTOM_MAX_ROUND;i++)
	{
		m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );	
		m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );	
		m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );	
		m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );	
		m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );	
	}

	m_Query.SetReturnData( &dwTourIndex, sizeof(int) );
	m_Query.SetData( m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CUSTOM_ROUND_GET, _SELECTDB, iQueryID, m_FT,	m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnInsertTournamentConfirmUser( DWORD dwTourIndex, DWORD dwUserIndex )
{
	//2047=game_league_auth_add INT INT
	const int iQueryID = 2047;

	Reset();

	m_FT.Write( dwTourIndex );
	m_FT.Write( dwUserIndex );

	m_Query.SetData( m_iDBAgentThreadID++, _RESULT_DESTROY, DBAGENT_TOURNAMENT_CONFIRM_USER_SET, _INSERTDB, iQueryID, m_FT,	m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectTournamentConfirmUserList( DWORD dwTourIndex, DWORD dwLastUserIndex, int iSelectCount )
{	
	//2048=game_league_auth_list INT INT INT
	const int iQueryID = 2048;

	Reset();

	m_FT.Write( iSelectCount );
	m_FT.Write( dwLastUserIndex );
	m_FT.Write( dwTourIndex );

	// - 유저인덱스, 닉네임, 레벨
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				
	m_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );				
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );				

	m_Query.SetReturnData( &dwTourIndex, sizeof(DWORD) );
	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_CONFIRM_USER_LIST, _SELECTEX1DB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnUpdateTournamentInfoDataSave( DWORD dwTourInfoIndex, ioHashString &rkAnnounce, DWORD dwAppDate, DWORD dwDelayDate )
{
	//2049=game_league_info_save INT STR INT INT
	const int iQueryID = 2049;

	Reset();
	
	std::string szResultAnnounce;
	Help::GetSafeTextWriteDB( szResultAnnounce, rkAnnounce );        //DB에 들어가서는 안될 문자를 변경한다

	m_FT.Write( dwTourInfoIndex );
	m_FT.Write( szResultAnnounce.c_str(), szResultAnnounce.length(), TRUE );
	m_FT.Write( dwAppDate );
	m_FT.Write( dwDelayDate );

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_INFO_DATA_SAVE_UPD, _UPDATEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnUpdateTournamentRoundDataSave( cSerialize &m_FT )
{
	//2050=game_league_round_save INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT
	const int iQueryID = 2050;

	m_VT.clear();

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_ROUND_DATA_SAVE_UPD, _UPDATEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnDeleteTournamentCustomData( DWORD dwTourIndex )
{
	//2057=game_league_delete INT
	const int iQueryID = 2057;

	Reset();

	m_FT.Write( dwTourIndex );
	m_Query.SetData( m_iDBAgentThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_CUSTOM_DATA_DELETE, _DELETEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnInsertTournamentCustomBackup( DWORD dwTourIndex )
{
	//2058=game_league_backup_user INT

	LOG.PrintTimeAndLog( 0, "DB OnInsertTournamentCustomBackup Call %d", dwTourIndex );

	const int iQueryID = 2058;

	Reset();

	m_FT.Write( dwTourIndex );
	m_Query.SetData( m_iDBAgentThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_CUSTOM_BACKUP, _UPDATEDB, iQueryID, m_FT,	m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnInsertTournamentCustomReward( DWORD dwTourIndex )
{
	//2059=game_league_user_reward_add INT

	LOG.PrintTimeAndLog( 0, "DB OnInsertTournamentCustomReward Call %d", dwTourIndex );

	const int iQueryID = 2059;

	Reset();

	m_FT.Write( dwTourIndex );
	m_Query.SetData( m_iDBAgentThreadID, _RESULT_DESTROY, DBAGENT_TOURNAMENT_CUSTOM_REWARD, _INSERTDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnUpdateRelativeGrade( DWORD dwUniqueCode, int iReduceRate )
{
	const int iQueryID = 2082;  // game_relative_ranking_week INT INT

	Reset();

	m_FT.Write( dwUniqueCode );
	m_FT.Write( iReduceRate );

	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_DESTROY, DBAGENT_RELATIVE_GRADE_UPDATE, _UPDATEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnSelectPrevTournamentChampInfo( DWORD dwTourIndex )
{
	// 2088=game_league_history_get_winner_team_info
	const int iQueryID = 2088;

	Reset();

	// 팀명, 진영타입
	m_VT.push_back( GetValueType( vChar, TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE ) );
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );

	m_Query.SetReturnData( &dwTourIndex, sizeof(DWORD) );	
	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_TOURNAMENT_PREV_CAMP_INFO_GET, _SELECTDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << m_Query;
	if( !SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "DB OnSelectPrevTournamentChampInfo Send Fail! :%d - %d", GetLastError(), iQueryID );
		return;
	}
	LOG.PrintTimeAndLog( 0, "OnSelectPrevTournamentChampInfo Send" );
}

void DBClient::OnUserBlock( const ioHashString& szPublicID, BYTE byLimitType, CTime limitTime, const ioHashString& szReportID, const ioHashString& szReportIP, const ioHashString& szReason, const ioHashString& szNote, const DWORD dwMgrToolIndex )
{
	const int iQueryID = 2089;

	Reset();

	SYSTEMTIME limitSystemTime;
	limitTime.GetAsSystemTime( limitSystemTime );

	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szPublicID.c_str(), szPublicID.Length(), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (uint8)byLimitType ) );
	PACKET_GUARD_VOID( m_FT.Write( (uint8*)&limitSystemTime, sizeof(limitSystemTime), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szReportID.c_str(), szReportID.Length(), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szReportIP.c_str(), szReportIP.Length(), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szReason.c_str(), szReason.Length(), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szNote.c_str(), szNote.Length(), TRUE ) );

	m_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );

	m_Query.SetReturnData( &dwMgrToolIndex, sizeof(DWORD) );
	m_Query.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );
	
	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_USERBLOCK_SET, _INSERTDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnChangeUserBlock(const int ClientNumber, const ioHashString& szPublicID, BYTE byLimitType, CTime limitTime, const ioHashString& szReportID, const ioHashString& szReportIP, const ioHashString& szReason, const ioHashString& szNote, const DWORD dwMgrToolIndex )
{
	const int iQueryID = 2089;

	Reset();

	SYSTEMTIME limitSystemTime;
	limitTime.GetAsSystemTime( limitSystemTime );


	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szPublicID.c_str(), szPublicID.Length(), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (uint8)byLimitType ) );
	PACKET_GUARD_VOID( m_FT.Write( (uint8*)&limitSystemTime, sizeof(limitSystemTime), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szReportID.c_str(), szReportID.Length(), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szReportIP.c_str(), szReportIP.Length(), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szReason.c_str(), szReason.Length(), TRUE ) );
	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szNote.c_str(), szNote.Length(), TRUE ) );

	m_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );

	m_Query.SetReturnData( &ClientNumber, sizeof(int) );
	m_Query.SetReturnData( &dwMgrToolIndex, sizeof(DWORD) );
	m_Query.SetReturnData( szPublicID.c_str(), ID_NUM_PLUS_ONE );
	
	m_Query.SetData( ++m_iDBAgentThreadID, _RESULT_CHECK, DBAGENT_CHANGE_USERBLOCK_SET, _INSERTDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}
void DBClient::OnDeleteGoodsBuyCount(const BYTE byType)
{
	const int iQueryID = 190;

	Reset();

	// 타입
	PACKET_GUARD_VOID( m_FT.Write( byType ) );
	m_VT.push_back( GetValueType(vLONG, sizeof(LONG)) );

	m_Query.SetData( 0, _RESULT_CHECK, DBAGENT_EVENTSHOP_BUYCOUNT_DEL, _DELETEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(m_Query) );
	if( !SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "DB OnDeleteGoodsBuyCount Send Fail! :%d - %d", GetLastError(), iQueryID );
		return;
	}
	LOG.PrintTimeAndLog( 0, "OnDeleteGoodsBuyCount Send" );
}

void DBClient::OnSelectGoodsBuyCount(const int iCount, const int iPage)
{
	const int iQueryID = 188;

	Reset();

	PACKET_GUARD_VOID( m_FT.Write( iCount ) );
	PACKET_GUARD_VOID( m_FT.Write( iPage ) );
	m_VT.push_back( GetValueType(vLONG, sizeof(LONG)) ); // UserIndex
	m_VT.push_back( GetValueType(vChar, sizeof(char)) ); // Type
	m_VT.push_back( GetValueType(vLONG, sizeof(LONG)) ); // GoodsIndex
	m_VT.push_back( GetValueType(vChar, sizeof(char)) ); // BuyCount

	m_Query.SetData( 0, _RESULT_CHECK, DBAGENT_EVENTSHOP_BUYCOUNT_GET, _SELECTDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(m_Query) );
	if( !SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "DB OnSelectGoodsBuyCount Send Fail! :%d - %d", GetLastError(), iQueryID );
		return;
	}
	//LOG.PrintTimeAndLog( 0, "OnSelectGoodsBuyCount Send" );
}

void DBClient::OnUpdateGoodsBuyCount(const DWORD dwUserIndex, const BYTE byType, const DWORD dwGoodsIndex, const BYTE byCount)
{
	const int iQueryID = 189;

	Reset();

	PACKET_GUARD_VOID( m_FT.Write( dwUserIndex ) );
	PACKET_GUARD_VOID( m_FT.Write( byType ) );
	PACKET_GUARD_VOID( m_FT.Write( dwGoodsIndex ) );
	PACKET_GUARD_VOID( m_FT.Write( byCount ) );
	m_VT.push_back( GetValueType(vLONG, sizeof(LONG)) ); // Result

	m_Query.SetData( dwUserIndex, _RESULT_CHECK, DBAGENT_EVENTSHOP_BUYCOUNT_SET, _UPDATEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(m_Query) );
	if( !SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "DB OnUpdateGoodsBuyCount Send Fail! :%d - %d", GetLastError(), iQueryID );
		return;
	}
	//LOG.PrintTimeAndLog( 0, "OnUpdateGoodsBuyCount Send" );
}

void DBClient::OnResetBingoNumber()
{
	//game_reset_bingonumber
	const int iQueryID = 201;
	
	Reset();

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_RESET_BINGONUMBER, 
		_DELETEDB,
		iQueryID, 
		m_FT, 
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
	LOG.PrintTimeAndLog( 0, "OnDeleteGoodsBuyCount Send" );
}

void DBClient::OnResetOldMissionData( IN int iYear, IN int iMonth, IN int iDay )
{
	//game_reset_MissionData
	const int iQueryID = 401;
	Reset();
	char szData[MAX_PATH] = "";
	StringCbPrintf(szData, sizeof(szData), "%04d-%02d-%02d",iYear,iMonth,iDay);
	//m_FT.Write( szData, TRUE);
	PACKET_GUARD_VOID( m_FT.Write( (const uint8*)szData, sizeof(szData), TRUE ) );
	//m_VT.push_back( GetValueType(vChar, sizeof(szData)) );
	m_Query.SetData( 0, _RESULT_CHECK, DBAGENT_RESET_MISSION_DATA, 	_DELETEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(m_Query) );
	if( !SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "DB OnResetOldMissionData Send Fail! :%d - %d", GetLastError(), iQueryID );
		return;
	}
	LOG.PrintTimeAndLog( 0, "OnResetOldMissionData Send - %s", szData );
}

//점검이 있는 첫주에 한번만 실행되며, 12월일 경우 11월 15일 이전 데이터를 삭제합니다.
void DBClient::OnDeleteOldMissionData()
{
	if( m_bOnceRun ) return;
	
	CTime	cCurrentTime	= CTime::GetCurrentTime();
	if( cCurrentTime.GetDay() > 7 ) return;
	
	char szDate[MAX_PATH] = "";
	_itoa(cCurrentTime.GetYear(),szDate, 10);
	PACKET_GUARD_VOID( m_FT.Write(szDate, sizeof(szDate), TRUE ) );
	_itoa((cCurrentTime.GetMonth()==1)?12:cCurrentTime.GetMonth()-1,szDate, 10);
	PACKET_GUARD_VOID( m_FT.Write(szDate, sizeof(szDate), TRUE ) );
	_itoa(15,szDate, 10);
	PACKET_GUARD_VOID( m_FT.Write(szDate, sizeof(szDate), TRUE ) );
	//game_mission_data_delete
	const int iQueryID = 2112;
	m_VT.push_back( GetValueType(vLONG, sizeof(LONG)) ); // Result
	m_Query.SetData( 0, _RESULT_CHECK, DBAGENT_DELETE_OLD_MISSION_DATA, _DELETEDB, iQueryID, m_FT, m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(m_Query) );
	if( !SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "DB OnDeleteOldMissionData Send Fail! :%d - %d", GetLastError(), iQueryID );
		return;
	}

	LOG.PrintTimeAndLog( 0, "[EXEC] DB OnDeleteOldMissionData Send [yyyy-mm-dd] : %d-%d-%d", cCurrentTime.GetYear(),(cCurrentTime.GetMonth()==1)?12:cCurrentTime.GetMonth()-1,15);

	m_bOnceRun = true;
}


//점검이 있는 첫주에 한번만 실행되며, 12월일 경우 11월 15일 이전 데이터를 삭제합니다.
void DBClient::OnSelectPracticeInfoList( DWORD dwUserIndex, int iPracticeIndex, int iSelectCount )
{
	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec game_guild_get_list %d, %d", iSelectCount, iGuildIDX );

	const int iQueryID = 820;

	Reset();

	m_FT.Write(iSelectCount );
	m_FT.Write(dwUserIndex );
	m_FT.Write(iPracticeIndex );

	// 기본 정보
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					//수련장 인덱스
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );					//유저 인덱스
	m_VT.push_back( GetValueType( vChar, ID_NUM_PLUS_ONE ) );		
	m_VT.push_back( GetValueType( vLONG, sizeof(LONG) ) );					//수련장 타임
	m_VT.push_back( GetValueType(vTimeStamp,sizeof(DBTIMESTAMP))); //접속 날짜.

	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PRACTICE_TOTAL_INFO_GET, 
		_SELECTEX1DB,
		iQueryID, 
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}

void DBClient::OnPractice_Index_RankPresent(ioHashString szSenderId, std::vector< SPracticeReward > vSPracticeReward)
{
	const int iQueryID = 821;

	Reset();

	for( std::vector< SPracticeReward >::iterator it = vSPracticeReward.begin() ; it != vSPracticeReward.end() ; ++it )
	{
		SPracticeReward kPracticeReward = (*it);
		m_FT.Write( kPracticeReward.m_iPracticeRewadStart );
		m_FT.Write( kPracticeReward.m_iPracticeRewadEnd );
		m_FT.Write( kPracticeReward.m_iPracticeRewadment );
		m_FT.Write( kPracticeReward.m_iPracticeRewadperiad );
		for(int k = 0;k < PracticePresentCOUNT;k++)
		{
			m_FT.Write( kPracticeReward.m_kPresent[k].m_iPracticeRewadType );
			m_FT.Write( kPracticeReward.m_kPresent[k].m_iPracticeRewadCode );
			m_FT.Write( kPracticeReward.m_kPresent[k].m_iPracticeRewadCount );
		}
	}
	/*
	int RankAStart = 0, RankAEnd = 0, PresentAType = 0, PresentACode = 0, msgAType = 0, LimitDataA = 0;
	int RankBStart = 0, RankBEnd = 0, PresentBType = 0, PresentBCode = 0, msgBType = 0, LimitDataB = 0;
	int RankCStart = 0, RankCEnd = 0, PresentCType = 0, PresentCCode = 0, msgCType = 0, LimitDataC = 0;
	int RankDStart = 0, RankDEnd = 0, PresentDType = 0, PresentDCode = 0, msgDType = 0, LimitDataD = 0;
	RankAStart		= vSPracticeReward[0].m_iPracticeRewadStart;
	RankAEnd		= vSPracticeReward[0].m_iPracticeRewadEnd;
	PresentAType	= vSPracticeReward[0].m_iPracticeRewadType;
	PresentACode	= vSPracticeReward[0].m_iPracticeRewadCode;
	msgAType		= vSPracticeReward[0].m_iPracticeRewadment;
	LimitDataA		= vSPracticeReward[0].m_iPracticeRewadperiad;

	RankBStart		= vSPracticeReward[1].m_iPracticeRewadStart;
	RankBEnd		= vSPracticeReward[1].m_iPracticeRewadEnd;
	PresentBType	= vSPracticeReward[1].m_iPracticeRewadType;
	PresentBCode	= vSPracticeReward[1].m_iPracticeRewadCode;
	msgBType		= vSPracticeReward[1].m_iPracticeRewadment;
	LimitDataB		= vSPracticeReward[1].m_iPracticeRewadperiad;

	RankCStart		= vSPracticeReward[2].m_iPracticeRewadStart;
	RankCEnd		= vSPracticeReward[2].m_iPracticeRewadEnd;
	PresentCType	= vSPracticeReward[2].m_iPracticeRewadType;
	PresentCCode	= vSPracticeReward[2].m_iPracticeRewadCode;
	msgCType		= vSPracticeReward[2].m_iPracticeRewadment;
	LimitDataC		= vSPracticeReward[2].m_iPracticeRewadperiad;

	RankDStart		= vSPracticeReward[3].m_iPracticeRewadStart;
	RankDEnd		= vSPracticeReward[3].m_iPracticeRewadEnd;
	PresentDType	= vSPracticeReward[3].m_iPracticeRewadType;
	PresentDCode	= vSPracticeReward[3].m_iPracticeRewadCode;
	msgDType		= vSPracticeReward[3].m_iPracticeRewadment;
	LimitDataD		= vSPracticeReward[3].m_iPracticeRewadperiad;

	m_FT.Write( vSPracticeReward[0].m_iPracticeRewadStart	 );
	m_FT.Write( vSPracticeReward[0].m_iPracticeRewadEnd	 );
	m_FT.Write( PresentAType );
	m_FT.Write( PresentACode );
	m_FT.Write( vSPracticeReward[0].m_iPracticeRewadment	 );
	m_FT.Write( vSPracticeReward[0].m_iPracticeRewadperiad   );

	m_FT.Write( vSPracticeReward[1].m_iPracticeRewadStart	 );
	m_FT.Write( vSPracticeReward[1].m_iPracticeRewadEnd	 );
	m_FT.Write( PresentAType );
	m_FT.Write( PresentACode );
	m_FT.Write( vSPracticeReward[1].m_iPracticeRewadment	 );
	m_FT.Write( vSPracticeReward[1].m_iPracticeRewadperiad   );

	m_FT.Write( RankCStart	 );
	m_FT.Write( RankCEnd	 );
	m_FT.Write( PresentCType );
	m_FT.Write( PresentCCode );
	m_FT.Write( msgCType	 );
	m_FT.Write( LimitDataC	 );

	m_FT.Write( RankDStart	 );
	m_FT.Write( RankDEnd	 );
	m_FT.Write( PresentDType );
	m_FT.Write( PresentDCode );
	m_FT.Write( msgDType	 );
	m_FT.Write( LimitDataD	 );
	*/
	m_FT.Write( (const uint8*)szSenderId.c_str(), 20, TRUE );

	// 기본 정보
	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PRACTICE_INDEX_RANK_PRESENT, 
		_SELECTEX1DB,
		iQueryID, 
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}


void DBClient::OnPractice_BlockUserList(ioHashString szKickId)
{
	const int iQueryID = 822;

	Reset();

	m_FT.Write( (const uint8*)szKickId.c_str(), 20, TRUE );
	
	m_VT.push_back( GetValueType( vLONG, sizeof(DWORD) ) );					//유저 인덱스

	// 기본 정보
	m_Query.SetData( 
		++m_iDBAgentThreadID, 
		_RESULT_CHECK, 
		DBAGENT_PRACTICE_BLOCKUSER_LIST, 
		_SELECTEX1DB,
		iQueryID, 
		m_FT,
		m_VT );

	SP2Packet kPacket( DTPK_QUERY );
	if( kPacket.Write(m_Query) )
	{
		SendMessage( kPacket );
	}
}