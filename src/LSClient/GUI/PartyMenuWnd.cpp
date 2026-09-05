

#include "StdAfx.h"

#include "../ioGuildMarkMgr.h"
#include "../ioTournamentManager.h"
#include "../ioLadderTeamMgr.h"
#include "../ioNoviceGradeBenefitManager.h"
#include "../ioMissionManager.h"
#include "../ioRankBattleManager.h"
#include "../ioShuffleRoomMgr.h"
#include "../ioExtendSoundManager.h"
#include "../ioDateHelp.h"

#include "MonsterCoinWnd.h"
#include "PartyMenuWnd.h"
#include "LobbyMainWnd.h"
#include "QuickLobbyWnd.h"
#include "TrainingFuncWnd.h"
#include "UserInfoToolTip.h"
#include "TimeGateSearch.h"
#include "TimeGateSearchShuffle.h"
#include "MissionWnd.h"
#include "NamedTitleBtn.h"

#include "../GameStage/Mode/Practice/IoPracticeManager.h"

PartyInfoBtn::PartyInfoBtn()
{
	m_pBlueNormal = NULL;
	m_pRedNormal = NULL;
	m_pNoneNormal = NULL;
	m_pBlueOver = NULL;
	m_pRedOver = NULL;
	m_pNoneOver = NULL;

	m_pPeopleOrange = NULL;
	m_pPeopleGray   = NULL;

	m_eTeam = TEAM_BLUE;
	m_iSlotArray = m_iMaxSlot = 0;
	m_iGradeLevel = m_iPingStep = 0;
}

PartyInfoBtn::~PartyInfoBtn()
{
	SAFEDELETE( m_pBlueNormal );
	SAFEDELETE( m_pRedNormal );
	SAFEDELETE( m_pNoneNormal );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedOver );
	SAFEDELETE( m_pNoneOver );

	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
}

void PartyInfoBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNormal" )
	{
		SAFEDELETE( m_pBlueNormal );
		m_pBlueNormal = pImage;
	}
	else if( szType == "RedNormal" )
	{
		SAFEDELETE( m_pRedNormal );
		m_pRedNormal = pImage;
	}
	else if( szType == "NoneNormal" )
	{
		SAFEDELETE( m_pNoneNormal );
		m_pNoneNormal = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else if( szType == "NoneOver" )
	{
		SAFEDELETE( m_pNoneOver );
		m_pNoneOver = pImage;
	}
	else if( szType == "PeopleOrange" )
	{
		SAFEDELETE( m_pPeopleOrange );
		m_pPeopleOrange = pImage;
	}
	else if( szType == "PeopleGray" )
	{
		SAFEDELETE( m_pPeopleGray );
		m_pPeopleGray = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void PartyInfoBtn::iwm_create()
{
	m_PingStep.Create();
}

void PartyInfoBtn::SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, int iPingStep, TeamType eTeam )
{
	m_iMaxSlot   = iMaxSlot;
	m_iSlotArray = iSlotArray;
	m_iPingStep  = iPingStep;
	m_szName     = rkName;
	m_eTeam      = eTeam;
	m_iGradeLevel= iGradeLevel;

	if( m_szName.IsEmpty() )
	{
		SetInActive();
	}
	else
	{	
		SetActive();
	}
}

void PartyInfoBtn::OnRenderBase( int iXPos, int iYPos )
{
	ioUIRenderImage *pNormal = NULL;
	ioUIRenderImage *pOver   = NULL;
	switch( m_eTeam )
	{
	case TEAM_BLUE:
		{
			pNormal = m_pBlueNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pBlueOver;
		}
		break;
	case TEAM_RED:
		{
			pNormal = m_pRedNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pRedOver;
		}
		break;
	default:
		{
			pNormal = m_pNoneNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pNoneOver;
		}
		break;
	}

	if( pNormal )
		pNormal->Render( iXPos, iYPos );
	if( pOver )
		pOver->Render( iXPos, iYPos );
}

void PartyInfoBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderBase( iXPos, iYPos );

	if( m_szName.IsEmpty() )
	{
		if( m_iSlotArray < m_iMaxSlot )
		{
			if( m_pPeopleOrange )
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
		}
		else
		{
			if( m_pPeopleGray )
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );			
		}
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );	
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );

			if( m_eTeam == TEAM_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else if( m_eTeam == TEAM_RED )
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 103.0f, m_szName.c_str() );
		int iRealNameSize = min( 103.0f, g_FontMgr.GetTextWidth( m_szName.c_str(), TS_NORMAL, FONT_SIZE_12 ) );
		DWORD dwGuildIndex, dwGuildMark;
		g_UserInfoMgr.GetGuildInfo( m_szName, dwGuildIndex, dwGuildMark );
		g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 23 + iRealNameSize + 3, iYPos + 4 );

		OnRenderPing( iXPos + 147, iYPos + 2 );
	}
}

void PartyInfoBtn::OnRenderPing( int iXPos, int iYPos )
{
	int  iPingStep = 0;
	bool bServerRelay = false;
	if( m_szName == g_MyInfo.GetPublicID() )
	{
		iPingStep = g_GlobalTimer.GetServerDelayMS() / PingStepWnd::PING_MS_VALUE;	
	}			
	else if( P2PNetwork::IsNetworkTableUser( m_szName ) )
	{
		iPingStep = abs(  P2PNetwork::GetP2pPingTime( m_szName ) ) / PingStepWnd::PING_MS_VALUE;
		if( P2PNetwork::IsServerRelay( m_szName ) )
			bServerRelay = true;
	}
	else
	{
		iPingStep = m_iPingStep;
	}
	m_PingStep.RenderPing( iXPos, iYPos, iPingStep, bServerRelay );
}

void PartyInfoBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
	}	
}
//////////////////////////////////////////////////////////////////////////
PartyInfoWnd::PartyInfoWnd()
{
	m_pBottomFrm		= NULL;
	m_pModeIconBack		= NULL;
	m_pUserModeLine		= NULL;
	m_pUserModeBoxDark	= NULL;
	m_pUserModeBoxLight	= NULL;
	m_dwIndexBackup     = 0;
	m_bStateBackup		= false;
	m_iStateBackup		= 0;
	m_bIconOver			= false;
	m_pModeHelpTooltip  = NULL;
	m_bPenaltyPeso      = false;
}

PartyInfoWnd::~PartyInfoWnd()
{
	m_vUserList.clear();

	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pModeIconBack );
	SAFEDELETE( m_pUserModeLine );
	SAFEDELETE( m_pUserModeBoxDark );
	SAFEDELETE( m_pUserModeBoxLight );
}

void PartyInfoWnd::SetInfo( SP2Packet &rkPacket )	
{
	m_vUserList.clear();

	rkPacket >> m_BattleRoomInfo.m_dwIndex;

	if( m_BattleRoomInfo.m_dwIndex == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		HideWnd();
		return;
	}

	if( m_dwIndexBackup != m_BattleRoomInfo.m_dwIndex )
	{
		// 백업한 데이터가 쓸모 없다.
		m_bStateBackup = false;
		m_dwIndexBackup= 0;
	}

	bool bBattleRoom;
	int iState;
	rkPacket >> m_BattleRoomInfo.m_szName;
	rkPacket >> m_BattleRoomInfo.m_szOwnerName;
	rkPacket >> m_BattleRoomInfo.m_iCurJoiner;
	rkPacket >> m_BattleRoomInfo.m_iCurPlayer;
	rkPacket >> m_BattleRoomInfo.m_iMaxPlayerBlue;
	rkPacket >> m_BattleRoomInfo.m_iMaxPlayerRed;
	rkPacket >> m_BattleRoomInfo.m_iMaxObserver;
	rkPacket >> m_BattleRoomInfo.m_szPassword;
	rkPacket >> m_BattleRoomInfo.m_iAverageLevel;

	rkPacket >> m_BattleRoomInfo.m_bUseExtraOption;
	rkPacket >> m_BattleRoomInfo.m_bNoChallenger;

	rkPacket >> m_BattleRoomInfo.m_iChangeCharType;
	rkPacket >> m_BattleRoomInfo.m_iCoolTimeType;
	rkPacket >> m_BattleRoomInfo.m_iRedHPType;
	rkPacket >> m_BattleRoomInfo.m_iBlueHPType;
	rkPacket >> m_BattleRoomInfo.m_iDropDamageType;
	rkPacket >> m_BattleRoomInfo.m_iGravityType;
	rkPacket >> m_BattleRoomInfo.m_iPreSetMode;
	rkPacket >> m_BattleRoomInfo.m_iTeamAttackType;
	rkPacket >> m_BattleRoomInfo.m_iGetUpType;
	rkPacket >> m_BattleRoomInfo.m_iKOType;
	rkPacket >> m_BattleRoomInfo.m_iRedBlowType;
	rkPacket >> m_BattleRoomInfo.m_iBlueBlowType;
	rkPacket >> m_BattleRoomInfo.m_iRedMoveSpeedType;
	rkPacket >> m_BattleRoomInfo.m_iBlueMoveSpeedType;
	rkPacket >> m_BattleRoomInfo.m_iKOEffectType;
	rkPacket >> m_BattleRoomInfo.m_iRedEquipType;
	rkPacket >> m_BattleRoomInfo.m_iBlueEquipType;
	rkPacket >> m_BattleRoomInfo.m_iCatchModeRoundType;
	rkPacket >> m_BattleRoomInfo.m_iCatchModeRoundTimeType;
	rkPacket >> m_BattleRoomInfo.m_iGrowthUseType;
	rkPacket >> m_BattleRoomInfo.m_iExtraItemUseType;

	rkPacket >> bBattleRoom;	
	rkPacket >> iState;
	rkPacket >> m_BattleRoomInfo.m_iBattleEventType;
	rkPacket >> m_BattleRoomInfo.m_dwTourIndex;
	if( !m_bStateBackup )
		m_BattleRoomInfo.m_iState = iState;
	m_BattleRoomInfo.m_bPassword = !m_BattleRoomInfo.m_szPassword.IsEmpty();

	m_bStateBackup = false;
	if( g_App.IsMeDeveloper() )
	{
		// 개발자 수준 매칭 해제
		if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_NOT_MIN_LEVEL_MATCH || m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_NOT_MAX_LEVEL_MATCH )
			m_BattleRoomInfo.m_iState = BattleRoomInfo::BRS_ACTIVE;
	}

	int iSize;
	rkPacket >> iSize;
	if( iSize > 0 )
	{
		for(int i = 0;i < iSize;i++)
		{
			DWORD dwGuildIndex, dwGuildMark;
			BattleRoomUserInfo kUserInfo;
			rkPacket >> kUserInfo.m_iGradeLevel >> kUserInfo.m_szUserID >> kUserInfo.m_bObserver >> kUserInfo.m_iTeam 
					 >> dwGuildIndex >> dwGuildMark >> kUserInfo.m_iServerPingStep;
			m_vUserList.push_back( kUserInfo );

			g_GuildInfo.SetGuildMark( kUserInfo.m_szUserID, dwGuildIndex, dwGuildMark );
			// 친구 정보 갱신
			g_FriendManager.ClientFriendInfoUpdate( kUserInfo.m_szUserID, kUserInfo.m_iGradeLevel, UP_BATTLE_ROOM );
		}
	}	

	// 전투 정보
	rkPacket >> m_BattleRoomInfo.m_iModeSelectIndex >> m_BattleRoomInfo.m_iPlayState >> m_BattleRoomInfo.m_iModeType;
	switch( m_BattleRoomInfo.m_iPlayState )
	{
	case BST_PLAYING:
		{
			if( m_BattleRoomInfo.m_iModeType != MT_MONSTER_SURVIVAL && m_BattleRoomInfo.m_iModeType != MT_DUNGEON_A && m_BattleRoomInfo.m_iModeType != MT_RAID &&
				!Help::IsMonsterDungeonMode((ModeType)m_BattleRoomInfo.m_iModeType))
			{
				if( m_BattleRoomInfo.m_iModeType == MT_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_TEAM_SURVIVAL || 
					m_BattleRoomInfo.m_iModeType == MT_TEAM_SURVIVAL_AI || m_BattleRoomInfo.m_iModeType == MT_BATTLE ||
					m_BattleRoomInfo.m_iModeType == MT_BOSS || m_BattleRoomInfo.m_iModeType == MT_GANGSI ||
					m_BattleRoomInfo.m_iModeType == MT_FIGHT_CLUB || m_BattleRoomInfo.m_iModeType == MT_DOUBLE_CROWN )
				{
					rkPacket >> m_BattleRoomInfo.m_dwRemainPlayTime;
				}
				else
					rkPacket >> m_BattleRoomInfo.m_iBlueTeamWinCount >> m_BattleRoomInfo.m_iRedTeamWinCount;
			}
		}
		break;
	}

	// 비밀방
	char szPasswordRoom[MAX_PATH] = "";
	if( !m_BattleRoomInfo.m_szPassword.IsEmpty() )
		SafeSprintf( szPasswordRoom, sizeof( szPasswordRoom ), STR(2) );


	// UI TITLE	
	char szBtnTitle[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	switch( m_BattleRoomInfo.m_iModeType )
	{
	case MT_NONE:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(3), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(15) );
		break;
	case MT_SURVIVAL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(4), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(16) );
		break;
	case MT_FIGHT_CLUB:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(32), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(33) );
		break;
	case MT_TEAM_SURVIVAL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(5), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(17) );
		break;
	case MT_TOWER_DEFENSE:
		SafeSprintf( szTitle, sizeof( szTitle ), "깊은 숲 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), "깊은 숲?" );
		break;
	case MT_DARK_XMAS:
		SafeSprintf( szTitle, sizeof( szTitle ), "스노우타운 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), "스노우타운?" );
		break;
	case MT_FIRE_TEMPLE:
		SafeSprintf( szTitle, sizeof( szTitle ), "불의 신전 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), "불의 신전?" );
		break;
	case MT_SYMBOL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(6), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(18) );
		break;
	case MT_CATCH:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(7), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(19) );
		break;
	case MT_CATCH_RUNNINGMAN:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(34), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(35) );
		break;
	case MT_KING:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(8), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(20) );
		break;	
	case MT_MONSTER_SURVIVAL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(9), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(21) );
		break;
	case MT_BOSS:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(10), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(22) );
		break;
	case MT_FOOTBALL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(11), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(23) );
		break;
	case MT_HEROMATCH:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(12), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(24) );
		break;
	case MT_GANGSI:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(25), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(26) );
		break;
	case MT_DUNGEON_A:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(30), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(31) );
		break;
	case MT_DOUBLE_CROWN:
		SafeSprintf( szTitle, sizeof( szTitle ), "더블 크라운 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), "더블 크라운?" );
		break;
	case MT_FACTORY:
		SafeSprintf( szTitle, sizeof( szTitle ), "비밀공장 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), "비밀공장?" );
		break;
	case MT_TEAM_SURVIVAL_AI:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(36), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(37) );
		break;
	case MT_RAID:
		SafeSprintf( szTitle, sizeof( szTitle ), "레이드 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), "레이드?" );
		break;
	case MT_UNDERWEAR:
		SafeSprintf( szTitle, sizeof( szTitle ), "팬티몬 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), "팬티몬?" );
		break;
	case MT_CBT:
		SafeSprintf( szTitle, sizeof( szTitle ), "클베모드 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), "클베모드?" );
		break;
	case MT_FLAG_CAPTURE:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(38), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(39) );
		break;
	case MT_ARENA:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(40), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(41) );
		break;
	case MT_BATTLE:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(42), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(43) );
		break;
	case MT_FARMING:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(44), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(45) );
		break;
	}	

	// 배틀모드 관전 막음
	//if( m_BattleRoomInfo.m_iModeType == MT_BATTLE )
	//{
	//	ioWnd *pEnterObserver = FindChildWnd( ID_ENTER_OBSERVER	);
	//	pEnterObserver->SetInActive();
	//}
	//else //if( m_BattleRoomInfo.m_iModeType != MT_BATTLE )
	//{
	//	ioWnd *pEnterObserver = FindChildWnd( ID_ENTER_OBSERVER	);
	//	pEnterObserver->SetActive();
	//}

	bool bUseExtraOption = m_BattleRoomInfo.m_bUseExtraOption;

	if( m_BattleRoomInfo.m_iModeType == MT_UNDERWEAR || m_BattleRoomInfo.m_iModeType == MT_CBT || m_BattleRoomInfo.m_iModeType == MT_FARMING )
		bUseExtraOption = false;

	if( bUseExtraOption )
	{
		SafeSprintf( szTitle, sizeof( szTitle ), STR(13), g_ModeSelectInfo.GetCustomModeTitle().c_str(),  m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(29), g_ModeSelectInfo.GetCustomModeTitle().c_str() );
	}

	if( g_ModeSelectInfo.GetQuickStartIndexToManualIndex( m_BattleRoomInfo.m_iModeSelectIndex ) != -1 )
	{
		ioWnd *pManualBtn = FindChildWnd( ID_MANUAL_BTN );
		if( pManualBtn )
		{
			pManualBtn->SetTitleText( szBtnTitle );
			pManualBtn->ShowWnd();
		}
	}
	else
	{
		HideChildWnd( ID_MANUAL_BTN );
	}

	char szTitle2[MAX_PATH] = "";
	SafeSprintf( szTitle2, sizeof( szTitle2 ), STR(14), szTitle, szPasswordRoom );
	SetTitleText( szTitle2 );	

	if( !IsShow() )
	{
		ShowWnd();
	}
	else
	{
		iwm_show();
	}
}

const BattleRoomUserInfo &PartyInfoWnd::GetUserInfo( TeamType eTeam, int iArray )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		BattleRoomUserInfo &rkInfo = m_vUserList[i];
		if( rkInfo.m_iTeam != (int)eTeam ) continue;
		if( rkInfo.m_bObserver ) continue;

		if( --iArray < 0 )
			return m_vUserList[i];
	}
	static BattleRoomUserInfo kReturnInfo;
	return kReturnInfo;
}

void PartyInfoWnd::UpdateUserList()
{
	TeamType eBlueBtnTeam = TEAM_BLUE;
	TeamType eRedBtnTeam  = TEAM_RED;

	switch( m_BattleRoomInfo.m_iModeType )
	{
	case MT_NONE:
	case MT_SURVIVAL:
	case MT_FIGHT_CLUB:
	case MT_BOSS:
	case MT_MONSTER_SURVIVAL:
	case MT_GANGSI:
	case MT_DUNGEON_A:
	case MT_RAID:
		{
			eBlueBtnTeam = eRedBtnTeam = TEAM_NONE;
		}
		break;
	}

	if(Help::IsMonsterDungeonMode((ModeType)m_BattleRoomInfo.m_iModeType))
		eBlueBtnTeam = eRedBtnTeam = TEAM_NONE;

	int i = 0;
	// 블루팀
	for(i = 0;i < MAX_TEAM_LIST;i++)
	{
		PartyInfoBtn *pBlueBtn = dynamic_cast<PartyInfoBtn*>(FindChildWnd( ID_BLUE_TEAM_1 + i ));
		if( pBlueBtn )
		{
			const BattleRoomUserInfo &rkInfo = GetUserInfo( TEAM_BLUE, i );
			pBlueBtn->SetMemberInfo( m_BattleRoomInfo.m_iMaxPlayerBlue, i, rkInfo.m_szUserID, rkInfo.m_iGradeLevel, rkInfo.m_iServerPingStep, eBlueBtnTeam );
		}
	}

	// 레드팀
	for(i = 0;i < MAX_TEAM_LIST;i++)
	{
		PartyInfoBtn *pBlueBtn = dynamic_cast<PartyInfoBtn*>(FindChildWnd( ID_RED_TEAM_1 + i ));
		if( pBlueBtn )
		{
			const BattleRoomUserInfo &rkInfo = GetUserInfo( TEAM_RED, i );
			pBlueBtn->SetMemberInfo( m_BattleRoomInfo.m_iMaxPlayerRed, i, rkInfo.m_szUserID, rkInfo.m_iGradeLevel, rkInfo.m_iServerPingStep, eRedBtnTeam );
		}
	}
}

void PartyInfoWnd::SetBackupData( DWORD dwIndex, bool bPassword, int iState )
{
	m_dwIndexBackup = dwIndex;
	m_BattleRoomInfo.m_bPassword = bPassword;
	m_BattleRoomInfo.m_iState = m_iStateBackup = iState;
	m_bStateBackup = true;
}

void PartyInfoWnd::UpdataBattleRoomState( DWORD dwIndex, int iState )
{
	if( !IsShow() ) return;
	if( m_BattleRoomInfo.m_dwIndex == dwIndex )
	{
		m_BattleRoomInfo.m_iState = iState;		
	}
}

void PartyInfoWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void PartyInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else if( szType == "UserModeLine" )
	{
		SAFEDELETE( m_pUserModeLine );
		m_pUserModeLine = pImage;
	}
	else if( szType == "UserModeBoxDark" )
	{
		SAFEDELETE( m_pUserModeBoxDark );
		m_pUserModeBoxDark = pImage;
	}
	else if( szType == "UserModeBoxLight" )
	{
		SAFEDELETE( m_pUserModeBoxLight );
		m_pUserModeBoxLight = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PartyInfoWnd::iwm_show()
{	
	UpdateUserList();

	bool bUseExtraOption = m_BattleRoomInfo.m_bUseExtraOption;

	if( m_BattleRoomInfo.m_iModeType == MT_UNDERWEAR || m_BattleRoomInfo.m_iModeType == MT_CBT || m_BattleRoomInfo.m_iModeType == MT_FARMING )
		bUseExtraOption = false;

	if( bUseExtraOption )
	{
		SetSize( GetWidth(), USER_MODE_HEIGHT );
		SetWndPos( (Setting::Width() - GetWidth() ) / 2, ( (Setting::Height() - USER_MODE_HEIGHT ) / 2 ) - 20 );
	}
	else
	{
		SetSize( GetWidth(), NORMAL_MODE_HEIGHT );
		SetWndPos( (Setting::Width() - GetWidth() ) / 2, ( (Setting::Height() - NORMAL_MODE_HEIGHT ) / 2 ) - 20 );
	}

	ioWnd *pEnterParty = FindChildWnd( ID_ENTER_PARTY );
	if( pEnterParty )
		pEnterParty->SetWndPos( pEnterParty->GetXPos(), GetHeight() - BOTTOM_BTN_GAP );
	ioWnd *pEnterObserver = FindChildWnd( ID_ENTER_OBSERVER	);
	if( pEnterObserver )
	{
		pEnterObserver->SetWndPos( pEnterObserver->GetXPos(), GetHeight() - BOTTOM_BTN_GAP );
	}

	ioWnd *pCancel = FindChildWnd( ID_CANCEL );
	if( pCancel )
		pCancel->SetWndPos( pCancel->GetXPos(), GetHeight() - BOTTOM_BTN_GAP );

	if( m_BattleRoomInfo.m_iBattleEventType == BET_TOURNAMENT_BATTLE )
	{
		SetChildWndStyleAdd( ID_ENTER_PARTY, IWS_EXACTIVE );
	}
	else if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_ACTIVE )
	{
		SetChildWndStyleRemove( ID_ENTER_PARTY, IWS_EXACTIVE );
	}
	else
	{
		SetChildWndStyleAdd( ID_ENTER_PARTY, IWS_EXACTIVE );
	}

	RECT rcDrag = { 0, 0, GetWidth(), GetHeight() };
	SetDragRect( rcDrag );
	
	if( strcmp( g_MyInfo.GetPublicID().c_str(), "개발자B" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "개발자K" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "개발자L" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "개발자Q" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "개발자U" ) == 0 || //Except Extracting Hangeul
		strcmp( g_MyInfo.GetPublicID().c_str(), "매니저X" ) == 0 )  //Except Extracting Hangeul
	{
		ShowChildWnd( ID_DEVELOPER_ENTER );
	}
	else
	{
		HideChildWnd( ID_DEVELOPER_ENTER );
	}

	// 전투방 페널티 받는지 체크
	m_bPenaltyPeso      = g_BattleRoomMgr.IsBattleRoomMovePenaltyPeso();
}

void PartyInfoWnd::iwm_hide()
{
	m_bStateBackup = false;

	m_BattleRoomInfo.m_iBattleEventType = 0;
	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}	
}

bool PartyInfoWnd::iwm_esc()
{
	ioWnd *pExitBtn = FindChildWnd( ID_EXIT );
	if( pExitBtn )
		iwm_command( pExitBtn, IOBN_BTNUP, 0 );
	return true;
}

void PartyInfoWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false  */ )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	if( !bCreate )
	{
		if( IsShow() )
		{
			iwm_show();
		}
	}
}

void PartyInfoWnd::JoinBattleRoom( bool bObserver )
{
	if( bObserver )
	{
		if( m_BattleRoomInfo.m_iBattleEventType == BET_TOURNAMENT_BATTLE )
		{
			DWORD dwOwenrIndex = 0;
			TournamentNode* pTournament = g_TournamentMgr.GetTournament( m_BattleRoomInfo.m_dwTourIndex, false );
			if( pTournament )
				dwOwenrIndex = pTournament->GetOwnerIndex();

			if( !g_App.IsMeDeveloper() && dwOwenrIndex != g_MyInfo.GetUserIndex() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 경기방 관전은 주최자 및 기자단만 입장이 가능합니다." );
				return;
			}
		}
	}

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_USER_BATTLEROOM_JOIN );
	// Observer << Invite << PenaltyPeso << iPenaltyPeso << .... 순서로
	kPacket << bObserver << false << m_bPenaltyPeso << g_MyInfo.GetCurPenaltyPeso() << m_BattleRoomInfo.m_dwIndex;
	TCPNetwork::SendToServer( kPacket );
}

void PartyInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{			
			HideWnd();

			//전투방 리프레쉬
			if( m_iStateBackup != m_BattleRoomInfo.m_iState )
			{
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
					if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
						pLobbyMainWnd->RefreshBattleList( m_BattleRoomInfo.m_dwIndex );

					TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
					if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
						pTrainingFuncWnd->RefreshBattleList();
				}
				else
				{
					QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
					if( pWnd && pWnd->IsShow() )
						pWnd->RefreshBattleList( m_BattleRoomInfo.m_dwIndex );
				}
			}
		}
		break;
	case ID_MANUAL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iManualIndex = g_ModeSelectInfo.GetQuickStartIndexToManualIndex( m_BattleRoomInfo.m_iModeSelectIndex );
			if( iManualIndex != -1 )
			{
				ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
				if( pManualWnd )
					pManualWnd->ShowWndByCurManual( iManualIndex );
			}
		}
		break;
	case ID_ENTER_PARTY:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( m_BattleRoomInfo.m_iBattleEventType == BET_TOURNAMENT_BATTLE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 경기방은 입장이 불가능합니다." );
			}
			else if( g_BattleRoomMgr.IsKickOutRoom( m_BattleRoomInfo.m_dwIndex ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( g_MyInfo.GetActiveCharCount() == 0 || ( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			else if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_FULL_USER )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			}
			else if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_TIME_CLOSE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			}
			else if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_ENTER_X )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			}
			else if( (m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_NOT_MIN_LEVEL_MATCH) && !g_App.IsMeDeveloper(true) )	// 개발자 제외 2020-10-15
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6), g_MyInfo.GetPublicID().c_str() );
			}
			else if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_NOT_MAX_LEVEL_MATCH  && !g_App.IsMeDeveloper(true)  )	// 개발자 제외 2020-10-15
			{
				g_GUIMgr.SetMsgBox( MB_YESNO, this, STR(7), g_MyInfo.GetPublicID().c_str() );
			}
			else if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_ALREADY_ROOM )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
			}
			else if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_MAP_LIMIT_GRADE )
			{
				char szGradeName[MAX_PATH] = "";
				g_LevelMgr.GetGradeName( g_ModeSelectInfo.GetSelectQuickStartModeLimitGrade( m_BattleRoomInfo.m_iModeSelectIndex ), szGradeName, MAX_PATH, true, true );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12), g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_BattleRoomInfo.m_iModeSelectIndex ).c_str(), szGradeName );
			}
			else if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_OPTION_LIMIT_GRADE )
			{
				char szGradeName[MAX_PATH] = "";
				g_LevelMgr.GetGradeName( g_RoomOptionMgr.GetEnableLevel(), szGradeName, MAX_PATH, true, true );

				ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
				kPrinter[0].SetTextStyle( TS_NORMAL );
				kPrinter[0].SetBkColor( 0, 0, 0 );
				kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[0].AddTextPiece( FONT_SIZE_15, STR(13), g_ModeSelectInfo.GetCustomModeTitle().c_str() );

				kPrinter[0].SetTextColor( TCT_DEFAULT_ORANGE );
				kPrinter[0].AddTextPiece( FONT_SIZE_15, STR(14), szGradeName );

				kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[0].AddTextPiece( FONT_SIZE_15, STR(15) );

				g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
			}
			else if((m_BattleRoomInfo.m_iModeType == MT_MONSTER_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_DUNGEON_A) && 
						g_MyInfo.GetEtcMonsterCoin() + g_MyInfo.GetEtcGoldMonsterCoin() < g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex ) )
			{
				MonsterCoinUseWnd *pMonsterCoinWnd = dynamic_cast<MonsterCoinUseWnd*>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
				if( pMonsterCoinWnd )
				{
					int iUseCoinCnt = g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex );
					pMonsterCoinWnd->ShowJoinCoinLack( g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_BattleRoomInfo.m_iModeSelectIndex ), iUseCoinCnt, (ModeType)m_BattleRoomInfo.m_iModeType  );
					return;
				}

				//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
				return;
			}
			else if(m_BattleRoomInfo.m_iModeType == MT_RAID && 
				g_MyInfo.GetEtcRaidCoin() < g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex ) )
			{
				MonsterCoinUseWnd *pMonsterCoinWnd = dynamic_cast<MonsterCoinUseWnd*>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
				if( pMonsterCoinWnd )
				{
					int iUseCoinCnt = g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex );
					pMonsterCoinWnd->ShowJoinCoinLack( g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_BattleRoomInfo.m_iModeSelectIndex ), iUseCoinCnt, (ModeType)m_BattleRoomInfo.m_iModeType  );
					return;
				}

				//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
				return;
			}
			else if(Help::IsMonsterDungeonMode((ModeType)m_BattleRoomInfo.m_iModeType) && 
				g_MyInfo.GetEtcMonsterCoin() < g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex ))
			{
				MonsterCoinUseWnd *pMonsterCoinWnd = dynamic_cast<MonsterCoinUseWnd*>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
				if( pMonsterCoinWnd )
				{
					int iUseCoinCnt = g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex );
					pMonsterCoinWnd->ShowJoinCoinLack( g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_BattleRoomInfo.m_iModeSelectIndex ), iUseCoinCnt, (ModeType)m_BattleRoomInfo.m_iModeType );
					return;
				}

				//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
				return;
			}
			else if( m_BattleRoomInfo.m_bPassword )
			{
				PartyPasswordWnd *pPasswordWnd = dynamic_cast<PartyPasswordWnd*>(g_GUIMgr.FindWnd(PARTY_PASSWORD_WND));
				if( pPasswordWnd )
				{
					pPasswordWnd->SetBattleRoomInfo( m_BattleRoomInfo.m_dwIndex, m_BattleRoomInfo.m_iAverageLevel, m_BattleRoomInfo.m_szName,
													 m_BattleRoomInfo.m_iCurJoiner, m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue, m_BattleRoomInfo.m_iMaxPlayerRed, false );
				}
			}
			else
			{
				bool bUseExtraOption = m_BattleRoomInfo.m_bUseExtraOption;

				if( m_BattleRoomInfo.m_iModeType == MT_UNDERWEAR || m_BattleRoomInfo.m_iModeType == MT_CBT || m_BattleRoomInfo.m_iModeType == MT_FARMING )
					bUseExtraOption = false;

				if( bUseExtraOption )
				{
					ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );
					kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(16), g_ModeSelectInfo.GetCustomModeTitle().c_str() );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );
					kPrinter[1].SetTextColor( TCT_DEFAULT_ORANGE );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(17) );

					kPrinter[2].SetTextStyle( TS_NORMAL );
					kPrinter[2].SetBkColor( 0, 0, 0 );
					kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(18) );

					g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, STR(19), STR(20), STR(21), STR(22) );
					return;
				}
				else
				{
					JoinBattleRoom( false );
				}
			}
			HideWnd();
		}
		break;
	case ID_ENTER_OBSERVER:
		if( cmd == IOBN_BTNUP )
		{
			if( g_BattleRoomMgr.IsKickOutRoom( m_BattleRoomInfo.m_dwIndex ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
			}
			else if( g_MyInfo.GetActiveCharCount() == 0 || ( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			}
			else if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_TIME_CLOSE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
			}
			else if( m_BattleRoomInfo.m_bPassword )
			{
				PartyPasswordWnd *pPasswordWnd = dynamic_cast<PartyPasswordWnd*>(g_GUIMgr.FindWnd(PARTY_PASSWORD_WND));
				if( pPasswordWnd )
				{
					pPasswordWnd->SetBattleRoomInfo( m_BattleRoomInfo.m_dwIndex, m_BattleRoomInfo.m_iAverageLevel, m_BattleRoomInfo.m_szName,
													 m_BattleRoomInfo.m_iCurJoiner, m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue, m_BattleRoomInfo.m_iMaxPlayerRed, true );
				}
			}
			else
			{
				JoinBattleRoom( true );
			}
			HideWnd();
		}
		break;
	case ID_BLUE_TEAM_1:
	case ID_BLUE_TEAM_2:
	case ID_BLUE_TEAM_3:
	case ID_BLUE_TEAM_4:
	case ID_BLUE_TEAM_5:
	case ID_BLUE_TEAM_6:
	case ID_BLUE_TEAM_7:
	case ID_BLUE_TEAM_8:
	case ID_RED_TEAM_1:
	case ID_RED_TEAM_2:
	case ID_RED_TEAM_3:
	case ID_RED_TEAM_4:
	case ID_RED_TEAM_5:
	case ID_RED_TEAM_6:
	case ID_RED_TEAM_7:
	case ID_RED_TEAM_8:
		if( cmd == IOBN_BTNUP )
		{
			PartyInfoBtn *pUserBtn = dynamic_cast<PartyInfoBtn*>(pWnd);
			if( pUserBtn )
				g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), true );
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( m_BattleRoomInfo.m_dwIndex != 0 )
				{
					if( m_BattleRoomInfo.m_bPassword )
					{
						PartyPasswordWnd *pPasswordWnd = dynamic_cast<PartyPasswordWnd*>(g_GUIMgr.FindWnd(PARTY_PASSWORD_WND));
						if( pPasswordWnd )
						{
							pPasswordWnd->SetBattleRoomInfo( m_BattleRoomInfo.m_dwIndex, m_BattleRoomInfo.m_iAverageLevel, m_BattleRoomInfo.m_szName,
								m_BattleRoomInfo.m_iCurJoiner, m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue, m_BattleRoomInfo.m_iMaxPlayerRed, false );
						}
					}
					else
					{
						JoinBattleRoom( false );
					}
				}
				HideWnd();
			}
		}
		break;
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				JoinBattleRoom( false );
			}

			HideWnd();
		}
		break;
	case ID_DEVELOPER_ENTER:
		if( cmd == IOBN_BTNUP )
		{
			if( m_vUserList.empty() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "동기화될 유저가 존재하지 않습니다." ); //Except Extracting Hangeul
			}
			else
			{
				g_App.SendRoomStealthEnter( m_vUserList[0].m_szUserID );
			}
		}
		break;
	}
}

void PartyInfoWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pBottomFrm )
	{
		m_pBottomFrm->Render( iXPos, iYPos + ( GetHeight() - BOTTOM_BAR_GAP ), UI_RENDER_MULTIPLY );
	}

	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( 0.60f, 0.60f );
		m_pModeIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		// 모드 아이콘
		ioHashString kModeTitle = g_ModeSelectInfo.GetBattleRoomListTitle( m_BattleRoomInfo.m_iModeSelectIndex );
		ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetListModeUIResource( kModeTitle );
		if( pModeIcon )
		{
			pModeIcon->SetScale( 0.60f, 0.60f );
			pModeIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
			if( m_bIconOver )
				pModeIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 300.0f, m_BattleRoomInfo.m_szName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_BattleRoomInfo.m_iPlayState == BST_START )
	{
		if( m_BattleRoomInfo.m_iModeType == MT_MONSTER_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_DUNGEON_A )
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(1), g_ModeSelectInfo.GetSelectQuickStartModeSubTitle( m_BattleRoomInfo.m_iModeSelectIndex ).c_str() );
		else
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(2) );
	}
	else if( m_BattleRoomInfo.m_iPlayState == BST_RESULT )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		if( m_BattleRoomInfo.m_iModeType == MT_MONSTER_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_DUNGEON_A )
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(3), g_ModeSelectInfo.GetSelectQuickStartModeSubTitle( m_BattleRoomInfo.m_iModeSelectIndex ).c_str() );
		else
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(4) );
	}
	else
	{
		if( m_BattleRoomInfo.m_iModeType == MT_MONSTER_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_DUNGEON_A || m_BattleRoomInfo.m_iModeType == MT_RAID || 
			Help::IsMonsterDungeonMode((ModeType)m_BattleRoomInfo.m_iModeType) )
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(5), g_ModeSelectInfo.GetSelectQuickStartModeSubTitle( m_BattleRoomInfo.m_iModeSelectIndex ).c_str() );
		else if( m_BattleRoomInfo.m_iModeType == MT_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_TEAM_SURVIVAL || 
			m_BattleRoomInfo.m_iModeType == MT_TEAM_SURVIVAL_AI || m_BattleRoomInfo.m_iModeType == MT_BATTLE ||
			m_BattleRoomInfo.m_iModeType == MT_BOSS || m_BattleRoomInfo.m_iModeType == MT_GANGSI || m_BattleRoomInfo.m_iModeType == MT_FIGHT_CLUB || m_BattleRoomInfo.m_iModeType == MT_DOUBLE_CROWN )
		{
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(6), m_BattleRoomInfo.m_dwRemainPlayTime / 1000 );
		}
		else
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );

			kPrinter.SetTextColor( 0, 150, 255 );
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(7), m_BattleRoomInfo.m_iBlueTeamWinCount );

			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_11, "vs " );

			kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(8), m_BattleRoomInfo.m_iRedTeamWinCount );

			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(9), m_BattleRoomInfo.m_iBlueTeamWinCount + m_BattleRoomInfo.m_iRedTeamWinCount + 1 );

			kPrinter.PrintFullText( iXPos + 65, iYPos + 73, TAT_LEFT );
		}
	}

	bool bUseExtraOption = m_BattleRoomInfo.m_bUseExtraOption;

	if( m_BattleRoomInfo.m_iModeType == MT_UNDERWEAR || m_BattleRoomInfo.m_iModeType == MT_CBT || m_BattleRoomInfo.m_iModeType == MT_FARMING )
		bUseExtraOption = false;

	if( bUseExtraOption )
	{
		OnRenderUserMode( iXPos, iYPos );
	}
}

void PartyInfoWnd::OnRenderUserMode( int iXPos, int iYPos )
{
	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_CENTER);
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos + 191, iYPos + 277, FONT_SIZE_11, STR(1) );

	if( m_pUserModeLine )
		m_pUserModeLine->Render( iXPos + 18, iYPos + 282 );

	if( !m_pUserModeBoxDark || !m_pUserModeBoxLight )
		return;

	int i,j;
	enum { MAX_HORZ = 4, MAX_VERT = 6, XGAP = 87, YGAP = 19, };
	for(i = 0;i < MAX_HORZ;i++)
	{
		for(j = 0;j < MAX_VERT;j++)
		{
			if( j%2 == 1 )
				m_pUserModeBoxLight->Render( iXPos + 18 + (i*XGAP), iYPos + 294 + (j*YGAP) );
			else
				m_pUserModeBoxDark->Render( iXPos + 18 + (i*XGAP), iYPos + 294 + (j*YGAP) );
		}
	}

	// 유저 모드 옵션 내용
	enum { GLOBAL_HORZ = 2, GLOBAL_VERT = 6, TEAM_HORZ = 2, TEAM_VERT = 4, };
	for(i = 0;i < GLOBAL_HORZ;i++)
	{
		for(j = 0;j < GLOBAL_VERT;j++)
		{
			int iType = UMO_GROWTH_USE + j + (i * GLOBAL_VERT);
			if( IsDefatulUserModeType( iType ) )
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 60 + (i* XGAP), iYPos + 297 + (j*YGAP), FONT_SIZE_11, GetUserModeDesc( iType ).c_str() );
		}
	}

	for(i = 0;i < TEAM_HORZ;i++)
	{
		for(j = 0;j < TEAM_VERT;j++)
		{
			int iType = UMO_BLUE_HP + j + (i * TEAM_VERT);
			if( IsDefatulUserModeType( iType ) )
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
			else
			{
				if( COMPARE( iType, UMO_BLUE_HP, UMO_RED_HP ) )
					g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
				else
					g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			}
			g_FontMgr.PrintText( iXPos + 234 + (i* XGAP), iYPos + 297 + (j*YGAP), FONT_SIZE_11, GetUserModeDesc( iType ).c_str() );
		}
	}
}

ioHashString PartyInfoWnd::GetUserModeDesc( int iType )
{
	switch( iType )
	{
	case UMO_CHANGE_CHAR:
		return g_RoomOptionMgr.GetChangeCharDesc( m_BattleRoomInfo.m_iChangeCharType, true );
	case UMO_COOL_TIME:
		return g_RoomOptionMgr.GetCoolTimeDesc( m_BattleRoomInfo.m_iCoolTimeType, true );
	case UMO_DROP_DAMAGE:
		return g_RoomOptionMgr.GetDropDamageDesc( m_BattleRoomInfo.m_iDropDamageType, true );
	case UMO_TEAM_ATTACK:
		return g_RoomOptionMgr.GetTeamAttackDesc( m_BattleRoomInfo.m_iTeamAttackType, true );
	case UMO_GRAVITY:
		return g_RoomOptionMgr.GetGravityDesc( m_BattleRoomInfo.m_iGravityType, true );
	case UMO_GET_UP_DELAY:
		return g_RoomOptionMgr.GetGetUpDesc( m_BattleRoomInfo.m_iGetUpType, true );
	case UMO_KO:
		return g_RoomOptionMgr.GetKODesc( m_BattleRoomInfo.m_iKOType, true );
	case UMO_KO_EFFECT:
		return g_RoomOptionMgr.GetKOEffectDesc( m_BattleRoomInfo.m_iKOEffectType, true );
	case UMO_ROUND_TYPE:
		return g_RoomOptionMgr.GetRoundTypeDesc( m_BattleRoomInfo.m_iCatchModeRoundType, true );
	case UMO_ROUND_TIME:
		return g_RoomOptionMgr.GetRoundTimeTypeDesc( m_BattleRoomInfo.m_iCatchModeRoundTimeType, true );
	case UMO_BLUE_HP:
		return g_RoomOptionMgr.GetBlueHPDesc( m_BattleRoomInfo.m_iBlueHPType, true );
	case UMO_BLUE_MOVE_SPD:
		return g_RoomOptionMgr.GetBlueMoveSpeedDesc( m_BattleRoomInfo.m_iBlueMoveSpeedType, true );
	case UMO_BLUE_EQUIP:
		return g_RoomOptionMgr.GetBlueEquipDesc( m_BattleRoomInfo.m_iBlueEquipType, true );
	case UMO_BLUE_BLOW:
		return g_RoomOptionMgr.GetBlueBlowDesc( m_BattleRoomInfo.m_iBlueBlowType, true );
	case UMO_RED_HP:
		return g_RoomOptionMgr.GetRedHPDesc( m_BattleRoomInfo.m_iRedHPType, true );
	case UMO_RED_MOVE_SPD:
		return g_RoomOptionMgr.GetRedMoveSpeedDesc( m_BattleRoomInfo.m_iRedMoveSpeedType, true );
	case UMO_RED_EQUIP:
		return g_RoomOptionMgr.GetRedEquipDesc( m_BattleRoomInfo.m_iRedEquipType, true );
	case UMO_RED_BLOW:
		return g_RoomOptionMgr.GetRedBlowDesc( m_BattleRoomInfo.m_iRedBlowType, true );
	case UMO_GROWTH_USE:
		return g_RoomOptionMgr.GetGrowthUseDesc( m_BattleRoomInfo.m_iGrowthUseType, true );
	case UMO_EXTRAITEM_USE:
		return g_RoomOptionMgr.GetExtraItemUseDesc( m_BattleRoomInfo.m_iExtraItemUseType, true );
	}
	return ioHashString();
}

bool PartyInfoWnd::IsDefatulUserModeType( int iType )
{
	PreSetInfo kDefaultInfo;
	g_RoomOptionMgr.GetDefaultInfo( kDefaultInfo );

	switch( iType )
	{
	case UMO_CHANGE_CHAR:
		if( kDefaultInfo.m_iChangeCharType == m_BattleRoomInfo.m_iChangeCharType )
			return true;
		return false;
	case UMO_COOL_TIME:
		if( kDefaultInfo.m_iCoolTimeType == m_BattleRoomInfo.m_iCoolTimeType )
			return true;
		return false;
	case UMO_DROP_DAMAGE:
		if( kDefaultInfo.m_iDropDamageType == m_BattleRoomInfo.m_iDropDamageType )
			return true;
		return false;
	case UMO_TEAM_ATTACK:
		if( kDefaultInfo.m_iTeamAttackType == m_BattleRoomInfo.m_iTeamAttackType )
			return true;
		return false;
	case UMO_GRAVITY:
		if( kDefaultInfo.m_iGravityType == m_BattleRoomInfo.m_iGravityType )
			return true;
		return false;
	case UMO_GET_UP_DELAY:
		if( kDefaultInfo.m_iGetUpType == m_BattleRoomInfo.m_iGetUpType )
			return true;
		return false;
	case UMO_KO:
		if( kDefaultInfo.m_iKOType == m_BattleRoomInfo.m_iKOType )
			return true;
		return false;
	case UMO_KO_EFFECT:
		if( kDefaultInfo.m_iKOEffectType == m_BattleRoomInfo.m_iKOEffectType )
			return true;
		return false;
	case UMO_ROUND_TYPE:
		if( kDefaultInfo.m_iCatchModeRoundType == m_BattleRoomInfo.m_iCatchModeRoundType )
			return true;
		return false;
	case UMO_ROUND_TIME:
		if( kDefaultInfo.m_iCatchModeRoundTimeType == m_BattleRoomInfo.m_iCatchModeRoundTimeType )
			return true;
		return false;
	case UMO_BLUE_HP:
		if( kDefaultInfo.m_iBlueHPType == m_BattleRoomInfo.m_iBlueHPType )
			return true;
		return false;
	case UMO_BLUE_MOVE_SPD:
		if( kDefaultInfo.m_iBlueMoveSpeedType == m_BattleRoomInfo.m_iBlueMoveSpeedType )
			return true;
		return false;
	case UMO_BLUE_EQUIP:
		if( kDefaultInfo.m_iBlueEquipType == m_BattleRoomInfo.m_iBlueEquipType )
			return true;
		return false;
	case UMO_BLUE_BLOW:
		if( kDefaultInfo.m_iBlueBlowType == m_BattleRoomInfo.m_iBlueBlowType )
			return true;
		return false;
	case UMO_RED_HP:
		if( kDefaultInfo.m_iRedHPType == m_BattleRoomInfo.m_iRedHPType )
			return true;
		return false;
	case UMO_RED_MOVE_SPD:
		if( kDefaultInfo.m_iRedMoveSpeedType == m_BattleRoomInfo.m_iRedMoveSpeedType )
			return true;
		return false;
	case UMO_RED_EQUIP:
		if( kDefaultInfo.m_iRedEquipType == m_BattleRoomInfo.m_iRedEquipType )
			return true;
		return false;
	case UMO_RED_BLOW:
		if( kDefaultInfo.m_iRedBlowType == m_BattleRoomInfo.m_iRedBlowType )
			return true;
		return false;
	case UMO_GROWTH_USE:
		if( kDefaultInfo.m_iGrowthUseType == m_BattleRoomInfo.m_iGrowthUseType )
			return true;
		return false;
	case UMO_EXTRAITEM_USE:
		if( kDefaultInfo.m_iExtraItemUseType == m_BattleRoomInfo.m_iExtraItemUseType )
			return true;
		return false;
	}
	return false;
}

void PartyInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	ioMovingWnd::OnProcess( fTimePerSec );
	ProcessModeHelpTooltip();
}

void PartyInfoWnd::ProcessModeHelpTooltip()
{
	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		m_bIconOver = false;

		if( g_GUIMgr.GetPreOverWnd() == this ||
			g_GUIMgr.GetPreClickWnd() == this )
		{
			ioLSMouse *pMouse = g_App.GetMouse();

			enum { STARTX = 17, STARTY = 48, WIDTH = 45, HEIGHT = 45, };
			RECT rcIconRect;
			rcIconRect.left   = GetDerivedPosX() + STARTX;
			rcIconRect.top    = GetDerivedPosY() + STARTY;
			rcIconRect.right  = rcIconRect.left + WIDTH;
			rcIconRect.bottom = rcIconRect.top + HEIGHT;
			if( ::PtInRect( &rcIconRect, pMouse->GetMousePos() ) )
			{
				m_bIconOver = true;	
			}
		}    

		if( m_bIconOver )
		{
			pModeHelpTooltip->SetModeHelpTooltip( this, g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_BattleRoomInfo.m_iModeSelectIndex ), 
				g_ModeSelectInfo.GetQuickStartIndexToHelpIndex( m_BattleRoomInfo.m_iModeSelectIndex ) );
		}
		else
		{
			pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
		}
	}	
}
//////////////////////////////////////////////////////////////////////////
PartyPasswordWnd::PartyPasswordWnd()
{
	m_pPreEdit	= NULL;
	m_dwIndex	= -1;
	m_iLevel	= 0;
	m_iCurJoiner= 0;
	m_iCurPlayer= 0;
	m_iMaxPlayerBlue = 0;
	m_iMaxPlayerRed  = 0;
	m_bPenaltyPeso	 = false;
	m_bObserver		 = false;
}

PartyPasswordWnd::~PartyPasswordWnd()
{
}

void PartyPasswordWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PASS_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}

	// 전투방 페널티 받는지 체크
	m_bPenaltyPeso      = g_BattleRoomMgr.IsBattleRoomMovePenaltyPeso();
}

void PartyPasswordWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PASS_EDIT );
	if( pEdit )
		pEdit->KillKeyFocus();
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void PartyPasswordWnd::SetBattleRoomInfo( DWORD dwIndex, int iLevel, ioHashString szName, int iCurJoiner, int iCurPlayer, int iMaxPlayerBlue, int iMaxPlayerRed, bool bObserver )
{
	m_dwIndex	= dwIndex;
	m_iLevel	= iLevel;
	m_szName	= szName;
	m_iCurJoiner= iCurJoiner;
	m_iCurPlayer= iCurPlayer;
	m_iMaxPlayerBlue = iMaxPlayerBlue;
	m_iMaxPlayerRed  = iMaxPlayerRed;

	m_bObserver = bObserver;

	if( dwIndex != 0 )
		ShowWnd();
}

void PartyPasswordWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			JoinBattleRoom();
		}
		break;
	case ID_CANCEL:
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();			
		}
		break;
	case ID_PASS_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				JoinBattleRoom();
			}						
		}
		break;
	}
}

void PartyPasswordWnd::JoinBattleRoom()
{
	if( g_App.IsMouseBusy() )
		return;
	if( m_dwIndex == 0 )
		return;

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PASS_EDIT );
	if( pEdit )
	{
		TCPNetwork::MouseBusy( true );
		SP2Packet kPacket( CTPK_USER_BATTLEROOM_JOIN );
		// Observer << Invite << PenaltyPeso << iPenaltyPeso << .... 순서로
		kPacket << m_bObserver << false << m_bPenaltyPeso << g_MyInfo.GetCurPenaltyPeso() << m_dwIndex << pEdit->GetText();
		TCPNetwork::SendToServer( kPacket );
	}
	HideWnd();
}

void PartyPasswordWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
	g_FontMgr.PrintText( iXPos + 125, iYPos + 55, FONT_SIZE_13, STR(1) );
}
//////////////////////////////////////////////////////////////////////////
PartyCreateWnd::PartyCreateWnd(): m_pNameEdit( NULL ), m_pPWEdit( NULL ), m_pPreEdit( NULL )
{
	m_iMaxPlayerBlue = 8;
	m_iMaxPlayerRed  = 8;
	m_iMaxObserver = 4;
	m_iSelectTerm  = -1;
}

PartyCreateWnd::~PartyCreateWnd()
{
}

void PartyCreateWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;
	m_pPWEdit = FindChildWnd( ID_EDIT_PW );
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");

		if( g_MyInfo.GetUserEventType() == USER_TYPE_BROADCAST_MBC )
		{
			char szRandPW[MAX_PATH] = "";
			sprintf( szRandPW, "%d", rand()%8999 + 1000 );
			g_InputBox.SetString( szRandPW );
		}
	}

	m_pNameEdit = FindChildWnd( ID_EDIT_NAME );
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString("");
	}

	m_iMaxPlayerBlue = g_BattleRoomMgr.GetDefMaxPlayer() / 2;
	m_iMaxPlayerRed  = g_BattleRoomMgr.GetDefMaxPlayer() / 2;

	m_iSelectTerm  = -1;
	if( g_MyInfo.GetUserEventType() == USER_TYPE_BROADCAST_MBC )
		m_iSelectTerm = BMT_CATCH;


	ioWnd *pModeBtn = FindChildWnd( ID_CHANGE_MODE );
	if( pModeBtn )
	{
		pModeBtn->SetTitleText( STR(1) );
	}
	SetChildWndStyleAdd( ID_CREATE, IWS_EXACTIVE );
}

void PartyCreateWnd::iwm_hide()
{
	if( m_pPWEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pPWEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pNameEdit )
	{
		ioEdit *pEdit = (ioEdit*)m_pNameEdit;
		pEdit->KillKeyFocus();
	}
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void PartyCreateWnd::CreateBattleRoom( int iSelectTerm, int iBlueCount, int iRedCount )
{
	if( g_App.IsMouseBusy() )
		return;

	if( g_MyInfo.GetUserEventType() == USER_TYPE_BROADCAST_MBC )
		iSelectTerm = BMT_CATCH;

	if( iSelectTerm == -1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	if( g_BattleRoomMgr.IsBattleRoom() )
		return;
	if( g_LadderTeamMgr.IsLadderTeam() )
		return;

	if( m_pNameEdit == NULL )
		m_pNameEdit = FindChildWnd( ID_EDIT_NAME );
	if( m_pPWEdit == NULL )
		m_pPWEdit = FindChildWnd( ID_EDIT_PW );

	m_iMaxPlayerBlue = iBlueCount;
	m_iMaxPlayerRed  = iRedCount;
	ioEdit *pEdit = (ioEdit*)m_pNameEdit;
	ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;	
	if( pEdit && pPWEdit )
	{
		char szPassword[MAX_PATH] = "";
		sprintf( szPassword, "%s", pPWEdit->GetText() );

		SP2Packet kPacket( CTPK_CREATE_BATTLEROOM );
		kPacket << iSelectTerm << g_SlangUtil.ConvertString( pEdit->GetText() ) << szPassword << m_iMaxPlayerBlue << m_iMaxPlayerRed << m_iMaxObserver;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
		HideWnd();
	}			
}

void PartyCreateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_CREATE:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			CreateBattleRoom( m_iSelectTerm, m_iMaxPlayerBlue, m_iMaxPlayerRed );
		}
		break;
	case ID_CHANGE_MODE:
		if( cmd == IOBN_BTNUP )
		{			
			BattleQuickStartOption *pSelectWnd = dynamic_cast<BattleQuickStartOption*>(g_GUIMgr.FindWnd( BATTLE_QUICKSTART_OPTION ));
			if( pSelectWnd )
			{
				pSelectWnd->SetQuickStart( this, BattleQuickStartOption::E_CREATEBATTLE );
				pSelectWnd->ShowWnd();
			}
		}
		else if( cmd == ID_MODE_LIST )
		{			
			m_iSelectTerm = param;
			pWnd->SetTitleText( g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_iSelectTerm ).c_str()  );
			SetChildWndStyleRemove( ID_CREATE, IWS_EXACTIVE );
			if( m_iSelectTerm == BMT_TEAM_SURVIVAL_AI_EASY || m_iSelectTerm == BMT_TEAM_SURVIVAL_AI_HARD  )
			{
				//첫번째 맵!
				int iSlotMax = g_BattleRoomMgr.GetAIMaxSlot( 1 );
				SetMaxPlayer( iSlotMax, 0, 0 );
				if( m_iSelectTerm == BMT_TEAM_SURVIVAL_AI_EASY )
					g_BattleRoomMgr.SetAiLevel( 0 );
				else if( m_iSelectTerm == BMT_TEAM_SURVIVAL_AI_HARD )
					g_BattleRoomMgr.SetAiLevel( 1 );
			}
			// 배틀모드 일때 최대 인원 수정
			int iMapDefaultMaxPlayer = 0;
			if( m_iSelectTerm == BMT_BATTLE_3VS3 || m_iSelectTerm == BMT_BATTLE_2VS2 )
			{
				if( m_iSelectTerm == BMT_BATTLE_3VS3 )
					iMapDefaultMaxPlayer = g_ModeSelectInfo.GetBattleMapDefaultMaxPlayer( MT_BATTLE, 1 );	
				else if( m_iSelectTerm == BMT_BATTLE_2VS2 )
					iMapDefaultMaxPlayer = g_ModeSelectInfo.GetBattleMapDefaultMaxPlayer( MT_BATTLE, 2 );	
				m_iMaxPlayerBlue	= iMapDefaultMaxPlayer / 2;
				m_iMaxPlayerRed		= iMapDefaultMaxPlayer / 2;
			}
		}
		break;
	case ID_EDIT_NAME:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pPWEdit )
				{
					ioEdit *pPWEdit = (ioEdit*)m_pPWEdit;
					pPWEdit->SetKeyFocus();
				}
			}			
			else if( param == 1 )
			{
				CreateBattleRoom( m_iSelectTerm, m_iMaxPlayerBlue, m_iMaxPlayerRed );	
			}
		}
		break;
	case ID_EDIT_PW:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 )
			{
				if( m_pNameEdit )
				{
					ioEdit *pNameEdit = (ioEdit*)m_pNameEdit;
					pNameEdit->SetKeyFocus();
				}				
			}			
			else if( param == 1 )
			{
				CreateBattleRoom( m_iSelectTerm, m_iMaxPlayerBlue, m_iMaxPlayerRed );
			}
		}
		break;
	}
}

void PartyCreateWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 110,110,110 );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 57, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 80, FONT_SIZE_12, STR(2) );
	g_FontMgr.PrintText( iXPos + 16, iYPos + 109, FONT_SIZE_12, STR(3) );
}

bool PartyCreateWnd::QuestGuide( int iModeIndex, float &rfXPos, float &rfYPos )
{
	// 모드 체크
	if( iModeIndex != m_iSelectTerm )
	{
		ioWnd *pSelectBtn = FindChildWnd( ID_CHANGE_MODE );
		if( pSelectBtn )
		{
			rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
			rfYPos = pSelectBtn->GetDerivedPosY();
			return true;			
		}
		return false;
	}

	// 생성 버튼
	ioWnd *pCreateBtn = FindChildWnd( ID_CREATE );
	if( pCreateBtn && pCreateBtn->IsShow() )
	{
		rfXPos = pCreateBtn->GetDerivedPosX() + ( pCreateBtn->GetWidth() / 2 );
		rfYPos = pCreateBtn->GetDerivedPosY();
		return true;
	}
	return false;
}

void PartyCreateWnd::SetMaxPlayer( int iBlueCount, int iRedCount, int iObserverCount )
{
	m_iMaxPlayerBlue = iBlueCount;
	m_iMaxPlayerRed = iRedCount;
	m_iMaxObserver = iObserverCount;
}
//////////////////////////////////////////////////////////////////////////
PartyCatalogueBtn::PartyCatalogueBtn()
{
	m_pIconBack    = NULL;
	m_pRoomNumBack = NULL;
	m_pBossModeIcon= NULL;
	m_pLockKey     = NULL;
	m_pLockKeyGray = NULL;
	m_pIconEmpty   = NULL;
	m_bIconOver    = false;
}

PartyCatalogueBtn::~PartyCatalogueBtn()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pRoomNumBack );
	SAFEDELETE( m_pBossModeIcon );
	SAFEDELETE( m_pLockKey );
	SAFEDELETE( m_pLockKeyGray );
	SAFEDELETE( m_pIconEmpty );
}

void PartyCatalogueBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "RoomNumBack" )
	{
		SAFEDELETE( m_pRoomNumBack );
		m_pRoomNumBack = pImage;
	}
	else if( szType == "BossModeIcon" )
	{
		SAFEDELETE( m_pBossModeIcon );
		m_pBossModeIcon = pImage;
	}
	else if( szType == "LockKey" )
	{
		SAFEDELETE( m_pLockKey );
		m_pLockKey = pImage;
	}
	else if( szType == "LockKeyGray" )
	{
		SAFEDELETE( m_pLockKeyGray );
		m_pLockKeyGray = pImage;
	}
	else if( szType == "IconEmpty" )
	{
		SAFEDELETE( m_pIconEmpty );
		m_pIconEmpty = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void PartyCatalogueBtn::SetBattleRoomInfo( BattleRoomInfo &rkInfo )
{
	m_BattleRoomInfo = rkInfo;
	if( m_BattleRoomInfo.m_dwIndex != 0 )
	{
		m_szState = g_ModeSelectInfo.GetBattleRoomListTitle( m_BattleRoomInfo.m_iModeSelectIndex );
	}
}

void PartyCatalogueBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	
	m_bIconOver = false;

	if( m_BattleRoomInfo.m_dwIndex != 0 )
	{
		if( g_GUIMgr.GetPreOverWnd() == this ||
			g_GUIMgr.GetPreClickWnd() == this )
		{
			ioLSMouse *pMouse = g_App.GetMouse();

			enum { STARTX = 9, STARTY = 9, WIDTH = 45, HEIGHT = 45, };
			RECT rcIconRect;
			rcIconRect.left   = GetDerivedPosX() + STARTX;
			rcIconRect.top    = GetDerivedPosY() + STARTY;
			rcIconRect.right  = rcIconRect.left + WIDTH;
			rcIconRect.bottom = rcIconRect.top + HEIGHT;
			if( ::PtInRect( &rcIconRect, pMouse->GetMousePos() ) )
			{
				m_bIconOver = true;	
			}
		}    
	}
}

void PartyCatalogueBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_BattleRoomInfo.m_dwIndex == 0 )
	{
		if( m_pIconEmpty )
		{
			m_pIconEmpty->SetScale( 0.60f );
			m_pIconEmpty->Render( iXPos + 31, iYPos + 31, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}

		if( m_pRoomNumBack )
			m_pRoomNumBack->Render( iXPos + 55, iYPos + 34 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
		g_FontMgr.PrintText( iXPos + 57, iYPos + 16, FONT_SIZE_13, STR(1) );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 69, iYPos + 35, FONT_SIZE_10, "-" );

		if( !HasWndStyle( IWS_INACTIVE ) )
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
			g_FontMgr.PrintText( iXPos + 85, iYPos + 34, FONT_SIZE_11, STR(2) );
		}
	}
	else
	{	
		bool bBossMode = false;
		if( m_BattleRoomInfo.m_iMaxPlayerBlue != m_BattleRoomInfo.m_iMaxPlayerRed )    // 보스
		{
			bBossMode = !g_ModeSelectInfo.IsQuickStartFreeForAllMode( m_BattleRoomInfo.m_iModeSelectIndex );
		}

		// 모드 아이콘
		if( m_pIconBack )
		{
			m_pIconBack->SetScale( 0.60f, 0.60f );
            m_pIconBack->Render( iXPos + 31, iYPos + 31, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			if( bBossMode )
			{
				m_pIconBack->SetScale( 0.33f, 0.33f );
				m_pIconBack->Render( iXPos + 17, iYPos + 45, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			}
		}

		ioUIRenderImage *pTypeResource = g_ModeSelectInfo.GetListModeUIResource( m_szState );
		if( pTypeResource )
		{
			pTypeResource->SetScale( 0.60f, 0.60f );
			if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_ACTIVE )
				pTypeResource->Render( iXPos + 31, iYPos + 31, UI_RENDER_NORMAL, TFO_BILINEAR );
			else
				pTypeResource->Render( iXPos + 31, iYPos + 31, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );

			if( m_bIconOver )
				pTypeResource->Render( iXPos + 31, iYPos + 31, UI_RENDER_SCREEN, TFO_BILINEAR );

			if( bBossMode && m_pBossModeIcon )
			{
				m_pBossModeIcon->SetScale( 0.33f, 0.33f );

				if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_ACTIVE )
					m_pBossModeIcon->Render( iXPos + 17, iYPos + 45, UI_RENDER_NORMAL, TFO_BILINEAR );
				else
					m_pBossModeIcon->Render( iXPos + 17, iYPos + 45, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );

				if( m_bIconOver )
					m_pBossModeIcon->Render( iXPos + 17, iYPos + 45, UI_RENDER_SCREEN, TFO_BILINEAR );
			}
		}

		// 전투방 제목
		float fRoomNameLimit = 87.0f;
		if( m_BattleRoomInfo.m_bPassword )
			fRoomNameLimit = 74.0f;

		char szRoomName[MAX_PATH] = "";
		Help::StringCutFun( FONT_SIZE_13, fRoomNameLimit, TS_NORMAL, szRoomName, sizeof(szRoomName), m_BattleRoomInfo.m_szName.c_str() );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_ACTIVE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 57, iYPos + 16, FONT_SIZE_13, STR(3), szRoomName );

		// 비번
		float fRealXPos = g_FontMgr.GetTextWidth( szRoomName, TS_NORMAL, FONT_SIZE_13 );
		if( m_BattleRoomInfo.m_bPassword )
		{
			if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_ACTIVE && m_pLockKey )
				m_pLockKey->Render( iXPos + 57 + fRealXPos + 2, iYPos + 18 );
			else if( m_pLockKeyGray )
				m_pLockKeyGray->Render( iXPos + 57 + fRealXPos + 2, iYPos + 18 );
			fRealXPos += 13.0f;
		}	

		// 인원
		if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_ACTIVE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 57 + fRealXPos + 5, iYPos + 16, FONT_SIZE_13, STR(6), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayer );

		// 방번호
		if( m_pRoomNumBack )
			m_pRoomNumBack->Render( iXPos + 55, iYPos + 34 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( iXPos + 69, iYPos + 35, FONT_SIZE_10, STR(4), m_BattleRoomInfo.m_dwIndex );

		// 모드명
		g_FontMgr.SetAlignType( TAT_LEFT );
		if( m_BattleRoomInfo.m_iState == BattleRoomInfo::BRS_ACTIVE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );

		if( bBossMode ) 
			g_FontMgr.PrintTextWidthCut( iXPos + 85, iYPos + 34, FONT_SIZE_11, 85.0f, STR(7), m_szState.c_str() );
		else
			g_FontMgr.PrintTextWidthCut( iXPos + 85, iYPos + 34, FONT_SIZE_11, 110.f, STR(5), m_szState.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
PartyQuickJoinBtn::PartyQuickJoinBtn()
{
	m_pSetting    = NULL;
}

PartyQuickJoinBtn::~PartyQuickJoinBtn()
{
	SAFEDELETE( m_pSetting );
}

void PartyQuickJoinBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_pSetting )
		m_pSetting->Render( iXPos , iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
}

void PartyQuickJoinBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Setting" )
	{
		SAFEDELETE( m_pSetting );
		m_pSetting = pImage;
		if( m_pSetting )
			m_pSetting->SetScale( 0.70f );
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}
//////////////////////////////////////////////////////////////////////////
PartyCatalogueWnd::PartyCatalogueWnd()
{
	m_pRankJoinBG = NULL;
	m_pRankJoinEffect = NULL;

	m_iPageYPos = 258;
	m_iCurPage  = m_iMaxPage = 0;
	m_dwSearchTime			 = 0;
	m_dwEffectTime = 0;
	m_dwMaxPartyList		 = LOBBY_MAX_PARTY_LIST;

	m_bSuccessionModeOpen = false;
}

PartyCatalogueWnd::~PartyCatalogueWnd()
{
	SAFEDELETE( m_pRankJoinBG );
	SAFEDELETE( m_pRankJoinEffect );

	for( vBattleRoomInfo::iterator iter = m_vBattleRoomList.begin(); iter != m_vBattleRoomList.end(); ++iter )
		SAFEDELETE( *iter );

	m_vBattleRoomList.clear();
	g_pToolTipMgr->CloseToolTip();
}

void PartyCatalogueWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo(xElement);
	m_TopModeType = (TopModeType)xElement.GetIntAttribute_e( "TopModeType" );

	m_szSuccessionText[TEXT_TOOL_TIP] = xElement.GetStringAttribute_e("Succession_ToolTip");
	m_szSuccessionText[TEXT_GRADE_ERROR] = xElement.GetStringAttribute_e("Succession_Error_Grade");
	m_szSuccessionText[TEXT_CLOSE] = xElement.GetStringAttribute_e("Succession_Error_Time");
}

void PartyCatalogueWnd::StartSend( bool bProcess )
{
	if( !g_App.IsMouseRender() ) 
		return;

	SP2Packet kPacket( CTPK_JOIN_BATTLEROOM_LIST );
	kPacket << m_iCurPage << m_dwMaxPartyList << g_BattleRoomMgr.GetQuickStartModeType() 
		    << g_BattleRoomMgr.GetQuickStartMinPlayer( g_BattleRoomMgr.GetQuickStartModeType() ) 
		    << g_BattleRoomMgr.GetQuickStartMaxPlayer( g_BattleRoomMgr.GetQuickStartModeType() )  
		    << g_BattleRoomMgr.IsQuickStartSameUser( g_BattleRoomMgr.GetQuickStartModeType() );
	TCPNetwork::SendToServer( kPacket );	
	m_dwSearchTime = FRAMEGETTIME();
}

bool PartyCatalogueWnd::IsIndexToList( DWORD dwIndex )
{
	for(int i = 0;i < (int)m_vBattleRoomList.size();i++)
	{
		BattleRoomInfo *pInfo = m_vBattleRoomList[i];
		if( !pInfo )
			continue;
		if( pInfo->m_dwIndex == dwIndex )
			return true;
	}
	return false;
}

void PartyCatalogueWnd::ProcessSearch()
{
	if( FRAMEGETTIME() - m_dwSearchTime > PARTY_SEARCH_TIME )
	{
		StartSend( true );
	}
}

void PartyCatalogueWnd::iwm_show()
{
	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		ShowChildWnd( ID_PARTY_CREATE );
		ShowChildWnd( ID_SELECT_MODE );
		ShowChildWnd( ID_QUICK_JOIN );
		HideChildWnd( ID_PARTY_CREATE2 );
		HideChildWnd( ID_SELECT_MODE2 );
		HideChildWnd( ID_QUICK_JOIN2 );
	}
	else
	{
		HideChildWnd( ID_PARTY_CREATE );
		HideChildWnd( ID_SELECT_MODE );
		HideChildWnd( ID_QUICK_JOIN );
		ShowChildWnd( ID_PARTY_CREATE2 );
		ShowChildWnd( ID_SELECT_MODE2 );
		ShowChildWnd( ID_QUICK_JOIN2 );		
	}


	if ( m_TopModeType == TMT_SUFFLE )
	{
		ShowChildWnd( ID_SHUFFLE_JOIN );
		HideChildWnd( ID_RANKBATTLEJOIN );
		HideChildWnd( ID_SUCCESSION_MODE );
	}
	else if ( m_TopModeType == TMT_RANK )
	{
		HideChildWnd( ID_SHUFFLE_JOIN );
		HideChildWnd( ID_SUCCESSION_MODE );
		ShowChildWnd( ID_RANKBATTLEJOIN );
	}
	else if ( m_TopModeType == TMT_SUCCESSION )
	{
		
		HideChildWnd( ID_SHUFFLE_JOIN );
		HideChildWnd( ID_RANKBATTLEJOIN );
		
		CheckSuccessionModeButton();
	}
	m_dwEffectTime = FRAMEGETTIME();
	m_iCurPage = m_iMaxPage = 0;
	for( vBattleRoomInfo::iterator iter = m_vBattleRoomList.begin(); iter != m_vBattleRoomList.end(); ++iter )
		SAFEDELETE( *iter );
	m_vBattleRoomList.clear();

	StartSend();

	for( int i = ID_LIST_1; i <= ID_LIST_8; i++ )
	{
		PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)FindChildWnd( i );
		if( pBtn )
		{
			BattleRoomInfo kInfo;
			pBtn->SetBattleRoomInfo( kInfo );
			pBtn->AddWndStyle( IWS_EXACTIVE );
			if( i < static_cast<int>( ID_LIST_1 + m_dwMaxPartyList ) )
				pBtn->ShowWnd();
			else
				pBtn->HideWnd();
		}
	}
}

void PartyCatalogueWnd::PlazaUIMove()
{
	ioWnd* pLeft = FindChildWnd( ID_PAGE_LEFT );
	if( pLeft )
		pLeft->SetWndPos( pLeft->GetDefaultPosX(), pLeft->GetDefaultPosY() + 65 );

	ioWnd* pRight = FindChildWnd( ID_PAGE_RIGHT );
	if( pRight )
		pRight->SetWndPos( pRight->GetDefaultPosX(), pRight->GetDefaultPosY() + 65 );

	ioWnd* pRefresh = FindChildWnd( ID_REFRESH );
	if( pRefresh )
		pRefresh->SetWndPos( pRefresh->GetDefaultPosX(), pRefresh->GetDefaultPosY() + 65 );

	m_iPageYPos = 323;
}

void PartyCatalogueWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_PARTY_CREATE:
	case ID_PARTY_CREATE2:
		if( cmd == IOBN_BTNUP  )
		{	
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else 
			{
				if( g_MyInfo.GetBlockType() == BKT_LIMITATION )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return;
				}
				g_GUIMgr.ShowWnd( PARTY_CREATE_WND );
			}
		}
		break;
	case ID_SELECT_MODE:
	case ID_SELECT_MODE2:
		if( cmd == IOBN_BTNUP )
		{
			BattleQuickStartOption *pSelectWnd = dynamic_cast<BattleQuickStartOption*>(g_GUIMgr.FindWnd( BATTLE_QUICKSTART_OPTION ));
			if( pSelectWnd )
			{
				pSelectWnd->SetQuickStart( this );
				pSelectWnd->ShowWnd();
			}
		}		
		break;
	case ID_QUICK_JOIN:
	case ID_QUICK_JOIN2:
		if( cmd == IOBN_BTNUP )
		{	
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else if( g_BattleRoomMgr.GetQuickStartModeType() == BMT_USER_CUSTOM )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
			else if( !g_MyInfo.IsTutorialUser() )
			{
				if( g_BattleRoomMgr.IsBattleRoom() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				}
				else if( g_LadderTeamMgr.IsLadderTeam() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
				}
				else
				{
					TimeGateSearch *pSearchWnd = dynamic_cast<TimeGateSearch*>(g_GUIMgr.FindWnd( TIMEGATE_SEARCH ));
					if( pSearchWnd )
					{
						if( pSearchWnd->IsShow() && pSearchWnd->GetSearchType() == TimeGateSearch::ST_PARTY_JOIN )
							pSearchWnd->HideWnd();
						else
						{
							pSearchWnd->SetSearching( STR(5), TimeGateSearch::ST_PARTY_JOIN );		
						}
					}
				}		
			}
		}
		else if( cmd == IOWN_OVERED )
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast< LobbyMainWnd * >( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )				
					pLobbyMainWnd->SetQuickJoinTooltip( this, g_BattleRoomMgr.GetQuickStartInfo().m_iQuickStartType );

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )	
					pTrainingFuncWnd->SetQuickJoinTooltip( this, g_BattleRoomMgr.GetQuickStartInfo().m_iQuickStartType );
			}
			else
			{
				QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pWnd )
					pWnd->SetQuickJoinTooltip( this, g_BattleRoomMgr.GetQuickStartInfo().m_iQuickStartType );
			}
		}
		else if( cmd == IOWN_LEAVED )
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast< LobbyMainWnd * >( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
					pLobbyMainWnd->SetQuickJoinTooltip( NULL, -1 );

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )	
					pTrainingFuncWnd->SetQuickJoinTooltip( NULL, -1 );
			}
			else
			{
				QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pWnd )
					pWnd->SetQuickJoinTooltip( NULL, -1 );
			}
		}
		break;
	case ID_PAGE_LEFT:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( max( 0, m_iCurPage - 1 ) );
		}
		break;
	case ID_PAGE_RIGHT:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( min( m_iMaxPage, m_iCurPage + 1 ) );
		}
		break;
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
	case ID_LIST_6:
	case ID_LIST_7:
	case ID_LIST_8:
		if( cmd == IOBN_BTNUP || cmd == IOEX_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else 
			{
				PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)pWnd;
				if( pBtn->GetBattleRoomIndex() == 0 )
				{
					iwm_command( FindChildWnd( ID_PARTY_CREATE ), IOBN_BTNUP, 0 );
					return;
				}

				if( g_BattleRoomMgr.GetIndex() == pBtn->GetBattleRoomIndex() ) return;

				PartyInfoWnd *pPartyInfoWnd = dynamic_cast<PartyInfoWnd*>(g_GUIMgr.FindWnd( PARTY_INFO_WND ));
				if( pPartyInfoWnd )
				{
					pPartyInfoWnd->HideWnd();
					pPartyInfoWnd->SetBackupData( pBtn->GetBattleRoomIndex(), pBtn->IsPassword(), pBtn->GetBattleRoomState() );

					SP2Packet kPacket( CTPK_BATTLEROOM_JOIN_INFO );
					kPacket << pBtn->GetBattleRoomIndex();
					TCPNetwork::SendToServer( kPacket );					
				}
			}
		}
		break;
	case ID_REFRESH:
		if( cmd == IOBN_BTNUP )
		{
			int a = 100;
			//수련장 용 테스트
//#ifdef _DEBUG
//			g_GUIMgr.HideWnd( PARTY_INVITED_WND );
//			g_GUIMgr.HideWnd( PARTY_INFO_WND );
//			g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
//			SP2Packet kPacket( CTPK_PRACTICE_ENTER );
//			ioINILoader kLoader("config/mode/practicemode/test.ini");
//
//			kLoader.SetTitle_e( "Mode_Index" );
//			int iIndex = kLoader.LoadInt_e("ModeIndex",1);
//			kPacket << g_PracticeMgr.GetPracticeMap(iIndex);//중국클라에선 가이드에있는 멤버변수
//			kPacket << iIndex;
//			LOG.PrintTimeAndLog(0, "수련장 입장1(CTPK_PRACTICE_ENTER)");
//			TCPNetwork::SendToServer( kPacket );
//			if( TCPNetwork::CheckSendToServer( kPacket ) )
//			{
//				TCPNetwork::MouseBusy( true );
//			}
//			g_PracticeMgr.SetPreIndex( iIndex );
//#endif

			SetChildWndStyleAdd( ID_REFRESH, IWS_EXACTIVE );	
			StartSend( false );
		}
		break;
	case ID_SHUFFLE_JOIN:
		if( cmd == IOBN_BTNUP )
		{
			TimeGateSearchShuffle *pSearchWnd = dynamic_cast<TimeGateSearchShuffle*>(g_GUIMgr.FindWnd( TIMEGATE_SEARCH_SHUFFLE ));
			if( pSearchWnd )
			{
				pSearchWnd->SetSearching();
			}
		}
		else if( cmd == IOWN_OVERED )
		{
			g_pToolTipMgr->OpenToolTip( IoToolTipManager::eToolTip_Base, NULL, NULL, NULL, g_ShuffleRoomMgr.GetMenuTooltip().c_str() );
		}
		else if( cmd == IOWN_LEAVED )
		{
			g_pToolTipMgr->CloseToolTip();
		}
		break;
	case ID_RANKBATTLEJOIN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
				return;
			}
			g_GUIMgr.HideWnd( PARTY_INVITED_WND );
			g_GUIMgr.HideWnd( PARTY_INFO_WND );
			g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

			ioMyRoomMode::SetMyRoomType( MRT_RANKBATTLE );
			g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
			g_App.SetModeType( 0, 0, 0, PT_NONE );
		}
		else if( cmd == IOWN_OVERED )
		{
			g_pToolTipMgr->OpenToolTip( IoToolTipManager::eToolTip_Base, NULL, NULL, NULL, g_RankBattleMgr.GetMenuTooltip().c_str() );
		}
		else if( cmd == IOWN_LEAVED )
		{
			g_pToolTipMgr->CloseToolTip();
		}
		break;
	case BATTLE_QUICKSTART_OPTION:
		if( cmd == IOBN_BTNUP )
		{
			// 첫 페이지부터 다시 전송
			m_iCurPage = 0;
			StartSend( false );
		}
		break;
	case ID_SUCCESSION_MODE:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.HideWnd( PARTY_INVITED_WND );
			g_GUIMgr.HideWnd( PARTY_INFO_WND );
			g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

			if ( !g_GUIMgr.IsShow( SUCCESSION_BATTLE_WND ) )
				g_GUIMgr.ShowWnd( SUCCESSION_BATTLE_WND );
			else
				g_GUIMgr.HideWnd( SUCCESSION_BATTLE_WND );
		}
		else if( cmd == IOWN_OVERED )
		{
			if (!m_szSuccessionText[TEXT_TOOL_TIP].IsEmpty())
				g_pToolTipMgr->OpenToolTip( IoToolTipManager::eToolTip_Base, NULL, NULL, NULL, m_szSuccessionText[TEXT_TOOL_TIP].c_str() );
		}
		else if( cmd == IOWN_LEAVED )
			g_pToolTipMgr->CloseToolTip();
		break;
	case ID_SUCCESSION_INACTIVE_MODE:
		if( cmd == IOBN_BTNUP )
		{
			if( g_App.IsSuccessionModeOpen() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szSuccessionText[TEXT_GRADE_ERROR].c_str() );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szSuccessionText[TEXT_CLOSE].c_str() );
			}

		}

		else if( cmd == IOWN_OVERED )
		{
			if (!m_szSuccessionText[TEXT_TOOL_TIP].IsEmpty())
				g_pToolTipMgr->OpenToolTip( IoToolTipManager::eToolTip_Base, NULL, NULL, NULL, m_szSuccessionText[TEXT_TOOL_TIP].c_str() );
		}
		else if( cmd == IOWN_LEAVED )
			g_pToolTipMgr->CloseToolTip();
		break;
	}
}

void PartyCatalogueWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessSearch();		
	ProcessModeHelpTooltip();
	CheckSuccessionModeButton();
}

void PartyCatalogueWnd::ProcessModeHelpTooltip()
{
	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		LobbyMainWnd *pLobbyMainWnd = dynamic_cast< LobbyMainWnd * >( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
		if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
		{
			for( int i = ID_LIST_1; i < static_cast<int>( ID_LIST_1 + m_dwMaxPartyList ); i++ )
			{
				PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)FindChildWnd( i );
				if( !pBtn ) 
					continue;

				if( !pBtn->isIconOver() ) 
					continue;

				int iHelpIndex = g_ModeSelectInfo.GetQuickStartIndexToHelpIndex( pBtn->GetBattleRoomSelectIndex() );
				ioHashString szModeTitle = g_ModeSelectInfo.GetSelectQuickStartModeTitle( pBtn->GetBattleRoomSelectIndex() );					
				pLobbyMainWnd->SetModeHelpTooltip( this, szModeTitle, iHelpIndex );
				return;
			}

			pLobbyMainWnd->SetModeHelpTooltip( NULL, "", -1 );
		}

		TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
		if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )	
		{
			for( int i = ID_LIST_1; i < static_cast<int>( ID_LIST_1 + m_dwMaxPartyList ); i++ )
			{
				PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)FindChildWnd( i );
				if( !pBtn ) 
					continue;

				if( !pBtn->isIconOver() ) 
					continue;

				int iHelpIndex = g_ModeSelectInfo.GetQuickStartIndexToHelpIndex( pBtn->GetBattleRoomSelectIndex() );
				ioHashString szModeTitle = g_ModeSelectInfo.GetSelectQuickStartModeTitle( pBtn->GetBattleRoomSelectIndex() );					
				pTrainingFuncWnd->SetModeHelpTooltip( this, szModeTitle, iHelpIndex );
				return;
			}

			pTrainingFuncWnd->SetModeHelpTooltip( NULL, "", -1 );
		}
	}
	else
	{
		QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
		if( pWnd )
		{
			for( int i = ID_LIST_1; i < static_cast<int>( ID_LIST_1 + m_dwMaxPartyList ); i++ )
			{
				PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)FindChildWnd( i );
				if( !pBtn )
					continue;

				if( !pBtn->isIconOver() )
					continue;

				int iHelpIndex = g_ModeSelectInfo.GetQuickStartIndexToHelpIndex( pBtn->GetBattleRoomSelectIndex() );
				ioHashString szModeTitle = g_ModeSelectInfo.GetSelectQuickStartModeTitle( pBtn->GetBattleRoomSelectIndex() );					
				pWnd->SetModeHelpTooltip( this, szModeTitle, iHelpIndex );
				return;
			}

			pWnd->SetModeHelpTooltip( NULL, "", -1 );
		}		
	}		
}

void PartyCatalogueWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "RankJoinBG" )
	{
		SAFEDELETE( m_pRankJoinBG );
		m_pRankJoinBG = pImage;
	}
	else if( szType == "RankJoinEffect" )
	{
		SAFEDELETE( m_pRankJoinEffect );
		m_pRankJoinEffect = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PartyCatalogueWnd::CheckSuccessionModeButton()
{
	if( g_App.IsSuccessionModeOpen() && !m_bSuccessionModeOpen )
	{
		int iGradeLevel = g_MyInfo.GetGradeLevel();
		if ( iGradeLevel < 15 )
		{
			HideChildWnd( ID_SUCCESSION_MODE );
			ShowChildWnd( ID_SUCCESSION_INACTIVE_MODE );
		}
		else
		{
			ShowChildWnd( ID_SUCCESSION_MODE );
			HideChildWnd( ID_SUCCESSION_INACTIVE_MODE );
		}

		m_bSuccessionModeOpen = true;
	}
	else if( !g_App.IsSuccessionModeOpen() && m_bSuccessionModeOpen )
	{
		HideChildWnd( ID_SUCCESSION_MODE );
		ShowChildWnd( ID_SUCCESSION_INACTIVE_MODE );

		m_bSuccessionModeOpen = false;
	}
}

void PartyCatalogueWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 201, iYPos + m_iPageYPos, FONT_SIZE_12, STR(1), m_iCurPage + 1 );
}

void PartyCatalogueWnd::OnRenderAfterChild()
{
	if( !g_App.IsSuccessionModeOpen() )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pRankJoinBG )
		m_pRankJoinBG->Render( iXPos + 363, iYPos - 14, UI_RENDER_NORMAL, TFO_BILINEAR );
	if( m_pRankJoinEffect )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		DWORD dwGapTime = dwCurTime - m_dwEffectTime;
		if( dwGapTime < 500 )
			m_pRankJoinEffect->SetAlpha( MAX_ALPHA_RATE - MAX_ALPHA_RATE * dwGapTime / 1000 );
		else if( dwGapTime < 1000 )
			m_pRankJoinEffect->SetAlpha( MAX_ALPHA_RATE * dwGapTime / 1000 );
		else
		{
			m_dwEffectTime = dwCurTime;
			m_pRankJoinEffect->SetAlpha( MAX_ALPHA_RATE );
		}
		m_pRankJoinEffect->Render( iXPos + 363, iYPos - 14, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void PartyCatalogueWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = iCurPage;

	if( m_iCurPage != iPrevPage )
		StartSend();

	int iListSIze = m_vBattleRoomList.size();
	int iStartPos = 0;
	for( int i = ID_LIST_1; i < static_cast<int>( ID_LIST_1 + m_dwMaxPartyList ); i++, iStartPos++ )
	{
		PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)FindChildWnd( i );
		if( pBtn )
		{
			if( iStartPos < iListSIze )
			{
				pBtn->SetBattleRoomInfo( *m_vBattleRoomList[iStartPos] );
				pBtn->RemoveWndStyle( IWS_EXACTIVE );
			}
			else
			{
				BattleRoomInfo kInfo;
				pBtn->SetBattleRoomInfo( kInfo );
				pBtn->AddWndStyle( IWS_EXACTIVE );
			}
		}
	}	
}

void PartyCatalogueWnd::SetPartyList( SP2Packet &rkPacket )
{	
	for(vBattleRoomInfo::iterator iter = m_vBattleRoomList.begin(); iter != m_vBattleRoomList.end(); ++iter)
		SAFEDELETE( *iter );
	m_vBattleRoomList.clear();

	PartyInfoWnd *pPartyInfoWnd = dynamic_cast<PartyInfoWnd*>(g_GUIMgr.FindWnd( PARTY_INFO_WND ));
	if( !pPartyInfoWnd ) return;

	int i = 0;
	int iTotalList;
	int iCurList;
	rkPacket >> iTotalList >> iCurList;		
	for(i = 0;i < iCurList; i++ )
	{
		BattleRoomInfo *pInfo = new BattleRoomInfo;
		if( !pInfo )
			continue;
		rkPacket >> pInfo->m_dwIndex >> pInfo->m_iAverageLevel >> pInfo->m_szName >> pInfo->m_iCurJoiner >> pInfo->m_iCurPlayer
				 >> pInfo->m_iMaxPlayerBlue >> pInfo->m_iMaxPlayerRed >> pInfo->m_iMaxObserver >> pInfo->m_iModeSelectIndex >> pInfo->m_bPassword >> pInfo->m_iState
				 >> pInfo->m_bUseExtraOption >> pInfo->m_bNoChallenger;

		pInfo->m_iMaxPlayer = pInfo->m_iMaxPlayerBlue + pInfo->m_iMaxPlayerRed;
		pInfo->m_iNumber = ( m_iCurPage * m_dwMaxPartyList ) + i;
		m_vBattleRoomList.push_back( pInfo );
		pPartyInfoWnd->UpdataBattleRoomState( pInfo->m_dwIndex, pInfo->m_iState );
	}
    
	m_iMaxPage = max( 0, iTotalList - 1 ) / m_dwMaxPartyList;
	UpdateSetPage( m_iCurPage );
	SetChildWndStyleRemove( ID_REFRESH, IWS_EXACTIVE );
}

bool PartyCatalogueWnd::QuestGuide( int iModeSelectIndex, float &rfXPos, float &rfYPos )
{
	for( int i = ID_LIST_1; i < static_cast<int>( ID_LIST_1 + m_dwMaxPartyList ); i++ )
	{
		PartyCatalogueBtn *pBtn = (PartyCatalogueBtn*)FindChildWnd( i );
		if( pBtn && pBtn->IsShow() )
		{
			if( pBtn->GetBattleRoomIndex() == 0 ) continue;
			if( pBtn->GetBattleRoomSelectIndex() == iModeSelectIndex )
			{
				rfXPos = pBtn->GetDerivedPosX() + ( pBtn->GetWidth() / 2 );
				rfYPos = pBtn->GetDerivedPosY();
				return true;
			}
		}
	}	

	if( m_iMaxPage == 0 )
		return false;

	// 리스트에 없으면 페이지 버튼에 표시
	if( m_iCurPage >= m_iMaxPage )
	{
		ioWnd *pLeftBtn = FindChildWnd( ID_PAGE_LEFT );
		if( pLeftBtn )
		{
			rfXPos = pLeftBtn->GetDerivedPosX() + ( pLeftBtn->GetWidth() / 2 );
			rfYPos = pLeftBtn->GetDerivedPosY();
			return true;
		}
	}
	else
	{
		ioWnd *pRightBtn = FindChildWnd( ID_PAGE_RIGHT );
		if( pRightBtn )
		{
			rfXPos = pRightBtn->GetDerivedPosX() + ( pRightBtn->GetWidth() / 2 );
			rfYPos = pRightBtn->GetDerivedPosY();
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
PartyInvitedBtn::PartyInvitedBtn()
{
	m_pBlueNormal = NULL;
	m_pRedNormal = NULL;
	m_pNoneNormal = NULL;
	m_pBlueOver = NULL;
	m_pRedOver = NULL;
	m_pNoneOver = NULL;

	m_pPeopleOrange = NULL;
	m_pPeopleGray   = NULL;

	m_eTeam = TEAM_BLUE;
	m_iSlotArray = m_iMaxSlot = 0;
	m_iGradeLevel = m_iPingStep = 0;
}

PartyInvitedBtn::~PartyInvitedBtn()
{
	SAFEDELETE( m_pBlueNormal );
	SAFEDELETE( m_pRedNormal );
	SAFEDELETE( m_pNoneNormal );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedOver );
	SAFEDELETE( m_pNoneOver );

	SAFEDELETE( m_pPeopleOrange );
	SAFEDELETE( m_pPeopleGray );
}

void PartyInvitedBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNormal" )
	{
		SAFEDELETE( m_pBlueNormal );
		m_pBlueNormal = pImage;
	}
	else if( szType == "RedNormal" )
	{
		SAFEDELETE( m_pRedNormal );
		m_pRedNormal = pImage;
	}
	else if( szType == "NoneNormal" )
	{
		SAFEDELETE( m_pNoneNormal );
		m_pNoneNormal = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else if( szType == "NoneOver" )
	{
		SAFEDELETE( m_pNoneOver );
		m_pNoneOver = pImage;
	}
	else if( szType == "PeopleOrange" )
	{
		SAFEDELETE( m_pPeopleOrange );
		m_pPeopleOrange = pImage;
	}
	else if( szType == "PeopleGray" )
	{
		SAFEDELETE( m_pPeopleGray );
		m_pPeopleGray = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void PartyInvitedBtn::iwm_create()
{
	m_PingStep.Create();
}

void PartyInvitedBtn::SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, int iPingStep, TeamType eTeam )
{
	m_iMaxSlot   = iMaxSlot;
	m_iSlotArray = iSlotArray;
	m_szName     = rkName;
	m_eTeam      = eTeam;
	m_iGradeLevel= iGradeLevel;
	m_iPingStep  = iPingStep;

	if( m_szName.IsEmpty() )
	{
		SetInActive();
	}
	else
	{	
		SetActive();
	}
}

void PartyInvitedBtn::OnRenderBase( int iXPos, int iYPos )
{
	ioUIRenderImage *pNormal = NULL;
	ioUIRenderImage *pOver   = NULL;
	switch( m_eTeam )
	{
	case TEAM_BLUE:
		{
			pNormal = m_pBlueNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pBlueOver;
		}
		break;
	case TEAM_RED:
		{
			pNormal = m_pRedNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pRedOver;
		}
		break;
	default:
		{
			pNormal = m_pNoneNormal;
			if( m_bOver || m_bClicked )
				pOver = m_pNoneOver;
		}
		break;
	}

	if( pNormal )
		pNormal->Render( iXPos, iYPos );
	if( pOver )
		pOver->Render( iXPos, iYPos );
}

void PartyInvitedBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();
	
	OnRenderBase( iXPos, iYPos );
	
	if( m_szName.IsEmpty() )
	{
		if( m_iSlotArray < m_iMaxSlot )
		{
			if( m_pPeopleOrange )
				m_pPeopleOrange->Render( iXPos + 4, iYPos + 2 );
		}
		else
		{
			if( m_pPeopleGray )
				m_pPeopleGray->Render( iXPos + 4, iYPos + 2 );			
		}
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );	
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( m_bOver || m_bClicked )
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, RGB( 255, 255, 255 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else
		{
			g_LevelMgr.RenderGrade( iXPos + 4, iYPos + 2, m_iGradeLevel, TEAM_PRIVATE_1 );

			if( m_eTeam == TEAM_BLUE )
				g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			else if( m_eTeam == TEAM_RED )
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			else
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 59.0f, m_szName.c_str() );

		m_PingStep.RenderPing( iXPos + 85, iYPos + 2, m_iPingStep, false );
	}
}

void PartyInvitedBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( m_szName.IsEmpty() ) return;

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse && pMouse->IsMouseShowing() )
			pMouse->SetToolTipID( m_szName, UserInfoToolTip::TT_GLOBAL );
	}	
}
//////////////////////////////////////////////////////////////////////////
bool PartyInvitedWnd::sg_bAutoRefuse = false;
PartyInvitedWnd::PartyInvitedWnd()
{
	m_dwStartTime = 0;
	m_iCurPage	  = 0;
	m_pPlayStage  = NULL;
	m_iMaxPage    = 1;

	m_pModeIconBack = NULL;
	m_bIconOver			= false;
	m_pModeHelpTooltip  = NULL;
}

PartyInvitedWnd::~PartyInvitedWnd()
{
	m_vUserList.clear();

	SAFEDELETE( m_pModeIconBack );
}

void PartyInvitedWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PartyInvitedWnd::SetInfo(  SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;

	m_vUserList.clear();
	bool bBattleRoom;
	rkPacket >> m_BattleRoomInfo.m_dwIndex;
	rkPacket >> m_BattleRoomInfo.m_szName;
	rkPacket >> m_BattleRoomInfo.m_szOwnerName;
	rkPacket >> m_BattleRoomInfo.m_iCurJoiner;
	rkPacket >> m_BattleRoomInfo.m_iCurPlayer;
	rkPacket >> m_BattleRoomInfo.m_iMaxPlayerBlue;
	rkPacket >> m_BattleRoomInfo.m_iMaxPlayerRed;
	rkPacket >> m_BattleRoomInfo.m_iMaxObserver;
	rkPacket >> m_BattleRoomInfo.m_szPassword;
	rkPacket >> m_BattleRoomInfo.m_iAverageLevel;

	rkPacket >> m_BattleRoomInfo.m_bUseExtraOption;
	rkPacket >> m_BattleRoomInfo.m_bNoChallenger;

	rkPacket >> m_BattleRoomInfo.m_iChangeCharType;
	rkPacket >> m_BattleRoomInfo.m_iCoolTimeType;
	rkPacket >> m_BattleRoomInfo.m_iRedHPType;
	rkPacket >> m_BattleRoomInfo.m_iBlueHPType;
	rkPacket >> m_BattleRoomInfo.m_iDropDamageType;
	rkPacket >> m_BattleRoomInfo.m_iGravityType;
	rkPacket >> m_BattleRoomInfo.m_iPreSetMode;
	rkPacket >> m_BattleRoomInfo.m_iTeamAttackType;
	rkPacket >> m_BattleRoomInfo.m_iGetUpType;
	rkPacket >> m_BattleRoomInfo.m_iKOType;
	rkPacket >> m_BattleRoomInfo.m_iRedBlowType;
	rkPacket >> m_BattleRoomInfo.m_iBlueBlowType;
	rkPacket >> m_BattleRoomInfo.m_iRedMoveSpeedType;
	rkPacket >> m_BattleRoomInfo.m_iBlueMoveSpeedType;
	rkPacket >> m_BattleRoomInfo.m_iKOEffectType;
	rkPacket >> m_BattleRoomInfo.m_iRedEquipType;
	rkPacket >> m_BattleRoomInfo.m_iBlueEquipType;
	rkPacket >> m_BattleRoomInfo.m_iCatchModeRoundType;
	rkPacket >> m_BattleRoomInfo.m_iCatchModeRoundTimeType;
	rkPacket >> m_BattleRoomInfo.m_iGrowthUseType;
	rkPacket >> m_BattleRoomInfo.m_iExtraItemUseType;
	rkPacket >> bBattleRoom;
	
	bool bFriendInParty = false;
	int iUserSize = 0;
	rkPacket >> iUserSize;

	if( iUserSize > 0 )
	{
		for(int i = 0;i < iUserSize;i++)
		{
			DWORD dwGuildIndex, dwGuildMark;
			BattleRoomUserInfo kUserInfo;
			rkPacket >> kUserInfo.m_iGradeLevel >> kUserInfo.m_szUserID >> kUserInfo.m_bObserver >> kUserInfo.m_iTeam 
					 >> dwGuildIndex >> dwGuildMark >> kUserInfo.m_iServerPingStep;
			
			m_vUserList.push_back( kUserInfo );
			g_GuildInfo.SetGuildMark( kUserInfo.m_szUserID, dwGuildIndex, dwGuildMark );
			if( !bFriendInParty )
			{
				if( g_FriendManager.IsFriend(kUserInfo.m_szUserID) || g_GuildInfo.IsMyGuildUser( kUserInfo.m_szUserID )  )
				{
					bFriendInParty = true;
					// 친구 정보 갱신
					g_FriendManager.ClientFriendInfoUpdate( kUserInfo.m_szUserID, kUserInfo.m_iGradeLevel, UP_BATTLE_ROOM );
				}
			}
		}
	}

	if( !bFriendInParty )
	{
		if( Setting::AutoRejectParty() || sg_bAutoRefuse )	 
		{
			return;
		}

		// 거절한 전투방에서 1분이내에 초대가 올 경우 초대 무시 단, 친구의 초대는 예외
		if( g_BattleRoomMgr.IsInvitedRefuseRoom( m_BattleRoomInfo.m_dwIndex ) )
		{
			return;
		}
	}
	g_SoundMgr.PlaySound( "interface/team_invite.wav" );
	m_dwStartTime = FRAMEGETTIME();

	// 전투 정보
	rkPacket >> m_BattleRoomInfo.m_iModeSelectIndex >> m_BattleRoomInfo.m_iPlayState >> m_BattleRoomInfo.m_iModeType;
	switch( m_BattleRoomInfo.m_iPlayState )
	{
	case BST_PLAYING:
		{
			if( m_BattleRoomInfo.m_iModeType != MT_MONSTER_SURVIVAL && m_BattleRoomInfo.m_iModeType != MT_DUNGEON_A && m_BattleRoomInfo.m_iModeType != MT_RAID &&
				!Help::IsMonsterDungeonMode((ModeType)m_BattleRoomInfo.m_iModeType))
			{
				if( m_BattleRoomInfo.m_iModeType == MT_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_TEAM_SURVIVAL || 
					m_BattleRoomInfo.m_iModeType == MT_TEAM_SURVIVAL_AI || m_BattleRoomInfo.m_iModeType == MT_BATTLE ||
					m_BattleRoomInfo.m_iModeType == MT_BOSS || m_BattleRoomInfo.m_iModeType == MT_GANGSI || m_BattleRoomInfo.m_iModeType == MT_FIGHT_CLUB ||
					m_BattleRoomInfo.m_iModeType == MT_DOUBLE_CROWN )
				{
					rkPacket >> m_BattleRoomInfo.m_dwRemainPlayTime;
				}
				else
					rkPacket >> m_BattleRoomInfo.m_iBlueTeamWinCount >> m_BattleRoomInfo.m_iRedTeamWinCount;
			}			
		}
		break;
	}

	// UI TITLE	
	char szTitle[MAX_PATH] = "";
	switch( m_BattleRoomInfo.m_iModeType )
	{
	case MT_NONE:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_SURVIVAL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_FIGHT_CLUB:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(15), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_TEAM_SURVIVAL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(3), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_TOWER_DEFENSE://UNDONE Tower defense PartyInviteWnd
		SafeSprintf( szTitle, sizeof( szTitle ), "깊은 숲 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_DARK_XMAS:
		SafeSprintf( szTitle, sizeof( szTitle ), "스노우타운 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_FIRE_TEMPLE:
		SafeSprintf( szTitle, sizeof( szTitle ), "불의 신전 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_SYMBOL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(4), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_CATCH:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(5), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_CATCH_RUNNINGMAN:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(16), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_KING:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(6), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_MONSTER_SURVIVAL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(7), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_BOSS:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(8), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_FOOTBALL:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(9), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_HEROMATCH:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(10), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_GANGSI:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(12), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_DUNGEON_A:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(14), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_DOUBLE_CROWN:
		SafeSprintf( szTitle, sizeof( szTitle ), "더블 크라운 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_FACTORY:
		SafeSprintf( szTitle, sizeof( szTitle ), "비밀공장 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_TEAM_SURVIVAL_AI:
		SafeSprintf( szTitle, sizeof( szTitle ), "AI팀데스매치 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_RAID:
		SafeSprintf( szTitle, sizeof( szTitle ), "레이드 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_UNDERWEAR:
		SafeSprintf( szTitle, sizeof( szTitle ), "팬티몬 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_CBT:
		SafeSprintf( szTitle, sizeof( szTitle ), "클베모드 %d/%d", m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_FLAG_CAPTURE:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(17), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_ARENA:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(18), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_BATTLE:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(19), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	case MT_FARMING:
		SafeSprintf( szTitle, sizeof( szTitle ), STR(20), m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );
		break;
	}

	bool bUseExtraOption = m_BattleRoomInfo.m_bUseExtraOption;

	if( m_BattleRoomInfo.m_iModeType == MT_UNDERWEAR || m_BattleRoomInfo.m_iModeType == MT_CBT || m_BattleRoomInfo.m_iModeType == MT_FARMING )
		bUseExtraOption = false;

	if( bUseExtraOption )
		SafeSprintf( szTitle, sizeof( szTitle ), STR(11), g_ModeSelectInfo.GetCustomModeTitle().c_str(),  m_BattleRoomInfo.m_iCurPlayer, m_BattleRoomInfo.m_iMaxPlayerBlue + m_BattleRoomInfo.m_iMaxPlayerRed );

	SetTitleText( szTitle );

	if( g_ModeSelectInfo.GetQuickStartIndexToManualIndex( m_BattleRoomInfo.m_iModeSelectIndex ) != -1 )
	{
		ShowChildWnd( ID_MANUAL_BTN );
	}
	else
	{
		HideChildWnd( ID_MANUAL_BTN );
	}

	if( IsShow() )
		HideWnd();

	ShowWnd();

	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
}

const BattleRoomUserInfo &PartyInvitedWnd::GetUserInfo( TeamType eTeam, int iArray )
{
	for(int i = 0;i < (int)m_vUserList.size();i++)
	{
		BattleRoomUserInfo &rkInfo = m_vUserList[i];
		if( rkInfo.m_iTeam != (int)eTeam ) continue;
		if( rkInfo.m_bObserver ) continue;
			
		if( --iArray < 0 )
			return m_vUserList[i];
	}
	static BattleRoomUserInfo kReturnInfo;
	return kReturnInfo;
}

void PartyInvitedWnd::UpdatePage( int iCurPage )
{
	m_iCurPage = max( 0, iCurPage );
	m_iCurPage = min( m_iCurPage, m_iMaxPage );
	const int iStartArray = m_iCurPage * MAX_PAGE_LIST;
	TeamType eBlueBtnTeam = TEAM_BLUE;
	TeamType eRedBtnTeam  = TEAM_RED;

	switch( m_BattleRoomInfo.m_iModeType )
	{
	case MT_NONE:
	case MT_SURVIVAL:
	case MT_FIGHT_CLUB:
	case MT_BOSS:
	case MT_MONSTER_SURVIVAL:
	case MT_DUNGEON_A:
	case MT_GANGSI:
	case MT_RAID:
		{
			eBlueBtnTeam = eRedBtnTeam = TEAM_NONE;
		}
		break;
	}

	if(Help::IsMonsterDungeonMode((ModeType)m_BattleRoomInfo.m_iModeType))
		eBlueBtnTeam = eRedBtnTeam = TEAM_NONE;

	int i = 0;
	int iSlotArray = 0;		
	// 블루팀
	for(i = 0,iSlotArray = iStartArray;i < MAX_PAGE_LIST;i++,iSlotArray++)
	{
		PartyInvitedBtn *pBlueBtn = dynamic_cast<PartyInvitedBtn*>(FindChildWnd( ID_BLUE_TEAM_1 + i ));
		if( pBlueBtn )
		{
			const BattleRoomUserInfo &rkInfo = GetUserInfo( TEAM_BLUE, iSlotArray );
			pBlueBtn->SetMemberInfo( m_BattleRoomInfo.m_iMaxPlayerBlue, iSlotArray, rkInfo.m_szUserID, rkInfo.m_iGradeLevel, rkInfo.m_iServerPingStep, eBlueBtnTeam );
		}
	}

	// 레드팀
	for(i = 0,iSlotArray = iStartArray;i < MAX_PAGE_LIST;i++,iSlotArray++)
	{
		PartyInvitedBtn *pBlueBtn = dynamic_cast<PartyInvitedBtn*>(FindChildWnd( ID_RED_TEAM_1 + i ));
		if( pBlueBtn )
		{
			const BattleRoomUserInfo &rkInfo = GetUserInfo( TEAM_RED, iSlotArray );
			pBlueBtn->SetMemberInfo( m_BattleRoomInfo.m_iMaxPlayerRed, iSlotArray, rkInfo.m_szUserID, rkInfo.m_iGradeLevel, rkInfo.m_iServerPingStep, eRedBtnTeam );
		}
	}
}

void PartyInvitedWnd::JoinBattleRoom()
{
	TCPNetwork::MouseBusy( true );
	SP2Packet kJPacket( CTPK_USER_BATTLEROOM_JOIN );
	// Observer << Invite << PenaltyPeso << iPenaltyPeso << .... 순서로
	kJPacket << false << true << false << g_MyInfo.GetCurPenaltyPeso() << m_BattleRoomInfo.m_dwIndex << m_BattleRoomInfo.m_szPassword;
	TCPNetwork::SendToServer( kJPacket );
}

bool PartyInvitedWnd::iwm_esc()
{
	if( ioWnd::iwm_esc() )
	{
		SetRefuse();
		return true;
	}
	return false;
}

void PartyInvitedWnd::SetRefuse()
{
	if( m_BattleRoomInfo.m_dwIndex != 0 )
		g_BattleRoomMgr.SetInvitedRefuse( m_BattleRoomInfo.m_dwIndex );
}

void PartyInvitedWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_REFUSE:
		if( cmd == IOBN_BTNUP )
		{			
			HideWnd();
			SetRefuse();
		}
		break;
	case ID_AUTO_REFUSE:
		if( cmd == IOBN_BTNUP )
		{	
			sg_bAutoRefuse = !sg_bAutoRefuse;
			if( sg_bAutoRefuse )
			{
				HideWnd();
				SetRefuse();
			}			
		}
		break;
	case ID_LEFT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdatePage( m_iCurPage - 1 );
		}
		break;
	case ID_RIGHT_PAGE:
		if( cmd == IOBN_BTNUP )
		{	
			UpdatePage( m_iCurPage + 1 );
		}
		break;
	case ID_MANUAL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iManualIndex = g_ModeSelectInfo.GetQuickStartIndexToManualIndex( m_BattleRoomInfo.m_iModeSelectIndex );
			if( iManualIndex != -1 )
			{
				ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
				if( pManualWnd )
					pManualWnd->ShowWndByCurManual( iManualIndex );
			}
		}
		break;
	case ID_AGREE:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
			}
			else if(  g_MyInfo.GetActiveCharCount() == 0 || 
				      ( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else if( ( m_BattleRoomInfo.m_iModeType == MT_MONSTER_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_DUNGEON_A ) && 
						g_MyInfo.GetEtcMonsterCoin() + g_MyInfo.GetEtcGoldMonsterCoin() < g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex ) )
			{
				MonsterCoinUseWnd *pMonsterCoinWnd = dynamic_cast<MonsterCoinUseWnd*>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
				if( pMonsterCoinWnd )
				{
					int iUseCoinCnt = g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex );
					pMonsterCoinWnd->ShowJoinCoinLack( g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_BattleRoomInfo.m_iModeSelectIndex ), iUseCoinCnt, (ModeType)m_BattleRoomInfo.m_iModeType );
					return;
				}

				//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
				return;
			}
			else if( m_BattleRoomInfo.m_iModeType == MT_RAID && 
				g_MyInfo.GetEtcRaidCoin() < g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex ) )
			{
				MonsterCoinUseWnd *pMonsterCoinWnd = dynamic_cast<MonsterCoinUseWnd*>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
				if( pMonsterCoinWnd )
				{
					int iUseCoinCnt = g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex );
					pMonsterCoinWnd->ShowJoinCoinLack( g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_BattleRoomInfo.m_iModeSelectIndex ), iUseCoinCnt, (ModeType)m_BattleRoomInfo.m_iModeType );
					return;
				}

				//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
				return;
			}
			else if(Help::IsMonsterDungeonMode((ModeType)m_BattleRoomInfo.m_iModeType) && 
				g_MyInfo.GetEtcMonsterCoin() < g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex ))
			{
				MonsterCoinUseWnd *pMonsterCoinWnd = dynamic_cast<MonsterCoinUseWnd*>( g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
				if( pMonsterCoinWnd )
				{
					int iUseCoinCnt = g_ModeSelectInfo.GetSelectQuickStartModeUseCoinCnt( m_BattleRoomInfo.m_iModeSelectIndex );
					pMonsterCoinWnd->ShowJoinCoinLack( g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_BattleRoomInfo.m_iModeSelectIndex ), iUseCoinCnt, (ModeType)m_BattleRoomInfo.m_iModeType );
					return;
				}

				//UJ 140120, 보안강화: 몬스터 코인 없이 계속 플레이할 수 있는 치팅을 막는다
				return;
			}
			else
			{
				bool bUseExtraOption = m_BattleRoomInfo.m_bUseExtraOption;

				if( m_BattleRoomInfo.m_iModeType == MT_UNDERWEAR || m_BattleRoomInfo.m_iModeType == MT_CBT || m_BattleRoomInfo.m_iModeType == MT_FARMING )
					bUseExtraOption = false;

				if( bUseExtraOption )
				{
					ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );
					kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2), g_ModeSelectInfo.GetCustomModeTitle().c_str() );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );
					kPrinter[1].SetTextColor( TCT_DEFAULT_ORANGE );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(3) );

					kPrinter[2].SetTextStyle( TS_NORMAL );
					kPrinter[2].SetBkColor( 0, 0, 0 );
					kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(4) );

					g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, STR(5), STR(6), STR(7), STR(8) );
					return;
				}
				else
				{
					JoinBattleRoom();
				}
			}
			HideWnd();
		}
		break;
	case ID_BLUE_TEAM_1:
	case ID_BLUE_TEAM_2:
	case ID_BLUE_TEAM_3:
	case ID_BLUE_TEAM_4:
	case ID_RED_TEAM_1:
	case ID_RED_TEAM_2:
	case ID_RED_TEAM_3:
	case ID_RED_TEAM_4:
		if( cmd == IOBN_BTNUP )
		{
			PartyInvitedBtn *pUserBtn = dynamic_cast<PartyInvitedBtn*>(pWnd);
			if( pUserBtn )
				g_App.OpenMannerWnd( pUserBtn->GetMemberName().c_str(), true );
		}
		break;
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				JoinBattleRoom();
			}

			HideWnd();
		}
		break;
	}
}

void PartyInvitedWnd::iwm_show()
{
	ioButton *pAgreeBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_AGREE ) );
	if( pAgreeBtn )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), INVITE_DELAY_SEC );
		pAgreeBtn->SetTitleText( szTitle );
	}

	ioCheckButton *pCheckBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_AUTO_REFUSE ) );
	if( pCheckBtn )
		pCheckBtn->SetCheck( sg_bAutoRefuse );

	UpdatePage( 0 );	
}

void PartyInvitedWnd::iwm_hide()
{
	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}	
}

void PartyInvitedWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( 0.60f, 0.60f );
		m_pModeIconBack->Render( iXPos + 39, iYPos + 70, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		// 모드 아이콘
		ioHashString kModeTitle = g_ModeSelectInfo.GetBattleRoomListTitle( m_BattleRoomInfo.m_iModeSelectIndex );
		ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetListModeUIResource( kModeTitle );
		if( pModeIcon )
		{
			pModeIcon->SetScale( 0.60f, 0.60f );
			pModeIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_NORMAL, TFO_BILINEAR );
			if( m_bIconOver )
				pModeIcon->Render( iXPos + 39, iYPos + 70, UI_RENDER_SCREEN, TFO_BILINEAR );
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + 65, iYPos + 55, FONT_SIZE_13, 175.0f, m_BattleRoomInfo.m_szName.c_str() );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	if( m_BattleRoomInfo.m_iPlayState == BST_START )
	{
		if( m_BattleRoomInfo.m_iModeType == MT_MONSTER_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_DUNGEON_A )
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(1), g_ModeSelectInfo.GetSelectQuickStartModeSubTitle( m_BattleRoomInfo.m_iModeSelectIndex ).c_str() );
		else
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(2) );
	}
	else if( m_BattleRoomInfo.m_iPlayState == BST_RESULT )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		if( m_BattleRoomInfo.m_iModeType == MT_MONSTER_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_DUNGEON_A )
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(3), g_ModeSelectInfo.GetSelectQuickStartModeSubTitle( m_BattleRoomInfo.m_iModeSelectIndex ).c_str() );
		else
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(4) );
	}
	else
	{
		if( m_BattleRoomInfo.m_iModeType == MT_MONSTER_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_DUNGEON_A || m_BattleRoomInfo.m_iModeType == MT_RAID ||  
			Help::IsMonsterDungeonMode((ModeType)m_BattleRoomInfo.m_iModeType) )
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(5), g_ModeSelectInfo.GetSelectQuickStartModeSubTitle( m_BattleRoomInfo.m_iModeSelectIndex ).c_str() );
		else if( m_BattleRoomInfo.m_iModeType == MT_SURVIVAL || m_BattleRoomInfo.m_iModeType == MT_TEAM_SURVIVAL || 
				 m_BattleRoomInfo.m_iModeType == MT_TEAM_SURVIVAL_AI || m_BattleRoomInfo.m_iModeType == MT_BATTLE ||
				 m_BattleRoomInfo.m_iModeType == MT_BOSS || m_BattleRoomInfo.m_iModeType == MT_GANGSI || m_BattleRoomInfo.m_iModeType == MT_FIGHT_CLUB ||
				 m_BattleRoomInfo.m_iModeType == MT_DOUBLE_CROWN )
		{
			g_FontMgr.PrintText( iXPos + 65, iYPos + 73, FONT_SIZE_11, STR(6), m_BattleRoomInfo.m_dwRemainPlayTime / 1000 );
		}
		else
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );

			kPrinter.SetTextColor( 0, 150, 255 );
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(7), m_BattleRoomInfo.m_iBlueTeamWinCount );

			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_11, "vs " );

			kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(8), m_BattleRoomInfo.m_iRedTeamWinCount );

			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(9), m_BattleRoomInfo.m_iBlueTeamWinCount + m_BattleRoomInfo.m_iRedTeamWinCount + 1 );

			kPrinter.PrintFullText( iXPos + 65, iYPos + 73, TAT_LEFT );
		}
	}
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(10), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(11), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 128, iYPos + 193, TAT_CENTER );
}

void PartyInvitedWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
	ProcessModeHelpTooltip();

	if(m_dwStartTime == 0)
		return;

	if( FRAMEGETTIME() - m_dwStartTime >= INVITE_DELAY_SEC * 1000 )
	{
		HideWnd();	
		m_dwStartTime = 0;
	}
	else
	{
		ioButton *pBtn = (ioButton*)FindChildWnd( ID_AGREE );
		if( pBtn )
		{
			DWORD dwSec = ((float)(FRAMEGETTIME() - m_dwStartTime) / 1000);
			int   iSec  = max( 0, INVITE_DELAY_SEC - dwSec );	
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), iSec );
			pBtn->SetTitleText( szTitle );
		}
	}
}

void PartyInvitedWnd::ProcessModeHelpTooltip()
{
	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		m_bIconOver = false;

		if( g_App.IsMouseRender() )
		{
			if( g_GUIMgr.GetPreOverWnd() == this ||
				g_GUIMgr.GetPreClickWnd() == this )
			{
				ioLSMouse *pMouse = g_App.GetMouse();

				enum { STARTX = 17, STARTY = 48, WIDTH = 45, HEIGHT = 45, };
				RECT rcIconRect;
				rcIconRect.left   = GetDerivedPosX() + STARTX;
				rcIconRect.top    = GetDerivedPosY() + STARTY;
				rcIconRect.right  = rcIconRect.left + WIDTH;
				rcIconRect.bottom = rcIconRect.top + HEIGHT;
				if( ::PtInRect( &rcIconRect, pMouse->GetMousePos() ) )
				{
					m_bIconOver = true;	
				}
			}
		}    

		if( m_bIconOver )
		{
			pModeHelpTooltip->SetModeHelpTooltip( this, g_ModeSelectInfo.GetSelectQuickStartModeTitle( m_BattleRoomInfo.m_iModeSelectIndex ), 
												  g_ModeSelectInfo.GetQuickStartIndexToHelpIndex( m_BattleRoomInfo.m_iModeSelectIndex ) );
		}
		else
		{
			pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
		}
	}	
}
//////////////////////////////////////////////////////////////////////////
BattleQuickStartPlayerOption::BattleQuickStartPlayerOption()
{
	m_pOrangeStrip  = NULL;
	m_pTickLine		= NULL;

	m_pMinimumBtn = NULL;
	m_pMaximumBtn = NULL;

	m_iMinPlayer = m_iMaxPlayer = 0;
}

BattleQuickStartPlayerOption::~BattleQuickStartPlayerOption()
{
	SAFEDELETE( m_pOrangeStrip );
	SAFEDELETE( m_pTickLine );
}

void BattleQuickStartPlayerOption::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "OrangeStrip" )
	{
		SAFEDELETE( m_pOrangeStrip );
		m_pOrangeStrip = pImage;
		if( m_pOrangeStrip )
		{
			m_pOrangeStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOrangeStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		}
	}
	else if( szType == "TickLine" )
	{
		SAFEDELETE( m_pTickLine );
		m_pTickLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void BattleQuickStartPlayerOption::iwm_show()
{
	SetBtnTitle( m_iMinPlayer, m_iMaxPlayer, false );
}

void BattleQuickStartPlayerOption::SetPlayerOption( int iLimitMaxPlayer, int iMinPlayer, int iMaxPlayer )
{
	m_pMinimumBtn = FindChildWnd( ID_MINIMUM );
	m_pMaximumBtn = FindChildWnd( ID_MAXIMUM );

	SetSize( 108 + (iLimitMaxPlayer * TICK_GAP), GetHeight() );
	SetBtnTitle( min( iMinPlayer, iLimitMaxPlayer ), min( iLimitMaxPlayer, iMaxPlayer ), true );

	if( m_pParent )
		m_pParent->iwm_command( this, IOBN_BTNUP, 0 );
}

void BattleQuickStartPlayerOption::SetBtnTitle( int iMinPlayer, int iMaxPlayer, bool bRePos )
{
	m_iMinPlayer  = iMinPlayer;
	m_iMaxPlayer  = iMaxPlayer;

	if( m_pMinimumBtn )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), m_iMinPlayer );
		m_pMinimumBtn->SetTitleText( szTitle );		
		if( bRePos )
			m_pMinimumBtn->SetWndPos( ( 59 + ( m_iMinPlayer - 1 ) * TICK_GAP ) - m_pMinimumBtn->GetWidth(), 0 );
	}

	if( m_pMaximumBtn )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2), m_iMaxPlayer );
		m_pMaximumBtn->SetTitleText( szTitle );
		if( bRePos )
			m_pMaximumBtn->SetWndPos( ( 59 + ( m_iMaxPlayer - 1 ) * TICK_GAP ) + 1, 0 );
	}
}

void BattleQuickStartPlayerOption::MoveMinBtn( bool bBtnUp )
{
	if( !m_pMinimumBtn || !m_pMaximumBtn )
		return;

	if( m_pMinimumBtn->GetXPos() < 0 )
		m_pMinimumBtn->SetWndPos( 0, 0 );
	else if( m_pMinimumBtn->GetXPos() + m_pMinimumBtn->GetWidth() >= GetWidth() - m_pMaximumBtn->GetWidth() )
		m_pMinimumBtn->SetWndPos( GetWidth() - ( m_pMinimumBtn->GetWidth() + m_pMaximumBtn->GetWidth() + 1 ), 0 );

	if( m_pMinimumBtn->GetYPos() != 0 )
		m_pMinimumBtn->SetWndPos( m_pMinimumBtn->GetXPos(), 0 );

	// 최대 버튼을 밀어낸다.
	if( m_pMinimumBtn->GetXPos() + m_pMinimumBtn->GetWidth() >= m_pMaximumBtn->GetXPos() )
		m_pMaximumBtn->SetWndPos( m_pMinimumBtn->GetXPos() + m_pMinimumBtn->GetWidth() + 1, m_pMaximumBtn->GetYPos() );

	int iMinDotPos = m_pMinimumBtn->GetXPos() + m_pMinimumBtn->GetWidth();
	int iMaxDotPos = m_pMaximumBtn->GetXPos();	
	int iMinPlayer = m_iMinPlayer;
	int iMaxPlayer = m_iMaxPlayer;
	for(int i = 0;i < MAX_TICK;i++)
	{
		int iRealXPos = 59 + i * TICK_GAP;		
		// 최소 
		if( COMPARE( iMinDotPos, iRealXPos - ( TICK_GAP / 2 ), iRealXPos + ( TICK_GAP / 2 ) + 1 ) )
		{
			iMinPlayer = i + 1;
			if( bBtnUp )
				m_pMinimumBtn->SetWndPos( iRealXPos - m_pMinimumBtn->GetWidth(), 0 );
		}
	
		// 최대
		if( COMPARE( iMaxDotPos, iRealXPos - ( TICK_GAP / 2 ), iRealXPos + ( TICK_GAP / 2 ) + 1 ) )
		{
			iMaxPlayer = i + 1;
			if( bBtnUp )
				m_pMaximumBtn->SetWndPos( iRealXPos + 1, 0 );
		}
	}

	if( m_iMinPlayer != iMinPlayer || m_iMaxPlayer != iMaxPlayer )
	{
		SetBtnTitle( iMinPlayer, iMaxPlayer, false );

		if( m_pParent )
			m_pParent->iwm_command( this, IOBN_BTNUP, 0 );
	}	
}

void BattleQuickStartPlayerOption::MoveMaxBtn( bool bBtnUp )
{
	if( !m_pMinimumBtn || !m_pMaximumBtn )
		return;

	if( m_pMaximumBtn->GetXPos() <= m_pMinimumBtn->GetWidth() )
		m_pMaximumBtn->SetWndPos( m_pMinimumBtn->GetWidth() + 1, 0 );
	else if( m_pMaximumBtn->GetXPos() >= GetWidth() - m_pMaximumBtn->GetWidth() )
		m_pMaximumBtn->SetWndPos( GetWidth() - m_pMaximumBtn->GetWidth(), 0 );

	if( m_pMaximumBtn->GetYPos() != 0 )
		m_pMaximumBtn->SetWndPos( m_pMaximumBtn->GetXPos(), 0 );

	// 최소 버튼을 밀어낸다.
	if( m_pMaximumBtn->GetXPos() < m_pMinimumBtn->GetXPos() + m_pMinimumBtn->GetWidth() )
		m_pMinimumBtn->SetWndPos( m_pMaximumBtn->GetXPos() - ( m_pMinimumBtn->GetWidth() + 1 ), m_pMinimumBtn->GetYPos() );

	int iMinDotPos = m_pMinimumBtn->GetXPos() + m_pMinimumBtn->GetWidth();
	int iMaxDotPos = m_pMaximumBtn->GetXPos();	
	int iMinPlayer = m_iMinPlayer;
	int iMaxPlayer = m_iMaxPlayer;
	for(int i = 0;i < MAX_TICK;i++)
	{
		int iRealXPos = 59 + i * TICK_GAP;		
		// 최소 
		if( COMPARE( iMinDotPos, iRealXPos - ( TICK_GAP / 2 ), iRealXPos + ( TICK_GAP / 2 ) + 1 ) )
		{
			iMinPlayer = i + 1;
			if( bBtnUp )
				m_pMinimumBtn->SetWndPos( iRealXPos - m_pMinimumBtn->GetWidth(), 0 );
		}

		// 최대
		if( COMPARE( iMaxDotPos, iRealXPos - ( TICK_GAP / 2 ), iRealXPos + ( TICK_GAP / 2 ) + 1 ) )
		{
			iMaxPlayer = i + 1;
			if( bBtnUp )
				m_pMaximumBtn->SetWndPos( iRealXPos + 1, 0 );
		}
	}

	if( m_iMinPlayer != iMinPlayer || m_iMaxPlayer != iMaxPlayer )
	{
		SetBtnTitle( iMinPlayer, iMaxPlayer, false );		

		if( m_pParent )
			m_pParent->iwm_command( this, IOBN_BTNUP, 0 );
	}
}

void BattleQuickStartPlayerOption::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_MINIMUM:
		if( cmd == IOWN_MOVED )
		{
			MoveMinBtn( false );
		}
		else if( cmd == IOBN_BTNUP )
		{
			MoveMinBtn( true );
		}
		break;
	case ID_MAXIMUM:
		if( cmd == IOWN_MOVED )
		{
			MoveMaxBtn( false );
		}
		else if( cmd == IOBN_BTNUP )
		{
			MoveMaxBtn( true );
		}
		break;
	}
}

void BattleQuickStartPlayerOption::OnRender()
{
	ioWnd::OnRender();

	if( !m_pMinimumBtn || !m_pMaximumBtn || !m_pOrangeStrip || !m_pTickLine )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	int iStripWidth = m_pMaximumBtn->GetXPos() - ( m_pMinimumBtn->GetXPos() + m_pMinimumBtn->GetWidth() );
	m_pOrangeStrip->SetSize( iStripWidth, 5 );
	m_pOrangeStrip->Render( iXPos + m_pMinimumBtn->GetXPos() + m_pMinimumBtn->GetWidth(), iYPos + 5 );

	for(int i = 0;i < MAX_TICK;i++)
	{
		int iRealXPos = iXPos + 59 + i * TICK_GAP;
		if( !COMPARE( iRealXPos - iXPos, m_pMinimumBtn->GetXPos() + m_pMinimumBtn->GetWidth(), m_pMaximumBtn->GetXPos() ) )
			continue;

		m_pTickLine->Render( iRealXPos, iYPos + 6, UI_RENDER_SCREEN );
	}
}

BattleQuickStartOption::BattleQuickStartOption()
{
	m_pGreenState	= NULL;
	m_pModeIconBack	= NULL;
	m_pCheckOn		= NULL;
	m_pCheckOff		= NULL;
	m_pDot          = NULL;
	m_pModeHelpTooltip  = NULL;

	m_pOpenWnd      = NULL;
	m_iOverIndex	= -1;
	m_iPushIndex    = -1;

	m_eKind = E_FAVORITE;
}

BattleQuickStartOption::~BattleQuickStartOption()
{
	SAFEDELETE( m_pGreenState );
	SAFEDELETE( m_pModeIconBack );
	SAFEDELETE( m_pCheckOn );
	SAFEDELETE( m_pCheckOff );
	SAFEDELETE( m_pDot );
}

void BattleQuickStartOption::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "GreenState" )
	{
		SAFEDELETE( m_pGreenState );
		m_pGreenState = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void BattleQuickStartOption::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else if( szType == "CheckOn" )
	{
		SAFEDELETE( m_pCheckOn );
		m_pCheckOn = pImage;
	}
	else if( szType == "CheckOff" )
	{
		SAFEDELETE( m_pCheckOff );
		m_pCheckOff = pImage;
	}
	else if( strcmp(szType.c_str(), "Dot") == 0 )
	{
		SAFEDELETE( m_pDot );
		m_pDot = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}	
}

const BattleQuickStartOption::ModeData &BattleQuickStartOption::GetModeData( int iModeIndex )
{
	int iListSize = m_ModeList.size();
	for(int i = 0;i < iListSize;i++)
	{
		if( m_ModeList[i].m_iIndex != iModeIndex ) continue;

		return m_ModeList[i];
	}

	static ModeData kError;
	return kError;
}

void BattleQuickStartOption::SetCurrentSameUser( bool bSameUser )
{
	int iModeIndex = m_QuickStartInfo.m_iQuickStartType;
	if( COMPARE( iModeIndex, 0, BMT_MAX ) )
		m_QuickStartInfo.m_bQuickStartSameUser[iModeIndex] = bSameUser;
}

void BattleQuickStartOption::SetCurrentMaxPlayer( int iMaxPlayer )
{
	int iModeIndex = m_QuickStartInfo.m_iQuickStartType;
	if( COMPARE( iModeIndex, 0, BMT_MAX ) )
		m_QuickStartInfo.m_iQuickStartMaxUser[iModeIndex] = iMaxPlayer;	
}

void BattleQuickStartOption::SetCurrentMinPlayer( int iMinPlayer )
{
	int iModeIndex = m_QuickStartInfo.m_iQuickStartType;
	if( COMPARE( iModeIndex, 0, BMT_MAX ) )
		m_QuickStartInfo.m_iQuickStartMinUser[iModeIndex] = iMinPlayer;	
}

bool BattleQuickStartOption::GetUseSameUser( int iModeIndex )
{
	int iListSize = m_ModeList.size();
	for(int i = 0;i < iListSize;i++)
	{
		if( m_ModeList[i].m_iIndex != iModeIndex ) continue;

		return m_ModeList[i].m_bUseSameUser;
	}
	return false;
}

int BattleQuickStartOption::GetLimitMaxUser( int iModeIndex )
{
	int iListSize = m_ModeList.size();
	for(int i = 0;i < iListSize;i++)
	{
		if( m_ModeList[i].m_iIndex != iModeIndex ) continue;

		return m_ModeList[i].m_iLimitMaxUser;
	}
	return 0;
}

bool  BattleQuickStartOption::GetSameUser( int iModeIndex )
{
	if( COMPARE( iModeIndex, 0, BMT_MAX ) )
		return m_QuickStartInfo.m_bQuickStartSameUser[iModeIndex];
	return false;
}

int  BattleQuickStartOption::GetMaxPlayer( int iModeIndex )
{
	if( COMPARE( iModeIndex, 0, BMT_MAX ) )
		return m_QuickStartInfo.m_iQuickStartMaxUser[iModeIndex];	
	return 0;
}

int  BattleQuickStartOption::GetMinPlayer( int iModeIndex )
{
	if( COMPARE( iModeIndex, 0, BMT_MAX ) )
		return m_QuickStartInfo.m_iQuickStartMinUser[iModeIndex];	
	return 0;
}

void BattleQuickStartOption::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	switch( m_eKind )
	{
	case E_FAVORITE:
		FavoriteCommand( pWnd, cmd, param );
		break;

	case E_CREATEBATTLE:
		CreateBattleCommand( pWnd, cmd, param );
		break;

	case E_LADDERBATTLE:
		break;
	}
}

void BattleQuickStartOption::FavoriteCommand( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_JOIN:
	case ID_COMPLETE:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pOpenWnd )
			{
				m_QuickStartInfo.m_iQuickStartType = GetCheckedMode();
				g_BattleRoomMgr.SaveQuickStartInfo( m_QuickStartInfo );
				m_pOpenWnd->iwm_command( this, IOBN_BTNUP, m_QuickStartInfo.m_iQuickStartType );				
				g_QuestMgr.QuestCompleteTerm( QC_QUICK_START_OPTION_SELECT, m_QuickStartInfo.m_iQuickStartType );
			}
			HideWnd();
			if(dwID == ID_JOIN)
			{
				if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
					g_MyInfo.IsExpertEntryTemporary() )
				{
					g_App.OpenFormalityEntryPage();
				}
				else if( g_BattleRoomMgr.GetQuickStartModeType() == BMT_USER_CUSTOM )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "유저모드는 빠른시작이 불가능합니다." );
				}
				else if( !g_MyInfo.IsTutorialUser() )
				{
					if( g_LadderTeamMgr.IsLadderTeam() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "진영전 중에는 빠른전투를 할 수 없습니다.");
					}
					else
					{
						TimeGateSearch *pSearchWnd = dynamic_cast<TimeGateSearch*>(g_GUIMgr.FindWnd( TIMEGATE_SEARCH ));
						if( pSearchWnd )
						{
							if( pSearchWnd->IsShow() && pSearchWnd->GetSearchType() == TimeGateSearch::ST_PARTY_JOIN )
								pSearchWnd->HideWnd();
							else
							{
								pSearchWnd->SetSearching( "조건에 맞는 전투방을 검색중 . . .", TimeGateSearch::ST_PARTY_JOIN );		
							}
						}
					}		
				}
			}
		}
		break;
	case ID_SAME_PLAYER_CHECK:
		if( cmd == IOBN_BTNUP )
		{
			SetCurrentSameUser( !GetSameUser( m_QuickStartInfo.m_iQuickStartType ) );
			ioCheckButton *pSamePlayer = dynamic_cast<ioCheckButton*>( pWnd );
			if( pSamePlayer )
			{
				pSamePlayer->SetCheck( GetSameUser( m_QuickStartInfo.m_iQuickStartType ) );
			}
		}
		break;
	case ID_PLAYER_OPTION:
		if( cmd == IOBN_BTNUP )
		{
			BattleQuickStartPlayerOption *pPlayerOption = dynamic_cast<BattleQuickStartPlayerOption*>( pWnd );
			if( pPlayerOption )
			{
				SetCurrentMaxPlayer( pPlayerOption->GetMaxPlayer() );
				SetCurrentMinPlayer( pPlayerOption->GetMinPlayer() );
			}
		}
		break;
	case ID_MODE_HELP:
		if(cmd == IOBN_BTNUP)
		{
			int iManualIndex = g_ModeSelectInfo.GetQuickStartIndexToManualIndex( m_QuickStartInfo.m_iQuickStartType );

			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
			{
				if( iManualIndex == -1 )
					pManualWnd->ShowWndByCurManual(1);
				else
					pManualWnd->ShowWndByCurManual(iManualIndex);

				HideWnd();
			}
		}
		break;
	default:
		if( COMPARE(dwID, ID_FAVORITE_MODE_START, ID_FAVORITE_MODE_END))
		{
			if(cmd == IOBN_BTNUP)
			{
				for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
				{
					NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
					if(pTmpBtn)
					{
						pTmpBtn->SetSubModeStyle(0);
						pTmpBtn->HideAllChildWnd();
						pTmpBtn->ResetCheck(dwID);
					}
				}

				NormalModeBtn *pBtn = dynamic_cast<NormalModeBtn*>( pWnd );
				if(pBtn->GetBtnType() == NormalModeBtn::BTN_PVE_TYPE || pBtn->GetBtnType() == NormalModeBtn::BTN_PVP_TYPE)
				{
					NormalModeBtn* pPVEBtn = (NormalModeBtn*)pBtn->FindChildWnd(param);
					if(pPVEBtn)
					{
						pPVEBtn->SetCheck(true);
						m_QuickStartInfo.m_iQuickStartType = GetCheckedMode();
						SetExtendOption();
					}
				}
				else
				{
					pBtn->SetCheck(true);
					m_QuickStartInfo.m_iQuickStartType = GetCheckedMode();
					SetExtendOption();
				}
			}
			else if(cmd == IOWN_OVERED)
			{
				NormalModeBtn *pBtn = dynamic_cast<NormalModeBtn*>( pWnd );
				if(pBtn)
					ProcessModeHelpTooltip(pBtn, true);
			}
			else if(cmd == IOWN_LEAVED)
			{
				ProcessModeHelpTooltip(NULL, false);
			}
		}
		break;
	}
}

void BattleQuickStartOption::CreateBattleCommand( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;

	case ID_MODE_HELP:
		if(cmd == IOBN_BTNUP)
		{
			int iManualIndex = g_ModeSelectInfo.GetQuickStartIndexToManualIndex( m_QuickStartInfo.m_iQuickStartType );

			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
			{
				if( iManualIndex == -1 )
					pManualWnd->ShowWndByCurManual(1);
				else
					pManualWnd->ShowWndByCurManual(iManualIndex);

				HideWnd();

				PartyCreateWnd *pPartyCreate = dynamic_cast<PartyCreateWnd*>( g_GUIMgr.FindWnd( PARTY_CREATE_WND ) );
				pPartyCreate->HideWnd();
			}
		}
		break;

	default:
		if( COMPARE(dwID, ID_FAVORITE_MODE_START, ID_FAVORITE_MODE_END))
		{
			if(cmd == IOBN_BTNUP)
			{
				for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
				{
					NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
					if(pTmpBtn)
					{
						pTmpBtn->SetSubModeStyle(0);
						pTmpBtn->HideAllChildWnd();
					}
				}

				NormalModeBtn *pBtn = dynamic_cast<NormalModeBtn*>( pWnd );
				if(pBtn->GetBtnType() == NormalModeBtn::BTN_PVE_TYPE || pBtn->GetBtnType() == NormalModeBtn::BTN_PVP_TYPE )
				{
					NormalModeBtn* pPVEBtn = (NormalModeBtn*)pBtn->FindChildWnd(param);
					if(pPVEBtn)
					{
						pPVEBtn->SetCheck(true);
						PartyCreateWnd *pPartyCreate = dynamic_cast<PartyCreateWnd*>( g_GUIMgr.FindWnd( PARTY_CREATE_WND ) );
						if(pPartyCreate)
						{
							pPartyCreate->iwm_command( pPartyCreate->FindChildWnd(PartyCreateWnd::ID_CHANGE_MODE), 
								PartyCreateWnd::ID_MODE_LIST, GetCheckedMode());
						}
						HideWnd();
					}
				}
				else
				{
					pBtn->SetCheck(true);
					PartyCreateWnd *pPartyCreate = dynamic_cast<PartyCreateWnd*>( g_GUIMgr.FindWnd( PARTY_CREATE_WND ) );
					if(pPartyCreate)
					{	
						pPartyCreate->iwm_command( pPartyCreate->FindChildWnd(PartyCreateWnd::ID_CHANGE_MODE), 
							PartyCreateWnd::ID_MODE_LIST, GetCheckedMode());
					}
					HideWnd();
				}
			}
			else if(cmd == IOWN_OVERED)
			{
				NormalModeBtn *pBtn = dynamic_cast<NormalModeBtn*>( pWnd );
				if(pBtn)
					ProcessModeHelpTooltip(pBtn, true);
			}
			else if(cmd == IOWN_LEAVED)
			{
				ProcessModeHelpTooltip(NULL, false);
			}
		}
		break;
	}
}

bool BattleQuickStartOption::iwm_spacebar()
{
	ioButton *pYesBtn = (ioButton*)FindChildWnd(ID_JOIN);
	if( !pYesBtn ) return false;
	if( !pYesBtn->IsShow() ) return false;

	iwm_command( pYesBtn, IOBN_BTNUP, 0 );
	return true;
}

void BattleQuickStartOption::iwm_show()
{
	m_iOverIndex = -1;
	m_iPushIndex = -1;

	m_QuickStartInfo = g_BattleRoomMgr.GetQuickStartInfo();	

	if( m_eKind == E_FAVORITE)
	{
		SetExtendOption();
		SetCheckModeBtn();
	}
}

void BattleQuickStartOption::iwm_hide()
{
	for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
	{
		NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
		if(pTmpBtn)
			pTmpBtn->ResetCheck();
	}
}

void BattleQuickStartOption::SetExtendOption()
{
	ioCheckButton *pSamePlayer = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_SAME_PLAYER_CHECK ) );
	if( pSamePlayer )
	{
		if( GetUseSameUser( m_QuickStartInfo.m_iQuickStartType ) )
		{
			pSamePlayer->SetActive();
		}
		else
		{
			pSamePlayer->SetInActive();
		}
		pSamePlayer->SetCheck( GetSameUser( m_QuickStartInfo.m_iQuickStartType ) );
	}

	BattleQuickStartPlayerOption *pPlayerOption = dynamic_cast<BattleQuickStartPlayerOption*>( FindChildWnd( ID_PLAYER_OPTION ) );
	if( pPlayerOption )
	{
		pPlayerOption->SetPlayerOption( GetLimitMaxUser( m_QuickStartInfo.m_iQuickStartType ),
										GetMinPlayer( m_QuickStartInfo.m_iQuickStartType ), 
										GetMaxPlayer( m_QuickStartInfo.m_iQuickStartType ) );
	}
}

void BattleQuickStartOption::SetExtendOptionDefault()
{
	int iType = m_QuickStartInfo.m_iQuickStartType;
	if( COMPARE( iType, 0, BMT_MAX ) )
	{		
		m_QuickStartInfo.m_iQuickStartMaxUser[iType] = g_ModeSelectInfo.GetSelectQuickStartModeDefMaxPlayer( iType );
		m_QuickStartInfo.m_iQuickStartMinUser[iType] = g_ModeSelectInfo.GetSelectQuickStartModeDefMinPlayer( iType );
		m_QuickStartInfo.m_bQuickStartSameUser[iType] = g_ModeSelectInfo.GetSelectQuickStartModeDefSameUser( iType );
	}
	SetExtendOption();
}

void BattleQuickStartOption::SetTitleAndSizeWnd( KINDWND eKind )
{
	switch(eKind)
	{
	case E_FAVORITE:
		{
			SetTitleText( "빠른참가＆방정렬 설정" );
			SetSize( 670, 521 );
			ShowChildWnd(ID_JOIN);
			ShowChildWnd(ID_COMPLETE);
			ShowChildWnd(ID_SAME_PLAYER_CHECK);
			ShowChildWnd(ID_PLAYER_OPTION);

			for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
			{
				NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
				if(pTmpBtn)
				{
					if(pTmpBtn->GetModeStyle() == 0 || pTmpBtn->GetModeStyle() == 18)
						pTmpBtn->SetActive();
				}
			}
		}
		break;

	case E_CREATEBATTLE:
		{
			SetTitleText( "모드 선택" );
			SetSize( 670, 458 );
			HideChildWnd(ID_JOIN);
			HideChildWnd(ID_COMPLETE);
			HideChildWnd(ID_SAME_PLAYER_CHECK);
			HideChildWnd(ID_PLAYER_OPTION);
			for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
			{
				NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);

				if(pTmpBtn)
				{
					if(pTmpBtn->GetModeStyle() == 0 || pTmpBtn->GetModeStyle() == 18)
						pTmpBtn->SetInActive();
				}
			}
		}
		break;

	case E_LADDERBATTLE:
		SetTitleText( "모드 선택" );
		SetSize( 670, 458 );
		HideChildWnd(ID_JOIN);
		HideChildWnd(ID_COMPLETE);
		HideChildWnd(ID_SAME_PLAYER_CHECK);
		HideChildWnd(ID_PLAYER_OPTION);
		break;

	default:
		break;
	}
}

void BattleQuickStartOption::SetQuickStart( ioWnd *pOpenWnd, KINDWND eKind )
{
	m_eKind = eKind;
	SetTitleAndSizeWnd( m_eKind );
	m_pOpenWnd = pOpenWnd;
	m_ModeList.clear();

	for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
	{
		NormalModeBtn* pBtn = (NormalModeBtn*)FindChildWnd(i);
		if(pBtn)
			pBtn->HideWnd();
	}

	m_nAddMode = 0;
	g_ModeSelectInfo.GetQuickModeList( this );

	enum 
	{
		X_POS = 17, Y_POS = 54, X_GAP = 107, Y_GAP = 98, N_ROW = 6,
	};

	for(int i = 0; i < NUM_MODE - m_nAddMode; i++)
	{
		int nIdx = m_nAddMode + i, nX = 0, nY = 0;

		if( nIdx > 0)
		{
			nX = nIdx % N_ROW; 
			nY = nIdx / N_ROW;
		}

		NormalModeBtn* pBtn = (NormalModeBtn*)FindChildWnd(ID_FAVORITE_MODE_END - i - 1);

		if(pBtn)
		{
			pBtn->SetWndPos(X_POS + (X_GAP * nX), Y_POS + (Y_GAP * nY) );
			pBtn->ShowWnd();
		}
	}
}

int BattleQuickStartOption::GetCheckedMode()
{
	int nModeStyle = 0;

	bool bSet = false;

	for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
	{
		NormalModeBtn* pBtn = (NormalModeBtn*)FindChildWnd(i);

		if(pBtn)
		{
			if(pBtn->GetCheck())
			{
				if( pBtn->GetBtnType() == NormalModeBtn::BTN_PVE_TYPE || pBtn->GetBtnType() == NormalModeBtn::BTN_PVP_TYPE)
					return (pBtn->GetModeStyle() + pBtn->GetSubModeStyle() - 1);
				else
					return pBtn->GetModeStyle();
			}
		}
	}

	return nModeStyle;
}

void BattleQuickStartOption::SetCheckModeBtn()
{
	int nModeStyle = -1;

	bool bSet = false;

	for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
	{
		NormalModeBtn* pBtn = (NormalModeBtn*)FindChildWnd(i);

		if(pBtn)
		{
			if(pBtn->GetBtnType() == NormalModeBtn::BTN_PVE_TYPE)
			{
				for(int j = PVEModeBtn::SUB_BTN_START; j < PVEModeBtn::SUB_BTN_END; j++)
				{
					bSet = false;
					NormalModeBtn* pTmpBtn = (NormalModeBtn*)pBtn->FindChildWnd(j);
					if(pTmpBtn)
					{
						nModeStyle = pBtn->GetModeStyle() + pTmpBtn->GetSubModeStyle() - 1;

						if( nModeStyle == m_QuickStartInfo.m_iQuickStartType)
						{
							pBtn->iwm_command(pTmpBtn, IOBN_BTNUP, 0);
							pBtn->ShowAllChildWnd();
							bSet = true;
							break;
						}
					}
				}

				if(bSet) break;
			}
			else if(pBtn->GetBtnType() == NormalModeBtn::BTN_PVP_TYPE)
			{
				for(int j = PVPModeBtn::SUB_BTN_START; j < PVPModeBtn::SUB_BTN_END; j++)
				{
					bSet = false;
					NormalModeBtn* pTmpBtn = (NormalModeBtn*)pBtn->FindChildWnd(j);
					if(pTmpBtn)
					{
						nModeStyle = pBtn->GetModeStyle() + pTmpBtn->GetSubModeStyle() - 1;

						if( nModeStyle == m_QuickStartInfo.m_iQuickStartType)
						{
							pBtn->iwm_command(pTmpBtn, IOBN_BTNUP, 0);
							pBtn->ShowAllChildWnd();
							bSet = true;
							break;
						}
					}
				}

				if(bSet) break;
			}
			else
			{
				nModeStyle = pBtn->GetModeStyle();

				if( nModeStyle == m_QuickStartInfo.m_iQuickStartType)
				{
					iwm_command(pBtn, IOBN_BTNUP, 0);
					break;
				}
			}
		}
	}
}

void BattleQuickStartOption::AddList( int iIndex, const ioHashString &szTitle, int iHelp, int iLimitMaxUser, 
			int iDefMinUser, int iDefMaxUser, bool bUseSameUser, ioUIRenderImage *pMarkRes )
{
	ModeData kModeData;
	kModeData.m_iIndex = iIndex;
	kModeData.m_iHelp  = iHelp;
	kModeData.m_szTitle= szTitle;
	kModeData.m_pMarkRes = pMarkRes;
	kModeData.m_iLimitMaxUser = iLimitMaxUser;
	kModeData.m_iDefMinUser   = iDefMinUser;
	kModeData.m_iDefMaxUser   = iDefMaxUser;
	kModeData.m_bUseSameUser  = bUseSameUser;
	m_ModeList.push_back( kModeData );

	SetModeBtn();
}

void BattleQuickStartOption::SetModeBtn()
{
	if(m_ModeList.empty())
		return;

	enum 
	{
		X_POS = 17,
		Y_POS = 54,
		X_GAP = 107,
		Y_GAP = 98,
		N_ROW = 6,
	};

	int nIdx = m_nAddMode;

	int nX = 0;
	int nY = 0;

	if( nIdx > 0)
	{
		nX = nIdx % N_ROW;
		nY = nIdx / N_ROW;
	}

	NormalModeBtn* pBtn = NULL;

	for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
	{
		NormalModeBtn* pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
		if(pTmpBtn)
		{
			if( pTmpBtn->GetModeStyle() == m_ModeList[m_ModeList.size() - 1].m_iIndex )
			{
				pBtn = pTmpBtn;
				pBtn->SetNewHotMark(m_ModeList[m_ModeList.size() - 1].m_pMarkRes);
				break;
			}
		}
	}

	if(pBtn)
	{
		pBtn->SetWndPos(X_POS + (X_GAP * nX), Y_POS + (Y_GAP * nY) );
		pBtn->ShowWnd();
		if(pBtn->GetBtnType() == NormalModeBtn::BTN_PVE_TYPE)
			pBtn->HideAllChildWnd();

		m_nAddMode++;
	}
}

void BattleQuickStartOption::ProcessRadioCheck()
{
	int  iPrevOverIndex = m_iOverIndex;
	
	m_iOverIndex = -1;
	int iListSize = m_ModeList.size();
	int iStartX   = GetDerivedPosX() + DEFAULT_LIST_X;
	int iStartY   = (GetDerivedPosY() + DEFAULT_LIST_Y) - ((iListSize - 1) * LIST_YPOS_GAP );

	ioLSMouse *pMouse = g_App.GetMouse();
	for(int i = 0;i < iListSize;i++)
	{
		RECT rcNameRect;
		rcNameRect.left   = iStartX;
		rcNameRect.top    = iStartY + (i*LINE_YPOS_GAP);
		rcNameRect.right  = rcNameRect.left + CHECK_BOX_W;
		rcNameRect.bottom = rcNameRect.top + CHECK_BOX_H;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			if( iPrevOverIndex != m_ModeList[i].m_iIndex &&
				m_QuickStartInfo.m_iQuickStartType != iPrevOverIndex )
			{
				g_SoundMgr.PlaySound( "interface/public_mouseover_00.wav" );
			}

			m_iOverIndex = m_ModeList[i].m_iIndex;
			if( pMouse->IsLBtnDown() && m_iPushIndex == -1 )
			{
				if( m_QuickStartInfo.m_iQuickStartType != m_ModeList[i].m_iIndex )
					g_SoundMgr.PlaySound( "interface/public_mouseclick_00.wav" );
				m_QuickStartInfo.m_iQuickStartType = m_ModeList[i].m_iIndex;
				m_iPushIndex = m_QuickStartInfo.m_iQuickStartType;
				SetExtendOptionDefault();
			}
			break;
		}
	}

	if( pMouse->IsLBtnUp() )
	{
		m_iPushIndex = -1;
	}
	else if( pMouse->IsLBtnDown() )
	{
		m_iPushIndex = m_QuickStartInfo.m_iQuickStartType;
	}
}

void BattleQuickStartOption::OnRender()
{
	ioWnd::OnRender();

	if(m_eKind == E_FAVORITE)
	{
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( 110, 110, 110 );
		g_FontMgr.PrintText( iXPos + 17, iYPos + 472, FONT_SIZE_12, STR(1) );
		g_FontMgr.PrintText( iXPos + 17, iYPos + 490, FONT_SIZE_12, STR(2) );
	}
}

void BattleQuickStartOption::OnRenderModeList( int iXPos, int iYPos )
{
	int iListSize = m_ModeList.size();
	int iStartX   = iXPos + DEFAULT_LIST_X;
	int iStartY   = (iYPos + DEFAULT_LIST_Y) - ((iListSize - 1) * LIST_YPOS_GAP );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	for(int i = 0;i < iListSize;i++)
	{
		ModeData &rkData = m_ModeList[i];
		if( m_pCheckOff )
			m_pCheckOff->Render( iStartX, iStartY + (i*LINE_YPOS_GAP) );
		if( rkData.m_iIndex == m_QuickStartInfo.m_iQuickStartType )
		{
			if( m_pCheckOn )
				m_pCheckOn->Render( iStartX, iStartY + (i*LINE_YPOS_GAP) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	
		}
		else
		{			
            g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
		}
		g_FontMgr.PrintText( iStartX + 20, iStartY + (i*LINE_YPOS_GAP), FONT_SIZE_13, rkData.m_szTitle.c_str() );	

		if( rkData.m_pMarkRes )
		{
			int iTitleSize = g_FontMgr.GetTextWidth( rkData.m_szTitle.c_str(), TS_NORMAL, FONT_SIZE_13 );
			rkData.m_pMarkRes->Render( iStartX + 22 + iTitleSize, iStartY + (i*LINE_YPOS_GAP) + 2 );
		}
	}
}

void BattleQuickStartOption::OnRenderModeHelp( int iXPos, int iYPos )
{
	const ModeData &rkData = GetModeData( m_QuickStartInfo.m_iQuickStartType );
	if( m_pModeIconBack )
	{
		m_pModeIconBack->SetScale( FLOAT1 );
		m_pModeIconBack->Render( iXPos + 99, iYPos + 103, UI_RENDER_MULTIPLY );

		// 모드 아이콘
		ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetListModeUIResource( rkData.m_szTitle );
		if( pModeIcon )
		{
			pModeIcon->SetScale( FLOAT1 );
			pModeIcon->Render( iXPos + 99, iYPos + 103 );
		}
	}

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );

	// 모드 상태
	ioHashString szModeState = g_ModeSelectInfo.GetQuickStartHelpState( rkData.m_iHelp );
	if( !szModeState.IsEmpty() )
	{
		if( m_pGreenState )
			m_pGreenState->Render( iXPos, iYPos );

		g_FontMgr.SetTextColor( 255, 255, 255 );	
		g_FontMgr.PrintText( iXPos + 44, iYPos + 63, FONT_SIZE_11, szModeState.c_str() );	
	}

	// 모드명
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	g_FontMgr.PrintTextWidthCut( iXPos + 99, iYPos + 149, FONT_SIZE_13, 150.0f, rkData.m_szTitle.c_str() );	

	// 헬프 7줄
	enum{ MAX_HELP = 8, };
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
	for(int i = 0;i < MAX_HELP;i++)
	{
		ioHashString szHelp = g_ModeSelectInfo.GetQuickStartHelpMent( rkData.m_iHelp, i );
		if( szHelp.IsEmpty() ) continue;

		g_FontMgr.PrintTextWidthCut( iXPos + 99, iYPos + 184 + (i * 16), FONT_SIZE_12, 150.0f, szHelp.c_str() );	
	}
}

bool BattleQuickStartOption::QuestGuide( int iModeIndex, float &rfXPos, float &rfYPos )
{
	// 모드를 선택했다면 버튼에 표시
	if(  m_QuickStartInfo.m_iQuickStartType == iModeIndex )
	{
		ioWnd *pSelectBtn = FindChildWnd( ID_COMPLETE );
		if( pSelectBtn )
		{
			rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
			rfYPos = pSelectBtn->GetDerivedPosY();
			return true;
		}
	}
	else
	{
		for(int i = ID_FAVORITE_MODE_START; i < ID_FAVORITE_MODE_END; i++)
		{
			NormalModeBtn* pSelectBtn = (NormalModeBtn*)FindChildWnd(i);

			if(pSelectBtn)
			{
				if( pSelectBtn->GetModeStyle() == iModeIndex )
				{
					rfXPos = pSelectBtn->GetDerivedPosX() + ( pSelectBtn->GetWidth() / 2 );
					rfYPos = pSelectBtn->GetDerivedPosY() + 10;
					return true;
				}
			}
		}
	}
	return false;
}

void BattleQuickStartOption::ProcessModeHelpTooltip(NormalModeBtn* pBtn, bool bOver)
{
	if( !IsShow() ) return;

	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{		
		if(bOver)
		{
			if(pBtn)
			{
				const ModeData &rkData = GetModeData(pBtn->GetModeStyle());

				if( pBtn->GetModeStyle() == NormalModeBtn::BTN_NORMAL_TYPE)
				{
					pModeHelpTooltip->SetModeHelpTooltip( this, rkData.m_szTitle.c_str(), rkData.m_iHelp );
				}
				else
				{
					ioHashString szTitle;
					ioUITitle *pTitle = pBtn->GetTitle();
					if(pTitle)
						pTitle->GetFirstText(szTitle);

					pModeHelpTooltip->SetModeHelpTooltip( this, szTitle.c_str(), rkData.m_iHelp );
				}
			}
		}
		else
			pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}
}
//////////////////////////////////////////////////////////////////////////
LobbyBattleInfoWnd::LobbyBattleInfoWnd()
{
	m_dwTabID = ID_MISSION_BTN;
	m_bIsNoviceGrade = false;
}

LobbyBattleInfoWnd::~LobbyBattleInfoWnd()
{
}

void LobbyBattleInfoWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/lobbyquestnewalramwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/lobbymainmissionwnd.xml", this );
}

void LobbyBattleInfoWnd::iwm_show()
{ 
	ChangeTab( ID_MISSION_BTN );

	HideChildWnd( ID_NOVICE_GRADE_BTN );
	HideChildWnd( ID_NOVICE_GRADE_TOOTIP_BTN );

	if ( g_NoviceGradeBenefitMgr.IsBenefitGrade( g_MyInfo.GetGradeLevel() ) )
	{
		ShowChildWnd( ID_NOVICE_GRADE_BTN );
		ShowChildWnd( ID_NOVICE_GRADE_TOOTIP_BTN );
		m_bIsNoviceGrade = true;
	}
}

void LobbyBattleInfoWnd::iwm_hide()
{
	HideChildWnd( ID_MISSION_LIST_WND );
	HideChildWnd( ID_QUEST_ALARM_WND );
	HideChildWnd( ID_NOVICE_GRADE_BTN );
	HideChildWnd( ID_NOVICE_GRADE_TOOTIP_BTN );
	m_bIsNoviceGrade = false;
}

void LobbyBattleInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_MY_INFO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_App.OpenMannerWnd( g_MyInfo.GetPublicID().c_str(), true );
		}
		break;
	case ID_QUEST_BTN:
	case ID_MISSION_BTN:
		{
			if( cmd == IOBN_BTNDOWN )
			{
				ChangeTab( dwID );
			}
		}
		break;
	case ID_NOVICE_GRADE_TOOTIP_BTN:
		{
			if( cmd == IOWN_OVERED )
			{
				ShowChildWnd( ID_NOVICE_GRADE_TOOTIP );
			}
			else if( cmd == IOWN_LEAVED )
			{
				HideChildWnd( ID_NOVICE_GRADE_TOOTIP );
			}
		}
		break;
	case ID_TIME_GATE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			TimeGateBtn *pBtn = dynamic_cast<TimeGateBtn*>( pWnd );
			if( pBtn->GetTimeGateState() != TimeGateBtn::eActive )
				return;

			TimeGateResultWnd* pResultWnd = dynamic_cast<TimeGateResultWnd*>( g_GUIMgr.FindWnd( TIMEGATE_RESULT_WND ) );
			if( pResultWnd )
				pResultWnd->SendPakcet();
		}
		break;
	}
}

void LobbyBattleInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	if ( g_MissionManager.IsChangeMission() )
		UpdateRadioBtn();

	SetNamedTitleSparkle();
}

void LobbyBattleInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 12, 66, 111 );
	g_FontMgr.SetTextColor( 255, 255, 255 );

	if ( m_bIsNoviceGrade )
		g_FontMgr.PrintTextWidthCut( iXPos + 59, iYPos + 11, FONT_SIZE_17, 105.0f, g_MyInfo.GetPublicID().c_str() );
	else
	{
		g_LevelMgr.RenderGrade( g_MyInfo.GetPublicID(), iXPos + 14, iYPos + 14, g_MyInfo.GetGradeLevel(), TEAM_PRIVATE_2 );
		g_FontMgr.PrintTextWidthCut( iXPos + 32, iYPos + 11, FONT_SIZE_17, 115.0f, g_MyInfo.GetPublicID().c_str() );
	}
}

void LobbyBattleInfoWnd::ChangeTab( DWORD dwTabID )
{
	m_dwTabID = dwTabID;
	CheckRadioButton( ID_MISSION_BTN, ID_QUEST_BTN, m_dwTabID );

	HideChildWnd( ID_MISSION_LIST_WND );
	HideChildWnd( ID_QUEST_ALARM_WND );

	switch( m_dwTabID )
	{
	case ID_MISSION_BTN:
		ShowChildWnd( ID_MISSION_LIST_WND );
		break;
	case ID_QUEST_BTN:
		ShowChildWnd( ID_QUEST_ALARM_WND );
		break;
	}

	UpdateRadioBtn();
}

void LobbyBattleInfoWnd::UpdateRadioBtn()
{
	MissionRadioBtn *MissionBtn = dynamic_cast<MissionRadioBtn*>( FindChildWnd( ID_MISSION_BTN ) );
	if( MissionBtn )
	{
		bool bStartSparkle = false;
		//현재 탭이 퀘스트일 때
		if ( m_dwTabID != ID_MISSION_BTN )
		{
			for ( int i=MDT_DAILY ; i<MDT_MAX ; ++i )
			{
				//보상 받기가 가능한 미션이 있을 경우
				if ( g_MissionManager.GetAttinCountToType( i ) > 0 )
				{
					bStartSparkle = true;
					break;
				}
			}
		}
		//보상 받기가 가능한 미션이 있고
		if ( bStartSparkle )
		{
			//현재 이펙트 상태가 아니라면
			if ( !MissionBtn->IsSparkling() )
				MissionBtn->SetStartSparkle();	//이펙트 스타트
		}
		//보상 받기가 가능한 미션이 없거나 현재 탭이 미션일 경우
		else
		{
			//이펙트 상태이면 End 시켜준다.
			if ( MissionBtn->IsSparkling() )
				MissionBtn->SetEndSparkle();	//이펙트 종료
		}
	}	
}

void LobbyBattleInfoWnd::SetNamedTitleSparkle()
{
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	NamedTitleBtn *pNamedTitleBtn = dynamic_cast<NamedTitleBtn*>( FindChildWnd( ID_MY_INFO_BTN ) );
	if ( !pNamedTitleBtn )
		return;

	if ( pUserNamedTitle->IsNewNamedTitle() )
	{
		if ( !pNamedTitleBtn->IsEffect() )
			pNamedTitleBtn->SetStartEffect();
	}
	else
	{
		if ( pNamedTitleBtn->IsEffect() )
			pNamedTitleBtn->SetEndEffect();
	}
}
//////////////////////////////////////////////////////////////////////////
NormalModeBtn::NormalModeBtn()
{
	m_nType = BTN_NORMAL_TYPE;
	m_nModeStyle = 0;
	m_nSubModeStyle = 0;
	m_bCheck = false;
	m_bTwinkle = false;
	m_pCheckOn = NULL;
	m_pNewHot = NULL;
	m_fWinkTime = 0.f;
}

NormalModeBtn::~NormalModeBtn()
{
	SAFEDELETE( m_pCheckOn );
}

void NormalModeBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CheckOn" )
	{
		SAFEDELETE( m_pCheckOn );
		m_pCheckOn = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void NormalModeBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo(xElement);
	m_nModeStyle = xElement.GetIntAttribute_e( "ModeStyle" );
	m_nSubModeStyle = xElement.GetIntAttribute_e( "SubModeStyle" );
	m_ptCheck.x = xElement.GetIntAttribute_e( "XCheck" );
	m_ptCheck.y = xElement.GetIntAttribute_e( "YCheck" );
}

void NormalModeBtn::OnRender()
{
	ioButton::OnRender();

 	int iXPos = GetDerivedPosX();
 	int iYPos = GetDerivedPosY();

	if(m_pCheckOn && m_bCheck)
	{
		m_pCheckOn->Render(iXPos + m_ptCheck.x, iYPos + m_ptCheck.y);
		if(m_bTwinkle)
			m_pCheckOn->Render(iXPos + m_ptCheck.x, iYPos + m_ptCheck.y, UI_RENDER_ADD);
	}

	if(m_pNewHot)
		m_pNewHot->Render(iXPos + 3, iYPos + 3);
}

void NormalModeBtn::OnDrawOveredAdd( int iXPos, int iYPos )
{
	if( m_pOverAddFrm )
	{
		m_pOverAddFrm->Render( iXPos, iYPos, UI_RENDER_SCREEN );
	}

	if( m_pOverAdd )
	{
		m_pOverAdd->Render( iXPos, iYPos, UI_RENDER_SCREEN );
	}
}

void NormalModeBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	m_pNormal->SetAlpha(MAX_ALPHA_RATE * 0.3f);
	m_pNormal->Render( iXPos, iYPos, UI_RENDER_GRAY );
	m_pNormal->SetAlpha(MAX_ALPHA_RATE);
}

void NormalModeBtn::SetCheck(bool bCheck) 
{
	if(m_pCheckOn)
	{
		m_bCheck = m_bCheck ^ bCheck;
		if(m_bCheck)
		{
			m_fWinkTime = (float)FRAMEGETTIME() + (float)TWINKLE_TIME;
			m_bTwinkle = true;

			if(m_nSubModeStyle != 0)
			{
				PVEModeBtn* pBtn = (PVEModeBtn*)GetParent();
				if(pBtn && pBtn->GetBtnType() == BTN_PVE_TYPE)
					pBtn->SetSubModeStyle(m_nSubModeStyle);
				else
				{
					PVPModeBtn* pBtn = (PVPModeBtn*)GetParent();
					if(pBtn && pBtn->GetBtnType() == BTN_PVP_TYPE)
						pBtn->SetSubModeStyle(m_nSubModeStyle);
				}
			}
		}
		else
		{
			if(m_nSubModeStyle != 0)
			{
				PVEModeBtn* pBtn = (PVEModeBtn*)GetParent();
				if(pBtn && pBtn->GetBtnType() == BTN_PVE_TYPE)
					pBtn->SetSubModeStyle(0);
				else
				{
					PVPModeBtn* pBtn = (PVPModeBtn*)GetParent();
					if(pBtn && pBtn->GetBtnType() == BTN_PVP_TYPE)
						pBtn->SetSubModeStyle(0);
				}
			}
		}
	}
}

void NormalModeBtn::ResetCheck(DWORD dwID)
{
	if(dwID != GetID())
		m_bCheck = false;
}

void NormalModeBtn::iwm_mouseover( const ioMouse& mouse )
{
	if( m_nSubModeStyle != 0 && GetParent() )
	{
		ioMouse NoneMouse;
		GetParent()->iwm_mouseover( NoneMouse );
	}

	ioButton::iwm_mouseover( mouse );
}

void NormalModeBtn::iwm_mouseleave( const ioMouse& mouse )
{
	if( m_nSubModeStyle != 0 && GetParent() )
	{
		ioMouse NoneMouse;
		GetParent()->iwm_mouseleave( NoneMouse );
	}

	ioWnd::iwm_mouseleave( mouse );
}

void NormalModeBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if(m_bCheck)
	{
		if( m_fWinkTime < (float)FRAMEGETTIME() )
		{
			m_fWinkTime += (float)TWINKLE_TIME;
			m_bTwinkle = m_bTwinkle ^ true;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
PVEModeBtn::PVEModeBtn()
{
	m_nType = BTN_PVE_TYPE;
}

PVEModeBtn::~PVEModeBtn()
{
}

void PVEModeBtn::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if(GetCheck())
	{
		OnDrawBack( iXPos, iYPos );
		OnDrawOvered(iXPos, iYPos);
		PrintOverTitle( iXPos, iYPos );
		OnDrawNormalList( iXPos, iYPos );
		return;
	}

	ioButton::OnRender();

	if(m_pNewHot)
	{
		if(!IsOvered() && !IsPushed())
			m_pNewHot->Render(iXPos + 3, iYPos + 3);
	}
}

void PVEModeBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	if(COMPARE(dwID, SUB_BTN_START, SUB_BTN_END))
	{
		if( cmd == IOBN_BTNUP)
		{
			for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
			{
				NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
				if((pTmpBtn) && (dwID != pTmpBtn->GetID()))
					pTmpBtn->ResetCheck(dwID);
			}

			if(GetParent())
				GetParent()->iwm_command(this, IOBN_BTNUP, dwID);
		}
	}
}

void PVEModeBtn::ShowAllChildWnd()
{
	for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
	{
		BattleQuickStartOption* pQuickWnd = (BattleQuickStartOption*)GetParent();
		if(pQuickWnd)
		{
			int nLimit = pQuickWnd->GetLimitMaxUser(GetModeStyle() + i - 1);
			if(nLimit == 0)
				continue;
		}

		ShowChildWnd(i);
	}
}

void PVEModeBtn::HideAllChildWnd()
{
	for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
		HideChildWnd(i);
}

void PVEModeBtn::ResetCheck(DWORD dwID)
{
	if(dwID != GetID())
	{
		for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
		{
			NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
			if(pTmpBtn)
				pTmpBtn->ResetCheck(dwID);
		}

		SetSubModeStyle(0);
	}
}

void  PVEModeBtn::ResetCheck()
{
	for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
	{
		NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
		if(pTmpBtn)
			pTmpBtn->ResetCheck();
	}

	SetSubModeStyle(0);
}

void PVEModeBtn::iwm_mouseover( const ioMouse& mouse )
{
	ShowAllChildWnd();

	ioButton::iwm_mouseover( mouse );
}

void PVEModeBtn::iwm_mouseleave( const ioMouse& mouse )
{
	if( GetSubModeStyle() == 0 )
		HideAllChildWnd();

	ioWnd::iwm_mouseleave( mouse );
}

bool PVEModeBtn::GetCheck() 
{
	for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
	{
		NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
		if(pTmpBtn)
		{
			if(pTmpBtn->GetCheck())
				return true;
		}
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
PVPModeBtn::PVPModeBtn()
{
	m_nType = BTN_PVP_TYPE;
}

PVPModeBtn::~PVPModeBtn()
{
}

void PVPModeBtn::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if(GetCheck())
	{
		OnDrawBack( iXPos, iYPos );
		OnDrawOvered(iXPos, iYPos);
		PrintOverTitle( iXPos, iYPos );
		OnDrawNormalList( iXPos, iYPos );
		return;
	}

	ioButton::OnRender();

	if(m_pNewHot)
	{
		if(!IsOvered() && !IsPushed())
			m_pNewHot->Render(iXPos + 3, iYPos + 3);
	}
}

void PVPModeBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	if(COMPARE(dwID, SUB_BTN_START, SUB_BTN_END))
	{
		if( cmd == IOBN_BTNUP)
		{
			for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
			{
				NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
				if((pTmpBtn) && (dwID != pTmpBtn->GetID()))
					pTmpBtn->ResetCheck(dwID);
			}

			if(GetParent())
				GetParent()->iwm_command(this, IOBN_BTNUP, dwID);
		}
	}
}

void PVPModeBtn::ShowAllChildWnd()
{
	for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
	{
		BattleQuickStartOption* pQuickWnd = (BattleQuickStartOption*)GetParent();
		if(pQuickWnd)
		{
			int nLimit = pQuickWnd->GetLimitMaxUser(GetModeStyle() + i - 1);
			if(nLimit == 0)
				continue;
		}

		ShowChildWnd(i);
	}
}

void PVPModeBtn::HideAllChildWnd()
{
	for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
		HideChildWnd(i);
}

void PVPModeBtn::ResetCheck(DWORD dwID)
{
	if(dwID != GetID())
	{
		for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
		{
			NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
			if(pTmpBtn)
				pTmpBtn->ResetCheck(dwID);
		}

		SetSubModeStyle(0);
	}
}

void  PVPModeBtn::ResetCheck()
{
	for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
	{
		NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
		if(pTmpBtn)
			pTmpBtn->ResetCheck();
	}

	SetSubModeStyle(0);
}

void PVPModeBtn::iwm_mouseover( const ioMouse& mouse )
{
	ShowAllChildWnd();

	ioButton::iwm_mouseover( mouse );
}

void PVPModeBtn::iwm_mouseleave( const ioMouse& mouse )
{
	if( GetSubModeStyle() == 0 )
		HideAllChildWnd();

	ioWnd::iwm_mouseleave( mouse );
}

bool PVPModeBtn::GetCheck() 
{
	for(int i = SUB_BTN_START; i < SUB_BTN_END; i++)
	{
		NormalModeBtn *pTmpBtn = (NormalModeBtn*)FindChildWnd(i);
		if(pTmpBtn)
		{
			if(pTmpBtn->GetCheck())
				return true;
		}
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
TimeGateBtn::TimeGateBtn()
{
	m_pTimeGateBG = NULL;
	m_pTimeGateText = NULL;
	m_pTextBG = NULL;
	m_pSelected = NULL;

	for( int i = 0; i < 7; i++ )
		m_pTimeGateEffect[i] = NULL;

	m_iTimeGateState = eWait;
	m_iEffectIndex = 0;
	m_dwEffectTime = 0;
}

TimeGateBtn::~TimeGateBtn()
{
	SAFEDELETE( m_pTimeGateBG );
	SAFEDELETE( m_pTimeGateText );
	SAFEDELETE( m_pTextBG );
	SAFEDELETE( m_pSelected );

	for( int i = 0; i < 7; i++ )
		SAFEDELETE( m_pTimeGateEffect[i] );
}

void TimeGateBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "Selected" )
	{
		SAFEDELETE( m_pSelected );
		m_pSelected = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void TimeGateBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TimeGateBG" )
	{
		SAFEDELETE( m_pTimeGateBG );
		m_pTimeGateBG = pImage;
	}
	else if( szType == "TimeGateEffect1" )
	{
		SAFEDELETE( m_pTimeGateEffect[0] );
		m_pTimeGateEffect[0] = pImage;
	}
	else if( szType == "TimeGateEffect2" )
	{
		SAFEDELETE( m_pTimeGateEffect[1] );
		m_pTimeGateEffect[1] = pImage;
	}
	else if( szType == "TimeGateEffect3" )
	{
		SAFEDELETE( m_pTimeGateEffect[2] );
		m_pTimeGateEffect[2] = pImage;
	}
	else if( szType == "TimeGateEffect4" )
	{
		SAFEDELETE( m_pTimeGateEffect[3] );
		m_pTimeGateEffect[3] = pImage;
	}
	else if( szType == "TimeGateEffect5" )
	{
		SAFEDELETE( m_pTimeGateEffect[4] );
		m_pTimeGateEffect[4] = pImage;
	}
	else if( szType == "TimeGateEffect6" )
	{
		SAFEDELETE( m_pTimeGateEffect[5] );
		m_pTimeGateEffect[5] = pImage;
	}
	else if( szType == "TimeGateEffect7" )
	{
		SAFEDELETE( m_pTimeGateEffect[6] );
		m_pTimeGateEffect[6] = pImage;
	}
	else if( szType == "TimeGateText" )
	{
		SAFEDELETE( m_pTimeGateText );
		m_pTimeGateText = pImage;
	}
	else if( szType == "TextBG" )
	{
		SAFEDELETE( m_pTextBG );
		m_pTextBG = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void TimeGateBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo(xElement);

	char szKey[MAX_PATH];
	for( int i = 0; i < 3; ++i )
	{
		wsprintf_e( szKey, "TimeGateText%d", i+1 );
		m_szTimeGateText[i] = xElement.GetStringAttribute( szKey );
	}
	m_iEffectAniTime = xElement.GetIntAttribute_e( "EffectAniTime" );
	if( m_iEffectAniTime == 0 )
		m_iEffectAniTime = 100;
}

void TimeGateBtn::iwm_create()
{
	ioINILoader_e kLoader( "config/sp2_timegate_info.ini" );
	kLoader.SetTitle_e( "common" );
	m_iCoolTime = kLoader.LoadInt_e( "need_cooltime", 0 );
}

void TimeGateBtn::OnProcess( float fTimePerSec )
{
	CTime kTimeGateTime = g_MyInfo.GetTimeGateTime();
	CTime KOpenTime = kTimeGateTime + CTimeSpan( 0, 0, m_iCoolTime, 0 );
	CTime kCurrTime = DateHelp::GetCurrTime();
	CTimeSpan Gap = KOpenTime - kCurrTime;
	if( kTimeGateTime.GetTime() == 0 || Gap.GetTimeSpan() < 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( m_iTimeGateState == eActive )
		{
			m_iEffectIndex = ( dwCurTime - m_dwEffectTime ) / m_iEffectAniTime;
			if( m_iEffectIndex >= 7 )
			{
				m_iEffectIndex = 0;
				m_dwEffectTime = dwCurTime;
			}
			return;
		}
		m_iTimeGateState = eActive;
		m_iEffectIndex = 0;
		m_dwEffectTime = dwCurTime;
		m_szRenderText = m_szTimeGateText[eActive];
		
	}
	else if( Gap.GetTotalMinutes() < 1 )
	{
		m_iTimeGateState = eWait_Close;
		m_szRenderText = m_szTimeGateText[eWait_Close];
	}
	else
	{
		char szTemp[MAX_PATH] = "";
		wsprintf( szTemp, m_szTimeGateText[eWait].c_str(), Gap.GetHours(), Gap.GetMinutes() );
		m_szRenderText = szTemp;
		m_iTimeGateState = eWait;
	}
}

void TimeGateBtn::OnRender()
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pTimeGateBG )
		m_pTimeGateBG->Render( iXPos, iYPos );
	if( m_iTimeGateState == eActive )
	{
		if( m_pTimeGateEffect[m_iEffectIndex] )
			m_pTimeGateEffect[m_iEffectIndex]->Render( iXPos + 3, iYPos + 3 );
		if( m_pSelected )
			m_pSelected->Render( iXPos, iYPos );
	}
	if( m_pTimeGateText )
		m_pTimeGateText->Render( iXPos + 68, iYPos + 10 );
	if( m_pTextBG )
		m_pTextBG->Render( iXPos + 66, iYPos + 37 );

	g_FontMgr.SetTextStyle( TS_NORMAL );	
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetVertAlign( TVA_TOP );
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	g_FontMgr.PrintText( iXPos + 118, iYPos + 40, FONT_SIZE_11, m_szRenderText.c_str() );

	ioButton::OnRender();
}
//////////////////////////////////////////////////////////////////////////
TimeGateResultWnd::TimeGateResultWnd()
{
	m_pTimeGateBG = NULL;
	m_pTimeGateTitle = NULL;
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pIconBG = NULL;

	m_pLoopEffect = NULL;
	m_pResultEffect = NULL;

	m_iState = TimeGate_Rolling;
	m_iListType = 0;
}

TimeGateResultWnd::~TimeGateResultWnd()
{
	SAFEDELETE( m_pTimeGateBG );
	SAFEDELETE( m_pTimeGateTitle );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pIconBG );
}

void TimeGateResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( !pImage )
		return;

	if( szType == "TimeGateBG" )
	{
		SAFEDELETE( m_pTimeGateBG );
		m_pTimeGateBG = pImage;
		m_pTimeGateBG->SetScale( 1.4f );
	}
	else if( szType == "TimeGateTitle" )
	{
		SAFEDELETE( m_pTimeGateTitle );
		m_pTimeGateTitle = pImage;
	}
	else if( szType == "IconBG" )
	{
		SAFEDELETE( m_pIconBG );
		m_pIconBG = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TimeGateResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "Rolling_Total_Time" );
	m_szResultMsg = xElement.GetStringAttribute_e( "ResultMsg" );
	m_szRollingSound = xElement.GetStringAttribute_e( "Rolling_Sound" );
	m_szResultSound = xElement.GetStringAttribute_e( "Result_Sound" );

	m_iEffectPosX = xElement.GetIntAttribute_e( "EffectPosX" );
	m_iEffectPosY = xElement.GetIntAttribute_e( "EffectPosY" );
}

void TimeGateResultWnd::iwm_create()
{
	ioINILoader_e kLoader( "config/sp2_timegate_info.ini" );
	kLoader.SetTitle_e( "common" );

	char szBuf[MAX_BUFFER_SIZE] = "";
	kLoader.LoadString_e( "error_message", "", szBuf, MAX_BUFFER_SIZE );
	m_szErrorMessage = szBuf;

	int iCount = kLoader.LoadInt_e( "count", 0 );
	for( int i = 0; i < iCount; i++ )
	{
		char szKey[MAX_PATH];
		char szText[MAX_BUFFER_SIZE];
		wsprintf_e( szKey, "item_list%d", i + 1 );
		kLoader.SetTitle( szKey );

		wsprintf_e( szKey, "item_list%d_msg", i + 1 );
		kLoader.LoadString( szKey, "", szText, MAX_BUFFER_SIZE );

		m_RewardTextMap.insert( RewardTextMap::value_type( i + 1, szText ) );
	}
}

void TimeGateResultWnd::iwm_show()
{
	m_pLoopEffect = GetEffect("LoopEffect");
	m_pResultEffect = GetEffect("ResultEffect");
}

void TimeGateResultWnd::iwm_hide()
{
	if( m_pLoopEffect )
		m_pLoopEffect->Stop();
	if(m_pResultEffect)
		m_pResultEffect->Stop();

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );
}

void TimeGateResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{			
			HideWnd();
		}
		break;
	}
}

bool TimeGateResultWnd::iwm_esc()
{
	if( m_iState != TimeGate_Result )
		return true;

	ioWnd *pExitBtn = FindChildWnd( ID_CLOSE );
	if( pExitBtn )
		iwm_command( pExitBtn, IOBN_BTNUP, 0 );

	return true;
}

void TimeGateResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_iState == TimeGate_Rolling && m_dwRollingEndTime < dwCurTime )
		SetResult();
}

void TimeGateResultWnd::SendPakcet()
{
	SP2Packet kPacket( CTPK_TIME_GATE_REQUEST );
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void TimeGateResultWnd::SetResult()
{
	m_iState = TimeGate_Result;
	ShowChildWnd( ID_CLOSE );

	if( m_pResultEffect )
	{
		if( m_pLoopEffect )
			m_pLoopEffect->Stop();
		
		m_pResultEffect->Start();
		m_pResultEffect->SetPosX( m_iEffectPosX - m_pResultEffect->GetWidth() / 2 );
		m_pResultEffect->SetPosY( m_iEffectPosY - m_pResultEffect->GetHeight() / 2 );
	}
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );

	if( g_ExSoundMgr.IsSound( ExSound::EST_GASHAPON_GET ) )
		g_ExSoundMgr.PlaySound( ExSound::EST_GASHAPON_GET );
	else
	{
		if( !m_szResultSound.IsEmpty() )
			g_SoundMgr.PlaySound( m_szResultSound );
	}
}

void TimeGateResultWnd::SetTimeGateInfo( int iListType, int iPresentType, int iValue1, int iValue2 )
{
	m_iState = TimeGate_Rolling;
	m_dwRollingEndTime = FRAMEGETTIME() + m_dwRollingTotalTime;

	ShowWnd();
	HideChildWnd( ID_CLOSE );

	if( m_pLoopEffect )
	{
		if( !m_pLoopEffect->IsLive() )
			m_pLoopEffect->Start();

		m_pLoopEffect->SetPosX( m_iEffectPosX - m_pLoopEffect->GetWidth() / 2 );
		m_pLoopEffect->SetPosY( m_iEffectPosY - m_pLoopEffect->GetHeight() / 2 );
	}
	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	m_iListType = iListType;

	m_szItemName = g_PresentMgr.GetPresentValue1Text( iPresentType, iValue1, iValue2 );
	m_szItemInfo = g_PresentMgr.GetPresentValue2Text( iPresentType, iValue1, iValue2 );

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	ioHashString szIconName = g_PresentMgr.GetPresentIconText( iPresentType, iValue1, iValue2 );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szIconName );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( iPresentType, iValue1, iValue2 ) );
}

void TimeGateResultWnd::SetTimeGateFail()
{
	g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrorMessage.c_str() );
	HideWnd();
}

void TimeGateResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_iState != TimeGate_Result )
	{
		if( m_pTimeGateBG )
			m_pTimeGateBG->Render( iXPos + 167, iYPos + 151, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else
	{
		if( m_pTimeGateTitle )
			m_pTimeGateTitle->Render( iXPos + 167, iYPos - 36 );
		if( m_pIconBG )
			m_pIconBG->Render( iXPos + 167, iYPos + 119 );
		if( m_pIcon )
			m_pIcon->Render( iXPos + 167, iYPos + 119 );
		if( m_pSubIcon )
			m_pSubIcon->Render( iXPos + 167, iYPos + 119 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetVertAlign( TVA_TOP );
		if( m_RewardTextMap.find( m_iListType ) != m_RewardTextMap.end() )
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_SKY );
			g_FontMgr.PrintText( iXPos + 167, iYPos + 44, FONT_SIZE_16, m_RewardTextMap.find( m_iListType )->second.c_str() );
		}
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 167, iYPos + 235, FONT_SIZE_12, m_szResultMsg.c_str() );

		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );		
		g_FontMgr.SetBkColor( TCT_DEFAULT_BLACK );
		g_FontMgr.SetTextColor( TCT_DEFAULT_YELLOW2 );
		g_FontMgr.PrintText( iXPos + 167, iYPos + 175, FONT_SIZE_18, m_szItemName.c_str() );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 167, iYPos + 199, FONT_SIZE_18, m_szItemInfo.c_str() );
	}
}