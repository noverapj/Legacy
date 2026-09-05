#pragma once
#include "stdafx.h"

#include "../MainProcess.h"

#include "User.h"
#include "Room.h"
#include "ShuffleRoomNode.h"
#include "ShuffleRoomManager.h"
#include "ShuffleBonusModeHelp.h"
#include "ShuffleBonusMode.h"

#include "ioSetItemInfoManager.h"
#include "ioItemInfoManager.h"
#include "ioSetItemInfo.h"

#include "RoomNodeManager.h"
#include "ioMonsterMapLoadMgr.h"
#include "ModeItemCrator.h"

#include "../EtcHelpFunc.h"
#include "../DataBase/LogDBClient.h"

#include <iterator>     // std::back_inserter

ShuffleBonusMode::ShuffleBonusMode( Room *pCreator ) : Mode( pCreator )
{	
	m_SingleTeamList.clear();
	m_SingleTeamPosArray.clear();
}

ShuffleBonusMode::~ShuffleBonusMode()
{
}

void ShuffleBonusMode::LoadINIValue()
{
	Mode::LoadINIValue();
		
	m_iCurStarRegenPos = 0;
	m_iMaxStarRegenCnt = 0;
	m_iCurStarRegenCnt = 0;
	m_iStarCount = 0;	
	
	LoadShuffleItemInfo();
	LoadShuffleStarPosInfo();
	LoadNpc();

	m_dwCurRoundDuration = m_dwRoundDuration;
}

void ShuffleBonusMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "info" );
	m_iMaxPlayer = min( MAX_PLAYER, rkLoader.LoadInt( "max_player", MAX_PLAYER ) );
	m_iConstMaxPlayer = min( MAX_PLAYER, rkLoader.LoadInt( "const_max_player", MAX_PLAYER ) );

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

	// 라운드만큼 버퍼 생성.
	m_vRoundHistory.clear();
	for(int i = 0; i < m_iMaxRound;i++)
	{
		RoundHistory rh;
		m_vRoundHistory.push_back( rh );
	}

	m_SingleTeamList.clear();
	for( int i=TEAM_PRIVATE_1;i < TEAM_PRIVATE_16+1;i++)
	{
		m_SingleTeamList.push_back( i );
	}

	m_dwRemainTime = rkLoader.LoadInt( "remain_time", 0 );
	SetStartPosArray();

	LoadWoundedInfo( rkLoader );
}

void ShuffleBonusMode::LoadWoundedInfo( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "wounded" );

	char szBuf[MAX_PATH];
	::memset( szBuf, 0, MAX_PATH );

	int iCount = rkLoader.LoadInt( "wounded_drop_count", 0 );
	for( int i = 0; i < iCount; i++ )
	{
		WoundedDropInfo Info;
		sprintf_s( szBuf, "wounded_drop%d_blow_type", i+1 );
		Info.m_BlowType = (BlowTargetType)rkLoader.LoadInt( szBuf, (int)BTT_NONE );

		sprintf_s( szBuf, "wounded_drop%d_star_count", i+1 );
		Info.m_iDropCount = rkLoader.LoadInt( szBuf, 0 );

		if( Info.IsEmpty() )
			continue;

		m_WoundedDropInfoVec.push_back( Info );
	}
}

void ShuffleBonusMode::LoadShuffleItemInfo()
{
	ioINILoader kLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	kLoader.SetTitle( "star_item" );

	m_dwStarActivateTime = (DWORD)kLoader.LoadInt( "star_activate_time", 0 );
	m_dwStarWaitTime	 = (DWORD)kLoader.LoadInt( "star_wait_time", 0 );

	m_fFloatPowerMin = kLoader.LoadFloat( "star_float_power_min", 500.0f );
	m_fFloatPowerMax = kLoader.LoadFloat( "star_float_power_max", 1000.0f );
	m_fFloatSpeedMin = kLoader.LoadFloat( "star_float_speed_min", 300.0f );
	m_fFloatSpeedMax = kLoader.LoadFloat( "star_float_speed_max", 500.0f );

	m_fDropDieStarDecreaseRate = kLoader.LoadFloat( "drop_die_decrease_rate", 0.50f );
	m_fDropStarDecreaseRate    = kLoader.LoadFloat( "drop_decrease_rate", 0.25f );

	kLoader.SetTitle( "buff_item" );
		
	m_dwBuffItemCreateTime = kLoader.LoadInt( "call_buff_create_time", 0 );
	m_dwBuffItemRegenTime  = kLoader.LoadInt( "call_buff_regen_time", 0 );
	m_dwBuffItemWaitTime   = kLoader.LoadInt( "call_buff_wait_time", 0 );
	m_dwBuffRandMaxRange   = kLoader.LoadInt( "call_buff_max_range", 0 );
}

void ShuffleBonusMode::LoadShuffleStarPosInfo()
{
	char szFileName[MAX_PATH] = "";
	sprintf_s( szFileName, "config/sp2_shufflebonus_mode%d_map%d.ini", GetModeSubNum(), GetModeMapNum() );
	ioINILoader kLoader( szFileName );
	if( !kLoader.IsLoadComplete() )
	{
		return;
	}

	char szKey[MAX_PATH] = "";
	kLoader.SetTitle( "shuffle_bonus_star" );

	m_iMaxStarRegenPos = kLoader.LoadInt( "max_regen_pos", 0 );

	m_vShuffleStarRegenPos.reserve( m_iMaxStarRegenPos );

	for( int i=0; i< m_iMaxStarRegenPos; ++i )
	{		
		sprintf_s( szKey, "regen%d_x", i+1 );
		float fXPos = kLoader.LoadFloat( szKey, 0.0f );

		sprintf_s( szKey, "regen%d_z", i+1 );
		float fZPos = kLoader.LoadFloat( szKey, 0.0f );

		ShuffleStarRegenPos kStarInfo;
		kStarInfo.m_fXPos = fXPos;
		kStarInfo.m_fZPos = fZPos;
		m_vShuffleStarRegenPos.push_back( kStarInfo );

	}
	io_random_shuffle( m_vShuffleStarRegenPos.begin(), m_vShuffleStarRegenPos.end() );
	std::copy( m_vShuffleStarRegenPos.begin(), m_vShuffleStarRegenPos.end(), std::back_inserter( m_vBuffItemCreatePos ) );	
}

void ShuffleBonusMode::SetStartPosArray()
{
	int iSinglePosCnt = m_SingleTeamList.size();
	if(iSinglePosCnt <= 1) // 최소한 2개 필요
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Error 2 - ShuffleBonusMode::GetStartPosArray");
		return;
	}

	m_SingleTeamPosArray.reserve(iSinglePosCnt);
	for( int i=0; i<iSinglePosCnt; i++ )
	{
		m_SingleTeamPosArray.push_back(i);
	}

	io_random_shuffle( m_SingleTeamPosArray.begin(), m_SingleTeamPosArray.end() );

	m_iBluePosArray = m_SingleTeamPosArray[0];
	m_iRedPosArray  = m_SingleTeamPosArray[1];
}


void ShuffleBonusMode::LoadNpc()
{
	ioINILoader kLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	kLoader.SetTitle( "npc" );
	int iNpcCnt = kLoader.LoadInt( "npc_cnt", 0 );	

	for( int i = 0; i < iNpcCnt; i++ )
	{
		char szKey[MAX_PATH] = "";

		NpcInfo Info;

		sprintf_s( szKey, "npc%d_rand_table", i+1 );
		Info.m_iRandTable = kLoader.LoadInt( szKey, 1 );

		sprintf_s( szKey, "npc%d_alive_time", i+1 );
		Info.m_dwAliveTime = kLoader.LoadInt( szKey, 7200000 );  // 디폴트 2시간

		m_NpcInfoVec.push_back( Info );
	}
}

void ShuffleBonusMode::DestroyMode()
{
	Mode::DestroyMode();
	DestoryModeItemByStar();
	m_vRecordList.clear();
}

void ShuffleBonusMode::DestoryModeItemByStar()
{
	for( ModeItemVec::iterator iter = m_vDropZoneStar.begin(); iter != m_vDropZoneStar.end(); ++iter )
	{
		ModeItem* pItem = *iter;
		if( pItem )
			SAFEDELETE( pItem );
	}

	m_vDropZoneStar.clear();
	m_iDropZoneUserStar = 0;
}

void ShuffleBonusMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "survival%d_object_group%d", iSubNum, iGroupNum );
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
	PACKET_GUARD_VOID( kPacket.Write( iNewItemCnt ) );

	for(int i=0; i<iNewItemCnt; i++ )
	{
		ioItem *pItem = vItemList[i];
		m_pCreator->AddFieldItem( pItem );
		PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemCode() ) );
		PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemReinforce() ) );
		PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemMaleCustom() ) );
		PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemFemaleCustom() ) );
		PACKET_GUARD_VOID( kPacket.Write( pItem->GetGameIndex() ) );
		PACKET_GUARD_VOID( kPacket.Write( pItem->GetItemPos() ) );
		PACKET_GUARD_VOID( kPacket.Write( pItem->GetOwnerName() ) );
		PACKET_GUARD_VOID( kPacket.Write( "" ) );
	}

	SendRoomAllUser( kPacket );
}

void ShuffleBonusMode::AddNewRecord( User *pUser )
{
	ShuffleBonusRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	RemoveTeamType( pUser->GetTeam() );
	UpdateUserRank();

	PlayMonsterSync( &kRecord );
}

void ShuffleBonusMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	if(!pUser) return;
	
	int iCharCnt = m_vRecordList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vRecordList[i].pUser == pUser )
		{
			SetModeEndDBLog( &m_vRecordList[i], iCharCnt, LogDBClient::PRT_EXIT_ROOM );
			AddTeamType( pUser->GetTeam() );
			m_vRecordList.erase( m_vRecordList.begin() + i );
			break;
		}
	}

	UpdateUserRank();
	
	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );		
		RemoveRecordChangeMonsterSync( pUser->GetPublicID() );		
	}
}

void ShuffleBonusMode::AddTeamType( TeamType eTeam )
{
	if( COMPARE( eTeam, TEAM_PRIVATE_1, TEAM_PRIVATE_16+1 ) )
		m_SingleTeamList.push_back( eTeam ); 
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleBonusMode::AddTeamType %d", eTeam );
}

void ShuffleBonusMode::RemoveTeamType( TeamType eTeam )
{
	int iTeamSize = m_SingleTeamList.size();
	for(int i = 0;i < iTeamSize;i++)
	{
		if( m_SingleTeamList[i] == eTeam )
		{
			m_SingleTeamList.erase( m_SingleTeamList.begin() + i );
			break;
		}
	}
}


void ShuffleBonusMode::ProcessReady()
{
	DWORD dwCurTime = TIMEGETTIME();
	if( m_dwStateChangeTime + m_dwReadyStateTime >= dwCurTime )
		return;
	// 유저 입장 대기중이면 플레이 상태로 전환하지 않는다.
	if( m_pCreator->IsRoomEnterUserDelay() )
		return;

	SP2Packet kPacket( STPK_ROUND_START );
	PACKET_GUARD_VOID( kPacket.Write( m_iCurRound ) );	
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

	// 별 등장 빈도 / 갯수 셋팅.
	CalcStarCorrection();
		
	// 레디 상태에서 이탈한 유저에 대한 체크
	CheckUserLeaveEnd();

	//시작하자마자 생성되도록함
	DWORD dwTime = TIMEGETTIME();
	m_dwStarRegenCheckTime = TIMEGETTIME();
	m_dwCurrBuffItemCreateTime = dwTime + m_dwBuffItemCreateTime;
	
	DestoryModeItemByStar();
}

void ShuffleBonusMode::ProcessPlay()
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
	ProcessGenerateNpc();

	ProcessGenerateBonusStar();
	ProcessGenerateBuffItem();
	ProcessRegenBuffItem();
}

void ShuffleBonusMode::CheckRoundEnd( bool bProcessCall )
{
	if( !bProcessCall && m_pCreator->GetPlayUserCnt() < 2 )
	{
		m_dwCurRoundDuration = 0;

		WinTeamType eWinTeam = WTT_DRAW;
		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
		return;
	}

	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;

	if( m_dwCurRoundDuration < dwGapTime+1000 )
	{
		m_dwCurRoundDuration = 0;

		WinTeamType eWinTeam = WTT_DRAW;
		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
	}
}

void ShuffleBonusMode::SetRoundEndInfo( WinTeamType eWinTeam )
{
	m_CurRoundWinTeam = eWinTeam;
	m_bRoundSetEnd = true;
	m_bCheckContribute = false;
	m_bCheckAwardChoose = false;
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

void ShuffleBonusMode::UpdateRoundRecord()
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
}

void ShuffleBonusMode::UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	Mode::UpdateDropDieRecord( pDier, szAttacker, szBestAttacker );

	if( !pDier )
		return;

	ShuffleBonusRecord *pRecord = FindShuffleBonusRecord( pDier->GetPublicID().c_str() );
	if( pRecord )
	{
		int iDecreaseStar = (int)max( 0.0f, (float)pRecord->m_iStarCount * m_fDropDieStarDecreaseRate );
		if( iDecreaseStar > pRecord->m_iStarCount )
		{		
			iDecreaseStar = pRecord->m_iStarCount;
		}

		pRecord->m_iStarCount = max(0, pRecord->m_iStarCount - iDecreaseStar );
		m_iDropZoneUserStar += iDecreaseStar;
	}
}

ModeType ShuffleBonusMode::GetModeType() const
{
	return MT_SHUFFLE_BONUS;
}

void ShuffleBonusMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck )
{
	ShuffleBonusRecord *pRecord = FindShuffleBonusRecord( szName );
	if( pRecord )
	{
		// 레코드 정보 유무
		PACKET_GUARD_VOID( rkPacket.Write( true ) );

		int iKillSize = pRecord->iKillInfoMap.size();		
		PACKET_GUARD_VOID( rkPacket.Write( iKillSize ) );

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_k = pRecord->iKillInfoMap.begin();
		while( iter_k != pRecord->iKillInfoMap.end() )
		{
			PACKET_GUARD_VOID( rkPacket.Write( iter_k->first ) );
			PACKET_GUARD_VOID( rkPacket.Write( iter_k->second ) );

			++iter_k;
		}
		LOOP_GUARD_CLEAR();

		int iDeathSize = pRecord->iDeathInfoMap.size();
		PACKET_GUARD_VOID( rkPacket.Write( iDeathSize ) );

		LOOP_GUARD();
		KillDeathInfoMap::iterator iter_d = pRecord->iDeathInfoMap.begin();
		while( iter_d != pRecord->iDeathInfoMap.end() )
		{
			PACKET_GUARD_VOID( rkPacket.Write( iter_d->first ) );
			PACKET_GUARD_VOID( rkPacket.Write( iter_d->second ) );

			++iter_d;
		}
		LOOP_GUARD_CLEAR();

		if( bDieCheck )
		{
			PACKET_GUARD_VOID( rkPacket.Write( pRecord->bDieState ) );
		}
		
		PACKET_GUARD_VOID( rkPacket.Write( pRecord->m_iStarCount ) );
	}
	else
	{
		// 레코드 정보 유무
		PACKET_GUARD_VOID( rkPacket.Write( false ) );
	}
}

int ShuffleBonusMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* ShuffleBonusMode::GetModeINIFileName() const
{
	return "config/ShuffleBonusMode.ini";
}

void ShuffleBonusMode::GetModeInfo( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write( GetModeType() ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iCurRound ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_iMaxRound ) );

	PACKET_GUARD_VOID( rkPacket.Write( m_dwRoundDuration ) );

	int iPosCnt = m_SingleTeamPosArray.size();
	PACKET_GUARD_VOID( rkPacket.Write( iPosCnt ) );

	for( int i=0; i < iPosCnt; ++i )
		PACKET_GUARD_VOID( rkPacket.Write( m_SingleTeamPosArray[i] ) );
}

TeamType ShuffleBonusMode::GetNextTeamType()
{
	if( m_SingleTeamList.empty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ShuffleBonusMode::GetNextTeamType Not Team : %d", (int)m_vRecordList.size() );
		return TEAM_NONE;
	}

	return (TeamType)m_SingleTeamList[0];
}

int ShuffleBonusMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
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
			if( !pRecord || !pRecord->pUser )
				continue;

			if( pRecord->pUser->IsObserver() )
				continue;
			if( pRecord->pUser->IsStealth() ) 
				continue;

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

ModeRecord* ShuffleBonusMode::FindModeRecord( const ioHashString &rkName )
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

ModeRecord* ShuffleBonusMode::FindModeRecord( User *pUser )
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

ModeRecord* ShuffleBonusMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

ModeItem* ShuffleBonusMode::FindModeItemByStarDrop( DWORD dwModeItemIndex )
{	
	if( m_vDropZoneStar.empty() )
		return NULL;

	for( ModeItemVec::iterator iter = m_vDropZoneStar.begin(); iter != m_vDropZoneStar.end(); ++iter )
	{
		ModeItem* pItem = *iter;
		if( pItem && pItem->m_dwModeItemIdx == dwModeItemIndex )
			return pItem;
	}

	return NULL;
}

ShuffleBonusRecord* ShuffleBonusMode::FindShuffleBonusRecord( DWORD dwIndex )
{
	return (ShuffleBonusRecord*)FindModeRecord( dwIndex );
}

ShuffleBonusRecord* ShuffleBonusMode::FindShuffleBonusRecord( const ioHashString &rkName )
{
	return (ShuffleBonusRecord*)FindModeRecord( rkName );
}

ShuffleBonusRecord* ShuffleBonusMode::FindShuffleBonusRecord( User *pUser )
{
	return (ShuffleBonusRecord*)FindModeRecord( pUser );
}

ShuffleBonusRecord* ShuffleBonusMode::FindShuffleBonusRecordByUserID( const ioHashString &rkUserID )
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

int ShuffleBonusMode::GetCurTeamUserCnt( TeamType eTeam )
{
	return 1;
}

void ShuffleBonusMode::UpdateUserDieTime( User *pDier )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( pDieRecord && pDieRecord->bDieState )
	{
		DWORD dwRevivalGap = (DWORD)GetRevivalGapTime( pDieRecord->iRevivalCnt );
		pDieRecord->dwCurDieTime = TIMEGETTIME();
		pDieRecord->dwRevivalGap = dwRevivalGap;
		pDieRecord->iRevivalCnt++;
	}
}

bool ShuffleBonusMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_SHUFFLEROOM_USER_BLOW:
		OnUserShuffleRoomUserBlow( rkPacket );
		return true;
	case CTPK_SHUFFLEROOM_DROPZONE:
		OnDropZoneDrop( rkPacket );
		return true;
	}

	return false;
}

void ShuffleBonusMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd )
		return;

	if( m_pCreator->GetPlayUserCnt() < 2 )
		CheckRoundEnd( false );
}

void ShuffleBonusMode::ProcessGenerateBonusStar()
{
	if( m_iCurStarRegenCnt >= m_iMaxStarRegenCnt )
		return;

	if( m_dwStarRegenCheckTime > TIMEGETTIME() )
		return;

	if( m_vShuffleStarRegenPos.empty() )
		return;

	if( m_ModeState != MS_PLAY )
		return;

	IORandom eRandom;
	eRandom.Randomize();

	m_dwStarRegenCheckTime = TIMEGETTIME() + m_dwStarRegenTime;

	int iDropZoneStar = m_vDropZoneStar.size();	

	DWORD dwActivateTime = 0;
	int iCount = m_iStarCount + iDropZoneStar + m_iDropZoneUserStar;
	
	SP2Packet kPacket( STPK_CREATE_MODE_ITEM );

	PACKET_GUARD_VOID( kPacket.Write( static_cast<int>( MIT_SHUFFLE_STAR ) ) );
	PACKET_GUARD_VOID( kPacket.Write( iCount ) );
	PACKET_GUARD_VOID( kPacket.Write( m_dwStarWaitTime ) );
	
	LOOP_GUARD();
	for( int i = 0; i < iCount; ++i )
	{		
		ModeItem* pItem = CreateModeItem( MIT_SHUFFLE_STAR );
		if( !pItem )		
			continue;		

		PACKET_GUARD_VOID( kPacket.Write( pItem->m_dwModeItemIdx ) );		

		PACKET_GUARD_VOID( kPacket.Write( m_vShuffleStarRegenPos[m_iCurStarRegenPos].m_fXPos ) );
		PACKET_GUARD_VOID( kPacket.Write( m_vShuffleStarRegenPos[m_iCurStarRegenPos].m_fZPos ) );

		int iAngle = eRandom.Random( 360 );
		PACKET_GUARD_VOID( kPacket.Write( iAngle ) );

		float fSpeed = eRandom.Random( m_fFloatSpeedMax - m_fFloatSpeedMin ) + m_fFloatSpeedMin;
		PACKET_GUARD_VOID( kPacket.Write( fSpeed ) );

		float fPower = eRandom.Random( m_fFloatPowerMax - m_fFloatPowerMin) + m_fFloatPowerMin;
		PACKET_GUARD_VOID( kPacket.Write( fPower ) );

		PACKET_GUARD_VOID( kPacket.Write( dwActivateTime ) );
		dwActivateTime += m_dwStarActivateTime;
	}
	m_pCreator->RoomSendPacketTcp( kPacket );
	LOOP_GUARD_CLEAR();

	m_iCurStarRegenCnt++;
	m_iCurStarRegenPos++;
	if( m_iCurStarRegenPos >= m_iMaxStarRegenPos )
	{
		m_iCurStarRegenPos = 0;
		io_random_shuffle( m_vShuffleStarRegenPos.begin(), m_vShuffleStarRegenPos.end() );
	}

	DestoryModeItemByStar();
}

void ShuffleBonusMode::ProcessGenerateBuffItem()
{
	DWORD dwTime = TIMEGETTIME();
	if( m_dwCurrBuffItemCreateTime == 0 || dwTime < m_dwCurrBuffItemCreateTime )
		return;

	if( m_vBuffItemCreatePos.empty() )
		return;

	BuffModeItem* pItem = BuffModeItem::ToBuffModeItem( CreateModeItem( MIT_BUFF ) );
	if( !pItem )
		return;
		
	io_random_shuffle( m_vBuffItemCreatePos.begin(), m_vBuffItemCreatePos.end() );

	pItem->m_fXPos = m_vBuffItemCreatePos[0].m_fXPos;
	pItem->m_fZPos = m_vBuffItemCreatePos[0].m_fZPos;

	m_dwCurrBuffItemCreateTime = 0;

	SP2Packet kPacket( STPK_CREATE_MODE_ITEM );
	PACKET_GUARD_VOID( kPacket.Write( static_cast<int>( MIT_BUFF ) ) );	
	PACKET_GUARD_VOID( kPacket.Write( m_dwBuffItemWaitTime ) );
	PACKET_GUARD_VOID( kPacket.Write( 1 ) );
	PACKET_GUARD_VOID( kPacket.Write( pItem->m_dwModeItemIdx ) );
	PACKET_GUARD_VOID( kPacket.Write( pItem->m_fXPos ) );
	PACKET_GUARD_VOID( kPacket.Write( pItem->m_fZPos ) );
	SendRoomAllUser( kPacket );
}

void ShuffleBonusMode::ProcessRegenBuffItem()
{
	if( m_vBuffItemRegenInfo.empty() )
		return;

	if( m_vShuffleStarRegenPos.empty() )
		return;

	DWORD dwTime = TIMEGETTIME();
	
	std::vector<BuffModeItem*> vSendBuffer;
	vBuffItemRegenInfo::iterator iter = m_vBuffItemRegenInfo.begin();
	for( ; iter != m_vBuffItemRegenInfo.end(); )
	{
		const BuffItemRegenInfo& rkInfo = *iter;
		if( dwTime < rkInfo.m_dwRegenTime )
		{
			++iter;
			continue;
		}

		BuffModeItem* pItem = BuffModeItem::ToBuffModeItem( CreateModeItem( MIT_BUFF ) );
		if( !pItem )
		{
			++iter;
			continue;
		}
		
		//기획자의 요청에 의해서 랜덤리젠 형태로 변경
		//pItem->m_fXPos = rkInfo.m_fXPos;
		//pItem->m_fZPos = rkInfo.m_fZPos;
		io_random_shuffle( m_vBuffItemCreatePos.begin(), m_vBuffItemCreatePos.end() );

		pItem->m_fXPos = m_vBuffItemCreatePos[0].m_fXPos;
		pItem->m_fZPos = m_vBuffItemCreatePos[0].m_fZPos;

		vSendBuffer.push_back( pItem );
		iter = m_vBuffItemRegenInfo.erase( iter );
	}

	int iSize = static_cast<int>( vSendBuffer.size() );

	SP2Packet kPacket( STPK_CREATE_MODE_ITEM );
	PACKET_GUARD_VOID( kPacket.Write( static_cast<int>( MIT_BUFF ) ) );
	PACKET_GUARD_VOID( kPacket.Write( m_dwBuffItemWaitTime ) );
	PACKET_GUARD_VOID( kPacket.Write( iSize ) );
	for( int i = 0; i < iSize; ++i )
	{
		BuffModeItem* pItem = vSendBuffer[i];
		PACKET_GUARD_VOID( kPacket.Write( pItem->m_dwModeItemIdx ) );
		PACKET_GUARD_VOID( kPacket.Write( pItem->m_fXPos ) );
		PACKET_GUARD_VOID( kPacket.Write( pItem->m_fZPos ) );
	}
	SendRoomAllUser( kPacket );
}

void ShuffleBonusMode::OnGetModeItem( SP2Packet &rkPacket )
{
	if( m_ModeState != MS_PLAY )
		return;

	DWORD dwUserIndex     = 0;
	DWORD dwModeItemIndex = 0;
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwModeItemIndex ) );
	
	User *pUser = g_UserNodeManager.GetUserNode( dwUserIndex );
	if( !pUser )
		return;
	
	const ModeItem* pItem = FindModeItem( dwModeItemIndex );
	if( pItem )
	{
		SP2Packet kPacket( STPK_GET_MODE_ITEM );
		PACKET_GUARD_VOID( kPacket.Write( dwUserIndex ) );
		PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );
		PACKET_GUARD_VOID( kPacket.Write( pItem->GetType() ) );
		PACKET_GUARD_VOID( kPacket.Write( dwModeItemIndex ) );

		switch( pItem->GetType() )
		{
		case MIT_SHUFFLE_STAR:
			{			
				ShuffleBonusRecord *pRecord = FindShuffleBonusRecord( pUser );
				if( pRecord )
				{
					pRecord->m_iStarCount++;
					PACKET_GUARD_VOID( kPacket.Write( pRecord->m_iStarCount ) );
				}
			}
			break;
		case MIT_BUFF:
			{
				OnGetModeItemByBuffItem( pItem, kPacket );
			}
			break;
		}

		SendRoomAllUser( kPacket );
		DeleteModeItem( dwModeItemIndex );
	}
}

void ShuffleBonusMode::OnGetModeItemByBuffItem( const ModeItem* pItem, SP2Packet &rkPacket )
{
	DWORD dwCallBuff = 0;
	const BuffModeItem* pBuffItem = BuffModeItem::ToBuffModeItemConst( pItem );
	if( pBuffItem )
	{
		BuffItemRegenInfo Info;
		Info.m_dwRegenTime = TIMEGETTIME() + m_dwBuffItemRegenTime;
		Info.m_fXPos  = pBuffItem->m_fXPos;
		Info.m_fZPos  = pBuffItem->m_fZPos;		
		m_vBuffItemRegenInfo.push_back( Info );

		IORandom Rand;
		Rand.Randomize();
		Rand.SetRandomSeed( timeGetTime() );
		dwCallBuff = Rand.Random( 0, m_dwBuffRandMaxRange );
		Info.m_dwRand = dwCallBuff;
		
	}

	PACKET_GUARD_VOID( rkPacket.Write( dwCallBuff ) );	
}

void ShuffleBonusMode::OnUserShuffleRoomUserBlow( SP2Packet &rkPacket )
{
	DWORD dwUserIndex = 0;
	PACKET_GUARD_VOID( rkPacket.Read( dwUserIndex ) );

	int iBlowType = 0;
	PACKET_GUARD_VOID( rkPacket.Read( iBlowType ) );

	float fXPos = 0.0f;
	float fYPos = 0.0f;
	float fZPos = 0.0f;
	
	PACKET_GUARD_VOID( rkPacket.Read( fXPos ) );
	PACKET_GUARD_VOID( rkPacket.Read( fYPos ) );
	PACKET_GUARD_VOID( rkPacket.Read( fZPos ) );
	
	User *pUser = g_UserNodeManager.GetUserNode( dwUserIndex );
	if( !pUser )
		return;

	ShuffleBonusRecord *pRecord = FindShuffleBonusRecord( pUser );
	if( !pRecord )
		return;

	if( pRecord->m_iStarCount <= 0 )
		return;

	int iDropCount = GetBlowDropStarCount( iBlowType );		
	if( iDropCount > pRecord->m_iStarCount )
	{		
		iDropCount = pRecord->m_iStarCount;		
	}

	ModeItemVec SendItem;
	for( int i = 0; i < iDropCount; ++i )
	{		
		ModeItem* pItem = CreateModeItem( MIT_SHUFFLE_STAR );
		if( !pItem )
			continue;

		SendItem.push_back( pItem );
	}

	IORandom eRandom;
	eRandom.Randomize();

	int iSize = (int)SendItem.size();
	pRecord->m_iStarCount = pRecord->m_iStarCount - iSize;

	SP2Packet kPacket( STPK_SHUFFLEROOM_USER_BLOW );
	PACKET_GUARD_VOID( kPacket.Write( pUser->GetUserIndex() ) );
	PACKET_GUARD_VOID( kPacket.Write( pUser->GetPublicID() ) );	
	PACKET_GUARD_VOID( kPacket.Write( iBlowType ) );
	PACKET_GUARD_VOID( kPacket.Write( fXPos ) );
	PACKET_GUARD_VOID( kPacket.Write( fYPos ) );
	PACKET_GUARD_VOID( kPacket.Write( fZPos ) );	
	PACKET_GUARD_VOID( kPacket.Write( iSize ) );
	for( int i = 0; i < iSize; ++i )
	{
		ModeItem* pItem = SendItem[i];

		PACKET_GUARD_VOID( kPacket.Write( pItem->m_dwModeItemIdx ) );

		int iAngle = eRandom.Random( 360 );
		PACKET_GUARD_VOID( kPacket.Write( iAngle ) );

		float fSpeed = eRandom.Random( m_fFloatSpeedMax - m_fFloatSpeedMin ) + m_fFloatSpeedMin;
		PACKET_GUARD_VOID( kPacket.Write( fSpeed ) );

		float fPower = eRandom.Random( m_fFloatPowerMax - m_fFloatPowerMin) + m_fFloatPowerMin;
		PACKET_GUARD_VOID( kPacket.Write( fPower ) );
	}

	m_pCreator->RoomSendPacketTcp( kPacket );
}

void ShuffleBonusMode::OnDropZoneDrop( SP2Packet &rkPacket )
{
	int iDropType = 0;
	PACKET_GUARD_VOID( rkPacket.Read( iDropType ) );
	
	switch( iDropType )
	{
	case SHUFFLEROOM_DROP_USER:
		{
			OnDropZoneDropByUser( rkPacket );
		}
		break;
	case SHUFFLEROOM_DROP_STAR:
		{
			OnDropZoneDropByStar( rkPacket );
		}
		break;
	default:
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - invaild drop type : %d", __FUNCTION__, iDropType );
		}
		break;
	}	
}

void ShuffleBonusMode::OnDropZoneDropByUser( SP2Packet &rkPacket )
{
	ioHashString szName;
	PACKET_GUARD_VOID( rkPacket.Read( szName ) );

	ShuffleBonusRecord *pRecord = FindShuffleBonusRecord( szName );
	if( pRecord && pRecord->pUser )
	{		
		int iDecreaseStar = (int)max( 0.0f, (float)pRecord->m_iStarCount * m_fDropStarDecreaseRate );		
		if( iDecreaseStar > pRecord->m_iStarCount )
		{		
			iDecreaseStar = pRecord->m_iStarCount;
		}

		pRecord->m_iStarCount = pRecord->m_iStarCount - iDecreaseStar;
		m_iDropZoneUserStar += iDecreaseStar;
		
		SP2Packet kPacket( STPK_SHUFFLEROOM_DROPZONE );
		PACKET_GUARD_VOID( kPacket.Write( SHUFFLEROOM_DROP_USER ) );
		PACKET_GUARD_VOID( kPacket.Write( pRecord->pUser->GetPublicID() ) );
		PACKET_GUARD_VOID( kPacket.Write( pRecord->m_iStarCount ) );
		SendRoomAllUser( kPacket );
	}
}

void ShuffleBonusMode::OnDropZoneDropByStar( SP2Packet &rkPacket )
{
	int iCount = 0;
	PACKET_GUARD_VOID( rkPacket.Read( iCount ) );

	DWORDVec dwDropVec;
	for( int i = 0; i < iCount; ++i )
	{
		DWORD dwDropItemIdx = 0;
		PACKET_GUARD_VOID( rkPacket.Read( dwDropItemIdx ) );

		if( dwDropItemIdx == 0 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - item index == 0", __FUNCTION__ );
			continue;
		}
		
		ModeItem* pModeItem = FindModeItem( dwDropItemIdx );
		if( !pModeItem )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - pModeItem == NULL : %d", __FUNCTION__, dwDropItemIdx );
			continue;
		}

		const ModeItem* pDropItem = FindModeItemByStarDrop( dwDropItemIdx );
		if( pDropItem )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - exist mode item : %d", __FUNCTION__, dwDropItemIdx );
			continue;
		}

		m_vDropZoneStar.push_back( pModeItem );
		DeleteModeItem( dwDropItemIdx );

		dwDropVec.push_back( dwDropItemIdx );
	}

	if( dwDropVec.empty() )
		return;

	SP2Packet kPacket( STPK_SHUFFLEROOM_DROPZONE );
	PACKET_GUARD_VOID( kPacket.Write( SHUFFLEROOM_DROP_STAR ) );
	PACKET_GUARD_VOID( kPacket.Write( (int)dwDropVec.size() ) );
	for( int i = 0; i <(int)dwDropVec.size(); ++i )
	{
		PACKET_GUARD_VOID( kPacket.Write( dwDropVec[i] ) );
	}
	m_pCreator->RoomSendPacketTcp( kPacket );
}

void ShuffleBonusMode::CalcStarCorrection()
{
	if( !m_pCreator )
		return;
	
	if( m_vRecordList.empty() )
		return;
	
	ShuffleRoomParent *pShuffleRoom;
	if( m_vRecordList[0].pUser )
		pShuffleRoom = m_vRecordList[0].pUser->GetMyShuffleRoom();

	if( !pShuffleRoom )
		return;

	DWORD dwCheckTime = TIMEGETTIME() - pShuffleRoom->GetCreateTime();

	int iTimeValue = g_ShuffleRoomManager.GetTimeCorrectionValue( dwCheckTime );
	int iUserValue = g_ShuffleRoomManager.GetUserCorrectionValue( m_vRecordList.size() );
	int iCorrectionPt = iTimeValue + iUserValue;

	int iStarCntByTime    = g_ShuffleRoomManager.GetTimeCorrectionStarCnt( dwCheckTime );
	int iStarCntByUserCnt = g_ShuffleRoomManager.GetUserCorrectionStarCnt( m_vRecordList.size() );

	m_iStarCount = iStarCntByTime + iStarCntByUserCnt;

	m_iMaxStarRegenCnt = g_ShuffleRoomManager.GetStarPhaseByCorrection( iCorrectionPt );
	m_dwStarRegenTime = m_dwRoundDuration / m_iMaxStarRegenCnt;	
}


void ShuffleBonusMode::FinalRoundProcessByShuffle()
{
	ShuffleBonusPtrRecordList vList;
	int iRecordCnt = GetRecordCnt();
	for( int i = 0; i < iRecordCnt; ++i )
	{
		ShuffleBonusRecord *pRecord = FindShuffleBonusRecord( i );
		if( !pRecord )
			continue;

		if( pRecord->m_iStarCount <= 0 )
			continue;

		User *pUser = pRecord->pUser;
		if( !pUser )
			continue;
				
		ShuffleRoomNode *pNode = dynamic_cast<ShuffleRoomNode*>( pUser->GetMyShuffleRoom() );
		if( pNode )
		{
			pNode->CheckShuffleReward( pUser, pRecord->m_iStarCount, pRecord->m_iStarCountByCalcBonus, pRecord->m_fBonusPercent );
			vList.push_back( pRecord );
		}
	}

	SP2Packet kPacket( STPK_SHUFFLE_REWARD_RESULT );	
	PACKET_GUARD_VOID( kPacket.Write( (int)vList.size() ) );
	for( int i = 0; i < (int)vList.size(); ++i )
	{
		ShuffleBonusRecord* pRecord = vList[i];
		PACKET_GUARD_VOID( kPacket.Write( pRecord->pUser->GetPublicID() ) );
		PACKET_GUARD_VOID( kPacket.Write( pRecord->m_iStarCount ) );
		PACKET_GUARD_VOID( kPacket.Write( pRecord->m_iStarCountByCalcBonus ) );
		PACKET_GUARD_VOID( kPacket.Write( pRecord->m_fBonusPercent ) );
	}
	SendRoomAllUser( kPacket );
}

int ShuffleBonusMode::GetBlowDropStarCount( int iBlowType )
{
	WoundedDropInfoVec::iterator iter = m_WoundedDropInfoVec.begin();
	for( ; iter != m_WoundedDropInfoVec.end(); ++iter )
	{
		const WoundedDropInfo& rkInfo = *iter;
		if( rkInfo.m_BlowType == iBlowType )
			return rkInfo.m_iDropCount;
	}

	return 0;
}

void ShuffleBonusMode::ProcessGenerateNpc()
{
	DWORD dwCurrTime = TIMEGETTIME();

	int iCount = 0;	
	MonsterRecordList vSendList;
	for(int i = 0; i < (int)m_NpcInfoVec.size(); ++i )
	{		
		NpcInfo& rkInfo = m_NpcInfoVec[i];
		if( rkInfo.m_bSpawn )
			continue;
		
		MonsterRecordList vMonsterGet;
		if( !g_MonsterMapLoadMgr.GetRandMonster( rkInfo.m_iRandTable, vMonsterGet, iCount, 1, false, false ) )
			continue;

		if( vMonsterGet.empty() )
			continue;

		int iIndex = vMonsterGet.size() - 1;
		MonsterRecord& Record = vMonsterGet[iIndex];

		if( dwCurrTime < m_dwModeStartTime + Record.dwStartTime )
			continue;
		
		Record.eTeam         = TEAM_PRIVATE_120;
		Record.nGrowthLvl    = 100;
		Record.dwAliveTime   = dwCurrTime + rkInfo.m_dwAliveTime;

		rkInfo.m_bSpawn      = true;
		rkInfo.m_szNpcName   = Record.szName;
		Record.dwStartTime   = 0;

		vSendList.push_back( Record );
	}

	if( vSendList.empty() )
		return;	

	IORandom eRandom;
	eRandom.Randomize();
	int iRandArray = eRandom.Random( 0, (int)m_vShuffleStarRegenPos.size() );

	SP2Packet kPacket( STPK_SPAWN_MONSTER );	
	PACKET_GUARD_VOID( kPacket.Write( (int)vSendList.size() ) );

	for(int i = 0; i < (int)vSendList.size(); ++i )
	{
		MonsterRecord rkMonster = vSendList[i];
		m_MonsterRecordVec.push_back( rkMonster );
		rkMonster.eState         = RS_PLAY;
		rkMonster.szSyncUser     = SearchMonsterSyncUser();
		rkMonster.fStartXPos	 = m_vShuffleStarRegenPos[iRandArray].m_fXPos;
		rkMonster.fStartZPos	 = m_vShuffleStarRegenPos[iRandArray].m_fZPos;


#ifdef ANTIHACK
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.dwNPCIndex ) );
#endif
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.dwCode ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.szName ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.szSyncUser ) );
		PACKET_GUARD_VOID( kPacket.Write( (int)rkMonster.eTeam ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.nGrowthLvl ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.bGroupBoss ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.dwStartTime ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.fStartXPos ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.fStartZPos ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.dwAliveTime ) );
	}

	SendRoomAllUser(kPacket);
}

const ioHashString &ShuffleBonusMode::SearchMonsterSyncUser()
{
	static ioHashString szError = "동기화유저없음";
	if( m_vRecordList.empty() ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s(%d) : None User Record", __FUNCTION__, m_pCreator->GetRoomIndex() );
		return szError;
	}

	ShuffleBonusRecord *pReturnRecord = NULL;
	ShuffleBonusRecordList::iterator iter = m_vRecordList.begin();
	for(;iter != m_vRecordList.end();iter++)
	{
		ShuffleBonusRecord &rkRecord = *iter;
		if( rkRecord.pUser == NULL ) 
			continue;

		if( !pReturnRecord )
		{
			pReturnRecord = &rkRecord;
			continue;
		}

		if( rkRecord.eState != RS_PLAY )
			continue;

		int iPrevPoint = pReturnRecord->pUser->GetPingStep() + pReturnRecord->m_iMonsterSyncCount;
		int iNextPoint = rkRecord.pUser->GetPingStep() + rkRecord.m_iMonsterSyncCount;
		if( iPrevPoint > iNextPoint )
			pReturnRecord = &rkRecord;
	}

	if( pReturnRecord == NULL || pReturnRecord->pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s(%d) : None Return Record : %d", __FUNCTION__, m_pCreator->GetRoomIndex(), m_vRecordList.size() );
		return szError;
	}

	// 몬스터 한마리 동기화 추가~
	pReturnRecord->m_iMonsterSyncCount++;
	return pReturnRecord->pUser->GetPublicID();
}

void ShuffleBonusMode::PlayMonsterSync( ShuffleBonusRecord *pSendRecord )
{
	if( m_bRoundSetEnd ) 
		return;

	// 플레이중 입장한 유저들에게만 동기화 시킨다.
	if( GetState() != MS_PLAY )
		return;

	if( pSendRecord == NULL || pSendRecord->pUser == NULL )
		return;


	if( m_MonsterRecordVec.empty() )
		return;

	SP2Packet kPacket( STPK_MONSTER_INFO_SYNC );

	int iSyncSize = m_MonsterRecordVec.size();		
	PACKET_GUARD_VOID( kPacket.Write( iSyncSize ) );
	for(int i = 0; i < iSyncSize; i++ )
	{
		MonsterRecord &rkMonster = m_MonsterRecordVec[i];

#ifdef ANTIHACK
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.dwNPCIndex ) );
#endif
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.dwCode ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.szName ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.szSyncUser ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.eTeam ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.nGrowthLvl ) );	
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.bGroupBoss ) );			
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.dwStartTime ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.fStartXPos ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.fStartZPos ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.dwAliveTime ) );
	}

	pSendRecord->pUser->SendMessage( kPacket ); 
}

void ShuffleBonusMode::RemoveRecordChangeMonsterSync( const ioHashString &rkRemoveName )
{
	if( m_bRoundSetEnd )
		return;

	if( m_vRecordList.empty() )
		return;

	if( rkRemoveName.IsEmpty() )
		return;
	

	MonsterRecordList vSyncRecord;

	for( int i = 0; i < (int)m_MonsterRecordVec.size(); ++i )
	{
		MonsterRecord &rkMonster = m_MonsterRecordVec[i];
		if( rkMonster.eState != RS_PLAY )
			continue;

		if( rkMonster.szSyncUser != rkRemoveName )
			continue;

		rkMonster.szSyncUser = SearchMonsterSyncUser();
		vSyncRecord.push_back( rkMonster );
	}

	if( vSyncRecord.empty() )
		return;

	int iSyncSize = vSyncRecord.size();
	SP2Packet kPacket( STPK_MONSTER_SYNC_CHANGE );	
	PACKET_GUARD_VOID( kPacket.Write( iSyncSize ) );
	for(int i = 0; i < iSyncSize; ++i )
	{
		MonsterRecord &rkMonster = m_MonsterRecordVec[i];
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.szName ) );
		PACKET_GUARD_VOID( kPacket.Write( rkMonster.szSyncUser ) );
		
	}
	SendRoomAllUser( kPacket );
	vSyncRecord.clear();
}

void ShuffleBonusMode::OnWeaponDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
	if( m_MonsterRecordVec.empty() )
		return;

	MonsterRecord *pDieMonster = &m_MonsterRecordVec[0];

	if( pDieMonster->eState != RS_PLAY )
		return;

	// 몬스터가 죽은 위치
	float fDiePosX, fDiePosZ;	
	PACKET_GUARD_VOID( rkPacket.Read( fDiePosX ) );
	PACKET_GUARD_VOID( rkPacket.Read( fDiePosZ ) );

	// Killer 유저 정보
	int iLastAttackerTeam;
	ioHashString szLastAttackerName;
	ioHashString szLastAttackerSkillName;
	DWORD dwLastAttackerWeaponItemCode;
	PACKET_GUARD_VOID( rkPacket.Read( szLastAttackerName ) );
	PACKET_GUARD_VOID( rkPacket.Read( szLastAttackerSkillName ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwLastAttackerWeaponItemCode ) );
	PACKET_GUARD_VOID( rkPacket.Read( iLastAttackerTeam ) );
	
	// 몬스터 죽음 처리
	{
		pDieMonster->eState = RS_DIE;
		pDieMonster->dwCurDieTime = TIMEGETTIME();

		ShuffleBonusRecord *pSyncUserRecord = FindShuffleBonusRecord( pDieMonster->szSyncUser );

		if( pSyncUserRecord )
			pSyncUserRecord->m_iMonsterSyncCount = max( 0, pSyncUserRecord->m_iMonsterSyncCount - 1 );
	}

	// 데미지 리스트 처리
	int iDamageCnt;
	ioHashString szBestAttackerName;	
	PACKET_GUARD_VOID( rkPacket.Read( iDamageCnt ) );

	int iTotalDamage = 0;
	int iLastDamage  = 0;
	int iBestDamage  = 0;

	DamageTableList vDamageList;
	if( iDamageCnt > 0 )
	{
		vDamageList.reserve( iDamageCnt );

		for( int i=0; i < iDamageCnt; ++i )
		{
			DamageTable kDamageTable;
			PACKET_GUARD_VOID( rkPacket.Read( kDamageTable.szName ) );
			PACKET_GUARD_VOID( rkPacket.Read( kDamageTable.iDamage ) );

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

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = (float)iLastDamage / iTotalDamage;
		fBestRate = (float)iBestDamage / iTotalDamage;
	}

	for(int i = 0; i < (int)m_NpcInfoVec.size(); ++i )
	{
		NpcInfo& rkInfo = m_NpcInfoVec[i];
		if( rkInfo.m_szNpcName == rkDieName )
		{
			rkInfo.m_bSpawn = false;
			rkInfo.m_szNpcName.Clear();
		}
	}

	SP2Packet kReturn( STPK_WEAPON_DIE );
	PACKET_GUARD_VOID( kReturn.Write( pDieMonster->szName ) );
	PACKET_GUARD_VOID( kReturn.Write( szLastAttackerName ) );
	PACKET_GUARD_VOID( kReturn.Write( szLastAttackerSkillName ) );
	PACKET_GUARD_VOID( kReturn.Write( dwLastAttackerWeaponItemCode ) );
	PACKET_GUARD_VOID( kReturn.Write( iLastAttackerTeam ) );
	PACKET_GUARD_VOID( kReturn.Write( szBestAttackerName ) );
	PACKET_GUARD_VOID( kReturn.Write( fLastRate ) );
	PACKET_GUARD_VOID( kReturn.Write( fBestRate ) );

	GetCharModeInfo( kReturn, pDieMonster->szName );
	GetCharModeInfo( kReturn, szLastAttackerName );
	SendRoomAllUser( kReturn );	
}

void ShuffleBonusMode::OnDropDieNpc( const ioHashString &rkDieName, SP2Packet &rkPacket )
{
	if( m_MonsterRecordVec.empty() )
		return;

	MonsterRecord *pDieMonster = &m_MonsterRecordVec[0];

	if( pDieMonster->eState != RS_PLAY )
		return;

	// 몬스터가 죽은 위치
	float fDiePosX, fDiePosZ;	
	PACKET_GUARD_VOID( rkPacket.Read( fDiePosX ) );
	PACKET_GUARD_VOID( rkPacket.Read( fDiePosZ ) );

	// Killer 유저 정보
	int iLastAttackerTeam;
	ioHashString szLastAttackerName;
	ioHashString szLastAttackerSkillName;
	DWORD dwLastAttackerWeaponItemCode;
	PACKET_GUARD_VOID( rkPacket.Read( szLastAttackerName ) );
	PACKET_GUARD_VOID( rkPacket.Read( szLastAttackerSkillName ) );
	PACKET_GUARD_VOID( rkPacket.Read( dwLastAttackerWeaponItemCode ) );
	PACKET_GUARD_VOID( rkPacket.Read( iLastAttackerTeam ) );

	// 몬스터 죽음 처리
	{
		pDieMonster->eState = RS_DIE;
		pDieMonster->dwCurDieTime = TIMEGETTIME();

		ShuffleBonusRecord *pSyncUserRecord = FindShuffleBonusRecord( pDieMonster->szSyncUser );

		if( pSyncUserRecord )
			pSyncUserRecord->m_iMonsterSyncCount = max( 0, pSyncUserRecord->m_iMonsterSyncCount - 1 );
	}

	// 데미지 리스트 처리
	int iDamageCnt;
	ioHashString szBestAttackerName;	
	PACKET_GUARD_VOID( rkPacket.Read( iDamageCnt ) );

	int iTotalDamage = 0;
	int iLastDamage  = 0;
	int iBestDamage  = 0;

	DamageTableList vDamageList;
	if( iDamageCnt > 0 )
	{
		vDamageList.reserve( iDamageCnt );

		for( int i=0; i < iDamageCnt; ++i )
		{
			DamageTable kDamageTable;
			PACKET_GUARD_VOID( rkPacket.Read( kDamageTable.szName ) );
			PACKET_GUARD_VOID( rkPacket.Read( kDamageTable.iDamage ) );

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

	float fLastRate = 0.0f;
	float fBestRate = 0.0f;

	if( iTotalDamage > 0 )
	{
		fLastRate = (float)iLastDamage / iTotalDamage;
		fBestRate = (float)iBestDamage / iTotalDamage;
	}

	for(int i = 0; i < (int)m_NpcInfoVec.size(); ++i )
	{
		NpcInfo& rkInfo = m_NpcInfoVec[i];
		if( rkInfo.m_szNpcName == rkDieName )
		{
			rkInfo.m_bSpawn = false;
			rkInfo.m_szNpcName.Clear();
		}
	}

	SP2Packet kReturn( STPK_DROP_DIE );
	PACKET_GUARD_VOID( kReturn.Write( pDieMonster->szName ) );
	PACKET_GUARD_VOID( kReturn.Write( szLastAttackerName ) );
	PACKET_GUARD_VOID( kReturn.Write( szLastAttackerSkillName ) );
	PACKET_GUARD_VOID( kReturn.Write( dwLastAttackerWeaponItemCode ) );
	PACKET_GUARD_VOID( kReturn.Write( iLastAttackerTeam ) );
	PACKET_GUARD_VOID( kReturn.Write( szBestAttackerName ) );
	PACKET_GUARD_VOID( kReturn.Write( fLastRate ) );
	PACKET_GUARD_VOID( kReturn.Write( fBestRate ) );

	GetCharModeInfo( kReturn, pDieMonster->szName );
	GetCharModeInfo( kReturn, szLastAttackerName );
	SendRoomAllUser( kReturn );
}
