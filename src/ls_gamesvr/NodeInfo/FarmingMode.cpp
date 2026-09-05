

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "FarmingMode.h"

#include "Room.h"
#include "ModeHelp.h"
#include "RoomNodeManager.h"
#include "../DataBase/LogDBClient.h"

FarmingMode::FarmingMode( Room *pCreator ) : Mode( pCreator )
{
	m_iRedCatchBluePlayer = 0;
	m_iBlueCatchRedPlayer = 0;

	m_dwRoundTimeSendTime_TEMP = TIMEGETTIME();		// 2019-07-03 by bckim, 파밍모드 추가
	m_dwModeStartTime_TEMP = TIMEGETTIME();			// 2019-07-03 by bckim, 파밍모드 추가
}

FarmingMode::~FarmingMode()
{
}

void FarmingMode::LoadINIValue()
{
	Mode::LoadINIValue();


	// 2019-07-03 by bckim, 파밍모드 추가
	FirstItemDrop = true;

	Dispersion_Ratio = 0.0f;			// 아이템 드랍 포지션 범위 집중도 
	dwFirst_group_start_time = 0;

	dwSecond_group_start_time = 0;
	dwSecond_group_supply_item_term = 0;

	iFirst_item_group_count = 0;
	iSecond_item_group_count = 0;

	for(int i = 0; i<10; i++)
	{
		dwFirst_item_group[i] = 0;
		dwSecond_item_group[i] = 0;
	}

	LoadINISupplyItemBasicInfo();
	LoadINISupplyItemPos();
	// End. 2019-07-03 by bckim, 파밍모드 추가


	m_dwCurRoundDuration = m_dwRoundDuration;
	m_vRoundHistory.clear();
}

void FarmingMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();

	// 2019-07-03 by bckim, 파밍모드 추가
	m_mMapSupplyFirstItemPos_BLUE.clear();
	m_mMapSupplyFirstItemPos_RED.clear();
	m_mMapSupplyItemFixPos.clear();

	FirstItemDrop = true;
	// End. 2019-07-03 by bckim, 파밍모드 추가
}

void FarmingMode::InitObjectGroupList()
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

void FarmingMode::AddNewRecord( User *pUser )
{
	CatchRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	UpdateUserRank();
}

void FarmingMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
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
}

void FarmingMode::ProcessPlay()
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

	// 2019-07-03 by bckim, 파밍모드 추가
	if( m_bZeroHP ) return;

	if ( FirstItemDrop )
		FirstOnCreateFieldItem();
	else
		OnCreateFieldItem();
	// End. 2019-07-03 by bckim, 파밍모드 추가

}

void FarmingMode::RestartMode()
{
	m_dwCurRoundDuration = m_dwRoundDuration;
	m_bZeroHP = false;
		
	FirstItemDrop = true;	// 2019-07-03 by bckim, 파밍모드 추가
		
	m_iCurItemSupplyIdx = 0;
	m_iCurBallSupplyIdx = 0;

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		CatchRecord &rkRecord = m_vRecordList[i];
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

int FarmingMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
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

ModeRecord* FarmingMode::FindModeRecord( const ioHashString &rkName )
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

ModeRecord* FarmingMode::FindModeRecord( User *pUser )
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

ModeRecord* FarmingMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

CatchRecord* FarmingMode::FindCatchRecord( const ioHashString &rkName )
{
	return (CatchRecord*)FindModeRecord( rkName );
}

CatchRecord* FarmingMode::FindCatchRecord( User *pUser )
{
	return (CatchRecord*)FindModeRecord( pUser );
}

void FarmingMode::UpdateDieState( User *pDier )
{
	CatchRecord *pDieRecord = FindCatchRecord( pDier );
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

void FarmingMode::UpdateUserDieTime( User *pDier )
{
	CatchRecord *pDieRecord = FindCatchRecord( pDier );
	if( !pDieRecord ) return;
	if( !pDieRecord->bDieState ) return;

	DWORD dwRevivalGap = (DWORD)GetRevivalGapTime( pDieRecord->iRevivalCnt );
	pDieRecord->dwRevivalGap = dwRevivalGap;
	pDieRecord->iRevivalCnt++;
}

ModeType FarmingMode::GetModeType() const
{
	//return MT_UNDERWEAR;
	return MT_FARMING;
}

void FarmingMode::GetModeInfo( SP2Packet &rkPacket )
{
	Mode::GetModeInfo( rkPacket );

	rkPacket << m_iRedCatchBluePlayer;
	rkPacket << m_iBlueCatchRedPlayer;

	GetModeHistory( rkPacket );
}

void FarmingMode::GetModeHistory( SP2Packet &rkPacket )
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

void FarmingMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck )
{
	CatchRecord *pRecord = FindCatchRecord( rkName );
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

int FarmingMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* FarmingMode::GetModeINIFileName() const
{
	return "config/farmingmode.ini";
}

TeamType FarmingMode::GetNextTeamType()
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

void FarmingMode::CheckRoundEnd( bool bProcessCall )
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

void FarmingMode::SetRoundEndInfo( WinTeamType eWinTeam )
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


	// 2019-07-03 by bckim, 파밍모드 추가
	m_dwRoundTimeSendTime_TEMP = TIMEGETTIME();
	m_dwModeStartTime_TEMP = TIMEGETTIME();
	// End.2019-07-03 by bckim, 파밍모드 추가

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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FarmingMode::SetRoundEndInfo() m_iCurRound != m_vRoundHistory.size()" );
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

bool FarmingMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
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
	}

	return false;
}

void FarmingMode::OnPrisonerEscape( User *pUser, SP2Packet &rkPacket )
{
	ioHashString szName, szLastAttacker, szLastAttackerSkill;	
	rkPacket >> szName >> szLastAttacker >> szLastAttackerSkill;

	CatchRecord *pEscape = FindCatchRecord( szName );
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FarmingMode::OnPrisonerEscape() - %s has not Team.",
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

void FarmingMode::OnPrisonerDrop( User *pUser, SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	CatchRecord *pEscape = FindCatchRecord( pUser );
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

void FarmingMode::OnPrisonerMode( User *pUser, SP2Packet &rkPacket )
{
	ioHashString szPrisoner, szLastAttacker, szLastAttackerSkill;
	Vector3 vPos;
	rkPacket >> szPrisoner >> szLastAttacker >> szLastAttackerSkill;
	rkPacket >> vPos;

	CatchRecord *pPrisoner = FindCatchRecord( szPrisoner );
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

void FarmingMode::UpdateRoundRecord()
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

void FarmingMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FarmingMode::OnEventSceneEnd - %s Not Exist Record",
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

int FarmingMode::GetCurTeamUserCnt( TeamType eTeam )
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
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FarmingMode::GetCurTeamUserCnt() - User's ModeRecord is Not Exist(%d).", i );
		}
	}

	return iUserCnt;
}

bool FarmingMode::CheckRoundJoin( User *pSend )
{
	if( m_ModeState != MS_RESULT && m_ModeState != MS_RESULT_WAIT )
		return false;

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "FarmingMode::CheckRoundJoin - %s Not Exist Record",
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

// 2019-07-03 by bckim, 파밍모드 추가
void FarmingMode::LoadINISupplyItemBasicInfo()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	char szKey[MAX_PATH] = "";
	rkLoader.SetTitle( "supply_item_basic_info" );
	Dispersion_Ratio = rkLoader.LoadFloat( "Dispersion_Ratio", 20.0f );

	// ;first_item_group  
	dwFirst_group_start_time = (DWORD)rkLoader.LoadInt( "first_group_start_time", 0 );	
	iFirst_item_group_count = (DWORD)rkLoader.LoadInt( "first_item_group_count", 0 );

	for( int i = 0; i < iFirst_item_group_count; i++ )
	{
		StringCbPrintf( szKey, sizeof( szKey ), "first_item%d_group", i+1);
		dwFirst_item_group[i] = (DWORD)rkLoader.LoadInt( szKey, 0 );
	}

	// ;second_item_group 
	dwSecond_group_start_time = (DWORD)rkLoader.LoadInt( "second_group_start_time", 0 );
	dwSecond_group_supply_item_term = (DWORD)rkLoader.LoadInt( "second_group_supply_item_term", 0 );
	iSecond_item_group_count = (DWORD)rkLoader.LoadInt( "second_item_group_count", 0 );

	for( int i = 0; i < iSecond_item_group_count; i++ )
	{
		StringCbPrintf( szKey, sizeof( szKey ), "second_item%d_group", i+1);
		dwSecond_item_group[i] = (DWORD)rkLoader.LoadInt( szKey, 0 );
	}
}

void FarmingMode::LoadINISupplyItemPos()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	char szKey[MAX_PATH] = "";
	///////////////////////////////////////////////////
	m_mMapSupplyFirstItemPos_BLUE.clear();
	m_mMapSupplyFirstItemPos_RED.clear();
	m_mMapSupplyItemFixPos.clear();

	rkLoader.SetTitle( "supply_item_pos" );
	int iSupply_map_cnt = rkLoader.LoadInt( "supply_map_cnt", 0 );

	for( int i = 0; i < iSupply_map_cnt; i++ )
	{		
		int iMapID	= 0;
		CoordinateInfo stInfo;
		StringCbPrintf( szKey, sizeof( szKey ), "map%d_id", i+1 );
		iMapID	= rkLoader.LoadInt( szKey, 0 );

		if( 0 == iMapID ) 
			break;

		////////////////////////m_mMapSupplyFirstItemPos_BLUE////////////////////
		for( int k = 0; k < 100; k++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "firstsupply%d_pos%d_blue_x", i+1, k+1 );
			stInfo.iX	= rkLoader.LoadInt( szKey, 0 );
			StringCbPrintf( szKey, sizeof( szKey ), "firstsupply%d_pos%d_blue_y", i+1, k+1 );
			stInfo.iY	= rkLoader.LoadInt( szKey, 0 );
			StringCbPrintf( szKey, sizeof( szKey ), "firstsupply%d_pos%d_blue_z", i+1, k+1 );
			stInfo.iZ	= rkLoader.LoadInt( szKey, 0 );

			if( 0 == stInfo.iX && 0 == stInfo.iY && 0 == stInfo.iZ )
				break;

			MAPSUPPLYITEMPOS::iterator it = m_mMapSupplyFirstItemPos_BLUE.find(iMapID);
			if( it != m_mMapSupplyFirstItemPos_BLUE.end() )
				(it->second).push_back(stInfo);
			else
			{
				COORDINATEINFO vVac;
				vVac.push_back(stInfo);
				m_mMapSupplyFirstItemPos_BLUE.insert(std::make_pair(iMapID, vVac));
			}
		}		
		///////////////////////m_mMapSupplyFirstItemPos_RED/////////////////////
		for( int k = 0; k < 100; k++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "firstsupply%d_pos%d_red_x", i+1, k+1 );
			stInfo.iX	= rkLoader.LoadInt( szKey, 0 );
			StringCbPrintf( szKey, sizeof( szKey ), "firstsupply%d_pos%d_red_y", i+1, k+1 );
			stInfo.iY	= rkLoader.LoadInt( szKey, 0 );
			StringCbPrintf( szKey, sizeof( szKey ), "firstsupply%d_pos%d_red_z", i+1, k+1 );
			stInfo.iZ	= rkLoader.LoadInt( szKey, 0 );

			if( 0 == stInfo.iX && 0 == stInfo.iY && 0 == stInfo.iZ )
				break;

			MAPSUPPLYITEMPOS::iterator it = m_mMapSupplyFirstItemPos_RED.find(iMapID);
			if( it != m_mMapSupplyFirstItemPos_RED.end() )
				(it->second).push_back(stInfo);
			else
			{
				COORDINATEINFO vVac;
				vVac.push_back(stInfo);
				m_mMapSupplyFirstItemPos_RED.insert(std::make_pair(iMapID, vVac));
			}
		}
		/////////////////////m_mMapSupplyItemFixPos ///////////////////////
		for( int j = 0; j < 100; j++ )
		{
			StringCbPrintf( szKey, sizeof( szKey ), "supply%d_fix_pos%d_x", i+1, j+1 );
			stInfo.iX	= rkLoader.LoadInt( szKey, 0 );
			StringCbPrintf( szKey, sizeof( szKey ), "supply%d_fix_pos%d_y", i+1, j+1 );
			stInfo.iY	= rkLoader.LoadInt( szKey, 0 );
			StringCbPrintf( szKey, sizeof( szKey ), "supply%d_fix_pos%d_z", i+1, j+1 );
			stInfo.iZ	= rkLoader.LoadInt( szKey, 0 );

			if( 0 == stInfo.iX && 0 == stInfo.iY && 0 == stInfo.iZ )
				break;

			MAPSUPPLYITEMPOS::iterator it = m_mMapSupplyItemFixPos.find(iMapID);
			if( it != m_mMapSupplyItemFixPos.end() )
				(it->second).push_back(stInfo);
			else
			{
				COORDINATEINFO vVac;
				vVac.push_back(stInfo);
				m_mMapSupplyItemFixPos.insert(std::make_pair(iMapID, vVac));
			}
		}
	}
}

void FarmingMode::FirstOnCreateFieldItem()			
{
	if( m_ModeState != MS_PLAY )
		return;

	DWORD dwCurTime = TIMEGETTIME();
	if( m_dwRoundTimeSendTime_TEMP + 2000 > dwCurTime )
		return;

	m_dwRoundTimeSendTime_TEMP = TIMEGETTIME();

	FirstOnCreateFieldItem_TEAM(TEAM_BLUE);
	FirstOnCreateFieldItem_TEAM(TEAM_RED);	

	FirstItemDrop = false;
}

void FarmingMode::FirstOnCreateFieldItem_TEAM( TeamType team)			
{
	DWORD dwCurTime = TIMEGETTIME();
	srand( (unsigned int)dwCurTime );	

	for ( int t=0;t<5;t++)
	{
		int iRndItemNum = min( (int)(rand()%iFirst_item_group_count) , 10 );	
		DWORD dwDropEtcItemType = dwFirst_item_group[iRndItemNum];

		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( dwDropEtcItemType );
		if( pEtcItem )
		{
			DWORD dwPackageIndex = 0;
			pEtcItem->FindSuperGashaponPackageRandomPresent(dwDropEtcItemType,dwPackageIndex);
			ioItem *pItem = m_pCreator->CreateItemByCode( dwPackageIndex );

			if( pItem )
			{					
				int MapNum = GetModeMapNum();
				int SubNum = GetModeSubNum();

				Vector3 vPosition;
				GetFirstCoorninateInfo(SubNum, vPosition, team ,t );					//GetCoorninateInfo(iMapID, stInfo);

				if( vPosition.x  == 0.0f && vPosition.y == 0.0f && vPosition.z == 0.0f  )
					return;

				vPosition.x = (float)(m_ioRand.Random( ((int)(vPosition.x - Dispersion_Ratio) + 3000) ,((int)(vPosition.x + Dispersion_Ratio) + 3000))) - 3000.0f;
				//vPosition.y = (float)(m_ioRand.Random( ((int)(vPosition.y - Dispersion_Ratio) + 3000) ,((int)(vPosition.y + Dispersion_Ratio) + 3000))) - 3000.0f;
				vPosition.z = (float)(m_ioRand.Random( ((int)(vPosition.z - Dispersion_Ratio) + 3000) ,((int)(vPosition.z + Dispersion_Ratio) + 3000))) - 3000.0f;

				pItem->SetItemPos( vPosition );
				m_pCreator->AddFieldItem( pItem );

				SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );			
				PACKET_GUARD_VOID( kPacket.Write( 1 ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemCode() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemReinforce() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemMaleCustom() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemFemaleCustom() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetGameIndex() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemPos() ) );
				PACKET_GUARD_VOID( kPacket.Write( "" ) );		// PACKET_GUARD_VOID( kPacket.Write( szOwnerName ) );			
				SendRoomAllUser( kPacket );

#ifdef FARMING_MODE_BY_BCKIM_DEBUG	// 2019-07-03 by bckim, 파밍모드 추가
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[FirstOnCreateFieldItem_TEAM] itemPACK[%d]item[%d]itemname[%s]pos[%4.1f][%4.1f][%4.1f][%d]", dwDropEtcItemType,pItem->GetItemCode(),
					pItem->GetItemName().c_str(),vPosition.x,vPosition.y,vPosition.z, dwCurTime );
#endif // FARMING_MODE_BY_BCKIM_DEBUG				
			}
		}
	}	
}

void FarmingMode::OnCreateFieldItem()	
{
	if( m_ModeState != MS_PLAY )
		return;

	DWORD dwCurTime = TIMEGETTIME();
	DWORD dwDropEtcItemType = -1;

	if( m_dwModeStartTime_TEMP + dwSecond_group_start_time > TIMEGETTIME() )   // 2번째 그룹 시작시간 체크 
		return;

	if( m_dwRoundTimeSendTime_TEMP + dwSecond_group_supply_item_term  > dwCurTime)  // 2번째 그룹 지급 텀 체크 
		return;

	m_dwRoundTimeSendTime_TEMP = dwCurTime;						


	//////////////
	int SubNum = GetModeSubNum();	

	MAPSUPPLYITEMPOS::iterator it	= m_mMapSupplyItemFixPos.find(SubNum);
	if( it == m_mMapSupplyItemFixPos.end() )
		return;

	COORDINATEINFO& vCoordinateVec = it->second;
	int  iCoorSize = (int)vCoordinateVec.size();
	if(iCoorSize == 0 )
		return;

	//////////////////

	srand( (unsigned int)TIMEGETTIME() );
	for(int i=0;i<iCoorSize;i++)
	{
		int iRndItemNum = min( (int)(rand()%iSecond_item_group_count) , 10 );	
		dwDropEtcItemType = dwSecond_item_group[iRndItemNum];

		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( dwDropEtcItemType );
		if( pEtcItem )
		{
			DWORD dwPackageIndex = 0;
			pEtcItem->FindSuperGashaponPackageRandomPresent(dwDropEtcItemType,dwPackageIndex);
			ioItem *pItem = m_pCreator->CreateItemByCode( dwPackageIndex );

			if( pItem )
			{
				Vector3 vPosition;
				//int SubNum = GetModeSubNum();			위에 올라감 
				GetCoorninateInfo(SubNum, vPosition, i);					//GetCoorninateInfo(iMapID, stInfo);

				if( vPosition.x  == 0.0f && vPosition.y == 0.0f && vPosition.z == 0.0f  )
					return;

				vPosition.x = (float)(m_ioRand.Random( ((int)(vPosition.x - Dispersion_Ratio) + 3000) ,((int)(vPosition.x + Dispersion_Ratio) + 3000))) - 3000.0f;
				//vPosition.y = (float)(m_ioRand.Random( ((int)(vPosition.y - Dispersion_Ratio) + 3000) ,((int)(vPosition.y + Dispersion_Ratio) + 3000))) - 3000.0f;
				vPosition.z = (float)(m_ioRand.Random( ((int)(vPosition.z - Dispersion_Ratio) + 3000) ,((int)(vPosition.z + Dispersion_Ratio) + 3000))) - 3000.0f;

				pItem->SetItemPos( vPosition );
				m_pCreator->AddFieldItem( pItem );

				SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );			
				PACKET_GUARD_VOID( kPacket.Write( 1 ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemCode() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemReinforce() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemMaleCustom() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemFemaleCustom() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetGameIndex() ) );
				PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemPos() ) );
				// PACKET_GUARD_VOID( kPacket.Write( szOwnerName ) );			
				PACKET_GUARD_VOID( kPacket.Write( "" ) );	
				SendRoomAllUser( kPacket );

#ifdef FARMING_MODE_BY_BCKIM_DEBUG	// 2019-07-03 by bckim, 파밍모드 추가
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[CREATE_FIELD_ITEM_UNDER] itemPACK[%d]item[%d]itemname[%s]pos[%4.1f][%4.1f][%4.1f]", dwDropEtcItemType,pItem->GetItemCode(),
					pItem->GetItemName().c_str(),vPosition.x,vPosition.y,vPosition.z );			
#endif // FARMING_MODE_BY_BCKIM_DEBUG
			}	
		}
	}
}

void FarmingMode::GetCoorninateInfo(int iMapID, Vector3& vPosition, int iIndex )
{
	CoordinateInfo stInfo;
	MAPSUPPLYITEMPOS::iterator it	= m_mMapSupplyItemFixPos.find(iMapID);
	if( it == m_mMapSupplyItemFixPos.end() )
		return;

	COORDINATEINFO& vCoordinateVec = it->second;
	if( iIndex < 0 || iIndex >= (int)vCoordinateVec.size() )
		return;

	stInfo = vCoordinateVec[iIndex];	
	vPosition.x = stInfo.iX;
	vPosition.y = stInfo.iY;
	vPosition.z = stInfo.iZ;
}

void FarmingMode::GetFirstCoorninateInfo(int iMapID, Vector3& vPosition, TeamType team , int iIndex)
{
	CoordinateInfo stInfo;
	if( team == TEAM_BLUE)
	{
		MAPSUPPLYITEMPOS::iterator it = m_mMapSupplyFirstItemPos_BLUE.find(iMapID);
		if( it == m_mMapSupplyFirstItemPos_BLUE.end() )
			return;

		COORDINATEINFO& vCoordinateVec = it->second;
		if( iIndex < 0 || iIndex >= (int)vCoordinateVec.size() )
			return;

		stInfo = vCoordinateVec[iIndex];	
	}

	if( team == TEAM_RED )
	{
		MAPSUPPLYITEMPOS::iterator it = m_mMapSupplyFirstItemPos_RED.find(iMapID);
		if( it == m_mMapSupplyFirstItemPos_RED.end() )
			return;

		COORDINATEINFO& vCoordinateVec = it->second;		
		if( iIndex < 0 || iIndex >= (int)vCoordinateVec.size() )
			return;

		stInfo = vCoordinateVec[iIndex];	
	}

	vPosition.x = stInfo.iX;
	vPosition.y = stInfo.iY;
	vPosition.z = stInfo.iZ;
}
// End. 2019-07-03 by bckim, 파밍모드 추가
