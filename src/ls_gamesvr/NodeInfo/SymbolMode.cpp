

#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "Room.h"
#include "SymbolMode.h"

#include "ModeHelp.h"
#include "../DataBase/LogDBClient.h"

SymbolMode::SymbolMode( Room *pCreator ) : Mode( pCreator )
{
	m_iBlueSymbolCnt = 0;
	m_iRedSymbolCnt  = 0;

	m_iMinVictoryActiveSymbolCnt = 3;

	// For Gauge
	m_dwTicCheckTime = 0;
	m_bNoLimiteSymbol = false;

	m_fRedGauge = 0.0f;
	m_fBlueGauge = 0.0f;
	m_fCurRecoverGauge = 0.0f;
	m_fDecreaseScoreTimeRate = 0.0f;

	m_AscendancyTeam = TEAM_NONE;
}

SymbolMode::~SymbolMode()
{
}

void SymbolMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vSymbolList.clear();
	m_vRecordList.clear();
}

void SymbolMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;
	wsprintf( szTitle, "symbol%d_object_group%d", iSubNum, iGroupNum );
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

void SymbolMode::AddNewRecord( User *pUser )
{
	SymbolRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	UpdateUserRank();
}

void SymbolMode::RemoveRecord( User *pUser, bool bRoomDestroy )
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

	// 모드 : 09.06.03
	//UpdateSymbolGauge();
	
	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	}
}

void SymbolMode::LoadINIValue()
{
	Mode::LoadINIValue();

	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	char szBuf[MAX_PATH];
	char szTitle[MAX_PATH], szTeam[MAX_PATH];

	// For Gauge
	rkLoader.SetTitle( "symbol_gauge" );

	m_dwRecoverTic = rkLoader.LoadInt( "symbol_gauge_tic", 0 );
	m_fMaxGauge = rkLoader.LoadFloat( "symbol_gauge_max", 0.0f );

	m_fDecreaseScoreTimeRate = rkLoader.LoadFloat( "symbol_gauge_decrease_rate", 0.0f );

	int iRecoverGaugeCnt = rkLoader.LoadInt( "symbol_gauge_recover_cnt", 0 );
	m_RecoverGaugeList.clear();

	for ( int i=0; i < iRecoverGaugeCnt; i++ )
	{
		RecoverGauge eGauge;
		wsprintf( szBuf, "symbol_gauge_recover_win%d", i+1 );
		eGauge.m_fWinGauge = rkLoader.LoadFloat( szBuf, 0.0f );
		
		wsprintf( szBuf, "symbol_gauge_recover_draw%d", i+1 );
		eGauge.m_fDrawGauge = rkLoader.LoadFloat( szBuf, 0.0f );
		
		wsprintf( szBuf, "symbol_gauge_recover_lose%d", i+1 );
		eGauge.m_fLoseGauge = rkLoader.LoadFloat( szBuf, 0.0f );

		m_RecoverGaugeList.push_back( eGauge );
	}
	//

	//
	rkLoader.SetTitle( "round" );
	m_fScoreGaugeMaxRate = rkLoader.LoadFloat( "score_gauge_max_rate", 1.0f );
	m_dwScoreGaugeConstValue = rkLoader.LoadInt( "score_gauge_const_value", 210000 );
	m_bNoLimiteSymbol = rkLoader.LoadBool( "no_limite_symbol", false );

	m_fScoreGaugeDethTimeRate = rkLoader.LoadFloat( "score_gauge_deth_time_rate", 1.0f );
	//

	rkLoader.SetTitle( "symbol_common" );

	m_iMinVictoryActiveSymbolCnt = rkLoader.LoadInt( "min_victory_symbol_cnt", 3 );

	int iMaxLimitsLevel = rkLoader.LoadInt( "max_limits_level", 1 );
	m_SymbolLimitsCharCntList.clear();
	m_SymbolLimitsCharCntList.reserve( iMaxLimitsLevel );

	for(int i=0 ; i<iMaxLimitsLevel ; i++ )
	{
		wsprintf( szBuf, "limits_char_level%d", i+1 );

		int iCharCnt = rkLoader.LoadInt( szBuf, 0 );
		m_SymbolLimitsCharCntList.push_back( iCharCnt );
	}

	int iSymbolCnt = rkLoader.LoadInt( "symbol_cnt", 0 );
	m_vSymbolList.clear();
	m_vSymbolList.reserve( iSymbolCnt );

	for(int i=0 ; i<iSymbolCnt ; i++ )
	{
		wsprintf( szTitle, "symbol%d", i+1 );
		rkLoader.LoadString( szTitle, "team", "NONE", szTeam, MAX_PATH );

		SymbolStruct kSymbol;
		kSymbol.m_OrgTeam = ConvertStringToTeamType( szTeam );
		m_vSymbolList.push_back( kSymbol );
	}

	RestoreSymbolList();
	SetReSymbolAcitvity();

	m_dwCurRoundDuration = m_dwRoundDuration;
	m_vRoundHistory.clear();
}

void SymbolMode::RestoreSymbolList()
{
	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		m_vSymbolList[i].RestoreTeam();
	}
}

void SymbolMode::ProcessPlay()
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

// 모드 : 09.06.03
void SymbolMode::CheckRoundEnd( bool bProcessCall )
{
	DWORD dwGapTime = TIMEGETTIME() - m_dwStateChangeTime;

	int iTotalCnt = GetActiveSymbolCnt();
	int iRedCnt = GetTeamSymbolCnt( TEAM_RED );
	int iBlueCnt = GetTeamSymbolCnt( TEAM_BLUE );
	
	WinTeamType eWinTeam = WTT_DRAW;
	if( iTotalCnt == iRedCnt )
		eWinTeam = WTT_RED_TEAM;
	else if( iTotalCnt == iBlueCnt )
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

	if( eWinTeam == WTT_RED_TEAM )
		SetScore( TEAM_RED, m_bZeroHP );
	else if( eWinTeam == WTT_BLUE_TEAM )
		SetScore( TEAM_BLUE, m_bZeroHP );

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

void SymbolMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd ) return;

	if( GetTeamUserCnt( TEAM_BLUE ) == 0 ||	GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		CheckRoundEnd( false );
	}
}

void SymbolMode::SetRoundEndInfo( WinTeamType eWinTeam )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SymbolMode::SetRoundEndInfo() m_iCurRound != m_vRoundHistory.size()" );
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

void SymbolMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SymbolMode::OnEventSceneEnd - %s Not Exist Record",
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
		// 모드 : 09.06.03
		//UpdateSymbolGauge();

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
		// 모드 : 09.06.03
		//UpdateSymbolGauge();

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

		// 모드 : 09.06.03
		//UpdateSymbolGauge();

		SP2Packet kPacket( STPK_ROUND_JOIN );
		kPacket << m_fCurRecoverGauge;
		kPacket << pRecord->pUser->GetPublicID();
		kPacket << iModeState;
		kPacket << dwPastTime;
		kPacket << GetSelectCharTime();
		kPacket << m_dwCurRoundDuration;
		SendRoomAllUser( kPacket );
	}
}


void SymbolMode::RestartMode()
{
	Mode::RestartMode();

	// 결과중에 유저들이 전부 나가면 최종 결과 실행.
	if( GetTeamUserCnt( TEAM_BLUE ) == 0 || 
		GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		m_bRoundSetEnd = true;
		m_bCheckContribute = false;
		m_bCheckAwardChoose = false;
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
		SymbolRecord &rkRecord = m_vRecordList[i];
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
	
	m_iRedSymbolCnt = 0;
	m_iBlueSymbolCnt = 0;

	RestoreSymbolList();
	SetReSymbolAcitvity();

	// For Gauge
	m_dwTicCheckTime = 0;

	m_fRedGauge = 0.0f;
	m_fBlueGauge = 0.0f;
	m_fCurRecoverGauge = 0.0f;
	m_AscendancyTeam = TEAM_NONE;

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

ModeType SymbolMode::GetModeType() const
{
	return MT_SYMBOL;
}

void SymbolMode::GetModeInfo( SP2Packet &rkPacket )
{
	Mode::GetModeInfo( rkPacket );

	rkPacket << m_iBlueSymbolCnt;
	rkPacket << m_iRedSymbolCnt;
	
	rkPacket << m_AscendancyTeam;
	rkPacket << m_fBlueGauge;
	rkPacket << m_fRedGauge;
	rkPacket << m_fCurRecoverGauge;

	int iSymbolCnt = m_vSymbolList.size();
	rkPacket << iSymbolCnt;
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{
		rkPacket << m_vSymbolList[i].m_bActive;
		rkPacket << m_vSymbolList[i].m_Team;
		rkPacket << m_vSymbolList[i].m_iRevivalCnt;
	}

	GetModeHistory( rkPacket );
}

void SymbolMode::GetModeHistory( SP2Packet &rkPacket )
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


void SymbolMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &szName, bool bDieCheck )
{
	SymbolRecord *pRecord = FindSymbolRecord( szName );
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

int SymbolMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* SymbolMode::GetModeINIFileName() const
{
	return "config/symbolmode.ini";
}

TeamType SymbolMode::GetNextTeamType()
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

int SymbolMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
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

ModeRecord* SymbolMode::FindModeRecord( const ioHashString &rkName )
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

ModeRecord* SymbolMode::FindModeRecord( User *pUser )
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

ModeRecord* SymbolMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

SymbolRecord* SymbolMode::FindSymbolRecord( const ioHashString &rkName )
{
	return (SymbolRecord*)FindModeRecord( rkName );
}

SymbolRecord* SymbolMode::FindSymbolRecord( User *pUser )
{
	return (SymbolRecord*)FindModeRecord( pUser );
}

bool SymbolMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_SYMBOL_DIE:
		OnSymbolDie( pSend, rkPacket );
		return true;
	case CTPK_SYMBOL_DAMAMGED:
		OnSymbolDamaged( pSend, rkPacket );
		return true;
	}

	return false;
}

void SymbolMode::OnSymbolDie( User *pUser, SP2Packet &rkPacket )
{
	int iArrayIdx, iRevivalCnt;
	rkPacket >> iArrayIdx >> iRevivalCnt;

	float fRevivalHP;
	rkPacket >> fRevivalHP;

	int iSymbolCnt = m_vSymbolList.size();
	if( !COMPARE( iArrayIdx, 0, iSymbolCnt ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SymbolMode::OnSymbolDie - %s Kill Symbol Overflow(%d/%d)",
								pUser->GetPublicID().c_str(), iArrayIdx, iRevivalCnt );
		return;
	}

	SymbolStruct &rkSymbol = m_vSymbolList[iArrayIdx];
	if( rkSymbol.m_iRevivalCnt > iRevivalCnt )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SymbolMode::OnSymbolDie - %s Kill Symbol Not Equal RevivalCnt",
								pUser->GetPublicID().c_str() );
		return;
	}

	TeamType eKillTeam = pUser->GetTeam();
	TeamType ePreTeam = rkSymbol.m_Team;

	if( !IsEnableDie( iArrayIdx, eKillTeam ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SymbolMode::OnSymbolDie - %s Kill Symbol Disable Die Position",
								pUser->GetPublicID().c_str() );
		return;
	}

	rkSymbol.m_Team = eKillTeam;
	rkSymbol.m_iRevivalCnt++;

	// 모드 : 09.06.03
	//UpdateSymbolGauge();

	SP2Packet kReturn( STPK_SYMBOL_DIE );
	kReturn << pUser->GetPublicID();
	kReturn << iArrayIdx;
	kReturn << fRevivalHP;
	kReturn << rkSymbol.m_iRevivalCnt;
	kReturn << eKillTeam;
	kReturn << m_AscendancyTeam;
	kReturn << m_fCurRecoverGauge;
	kReturn << m_fBlueGauge;
	kReturn << m_fRedGauge;
	SendRoomAllUser( kReturn );

	// 모드 : 09.06.03
	CheckRoundEnd(true);
}

void SymbolMode::OnSymbolDamaged( User *pUser, SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	int iArrayIdx, iRevivalCnt;
	rkPacket >> szAttacker;
	rkPacket >> iArrayIdx >> iRevivalCnt;

	int iSymbolCnt = m_vSymbolList.size();
	if( !COMPARE( iArrayIdx, 0, iSymbolCnt ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SymbolMode::OnSymbolDamaged - %s Damaged Symbol Overflow(%d/%d)",
								pUser->GetPublicID().c_str(), iArrayIdx, iRevivalCnt );
		return;
	}

	SymbolStruct &rkSymbol = m_vSymbolList[iArrayIdx];
	if( rkSymbol.m_iRevivalCnt > iRevivalCnt )
		return;

	float fMaxHP;
	rkPacket >> fMaxHP;
	
	if( rkSymbol.m_fMaxHP == 0.0f )
	{
		rkSymbol.m_fCurHP = fMaxHP;
		rkSymbol.m_fMaxHP = fMaxHP;
	}
	else
	{
		rkSymbol.m_fCurHP *= fMaxHP / rkSymbol.m_fMaxHP;
		rkSymbol.m_fMaxHP = fMaxHP;
	}

	float fDamage;
	bool bAddDamage;
	rkPacket >> fDamage >> bAddDamage;

	if( bAddDamage )
	{
		rkSymbol.m_fCurHP += fDamage;
		rkSymbol.m_fCurHP = min( rkSymbol.m_fCurHP, rkSymbol.m_fMaxHP );
	}
	else
	{
		rkSymbol.m_fCurHP -= fDamage;
		rkSymbol.m_fCurHP = max( 0.0f, rkSymbol.m_fCurHP );
	}

	int iActionStop,iShakeCamera;
	bool bSymbolMax;
	rkPacket >> iActionStop >> iShakeCamera >> bSymbolMax;

	//Send
	SP2Packet kReturn( STPK_SYMBOL_DAMAMGED );
	kReturn << szAttacker;
	kReturn << iArrayIdx;
	kReturn << iRevivalCnt;
	kReturn << rkSymbol.m_fMaxHP;
	kReturn << fDamage;
	kReturn << bAddDamage;
	kReturn << iActionStop;
	kReturn << iShakeCamera;
	kReturn << bSymbolMax;
	SendRoomAllUser( kReturn, pUser );
}

void SymbolMode::SetReSymbolAcitvity()
{
	// init symbol act
	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{	
		SymbolStruct &rkSymbol = m_vSymbolList[i];
		rkSymbol.SetActive( true );
	}

	// send
	SP2Packet kPacket( STPK_SYMBOL_ACTIVITY );
	kPacket << iSymbolCnt;
	for(int i =0 ; i<iSymbolCnt ; i++ )
		kPacket << m_vSymbolList[i].m_bActive;

	SendRoomAllUser( kPacket );
}

int SymbolMode::GetActiveSymbolCnt()
{
	int iActiveCnt = 0;
	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{	
		SymbolStruct &rkSymbol = m_vSymbolList[i];
		if( rkSymbol.m_bActive )
			iActiveCnt++;
	}

	return iActiveCnt;
}

// 모드 : 09.06.03
int SymbolMode::GetTeamSymbolCnt( TeamType eTeam )
{
	int iTeamCnt = 0;
	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{	
		SymbolStruct &rkSymbol = m_vSymbolList[i];
		if( !rkSymbol.m_bActive ) continue;

		if( rkSymbol.m_Team == eTeam )
			iTeamCnt++;
	}

	return iTeamCnt;
}

void SymbolMode::UpdateSymbolGauge()
{
	int iRedCnt, iBlueCnt;
	iRedCnt = iBlueCnt = 0;

	int iSymbolCnt = m_vSymbolList.size();
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{	
		SymbolStruct &rkSymbol = m_vSymbolList[i];
		if( !rkSymbol.m_bActive ) continue;

		if( rkSymbol.m_Team == TEAM_BLUE )
			iBlueCnt++;
		else if( rkSymbol.m_Team == TEAM_RED )
			iRedCnt++;
	}

	if( iBlueCnt > iRedCnt )
		m_AscendancyTeam = TEAM_BLUE;
	else if( iBlueCnt < iRedCnt )
		m_AscendancyTeam = TEAM_RED;
	else
		m_AscendancyTeam = TEAM_NONE;

	int iGapCnt = iBlueCnt - iRedCnt;
	iGapCnt = abs( iGapCnt );

	if( m_AscendancyTeam == TEAM_NONE )
	{
		m_fCurRecoverGauge = 0.0f;
		m_dwTicCheckTime = 0;
	}
	else
	{
		// 상징물 수, 승/패에 따라 선택
		int iRecoverGaugeListCnt = m_RecoverGaugeList.size();
		if( COMPARE( iGapCnt, 1, iRecoverGaugeListCnt+1 ) )
		{
			if( m_AscendancyTeam == CheckCurWinTeam() )
				m_fCurRecoverGauge = m_RecoverGaugeList[iGapCnt-1].m_fWinGauge;
			else if( CheckCurWinTeam() == TEAM_NONE )
				m_fCurRecoverGauge = m_RecoverGaugeList[iGapCnt-1].m_fDrawGauge;
			else
				m_fCurRecoverGauge = m_RecoverGaugeList[iGapCnt-1].m_fLoseGauge;
		}
		else
			m_fCurRecoverGauge = 0.0f;
	}

	// 인원 차에 의한 조정
	float fRedTeamCnt = (float)GetCurTeamUserCnt( TEAM_RED );
	float fBlueTeamCnt = (float)GetCurTeamUserCnt( TEAM_BLUE );

	if( fRedTeamCnt > 0 && fBlueTeamCnt > 0 )
	{
		if( m_AscendancyTeam == TEAM_RED && fRedTeamCnt > fBlueTeamCnt )
			m_fCurRecoverGauge = m_fCurRecoverGauge * ( fBlueTeamCnt / fRedTeamCnt );
		else if( m_AscendancyTeam == TEAM_BLUE && fBlueTeamCnt > fRedTeamCnt )
			m_fCurRecoverGauge = m_fCurRecoverGauge * ( fRedTeamCnt / fBlueTeamCnt );
	}

	// 총인원에 따른 비율 조정
	CheckDecreaseGaugeByPeopleCnt();

	// 라운드 시간에따른 조정
	// 임시 수정
	/*
	DWORD dwGapTime = TIMEGETTIME() - m_dwModeStartTime;
	float fCurRate = 1.0f + ( (float)dwGapTime / m_dwScoreGaugeConstValue );
	fCurRate = min( fCurRate, m_fScoreGaugeMaxRate );
	m_fCurRecoverGauge *= fCurRate;
	*/

	if( IsZeroHP() )
	{
		m_fCurRecoverGauge *= m_fScoreGaugeDethTimeRate;
	}
	//
}

void SymbolMode::ProcessGauge()
{
	if( m_AscendancyTeam == TEAM_NONE )
	{
		m_dwTicCheckTime = 0;
		return;
	}

	DWORD dwCurTime = TIMEGETTIME();
	if( m_dwTicCheckTime == 0 )
		m_dwTicCheckTime = dwCurTime;

	DWORD dwGapTime = dwCurTime - m_dwTicCheckTime;
	
	LOOP_GUARD();
	while( dwGapTime > m_dwRecoverTic )
	{
		dwGapTime -= m_dwRecoverTic;

		if( m_AscendancyTeam == TEAM_BLUE )
		{
			if( m_fRedGauge > 0.0f )
			{
				if( m_fRedGauge > m_fCurRecoverGauge )
				{
					m_fRedGauge -= m_fCurRecoverGauge;
				}
				else
				{
					float fGapGauge = m_fCurRecoverGauge - m_fRedGauge;
					m_fRedGauge = 0.0f;
					m_fBlueGauge += fGapGauge;
				}
			}
			else
			{
				m_fBlueGauge += m_fCurRecoverGauge;
			}

			if( m_fBlueGauge >= m_fMaxGauge )
			{
				m_fBlueGauge = 0.0f;
				SetScore( TEAM_BLUE, m_bZeroHP );
			}
		}
		else if( m_AscendancyTeam == TEAM_RED )
		{
			if( m_fBlueGauge > 0.0f )
			{
				if( m_fBlueGauge > m_fCurRecoverGauge )
				{
					m_fBlueGauge -= m_fCurRecoverGauge;
				}
				else
				{
					float fGapGauge = m_fCurRecoverGauge - m_fBlueGauge;
					m_fBlueGauge = 0.0f;
					m_fRedGauge += fGapGauge;
				}
			}
			else
			{
				m_fRedGauge += m_fCurRecoverGauge;
			}

			if( m_fRedGauge >= m_fMaxGauge )
			{
				m_fRedGauge = 0.0f;
				SetScore( TEAM_RED, m_bZeroHP );
			}
		}
	}
	LOOP_GUARD_CLEAR();

	m_dwTicCheckTime = dwCurTime - dwGapTime;
}

void SymbolMode::UpdateRoundRecord()
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

	m_iRedTeamWinCnt += m_iRedSymbolCnt;
	m_iBlueTeamWinCnt += m_iBlueSymbolCnt;
}

TeamType SymbolMode::CheckCurWinTeam()
{
	int iCurRedWinCnt, iCurBlueWinCnt;
	iCurRedWinCnt = m_iRedTeamWinCnt + m_iRedSymbolCnt;
	iCurBlueWinCnt = m_iBlueTeamWinCnt + m_iBlueSymbolCnt;

	if( iCurRedWinCnt > iCurBlueWinCnt )
		return TEAM_RED;
	else if( iCurRedWinCnt < iCurBlueWinCnt )
		return TEAM_BLUE;

	return TEAM_NONE;
}

// 모드 : 09.06.03
void SymbolMode::SetScore( TeamType eTeam, bool bLast )
{
	if(eTeam == TEAM_BLUE)
	{
		if( GetTeamUserCnt( TEAM_BLUE ) == 0 )
			return;

		m_iBlueSymbolCnt++;
		//UpdateSymbolGauge();

		SendScore( TEAM_BLUE, bLast );
	}
	else if(eTeam == TEAM_RED)
	{
		if( GetTeamUserCnt( TEAM_RED ) == 0 )
			return;

		m_iRedSymbolCnt++;
		//UpdateSymbolGauge();

		SendScore( TEAM_RED, bLast );
	}
}

/*
void SymbolMode::SetScore( TeamType eTeam, bool bLast )
{
	if(eTeam == TEAM_BLUE)
	{
		if( GetTeamUserCnt( TEAM_BLUE ) == 0 )
			return;

		m_iBlueSymbolCnt++;
		UpdateSymbolGauge();

		SendScore( TEAM_BLUE, bLast );
	}
	else if(eTeam == TEAM_RED)
	{
		if( GetTeamUserCnt( TEAM_RED ) == 0 )
			return;

		m_iRedSymbolCnt++;
		UpdateSymbolGauge();

		SendScore( TEAM_RED, bLast );
	}
}
*/

void SymbolMode::SendScore(TeamType eTeam, bool bLast )
{
	SP2Packet kPacket( STPK_MODE_SCORE );
	kPacket << m_iBlueSymbolCnt;
	kPacket << m_iRedSymbolCnt;
	kPacket << (int)eTeam;
	kPacket << bLast;
	kPacket << m_fCurRecoverGauge;
	
	m_pCreator->RoomSendPacketTcp( kPacket );
}

bool SymbolMode::IsEnableDie( int iSymbolIdx, TeamType eAttackTeam )
{
	SymbolStruct &rkTarget = m_vSymbolList[iSymbolIdx];

	if( rkTarget.m_Team == TEAM_NONE )
		return true;

	if( m_bNoLimiteSymbol )
		return true;

	int iSymbolCnt = m_vSymbolList.size();
	int iTeamSymbolCnt = 0;
	for( int i=0 ; i<iSymbolCnt ; i++ )
	{	
		SymbolStruct &rkSymbol = m_vSymbolList[i];
		if( !rkSymbol.m_bActive ) continue;

		if( rkSymbol.m_Team == eAttackTeam )
			iTeamSymbolCnt++;
	}

	if( iTeamSymbolCnt == 0 )
	{
		if( eAttackTeam == TEAM_BLUE && iSymbolIdx == 0 )
			return true;
		else if( eAttackTeam == TEAM_RED && iSymbolIdx == iSymbolCnt-1 )
			return true;
	}

	int iBeforeSymbolIndex, iAfterSymbolIndex;
	
	iBeforeSymbolIndex = iSymbolIdx -1;
	iAfterSymbolIndex = iSymbolIdx + 1;

	if( COMPARE( iBeforeSymbolIndex, 0, iSymbolCnt ) )
	{
		if( !m_vSymbolList[iBeforeSymbolIndex].m_bActive )
		{
			iBeforeSymbolIndex--;

			if( COMPARE( iBeforeSymbolIndex, 0, iSymbolCnt ) && 
				m_vSymbolList[iBeforeSymbolIndex].m_Team == eAttackTeam )
				return true;
		}
		else
		{
			if( m_vSymbolList[iBeforeSymbolIndex].m_Team == eAttackTeam )
				return true;
		}
	}

	if( COMPARE( iAfterSymbolIndex, 0, iSymbolCnt ) )
	{
		if( !m_vSymbolList[iAfterSymbolIndex].m_bActive )
		{
			iAfterSymbolIndex++;

			if( COMPARE( iAfterSymbolIndex, 0, iSymbolCnt ) && 
				m_vSymbolList[iAfterSymbolIndex].m_Team == eAttackTeam )
				return true;
		}
		else
		{
			if( m_vSymbolList[iAfterSymbolIndex].m_Team == eAttackTeam )
				return true;
		}
	}

	return false;
}

void SymbolMode::SendScoreGauge()
{
	SP2Packet kPacket( STPK_UPDATE_SCORE_GAUGE );
	kPacket << m_fCurRecoverGauge;
	SendRoomAllUser( kPacket );
}

int SymbolMode::GetCurTeamUserCnt( TeamType eTeam )
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
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SymbolMode::GetCurTeamUserCnt() - User's ModeRecord is Not Exist(%d).", i );
		}
	}

	return iUserCnt;
}

bool SymbolMode::CheckRoundJoin( User *pSend )
{
	if( m_ModeState != MS_RESULT && m_ModeState != MS_RESULT_WAIT )
		return false;

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SymbolMode::CheckRoundJoin - %s Not Exist Record",
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

	// 모드 : 09.06.03
	//UpdateSymbolGauge();

	SP2Packet kPacket( STPK_ROUND_JOIN );
	kPacket << m_fCurRecoverGauge;
	kPacket << pRecord->pUser->GetPublicID();
	kPacket << iModeState;
	kPacket << dwPastTime;
	kPacket << GetSelectCharTime();
	kPacket << m_dwCurRoundDuration;
	SendRoomAllUser( kPacket );

	return true;
}

float SymbolMode::CheckDecreaseGaugeByPeopleCnt()
{
	int iGapCnt = GetCurTeamUserCnt( TEAM_RED ) + GetCurTeamUserCnt( TEAM_BLUE ) - 2;
	iGapCnt = max( 0, min(iGapCnt, 14) );

	float fRate = m_fDecreaseScoreTimeRate * (float)iGapCnt / 14.0f;
	float fGapGauge = m_fCurRecoverGauge * fRate;
	m_fCurRecoverGauge += fGapGauge;

	return m_fCurRecoverGauge;
}
