#pragma once

#include "../io3DEngine/ioMovingWnd.h"

#include "FSMStateMgr.h"

class io2DCircleGauge;
class ioPlayStage;
class ioUIRenderImage;
class ioUIRenderFrame;
class ioUI3DRender;
class ioCreateChar;
class ioBaseChar;
class ioINILoader;
struct CHARACTER;

class FightClub3DChar
{
public:
	enum
	{
		VIEW_WIDTH = 350,
		VIEW_HEIGHT= 210,
	};

private:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_pAvata;

protected:
	int m_iOffsetXPos;
	int m_iOffsetYPos;

public:
	void Initialize();
	void SetChar( ioBaseChar *pChar, bool bLeftChar );
	void FaceChange( const char *szTexPostfix, int iDuration );
	void SetHelmChange( ioBaseChar *pChar );

	void Update( float fTimePerSec );
	void Render( int iXPos, int iYPos );

public:
	FightClub3DChar();
	~FightClub3DChar();
};

class FightClubTimeGaugeWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_CHAMPION_BTN			= 1,
		ID_CHALLENGER_BTN		= 2,
		ID_ENABLE_CHALLENGER	= 3,	// 도전자 받기 상태 버튼 : 텍스트는 '도전자 안받기' 주의 필요
		ID_NO_CHALLENGER		= 4,	// 도전자 안받기 상태 버튼 : 텍스트는 '도전자 받기' 주의 필요
	};

public:
	enum
	{
		MAX_FIGHT_NPC_WIN_COUNT = 5,
	};

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

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};
	
protected:
	ioUIRenderImage *m_pTimeText;
	ioUIRenderImage *m_pTimeTextRed;
	ioUIRenderImage *m_pTimeNumText;
	ioUIRenderImage *m_pTimeNumTextRed;
	ioUIRenderImage *m_pTimeBack;
	ioUIRenderImage *m_pTimeBackRed;

	ioUIRenderImage *m_pPowerBackEffectL;
	ioUIRenderImage *m_pPowerBackGaugeL;
	ioUIRenderImage *m_pPowerGaugeL;
	ioUIRenderImage *m_pPowerGaugeEffectL;
	ioUIRenderImage *m_pPowerGaugeMaxL;
	ioUIRenderImage *m_pPowerBackEffectR;
	ioUIRenderImage *m_pPowerBackGaugeR;
	ioUIRenderImage *m_pPowerGaugeR;
	ioUIRenderImage *m_pPowerGaugeEffectR;
	ioUIRenderImage *m_pPowerGaugeMaxR;

	ioUIRenderImage *m_pChampionText;
	ioUIRenderImage *m_pLeftBackLine;
	ioUIRenderImage *m_pLeftBackLineLight;
	ioUIRenderImage *m_pLeftGaugeBack;
	ioUIRenderImage *m_pLeftGauge100;
	ioUIRenderImage *m_pLeftGaugeGlow;
	ioUIRenderImage *m_pLeftGaugeRed;
	ioUIRenderImage *m_pLeftBlueNameBar;
	ioUIRenderImage *m_pLeftRedNameBar;

	ioUIRenderImage *m_pChallengerText;
	ioUIRenderImage *m_pRightBackLine;
	ioUIRenderImage *m_pRightBackLineLight;
	ioUIRenderImage *m_pRightGaugeBack;
	ioUIRenderImage *m_pRightGauge100;
	ioUIRenderImage *m_pRightGaugeGlow;
	ioUIRenderImage *m_pRightGaugeRed;
	ioUIRenderImage *m_pRightBlueNameBar;
	ioUIRenderImage *m_pRightRedNameBar;

	ioUIRenderImage *m_pChampionWinBar;
	ioUIRenderImage *m_pChampionWinEff;
	ioUIRenderImage *m_pChampionWinNum;

	ioUIRenderImage *m_pSeqMeBar;
	ioUIRenderImage *m_pNextMeBattle;
	ioUIRenderImage *m_pOwnerReserveChar;
	ioUIRenderImage *m_pOwnerReserveSubChar;

	ioUIRenderImage *m_pCheerBack;
	ioUIRenderImage *m_pChampCheerPoint;
	ioUIRenderImage *m_pChampCheerNumber;
	ioUIRenderImage *m_pChampCheerText;
	ioUIRenderImage *m_pChallCheerPoint;
	ioUIRenderImage *m_pChallCheerNumber;
	ioUIRenderImage *m_pChallCheerText;
	ioUIRenderImage *m_pCheerTimeBack;

	ioUIRenderImage *m_pFightNpcClearBack;
	ioUIRenderImage *m_pFightNpcClearWin;
	
	io2DCircleGauge *m_pCircleGaugeNor;
	io2DCircleGauge *m_pCircleGaugeDan;

	ioUIRenderImage *m_pAcceptBack;
	ioUIRenderImage *m_pAcceptPoint;

	ioUIRenderImage *m_pFightNPCRoundText;
	ioUIRenderImage *m_pFightNPCRoundNum;

	typedef std::map< DWORD, ioUIRenderImage *> ComGradeMap;
	ComGradeMap m_ComGradeList;

	int m_iBlindOffSetY;
	int m_iPreBlindOffSetY;

	FSMStateMgr m_ShowAniState;
	
	int m_iDefaultX;
	int m_iDefaultY;
	
	static bool m_bPosToDefualt;

	ioPlayStage *m_pPlayStage;

protected:
	int		m_iRemainSecond;
	bool	m_bTimeAlert;
	DWORD   m_dwTimeAlertTime;
	float   m_fAlphaValue;

protected:
	struct CharSeqData
	{
		ioHashString m_kName;
		int          m_iSeq;
		int          m_iLevel;
		int          m_iCheer;

		CharSeqData()
		{
			m_iSeq = m_iLevel = m_iCheer = 0;
		}
	};
	typedef std::vector< CharSeqData > CharSeqDataVec;
	class CharSeqDataSort : public std::binary_function< const CharSeqData&, const CharSeqData&, bool >
	{
	public:
		bool operator()( const CharSeqData &lhs , const CharSeqData &rhs ) const
		{
			if( lhs.m_iSeq < rhs.m_iSeq )
			{
				return true;
			}	
			return false;
		}
	};
	CharSeqDataVec m_CharSeqData;

protected:
	ioHashString    m_szLeftUserName;
	float			m_fLeftCurrentHPRate;
	float           m_fLeftUserHPRate;
	int             m_iLeftUserGrade;
	bool            m_bLeftGaugeStart;
	DWORD           m_dwLfetGaugeTime;
	FightClub3DChar *m_pLeftCharIcon;

protected:
	ioHashString    m_szRightUserName;
	ioHashString    m_szRightUserViewName;

	float			m_fRightCurrentHPRate;
	float           m_fRightUserHPRate;
	int             m_iRightUserGrade;
	bool            m_bRightGaugeStart;
	DWORD           m_dwRightGaugeTime;
	FightClub3DChar *m_pRightCharIcon;

protected:
	int             m_iChampionWinCount;
	float           m_fChampionWinEffectRate;
	DWORD           m_dwChampionWinEffectTime;

protected:
	int             m_iOwnerReserveCharType;

protected:
	ioHashString    m_MouseOverName;
	bool            m_bMouseLBtnDown;

protected:          // 파워!!
	struct PowerGaugeData
	{
		float m_fLimitValue;
		int   m_iPointValue;
	};
	typedef std::vector< PowerGaugeData > PowerGaugeDataVec;
	PowerGaugeDataVec m_TimePowerData;
	PowerGaugeDataVec m_HealthPowerData;
	int               m_iMaxPowerValue;
	
	DWORD             m_dwLeftPowerGaugeTime;
	float             m_fLeftPowerRate;
	float             m_fLeftPowerCurrentRate;

	DWORD             m_dwRightPowerGaugeTime;
	float             m_fRightPowerRate;
	float             m_fRightPowerCurrentRate;

	DWORD             m_dwPowerGaugeTime;
	float             m_fPowerGaugeRate;
	bool			  m_bPowerGaugeRateReverse;

protected:           // 종료
	DWORD             m_dwWarningSecond;
	ioHashString      m_szWarningTickSnd;

protected:
	int               m_iCheerRemainTime;
	int               m_iCheerFullTime;
	int               m_iCheerLimitTime;
	DWORD             m_dwCheerSendTime;
	bool              m_bCheerPossible;

protected:
	DWORD			  m_dwCheerTabID;

// fight npc
protected:
	bool m_bUseFightNPC;
	int m_iCurFightNPCStage;

	enum AcceptPintState
	{
		APS_RIGHT,
		APS_LEFT,
	};

	int m_iAcceptX;

	AcceptPintState m_AcceptPointState;
	DWORD m_dwAcceptPointTime;
	DWORD m_dwAcceptPointCheckTime;

protected:
	void InitShowAniState();
	void UpdateAniState();
	bool UpdateShowAni();
	void UpdateDefaultPos();	

protected:
	bool IsCheerPossible();
	void CheckCheerPossible();
	int  GetCheerCount( int iCheerType );
	void CheckCheerButton( DWORD dwID );
	void SendCheerData();

protected:
	ioUIRenderImage *GetComGradeRes( int iGrade );
	
public:
	void SetBlindOffSet( int iYOffSet );
	void ChangeShowAniState( ShowAniType eShowAniType );
	int  GetCurShowAniState();

	void InitAniState();

	int GetMainDefaultXPos() { return m_iDefXPos; }
	int GetMainDefaultYPos() { return m_iDefYPos; }
	
	DWORD GetCheerTabID()	{ return m_dwCheerTabID; }

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

protected:
	void OnTimeProcess();
	void OnLeftHealthProcess( float fTimePerSec );
	void OnRightHealthProcess( float fTimePerSec );
	void OnLeftPowerProcess( float fTimePerSec );
	void OnRightPowerProcess( float fTimePerSec );
	void OnPowerGaugeProcess( float fTimePerSec );
	void OnCheerTimeProcess( float fTimePerSec );
	void OnMouseProcess();
	bool IsMouseCheck( RECT &rkRect, ioHashString &rkName, const ioHashString &rkPrevName, const bool &rkPrevBtnDown );

protected:
	void OnTimeRender( int iXPos, int iYPos );
	void OnChampionRender( int iXPos, int iYPos );
	void OnChallengerRender( int iXPos, int iYPos );
	void OnSeqUserListRenter( int iXPos, int iYPos );
	void OnPowerGaugeLeftRender( int iXPos, int iYPos );
	void OnPowerGaugeRightRender( int iXPos, int iYPos );
	void OnCheerRender( int iXPos, int iYPos );
	void OnCheerTimeRender( int iXPos, int iYPos );

public:   // 룸생성 후 들어오는 유저들에 대한 순서
	void ClearCharSeqInfo();	
	void InsertCharSeqInfo( const ioHashString &rkName, int iState, int iSeq, int iVictories, int iCheer, int iLevel );
	void RemoveCharSeqInfo( const ioHashString &rkName );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetChampionUser( ioBaseChar *pChar );
	void SetChallengerUser( ioBaseChar *pChar );
	void SetFaceChange( const ioHashString &rkName, const char *szTexPostfix, int iDuration );
	void SetCharChange( ioBaseChar *pChar );
	void SetHelmChange( ioBaseChar *pChar );
	void SetOwnerReserveChar( int iCharType );
	void UpdateTime( int iRemainTime, int iMaxTime );

public:	
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	static bool GetPosToDefualt() { return m_bPosToDefualt; }
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefualt = PosToDefualt; }

public:
	const ioHashString &GetChampionUserName(){ return m_szLeftUserName; }
	const ioHashString &GetChallengerUserName(){ return m_szRightUserName; }

public:
	int GetMeSeqCount();
	int GetMeCheerType();

// fight npc
public:
	void SetUseFightNPC( bool bUse, int iRoundReadyCharCount );
	void InsertNPCSeqInfo( const ioHashString &rkName, int iState, int iStage );

protected:
	void OnProcessFightNPC();

	void OnChallengerNPCRender( int iXPos, int iYPos );
	void OnFightNPCRender( int iXPos, int iYPos );
	
public:
	FightClubTimeGaugeWnd();
	virtual ~FightClubTimeGaugeWnd();
};
//////////////////////////////////////////////////////////////////////////
class FightClubReadyWnd : public ioWnd
{
protected:
	// ANI 시간
	enum
	{
		BR_BACK_OPEN_TIME    = 400,
		BR_SLIDE_TIME        = 400,
		BR_EFFECT_SLIDE_TIME = 400,
		BR_DELAY_TIME        = 1000,
		BR_FADEOUT_TIME      = 600,

		GA_START_TIME			= 500,
		GA_EFFECT_FADEOUT_TIME  = 300,
		GA_HALF_DELAY_TIME		= 300,
		GA_FADEOUT_TIME			= 300,
	};

	// Back - > Ready Ani
	enum
	{
		BR_BACK_OPEN = 0,
		BR_SLIDE,
		BR_EFFECT_SLIDE,
		BR_DELAY,
		BR_FADEOUT,
		BR_HIDE,
	};

	// Go Ani
	enum
	{
		GA_DELAY = 0,
		GA_START,
		GA_EFFECT_FADEOUT,
		GA_HALF_DELAY,
		GA_FADEOUT,
		GA_END,
	};

protected:
	ioPlayStage *m_pPlayStage;

	ioUIRenderFrame *m_pBackFrm;
	ioUIRenderImage *m_pBackFrmLine;
	ioUIRenderImage *m_pReadyText;
	ioUIRenderImage *m_pReadyTextEffect;

	ioUIRenderImage *m_pBlackBack;
	ioUIRenderImage *m_pGoBackEffect;
	ioUIRenderImage *m_pGoText;
	ioUIRenderImage *m_pGoTextEffect;

protected:     // 바탕 & READY
	DWORD      m_dwBackReadyState;
	float      m_fBackReadyRate;
	DWORD      m_dwBackReadyTime;
	ioHashString m_szReadySnd;

protected:     // Go
	DWORD      m_dwGoState;
	float      m_fGoAniRate;
	DWORD      m_dwGoAniTime;
	ioHashString m_szGoSnd;

protected:
	bool m_bUseFightNPC;
	int m_iMaxStage;
	int m_iCurStage;

	DWORD m_dwFightAniTime;
	float m_fFightAlphaRate;
	float m_fFightScaleRate;

	ioUIRenderImage *m_pRoundText;
	ioUIRenderImage *m_pRoundNum;
	ioUIRenderImage *m_pFinalText;

	ioUIRenderImage *m_pFightText;

	ioHashStringVec m_vRoundSnd;
	ioHashString m_szFinalRoundSnd;
	ioHashString m_szFightSnd;

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void InitValue();

public:	
	virtual void iwm_show();

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void ShowReadyWnd( bool bUseFightNPC, int iMaxStage, int iCurStage );

protected:
	virtual void OnProcess( float fTimePerSec );
	void OnProcessBackReady();
	void OnProcessGo();

	void OnProcessRound();
	void OnProcessFight();

protected:
	virtual void OnRender();
	void OnRenderBackFrm( int iXPos, int iYPos );
	void OnRenderReady( int iXPos, int iYPos );
	void OnRenderGO( int iXPos, int iYPos );

	void OnRenderRound( int iXPos, int iYPos );
	void OnRenderFight( int iXPos, int iYPos );

public:
	FightClubReadyWnd();
	virtual ~FightClubReadyWnd();
};
//////////////////////////////////////////////////////////////////////////
class FightClubRoundEndWnd : public ioWnd
{
protected:
	enum
	{
		BONUS_SOLDIER			= 1,
		BONUS_GUILD				= 2,
		BONUS_PCROOM			= 3,
		BONUS_MODE				= 4,
		BONUS_MANNER			= 5,
		BONUS_FRIEND			= 6,
		BONUS_EVENT				= 7,
		BONUS_PESO_EVENT		= 8,
		BONUS_ETC_ITEM			= 9,
		BONUS_CAMP_BATTLE		= 10,
		BONUS_AWARD				= 11,
		BONUS_ETC_ITEM_EXP_PESO	= 12,
		BONUS_VICTORIES_PESO	= 13,
		BONUS_EVENT_PESO_EXP	= 14,
		BONUS_LEVELUP           = 15,
		BONUS_CONTRIBUTE        = 16,
		BONUS_HERO_TITLE_PESO   = 17,
		BONUS_MODE_CONSECUTIVELY= 18,
	};

	enum
	{
		MAX_BONUS = 9,
	};

public:
	enum
	{
		ROUNDEND_KO = 0,
		ROUNDEND_TIMEOUT,
		ROUNDEND_PERFECT,
		ROUNDEND_LEAVEROOM,
	};

	enum
	{
		ROUNDEND_START_TIME = 300,
		ROUNDEND_EFFECT_TIME= 400,
		ROUNDEND_DELAY_TIME = 1000,
		ROUNDEND_FADEOUT_TIME = 500,
		ROUNDEND_LEAVEROOM_START_TIME= 400,
		REWARD_ROLLING_TIME = 1000,
	};

	enum
	{
		ROUNDEND_START = 0,
		ROUNDEND_EFFECT,
		ROUNDEND_DELAY,
		ROUNDEND_LEAVEROOM_START,         // 유저 이탈시 처리 - 정상 상황에서는 발생하지 않음
		ROUNDEND_FADEOUT,
		ROUNDEND_END,
	};

	enum
	{
		REWARD_NONE = 0,
		REWARD_ROLLING,
	};

protected:
	ioUIRenderFrame *m_pBackFrm;
	ioUIRenderImage *m_pBackFrmLine;
	ioUIRenderImage *m_pRoundEndKO;
	ioUIRenderImage *m_pRoundEndTimeOut;
	ioUIRenderImage *m_pRoundEndPerfect;
	ioUIRenderImage *m_pRoundEndKOEffect;
	ioUIRenderImage *m_pRoundEndPerfectEffect;

	ioUIRenderImage *m_pRewardBack;
	ioUIRenderImage *m_pPesoIcon;
	ioUIRenderImage *m_pPesoText;
	ioUIRenderImage *m_pPesoNumber;
	ioUIRenderImage *m_pGradeIcon;
	ioUIRenderImage *m_pExpText;
	ioUIRenderImage *m_pExpNumber;

	ioUIRenderImage *m_pWinText;
	ioUIRenderImage *m_pVictoryWinText;
	ioUIRenderImage *m_pVictoryWinNum;
	ioUIRenderImage *m_pLoseText;
	
	ioUIRenderImage *m_pChampionText;
	ioUIRenderImage *m_pChampVictoryText;
	ioUIRenderImage *m_pChampWinText;
	ioUIRenderImage *m_pChampWinNum;
	ioUIRenderImage *m_pChallengerWinText;

	ioUIRenderImage *m_pNPCFightWin;
	ioUIRenderImage *m_pNPCFightFinalWin;

//Tournament
protected:
	ioUIRenderImage* m_pBlueRoundNum;			//숫자
	ioUIRenderImage* m_pBlueRoundText;			//강
	ioUIRenderImage* m_pBlueFinalRoundText;		//결승
	ioUIRenderImage* m_pBlueNextGoText;			//진출
	ioUIRenderImage* m_pBlueNextFailText;		//탈락
	ioUIRenderImage* m_pBlueChampionText;		//최종우승
	ioUIRenderImage* m_pBlueHalfChampionText;	//준우승

protected:         // KO
	DWORD m_dwRoundEndType;
	DWORD m_dwRoundEndState;
	DWORD m_dwRoundEndTime;
	float m_fRoundEndRate;
	bool  m_bRoundEndSync;
	ioHashString m_szRoundEndLeaveUser;

protected:         // Reward
	DWORD m_dwRewardState;
	float m_fRewardRate;
	DWORD m_dwRewardTime;
	int   m_iRewardExp;
	int   m_iRewardPeso;
	ioHashString m_szWinerName;
	ioHashString m_szLoserName;
	DWORD m_dwWinnerUserIndex;
	DWORD m_dwLoseUserIndex;
	int   m_iWinerVictory;
	bool  m_bChampWin;
	int   m_iMeSeqCount;

protected:
	struct BonusData
	{
		DWORD		 m_dwBonusID;       
		int			 m_iBonusExp;
		int			 m_iBonusPeso;
		ioHashString m_szTitle;
		BonusData()
		{
			m_dwBonusID   = 0;
			m_iBonusExp   = 0;
			m_iBonusPeso  = 0;
		}
	};
	typedef std::vector< BonusData > vBonusData;
	vBonusData m_BonusData;
	IntVec     m_PlayClassType;

public:			// sound
	ioHashString m_szKoSnd;
	ioHashString m_szWinSnd;
	ioHashString m_szLoseSnd;
	ioHashString m_szPerfectSnd;
	ioHashString m_szRollingSnd;
	ioHashString m_szTimeOutSnd;

protected:
	bool m_bUseFightNPC;
	int m_iCurNPCStage;
	int m_iMaxNPCStage;

protected:
	int m_iTournamentCurRound;
	bool m_bTournamentModeType;

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void InitValue();

protected:
	ioUIRenderImage *GetGradeLevel( int iGradeLevel );
	int GetNumberCountRand( int iNumber );

public:	
	virtual void iwm_show();

protected:
	virtual void OnProcess( float fTimePerSec );
	void OnProcessRoundEnd();
	void OnProcessReward();

protected:
	virtual void OnRender();
	void OnRenderRoundEnd( int iXPos, int iYPos );
	void OnRenderReward( int iXPos, int iYPos );
	void OnRenderCustomTournamentReward( int iXPos, int iYPos );
	void OnRenderWinLose( int iXPos, int iYPos );
	void OnRenderCheerResult( int iXPos, int iYPos );
	void OnRenderBonus( int iXPos, int iYPos );
	void OnRenderBonusText( const BonusData &rkData, int iXPos, int iYPos );

	void OnRenderFightNPCWin( int iXPos, int iYPos );
	void OnRenderCustomTournamentWinLose( int iXPos, int iYPos );
	void OnRenderCustomTournamentChampion( int iXPos, int iYPos );
	void OnRenderCustomTournamentFinalRound( int iXPos, int iYPos );
	void OnRenderCustomTournamentRound( int iXPos, int iYPos );

public:
	void SetRoundEnd( int iEndType, ioHashString &rkLeaveName );	
	void SetRoundResult( DWORD dwResultTime, ioHashString &rkChamp, ioHashString &rkChall, int iChampTotalExp, int iChampTotalPeso, int iChampVictory, bool bChampWin, float fChampBonusArray[ioPlayMode::BA_MAX], int iMeSeqCount );
	void SetTournamentResult();
	void FindTournamentWinUser( ioHashString szWinerName, DWORD& dwWinUserIdx, DWORD& dwLoseUserIdx );
	void SetFightNPCInfo( bool bUseFightNPC, int iMaxStage, int iCurStage );

	void SetBonus( float fBonusArray[ioPlayMode::BA_MAX], bool bChamp );
	void SetPlayClassType( int iClassType );

public:
	FightClubRoundEndWnd();
	virtual ~FightClubRoundEndWnd();
};
//////////////////////////////////////////////////////////////////////////
class FightClubChallengerWnd : public ioWnd
{
public:
	enum
	{
		CHALLENGER_START_TIME = 500,
		CHALLENGER_DELAY_TIME = 3000,
		CHALLENGER_FADEOUT_TIME = 500,
	};

	enum
	{
		CHALLENGER_START = 0,
		CHALLENGER_EFFECT,
		CHALLENGER_DELAY,
		CHALLENGER_FADEOUT,
		CHALLENGER_END,
	};

protected:
	ioUIRenderFrame *m_pBackFrm;
	ioUIRenderImage *m_pBackFrmLine;
	ioUIRenderImage *m_pNewChallengerText;

protected:
	DWORD m_dwChallengerState;
	DWORD m_dwChallengerTime;
	
	float m_fChallengerRate;
	bool m_bChallengerSync;

public:	// sound
	ioHashString m_szChallengerSnd;

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void InitValue();

public:	
	virtual void iwm_show();

protected:
	virtual void OnProcess( float fTimePerSec );
	void OnProcessChallenger();

protected:
	virtual void OnRender();

	void OnRenderChallenger( int iXPos, int iYPos );

public:
	void SetChallenger();

public:
	FightClubChallengerWnd();
	virtual ~FightClubChallengerWnd();
};
//////////////////////////////////////////////////////////////////////////
class FightClubFightNPCContinueWnd : public ioWnd
{
public:
	enum
	{
		ID_RETRY_BTN	= 1,
		ID_ROOM_BTN		= 2,
	};

protected:
	ioUIRenderImage *m_pContinueText;
	ioUIRenderImage *m_pContinueNum;

	ioHashString m_ChampName;

protected:
	int m_iStartCnt;
	int m_iCurCnt;
	int m_iCurStage;

	bool m_bRequestServer;
	DWORD m_dwContinueStartTime;

	ioHashString m_ContinueBackSnd;
	ioHashString m_ContinueCntSnd;
	ioHashString m_ContinueCntEndSnd;

public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void InitValue();

protected:
	virtual void OnProcess( float fTimePerSec );
	void OnProcessContinue();

protected:
	virtual void OnRender();

	void OnRenderContinue( int iXPos, int iYPos );

public:
	void SetContinue( int iStage, const ioHashString &szChampName );

public:
	FightClubFightNPCContinueWnd();
	virtual ~FightClubFightNPCContinueWnd();
};

