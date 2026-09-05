#pragma once

#include "ioMessageBox.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ioMessageBox의 클래스들을 분리했음
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*********************************************************************************
아래의 메시지 팝업들이 시스템 팝업을 상속받을 이유가 있는지 모르겠음.... 
구조도 다르며 다른 기능에 대한 알림 기능을 수행하지 못한다. 
아래의 부모 클래스는 시스템 팝업이 변경되는데에 대해 아래의 팝업이 변경
되는걸 방지함.       2009.0220.LJH
**********************************************************************************/
class ioExpansionBox : public ioWnd
{
public:
	enum
	{
		ID_YES		= 1,
		ID_NO		= 2,
		ID_X		= 3,
		ID_OTHER	= 4,
	};

	enum 
	{
		HEIGHT_TOP_GAP    = 41,
		HEIGHT_BOTTOM_GAP = 23,
		TEXT_LINE_GAP     = 24,
		BTN_YPOS_OFFSET   = 10,
		TOP_TEXT_GAP      = 41,
	};

protected:
	StringVector m_MessageList;
	ioUIRenderImage *m_pLine;

	int m_iMinWidth;
	int m_iMinHeight;

	int m_iTopBottomTextGap;
	int m_iSideTextGap;
	int m_iBoxType;

	ioWnd *m_pPrevWnd;
	ioWnd *m_pReturnWnd;

public:
	void SetMessage( int iType, const char *szBuf, ioWnd *pPrevWnd, ioWnd *pReturnWnd );

protected:
	virtual void ReBatchButton( int iType );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

protected:
	virtual void OnRender();

public:
	ioExpansionBox();
	virtual ~ioExpansionBox();
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// New
//////////////////////////////////////////////////////////////////////////
class ioPlayStage;
class ioExitMsgBox : public ioWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_LEAVE_RESERVE	= 2,
		ID_CANCEL			= 3,
		ID_QUICK_LEAVE		= 4,
		ID_RESERVE_CANCEL	= 5,
		ID_SURRENDER		= 6,
		ID_NEVER_SURRENDER	= 7,
	};

	enum
	{
		MSG_LOSA_EXIT       = 1,
		MSG_BATTLE_LEAVE    = 2,
		MSG_LADDER_LEAVE    = 3,
		MSG_ENTER_LOBBY     = 4,
		MSG_ROOM_EXIT       = 5,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pPenaltyIcon;
	ioUIRenderImage *m_pNonePenaltyIcon;

protected:
	ioPlayStage *m_pPlayStage;

protected:
	DWORD m_dwStartTime;
	DWORD m_dwMsgType;
	bool  m_bPenaltyExit;

public:
	void SetMsgInfo( ioPlayStage *pPlayStage );
	void SetPlayJoin();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual void iwm_show();
	virtual bool iwm_spacebar();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	int  GetExitType();
	bool IsMyCharPenaltyState();

protected:
	void ExitLogByRegularTournament();

public:
	ioExitMsgBox();
	virtual ~ioExitMsgBox();
};

//////////////////////////////////////////////////////////////////////////
// 2013. 01. 28 LJH
// 전투방에서 전투방으로의 이동 기능 추가를 위해서 ioExitMsgBox분리
// ioExitMsgBox는 더이상 전투방에서 사용하지 않는다. 
//////////////////////////////////////////////////////////////////////////
class ioUIRenderFrame;
class ioBattleRoomExitMsgBox : public ioWnd
{
public:
	enum
	{
		ID_EXIT					= 1,
		ID_ROOM_LEAVE_BLUE		= 2,
		ID_ROOM_LEAVE_RED		= 3,
		ID_ROOM_SURRENDER_BLUE	= 4,
		ID_ROOM_SURRENDER_RED	= 5,
		ID_ROOM_LIST			= 6,
		ID_ROOM_QUICK_JOIN		= 7,
		ID_ROOM_SETTING			= 8,

		ID_ROOM_LIST_WND    = 100,
	};

	enum
	{
		BOTTOM_BAR_START_Y    = 193,
		NORMAL_WND_HEIGHT     = 258,
		ROOM_SEARCH_WND_HEIGHT= 481, 
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pPenaltyIcon;
	ioUIRenderImage *m_pNonePenaltyIcon;
	ioUIRenderFrame *m_pBottomBar;

protected:
	ioPlayStage *m_pPlayStage;

protected:
	DWORD m_dwStartTime;
	bool  m_bPenaltyExit;
	bool  m_bObserberUser;
	bool  m_bOpenRoomListWnd;

protected:
	void OpenRoomListWnd( bool bOpen );

public:
	void SetMsgInfo( ioPlayStage *pPlayStage );
	void SetPlayJoin();

public:
	virtual void iwm_hide();
	virtual void iwm_show();
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	bool IsMyCharPenaltyState();

public:
	void ApplyBattleRoomList( SP2Packet &rkPacket );
	void ExitLogByRegularTournament();

public:
	inline const bool IsGlobalCheckPenaltyExit() const { return m_bPenaltyExit; }

public:
	ioBattleRoomExitMsgBox();
	virtual ~ioBattleRoomExitMsgBox();
};

//////////////////////////////////////////////////////////////////////////

class ioBattleRoomExitMsgBoxBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pLiner;
	ioUIRenderImage *m_pSawtoothIcon;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	ioBattleRoomExitMsgBoxBtn();
	virtual ~ioBattleRoomExitMsgBoxBtn();
};

//////////////////////////////////////////////////////////////////////////
class ioBattleRoomExitPopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_LEAVE_RESERVE	= 2,
		ID_RESERVE_CANCEL	= 3,
		ID_SURRENDER		= 4,
		ID_NEVER_SURRENDER	= 5,
		ID_QUICK_LEAVE		= 6,
	};

public:
	enum
	{
		POPUP_TYPE_LEAVE = 1,
		POPUP_TYPE_SURRENDER = 2,
	};

protected:
	enum
	{
		BOTTOM_GRAY_Y		= 65,
		BTN_BOTTOM_Y		= 48,
		LEAVE_HEIGHT		= 147,
		SURRENDER_HEIGHT	= 166,
	};

protected:
	ioUIRenderFrame *m_pBottomBar;

protected:
	ioPlayStage *m_pPlayStage;
	int          m_iPopupType;
	bool         m_bPenaltyExit;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();

public:
	void SetPopupType( ioPlayStage *pPlayStage, int iPopupType, bool bPenaltyExit );

protected:
	void ExitLogByRegularTournament( int iType );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnRender();

public:
	ioBattleRoomExitPopup();
	virtual ~ioBattleRoomExitPopup();
};

//////////////////////////////////////////////////////////////////////////
class ioBattleRoomExitRoomListWnd : public ioWnd
{
public:
	enum
	{
		ID_PAGE_LEFT    = 1,
		ID_PAGE_RIGHT   = 2,
		ID_REFRESH      = 3,

		ID_LIST_1		= 100,
		ID_LIST_2		= 101,
		ID_LIST_3		= 102,
		ID_LIST_4		= 103,
		ID_LIST_5		= 104,
		ID_LIST_6		= 105,

		ID_MODE_HELP_TOOLTIP = 500,
	};

	enum
	{
		MAX_PARTY_LIST		= 6,
		PARTY_SEARCH_TIME	= 10000,		
	};

protected:
	ioWnd *m_pModeHelpTooltip;

protected:
	DWORD      m_dwSearchTime;
	int        m_iCurPage;	
	int        m_iMaxPage;

	typedef std::vector< BattleRoomInfo* > vBattleRoomInfo;
	vBattleRoomInfo m_vBattleRoomList;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void UpdateSetPage( int iCurPage );
	void ProcessSearch();

public:
	void SetPartyList( SP2Packet &rkPacket );
	void StartSend( bool bProcess = false );
	bool IsIndexToList( DWORD dwIndex );
	void Refresh();

protected:
	void ProcessModeHelpTooltip();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ioBattleRoomExitRoomListWnd();
	virtual ~ioBattleRoomExitRoomListWnd();
};