

#include "stdafx.h"
#include "TournamentNode.h"
#include "ioTournamentManager.h"

#include "GUI/PartyVersusWnd.h"
#include "GUI/VoiceChatWnd.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/ioExpansionBox.h"

template<> ioBattleRoomMgr* Singleton< ioBattleRoomMgr >::ms_Singleton = 0;

ioBattleRoomMgr::ioBattleRoomMgr()
{
	m_BattleRoom.m_dwIndex    = 0;
	m_BattleRoom.m_dwTourIndex	 = 0;
	m_BattleRoom.m_dwBlueTeamIndex= 0;
	m_BattleRoom.m_dwRedTeamIndex = 0;
	m_BattleRoom.m_dwTourStartTimer = 0;
	m_BattleRoom.m_bTourStartSender = false;
	m_iDefaultMaxPlayer       = MAX_PLAYER;
	m_iDefaultMaxObserver	  = MAX_OBSERVER;
	m_bBattleModePlaying	  = true;
	m_iLimitJoinLevel		  = 2;
	m_iBattleDefaultCatchModeLevel = 10;
	m_dwAutoStartAlarmCheckTime = 5000;

	m_iSelectModeIndex  = -1;
	m_iSelectMapIndex	= -1;
	m_iPreSelectModeIndex  = -1;
	m_iPreSelectMapIndex	= -1;
	m_bSafetyLevelRoom  = false;
	m_bChangeMeHostAlarm= false;
	m_iModeStartPlayerCount = 0;
	m_dwProcessTimer = 0;

	InitRecord();
	InitBattleCancelCount();
}

ioBattleRoomMgr::~ioBattleRoomMgr()
{	
	m_vKickOutRoom.clear();
	m_vInvitedRefuse.clear();
	m_vUserList.clear();
}

ioBattleRoomMgr& ioBattleRoomMgr::GetSingleton()
{
	return Singleton< ioBattleRoomMgr >::GetSingleton();
}

void ioBattleRoomMgr::LoadBattleRoomInfo()
{
	{
		ioINILoader_e kLoader( "config/party_config.ini" );
	
		kLoader.SetTitle_e( "info" );	
		m_iDefaultMaxPlayer = kLoader.LoadInt_e( "Default_MaxPlayer", MAX_PLAYER );
		m_iDefaultMaxObserver = kLoader.LoadInt_e( "Default_MaxObserver", MAX_OBSERVER );
		m_iLimitJoinLevel   = kLoader.LoadInt_e( "Limit_JoinLevel", 2 );
		
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		kLoader.LoadString_e( "BlueTeamEmoticon", "", szBuf, MAX_PATH );
		m_szBlueTeamEmoticon = szBuf;
		kLoader.LoadString_e( "RedTeamEmoticon", "", szBuf, MAX_PATH );
		m_szRedTeamEmoticon = szBuf;
	
		m_iBattleDefaultCatchModeLevel = kLoader.LoadInt_e( "default_catchmode_level", 10 );
	
		m_dwAutoStartAlarmCheckTime = kLoader.LoadInt_e( "AutoStartAlarmCheckTime", 5000 );
	
		kLoader.SetTitle_e( "sound" );	
		kLoader.LoadString_e( "mode_start_sound_def", "", szBuf, MAX_PATH );
		m_szStartSoundDef = szBuf;
		kLoader.LoadString_e( "mode_start_sound_go", "", szBuf, MAX_PATH );
		m_szStartSoundGo = szBuf;
		int i, iCheckSec;
		iCheckSec = kLoader.LoadInt_e( "check_sec", 0 );
		for(i = 0;i < iCheckSec;i++)
		{
			sprintf_e( szKey, "mode_start_sound_%dsec", i + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_StartSoundSec.push_back( szBuf );
		}
		// AI 맵에 따른 슬롯 갯수
		kLoader.SetTitle_e( "AISlot" );
		int aiMapCount = kLoader.LoadInt_e( "mapCount", 0 );
		for( int i = 0; i < aiMapCount; ++i )
		{
			sprintf_e( szKey, "mapindex%d", i + 1 );
			int iMaxSlot = kLoader.LoadInt( szKey, 2 );
			m_vecAIMaxSlot.push_back( iMaxSlot );
		}
	}

	// 선호모드 Save 기록 로드
	{
		char fName[MAX_PATH]="";
		sprintf( fName, "Save\\%s\\quick_start.ini", g_MyInfo.GetPublicID().c_str() );

		ioINILoader kLoader( fName, false );
		kLoader.SetTitle_e( "info" );

		m_QuickStartInfo.m_iQuickStartType = kLoader.LoadInt_e( "mode_type", BMT_ALL_MODE );
		for(int i = 0;i < BMT_MAX;i++)
		{
			char szKey[MAX_PATH] = "";
			sprintf_e( szKey, "sub%d_max_player", i );
			m_QuickStartInfo.m_iQuickStartMaxUser[i] = kLoader.LoadInt( szKey, g_ModeSelectInfo.GetSelectQuickStartModeDefMaxPlayer( i ) );

			sprintf_e( szKey, "sub%d_min_player", i );
			m_QuickStartInfo.m_iQuickStartMinUser[i] = kLoader.LoadInt( szKey, g_ModeSelectInfo.GetSelectQuickStartModeDefMinPlayer( i ) );

			sprintf_e( szKey, "sub%d_same_user", i );
			m_QuickStartInfo.m_bQuickStartSameUser[i] = kLoader.LoadBool( szKey, g_ModeSelectInfo.GetSelectQuickStartModeDefSameUser( i ) );
		}
	}
	SaveQuickStartInfo( m_QuickStartInfo );
}

void ioBattleRoomMgr::SaveQuickStartInfo( QuickStartInfo &rkQuickStartInfo )
{
	char fName[MAX_PATH]="";
	sprintf( fName, "Save\\%s\\quick_start.ini", g_MyInfo.GetPublicID().c_str() );
	FILE *fp = fopen( fName, "r" );
	if( !fp )
	{
		sprintf( fName, "Save\\%s" , g_MyInfo.GetPublicID().c_str() );
		CreateDirectory("Save",NULL);
		CreateDirectory(fName,NULL);
		sprintf(fName,"Save\\%s\\quick_start.ini", g_MyInfo.GetPublicID().c_str() );

		fp = fopen( fName, "w" );
		if( !fp )
		{
			LOG.PrintTimeAndLog( 0, "ioBattleRoomMgr::SaveQuickStartInfo File Create Failed." );
			return;
		}
	}
	fclose( fp );

	m_QuickStartInfo = rkQuickStartInfo;

	ioINILoader kLoader( fName, false );
	kLoader.SetTitle_e( "info" );
	kLoader.SaveInt( "mode_type", m_QuickStartInfo.m_iQuickStartType );
	for(int i = 0;i < BMT_MAX;i++)
	{
		if( g_ModeSelectInfo.IsNoneQuickStartMode( i ) ) continue;      // sp2_mode_select.ini에 있는 내용만 저장

 		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "sub%d_max_player", i );
		kLoader.SaveInt( szKey, m_QuickStartInfo.m_iQuickStartMaxUser[i] );

		sprintf_e( szKey, "sub%d_min_player", i );
		kLoader.SaveInt( szKey, m_QuickStartInfo.m_iQuickStartMinUser[i] );

		sprintf_e( szKey, "sub%d_same_user", i );
		kLoader.SaveBool( szKey, m_QuickStartInfo.m_bQuickStartSameUser[i] );
	}
}

QuickStartInfo ioBattleRoomMgr::GetQuickStartInfo()
{
	return m_QuickStartInfo;
}

int ioBattleRoomMgr::GetQuickStartModeType()
{
	if( m_QuickStartInfo.m_iQuickStartType == -1 )
	{
		m_QuickStartInfo.m_iQuickStartType = BMT_MAX;
		SaveQuickStartInfo( m_QuickStartInfo );
	}
	return m_QuickStartInfo.m_iQuickStartType;
}

int ioBattleRoomMgr::GetQuickStartMaxPlayer( int iModeIndex )
{
	if( COMPARE( iModeIndex, 0, BMT_MAX ) )
		return m_QuickStartInfo.m_iQuickStartMaxUser[iModeIndex];
	return MAX_PLAYER;
}

int ioBattleRoomMgr::GetQuickStartMinPlayer( int iModeIndex )
{
	if( COMPARE( iModeIndex, 0, BMT_MAX ) )
		return m_QuickStartInfo.m_iQuickStartMinUser[iModeIndex];
	return 0;
}

bool ioBattleRoomMgr::IsQuickStartSameUser( int iModeIndex )
{
	if( COMPARE( iModeIndex, 0, BMT_MAX ) )
		return m_QuickStartInfo.m_bQuickStartSameUser[iModeIndex];
	return false;
}

void ioBattleRoomMgr::SetRoomOptionDefault( bool bAlarm )
{
	m_BattleRoom.m_iPreSetMode = g_RoomOptionMgr.GetOptionTypeCnt(PartyOptionWnd::ID_PRESET_BTN) - 1;

	PreSetInfo kPreSet;
	if( g_RoomOptionMgr.GetDefaultInfo( kPreSet ) )
	{
		SetTeamAttackType( kPreSet.m_iTeamAttackType, bAlarm );
		SetChangeCharType( kPreSet.m_iChangeCharType, bAlarm );
		SetRedHPType( kPreSet.m_iRedHPType, bAlarm );
		SetBlueHPType( kPreSet.m_iBlueHPType, bAlarm );
		SetCoolTimeType( kPreSet.m_iCoolTimeType, bAlarm );
		SetDropDamageType( kPreSet.m_iDropDamageType, bAlarm );
		SetGravityType( kPreSet.m_iGravityType, bAlarm );
		SetGetUpType( kPreSet.m_iGetUpType, bAlarm );
		SetKOType( kPreSet.m_iKOType, bAlarm );
		SetKOEffectType( kPreSet.m_iKOEffectType, bAlarm );
		SetRedBlowType( kPreSet.m_iRedBlowType, bAlarm );
		SetBlueBlowType( kPreSet.m_iBlueBlowType, bAlarm );
		SetRedMoveSpeedType( kPreSet.m_iRedMoveSpeedType, bAlarm );
		SetBlueMoveSpeedType( kPreSet.m_iBlueMoveSpeedType, bAlarm );
		SetRedEquipType( kPreSet.m_iRedEquipType, bAlarm );
		SetBlueEquipType( kPreSet.m_iBlueEquipType, bAlarm );

		SetRoundType( kPreSet.m_iCatchModeRoundType, bAlarm );
		SetRoundTimeType( kPreSet.m_iCatchModeRoundTimeType, bAlarm );

		SetGrowthUseType( kPreSet.m_iGrowthUseType, bAlarm );
		SetExtraItemUseType( kPreSet.m_iExtraItemUseType, bAlarm );
	}
}

void ioBattleRoomMgr::SetFirstPreSetMode()
{
	if( g_RoomOptionMgr.GetOptionTypeCnt(PartyOptionWnd::ID_PRESET_BTN) == 0 )
		return;

	m_BattleRoom.m_iPreSetMode = 0;

	PreSetInfo kPreSet;

	if( g_RoomOptionMgr.GetPreSetInfo( m_BattleRoom.m_iPreSetMode, kPreSet ) )
	{
		SetTeamAttackType( kPreSet.m_iTeamAttackType, false );

		SetChangeCharType( kPreSet.m_iChangeCharType, false );
		SetRedHPType( kPreSet.m_iRedHPType, false );
		SetBlueHPType( kPreSet.m_iBlueHPType, false );

		SetCoolTimeType( kPreSet.m_iCoolTimeType, false );
		SetDropDamageType( kPreSet.m_iDropDamageType, false );
		SetGravityType( kPreSet.m_iGravityType, false );
		SetGetUpType( kPreSet.m_iGetUpType, false );
		SetKOType( kPreSet.m_iKOType, false );
		SetKOEffectType( kPreSet.m_iKOEffectType, false );
		SetRedBlowType( kPreSet.m_iRedBlowType, false );
		SetBlueBlowType( kPreSet.m_iBlueBlowType, false );
		SetRedMoveSpeedType( kPreSet.m_iRedMoveSpeedType, false );
		SetBlueMoveSpeedType( kPreSet.m_iBlueMoveSpeedType, false );
		SetRedEquipType( kPreSet.m_iRedEquipType, false );
		SetBlueEquipType( kPreSet.m_iBlueEquipType, false );

		SetRoundType( kPreSet.m_iCatchModeRoundType, false );
		SetRoundTimeType( kPreSet.m_iCatchModeRoundTimeType, false );

		SetGrowthUseType( kPreSet.m_iGrowthUseType, false );
		SetExtraItemUseType( kPreSet.m_iExtraItemUseType, false );
	}
}

void ioBattleRoomMgr::UserListClear()
{
	m_vUserList.clear();
}

void ioBattleRoomMgr::SetKickOutRoom( DWORD dwIndex )
{
	KickOutRoom kor;
	kor.m_dwIndex        = dwIndex;
	kor.m_dwKickOutTimer = FRAMEGETTIME();
	m_vKickOutRoom.push_back( kor );
}

bool ioBattleRoomMgr::IsKickOutRoom( DWORD dwIndex )
{
	int iSize = m_vKickOutRoom.size();
	if( iSize == 0 )
		return false;

	for(int i = 0;i < iSize;i++)
	{
		KickOutRoom &kor = m_vKickOutRoom[i];
		if( kor.m_dwIndex == dwIndex )
		{
			if( FRAMEGETTIME() - kor.m_dwKickOutTimer > 180000 )
			{
				m_vKickOutRoom.erase( m_vKickOutRoom.begin() + i );
				return false;
			}
			else
				return true;
		}
	}
	return false;
}

void ioBattleRoomMgr::SetInvitedRefuse( DWORD dwIndex )
{
	InvitedRefuse kRefuse;
	kRefuse.m_dwIndex    	= dwIndex;
	kRefuse.m_dwRefuseTimer = FRAMEGETTIME();
	m_vInvitedRefuse.push_back( kRefuse );
}

bool ioBattleRoomMgr::IsInvitedRefuseRoom( DWORD dwIndex )
{
	int iSize = m_vInvitedRefuse.size();
	if( iSize == 0 ) return false;

	for(int i = 0;i < iSize;i++)
	{
		InvitedRefuse &kRefuse = m_vInvitedRefuse[i];
		if( kRefuse.m_dwIndex == dwIndex )
		{
			if( FRAMEGETTIME() - kRefuse.m_dwRefuseTimer > 60000 )
			{
				m_vInvitedRefuse.erase( m_vInvitedRefuse.begin() + i );
				return false;
			}
			else
				return true;
		}
	}
	return false;
}

void ioBattleRoomMgr::SetKickOutUser( ioHashString szName )
{
	m_szKickOutUser = szName;
	int iTeam = GetBattleUserTeam( szName );

	ioComplexStringPrinter kPrinter;
	g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );		

	if( iTeam == TEAM_RED )
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szName.c_str() );
	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );
	g_ChatMgr.SetChatComplexString( STR(1), kPrinter );
}

void ioBattleRoomMgr::LeaveRoom( bool bMove /* = false */ )
{
	InitBattleCancelCount();
	if( m_BattleRoom.m_dwIndex != 0 )
	{
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );	
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );
		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_bBattleModePlaying   = true;
	m_BattleRoom.m_dwIndex = 0;
	m_BattleRoom.m_dwTourIndex	 = 0;
	m_BattleRoom.m_dwBlueTeamIndex= 0;
	m_BattleRoom.m_dwRedTeamIndex = 0;
	m_BattleRoom.m_dwTourStartTimer = 0;
	m_BattleRoom.m_bTourStartSender = false;
	m_BattleRoom.m_bRandomTeamMode		= true;
	m_BattleRoom.m_bStartRoomEnterX		= false;
	m_BattleRoom.m_bAutoModeStart       = false;
	m_BattleRoom.m_bBadPingKick         = true;
	
	m_BattleRoom.m_bUseExtraOption = false;
	m_BattleRoom.m_bNoChallenger = false;

	SetFirstPreSetMode();

	m_BattleRoom.m_iBattleEventType     = BET_NORMAL;
	m_iSelectModeIndex  = -1;
	m_iSelectMapIndex	= -1;
	m_iPreSelectModeIndex  = -1;
	m_iPreSelectMapIndex	= -1;
	m_bSafetyLevelRoom  = false;
	m_bChangeMeHostAlarm= false;
	m_szPrevMyOwnerID.Clear();
	InitRecord();
	UserListClear();
	g_FriendManager.SetUpdate();

	if( !bMove )
	{
		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
		{
			if( ioMyRoomMode::GetMyRoomType() == MRT_BATTLE )
			{
				ioMyRoomMode::SetMyRoomType( MRT_LOBBY );
				if( ioPlayMode::GetModeType() == MT_MYROOM )
				{
					g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
					g_App.SetModeType( 0, 0, 0, PT_NONE );
				}		
			}
			else if( ioMyRoomMode::GetMyRoomType() != MRT_RANKBATTLE )
				ioMyRoomMode::SetMyRoomType( MRT_LOBBY );
		}
		else
		{
			if( ioPlayMode::GetModeType() == MT_MYROOM )
			{
				g_App.SendSearchModeJoin();
			}
		}
	}
}

void ioBattleRoomMgr::InitRecord()
{
	m_BattleRoom.m_iBlueWin = 0;
	m_BattleRoom.m_iBlueLose = 0;
	m_BattleRoom.m_iBlueVictories = 0;
	m_BattleRoom.m_iRedWin = 0;
	m_BattleRoom.m_iRedLose = 0;
	m_BattleRoom.m_iRedVictories = 0;
}

bool ioBattleRoomMgr::IsBattleRoom()
{
	if( m_BattleRoom.m_dwIndex == 0 )
		return false;

	return true;
}

void ioBattleRoomMgr::ChangeMyOwner()
{
	if( !m_szPrevMyOwnerID.IsEmpty() && !m_BattleRoom.m_szOwnerName.IsEmpty() )
	{
		if( m_szPrevMyOwnerID != m_BattleRoom.m_szOwnerName )
		{

			TournamentNode* pNode =  g_TournamentMgr.GetTournament( GetTournamentIndex(), false );
			if( pNode && pNode->IsLogSend() && IsTournamentModeType() )
			{
				char szLog[MAX_PATH];
				sprintf( szLog, "[대회로그] 방장이 바뀜 - %s -> %s", m_szPrevMyOwnerID.c_str(), m_BattleRoom.m_szOwnerName.c_str() );

				SP2Packet kPacket2( LUPK_LOG );
				kPacket2 << "TournamentLog";  // 로그 파일 타입
				kPacket2 << szLog;
				kPacket2 << 4000;  // 오류번호:4000
				kPacket2 << false; // write db
				P2PNetwork::SendLog(kPacket2);				

				if( g_App.IsMeDeveloper() )
					LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
			}

			// 방장이 변경되었다.		
			if( IsBattleModePlaying() )
			{
				g_ChatMgr.SetHostChange( STR(1), m_BattleRoom.m_szOwnerName.c_str(), (TeamType)GetBattleUserTeam( m_BattleRoom.m_szOwnerName ) );
			}
			else
			{
				g_ChatMgr.SetHostChange( STR(2), m_BattleRoom.m_szOwnerName.c_str(), TEAM_NONE );
			}

			if( m_BattleRoom.m_szOwnerName == g_MyInfo.GetPublicID() )
			{
				m_bChangeMeHostAlarm = true; 
				if( !IsBattleModePlaying() )
				{
					g_GUIMgr.ShowWnd( PARTY_SHAM_BATTLE_WND );
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
					m_bChangeMeHostAlarm = false; 
				}
			}							
		}
	}
	m_szPrevMyOwnerID = m_BattleRoom.m_szOwnerName;
}

bool ioBattleRoomMgr::ChangeMeHostAlarm()
{
	bool bReturn = m_bChangeMeHostAlarm;
	m_bChangeMeHostAlarm = false;
	if( !IsBattleRoom() )
		return false;
	return bReturn;
}

void ioBattleRoomMgr::UpdateUserLevel( const ioHashString &szUserID, int iGradeLevel )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			m_vUserList[i].m_iGradeLevel = iGradeLevel;

			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
				pShamBattle->UpdateShamBattle();
			return;
		}
	}
}

void ioBattleRoomMgr::UpdateUserAbilityLevel( const ioHashString &szUserID, int iAbilityLevel )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			m_vUserList[i].m_iAbilityLevel = iAbilityLevel;
			return;
		}
	}
}

void ioBattleRoomMgr::UpdateUserSafetyLevel( const ioHashString &szUserID, bool bSafetyLevel )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			m_vUserList[i].m_bSafetyLevel = bSafetyLevel;
			return;
		}
	}
}

void ioBattleRoomMgr::UpdateUserMonsterCoin( const ioHashString &szUserID, int iMonsterCoin, int iGoldMonsterCoin, int iRaidCoin )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			m_vUserList[i].m_iMonsterCoin = iMonsterCoin;
			m_vUserList[i].m_iGoldMonsterCoin = iGoldMonsterCoin;
			m_vUserList[i].m_iRaidCoin = iRaidCoin;
			m_vUserList[i].m_dwMonsterCoinFirstSyncTime = 0;

			//
			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
				pShamBattle->UpdateShamBattle();
			return;
		}
	}
}

void ioBattleRoomMgr::UpdateUserCharIndex( const ioHashString &szUserID, DWORD dwCharIndex, bool bSync )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		BattleRoomUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == szUserID )
		{
			rkUserData.m_dwCharIndex = dwCharIndex;
			if( bSync )
			{
				rkUserData.m_dwCharIndexFirstSyncTime = 0;
			}

			//
			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
				pShamBattle->UpdateShamBattle();
			return;
		}
	}
}

void ioBattleRoomMgr::UpdateUserCharMotion( const ioHashString &szUserID, DWORD dwCharMotion )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		BattleRoomUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == szUserID )
		{
			rkUserData.m_dwCharMotionItem = dwCharMotion;
			rkUserData.m_dwCharMotionFirstSyncTime = 0;
			//
			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
				pShamBattle->UpdateShamBattle();
			return;
		}
	}
}

void ioBattleRoomMgr::JoinUser( BattleRoomUserInfo &kUser )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		BattleRoomUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == kUser.m_szUserID )
		{
			rkUserData.m_dwUserIndex    = kUser.m_dwUserIndex;
			rkUserData.m_iGradeLevel	= kUser.m_iGradeLevel;
			rkUserData.m_iAbilityLevel  = kUser.m_iAbilityLevel;
			rkUserData.m_bSafetyLevel   = kUser.m_bSafetyLevel;
			rkUserData.m_bObserver		= kUser.m_bObserver;
			rkUserData.m_iTeam			= kUser.m_iTeam;		
			rkUserData.m_szPublicIP     = kUser.m_szPublicIP;
			rkUserData.m_szPrivateIP    = kUser.m_szPrivateIP;
			rkUserData.m_iPort          = kUser.m_iPort;	
			rkUserData.m_szTransferIP   = kUser.m_szTransferIP;
			rkUserData.m_iTransferPort  = kUser.m_iTransferPort;
			rkUserData.m_iBattleModeOrder  = kUser.m_iBattleModeOrder;

			// 자기 자신의 몬스터 코인 & 캐릭터 인덱스 세팅
			if( rkUserData.m_szUserID == g_MyInfo.GetPublicID() )
			{
				rkUserData.m_iMonsterCoin		= g_MyInfo.GetEtcMonsterCoin();
				rkUserData.m_iGoldMonsterCoin	= g_MyInfo.GetEtcGoldMonsterCoin();	
				rkUserData.m_iRaidCoin			= g_MyInfo.GetEtcRaidCoin();	
				rkUserData.m_dwCharMotionItem	= g_MyInfo.GetEtcMotionAni( ioEtcItemMotion::MOTION_OPTION_5 );
				rkUserData.m_dwCharIndex		= g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
				if( rkUserData.m_dwCharIndex == 0 )
					rkUserData.m_dwCharIndex = g_MyInfo.GetCharIndex( 0 );
			}
			return;
		}
	}

	if( kUser.m_szUserID == g_MyInfo.GetPublicID() )
	{
		kUser.m_iMonsterCoin = g_MyInfo.GetEtcMonsterCoin();
		kUser.m_iGoldMonsterCoin = g_MyInfo.GetEtcGoldMonsterCoin();
		kUser.m_iRaidCoin			= g_MyInfo.GetEtcRaidCoin();
		kUser.m_dwMonsterCoinFirstSyncTime = 0;

		// 방장에게 즉시 서버 중계로 코인 동기화
		if( GetOwnerName() != g_MyInfo.GetPublicID() )
		{
			SP2Packet kPacket( CUPK_MONSTERCOIN_SYNC );
			kPacket << g_MyInfo.GetPublicID() << g_MyInfo.GetEtcMonsterCoin() << g_MyInfo.GetEtcGoldMonsterCoin() << g_MyInfo.GetEtcRaidCoin();
			P2PNetwork::SendToRelayPacket( GetOwnerName(), kPacket );
		}
		//
		kUser.m_dwCharMotionFirstSyncTime = 0;
		kUser.m_dwCharMotionItem	= g_MyInfo.GetEtcMotionAni( ioEtcItemMotion::MOTION_OPTION_5 );

		kUser.m_dwCharIndex  = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
		if( kUser.m_dwCharIndex == 0 )
			kUser.m_dwCharIndex = g_MyInfo.GetCharIndex( 0 );
	}
	else
	{
		kUser.m_dwCharMotionFirstSyncTime = FRAMEGETTIME();
		kUser.m_dwMonsterCoinFirstSyncTime = FRAMEGETTIME();
		kUser.m_dwCharIndexFirstSyncTime = FRAMEGETTIME();
	}

	m_vUserList.push_back( kUser );
	SetCurJoiner( m_vUserList.size() );
	
	if( kUser.m_szUserID == g_MyInfo.GetPublicID() )
	{
		g_MyInfo.SetMyAddress( kUser.m_szPublicIP, kUser.m_iPort );

		// 관전이면 음성채팅 기능 차단
		if( Setting::UseVoiceChat() && g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		{
			SettingOption kNewOption = Setting::GetSettingOption();
			kNewOption.m_bUseVoiceChat = false;

			g_App.ApplyOptionChange( kNewOption );
		}		
	}
	else if( kUser.m_szUserID != g_MyInfo.GetPublicID() )
	{
		g_App.AddNetworkTable( kUser.m_dwUserIndex, kUser.m_szUserID, kUser.m_szPublicIP, kUser.m_szPrivateIP, kUser.m_szTransferIP,
							   kUser.m_iPort, kUser.m_iTransferPort, P2P_STYLE_VOICE, 3 );

		// 친구 정보 갱신
		g_FriendManager.ClientFriendInfoUpdate( kUser.m_szUserID, kUser.m_iGradeLevel, UP_BATTLE_ROOM );

		if( Setting::UseVoiceChat() && Setting::UseMIC() )
		{
			SP2Packet kPacket( CTPK_VOICE_INFO );
			kPacket << ID_VOICE_ON;
			kPacket << kUser.m_szUserID;
			TCPNetwork::SendToServer( kPacket );
#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO | ID_VOICE_ON | %s", __FUNCTION__, kUser.m_szUserID.c_str() );
#endif
		}
	}

	if( (int)m_vUserList.size() < GetMaxPlayer() )
		return;

	if( kUser.m_szUserID != g_MyInfo.GetPublicID() && !kUser.m_bObserver )
	{
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), kUser.m_szUserID.c_str() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2));
		g_ChatMgr.SetChatComplexString( STR(3), kPrinter );
	}
}

void ioBattleRoomMgr::LeaveUser( ioHashString szUserID, ModeType eModeType )
{
	bool bObserver = false;
	int iTeam = TEAM_NONE;
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			iTeam = m_vUserList[i].m_iTeam;
			bObserver = m_vUserList[i].m_bObserver;
			m_vUserList.erase( m_vUserList.begin() + i );
			break;
		}
	}

	if( szUserID != g_MyInfo.GetPublicID() )
	{
		g_App.RemoveNetworkTable( szUserID , P2P_STYLE_VOICE );
		g_VoiceChat.RemoveSoundPlayer( szUserID.c_str() );
		g_VoiceOffListManager.Delete( szUserID );
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | Delete voice | %s ", __FUNCTION__, szUserID.c_str()  );
#endif
		VoiceChatWnd *pVoice = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if( pVoice )
			pVoice->SetVoiceOff( szUserID );
	}
	
	// 개인본부/길드본부/훈련소/광장에서만 표시
	if( eModeType == MT_MYROOM || eModeType == MT_TRAINING || eModeType == MT_HEADQUARTERS || eModeType == MT_HOUSE )
	{
		if( GetKickOutUser() != szUserID && !bObserver )
		{
			if( szUserID != g_MyInfo.GetPublicID() )
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szUserID.c_str() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );
				g_ChatMgr.SetChatComplexString( STR(3), kPrinter );
			}
		}	
	}
}

bool ioBattleRoomMgr::IsHaveUser( ioHashString &rszUserID )
{
	if( !IsBattleRoom() ) return false;

	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == rszUserID )
			return true;
	}
	return false;
}

const BattleRoomUserInfo &ioBattleRoomMgr::GetUserInfo( int iArray )
{
	int iCharCnt = m_vUserList.size();
	if( COMPARE( iArray, 0, iCharCnt ) )
		return m_vUserList[iArray];
	static BattleRoomUserInfo sgTemp;
	return sgTemp;
}

void ioBattleRoomMgr::SetBattleTeam( ioHashString szName, TeamType eTeam, bool bBattleModeSubSlot )
{
	// 블루팀 / 레드팀 둘중하나여야한다.
	if( eTeam != TEAM_BLUE && eTeam != TEAM_RED && eTeam != TEAM_NONE )
		return;

	int iCharCnt = m_vUserList.size();
	int i = 0;
	if(bBattleModeSubSlot)
		i = iCharCnt - 1;
	for( ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szName )
		{
			m_vUserList[i].m_iTeam = eTeam;

			if( eTeam == TEAM_NONE && !m_vUserList[i].m_bObserver )
			{
				m_vUserList[i].m_bObserver = true;

				// 음성채팅 기능 차단
				SettingOption kNewOption = Setting::GetSettingOption();
				kNewOption.m_bUseVoiceChat = false;

				g_App.ApplyOptionChange( kNewOption );
			}
			else if( eTeam != TEAM_NONE && m_vUserList[i].m_bObserver )
			{
				m_vUserList[i].m_bObserver = false;
			}

			break;
		}
	}
}

int ioBattleRoomMgr::GetBattleUserTeam( ioHashString szName )
{
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szName )
		{
			return m_vUserList[i].m_iTeam;
		}
	}

	return TEAM_NONE;
}

int ioBattleRoomMgr::GetBattleTeamAverageLevel( TeamType eTeam )
{
	int iAverageLevel = 0;
	int iTeamCount    = 0;
	int iCharCnt = m_vUserList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		if( g_App.IsAdminID( m_vUserList[i].m_szUserID.c_str() ) ) continue;
		if( m_vUserList[i].m_bObserver ) continue;

		if( m_vUserList[i].m_iTeam == eTeam )
		{
			iAverageLevel += m_vUserList[i].m_iAbilityLevel;
			iTeamCount++;
		}
	}
	if( iTeamCount == 0 )
		return 0;
	iAverageLevel /= iTeamCount;
	iAverageLevel = min( max( iAverageLevel - g_LevelMgr.GetAddGradeLevel(), 0 ), g_LevelMgr.GetMaxGradeLevel() );
	return iAverageLevel;
}

int ioBattleRoomMgr::GetAverageLevel()
{
	int iSize = 0;
	int iLevel = 0;
	int iObserverSize = 0;
	int iObserverLevel = 0;

	int iCharCnt = m_vUserList.size();
	int i = 0;
	for( i=0; i < iCharCnt; i++ )
	{
		if( g_App.IsAdminID( m_vUserList[i].m_szUserID.c_str() ) ) continue;

		// 옵저버 제외 레벨
		if( !m_vUserList[i].m_bObserver )
		{
			iLevel += m_vUserList[i].m_iAbilityLevel;
			iSize++;
		}
	}

	for( i=0; i < iCharCnt; i++ )
	{
		if( g_App.IsAdminID( m_vUserList[i].m_szUserID.c_str() ) ) continue;

		// 옵저버 레벨
		if( m_vUserList[i].m_bObserver )
		{
			iObserverLevel += m_vUserList[i].m_iAbilityLevel;
			iObserverSize++;
		}
	}

	if( iSize <= 0 && iObserverSize > 0 )
	{
		iObserverLevel /= iObserverSize;
		iObserverLevel = min( max( iObserverLevel - g_LevelMgr.GetAddGradeLevel(), 0 ), g_LevelMgr.GetMaxGradeLevel() );
		return iObserverLevel;
	}
	else if( iSize <= 0 && iObserverSize <= 0 )
	{
		return 0;
	}

	iLevel /= iSize;
	iLevel = min( max( iLevel - g_LevelMgr.GetAddGradeLevel(), 0 ), g_LevelMgr.GetMaxGradeLevel() );
	return iLevel;
}

const BattleRoomUserInfo &ioBattleRoomMgr::GetBattleUserInfo( TeamType eTeam, int iArray )
{
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_iTeam == eTeam )
		{
			iArray--;
			if( iArray < 0 )
				return m_vUserList[i];
		}		
	}

	static BattleRoomUserInfo sgTemp;
	return sgTemp;
}

const BattleRoomUserInfo &ioBattleRoomMgr::GetBattleUserInfo( const ioHashString& rszUserID )
{
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == rszUserID )
			return m_vUserList[i];
	}

	static BattleRoomUserInfo sgTemp;
	return sgTemp;
}

const BattleRoomUserInfo &ioBattleRoomMgr::GetBattleObserverInfo( int iArray )
{
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_bObserver )
		{
			iArray--;
			if( iArray < 0 )
				return m_vUserList[i];
		}		
	}

	static BattleRoomUserInfo sgTemp;
	return sgTemp;
}

int ioBattleRoomMgr::GetBattleTeamUserCount( TeamType eTeam )
{
	int iCount = 0;
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( eTeam == TEAM_NONE && m_vUserList[i].m_bObserver )
		{
			iCount++;
		}
		else if( eTeam != TEAM_NONE && m_vUserList[i].m_iTeam == eTeam )
		{
			iCount++;
		}		
	}

	return iCount;
}

void ioBattleRoomMgr::SetTournamentData( DWORD dwTourIndex, DWORD dwBlueTeamIndex, DWORD dwRedTeamIndex, DWORD dwStartTimer )
{
	if( IsBattleRoom() )
	{
		m_BattleRoom.m_dwTourIndex     = dwTourIndex;
		m_BattleRoom.m_dwBlueTeamIndex = dwBlueTeamIndex;
		m_BattleRoom.m_dwRedTeamIndex  = dwRedTeamIndex;
		m_BattleRoom.m_dwTourStartTimer= dwStartTimer;
		m_BattleRoom.m_bTourStartSender= false;
	}
}

void ioBattleRoomMgr::SetTournamentDeleteTeam( DWORD dwDeleteTeam, DWORD dwStartTimer )
{
	if( IsBattleRoom() )
	{
		if( m_BattleRoom.m_dwBlueTeamIndex == dwDeleteTeam )
		{
			m_BattleRoom.m_dwBlueTeamIndex = 0;
		}
		else if( m_BattleRoom.m_dwRedTeamIndex == dwDeleteTeam )
		{
			m_BattleRoom.m_dwRedTeamIndex = 0;
		}

		m_BattleRoom.m_dwTourStartTimer = dwStartTimer;
		m_BattleRoom.m_bTourStartSender= false;
	}
}

void ioBattleRoomMgr::SetTournamentNewTeam( DWORD dwNewTeam )
{
	if( IsBattleRoom() )
	{
		if( m_BattleRoom.m_dwBlueTeamIndex == 0 )
		{
			m_BattleRoom.m_dwBlueTeamIndex = dwNewTeam;
		}
		else if( m_BattleRoom.m_dwRedTeamIndex == 0 )
		{
			m_BattleRoom.m_dwRedTeamIndex = dwNewTeam;
		}
	}
}

bool ioBattleRoomMgr::ApplyBattleRoomInfo( SP2Packet &rkPacket, bool &rBattleRoom )
{
	DWORD dwNewBattleIndex;
	rkPacket >> dwNewBattleIndex;
	if( IsBattleRoom() )
	{
		if( m_BattleRoom.m_dwIndex != dwNewBattleIndex )
		{
			LeaveRoom( true );
		}
	}	

	DWORD dwBackupIndex    = m_BattleRoom.m_dwIndex;
	m_BattleRoom.m_dwIndex = dwNewBattleIndex;
	rkPacket >> m_BattleRoom.m_szName >> m_BattleRoom.m_szOwnerName >> m_BattleRoom.m_iCurJoiner >> m_BattleRoom.m_iCurPlayer
			 >> m_BattleRoom.m_iMaxPlayerBlue >> m_BattleRoom.m_iMaxPlayerRed >> m_BattleRoom.m_iMaxObserver >> m_BattleRoom.m_szPassword >> m_BattleRoom.m_iAverageLevel
			 >> m_BattleRoom.m_bUseExtraOption >> m_BattleRoom.m_bNoChallenger >> m_BattleRoom.m_iChangeCharType
			 >> m_BattleRoom.m_iCoolTimeType >> m_BattleRoom.m_iRedHPType >> m_BattleRoom.m_iBlueHPType
			 >> m_BattleRoom.m_iDropDamageType >> m_BattleRoom.m_iGravityType
			 >> m_BattleRoom.m_iPreSetMode >> m_BattleRoom.m_iTeamAttackType >> m_BattleRoom.m_iGetUpType
			 >> m_BattleRoom.m_iKOType >> m_BattleRoom.m_iRedBlowType >> m_BattleRoom.m_iBlueBlowType
			 >> m_BattleRoom.m_iRedMoveSpeedType >> m_BattleRoom.m_iBlueMoveSpeedType >> m_BattleRoom.m_iKOEffectType
			 >> m_BattleRoom.m_iRedEquipType >> m_BattleRoom.m_iBlueEquipType
			 >> m_BattleRoom.m_iCatchModeRoundType >> m_BattleRoom.m_iCatchModeRoundTimeType
			 >> m_BattleRoom.m_iGrowthUseType >> m_BattleRoom.m_iExtraItemUseType
			 >> rBattleRoom;

	bool bFirstEnter = false;
	if( m_BattleRoom.m_dwIndex != dwBackupIndex )
	{
		g_FriendManager.SetUpdate();
		bFirstEnter = true;
	}
	ChangeMyOwner();

	//전투방 입장하면 길드마크 변경 웹페이지 닫는다.
	g_App.CloseGuildMarkPage();

	TournamentNode* pNode = g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
	if( pNode && pNode->IsLogSend() && IsTournamentModeType() )
	{
		char szLog[MAX_PATH];
		sprintf( szLog, "[대회로그] 대회 배틀룸 입장(자신) - %s", g_MyInfo.GetPublicID().c_str() );

		SP2Packet kPacket2( LUPK_LOG );
		kPacket2 << "TournamentLog";  // 로그 파일 타입
		kPacket2 << szLog;
		kPacket2 << 4000;  // 오류번호:4000
		kPacket2 << false; // write db
		P2PNetwork::SendLog(kPacket2);

		if( g_App.IsMeDeveloper() )
			LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
	}

	return bFirstEnter;
}

ioHashString ioBattleRoomMgr::ApplyJoinUserInfo( SP2Packet &rkPacket )
{
	DWORD dwGuildIndex, dwGuildMark;
	BattleRoomUserInfo kInfo;
	rkPacket >> m_BattleRoom.m_iAverageLevel >> kInfo.m_szUserID >> kInfo.m_iGradeLevel >> kInfo.m_iAbilityLevel >> kInfo.m_bSafetyLevel >> kInfo.m_bObserver >> kInfo.m_iTeam;
	rkPacket >> kInfo.m_dwUserIndex >> kInfo.m_szPublicIP >> kInfo.m_iPort >> kInfo.m_szPrivateIP >> kInfo.m_szTransferIP >> kInfo.m_iTransferPort;
	rkPacket >> kInfo.m_iBattleModeOrder;
	rkPacket >> dwGuildIndex >> dwGuildMark;
	JoinUser( kInfo );
	
	if( IsTournamentModeType() )
	{
		PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
		if( pShamBattle && pShamBattle->IsShow() )
		{
			if( pShamBattle->GetSearchEntryTeamType() == kInfo.m_iTeam )
				pShamBattle->SetEntrySerchShow( false );
		}
	}

	g_GuildInfo.SetGuildMark( kInfo.m_szUserID, dwGuildIndex, dwGuildMark );

	TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
	if( pNode && pNode->IsLogSend() && IsTournamentModeType() )
	{
		char szLog[MAX_PATH];
		sprintf( szLog, "[대회로그] 대회 배틀룸 입장(다른유저) - %s", kInfo.m_szUserID );

		SP2Packet kPacket2( LUPK_LOG );
		kPacket2 << "TournamentLog";  // 로그 파일 타입
		kPacket2 << szLog;
		kPacket2 << 4000;  // 오류번호:4000
		kPacket2 << false; // write db
		P2PNetwork::SendLog(kPacket2);

		if( g_App.IsMeDeveloper() )
			LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
	}

	return kInfo.m_szUserID;
}

ioHashString ioBattleRoomMgr::ApplyLeaveUserInfo( SP2Packet &rkPacket, ModeType eModeType )
{
	ioHashString szUserID;
	rkPacket >> szUserID >> m_BattleRoom.m_iAverageLevel;
	LeaveUser( szUserID, eModeType );

	SetCurJoiner( m_vUserList.size() );
	return szUserID;
}

void ioBattleRoomMgr::ApplyBattleRoomRecord( SP2Packet &rkPacket )
{
	if( !IsBattleRoom() ) return;

	rkPacket >> m_BattleRoom.m_iBlueWin >> m_BattleRoom.m_iBlueLose >> m_BattleRoom.m_iBlueVictories;
	rkPacket >> m_BattleRoom.m_iRedWin >> m_BattleRoom.m_iRedLose >> m_BattleRoom.m_iRedVictories;
}

void ioBattleRoomMgr::ApplyBattleRoomOption( SP2Packet &rkPacket, bool bAlarm )
{
	if( !IsBattleRoom() ) return;

	if( bAlarm )
	{
		bool bRandomTeamMode, bStartRoomEnterX, bAutoModeStart, bBadPingKick;
		bool bUseExtraOption, bNoChallenger;
		int iChangeCharType, iTeamAttackType, iGetUpType;
		int iCoolTimeType, iRedHPType, iBlueHPType, iDropDamageType;
		int iGravityType, iKOType, iKOEffectType;
		int iRedBlowType, iBlueBlowType;
		int iRedMoveSpeedType, iBlueMoveSpeedType;
		int iRedEquipType, iBlueEquipType;
		int iRoundType, iRoundTimeType;
		int iGrowthUseType, iExtraItemUseType;
		int iPreSetModeType;

		rkPacket >> bRandomTeamMode >> bStartRoomEnterX >> bAutoModeStart >> bBadPingKick
				 >> bUseExtraOption >> bNoChallenger >> iChangeCharType
				 >> iCoolTimeType >> iRedHPType >> iBlueHPType
				 >> iDropDamageType >> iGravityType
				 >> iPreSetModeType >> iTeamAttackType >> iGetUpType
				 >> iKOType >> iRedBlowType >> iBlueBlowType
				 >> iRedMoveSpeedType >> iBlueMoveSpeedType >> iKOEffectType
				 >> iRedEquipType >> iBlueEquipType
				 >> iRoundType >> iRoundTimeType
				 >> iGrowthUseType >> iExtraItemUseType;

		SetRandomTeamMode( bRandomTeamMode );
		SetStartRoomEnterX( bStartRoomEnterX );
		SetAutoModeStart( bAutoModeStart );
		SetBadPingKick( bBadPingKick );

		SetUseExtraOption( bUseExtraOption, bAlarm );
		SetNoChallenger( bNoChallenger );
		SetChangeCharType( iChangeCharType, bAlarm );
		SetTeamAttackType( iTeamAttackType, bAlarm );

		SetCoolTimeType( iCoolTimeType, bAlarm );
		SetRedHPType( iRedHPType, bAlarm );
		SetBlueHPType( iBlueHPType, bAlarm );
		SetDropDamageType( iDropDamageType, bAlarm );
		SetGravityType( iGravityType, bAlarm );
		SetGetUpType( iGetUpType, bAlarm );
		SetKOType( iKOType, bAlarm );
		SetKOEffectType( iKOEffectType, bAlarm );
		SetRedBlowType( iRedBlowType, bAlarm );
		SetBlueBlowType( iBlueBlowType, bAlarm );
		SetRedMoveSpeedType( iRedMoveSpeedType, bAlarm );
		SetBlueMoveSpeedType( iBlueMoveSpeedType, bAlarm );
		SetRedEquipType( iRedEquipType, bAlarm );
		SetBlueEquipType( iBlueEquipType, bAlarm );

		SetRoundType( iRoundType, bAlarm );
		SetRoundTimeType( iRoundTimeType, bAlarm );

		SetGrowthUseType( iGrowthUseType, bAlarm );
		SetExtraItemUseType( iExtraItemUseType, bAlarm );

		SetPreSetModeType( iPreSetModeType, false );
	}
	else
	{
		rkPacket >> m_BattleRoom.m_bRandomTeamMode >> m_BattleRoom.m_bStartRoomEnterX >> m_BattleRoom.m_bAutoModeStart >> m_BattleRoom.m_bBadPingKick 
				 >> m_BattleRoom.m_bUseExtraOption >> m_BattleRoom.m_bNoChallenger >> m_BattleRoom.m_iChangeCharType
				 >> m_BattleRoom.m_iCoolTimeType >> m_BattleRoom.m_iRedHPType >> m_BattleRoom.m_iBlueHPType
				 >> m_BattleRoom.m_iDropDamageType >> m_BattleRoom.m_iGravityType
				 >> m_BattleRoom.m_iPreSetMode >> m_BattleRoom.m_iTeamAttackType >> m_BattleRoom.m_iGetUpType
				 >> m_BattleRoom.m_iKOType >> m_BattleRoom.m_iRedBlowType >> m_BattleRoom.m_iBlueBlowType
				 >> m_BattleRoom.m_iRedMoveSpeedType >> m_BattleRoom.m_iBlueMoveSpeedType >> m_BattleRoom.m_iKOEffectType
				 >> m_BattleRoom.m_iRedEquipType >> m_BattleRoom.m_iBlueEquipType
				 >> m_BattleRoom.m_iCatchModeRoundType >> m_BattleRoom.m_iCatchModeRoundTimeType
				 >> m_BattleRoom.m_iGrowthUseType >> m_BattleRoom.m_iExtraItemUseType;
	}
}

void ioBattleRoomMgr::ApplyBattleRoomEventType( SP2Packet &rkPacket )
{
	if( !IsBattleRoom() ) return;

	rkPacket >> m_BattleRoom.m_iBattleEventType; 
}

void ioBattleRoomMgr::SetMaxPlayer( int iBlueCount, int iRedCount, int iObserver )
{ 
	// 블루팀 인원 변경
	if( iBlueCount != m_BattleRoom.m_iMaxPlayerBlue )
	{
		//알림
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4));

		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(6), iBlueCount );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5));
		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	// 레드팀인원 변경
	if( iRedCount != m_BattleRoom.m_iMaxPlayerRed )
	{
		//알림
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4));

		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(7), iRedCount );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5));
		g_ChatMgr.SetChatComplexString( STR(8), kPrinter );
	}

	if( m_BattleRoom.m_szOwnerName == g_MyInfo.GetPublicID() )
	{
		int iPrevMaxPlayer = GetMaxPlayer();
		if( iPrevMaxPlayer >= Help::GetPlayingUserBonusCount() && iBlueCount + iRedCount < Help::GetPlayingUserBonusCount() )
			SetPlayerCountChangeAlarm( false );
		else if( iPrevMaxPlayer < Help::GetPlayingUserBonusCount() && iBlueCount + iRedCount >= Help::GetPlayingUserBonusCount() )
			SetPlayerCountChangeAlarm( true );
	}
	m_BattleRoom.m_iMaxPlayerBlue = iBlueCount;
	m_BattleRoom.m_iMaxPlayerRed  = iRedCount;
	m_BattleRoom.m_iMaxObserver = iObserver;
}

void ioBattleRoomMgr::SetPlayerCountChangeAlarm( bool bCountOver )
{
	return;       // 블럭 :: 20090212
	if( bCountOver )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_ORANGE );
		kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1), Help::GetPlayingUserBonusCount()  );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_ORANGE );
		kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(2) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	}
	else
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_ORANGE );
		kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(3), Help::GetPlayingUserBonusCount()  );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_ORANGE );
		kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(4) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	}
}

void ioBattleRoomMgr::SetRandomTeamMode( bool bRandom )
{
	if( m_BattleRoom.m_bRandomTeamMode != bRandom )
	{
		//알림
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );
		if( bRandom )
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3));
		else
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4));
		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}
	m_BattleRoom.m_bRandomTeamMode = bRandom; 
}

void ioBattleRoomMgr::SetStartRoomEnterX( bool bEnterX )
{
	if( m_BattleRoom.m_bStartRoomEnterX != bEnterX )
	{
		//알림
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );
		if( bEnterX )
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3));
		else
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4));
		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}
	m_BattleRoom.m_bStartRoomEnterX = bEnterX; 
}

void ioBattleRoomMgr::SetAutoModeStart( bool bAuto )
{ 
	if( m_BattleRoom.m_bAutoModeStart != bAuto )
	{
		//알림
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );
		if( bAuto )
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3));
		else
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4));
		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}
	m_BattleRoom.m_bAutoModeStart = bAuto; 
}	

void ioBattleRoomMgr::SetBadPingKick( bool bBadPingKick )
{
	if( m_BattleRoom.m_bBadPingKick != bBadPingKick )
	{
		//알림
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );
		if( bBadPingKick )
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );
		else
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );
		g_ChatMgr.SetChatComplexString( STR(4), kPrinter );
	}
	m_BattleRoom.m_bBadPingKick = bBadPingKick;
}

void ioBattleRoomMgr::UpdateRecord( TeamType eWinTeam )
{
	if( !IsBattleRoom() ) return;

 	if( eWinTeam == TEAM_BLUE )
	{
		m_BattleRoom.m_iBlueWin++;
		m_BattleRoom.m_iBlueVictories++;

		m_BattleRoom.m_iRedLose++;
		m_BattleRoom.m_iRedVictories = 0;
	}
	else if( eWinTeam == TEAM_RED )
	{
		m_BattleRoom.m_iRedWin++;
		m_BattleRoom.m_iRedVictories++;

		m_BattleRoom.m_iBlueLose++;
		m_BattleRoom.m_iBlueVictories = 0;
	}
	else if( eWinTeam == TEAM_NONE )
	{
		m_BattleRoom.m_iBlueVictories = 0;
		m_BattleRoom.m_iRedVictories = 0;
	}
}

void ioBattleRoomMgr::GetBattleRoomInfo( BattleRoomInfo *pBattleRoomInfo )
{
	pBattleRoomInfo->m_dwIndex		  = m_BattleRoom.m_dwIndex;
	pBattleRoomInfo->m_szOwnerName    = m_BattleRoom.m_szOwnerName;
	pBattleRoomInfo->m_szName		  = m_BattleRoom.m_szName;
	pBattleRoomInfo->m_szPassword     = m_BattleRoom.m_szPassword;
	pBattleRoomInfo->m_iCurJoiner     = m_BattleRoom.m_iCurJoiner;
	pBattleRoomInfo->m_iCurPlayer     = GetCurPlayer();		// 이걸로... 09.02.23 KDH
	pBattleRoomInfo->m_iMaxPlayerBlue = m_BattleRoom.m_iMaxPlayerBlue;
	pBattleRoomInfo->m_iMaxPlayerRed  = m_BattleRoom.m_iMaxPlayerRed;
	pBattleRoomInfo->m_iMaxObserver   = m_BattleRoom.m_iMaxObserver;
	pBattleRoomInfo->m_iAverageLevel  = m_BattleRoom.m_iAverageLevel;	
	pBattleRoomInfo->m_iBlueWin		  = m_BattleRoom.m_iBlueWin;
	pBattleRoomInfo->m_iBlueLose	  = m_BattleRoom.m_iBlueLose;
	pBattleRoomInfo->m_iBlueVictories = m_BattleRoom.m_iBlueVictories;
	pBattleRoomInfo->m_iRedWin		  = m_BattleRoom.m_iRedWin;
	pBattleRoomInfo->m_iRedLose		  = m_BattleRoom.m_iRedLose;
	pBattleRoomInfo->m_iRedVictories  = m_BattleRoom.m_iRedVictories;
}

bool ioBattleRoomMgr::IsAllUserSafetyLevel()
{
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( !m_vUserList[i].m_bSafetyLevel )
		{
			return false;
		}		
	}
	return true;
}

bool ioBattleRoomMgr::IsAllUserHighGradeLevel( int iGradeLevel )
{
	int iCharCnt = m_vUserList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		BattleRoomUserInfo &rkUser = m_vUserList[i];
		if( rkUser.m_bObserver ) continue;
		if( rkUser.m_iGradeLevel >= iGradeLevel ) continue;

		return false;
	}
	return true;
}

bool ioBattleRoomMgr::IsBroadcastModeType()
{
	if( GetBattleEventType() == BET_BROADCAST_MBC )
	{
		return true;
	}
	return false;
}

bool ioBattleRoomMgr::IsTournamentModeType()
{
	if( !IsBattleRoom() ) 
		return false;

	if( GetBattleEventType() == BET_TOURNAMENT_BATTLE )
		return true;
	return false;
}

bool ioBattleRoomMgr::IsTeamEmoticon()
{
	if( !IsBattleRoom() )
		return false;

	if( GetBattleEventType() == BET_BROADCAST_MBC )
		return true;
	return false;
}

void ioBattleRoomMgr::SetSelectModeNMap( int iModeSelect, int iMapSelect, bool bSafetyLevelRoom )
{
	m_iPreSelectModeIndex = m_iSelectModeIndex;
	m_iPreSelectMapIndex  = m_iSelectMapIndex;

	m_iSelectModeIndex = iModeSelect;
	m_iSelectMapIndex  = iMapSelect;
	m_bSafetyLevelRoom = bSafetyLevelRoom;
	bool bAIMode = g_ModeSelectInfo.IsAIMode( m_iSelectModeIndex );
	if( bAIMode )
	{
		int iUserSize = m_vUserList.size();
		for( int i = 0; i < iUserSize; ++i )
		{
			m_vUserList[i].m_iTeam = TEAM_BLUE;
		}
	}
}

void ioBattleRoomMgr::UpdateUserBattleOrder( const ioHashString &szUserID, int iBattleOrder )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			m_vUserList[i].m_iBattleModeOrder = iBattleOrder;

			//PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			//if( pShamBattle )
			//	pShamBattle->UpdateShamBattle();
			return;
		}
	}
}

int ioBattleRoomMgr::GetUserBattleOrder( const ioHashString &szUserID )
{
	int iBattleOrder = -1;
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
			iBattleOrder = m_vUserList[i].m_iBattleModeOrder;
	}
	return iBattleOrder;
}

int ioBattleRoomMgr::GetUserBattleWaitPosIdx( const ioHashString &szUserID )
{
	int iBattleWaitPos = -1;
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
			iBattleWaitPos = m_vUserList[i].m_iBattleModeWaitPosIndex;
	}
	return iBattleWaitPos;
}

int ioBattleRoomMgr::GetUserTeam( const ioHashString &szUserID )
{
	int iTeam = -1;
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
			iTeam = m_vUserList[i].m_iTeam;
	}
	return iTeam;
}

bool ioBattleRoomMgr::CheckSameTeam( const ioHashString &szUserID_1, const ioHashString &szUserID_2 )
{
	int iTeam1 = -1;
	int iTeam2 = -1;
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID_1 )
			iTeam1 = m_vUserList[i].m_iTeam;
		else if( m_vUserList[i].m_szUserID == szUserID_2 )
			iTeam2 = m_vUserList[i].m_iTeam;
	}
	if(iTeam1 == iTeam2)
		return true;
	return false;
}

// 배틀모드 전용 함수(나와 같은 팀인 출전 중인 유저의 닉네임 반환하는 함수)
ioHashString ioBattleRoomMgr::FindSameTeamUserInBattle( const ioHashString &szMyUserID )
{
	TeamType eTeam = TEAM_NONE;
	bool bFindMyTeam = false;
	ioHashString UserNickInBattle;

	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		// 무슨 팀인지 찾고 찾았으면 i 인덱스 0으로 되돌리고 찾은 플래그 true로 변경
		if( m_vUserList[i].m_szUserID == szMyUserID && !bFindMyTeam )
		{
			eTeam = (TeamType)m_vUserList[i].m_iTeam;
			bFindMyTeam = true;
			i = -1;
			continue;
		}
		
		// 같은 팀이고 난입이 아닌 출전하고 있는 상태이면 닉네임 반환
		if( eTeam == (TeamType)m_vUserList[i].m_iTeam && 
			CheckUserBattleFightFlag( m_vUserList[i].m_szUserID ) &&
			!CheckUserEntryState( m_vUserList[i].m_szUserID ) &&
			bFindMyTeam )
		{
			UserNickInBattle = m_vUserList[i].m_szUserID;
		}
	}
	return UserNickInBattle;
}

ioHashString ioBattleRoomMgr::FindFightUserInBattleByTeam( TeamType eTeam )
{
	bool bFindMyTeam = false;
	ioHashString UserNickInBattle;

	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		// 
		if( m_vUserList[i].m_bBattleModeFighting && m_vUserList[i].m_iTeam == (int)eTeam &&
			!m_vUserList[i].m_bBattleModeEntry )
			UserNickInBattle = m_vUserList[i].m_szUserID;

	}
	return UserNickInBattle;
}

ioHashString ioBattleRoomMgr::GetUserNameByWaitIndex( int iWaitingIndex )
{
	ioHashString szUserName;
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_iBattleModeWaitPosIndex == iWaitingIndex )
		{
			szUserName = m_vUserList[i].m_szUserID;
			break;
		}
	}
	return szUserName;
}

void ioBattleRoomMgr::SetWaitIndex( const ioHashString &szUserID, int iWaitingIndex )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			m_vUserList[i].m_iBattleModeWaitPosIndex = iWaitingIndex;
		}
	}
}

void ioBattleRoomMgr::SwitchWaitIndex( const ioHashString &szUserID_IN, const ioHashString &szUserID_OUT )
{
	int iUserListIdx1 = 0;
	int iUserListIdx2 = 0;
	int iTemp = 0;
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID_IN )
			iUserListIdx1 = i;
		else if( m_vUserList[i].m_szUserID == szUserID_OUT )
			iUserListIdx2 = i;
	}

	LOG.PrintTimeAndLog( 0, "%s - StartUser : %s, EndUser : %s", __FUNCTION__, szUserID_IN.c_str(), szUserID_OUT.c_str() );
	if(iUserListIdx1 != iUserListIdx2)
	{
		LOG.PrintTimeAndLog( 0, "%s - [ WaitPosIndex ] Starter : %d, Ender : %d", __FUNCTION__, m_vUserList[ iUserListIdx1 ].m_iBattleModeWaitPosIndex, m_vUserList[ iUserListIdx2 ].m_iBattleModeWaitPosIndex );
		iTemp = m_vUserList[ iUserListIdx1 ].m_iBattleModeWaitPosIndex;
		m_vUserList[ iUserListIdx1 ].m_iBattleModeWaitPosIndex = m_vUserList[ iUserListIdx2 ].m_iBattleModeWaitPosIndex;
		m_vUserList[ iUserListIdx2 ].m_iBattleModeWaitPosIndex = iTemp;
		LOG.PrintTimeAndLog( 0, "%s - [ Swap ] Starter : %d, Ender : %d", __FUNCTION__, m_vUserList[ iUserListIdx1 ].m_iBattleModeWaitPosIndex, m_vUserList[ iUserListIdx2 ].m_iBattleModeWaitPosIndex );
	}
}

void ioBattleRoomMgr::SetUserBattleFightFlag( const ioHashString &szUserID, bool bFight )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
			m_vUserList[i].m_bBattleModeFighting = bFight;
	}
}

bool ioBattleRoomMgr::CheckUserBattleFightFlag( const ioHashString &szUserID )
{
	bool bFightFlag = false;
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			bFightFlag = m_vUserList[i].m_bBattleModeFighting;
			break;
		}
	}
	return bFightFlag;
}

void ioBattleRoomMgr::SetUserEntryState( const ioHashString &szUserID, bool bEntry )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
			m_vUserList[i].m_bBattleModeEntry = bEntry;
	}
}

bool ioBattleRoomMgr::CheckUserEntryState( const ioHashString &szUserID )
{
	bool bEntryState = false;
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			bEntryState = m_vUserList[i].m_bBattleModeEntry;
			break;
		}
	}
	return bEntryState;
}

void ioBattleRoomMgr::SetUnmoutEquipItem( const ioHashString &szUserID, int iItemType, bool bUnmount )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			m_vUserList[i].m_bUnmountingEquipItem[ iItemType ] = bUnmount;
			break;
		}
	}
}

bool ioBattleRoomMgr::GetUnmoutEquipItem( const ioHashString &szUserID, int iItemType )
{
	bool bUnmount = false;
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			bUnmount = m_vUserList[i].m_bUnmountingEquipItem[ iItemType ];
			return bUnmount;
		}
	}
	return bUnmount;
}

void ioBattleRoomMgr::FillBattleRoomInfo( SP2Packet &rkPacket )
{
	int iUserSize = m_vUserList.size();
	rkPacket << iUserSize;
	for(int i = 0;i < iUserSize;i++)
		rkPacket << m_vUserList[i].m_szUserID << m_vUserList[i].m_iTeam;
}

bool ioBattleRoomMgr::IsObserverUser( const ioHashString &szUserID )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
	    if( rkInfo.m_szUserID == szUserID )
		{
			return rkInfo.m_bObserver;
		}
	}
	return false;
}

int ioBattleRoomMgr::SwitchBattleModeTag( const ioHashString &szUserID, int iBattleTagState )
{
	int iCheck = 0;
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		BattleRoomUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == szUserID )
		{
			if(iBattleTagState == INT_MAX)
				rkUserData.m_iBattleModeTag++; //= !rkUserData.m_bBattleModeTag;
			else
				rkUserData.m_iBattleModeTag = iBattleTagState;
			iCheck = rkUserData.m_iBattleModeTag;
		}
	}
	return iCheck;
}

int ioBattleRoomMgr::GetBattleModeTag( const ioHashString &szUserID )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		BattleRoomUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == szUserID )
		{
			return rkUserData.m_iBattleModeTag;
		}
	}
	return false;
}

int ioBattleRoomMgr::GetCurPlayer()
{
	int iCurPlayer = m_BattleRoom.m_iCurJoiner - GetBattleTeamUserCount( TEAM_NONE ); // CurJoiner - Observer
	if( iCurPlayer < 0 )
		iCurPlayer = 0;
	return iCurPlayer;
}

bool ioBattleRoomMgr::IsTeamNoneMonsterCoinCheck( int iUseCoinCnt )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_bObserver ) continue;
		if( rkInfo.m_iMonsterCoin + rkInfo.m_iGoldMonsterCoin < iUseCoinCnt )
			return true;
	}
	return false;
}

void ioBattleRoomMgr::GetNeedMonsterCoinUserIDVec( IN int iUseCoinCnt, OUT ioHashStringVec &rvUserID )
{
	rvUserID.clear();
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_bObserver ) continue;
		if( rkInfo.m_iMonsterCoin + rkInfo.m_iGoldMonsterCoin < iUseCoinCnt )
			rvUserID.push_back( rkInfo.m_szUserID );
	}
}

bool ioBattleRoomMgr::IsTeamNoneRaidCoinCheck( int iUseCoinCnt )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_bObserver ) continue;
		if( rkInfo.m_iRaidCoin < iUseCoinCnt )
			return true;
	}
	return false;
}

void ioBattleRoomMgr::GetNeedRaidCoinUserIDVec( IN int iUseCoinCnt, OUT ioHashStringVec &rvUserID )
{
	rvUserID.clear();
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_bObserver ) continue;
		if( rkInfo.m_iRaidCoin < iUseCoinCnt )
			rvUserID.push_back( rkInfo.m_szUserID );
	}
}

void ioBattleRoomMgr::SyncMonsterCoin()
{
	if( !IsBattleRoom() ) return;
	
	// 전투 정보 업데이트
	UpdateUserMonsterCoin( g_MyInfo.GetPublicID(), g_MyInfo.GetEtcMonsterCoin(), g_MyInfo.GetEtcGoldMonsterCoin(), g_MyInfo.GetEtcRaidCoin() );

	// 전투방 유저들에게 동기화
	SP2Packet kPacket( CUPK_MONSTERCOIN_SYNC );
	kPacket << g_MyInfo.GetPublicID() << g_MyInfo.GetEtcMonsterCoin() << g_MyInfo.GetEtcGoldMonsterCoin() << g_MyInfo.GetEtcRaidCoin();
	SendBattleRoomP2P( kPacket );
}

int ioBattleRoomMgr::GetUserMonsterCoin( const ioHashString &rkUserID )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_szUserID == rkUserID )
		{
			return rkInfo.m_iMonsterCoin;
		}
	}
	return 0;
}

int ioBattleRoomMgr::GetUserGoldMonsterCoin( const ioHashString &rkUserID )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_szUserID == rkUserID )
		{
			return rkInfo.m_iGoldMonsterCoin;
		}
	}
	return 0;
}

int ioBattleRoomMgr::GetUserRaidCoin( const ioHashString &rkUserID )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_szUserID == rkUserID )
		{
			return rkInfo.m_iRaidCoin;
		}
	}
	return 0;
}

DWORD ioBattleRoomMgr::GetUserCharIndex( const ioHashString &rkUserID )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_szUserID == rkUserID )
		{
			return rkInfo.m_dwCharIndex;
		}
	}
	return 0;
}

DWORD ioBattleRoomMgr::GetUserCharMotion( const ioHashString &rkUserID )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const BattleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_szUserID == rkUserID )
		{
			return rkInfo.m_dwCharMotionItem;
		}
	}
	return 0;
}

ioHashString ioBattleRoomMgr::GetBattleOrder(DWORD dwUserIndex)
{
	ioHashString szBattleOrderText;
	int i = 0;
	for(i = 0; i < (int)m_vUserList.size(); i++)
	{
		if(m_vUserList[i].m_dwUserIndex == dwUserIndex)
			break;
	}
	
	int iBattleOrder = m_vUserList[i].m_iBattleModeOrder;
	if(iBattleOrder == BMO_RANDOM)
		szBattleOrderText = STR(4);
	else if(iBattleOrder == BMO_FIRST)
		szBattleOrderText = STR(1);
	else if(iBattleOrder == BMO_SECOND)
	{
		if( GetSelectMapIndex() == 2 )			// 배틀모드 (2vs2만 체크..) 이면 중견이 아니라 후발
			szBattleOrderText = STR(3);
		else
			szBattleOrderText = STR(2);
	}
	else if(iBattleOrder == BMO_THIRD)
		szBattleOrderText = STR(3);
	else
		szBattleOrderText = STR(5);
	return szBattleOrderText;
}

void ioBattleRoomMgr::PlayStartSound( int iCurSec )
{
	ioHashString szPlaySound = m_szStartSoundDef;
	if( iCurSec == -1 )
	{
		if( !m_szStartSoundGo.IsEmpty() )
			szPlaySound = m_szStartSoundGo;
	}
	else if( COMPARE( iCurSec - 1, 0, (int)m_StartSoundSec.size() ) )
	{
		if( !m_StartSoundSec[iCurSec - 1].IsEmpty() )
			szPlaySound = m_StartSoundSec[iCurSec - 1];
	}

	if( !szPlaySound.IsEmpty() ) 
		g_SoundMgr.PlaySound( szPlaySound );
}

const ioHashString &ioBattleRoomMgr::GetTeamEmoticon( const ioHashString &rkName )
{
	TeamType eTeam = (TeamType)GetBattleUserTeam( rkName );
	if( eTeam == TEAM_BLUE )
		return m_szBlueTeamEmoticon;
	return m_szRedTeamEmoticon;
}

void ioBattleRoomMgr::ProcessBattleRoom()
{
	if( !IsBattleRoom() ) 
		return;

	//
	enum{ TICK_TIME = 1000, };
	if( FRAMEGETTIME() - m_dwProcessTimer > TICK_TIME )
	{
		m_dwProcessTimer = FRAMEGETTIME();

		if( IsTournamentModeType() )
		{
			if( m_BattleRoom.m_dwTourStartTimer > TICK_TIME )
				m_BattleRoom.m_dwTourStartTimer -= TICK_TIME;
			else
				m_BattleRoom.m_dwTourStartTimer = 0;
		}
	}

	//
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		BattleRoomUserInfo &rkUserInfo = m_vUserList[i];
		if( rkUserInfo.m_szUserID == g_MyInfo.GetPublicID() ) continue;
		
		// 몬스터 코인 동기화
		if( rkUserInfo.m_dwMonsterCoinFirstSyncTime > 0 )
		{
			if( FRAMEGETTIME() - rkUserInfo.m_dwMonsterCoinFirstSyncTime > 5000 )
			{
				rkUserInfo.m_dwMonsterCoinFirstSyncTime = FRAMEGETTIME();

				SP2Packet kPacket( CUPK_MONSTERCOIN_FIRST_SYNC );
				kPacket << g_MyInfo.GetUserIndex() << g_MyInfo.GetPublicID() << g_MyInfo.GetEtcMonsterCoin() << g_MyInfo.GetEtcGoldMonsterCoin() << g_MyInfo.GetEtcRaidCoin();
				P2PNetwork::SendToUserByIndex( rkUserInfo.m_dwUserIndex, kPacket );
			}
		}

		// 캐릭터 모션 동기화
		if( rkUserInfo.m_dwCharMotionFirstSyncTime > 0 )
		{
			if( FRAMEGETTIME() - rkUserInfo.m_dwCharMotionFirstSyncTime > 4000 )
			{
				rkUserInfo.m_dwCharMotionFirstSyncTime = FRAMEGETTIME();

				SP2Packet kPacket( CUPK_ETCITEM_MOTION_OPTION_FIRST_SYNC );
				kPacket << g_MyInfo.GetPublicID() << ioEtcItemMotion::MOTION_OPTION_5 << GetUserCharMotion( g_MyInfo.GetPublicID() );
				P2PNetwork::SendToUserByIndex( rkUserInfo.m_dwUserIndex, kPacket );
			}
		}

		// 캐릭터 동기화
		if( rkUserInfo.m_dwCharIndexFirstSyncTime > 0 )
		{
			if( FRAMEGETTIME() - rkUserInfo.m_dwCharIndexFirstSyncTime > 5000 )
			{
				rkUserInfo.m_dwCharIndexFirstSyncTime = FRAMEGETTIME();

				CHARACTER kCharInfo;
				DWORD dwMyCharIndex = GetUserCharIndex( g_MyInfo.GetPublicID() );
				g_MyInfo.CopyCharacter( g_MyInfo.GetCharArray( dwMyCharIndex ), &kCharInfo );
				SP2Packet kPacket( CUPK_FIRST_SYNC_CHARINFO );
				kPacket << g_MyInfo.GetPublicID() << dwMyCharIndex << kCharInfo;
				g_MyInfo.FillCharItemData( dwMyCharIndex, kPacket );
				P2PNetwork::SendToUserByIndex( rkUserInfo.m_dwUserIndex, kPacket );				
			}
		}
	}
}

void ioBattleRoomMgr::SendBattleRoomP2P( SP2Packet &rkPacket )
{
	if( !IsBattleRoom() ) 
		return;

	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		BattleRoomUserInfo &rkUserInfo = m_vUserList[i];
		if( rkUserInfo.m_szUserID == g_MyInfo.GetPublicID() ) continue;

		P2PNetwork::SendToUserByIndex( rkUserInfo.m_dwUserIndex, rkPacket );
	}
}

void ioBattleRoomMgr::SendBattleRoomAnn( const char *szChat )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	SP2Packet kPacket( CUPK_CHAT );
	kPacket << CT_ANNOUNCE;
	kPacket << g_MyInfo.GetPublicID();
	kPacket << szChat;

	SendBattleRoomP2P( kPacket );
}

int ioBattleRoomMgr::GetTeamAttackType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iTeamAttackType;

	return 0;
}

int ioBattleRoomMgr::GetChangeCharType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iChangeCharType;

	return 0;
}

int ioBattleRoomMgr::GetCoolTimeType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iCoolTimeType;

	return 0;
}

int ioBattleRoomMgr::GetRedHPType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iRedHPType;

	return 0;
}

int ioBattleRoomMgr::GetBlueHPType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iBlueHPType;

	return 0;
}

int ioBattleRoomMgr::GetDropDamageType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iDropDamageType;

	return 0;
}

int ioBattleRoomMgr::GetGravityType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iGravityType;

	return 0;
}

int ioBattleRoomMgr::GetGetUpType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iGetUpType;

	return 0;
}

int ioBattleRoomMgr::GetRedMoveSpeedType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iRedMoveSpeedType;

	return 0;
}

int ioBattleRoomMgr::GetBlueMoveSpeedType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iBlueMoveSpeedType;

	return 0;
}

int ioBattleRoomMgr::GetRedEquipType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iRedEquipType;

	return 0;
}

int ioBattleRoomMgr::GetBlueEquipType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iBlueEquipType;

	return 0;
}

int ioBattleRoomMgr::GetKOType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iKOType;

	return 0;
}

int ioBattleRoomMgr::GetKOEffectType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iKOEffectType;

	return 0;
}

int ioBattleRoomMgr::GetRedBlowType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iRedBlowType;

	return 0;
}

int ioBattleRoomMgr::GetBlueBlowType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iBlueBlowType;

	return 0;
}

int ioBattleRoomMgr::GetRoundType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iCatchModeRoundType;

	// 서버에서 예외처리 위해 -1
	return -1;
}

int ioBattleRoomMgr::GetRoundTimeType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iCatchModeRoundTimeType;

	// 서버에서 예외처리 위해 -1
	return -1;
}

int ioBattleRoomMgr::GetGrowthUseType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iGrowthUseType;

	return 0;
}

int ioBattleRoomMgr::GetExtraItemUseType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iExtraItemUseType;

	return 0;
}

int ioBattleRoomMgr::GetPreSetModeType()
{
	if( IsUseExtraOption() )
		return m_BattleRoom.m_iPreSetMode;

	int iDefault = g_RoomOptionMgr.GetOptionTypeCnt(PartyOptionWnd::ID_PRESET_BTN) - 1;
	iDefault = max( 0, iDefault );

	return iDefault;
}

void ioBattleRoomMgr::SetUseExtraOption( bool bUseOption, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption != bUseOption && bAlarm )
	{
		//알림
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), g_ModeSelectInfo.GetCustomModeTitle().c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		if( bUseOption )
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );
		else
			kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );
		
		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_bUseExtraOption = bUseOption;
}

void ioBattleRoomMgr::SetNoChallenger( bool bNoChallenger )
{
	m_BattleRoom.m_bNoChallenger = bNoChallenger;
}

void ioBattleRoomMgr::SetChangeCharType( int iChangeCharType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iChangeCharType != iChangeCharType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_CHANGE_CHAR_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetChangeCharDesc( iChangeCharType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );
		
		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iChangeCharType = iChangeCharType; 
}

void ioBattleRoomMgr::SetTeamAttackType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iTeamAttackType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_TEAM_ATTACK_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetTeamAttackDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iTeamAttackType = iType;
}

void ioBattleRoomMgr::SetCoolTimeType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iCoolTimeType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_COOLTIME_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetCoolTimeDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iCoolTimeType = iType;
}

void ioBattleRoomMgr::SetRedHPType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iRedHPType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_HP_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetRedHPDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iRedHPType = iType;
}

void ioBattleRoomMgr::SetBlueHPType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iBlueHPType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_HP_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetBlueHPDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(5) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iBlueHPType = iType;
}

void ioBattleRoomMgr::SetDropDamageType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iDropDamageType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_DROP_DAMAGE_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetDropDamageDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iDropDamageType = iType;
}

void ioBattleRoomMgr::SetGravityType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iGravityType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_GRAVITY_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetGravityDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iGravityType = iType;
}

void ioBattleRoomMgr::SetGetUpType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iGetUpType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_GETUP_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetGetUpDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iGetUpType = iType;
}

void ioBattleRoomMgr::SetRedMoveSpeedType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iRedMoveSpeedType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_MOVE_SPEED_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetRedMoveSpeedDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iRedMoveSpeedType = iType;
}

void ioBattleRoomMgr::SetBlueMoveSpeedType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iBlueMoveSpeedType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_MOVE_SPEED_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetBlueMoveSpeedDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iBlueMoveSpeedType = iType;
}

void ioBattleRoomMgr::SetKOType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iKOType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_KO_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetKODesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iKOType = iType;
}

void ioBattleRoomMgr::SetKOEffectType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iKOEffectType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_KO_EFFECT_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetKOEffectDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iKOEffectType = iType;
}

void ioBattleRoomMgr::SetRedBlowType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iRedBlowType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_BLOW_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetRedBlowDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iRedBlowType = iType;
}

void ioBattleRoomMgr::SetBlueBlowType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iBlueBlowType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_BLOW_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetBlueBlowDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iBlueBlowType = iType;
}

void ioBattleRoomMgr::SetRedEquipType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iRedEquipType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_EQUIP_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetRedEquipDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iRedEquipType = iType;
}

void ioBattleRoomMgr::SetBlueEquipType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iBlueEquipType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_EQUIP_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetBlueEquipDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(4) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iBlueEquipType = iType;
}

void ioBattleRoomMgr::SetRoundType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iCatchModeRoundType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_ROUND_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetRoundTypeDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iCatchModeRoundType = iType;
}

void ioBattleRoomMgr::SetRoundTimeType( int iType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iCatchModeRoundTimeType != iType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_ROUND_TIME_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetRoundTimeTypeDesc( iType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
	}

	m_BattleRoom.m_iCatchModeRoundTimeType = iType;
}

void ioBattleRoomMgr::SetGrowthUseType( int iGrowthUseType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iGrowthUseType != iGrowthUseType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_GROWTH_USE_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetGrowthUseDesc( iGrowthUseType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );

		g_ChatMgr.SetChatComplexString( STR(3), kPrinter );
	}

	m_BattleRoom.m_iGrowthUseType = iGrowthUseType;
}

void ioBattleRoomMgr::SetExtraItemUseType( int iExtraItemUseType, bool bAlarm )
{
	if( m_BattleRoom.m_bUseExtraOption &&
		m_BattleRoom.m_iExtraItemUseType != iExtraItemUseType &&
		bAlarm )
	{
		//알림
		ioHashString szTleText, szBtnText;
		PartyOptionWnd *pOption = dynamic_cast<PartyOptionWnd*>( g_GUIMgr.FindWnd(PARTY_OPTION_WND) );
		if( pOption )
		{
			szTleText = pOption->GetOptionTitle( PartyOptionWnd::ID_EXTRAITEM_USE_TYPE_TLE );
			szBtnText = g_RoomOptionMgr.GetExtraItemUseDesc( iExtraItemUseType );
		}

		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), szTleText.c_str(), szBtnText.c_str() );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );

		g_ChatMgr.SetChatComplexString( STR(3), kPrinter );
	}

	m_BattleRoom.m_iExtraItemUseType = iExtraItemUseType;
}

bool ioBattleRoomMgr::SetPreSetModeType( int iType, bool bUpdate )
{
	if( m_BattleRoom.m_bUseExtraOption && m_BattleRoom.m_iPreSetMode != iType )
	{
		m_BattleRoom.m_iPreSetMode = iType;

		if( bUpdate )
		{
			PreSetInfo kPreSet;

			if( g_RoomOptionMgr.GetPreSetInfo( iType, kPreSet ) )
			{
				SetTeamAttackType( kPreSet.m_iTeamAttackType, true );

				SetChangeCharType( kPreSet.m_iChangeCharType, true );
				SetRedHPType( kPreSet.m_iRedHPType, true );
				SetBlueHPType( kPreSet.m_iBlueHPType, true );

				SetCoolTimeType( kPreSet.m_iCoolTimeType, true );
				SetDropDamageType( kPreSet.m_iDropDamageType, true );
				SetGravityType( kPreSet.m_iGravityType, true );
				SetGetUpType( kPreSet.m_iGetUpType, true );
				SetKOType( kPreSet.m_iKOType, true );
				SetKOEffectType( kPreSet.m_iKOEffectType, true );
				SetRedBlowType( kPreSet.m_iRedBlowType, true );
				SetBlueBlowType( kPreSet.m_iBlueBlowType, true );
				SetRedMoveSpeedType( kPreSet.m_iRedMoveSpeedType, true );
				SetBlueMoveSpeedType( kPreSet.m_iBlueMoveSpeedType, true );
				SetRedEquipType( kPreSet.m_iRedEquipType, true );
				SetBlueEquipType( kPreSet.m_iBlueEquipType, true );

				SetRoundType( kPreSet.m_iCatchModeRoundType, true );
				SetRoundTimeType( kPreSet.m_iCatchModeRoundTimeType, true );

				SetGrowthUseType( kPreSet.m_iGrowthUseType, true );
				SetExtraItemUseType( kPreSet.m_iExtraItemUseType, true );
			}
		}

		return true;
	}

	return false;
}

bool ioBattleRoomMgr::SetPreSetModeTypeManual( int iType )
{
	m_BattleRoom.m_bUseExtraOption = true;
	m_BattleRoom.m_iPreSetMode = iType;

	PreSetInfo kPreSet;

	if( g_RoomOptionMgr.GetPreSetInfo( iType, kPreSet ) )
	{
		SetTeamAttackType( kPreSet.m_iTeamAttackType, false );

		SetChangeCharType( kPreSet.m_iChangeCharType, false );
		SetRedHPType( kPreSet.m_iRedHPType, false );
		SetBlueHPType( kPreSet.m_iBlueHPType, false );

		SetCoolTimeType( kPreSet.m_iCoolTimeType, false );
		SetDropDamageType( kPreSet.m_iDropDamageType, false );
		SetGravityType( kPreSet.m_iGravityType, false );
		SetGetUpType( kPreSet.m_iGetUpType, false );
		SetKOType( kPreSet.m_iKOType, false );
		SetKOEffectType( kPreSet.m_iKOEffectType, false );
		SetRedBlowType( kPreSet.m_iRedBlowType, false );
		SetBlueBlowType( kPreSet.m_iBlueBlowType, false );
		SetRedMoveSpeedType( kPreSet.m_iRedMoveSpeedType, false );
		SetBlueMoveSpeedType( kPreSet.m_iBlueMoveSpeedType, false );
		SetRedEquipType( kPreSet.m_iRedEquipType, false );
		SetBlueEquipType( kPreSet.m_iBlueEquipType, false );

		SetRoundType( kPreSet.m_iCatchModeRoundType, false );
		SetRoundTimeType( kPreSet.m_iCatchModeRoundTimeType, false );

		SetGrowthUseType( kPreSet.m_iGrowthUseType, false );
		SetExtraItemUseType( kPreSet.m_iExtraItemUseType, false );

		return true;
	}

	return false;
}


void ioBattleRoomMgr::SendBattleRoomOption()
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	SP2Packet kPacket( CTPK_MACRO_COMMAND );
	kPacket << MACRO_OPTION_CHANGE
			<< m_BattleRoom.m_bRandomTeamMode << m_BattleRoom.m_bStartRoomEnterX << m_BattleRoom.m_bAutoModeStart << m_BattleRoom.m_bBadPingKick
			<< m_BattleRoom.m_bUseExtraOption << m_BattleRoom.m_bNoChallenger << m_BattleRoom.m_iChangeCharType
			<< m_BattleRoom.m_iCoolTimeType << m_BattleRoom.m_iRedHPType << m_BattleRoom.m_iBlueHPType
			<< m_BattleRoom.m_iDropDamageType << m_BattleRoom.m_iGravityType
			<< m_BattleRoom.m_iPreSetMode << m_BattleRoom.m_iTeamAttackType << m_BattleRoom.m_iGetUpType
			<< m_BattleRoom.m_iKOType << m_BattleRoom.m_iRedBlowType << m_BattleRoom.m_iBlueBlowType
			<< m_BattleRoom.m_iRedMoveSpeedType << m_BattleRoom.m_iBlueMoveSpeedType << m_BattleRoom.m_iKOEffectType
			<< m_BattleRoom.m_iRedEquipType << m_BattleRoom.m_iBlueEquipType
			<< m_BattleRoom.m_iCatchModeRoundType << m_BattleRoom.m_iCatchModeRoundTimeType
			<< m_BattleRoom.m_iGrowthUseType << m_BattleRoom.m_iExtraItemUseType;
	TCPNetwork::SendToServer( kPacket );

	bool bAIMode = g_ModeSelectInfo.IsAIMode( m_iSelectModeIndex );
	if( bAIMode )
	{
		SP2Packet kPacket2( CTPK_MACRO_COMMAND );
		kPacket2 << MACRO_AI_LEVEL_CHANGE << GetAiLevel();
		TCPNetwork::SendToServer( kPacket2 );
	}
}

void ioBattleRoomMgr::ModeStartPlayerCount()
{
	m_iModeStartPlayerCount = 0;
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_bObserver ) continue;

		m_iModeStartPlayerCount++;
	}
}

void ioBattleRoomMgr::StartChatBubble( const char *szID, const char *szChat, bool bWholeChat )
{
	if( !IsBattleRoom() ) return;

	PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
	if( pShamBattle && pShamBattle->IsShow() )
	{
		pShamBattle->StartChatBubble( szID, szChat, bWholeChat );
	}
	else
	{
		TotalResultMainWnd *pTotalResult = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
		if( pTotalResult && pTotalResult->IsShow() )
		{
			pTotalResult->StartChatBubble( szID, szChat, bWholeChat );
		}
	}
}

void ioBattleRoomMgr::SetBattleCancelCount()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsLimitStartCancel() )
		m_iBattleCancelCount++;
	else
		m_iBattleCancelCount = 0;
}

bool ioBattleRoomMgr::IsBattleRoomMovePenaltyPeso()
{
	// 전투방에서 전투방으로 이동시에만 사용할것!!!!
	if( IsBattleRoom() == false )
		return false;

	ioBattleRoomExitMsgBox *pBattleRoomExit = dynamic_cast< ioBattleRoomExitMsgBox * >( g_GUIMgr.FindWnd( BATTLEROOM_EXIT_MSG_BOX ) );
	if( pBattleRoomExit == NULL )
		return false;
	return pBattleRoomExit->IsGlobalCheckPenaltyExit();          // UI가 Show 될 때 페널티 여부가 판단된다.
}
int ioBattleRoomMgr::GetAIMaxSlot( int iMapIndex )
{
	int iSlotCount = 2;	//default

	int index = iMapIndex-1;
	int nSize = m_vecAIMaxSlot.size();
	if( COMPARE( index, 0, nSize ) )
	{
		iSlotCount = m_vecAIMaxSlot[index];
	}

	return iSlotCount;
}

void ioBattleRoomMgr::SetAiLevel( int iLevel )
{
	m_BattleRoom.m_iAiLevel = iLevel;
}

int ioBattleRoomMgr::GetAiLevel()
{
	return m_BattleRoom.m_iAiLevel;
}

void ioBattleRoomMgr::ClearBattleModeInfoAtEndOfBattleMode()
{
	int i=0;
	for( i=0 ; i < (int)m_vUserList.size(); i++ )
		m_vUserList[i].ClearBattleModeInfo();
	
	for( i=0 ; i < (int)m_vUserList.size(); i++ )
	{
		m_vUserList[i].m_bUnmountingEquipItem[ ES_WEAPON ] = false;
		m_vUserList[i].m_bUnmountingEquipItem[ ES_ARMOR ] = false;
		m_vUserList[i].m_bUnmountingEquipItem[ ES_HELMET ] = false;
		m_vUserList[i].m_bUnmountingEquipItem[ ES_CLOAK ] = false;
	}
}

int ioBattleRoomMgr::GetTotalUserCount()
{
	return (int)m_vUserList.size();
}

ioHashString ioBattleRoomMgr::GetUserNameByVecIdx( int iIndex )
{
	ioHashString szUserName;
	if( iIndex < (int)m_vUserList.size() )
		szUserName = m_vUserList[ iIndex ].m_szUserID;

	return szUserName;
}