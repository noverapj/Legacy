#include "stdafx.h"
#include "ioShuffleRoomMgr.h"
#include "GUI/VoiceChatWnd.h"
#include "GUI/PartyVersusWnd.h"
#include "GUI/TotalResultMainWnd.h"

template<> ioShuffleRoomMgr* Singleton< ioShuffleRoomMgr >::ms_Singleton = 0;

ioShuffleRoomMgr::ioShuffleRoomMgr()
{
	m_nModeMaxCount				= 0;

	m_ShuffleRoom.m_dwIndex		= 0;
	m_iDefaultMaxPlayer			= MAX_PLAYER;
	m_bShuffleModePlaying		= true;
	m_iLimitJoinLevel			= 2;

	m_iModeStartPlayerCount		= 0;
	m_dwProcessTimer			= 0;

	InitRecord();
	InitPhaseInfo();
	InitShuffleCancelCount();
	InitGlobalSearchingShuffleCount();
}

ioShuffleRoomMgr::~ioShuffleRoomMgr()
{	
	m_vKickOutRoom.clear();
	m_vUserList.clear();
}

ioShuffleRoomMgr& ioShuffleRoomMgr::GetSingleton()
{
	return Singleton< ioShuffleRoomMgr >::GetSingleton();
}

void ioShuffleRoomMgr::LoadShuffleRoomInfo()
{
	ioINILoader_e kLoader( "config/party_config.ini" );
	kLoader.SetTitle_e( "info" );	
	m_iDefaultMaxPlayer = kLoader.LoadInt_e( "Default_MaxPlayer", MAX_PLAYER );
	m_iLimitJoinLevel   = kLoader.LoadInt_e( "Limit_JoinLevel", 2 );

	char szKey[MAX_PATH], szBuf[MAX_PATH];
	kLoader.LoadString_e( "BlueTeamEmoticon", "", szBuf, MAX_PATH );
	m_szBlueTeamEmoticon = szBuf;
	kLoader.LoadString_e( "RedTeamEmoticon", "", szBuf, MAX_PATH );
	m_szRedTeamEmoticon = szBuf;

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

	ioINILoader_e kShuffleLoader( "config/mode/ShuffleMode/sp2_shuffleroom.ini");
	kShuffleLoader.SetTitle( "common" );
	m_nModeMaxCount = kShuffleLoader.LoadInt( "max_mode", 1 );

	kShuffleLoader.SetTitle_e( "TodayMode" );
	for( int i = 0; i < 7; ++i )
	{
		for( int j = 0; j <m_nModeMaxCount; ++j )
		{
			wsprintf_e( szKey, "todaymode%d_mode%d", i+1, j+1 );
			m_TodayModeInfo[i].push_back( kShuffleLoader.LoadInt( szKey, 1 ) );
		}
	}

	kShuffleLoader.SetTitle_e( "matching_table" );
	int iCount = kShuffleLoader.LoadInt( "match_minmax_level_cnt", 0 );
	for( int i=0; i<iCount; ++i )
	{
		MatchConditionLevel Level;
		wsprintf( szKey, "match%d_user_min_level", i+1 );		
		Level.m_iUserMinLevel = kShuffleLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "match%d_user_max_level", i+1 );
		Level.m_iUserMaxLevel = kShuffleLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "match%d_condition_count", i+1 );
		int iCount = kShuffleLoader.LoadInt( szKey, 0 );

		if( 0 < iCount )
			Level.m_vMatchConditionValue.reserve( iCount );

		for( int j = 0; j < iCount; ++j )
		{
			MatchConditionValue MatchCondition;

			wsprintf( szKey, "match%d_condition%d_min", i+1, j+1 );
			MatchCondition.m_iMatchMinLevel = kShuffleLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "match%d_condition%d_max", i+1, j+1 );
			MatchCondition.m_iMatchMaxLevel = kShuffleLoader.LoadInt( szKey, 0 );

			Level.m_vMatchConditionValue.push_back( MatchCondition );
		}

		m_vMatchConditionLevel.push_back( Level );
	}

	kShuffleLoader.SetTitle_e( "kick_out_table" );
	m_dwKickVoteEnableTime = kShuffleLoader.LoadInt( "kick_out_enable_time", 0 );

	kShuffleLoader.SetTitle_e( "tooltip" );
	for( int i = 0; i < 8; ++i )
	{
		wsprintf( szKey, "tooltip%d", i + 1 );
		kShuffleLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString szTooltip = szBuf;

		m_Toolip.insert( std::map< int, ioHashString >::value_type( i, szTooltip ) );
	}
}

void ioShuffleRoomMgr::InitRecord()
{
	m_ShuffleRoom.m_iBlueWin = 0;
	m_ShuffleRoom.m_iBlueLose = 0;
	m_ShuffleRoom.m_iBlueVictories = 0;
	m_ShuffleRoom.m_iRedWin = 0;
	m_ShuffleRoom.m_iRedLose = 0;
	m_ShuffleRoom.m_iRedVictories = 0;
}

void ioShuffleRoomMgr::InitPhaseInfo()
{
	m_iPhase = 0;
	m_iShuffleStarCnt            = 0;
	m_iShuffleStarCntByCalcBonus = 0;
	m_fShuffleBonusPercent       = 0.0f;

	for( int i=0; i<SHUFFLE_PHASE_MAX; ++i )
	{
		m_szModeName[i] = "???";
	}
}

void ioShuffleRoomMgr::UserListClear()
{
	m_vUserList.clear();
}

int ioShuffleRoomMgr::GetAverageLevel()
{
	int iSize = 0;
	int iLevel = 0;
	int iObserverSize = 0;
	int iObserverLevel = 0;

	int iCharCnt = m_vUserList.size();
	int i = 0;
	for( i=0; i < iCharCnt; i++ )
	{
		if( g_App.IsAdminID( m_vUserList[i].m_szUserID.c_str() ) )
			continue;

		iLevel += m_vUserList[i].m_iAbilityLevel;
		iSize++;
	}

	iSize = max( iSize, 1 );
	iLevel /= iSize;
	iLevel = min( max( iLevel - g_LevelMgr.GetAddGradeLevel(), 0 ), g_LevelMgr.GetMaxGradeLevel() );
	return iLevel;
}

bool ioShuffleRoomMgr::IsTeamEmoticon()
{
	if( !IsShuffleRoom() )
		return false;

	return false;
}

bool ioShuffleRoomMgr::IsShuffleRoom()
{
	if( m_ShuffleRoom.m_dwIndex == 0 )
		return false;

	return true;
}

void ioShuffleRoomMgr::SetKickOutRoom( DWORD dwIndex )
{
	KickOutRoom kor;
	kor.m_dwIndex        = dwIndex;
	kor.m_dwKickOutTimer = FRAMEGETTIME();
	m_vKickOutRoom.push_back( kor );
}

bool ioShuffleRoomMgr::IsKickOutRoom( DWORD dwIndex )
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

void ioShuffleRoomMgr::SetKickOutUser( ioHashString szName )
{
	m_szKickOutUser = szName;
	int iTeam = GetShuffleUserTeam( szName );

	ioComplexStringPrinter kPrinter;
	g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );		

	if( iTeam == TEAM_RED )
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szName.c_str() );
	kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
	kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "님, 강제퇴장 됨." );
	g_ChatMgr.SetChatComplexString( "정보", kPrinter );
}

void ioShuffleRoomMgr::LeaveRoom( bool bMove /* = false */ )
{
	InitShuffleCancelCount();
	InitGlobalSearchingShuffleCount();

	if( m_ShuffleRoom.m_dwIndex != 0 )
	{
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );	
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "전투방에서 이탈." );
		g_ChatMgr.SetChatComplexString( "정보", kPrinter );
	}

	m_bShuffleModePlaying   = true;
	m_ShuffleRoom.m_dwIndex = 0;

	m_szPrevMyOwnerID.Clear();

	InitRecord();
	InitPhaseInfo();
	UserListClear();
	g_FriendManager.SetUpdate();

	if( !bMove )
	{
		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
		{
			if( ioMyRoomMode::GetMyRoomType() == MRT_SHUFFLE )
			{				
				if( ioPlayMode::GetModeType() == MT_MYROOM )
				{
					g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
					g_App.SetModeType( 0, 0, 0, PT_NONE );
				}
			}
			if( ioMyRoomMode::GetMyRoomType() != MRT_RANKBATTLE )
				ioMyRoomMode::SetMyRoomType( MRT_LOBBY );
		}
	}
}

void ioShuffleRoomMgr::ChangeMyOwner()
{
	if( !m_szPrevMyOwnerID.IsEmpty() && !m_ShuffleRoom.m_szOwnerName.IsEmpty() )
	{
		if( m_szPrevMyOwnerID != m_ShuffleRoom.m_szOwnerName )
		{
			g_ChatMgr.SetHostChange( "전투", m_ShuffleRoom.m_szOwnerName.c_str(), TEAM_NONE );
		}
	}
	m_szPrevMyOwnerID = m_ShuffleRoom.m_szOwnerName;
}

void ioShuffleRoomMgr::UpdateUserLevel( const ioHashString &szUserID, int iGradeLevel )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			m_vUserList[i].m_iGradeLevel = iGradeLevel;
		}
	}
}

void ioShuffleRoomMgr::UpdateUserAbilityLevel( const ioHashString &szUserID, int iAbilityLevel )
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

void ioShuffleRoomMgr::UpdateUserCharIndex( const ioHashString &szUserID, DWORD dwCharIndex, bool bSync )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		ShuffleRoomUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == szUserID )
		{
			rkUserData.m_dwCharIndex = dwCharIndex;
			if( bSync )
			{
				rkUserData.m_dwCharIndexFirstSyncTime = 0;
			}

			return;
		}
	}
}

void ioShuffleRoomMgr::UpdateUserCharMotion( const ioHashString &szUserID, DWORD dwCharMotion )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		ShuffleRoomUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == szUserID )
		{
			rkUserData.m_dwCharMotionItem = dwCharMotion;
			rkUserData.m_dwCharMotionFirstSyncTime = 0;

			return;
		}
	}
}

void ioShuffleRoomMgr::JoinUser( ShuffleRoomUserInfo &kUser )
{
	for( int i=0 ; i<(int)m_vUserList.size() ; i++ )
	{
		ShuffleRoomUserInfo &rkUserData = m_vUserList[i];
		if( rkUserData.m_szUserID == kUser.m_szUserID )
		{
			rkUserData.m_dwUserIndex    = kUser.m_dwUserIndex;
			rkUserData.m_iGradeLevel	= kUser.m_iGradeLevel;
			rkUserData.m_iAbilityLevel  = kUser.m_iAbilityLevel;
			rkUserData.m_iTeam			= kUser.m_iTeam;		
			rkUserData.m_szPublicIP     = kUser.m_szPublicIP;
			rkUserData.m_szPrivateIP    = kUser.m_szPrivateIP;
			rkUserData.m_iPort          = kUser.m_iPort;	
			rkUserData.m_szTransferIP   = kUser.m_szTransferIP;
			rkUserData.m_iTransferPort  = kUser.m_iTransferPort;

			// 자기 자신의 몬스터 코인 & 캐릭터 인덱스 세팅
			if( rkUserData.m_szUserID == g_MyInfo.GetPublicID() )
			{
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
		kUser.m_dwCharIndexFirstSyncTime = FRAMEGETTIME();
	}

	m_vUserList.push_back( kUser );
	SetCurJoiner( m_vUserList.size() );
	
	if( kUser.m_szUserID == g_MyInfo.GetPublicID() )
	{
		g_MyInfo.SetMyAddress( kUser.m_szPublicIP, kUser.m_iPort );
	}
	else if( kUser.m_szUserID != g_MyInfo.GetPublicID() )
	{
		g_App.AddNetworkTable( kUser.m_dwUserIndex, kUser.m_szUserID, kUser.m_szPublicIP, kUser.m_szPrivateIP, kUser.m_szTransferIP,
							   kUser.m_iPort, kUser.m_iTransferPort, P2P_STYLE_VOICE, 2 );

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

	if( kUser.m_szUserID != g_MyInfo.GetPublicID() )
	{
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%s", kUser.m_szUserID.c_str() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "님, 전투방에 참가." );
		g_ChatMgr.SetChatComplexString( "정보", kPrinter );
	}
}

void ioShuffleRoomMgr::LeaveUser( ioHashString szUserID, ModeType eModeType )
{
	int iTeam = TEAM_NONE;
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szUserID )
		{
			iTeam = m_vUserList[i].m_iTeam;
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
		if( GetKickOutUser() != szUserID )
		{
			if( szUserID != g_MyInfo.GetPublicID() )
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%s", szUserID.c_str() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "님, 전투방 이탈." );
				g_ChatMgr.SetChatComplexString( "정보", kPrinter );
			}
		}	
	}
}

bool ioShuffleRoomMgr::IsHaveUser( ioHashString &rszUserID )
{
	if( !IsShuffleRoom() ) return false;

	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == rszUserID )
			return true;
	}
	return false;
}

const ShuffleRoomUserInfo &ioShuffleRoomMgr::GetUserInfo( int iArray )
{
	int iCharCnt = m_vUserList.size();
	if( COMPARE( iArray, 0, iCharCnt ) )
		return m_vUserList[iArray];
	static ShuffleRoomUserInfo sgTemp;
	return sgTemp;
}

void ioShuffleRoomMgr::SetShuffleTeam( ioHashString szName, TeamType eTeam )
{
	// 블루팀 / 레드팀 둘중하나여야한다.
	if( eTeam != TEAM_BLUE && eTeam != TEAM_RED && eTeam != TEAM_NONE )
		return;

	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( m_vUserList[i].m_szUserID == szName )
		{
			m_vUserList[i].m_iTeam = eTeam;

			break;
		}
	}
}

int ioShuffleRoomMgr::GetShuffleUserTeam( ioHashString szName )
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

int ioShuffleRoomMgr::GetShuffleTeamAverageLevel( TeamType eTeam )
{
	int iAverageLevel = 0;
	int iTeamCount    = 0;
	int iCharCnt = m_vUserList.size();
	for(int i = 0;i < iCharCnt;i++)
	{
		if( g_App.IsAdminID( m_vUserList[i].m_szUserID.c_str() ) ) continue;

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

const ShuffleRoomUserInfo &ioShuffleRoomMgr::GetShuffleUserInfo( TeamType eTeam, int iArray )
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

	static ShuffleRoomUserInfo sgTemp;
	return sgTemp;
}

int ioShuffleRoomMgr::GetShuffleTeamUserCount( TeamType eTeam )
{
	int iCount = 0;
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		if( eTeam != TEAM_NONE && m_vUserList[i].m_iTeam == eTeam )
		{
			iCount++;
		}		
	}

	return iCount;
}

void ioShuffleRoomMgr::UpdateRecord( TeamType eWinTeam )
{
	if( !IsShuffleRoom() ) return;

 	if( eWinTeam == TEAM_BLUE )
	{
		m_ShuffleRoom.m_iBlueWin++;
		m_ShuffleRoom.m_iBlueVictories++;

		m_ShuffleRoom.m_iRedLose++;
		m_ShuffleRoom.m_iRedVictories = 0;
	}
	else if( eWinTeam == TEAM_RED )
	{
		m_ShuffleRoom.m_iRedWin++;
		m_ShuffleRoom.m_iRedVictories++;

		m_ShuffleRoom.m_iBlueLose++;
		m_ShuffleRoom.m_iBlueVictories = 0;
	}
	else if( eWinTeam == TEAM_NONE )
	{
		m_ShuffleRoom.m_iBlueVictories = 0;
		m_ShuffleRoom.m_iRedVictories = 0;
	}
}

void ioShuffleRoomMgr::GetShuffleRoomInfo( ShuffleRoomInfo *pShuffleRoomInfo )
{
	pShuffleRoomInfo->m_dwIndex		  = m_ShuffleRoom.m_dwIndex;
	pShuffleRoomInfo->m_szOwnerName    = m_ShuffleRoom.m_szOwnerName;
	pShuffleRoomInfo->m_szName		  = m_ShuffleRoom.m_szName;
	pShuffleRoomInfo->m_iCurJoiner     = m_ShuffleRoom.m_iCurJoiner;
	pShuffleRoomInfo->m_iCurPlayer     = GetCurPlayer();		// 이걸로... 09.02.23 KDH
	pShuffleRoomInfo->m_iMaxPlayerBlue = m_ShuffleRoom.m_iMaxPlayerBlue;
	pShuffleRoomInfo->m_iMaxPlayerRed  = m_ShuffleRoom.m_iMaxPlayerRed;
	pShuffleRoomInfo->m_iAverageLevel  = m_ShuffleRoom.m_iAverageLevel;	
	pShuffleRoomInfo->m_iBlueWin		  = m_ShuffleRoom.m_iBlueWin;
	pShuffleRoomInfo->m_iBlueLose	  = m_ShuffleRoom.m_iBlueLose;
	pShuffleRoomInfo->m_iBlueVictories = m_ShuffleRoom.m_iBlueVictories;
	pShuffleRoomInfo->m_iRedWin		  = m_ShuffleRoom.m_iRedWin;
	pShuffleRoomInfo->m_iRedLose		  = m_ShuffleRoom.m_iRedLose;
	pShuffleRoomInfo->m_iRedVictories  = m_ShuffleRoom.m_iRedVictories;
}

DWORD ioShuffleRoomMgr::GetUserCharIndex( const ioHashString &rkUserID )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const ShuffleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_szUserID == rkUserID )
		{
			return rkInfo.m_dwCharIndex;
		}
	}
	return 0;
}

DWORD ioShuffleRoomMgr::GetUserCharMotion( const ioHashString &rkUserID )
{
	for(vUserInfo::const_iterator iter = m_vUserList.begin(); iter != m_vUserList.end(); ++iter)
	{
		const ShuffleRoomUserInfo &rkInfo = (*iter);
		if( rkInfo.m_szUserID == rkUserID )
		{
			return rkInfo.m_dwCharMotionItem;
		}
	}
	return 0;
}

void ioShuffleRoomMgr::PlayStartSound( int iCurSec )
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

const ioHashString &ioShuffleRoomMgr::GetTeamEmoticon( const ioHashString &rkName )
{
	TeamType eTeam = (TeamType)GetShuffleUserTeam( rkName );
	if( eTeam == TEAM_BLUE )
		return m_szBlueTeamEmoticon;
	return m_szRedTeamEmoticon;
}

void ioShuffleRoomMgr::ProcessShuffleRoom()
{
	if( !IsShuffleRoom() ) 
		return;

	//
	enum{ TICK_TIME = 1000, };
	if( FRAMEGETTIME() - m_dwProcessTimer > TICK_TIME )
	{
		m_dwProcessTimer = FRAMEGETTIME();
	}

	//
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ShuffleRoomUserInfo &rkUserInfo = m_vUserList[i];
		if( rkUserInfo.m_szUserID == g_MyInfo.GetPublicID() ) continue;
		
		// 캐릭터 모션 동기화
		if( rkUserInfo.m_dwCharMotionFirstSyncTime > 0 )
		{
			if( FRAMEGETTIME() - rkUserInfo.m_dwCharMotionFirstSyncTime > 4000 )
			{
				rkUserInfo.m_dwCharMotionFirstSyncTime = FRAMEGETTIME();

				SP2Packet kPacket( CUPK_ETCITEM_MOTION_OPTION_FIRST_SYNC );
				kPacket << g_MyInfo.GetPublicID() << ioEtcItemMotion::MOTION_OPTION_5 << GetUserCharMotion( g_MyInfo.GetPublicID() );
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

void ioShuffleRoomMgr::ModeStartPlayerCount()
{
	m_iModeStartPlayerCount = 0;
	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		m_iModeStartPlayerCount++;
	}
}

int ioShuffleRoomMgr::GetTodayModeType( int iWeekOfDay, int iArray )
{
	if( COMPARE( iWeekOfDay, 0, 7 ) )
	{
		int iSize = static_cast<int>( m_TodayModeInfo[iWeekOfDay].size() );
		if( COMPARE( iArray, 0, iSize ) )
		{
			return m_TodayModeInfo[iWeekOfDay][iArray];
		}
	}

	return MT_NONE;
}

void ioShuffleRoomMgr::StartChatBubble( const char *szID, const char *szChat, bool bWholeChat )
{
	if( !IsShuffleRoom() ) return;

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

void ioShuffleRoomMgr::SetShuffleCancelCount()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsLimitStartCancel() )
		m_iShuffleCancelCount++;
	else
		m_iShuffleCancelCount = 0;
}

void ioShuffleRoomMgr::SendShuffleRoomP2P( SP2Packet &rkPacket )
{
	if( !IsShuffleRoom() ) 
		return;

	int iCharCnt = m_vUserList.size();
	for( int i=0 ; i<iCharCnt ; i++ )
	{
		ShuffleRoomUserInfo &rkUserInfo = m_vUserList[i];
		if( rkUserInfo.m_szUserID == g_MyInfo.GetPublicID() ) continue;

		P2PNetwork::SendToUser( rkUserInfo.m_szUserID, rkPacket );
	}
}

void ioShuffleRoomMgr::SendShuffleRoomAnn( const char *szChat )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	SP2Packet kPacket( CUPK_CHAT );
	kPacket << CT_ANNOUNCE;
	kPacket << g_MyInfo.GetPublicID();
	kPacket << szChat;

	SendShuffleRoomP2P( kPacket );
}

bool ioShuffleRoomMgr::ApplyShuffleRoomInfo( SP2Packet &rkPacket, bool &rShuffleRoom )
{
	DWORD dwNewShuffleIndex;
	rkPacket >> dwNewShuffleIndex;

	if( IsShuffleRoom() )
	{
		if( m_ShuffleRoom.m_dwIndex != dwNewShuffleIndex )
		{
			LeaveRoom( true );
		}
	}

	DWORD dwBackupIndex    = m_ShuffleRoom.m_dwIndex;
	m_ShuffleRoom.m_dwIndex = dwNewShuffleIndex;
	rkPacket >> m_ShuffleRoom.m_szName >> m_ShuffleRoom.m_szOwnerName >> m_ShuffleRoom.m_iCurJoiner >> m_ShuffleRoom.m_iCurPlayer
			 >> m_ShuffleRoom.m_iMaxPlayerBlue >> m_ShuffleRoom.m_iMaxPlayerRed >> m_ShuffleRoom.m_iAverageLevel >> m_iPhase
			 >> rShuffleRoom;

	bool bFirstEnter = false;

	if( m_ShuffleRoom.m_dwIndex != dwBackupIndex )
	{
		g_FriendManager.SetUpdate();
		bFirstEnter = true;
	}
	ChangeMyOwner();

	//전투방 입장하면 길드마크 변경 웹페이지 닫는다.
	g_App.CloseGuildMarkPage();

	return bFirstEnter;
}

ioHashString ioShuffleRoomMgr::ApplyJoinUserInfo( SP2Packet &rkPacket )
{
	DWORD dwGuildIndex, dwGuildMark;
	ShuffleRoomUserInfo kInfo;
	rkPacket >> m_ShuffleRoom.m_iAverageLevel >> kInfo.m_szUserID >> kInfo.m_iGradeLevel >> kInfo.m_iAbilityLevel >> kInfo.m_iTeam;
	rkPacket >> kInfo.m_dwUserIndex >> kInfo.m_szPublicIP >> kInfo.m_iPort >> kInfo.m_szPrivateIP >> kInfo.m_szTransferIP >> kInfo.m_iTransferPort;
	rkPacket >> dwGuildIndex >> dwGuildMark;
	JoinUser( kInfo );

	g_GuildInfo.SetGuildMark( kInfo.m_szUserID, dwGuildIndex, dwGuildMark );

	return kInfo.m_szUserID;
}

ioHashString ioShuffleRoomMgr::ApplyLeaveUserInfo( SP2Packet &rkPacket, ModeType eModeType )
{
	ioHashString szUserID;
	rkPacket >> szUserID >> m_ShuffleRoom.m_iAverageLevel;
	LeaveUser( szUserID, eModeType );

	SetCurJoiner( m_vUserList.size() );
	return szUserID;
}

void ioShuffleRoomMgr::ApplyShuffleRoomRecord( SP2Packet &rkPacket )
{
	if( !IsShuffleRoom() ) return;

	rkPacket >> m_ShuffleRoom.m_iBlueWin >> m_ShuffleRoom.m_iBlueLose >> m_ShuffleRoom.m_iBlueVictories;
	rkPacket >> m_ShuffleRoom.m_iRedWin >> m_ShuffleRoom.m_iRedLose >> m_ShuffleRoom.m_iRedVictories;
}

void ioShuffleRoomMgr::ApplyShufflePhase( SP2Packet &rkPacket )
{
	InitPhaseInfo();

	int iCurPhase;
	rkPacket >> iCurPhase;
	SetPhase( iCurPhase );

	int iSize;
	rkPacket >> iSize;
	for( int i=0; i<iSize; ++i )
	{
		int iModeType;
		rkPacket >> iModeType;

		GetModeNameByType( iModeType, m_szModeName[i] );
	}
}

void ioShuffleRoomMgr::FillShuffleRoomInfo( SP2Packet &rkPacket )
{
	int iUserSize = m_vUserList.size();
	rkPacket << iUserSize;
	for(int i = 0;i < iUserSize;i++)
		rkPacket << m_vUserList[i].m_szUserID << m_vUserList[i].m_iTeam;
}

void ioShuffleRoomMgr::GetModeNameByType( IN int iType, OUT ioHashString &szModeName )
{
	switch( iType )
	{
	case MT_NONE:
	case MT_TRAINING:
	case MT_MONSTER_SURVIVAL:
	case MT_HEROMATCH:
	case MT_DUNGEON_A:
	case MT_HEADQUARTERS:
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_FACTORY:
	case MT_HOUSE:
	case MT_MYROOM:
	case MT_RAID:
		szModeName = "???";
		break;
	case MT_SYMBOL:
		szModeName = "파워스톤";
		break;
	case MT_CATCH:
		szModeName = "포로탈출";
		break;
	case MT_KING:
		szModeName = "히든크라운";
		break;
	case MT_SURVIVAL:
		szModeName = "데스매치";
		break;
	case MT_TEAM_SURVIVAL:
		szModeName = "팀데스매치";
		break;
	case MT_BOSS:
		szModeName = "보스레이드";
		break;
	case MT_FOOTBALL:
		szModeName = "축구경기";
		break;
	case MT_GANGSI:
		szModeName = "강시서바이벌";
		break;
	case MT_CATCH_RUNNINGMAN:
		szModeName = "드워프모드";
		break;
	case MT_FIGHT_CLUB:
		szModeName = "챔피언모드";
		break;
	case MT_DOUBLE_CROWN:
		szModeName = "더블크라운";
		break;
	case MT_SHUFFLE_BONUS:
		szModeName = "보너스스테이지";
		break;
	case MT_TEAM_SURVIVAL_AI:
		szModeName = "팀서바이벌AI";
		break;
	case MT_UNDERWEAR:
		szModeName = "팬티몬";
		break;
	case MT_CBT:
		szModeName = "클베모드";
		break;
	case MT_FLAG_CAPTURE:
		szModeName = "깃발모드";
		break;
	case MT_ARENA:
		szModeName = "아레나모드";
	case MT_FARMING:
		szModeName = "파밍모드";
		break;
	}
}

ioHashString &ioShuffleRoomMgr::GetModeName( int iPhase )
{
	if( m_iPhase < iPhase )
		return m_szModeName[SHUFFLE_PHASE_MAX-1];

	return m_szModeName[iPhase];
}

int ioShuffleRoomMgr::GetMatchConditionMaxCount( int iUserLevel )
{	
	for( vMatchConditionLevel::iterator iter = m_vMatchConditionLevel.begin(); iter != m_vMatchConditionLevel.end(); ++iter )
	{
		const MatchConditionLevel& rkLevel = *iter;

		if( COMPARE( iUserLevel, rkLevel.m_iUserMinLevel, rkLevel.m_iUserMaxLevel + 1 ) )
		{
			return static_cast<int>( rkLevel.m_vMatchConditionValue.size() );
		}
	}

	return 0;
}

ioHashString ioShuffleRoomMgr::GetMenuTooltip()
{
	int iDayOfWeek = 7;
#ifdef SHIPPING	
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );	
	iDayOfWeek = rkServerTime.wDayOfWeek;
#else
	SYSTEMTIME systime;
	GetSystemTime(&systime);
	iDayOfWeek = systime.wDayOfWeek;
#endif
	if( !COMPARE( iDayOfWeek, 0, 8 ) )
		iDayOfWeek = 7;

	return m_Toolip.find( iDayOfWeek )->second;
}