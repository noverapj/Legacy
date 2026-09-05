

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "Room.h"
#include "FootballMode.h"

#include "ModeHelp.h"
#include "../DataBase/LogDBClient.h"

extern CLog RateCheckLOG;

FootballMode::FootballMode( Room *pCreator ) : Mode( pCreator )
{
	m_bRedGoal = false;
	m_bBlueGoal = false;
}

FootballMode::~FootballMode()
{
}

void FootballMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();
}

void FootballMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;
	wsprintf( szTitle, "football%d_object_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );
	
	int iPushStructCnt = rkLoader.LoadInt( "push_struct_cnt", 0 );
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
	vObjectItemList.clear();
	vObjectItemList.reserve( iObjectItemCnt );

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

	if( vItemList.empty() )
		return;

	SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );
	int iNewItemCnt = vItemList.size();
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

void FootballMode::AddNewRecord( User *pUser )
{
	FootballRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	UpdateUserRank();
}

void FootballMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
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

	UpdateUserRank();

	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	}
}

void FootballMode::LoadINIValue()
{
	Mode::LoadINIValue();

	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	m_dwCurRoundDuration = m_dwRoundDuration;
	m_vRoundHistory.clear();

	m_bRedGoal = false;
	m_bBlueGoal = false;
}

void FootballMode::ProcessPlay()
{
	ProcessRevival();
	CheckRoundTimePing();
	CheckFieldItemLiveTime();
	CheckItemSupply( m_dwStateChangeTime );
	CheckBallSupply( m_dwStateChangeTime );
	CheckMachineSupply( m_dwStateChangeTime );
	CheckNeedSendPushStruct();
	CheckRoundEnd( true );
	
	ProcessEvent();
	ProcessBonusAlarm();
}

void FootballMode::CheckRoundEnd( bool bProcessCall )
{
	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;

	WinTeamType eWinTeam = WTT_DRAW;
	if( m_bRedGoal )
		eWinTeam = WTT_RED_TEAM;
	else if( m_bBlueGoal )
		eWinTeam = WTT_BLUE_TEAM;

	if( m_dwCurRoundDuration < dwGapTime+1000 )
	{
		if( !m_bZeroHP )
		{
			m_bZeroHP = true;
			m_dwCurRoundDuration = 0;
			m_dwCurSuddenDeathDuration = TIMEGETTIME();
			m_fSuddenDeathBlueCont	   = 0.0f;
			m_fSuddenDeathRedCont	   = 0.0f;

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
	if( GetState() != MS_PLAY && GetState() != MS_READY)
		eWinTeam = WTT_DRAW;
	else if( iBlueUser == 0 )
		eWinTeam = WTT_RED_TEAM;
	else if( iRedUser == 0 )
		eWinTeam = WTT_BLUE_TEAM;

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

void FootballMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd ) return;

	if( GetTeamUserCnt( TEAM_BLUE ) == 0 ||	GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		CheckRoundEnd( false );
	}
}

void FootballMode::SetRoundEndInfo( WinTeamType eWinTeam )
{
	m_CurRoundWinTeam = eWinTeam;
	
	if(	GetTeamUserCnt( TEAM_BLUE ) == 0 ||
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
		}
		if( pRecord->pUser && pRecord->pUser->GetStartTimeLog() > 0 )
		{
			if( pRecord->eState != RS_VIEW && pRecord->eState != RS_OBSERVER )
				pRecord->AddDeathTime( TIMEGETTIME() - pRecord->pUser->GetStartTimeLog() );
			else
				g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_VIEW );
			pRecord->pUser->SetStartTimeLog(0);
		}
	}
	
	int HistorySize = m_vRoundHistory.size();
	if( m_iCurRound-1 > HistorySize )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FootballMode::SetRoundEndInfo() m_iCurRound != m_vRoundHistory.size()" );
	}
	else
	{
		RoundHistory rh;
		if( eWinTeam == TEAM_RED )
		{
			rh.iBluePoint = 0;
			rh.iRedPoint = 1;
		}
		else if( eWinTeam == TEAM_BLUE )
		{
			rh.iBluePoint = 1;
			rh.iRedPoint = 0;
		}
		m_vRoundHistory.push_back( rh );
	}
}

void FootballMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FootballMode::OnEventSceneEnd - %s Not Exist Record",
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
}

void FootballMode::RestartMode()
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
		FootballRecord &rkRecord = m_vRecordList[i];
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

	m_bRedGoal = false;
	m_bBlueGoal = false;
	
	m_pCreator->DestroyAllFieldItems();

	SetModeState( MS_READY );

	SP2Packet kPacket( STPK_ROUND_READY );
	kPacket << m_iCurRound;
	kPacket << m_iBluePosArray;
	kPacket << m_iRedPosArray;
	kPacket << m_dwCurRoundDuration;
	SendRoomAllUser( kPacket );

	InitObjectGroupList();
}

ModeType FootballMode::GetModeType() const
{
	return MT_FOOTBALL;
}

void FootballMode::GetModeInfo( SP2Packet &rkPacket )
{
	Mode::GetModeInfo( rkPacket );

	GetModeHistory( rkPacket );
}

void FootballMode::GetModeHistory( SP2Packet &rkPacket )
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

void FootballMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck )
{
	FootballRecord *pRecord = FindFootballRecord( szName );
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

int FootballMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* FootballMode::GetModeINIFileName() const
{
	return "config/footballmode.ini";
}

TeamType FootballMode::GetNextTeamType()
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
	if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
		return TEAM_BLUE;
	else if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
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

int FootballMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
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

ModeRecord* FootballMode::FindModeRecord( const ioHashString &rkName )
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

ModeRecord* FootballMode::FindModeRecord( User *pUser )
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

ModeRecord* FootballMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

FootballRecord* FootballMode::FindFootballRecord( const ioHashString &rkName )
{
	return (FootballRecord*)FindModeRecord( rkName );
}

FootballRecord* FootballMode::FindFootballRecord( User *pUser )
{
	return (FootballRecord*)FindModeRecord( pUser );
}

bool FootballMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_FOOTBALL_GOAL:
		OnGoal( pSend, rkPacket );
		return true;
	}

	return false;
}

void FootballMode::OnGoal( User *pUser, SP2Packet &rkPacket )
{
	if( GetState() != MS_PLAY )
		return;

	// goal 넣은 팀
	int iTeam;
	rkPacket >> iTeam;

	// 마지막 터치 유저
	ioHashString szGoalName;
	rkPacket >> szGoalName;

	// 마지막 앞 터치 유저
	ioHashString szAssistName;
	rkPacket >> szAssistName;

	User *pGoalUser = m_pCreator->FindUserInRoom( szGoalName );
	if( !pGoalUser )
	{
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FootballMode::OnGoal - Not Find user : %s", szGoalName.c_str() );

		SP2Packet kReturn( STPK_FOOTBALL_GOAL );
		kReturn << FOOTBALL_GOAL_FAIL;
		kReturn << szGoalName;
		pUser->SendMessage( kReturn );
		return;
	}

	if( m_bRedGoal || m_bBlueGoal )
	{
		RateCheckLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FootballMode::OnGoal - Already Goal : %d, %d", m_bRedGoal, m_bBlueGoal );

		SP2Packet kReturn( STPK_FOOTBALL_GOAL );
		kReturn << FOOTBALL_GOAL_FAIL;
		kReturn << szGoalName;
		pUser->SendMessage( kReturn );
		return;
	}

	if( iTeam == TEAM_RED )
		m_bRedGoal = true;
	else if( iTeam == TEAM_BLUE )
		m_bBlueGoal = true;

	SP2Packet kReturn( STPK_FOOTBALL_GOAL );
	kReturn << FOOTBALL_GOAL_OK;
	kReturn << iTeam;
	kReturn << szGoalName;
	kReturn << szAssistName;
	SendRoomAllUser( kReturn );

	CheckRoundEnd(true);
}

void FootballMode::UpdateRoundRecord()
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
		m_iRedTeamWinCnt++;
	else if( IsBlueWin( m_CurRoundWinTeam ) )
		m_iBlueTeamWinCnt++;
}

int FootballMode::GetCurTeamUserCnt( TeamType eTeam )
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
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FootballMode::GetCurTeamUserCnt() - User's ModeRecord is Not Exist(%d).", i );
		}
	}

	return iUserCnt;
}

bool FootballMode::CheckRoundJoin( User *pSend )
{
	if( m_ModeState != MS_RESULT && m_ModeState != MS_RESULT_WAIT )
		return false;

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FootballMode::CheckRoundJoin - %s Not Exist Record", pSend->GetPublicID().c_str() );
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

	return true;
}

