

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "Room.h"
#include "HiddenkingMode.h"

#include "ModeHelp.h"
#include "../EtcHelpFunc.h"
#include "../DataBase/LogDBClient.h"

HiddenkingMode::HiddenkingMode( Room *pCreator ) : Mode( pCreator )
{
	m_iBlueTeamScore  = 0;
	m_iRedTeamScore   = 0;
	m_iTotalBlueScore = 0;
	m_iTotalRedScore = 0;

	m_iNeedPoint = 0;

	m_dwBlueTakeKingTime = 0;
	m_dwRedTakeKingTime  = 0;
	m_dwGivePointTime    = 0;
	m_dwRemainTime = 0;

	m_dwCheckKingPingTime = 0;

	m_fManyPeopleRate = 1.0f;
	m_fDecreaseScoreTimeRate = 0.0f;
	
	m_fRemainTimeRate = 1.0f;
}
HiddenkingMode::~HiddenkingMode()
{
}

void HiddenkingMode::LoadINIValue()
{
	Mode::LoadINIValue();

	m_dwCurRoundDuration = m_dwRoundDuration;
	m_vRoundHistory.clear();

	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );
	rkLoader.SetTitle( "round" );
	m_iNeedPoint = rkLoader.LoadInt( "win_need_point", 1 );
}

void HiddenkingMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();
	m_vWearPosList.clear();
}

void HiddenkingMode::AddNewRecord( User *pUser )
{
	HiddenkingRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	TeamType eKingTeam = TEAM_NONE;
	if( m_dwBlueTakeKingTime != 0 )
		eKingTeam = TEAM_BLUE;
	else if( m_dwRedTakeKingTime != 0 )
		eKingTeam = TEAM_RED;

	UpdateUserRank();
}

void HiddenkingMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	CheckCreateCrown(pUser); // 삭제 전에 실행 

	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
		{
			SetModeEndDBLog( &m_vRecordList[i], iCharCnt, LogDBClient::PRT_EXIT_ROOM );
			m_vRecordList.erase( m_vRecordList.begin() + i );
			break;
		}
	}

	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	}

	if( !m_bRoundSetEnd && !bRoomDestroy )
	{

		TeamType eKingTeam = TEAM_NONE;
		if( m_dwBlueTakeKingTime != 0 )
			eKingTeam = TEAM_BLUE;
		else if( m_dwRedTakeKingTime != 0 )
			eKingTeam = TEAM_RED;

		DWORD dwPrePointTime = m_dwGivePointTime;
		DWORD dwPointTime = GetPointTime(eKingTeam);
		m_dwRemainTime = dwPointTime * ((float)m_dwRemainTime / (float)dwPrePointTime);
		UpdateUserRank();
		SP2Packet kModeInfoPk( STPK_MODE_INFO );
		GetModeInfo( kModeInfoPk );
		SendRoomPlayUser( kModeInfoPk );
	}
}

void HiddenkingMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	Mode::LoadRoundCtrlValue( rkLoader );

	rkLoader.SetTitle( "round" );
	m_dwWinTeamScoreTime = rkLoader.LoadInt( "win_team_score_time", 60000 );
	m_dwLoseTeamScoreTime = rkLoader.LoadInt( "lose_team_score_time", 30000 );
	m_dwDrawScoreTime = rkLoader.LoadInt( "draw_score_time", 45000 );

	m_fManyPeopleRate = rkLoader.LoadFloat( "many_people_extra_rate", 1.0f );
	m_fDecreaseScoreTimeRate = rkLoader.LoadFloat( "team_score_decrease_rate", 0.0f );

	m_fScoreGaugeMaxRate = rkLoader.LoadFloat( "score_gauge_max_rate", 1.0f );
	m_dwScoreGaugeConstValue = rkLoader.LoadInt( "score_gauge_const_value", 210000 );
	m_fScoreGaugeDethTimeRate = rkLoader.LoadFloat( "score_gauge_deth_time_rate", 1.0f );

	m_iCrownRate = rkLoader.LoadInt( "crown_rate", 0 );

	rkLoader.SetTitle( "king_ping_check" );
	m_dwKingPingTime = rkLoader.LoadInt( "king_ping_time", 0 );
	m_iKingPingCnt = rkLoader.LoadInt( "king_ping_cnt", 4 );
}

void HiddenkingMode::ProcessReady()
{
	Mode::ProcessReady();

	m_dwGivePointTime = m_dwDrawScoreTime;
}

void HiddenkingMode::ProcessPlay()
{
	ProcessRevival();
	CheckRoundTimePing();
	CheckFieldItemLiveTime();
	CheckItemSupply( m_dwStateChangeTime );
	CheckBallSupply( m_dwStateChangeTime );
	CheckMachineSupply( m_dwStateChangeTime );
	CheckNeedSendPushStruct();
	CheckGivePoint();
	CheckRoundEnd( true );
	ProcessEvent();
	ProcessKingPing();
	ProcessBonusAlarm();
}

void HiddenkingMode::CheckRoundEnd( bool bProcessCall )
{
	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;

	WinTeamType eWinTeam = WTT_DRAW;
	if( m_iRedTeamScore > m_iBlueTeamScore )
	{
		eWinTeam = WTT_RED_TEAM;
	}
	else if( m_iRedTeamScore < m_iBlueTeamScore )
	{
		eWinTeam = WTT_BLUE_TEAM;
	}

	if( m_dwCurRoundDuration < dwGapTime+1000 )
	{
		if( !m_bZeroHP )
		{
			m_bZeroHP = true;
			m_dwCurRoundDuration = 0;
			m_dwCurSuddenDeathDuration = TIMEGETTIME();
			m_fSuddenDeathBlueCont	   = 0.0f;
			m_fSuddenDeathRedCont	   = 0.0f;

			TeamType eKingTeam = TEAM_NONE;
			if( m_dwBlueTakeKingTime != 0 )
				eKingTeam = TEAM_BLUE;
			else if( m_dwRedTakeKingTime != 0 )
				eKingTeam = TEAM_RED;

			SP2Packet kPacket( STPK_ZERO_HP );
			kPacket << m_dwSuddenDeathTime;
			SendRoomAllUser( kPacket );

			// 0초가되면 시간이 멈춘다.
			int iRecordCnt = GetRecordCnt();
			for( int i=0 ; i<iRecordCnt ; i++ )
			{
				ModeRecord *pRecord = FindModeRecord( i );
				if( !pRecord ) continue;
					
				if( pRecord->pUser )
				{
					if( pRecord->eState != RS_VIEW && pRecord->eState != RS_OBSERVER )
						pRecord->pUser->SetStartTimeLog( TIMEGETTIME() );
				}
			}
		}
	}

	if( m_bZeroHP )
	{
		CheckSuddenDeathEnd();

		// 서든 데스 결과는 양팀 기여도 합
		if( m_bTournamentRoom )
		{
			if( m_fSuddenDeathBlueCont > 0.0f && m_fSuddenDeathRedCont > 0.0f )
			{
				if( eWinTeam == WTT_DRAW )
				{
					if( m_fSuddenDeathBlueCont > m_fSuddenDeathRedCont )
						eWinTeam = WTT_BLUE_TEAM;
					else
						eWinTeam = WTT_RED_TEAM;
				}
			}
		}
	}


	if( bProcessCall && eWinTeam == WTT_DRAW )
		return;

	int iBlueUser = GetCurTeamUserCnt( TEAM_BLUE );
	int iRedUser  = GetCurTeamUserCnt( TEAM_RED );
	if( GetState() != MS_PLAY && GetState() != MS_READY )
	{
		eWinTeam = WTT_DRAW;		
	}
	else if( iBlueUser == 0 )
	{
		eWinTeam = WTT_RED_TEAM;
		SetScore( TEAM_RED, m_bZeroHP );
	}
	else if( iRedUser == 0 )
	{
		eWinTeam = WTT_BLUE_TEAM;
		SetScore( TEAM_BLUE, m_bZeroHP );
	}

	if( eWinTeam == WTT_DRAW )
		return;

	if( m_dwCurRoundDuration < dwGapTime )
		m_dwCurRoundDuration = 0;
	else
		m_dwCurRoundDuration -= dwGapTime;

	SetRoundEndInfo( eWinTeam );
	if( m_bRoundSetEnd )       //세트가 종료되면 세트의 결과를 전송
	{
		if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
			eWinTeam = WTT_RED_TEAM;
		else if( m_iRedTeamWinCnt < m_iBlueTeamWinCnt )
			eWinTeam = WTT_BLUE_TEAM;
		else
			eWinTeam = WTT_DRAW;

		if( m_bTournamentRoom )
		{
			// 서든 데스 모드에서는 나간팀이 진다.
			if( iBlueUser == 0 )
				eWinTeam = WTT_RED_TEAM;
			else
				eWinTeam = WTT_BLUE_TEAM;
		}
	}
	SendRoundResult( eWinTeam );
}

void HiddenkingMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd ) return;

	if( GetTeamUserCnt( TEAM_BLUE ) == 0 ||
		GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		CheckRoundEnd( false );
	}	
}

// 모드 : 09.06.03
/*
void HiddenkingMode::SetRoundEndInfo( WinTeamType eTeam )
{
	m_CurRoundWinTeam = eTeam;
	m_bRoundSetEnd = true;
	m_bCheckContribute = false;
	SetModeState( MS_RESULT_WAIT );

	UpdateRoundRecord();

	m_vPushStructList.clear();
	m_pCreator->DestroyAllFieldItems();

	// PlayingTime Update
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			pRecord->AddPlayingTime();
			pRecord->AddShufflePlayingTime();
			pRecord->AddClassPlayingTime();
			if( pRecord->pUser && pRecord->pUser->GetStartTimeLog() > 0 )
			{
				if( pRecord->eState != RS_VIEW && pRecord->eState != RS_OBSERVER )
					pRecord->AddDeathTime( TIMEGETTIME() - pRecord->pUser->GetStartTimeLog() );
				else
					g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_VIEW );
				pRecord->pUser->SetStartTimeLog(0);
			}
		}
	}

	int HistorySize = m_vRoundHistory.size();
	if( m_iCurRound-1 > HistorySize )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "HiddenkingMode::SetRoundEndInfo() m_iCurRound != m_vRoundHistory.size()" );
	}
	else
	{
		RoundHistory rh;
		if( eTeam == TEAM_RED )
		{
			rh.iBluePoint = 0;
			rh.iRedPoint = 1;
		}
		else if( eTeam == TEAM_BLUE )
		{
			rh.iBluePoint = 1;
			rh.iRedPoint = 0;
		}
		m_vRoundHistory.push_back( rh );
	}

	ClearKing();
}
*/

void HiddenkingMode::SetRoundEndInfo( WinTeamType eTeam )
{
	m_CurRoundWinTeam = eTeam;

	if( GetTeamUserCnt( TEAM_BLUE ) == 0 ||
		GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		m_bRoundSetEnd = true;
	}

	m_bCheckContribute = false;
	m_bCheckAwardChoose = false;
	m_bCheckSuddenDeathContribute = false;
	SetModeState( MS_RESULT_WAIT );

	UpdateRoundRecord();

	if( m_iNeedRound > 0 )
	{
		if( m_iRedTeamWinCnt == m_iNeedRound || m_iBlueTeamWinCnt == m_iNeedRound )
			m_bRoundSetEnd = true;
	}
	

	m_vPushStructList.clear();
	m_vBallStructList.clear();
	m_vMachineStructList.clear();
	m_pCreator->DestroyAllFieldItems();

	// PlayingTime Update
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			pRecord->AddPlayingTime();
			pRecord->AddClassPlayingTime();
			if( pRecord->pUser && pRecord->pUser->GetStartTimeLog() > 0 )
			{
				if( pRecord->eState != RS_VIEW && pRecord->eState != RS_OBSERVER )
					pRecord->AddDeathTime( TIMEGETTIME() - pRecord->pUser->GetStartTimeLog() );
				else
					g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_VIEW );
				pRecord->pUser->SetStartTimeLog(0);
			}
		}
	}
	
	int HistorySize = m_vRoundHistory.size();
	if( m_iCurRound-1 > HistorySize )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "HiddenkingMode::SetRoundEndInfo() m_iCurRound != m_vRoundHistory.size()" );
	}
	else
	{
		RoundHistory rh;
		if( eTeam == TEAM_RED )
		{
			rh.iBluePoint = 0;
			rh.iRedPoint = 1;
		}
		else if( eTeam == TEAM_BLUE )
		{
			rh.iBluePoint = 1;
			rh.iRedPoint = 0;
		}
		m_vRoundHistory.push_back( rh );
	}

	ClearKing();
}

void HiddenkingMode::RestartMode()
{
	Mode::RestartMode();

	// 결과중에 유저들이 전부 나가면 최종 결과 실행.
	if( GetTeamUserCnt( TEAM_BLUE ) == 0 || 
		GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		m_bRoundSetEnd = true;
		m_bCheckContribute = false;
		m_bCheckAwardChoose = false;
		m_bCheckSuddenDeathContribute = false;
		SetModeState( MS_RESULT_WAIT );
		WinTeamType eWinTeam = WTT_DRAW;
		if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
			eWinTeam = WTT_RED_TEAM;
		else if( m_iRedTeamWinCnt < m_iBlueTeamWinCnt )
			eWinTeam = WTT_BLUE_TEAM;
		else
			eWinTeam = WTT_DRAW;
		SendRoundResult( eWinTeam );
		return;
	}

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		HiddenkingRecord &rkRecord = m_vRecordList[i];
		rkRecord.dwPlayingStartTime= 0;
		rkRecord.dwCurDieTime = 0;
		rkRecord.iRevivalCnt = 0;
		rkRecord.dwRevivalGap = (DWORD)GetRevivalGapTime( 0 );
		rkRecord.bCatchState = false;
		rkRecord.bDieState = false;

		if( rkRecord.eState == RS_VIEW )
			rkRecord.eState = RS_PLAY;
	}

	m_CurRoundWinTeam = WTT_NONE;

	m_iBlueTeamScore = 0;
	m_iRedTeamScore  = 0;

	m_dwBlueTakeKingTime = 0;
	m_dwRedTakeKingTime  = 0;
	m_dwRemainTime = 0;
	m_dwCheckKingPingTime = 0;

	m_szNameOfKing.Clear();

	m_pCreator->DestroyAllFieldItems();

	SetModeState( MS_READY );

	SP2Packet kPacket( STPK_ROUND_READY );
	kPacket << m_iCurRound;
	kPacket << m_iBluePosArray;
	kPacket << m_iRedPosArray;
	kPacket << m_dwCurRoundDuration;
	SendRoomPlayUser( kPacket );

	InitObjectGroupList();
}

ModeType HiddenkingMode::GetModeType() const
{
	return MT_KING;
}

void HiddenkingMode::GetModeInfo( SP2Packet &rkPacket )
{
	Mode::GetModeInfo( rkPacket );

	rkPacket << m_iBlueTeamScore;
	rkPacket << m_iRedTeamScore;

	TeamType eKingTeam = TEAM_NONE;
	if( m_dwBlueTakeKingTime != 0 )
		eKingTeam = TEAM_BLUE;
	else if( m_dwRedTakeKingTime != 0 )
		eKingTeam = TEAM_RED;

	rkPacket << eKingTeam;
	rkPacket << GetPointTime( eKingTeam );
	rkPacket << m_dwRemainTime;
	
	GetModeHistory( rkPacket );
}

void HiddenkingMode::GetModeHistory( SP2Packet &rkPacket )
{
	int i = 0;
	int HistorySize = m_vRoundHistory.size();

	if( HistorySize == 0 || m_iCurRound-1 > HistorySize )
	{
		for( i = 0; i < m_iCurRound; i++ )	
		{
			RoundHistory rh;
			rkPacket << rh.iBluePoint << rh.iRedPoint;
		}
	}
	else
	{
		for( i = 0; i < m_iCurRound-1; i++ )	
		{
			RoundHistory rh = m_vRoundHistory[i];
			rkPacket << rh.iBluePoint << rh.iRedPoint;
		}

		if( HistorySize == m_iCurRound )
		{
			RoundHistory rh = m_vRoundHistory[m_iCurRound-1];
			rkPacket << rh.iBluePoint << rh.iRedPoint;
		}
		else
		{
			rkPacket << 0 << 0;
		}
	}
}

void HiddenkingMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck )
{
	HiddenkingRecord *pRecord = FindKingRecord( szName );
	if( pRecord )
	{
		// 레코드 정보 유무
		rkPacket << true;

		int iKillSize = pRecord->iKillInfoMap.size();
		rkPacket << iKillSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_k = pRecord->iKillInfoMap.begin();
		while( iter_k != pRecord->iKillInfoMap.end() )
		{
			rkPacket << iter_k->first;
			rkPacket << iter_k->second;

			++iter_k;
		}
		LOOP_GUARD_CLEAR();

		int iDeathSize = pRecord->iDeathInfoMap.size();
		rkPacket << iDeathSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_d = pRecord->iDeathInfoMap.begin();
		while( iter_d != pRecord->iDeathInfoMap.end() )
		{
			rkPacket << iter_d->first;
			rkPacket << iter_d->second;

			++iter_d;
		}
		LOOP_GUARD_CLEAR();

		if( bDieCheck )
		{
			rkPacket << pRecord->bDieState;
		}
		rkPacket << pRecord->bCatchState;
	}
	else
	{
		// 레코드 정보 유무
		rkPacket << false;
	}
}

int HiddenkingMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* HiddenkingMode::GetModeINIFileName() const
{
	return "config/hiddenkingmode.ini";
}

TeamType HiddenkingMode::GetNextTeamType()
{
	int iRedCnt, iBlueCnt;
	iRedCnt = iBlueCnt = 0;

	int i = 0;
	int iCharCnt = m_vRecordList.size();
	for(int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetTeam() == TEAM_RED )
			iRedCnt++;
		else if( m_vRecordList[i].pUser->GetTeam() == TEAM_BLUE )
			iBlueCnt++;
	}

	if( iRedCnt > iBlueCnt )
		return TEAM_BLUE;
	else if( iBlueCnt > iRedCnt )
		return TEAM_RED;

	// Red == Blue
	if( m_iTotalRedScore > m_iTotalBlueScore )
		return TEAM_BLUE;
	else if( m_iTotalBlueScore > m_iTotalRedScore )
		return TEAM_RED;

	// 평균 레벨이 낮은 팀
	int iRedLevel = 0;
	int iBlueLevel= 0;
	for(int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetTeam() == TEAM_RED )
			iRedLevel += m_vRecordList[i].pUser->GetGradeLevel();
		else if( m_vRecordList[i].pUser->GetTeam() == TEAM_BLUE )
			iBlueLevel += m_vRecordList[i].pUser->GetGradeLevel();
	}
	if( iRedCnt != 0 )
		iRedLevel /= iRedCnt;
	if( iBlueCnt != 0 )
		iBlueLevel /= iBlueCnt;

	if( iRedLevel < iBlueLevel )
		return TEAM_RED;
	else if( iRedLevel > iBlueLevel )
		return TEAM_BLUE;

	int iRandomTeam = rand() % 2;
	if( iRandomTeam > 0 )
		return TEAM_RED;

	return TEAM_BLUE;
}

int HiddenkingMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
{
	ModeRecord *pKickRecord = FindModeRecord( szKickUserName );
	if( !pKickRecord || !pKickRecord->pUser )
		return USER_KICK_VOTE_PROPOSAL_ERROR_7;

	// 인원 체크 
	if( !pKickRecord->pUser->IsObserver() )
	{
		int iAlreadyTeam = GetTeamUserCnt( pKickRecord->pUser->GetTeam() );
		if( iAlreadyTeam < m_KickOutVote.GetKickVoteUserPool() )
			return USER_KICK_VOTE_PROPOSAL_ERROR_12;
	}

	// 라운드 or 시간 체크
	if( IsRoundSetEnd() )
		return USER_KICK_VOTE_PROPOSAL_ERROR_10;

	// 라운드 수 체크			
	if( m_iBlueTeamWinCnt >= m_KickOutVote.GetKickVoteRoundWin() || 
		m_iRedTeamWinCnt >= m_KickOutVote.GetKickVoteRoundWin() )
	{
		return USER_KICK_VOTE_PROPOSAL_ERROR_10;
	}
	return 0;
}

ModeRecord* HiddenkingMode::FindModeRecord( const ioHashString &rkName )
{
	if( rkName.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetPublicID() == rkName )
			return &m_vRecordList[i];
	}

	return NULL;
}

ModeRecord* HiddenkingMode::FindModeRecord( User *pUser )
{
	if( !pUser )	return NULL;

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
			return &m_vRecordList[i];
	}

	return NULL;
}

ModeRecord* HiddenkingMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

HiddenkingRecord* HiddenkingMode::FindKingRecord( const ioHashString &rkName )
{
	return (HiddenkingRecord*)FindModeRecord( rkName );
}

HiddenkingRecord* HiddenkingMode::FindKingRecord( User *pUser )
{
	return (HiddenkingRecord*)FindModeRecord( pUser );
}

void HiddenkingMode::ClearKing()
{
	int iCharCnt = m_vRecordList.size();
	for( int i = 0; i < iCharCnt; i++)
	{
		HiddenkingRecord &rkRecode = m_vRecordList[i];
		if(rkRecode.pUser)
		{
			ioItem *pItem = rkRecode.pUser->ReleaseItem(EQUIP_WEAR);
			SAFEDELETE( pItem );
		}
	}
}

void HiddenkingMode::SetScore(TeamType eTeam, bool bCrown )
{
	if(eTeam == TEAM_BLUE)
	{
		if( bCrown )
		{
			int iAddScore;
			if( m_iCrownRate > 0 )
			{
				iAddScore = GetPlayingUserCnt() / m_iCrownRate;
				float fCheckScore = (float)GetPlayingUserCnt() / m_iCrownRate;
				fCheckScore -= iAddScore;
				if( fCheckScore >= 0.5f )
					iAddScore++;
			}
			else
			{
				iAddScore = 1;
			}

			m_iBlueTeamScore += iAddScore;
			m_iTotalBlueScore += iAddScore;
		}
		else
		{
			m_fRemainTimeRate = (float)m_dwRemainTime / (float)m_dwGivePointTime;
			m_iBlueTeamScore++;
			m_iTotalBlueScore++;
		}
	}
	else if(eTeam == TEAM_RED)
	{
		if( bCrown )
		{
			int iAddScore;
			if( m_iCrownRate > 0 )
			{
				iAddScore = GetPlayingUserCnt() / m_iCrownRate;
				float fCheckScore = (float)GetPlayingUserCnt() / m_iCrownRate;
				fCheckScore -= iAddScore;
				if( fCheckScore >= 0.5f )
					iAddScore++;
			}
			else
			{
				iAddScore = 1;
			}

			m_iRedTeamScore += iAddScore;
			m_iTotalRedScore += iAddScore;
		}
		else
		{
			m_fRemainTimeRate = (float)m_dwRemainTime / (float)m_dwGivePointTime;
			m_iRedTeamScore++;
			m_iTotalRedScore++;
		}
	}
}

void HiddenkingMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "hidden%d_object_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );

	int iPushStructCnt = rkLoader.LoadInt( "push_struct_cnt", 0 );
	m_vPushStructList.reserve( iPushStructCnt );

	//Push Struct
	for( int i=0; i<iPushStructCnt; i++ )
	{
		PushStruct kPush;
		kPush.m_iIndex = i + 1;

		wsprintf( szTitle, "push_struct%d_num", i+1 );
		kPush.m_iNum = rkLoader.LoadInt( szTitle, 0 );
		
		wsprintf( szTitle, "push_struct%d_pos_x", i+1 );
		kPush.m_CreatePos.x = rkLoader.LoadFloat( szTitle, 0.0f );

		wsprintf( szTitle, "push_struct%d_pos_y", i+1 );
		kPush.m_CreatePos.y = rkLoader.LoadFloat( szTitle, 0.0f );
		
		wsprintf( szTitle, "push_struct%d_pos_z", i+1 );
		kPush.m_CreatePos.z = rkLoader.LoadFloat( szTitle, 0.0f );

		m_iPushStructIdx = kPush.m_iIndex;
		m_vPushStructList.push_back( kPush );
	}


	int iObjectItemCnt = rkLoader.LoadInt( "object_item_cnt", 0 );

	ObjectItemList vObjectItemList;
	vObjectItemList.reserve( iObjectItemCnt );

	//Object Item
	for(int i=0; i<iObjectItemCnt; i++ )
	{
		ObjectItem kObjectItem;
		wsprintf( szTitle, "object_item%d_name", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		kObjectItem.m_ObjectItemName = szBuf;
		wsprintf( szTitle, "object_item%d_pos_x", i+1 );
		kObjectItem.m_fPosX = rkLoader.LoadFloat( szTitle, 0.0f );
		wsprintf( szTitle, "object_item%d_pos_z", i+1 );
		kObjectItem.m_fPosZ = rkLoader.LoadFloat( szTitle, 0.0f );

		vObjectItemList.push_back( kObjectItem );
	}

	//Push Struct
	SP2Packet kPushPacket( STPK_PUSHSTRUCT_INFO );
	if( GetPushStructInfo( kPushPacket ) )
	{
		SendRoomAllUser( kPushPacket );
	}

	//Object Item
	ItemVector vItemList;
	int iObjectCnt = vObjectItemList.size();

	for(int i=0; i<iObjectCnt; i++ )
	{
		const ObjectItem &rkObjItem = vObjectItemList[i];

		ioItem *pItem = m_pCreator->CreateItemByName( rkObjItem.m_ObjectItemName );
		if( pItem )
		{
			Vector3 vPos( rkObjItem.m_fPosX, 0.0f, rkObjItem.m_fPosZ );
			pItem->SetItemPos( vPos );
			vItemList.push_back( pItem );
		}
	}

	LoadWearItem(vItemList);

	if( vItemList.empty() )
		return;

	int iNewItemCnt = vItemList.size();
	SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );
	kPacket << iNewItemCnt;
	for(int i=0; i<iNewItemCnt; i++ )
	{
		ioItem *pItem = vItemList[i];
		m_pCreator->AddFieldItem( pItem );
		kPacket << pItem->GetItemCode();
		kPacket << pItem->GetItemReinforce();
		kPacket << pItem->GetItemMaleCustom();
		kPacket << pItem->GetItemFemaleCustom();
		kPacket << pItem->GetGameIndex();
		kPacket << pItem->GetItemPos();
		kPacket << pItem->GetOwnerName();
		kPacket << "";
	}

	SendRoomAllUser( kPacket );
}

void HiddenkingMode::LoadWearPosList(ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH] = "";

	int iSubNum = GetModeSubNum();
	int iMapIndex = GetModeMapNum();
	wsprintf( szTitle, "hidden%d_wear_generate%d", iSubNum, iMapIndex );

	int iItemPosCnt = rkLoader.LoadInt( szTitle, "pos_cnt", 0 );

	m_vWearPosList.clear();
	m_vWearPosList.reserve( iItemPosCnt );

	char szBuf[MAX_PATH]="";
	for( int i=0 ; i<iItemPosCnt ; i++ )
	{
		Vector3 vPos;
		memset(szBuf, 0, MAX_PATH);
		wsprintf( szBuf, "pos%d_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szTitle, szBuf, 0.0f );

		vPos.y = 0.0f;
		
		memset(szBuf, 0, MAX_PATH);
		wsprintf( szBuf, "pos%d_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szTitle, szBuf, 0.0f );
		m_vWearPosList.push_back( vPos );
	}
}

void HiddenkingMode::LoadWearItem( ItemVector &rvItemList)
{
	// ini load
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();

	char szBuf[MAX_PATH]="", szTitle[MAX_PATH]="";
	wsprintf( szTitle, "hidden%d_object_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );
	int iWearItemCnt = rkLoader.LoadInt( "wear_item_cnt", 0 );

	LoadWearPosList(rkLoader);

	for(int i=0; i<iWearItemCnt; i++ )
	{
		memset(szTitle, 0, MAX_PATH);
		memset(szBuf, 0, MAX_PATH);

		ObjectItem kWearItem;
		wsprintf( szTitle, "wear_item%d_name", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		
		ioItem *pItem = m_pCreator->CreateItemByName( szBuf );
		if( pItem )
		{
			pItem->SetItemPos( GetRandomWearPos( true ) );
			rvItemList.push_back( pItem );
		}
	}
}

Vector3 HiddenkingMode::GetRandomWearPos( bool bStartRound )
{
	int iMaxWearPos = m_vWearPosList.size();
	int iTempArray = rand() % iMaxWearPos;

	if( bStartRound )
		return m_vWearPosList[0];

	return m_vWearPosList[iTempArray];
}

void HiddenkingMode::CheckDropCrown(ioItem *pItem)
{
	if(!pItem) return;
	if( pItem->GetCrownItemType() != ioItem::MCT_HIDDEN_KING ) return;

	m_szNameOfKing.Clear();
	SetTakeKingTime( TEAM_NONE );

	m_dwCheckKingPingTime = 0;
}

void HiddenkingMode::CheckPickCrown( ioItem *pItem, User *pUser )
{
	if(!pItem) return;
	if(!pUser) return;
	if( pItem->GetCrownItemType() != ioItem::MCT_HIDDEN_KING ) return;
	
	m_szNameOfKing = pUser->GetPublicID();
	SetTakeKingTime(pUser->GetTeam());

	m_dwCheckKingPingTime = TIMEGETTIME();
}

Vector3 HiddenkingMode::GetRandomItemPos(ioItem *pItem)
{
	if(pItem)
	{
		int iEquipSlot = Help::GetEquipSlot( pItem->GetItemCode() );
		if(iEquipSlot == EQUIP_WEAR)
			return GetRandomWearPos( false );
	}

	if( m_vItemCreatePosList.empty() )
		return Vector3( 0.0f, 0.0f, 0.0f );

	Vector3 vPos;
	if( !m_vItemShufflePosList.empty() )
	{
		vPos = m_vItemShufflePosList.front();
		m_vItemShufflePosList.pop_front();
		return vPos;
	}

	m_vItemShufflePosList.clear();
	m_vItemShufflePosList.insert( m_vItemShufflePosList.begin(),
								  m_vItemCreatePosList.begin(),
								  m_vItemCreatePosList.end() );

	io_random_shuffle( m_vItemShufflePosList.begin(),
						 m_vItemShufflePosList.end() );

	vPos = m_vItemShufflePosList.front();
	m_vItemShufflePosList.pop_front();
	return vPos;
}

void HiddenkingMode::CheckGivePoint()
{
	DWORD dwCurTime = TIMEGETTIME();
	DWORD dwGapTime;

	if( m_dwBlueTakeKingTime != 0 )
	{
		dwGapTime = dwCurTime - m_dwBlueTakeKingTime;
		if( m_dwRemainTime > dwGapTime )
		{
			m_dwRemainTime -= dwGapTime;

			//Test
			m_dwBlueTakeKingTime = dwCurTime;
			m_dwRedTakeKingTime = 0;
		}
		else
		{

			SetScore(TEAM_BLUE, true );
			m_dwRemainTime = GetPointTime(TEAM_BLUE);

			// 모드 : 09.06.03
			/*
			bool bLast = false;
			if( m_iBlueTeamScore == m_iNeedPoint )
				bLast = true;

			SendScore( TEAM_BLUE, bLast);

			m_dwBlueTakeKingTime = dwCurTime;
			m_dwRedTakeKingTime = 0;
			*/
			SendScore(m_bZeroHP);

			m_dwBlueTakeKingTime = 0;
			m_dwRedTakeKingTime = 0;
		}
	}
	else if( m_dwRedTakeKingTime != 0) 
	{
		dwGapTime = dwCurTime - m_dwRedTakeKingTime;
		if( m_dwRemainTime > dwGapTime )
		{
			m_dwRemainTime -= dwGapTime;

			//Test
			m_dwRedTakeKingTime = dwCurTime;
			m_dwBlueTakeKingTime = 0;
		}
		else
		{
			SetScore(TEAM_RED, true );
			m_dwRemainTime = GetPointTime(TEAM_RED);

			// 모드 : 09.06.03
			/*
			bool bLast = false;
			if( m_iRedTeamScore == m_iNeedPoint )
				bLast = true;

			SendScore( TEAM_RED, bLast);

			m_dwBlueTakeKingTime = 0;
			m_dwRedTakeKingTime = dwCurTime;
			*/
			SendScore(m_bZeroHP);

			m_dwBlueTakeKingTime = 0;
			m_dwRedTakeKingTime = 0;
		}
	}
}

void HiddenkingMode::SetTakeKingTime( TeamType eKingTeam )
{
	if(eKingTeam == TEAM_BLUE)
	{
		m_dwBlueTakeKingTime = TIMEGETTIME();
		m_dwRedTakeKingTime  = 0;
		m_dwRemainTime = GetPointTime(TEAM_BLUE);
	}
	else if(eKingTeam == TEAM_RED)
	{
		m_dwBlueTakeKingTime = 0;
		m_dwRedTakeKingTime  = TIMEGETTIME();
		m_dwRemainTime = GetPointTime(TEAM_RED);
	}
	else
	{
		m_dwBlueTakeKingTime = 0;
		m_dwRedTakeKingTime  = 0;
		m_dwRemainTime = 0;

		m_szNameOfKing.Clear();
	}

	SP2Packet kPacket( STPK_KING_TAKE );
	kPacket << m_szNameOfKing;
	kPacket << m_dwRemainTime;
	m_pCreator->RoomSendPacketTcp( kPacket );
}

void HiddenkingMode::CheckCreateCrown(User *pUser)
{
	if(!pUser) return;
	if(m_szNameOfKing.IsEmpty()) return;
	if(pUser->GetPublicID() != m_szNameOfKing) return;

	ioItem *pDropItem = pUser->ReleaseItem(EQUIP_WEAR);
	if(!pDropItem) return;
	if( pDropItem->GetCrownItemType() != ioItem::MCT_HIDDEN_KING ) return;
	if(!m_pCreator) return;

	m_szNameOfKing.Clear();
	SetTakeKingTime(TEAM_NONE);

	pDropItem->SetItemPos( GetRandomWearPos( true ) );

	SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );
	kPacket << 1;

	m_pCreator->AddFieldItem( pDropItem );
	kPacket << pDropItem->GetItemCode();
	kPacket << pDropItem->GetItemReinforce();
	kPacket << pDropItem->GetItemMaleCustom();
	kPacket << pDropItem->GetItemFemaleCustom();
	kPacket << pDropItem->GetGameIndex();
	kPacket << pDropItem->GetItemPos();
	kPacket << pDropItem->GetOwnerName();
	kPacket << "";
	
	SendRoomAllUser( kPacket, pUser );
}

void HiddenkingMode::SendScore( bool bRoundEnd )
{
	SP2Packet kPacket( STPK_MODE_SCORE );
	kPacket << bRoundEnd;
	// 모드 : 09.06.03
	//kPacket << (int)eTeam;
	kPacket << m_iBlueTeamScore;
	kPacket << m_iRedTeamScore;
	kPacket << m_dwRemainTime;
	
	m_pCreator->RoomSendPacketTcp( kPacket );
}

void HiddenkingMode::UpdateRoundRecord()
{
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			if( pRecord->pUser )
			{
				pRecord->pUser->UpdateCharLimitDate();
				pRecord->pUser->UpdateEtcItemTime( __FUNCTION__ );
				pRecord->pUser->DeleteEtcItemPassedDate();
				pRecord->pUser->DeleteExtraItemPassedDate(true);
				pRecord->pUser->DeleteMedalItemPassedDate(true);
				pRecord->pUser->DeleteExMedalSlotPassedDate();
				pRecord->pUser->DeleteCharAwakePassedDate( );
				pRecord->pUser->DeleteCostumePassedDate();
				pRecord->pUser->ReleaseAccessoryPassedDate();
				// 임시 : 시간육성
				pRecord->pUser->CheckTimeGrowth();
				pRecord->pUser->DeleteExpiredBonusCash();
			}
		}
	}

	UpdateUserRank();

	if( IsRedWin( m_CurRoundWinTeam ) )
	{
		m_iRedTeamWinCnt += m_iRedTeamScore * m_iScoreRate;
		m_iBlueTeamWinCnt += m_iBlueTeamScore;
	}
	else if( IsBlueWin( m_CurRoundWinTeam ) )
	{
		m_iBlueTeamWinCnt += m_iBlueTeamScore * m_iScoreRate;
		m_iRedTeamWinCnt += m_iRedTeamScore;
	}
	else
	{
		m_iRedTeamWinCnt += m_iRedTeamScore;
		m_iBlueTeamWinCnt += m_iBlueTeamScore;
	}
}

DWORD HiddenkingMode::GetPointTime( TeamType eKingTeam )
{
	int iRedTeamCnt = GetCurTeamUserCnt( TEAM_RED );
	int iBlueTeamCnt = GetCurTeamUserCnt( TEAM_BLUE );

	m_dwGivePointTime = m_dwDrawScoreTime;

	if( eKingTeam == TEAM_RED )
	{
		if( m_iTotalRedScore > m_iTotalBlueScore )
			m_dwGivePointTime = m_dwWinTeamScoreTime;
		else if( m_iTotalRedScore < m_iTotalBlueScore )
			m_dwGivePointTime = m_dwLoseTeamScoreTime;
		else
			m_dwGivePointTime = m_dwDrawScoreTime;

		CheckDecreasePointTimeByPeopleCnt();

		if( CheckManyPeopleTeam() == TEAM_RED && iBlueTeamCnt > 0 )
		{
			float fRate = (float)iRedTeamCnt / (float)iBlueTeamCnt;
			fRate *= m_fManyPeopleRate;
			m_dwGivePointTime = (DWORD)(m_dwGivePointTime * fRate );
		}
	}
	else if( eKingTeam == TEAM_BLUE )
	{
		if( m_iTotalRedScore > m_iTotalBlueScore )
			m_dwGivePointTime = m_dwLoseTeamScoreTime;
		else if( m_iTotalRedScore < m_iTotalBlueScore )
			m_dwGivePointTime = m_dwWinTeamScoreTime;
		else
			m_dwGivePointTime = m_dwDrawScoreTime;

		CheckDecreasePointTimeByPeopleCnt();

		if( CheckManyPeopleTeam() == TEAM_BLUE && iRedTeamCnt > 0 )
		{
			float fRate = (float)iBlueTeamCnt / (float)iRedTeamCnt;
			fRate *= m_fManyPeopleRate;
			m_dwGivePointTime = (DWORD)(m_dwGivePointTime * fRate );
		}
	}

	// 라운드 시간에따른 조정
	// 임시 수정
	/*
	DWORD dwGapTime = TIMEGETTIME() - m_dwModeStartTime;
	float fCurRate = 1.0f + ( (float)dwGapTime / m_dwScoreGaugeConstValue );
	fCurRate = min( fCurRate, m_fScoreGaugeMaxRate );

	m_dwGivePointTime /= fCurRate;
	*/

	if( IsZeroHP() )
	{
		m_dwGivePointTime /= m_fScoreGaugeDethTimeRate;
	}
	//

	return m_dwGivePointTime;
}

DWORD HiddenkingMode::CheckDecreasePointTimeByPeopleCnt()
{
	int iGapCnt = GetCurTeamUserCnt( TEAM_RED ) + GetCurTeamUserCnt( TEAM_BLUE ) - 2;
	iGapCnt = max( 0, min(iGapCnt, 14) );

	float fRate = m_fDecreaseScoreTimeRate * (float)iGapCnt / 14.0f;
	DWORD dwGapTime = m_dwGivePointTime * fRate;
	m_dwGivePointTime -= dwGapTime;

	return m_dwGivePointTime;
}

TeamType HiddenkingMode::CheckManyPeopleTeam()
{
	int iRedTeamCnt = GetCurTeamUserCnt( TEAM_RED );
	int iBlueTeamCnt = GetCurTeamUserCnt( TEAM_BLUE );

	if( iRedTeamCnt > iBlueTeamCnt )
		return TEAM_RED;
	else if( iBlueTeamCnt > iRedTeamCnt )
		return TEAM_BLUE;
	else
		return TEAM_NONE;
}

int HiddenkingMode::GetCurTeamUserCnt( TeamType eTeam )
{
	int iUserCnt = 0;
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			if( pRecord->eState == RS_VIEW ||
				pRecord->eState == RS_OBSERVER ||
				pRecord->eState == RS_LOADING )
				continue;

			User *pUser = pRecord->pUser;

			if( pUser && pUser->GetTeam() == eTeam )
				iUserCnt++;
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "HiddenkingMode::GetCurTeamUserCnt() - User's ModeRecord is Not Exist(%d).", i );
		}
	}

	return iUserCnt;
}

void HiddenkingMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
{
	bool bRoundChange;
	int iRoomIndex;
	rkPacket >> bRoundChange >> iRoomIndex;
	if( iRoomIndex != m_pCreator->GetRoomIndex() )
		return;

	if( !bRoundChange )
	{
		if( !pSend->IsObserver() && !pSend->IsStealth() )
		{
			SP2Packet kPacket( STPK_START_SELECT_CHAR );
			kPacket << GetSelectCharTime();
			pSend->SendMessage( kPacket );
		}

		return;
	}

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "HiddenkingMode::OnEventSceneEnd - %s Not Exist Record",
								pSend->GetPublicID().c_str() );
		return;
	}

	//로딩 시간을 로그로 남김
	pRecord->CheckLoadingTime();
	pRecord->pUser->EquipDBItemToAllChar();
	SetFirstRevivalTime( pRecord );

	int iModeState;
	switch( m_ModeState )
	{
	case MS_READY:
	case MS_PLAY:
		iModeState = m_ModeState;
		break;
	case MS_RESULT_WAIT:
	case MS_RESULT:
		iModeState = MS_RESULT_WAIT;
		break;
	}

	DWORD dwPastTime = TIMEGETTIME() - m_dwStateChangeTime;
	if( pRecord->pUser->IsObserver() || pRecord->pUser->IsStealth() )
	{
		pRecord->eState = RS_OBSERVER;
		pRecord->pUser->SetStartTimeLog( TIMEGETTIME() );
		SP2Packet kPacket( STPK_ROUND_JOIN_OBSERVER );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << m_dwCurRoundDuration;
		SendRoomAllUser( kPacket );
	}
	else if( m_bUseViewMode && m_ModeState == MS_PLAY && dwPastTime > m_dwViewCheckTime )
	{
		pRecord->eState = RS_VIEW;
		pRecord->pUser->SetStartTimeLog( TIMEGETTIME() );
		SP2Packet kPacket( STPK_ROUND_JOIN_VIEW );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << m_dwCurRoundDuration;
		SendRoomAllUser( kPacket );
	}
	else
	{
		pRecord->eState = RS_PLAY;
		pRecord->StartPlaying();        //( 관전X, 데스타임X )
		pRecord->pUser->StartCharLimitDate( Mode::GetCharLimitCheckTime(), __FILE__, __LINE__ );
		pRecord->pUser->StartEtcItemTime( __FUNCTION__ );

		SP2Packet kPacket( STPK_ROUND_JOIN );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << GetSelectCharTime();
		kPacket << m_dwCurRoundDuration;
		SendRoomAllUser( kPacket );
	}

	TeamType eKingTeam = TEAM_NONE;
	if( m_dwBlueTakeKingTime != 0 )
		eKingTeam = TEAM_BLUE;
	else if( m_dwRedTakeKingTime != 0 )
		eKingTeam = TEAM_RED;

	DWORD dwPrePointTime = m_dwGivePointTime;
	DWORD dwPointTime = GetPointTime(eKingTeam);
	m_dwRemainTime = dwPointTime * ((float)m_dwRemainTime / (float)dwPrePointTime);

	SP2Packet kModeInfoPk( STPK_MODE_INFO );
	GetModeInfo( kModeInfoPk );
	SendRoomPlayUser( kModeInfoPk );
}

bool HiddenkingMode::CheckRoundJoin( User *pSend )
{
	if( m_ModeState != MS_RESULT && m_ModeState != MS_RESULT_WAIT )
		return false;

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "HiddenkingMode::CheckRoundJoin - %s Not Exist Record",
								 pSend->GetPublicID().c_str() );
		return false;
	}

	pRecord->pUser->EquipDBItemToAllChar();
	SetFirstRevivalTime( pRecord );

	int iModeState = MS_RESULT_WAIT;

	DWORD dwPastTime = TIMEGETTIME() - m_dwStateChangeTime;
	pRecord->eState = RS_PLAY;
	pRecord->StartPlaying();        //( 관전X, 데스타임X )
	pRecord->pUser->StartCharLimitDate( Mode::GetCharLimitCheckTime(), __FILE__, __LINE__ );
	pRecord->pUser->StartEtcItemTime( __FUNCTION__ );

	SP2Packet kPacket( STPK_ROUND_JOIN );
	kPacket << pRecord->pUser->GetPublicID();
	kPacket << iModeState;
	kPacket << dwPastTime;
	kPacket << GetSelectCharTime();
	kPacket << m_dwCurRoundDuration;
	SendRoomAllUser( kPacket );

	TeamType eKingTeam = TEAM_NONE;
	if( m_dwBlueTakeKingTime != 0 )
		eKingTeam = TEAM_BLUE;
	else if( m_dwRedTakeKingTime != 0 )
		eKingTeam = TEAM_RED;

	DWORD dwPrePointTime = m_dwGivePointTime;
	DWORD dwPointTime = GetPointTime(eKingTeam);
	m_dwRemainTime = dwPointTime * ((float)m_dwRemainTime / (float)dwPrePointTime);

	SP2Packet kModeInfoPk( STPK_MODE_INFO );
	GetModeInfo( kModeInfoPk );
	SendRoomPlayUser( kModeInfoPk );

	return true;
}

void HiddenkingMode::ProcessKingPing()
{
	//LOG.PrintTimeAndLog(0,"킹핑이야");
	if( m_szNameOfKing.IsEmpty() ) return;

	if( m_dwKingPingTime == 0 )
		return;

	DWORD dwCurTime = TIMEGETTIME();
	DWORD dwGapTime = 0;

	if( dwCurTime > m_dwCheckKingPingTime )
		dwGapTime = dwCurTime - m_dwCheckKingPingTime;

	if( dwGapTime > m_dwKingPingTime )
	{
		bool bCrownDrop = false;
		int iCharCnt = m_vRecordList.size();
		for( int i=0; i < iCharCnt; ++i )
		{
			User *pUser = m_vRecordList[i].pUser;
			if( !pUser ) continue;
			
			if(pUser->IsRelayUse())
			{
				if(pUser->DropKing() >= 1) // 고민해볼것 몇초로 할찌는 ini 설정이 달라저야할까? 현재는 1분임 
				{
					if( !pUser->GetPublicID().IsEmpty() && pUser->GetPublicID() == m_szNameOfKing )
					{
						BadPingDropCrown(pUser);

					}
					
				}
				pUser->DropKing(0);
				continue;
			}
			
		

			if( !pUser->GetPublicID().IsEmpty() && pUser->GetPublicID() == m_szNameOfKing )
			{
				int iCurCnt = pUser->GetCurKingPingCnt();
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "HiddenkingMode::ProcessKingPing - King: %s, PingCnt: %d", m_szNameOfKing.c_str(), iCurCnt );
				
				if( iCurCnt < m_iKingPingCnt )
				{
					// 왕관 드롭
					bCrownDrop = true;
					BadPingDropCrown( pUser );
				}

				pUser->ClearKingPingCnt();
				break;
			}
		}

		if( bCrownDrop )
			m_dwCheckKingPingTime = 0;
		else
			m_dwCheckKingPingTime = dwCurTime;
	}
}

void HiddenkingMode::BadPingDropCrown( User *pUser )
{
	if(!pUser) return;
	if(m_szNameOfKing.IsEmpty()) return;
	if(pUser->GetPublicID() != m_szNameOfKing) return;

	ioItem *pDropItem = pUser->ReleaseItem(EQUIP_WEAR);
	if(!pDropItem) return;
	if( pDropItem->GetCrownItemType() != ioItem::MCT_HIDDEN_KING ) return;
	if(!m_pCreator) return;

	m_szNameOfKing.Clear();
	SetTakeKingTime(TEAM_NONE);

	pDropItem->SetItemPos( GetRandomWearPos( true ) );

	m_pCreator->AddFieldItem( pDropItem );

	SP2Packet kPacket( STPK_BAD_PING_CROWN_DROP );
	kPacket << pUser->GetPublicID();
	kPacket << pDropItem->GetItemCode();
	kPacket << pDropItem->GetItemReinforce();
	kPacket << pDropItem->GetItemMaleCustom();
	kPacket << pDropItem->GetItemFemaleCustom();
	kPacket << pDropItem->GetGameIndex();
	kPacket << pDropItem->GetItemPos();
	kPacket << pDropItem->GetOwnerName();
	SendRoomAllUser( kPacket );
}

