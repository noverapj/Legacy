
#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/HelpFunc.h"

#include "../Setting.h"
#include "../WndID.h"
#include "../DirectInput.h"
#include "../ioMyInfo.h"
#include "../ioBattleRoomMgr.h"
#include "../ioExtendSoundManager.h"

#include "ModeRankWnd.h"
#include "ioSP2GUIManager.h"
#include "BriefingWnd.h"
#include "ioModeHelpManager.h"
#include "../GameStage/Mode/Practice/IoPracticeManager.h"
#include "PartyVersusWnd.h"
#include "../ioBattleMode.h"

BriefingWnd::BriefingWnd() : m_pModeRankWnd( NULL )
{
	m_dwBriefingType = BRIEFING_START;

	m_pPlayStage			= NULL;

	m_pFinalRoundText		= NULL;
	m_pRoundText			= NULL;
	m_pRoundNum				= NULL;
	m_pGameStart			= NULL;
	m_pGameEnd				= NULL;
	m_pGameWin				= NULL;
	m_pGameDraw				= NULL;
	m_pGameLose				= NULL;
	m_pGameBlueWin			= NULL;
	m_pGameRedWin			= NULL;
	m_pGameBlueLose			= NULL;
	m_pGameRedLose			= NULL;
	m_pIconWin				= NULL;
	m_pIconDraw				= NULL;
	m_pIconLose				= NULL;
	m_pHelpBar				= NULL;
	m_pModeTitle			= NULL;
	m_pModeIcon				= NULL;
	m_pPvETitleWin			= NULL;
	m_pPvETitleLose			= NULL;
	m_pPvETitleFloor		= NULL;
	m_pPvETitleFloorNum		= NULL;
	m_pPvEModeTitleSub		= NULL;
	m_pPvEModeTitleLine		= NULL;
	m_pTournamentResultBack = NULL;
	m_pBattleModeRedTeamFrame	= NULL;
	m_pBattleModeBlueTeamFrame	= NULL;
	m_pBattleModeFrameCenter	= NULL;
	m_pBattleModeVS			= NULL;

	m_pShuffleResultEffect  = NULL;
	m_pShuffleEnd			= NULL;
	m_pShuffleStarBox		= NULL;
	m_pShuffleStarNum		= NULL;

	m_pHunterCoinResultEffect	= NULL;
	m_pHunterCoinEnd			= NULL;
	m_pHunterCoinBox			= NULL;
	m_pHunterCoinNum			= NULL;

	m_dwBriefingStartTime = 0;
	m_dwBriefingTime	  = 0;
	m_dwBriefingResultTime= 0;
	m_dwBriefingTotalResultTime = 0;
	m_iCurRound  = 0;
	m_iMaxRound  = 0;
	m_eModeType = MT_NONE;
	m_eOwnerTeam= TEAM_NONE;
	m_dwResultType  = RESULT_DRAW;
	m_iAlphaRate     = 0;

	m_iModeTitleX = 0;
	m_iModeTitleY = 0;
	m_iModeIconX  = 0;
	m_iModeIconY  = 0;

	m_dwContributeTotalBluePoint = 0;
	m_dwContributeTotalRedPoint  = 0;

	m_iStarCount = 0;
	m_iHunterCoinCount = 0;
}

BriefingWnd::~BriefingWnd()
{
	SAFEDELETE( m_pFinalRoundText );
	SAFEDELETE( m_pRoundText );
	SAFEDELETE( m_pRoundNum );
	SAFEDELETE( m_pGameStart );
	SAFEDELETE( m_pGameEnd );
	SAFEDELETE( m_pGameWin );
	SAFEDELETE( m_pGameDraw );
	SAFEDELETE( m_pGameLose );
	SAFEDELETE( m_pGameBlueWin );
	SAFEDELETE( m_pGameRedWin );
	SAFEDELETE( m_pGameBlueLose );
	SAFEDELETE( m_pGameRedLose );
	SAFEDELETE( m_pIconWin );
	SAFEDELETE( m_pIconDraw );
	SAFEDELETE( m_pIconLose );
	SAFEDELETE( m_pHelpBar );
	SAFEDELETE( m_pModeTitle );
	SAFEDELETE( m_pModeIcon );
	SAFEDELETE( m_pPvETitleWin );
	SAFEDELETE( m_pPvETitleLose );
	SAFEDELETE( m_pPvETitleFloor );
	SAFEDELETE( m_pPvETitleFloorNum );
	SAFEDELETE( m_pPvEModeTitleSub );
	SAFEDELETE( m_pPvEModeTitleLine );
	SAFEDELETE( m_pTournamentResultBack );
	SAFEDELETE( m_pBattleModeRedTeamFrame );
	SAFEDELETE( m_pBattleModeBlueTeamFrame );
	SAFEDELETE( m_pBattleModeFrameCenter );
	SAFEDELETE( m_pBattleModeVS );

	SAFEDELETE( m_pShuffleResultEffect );
	SAFEDELETE( m_pShuffleEnd );
	SAFEDELETE( m_pShuffleStarBox );
	SAFEDELETE( m_pShuffleStarNum );

	SAFEDELETE( m_pHunterCoinResultEffect );
	SAFEDELETE( m_pHunterCoinEnd );
	SAFEDELETE( m_pHunterCoinBox );
	SAFEDELETE( m_pHunterCoinNum );

	if( !m_CurrentSound.IsEmpty() )
		g_SoundMgr.StopSound( m_CurrentSound, 0 );
}

void BriefingWnd::LoadResource( ModeType eModeType, PlazaType ePlazaType )
{
	m_eModeType = eModeType;
	ioINILoader_e kLoader( "config/sp2_mode_briefing.ini" );
	switch( m_eModeType )
	{
	case MT_TOWER_DEFENSE:
		kLoader.SetTitle_e( "tower_defense" );
		break;
	case MT_DARK_XMAS:
		kLoader.SetTitle_e( "dark_xmas" );
		break;
	case MT_FIRE_TEMPLE:
		kLoader.SetTitle_e( "fire_temple" );
		break;
	case MT_SYMBOL:
		kLoader.SetTitle_e( "power" );
		break;
	case MT_UNDERWEAR:
		kLoader.SetTitle_e( "underwear" );
		break;
	case MT_CBT:
		kLoader.SetTitle_e( "cbt_mode" );
		break;
	case MT_CATCH:
		kLoader.SetTitle_e( "prisoner" );
		break;
	case MT_CATCH_RUNNINGMAN:
		kLoader.SetTitle_e( "prisoner_runningman" );
		break;
	case MT_KING:
		kLoader.SetTitle_e( "king" );
		break;
	case MT_SURVIVAL:
		kLoader.SetTitle_e( "survival" );
		break;
	case MT_FIGHT_CLUB:
		kLoader.SetTitle_e( "fight" );
		break;
	case MT_TEAM_SURVIVAL:
		kLoader.SetTitle_e( "teamsurvival" );
		break;
	case MT_BOSS:
		kLoader.SetTitle_e( "boss" );
		break;
	case MT_MONSTER_SURVIVAL:
		kLoader.SetTitle_e( "monstersurvival" );
		break;
	case MT_TRAINING:
		if( ePlazaType == PT_COMMUNITY )
			kLoader.SetTitle_e( "plaza_community" );
		else if( ePlazaType == PT_GUILD )
			kLoader.SetTitle_e( "plaza_guild" );
		else
			kLoader.SetTitle_e( "plaza_battle" );
		break;
	case MT_FOOTBALL:
		kLoader.SetTitle_e( "football" );
		break;
	case MT_HEROMATCH:
		kLoader.SetTitle_e( "heromatch" );
		break;
	case MT_GANGSI:
		kLoader.SetTitle_e( "gangsi" );
		break;
	case MT_DUNGEON_A:
		kLoader.SetTitle_e( "dungeon_a" );
		break;
	case MT_MYROOM:
		kLoader.SetTitle_e( "myroom" );
		break;
	case MT_HEADQUARTERS:
		kLoader.SetTitle_e( "headquarters" );
		break;
	case MT_DOUBLE_CROWN:
		kLoader.SetTitle_e( "doublecrown" );
		break;
	case MT_SHUFFLE_BONUS:
		kLoader.SetTitle_e( "shuffle" );
		break;
	case MT_FACTORY:
		kLoader.SetTitle_e( "factory" );
		break;
	case MT_TEAM_SURVIVAL_AI:
		kLoader.SetTitle_e( "teamsurvivalAI" );
		break;
	case MT_HOUSE:
		kLoader.SetTitle_e( "house" );
		break;
	case MT_RAID:
		kLoader.SetTitle_e( "raid" );
		break;
	case MT_PRACTICE:
		kLoader.SetTitle_e( "practice" );
		break;
	case MT_FLAG_CAPTURE:
		kLoader.SetTitle_e( "flagcapture" );
		break;
	case MT_ARENA:
		kLoader.SetTitle_e( "arena" );
		break;
	case MT_BATTLE:
		kLoader.SetTitle_e( "battle" );
		break;
	case MT_FARMING:
		kLoader.SetTitle_e( "farming" );
		break;
	}

	char szBuf[MAX_PATH] = "";
	m_iMaxRound = kLoader.LoadInt_e( "max_round", 0 );
	m_dwBriefingTime = kLoader.LoadInt_e( "briefing_time", 0 );
	m_dwBriefingResultTime = kLoader.LoadInt_e( "briefing_result_time", 0 );
	m_dwBriefingTotalResultTime = kLoader.LoadInt_e( "briefing_total_result_time", 0 );
	kLoader.LoadString_e( "briefing_text", "", szBuf, MAX_PATH );          //라운드 시작 브리핑 텍스트
	m_BriefingText = szBuf;
	
	kLoader.LoadString_e( "title_image", "", szBuf, MAX_PATH );
	m_pModeTitle = g_UIImageSetMgr.CreateImageByFullName( szBuf );


	if( m_eModeType == MT_PRACTICE )
	{
		m_dwBriefingTime = g_PracticeMgr.GetPracticeBriefingTime();

		ioHashString szTitle = g_PracticeMgr.GetPracticeLoadingText();
		if( !szTitle.IsEmpty() )
			m_pModeTitle = g_UIImageSetMgr.CreateImageByFullName( szTitle );

		m_BriefingText = g_PracticeMgr.GetPracticeTypeDesc();

		kLoader.LoadString_e( "icon_image", "", szBuf, MAX_PATH );
		m_pModeIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	}

	if( m_pModeTitle )
	{
		m_iModeTitleX = kLoader.LoadInt_e( "title_x", 0 );
		m_iModeTitleY = kLoader.LoadInt_e( "title_y", 0 );
	}
	if( g_BattleRoomMgr.IsUseExtraOption() )
	{
		if( m_eModeType == MT_UNDERWEAR ||  m_eModeType == MT_CBT || m_eModeType == MT_FARMING )
			kLoader.LoadString_e( "icon_image", "", szBuf, MAX_PATH );
		else
			kLoader.LoadString_e( "icon_image_user_mode", "", szBuf, MAX_PATH );
	}
	else
	{
		kLoader.LoadString_e( "icon_image", "", szBuf, MAX_PATH );
	}

	m_pModeIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	if( m_pModeIcon )
	{
		m_iModeIconX = kLoader.LoadInt_e( "icon_x", 0 );
		m_iModeIconY = kLoader.LoadInt_e( "icon_y", 0 );
	}

	kLoader.LoadString_e( "final_result_map_effect_blue", "", szBuf, MAX_PATH );
	m_FinalResultMapEffectBlue = szBuf;
	kLoader.LoadString_e( "final_result_map_effect_red", "", szBuf, MAX_PATH );
	m_FinalResultMapEffectRed = szBuf;
	kLoader.LoadString_e( "final_result_map_effect_none", "", szBuf, MAX_PATH );
	m_FinalResultMapEffectNone= szBuf;
}

ioHashString &BriefingWnd::GetFinalResultMapEffect( TeamType eTeamType )
{
	if( eTeamType == TEAM_BLUE )
		return m_FinalResultMapEffectBlue;
	else if( eTeamType == TEAM_RED )
		return m_FinalResultMapEffectRed;

	return m_FinalResultMapEffectNone;
}

void BriefingWnd::PlaySoundName( const ioHashString &szSoundName )
{
	if( !m_CurrentSound.IsEmpty() )
		g_SoundMgr.StopSound( m_CurrentSound, 0 );
	if( !szSoundName.IsEmpty() )
		g_SoundMgr.PlaySound( szSoundName );			
	m_CurrentSound = szSoundName;
}

void BriefingWnd::SetStartBriefing( int iCurRound )
{
	if( iCurRound < 1 )  return;

	m_dwBriefingType = BRIEFING_START;
	m_iCurRound		 = iCurRound;	
	m_dwBriefingStartTime = REALGETTIME();

	if( m_eModeType == MT_FOOTBALL )
		PlaySoundName( m_StartSoundFootBall );
	else
		PlaySoundName( m_StartSound );

	if( m_eModeType == MT_BATTLE )
		SetStartBriefingForBattleMode();
	else
		HidePartyMemberSlotUI();			// 배틀모드가 아닐때 대기방 유저슬롯UI Hide시킴

	ShowWnd();

	if( m_eModeType != MT_SHUFFLE_BONUS )
	{
		if( !m_pModeRankWnd )
			m_pModeRankWnd = FindChildWnd( ID_MODE_RANK );
		if( m_pModeRankWnd )
			m_pModeRankWnd->HideWnd();
	}
}

void BriefingWnd::SetPlayStage( ioPlayStage* pPlayStage )
{
	m_pPlayStage	= pPlayStage;
}

void BriefingWnd::SetResultInfo( ioPlayStage *pPlayStage, DWORD dwBriefingType, DWORD dwResultType, TeamType eTeamType, int iCurRound )
{
	m_pPlayStage	= pPlayStage;
	m_dwBriefingType = dwBriefingType;
	m_dwResultType	 = dwResultType;
	m_eOwnerTeam     = eTeamType;
	m_iCurRound		 = iCurRound;
	m_dwBriefingStartTime = REALGETTIME();

	
	if( dwBriefingType == BRIEFING_FINAL_RESULT && g_ExSoundMgr.IsSound( ExSound::EST_WIN ) && g_ExSoundMgr.IsSound( ExSound::EST_LOSE ) )
	{
		if( dwResultType == RESULT_WIN )
			g_ExSoundMgr.PlaySound( ExSound::EST_WIN );
		else
			g_ExSoundMgr.PlaySound( ExSound::EST_LOSE );
	}
	else
	{
		switch( dwBriefingType )
		{
		case BRIEFING_RESULT:
			{
				if( m_eModeType == MT_FOOTBALL )
				{
					if( dwResultType == RESULT_WIN )
						PlaySoundName( m_RoundWinSoundFootBall );
					else
						PlaySoundName( m_RoundLoseSoundFootBall );
				}
				else
				{
					if( dwResultType == RESULT_WIN )
						PlaySoundName( m_RoundWinSound );
					else
						PlaySoundName( m_RoundLoseSound );
				}
			}
			break;
		case BRIEFING_FINAL_RESULT:
			{
				if( m_eModeType == MT_FOOTBALL )
				{
					if( dwResultType == RESULT_WIN )
						PlaySoundName( m_ResultWinSoundFootBall );
					else if( dwResultType == RESULT_LOSE )
						PlaySoundName( m_ResultLoseSoundFootBall );
					else
						PlaySoundName( m_ResultDrawSoundFootBall );
				}
				else
				{
					if( dwResultType == RESULT_WIN )
						PlaySoundName( m_ResultWinSound );
					else if( dwResultType == RESULT_LOSE )
						PlaySoundName( m_ResultLoseSound );
					else
						PlaySoundName( m_ResultDrawSound );
				}
			}
			break;
		}
	}

	ShowWnd();

	if( m_eModeType != MT_SHUFFLE_BONUS && m_eModeType != MT_RAID )
	{
		if( !m_pModeRankWnd )
			m_pModeRankWnd = FindChildWnd( ID_MODE_RANK );

		if( m_pModeRankWnd )
		{
			ModeRankWnd* pModeRankWnd = dynamic_cast<ModeRankWnd*>( m_pModeRankWnd );
			if ( pModeRankWnd )
			{
				pModeRankWnd->SetPlayStage( pPlayStage );
				pModeRankWnd->SetRank( ModeRankWnd::RESULT_TYPE );
			}
		}
	}

	if( m_eModeType == MT_BATTLE )
	{
		for( int i = 0; i < 3; i++ )
		{
			const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( TEAM_BLUE, i );
			PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( ID_BLUE_BATTLE_USER_1 + i ));
			if (pBtn != NULL)
				pBtn->HideWnd();
		}

		for( int i = 0; i < 3; i++ )
		{
			const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( TEAM_RED, i );
			PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( ID_RED_BATTLE_USER_1 + i ));
			if (pBtn != NULL)
				pBtn->HideWnd();
		}
	}
}

void BriefingWnd::SetTournamentContributePoint( DWORD dwBluePoint, DWORD dwRedPoint )
{
	m_dwContributeTotalBluePoint = dwBluePoint;
	m_dwContributeTotalRedPoint = dwRedPoint;
}

void BriefingWnd::SetShuffleStarCount( int iStarCount )
{
	m_iStarCount = iStarCount;
}

void BriefingWnd::SetHunterCoinCount( int iHunterCoinCount )
{
	m_iHunterCoinCount = iHunterCoinCount;
}

void BriefingWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_StartSound		= xElement.GetStringAttribute_e( "StartSound" );
	m_RoundWinSound		= xElement.GetStringAttribute_e( "RoundWinSound" );
	m_RoundLoseSound	= xElement.GetStringAttribute_e( "RoundLoseSound" );
	m_ResultWinSound	= xElement.GetStringAttribute_e( "ResultWinSound" );
	m_ResultLoseSound	= xElement.GetStringAttribute_e( "ResultLoseSound" );
	m_ResultDrawSound	= xElement.GetStringAttribute_e( "ResultDrawSound" );

	m_StartSoundFootBall		= xElement.GetStringAttribute_e( "StartSoundFootBall" );
	m_RoundWinSoundFootBall		= xElement.GetStringAttribute_e( "RoundWinSoundFootBall" );
	m_RoundLoseSoundFootBall	= xElement.GetStringAttribute_e( "RoundLoseSoundFootBall" );
	m_ResultWinSoundFootBall	= xElement.GetStringAttribute_e( "ResultWinSoundFootBall" );
	m_ResultLoseSoundFootBall	= xElement.GetStringAttribute_e( "ResultLoseSoundFootBall" );
	m_ResultDrawSoundFootBall	= xElement.GetStringAttribute_e( "ResultDrawSoundFootBall" );
}

void BriefingWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "FinalRoundText" )
	{
		SAFEDELETE( m_pFinalRoundText );
		m_pFinalRoundText = pImage;
	}
	else if( szType == "RoundText" )
	{
		SAFEDELETE( m_pRoundText );
		m_pRoundText = pImage;
	}
	else if( szType == "Number" )
	{
		SAFEDELETE( m_pRoundNum );
		m_pRoundNum = pImage;
	}
	else if( szType == "GameStart" )
	{
		SAFEDELETE( m_pGameStart );
		m_pGameStart = pImage;
	}
	else if( szType == "GameEnd" )
	{
		SAFEDELETE( m_pGameEnd );
		m_pGameEnd = pImage;
	}
	else if( szType == "GameWin" )
	{
		SAFEDELETE( m_pGameWin );
		m_pGameWin = pImage;
	}
	else if( szType == "GameDraw" )
	{
		SAFEDELETE( m_pGameDraw );
		m_pGameDraw = pImage;
	}
	else if( szType == "GameLose" )
	{
		SAFEDELETE( m_pGameLose );
		m_pGameLose = pImage;
	}
	else if( szType == "GameBlueWin" )
	{
		SAFEDELETE( m_pGameBlueWin );
		m_pGameBlueWin = pImage;
	}
	else if( szType == "GameRedWin" )
	{
		SAFEDELETE( m_pGameRedWin );
		m_pGameRedWin = pImage;
	}
	else if( szType == "GameBlueLose" )
	{
		SAFEDELETE( m_pGameBlueLose );
		m_pGameBlueLose = pImage;
	}
	else if( szType == "GameRedLose" )
	{
		SAFEDELETE( m_pGameRedLose );
		m_pGameRedLose = pImage;
	}
	else if( szType == "IconWin" )
	{
		SAFEDELETE( m_pIconWin );
		m_pIconWin = pImage;
	}
	else if( szType == "IconDraw" )
	{
		SAFEDELETE( m_pIconDraw );
		m_pIconDraw = pImage;
	}
	else if( szType == "IconLose" )
	{
		SAFEDELETE( m_pIconLose );
		m_pIconLose = pImage;
	}
	else if( szType == "HelpBar" )
	{
		SAFEDELETE( m_pHelpBar );
		m_pHelpBar = pImage;
	}
	else if( szType == "PvETitleWin" )
	{
		SAFEDELETE( m_pPvETitleWin );
		m_pPvETitleWin = pImage;
	}
	else if( szType == "PvETitleLose" )
	{
		SAFEDELETE( m_pPvETitleLose );
		m_pPvETitleLose = pImage;
	}
	else if( szType == "PvETitleFloor" )
	{
		SAFEDELETE( m_pPvETitleFloor );
		m_pPvETitleFloor = pImage;
	}
	else if( szType == "PvETitleFloorNum" )
	{
		SAFEDELETE( m_pPvETitleFloorNum );
		m_pPvETitleFloorNum = pImage;
	}
	else if( szType == "PvEModeTitleSub" )
	{
		SAFEDELETE( m_pPvEModeTitleSub );
		m_pPvEModeTitleSub = pImage;
	}
	else if( szType == "PvEModeTitleLine" )
	{
		SAFEDELETE( m_pPvEModeTitleLine );
		m_pPvEModeTitleLine = pImage;
	}
	else if( szType == "TournamentResultBack" )
	{
		SAFEDELETE( m_pTournamentResultBack );
		m_pTournamentResultBack = pImage;
	}
	else if( szType == "ShuffleResultEffect" )
	{
		SAFEDELETE( m_pShuffleResultEffect );
		m_pShuffleResultEffect = pImage;
	}
	else if( szType == "ShuffleEnd" )
	{
		SAFEDELETE( m_pShuffleEnd );
		m_pShuffleEnd = pImage;
	}
	else if( szType == "ShuffleStarBox" )
	{
		SAFEDELETE( m_pShuffleStarBox );
		m_pShuffleStarBox = pImage;
	}
	else if( szType == "ShuffleStarNum" )
	{
		SAFEDELETE( m_pShuffleStarNum );
		m_pShuffleStarNum = pImage;
	}
	else if( szType == "HunterCoinResultEffect" )
	{
		SAFEDELETE( m_pHunterCoinResultEffect );
		m_pHunterCoinResultEffect = pImage;
	}
	else if( szType == "HunterCoinEnd" )
	{
		SAFEDELETE( m_pHunterCoinEnd );
		m_pHunterCoinEnd = pImage;
	}
	else if( szType == "HunterCoinBox" )
	{
		SAFEDELETE( m_pHunterCoinBox );
		m_pHunterCoinBox = pImage;
	}
	else if( szType == "HunterCoinNum" )
	{
		SAFEDELETE( m_pHunterCoinNum );
		m_pHunterCoinNum = pImage;
	}
	else if( szType == "BattleModeRedTeamFrame" )
	{
		SAFEDELETE( m_pBattleModeRedTeamFrame );
		m_pBattleModeRedTeamFrame = pImage;
	}
	else if( szType == "BattleModeBlueTeamFrame" )
	{
		SAFEDELETE( m_pBattleModeBlueTeamFrame );
		m_pBattleModeBlueTeamFrame = pImage;
	}
	else if( szType == "BattleModeFrameCenter" )
	{
		SAFEDELETE( m_pBattleModeFrameCenter );
		m_pBattleModeFrameCenter = pImage;
	}
	else if( szType == "BattleModeVS" )
	{
		SAFEDELETE( m_pBattleModeVS );
		m_pBattleModeVS = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void BriefingWnd::iwm_show()
{
	switch( m_dwBriefingType )
	{
	case BRIEFING_RESULT:		
	case BRIEFING_FINAL_RESULT:
		{
			g_GUIMgr.HideWnd( MINIMAP_WND );
			g_GUIMgr.HideWnd( TIME_SCORE_WND );
			g_GUIMgr.HideWnd( SKILL_LIST_WND );
			g_GUIMgr.HideWnd( ACCESSORY_SKILL_LIST_WND );
			g_GUIMgr.HideWnd( MODE_RANK_WND );
			g_GUIMgr.HideWnd( CENTER_INFO_WND );
			g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );
			g_GUIMgr.HideWnd( MONSTER_BOSS_GAUGE_WND );
			g_GUIMgr.HideWnd( TRAINING_FUNC_MENU );

			g_ModeHelpMgr.HideHelpDirectly();

			// TAB키를 누르고 있으면 보이지 않는다.
			if( g_Input.IsKeyDown( KEY_TAB ) )
			{
				SetWndPos( Setting::Width(), Setting::Height() );
			}
			else
			{
				SetWndPos( m_iDefXPos, m_iDefYPos );
			}
		}
		break;
	default:
		SetWndPos( m_iDefXPos, m_iDefYPos );
		break;
	}		
	m_iAlphaRate = MAX_ALPHA_RATE;
}

void BriefingWnd::iwm_hide()
{
	switch( m_dwBriefingType )
	{
	case BRIEFING_RESULT:
		g_GUIMgr.CheckShopSound( false );

		if( m_eModeType != MT_SURVIVAL && m_eModeType != MT_BOSS && m_eModeType != MT_MONSTER_SURVIVAL && 
			m_eModeType != MT_GANGSI && m_eModeType != MT_DUNGEON_A && m_eModeType != MT_FIGHT_CLUB && m_eModeType != MT_SHUFFLE_BONUS && m_eModeType != MT_RAID  && 
			!Help::IsMonsterDungeonMode(m_eModeType) )
			g_GUIMgr.ShowWnd( ROUND_RESULT_MAIN_WND );
		break;
	case BRIEFING_FINAL_RESULT:
		g_SoundMgr.AllSoundStop();
		g_SoundMgr.StopStream();

		g_GUIMgr.CheckShopSound( false );

		g_GUIMgr.ShowWnd( TOTAL_RESULT_MAIN_WND );

		if( !m_CurrentSound.IsEmpty() )
		{
			g_SoundMgr.StopSound( m_CurrentSound, 0 );
			m_CurrentSound.Clear();
		}

		HideChildWnd( ID_MODE_RANK );
		break;
	}
}

void BriefingWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwBriefingStartTime;
	DWORD dwAlphaTime = 0;
	switch( m_dwBriefingType )
	{
	case BRIEFING_START:
		if( dwGapTime > m_dwBriefingTime )
			m_dwBriefingStartTime = 0;
		else if( dwGapTime > m_dwBriefingTime - 1000 )           //마지막 1초는 알파값이 적용된다.
			dwAlphaTime= dwGapTime - ( m_dwBriefingTime - 1000 );
		break;
	case BRIEFING_RESULT:
		if( dwGapTime > m_dwBriefingResultTime )
			m_dwBriefingStartTime = 0;
		else if( dwGapTime > m_dwBriefingResultTime - 1000 )           //마지막 1초는 알파값이 적용된다.
			dwAlphaTime = dwGapTime - ( m_dwBriefingResultTime - 1000 );
		break;
	case BRIEFING_FINAL_RESULT:
		if( dwGapTime > m_dwBriefingTotalResultTime )
			m_dwBriefingStartTime = 0;
		else if( dwGapTime > m_dwBriefingTotalResultTime - 1000 )           //마지막 1초는 알파값이 적용된다.
			dwAlphaTime = dwGapTime - ( m_dwBriefingTotalResultTime - 1000 );
		break;
	}

	if( m_dwBriefingStartTime == 0 )
	{
		HideWnd();
	}
	else if( dwAlphaTime != 0 )
	{
		float fRate = (float)dwAlphaTime / FLOAT1000;
		fRate = min( FLOAT1, fRate );
		fRate = FLOAT1 - fRate;			
		m_iAlphaRate = (float)MAX_ALPHA_RATE * fRate;
	}
	else
		m_iAlphaRate = MAX_ALPHA_RATE;

	switch( m_dwBriefingType )
	{
	case BRIEFING_RESULT:
	case BRIEFING_FINAL_RESULT:
		{
			ModeRankWnd* pModeRankWnd = dynamic_cast<ModeRankWnd*>( m_pModeRankWnd );
			if ( pModeRankWnd )
			{
				pModeRankWnd->SetAlpha( m_iAlphaRate );
			}

			// TAB키를 누르고 있으면 보이지 않는다.
			if( g_Input.IsKeyDown( KEY_TAB ) )
			{
				SetWndPos( Setting::Width(), Setting::Height() );
			}
			else
			{
				SetWndPos( m_iDefXPos, m_iDefYPos );
			}
		}
		break;
	}
}

void BriefingWnd::OnRender()
{
	if( m_eModeType == MT_HEROMATCH && m_dwBriefingType == BRIEFING_START  )
		return;     // 래더전은 시작 브리핑 표시 안함
	if( m_eModeType == MT_SUCCESSION && m_dwBriefingType == BRIEFING_START  )
		return;     // 래더전은 시작 브리핑 표시 안함

	RenderModeBriefing( GetDerivedPosX(), GetDerivedPosY() );        

	//모드 랭크 UI가 마지막에 그려진다.
	ioWnd::OnRender();	
}

void BriefingWnd::RenderModeBriefing( int iXPos, int iYPos )
{
	iXPos = iXPos + ( Setting::Width() / 2 );
	iYPos = iYPos + ( Setting::Height() / 2 );
	
	// 해상도에 따라서 더 위로 올린다.
	if( m_dwBriefingType != BRIEFING_START )
	{
		const int iMinHeight = 600;
		int iHeightCenterGap = (float)( Setting::Height() - iMinHeight ) * 0.192f;
		iYPos -= iHeightCenterGap;
	}

	// PvE Mode는 다른 브리핑 규칙을 따른다.
	if( m_eModeType == MT_MONSTER_SURVIVAL || m_eModeType == MT_DUNGEON_A ) 
	{
		switch( m_dwBriefingType )
		{
		case BRIEFING_START:
			RenderPvEBriefingStart( iXPos, iYPos );
			break;
		case BRIEFING_FINAL_RESULT:
			RenderPvEBriefingFinalResult( iXPos, iYPos + 15 );
			break;
		}
	}
	else if( Help::IsMonsterDungeonMode(m_eModeType) || m_eModeType == MT_RAID )
	{
		if( m_pModeTitle )
		{
			m_pModeTitle->SetAlpha( (BYTE)m_iAlphaRate );
			m_pModeTitle->Render( iXPos + m_iModeTitleX, iYPos + m_iModeTitleY );
		}

		switch( m_dwBriefingType )
		{
		case BRIEFING_START:
			RenderBriefingStart( iXPos, iYPos );
			break;
		case BRIEFING_FINAL_RESULT:
//			RenderPvEBriefingFinalResult( iXPos, iYPos + 15 );
			RenderBriefingFinalResult( iXPos, iYPos - 50 );
			break;
		}
	}
	else
	{
		if( m_pModeTitle )
		{
			m_pModeTitle->SetAlpha( (BYTE)m_iAlphaRate );
			m_pModeTitle->Render( iXPos + m_iModeTitleX, iYPos + m_iModeTitleY );
		}	
		switch( m_dwBriefingType )
		{
		case BRIEFING_START:
			RenderBriefingStart( iXPos, iYPos );
			break;
		case BRIEFING_RESULT:
			RenderBriefingResult( iXPos, iYPos - 50 );
			break;
		case BRIEFING_FINAL_RESULT:
			RenderBriefingFinalResult( iXPos, iYPos - 50 );
			break;
		}
	}
}

void BriefingWnd::RenderBriefingStart( int iXPos, int iYPos )
{
	if( m_pHelpBar && !m_BriefingText.IsEmpty() && m_eModeType != MT_BATTLE )
	{
		m_pHelpBar->SetAlpha( (BYTE)m_iAlphaRate );
		m_pHelpBar->Render( iXPos - 264, iYPos - 36 );
	}

	if( m_pModeIcon && m_eModeType != MT_BATTLE )
	{
		m_pModeIcon->SetAlpha( (BYTE)m_iAlphaRate );
		m_pModeIcon->Render( iXPos + m_iModeIconX, iYPos + m_iModeIconY );
	}
	switch( m_eModeType )
	{
	case MT_SYMBOL:
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_CATCH:
	case MT_KING:
	case MT_FOOTBALL:
	case MT_HEROMATCH:
	case MT_CATCH_RUNNINGMAN:
	case MT_ARENA:
	case MT_FARMING:
		{
			if( m_pGameStart )
			{
				m_pGameStart->SetAlpha( (BYTE)m_iAlphaRate );
				m_pGameStart->Render( iXPos - 24, iYPos - 124 );			
			}

			if( m_iCurRound == ( m_iMaxRound * 2 ) - 1 )
			{
				if( m_pFinalRoundText )
				{
					m_pFinalRoundText->SetAlpha( (BYTE)m_iAlphaRate );
					m_pFinalRoundText->Render( iXPos - 24, iYPos - 124 );
				}
			}
			else
			{
				if( m_pRoundText )
				{
					m_pRoundText->SetAlpha( (BYTE)m_iAlphaRate );
					m_pRoundText->Render( iXPos - 24, iYPos - 124 );	
				}
				if( m_pRoundNum )
				{
					m_pRoundNum->SetAlpha( (BYTE)m_iAlphaRate );
					m_pRoundNum->RenderNum( iXPos + 45, iYPos - 123, m_iCurRound );
				}
			}			
		}
		break;
	case MT_SURVIVAL:
	case MT_FIGHT_CLUB:
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_BOSS:
	case MT_GANGSI:
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_DOUBLE_CROWN:
	case MT_SHUFFLE_BONUS:
	case MT_FACTORY:
	case MT_RAID:
	case MT_FLAG_CAPTURE:
	//case MT_BATTLE:
		{
			if( m_pGameStart )
			{
				m_pGameStart->SetAlpha( (BYTE)m_iAlphaRate );
				m_pGameStart->Render( iXPos - 24, iYPos - 130 );
			}
		}
		break;
	case MT_BATTLE:
		{
			RenderBriefingStartForBattleMode( iXPos, iYPos );
		}
		break;
	}	

	if( !m_BriefingText.IsEmpty() && m_eModeType != MT_BATTLE )
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( 229, 229, 229 );
		g_FontMgr.PrintTextAlpha( iXPos, iYPos - 34, FONT_SIZE_13, (BYTE)m_iAlphaRate, m_BriefingText.c_str() );
	}
}

void BriefingWnd::RenderBriefingStartForBattleMode( int iXPos, int iYPos )
{
	//m_pBattleModeRedTeamFrame->SetAlpha( (BYTE)m_iAlphaRate );
	m_pBattleModeRedTeamFrame->Render( iXPos - 199, iYPos );

	//m_pBattleModeBlueTeamFrame->SetAlpha( (BYTE)m_iAlphaRate );
	m_pBattleModeBlueTeamFrame->Render( iXPos + 199, iYPos );

	//m_pBattleModeFrameCenter->SetAlpha( (BYTE)m_iAlphaRate );
	m_pBattleModeFrameCenter->Render( iXPos, iYPos );

	//m_pBattleModeVS->SetAlpha( (BYTE)m_iAlphaRate );
	m_pBattleModeVS->Render( iXPos, iYPos );

	/*for( int i = ID_BLUE_BATTLE_USER_1; i <= ID_RED_BATTLE_USER_3; i++ )
	{
		PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( i ));

		if( pBtn )
		{
			pBtn->SetWindowAlpha( (BYTE)m_iAlphaRate );
			pBtn->RenderWnd();
		}
	}*/
}

void BriefingWnd::SetStartBriefingForBattleMode()
{
	if( !m_pPlayStage )
		return;
	ioPlayMode* pPlayMode = m_pPlayStage->GetPlayMode();
	if( !pPlayMode )
		return;
	ioBattleMode* pBattleMode = ToBattleMode( pPlayMode );
	if( !pBattleMode )
		return;

	// 모든 유저 정보 갱신되지 않았으면 아래 정보 넣는 것 실행되지 않음
	if( !pBattleMode->AllFirstUserCheck() )
		return;

	int iXPos = GetDerivedPosX() + ( Setting::Width() / 2 );
	int iYPos = GetDerivedPosY() + ( Setting::Height() / 2 );

	for( int i = 0; i < 3; i++ )
	{
		const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( TEAM_BLUE, i );
		PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( ID_BLUE_BATTLE_USER_1 + i ));
		if (pBtn != NULL)
		{
			pBtn->MemberInfo( TEAM_BLUE, i + 1, kInfo.m_iGradeLevel, kInfo.m_szUserID, -1, -1, -1 );
			pBtn->ShowWnd();
			//pBtn->SetWndPos( iXPos + 52 + ( i * 90 ), iYPos -92 );
			pBtn->SetWndPos( iXPos - 353 + ( i * 93 ), iYPos - 117 );
		}
	}

	for( int i = 0; i < 3; i++ )
	{
		const BattleRoomUserInfo &kInfo = g_BattleRoomMgr.GetBattleUserInfo( TEAM_RED, i );
		PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( ID_RED_BATTLE_USER_1 + i ));
		if (pBtn != NULL)
		{
			pBtn->MemberInfo( TEAM_RED, i + 1, kInfo.m_iGradeLevel, kInfo.m_szUserID, -1, -1, -1 );
			pBtn->ShowWnd();
			//pBtn->SetWndPos( iXPos -322 + ( i * 93 ), iYPos -92 );
			pBtn->SetWndPos( iXPos + 72 + ( i * 90 ), iYPos - 117 );
		}
	}
}

void BriefingWnd::HidePartyMemberSlotUI()
{
	for( int i = 0; i < 3; i++ )
	{
		PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( ID_BLUE_BATTLE_USER_1 + i ));
		if (pBtn != NULL)
			pBtn->HideWnd();
	}

	for( int i = 0; i < 3; i++ )
	{
		PartyMemberList *pBtn = dynamic_cast<PartyMemberList*>(FindChildWnd( ID_RED_BATTLE_USER_1 + i ));
		if (pBtn != NULL)
			pBtn->HideWnd();
	}
}

void BriefingWnd::RenderBriefingResult( int iXPos, int iYPos )
{
	if( m_pHelpBar && !g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) )
	{
		m_pHelpBar->SetAlpha( (BYTE)m_iAlphaRate );
		m_pHelpBar->Render( iXPos - 264, iYPos - 36 );
	}

	switch( m_eModeType )
	{
	case MT_SYMBOL:
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_CATCH:
	case MT_KING:
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_FOOTBALL:
	case MT_HEROMATCH:		
	case MT_CATCH_RUNNINGMAN:
	case MT_DOUBLE_CROWN:
	case MT_SUCCESSION:
	case MT_FLAG_CAPTURE:
	case MT_ARENA:
	case MT_BATTLE:
	case MT_FARMING:
		{
			if( m_dwResultType == RESULT_WIN )
			{
				if( m_pIconWin )
				{
					m_pIconWin->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconWin->Render( iXPos - 62, iYPos - 89 );
				}

				ioUIRenderImage *pWinTitle = m_pGameRedWin;
				if( m_eOwnerTeam == TEAM_BLUE )
					pWinTitle = m_pGameBlueWin;				
				if( pWinTitle )
				{
					pWinTitle->SetAlpha( (BYTE)m_iAlphaRate );
					pWinTitle->Render( iXPos - 35, iYPos - 124 );		
				}
			}
			else 
			{
				if( m_pIconLose )
				{
					m_pIconLose->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconLose->Render( iXPos - 62, iYPos - 89 );
				}

				ioUIRenderImage *pLoseTitle = m_pGameRedLose;
				if( m_eOwnerTeam == TEAM_BLUE )
					pLoseTitle = m_pGameBlueLose;	
				if( pLoseTitle )
				{
					pLoseTitle->SetAlpha( (BYTE)m_iAlphaRate );
					pLoseTitle->Render( iXPos - 35, iYPos - 124 );			
				}
			}

			if( m_pRoundText )
			{
				m_pRoundText->SetAlpha( (BYTE)m_iAlphaRate );
				m_pRoundText->Render( iXPos - 35, iYPos - 124 );	
			}
			if( m_pRoundNum )
			{
				m_pRoundNum->SetAlpha( (BYTE)m_iAlphaRate );
				m_pRoundNum->RenderNum( iXPos + 34, iYPos - 123, m_iCurRound );
			}
		}
		break;
	}	
}

void BriefingWnd::RenderBriefingFinalResult( int iXPos, int iYPos )
{
	if( m_pHelpBar && !g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) && m_eModeType != MT_SHUFFLE_BONUS )
	{
		m_pHelpBar->SetAlpha( (BYTE)m_iAlphaRate );
		m_pHelpBar->Render( iXPos - 264, iYPos - 36 );
	}

	int iDrawResultX = 0;

	int iTournamentYOffset = 0;
	if( g_BattleRoomMgr.IsTournamentModeType() && 0 < m_dwContributeTotalBluePoint && 0 < m_dwContributeTotalRedPoint )
	{
		iTournamentYOffset = -23;
	}	

	switch( m_eModeType )
	{
	case MT_SYMBOL:
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_CATCH:
	case MT_KING:
	case MT_FOOTBALL:
	case MT_HEROMATCH:
	case MT_CATCH_RUNNINGMAN:
	case MT_SUCCESSION:
	case MT_ARENA:
	case MT_FARMING:
		{
			if( m_dwResultType == RESULT_WIN )
			{
				if( m_pIconWin )
				{
					m_pIconWin->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconWin->Render( iXPos - 51, iYPos - 89 + iTournamentYOffset );
				}
				if( m_pGameWin )
				{
					m_pGameWin->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameWin->Render( iXPos - 24, iYPos - 124 + iTournamentYOffset );		
				}
			}
			else if( m_dwResultType == RESULT_LOSE )
			{
				if( m_pIconLose )
				{
					m_pIconLose->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconLose->Render( iXPos - 51, iYPos - 89 + iTournamentYOffset );
				}
				if( m_pGameLose )
				{
					m_pGameLose->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameLose->Render( iXPos - 24, iYPos - 124 + iTournamentYOffset );
				}				
			}
			else
			{
				iDrawResultX = 7;
				//무승부
				if( m_pIconDraw )
				{
					m_pIconDraw->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconDraw->Render( ( iXPos - 51 ) + iDrawResultX, iYPos - 89 + iTournamentYOffset );
				}
				if( m_pGameDraw )
				{
					m_pGameDraw->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameDraw->Render( ( iXPos - 24 ) + iDrawResultX, iYPos - 124 + iTournamentYOffset );
				}
			}

			if( m_pFinalRoundText )
			{
				m_pFinalRoundText->SetAlpha( (BYTE)m_iAlphaRate );
				m_pFinalRoundText->Render( iXPos - 24, iYPos - 124 + iTournamentYOffset );
			}

			if( g_BattleRoomMgr.IsTournamentModeType() && 0 < m_dwContributeTotalBluePoint && 0 < m_dwContributeTotalRedPoint )
			{
				m_pTournamentResultBack->SetAlpha( (BYTE)m_iAlphaRate );
				m_pTournamentResultBack->Render( iXPos - 264, iYPos - 77 );

				ioComplexStringPrinter kPrinter;
				kPrinter.SetTextStyle( TS_NORMAL );
				kPrinter.SetBkColor( 0, 0, 0 );
				kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
				kPrinter.AddTextPiece( FONT_SIZE_13, "팀기여도" );
				
				kPrinter.SetTextStyle( TS_NORMAL );
				kPrinter.SetBkColor( 0, 0, 0 );
				kPrinter.SetTextColor( D3DCOLOR_XRGB( 0, 150, 255 ) );
				kPrinter.AddTextPiece( FONT_SIZE_13, " %d％", m_dwContributeTotalBluePoint );

				kPrinter.SetTextStyle( TS_NORMAL );
				kPrinter.SetBkColor( 0, 0, 0 );
				kPrinter.SetTextColor( D3DCOLOR_XRGB( 255, 180, 0 ) );
				kPrinter.AddTextPiece( FONT_SIZE_13, " VS " );

				kPrinter.SetTextStyle( TS_NORMAL );
				kPrinter.SetBkColor( 0, 0, 0 );
				kPrinter.SetTextColor( D3DCOLOR_XRGB( 255, 60, 0 ) );
				kPrinter.AddTextPiece( FONT_SIZE_13, "%d％", m_dwContributeTotalRedPoint );

				kPrinter.PrintFullText( iXPos + 0, iYPos -72, TAT_CENTER );
				kPrinter.ClearList();
			}
		}
		break;
	case MT_FIRE_TEMPLE:
	case MT_DARK_XMAS:
	case MT_TOWER_DEFENSE:
	case MT_SURVIVAL:
	case MT_FIGHT_CLUB:
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_DOUBLE_CROWN:
	case MT_FACTORY:
	case MT_FLAG_CAPTURE:
	case MT_BATTLE:
		{
			if( m_dwResultType == RESULT_WIN )
			{
				if( m_pIconWin )
				{
					m_pIconWin->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconWin->Render( iXPos - 51, iYPos - 89 + iTournamentYOffset );
				}
				if( m_pGameWin )
				{
					m_pGameWin->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameWin->Render( iXPos - 24, iYPos - 130 + iTournamentYOffset );		
				}
			}
			else if( m_dwResultType == RESULT_LOSE )
			{
				if( m_pIconLose )
				{
					m_pIconLose->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconLose->Render( iXPos - 51, iYPos - 89 + iTournamentYOffset );
				}
				if( m_pGameLose )
				{
					m_pGameLose->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameLose->Render( iXPos - 24, iYPos - 130 + iTournamentYOffset );		
				}
			}
			else
			{
				iDrawResultX = 7;
				//무승부
				if( m_pIconDraw )
				{
					m_pIconDraw->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconDraw->Render( ( iXPos - 51 ) + iDrawResultX, iYPos - 89 + iTournamentYOffset );
				}
				if( m_pGameDraw )
				{
					m_pGameDraw->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameDraw->Render( ( iXPos - 24 ) + iDrawResultX, iYPos - 124 + iTournamentYOffset );
				}
			}

			if( g_BattleRoomMgr.IsTournamentModeType() )
			{
				m_pTournamentResultBack->SetAlpha( (BYTE)m_iAlphaRate );
				m_pTournamentResultBack->Render( iXPos - 264, iYPos - 77 );

				ioComplexStringPrinter kPrinter;
				kPrinter.SetTextStyle( TS_NORMAL );
				kPrinter.SetBkColor( 0, 0, 0 );
				kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
				kPrinter.AddTextPiece( FONT_SIZE_13, "팀기여도" );

				kPrinter.SetTextStyle( TS_NORMAL );
				kPrinter.SetBkColor( 0, 0, 0 );
				kPrinter.SetTextColor( D3DCOLOR_XRGB( 0, 150, 255 ) );
				kPrinter.AddTextPiece( FONT_SIZE_13, " %d%", m_dwContributeTotalBluePoint );

				kPrinter.SetTextStyle( TS_NORMAL );
				kPrinter.SetBkColor( 0, 0, 0 );
				kPrinter.SetTextColor( D3DCOLOR_XRGB( 255, 180, 0 ) );
				kPrinter.AddTextPiece( FONT_SIZE_13, " VS " );

				kPrinter.SetTextStyle( TS_NORMAL );
				kPrinter.SetBkColor( 0, 0, 0 );
				kPrinter.SetTextColor( D3DCOLOR_XRGB( 255, 60, 0 ) );
				kPrinter.AddTextPiece( FONT_SIZE_13, "%d%", m_dwContributeTotalRedPoint );

				kPrinter.PrintFullText( iXPos + 0, iYPos -72, TAT_CENTER );
				kPrinter.ClearList();
			}
		}
		break;
	case MT_BOSS:
		{
			if( m_dwResultType == RESULT_WIN )
			{
				if( m_pIconWin )
				{
					m_pIconWin->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconWin->Render( iXPos - 51, iYPos - 89 );
				}
				if( m_pGameEnd )
				{
					m_pGameEnd->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameEnd->Render( iXPos - 24, iYPos - 130 );		
				}
			}
			else if( m_dwResultType == RESULT_LOSE )
			{
				if( m_pIconLose )
				{
					m_pIconLose->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconLose->Render( iXPos - 51, iYPos - 89 );
				}
				if( m_pGameEnd )
				{
					m_pGameEnd->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameEnd->Render( iXPos - 24, iYPos - 130 );		
				}
			}
			else
			{
				if( m_pIconDraw )
				{
					m_pIconDraw->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconDraw->Render( ( iXPos - 51 ), iYPos - 89 );
				}
				if( m_pGameEnd )
				{
					m_pGameEnd->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameEnd->Render( ( iXPos - 24 ), iYPos - 124 );
				}
			}
		}
		break;
	case MT_GANGSI:
		{
			if( m_dwResultType == RESULT_WIN )
			{
				if( m_pIconWin )
				{
					m_pIconWin->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconWin->Render( iXPos - 51, iYPos - 89 );
				}
				if( m_pGameWin )
				{
					m_pGameWin->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameWin->Render( iXPos - 24, iYPos - 130 );		
				}
			}
			else if( m_dwResultType == RESULT_LOSE )
			{
				if( m_pIconLose )
				{
					m_pIconLose->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconLose->Render( iXPos - 51, iYPos - 89 );
				}
				if( m_pGameLose )
				{
					m_pGameLose->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameLose->Render( iXPos - 24, iYPos - 130 );		
				}
			}
			else
			{
				iDrawResultX = 7;
				//무승부
				if( m_pIconDraw )
				{
					m_pIconDraw->SetAlpha( (BYTE)m_iAlphaRate );
					m_pIconDraw->Render( ( iXPos - 51 ) + iDrawResultX, iYPos - 89 );
				}
				if( m_pGameDraw )
				{
					m_pGameDraw->SetAlpha( (BYTE)m_iAlphaRate );
					m_pGameDraw->Render( ( iXPos - 24 ) + iDrawResultX, iYPos - 124 );
				}
			}
		}
		break;
	case MT_SHUFFLE_BONUS:
		{
			if( m_pShuffleResultEffect )
			{
				m_pShuffleResultEffect->SetAlpha( (BYTE)m_iAlphaRate );
				m_pShuffleResultEffect->Render( iXPos, iYPos - 247 );
			}

			if( m_pShuffleEnd )
			{
				m_pShuffleEnd->SetAlpha( (BYTE)m_iAlphaRate );
				m_pShuffleEnd->Render( iXPos, iYPos - 172 );				
			}

			if( m_pShuffleStarBox )
			{
				m_pShuffleStarBox->SetAlpha( (BYTE)m_iAlphaRate );
				m_pShuffleStarBox->Render( iXPos, iYPos - 105 );
			}

			if( m_pShuffleStarNum )
			{
				m_pShuffleStarNum->SetAlpha( (BYTE)m_iAlphaRate );
				m_pShuffleStarNum->RenderNum( iXPos, iYPos - 43, m_iStarCount, -5.0f );
			}
		}
		break;
	case MT_RAID:
		{
			if( m_pHunterCoinResultEffect )
			{
				m_pHunterCoinResultEffect->SetAlpha( (BYTE)m_iAlphaRate );
				m_pHunterCoinResultEffect->Render( iXPos, iYPos - 247 );
			}

			if( m_pHunterCoinEnd )
			{
				m_pHunterCoinEnd->SetAlpha( (BYTE)m_iAlphaRate );
				m_pHunterCoinEnd->Render( iXPos, iYPos - 172 );				
			}

			if( m_pHunterCoinBox )
			{
				m_pHunterCoinBox->SetAlpha( (BYTE)m_iAlphaRate );
				m_pHunterCoinBox->Render( iXPos, iYPos - 105 );
			}

			if( m_pHunterCoinNum )
			{
				m_pHunterCoinNum->SetAlpha( (BYTE)m_iAlphaRate );
				m_pHunterCoinNum->RenderNum( iXPos, iYPos - 43, m_iHunterCoinCount, -5.0f );
			}
		}
		break;
	}	
}

void BriefingWnd::RenderPvEBriefingStart( int iXPos, int iYPos )
{
	if( m_pPvEModeTitleLine )
	{
		m_pPvEModeTitleLine->SetAlpha( (BYTE)m_iAlphaRate );
		m_pPvEModeTitleLine->Render( iXPos - 215, iYPos - 67 );
	}

	if( m_pPvEModeTitleSub )
	{
		 m_pPvEModeTitleSub->SetAlpha( (BYTE)m_iAlphaRate );
		 m_pPvEModeTitleSub->Render( iXPos - 59, iYPos - 53 );
	}

	// 층 수 표시
	enum
	{
		FLOOR_NUM_SIZE      = 52,
		FLOOR_NUM_GAP       = 13,
		FLOOR_NUM_TEXT_GAP  = 6,
		FLOOR_NUM_TEXT_SIZE = 61,
	};
	int iFloorXPos = iXPos;
	int iFloorNumCount  = GetNumberCount( m_iCurRound );
	int iTotalFloorNum  = ( FLOOR_NUM_SIZE * iFloorNumCount ) - ( ( iFloorNumCount - 1 ) * FLOOR_NUM_GAP ); 
	int iCenterFloorPos = ( ( iTotalFloorNum - FLOOR_NUM_TEXT_GAP ) + FLOOR_NUM_TEXT_SIZE ) / 2;

	iFloorXPos -= iCenterFloorPos;
	if( m_pPvETitleFloorNum )
	{
		m_pPvETitleFloorNum->SetAlpha( (BYTE)m_iAlphaRate );
		m_pPvETitleFloorNum->RenderNum( iFloorXPos, iYPos - 137, m_iCurRound, -FLOOR_NUM_GAP );
		iFloorXPos += ( iTotalFloorNum - FLOOR_NUM_TEXT_GAP );
	}
	if( m_pPvETitleFloor )
	{
		m_pPvETitleFloor->SetAlpha( (BYTE)m_iAlphaRate );
		m_pPvETitleFloor->Render( iFloorXPos, iYPos - 120 );
	}
}

void BriefingWnd::RenderPvEBriefingFinalResult( int iXPos, int iYPos )
{
	if( m_pHelpBar && !g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) )
	{
		m_pHelpBar->SetAlpha( (BYTE)m_iAlphaRate );
		m_pHelpBar->Render( iXPos - 264, iYPos - 101 );
	}

	if( m_pPvEModeTitleLine )
	{
		m_pPvEModeTitleLine->SetAlpha( (BYTE)m_iAlphaRate );
		m_pPvEModeTitleLine->Render( iXPos - 215, iYPos - 153 );
	}

	// 층 수 표시
	enum
	{
		FLOOR_ACHIEVE_SIZE  = 105,
		FLOOR_NUM_SIZE      = 52,
		FLOOR_NUM_GAP       = 13,
		FLOOR_NUM_TEXT_GAP  = 6,
		FLOOR_NUM_TEXT_SIZE = 61,
	};
	int iFloorXPos = iXPos;
	int iFloorNumCount  = GetNumberCount( m_iCurRound );
	int iTotalFloorNum  = ( FLOOR_NUM_SIZE * iFloorNumCount ) - ( ( iFloorNumCount - 1 ) * FLOOR_NUM_GAP ); 
	int iCenterFloorPos = ( FLOOR_ACHIEVE_SIZE + ( iTotalFloorNum - FLOOR_NUM_TEXT_GAP ) + FLOOR_NUM_TEXT_SIZE ) / 2;

	iFloorXPos -= iCenterFloorPos;
	if( m_pPvETitleWin && m_pPvETitleLose )
	{
		if( m_dwResultType == RESULT_WIN )
		{
			m_pPvETitleWin->SetAlpha( (BYTE)m_iAlphaRate );
			m_pPvETitleWin->Render( iFloorXPos, iYPos - 184 );
		}
		else
		{
			m_pPvETitleLose->SetAlpha( (BYTE)m_iAlphaRate );
			m_pPvETitleLose->Render( iFloorXPos, iYPos - 184 );
		}
		iFloorXPos += FLOOR_ACHIEVE_SIZE;
	}
	if( m_pPvETitleFloorNum )
	{
		m_pPvETitleFloorNum->SetAlpha( (BYTE)m_iAlphaRate );
		m_pPvETitleFloorNum->RenderNum( iFloorXPos, iYPos - 221, m_iCurRound, -FLOOR_NUM_GAP );
		iFloorXPos += ( iTotalFloorNum - FLOOR_NUM_TEXT_GAP );
	}
	if( m_pPvETitleFloor )
	{
		m_pPvETitleFloor->SetAlpha( (BYTE)m_iAlphaRate );
		m_pPvETitleFloor->Render( iFloorXPos, iYPos - 204 );
	}
}
