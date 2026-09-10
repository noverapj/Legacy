#pragma once

#include "../io3DEngine/ioMovingWnd.h"

#include "../CharacterInfo.h"
#include "../TournamentNode.h"
#include "../ioComplexStringPrinter.h"
#include "../io3DEngine/ioStringConverter.h"

class ioUI3DRender;
class ioCreateChar;
class TournamentTeamCharWnd : public ioWnd
{
public:
	enum
	{
		VIEW_WIDTH = 96,
		VIEW_HEIGHT= 169,
	};

protected:
	ioUIRenderImage *m_pBubbleGra;
	ioUIRenderImage *m_pBubbleTail;
	ioUIRenderImage *m_pNumberText;
	ioUIRenderImage *m_pRoundText;
	ioUIRenderImage *m_pFinalText;

protected:
	ioUI3DRender   *m_pUI3DRender;
	ioCreateChar   *m_aMyChar;
	CHARACTER       m_CharInfo;
	
protected:
	BYTE  m_TourPos;
	SHORT m_Position;
	DWORD m_dwMaxRound;
	DWORD m_dwMaxStartTeam;
	DWORD m_dwNextRoundDate;

protected:
	DWORD m_dwStartTime;

public:
	virtual void iwm_create();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderSoldier( int iXPos, int iYPos );

public:
	void SetTournamentTeamData( DWORD dwMyTeamIndex );

public:
	BYTE GetTourPos(){ return m_TourPos; }
	DWORD GetMaxStartTeam(){ return m_dwMaxStartTeam; }
	DWORD GetNextRoundDate(){ return m_dwNextRoundDate; }
	
public:
	TournamentTeamCharWnd();
	virtual ~TournamentTeamCharWnd();
};

struct MentInfo
{
	int m_iXOffset;
	int m_iYOffset; 
	ioComplexStringPrinter m_szMent;
	MentInfo()
	{
		m_iXOffset = m_iYOffset = 0;
	}
};

class TournamentDetailMainTabBtn : public ioButton
{
private:
	ioUIRenderImage *m_pActiveIcon;
	bool m_bClicked;
	
public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();

public:
	bool isClick(){ return m_bClicked; }
	void SetClick( bool bClick ){ m_bClicked = bClick; }

public:
	TournamentDetailMainTabBtn();
	virtual ~TournamentDetailMainTabBtn();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailMainWnd : public ioMovingWnd
{

public:
	enum ChildType
	{
		ID_PREV_TOURNAMENT_BTN		= 1,	//이전대회정보
		ID_TOURNAMENT_DEFINE		= 2,	//대회란?
		ID_WND_CLOSE				= 3,

		ID_TOURNAMENT_INFO_BTN		= 4,
		ID_VICTORY_CHEER_BTN		= 5,
		ID_ALLOCATE_INFO_BTN		= 6,
		ID_TOURNAMENT_SCHEDULE_BTN	= 7,
		ID_AWARD_INFO_BTN			= 8,
				
		ID_TOURNAMENT_INFO_WND		= 10,
		ID_VICTORY_CHEER_WND		= 11,
		ID_ALLOCATE_INFO_WND		= 12,
		ID_TOURNAMENT_SCHEDULE_WND	= 13,
		ID_AWARD_INFO_WND			= 14,

		ID_ENTER_PLAYER				= 100,
		ID_BATTLE_ROOM_LIST         = 101,
		ID_INVITE_PLAYER			= 102,

		ID_MODE_HELP_TOOLTIP	= 500,
		ID_CHARACTER_WND        = 501,		
	};

	struct ActiveWnd
	{
		ChildType m_eBtn;
		ChildType m_eWnd;
	};

	typedef std::vector<ActiveWnd> ChildTypeList;

protected:
	DWORD m_dwCurChildActiveListIdx;
	DWORD m_dwTournamentIndex;
	DWORD m_dwTournamentOwnerIndex;

	ChildTypeList m_vChildActiveList;
	ChildTypeList m_vChildInActiveList;
	
	DWORD m_dwTournamentHelpID;

protected:
	ioEdit *m_pPreEdit;

protected:
	void EnterButtonExPopup();
	void UpdateTournamentData( TournamentNode *pTournament );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void CheckExInActvieWnd();
	void ResetChildActvieWnd();
	void NextActvieWnd();
	void PrevActvieWnd();
	void SetChildActvieWnd( DWORD dwBtnID );
	void SetAllocated( DWORD dwTourIdx );

public:
	void ApplyDecision();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTournamentTtitle( TournamentNode *pTournament );
	void SetTournamentData( DWORD dwTourIndex );

	inline DWORD GetTournamentIndex(){ return m_dwTournamentIndex; }

public:
	TournamentDetailMainWnd();
	virtual ~TournamentDetailMainWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailFrameWnd : public ioWnd
{
public:
	enum
	{		
		ID_PREV_BTN = 1,
		ID_NEXT_BTN = 2,
	};

protected:
	ioUIRenderImage *m_pScheduleProgressActiveLeft;
	ioUIRenderImage *m_pScheduleProgressActiveCenter;
	ioUIRenderImage *m_pScheduleProgressActiveRight;

	ioUIRenderImage *m_pActiveAcceptCenterClose;
	ioUIRenderImage *m_pActiveAcceptRightClose;

	ioUIRenderImage *m_pActiveCheerCenterClose;
	ioUIRenderImage *m_pActiveCheerRightClose;

	ioUIRenderImage *m_pActiveTournamentCenterClose;
	ioUIRenderImage *m_pActiveTournamentRightClose;

	ioUIRenderImage *m_pActiveAwardCenterClose;
	ioUIRenderImage *m_pActiveAwardRightClose;
	
	ioUIRenderImage *m_pScheduleProgressInactiveLeft;
	ioUIRenderImage *m_pScheduleProgressInactiveCenter;
	ioUIRenderImage *m_pScheduleProgressInactiveRight;
	ioUIRenderImage *m_pScheduleProgressInactiveClose;

	ioUIRenderImage *m_pDevideDot;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;
	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

protected:
	BYTE m_TournamentState;
	BYTE m_TournamentType;

	DWORD m_dwTournamentIndex;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_prevmenu();
	virtual void iwm_nextmenu();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void OnUpdateProcess();
	virtual void UpdateMainUI();

public:
	virtual void SetTournamentData( DWORD dwTourIndex );

protected:
	virtual void OnProcess( float fTimePerSec );

protected:
	void OnInactiveNotCloseRender( int iXPos, int iYPos );
	void OnDevideWndDotRender( int iXPos, int iYPos );

public:
	TournamentDetailFrameWnd();
	virtual ~TournamentDetailFrameWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailFrameInfoWnd : public TournamentDetailFrameWnd
{
public:
	enum
	{
		ID_INNER_TOURNAMENT_INFO	 = 10,
		ID_INNER_TOURNAMENT_SCHEDULE = 11,
		ID_INNER_TOURNAMENT_REWARD	 = 12,
	};

public:
	virtual void SetTournamentData( DWORD dwTourIndex );
	virtual void UpdateMainUI();

	void SetAllocated( DWORD dwTourIdx );

public:
	virtual void iwm_create();

protected:
	void OnActiveTypeRender( int iXPos, int iYPos );
	void OnActiveRender( int iXPos, int iYPos );
	void OnInfoRender( int iXPos, int iYPos );
	
protected:
	virtual void OnRender();
	

public:
	TournamentDetailFrameInfoWnd();
	~TournamentDetailFrameInfoWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailInnerPageNoticeWnd : public ioWnd
{
public:
	enum
	{
		ID_PREV_BTN	  = 1,
		ID_NEXT_BTN	  = 2,
		ID_NOTICE_BTN = 3,
	};

	enum
	{
		MAX_PAGE_LINE = 7,
	};

private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pHelpIcon;
	ioUIRenderImage *m_pNoticeIcon;
	ioUIRenderImage *m_pIconBack;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;
	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	int m_iCurPage;
	int m_iMaxPage;

	DWORD m_dwTournamentIndex;
		
protected://토너먼트 정보
	BYTE		 m_TournamentType;
	BYTE         m_MaxPlayer;
	ioHashString m_szTournamentTitle;
	ioHashString m_szPlayModeName;
	DWORD		 m_dwOwnerIndex;
	DWORD		 m_TournamentMaxRound;
	DWORD		 m_TournamentStartTeamCount;
	DWORDVec     m_TournamentRoundDateList;

	StringVector m_vAnnounce;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void SetTournamentData( DWORD dwTourIndex );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();


protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPageNumberRender( int iXPos, int iYPos );
	void OnIconRender( int iXPos, int iYPos );

protected:
	void OnNoticeWndRender( int iXPos, int iYPos );
	void OnHelpWndRender( int iXPos, int iYPos );
	void OnScheduleWndRender( int iXPos, int iYPos );
	void OnRewardWndRender( int iXPos, int iYPos );

public:
	TournamentDetailInnerPageNoticeWnd();
	virtual ~TournamentDetailInnerPageNoticeWnd();
};

//////////////////////////////////////////////////////////////////////////
class TournamentDetailInnerPageScheduleWnd : public ioWnd
{
public:
	enum
	{
		ID_PREV_BTN  = 1,
		ID_NEXT_BTN  = 2,
		ID_OWNER_COMMAND_BTN = 3,
	};

	enum
	{
		MAX_PAGE_LINE = 7,
	};

private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;
	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	int m_iCurPage;
	int m_iMaxPage;

	bool m_bTournamentOwner;
	DWORD m_dwTournamentIndex;

protected://토너먼트 정보
	BYTE		 m_TournamentState;
	BYTE         m_MaxPlayer;
	ioHashString m_szPlayModeName;
	DWORD		 m_dwOwnerIndex;
	DWORD		 m_TournamentMaxRound;
	DWORD		 m_TournamentStartTeamCount;
	DWORDVec     m_TournamentRoundDateList;
	int			 m_iScheduleGapTime;

protected:
	byte m_TournamentType;

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void SetTournamentData( DWORD dwTourIndex );

public:	
	void SetAllocated( DWORD dwTourIdx );	

protected:
	void CreateCustomData();
	void OnUpdateProcess();	
	void UpdateOwnerCommand( TournamentNode* pTournament );
	void UpdateOwnerCommandOnline( TournamentNode* pTournament );
	void UpdateOwnerCommandOffline( TournamentNode* pTournament );
	void UpdateOwnerCommandStateTournament( TournamentNode* pTournament );
	void UpdateScheduleInfo();
	void UpdateRegularScheduleInfo( TournamentNode* pTournament );
	void UpdateCustomScheduleInfo( TournamentNode* pTournament );
	void ChangeOwnerCommandButton( ioHashString szTitle, bool bActive, DWORD dwColor = 0 );

protected:
	void IwmOwnerCommand( DWORD dwCmd );
	void IwmOwnerNormalCommand();
	void IwmOwnerExCommand();

protected:
	void SetPageButtonDefaultPos();
	void SetPageButtonPos();


public:
	void SetTournamentStart();
	void SendTournamentStart();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();


protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnIconRender( int iXPos, int iYPos );
	void OnPageNumberRender( int iXPos, int iYPos );

protected:
	void OnHelpWndRender( int iXPos, int iYPos );
	void OnScheduleMentRender( int iXPos, int iYPos );
	void OnScheduleRegularRender( int iXPos, int iYPos );
	void OnScheduleCustomRender( int iXPos, int iYPos );
	void OnRewardWndRender( int iXPos, int iYPos );

public:
	TournamentDetailInnerPageScheduleWnd();
	virtual ~TournamentDetailInnerPageScheduleWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailInnerPageRewardWnd : public ioWnd
{
public:
	enum
	{
		ID_PREV_BTN      = 1,
		ID_NEXT_BTN      = 2,
		ID_REWARDADD_BTN = 3,
	};

	enum
	{
		MAX_PAGE_LINE = 7,
	};

private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;
	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	int m_iCurPage;
	int m_iMaxPage;

	bool m_bTournamentOwner;
	DWORD m_dwTournamentIndex;
	byte m_TournamentType;

protected://토너먼트 정보
	BYTE         m_MaxPlayer;
	ioHashString m_szPlayModeName;
	DWORD		 m_dwOwnerIndex;
	DWORD		 m_TournamentMaxRound;
	DWORD		 m_TournamentStartTeamCount;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void SetTournamentData( DWORD dwTourIndex );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();


protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPageNumberRender( int iXPos, int iYPos );
	void OnIconRender( int iXPos, int iYPos );

protected:
	void OnRewardWndRender( int iXPos, int iYPos );

public:
	TournamentDetailInnerPageRewardWnd();
	virtual ~TournamentDetailInnerPageRewardWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailFrameCheerWnd : public TournamentDetailFrameWnd
{
public:
	enum
	{
		ID_INNER_CHEER = 10,
	};

public:
	virtual void SetTournamentData( DWORD dwTourIndex );
	virtual void UpdateMainUI();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	void ApplyDecision();

protected:
	void OnActiveTypeRender( int iXPos, int iYPos );
	void OnActiveRender( int iXPos, int iYPos );
	void OnInfoRender( int iXPos, int iYPos );

protected:
	virtual void OnRender();
	
public:
	TournamentDetailFrameCheerWnd();
	~TournamentDetailFrameCheerWnd();
};

//////////////////////////////////////////////////////////////////////////

class CheerListRadioBtn : public ioRadioButton
{
private:
	ioUIRenderImage *m_pCheckMark;
	ioUIRenderImage *m_pCheckMarkBack;
	ioUIRenderImage *m_pCheckDisable;

	DWORD  m_dwTeamIdx;
	ioHashString m_szTeamName;
	int m_iCheerPoint;
	int m_iCampPos;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnRender();

public:
	void SetCheerData( DWORD dwTeamIdx, const ioHashString& szTeamName, int iCampPos, int iCheerPoint );
	inline DWORD GetTeamIdx(){ return m_dwTeamIdx; }
	inline const ioHashString& GetTeamName(){ return m_szTeamName; }
	inline int GetCampPos(){ return m_iCampPos; }

public:
	CheerListRadioBtn();
	virtual ~CheerListRadioBtn();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailInnerCheerListWnd : public ioWnd
{
public:
	enum
	{
		ID_RADIO_BTN1 = 1,
		ID_RADIO_BTN2,
		ID_RADIO_BTN3,
		ID_RADIO_BTN4,
		ID_RADIO_BTN5,
		ID_RADIO_BTN6,
		ID_RADIO_BTN7,
		ID_RADIO_BTN8,
		ID_RADIO_BTN9,
		ID_RADIO_BTN10,
		ID_RADIO_BTN11,
		ID_RADIO_BTN12,
		ID_RADIO_BTN13,
		ID_RADIO_BTN14,
		ID_RADIO_BTN15,
		ID_RADIO_BTN16,
				
		ID_CHEER_DECISION_BTN   = 100,
		ID_VERT_SCROLL		    = 1000,
	};

	enum CHEERSTATE
	{
		CS_NONE,
		CS_DECISION,
		CS_NO_DECISION,
	};

private:
	ioUIRenderImage* m_pDarkLine;
	ioUIRenderImage* m_pLightLine;

	ioUIRenderImage* m_pBackCircle;
	ioUIRenderImage* m_pBackHub;
	ioUIRenderImage* m_pCheerIcon;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

protected:
	int m_iCurPage;
	int m_iMaxPage;

	DWORD m_dwTournamentIndex;
	DWORD m_dwAllocateSyncTimer;
	DWORD m_dwCheerTeamCnt;

	DWORD m_dwMyTeamIdx;

protected:
	CHEERSTATE m_CheerState;
	ioHashString m_szTourCheerTeamName;
	ioHashString m_szTourCheerPoint;
	
	int m_iCampPos;

public:
	virtual void iwm_create();
	virtual void iwm_show();

	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	
public:
	void SetCheerData( DWORD dwTourIndex );

protected:
	void CheckMyTeamPage();
	void ProcessMyTeamPage();

public:

	void NextPage();
	void PrevPage();
	void SetMaxPage();

	void SetPageParsingData();
	void SetDataSync();
		
	void SendTourTeamDataList();
	void SendCheerDecision();

	void ApplyDecision();

public:
	void CalcCheerPoint();
	void OnTeamDataSyncProcess();
	void OnCheckCheerState();

public:

	void SetChildActvieWnd( DWORD dwWndID );
	void SetRadioAllActive();
	void SetRadioSelectActive( DWORD dwTeamIdx );

public:
	CheerListRadioBtn* GetSelectRadio();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnListBackRender( int iXPos, int iYPos );
	void OnCheerIconRender( int iXPos, int iYPos );
	void OnMentRender( int iXPos, int iYPos );
	void OnCheerPointRender( int iXPos, int iYPos );
	void OnCheerRender( int iXPos, int iYPos );

public:
	TournamentDetailInnerCheerListWnd();
	virtual ~TournamentDetailInnerCheerListWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentCheerPopUpWnd : public ioWnd 
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_CANCEL = 2,
		ID_OK	  = 3,
	};

protected:
	ioWnd* m_pCallWnd;

	ioHashString m_szTeamName;
	BYTE m_CampPos;

public:	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();

public:
	void SetCheerDecision( const ioHashString& szTeamName, BYTE CampPos, ioWnd* pCallWnd );

protected:
	virtual void OnRender();

public:
	TournamentCheerPopUpWnd();
	virtual ~TournamentCheerPopUpWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentPredictPopUpWnd : public ioWnd 
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_CANCEL = 2,
		ID_OK	  = 3,
	};

protected:
	ioWnd* m_pCallWnd;

	ioHashString m_szTeamName;
	BYTE m_CampPos;

public:	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();

public:
	void SetPredictDecision( const ioHashString& szTeamName, BYTE CampPos, ioWnd* pCallWnd );

protected:
	virtual void OnRender();

public:
	TournamentPredictPopUpWnd();
	virtual ~TournamentPredictPopUpWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentPridictResultPopUp : public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN	 = 1,
		ID_CLOSE_BTN = 2,
	};

protected:
	ioUIRenderImage* m_pVictroyIcon;
	ioUIRenderImage* m_pVictoryBack;

	DWORD m_dwModeType;

public:	
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetModeType( DWORD dwModeType );

public:
	void OnVictoryRender( int iXPos, int iYPos );

protected:
	virtual void OnRender();

public:
	TournamentPridictResultPopUp();
	~TournamentPridictResultPopUp();
};

//////////////////////////////////////////////////////////////////////////

class ioUITitle;
class MatchBtn : public ioButton
{
public:
	enum LineState
	{
		LS_NONE,
		LS_UP,
		LS_DOWN,
	};	

protected:
	ioUIRenderFrame *m_pBlueNor;
	ioUIRenderFrame *m_pBlueOver;
	ioUIRenderFrame *m_pBluePush;

	ioUIRenderFrame *m_pRedNor;
	ioUIRenderFrame *m_pRedOver;
	ioUIRenderFrame *m_pRedPush;

	ioUIRenderFrame *m_pGreenNor;
	ioUIRenderFrame *m_pGreenOver;
	ioUIRenderFrame *m_pGreenPush;

	ioUIRenderFrame *m_pCurrentNor;
	ioUIRenderFrame *m_pCurrentPush;
	ioUIRenderFrame *m_pCurrentOver;
	
	ioUIRenderImage *m_pMatchLine;			//토너먼트 매치 라인	
	ioUIRenderImage *m_pMatchActiveLine;	//토너먼트 진출 라인

	ioUIRenderImage* m_pMyCheerTeam;
	
protected:
	LineState m_LineState;		

	DWORD m_dwMyPageRound;					//자신이 속한 페이지 내의 라운드
	int	  m_iTournamentMaxRound;			//최대 라운드 수
	int   m_iTournamentCurHorzPage;			//스케쥴표 내의 현재 페이지	
	byte  m_TournamentState;

	int m_iUpHeight;
	int m_iUpYPosition;
	
	RoundTeamData m_RoundTeamData;
	bool m_bNextRound;
	bool m_bCheerTeam;
	
private:
	DWORD m_dwOrgTextColor;

public:
	virtual void iwm_create();

public:
	void SetMatchLine( ioUIRenderImage* pMatchLine, ioUIRenderImage* pMatchAciveLine, int iUpHeight, int iUpYPosition );
	void SetMatchData( DWORD dwTourIndex, int iTournamentCurHorzPage, DWORD dwMyPageRound, SHORT Position );					
	void CheckNextRoundGo( DWORD dwTournamentIndex, DWORD dwCurRound, const RoundTeamData& rkTeamData, SHORT Position );
	void CheckNextPageGo( DWORD dwTournamentIndex, DWORD dwCurRound, const RoundTeamData& rkTeamData,  SHORT Position );

	void ChangeStyle( DWORD dwTournamentIndex, DWORD dwCurRound, const RoundTeamData& rkTeamData, SHORT Position );
	void ChangeMyTeamStyle( BYTE TourType, const RoundTeamData& rkTeamData );
	void ChangeOtherTeamStyle( BYTE TourType, const RoundTeamData& rkTeamData );
	void ChangeEmptyTeamStyle( DWORD dwCurRound, SHORT Position );
	void SetActiveStyle( ioUIRenderFrame* pNor, ioUIRenderFrame* pPush, ioUIRenderFrame* pOver, ioHashString szTtitle, DWORD dwTextColor, DWORD dwBkColor );
	void SetTitle( ioHashString szTtitle, DWORD dwTextColor, DWORD dwBkColor, TextStyle eStyle, int iWidthCut = 0, int iOffsetX = 0, int iOffsetY = 0 );

public:
	inline DWORD GetTeamIndex(){ return m_RoundTeamData.m_dwTeamIndex; }
	inline RoundTeamData GetRoundTeamData(){ return m_RoundTeamData; }
	
protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnRender();

protected:
	void onMatchLineRender( int iXPos, int iYPos );

	void onMatchActiveUpLineRender( int iXPos, int iYPos );
	void onMatchActiveDownLineRender( int iXPos, int iYPos );
	
	void onMatchNextPageLineRender( int iXPos, int iYPos );
	void onMatchNextPageActiveLineRender( int iXPos, int iYPos );
	void onMatchPrevPageLineRender( int iXPos, int iYPos );
	void onMatchPrevPageActiveLineRender( int iXPos, int iYPos );
	
public:
	MatchBtn();
	virtual ~MatchBtn();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailScheduleFrameWnd : public TournamentDetailFrameWnd
{
public:

	enum
	{
		//16개
		ID_CHILD_BTN000001 = 100000,
		ID_CHILD_BTN000002 = 100001,
		ID_CHILD_BTN000003 = 100002,
		ID_CHILD_BTN000004 = 100003,
		ID_CHILD_BTN000005 = 100004,
		ID_CHILD_BTN000006 = 100005,
		ID_CHILD_BTN000007 = 100006,
		ID_CHILD_BTN000008 = 100007,
		ID_CHILD_BTN000009 = 100008,
		ID_CHILD_BTN000010 = 100009,
		ID_CHILD_BTN000011 = 100010,
		ID_CHILD_BTN000012 = 100011,
		ID_CHILD_BTN000013 = 100012,
		ID_CHILD_BTN000014 = 100013,
		ID_CHILD_BTN000015 = 100014,
		ID_CHILD_BTN000016 = 100015,

		//8개		
		ID_CHILD_BTN00001  = 10000,
		ID_CHILD_BTN00002  = 10001,
		ID_CHILD_BTN00003  = 10002,
		ID_CHILD_BTN00004  = 10003,
		ID_CHILD_BTN00005  = 10004,
		ID_CHILD_BTN00006  = 10005,
		ID_CHILD_BTN00007  = 10006,
		ID_CHILD_BTN00008  = 10007,

		//4개
		ID_CHILD_BTN0001   = 1000,
		ID_CHILD_BTN0002   = 1001,
		ID_CHILD_BTN0003   = 1002,
		ID_CHILD_BTN0004   = 1003,
		
		//2개
		ID_CHILD_BTN001    = 100,
		ID_CHILD_BTN002    = 101,

		//1개		
		ID_CHILD_BTN01     = 10,

		ID_PREV_BTN       = 1,
		ID_NEXT_BTN		  = 2,
		ID_VERT_SCROLL    = 3,
	};

private:

	struct LineInfo
	{
		int m_iXPos;
		int m_iYPos;

		int m_iWidth;
		int m_iHeight;
	};

	struct MatchLineInfo
	{
		int m_iRound;
		int m_iCnt;
		int m_iMatchRound;
		int m_iXOffset;
		int m_iYOffset;

		int m_iUpBtnID;
		int m_iDownBtnID;

		LineInfo m_UpNext;
		LineInfo m_UpLink;
		LineInfo m_Center;
		LineInfo m_DownNext;
		LineInfo m_DownLink;
	};

	typedef std::vector<MatchLineInfo> MatchLineList;
	MatchLineList m_vMatchLineList;

	//상단 메뉴
	ioUIRenderImage *m_pFirstPartActiveLeft;
	ioUIRenderImage *m_pFirstPartActiveCenter;
	ioUIRenderImage *m_pFirstPartActiveRight;
		
	ioUIRenderImage *m_pRestPartActiveCenter;
	ioUIRenderImage *m_pRestPartActiveRight;

	ioUIRenderImage *m_pFirstPartInctiveLeft;
	ioUIRenderImage *m_pFirstPartInctiveCenter;
	ioUIRenderImage *m_pFirstPartInctiveRight;

	ioUIRenderImage *m_pRestPartInctiveCenter;
	ioUIRenderImage *m_pRestPartInctiveRight;
	
	ioUIRenderImage *m_pMatchLine;			//토너먼트 매치 라인
	ioUIRenderImage *m_pMatchLineActive;	//토너먼트 진출 라인
		
	ioUIRenderImage *m_pChampion;
	ioUIRenderImage *m_pChampionBack;
	
	int m_iTournamentMaxRound;
	int m_iTournamentStartTeamCount;
		
	int m_iTournamentMaxHorzPage;
	int m_iTournamentCurHorzPage; //현재 선택 라운드
	
	int m_iTournamentMaxVertPage;
	int m_iTournamentCurVertPage;
	
	// 동기화 체크 타이머
	DWORD m_dwRoundTeamDataSyncTimer;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void iwm_prevmenu();

public:
	void round_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRoundDataSyncProcess();

protected:	
	void SetVertMaxPage();
	void SetParsingUserList();
	void SetRound();

	void NextSlide();
	void PrevSlide();

	void OnActiveTypeRender( int iXPos, int iYPos );
	void OnPartyRender( int iXPos, int iYPos );
	
	void OnRoundTextRender( int iXPos, int iYPos );
	void OnChampionRender( int iXPos, int iYPos );

public:
	TournamentDetailScheduleFrameWnd();
	virtual ~TournamentDetailScheduleFrameWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailFrameAwardWnd : public TournamentDetailFrameWnd
{
public:
	enum
	{
		ID_INNER_TOURNAMENT_REAWARD = 10,
		ID_INNER_CAMP_REAWARD		= 11,
		ID_INNER_CHEER_REAWARD		= 12,
		ID_INNER_CUSTOM_REAWARD		= 13,
	};

protected:
	BYTE m_TournamentType;

public:
	virtual void iwm_create();
	virtual void iwm_show();

public:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void SetTournamentData( DWORD dwTourIndex );
	virtual void UpdateMainUI();

protected:
	void OnActiveTypeRender( int iXPos, int iYPos );
	void OnActiveRender( int iXPos, int iYPos );
	void OnRegularMentRender( int iXPos, int iYPos );
	void OnCusotmMentRender( int iXPos, int iYPos );
	void OnRewardRender( int iXPos, int iYPos );	

public:
	TournamentDetailFrameAwardWnd();
	~TournamentDetailFrameAwardWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentDetailFrameAllocateWnd : public TournamentDetailFrameWnd
{
public:
	enum
	{
		ID_INNER_WND = 10,
	};

protected:
	BYTE m_TournamentType;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void SetTournamentData( DWORD dwTourIndex );
	virtual void UpdateMainUI();

protected:
	void OnActiveTypeRender( int iXPos, int iYPos );
	void OnActiveRender( int iXPos, int iYPos );
	void OnMentRender( int iXPos, int iYPos );
	void OnCusotmMentRender( int iXPos, int iYPos );
	void OnAllocateRender( int iXPos, int iYPos );	

public:
	TournamentDetailFrameAllocateWnd();
	~TournamentDetailFrameAllocateWnd();
};

//////////////////////////////////////////////////////////////////////////

//보상관련
typedef std::vector<ioUIRenderImage*> RewardIconList;
typedef std::vector<ioHashString> DetailRewardTextList;

//////////////////////////////////////////////////////////////////////////

class TournamentDetailInnerRewardWnd : public ioWnd
{
private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentNone;
	ioUIRenderImage *m_pBackCircle;
	ioUIRenderImage *m_pBackHub;
	ioUIRenderImage *m_pFinishWin;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	float m_fPresentOffsetX;
	float m_fPresentScaleRate;
	DWORD m_dwPresentBackCnt;

protected:
	DWORD m_dwTournamentIndex;
	DWORD m_dwTournamentMaxRound;
	DWORD m_TournamentPos;
	DWORD m_TournamentTeamPos;
	
	DWORD m_dwCampPos;
	byte m_TournamentState;

protected:
	ioHashString m_szResultTtitle;
	ioHashStringVec m_vReulstTextList;
	ResultIconList m_vIconNameList;
	RewardIconList m_vIconList;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetTournamentData( DWORD dwTourIndex );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPresentRender( int iXPos, int iYPos, bool bReward = true );	
	void OnBackRender( int iXPos, int iYPos,  bool bReward = true );
	void OnRewardIconRender( int iXPos, int iYPos );

protected:
	void OnTournamentRewardNoneRender( int iXPos, int iYPos );
	void OnTournamentRewardResultRender( int iXPos, int iYPos );
	void OnTournamentNoneRender( int iXPos, int iYPos );

public:
	TournamentDetailInnerRewardWnd();
	virtual ~TournamentDetailInnerRewardWnd();
};
//////////////////////////////////////////////////////////////////////////

class TournamentDetailInnerCampRewardWnd : public ioWnd
{
public:
	enum
	{
		BLUE_CAMP_WIN  = 1,
		BLUE_CAMP_LOSE = 2,
		RED_CAMP_WIN   = 3,
		RED_CAMP_LOSE  = 4,
		CAMP_DRAW      = 5,
	};

private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentNone;
	ioUIRenderImage *m_pBackCircle;	
	ioUIRenderImage *m_pBackHub;
	ioUIRenderImage *m_pCampIcon;
	ioUIRenderImage *m_pCampNoneIcon;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	float m_fPresentOffsetX;
	float m_fPresentScaleRate;
	DWORD m_dwPresentBackCnt;

protected:
	DWORD m_dwChampCampPos;
	DWORD m_dwCampPos;
	DWORD m_dwCampResult;

	byte m_TournamentState;

protected:
	ioHashString m_szResultTtitle;
	ioHashStringVec m_vReulstTextList;
	ResultIconList m_vIconList;	

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetTournamentData( DWORD dwTourIndex );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	
protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPresentRender( int iXPos, int iYPos, bool bReward = true );
	void OnBackRender( int iXPos, int iYPos,  bool bReward = true );
	void OnCampIconRender( int iXPos, int iYPos );

protected:
	void OnCampRewardNoneRender( int iXPos, int iYPos );
	void OnCampRewardResultRender( int iXPos, int iYPos );
	void OnCampNoneRender( int iXPos, int iYPos );

public:
	TournamentDetailInnerCampRewardWnd();
	virtual ~TournamentDetailInnerCampRewardWnd();
};
//////////////////////////////////////////////////////////////////////////

class TournamentDetailInnerCheerWnd : public ioWnd
{
private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentNone;
	ioUIRenderImage *m_pPresentPeso;
	ioUIRenderImage *m_pBackCircle;
	ioUIRenderImage *m_pBackHub;
	ioUIRenderImage *m_pCheerReward;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	float m_fPresentOffsetX;
	float m_fPresentScaleRate;
	DWORD m_dwPresentBackCnt;

protected:
	DWORD m_dwTournamentIndex;

	DWORD m_dwCampPos;
	DWORD m_dwCheerPeso;
	byte m_TournamentState;
	bool m_bChampionTeam;
	bool m_bCheer;

protected:
	ioHashString m_szTeamName;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetCheerData( DWORD dwTourIndex );

protected:
	void CalcCheerPoint();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	
protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPresentRender( int iXPos, int iYPos, bool bReward = true );
	void OnBackRender( int iXPos, int iYPos,  bool bReward = true );
	void OnCheerIconRender( int iXPos, int iYPos );

protected:
	void OnCheerRewardNoneRender( int iXPos, int iYPos );
	void OnCheerRewardResultRender( int iXPos, int iYPos );
	void OnCheerNoneRender( int iXPos, int iYPos );

public:
	TournamentDetailInnerCheerWnd();
	virtual ~TournamentDetailInnerCheerWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentAlarmMainRewardWnd : public ioWnd
{
protected:
	ioUIRenderImage* m_pDevideDot;
	
	float m_fDevideOffsetX;
	float m_fDevideOffsetY;
	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

public:
	enum
	{
		ID_TOURNAMENT_REAWARD = 10,
		ID_CAMP_REAWARD		  = 11,
		ID_CHEER_REAWARD	  = 12,
	};

	enum
	{
		ID_CLOSE	 = 1,
		ID_CONFIRM	 = 2,
	};


protected:
	ioHashString m_szTeamName;

	BYTE m_AttainTourPos;
	DWORD m_dwStartDate;
	int m_iMyCampPos;
	int m_iWinCampPos;
	int m_iLadderBonusPeso;

	// 추가
	int m_iMyLadderRank;
	int m_iMyLadderPoint;

	DWORD m_dwCheerPeso;

public:
	void SetRewardInfo( BYTE AttainTourPos,
						DWORD dwStartDate,
						int iMyCampPos,
						int iWinCampPos,
						int iLadderBonusPeso,
						int iMyLadderRank,
						int iMyLadderPoint,
						DWORD dwCheerPeso,
						const ioHashString& szTeamName );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void OnRender();

public:
	void OnDevideWndDotRender( int iXPos, int iYPos );

public:
	int GetMyCampType(){ return m_iMyCampPos; }
	int GetWinCampType(){ return m_iWinCampPos; }
	int GetSeasonBonus(){ return m_iLadderBonusPeso; }
	int GetMyCampRank(){ return m_iMyLadderRank; }
	int GetMyCampPoint(){ return m_iMyLadderPoint; }

public:
	TournamentAlarmMainRewardWnd();
	~TournamentAlarmMainRewardWnd();
};

//////////////////////////////////////////////////////////////////////////

typedef std::map<int, ioUIRenderImage*> ResultRewardIconMap;
struct ResultData
{
	ioHashString m_szTtitle;
	ioHashStringVec m_szResultList;
	ResultIconList m_vIconList;
};
typedef std::map<int, ResultData> ResultDataMap;

//////////////////////////////////////////////////////////////////////////

class TournamentAlarmRewardWnd : public ioWnd
{
private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentNone;
	ioUIRenderImage *m_pBackCircle;
	ioUIRenderImage *m_pBackHub;
	ioUIRenderImage *m_pFinishWin;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	float m_fPresentOffsetX;
	float m_fPresentScaleRate;
	DWORD m_dwPresentBackCnt;

protected:
	DWORD m_dwStartDate;
	DWORD m_dwCampPos;
	BYTE m_AttainTourPos;
	ResultDataMap m_ResultDataMap;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void CreateRewardData( DWORD iReguralDateType );	
	void SetTournamentData( DWORD dwStartDate, BYTE AttainTourPos, int iMyCampPos );

protected:
	ioHashString& GetResultTitle( int iRound );
	ioHashStringVec& GetResultTextList( int iRound );
	ResultIconList& GetResultIconList( int iRound );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();


protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPresentRender( int iXPos, int iYPos, bool bReward = true );	
	void OnBackRender( int iXPos, int iYPos, bool bReward = true );
	void OnRewardIconRender( int iXPos, int iYPos );

protected:
	void OnResultRender( int iXPos, int iYPos );
	void OnResultNoneRender( int iXPos, int iYPos );

public:
	TournamentAlarmRewardWnd();
	virtual ~TournamentAlarmRewardWnd();
};
//////////////////////////////////////////////////////////////////////////

class TournamentAlarmCampRewardWnd : public ioWnd
{
public:
	enum
	{
		BLUE_CAMP_WIN  = 1,
		BLUE_CAMP_LOSE = 2,
		RED_CAMP_WIN   = 3,
		RED_CAMP_LOSE  = 4,
		CAMP_DRAW      = 5,
	};

private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentNone;
	ioUIRenderImage *m_pBackCircle;
	ioUIRenderImage *m_pBackHub;
	ioUIRenderImage *m_pCampRedIcon;
	ioUIRenderImage *m_pCampBlueIcon;
	ioUIRenderImage *m_pCampNoneIcon;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	float m_fPresentOffsetX;
	float m_fPresentScaleRate;
	DWORD m_dwPresentBackCnt;

protected:
	int m_iLadderBonusPeso;

	DWORD m_dwStartDate;
	DWORD m_dwCampPos;
	DWORD m_dwCampResult;
	ResultDataMap m_ResultDataMap;

public:
	void CreateRewardData( DWORD iReguralDateType );
	void SetTournamentData( DWORD dwStartDate, BYTE AttainTourPos, int iMyCampPos );

protected:
	ioHashString& GetResultTitle( int iRound );
	ioHashStringVec& GetResultTextList( int iRound );
	ResultIconList& GetResultIconList( int iRound );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetTournamentData( DWORD dwTourIndex, DWORD dwStartDate, int iMyCampPos, int iWinCampPos, int iLadderBonusPeso );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPresentRender( int iXPos, int iYPos, bool bReward = true );
	void OnBackRender( int iXPos, int iYPos, bool bReward =true );
	void OnCampIconRender( int iXPos, int iYPos );

protected:
	void OnResultRender( int iXPos, int iYPos );
	void OnResultNoneRender( int iXPos, int iYPos );

public:
	TournamentAlarmCampRewardWnd();	
	virtual ~TournamentAlarmCampRewardWnd();
};
//////////////////////////////////////////////////////////////////////////

class TournamentAlarmCheerRewardWnd : public ioWnd
{
private:
	ioUIRenderImage *m_pDevideDot;
	ioUIRenderImage *m_pPresentBack;
	ioUIRenderImage *m_pPresentNone;
	ioUIRenderImage *m_pPresentPeso;
	ioUIRenderImage *m_pBackCircle;
	ioUIRenderImage *m_pBackHub;
	ioUIRenderImage *m_pCheerReward;

	float m_fDevideOffsetX;
	float m_fDevideOffsetY;

	DWORD m_dwDevideVertCnt;
	DWORD m_dwDevideHorzCnt;

	float m_fPresentOffsetX;
	float m_fPresentScaleRate;
	DWORD m_dwPresentBackCnt;

protected:
	DWORD m_dwCampPos;
	DWORD m_dwCheerPeso;

	BYTE m_AttainTourPos;
	ioHashString m_szWinnerTeamName;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetCheerReward( BYTE AttainTourPos, DWORD dwCampPos, DWORD dwCheerPeso, const ioHashString& szWinnerTeamName );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnDevideDotRender( int iXPos, int iYPos );
	void OnPresentRender( int iXPos, int iYPos, bool bReward =true );
	void OnBackRender( int iXPos, int iYPos, bool bReward =true );
	void OnCheerIconRender( int iXPos, int iYPos );

protected:
	void OnResultRender( int iXPos, int iYPos );
	void OnResultNoneRender( int iXPos, int iYPos );

public:
	TournamentAlarmCheerRewardWnd();
	virtual ~TournamentAlarmCheerRewardWnd();
};

//////////////////////////////////////////////////////////////////////////
class TournamentRoomListBtn : public ioButton
{
protected:
	DWORD m_dwBattleIndex;

protected:
	BYTE m_BlueCamp;
	BYTE m_RedCamp;
	ioHashString m_BlueName;
	ioHashString m_RedName;

public:
	void SetRoomValue( DWORD dwBattleIndex, const ioHashString &rkBlueName, BYTE BlueCamp, const ioHashString &rkRedName, BYTE RedCamp );

public:
	DWORD GetBattleIndex(){ return m_dwBattleIndex; }

protected:
	virtual void OnRender();

public:
	TournamentRoomListBtn();
	virtual ~TournamentRoomListBtn();
};

//////////////////////////////////////////////////////////////////////////

class TournamentRoomListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_LEFT = 2,
		ID_RIGHT= 3,

		ID_ROOM_1  = 11,
		ID_ROOM_2  = 12,
		ID_ROOM_3  = 13,
		ID_ROOM_4  = 14,
		ID_ROOM_5  = 15,
		ID_ROOM_6  = 16,
		ID_ROOM_7  = 17,
		ID_ROOM_8  = 18,
		ID_ROOM_9  = 19,
		ID_ROOM_10 = 20,
		ID_ROOM_11 = 21,
		ID_ROOM_12 = 22,
		ID_ROOM_13 = 23,
		ID_ROOM_14 = 24,
		ID_ROOM_15 = 25,
		ID_ROOM_16 = 26,
	};

	enum
	{
		MAX_ROOM_LIST		= 16,
		ROOM_SEARCH_TIME	= 20000,		
	};

protected:
	DWORD m_dwSearchTime;

protected:
	DWORD m_dwTourIndex;

protected:
	int m_iCurPage;
	int m_iMaxPage;

protected:
	void ProcessSearch();
	void UpdateSetPage( int iCurPage );
	void StartSend( bool bProcess = false );

public:
	void SetTournament( DWORD dwTourIndex );
	void SetRoomList( SP2Packet &rkPacket );

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );	

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	TournamentRoomListWnd();
	virtual ~TournamentRoomListWnd();
};