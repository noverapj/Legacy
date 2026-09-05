#include "stdafx.h"
#include "TeamSurvivalAIMode.h"
#include "../MainProcess.h"
#include "Room.h"
#include "ioEventUserNode.h"
#include "ioEtcItemManager.h"
#include "LadderTeamManager.h"
#include "ioItemInfoManager.h"
#include "TournamentManager.h"
#include "ioExerciseCharIndexManager.h"
#include "MissionManager.h"
#include "../EtcHelpFunc.h"

#include "../DataBase/LogDBClient.h"
#include <strsafe.h>
#include <boost/foreach.hpp>
#include "RoomNodeManager.h"

TeamSurvivalAIMode::TeamSurvivalAIMode( Room *pCreator ) : Mode( pCreator )
{
	m_iRedKillPoint = 0;
	m_iBlueKillPoint = 0;

	m_fRedKillPointRate = 0.0f;
	m_fBlueKillPointRate = 0.0f;

	m_fWinScoreConstant			= 0.0f;
	m_fScoreGapConst			= 0.0f;
	m_fScoreGapRateConst		= 0.0f;
	m_fLadderScoreGapConst		= 0.0f;
	m_fLadderScoreGapRateConst	= 0.0f;

	m_fNormalPesoCorrection		= 0.f;
	m_fNormalExpCorrection		= 0.f;

	m_dwLimitNpcSpawn			= 0;
	m_dwReadyTime				= 0;
	m_dwSpawnTime				= 0;

	m_dwGenCount				= 0;
	m_dwDuplication				= 0;
	m_dwNextNPCCode				= 0;
	m_iLevel = 0;
}

TeamSurvivalAIMode::~TeamSurvivalAIMode()
{
}

void TeamSurvivalAIMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();
}

void TeamSurvivalAIMode::LoadINIValue()
{
	Mode::LoadINIValue();

	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );
	LoadNPCInfo( rkLoader );
	LoadBalancePoint( rkLoader );
	rkLoader.SetTitle( "point" );
	m_fNormalPesoCorrection = rkLoader.LoadFloat( "NormalPesoCorrection", 2.5f );
	m_fNormalExpCorrection = rkLoader.LoadFloat( "NormalExpCorrection", 0.95f );
	ioINILoader kLoader("config/sp2_AI_dropitem.ini");
	LoadNPCEquipInfo( kLoader );
	m_dwCurRoundDuration = m_dwRoundDuration;

	m_vRoundHistory.clear();
}

void TeamSurvivalAIMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	Mode::LoadRoundCtrlValue( rkLoader );

	rkLoader.SetTitle( "round" );
	m_fWinScoreConstant = rkLoader.LoadFloat( "win_score_constant", 1.0f );

	m_fScoreGapConst = rkLoader.LoadFloat( "score_gap_const", 50.0f );
	m_fScoreGapRateConst = rkLoader.LoadFloat( "score_gap_rate_const", 1.5f );
	m_fLadderScoreGapConst = rkLoader.LoadFloat( "ladder_score_gap_const", 50.0f );
	m_fLadderScoreGapRateConst = rkLoader.LoadFloat( "ladder_score_gap_rate_const", 1.5f );
}

void TeamSurvivalAIMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "team_survival%d_object_group%d", iSubNum, iGroupNum );
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

void TeamSurvivalAIMode::AddNewRecord( User *pUser )
{
	if( !pUser )
		return;

	if( FindModeRecord( pUser ) )
		return;

	TeamSurvivalAIRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	UpdateUserRank();
}

void TeamSurvivalAIMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	if(pUser == NULL)
		return;

	int iPreCnt = GetCurTeamUserCnt( pUser->GetTeam() );

	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
		{
			SetModeEndDBLog( &m_vRecordList[i], iCharCnt, LogDBClient::PRT_EXIT_ROOM );
			m_vRecordList.erase( m_vRecordList.begin() + i );
			UpdateCurKillPoint( pUser->GetTeam(), iPreCnt );
			break;
		}
	}

	UpdateUserRank();
	
	m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	RemoveRecordChangeMonsterSync( pUser->GetPublicID() );
}

void TeamSurvivalAIMode::ProcessReady()
{
	DWORD dwCurTime = TIMEGETTIME();
	if( m_dwStateChangeTime + m_dwReadyStateTime >= dwCurTime )
		return;
	// 유저 입장 대기중이면 플레이 상태로 전환하지 않는다.
	if( m_pCreator && m_pCreator->IsRoomEnterUserDelay() )
		return;

	SP2Packet kPacket( STPK_ROUND_START );
	PACKET_GUARD_VOID( kPacket.Write(m_iCurRound) );
	SendRoomPlayUser( kPacket );
	SetModeState( MS_PLAY );

	m_iReadyBlueUserCnt = GetTeamUserCnt( TEAM_BLUE );

	int iRecordCnt = m_vRecordList.size();
	if( iRecordCnt == 0 ) 
		return;

	if( ModeStart() )
	{
		SetModeState( MS_PLAY );
	}

	// 레디 상태에서 이탈한 유저에 대한 체크
	CheckUserLeaveEnd();
}

void TeamSurvivalAIMode::ProcessPlay()
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
	ReviveNPC();
}

void TeamSurvivalAIMode::RestartMode()
{
	Mode::RestartMode();

	// 결과중에 유저들이 전부 나가면 최종 결과 실행.
	if( GetTeamUserCnt( TEAM_BLUE ) == 0 )
	{
		m_bRoundSetEnd = true;
		m_bCheckContribute = false;
		m_bCheckAwardChoose = false;
		SetModeState( MS_RESULT_WAIT );
		WinTeamType eWinTeam = WTT_DRAW;
		eWinTeam = WTT_RED_TEAM;
		m_fBlueKillPointRate = 0.f;
		SendRoundResult( eWinTeam );
		return;
	}

	int iRecordCnt = m_vRecordList.size();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		TeamSurvivalAIRecord &rkRecord = m_vRecordList[i];
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
	
	m_iRedKillPoint = 0;
	m_iBlueKillPoint = 0;

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

int TeamSurvivalAIMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
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
		return USER_KICK_VOTE_PROPOSAL_ERROR_9;

	// 시간 체크
	DWORD dwGapTime = TIMEGETTIME() - m_dwModeStartTime;
	if( dwGapTime > m_KickOutVote.GetKickVoteRoundTime() )
		return USER_KICK_VOTE_PROPOSAL_ERROR_9;
	return 0;
}

ModeRecord* TeamSurvivalAIMode::FindModeRecord( const ioHashString &rkName )
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

ModeRecord* TeamSurvivalAIMode::FindModeRecord( User *pUser )
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

ModeRecord* TeamSurvivalAIMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

TeamSurvivalAIRecord* TeamSurvivalAIMode::FindTeamSurvivalAIRecord( const ioHashString &rkName )
{
	return (TeamSurvivalAIRecord*)FindModeRecord( rkName );
}

TeamSurvivalAIRecord* TeamSurvivalAIMode::FindTeamSurvivalAIRecord( User *pUser )
{
	return (TeamSurvivalAIRecord*)FindModeRecord( pUser );
}

ModeType TeamSurvivalAIMode::GetModeType() const
{
	return MT_TEAM_SURVIVAL_AI;
}

void TeamSurvivalAIMode::GetModeInfo( SP2Packet &rkPacket )
{
	Mode::GetModeInfo( rkPacket );

	rkPacket << m_fWinScoreConstant;

	rkPacket << m_iRedKillPoint;
	rkPacket << m_iBlueKillPoint;

	GetModeHistory( rkPacket );
}

void TeamSurvivalAIMode::GetExtraModeInfo( SP2Packet &rkPacket )
{
	rkPacket << m_fWinScoreConstant;

	rkPacket << m_iRedKillPoint;
	rkPacket << m_iBlueKillPoint;
}

void TeamSurvivalAIMode::GetModeHistory( SP2Packet &rkPacket )
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

void TeamSurvivalAIMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck )
{
	TeamSurvivalAIRecord *pRecord = FindTeamSurvivalAIRecord( rkName );
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

int TeamSurvivalAIMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* TeamSurvivalAIMode::GetModeINIFileName() const
{
	return "config/TeamSurvivalAIMode.ini";
}

TeamType TeamSurvivalAIMode::GetNextTeamType()
{
	int iBlueCount = GetRealUserCnt();
	int iMaxSlotCount = (int)GetMapIndexbySlotCount();
	if( iBlueCount >= iMaxSlotCount )
		return TEAM_NONE;

	return TEAM_BLUE;
}

float TeamSurvivalAIMode::GetResultScoreGapValue( bool bLadderPoint, TeamType eWinTeam )
{
	float fScoreGap = 0.0f;
	if( bLadderPoint )
	{
		if( eWinTeam == TEAM_BLUE )
			fScoreGap = (abs(m_fRedKillPointRate - m_fBlueKillPointRate) + m_fLadderScoreGapConst) / (m_fBlueKillPointRate + m_fLadderScoreGapConst);
		else
			fScoreGap = (abs(m_fRedKillPointRate - m_fBlueKillPointRate) + m_fLadderScoreGapConst) / (m_fRedKillPointRate + m_fLadderScoreGapConst);
		fScoreGap *= m_fLadderScoreGapRateConst;
	}
	else
	{         
		if( eWinTeam == TEAM_BLUE )
			fScoreGap = (abs(m_fRedKillPointRate - m_fBlueKillPointRate) + m_fScoreGapConst) / (m_fBlueKillPointRate + m_fScoreGapConst);
		else
			fScoreGap = (abs(m_fRedKillPointRate - m_fBlueKillPointRate) + m_fScoreGapConst) / (m_fRedKillPointRate + m_fScoreGapConst);
		fScoreGap *= m_fScoreGapRateConst;
	}
	return fScoreGap;
}

float TeamSurvivalAIMode::GetUserCorrection( TeamType eWinTeam, float fRoundPoint, float fScoreGap )
{
	float fUserCorrection = 1.0f;

	int iBlueUserCnt = max( m_iReadyBlueUserCnt, GetTeamUserCnt( TEAM_BLUE ) );

	float fA = fRoundPoint * iBlueUserCnt * 2.f;
	float fB = fRoundPoint + fScoreGap;
	float fC = fRoundPoint - fScoreGap;

	fUserCorrection = fA / max( 1.0f, ( ( fB * iBlueUserCnt ) + ( fC * iBlueUserCnt ) ) );
	if( fUserCorrection > 1.0f )
		fUserCorrection = 1.0f;

	return fUserCorrection;
}

void TeamSurvivalAIMode::CheckRoundEnd( bool bProcessCall )
{
	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;

	WinTeamType eWinTeam = WTT_DRAW;
	TeamType eTeam = CheckKillPoint();
	if( eTeam == TEAM_RED )
		eWinTeam = WTT_RED_TEAM;
	else if( eTeam == TEAM_BLUE )
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
	if( GetState() != MS_PLAY && GetState() != MS_READY)
	{
		eWinTeam = WTT_DRAW;
		return;
	}
	else if( iBlueUser == 0 )
	{
		eWinTeam = WTT_RED_TEAM;
		m_fBlueKillPointRate = 0.f;
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

void TeamSurvivalAIMode::SetRoundEndInfo( WinTeamType eWinTeam )
{
	m_CurRoundWinTeam = eWinTeam;

	if(	GetTeamUserCnt( TEAM_BLUE ) == 0 ||
		GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		m_bRoundSetEnd = true;
	}

	m_bCheckContribute = false;
	m_bCheckAwardChoose = false;
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TeamSurvivalAIMode::SetRoundEndInfo() m_iCurRound != m_vRoundHistory.size()" );
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

void TeamSurvivalAIMode::UpdateRoundRecord()
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

void TeamSurvivalAIMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TeamSurvivalAIMode::OnEventSceneEnd - %s Not Exist Record",
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
		kPacket << m_fWinScoreConstant;
		kPacket << m_iRedKillPoint;
		kPacket << m_iBlueKillPoint;
		SendRoomAllUser( kPacket );
	}
	/*else if( m_bUseViewMode && m_ModeState == MS_PLAY && dwPastTime > m_dwViewCheckTime )
	{
		pRecord->eState = RS_VIEW;
		pRecord->pUser->SetStartTimeLog( TIMEGETTIME() );

		SP2Packet kPacket( STPK_ROUND_JOIN_VIEW );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << m_dwCurRoundDuration;
		kPacket << m_fWinScoreConstant;
		kPacket << m_iRedKillPoint;
		kPacket << m_iBlueKillPoint;
		SendRoomAllUser( kPacket );
	}*/
	else
	{
		int iPreCnt = GetCurTeamUserCnt( pRecord->pUser->GetTeam() );

		pRecord->eState = RS_PLAY;
		pRecord->StartPlaying();        //( 관전X, 데스타임X )
		pRecord->pUser->StartCharLimitDate( Mode::GetCharLimitCheckTime(), __FILE__, __LINE__ );
		pRecord->pUser->StartEtcItemTime( __FUNCTION__ );

		UpdateCurKillPoint( pRecord->pUser->GetTeam(), iPreCnt );

		SP2Packet kPacket( STPK_ROUND_JOIN );
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << GetSelectCharTime();
		kPacket << m_dwCurRoundDuration;
		kPacket << m_fWinScoreConstant;
		kPacket << m_iRedKillPoint;
		kPacket << m_iBlueKillPoint;
		SendRoomAllUser( kPacket );
	}
}

int TeamSurvivalAIMode::GetCurTeamUserCnt( TeamType eTeam )
{
	int iNPCCnt = 0;
	BOOST_FOREACH( NPCRecord& rMonster, m_vNPCList )
	{
		if( rMonster.eTeam == eTeam )
			iNPCCnt++;
	}

	if( eTeam == TEAM_BLUE )
	{
		return GetRecordCnt() + iNPCCnt;
	}
	else if( eTeam == TEAM_RED )
	{
		return iNPCCnt;
	}

	return 0;
}

bool TeamSurvivalAIMode::CheckRoundJoin( User *pSend )
{
	if( !pSend )
		return false;

	if( m_ModeState != MS_RESULT && m_ModeState != MS_RESULT_WAIT )
		return false;

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "TeamSurvivalAIMode::CheckRoundJoin - %s Not Exist Record",
								 pSend->GetPublicID().c_str() );
		return false;
	}

	pRecord->pUser->EquipDBItemToAllChar();
	SetFirstRevivalTime( pRecord );

	int iModeState = MS_RESULT_WAIT;

	DWORD dwPastTime = TIMEGETTIME() - m_dwStateChangeTime;

	int iPreCnt = GetCurTeamUserCnt( pRecord->pUser->GetTeam() );

	pRecord->eState = RS_PLAY;
	pRecord->StartPlaying();        //( 관전X, 데스타임X )	
	pRecord->pUser->StartCharLimitDate( Mode::GetCharLimitCheckTime(), __FILE__, __LINE__ );
	pRecord->pUser->StartEtcItemTime( __FUNCTION__ );

	UpdateCurKillPoint( pRecord->pUser->GetTeam(), iPreCnt );

	SP2Packet kPacket( STPK_ROUND_JOIN );
	kPacket << pRecord->pUser->GetPublicID();
	kPacket << iModeState;
	kPacket << dwPastTime;
	kPacket << GetSelectCharTime();
	kPacket << m_dwCurRoundDuration;
	SendRoomAllUser( kPacket );

	SP2Packet kModeInfoPk( STPK_MODE_INFO );
	GetModeInfo( kModeInfoPk );
	SendRoomPlayUser( kModeInfoPk );
	
	return true;
}

void TeamSurvivalAIMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd ) return;

	if( GetTeamUserCnt( TEAM_BLUE ) == 0 )
	{
		CheckRoundEnd( false );
	}
}

void TeamSurvivalAIMode::UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	if( !pDier )
		return;

	if( !m_pCreator )
		return;

	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( !pDieRecord )
		return;
	if( !pDieRecord->pUser )
		return;

	// 마지막으로 타격한 유저와 죽은 유저
	TeamType eDieTeam = pDieRecord->pUser->GetTeam();
	if( !szAttacker.IsEmpty() && pDier->GetPublicID() != szAttacker )
	{
		NPCRecord* pNPCRecord = FindMonsterInfo( szAttacker );
		if( pNPCRecord )
		{
			pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
		}
		else
		{
			ModeRecord *pAttRecord = FindModeRecord( szAttacker );
			if( !pAttRecord )	return;

			float fKillPoint = 0.5f + ( (float)GetKillPoint( pAttRecord->pUser->GetTeam() ) * 0.5f );
			if( pDieRecord->pUser->GetTeam() != pAttRecord->pUser->GetTeam() )
			{
				pAttRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
				pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
			}
			else
			{
				if( pAttRecord->pUser != pDieRecord->pUser )	// team kill
				{
					pAttRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), -fKillPoint );
					pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
				}
				else
				{
					pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
				}
			}
		}
	}
	else
	{
		pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
	}


	if( eDieTeam == TEAM_RED )
		SetScore( TEAM_BLUE );
	else if( eDieTeam == TEAM_BLUE )
		SetScore( TEAM_RED );

	// 가장 많은 데미지를 입힌 유저
	if( !szBestAttacker.IsEmpty() && pDier->GetPublicID() != szBestAttacker )
	{
		ModeRecord *pAttRecord = FindModeRecord( szBestAttacker );
		if( !pAttRecord )	return;

		float fKillPoint = 0.5f + ( (float)GetKillPoint( pAttRecord->pUser->GetTeam() ) * 0.5f );
		if( pDieRecord->pUser->GetTeam() != pAttRecord->pUser->GetTeam() )
			pAttRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
	}
}

void TeamSurvivalAIMode::UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( !pDieRecord || !pDieRecord->pUser )	return;
	if( !m_pCreator ) return;

	// 마지막으로 타격한 유저와 죽은 유저
	TeamType eDieTeam = pDieRecord->pUser->GetTeam();
	ModeRecord *pKillRecord = FindModeRecord( szAttacker );
	if( pKillRecord && pKillRecord->pUser )
	{
		float fKillPoint = 0.5f + ( (float)GetKillPoint( pKillRecord->pUser->GetTeam() ) * 0.5f );
		if( pKillRecord->pUser->GetTeam() != pDier->GetTeam() )
		{
			pKillRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
			pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
		}
		else
		{
			if( pKillRecord->pUser != pDier )	// team kill
			{
				pKillRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), -fKillPoint );
				pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
			}
			else
			{
				pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
			}
		}
	}
	else
	{
		pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );
	}

	if( eDieTeam == TEAM_RED )
		SetScore( TEAM_BLUE );
	else if( eDieTeam == TEAM_BLUE )
		SetScore( TEAM_RED );

	// 가장 많은 데미지를 입힌 유저
	ModeRecord *pBestAttackerRecord = FindModeRecord( szBestAttacker );
	if( pBestAttackerRecord && pBestAttackerRecord->pUser )
	{
		float fKillPoint = 0.5f + ( (float)GetKillPoint( pBestAttackerRecord->pUser->GetTeam() ) * 0.5f );
		if( pBestAttackerRecord->pUser->GetTeam() != pDier->GetTeam() )
			pBestAttackerRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
	}
}

void TeamSurvivalAIMode::UpdateCurKillPoint( TeamType eTeam, int iPreCnt )
{
	int iCurCnt = GetCurTeamUserCnt( eTeam );
	if( iCurCnt <= 0 || iPreCnt <= 0 )
		return;

	float fChangeRate = (float)iCurCnt / iPreCnt;

	if( eTeam == TEAM_RED && m_iBlueKillPoint > 0 )
		m_iBlueKillPoint = m_iBlueKillPoint * fChangeRate;
	else if( eTeam == TEAM_BLUE && m_iRedKillPoint > 0 )
		m_iRedKillPoint = m_iRedKillPoint * fChangeRate;

	UpdateCurKillPointRate();
}

void TeamSurvivalAIMode::UpdateCurKillPointRate()
{
	float fRedCnt = GetCurTeamUserCnt( TEAM_BLUE ) * m_fWinScoreConstant * 100;
	float fBlueCnt = GetCurTeamUserCnt( TEAM_RED ) * m_fWinScoreConstant * 100;

	if( fRedCnt <= 0 || fBlueCnt <= 0 )
		return;

	if( m_iBlueKillPoint > 0 )
	{
		m_fBlueKillPointRate = m_iBlueKillPoint / fBlueCnt;
		m_fBlueKillPointRate = min( 1.0f, m_fBlueKillPointRate );
	}

	if( m_iRedKillPoint > 0 )
	{
		m_fRedKillPointRate = m_iRedKillPoint / fRedCnt;
		m_fRedKillPointRate = min( 1.0f, m_fRedKillPointRate );
	}
}

TeamType TeamSurvivalAIMode::CheckKillPoint()
{
	// 레드팀 목표값은 블루팀 인원 * 상수
	float fRedCnt = GetCurTeamUserCnt( TEAM_BLUE ) * m_fWinScoreConstant * 100;
	float fBlueCnt = GetCurTeamUserCnt( TEAM_RED ) * m_fWinScoreConstant * 100;

	if( fRedCnt <= 0 || fBlueCnt <= 0 )
		return TEAM_NONE;

	TeamType ePointTeam = TEAM_NONE;
	if( m_iRedKillPoint >= fRedCnt )
		ePointTeam = TEAM_RED;
	else if( m_iBlueKillPoint >= fBlueCnt )
		ePointTeam = TEAM_BLUE;

	return ePointTeam;
}

void TeamSurvivalAIMode::SetScore( TeamType eTeam )
{
	if(eTeam == TEAM_BLUE)
		m_iBlueKillPoint += 100;
	else if(eTeam == TEAM_RED)
		m_iRedKillPoint += 100;

	UpdateCurKillPointRate();
}

void TeamSurvivalAIMode::FinalRoundPoint( ModeRecord *pRecord, bool bAbuseUser, float fTotalVictoriesRate, float fTotalConsecutivelyRate )
{
	if( !pRecord )	return;

	User* pUser = pRecord->pUser;
	if( !pUser ) return;

	if( !m_pCreator )
		return;

	fTotalVictoriesRate = 1.0f;
	fTotalConsecutivelyRate = 1.0f;

	// 옵저버는 이 함수를 실행할 필요가 없다.
	if( pUser->IsObserver() || pUser->IsStealth() )
	{
		pUser->SetModeConsecutively( MT_NONE );       // 옵저버는 연속 게임 초기화
		return;
	}

	int iCurMaxSlot = pUser->GetCurMaxCharSlot();

	if( m_dwModePointTime == 0 || m_dwModeRecordPointTime == 0 )
		return;

	TeamType eWinTeam  = TEAM_NONE;
	if( m_iBlueTeamWinCnt > m_iRedTeamWinCnt )
		eWinTeam = TEAM_BLUE;
	else if( m_iRedTeamWinCnt > m_iBlueTeamWinCnt )
		eWinTeam = TEAM_RED;


	//승패 기록
	int iWinLoseTiePoint = 0;
	if( !bAbuseUser )
	{
		iWinLoseTiePoint = GetWinLoseTiePoint( pUser->GetTeam(), eWinTeam, (float)GetRecordPlayTime( pRecord ) / m_dwModeRecordPointTime );
		if( eWinTeam == pUser->GetTeam() )
			pUser->AddWinCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );				
		else if( eWinTeam != TEAM_NONE )
			pUser->AddLoseCount(  m_pCreator->GetRoomStyle(), GetModeType(), iWinLoseTiePoint );
	}							

	//플레이 중인 모드
	ModeCategory ePlayMode = GetPlayModeCategory();

	//총 라운드 수 A
	float fRoundPoint = m_ModePointRound;			
	//스코어 차이  B
	float fScoreGap = GetResultScoreGapValue( false, eWinTeam );
	if( eWinTeam == TEAM_NONE )
		fScoreGap = 0.0f;
	//인원 보정 C
	float fUserCorrection = GetUserCorrection( eWinTeam, fRoundPoint, fScoreGap );
	//플레이 시간 보정값 D
	float fPlayTimeCorrection = (float)GetRecordPlayTime( pRecord ) / m_dwModePointTime;
	//페소보정값 E
	float fPesoCorrection = m_fPesoCorrection;
	if( m_iLevel == 1 )
		fPesoCorrection = m_fNormalPesoCorrection;	
	//경험치 보정값 F
	float fExpCorrection = m_fExpCorrection;
	if( m_iLevel == 1 )
		fExpCorrection  = m_fNormalExpCorrection;
	//차단 G
	float fBlockPoint = pUser->GetBlockPointPer();
	//기여도 H
	float fContributePer = pRecord->fContributePer;
	//길드보너스 I
	pRecord->fBonusArray[BA_GUILD] = m_pCreator->GetGuildBonus( pRecord->pUser->GetTeam() );
	float fGuildBonus = pRecord->fBonusArray[BA_GUILD];
	//용병 보너스 J
	pRecord->fBonusArray[BA_SOLDIER_CNT] = Help::GetSoldierPossessionBonus( pUser->GetActiveCharCount() );
	float fSoldierCntBonus = pRecord->fBonusArray[BA_SOLDIER_CNT];
	//PC방 보너스 K
	if( pUser->IsPCRoomAuthority() )
	{
		pRecord->fBonusArray[BA_PCROOM_EXP] = Help::GetPCRoomBonusExp();
		pRecord->fBonusArray[BA_PCROOM_PESO]= Help::GetPCRoomBonusPeso();

		if( g_EventMgr.IsAlive( EVT_PCROOM_BONUS, pUser->GetChannelingType(), ePlayMode ) )
		{
			EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
			PCRoomEventUserNode* pPcroomEventNode = static_cast<PCRoomEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_PCROOM_BONUS, ePlayMode ) );
			
			if( pPcroomEventNode )
				pPcroomEventNode->SetPesoAndExpBonus( pUser ,pRecord->fBonusArray[BA_PCROOM_PESO], pRecord->fBonusArray[BA_PCROOM_EXP], ePlayMode ); 
		}
	}
	float fPCRoomBonusExp  = pRecord->fBonusArray[BA_PCROOM_EXP];
	float fPCRoomBonusPeso = pRecord->fBonusArray[BA_PCROOM_PESO];
	//모드 보너스 L
	if( pUser )
	{
		// EVT_MODE_BONUS (1)
		//셔플 모드 일경우 모드 체크 건너뛰기 위해. 셔플 모드가 아닐경우 모드 체크 
		ModeCategory eModeBonus = ePlayMode;

		if( eModeBonus != MC_SHUFFLE )
			eModeBonus = MC_DEFAULT;

		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		ModeBonusEventUserNode* pEvent1 = static_cast< ModeBonusEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_MODE_BONUS, eModeBonus ) );
		if( pEvent1 )
		{
			if( pEvent1->IsEventMode( GetModeType(), eModeBonus ) )
				pRecord->fBonusArray[ BA_PLAYMODE ] = pEvent1->GetEventPer( fPCRoomBonusExp, pUser, eModeBonus );
		}

		// EVT_MODE_BONUS2 (2)
		ModeBonusEventUserNode* pEvent2 = static_cast< ModeBonusEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_MODE_BONUS2, eModeBonus ) );
		if( pEvent2 )
		{
			if( pEvent2->IsEventMode( GetModeType(), eModeBonus ) )
				pRecord->fBonusArray[ BA_PLAYMODE ] += pEvent2->GetEventPer( fPCRoomBonusExp, pUser, eModeBonus );
		}
		
		if( pRecord->fBonusArray[BA_PLAYMODE] == 0.0f )
		{
			pRecord->fBonusArray[BA_PLAYMODE] = m_fPlayModeBonus;
		}
	}
	float fModeBonus = pRecord->fBonusArray[BA_PLAYMODE];

	//친구 보너스 M	
	if( pUser->IsPCRoomAuthority() )
	{		
		pRecord->fBonusArray[BA_FRIEND] = min( GetPcRoomMaxFriendBonus(), GetPcRoomFriendBonus() * (float)GetSameFriendUserCnt( pUser ) );
	}
	else
	{
		pRecord->fBonusArray[BA_FRIEND] = min( GetMaxFriendBonus(), GetFriendBonus() * (float)GetSameFriendUserCnt( pUser ) );
	}

	float fFriendBonusPer = pRecord->fBonusArray[BA_FRIEND];
	// 이벤트 경험치 보너스 N
	float fEventBonus = 0.0f;
	if( pUser )
	{
		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		ExpEventUserNode *pEventNode = static_cast<ExpEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_EXP, ePlayMode ) );
		if( pEventNode )
		{
			pRecord->fBonusArray[BA_EVENT] = pEventNode->GetEventPer( fPCRoomBonusExp, pUser, ePlayMode );
			fEventBonus = pRecord->fBonusArray[BA_EVENT];
		}

		// second event : evt_exp2
		ExpEventUserNode* pExp2 = static_cast< ExpEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_EXP2, ePlayMode ) );
		if( pExp2 )
		{
			pRecord->fBonusArray[ BA_EVENT ] += pExp2->GetEventPer( fPCRoomBonusExp, pUser, ePlayMode );
			fEventBonus = pRecord->fBonusArray[ BA_EVENT ];
		}
	}
	// 이벤트 페소 보너스 O
	float fPesoEventBonus = 0.0f;
	if( pUser )
	{
		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		PesoEventUserNode *pEventNode = static_cast<PesoEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_PESO, ePlayMode ) );
		if( pEventNode )
		{
			pRecord->fBonusArray[BA_EVENT_PESO] = pEventNode->GetPesoPer( fPCRoomBonusPeso, pUser, ePlayMode );
			fPesoEventBonus = pRecord->fBonusArray[BA_EVENT_PESO];
		}

		// second event : evt_peso2
		PesoEventUserNode* pPeso2 = static_cast< PesoEventUserNode* >( rEventUserManager.GetEventUserNode( EVT_PES02, ePlayMode ) );
		if( pPeso2 )
		{
			pRecord->fBonusArray[ BA_EVENT_PESO ] += pPeso2->GetPesoPer( fPCRoomBonusPeso, pUser, ePlayMode );
			fPesoEventBonus = pRecord->fBonusArray[ BA_EVENT_PESO ];
		}
	}
	// 권한 아이템 보너스 P
	float fEtcItemBonus = 0.0f;
	float fEtcItemPesoBonus = 0.0f;
	float fEtcItemExpBonus  = 0.0f;
	if( pUser )
	{
		ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
		if( pUserEtcItem )
		{
			ioEtcItem *pItemItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_PESO_EXP_BONUS );
			ioUserEtcItem::ETCITEMSLOT kSlot;
			if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_PESO_EXP_BONUS, kSlot) && pItemItem )
			{
				pRecord->fBonusArray[BA_ETC_ITEM] = ( (float) pItemItem->GetUseValue() / 100.0f ); // 20 -> 0.20
				fEtcItemBonus = pRecord->fBonusArray[BA_ETC_ITEM];
			}

			pItemItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_PESO_BONUS );
			if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_PESO_BONUS, kSlot ) && pItemItem )
			{
				pRecord->fBonusArray[BA_ETC_ITEM_PESO] = ( (float) pItemItem->GetUseValue() / 100.0f ); // 20 -> 0.20
				fEtcItemPesoBonus = pRecord->fBonusArray[BA_ETC_ITEM_PESO];
			}

			pItemItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_EXP_BONUS );
			if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_EXP_BONUS, kSlot ) && pItemItem )
			{
				pRecord->fBonusArray[BA_ETC_ITEM_EXP] = ( (float) pItemItem->GetUseValue() / 100.0f ); // 20 -> 0.20
				fEtcItemExpBonus = pRecord->fBonusArray[BA_ETC_ITEM_EXP];
			}
		}
	}
	// 진영전 보너스 Q
	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
		pRecord->fBonusArray[BA_CAMP_BONUS] = Help::GetLadderBonus();
	float fCampBattleBonus = pRecord->fBonusArray[BA_CAMP_BONUS];

	// 시상식 보너스 R
	float fAwardBonus = pRecord->fBonusArray[BA_AWARD_BONUS];

	// 영웅전 타이틀 보너스 S
	pRecord->fBonusArray[BA_HERO_TITLE_PESO] = GetHeroTitleBonus( pUser );
	float fHeroTitlePesoBonus = pRecord->fBonusArray[BA_HERO_TITLE_PESO];

	// 연속 모드 보너스 T 14.04.09부로 사용하지 않음
	//pRecord->fBonusArray[BA_MODE_CONSECUTIVELY] = pUser->GetModeConsecutivelyBonus();
	//float fModeConsecutivelyBonus = (1.0f + pRecord->fBonusArray[BA_MODE_CONSECUTIVELY]) * fTotalConsecutivelyRate;
	float fModeConsecutivelyBonus = 1.0f;

	//획득 경험치
	float fAcquireExp       = 0.0f;
	float fExpPlusValue     = max( 0.0f, fContributePer + fGuildBonus + fSoldierCntBonus + fPCRoomBonusExp + fModeBonus + fFriendBonusPer + fEventBonus + fEtcItemBonus + fCampBattleBonus + fEtcItemExpBonus );
	float fExpTotalMultiply = fUserCorrection * fPlayTimeCorrection * fExpCorrection * fBlockPoint * fExpPlusValue;
	char szLogArguWinLose[MAX_PATH]="";
	char szLogArguPlusMinus[MAX_PATH]="";
	if( pUser->GetTeam() == eWinTeam )
	{
		fAcquireExp = ( fRoundPoint + fScoreGap ) * fExpTotalMultiply;
		StringCbCopy( szLogArguWinLose, sizeof(szLogArguWinLose), "WIN" );
		StringCbCopy( szLogArguPlusMinus, sizeof(szLogArguPlusMinus), "+" );
	}
	else
	{
		fAcquireExp = ( fRoundPoint - fScoreGap ) * fExpTotalMultiply;
		StringCbCopy( szLogArguWinLose, sizeof(szLogArguWinLose), "LOSE" );
		StringCbCopy( szLogArguPlusMinus, sizeof(szLogArguPlusMinus), "-" );
	}
	fAcquireExp = fAcquireExp * fModeConsecutivelyBonus;
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[%d] %s [%d]: %s EXP : ( %.2f %s %.2f ) x %.2f x %.4f x %.2f x %.2f x ( %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f ) x %.2f = %.2f",
		                    m_pCreator->GetRoomIndex(), pUser->GetPublicID().c_str(), (int)bAbuseUser, szLogArguWinLose, 
		                    fRoundPoint, szLogArguPlusMinus, fScoreGap, fUserCorrection, fPlayTimeCorrection, fExpCorrection, fBlockPoint, fContributePer, fGuildBonus, 
							fSoldierCntBonus, fPCRoomBonusExp, fModeBonus, fFriendBonusPer, fEventBonus, fEtcItemBonus, fCampBattleBonus, fEtcItemExpBonus, fModeConsecutivelyBonus, fAcquireExp );

	if( m_bTournamentRoom )
	{
		// 대회 경기방은 경험치 없음. 
		fAcquireExp = 0.0f;           
	}

	//획득 페소
	float fAcquirePeso       = 0.0f;
	float fPesoPlusValue     = max( 0.0f, fContributePer + fGuildBonus + fPCRoomBonusPeso + fModeBonus + fFriendBonusPer + fPesoEventBonus + fEtcItemBonus + fCampBattleBonus + fAwardBonus + fEtcItemPesoBonus + fHeroTitlePesoBonus );
	float fPesoTotalMultiply = fUserCorrection * fPlayTimeCorrection * fPesoCorrection * fBlockPoint * fPesoPlusValue;
	if( pUser->GetTeam() == eWinTeam )
	{
		fAcquirePeso = ( fRoundPoint + fScoreGap ) * fPesoTotalMultiply;
	}
	else
	{
		fAcquirePeso = ( fRoundPoint - fScoreGap ) * fPesoTotalMultiply;
	}

	float fCurVictories = 0.0f;
	/*float fCurVictories = 0.0f;
	int iCurVictoriesCnt = pRecord->iVictories;
	if( pUser->IsLadderTeam() )
		iCurVictoriesCnt = pUser->GetMyVictories();

	int iGapVictories = iCurVictoriesCnt - Help::GetFirstVictories();
	if( iGapVictories >= 0 )
	{
		fCurVictories = Help::GetFirstVictoriesRate() + Help::GetVictoriesRate() * iGapVictories;
		fCurVictories = min( Help::GetMaxVictoriesRate(), fCurVictories );
	}

	pRecord->fBonusArray[BA_VICTORIES_PESO] = fCurVictories;*/

	float fVictoriesBonus = (1.0f + fCurVictories) * fTotalVictoriesRate;
	fAcquirePeso = fAcquirePeso * fVictoriesBonus;
	fAcquirePeso = fAcquirePeso * fModeConsecutivelyBonus;

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[%d] %s [%d]: %s PESO : (( %.2f %s %.2f ) x %.2f x %.4f x %.2f x %.2f x ( %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f + %.2f ) x %.2f x %.2f ) = %.2f",
		                    m_pCreator->GetRoomIndex(), pUser->GetPublicID().c_str(), (int)bAbuseUser, szLogArguWinLose, 
		                    fRoundPoint, szLogArguPlusMinus, fScoreGap, fUserCorrection, fPlayTimeCorrection, fPesoCorrection, fBlockPoint, fContributePer, fGuildBonus, fPCRoomBonusPeso, fModeBonus, fFriendBonusPer, fPesoEventBonus, fEtcItemBonus, fCampBattleBonus, fAwardBonus, fEtcItemPesoBonus, fHeroTitlePesoBonus, fModeConsecutivelyBonus, fVictoriesBonus, fAcquirePeso );

	// 레벨별 경험치 / 페소 조정.
	int iBalanceCnt = m_vBalancePoint.size();
	for( int i=0; i<iBalanceCnt; ++i )
	{
		if( m_vBalancePoint[i].IsInSection( pUser->GetGradeLevel() ) )
		{
			fAcquireExp  *= m_vBalancePoint[i].exp_rate;
			fAcquirePeso *= m_vBalancePoint[i].gold_rate;
			break;
		}
	}

	fAcquirePeso += 0.5f;     //반올림

	if( m_bTournamentRoom )
	{
		// 대회 경기방은 페소 고정. 
		fAcquirePeso = g_TournamentManager.GetRegularTournamentBattleRewardPeso( m_dwTournamentIndex );             
	}

	//어뷰즈 판정
	if( bAbuseUser )
	{
		fAcquireExp = 0.0f;
		fAcquirePeso= 0.0f;
	}
	else
	{
		// 플레이 시간 이벤트
		EventUserManager &rEventUserManager = pUser->GetEventUserMgr();
		ChanceMortmainCharEventUserNode *pEventNode = static_cast<ChanceMortmainCharEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_CHANCE_MORTMAIN_CHAR ) );
		if( pEventNode )
		{
			pEventNode->UpdatePlayTime( pUser, GetRecordPlayTime( pRecord ) );
		}
		PlayTimePresentEventUserNode *pPlayTimePresentEventNode = static_cast<PlayTimePresentEventUserNode*> ( rEventUserManager.GetEventUserNode( EVT_PLAYTIME_PRESENT ) );
		if( pPlayTimePresentEventNode )
			pPlayTimePresentEventNode->UpdatePlayTime( pUser, GetRecordPlayTime( pRecord ) );
	}

	//진영전이 종료되면 포인트와 페소를 지급하지 않는다.
	if( m_pCreator->GetRoomStyle() == RSTYLE_LADDERBATTLE )
	{
		if( !g_LadderTeamManager.IsCampBattlePlay() )
		{
			fAcquireExp = 0.0f;
			fAcquirePeso= 0.0f;
		}
	}
	pRecord->iTotalExp  = 0;
	pRecord->iTotalPeso = 0;
	pRecord->iTotalAddPeso = 0;

	// 페소 지급.
	pUser->AddMoney( (int)fAcquirePeso );
	g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pUser, 0, 0, LogDBClient::LET_MODE, LogDBClient::PT_BATTLE, GetModeType(), 0, (int)fAcquirePeso, NULL);

	pRecord->iTotalPeso = (int)fAcquirePeso;
	pRecord->iTotalAddPeso = (int)fAcquirePeso;

	int i = 0;
	enum { MAX_GET_POINT_CHAR = 5, };

	DWORDVec dwPlayTimeList;
	dwPlayTimeList.clear();

	pRecord->iResultClassTypeList.clear();
	pRecord->iResultClassPointList.clear();
	pRecord->bResultLevelUP = false;		

	// 연속 모드 
	if( !bAbuseUser )
	{
		pUser->SetModeConsecutively( GetModeType() );
	}

	if( !bAbuseUser )
	{
		// 다음주 용병 가격을 위해 플레이 시간을 따로 저장한다.
		g_ItemPriceMgr.SetGradePlayTimeCollected( pRecord->pUser->GetGradeLevel(), GetRecordPlayTime( pRecord ) / 1000 );
	}

	// 출전 시간에 따라 상위 클래스를 구한다.
	DWORD dwTotalTime = pRecord->GetHighPlayingTime( MAX_GET_POINT_CHAR, pRecord->iResultClassTypeList, dwPlayTimeList );
	if( dwTotalTime == 0 ) 
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Return Playing Time Zeor!!!" );
		return;
	}

	int iListSize = pRecord->iResultClassTypeList.size();
	for(i = 0; i < MAX_GET_POINT_CHAR; i++)
	{
		if( i >= iCurMaxSlot ) break;
		if( !COMPARE( i, 0, iListSize ) ) break;

		if( pRecord->iResultClassTypeList[i] == 0 ) continue;

		float fSoldierPer = (float)dwPlayTimeList[i] / dwTotalTime;
		int iCurPoint = ( fAcquireExp * fSoldierPer ) + 0.5f;     //반올림
		pRecord->iResultClassPointList.push_back( iCurPoint );
		pRecord->iTotalExp += pRecord->iResultClassPointList[i];

		// 용병 경험치 특별 아이템 - 보너스
		float fClassPoint = (float)pRecord->iResultClassPointList[i];
		float fClassBonus = pUser->GetSoldierExpBonus( pRecord->iResultClassTypeList[i] );
		float fAddEventExp = pUser->GetExpBonusEvent();
		float fSoldierExpBonus = ( fClassPoint * ( fClassBonus + fAddEventExp ) );
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ClassType[%d] PlayTime[%d - %d - %d] - WinPoint[%d] - Bonus[%.2f]", pRecord->iResultClassTypeList[i],
																					   dwPlayTimeList[i],
																					   dwTotalTime,
																					   GetRecordPlayTime( pRecord ),
																					   pRecord->iResultClassPointList[i], fSoldierExpBonus );

		// 경험치 지급 및 레벨업 확인
		if( pUser->IsClassTypeExerciseStyle( pRecord->iResultClassTypeList[i], EXERCISE_RENTAL ) == false )
			pUser->AddClassExp( pRecord->iResultClassTypeList[i], pRecord->iResultClassPointList[i] + fSoldierExpBonus );
		if( pUser->AddGradeExp( pRecord->iResultClassPointList[i] ) )
			pRecord->bResultLevelUP = true;

		// 
		pRecord->iResultClassPointList[i] += fSoldierExpBonus;

		if( pUser->IsClassTypeExerciseStyle( pRecord->iResultClassTypeList[i], EXERCISE_RENTAL ) )
		{
			// 계급 경험치만 획득하고 용병 경험치는 획득 안됨
			pRecord->iResultClassPointList[i] = 0;
		}
	}
	// 용병단과 용병들의 레벨업 보상을 지급한다.
	pRecord->iTotalPeso += pUser->GradeNClassUPBonus();	

	//미션
	static DWORDVec vValues;
	vValues.clear();
	if( pUser )
	{
		if( pRecord )
		{
			vValues.push_back(pRecord->GetAllPlayingTime());
			vValues.push_back(m_pCreator->GetRoomStyle());
			vValues.push_back(GetModeType());
			
			g_MissionMgr.DoTrigger(MISSION_CLASS_MODEPLAY, pUser, vValues);
		}
	}
}

void TeamSurvivalAIMode::SendRoundResult( WinTeamType eWinTeam )
{
	SP2Packet kPacket( STPK_ROUND_END );
	kPacket << eWinTeam;
	kPacket << m_iRedTeamWinCnt;
	kPacket << m_iBlueTeamWinCnt;

	kPacket << m_fRedKillPointRate;
	kPacket << m_fBlueKillPointRate;
	
	kPacket << GetPlayingUserCnt();

	int iRecordCnt = GetRecordCnt();
	for( int i=0 ; i<iRecordCnt ; i++ )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord || pRecord->eState == RS_LOADING )
			continue;

		kPacket << pRecord->pUser->GetPublicID();

		//
		int iMyVictories = 0;
		if( pRecord->pUser )
		{
			if( m_bRoundSetEnd && eWinTeam != WTT_DRAW && eWinTeam != WTT_NONE )
				pRecord->pUser->IncreaseMyVictories( IsWinTeam(eWinTeam, pRecord->pUser->GetTeam()) );

			iMyVictories = pRecord->pUser->GetMyVictories();
		}

		kPacket << iMyVictories;
		//

		//
		int iKillSize = pRecord->iKillInfoMap.size();
		kPacket << iKillSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_k = pRecord->iKillInfoMap.begin();
		while( iter_k != pRecord->iKillInfoMap.end() )
		{
			kPacket << iter_k->first;
			kPacket << iter_k->second;

			++iter_k;
		}
		LOOP_GUARD_CLEAR();

		int iDeathSize = pRecord->iDeathInfoMap.size();
		kPacket << iDeathSize;

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_d = pRecord->iDeathInfoMap.begin();
		while( iter_d != pRecord->iDeathInfoMap.end() )
		{
			kPacket << iter_d->first;
			kPacket << iter_d->second;

			++iter_d;
		}
		LOOP_GUARD_CLEAR();
		//

		kPacket << pRecord->iCurRank;
		kPacket << pRecord->iPreRank;
	}

	kPacket << m_bRoundSetEnd;

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

void TeamSurvivalAIMode::UpdateNPCDieRecord( const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	// 마지막으로 타격한 유저와 죽은 유저
	// 공격자 USER
	ModeRecord* pAttUserRecord = FindModeRecord( szAttacker );
	if( pAttUserRecord && pAttUserRecord->pUser && m_pCreator )
	{
		float fKillPoint = 0.5f + ( (float)GetKillPoint( pAttUserRecord->pUser->GetTeam() ) * 0.5f );
		pAttUserRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );

		TeamType eAttackerTeam  = pAttUserRecord->pUser->GetTeam();

		// 가장 많은 데미지를 입힌 유저
		ModeRecord* pBestAttRecord = FindModeRecord( szBestAttacker );
		if( pBestAttRecord )
		{
			float fKillPoint = 0.5f + ( (float)GetKillPoint( pAttUserRecord->pUser->GetTeam() ) * 0.5f );
			pBestAttRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
		}

		return;
	}
}

void TeamSurvivalAIMode::OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
	NPCRecord* pDieMonster =  (NPCRecord*)FindMonsterInfo( rkDieName );

	if( !pDieMonster )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][teamsurvival]OnDropDieNpc Die Monster is null : [%lu] [%s]", m_pCreator->GetRoomIndex(), rkDieName.c_str() );
		return;
	}

	if( pDieMonster->eState != RS_PLAY )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][teamsurvival]OnDropDieNpc None live NPC : [%lu] [%s]", m_pCreator->GetRoomIndex(), rkDieName.c_str() );
		return;
	}

	// NPC가 죽은 위치.
	float fDiePosX = 0, fDiePosY = 0, fDiePosZ = 0;
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosX) );
	//PACKET_GUARD_VOID( rkPacket.Read(fDiePosY) );
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

	// 데미지 리스트 처리
	int iDamageCnt = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iDamageCnt) );

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;
	ioHashString szBestAttackerName;

	static DamageTableList vDamageList;
	vDamageList.clear();

	if( (iDamageCnt > 0) && (iDamageCnt < 100) )
	{
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
		UpdateNPCDieRecord( szLastAttackerName, szBestAttackerName );

		if( pDieMonster->eTeam == TEAM_BLUE )
			SetScore( TEAM_RED );
		else
			SetScore( TEAM_BLUE );
	}	

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = (float)iLastDamage / iTotalDamage;
		fBestRate = (float)iBestDamage / iTotalDamage;
	}

	// 몬스터 죽음 처리
	{
		pDieMonster->eState = RS_DIE;
		pDieMonster->dwCurDieTime = TIMEGETTIME();
		TeamSurvivalAIRecord *pSyncUserRecord =  (TeamSurvivalAIRecord*)FindModeRecord( pDieMonster->szSyncUser );
		if( pSyncUserRecord )
			pSyncUserRecord->iMonsterSyncCount = max( 0, pSyncUserRecord->iMonsterSyncCount - 1 );
	}

	SP2Packet kReturn( STPK_DROP_DIE );
	PACKET_GUARD_VOID( kReturn.Write(pDieMonster->szName) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerSkillName) );
	PACKET_GUARD_VOID( kReturn.Write(dwLastAttackerWeaponItemCode) );
	PACKET_GUARD_VOID( kReturn.Write(iLastAttackerTeam) );
	PACKET_GUARD_VOID( kReturn.Write(szBestAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(fLastRate) );
	PACKET_GUARD_VOID( kReturn.Write(fBestRate) );

	if( pDieMonster )
	{
		GetCharModeInfo( kReturn, pDieMonster->szName );
		GetCharModeInfo( kReturn, szLastAttackerName );

		SendRoomAllUser( kReturn );
	}
}

void TeamSurvivalAIMode::OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
	NPCRecord *pDieMonster = (NPCRecord*)FindMonsterInfo( rkDieName );

	if( !m_pCreator )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][teamai]OnWeaponDieNpc Creator is null" );
		return;
	}

	if( !pDieMonster )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][teamai]OnWeaponDieNpc Die Monster is null : [%lu] [%s]", m_pCreator->GetRoomIndex(), rkDieName.c_str() );
		return;
	}

	if( pDieMonster->eState != RS_PLAY )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][teamai]OnWeaponDieNpc None live NPC : [%lu] [%s]", m_pCreator->GetRoomIndex(), rkDieName.c_str() );
		return;
	}

	// NPC가 죽은 위치.
	float fDiePosX = 0, fDiePosY = 0, fDiePosZ = 0;
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosX) );
	//PACKET_GUARD_VOID( rkPacket.Read(fDiePosY) );
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

	// 데미지 리스트 처리
	int iDamageCnt = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iDamageCnt) );

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;
	ioHashString szBestAttackerName;

	static DamageTableList vDamageList;
	vDamageList.clear();

	if( (iDamageCnt > 0) && (iDamageCnt < 100) )
	{
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
		UpdateNPCDieRecord( szLastAttackerName, szBestAttackerName );

		if( pDieMonster->eTeam == TEAM_BLUE )
			SetScore( TEAM_RED );
		else
			SetScore( TEAM_BLUE );
	} 

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = (float)iLastDamage / iTotalDamage;
		fBestRate = (float)iBestDamage / iTotalDamage;
	}

	if( vDamageList.empty() )
	{
		LOG.PrintTimeAndLog(0,"[hack][teamsurvival]DamageList is empty : [%s]", szLastAttackerName.c_str() );
		//User* pUser = g_UserNodeManager.GetUserNode(pDieMonster->szSyncUser);
		return;
	}
	else
	{
		// 몬스터 죽음 처리
		pDieMonster->eState = RS_DIE;
		pDieMonster->dwCurDieTime = TIMEGETTIME();

		TeamSurvivalAIRecord *pSyncUserRecord =  (TeamSurvivalAIRecord*)FindModeRecord( pDieMonster->szSyncUser );
		if( pSyncUserRecord )
			pSyncUserRecord->iMonsterSyncCount = max( 0, pSyncUserRecord->iMonsterSyncCount - 1 );
	}

	SP2Packet kReturn( STPK_WEAPON_DIE );
	PACKET_GUARD_VOID( kReturn.Write(pDieMonster->szName) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(szLastAttackerSkillName) );
	PACKET_GUARD_VOID( kReturn.Write(dwLastAttackerWeaponItemCode) );
	PACKET_GUARD_VOID( kReturn.Write(iLastAttackerTeam) );
	PACKET_GUARD_VOID( kReturn.Write(szBestAttackerName) );
	PACKET_GUARD_VOID( kReturn.Write(fLastRate) );
	PACKET_GUARD_VOID( kReturn.Write(fBestRate) );

	GetCharModeInfo( kReturn, pDieMonster->szName );
	GetCharModeInfo( kReturn, szLastAttackerName );
	SendRoomAllUser( kReturn );	
}

void TeamSurvivalAIMode::LoadMapINIValue()
{	
}

void TeamSurvivalAIMode::LoadNPCInfo( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "teamsurvival_npc" );

	int iMaxNPCCnt = rkLoader.LoadInt( "teamsurvival_npc_cnt", 0 );

	static IntVec vIndex;
	vIndex.clear();

	for( int i=0; i<iMaxNPCCnt; ++i )
		vIndex.push_back( i+1 );

	srand( timeGetTime() );
	io_random_shuffle( vIndex.begin(), vIndex.end() );

	m_vNPCCodeList.clear();

	for( int i=0; i<iMaxNPCCnt; ++i )
	{
		int iNPCID = vIndex[i];
		char szTitle[MAX_PATH];
		wsprintf( szTitle, "teamsurvival_npc%d_code", iNPCID );
		int iNPCCode = rkLoader.LoadInt( szTitle, 0 );

		m_vNPCCodeList.push_back(iNPCCode);
	}


	m_vNPCCodeListHard.clear();
	rkLoader.SetTitle( "teamsurvival_npc_hard" );
	iMaxNPCCnt = rkLoader.LoadInt( "teamsurvival_npc_cnt", 0 );
	vIndex.clear();
	for( int i=0; i<iMaxNPCCnt; ++i )
		vIndex.push_back( i+1 );
	srand( timeGetTime() );
	io_random_shuffle( vIndex.begin(), vIndex.end() );	
	for( int i=0; i<iMaxNPCCnt; ++i )
	{
		int iNPCID = vIndex[i];
		char szTitle[MAX_PATH];
		wsprintf( szTitle, "teamsurvival_npc%d_code", iNPCID );
		int iNPCCode = rkLoader.LoadInt( szTitle, 0 );

		m_vNPCCodeListHard.push_back(iNPCCode);
	}

	rkLoader.SetTitle( "teamsurvival_slot" );
	m_vSlotCount.clear();

	int iMaxSlotCnt = rkLoader.LoadInt( "teamsurvival_slot_cnt", 0 );
	for( int i = 0; i < iMaxSlotCnt; ++i )
	{
		char szTitle[MAX_PATH];
		wsprintf( szTitle, "teamsurvival_map%d_slot", i + 1 );
		int iSlotCnt = rkLoader.LoadInt( szTitle, 4 );
		m_vSlotCount.push_back( iSlotCnt );
	}
}

void TeamSurvivalAIMode::LoadBalancePoint(ioINILoader &rkLoader )
{
	m_vBalancePoint.clear();

	rkLoader.SetTitle( "balance" );

	char szKey[MAX_PATH] = "";
	int iCnt = rkLoader.LoadInt( "balance_point_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		BalancePoint kPoint;

		wsprintf( szKey, "balance_point%d_min_lv", i+1 );
		kPoint.min_lv = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "balance_point%d_max_lv", i+1 );
		kPoint.max_lv = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "balance_point%d_exp_rate", i+1 );
		kPoint.exp_rate = rkLoader.LoadFloat( szKey, 1.0f );

		wsprintf( szKey, "balance_point%d_gold_rate", i+1 );
		kPoint.gold_rate = rkLoader.LoadFloat( szKey, 1.0f );

		m_vBalancePoint.push_back( kPoint );
	}
}

float TeamSurvivalAIMode::GetMonsterStartXPos( float fXPos, int &rRandIndex  )
{
	if( fXPos != 0.0f ) return fXPos;
	if( m_RandomStartPos.empty() ) return fXPos;

	rRandIndex = rand()%(int)m_RandomStartPos.size();
	RandomStartPos &rkRandPos = m_RandomStartPos[rRandIndex];
	if( rkRandPos.m_fStartXRange <= 0.0f ) return fXPos;

	return rkRandPos.m_fStartXPos + ( rand() % (int)rkRandPos.m_fStartXRange );
}

float TeamSurvivalAIMode::GetMonsterStartZPos( float fZPos, int iRandIndex )
{
	if( fZPos != 0.0f ) return fZPos;
	if( m_RandomStartPos.empty() ) return fZPos;
	if( !COMPARE( iRandIndex, 0, (int)m_RandomStartPos.size() ) ) return fZPos;

	RandomStartPos &rkRandPos = m_RandomStartPos[iRandIndex];
	if( rkRandPos.m_fStartZRange <= 0.0f ) return fZPos;

	return rkRandPos.m_fStartZPos - ( rand() % (int)rkRandPos.m_fStartZRange );
}

DWORD TeamSurvivalAIMode::NameGenerator( OUT char *szName, IN int iSize )
{
	DWORD dwRand = rand() % 10000;

	dwRand *= 10000;

	++m_dwGenCount;

	DWORD dwID = m_dwGenCount + dwRand;

	for( DWORD dwIndex = 0; dwIndex < m_vRecordList.size(); dwIndex++ )
	{
		LOOP_GUARD();
		while( true )
		{
			if( dwID != m_vRecordList[dwIndex].pUser->GetUserIndex() )
				break;

			++dwID;
			++m_dwGenCount;
		}
		LOOP_GUARD_CLEAR();
	}	
	StringCbPrintf( szName, iSize, "%d", dwID );
	return dwID;	
}

void TeamSurvivalAIMode::OnRequestStart( User *pUser, SP2Packet &rkPacket )
{
	int iPhase = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iPhase) );

	if ( ModeStart() )
	{
		SetModeState( MS_READY );
	}
}

void TeamSurvivalAIMode::OnRequestStartDev( User *pUser, SP2Packet &rkPacket )
{
	int iPhase = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iPhase) );

	if ( ModeStart() )
	{
		SetModeState( MS_READY );
	}
}

bool TeamSurvivalAIMode::ModeStart()
{
	if( !m_pCreator )
		return false;

	// 유저들 관전 상태 해제
	for(int i = 0;i < GetRecordCnt();i++)
	{
		TeamSurvivalAIRecord* pRecord = static_cast<TeamSurvivalAIRecord*>(FindModeRecord( i ));
		if( pRecord )
		{
			if( pRecord->pUser == NULL ) continue;
			if( pRecord->eState == RS_VIEW || pRecord->eState == RS_OBSERVER ) continue;	
			if( pRecord->pUser->GetTeam() != TEAM_BLUE ) continue;

			pRecord->bClientViewState = false;
			pRecord->PlayTimeInit();
			pRecord->StartPlaying();
			pRecord->dwClassPlayingStartTime = 0;
		}
	}

	m_dwSpawnTime = m_dwStateChangeTime = TIMEGETTIME();

	// 턴 시작시 소환물 / 필드 아이템 제거
	m_vPushStructList.clear();
	m_vBallStructList.clear();
	m_vMachineStructList.clear();
	m_pCreator->DestroyAllFieldItems();

	int iBlueSlotCnt = 0;
	int iRedSlotCnt  = 0;

	// need
	// n vs n 할때 쓸 슬롯 값 정해줘야됨
	iRedSlotCnt = iBlueSlotCnt = GetMapIndexbySlotCount();

	SpawnNPC( iRedSlotCnt, true );

	int iUserCount = GetRealUserCnt();
	int iFriendCount = iRedSlotCnt - iUserCount;
	if( iFriendCount > 0 )
	{
		SpawnNPC( iFriendCount, false );
	}

	return true;
}


void TeamSurvivalAIMode::SpawnNPC( const DWORD dwMaxCount, bool bEnemy )
{
	if( dwMaxCount == 0 )
		return;

	DWORD dwSpawnCount = 0;
	DWORD dwMaxSpawn = dwMaxCount;

	// Get NPC Info. ( Code number )
	// It was selected with shuffle in 'LoadNPCInfo()'
	// see 'TeamSurvivalAIMode.ini  [teamsurvival_npc]  code -> npc_ai_npclist.xml
	// To do : create new ai controle system for the mass production.

	NPCRecordList vNPCList;
	vNPCList.clear();

	for( DWORD dwSpawnCount = 0; dwSpawnCount < dwMaxSpawn; dwSpawnCount++ )
	{
		NPCRecord rkMonster;
		rkMonster.dwCode		= GetNextNPCCode();
		rkMonster.eState		= RS_PLAY;
		rkMonster.szSyncUser	= SearchMonsterSyncUser();
		if( bEnemy )
			rkMonster.eTeam = TEAM_RED;
		else
			rkMonster.eTeam = TEAM_BLUE;

		int iRandIndex = 0;
		rkMonster.fStartXPos	= 0.f;
		rkMonster.fStartZPos	= 0.f;


		char szNpcName[MAX_PATH] = "";
		rkMonster.dwMonsterID = NameGenerator( szNpcName, sizeof(szNpcName) );
		rkMonster.iRoomIndex = m_pCreator->GetRoomIndex();
		rkMonster.szName = szNpcName;

		InitNPCEquipItem( rkMonster );

		m_vNPCList.push_back( rkMonster );
		vNPCList.push_back( rkMonster );
	}

	SP2Packet kPacket( STPK_SPAWN_AI_MONSTER );

	PACKET_GUARD_VOID( kPacket.Write((int)vNPCList.size()) );

	BOOST_FOREACH( NPCRecord& kMonster, vNPCList )
	{
		PACKET_GUARD_VOID( kPacket.Write(kMonster.dwCode) );
		PACKET_GUARD_VOID( kPacket.Write(kMonster.dwMonsterID) );
		PACKET_GUARD_VOID( kPacket.Write((int)kMonster.eTeam) );
		PACKET_GUARD_VOID( kPacket.Write(kMonster.szName) );
		PACKET_GUARD_VOID( kPacket.Write(kMonster.szSyncUser) );
		PACKET_GUARD_VOID( kPacket.Write(kMonster.fStartXPos) );
		PACKET_GUARD_VOID( kPacket.Write(kMonster.fStartZPos) );

		//장비 코드 4개 무조건 넣어주기
		for( int i = 0; i < 4; ++i )
		{
			int gameIndex = 0;
			if( kMonster.m_pEquipSlot )
			{
				const ioItem* pItem = kMonster.m_pEquipSlot->GetItem( i );
				if( pItem )
				{
					gameIndex = pItem->GetGameIndex();

				}
			}
			PACKET_GUARD_VOID( kPacket.Write(gameIndex) );
		}
	}

	SendRoomAllUser( kPacket );	
}

const ioHashString &TeamSurvivalAIMode::SearchMonsterSyncUser()
{
	static ioHashString szError = "NonSync";
	if( m_vRecordList.empty() ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][teamsurvivalai]Search monster sync user : none user record [%d] ", m_pCreator->GetRoomIndex() );
		return szError;
	}

	// 싱크 조건을 유저가 동등하게 분류, 같으면 핑이 낮은 쪽으로..
	// 핑이 100 이상이면 싱크 카운트 1로 구분

	int iPoint = 999;	
	TeamSurvivalAIRecord* pReturnRecord = NULL;
	TeamSurvivalAIRecordList::iterator iter = m_vRecordList.begin();
	for(;iter != m_vRecordList.end();iter++)
	{
		TeamSurvivalAIRecord &rkRecord = *iter;
		if( rkRecord.pUser == NULL ) continue;
		//if( rkRecord.eState != RS_PLAY ) continue;
		if( rkRecord.pUser->GetTeam() != TEAM_BLUE ) continue;

		int syncCount = rkRecord.iMonsterSyncCount + rkRecord.pUser->GetPingStep();
		if( syncCount < iPoint )
		{
			iPoint = syncCount;
			pReturnRecord = &rkRecord;
		}
	}

	if( pReturnRecord == NULL || pReturnRecord->pUser == NULL )
	{
		if( m_pCreator )
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][teamAI] none return record : [roomIndex:%d, recordSize:%d]", m_pCreator->GetRoomIndex(), m_vRecordList.size() );
		return szError;
	}

	// 몬스터 한마리 동기화 추가~
	pReturnRecord->iMonsterSyncCount++;
	return pReturnRecord->pUser->GetPublicID();
}

void TeamSurvivalAIMode::ReviveNPC()
{
	//NPCRecordList vNPCList;
	std::vector< NPCRecord > vNPCList;
	vNPCList.clear();

	BOOST_FOREACH( NPCRecord& rkMonster, m_vNPCList )
	{
		if( rkMonster.eState == RS_DIE )
		{
			DWORD curTime = TIMEGETTIME();
			if( curTime - rkMonster.dwCurDieTime > 2000 )
			{
				rkMonster.eState = RS_PLAY;
				rkMonster.dwCode = GetNextNPCCode();
				InitNPCEquipItem( rkMonster );

				NPCRecord rkReviveMonster = rkMonster;
				vNPCList.push_back(rkReviveMonster);
			}
		}
	}

	if( vNPCList.size() > 0 )
	{
		SP2Packet kPacket( STPK_SPAWN_AI_MONSTER );

		PACKET_GUARD_VOID( kPacket.Write((int)vNPCList.size()) );

		BOOST_FOREACH( NPCRecord& rkMonster, vNPCList )
		{
			PACKET_GUARD_VOID( kPacket.Write(rkMonster.dwCode) );
			PACKET_GUARD_VOID( kPacket.Write(rkMonster.dwMonsterID) );
			PACKET_GUARD_VOID( kPacket.Write(rkMonster.eTeam) );
			PACKET_GUARD_VOID( kPacket.Write(rkMonster.szName) );
			PACKET_GUARD_VOID( kPacket.Write(rkMonster.szSyncUser) );
			PACKET_GUARD_VOID( kPacket.Write(rkMonster.fStartXPos) );
			PACKET_GUARD_VOID( kPacket.Write(rkMonster.fStartZPos) );

			//장비 코드 4개 무조건 넣어주기
			for( int i = 0; i < 4; ++i )
			{
				int gameIndex = 0;
				if( rkMonster.m_pEquipSlot )
				{
					const ioItem* pItem = rkMonster.m_pEquipSlot->GetItem( i );
					if( pItem )
					{
						gameIndex = pItem->GetGameIndex();

					}
				}
				PACKET_GUARD_VOID( kPacket.Write(gameIndex) );
			}
		}
		SendRoomAllUser( kPacket );	
	}
}

DWORD TeamSurvivalAIMode::GetLiveMonsterCount( )
{
	int iMonsterCount = 0;

	BOOST_FOREACH( NPCRecord& rMonster, m_vNPCList )
	{
		if( rMonster.eState != RS_PLAY )
			continue;

		++iMonsterCount;
	}

	return iMonsterCount;
}

DWORD TeamSurvivalAIMode::GetDeadMonsterCount( )
{
	int iMonsterCount = 0;

	BOOST_FOREACH( NPCRecord& rMonster, m_vNPCList )
	{
		if( rMonster.eState != RS_DIE )
			continue;

		++iMonsterCount;
	}

	return iMonsterCount;
}

NPCRecord* TeamSurvivalAIMode::FindMonsterInfo( const ioHashString &rkName )
{
	BOOST_FOREACH( NPCRecord& rMonster, m_vNPCList )
	{
		if( rMonster.szName == rkName )
			return &rMonster;
	}

	return NULL;
}

void TeamSurvivalAIMode::RemoveRecordChangeMonsterSync( const ioHashString &rkRemoveName )
{
	if( m_bRoundSetEnd ) return;

	NPCRecordList RecordList;

	BOOST_FOREACH( NPCRecord& rMonster, m_vNPCList )
	{
		if( rMonster.szSyncUser == rkRemoveName )
			rMonster.szSyncUser = SearchMonsterSyncUser();

		RecordList.push_back( rMonster);
	}

	int iSyncSize = RecordList.size();
	SP2Packet kPacket( STPK_MONSTER_SYNC_CHANGE );
	PACKET_GUARD_VOID( kPacket.Write( iSyncSize ) );
	for(int i = 0;i < iSyncSize;i++)
	{
		PACKET_GUARD_VOID( kPacket.Write(RecordList[i].szName) );
		PACKET_GUARD_VOID( kPacket.Write(RecordList[i].szSyncUser) );
	}
	SendRoomAllUser( kPacket );
}

DWORD TeamSurvivalAIMode::GetNextNPCCode()
{
	if( m_iLevel == 0 )
	{
		if( m_vNPCCodeList.empty() )
			return 0;
		int nSize = m_vNPCCodeList.size();
		m_dwNextNPCCode++;
		if( (int)m_dwNextNPCCode >= nSize )
			m_dwNextNPCCode = 0;

		return m_vNPCCodeList[m_dwNextNPCCode];
	}

	if( m_vNPCCodeListHard.empty() )
		return 0;
	int nSize = m_vNPCCodeListHard.size();
	m_dwNextNPCCode++;
	if( (int)m_dwNextNPCCode >= nSize )
		m_dwNextNPCCode = 0;

	return m_vNPCCodeListHard[m_dwNextNPCCode];	
}

void TeamSurvivalAIMode::OnDropItemNpc( User* pSendUser, const ioHashString &rkOwnerID, SP2Packet &rkPacket )
{
	NPCRecord* pNPCRecord = FindMonsterInfo( rkOwnerID );
	if( !pNPCRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][TeamSurvivalAI] dropitemnpc none npc : %d - %s", m_pCreator->GetRoomIndex(), rkOwnerID.c_str() );
		return;
	}

	int iGameIndex = 0, iItemCode = 0, iSlot = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iGameIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iItemCode) );
	PACKET_GUARD_VOID( rkPacket.Read(iSlot) );

	ioItem* pPreItem = ReleaseItem( pNPCRecord, iSlot );

	if( !pPreItem || !IsCanPickItemState() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][headquarters] dropitemnpc not has item : %s - %d", rkOwnerID.c_str(), iItemCode );

		if( pSendUser )
		{
			SP2Packet kFailReturn( STPK_DROP_ITEM_FAIL );
			PACKET_GUARD_VOID( kFailReturn.Write(rkOwnerID) );
			PACKET_GUARD_VOID( kFailReturn.Write(iSlot) );
			pSendUser->SendMessage( kFailReturn );
		}
		return;
	}

	float fCurGauge = 0;
	PACKET_GUARD_VOID( rkPacket.Read(fCurGauge) );
	pPreItem->SetCurItemGauge( fCurGauge );

	int iCurBullet = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iCurBullet) );

	Vector3 vDropPos;
	PACKET_GUARD_VOID( rkPacket.Read(vDropPos) );
	pPreItem->SetItemPos( vDropPos );

	bool bDropZone = false;
	PACKET_GUARD_VOID( rkPacket.Read(bDropZone) );

	if( pPreItem->IsNotDeleteItem() && pPreItem->GetCrownItemType() == ioItem::MCT_NONE && bDropZone )
	{
		pPreItem->SetEnableDelete( false );
	}
	m_pCreator->DropItemOnField( rkOwnerID, pPreItem, iSlot, fCurGauge, iCurBullet );
}

void TeamSurvivalAIMode::InitNPCEquipItem( NPCRecord& rkMonster )
{	
	if( rkMonster.m_pEquipSlot == NULL )
		rkMonster.m_pEquipSlot = new ioEquipSlot;

	//코드에 따라서 장비코드 세팅
	ioEquipSlot* pSlot = rkMonster.m_pEquipSlot;	
	if( pSlot )
	{
		for( int i = 0; i < 4; ++i )
		{
			ITEM_DATA kItemData = GetEquipItemData( rkMonster.dwCode, i );
			ioItem* pNewItem = m_pCreator->CreateItem( kItemData, rkMonster.szName );
			pSlot->EquipItem( i, pNewItem );
		}
	}
}

ITEM_DATA TeamSurvivalAIMode::GetEquipItemData( DWORD dwNPCCode, int iSlot )
{
	ITEM_DATA itemData;

	auto it = m_mapItemCode.find( dwNPCCode );
	if( it != m_mapItemCode.end() )
	{
		sEquipItemCode eItemCode = it->second;
		if( COMPARE( iSlot, 0, 4 ) )
		{
			itemData.m_item_code = eItemCode.dwItemCode[iSlot];		
		}
	}
	else
	{
		//test
		it = m_mapItemCode.begin();

		if( it == m_mapItemCode.end() )
			return itemData;
		sEquipItemCode eItemCode = it->second;
		if( COMPARE( iSlot, 0, 4 ) )
		{
			itemData.m_item_code = eItemCode.dwItemCode[iSlot];		
		}

	}

	return itemData;
}

ioItem* TeamSurvivalAIMode::ReleaseItem( NPCRecord* pRecord, int iSlot )
{
	if( !pRecord->m_pEquipSlot )
		return NULL;

	return pRecord->m_pEquipSlot->ReleaseItem( iSlot );
}

void TeamSurvivalAIMode::LoadNPCEquipInfo( ioINILoader& kLoader )
{
	//장비코드 추가
	kLoader.SetTitle( "Common" );
	int iEquipCount = kLoader.LoadInt( "count", 0 );
	for( int i = 0; i < iEquipCount; ++i )
	{
		char szTitle[MAX_PATH];
		wsprintf( szTitle, "equipCode%d", i );
		kLoader.SetTitle( szTitle );

		DWORD dwNPCCode = kLoader.LoadInt( "npccode", 0 );
		sEquipItemCode eItemCode;		
		for( int j = 0; j < 4; ++j )
		{
			char szKey[MAX_PATH];
			wsprintf( szKey, "itemcode%d", j );
			eItemCode.dwItemCode[j] = kLoader.LoadInt( szKey, 0 );
		}

		m_mapItemCode.insert( std::map<DWORD,sEquipItemCode>::value_type(dwNPCCode,eItemCode) );
	}
}

void TeamSurvivalAIMode::OnDropDieUser( User *pDieUser, SP2Packet &rkPacket )
{
	if( pDieUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[error][teamai] dropdieuser user is null" );
		return;
	}

	if( !m_pCreator )
		return;

	float fDiePosX = 0, fDiePosY = 0, fDiePosZ = 0;
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosX) );
	//PACKET_GUARD_VOID( rkPacket.Read(fDiePosY) );
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosZ) );

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
	if( !pRecord->pUser ) return;
	if( pRecord->eState == RS_LOADING ) return;

	pRecord->SetDieLastAttackerInfo( szLastAttackerName, (TeamType)iLastAttackerTeam, dwLastAttackerWeaponItemCode );

	UpdateDieState( pRecord->pUser );

	int iDamageCnt = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iDamageCnt) );

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;
	ioHashString szBestAttackerName;

	static DamageTableList vDamageList;
	vDamageList.clear();

	if( (iDamageCnt > 0) && (iDamageCnt < 100) )
	{
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
		UpdateDropDieRecord( pRecord->pUser, szLastAttackerName, szBestAttackerName );
	}	

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = (float)iLastDamage / iTotalDamage;
		fBestRate = (float)iBestDamage / iTotalDamage;
	}

	SP2Packet kReturn( STPK_DROP_DIE );
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

void TeamSurvivalAIMode::OnWeaponDieUser( User *pDieUser, SP2Packet &rkPacket )
{
	if( pDieUser == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "[error][teamsurvival] weapondieuser user is null" );
		return;
	}

	if( !m_pCreator )
		return;

	// 유저가 죽은 위치.
	float fDiePosX = 0, fDiePosY = 0, fDiePosZ = 0;
	PACKET_GUARD_VOID( rkPacket.Read(fDiePosX) );
	//PACKET_GUARD_VOID( rkPacket.Read(fDiePosY) );
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
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][TeamSurvival] user no record : (%s)", pDieUser->GetPublicID().c_str() );
		return;
	}
	if( !pRecord->pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][TeamSurvival] user no record user : (%s)", pDieUser->GetPublicID().c_str() );
		return;
	}
	if( pRecord->eState == RS_LOADING )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][TeamSurvival] user RS_LOADING : (%s)", pDieUser->GetPublicID().c_str() );
		return;
	}

	UpdateDieState( pRecord->pUser );

	if( pRecord->pUser->IsEquipedItem() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][TeamSurvival] user EQUIPEDItem : (%s)", pDieUser->GetPublicID().c_str() );
	}
	pRecord->SetDieLastAttackerInfo( szLastAttackerName, (TeamType)iLastAttackerTeam, dwLastAttackerWeaponItemCode );

	int iDamageCnt = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iDamageCnt) );

	int iTotalDamage = 0;
	int iLastDamage = 0;
	int iBestDamage = 0;
	ioHashString szBestAttackerName;

	static DamageTableList vDamageList;
	vDamageList.clear();

	if( (iDamageCnt > 0) && (iDamageCnt < 100) )
	{
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
		fLastRate = (float)iLastDamage / iTotalDamage;
		fBestRate = (float)iBestDamage / iTotalDamage;
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

DWORD TeamSurvivalAIMode::GetMapIndexbySlotCount()
{
	int nSlotCount = 2;
	int iCurMapIndex = GetModeSubNum();
	iCurMapIndex = iCurMapIndex - 1;
	int vSize = m_vSlotCount.size();
	if( COMPARE( iCurMapIndex, 0, vSize ) )
	{
		nSlotCount = m_vSlotCount[iCurMapIndex];
	}
	return nSlotCount;
}

int TeamSurvivalAIMode::GetRealUserCnt()
{
	int iUserCnt = 0;
	int nSize = m_vRecordList.size();
	for( int i = 0; i < nSize; ++i )
	{
		if( m_vRecordList[i].pUser )
		{
// 			if( m_vRecordList[i].eState != RS_OBSERVER )
			if( m_vRecordList[i].pUser->GetTeam() == TEAM_BLUE )
				iUserCnt++;
		}
	}
	return iUserCnt;
}

void TeamSurvivalAIMode::SetAILevel( int iLevel )
{
	m_iLevel = iLevel;
}