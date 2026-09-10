#pragma once

#include "../io3DEngine/ioMovingWnd.h"
#include "../CharacterInfo.h"
#include "../TournamentNode.h"
#include "../ioComplexStringPrinter.h"
#include "../io3DEngine/ioStringConverter.h"
#include "ioPullDownItem.h"
#include "ioPullDownEventListener.h"

#include "GlobalWnd.h"

struct BannerSet
{
	int m_Index;
	ioHashString m_szBannerName;
	ioUIRenderImage* m_pBannerMark;
	ioUIRenderImage* m_pBanner;

	BannerSet()
	{
		m_Index = -1;
		m_pBannerMark = NULL;
		m_pBanner = NULL;
	}
};
typedef std::vector<BannerSet> BannerSetList;

struct TournamentMatchType
{
	ioHashString m_szMenuName;
	int m_iTournamentMatchType;
	int m_iHelpIndex;
	int m_iManualIndex;
	int m_iMinUser;
	int m_iMaxUser;
	int m_iDefaultUser;

	TournamentMatchType()
	{
		m_iTournamentMatchType = 0;
		m_iHelpIndex = -2;
		m_iManualIndex = -2;
		m_iMinUser = 2;
		m_iMaxUser = 4;
		m_iDefaultUser = 0;
	}
};

struct TournamentScale
{
	int m_Index;
	ioHashString m_szMenuName;
	short m_iTournamentRound;

	TournamentScale()
	{
		m_Index = 0;
		m_iTournamentRound = 0;
	}
};

struct TournamentProgressType
{
	ioHashString m_szMenuName;
	ioHashString m_szSubMenuName;
	byte m_Type;
	int m_iHelpIndex;
	int m_iManualIndex;

	TournamentProgressType()
	{
		m_Type = 0;
		m_iHelpIndex = -2;
		m_iManualIndex = -2;
	}
};

struct ScheduleSet
{
	ioHashString m_szMenuName;
	int m_iPos;

	ioHashString m_szTitle;
	ioHashStringVec m_szMentList;
	ioCalendarSet m_CalendarSet;
	
	ScheduleSet()
	{
		m_iPos = 0;
		m_CalendarSet.SetClear();
	}
};
typedef std::vector<ScheduleSet> ScheduleSetVec;

//////////////////////////////////////////////////////////////////////////

class CustomTournamentListBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pSelectBack;
	ioUIRenderImage *m_pEmptyBack;
	ioUIRenderImage *m_pBannerMark;

	bool m_bDisable;
	bool m_bSelect;
	bool m_bActive;

	DWORD m_dwIndex;

public:
	void SetBannerMark( ioUIRenderImage* pBannerMark, DWORD dwIndex );
	void SetSelect( bool bSelect, bool bDisable );

public:
	inline DWORD GetIndex(){ return m_dwIndex; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	CustomTournamentListBtn();
	virtual ~CustomTournamentListBtn();
};


//////////////////////////////////////////////////////////////////////////

class TournamentTypeSinglePullDownItem : public SinglePullDownItem
{
public:
	int m_iHelpIndex;
	int	m_iManualIndex;
	int m_iMatchType;

	ioUIRenderImage* m_pMarkReg;

	TournamentTypeSinglePullDownItem()
	{
		m_pMarkReg		= NULL;
		m_iHelpIndex	= 0;
		m_iManualIndex	= 0;
		m_iMatchType	= 0;
	}
	virtual ~TournamentTypeSinglePullDownItem(){}
};

//////////////////////////////////////////////////////////////////////////

class TournamentTypeDoublePullDownItem : public DoublePullDownItem
{
public:
	int m_iHelpIndex;
	int	m_iManualIndex;

	ioUIRenderImage* m_pMarkReg;

	TournamentTypeDoublePullDownItem()
	{
		m_pMarkReg		= NULL;
		m_iHelpIndex	= 0;
		m_iManualIndex	= 0;
	}
	virtual ~TournamentTypeDoublePullDownItem(){}
};


//////////////////////////////////////////////////////////////////////////

class CustomTournamentCreateWnd : public ioMovingWnd, public PullDownEventListener
{
public:
	enum
	{	
		ID_MODE_HELP_TOOLTIP = 500,
	};

	enum
	{		
		ID_CLOSE_BTN				  = 1,
		ID_HELP_BTN					  = 2,

		ID_VIEW_WND					  = 10,
		ID_SCHEDULE_WND				  = 11,

		ID_TOURNAMENT_EDIT			  = 100,
		ID_BANNER_BTN				  = 101,
		ID_BANNER_LIST				  = 102,
		ID_BANNERLOAD_BTN			  = 103,

		ID_BANNERMARK_BTN			  = 104,
		ID_BANNERMARK_LIST			  = 105,
		ID_BANNERMARKLOAD_BTN		  = 106,

		ID_TOURNAMENT_SCALE_BTN		  = 107,
		ID_TOURNAMENT_SCALE_LIST	  = 108,

		ID_TOURNAMENT_TYPE_BTN		  = 109,
		ID_TOURNAMENT_TYPE_LIST		  = 110,

		ID_TOURNAMENT_USERNUMBER_BTN  = 111,
		ID_TOURNAMENT_USERNUMBER_LIST = 112,

		ID_TOURNAMENT_PROGRESS_BTN	  = 113,
		ID_TOURNAMENT_PROGRESS_LIST	  = 114,

		ID_REGULRATION_CHECK		  = 200,
		ID_REGULRATION_BTN			  = 300,
		ID_CREATE_BTN				  = 400,
	};

	enum
	{
		MAX_BANNER	 = 12,
		MAX_BACKLINE = 13,
	};

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pManualMark;

protected:
	ioUIRenderImage *m_pBanner;
	ioUIRenderImage* m_pDevideDot;

protected:
	int m_iDefaultBannerMarkIndex;
	int m_iDefaultBannerMainIndex;
	int m_iDefaultTournamentScaleIndex;
	int m_iDefaultTournamentTypeIndex;
	int m_iDefaultTournamentProgressIndex;

protected:
	typedef std::vector<TournamentMatchType> TournamentMatchTypeList;
	typedef std::vector<int> UserNumberList;
	typedef std::vector<TournamentScale> TournamentScaleList;
	typedef std::vector<TournamentProgressType> TournamentProgressTypeList;

	BannerSetList m_vCustomBannerList;
	TournamentScaleList m_vTournamentScaleList;
	TournamentMatchTypeList m_vMatchTypeList;
	TournamentProgressTypeList m_vTournamentProgressTypeList;

protected:
	ioHashString m_szTournamentName;
	BannerSet m_CurBannerMark;
	BannerSet m_CurBanner;
	TournamentScale m_CurTournamentScale;
	TournamentMatchType m_CurTournamentType;
	byte m_CurTournamentUserNumber;
	TournamentProgressType m_CurTournamentProgressType;
	ScheduleSetVec m_CurTournamentScheduleVec;

protected:
	DWORD m_dwTournamentHelpID;
	DWORD m_dwEtcItemType;

protected:
	ioWnd *m_pModeHelpTooltip;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_pulldown_event( const PullDownEvent& Event );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	BannerSet& FindBannerSet( int Index );
	TournamentScale& FindTournamentScale( int Index );
	TournamentMatchType& FindTournamentType( int iMatchType );
	TournamentProgressType& FindTournamentProgressType( int iType );

protected:
	void CreateCustomData();

protected:
	void CreateBannerMarkList();
	void CreateBannerList();
	void CreateTournamentScaleList();
	void CreateTournamentTypeList();
	void CreateTournamentUserNumberList();
	void CreateTournamentProgressList();
	
protected:
	void OpenBannerMarkList( ioWnd *pOpenWnd );
	void OpenBannerList( ioWnd *pOpenWnd );
	void OpenTournamentScaleList( ioWnd *pOpenWnd );
	void OpenTournamentTypeList( ioWnd *pOpenWnd );
	void OpenTournamentUserNumberList( ioWnd *pOpenWnd );
	void OpenTournamentProgressList( ioWnd *pOpenWnd );

protected:
	void SetBannerMark(int Index);
	void SetBanner(int Index);
	void SetTournamentScale(int Index);
	void SetTournamentType(int Index);
	void SetTournamentUserNumber( int iNumber = 0 );
	void SetTournamentProgress(int Index);
	void SetTournamentSchedule();
	void SetDateTournamentView();

protected:
	bool CheckEnableTournament();
	void TournamentStartMessage();
	void CheckActiveCreate();
	void SendCreateTournament();

protected:
	bool IsOnlyEngHanNumText( const char *szText ); //영문, 한글, 숫자만 가능
	void InitEditWnd();
	void KillKeyFoucs();

public:
	void UseEtcItem( DWORD dwEtcItemType );

public:
	virtual void OnProcess( float fTimePerSec );

	void OnTooltipEvent( int iManualIndex );
	void OnModeHelpToolTip( ioWnd* pCallWnd, const ioHashString& szTtitle, int iHelpIdx );

protected:
	virtual void OnRender();

	void OnBackLineRender( int iXPos, int iYPos );
	void OnRegulationRender( int iXPos, int iYPos );
	void OnDevideWndDotRender( int iXPos, int iYPos );

public:
	CustomTournamentCreateWnd();
	~CustomTournamentCreateWnd();
};

//////////////////////////////////////////////////////////////////////////

class CustomTournamentViewWnd : public ioWnd
{
public:
	enum
	{
		ID_BANNER_MARK_BTN1	 = 1,
		ID_BANNER_MARK_BTN2  = 2,
		ID_BANNER_MARK_BTN3  = 3,
		ID_BANNER_MARK_BTN4  = 4,
		ID_BANNER_MARK_BTN5  = 5,
		ID_BANNER_MARK_BTN6  = 6,
		ID_BANNER_MARK_BTN7  = 7,
		ID_BANNER_MARK_BTN8  = 8,
		ID_BANNER_MARK_BTN9  = 9,
		ID_BANNER_MARK_BTN10 = 10,
		ID_BANNER_MARK_BTN11 = 11,
		ID_BANNER_MARK_BTN12 = 12,
	};

	enum
	{
		MAX_BANNER	 = 12,
		MAX_BACKLINE = 13,
	};

protected:
	ioUIRenderFrame *m_pMainBannerFrm;
	ioUIRenderFrame *m_pScheduleOpenGray;
	ioUIRenderFrame *m_pScheduleOpenOrange;
	ioUIRenderFrame *m_pScheduleCloseGray;
	ioUIRenderFrame *m_pScheduleCloseOrange;

protected:
	ioUIRenderImage *m_pHorzLine;
	ioUIRenderImage *m_pVertLine;
	ioUIRenderImage *m_pTextBack;
	ioUIRenderImage *m_pTodayMark;
	ioUIRenderImage *m_pExtraExplainBack;

protected:
	ioUIRenderImage *m_pBanner;


protected:
	BannerSetList m_vCustomBannerList;

protected:
	ioHashString m_szTournamentName;
	ioHashString m_szTournamentScale;
	ioHashString m_szTournamentType;

	DWORD m_dwManualTournament;
	ioCalendarSet m_TournamentDate;
	int m_iUserNumber;

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	void SetSelectBanner( BannerSet& rkBanner );
	void SetSelectBannerMark( BannerSet& rkBanner );

	void SetTournamentName( ioHashString& szName );
	void SetTournamentScale( ioHashString& szScale );
	void SetTournamentType( ioHashString& szType, int iNumber );
	void SetTournamentProgress( DWORD dwManualTournament );
	void SetTournamentDate( ioCalendarSet& Date );

protected:
	virtual void OnRender();

	void OnMainBannerRender( int iXPos, int iYPos );
	void OnExplainRender( int iXPos, int iYPos );
	void OnExtraExplainRender( int iXPos, int iYPos );
	void OnScheduleRender( int iXPos, int iYPos );

public:
	CustomTournamentViewWnd();
	~CustomTournamentViewWnd();
};

//////////////////////////////////////////////////////////////////////////

class CustomTournamentScheduleBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pActiveIcon;
	ioUIRenderImage *m_pInActiveIcon;

	int m_iDefaultXPos;
	int m_iDefaultYPos;
	int m_iTourPos;

protected:
	virtual void iwm_create();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	void SetOffset( int iXOffset, int iYOffset );
	void SetScheduleSetPos( int iScheduleSetPos );

public:
	void SetTourPos( int iTourPos ){ m_iTourPos = iTourPos; }
	inline int CustomTournamentScheduleBtn::GetTourPos(){ return m_iTourPos; }

public:
	CustomTournamentScheduleBtn();
	virtual ~CustomTournamentScheduleBtn();
};

//////////////////////////////////////////////////////////////////////////

class CustomTournamentScheduleWnd : public ioWnd
{
public:
	enum
	{
		ID_SCHEDULE_BTN1 = 1,
		ID_SCHEDULE_BTN2 = 2,
		ID_SCHEDULE_BTN3 = 3,
		ID_SCHEDULE_BTN4 = 4,
		ID_SCHEDULE_BTN5 = 5,
		ID_SCHEDULE_BTN6 = 6,
		ID_SCHEDULE_BTN7 = 7,

		ID_VERT_SCROLL	 = 100,
	};

	enum
	{
		MAX_SCHEDULE = 7,
	};

	enum
	{
		TOURNAMENT_WEEK  = 2,
	};
		
protected:
	ScheduleSetVec m_vScheduleSet;

protected:
	int m_iScheduleGapTime;
	int m_iCurPos;
	int m_iTournamentMaxRound;
	BYTE m_TournamentProgressType;

	int m_iMatchStartHour;
	int m_iMatchEndHour;

	int m_iTournamentPeriod;
	
protected:
	ioCalendarSet m_StartCalendar;
	int m_iPeriodEnableTime;

	int m_iModifyReservationParam;
	ioCalendarSet m_ModifyReservationCalendarSet;

public:
	virtual void iwm_create();
	virtual void iwm_show();	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );

protected:
	void CreateCustomData();
	void ChangeTimeText( int Number, ioHashString& szNumber );
	void ChangeDateText( const ioCalendarSet& rkDate, ioHashString& szDestDate );

public:
	void InitScroll();
	void InitButton();

public:
	ScheduleSet& FindScheduleSet( int iPos );

public:
	void SelectScheduleSetAfterClear( int iStartPos );
	void SelectScheduleSetBeforeClear( int iEndPos );

public:
	void SetTournamentSchedule( int iTournamentRound );
	void SetTournamentProgress( BYTE ProgressType );

protected:
	void OpenTournamentCalendar( int iPos );
	void SetTournamentCalendar( ioCalendarSet& kSelectDateSet, ScheduleSet& kPrevSet, ScheduleSet& kCurSet );
	bool CheckNextSchedule( ScheduleSet& kNextSet, ioCalendarSet& kSelectDateSet, DWORD param );
	void SetResetMessage( int iPos );

public:
	virtual void SetActive();
	virtual void SetInActive();

public:
	void CheckSequenceInActive();

public:
	inline const ioCalendarSet& GetTournamentStartDate() const { return m_StartCalendar; }
	ScheduleSetVec& GetTounamentScheduleSetVec();

public:
	const ioCalendarSet& GetTournamentStart();
	const ioCalendarSet& GetTournamentEnd();

	int GetTournamentStartRemainTime();
	
protected:
	virtual void OnRender();

protected:
	void OnScheduleRender( int iXPos, int iYPos );

public:
	CustomTournamentScheduleWnd();
	~CustomTournamentScheduleWnd();
};

//////////////////////////////////////////////////////////////////////////
class CustomTournamentCreateResultWnd : public ioWnd 
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_CLOSE = 2,
		ID_INFO	 = 3,
	};

protected:
	ioUIRenderImage *m_pMarkBack;
	ioUIRenderImage *m_pMark;

	ioHashString m_szCustomTournamentTtitle;
	DWORD m_dwTournamentIndex;	

public:	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	void SetCustomTournamentData( DWORD dwTournamentIndex );

public:
	CustomTournamentCreateResultWnd();
	virtual ~CustomTournamentCreateResultWnd();
};