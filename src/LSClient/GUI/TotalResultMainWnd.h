#pragma once

#include "../ioPlayMode.h"
#include "../CharacterInfo.h"

#include "FSMStateMgr.h"
#include "../io3DEngine/ioMovingWnd.h"
#include "ResultInfo.h"

class SP2Packet;
class ioUI3DRender;
class ioCreateChar;
class ioXMLElement;
class ioPlayStage;
class ioUIRenderImage;
class ioUIRenderFrame;
//////////////////////////////////////////////////////////////////////////
class AwardChatBubbleWnd : public ioWnd
{
protected:
	ioUIRenderImage *m_pTooltipTail;

protected:
	int		m_iDefaultX;
	int		m_iDefaultY;
	int		m_iDefaultW;
	int		m_iDefaultH;
	int		m_iAddHeight;
	int		m_iStartXPos;
	int		m_iStartYPos;
	int		m_iWidthCut;
	int		m_iMaxLine;
	DWORD	m_dwShowTime;
	DWORD   m_dwShowEventTime;

protected:
	struct ChatBubble
	{
		DWORD m_dwTextColor;
		ioHashString m_szText;
		ChatBubble()
		{
			m_dwTextColor = 0;
		}
	};
	typedef std::vector< ChatBubble > vChatBubble;
	vChatBubble  m_BubbleList;
	DWORD        m_dwStartTime;
	DWORD        m_dwShowAniTime;
	ioHashString m_szBubbleName;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual bool OnRenderShowAndHide();
	virtual void OnRender();

public:
	void StartChatBubble( const char *szBubbleName, const char *szChat, bool bWholeChat );

public:
	AwardChatBubbleWnd();
	virtual ~AwardChatBubbleWnd();
};
//////////////////////////////////////////////////////////////////////////
class AwardSlotNameBtn : public ioButton
{
protected:
	ioUIRenderFrame *m_pThickBlueFrm;
	ioUIRenderFrame *m_pLightBlueFrm;
	ioUIRenderFrame *m_pThickRedFrm;
	ioUIRenderFrame *m_pLightRedFrm;

	int m_iGradeLevel;
	int m_iClassType;
	TeamType m_eTeam;
	ioHashString m_szName;
	bool m_bOriginal;

public:
	void SetAwardName( int iClassType, int iLevel, TeamType eTeam, const ioHashString &szName, bool bOriginal = false );
	const ioHashString &GetName(){ return m_szName; }

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	AwardSlotNameBtn();
	virtual ~AwardSlotNameBtn();
};
//////////////////////////////////////////////////////////////////////////
class AwardSlotWnd : public ioWnd
{
public:
	enum
	{
		ID_NAME  = 1,
		ID_DUMMY = 2,

		ID_CHAT_BUBBLE = 100,
	};

	enum
	{
		AWARD_COMPLETE = 2000,
	};

protected:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_aMyChar;

protected:
	struct AwardInfo
	{
		int m_iType;
		int m_iLevel;
		int m_iTeam;
		ioHashString m_szName;
		int m_iValue;
		int m_iPoint;
		int m_iGoodBadType;
		float m_fAwardBonus;
		CHARACTER m_charInfo;
		int m_iClassLevel;
		ITEM_DATA m_EquipItem[MAX_INVENTORY];
		MedalSyncInfoVec m_vMedalItemTypeVec;
		DWORD     m_dwCharMotionItem;
		
		// 이벤트로 특별 아이템 받음
		int   m_iEtcItemType;
		int   m_iEtcItemCount;

		// 특별 수상으로 인해 받은 선물 정보
		bool  m_bGlobalAlarm;
		short m_iPresentType;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		int   m_iPresentValue3;
		int   m_iPresentValue4;
		AwardInfo()
		{
			m_iType	= 0;
			m_iLevel= 0;
			m_iTeam = 0;
			m_iValue= 0;
			m_iPoint= 0;
			m_iGoodBadType = 0;
			m_fAwardBonus = 0.0f;
			m_iClassLevel = 0;
			m_charInfo.Init();
			for( int i = 0; i < MAX_INVENTORY; i++ )
				m_EquipItem[i].Initialize();

			m_vMedalItemTypeVec.clear();
			m_dwCharMotionItem = 0;
			m_iEtcItemType = 0;
			m_iEtcItemCount= 0;
			m_bGlobalAlarm = false;
			m_iPresentType = 0;
			m_iPresentValue1 = 0;
			m_iPresentValue2 = 0;
			m_iPresentValue3 = 0;
			m_iPresentValue4 = 0;
		}
	};
	AwardInfo     m_AwardInfo;
	AwardInfo     m_CurAwardInfo;
	FinalInfoList m_FinalInfoList;

protected:
	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pCharBottom;
	ioUIRenderImage *m_pTitleBack;
	ioUIRenderImage *m_pEmptyAward;
	ioUIRenderImage *m_pEtcItemIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pEtcItemNum;
	ioUIRenderImage *m_pEtcItemPlus;

	int   m_iCurFrame;
	DWORD m_dwCurTimer;
	DWORD m_dwRandTime;
	DWORD m_dwCurTickTime;
	DWORD m_dwRandTickTime;
	DWORD m_dwOriginalTime;
	DWORD m_dwSlowTime;
	bool  m_bRandTimeEnd;

	float m_fOriginalRate;      //아이콘 + 타이틀

	bool m_bCharOver;
	bool m_bOriginal;

	ioHashString m_szCoundSound;
	ioHashString m_szAwakeEffect;

protected:
	static int sg_iMyAwardCount;

protected:
	void CreateCharacter( const CHARACTER &rkInfo, const ITEM_DATA* pItem, const int iAniCode, const ioHashString &szAniName, DWORD dwCharMotionItem, const ioHashString &szID );
	void SetOriginalAward();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetAward( SP2Packet &rkPacket );
	void AddFinalInfo( const FinalInfo &rkInfo );
	void PlayCountSound();
	bool IsOriginal();

	const ioHashString& GetCharName();
	int GetAwardType();
	int GetItemReinforce( int iSlot );
	int GetCharLevel();
	void GetMedalItemType( OUT MedalSyncInfoVec &rvMedalItemType );
	void SetAwardCharMotion( DWORD dwCharMotion );

protected:
	void OnProcessRand();
	void OnProcessOriginal();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void RenderNumInfo( int iXPos, int iYPos );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void StartChatBubble( const char *szBubbleName, const char *szChat, bool bWholeChat );

public:
	AwardSlotWnd();
	virtual ~AwardSlotWnd();
};
//////////////////////////////////////////////////////////////////////////
class TotalResultAwardWnd : public ioWnd
{
public:
	enum
	{
		ID_AWARD_1 = 1,
		ID_AWARD_2 = 2,
		ID_AWARD_3 = 3,
		ID_AWARD_4 = 4,
	};

	enum
	{
		AWARD_ROLLING_END = 1000,
		AWARD_TIME_END    = 1001,
	};

protected:
	ioPlayStage	  *m_pPlayStage;
	FinalInfoList m_FinalInfoList;

protected:
	ioHashString m_BackSound;
	DWORD        m_dwCurTime;
	DWORD        m_dwShowTime;
	int          m_iAwardCount;
	bool         m_bPresentAlarmBlock;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetAwarding( SP2Packet &rkPacket );
	void SetAwardingCharMotion( const ioHashString &rkCharName, int iAwardType, DWORD dwCharMotionItem );
	void AddFinalInfo( const FinalInfo &rkInfo );

public:
	int GetMyTeam();
	int GetAwardCharItemReinforce( const ioHashString &szName, int iSlot );
	int GetAwardCharLevel( const ioHashString &szName );
	void GetMedalItemType( IN const ioHashString &szName, OUT MedalSyncInfoVec &rvMedalItemTypeVec );

public:
	bool IsPresentAlarmBlock();

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	void StartChatBubble( const char *szBubbleName, const char *szChat, bool bWholeChat );

public:
	TotalResultAwardWnd();
	virtual ~TotalResultAwardWnd();
};
//////////////////////////////////////////////////////////////////////////
class TotalResultTreasureCardBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pCardCover;
	ioUIRenderImage *m_pPresentCover;
	ioUIRenderImage *m_pPresentCoverOverAdd;
	ioUIRenderImage *m_pPresentCoverMe;

protected:
	ioUIRenderImage *m_pPresentIcon;
	ioUIRenderImage *m_pSubIcon;

protected:
	ioHashString m_Name;

protected:
	short m_iPresentType;
	int   m_iPresentValue1;
	int   m_iPresentValue2;

protected:
	bool  m_bScreenStart;
	bool  m_bScreen;
	DWORD m_dwCurrentTime;;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderPresent( int iXPos, int iYPos );

public:
	void SetName( const ioHashString &rkName );
	void SetPresent( short iPresentType, int iPresentValue1, int iPresentValue2 );
	void SetScreenStart();

public:
	const ioHashString &GetName(){ return m_Name; }
	bool IsEmptyCard(){ return (m_iPresentType == 0); }

public:
	TotalResultTreasureCardBtn();
	virtual ~TotalResultTreasureCardBtn();
};
//
class TotalResultTreasureCardWnd : public ioWnd
{
public:
	enum
	{
		ID_CARD_1  = 1,        //  1 ~ 21 예약
		ID_CARD_21 = 21,  
	};

	enum
	{
		MAX_CARD_SLOT = 21,
	};

	enum
	{
		STATE_NONE		= 0,
		STATE_START		= 1000,
		STATE_CLICK		= 1001,
		STATE_DELAY     = 1002,
		STATE_AUTO_OPEN = 1003,
		STATE_CLEAR		= 1004,
	};

protected:
	ioUIRenderImage *m_pCardText1;
	ioUIRenderImage *m_pCardText2;
	ioUIRenderImage *m_pCardTextNum;
	ioUIRenderImage *m_pNoneCardText1;
	ioUIRenderImage *m_pNoneCardText2;

protected:
	ioUIRenderFrame *m_pNameBubble;
	ioUIRenderImage *m_pNameBubbleTail;
	ioUIRenderFrame *m_pNameBubbleBlue;
	ioUIRenderImage *m_pNameBubbleTailBlue;

protected:
	ioPlayStage	  *m_pPlayStage;
	FinalInfoList  	m_FinalInfoList;

protected:
	bool         m_bPresentAlarmBlock;
	int          m_iStartTreasureCount;
	int          m_iCurrentTreasureCount;

protected:
	ioHashString m_BackSound;
	ioHashString m_AutoOpenSound;
	DWORD        m_dwStartTime;
	DWORD        m_dwStartAlphaTime;
	DWORD        m_dwClickTime;
	DWORD        m_dwAutoOpenTime;
	DWORD        m_dwClearTime;

protected:
	DWORD        m_dwState;
	DWORD        m_dwCurrentTime;
	DWORD        m_dwCurrentTimeTick;
	DWORD        m_dwTickTime;
	DWORD        m_dwClickRemainTime;
	int			 m_iCurrentStateXPos;
	int			 m_iCurrentStateYPos;
	float        m_fCurrentStateRate;
	int          m_iCurrentStateSec;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	bool IsPresentAlarmBlock();

public:
	void SetStartTreasure( int iTreasureCount );
	void SetPlayStage( ioPlayStage *pPlayStage );
	void AddFinalInfo( const FinalInfo &rkInfo );
	void SetAllCardScreenStart();
	void SetAllCardActive( bool bActive );
	void TreasureCardTimeOut( SP2Packet &rkPacket );
	void TreasureCardClick( SP2Packet &rkPacket );
	
protected:
	int GetEmptyCardCount();
	int GetTotalRemainCardClickCount();

protected:
	void SendTimeOut();

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ProcessState();
	void ProcessRandomCard( bool bOneCard );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRenderAfterChild();

protected:
	void OnRenderStartMent( int iXPos, int iYPos );
	void OnRenderBubbleName( const ioHashString &rkName, int iXPos, int iYPos );

public:
	TotalResultTreasureCardWnd();
	virtual ~TotalResultTreasureCardWnd();
};

//////////////////////////////////////////////////////////////////////////
class TotalResultTournamentWnd : public ioWnd
{
public:
	enum
	{
		TORUNAMENT_GUAGE_COMPLETE,
	};

protected:
	enum
	{
		TOURNAMENT_VIEW_MAX = 8,
		GAUGE_WIDTH	  = 440,
		GAGUE_MIDDILE = 4,
	};

	enum
	{
		VIEW_WIDTH = 536,
		VIEW_HEIGHT= 317,
	};

protected:
	ioUIRenderFrame* m_pResultTextBack;

	ioUIRenderImage* m_pBack;
	ioUIRenderImage* m_pProgressGaugeBack;
	ioUIRenderImage* m_pProgressGauge;
	ioUIRenderImage* m_pProgressGaugeArrow;
	ioUIRenderImage* m_pChampIcon;
	ioUIRenderImage* m_pChampIconBack;
	ioUIRenderImage* m_pPesoIcon;
	ioUIRenderImage* m_pPesoIconBack;
	ioUIRenderImage* m_pPesoText;
	ioUIRenderImage* m_pPesoNum;

	ioUIRenderImage* m_pBlueRoundNum;			//숫자
	ioUIRenderImage* m_pBlueRoundText;			//강
	ioUIRenderImage* m_pBlueFinalRoundText;		//결승
	ioUIRenderImage* m_pBlueNextGoText;			//진출
	ioUIRenderImage* m_pBlueNextFailText;		//탈락
	ioUIRenderImage* m_pBlueChampionText;		//최종우승
	ioUIRenderImage* m_pBlueHalfChampionText;	//준우승

	ioUIRenderImage* m_pRedRoundNum;
	ioUIRenderImage* m_pRedRoundText;
	ioUIRenderImage* m_pRedFinalRoundText;
	ioUIRenderImage* m_pRedNextGoText;
	ioUIRenderImage* m_pRedNextFailText;
	ioUIRenderImage* m_pRedChampionText;
	ioUIRenderImage* m_pRedHalfChampionText;

	ioHashString m_szWinBackSound;
	ioHashString m_szLoseBackSound;
	ioHashString m_szResultTextSound;
	ioHashString m_szPesoRollingSound;
	ioHashString m_szGaugeStrechSound;

	bool m_bResultTextSound;
	bool m_bPesoRollingSound;
	bool m_bGaugeStrechSound;

protected:
	ioPlayStage *m_pPlayStage;

protected:
	int m_iTournamentMaxRound;
	int m_iTournamentCurRound;
	int m_iTournamentCurPos;
	int m_iTournamentStartTeamCount;

	int m_iTournamentPageMax;
	int m_iTournamentPageMaxRound;
	
	int m_iTournamentCurPage;
	int m_iTournamentGaugeWidth;
		
	int m_iPesoOrgValue;
	int m_iPesoCurValue;
	int m_iPesoSize;
	WinTeamType m_eWinner;
	TeamType    m_eMyTeam;

	ioHashString m_szTournamentTeamName;

protected:
	ioUI3DRender   *m_pUI3DRender;
	ioCreateChar   *m_aMyChar;
	CHARACTER       m_CharInfo;

	ioHashString	m_szWinMotion;
	ioHashString	m_szLoseMotion;
	
protected:
	DWORD m_dwStartTime;
	DWORD m_dwRolliningStartTime;

	DWORD m_dwTeamNameStartTime;
	DWORD m_dwTeamNameGapTime;
	DWORD m_dwTeamNameXPos;

	DWORD m_dwResultTextStartTime;
	DWORD m_dwResultTextGapTime;

	DWORD m_dwResultTextBackStartTime;
	DWORD m_dwResultTextBackGapTime;

	DWORD m_dwResultTextBlinkStartTime;
	DWORD m_dwResultTextBlinkGapTime;

	DWORD m_dwPesoRollingStartTime;
	DWORD m_dwPesoRollingGapTime;
	DWORD m_dwPesoRollingTime;

	DWORD m_dwPesoBlinkStartTime;
	DWORD m_dwPesoBlinkGapTime;
	DWORD m_dwPesoBlinkScale;

	DWORD m_dwGagueStrechStartTime;
	DWORD m_dwGagueStrechGapTime;

	DWORD m_dwRoundTextStartTime;
	DWORD m_dwRoundTextGapTime;

	float m_fTournamentTeamAniRate;	
	float m_fTournamentResultTextAniRate;
	float m_fTournamentResultTextBackAniRate;
	float m_fTournamentResultTextBlinkAniRate;
	float m_fTournamentPesoRollingAniRate;
	float m_fTournamentPesoScalingAniRate;
	float m_fTournamentPesoBlinkAniRate;
	float m_fTournamentGaugeStrechAniRate;
	float m_fTournamentGaugeAlphaAniRate;	

protected:
	bool m_bCheckLoopFunc;

public:
	virtual void iwm_create();
	virtual void iwm_show();

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetTournamentData();
	void SetPesoValue( int iPeso );
	void SetTeamScore( TeamType eMyTeam, float fBlueWin, float fRedWin );
	
	void SetGaugeValue();

protected:
	void OnTeamNameMoveProcess( float dwCurTime );
	void OnResultTextMoveProcess( float dwCurTime );
	void OnResultTextBackProcess( float dwCurTime );
	void OnResultTextBlinkProcess( float dwCurTime );
	void OnPesoRollingProcess( float dwCurTime );
	void OnPesoBlinkProcess( float dwCurTime );
	void OnGaugeStrechProcess( float dwCurTime );
	void OnGaugeTextAlphaProcess( float dwCurTime );

protected:
	void OnTeamNameRender( int iXPos, int iYPos );
	void OnTournamentTextRender( int iXPos, int iYPos );
	void OnTournamentPesoRender( int iXPos, int iYPos );

	void OnProgressGaugeRender( int iXPos, int iYPos );
	void OnRenderSoldier( int iXPos, int iYPos );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	TotalResultTournamentWnd();
	virtual ~TotalResultTournamentWnd();
};

//////////////////////////////////////////////////////////////////////////
class ResultScoreUserWnd : public ioWnd
{
protected:
	ioUIRenderFrame *m_pMyTeamFrm;
	ioUIRenderFrame *m_pGuildMarkFrm;
	ioUIRenderFrame *m_pCampBonusFrm;
	ioUIRenderFrame *m_pShuffleFrm;

protected:
	ioUIRenderImage *m_pBlueWin;
	ioUIRenderImage *m_pBlueLose;
	ioUIRenderImage *m_pBlueDraw;
	ioUIRenderImage *m_pRedWin;
	ioUIRenderImage *m_pRedLose;
	ioUIRenderImage *m_pRedDraw;
	ioUIRenderImage *m_pGameOver;
	ioUIRenderImage *m_pClear;
	ioUIRenderImage *m_pCampBlueMark;
	ioUIRenderImage *m_pCampRedMark;
	ioUIRenderImage *m_pCampMarkBack;
	ioUIRenderImage *m_pModeIconBack;
	ioUIRenderImage *m_pHumanIcon;
	ioUIRenderImage *m_pHumanText;
	ioUIRenderImage *m_pGangsiText;

	//
	ioUIRenderImage *m_pBlueDark;
	ioUIRenderImage *m_pBlueLight;
	ioUIRenderImage *m_pBlueOver;
	ioUIRenderImage *m_pRedDark;
	ioUIRenderImage *m_pRedLight;
	ioUIRenderImage *m_pRedOver;
	
	//
	ioUIRenderImage *m_pPCRoomBlue;
	ioUIRenderImage *m_pPCRoomRed;
	ioUIRenderImage *m_pGuildBlue;
	ioUIRenderImage *m_pGuildRed;
	ioUIRenderImage *m_pFriendBlue;
	ioUIRenderImage *m_pFriendRed;
	ioUIRenderImage *m_pVictoriesBlue1;
	ioUIRenderImage *m_pVictoriesRed1;
	ioUIRenderImage *m_pVictoriesBlue2;
	ioUIRenderImage *m_pVictoriesRed2;
	ioUIRenderImage *m_pVictoriesNumBlue;
	ioUIRenderImage *m_pVictoriesNumRed;

	//
	ioUIRenderImage *m_pBlueNumber;
	ioUIRenderImage *m_pRedNumber;
	ioUIRenderImage *m_pBluePer;
	ioUIRenderImage *m_pRedPer;
	ioUIRenderImage *m_pBluePointText;
	ioUIRenderImage *m_pRedPointText;
	ioUIRenderImage *m_pRedRankText;

	//
	ioUIRenderImage *m_pGradeUpText;

	//
	ioUIRenderImage *m_pCurrentDark;
	ioUIRenderImage *m_pCurrentLight;
	ioUIRenderImage *m_pCurrentOver;
	ioUIRenderImage *m_pCurrentResult;
	ioUIRenderImage *m_pCurrentPoint;
	ioUIRenderImage *m_pCurrentPointText;

	//
protected:
	TeamType m_eWindowTeam;
	int      m_iMaxPlayer;
	DWORD    m_dwTickTime;
	DWORD    m_dwCurrentTime;
	int      m_iStartIndex;
	int      m_MouseOverIndex;
	int      m_iPoint;
	bool     m_bGangsiWin;

protected:
	DWORD    m_dwRollingTick;
	DWORD    m_dwRollingCurrentTime;
	int      m_iRollingIndex;

protected:
	ioPlayStage	  *m_pPlayStage;
	FinalInfoList m_FinalInfoList;

	float m_fBlueGuildTeamBonus;
	float m_fRedGuildTeamBonus;

protected:
	enum GradeUpAni
	{
		GUA_NONE = 0,
		GUA_START,
		GUA_INFO_VIEW,	
		GUA_SCALE_UP1,	
		GUA_SCALE_UP2,	
		GUA_SCALE_UP3,
	};
	FSMStateMgr m_GradeUpAni;

protected:
	ioHashString m_szCountSound;

	ioHashString m_szBlueTeamName;
	ioHashString m_szRedTeamName;

protected:
	bool m_bSocrePointRender;

protected:
	ioUIRenderImage* m_pShuffleResultIcon;
	ioUIRenderImage* m_pShuffleResultIconBack;
	ioUIRenderImage* m_pShuffleLine;

	int m_iStarCount;
	int m_iStarCountByCalcBonus;
	float m_fBonusStarPercent;

protected:
	void InitGradeUpAni();
	int GetMyContribute();
	int GetMyKO();
	int GetResultLadderPoint();
	int GetMyShuffleStar();
	bool IsMyTeam();
	int GetMyHunterCoin();

public:
	void SetCustomWnd( int iMaxPlayer, int iWndXPos, int iWndYPos, int iWndWidth, int iWndHeight );
	void SetStartScore();
	void SetWindowTeam( TeamType eTeam );
	void SetSingleScore();
	void SetDungeonsScore( int iResultFloor, int iMaxFloor );
	void SetTeamScore( TeamType eMyTeam, int iBlueWin, int iRedWin, bool bSocrePointRender = true );
	void SetGangsiScore( bool bBlueTeamWin );
	void SetGuildTeamBonus( float fBlueGuildTeamBonus, float fRedGuildTeamBonus );
	void SetPlayStage( ioPlayStage *pPlayStage );	
	void AddFinalInfo( const FinalInfo &rkInfo );

public:
	virtual void iwm_show();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ProcessGradeUpAni();
	void ProcessSiteClick();
	void ProcessRolling();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderResult( int iXPos, int iYPos );
	void OnRenderGangsiResult( int iXPos, int iYPos );
	void OnRenderBack( int iXPos, int iYPos );
	void OnRenderUser( int iXPos, int iYPos, ModeType eType );
	void OnRenderCampBonus( int iXPos, int iYPos );

	void OnRenderShuffleResult( int iXPos, int iYPos );

public:
	ResultScoreUserWnd();
	virtual ~ResultScoreUserWnd();
};
//////////////////////////////////////////////////////////////////////////
class ResultRaidScoreUserBtn : public ioButton
{
public:
	enum
	{	
		MAX_RANK = 4,
		MAX_MVP_NUMBER = 5,
	};

	enum
	{
		S_RANK = 0,
		A_RANK = 1,
		B_RANK = 2,
		C_RANK = 3,
		MAX_RANK_ELEMENT = 4,
	};

	enum
	{
		VIEW_WIDTH = 98,
		VIEW_HEIGHT= 220,
	};

	enum
	{
		STATE_NONE = 0,
		STATE_RANK_ROLLING = 2000,
		STATE_RANK_ROLLING_STOP = 2001,
		STATE_MVP = 2003,
	};

protected:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_aMyChar;

protected:
	ioUIRenderFrame *m_pSlotNormal;
	ioUIRenderFrame *m_pSlotOver;

	ioUIRenderImage *m_pSlotGra;
	ioUIRenderImage *m_pSlotGraOver;
	ioUIRenderImage *m_pEmptySlot;
	ioUIRenderImage *m_pSlotMVP[MAX_MVP_NUMBER];
	ioUIRenderImage *m_pSlotLineDark;
	ioUIRenderImage *m_pSlotLineLight;
	ioUIRenderImage *m_pSlotRank[MAX_RANK];

protected:
	ioPlayStage *m_pPlayStage;
	FinalInfo    m_FinalUser;
	int          m_iRankElement[MAX_RANK_ELEMENT];
	bool         m_bRaidClear;
	int          m_iCurrentRank;
	float        m_fCurrentRankRate;
	float        m_fCurrentMVPRate;

protected:
	DWORD        m_dwState;
	DWORD        m_dwCurrentTime;

	DWORD        m_dwRollingTime;
	DWORD        m_dwRollingStopTime;
	DWORD        m_dwMVPTime;
	ioHashString m_szRollingStopSnd;
	ioHashString m_szMVPSnd;

protected:
	void CreateCharacter();

protected:
	int GetRankCompare( int iRank );
	int GetRaidRankChange( int iMaxPlayer, int iRank );

public:
	virtual void iwm_create();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ProcessRolling();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderSoldier( int iXPos, int iYPos );
	void OnRenderSlot( int iXPos, int iYPos );
	void OnRenderRank( int iXPos, int iYPos );

public:
	void SetStartScore( bool bClear );
	void SetMVP();
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetFinalInfo( int iMaxPlayer, const FinalInfo &rkInfo );
	bool IsEmptySlot(){ return m_FinalUser.m_szName.IsEmpty(); }
	
public:
	const ioHashString &GetUserName(){ return m_FinalUser.m_szName; }
	DWORD GetState(){ return m_dwState; }

public:
	ResultRaidScoreUserBtn();
	virtual ~ResultRaidScoreUserBtn();
};
//////////////////////////////////////////////////////////////////////////
class ResultRaidScoreUserWnd : public ioWnd
{
public:
	enum
	{
		ID_USER_SLOT1 = 1,
		ID_USER_SLOT2 = 2,
		ID_USER_SLOT3 = 3,
		ID_USER_SLOT4 = 4,
	};

	enum
	{
		STATE_NONE  = 0,
		STATE_MVP   = 2000,
		STATE_DELAY = 2001,
	};

protected:
	ioPlayStage	  *m_pPlayStage;
	ioHashString   m_szRollingSound;
	bool           m_bRaidClear;

protected:
	DWORD m_dwCurrentTime;
	DWORD m_dwState;

	DWORD m_dwMVPDelay;
	DWORD m_dwEndDelay;

public:
	void SetStartScore();
	void SetRaidScore( bool bClear );
	void SetDungeonsScore( int iResultFloor, int iMaxFloor );
	void SetPlayStage( ioPlayStage *pPlayStage );
	void AddFinalInfo( int iMaxPlayer, const FinalInfo &rkInfo );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void ProcessState();

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	ResultRaidScoreUserWnd();
	virtual ~ResultRaidScoreUserWnd();
};
//////////////////////////////////////////////////////////////////////////
class TotalResultScoreWnd : public ioWnd
{
public:
	enum
	{
		ID_SINGLE_WND = 1,     // 해골영웅,보스레이드,데스매치
		ID_WINNER_WND = 2,     // 팀 모드 & 강시 모드 
		ID_LOSER_WND  = 3,     // 팀 모드 & 강시 모드
		ID_RAID_WND   = 4,     // PvE 모드
	};

protected:
	ioPlayStage	  *m_pPlayStage;
	FinalInfoList m_FinalInfoList;

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void AddFinalInfo( const FinalInfo &rkInfo );
	void SetSingleScore();
	void SetDungeonsScore( int iResultFloor, int iMaxFloor );
	void SetTowerDefScore();
	void SetRaidScore( bool bClear );
	void SetTeamScore( TeamType eMyTeam, int iBlueWin, int iRedWin, TeamType eExceptTourWinTeam = TEAM_NONE );
	void SetGangsiScore();
	void SetGuildTeamBonus( float fBlueGuildTeamBonus, float fRedGuildTeamBonus );
	
public:
	void SetStartScore();

public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	TotalResultScoreWnd();
	virtual ~TotalResultScoreWnd();
};
//////////////////////////////////////////////////////////////////////////
class TotalResultMainWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_AWARD_WND			 = 1,
		ID_SCORE_WND			 = 2,
		ID_EXP_WND				 = 3,
		ID_REWARD_WND			 = 4,
		ID_TREASURE_WND			 = 5,
		ID_TOURNAMENT_RESULT_WND = 6,
		ID_RAID_CARD_WND		 = 7,
		ID_SPIRIT_REWARD_WND	 = 8,
		ID_CHAT_WND			= 100,
		ID_ALPHA_WND		= 101,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

	int	  m_iDefaultX;
	int   m_iDefaultY;

protected:
	ioPlayStage	  *m_pPlayStage;
	FinalInfoList m_FinalInfoList;

protected:
	static bool sg_bHelpBarShow;
	static bool sg_bLevelGaugeAniEnd;

protected:
	ioUIRenderFrame* m_pTournamentTtitleBack;
	ioUITitle *m_pTournamentUITitle;

protected:
	float m_fContributeTotalBlueScore;
	float m_fContributeTotalRedScore;

	int m_iSpiritCode;
	int m_iSpiritCount;

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	static bool IsHelpBarShow(){ return sg_bHelpBarShow; }
	static bool IsNoPenalty(){ return sg_bHelpBarShow; } 
	static bool IsLevelGaugeAniEnd(){ return sg_bLevelGaugeAniEnd; }

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	void SetAwarding( SP2Packet &rkPacket );
	void SetAwardingCharMotion( const ioHashString &rkCharName, int iAwardType, DWORD dwCharMotionItem );
	void AddFinalInfo( const FinalInfo &rkInfo );

	/************************************************************************/
	/*  시상식 UI에서 예전에 사용했던 함수들....                            */
	/************************************************************************/	
public:
	bool IsPresentAlarmBlock();
	int GetAwardCharItemReinforce( const ioHashString &szName, int iSlot );
	int GetAwardCharLevel( const ioHashString &szName );
	void GetAwardMedalItemType( IN const ioHashString &szName, OUT MedalSyncInfoVec &rvMedalItemTypeVec );

	/************************************************************************/
	/*  아래에 함수를 추가해서 모드별로 특별한 정보들을 수집한다. 예전처럼  */
	/*  XML을 계속 늘려나가면 이미지 수정하나만해도 XML을 너무 바꿔줘야해서 */
	/*  불편함..															*/
	/************************************************************************/	
public:
	void SetSingleScore();
	void SetGangsiScore();
	void SetDungeonsScore( int iResultFloor, int iMaxFloor );
	void SetTowerDefScore();
	void SetRaidScore( bool bClear );
	void SetTeamScore( TeamType eMyTeam, int iBlueWin, int iRedWin );
	void SetGuildTeamBonus( float fBlueGuildTeamBonus, float fRedGuildTeamBonus );
	void SetFinalRoundPoint( IntVec &rvClassTypeList, IntVec &rvClassPointList, float fBonusArray[ioPlayMode::BA_MAX] );
	void SetTournamentContributeScore( float fBlueScore, float fRedScore );
	void SetShufflePoint( int iPlayPoint, int iWinningPoint, int iConsecutivePoint, int iAwardPoint );
	void SetSuccessionScore( TeamType eMyTeam, int iBlueWin, int iRedWin, int iBonusPeso );
	void SetStartRaidTreasure( int nCnt );

	/************************************************************************/
	/*  채팅 내용을 시상식 3D 캐릭터 머리에 말풍선으로 표시                 */
	/************************************************************************/
public:
	void StartChatBubble( const char *szBubbleName, const char *szChat, bool bWholeChat );
	
	/************************************************************************/
	/*  보물 카드				                                            */
	/************************************************************************/
public:
	void TreasureCardTimeOut( SP2Packet &rkPacket );
	void TreasureCardClick( SP2Packet &rkPacket );

	void ShowSpiritRewardWnd( bool bCard );
	void SetSpiritRewardInfo( int iCode, int iCount );

public:
	TotalResultMainWnd();
	virtual ~TotalResultMainWnd();
};
//////////////////////////////////////////////////////////////////////////
class TotalResultChatWnd : public ioWnd
{
public:
	enum
	{
		ID_CHAT_MSG_WND   = 1,
		ID_CHAT_INPUT_WND = 2,
	};

protected:
	ioHashString m_szLoadInfo;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	TotalResultChatWnd();
	virtual ~TotalResultChatWnd();
};

//////////////////////////////////////////////////////////////////////////
class TotalResultRaidCardBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pCardCover;
	ioUIRenderImage *m_pPresentCover;
	ioUIRenderImage *m_pPresentCoverOverAdd;
	ioUIRenderImage *m_pPresentCoverMe;

protected:
	ioUIRenderImage *m_pPresentIcon;
	ioUIRenderImage *m_pSubIcon;

protected:
	ioHashString m_Name;

protected:
	short m_iPresentType;
	int   m_iPresentValue1;
	int   m_iPresentValue2;

protected:
	bool  m_bScreenStart;
	bool  m_bScreen;
	DWORD m_dwCurrentTime;;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderPresent( int iXPos, int iYPos );

public:
	void SetName( const ioHashString &rkName );
	void SetPresent( short iPresentType, int iPresentValue1, int iPresentValue2 );
	void SetScreenStart();

public:
	const ioHashString &GetName(){ return m_Name; }
	bool IsEmptyCard(){ return (m_iPresentType == 0); }

public:
	TotalResultRaidCardBtn();
	virtual ~TotalResultRaidCardBtn();
};

class TotalResultRaidCardWnd : public ioWnd
{
public:
	enum
	{
		ID_CARD_1 = 1,
		MAX_CARD_SLOT = 5,
	};
	
	enum
	{
		STATE_NONE		= 0,
		STATE_START		= 1000,
		STATE_CLICK		= 1001,
		STATE_DELAY     = 1002,
		STATE_AUTO_OPEN = 1003,
		STATE_CLEAR		= 1004,
	};

protected:
	ioUIRenderImage *m_pCardText1;
	ioUIRenderImage *m_pCardText2;
	ioUIRenderImage *m_pCardTextNum;
	ioUIRenderImage *m_pNoneCardText1;
	ioUIRenderImage *m_pNoneCardText2;

protected:
	ioUIRenderFrame *m_pNameBubble;
	ioUIRenderImage *m_pNameBubbleTail;
	ioUIRenderFrame *m_pNameBubbleBlue;
	ioUIRenderImage *m_pNameBubbleTailBlue;

protected:
	ioPlayStage	  *m_pPlayStage;
	FinalInfoList  	m_FinalInfoList;

protected:
	bool         m_bPresentAlarmBlock;
	int          m_iStartTreasureCount;
	int          m_iCurrentTreasureCount;

protected:
	ioHashString m_BackSound;
	ioHashString m_AutoOpenSound;
	DWORD        m_dwStartTime;
	DWORD        m_dwStartAlphaTime;
	DWORD        m_dwClickTime;
	DWORD        m_dwAutoOpenTime;
	DWORD        m_dwClearTime;

protected:
	DWORD        m_dwState;
	DWORD        m_dwCurrentTime;
	DWORD        m_dwCurrentTimeTick;
	DWORD        m_dwTickTime;
	DWORD        m_dwClickRemainTime;
	int			 m_iCurrentStateXPos;
	int			 m_iCurrentStateYPos;
	float        m_fCurrentStateRate;
	int          m_iCurrentStateSec;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	bool IsPresentAlarmBlock();

public:
	void SetStartTreasure( int iTreasureCount );
	void SetPlayStage( ioPlayStage *pPlayStage );
	void AddFinalInfo( const FinalInfo &rkInfo );
	void SetAllCardScreenStart();
	void SetAllCardActive( bool bActive );
	void TreasureCardTimeOut( SP2Packet &rkPacket );
	void TreasureCardClick( SP2Packet &rkPacket );

protected:
	int GetEmptyCardCount();
	int GetTotalRemainCardClickCount();

protected:
	void SendTimeOut();

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ProcessState();
	void ProcessRandomCard( bool bOneCard );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRenderAfterChild();

protected:
	void OnRenderStartMent( int iXPos, int iYPos );
	void OnRenderBubbleName( const ioHashString &rkName, int iXPos, int iYPos );

public:
	TotalResultRaidCardWnd();
	virtual ~TotalResultRaidCardWnd();
};
//////////////////////////////////////////////////////////////////////////
class TotalResultSpiritRewardWnd : public ioWnd
{
private:
	ioUIRenderImage *m_pIcon;
	ioUI3DEffectRender*	m_pEffect;
	ioHashString m_szDescText[2];
	int m_iSpiritCode;
	int m_iCount;

public:
	virtual void iwm_show();
	virtual void iwm_hide();

	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnRender();

	void SetInfo( int iCode, int iCount );

public:
	TotalResultSpiritRewardWnd();
	virtual ~TotalResultSpiritRewardWnd();
};