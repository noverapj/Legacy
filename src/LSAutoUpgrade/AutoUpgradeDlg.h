// AutoUpgradeDlg.h : header file
//

#if !defined(AFX_AUTOUPGRADEDLG_H__0FBFC81C_40E3_4498_B087_A9EF3536B687__INCLUDED_)
#define AFX_AUTOUPGRADEDLG_H__0FBFC81C_40E3_4498_B087_A9EF3536B687__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HTTPManager.h"
#include "skin/CDialogSK.h"
#include "skin/HoverButton.h"
#include "skin/ioProgressBar.h"
#include "skin/CxStatic.h"
#include "ioErrorDlg.h"
#include "ioDaumDlg.h"
#include "ioLoginDlg.h"
#include "ioOTPDlg.h"
#include "WaitingDlg.h"
#include "skin/WebBrowser.h"
/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeDlg dialog

class ioClientUDPSocket;

#define TIME_ID_LAUNCHER				1  
#define TIME_ID_EXIT					2 
#define TIME_ID_CLIENT_EXIT				3
#define TIME_ID_SECONDS_CNT				4
#define TIME_ID_WAIT_OPTIMIZE			5
#define TIME_ID_WAIT_START				6
#define TIME_ID_STEAM_LAUNCHER_CHECK    7

#define CLIENT_FILE_NAME "%s\\lostsaga.exe"
#ifdef KAMU_EAC
	#define LAUNCHER_FILE_NAME "%s\\launch_game.exe"
#else
	#define LAUNCHER_FILE_NAME "%s\\autoluncher.exe"
#endif
#define CMD_RESTART_ADD_SP " RESTART"

#define CMD_SECTION_TOKEN        "?"
#define CMD_EXPAND_SECTION_TOKEN ","

#define WAIT_TIME_OPTIMIZE 3000
#define WAIT_TIME_START    30000

#define PARAMETER_VERSION  10201

//----------------Client와 통신------------
#define WM_MY_MSG_A_EXIT       (WM_USER + 300)
#define WM_MY_MSG_C_CONNECT    (WM_USER + 301)
#define WM_MY_MSG_C_CONNECT_OK (WM_USER + 302)
#define WM_MY_MSG_C_ERROR      (WM_USER + 303)
#define WM_MY_MSG_C_SUCCESS    (WM_USER + 304)
#define WM_MY_MSG_C_IP_BLOCK   (WM_USER + 305)  // client
#define WM_FB_DATA             (WM_USER + 401)

class CAutoUpgradeDlg : public CDialogSK
{
// Construction
public:
	CAutoUpgradeDlg(CWnd* pParent = NULL);	// standard constructor

public:
	// Dialog Data
	//{{AFX_DATA(CAutoUpgradeDlg)
	enum { IDD = IDD_AUTOUPGRADE_DIALOG }; // 이열거형 보다 내가 정의한 열거형이 위로 오면 변수 생성 마법사에서 오류 발생
	//}}AFX_DATA
	enum APS_DONE_STEP
	{
		APS_FIREWALL = 1, APS_RUNINIT, ASP_LOGIN_CONNECT, ASP_PATCH_INFO, ASP_CHK_HDD, ASP_PATCH, ASP_PUSH_STARTBTN, ASP_EXECUTE_LOSA
	};
private:
	enum OPTION_TYPE
	{
		OT_SINGLE_MODE            = 0,
		OT_RESOURCE_UPDATE        = 1,
		OT_READ_PAC               = 2,
		OT_EXTRACT_PAC            = 3,
		OT_GAME_MODE              = 4,
		OT_FULLSCREEN             = 5,
		OT_DEBUG_SHOW_BOUND_BOX   = 6,
		OT_DEBUG_SHOW_EXTRA_INFO  = 7,
		OT_DEBUG_MEMORY_USAGE     = 8,
		OT_SHOW_CONSOLWINDOW      = 9,
		OT_VISUAL_STUDIO_DEBUG    = 10,
		OT_ADMIN_FOLDER_COPY      = 11,
		OT_GAME_MODE_SUB          = 12,
		OT_GAME_MODE_MAP          = 13,
		OT_MAX
	};

	enum CMD_TYPE
	{
		CT_EXPAND_VALUE       = 0,
		CT_VCODE			  = 1,	
		CT_OPTIONS			  = 2,
		CT_FULLZIPVER		  = 4,
		CT_COPY_FOLDER_SOURCE = 5,
		CT_COPY_FOLDER_DEST   = 6,
		CT_MAX,
	};

	CString        m_szCFSourceFolder;
	CString        m_szCFDestFolder;
	CString        m_szFullZipVer;
	int            m_iMaxGarbageMega;
	int            m_iManualMaxOptimizeMega;
	CString        m_szININame;
	
	HTTPManager    *m_HTTPMANAGER;
	char            m_szRootDir[MAX_PATH];	

	CHoverButton    m_XButton;
	CHoverButton    m_OptimizeBtn;
	CHoverButton    m_StartButton;
	CHoverButton	m_MigrationButtonL;
	CHoverButton	m_MigrationButtonR;
	DWORD           m_dwStartTime;

	bool            m_bExtractPac;
	bool            m_bVSDebug;
	bool            m_bCopyFolder;
	bool            m_bPieceFileUpdate;

	CString         m_szNoticeText;
	int             m_iDisplaySeconds;
	ioErrorDlg      m_ErrorDlg;
	ioDaumDlg       m_DaumDlg;
	ioLoginDlg      m_LoginDlg;
	ioExplorerDlg	m_ExplrDlg;
	ioOTPDlg        m_OTPDlg;
	CWaitingDlg		*m_waitingDlg;
	ioClientUDPSocket	*m_pUDPSocket;
	CString         m_szLogServerIP;
	int             m_iPort;
	CString			m_szLauncherURL;
	CString			m_szStartINIURL;

	CExplorer1      m_WebBrowser;

private:
	void            ParseCmd( IN const char *szCmdLine, OUT CString &szOptinosOnCmd );
	void            CentreWindowAndTopMost();
	bool            EndDialogWithSendMsg();

	void			SetText(const char *pText, int iCnt);
	void            ParseExpandValue( const char *szExpandVelue);
	void            InitSocket();
	void			LoadINI();
	void            SendSavedLog();
	BOOL			m_bStartBtn;
	BOOL			m_bExit;
	int				m_iAutoProcStep;
#ifdef STEAM_ATTACH
	int				m_iSteamProc;
#endif
	void			LoadExplorerDlg();

public:

	CString			m_szOGPKey;
	char			m_szBackUrl[MAX_PATH];
	const char*		GetBackGroundURL() const { return m_szBackUrl; }

	ioProgressBar   m_ProgressBar;
	CxStatic        m_Static;

	bool            IsExtractPac() const { return m_bExtractPac; }
	bool			IsVSDebug() const { return m_bVSDebug; }
	bool            IsCopyFolder() const { return m_bCopyFolder; }
	bool			IsPieceFileUpdate() const { return m_bPieceFileUpdate; }

	DWORD           GetStartTime() const { return m_dwStartTime; }
	void            ExecuteProcess(char* szFileName , char* szCmdLine);
	BOOL            RunOptionWithNoUI();
	void			GenerateParameter(char* original, char* destination, const int length);

	const CString&  GetCFSourceFolder() const { return m_szCFSourceFolder; }
	const CString&  GetCFDestFolder() const { return m_szCFDestFolder; }
	const CString&  GetCmdFullZipVer() const { return m_szFullZipVer; }
	const CString&  GetININame() const { return m_szININame; }
	const char*     GetRootDir() const { return m_szRootDir; }

	void			MinimizeAllWindows(BOOL bMinimize);
	void            SetWantEndDialog();
	void 			DeleteClientDirectorys( const char *szFolder );

	int				GetMaxGarbageMega() const { return m_iMaxGarbageMega; }
	int             GetManualMaxOptimizeMega() const { return m_iManualMaxOptimizeMega; }

	void			ShowErrorDlg( const char* szErrorText, int iErrorNumber, bool bExitProgram = true);
	void			SendLOG( const char *szLog, int iErrorNumber , bool bUpgradeError = true);

	BOOL			IsOptimizeChecked() const;
	BOOL			IsWaitingDlgError();

	void            ShowStartBtn();
	void			SetStartButton(bool bSet);
	void			SetMigrationButton(bool bSet);
	void			SetOptimizeBtnDisable();
	void            ShowLogin();
	bool            IsLoginOk();
	void            SetLoginTimer();
	void            SetLoginNetworkState( NetworkState eNetworkState );
	void            ShowOTP();
	bool            IsOTPOk();
	void            SetOTPNetworkState( NetworkState eNetworkState );
	void            ResumeHttpManager();
	void			SetMigrationNetworkState(NetworkState eNetworkState);

	void            LoginDlgRecvPacket( SP2Packet &rkPacket );
	void            OTPDlgRecvPacket( SP2Packet &rkPacket );
	void            BalancerRecvPacket( SP2Packet &rkPacket );
	void            MigrationDlgRecvPacket( SP2Packet &rkPacket );
	void			SetAutoupStep( int iStep ) { m_iAutoProcStep = iStep; };

	__int64         GetGameServerID();
	BOOL            IsGameServerID();
	void            GetStrGameServerID( string &rsGameServerID );

	bool			CheckParamVersion( char* param );

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoUpgradeDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CAutoUpgradeDlg)
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnExit();
	afx_msg void OnStart();
	afx_msg void OnMigration();
	afx_msg void OnMigrationR();
	afx_msg void OnTimer(UINT nIDEvent);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	DECLARE_EVENTSINK_MAP()
	void DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL);
	void BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
	void GetID_PlayPark(LPDISPATCH pDisp);
	void GetID_AsiaSoft(LPDISPATCH pDisp);
	void GetID_PlayCyberGames(LPDISPATCH pDisp);
private:
	int	m_iClientMsg;

#ifdef STEAM_ATTACH
public :
	bool CheckLauncher();
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPGRADEDLG_H__0FBFC81C_40E3_4498_B087_A9EF3536B687__INCLUDED_)
