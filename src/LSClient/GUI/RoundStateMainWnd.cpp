

#include "StdAfx.h"

#include "../ioHouseMode.h"
#include "../ioGuildMarkMgr.h"
#include "../ioTournamentManager.h"
#include "../ioTournamentTeamManager.h"
#include "../ioShuffleRoomMgr.h"
#include "../ioRankBattleManager.h"
#include "../ioLadderTeamMgr.h"

#include "RoundStateMainWnd.h"
#include "PlazaMainWnd.h"
#include "UserInfoToolTip.h"
#include "ioSuccessionModeWnd.h"

// 2019-07-03
#include "SoldierSelectWnd.h"
#include "UISoldierSelectWnd.h"
#include "MyInventoryWnd.h"

//////////////////////////////////////////////////////////////////////////
RoundStateUserWnd::RoundStateUserWnd()
{
	m_pMyTeamFrm= NULL;
	m_pGuildMarkFrm = NULL;

	m_pBlueDark = NULL;
	m_pBlueLight= NULL;
	m_pBlueOver = NULL;
	m_pRedDark  = NULL;
	m_pRedLight = NULL;
	m_pRedOver  = NULL;

	m_pPCRoomBlue = NULL;
	m_pPCRoomRed  = NULL;
	m_pGuildBlue  = NULL;
	m_pGuildRed   = NULL;
	m_pFriendBlue = NULL;
	m_pFriendRed  = NULL;
	m_pVictoriesBlue1 = NULL;
	m_pVictoriesRed1  = NULL;
	m_pVictoriesBlue2 = NULL;
	m_pVictoriesRed2  = NULL;
	m_pVictoriesNumBlue = NULL;
	m_pVictoriesNumRed  = NULL;
	m_pModeIconBack = NULL;
	m_pOrangeMan = NULL;
	m_pGrayMan = NULL;

	m_pBlueVert	= NULL;
	m_pRedVert	= NULL;
	m_pMonsterModeTitle = NULL;
	m_pSurvivalModeTitle= NULL;
	m_pBossModeTitle = NULL;
	m_pGangsiModeTitle1 = NULL;
	m_pGangsiModeTitle2 = NULL;
	m_pFightModeTitle   = NULL;
	m_pShuffleBonusModeTitle = NULL;
	m_pRaidModeTitle = NULL;
	m_pBlueNumber = NULL;
	m_pRedNumber = NULL;
	m_pBlueFloor = NULL;
	m_pRedRank = NULL;
	m_pBlueRank = NULL;
	m_pBluePer = NULL;
	m_pRedPer  = NULL;
	m_pBluePoint = NULL;
	m_pRedPoint= NULL;

	m_pTowerDefTitle = NULL;
	m_pSnowTownTitle = NULL;
	m_pFireTempleTitle = NULL;
	m_pFactoryTitle = NULL;

	m_pTowerDefDiff[TOWERDEF_EASY] = NULL;
	m_pTowerDefDiff[TOWERDEF_NORMAL] = NULL;
	m_pTowerDefDiff[TOWERDEF_HARD] = NULL;
	m_pTowerDefDiff[TOWERDEF_MASTER] = NULL;

	m_pGoldMonsterCoin  = NULL;
	m_pMonsterCoin		= NULL;
	m_pRaidCoin			= NULL;
	m_pCoinNumber		= NULL;
	m_pCoinUnlimited	= NULL;

	m_pCurrentDark = NULL;
	m_pCurrentLight= NULL;
	m_pCurrentOver = NULL;
	m_pCurrentNumber = NULL;
	m_pCurrentNumberText  = NULL;

	m_pBlueTeam = NULL;
	m_pRedTeam  = NULL;

	m_pSuccessionTitle = NULL;
	
	m_pPlayStage= NULL;
	m_eWindowTeam = TEAM_NONE;
	m_iMaxSlot = m_iCurrentFloor = m_iCurrentRank = m_iCurrentScore = m_iCurrentLiveUser = 0;

	m_iMaxLine = m_iLinePlayer = m_iLinePlayerGap = m_iLineStartX = 0;
	m_iLineStartY = m_iLineWidth = m_iLineHeight = m_iNameLimit = 0;
	m_MouseOverIndex = -1;

	m_pSuccessionFrm = NULL;
}

RoundStateUserWnd::~RoundStateUserWnd()
{
	SAFEDELETE( m_pMyTeamFrm );
	SAFEDELETE( m_pGuildMarkFrm );

    SAFEDELETE( m_pBlueDark );
	SAFEDELETE( m_pBlueLight );
	SAFEDELETE( m_pBlueOver );
	SAFEDELETE( m_pRedDark );
	SAFEDELETE( m_pRedLight );
	SAFEDELETE( m_pRedOver );

	SAFEDELETE( m_pPCRoomBlue );
	SAFEDELETE( m_pPCRoomRed );
	SAFEDELETE( m_pGuildBlue );
	SAFEDELETE( m_pGuildRed );
	SAFEDELETE( m_pFriendBlue );
	SAFEDELETE( m_pFriendRed );
	SAFEDELETE( m_pVictoriesBlue1 );
	SAFEDELETE( m_pVictoriesRed1 );
	SAFEDELETE( m_pVictoriesBlue2 );
	SAFEDELETE( m_pVictoriesRed2 );
	SAFEDELETE( m_pVictoriesNumBlue );
	SAFEDELETE( m_pVictoriesNumRed );
	SAFEDELETE( m_pModeIconBack );
	SAFEDELETE( m_pOrangeMan );
	SAFEDELETE( m_pGrayMan );

	SAFEDELETE( m_pBlueVert );
	SAFEDELETE( m_pRedVert );
	SAFEDELETE( m_pMonsterModeTitle );
	SAFEDELETE( m_pSurvivalModeTitle );
	SAFEDELETE( m_pBossModeTitle );
	SAFEDELETE( m_pGangsiModeTitle1 );
	SAFEDELETE( m_pGangsiModeTitle2 );
	SAFEDELETE( m_pFightModeTitle );
	SAFEDELETE( m_pShuffleBonusModeTitle );
	SAFEDELETE( m_pRaidModeTitle );
	SAFEDELETE( m_pBlueNumber );
	SAFEDELETE( m_pRedNumber );
	SAFEDELETE( m_pBlueFloor );
	SAFEDELETE( m_pRedRank );
	SAFEDELETE( m_pBlueRank );
	SAFEDELETE( m_pBluePer );
	SAFEDELETE( m_pRedPer );
	SAFEDELETE( m_pBluePoint );
	SAFEDELETE( m_pRedPoint );

	SAFEDELETE(m_pTowerDefTitle);
	SAFEDELETE(m_pSnowTownTitle);
	SAFEDELETE(m_pFireTempleTitle);
	SAFEDELETE(m_pFactoryTitle);
	SAFEDELETE(m_pTowerDefDiff[TOWERDEF_EASY]);
	SAFEDELETE(m_pTowerDefDiff[TOWERDEF_NORMAL]);
	SAFEDELETE(m_pTowerDefDiff[TOWERDEF_HARD]);
	SAFEDELETE(m_pTowerDefDiff[TOWERDEF_MASTER]);

	SAFEDELETE( m_pGoldMonsterCoin );
	SAFEDELETE( m_pMonsterCoin );
	SAFEDELETE( m_pRaidCoin );
	SAFEDELETE( m_pCoinNumber );
	SAFEDELETE( m_pCoinUnlimited );

	SAFEDELETE( m_pBlueTeam );
	SAFEDELETE( m_pRedTeam );
	
	SAFEDELETE( m_pSuccessionTitle );
	SAFEDELETE( m_pSuccessionFrm );
}

void RoundStateUserWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "MyTeamFrm" )
	{
		SAFEDELETE( m_pMyTeamFrm );
		m_pMyTeamFrm = pFrame;
	}
	else if( szType == "GuildMarkFrm" )
	{
		SAFEDELETE( m_pGuildMarkFrm );
		m_pGuildMarkFrm = pFrame;
	}
	else if( szType == "SuccessionFrm" )
	{
		SAFEDELETE( m_pSuccessionFrm );
		m_pSuccessionFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void RoundStateUserWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueDark" )
	{
		SAFEDELETE( m_pBlueDark );
		m_pBlueDark = pImage;
	}
	else if( szType == "BlueLight" )
	{
		SAFEDELETE( m_pBlueLight );
		m_pBlueLight = pImage;
	}
	else if( szType == "BlueOver" )
	{
		SAFEDELETE( m_pBlueOver );
		m_pBlueOver = pImage;
	}
	else if( szType == "RedDark" )
	{
		SAFEDELETE( m_pRedDark );
		m_pRedDark = pImage;
	}
	else if( szType == "RedLight" )
	{
		SAFEDELETE( m_pRedLight );
		m_pRedLight = pImage;
	}
	else if( szType == "RedOver" )
	{
		SAFEDELETE( m_pRedOver );
		m_pRedOver = pImage;
	}
	else if( szType == "PCRoomBlue" )
	{
		SAFEDELETE( m_pPCRoomBlue );
		m_pPCRoomBlue = pImage;
	}
	else if( szType == "PCRoomRed" )
	{
		SAFEDELETE( m_pPCRoomRed );
		m_pPCRoomRed = pImage;
	}
	else if( szType == "GuildBlue" )
	{
		SAFEDELETE( m_pGuildBlue );
		m_pGuildBlue = pImage;
	}
	else if( szType == "GuildRed" )
	{
		SAFEDELETE( m_pGuildRed );
		m_pGuildRed = pImage;
	}
	else if( szType == "FriendBlue" )
	{
		SAFEDELETE( m_pFriendBlue );
		m_pFriendBlue = pImage;
	}
	else if( szType == "FriendRed" )
	{
		SAFEDELETE( m_pFriendRed );
		m_pFriendRed = pImage;
	}
	else if( szType == "VictoriesBlue1" )
	{
		SAFEDELETE( m_pVictoriesBlue1 );
		m_pVictoriesBlue1 = pImage;
	}
	else if( szType == "VictoriesRed1" )
	{
		SAFEDELETE( m_pVictoriesRed1 );
		m_pVictoriesRed1 = pImage;
	}
	else if( szType == "VictoriesBlue2" )
	{
		SAFEDELETE( m_pVictoriesBlue2 );
		m_pVictoriesBlue2 = pImage;
	}
	else if( szType == "VictoriesRed2" )
	{
		SAFEDELETE( m_pVictoriesRed2 );
		m_pVictoriesRed2 = pImage;
	}
	else if( szType == "VictoriesNumBlue" )
	{
		SAFEDELETE( m_pVictoriesNumBlue );
		m_pVictoriesNumBlue = pImage;
	}
	else if( szType == "VictoriesNumRed" )
	{
		SAFEDELETE( m_pVictoriesNumRed );
		m_pVictoriesNumRed = pImage;
	}
	else if( szType == "ModeIconBack" )
	{
		SAFEDELETE( m_pModeIconBack );
		m_pModeIconBack = pImage;
	}
	else if( szType == "OrangeMan" )
	{
		SAFEDELETE( m_pOrangeMan );
		m_pOrangeMan = pImage;
	}
	else if( szType == "GrayMan" )
	{
		SAFEDELETE( m_pGrayMan );
		m_pGrayMan = pImage;
	}
	else if( szType == "BlueVert" )
	{
		SAFEDELETE( m_pBlueVert );
		m_pBlueVert = pImage;
	}	
	else if( szType == "RedVert" )
	{
		SAFEDELETE( m_pRedVert );
		m_pRedVert = pImage;
	}	
	else if( szType == "MonsterModeTitle" )
	{
		SAFEDELETE( m_pMonsterModeTitle );
		m_pMonsterModeTitle = pImage;
	}	
	else if( szType == "SurvivalModeTitle" )
	{
		SAFEDELETE( m_pSurvivalModeTitle );
		m_pSurvivalModeTitle = pImage;
	}	
	else if( szType == "BossModeTitle" )
	{
		SAFEDELETE( m_pBossModeTitle );
		m_pBossModeTitle = pImage;
	}	
	else if( szType == "GangsiModeTitle1" )
	{
		SAFEDELETE( m_pGangsiModeTitle1 );
		m_pGangsiModeTitle1 = pImage;
	}	
	else if( szType == "GangsiModeTitle2" )
	{
		SAFEDELETE( m_pGangsiModeTitle2 );
		m_pGangsiModeTitle2 = pImage;
	}	
	else if( szType == "FightModeTitle" )
	{
		SAFEDELETE( m_pFightModeTitle );
		m_pFightModeTitle = pImage;
	}
	else if( szType == "ShuffleBonusModeTitle" )
	{
		SAFEDELETE( m_pShuffleBonusModeTitle );
		m_pShuffleBonusModeTitle = pImage;
	}
	else if( szType == "RaidModeTitle" )
	{
		SAFEDELETE( m_pRaidModeTitle );
		m_pRaidModeTitle = pImage;
	}
	else if( szType == "BlueNumber" )
	{
		SAFEDELETE( m_pBlueNumber );
		m_pBlueNumber = pImage;
	}	
	else if( szType == "RedNumber" )
	{
		SAFEDELETE( m_pRedNumber );
		m_pRedNumber = pImage;
	}	
	else if( szType == "BlueFloor" )
	{
		SAFEDELETE( m_pBlueFloor );
		m_pBlueFloor = pImage;
	}	
	else if( szType == "RedRank" )
	{
		SAFEDELETE( m_pRedRank );
		m_pRedRank = pImage;
	}
	else if( szType == "BlueRank" )
	{
		SAFEDELETE( m_pBlueRank );
		m_pBlueRank = pImage;
	}	
	else if( szType == "BluePer" )
	{
		SAFEDELETE( m_pBluePer );
		m_pBluePer = pImage;
	}	
	else if( szType == "RedPer" )
	{
		SAFEDELETE( m_pRedPer );
		m_pRedPer = pImage;
	}	
	else if( szType == "BluePoint" )
	{
		SAFEDELETE( m_pBluePoint );
		m_pBluePoint = pImage;
	}	
	else if( szType == "RedPoint" )
	{
		SAFEDELETE( m_pRedPoint );
		m_pRedPoint = pImage;
	}
	else if( szType == "TowerDefTitle" )
	{
		SAFEDELETE( m_pTowerDefTitle );
		m_pTowerDefTitle = pImage;
	}
	else if( szType == "FireTempleTitle" )
	{
		SAFEDELETE( m_pFireTempleTitle );
		m_pFireTempleTitle = pImage;
	}
	else if( szType == "SnowTownTitle" )
	{
		SAFEDELETE( m_pSnowTownTitle );
		m_pSnowTownTitle = pImage;
	}
	else if( szType == "FactoryTitle" )
	{
		SAFEDELETE( m_pFactoryTitle );
		m_pFactoryTitle = pImage;
	}
	else if( szType == "TowerDefEasy" )
	{
		SAFEDELETE( m_pTowerDefDiff[TOWERDEF_EASY] );
		m_pTowerDefDiff[TOWERDEF_EASY] = pImage;
	}
	else if( szType == "TowerDefNoraml" )
	{
		SAFEDELETE( m_pTowerDefDiff[TOWERDEF_NORMAL] );
		m_pTowerDefDiff[TOWERDEF_NORMAL] = pImage;
	}
	else if( szType == "TowerDefHard" )
	{
		SAFEDELETE( m_pTowerDefDiff[TOWERDEF_HARD] );
		m_pTowerDefDiff[TOWERDEF_HARD] = pImage;
	}
	else if( szType == "TowerDefMaster" )
	{
		SAFEDELETE( m_pTowerDefDiff[TOWERDEF_MASTER] );
		m_pTowerDefDiff[TOWERDEF_MASTER] = pImage;
	}
	else if( szType == "GoldMonsterCoin" )
	{
		SAFEDELETE( m_pGoldMonsterCoin );
		m_pGoldMonsterCoin = pImage;
	}	
	else if( szType == "MonsterCoin" )
	{
		SAFEDELETE( m_pMonsterCoin );
		m_pMonsterCoin = pImage;
	}	
	else if( szType == "RaidCoin" )
	{
		SAFEDELETE( m_pRaidCoin );
		m_pRaidCoin = pImage;
	}	
	else if( szType == "CoinNumber" )
	{
		SAFEDELETE( m_pCoinNumber );
		m_pCoinNumber = pImage;
	}	
	else if( szType == "CoinUnlimited" )
	{
		SAFEDELETE( m_pCoinUnlimited );
		m_pCoinUnlimited = pImage;
	}
	else if( szType == "BlueTeam" )
	{
		SAFEDELETE( m_pBlueTeam );
		m_pBlueTeam = pImage;
	}
	else if( szType == "RedTeam" )
	{
		SAFEDELETE( m_pRedTeam );
		m_pRedTeam = pImage;
	}
	else if( szType == "SuccessionTitle" )
	{
		SAFEDELETE( m_pSuccessionTitle );
		m_pSuccessionTitle = pImage;
	}
	else 
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void RoundStateUserWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iMaxLine		= xElement.GetIntAttribute_e( "MaxLine" );
	m_iLinePlayer	= xElement.GetIntAttribute_e( "LinePlayer" );
	m_iLinePlayerGap= xElement.GetIntAttribute_e( "LinePlayerGap" );
	m_iLineStartX	= xElement.GetIntAttribute_e( "LineStartX" );
	m_iLineStartY	= xElement.GetIntAttribute_e( "LineStartY" );
	m_iLineWidth	= xElement.GetIntAttribute_e( "LineWidth" );
	m_iLineHeight	= xElement.GetIntAttribute_e( "LineHeight" );
	m_iNameLimit	= xElement.GetIntAttribute_e( "NameLimit" );
}

bool RoundStateUserWnd::IsMyTeam()
{
	for(int i = 0;i < (int)m_RoundInfoList.size();i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];
		if( rkInfo.m_bOwner )
			return true;
	}
	return false;
}

bool RoundStateUserWnd::IsDieUser( const ioHashString &szName )
{
	if( !m_pPlayStage ) return true;

	ioBaseChar *pUser = m_pPlayStage->GetBaseChar( szName );
	if( !pUser ) return true;

	if( pUser->GetState() == CS_DIE || 
		pUser->GetState() == CS_VIEW || 
		pUser->GetState() == CS_LOADING || 
		pUser->IsPrisonerMode() )
		return true;

	return false;
}

TeamType RoundStateUserWnd::GetUserTeam( int iIndex )
{
	if( !COMPARE( iIndex, 0, (int)m_RoundInfoList.size() ) ) return TEAM_NONE;

	return m_RoundInfoList[iIndex].m_Team;
}

void RoundStateUserWnd::SetWindowTeam( TeamType eTeam )
{
	m_eWindowTeam = eTeam; 
}

void RoundStateUserWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void RoundStateUserWnd::AddRoundInfo( const RoundInfo &rkInfo )
{
	m_RoundInfoList.push_back( rkInfo );
}

void RoundStateUserWnd::ClearRoundInfo()
{
	m_RoundInfoList.clear();
}

void RoundStateUserWnd::SetTournamentTeamName( ioHashString szName )
{
	m_szTournamentTeamName = szName;
}

void RoundStateUserWnd::SetPlazaScore( int iMaxSlot )
{
	m_iMaxSlot = iMaxSlot;
}

void RoundStateUserWnd::SetHeadequartersScore( int iMaxSlot )
{
	m_iMaxSlot = iMaxSlot;
}

void RoundStateUserWnd::SetSingleScore( int iMaxSlot )
{
	m_iMaxSlot = iMaxSlot;
	m_iCurrentRank = 0;
	m_iNameLimit   = 113;

	for(int i = 0;i < (int)m_RoundInfoList.size();i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];
		if( rkInfo.m_bOwner )
		{
			m_iCurrentRank = i + 1;
			break;
		}
	}

	if( m_iCurrentRank == 0 )
		m_iCurrentRank = m_RoundInfoList.size();
}

void RoundStateUserWnd::SetGangsiScore( int iMaxSlot )
{
	m_iMaxSlot = iMaxSlot;
	m_iCurrentRank = 0;
	m_iNameLimit   = 113;
	m_iCurrentLiveUser = 0;
	for(int i = 0;i < (int)m_RoundInfoList.size();i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];
		if( rkInfo.m_Team == TEAM_RED )
			m_iCurrentLiveUser++;
	}
}

void RoundStateUserWnd::SetDungeonsScore( int iMaxSlot, int iCurrentFloor )
{
	m_iMaxSlot = iMaxSlot;
	m_iCurrentFloor = iCurrentFloor;
}

void RoundStateUserWnd::SetTeamScore( int iMaxSlot, int iScore )
{
	m_iMaxSlot = iMaxSlot;
	m_iCurrentScore = iScore;

	if( g_LadderTeamMgr.IsLadderTeam() )
		m_iNameLimit = 113;
}

void RoundStateUserWnd::iwm_create()
{
	m_PingStep.Create();
}

void RoundStateUserWnd::iwm_show()
{
	if( m_eWindowTeam == TEAM_BLUE )
	{
		m_pCurrentDark = m_pBlueDark;
		m_pCurrentLight= m_pBlueLight;
		m_pCurrentOver = m_pBlueOver;
		m_pCurrentNumber = m_pBlueNumber;
		if( ioPlayMode::GetModeType() == MT_TEAM_SURVIVAL || ioPlayMode::GetModeType() == MT_DOUBLE_CROWN || ioPlayMode::GetModeType() == MT_TEAM_SURVIVAL_AI )
		{
			m_pCurrentNumberText = m_pBluePer;
		}
		else
			m_pCurrentNumberText = m_pBluePoint;
	}
	else
	{
		m_pCurrentDark = m_pRedDark;
		m_pCurrentLight= m_pRedLight;
		m_pCurrentOver = m_pRedOver;
		m_pCurrentNumber = m_pRedNumber;

		if( ioPlayMode::GetModeType() == MT_TEAM_SURVIVAL || ioPlayMode::GetModeType() == MT_DOUBLE_CROWN || ioPlayMode::GetModeType() == MT_TEAM_SURVIVAL_AI )
			m_pCurrentNumberText = m_pRedPer;
		else
			m_pCurrentNumberText = m_pRedPoint;
	}
}

void RoundStateUserWnd::iwm_hide()
{
}

void RoundStateUserWnd::ProcessSiteClick()
{
	m_MouseOverIndex = -1;

	if( !g_App.IsVisibleMouse() || g_GUIMgr.GetPreOverWnd() != this )
		return;

	int iXPos, iYPos;
	iXPos = GetDerivedPosX() + m_iLineStartX;
	iYPos = GetDerivedPosY() + m_iLineStartY;

	int iUserSize = m_RoundInfoList.size();
	ioLSMouse *pMouse = g_App.GetMouse();
	for(int i = 0;i < iUserSize;i++)
	{
		if( m_RoundInfoList[i].m_bNPC )
		{
			if( 0 == ( i + 1 ) % m_iLinePlayer )
			{
				iXPos += m_iLinePlayerGap;
				iYPos = GetDerivedPosY() + m_iLineStartY;
			}
			else
			{
				iYPos += m_iLineHeight;
			}
			continue;
		}

		RECT rcNameRect;
		rcNameRect.left   = iXPos;
		rcNameRect.top    = iYPos;
		rcNameRect.right  = rcNameRect.left + m_iLineWidth;
		rcNameRect.bottom = rcNameRect.top + m_iLineHeight;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_MouseOverIndex = i;
			if( pMouse->IsLBtnUp() )
			{
				g_App.OpenMannerWnd( m_RoundInfoList[i].m_szName.c_str(), true );
			}
			else if( pMouse->IsMouseShowing() )
				pMouse->SetToolTipID( m_RoundInfoList[i].m_szName, UserInfoToolTip::TT_ROOM );
			break;
		}

		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iXPos += m_iLinePlayerGap;
			iYPos = GetDerivedPosY() + m_iLineStartY;
		}
		else
		{
			iYPos += m_iLineHeight;
		}
	}
}

void RoundStateUserWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessSiteClick();
}

void RoundStateUserWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	// 바닥 라인
	switch( ioPlayMode::GetModeType() )
	{
	case MT_GANGSI:
		if( m_pMyTeamFrm && IsMyTeam() )
			m_pMyTeamFrm->Render( iXPos, iYPos );
		OnRenderGangsiBack( iXPos + m_iLineStartX, iYPos + m_iLineStartY );
		break;
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_FACTORY:
		if( m_pMyTeamFrm && IsMyTeam() )
		{
			m_pMyTeamFrm->SetSize(528, 255);
			m_pMyTeamFrm->Render( iXPos, iYPos );
		}

		OnRenderTowerDefLine(iXPos + m_iLineStartX, iYPos + m_iLineStartY );
		break;
	case MT_SUCCESSION:
		if( m_pSuccessionFrm )
			m_pSuccessionFrm->Render( iXPos, iYPos );
		OnRenderBack( iXPos + m_iLineStartX, iYPos + m_iLineStartY );
		break;
	default:
		if( m_pMyTeamFrm && IsMyTeam() )
			m_pMyTeamFrm->Render( iXPos, iYPos );
		OnRenderBack( iXPos + m_iLineStartX, iYPos + m_iLineStartY );
		break;
	}

	// 타이틀 & 유저 정보
	switch( ioPlayMode::GetModeType() )
	{
	case MT_TRAINING:
	case MT_HEADQUARTERS:
	case MT_HOUSE:
		OnRenderPlazaUser( iXPos, iYPos );
		break;
	case MT_MONSTER_SURVIVAL:
	case MT_DUNGEON_A:
		OnRenderMonsterTitle( iXPos, iYPos );
		OnRenderMonsterUser( iXPos, iYPos );				
		break;
	case MT_SURVIVAL:
		OnRenderSurvivalTitle( iXPos, iYPos );
		OnRenderSingleUser( iXPos, iYPos );
		break;
	case MT_SHUFFLE_BONUS:
		OnRenderShuffleBonusTitle( iXPos, iYPos );
		OnRenderShuffleBonusUser( iXPos, iYPos );
		break;
	case MT_FIGHT_CLUB:
		OnRenderFightClubTitle( iXPos, iYPos );
		OnRenderFightClubUser( iXPos, iYPos );
		break;
	case MT_BOSS:
		OnRenderBossTitle( iXPos, iYPos );
		OnRenderSingleUser( iXPos, iYPos );
		break;
	case MT_TOWER_DEFENSE:  //TODO tf TAB GUI IN PLAYSTAGE
		OnRenderTowerDefTitle(iXPos, iYPos);
		OnRenderTowerDefUser(iXPos, iYPos);
		break;
	case MT_FIRE_TEMPLE:
		OnRenderFireTempleTitle(iXPos, iYPos);
		OnRenderTowerDefUser(iXPos, iYPos);
		break;
	case MT_DARK_XMAS:
		OnRenderSnowTownTitle(iXPos, iYPos);
		OnRenderTowerDefUser(iXPos, iYPos);
		break;
	case MT_SYMBOL:
	case MT_UNDERWEAR:
	case MT_CBT:
	case MT_CATCH:
	case MT_KING:
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_BATTLE:
	case MT_HEROMATCH:
	case MT_CATCH_RUNNINGMAN:
	case MT_FLAG_CAPTURE:
	case MT_ARENA:
	case MT_FARMING:
		OnRenderTeamTitle( iXPos, iYPos );
		OnRenderTeamUser( iXPos, iYPos );
		break;	
	case MT_FOOTBALL:
		OnRenderTeamTitle( iXPos, iYPos );
		OnRenderSoccerUser( iXPos, iYPos );
		break;	
	case MT_GANGSI:
		OnRenderGangsiTitle( iXPos, iYPos );
		OnRenderSingleUser( iXPos, iYPos );
		break;
	case MT_DOUBLE_CROWN:
		OnRenderTeamTitle( iXPos, iYPos, false, false );
		OnRenderTeamUser( iXPos, iYPos );
		break;
	case MT_FACTORY:
		OnRenderFactoryTitle(iXPos, iYPos);
		OnRenderTowerDefUser(iXPos, iYPos);
		break;
	case MT_RAID:
		OnRenderRaidTitle( iXPos, iYPos );
		OnRenderRaidUser( iXPos, iYPos );
		break;
	case MT_SUCCESSION:
		OnRenderSuccessionTitle( iXPos, iYPos );
		OnRenderSuccessionUser( iXPos, iYPos );
		break;
	}
}

void RoundStateUserWnd::OnRenderBack( int iXPos, int iYPos )
{
	if( !m_pCurrentDark || !m_pCurrentLight ) return;

	int iRealXPos = iXPos;
	int iRealYPos = iYPos;
	for(int i = 0;i < m_iMaxLine;i++)
	{
		if( i%2 == 0 )
			m_pCurrentDark->Render( iRealXPos, iRealYPos );
		else
			m_pCurrentLight->Render( iRealXPos, iRealYPos );

		if( i >= (int)m_RoundInfoList.size() )
		{
			if( i < m_iMaxSlot )
			{
				if( m_pOrangeMan )
					m_pOrangeMan->Render( iRealXPos + 4, iRealYPos + 2 );
			}
			else
			{
				if( m_pGrayMan )
					m_pGrayMan->Render( iRealXPos + 4, iRealYPos + 2 );
			}
		}

		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}


void RoundStateUserWnd::OnRenderTowerDefLine( int iXPos, int iYPos )
{
	if( !m_pBlueDark || !m_pBlueLight ) return;

	int iRealXPos = iXPos;
	int iRealYPos = iYPos;

	m_pBlueDark->SetSize(514, 21);
	m_pBlueLight->SetSize(514, 21);

	for(int i = 0;i < m_iMaxLine/2;i++)
	{
		if( i%2 == 0 )
			m_pBlueDark->Render( iRealXPos, iRealYPos );
		else
			m_pBlueLight->Render( iRealXPos, iRealYPos );

		iRealYPos += m_iLineHeight;
	}
}


void RoundStateUserWnd::OnRenderGangsiBack( int iXPos, int iYPos )
{
	if( !m_pCurrentDark || !m_pCurrentLight || !m_pBlueDark || !m_pBlueLight ) return;

	int iRealXPos = iXPos;
	int iRealYPos = iYPos;
	for(int i = 0;i < m_iMaxLine;i++)
	{
		if( i%2 == 0 )
		{
			if( GetUserTeam( i ) == TEAM_BLUE )
				m_pBlueDark->Render( iRealXPos, iRealYPos );
			else
				m_pCurrentDark->Render( iRealXPos, iRealYPos );
		}
		else
		{
			if( GetUserTeam( i ) == TEAM_BLUE )
				m_pBlueLight->Render( iRealXPos, iRealYPos );
			else
				m_pCurrentLight->Render( iRealXPos, iRealYPos );
		}

		if( i >= (int)m_RoundInfoList.size() )
		{
			if( i < m_iMaxSlot )
			{
				if( m_pOrangeMan )
					m_pOrangeMan->Render( iRealXPos + 4, iRealYPos + 2 );
			}
			else
			{
				if( m_pGrayMan )
					m_pGrayMan->Render( iRealXPos + 4, iRealYPos + 2 );
			}
		}

		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

int RoundStateUserWnd::OnRenderGradeAndNick( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver )
{
	// 계급
	if( IsDieUser( rkInfo.m_szName ) )    // 죽으면 오버해도 무조건 회색
		g_LevelMgr.RenderGrade( rkInfo.m_szName, iXPos, iYPos, rkInfo.m_iLevel, TEAM_PRIVATE_3 );
	else if( bOver )  
		g_LevelMgr.RenderGrade( rkInfo.m_szName, iXPos, iYPos, rkInfo.m_iLevel, TEAM_PRIVATE_1, UI_RENDER_NORMAL_ADD_COLOR, TCT_DEFAULT_WHITE );
	else
		g_LevelMgr.RenderGrade( rkInfo.m_szName, iXPos, iYPos, rkInfo.m_iLevel, TEAM_PRIVATE_1 );

	// 아이디
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( IsDieUser( rkInfo.m_szName ) )    // 죽으면 오버해도 무조건 회색
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	else if( bOver )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	else if( rkInfo.m_Team == TEAM_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.PrintTextWidthCut( iXPos + 18, iYPos + 2, FONT_SIZE_12, m_iNameLimit, rkInfo.m_szName.c_str() );
	return 18 + g_FontMgr.GetTextWidthCutSize( rkInfo.m_szName.c_str(), TS_NORMAL, FONT_SIZE_12, m_iNameLimit );
}

int RoundStateUserWnd::OnRenderGuildMark( const RoundInfo &rkInfo, int iXPos, int iYPos )
{
	// 길드마크
	DWORD dwGuildIndex, dwGuildMark;
	g_UserInfoMgr.GetGuildInfo( rkInfo.m_szName, dwGuildIndex, dwGuildMark );
	g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, iXPos + 3, iYPos );	
	if( dwGuildIndex == 0 )
		return 0;
	return g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 3;
}

void RoundStateUserWnd::OnRenderBonus( const RoundInfo &rkInfo, int iXPos, int iYPos )
{
	// (PC방,길드,친구,연승)
	if( rkInfo.m_bPCRoomBonus )
	{
		if( rkInfo.m_Team == TEAM_BLUE && m_pPCRoomBlue )
			m_pPCRoomBlue->Render( iXPos, iYPos );
		else if( rkInfo.m_Team == TEAM_RED && m_pPCRoomRed )
			m_pPCRoomRed->Render( iXPos, iYPos );
	}
	else if( rkInfo.m_bGuildBonus )
	{
		if( rkInfo.m_Team == TEAM_BLUE && m_pGuildBlue )
			m_pGuildBlue->Render( iXPos, iYPos );
		else if( rkInfo.m_Team == TEAM_RED && m_pGuildRed )
			m_pGuildRed->Render( iXPos, iYPos );
	}
	else if( rkInfo.m_bFriendBonus )
	{
		if( rkInfo.m_Team == TEAM_BLUE && m_pFriendBlue )
			m_pFriendBlue->Render( iXPos, iYPos );
		else if( rkInfo.m_Team == TEAM_RED && m_pFriendRed )
			m_pFriendRed->Render( iXPos, iYPos );
	}
	else if( rkInfo.m_iVictories > 1 )
	{
		if( rkInfo.m_iVictories < 10 )
		{
			if( rkInfo.m_Team == TEAM_BLUE && m_pVictoriesBlue1 )
			{
				m_pVictoriesBlue1->Render( iXPos, iYPos );
				if( m_pVictoriesNumBlue )
					m_pVictoriesNumBlue->RenderNum( iXPos + 4, iYPos + 4, rkInfo.m_iVictories );
			}
			else if( rkInfo.m_Team == TEAM_RED && m_pVictoriesRed1 )
			{
				m_pVictoriesRed1->Render( iXPos, iYPos );
				if( m_pVictoriesNumRed )
					m_pVictoriesNumRed->RenderNum( iXPos + 4, iYPos + 4, rkInfo.m_iVictories );
			}
		}
		else
		{
			if( rkInfo.m_Team == TEAM_BLUE && m_pVictoriesBlue2 )
			{	
				m_pVictoriesBlue2->Render( iXPos, iYPos );
				if( m_pVictoriesNumBlue )
					m_pVictoriesNumBlue->RenderNum( iXPos + 4, iYPos + 4, min( 99, rkInfo.m_iVictories ) );
			}
			else if( rkInfo.m_Team == TEAM_RED && m_pVictoriesRed2 )
			{
				m_pVictoriesRed2->Render( iXPos, iYPos );
				if( m_pVictoriesNumRed )
					m_pVictoriesNumRed->RenderNum( iXPos + 4, iYPos + 4, min( 99, rkInfo.m_iVictories ) );
			}
		}
	}
}

void RoundStateUserWnd::OnRenderMonsterCoin( const RoundInfo &rkInfo, int iXPos, int iYPos )
{
	if( m_pGoldMonsterCoin && m_pMonsterCoin && m_pCoinNumber && m_pCoinUnlimited )
	{
		m_pCoinNumber->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		// 몬스터 코인 표시
		m_pMonsterCoin->Render( iXPos, iYPos );
		if( rkInfo.m_iMonsterCoin >= 100 )
			m_pCoinUnlimited->Render( iXPos + 4, iYPos + 6 );
		else
			m_pCoinNumber->RenderNum( iXPos + 9, iYPos + 6, rkInfo.m_iMonsterCoin );
		
		// 골드 몬스터 코인 표시
		m_pGoldMonsterCoin->Render( iXPos + 18, iYPos );
		if( rkInfo.m_iGoldMonsterCoin >= 100 )
			m_pCoinUnlimited->Render( iXPos + 22, iYPos + 6 );
		else
			m_pCoinNumber->RenderNum( iXPos + 27, iYPos + 6, rkInfo.m_iGoldMonsterCoin );
	}
}

void RoundStateUserWnd::OnRenderLadderPoint( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( bOver )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	else if( rkInfo.m_Team == TEAM_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( rkInfo.m_Team == TEAM_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_12, STR(1), rkInfo.m_iLadderPoint );
}

void RoundStateUserWnd::OnRenderKillDeath( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( bOver )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	else if( rkInfo.m_Team == TEAM_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( rkInfo.m_Team == TEAM_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_12, STR(1), rkInfo.m_iUniqueTotalKill, rkInfo.m_iUniqueTotalDeath );
}

void RoundStateUserWnd::OnRenderContribute( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver )
{
	if( g_BattleRoomMgr.IsTournamentModeType() )
		return;

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( bOver )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	else if( rkInfo.m_Team == TEAM_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( rkInfo.m_Team == TEAM_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_12, STR(1), rkInfo.m_iContributePer );
}

void RoundStateUserWnd::OnRenderFightWin( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( bOver )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	else if( rkInfo.m_Team == TEAM_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( rkInfo.m_Team == TEAM_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_12, STR(1), rkInfo.m_iFightWin );
}

void RoundStateUserWnd::OnRenderFightLose( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( bOver )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	else if( rkInfo.m_Team == TEAM_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( rkInfo.m_Team == TEAM_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.SetAlignType( TAT_CENTER );
	
	g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_12, STR(1), rkInfo.m_iFightLose );
}

void RoundStateUserWnd::OnRenderGoal( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( bOver )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	else if( rkInfo.m_Team == TEAM_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( rkInfo.m_Team == TEAM_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_12, STR(1), rkInfo.m_iGoal, rkInfo.m_iGoalAssist );
}

void RoundStateUserWnd::OnRenderPing( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver )
{
	bool bServerRelay = false;
	int  iPingStep = 0;
	if( rkInfo.m_bOwner )
	{
		iPingStep = g_GlobalTimer.GetServerDelayMS() / PingStepWnd::PING_MS_VALUE;	
	}			
	else
	{
		iPingStep = abs(  P2PNetwork::GetP2pPingTime( rkInfo.m_szName ) ) / PingStepWnd::PING_MS_VALUE;
		if( P2PNetwork::IsServerRelay( rkInfo.m_szName ) )
			bServerRelay = true;
	}
	m_PingStep.RenderPing( iXPos, iYPos, iPingStep, bServerRelay );
}

void RoundStateUserWnd::OnRenderTowerDefRank(const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver)
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( bOver )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	else 
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );

	g_FontMgr.SetAlignType( TAT_CENTER );

	ioHashString szRank;

	int nRank[4];
	nRank[0] = 3;
	nRank[1] = 3;
	nRank[2] = 3;
	nRank[3] = 3;

	int nPlayer = 0;

	if(m_pPlayStage)
		nPlayer = m_pPlayStage->ContributeRank(rkInfo.m_szName, nRank[0], nRank[1], nRank[2], nRank[3]);

	int nPosX = iXPos;

	for(int i = 0; i < 3; i++)
	{
		szRank.Clear();

		if( nRank[i] == 0) szRank = "S";
		else if( nRank[i] == 1) szRank = "A";
		else if( nRank[i] == 2) szRank = "B";
		else szRank = "C";

		g_FontMgr.PrintText( nPosX, iYPos, FONT_SIZE_12, szRank.c_str());	
		nPosX += 51;
	}
}

void RoundStateUserWnd::OnRenderStarCount( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver )
{
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );

	if( bOver )
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	else if( rkInfo.m_Team == TEAM_BLUE )
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	else if( rkInfo.m_Team == TEAM_RED )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
	g_FontMgr.SetAlignType( TAT_CENTER );

	g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_12, "%d개", rkInfo.m_iStarCnt );
}

void RoundStateUserWnd::OnRenderRaidCoinCount( const RoundInfo &rkInfo, int iXPos, int iYPos )
{
	if( m_pRaidCoin && m_pCoinNumber && m_pCoinUnlimited )
	{
		m_pRaidCoin->Render( iXPos, iYPos );
		if( rkInfo.m_iRaidCoin >= 100 )
			m_pCoinUnlimited->Render( iXPos + 8, iYPos + 5 );
		else if( rkInfo.m_iRaidCoin >= 10 )
			m_pCoinNumber->RenderNum( iXPos + 7, iYPos + 5, rkInfo.m_iRaidCoin );
		else
			m_pCoinNumber->RenderNum( iXPos + 10, iYPos + 5, rkInfo.m_iRaidCoin );
	}
}

void RoundStateUserWnd::OnRenderMonsterTitle( int iXPos, int iYPos )
{
	if( !m_pMonsterModeTitle || !m_pBlueNumber || !m_pModeIconBack || !m_pBlueFloor ) return;

	int iFloorCountGap = 0;
	if( m_iCurrentFloor >= 10 )
		iFloorCountGap = 11;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 232 - iFloorCountGap, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 232 - iFloorCountGap, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pMonsterModeTitle->Render( iXPos + 258 - iFloorCountGap, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..

	// 
	m_pBlueNumber->RenderNum( iXPos + 368, iYPos + 16, m_iCurrentFloor );
	if( m_iCurrentFloor >= 10 )
		iFloorCountGap = 46;
	else
		iFloorCountGap = 23;
	m_pBlueFloor->Render( iXPos + 368 + iFloorCountGap, iYPos + 16 );
}

void RoundStateUserWnd::OnRenderTowerDefTitle( int iXPos, int iYPos )
{
	if( !m_pTowerDefTitle || !m_pTowerDefDiff[TOWERDEF_EASY] || !m_pTowerDefDiff[TOWERDEF_NORMAL] || 
		!m_pTowerDefDiff[TOWERDEF_HARD] || !m_pTowerDefDiff[TOWERDEF_MASTER] || !m_pModeIconBack  ) return;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 177, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_MyInfo.GetSuccessionRevengeMode() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 177, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pTowerDefTitle->Render( iXPos + 203, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..
	m_pTowerDefDiff[m_pPlayStage->GetModeSubNum() - 1]->Render( iXPos + 293, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..
}

void RoundStateUserWnd::OnRenderFireTempleTitle( int iXPos, int iYPos )
{
	if( !m_pFireTempleTitle || !m_pTowerDefDiff[TOWERDEF_EASY] || !m_pTowerDefDiff[TOWERDEF_NORMAL] || 
		!m_pTowerDefDiff[TOWERDEF_HARD] || !m_pTowerDefDiff[TOWERDEF_MASTER] || !m_pModeIconBack  ) return;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 163, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	//TODO 스노우타운 고칠 곳
	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 163, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pFireTempleTitle->Render( iXPos + 189, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..
	m_pTowerDefDiff[m_pPlayStage->GetModeSubNum() - 1]->Render( iXPos + 303, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..
}

void RoundStateUserWnd::OnRenderSnowTownTitle( int iXPos, int iYPos )
{
	if( !m_pSnowTownTitle || !m_pTowerDefDiff[TOWERDEF_EASY] || !m_pTowerDefDiff[TOWERDEF_NORMAL] || 
		!m_pTowerDefDiff[TOWERDEF_HARD] || !m_pTowerDefDiff[TOWERDEF_MASTER] || !m_pModeIconBack  ) return;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 154, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	//TODO 스노우타운 고칠 곳
	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 154, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pSnowTownTitle->Render( iXPos + 180, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..
	m_pTowerDefDiff[m_pPlayStage->GetModeSubNum() - 1]->Render( iXPos + 316, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..
}

void RoundStateUserWnd::OnRenderFactoryTitle( int iXPos, int iYPos )
{
	if( !m_pFactoryTitle || !m_pTowerDefDiff[TOWERDEF_EASY] || !m_pTowerDefDiff[TOWERDEF_NORMAL] || 
		!m_pTowerDefDiff[TOWERDEF_HARD] || !m_pTowerDefDiff[TOWERDEF_MASTER] || !m_pModeIconBack  ) return;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 156, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	//TODO 스노우타운 고칠 곳
	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 156, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pFactoryTitle->Render( iXPos + 182, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..
	m_pTowerDefDiff[m_pPlayStage->GetModeSubNum() - 1]->Render( iXPos + 292, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..
}

void RoundStateUserWnd::OnRenderSurvivalTitle( int iXPos, int iYPos )
{
	if( !m_pSurvivalModeTitle || !m_pRedNumber || !m_pModeIconBack || !m_pRedRank ) return;

	int iRankCountGap = 0;
	if( m_iCurrentRank >= 10 )
		iRankCountGap = 11;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 232 - iRankCountGap, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 232 - iRankCountGap, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pSurvivalModeTitle->Render( iXPos + 258 - iRankCountGap, iYPos + 16 );

	// 
	m_pRedNumber->RenderNum( iXPos + 368, iYPos + 16, m_iCurrentRank );
	if( m_iCurrentRank >= 10 )
		iRankCountGap = 46;
	else
		iRankCountGap = 23;
	m_pRedRank->Render( iXPos + 368 + iRankCountGap, iYPos + 16 );
}

void RoundStateUserWnd::OnRenderFightClubTitle( int iXPos, int iYPos )
{
	if( !m_pFightModeTitle || !m_pModeIconBack ) return;
	
	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 276, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 276, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pFightModeTitle->Render( iXPos + 304, iYPos + 17 );
}

void RoundStateUserWnd::OnRenderShuffleBonusTitle( int iXPos, int iYPos )
{
	if( !m_pShuffleBonusModeTitle || !m_pRedNumber || !m_pModeIconBack || !m_pRedRank ) return;

	int iRankCountGap = 0;
	if( m_iCurrentRank >= 10 )
		iRankCountGap = 11;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 218 - iRankCountGap, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 218 - iRankCountGap, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pShuffleBonusModeTitle->Render( iXPos + 247 - iRankCountGap, iYPos + 16 );

	// 
	m_pRedNumber->RenderNum( iXPos + 388, iYPos + 16, m_iCurrentRank );
	if( m_iCurrentRank >= 10 )
		iRankCountGap = 46;
	else
		iRankCountGap = 23;
	m_pRedRank->Render( iXPos + 388 + iRankCountGap, iYPos + 16 );
}

void RoundStateUserWnd::OnRenderRaidTitle( int iXPos, int iYPos )
{
	if( !m_pRaidModeTitle || !m_pBlueNumber || !m_pModeIconBack || !m_pBlueRank ) return;

	int iRankCountGap = 0;
	if( m_iCurrentRank >= 10 )
		iRankCountGap = 11;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 232 - iRankCountGap, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 232 - iRankCountGap, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pRaidModeTitle->Render( iXPos + 258 - iRankCountGap, iYPos + 16 );

	// 
	m_pBlueNumber->RenderNum( iXPos + 342, iYPos + 16, m_iCurrentRank );
	if( m_iCurrentRank >= 10 )
		iRankCountGap = 46;
	else
		iRankCountGap = 23;
	m_pBlueRank->Render( iXPos + 342 + iRankCountGap, iYPos + 16 );
}

void RoundStateUserWnd::OnRenderSuccessionTitle( int iXPos, int iYPos )
{
	if ( !m_pSuccessionTitle || !m_pModeIconBack )
		return;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 254, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	//TODO 스노우타운 고칠 곳
	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_MyInfo.GetSuccessionRevengeMode() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 254, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pSuccessionTitle->Render( iXPos + 282, iYPos + 16 );  //TODO : 모드 타이틀 이미지임..
}

void RoundStateUserWnd::OnRenderBossTitle( int iXPos, int iYPos )
{
	if( !m_pBossModeTitle || !m_pRedNumber || !m_pModeIconBack || !m_pRedRank ) return;

	int iRankCountGap = 0;
	if( m_iCurrentRank >= 10 )
		iRankCountGap = 11;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 219 - iRankCountGap, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 219 - iRankCountGap, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pBossModeTitle->Render( iXPos + 245 - iRankCountGap, iYPos + 16 );

	// 
	m_pRedNumber->RenderNum( iXPos + 381, iYPos + 16, m_iCurrentRank );
	if( m_iCurrentRank >= 10 )
		iRankCountGap = 46;
	else
		iRankCountGap = 23;
	m_pRedRank->Render( iXPos + 381 + iRankCountGap, iYPos + 16 );
}

void RoundStateUserWnd::OnRenderGangsiTitle( int iXPos, int iYPos )
{
	if( !m_pGangsiModeTitle1 || !m_pGangsiModeTitle2 || !m_pRedNumber || !m_pModeIconBack ) return;

	int iRankCountGap = 0;
	if( m_iCurrentLiveUser >= 10 )
		iRankCountGap = 11;

	m_pModeIconBack->SetScale( 0.54f );
	m_pModeIconBack->Render( iXPos + 177 - iRankCountGap, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	// 모드 아이콘
	ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
	if( pModeIcon )
	{
		pModeIcon->SetScale( 0.54f );
		pModeIcon->Render( iXPos + 177 - iRankCountGap, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// 모드 타이틀
	m_pGangsiModeTitle1->Render( iXPos + 203 - iRankCountGap, iYPos + 16 );

	// 
	m_pRedNumber->RenderNum( iXPos + 365 - iRankCountGap, iYPos + 16, m_iCurrentLiveUser );

	//
	iRankCountGap = 0;
	if( m_iCurrentLiveUser >= 10 )
		iRankCountGap = 12;
	m_pGangsiModeTitle2->Render( iXPos + 388 + iRankCountGap, iYPos + 16 );
}

void RoundStateUserWnd::OnRenderTeamTitle( int iXPos, int iYPos, bool bPointRender, bool bScroreRender )
{
	int iScoreXGap = 12;

	if( m_pCurrentNumber  && m_pCurrentNumberText && bPointRender )
	{	
		m_pCurrentNumber->RenderNum( iXPos + iScoreXGap, iYPos + 16, m_iCurrentScore );
		iScoreXGap += GetNumberCount( m_iCurrentScore ) * 23;

		m_pCurrentNumberText->Render( iXPos + iScoreXGap, iYPos + 16 );
		iScoreXGap += 32;
	}
	else
	{
		if( m_eWindowTeam == TEAM_BLUE )
		{
			if( m_pBlueTeam )
				m_pBlueTeam->Render( iXPos + iScoreXGap, iYPos + 16, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			if( m_pRedTeam )
				m_pRedTeam->Render( iXPos + iScoreXGap, iYPos  + 16, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}

	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

		BattleRoomInfo BattleInfo;
		g_BattleRoomMgr.GetBattleRoomInfo( &BattleInfo );
		g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 30, FONT_SIZE_13, m_szTournamentTeamName.c_str() );


	}
	else if( g_BattleRoomMgr.IsBattleRoom() )
	{
		// 싱글 모드가 아닐 때만 승무패표시
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

		BattleRoomInfo BattleInfo;
		g_BattleRoomMgr.GetBattleRoomInfo( &BattleInfo );
		if( m_eWindowTeam == TEAM_BLUE )
		{
			if( bScroreRender )
			{
				if( BattleInfo.m_iBlueVictories > 1 )
					g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 30, FONT_SIZE_13, STR(1), BattleInfo.m_iBlueWin, BattleInfo.m_iBlueLose, BattleInfo.m_iBlueVictories );
				else
					g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 30, FONT_SIZE_13, STR(2), BattleInfo.m_iBlueWin, BattleInfo.m_iBlueLose );
			}
		}
		else if( m_eWindowTeam == TEAM_RED )
		{
			if( bScroreRender )
			{
				if( BattleInfo.m_iRedVictories > 1 )
					g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 30, FONT_SIZE_13, STR(3), BattleInfo.m_iRedWin, BattleInfo.m_iRedLose, BattleInfo.m_iRedVictories );
				else
					g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 30, FONT_SIZE_13, STR(4), BattleInfo.m_iRedWin, BattleInfo.m_iRedLose );
			}
		}

		if( m_pModeIconBack )
		{
			m_pModeIconBack->SetScale( 0.54f );
			m_pModeIconBack->Render( iXPos + 285, iYPos + 30, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			// 모드 아이콘
			ioUIRenderImage *pModeIcon = g_ModeSelectInfo.GetModeIcon( ioPlayMode::GetModeType(), m_eWindowTeam, g_BattleRoomMgr.IsUseExtraOption() );
			if( pModeIcon )
			{
				pModeIcon->SetScale( 0.54f );
				pModeIcon->Render( iXPos + 285, iYPos + 30, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		if( m_eWindowTeam == TEAM_BLUE )
		{
			LadderTeamInfo LadderInfo = g_LadderTeamMgr.GetLadderBlueTeamInfo();
			if( g_LadderTeamMgr.IsHeroMatchMode() )
			{
				if( LadderInfo.m_szOwnerName == g_MyInfo.GetPublicID() )
				{
					if( g_MyInfo.GetHeroTodayRank() == 0 )
						g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, LadderInfo.m_szTeamName.c_str() );	
					else
						g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, STR(5), g_MyInfo.GetHeroTodayRank(), LadderInfo.m_szTeamName.c_str() );	
				}
				else
				{
					LadderHeroUserInfo kHeroInfo = g_LadderTeamMgr.GetHeroMatchOtherInfo();
					if( kHeroInfo.m_iHeroTodayRank == 0 )
						g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, LadderInfo.m_szTeamName.c_str() );	
					else
						g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, STR(6), kHeroInfo.m_iHeroTodayRank, LadderInfo.m_szTeamName.c_str() );	
				}
			}
			else
			{
				g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, LadderInfo.m_szTeamName.c_str() );		
			}
			if( LadderInfo.m_iVictoriesRecord > 1 )
				g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 30, FONT_SIZE_13, STR(7), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord, LadderInfo.m_iVictoriesRecord );
			else
				g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 30, FONT_SIZE_13, STR(8), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord );

			if( LadderInfo.m_dwGuildIndex != 0 )
			{
				ioGuildData *pGuildData = g_GuildInfo.GetGuildData( LadderInfo.m_dwGuildIndex );
				if( pGuildData )
				{
					g_GuildMarkMgr.RenderMarkSize( 35, 35, pGuildData->GetGuildIndex(), pGuildData->GetGuildMark(), iXPos + 268, iYPos + 13 );
					if( m_pGuildMarkFrm )
						m_pGuildMarkFrm->Render( iXPos, iYPos );
				}
			}
		}
		else if( m_eWindowTeam == TEAM_RED )
		{
			LadderTeamInfo LadderInfo = g_LadderTeamMgr.GetLadderRedTeamInfo();			
			if( g_LadderTeamMgr.IsHeroMatchMode() )
			{
				if( LadderInfo.m_szOwnerName == g_MyInfo.GetPublicID() )
				{
					if( g_MyInfo.GetHeroTodayRank() == 0 )
						g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, LadderInfo.m_szTeamName.c_str() );
					else
						g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, STR(9), g_MyInfo.GetHeroTodayRank(), LadderInfo.m_szTeamName.c_str() );	
				}
				else
				{
					LadderHeroUserInfo kHeroInfo = g_LadderTeamMgr.GetHeroMatchOtherInfo();
					if( kHeroInfo.m_iHeroTodayRank == 0 )
						g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, LadderInfo.m_szTeamName.c_str() );	
					else
						g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, STR(10), kHeroInfo.m_iHeroTodayRank, LadderInfo.m_szTeamName.c_str() );	
				}
			}
			else
			{
				g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 16, FONT_SIZE_13, LadderInfo.m_szTeamName.c_str() );
			}
			if( LadderInfo.m_iVictoriesRecord > 1 )
				g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 30, FONT_SIZE_13, STR(11), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord, LadderInfo.m_iVictoriesRecord );
			else
				g_FontMgr.PrintText( iXPos + iScoreXGap + 4, iYPos + 30, FONT_SIZE_13, STR(12), LadderInfo.m_iWinRecord, LadderInfo.m_iLoseRecord );

			if( LadderInfo.m_dwGuildIndex != 0 )
			{
				ioGuildData *pGuildData = g_GuildInfo.GetGuildData( LadderInfo.m_dwGuildIndex );
				if( pGuildData )
				{
					g_GuildMarkMgr.RenderMarkSize( 35, 35, pGuildData->GetGuildIndex(), pGuildData->GetGuildMark(), iXPos + 268, iYPos + 13 );
					if( m_pGuildMarkFrm )
						m_pGuildMarkFrm->Render( iXPos, iYPos );
				}
			}
		}
	}
}

void RoundStateUserWnd::OnRenderPlazaUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver || !m_pBlueOver || !m_pRedOver || !m_pRedVert ) return;
	
	int i = 0;
	// 
	int iMaxUserLineTitle = m_iMaxLine / m_iLinePlayer;
	for(i = 0;i < iMaxUserLineTitle;i++)
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 12 + i * m_iLinePlayerGap, iYPos + 11, FONT_SIZE_12, STR(1) );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.PrintText( iXPos + 135 + i * m_iLinePlayerGap, iYPos + 11, FONT_SIZE_12, STR(2) );

		if( m_pRedVert && i < iMaxUserLineTitle - 1 )
			m_pRedVert->Render( iXPos + i * m_iLinePlayerGap, iYPos, UI_RENDER_MULTIPLY );
	}

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();
	for(i = 0;i < iSize;i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( rkInfo.m_bOwner )
		{
			if( rkInfo.m_Team == TEAM_BLUE )
				m_pBlueOver->Render( iRealXPos, iRealYPos );
			else if( rkInfo.m_Team == TEAM_RED )
				m_pRedOver->Render( iRealXPos, iRealYPos );
		}
		else if( m_MouseOverIndex == i )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		// 계급 & 아이디
		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 길드 마크
		OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );

		// 핑
		OnRenderPing( rkInfo, iRealXPos + 116, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		//
		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos + m_iLineStartY;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

void RoundStateUserWnd::OnRenderTeamUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver ) return;

	// 
	int i = 0;
	int iMaxUserLineTitle = m_iMaxLine / m_iLinePlayer;
	for(i = 0;i < iMaxUserLineTitle;i++)
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );

		if( m_eWindowTeam == TEAM_BLUE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 12 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(1) );

		g_FontMgr.SetAlignType( TAT_CENTER );
		if( !g_BattleRoomMgr.IsTournamentModeType() )
		{
			g_FontMgr.PrintText( iXPos + 219 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(2) );
		}
		g_FontMgr.PrintText( iXPos + 261 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(3) );
		g_FontMgr.PrintText( iXPos + 294 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(4) );
	}

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();
	for(i = 0;i < iSize;i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( m_MouseOverIndex == i || rkInfo.m_bOwner )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		// 계급 & 아이디
		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 길드 마크
		iWidth += OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );

		// 보너스
		OnRenderBonus( rkInfo, iRealXPos + iWidth + 7, iRealYPos + 3 );

		// 기여도
		OnRenderContribute( rkInfo, iRealXPos + 212, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// KO & Death
		OnRenderKillDeath( rkInfo, iRealXPos + 254, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 핑
		OnRenderPing( rkInfo, iRealXPos + 275, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		//
		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos + m_iLineStartY;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

void RoundStateUserWnd::OnRenderSingleUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver || !m_pBlueOver || !m_pRedOver || !m_pRedVert ) return;

	m_pRedVert->Render( iXPos, iYPos );//중앙선

	// 
	int i = 0;
	int iMaxUserLineTitle = m_iMaxLine / m_iLinePlayer;
	for(i = 0;i < iMaxUserLineTitle;i++)
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 12 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(1) );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.PrintText( iXPos + 219 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(2) );
		g_FontMgr.PrintText( iXPos + 261 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(3) );
		g_FontMgr.PrintText( iXPos + 294 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(4) );
	}

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();
	for(i = 0;i < iSize;i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( rkInfo.m_bOwner )
		{
			if( rkInfo.m_Team == TEAM_BLUE )
				m_pBlueOver->Render( iRealXPos, iRealYPos );
			else if( rkInfo.m_Team == TEAM_RED )
				m_pRedOver->Render( iRealXPos, iRealYPos );
		}
		else if( m_MouseOverIndex == i )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		// 계급 & 아이디
		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 길드 마크
		iWidth += OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );

		// 보너스
		OnRenderBonus( rkInfo, iRealXPos + iWidth + 7, iRealYPos + 3 );

		// 기여도
		OnRenderContribute( rkInfo, iRealXPos + 212, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// KO & Death
		OnRenderKillDeath( rkInfo, iRealXPos + 254, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 핑
		OnRenderPing( rkInfo, iRealXPos + 275, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		//
		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos + m_iLineStartY;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

void RoundStateUserWnd::OnRenderSoccerUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver ) return;

	// 
	int i = 0;
	int iMaxUserLineTitle = m_iMaxLine / m_iLinePlayer;
	for(i = 0;i < iMaxUserLineTitle;i++)
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		if( m_eWindowTeam == TEAM_BLUE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 12 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(1) );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.PrintText( iXPos + 219 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(2) );
		g_FontMgr.PrintText( iXPos + 261 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(3) );
		g_FontMgr.PrintText( iXPos + 294 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(4) );
	}

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();
	for(i = 0;i < iSize;i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( m_MouseOverIndex == i || rkInfo.m_bOwner )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		// 계급 & 아이디
		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 길드 마크
		iWidth += OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );

		// 보너스
		OnRenderBonus( rkInfo, iRealXPos + iWidth + 7, iRealYPos + 3 );

		// 기여도
		OnRenderContribute( rkInfo, iRealXPos + 212, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 골
		OnRenderGoal( rkInfo, iRealXPos + 254, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 핑
		OnRenderPing( rkInfo, iRealXPos + 275, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		//
		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos + m_iLineStartY;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

void RoundStateUserWnd::OnRenderTowerDefUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver || !m_pBlueOver ) return;

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.PrintText( iXPos + 12, iYPos + 62, FONT_SIZE_12, "닉네임" );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.PrintText( iXPos + 352, iYPos + 62, FONT_SIZE_12, "공격" );
	g_FontMgr.PrintText( iXPos + 403, iYPos + 62, FONT_SIZE_12, "방어" );
	g_FontMgr.PrintText( iXPos + 454, iYPos + 62, FONT_SIZE_12, "타임" );
	g_FontMgr.PrintText( iXPos + 505, iYPos + 62, FONT_SIZE_12, "핑" );

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();

	m_pBlueOver->SetSize(514,21);
	m_pCurrentOver->SetSize(514,21);

	for(int i = 0;i < iSize;i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( rkInfo.m_bOwner )
			m_pBlueOver->Render( iRealXPos, iRealYPos );
		else if( m_MouseOverIndex == i )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );
		iWidth += OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );
		OnRenderBonus( rkInfo, iRealXPos + iWidth + 7, iRealYPos + 3 );
		OnRenderTowerDefRank( rkInfo, iRealXPos + 352, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );
		OnRenderPing( rkInfo, iRealXPos + 486, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );
		iRealYPos += m_iLineHeight;
	}
}


void RoundStateUserWnd::OnRenderMonsterUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver || !m_pBlueOver || !m_pRedOver || !m_pBlueVert ) return;

	m_pBlueVert->Render( iXPos, iYPos );//중앙선

	// 
	int i = 0;
	int iMaxUserLineTitle = m_iMaxLine / m_iLinePlayer;
	for(i = 0;i < iMaxUserLineTitle;i++)
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 12 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(1) );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.PrintText( iXPos + 178 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(2) );
		g_FontMgr.PrintText( iXPos + 219 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(3) );
		g_FontMgr.PrintText( iXPos + 261 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(4) );
		g_FontMgr.PrintText( iXPos + 294 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(5) );
	}

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();
	for(i = 0;i < iSize;i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( rkInfo.m_bOwner )
		{
			if( rkInfo.m_Team == TEAM_BLUE )
				m_pBlueOver->Render( iRealXPos, iRealYPos );
			else if( rkInfo.m_Team == TEAM_RED )
				m_pRedOver->Render( iRealXPos, iRealYPos );
		}
		else if( m_MouseOverIndex == i )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		// 계급 & 아이디
		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 길드 마크
		iWidth += OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );

		// 보너스
		OnRenderBonus( rkInfo, iRealXPos + iWidth + 7, iRealYPos + 3 );

		// 몬스터 코인
		OnRenderMonsterCoin( rkInfo, iRealXPos + 153, iRealYPos + 2 );

		// 기여도
		OnRenderContribute( rkInfo, iRealXPos + 212, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// KO & Death
		OnRenderKillDeath( rkInfo, iRealXPos + 254, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 핑
		OnRenderPing( rkInfo, iRealXPos + 275, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		//
		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos + m_iLineStartY;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

void RoundStateUserWnd::OnRenderFightClubUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver || !m_pBlueOver || !m_pRedOver || !m_pRedVert ) return;

	m_pRedVert->Render( iXPos, iYPos );//중앙선

	// 
	int i = 0;
	int iMaxUserLineTitle = m_iMaxLine / m_iLinePlayer;
	for(i = 0;i < iMaxUserLineTitle;i++)
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 12 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(1) );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.PrintText( iXPos + 219 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(2) );
		g_FontMgr.PrintText( iXPos + 261 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(3) );
		g_FontMgr.PrintText( iXPos + 294 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, STR(4) );
	}

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();
	for(i = 0;i < iSize;i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( rkInfo.m_bOwner )
		{
			if( rkInfo.m_Team == TEAM_BLUE )
				m_pBlueOver->Render( iRealXPos, iRealYPos );
			else if( rkInfo.m_Team == TEAM_RED )
				m_pRedOver->Render( iRealXPos, iRealYPos );
		}
		else if( m_MouseOverIndex == i )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		// 계급 & 아이디
		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 길드 마크
		iWidth += OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );

		// 보너스
		OnRenderBonus( rkInfo, iRealXPos + iWidth + 7, iRealYPos + 3 );

		// 승
		OnRenderFightWin( rkInfo, iRealXPos + 212, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 패
		OnRenderFightLose( rkInfo, iRealXPos + 254, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 핑
		OnRenderPing( rkInfo, iRealXPos + 275, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		//
		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos + m_iLineStartY;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

void RoundStateUserWnd::OnRenderShuffleBonusUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver || !m_pBlueOver || !m_pRedOver || !m_pRedVert ) return;

	m_pRedVert->Render( iXPos, iYPos );//중앙선

	// 
	int iMaxUserLineTitle = m_iMaxLine / m_iLinePlayer;
	for( int i=0 ; i<iMaxUserLineTitle ; ++i )
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 12 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "닉네임" );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.PrintText( iXPos + 261 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "별" );
		g_FontMgr.PrintText( iXPos + 294 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "핑" );
	}

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();
	for( int i=0 ; i<iSize ; ++i )
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( rkInfo.m_bOwner )
		{
			if( rkInfo.m_Team == TEAM_BLUE )
				m_pBlueOver->Render( iRealXPos, iRealYPos );
			else if( rkInfo.m_Team == TEAM_RED )
				m_pRedOver->Render( iRealXPos, iRealYPos );
		}
		else if( m_MouseOverIndex == i )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		// 계급 & 아이디
		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 길드 마크
		iWidth += OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );

		// 보너스
		OnRenderBonus( rkInfo, iRealXPos + iWidth + 7, iRealYPos + 3 );

		// 별 갯수
		OnRenderStarCount( rkInfo, iRealXPos + 254, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 핑
		OnRenderPing( rkInfo, iRealXPos + 275, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		//
		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos + m_iLineStartY;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

void RoundStateUserWnd::OnRenderRaidUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver || !m_pBlueOver || !m_pRedOver || !m_pBlueVert ) return;

	m_pBlueVert->Render( iXPos, iYPos );//중앙선

	// 
	int i = 0;
	int iMaxUserLineTitle = m_iMaxLine / m_iLinePlayer;
	for(i = 0;i < iMaxUserLineTitle;i++)
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 12 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "닉네임" );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.PrintText( iXPos + 178 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "티켓" );
		g_FontMgr.PrintText( iXPos + 219 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "기여도" );
		g_FontMgr.PrintText( iXPos + 261 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "킬데스" );
		g_FontMgr.PrintText( iXPos + 294 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "핑" );
	}

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();
	for(i = 0;i < iSize;i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( rkInfo.m_bOwner )
		{
			if( rkInfo.m_Team == TEAM_BLUE )
				m_pBlueOver->Render( iRealXPos, iRealYPos );
			else if( rkInfo.m_Team == TEAM_RED )
				m_pRedOver->Render( iRealXPos, iRealYPos );
		}
		else if( m_MouseOverIndex == i )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		// 계급 & 아이디
		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 길드 마크
		iWidth += OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );

		// 보너스
		OnRenderBonus( rkInfo, iRealXPos + iWidth + 7, iRealYPos + 3 );

		OnRenderRaidCoinCount( rkInfo, iRealXPos + 156, iRealYPos + 3 );

		// 기여도
		OnRenderContribute( rkInfo, iRealXPos + 212, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// KO & Death
		OnRenderKillDeath( rkInfo, iRealXPos + 254, iRealYPos + 3, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 핑
		OnRenderPing( rkInfo, iRealXPos + 275, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		//
		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos + m_iLineStartY;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

void RoundStateUserWnd::OnRenderSuccessionUser( int iXPos, int iYPos )
{
	if( !m_pCurrentOver ) return;

	// 
	int i = 0;
	int iMaxUserLineTitle = m_iMaxLine / m_iLinePlayer;
	for(i = 0;i < iMaxUserLineTitle;i++)
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );

		if( m_eWindowTeam == TEAM_BLUE )
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.PrintText( iXPos + 12 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "닉네임" );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.PrintText( iXPos + 294 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "핑" );

		if ( g_App.IsMeDeveloper() )
			g_FontMgr.PrintText( iXPos + 261 + i * m_iLinePlayerGap, iYPos + 62, FONT_SIZE_12, "MMR" );
	}

	int iRealXPos = iXPos + m_iLineStartX;
	int iRealYPos = iYPos + m_iLineStartY;
	int iSize = m_RoundInfoList.size();
	for(i = 0;i < iSize;i++)
	{
		RoundInfo &rkInfo = m_RoundInfoList[i];

		if( m_MouseOverIndex == i || rkInfo.m_bOwner )
			m_pCurrentOver->Render( iRealXPos, iRealYPos );

		// 계급 & 아이디
		int iWidth = OnRenderGradeAndNick( rkInfo, iRealXPos + 4, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		// 길드 마크
		iWidth += OnRenderGuildMark( rkInfo, iRealXPos + 4 + iWidth, iRealYPos + 4 );

		// 보너스
		OnRenderBonus( rkInfo, iRealXPos + iWidth + 7, iRealYPos + 3 );

		if ( g_App.IsMeDeveloper() )
		{
			SuccessionMatchVersusSceneWnd* pVersusScene = dynamic_cast<SuccessionMatchVersusSceneWnd*>(g_GUIMgr.FindWnd( SUCCESSION_BATTLE_VERSUS_SCENE_WND ));
			if ( pVersusScene )
			{
				int iMMRPoint = 0;
				if ( rkInfo.m_bOwner )
					iMMRPoint = g_MyInfo.GetSuccessionMMRPoint();
				else
					iMMRPoint = pVersusScene->GetMMRPoint( (int)rkInfo.m_Team );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextStyle( TS_NORMAL );
				if ( m_MouseOverIndex == i || rkInfo.m_bOwner )
					g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				else if( rkInfo.m_Team == TEAM_BLUE )
					g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
				else if( rkInfo.m_Team == TEAM_RED )
					g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
				g_FontMgr.SetAlignType( TAT_CENTER );

				g_FontMgr.PrintText( iRealXPos + 254, iRealYPos + 3, FONT_SIZE_12, "%d", iMMRPoint );
			}
		}

		// 핑
		OnRenderPing( rkInfo, iRealXPos + 275, iRealYPos + 2, ( m_MouseOverIndex == i || rkInfo.m_bOwner ) );

		//
		if( 0 == ( i + 1 ) % m_iLinePlayer )
		{
			iRealXPos += m_iLinePlayerGap;
			iRealYPos = iYPos + m_iLineStartY;
		}
		else
		{
			iRealYPos += m_iLineHeight;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
RoundStateMainWnd::RoundStateMainWnd()
{
	m_pPlayStage = NULL;
}

RoundStateMainWnd::~RoundStateMainWnd()
{
}

void RoundStateMainWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void RoundStateMainWnd::ClearRoundInfo()
{
	m_RoundInfoList.clear();

	RoundStateUserWnd *pPlazaWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_PLAZA_WND ));
	if( pPlazaWnd )
		pPlazaWnd->ClearRoundInfo();

	RoundStateUserWnd *pUserWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
		pUserWnd->ClearRoundInfo();

	RoundStateUserWnd *pBlueWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_BLUE_WND ));
	if( pBlueWnd )
		pBlueWnd->ClearRoundInfo();

	RoundStateUserWnd *pRedWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_RED_WND ));
	if( pRedWnd )
		pRedWnd->ClearRoundInfo();
}

void RoundStateMainWnd::AddRoundInfo( const RoundInfo &rkInfo )
{
	if( rkInfo.m_Team == TEAM_NONE ) 
		return;

	m_RoundInfoList.push_back( rkInfo );
}

void RoundStateMainWnd::SetPlazaScore()
{
	if( !m_pPlayStage ) return;
	
	HideChildWnd( ID_PLAZA_WND );
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_BLUE_WND );
	HideChildWnd( ID_RED_WND );

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoKoSort() );
	
	// 타이틀
	m_Title.ClearList();
	m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_Title.SetBkColor( 12, 66, 111 );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	if( m_pPlayStage->GetPlazaModeType() == PT_BATTLE )
		m_Title.AddTextPiece( FONT_SIZE_17, STR(1) );
	else if( m_pPlayStage->GetPlazaModeType() == PT_COMMUNITY )
		m_Title.AddTextPiece( FONT_SIZE_17, STR(2) );
	else if( m_pPlayStage->GetPlazaModeType() == PT_GUILD )
		m_Title.AddTextPiece( FONT_SIZE_17, STR(3) );
	m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	m_Title.AddTextPiece( FONT_SIZE_17, " | " );

	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	PlazaMainWnd *pMainWnd = dynamic_cast<PlazaMainWnd*>(g_GUIMgr.FindWnd( PLAZA_MAIN_WND ));
	if( !pMainWnd ) return;
    
	if( pMainWnd->IsRoomPW() )
		m_Title.AddTextPiece( FONT_SIZE_17, STR(4), pMainWnd->GetPlazaName().c_str() );
	else
		m_Title.AddTextPiece( FONT_SIZE_17, STR(5), pMainWnd->GetPlazaName().c_str() );		

	// 윈도우 세팅
	RoundStateUserWnd *pPlazaWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_PLAZA_WND ));
	if( pPlazaWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];

			// 자신은 블루 
			if( rkInfo.m_bOwner )
				rkInfo.m_Team = TEAM_BLUE;
			else
				rkInfo.m_Team = TEAM_RED;
			pPlazaWnd->AddRoundInfo( rkInfo );
		}
		pPlazaWnd->SetPlayStage( m_pPlayStage );
		pPlazaWnd->SetWindowTeam( TEAM_RED );
		pPlazaWnd->SetPlazaScore( pMainWnd->GetMaxUserCount() );
		pPlazaWnd->ShowWnd();
	}
}

void RoundStateMainWnd::SetHeadequartersScore()
{
	if( !m_pPlayStage ) return;

	HideChildWnd( ID_PLAZA_WND );
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_BLUE_WND );
	HideChildWnd( ID_RED_WND );

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoKoSort() );

	// 타이틀
	m_Title.ClearList();
	m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_Title.SetBkColor( 12, 66, 111 );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );

	if( m_pPlayStage->GetPlayMode()->GetModeType() == MT_HEADQUARTERS )
		m_Title.AddTextPiece( FONT_SIZE_17, "훈련소" );
	else if( m_pPlayStage->GetPlayMode()->GetModeType() == MT_HOUSE )
		m_Title.AddTextPiece( FONT_SIZE_17, "개인본부" );
	else
		m_Title.AddTextPiece( FONT_SIZE_17, "" );

	m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	m_Title.AddTextPiece( FONT_SIZE_17, " | " );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );

	if( m_pPlayStage->GetPlayMode()->GetModeType() == MT_HEADQUARTERS )
	{
		ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
		if( pHeadquartersMode )
		{
			m_Title.AddTextPiece( FONT_SIZE_17, "%s", pHeadquartersMode->GetMasterName().c_str() );
		}
	}
	else if( m_pPlayStage->GetPlayMode()->GetModeType() == MT_HOUSE )
	{
		ioHouseMode *pHouse = ToHouseMode( m_pPlayStage->GetPlayMode() );
		if( pHouse )
		{
			m_Title.AddTextPiece( FONT_SIZE_17, "%s", pHouse->GetMasterName().c_str() );
		}
	}
	else
	{
		m_Title.AddTextPiece( FONT_SIZE_17, "" );
	}	

	// 윈도우 세팅
	RoundStateUserWnd *pPlazaWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_PLAZA_WND ));
	if( pPlazaWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];

			// 자신은 블루 
			if( rkInfo.m_bOwner )
				rkInfo.m_Team = TEAM_BLUE;
			else
				rkInfo.m_Team = TEAM_RED;
			pPlazaWnd->AddRoundInfo( rkInfo );
		}
		pPlazaWnd->SetPlayStage( m_pPlayStage );
		pPlazaWnd->SetWindowTeam( TEAM_RED );
		pPlazaWnd->SetHeadequartersScore( MAX_PLAYER / 2 );
		pPlazaWnd->ShowWnd();
	}
}

void RoundStateMainWnd::SetSingleScore()
{
	if( !m_pPlayStage ) return;

	HideChildWnd( ID_PLAZA_WND );
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_BLUE_WND );
	HideChildWnd( ID_RED_WND );

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoSort() );

	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		SetTournamentData();

		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, STR(1) );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );

		if( m_iTournamentCurrentRound  <= 2 )
		{
			m_Title.AddTextPiece( FONT_SIZE_17, "결승" );
		}
		else
		{
			m_Title.AddTextPiece( FONT_SIZE_17, "%d강", m_iTournamentCurrentRound );
		}
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, "진행 현황" );
		m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, "%s", g_ShuffleRoomMgr.GetName().c_str() );
	}
	else if( g_RankBattleMgr.IsRankBattlePlaying() )
	{
		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, "진행 현황" );
	}
	else
	{
		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, STR(1) );
		m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		if( g_BattleRoomMgr.GetPW().IsEmpty() )
			m_Title.AddTextPiece( FONT_SIZE_17, STR(2), g_BattleRoomMgr.GetName().c_str() );
		else
			m_Title.AddTextPiece( FONT_SIZE_17, STR(3), g_BattleRoomMgr.GetName().c_str() );		
	}

	// 윈도우 세팅
	RoundStateUserWnd *pUserWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			
			// 자신은 블루 
			if( rkInfo.m_bOwner )
				rkInfo.m_Team = TEAM_BLUE;
			else
				rkInfo.m_Team = TEAM_RED;
			pUserWnd->AddRoundInfo( rkInfo );
		}
		pUserWnd->SetPlayStage( m_pPlayStage );
		pUserWnd->SetWindowTeam( TEAM_RED );
		pUserWnd->SetSingleScore( g_BattleRoomMgr.GetMaxPlayer() );
		pUserWnd->ShowWnd();
	}
}

void RoundStateMainWnd::SetTowerDefScore()
{
	if( !m_pPlayStage ) return;

	HideChildWnd( ID_PLAZA_WND );
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_BLUE_WND );
	HideChildWnd( ID_RED_WND );

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoKoSort() );

	// 타이틀
	m_Title.ClearList();
	m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_Title.SetBkColor( 12, 66, 111 );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	m_Title.AddTextPiece( FONT_SIZE_17, "진행 현황" );
	m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	m_Title.AddTextPiece( FONT_SIZE_17, " | " );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	if( g_BattleRoomMgr.GetPW().IsEmpty() )
		m_Title.AddTextPiece( FONT_SIZE_17, "%s", g_BattleRoomMgr.GetName().c_str() );
	else
		m_Title.AddTextPiece( FONT_SIZE_17, "%s(비밀방)", g_BattleRoomMgr.GetName().c_str() );		

	// 윈도우 세팅
	RoundStateUserWnd *pUserWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			rkInfo.m_Team = TEAM_BLUE;
			pUserWnd->AddRoundInfo( rkInfo );
		}
		pUserWnd->SetPlayStage( m_pPlayStage );
		pUserWnd->SetWindowTeam( TEAM_BLUE );
		pUserWnd->SetSingleScore( g_BattleRoomMgr.GetMaxPlayer() );
		pUserWnd->ShowWnd();
	}
}

void RoundStateMainWnd::SetGangsiScore()
{
	if( !m_pPlayStage ) return;

	HideChildWnd( ID_PLAZA_WND );
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_BLUE_WND );
	HideChildWnd( ID_RED_WND );

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoSort() );

	// 타이틀
	m_Title.ClearList();
	m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_Title.SetBkColor( 12, 66, 111 );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	m_Title.AddTextPiece( FONT_SIZE_17, STR(1) );
	m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	m_Title.AddTextPiece( FONT_SIZE_17, " | " );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	if( g_BattleRoomMgr.GetPW().IsEmpty() )
		m_Title.AddTextPiece( FONT_SIZE_17, STR(2), g_BattleRoomMgr.GetName().c_str() );
	else
		m_Title.AddTextPiece( FONT_SIZE_17, STR(3), g_BattleRoomMgr.GetName().c_str() );		

	// 윈도우 세팅
	RoundStateUserWnd *pUserWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			pUserWnd->AddRoundInfo( rkInfo );
		}
		pUserWnd->SetPlayStage( m_pPlayStage );
		pUserWnd->SetWindowTeam( TEAM_RED );
		pUserWnd->SetGangsiScore( g_BattleRoomMgr.GetMaxPlayer() );
		pUserWnd->ShowWnd();
	}
}

void RoundStateMainWnd::SetDungeonsScore( int iCurrentFloor )
{
	if( !m_pPlayStage ) return;

	HideChildWnd( ID_PLAZA_WND );
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_BLUE_WND );
	HideChildWnd( ID_RED_WND );

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoSort() );

	// 타이틀
	m_Title.ClearList();
	m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_Title.SetBkColor( 12, 66, 111 );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	m_Title.AddTextPiece( FONT_SIZE_17, STR(1) );
	m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	m_Title.AddTextPiece( FONT_SIZE_17, " | " );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	if( g_BattleRoomMgr.GetPW().IsEmpty() )
		m_Title.AddTextPiece( FONT_SIZE_17, STR(2), g_BattleRoomMgr.GetName().c_str() );
	else
		m_Title.AddTextPiece( FONT_SIZE_17, STR(3), g_BattleRoomMgr.GetName().c_str() );		

    // 윈도우 세팅
	RoundStateUserWnd *pUserWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];

			// 모두 블루 
			rkInfo.m_Team = TEAM_BLUE;
			pUserWnd->AddRoundInfo( rkInfo );
		}
		pUserWnd->SetPlayStage( m_pPlayStage );
		pUserWnd->SetWindowTeam( TEAM_BLUE );
		pUserWnd->SetDungeonsScore( g_BattleRoomMgr.GetMaxPlayer(), iCurrentFloor );
		pUserWnd->ShowWnd();
	}
}

void RoundStateMainWnd::SetTeamScore( int iBlueWin, int iRedWin )
{
	if( !m_pPlayStage  ) return;

	HideChildWnd( ID_PLAZA_WND );
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_BLUE_WND );
	HideChildWnd( ID_RED_WND );

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoSort() );

	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		SetTournamentData();

		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, STR(1) );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		
		if( m_iTournamentCurrentRound  <= 2 )
		{
			m_Title.AddTextPiece( FONT_SIZE_17, "결승" );
		}
		else
		{
			m_Title.AddTextPiece( FONT_SIZE_17, "%d강", m_iTournamentCurrentRound );
		}
		
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, STR(1) );
		m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		if( g_LadderTeamMgr.GetLadderTeamPW().IsEmpty() )
			m_Title.AddTextPiece( FONT_SIZE_17, STR(2), g_LadderTeamMgr.GetLadderTeamName().c_str() );
		else
			m_Title.AddTextPiece( FONT_SIZE_17, STR(3), g_LadderTeamMgr.GetLadderTeamName().c_str() );		
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, "진행 현황" );
		m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, "%s", g_ShuffleRoomMgr.GetName().c_str() );
	}
	else if( g_RankBattleMgr.IsRankBattlePlaying() )
	{
		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, "진행 현황" );
	}
	else
	{
		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, STR(4) );
		m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		if( g_BattleRoomMgr.GetPW().IsEmpty() )
			m_Title.AddTextPiece( FONT_SIZE_17, STR(5), g_BattleRoomMgr.GetName().c_str() );
		else
			m_Title.AddTextPiece( FONT_SIZE_17, STR(6), g_BattleRoomMgr.GetName().c_str() );		
	}

	RoundStateUserWnd *pBlueWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_BLUE_WND ));
	if( pBlueWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			if( rkInfo.m_Team == TEAM_BLUE )
				pBlueWnd->AddRoundInfo( rkInfo );
		}

		pBlueWnd->SetWindowTeam( TEAM_BLUE );
		pBlueWnd->SetPlayStage( m_pPlayStage );
		if( g_LadderTeamMgr.IsLadderTeam() )
		{
			const LadderTeamInfo &rkTeamInfo = g_LadderTeamMgr.GetLadderBlueTeamInfo();
			pBlueWnd->SetTeamScore( rkTeamInfo.m_iMaxPlayer, iBlueWin );
		}
		else
			pBlueWnd->SetTeamScore( g_BattleRoomMgr.GetMaxPlayerBlue(), iBlueWin );
		pBlueWnd->ShowWnd();
	}

	RoundStateUserWnd *pRedWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_RED_WND ));
	if( pRedWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			if( rkInfo.m_Team == TEAM_RED )
				pRedWnd->AddRoundInfo( rkInfo );
		}

		pRedWnd->SetWindowTeam( TEAM_RED );
		pRedWnd->SetPlayStage( m_pPlayStage );
		if( g_LadderTeamMgr.IsLadderTeam() )
		{
			const LadderTeamInfo &rkTeamInfo = g_LadderTeamMgr.GetLadderRedTeamInfo();
			pRedWnd->SetTeamScore( rkTeamInfo.m_iMaxPlayer, iRedWin );
		}
		else
			pRedWnd->SetTeamScore( g_BattleRoomMgr.GetMaxPlayerRed(), iRedWin );
		pRedWnd->ShowWnd();
	}			
}

void RoundStateMainWnd::SetStarCountScore()
{
	if( !m_pPlayStage ) return;

	HideChildWnd( ID_PLAZA_WND );
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_BLUE_WND );
	HideChildWnd( ID_RED_WND );

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoStarSort() );

	// 타이틀
	m_Title.ClearList();
	m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_Title.SetBkColor( 12, 66, 111 );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	m_Title.AddTextPiece( FONT_SIZE_17, "진행 현황" );
	m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	m_Title.AddTextPiece( FONT_SIZE_17, " | " );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	m_Title.AddTextPiece( FONT_SIZE_17, "%s", g_ShuffleRoomMgr.GetName().c_str() );

	// 윈도우 세팅
	RoundStateUserWnd *pUserWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];

			// 자신은 블루 
			if( rkInfo.m_bOwner )
				rkInfo.m_Team = TEAM_BLUE;
			else
				rkInfo.m_Team = TEAM_RED;
			pUserWnd->AddRoundInfo( rkInfo );
		}
		pUserWnd->SetPlayStage( m_pPlayStage );
		pUserWnd->SetWindowTeam( TEAM_RED );
		pUserWnd->SetSingleScore( g_BattleRoomMgr.GetMaxPlayer() );
		pUserWnd->ShowWnd();
	}
}

void RoundStateMainWnd::SetRaidCoinCountScore()
{
	if( !m_pPlayStage ) return;

	HideChildWnd( ID_PLAZA_WND );
	HideChildWnd( ID_SINGLE_WND );
	HideChildWnd( ID_BLUE_WND );
	HideChildWnd( ID_RED_WND );

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoSort() );

	// 타이틀
	m_Title.ClearList();
	m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_Title.SetBkColor( 12, 66, 111 );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	m_Title.AddTextPiece( FONT_SIZE_17, "진행 현황" );
	m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	m_Title.AddTextPiece( FONT_SIZE_17, " | " );
	m_Title.SetTextColor( TCT_DEFAULT_WHITE );
	if( g_BattleRoomMgr.GetPW().IsEmpty() )
		m_Title.AddTextPiece( FONT_SIZE_17, "%s", g_BattleRoomMgr.GetName().c_str() );
	else
		m_Title.AddTextPiece( FONT_SIZE_17, "%s(비밀방)", g_BattleRoomMgr.GetName().c_str() );		

	// 윈도우 세팅
	RoundStateUserWnd *pUserWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_SINGLE_WND ));
	if( pUserWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			rkInfo.m_Team = TEAM_BLUE;
			pUserWnd->AddRoundInfo( rkInfo );
		}
		pUserWnd->SetPlayStage( m_pPlayStage );
		pUserWnd->SetWindowTeam( TEAM_BLUE );
		pUserWnd->SetSingleScore( g_BattleRoomMgr.GetMaxPlayer() );
		pUserWnd->ShowWnd();
	}
}

void RoundStateMainWnd::iwm_show()
{
	if( !m_pPlayStage ) return;

	switch( m_pPlayStage->GetModeType() )
	{
	case MT_TRAINING:
	case MT_HEADQUARTERS:
	case MT_HOUSE:
		SetSize( PLAZA_WIDTH, PLAZA_HEIGHT );
		break;
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_FACTORY:
		SetSize(TOWER_MODE_WIDTH, TOWER_MODE_HEIGHT);
		break;
	default:
		SetSize( MODE_WIDTH, MODE_HEIGHT );
		break;
	}

	if( g_ShuffleRoomMgr.IsShuffleRoom() || g_RankBattleMgr.IsRankBattlePlaying() )
	{
		HideChildWnd( ID_ROOM_INFO );
	}
	else
	{
		ioWnd *pRoomInfo = FindChildWnd( ID_ROOM_INFO );
		if( pRoomInfo )
			pRoomInfo->SetWndPos( GetWidth() - ROOM_INFO_BTN_RIGHT_GAP, pRoomInfo->GetYPos() );
	}

	SetWndPos( ( Setting::Width() / 2 ) - ( GetWidth() / 2 ),
			   ( Setting::Height() / 2 ) - ( GetHeight() / 2 ) - 20 );
}

void RoundStateMainWnd::iwm_hide()
{
}

void RoundStateMainWnd::SetTournamentData()
{
	m_iTournamentCurrentRound = 0;
	if( g_BattleRoomMgr.IsBattleRoom() && g_BattleRoomMgr.IsTournamentModeType() )
	{
		DWORD dwTournamentIndex = g_BattleRoomMgr.GetTournamentIndex();
		TournamentNode *pTournament = g_TournamentMgr.GetTournament( dwTournamentIndex );
		if( pTournament )
		{
			TeamGlobalData* pBlue = g_TournamentTeamMgr.GetTeamData(  g_BattleRoomMgr.GetTournamentBlueIndex(), false );
			if( pBlue )
			{
				RoundStateUserWnd *pBlueWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_BLUE_WND ));
				if( pBlueWnd )
				{
					pBlueWnd->SetTournamentTeamName( pBlue->GetTeamName() );

					if( m_iTournamentCurrentRound == 0 )
						m_iTournamentCurrentRound = Help::TournamentCurrentRoundWithTeam( pTournament->GetTournamentStartTeamCount(), pBlue->GetTourPos() - 1 );
					

				}
			}

			TeamGlobalData* pRed = g_TournamentTeamMgr.GetTeamData( g_BattleRoomMgr.GetTournamentRedIndex(), false );
			if( pRed )
			{
				RoundStateUserWnd *pRedWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_RED_WND ));
				if( pRedWnd )
				{
					pRedWnd->SetTournamentTeamName( pRed->GetTeamName() );

					if( m_iTournamentCurrentRound == 0 )
						m_iTournamentCurrentRound = Help::TournamentCurrentRoundWithTeam( pTournament->GetTournamentStartTeamCount(), pRed->GetTourPos() - 1 );
				}
			}
		}
		HideChildWnd( ID_EXIT_BTN );
	}
}

void RoundStateMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ROOM_INFO:
		if( cmd == IOBN_BTNUP )
		{
			switch( ioPlayMode::GetModeType() )
			{
			case MT_TRAINING:
				{
					if( g_GUIMgr.IsShow( PLAZA_MAIN_WND ) )
						g_GUIMgr.HideWnd( PLAZA_MAIN_WND );
					else
						g_GUIMgr.ShowWnd( PLAZA_MAIN_WND );
				}
				break;
			case MT_HEADQUARTERS:
				{
					// 방정보
					if( g_MyInfo.IsTutorialUser() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					}
					else
					{
						if( g_GUIMgr.IsShow( HEADQUARTERS_MAIN_WND ) )
							g_GUIMgr.HideWnd( HEADQUARTERS_MAIN_WND );
						else
							g_GUIMgr.ShowWnd( HEADQUARTERS_MAIN_WND );
					}
				}
				break;
			case MT_HOUSE:
				{
					// 방정보
					if( g_MyInfo.IsTutorialUser() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					}
					else
					{
						if( g_GUIMgr.IsShow( HOUSE_MAIN_WND ) )
							g_GUIMgr.HideWnd( HOUSE_MAIN_WND );
						else
							g_GUIMgr.ShowWnd( HOUSE_MAIN_WND );
					}
				}
				break;
			default:
				{
					if( g_BattleRoomMgr.IsBattleRoom() )
					{
						if( g_GUIMgr.IsShow( PARTY_BATTLE_SIMPLE_WND ) )
							g_GUIMgr.HideWnd( PARTY_BATTLE_SIMPLE_WND );
						else
							g_GUIMgr.ShowWnd( PARTY_BATTLE_SIMPLE_WND );
					}
					else if( g_LadderTeamMgr.IsLadderTeam() )
					{
						if( g_GUIMgr.IsShow( MY_LADDER_TEAM_INFO_WND ) )
							g_GUIMgr.HideWnd( MY_LADDER_TEAM_INFO_WND );
						else
							g_GUIMgr.ShowWnd( MY_LADDER_TEAM_INFO_WND );
					}
				}
				break;
			}
		}
		break;
	}
}

void RoundStateMainWnd::OnRender()
{
	ioWnd::OnRender();

	m_Title.PrintFullTextWidthCut( GetDerivedPosX() + 13, GetDerivedPosY() + 11, TAT_LEFT, 580.0f );
}

// 2019-07-03
bool RoundStateMainWnd::IsSearchShow()
{	
	SoldierSelectWnd *pSoldierSelectWnd = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( (NULL != pSoldierSelectWnd) && (true == pSoldierSelectWnd->IsShow()) )
	{
		return true;
	}

	UISoldierSelectWnd *pUISoldierSelectWnd = dynamic_cast<UISoldierSelectWnd*>(g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ));
	if( (NULL != pUISoldierSelectWnd) && (true == pUISoldierSelectWnd->IsShow()) )
	{
		return true;
	}

	MyInventoryWnd *pMyInventoryWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsNewSearchWnd()) )
	{
		return true;
	}	

	return false;
}
//////////////////////////////////////////////////////////////////////////
RoundResultMainWnd::RoundResultMainWnd()
{
	m_pPlayStage = NULL;
}

RoundResultMainWnd::~RoundResultMainWnd()
{
}

void RoundResultMainWnd::iwm_show()
{
	RoundStateMainWnd::iwm_show();

	g_GUIMgr.HideWnd( MINIMAP_WND );
	g_GUIMgr.HideWnd( TIME_SCORE_WND );
	g_GUIMgr.HideWnd( SKILL_LIST_WND );
	g_GUIMgr.HideWnd( ACCESSORY_SKILL_LIST_WND );
	g_GUIMgr.HideWnd( CENTER_INFO_WND );
	g_GUIMgr.HideWnd( MODE_RANK_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_TIME_GAUGE_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_ROUND_END_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_CHALLENGER_WND );
	g_GUIMgr.HideWnd( TOWERDEF_USER_GAUGE_WND );
	g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_CHALLENGER_WND );
	g_GUIMgr.HideWnd( FIGHTCLUB_CONTINUE_WND );
	g_GUIMgr.HideWnd( TUTORIAL_KEYBOARD_WND );
	g_GUIMgr.HideWnd( TRAINING_FUNC_MENU );
	g_GUIMgr.HideWnd( BATTLE_MODE_TIME_GAUGE_WND );
	g_GUIMgr.HideWnd( BATTLE_MODE_FUNCTION_WND );
	g_GUIMgr.HideWnd( BATTLE_MODE_BLUE_USER_INFO_WND );
	g_GUIMgr.HideWnd( BATTLE_MODE_RED_USER_INFO_WND );
	g_ModeHelpMgr.HideHelpDirectly();

	if( g_Input.IsKeyDown( KEY_TAB ) )
	{
		SetWndPos( Setting::Width(), Setting::Height() );
	}
}

void RoundResultMainWnd::iwm_hide()
{
	if( m_RoundInfoList.size() != 0 )
	{
		g_GUIMgr.ShowWnd( MINIMAP_WND );
		g_GUIMgr.ShowWnd( TIME_SCORE_WND );
		g_GUIMgr.ShowWnd( SKILL_LIST_WND );
		g_GUIMgr.ShowWnd( ACCESSORY_SKILL_LIST_WND );
		g_GUIMgr.ShowWnd( FIGHTCLUB_TIME_GAUGE_WND );
		g_GUIMgr.ShowWnd( MINI_SOLDIER_SELECT_WND );
	}	
	RoundStateMainWnd::iwm_hide();
}

void RoundResultMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	if( g_Input.IsKeyDown( KEY_TAB ) )
	{
		SetWndPos( Setting::Width(), Setting::Height() );
	}
	else
	{
		SetWndPos( ( Setting::Width() / 2 ) - ( GetWidth() / 2 ),
				   ( Setting::Height() / 2 ) - ( GetHeight() / 2 ) - 20 );
	}

	if( m_pPlayStage )
	{
		ioPlayMode *pPlayMode = m_pPlayStage->GetPlayMode();
		if( !pPlayMode ) return;
		if( pPlayMode->GetModeState() == ioPlayMode::MS_PLAY )
		{
			HideWnd();
		}
	}
}

void RoundResultMainWnd::SetTeamScore( int iBlueWin, int iRedWin )
{
	if( !m_pPlayStage  ) return;

	std::sort( m_RoundInfoList.begin(), m_RoundInfoList.end(), RoundInfoSort() );

	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		SetTournamentData();

		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, STR(1) );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );

		if( m_iTournamentCurrentRound  <= 2 )
		{
			m_Title.AddTextPiece( FONT_SIZE_17, "결승" );
		}
		else
		{
			m_Title.AddTextPiece( FONT_SIZE_17, "%d강", m_iTournamentCurrentRound );
		}

	}else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, STR(1) );
		m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		if( g_LadderTeamMgr.GetLadderTeamPW().IsEmpty() )
			m_Title.AddTextPiece( FONT_SIZE_17, STR(2), g_LadderTeamMgr.GetLadderTeamName().c_str() );
		else
			m_Title.AddTextPiece( FONT_SIZE_17, STR(3), g_LadderTeamMgr.GetLadderTeamName().c_str() );		
	}
	else
	{
		// 타이틀
		m_Title.ClearList();
		m_Title.SetTextStyle( TS_OUTLINE_FULL_2X );
		m_Title.SetBkColor( 12, 66, 111 );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		m_Title.AddTextPiece( FONT_SIZE_17, STR(4) );
		m_Title.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		m_Title.AddTextPiece( FONT_SIZE_17, " | " );
		m_Title.SetTextColor( TCT_DEFAULT_WHITE );
		if( g_BattleRoomMgr.GetPW().IsEmpty() )
			m_Title.AddTextPiece( FONT_SIZE_17, STR(5), g_BattleRoomMgr.GetName().c_str() );
		else
			m_Title.AddTextPiece( FONT_SIZE_17, STR(6), g_BattleRoomMgr.GetName().c_str() );		
	}

	RoundStateUserWnd *pBlueWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_BLUE_WND ));
	if( pBlueWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			if( rkInfo.m_Team == TEAM_BLUE )
				pBlueWnd->AddRoundInfo( rkInfo );
		}

		pBlueWnd->SetWindowTeam( TEAM_BLUE );
		pBlueWnd->SetPlayStage( m_pPlayStage );
		if( g_LadderTeamMgr.IsLadderTeam() )
		{
			const LadderTeamInfo &rkTeamInfo = g_LadderTeamMgr.GetLadderBlueTeamInfo();
			pBlueWnd->SetTeamScore( rkTeamInfo.m_iMaxPlayer, iBlueWin );
		}
		else
			pBlueWnd->SetTeamScore( g_BattleRoomMgr.GetMaxPlayerBlue(), iBlueWin );
		pBlueWnd->ShowWnd();
	}

	RoundStateUserWnd *pRedWnd = dynamic_cast<RoundStateUserWnd *>(FindChildWnd( ID_RED_WND ));
	if( pRedWnd )
	{
		int iUserSize = m_RoundInfoList.size();
		for(int i = 0;i < iUserSize;i++)
		{
			RoundInfo &rkInfo = m_RoundInfoList[i];
			if( rkInfo.m_Team == TEAM_RED )
				pRedWnd->AddRoundInfo( rkInfo );
		}

		pRedWnd->SetWindowTeam( TEAM_RED );
		pRedWnd->SetPlayStage( m_pPlayStage );
		if( g_LadderTeamMgr.IsLadderTeam() )
		{
			const LadderTeamInfo &rkTeamInfo = g_LadderTeamMgr.GetLadderRedTeamInfo();
			pRedWnd->SetTeamScore( rkTeamInfo.m_iMaxPlayer, iRedWin );
		}
		else
			pRedWnd->SetTeamScore( g_BattleRoomMgr.GetMaxPlayerRed(), iRedWin );
		pRedWnd->ShowWnd();
	}			
}