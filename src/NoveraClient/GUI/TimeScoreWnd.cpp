#include "StdAfx.h"

#include "../ioVoiceChat/Fourier.h"
#include "../io2DCircleGauge.h"

#include "timescorewnd.h"
#include "../ioShuffleRoomMgr.h"

class ioCircleGaugeQuad : public IUIQuad
{
protected:
	io2DCircleGauge *m_pGauge;

public:
	virtual void AddInfo( ioQuadRenderArray *pQuadArray )
	{
		pQuadArray->AddDirectQuadInfo( this );
	}

	virtual int GetQuadCount() const
	{
		return 0;
	}

public:
	virtual bool IsDirectQuad() const
	{
		return true;
	}

	virtual void RenderDirect( ioRenderSystem *pRenderSystem )
	{
		if( m_pGauge )
		{
			m_pGauge->Render( pRenderSystem );
		}
	}

public:
	ioCircleGaugeQuad( io2DCircleGauge *pGauge )
	{
		m_pGauge = pGauge;
	}
	virtual ~ioCircleGaugeQuad() {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TimeScoreWnd::m_bPosToDefualt = false;

TimeScoreWnd::TimeScoreWnd()
{
	m_pCircleGauge = NULL;
	m_pCircleSuddenDeathGauge = NULL;

	m_pTimeIcon    = NULL;
	m_pTimeNumber  = NULL;

	m_pBack         = NULL;
	m_pMold         = NULL;
	m_pWarningBack  = NULL;

	m_pPvETitleBG	  = NULL;
	m_pPvETitleFloor  = NULL;
	m_pPvETitleFloorNum = NULL;
	m_pPvEMonsterCoin  = NULL;
	m_pPvEMonsterCoinX  = NULL;
	m_pPvEMonsterCoinNum  = NULL;

	m_pTeamNumberBlue   = NULL;
	m_pTeamNumberRed    = NULL;
	m_pTitleTeamDeath   = NULL;
	m_pTitleCbt			= NULL;
	m_pTitleUnderwear	= NULL;
	m_pTitleCatch	    = NULL;
	m_pTitleStone	    = NULL;
	m_pTitleKing	    = NULL;
	m_pTitleSoccer	    = NULL;
	m_pTitleHero	    = NULL;
	m_pTitleDeath	    = NULL;
	m_pTitleBoss	    = NULL;
	m_pTitleGangsi	    = NULL;
	m_pTitleRunningMan  = NULL;	
	m_pTitleFalgCapture = NULL;
	m_pTitleArena		= NULL;
	m_pTitleFarming		= NULL;
	m_pTeamColon	    = NULL;
	m_pSingleRank	    = NULL;
	m_pSingleSurvivor   = NULL;

	m_pTitleDoubleCrown   = NULL;
	m_pDoubleCrownSimilar = NULL;
	m_pDoubleBlueCloseWin = NULL;
	m_DoubleRedCloseWin   = NULL;

	m_pShuffleBonus = NULL;
	m_pStarIcon     = NULL;
	m_pStarCross    = NULL;
	m_pStarNumber   = NULL;

	m_eMyTeam = TEAM_NONE;
	m_eModeType = MT_NONE;

	m_eWinningTeam = TEAM_NONE;
	m_iBlueWinCount = 0;
	m_iRedWinCount  = 0;

	m_iFullSecond   = 0;
	m_iRemainSecond = 0;
	m_iBlindOffSetY = 0;
	m_iPreBlindOffSetY = 0;
	m_iDefaultX     = 0;
	m_iDefaultY     = 0;
	m_iPreBlueWinCount = 0;
	m_dwMonsterSurvivalTurn = 0;
	m_dwMonsterSurvivalDifficulty = 0;

	m_bBlueScoreUP = false;
	m_dwTwinkleTime = 0;
	m_iNumberWhiteCount = 0;

	InitShowAniState();
	InitWarningAniState();
	InitScoreChangeState();
	
	m_bSuddenDeadth = false;

	m_iSuddnDeathFullTime = 0;
	m_iSuddnDeathRemainTime = 0;

	m_iShuffleBonusStarCount = 0;

	m_pTitleSuccession  = NULL;
	m_pSuccessionText	= NULL;
	m_pSuccessionNumber = NULL;
}

TimeScoreWnd::~TimeScoreWnd()
{
	SAFEDELETE( m_pCircleGauge );
	SAFEDELETE( m_pCircleSuddenDeathGauge );
	SAFEDELETE( m_pTimeIcon );
	SAFEDELETE( m_pTimeNumber ); 

	SAFEDELETE( m_pBack );         
	SAFEDELETE( m_pMold );         
	SAFEDELETE( m_pWarningBack );

	SAFEDELETE( m_pPvETitleBG );
	SAFEDELETE( m_pPvETitleFloor );
	SAFEDELETE( m_pPvETitleFloorNum );
	SAFEDELETE( m_pPvEMonsterCoin );
	SAFEDELETE( m_pPvEMonsterCoinX );
	SAFEDELETE( m_pPvEMonsterCoinNum );

	SAFEDELETE( m_pTeamNumberBlue );   
	SAFEDELETE( m_pTeamNumberRed );    
	SAFEDELETE( m_pTitleTeamDeath );
	SAFEDELETE( m_pTitleCbt );
	SAFEDELETE( m_pTitleUnderwear );
	SAFEDELETE( m_pTitleCatch );
	SAFEDELETE( m_pTitleStone );
	SAFEDELETE( m_pTitleKing );
	SAFEDELETE( m_pTitleSoccer );
	SAFEDELETE( m_pTitleHero );
	SAFEDELETE( m_pTitleDeath );
	SAFEDELETE( m_pTitleBoss );
	SAFEDELETE( m_pTitleGangsi );
	SAFEDELETE( m_pTitleRunningMan );
	SAFEDELETE( m_pTitleDoubleCrown );
	SAFEDELETE( m_pTitleFalgCapture );
	SAFEDELETE( m_pTitleArena );
	SAFEDELETE( m_pTitleFarming );
	SAFEDELETE( m_pDoubleCrownSimilar );
	SAFEDELETE( m_pDoubleBlueCloseWin );
	SAFEDELETE( m_DoubleRedCloseWin );

	SAFEDELETE( m_pTeamColon );
	SAFEDELETE( m_pSingleRank );
	SAFEDELETE( m_pSingleSurvivor );

	SAFEDELETE( m_pShuffleBonus );
	SAFEDELETE( m_pStarIcon );
	SAFEDELETE( m_pStarCross );
	SAFEDELETE( m_pStarNumber );

	SAFEDELETE( m_pTitleSuccession );
	SAFEDELETE( m_pSuccessionText );
	SAFEDELETE( m_pSuccessionNumber );

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "TimeScoreWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "TimeScoreWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void TimeScoreWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TimeIcon" )
	{
		SAFEDELETE( m_pTimeIcon );
		m_pTimeIcon = pImage;
	}
	else if( szType == "TimeNumber" )
	{
		SAFEDELETE( m_pTimeNumber );
		m_pTimeNumber = pImage;
	}	
	else if( szType == "Back" )
	{
		SAFEDELETE( m_pBack );
		m_pBack = pImage;
	}
	else if( szType == "Mold" )
	{
		SAFEDELETE( m_pMold );
		m_pMold = pImage;
	}
	else if( szType == "WarningBack" )
	{
		SAFEDELETE( m_pWarningBack );
		m_pWarningBack = pImage;
	}
	else if( szType == "PvETitleBG" )
	{
		SAFEDELETE( m_pPvETitleBG );
		m_pPvETitleBG = pImage;
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
	else if( szType == "TeamNumberBlue" )
	{
		SAFEDELETE( m_pTeamNumberBlue );
		m_pTeamNumberBlue = pImage;
	}
	else if( szType == "TeamNumberRed" )
	{
		SAFEDELETE( m_pTeamNumberRed );
		m_pTeamNumberRed = pImage;
	}
	else if( szType == "TitleTeamDeath" )
	{
		SAFEDELETE( m_pTitleTeamDeath );
		m_pTitleTeamDeath = pImage;
	}
	else if( szType == "TitleCbt" )
	{
		SAFEDELETE( m_pTitleCbt );
		m_pTitleCbt = pImage;
	}
	else if( szType == "TitleUnderwear" )
	{
		SAFEDELETE( m_pTitleUnderwear );
		m_pTitleUnderwear = pImage;
	}
	else if( szType == "TitleCatch" )
	{
		SAFEDELETE( m_pTitleCatch );
		m_pTitleCatch = pImage;
	}
	else if( szType == "TitleStone" )
	{
		SAFEDELETE( m_pTitleStone );
		m_pTitleStone = pImage;
	}
	else if( szType == "TitleKing" )
	{
		SAFEDELETE( m_pTitleKing );
		m_pTitleKing = pImage;
	}
	else if( szType == "TitleSoccer" )
	{
		SAFEDELETE( m_pTitleSoccer );
		m_pTitleSoccer = pImage;
	}
	else if( szType == "TitleHero" )
	{
		SAFEDELETE( m_pTitleHero );
		m_pTitleHero = pImage;
	}
	else if( szType == "TitleDeath" )
	{
		SAFEDELETE( m_pTitleDeath );
		m_pTitleDeath = pImage;
	}
	else if( szType == "TitleBoss" )
	{
		SAFEDELETE( m_pTitleBoss );
		m_pTitleBoss = pImage;
	}
	else if( szType == "TitleGangsi" )
	{
		SAFEDELETE( m_pTitleGangsi );
		m_pTitleGangsi = pImage;
	}
	else if( szType == "TitleRunningMan" )
	{
		SAFEDELETE( m_pTitleRunningMan );
		m_pTitleRunningMan = pImage;
	}
	else if( szType == "TitleFlagCapture" )
	{
		SAFEDELETE( m_pTitleFalgCapture );
		m_pTitleFalgCapture = pImage;
	}
	else if( szType == "TitleArena" )
	{
		SAFEDELETE( m_pTitleArena );
		m_pTitleArena = pImage;
	}
	else if( szType == "TitleFarming" )
	{
		SAFEDELETE( m_pTitleFarming );
		m_pTitleFarming = pImage;
	}
	else if( szType == "TeamColon" )
	{
		SAFEDELETE( m_pTeamColon );
		m_pTeamColon = pImage;
	}
	else if( szType == "SingleRank" )
	{
		SAFEDELETE( m_pSingleRank );
		m_pSingleRank = pImage;
	}
	else if( szType == "SingleSurvivor" )
	{
		SAFEDELETE( m_pSingleSurvivor );
		m_pSingleSurvivor = pImage;
	}
	else if( szType == "TitleDoubleCrown" )
	{
		SAFEDELETE( m_pTitleDoubleCrown );
		m_pTitleDoubleCrown = pImage;
	}
	else if( szType == "DoubleCrownSimilar" )
	{
		SAFEDELETE( m_pDoubleCrownSimilar );
		m_pDoubleCrownSimilar = pImage;
	}
	else if( szType == "DoubleBlueCloseWin" )
	{
		SAFEDELETE( m_pDoubleBlueCloseWin );
		m_pDoubleBlueCloseWin = pImage;
	}
	else if( szType == "DoubleRedCloseWin" )
	{
		SAFEDELETE( m_DoubleRedCloseWin );
		m_DoubleRedCloseWin = pImage;
	}
	else if( szType == "TitleShuffleBonus" )
	{
		SAFEDELETE( m_pShuffleBonus );
		m_pShuffleBonus = pImage;
	}
	else if( szType == "ShuffleBonusStarIcon" )
	{
		SAFEDELETE( m_pStarIcon );
		m_pStarIcon = pImage;
	}
	else if( szType == "ShuffleBonusCross" )
	{
		SAFEDELETE( m_pStarCross );
		m_pStarCross = pImage;
	}
	else if( szType == "ShuffleBonusNumber" )
	{
		SAFEDELETE( m_pStarNumber );
		m_pStarNumber = pImage;
	}
	else if( szType == "TitleSuccession" )
	{
		SAFEDELETE( m_pTitleSuccession );
		m_pTitleSuccession = pImage;
	}
	else if( szType == "SuccessionText" )
	{
		SAFEDELETE( m_pSuccessionText );
		m_pSuccessionText = pImage;
	}
	else if( szType == "SuccessionNumber" )
	{
		SAFEDELETE( m_pSuccessionNumber );
		m_pSuccessionNumber = pImage;
	}
	else 
	{
		//UJ 140113, 보안강화: 인니 Unlimited bronze coin 부분
		_ENCSTR("PvEMonsterCoin", szEncPvEMonsterCoin);
		_ENCSTR("PvEMonsterCoinX", szEncPvEMonsterCoinX);
		_ENCSTR("PvEMonsterCoinNum", szEncPvEMonsterCoinNum);
		if( strcmp(szType.c_str(), szEncPvEMonsterCoin) == 0 )
		{
			SAFEDELETE( m_pPvEMonsterCoin );
			m_pPvEMonsterCoin = pImage;
		}
		else if( strcmp(szType.c_str(), szEncPvEMonsterCoinX) == 0 )
		{
			SAFEDELETE( m_pPvEMonsterCoinX );
			m_pPvEMonsterCoinX = pImage;
		}
		else if( strcmp(szType.c_str(), szEncPvEMonsterCoinNum) == 0 )
		{
			SAFEDELETE( m_pPvEMonsterCoinNum );
			m_pPvEMonsterCoinNum = pImage;
		}
		else
			ioWnd::AddRenderImage( szType, pImage );
	}
}

void TimeScoreWnd::OnRender()
{
	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	switch( m_eModeType )
	{
	case MT_TEAM_SURVIVAL:
	case MT_TEAM_SURVIVAL_AI:
	case MT_BATTLE:
		RenderTimeGauge( iXPos, iYPos );
		RenderTeamDeathScore( iXPos, iYPos );
		break;
	case MT_DOUBLE_CROWN:
		RenderTimeGauge( iXPos, iYPos );
		RenderDobuleCrownWinning( iXPos, iYPos );		
		break;
	case MT_SURVIVAL:
	case MT_FIGHT_CLUB:
		RenderTimeGauge( iXPos, iYPos );
		RenderSingleScore( iXPos, iYPos );
		break;
	case MT_BOSS:
		RenderTimeGauge( iXPos, iYPos );
		RenderBossScore( iXPos, iYPos );
		break;
	case MT_MONSTER_SURVIVAL:
	case MT_DUNGEON_A:
		RenderTimeGauge( iXPos, iYPos );
		RenderMonsterSurvival( iXPos, iYPos );
		break;
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_FACTORY:
		//TODO jal 요기에서 왼쪽 위 렌더 수정 필요
//		RenderMonsterSurvival( iXPos, iYPos );  
		break;
	case MT_GANGSI:
		RenderTimeGauge( iXPos, iYPos );
		RenderGangsiScore( iXPos, iYPos );
		break;
	case MT_SHUFFLE_BONUS:
		RenderTimeGauge( iXPos, iYPos );
		RenderShuffleBonusStar( iXPos, iYPos );
		break;
	case MT_SUCCESSION:
		RenderTimeGauge( iXPos, iYPos );
		RenderSuccessionNumber( iXPos, iYPos );
		break;
	case MT_FLAG_CAPTURE:
		RenderTimeGauge( iXPos, iYPos );
		RenderFlagCaptureScore( iXPos, iYPos );
		break;
	default:
		RenderTimeGauge( iXPos, iYPos );
		RenderTeamScore( iXPos, iYPos );
		break;
	}
}

void TimeScoreWnd::RenderTimeGauge( int iXPos, int iYPos )
{
	if( !m_pBack || !m_pWarningBack || !m_pCircleGauge || !m_pMold || !m_pTimeIcon || !m_pTimeNumber || !m_pCircleSuddenDeathGauge )
		return;

	enum { BACK_XOFFSET = 11, BACK_YOFFSET = 6, };
	// 뒷배경 출력
	m_pBack->Render( iXPos+BACK_XOFFSET, iYPos+BACK_YOFFSET );

	if( m_WarningAniState.GetCurState() == WAT_HIDING)
	{
		float fRate = FLOAT1 - ( (float) m_WarningAniState.GetCurTime() / (float) m_WarningAniState.GetDelay() );
		m_pWarningBack->SetAlpha( 255.0f * fRate );
		m_pWarningBack->Render( iXPos+BACK_XOFFSET, iYPos+BACK_YOFFSET );
	}

	if( m_pCircleGauge->IsNeedRender() )
	{
		// 소멸은 g_UIRenderer가 해준다.
		ioCircleGaugeQuad *pQuad = new ioCircleGaugeQuad( m_pCircleGauge );
		if( pQuad )
		{
			g_UIRenderer.AddUIQuad( pQuad );
		}
	}

	if( m_pCircleSuddenDeathGauge->IsNeedRender() )
	{
		ioCircleGaugeQuad *pQuad = new ioCircleGaugeQuad( m_pCircleSuddenDeathGauge );
		if( pQuad )
		{
			g_UIRenderer.AddUIQuad( pQuad );
		}
	}

	int iSecondTime = 0;
	if( g_BattleRoomMgr.IsTournamentModeType() )
	{		
		if( 0 < m_iRemainSecond )
			iSecondTime = m_iRemainSecond;
		else
			iSecondTime = m_iSuddnDeathRemainTime;
	}
	else
	{
		iSecondTime = m_iRemainSecond;		
	}

	if( iSecondTime <= 0 )
		iSecondTime = 0;

	// 윗부분 출력...
	m_pMold->Render( iXPos+BACK_XOFFSET, iYPos+BACK_YOFFSET, UI_RENDER_ADD );
	m_pTimeIcon->Render( iXPos+68, iYPos+13, UI_RENDER_ADD );

	enum { MIN_SECOND_POS = 10, MIN_THIRD_POS = 100 , FIRST_XOFFSET = 77, SECOND_XOFFST = 84, THIRD_XOFFST = 91, };
	m_pTimeNumber->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	if( iSecondTime < MIN_SECOND_POS )
	{
		m_pTimeNumber->RenderNum( iXPos + FIRST_XOFFSET, iYPos + 14, 0, FLOAT1 , FLOAT1, UI_RENDER_ADD );
		m_pTimeNumber->RenderNum( iXPos + SECOND_XOFFST, iYPos + 14, 0, FLOAT1 , FLOAT1, UI_RENDER_ADD );
		m_pTimeNumber->RenderNum( iXPos + THIRD_XOFFST, iYPos + 14, iSecondTime, FLOAT1 , FLOAT1, UI_RENDER_ADD );
	}
	else if( iSecondTime < MIN_THIRD_POS )
	{
		m_pTimeNumber->RenderNum( iXPos + FIRST_XOFFSET, iYPos + 14, 0, FLOAT1 , FLOAT1, UI_RENDER_ADD );
		m_pTimeNumber->RenderNum( iXPos + SECOND_XOFFST, iYPos + 14, iSecondTime, FLOAT1 , FLOAT1, UI_RENDER_ADD );
	}
	else
		m_pTimeNumber->RenderNum( iXPos + FIRST_XOFFSET, iYPos + 14, iSecondTime, FLOAT1 , FLOAT1, UI_RENDER_ADD );
}

void TimeScoreWnd::RenderTeamScore( int iXPos, int iYPos )
{
	int iModeYPosGap = 0;
	switch( m_eModeType )
	{
	case MT_UNDERWEAR:
		if( m_pTitleUnderwear )
			m_pTitleUnderwear->Render( iXPos + 39, iYPos + 39 );
		break;
	case MT_CBT:
		if( m_pTitleCbt )
			m_pTitleCbt->Render( iXPos + 39, iYPos + 39 );
		break;
	case MT_CATCH:
		if( m_pTitleCatch )
			m_pTitleCatch->Render( iXPos + 39, iYPos + 39 );
		break;
	case MT_CATCH_RUNNINGMAN:
		if( m_pTitleRunningMan )
			m_pTitleRunningMan->Render( iXPos + 39, iYPos + 39 );		
		break;
	case MT_SYMBOL:
		if( m_pTitleStone )
			m_pTitleStone->Render( iXPos + 39, iYPos + 39 );
		break;
	case MT_KING:
		if( m_pTitleKing )
			m_pTitleKing->Render( iXPos + 39, iYPos + 39 );
		break;
	case MT_FOOTBALL:
		if( m_pTitleSoccer )
			m_pTitleSoccer->Render( iXPos + 39, iYPos + 39 );
		break;
	case MT_HEROMATCH:
		if( m_pTitleHero )
			m_pTitleHero->Render( iXPos + 39, iYPos + 39 );
		break;
	case MT_ARENA:
		if( m_pTitleArena )
			m_pTitleArena->Render( iXPos + 39, iYPos + 39 );
		break;
	case MT_FARMING:
		if( m_pTitleFarming )
			m_pTitleFarming->Render( iXPos + 39, iYPos + 39 );
		break;
	}

	if( m_pTeamColon )
		m_pTeamColon->Render( iXPos + 78, iYPos + 71 + iModeYPosGap );

	if( !m_pTeamNumberBlue || !m_pTeamNumberRed ) 
		return;

	// blue
	m_pTeamNumberBlue->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	m_pTeamNumberBlue->RenderNum( iXPos + 43, iYPos + 63 + iModeYPosGap, m_iBlueWinCount );
	
	// red
	m_pTeamNumberRed->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	m_pTeamNumberRed->RenderNum( iXPos + 92, iYPos + 63 + iModeYPosGap, m_iRedWinCount );
}

void TimeScoreWnd::RenderTeamDeathScore( int iXPos, int iYPos )
{
	if( m_pTitleTeamDeath )
		m_pTitleTeamDeath->Render( iXPos + 39, iYPos + 39 );

	if( m_pTeamColon )
		m_pTeamColon->Render( iXPos + 78, iYPos + 71 );

	if( !m_pTeamNumberBlue || !m_pTeamNumberRed ) 
		return;

    // blue
	m_pTeamNumberBlue->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	if( m_iBlueWinCount < 100 )
	{
		m_pTeamNumberBlue->RenderNum( iXPos + 19, iYPos + 63, m_iBlueWinCount / 10 );
		m_pTeamNumberBlue->RenderNum( iXPos + 45, iYPos + 63, m_iBlueWinCount % 10 );

		if( m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberBlue->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberBlue->RenderNum( iXPos + 19, iYPos + 63, m_iBlueWinCount / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberBlue->RenderNum( iXPos + 45, iYPos + 63, m_iBlueWinCount % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}
	}
	else
	{
		m_pTeamNumberBlue->RenderNum( iXPos - 7, iYPos + 63, m_iBlueWinCount / 100 );
		m_pTeamNumberBlue->RenderNum( iXPos + 19, iYPos + 63, (m_iBlueWinCount % 100) / 10 );
		m_pTeamNumberBlue->RenderNum( iXPos + 45, iYPos + 63, m_iBlueWinCount % 10 );		

		if( m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberBlue->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberBlue->RenderNum( iXPos - 7, iYPos + 63, m_iBlueWinCount / 100, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberBlue->RenderNum( iXPos + 19, iYPos + 63, (m_iBlueWinCount % 100) / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberBlue->RenderNum( iXPos + 45, iYPos + 63, m_iBlueWinCount % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );		
		}
	}

	// red
	m_pTeamNumberRed->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	if( m_iRedWinCount < 100 )
	{
		m_pTeamNumberRed->RenderNum( iXPos + 90, iYPos + 63, m_iRedWinCount / 10 );
		m_pTeamNumberRed->RenderNum( iXPos + 116, iYPos + 63, m_iRedWinCount % 10 );

		if( !m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberRed->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberRed->RenderNum( iXPos + 90, iYPos + 63, m_iRedWinCount / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberRed->RenderNum( iXPos + 116, iYPos + 63, m_iRedWinCount % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}		
	}
	else
	{
		int iTemp = m_iRedWinCount % 100;
		m_pTeamNumberRed->RenderNum( iXPos + 90, iYPos + 63 , m_iRedWinCount / 100 );
		m_pTeamNumberRed->RenderNum( iXPos + 116, iYPos + 63 , (m_iRedWinCount % 100) / 10 );	
		m_pTeamNumberRed->RenderNum( iXPos + 142, iYPos + 62 , m_iRedWinCount % 10 );		

		if( !m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberRed->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberRed->RenderNum( iXPos + 90, iYPos + 63 , m_iRedWinCount / 100, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberRed->RenderNum( iXPos + 116, iYPos + 63 , (m_iRedWinCount % 100) / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );	
			m_pTeamNumberRed->RenderNum( iXPos + 142, iYPos + 62 , m_iRedWinCount % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}		
	}
}

void TimeScoreWnd::RenderSingleScore( int iXPos, int iYPos )
{
	if( m_pTitleDeath )
		m_pTitleDeath->Render( iXPos + 39, iYPos + 39 );

	if( !m_pTeamNumberBlue || !m_pSingleRank )
		return;

	int iSingleRank = m_iBlueWinCount;	
	if( iSingleRank < 10 )
	{
		m_pTeamNumberBlue->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		m_pTeamNumberBlue->RenderNum( iXPos + 52, iYPos + 63, iSingleRank );
		m_pSingleRank->Render( iXPos + 83, iYPos + 63 );

		if( m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberBlue->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberBlue->RenderNum( iXPos + 52, iYPos + 63, iSingleRank, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}
	}
	else
	{
		m_pTeamNumberBlue->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		m_pTeamNumberBlue->RenderNum( iXPos + 37, iYPos + 63, iSingleRank / 10 );
		m_pTeamNumberBlue->RenderNum( iXPos + 64, iYPos + 63, iSingleRank % 10 );
		m_pSingleRank->Render( iXPos + 95, iYPos + 63 );

		if( m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberBlue->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberBlue->RenderNum( iXPos + 37, iYPos + 63, iSingleRank / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberBlue->RenderNum( iXPos + 64, iYPos + 63, iSingleRank % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}
	}
	
}

void TimeScoreWnd::RenderBossScore( int iXPos, int iYPos )
{
	if( m_pTitleBoss )
		m_pTitleBoss->Render( iXPos + 39, iYPos + 39 );

	if( !m_pTeamNumberBlue || !m_pTeamNumberRed || !m_pSingleRank )
		return;

	int iSingleRank = m_iBlueWinCount;
	if( iSingleRank < 10 )
	{
		if( m_eMyTeam == TEAM_BLUE )
		{
			m_pTeamNumberBlue->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
			m_pTeamNumberBlue->RenderNum( iXPos + 52, iYPos + 63, iSingleRank );
		}
		else
		{
			m_pTeamNumberRed->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
			m_pTeamNumberRed->RenderNum( iXPos + 52, iYPos + 63, iSingleRank );
		}
		m_pSingleRank->Render( iXPos + 83, iYPos + 63 );
	}
	else
	{
		if( m_eMyTeam == TEAM_BLUE )
		{
			m_pTeamNumberBlue->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
			m_pTeamNumberBlue->RenderNum( iXPos + 37, iYPos + 63, iSingleRank / 10 );
			m_pTeamNumberBlue->RenderNum( iXPos + 64, iYPos + 63, iSingleRank % 10 );
		}
		else
		{
			m_pTeamNumberRed->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
			m_pTeamNumberRed->RenderNum( iXPos + 37, iYPos + 63, iSingleRank / 10 );
			m_pTeamNumberRed->RenderNum( iXPos + 64, iYPos + 63, iSingleRank % 10 );
		}
		m_pSingleRank->Render( iXPos + 95, iYPos + 63 );
	}
}

void TimeScoreWnd::RenderGangsiScore( int iXPos, int iYPos )
{
	if( m_pTitleGangsi )
		m_pTitleGangsi->Render( iXPos + 39, iYPos + 39 );

	if( !m_pTeamNumberRed || !m_pSingleSurvivor )	return;

	if( m_iRedWinCount < 10 )
	{
		m_pTeamNumberRed->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		m_pTeamNumberRed->RenderNum( iXPos + 52, iYPos + 63 , m_iRedWinCount );	

		if( !m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberRed->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberRed->RenderNum( iXPos + 52, iYPos + 63 , m_iRedWinCount, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}
		m_pSingleSurvivor->Render( iXPos + 83, iYPos + 63 );
	}	
	else
	{
		m_pTeamNumberRed->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		m_pTeamNumberRed->RenderNum( iXPos + 37, iYPos + 63, m_iRedWinCount / 10 );
		m_pTeamNumberRed->RenderNum( iXPos + 64, iYPos + 63, m_iRedWinCount % 10 );

		if( !m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberRed->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberRed->RenderNum( iXPos + 37, iYPos + 63 , m_iRedWinCount / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberRed->RenderNum( iXPos + 64, iYPos + 63 , m_iRedWinCount % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}
		m_pSingleSurvivor->Render( iXPos + 95, iYPos + 63 );
	}
}

void TimeScoreWnd::RenderMonsterSurvival( int iXPos, int iYPos )
{
	if( !m_pPvETitleBG || !m_pPvETitleFloor || !m_pPvETitleFloorNum || !m_pPvEMonsterCoin || !m_pPvEMonsterCoinX || !m_pPvEMonsterCoinNum ) 
		return;

	m_pPvETitleBG->Render( iXPos + 2, iYPos + 80 );

	// 층 수 표시
	enum
	{
		FLOOR_NUM_SIZE = 37,
		FLOOR_NUM_GAP  = 12,
		FLOOR_NUM_TEXT_GAP = 9,
		FLOOR_NUM_TEXT_SIZE = 35,
	};
	int iFloorXPos = iXPos + 84;
	int iFloorNumCount  = GetNumberCount( m_dwMonsterSurvivalTurn );
	int iTotalFloorNum  = ( FLOOR_NUM_SIZE * iFloorNumCount ) - ( ( iFloorNumCount - 1 ) * FLOOR_NUM_GAP ); 
	int iCenterFloorPos = ( ( iTotalFloorNum - FLOOR_NUM_TEXT_GAP ) + FLOOR_NUM_TEXT_SIZE ) / 2;

	iFloorXPos -= iCenterFloorPos;
	m_pPvETitleFloorNum->RenderNum( iFloorXPos, iYPos + 35, m_dwMonsterSurvivalTurn, -FLOOR_NUM_GAP );
	iFloorXPos += ( iTotalFloorNum - FLOOR_NUM_TEXT_GAP );
	m_pPvETitleFloor->Render( iFloorXPos, iYPos + 49 );

	// 내 코인 갯수 표시
	enum
	{
		COIN_ICON_SIZE = 28,
		COIN_X_SIZE    = 14,
		COIN_NUM_SIZE  = 26,
		COIN_NUM_GAP   = 7,
	};
	int iCoinXPos = iXPos + 84;
	int iMonsterCoin = g_MyInfo.GetEtcGoldMonsterCoin();
	int iCoinNumCount = GetNumberCount( iMonsterCoin );
	int iTotalCoinNum = ( COIN_NUM_SIZE * iCoinNumCount ) - ( (iCoinNumCount - 1) * COIN_NUM_GAP );
	int iCenterCoinPos= ( COIN_ICON_SIZE + COIN_X_SIZE + iTotalCoinNum ) / 2;

	iCoinXPos -= iCenterCoinPos;
	m_pPvEMonsterCoin->SetScale( 0.41f, 0.41f );
	m_pPvEMonsterCoin->Render( iCoinXPos + ( m_pPvEMonsterCoin->GetOffsetX() * 0.59f ), iYPos + 88 + ( m_pPvEMonsterCoin->GetOffsetY() * 0.59f ), UI_RENDER_NORMAL, TFO_BILINEAR );
	iCoinXPos += COIN_ICON_SIZE;
	m_pPvEMonsterCoinX->Render( iCoinXPos, iYPos + 99 );
	iCoinXPos += COIN_X_SIZE;
	m_pPvEMonsterCoinNum->RenderNum( iCoinXPos, iYPos + 93, iMonsterCoin, -COIN_NUM_GAP );
}

void TimeScoreWnd::RenderDobuleCrownWinning( int iXPos, int iYPos )
{
	if( m_pTitleDoubleCrown )
		m_pTitleDoubleCrown->Render( iXPos + 39, iYPos + 39 );
	
	switch( m_eWinningTeam )
	{
	case TEAM_BLUE:
		{
			if( m_pDoubleBlueCloseWin )
				m_pDoubleBlueCloseWin->Render( iXPos + 84, iYPos + 65 );
		}
		break;
	case TEAM_RED:
		{
			if( m_DoubleRedCloseWin )
				m_DoubleRedCloseWin->Render( iXPos + 84, iYPos + 65 );
		}
		break;
	default:
		{
			if( m_pDoubleCrownSimilar )		
				m_pDoubleCrownSimilar->Render( iXPos + 84, iYPos + 65 );		
		}
		break;
	}
}

void TimeScoreWnd::RenderShuffleBonusStar( int iXPos, int iYPos )
{
	if( m_pShuffleBonus )
		m_pShuffleBonus->Render( iXPos + 41, iYPos + 38 );

	if( !m_pStarIcon && !m_pStarCross && !m_pStarNumber )
		return;

	int iStarWidth = m_pStarIcon->GetWidth();
	int iCrossWidth = m_pStarCross->GetWidth();
	int iNumberWidth = m_pStarNumber->GetWidth() / 10;

	int iStarCnt = g_ShuffleRoomMgr.GetShuffleStarCnt();

	if( iStarCnt < 10 )
		iNumberWidth *= 1;
	else if( iStarCnt < 100 )
		iNumberWidth *= 2;
	else if( iStarCnt < 1000 )
		iNumberWidth *= 3;

	int iXOffset = iXPos + 79 - ( ( iStarWidth + iCrossWidth + iNumberWidth - 25 ) / 2 );

	m_pStarIcon->Render( iXOffset, iYPos + 53, UI_RENDER_NORMAL, TFO_BILINEAR );

	iXOffset += iStarWidth - 24;
	m_pStarCross->Render( iXOffset, iYPos + 77 );

	iXOffset += iCrossWidth - 1;
	m_pStarNumber->RenderNum( iXOffset, iYPos + 69, iStarCnt, -FLOAT1 );
}

void TimeScoreWnd::RenderSuccessionNumber( int iXPos, int iYPos )
{
	if( m_pTitleSuccession )
		m_pTitleSuccession->Render( iXPos + 41, iYPos + 41 );

	const int& rkCurSuccessionCnt = g_MyInfo.GetCurSuccessionCount();
	//int rkCurSuccessionCnt = m_iTest;
	if ( m_pSuccessionNumber && m_pSuccessionText )
	{
		//999승 이상
		if ( rkCurSuccessionCnt > 999 )
		{
			m_pSuccessionNumber->RenderNum( iXPos + 10, iYPos + 64, 9, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pSuccessionNumber->RenderNum( iXPos + 35, iYPos + 64, 9, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pSuccessionNumber->RenderNum( iXPos + 60, iYPos + 64, 9, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pSuccessionText->Render( iXPos + 93, iYPos + 70 );
		}
		//세자리
		else if ( rkCurSuccessionCnt >= 100 )
		{
			int iH = rkCurSuccessionCnt/100;
			int iT = (iH*100-rkCurSuccessionCnt)/10;
			int iO = rkCurSuccessionCnt%10;
			m_pSuccessionNumber->RenderNum( iXPos + 10, iYPos + 64, iH, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pSuccessionNumber->RenderNum( iXPos + 35, iYPos + 64, iT, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pSuccessionNumber->RenderNum( iXPos + 60, iYPos + 64, iO, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pSuccessionText->Render( iXPos + 93, iYPos + 70 );
		}
		//두자리
		else if ( rkCurSuccessionCnt >= 10 )
		{
			int iT = rkCurSuccessionCnt/10;
			int iO = rkCurSuccessionCnt%10;
			m_pSuccessionNumber->RenderNum( iXPos + 23, iYPos + 64, iT, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pSuccessionNumber->RenderNum( iXPos + 48, iYPos + 64, iO, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pSuccessionText->Render( iXPos + 81, iYPos + 70 );
		}
		else
		{
			m_pSuccessionNumber->RenderNum( iXPos + 35, iYPos + 64, rkCurSuccessionCnt, FLOAT1 , FLOAT1, UI_RENDER_NORMAL );
			m_pSuccessionText->Render( iXPos + 68, iYPos + 70 );
		}
	}
}

void TimeScoreWnd::RenderFlagCaptureScore( int iXPos, int iYPos )
{
	if( m_pTitleFalgCapture )
		m_pTitleFalgCapture->Render( iXPos + 39, iYPos + 39 );

	if( m_pTeamColon )
		m_pTeamColon->Render( iXPos + 78, iYPos + 71 );

	if( !m_pTeamNumberBlue || !m_pTeamNumberRed ) 
		return;

	// blue
	m_pTeamNumberBlue->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	if( m_iBlueWinCount < 100 )
	{
		m_pTeamNumberBlue->RenderNum( iXPos + 19, iYPos + 63, m_iBlueWinCount / 10 );
		m_pTeamNumberBlue->RenderNum( iXPos + 45, iYPos + 63, m_iBlueWinCount % 10 );

		if( m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberBlue->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberBlue->RenderNum( iXPos + 19, iYPos + 63, m_iBlueWinCount / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberBlue->RenderNum( iXPos + 45, iYPos + 63, m_iBlueWinCount % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}
	}
	else
	{
		m_pTeamNumberBlue->RenderNum( iXPos - 7, iYPos + 63, m_iBlueWinCount / 100 );
		m_pTeamNumberBlue->RenderNum( iXPos + 19, iYPos + 63, (m_iBlueWinCount % 100) / 10 );
		m_pTeamNumberBlue->RenderNum( iXPos + 45, iYPos + 63, m_iBlueWinCount % 10 );		

		if( m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberBlue->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberBlue->RenderNum( iXPos - 7, iYPos + 63, m_iBlueWinCount / 100, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberBlue->RenderNum( iXPos + 19, iYPos + 63, (m_iBlueWinCount % 100) / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberBlue->RenderNum( iXPos + 45, iYPos + 63, m_iBlueWinCount % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );		
		}
	}

	// red
	m_pTeamNumberRed->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
	if( m_iRedWinCount < 100 )
	{
		m_pTeamNumberRed->RenderNum( iXPos + 90, iYPos + 63, m_iRedWinCount / 10 );
		m_pTeamNumberRed->RenderNum( iXPos + 116, iYPos + 63, m_iRedWinCount % 10 );

		if( !m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberRed->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberRed->RenderNum( iXPos + 90, iYPos + 63, m_iRedWinCount / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberRed->RenderNum( iXPos + 116, iYPos + 63, m_iRedWinCount % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}		
	}
	else
	{
		int iTemp = m_iRedWinCount % 100;
		m_pTeamNumberRed->RenderNum( iXPos + 90, iYPos + 63 , m_iRedWinCount / 100 );
		m_pTeamNumberRed->RenderNum( iXPos + 116, iYPos + 63 , (m_iRedWinCount % 100) / 10 );	
		m_pTeamNumberRed->RenderNum( iXPos + 142, iYPos + 62 , m_iRedWinCount % 10 );		

		if( !m_bBlueScoreUP && m_iNumberWhiteCount % 2 == 1 )
		{
			m_pTeamNumberRed->SetColor( TCT_DEFAULT_WHITE );
			m_pTeamNumberRed->RenderNum( iXPos + 90, iYPos + 63 , m_iRedWinCount / 100, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
			m_pTeamNumberRed->RenderNum( iXPos + 116, iYPos + 63 , (m_iRedWinCount % 100) / 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );	
			m_pTeamNumberRed->RenderNum( iXPos + 142, iYPos + 62 , m_iRedWinCount % 10, 0.0f, FLOAT1, UI_RENDER_NORMAL_ADD_COLOR );
		}		
	}
}

void TimeScoreWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	UpdateAniState();
	UpdateGauge();
	UpdateNumberTwinkle();
	
	ioMovingWnd::OnProcess( fTimePerSec );
}

void TimeScoreWnd::iwm_create()
{
	m_pCircleGauge = new io2DCircleGauge;
	if( m_pCircleGauge)                
		m_pCircleGauge->InitResource( "score_gauge.dds", 300, 23, 23, io2DCircleGauge::ST_6OCLOCK );

	m_pCircleSuddenDeathGauge = new io2DCircleGauge;
	if( m_pCircleSuddenDeathGauge)                
		m_pCircleSuddenDeathGauge->InitResource( "score_gauge2.dds", 300, 23, 23, io2DCircleGauge::ST_6OCLOCK );

	int iXPos = g_GUIMgr.GUIPosLoadInt( "TimeScoreWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "TimeScoreWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();

	if( m_bPosToDefualt )
	{
		m_bPosToDefualt = false;
		SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}
}

void TimeScoreWnd::UpdateNumberTwinkle()
{
	if( m_dwTwinkleTime == 0 ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwTwinkleTime;
	if( dwGapTime > 35 )
	{
		m_iNumberWhiteCount++;
		m_dwTwinkleTime = FRAMEGETTIME();
	}

	// 4번 반복
	if( m_iNumberWhiteCount >= 8 )
	{
		m_iNumberWhiteCount = 0;
		m_dwTwinkleTime = 0;
	}
}

void TimeScoreWnd::UpdateWinningTeam( TeamType eWinningTeam )
{
	m_eWinningTeam = eWinningTeam;
}

void TimeScoreWnd::UpdateScore( int iBlueWinCount , int iRedWinCount )
{
	m_iBlueWinCount = iBlueWinCount;
	m_iRedWinCount  = iRedWinCount;

	if( m_eModeType == MT_SYMBOL ||
		m_eModeType == MT_UNDERWEAR ||
		m_eModeType == MT_CBT ||
		m_eModeType == MT_CATCH ||
		m_eModeType == MT_CATCH_RUNNINGMAN ||
		m_eModeType == MT_KING ||
		m_eModeType == MT_TEAM_SURVIVAL ||
		m_eModeType == MT_TEAM_SURVIVAL_AI ||
		m_eModeType == MT_MONSTER_SURVIVAL ||
		m_eModeType == MT_FOOTBALL ||
		m_eModeType == MT_HEROMATCH ||
		m_eModeType == MT_DUNGEON_A || 
		m_eModeType == MT_DOUBLE_CROWN ||
		m_eModeType == MT_SUCCESSION ||
		m_eModeType == MT_FLAG_CAPTURE ||
		m_eModeType == MT_ARENA ||
		m_eModeType == MT_FARMING ||
		Help::IsMonsterDungeonMode(m_eModeType) ) 
	{
		return;
	}

	if( m_iPreBlueWinCount != m_iBlueWinCount && m_iPreBlueWinCount != 0 )
	{
		m_ScoreChangeState.ChangeState( SCT_WHITE );
	}

	m_iPreBlueWinCount = m_iBlueWinCount;
}

void TimeScoreWnd::UpdateTurn( DWORD dwCurTurn, DWORD dwDifficulty )
{
	m_dwMonsterSurvivalTurn = dwCurTurn;
	m_dwMonsterSurvivalDifficulty = dwDifficulty;
}

void TimeScoreWnd::UpdateTime( DWORD dwRemainTime, DWORD dwFullTime )
{
	m_iFullSecond = (int)(dwFullTime)/1000;
	m_iRemainSecond  = (int)(dwRemainTime)/1000;
}

void TimeScoreWnd::UpdateSuddenDeathTime( DWORD dwRemainTime, DWORD dwFullTime )
{
	m_iSuddnDeathFullTime = (int)(dwFullTime)/1000;
	m_iSuddnDeathRemainTime = (int)(dwRemainTime)/1000;
}

void TimeScoreWnd::SetBlindOffSet( int iYOffSet )
{
	m_iBlindOffSetY = iYOffSet;
}

void TimeScoreWnd::UpdateGauge()
{
	enum 
	{ 
		BACK_XOFFSET = 44,
		BACK_YOFFSET = 50,
	};

	if( m_pCircleGauge )
	{
		float fGaugeRate = 0.0f;
		if( m_iFullSecond > 0 )
		{
			fGaugeRate = (float)m_iRemainSecond / (float)m_iFullSecond;
		}

		m_pCircleGauge->UpdateGauge( GetDerivedPosX()-BACK_XOFFSET,
									 GetDerivedPosY()-BACK_YOFFSET,
									 fGaugeRate );
	}

	if( 0 < m_iRemainSecond )
		m_iSuddnDeathRemainTime = 0;

	if( m_pCircleSuddenDeathGauge )
	{
		float fGaugeRate = 0.0f;
		if( m_iSuddnDeathRemainTime > 0 )
		{
			fGaugeRate = (float)m_iSuddnDeathRemainTime / (float)m_iSuddnDeathFullTime;
		}

		m_pCircleSuddenDeathGauge->UpdateGauge( GetDerivedPosX()-BACK_XOFFSET,
			GetDerivedPosY()-BACK_YOFFSET,
			fGaugeRate );
	}
}

void TimeScoreWnd::InitAniState()
{
	m_ShowAniState.SetCurState( SAT_NONE );
	m_WarningAniState.SetCurState( WAT_NONE );
	m_ScoreChangeState.SetCurState( SCT_NONE );
}

void TimeScoreWnd::InitShowAniState()
{
	enum { DELAY_DELAY = 165, DELAY_UP = 198, DELAY_DOWN = 66, DELAY_UP_STOP = 66, };

	m_ShowAniState.SetCurState( SAT_NONE );

	FSMState *pState = new FSMState(SAT_NONE);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
		pState->Add( SAT_END,  SAT_END );
	}
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_START_ROUND);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY);
		pState->Add( SAT_NONE,  SAT_NONE );
	}
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_DELAY);
	if( pState )
		pState->Add( SAT_DELAY,  SAT_UP , DELAY_DELAY);
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_UP);
	if( pState )
		pState->Add( SAT_UP,  SAT_DOWN , DELAY_UP);
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_DOWN);
	if( pState )
		pState->Add( SAT_DOWN,  SAT_UP_STOP , DELAY_DOWN );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_UP_STOP);
	if( pState )
		pState->Add( SAT_UP_STOP,  SAT_STOP , DELAY_UP_STOP );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_STOP);
	if( pState )
		pState->Add( SAT_END,  SAT_END );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_END);
	if( pState )
	{
		pState->Add( SAT_NONE,  SAT_NONE );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
	}
	m_ShowAniState.Add( pState );
}

void TimeScoreWnd::UpdateAniState()
{
	if( UpdateShowAni() )
		return;

	UpdateDefaultPos();

	// 10초전 ani
	m_WarningAniState.UpdateTime();
	enum { WARNING_SECOND = 10,};
	if( COMPARE( m_iRemainSecond, 1, WARNING_SECOND+1) )
		m_WarningAniState.ChangeState( WAT_HIDING );

	// blind ani
	if( m_iBlindOffSetY != m_iPreBlindOffSetY )
	{
		int iCurYPos = m_iDefaultY + m_iBlindOffSetY;
		if( !COMPARE( iCurYPos, 0, ( Setting::Height() - GetHeight() ) + 1) )
			return;
		SetWndPos( m_iDefaultX, iCurYPos );
		m_iPreBlindOffSetY = m_iBlindOffSetY;	
	}

	if( m_eModeType == MT_SYMBOL ||
		m_eModeType == MT_UNDERWEAR ||
		m_eModeType == MT_CBT ||
		m_eModeType == MT_CATCH ||
		m_eModeType == MT_CATCH_RUNNINGMAN ||
		m_eModeType == MT_KING ||
		m_eModeType == MT_TEAM_SURVIVAL ||
		m_eModeType == MT_TEAM_SURVIVAL_AI ||
		m_eModeType == MT_MONSTER_SURVIVAL ||
		m_eModeType == MT_FOOTBALL ||
		m_eModeType == MT_HEROMATCH ||
		m_eModeType == MT_DUNGEON_A || 
		m_eModeType == MT_DOUBLE_CROWN ||
		m_eModeType == MT_SUCCESSION ||
		m_eModeType == MT_FLAG_CAPTURE ||
		m_eModeType == MT_ARENA ||
		m_eModeType == MT_FARMING ||
		Help::IsMonsterDungeonMode(m_eModeType) ) 
	{
		return;
	}

	m_ScoreChangeState.UpdateTime();
}

void TimeScoreWnd::InitWarningAniState()
{
	enum { DELAY_HIDING = 250, DELAY_DONT_RENDER = 250, };
	m_WarningAniState.SetCurState( WAT_NONE );

	FSMState *pState = new FSMState(WAT_NONE);
	if( pState )
		pState->Add( WAT_HIDING,  WAT_HIDING );
	m_WarningAniState.Add( pState );

	pState = new FSMState(WAT_HIDING);
	if( pState )
	{
		pState->Add( WAT_HIDING,  WAT_DONT_RENDER , DELAY_HIDING);
		pState->Add( WAT_NONE, WAT_NONE );
	}
	m_WarningAniState.Add( pState );	

	pState = new FSMState(WAT_DONT_RENDER);
	if( pState )
	{
		pState->Add( WAT_DONT_RENDER,  WAT_HIDING , DELAY_DONT_RENDER);
		pState->Add( WAT_NONE, WAT_NONE );
	}
	m_WarningAniState.Add( pState );	
}

void TimeScoreWnd::InitScoreChangeState()
{
	enum { DELAY_REDUCTION = 100,};

	FSMState *pState = new FSMState(SCT_NONE);
	if( pState )
		pState->Add( SCT_WHITE,  SCT_WHITE );
	m_ScoreChangeState.Add( pState );

	pState = new FSMState(SCT_WHITE);
	if( pState )
		pState->Add( SCT_REDUCTION,  SCT_REDUCTION );
	m_ScoreChangeState.Add( pState );	

	pState = new FSMState(SCT_REDUCTION);
	if( pState )
	{
		pState->Add( SCT_REDUCTION,  SCT_NONE, DELAY_REDUCTION);
		pState->Add( SCT_WHITE,  SCT_WHITE );
	}
	m_ScoreChangeState.Add( pState );	

	m_ScoreChangeState.SetCurState( SCT_NONE );
}

void TimeScoreWnd::ChangeShowAniState( ShowAniType eShowAniType )
{
	m_ShowAniState.ChangeState( (int) eShowAniType );
}

bool TimeScoreWnd::UpdateShowAni()
{
	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_NONE )
		m_ShowAniState.ChangeState( SAT_END );

	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_START_ROUND )
		m_ShowAniState.ChangeState( SAT_DELAY );

	if( g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_END )
		m_ShowAniState.ChangeState( SAT_NONE );

	m_ShowAniState.UpdateTime();

	if( m_ShowAniState.GetCurState() == SAT_UP )
	{
		enum { MINUS_Y_POS = 140 };
		int iStartYPos = MINUS_Y_POS + m_iDefaultY;
		int iCurYpos = m_iDefaultY - ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * iStartYPos );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_DOWN )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( sin( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_UP_STOP )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_STOP )
	{
		SetWndPos( m_iDefaultX, m_iDefaultY );
		m_ShowAniState.ChangeState( SAT_END );
		return true;
	}

	return false;
}

int TimeScoreWnd::GetCurShowAniState()
{
	return m_ShowAniState.GetCurState();
}

void TimeScoreWnd::UpdateDefaultPos()
{
	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ))
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			int iYPos = GetYPos() - m_iBlindOffSetY;
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = iYPos;
		}
	}
}

void TimeScoreWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	float fPosGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	m_iDefXPos = iTempDefXPos + fPosGap;
	m_iDefYPos = iTempDefYPos + fPosGap;

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY + m_iBlindOffSetY );
	}
}

void TimeScoreWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szAniSound = xElement.GetStringAttribute_e( "AniSound" );
	m_iTest = xElement.GetIntAttribute_e("TestNum");
}

void TimeScoreWnd::SetNumberTwinkle( bool bBlueScore )
{
	m_bBlueScoreUP = bBlueScore;
	m_dwTwinkleTime= FRAMEGETTIME();
	m_iNumberWhiteCount = 0;
}
