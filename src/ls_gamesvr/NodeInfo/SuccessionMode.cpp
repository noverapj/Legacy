
#include "stdafx.h"
#include "../MainProcess.h"

#include "SuccessionMode.h"

#include "Room.h"
#include "ModeHelp.h"
#include "RoomNodeManager.h"
#include "../EtcHelpFunc.h"
#include "../DataBase/LogDBClient.h"
#include "../DataBase/DBClient.h"

#include "MatchManager.h"
#include "ioSpiritManager.h"

SuccessionMode::SuccessionMode( Room *pCreator ) : Mode( pCreator )
{
	m_iRedCatchBluePlayer = 0;
	m_iBlueCatchRedPlayer = 0;
	m_iBasePoint = 10;
	m_iLeaveWinPoint = 5;
	m_iLeaveLosePoint = 5;
}

SuccessionMode::~SuccessionMode()
{
}

void SuccessionMode::InitMode()
{
	Mode::InitMode();

	m_bReserveRevengeMatch = false;
}

void SuccessionMode::LoadINIValue()
{
	Mode::LoadINIValue();

	m_dwCurRoundDuration = m_dwRoundDuration;
	m_vRoundHistory.clear();

	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );
	rkLoader.SetTitle( "ELO_info" );
	m_iBasePoint = rkLoader.LoadInt( "base_point", 10 );
	m_iLeaveWinPoint = rkLoader.LoadInt( "leave_win_point", 5 );
	m_iLeaveLosePoint = rkLoader.LoadInt( "leave_lose_point", 5 );
}

void SuccessionMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();
}

void SuccessionMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "catch%d_object_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );

	int iPushStructCnt = rkLoader.LoadInt( "push_struct_cnt", 0 );
	m_vPushStructList.reserve( iPushStructCnt );

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

void SuccessionMode::AddNewRecord( User *pUser )
{
	SuccessionRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	if( pUser )
	{
		pUser->AddSuccessionPlayCount();
		pUser->SaveSuccessionData();
	}

	UpdateUserRank();
}

void SuccessionMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	bool bLeaveUser = true;

	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
		{
			SetModeEndDBLog( &m_vRecordList[i], iCharCnt, LogDBClient::PRT_EXIT_ROOM );

			if( m_vRecordList[i].bPrisoner || m_vRecordList[i].bDieState )
			{
				if( m_vRecordList[i].pUser->GetTeam() == TEAM_RED )
				{
					m_iBlueCatchRedPlayer--;
					m_iBlueCatchRedPlayer = max( 0, m_iBlueCatchRedPlayer );
				}
				else if( m_vRecordList[i].pUser->GetTeam() == TEAM_BLUE )
				{
					m_iRedCatchBluePlayer--;
					m_iRedCatchBluePlayer = max( 0, m_iRedCatchBluePlayer );
				}
				bLeaveUser = false;
			}
			m_vRecordList.erase( m_vRecordList.begin() + i );			
			break;
		}
	}

	UpdateUserRank();

	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	}

	if( !m_bRoundSetEnd && !bRoomDestroy && GetState() == MS_PLAY )
	{
		// 유저가 전부 나가면 1점 !!
		WinTeamType eWinTeam = WTT_DRAW;	
		int iBlueUser = GetCurTeamUserCnt( TEAM_BLUE );
		int iRedUser  = GetCurTeamUserCnt( TEAM_RED );
		if( iBlueUser == 0 )
			eWinTeam = WTT_RED_TEAM;
		else if( iRedUser == 0 )
			eWinTeam = WTT_BLUE_TEAM;
		else 
			return;

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

	// 이탈유저 체크
	int iBlueUser = GetCurTeamUserCnt( TEAM_BLUE );
	int iRedUser  = GetCurTeamUserCnt( TEAM_RED );
	if( iBlueUser == 1 && iRedUser == 0 )
	{
		if( GetState() == MS_RESULT_WAIT || GetState() == MS_PLAY )
		{
			if( !IsRoomInfoLog() && bLeaveUser )
			{
				SetModeEndDBLog( m_pCreator, LogDBClient::PRT_EXIT_ROOM );
				SetRoomInfoLog( true );
			}
		}

	}
	else if( iBlueUser == 0 && iRedUser == 1 )
	{
		if( GetState() == MS_RESULT_WAIT || GetState() == MS_PLAY )
		{
			if( !IsRoomInfoLog() && bLeaveUser )
			{
				SetModeEndDBLog( m_pCreator, LogDBClient::PRT_EXIT_ROOM );
				SetRoomInfoLog( true );
			}
		}
	}

}

void SuccessionMode::ProcessPlay()
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

void SuccessionMode::RestartMode()
{
	m_dwCurRoundDuration = m_dwRoundDuration;
	m_bZeroHP = false;

	m_iCurItemSupplyIdx = 0;
	m_iCurBallSupplyIdx = 0;

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		SuccessionRecord &rkRecord = m_vRecordList[i];
		rkRecord.pUser->EquipDBItemToLiveChar();

		rkRecord.dwCurDieTime = 0;
		rkRecord.iRevivalCnt  = 0;
		rkRecord.dwRevivalGap = (DWORD)GetRevivalGapTime( 0 );
		rkRecord.bCatchState = false;

		rkRecord.dwPlayingStartTime= 0;
		rkRecord.dwCurPrisonerTime = 0;
		rkRecord.bFirstPrisoner = false;
		rkRecord.bPrisoner = false;
		rkRecord.bDieState = false;

		if( rkRecord.eState == RS_VIEW )
			rkRecord.eState = RS_PLAY;
	}

	m_CurRoundWinTeam = WTT_NONE;

	m_iRedCatchBluePlayer = 0;
	m_iBlueCatchRedPlayer = 0;

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

int SuccessionMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
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

ModeRecord* SuccessionMode::FindModeRecord( const ioHashString &rkName )
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

ModeRecord* SuccessionMode::FindModeRecord( User *pUser )
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

ModeRecord* SuccessionMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

SuccessionRecord* SuccessionMode::FindSuccessionRecord( const ioHashString &rkName )
{
	return (SuccessionRecord*)FindModeRecord( rkName );
}

SuccessionRecord* SuccessionMode::FindSuccessionRecord( User *pUser )
{
	return (SuccessionRecord*)FindModeRecord( pUser );
}

void SuccessionMode::UpdateDieState( User *pDier )
{
	SuccessionRecord *pDieRecord = FindSuccessionRecord( pDier );
	if( !pDieRecord ) return;
	if( pDieRecord->bDieState ) return;

	pDieRecord->bDieState = true;
	pDieRecord->dwCurDieTime = 0;
	pDieRecord->bExperienceState = false;

	if( !pDieRecord->bPrisoner )
	{
		if( pDier->GetTeam() == TEAM_RED )
			m_iBlueCatchRedPlayer++;
		else if( pDier->GetTeam() == TEAM_BLUE )
			m_iRedCatchBluePlayer++;
	}
}

void SuccessionMode::UpdateUserDieTime( User *pDier )
{
	SuccessionRecord *pDieRecord = FindSuccessionRecord( pDier );
	if( !pDieRecord ) return;
	if( !pDieRecord->bDieState ) return;

	DWORD dwRevivalGap = (DWORD)GetRevivalGapTime( pDieRecord->iRevivalCnt );
	pDieRecord->dwRevivalGap = dwRevivalGap;
	pDieRecord->iRevivalCnt++;
}

ModeType SuccessionMode::GetModeType() const
{
	return MT_SUCCESSION;
}

void SuccessionMode::GetModeInfo( SP2Packet &rkPacket )
{
	Mode::GetModeInfo( rkPacket );

	rkPacket << m_iRedCatchBluePlayer;
	rkPacket << m_iBlueCatchRedPlayer;

	GetModeHistory( rkPacket );
}

void SuccessionMode::GetModeHistory( SP2Packet &rkPacket )
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

void SuccessionMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck )
{
	SuccessionRecord *pRecord = FindSuccessionRecord( rkName );
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
		rkPacket << pRecord->bPrisoner;
		rkPacket << pRecord->bCatchState;
	}
	else
	{
		// 레코드 정보 유무
		rkPacket << false;
	}
}

int SuccessionMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* SuccessionMode::GetModeINIFileName() const
{
	return "config/successionmode.ini";
}

TeamType SuccessionMode::GetNextTeamType()
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

void SuccessionMode::CheckRoundEnd( bool bProcessCall )
{
	WinTeamType eWinTeam = WTT_DRAW;
	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;

	if( m_iRedCatchBluePlayer == GetCurTeamUserCnt( TEAM_BLUE ) )
		eWinTeam = WTT_RED_TEAM;
	else if( m_iBlueCatchRedPlayer == GetCurTeamUserCnt( TEAM_RED ) )
		eWinTeam = WTT_BLUE_TEAM;

	if( m_dwCurRoundDuration < dwGapTime+1000 )
	{
		if( !m_bZeroHP )
		{
			SP2Packet kPacket( STPK_ZERO_HP );
			kPacket << m_dwSuddenDeathTime;
			SendRoomAllUser( kPacket );
			m_bZeroHP = true;

			m_dwCurRoundDuration = 0;
			m_dwCurSuddenDeathDuration = TIMEGETTIME();
			m_fSuddenDeathBlueCont	   = 0.0f;
			m_fSuddenDeathRedCont	   = 0.0f;

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

	if( eWinTeam == WTT_DRAW )
		return;

	int iBlueUser = GetTeamUserCnt( TEAM_BLUE );
	int iRedUser  = GetTeamUserCnt( TEAM_RED );
	if( iBlueUser == 0 || iRedUser == 0 )
	{
		eWinTeam = WTT_DRAW;
	}

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

void SuccessionMode::SetRoundEndInfo( WinTeamType eWinTeam )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SuccessionMode::SetRoundEndInfo() m_iCurRound != m_vRoundHistory.size()" );
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

void SuccessionMode::SendRoundResult( WinTeamType eWinTeam )
{
	SP2Packet kPacket( STPK_ROUND_END );
	PACKET_GUARD_VOID( kPacket.Write(m_bReserveRevengeMatch) );
	PACKET_GUARD_VOID( kPacket.Write(eWinTeam) );
	PACKET_GUARD_VOID( kPacket.Write(m_iRedTeamWinCnt) );
	PACKET_GUARD_VOID( kPacket.Write(m_iBlueTeamWinCnt) );
	PACKET_GUARD_VOID( kPacket.Write(GetPlayingUserCnt()) );

	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || pRecord->eState == RS_LOADING )
			continue;

		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->GetPublicID()) );
		//
		int iMyVictories = 0;
		if( pRecord->pUser )
		{
			if( m_bRoundSetEnd && eWinTeam != WTT_DRAW && eWinTeam != WTT_NONE )
				pRecord->pUser->IncreaseMyVictories( IsWinTeam( eWinTeam, pRecord->pUser->GetTeam() ) );

			iMyVictories = pRecord->pUser->GetMyVictories();
		}

		PACKET_GUARD_VOID( kPacket.Write(iMyVictories) );

		int iKillSize = pRecord->iKillInfoMap.size();
		PACKET_GUARD_VOID( kPacket.Write(iKillSize) );

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_k = pRecord->iKillInfoMap.begin();
		while( iter_k != pRecord->iKillInfoMap.end() )
		{
			PACKET_GUARD_VOID( kPacket.Write(iter_k->first) );
			PACKET_GUARD_VOID( kPacket.Write(iter_k->second) );

			++iter_k;
		}
		LOOP_GUARD_CLEAR();

		int iDeathSize = pRecord->iDeathInfoMap.size();
		PACKET_GUARD_VOID( kPacket.Write(iDeathSize) );

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_d = pRecord->iDeathInfoMap.begin();
		while( iter_d != pRecord->iDeathInfoMap.end() )
		{
			PACKET_GUARD_VOID( kPacket.Write(iter_d->first) );
			PACKET_GUARD_VOID( kPacket.Write(iter_d->second) );

			++iter_d;
		}
		LOOP_GUARD_CLEAR();
		//

		PACKET_GUARD_VOID( kPacket.Write(pRecord->iCurRank) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iPreRank) );
	}

	PACKET_GUARD_VOID( kPacket.Write(m_bRoundSetEnd) );

	FillResultSyncUser( kPacket );

	SendRoomAllUser( kPacket );

	// 클라이언트가 위의 패킷을 받으면 서버로 캐릭터 살리는 패킷을 보내는데 이해가 되지 않는다. 
	// 그냥 아래처럼하면 패킷 보낼 필요 없지 않을까?  LJH..... 20081002
	for(int i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || !pRecord->pUser )
			continue;
		pRecord->pUser->SetCharDie( false );
	}
}

bool SuccessionMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_PRISONER_ESCAPE:
		OnPrisonerEscape( pSend, rkPacket );
		return true;
	case CTPK_PRISONER_DROP:
		OnPrisonerDrop( pSend, rkPacket );
		return true;
	case CTPK_PRISONERMODE:
		OnPrisonerMode( pSend, rkPacket );
		return true;
	case CTPK_SUCCESSION_REQUEST_REVENGE:
		OnRequestRevenge( pSend, rkPacket );
		return true;
	}

	return false;
}

void SuccessionMode::OnPrisonerEscape( User *pUser, SP2Packet &rkPacket )
{
	ioHashString szName, szLastAttacker, szLastAttackerSkill;	
	rkPacket >> szName >> szLastAttacker >> szLastAttackerSkill;

	SuccessionRecord *pEscape = FindSuccessionRecord( szName );
	if( !pEscape || !pEscape->bPrisoner ) return;

	if( pEscape->pUser->GetTeam() == TEAM_RED )
	{
		m_iBlueCatchRedPlayer--;
		m_iBlueCatchRedPlayer = max( 0, m_iBlueCatchRedPlayer );
	}
	else if( pEscape->pUser->GetTeam() == TEAM_BLUE )
	{
		m_iRedCatchBluePlayer--;
		m_iRedCatchBluePlayer = max( 0, m_iRedCatchBluePlayer );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SuccessionMode::OnPrisonerEscape() - %s has not Team.",
			pEscape->pUser->GetPublicID().c_str() );
		return;
	}

	pEscape->bPrisoner = false;
	pEscape->bDieState = false;
	pEscape->dwCurDieTime = 0;

	SP2Packet kReturn( STPK_PRISONER_ESCAPE );
	kReturn << pUser->GetPublicID() << szLastAttacker << szLastAttackerSkill;
	SendRoomAllUser( kReturn );
}

void SuccessionMode::OnPrisonerDrop( User *pUser, SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	SuccessionRecord *pEscape = FindSuccessionRecord( pUser );
	if( !pEscape || !pEscape->bPrisoner ) return;

	pEscape->bPrisoner = false;
	pEscape->bDieState = true;
	pEscape->dwCurDieTime = 0;

	int iDamageCnt;
	ioHashString szBestAttacker;
	rkPacket >> iDamageCnt;

	int iTotalDamage = 0;
	int iLastDamage = 0;	

	if( iDamageCnt > 0 )
	{
		DamageTableList vDamageList;
		vDamageList.reserve( iDamageCnt );

		for( int i=0; i < iDamageCnt; ++i )
		{
			DamageTable kDamageTable;
			rkPacket >> kDamageTable.szName;
			rkPacket >> kDamageTable.iDamage;

			if( kDamageTable.szName == szAttacker )
				iLastDamage = kDamageTable.iDamage;

			vDamageList.push_back( kDamageTable );

			if( kDamageTable.iDamage > 0 )
			{
				iTotalDamage += kDamageTable.iDamage;

				ModeRecord *pRecord = FindModeRecord( kDamageTable.szName );
				if( pRecord )
				{
					pRecord->iTotalDamage += kDamageTable.iDamage;
				}
			}
		}

		std::sort( vDamageList.begin(), vDamageList.end(), DamageTableSort() );

		szBestAttacker = vDamageList[0].szName;
	}

	if( GetState() == MS_PLAY )
	{
		UpdateWeaponDieRecord( pUser, szAttacker, szBestAttacker );
	}

	float fLastRate = 0.0f;
	if( iTotalDamage > 0 )
	{
		fLastRate = (float)iLastDamage / iTotalDamage;
	}

	SP2Packet kReturn( STPK_PRISONER_DROP );
	kReturn << pUser->GetPublicID();
	kReturn << szAttacker;
	kReturn << fLastRate;
	GetCharModeInfo( kReturn, pUser->GetPublicID() );
	GetCharModeInfo( kReturn, szAttacker );
	SendRoomAllUser( kReturn );	
}

void SuccessionMode::OnPrisonerMode( User *pUser, SP2Packet &rkPacket )
{
	ioHashString szPrisoner, szLastAttacker, szLastAttackerSkill;
	Vector3 vPos;
	rkPacket >> szPrisoner >> szLastAttacker >> szLastAttackerSkill;
	rkPacket >> vPos;

	SuccessionRecord *pPrisoner = FindSuccessionRecord( szPrisoner );
	if( !pPrisoner ) return;
	if( pPrisoner->bPrisoner ) return;
	if( pPrisoner->pUser->IsEquipedItem() ) return;

	pPrisoner->dwCurDieTime = 0;
	pPrisoner->bPrisoner = true;

	if( pUser->GetTeam() == TEAM_RED )
		m_iBlueCatchRedPlayer++;
	else if( pUser->GetTeam() == TEAM_BLUE )
		m_iRedCatchBluePlayer++;
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Catch::OnPrisonerMode() - %s has not Team",
			szPrisoner.c_str() );
		return;
	}

	int iDamageCnt;
	ioHashString szBestAttacker;
	rkPacket >> iDamageCnt;
	if( iDamageCnt > 0 )
	{
		DamageTableList vDamageList;
		vDamageList.reserve( iDamageCnt );

		for( int i=0; i < iDamageCnt; ++i )
		{
			DamageTable kDamageTable;
			rkPacket >> kDamageTable.szName;
			rkPacket >> kDamageTable.iDamage;

			vDamageList.push_back( kDamageTable );

			if( kDamageTable.iDamage > 0 )
			{
				ModeRecord *pRecord = FindModeRecord( kDamageTable.szName );
				if( pRecord )
				{
					pRecord->iTotalDamage += kDamageTable.iDamage;
				}
			}
		}

		std::sort( vDamageList.begin(), vDamageList.end(), DamageTableSort() );

		szBestAttacker = vDamageList[0].szName;
	}

	bool bFirstPrisoner = pPrisoner->bFirstPrisoner;
	if( GetState() == MS_PLAY )
	{
		if( !pPrisoner->bFirstPrisoner )
		{
			pPrisoner->bFirstPrisoner = true;
			UpdateWeaponDieRecord( pUser, szLastAttacker, szBestAttacker );
		}
	}

	SP2Packet kPacket( STPK_PRISONERMODE );
	kPacket << pPrisoner->pUser->GetPublicID();
	kPacket << szLastAttacker;
	kPacket << szLastAttackerSkill;
	kPacket << bFirstPrisoner;
	kPacket << vPos;
	GetCharModeInfo( kPacket, pPrisoner->pUser->GetPublicID() );
	GetCharModeInfo( kPacket, szLastAttacker );
	SendRoomAllUser( kPacket );	
}

void SuccessionMode::OnRequestRevenge( User *pUser, SP2Packet &rkPacket )
{
	if( m_bReserveRevengeMatch )
		return;


	//HRYOON 시간세팅 
	if( pUser )
	{
		pUser->GetUserMatch()->SetReqTime();
	}

	int iType = 0;
	DWORD dwUserIndex;
	PACKET_GUARD_VOID( rkPacket.Read(dwUserIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iType) );

	SP2Packet kRequest( STPK_SUCCESSION_REQUEST_REVENGE );
	PACKET_GUARD_VOID( kRequest.Write(iType) );

	for( int i=0; i<GetRecordCnt(); ++i )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord )
			continue;

		if( !pRecord->pUser )
			continue;

		if( pRecord->pUser->GetUserIndex() == dwUserIndex )
			continue;

		pRecord->pUser->SendMessage( kRequest );
	}

	if( iType == REVENGE_ACCEPT )
	{
		m_bReserveRevengeMatch = true;

		DWORDVec vUserIndex;
		for( int i=0; i<GetRecordCnt(); ++i )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord )
				continue;

			if( !pRecord->pUser )
				continue;

			vUserIndex.push_back( pRecord->pUser->GetUserIndex() );
		}

		if( vUserIndex.size() == 2 )
		{
			g_MatchManager.MatchEnterRoom( vUserIndex[0], vUserIndex[1], true );
		}
	}
}

void SuccessionMode::UpdateRoundRecord()
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

void SuccessionMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SuccessionMode::OnEventSceneEnd - %s Not Exist Record",
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

int SuccessionMode::GetCurTeamUserCnt( TeamType eTeam )
{
	int iUserCnt = 0;
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			if( pRecord->eState == RS_VIEW || pRecord->eState == RS_OBSERVER )
				continue;

			User *pUser = pRecord->pUser;

			if( pUser && pUser->GetTeam() == eTeam )
				iUserCnt++;
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SuccessionMode::GetCurTeamUserCnt() - User's ModeRecord is Not Exist(%d).", i );
		}
	}

	return iUserCnt;
}

bool SuccessionMode::CheckRoundJoin( User *pSend )
{
	if( m_ModeState != MS_RESULT && m_ModeState != MS_RESULT_WAIT )
		return false;

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SuccessionMode::CheckRoundJoin - %s Not Exist Record",
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

	return true;
}

void SuccessionMode::CheckLeaveMatch( User* pUser )
{
	if( !pUser ) return;

	if( IsRoundSetEnd() && GetState() == MS_RESULT )
	{
		return;
	}

	int iPrePoint = pUser->GetSuccessionMMR();
	pUser->SuccessionModeLose();
	pUser->DelSuccessionMMR( m_iLeaveLosePoint );

	pUser->DelSuccessionRankMMR( m_iLeaveLosePoint );

	pUser->SaveSuccessionData();
	pUser->SendSuccessionData();
	LOG.PrintTimeAndLog( 0, "[info][1v1] %s-lose : %d >> %d(-%d)", pUser->GetPublicID().c_str(), iPrePoint, pUser->GetSuccessionMMR(), m_iLeaveLosePoint );
}

void SuccessionMode::SetTeam( DWORD dwBlueUserIndex, DWORD dwRedUserIndex )
{
	m_dwBlueUserIndex = dwBlueUserIndex;
	m_dwRedUserIndex = dwRedUserIndex;
}

TeamType SuccessionMode::GetTeamTypeByUserIndex( DWORD dwUserIndex )
{
	if( m_dwBlueUserIndex == dwUserIndex )
		return TEAM_BLUE;
	else if( m_dwRedUserIndex == dwUserIndex )
		return TEAM_RED;

	return TEAM_NONE;
}

void SuccessionMode::FinalRoundProcess()
{
	if( !m_bRoundSetEnd )	return;

	m_iMaxRound = m_iCurRound;

	//
	if( !IsRoomInfoLog() )
	{
		SetModeEndDBLog( m_pCreator, LogDBClient::PRT_END_SET );
	}
	else
	{
		SetRoomInfoLog( true );
	}

	UpdateBattleRoomRecord();
	UpdateLadderBattleRecord();
	UpdateTournamentRecord();
	UpdateShuffleRoomRecord();

	// 결과 승무패 및 경험치, 페소 지급
	float fTotalVictoriesRate = GetTotalVictoriesRate();
	float fTotalConsecutivelyRate = GetTotalModeConsecutivelyRate();

	CalculateMMR();

	int iRecordCnt = GetRecordCnt();
	for(int i = 0 ; i < iRecordCnt; i++ )
	{
		if( IsPlayingUser(i) )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord )
				continue;

			if( !pRecord->pUser )
				continue;

			bool bAbuseUser = IsAbuseUser( i );

			FinalRoundPoint( pRecord, bAbuseUser, fTotalVictoriesRate, fTotalConsecutivelyRate );
		}
	}

	// 결과 전송
	DWORD dwServerDate = Help::ConvertCTimeToDate( CTime::GetCurrentTime() );
	for(int i = 0; i < iRecordCnt ; i++ )
	{
		if( IsPlayingUser(i) )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord )
				continue;

			if( !pRecord->pUser )
				continue;

			FinalRoundResult( pRecord, dwServerDate );	
		}
	}
}

void SuccessionMode::FinalRoundResult( ModeRecord *pRecord, DWORD dwServerDate )
{	
	if( !pRecord || !pRecord->pUser )
		return;

	User *pUser = pRecord->pUser;

	SP2Packet kPacket( STPK_FINAL_ROUND_RESULT );

	// 최종 결과 시간( 결과 브리핑 + 시상식 + 최종 결과 )
	PACKET_GUARD_VOID( kPacket.Write(m_dwFinalResultStateTime + m_dwAwardingTime) );

	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		//각 팀의 랭킹 전송
		m_pCreator->FillLadderTeamRank( kPacket );
		//각 팀의 길드 보너스 전송
		PACKET_GUARD_VOID( kPacket.Write(GetLadderGuildTeamBonus(TEAM_BLUE)) );
		PACKET_GUARD_VOID( kPacket.Write(GetLadderGuildTeamBonus(TEAM_RED)) );
		//래더전 경험치
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetHeroExp()) );
	}

	pUser->FillFinalRoundResult( m_pCreator->GetRoomStyle(), GetModeType(), kPacket );

	// 용병 획득 경험치
	int iExpCharSize = pRecord->iResultClassTypeList.size();

	PACKET_GUARD_VOID( kPacket.Write(iExpCharSize) );
	for( int i=0; i<iExpCharSize; ++i )
	{
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iResultClassTypeList[i]) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iResultClassPointList[i]) );
	}

	// 내 정보.
	int iBonusPeso = g_MatchManager.GetPesoBonus( pUser->GetSuccessionWinCount() );
	PACKET_GUARD_VOID( kPacket.Write(iBonusPeso) );
	pUser->AddMoney( iBonusPeso );

	PACKET_GUARD_VOID( kPacket.Write(pUser->GetMoney()) );
	for( int i=0; i<BA_MAX; ++i )
	{
		PACKET_GUARD_VOID( kPacket.Write(pRecord->fBonusArray[i]) );
	}

	// 플레이중인 유저 결과 정보 
	int iPlayUserCnt = 0;
	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; ++i )
	{
		ModeRecord *pResultRecord = FindModeRecord( i );
		if( !pResultRecord || pResultRecord->eState == RS_LOADING )
			continue;

		iPlayUserCnt++;
	}

	PACKET_GUARD_VOID( kPacket.Write(iPlayUserCnt) );
	for( int i=0 ; i<iRecordCnt ; ++i )
	{
		ModeRecord *pResultRecord = FindModeRecord( i );
		if( !pResultRecord || pResultRecord->eState == RS_LOADING )
			continue;		

		//( 아이디 - 경험치 - 페소 - 레벨업 - 레벨 - 현재 래더 포인트 - 획득 래더포인트 )
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->pUser->GetPublicID()) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->iTotalExp) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->iTotalPeso) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->bResultLevelUP) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->pUser->GetGradeLevel()) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->pUser->GetLadderPoint()) );
		PACKET_GUARD_VOID( kPacket.Write(pResultRecord->iTotalLadderPoint) );
	}

	// 다음 모드 모름
	PACKET_GUARD_VOID( kPacket.Write(false) );

	int iRewardSpirit = g_SpiritManager.GetModeReward( GetModeType() );
	int iRewardSpiritQt = g_SpiritManager.GetModeRewardQuantity();
	PACKET_GUARD_VOID( kPacket.Write(iRewardSpirit) );
	PACKET_GUARD_VOID( kPacket.Write(iRewardSpiritQt) );
	pRecord->pUser->SetModeRewardSpirit( iRewardSpirit, iRewardSpiritQt, MT_SUCCESSION );

	pUser->SendMessage( kPacket );
}

void SuccessionMode::CalculateMMR()
{
	SuccessionRecord* pUserRecord1 = static_cast<SuccessionRecord*>( FindModeRecord( 0 ) );
	SuccessionRecord* pUserRecord2 = static_cast<SuccessionRecord*>( FindModeRecord( 1 ) );

	if( pUserRecord1 && !pUserRecord2 )
	{
		// 두번째 유저가 이탈했다.
		User *pUser = pUserRecord1->pUser;
		if( pUser )
		{
			int iPrePoint = pUser->GetSuccessionMMR();
			pUser->SuccessionModeWin();
			pUser->AddSuccessionMMR( m_iLeaveWinPoint );
			
			pUser->AddSuccessionRankMMR( m_iLeaveWinPoint );

			pUser->SaveSuccessionData();
			pUser->SendSuccessionData();

			LOG.PrintTimeAndLog( 0, "[info][1v1] %s-win : %d >> %d(+%d)", pUser->GetPublicID().c_str(), iPrePoint, pUser->GetSuccessionMMR(), m_iLeaveWinPoint );
		}

		return;
	}
	else if( !pUserRecord1 && pUserRecord2 )
	{
		// 첫번째 유저가 이탈했다.
		User *pUser = pUserRecord2->pUser;
		if( pUser )
		{
			int iPrePoint = pUser->GetSuccessionMMR();
			pUser->SuccessionModeWin();
			pUser->AddSuccessionMMR( m_iLeaveWinPoint );

			pUser->AddSuccessionRankMMR( m_iLeaveWinPoint );

			pUser->SaveSuccessionData();
			pUser->SendSuccessionData();

			LOG.PrintTimeAndLog( 0, "[info][1v1] %s-win : %d >> %d(+%d)", pUser->GetPublicID().c_str(), iPrePoint, pUser->GetSuccessionMMR(), m_iLeaveWinPoint );
		}

		return;
	}
	else if( !pUserRecord1 && !pUserRecord2 )
	{
		// 둘다 이탈했다.
		return;
	}

	User *pUser1 = pUserRecord1->pUser;
	User *pUser2 = pUserRecord2->pUser;
	if( !pUser1 || !pUser2 )
		return;

	float fUser1WinRate( 0.0f );
	float fUser2WinRate( 0.0f );

	if( g_MatchManager.UserELO() )
	{
		fUser1WinRate = 1.0f / ( 1.0f  + pow( 10.0f, ( (float)pUser2->GetSuccessionMMR() - (float)pUser1->GetSuccessionMMR() ) / 400.0f ) );
		fUser2WinRate = 1.0f / ( 1.0f  + pow( 10.0f, ( (float)pUser1->GetSuccessionMMR() - (float)pUser2->GetSuccessionMMR() ) / 400.0f ) );
	}

	// 로그용
	int iPrePoint1 = pUser1->GetSuccessionMMR();
	int iPrePoint2 = pUser2->GetSuccessionMMR();

	float fWinnerPoint = 0.5f;
	float fLoserPoint = 0.5f;

	if( pUser1->GetTeam() == GetWinTeam() )
	{
		// 소수점 반올림
		if( g_MatchManager.UserELO() )
		{
			fWinnerPoint = (float)m_iBasePoint * ( 1.0f - fUser1WinRate ) + 0.5f;
			fLoserPoint = (float)m_iBasePoint * fUser2WinRate + 0.5f;
		}
		else
		{
			fWinnerPoint = (float)g_MatchManager.GetWinPoint();
			fLoserPoint = (float)g_MatchManager.GetLosePoint();
		}

		pUser1->SuccessionModeWin();
		pUser1->AddSuccessionMMR( (int)fWinnerPoint );
		pUser1->AddSuccessionRankMMR( (int)fWinnerPoint );

		pUser2->SuccessionModeLose();
		pUser2->DelSuccessionMMR( (int)fLoserPoint );
		pUser2->DelSuccessionRankMMR( (int)fLoserPoint );

		LOG.PrintTimeAndLog( 0, "[info][1v1] %s-win : %d >> %d(+%d) (%.3f)", pUser1->GetPublicID().c_str(), iPrePoint1, pUser1->GetSuccessionMMR(), (int)fWinnerPoint, fUser1WinRate );
		LOG.PrintTimeAndLog( 0, "[info][1v1] %s-lose : %d >> %d(%d) (%.3f)", pUser2->GetPublicID().c_str(), iPrePoint2, pUser2->GetSuccessionMMR(), (int)fLoserPoint, fUser2WinRate );
	}
	else
	{
		// 소수점 반올림
		if( g_MatchManager.UserELO() )
		{
			fWinnerPoint = (float)m_iBasePoint * ( 1.0f - fUser2WinRate ) + 0.5f;
			fLoserPoint = (float)m_iBasePoint * fUser1WinRate + 0.5f;
		}
		else
		{
			fWinnerPoint = (float)g_MatchManager.GetWinPoint();
			fLoserPoint = (float)g_MatchManager.GetLosePoint();
		}

		pUser1->SuccessionModeLose();
		pUser1->DelSuccessionMMR( (int)fLoserPoint );
		pUser1->DelSuccessionRankMMR( (int)fLoserPoint );

		pUser2->SuccessionModeWin();
		pUser2->AddSuccessionMMR( (int)fWinnerPoint );
		pUser2->AddSuccessionRankMMR( (int)fWinnerPoint );

		LOG.PrintTimeAndLog( 0, "[info][1v1] %s-win : %d >> %d(+%d) (%.3f)", pUser2->GetPublicID().c_str(), iPrePoint2, pUser2->GetSuccessionMMR(), (int)fWinnerPoint, fUser2WinRate );
		LOG.PrintTimeAndLog( 0, "[info][1v1] %s-lose : %d >> %d(%d) (%.3f)", pUser1->GetPublicID().c_str(), iPrePoint1, pUser1->GetSuccessionMMR(), (int)fLoserPoint, fUser1WinRate );
	}

	// db update
	pUser1->SaveSuccessionData();
	pUser1->SendSuccessionData();

	pUser2->SaveSuccessionData();
	pUser2->SendSuccessionData();
}