#pragma once

class ioWnd;
class ioBaseChar;
class ioPlayStage;
class ioComplexStringPrinter;
class ioPullDownItemFactory;
class WndBoundaryContainer;

enum
{
	MB_RESERVE_NO_ROOM,
	MB_RESERVE_ABUSE,
	MB_RESERVE_OPEN_SPACE_ABUSE,
	MB_RESERVE_SPEEDHACK,	
	MB_INFOMATION_ALTTAB,
	MB_INFOMATION_PENALTY,
	MB_RESERVE_FULL_ROOM,
	MB_RESERVE_SPEEDHACK_PENALTY,
	MB_LEAVE_SAFE_ROOM,
	MB_PLAZA_KICK_OUT,
	MB_SOLDIER_LIMIT,
	MB_ENTRY_NOT_FORMALITY,
	MB_BATTLE_ROOM_KICK_OUT,
	MB_LADDER_TEAM_KICK_OUT,
	MB_UDP_FAIL_EXIT_ROOM,
	MB_RESERVED_CHANGE_ID,
	MB_VOTE_KICK_OUT,
	MB_CAMP_BATTLE_END,
	MB_MONSTER_COIN_LACK,
	MB_RESERVE_BAD_NETWORK_KICK,
	MB_HEADQUARTERS_KICK_OUT,
	MB_HEADQUARTERS_OUT_MASTER_LOGOUT,
	MB_HEADQUARTERS_OUT_FRIEND_DELETE,
	MB_SERVER_MOVE_DELAY,
	MB_SERVER_MOVE_NO_ROOM,
	MB_RESERVE_MACRO_OUT,
	MB_RELAYSVR_DISCONNECTED, // for relay
	MB_RESERVE_EXCEPTION_BATTLE_ROOM,
	MB_SHUFFLE_ROOM_KICK_OUT,
	MB_BATTLEROOM_JOIN_SEARCH_FAILED,
	MB_BATTLEROOM_JOIN_NOT_NODE,
	MB_BATTLEROOM_JOIN_FULL_USER,
	MB_BATTLEROOM_JOIN_NOT_PASSWORD,
	MB_BATTLEROOM_JOIN_EXCEPTION,
	MB_BATTLEROOM_JOIN_TIME_CLOSE,
	MB_BATTLEROOM_JOIN_START_ROOM_ENTER_X,
	MB_BATTLEROOM_JOIN_SAFETY_ROOM,
	MB_BATTLEROOM_JOIN_MAP_LIMIT_PLAYER,
	MB_BATTLEROOM_JOIN_MAP_LIMIT_GRADE,
	MB_BATTLEROOM_JOIN_NO_CHALLENGER,
	MB_HOUSE_KICK_OUT,
	MB_HOUSE_MASTER_OUT,
	MB_ROOM_EXCEPTION,
	MB_RAID_COIN_LACK,
	MB_FLAG_MODE_ERROR
};

enum
{
	RESERVE_LADDER_TEAM_UI = 0x0001,
};

class ioUIRenderFrame;
class ioSP2GUIManager : public ioGUIManager, public Singleton< ioSP2GUIManager >
{
public:
	enum
	{
		SCREEN_OUT_POS = -9999,
		MSG_BUF_SIZE   = 1024,
	};
public:
	class IWndCreator
	{
	public:
		virtual ioWnd* CreateWnd() = 0;
	};

	template< typename T >	class WindowCreator : public IWndCreator
	{
	public:
		virtual ioWnd* CreateWnd()
		{
			return new T;
		}
	};

protected:
	typedef std::map< ioHashString, IWndCreator* > WindowCreateTable;
	WindowCreateTable m_CreateTable;

protected:
	ioWnd *m_pMsgReturn;
	char  m_szMsgBuf[MSG_BUF_SIZE];

	IntVec m_vReserveMsgBox;		//스테이지가 변경되고 즉시 보여줄 예약 메시지 박스	
	int   m_iPenaltyPeso;			//벌금팝업관련. 벌금 기억하기 위한 변수
	int   m_iPenaltyLadderPoint;

	DWORD m_dwReserveShowUI;         //스테이트 변경 후 활성화 시킬 UI

	bool m_bSkipUIByBlind;
	bool m_bNoneUIMode;
	bool m_bEnableShopSound;
	
	ioUIRenderImage *m_pModalBackGround;
	ioPlayStage *m_pPlayStage;

	ioPullDownItemFactory* m_pPullDownFactory;
		
#ifndef SHIPPING
	typedef std::map<DWORD, WndBoundaryContainer*> WndBoundaryContainerMap;
	WndBoundaryContainerMap m_WndBoundaryContainerMap;
#endif

public:
	virtual void InitManager();
	virtual void ClearAllGUI();

	template < typename T >
	void RegisterWndCreator( const char* key )
	{
		if ( !key ) {
			return;
		}
		m_CreateTable[ key ] = new WindowCreator<T>;
	}

protected:
	bool IsEscKeyCheck();
	bool IsSpaceBarKeyCheck();
	virtual void ProcessGUI( ioMouse &mouse );
	virtual ioWnd* CreateNewWindow( const ioHashString &szClassName );

	void RegisterWndCreator();

	void SetSelectedWnd();

	void AddAndSetMsgBox( int iType, ioWnd *pPrevWnd, ioWnd *pWnd );

public:
	void RenderBackMostWindows();
	void RenderNotBackMostWindows();

	bool CheckAndRenderModalBackGround( bool bRender, ioWnd *pWnd );

public:
	ioUIRenderFrame *CreateFrame( const ioHashString &rkFrameName );

public:
	ioWnd* AddWndWithID( const char *szXMLFileName, DWORD dwID );
	ioWnd* AddWndWithID( const char *szXMLFileName, ioWnd *pParent, DWORD dwID );

public:
	// 인자에 따른 다양한 SetMsgBox 추가 
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, ... );
#else
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource );
#endif
	void SetMsgBox(int iType, ioWnd *pWnd, IoString* pSource);
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, DWORD dwArg1 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, __int64 i64Arg1 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1, int iArg2 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1, const char *szArg2 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, int iArg2 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, __int64 i64Arg2 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, const char *szArg2 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1, int iArg2, int iArg3 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1, const char *szArg2, int iArg3 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, int iArg2, const char *szArg3 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4 );
	void SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, const char *szArg4 );

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
	void SetPrevMsgBox( ioWnd *pPrevWnd, int iType, ioWnd *pWnd, const char *szSource, ... );
#else
	void SetPrevMsgBox( ioWnd *pPrevWnd, int iType, ioWnd *pWnd, const char *szSource );
#endif
	void SetPrevMsgBox( ioWnd *pPrevWnd, int iType, ioWnd *pWnd, const char *szSource, const char *szArg1 );

	void SetPrevMsgListBox( ioWnd *pPrevWnd, int iType, ioWnd *pWnd,
							ioComplexStringPrinter *MsgPrinter );

	void SetPrevMsgListBoxWithTitle( ioWnd *pPrevWnd, int iType, ioWnd *pWnd, 
		                             ioComplexStringPrinter *MsgPrinter, 
									 const char *szTitle, const char *szOKTitle, const char *szYesTitle, const char *szNoTitle,
									 TextAlignType eAlignType = TAT_CENTER );

	void SetExpandMedalSlotMsgBox( ioWnd *pPrevWnd, ioWnd *pWnd,
								   int iSlotNumber,
								   TextAlignType eAlignType = TAT_CENTER );


	void SetFishingMsgListBox( ioWnd *pWnd, int iStartArray );
	void SetFishingLevelUpBox( ioWnd *pWnd, int iCurLevel, bool bAddSlot );

	void SetPrevMsgListPinkBoxWithTitle( ioWnd *pPrevWnd, int iType, ioWnd *pWnd, ioComplexStringPrinter *MsgPrinter, const char *szTitle, const char *szOKTitle, const char *szYesTitle, const char *szNoTitle, TextAlignType eAlignType = TAT_CENTER );

	void NotifyMsgBoxClose( ioWnd *pMsgBox, int iPushed );
	void NotifyMsgBoxCommand( ioWnd *pMsgbox, int iPushed );
	void SetReserveMsgBox( int iMsgType );
	void AddReserverShowUI( DWORD dwAddUI );

public:
	void CheckGUIPosSave();
	int GUIPosLoadInt( const char *szTitle, const char *szKeyName, int iValue ,int iVersion = 0);
	int GUIPosSaveInt( const char *szTitle, const char *szKeyName, int iValue ,int iVersion = 0);

	int EtcValueLoadInt( const char *szTitle, const char *szKeyName );
	int EtcValueSaveInt( const char *szTitle, const char *szKeyName, int iValue );

public:
	ioWnd *GetPreOverWnd(){ return m_pPreOverWnd; }
	ioWnd *GetPreClickWnd(){ return m_pPreClickWnd; }

public:
	void MoveZorderLayer( DWORD dwWndID, DWORD dwMoveID );       // dwWndID 뒤에 dwMoveID를 위치 시킨다.
	void MoveZorderTop( DWORD id );	// ZOrder를 제일 위로 올린다.
	void SetModal( ioWnd* wnd );

public:
	void SetSkipUIByBlind( bool bNoRender ) { m_bSkipUIByBlind = bNoRender; }
	void SetNoneUIMode( bool bNoneUI );
	void ShowReserveMsgBox( int iReserveMsgBox = -1 );
	void ShowReserveUI();
	void SetPenaltyPeso( int iPeso ) { m_iPenaltyPeso = iPeso; }
	void SetPenaltyLadderPoint( int iLadderPoint ){ m_iPenaltyLadderPoint = iLadderPoint; }

	inline bool IsSkipUIByBlind() const { return m_bSkipUIByBlind; }	
	inline bool IsNoneUIMode() const { return m_bNoneUIMode; }
	
public:
	bool IsUIKeyBlock();
	bool IsDeskTopOver() const;
	bool IsCurrentOver( DWORD dwID ) const;
	bool IsShowSettingKeyWnd();
	bool IsDragMove( DWORD dwID ) const;

	void CheckShopSound( bool bEnable );
	bool IsCanShopSoundForField();

public:
	void SetUIPostionToDefault();

	void ShowPartyUICmd();
	void HidePartyUICmd();

public:
	void ShowLoginPopup( ioHashString szName );
	void ShowBestFriendPopup( ioHashString szName, bool bLogin );
	void SetPlayStage( ioPlayStage *pPlayStage ){ m_pPlayStage = pPlayStage; }
	ioPlayStage* GetPlayStage(){ return m_pPlayStage; }

public:   // Child Wnd Show ID List Log
	void ChildWndShowIDList();	

public:
	virtual void NotifyLoadWnd( ioWnd* pWnd );
	virtual void NotifyLoadFrame( ioUIRenderFrame* pFrame, const ioHashString& szType, ioWnd* pWnd );
	virtual void NotifyUIRenderImage( ioUIRenderImage* pImage, const ioHashString& szType, ioWnd* pWnd );

public:
	void DestroyBoundary();
	WndBoundaryContainer* GetBoundaryContainer( DWORD dwIndex );
	void RenderBoundary();

public:
	static ioSP2GUIManager& GetSingleton();

public:
	ioSP2GUIManager();
	virtual ~ioSP2GUIManager();
};

#define g_GUIMgr ioSP2GUIManager::GetSingleton()