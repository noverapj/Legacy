

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "DungeonAMode.h"

#include "Room.h"
#include "RoomNodeManager.h"
#include "ioEventUserNode.h"
#include "ioEtcItemManager.h"
#include "LadderTeamManager.h"
#include "ioPresentHelper.h"
#include "ioMonsterMapLoadMgr.h"

#include "../EtcHelpFunc.h"
#include "../DataBase/LogDBClient.h"
#include <strsafe.h>

DungeonAMode::DungeonAMode( Room *pCreator ) : MonsterSurvivalMode( pCreator )
{
	m_dwTreasureCardIndex = 0;
}

DungeonAMode::~DungeonAMode()
{
}

ModeType DungeonAMode::GetModeType() const
{
	return MT_DUNGEON_A;
}

const char* DungeonAMode::GetModeINIFileName() const
{
	return "config/dungeon_a_mode.ini";
}

void DungeonAMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "dungeon_a%d_object_group%d", iSubNum, iGroupNum );
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

void DungeonAMode::LoadMapINIValue()
{
	char szFileName[MAX_PATH] = "";
	sprintf_s( szFileName, "config/sp2_dungeon_a_mode%d_map.ini", GetModeSubNum() );

	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( szFileName );
	rkLoader.SetTitle( "info" );
	m_dwAbusePlayTime = rkLoader.LoadInt( "abuse_play_time", 0 );
	m_dwPresentPlayRandValue = rkLoader.LoadInt( "present_play_rand_value", 0 );
	m_dwSpecialAwardMaxPoint = rkLoader.LoadInt( "special_award_max_point", 0 );
	m_dwTreasureCardIndex    = rkLoader.LoadInt( "treasure_card_index", 0 );

	int iNpcNameCount = 0;
	int iHighTurn, iLowTurn;
	int iMaxHighTurn = rkLoader.LoadInt( "max_high_turn", 0 );
	for(iHighTurn = 0;iHighTurn < iMaxHighTurn;iHighTurn++)
	{
		char szKey[MAX_PATH];
		sprintf_s( szKey, "high_turn%d", iHighTurn + 1 );
		rkLoader.SetTitle( szKey );
		HighTurnData kHighTurn;
		int iMaxLowTurn = rkLoader.LoadInt( "max_low_turn", 0 );

		// 턴 시간
		DWORD dwPlusTurnTime   = rkLoader.LoadInt( "plus_turn_time", 0 );
		DWORD dwNormalTurnTime = rkLoader.LoadInt( "normal_turn_time", 0 );
		DWORD dwBossTurnTime   = rkLoader.LoadInt( "boss_turn_time", 0 );

		// 부활 시간
		DWORD dwTurnRevivalTime = rkLoader.LoadInt( "boss_turn_revival_time", 0 );
		DWORD dwTurnRevivalDownTime = rkLoader.LoadInt( "boss_turn_revival_down_time", 0 );

		// 보스턴 지정
		char szBossTurn[MAX_PATH] = "";
		rkLoader.LoadString( "boss_turn_list", "", szBossTurn, MAX_PATH );		
		IntVec vTurnList;
		for(iLowTurn = 0;iLowTurn < iMaxLowTurn;iLowTurn++)
		{
			sprintf_s( szKey, "turn%d_idx", iLowTurn + 1 );
			vTurnList.push_back( rkLoader.LoadInt( szKey, 0 ) );
		}
		
		//io_random_shuffle( vTurnList.begin(), vTurnList.end() );      //LowTurn들을 섞는다. = PvE에서는 섞지 않는다.

		iMaxLowTurn = min( (int)vTurnList.size(), rkLoader.LoadInt( "max_play_turn", iMaxLowTurn ) );
		for(iLowTurn = 0;iLowTurn < iMaxLowTurn;iLowTurn++)
		{
			TurnData kTurn;
			kTurn.m_bBossTurn = Help::IsStringCheck( szBossTurn, iLowTurn + 1, '.' );
			if( g_MonsterMapLoadMgr.GetTurnData( vTurnList[iLowTurn], kTurn, iHighTurn, iLowTurn, iNpcNameCount, false ) )
			{
				if( kTurn.m_bBossTurn )
				{
					kTurn.m_dwTurnTime = dwBossTurnTime + ( dwPlusTurnTime * iLowTurn );   // 보스 턴 시간
					kTurn.m_dwRevivalTime = dwTurnRevivalTime;          // 부활 시간 
					dwTurnRevivalTime -= dwTurnRevivalDownTime;
				}
				else
				{
					kTurn.m_dwTurnTime = dwNormalTurnTime + ( dwPlusTurnTime * iLowTurn );   // 일반 턴 시간
				}
				kHighTurn.m_TurnData.push_back( kTurn );
			}						
		}
		kHighTurn.m_dwCurrentTurnIdx = 0;
		m_HighTurnList.push_back( kHighTurn );
	}
	m_dwCurrentHighTurnIdx = 0;

	// 몬스터 시작 위치
	int i = 0;
	rkLoader.SetTitle( "start_pos" );
	m_RandomStartPos.clear();
	int iMaxRandomStartPos = rkLoader.LoadInt( "max_rand_pos", 0 );
	for(i = 0;i < iMaxRandomStartPos;i++)
	{
		RandomStartPos kRandomPos;
		char szKey[MAX_PATH];

		sprintf_s( szKey, "start%d_pos_x", i + 1 );		
		kRandomPos.m_fStartXPos = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_s( szKey, "start%d_pos_z", i + 1 );		
		kRandomPos.m_fStartZPos = rkLoader.LoadFloat( szKey, 0.0f );

		sprintf_s( szKey, "start%d_range_x", i + 1 );		
		kRandomPos.m_fStartXRange = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_s( szKey, "start%d_range_z", i + 1 );		
		kRandomPos.m_fStartZRange = rkLoader.LoadFloat( szKey, 0.0f );

		m_RandomStartPos.push_back( kRandomPos );
	}

	// Death NPC
	m_vDeathNPCList.clear();	
	rkLoader.SetTitle( "death_npc" );
	int iMaxDeathNPC = rkLoader.LoadInt( "max_death_npc", 0 );
	for(i = 0;i < iMaxDeathNPC;i++)
	{
		char szKey[MAX_PATH] = "";
		MonsterRecord kDeathNpc;
		// NPC 코드
		sprintf_s( szKey, "monster%d_code", i + 1 );
		kDeathNpc.dwCode = rkLoader.LoadInt( szKey, 0 );

		// NPC 출격 시간
		sprintf_s( szKey, "monster%d_start_time", i + 1 );
		kDeathNpc.dwStartTime = rkLoader.LoadInt( szKey, 0 );

		// NPC 시작 위치
		int iRandIndex = 0;
		kDeathNpc.fStartXPos = GetMonsterStartXPos( kDeathNpc.fStartXPos, iRandIndex );
		kDeathNpc.fStartZPos = GetMonsterStartZPos( kDeathNpc.fStartZPos, iRandIndex );

		// NPC 이름은 서버가 자동으로 만든다.
		char szNpcName[MAX_PATH] = "";
		sprintf_s( szNpcName, " -N%d- ", ++iNpcNameCount );        // 좌우에 공백을 넣어서 유저 닉네임과 중복되지 않도록 한다.
		kDeathNpc.szName = szNpcName;

		m_vDeathNPCList.push_back( kDeathNpc );
	}
}

void DungeonAMode::CheckTurnMonster()
{
	if( m_bRoundSetEnd ) return;

	if( !COMPARE( m_dwCurrentHighTurnIdx, 0, (int)m_HighTurnList.size() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "MonsterSurvivalMode::CheckTurnMonster(%d) : Index Error 1: %d - %d ", m_pCreator->GetRoomIndex(), m_dwCurrentHighTurnIdx, (int)m_HighTurnList.size() );
		return;
	}

	HighTurnData &rkHighTurn = m_HighTurnList[m_dwCurrentHighTurnIdx];
	if( !COMPARE( rkHighTurn.m_dwCurrentTurnIdx, 0, (int)rkHighTurn.m_TurnData.size() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "MonsterSurvivalMode::CheckTurnMonster(%d) : Index Error 2: %d - %d ", m_pCreator->GetRoomIndex(), rkHighTurn.m_dwCurrentTurnIdx, (int)rkHighTurn.m_TurnData.size() );
		return;
	}

	TurnData &rkTurn = rkHighTurn.m_TurnData[rkHighTurn.m_dwCurrentTurnIdx];
	int iLiveMonster = 0;
	int iMaxNPC = rkTurn.m_vMonsterList.size();
	for(int i = 0;i < iMaxNPC;i++)
	{
		MonsterRecord &rkMonster = rkTurn.m_vMonsterList[i];
		if( rkMonster.eState == RS_PLAY )
			iLiveMonster++;
	}

	if( iLiveMonster == 0 )
	{
		rkTurn.m_dwRevivalTime = 0;        // 부활 초기화.
		if( !rkTurn.m_bMonsterAllDie )
		{
			rkTurn.m_bMonsterAllDie = true;

			// 턴 포인트 누적
			ClearTurnAddPoint( rkTurn );

			// 다음 턴
			NextTurnCheck();
		
			// 턴 종료 서버 알림
			EndTurnToServerAlarm( rkTurn );
		}		
	}
}

void DungeonAMode::NextTurnCheck()
{
	if( m_bRoundSetEnd ) return;

	int iRecordCnt = m_vRecordList.size();
	if( iRecordCnt == 0 ) 
		return;

	// 다음LowTurn이 존재하면 다음턴 진행	
	if( !COMPARE( m_dwCurrentHighTurnIdx, 0, (int)m_HighTurnList.size() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DungeonAMode::NextTurnCheck(%d) : Index Error 1: %d - %d ", m_pCreator->GetRoomIndex(), m_dwCurrentHighTurnIdx, (int)m_HighTurnList.size() );
		return;
	}
	HighTurnData &rkHighTurn = m_HighTurnList[m_dwCurrentHighTurnIdx];
	if( !COMPARE( rkHighTurn.m_dwCurrentTurnIdx, 0, (int)rkHighTurn.m_TurnData.size() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DungeonAMode::NextTurnCheck(%d) : Index Error 2: %d - %d ", m_pCreator->GetRoomIndex(), rkHighTurn.m_dwCurrentTurnIdx, (int)rkHighTurn.m_TurnData.size() );
		return;
	}

	TurnData &rkTurn = rkHighTurn.m_TurnData[rkHighTurn.m_dwCurrentTurnIdx];
	if( COMPARE( rkHighTurn.m_dwCurrentTurnIdx + 1, 0, (int)rkHighTurn.m_TurnData.size() ) )
	{
		StartTurn( m_dwCurrentHighTurnIdx, rkHighTurn.m_dwCurrentTurnIdx + 1 );
	}
	else if( COMPARE( m_dwCurrentHighTurnIdx + 1, 0, (int)m_HighTurnList.size() ) )        // 다음 HighTurn이 존재하면 다음턴 진행
	{
		StartTurn( m_dwCurrentHighTurnIdx + 1, 0 );
	}
}

void DungeonAMode::ProcessResultWait()
{
	// 시상식 없음
	if( m_bRoundSetEnd )
	{
		if( m_bCheckContribute && m_bCheckAllRecvDamageList )
		{
			if( m_dwStateChangeTime + m_dwFinalResultWaitStateTime < TIMEGETTIME() )
			{
				FinalRoundProcess();
				SetModeState( MS_RESULT );
			}
		}
		else
		{
			if( m_dwStateChangeTime + 5000 < TIMEGETTIME() )
			{
				FinalRoundProcess();
				SetModeState( MS_RESULT );
			}
		}
	}
	else
	{
		SetModeState( MS_RESULT );
	}
}

void DungeonAMode::ProcessResult()
{
	// 시상식 없음
	DWORD dwResultDuration = m_dwResultStateTime;
	if( m_bRoundSetEnd )
	{
		dwResultDuration = m_dwFinalResultStateTime;
	}

	if( m_dwStateChangeTime + dwResultDuration > TIMEGETTIME() )
		return;

	if( m_bRoundSetEnd )
	{
		if( ExecuteReservedExit() )
			return;	// true가 리턴된 경우는 모든 유저가 나가버린 상태
	}

	if( !m_bRoundSetEnd )
	{
		m_iCurRound++;                 // 세트가 종료되지 않은 상태에서만 라운드를 증가시킨다.
		RestartMode();
	}
	else
	{
		m_bRequestDestroy = true;		

		// log
		int iRecordCnt = GetRecordCnt();
		for(int i = 0;i < iRecordCnt;i++ )       
		{
			ModeRecord *pRecord = FindModeRecord( i );
			SetModeEndDBLog( pRecord, iRecordCnt, LogDBClient::PRT_END_SET );
		}
		//
		SetModeEndDBLog( m_pCreator, LogDBClient::PRT_END_SET );
	}
}

void DungeonAMode::OnLastPlayRecordInfo( User *pUser, SP2Packet &rkPacket )
{
	if( m_bCheckContribute ) return;

	m_bCheckContribute = true;

	int i = 0;
	int iCharCnt = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iCharCnt) );
	MAX_GUARD(iCharCnt, 50);

	for(i = 0;i < iCharCnt;i++)
	{
		ioHashString szName;
		int iContribute = 0, iUniqueTotalKill = 0, iUniqueTotalDeath = 0, iVictories = 0;
		PACKET_GUARD_VOID( rkPacket.Read(szName) );
		PACKET_GUARD_VOID( rkPacket.Read(iContribute) );
		PACKET_GUARD_VOID( rkPacket.Read(iUniqueTotalKill) );
		PACKET_GUARD_VOID( rkPacket.Read(iUniqueTotalDeath) );
		PACKET_GUARD_VOID( rkPacket.Read(iVictories) );

		ModeRecord *pRecord = FindModeRecord( szName );
		if( pRecord )
		{
			pRecord->iContribute	   = iContribute;
			pRecord->iUniqueTotalKill  = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath = iUniqueTotalDeath;
			pRecord->iVictories		   = iVictories;
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "DungeonAMode::OnLastContributeInfo Recv: %s - %d - %d - %d - %d", szName.c_str(), iContribute, iUniqueTotalKill, iUniqueTotalDeath, iVictories );

			// error check
			if( iContribute == 0 )
			{
				if( iUniqueTotalKill != 0 || iUniqueTotalDeath != 0 )
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DungeonAMode::OnLastContributeInfo Error: %s - %d - %d - %d - %d", szName.c_str(), iContribute, iUniqueTotalKill, iUniqueTotalDeath, iVictories );
			}
		}
	}

	int iRecordCnt		= GetRecordCnt();
	int iMaxContribute	= 0;
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		iMaxContribute += pRecord->iContribute;
	}

	// 옵저버 유저 제외
	int iOb = 0;
	for(i = 0;i < iRecordCnt;i++)
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;
		if( pRecord->pUser->IsObserver() || pRecord->pUser->IsStealth() )
			iOb++;
	}

	if( iMaxContribute > 0 )
	{
		for(i = 0;i < iRecordCnt;i++)
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord ) continue;
			if( !pRecord->pUser ) continue;

			pRecord->fContributePer = (float)( iRecordCnt - iOb ) * ((float)pRecord->iContribute / iMaxContribute);
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "DungeonAMode::OnLastContributeInfo Per: %s - %.2f", pRecord->pUser->GetPublicID().c_str(), pRecord->fContributePer );
		}	
	}	

	int iTreasureClearCount = 0;
	UpdateUserRank();

	// MVP
	if( IsBlueWin( m_CurRoundWinTeam ) )
	{
		for(i = 0;i < iRecordCnt;i++)
		{
			MonsterSurvivalRecord *pRecord = static_cast< MonsterSurvivalRecord * >( FindModeRecord( i ) );
			if( !pRecord ) continue;
			if( !pRecord->pUser ) continue;
			
			if( pRecord->iCurRank == 1 )
			{
				//TODO 타워디펜스모드 때문에 막아둠   보상 주자!!
//				pRecord->iTreasureCardCount = g_MonsterMapLoadMgr.GetTreasureCardMVPCount( m_dwTreasureCardIndex );
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "DungeonAMode::OnLastContributeInfo MVP : %s : %d", pRecord->pUser->GetPublicID().c_str(), pRecord->iTreasureCardCount );
				break;
			}
		}	
		//TODO 이거도 막아둠
//		iTreasureClearCount = g_MonsterMapLoadMgr.GetTreasureCardClearCount( m_dwTreasureCardIndex );
	}

	// 전체 유저의 기여도와 킬데스레벨을 전체 유저에게 전송한다.
	SP2Packet kPacket( STPK_FINAL_RESULT_USER_INFO );
	PACKET_GUARD_VOID( kPacket.Write(iRecordCnt) );

	for(i = 0;i < iRecordCnt;i++)
	{
		MonsterSurvivalRecord *pRecord = static_cast< MonsterSurvivalRecord * >( FindModeRecord( i ) );
		if( !pRecord ) continue;
		if( !pRecord->pUser ) continue;

		int iVictories = pRecord->iVictories;
		if( pRecord->pUser->IsLadderTeam() )
			pRecord->pUser->GetMyVictories();

		pRecord->iTreasureCardCount += iTreasureClearCount;
		
		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->GetPublicID()) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iCurRank) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iContribute) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iUniqueTotalKill) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iUniqueTotalDeath) );
		PACKET_GUARD_VOID( kPacket.Write(iVictories) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->GetKillDeathLevel()) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->IsSafetyLevel()) );


		// 가장 긴 시간 플레이했던 용병의 정보(치장 포함)
		int iClassType = pRecord->GetHighPlayingClass();
		PACKET_GUARD_VOID( kPacket.Write(iClassType) );
		PACKET_GUARD_VOID( kPacket.Write(pRecord->pUser->GetCharIndex(pRecord->pUser->GetCharArrayByClass(iClassType))) );


		// 보물 상자 클릭 
		PACKET_GUARD_VOID( kPacket.Write(pRecord->iTreasureCardCount) );

		g_UserNodeManager.UpdateUserSync( pRecord->pUser );      //유저 정보 갱신

		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "DungeonAMode::OnLastContributeInfo : %s : %d", pRecord->pUser->GetPublicID().c_str(), pRecord->iTreasureCardCount );
	}
	m_pCreator->RoomSendPacketTcp( kPacket );
}