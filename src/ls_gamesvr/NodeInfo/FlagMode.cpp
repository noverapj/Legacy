#include "stdafx.h"
#include "../MainProcess.h"

#include "FlagMode.h"




#include "Room.h"
#include "FlagModeHelp.h"

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


FlagMode::FlagMode( Room *pCreator ) : Mode( pCreator )
{
	m_dwCurDecreaseTickTime = 0;
	m_dwRoundEndContribute  = 0;
	m_dwBlueContribute		= 0;
	m_dwRedContribute		= 0;

	m_bRoundEndContribute   = false;

	m_dwBlueCheckFlagPingTime = 0;
	m_dwRedCheckFlagPingTime  = 0;

	m_iBlueUserGap			  = 0;
	m_iRedUserGap			  = 0;

	m_fCurBlue_FlagPoint = 0.0f;
	m_fCurRed_FlagPoint = 0.0f;
	tick_start = false;
	flag_tick_count	= 0;

	m_iFagRegenerationTime = 10;
	m_fFlagKillPoint = 0.0f;
	m_fFlagTimePoint = 0.0f;
	m_iFlagTimePointMax = 100;
	m_iFlagEnableZoneRange = 100;

	m_dwRetentionTime = 0;

}


FlagMode::~FlagMode()
{
}

void FlagMode::DestroyMode()
{
	Mode::DestroyMode();

	m_vRecordList.clear();
}

void FlagMode::LoadINIValue()
{
	Mode::LoadINIValue();

	m_dwCurRoundDuration = m_dwRoundDuration;
	m_vRoundHistory.clear();
}
	
void FlagMode::LoadRoundCtrlValue( ioINILoader &rkLoader )
{
	Mode::LoadRoundCtrlValue( rkLoader );

	rkLoader.SetTitle( "round" );
	m_fWinScoreConstant			= rkLoader.LoadFloat( "win_score_constant", 1.0f );

	m_fScoreGapConst			= rkLoader.LoadFloat( "score_gap_const", 50.0f );
	m_fScoreGapRateConst		= rkLoader.LoadFloat( "score_gap_rate_const", 1.5f );
	m_fLadderScoreGapConst		= rkLoader.LoadFloat( "ladder_score_gap_const", 50.0f );
	m_fLadderScoreGapRateConst  = rkLoader.LoadFloat( "ladder_score_gap_rate_const", 1.5f );

	rkLoader.SetTitle( "crown_point" );
	m_fDefaultFlagPoint = rkLoader.LoadFloat( "max_crown_point", 100.0f );
	m_fCurRedCrownPoint  = m_fDefaultFlagPoint;
	m_fCurBlueCrownPoint = m_fDefaultFlagPoint;

	LoadFlagPing( rkLoader );
	LoadBasicFlag( rkLoader );			
}

void FlagMode::LoadFlagPing( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "flag_ping_check" );
	m_dwFlagPingTime = rkLoader.LoadInt( "flag_ping_time", 0 );
	m_dwFlagPingCnt  = rkLoader.LoadInt( "flag_ping_cnt", 4 );
}

void FlagMode::LoadBasicFlag( ioINILoader &rkLoader )
{
	rkLoader.SetTitle( "flag_basic_info" );
	
	int iRegeneTime = rkLoader.LoadInt( "flag_regeneration_time_sec", 10 );
	SetFlagRegeneTime(max(iRegeneTime,1));

	float KillPoint = rkLoader.LoadFloat( "flag_kill_point", 0 );
	SetFlagKillPoint( max(KillPoint,0.0f));

	float timePoint = rkLoader.LoadFloat( "flag_own_time_point", 0 );
	SetFlagTimePoint( max(timePoint,0.0f));

	int iTimeMaxPoint = rkLoader.LoadInt( "flag_own_time_point_max_values", 100 );
	SetFlagTimePointMax( max(iTimeMaxPoint,0));
	
	int iTimeMinPoint = rkLoader.LoadInt( "flag_own_time_point_min_values", 1 );
	SetFlagTimePointMin( max(iTimeMinPoint,0));

	int iRange = rkLoader.LoadInt( "flag_enable_zone_range", 100 );
	SetFlagEnableZoneRange( max(iRange,10));

	int iTerm = rkLoader.LoadInt( "flag_return_term", 5000 );
	SetFlagReturnTerm( max(iTerm,1000));

	SetFlagEnableZoneRange( max(iRange,10));


#ifdef FLAG_MODE_BY_BCKIM_DEBUG
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:LoadBasicFlag] KillPoint[%f],timePoint[%f]",KillPoint,timePoint);
#endif // FLAG_MODE_BY_BCKIM_DEBUG


}
// End. 2018-03-15 by bckim, 깃발모드 추가

void FlagMode::InitObjectGroupList()
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

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// [POSITION_INIT] InitObjectGroupList
		Vector3 vPos_temp = pItem->GetItemPos();
		LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:InitObjectGroupList]COUNT[%d]FLAG_POS(x,y,x[%2f][%2f][%2f])RoomIndex[%d],idx[%d]",i,vPos_temp.x,vPos_temp.y,vPos_temp.z, m_pCreator->GetRoomIndex(),pItem->GetGameIndex());
#endif // FLAG_MODE_BY_BCKIM_DEBUG

	}	

	SendRoomAllUser( kPacket );
}

void FlagMode::FlagModeAddPoint(TeamType iType,  User *pUser)
{
	float increasepoint = GetFlagKillPoint();
	if ( iType == TEAM_BLUE )
		m_fCurRed_FlagPoint += increasepoint;
		
	else if ( iType == TEAM_RED )
		m_fCurBlue_FlagPoint += increasepoint;


	SP2Packet kPacket( STPK_FLAG_KILL_POINT );
	PACKET_GUARD_VOID( kPacket.Write(m_pCreator->GetRoomIndex()));
	PACKET_GUARD_VOID( kPacket.Write(m_fCurBlue_FlagPoint));		// 블루 포인트
	PACKET_GUARD_VOID( kPacket.Write(m_fCurRed_FlagPoint));			// 레드 포인트
	SendRoomAllUser( kPacket, pUser );
	return;
}

void FlagMode::LoadWearItem( ItemVector &rvItemList )
{
	// ini load
	ioINILoader &rkLoader = g_ModeINIMgr.GetINI( GetModeINIFileName() );
	
	int iSubNum = GetModeSubNum();
	int iGroupNum = GetModeMapNum();

	char szBuf[MAX_PATH]="", szTitle[MAX_PATH]="";
	wsprintf( szTitle, "flag%d_object_group%d", iSubNum, iGroupNum );
	rkLoader.SetTitle( szTitle );
	int iWearItemCnt = rkLoader.LoadInt( "wear_item_cnt", 0 );

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// 로드 ini // LoadWearItem
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:FlagMode::LoadWearItem] iSubNum[%d]iGroupNum[%d]iWearItemCnt[%d]",iSubNum,iGroupNum,iWearItemCnt);
#endif // FLAG_MODE_BY_BCKIM_DEBUG


	LoadFlagPosition( rkLoader );	

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
			//Vector3 vPos = GetRandomWearPos( true, pItem->GetItemTeamType() );

			Vector3 vPos = GetFlagPosition();
			pItem->SetItemPos( vPos );
			rvItemList.push_back( pItem );
		}
	}
}

void FlagMode::LoadWearPosList( ioINILoader &rkLoader, Vector3Vec& vVec )
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

void FlagMode::LoadFlagPosition(ioINILoader &rkLoader)
{
	char szTitle[MAX_PATH] = "";

	int iSubNum = GetModeSubNum();
	int iMapIndex = GetModeMapNum();

	wsprintf( szTitle, "flag%d_position_generate%d", iSubNum, iMapIndex );
	rkLoader.SetTitle( szTitle );
	LoadFlagPosition( rkLoader, m_vFlagPosition );

	wsprintf( szTitle, "flag%d_zone%d_blue", iSubNum, iMapIndex );
	rkLoader.SetTitle( szTitle );
	LoadFlagPosition( rkLoader, m_vFlagZone_Blue );

	wsprintf( szTitle, "flag%d_zone%d_red", iSubNum, iMapIndex );
	rkLoader.SetTitle( szTitle );
	LoadFlagPosition( rkLoader, m_vFlagZone_Red );
}

void FlagMode::LoadFlagPosition( ioINILoader &rkLoader, Vector3& vPos )
{	
	vPos.x = 0.0f;
	vPos.y = 0.0f;
	vPos.z = 0.0f;

	char szBuf[MAX_PATH]="";
	memset(szBuf, 0, MAX_PATH);
	wsprintf( szBuf, "pos_x");
	vPos.x = rkLoader.LoadFloat( szBuf, 0.0f );
	
	memset(szBuf, 0, MAX_PATH);
	wsprintf( szBuf, "pos_y");
	vPos.y = rkLoader.LoadFloat( szBuf, 0.0f );		
	
	memset(szBuf, 0, MAX_PATH);
	wsprintf( szBuf, "pos_z");
	vPos.z = rkLoader.LoadFloat( szBuf, 0.0f );
}

/*
Vector3 FlagMode::GetRandomWearPos( bool bStartRound, int iTeamType )
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
*/
Vector3 FlagMode::GetFlagPosition()
{
	return m_vFlagPosition;
}

Vector3 FlagMode::GetFlagZonePositon(TeamType iTeam)
{
	Vector3 vec(1000,1000,1000);

	switch(iTeam)
	{
	case TEAM_BLUE:
		vec = m_vFlagZone_Blue;
		break;
	case TEAM_RED:
		vec = m_vFlagZone_Red;
		break;
	}

	return vec;
}

Vector3 FlagMode::GetRandomItemPos(ioItem *pItem)
{
	if(pItem)
	{
		int iEquipSlot = Help::GetEquipSlot( pItem->GetItemCode() );
		if(iEquipSlot == EQUIP_WEAR)
			//return GetRandomWearPos( false, pItem->GetItemTeamType() );
			return GetFlagPosition();
	}

	return Mode::GetRandomItemPos( pItem );
}

void FlagMode::CheckCreateFlag( User *pUser )
{
	if(!m_pCreator) return;
	if(!pUser) return;

	ioItem *pDropItem = pUser->ReleaseItem(EQUIP_WEAR);

	if(!pDropItem) 
		return;			// 깃발 아이템이 아니면.. 

	// if( pDropItem->GetItemTeamType() != TEAM_NONE ) return;

	Set_Ownerless_Status();	
	pDropItem->SetItemPos( GetFlagPosition());
	
#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// [POSITION] CheckCreateFlag(SetItemPos)
	Vector3 vPos_temp = pDropItem->GetItemPos();
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:CheckCreateFlag] FLAG_POS(x,y,x[%2f][%2f][%2f])",vPos_temp.x,vPos_temp.y,vPos_temp.z);
#endif // FLAG_MODE_BY_BCKIM_DEBUG

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

DWORD FlagMode::GetRetentionTime()
{
	return m_dwRetentionTime;
}

void FlagMode::SetFlagRegeneTime(int iFagRegenerationTime)
{
	m_iFagRegenerationTime = iFagRegenerationTime;
}
void FlagMode::SetFlagKillPoint(float fKillPoint)
{
	m_fFlagKillPoint = fKillPoint;
}
void FlagMode::SetFlagTimePoint(float fTimePoint)
{
	m_fFlagTimePoint = fTimePoint;
}

void FlagMode::SetFlagTimePointMax(int iTimePointMax)
{//flag_own_time_point_max_values
	m_iFlagTimePointMax = iTimePointMax;
}
void FlagMode::SetFlagTimePointMin(int iTimePointMin)
{//flag_own_time_point_max_values
	m_iFlagTimePointMin = iTimePointMin;
}

void FlagMode::SetFlagEnableZoneRange(int iRange)
{
	m_iFlagEnableZoneRange = iRange;
}

void FlagMode::SetFlagReturnTerm(int iTerm)
{
	m_iFlagReturnTerm = iTerm;
}


int FlagMode::GetFlagRegeneTime()
{
	return m_iFagRegenerationTime;
}
float FlagMode::GetFlagKillPoint()
{
	return m_fFlagKillPoint;
}
float FlagMode::GetFlagTimePoint()
{
	return m_fFlagTimePoint;
}
int FlagMode::GetFlagTimePointMax()
{
	return min(m_iFlagTimePointMax,100);
}
int FlagMode::GetFlagTimePointMin()
{
	return max(m_iFlagTimePointMin,1);
}
int FlagMode::GetFlagEnableZoneRange()
{
	return m_iFlagEnableZoneRange;
}
int FlagMode::GetFlagReturnTerm()
{
	return m_iFlagReturnTerm;
}

void FlagMode::Set_Owner_Status(const ioHashString& szPublicID)
{
	//if ( m_szFlagCaptureName.IsEmpty() == TRUE )
	{
		m_szFlagCaptureName = szPublicID;

		CTime cRechargeTime = CTime::GetCurrentTime();
		m_dwRetentionTime = (DWORD)cRechargeTime.GetTime();	
	}
	return;
}

void FlagMode::Set_Ownerless_Status ()
{
	//if ( m_szFlagCaptureName.IsEmpty() == FLASE )
		m_szFlagCaptureName.Clear();
		m_dwRetentionTime = 0;
}

bool FlagMode::Is_Flag_Owner(const ioHashString& szPublicID)
{
	if ( m_szFlagCaptureName == szPublicID )
		return true;

	return false;
}

const ioHashString& FlagMode::Get_Current_Owner_PublicID()
{
	return m_szFlagCaptureName;
}

// End. 2018-03-15 by bckim, 깃발모드 추가	

void FlagMode::CheckBadPingDropCrown( User *pUser )
{
}

void FlagMode::AddNewRecord( User *pUser )
{
	FlagRecord kRecord;
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
//			m_BlueSyncEventTableList.push_back( Event );
		}

		//레드
		{
			SyncEventTable Event;
			Event.iTime		 = TIMEGETTIME();
			Event.eEventType = SET_USER_JOIN;
			Event.iValue = GetTeamUserCnt( TEAM_RED ) - GetTeamUserCnt( TEAM_BLUE );
//			m_RedSyncEventTableList.push_back( Event );
		}		
	}

	UpdateUserRank();
}

void FlagMode::RemoveRecord( User *pUser, bool bRoomDestroy )
{
	CheckCreateFlag( pUser ); // 삭제 전에 실행

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
//		m_BlueSyncEventTableList.push_back( Event );
	}
	
	//레드
	{
		SyncEventTable Event;
		Event.iTime		 = TIMEGETTIME();
		Event.eEventType = SET_USER_LEAVE;
		Event.iValue = GetTeamUserCnt( TEAM_RED ) - GetTeamUserCnt( TEAM_BLUE );
//		m_RedSyncEventTableList.push_back( Event );
	}

	if( pUser )
	{
		m_KickOutVote.RemoveVoteUserList( pUser->GetPublicID() );
	}
}

void FlagMode::ProcessPlay()
{
	ProcessRevival();
	ProcessFlagPing();

	RecreateFlagPing();

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

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// 전투시작 임시 값 확인  ProcessPlay			// 라이브 패치떄는 지울 것 
	ioHashString temp = Get_Current_Owner_PublicID();
	if( m_pCreator )
	{
		LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:ProcessPlay]m_szFlagCaptureName A_NAME[%s] RoomIndex[%d]",temp.c_str(),m_pCreator->GetRoomIndex());
	}	
#endif // FLAG_MODE_BY_BCKIM_DEBUG	

}

void FlagMode::RestartMode()
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
		FlagRecord &rkRecord = m_vRecordList[i];
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

	m_fCurRedCrownPoint  = m_fDefaultFlagPoint;
	m_fCurBlueCrownPoint = m_fDefaultFlagPoint;	

	m_fCurBlue_FlagPoint = 0.0f;
	m_fCurRed_FlagPoint = 0.0f;
	
	m_dwRoundEndContribute = 0;
	m_dwBlueContribute = 0;
	m_dwRedContribute  = 0;

	m_dwBlueCheckFlagPingTime = 0;
	m_dwRedCheckFlagPingTime  = 0;

	m_iBlueUserGap = 0;
	m_iRedUserGap  = 0;

	m_dwRetentionTime = 0;

//	m_BlueSyncEventTableList.clear();
//	m_RedSyncEventTableList.clear();

	m_pCreator->DestroyAllFieldItems();
	SetModeState( MS_READY );

	SP2Packet kPacket( STPK_ROUND_READY );
	kPacket << m_iCurRound;
	kPacket << m_iBluePosArray;
	kPacket << m_iRedPosArray;
	kPacket << m_dwCurRoundDuration;

	kPacket << m_fCurRed_FlagPoint;
	kPacket << m_fCurBlue_FlagPoint;

	SendRoomAllUser( kPacket );

	InitObjectGroupList();
}

int FlagMode::GetUserKickVoteLimit( const ioHashString &szKickUserName )
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

ModeRecord* FlagMode::FindModeRecord( const ioHashString &rkName )
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

ModeRecord* FlagMode::FindModeRecord( User *pUser )
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

ModeRecord* FlagMode::FindModeRecord( int iIdx )
{
	if( COMPARE( iIdx, 0, GetRecordCnt() ) )
		return &m_vRecordList[iIdx];

	return NULL;
}

FlagRecord* FlagMode::FindFlagRecord( const ioHashString &rkName )
{
	return (FlagRecord*)FindModeRecord( rkName );
}

FlagRecord* FlagMode::FindFlagRecord( User *pUser )
{
	return (FlagRecord*)FindModeRecord( pUser );
}

void FlagMode::SetModeState( ModeState eState )
{
	Mode::SetModeState( eState );

	if( eState == MS_PLAY )
	{
		m_iBlueUserGap	= GetTeamUserCnt( TEAM_BLUE ) - GetTeamUserCnt( TEAM_RED );
		m_iRedUserGap	= GetTeamUserCnt( TEAM_RED ) - GetTeamUserCnt( TEAM_BLUE );
	}
}

ModeType FlagMode::GetModeType() const
{
	return MT_FLAG;
}

void FlagMode::GetModeInfo( SP2Packet &rkPacket )
{
	Mode::GetModeInfo( rkPacket );
	
	rkPacket << m_fCurRed_FlagPoint;
	rkPacket << m_fCurBlue_FlagPoint;

	GetModeHistory( rkPacket );
}

void FlagMode::GetExtraModeInfo( SP2Packet &rkPacket )
{
	rkPacket << m_fCurRed_FlagPoint;
	rkPacket << m_fCurBlue_FlagPoint;
}

void FlagMode::GetModeHistory( SP2Packet &rkPacket )
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

void FlagMode::GetCharModeInfo( SP2Packet &rkPacket, const ioHashString &rkName, bool bDieCheck )
{
	FlagRecord *pRecord = FindFlagRecord( rkName );
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

int FlagMode::GetRecordCnt() const
{
	return m_vRecordList.size();
}

const char* FlagMode::GetModeINIFileName() const
{
	return "config/flagcapturemode.ini";
}

TeamType FlagMode::GetNextTeamType()
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

void FlagMode::CheckRoundEnd( bool bProcessCall )
{
	if( m_bRoundSetEnd )
		return;

	DWORD dwCurTime = TIMEGETTIME();

	//기여도 승패 판정 패킷을 보낸 후 3초안에 승패 판정이 안났으면 서버에 현재 까지 수집된 기여도 결과로 강제판단
	if( 0 < m_dwRoundEndContribute && m_dwRoundEndContribute + 3000 < dwCurTime )
	{
		WinTeamType eWinTeam = CheckFlagContributePointWinTeam();
		SendRoundEndContributeResult();
		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
		return;
	}
		
	if( !bProcessCall && m_bRoundEndContribute )
	{
		WinTeamType eWinTeam = CheckFlagContributePointWinTeam();
		SendRoundEndContributeResult();
		SetRoundEndInfo( eWinTeam );
		SendRoundResult( eWinTeam );
		return;
	}

	DWORD dwGapTime = dwCurTime - m_dwStateChangeTime;

	WinTeamType eWinTeam = WTT_DRAW;
	TeamType eTeam = CheckFlagPointWinTeam();
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
	/*
	if( eWinTeam == WTT_DRAW && m_fCurRed_FlagPoint == 0.0f && m_fCurBlue_FlagPoint == 0.0f )
	{
		SendRoundEndContribute();
		return;
	}
	*/
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
			if(iBlueUser == 0) 
			{
				if(m_fCurRed_FlagPoint > 50.0f)
					eWinTeam = WTT_RED_TEAM;
			}
			else if(iRedUser == 0) 
			{
				if(m_fCurBlue_FlagPoint > 50.0f)
					eWinTeam = WTT_BLUE_TEAM;
			}			
				else
				{
				eWinTeam = WTT_NONE;
				}
			}
			SetRoundEndInfo( eWinTeam );
			SendRoundResult( eWinTeam );
			return;
	}

	//  데스매치 형식으로  
	if( m_dwCurRoundDuration < dwGapTime+1000 )
	{
		if( !m_bZeroHP )
		{
			m_bZeroHP = true;
			m_dwCurRoundDuration = 0;
			//m_dwCurSuddenDeathDuration = TIMEGETTIME();
			//m_fSuddenDeathBlueCont	   = 0.0f;
			//m_fSuddenDeathRedCont	   = 0.0f;

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
}


void FlagMode::SetRoundEndInfo( WinTeamType eWinTeam )
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

void FlagMode::UpdateRoundRecord()
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

void FlagMode::OnEventSceneEnd( User *pSend, SP2Packet &rkPacket )
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

		kPacket << m_fCurRed_FlagPoint;
		kPacket << m_fCurBlue_FlagPoint;
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

		kPacket << m_fCurRed_FlagPoint;
		kPacket << m_fCurBlue_FlagPoint;
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

		kPacket << m_fCurRed_FlagPoint;
		kPacket << m_fCurBlue_FlagPoint;

		SendRoomAllUser( kPacket );
	}
}

int FlagMode::GetCurTeamUserCnt( TeamType eTeam )
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

bool FlagMode::CheckRoundJoin( User *pSend )
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
	kPacket << m_fCurRed_FlagPoint;
	kPacket << m_fCurBlue_FlagPoint;
	//kPacket << m_fCurRedDecreaseCrownPoint;
	//kPacket << m_fCurBlueDecreaseCrownPoint;
	SendRoomAllUser( kPacket );

	SP2Packet kModeInfoPk( STPK_MODE_INFO );
	GetModeInfo( kModeInfoPk );
	SendRoomPlayUser( kModeInfoPk );
	
	return true;
}

void FlagMode::CheckUserLeaveEnd()
{
	if( m_bRoundSetEnd ) return;

	if( GetTeamUserCnt( TEAM_BLUE ) == 0 ||	GetTeamUserCnt( TEAM_RED ) == 0 )
	{
		CheckRoundEnd( false );
	}
}

void FlagMode::UpdateDropDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
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

void FlagMode::UpdateWeaponDieRecord( User *pDier, const ioHashString &szAttacker, const ioHashString &szBestAttacker )
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

void FlagMode::SendRoundResult( WinTeamType eWinTeam )
{

	/*
	이긴팀 
	블루 점수 
	레드 점수 
	
	PACKET_GUARD_VOID( kPacket.Write(m_fCurBlue_FlagPoint));		// 블루 포인트
	PACKET_GUARD_VOID( kPacket.Write(m_fCurRed_FlagPoint));	

	*/


	SP2Packet kPacket( STPK_ROUND_END );
	kPacket << (int)eWinTeam;
	kPacket << m_iRedTeamWinCnt;
	kPacket << m_iBlueTeamWinCnt;
	
	kPacket << m_fCurRed_FlagPoint;
	kPacket << m_fCurBlue_FlagPoint;

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

	int size = kPacket.GetBufferSize();

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

void FlagMode::ProcessCrownPoint()
{
	if( GetState() != MS_PLAY )
		return;

	//블루 계산
//	m_fCurBlueCrownPoint = 100.0f - CalcCrownPoint( m_BlueSyncEventTableList, TEAM_BLUE );
	m_fCurBlueCrownPoint = max( 0.0f, m_fCurBlueCrownPoint );

	//레드 계산
//	m_fCurRedCrownPoint = 100.0f - CalcCrownPoint( m_RedSyncEventTableList, TEAM_RED );
	m_fCurRedCrownPoint = max( 0.0f, m_fCurRedCrownPoint );
}

TeamType FlagMode::CheckFlagPointWinTeam()
{
	TeamType ePointTeam = TEAM_NONE;

	if( m_fCurBlue_FlagPoint >= 100.0f)
		ePointTeam = TEAM_BLUE;
	else if( m_fCurRed_FlagPoint >= 100.0f )
		ePointTeam = TEAM_RED;

	return ePointTeam;
}

void FlagMode::SendRoundEndContribute()
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

void FlagMode::SendRoundEndContributeResult()
{
	if( !m_bRoundEndContribute )
		return;

	SP2Packet kPacket( STPK_ROUND_END_CONTRIBUTE );
	kPacket << DOUBLECROWN_CONTRIBUTE_END_RESULT;
	kPacket << m_dwBlueContribute;
	kPacket << m_dwRedContribute;
	SendRoomAllUser( kPacket );	
}

WinTeamType FlagMode::CheckFlagContributePointWinTeam()
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
		eWin = WTT_DRAW;  //CheckFlagRandWinTeam();
	
	return eWin;
}

WinTeamType FlagMode::CheckFlagRandWinTeam()
{
	return (WinTeamType)(rand() % 2 + 1);
}

void FlagMode::OnRoundEndContribute( User *pUser, SP2Packet &rkPacket )
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


float FlagMode::GetResultScoreGapValue( bool bLadderPoint, TeamType eWinTeam )
{
	float fBlueRate = m_fCurBlueCrownPoint / m_fDefaultFlagPoint;
	float fRedRate  = m_fCurRedCrownPoint  / m_fDefaultFlagPoint;
	
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

void FlagMode::ProcessFlagPing()
{
	if( m_dwFlagPingTime == 0 )
		return;

	//CheckFlagPing( m_dwBlueCheckFlagPingTime, TEAM_BLUE );
	//CheckFlagPing( m_dwRedCheckFlagPingTime, TEAM_RED );

	DWORD dw_FlagPingTime = GetRetentionTime();
	CheckFlagPing( dw_FlagPingTime, TEAM_BLUE );
	CheckFlagPing( dw_FlagPingTime, TEAM_RED );
}

void FlagMode::CheckFlagPing( DWORD& dwCheckFlagPingTime, TeamType eTeam )
{
	DWORD dwCurTime = TIMEGETTIME();
	DWORD dwGapTime = 0;

	if( dwCurTime > dwCheckFlagPingTime )
		dwGapTime = dwCurTime - dwCheckFlagPingTime;

	if( dwGapTime > m_dwFlagPingTime )
	{
		bool bFlagDrop = false;
		int iCharCnt = m_vRecordList.size();

		for( int i = 0; i < iCharCnt; ++i )
		{
			User *pUser = m_vRecordList[i].pUser;
			if( !pUser )
				continue;

			if( pUser->GetTeam() != eTeam )
				continue;

			ioHashString szFlagName = Get_Current_Owner_PublicID();
			if( pUser->IsRelayUse() )
			{
				if(pUser->DropFlag() >= 1 && !pUser->GetPublicID().IsEmpty() && pUser->GetPublicID() == szFlagName )
				{
					BadPingDropFlag(pUser);					
				}

				pUser->DropFlag(0);
				continue;
			}

			if( !pUser->GetPublicID().IsEmpty() && pUser->GetPublicID() == szFlagName )
			{
				DWORD dwCurCnt = pUser->GetCurFlagPingCnt();

				if( pUser->IsDeveloper() )
					LOG.PrintTimeAndLog( 0, "%s - Flag: %s, PingCnt: %d", __FUNCTION__, szFlagName.c_str(), dwCurCnt );

				if( dwCurCnt < m_dwFlagPingCnt )
				{
					// 깃발 드롭
					bFlagDrop = true;
					BadPingDropFlag( pUser );

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		
				//	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:BadPingDropFlag] dwCurCnt[%d], m_dwFlagPingCnt{%d]",dwCurCnt,m_dwFlagPingCnt);
#endif // FLAG_MODE_BY_BCKIM_DEBUG
				}

				pUser->ClearFlagPingCnt();
				break;
			}
		}

		if( bFlagDrop )
		{
			dwCheckFlagPingTime = 0;
		}
		else
		{
			dwCheckFlagPingTime = dwCurTime;
		}
	}
}

void FlagMode::RecreateFlagPing()
{
	if (tick_start)
	{
		CTime kLimitTime = CTime::GetCurrentTime();
		DWORD dwTime = (DWORD)(kLimitTime.GetTime());	

		int RegenTime = GetFlagRegeneTime();
		if(  (DWORD)RegenTime > ( dwTime - flag_tick_count) )
			return;

		flag_tick_count = 0;
		tick_start = false;

		m_pItem->SetItemPos(GetFlagPosition());
		m_pCreator->AddFieldItem( m_pItem );

		SP2Packet kPacket( STPK_FIELD_ITEM_SUPPLY );
		int iNewItemCnt = 1;			// 깃발은 하나. 
		kPacket << iNewItemCnt;
		kPacket << m_pItem->GetItemCode();
		kPacket << m_pItem->GetItemReinforce();
		kPacket << m_pItem->GetItemMaleCustom();
		kPacket << m_pItem->GetItemFemaleCustom();
		kPacket << m_pItem->GetGameIndex();
		kPacket << m_pItem->GetItemPos();
		kPacket << m_pItem->GetOwnerName();
		kPacket << "";

		SendRoomAllUser( kPacket );	
	}	
}

void FlagMode::BadPingDropFlag( User *pUser )
{
	if( !pUser )
		return;

	if( false == Is_Flag_Owner(pUser->GetPublicID()) )
		return;

	ioItem *pDropItem = pUser->ReleaseItem( EQUIP_WEAR );
	if( !pDropItem )
		return;

	if( !m_pCreator )
		return;

	pDropItem->SetItemPos( GetFlagPosition());
	m_pCreator->AddFieldItem( pDropItem );

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// [POSITION] BadPingDropCrown(SetItemPos)
	Vector3 vPos_temp = pDropItem->GetItemPos();
	LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:BadPing] FLAG_POS(x,y,x[%.2f][%.2f][%.2f])",vPos_temp.x,vPos_temp.y,vPos_temp.z);
#endif // FLAG_MODE_BY_BCKIM_DEBUG

	SP2Packet kPacket( STPK_BAD_PING_FLAG_DROP );
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

bool FlagMode::ProcessTCPPacket( User *pSend, SP2Packet &rkPacket )
{
	if( Mode::ProcessTCPPacket( pSend, rkPacket ) )
		return true;

	switch( rkPacket.GetPacketID() )
	{
	case CTPK_FLAG_SYNC_REQUEST:
		OnFlagSyncRequest( pSend, rkPacket );
		return true;
	case CTPK_FLAG_POINT_IDENTIFY:	
		OnFlagPointIdentify (pSend, rkPacket);		
		return true;
		
	}

	return false;
}

void FlagMode::OnFlagSyncRequest( User *pUser, SP2Packet &rkPacket )
{

	int iGameIndex;
	ioHashString szUserID1;
	ioHashString szUserID2;
	Vector3 vPos;

	PACKET_GUARD_VOID( rkPacket.Read(iGameIndex) );
	PACKET_GUARD_VOID( rkPacket.Read( szUserID1 ) );
	PACKET_GUARD_VOID( rkPacket.Read( szUserID2 ) );
	PACKET_GUARD_VOID( rkPacket.Read(vPos) );
	
	float blue = 3.0f;
	float red = 2.0f;

	SP2Packet kPacket( STPK_FLAG_SYNC_REQUEST );
	PACKET_GUARD_VOID( kPacket.Write(iGameIndex) );
	PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );		// 깃발 소유자 
	PACKET_GUARD_VOID( kPacket.Write(vPos));						// 소유자 깃발 포지션
	PACKET_GUARD_VOID( kPacket.Write(m_fCurBlue_FlagPoint));		// 블루 포인트
	PACKET_GUARD_VOID( kPacket.Write(m_fCurRed_FlagPoint));			// 레드 포인트
	PACKET_GUARD_VOID( kPacket.Write(blue));						// 기타 1
	PACKET_GUARD_VOID( kPacket.Write(red));							// 기타 2
	
	m_pCreator->RoomSendPacketTcp( kPacket );

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// OnFlagSyncRequest		// 라이브 패치떄는 지울 것 
	//LOG.PrintTimeAndLog(0,"[FLAG_MODE_BY_BCKIM:OnFlagSyncRequest]ID[%s] RedPoint[%2f],BluePoint[%2f]",pUser->GetPublicID().c_str(),m_fCurBlue_FlagPoint, m_fCurRed_FlagPoint);	
#endif // FLAG_MODE_BY_BCKIM_DEBUG

}

void FlagMode::OnFlagPointIdentify ( User *pUser, SP2Packet &rkPacket)
{
	int iGameIndex;
	ioHashString szUserID1;
	Vector3 vPos;
	DWORD itemIndex;
	int iItemCode;
	DWORD time;
	int iTeamType;

	PACKET_GUARD_VOID( rkPacket.Read(iGameIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(szUserID1) );
	PACKET_GUARD_VOID( rkPacket.Read(vPos) );
	PACKET_GUARD_VOID( rkPacket.Read(itemIndex) );
	PACKET_GUARD_VOID( rkPacket.Read(iItemCode) );
	PACKET_GUARD_VOID( rkPacket.Read(time) );
	PACKET_GUARD_VOID( rkPacket.Read(iTeamType) );

	// 기본 예외 사항 확인 
	// 팀 상태
	if( pUser->GetTeam() != TEAM_BLUE && pUser->GetTeam() != TEAM_RED )
	{
		SP2Packet kPacket( STPK_FLAG_POINT_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(FLAG_MODE_ERROR) );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );			
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetTeam()) );
		m_pCreator->RoomSendPacketTcp( kPacket );

		LOG.PrintTimeAndLog(0,"[ERROR:FLAG_MODE_ERROR]ID[%s] TEAM[%d]" ,pUser->GetPublicID().c_str(),pUser->GetTeam());	

		return;
	}

	// 소유자 확인 
	if( Is_Flag_Owner(szUserID1) == false )
	{		
		SP2Packet kPacket( STPK_FLAG_POINT_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(FLAG_ITEM_OWNER_ERROR) );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );	
		ioHashString temp = Get_Current_Owner_PublicID();
		PACKET_GUARD_VOID( kPacket.Write(temp));
		m_pCreator->RoomSendPacketTcp( kPacket );

		LOG.PrintTimeAndLog(0,"[ERROR:FLAG_ITEM_OWNER_ERROR]ID[%s] OWNER_ID[%s]",pUser->GetPublicID().c_str(),temp.c_str());	

		return;	
	}
	
	// 위치 확인 
	int iEnableGap = GetFlagEnableZoneRange();	
	Vector3 vPosition = GetFlagZonePositon(pUser->GetTeam());
	
	//LOG.PrintTimeAndLog(0,"[ERROR:FLAG_POINT_DISTANCE_GAP_ERROR]ID[%s] client_vPos[%f,%f,%f]",pUser->GetPublicID().c_str(),vPos.x,vPos.y,vPos.z);
	//LOG.PrintTimeAndLog(0,"[ERROR:FLAG_POINT_DISTANCE_GAP_ERROR]ID[%s] zone_vPosition[%f,%f,%f]",pUser->GetPublicID().c_str(),vPosition.x,vPosition.y,vPosition.z);
	
	int iDistance = (int)sqrt( pow((double)vPosition.x - vPos.x, 2) + pow((double)vPosition.y - vPos.y, 2) + pow((double)vPosition.z - vPos.z, 2) );
	if( iDistance > iEnableGap )
	{		
		SP2Packet kPacket( STPK_FLAG_POINT_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(FLAG_POINT_DISTANCE_GAP_ERROR) );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );		
		PACKET_GUARD_VOID( kPacket.Write(iDistance));					
		m_pCreator->RoomSendPacketTcp( kPacket );
		
		LOG.PrintTimeAndLog(0,"[ERROR:FLAG_POINT_DISTANCE_GAP_ERROR]ID[%s] Distance[%d]>EnableGap[%d] RoomIndex[%d]",pUser->GetPublicID().c_str(),iDistance, iEnableGap,m_pCreator->GetRoomIndex());
		LOG.PrintTimeAndLog(0,"[ERROR:FLAG_POINT_DISTANCE_GAP_ERROR]ID[%s] client_vPos[%f,%f,%f]",pUser->GetPublicID().c_str(),vPos.x,vPos.y,vPos.z);
		LOG.PrintTimeAndLog(0,"[ERROR:FLAG_POINT_DISTANCE_GAP_ERROR]ID[%s] zone_vPosition[%f,%f,%f]",pUser->GetPublicID().c_str(),vPosition.x,vPosition.y,vPosition.z);
		LOG.PrintTimeAndLog(0,"[ERROR:FLAG_POINT_DISTANCE_GAP_ERROR]ID[%s] GetTeam[%d]",pUser->GetPublicID().c_str(),(int)(pUser->GetTeam()));
		
		return; 	
	}

	// 보유시간 차이 확인 
	CTime cRechargeTime = CTime::GetCurrentTime();		// 현재 틱( 현재 시간 ) 
	DWORD TempGapTime = (DWORD)cRechargeTime.GetTime();	
	TempGapTime = TempGapTime - GetRetentionTime();		// 깃발 획듯 시간 - 현재 시간 = 보유 시간 

	int timeTerm = GetFlagReturnTerm();					// 깃발 존에서 최소한 머물러 있어야 하는 시간. 1000
	timeTerm = timeTerm/1000;

//	LOG.PrintTimeAndLog(0,"[ERROR:FLAG_ITEM_OWN_TIME_ERROR]ID[%s] Client_TIME[%d(%d)],Server_Time[%d(%d)] RoomIndex[%d]",pUser->GetPublicID().c_str(),time/1000,time,TempGapTime,GetRetentionTime(),m_pCreator->GetRoomIndex() );	

	// time < 클라이언트에서 주는 값 틱 
	if( !COMPARE(time/1000,  min(TempGapTime - 5,0),TempGapTime + 5) ) 		// 2018-06-22 by bckim, 깃발모드 버그 수정  	
		// && (TempGapTime >= (DWORD)timeTerm) )	// 의미 없음 
	{		
		SP2Packet kPacket( STPK_FLAG_POINT_RESULT );
		PACKET_GUARD_VOID( kPacket.Write(FLAG_ITEM_OWN_TIME_ERROR));
		PACKET_GUARD_VOID( kPacket.Write(iGameIndex) );
		PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );		// 깃발 소유자 
		PACKET_GUARD_VOID( kPacket.Write(TempGapTime));					// 소유자 깃발 포지션
		m_pCreator->RoomSendPacketTcp( kPacket );

		LOG.PrintTimeAndLog(0,"[ERROR:FLAG_ITEM_OWN_TIME_ERROR]ID[%s] Client_TIME[%d],Server_Time[%d] RoomIndex[%d] ",pUser->GetPublicID().c_str(), time,TempGapTime, m_pCreator->GetRoomIndex() );	
		return;	
	}

	// 점수 올리기 
	int iIncreaseTimePointMax = GetFlagTimePointMax();
	int iIncreaseTimePointMin = GetFlagTimePointMin();
	float fIncreaseTimePoint = GetFlagTimePoint();

	float fAddValues = max((float)( TempGapTime * fIncreaseTimePoint),0.0f) + iIncreaseTimePointMin ;
	
	if( pUser->GetTeam() == TEAM_BLUE )
	{
		m_fCurBlue_FlagPoint += min(fAddValues,(float)iIncreaseTimePointMax);
	}
	else if( pUser->GetTeam() == TEAM_RED )
	{
		m_fCurRed_FlagPoint += min(fAddValues,(float)iIncreaseTimePointMax);
	}

	//////////////////////////////////////////////////////////////////////////
	// 소유자 초기화 
	Set_Ownerless_Status ();
	int Selected_char = pUser->GetSelectChar();	
	m_pItem = pUser->ReleaseItem(EQUIP_WEAR);

	//int temp_ItemCode = m_pItem->GetItemCode();			// 사용 안함 
	//int temp_GameIndex = m_pItem->GetGameIndex();		// 사용 안함 
	tick_start = true;
	CTime Temp_current_tiem = CTime::GetCurrentTime();
	flag_tick_count = (DWORD)Temp_current_tiem.GetTime();
	//////////////////////////////////////////////////////////////////////////

	SP2Packet kPacket( STPK_FLAG_POINT_RESULT );
	PACKET_GUARD_VOID( kPacket.Write(FLAG_POINT_IDENTIFY_SUCCESS) );
	PACKET_GUARD_VOID( kPacket.Write(iGameIndex) );
	PACKET_GUARD_VOID( kPacket.Write(pUser->GetPublicID()) );		// 깃발 소유자 
	PACKET_GUARD_VOID( kPacket.Write(vPos));						// 소유자 깃발 포지션
	PACKET_GUARD_VOID( kPacket.Write(m_fCurBlue_FlagPoint));		// 블루 포인트
	PACKET_GUARD_VOID( kPacket.Write(m_fCurRed_FlagPoint));			// 레드 포인트
	PACKET_GUARD_VOID( kPacket.Write(itemIndex));					// 아이템 인덱스
	PACKET_GUARD_VOID( kPacket.Write(iItemCode));					// 아이템 코드 
	PACKET_GUARD_VOID( kPacket.Write(GetFlagRegeneTime()));			// 깃발 리젠 타임.

#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// FLAG_POINT_IDENTIFY_SUCCESS
	LOG.PrintTimeAndLog(0,"[RESULT:FLAG_POINT_IDENTIFY_SUCCESS]ID[%s] RedPoint[%2f],BluePoint[%2f]",pUser->GetPublicID().c_str(),m_fCurRed_FlagPoint, m_fCurBlue_FlagPoint);	
#endif // FLAG_MODE_BY_BCKIM_DEBUG

	m_pCreator->RoomSendPacketTcp( kPacket );

	if( m_fCurBlue_FlagPoint >= 100.0f )
	{
		SendRoundResult((WinTeamType)TEAM_BLUE);
#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// FLAG_POINT_IDENTIFY_SUCCESS
		LOG.PrintTimeAndLog(0,"[TEAM_BLUE_RESULT_WINER]ID[%s] 3=TEAM[%d] RedPoint[%2f],BluePoint[%2f]",pUser->GetPublicID().c_str(),pUser->GetTeam(),m_fCurRed_FlagPoint, m_fCurBlue_FlagPoint);	
#endif // FLAG_MODE_BY_BCKIM_DEBUG
	}
	else if( m_fCurRed_FlagPoint >= 100.0f )
	{
		SendRoundResult((WinTeamType)TEAM_RED);
#ifdef FLAG_MODE_BY_BCKIM_DEBUG		// FLAG_POINT_IDENTIFY_SUCCESS
		LOG.PrintTimeAndLog(0,"[TEAM_RED_RESULT_WINER:]ID[%s] 2=TEAM[%d] RedPoint[%2f],BluePoint[%2f]",pUser->GetPublicID().c_str(),pUser->GetTeam(),m_fCurRed_FlagPoint, m_fCurBlue_FlagPoint);	
#endif // FLAG_MODE_BY_BCKIM_DEBUG
	}
}



