// LogDBClient.cpp: implementation of the LogDBClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogDBClient.h"

#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../Network/GameServer.h"
#include "../Network/iocpHandler.h"
#include "../Network/ioPacketQueue.h"
#include "../NodeInfo/ioCharacter.h"
#include "../NodeInfo/ModeHelp.h"
#include "../NodeInfo/User.h"
#include "../NodeInfo/UserNodeManager.h"
#include "../NodeInfo/ioItemInfoManager.h"
#include "../NodeInfo/Mode.h"
#include "../NodeInfo/Room.h"
#include "../NodeInfo/BattleRoomParent.h"
#include "../NodeInfo/ioSetItemInfo.h"
#include "../NodeInfo/ioSetItemInfoManager.h"
#include "../WemadeLog/ioLogSerialize.h"
#include "../WemadeLog/WemadeLog.h"
#include <strsafe.h>
#include "../NodeInfo/ServerNodeManager.h"
#include "../EtcHelpFunc.h"
#include "../Local/ioLocalParent.h"

extern CLog TradeLOG;



LogDBClient *LogDBClient::sg_Instance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogDBClient::LogDBClient( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	m_dwCurrentTime   = 0;
	m_iClassPriceTime = 0;
	m_iDBAgentPort    = 0;
}

LogDBClient::~LogDBClient()
{

}

LogDBClient &LogDBClient::GetInstance()
{
	if(sg_Instance == NULL)
	{
		ioINILoader kLoader( "ls_config_game.ini" );
		kLoader.SetTitle( "GameLogDB Session" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", MAX_BUFFER );	
		sg_Instance = new LogDBClient( INVALID_SOCKET, iSendBufferSize, MAX_BUFFER * 2 );
		
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][logDB]session sendBuffer size : [%d]", iSendBufferSize );
	}
	return *sg_Instance;
}

void LogDBClient::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}

bool LogDBClient::ConnectTo()
{ 
	if( g_ServerNodeManager.GetServerIndex() == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LogDBClient::ConnectTo ServerIndex is empty" );
		return false;
	}

	// 서버선택
	GenerateLOGDBAgentInfo();

	SOCKET socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( socket == INVALID_SOCKET )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LogDBClient::ConnectTo socket %d[%s:%d]", GetLastError(), m_DBAgentIP.c_str(), m_iDBAgentPort );
		return false;
	}
	sockaddr_in serv_addr;
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr( m_DBAgentIP.c_str() );
	serv_addr.sin_port			= htons( m_iDBAgentPort );
	if( ::connect( socket, (sockaddr*)&serv_addr, sizeof(serv_addr) ) != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LogDBClient::ConnectTo connect %d[%s:%d]", GetLastError(), m_DBAgentIP.c_str(), m_iDBAgentPort );
		closesocket(socket);
		return false;
	}

	g_iocp.AddHandleToIOCP( (HANDLE)socket, (DWORD)this );
	CConnectNode::SetSocket( socket );

	OnCreate();	
	AfterCreate();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][logDB]On connect : [%s] [%d] [%d]", m_DBAgentIP.c_str(), m_iDBAgentPort, 0 );

	if( !IsActive() )  
		return false;

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
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LogDBClient : Disconnect" );
}

void LogDBClient::SessionClose(BOOL safely)
{
	OnDestroy();
}

bool LogDBClient::SendMessage( CPacket &rkPacket )
{
	g_ProcessChecker.LogDBSendMessage( rkPacket.GetPacketID(), rkPacket.GetBufferSize() );

	if(!CConnectNode::SendMessage( rkPacket ))
	{
		int iQueryID = 0;
		if(rkPacket.GetBufferSize() > sizeof(CQueryData))
		{
			CQueryData query_data;

			SP2Packet kPacket  = (SP2Packet&)rkPacket;
			kPacket >> query_data;

			iQueryID = query_data.GetQueryID();
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"LogDBClient [%d] - Send Fail! :%d", iQueryID, GetLastError());
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

void LogDBClient::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
	g_ProcessChecker.LogDBRecvMessage( packet.GetBufferSize() );
}

void LogDBClient::ProcessTime()
{
	if(TIMEGETTIME() - m_dwCurrentTime < 60000) return; // 1분 확인
	
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( !IsActive() )
	{
		ConnectTo();
	}
	m_dwCurrentTime = TIMEGETTIME();

	static int i1minsCnt = 0;
	i1minsCnt++;
	if( i1minsCnt > 5 ) // 5분
	{
		//EU 일 경우 위메이드 채널링 정보만 보낸다. 
		if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_EU )
		{
		OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
					   , g_App.GetGameServerName(), g_UserNodeManager.GetNodeSizeByChannelingType( CNT_WEMADEBUY ), CNT_WEMADEBUY );
		}
		//US 일 경우 북미유저 + 스팀 유저 정보 보낸다.
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US )
		{
			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
			       , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_WEMADEBUY ), CNT_WEMADEBUY );

		OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
				, g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_STEAM ), CNT_STEAM );
		}
#ifdef __OHTG_BILLING_TAIWAN_HAPPYTUK__
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_TAIWAN )
		{
			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
			       , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_WEMADEBUY ), CNT_WEMADEBUY );

		OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
				, g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_HAPPYTUK ), CNT_HAPPYTUK );
		}
		else 
#endif //__OHTG_BILLING_TAIWAN_HAPPYTUK__
		{
			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
					   , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_WEMADEBUY ), CNT_WEMADEBUY );

			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
					   , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_MGAME ), CNT_MGAME );

			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
					   , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_DAUM ), CNT_DAUM );

			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
          			   , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_NAVER ), CNT_NAVER );

			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
					   , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_TOONILAND ), CNT_TOONILAND );

			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
					   , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_NEXON ), CNT_NEXON );

			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
					   , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_HANGAME ), CNT_HANGAME );

			OnConConnect(g_App.GetGameServerID(), g_App.GetPublicIP(), g_App.GetCSPort()
					   , g_App.GetGameServerName(), g_UserNodeManager.GetChennelingMaxUserCnt( CNT_VALOFE ), CNT_VALOFE );

			//각 채널링 유저수 현재의 채널링 유저수로 동기화 
			g_UserNodeManager.SetMaxUserCntToCurCnt();

			i1minsCnt = 0;
		}
	}
}

void LogDBClient::ProcessPing()
{
	if( IsActive() )
	{
		OnPing();
	}
}

void LogDBClient::ProcessFlush()
{
	if( !IsActive() )
		return;
	if( GetSocket() == INVALID_SOCKET )
		return;
	
	CSendIO::FlushSendBuffer();
}

void LogDBClient::PacketParsing( CPacket &packet )
{

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

	CQueryData query_data;
	query_data.SetReturnData( &dwCurrent, sizeof(DWORD) );

	cSerialize v_FT;
	vVALUETYPE v_VT;

	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(), 
		_RESULT_CHECK, 
		DBAGENT_LOG_PINGPONG, 
		_UPDATEDB,
		queryId, 
		v_FT, 
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;
	SendMessage( kPacket );
}
//////////////////////////////////////////////////////////////////////////
// SEND
//////////////////////////////////////////////////////////////////////////
void LogDBClient::OnInsertPlayResult( ModeRecord *pRecord, PlayResultType ePlayResultType )
{
	if( pRecord == NULL )
		return;

	if( pRecord->bWriteLog )
		return;

	if( pRecord->pUser == NULL )
		return;

	if( pRecord->pUser->GetPublicID().IsEmpty() )
		return;

	ModeType eModeType = MT_NONE;
	int iModeSubNum = 0;
	int iModeMapNum = 0;
	int iBlueTeamWinCnt = 0;
	int iRedTeamWinCnt  = 0;
	int iRoomIndex = 0;
	int iRedTeamCount = 0;
	int iBlueTeamCount = 0;


	Room *pRoom = pRecord->pUser->GetMyRoom();
	Mode* pMode = pRoom->GetModeInfo();
	if( pRoom && pMode )
	{

		eModeType = pRoom->GetModeType();
		iModeSubNum = pRoom->GetModeSubNum();
		iModeMapNum = pRoom->GetModeMapNum();
		iBlueTeamWinCnt = pRoom->GetBlueWinCnt();
		iRedTeamWinCnt  = pRoom->GetRedWinCnt();
		iRoomIndex = pRoom->GetRoomIndex();
		iBlueTeamCount = pMode->GetTeamUserCnt( TEAM_BLUE );
		iRedTeamCount = pMode->GetTeamUserCnt( TEAM_RED );
		
	}

	GameType eGameType = GetGameType( eModeType, pRoom );

	int iTotalWin = 0;
	int iTotalLose = 0;
	if(pRecord->pUser->GetTeam() == TEAM_BLUE)
	{
		iTotalWin = iBlueTeamWinCnt; 
		iTotalLose = iRedTeamWinCnt; 
	}
	else
	{
		iTotalWin = iRedTeamWinCnt;
		iTotalLose = iBlueTeamWinCnt;
	}

	const int iMax = pRecord->pUser->GetCharCount();
	for (int i = 0; i < iMax ; i++)
		OnInsertCharacterResult(pRecord, i );

	int iAllTotalKill = pRecord->GetAllTotalKill();
	int iAllTotalDeath= pRecord->GetAllTotalDeath();
	if( eModeType == MT_HEADQUARTERS )
		iAllTotalKill = iAllTotalDeath = 0;

	DWORD dwPlayingTime = pRecord->GetAllPlayingTime()/1000;
	DWORD dwDeathTime   = pRecord->GetDeathTime()/1000;

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf( str_query, sizeof( str_query ), "exec log_data_play_add %d,'%s',%d,%d,%d,  %d,%d,%d,%d,%d,  %d,%d,%d,%d, %d"
	//	           , pRecord->pUser->GetUserIndex(), pRecord->pUser->GetPublicID().c_str(), eGameType, eModeType, iModeSubNum
	//			   , iModeMapNum, dwPlayingTime, dwDeathTime, pRecord->iTotalAddPeso, iTotalWin
	//			   , iTotalLose, iAllTotalKill, iAllTotalDeath, ePlayResultType , pRecord->pUser->GetPCRoomNumber() );

	const int iQueryID = 1000;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_PLAY );
	v_FT.Append( pRecord->pUser->GetUserIndex() );
	v_FT.Append( pRecord->pUser->GetPublicID().c_str(), pRecord->pUser->GetPublicID().Length(), TRUE  );
	v_FT.Append( (uint32)eGameType  );
	v_FT.Append( (uint32)eModeType  );
	v_FT.Append( iModeSubNum  );
	v_FT.Append( iModeMapNum  );
	v_FT.Append( dwPlayingTime  );
	v_FT.Append( dwDeathTime  );
	v_FT.Append( pRecord->iTotalAddPeso  );
	v_FT.Append( iTotalWin  );
	v_FT.Append( iTotalLose  );
	v_FT.Append( iAllTotalKill  );
	v_FT.Append( iAllTotalDeath  );
	v_FT.Append( (uint32)ePlayResultType  );
	v_FT.Append( pRecord->pUser->GetPCRoomNumber()  );
	v_FT.Append( iRoomIndex  );
	v_FT.Append( (int)pRecord->fContributePer  );
	v_FT.Append( iBlueTeamCount  );
	v_FT.Append( iRedTeamCount  );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertPlayRoomResult( Room* pRoom, PlayResultType ePlayResultType )
{
	ModeType eModeType = MT_NONE;
	int iRoomIndex = 0;
	int iModeSubNum = 0;
	int iBlueTeamWinCnt = 0;
	int iRedTeamWinCnt  = 0;
	int iBlueTeamCount = 0;
	int iRedTeamCount = 0;
	int iPlayTime = 0;
	DWORD dwCurTime = TIMEGETTIME();

	Mode* pMode = pRoom->GetModeInfo();
	if( pRoom && pMode )
	{
		iRoomIndex = pRoom->GetRoomIndex();
		eModeType = pRoom->GetModeType();
		iModeSubNum = pRoom->GetModeSubNum();
		iBlueTeamWinCnt = pRoom->GetBlueWinCnt();
		iRedTeamWinCnt  = pRoom->GetRedWinCnt();
		iBlueTeamCount = pMode->GetTeamUserCnt( TEAM_BLUE );
		iRedTeamCount = pMode->GetTeamUserCnt( TEAM_RED );
		iPlayTime = pRoom->GetRoomPlayTime();
		iPlayTime = (dwCurTime - iPlayTime)/1000;
	}

	GameType eGameType = GetGameType( eModeType, pRoom );

	DWORD dwPlayingTime = pRoom->GetCreateTime();

	const int iQueryID = 1023; // 1023=log_data_play_room_add

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_PLAY_ROOM );
	v_FT.Append( iRoomIndex );
	v_FT.Append( iBlueTeamWinCnt );
	v_FT.Append( iRedTeamWinCnt );
	v_FT.Append( iBlueTeamCount );
	v_FT.Append( iRedTeamWinCnt );
	v_FT.Append( iBlueTeamWinCnt );
	v_FT.Append( iRedTeamCount  );
	v_FT.Append( eGameType  );
	v_FT.Append( eModeType  );
	v_FT.Append( iModeSubNum  );
	v_FT.Append( 0 );
	v_FT.Append( 0 );
	v_FT.Append( ePlayResultType );
	v_FT.Append( iPlayTime );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertCharacterResult( ModeRecord *pRecord, int iArray )
{
	if( pRecord == NULL )
		return;

	if( pRecord->pUser == NULL )
		return;

	if( pRecord->pUser->GetPublicID().IsEmpty() )
		return;

	if( !COMPARE(iArray, 0, pRecord->pUser->GetCurMaxCharSlot()) )
		return;

	int iClassType = pRecord->pUser->GetCharClassType(iArray);

	if(pRecord->GetClassPlayingTime(iClassType) == 0)
		return;

	int iKillCnt = pRecord->GetKillCount( iClassType );
	int iDeathCnt = pRecord->GetDeathCount( iClassType );

	ModeType eModeType = MT_NONE;
	int iModeSubNum = 0;
	int iModeMapNum = 0;
	Room *pRoom = pRecord->pUser->GetMyRoom();
	if( pRoom )
	{
		eModeType  = pRoom->GetModeType();
		iModeSubNum = pRoom->GetModeSubNum();
		iModeMapNum = pRoom->GetModeMapNum();
	}
	GameType eGameType = GetGameType( eModeType, pRoom );

	if( eModeType == MT_HEADQUARTERS )
		iKillCnt = iDeathCnt = 0;

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_character_add %d,'%s',%d,%d,%d,  %d,%d,%d,%d,%d"
	//	         , pRecord->pUser->GetUserIndex(), pRecord->pUser->GetPublicID().c_str(), eGameType, eModeType, iModeSubNum
	//			 , iModeMapNum, pRecord->GetClassPlayingTime(iClassType)/1000, iClassType, iKillCnt, iDeathCnt);

	const int iQueryID = 1001;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_CHARACTER );
	v_FT.Append( pRecord->pUser->GetUserIndex() );
	v_FT.Append( pRecord->pUser->GetPublicID().c_str(), pRecord->pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( eGameType );
	v_FT.Append( eModeType );
	v_FT.Append( iModeSubNum );
	v_FT.Append( iModeMapNum );
	v_FT.Append( pRecord->GetClassPlayingTime(iClassType)/1000 );
	v_FT.Append( iClassType );
	v_FT.Append( iKillCnt );
	v_FT.Append( iDeathCnt );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY,
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertPeso( User *pUser, int iPeso, PesoType ePesoType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_peso_add %d,'%s',%d,%d"
	//	          , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPeso, ePesoType );

	const int iQueryID = 1002;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_PESO );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( iPeso );
	v_FT.Append( ePesoType );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	SendMessage( kPacket );
}

void LogDBClient::OnInsertTime( User *pUser, TimeType eTimeType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	// 투토리얼 유저는 따로 시간을 계산하므로 여기서는 기록하지 않는다.
	if( pUser->GetUserState() != US_TUTORIAL_CLEAR )
		return;

	if( eTimeType == TT_VIEW )
	{
		Room *pRoom = pUser->GetMyRoom();
		if( pRoom && pRoom->GetRoomStyle() == RSTYLE_LADDERBATTLE )
			eTimeType = TT_VIEW_LADDER;
	}
	else if( eTimeType == TT_EXIT_PROGRAM )
	{
		// 본부에서 나가는 경우이므로 룸에서 나가는 경우는 예외 처리함.
		if( pUser->GetMyRoom() )
			return;
	}
	
	if( eTimeType == TT_EXCAVATING )
	{
#ifdef EXCAVATION_EX_BYBCKIM		// 2018-09-12 by bckim, 탐사 진행 시간 로그 [dbo].[log_data_time]
		ioUserExcavation* pTemp = pUser->GetUserExcavation();
		if( pTemp == NULL )
			return;
		if( pTemp->GetExcavating_Ex_Time() == 0 )
			return;
#else
		if( pUser->GetExcavatingTime() == 0 )
			return;
#endif // EXCAVATION_EX_BYBCKIM_DEBUG		
	}
	else
	{
		if( pUser->GetStartTimeLog() == 0 )
			if( eTimeType != TT_LADDER_MATCH )
				return;
	}

	DWORD dwPassedTime = 0;

#ifdef EXCAVATION_EX_BYBCKIM			// 2018-09-12 by bckim, 탐사 진행 시간 로그 [dbo].[log_data_time]

	if( eTimeType == TT_EXCAVATING )
	{
		ioUserExcavation* pTemp = pUser->GetUserExcavation();
		if( pTemp == NULL )
			return;
		dwPassedTime = ( TIMEGETTIME() - pTemp->GetExcavating_Ex_Time() )/1000;
	}
#else
	if( eTimeType == TT_EXCAVATING )
		dwPassedTime = ( TIMEGETTIME() - pUser->GetExcavatingTime() )/1000;
#endif // EXCAVATION_EX_BYBCKIM		
	else if( eTimeType == TT_LADDER_MATCH )
	{
		dwPassedTime = ( TIMEGETTIME() - pUser->GetLadderMatchTime() )/1000;
	}
	else
		dwPassedTime = ( TIMEGETTIME() - pUser->GetStartTimeLog() )/1000;

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_time_add %d,'%s',%d,%d"
	//	         , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), dwPassedTime, eTimeType );

	const int iQueryID = 1003;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_TIME );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( dwPassedTime );
	v_FT.Append( eTimeType );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	SendMessage( kPacket );
}

void LogDBClient::OnInsertChar( User *pUser, int iClassType, int iLimitDate, int iPesoPrice, const char *szItemIndex, CharType eCharType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	enum { NOTE_SISZE = 100, };
	char szNote[NOTE_SISZE]="";
	StringCbPrintf( szNote, sizeof( szNote ), "[%s] Soldier(%s)", szItemIndex, GetClassName( iClassType ).c_str() );

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_item_class_add %d,'%s',%d,%d,%d,%d   ,%d,'%s','%s'"
	//	         , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetGradeLevel(), iClassType, iLimitDate, eCharType
	//	         , -iPesoPrice, pUser->GetPublicIP(), szNote );

	const int iQueryID = 1004;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_BUY_ITEM_CLASS );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iClassType );
	v_FT.Append( iLimitDate );
	v_FT.Append( eCharType );
	v_FT.Append( -iPesoPrice );

	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( szNote, sizeof(szNote), TRUE);

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	SendMessage( kPacket );
}

void LogDBClient::OnInsertDeco( User *pUser, int iItemType, int iItemCode, int iPesoPrice, const char *szItemIndex, DecoType eDecoType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	int iClassType = iItemType/100000;
	int iKindred   = ( iItemType%100000 ) / 1000;
	int iDecoType  = iItemType%1000;

	enum { NOTE_SISZE = 100, };
	char szNote[NOTE_SISZE]="";
	StringCbPrintf( szNote, sizeof( szNote ), "[%s] Deco(%s)", szItemIndex, GetClassName( iClassType ).c_str() );

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_item_decoration_add %d,'%s',%d,%d,%d   ,%d,%d,%d,'%s','%s'"
	//	         , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iClassType, iKindred, iDecoType
	//			 , iItemCode, eDecoType, -iPesoPrice, pUser->GetPublicIP(), szNote );

	const int iQueryID = 1005;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_BUY_ITEM_DECORATION );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( iClassType );
	v_FT.Append( iKindred );
	v_FT.Append( iDecoType );
	v_FT.Append( iItemCode );
	v_FT.Append( eDecoType );
	v_FT.Append( -iPesoPrice );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( szNote, sizeof(szNote), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertExtraItem( User *pUser, int iItemCode, int iReinforce, int iMachineCode, int iLimitDays, int iPesoPrice, int iPeriodType, DWORD dwMaleCustom, DWORD dwFemaleCustom, const char *szItemIndex, ExtraType eExtraType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	// iItemCode->DB에서는 itemX_type으로 저장됨 
	enum LogPartsType
	{
		LPT_WEAPON = 1,
		LPT_ARMOR  = 2,
		LPT_HELMET = 3,
		LPT_CLOAK  = 4,

	};
	LogPartsType eParts = (LogPartsType) ( ( iItemCode / 100000 ) + 1 ); // +1은 재우씨 요청으로 DB에서 0부터 시작하는 타입은 구분하기 어렵다고 하여 parts 타입에 +1 더함. 로그에서만 사용됨

	enum { NOTE_SIZE = 100, };
	char szNote[NOTE_SIZE]="";
	StringCbPrintf( szNote, sizeof( szNote ), "[%s] Extra(%u:%u)", szItemIndex, dwMaleCustom, dwFemaleCustom );
	if( iPeriodType == ioUserExtraItem::EPT_MORTMAIN )
		StringCbCat( szNote, sizeof( szNote ), " Permanent" );

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_item_equip_add %d,'%s',%d,%d,%d   ,%d,%d,%d,%d,'%s'   ,'%s'"
	//	         , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), eParts, iItemCode, iReinforce
	//	         , iMachineCode, iLimitDays, eExtraType, -iPesoPrice, pUser->GetPublicIP()
	//	         , szNote );

	const int iQueryID = 1006;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_BUY_ITEM_EQUIP );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( eParts);
	v_FT.Append( iItemCode );
	v_FT.Append( iReinforce );
	v_FT.Append( iMachineCode );
	v_FT.Append( iLimitDays );
	v_FT.Append( eExtraType );
	v_FT.Append( -iPesoPrice );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE);
	v_FT.Append( szNote, sizeof(szNote), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertEtc( User *pUser, int iItemType, int iItemValue, int iPesoPrice, const char *szItemIndex, EtcType eEtcType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	enum { NOTE_SISZE = 100, };
	char szNote[NOTE_SISZE]="";
	StringCbPrintf( szNote, sizeof( szNote ), "[%s] Etc", szItemIndex );

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_item_special_add  %d,'%s',%d,%d,%d   ,%d,'%s','%s'"
	//	         , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iItemType, iItemValue, eEtcType
	//	         , -iPesoPrice, pUser->GetPublicIP(), szNote );

	const int iQueryID = 1007;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_BUY_ITEM_SPECIAL );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( iItemType );
	v_FT.Append( iItemValue );
	v_FT.Append( eEtcType );
	v_FT.Append( -iPesoPrice );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( szNote, sizeof(szNote), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	SendMessage( kPacket );
}

void LogDBClient::OnInsertTutorialTime( User *pUser )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetStartTimeLog() == 0)
		return;

	int iTutorialLevel = 0;
	if( pUser->GetUserState() == US_TUTORIAL_CLEAR) 
		iTutorialLevel = -1;
	else
		iTutorialLevel = pUser->GetUserState() % 100;


	DWORD dwTotalTime = (TIMEGETTIME() - pUser->GetStartTimeLog() )/1000;

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_tutorial_add  %d,'%s',%d,%d", pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iTutorialLevel, dwTotalTime);

	const int iQueryID = 1008;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_TUTORIAL );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( iTutorialLevel );
	v_FT.Append( dwTotalTime );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertCashItem( User *pUser, int iItemType, int iItemValue, int iCashPrice, const char *szItemIndex, CashItemType eCashItemType, ioHashString szSubscriptionID )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	enum { NOTE_SISZE = 200, };
	char szNote[NOTE_SISZE]="";
	int iBuyType = 0;	// 0 : 일반, 1 : 선물, 2: 팝업
	int iType = 0;
	static IntVec vValues;
	vValues.clear();

	if( eCashItemType == CIT_CHAR )
	{
		iType = CIT_CHAR;
		int iClassType = iItemType;
		StringCbPrintf( szNote, sizeof( szNote ), "[%s] Soldier(%s)", szItemIndex, GetClassName(iClassType).c_str() );
	}
	else if( eCashItemType == CIT_DECO )
	{
		iType = CIT_DECO;
		int iClassType = iItemType/100000;
		StringCbPrintf( szNote, sizeof( szNote ), "[%s] Deco(%s)", szItemIndex, GetClassName(iClassType).c_str() );
	}
	else if( eCashItemType == CIT_ETC )
	{
		iType = CIT_ETC;
		StringCbPrintf( szNote, sizeof( szNote ), "[%s] Etc", szItemIndex );
	}
	else if( eCashItemType == CIT_EXTRA )
	{
		iType = CIT_EXTRA;
		StringCbPrintf( szNote, sizeof( szNote ), "[%s] Extra", szItemIndex );
	}
	else if( eCashItemType == CIT_PRESENT )
	{
		Help::TokenizeToINT(szItemIndex, ":", vValues);
		if( !vValues.empty() )
			iType = vValues[0];

		StringCbPrintf( szNote, sizeof( szNote ), "[%s] Present", szItemIndex );
		iBuyType = 1;
	}
	else if( eCashItemType == CIT_POPUP )
	{
		Help::TokenizeToINT(szItemIndex, ":", vValues);
		if( !vValues.empty() )
			iType = vValues[0];

		StringCbPrintf( szNote, sizeof( szNote ), "[%s] PopUp", szItemIndex );
		iBuyType = 2;
	}
	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_item_gold_add  %d,'%s',%d,%d,%d,%d  ,%d,%d,'%s','%s'"
	//	         , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetGradeLevel(), eCashItemType, iItemType, iItemValue
	//			 , -iCashPrice, pUser->GetChannelingType(), pUser->GetPublicIP(), szNote );

	const int iQueryID = 1009;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_BUY_ITEM_GOLD );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iType );
	v_FT.Append( iItemType );
	v_FT.Append( iItemValue );
	v_FT.Append( -iCashPrice );
	v_FT.Append( (int16)(pUser->GetChannelingType()) );
	v_FT.Append( szSubscriptionID.c_str(), szSubscriptionID.Length(), TRUE );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( szNote, sizeof(szNote), TRUE );
	v_FT.Append( (BYTE)iBuyType );
	
	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	SendMessage( kPacket );
}

void LogDBClient::OnInsertPresent( DWORD dwSendUserIndex, const ioHashString &rszSendPublicID, const char *szSendPublicIP, DWORD dwRecievUserIndex, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, PresentType ePresentType, const char *szNote )
{
	if( rszSendPublicID.IsEmpty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s PublicID is empty. (%d:%d)", __FUNCTION__ , dwSendUserIndex, dwRecievUserIndex );
		return;
	}

	if( szSendPublicIP == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s IP is NULL. (%d:%d)", __FUNCTION__ , dwSendUserIndex, dwRecievUserIndex );
		return;
	}

	enum { NOTE_SISZE = 100, };
	char szShortNote[NOTE_SISZE]="";
	StringCbCopyN( szShortNote, sizeof( szShortNote ), szNote, NOTE_SISZE-1 );

	char szAddNote[NOTE_SISZE]="";
	StringCbPrintf( szAddNote, sizeof( szAddNote ), " %d:%d", iPresentValue3, iPresentValue4  );
	StringCbCat( szShortNote, sizeof( szShortNote ), szAddNote );

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_present_add %d,'%s',%d,%d,%d  ,%d,%d,'%s','%s'"
	//	          , dwSendUserIndex, rszSendPublicID.c_str(), dwRecievUserIndex, iPresentType, iPresentValue1
	//			  , iPresentValue2, (int)ePresentType, szSendPublicIP, szShortNote );

	const int iQueryID = 1010;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_PRESENT );
	v_FT.Append( dwSendUserIndex );
	v_FT.Append( rszSendPublicID.c_str(), rszSendPublicID.Length(), TRUE );
	v_FT.Append( dwRecievUserIndex );
	v_FT.Append( iPresentType );
	v_FT.Append( iPresentValue1 );
	v_FT.Append( iPresentValue2 );
	v_FT.Append( (int)ePresentType );
	v_FT.Append( szSendPublicIP, strlen(szSendPublicIP), TRUE );
	v_FT.Append( szShortNote, strlen(szShortNote), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertTrade( DWORD dwUserIndex,
								 const ioHashString &rszPublicID,
								 DWORD dwTradeIndex,
								 DWORD dwItemType,
								 DWORD dwMagicCode,
								 DWORD dwValue,
								 __int64 iItemPrice,
								 TradeSysType eType,
								 const char *szPublicIP,
								 const char *szNote )
{
	if( rszPublicID.IsEmpty() )
	{
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s PublicID is empty. (%d:%d)", __FUNCTION__ , dwUserIndex, dwTradeIndex );
		return;
	}

	if( szPublicIP == NULL )
	{
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s IP is NULL. (%d:%d)", __FUNCTION__ , dwUserIndex, dwTradeIndex );
		return;
	}

	enum { NOTE_SISZE = 100, };
	char szShortNote[NOTE_SISZE]="";
	StringCbCopyN( szShortNote, sizeof( szShortNote ), szNote, NOTE_SISZE-1 );

	//char szQuery[MAX_PATH] = "";
	//StringCbPrintf(szQuery, sizeof(szQuery), "exec log_data_trade_add %d,'%s',%d,%d,%d,%I64d,%d,'%s','%s'",
	//			   dwUserIndex, rszPublicID.c_str(), dwItemType, dwMagicCode, dwValue, iItemPrice, eType, szPublicIP, szShortNote );

	const int iQueryID = 1011;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_TRADE );
	v_FT.Append( dwUserIndex );
	v_FT.Append( rszPublicID.c_str(), rszPublicID.Length(), TRUE );
	v_FT.Append( dwItemType );
	v_FT.Append( dwMagicCode );
	v_FT.Append( dwValue );
	v_FT.Append( iItemPrice );
	v_FT.Append( eType );
	v_FT.Append( szPublicIP, strlen(szPublicIP), TRUE );
	v_FT.Append( szShortNote, strlen(szShortNote), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnConConnect( const uint64 gameServerId, const ioHashString &szIP, const int iPort , const ioHashString &szServerName , const int iConConnect, ChannelingType eChannelingType )
{
	if(szIP.IsEmpty())
		return;

	if(szServerName.IsEmpty())
		return;

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec game_log_concurrent %s, '%s', %d, '%s', %d, %d"
	//	    ,szGameServerID.c_str(), szIP.c_str(), iPort, szServerName.c_str(), iConConnect, (int) eChannelingType );

	const int iQueryID = 165;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_CONCURRENT );
	v_FT.Append( gameServerId );
	v_FT.Append( szIP.c_str(), szIP.Length(), TRUE );
	v_FT.Append( iPort );
	v_FT.Append( szServerName.c_str(), szServerName.Length(), TRUE );
	v_FT.Append( iConConnect );
	v_FT.Append( (int16) eChannelingType );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}


const ioHashString LogDBClient::GetClassName( const int iClassType )
{
	ioHashString szClassName;

	DWORD dwSetItemCode = iClassType + SET_ITEM_CODE;
	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByCode( dwSetItemCode );
	if( pSetInfo )
		szClassName = pSetInfo->GetName();

	return szClassName;
}

LogDBClient::GameType LogDBClient::GetGameType( ModeType eModeType, Room *pRoom )
{
	GameType eGameType = GT_NONE;

	if( eModeType == MT_TRAINING )
		eGameType = GT_SQUARE;
	else if( eModeType == MT_NONE || eModeType == MT_HEADQUARTERS ) // 본부
		eGameType = GT_NONE;
	else
	{
		if( pRoom && pRoom->GetRoomStyle() == RSTYLE_LADDERBATTLE )
			eGameType = GT_LADDER;
		else if( pRoom && pRoom->GetRoomStyle() == RSTYLE_SHUFFLEROOM )
			eGameType = GT_SHUFFLE;
		else if( pRoom && pRoom->GetRoomStyle() == RSTYLE_MATCHROOM )
			eGameType = GT_MATCH;
		else
			eGameType = GT_BATTLE;
	}

	return eGameType;
}

void LogDBClient::OnInsertMedalItem( User *pUser, int iItemType, int iPeriodType, const char *szItemIndex, MedalType eMedalType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	enum { NOTE_SISZE = 100, };
	char szNote[NOTE_SISZE]="";
	StringCbPrintf( szNote, sizeof( szNote ), "[%s] Medal", szItemIndex );

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_medal_add %d,'%s',%d,%d,%d"
	//	         , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iItemType, iPeriodType, (int)eMedalType );

	const int iQueryID = 1012;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_MEDAL );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( iItemType);
	v_FT.Append( iPeriodType );
	v_FT.Append( (int)eMedalType );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB,
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertExMedalSlot( User *pUser, int iClassType, BYTE iSlotNumber, int iLimitTime, ExMedalType eMedalType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	int iClassLevel = pUser->GetClassLevel( pUser->GetCharArrayByClass( iClassType ), true );

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_medal_extend_add %d,'%s',%d,%d,%d,%d,%d"
	//	, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iClassLevel, iClassType, iSlotNumber, iLimitTime, (int)eMedalType );

	const int iQueryID = 1013;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_MEDAL_EXTEND );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( iClassLevel );
	v_FT.Append( iClassType );
	v_FT.Append( iSlotNumber );
	v_FT.Append( iLimitTime );
	v_FT.Append( (int)eMedalType );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertQuest( User *pUser, DWORD dwMainIndex, DWORD dwSubIndex, QuestType eQuestType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec log_data_quest_add %d, '%s', %d, %d, %d, %d", 
	//	pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetGradeLevel(), dwMainIndex, dwSubIndex, (int)eQuestType );

	const int iQueryID = 1014;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_QUEST );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( dwMainIndex );
	v_FT.Append( dwSubIndex );
	v_FT.Append( (int)eQuestType );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB,
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnCheatingQuest( User *pUser, DWORD dwMainIndex, DWORD dwSubIndex )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;
	
	if( pUser->GetPublicIP() == NULL )
		return;

	const int iQueryID = 3002;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_QUEST );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPrivateID().c_str(), pUser->GetPrivateID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( dwMainIndex );
	v_FT.Append( dwSubIndex );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB,
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertRecordInfo( User *pUser, int iPlayTime, RecordTypes eRecordType )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	//char str_query[MAX_PATH] = "";
	//sprintf_s( str_query, "exec log_data_pcroom_add %d, '%s', %d, '%s', %d, %d, %d", 
	//	pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iPlayTime, pUser->GetPublicIP(), pUser->GetPCRoomNumber(), (int)ePCRoomType, (int)ePCRoomSubType );

	const int iQueryID = 1015;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_PCROOM );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( iPlayTime );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( pUser->GetPCRoomNumber() );
	v_FT.Append( (int) eRecordType );
	v_FT.Append( (int) 0 );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);

	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	SendMessage( kPacket );
}

void LogDBClient::OnInsertPCInfo( User *pUser, const ioHashString &rsOS, const ioHashString &rsWebBrowser, const ioHashString &rsDXVersion, const ioHashString &rsCPU, const ioHashString &rsGPU, int iMemory
	                            , const ioHashString &rsDesktopWH, const ioHashString &rsGameWH, bool bFullScreen, 	const ioHashString &rsHddSerialString )	// 2019-03-05 by bckim, 유저 UAC rsHddSerialString 추가
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	enum { MAX_DB_FIELD_SIZE = 90, MAX_SMALL_DB_FIELD_SIZE = 20, };
	char szOS[MAX_PATH]="";
	char szWebBrowser[MAX_PATH]="";
	char szDXVersion[MAX_PATH]="";
	char szCPU[MAX_PATH]="";
	char szGPU[MAX_PATH]="";
	char szDesktopWH[MAX_SMALL_DB_FIELD_SIZE]="";
	char szGameWH[MAX_SMALL_DB_FIELD_SIZE]="";
	char szHddSerialString[MAX_PATH]="";
	
	StringCbCopyN( szOS, sizeof( szOS ), rsOS.c_str(), MAX_DB_FIELD_SIZE );
	StringCbCopyN( szWebBrowser, sizeof( szWebBrowser ), rsWebBrowser.c_str(), MAX_DB_FIELD_SIZE );
	StringCbCopyN( szDXVersion, sizeof( szDXVersion ), rsDXVersion.c_str(), MAX_DB_FIELD_SIZE );
	StringCbCopyN( szCPU, sizeof( szCPU ), rsCPU.c_str(), MAX_DB_FIELD_SIZE );
	StringCbCopyN( szGPU, sizeof( szGPU ), rsGPU.c_str(), MAX_DB_FIELD_SIZE );
	StringCbCopyN( szDesktopWH, sizeof( szDesktopWH ), rsDesktopWH.c_str(), MAX_DB_FIELD_SIZE );
	StringCbCopyN( szGameWH, sizeof( szGameWH ), rsGameWH.c_str(), MAX_DB_FIELD_SIZE );

	StringCbCopyN( szHddSerialString, sizeof( szHddSerialString ), rsHddSerialString.c_str(), MAX_DB_FIELD_SIZE );	// 2019-03-05 by bckim, 유저 UAC rsHddSerialString 추가

	//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s:%s:%s:%s:%s:%d:%s:%s:%d", __FUNCTION__, rsOS.c_str(), rsWebBrowser.c_str(), rsDXVersion.c_str(), rsCPU.c_str(), rsGPU.c_str(), iMemory, szDesktopWH, szGameWH, bFullScreen );

	//char str_query[MAX_PATH*2] = "";
	//sprintf_s( str_query, "exec log_data_localinfo_add %d, '%s', %d, '%s', '%s', '%s', '%s', '%s', %d, '%s'", 
	//pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetGradeLevel(), szOS, szWebBrowser, szDXVersion, szCPU, szGPU, iMemory, pUser->GetPublicIP() );

	char szMemory[512] = "";
	sprintf_s( szMemory, "%d", iMemory );

	const int iQueryID = 1016;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_LOCALINFO );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( szOS, strlen(szOS), TRUE );
	v_FT.Append( szWebBrowser, strlen(szWebBrowser), TRUE);
	v_FT.Append( szDXVersion, strlen(szDXVersion), TRUE);
	v_FT.Append( szCPU, strlen(szCPU), TRUE);
	v_FT.Append( szGPU, strlen(szGPU), TRUE);
	v_FT.Append( szMemory, strlen(szMemory), TRUE);
	v_FT.Append( szDesktopWH, strlen(szDesktopWH), TRUE);
	v_FT.Append( szGameWH, strlen(szGameWH), TRUE);
	v_FT.Append( (uint8)bFullScreen );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE);
	v_FT.Append( szHddSerialString, strlen(szHddSerialString), TRUE);	// 2019-03-05 by bckim, 유저 UAC rsHddSerialString 추가


	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertAlchemicFunc( User *pUser, short iFuncType, int iFuncCode, BYTE iResultType,
										int iUseCnt1, int iUseCnt2, int iResult1, int iResult2 )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	enum { NOTE_SIZE = 100, };
	char szNote[NOTE_SIZE]="";

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_piece_mix_add %d,'%s',%d,%d,%d,%d,%d,%d,%d,%d"
	//	         , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetGradeLevel(), iFuncType, iFuncCode, iResultType
	//	         , iUseCnt1, iUseCnt2, iResult1, iResult2 );

	const int iQueryID = 1017;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_PIECE_MIX );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iFuncType);
	v_FT.Append( iFuncCode );
	v_FT.Append( iResultType );
	v_FT.Append( iUseCnt1 );
	v_FT.Append( iUseCnt2 );
	v_FT.Append( iResult1 );
	v_FT.Append( iResult2 );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertDisassemble( User *pUser, int iType, int iCode, int iResultCode, int iResultCnt )
{
	if( pUser == NULL )
		return;

	if( pUser->GetUserIndex() == 0 )
		return;

	if( pUser->GetPublicID().IsEmpty() )
		return;

	if( pUser->GetPublicIP() == NULL )
		return;

	enum { NOTE_SIZE = 100, };
	char szNote[NOTE_SIZE]="";

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_piece_divide_add %d,'%s',%d,%d,%d,%d,%d"
	//	         , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetGradeLevel()
	//	         , iType, iCode, iResultCnt, iResultCode );

	const int iQueryID = 1018;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_PIECE_DIVIDE );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iType);
	v_FT.Append( iCode );
	v_FT.Append( iResultCnt );
	v_FT.Append( iResultCode );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertAddAlchemicPiece( int iUserIndex, const ioHashString &rszPublicID, int iLevel, int iPlayTime, BYTE iDifficulty, int iCnt )
{
	if( iUserIndex == 0 )
		return;

	if( rszPublicID.IsEmpty() )
		return;

	enum { NOTE_SIZE = 100, };
	char szNote[NOTE_SIZE]="";

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_data_piece_obtain_add %d,'%s',%d,%d,%d,%d"
	//	         , iUserIndex, rszPublicID.c_str(), iLevel
	//	         , iPlayTime, iDifficulty, iCnt );

	const int iQueryID = 1019;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_PIECE_OBTAIN );
	v_FT.Append( iUserIndex );
	v_FT.Append( rszPublicID.c_str(), rszPublicID.Length(), TRUE );
	v_FT.Append( iLevel );
	v_FT.Append( iPlayTime);
	v_FT.Append( iDifficulty );
	v_FT.Append( iCnt );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertCloverInfo( const int iUserIndex, const int iFriendIndex, const BYTE byCloverType, const int iCount )
{
	if( iUserIndex == 0 )
		return;
	
	const int iQueryID = 2051;	// log_data_clover_add INT INT INT8 INT

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_CLOVER );
	v_FT.Append( iUserIndex );
	v_FT.Append( iFriendIndex );
	v_FT.Append( byCloverType );
	v_FT.Append( iCount);

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write(query_data) );

	SendMessage( kPacket );
}

void LogDBClient::OnInsertBingoChoiceNumber( const int iUserIndex, const ioHashString& szPublicID, const BYTE byChoiceType, const int iSelectNumber, const BYTE byState )
{
	if( iUserIndex == 0 )
		return;
	
	const int iQueryID = 2076;	// log_event_bingo_add INT STR INT8 INT8 INT8

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_BEGIN );
	v_FT.Append( iUserIndex );
	v_FT.Append( szPublicID.c_str(), szPublicID.Length(), TRUE );
	v_FT.Append( byChoiceType );
	v_FT.Append( static_cast< BYTE >( iSelectNumber ) );
	v_FT.Append( byState);

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::GenerateLOGDBAgentInfo()
{
	int iIndex = g_ServerNodeManager.GetServerIndex() % m_vServerIP.size();

	m_DBAgentIP		= m_vServerIP[iIndex].c_str();
	m_iDBAgentPort	= m_vServerPort[iIndex];
}

void LogDBClient::SetLOGDBAgentInfo(std::vector<std::string>& vServerIP, std::vector<int>& vServerPort)
{
	m_vServerIP		= vServerIP;
	m_vServerPort	= vServerPort;
}

void LogDBClient::OnInsertEventCash( User *pUser, DWORD dwEtcItemType, int iAddCash, EventCashType eEventCashType )
{
	if( pUser == NULL )
		return;

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_event_cash_add %d,'%s',%d,%d,%d,%d,'%s'"
	//	             , pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetGradeLevel(), dwEtcItemType, iAddCash, (int)eEventCashType, pUser->GetPublicIP() );

	const int iQueryID = 1020;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_BEGIN );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( dwEtcItemType);
	v_FT.Append( iAddCash );
	v_FT.Append( (uint8)eEventCashType );
	v_FT.Append(  pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

void LogDBClient::OnInsertSubscription( User *pUser, int iIndex, short iPresentType, int iValue1, int iValue2, int iSubscriptionGold, ioHashString szSubscriptionID, DWORD dwLimitDate, SubscriptionState eStateType )
{
	if( pUser == NULL )
		return;

	enum { NOTE_SISZE = 100, };
	char szNote[NOTE_SISZE]="";
	StringCbPrintf( szNote, sizeof( szNote ), "Index : %d", iIndex );

	//char str_query[MAX_PATH] = "";
	//StringCbPrintf(str_query, sizeof(str_query), "exec log_item_sbox_add %d,'%s',%d,%d,%d,%d,%d,%d,'%s',%d,'%s',%s,%d",
	//				 pUser->GetUserIndex(), pUser->GetPublicID().c_str(), pUser->GetGradeLevel(), iPresentType, iValue1, iValue2,
	//				 iSubscriptionGold, pUser->GetChannelingType(), szSubscriptionID.c_str(), (int)eStateType,
	//			     pUser->GetPublicIP().c_str(), szShortNote, dwLimitDate );

	// 새로운 ID 필요
	const int iQueryID = 1022;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_SUBSCRIPTION );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iPresentType );
	v_FT.Append( iValue1 );
	v_FT.Append( iValue2 );
	v_FT.Append( iSubscriptionGold );
	v_FT.Append( (int16)(pUser->GetChannelingType()) );
	v_FT.Append( szSubscriptionID.c_str(), szSubscriptionID.Length(), TRUE );
	v_FT.Append( (int)eStateType );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( szNote, sizeof(szNote), TRUE );
	v_FT.Append( (uint32)dwLimitDate );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	kPacket << query_data;

	SendMessage( kPacket );
}

//pet Log
void LogDBClient::OnInsertPetLog( User *pUser, const int& iPetIndex, const int& iPetCode, const int& iPetRank, const int& iPetLevel, const int& iPetExp, const int& iCode, PetDataType ePetDataType, int iSubInfo )
{
	if( pUser == NULL )
		return;

	const int iQueryID = 1030;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	BYTE bRank = iPetRank;
	BYTE bLogType = ePetDataType;

	v_FT.Begin( WE_LOG_DATA_PET );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iPetIndex );
	v_FT.Append( iPetCode );
	v_FT.Append( bRank );
	v_FT.Append( iPetLevel );
	v_FT.Append( iPetExp );
	v_FT.Append( iCode );
	v_FT.Append( bLogType );
	v_FT.Append( iSubInfo );
	v_FT.Append( NULL );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket(DTPK_QUERY);
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );

	SendMessage( kPacket );
}

void LogDBClient::OnInsertMaterialCompound( User *pUser, const int& iExtraItemCode, const int& iBeforeReinforce, const int& iNowReinforce, const int& iMaterialCode, MaterialCompoundResult eCompoundResult )
{
	if( pUser == NULL )
		return;

	const int iQueryID = 1040;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	BYTE bBeforeReinforce = iBeforeReinforce;
	BYTE bNowReinforce = iNowReinforce;

	v_FT.Begin( WE_LOG_DATA_UPGRADE ); // 이부분
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iExtraItemCode );
	v_FT.Append( bBeforeReinforce );
	v_FT.Append( bNowReinforce );
	v_FT.Append( iMaterialCode );
	v_FT.Append( eCompoundResult );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );

	SendMessage( kPacket );
}

void LogDBClient::OnInsertCharAwakeInfo( User *pUser, const int& iSoldierCode, const int& iMaterialCode, const short& iUseMaterialCount, BYTE chAwakeType )
{
	if( pUser == NULL )
		return;

	const int iQueryID = 1050;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_CHAR_AWAKE ); // 이부분
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iSoldierCode );
	v_FT.Append( iMaterialCode );
	v_FT.Append( iUseMaterialCount );
	v_FT.Append( chAwakeType );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

void LogDBClient::OnInsertCostumeInfo( User *pUser, const int& iCostumeCode, int iCount, int iEventType )
{
	//log_item_costume_add

	if( pUser == NULL )
		return;

	const int iQueryID = 1060;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_COSTUME );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iCostumeCode );
	v_FT.Append( iEventType );
	v_FT.Append( iCount );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( NULL );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

void LogDBClient::OnInsertAccessoryInfo( User *pUser, const int& iAccessoryCode, int iCount, int iEventType )
{
	//log_item_accessory_add

	if( pUser == NULL )
		return;

	const int iQueryID = 1065;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_ACCESSORY );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iAccessoryCode );
	v_FT.Append( iEventType );
	v_FT.Append( iCount );
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( NULL );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
void LogDBClient::OnInsertCatdMatchingInfo( User *pUser,int iLogType,BYTE M_Type,BYTE M_Mark1,BYTE M_Mark2,BYTE Mark1,BYTE Mark2, int iRewardStep, int iType, DWORD dwValue1, DWORD dwValue2 )
{
	/*
	2203=log_data_cardmatching_add INT STR INT INT INT INT INT INT INT INT INT INT INT
	SELECT TOP 1000 [idx],[AccountIDX],[NickName],[UserLevel],[MissionType],[MissionMark1],[MissionMark2],[UserMark1],[UserMark2],[RewardStep],[ItemType],[ItemCode]
	,[ItemValue],[RegDate]	FROM [LosaLogData].[dbo].[log_data_cardmatching]
	*/
	
	if( pUser == NULL )
		return;

	const int iQueryID = 2203;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_CARDMATCHING );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( (int)iLogType );
	v_FT.Append((int)M_Type );		// Missiontype 
	v_FT.Append((int)M_Mark1 );		// MissionMark1
	v_FT.Append((int)M_Mark2 );		// MissionMark2
	v_FT.Append( (int)Mark1 );		// Mark1
	v_FT.Append( (int)Mark2 );		// Mark2
	v_FT.Append( iRewardStep );
	v_FT.Append( iType );
	v_FT.Append( dwValue1 );
	v_FT.Append( dwValue2 );	

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}
// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가

void LogDBClient::OnInsertOakbarrelInfo( User *pUser, int iFlag, BYTE byStep, int iType, DWORD dwValue1, DWORD dwValue2 )
{
	//log_data_oakbarrel_add

	if( pUser == NULL )
		return;

	const int iQueryID = 1066;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_OAKBARREL );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iType );
	v_FT.Append( dwValue1 );
	v_FT.Append( dwValue2 );
	v_FT.Append( (int)byStep );
	v_FT.Append( iFlag );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

// 2018-01-15 by bckim, 탐사 확장 - 로그 
void LogDBClient::OnInsertExcavatingLogInfo( int iLogType, User *pUser, const CoordinateInfo *istInfo, const UserExcavationRewardInfo *istRewardInfo )
{
	if( pUser == NULL )
		return;
	/*
	LNET_START_DIGGING			= 1,		탐사 땅파기시작 
	LNET_DIGGING_SUCCESS		= 2,		땅파기 결과 성공
	LNET_DIGGING_REAPPRAISAL	= 3,		땅파기 결과 성공 /  재감정 
	LNET_DIGGING_FAIL			= 4,		땅파기 결과 실패 
	LNET_EXCAVATION_RESULT		= 5,		탐사 성공시 결과 및 보상 
	LNET_EXCAVATION_END			= 6,		보상 지급 
	LNET_RECHARGE_SHOVEL		= 7,		
	LNET_LEVELUP_RECHARGE_SHOVEL= 8,
	*/

	if ( iLogType == 0
		//|| iLogType == LNET_START_DIGGING 
		|| iLogType == LNET_DIGGING_SUCCESS 
		|| iLogType == LNET_DIGGING_REAPPRAISAL 
		|| iLogType == LNET_DIGGING_FAIL 
		|| iLogType == LNET_EXCAVATION_RESULT 
		//|| iLogType == LNET_EXCAVATION_END 
		|| iLogType == LNET_RECHARGE_SHOVEL 
		//|| iLogType == LNET_LEVELUP_RECHARGE_SHOVEL
		)	
	{
		return;
	}


	const int iQueryID = 2122;
	//	2122=log_Excavating_log_add INT INT STR STR INT INT INT INT INT INT INT INT INT INT INT INT INT INT INT64 STR
	/*
	INT		LOG_TYPE			
	IINT		ACCOUNT_IDX
	VARCHAR(20)	USER_ID
	VARCHAR(20)	USER_NAME
	INT		USER_EX_LEVEL

	INT		USER_EX_EXP
	INT		CLASS_TYPE
	INT		CLASS_LEVEL
	INT		CLASS_EXPERT
	INT		REMAIN_COUNT

	INT		MAP_ID
	INT		X
	INT		Y
	INT		Z
	INT		REWARD_TYPE

	INT		INDEX
	INT		PRICE
	INT		MULIPLE
	INT64		MONEY_RESULT
	VARCHER(50)	USER_IP
	*/

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_GAME_LOG );			
	v_FT.Append( iLogType );															//	logtype
	v_FT.Append( pUser->GetUserIndex() );												//	account IDX 
	v_FT.Append( pUser->GetPrivateID().c_str(), pUser->GetPrivateID().Length(), TRUE );	//	USER ID 
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );	// User Name 
	v_FT.Append( pUser->GetExcavationLevel());											// 탐사 레벨
	v_FT.Append( pUser->GetExcavationExp());											// 탐사 경험치
	v_FT.Append( pUser->GetSelectClassType());											// 클래스 타입

	int iClassType = pUser->GetSelectClassType();
	int iClassLevel = pUser->GetClassExpert()->GetClassLevel(iClassType);
	int iClassExp =  pUser->GetClassExpert()->GetClassExp(iClassType);
	v_FT.Append(iClassLevel);															// 클래스 레벨 
	v_FT.Append(iClassExp);																// 클래스 경험치 

	int iRemaind_shovel_count = pUser->GetUserEtcItem()->GetExcavationKitCount();
	v_FT.Append(iRemaind_shovel_count);															// 최종 남아있는 삽 개수 

	int mapID = pUser->GetUserExcavation()->GetCurMapID();									// 현재 맵 정보 
	v_FT.Append(mapID);												

	CoordinateInfo stInfo;			// 현재 탐사하려는 탐사 정보 
	if( NULL != istInfo)
	{
		memcpy(&stInfo,istInfo,sizeof(CoordinateInfo));
	}
	v_FT.Append(stInfo.iX);	
	v_FT.Append(stInfo.iY);	
	v_FT.Append(stInfo.iZ);	

	UserExcavationRewardInfo stRewardInfo;		// 보상 정보 확인 
	stRewardInfo.m_iMultiple = 0;				// 초기값이 1 이여서 0으로 
	if( NULL != istRewardInfo)
	{
		memcpy(&stRewardInfo,istRewardInfo,sizeof(UserExcavationRewardInfo));
	}
	v_FT.Append(stRewardInfo.m_iRewardType);	
	v_FT.Append(stRewardInfo.m_iIndex);
	v_FT.Append(stRewardInfo.m_iPrice);	
	v_FT.Append(stRewardInfo.m_iMultiple);			
	
	__int64 money = pUser->GetMoney();
	v_FT.Append(money);		// 최종 결과 페소 

	ioHashString szIP = pUser->GetPublicIP();
	v_FT.Append( szIP.c_str(), szIP.Length(), TRUE );				// 아이피 정보 

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );	
}
// End. 2018-01-15 by bckim, 탐사 확장 - 로그 


// 2018-08-30 by bckim, 주사위 이벤트 추가 - 로그 
void LogDBClient::OnInsertDiceGameLogInfo( int iLogType, User *pUser,const int iGameState)
{
	if( pUser == NULL )
		return;

	DiceGame* pDiceGame = pUser->GetUserDiceGame();
	if( pDiceGame == NULL )
		return;

	const int iQueryID = 2206;
	//	2206=log_data_DiceGame_update INT INT INT INT INT INT INT INT INT INT INT INT INT8 INT INT INT INT INT INT INT INT INT INT
	/*
	[AccountIDX] [int] NOT NULL,
	[GameState]	[int] NOT NULL,
	[DiceCountUsed]	[int] NOT NULL,
	[BoardCountUsed]	[int] NOT NULL,
	[RewardCountUsed]	[int] NOT NULL,
	[Position] [int] NOT NULL,
	....
	*/

	int iBoardCountUsed = 0;
	int iRewardCountUsed = 0;

	switch(iLogType)
	{
	case LNET_DICEGAME_USE_ITEM_BOARD:
		iBoardCountUsed = 1;
		break;
	case LNET_DICEGAME_USE_ITEM_REWARD:
		iRewardCountUsed = 1;
		break;
	}

	int iCountUsed = pDiceGame->GetDiceCountUsed();
	pDiceGame->InitDiceCountUsed();
	int ArrayTraceInfo[DICE_GAME_TRACE_DB] = {0,};
	pDiceGame->AllGetDiceGameTrace(ArrayTraceInfo);
	int ArrayRewardIndex[DICE_GAME_REWARD_DB] = {0,};
	pDiceGame->AllGetRewardIndex(ArrayRewardIndex);

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;
	v_FT.Begin( WE_LOG_GAME_LOG );			
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( iGameState );				// 게임 state				// 진행중 0 : 완료시 1 
	v_FT.Append( iCountUsed );				// 주사위 던진 개수			// 최종 저장하나 난 뒤 추가값 
	v_FT.Append( iBoardCountUsed );			// 보드 변경 아이템			// 무조건 1 증가 
	v_FT.Append( iRewardCountUsed );		// 보상 변경 아이템			// 무조건 1 증가 
	v_FT.Append( pDiceGame->GetDiceGamePosition() );
	for( int i = 0; i < DICE_GAME_TRACE_DB; ++i )
		v_FT.Append( ArrayTraceInfo[i] );	
	v_FT.Append( pDiceGame->GetBoradIndex());		
	for( int i = 0; i < DICE_GAME_REWARD_DB; ++i )			
		v_FT.Append( ArrayRewardIndex[i] );	

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );	
}
// End. 2018-08-30 by bckim, 주사위 이벤트 추가 - 로그 

void LogDBClient::OnInsertGameLogInfo( int iLogType, User *pUser, int iTableIndex, int iCode, BYTE byEventType, int iParam1, int iParam2, int iParam3, int iParam4, char* szParam5 )
{
	//log_gameLog_set_log
	if( pUser == NULL )
		return;

	if( iLogType == GLT_PESO_GAIN || iLogType == GLT_PESO_CONSUME )
	{
		if( iParam4 == 0 )
			return;
	}

	const int iQueryID = 1070;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_GAME_LOG );
	v_FT.Append( iLogType );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPrivateID().c_str(), pUser->GetPrivateID().Length(), TRUE );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( pUser->GetGradeExpert());
	v_FT.Append( pUser->GetChannelingType() );
	v_FT.Append( iTableIndex );
	v_FT.Append( iCode );
	v_FT.Append( byEventType );
	v_FT.Append( iParam1 );
	v_FT.Append( iParam2 );
	v_FT.Append( iParam3 );
	v_FT.Append( iParam4 );

	ioHashString szParam ;
	if( szParam5 )
		szParam = szParam5;
	else
		szParam = "0";

	v_FT.Append( szParam.c_str(), szParam.Length(), TRUE );

	ioHashString szIP = pUser->GetPublicIP();
	v_FT.Append( szIP.c_str(), szIP.Length(), TRUE );
	
	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

void LogDBClient::OnInsertUsageOfBonusCash( User *pUser, int iBonusCashIndex, int iAmount, int iType, int iCode, int iValue, const ioHashString &szBillingGUID ,int iStatus)
{
	//log_item_bonus_gold_set_add

	if( pUser == NULL )
		return;

	const int iQueryID = 2108;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;
	
	v_FT.Begin( WE_LOG_DATA_COSTUME );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( iBonusCashIndex );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iAmount );
	v_FT.Append( iType );
	v_FT.Append( iCode );
	v_FT.Append( iValue );
	v_FT.Append( (short)pUser->GetChannelingType() );
	v_FT.Append( (BYTE)1 );  //Shop Type 1:게임 / 2: 웹..
	v_FT.Append( pUser->GetPublicIP(), strlen(pUser->GetPublicIP()), TRUE );
	v_FT.Append( szBillingGUID.c_str(),szBillingGUID.Length(), TRUE );
	v_FT.Append( iStatus );
	
	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

void LogDBClient::OnInsertMatchInfo( User *pUser, int iStartTier, int iEndTier, int iDelayTime, int iMatchPoint, int iMaxWinCount, MatchTypes eType, int iRoomIndex, 
	int	iRevenge )
{
	//log_data_matchmode2_add

	if( pUser == NULL )
		return;

	const int iQueryID = 2109;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;
	
	v_FT.Begin( WE_LOG_DATA_MATCH );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE  );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( (int)iStartTier );
	v_FT.Append( (int)iEndTier );
	v_FT.Append( iDelayTime );
	v_FT.Append( iMatchPoint );
	v_FT.Append( iMaxWinCount );
	v_FT.Append( (int)eType );
	v_FT.Append( iRoomIndex );
	v_FT.Append( iRevenge );
	
	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

void LogDBClient::OnInsertPrisonerRoomLog( const uint64 iServerID, int iEnterRoom, int iMakeRoom, int iChoiceRoom )
{
	//log_trace_prisoner_start_add
	const int iQueryID = 2110;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_PRISONERROOM );
	v_FT.Append( iServerID );
	v_FT.Append( iMakeRoom );
	v_FT.Append( iEnterRoom );
	v_FT.Append( iChoiceRoom );
	
	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

void LogDBClient::OnInsertEnterBattleRoomLog( int iRoomNumber, int iTeamBlue, int iTeamRed, int iTeamMaxBlue, int iTeamMaxRed, int iRandomTeamMode, int iclosedRoomOption, int iUserModeOption )
{
	//log_trace_prisoner_option_add
	const int iQueryID = 2111;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_PLAYROOM );
	v_FT.Append( iRoomNumber );
	v_FT.Append( iTeamBlue );
	v_FT.Append( iTeamRed );
	v_FT.Append( iTeamMaxBlue );
	v_FT.Append( iTeamMaxRed );
	v_FT.Append( iRandomTeamMode );
	v_FT.Append( iclosedRoomOption );
	v_FT.Append( iUserModeOption );

	CQueryData query_data;
	query_data.SetData( 
	g_ServerNodeManager.GetServerIndex(),
	_RESULT_DESTROY, 
	LOGDBAGENT_SET, 
	_INSERTDB, 
	iQueryID,
	v_FT,
	v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );	
}

void LogDBClient::OnInsertPieceToPeso( User *pUser, int iPieceCount, int iAddictivePieceCount, int iGetPeso )
{
	// 2115=log_data_spirit_peso_add INT STR INT INT INT INT
	if( pUser == NULL )
		return;

	const int iQueryID = 2115;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_SPIRIT );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iPieceCount );
	v_FT.Append( iAddictivePieceCount );
	v_FT.Append( iGetPeso );

	CQueryData query_data;
	query_data.SetData( g_ServerNodeManager.GetServerIndex(), _RESULT_DESTROY, LOGDBAGENT_SET, _INSERTDB, iQueryID, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );	
}

void LogDBClient::OnInsertObtainSpirit( User *pUser, int iSpiritCode, int iSpiritQuantity, int iValue, BYTE byLogType )
{
	// 2116=log_data_spirit_obtain_add INT STR INT INT INT INT INT8
	// byLogType == 1 이면
	// iValue = 모드타입
	// byLogType == 2 이면
	// iValue = 획득페소

	if( pUser == NULL )
		return;

	const int iQueryID = 2116;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_SPIRIT );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iSpiritCode );
	v_FT.Append( iSpiritQuantity );
	v_FT.Append( iValue );
	v_FT.Append( byLogType );

	CQueryData query_data;
	query_data.SetData( g_ServerNodeManager.GetServerIndex(), _RESULT_DESTROY, LOGDBAGENT_SET, _INSERTDB, iQueryID, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );	
}

void LogDBClient::OnInsertComposeSpirit( User *pUser, int iClassType, int iSpiritCode, int iSpiritQuantity, int iSpecialSpiritCode, int iSpecialSpiritQuantity )
{
	// 2117=log_data_spirit_mix_add INT STR INT INT INT INT INT INT

	if( pUser == NULL )
		return;

	const int iQueryID = 2117;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_SPIRIT );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iClassType );
	v_FT.Append( iSpiritCode );
	v_FT.Append( iSpiritQuantity );
	v_FT.Append( iSpecialSpiritCode );
	v_FT.Append( iSpecialSpiritQuantity );

	CQueryData query_data;
	query_data.SetData( g_ServerNodeManager.GetServerIndex(), _RESULT_DESTROY, LOGDBAGENT_SET, _INSERTDB, iQueryID, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );	
}

void LogDBClient::OnInsertDecomposeSpirit( User *pUser, int iClassType, int iSpiritCode, int iSpiritCount, int iSoulStoneCount, BYTE byCritical )
{
	// 2118=log_data_spirit_divide_add INT STR INT INT INT INT INT INT8

	if( pUser == NULL )
		return;

	const int iQueryID = 2118;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_SPIRIT );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iClassType );
	v_FT.Append( iSpiritCode );
	v_FT.Append( iSpiritCount );
	v_FT.Append( iSoulStoneCount );
	v_FT.Append( byCritical );

	CQueryData query_data;
	query_data.SetData( g_ServerNodeManager.GetServerIndex(), _RESULT_DESTROY, LOGDBAGENT_SET, _INSERTDB, iQueryID, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );	
}

void LogDBClient::OnInsertConversionSpirit( User *pUser, int iUseSpiritCode, int iUseSpiritCount, int iGetSpiritCode, int iGetSpiritCount, BYTE byCritical )
{
	// 2119=log_data_spirit_change_add INT STR INT INT INT INT INT INT8

	if( pUser == NULL )
		return;

	const int iQueryID = 2119;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_SPIRIT );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iUseSpiritCode );
	v_FT.Append( iUseSpiritCount );
	v_FT.Append( iGetSpiritCode );
	v_FT.Append( iGetSpiritCount );
	v_FT.Append( byCritical );

	CQueryData query_data;
	query_data.SetData( g_ServerNodeManager.GetServerIndex(), _RESULT_DESTROY, LOGDBAGENT_SET, _INSERTDB, iQueryID, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );	
}

void LogDBClient::OnInsertCustomMedalInfo( User *pUser, int iMedalIndex, int iMedalCode, int iStat1, int iStat2, int iStat3, int iStat4,
	int iStat5, int iStat6, int iStat7, int iStat8, int iLimitType, SYSTEMTIME sysTime, MedalType iLogType )
{
	// 2120=og_data_custom_medal_add INT INT INT INT INT INT INT INT INT INT INT INT DATETIME TINYINT  

	if( pUser == NULL )
		return;

	const int iQueryID = 2120;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_SPIRIT );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( iMedalIndex );
	v_FT.Append( iMedalCode );
	v_FT.Append( iStat1 );
	v_FT.Append( iStat2 );
	v_FT.Append( iStat3 );
	v_FT.Append( iStat4 );
	v_FT.Append( iStat5 );
	v_FT.Append( iStat6 );
	v_FT.Append( iStat7 );
	v_FT.Append( iStat8 );
	v_FT.Append( iLimitType );
	v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
	v_FT.Append( (int)iLogType );

	CQueryData query_data;
	query_data.SetData( g_ServerNodeManager.GetServerIndex(), _RESULT_DESTROY, LOGDBAGENT_SET, _INSERTDB, iQueryID, v_FT, v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );	
}

// 캐릭터 교체 해킹 로그 관련
// AccountIDX		INT						-- 인덱스
// NickName			NVARCHAR(20)			-- 닉네임
// UserLevel		INT						-- 레벨
// PlayType			INT						-- 3: 광장, 4: 전투, 5: 진영전, 6: 오늘의모드, 7: 랭킹전
// ModeType			INT						-- 1: 파워스톤, 2: 포로탈출, ... , 28: 일대일모드
// SubType1			INT					
// SubType2			INT					
// ItemType			INT						-- 1: 용병, 2: 치장, 3: 특별아이템, 5: 장비, ...
// ItemCode			INT					
// ItemValue		INT			
// SubType1, SubType2 값은 맵종류와 맵Arr
// ItemValue 는 아이템 종류에 따른 값
void LogDBClient::OnInsertHackLogInfo( User *pUser, int playType, int modeType, int subType1, int subType2, int itemType, int itemCode, int itemValue)
{
	//USP_LOG_HACK_SET_ADD
	if( pUser == NULL )
		return;

	const int iQueryID = 1071;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_HACK );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( playType);
	v_FT.Append( modeType );
	v_FT.Append( subType1 );
	v_FT.Append( subType2 );
	v_FT.Append( itemType );
	v_FT.Append( itemCode );
	v_FT.Append( itemValue );
	
	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

void LogDBClient::OnInsertHackLogMessage( User *pUser, const char * szItemName, int iItemReinforce)
{
	//USP_LOG_HACK_MESSAGE_SET_ADD
	if( pUser == NULL )
		return;

	enum { NOTE_SISZE = 200, };
	char szNote[NOTE_SISZE]="";
	StringCbPrintf( szNote, sizeof( szNote ), "[%s]	Reinforce[%d]", szItemName, iItemReinforce );

	const int iQueryID = 1072;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_HACK_MESSAGE );
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( szNote, sizeof(szNote), TRUE);

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );
	SendMessage( kPacket );
}

void LogDBClient::OnInsertAccessoryCompound( User *pUser, const int& iItemCode, const int& iBeforeReinforce, const int& iNowReinforce, const int& iMaterialCode, MaterialCompoundResult eCompoundResult )
{
	if( pUser == NULL )
		return;

	// log_data_accessory_upgrade_add
	const int iQueryID = 2121;

	ioLogSerialize v_FT;
	vVALUETYPE v_VT;

	v_FT.Begin( WE_LOG_DATA_UPGRADE ); // 이부분
	v_FT.Append( pUser->GetUserIndex() );
	v_FT.Append( pUser->GetPublicID().c_str(), pUser->GetPublicID().Length(), TRUE );
	v_FT.Append( pUser->GetGradeLevel() );
	v_FT.Append( iItemCode );
	v_FT.Append( iBeforeReinforce );
	v_FT.Append( iNowReinforce );
	v_FT.Append( iMaterialCode );
	v_FT.Append( eCompoundResult );

	CQueryData query_data;
	query_data.SetData( 
		g_ServerNodeManager.GetServerIndex(),
		_RESULT_DESTROY, 
		LOGDBAGENT_SET, 
		_INSERTDB, 
		iQueryID,
		v_FT,
		v_VT );

	SP2Packet kPacket( DTPK_QUERY );
	PACKET_GUARD_VOID( kPacket.Write( query_data ) );

	SendMessage( kPacket );
}
