

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioProgressBar.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../ioVoiceChat/Fourier.h"

#include "../TextColorTable.h"
#include "../ioApplication.h"
#include "../ioMyInfo.h"
#include "../Setting.h"
#include "../ioPlayMode.h"
#include "../WndID.h"
#include "../ioPresentMgr.h"

#include "ioSP2GUIManager.h"
#include "TimeScoreWnd.h"
#include "CenterInfoWnd.h"
#include "../io3DEngine/ioStringManager.h"

CenterInfoWnd::CenterInfoWnd()
{
	int i = 0;
	m_pBlueNum	  = NULL;
	m_pRedNum	  = NULL;
	m_pWhiteNum	  = NULL;
	m_pNumColon	  = NULL;
	m_pCenterLine = NULL;
	m_pDeathTimeText = NULL;
	m_pSuddenDeathTimeText = NULL;

	m_pTextSec = NULL;
	for(i = 0;i < MAX_EFFECT;i++)
	{
		m_pBlueEffectSpr[i] = NULL;
		m_pRedEffectSpr[i] = NULL;
	}

	m_pBlueIconBack  = NULL;
	m_pBlueGaugeBack = NULL;
	m_pBlueGauge	 = NULL;

	m_pRedIconBack   = NULL;
	m_pRedGaugeBack  = NULL;
	m_pRedGauge	     = NULL;

	m_pWhiteGauge    = NULL;
	m_pTextRank      = NULL;
	m_pTextSurvivor  = NULL;

	m_pBlueCrown	 = NULL;
	m_pBlueCrownText = NULL;
	m_pRedCrown		 = NULL;
	m_pRedCrownText	 = NULL;

	for(i = 0;i < MAX_BOSSICON;i++)
		m_pBossIcon[i]	= NULL;
	m_pBossText = NULL;

	m_pBlueStoneIcon = NULL;
	m_pRedStoneIcon = NULL;
	m_pKoText = NULL;
	m_pStoneGaugeBack = NULL;
	m_pStoneBlueGauge1 = NULL;
	m_pStoneBlueGauge2 = NULL;
	m_pStoneRedGauge1 = NULL;
	m_pStoneRedGauge2 = NULL;
	m_pStoneBlueWhiteGauge = NULL;
	m_pStoneRedWhiteGauge = NULL;
	m_pStoneAttack1 = NULL;
	m_pStoneAttack2 = NULL;
	m_pStoneBlueAttack = NULL;
	m_pStoneRedAttack  = NULL;	
	
	m_pDiceRewardFailIcon = NULL;
	m_pDiceRewardIconBack = NULL;
	m_pDicePrizeIcon      = NULL;
	m_pDiceSubIcon = NULL;

	m_pDoubleCrownSimilar = NULL;
	m_pDoubleBlueCrown	  = NULL;
	m_pDoubleBlueCloseWin = NULL;

	m_pDoubleRedCrown	 = NULL;
	m_pDoubleRedCloseWin = NULL;
	m_pDoubleBlueWhiteGauge = NULL;
	m_pDoubleRedWhiteGauge  = NULL;

	m_pFlagCaptureNewFlag = NULL;
	m_pFlagCaptureBlueReturn = NULL;
	m_pFlagCaptureRedReturn = NULL;

	m_bTeamDeathWarningScore = false;
	m_bDeathMatchTimeWarning = false;
	m_eKingTeam = m_eOwnerTeam = TEAM_NONE;
	m_dwGivePointTime = m_dwCrownHoldTime = 0;
	m_fScoreWarningRate = 0.0f;
	m_iEffectArray = -1;
	m_ePrevTeamDeathAni = TDA_NONE;
	m_ePrevDeathMatchAni = DMA_NONE;

	m_iBossRaidOrder = m_iBossRaidMaxOrder = m_iBossRaidMyLevel = 0;
	m_dwBossRaidTime = 0;
	m_bBossRaidSnd = false;
	
	m_eKillTeam = m_eDieTeam = TEAM_NONE;
	m_iCurrentBluePoint = m_iCurrentRedPoint = 0;	
	m_iAddBluePoint = m_iAddRedPoint = 0;
	m_bBlueScoreUP = false;
	m_fTimeRate = m_fAlphaRate = 0.0f;
	m_iAddXPos = m_iAddYPos = 0;
	m_bDeathTimeScreen = false;

	m_fBlueSymbolRate = m_fRedSymbolRate = 0.0f;
	m_fPrevBlueSymbolRate = m_fPrevRedSymbolRate = 0.0f;
	m_fBlueGaugeAlpha = m_fRedGaugeAlpha = 0.0f;
	m_fBlueGaugeDownTime = m_fRedGaugeDownTime = 0.0f;

	m_iPrevRank = m_iCurRank = -1;
	m_fPrevRankRate = m_fCurRankRate = 0.0f;
	m_bGangsiAlarmScreen = false;
	m_dwGangsiAlarmTickTime = m_dwGangsiAlarmWarningTime = 0;
	m_bGangsiAlarmText = false;

	//
	m_bBlueSymbolAttack = false;
	m_fBlueSymbolAttackTime = 0.0f;
	m_iBlueSymbolVibration = 0;
	m_iBlueSymbolAttackX = 0;
	m_iBlueSymbolAttackY = 0;
	m_bRedSymbolAttack = false;
	m_fRedSymbolAttackTime = 0.0f;
	m_iRedSymbolVibration = 0;
	m_iRedSymbolAttackX = 0;
	m_iRedSymbolAttackY = 0;

	//
	m_bDoubleBlueAttack = false;
	m_fDoubleBlueAttackTime = 0.0f;
	m_iDoubleBlueVibration = 0;
	m_iDoubleBlueAttackX = 0;
	m_iDoubleBlueAttackY = 0;

	m_bDoubleRedAttack = false;
	m_fDoubleRedAttackTime = 0.0f;
	m_iDoubleRedVibration = 0;
	m_iDoubleRedAttackX = 0;
	m_iDoubleRedAttackY = 0;

	m_iDoubleWinningTeam = TEAM_NONE;
	m_iBlueEffectArray = -1;
	m_iRedEffectArray  = -1;
	m_bRedKing = false;
	m_bBlueKing = false;

	//
	m_fDoubleBlueRate = m_fDoubleRedRate = 0.0f;
	m_fDoublePrevBlueRate = m_fDoublePrevRedRate = 0.0f;
	m_fDoubleBlueGaugeAlpha = m_fDoubleRedGaugeAlpha = 0.0f;
	m_fDoubleBlueGaugeDownTime = m_fDoubleRedGaugeDownTime = 0.0f;

	m_eFlagKillTeam = m_eFlagDieTeam = TEAM_NONE;
	m_iFlagBluePoint = m_iFlagRedPoint = 0;	
	m_iFlagAddBluePoint = m_iFlagAddRedPoint = 0;
	m_bFlagBlueScoreUP = false;
	m_bFlagCaptureWarningScore = false;
	m_bFlagNoPointChange = false;
	m_eFlagReturn = FR_NONE;
	m_dwReturnAlramTime = 0;
	m_dwNewFlagTime = 0;
	m_dwFlagWaitRemainTime = 0;
	m_ePrevFlagCaptureAni = FCA_NONE;

	//
	m_iWhiteColor = 0;
	m_iPrizeRollingArray = 0;
	m_bPrizeScreen = false;

	m_iDefaultX     = 0;
	m_iDefaultY     = 0;

	m_bNoHideWnd    = false;

	InitHiddenCrownAni();
	InitBossRaidAni();
	InitTeamDeathAni();
	InitDeathMatchAni();
	InitDeathTimeAni();
	InitSymbolGaugeAni();
	InitGangsiModeAni();
	InitGangsiAlarmAni();
	InitDungeonAModeAni();
	InitDoubleCrownAni();
	InitFlagCaptureAni();
}

CenterInfoWnd::~CenterInfoWnd()
{
	SAFEDELETE( m_pBlueNum );
	SAFEDELETE( m_pRedNum );
	SAFEDELETE( m_pWhiteNum );
	SAFEDELETE( m_pNumColon );
	SAFEDELETE( m_pCenterLine );
	SAFEDELETE( m_pDeathTimeText );
	SAFEDELETE( m_pSuddenDeathTimeText );
	for(int i = 0;i < MAX_EFFECT;i++)
	{
		SAFEDELETE( m_pBlueEffectSpr[i] );
		SAFEDELETE( m_pRedEffectSpr[i] );
	}

	SAFEDELETE( m_pBlueIconBack );
	SAFEDELETE( m_pBlueGaugeBack );
	SAFEDELETE( m_pBlueGauge );

	SAFEDELETE( m_pRedIconBack );
	SAFEDELETE( m_pRedGaugeBack );
	SAFEDELETE( m_pRedGauge );
	
	SAFEDELETE( m_pWhiteGauge );
	SAFEDELETE( m_pTextRank );
	SAFEDELETE( m_pTextSurvivor );

	SAFEDELETE( m_pBlueCrown );
	SAFEDELETE( m_pBlueCrownText );
	SAFEDELETE( m_pRedCrown );
	SAFEDELETE( m_pRedCrownText );

	for(i = 0;i < MAX_BOSSICON;i++)
		SAFEDELETE( m_pBossIcon[i] );
	SAFEDELETE( m_pBossText );

	SAFEDELETE( m_pBlueStoneIcon );
	SAFEDELETE( m_pRedStoneIcon );
	SAFEDELETE( m_pKoText );
	SAFEDELETE( m_pStoneGaugeBack );
	SAFEDELETE( m_pStoneBlueGauge1 );
	SAFEDELETE( m_pStoneBlueGauge2 );
	SAFEDELETE( m_pStoneRedGauge1 );
	SAFEDELETE( m_pStoneRedGauge2 );
	SAFEDELETE( m_pStoneBlueWhiteGauge );
	SAFEDELETE( m_pStoneRedWhiteGauge );
	SAFEDELETE( m_pStoneAttack1 );
	SAFEDELETE( m_pStoneAttack2 );
	SAFEDELETE( m_pTextSec );

	SAFEDELETE( m_pDiceRewardFailIcon );
	SAFEDELETE( m_pDiceRewardIconBack );
	SAFEDELETE( m_pDicePrizeIcon );
	SAFEDELETE( m_pDiceSubIcon );

	SAFEDELETE( m_pDoubleCrownSimilar );
	SAFEDELETE( m_pDoubleBlueCrown );
	SAFEDELETE( m_pDoubleBlueCloseWin );
	SAFEDELETE( m_pDoubleRedCrown );
	SAFEDELETE( m_pDoubleRedCloseWin );
	SAFEDELETE( m_pDoubleBlueWhiteGauge );
	SAFEDELETE( m_pDoubleRedWhiteGauge );

	SAFEDELETE( m_pFlagCaptureNewFlag );
	SAFEDELETE( m_pFlagCaptureBlueReturn );
	SAFEDELETE( m_pFlagCaptureRedReturn );

	BossRaidSound( false );

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || !COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "CenterInfoWnd2", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "CenterInfoWnd2", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void CenterInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BlueNum" )
	{
		SAFEDELETE( m_pBlueNum );
		m_pBlueNum = pImage;
	}
	else if( szType == "RedNum" )
	{
		SAFEDELETE( m_pRedNum );
		m_pRedNum = pImage;
	}
	else if( szType == "WhiteNum" )
	{
		SAFEDELETE( m_pWhiteNum );
		m_pWhiteNum = pImage;
	}
	else if( szType == "NumColon" )
	{
		SAFEDELETE( m_pNumColon );
		m_pNumColon = pImage;
	}
	else if( szType == "CenterLine" )
	{
		SAFEDELETE( m_pCenterLine );
		m_pCenterLine = pImage;
	}
	else if( szType == "DeathTimeText" )
	{
		SAFEDELETE( m_pDeathTimeText );
		m_pDeathTimeText = pImage;
	}
	else if( szType == "SuddenDeathTimeText" )
	{
		SAFEDELETE( m_pSuddenDeathTimeText );
		m_pSuddenDeathTimeText = pImage;
	}
	else if( szType == "BlueEffectSpr1" )
	{
		SAFEDELETE( m_pBlueEffectSpr[0] );
		m_pBlueEffectSpr[0] = pImage;
	}
	else if( szType == "BlueEffectSpr2" )
	{
		SAFEDELETE( m_pBlueEffectSpr[1] );
		m_pBlueEffectSpr[1] = pImage;
	}
	else if( szType == "BlueEffectSpr3" )
	{
		SAFEDELETE( m_pBlueEffectSpr[2] );
		m_pBlueEffectSpr[2] = pImage;
	}
	else if( szType == "BlueEffectSpr4" )
	{
		SAFEDELETE( m_pBlueEffectSpr[3] );
		m_pBlueEffectSpr[3] = pImage;
	}
	else if( szType == "BlueEffectSpr5" )
	{
		SAFEDELETE( m_pBlueEffectSpr[4] );
		m_pBlueEffectSpr[4] = pImage;
	}
	else if( szType == "BlueEffectSpr6" )
	{
		SAFEDELETE( m_pBlueEffectSpr[5] );
		m_pBlueEffectSpr[5] = pImage;
	}
	else if( szType == "BlueEffectSpr7" )
	{
		SAFEDELETE( m_pBlueEffectSpr[6] );
		m_pBlueEffectSpr[6] = pImage;
	}
	else if( szType == "BlueEffectSpr8" )
	{
		SAFEDELETE( m_pBlueEffectSpr[7] );
		m_pBlueEffectSpr[7] = pImage;
	}
	else if( szType == "BlueEffectSpr9" )
	{
		SAFEDELETE( m_pBlueEffectSpr[8] );
		m_pBlueEffectSpr[8] = pImage;
	}
	else if( szType == "BlueEffectSpr10" )
	{
		SAFEDELETE( m_pBlueEffectSpr[9] );
		m_pBlueEffectSpr[9] = pImage;
	}
	else if( szType == "RedEffectSpr1" )
	{
		SAFEDELETE( m_pRedEffectSpr[0] );
		m_pRedEffectSpr[0] = pImage;
	}
	else if( szType == "RedEffectSpr2" )
	{
		SAFEDELETE( m_pRedEffectSpr[1] );
		m_pRedEffectSpr[1] = pImage;
	}
	else if( szType == "RedEffectSpr3" )
	{
		SAFEDELETE( m_pRedEffectSpr[2] );
		m_pRedEffectSpr[2] = pImage;
	}
	else if( szType == "RedEffectSpr4" )
	{
		SAFEDELETE( m_pRedEffectSpr[3] );
		m_pRedEffectSpr[3] = pImage;
	}
	else if( szType == "RedEffectSpr5" )
	{
		SAFEDELETE( m_pRedEffectSpr[4] );
		m_pRedEffectSpr[4] = pImage;
	}
	else if( szType == "RedEffectSpr6" )
	{
		SAFEDELETE( m_pRedEffectSpr[5] );
		m_pRedEffectSpr[5] = pImage;
	}
	else if( szType == "RedEffectSpr7" )
	{
		SAFEDELETE( m_pRedEffectSpr[6] );
		m_pRedEffectSpr[6] = pImage;
	}
	else if( szType == "RedEffectSpr8" )
	{
		SAFEDELETE( m_pRedEffectSpr[7] );
		m_pRedEffectSpr[7] = pImage;
	}
	else if( szType == "RedEffectSpr9" )
	{
		SAFEDELETE( m_pRedEffectSpr[8] );
		m_pRedEffectSpr[8] = pImage;
	}
	else if( szType == "RedEffectSpr10" )
	{
		SAFEDELETE( m_pRedEffectSpr[9] );
		m_pRedEffectSpr[9] = pImage;
	}
	else if( szType == "BlueIconBack" )
	{
		SAFEDELETE( m_pBlueIconBack );
		m_pBlueIconBack = pImage;
	}
	else if( szType == "BlueGaugeBack" )
	{
		SAFEDELETE( m_pBlueGaugeBack );
		m_pBlueGaugeBack = pImage;
	}
	else if( szType == "BlueGauge" )
	{
		SAFEDELETE( m_pBlueGauge );
		m_pBlueGauge = pImage;
	}
	else if( szType == "RedIconBack" )
	{
		SAFEDELETE( m_pRedIconBack );
		m_pRedIconBack = pImage;
	}
	else if( szType == "RedGaugeBack" )
	{
		SAFEDELETE( m_pRedGaugeBack );
		m_pRedGaugeBack = pImage;
	}
	else if( szType == "RedGauge" )
	{
		SAFEDELETE( m_pRedGauge );
		m_pRedGauge = pImage;
	}
	else if( szType == "WhiteGauge" )
	{
		SAFEDELETE( m_pWhiteGauge );
		m_pWhiteGauge = pImage;
	}
	else if( szType == "TextRank" )
	{
		SAFEDELETE( m_pTextRank );
		m_pTextRank = pImage;
	}
	else if( szType == "TextSurvivor" )
	{
		SAFEDELETE( m_pTextSurvivor );
		m_pTextSurvivor = pImage;
	}
	else if( szType == "BlueCrown" )
	{
		SAFEDELETE( m_pBlueCrown );
		m_pBlueCrown = pImage;
	}
	else if( szType == "BlueCrownText" )
	{
		SAFEDELETE( m_pBlueCrownText );
		m_pBlueCrownText = pImage;
	}
	else if( szType == "RedCrown" )
	{
		SAFEDELETE( m_pRedCrown );
		m_pRedCrown = pImage;
	}
	else if( szType == "RedCrownText" )
	{
		SAFEDELETE( m_pRedCrownText );
		m_pRedCrownText = pImage;
	}
	else if( szType == "BossIcon1" )
	{
		SAFEDELETE( m_pBossIcon[0] );
		m_pBossIcon[0] = pImage;
	}
	else if( szType == "BossIcon2" )
	{
		SAFEDELETE( m_pBossIcon[1] );
		m_pBossIcon[1] = pImage;
	}
	else if( szType == "BossIcon3" )
	{
		SAFEDELETE( m_pBossIcon[2] );
		m_pBossIcon[2] = pImage;
	}
	else if( szType == "BossIcon4" )
	{
		SAFEDELETE( m_pBossIcon[3] );
		m_pBossIcon[3] = pImage;
	}
	else if( szType == "BossIcon5" )
	{
		SAFEDELETE( m_pBossIcon[4] );
		m_pBossIcon[4] = pImage;
	}
	else if( szType == "BossIcon6" )
	{
		SAFEDELETE( m_pBossIcon[5] );
		m_pBossIcon[5] = pImage;
	}
	else if( szType == "BossIcon7" )
	{
		SAFEDELETE( m_pBossIcon[6] );
		m_pBossIcon[6] = pImage;
	}
	else if( szType == "BossIcon8" )
	{
		SAFEDELETE( m_pBossIcon[7] );
		m_pBossIcon[7] = pImage;
	}
	else if( szType == "BossIcon9" )
	{
		SAFEDELETE( m_pBossIcon[8] );
		m_pBossIcon[8] = pImage;
	}
	else if( szType == "BossIcon10" )
	{
		SAFEDELETE( m_pBossIcon[9] );
		m_pBossIcon[9] = pImage;
	}
	else if( szType == "BossText" )
	{
		SAFEDELETE( m_pBossText );
		m_pBossText = pImage;
	}
	else if( szType == "BlueStoneIcon" )
	{
		SAFEDELETE( m_pBlueStoneIcon );
		m_pBlueStoneIcon = pImage;
	}
	else if( szType == "RedStoneIcon" )
	{
		SAFEDELETE( m_pRedStoneIcon );
		m_pRedStoneIcon = pImage;
	}
	else if( szType == "KoText" )
	{
		SAFEDELETE( m_pKoText );
		m_pKoText = pImage;
	}
	else if( szType == "StoneGaugeBack" )
	{
		SAFEDELETE( m_pStoneGaugeBack );
		m_pStoneGaugeBack = pImage;
	}
	else if( szType == "StoneBlueGauge1" )
	{
		SAFEDELETE( m_pStoneBlueGauge1 );
		m_pStoneBlueGauge1 = pImage;
	}
	else if( szType == "StoneBlueGauge2" )
	{
		SAFEDELETE( m_pStoneBlueGauge2 );
		m_pStoneBlueGauge2 = pImage;
	}
	else if( szType == "StoneRedGauge1" )
	{
		SAFEDELETE( m_pStoneRedGauge1 );
		m_pStoneRedGauge1 = pImage;
	}
	else if( szType == "StoneRedGauge2" )
	{
		SAFEDELETE( m_pStoneRedGauge2 );
		m_pStoneRedGauge2 = pImage;
	}
	else if( szType == "StoneBlueWhiteGauge" )
	{
		SAFEDELETE( m_pStoneBlueWhiteGauge );
		m_pStoneBlueWhiteGauge = pImage;
	}
	else if( szType == "StoneRedWhiteGauge" )
	{
		SAFEDELETE( m_pStoneRedWhiteGauge );
		m_pStoneRedWhiteGauge = pImage;
	}
	else if( szType == "StoneAttack1" )
	{
		SAFEDELETE( m_pStoneAttack1 );
		m_pStoneAttack1 = pImage;
	}
	else if( szType == "StoneAttack2" )
	{
		SAFEDELETE( m_pStoneAttack2 );
		m_pStoneAttack2 = pImage;
	}
	else if( szType == "TextSec" )
	{
		SAFEDELETE( m_pTextSec );
		m_pTextSec = pImage;
	}
	else if( szType == "DiceRewardFailIcon" )
	{
		SAFEDELETE( m_pDiceRewardFailIcon );
		m_pDiceRewardFailIcon = pImage;
	}
	else if( szType == "DiceRewardIconBack" )
	{
		SAFEDELETE( m_pDiceRewardIconBack );
		m_pDiceRewardIconBack = pImage;
	}
	else if( szType == "DoubleBlueCrown" )
	{
		SAFEDELETE( m_pDoubleBlueCrown );
		m_pDoubleBlueCrown = pImage;
	}
	else if( szType == "DoubleRedCrown" )
	{
		SAFEDELETE( m_pDoubleRedCrown );
		m_pDoubleRedCrown = pImage;
	}
	else if( szType == "DoubleBlueCloseWin" )
	{		
		SAFEDELETE( m_pDoubleBlueCloseWin );
		m_pDoubleBlueCloseWin = pImage;
	}
	else if( szType == "DoubleRedCloseWin" )
	{		
		SAFEDELETE( m_pDoubleRedCloseWin );
		m_pDoubleRedCloseWin = pImage;
	}
	else if( szType == "DoubleCrownSimilar" )
	{
		SAFEDELETE( m_pDoubleCrownSimilar );
		m_pDoubleCrownSimilar = pImage;
	}
	else if( szType == "DoubleBlueWhiteGauge" )
	{
		SAFEDELETE( m_pDoubleBlueWhiteGauge );
		m_pDoubleBlueWhiteGauge = pImage;
	}
	else if( szType == "DoubleRedWhiteGauge" )
	{
		SAFEDELETE( m_pDoubleRedWhiteGauge );
		m_pDoubleRedWhiteGauge = pImage;
	}
	else if( szType == "FlagCaptureNewFlag" )
	{
		SAFEDELETE( m_pFlagCaptureNewFlag );
		m_pFlagCaptureNewFlag = pImage;
	}
	else if( szType == "FlagCaptureBlueReturn" )
	{
		SAFEDELETE( m_pFlagCaptureBlueReturn );
		m_pFlagCaptureBlueReturn = pImage;
	}
	else if( szType == "FlagCaptureRedReturn" )
	{
		SAFEDELETE( m_pFlagCaptureRedReturn );
		m_pFlagCaptureRedReturn = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CenterInfoWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "CenterInfoWnd2", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "CenterInfoWnd2", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();
}

void CenterInfoWnd::iwm_show()
{
}

void CenterInfoWnd::iwm_hide()
{
	BossRaidSound( false );
}

void CenterInfoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_BossRaidSound		= xElement.GetStringAttribute_e( "BossRaidSound" );
	m_GangsiLiveSound   = xElement.GetStringAttribute_e( "GangsiLiveSound" );
	m_dwReturnAlramDuration   = xElement.GetIntAttribute_e( "FlagReturnAlramTime" );
}

void CenterInfoWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
	}
}

void CenterInfoWnd::InitHiddenCrownAni()
{
	enum { DELAY_DROP = 250, DELAY_SUP1 = 100, DELAY_SUP2 = 100, DELAY_LOOP = 1000000000 };

	m_HiddenCrownAni.SetCurState( HCA_NONE );

	FSMState *pState = new FSMState( HCA_NONE );
	if( pState )
		pState->Add( HCA_NONE,  HCA_START );
	m_HiddenCrownAni.Add( pState );

	pState = new FSMState( HCA_START );
	if( pState )
		pState->Add( HCA_START,  HCA_DROP, 0 );
	m_HiddenCrownAni.Add( pState );

	pState = new FSMState( HCA_DROP );
	if( pState )
		pState->Add( HCA_DROP,  HCA_SUP1, DELAY_DROP );
	m_HiddenCrownAni.Add( pState );

	pState = new FSMState( HCA_SUP1 );
	if( pState )
		pState->Add( HCA_SUP1,  HCA_SUP2 , DELAY_SUP1 );
	m_HiddenCrownAni.Add( pState );	

	pState = new FSMState( HCA_SUP2 );
	if( pState )
		pState->Add( HCA_SUP2,  HCA_LOOP , DELAY_SUP2 );
	m_HiddenCrownAni.Add( pState );

	pState = new FSMState( HCA_LOOP );
	if( pState )
		pState->Add( HCA_LOOP,  HCA_NONE , DELAY_LOOP );
	m_HiddenCrownAni.Add( pState );
}

void CenterInfoWnd::InitBossRaidAni()
{
	enum { DELAY_DROP = 250, DELAY_SUP1 = 100, DELAY_SUP2 = 100, DELAY_LOOP = 1000000000 };

	m_BossRaidAni.SetCurState( BRA_NONE );

	FSMState *pState = new FSMState( BRA_NONE );
	if( pState )
		pState->Add( BRA_NONE,  BRA_START );
	m_BossRaidAni.Add( pState );

	pState = new FSMState( BRA_START );
	if( pState )
		pState->Add( BRA_START,  BRA_DROP, 0 );
	m_BossRaidAni.Add( pState );

	pState = new FSMState( BRA_DROP );
	if( pState )
		pState->Add( BRA_DROP,  BRA_SUP1, DELAY_DROP );
	m_BossRaidAni.Add( pState );

	pState = new FSMState( BRA_SUP1 );
	if( pState )
		pState->Add( BRA_SUP1,  BRA_SUP2 , DELAY_SUP1 );
	m_BossRaidAni.Add( pState );	

	pState = new FSMState( BRA_SUP2 );
	if( pState )
		pState->Add( BRA_SUP2,  BRA_LOOP , DELAY_SUP2 );
	m_BossRaidAni.Add( pState );

	pState = new FSMState( BRA_LOOP );
	if( pState )
		pState->Add( BRA_LOOP,  BRA_NONE , DELAY_LOOP );
	m_BossRaidAni.Add( pState );
}

void CenterInfoWnd::InitTeamDeathAni()
{
	enum { DELAY_ROLLING = 500, DELAY_TWINKLE = 2100, DELAY_MOVE_SCORE_UI = 350, WARNING_LOOP = 1000000000 };

	m_TeamDeathAni.SetCurState( TDA_NONE );

	FSMState *pState = new FSMState( TDA_NONE );
	if( pState )
		pState->Add( TDA_NONE,  TDA_START );
	m_TeamDeathAni.Add( pState );

	pState = new FSMState( TDA_START );
	if( pState )
		pState->Add( TDA_START,  TDA_ROLLING, 0 );
	m_TeamDeathAni.Add( pState );

	pState = new FSMState( TDA_ROLLING );
	if( pState )
		pState->Add( TDA_ROLLING,  TDA_TWINKLE, DELAY_ROLLING );
	m_TeamDeathAni.Add( pState );

	pState = new FSMState( TDA_TWINKLE );
	if( pState )
		pState->Add( TDA_TWINKLE,  TDA_MOVE_SCORE_UI , DELAY_TWINKLE );
	m_TeamDeathAni.Add( pState );	

	pState = new FSMState( TDA_MOVE_SCORE_UI );
	if( pState )
		pState->Add( TDA_MOVE_SCORE_UI,  TDA_NONE , DELAY_MOVE_SCORE_UI );
	m_TeamDeathAni.Add( pState );

	pState = new FSMState( TDA_WARNING_SCORE );
	if( pState )
		pState->Add( TDA_WARNING_SCORE,  TDA_NONE , WARNING_LOOP );
	m_TeamDeathAni.Add( pState );
}

void CenterInfoWnd::InitDeathMatchAni()
{
	enum { DELAY_ROLLING = 500, DELAY_TWINKLE = 2100, DELAY_MOVE_SCORE_UI = 350, WARNING_LOOP = 1000000000 };

	m_DeathMatchAni.SetCurState( DMA_NONE );

	FSMState *pState = new FSMState( DMA_NONE );
	if( pState )
		pState->Add( DMA_NONE,  DMA_START );
	m_DeathMatchAni.Add( pState );

	pState = new FSMState( DMA_START );
	if( pState )
		pState->Add( DMA_START,  DMA_ROLLING, 0 );
	m_DeathMatchAni.Add( pState );

	pState = new FSMState( DMA_ROLLING );
	if( pState )
		pState->Add( DMA_ROLLING,  DMA_TWINKLE, DELAY_ROLLING );
	m_DeathMatchAni.Add( pState );

	pState = new FSMState( DMA_TWINKLE );
	if( pState )
		pState->Add( DMA_TWINKLE,  DMA_MOVE_SCORE_UI , DELAY_TWINKLE );
	m_DeathMatchAni.Add( pState );	

	pState = new FSMState( DMA_MOVE_SCORE_UI );
	if( pState )
		pState->Add( DMA_MOVE_SCORE_UI, DMA_NONE , DELAY_MOVE_SCORE_UI );
	m_DeathMatchAni.Add( pState );

	pState = new FSMState( DMA_WARNING_TIME );
	if( pState )
		pState->Add( DMA_WARNING_TIME, DMA_NONE , WARNING_LOOP );
	m_DeathMatchAni.Add( pState );
}

void CenterInfoWnd::InitDeathTimeAni()
{
	enum { DELAY_FADEIN = 150, DELAY_VIBRATION = 1490, DELAY_FADEOUT = 150, };

	m_DeathTimeAni.SetCurState( DTA_NONE );

	FSMState *pState = new FSMState( DTA_NONE );
	if( pState )
		pState->Add( DTA_NONE,  DTA_START );
	m_DeathTimeAni.Add( pState );

	pState = new FSMState( DTA_START );
	if( pState )
		pState->Add( DTA_START,  DTA_FADEIN, 0 );
	m_DeathTimeAni.Add( pState );

	pState = new FSMState( DTA_FADEIN );
	if( pState )
		pState->Add( DTA_FADEIN,  DTA_VIBRATION, DELAY_FADEIN );
	m_DeathTimeAni.Add( pState );

	pState = new FSMState( DTA_VIBRATION );
	if( pState )
		pState->Add( DTA_VIBRATION,  DTA_FADEOUT , DELAY_VIBRATION );
	m_DeathTimeAni.Add( pState );	

	pState = new FSMState( DTA_FADEOUT );
	if( pState )
		pState->Add( DTA_FADEOUT,  DTA_NONE , DELAY_FADEOUT );
	m_DeathTimeAni.Add( pState );

	// 떨림 좌표
	enum { LOOP = 6, };
	for(int i = 0;i < LOOP;i++)
	{
		tagDeathTimeData kData;
		kData.m_iXPos = kData.m_iYPos = 0;
		kData.m_bScreen = true;
		m_DeathTimeData.push_back( kData );

		kData.m_iXPos = 2;
		kData.m_iYPos = 0;
		kData.m_bScreen = false;
		m_DeathTimeData.push_back( kData );

		kData.m_iXPos = -1;
		kData.m_iYPos = 0;
		kData.m_bScreen = false;
		m_DeathTimeData.push_back( kData );

		kData.m_iXPos = -1;
		kData.m_iYPos = 2;
		kData.m_bScreen = true;
		m_DeathTimeData.push_back( kData );

		kData.m_iXPos = -1;
		kData.m_iYPos = -1;
		kData.m_bScreen = false;
		m_DeathTimeData.push_back( kData );

		kData.m_iXPos = 0;
		kData.m_iYPos = 1;
		kData.m_bScreen = false;
		m_DeathTimeData.push_back( kData );

		kData.m_iXPos = -2;
		kData.m_iYPos = 0;
		kData.m_bScreen = true;
		m_DeathTimeData.push_back( kData );

		kData.m_iXPos = 1;
		kData.m_iYPos = -1;
		kData.m_bScreen = false;
		m_DeathTimeData.push_back( kData );

		kData.m_iXPos = -2;
		kData.m_iYPos = 1;
		kData.m_bScreen = false;
		m_DeathTimeData.push_back( kData );
	}
}

void CenterInfoWnd::InitSymbolGaugeAni()
{
	enum { DELAY_DROP = 250, DELAY_SUP1 = 100, DELAY_SUP2 = 100, DELAY_LOOP = 1000000000 };

	m_SymbolGaugeAni.SetCurState( SGA_NONE );

	FSMState *pState = new FSMState( SGA_NONE );
	if( pState )
		pState->Add( SGA_NONE, SGA_START );
	m_SymbolGaugeAni.Add( pState );

	pState = new FSMState( SGA_START );
	if( pState )
		pState->Add( SGA_START, SGA_DROP, 0 );
	m_SymbolGaugeAni.Add( pState );

	pState = new FSMState( SGA_DROP );
	if( pState )
		pState->Add( SGA_DROP, SGA_SUP1, DELAY_DROP );
	m_SymbolGaugeAni.Add( pState );

	pState = new FSMState( SGA_SUP1 );
	if( pState )
		pState->Add( SGA_SUP1, SGA_SUP2 , DELAY_SUP1 );
	m_SymbolGaugeAni.Add( pState );	

	pState = new FSMState( SGA_SUP2 );
	if( pState )
		pState->Add( SGA_SUP2, SGA_LOOP , DELAY_SUP2 );
	m_SymbolGaugeAni.Add( pState );

	pState = new FSMState( SGA_LOOP );
	if( pState )
		pState->Add( SGA_LOOP, SGA_NONE , DELAY_LOOP );
	m_SymbolGaugeAni.Add( pState );
}

void CenterInfoWnd::InitGangsiModeAni()
{
	enum { DELAY_ROLLING = 500, DELAY_TWINKLE = 2100, DELAY_MOVE_SCORE_UI = 350, };

	m_GangsiModeAni.SetCurState( GMA_NONE );

	FSMState *pState = new FSMState( GMA_NONE );
	if( pState )
		pState->Add( GMA_NONE,  GMA_START );
	m_GangsiModeAni.Add( pState );

	pState = new FSMState( GMA_START );
	if( pState )
		pState->Add( GMA_START,  GMA_ROLLING, 0 );
	m_GangsiModeAni.Add( pState );

	pState = new FSMState( GMA_ROLLING );
	if( pState )
		pState->Add( GMA_ROLLING,  GMA_TWINKLE, DELAY_ROLLING );
	m_GangsiModeAni.Add( pState );

	pState = new FSMState( GMA_TWINKLE );
	if( pState )
		pState->Add( GMA_TWINKLE,  GMA_MOVE_SCORE_UI , DELAY_TWINKLE );
	m_GangsiModeAni.Add( pState );	

	pState = new FSMState( GMA_MOVE_SCORE_UI );
	if( pState )
		pState->Add( GMA_MOVE_SCORE_UI,  GMA_NONE , DELAY_MOVE_SCORE_UI );
	m_GangsiModeAni.Add( pState );
}

void CenterInfoWnd::InitGangsiAlarmAni()
{
	enum { DELAY_FADEIN = 150, DELAY_VIBRATION = 1000000000, DELAY_FADEOUT = 150, };

	m_GangsiAlarmAni.SetCurState( GAA_NONE );

	FSMState *pState = new FSMState( GAA_NONE );
	if( pState )
		pState->Add( GAA_NONE,  GAA_START );
	m_GangsiAlarmAni.Add( pState );

	pState = new FSMState( GAA_START );
	if( pState )
		pState->Add( GAA_START,  GAA_FADEIN, 0 );
	m_GangsiAlarmAni.Add( pState );

	pState = new FSMState( GAA_FADEIN );
	if( pState )
		pState->Add( GAA_FADEIN,  GAA_VIBRATION, DELAY_FADEIN );
	m_GangsiAlarmAni.Add( pState );

	pState = new FSMState( GAA_VIBRATION );
	if( pState )
		pState->Add( GAA_VIBRATION,  GAA_FADEOUT , DELAY_VIBRATION );
	m_GangsiAlarmAni.Add( pState );	

	pState = new FSMState( GAA_FADEOUT );
	if( pState )
		pState->Add( GAA_FADEOUT,  GAA_NONE , DELAY_FADEOUT );
	m_GangsiAlarmAni.Add( pState );

	// 떨림 좌표
	enum { LOOP = 6, };
	for(int i = 0;i < LOOP;i++)
	{
		tagGangsiAlarmData kData;
		kData.m_iXPos = kData.m_iYPos = 0;
		kData.m_bScreen = true;
		m_GangsiAlarmData.push_back( kData );

		kData.m_iXPos = 2;
		kData.m_iYPos = 0;
		kData.m_bScreen = false;
		m_GangsiAlarmData.push_back( kData );

		kData.m_iXPos = -1;
		kData.m_iYPos = 0;
		kData.m_bScreen = false;
		m_GangsiAlarmData.push_back( kData );

		kData.m_iXPos = -1;
		kData.m_iYPos = 2;
		kData.m_bScreen = true;
		m_GangsiAlarmData.push_back( kData );

		kData.m_iXPos = -1;
		kData.m_iYPos = -1;
		kData.m_bScreen = false;
		m_GangsiAlarmData.push_back( kData );

		kData.m_iXPos = 0;
		kData.m_iYPos = 1;
		kData.m_bScreen = false;
		m_GangsiAlarmData.push_back( kData );

		kData.m_iXPos = -2;
		kData.m_iYPos = 0;
		kData.m_bScreen = true;
		m_GangsiAlarmData.push_back( kData );

		kData.m_iXPos = 1;
		kData.m_iYPos = -1;
		kData.m_bScreen = false;
		m_GangsiAlarmData.push_back( kData );

		kData.m_iXPos = -2;
		kData.m_iYPos = 1;
		kData.m_bScreen = false;
		m_GangsiAlarmData.push_back( kData );
	}
}

void CenterInfoWnd::InitDungeonAModeAni()
{
	enum { DELAY_FADEIN = 150, DELAY_INFINITY = 1000000000, DELAY_FADEOUT = 1000000000, };

	m_DungeonAModeAni.SetCurState( MDA_NONE );

	FSMState *pState = new FSMState( MDA_NONE );
	if( pState )
		pState->Add( MDA_NONE,  MDA_START );
	m_DungeonAModeAni.Add( pState );

	pState = new FSMState( MDA_START );
	if( pState )
		pState->Add( MDA_START, MDA_INFINITY, 0 );
	m_DungeonAModeAni.Add( pState );

	pState = new FSMState( MDA_INFINITY );
	if( pState )
		pState->Add( MDA_INFINITY, MDA_FADEOUT, DELAY_INFINITY );
	m_DungeonAModeAni.Add( pState );

	pState = new FSMState( MDA_FADEOUT );
	if( pState )
		pState->Add( MDA_FADEOUT,  MDA_NONE , DELAY_FADEOUT );
	m_DungeonAModeAni.Add( pState );	
}

void CenterInfoWnd::InitDoubleCrownAni()
{
	enum { DELAY_DROP = 250, DELAY_SUP1 = 100, DELAY_SUP2 = 100, DELAY_LOOP = 1000000000 };

	m_DoubleCrownAni.SetCurState( DCA_NONE );

	FSMState *pState = new FSMState( DCA_NONE );
	if( pState )
		pState->Add( DCA_NONE, DCA_START );
	m_DoubleCrownAni.Add( pState );

	pState = new FSMState( DCA_START );
	if( pState )
		pState->Add( DCA_START, DCA_DROP, 0 );
	m_DoubleCrownAni.Add( pState );

	pState = new FSMState( DCA_DROP );
	if( pState )
		pState->Add( DCA_DROP, DCA_SUP1, DELAY_DROP );
	m_DoubleCrownAni.Add( pState );

	pState = new FSMState( DCA_SUP1 );
	if( pState )
		pState->Add( DCA_SUP1, DCA_SUP2 , DELAY_SUP1 );
	m_DoubleCrownAni.Add( pState );	

	pState = new FSMState( DCA_SUP2 );
	if( pState )
		pState->Add( DCA_SUP2, DCA_LOOP , DELAY_SUP2 );
	m_DoubleCrownAni.Add( pState );

	pState = new FSMState( DCA_LOOP );
	if( pState )
		pState->Add( DCA_LOOP, DCA_NONE , DELAY_LOOP );
	m_DoubleCrownAni.Add( pState );
}

void CenterInfoWnd::InitFlagCaptureAni()
{
	enum { DELAY_ROLLING = 500, DELAY_TWINKLE = 2100, DELAY_MOVE_SCORE_UI = 350, WARNING_LOOP = 1000000000 };

	m_FlagCaptureAni.SetCurState( FCA_NONE );

	FSMState *pState = new FSMState( FCA_NONE );
	if( pState )
		pState->Add( FCA_NONE,  FCA_START );
	m_FlagCaptureAni.Add( pState );

	pState = new FSMState( FCA_START );
	if( pState )
		pState->Add( FCA_START,  FCA_ROLLING, 0 );
	m_FlagCaptureAni.Add( pState );

	pState = new FSMState( FCA_ROLLING );
	if( pState )
		pState->Add( FCA_ROLLING,  FCA_TWINKLE, DELAY_ROLLING );
	m_FlagCaptureAni.Add( pState );

	pState = new FSMState( FCA_TWINKLE );
	if( pState )
		pState->Add( FCA_TWINKLE,  FCA_MOVE_SCORE_UI , DELAY_TWINKLE );
	m_FlagCaptureAni.Add( pState );	

	pState = new FSMState( FCA_MOVE_SCORE_UI );
	if( pState )
		pState->Add( FCA_MOVE_SCORE_UI,  FCA_NONE , DELAY_MOVE_SCORE_UI );
	m_FlagCaptureAni.Add( pState );

	pState = new FSMState( FCA_WARNING_SCORE );
	if( pState )
		pState->Add( FCA_WARNING_SCORE,  FCA_NONE , WARNING_LOOP );
	m_FlagCaptureAni.Add( pState );

	pState = new FSMState( FCA_REMAIN_TIME );
	if( pState )
		pState->Add( FCA_REMAIN_TIME,  FCA_NONE , WARNING_LOOP );
	m_FlagCaptureAni.Add( pState );

	m_dwNewFlagDuration = DELAY_ROLLING + DELAY_TWINKLE + DELAY_MOVE_SCORE_UI;
}

void CenterInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );

	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ) )
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			int iYPos = GetYPos();
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ) )
				m_iDefaultY = iYPos;
		}
	}

	switch( ioPlayMode::GetModeType() )
	{
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
		ProcessTeamDeath();
		break;
	case MT_SURVIVAL:
	case MT_BATTLE:
	case MT_SHUFFLE_BONUS:
		ProcessDeathMatch();
		break;
	case MT_KING:
		ProcessHiddenCrown();
		break;
	case MT_BOSS:
		ProcessBossRaid();
		break;
	case MT_SYMBOL:
		ProcessSymbolGauge();
		break;
	case MT_GANGSI:
		ProcessGangsiAlarm();
		ProcessGangsiMode();
		break;
	case MT_DUNGEON_A:
		ProcessDungeonAMode();
		break;
	case MT_DOUBLE_CROWN:
		ProcessDoubleCrownGauge();
		ProcessDoubleCrownEffect();
		break;
	case MT_FLAG_CAPTURE:
		ProcessFlagCapture();
		break;
	}

	ProcessDeathTime();
}

void CenterInfoWnd::ProcessHiddenCrown()
{
	if( m_HiddenCrownAni.GetCurState() == HCA_NONE )
		return;

	m_HiddenCrownAni.UpdateTime();
	switch( m_HiddenCrownAni.GetCurState() )
	{
	case HCA_DROP:
		{
			float fRate = cosf( ( D3DX_PI / 2 ) * (float)( m_HiddenCrownAni.GetCurTime() ) / m_HiddenCrownAni.GetDelay() );

			m_iAddYPos = -35.0f * fRate;
			m_iWhiteColor = 255.0f * fRate;
		}
		break;
	case HCA_SUP1:
		{
			float fRate = sinf( ( D3DX_PI / 2 ) * (float)( m_HiddenCrownAni.GetCurTime() ) / m_HiddenCrownAni.GetDelay() );

			m_iAddYPos = -8.0f * fRate;
			m_iWhiteColor = 0;
		}
		break;
	case HCA_SUP2:
		{
			float fRate = cosf( ( D3DX_PI / 2 ) * (float)( m_HiddenCrownAni.GetCurTime() ) / m_HiddenCrownAni.GetDelay() );

			m_iAddYPos = -8.0f * fRate;
			m_iWhiteColor = 0;
		}
		break;
	case HCA_LOOP:
		{
			m_iAddYPos = 0;			
			m_iWhiteColor = 0;
			int iAlphaDelay, iAlphaRateTime, iEffectDelay, iEffectLoopTime;
			iAlphaDelay = iAlphaRateTime = iEffectDelay = iEffectLoopTime = 1;
			DWORD dwGapTime = FRAMEGETTIME() - m_dwCrownHoldTime;
			if( dwGapTime > ( (float)m_dwGivePointTime * m_fScoreWarningRate ) )
			{
				// 70% 남았다 - 0.5초마다 330동안 90% - > 0 알파 + 1초마다 이펙트 100ms x 10
				iAlphaDelay = 500;
				iAlphaRateTime = 330;
				iEffectDelay = 1000;
				iEffectLoopTime = 100;
			}
			else
			{				
				// 70% 이상 남았다 - 2초마다 330동안 90% - > 0 알파 + 2초마다 이펙트 100ms x 10
				iAlphaDelay = 2000;
				iAlphaRateTime = 330;
				iEffectDelay = 2000;
				iEffectLoopTime = 100;				
			}

			dwGapTime = m_HiddenCrownAni.GetCurTime() % iAlphaDelay;
			if( dwGapTime < (DWORD)iAlphaRateTime )
			{
				m_fAlphaRate = 0.90f * max( 0.0f, ( FLOAT1 - (float)dwGapTime / iAlphaRateTime ) );
			}
			else
			{
				m_fAlphaRate = 0.0f;
			}
	
			dwGapTime = m_HiddenCrownAni.GetCurTime() % iEffectDelay;
			m_iEffectArray = (float)dwGapTime / iEffectLoopTime;
		}
		break;
	}
}

void CenterInfoWnd::ProcessBossRaid()
{
	if( m_BossRaidAni.GetCurState() == BRA_NONE )
		return;

	m_BossRaidAni.UpdateTime();
	switch( m_BossRaidAni.GetCurState() )
	{
	case BRA_DROP:
		{
			float fRate = cosf( ( D3DX_PI / 2 ) * (float)( m_BossRaidAni.GetCurTime() ) / m_BossRaidAni.GetDelay() );

			m_iAddYPos = -35.0f * fRate;
			m_iWhiteColor = 255.0f * fRate;
		}
		break;
	case BRA_SUP1:
		{
			float fRate = sinf( ( D3DX_PI / 2 ) * (float)( m_BossRaidAni.GetCurTime() ) / m_BossRaidAni.GetDelay() );

			m_iAddYPos = -8.0f * fRate;
			m_iWhiteColor = 0;
		}
		break;
	case BRA_SUP2:
		{
			float fRate = cosf( ( D3DX_PI / 2 ) * (float)( m_BossRaidAni.GetCurTime() ) / m_BossRaidAni.GetDelay() );

			m_iAddYPos = -8.0f * fRate;
			m_iWhiteColor = 0;
		}
		break;
	case BRA_LOOP:
		{
			m_iAddYPos = 0;			
			m_iWhiteColor = 0;
		
			if( m_dwBossRaidTime > 0 )
			{
				enum { ALPHA_TIME = 330, EFFECT_TIME = 100, };
				DWORD dwGapTime = FRAMEGETTIME() - m_dwBossRaidTime;
				if( m_iBossRaidOrder == 0 )
				{
					if( dwGapTime > 1000 )
					{
						m_dwBossRaidTime = FRAMEGETTIME();
						m_fAlphaRate = 0.0f;
						m_iEffectArray = -1;
					}
					else
					{
						dwGapTime = min( dwGapTime, 1000 );
						DWORD dwAlphaTime = dwGapTime % 500;
						m_fAlphaRate = 0.90f * max( 0.0f, ( FLOAT1 - (float)dwAlphaTime / 330 ) );  // 알파는 0.5초마다
 						m_iEffectArray = (float)dwGapTime / 100;	                              // 이펙트는 1초마다
					}
				}
				else
				{				
					if( dwGapTime > 1000 )
					{
						m_dwBossRaidTime = 0;
						m_fAlphaRate = 0.0f;
						m_iEffectArray = -1;
					}
					else
					{
						m_fAlphaRate = 0.90f * max( 0.0f, ( FLOAT1 - (float)dwGapTime / 330 ) );
						m_iEffectArray = (float)dwGapTime / 100;	
					}
				}				
			}
			else
			{
				m_fAlphaRate = 0.0f;
				m_iEffectArray = -1;
			}
		}
		break;
	}
}

void CenterInfoWnd::ProcessSymbolGauge()
{
	if( m_SymbolGaugeAni.GetCurState() == SGA_NONE )
		return;

	m_SymbolGaugeAni.UpdateTime();
	switch( m_SymbolGaugeAni.GetCurState() )
	{
	case SGA_DROP:
		{
			float fRate = cosf( ( D3DX_PI / 2 ) * (float)( m_SymbolGaugeAni.GetCurTime() ) / m_SymbolGaugeAni.GetDelay() );

			m_iAddYPos = -35.0f * fRate;
			m_iWhiteColor = 255.0f * fRate;
		}
		break;
	case SGA_SUP1:
		{
			float fRate = sinf( ( D3DX_PI / 2 ) * (float)( m_SymbolGaugeAni.GetCurTime() ) / m_SymbolGaugeAni.GetDelay() );

			m_iAddYPos = -8.0f * fRate;
			m_iWhiteColor = 0;
		}
		break;
	case SGA_SUP2:
		{
			float fRate = cosf( ( D3DX_PI / 2 ) * (float)( m_SymbolGaugeAni.GetCurTime() ) / m_SymbolGaugeAni.GetDelay() );

			m_iAddYPos = -8.0f * fRate;
			m_iWhiteColor = 0;
		}
		break;
	case SGA_LOOP:
		{
			m_iAddYPos = 0;		
			m_iWhiteColor = 0;

			const float fGaugeTick = FLOAT1000;
			if( m_fBlueGaugeDownTime > 0.0f )
			{
				float fGapTime = FRAMEGETTIME() - m_fBlueGaugeDownTime;
				if( m_fPrevBlueSymbolRate > m_fBlueSymbolRate )
				{					
					m_fPrevBlueSymbolRate = max( m_fBlueSymbolRate, m_fPrevBlueSymbolRate - ( 0.005f * ( fGapTime / fGaugeTick ) ) );				
				}
				m_fBlueGaugeAlpha = 0.90f * max( 0.0f, ( FLOAT1 - fGapTime / 150.0f ) );
			}

			if( m_fRedGaugeDownTime > 0.0f )
			{
				float fGapTime = FRAMEGETTIME() - m_fRedGaugeDownTime;
				if( m_fPrevRedSymbolRate > m_fRedSymbolRate )
				{					
					m_fPrevRedSymbolRate = max( m_fRedSymbolRate, m_fPrevRedSymbolRate - ( 0.005f * ( fGapTime / fGaugeTick ) ) );				
				}
				m_fRedGaugeAlpha = 0.90f * max( 0.0f, ( FLOAT1 - fGapTime / 150.0f ) );
			}
			
			int iSymbolVibration[5] = { -3, 3, -2, 2, 0, };
			if( m_bBlueSymbolAttack )
			{
				DWORD dwGapTime = FRAMEGETTIME() - m_fBlueSymbolAttackTime;
				int iVibArray = dwGapTime / 35;
				if( COMPARE( iVibArray, 0, 5 ) )
					m_iBlueSymbolVibration = iSymbolVibration[iVibArray];
				
				if( dwGapTime > 150 )
				{
					m_bBlueSymbolAttack = false;
				}
			}
			else
			{
				m_iBlueSymbolVibration = 0;
			}

			if( m_bRedSymbolAttack )
			{
				DWORD dwGapTime = FRAMEGETTIME() - m_fRedSymbolAttackTime;
				int iVibArray = dwGapTime / 35;
				if( COMPARE( iVibArray, 0, 5 ) )
					m_iRedSymbolVibration = iSymbolVibration[iVibArray];

				if( dwGapTime > 150 )
				{
					m_bRedSymbolAttack = false;
				}
			}
			else
			{
				m_iRedSymbolVibration = 0;
			}
		}
		break;
	}
}

void CenterInfoWnd::ProcessTeamDeath()
{
	if( m_TeamDeathAni.GetCurState() == TDA_NONE ) 
		return;

	m_TeamDeathAni.UpdateTime();
	switch( m_TeamDeathAni.GetCurState() )
	{
	case TDA_ROLLING:
		{
			m_fTimeRate = (float)m_TeamDeathAni.GetCurTime() / m_TeamDeathAni.GetDelay();
		}
		break;
	case TDA_TWINKLE:
	case TDA_WARNING_SCORE:
		{
			// 롤링 끝
			m_iCurrentBluePoint += m_iAddBluePoint;
			m_iCurrentRedPoint  += m_iAddRedPoint;
			m_iAddBluePoint      = m_iAddRedPoint = 0;
			
			// 알파
			enum { ROTATION = 175, };
			int iRotateCount = m_TeamDeathAni.GetCurTime() / ROTATION;
			DWORD dwCurTime  = m_TeamDeathAni.GetCurTime() % ROTATION;
			
			if( iRotateCount % 2 == 0 )
			{	
				m_fTimeRate = (float)dwCurTime / ROTATION;
			}
			else
			{	
				m_fTimeRate = FLOAT1 - (float)dwCurTime / ROTATION;
			}
		}
		break;
	case TDA_MOVE_SCORE_UI:
		{
			if( m_bTeamDeathWarningScore )
			{
				m_TeamDeathAni.SetCurState( TDA_WARNING_SCORE );	
			}
			else
			{
				enum { MOVE_TIME = 350, };
				m_fAlphaRate = cosf( ( D3DX_PI / 2 ) * (float)( m_TeamDeathAni.GetCurTime() ) / MOVE_TIME );

				TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd*>(g_GUIMgr.FindWnd( TIME_SCORE_WND ));
				if( pTimeScoreWnd )
				{
					int iStartXPos = GetXPos() + (GetWidth() / 2);
					int iStartYPos = GetYPos() + (GetHeight() / 2);
					int iEndXPos   = pTimeScoreWnd->GetXPos() + 84;
					int iEndYPos   = pTimeScoreWnd->GetYPos() + 96;

					m_iAddXPos = -( (iStartXPos - iEndXPos) * ( FLOAT1 - m_fAlphaRate ) );
					m_iAddYPos = -( (iStartYPos - iEndYPos) * ( FLOAT1 - m_fAlphaRate ) );
				}
			}
		}
		break;
	case TDA_NONE:
		{
			HideWnd();
			TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd*>(g_GUIMgr.FindWnd( TIME_SCORE_WND ));
			if( pTimeScoreWnd )
			{
				pTimeScoreWnd->SetNumberTwinkle( m_bBlueScoreUP );
			}
		}
		break;
	}
}

void CenterInfoWnd::ProcessDeathMatch()
{
	if( m_DeathMatchAni.GetCurState() == DMA_NONE ) 
		return;

	m_DeathMatchAni.UpdateTime();
	switch( m_DeathMatchAni.GetCurState() )
	{
	case DMA_ROLLING:
		{
			float fTimeRate = (float)( m_DeathMatchAni.GetCurTime() ) / m_DeathMatchAni.GetDelay();
			
			m_fPrevRankRate = cosf( ( D3DX_PI / 2 ) * fTimeRate );       
			m_fCurRankRate  = sinf( ( D3DX_PI / 2 ) * fTimeRate );
		}
		break;
	case DMA_TWINKLE:
	case DMA_WARNING_TIME:
		{
			m_fPrevRankRate = 0.0f;
			m_fCurRankRate  = FLOAT1;
			// 알파
			enum { ROTATION = 175, };
			int iRotateCount = m_DeathMatchAni.GetCurTime() / ROTATION;
			DWORD dwCurTime  = m_DeathMatchAni.GetCurTime() % ROTATION;

			if( iRotateCount % 2 == 0 )
			{	
				m_fTimeRate = (float)dwCurTime / ROTATION;
			}
			else
			{	
				m_fTimeRate = FLOAT1 - (float)dwCurTime / ROTATION;
			}
		}
		break;
	case DMA_MOVE_SCORE_UI:
		{
			if( m_bDeathMatchTimeWarning )
			{
				m_DeathMatchAni.SetCurState( DMA_WARNING_TIME );	
			}
			else
			{
				enum { MOVE_TIME = 350, };
				m_fAlphaRate = cosf( ( D3DX_PI / 2 ) * (float)( m_DeathMatchAni.GetCurTime() ) / MOVE_TIME );

				TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd*>(g_GUIMgr.FindWnd( TIME_SCORE_WND ));
				if( pTimeScoreWnd )
				{
					int iStartXPos = GetXPos() + (GetWidth() / 2);
					int iStartYPos = GetYPos() + (GetHeight() / 2);
					int iEndXPos   = pTimeScoreWnd->GetXPos() + 84;
					int iEndYPos   = pTimeScoreWnd->GetYPos() + 96;

					m_iAddXPos = -( (iStartXPos - iEndXPos) * ( FLOAT1 - m_fAlphaRate ) );
					m_iAddYPos = -( (iStartYPos - iEndYPos) * ( FLOAT1 - m_fAlphaRate ) );
				}
			}
		}
		break;
	case DMA_NONE:
		{
			HideWnd();
			TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd*>(g_GUIMgr.FindWnd( TIME_SCORE_WND ));
			if( pTimeScoreWnd )
			{
				pTimeScoreWnd->SetNumberTwinkle( true );
			}
		}
		break;
	}
}

void CenterInfoWnd::ProcessDeathTime()
{
	if( m_DeathTimeAni.GetCurState() == DTA_NONE )
		return;

	m_DeathTimeAni.UpdateTime();
	switch( m_DeathTimeAni.GetCurState() )
	{
	case DTA_FADEIN:
		{
			m_bDeathTimeScreen = false;
			m_fTimeRate = cosf( ( D3DX_PI / 2 ) * (float)( m_DeathTimeAni.GetCurTime() ) / m_DeathTimeAni.GetDelay() );
			m_fAlphaRate= FLOAT1 - m_fTimeRate;
		}
		break;
	case DTA_VIBRATION:
		{
			m_fTimeRate  = 0.0f;
			m_fAlphaRate = FLOAT1;

			enum { ARRAY_TIME = 33, };
			int iVibrationArray = m_DeathTimeAni.GetCurTime() / ARRAY_TIME;
			if( COMPARE( iVibrationArray, 0, (int)m_DeathTimeData.size() ) )
			{
				m_iAddXPos = m_DeathTimeData[iVibrationArray].m_iXPos;
				m_iAddYPos = m_DeathTimeData[iVibrationArray].m_iYPos;
				m_bDeathTimeScreen = m_DeathTimeData[iVibrationArray].m_bScreen;
			}            
		}
		break;
	case DTA_FADEOUT:
		{
			m_bDeathTimeScreen = false;
			m_fTimeRate = sinf( ( D3DX_PI / 2 ) * (float)( m_DeathTimeAni.GetCurTime() ) / m_DeathTimeAni.GetDelay() );
			m_fAlphaRate= FLOAT1 - m_fTimeRate;
		}
		break;
	case DTA_NONE:
		{
			if( m_bNoHideWnd )
			{
				m_fTimeRate = 0.0f;
				m_fAlphaRate= FLOAT1;
				m_iAddXPos = m_iAddYPos = 0;

				m_TeamDeathAni.SetCurState( m_ePrevTeamDeathAni );
				m_DeathMatchAni.SetCurState( m_ePrevDeathMatchAni );
				m_FlagCaptureAni.SetCurState( m_ePrevFlagCaptureAni );
			}
			else
			{
				HideWnd();
			}
		}
		break;
	}
}

void CenterInfoWnd::ProcessGangsiMode()
{
	if( m_GangsiModeAni.GetCurState() == GMA_NONE ) 
		return;

	m_GangsiModeAni.UpdateTime();
	switch( m_GangsiModeAni.GetCurState() )
	{
	case GMA_ROLLING:
		{
			float fTimeRate = (float)( m_GangsiModeAni.GetCurTime() ) / m_GangsiModeAni.GetDelay(); 
			m_fPrevRankRate = cosf( ( D3DX_PI / 2 ) * fTimeRate );
			m_fCurRankRate  = sinf( ( D3DX_PI / 2 ) * fTimeRate );
		}
		break;
	case GMA_TWINKLE:
		{
			m_fPrevRankRate = 0.0f;
			m_fCurRankRate  = FLOAT1;

			// 알파
			enum { ROTATION = 175, };
			int iRotateCount = m_GangsiModeAni.GetCurTime() / ROTATION;
			DWORD dwCurTime  = m_GangsiModeAni.GetCurTime() % ROTATION;

			if( iRotateCount % 2 == 0 )
			{	
				m_fTimeRate = (float)dwCurTime / ROTATION;
			}
			else
			{	
				m_fTimeRate = FLOAT1 - (float)dwCurTime / ROTATION;
			}
		}
		break;
	case GMA_MOVE_SCORE_UI:
		{
			enum { MOVE_TIME = 350, };
			m_fAlphaRate = cosf( ( D3DX_PI / 2 ) * (float)( m_GangsiModeAni.GetCurTime() ) / MOVE_TIME );

			TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd*>(g_GUIMgr.FindWnd( TIME_SCORE_WND ));
			if( pTimeScoreWnd )
			{
				int iStartXPos = GetXPos() + (GetWidth() / 2);
				int iStartYPos = GetYPos() + (GetHeight() / 2);
				int iEndXPos   = pTimeScoreWnd->GetXPos() + 84;
				int iEndYPos   = pTimeScoreWnd->GetYPos() + 96;

				m_iAddXPos = -( (iStartXPos - iEndXPos) * ( FLOAT1 - m_fAlphaRate ) );
				m_iAddYPos = -( (iStartYPos - iEndYPos) * ( FLOAT1 - m_fAlphaRate ) );
			}
		}
		break;
	case GMA_NONE:
		{
			HideWnd();
			TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd*>(g_GUIMgr.FindWnd( TIME_SCORE_WND ));
			if( pTimeScoreWnd )
			{
				pTimeScoreWnd->SetNumberTwinkle( false );
			}
		}
		break;
	}
}

void CenterInfoWnd::ProcessGangsiAlarm()
{
	if( m_GangsiAlarmAni.GetCurState() == GAA_NONE )
		return;

	m_GangsiAlarmAni.UpdateTime();
	switch( m_GangsiAlarmAni.GetCurState() )
	{
	case GAA_FADEIN:
		{
			m_bGangsiAlarmScreen = false;
			m_fTimeRate = cosf( ( D3DX_PI / 2 ) * (float)( m_GangsiAlarmAni.GetCurTime() ) / m_GangsiAlarmAni.GetDelay() );
			m_fAlphaRate= FLOAT1 - m_fTimeRate;
		}
		break;
	case GAA_VIBRATION:
		{
			m_fTimeRate  = 0.0f;
			m_fAlphaRate = FLOAT1;

			enum { ARRAY_TIME = 33, };
			int iVibrationArray = m_GangsiAlarmAni.GetCurTime() / ARRAY_TIME;
			if( !COMPARE( iVibrationArray, 0, (int)m_GangsiAlarmData.size() ) )
				iVibrationArray = iVibrationArray % (int)m_GangsiAlarmData.size();

			m_iAddXPos = m_GangsiAlarmData[iVibrationArray].m_iXPos;
			m_iAddYPos = m_GangsiAlarmData[iVibrationArray].m_iYPos;
			m_bGangsiAlarmScreen = m_GangsiAlarmData[iVibrationArray].m_bScreen;

			if( m_dwGangsiAlarmTickTime > m_dwGangsiAlarmWarningTime || m_dwGangsiAlarmTickTime == 0 )
			{
				enum { VIBRATION_TIME = 1490, };
				if( m_GangsiAlarmAni.GetCurTime() > VIBRATION_TIME )
				{
					m_GangsiAlarmAni.SetCurState( GAA_FADEOUT );
				}
			}
		}
		break;
	case GAA_FADEOUT:
		{
			m_bGangsiAlarmScreen = false;
			m_fTimeRate = sinf( ( D3DX_PI / 2 ) * (float)( m_GangsiAlarmAni.GetCurTime() ) / m_GangsiAlarmAni.GetDelay() );
			m_fAlphaRate= FLOAT1 - m_fTimeRate;
		}
		break;
	case GAA_NONE:
		{
			HideWnd();
		}
		break;
	}
}

void CenterInfoWnd::ProcessDungeonAMode()
{
	if( m_DungeonAModeAni.GetCurState() == MDA_NONE ) 
		return;

	m_DungeonAModeAni.UpdateTime();
	switch( m_DungeonAModeAni.GetCurState() )
	{
	case MDA_INFINITY:
		{
			enum { ROTATION = 200, };
			m_iPrizeRollingArray = m_DungeonAModeAni.GetCurTime() / ROTATION;
		}
		break;
	case MDA_FADEOUT:
		{
			enum { ROTATION = 250, };
			m_iPrizeRollingArray = m_DungeonAModeAni.GetCurTime() / ROTATION;
			if( m_iPrizeRollingArray % 2 == 0 )
				m_bPrizeScreen = false;
			else
				m_bPrizeScreen = true;

			//
			if( m_DungeonAModeAni.GetCurTime() > 1500 )
			{
				m_iPrizeRollingArray = 0;
				m_bPrizeScreen       = false;
				m_szPrizeWinner.Clear();
				if( !m_MonsterDiceList.empty() )
				{
					m_MonsterDiceList.erase( m_MonsterDiceList.begin() );
				}

				if( m_MonsterDiceList.empty() )
					m_DungeonAModeAni.SetCurState( MDA_NONE );
				else
				{
					MonsterDiceData &kDiceData = *m_MonsterDiceList.begin();
					SAFEDELETE( m_pDicePrizeIcon );
					SAFEDELETE( m_pDiceSubIcon );
					m_pDicePrizeIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentIconText( kDiceData.m_iPresentType, kDiceData.m_iPresentValue1, kDiceData.m_iPresentValue2 ) );
					m_pDiceSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( kDiceData.m_iPresentType, kDiceData.m_iPresentValue1, kDiceData.m_iPresentValue2 ) );
					m_DungeonAModeAni.SetCurState( MDA_INFINITY );
				}
			}
		}
		break;
	case MDA_NONE:
		{
			HideWnd();
		}
		break;
	}
}

void CenterInfoWnd::ProcessDoubleCrownGauge()
{
	if( m_DoubleCrownAni.GetCurState() == DCA_NONE )
		return;

	m_DoubleCrownAni.UpdateTime();
	switch( m_DoubleCrownAni.GetCurState() )
	{
	case DCA_DROP:
		{
			float fRate = cosf( ( D3DX_PI / 2 ) * (float)( m_DoubleCrownAni.GetCurTime() ) / m_DoubleCrownAni.GetDelay() );

			m_iAddYPos = -35.0f * fRate;
			m_iWhiteColor = 255.0f * fRate;
		}
		break;
	case DCA_SUP1:
		{
			float fRate = sinf( ( D3DX_PI / 2 ) * (float)( m_DoubleCrownAni.GetCurTime() ) / m_DoubleCrownAni.GetDelay() );

			m_iAddYPos = -8.0f * fRate;
			m_iWhiteColor = 0;
		}
		break;
	case DCA_SUP2:
		{
			float fRate = cosf( ( D3DX_PI / 2 ) * (float)( m_DoubleCrownAni.GetCurTime() ) / m_DoubleCrownAni.GetDelay() );

			m_iAddYPos = -8.0f * fRate;
			m_iWhiteColor = 0;
		}
		break;
	case DCA_LOOP:
		{
			m_iAddYPos = 0;
			m_iWhiteColor = 0;

			const float fGaugeTick = FLOAT1000;
			if( m_fDoubleBlueGaugeDownTime > 0.0f )
			{
				float fGapTime = FRAMEGETTIME() - m_fDoubleBlueGaugeDownTime;
				if( m_fDoublePrevBlueRate > m_fDoubleBlueRate )
				{					
					m_fDoublePrevBlueRate = max( m_fDoubleBlueRate, m_fDoublePrevBlueRate - ( 0.005f * ( fGapTime / fGaugeTick ) ) );				
				}
				m_fDoubleBlueGaugeAlpha = 0.90f * max( 0.0f, ( FLOAT1 - fGapTime / 150.0f ) );
			}

			if( m_fDoubleRedGaugeDownTime > 0.0f )
			{
				float fGapTime = FRAMEGETTIME() - m_fDoubleRedGaugeDownTime;
				if( m_fDoublePrevRedRate > m_fDoubleRedRate )
				{					
					m_fDoublePrevRedRate = max( m_fDoubleRedRate, m_fDoublePrevRedRate - ( 0.005f * ( fGapTime / fGaugeTick ) ) );				
				}
				m_fDoubleRedGaugeAlpha = 0.90f * max( 0.0f, ( FLOAT1 - fGapTime / 150.0f ) );
			}

			int iVibration[5] = { -3, 3, -2, 2, 0, };
			if( m_bDoubleBlueAttack )
			{
				DWORD dwGapTime = FRAMEGETTIME() - m_fDoubleBlueAttackTime;
				int iVibArray = dwGapTime / 35;
				if( COMPARE( iVibArray, 0, 5 ) )
					m_iDoubleBlueVibration = iVibration[iVibArray];

				if( dwGapTime > 150 )
				{
					m_bDoubleBlueAttack = false;
				}
			}
			else
			{
				m_iDoubleBlueVibration = 0;
			}

			if( m_bDoubleRedAttack )
			{
				DWORD dwGapTime = FRAMEGETTIME() - m_fDoubleRedAttackTime;
				int iVibArray = dwGapTime / 35;
				if( COMPARE( iVibArray, 0, 5 ) )
					m_iDoubleRedVibration = iVibration[iVibArray];

				if( dwGapTime > 150 )
				{
					m_bDoubleRedAttack = false;
				}
			}
			else
			{
				m_iDoubleRedVibration = 0;
			}
		}
		break;
	}
}
void CenterInfoWnd::ProcessDoubleCrownEffect()
{
	if( m_DoubleCrownAni.GetCurState() != DCA_LOOP )
		return;
	
	int iBlueEffectDelay    = -1;
	int iBlueEffectLoopTime = -1;
	int iRedEffectDelay     = -1;
	int iRedEffectLoopTime  = -1;

	if( !m_bBlueKing )
	{
		if( m_fDoubleBlueRate < FLOAT05 )
		{
			iBlueEffectDelay    = 1000;
			iBlueEffectLoopTime = 100;
		}
		else
		{
			iBlueEffectDelay    = 2000;
			iBlueEffectLoopTime = 100;
		}

		DWORD dwGapTime = m_DoubleCrownAni.GetCurTime() % iBlueEffectDelay;
		m_iBlueEffectArray = (float)dwGapTime / iBlueEffectLoopTime;
	}
	else
	{
		m_iBlueEffectArray = -1;
	}

	if( !m_bRedKing )
	{
		if( m_fDoubleRedRate < FLOAT05 )
		{
			iRedEffectDelay    = 1000;
			iRedEffectLoopTime = 100;
		}
		else
		{
			iRedEffectDelay    = 2000;
			iRedEffectLoopTime = 100;
		}

		DWORD dwGapTime = m_DoubleCrownAni.GetCurTime() % iRedEffectDelay;
		m_iRedEffectArray = (float)dwGapTime / iRedEffectLoopTime;
	}
	else
	{
		m_iRedEffectArray = -1;
	}
}

void CenterInfoWnd::ProcessFlagCapture()
{
	if( m_FlagCaptureAni.GetCurState() == FCA_NONE ) 
		return;

	m_FlagCaptureAni.UpdateTime();
	switch( m_FlagCaptureAni.GetCurState() )
	{
	case FCA_ROLLING:
		{
			m_fTimeRate = (float)m_FlagCaptureAni.GetCurTime() / m_FlagCaptureAni.GetDelay();
		}
		break;
	case FCA_TWINKLE:
	case FCA_WARNING_SCORE:
		{
			// 롤링 끝
			m_iFlagBluePoint += m_iFlagAddBluePoint;
			m_iFlagRedPoint  += m_iFlagAddRedPoint;
			m_iFlagAddBluePoint      = m_iFlagAddRedPoint = 0;

			// 알파
			enum { ROTATION = 175, };
			int iRotateCount = m_FlagCaptureAni.GetCurTime() / ROTATION;
			DWORD dwCurTime  = m_FlagCaptureAni.GetCurTime() % ROTATION;

			if( iRotateCount % 2 == 0 )
			{	
				m_fTimeRate = (float)dwCurTime / ROTATION;
			}
			else
			{	
				m_fTimeRate = FLOAT1 - (float)dwCurTime / ROTATION;
			}
		}
		break;
	case FCA_REMAIN_TIME:
		{
			if( m_dwFlagWaitRemainTime == 0 )
			{
				if( m_bFlagCaptureWarningScore )
					m_FlagCaptureAni.SetCurState( FCA_WARNING_SCORE );	
				else
					m_FlagCaptureAni.SetCurState( FCA_NONE );
			}
		}
		break;
	case FCA_MOVE_SCORE_UI:
		{
			if( m_bFlagNoPointChange )
				return;

			if( m_bFlagCaptureWarningScore )
			{
#ifndef SHIPPING
				LOG.PrintConsoleLog( 0, "CenterInfoWnd::ProcessFlagCapture - m_dwFlagWaitRemainTime = %d", m_dwFlagWaitRemainTime );
#endif
				if( m_dwFlagWaitRemainTime > 0 )
				{
					m_szFlagKiller.Clear();
					m_szFlagDier.Clear();
					m_szFlagCapturer.Clear();
					m_szFlagDroper.Clear();
					m_szFlagCarrier.Clear();

					m_FlagCaptureAni.SetCurState( FCA_REMAIN_TIME );
				}
				else
					m_FlagCaptureAni.SetCurState( FCA_WARNING_SCORE );	
			}
			else
			{
				enum { MOVE_TIME = 350, };
				m_fAlphaRate = cosf( ( D3DX_PI / 2 ) * (float)( m_FlagCaptureAni.GetCurTime() ) / MOVE_TIME );

				TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd*>(g_GUIMgr.FindWnd( TIME_SCORE_WND ));
				if( pTimeScoreWnd )
				{
					int iStartXPos = GetXPos() + (GetWidth() / 2);
					int iStartYPos = GetYPos() + (GetHeight() / 2);
					int iEndXPos   = pTimeScoreWnd->GetXPos() + 84;
					int iEndYPos   = pTimeScoreWnd->GetYPos() + 96;

					m_iAddXPos = -( (iStartXPos - iEndXPos) * ( FLOAT1 - m_fAlphaRate ) );
					m_iAddYPos = -( (iStartYPos - iEndYPos) * ( FLOAT1 - m_fAlphaRate ) );
				}
			}
		}
		break;
	case FCA_NONE:
		{
			HideWnd();
			TimeScoreWnd *pTimeScoreWnd = dynamic_cast<TimeScoreWnd*>(g_GUIMgr.FindWnd( TIME_SCORE_WND ));
			if( pTimeScoreWnd && !m_bFlagNoPointChange )
			{
				pTimeScoreWnd->SetNumberTwinkle( m_bFlagBlueScoreUP );
			}

			m_szFlagKiller.Clear();
			m_szFlagDier.Clear();
			m_szFlagCapturer.Clear();
			m_szFlagDroper.Clear();
			m_szFlagCarrier.Clear();

			if( m_dwFlagWaitRemainTime > 0 )
			{
				m_fTimeRate = 0.0f;
				m_fAlphaRate= FLOAT1;
				m_iAddXPos = m_iAddYPos = 0;
				m_FlagCaptureAni.SetCurState( FCA_REMAIN_TIME );

				ShowWnd();
			}
		}
		break;
	}

	if( m_eFlagReturn > FR_NEW_FLAG )
	{
		if( REALGETTIME() >= m_dwReturnAlramTime + m_dwReturnAlramDuration )
			m_eFlagReturn = FR_NONE;
	}
	else if( m_eFlagReturn == FR_NEW_FLAG )
	{
		if( REALGETTIME() >= m_dwNewFlagTime + m_dwNewFlagDuration )
			m_eFlagReturn = FR_NONE;
	}
}

void CenterInfoWnd::OnRender() 
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX() + ( GetWidth() / 2 );
	iYPos = GetDerivedPosY() + ( GetHeight() / 2 );

	if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		RenderSuddenDeathTime( iXPos, iYPos );		
	}
	else
	{
		switch( ioPlayMode::GetModeType() )
		{
		case MT_TEAM_SURVIVAL:
		case MT_TEAM_SURVIVAL_AI:
			RenderTeamDeath( iXPos + m_iAddXPos, iYPos + m_iAddYPos );
			break;
		case MT_SURVIVAL:
		case MT_BATTLE:
		case MT_SHUFFLE_BONUS:
			RenderDeathMatch( iXPos + m_iAddXPos, iYPos + m_iAddYPos );
			break;
		case MT_KING:
			RenderHiddenCrown( iXPos, iYPos );
			break;
		case MT_BOSS:
			RenderBossRaid( iXPos, iYPos );
			break;
		case MT_SYMBOL:
			RenderSymbolGauge( iXPos, iYPos );
			break;
		case MT_GANGSI:
			RenderGangsiAlarm( iXPos, iYPos );
			RenderGangsiMode( iXPos + m_iAddXPos, iYPos + m_iAddYPos );
			break;
		case MT_DUNGEON_A:
			RenderDungeonAMode( iXPos, iYPos );
			break;
		case MT_DOUBLE_CROWN:
			RenderDoubleCrownGauge( iXPos, iYPos );
			break;
		case MT_FLAG_CAPTURE:
			RenderFlagCapture( iXPos + m_iAddXPos, iYPos + m_iAddYPos );
			break;
		}

		RenderDeathTime( iXPos, iYPos );
	}	
}

void CenterInfoWnd::RenderHiddenCrown( int iXPos, int iYPos )
{
	if( m_HiddenCrownAni.GetCurState() == HCA_NONE || m_HiddenCrownAni.GetCurState() == HCA_START ) return;	

	ioUIRenderImage *pCrownIcon = m_pBlueCrown;
	ioUIRenderImage *pCrownIconBack = m_pBlueIconBack;
	ioUIRenderImage *pCrownGaugeBack = m_pBlueGaugeBack;
	ioUIRenderImage *pCrownGauge = m_pBlueGauge;
	ioUIRenderImage *pCrownText = m_pBlueCrownText;

	if( m_eKingTeam == TEAM_RED )
	{
		pCrownIcon = m_pRedCrown;
		pCrownIconBack = m_pRedIconBack;
		pCrownGaugeBack = m_pRedGaugeBack;
		pCrownGauge = m_pRedGauge;
		pCrownText = m_pRedCrownText;
	}

	if( !pCrownIcon || !pCrownIconBack || !pCrownGaugeBack || !pCrownGauge || !pCrownText )
		return;

	// 게이지 
	{
		pCrownGaugeBack->SetColor( 255, 255, 255 );
		pCrownGaugeBack->Render( iXPos - 108, (iYPos - 23) + m_iAddYPos );
		if( m_iWhiteColor > 0 )
		{
			pCrownGaugeBack->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			pCrownGaugeBack->Render( iXPos - 108, (iYPos - 23) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR );
		}

		if( m_dwGivePointTime > 0 )
		{
			pCrownGauge->SetColor( 255, 255, 255 );
			float fTimeRate = min( FLOAT1, (float)(FRAMEGETTIME() - m_dwCrownHoldTime) / m_dwGivePointTime );
			pCrownGauge->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, pCrownGauge->GetWidth() * fTimeRate );
			if( m_iWhiteColor > 0 )
			{
				pCrownGauge->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
				pCrownGauge->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, pCrownGauge->GetWidth() * fTimeRate, UI_RENDER_NORMAL_ADD_COLOR );
			}

			if( m_fAlphaRate >= 0.0f )
			{
				if( m_pWhiteGauge )
				{
					m_pWhiteGauge->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
					m_pWhiteGauge->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, m_pWhiteGauge->GetWidth() * fTimeRate );
				}
			}
		}
	}
	
	//
	{
		pCrownText->Render( iXPos, (iYPos - 16) + m_iAddYPos, UI_RENDER_ADD );
	}

	// 왕관
	{
		pCrownIconBack->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );
		pCrownIcon->SetScale( 0.66f );
		pCrownIcon->SetColor( 255, 255, 255 );
		pCrownIcon->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_iWhiteColor > 0 )
		{
			pCrownIcon->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			pCrownIcon->Render(  iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
		}
	}

	// 이펙트
	{
		if( COMPARE( m_iEffectArray, 0, MAX_EFFECT ) )
		{
			if( m_eKingTeam == TEAM_RED )
			{
				if( m_pRedEffectSpr[m_iEffectArray] )
					m_pRedEffectSpr[m_iEffectArray]->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );
			}
			else
			{
				if( m_pBlueEffectSpr[m_iEffectArray] )
					m_pBlueEffectSpr[m_iEffectArray]->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );
			}
		}
	}

	// Line
	if( m_pCenterLine )
	{
		m_pCenterLine->SetAlpha( MAX_ALPHA_RATE );
		m_pCenterLine->Render( iXPos - 109, iYPos + 12 );
	}

	// KO
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	if( m_eKingTeam == m_eOwnerTeam )
	{
		// 우리팀 왕관
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(1) );

	}
	else if( m_eOwnerTeam == TEAM_NONE )
	{
		// 관전자
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(2) );
	}
	else
	{
		// 상대팀 왕관
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(3) );
	}
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER, MAX_ALPHA_RATE );
}

void CenterInfoWnd::RenderBossRaid( int iXPos, int iYPos )
{
	if( m_BossRaidAni.GetCurState() == BRA_NONE || m_BossRaidAni.GetCurState() == BRA_START ) return;
	if( !m_pRedIconBack || !m_pRedGaugeBack || !m_pRedGauge || !m_pBossText )
		return;

	if( m_iBossRaidMaxOrder == 0 )
		return;

	// 게이지 
	{
		m_pRedGaugeBack->SetColor( 255, 255, 255 );
		m_pRedGaugeBack->Render( iXPos - 108, (iYPos - 23) + m_iAddYPos );
		if( m_iWhiteColor > 0 )
		{
			m_pRedGaugeBack->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			m_pRedGaugeBack->Render( iXPos - 108, (iYPos - 23) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR );
		}
		float fTimeRate = FLOAT1;
		if( m_iBossRaidOrder > 0 )
			fTimeRate = min( FLOAT1, (float)(m_iBossRaidMaxOrder - m_iBossRaidOrder) / m_iBossRaidMaxOrder );
		m_pRedGauge->SetColor( 255, 255, 255 );
		m_pRedGauge->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, m_pBlueGauge->GetWidth() * fTimeRate );
		if( m_iWhiteColor > 0 )
		{
			m_pRedGauge->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			m_pRedGauge->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, m_pBlueGauge->GetWidth() * fTimeRate, UI_RENDER_NORMAL_ADD_COLOR );
		}

		if( m_fAlphaRate >= 0.0f )
		{
			if( m_pWhiteGauge )
			{
				m_pWhiteGauge->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
				m_pWhiteGauge->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, m_pWhiteGauge->GetWidth() * fTimeRate );
			}
		}
	}

	//
	{
		m_pBossText->Render( iXPos, (iYPos - 16) + m_iAddYPos, UI_RENDER_ADD );
	}


	// 아이콘
	{
		m_pRedIconBack->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );

		int iIconArray = m_iBossRaidMyLevel - 1;
		if( COMPARE( iIconArray, 0 , MAX_BOSSICON ) )
		{
			if( m_pBossIcon[iIconArray] )
			{
				m_pBossIcon[iIconArray]->SetScale( 0.66f );
				m_pBossIcon[iIconArray]->SetColor( 255, 255, 255 );
				m_pBossIcon[iIconArray]->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
				if( m_iWhiteColor > 0 )
				{
					m_pBossIcon[iIconArray]->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
					m_pBossIcon[iIconArray]->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );

				}
			}
		}
	}

	// 이펙트
	{
		if( COMPARE( m_iEffectArray, 0, MAX_EFFECT ) )
		{
			if( m_pRedEffectSpr[m_iEffectArray] )
				m_pRedEffectSpr[m_iEffectArray]->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );
		}
	}

	// Line
	if( m_pCenterLine )
	{
		m_pCenterLine->SetAlpha( MAX_ALPHA_RATE );
		m_pCenterLine->Render( iXPos - 109, iYPos + 12 );
	}

	// TEXT
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_14, STR(1) );
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER, MAX_ALPHA_RATE );
}

void CenterInfoWnd::RenderTeamDeath( int iXPos, int iYPos )
{
	if( m_TeamDeathAni.GetCurState() == TDA_NONE || m_TeamDeathAni.GetCurState() == TDA_START ) return;
	if( !m_pBlueNum || !m_pRedNum || !m_pWhiteNum || !m_pNumColon || !m_pCenterLine ) return;

	// Blue Score
	int iBluePoint = m_iCurrentBluePoint + ( (float)m_iAddBluePoint * m_fTimeRate );
	m_pBlueNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	if( iBluePoint < 100 )
	{
		m_pBlueNum->RenderNum( iXPos - 86, iYPos - 36, iBluePoint / 10 );
		m_pBlueNum->RenderNum( iXPos - 51, iYPos - 36, iBluePoint % 10 );

		if( ( m_bBlueScoreUP && m_TeamDeathAni.GetCurState() == TDA_TWINKLE ) || m_bTeamDeathWarningScore )
		{
			m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
			m_pWhiteNum->RenderNum( iXPos - 86, iYPos - 36, iBluePoint / 10 );
			m_pWhiteNum->RenderNum( iXPos - 51, iYPos - 36, iBluePoint % 10 );
		}
	}
	else
	{
		m_pBlueNum->RenderNum( iXPos - 121, iYPos - 36, iBluePoint / 100 );
		m_pBlueNum->RenderNum( iXPos - 86, iYPos - 36, (iBluePoint % 100) / 10 );
		m_pBlueNum->RenderNum( iXPos - 51, iYPos - 36, iBluePoint % 10 );		

		if( ( m_bBlueScoreUP && m_TeamDeathAni.GetCurState() == TDA_TWINKLE ) || m_bTeamDeathWarningScore )
		{
			m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
			m_pWhiteNum->RenderNum( iXPos - 121, iYPos - 36, iBluePoint / 100 );
			m_pWhiteNum->RenderNum( iXPos - 86, iYPos - 36, (iBluePoint % 100) / 10 );
			m_pWhiteNum->RenderNum( iXPos - 51, iYPos - 36, iBluePoint % 10 );		
		}
	}

	// Red Score 
	int iRedPoint = m_iCurrentRedPoint + ( (float)m_iAddRedPoint * m_fTimeRate );
	m_pRedNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	if( iRedPoint < 100 )
	{
		m_pRedNum->RenderNum( iXPos + 10, iYPos - 36, iRedPoint / 10 );
		m_pRedNum->RenderNum( iXPos + 45, iYPos - 36, iRedPoint % 10 );

		if( ( !m_bBlueScoreUP && m_TeamDeathAni.GetCurState() == TDA_TWINKLE ) || m_bTeamDeathWarningScore )
		{
			m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
			m_pWhiteNum->RenderNum( iXPos + 10, iYPos - 36, iRedPoint / 10 );
			m_pWhiteNum->RenderNum( iXPos + 45, iYPos - 36, iRedPoint % 10 );
		}
	}
	else
	{
		m_pRedNum->RenderNum( iXPos + 10, iYPos - 36, iRedPoint / 100 );
		m_pRedNum->RenderNum( iXPos + 45, iYPos - 36, (iRedPoint % 100) / 10 );
		m_pRedNum->RenderNum( iXPos + 80, iYPos - 36, iRedPoint % 10 );		

		if( ( !m_bBlueScoreUP && m_TeamDeathAni.GetCurState() == TDA_TWINKLE ) || m_bTeamDeathWarningScore )
		{
			m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
			m_pWhiteNum->RenderNum( iXPos + 10, iYPos - 36, iRedPoint / 100 );
			m_pWhiteNum->RenderNum( iXPos + 45, iYPos - 36, (iRedPoint % 100) / 10 );
			m_pWhiteNum->RenderNum( iXPos + 80, iYPos - 36, iRedPoint % 10 );		
		}
	}

	// Colon
	m_pNumColon->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	m_pNumColon->Render( iXPos - 6, iYPos - 27 );

	// Line
	m_pCenterLine->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	m_pCenterLine->Render( iXPos - 109, iYPos + 12 );

	// KO
	if( m_TeamDeathAni.GetCurState() == TDA_WARNING_SCORE )
	{
		ioComplexStringPrinter kPrinter;	
		kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
		kPrinter.SetBkColor( 0, 0, 0 );
		if( m_iCurrentBluePoint > m_iCurrentRedPoint )
		{
			kPrinter.SetTextColor( 0, 150, 255 );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(4) );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(5) );
		}
		else if( m_iCurrentBluePoint < m_iCurrentRedPoint )
		{
			kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(6) );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(7) );
		}
		else
		{
			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(8) );
		}
		kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER, (float)MAX_ALPHA_RATE * m_fAlphaRate );
	}
	else
	{
		ioComplexStringPrinter kPrinter;	
		kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
		if( m_szKiller.IsEmpty() )
		{
			kPrinter.SetBkColor( 0, 0, 0 );
			if( m_eDieTeam == TEAM_BLUE )
				kPrinter.SetTextColor( 0, 150, 255 );
			else
				kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_14, m_szDier.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(1) );
		}
		else
		{
			kPrinter.SetBkColor( 0, 0, 0 );
			if( m_eKillTeam == TEAM_BLUE )
				kPrinter.SetTextColor( 0, 150, 255 );
			else
				kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_14, m_szKiller.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(2) );

			if( m_eDieTeam == TEAM_BLUE )
				kPrinter.SetTextColor( 0, 150, 255 );
			else
				kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_14, m_szDier.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(3) );
		}
		kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER, (float)MAX_ALPHA_RATE * m_fAlphaRate );
	}
}

void CenterInfoWnd::RenderDeathMatch( int iXPos, int iYPos )
{
	if( m_DeathMatchAni.GetCurState() == DMA_NONE || m_DeathMatchAni.GetCurState() == DMA_START ) return;
	if( !m_pBlueNum || !m_pWhiteNum || !m_pTextRank || !m_pCenterLine ) return;

	if( m_fPrevRankRate > 0.0f )
	{
		// Prev Rank
		int iCurrentXPos = FLOAT100 * (1.0f - m_fPrevRankRate);
		if( m_iPrevRank < m_iCurRank )    // 거꾸로 간다.
			iCurrentXPos = - ( FLOAT100 * (1.0f - m_fPrevRankRate));
		m_pBlueNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fPrevRankRate );
		m_pTextRank->SetAlpha( (float)MAX_ALPHA_RATE * m_fPrevRankRate );
		if( m_iPrevRank < 10 )
		{
			m_pBlueNum->RenderNum( (iXPos - 39) + iCurrentXPos, iYPos - 36, m_iPrevRank );
			if( m_DeathMatchAni.GetCurState() == DMA_TWINKLE || m_DeathMatchAni.GetCurState() == DMA_WARNING_TIME )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( (iXPos - 39) + iCurrentXPos, iYPos - 36, m_iPrevRank );
			}
			m_pTextRank->Render( (iXPos - 1) + iCurrentXPos, iYPos - 36 );
		}
		else
		{
			m_pBlueNum->RenderNum( (iXPos - 57) + iCurrentXPos, iYPos - 36, m_iPrevRank / 10 );
			m_pBlueNum->RenderNum( (iXPos - 22) + iCurrentXPos, iYPos - 36, m_iPrevRank % 10 );
			if( m_DeathMatchAni.GetCurState() == DMA_TWINKLE || m_DeathMatchAni.GetCurState() == DMA_WARNING_TIME )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( (iXPos - 57) + iCurrentXPos, iYPos - 36, m_iPrevRank / 10 );
				m_pWhiteNum->RenderNum( (iXPos - 22) + iCurrentXPos, iYPos - 36, m_iPrevRank % 10 );
			}
			m_pTextRank->Render( (iXPos + 16) + iCurrentXPos, iYPos - 36 );
		}
	}

	{
		// Current Rank
		int iCurrentXPos = -( FLOAT100 * (1.0f - m_fCurRankRate));
		if( m_iPrevRank < m_iCurRank )    // 거꾸로 간다.
			iCurrentXPos = FLOAT100 * (1.0f - m_fCurRankRate);
		m_pBlueNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurRankRate );
		m_pTextRank->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurRankRate );
		if( m_iCurRank < 10 )
		{
			m_pBlueNum->RenderNum( (iXPos - 39) + iCurrentXPos, iYPos - 36, m_iCurRank );
			if( m_DeathMatchAni.GetCurState() == DMA_TWINKLE || m_DeathMatchAni.GetCurState() == DMA_WARNING_TIME )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( (iXPos - 39) + iCurrentXPos, iYPos - 36, m_iCurRank );
			}
			m_pTextRank->Render( (iXPos - 1) + iCurrentXPos, iYPos - 36 );
		}
		else
		{
			m_pBlueNum->RenderNum( (iXPos - 57) + iCurrentXPos, iYPos - 36, m_iCurRank / 10 );
			m_pBlueNum->RenderNum( (iXPos - 22) + iCurrentXPos, iYPos - 36, m_iCurRank % 10 );
			if( m_DeathMatchAni.GetCurState() == DMA_TWINKLE || m_DeathMatchAni.GetCurState() == DMA_WARNING_TIME )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( (iXPos - 57) + iCurrentXPos, iYPos - 36, m_iCurRank / 10 );
				m_pWhiteNum->RenderNum( (iXPos - 22) + iCurrentXPos, iYPos - 36, m_iCurRank % 10 );
			}
			m_pTextRank->Render( (iXPos + 16) + iCurrentXPos, iYPos - 36 );
		}
	}

	// Line
	m_pCenterLine->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	m_pCenterLine->Render( iXPos - 109, iYPos + 12 );

	//
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( 0, 150, 255 );
	kPrinter.AddTextPiece( FONT_SIZE_14, STR(1), g_MyInfo.GetPublicID().c_str() );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_14, STR(2) );
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER, (float)MAX_ALPHA_RATE * m_fAlphaRate );
}

void CenterInfoWnd::RenderDeathTime( int iXPos, int iYPos )
{
	if( m_DeathTimeAni.GetCurState() == DTA_NONE || m_DeathTimeAni.GetCurState() == DTA_START ) return;
	if( !m_pDeathTimeText || !m_pCenterLine ) return;

	// TEXT Resource
	m_pDeathTimeText->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	m_pDeathTimeText->SetScale( FLOAT1 + (3.0f * m_fTimeRate) );
    m_pDeathTimeText->Render( iXPos + m_iAddXPos, ( iYPos - 14 ) + m_iAddYPos );
	if( m_bDeathTimeScreen )
		m_pDeathTimeText->Render( iXPos + m_iAddXPos, ( iYPos - 14 ) + m_iAddYPos, UI_RENDER_SCREEN );

	// Line
	m_pCenterLine->SetAlpha( MAX_ALPHA_RATE );
	m_pCenterLine->Render( iXPos - 109, iYPos + 12 );

	// 
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	if( ioPlayMode::GetModeType() == MT_FOOTBALL )
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(1) );
	else
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(2) );
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER );
}

void CenterInfoWnd::RenderSuddenDeathTime( int iXPos, int iYPos )
{
	if( m_DeathTimeAni.GetCurState() == DTA_NONE || m_DeathTimeAni.GetCurState() == DTA_START ) return;
	if( !m_pSuddenDeathTimeText || !m_pCenterLine ) return;

	// TEXT Resource
	m_pSuddenDeathTimeText->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	m_pSuddenDeathTimeText->SetScale( FLOAT1 + (3.0f * m_fTimeRate) );
	m_pSuddenDeathTimeText->Render( iXPos + m_iAddXPos, ( iYPos - 14 ) + m_iAddYPos );
	if( m_bDeathTimeScreen )
		m_pSuddenDeathTimeText->Render( iXPos + m_iAddXPos, ( iYPos - 14 ) + m_iAddYPos, UI_RENDER_SCREEN );

	// Line
	m_pCenterLine->SetAlpha( MAX_ALPHA_RATE );
	m_pCenterLine->Render( iXPos - 109, iYPos + 12 );

	// 
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_14, "서든데스에서 결판나지 않으면 기여도로 승패 판정" );
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER );
}

void CenterInfoWnd::RenderSymbolGauge( int iXPos, int iYPos )
{	
	if( m_SymbolGaugeAni.GetCurState() == SGA_NONE || m_SymbolGaugeAni.GetCurState() == SGA_START ) return;

	// 게이지 
	if( m_pStoneGaugeBack )
	{
		m_pStoneGaugeBack->SetColor( 255, 255, 255 );
		m_pStoneGaugeBack->Render( iXPos - 108, (iYPos - 23) + m_iAddYPos );
		if( m_iWhiteColor > 0 )
		{
			m_pStoneGaugeBack->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			m_pStoneGaugeBack->Render( iXPos - 108, (iYPos - 23) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR );
		}
		// blue
		if( m_pStoneBlueGauge1 && m_pStoneBlueGauge2 && m_pStoneBlueWhiteGauge )
		{
			m_pStoneBlueGauge2->SetColor( 255, 255, 255 );
			float fWidth = min( m_pStoneBlueGauge2->GetWidth(), m_pStoneBlueGauge2->GetWidth() * m_fPrevBlueSymbolRate );
			m_pStoneBlueGauge2->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, fWidth );

			fWidth = min( m_pStoneBlueGauge1->GetWidth(), m_pStoneBlueGauge1->GetWidth() * m_fBlueSymbolRate );
			m_pStoneBlueGauge1->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, fWidth );

			if( m_iWhiteColor > 0 )
			{
				m_pStoneBlueGauge2->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
				m_pStoneBlueGauge2->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, fWidth, UI_RENDER_NORMAL_ADD_COLOR );
			}
			if( m_fBlueGaugeAlpha > 0.0f )
			{
				m_pStoneBlueWhiteGauge->SetAlpha( (float)MAX_ALPHA_RATE * m_fBlueGaugeAlpha );
				fWidth = min( m_pStoneBlueWhiteGauge->GetWidth(), m_pStoneBlueWhiteGauge->GetWidth() * m_fBlueSymbolRate );
				m_pStoneBlueWhiteGauge->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, fWidth );
			}
		}

		// red
		if( m_pStoneRedGauge1 && m_pStoneRedGauge2 && m_pStoneRedWhiteGauge )
		{
			m_pStoneRedGauge2->SetColor( 255, 255, 255 );
			float fWidth = min( m_pStoneRedGauge2->GetWidth(), m_pStoneRedGauge2->GetWidth() * ( FLOAT1 - m_fPrevRedSymbolRate ) );
			m_pStoneRedGauge2->RenderWidthCut( iXPos, (iYPos - 23) + m_iAddYPos, fWidth, m_pStoneBlueGauge2->GetWidth() );

			fWidth = min( m_pStoneRedGauge1->GetWidth(), m_pStoneRedGauge1->GetWidth() * ( FLOAT1 - m_fRedSymbolRate ) );
			m_pStoneRedGauge1->RenderWidthCut( iXPos, (iYPos - 23) + m_iAddYPos, fWidth, m_pStoneRedGauge1->GetWidth() );
			if( m_iWhiteColor > 0 )
			{
				m_pStoneRedGauge2->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
				m_pStoneRedGauge2->RenderWidthCut( iXPos, (iYPos - 23) + m_iAddYPos, fWidth, m_pStoneBlueGauge2->GetWidth(), UI_RENDER_NORMAL_ADD_COLOR );
			}
			if( m_fRedGaugeAlpha > 0.0f )
			{
				m_pStoneRedWhiteGauge->SetAlpha( (float)MAX_ALPHA_RATE * m_fRedGaugeAlpha );
				fWidth = min( m_pStoneRedWhiteGauge->GetWidth(), m_pStoneRedWhiteGauge->GetWidth() * ( FLOAT1 - m_fRedSymbolRate ) );
				m_pStoneRedWhiteGauge->RenderWidthCut( iXPos, (iYPos - 23) + m_iAddYPos, fWidth, m_pStoneRedGauge1->GetWidth() );
			}
		}		
	}

	//
	if( m_pKoText )
	{
		m_pKoText->Render( iXPos, (iYPos - 16) + m_iAddYPos, UI_RENDER_ADD );
	}


	// Icon
	if( m_pBlueIconBack && m_pBlueStoneIcon )
	{
		m_pBlueIconBack->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );
		m_pBlueStoneIcon->SetScale( 0.66f );
		m_pBlueStoneIcon->SetColor( 255, 255, 255 );
		m_pBlueStoneIcon->Render( ( iXPos - 122 ) + m_iBlueSymbolVibration, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_iWhiteColor > 0 )
		{
			m_pBlueStoneIcon->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			m_pBlueStoneIcon->Render( ( iXPos - 122 ) + m_iBlueSymbolVibration, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
		}

		if( m_bBlueSymbolAttack && m_pStoneBlueAttack )
		{
			float fGapTime = FRAMEGETTIME() - m_fBlueSymbolAttackTime;
			float fAlphaRate = max( 0.0f, FLOAT1 - ( fGapTime / 150.0f ) );
			m_pStoneBlueAttack->SetAlpha( (float)MAX_ALPHA_RATE * fAlphaRate );
			m_pStoneBlueAttack->Render( ( iXPos - 122 ) + m_iBlueSymbolAttackX, (iYPos - 8) + m_iAddYPos + m_iBlueSymbolAttackY );
		}
	}

	if( m_pRedIconBack && m_pRedStoneIcon )
	{
		m_pRedIconBack->Render( iXPos + 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );
		m_pRedStoneIcon->SetScale( 0.66f );
		m_pRedStoneIcon->SetColor( 255, 255, 255 );
		m_pRedStoneIcon->Render( (iXPos + 122) + m_iRedSymbolVibration, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_iWhiteColor > 0 )
		{
			m_pRedStoneIcon->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			m_pRedStoneIcon->Render( (iXPos + 122) + m_iRedSymbolVibration, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
		}

		if( m_bRedSymbolAttack && m_pStoneRedAttack )
		{
			float fGapTime = FRAMEGETTIME() - m_fRedSymbolAttackTime;
			float fAlphaRate = max( 0.0f, FLOAT1 - ( fGapTime / 150.0f ) );
			m_pStoneRedAttack->SetAlpha( (float)MAX_ALPHA_RATE * fAlphaRate );
			m_pStoneRedAttack->Render( ( iXPos + 122 ) + m_iRedSymbolAttackX, (iYPos - 8) + m_iAddYPos + m_iRedSymbolAttackY );
		}
	}

	// Line
	if( m_pCenterLine )
	{
		m_pCenterLine->SetAlpha( MAX_ALPHA_RATE );
		m_pCenterLine->Render( iXPos - 109, iYPos + 12 );
	}

	// TEXT
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_14, STR(1) );
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER, MAX_ALPHA_RATE );
}

void CenterInfoWnd::RenderGangsiMode( int iXPos, int iYPos )
{
	if( m_GangsiAlarmAni.GetCurState() != GAA_NONE ) return;
	if( m_GangsiModeAni.GetCurState() == GMA_NONE || m_GangsiModeAni.GetCurState() == GMA_START ) return;
	if( !m_pRedNum || !m_pWhiteNum || !m_pCenterLine || !m_pTextSurvivor ) return;

	if( m_fPrevRankRate > 0.0f )
	{
		// Prev Rank
		int iCurrentXPos = FLOAT100 * (1.0f - m_fPrevRankRate);
		m_pRedNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fPrevRankRate );
		m_pTextSurvivor->SetAlpha( (float)MAX_ALPHA_RATE * m_fPrevRankRate );
		if( m_iPrevRank < 10 )
		{
			m_pRedNum->RenderNum( (iXPos - 87) + iCurrentXPos, iYPos - 36, m_iPrevRank );
			if( m_GangsiModeAni.GetCurState() == GMA_TWINKLE )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( (iXPos - 87) + iCurrentXPos, iYPos - 36, m_iPrevRank );
			}
			m_pTextSurvivor->Render( (iXPos - 49) + iCurrentXPos, iYPos - 36 );
		}
		else
		{
			m_pRedNum->RenderNum( (iXPos - 104) + iCurrentXPos, iYPos - 36, m_iPrevRank / 10 );
			m_pRedNum->RenderNum( (iXPos - 69) + iCurrentXPos, iYPos - 36, m_iPrevRank % 10 );
			if( m_GangsiModeAni.GetCurState() == GMA_TWINKLE )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( (iXPos - 104) + iCurrentXPos, iYPos - 36, m_iPrevRank / 10 );
				m_pWhiteNum->RenderNum( (iXPos - 69) + iCurrentXPos, iYPos - 36, m_iPrevRank % 10 );
			}
			m_pTextSurvivor->Render( (iXPos - 31) + iCurrentXPos, iYPos - 36 );
		}
	}

	{
		// Current Rank
		int iCurrentXPos = -( FLOAT100 * (1.0f - m_fCurRankRate));
		m_pRedNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurRankRate );
		m_pTextSurvivor->SetAlpha( (float)MAX_ALPHA_RATE * m_fCurRankRate );
		if( m_iCurRank < 10 )
		{
			m_pRedNum->RenderNum( (iXPos - 87) + iCurrentXPos, iYPos - 36, m_iCurRank );
			if( m_GangsiModeAni.GetCurState() == GMA_TWINKLE )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( (iXPos - 87) + iCurrentXPos, iYPos - 36, m_iCurRank );
			}
			m_pTextSurvivor->Render( (iXPos - 49) + iCurrentXPos, iYPos - 36 );
		}
		else
		{
			m_pRedNum->RenderNum( (iXPos - 104) + iCurrentXPos, iYPos - 36, m_iCurRank / 10 );
			m_pRedNum->RenderNum( (iXPos - 69) + iCurrentXPos, iYPos - 36, m_iCurRank % 10 );
			if( m_GangsiModeAni.GetCurState() == GMA_TWINKLE )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( (iXPos - 104) + iCurrentXPos, iYPos - 36, m_iCurRank / 10 );
				m_pWhiteNum->RenderNum( (iXPos - 69) + iCurrentXPos, iYPos - 36, m_iCurRank % 10 );
			}
			m_pTextSurvivor->Render( (iXPos - 31) + iCurrentXPos, iYPos - 36 );
		}
	}

	// Line
	m_pCenterLine->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	m_pCenterLine->Render( iXPos - 109, iYPos + 12 );

	//
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );

	if( m_szGangsiName.IsEmpty() )
	{
		kPrinter.SetBkColor( 0, 0, 0 );

		kPrinter.SetTextColor( 255, 60, 0 );
		kPrinter.AddTextPiece( FONT_SIZE_14, m_szSurvivorName.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(1) );
	}
	else
	{
		kPrinter.SetBkColor( 0, 0, 0 );

		kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(2) );

		kPrinter.SetTextColor( 0, 150, 255 );
		kPrinter.AddTextPiece( FONT_SIZE_14, m_szGangsiName.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(3) );
		
		kPrinter.SetTextColor( 255, 60, 0 );
		kPrinter.AddTextPiece( FONT_SIZE_14, m_szSurvivorName.c_str() );

		kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(4) );
	}
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER,  (float)MAX_ALPHA_RATE * m_fAlphaRate );
}

void CenterInfoWnd::RenderGangsiAlarm( int iXPos, int iYPos )
{
	if( m_GangsiAlarmAni.GetCurState() == GAA_NONE || m_GangsiAlarmAni.GetCurState() == GAA_START ) return;
	if( !m_pBlueNum || !m_pTextSec || !m_pCenterLine ) return;

	int iCurSec = (float)( m_dwGangsiAlarmTickTime + 500 ) / 1000;
	
	m_pBlueNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	m_pTextSec->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	if( iCurSec < 10 )  // 1자리
	{
		m_pTextSec->SetScale( FLOAT1 + (3.0f * m_fTimeRate) );

		m_pBlueNum->RenderNum( (iXPos - 40) + m_iAddXPos - (121 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), iCurSec, 0.0f, FLOAT1 + (3.0f * m_fTimeRate) );	
		m_pTextSec->Render( (iXPos - 2) + m_iAddXPos - (7 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate) );

		if( m_bGangsiAlarmScreen )
		{
			m_pBlueNum->RenderNum( (iXPos - 40) + m_iAddXPos - (121 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), iCurSec, 0.0f, FLOAT1 + (3.0f * m_fTimeRate), UI_RENDER_SCREEN );	
			m_pTextSec->Render( (iXPos - 2) + m_iAddXPos - (7 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), UI_RENDER_SCREEN );
		}
	}
	else if( iCurSec < 100 ) // 2자리
	{
		m_pTextSec->SetScale( FLOAT1 + (3.0f * m_fTimeRate) );
	
		m_pBlueNum->RenderNum( (iXPos - 58) + m_iAddXPos - (174 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), iCurSec / 10, 0.0f, FLOAT1 + (3.0f * m_fTimeRate) );
		m_pBlueNum->RenderNum( (iXPos - 23) + m_iAddXPos - (69 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), iCurSec % 10, 0.0f, FLOAT1 + (3.0f * m_fTimeRate) );
		m_pTextSec->Render( (iXPos + 15) + m_iAddXPos + (45 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate) );

		if( m_bGangsiAlarmScreen )
		{
			m_pBlueNum->RenderNum( (iXPos - 58) + m_iAddXPos - (174 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), iCurSec / 10, 0.0f, FLOAT1 + (3.0f * m_fTimeRate), UI_RENDER_SCREEN );
			m_pBlueNum->RenderNum( (iXPos - 23) + m_iAddXPos - (69 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 *  m_fTimeRate), iCurSec % 10, 0.0f, FLOAT1 + (3.0f * m_fTimeRate), UI_RENDER_SCREEN );
			m_pTextSec->Render(  (iXPos + 15) + m_iAddXPos + (45 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), UI_RENDER_SCREEN );
		}
	}
	else // 3자리
	{
		m_pTextSec->SetScale( FLOAT1 + (3.0f * m_fTimeRate) );

		m_pBlueNum->RenderNum( (iXPos - 75) + m_iAddXPos - (226 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), iCurSec / 100, 0.0f, FLOAT1 + (3.0f * m_fTimeRate) );
		m_pBlueNum->RenderNum( (iXPos - 40) + m_iAddXPos - (121 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), (iCurSec % 100) / 10, 0.0f, FLOAT1 + (3.0f * m_fTimeRate) );
		m_pBlueNum->RenderNum( (iXPos - 5) + m_iAddXPos - (16 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), iCurSec % 10, 0.0f, FLOAT1 + (3.0f * m_fTimeRate) );
		m_pTextSec->Render( (iXPos + 33) + m_iAddXPos + (98 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate) );

		if( m_bGangsiAlarmScreen )
		{
			m_pBlueNum->RenderNum( (iXPos - 75) + m_iAddXPos - (226 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), iCurSec / 100, 0.0f, FLOAT1 + (3.0f * m_fTimeRate), UI_RENDER_SCREEN );
			m_pBlueNum->RenderNum( (iXPos - 40) + m_iAddXPos - (121 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), (iCurSec % 100) / 10, 0.0f, FLOAT1 + (3.0f * m_fTimeRate), UI_RENDER_SCREEN );
			m_pBlueNum->RenderNum( (iXPos - 5) + m_iAddXPos - (16 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), iCurSec % 10, 0.0f, FLOAT1 + (3.0f * m_fTimeRate), UI_RENDER_SCREEN );
			m_pTextSec->Render( (iXPos + 33) + m_iAddXPos + (98 * m_fTimeRate), (iYPos - 36) + m_iAddYPos - (67 * m_fTimeRate), UI_RENDER_SCREEN );
		}
	}

	// Line
	m_pCenterLine->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
	m_pCenterLine->Render( iXPos - 109, iYPos + 12 );

	// 
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	if( !m_bGangsiAlarmText )
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(1) );
	else
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(2) );
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER, (float)MAX_ALPHA_RATE * m_fAlphaRate );
}

void CenterInfoWnd::RenderDungeonAMode( int iXPos, int iYPos )
{
	if( m_DungeonAModeAni.GetCurState() == MDA_NONE || m_DungeonAModeAni.GetCurState() == MDA_START ) return;
	if( !m_pNumColon || !m_pCenterLine || !m_pDiceRewardIconBack || !m_pDiceRewardFailIcon ) return;
	if( m_MonsterDiceList.empty() ) return;
	
	MonsterDiceData &rkDiceData = *m_MonsterDiceList.begin();

	int i = 0;
	int iUserSize  = (int)rkDiceData.m_RankUserList.size();
	int iStartXPos = iXPos - ((iUserSize - 1) * 40);
	int iStartYPos = iYPos - 71;
	for(i = 0;i < iUserSize;i++)
	{
		ioHashString &rkRankName = rkDiceData.m_RankUserList[i];

		m_pDiceRewardIconBack->SetScale( 0.80f );
		m_pDiceRewardIconBack->Render( iStartXPos + (i * 80), iStartYPos + 57, UI_RENDER_MULTIPLY, TFO_BILINEAR );

		bool bPrizeIcon = false;
		if( m_szPrizeWinner == rkRankName )
		{
			bPrizeIcon = true;
		}
		else if( iUserSize == 1 && m_iPrizeRollingArray % 2 == 0 )    // 1명
		{
			bPrizeIcon = true;
		}
		else if( iUserSize > 1 && m_iPrizeRollingArray % iUserSize == i )  // 여러명
		{
			if( m_szPrizeWinner.IsEmpty() )
				bPrizeIcon = true;
		}

		if( bPrizeIcon )
		{
			// 선물 아이콘
			if( m_pDicePrizeIcon )
			{
				m_pDicePrizeIcon->SetScale( 0.80f );
				m_pDicePrizeIcon->Render( iStartXPos + (i * 80), iStartYPos + 57, UI_RENDER_NORMAL, TFO_BILINEAR );
				if( m_bPrizeScreen )
					m_pDicePrizeIcon->Render( iStartXPos + (i * 80), iStartYPos + 57, UI_RENDER_SCREEN, TFO_BILINEAR );
			}
			if( m_pDiceSubIcon )
			{
				m_pDiceSubIcon->SetScale( 0.80f );
				m_pDiceSubIcon->Render( iStartXPos + (i * 80), iStartYPos + 57, UI_RENDER_NORMAL, TFO_BILINEAR );
				if( m_bPrizeScreen )
					m_pDiceSubIcon->Render( iStartXPos + (i * 80), iStartYPos + 57, UI_RENDER_SCREEN, TFO_BILINEAR );
			}
		}
		else  
		{
			// 꽝 아이콘
			m_pDiceRewardFailIcon->SetScale( 0.80f );
			m_pDiceRewardFailIcon->Render( iStartXPos + (i * 80), iStartYPos + 57, UI_RENDER_NORMAL, TFO_BILINEAR );
		}

		if( i != 0 )
		{
			m_pNumColon->Render( iStartXPos + (i * 80) - 46, iStartYPos + 44 );
		}

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		if( g_MyInfo.GetPublicID() == rkRankName )
			g_FontMgr.SetTextColor( 0, 150, 255 );
		else
			g_FontMgr.SetTextColor( 255, 60, 0 );
		g_FontMgr.PrintText( iStartXPos + (i * 80), iStartYPos, FONT_SIZE_12, STR(1), i + 1 );
		g_FontMgr.PrintTextWidthCut( iStartXPos + (i * 80), iStartYPos + 16, FONT_SIZE_12, 77.0f, STR(2), rkRankName.c_str() );
	}


	// Line
	m_pCenterLine->SetAlpha( MAX_ALPHA_RATE );
	m_pCenterLine->Render( iXPos - 109, iYPos + 12 );

	// 
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE ); 
	kPrinter.AddTextPiece( FONT_SIZE_14, STR(3), rkDiceData.m_MonsterName.c_str()  );

	kPrinter.SetTextColor( 255, 60, 0 ); 
	if( rkDiceData.m_iPresentType == PRESENT_PESO )
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(4), rkDiceData.m_iPresentValue1, g_PresentMgr.GetPresentValue1Text( rkDiceData.m_iPresentType, rkDiceData.m_iPresentValue1, rkDiceData.m_iPresentValue2 ).c_str() );
	else if( rkDiceData.m_iPresentType == PRESENT_EXTRAITEM )
	{
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(5), ( rkDiceData.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2, g_PresentMgr.GetPresentValue1Text( rkDiceData.m_iPresentType, rkDiceData.m_iPresentValue1, rkDiceData.m_iPresentValue2 ).c_str(),
													      g_PresentMgr.GetPresentValue2Text( rkDiceData.m_iPresentType, rkDiceData.m_iPresentValue1, rkDiceData.m_iPresentValue2 ).c_str() );
	}
	else
	{
		kPrinter.AddTextPiece( FONT_SIZE_14, STR(6), g_PresentMgr.GetPresentValue1Text( rkDiceData.m_iPresentType, rkDiceData.m_iPresentValue1, rkDiceData.m_iPresentValue2 ).c_str(),
													  g_PresentMgr.GetPresentValue2Text( rkDiceData.m_iPresentType, rkDiceData.m_iPresentValue1, rkDiceData.m_iPresentValue2 ).c_str() );
	}

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE ); 
	kPrinter.AddTextPiece( FONT_SIZE_14, STR(7) );
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER );
}

void CenterInfoWnd::RenderDoubleCrownGauge( int iXPos, int iYPos )
{	
	if( m_DoubleCrownAni.GetCurState() == DCA_NONE || m_DoubleCrownAni.GetCurState() == DCA_START ) return;

	// 게이지 
	if( m_pStoneGaugeBack )
	{
		m_pStoneGaugeBack->SetColor( 255, 255, 255 );
		m_pStoneGaugeBack->Render( iXPos - 108, (iYPos - 23) + m_iAddYPos );
		if( m_iWhiteColor > 0 )
		{
			m_pStoneGaugeBack->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			m_pStoneGaugeBack->Render( iXPos - 108, (iYPos - 23) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR );
		}
		// blue
		if( m_pStoneBlueGauge1 && m_pStoneBlueGauge2 && m_pDoubleBlueWhiteGauge )
		{
			m_pStoneBlueGauge2->SetColor( 255, 255, 255 );
			float fWidth = min( m_pStoneBlueGauge2->GetWidth(), m_pStoneBlueGauge2->GetWidth() * m_fDoublePrevBlueRate );
			m_pStoneBlueGauge2->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, fWidth );

			fWidth = min( m_pStoneBlueGauge1->GetWidth(), m_pStoneBlueGauge1->GetWidth() * m_fDoubleBlueRate );
			m_pStoneBlueGauge1->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, fWidth );

			if( m_iWhiteColor > 0 )
			{
				m_pStoneBlueGauge2->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
				m_pStoneBlueGauge2->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, fWidth, UI_RENDER_NORMAL_ADD_COLOR );
			}
			if( m_fDoubleBlueGaugeAlpha > 0.0f )
			{
				m_pDoubleBlueWhiteGauge->SetAlpha( (float)MAX_ALPHA_RATE * m_fDoubleBlueGaugeAlpha );
				fWidth = min( m_pDoubleBlueWhiteGauge->GetWidth(), m_pDoubleBlueWhiteGauge->GetWidth() * m_fDoubleBlueRate );
				m_pDoubleBlueWhiteGauge->RenderWidthCut( iXPos - 108, (iYPos - 23) + m_iAddYPos, 0, fWidth );
			}
		}

		// red
		if( m_pStoneRedGauge1 && m_pStoneRedGauge2 && m_pDoubleRedWhiteGauge )
		{
			m_pStoneRedGauge2->SetColor( 255, 255, 255 );
			float fWidth = min( m_pStoneRedGauge2->GetWidth(), m_pStoneRedGauge2->GetWidth() * ( FLOAT1 - m_fDoublePrevRedRate ) );
			m_pStoneRedGauge2->RenderWidthCut( iXPos, (iYPos - 23) + m_iAddYPos, fWidth, m_pStoneBlueGauge2->GetWidth() );

			fWidth = min( m_pStoneRedGauge1->GetWidth(), m_pStoneRedGauge1->GetWidth() * ( FLOAT1 - m_fDoubleRedRate ) );
			m_pStoneRedGauge1->RenderWidthCut( iXPos, (iYPos - 23) + m_iAddYPos, fWidth, m_pStoneRedGauge1->GetWidth() );
			if( m_iWhiteColor > 0 )
			{
				m_pStoneRedGauge2->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
				m_pStoneRedGauge2->RenderWidthCut( iXPos, (iYPos - 23) + m_iAddYPos, fWidth, m_pStoneBlueGauge2->GetWidth(), UI_RENDER_NORMAL_ADD_COLOR );
			}
			if( m_fDoubleRedGaugeAlpha > 0.0f )
			{
				m_pDoubleRedWhiteGauge->SetAlpha( (float)MAX_ALPHA_RATE * m_fDoubleRedGaugeAlpha );
				fWidth = min( m_pDoubleRedWhiteGauge->GetWidth(), m_pDoubleRedWhiteGauge->GetWidth() * ( FLOAT1 - m_fDoubleRedRate ) );
				m_pDoubleRedWhiteGauge->RenderWidthCut( iXPos, (iYPos - 23) + m_iAddYPos, fWidth, m_pStoneRedGauge1->GetWidth() );
			}
		}		
	}

	//Alram
	switch( m_iDoubleWinningTeam )
	{
	case TEAM_BLUE:
		{
			if( m_pDoubleBlueCloseWin )
				m_pDoubleBlueCloseWin->Render( iXPos, (iYPos - 16) + m_iAddYPos, UI_RENDER_ADD );
		}
		break;
	case TEAM_RED:
		{
			if( m_pDoubleRedCloseWin )
				m_pDoubleRedCloseWin->Render( iXPos, (iYPos - 16) + m_iAddYPos, UI_RENDER_ADD );
		}
		break;
	default:
		{
			if( m_pDoubleCrownSimilar )	
				m_pDoubleCrownSimilar->Render( iXPos, (iYPos - 16) + m_iAddYPos, UI_RENDER_ADD );
		}
		break;
	}
	
	// Icon
	if( m_pBlueIconBack && m_pDoubleBlueCrown )
	{
		m_pBlueIconBack->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );
		m_pDoubleBlueCrown->SetScale( 0.66f );
		m_pDoubleBlueCrown->SetColor( 255, 255, 255 );
		m_pDoubleBlueCrown->Render( ( iXPos - 122 ) + m_iDoubleBlueVibration, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_iWhiteColor > 0 )
		{
			m_pDoubleBlueCrown->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			m_pDoubleBlueCrown->Render( ( iXPos - 122 ) + m_iDoubleBlueVibration, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
		}

		if( m_bDoubleBlueAttack && m_pStoneBlueAttack )
		{
			float fGapTime = FRAMEGETTIME() - m_fDoubleBlueAttackTime;
			float fAlphaRate = max( 0.0f, FLOAT1 - ( fGapTime / 150.0f ) );
			m_pStoneBlueAttack->SetAlpha( (float)MAX_ALPHA_RATE * fAlphaRate );
			m_pStoneBlueAttack->Render( ( iXPos - 122 ) + m_iDoubleBlueAttackX, (iYPos - 8) + m_iAddYPos + m_iDoubleBlueAttackY );
		}
	}

	if( m_pRedIconBack && m_pDoubleRedCrown )
	{
		m_pRedIconBack->Render( iXPos + 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );
		m_pDoubleRedCrown->SetScale( 0.66f );
		m_pDoubleRedCrown->SetColor( 255, 255, 255 );
		m_pDoubleRedCrown->Render( (iXPos + 122) + m_iDoubleRedVibration, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_iWhiteColor > 0 )
		{
			m_pDoubleRedCrown->SetColor( m_iWhiteColor, m_iWhiteColor, m_iWhiteColor );
			m_pDoubleRedCrown->Render( (iXPos + 122) + m_iDoubleRedVibration, (iYPos - 8) + m_iAddYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
		}

		if( m_bDoubleRedAttack && m_pStoneRedAttack )
		{
			float fGapTime = FRAMEGETTIME() - m_fDoubleRedAttackTime;
			float fAlphaRate = max( 0.0f, FLOAT1 - ( fGapTime / 150.0f ) );
			m_pStoneRedAttack->SetAlpha( (float)MAX_ALPHA_RATE * fAlphaRate );
			m_pStoneRedAttack->Render( ( iXPos + 122 ) + m_iDoubleRedAttackX, (iYPos - 8) + m_iAddYPos + m_iDoubleRedAttackY );
		}
	}
		
	// 이펙트

	if( COMPARE( m_iRedEffectArray, 0, MAX_EFFECT ) )
	{
		if( m_pRedEffectSpr[m_iRedEffectArray] )
			m_pRedEffectSpr[m_iRedEffectArray]->Render( iXPos + 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );
	}

	if( COMPARE( m_iBlueEffectArray, 0, MAX_EFFECT ) )
	{
		if( m_pBlueEffectSpr[m_iBlueEffectArray] )
			m_pBlueEffectSpr[m_iBlueEffectArray]->Render( iXPos - 122, (iYPos - 8) + m_iAddYPos, UI_RENDER_SCREEN );	
	}

	// Line
	if( m_pCenterLine )
	{
		m_pCenterLine->SetAlpha( MAX_ALPHA_RATE );
		m_pCenterLine->Render( iXPos - 109, iYPos + 12 );
	}

	// TEXT
	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_14, "정해진 시간 동안 왕을 보호하고, 상대팀 왕관을 떨어트려라!" );
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER, MAX_ALPHA_RATE );
}

void CenterInfoWnd::RenderFlagCapture( int iXPos, int iYPos )
{
	if( m_FlagCaptureAni.GetCurState() == FCA_NONE || m_FlagCaptureAni.GetCurState() == FCA_START ) return;
	if( !m_pBlueNum || !m_pRedNum || !m_pWhiteNum || !m_pNumColon || !m_pCenterLine ) return;

	/*bool bScoreShow = false;
	if( ( m_eFlagReturn == FR_NONE || !m_szFlagKiller.IsEmpty() || !m_szFlagDier.IsEmpty() ) && ( m_szFlagCapturer.IsEmpty() && m_szFlagDroper.IsEmpty() ) )
		bScoreShow = true;*/
	bool bScoreShow = true;
	if( ( !m_bFlagCaptureWarningScore && ( !m_szFlagCapturer.IsEmpty() || !m_szFlagDroper.IsEmpty() || m_FlagCaptureAni.GetCurState() == FCA_REMAIN_TIME ) ) || m_eFlagReturn != FR_NONE )
		bScoreShow = false;

	if( bScoreShow )
	{
		// Blue Score
		int iBluePoint = m_iFlagBluePoint + ( (float)m_iFlagAddBluePoint * m_fTimeRate );
		m_pBlueNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
		if( iBluePoint < 100 )
		{
			m_pBlueNum->RenderNum( iXPos - 86, iYPos - 36, iBluePoint / 10 );
			m_pBlueNum->RenderNum( iXPos - 51, iYPos - 36, iBluePoint % 10 );

			if( ( m_bFlagBlueScoreUP && m_FlagCaptureAni.GetCurState() == FCA_TWINKLE ) || m_bFlagCaptureWarningScore )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( iXPos - 86, iYPos - 36, iBluePoint / 10 );
				m_pWhiteNum->RenderNum( iXPos - 51, iYPos - 36, iBluePoint % 10 );
			}
		}
		else
		{
			m_pBlueNum->RenderNum( iXPos - 121, iYPos - 36, iBluePoint / 100 );
			m_pBlueNum->RenderNum( iXPos - 86, iYPos - 36, (iBluePoint % 100) / 10 );
			m_pBlueNum->RenderNum( iXPos - 51, iYPos - 36, iBluePoint % 10 );		

			if( ( m_bFlagBlueScoreUP && m_FlagCaptureAni.GetCurState() == FCA_TWINKLE ) || m_bFlagCaptureWarningScore )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( iXPos - 121, iYPos - 36, iBluePoint / 100 );
				m_pWhiteNum->RenderNum( iXPos - 86, iYPos - 36, (iBluePoint % 100) / 10 );
				m_pWhiteNum->RenderNum( iXPos - 51, iYPos - 36, iBluePoint % 10 );		
			}
		}

		// Red Score 
		int iRedPoint = m_iFlagRedPoint + ( (float)m_iFlagAddRedPoint * m_fTimeRate );
		m_pRedNum->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
		if( iRedPoint < 100 )
		{
			m_pRedNum->RenderNum( iXPos + 10, iYPos - 36, iRedPoint / 10 );
			m_pRedNum->RenderNum( iXPos + 45, iYPos - 36, iRedPoint % 10 );

			if( ( !m_bFlagBlueScoreUP && m_FlagCaptureAni.GetCurState() == FCA_TWINKLE ) || m_bFlagCaptureWarningScore )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( iXPos + 10, iYPos - 36, iRedPoint / 10 );
				m_pWhiteNum->RenderNum( iXPos + 45, iYPos - 36, iRedPoint % 10 );
			}
		}
		else
		{
			m_pRedNum->RenderNum( iXPos + 10, iYPos - 36, iRedPoint / 100 );
			m_pRedNum->RenderNum( iXPos + 45, iYPos - 36, (iRedPoint % 100) / 10 );
			m_pRedNum->RenderNum( iXPos + 80, iYPos - 36, iRedPoint % 10 );		

			if( ( !m_bFlagBlueScoreUP && m_FlagCaptureAni.GetCurState() == FCA_TWINKLE ) || m_bFlagCaptureWarningScore )
			{
				m_pWhiteNum->SetAlpha( (float)((float)MAX_ALPHA_RATE * 0.3f) * m_fTimeRate );
				m_pWhiteNum->RenderNum( iXPos + 10, iYPos - 36, iRedPoint / 100 );
				m_pWhiteNum->RenderNum( iXPos + 45, iYPos - 36, (iRedPoint % 100) / 10 );
				m_pWhiteNum->RenderNum( iXPos + 80, iYPos - 36, iRedPoint % 10 );		
			}
		}

		// Colon
		m_pNumColon->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
		m_pNumColon->Render( iXPos - 6, iYPos - 27 );

		// Line
		m_pCenterLine->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
		m_pCenterLine->Render( iXPos - 109, iYPos + 12 );
	}
	else if( m_eFlagReturn != FR_NONE )
	{
		ioUIRenderImage *pImage = NULL;
		if( m_eFlagReturn == FR_NEW_FLAG )
		{
			pImage = m_pFlagCaptureNewFlag;
		}
		else if( m_eFlagReturn == FR_BLUE_RETURN )
		{
			pImage = m_pFlagCaptureBlueReturn;
		}
		else if( m_eFlagReturn == FR_RED_RETURN )
		{
			pImage = m_pFlagCaptureRedReturn;
		}

		if( pImage )
		{
			pImage->SetAlpha( (float)MAX_ALPHA_RATE * m_fAlphaRate );
			//pImage->Render( iXPos - 154, iYPos - 102 );
			pImage->Render( iXPos, iYPos );
		}
		else
		{
#ifndef SHIPPING
			LOG.PrintConsoleLog( 0, "CenterInfoWnd::RenderFlagCapture - pImage is NULL!! m_eFlagReturn = %d", m_eFlagReturn );
#endif
		}
	}

	ioComplexStringPrinter kPrinter;	
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );
	if( m_FlagCaptureAni.GetCurState() == FCA_WARNING_SCORE )
	{		
		if( m_iFlagBluePoint > m_iFlagRedPoint )
		{
			kPrinter.SetTextColor( 0, 150, 255 );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(4) );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(5) );
		}
		else if( m_iFlagBluePoint < m_iFlagRedPoint )
		{
			kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(6) );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(7) );
		}
		else
		{
			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(8) );
		}
	}
	else
	{
		// Flag Capture
		if( !m_szFlagCapturer.IsEmpty() )
		{
			if( m_eFlagCapturerTeam == TEAM_BLUE )
				kPrinter.SetTextColor( 0, 150, 255 );
			else
				kPrinter.SetTextColor( 255, 60, 0 );

			kPrinter.AddTextPiece( FONT_SIZE_14, m_szFlagCapturer.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(9) );
		}
		else if( !m_szFlagDroper.IsEmpty() )
		{
			if( m_eFlagDroperTeam == TEAM_BLUE )
				kPrinter.SetTextColor( 0, 150, 255 );
			else
				kPrinter.SetTextColor( 255, 60, 0 );

			kPrinter.AddTextPiece( FONT_SIZE_14, m_szFlagDroper.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(10) );
		}
		// Flag Carrier
		else if( !m_szFlagCarrier.IsEmpty() )
		{
			if( m_eFlagCarrierTeam == TEAM_BLUE )
				kPrinter.SetTextColor( 0, 150, 255 );
			else
				kPrinter.SetTextColor( 255, 60, 0 );

			kPrinter.AddTextPiece( FONT_SIZE_14, m_szFlagCarrier.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(11) );
		}
		else if( m_szFlagKiller.IsEmpty() && !m_szFlagDier.IsEmpty() )
		{
			if( m_eFlagDieTeam == TEAM_BLUE )
				kPrinter.SetTextColor( 0, 150, 255 );
			else
				kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_14, m_szFlagDier.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(1) );
		}
		else if( !m_szFlagKiller.IsEmpty() && !m_szFlagDier.IsEmpty() )
		{
			if( m_eFlagKillTeam == TEAM_BLUE )
				kPrinter.SetTextColor( 0, 150, 255 );
			else
				kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_14, m_szFlagKiller.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(2) );

			if( m_eFlagDieTeam == TEAM_BLUE )
				kPrinter.SetTextColor( 0, 150, 255 );
			else
				kPrinter.SetTextColor( 255, 60, 0 );
			kPrinter.AddTextPiece( FONT_SIZE_14, m_szFlagDier.c_str() );

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(3) );
		}
		else if( m_dwFlagWaitRemainTime > 0 )
		{
			DWORD dwRemainTime = 0;
			if( REALGETTIME() <= m_dwFlagWaitRemainTime )
				dwRemainTime = m_dwFlagWaitRemainTime - REALGETTIME();
			else
				dwRemainTime = 0;

			if( dwRemainTime > 0 )
				dwRemainTime = dwRemainTime / 1000;
			else
				m_dwFlagWaitRemainTime = 0;

			kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
			kPrinter.AddTextPiece( FONT_SIZE_14, STR(12), dwRemainTime );
		}
	}
	kPrinter.PrintFullText( iXPos, iYPos + 18, TAT_CENTER, (float)MAX_ALPHA_RATE * m_fAlphaRate );
}

void CenterInfoWnd::UpdateDeathTime()
{
	if( m_bNoHideWnd )
	{
		m_ePrevTeamDeathAni = (TeamDeathAni)m_TeamDeathAni.GetCurState();
		m_ePrevDeathMatchAni = (DeathMatchAni)m_DeathMatchAni.GetCurState();
		m_ePrevFlagCaptureAni = (FlagCaptureAni)m_FlagCaptureAni.GetCurState();
	}

	m_bDeathTimeScreen = false;
	m_fTimeRate = FLOAT1;
	m_fAlphaRate= 0.0f;
	m_iAddXPos = m_iAddYPos = 0;
	m_DeathTimeAni.SetCurState( DTA_FADEIN );
	m_TeamDeathAni.SetCurState( TDA_NONE );
	m_DeathMatchAni.SetCurState( DMA_NONE );
	m_HiddenCrownAni.SetCurState( HCA_NONE );
	m_BossRaidAni.SetCurState( BRA_NONE );
	m_SymbolGaugeAni.SetCurState( SGA_NONE );
	m_GangsiModeAni.SetCurState( GMA_NONE );
	m_DoubleCrownAni.SetCurState( DCA_NONE );
	m_FlagCaptureAni.SetCurState( FCA_NONE );
	ShowWnd();
}

void CenterInfoWnd::UpdateTeamDeathMode( TeamType eKill, const ioHashString &szKiller, TeamType eDie, const ioHashString &szDier, float fBlueRate, float fRedRate )
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;

	m_eKillTeam = eKill;
	m_szKiller  = szKiller;
	m_eDieTeam  = eDie;
	m_szDier    = szDier;
	
	//float 값의 0.7이 0.699999로 나오는 현상 해결.(0.7 또는 0.9)
	//UI상에서 제대로 보이게 timer , teamscore, centerinfo 창에 하드코딩
	float fBlueTempRate = fBlueRate + 0.0000001f;
	float fRedTempRate = fRedRate + 0.0000001f;

	int iBluePoint  = (int)( fBlueTempRate * FLOAT100 );
	int iRedPoint  = (int)( fRedTempRate * FLOAT100 );
	/*
	int iBluePoint = (int)( fBlueRate * FLOAT100 );
	int iRedPoint  = (int)( fRedRate * FLOAT100 );
	*/

	m_iAddBluePoint = max( 0, iBluePoint - m_iCurrentBluePoint );
	m_iAddRedPoint  = max( 0, iRedPoint - m_iCurrentRedPoint );

	m_bBlueScoreUP   = false;	
	if( m_eDieTeam == TEAM_RED )
	{	
		m_bBlueScoreUP = true;
	}
	m_fTimeRate = 0.0f;
	m_fAlphaRate= FLOAT1;
	m_iAddXPos = m_iAddYPos = 0;
	m_TeamDeathAni.SetCurState( TDA_START );
	if( IsShow() )
		HideWnd();
	ShowWnd();
}

void CenterInfoWnd::UpdateTeamDeathWarning( float fBlueRate, float fRedRate )
{
	if( m_DeathTimeAni.GetCurState() != DTA_NONE )
		return;

	m_bTeamDeathWarningScore = true;
	m_bNoHideWnd = true;

	// 화면에 없으면 표시
	if( m_TeamDeathAni.GetCurState() == TDA_NONE )
	{
		m_iCurrentBluePoint = (int)( fBlueRate * FLOAT100 );
		m_iCurrentRedPoint  = (int)( fRedRate * FLOAT100 );
		m_iAddBluePoint = m_iAddRedPoint = 0;

		m_fTimeRate = 0.0f;
		m_fAlphaRate= FLOAT1;
		m_iAddXPos = m_iAddYPos = 0;
		m_TeamDeathAni.SetCurState( TDA_WARNING_SCORE );
		if( !IsShow() )
			ShowWnd();
	}
}

void CenterInfoWnd::UpdateDeathMatchMode( int iCurrentRank )
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;
	if( iCurrentRank == -1 )
		return;

	// 첫 시작
	if( m_iCurRank == 0 )
	{
		m_iCurRank = m_iPrevRank = iCurrentRank;
	}

	// 긴박 상태가 아닐 때만 순위 떨어지는거 표시하지 않음.
	if( !m_bDeathMatchTimeWarning || 
		iCurrentRank == m_iCurRank )
	{
		if( iCurrentRank >= m_iCurRank )
		{
			if( m_DeathMatchAni.GetCurState() == DMA_NONE )
			{
				// 떨어지는건 기록만하고 패스
				m_iPrevRank = iCurrentRank;
				m_iCurRank  = iCurrentRank;
			}
			return;
		}
	}

	m_iPrevRank = m_iCurRank;
	m_iCurRank  = iCurrentRank;
	m_fTimeRate = 0.0f;
	m_fAlphaRate= FLOAT1;
	m_iAddXPos = m_iAddYPos = 0;
	m_DeathMatchAni.SetCurState( DMA_START );
	if( IsShow() )
		HideWnd();
	ShowWnd();
}

void CenterInfoWnd::UpdateDeathMatchWarning( int iCurrentRank )
{
	m_bDeathMatchTimeWarning = true;
	m_bNoHideWnd = true;

	if( m_DeathTimeAni.GetCurState() != DTA_NONE )
		return;

	// 화면에 없으면 표시
	if( m_DeathMatchAni.GetCurState() == DMA_NONE )
	{
		m_iPrevRank = iCurrentRank;
		m_iCurRank  = iCurrentRank;
		m_fTimeRate = 0.0f;
		m_fAlphaRate= FLOAT1;
		m_iAddXPos = m_iAddYPos = 0;
		m_DeathMatchAni.SetCurState( DMA_WARNING_TIME );
		if( !IsShow() )
			ShowWnd();
	}
}

void CenterInfoWnd::UpdateSymbolGauge( float fBlueRate, float fRedRate )
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;

	if( !IsShow() )
	{
		// 최초 등장
		m_iAddXPos = 0;
		m_iAddYPos = -35;
		m_iWhiteColor = 255;
		m_fBlueSymbolRate = m_fPrevBlueSymbolRate = fBlueRate;
		m_fBlueGaugeAlpha = 0.0f;

		m_fBlueGaugeDownTime = m_fRedGaugeDownTime = 0.0f;

		m_fRedSymbolRate  = m_fPrevRedSymbolRate = fRedRate;
		m_fRedGaugeAlpha  = 0.0f;

		m_SymbolGaugeAni.SetCurState( SGA_START );
		ShowWnd();
	}
	else
	{
		if( fBlueRate != m_fBlueSymbolRate )
		{
			float fPrevRate   = m_fBlueSymbolRate;
			m_fBlueSymbolRate = fBlueRate;
			if( fPrevRate > m_fBlueSymbolRate )
			{
				m_fPrevBlueSymbolRate = fPrevRate;
				m_fBlueGaugeDownTime  = FRAMEGETTIME();
			}
		}
		
		if( fRedRate != m_fRedSymbolRate )
		{
			float fPrevRate  = m_fRedSymbolRate;
			m_fRedSymbolRate = fRedRate;
			if( fPrevRate > m_fRedSymbolRate )
			{
				m_fPrevRedSymbolRate = fPrevRate;	
				m_fRedGaugeDownTime = FRAMEGETTIME();
			}
		}
	}	
}

void CenterInfoWnd::UpdateSymbolAttack( TeamType eSymbolTeam )
{
	if( !IsShow() ) return;
	if( m_DeathTimeAni.GetCurState() != TDA_NONE ) return;
	if( m_SymbolGaugeAni.GetCurState() != SGA_LOOP ) return;

	if( eSymbolTeam == TEAM_BLUE )
	{
		if( !m_bBlueSymbolAttack )
		{
			m_bBlueSymbolAttack = true;
			m_fBlueSymbolAttackTime = FRAMEGETTIME();
			m_iBlueSymbolVibration = -3;
			m_iBlueSymbolAttackX = ( rand() % 20 ) - 10;
			m_iBlueSymbolAttackY = ( rand() % 20 ) - 10;
			if( rand()%2 == 0 )
				m_pStoneBlueAttack = m_pStoneAttack1;
			else
				m_pStoneBlueAttack = m_pStoneAttack2;
		}
	}
	else if( eSymbolTeam == TEAM_RED )
	{
		if( !m_bRedSymbolAttack )
		{
			m_bRedSymbolAttack = true;
			m_fRedSymbolAttackTime = FRAMEGETTIME();
			m_iRedSymbolVibration = -3;
			m_iRedSymbolAttackX = ( rand() % 20 ) - 10;
			m_iRedSymbolAttackY = ( rand() % 20 ) - 10;
			if( rand()%2 == 0 )
				m_pStoneRedAttack = m_pStoneAttack1;
			else
				m_pStoneRedAttack = m_pStoneAttack2;
		}
	}
}

void CenterInfoWnd::UpdateKingMode( bool bKing, TeamType eKingTeam, TeamType eOwnerTeam, DWORD dwGivePointTime, DWORD dwCrownHoldTime, float fScoreWarningRate )
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;

	if( IsShow() && !bKing )
	{
		HideWnd();
		return;
	}
	else if( !IsShow() && bKing )
	{
		m_iAddXPos = 0;
		m_iAddYPos = -35;
		m_iWhiteColor = 255;
		m_fAlphaRate = 0.0f;
		m_iEffectArray = -1;
		m_HiddenCrownAni.SetCurState( HCA_START );
		ShowWnd();
	}

	if( bKing )
	{
		m_eKingTeam = eKingTeam;
		m_eOwnerTeam= eOwnerTeam;
		m_dwGivePointTime = dwGivePointTime;
		m_fScoreWarningRate = fScoreWarningRate;	
		if( m_dwCrownHoldTime == 0 )
		{
			m_dwCrownHoldTime = FRAMEGETTIME();
		}
	}
	else
	{
		m_dwCrownHoldTime = 0;
	}
}

void CenterInfoWnd::UpdateBossMode( int iBossRaidOrder, int iBossRaidMaxOrder, int iBossRaidMyLevel )
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;

	if( !IsShow() )
	{
		// 최초 등장
		m_iAddXPos = 0;
		m_iAddYPos = -35;
		m_iWhiteColor = 255;
		m_fAlphaRate = 0.0f;
		m_iEffectArray = -1;
		m_BossRaidAni.SetCurState( BRA_START );
		ShowWnd();
	}
	
	// 애니메이션 시작
	if( iBossRaidOrder < m_iBossRaidOrder )
	{
		m_dwBossRaidTime = FRAMEGETTIME();
	}
	m_iBossRaidOrder = iBossRaidOrder;
	m_iBossRaidMaxOrder = iBossRaidMaxOrder;
	m_iBossRaidMyLevel  = max( 1, iBossRaidMyLevel );

	if( m_iBossRaidOrder == 0 )
	{
		BossRaidSound( true );
	}
	else
	{
		BossRaidSound( false );
	}
}

void CenterInfoWnd::UpdateGangsiLiveUser( int iLiveUser, const ioHashString &szGangsi, const ioHashString &szSurvivor )
{
	if( iLiveUser == 0 ) return;

	m_szGangsiName   = szGangsi;
	m_szSurvivorName = szSurvivor;
	m_iPrevRank = m_iCurRank;
	m_iCurRank  = iLiveUser;
	m_fTimeRate = 0.0f;
	m_fAlphaRate= FLOAT1;
	m_iAddXPos = m_iAddYPos = 0;
	m_GangsiModeAni.SetCurState( GMA_START );
	m_GangsiAlarmAni.SetCurState( GAA_NONE );
	if( IsShow() )
		HideWnd();
	ShowWnd();

	if( !m_GangsiLiveSound.IsEmpty() )
	{
		g_SoundMgr.PlaySound( m_GangsiLiveSound );
	}
}

void CenterInfoWnd::UpdateGangsiAlarm( bool bGangsiAlarmText, DWORD dwCurrentTime, DWORD dwWarningTime )
{
	if( m_GangsiModeAni.GetCurState() != GMA_NONE )
		return;

	m_bGangsiAlarmText      = bGangsiAlarmText;
	m_dwGangsiAlarmTickTime = dwCurrentTime;
	m_dwGangsiAlarmWarningTime = dwWarningTime;
	if( m_GangsiAlarmAni.GetCurState() == GAA_NONE )
	{
		m_bGangsiAlarmScreen = false;
		m_fTimeRate = FLOAT1;
		m_fAlphaRate= 0.0f;
		m_iAddXPos = m_iAddYPos = 0;
		m_GangsiAlarmAni.SetCurState( GAA_START );
		ShowWnd();
	}
}

void CenterInfoWnd::UpdateDungeonAMode( const ioHashString &kMonsterName, int iPresentType, int iPresentValue1, int iPresentValue2, ioHashStringVec &rkRankUserList )
{
	MonsterDiceData kDiceData;
	kDiceData.m_MonsterName		= kMonsterName;
	kDiceData.m_iPresentType	= iPresentType;
	kDiceData.m_iPresentValue1	= iPresentValue1;
	kDiceData.m_iPresentValue2  = iPresentValue2;
	for(int i = 0;i < (int)rkRankUserList.size();i++)
	{
		kDiceData.m_RankUserList.push_back( rkRankUserList[i] );
	}
	m_MonsterDiceList.push_back( kDiceData );

	if( m_DungeonAModeAni.GetCurState() == MDA_NONE )
	{
		m_iPrizeRollingArray = 0;
		m_bPrizeScreen = false;
		m_szPrizeWinner.Clear();
		SAFEDELETE( m_pDicePrizeIcon );
		SAFEDELETE( m_pDiceSubIcon );
		m_pDicePrizeIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentIconText( kDiceData.m_iPresentType, kDiceData.m_iPresentValue1, kDiceData.m_iPresentValue2 ) );
		m_pDiceSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_PresentMgr.GetPresentSubIconText( kDiceData.m_iPresentType, kDiceData.m_iPresentValue1, kDiceData.m_iPresentValue2 ) );
		m_DungeonAModeAni.SetCurState( MDA_START );
		ShowWnd();
	}
}

void CenterInfoWnd::UpdateMonsterDiceResult( const ioHashString &kPrizeWinner, int iPresentType, int iPresentValue1, int iPresentValue2 )
{
	if( m_DungeonAModeAni.GetCurState() == MDA_NONE )
		return;

	m_szPrizeWinner = kPrizeWinner;
	m_DungeonAModeAni.SetCurState( MDA_FADEOUT );
}

void CenterInfoWnd::BossRaidSound( bool bPlay )
{
	if( m_BossRaidSound.IsEmpty() ) return;

	if( m_bBossRaidSnd && !bPlay )
	{
		g_SoundMgr.StopSound( m_BossRaidSound, 0 );
		m_bBossRaidSnd = false;
	}
	else if( !m_bBossRaidSnd && bPlay )
	{
		g_SoundMgr.PlaySound( m_BossRaidSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
		m_bBossRaidSnd = true;
	}
}

bool CenterInfoWnd::IsHiddenKingWarning()
{
	if( !IsShow() ) 
		return false;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwCrownHoldTime;
	if( dwGapTime > ( (float)m_dwGivePointTime * m_fScoreWarningRate ) )
		return true;
	return false;
}

void CenterInfoWnd::UpdateDoubleCrownGauge( float fBlueRate, float fRedRate, bool bBlueKing, bool bRedKIng )
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;

	m_bBlueKing = bBlueKing;
	m_bRedKing  = bRedKIng;

	if( !IsShow() )
	{
		// 최초 등장
		m_iAddXPos = 0;
		m_iAddYPos = -35;
		m_iWhiteColor = 255;
		m_iBlueEffectArray = -1;
		m_iRedEffectArray  = -1;

		m_fDoubleBlueRate = m_fDoublePrevBlueRate = fBlueRate;
		m_fDoubleBlueGaugeAlpha = 0.0f;

		m_fDoubleRedRate  = m_fDoublePrevRedRate = fRedRate;
		m_fDoubleRedGaugeAlpha = 0.0f;

		m_fDoubleBlueGaugeDownTime = m_fDoubleRedGaugeDownTime = 0.0f;
		m_DoubleCrownAni.SetCurState( DCA_START );
		ShowWnd();
	}
	else
	{
		if( fBlueRate != m_fDoubleBlueRate )
		{
			float fPrevRate   = m_fDoubleBlueRate;
			m_fDoubleBlueRate = fBlueRate;
			if( fPrevRate > m_fDoubleBlueRate )
			{
				m_fDoublePrevBlueRate = fPrevRate;
				m_fDoubleBlueGaugeDownTime  = FRAMEGETTIME();
			}
		}

		if( fRedRate != m_fDoubleRedRate )
		{
			float fPrevRate  = m_fDoubleRedRate;
			m_fDoubleRedRate = fRedRate;
			if( fPrevRate > m_fDoubleRedRate )
			{
				m_fDoublePrevRedRate = fPrevRate;
				m_fDoubleRedGaugeDownTime = FRAMEGETTIME();
			}
		}
	}	
}

void CenterInfoWnd::UpdateDoubleCrownAttack( TeamType eTeam )
{
	if( !IsShow() ) 
		return;
	if( m_DeathTimeAni.GetCurState() != TDA_NONE ) 
		return;

	if( m_DoubleCrownAni.GetCurState() != DCA_LOOP ) 
		return;

	if( eTeam == TEAM_BLUE )
	{
		if( !m_bDoubleBlueAttack )
		{
			m_bDoubleBlueAttack = true;
			m_fDoubleBlueAttackTime = FRAMEGETTIME();
			m_iDoubleBlueVibration = -3;
			m_iDoubleBlueAttackX = ( rand() % 20 ) - 10;
			m_iDoubleBlueAttackY = ( rand() % 20 ) - 10;
			if( rand()%2 == 0 )
				m_pStoneBlueAttack = m_pStoneAttack1;
			else
				m_pStoneBlueAttack = m_pStoneAttack2;
		}
	}
	else if( eTeam == TEAM_RED )
	{
		if( !m_bDoubleRedAttack )
		{
			m_bDoubleRedAttack = true;
			m_fDoubleRedAttackTime = FRAMEGETTIME();
			m_iDoubleRedVibration = -3;
			m_iDoubleRedAttackX = ( rand() % 20 ) - 10;
			m_iDoubleRedAttackY = ( rand() % 20 ) - 10;
			if( rand()%2 == 0 )
				m_pStoneRedAttack = m_pStoneAttack1;
			else
				m_pStoneRedAttack = m_pStoneAttack2;
		}
	}
}

void CenterInfoWnd::UpdateDoubleCrownWinningTeam( TeamType eTeam )
{
	if( !IsShow() )
		return;

	if( m_DeathTimeAni.GetCurState() != TDA_NONE ) 
		return;

	if( m_DoubleCrownAni.GetCurState() != DCA_LOOP ) 
		return;

	m_iDoubleWinningTeam = (int)eTeam;
}

void CenterInfoWnd::UpdateFlagCaptureMode( TeamType eCapture, const ioHashString &szCapture, float fBlueRate, float fRedRate, DWORD dwFlagWaitTime )
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;

	m_eFlagReturn = FR_NONE;
	m_szFlagKiller.Clear();
	m_szFlagDier.Clear();
	m_szFlagCapturer.Clear();
	m_szFlagDroper.Clear();
	m_bFlagNoPointChange = false;
	m_dwFlagWaitRemainTime = REALGETTIME() + dwFlagWaitTime;

	if( eCapture == TEAM_BLUE )
		m_eFlagReturn = FR_BLUE_RETURN;
	else if( eCapture == TEAM_RED )
		m_eFlagReturn = FR_RED_RETURN;

	m_eFlagCarrierTeam = eCapture;
	m_szFlagCarrier = szCapture;

	m_dwReturnAlramTime = REALGETTIME();

	m_iFlagAddBluePoint = max( 0, (int)fBlueRate - m_iFlagBluePoint );
	m_iFlagAddRedPoint  = max( 0, (int)fRedRate - m_iFlagRedPoint );

	m_bFlagBlueScoreUP   = false;	
	if( m_eFlagReturn == FR_BLUE_RETURN )
	{	
		m_bFlagBlueScoreUP = true;
	}
	m_fTimeRate = 0.0f;
	m_fAlphaRate= FLOAT1;
	m_iAddXPos = m_iAddYPos = 0;
	m_FlagCaptureAni.SetCurState( FCA_START );
	if( IsShow() )
		HideWnd();
	ShowWnd();
}

void CenterInfoWnd::UpdateFlagCaptureKO( TeamType eKill, const ioHashString &szKiller, TeamType eDie, const ioHashString &szDier, float fBlueRate, float fRedRate )
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;

	m_eFlagReturn = FR_NONE;
	m_eFlagKillTeam = eKill;
	m_szFlagKiller  = szKiller;
	m_eFlagDieTeam  = eDie;
	m_szFlagDier    = szDier;
	m_szFlagCapturer.Clear();
	m_szFlagDroper.Clear();
	m_szFlagCarrier.Clear();
	m_bFlagNoPointChange = false;
	
	m_iFlagAddBluePoint = max( 0, (int)fBlueRate - m_iFlagBluePoint );
	m_iFlagAddRedPoint  = max( 0, (int)fRedRate - m_iFlagRedPoint );

#ifndef SHIPPING
	LOG.PrintConsoleLog( 0, "CenterInfoWnd::UpdateFlagCaptureKO - m_iFlagBluePoint = %d, m_iFlagRedPoint = %d, fBlueRate = %f, fRedRate = %f", m_iFlagBluePoint, m_iFlagRedPoint, fBlueRate, fRedRate );
	LOG.PrintConsoleLog( 0, "CenterInfoWnd::UpdateFlagCaptureKO - m_iFlagAddBluePoint = %d, m_iFlagAddRedPoint = %d", m_iFlagAddBluePoint, m_iFlagAddRedPoint );
#endif

	m_bFlagBlueScoreUP   = false;	
	if( m_eFlagDieTeam == TEAM_RED )
	{	
		m_bFlagBlueScoreUP = true;
	}
	m_fTimeRate = 0.0f;
	m_fAlphaRate= FLOAT1;
	m_iAddXPos = m_iAddYPos = 0;
	m_FlagCaptureAni.SetCurState( FCA_START );
	if( IsShow() )
		HideWnd();
	ShowWnd();
}

void CenterInfoWnd::UpdateFlagCaptureAlram( TeamType eTeam, const ioHashString &szChar, bool bCapture )
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;

	m_eFlagReturn = FR_NONE;
	m_szFlagKiller.Clear();
	m_szFlagDier.Clear();
	m_szFlagCapturer.Clear();
	m_szFlagDroper.Clear();
	m_szFlagCarrier.Clear();
	m_bFlagNoPointChange = true;

	if( bCapture )
	{
		m_eFlagCapturerTeam = eTeam;
		m_szFlagCapturer = szChar;
	}
	else
	{
		m_eFlagDroperTeam = eTeam;
		m_szFlagDroper = szChar;
	}

	m_fTimeRate = 0.0f;
	m_fAlphaRate= FLOAT1;
	m_iAddXPos = m_iAddYPos = 0;
	m_FlagCaptureAni.SetCurState( FCA_START );
	if( IsShow() )
		HideWnd();
	ShowWnd();
}

void CenterInfoWnd::UpdateFlagCaptureNewFlag()
{
	if( m_DeathTimeAni.GetCurState() != TDA_NONE )
		return;

	m_eFlagReturn = FR_NEW_FLAG;
	m_szFlagKiller.Clear();
	m_szFlagDier.Clear();
	m_szFlagCapturer.Clear();
	m_szFlagDroper.Clear();
	m_szFlagCarrier.Clear();
	m_dwNewFlagTime = REALGETTIME();
	m_bFlagNoPointChange = true;

	m_fTimeRate = 0.0f;
	m_fAlphaRate= FLOAT1;
	m_iAddXPos = m_iAddYPos = 0;
	m_FlagCaptureAni.SetCurState( FCA_START );
	if( IsShow() )
		HideWnd();
	ShowWnd();
}

void CenterInfoWnd::UpdateFlagCaptureWarning( float fBlueRate, float fRedRate )
{
	if( m_DeathTimeAni.GetCurState() != DTA_NONE )
		return;

	m_bFlagCaptureWarningScore = true;
	m_bNoHideWnd = true;

	// 화면에 없으면 표시
	if( m_FlagCaptureAni.GetCurState() == FCA_NONE )
	{
		m_iFlagBluePoint = (int)fBlueRate;
		m_iFlagRedPoint  = (int)fRedRate;
		m_iFlagAddBluePoint = m_iFlagAddRedPoint = 0;

		m_fTimeRate = 0.0f;
		m_fAlphaRate= FLOAT1;
		m_iAddXPos = m_iAddYPos = 0;
		m_FlagCaptureAni.SetCurState( FCA_WARNING_SCORE );
		if( !IsShow() )
			ShowWnd();
	}
}