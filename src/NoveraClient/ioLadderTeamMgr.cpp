

#include "stdafx.h"

#include "ioLadderTeamMgr.h"

#include "ioEtcItem.h"

#include "GUI/ioUserInfoMgr.h"
#include "GUI/ioMessageBox.h"
#include "GUI/ioFriendManager.h"
#include "GUI/LadderTeamWnd.h"
#include "GUI/VoiceChatWnd.h"
#include "GUI/TotalResultMainWnd.h"

template<> ioLadderTeamMgr* Singleton< ioLadderTeamMgr >::ms_Singleton = 0;

ioLadderTeamMgr::ioLadderTeamMgr()
{
	m_LadderTeam.m_dwTeamIndex    = 0;
	m_LadderTeam.m_dwGuildIndex   = 0;

	m_iSelectModeIndex  = -1;
	m_iSelectMapIndex	= -1;

	m_bLadderBattleModePlaying = false;

	m_dwTeamState      = TMS_READY;
	m_dwJoinGuildIndex = 0;
	m_bSearchLevelMatch = false;
	m_bSearchSameUser  = false;
	m_bHeroMatchMode   = false;
	m_bOtherChatHide   = false;
	m_bBadPingKick	   = true;
	m_iLadderTeamLimitGrade = 0;
	m_iHeroMatchLimitGrade  = 0;

	m_iOtherCampType = CAMP_NONE;
	m_iOtherTeamType = TEAM_NONE;

	m_HeroMatchOtherInfo.Initialize();
}

ioLadderTeamMgr::~ioLadderTeamMgr()
{	
	m_vLeaveRoom.clear();
	m_vKickOutRoom.clear();
	m_vUserList.clear();
	m_SearchSecSndMap.clear();
}

ioLadderTeamMgr& ioLadderTeamMgr::GetSingleton()
{
	return Singleton< ioLadderTeamMgr >::GetSingleton();
}

void ioLadderTeamMgr::LoadLadderTeamInfo()
{
	ioINILoader_e kLoader( "config/ladder_config.ini" );

	kLoader.SetTitle_e( "info" );	
	m_iLadderTeamLimitGrade = kLoader.LoadInt_e( "LadderTeamLimitGrade", 0 );
	m_iHeroMatchLimitGrade  = kLoader.LoadInt_e( "HeroMatchLimitGrade", 0 );

	int i = 0;
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	kLoader.SetTitle_e( "TeamName" );
	int iMaxName = kLoader.LoadInt_e( "MaxName", 0 );
	for(i = 0;i < iMaxName;i++)
	{
		sprintf_e( szKey, "Name_%d", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_ReserveTeamName.push_back( szBuf );
	}

	m_SearchSecSndMap.clear();
	kLoader.SetTitle_e( "Sound" );
	kLoader.LoadString_e( "SearchFailedSnd", "", szBuf, MAX_PATH );
	m_szSearchFailedSnd = szBuf;
	kLoader.LoadString_e( "SearchCancelSnd", "", szBuf, MAX_PATH );
	m_szSearchCancelSnd = szBuf;
	kLoader.LoadString_e( "SearchSecDefSnd", "", szBuf, MAX_PATH );
	m_szSearchSecSndDef = szBuf;
	int iMaxSec = kLoader.LoadInt_e( "MaxSearchSnd", 0 );
	for(i = 0;i < iMaxSec;i++)
	{
		sprintf_e( szKey, "SearchSound_%dSec", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_SearchSecSndMap.insert( SearchSecSoundMap::value_type( i + 1, szBuf ) );
	}
}

void ioLadderTeamMgr::LeaveTeam()
{
	if( m_LadderTeam.m_dwTeamIndex != 0 )
	{
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );
		g_ChatMgr.SetChatComplexString( STR(2), kPrinter );
		g_GUIMgr.HideWnd( LADDER_BATTLE_WND );
	}

	if( m_LadderTeam.m_dwTeamIndex != 0 )
	{
		SetLeaveRoomInfo( m_LadderTeam.m_dwTeamIndex );
	}

	m_LadderTeam.m_dwTeamIndex    = 0;
	m_LadderTeam.m_dwGuildIndex   = 0;

	m_szPrevMyOwnerID.Clear();
	m_bLadderBattleModePlaying = false;

	m_dwTeamState       = TMS_READY;
	m_dwJoinGuildIndex  = 0;
	m_bSearchLevelMatch  = false;
	m_bSearchSameUser   = false;
	m_bHeroMatchMode    = false;
	m_bOtherChatHide    = false;
	m_bBadPingKick		= true;
	m_iSelectModeIndex  = -1;
	m_iSelectMapIndex	= -1;
	m_HeroMatchOtherInfo.Initialize();
	SetKickOutUser( "" );
	UserListClear();
	g_FriendManager.SetUpdate();	

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		if( ioMyRoomMode::GetMyRoomType() == MRT_LADDER )
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

void ioLadderTeamMgr::UserListClear()
{
	m_vUserList.clear();
}

void ioLadderTeamMgr::SetLeaveRoomInfo( DWORD dwIndex )
{
	LeaveRoomInfo kor;
	kor.m_dwIndex         = dwIndex;
	kor.m_dwLeaveRoomTime = FRAMEGETTIME();
	m_vLeaveRoom.push_back( kor );
}

bool ioLadderTeamMgr::IsLeaveRoomInfo( DWORD dwIndex )
{
	int iSize = m_vLeaveRoom.size();
	if( iSize == 0 )
		return false;

	for(int i = 0;i < iSize;i++)
	{
		LeaveRoomInfo &kor = m_vLeaveRoom[i];
		if( kor.m_dwIndex == dwIndex )
		{
			if( FRAMEGETTIME() - kor.m_dwLeaveRoomTime > 30000 )
			{
				m_vLeaveRoom.erase( m_vLeaveRoom.begin() + i );
				return false;
			}
			else
				return true;
		}
	}
	return false;
}

void ioLadderTeamMgr::SetKickOutRoom( DWORD dwIndex )
{
	LeaveRoomInfo kor;
	kor.m_dwIndex         = dwIndex;
	kor.m_dwLeaveRoomTime = FRAMEGETTIME();
	m_vKickOutRoom.push_back( kor );
}

bool ioLadderTeamMgr::IsKickOutRoom( DWORD dwIndex )
{
	int iSize = m_vKickOutRoom.size();
	if( iSize == 0 )
		return false;

	for(int i = 0;i < iSize;i++)
	{
		LeaveRoomInfo &kor = m_vKickOutRoom[i];
		if( kor.m_dwIndex == dwIndex )
		{
			if( FRAMEGETTIME() - kor.m_dwLeaveRoomTime > 180000 )
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

void ioLadderTeamMgr::SetInvitedRefuse( DWORD dwIndex )
{
	InvitedRefuse kRefuse;
	kRefuse.m_dwIndex    	= dwIndex;
	kRefuse.m_dwRefuseTimer = FRAMEGETTIME();
	m_vInvitedRefuse.push_back( kRefuse );
}

bool ioLadderTeamMgr::IsInvitedRefuseRoom( DWORD dwIndex )
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
			{
				return true;
			}
		}
	}
	return false;
}

void ioLadderTeamMgr::SetKickOutUser( ioHashString szName )
{
	m_szKickOutUser = szName;

	if( m_szKickOutUser.IsEmpty() )
		return;
	
	ioComplexStringPrinter kPrinter;
	g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
	kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szName.c_str() );
	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );
	g_ChatMgr.SetChatComplexString( STR(1), kPrinter );
}

bool ioLadderTeamMgr::IsLadderTeam()
{
	if( m_LadderTeam.m_dwTeamIndex == 0 )
		return false;

	return true;
}

bool ioLadderTeamMgr::IsLadderTeamFull()
{
	if( m_LadderTeam.m_iCurPlayer >= m_LadderTeam.m_iMaxPlayer )
		return true;
	return false;
}

void ioLadderTeamMgr::SetLadderBattleModePlaying( bool bPlaying )
{
	m_bLadderBattleModePlaying = bPlaying;
}

void ioLadderTeamMgr::SetTeamState( DWORD dwState )
{
	if( m_dwTeamState == dwState ) 
		return;

	switch( dwState )
	{
	case TMS_READY:
		{
			if( m_dwTeamState == TMS_SEARCH_RESERVE || m_dwTeamState == TMS_SEARCH_PROCEED )
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );
				g_ChatMgr.SetChatComplexString( STR(2), kPrinter, false );

				if( !m_szSearchCancelSnd.IsEmpty() )
					g_SoundMgr.PlaySound( m_szSearchCancelSnd );
			}
		}
		break;
	case TMS_SEARCH_RESERVE:
		{
			if( m_dwTeamState == TMS_READY )
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );
				g_ChatMgr.SetChatComplexString( STR(4), kPrinter, false );
			}
		}
		break;
	case TMS_SEARCH_PROCEED:
		break;
	}
	m_dwTeamState = dwState;
}

void ioLadderTeamMgr::MatchSearchFailed()
{
	if( !IsLadderTeam() ) return;

	m_dwTeamState = TMS_READY;
	ioComplexStringPrinter kPrinter;
	g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );
	g_ChatMgr.SetChatComplexString( STR(2), kPrinter, false );
	if( GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
	{
		//g_GUIMgr.SetMsgBox( MB_OK, NULL, "검색조건을 넓혀서 재시도해주세요." );

		if( IsHeroMatchMode() )
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(7) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(8) );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		else
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(3)  );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(4) );

			kPrinter[2].SetTextStyle( TS_NORMAL );
			kPrinter[2].SetBkColor( 0, 0, 0 );	
			kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[2].AddTextPiece( FONT_SIZE_13,"    " );

			kPrinter[3].SetTextStyle( TS_NORMAL );
			kPrinter[3].SetBkColor( 0, 0, 0 );	
			kPrinter[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(5) );

			kPrinter[4].SetTextStyle( TS_NORMAL );
			kPrinter[4].SetBkColor( 0, 0, 0 );	
			kPrinter[4].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(6) );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
	}

	if( !m_szSearchFailedSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szSearchFailedSnd );
}

bool ioLadderTeamMgr::IsSearching()
{
	if( GetTeamState() == TMS_SEARCH_RESERVE || GetTeamState() == TMS_SEARCH_PROCEED )
		return true;
	return false;
}

void ioLadderTeamMgr::SetTeamRanking( DWORD dwTeamIndex, int iTeamRanking )
{
	if( !IsLadderTeam() ) return;

	if( dwTeamIndex == m_LadderTeam.m_dwTeamIndex )
	{
		m_LadderTeam.m_iTeamRanking = iTeamRanking;
	}	
	else if( dwTeamIndex == m_OtherTeam.m_dwTeamIndex )
	{
		m_OtherTeam.m_iTeamRanking = iTeamRanking;
	}
}

void ioLadderTeamMgr::SetTeamRanking( DWORD dwTeamIndex, int iTeamRanking, int iGapRanking )
{
	if( !IsLadderTeam() ) return;

	if( dwTeamIndex == m_LadderTeam.m_dwTeamIndex )
	{
		m_LadderTeam.m_iTeamRanking = iTeamRanking;
		m_LadderTeam.m_iGapTeamRanking = iGapRanking;
	}	
	else if( dwTeamIndex == m_OtherTeam.m_dwTeamIndex )
	{
		m_OtherTeam.m_iTeamRanking = iTeamRanking;
		m_OtherTeam.m_iGapTeamRanking = iGapRanking;
	}
}

void ioLadderTeamMgr::SetJoinGuildIndex( DWORD dwJoinGuildIndex )
{
	m_dwJoinGuildIndex = dwJoinGuildIndex;
}

void ioLadderTeamMgr::SetSearchLevelMatch( bool bSearchLevelMatch )
{
	m_bSearchLevelMatch = bSearchLevelMatch;
}

void ioLadderTeamMgr::SetSearchSameUser( bool bSearchSameUser )
{
	m_bSearchSameUser = bSearchSameUser;
}

void ioLadderTeamMgr::SetOtherChatHide( bool bOtherChatHide )
{
	m_bOtherChatHide = bOtherChatHide;
}

void ioLadderTeamMgr::SetBadPingKick( bool bBadPingKick )
{
	m_bBadPingKick = bBadPingKick;
}

void ioLadderTeamMgr::ChangeMyOwner()
{
	if( !m_szPrevMyOwnerID.IsEmpty() && !m_LadderTeam.m_szOwnerName.IsEmpty() )
	{
		if( m_szPrevMyOwnerID != m_LadderTeam.m_szOwnerName )
		{
			// 방장이 변경되었다.
			if( m_LadderTeam.m_iCampType == CAMP_BLUE )
				g_ChatMgr.SetHostChange( STR(1), m_LadderTeam.m_szOwnerName.c_str(), TEAM_BLUE );	
			else
				g_ChatMgr.SetHostChange( STR(2), m_LadderTeam.m_szOwnerName.c_str(), TEAM_RED );	
			if( !IsLadderBattleModePlaying() && m_LadderTeam.m_szOwnerName == g_MyInfo.GetPublicID() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			}				
		}
	}
	m_szPrevMyOwnerID = m_LadderTeam.m_szOwnerName;
}

void ioLadderTeamMgr::UpdateUserData( const ioHashString &szUserID, int iGradeLevel, int iAddLadderPoint )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			m_vUserList[i].m_iGradeLevel	= iGradeLevel;
			m_vUserList[i].m_iLadderPoint   = max( 0, m_vUserList[i].m_iLadderPoint + iAddLadderPoint );
			//
			LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
			if( pLadderBattleWnd )
				pLadderBattleWnd->UpdateLadderBattle();
			return;
		}
	}
}

void ioLadderTeamMgr::UpdateUserCharIndex( const ioHashString &szUserID, DWORD dwCharIndex, bool bSync )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		LadderTeamUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == szUserID )
		{
			rkUserData.m_dwCharIndex = dwCharIndex;
			if( bSync )
			{
				rkUserData.m_dwCharIndexFirstSyncTime = 0;
			}
			//
			LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
			if( pLadderBattleWnd )
				pLadderBattleWnd->UpdateLadderBattle();
			return;
		}
	}
}

void ioLadderTeamMgr::UpdateUserCharMotion( const ioHashString &szUserID, DWORD dwCharMotion )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		LadderTeamUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == szUserID )
		{
			rkUserData.m_dwCharMotionItem = dwCharMotion;
			rkUserData.m_dwCharMotionFirstSyncTime = 0;

			//
			LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
			if( pLadderBattleWnd )
				pLadderBattleWnd->UpdateLadderBattle();
			return;
		}
	}
}

void ioLadderTeamMgr::JoinUser( LadderTeamUserInfo &kUser )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		LadderTeamUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == kUser.m_szUserID )
		{
			rkUserData.m_dwUserIndex    = kUser.m_dwUserIndex;
			rkUserData.m_iGradeLevel	= kUser.m_iGradeLevel;
			rkUserData.m_iAbilityLevel  = kUser.m_iAbilityLevel;
			rkUserData.m_iLadderPoint   = kUser.m_iLadderPoint;
			rkUserData.m_szPublicIP     = kUser.m_szPublicIP;
			rkUserData.m_szPrivateIP    = kUser.m_szPrivateIP;
			rkUserData.m_iPort          = kUser.m_iPort;	
			rkUserData.m_szTransferIP   = kUser.m_szTransferIP;
			rkUserData.m_iTransferPort  = kUser.m_iTransferPort;
			
			// 자기 자신의 캐릭터 인덱스 세팅
			if( rkUserData.m_szUserID == g_MyInfo.GetPublicID() )
			{
				rkUserData.m_dwCharMotionItem = g_MyInfo.GetEtcMotionAni( ioEtcItemMotion::MOTION_OPTION_6 );  // 래더전 대기 모션
				rkUserData.m_dwCharIndex  = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
				if( rkUserData.m_dwCharIndex == 0 )
					rkUserData.m_dwCharIndex = g_MyInfo.GetCharIndex( 0 );
			}
			return;
		}
	}

	if( kUser.m_szUserID == g_MyInfo.GetPublicID() )
	{
		kUser.m_dwCharMotionItem = g_MyInfo.GetEtcMotionAni( ioEtcItemMotion::MOTION_OPTION_6 );  // 래더전 대기 모션
		kUser.m_dwCharIndex  = g_MyInfo.GetCharIndex( g_MyInfo.GetSelectCharArray() );
		if( kUser.m_dwCharIndex == 0 )
			kUser.m_dwCharIndex = g_MyInfo.GetCharIndex( 0 );
	}
	else
	{
		kUser.m_dwCharMotionFirstSyncTime = FRAMEGETTIME();
		kUser.m_dwCharIndexFirstSyncTime = FRAMEGETTIME();
	}

	m_vUserList.push_back( kUser );
	
	if( kUser.m_szUserID == g_MyInfo.GetPublicID() )
	{
		g_MyInfo.SetMyAddress( kUser.m_szPublicIP, kUser.m_iPort );
		g_CampMgr.TodayBattleEndAlarm( true );
	}
	else if( kUser.m_szUserID != g_MyInfo.GetPublicID() )
	{
		g_App.AddNetworkTable( kUser.m_dwUserIndex, kUser.m_szUserID, kUser.m_szPublicIP, kUser.m_szPrivateIP, kUser.m_szTransferIP,
							   kUser.m_iPort, kUser.m_iTransferPort, P2P_STYLE_VOICE, 1 );

		// 친구 정보 갱신
		g_FriendManager.ClientFriendInfoUpdate( kUser.m_szUserID, kUser.m_iGradeLevel, UP_LADDER_TEAM );

		if ( Setting::UseVoiceChat() && Setting::UseMIC() )
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

	if( kUser.m_szUserID != g_MyInfo.GetPublicID() )
	{
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), kUser.m_szUserID.c_str() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );
		g_ChatMgr.SetChatComplexString( STR(3), kPrinter );
	}
}

void ioLadderTeamMgr::LeaveUser( ioHashString szUserID )
{
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
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
	
	if( GetKickOutUser() != szUserID )
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
	else
	{
		SetKickOutUser( "" );
	}
}

bool ioLadderTeamMgr::IsHaveUser( ioHashString &rszUserID )
{
	if( !IsLadderTeam() ) return false;

	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == rszUserID )
			return true;
	}
	return false;
}

const LadderTeamUserInfo &ioLadderTeamMgr::GetUserInfo( int iArray )
{
	int iCharCnt = m_vUserList.size();
	if( COMPARE( iArray, 0, iCharCnt ) )
		return m_vUserList[iArray];
	static LadderTeamUserInfo sgTemp;
	return sgTemp;
}

int ioLadderTeamMgr::GetUserCount()
{
	return m_vUserList.size();
}

bool ioLadderTeamMgr::ApplyLadderTeamInfo( SP2Packet &rkPacket )
{
	// 내 길드 정보
	DWORD dwBackupIndex = m_LadderTeam.m_dwTeamIndex;
	rkPacket >> m_LadderTeam.m_dwTeamIndex >> m_LadderTeam.m_iCampType >> m_LadderTeam.m_szTeamName >> m_LadderTeam.m_szTeamPW >> m_LadderTeam.m_szOwnerName >> m_LadderTeam.m_iMaxPlayer;
	rkPacket >> m_LadderTeam.m_iWinRecord >> m_LadderTeam.m_iLoseRecord >> m_LadderTeam.m_iVictoriesRecord;

	bool bFirstEnter = false;
	if( m_LadderTeam.m_dwTeamIndex != dwBackupIndex )
	{
		g_FriendManager.SetUpdate();
		bFirstEnter = true;
	}
	g_App.CloseGuildMarkPage();
	return bFirstEnter;
}

ioHashString ioLadderTeamMgr::ApplyJoinUserInfo( SP2Packet &rkPacket )
{
	DWORD dwGuildIndex, dwGuildMark;
	LadderTeamUserInfo kInfo;
	rkPacket >> kInfo.m_szUserID >> kInfo.m_iGradeLevel >> kInfo.m_iAbilityLevel >> kInfo.m_iLadderPoint;
	rkPacket >> kInfo.m_dwUserIndex >> kInfo.m_szPublicIP >> kInfo.m_iPort >> kInfo.m_szPrivateIP >> kInfo.m_szTransferIP >> kInfo.m_iTransferPort;
	rkPacket >> dwGuildIndex >> dwGuildMark;
	JoinUser( kInfo );

	g_GuildInfo.SetGuildMark( kInfo.m_szUserID, dwGuildIndex, dwGuildMark );
	m_LadderTeam.m_iCurPlayer = GetUserCount();

	return kInfo.m_szUserID;
}

ioHashString ioLadderTeamMgr::ApplyLeaveUserInfo( SP2Packet &rkPacket )
{
	ioHashString szLeaveUser, szOwnerUser;
	rkPacket >> szLeaveUser >> szOwnerUser;
	LeaveUser( szLeaveUser );		

	m_LadderTeam.m_szOwnerName = szOwnerUser;
	m_LadderTeam.m_iCurPlayer = GetUserCount();
	ChangeMyOwner();	

	return szLeaveUser;
}

void ioLadderTeamMgr::UpdateRecord( TeamType ePlayTeam, TeamType eWinTeam )
{
	if( !IsLadderTeam() ) return;
	if( eWinTeam == TEAM_NONE ) return;
	
	if( eWinTeam == ePlayTeam )
	{
		// 우리팀 승
		m_LadderTeam.m_iWinRecord++;
		m_LadderTeam.m_iVictoriesRecord++;		

		// 상대팀 패
		m_OtherTeam.m_iLoseRecord++;
		m_OtherTeam.m_iVictoriesRecord = 0;	
	}
	else
	{
		// 우리팀 패
		m_LadderTeam.m_iLoseRecord++;
		m_LadderTeam.m_iVictoriesRecord = 0;		

		// 상대팀 승
		m_OtherTeam.m_iWinRecord++;
		m_OtherTeam.m_iVictoriesRecord++;	
	}
}

const LadderTeamInfo &ioLadderTeamMgr::GetLadderTeamInfo()
{
	return m_LadderTeam;
}

const LadderTeamInfo &ioLadderTeamMgr::GetLadderOtherTeamInfo()
{
	return m_OtherTeam;
}

const LadderTeamInfo &ioLadderTeamMgr::GetLadderBlueTeamInfo()
{
	if( (TeamType)m_OtherTeam.m_iTeamType == TEAM_BLUE )
		return m_OtherTeam;
	return m_LadderTeam;
}

const LadderTeamInfo &ioLadderTeamMgr::GetLadderRedTeamInfo()
{
	if( (TeamType)m_OtherTeam.m_iTeamType == TEAM_RED )
		return m_OtherTeam;
	return m_LadderTeam;
}

int ioLadderTeamMgr::GetLadderAverageLevel()
{
	m_LadderTeam.m_iAverageLevel = 0;
	int iSize = 0;
	int iLevel = 0;

	int iCharCnt = m_vUserList.size();
	int i = 0;
	for( i=0; i < iCharCnt; i++ )
	{
		if( g_App.IsAdminID( m_vUserList[i].m_szUserID.c_str() ) ) continue;

		iLevel += m_vUserList[i].m_iAbilityLevel;
		iSize++;
	}

	if( iSize <= 0 )
	{
		return 0;
	}

	m_LadderTeam.m_iAverageLevel = iLevel / iSize;
	m_LadderTeam.m_iAverageLevel = min( max( m_LadderTeam.m_iAverageLevel - g_LevelMgr.GetAddGradeLevel(), 0 ), g_LevelMgr.GetMaxGradeLevel() );
	return m_LadderTeam.m_iAverageLevel;
}

void ioLadderTeamMgr::SetMaxPlayer( int iMaxPlayer )
{
	m_LadderTeam.m_iMaxPlayer = iMaxPlayer;
}

void ioLadderTeamMgr::SetLadderOtherTeamName( DWORD dwTeamIndex, const ioHashString &szName )
{
	if( !IsLadderTeam() ) return;

	if( m_OtherTeam.m_dwTeamIndex == dwTeamIndex )
		m_OtherTeam.m_szTeamName = szName;
}

void ioLadderTeamMgr::SetLadderOtherTeamName( const ioHashString &szName )
{
	if( !IsLadderTeam() ) return;

	m_OtherTeam.m_szTeamName = szName;
}

DWORD ioLadderTeamMgr::GetGuildIndex()
{
	if( !IsGuildTeam() ) return 0;
	if( IsHeroMatchMode() ) return 0;

	return m_LadderTeam.m_dwGuildIndex;
}

DWORD ioLadderTeamMgr::GetGuildMark()
{
	if( !IsGuildTeam() ) return 0;
	if( IsHeroMatchMode() ) return 0;

	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( GetGuildIndex() );
	if( !pGuildData )
		return 0;

	return pGuildData->GetGuildMark();
}

void ioLadderTeamMgr::PlaySearchSoundSec( int iSec )
{
	SearchSecSoundMap::iterator iter = m_SearchSecSndMap.find( iSec );
	if( iter != m_SearchSecSndMap.end() )
	{
		ioHashString &rkSound = iter->second;
		if( !rkSound.IsEmpty() )
			g_SoundMgr.PlaySound( rkSound );
	}
	else if( !m_szSearchSecSndDef.IsEmpty() )
		g_SoundMgr.PlaySound( m_szSearchSecSndDef );
}

void ioLadderTeamMgr::SetSelectModeNMap( int iModeSelect, int iMapSelect )
{
	m_iSelectModeIndex = iModeSelect;
	m_iSelectMapIndex  = iMapSelect;

	m_bHeroMatchMode   = g_ModeSelectInfo.IsLadderTeamHeroMatchMode( m_iSelectModeIndex );
}

void ioLadderTeamMgr::FillOtherTeamInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_OtherTeam.m_dwTeamIndex >> m_OtherTeam.m_iCampType >> m_OtherTeam.m_iTeamType;
	rkPacket >> m_OtherTeam.m_szTeamName >> m_OtherTeam.m_dwGuildIndex;
	rkPacket >> m_OtherTeam.m_iWinRecord >> m_OtherTeam.m_iLoseRecord >> m_OtherTeam.m_iVictoriesRecord;
	rkPacket >> m_OtherTeam.m_iTeamRanking;

	// 길드팀이면 길드 정보 요청
	if( m_OtherTeam.m_dwGuildIndex != 0 )
	{
		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( m_OtherTeam.m_dwGuildIndex, true );   //길드정보가 없으면 가지고 온다.
		if( pGuildData )
		{
			//없을 때 정보 가지고 오는게 목적임
		}
	}
}

void ioLadderTeamMgr::ApplyHeroMatchOtherInfo( SP2Packet &rkPacket )
{
	if( !IsLadderTeam() || !IsHeroMatchMode() ) return;

	m_HeroMatchOtherInfo.Initialize();

	rkPacket >> m_HeroMatchOtherInfo.m_szUserID >> m_HeroMatchOtherInfo.m_iGradeLevel;
	rkPacket >> m_HeroMatchOtherInfo.m_iHeroTodayRank >> m_HeroMatchOtherInfo.m_iHeroTitle;
	rkPacket >> m_HeroMatchOtherInfo.m_iHeroWin >> m_HeroMatchOtherInfo.m_iHeroLose;

	m_HeroMatchOtherInfo.m_CharInfo.ApplyData( rkPacket );
	rkPacket >> m_HeroMatchOtherInfo.m_iClassLevel;
	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		rkPacket >> m_HeroMatchOtherInfo.m_EquipItem[i].m_item_code;
		rkPacket >> m_HeroMatchOtherInfo.m_EquipItem[i].m_item_reinforce;
		rkPacket >> m_HeroMatchOtherInfo.m_EquipItem[i].m_item_male_custom;
		rkPacket >> m_HeroMatchOtherInfo.m_EquipItem[i].m_item_female_custom;
	}

	int iOtherCampType = CAMP_BLUE;
	//if( g_MyInfo.GetUserCampPos() == CAMP_BLUE )
		//iOtherCampType = CAMP_RED;
	if( m_iOtherCampType != CAMP_NONE )
		iOtherCampType = m_iOtherCampType;

	int iOtherTeamType = TEAM_BLUE;
	iOtherTeamType = m_iOtherTeamType;

	g_CampMgr.SetHeroMatchInfo( m_HeroMatchOtherInfo.m_iHeroTodayRank, m_HeroMatchOtherInfo.m_iHeroTitle,
								m_HeroMatchOtherInfo.m_iGradeLevel, iOtherCampType, iOtherTeamType, m_HeroMatchOtherInfo.m_szUserID );
}

const ioHashString &ioLadderTeamMgr::GetRandomName()
{
	int iSize = m_ReserveTeamName.size();
	if( iSize == 0 )
	{
		static ioHashString szError = STR(1);
		return szError;
	}

	int r = rand()%iSize;
	return m_ReserveTeamName[r];
}

DWORD ioLadderTeamMgr::GetUserCharIndex( const ioHashString &rkUserID )
{
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		LadderTeamUserInfo &rkUserInfo = m_vUserList[i];
		if( rkUserInfo.m_szUserID == rkUserID )
		{
			return rkUserInfo.m_dwCharIndex;
		}
	}
	return 0;
}

DWORD ioLadderTeamMgr::GetUserCharMotion( const ioHashString &rkUserID )
{
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		LadderTeamUserInfo &rkUserInfo = m_vUserList[i];
		if( rkUserInfo.m_szUserID == rkUserID )
		{
			return rkUserInfo.m_dwCharMotionItem;
		}
	}
	return 0;
}

void ioLadderTeamMgr::StartChatBubble( const char *szID, const char *szChat, bool bWholeChat )
{
	if( !IsLadderTeam() ) return;

	LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
	if( !IsHeroMatchMode() && pLadderBattleWnd && pLadderBattleWnd->IsShow() )
	{
		pLadderBattleWnd->StartChatBubble( szID, szChat, bWholeChat );
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

void ioLadderTeamMgr::ProcessLadderTeam()
{
	if( !IsLadderTeam() ) 
		return;
	
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		LadderTeamUserInfo &rkUserInfo = m_vUserList[i];
		if( rkUserInfo.m_szUserID == g_MyInfo.GetPublicID() ) continue;

		// 캐릭터 모션 동기화
		if( rkUserInfo.m_dwCharMotionFirstSyncTime > 0 )
		{
			if( FRAMEGETTIME() - rkUserInfo.m_dwCharMotionFirstSyncTime > 4000 )
			{
				rkUserInfo.m_dwCharMotionFirstSyncTime = FRAMEGETTIME();

				SP2Packet kPacket( CUPK_ETCITEM_MOTION_OPTION_FIRST_SYNC );
				kPacket << g_MyInfo.GetPublicID() << ioEtcItemMotion::MOTION_OPTION_6 << GetUserCharMotion( g_MyInfo.GetPublicID() );
				P2PNetwork::SendToUser( rkUserInfo.m_szUserID, kPacket );
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
				P2PNetwork::SendToUser( rkUserInfo.m_szUserID, kPacket );				
			}
		}
	}
}

void ioLadderTeamMgr::SetOtherTypeBeforeLoading( int iCampType, int iTeamType )
{
	 m_iOtherCampType = iCampType;
	 m_iOtherTeamType = iTeamType;
}
