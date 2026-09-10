#pragma once
#include "../ioCalendar.h"

class ioUIRenderImage;
class InviteProgressWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		   = 1,
	};

	enum
	{
		INVITE_WAIT_SEC = 2000,
	};

protected:
	ioUIRenderImage *m_pPhoneIcon;
	ioUIRenderImage *m_pBackType1;
	ioUIRenderImage *m_pBackType2;
	ioUIRenderImage *m_pGauge;

protected:
	int m_iInviteCount;
	ioHashString m_szName;
	DWORD m_dwStartTimer;
	bool  m_bComplete;
	ioWnd *m_pOpenWnd;

public:
	void StartInivteWait( ioWnd *pOpenWnd, int iValue, const ioHashString &szName );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	InviteProgressWnd();
	virtual ~InviteProgressWnd();
};
//////////////////////////////////////////////////////////////////////////
class JoinRequestProgressWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		   = 1,
	};

	enum
	{
		INVITE_WAIT_SEC = 8500,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack1;
	ioUIRenderImage *m_pIconBack2;
	ioUIRenderImage *m_pGauge;

protected:
	DWORD	m_dwStartTimer;
	bool	m_bComplete;
	int		m_eModeType;

public:
	void SetModeType( int eModeType );

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	JoinRequestProgressWnd();
	virtual ~JoinRequestProgressWnd();
};
//////////////////////////////////////////////////////////////////////////
class UppderAlphaWnd : public ioWnd
{
protected:
	DWORD m_dwAlphaTime;
	DWORD m_dwShowTimer;
	bool  m_bSetSizeFromParent;
	
public:
	void SetSizeFromParent(bool bSetSizeFromParent) { m_bSetSizeFromParent = bSetSizeFromParent; }

public:
	virtual void iwm_show();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	UppderAlphaWnd();
	virtual ~UppderAlphaWnd();
};
//////////////////////////////////////////////////////////////////////////
class GradeUPWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		   = 1,
		ID_CLOSE       = 2,
		ID_ALPHA_WND   = 100,
	};

protected:
	ioUIRenderImage *m_pGradeBack;
	ioUIRenderImage *m_pMajorMark;
	ioUIRenderImage *m_pMinorGrade;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	GradeUPWnd();
	virtual ~GradeUPWnd();
};
//////////////////////////////////////////////////////////////////////////
class SoldierLevelUPWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		   = 1,
		ID_CLOSE       = 2,
		ID_ALPHA_WND   = 100,
	};

protected:
	ioUIRenderImage *m_pSoldierIcon;
	ioUIRenderImage *m_pSoldierSubIcon;
	
protected:
	int m_iClassType;
	int m_iPesoBonus;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetLevelUPData( int iClassType, int iPesoBonus );
	bool CheckHaveResource( int iClassType );

public:
	SoldierLevelUPWnd();
	virtual ~SoldierLevelUPWnd();
};
//////////////////////////////////////////////////////////////////////////
class GlobalChatWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_CHAT_MSG_WND   = 1,
		ID_CHAT_INPUT_WND = 2,

		ID_CHAT_ALL_TAB		= 11,
		ID_CHAT_ROOM_TAB	= 12,
		ID_CHAT_SYSTEM_TAB	= 13,
		ID_CHAT_CUSTOM_TAB	= 14,

		ID_CUSTOM_OPTION	= 101,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

protected:
	ioHashString m_szLoadInfo;
	ioPlayStage *m_pPlayStage;

protected:
	int m_iDefaultX;
	int m_iDefaultY;

	SettingOption m_CurrentOption;

	DWORD m_dwCurSelectTab;

	ioUIRenderImage* m_pBackImage1;
	ioUIRenderImage* m_pBackImage2;

	ioUIRenderImage* m_pCurBackImage;
	BYTE m_iAlphaRate;

	ioUIRenderImage* m_pDotImage;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

	void SetInitState();
	void RefreshChatOption();

protected:
	void CheckRoomTitle();
	bool CheckTabButton( DWORD dwID );
	void CheckCurFeature();

public:
	GlobalChatWnd();
	virtual ~GlobalChatWnd();
};
//////////////////////////////////////////////////////////////////////////
class ChatOptionWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT_BTN		= 1,
		ID_OK_BTN		= 2,
		ID_CANCEL_BTN	= 3,
		ID_INIT_BTN		= 4,

		ID_RADIO_BTN1	= 11,
		ID_RADIO_BTN2	= 12,
		ID_RADIO_BTN3	= 13,
		ID_RADIO_BTN4	= 14,
		ID_RADIO_BTN5	= 15,

		ID_NORMAL_CHAT	= 21,
		ID_WHOLE_CHAT	= 22,
		ID_SYSTEM_CHAT	= 23,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

protected:
	ioPlayStage *m_pPlayStage;
	SettingOption m_CurrentOption;

protected:
	int m_iDefaultX;
	int m_iDefaultY;

	DWORD m_dwCurRadioBTN;
	DWORD m_dwCurFeature;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

protected:
	void UpdateByCurrentOption();

	void CheckChatBack( DWORD dwID );
	void SetButtonCheck( DWORD dwID, bool bCheck );

	void ApplyOption();

public:
	ChatOptionWnd();
	virtual ~ChatOptionWnd();
};
//////////////////////////////////////////////////////////////////////////
#define  GLOBAL_TEXT_TOOLTIP_LINE 5
class GlobalTextTooltip : public ioWnd
{
public:
	enum
	{
		MIN_WIDTH		= 0,
		MIN_HEIGHT		= 26,
		TEXT_LINE_GAP   = 18,
		SIDE_TEXT_GAP	= 14,

	};

protected:
	ioComplexStringPrinter m_Printer[GLOBAL_TEXT_TOOLTIP_LINE];

protected:
	void ProcessPosition();
	void ClearList();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTooltipText( ioComplexStringPrinter *rkPrinter );

public:
	GlobalTextTooltip();
	virtual ~GlobalTextTooltip();
};
//////////////////////////////////////////////////////////////////////////
class ModeHelpTooltip : public ioWnd
{
public:
	enum
	{
		DEFAULT_WIDTH = 164,
		DEFAULT_HEIGHT= 70,

		START_TITLE_X  = 82,
		START_TITLE_Y  = 14,

		START_HELP_X   = 84,
		START_HELP_Y   = 44,

		LINE_GAP_HEIGHT= 16,
	};

protected:
	int m_iQuickstartModeHelpIndex;
	ioHashString m_szModeTitle;

protected:
	ioWnd *m_pCallWnd;

public:
	void SetModeHelpTooltip( ioWnd *pCallWnd, const ioHashString &szTitle, int iQuickstartModeHelpIndex ); 

protected:
	void ProcessPosition();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ModeHelpTooltip();
	virtual ~ModeHelpTooltip();
};
//////////////////////////////////////////////////////////////////////////
class UserBlockTypeAlarm : public ioWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_CLOSE = 2,
	};

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );	
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

protected:
	virtual void OnRender();

public:
	UserBlockTypeAlarm();
	virtual ~UserBlockTypeAlarm();
};
/**************************************************************************/
/* 전투방과 진영팀의 UI에 붙는 말풍선입니다. 3D 캐릭터 머리위에 나오는것이*/
/* 아닙니다                                                               */
/**************************************************************************/
class ChatBubbleWnd : public ioWnd
{
protected:
	ioUIRenderImage *m_pTooltipTail;

protected:
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
	void ChangeChatBubble( ChatBubbleWnd *pChatBubbleWnd );

public:
	const ioHashString &GetBubbleName(){ return m_szBubbleName; }

public:
	ChatBubbleWnd();
	virtual ~ChatBubbleWnd();
};
//////////////////////////////////////////////////////////////////////////
class MovieModeHelpWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT		 = 1,
		ID_MOVIEMODE = 2,
		ID_CANCEL	 = 3,
	};

protected:
	ioUIRenderImage *m_pLeftRes;
	ioUIRenderImage *m_pRightRes;
	ioUIRenderImage *m_pMouseRes;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );	
	virtual bool iwm_spacebar();
	
protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	MovieModeHelpWnd();
	virtual ~MovieModeHelpWnd();
};
//////////////////////////////////////////////////////////////////////////
class ClauseViewWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT	  = 1,
		ID_CLOSE  = 2,

		ID_VERT_SCROLL    = 1000,
	};

public:
	enum
	{
		MAX_LINE = 8,
	};

protected:
	int    m_iCurPos;

protected:
	struct ClauseText
	{
		int m_iOffSet;
		ioHashString m_szText;
		
		ClauseText()
		{
			m_iOffSet = 0;
		}
	};
	typedef std::vector< ClauseText > vClauseText;
	vClauseText m_ClauseText;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void iwm_vscroll( DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

protected:
	virtual void OnRender();

public:
	ClauseViewWnd();
	virtual ~ClauseViewWnd();
};
//////////////////////////////////////////////////////////////////////////
class FileWriteUploadWnd : public ioWnd
{
protected:
	ioUIRenderImage *m_pGaugeBar;

protected:
	int m_iMaxFile;
	int m_iCurFile;
	int m_iMaxFileSize;
	int m_iCurFileSize;
	int m_iUploadFullRate;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnRender();
	void OnRenderGauge( int iXPos, int iYPos );

public:
	void InitUploadData( int iMaxFile );
	void UploadRemainFile( int iRemailFile );
	void UploadFileSize( int iCurFileSize, int iMaxFileSize );

public:
	FileWriteUploadWnd();
	virtual ~FileWriteUploadWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomThumbnailTooltip : public ioWnd
{
public:
	enum
	{
		ID_CUSTOM_1 = 1,
		ID_CUSTOM_2 = 2,
	};

	enum
	{
		MIN_HEIGHT		= 26,
		TEXT_LINE_GAP   = 18,
		SIDE_TEXT_GAP	= 14,
		THUMBNAIL_WIDTH = 190,
		THUMBNAIL_HEIGHT= 193,

	};

protected:
	ioComplexStringPrinter m_Printer[GLOBAL_TEXT_TOOLTIP_LINE];
	EXTRAITEMSLOT          m_ExtraItem;

protected:
	void ProcessPosition();
	void ClearList();

public:
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTooltipText( ioComplexStringPrinter *rkPrinter, EXTRAITEMSLOT &rkExtraItem );

public:
	ItemCustomThumbnailTooltip();
	virtual ~ItemCustomThumbnailTooltip();
};
//////////////////////////////////////////////////////////////////////////
class EtcMotionSelectWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_COMPLETE = 2,
		ID_CANCEL	= 3, 
		ID_SELECT_BTN  = 4,

		ID_SELECT_LIST = 1000,
	};
protected:
	ioWnd *m_pCallWnd;
	DWORD  m_dwEtcItem;
	DWORD  m_dwBeforeSelectID;
	DWORD  m_dwAfterSelectID;

public:
	void ShowSelectWnd( ioWnd *pCallWnd );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	EtcMotionSelectWnd();
	virtual ~EtcMotionSelectWnd();
};
//////////////////////////////////////////////////////////////////////////
class EtcMotionSelectListWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL = 1000,		
	};

	enum
	{
		MAX_PAGE_LIST = 10,
	};

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	int   m_iAddWidth;
	int   m_iAddHeight;
	int   m_iListXPos;
	int   m_iListYPos;
	int   m_iHeightGap;
	int   m_iCutWidth;
	int   m_iStartXPos;
	int   m_iStartYPos;

protected:
	DWORD m_dwEtcItem;
	int   m_iCurPos;	

protected:
	struct SelectData
	{
		DWORD m_dwSelectID;
		ioHashString m_szTitle;
	};
	typedef std::vector< SelectData > SelectList;
	SelectList m_SelectList;
	DWORD      m_dwSelectID;

public:
	void ShowSelectListWnd( DWORD dwEtcItem );	

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	EtcMotionSelectListWnd();
	virtual ~EtcMotionSelectListWnd();
};
//////////////////////////////////////////////////////////////////////////
class PingStepWnd // 필요한 Wnd에서 생성
{
public:
	enum
	{
		PING_MS_VALUE = 30,
		MAX_PING_STEP = 5,
	};

protected:
	ioUIRenderImage *m_pPingStep[MAX_PING_STEP];
	ioUIRenderImage *m_pServerRelay;

public:
	void Create();

public:
	void RenderPing( int iXPos, int iYPos, int iPingStep, bool bServerRelay, UIRenderType eRenderType = UI_RENDER_NORMAL );

public:
	PingStepWnd();
	virtual ~PingStepWnd();
};
//////////////////////////////////////////////////////////////////////////
class SoldierExperienceWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_CLOSE= 2,
	};

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	SoldierExperienceWnd();
	virtual ~SoldierExperienceWnd();
};
//////////////////////////////////////////////////////////////////////////

class NetworkWnd : public ioWnd
{
public:
	enum
	{
		ID_STEP_WND	= 1,

		MAX_TEXT	= 3,
	};

protected:
	int m_iDefaultX;
	int m_iDefaultY;

	int m_iYOffSet;
	int m_iCurStep;

	int m_iPlayPosY;
	int m_iAddPlayPosY;

	IoUIText* m_NetText[MAX_TEXT];

	ioHashString m_szTooltipPing;
	ioHashString m_szTooltipLoss;

public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void SetPositionToDefault( int iParentWidth,
		int iParentHeight,
		bool bChildMove,
		bool bCreate = false );

	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	void SetModeShow( int iModeType );
	void SetDeathPos();
	void SetPlayPos();
	void SetAddPlayPosY( int iAddPlayPosY ) { m_iAddPlayPosY = iAddPlayPosY; SetPlayPos(); }

protected:
	void ResetWndPos();

	void ClearUITextList();
	void GetUITextList();

	void ChangeTextState();

	void OpenToolTip( int iWndID );

public:
	NetworkWnd();
	virtual ~NetworkWnd();
};

/////////////////

class StringScrollListWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL = 1000,		
	};

	enum
	{
		MAX_PAGE_LIST  = 10,
	};

	enum OpenStyle
	{
		DOWN_STYLE,
		UP_STYLE,
	};

protected:
	int m_iDefaultXPos;
	int m_iDefaultYPos;

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	OpenStyle m_eStyle;

	int   m_iAddWidth;
	int   m_iAddHeight;
	int   m_iListXPos;
	int   m_iListYPos;
	int   m_iHeightGap;
	int   m_iCutWidth;
	int   m_iStartXPos;
	int   m_iStartYPos;

protected:
	struct ListData
	{			
		int m_iNotifyCommand;
		int m_iNotifyCommandCode;

		ioHashString m_szTitle;
		ioUIRenderImage *m_pMarkRes;
		ListData()
		{
			m_iNotifyCommand = 0;
			m_iNotifyCommandCode = 0;
			m_pMarkRes = NULL;
		}
	};
	typedef std::vector< ListData > vListData;
	vListData m_ListData;
	ListData  m_OverListItem;
	int		  m_iCurPos;	
	ioWnd    *m_pOpenBtn;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetList( ioWnd *pOpenBtn, int iStartXPos, int iStartYPos, OpenStyle eStyle = DOWN_STYLE );
	void AddList( int iNotifyCommand, int iNotifyCommandCode, const ioHashString &szTitle, ioUIRenderImage *pMarkRes );
	void ShowListWnd();

public:
	ioWnd *GetOpenBtn(){ return m_pOpenBtn; }

public:
	StringScrollListWnd();
	virtual ~StringScrollListWnd();
};
//////////////////////////////////////////////////////////////////////////
class CalendarButton : public ioButton
{
protected:
	ioUIRenderFrame *m_pSelectNor;
	ioUIRenderFrame *m_pSelectOver;
	ioUIRenderFrame *m_pSelectPush;

	ioUIRenderFrame *m_pCurrentNor;
	ioUIRenderFrame *m_pCurrentPush;
	ioUIRenderFrame *m_pCurrentOver;

protected:
	static ioCalendarSet m_StartEnableSet;	//버튼들의 활성화 날짜(0이면 모두 활성화)
	static int m_iPeriodEnableTime;

protected:
	ioCalendarSet m_CalendarSet;

protected:
	bool m_bSelect;
	bool m_bPeriod;

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	void SetDate( int iYear, int iMonth, int iDay, int iDayOfWeek );
	void SetSelect( bool bSelect );

public:
	ioCalendarSet& GetDate();

public:
	bool GetPeriod(){ return m_bPeriod; }

public:
	static void SetDateOption( ioCalendarSet StartEnableSet, int iPeriodEnableTime );
	static ioCalendarSet& GetEnableDate(){ return m_StartEnableSet; }

protected:
	virtual void OnRender();

	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

public:
	CalendarButton();
	~CalendarButton();
};
//////////////////////////////////////////////////////////////////////////
class CalendarWnd : public ioWnd
{
private:
	enum
	{
		ID_CLOSE			= 1,
		ID_CONFIRM			= 2,
		ID_CANCEL			= 3,

		ID_HOUR_INPUT		= 10,
		ID_HOUR_BTN			= 11,
		ID_HOUR_LIST		= 12,
		ID_MINITUE_INPUT	= 13,
		ID_MINITUE_BTN		= 14,
		ID_MINITUE_LIST		= 15,

		ID_PREV_BTN			= 20,
		ID_NEXT_BTN			= 21,

		ID_CALENDAR_SUN1	= 100,
		ID_CALENDAR_MON1	= 101,
		ID_CALENDAR_TUE1	= 102,
		ID_CALENDAR_WEN1	= 103,
		ID_CALENDAR_THU1	= 104,
		ID_CALENDAR_FRI1	= 105,
		ID_CALENDAR_SAT1	= 106,

		ID_CALENDAR_SUN2	= 200,
		ID_CALENDAR_MON2	= 201,
		ID_CALENDAR_TUE2	= 202,
		ID_CALENDAR_WEN2	= 203,
		ID_CALENDAR_THU2	= 204,
		ID_CALENDAR_FRI2	= 205,
		ID_CALENDAR_SAT2	= 206,

		ID_CALENDAR_SUN3	= 300,
		ID_CALENDAR_MON3	= 301,
		ID_CALENDAR_TUE3	= 302,
		ID_CALENDAR_WEN3	= 303,
		ID_CALENDAR_THU3	= 304,
		ID_CALENDAR_FRI3	= 305,
		ID_CALENDAR_SAT3	= 306,

		ID_CALENDAR_SUN4	= 400,
		ID_CALENDAR_MON4	= 401,
		ID_CALENDAR_TUE4	= 402,
		ID_CALENDAR_WEN4	= 403,
		ID_CALENDAR_THU4	= 404,
		ID_CALENDAR_FRI4	= 405,
		ID_CALENDAR_SAT4	= 406,

		ID_CALENDAR_SUN5	= 500,
		ID_CALENDAR_MON5	= 501,
		ID_CALENDAR_TUE5	= 502,
		ID_CALENDAR_WEN5	= 503,
		ID_CALENDAR_THU5	= 504,
		ID_CALENDAR_FRI5	= 505,
		ID_CALENDAR_SAT5	= 506,

		ID_CALENDAR_SUN6	= 600,
		ID_CALENDAR_MON6	= 601,
		ID_CALENDAR_TUE6	= 602,
		ID_CALENDAR_WEN6	= 603,
		ID_CALENDAR_THU6	= 604,
		ID_CALENDAR_FRI6	= 605,
		ID_CALENDAR_SAT6	= 606,
	};

protected:
	enum StartWeek
	{
		WEEK_LINE1	  = 100,
		WEEK_LINE2	  = 200,
		WEEK_LINE3	  = 300,
		WEEK_LINE4	  = 400,
		WEEK_LINE5	  = 500,
		WEEK_LINE6	  = 600,
		WEEK_LINE_MAX = 6,
	};

	enum
	{
		WEEK			= 7,
		ONEDAYTIME		= 24,
		ONEHOUR			= 60,
	};
	
protected:
	ioHashString	m_szDate;
	ioHashStringVec m_szExplainList;
	ioEdit* m_pPreEdit;

protected:
	ioCalendarSet m_CurCalendarSet;
	int m_iPeriodEnableTime;
	
protected:
	ioCalendarSet m_SelectDate;		//현재 선택된 버튼의 날짜값

	DWORD m_dwNotifyParam;
	DWORD m_dwWheelStartTime;
	DWORD m_dwWheelTime;

protected:
	ioWnd* m_pDateRequestWnd;		//달력 정보를 호출한 윈도우

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_wheel( int zDelta );

protected:
	void CreateHourList();
	void CreateMinitueList();

protected:
	void OpenHourList( ioWnd *pOpenWnd );
	void OpenMinitueList( ioWnd *pOpenWnd );

public:
	void SetSelectHour( int iValue );
	void SetSelectMin( int iValue );

public:
	int GetSelectHour();
	int GetSelectMin();

public:
	bool isEnableHour();
	bool isEnableMin();

public:
	ioWnd* FindCalendarButton( ioCalendarSet& FindSet );
	void CheckSelectButton( ioCalendarSet& SelectSet );

public:
	void GetSelectDate( ioCalendarSet& SelectSet );

public:
	virtual void InitCalendar( ioWnd* pDateRequestWnd,
							   DWORD dwNotifyParam,
							   ioHashString& szTtitle,
							   ioHashStringVec& szExplain,
							   ioCalendarSet& StartCalendarSet,
							   int iPeriodEnableTime = 0 );	//0일경우 달력버튼 모두 활성화

	virtual void ModifyCalendar( ioWnd* pDateRequestWnd,
								 DWORD dwNotifyParam,
								 ioHashString& szTtitle,
								 ioHashStringVec& szExplain,
								 ioCalendarSet& CurCalendarSet );

	virtual void InitCalendarButton( int iYear, int iMonth, int iDay );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();

protected:
	void OnTitleRender( int iXPos, int iYPos );
	void OnExplainRender( int iXPos, int iYPos );

public:
	CalendarWnd();
	~CalendarWnd();
};

class ioPracticeMode;
class RetryModeWnd : public ioWnd
{
public:
	enum
	{
		ID_RETRY_MODE_BTN = 1,
	};
protected:
	ioPracticeMode *m_pPlayMode;

public:
	void SetPlayMode( ioPracticeMode *pPlayMode ){ m_pPlayMode = pPlayMode;	}

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void iwm_command(ioWnd *pWnd, int cmd, DWORD param);

	RetryModeWnd();
	virtual ~RetryModeWnd();
};