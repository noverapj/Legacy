#pragma once

class ioWebBrowser;

class ioBrowserManager : public Singleton< ioBrowserManager >
{
protected:
	typedef std::map< HWND, ioWebBrowser* > WebBrowserMap;
	WebBrowserMap m_WebBrowserMap;

	HINSTANCE m_hInstance;
	HWND m_hClient;

	ioHashString m_HeadQuaterURL;
	ioHashString m_HeadquartersMiniURL;
	ioHashString m_AutoLoginURL;
	ioHashString m_FormalityEntryURL;
	ioHashString m_GuildHomePageURL;
	ioHashString m_GuildMarkPageURL;
	ioHashString m_ChannelingURL;
	ioHashString m_CashURL;
	ioHashString m_CashEntryURL;
	ioHashString m_ProbabilityURL;

	ioHashString m_szKeyOnCmd;
	DWORD m_dwRefreshTime;

	HBITMAP m_hWebTitleHome;
	HBITMAP m_hWebTitleEntry;
	HBITMAP m_hWebTitleGuild;
	HBITMAP m_hWebTitleGuildMark;
	HBITMAP m_hWebTitleCash;
	HBITMAP m_hWebOutLine;
	HBITMAP m_hWebExit;

	int m_nBrowserSingleExtraWidth;
	int m_nBrowserSingleExtraHeight;


	//¿±≈¬√∂
	DWORD m_dwSeeProcess;

	DWORD m_dwSendCashInfoTime;

public:
	static ioBrowserManager& GetSingleton();

public:
	void Initialize( HINSTANCE hInstance, HWND hClient, const ioHashString &szKeyOnCmd );
	void Process();
	void LoadINI( const char *szININame );

	ioHashString GetAutoLoginUrl();

public:
	ioWebBrowser* OpenBrowser( const char *szURL,
							   BrowsePageType ePageType = BROWSE_DEFAULT,
							   BrowsePageSubType ePageSubType = BROWSE_SUB_DEFAULT,
							   bool bNeedLogin = true, bool bNeedExcuteWeb = false );

	ioWebBrowser* FindBrowser( HWND hParent );
	ioWebBrowser* FindBrowserByPageType( BrowsePageType ePageType );

	void CheckDeviceModeAndMouse();

public:
	void WebParentClicked( HWND hWnd );
	void WebParentClickOff( HWND hWnd );

public:
	bool CloseBrowser( HWND hWnd );
	bool CloseBrowserByType( BrowsePageType eType );
	bool HasFocusInBrowser( HWND hFocus );
	// ¿”Ω√ ∑∫±« by Minyeong
	void CloseAllBrowser();
	//

	void NotifyDeleteFriendByWeb( const char *szFriendNick );
	void NotifyUserEntryRefresh( const char *szNick );
	void NotifyGuildMarkChangeComplete();
	void NotifyHideProcess( BrowsePageType eType );

	void SetSeeIEProcessID( DWORD dwIEId);

protected:
	ioWebBrowser* OpenNewBrowser( BrowsePageType ePageType );

	void ClearCloseReserved();
	void HideBrowserByEscape();
	void RefreshLogin();
	void ProcessWebParentDrag();

	void ProcessSeeIEProcess();

public:
	void DrawWebParent( HWND hWnd, HDC hDC, RECT rcClient );
	void DrawExitButton( HDC hButtonDC, int iItemState );

public:
	inline const ioHashString& GetKeyOnCmd() const { return m_szKeyOnCmd; }
	inline const ioHashString& GetHeadQuaterURL() const { return m_HeadQuaterURL; }
	inline const ioHashString& GetHeadquartersMiniURL() const { return m_HeadquartersMiniURL; }
	inline const ioHashString& GetFormalityEntryURL() const { return m_FormalityEntryURL; }
	inline const ioHashString& GetGuildHomePageURL() const { return m_GuildHomePageURL; }
	inline const ioHashString& GetGuildMarkPageURL() const { return m_GuildMarkPageURL; }
	inline const ioHashString& GetChannelingURL() const { return m_ChannelingURL; }
	inline const ioHashString& GetCashURL() const { return m_CashURL; }
	inline const ioHashString& GetCashEntryURL() const { return m_CashEntryURL; }
	inline const ioHashString& GetProbabilityURL() const {return m_ProbabilityURL;}
	inline HINSTANCE GetHInstance() const { return m_hInstance; }
	inline HWND GetClientHwnd() const { return m_hClient; }
	

public:
	int  GetBrowserCount() const;
	bool IsWebOpened() const;

	bool OnPreMessage( MSG &msg );

public:
	int GetBrowserSingleExtraWidth();
	int GetBrowserSingleExtraHeight();

public:
	ioBrowserManager();
	virtual ~ioBrowserManager();
};

#define g_BrowserMgr ioBrowserManager::GetSingleton()

