#pragma once

#include "../io3DEngine/ioMovingWnd.h"

class ioPlayMode;
class SP2Packet;
class ioPlayStage;
class ioUIRenderImage;
class ioUIRenderFrame;

class TournamentListBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pSelectBack;
	ioUIRenderImage *m_pEmptyBack;
	ioUIRenderImage *m_pBannerMark;

protected:
	DWORD m_dwIndex;
	DWORD m_dwOwnerIndex;
	BYTE  m_Type;

public:
	void SetTournament( DWORD dwIndex, DWORD dwOwnerIndex, BYTE Type );

public:
	DWORD GetIndex(){ return m_dwIndex; }
	DWORD GetOwnerIndex(){ return m_dwOwnerIndex; }

public:
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );

protected:
	virtual void OnRender();

public:
	TournamentListBtn();
	virtual ~TournamentListBtn();
};
//////////////////////////////////////////////////////////////////////////
class TournamentMainWnd : public ioWnd
{
public:
	enum
	{
		ID_TOURNAMENT_INFO	= 1,
		ID_TOURNAMENT_ENTER	= 2,
		ID_TOURNAMENT_VIEW  = 3,

		ID_TOURNAMENT_LEFT_BTN	= 4,
		ID_TOURNAMENT_RIGHT_BTN = 5,

		ID_TOURNAMENT_BTN1      = 10,		//정규 대회 예약
		ID_TOURNAMENT_BTN2      = 11,
		ID_TOURNAMENT_BTN3      = 12,
		ID_TOURNAMENT_BTN4      = 13,
		ID_TOURNAMENT_BTN5      = 14,
		ID_TOURNAMENT_BTN6      = 15,
		ID_TOURNAMENT_BTN7      = 16,
		ID_TOURNAMENT_BTN8      = 17,
		ID_TOURNAMENT_BTN9      = 18,
		ID_TOURNAMENT_BTN10     = 19,
		ID_TOURNAMENT_BTN11     = 20,
		ID_TOURNAMENT_BTN12     = 21,
		ID_TOURNAMENT_BTN13     = 22,
	};

protected:
	enum
	{
		MAX_FIRST_PAGE_LIST = 9,
		MAX_NEXT_PAGE_LIST  = 12,
		TOURNAMENT_REQUEST_TIME	= 20000,
	};

protected:
	ioUIRenderFrame *m_pMainBannerFrm;
	ioUIRenderFrame *m_pScheduleOpenGray;
	ioUIRenderFrame *m_pScheduleOpenOrange;
	ioUIRenderFrame *m_pScheduleOpenPink;
	ioUIRenderFrame *m_pScheduleCloseGray;
	ioUIRenderFrame *m_pScheduleCloseGreen;
	ioUIRenderFrame *m_pScheduleCloseOrange;

protected:
	ioUIRenderImage *m_pHorzLine;
	ioUIRenderImage *m_pVertLine;
	ioUIRenderImage *m_pTextBack;
	ioUIRenderImage *m_pTodayMark;
	ioUIRenderImage *m_pMyTeamArrow;

protected:
	ioUIRenderImage *m_pBanner;

protected:
	DWORD m_dwSelectIndex;
	DWORD m_dwWantTourIndex;

protected:	// 정보
	BYTE  m_Type;
	BYTE  m_State;
	BYTE  m_CustomProgressType;
	DWORD m_dwOwnerIdx;
	DWORD m_StateEndMonth;
	DWORD m_StateEndDay;
	DWORD m_StateEndHour;
	DWORD m_StateEndMinute;
	DWORD m_TournamentStartTeamCount;
	DWORD m_TournamentMaxRound;
	ioHashString m_szTournamentTitle;
	ioHashString m_szPlayModeName;
	BYTE  m_MaxPlayer;

protected:
	ioWndList  m_TournamentBtnSet;
	int        m_iTournamentPage;	
	DWORD      m_dwTournamentRequestTime;

protected:
	DWORD m_dwScreenTimer;

	DWORD m_dwMyTeamArrowTimer;
	float m_fArrowYPos;

public:
	static bool m_bScreen;

protected:
	bool  IsStateEndToday();
	void  SetTournamentBtn();
	void  SendTournamentList( int iCurrentPage );

public:
	void SetTournamentLink( DWORD dwTourIndex );

public:	
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );

	void OnCheckRegularTournament();
	void OnScreenProcess();
	void OnUpdateProcess();
	void OnListSyncProcess();
	void OnMyTeamArrowProcess();
	
protected:
	virtual void OnRender();
	void OnMainBannerRender( int iXPos, int iYPos );
	void OnExplainRender( int iXPos, int iYPos );
	void OnScheduleRender( int iXPos, int iYPos );
	void OnOfflineScheduleRender( int iXPos, int iYPos );
	void OnMyTeamArrowRender();
	void OnRender( int iXPos, int iYPos );

public:
	void UpdateMainUI();

public:
	void IwmCommandEnterPlayer( DWORD dwIndex );
	void ShowTeamInfo( DWORD dwTourIndex );
	void ShowTeamEntryList( DWORD dwTourIndex );
	void IwmCommandTeamCreate( DWORD dwIndex );
	void IwmTournamentConfirmCheck( DWORD dwIndex );
	
public:
	DWORD GetTournamentIndex(){ return m_dwSelectIndex; }

public:
	void ApplyTournamentList( SP2Packet &rkPacket );
	void ApplyTournamentConfirmCheck( SP2Packet &rkPacket );
	void ApplyTournamentConfirmRequest( SP2Packet &rkPacket );	

public:
	TournamentMainWnd();
	virtual ~TournamentMainWnd();
};
//////////////////////////////////////////////////////////////////////////
class LobbyTournamentInfoWnd : public ioWnd
{
public:
	enum
	{
		ID_TEAM_BTN1 = 1,
		ID_TEAM_BTN2 = 2,
		ID_TEAM_BTN3 = 3,

		ID_LEFT_BTN  = 4,
		ID_RIGHT_BTN = 5,

		ID_TOURNAMENT_CREATE_BTN  = 6,
	};

	enum
	{
		MAX_PAGE_COUNT = 3,
	};

protected:
	int m_iCurPage;
	int m_iMaxPage;

protected:
	TeamDataVec m_TeamDataList;

protected:
	const DWORD m_dwEtcItem;
	const DWORD m_dwPremiumEtcItem;

protected:
	void CreateTournament();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	
protected:
	virtual void OnRender();

protected:
	void UpdateTournamentTeam( int iCurPage );
	
public:
	LobbyTournamentInfoWnd();
	virtual ~LobbyTournamentInfoWnd();
};
//////////////////////////////////////////////////////////////////////////
class LobbyTournamentTeamBtn : public ioButton
{
public:
	enum
	{
		ID_TOURNAMENT_BNT = 1,
	};

protected:
	ioUIRenderImage *m_pSmallBanner;		

protected:
	DWORD m_dwTeamIndex;
	DWORD m_dwTournamentIndex;

protected:
	// 대회 정보
	BYTE  m_Type;
	BYTE  m_State;
	DWORD m_StateEndMonth;
	DWORD m_StateEndDay;
	DWORD m_StateEndHour;
	DWORD m_StateEndMinute;
	ioHashString m_szTournamentTitle;
	ioHashString m_szTeamName;
	DWORD m_TournamentStartTeamCount;
	DWORD m_TournamentMaxRound;
	BYTE  m_MaxPlayer;

	// 팀 정보
	BYTE  m_CampPos;
	BYTE  m_TourPos;
	SHORT m_Position;
	int   m_iCurPlayer;

protected:
	DWORD m_dwLoadingCommaTimer;
	int   m_iLoadingCommaCount;
	
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetTournamentTeam( DWORD dwTourIndex, DWORD dwTeamIndex );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	void OnUpdateProcess();
	void OnLoadingProcess();

protected:
	virtual void OnRender();
	void OnRenderData( int iXPos, int iYPos );

protected:
	void UpdateTeamData();

public:
	inline DWORD GetTournamentIndex(){ return m_dwTournamentIndex; }
	inline DWORD GetTournamentTeamIndex(){ return m_dwTeamIndex; }

public:
	LobbyTournamentTeamBtn();
	virtual ~LobbyTournamentTeamBtn();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamCreateWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT			 = 1,
		ID_CREATE		 = 2,
		ID_CANCEL		 = 3,
		ID_TEAM_NAME_EDIT= 4,
	};

protected:
	ioUIRenderImage *m_pIcon;	
	ioUIRenderImage *m_pIconBack;

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pNameEdit;

protected:
	DWORD  m_dwTournamentIndex;
	BYTE   m_MaxPlayer;

protected:
	bool IsOnlyEngHanNumText( const char *szText );     //영문, 한글, 숫자만 가능
	void InitEditWnd();
	void SendTeamCreate();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	void SetTournamentIndex( DWORD dwIndex );

public:
	TournamentTeamCreateWnd();
	virtual ~TournamentTeamCreateWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamCreateComplete : public ioWnd
{
public:
	enum
	{
		ID_EXIT      = 1,
		ID_TEAM_VIEW = 2,
		ID_OK        = 3,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;
	
protected:
	DWORD m_dwTeamIndex;
	DWORD m_dwTournamentIndex;
	
	BYTE m_Type;
	BYTE m_CampPos;
	ioHashString m_szTeamName;
	ioHashString m_szTournamentTitle;


public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	void SetTeamData( DWORD dwTourIndex, DWORD dwTeamIndex, ioHashString &rkTeamName, BYTE CampPos );

public:
	TournamentTeamCreateComplete();
	virtual ~TournamentTeamCreateComplete();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamUserOptionWnd : public ioWnd
{
public:
	enum
	{
		OPTION_INFO		    = 1000,
		OPTION_CHANGE_OWNER = 1004,
		OPTION_KICK		    = 1001,
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
	int   m_iStartXPos;
	int   m_iStartYPos;
	int   m_iOverListIndex;

protected:
	DWORD m_dwTourIndex;
	DWORD m_dwTeamIndex;
	DWORD m_dwOwnerIndex;
	DWORD m_dwLeaveTableIndex;
	DWORD m_dwLeaveUserIndex;
	ioHashString m_szName;

	typedef std::map< DWORD, ioHashString > OptionMap;
	OptionMap m_OptionMap;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetOptionInfo( DWORD dwTourIndex, DWORD dwTeamIndex, const ioHashString &rkName );

protected:
	bool IsKickUserOK();
	void CommandOption( DWORD dwCmd );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	TournamentTeamUserOptionWnd();
	virtual ~TournamentTeamUserOptionWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamInfoBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pBlueNormal;
	ioUIRenderImage *m_pRedNormal;
	ioUIRenderImage *m_pGrayNormal;
	ioUIRenderImage *m_pBlueOver;
	ioUIRenderImage *m_pRedOver;
	ioUIRenderImage *m_pGrayOver;
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;
	ioUIRenderImage *m_pMasterMark;

protected:
	BYTE         m_CampPos;
	ioHashString m_szName;
	int			 m_iGradeLevel;
	int			 m_iLadderPoint;
	bool         m_bMasterUser;

	int   m_iSlotArray;
	int   m_iMaxSlot;

public:
	void SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, BYTE CampPos, int iGradeLevel, int iLadderPoint, bool bMasterUser );
	const ioHashString &GetUserName(){ return m_szName; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();
	void OnRenderBase( int iXPos, int iYPos );

public:
	TournamentTeamInfoBtn();
	virtual ~TournamentTeamInfoBtn();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamInfoWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT			= 1,
		ID_RECRUIT		= 2,
		ID_DELAY_JOINERS= 3,
		ID_HELP         = 4,
		ID_TEAM_LEAVE   = 5,
		ID_JOIN_APP     = 6,

		ID_LIST_1		= 10,
		ID_LIST_2		= 11,
		ID_LIST_3		= 12,
		ID_LIST_4		= 13,
		ID_LIST_5		= 14,
		ID_LIST_6		= 15,
		ID_LIST_7		= 16,
		ID_LIST_8		= 17,

		ID_USER_OPTION  = 1000,
	};

	enum
	{
		MAX_LIST		= 8,
	};

protected:
	ioUIRenderImage *m_pCustomBannerIcon;
	ioUIRenderImage *m_pBannerIcon;
	ioUIRenderImage *m_pBannerIconBack;

protected:
	DWORD m_dwTourIndex;
	DWORD m_dwTeamIndex;
	DWORD m_dwTournamentHelpID;
	ioHashString m_szTeamName;
	ioHashString m_szTournamentTitle;

	BYTE    m_Type;
	BYTE    m_State;
	BYTE	m_CampPos;
	BYTE    m_MaxPlayer;
	int     m_iCurPlayer;
	DWORD	m_dwOwnerIndex;
	DWORD	m_dwTeamOwnerIndex;
	DWORD   m_dwLeaveTableIndex;
	DWORD   m_dwLeaveUserIndex;
	TeamUserVec m_UserList;

protected:
	bool IsTeamLeaveOK();

protected:
	void UpdateSyncCheck();
	void UpdateTournament();
	void UpdateData( bool bDataSync );
	void UpdateChildWnd();
	void UpdateUserData();
	void SendTeamLeave();
	void SendTeamEntryApp();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTournamentTeam( DWORD dwTourIndex, DWORD dwTeamIndex );

public:
	void DeleteTeamCheck( DWORD dwTeamIndex );

public:
	TournamentTeamInfoWnd();
	virtual ~TournamentTeamInfoWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamInvitationFriendWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL = 1000,		
	};

	enum
	{
		MAX_PAGE_LIST = 5,
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
	ioHashStringVec m_OnlineUserList;
	ioHashString    m_szOverListUser;
	int				m_iCurPos;	

protected:
	DWORD m_dwTournamentOwnerIndex;

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
	const ioHashString &GetOverUser(){ return m_szOverListUser; }
	void SetTournamentOwenrIndex( DWORD dwOwnerIndex );

public:
	TournamentTeamInvitationFriendWnd();
	virtual ~TournamentTeamInvitationFriendWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamInvitationWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT         = 1,
		ID_SEND_BTN		= 2,
		ID_CANCEL_BTN   = 3,
		ID_FRIEND_BTN   = 4,

		ID_NAME_EDIT    = 100,
		ID_FRIEND_LIST  = 1000,
	};

protected:
	DWORD m_dwTourOwnerIndex;
	DWORD m_dwTourIndex;
	DWORD m_dwTeamIndex;

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pNameEdit;

	void SendInvitation();
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTournamentInvitation( DWORD dwTourIndex, DWORD dwTeamIndex );

public:
	TournamentTeamInvitationWnd();
	virtual ~TournamentTeamInvitationWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamInvitedWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT           = 1,
		ID_INFO_BTN		  = 2,
		ID_REFUSE_BTN	  = 3,
		ID_INVITED_IGNORE = 4,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

protected:
	static DWORD m_dwTournamentIndex;
	static DWORD m_dwTeamIndex;

	static ioHashString m_szUserName;
	static ioHashString m_szTeamName;
	static ioHashString m_szTourTitle;

	static BYTE m_Type;
	static BYTE m_CampPos;

	static bool  sg_bInvitedIgnore;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();
	virtual void iwm_show();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetInvitedInfo( SP2Packet &rkPacket );

public:
	TournamentTeamInvitedWnd();
	virtual ~TournamentTeamInvitedWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamEntryDelayUserBtn : public ioButton
{
public:
	enum
	{
		ID_AGREE   = 1,
		ID_REFUSE  = 2,
	};	

protected:
	DWORD m_dwTourIndex;
	DWORD m_dwTeamIndex;

	DWORD m_dwTableIndex;
	DWORD m_dwUserIndex;
	int	  m_iUserLevel;
	ioHashString m_szUserID;
	int   m_iMaxList;
	DWORD m_dwMsgBoxID;
	BYTE  m_CampPos;

protected:
	bool IsError( DWORD dwID );

public:  
	void MemberInfo( int iMaxList, DWORD dwTourIndex, DWORD dwTeamIndex, DWORD dwTableIndex, DWORD dwUserIndex, int iUserLevel, const ioHashString &szUserID, BYTE CampPos );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();	
	virtual void OnProcess( float fTimePerSec );

public:
	ioHashString &GetMemberName(){ return m_szUserID; }

public:
	TournamentTeamEntryDelayUserBtn();
	virtual ~TournamentTeamEntryDelayUserBtn();
};

class TournamentTeamEntryDelayUserWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_LEFT_BTN			= 2,
		ID_RIGHT_BTN		= 3,
		ID_AUTO_SHOW_IGNORE = 4,

		ID_LIST_1	= 10,
		ID_LIST_2	= 11,
		ID_LIST_3	= 12,
		ID_LIST_4	= 13,
		ID_LIST_5	= 14,
	};	

	enum
	{
		MAX_LIST = 5,
		AUTO_SHOW_HEIGHT = 295,
		CUSTOM_SHOW_HEIGHT = 247,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderImage *m_pIcon;	
	ioUIRenderImage *m_pIconBack;

protected:
	static bool sg_bAutoShowIgnore;

protected:
	int  m_iCurPage;
	int  m_iMaxPage;
	bool m_bAutoShow;

	BYTE m_Type;
	BYTE m_CampPos;
	DWORD m_dwTourIndex;
	DWORD m_dwTeamIndex;
	ioHashString m_szTeamName;
	ioHashString m_szTournamentTitle;

	struct EntryUser
	{
		DWORD m_dwTableIndex;
		DWORD m_dwUserIndex;
		int   m_iUserLevel;
		BYTE  m_CampPos;
		ioHashString m_szUserID;

		EntryUser()
		{
			m_dwTableIndex = 0;
			m_dwUserIndex  = 0;
			m_iUserLevel   = 0;
			m_CampPos      = 0;
		}
	};
	typedef std::vector< EntryUser> vEntryUser;
	vEntryUser m_vUserList;

	void UpdateSetPage( int iCurPage );
	void ReSizeWnd();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	void UpdateEntryDelayUser();
	void SetAutoShow( DWORD dwTourIndex, DWORD dwTeamIndex, bool bAutoShow );
	void SetEntryDelayUser( SP2Packet &rkPacket );
	void DeleteUserList( DWORD dwTableIndex );

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	TournamentTeamEntryDelayUserWnd();
	virtual ~TournamentTeamEntryDelayUserWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamEventWnd : public ioMovingWnd
{
public:
	enum
	{
		EVENT_TYPE_JOIN = 1,
		EVENT_TYPE_TEAM_DELETE = 2,
		EVENT_TYPE_TEAM_LEAVE  = 3,
	};

	enum
	{
		ID_EXIT = 1,
		ID_INFO = 2,
		ID_CLOSE= 3,
		ID_OK   = 4,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack1;
	ioUIRenderImage *m_pIconBack2;
	ioUIRenderImage *m_pExclamationMark;

protected:
	DWORD m_dwTeamEvent;
	DWORD m_dwTourIndex;
	DWORD m_dwTeamIndex;

protected:
	BYTE  m_Type;
	BYTE  m_CampPos;
	ioHashString m_szTeamName;
	ioHashString m_szTourName;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

protected:
	void UpdateTournament();

protected:
	void OnJoinRender( int iXPos, int iYPos );
	void OnTeamDeleteRender( int iXPos, int iYPos );
	void OnTeamLeaveRender( int iXPos, int iYPos );

public:
	void SetTeamEventData( DWORD dwTourIndex, DWORD dwTeamIndex, DWORD dwEventType );

public:
	TournamentTeamEventWnd();
	virtual ~TournamentTeamEventWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentTeamAlarmWnd : public ioMovingWnd
{
public:
	enum
	{
		ALARM_TYPE_JOIN_USER  = 1,
		ALARM_TYPE_LEAVE_USER = 2,
	};

public:
	enum
	{
		ID_EXIT           = 1,
		ID_INFO_BTN		  = 2,
		ID_CLOSE_BTN	  = 3,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

protected:
	DWORD m_dwTournamentIndex;
	DWORD m_dwTeamIndex;
	DWORD m_dwAlarmType;

	ioHashString m_szUserName;
	ioHashString m_szTeamName;
	ioHashString m_szTourTitle;

	BYTE m_Type;
	BYTE m_CampPos;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void UpdateTournament();
	void SetAlarmData( DWORD dwTeamIndex, DWORD dwUserIndex, DWORD dwAlarmType );

public:
	TournamentTeamAlarmWnd();
	virtual ~TournamentTeamAlarmWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentBattleInviteWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_JOIN_BATTLE = 1,
	};

	enum 
	{
		TICK_TIME = 1000,
	};

protected:
	static DWORD m_dwRedIndex;
	static DWORD m_dwBlueIndex;
	static DWORD m_dwBattleIndex;
	static DWORD m_dwInviteTime;
	static bool m_bLogSync;

protected:
	DWORD m_dwTournamentIndex;
	DWORD m_dwInviteCheckTime;

	BYTE  m_MyCampPos;
	BYTE  m_OtherCampPos;
	DWORD m_dwOtherTeamIndex;
	DWORD m_dwMyTeamIndex;
	ioHashString m_szMyTeamName;
	ioHashString m_szOtherTeamName;

	

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void UpdateTournamentProcess();
	void UpdateTournament();
	void UpdateTournamentTeam();
	void SyncGlobalTeamData();
	void CheckSyncOtherTeamData();
	void GetTeamData();
	void ResetBattleData();

	static void SetTournamentBattle( DWORD dwBattleIndex, DWORD dwBlueIndex, DWORD dwRedIndex, DWORD dwInviteTime );

public:
	TournamentBattleInviteWnd();
	virtual ~TournamentBattleInviteWnd();
};

//////////////////////////////////////////////////////////////////////////
class TournamentRuleAgreeConfirmWnd : public ioWnd
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

	TournamentRuleAgreeConfirmWnd();
	~TournamentRuleAgreeConfirmWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentMatchWinAlarmWnd : public ioWnd
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
	virtual bool iwm_esc();

public:
	void SetModeType( DWORD dwModeType );

public:
	void OnVictoryRender( int iXPos, int iYPos );

protected:
	virtual void OnRender();

public:
	TournamentMatchWinAlarmWnd();
	~TournamentMatchWinAlarmWnd();
};

//////////////////////////////////////////////////////////////////////////

class TournamentUnearnedWinAlarmWnd : public ioWnd
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

	int m_iTournamentRoundWin;
	ioHashString m_szTournamentName;
	BYTE m_TournamentType;

public:	
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();

public:
	void SetTournamentData( ioHashString szTournamentName, int iGoRound, BYTE TournamentType );
	void TournamentTeamPosition();

public:
	void OnVictoryRender( int iXPos, int iYPos );

protected:
	virtual void OnRender();

public:
	TournamentUnearnedWinAlarmWnd();
	~TournamentUnearnedWinAlarmWnd();
};
//////////////////////////////////////////////////////////////////////////
class TournamentEtcItemNeedPopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT			   = 1,
		ID_CREATE_ITEM_BUY = 2,
		ID_CANCEL		   = 3,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	TournamentEtcItemNeedPopup();
	virtual ~TournamentEtcItemNeedPopup();
};
//////////////////////////////////////////////////////////////////////////
class TournamentEtcItemUsePopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_USE		= 2,
		ID_CANCEL	= 3,

		ID_RADIO_NORMAL = 10,
		ID_RADIO_PREMIUM= 11,
	};

	enum
	{
		ENABLE_CREATE_GAP_MIN = 5,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;

protected:
	const DWORD m_dwEtcItem;
	const DWORD m_dwPremiumEtcItem;

protected:
	DWORD m_dwRadioBtnID;
	ioHashString m_szRadioTitle;

protected:
	void SetRadioButton( DWORD dwID );

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	TournamentEtcItemUsePopup();
	virtual ~TournamentEtcItemUsePopup();	
};


//////////////////////////////////////////////////////////////////////////
class CusotmTournamentTeamListBtn : public ioButton
{
protected:
	DWORD m_dwTeamIdx;
	DWORD m_dwTourIdx;
	ioHashString m_szTeamName;

public:
	void SetTeamInfo( DWORD dwTourIdx, DWORD dwTeamIdx );

public:
	DWORD GetTeamIndex(){ return m_dwTeamIdx; }
	DWORD GetTourIndex(){ return m_dwTourIdx; }

protected:
	virtual void OnRender();

public:
	CusotmTournamentTeamListBtn();
	virtual ~CusotmTournamentTeamListBtn();
};

//////////////////////////////////////////////////////////////////////////
class CustomTournamentEntryListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT			     = 1,
		ID_LEFT			     = 2,
		ID_RIGHT		     = 3,
		ID_JOIN_REQUEST_LIST = 4,

		ID_TEAM_01		= 11,
		ID_TEAM_02		= 12,
		ID_TEAM_03		= 13,
		ID_TEAM_04		= 14,
		ID_TEAM_05		= 15,
		ID_TEAM_06		= 16,
		ID_TEAM_07		= 17,
		ID_TEAM_08		= 18,
		ID_TEAM_09		= 19,
		ID_TEAM_10		= 20,
		ID_TEAM_11		= 21,
		ID_TEAM_12		= 22,
		ID_TEAM_13		= 23,
		ID_TEAM_14		= 24,
		ID_TEAM_15		= 25,
		ID_TEAM_16		= 26,
	};

	enum
	{
		MAX_TEAM_LIST		= 16,
		TEAM_SEARCH_TIME	= 20000,
	};

protected:
	DWORD m_dwSearchTime;
	DWORD m_dwMaxTeamCnt;

protected:
	DWORD m_dwTourIndex;
	byte m_ProgressType;

protected:
	int m_iCurPage;
	int m_iMaxPage;

protected:
	void ProcessSearch();
	void UpdateSetPage( int iCurPage );
	void StartSend( bool bProcess = false );

public:
	void SetTournament( DWORD dwTourIndex );
	void ApplyCustomTournamentEntryList( SP2Packet &rkPacket );

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );	

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	CustomTournamentEntryListWnd();
	virtual ~CustomTournamentEntryListWnd();
};

//////////////////////////////////////////////////////////////////////////
class CustomTournamentDelayCreateTeamListBtn : public ioButton
{
public:
	enum
	{
		ID_AGREE   = 1,
		ID_REFUSE  = 2,
	};	

protected:
	DWORD m_dwTourIndex;
	DWORD m_dwUserIndex;
	int	  m_iUserLevel;
	ioHashString m_szUserID;
	int   m_iMaxList;
	DWORD m_dwMsgBoxID;

protected:
	bool IsError( DWORD dwID );

public:  
	void SetInfo( int iMaxList, DWORD dwTourIndex, DWORD dwUserIndex, int iUserLevel, const ioHashString &szUserID );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ioHashString &GetUserName(){ return m_szUserID; }

public:
	CustomTournamentDelayCreateTeamListBtn();
	virtual ~CustomTournamentDelayCreateTeamListBtn();
};

class CustomTournamentDelayCreateTeamListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_LEFT_BTN			= 2,
		ID_RIGHT_BTN		= 3,
		ID_AUTO_SHOW_IGNORE = 4,

		ID_LIST_1	= 10,
		ID_LIST_2	= 11,
		ID_LIST_3	= 12,
		ID_LIST_4	= 13,
		ID_LIST_5	= 14,
	};	

	enum
	{
		MAX_LIST = 5,
		AUTO_SHOW_HEIGHT = 295,
		CUSTOM_SHOW_HEIGHT = 247,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

protected:
	static bool sg_bAutoShowIgnore;
	bool m_bAutoShow;

	int m_iCurPage;
	int m_iMaxPage;
	
	DWORD m_dwTourIndex;
	BYTE m_TournamentType;
	ioHashString m_szTournamentTitle;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	void SetAutoShow( DWORD dwTourIdx, bool bAutoShow );

	void UpdateTournament();
	void UpdateSetPage( int iCurPage );
	void DeleteDelayCreateTeamList( DWORD dwUserIndex );

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	void ReSizeWnd();

public:
	CustomTournamentDelayCreateTeamListWnd();
	virtual ~CustomTournamentDelayCreateTeamListWnd();
};

//////////////////////////////////////////////////////////////////////////
class TournamentJoinConfirmAgreeWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT      = 1,		
		ID_OK        = 2,
	};

	enum TourJoinType
	{
		TOUR_JOIN_REQUEST,
		TOUR_JOIN_INVITED,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

protected:
	TourJoinType m_eType;
	BYTE m_TournamentProgressType;
	BYTE m_TournamentType;

	DWORD m_dwTournamentIndex;
	ioHashString m_szTournamentTitle;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTournament( DWORD dwTourIndex, TourJoinType eType );
	void UpdateTournament();

public:
	TournamentJoinConfirmAgreeWnd();
	virtual ~TournamentJoinConfirmAgreeWnd();
};

//////////////////////////////////////////////////////////////////////////
class TournamentInvitationWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT         = 1,
		ID_SEND_BTN		= 2,
		ID_CANCEL_BTN   = 3,
		ID_FRIEND_BTN   = 4,

		ID_NAME_EDIT    = 100,
		ID_FRIEND_LIST  = 1000,
	};

protected:
	DWORD m_dwTourIndex;
	DWORD m_dwTourOwnerIndex;

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pNameEdit;

	void SendInvitation();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTournamentInvitation( DWORD dwTourIndex, DWORD dwTourOwnerIndex );

public:
	TournamentInvitationWnd();
	virtual ~TournamentInvitationWnd();
};