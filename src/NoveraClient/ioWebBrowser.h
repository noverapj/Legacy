#pragma once

#include "iepmapi.h"

class ioBrowserEvents;
class ioBrowserManager;

class ioWebBrowser
{
protected:
	ioBrowserManager *m_pCreator;
	BrowsePageType m_BrowsePageType;
	BrowsePageSubType m_BrowsePageSubType;

	HWND m_hWebParent;
	HWND m_hExplorer;

	ioBrowserEvents *m_pInvokeEvent;
	DWORD m_dwAdviceCookie;

protected:
	enum LoginState
	{
		LS_NOT_LOGIN,
		LS_LOGINNING,
		LS_LOGIN_DONE,
	};
	LoginState m_LoginState;
	ioHashString m_ReservedURL;

protected:
	POINT m_ptPreCursor;
	bool  m_bWindowDrag;

protected:
	bool m_bCloseReserved;
	bool m_bShow;

protected:
	bool m_bHideProcess;
	bool m_nexonWindowClosed;

protected:
	int m_nIEhPosX;
	int m_nIEhPosY;
	int m_nIEhSizeX;
	int m_nIEhSizeY;

	bool m_bNeedExcuteWeb;
	bool m_bNoCloseXTrap;

public:
	typedef CComPtr<IUnknown> UnknownComPtr;
	typedef CComPtr<IWebBrowser2> WebBrowser2ComPtr;

public:
	bool Initialize( HWND hClient, HINSTANCE hInstance );
	void Release();

public:
	void WebParentDragStart();
	void WebParentDragEnd();
	void ProcessDrag();

	void SetNeedExcuteWeb(bool bNeed);

public:
	void ShowBrowser( bool bShow );
	void ReserveClose();

	void NavigateWithLogin( const char *szLoginURL, const char *szURL );
	void Navigate( const char *szURL );
	void Refresh();

	void SetBodyNoScroll();

public:
	void GetWebWindowRect( RECT *pRect );
	
	bool IsAutoLoginTime() const;
	bool IsFocusInBrowser( HWND hFocus ) const;

	inline bool IsShow() const { return m_bShow; }

	inline void SetBrowsePageType( BrowsePageType ePageType ) { m_BrowsePageType = ePageType; }
	inline BrowsePageType GetBrowsePageType() const { return m_BrowsePageType; }

	inline void SetBrowsePageSubType( BrowsePageSubType ePageSubType ){ m_BrowsePageSubType = ePageSubType; }
	inline BrowsePageSubType GetBrowsePageSubType() const { return m_BrowsePageSubType; }

	inline bool IsCloseReserved() const { return m_bCloseReserved; }
	inline HWND GetParentHWND() const { return m_hWebParent; }

public:
	void OnBeforeNavigate2( IDispatch *pDisp,
							VARIANT *url,
							VARIANT *Flags,
							VARIANT *TargetFrameName,
							VARIANT *PostData,
							VARIANT *Headers,
							VARIANT_BOOL * Cancel );

	void OnNavigateComplete2(IDispatch *pDisp, VARIANT *URL);
	void OnTitleChange( BSTR szTitle );
	void OnClientToHostWindow( long *CX,long *CY );
	void OnNewWindow( IDispatch **ppDisp, VARIANT_BOOL *pCancel );
	void OnDocumentComplete( IDispatch *pDisp, VARIANT *URL );
	void OnWindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel);

protected:
	// MFC의 Afx...()를 복사해온것임
	bool ConnectionAdvise( LPUNKNOWN pUnkSrc, REFIID iid,LPUNKNOWN pUnkSink, BOOL bRefCount, DWORD* pdwCookie );
	bool ConnectionUnadvise( LPUNKNOWN pUnkSrc, REFIID iid,LPUNKNOWN pUnkSink, BOOL bRefCount, DWORD dwCookie );

protected:
	void GetWindowSizePos(OUT int &riWidth,OUT int &riHeight,OUT int &riXPos,OUT int &riYPos );
	void SuppressAlerts(); //UJ 131007, 렉권방지: alert창, 각종 에러메세지 띄우지 않음
	void SetNoScroll(); // Scroll bar 미생성.
	void HideProcess();

public:
	WebBrowser2ComPtr GetWebBrowser2ComPtr();
	bool IsDownLoadComplete();

public:
	ioWebBrowser( ioBrowserManager *pCreator, BrowsePageType ePageType );
	virtual ~ioWebBrowser();
};

