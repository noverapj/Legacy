#include "stdafx.h"

#include "ioRankBattleManager.h"

#include "GUI/VoiceChatWnd.h"
#include "GUI/RankBattleWnd.h"

template<> ioRankBattleManager* Singleton< ioRankBattleManager >::ms_Singleton = 0;
ioRankBattleManager::ioRankBattleManager()
{
	m_iRankPoint = 0;
	m_iRankGrade = 0;
	m_iPrevRankGrade = -1;
	m_iTotalWin = 0;
	m_iTatalLose = 0;
	m_iWinRate = 0;
	m_iRank = 0;
	m_bSendRequest = false;
	m_bSendCancel = false;
	m_bRankBattlePlaying = false;
}

ioRankBattleManager::~ioRankBattleManager()
{
}

ioRankBattleManager& ioRankBattleManager::GetSingleton()
{
	return Singleton< ioRankBattleManager >::GetSingleton();
}

void ioRankBattleManager::LoadINIData()
{
	ioINILoader_e kLoader( "config/sp2_RankBattle.ini" );

	char szBuf[MAX_PATH] = "";
	char szTemp[MAX_PATH] = "";

	kLoader.SetTitle_e( "info" );
	int iTimeCount = kLoader.LoadInt_e( "battle_time_count", 0 );
	int iScheduleCount = kLoader.LoadInt_e( "schedule_count", 0 );
	int iRankCount = kLoader.LoadInt_e( "rank_count", 0 );
	m_iHelpTipIdx = kLoader.LoadInt_e( "help_tip_idx", 0 );

	m_TimeMap.clear();
	for( int i = 0; i < iTimeCount; ++i )
	{
		StringCbPrintf_e( szBuf, sizeof(szBuf), "battle_time%d", i + 1 );
		kLoader.SetTitle( szBuf );

		BattleTimeInfo cInfo;
		cInfo.m_iType = kLoader.LoadInt_e( "type", 0 );
		cInfo.m_iStartTime = kLoader.LoadInt_e( "start_time", 0 );
		cInfo.m_iEndTime = kLoader.LoadInt_e( "end_time", 0 );

		m_TimeMap.insert( BattleTimeInfoMap::value_type( cInfo.m_iType, cInfo ) );
	}

	m_Schedules.clear();
	for( int i = 0; i < iScheduleCount; ++i )
	{
		StringCbPrintf_e( szBuf, sizeof(szBuf), "schedule%d", i + 1 );
		kLoader.SetTitle( szBuf );

		BattleScheduleInfo cInfo;
		cInfo.m_iStartDay = kLoader.LoadInt_e( "start_day", 0 );
		cInfo.m_iEndDay = kLoader.LoadInt_e( "end_day", 0 );

		for( int j = 0; j < 7; j++ )
		{
			StringCbPrintf_e( szBuf, sizeof(szBuf), "weekday%d", j + 1 );
			cInfo.m_WeekDayTime[j] = kLoader.LoadInt( szBuf, 0 );
		}
		m_Schedules.push_back( cInfo );
	}

	for( int i = 0; i < iRankCount; ++i )
	{
		StringCbPrintf_e( szBuf, sizeof(szBuf), "rank%d", i + 1 );
		kLoader.SetTitle( szBuf );

		RankInfo cInfo;
		cInfo.m_iMaxPoint = kLoader.LoadInt_e( "max_point", 0 );
		kLoader.LoadString_e( "name", "", szTemp, MAX_PATH );
		cInfo.m_szRankName = szTemp;

		m_RankInfos.push_back( cInfo );
	}

	kLoader.SetTitle_e( "string" );
	kLoader.LoadString_e( "menu_tooltip", "", szBuf, MAX_PATH );
	m_szMenuTooltip = szBuf;

	kLoader.LoadString_e( "info_tooltip", "", szBuf, MAX_PATH );
	m_szInfoTooltip = szBuf;	
}

bool ioRankBattleManager::IsActive()
{
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	if( m_CheckServerTime.wYear == rkServerTime.wYear &&
		m_CheckServerTime.wMonth == rkServerTime.wMonth &&
		m_CheckServerTime.wDay == rkServerTime.wDay &&
		m_CheckServerTime.wHour == rkServerTime.wHour )
		return m_bActive;

	m_CheckServerTime = rkServerTime;

	int iCurDate = ( rkServerTime.wYear - 2000 ) * 10000 + rkServerTime.wMonth * 100 + rkServerTime.wDay;
	int iCurTime = rkServerTime.wHour;
	int iScheduleCount = m_Schedules.size();
	bool bActive = false;
	for( int i = 0; i < iScheduleCount; i++ )
	{
		if( m_Schedules[i].m_iStartDay <= iCurDate && 
			(m_Schedules[i].m_iEndDay >= iCurDate || m_Schedules[i].m_iEndDay == 0) )
		{
			BattleTimeInfoMap::iterator cIter = m_TimeMap.find( m_Schedules[i].m_WeekDayTime[rkServerTime.wDayOfWeek] );
			if( cIter != m_TimeMap.end() )
			{
				BattleTimeInfo cInfo = cIter->second;
				if( cInfo.m_iStartTime <= iCurTime && cInfo.m_iEndTime >= iCurTime )
					bActive = true;
			}
			break;
		}
	}
	m_bActive = bActive;
	return m_bActive;
}

bool ioRankBattleManager::ProcessTCPPacket( SP2Packet &rkPacket )
{
	switch( rkPacket.GetPacketID() )
	{
	case STPK_USER_MATCH_DATA:
		rkPacket >> m_iRankPoint >> m_iTotalWin >> m_iTatalLose >> m_iWinStreakCount >> m_iRank;
		if( m_iTotalWin == 0 && m_iTatalLose == 0 )
			m_iWinRate = 0;
		else
			m_iWinRate = m_iTotalWin * 100 / ( m_iTotalWin + m_iTatalLose );

		SetRankGrade( m_iRankPoint );
		return true;
	case STPK_USER_MATCH_RANKING_DATA:
		SetMyRankData( rkPacket );
		return true;
	case STPK_MATCH_TOP_RANKING_LIST:
		SetUserRankData( rkPacket );
		return true;
	case STPK_USER_MATCH_BATTLE_DATA:
		SetUserBattleData( rkPacket );
		return true;
	case STPK_USER_MATCH_REQUEST:
		SetRequest( rkPacket );
		return true;
	case STPK_USER_MATCH_CANCEL:
		SetCancel( rkPacket );
		return true;
	}
	return false;
}

void ioRankBattleManager::SetMyRankData( SP2Packet &rkPacket )
{
	int iCount = 0;
	rkPacket >> iCount;
	m_MyRecords.clear();
	for( int i = 0; i < iCount; i++ )
	{
		MyBattleRecord cInfo;
		rkPacket >> cInfo.m_iRankPoint >> cInfo.m_iRank >> cInfo.m_dwDate;
		m_MyRecords.push_back( cInfo );
	}
	RankBattleWnd *pRankBattleWnd = dynamic_cast<RankBattleWnd *>( g_GUIMgr.FindWnd(RANK_BATTLE_WND) );
	if( pRankBattleWnd )
		pRankBattleWnd->SetRankGraph();
}

void ioRankBattleManager::SetUserRankData( SP2Packet &rkPacket )
{
	int iCount = 0;
	rkPacket >> iCount;
	m_UserRecords.clear();
	for( int i = 0; i < iCount; i++ )
	{
		UserBattleRecord cInfo;
		rkPacket >> cInfo.m_szName >> cInfo.m_iRank >> cInfo.m_iRankPoint >> cInfo.m_iWin >> cInfo.m_iLose >> cInfo.m_iGradeLevel >> cInfo.m_bLogin;
		m_UserRecords.push_back( cInfo );
	}
	RankBattleWnd *pRankBattleWnd = dynamic_cast<RankBattleWnd *>( g_GUIMgr.FindWnd(RANK_BATTLE_WND) );
	if( pRankBattleWnd )
		pRankBattleWnd->ResizeScroll();
	//std::sort( m_UserRecords.begin(), m_UserRecords.end(), UserRecordSort() );
}

void ioRankBattleManager::SetUserBattleData( SP2Packet &rkPacket )
{
	int iCount = 0;
	rkPacket >> iCount;
	m_History.clear();
	for( int i = 0; i < iCount; i++ )
	{
		BattleHistory cInfo;
		rkPacket >> cInfo.m_byWinState >> cInfo.m_iWinGame >> cInfo.m_iLoseGame;

		m_History.push_back( cInfo );
	}
}

void ioRankBattleManager::SetRequest( SP2Packet &rkPacket )
{
	int iResult = 0;
	rkPacket >> iResult;

	m_bSendRequest = false;
	RankBattleWnd *pRankBattleWnd = dynamic_cast<RankBattleWnd *>( g_GUIMgr.FindWnd(RANK_BATTLE_WND) );
	if( pRankBattleWnd )
		pRankBattleWnd->ResetButtons();
}

void ioRankBattleManager::SetCancel( SP2Packet &rkPacket )
{
	int iResult = 0;
	rkPacket >> iResult;

	switch( iResult )
	{
	case 1:
	case 4:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "매칭에 실패했습니다.#다시 시도해주세요. (%d)", iResult );
		}
		break;
	case 2:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "검색시간이 초과되었습니다.#다시 시도해주세요." );
		}
		break;
	case 3:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "랭킹전은 10시~22시까지 가능합니다." );
		}
		break;
	case 5:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "간편회원은 랭킹전을 할 수 없습니다." );
		}
		break;
	}

	m_bSendCancel = false;
	if( iResult == 0 )
	{
		m_bSendRequest = false;
		RankBattleWnd *pRankBattleWnd = dynamic_cast<RankBattleWnd *>( g_GUIMgr.FindWnd(RANK_BATTLE_WND) );
		if( pRankBattleWnd )
			pRankBattleWnd->ResetButtons();
	}
}

void ioRankBattleManager::OnGameStart( SP2Packet &rkPacket )
{
	m_bSendRequest = false;
	m_bSendCancel = false;
	m_bRankBattlePlaying = true;

	int iModeType = 0;
	int iSubNum = 0;
	int iMapNum = 0;
	rkPacket >> iModeType >> iSubNum >> iMapNum;

	char szCall[MAX_PATH];
	sprintf_s( szCall, "%s - (%d), (%d), (%d)" ,__FUNCTION__, iModeType, iSubNum, iMapNum );

	g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
	g_App.SetModeType( iModeType, iSubNum, iMapNum, PT_NONE );
			
	// voice chat 초기화
	g_VoiceChat.RemoveAllSoundPlayer();
	g_VoiceOffListManager.DeleteAll();
	VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
	if(pWnd)
		pWnd->AllOtherVoiceOff();
}

void ioRankBattleManager::OnGameEnd( SP2Packet &rkPacket )
{
	SP2Packet kPacket( CTPK_EXIT_ROOM );
	kPacket << EXIT_ROOM_LOBBY << 0 << true;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void ioRankBattleManager::SendRankData()
{
	SP2Packet kPacket( CTPK_USER_MATCH_RANKING_DATA );
	TCPNetwork::SendToServer( kPacket );
}

void ioRankBattleManager::SendTop100()
{
	SP2Packet kPacket( CTPK_MATCH_TOP_RANKING_LIST );
	TCPNetwork::SendToServer( kPacket );
}

void ioRankBattleManager::SendHistory( int iPage )
{
	SP2Packet kPacket( CTPK_USER_MATCH_BATTLE_DATA );
	kPacket << iPage;
	TCPNetwork::SendToServer( kPacket );
}

void ioRankBattleManager::SendRequest()
{
	if( m_bSendRequest )
		return;

	m_bSendRequest = true;
	SP2Packet kPacket( CTPK_USER_MATCH_REQUEST );
	TCPNetwork::SendToServer( kPacket );

	RankBattleWnd *pRankBattleWnd = dynamic_cast<RankBattleWnd *>( g_GUIMgr.FindWnd(RANK_BATTLE_WND) );
	if( pRankBattleWnd )
		pRankBattleWnd->ResetButtons();
}

void ioRankBattleManager::SendCancel()
{
	if( !m_bSendRequest || m_bSendCancel )
		return;

	m_bSendCancel = true;
	SP2Packet kPacket( CTPK_USER_MATCH_CANCEL );
	TCPNetwork::SendToServer( kPacket );
}

MyBattleRecord ioRankBattleManager::GetRankRecord( int iIndex )
{
	int iRecordCount = m_MyRecords.size();
	if( iIndex < iRecordCount )
		return m_MyRecords[iIndex];

	static MyBattleRecord kReturn;
	return kReturn;
}

UserBattleRecord ioRankBattleManager::GetUserBattleRecord( int iRank )
{
	int iRecordCount = m_UserRecords.size();
	if( iRank <= iRecordCount )
		return m_UserRecords[iRank - 1];

	static UserBattleRecord kReturn;
	return kReturn;
}

int ioRankBattleManager::GetUserBattleRecordCount()
{
	return m_UserRecords.size();
}

bool ioRankBattleManager::GetBattleHistory( int iIndex, BattleHistory& rBattleHistory )
{
	int iCount = m_History.size();
	if( !COMPARE( iIndex, 0, iCount ) )
		return false;

	rBattleHistory = m_History[iIndex];
	return true;
}

void ioRankBattleManager::SetRankGrade( int iPoint )
{
	int iRankCount = m_RankInfos.size();
	for( int i = 0; i < iRankCount; i++ )
	{
		if( m_RankInfos[i].m_iMaxPoint == 0 || iPoint < m_RankInfos[i].m_iMaxPoint )
		{
			if( m_iPrevRankGrade != -1 )
				m_iPrevRankGrade = m_iRankGrade;
			else
				m_iPrevRankGrade = i;

			m_iRankGrade = i;
			break;
		}
	}
}

ioHashString ioRankBattleManager::GetRankGradeString()
{
	int iRankCount = m_RankInfos.size();
	if( m_iRankGrade < iRankCount )
		return m_RankInfos[m_iRankGrade].m_szRankName;

	return ioHashString();
}