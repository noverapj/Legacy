#pragma once

#include "../io3DEngine/ioMovingWnd.h"

#include "FSMStateMgr.h"

class ioUIRenderImage;
class ioUIRenderFrame;
class ioXMLElement;

class CenterInfoWnd : public ioMovingWnd
{
protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

public:
	enum
	{
		MAX_EFFECT = 10,
		MAX_BOSSICON = 10,
	};

protected:  
	ioUIRenderImage *m_pBlueNum;
	ioUIRenderImage *m_pRedNum;
	ioUIRenderImage *m_pWhiteNum;
	ioUIRenderImage *m_pNumColon;
	ioUIRenderImage *m_pCenterLine;
	ioUIRenderImage *m_pDeathTimeText;
	ioUIRenderImage *m_pSuddenDeathTimeText;
	ioUIRenderImage *m_pBlueEffectSpr[MAX_EFFECT];
	ioUIRenderImage *m_pRedEffectSpr[MAX_EFFECT];

	ioUIRenderImage *m_pBlueIconBack;
	ioUIRenderImage *m_pBlueGaugeBack;
	ioUIRenderImage *m_pBlueGauge;

	ioUIRenderImage *m_pRedIconBack;
	ioUIRenderImage *m_pRedGaugeBack;
	ioUIRenderImage *m_pRedGauge;

	ioUIRenderImage *m_pWhiteGauge;
	ioUIRenderImage *m_pTextRank;
	ioUIRenderImage *m_pTextSurvivor;	
	ioUIRenderImage *m_pTextSec;

	// Hidden Crown
	ioUIRenderImage *m_pBlueCrown;	
	ioUIRenderImage *m_pBlueCrownText;
	ioUIRenderImage *m_pRedCrown;	
	ioUIRenderImage *m_pRedCrownText;	

	// Boss Raid
	ioUIRenderImage *m_pBossIcon[MAX_BOSSICON];
	ioUIRenderImage *m_pBossText;

	// Symbol
	ioUIRenderImage *m_pBlueStoneIcon;
	ioUIRenderImage *m_pRedStoneIcon;
	ioUIRenderImage *m_pKoText;
	ioUIRenderImage *m_pStoneGaugeBack;
	ioUIRenderImage *m_pStoneBlueGauge1;
	ioUIRenderImage *m_pStoneBlueGauge2;
	ioUIRenderImage *m_pStoneRedGauge1;
	ioUIRenderImage *m_pStoneRedGauge2;
	ioUIRenderImage *m_pStoneBlueWhiteGauge;
	ioUIRenderImage *m_pStoneRedWhiteGauge;
	ioUIRenderImage *m_pStoneAttack1;
	ioUIRenderImage *m_pStoneAttack2;
	ioUIRenderImage *m_pStoneBlueAttack;
	ioUIRenderImage *m_pStoneRedAttack;

	// Dice
	ioUIRenderImage *m_pDiceRewardFailIcon;
	ioUIRenderImage *m_pDiceRewardIconBack;
	ioUIRenderImage *m_pDicePrizeIcon;
	ioUIRenderImage *m_pDiceSubIcon;

	//Double
	ioUIRenderImage *m_pDoubleCrownSimilar;
	ioUIRenderImage *m_pDoubleBlueCrown;
	ioUIRenderImage *m_pDoubleBlueCloseWin;

	ioUIRenderImage *m_pDoubleRedCrown;
	ioUIRenderImage *m_pDoubleRedCloseWin;

	ioUIRenderImage *m_pDoubleBlueWhiteGauge;
	ioUIRenderImage *m_pDoubleRedWhiteGauge;

	// Flag Capture
	ioUIRenderImage *m_pFlagCaptureNewFlag;
	ioUIRenderImage *m_pFlagCaptureBlueReturn;
	ioUIRenderImage *m_pFlagCaptureRedReturn;


protected:     // Symbol
	enum SymbolGaugeAni
	{
		SGA_NONE = 0,
		SGA_START,
		SGA_DROP,     // ~ 200
		SGA_SUP1,     // ~ 270
		SGA_SUP2,     // ~ 340
		SGA_LOOP,     // ~ Infinite
	};
	FSMStateMgr m_SymbolGaugeAni;
	
	float m_fBlueSymbolRate;
	float m_fPrevBlueSymbolRate;
	float m_fBlueGaugeAlpha;
	float m_fBlueGaugeDownTime;
	bool  m_bBlueSymbolAttack;
	float m_fBlueSymbolAttackTime;
	int   m_iBlueSymbolVibration;
	int   m_iBlueSymbolAttackX;
	int   m_iBlueSymbolAttackY;

	float m_fRedSymbolRate;
	float m_fPrevRedSymbolRate;
	float m_fRedGaugeAlpha;
	float m_fRedGaugeDownTime;
	bool  m_bRedSymbolAttack;
	float m_fRedSymbolAttackTime;
	int   m_iRedSymbolVibration;
	int   m_iRedSymbolAttackX;
	int   m_iRedSymbolAttackY;

protected:  // Boss Raid
	enum BossRaidAni
	{
		BRA_NONE = 0,
		BRA_START,
		BRA_DROP,     // ~ 200
		BRA_SUP1,     // ~ 270
		BRA_SUP2,     // ~ 340
		BRA_LOOP,     // ~ Infinite
	};
	FSMStateMgr m_BossRaidAni;

	int   m_iBossRaidOrder;
	int   m_iBossRaidMaxOrder;
	int   m_iBossRaidMyLevel;
	DWORD m_dwBossRaidTime;
	ioHashString m_BossRaidSound;
	bool  m_bBossRaidSnd;
	
protected:  // Hidden Crown
	enum HiddenCrownAni
	{
		HCA_NONE = 0,
		HCA_START,
		HCA_DROP,    // 0 ~ 200
		HCA_SUP1,    // ~ 270
		HCA_SUP2,    // ~ 340
		HCA_LOOP,    // ~ Infinite
	};
	FSMStateMgr m_HiddenCrownAni;

	TeamType m_eKingTeam;
	TeamType m_eOwnerTeam;
	DWORD    m_dwGivePointTime;
	DWORD    m_dwCrownHoldTime;
	float    m_fScoreWarningRate;

protected:	// Team DeathMatch	
	enum TeamDeathAni
	{
		TDA_NONE = 0,
		TDA_START,
		TDA_ROLLING,		// 0 ~ 500
		TDA_TWINKLE,		// ~ 2600 : 175ms씩 알파
		TDA_MOVE_SCORE_UI,	// ~ 2950
		TDA_WARNING_SCORE,  // 긴박 상태가되면 TDA_MOVE_SCORE_UI 대신 - > TDA_WARNING_SCORE로 상태 전환
	};
	FSMStateMgr m_TeamDeathAni;

	TeamType m_eKillTeam;
	ioHashString m_szKiller;
	TeamType m_eDieTeam;
	ioHashString m_szDier;
	int  m_iCurrentBluePoint;
	int  m_iCurrentRedPoint;
	int  m_iAddBluePoint;
	int  m_iAddRedPoint;
	bool m_bBlueScoreUP;
	bool m_bTeamDeathWarningScore;
	TeamDeathAni m_ePrevTeamDeathAni;

protected:	// DeathMatch	
	enum DeathMatchAni
	{
		DMA_NONE = 0,
		DMA_START,
		DMA_ROLLING,		// 0 ~ 500
		DMA_TWINKLE,		// ~ 2600 : 175ms씩 알파
		DMA_MOVE_SCORE_UI,	// ~ 2950
		DMA_WARNING_TIME,   // 긴박 상태가되면 DMA_MOVE_SCORE_UI 대신 - > DMA_WARNING_TIME로 상태 전환
	};
	FSMStateMgr m_DeathMatchAni;

	int m_iPrevRank;
	int m_iCurRank;

	float m_fPrevRankRate;
	float m_fCurRankRate;
	bool  m_bDeathMatchTimeWarning;
	DeathMatchAni m_ePrevDeathMatchAni;

protected:   // Death Time
	enum DeathTimeAni
	{
		DTA_NONE = 0,
		DTA_START,
		DTA_FADEIN,		// 0 ~ 150
		DTA_VIBRATION,  // ~ 1640
		DTA_FADEOUT,    // ~ 1790
	};
	FSMStateMgr m_DeathTimeAni;

	struct tagDeathTimeData
	{
		int  m_iXPos;
		int  m_iYPos;
		bool m_bScreen;
		tagDeathTimeData()
		{
			m_iXPos = m_iYPos = 0;
			m_bScreen = false;
		}
	};
	typedef std::vector< tagDeathTimeData > vDeathTimeData;
	vDeathTimeData m_DeathTimeData;	
	bool           m_bDeathTimeScreen;
	
protected:	// Gangsi Mode
	enum GangsiModeAni
	{
		GMA_NONE = 0,
		GMA_START,
		GMA_ROLLING,		// 0 ~ 500
		GMA_TWINKLE,		// ~ 2600 : 175ms씩 알파
		GMA_MOVE_SCORE_UI,	// ~ 2950
	};
	FSMStateMgr  m_GangsiModeAni;
	ioHashString m_szGangsiName;
	ioHashString m_szSurvivorName;
	ioHashString m_GangsiLiveSound;

	enum GangsiAlarmAni
	{
		GAA_NONE = 0,
		GAA_START,
		GAA_FADEIN,		// 0 ~ 150
		GAA_VIBRATION,  // ~ 1640
		GAA_FADEOUT,    // ~ 1790
	};
	FSMStateMgr m_GangsiAlarmAni;

	struct tagGangsiAlarmData
	{
		int  m_iXPos;
		int  m_iYPos;
		bool m_bScreen;
		tagGangsiAlarmData()
		{
			m_iXPos = m_iYPos = 0;
			m_bScreen = false;
		}
	};
	typedef std::vector< tagGangsiAlarmData > vGangsiAlarmData;
	vGangsiAlarmData m_GangsiAlarmData;	
	bool             m_bGangsiAlarmText;
	bool             m_bGangsiAlarmScreen;
	DWORD            m_dwGangsiAlarmTickTime;
	DWORD            m_dwGangsiAlarmWarningTime;

protected:	// DungeonAMode
	enum DungeonAModeAni
	{
		MDA_NONE = 0,
		MDA_START,
		MDA_INFINITY,		// 서버에서 올때까지 롤링
		MDA_FADEOUT,        // 서버에서 온 뒤 1초간 깜박 깜박
	};
	FSMStateMgr m_DungeonAModeAni;

	struct MonsterDiceData
	{
		 ioHashString m_MonsterName;
		 int m_iPresentType;
		 int m_iPresentValue1;
		 int m_iPresentValue2;
			 
		 ioHashStringVec m_RankUserList;
		 
		 MonsterDiceData()
		 {
			 m_iPresentType = m_iPresentValue1 = m_iPresentValue2 = 0;
		 }
	};
	typedef std::vector< MonsterDiceData > vMonsterDiceData;
	vMonsterDiceData m_MonsterDiceList;
	ioHashString     m_szPrizeWinner;
	int              m_iPrizeRollingArray;
	bool             m_bPrizeScreen;
    
protected: // DoubleCrown
	enum DoubleCrownAni
	{
		DCA_NONE = 0,
		DCA_START,
		DCA_DROP,     // ~ 200
		DCA_SUP1,     // ~ 270
		DCA_SUP2,     // ~ 340
		DCA_LOOP,     // ~ Infinite
	};
	FSMStateMgr m_DoubleCrownAni;
	
	float m_fDoubleBlueRate;
	float m_fDoublePrevBlueRate;
	float m_fDoubleBlueGaugeAlpha;
	float m_fDoubleBlueGaugeDownTime;

	bool  m_bDoubleBlueAttack;
	float m_fDoubleBlueAttackTime;
	int   m_iDoubleBlueVibration;
	int   m_iDoubleBlueAttackX;
	int   m_iDoubleBlueAttackY;
	int   m_iBlueEffectArray;
	bool  m_bBlueKing;

	float m_fDoubleRedRate;
	float m_fDoublePrevRedRate;
	float m_fDoubleRedGaugeAlpha;
	float m_fDoubleRedGaugeDownTime;

	bool  m_bDoubleRedAttack;
	float m_fDoubleRedAttackTime;
	int   m_iDoubleRedVibration;
	int   m_iDoubleRedAttackX;
	int   m_iDoubleRedAttackY;
	int   m_iDoubleWinningTeam;	
	int   m_iRedEffectArray;
	bool  m_bRedKing;

protected:	// Flag Capture
	enum FlagCaptureAni
	{
		FCA_NONE = 0,
		FCA_START,
		FCA_ROLLING,		// 0 ~ 500
		FCA_TWINKLE,		// ~ 2600 : 175ms씩 알파
		FCA_MOVE_SCORE_UI,	// ~ 2950
		FCA_WARNING_SCORE,  // 긴박 상태가되면 FCA_MOVE_SCORE_UI 대신 - > FCA_WARNING_SCORE로 상태 전환
		FCA_REMAIN_TIME		// 깃발 생성 대기 시간은 FCA_MOVE_SCORE_UI 대신 FCA_REMAIN_TIME로 상태 전환
	};
	FSMStateMgr m_FlagCaptureAni;

	enum FlagReturn
	{
		FR_NONE = 0,
		FR_NEW_FLAG,
		FR_BLUE_RETURN,
		FR_RED_RETURN
	};

	TeamType m_eFlagKillTeam;
	ioHashString m_szFlagKiller;
	TeamType m_eFlagDieTeam;
	ioHashString m_szFlagDier;
	TeamType m_eFlagCapturerTeam;
	ioHashString m_szFlagCapturer;
	TeamType m_eFlagDroperTeam;
	ioHashString m_szFlagDroper;
	TeamType m_eFlagCarrierTeam;
	ioHashString m_szFlagCarrier;
	FlagReturn m_eFlagReturn;
	int  m_iFlagBluePoint;
	int  m_iFlagRedPoint;
	int  m_iFlagAddBluePoint;
	int  m_iFlagAddRedPoint;
	bool m_bFlagBlueScoreUP;
	bool m_bFlagCaptureWarningScore;
	bool m_bFlagNoPointChange;
	DWORD m_dwReturnAlramTime;
	DWORD m_dwReturnAlramDuration;
	DWORD m_dwNewFlagTime;
	DWORD m_dwNewFlagDuration;
	DWORD m_dwFlagWaitRemainTime;
	FlagCaptureAni m_ePrevFlagCaptureAni;

protected:  // Common
	float m_fTimeRate;
	float m_fAlphaRate;
	int   m_iWhiteColor;
	int   m_iAddXPos;
	int   m_iAddYPos;
	int   m_iEffectArray;
	int	  m_iDefaultX;
	int   m_iDefaultY;
	bool  m_bNoHideWnd;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	void InitHiddenCrownAni();
	void InitBossRaidAni();
	void InitTeamDeathAni();
	void InitDeathMatchAni();
	void InitDeathTimeAni();
	void InitSymbolGaugeAni();
	void InitGangsiModeAni();
	void InitGangsiAlarmAni();
	void InitDungeonAModeAni();
	void InitDoubleCrownAni();
	void InitFlagCaptureAni();

public:
	void UpdateDeathTime();
	void UpdateTeamDeathMode( TeamType eKill, const ioHashString &szKiller, TeamType eDie, const ioHashString &szDier, float fBlueRate, float fRedRate );
	void UpdateTeamDeathWarning( float fBlueRate, float fRedRate );
	void UpdateDeathMatchMode( int iCurrentRank );
	void UpdateDeathMatchWarning( int iCurrentRank );
	void UpdateKingMode( bool bKing, TeamType eKingTeam, TeamType eOwnerTeam, DWORD dwGivePointTime, DWORD dwCrownHoldTime, float fScoreWarningRate );	
	void UpdateBossMode( int iBossRaidOrder, int iBossRaidMaxOrder, int iBossRaidMyLevel );
	void UpdateSymbolGauge( float fBlueRate, float fRedRate );
	void UpdateSymbolAttack( TeamType eSymbolTeam );
	void UpdateGangsiLiveUser( int iLiveUser, const ioHashString &szGangsi, const ioHashString &szSurvivor );
	void UpdateGangsiAlarm( bool bGangsiAlarmText, DWORD dwCurrentTime, DWORD dwWarningTime );
	void UpdateDungeonAMode( const ioHashString &kMonsterName, int iPresentType, int iPresentValue1, int iPresentValue2, ioHashStringVec &rkRankUserList );
	void UpdateMonsterDiceResult( const ioHashString &kPrizeWinner, int iPresentType, int iPresentValue1, int iPresentValue2 );
	void UpdateDoubleCrownGauge( float fBlueRate, float fRedRate, bool bBlueKing, bool bRedKIng );
	void UpdateDoubleCrownAttack( TeamType eTeam );
	void UpdateDoubleCrownWinningTeam( TeamType eTeam );
	void UpdateFlagCaptureMode( TeamType eCapture, const ioHashString &szCapture, float fBlueRate, float fRedRate, DWORD dwFlagWaitTime );
	void UpdateFlagCaptureKO( TeamType eKill, const ioHashString &szKiller, TeamType eDie, const ioHashString &szDier, float fBlueRate, float fRedRate );
	void UpdateFlagCaptureAlram( TeamType eTeam, const ioHashString &szChar, bool bCapture );
	void UpdateFlagCaptureNewFlag();
	void UpdateFlagCaptureWarning( float fBlueRate, float fRedRate );

protected:
	void RenderHiddenCrown( int iXPos, int iYPos );
	void RenderBossRaid( int iXPos, int iYPos );
	void RenderTeamDeath( int iXPos, int iYPos );
	void RenderDeathMatch( int iXPos, int iYPos );
	void RenderSymbolGauge( int iXPos, int iYPos );
	void RenderDeathTime( int iXPos, int iYPos );
	void RenderSuddenDeathTime( int iXPos, int iYPos );
	void RenderGangsiMode( int iXPos, int iYPos );
	void RenderGangsiAlarm( int iXPos, int iYPos );
	void RenderDungeonAMode( int iXPos, int iYPos );
	void RenderDoubleCrownGauge( int iXPos, int iYPos );
	void RenderFlagCapture( int iXPos, int iYPos );

protected:
	void ProcessHiddenCrown();
	void ProcessBossRaid();
	void ProcessTeamDeath();
	void ProcessDeathMatch();
	void ProcessSymbolGauge();
	void ProcessDeathTime();
	void ProcessGangsiMode();
	void ProcessGangsiAlarm();
	void ProcessDungeonAMode();
	void ProcessDoubleCrownGauge();
	void ProcessDoubleCrownEffect();
	void ProcessFlagCapture();

protected:
	void BossRaidSound( bool bPlay );

public:
	bool IsHiddenKingWarning();

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	CenterInfoWnd();
	virtual ~CenterInfoWnd();
};

