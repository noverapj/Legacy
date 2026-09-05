

#include "stdafx.h"
#include "../MainProcess.h"

#include "DoubleCrownMode.h"

#include "Room.h"
#include "DoubleCrownModeHelp.h"

#include "ioEventUserNode.h"
#include "ioEtcItemManager.h"
#include "LadderTeamManager.h"
#include "ioItemInfoManager.h"
#include "TournamentManager.h"

#include "ioExerciseCharIndexManager.h"

#include "../EtcHelpFunc.h"

#include "../DataBase/LogDBClient.h"
#include "../local/iolocalmanager.h"
#include <strsafe.h>

DoubleCrownMode::DoubleCrownMode( Room *pCreator ) : Mode( pCreator )
{
	m_dwCurDecreaseTickTime = 0;
	m_dwRoundEndContribute  = 0;
	m_dwBlueContribute		= 0;
	m_dwRedContribute		= 0;

	m_bRoundEndContribute   = false;

	m_dwBlueCheckKingPingTime = 0;
	m_dwRedCheckKingPingTime  = 0;

	m_iBlueUserGap			  = 0;
	m_iRedUserGap			  = 0;
}

DoubleCrownMode::~DoubleCrownMode()
{
}

void DoubleCrownMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();
}

void DoubleCrownMode::LoadINIValue()
{
	Mode::LoadINIValue();

	m_dwCurRoundDuration = m_dwRoundDuration;
	m_vRoundHistory.clear();
}
	
void DoubleCrownMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	Mode::LoadRoundCtrlValue( rkLoader );

	rkLoader.SetTitle( "round" );
	m_fWinScoreConstant			= rkLoader.LoadFloat( "win_score_constant", 1.0f );

	m_fScoreGapConst			= rkLoader.LoadFloat( "score_gap_const", 50.0f );
	m_fScoreGapRateConst		= rkLoader.LoadFloat( "score_gap_rate_const", 1.5f );
	m_fLadderScoreGapConst		= rkLoader.LoadFloat( "ladder_score_gap_const", 50.0f );
	m_fLadderScoreGapRateConst  = rkLoader.LoadFloat( "ladder_score_gap_rate_const", 1.5f );

	rkLoader.SetTitle( "crown_point" );
	m_fDefaultCrownPoint = rkLoader.LoadFloat( "max_crown_point", 100.0f );
	m_fCurRedCrownPoint  = m_fDefaultCrownPoint;
	m_fCurBlueCrownPoint = m_fDefaultCrownPoint;

	m_dwDecreaseTickTime  = rkLoader.LoadInt( "crown_decrease_time", 1000 );
	m_fDecreaseCrownPoint = rkLoader.LoadFloat( "crown_decrease_point", 1.0f );

	m_fCurRedDecreaseCrownPoint  = m_fDecreaseCrownPoint;
	m_fCurBlueDecreaseCrownPoint = m_fDecreaseCrownPoint;
	
	LoadKingPing( rkLoader );
	LoadBalance( rkLoader );
}

void DoubleCrownMode::LoadKingPing( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "king_ping_check" );
	m_dwKingPingTime = rkLoader.LoadInt( "king_ping_time", 0 );
	m_dwKingPingCnt  = rkLoader.LoadInt( "king_ping_cnt", 4 );
}

void DoubleCrownMode::LoadBalance( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "crown_point" );
	char szBuf[MAX_PATH];
	int iCount = rkLoader.LoadInt( "crown_member_balance_count", 0 );

	m_MemberBalanceVec.clear();
	if( 0 < iCount )
		m_MemberBalanceVec.reserve( iCount );

	for( int i = 0; i < iCount; ++i )
	{
		wsprintf( szBuf, "crown_member_balance%d", i );
		float fBalance = rkLoader.LoadFloat( szBuf, i * 0.1f );
		m_MemberBalanceVec.push_back( fBalance );
	}
}

float DoubleCrownMode::GetMemberBalanceRate( TeamType eTeam )
{
	int iGap	 = 0;
	int iRedCnt  = GetCurTeamUserCnt( TEAM_RED );
	int iBlueCnt = GetCurTeamUserCnt( TEAM_BLUE );

	switch( eTeam )
	{
	case TEAM_RED:
		{
			iGap = iRedCnt - iBlueCnt;
		}
		break;
	case TEAM_BLUE:
		{
			iGap = iBlueCnt - iRedCnt;
		}
		break;
	default:
		return 0.0f;
	}

	return GetMemberBalanceRateByGap( iGap );
}

float DoubleCrownMode::GetMemberBalanceRateByGap( int iGap )
{
	if( iGap < 0 )
		return 0;

	if( (int)m_MemberBalanceVec.size() <= iGap )
		return m_MemberBalanceVec.back();
	else
		return m_MemberBalanceVec[iGap];
}

void DoubleCrownMode::InitObjectGroupList()
{
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );

	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();
	char szBuf[MAX_PATH], szTitle[MAX_PATH];

	m_vPushStructList.clear();
	m_iPushStructIdx = 0;

	wsprintf( szTitle, "double_crown%d_object_group%d", iSubNum, iGroupNum );
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
	
	LoadWearItem( vItemList );

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

void DoubleCrownMode::LoadWearItem( ItemVector &rvItemList )
{
	// ini load
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );
	
	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();

	char szBuf[MAX_PATH]="", szTitle[MAX_PATH]="";
	wsprintf( szTitle, "doublecrown%d_object_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );
	int iWearItemCnt = rkLoader.LoadInt( "wear_item_cnt", 0 );

	LoadWearPosList( rkLoader, TEAM_BLUE );	
	LoadWearPosList( rkLoader, TEAM_RED );
	LoadWearPosList( rkLoader, TEAM_NONE );

	rkLoader.SetTitle( szTitle );

	for(int i = 0; i < iWearItemCnt; i++ )
	{
		memset(szTitle, 0, MAX_PATH);
		memset(szBuf, 0, MAX_PATH);

		ObjectItem kWearItem;
		wsprintf( szTitle, "wear_item%d_name", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioItem *pItem = m_pCreator->CreateItemByName( szBuf );
		if( pItem )
		{
			Vector3 vPos = GetRandomWearPos( true, pItem->GetItemTeamType() );
			pItem->SetItemPos( vPos );
			rvItemList.push_back( pItem );
		}
	}
}

void DoubleCrownMode::LoadWearPosList(ioINILoader &rkLoader, int iTeamType )
{
	char szTitle[MAX_PATH] = "";

	int iSubNum = GetModeSubNum();
	int iMapIndex = GetModeMapNum();
	switch( iTeamType )
	{
	case TEAM_BLUE:
		{
			wsprintf( szTitle, "doublecrown%d_wear_generate%d_blue", iSubNum, iMapIndex );
			rkLoader.SetTitle( szTitle );
			LoadWearPosList( rkLoader, m_vBlueWearPosList );
		}
		break;

	case TEAM_RED:
		{
			wsprintf( szTitle, "doublecrown%d_wear_generate%d_red", iSubNum, iMapIndex );
			rkLoader.SetTitle( szTitle );
			LoadWearPosList( rkLoader, m_vRedWearPosList );
		}
		break;
	default:
		{
			wsprintf( szTitle, "doublecrown%d_wear_generate%d", iSubNum, iMapIndex );
			rkLoader.SetTitle( szTitle );
			LoadWearPosList( rkLoader, m_vNoneWearPosList );
		}
		break;
	}
}

void DoubleCrownMode::LoadWearPosList( ioINILoader &rkLoader, Vector3Vec& vVec )
{	
	int iItemPosCnt = rkLoader.LoadInt( "pos_cnt", 0 );

	vVec.clear();
	vVec.reserve( iItemPosCnt );

	char szBuf[MAX_PATH]="";
	for( int i=0 ; i<iItemPosCnt ; i++ )
	{
		Vector3 vPos;

		memset(szBuf, 0, MAX_PATH);
		wsprintf( szBuf, "pos%d_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szBuf, 0.0f );
		vPos.y = 0.0f;

		memset(szBuf, 0, MAX_PATH);
		wsprintf( szBuf, "pos%d_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szBuf, 0.0f );		
		vVec.push_back( vPos );
	}
}

Vector3 DoubleCrownMode::GetRandomWearPos( bool bStartRound, int iTeamType )
{
	Vector3Vec Vec;
	switch( iTeamType )
	{
	case TEAM_BLUE:
		Vec = m_vBlueWearPosList;
		break;
	case TEAM_RED:
		Vec = m_vRedWearPosList;
		break;
	default:
		Vec = m_vNoneWearPosList;
		break;
	}

	int iMaxWearPos = Vec.size();	
	if( 0 < iMaxWearPos )
	{
		int iTempArray = rand() % iMaxWearPos;
		if( bStartRound )
			return Vec[0];
		return Vec[iTempArray];
	}

	return Vector3(0,0,0);
}

Vector3 DoubleCrownMode::GetRandomItemPos(ioItem *pItem)
{
	if(pItem)
	{
		int iEquipSlot = Help::GetEquipSlot( pItem->GetItemCode() );
		if(iEquipSlot == EQUIP_WEAR)
			return GetRandomWearPos( false, pItem->GetItemTeamType() );
	}

	return Mode::GetRandomItemPos( pItem );
}

void DoubleCrownMode::CheckDropCrown( ioItem *pItem )
{
	if( !pItem ) return;
	if( pItem->GetCrownItemType() != ioItem::MCT_DOUBLE_CROWN ) return;
	if( pItem->GetItemTeamType() == TEAM_NONE ) return;

	SetTakeKing( (TeamType)pItem->GetItemTeamType(), "" );
}

bool DoubleCrownMode::CheckPrePickCrown( ioItem *pItem, User *pUser )
{
	if( pItem && pItem->GetCrownItemType() == ioItem::MCT_DOUBLE_CROWN ) 
	{		
		if( pUser && pItem->GetItemTeamType() != (int)pUser->GetTeam() )
		{		
			return false;
		}
	}

	return true;
}

void DoubleCrownMode::CheckPickCrown( ioItem *pItem, User *pUser )
{
	if( !pItem ) return;
	if( !pUser ) return;
	if( pItem->GetCrownItemType() != ioItem::MCT_DOUBLE_CROWN ) return;
	if( pItem->GetItemTeamType() == TEAM_NONE ) return;
	
	SetTakeKing( (TeamType)pItem->GetItemTeamType(), pUser->GetPublicID() );
}

void DoubleCrownMode::CheckCreateCrown( User *pUser )
{
	if(!m_pCreator) return;
	if(!pUser) return;

	ioItem *pDropItem = pUser->ReleaseItem(EQUIP_WEAR);

	if(!pDropItem) return;
	if( pDropItem->GetCrownItemType() != ioItem::MCT_DOUBLE_CROWN ) return;	
	if( pDropItem->GetItemTeamType() == TEAM_NONE ) return;
		
	SetTakeKing( (TeamType)pDropItem->GetItemTeamType(), "" );
	pDropItem->SetItemPos( GetRandomWearPos( true, pDropItem->GetItemTeamType() ) );
	
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

void DoubleCrownMode::SetTakeKing( TeamType Type, const ioHashString& szPublicID )
{	
	switch( Type )
	{
	case TEAM_BLUE:
		{
			m_szBlueKingName = szPublicID;
			if( szPublicID.IsEmpty() )
			{
				m_dwBlueCheckKingPingTime = 0;				
			}
			else
			{
				m_dwBlueCheckKingPingTime  = TIMEGETTIME();
			}
		}
		break;
	case TEAM_RED:
		{
			m_szRedKingName =  szPublicID;
			if( szPublicID.IsEmpty() )
			{
				m_dwRedCheckKingPingTime = 0;				
			}
			else
			{
				m_dwRedCheckKingPingTime  = TIMEGETTIME();
			}
		}		
		break;
	}
	
	if( GetState() == MS_PLAY )
	{
		SyncEventTable Event;
		if( szPublicID.IsEmpty() )
		{
			Event.iTime		 = TIMEGETTIME();
			Event.eEventType = SET_CROWN_DROP;

			if( Type == TEAM_BLUE )
				Event.iValue = GetTeamUserCnt( TEAM_BLUE ) - GetTeamUserCnt( TEAM_RED );
			else
				Event.iValue = GetTeamUserCnt( TEAM_RED ) - GetTeamUserCnt( TEAM_BLUE );
		}
		else
		{
			Event.iTime		 = TIMEGETTIME();
			Event.eEventType = SET_CROWN_PICK;

			if( Type == TEAM_BLUE )
				Event.iValue = GetTeamUserCnt( TEAM_BLUE ) - GetTeamUserCnt( TEAM_RED );
			else
				Event.iValue  = GetTeamUserCnt( TEAM_RED ) - GetTeamUserCnt( TEAM_BLUE );
		}

		if( Type == TEAM_BLUE )
			m_BlueSyncEventTableList.push_back( Event );
		else
			m_RedSyncEventTableList.push_back( Event );	
	}

	SP2Packet kPacket( STPK_KING_TAKE );
	kPacket << szPublicID;
	kPacket << m_fCurRedCrownPoint;
	kPacket << m_fCurBlueCrownPoint;
	kPacket << m_fCurRedDecreaseCrownPoint;
	kPacket << m_fCurBlueDecreaseCrownPoint;
	m_pCreator->RoomSendPacketTcp( kPacket );
}

void DoubleCrownMode::CheckBadPingDropCrown( User *pUser )
{
}

void DoubleCrownMode::AddNewRecord( User *pUser )
{
	DoubleCrownRecord kRecord;
	kRecord.pUser = pUser;
	m_vRecordList.push_back( kRecord );

	if( GetState() == MS_PLAY )
	{
		//블루
		{
			SyncEventTable Event;
			Event.iTime		 = TIMEGETTIME();
			Event.eEventType = SET_USER_JOIN;
			Event.iValue = GetTeamUserCnt( TEAM_BLUE ) - GetTeamUserCnt( TEAM_RED );
			m_BlueSyncEventTableList.push_back( Event );
		}

		//레드
		{
			SyncEventTable Event;
			Event.iTime		 = TIMEGETTIME();
			Event.eEventType = SET_USER_JOIN;
			Event.iValue = GetTeamUserCnt( TEAM_RED ) - GetTeamUserCnt( TEAM_BLUE );
			m_RedSyncEventTableList.push_back( Event );
		}		
	}

	UpdateUserRank();
}

void DoubleCrownMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	CheckCreateCrown( pUser ); // 삭제 전에 실행

	int iPreCnt = GetCurTeamUserCnt( pUser->GetTeam() );

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

	//블루
	{
		SyncEventTable Event;
		Event.iTime		 = TIMEGETTIME();
		Event.eEventType = SET_USER_LEAVE;
		Event.iValue = GetTeamUserCnt( TEAM_BLUE ) - GetTeamUserCnt( TEAM_RED );
		m_BlueSyncEventTableList.push_back( Event );
	}
	
	//레드
	{
		SyncEventTable Event;
		Event.iTime		 = TIMEGETTIME();
		Event.eEventType = SET_USER_LEAVE;
		Event.iValue = GetTeamUserCnt( TEAM_RED ) - GetTeamUserCnt( TEAM_BLUE );
		m_RedSyncEventTableList.push_back( Event );
	}

	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	}
}

void DoubleCrownMode::ProcessPlay()
{
	ProcessRevival();
	ProcessKingPing();

	CheckRoundTimePing();
	CheckFieldItemLiveTime();
	CheckItemSupply( m_dwStateChangeTime );
	CheckBallSupply( m_dwStateChangeTime );
	CheckMachineSupply( m_dwStateChangeTime );
	CheckNeedSendPushStruct();
	ProcessCrownPoint();
	CheckRoundEnd( true );
	ProcessEvent();
	ProcessBonusAlarm();
}

void DoubleCrownMode::RestartMode()
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
		DoubleCrownRecord &rkRecord = m_vRecordList[i];
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

	m_szBlueKingName.Clear();
	m_szRedKingName.Clear();

	m_fCurRedCrownPoint  = m_fDefaultCrownPoint;
	m_fCurBlueCrownPoint = m_fDefaultCrownPoint;	
	m_fCurRedDecreaseCrownPoint  = m_fDecreaseCrownPoint;
	m_fCurBlueDecreaseCrownPoint = m_fDecreaseCrownPoint;

	m_dwRoundEndContribute = 0;
	m_dwBlueContribute = 0;
	m_dwRedContribute  = 0;

	m_dwBlueCheckKingPingTime = 0;
	m_dwRedCheckKingPingTime  = 0;

	m_iBlueUserGap = 0;
	m_iRedUserGap  = 0;

	m_BlueSyncEventTableList.clear();
	m_RedSyncEventTableList.clear();

	m_pCreator->DestroyAllFieldItems();
	SetModeState( MS_READY );

	SP2Packet kPacket( STPK_ROUND_READY );
	kPacket << m_iCurRound;
	kPacket << m_iBluePosArray;
	kPacket << m_iRedPosArray;
	kPacket << m_dwCurRoundDuration;

	kPacket << m_fCurRedCrownPoint;
	kPacket << m_fCurBlueCrownPoint;
	kPacket << m_fCurRedDecreaseCrownPoint;
	kPacket << m_fCurBlueDecreaseCrownPoint;
	
	SendRoomAllUser( kPacket );

	InitObjectGroupList();
}

int DoubleCrownMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
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

ModeRecord* DoubleCrownMode::FindModeRecord( const ioHashString &rkName )
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

ModeRecord* DoubleCrownMode::FindModeRecord( User *pUser )
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

ModeRecord* DoubleCrownMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

DoubleCrownRecord* DoubleCrownMode::FindDoubleCrownRecord( const ioHashString &rkName )
{
	return (DoubleCrownRecord*)FindModeRecord( rkName );
}

DoubleCrownRecord* DoubleCrownMode::FindDoubleCrownRecord( User *pUser )
{
	return (DoubleCrownRecord*)FindModeRecord( pUser );
}

void DoubleCrownMode::SetModeState( ModeState eState )
{
	Mode::SetModeState( eState );

	if( eState == MS_PLAY )
	{
		m_iBlueUserGap	= GetTeamUserCnt( TEAM_BLUE ) - GetTeamUserCnt( TEAM_RED );
		m_iRedUserGap	= GetTeamUserCnt( TEAM_RED ) - GetTeamUserCnt( TEAM_BLUE );
	}
}

ModeType DoubleCrownMode::GetModeType() const
{
	return MT_DOBULE_CROWN;
}

void DoubleCrownMode::GetModeInfo( SP2Packet &rkPacket )
{
	Mode::GetModeInfo( rkPacket );
	
	rkPacket << m_fCurRedCrownPoint;
	rkPacket << m_fCurBlueCrownPoint;
	rkPacket << m_fCurRedDecreaseCrownPoint;
	rkPacket << m_fCurBlueDecreaseCrownPoint;	

	GetModeHistory( rkPacket );
}

void DoubleCrownMode::GetExtraModeInfo( SP2Packet &rkPacket )
{
	rkPacket << m_fCurRedCrownPoint;
	rkPacket << m_fCurBlueCrownPoint;
	rkPacket << m_fCurRedDecreaseCrownPoint;
	rkPacket << m_fCurBlueDecreaseCrownPoint;	
}

void DoubleCrownMode::GetModeHistory( SP2Packet &rkPacket )
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

void DoubleCrownMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck )
{
	DoubleCrownRecord *pRecord = FindDoubleCrownRecord( rkName );
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

int DoubleCrownMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* DoubleCrownMode::GetModeINIFileName() const
{
	return "config/doublecrownmode.ini";
}

TeamType DoubleCrownMode::GetNextTeamType()
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

void DoubleCrownMode::CheckRoundEnd( bool bProcessCall )
{
	if( m_bRoundSetEnd )
		return;

	DWORD dwCurTime = TIMEGETTIME();

	//기여도 승패 판정 패킷을 보낸 후 3초안에 승패 판정이 안났으면 서버에 현재 까지 수집된 기여도 결과로 강제판단
	if( 0 < m_dwRoundEndContribute && m_dwRoundEndContribute + 3000 < dwCurTime )
	{
		WinTeamType eWinTeam = CheckCrownContributePointWinTeam();
		SendRoundEndContributeResult();
		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
		return;
	}
		
	if( !bProcessCall && m_bRoundEndContribute )
	{
		WinTeamType eWinTeam = CheckCrownContributePointWinTeam();
		SendRoundEndContributeResult();
		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
		return;
	}

	DWORD dwGapTime = dwCurTime - m_dwStateChangeTime;

	WinTeamType eWinTeam = WTT_DRAW;
	TeamType eTeam = CheckCrownPointWinTeam();
	if( eTeam == TEAM_RED )
		eWinTeam = WTT_RED_TEAM;
	else if( eTeam == TEAM_BLUE )
		eWinTeam = WTT_BLUE_TEAM;

	//완전히 승부가 난 경우
	if( eWinTeam != WTT_DRAW )
	{
		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
		return;
	}

	//동시에 0점이 됬을 경우
	if( eWinTeam == WTT_DRAW && m_fCurRedCrownPoint == 0.0f && m_fCurBlueCrownPoint == 0.0f )
	{
		SendRoundEndContribute();
		return;
	}

	if( !bProcessCall )
	{
		m_dwCurRoundDuration = 0;

		int iBlueUser = GetCurTeamUserCnt( TEAM_BLUE );
		int iRedUser  = GetCurTeamUserCnt( TEAM_RED );
				
		if( GetState() != MS_PLAY && GetState() != MS_READY )
		{
			if( eWinTeam == WTT_DRAW )
			{
				SendRoundEndContribute();
				return;				
			}		

			SetRoundEndInfo( eWinTeam );
			SendRoundResult( eWinTeam );
			return;
		}		
		else if( iBlueUser == 0 || iRedUser == 0 )
		{
			if( m_fCurRedCrownPoint > 90.0f && m_fCurBlueCrownPoint > 90.0f )
			{
				eWinTeam = WTT_DRAW;
			}
			else
			{
				if( iRedUser > iBlueUser )
					eWinTeam = WTT_RED_TEAM;
				else if( iBlueUser > iRedUser )
					eWinTeam = WTT_BLUE_TEAM;
				else
				{
					SendRoundEndContribute();
					return;
				}
			}

			SetRoundEndInfo( eWinTeam );
			SendRoundResult( eWinTeam );
			return;
		}
	}

	// 시간 체크
	if( m_dwCurRoundDuration < dwGapTime + 1000 )
	{
		m_dwCurRoundDuration = 0;

		if( m_fCurRedCrownPoint > m_fCurBlueCrownPoint )
			eWinTeam = WTT_RED_TEAM;
		else if( m_fCurBlueCrownPoint > m_fCurRedCrownPoint )
			eWinTeam = WTT_BLUE_TEAM;
		else
		{
			SendRoundEndContribute();
			return;
		}

		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
		return;
	}
}

void DoubleCrownMode::SetRoundEndInfo( WinTeamType eWinTeam )
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
				{
					pRecord->AddDeathTime( TIMEGETTIME() - pRecord->pUser->GetStartTimeLog() );
				}
				else
				{				
					g_LogDBClient.OnInsertTime( pRecord->pUser, LogDBClient::TT_VIEW );
				}
				pRecord->pUser->SetStartTimeLog(0);
			}
		}
	}
	
	int HistorySize = m_vRoundHistory.size();
	if( m_iCurRound-1 > HistorySize )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DoubleCrownMode::SetRoundEndInfo() m_iCurRound != m_vRoundHistory.size()" );
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

void DoubleCrownMode::UpdateRoundRecord()
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

void DoubleCrownMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DoubleCrownMode::OnEventSceneEnd - %s Not Exist Record",
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

		kPacket << m_fCurRedCrownPoint;
		kPacket << m_fCurBlueCrownPoint;
		kPacket << m_fCurRedDecreaseCrownPoint;
		kPacket << m_fCurBlueDecreaseCrownPoint;
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

		kPacket << m_fCurRedCrownPoint;
		kPacket << m_fCurBlueCrownPoint;
		kPacket << m_fCurRedDecreaseCrownPoint;
		kPacket << m_fCurBlueDecreaseCrownPoint;
		SendRoomAllUser( kPacket );
	}
	else
	{
		int iPreCnt = GetCurTeamUserCnt( pRecord->pUser->GetTeam() );

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

		kPacket << m_fCurRedCrownPoint;
		kPacket << m_fCurBlueCrownPoint;
		kPacket << m_fCurRedDecreaseCrownPoint;
		kPacket << m_fCurBlueDecreaseCrownPoint;

		SendRoomAllUser( kPacket );
	}
}

int DoubleCrownMode::GetCurTeamUserCnt( TeamType eTeam )
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
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DoubleCrownMode::GetCurTeamUserCnt() - User's ModeRecord is Not Exist(%d).", i );
		}
	}

	return iUserCnt;
}

bool DoubleCrownMode::CheckRoundJoin( User *pSend )
{
	if( m_ModeState != MS_RESULT && m_ModeState != MS_RESULT_WAIT )
		return false;

	ModeRecord *pRecord = FindModeRecord( pSend );
	if( !pRecord )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DoubleCrownMode::CheckRoundJoin - %s Not Exist Record",
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

	SP2Packet kPacket( STPK_ROUND_JOIN );
	kPacket << pRecord->pUser->GetPublicID();
	kPacket << iModeState;
	kPacket << dwPastTime;
	kPacket << GetSelectCharTime();
	kPacket << m_dwCurRoundDuration;
	kPacket << m_fCurRedCrownPoint;
	kPacket << m_fCurBlueCrownPoint;
	kPacket << m_fCurRedDecreaseCrownPoint;
	kPacket << m_fCurBlueDecreaseCrownPoint;
	SendRoomAllUser( kPacket );

	SP2Packet kModeInfoPk( STPK_MODE_INFO );
	GetModeInfo( kModeInfoPk );
	SendRoomPlayUser( kModeInfoPk );
	
	return true;
}

void DoubleCrownMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd ) return;

	if( GetTeamUserCnt( TEAM_BLUE ) == 0 ||	GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		CheckRoundEnd( false );
	}
}

void DoubleCrownMode::UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( !pDieRecord )	return;

	// 마지막으로 타격한 유저와 죽은 유저
	TeamType eDieTeam = pDieRecord->pUser->GetTeam();
	if( !szAttacker.IsEmpty() && pDier->GetPublicID() != szAttacker )
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
	else
	{
		pDieRecord->AddDeathCount( m_pCreator->GetRoomStyle(), GetModeType(), GetDeathPoint( pDieRecord->pUser->GetTeam() ) );

	}

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

void DoubleCrownMode::UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
{
	ModeRecord *pDieRecord = FindModeRecord( pDier );
	if( !pDieRecord )	return;

	// 마지막으로 타격한 유저와 죽은 유저
	TeamType eDieTeam = pDieRecord->pUser->GetTeam();
	ModeRecord *pKillRecord = FindModeRecord( szAttacker );
	if( pKillRecord )
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

	// 가장 많은 데미지를 입힌 유저
	ModeRecord *pBestAttackerRecord = FindModeRecord( szBestAttacker );
	if( pBestAttackerRecord )
	{
		float fKillPoint = 0.5f + ( (float)GetKillPoint( pBestAttackerRecord->pUser->GetTeam() ) * 0.5f );
		if( pBestAttackerRecord->pUser->GetTeam() != pDier->GetTeam() )
			pBestAttackerRecord->AddKillCount( m_pCreator->GetRoomStyle(), GetModeType(), fKillPoint );
	}
}

void DoubleCrownMode::SendRoundResult( WinTeamType eWinTeam )
{
	SP2Packet kPacket( STPK_ROUND_END );
	kPacket << (int)eWinTeam;
	kPacket << m_iRedTeamWinCnt;
	kPacket << m_iBlueTeamWinCnt;
	
	kPacket << m_fCurRedCrownPoint;
	kPacket << m_fCurBlueCrownPoint;
	kPacket << m_fCurRedDecreaseCrownPoint;
	kPacket << m_fCurBlueDecreaseCrownPoint;	

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

void DoubleCrownMode::UpdateCurDecreaseCrownPoint( TeamType eTeam, int iPreCnt )
{
	int iCurCnt = GetCurTeamUserCnt( eTeam );
	if( iCurCnt <= 0 || iPreCnt <= 0 )
		return;

	m_fCurRedDecreaseCrownPoint  = m_fDecreaseCrownPoint + GetMemberBalanceRate( TEAM_RED );
	m_fCurBlueDecreaseCrownPoint = m_fDecreaseCrownPoint + GetMemberBalanceRate( TEAM_BLUE );
}

void DoubleCrownMode::ProcessCrownPoint()
{
	if( GetState() != MS_PLAY )
		return;

	//블루 계산
	m_fCurBlueCrownPoint = 100.0f - CalcCrownPoint( m_BlueSyncEventTableList, TEAM_BLUE );
	m_fCurBlueCrownPoint = max( 0.0f, m_fCurBlueCrownPoint );

	//레드 계산
	m_fCurRedCrownPoint = 100.0f - CalcCrownPoint( m_RedSyncEventTableList, TEAM_RED );
	m_fCurRedCrownPoint = max( 0.0f, m_fCurRedCrownPoint );
}

float DoubleCrownMode::CalcCrownPoint( SyncEventTableList& TableList, TeamType eTeam )
{
	//모드 시작시간
	int iStartTime = m_dwStateChangeTime;

	//이전
	float fCurrPoint	= 0;
	int	  iPrevValue	= 0;
	if( eTeam == TEAM_BLUE )
		iPrevValue = m_iBlueUserGap;
	else
		iPrevValue = m_iRedUserGap;

	BYTE eCurrCorwn	= SET_CROWN_DROP;

	for( int i = 0 ; i < (int)TableList.size(); ++i )
	{
		const SyncEventTable& rkCurrTable = TableList[i];
		
		//왕관 드롭 상태일때만 계산
		float fGapTime	= 0;
		float fQuotient	= 0;
		float fPoint	= 0;

		if( eCurrCorwn != SET_CROWN_PICK )
		{
			fGapTime	= (float)max( 0, rkCurrTable.iTime - iStartTime );
			fQuotient	= fGapTime / (float)m_dwDecreaseTickTime;
			fPoint		= (m_fDecreaseCrownPoint + GetMemberBalanceRateByGap( iPrevValue ) ) * fQuotient;
		}

		fCurrPoint	+= fPoint;
		iPrevValue	= rkCurrTable.iValue;
		iStartTime	= rkCurrTable.iTime;

		if( rkCurrTable.eEventType == SET_CROWN_PICK || rkCurrTable.eEventType == SET_CROWN_DROP )
			eCurrCorwn = rkCurrTable.eEventType;
	}

	//현재
	if( eCurrCorwn != SET_CROWN_PICK )
	{
		float iGapTime  = (float)max( 0, TIMEGETTIME() - iStartTime );
		float fQuotient	= (float)iGapTime / m_dwDecreaseTickTime;
		float fPoint    = (m_fDecreaseCrownPoint + GetMemberBalanceRateByGap( iPrevValue ) ) * fQuotient;

		fCurrPoint	+= fPoint;
	}	

	return fCurrPoint;
}

TeamType DoubleCrownMode::CheckCrownPointWinTeam()
{
	TeamType ePointTeam = TEAM_NONE;

	if( m_fCurBlueCrownPoint > m_fCurRedCrownPoint && m_fCurRedCrownPoint == 0.0f )
		ePointTeam = TEAM_BLUE;
	else if( m_fCurRedCrownPoint > m_fCurBlueCrownPoint && m_fCurBlueCrownPoint == 0.0f )
		ePointTeam = TEAM_RED;

	return ePointTeam;
}

void DoubleCrownMode::SendRoundEndContribute()
{
	if( m_bRoundEndContribute )
		return;

	if( 0 < m_dwRoundEndContribute )
		return;

	SP2Packet kPacket( STPK_ROUND_END_CONTRIBUTE );
	kPacket << DOUBLECROWN_CONTRIBUTE_END;

	int iRecordCnt = GetRecordCnt();
	for( int i = 0 ; i < iRecordCnt ; ++i )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) 
			continue;

		if( !pRecord->pUser ) 
			continue;

		if( pRecord->eState != RS_PLAY )
			continue;

		pRecord->pUser->SendMessage( kPacket );
	}

	m_dwRoundEndContribute = TIMEGETTIME();
}

void DoubleCrownMode::SendRoundEndContributeResult()
{
	if( !m_bRoundEndContribute )
		return;

	SP2Packet kPacket( STPK_ROUND_END_CONTRIBUTE );
	kPacket << DOUBLECROWN_CONTRIBUTE_END_RESULT;
	kPacket << m_dwBlueContribute;
	kPacket << m_dwRedContribute;
	SendRoomAllUser( kPacket );	
}

WinTeamType DoubleCrownMode::CheckCrownContributePointWinTeam()
{
	m_dwBlueContribute = 0;
	m_dwRedContribute  = 0;

	float fBluePer = 0.0f;
	float fRedPer = 0.0f; 

	int iRecordCnt = GetRecordCnt();
	for( int i = 0 ; i < iRecordCnt; ++i )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord )
			continue;

		if( !pRecord->pUser )
			continue;

		if( pRecord->pUser->IsObserver() )
			continue;
		
		if( pRecord->pUser->IsStealth() )
			continue;

		switch( pRecord->pUser->GetTeam() )
		{
		case TEAM_BLUE:
			fBluePer += pRecord->fContributePer;
			break;
		case TEAM_RED:
			fRedPer  += pRecord->fContributePer;
			break;
		}
	}

	m_dwBlueContribute = fBluePer * 100.0f;
	m_dwRedContribute  = fRedPer * 100.0f;

	WinTeamType eWin = WTT_DRAW;
	if( m_dwBlueContribute > m_dwRedContribute )
		eWin = WTT_BLUE_TEAM;
	else if( m_dwRedContribute > m_dwBlueContribute )
		eWin = WTT_RED_TEAM;
	else
		eWin = CheckCrownRandWinTeam();
	
	return eWin;
}

WinTeamType DoubleCrownMode::CheckCrownRandWinTeam()
{
	return (WinTeamType)(rand() % 2 + 1);
}

void DoubleCrownMode::OnRoundEndContribute( User *pUser, SP2Packet &rkPacket )
{
	if( pUser == NULL )
		return;

	if( GetState() != MS_PLAY ) 
		return;
	
	if( m_dwRoundEndContribute == 0 )
		return;

	int iCommand;
	rkPacket >> iCommand;
	if( iCommand != DOUBLECROWN_CONTRIBUTE_END )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Command Error : %s - %d", __FUNCTION__, pUser->GetPublicID().c_str(), iCommand );
		return;
	}

	m_bRoundEndContribute = true;

	int i = 0;
	int iCharCnt;
	rkPacket >> iCharCnt;
	for(i = 0;i < iCharCnt; ++i )
	{
		ioHashString szName;
		int iContribute, iUniqueTotalKill, iUniqueTotalDeath, iVictories;
		rkPacket >> szName >> iContribute >> iUniqueTotalKill >> iUniqueTotalDeath >> iVictories;
		ModeRecord *pRecord = FindModeRecord( szName );
		if( pRecord )
		{
			pRecord->iContribute = iContribute;
			pRecord->iUniqueTotalKill  = iUniqueTotalKill;
			pRecord->iUniqueTotalDeath = iUniqueTotalDeath;
			pRecord->iVictories		   = iVictories;

			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s Recv : %s - %d - %d - %d - %d", __FUNCTION__, szName.c_str(), iContribute, iUniqueTotalKill, iUniqueTotalDeath, iVictories );

			if( iContribute == 0 )
			{
				if( iUniqueTotalKill != 0 || iUniqueTotalDeath != 0 )
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s Error: %s - %d - %d - %d - %d", __FUNCTION__, szName.c_str(), iContribute, iUniqueTotalKill, iUniqueTotalDeath, iVictories );
			}
		}
	}

	int iRecordCnt		= GetRecordCnt();
	int iMaxContribute	= 0;

	for( i = 0; i < iRecordCnt; ++i )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) 
			continue;

		if( !pRecord->pUser ) 
			continue;

		iMaxContribute += pRecord->iContribute;
	}

	// 옵저버 유저 제외
	int iOb = 0;
	for( i = 0; i < iRecordCnt; ++i )
	{
		ModeRecord *pRecord = FindModeRecord( i );
		if( !pRecord ) 
			continue;

		if( !pRecord->pUser )
			continue;

		if( pRecord->pUser->IsObserver() || pRecord->pUser->IsStealth() )
			iOb++;
	}

	if( iMaxContribute > 0 )
	{
		for( i = 0; i < iRecordCnt; ++i )
		{
			ModeRecord *pRecord = FindModeRecord( i );
			if( !pRecord ) 
				continue;

			if( !pRecord->pUser ) 
				continue;

			pRecord->fContributePer = (float)( iRecordCnt - iOb ) * ((float)pRecord->iContribute / iMaxContribute);
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s Per: %s - %.2f - %d - %d - %d", __FUNCTION__, pRecord->pUser->GetPublicID().c_str(), pRecord->fContributePer, iOb, pRecord->iContribute, iMaxContribute );
		}
	}
	
	CheckRoundEnd( false );
}


float DoubleCrownMode::GetResultScoreGapValue( bool bLadderPoint, TeamType eWinTeam )
{
	float fBlueRate = m_fCurBlueCrownPoint / m_fDefaultCrownPoint;
	float fRedRate  = m_fCurRedCrownPoint  / m_fDefaultCrownPoint;
	
	float fScoreGap = 0.0f;
	if( bLadderPoint )
	{
		if( eWinTeam == TEAM_BLUE )
			fScoreGap = ( abs( fRedRate - fBlueRate ) + m_fLadderScoreGapConst ) / ( fBlueRate + m_fLadderScoreGapConst );
		else
			fScoreGap = ( abs( fRedRate - fBlueRate ) + m_fLadderScoreGapConst ) / ( fRedRate + m_fLadderScoreGapConst );


		fScoreGap *= m_fLadderScoreGapRateConst;
	}
	else
	{
		if( eWinTeam == TEAM_BLUE )
			fScoreGap = ( abs( fRedRate - fBlueRate ) + m_fScoreGapConst) / ( fBlueRate + m_fScoreGapConst );
		else
			fScoreGap = ( abs( fRedRate - fBlueRate ) + m_fScoreGapConst) / ( fRedRate + m_fScoreGapConst );

		fScoreGap *= m_fScoreGapRateConst;
	}	

	return fScoreGap;
}

const ioHashString& DoubleCrownMode::GetTeamKing( TeamType eTeam )
{
	static ioHashString szNone;
	switch( eTeam )
	{
	case TEAM_BLUE:
		return m_szBlueKingName;
	case TEAM_RED:
		return m_szRedKingName;
	}

	return szNone;
}

void DoubleCrownMode::ProcessKingPing()
{
	if( m_dwKingPingTime == 0 )
		return;

	CheckKingPing( m_dwBlueCheckKingPingTime, TEAM_BLUE );
	CheckKingPing( m_dwRedCheckKingPingTime, TEAM_RED );
}

void DoubleCrownMode::CheckKingPing( DWORD& dwCheckKingPingTime, TeamType eTeam )
{
	DWORD dwCurTime = TIMEGETTIME();
	DWORD dwGapTime = 0;

	if( dwCurTime > dwCheckKingPingTime )
		dwGapTime = dwCurTime - dwCheckKingPingTime;

	if( dwGapTime > m_dwKingPingTime )
	{
		bool bCrownDrop = false;
		int iCharCnt = m_vRecordList.size();

		for( int i = 0; i < iCharCnt; ++i )
		{
			User *pUser = m_vRecordList[i].pUser;
			if( !pUser )
				continue;

			if( pUser->GetTeam() != eTeam )
				continue;

			ioHashString szKingName = GetTeamKing( pUser->GetTeam() );
			if( pUser->IsRelayUse() )
			{
				if(pUser->DropKing() >= 1 && !pUser->GetPublicID().IsEmpty() && pUser->GetPublicID() == szKingName )
				{
					BadPingDropCrown(pUser);					
				}

				pUser->DropKing(0);
				continue;
			}

			if( !pUser->GetPublicID().IsEmpty() && pUser->GetPublicID() == szKingName )
			{
				DWORD dwCurCnt = pUser->GetCurKingPingCnt();

				if( pUser->IsDeveloper() )
					LOG.PrintTimeAndLog( 0, "%s - King: %s, PingCnt: %d", __FUNCTION__, szKingName.c_str(), dwCurCnt );

				if( dwCurCnt < m_dwKingPingCnt )
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
		{
			dwCheckKingPingTime = 0;
		}
		else
		{
			dwCheckKingPingTime = dwCurTime;
		}
	}
}

void DoubleCrownMode::BadPingDropCrown( User *pUser )
{
	if( !pUser )
		return;

	ioHashString szKing = GetTeamKing( pUser->GetTeam() );
	if( szKing.IsEmpty() ) 
		return;

	if( pUser->GetPublicID() != szKing )
		return;
	
	ioItem *pDropItem = pUser->ReleaseItem( EQUIP_WEAR );
	if( !pDropItem )
		return;

	if( pDropItem->GetCrownItemType() != ioItem::MCT_DOUBLE_CROWN )
		return;

	if( !m_pCreator )
		return;

	SetTakeKing( pUser->GetTeam(), "" );
	pDropItem->SetItemPos( GetRandomWearPos( false, pDropItem->GetItemTeamType() ) );
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

bool DoubleCrownMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_DOUBLE_CROWN_SYNC_REQUEST:
		OnDoubleCrownSyncRequest( pSend, rkPacket );
		return true;
	}

	return false;
}

void DoubleCrownMode::OnDoubleCrownSyncRequest( User *pUser, SP2Packet &rkPacket )
{
	SP2Packet kPacket( STPK_DOUBLE_CROWN_SYNC_REQUEST );
	kPacket << m_fCurRedCrownPoint;
	kPacket << m_fCurBlueCrownPoint;
	kPacket << m_fCurRedDecreaseCrownPoint;
	kPacket << m_fCurBlueDecreaseCrownPoint;
	m_pCreator->RoomSendPacketTcp( kPacket );
}