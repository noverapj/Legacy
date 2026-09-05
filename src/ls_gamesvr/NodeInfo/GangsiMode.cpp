#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "Room.h"
#include "RoomNodeManager.h"
#include "GangsiMode.h"
#include "ioSetItemInfoManager.h"
#include "ioItemInfoManager.h"
#include "ioSetItemInfo.h"
#include "../EtcHelpFunc.h"
#include "../DataBase/LogDBClient.h"

#include "ShuffleRoomNode.h"

GangsiMode::GangsiMode( Room *pCreator ) : Mode( pCreator )
{	
	m_SingleTeamPosArray.clear();
}

GangsiMode::~GangsiMode()
{
}

void GangsiMode::LoadINIValue()
{
	Mode::LoadINIValue();

	m_dwCurRoundDuration = m_dwRoundDuration;
}

void GangsiMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "info" );
	m_iMaxPlayer = rkLoader.LoadInt( "max_player", MAX_PLAYER );
	if( m_iMaxPlayer > MAX_PLAYER )
		m_iMaxPlayer = MAX_PLAYER;

	rkLoader.SetTitle( "round" );
	m_iMaxRound = 1;

	m_dwRoundDuration   = rkLoader.LoadInt( "round_time", 300000 );

	m_dwReadyStateTime  = rkLoader.LoadInt( "ready_state_time", 4000 );
	m_dwSuddenDeathTime = rkLoader.LoadInt( "sudden_death_time", 60000 );
	m_dwResultStateTime = rkLoader.LoadInt( "result_state_time", 7000 );
	m_dwFinalResultWaitStateTime = rkLoader.LoadInt( "final_result_wait_state_time", 10000 );
	m_dwFinalResultStateTime = rkLoader.LoadInt( "final_result_state_time", 13000 );
	m_dwTournamentRoomFinalResultAddTime = rkLoader.LoadInt( "tournament_room_final_result_time", 10000 );
	m_dwTimeClose       = rkLoader.LoadInt("room_close_time", 10000 );

	m_bUseViewMode = rkLoader.LoadBool( "use_view_mode", false );
	m_dwViewCheckTime = rkLoader.LoadInt( "view_check_time", 30000 );

	m_iAbuseMinDamage = rkLoader.LoadInt( "abuse_min_damage", 1 );

	m_iScoreRate = rkLoader.LoadInt( "win_team_score_rate", 1 );

	m_dwRoomExitTime = rkLoader.LoadInt( "exit_room_time", 5000 );

	m_dwCharLimitCheckTime = rkLoader.LoadInt( "char_limit_check_min", 60000 );

	// 강시
	int i = 0;
	m_HostGangsiItemCode.clear();
	m_InfectionGangsiItemCode.clear();
	rkLoader.SetTitle( "gangsi" );
	int iMaxItem = rkLoader.LoadInt( "max_host_item", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "host_item_%d", i + 1 );
		m_HostGangsiItemCode.push_back( rkLoader.LoadInt( szKey, 0 ) );
	}

	iMaxItem = rkLoader.LoadInt( "max_infection_item", 0 );
	for(i = 0;i < iMaxItem;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s( szKey, "infection_item_%d", i + 1 );
		m_InfectionGangsiItemCode.push_back( rkLoader.LoadInt( szKey, 0 ) );
	}

	// 라운드만큼 버퍼 생성.
	m_vRoundHistory.clear();
	for(i = 0; i < m_iMaxRound;i++)
	{
		RoundHistory rh;
		m_vRoundHistory.push_back( rh );
	}

	SetStartPosArray();
}

void GangsiMode::SetStartPosArray()
{
	m_SingleTeamPosArray.reserve( MAX_PLAYER );
	for( int i=0; i<MAX_PLAYER; i++ )
	{
		m_SingleTeamPosArray.push_back(i);
	}

	io_random_shuffle( m_SingleTeamPosArray.begin(), m_SingleTeamPosArray.end() );

	m_iBluePosArray = m_SingleTeamPosArray[0];
	m_iRedPosArray  = m_SingleTeamPosArray[1];
}

void GangsiMode::DestroyMode()
{
	Mode::DestroyMode();
	m_vRecordList.clear();
}

void GangsiMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "gangsi%d_object_group%d", iSubNum, iGroupNum );
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



void GangsiMode::AddNewRecord( User *pUser )
{
	GangsiRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	UpdateUserRank();
}

void GangsiMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	if(!pUser) return;
	
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

void GangsiMode::ProcessReady()
{
	DWORD dwCurTime = TIMEGETTIME();
	if( m_dwStateChangeTime + m_dwReadyStateTime >= dwCurTime )
		return;
	// 유저 입장 대기중이면 플레이 상태로 전환하지 않는다.
	if( m_pCreator->IsRoomEnterUserDelay() )
		return;

	SP2Packet kPacket( STPK_ROUND_START );
	kPacket << m_iCurRound;
	SendRoomPlayUser( kPacket );
	SetModeState( MS_PLAY );

	// 라운드가 시작될 때 유저 수를 확인
	m_iReadyBlueUserCnt = max( m_iReadyBlueUserCnt, GetTeamUserCnt( TEAM_BLUE ) );
	m_iReadyRedUserCnt  = max( m_iReadyRedUserCnt, GetTeamUserCnt( TEAM_RED ) );

	// 길드전 라운드 시작될 때 레벨 확인
	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		m_iReadyBlueGuildLevel = max( m_iReadyBlueGuildLevel, m_pCreator->GetLadderTeamLevel( TEAM_BLUE ) );
		m_iReadyRedGuildLevel  = max( m_iReadyRedGuildLevel, m_pCreator->GetLadderTeamLevel( TEAM_RED ) );
	}

	// 강시 유저가 있는지 확인
	CheckGangsiToRandomGangsi();

	// 레디 상태에서 이탈한 유저에 대한 체크
	CheckUserLeaveEnd();
}

void GangsiMode::ProcessPlay()
{
	ProcessRevival();
	CheckFieldItemLiveTime();
	CheckItemSupply( m_dwStateChangeTime );
	CheckBallSupply( m_dwStateChangeTime );
	CheckMachineSupply( m_dwStateChangeTime );
	CheckNeedSendPushStruct();
	CheckRoundEnd( true );
	ProcessEvent();
	ProcessBonusAlarm();
}

void GangsiMode::CheckRoundEnd( bool bProcessCall )
{
	if( !bProcessCall && m_pCreator->GetPlayUserCnt() < 2 )
	{
		m_dwCurRoundDuration = 0;

		WinTeamType eWinTeam = WTT_DRAW;
		if( GetTeamUserCnt( TEAM_BLUE ) == 0 )
		{
			eWinTeam = WTT_RED_TEAM;
			m_iBlueTeamWinCnt = 0;
			m_iRedTeamWinCnt  = 3;    // 3 vs 0으로 인간 승리
		}
		else if( GetTeamUserCnt( TEAM_RED ) == 0 )
		{	
			eWinTeam = WTT_BLUE_TEAM;
			m_iBlueTeamWinCnt = 0;
			m_iRedTeamWinCnt  = 0;    // 0 vs 0으로 강시 승리
		}

		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
		return;
	}

	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;
	if( m_dwCurRoundDuration < dwGapTime )
	{
		// 시간제로 종료되면 레드팀 승리
		WinTeamType eWinTeam = WTT_RED_TEAM;
		m_iBlueTeamWinCnt = 0;
		m_iRedTeamWinCnt  = 3;    // 3 vs 0으로 인간 승리
		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
	}
	else
	{
		bool bGangsiAllDie = true;
		bool bUserAllDie   = true;
		int iRecordCnt = GetRecordCnt();
		for(int i = 0;i < iRecordCnt;i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord || !pRecord->pUser ) continue;
			if( pRecord->eState == RS_VIEW ) continue;
			if( pRecord->pUser->GetTeam() == TEAM_BLUE )
			{
				if( !pRecord->bDieState )
				{
					bGangsiAllDie = false;				
				}
			}
			else if( pRecord->pUser->GetTeam() == TEAM_RED )
			{
				if( !pRecord->bDieState )
				{
					bUserAllDie = false;				
				}
			}
		}

		int iUserCount = GetCurTeamUserCnt( TEAM_RED );
		if( bGangsiAllDie || iUserCount == 0 || bUserAllDie )
		{
			WinTeamType eWinTeam = WTT_DRAW;
			// 강시가 모두 죽으면 레드팀 승리
			if( bGangsiAllDie )
			{	
				eWinTeam = WTT_RED_TEAM;
				m_iBlueTeamWinCnt = 0;
				m_iRedTeamWinCnt  = 3;    // 3 vs 0으로 인간 승리
			}
			else if( bUserAllDie || iUserCount == 0 )  // 인간이 모두 죽으면
			{	
				eWinTeam = WTT_BLUE_TEAM;
				m_iBlueTeamWinCnt = 0;
				m_iRedTeamWinCnt  = 0;    // 0 vs 0으로 강시 승리
			}
			SetRoundEndInfo( eWinTeam );
			SendRoundResult( eWinTeam );
		}
	}
}

void GangsiMode::SetRoundEndInfo( WinTeamType eWinTeam )
{
	m_CurRoundWinTeam = eWinTeam;
	m_bRoundSetEnd = true;
	m_bCheckContribute = false;
	m_bCheckAwardChoose = false;
	m_bCheckSuddenDeathContribute = false;
	SetModeState( MS_RESULT_WAIT );

	UpdateRoundRecord();

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
	}

	ClearObjectItem();
}

void GangsiMode::UpdateRoundRecord()
{
	int iRecordCnt = GetRecordCnt();
	for(int i=0;i<iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( pRecord )
		{
			if( pRecord->pUser )
			{
				pRecord->pUser->UpdateCharLimitDate();
				pRecord->pUser->UpdateEtcItemTime( __FUNCTION__ );
				pRecord->pUser->DeleteEtcItemPassedDate();
				pRecord->pUser->DeleteExtraItemPassedDate( true );
				pRecord->pUser->DeleteMedalItemPassedDate( true );
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
}

ModeType GangsiMode::GetModeType() const
{
	return MT_GANGSI;
}

void GangsiMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck )
{
	GangsiRecord *pRecord = FindGangsiRecord( szName );
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
			rkPacket << pRecord->bHostGangsi << pRecord->bInfectionGangsi;
		}
	}
	else
	{
		// 레코드 정보 유무
		rkPacket << false;
	}
}

int GangsiMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* GangsiMode::GetModeINIFileName() const
{
	return "config/gangsimode.ini";
}

void GangsiMode::GetModeInfo( SP2Packet &rkPacket )
{
	rkPacket << GetModeType();
	rkPacket << m_iCurRound;
	rkPacket << m_iMaxRound;

	rkPacket << m_dwRoundDuration;

	int iPosCnt = m_SingleTeamPosArray.size();
	rkPacket << iPosCnt;

	for( int i=0; i < iPosCnt; ++i )
		rkPacket << m_SingleTeamPosArray[i];
}

TeamType GangsiMode::GetNextTeamType()
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GangsiMode::GetNextTeamType Call Error!!" );
	return TEAM_NONE;
}

int GangsiMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
{
	ModeRecord *pKickRecord = FindModeRecord( szKickUserName );
	if( !pKickRecord || !pKickRecord->pUser )
		return USER_KICK_VOTE_PROPOSAL_ERROR_7;

	// 인원 체크 
	if( !pKickRecord->pUser->IsObserver() )
	{
		int iPlayUserCount = 0;
		int iRecordCnt = GetRecordCnt();
		for(int i = 0;i < iRecordCnt;i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord || !pRecord->pUser ) continue;
			if( pRecord->pUser->IsObserver() ) continue;
			if( pRecord->pUser->IsStealth() ) continue;

			iPlayUserCount++;
		}
		if( iPlayUserCount <= m_KickOutVote.GetKickVoteUserPool() )
			return USER_KICK_VOTE_PROPOSAL_ERROR_11;
	}

	// 라운드 or 시간 체크
	if( IsRoundSetEnd() )
		return USER_KICK_VOTE_PROPOSAL_ERROR_9;

	// 시간 체크
	DWORD dwGapTime = TIMEGETTIME() - m_dwModeStartTime;
	if( dwGapTime > m_KickOutVote.GetKickVoteRoundTime() )
		return USER_KICK_VOTE_PROPOSAL_ERROR_9;

	return 0;
}

ModeRecord* GangsiMode::FindModeRecord( const ioHashString &rkName )
{
	if( rkName.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetPublicID() == rkName )
		{
			return &m_vRecordList[i];
		}
	}

	return NULL;
}

ModeRecord* GangsiMode::FindModeRecord( User *pUser )
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

ModeRecord* GangsiMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

GangsiRecord* GangsiMode::FindGangsiRecord( const ioHashString &rkName )
{
	return (GangsiRecord*)FindModeRecord( rkName );
}

GangsiRecord* GangsiMode::FindGangsiRecord( User *pUser )
{
	return (GangsiRecord*)FindModeRecord( pUser );
}

GangsiRecord* GangsiMode::FindGangsiRecordByUserID( const ioHashString &rkUserID )
{
	if( rkUserID.IsEmpty() )	return NULL;

	CRASH_GUARD();
	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetPublicID() == rkUserID )
		{
			return &m_vRecordList[i];
		}
	}

	return NULL;
}

int GangsiMode::GetCurTeamUserCnt( TeamType eTeam )
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
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GangsiMode::GetCurTeamUserCnt() - User's ModeRecord is Not Exist(%d).", i );
		}
	}

	return iUserCnt;
}

void GangsiMode::UpdateUserDieTime( User *pDier )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( pDieRecord && pDieRecord->bDieState )
	{
		if( pDier && pDier->GetTeam() == TEAM_RED )
		{
			DWORD dwRevivalGap = (DWORD)GetRevivalGapTime( pDieRecord->iRevivalCnt );
			pDieRecord->dwCurDieTime = TIMEGETTIME();
			pDieRecord->dwRevivalGap = dwRevivalGap;
			pDieRecord->iRevivalCnt++;
		}
		else
		{
			// 강시는 부활 안함.
			pDieRecord->dwCurDieTime = 0;
			pDieRecord->dwRevivalGap = 0;
			pDieRecord->iRevivalCnt++;
		}
	}
}
void GangsiMode::UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	// 죽음 기록하지 않음
}

void GangsiMode::UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	// 죽음 기록하지 않음
}

void GangsiMode::UpdateUserDieNextProcess( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	Mode::UpdateUserDieNextProcess( pDier, szAttacker, szBestAttacker );
	/*
	if( GetState() != Mode::MS_PLAY ) return;

	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( !pDieRecord )	return;

	// 드랍으로 죽은 유저는 좀비가 된다.
	if( pDieRecord->pUser )
	{
		pDieRecord->pUser->SetTeam( TEAM_BLUE );
	}
	CheckRoundEnd( true );
	*/
}

void GangsiMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd )
		return;

	if( m_pCreator->GetPlayUserCnt() < 2 )
		CheckRoundEnd( false );
	else 
	{
		CheckGangsiToRandomGangsi();
	}
}

void GangsiMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
{
	bool bRoundChange;
	int iRoomIndex;
	rkPacket >> bRoundChange >> iRoomIndex;
	if( iRoomIndex != m_pCreator->GetRoomIndex() )
		return;

	if( !bRoundChange && !IsZeroHP() )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "GangsiMode::OnEventSceneEnd - %s Not Exist Record",
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
		pRecord->pUser->StartCharLimitDate( GetCharLimitCheckTime(), __FILE__, __LINE__ );

		SP2Packet kPacket( STPK_ROUND_JOIN );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << GetSelectCharTime();
		kPacket << m_dwCurRoundDuration;
		SendRoomAllUser( kPacket );
	}
}

void GangsiMode::OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket )
{
	if( pDieUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Mode::OnWeaponDieUser None User!!" );
		return;
	}

	// 유저가 죽은 위치.
	float fDiePosX = 0.0f, fDiePosZ = 0.0f;
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosX) );
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosZ) );

	// Killer 유저 정보.
	int iLastAttackerTeam = 0;
	ioHashString szLastAttackerName;
	ioHashString szLastAttackerSkillName;
	DWORD dwLastAttackerWeaponItemCode = 0;
	PACKET_GUARD_VOID( rkPacket.Read(szLastAttackerName) );
	PACKET_GUARD_VOID( rkPacket.Read(szLastAttackerSkillName) );
	PACKET_GUARD_VOID( rkPacket.Read(dwLastAttackerWeaponItemCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iLastAttackerTeam) );

	ModeRecord *pRecord = FindModeRecord( pDieUser );
	if( !pRecord ) return;

	if( pRecord->eState == RS_LOADING ) return;

	//강시모드에서는 아이템이 착용중이여도 KO된다. 
	//if( pRecord->pUser->IsEquipedItem() ) return;

	pRecord->SetDieLastAttackerInfo( szLastAttackerName, (TeamType)iLastAttackerTeam, dwLastAttackerWeaponItemCode );

	UpdateDieState( pRecord->pUser );

	int iDamageCnt = 0;
	ioHashString szBestAttackerName;
	PACKET_GUARD_VOID( rkPacket.Read(iDamageCnt) );
	MAX_GUARD(iDamageCnt, 50);

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;

	if( iDamageCnt > 0 )
	{
		DamageTableList vDamageList;
		vDamageList.reserve( iDamageCnt );

		for( int i=0; i < iDamageCnt; ++i )
		{
			DamageTable kDamageTable;
			PACKET_GUARD_VOID( rkPacket.Read(kDamageTable.szName) );
			PACKET_GUARD_VOID( rkPacket.Read(kDamageTable.iDamage) );

			if( kDamageTable.szName == szLastAttackerName )
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

		szBestAttackerName = vDamageList[0].szName;
		iBestDamage = vDamageList[0].iDamage;
	}

	if( GetState() == Mode::MS_PLAY )
	{
		UpdateWeaponDieRecord( pRecord->pUser, szLastAttackerName, szBestAttackerName );
	} 

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = ((float)iLastDamage / iTotalDamage) * m_fLastAttackKillRecoveryRate;
		fBestRate = ((float)iBestDamage / iTotalDamage) * m_fBestAttackKillRecoveryRate;
	}

	SP2Packet kReturn( STPK_WEAPON_DIE );
	PACKET_GUARD_VOID( kReturn.Write(pRecord->pUser->GetPublicID()) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerSkillName) );
	PACKET_GUARD_VOID( kReturn.Write(dwLastAttackerWeaponItemCode) );
	PACKET_GUARD_VOID( kReturn.Write(iLastAttackerTeam) );
	PACKET_GUARD_VOID( kReturn.Write(szBestAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(fLastRate) );
	PACKET_GUARD_VOID( kReturn.Write(fBestRate) );
	GetCharModeInfo( kReturn, pRecord->pUser->GetPublicID() );
	GetCharModeInfo( kReturn, szLastAttackerName );
	m_pCreator->RoomSendPacketTcp( kReturn );

	UpdateUserDieNextProcess( pRecord->pUser, szLastAttackerName, szBestAttackerName );
}

/************************************************************************/
/* 강시는 죽지 않으므로 최소 한명은 꼭있어야만 게임이 진행된다.			*/
/************************************************************************/
void GangsiMode::CheckGangsiToRandomGangsi()
{
	int i = 0;
	int iRecordCnt = m_vRecordList.size();
	for(int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetTeam() == TEAM_BLUE )
		{
			return; // 강시 있음
		}
	}

	// 강시 없음
	ioHashStringVec vTeamList;
	for(int i=0 ; i<iRecordCnt ; i++ )
	{
		if( m_vRecordList[i].pUser->GetTeam() == TEAM_RED )
		{
			vTeamList.push_back( m_vRecordList[i].pUser->GetPublicID() );
		}
	}

	int iSize = vTeamList.size();
	if( iSize != 0 )
	{
		int r = rand()%iSize;
		GangsiRecord *pGangsiRecord = FindGangsiRecord( vTeamList[r] );
		if( pGangsiRecord && pGangsiRecord->pUser )
		{
			// 강시 변경 패킷 전송
			pGangsiRecord->pUser->SetTeam( TEAM_BLUE );

			SP2Packet kPacket( STPK_SELECT_GANGSI );
			pGangsiRecord->pUser->SendMessage( kPacket );  
		}
	}			
}

/************************************************************************/
/* 강시 유저가 착용하는 아이템 코드										*/
/************************************************************************/
int GangsiMode::GetGangsiItem( int iSlot )
{
	// 숙주 강시
	if( GetTeamUserCnt( TEAM_BLUE ) <= 1 )
	{
		int iMaxSlot = m_HostGangsiItemCode.size();
		if( !COMPARE( iSlot, 0, iMaxSlot ) ) return 0;

		return m_HostGangsiItemCode[iSlot];
	}
	
	// 간염된 강시
	int iMaxSlot = m_InfectionGangsiItemCode.size();
	if( !COMPARE( iSlot, 0, iMaxSlot ) ) return 0;

	return m_InfectionGangsiItemCode[iSlot];
}

/************************************************************************/
/* 숙주강시 /일반강시 처리												*/
/************************************************************************/
void GangsiMode::ChangeGangsiUser( User *pUser )
{
	if( !pUser ) return;

	GangsiRecord *pGangsiRecord = FindGangsiRecord( pUser );
	if( pGangsiRecord )
	{
		// 숙주 강시
		if( GetTeamUserCnt( TEAM_BLUE ) <= 1 )
		{
			pGangsiRecord->bHostGangsi = true;
			pGangsiRecord->bInfectionGangsi = false;
		}
		else	// 간염된 강시
		{
			pGangsiRecord->bHostGangsi = false;
			pGangsiRecord->bInfectionGangsi = true;
		}
	}	
}
