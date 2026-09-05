#pragma once

#include "../io3DEngine/ioMovingWnd.h"
#include "FSMStateMgr.h"

class io2DCircleGauge;
class ioUIRenderImage;

class TimeScoreWnd : public ioMovingWnd
{
public:
	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  // 가속
		SAT_DOWN         = 4,  // 감속
		SAT_UP_STOP      = 5,  // 가속
		SAT_STOP         = 6,
		SAT_END          = 7,
	};

	enum WarningAniType
	{
		WAT_NONE        = 0,
		WAT_HIDING      = 1,
		WAT_DONT_RENDER = 2,
		WAT_RETIMER		= 4,
	};
	
	enum ScoreChangeType
	{
		SCT_NONE      = 0,
		SCT_WHITE     = 1,
		SCT_REDUCTION = 2,
	};
	
	enum MSDifficulty
	{
		MSD_EASY   = 1,
		MSD_NORMAL = 2,
		MSD_HARD   = 3,
		MSD_LEGEND = 4,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

protected:
	io2DCircleGauge *m_pCircleGauge;
	io2DCircleGauge *m_pCircleSuddenDeathGauge;

	ioUIRenderImage *m_pTimeIcon;
	ioUIRenderImage *m_pTimeNumber;

	ioUIRenderImage *m_pBack;
	ioUIRenderImage *m_pMold;
	ioUIRenderImage *m_pWarningBack;

	ioUIRenderImage *m_pTitleCbt;
	ioUIRenderImage *m_pTitleUnderwear;
	ioUIRenderImage *m_pTitleTeamDeath;
	ioUIRenderImage *m_pTitleCatch;
	ioUIRenderImage *m_pTitleStone;
	ioUIRenderImage *m_pTitleKing;
	ioUIRenderImage *m_pTitleSoccer;
	ioUIRenderImage *m_pTitleHero;
	ioUIRenderImage *m_pTitleDeath;
	ioUIRenderImage *m_pTitleBoss;
	ioUIRenderImage *m_pTitleGangsi;	
	ioUIRenderImage *m_pTitleRunningMan;
	ioUIRenderImage *m_pTitleFalgCapture;
	ioUIRenderImage *m_pTitleArena;
	ioUIRenderImage *m_pTitleFarming;
	ioUIRenderImage *m_pTeamNumberBlue;
	ioUIRenderImage *m_pTeamNumberRed;
	ioUIRenderImage *m_pTeamColon;
	ioUIRenderImage *m_pSingleRank;
	ioUIRenderImage *m_pSingleSurvivor;
	
	ioUIRenderImage *m_pPvETitleBG;
	ioUIRenderImage *m_pPvETitleFloor;
	ioUIRenderImage *m_pPvETitleFloorNum;
	ioUIRenderImage *m_pPvEMonsterCoin;
	ioUIRenderImage *m_pPvEMonsterCoinX;
	ioUIRenderImage *m_pPvEMonsterCoinNum;

	ioUIRenderImage *m_pTitleDoubleCrown;
	ioUIRenderImage *m_pDoubleCrownSimilar;
	ioUIRenderImage *m_pDoubleBlueCloseWin;
	ioUIRenderImage *m_DoubleRedCloseWin;

	ioUIRenderImage *m_pShuffleBonus;
	ioUIRenderImage *m_pStarIcon;
	ioUIRenderImage *m_pStarCross;
	ioUIRenderImage *m_pStarNumber;

	ioUIRenderImage *m_pTitleSuccession;
	ioUIRenderImage *m_pSuccessionText;
	ioUIRenderImage *m_pSuccessionNumber;
	int				m_iTest;

	bool  m_bBlueScoreUP;
	DWORD m_dwTwinkleTime;
	int   m_iNumberWhiteCount;

	// 공통
	TeamType m_eMyTeam;
	ModeType m_eModeType;
	int m_iBlueWinCount;
	int m_iRedWinCount;
	TeamType m_eWinningTeam;

	int m_iFullSecond;
	int m_iRemainSecond;

	int m_iBlindOffSetY;
	int m_iPreBlindOffSetY;

	FSMStateMgr m_ShowAniState;
	FSMStateMgr m_WarningAniState;
	FSMStateMgr m_ScoreChangeState;
	FSMStateMgr m_SuddenDeathAniState;

	int m_iDefaultX;
	int m_iDefaultY;

	int m_iPreBlueWinCount;
	ioHashString m_szAniSound;

	DWORD m_dwMonsterSurvivalTurn;
	DWORD m_dwMonsterSurvivalDifficulty;

	static bool m_bPosToDefualt;

	//Sudden Death
	bool m_bSuddenDeadth;

	int m_iSuddnDeathFullTime;
	int m_iSuddnDeathRemainTime;

	// Shuffle Star
	int m_iShuffleBonusStarCount;

protected:
	void InitShowAniState();
	void InitWarningAniState();
	void InitScoreChangeState();
	void UpdateGauge();
	void UpdateAniState();
	bool UpdateShowAni();
	void UpdateDefaultPos();
	void UpdateNumberTwinkle();

	void RenderTimeGauge( int iXPos, int iYPos );
	void RenderTeamScore( int iXPos, int iYPos );
	void RenderTeamDeathScore( int iXPos, int iYPos );
	void RenderSingleScore( int iXPos, int iYPos );
	void RenderBossScore( int iXPos, int iYPos );
	void RenderGangsiScore( int iXPos, int iYPos );
	void RenderMonsterSurvival( int iXPos, int iYPos );
	void RenderDobuleCrownWinning( int iXPos, int iYPos );
	void RenderShuffleBonusStar( int iXPos, int iYPos );
	void RenderSuccessionNumber( int iXPos, int iYPos );
	void RenderFlagCaptureScore( int iXPos, int iYPos );

public:
	void UpdateScore( int iBlueWinCount , int iRedWinCount );
	void UpdateWinningTeam( TeamType eWinningTeam );

	void UpdateTurn( DWORD dwCurTurn, DWORD dwDifficulty );
	void SetModeType( ModeType eModeType ){ m_eModeType = eModeType; }
	void UpdateTime(  DWORD dwRemainTime, DWORD dwFullTime );
	void UpdateSuddenDeathTime(  DWORD dwRemainTime, DWORD dwFullTime );
	void SetBlindOffSet( int iYOffSet );
	void ChangeShowAniState( ShowAniType eShowAniType );
	int  GetCurShowAniState();

	void InitAniState();

	int GetMainDefaultXPos() { return m_iDefXPos; }
	int GetMainDefaultYPos() { return m_iDefYPos; }

public:
	void UpdateMyTeam( TeamType eTeam ){ m_eMyTeam = eTeam; }

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:	
	virtual void iwm_create();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	static bool GetPosToDefualt() { return m_bPosToDefualt; }
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefualt = PosToDefualt; }

public:
	void SetNumberTwinkle( bool bBlueScore );

public:
	TimeScoreWnd();
	virtual ~TimeScoreWnd();
};

