#pragma once


// ioLoginDlg 대화 상자입니다.
#include "skin/CDialogSK.h"
#include "skin/HoverButton.h"
#include "skin/CxStatic.h"
#include "afxwin.h"
#include "NetWork/SP2Packet.h"
#include "StringManager/ioHashString.h"
#include "skin/webbrowser.h"

#define IDEDIT_BLANK_TEXT    "ID"
#define PWEDIT_BLANK_TEXT    "Password"
#define TIME_ID_LOGIN        200
#define MAX_LOGIN_TIME_SEC   180
#define MAX_CONNECT_TIME_SEC 60
#define FACEBOOK_FILE_NAME   "%s\\lsfblogin.exe"
#define FACEBOOK_LOGIN_CMD   " RfeoEGF4093SEDeldo22" // cmd 넣기 위해서 공백 추가

typedef std::vector< ioHashString > ioHashStringVec;

class ioClientSocket;
class ioLoginDlg : public CDialogSK
{
public:
	ioLoginDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ioLoginDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOGINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	CHoverButton m_XBtn;
	CHoverButton m_IDSaveBtn;
	CHoverButton m_LoginBtn;
	CHoverButton m_EntryBtn;
	CHoverButton m_FindIDBtn;
	CHoverButton m_FindPWBtn;
	CEdit m_IDEdit;
	CEdit m_PWEdit;
	CComboBox m_ComboBox;
	CxStatic  m_Static;

	CBrush       m_EditSelectBGBrush;
	COLORREF     m_EditSelectBGColor;

	CBrush       m_EditNormalBGBrush;
	COLORREF     m_EditNormalBGColor;

	COLORREF     m_EditTextColor;
	COLORREF     m_EditNoticeTextColor;

	CFont        m_Font;
	CFont        m_FontStatic;
	CBitmap      m_IDEditNormalBitmap;
	CBitmap      m_IDEditSelectBitmap;

	CBitmap      m_PWEditNormalBitmap;
	CBitmap      m_PWEditSelectBitmap;

	CString      m_sEntryURL;
	CString      m_sFindIDURL;
	CString      m_sFindPWURL;
	bool         m_bIDEditBlank;
	bool         m_bPWEditBlank;
	bool         m_bSelectIDEdit;
	bool         m_bSelectPWEdit;

	ioClientSocket   *m_pClientSocket;

	bool         m_bLoginOk;
	int          m_iConnectTimerCnt;
	int          m_iTotalConnectTimerCnt;
	NetworkState m_eNetworkState;

	ioHashStringVec m_vServerIPPort;
	int             m_iServerIPPortIdx;

	ioHashString    m_sServerNumber;
	CString         m_sNoticeText;
		 
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoginIdsave();
	afx_msg void OnBnClickedLoginX();
	afx_msg void OnBnClickedLoginEnter();
	afx_msg void OnBnClickedLoginEntry();
	afx_msg void OnBnClickedLoginFind();
	afx_msg void OnBnClickedLoginFindId();
	afx_msg void OnBnClickedLoginFindPw();
	afx_msg void OnCbnSelchangeDomain();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg LONG OnClientSocket(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

public:

	void LoadBackImage();

	void LoadINI();
	void FirstConnectServer();
	void RecvPacket( SP2Packet &rkPacket );
	bool IsLoginOk() const { return m_bLoginOk; }
	void SetNetworkState( NetworkState eNetworkState ) { m_eNetworkState = eNetworkState; }
	void SetTextNetworkConnected( NetworkState eNetworkState );
	void SetLoginTimer();

protected:
	void DrawBitmap( CPaintDC *pDC, CBitmap *pBitmap, int iX, int iY, int iWidth, int iHeight );
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	bool SendServer( const char *szID, const char *szPW );
	void GetMyIP( OUT char *szMyIP, IN int iSize );

	void SetChildsActive( bool bActive );

	void LoadServerIPPortList();
	void GetNextServerIPPort( OUT char *szServerIP, IN int iServerIPSize, OUT int &riServerPort );
	void ConnectServer( const char *szServerIP, int iServerPort );

	void SetServerNumber( const char *szServerIP );
	void SetText( const char *pText );
public:
	
	DECLARE_EVENTSINK_MAP()
	void DocumentCompleteExplorer2(LPDISPATCH pDisp, VARIANT* URL);

	CExplorer1 m_webBackImage;
};

class ioExplorerDlg : public CDialogSK
{
#define MAX_ID_LENGTH 42
public:
	ioExplorerDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ioExplorerDlg();

	enum { IDD = IDD_EXPLORER_DLG };
	
	enum {	MIG_NO_DONE				= 1,
			MIG_SUCCESS				= 0, 
			MIG_NO_DBDATA			= -1, 
			MIG_USR_INDEX_ERR		= -2, 
			MIG_GARENA_ALREADY_DONE	= -3, 
			MIG_ASIA_ALREADY_DONE	= -4, 
			MIG_ACC_INFO_EMPTY		= -28 }; //Web migration infomation error
	enum 
	{ 
		SITE_ASIASOFT = 0,
		SITE_PLAYPARK,
		SITE_TCG,
		MAX_SITE
	};

	CComboBox m_ComboBox;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LONG OnClientSocket(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCbnSelchangeDomain();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnExpClickedExitBtn();
public:
	virtual BOOL OnInitDialog();
	CHoverButton m_XBtn;
	CHoverButton m_ConfirmBtn;

public:
	void FirstConnectServer();
	void RecvPacket( SP2Packet &rkPacket );
	void SetNetworkState( NetworkState eNetworkState ) { m_eNetworkState = eNetworkState; }
	void SetLoginTimer();

protected:
	bool SendServer();
	void GetMyIP( OUT char *szMyIP, IN int iSize );

	void LoadServerIPPortList();
	void GetNextServerIPPort( OUT char *szServerIP, IN int iServerIPSize, OUT int &riServerPort );
	void ConnectServer( const char *szServerIP, int iServerPort );

	void SetServerNumber( const char *szServerIP );
	
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	ioClientSocket   *m_pClientSocket;

	bool         m_bLoginOk;
	int          m_iConnectTimerCnt;
	int          m_iTotalConnectTimerCnt;
	NetworkState m_eNetworkState;

	ioHashStringVec m_vServerIPPort;
	int             m_iServerIPPortIdx;

	ioHashString    m_sServerNumber;

	struct STAsiaDomain 
	{
		bool bSelect;
		bool bSuccess;
		CString szHome;
		CString szRet;
		CString szID;
	};

public:
	DECLARE_EVENTSINK_MAP()
	void DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL);
	void BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
	void GetID_AsiaSoft(LPDISPATCH pDisp, CString &szID );
	void GetID_PlayPark(LPDISPATCH pDisp, CString &szID );
	void GetID_PlayCyberGames(LPDISPATCH pDisp, CString &szID );

	CExplorer1 m_webBackImage;

	STAsiaDomain m_stDomain[MAX_SITE];

	afx_msg void OnClose();
	afx_msg void OnExit();
	afx_msg void OnBnClickedBtnOk();
private:
	int m_iMigration;
	typedef std::vector<CString> vStrID;
	vStrID m_vecStrID;
	
};