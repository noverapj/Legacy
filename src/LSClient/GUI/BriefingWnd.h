#pragma once

#include "../io3DEngine/ioStringConverter.h"

class ioPlayStage;
class ioUIRenderImage;
class BriefingWnd : public ioWnd
{
public:	
	enum
	{
		ID_MODE_RANK = 1,
		ID_RED_BATTLE_USER_1	= 101,
		ID_RED_BATTLE_USER_2,
		ID_RED_BATTLE_USER_3,
		ID_BLUE_BATTLE_USER_1,
		ID_BLUE_BATTLE_USER_2,
		ID_BLUE_BATTLE_USER_3,

	};

	enum
	{
		BRIEFING_START = 1,
		BRIEFING_RESULT= 2,
		BRIEFING_FINAL_RESULT = 3,
	};

	enum
	{
		RESULT_WIN = 1,
		RESULT_LOSE= 2,
		RESULT_DRAW= 3,
	};

protected:
	ioPlayStage* m_pPlayStage;

	ioUIRenderImage *m_pFinalRoundText;
	ioUIRenderImage *m_pRoundText;
	ioUIRenderImage *m_pRoundNum;
	ioUIRenderImage *m_pGameStart;
	ioUIRenderImage *m_pGameEnd;
	ioUIRenderImage *m_pGameWin;
	ioUIRenderImage *m_pGameDraw;
	ioUIRenderImage *m_pGameLose;
	ioUIRenderImage *m_pGameBlueWin;
	ioUIRenderImage *m_pGameRedWin;
	ioUIRenderImage *m_pGameBlueLose;
	ioUIRenderImage *m_pGameRedLose;	
	ioUIRenderImage *m_pIconWin;
	ioUIRenderImage *m_pIconDraw;
	ioUIRenderImage *m_pIconLose;
	ioUIRenderImage *m_pHelpBar;
	ioUIRenderImage *m_pModeTitle;
	ioUIRenderImage *m_pModeIcon;

	// Battle Mode
	ioUIRenderImage *m_pBattleModeRedTeamFrame;
	ioUIRenderImage *m_pBattleModeBlueTeamFrame;
	ioUIRenderImage *m_pBattleModeFrameCenter;
	ioUIRenderImage *m_pBattleModeVS;

	// PvE Mode는 다른 브리핑 규칙을 따른다.
	ioUIRenderImage *m_pPvETitleWin;
	ioUIRenderImage *m_pPvETitleLose;
	ioUIRenderImage *m_pPvETitleFloor;
	ioUIRenderImage *m_pPvETitleFloorNum;
	ioUIRenderImage *m_pPvEModeTitleSub;
	ioUIRenderImage *m_pPvEModeTitleLine;
	ioUIRenderImage *m_pTournamentResultBack;

	ioUIRenderImage *m_pShuffleResultEffect;
	ioUIRenderImage *m_pShuffleEnd;
	ioUIRenderImage *m_pShuffleStarBox;
	ioUIRenderImage *m_pShuffleStarNum;

	ioUIRenderImage *m_pHunterCoinResultEffect;
	ioUIRenderImage *m_pHunterCoinEnd;
	ioUIRenderImage *m_pHunterCoinBox;
	ioUIRenderImage *m_pHunterCoinNum;

	DWORD m_dwBriefingType;
	DWORD m_dwBriefingStartTime;
	DWORD m_dwBriefingTime;
	DWORD m_dwBriefingResultTime;
	DWORD m_dwBriefingTotalResultTime;

	int		 m_iCurRound;
	int		 m_iMaxRound;
	DWORD	 m_dwResultType;
	TeamType m_eOwnerTeam;
	ModeType m_eModeType;
	int      m_iAlphaRate;
	
	ioHashString m_BriefingText;
	ioHashString m_FinalResultMapEffectBlue;
	ioHashString m_FinalResultMapEffectRed;
	ioHashString m_FinalResultMapEffectNone;
	
	ioHashString m_CurrentSound;

	ioHashString m_StartSound;
	ioHashString m_RoundWinSound;
	ioHashString m_RoundLoseSound;
	ioHashString m_ResultWinSound;
	ioHashString m_ResultLoseSound;
	ioHashString m_ResultDrawSound;

	ioHashString m_StartSoundFootBall;
	ioHashString m_RoundWinSoundFootBall;
	ioHashString m_RoundLoseSoundFootBall;
	ioHashString m_ResultWinSoundFootBall;
	ioHashString m_ResultLoseSoundFootBall;
	ioHashString m_ResultDrawSoundFootBall;

	int m_iModeTitleX;
	int m_iModeTitleY;
	int m_iModeIconX;
	int m_iModeIconY;

	ioWnd *m_pModeRankWnd;

	//tournament data
	DWORD m_dwContributeTotalBluePoint;
	DWORD m_dwContributeTotalRedPoint;

	//shuffle mode
	int m_iStarCount;
	int m_iHunterCoinCount;

    void PlaySoundName( const ioHashString &szSoundName );
public:
	void LoadResource( ModeType eModeType, PlazaType ePlazaType );
	void SetPlayStage( ioPlayStage* pPlayStage );
	void SetStartBriefing( int iCurRound );
	void SetResultInfo( ioPlayStage *pPlayStage, DWORD dwBriefingType, DWORD dwResultType, TeamType eTeamType, int iCurRound );
	void SetTournamentContributePoint( DWORD dwBluePoint, DWORD dwRedPoint );
	void SetShuffleStarCount( int iStarCount );
	void SetHunterCoinCount( int iHunterCoinCount );

protected:	
	void RenderBriefingStart( int iXPos, int iYPos );
	void RenderBriefingResult( int iXPos, int iYPos );
	void RenderBriefingFinalResult( int iXPos, int iYPos );
	void RenderPvEBriefingStart( int iXPos, int iYPos );
	void RenderPvEBriefingFinalResult( int iXPos, int iYPos );
	void RenderModeBriefing( int iXPos, int iYPos );
	void RenderBriefingStartForBattleMode( int iXPos, int iYPos );

public:
	void SetStartBriefingForBattleMode();
	void HidePartyMemberSlotUI();			// 배틀모드에서만 유저슬롯 표시해주는 UI를 배틀모드가 아닐때는 Hide시키는 함수
	
public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	DWORD GetTotalResultTime(){ return m_dwBriefingTotalResultTime; }
	DWORD GetBriefingType() { return m_dwBriefingType; }
	ioHashString &GetFinalResultMapEffect( TeamType eTeamType );

public:
	BriefingWnd();
	virtual ~BriefingWnd();
};

