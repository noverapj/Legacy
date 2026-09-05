#pragma once

class RankBattleWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_MY_RANK_BTN			= 1,
		ID_TAB_RANK_GRAPH		= 2,
		ID_TAB_RANK_TOP100		= 3,
		ID_TAB_BATTLE_RECORD	= 4,		
		ID_LEVEL_MATCH_SEARCH	= 5,
		ID_OTHER_CHAT_HIDE      = 6,
		ID_BAD_PING_KICK        = 7,

		ID_SEARCH_START			= 10,
		ID_SEARCH_END			= 11,
		ID_EXIT					= 12,
		ID_TOOLTIP				= 13,

		ID_RANK_GRAPH_WND		= 100,
		ID_RANK_TOP100_WND		= 101,
		ID_BATTLE_RECORD_WND	= 102,
		ID_CHAT_WND				= 1001,
	};
	enum
	{
		RANK_BRONZE = 0,
		RANK_SILVER = 1,
		RANK_GOLD = 2,
		RANK_PLATINUM = 3,
		RANK_DIA = 4,
		RANK_MASTER = 5,
		RANK_SIZE = 6,

		VIEW_WIDTH = 121,
		VIEW_HEIGHT= 240,

		NUM_W = 27, 
		NUM_GAP = 5, 
		TEXT_RANK_W = 27,
		TEXT_SEASON_W = 31,

		SEARCH_TIME = 180,
	};

protected:
	ioPlayStage		*m_pPlayStage;	
	ioUIRenderFrame	*m_pModeNMapGrayFrm;
	ioUIRenderFrame *m_pFrameBG[RANK_SIZE];
	ioUIRenderImage *m_pImageBG[RANK_SIZE];
	ioUIRenderImage *m_pGradation[RANK_SIZE];
	ioUIRenderImage *m_pRankContext[RANK_SIZE];
	ioUIRenderImage *m_pRankIcon[RANK_SIZE];
	ioUIRenderImage *m_pRankTitle;
	ioUIRenderImage *m_pGradeTitle;
	ioUIRenderImage *m_pSeasonTitle;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pRecordIcon;
	ioUIRenderImage *m_pNumber;
	ioUIRenderImage *m_pRankText;
	ioUIRenderImage *m_pPercentText;
	ioUIRenderImage *m_pNoneText;
	ioUIRenderImage *m_pNoneIcon;
	ioUIRenderImage *m_pDarkBar;
	ioUIRenderImage *m_pLightBar;

	ioUIRenderImage *m_pWarCountWait;
	ioUIRenderImage *m_pWarCountCircle;
	ioUIRenderImage *m_pTextBack;

	ioHashString m_szBackOgg;
	ioHashString m_szDescString[6];
	ioHashString m_szOnModeNMapRender[4];
	ioHashString m_szRemainTime;
	ioHashString m_szAlarmText;

	ioUI3DRender   *m_pUI3DRender;
	ioCreateChar   *m_aMyChar;
	CHARACTER       m_CharInfo;

	DWORD m_dwTabID;
	DWORD m_dwCurSearchTime;
	DWORD m_dwChangeCountTime;

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetRankGraph();
	void ResetButtons();
	void ResizeScroll();

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	void OnModeNMapRender( int iXPos, int iYPos );
	void OnRenderInfo( int iXPos, int iYPos );
	void OnRenderSoldier( int iXPos, int iYPos );

	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	void CreateCharacter();
	void CheckTabWnd( DWORD dwID );

public:
	RankBattleWnd();
	virtual ~RankBattleWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class ioRankBattleUpWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_CLOSE= 2,
		ID_ALPHA_WND   = 100,

		RANK_SIZE = 6,
	};

protected:
	ioUIRenderImage *m_pIconBackUp;
	ioUIRenderImage *m_pIconBackDown;
	ioUIRenderImage *m_pRankIcon[RANK_SIZE];

	ioHashString    m_szUpSound;
	ioHashString	m_szUpTitle;
	ioHashString	m_szUpDesc;
	ioHashString	m_szDownTitle;
	ioHashString	m_szDownDesc;
	ioHashString	m_szRankUpText[2];
	ioHashString	m_szRankDownText;

	int m_iRankGrade;
	bool m_bUp;

public:
	void SetPrintMessage( int iRankGrade, bool bUp );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ioRankBattleUpWnd();
	virtual ~ioRankBattleUpWnd();
};