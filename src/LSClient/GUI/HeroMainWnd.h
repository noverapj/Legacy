#pragma once

#include "../ioLadderTeamMgr.h"
#include "../ioRankBattleManager.h"

enum
{
	HERO_MATCH = 1,
	RANK_BATTLE = 2,
};

class HeroMainWnd : public ioWnd
{
public:
	enum
	{
		ID_HERO_MATCH_BTN   = 1,
		ID_CAMP_INFO_BTN    = 2,
				
		ID_CAMP_INFO_WND    = 11,
		ID_HERO_MATCH_WND   = 12,
	};

protected:
	DWORD m_dwTabID;

public:
	void CheckCampButton( DWORD dwID );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	HeroMainWnd();
	virtual ~HeroMainWnd();
};
//////////////////////////////////////////////////////////////////////////
class HeroMatchRankBtn : public ioButton
{
public:
	enum
	{
		VIEW_WIDTH = 121,
		VIEW_HEIGHT= 177,
	};

protected:
	ioUI3DRender   *m_pUI3DRender;
	ioCreateChar   *m_aMyChar;
	CHARACTER       m_CharInfo;

protected:
	ioUIRenderFrame *m_pBlueBar;
	ioUIRenderFrame *m_pRedBar;
	ioUIRenderFrame *m_pGrayBar;
	ioUIRenderFrame *m_pBlueOverBar;
	ioUIRenderFrame *m_pRedOverBar;
	ioUIRenderFrame *m_pGrayOverBar;

	ioUIRenderImage *m_pBlueGra;
	ioUIRenderImage *m_pRedGra;
	ioUIRenderImage *m_pGrayGra;

	ioUIRenderImage *m_pBlueOverGra;
	ioUIRenderImage *m_pRedOverGra;
	ioUIRenderImage *m_pGrayOverGra;

	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pNoneIcon;
	ioUIRenderImage *m_pHeroRankTitle;
	ioUIRenderImage *m_pNoneText;
	ioUIRenderImage *m_pNumber;
	ioUIRenderImage *m_pRankText;

	ioUIRenderFrame *m_pCurrentBar;
	ioUIRenderFrame *m_pCurrentOverBar;
	ioUIRenderImage *m_pCurrentGra;
	ioUIRenderImage *m_pCurrentOverGra;

public:
	virtual void iwm_create();

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderSoldier( int iXPos, int iYPos );

public:
	void CreateButton();

public:
	HeroMatchRankBtn();
	virtual ~HeroMatchRankBtn();
};
//////////////////////////////////////////////////////////////////////////
class HeroMatchWnd : public ioWnd
{
public:
	enum
	{
		ID_BLUE_TEAM_JOIN         = 1,
		ID_RED_TEAM_JOIN          = 2,
		ID_NONE_TEAM_JOIN         = 3,

		ID_BLUE_RANKING           = 4,
		ID_RED_RANKING            = 5,
		ID_NONE_RANKING           = 6,

		ID_MY_RANK_BTN			  = 7,

		//광장에서는 래더전 내부와 동일한 정보를 출력하기 위한 윈도우
		PLAZA_HERO_MATCH_INFO_BTN = 8,		
		PLAZA_HERO_MATCH_TEXT	  = 100,
	};

protected:
	ioUIRenderFrame *m_pBlueBar;
	ioUIRenderFrame *m_pRedBar;
	ioUIRenderFrame *m_pGrayBar;

	ioUIRenderImage *m_pBlueGra;
	ioUIRenderImage *m_pRedGra;
	ioUIRenderImage *m_pGrayGra;

	ioUIRenderImage *m_pBlueOverGra;
	ioUIRenderImage *m_pRedOverGra;
	ioUIRenderImage *m_pGrayOverGra;

	ioUIRenderImage *m_pHeroRankTitle;
	ioUIRenderImage *m_pSeasonHeroTitle;
	ioUIRenderImage *m_pSeasonRecordTitle;

	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pRecordIcon;
	ioUIRenderImage *m_pNumber;
	ioUIRenderImage *m_pRankText;
	ioUIRenderImage *m_pPercentText;
	ioUIRenderImage *m_pNoneText;
	ioUIRenderImage *m_pNoneIcon;

	ioUIRenderFrame *m_pCurrentBar;
	ioUIRenderImage *m_pCurrentGra;
	ioUIRenderImage *m_pCurrentOverGra;

	ioUIRenderImage *m_pDarkBar;
	ioUIRenderImage *m_pLightBar;

//광장 전용
protected:
	ioUIRenderFrame *m_pBlueBarByPlaza;
	ioUIRenderFrame *m_pRedBarByPlaza;
	ioUIRenderFrame *m_pGrayBarByPlaza;

	ioUIRenderImage *m_pBlueGraByPlaza;
	ioUIRenderImage *m_pRedGraByPlaza;
	ioUIRenderImage *m_pGrayGraByPlaza;
	
	ioUIRenderImage *m_pTextLine;

protected:
	int m_iMyCampType;
	
protected:
	void ChangeCamp();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderHeroTitle( int iXPos, int iYPos );
	void OnRenderSeasonRecord( int iXPos, int iYPos );
	void OnRenderHeroTitleByPlaza( int iXPos, int iYPos );
	void OnRenderSeasonRecordByPlaza( int iXPos, int iYPos );

public:
	HeroMatchWnd();
	virtual ~HeroMatchWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildHeroRankingListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT				   = 1,
		ID_GUILD_BTN		   = 2,
		ID_HERO_TOP100_BTN     = 3,

		ID_GUILD_RANKING_WND   = 10,
		ID_HERO_RANKING_WND    = 11,
	};

protected:
	DWORD        m_dwTabID;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetRankRadioBtn( DWORD dwTabID );

public:
	void ApplyGuildList( SP2Packet &rkPacket );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	GuildHeroRankingListWnd();
	virtual ~GuildHeroRankingListWnd();
};
//////////////////////////////////////////////////////////////////////////
class HeroRankingListBtn : public ioButton
{
protected:
	int m_iHeroRank;

	ioHashString m_szBlueCampName;
	ioHashString m_szRedCampName;

public:
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_lbuttondown( const ioMouse& mouse );

public:
	void SetHeroInfo( int iHeroRank );

public:
	int GetHeroRank(){ return m_iHeroRank; }

protected:
	virtual void OnRender();

public:
	HeroRankingListBtn();
	virtual ~HeroRankingListBtn();
};
//////////////////////////////////////////////////////////////////////////
class HeroRankingListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_LEFT_PAGE		 = 1,
		ID_RIGHT_PAGE		 = 2,

		ID_LIST_1       = 100,
		ID_LIST_2       = 101,
		ID_LIST_3       = 102,
		ID_LIST_4       = 103,
		ID_LIST_5       = 104,
		ID_LIST_6       = 105,
		ID_LIST_7       = 106,
		ID_LIST_8       = 107,
		ID_LIST_9       = 108,
		ID_LIST_10      = 109,
	};

	enum
	{
		MAX_PAGE_LIST		= 10,
		MAX_LIST            = 100,
	};

protected:
	int			 m_iCurPage;	
	int          m_iMaxPage;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void UpdateSetPage( int iCurPage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	HeroRankingListWnd();
	virtual ~HeroRankingListWnd();
};
//////////////////////////////////////////////////////////////////////////
class HeroMatchHistoryBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pResultWin;
	ioUIRenderImage *m_pResultLose;
	ioUIRenderImage *m_pResultDraw;
	ioUIRenderImage *m_pNoneData;

protected:
	HeroMatchHistory m_HeroHistory;
	BattleHistory m_BattleHistory;

	int m_iMode;

public:
	void SetMatchHistory( int iArray, int iMode );
	const ioHashString &GetNickName(){ return m_HeroHistory.m_szNickName; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	HeroMatchHistoryBtn();
	virtual ~HeroMatchHistoryBtn();
};
//////////////////////////////////////////////////////////////////////////
class HeroMatchHistoryWnd : public ioWnd
{
public:
	enum
	{
		ID_LIST_1 = 100,
		ID_LIST_2 = 101,
		ID_LIST_3 = 102,
		ID_LIST_4 = 103,

		ID_VERT_SCROLL = 1000,
	};

	enum
	{
		MAX_PAGE_LIST = 4,
	};

protected:
	int			 m_iCurPos;
	int m_iMode;

public:
	virtual void iwm_show();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	void SetMode( int iMode )			{ m_iMode = iMode; }

protected:
	void UpdateSetPage( int iCurPos );

public:
	HeroMatchHistoryWnd();
	virtual ~HeroMatchHistoryWnd();
};
//////////////////////////////////////////////////////////////////////////
class HeroMatchTop100Btn : public ioButton
{
protected:
	ioUIRenderFrame *m_pCustomDisableFrm;
	ioUIRenderImage *m_pBlueCampBar;
	ioUIRenderImage *m_pRedCampBar;

protected:
	int m_iHeroRank;
	int m_iMode;

public:
	void SetTop100Rank( int iRank, int iMode );
	int  GetTop100Rank(){ return m_iHeroRank; }

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	HeroMatchTop100Btn();
	virtual ~HeroMatchTop100Btn();
};
//////////////////////////////////////////////////////////////////////////
class HeroMatchTop100Wnd : public ioWnd
{
public:
	enum
	{
		ID_LIST_1 = 100,
		ID_LIST_2 = 101,
		ID_LIST_3 = 102,
		ID_LIST_4 = 103,
		ID_LIST_5 = 104,
		ID_LIST_6 = 105,
		ID_LIST_7 = 106,
		ID_LIST_8 = 107,

		ID_VERT_SCROLL = 1000,
	};

	enum
	{
		MAX_PAGE_LIST = 8,
		MAX_TOP100    = 100,
	};


protected:
	int m_iMode;
	int			 m_iCurPos;
	IntVec       m_iSyncArray;

public:
	virtual void iwm_show();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	void SetMode( int iMode )			{ m_iMode = iMode; }
	void ResizeScroll();

public:
	void UpdateSetPage( int iCurPos );

public:
	HeroMatchTop100Wnd();
	virtual ~HeroMatchTop100Wnd();
};
//////////////////////////////////////////////////////////////////////////
class HeroMatchRankGraphWnd : public ioWnd
{
public:
	enum
	{
		MAX_GRAPH_LINE = 14,

		GRAPH_ANI_TIME  = 140,
		GRAPH_ANI_NONE	= 1,
		GRAPH_ANI_CIRCLE= 2,
		GRAPH_ANI_LINE  = 3,
	};
protected:
	ioUIRenderImage *m_pGraphRect;
	ioUIRenderImage *m_pBlueCircle;
	ioUIRenderImage *m_pRedCircle;
	ioUIRenderImage *m_pBlueTodayCircle;
	ioUIRenderImage *m_pRedTodayCircle;
	ioUIRenderImage *m_pGraphLine[MAX_GRAPH_LINE];

protected:
	struct GraphData
	{
		DWORD m_dwAniState;
		DWORD m_dwAniTime;
		float m_fAniRate;
		int   m_iRanking;
		int   m_iRankPos;
		int   m_iNextRankPosGap;
		bool  m_bViewRanking;
		bool  m_bLineReverse;
		ioUIRenderImage *m_pCircle;
		ioUIRenderImage *m_pLine;

		GraphData()
		{
			m_dwAniState   = GRAPH_ANI_NONE;
			m_dwAniTime    = 0;
			m_fAniRate     = 0.0f;
			m_bViewRanking = false;
			m_bLineReverse = false;
			m_iRanking = m_iRankPos = m_iNextRankPosGap = 0;
			m_pCircle  = m_pLine = NULL;
		}        
	};
	typedef std::vector< GraphData > vGraphData;
	vGraphData m_GraphData;
	int m_iMode;

public:
	virtual void iwm_show();

public:
	void SetMode( int iMode )			{ m_iMode = iMode; }
	void SetRankBattleGraph();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderRect( int iXPos, int iYPos );
	void OnRenderDay( int iXPos, int iYPos );
	void OnRenderGraph( int iXPos, int iYPos );

protected:
	void ProcessCircleOver();
	void ProcessGraphAni();

	void SetHeroMatchGraph();

public:
	HeroMatchRankGraphWnd();
	virtual ~HeroMatchRankGraphWnd();
};
//////////////////////////////////////////////////////////////////////////
class HeroMatchVersusSceneWnd : public ioWnd
{
public:
	enum
	{
		MAX_EFFECT_SPR = 10,

		VIEW_WIDTH = 141,
		VIEW_HEIGHT= 240,
	};

protected:
	ioUI3DRender *m_pBlue3DRender;
	ioCreateChar *m_pBlueChar;

	ioUI3DRender *m_pRed3DRender;
	ioCreateChar *m_pRedChar;

protected:
	ioUIRenderFrame *m_pBlueFrame;
	ioUIRenderFrame *m_pRedFrame;
	ioUIRenderFrame *m_pBackFrame;

	ioUIRenderImage *m_pBlueGra;
	ioUIRenderImage *m_pRedGra;
	ioUIRenderImage *m_pDarkBar;
	ioUIRenderImage *m_pLightBar;
	ioUIRenderImage *m_pBlackStrip;
	ioUIRenderImage *m_pWhiteStrip;
	ioUIRenderImage *m_pHeroRankTitle;
	ioUIRenderImage *m_pNumber;
	ioUIRenderImage *m_pRankText;
	ioUIRenderImage *m_pNoneText;
	ioUIRenderImage *m_pLadderHeroMark;
	ioUIRenderImage *m_pCharShadow;
	ioUIRenderImage *m_pEffectSprite[MAX_EFFECT_SPR];

protected:
	enum
	{
		CARD_ANI_START = 0,
		CARD_ANI_BACK  = 1,
		CARD_ANI_DELAY = 2,
		CARD_ANI_FADEOUT= 3,

		START_MOVE_TIME= 230,
		BACK_MOVE_TIME = 500,
		DELAY_TIME     = 500,
		FADEOUT_MOVE_TIME = 200,
	};

	struct HeroData
	{
		LadderHeroUserInfo m_Data;

		// ANI
		DWORD m_dwAniState;
		DWORD m_dwMoveTime;
		
		float m_fAniRate;
		int m_iStartXPos;
		int m_iStartYPos;
		int m_iCurrentXPos;
		int m_iCurrentYPos;
		HeroData()
		{
			m_Data.Initialize();
			m_dwAniState = CARD_ANI_START;
			m_dwMoveTime = 0;
			m_fAniRate   = 0.0f;
			m_iStartXPos =m_iStartYPos = m_iCurrentXPos = m_iCurrentYPos = 0;
		}
	};
	HeroData m_BlueData;
	HeroData m_RedData;

	//////////////////////////////////////////////////////////////////////////
	enum
	{
		VS_ANI_NONE   = 0,
		VS_ANI_START  = 1,		
		VS_ANI_FADEOUT= 2,

		VS_START_TIME = 500,		
		VS_FADEOUT_TIME = 100,
	};

	struct VersusAni
	{
		DWORD m_dwAniState;
		DWORD m_dwAniTime;
		float m_fAniRate;

		// effect
		DWORD m_dwEffectTime;
		int   m_iEffectArray;
		VersusAni()
		{
			m_dwAniState = VS_ANI_NONE;
			m_dwAniTime  = 0;
			m_fAniRate   = 0.0f;

			m_dwEffectTime = 0;
			m_iEffectArray = 0;
		}
	};
	VersusAni m_VersusAni;

protected:
	bool  m_bFadeOutStart;
	ioPlayMode *m_pPlayMode;
	int   m_iSelectChar;
	DWORD m_dwSelectTime;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderBack( int iXPos, int iYPos );
	void OnRenderBlueChar( int iXPos, int iYPos );
	void OnRenderRedChar( int iXPos, int iYPos );
	void OnRenderVersus( int iXPos, int iYPos );

protected:
	void OnProcessChar();
	void OnProcessVS();

protected:
	ioCreateChar *GetCreateCharacter( ioUI3DRender *p3DRender, LadderHeroUserInfo &rkData );

public:
	void StartFadeOut( ioPlayMode *pPlayMode, int now_select_char, DWORD dwSelectTime );
	void OtherHeroInfoUpdate();
	
public:
	HeroMatchVersusSceneWnd();
	virtual ~HeroMatchVersusSceneWnd();
};
