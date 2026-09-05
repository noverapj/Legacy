// AutoUpgradeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "AutoUpgradeDlg.h"
#include "ThreadModules/ThreadManager.h"
#include <mmsystem.h>
#include "ioINILoaderAU.h"
#include <shldisp.h> // void MinimizeAllWindows(BOOL bMinimize)
#include ".\autoupgradedlg.h"
#include "HelpFunc.h"
#include "StringManager/safesprintf.h"
#include "StringManager/ioStringManager.h"
#include "NetWork/ioClientUDPSocket.h"
#include "NetWork/SP2Packet.h"
#include "NetWork/Protocol.h"
#ifdef XTRAP
#include "Xtrap/ioXtrap.h"
#endif
#include <strsafe.h>
#include "Local/ioLocalParent.h"
#include "channeling/iochannelingnodemanager.h"
#include "Channeling/ioChannelingNodeParent.h"
#include <exdisp.h>
#include <mshtmlc.h>
#include "Local/ioLocalUS.h"
#include "Local/ioLocalBrazil.h"
#include "Local/ioLocalThailand.h"
#include <comdef.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL tokenize(const std::string& str, const std::string& delimiters, std::vector<std::string>& tokens)
{
	tokens.clear();

	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	std::string token;
	while (string::npos != pos || string::npos != lastPos)
	{
		token = str.substr(lastPos, pos - lastPos);
		tokens.push_back(token);

		lastPos = str.find_first_not_of(delimiters, pos);

		pos = str.find_first_of(delimiters, lastPos);
	}
	return (tokens.size() > 0) ? TRUE : FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeDlg dialog

CAutoUpgradeDlg::CAutoUpgradeDlg(CWnd* pParent /*=NULL*/)
	: CDialogSK(CAutoUpgradeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoUpgradeDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_HTTPMANAGER		= NULL;
	m_waitingDlg		= NULL;

	m_bExtractPac		= false;
	m_bVSDebug			= false;
	m_bCopyFolder		= false;
	m_bPieceFileUpdate	= false;
	m_bStartBtn			= false;
	m_iDisplaySeconds	= 1;
	m_iMaxGarbageMega	= 0;
	m_pUDPSocket		= NULL;
	m_iPort				= 0;
	m_iManualMaxOptimizeMega= 0;
	m_bExit				= false;
	m_iClientMsg		= 0;
	m_iAutoProcStep		= 0;
}

void CAutoUpgradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogSK::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoUpgradeDlg)
	DDX_Control(pDX, IDC_BUTTON1, m_XButton);
	DDX_Control(pDX, IDC_PROGRESSBAR, m_ProgressBar);
	DDX_Control(pDX, IDC_TEXT, m_Static);
	DDX_Control(pDX, IDC_OPTIMIZE_BTN, m_OptimizeBtn);
	DDX_Control(pDX, IDC_START, m_StartButton);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EXPLORER1, m_WebBrowser);
	DDX_Control(pDX, IDC_MIGRATIONL, m_MigrationButtonL);
	DDX_Control(pDX, IDC_MIGRATIONR, m_MigrationButtonR);
}

BEGIN_MESSAGE_MAP(CAutoUpgradeDlg, CDialogSK)
	//{{AFX_MSG_MAP(CAutoUpgradeDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnExit)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_MIGRATIONL, OnMigration)
	ON_BN_CLICKED(IDC_MIGRATIONR, OnMigrationR )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeDlg message handlers

BOOL CAutoUpgradeDlg::OnInitDialog()
{
	CDialogSK::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	srand(timeGetTime()); // 소켓 암호화시에 rand() 사용하기 위해서, 이것을 추가하지 않으면 State Not Same 에러 발생

	// firewall
	::GetCurrentDirectory(MAX_PATH,m_szRootDir);
	char szFullPath[MAX_PATH*2];
	ZeroMemory( szFullPath, sizeof(szFullPath) );
	StringCbPrintf( szFullPath, sizeof(szFullPath), "%s\\autoupgrade.exe", m_szRootDir );
	Help::SetUpWindowsFirewall( szFullPath, L"LostSaga(upgrade)"); //Except Extracting Hangeul
	m_iAutoProcStep = APS_FIREWALL;
	InitSocket();

	int iIDBackBMP     = IDB_BACK;
	int iIDOptimizeBMP = IDB_OPTIMIZE_BTN;
	int iIDStartBMP    = IDB_START;
	int iIDMigrationBMP	= IDB_MIGRATION_L;
	int iIDMigrationBMPR= IDB_MIGRATION_R;

	int iOptimizeBtnWidth = 271;
	int iOptimizeBtnX     = 115;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		iIDBackBMP     = pLocal->GetResourceIDBackBmp();
		iIDOptimizeBMP = pLocal->GetResourceIDOptimizeBtnBmp();
		iIDStartBMP    = pLocal->GetResourceIDStartBtnBmp();
		iOptimizeBtnWidth = pLocal->GetOptimizeBtnWidth();
		iOptimizeBtnX     = pLocal->GetOptimizeBtnX();
		if( pLocal->IsPieceFilePermit() )
			Help::SetPieceFilePermit( true );
		else
			Help::SetPieceFilePermit( false );
	}

	// skin
	EnableEasyMove();                        // enable moving of the dialog by clicking anywhere in the dialog
	SetBitmap(iIDBackBMP);                   // set background bitmap
	SetStyle(LO_RESIZE);                     // resize dialog to the size of the bitmap
	SetTransparentColor(RGB(0, 255, 0)); // set gray as the transparent color

	SetWindowText("LOSTSAGA_AUTOUPGRADE");

	m_XButton.LoadBitmap(IDB_XBTN, TRUE );
	m_XButton.MoveWindow(470, 11, 23, 23);

	m_WebBrowser.MoveWindow( 22, 59, 460, 284 );
#ifdef MIGRATION
 	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND ) //현재 태국
 	{
 		m_MigrationButtonL.LoadBitmap( iIDMigrationBMP, TRUE );
 		m_MigrationButtonL.MoveWindow(23, 59, 228, 280);
 		m_MigrationButtonL.ShowWindow( SW_SHOW );
 
 		m_MigrationButtonR.LoadBitmap( iIDMigrationBMPR, TRUE );
 		m_MigrationButtonR.MoveWindow( 251, 59, 228, 280 );
 		m_MigrationButtonR.ShowWindow( SW_SHOW );
	}
#else
	m_MigrationButtonR.ShowWindow( SW_HIDE );
	m_MigrationButtonL.ShowWindow( SW_HIDE );
#endif

	m_StartButton.LoadBitmap( iIDStartBMP, TRUE );
	m_StartButton.MoveWindow(17, 393, 470, 32);
	m_StartButton.ShowWindow( SW_HIDE );

	m_OptimizeBtn.LoadBitmap( iIDOptimizeBMP );
	m_OptimizeBtn.MoveWindow(iOptimizeBtnX, 414, iOptimizeBtnWidth, 15);
	m_OptimizeBtn.SetCheckBtn( TRUE ); 

	m_ProgressBar.SetBitmap(IDB_PROGRESSBAR, IDB_PROGRESSBAR_BACK, IDC_PROGRESSBAR_OPTIMIZATION);
	m_ProgressBar.MoveWindow(17, 393, 470, 15);

	m_Static.SetBitmap(IDB_TEXTBACK, CxStatic::OriginalSize);
	m_Static.SetTextColor( RGB(58,58,58) );

	char szFontName[MAX_PATH] = "돋움";
	if( pLocal )
		StringCbCopy( szFontName, sizeof( szFontName ), pLocal->GetFontName() );

	m_Static.SetFont( szFontName, 12, FW_BOLD ); //Except Extracting Hangeul
	m_Static.MoveWindow(79, 375, 347, 18);
	m_Static.SetWindowText("");
	//

	CentreWindowAndTopMost();

	CString szDownLoadPath = m_szRootDir;
	szDownLoadPath += "\\losadown";
	CreateDirectory((LPCTSTR)szDownLoadPath,NULL);

	m_ErrorDlg.Create(IDD_IOERRORDLG);
	m_LoginDlg.Create( IDD_LOGINDLG );
	m_OTPDlg.Create( IDD_OTPDLG );

	m_HTTPMANAGER = new HTTPManager;
	if(m_HTTPMANAGER)
		m_HTTPMANAGER->Begin();

	m_dwStartTime = timeGetTime();

	LoadINI();

#ifdef SRC_TH
	// 태국 start.ini 파일 변경을 위한 임시 처리
	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
	{
		if(!m_szStartINIURL.IsEmpty())
		{
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if(pLocal)
			{
				ioLocalThailand * pLocalTH = (ioLocalThailand * )pLocal;
				pLocalTH->DownloadStartIni(m_szStartINIURL, m_HTTPMANAGER);
			}
		}
	}

#endif

	m_LoginDlg.LoadBackImage();

	SendSavedLog();

	// 2012-10-17 youngdie, 로그인서버에 데이타를 가져오고 대기열 처리을 처리함
	m_waitingDlg = new CWaitingDlg(NULL);
	if(m_waitingDlg)
	{
		m_waitingDlg->Create(IDD_DIALOG_WAITING);
		m_waitingDlg->Begin(m_szININame.GetBuffer());
	}


#ifdef _ADMIN_
#ifndef _DEBUG
	m_OptimizeBtn.EnableWindow( FALSE );
#endif
#endif

	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_ChannelingMgr.GetChannelingType() );
	if( pNode && pNode->IsShowDaumWindow( m_szRootDir ) )
	{
		m_DaumDlg.Create( IDD_DAUM_DLG );
		m_DaumDlg.ShowWindow( SW_SHOW );
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoUpgradeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogSK::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoUpgradeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAutoUpgradeDlg::OnClose()  // Alt + F4 방지하기 위해서 빈함수로 
{
	// TODO: Add your message handler code here and/or call default
}

BOOL CAutoUpgradeDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(wParam == IDOK) return FALSE;         //캔슬 방지.
	if(wParam == IDCANCEL) 
	{
		if(EndDialogWithSendMsg())
			return FALSE;

		SetWantEndDialog();
		return FALSE;
	}
	return  CDialogSK::OnCommand(wParam, lParam);
}

void CAutoUpgradeDlg::OnExit() 
{
	// TODO: Add your control notification handler code here
	if(EndDialogWithSendMsg())
		return;

	SetWantEndDialog();
}

void CAutoUpgradeDlg::OnStart() 
{
	// TODO: Add your control notification handler code here
	m_iAutoProcStep = ASP_PUSH_STARTBTN;
	LOG.PrintTimeAndLog(0 , "Compate - OnStart");
	if( m_StartButton.IsWindowVisible() == FALSE )
		return;
	if( m_StartButton.IsWindowEnabled() == FALSE )
		return;
	m_szNoticeText.Empty();
	m_iDisplaySeconds = 1;
	if( m_HTTPMANAGER )
		m_HTTPMANAGER->ResumeRun();
	KillTimer( TIME_ID_WAIT_START );
	SetMigrationButton(FALSE);
	m_StartButton.EnableWindow( FALSE );
	m_XButton.EnableWindow( FALSE );
}

BOOL CAutoUpgradeDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	//ThreadManager::GetInstance()->Clear();
	ThreadManager::ReleaseInstance();

	if(m_HTTPMANAGER)
	{
		delete m_HTTPMANAGER;
		m_HTTPMANAGER = NULL;
	}
	if(m_waitingDlg)
	{
		delete m_waitingDlg;
		m_waitingDlg = NULL;
	}
	if( m_pUDPSocket )
	{
		m_pUDPSocket->CloseUDP();
		delete m_pUDPSocket;
		m_pUDPSocket = NULL;
	}
	ioPortalHelp::CloseWinSock();
#ifdef XTRAP
	ioXtrap::ReleaseInstance();
#endif

	KillTimer(TIME_ID_SECONDS_CNT);
	KillTimer( TIME_ID_WAIT_OPTIMIZE );
	KillTimer( TIME_ID_WAIT_START);
	return CDialogSK::DestroyWindow();
}

void CAutoUpgradeDlg::DeleteClientDirectorys( const char *szFolder )
{
	if( szFolder == NULL )
		return;

	//디렉토리 삭제.
	char delName[MAX_PATH] = "";
	StringCbPrintf(delName, sizeof(delName), "%s\\%s\\*.*",m_szRootDir, szFolder );
	SHFILEOPSTRUCT FileOp = {0};	
	FileOp.hwnd = NULL;	
	FileOp.wFunc = FO_DELETE;	
	FileOp.pFrom = delName;	
	FileOp.pTo = NULL;	
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI;	
	FileOp.fAnyOperationsAborted = false;	
	FileOp.hNameMappings = NULL;	
	FileOp.lpszProgressTitle = NULL;	
	::SHFileOperation(&FileOp);	
}


void CAutoUpgradeDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case TIME_ID_LAUNCHER:
		{
			KillTimer(TIME_ID_LAUNCHER);

			char exeName[MAX_PATH] = "";
			StringCbPrintf(exeName, sizeof(exeName), LAUNCHER_FILE_NAME, m_szRootDir);

			char szCmdLine[MAX_PATH*2] = "";
			StringCbPrintf(szCmdLine, sizeof(szCmdLine), " %s", AfxGetApp()->m_lpCmdLine);
			ExecuteProcess(exeName, szCmdLine);
			LOG.PrintTimeAndLog(0, "Complete - OnTimer - EndDialog - TIME_ID_LAUNCHER");
			EndDialog(0);	
		}
		break;
	case TIME_ID_EXIT:	
		{
			if( !m_HTTPMANAGER 
				||(m_HTTPMANAGER && !m_HTTPMANAGER->IsRunThread()) )
			{
				LOG.PrintTimeAndLog(0, "Complete - OnTimer - EndDialog - TIME_ID_EXIT");
				KillTimer(TIME_ID_EXIT);
				EndDialog(0);
			}
		}
		break;
	case TIME_ID_CLIENT_EXIT:
		{
			LOG.PrintTimeAndLog(0, "Complete - OnTimer - EndDialog - TIME_ID_CLIENT_EXIT");
			KillTimer(TIME_ID_CLIENT_EXIT);
			EndDialog(0);
		}
		break;
	case TIME_ID_SECONDS_CNT:
		{
			if(m_szNoticeText.IsEmpty())
				m_szNoticeText = STR(1);

			SetText((LPCTSTR)m_szNoticeText, m_iDisplaySeconds++);
#ifdef MIGRATION
			//migration 도중 게임서버 응답이 없는 경우 메시지 창
			if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND)
			{
				if(m_iDisplaySeconds > 100 && !m_bExit ) //TH Migration //100초 응답 없으면 
				{
					m_bExit = true;
					MessageBox("Game server is under checking", "Sorry", MB_OK);;
					LOG.PrintTimeAndLog(0, "Complete - OnTimer - Game Server is under checking");
					KillTimer(TIME_ID_SECONDS_CNT);
					OnExit();
				}
			}
#endif
			enum { MAX_LIVE_SECOND = 600, CHECK_CLIENT_SECOND = 300, };
			if(m_iDisplaySeconds > MAX_LIVE_SECOND ) // 10분을 넘으면 종료함.
			{
				ShowErrorDlg(STR(2),210);
				m_szNoticeText = STR(3);
				SetText((LPCTSTR)m_szNoticeText, m_iDisplaySeconds);
				LOG.PrintTimeAndLog(0, "Complete - OnTimer - Auto Exit");
				KillTimer(TIME_ID_SECONDS_CNT);
				SendLOG( "Fail connection to gameserver", 210);
			}
			if( m_iDisplaySeconds > CHECK_CLIENT_SECOND )
			{
				static bool bRun = false;
				if( !bRun )
				{
					bRun = true;
					HWND hWnd = ::FindWindow(CLIENT_CLASS_NAME, NULL);
					if( hWnd == NULL )
					{
						ShowErrorDlg(STR(8),226);
						m_szNoticeText = STR(9);
						SetText((LPCTSTR)m_szNoticeText, m_iDisplaySeconds);
						LOG.PrintTimeAndLog(0, "Complete - OnTimer - Check Client Exit");
						KillTimer(TIME_ID_SECONDS_CNT);
						if( m_iClientMsg != 0 )
						{
							char szText[MAX_PATH] = "Fail Client Run";
							//ClientSnedMsg
							if( WM_MY_MSG_A_EXIT == m_iClientMsg)				strcat(szText, " | LastClientMsg(WM_MY_MSG_A_EXIT) | DelayTime : ");
							else if( WM_MY_MSG_C_CONNECT == m_iClientMsg)		strcat(szText, " | LastClientMsg(WM_MY_MSG_C_CONNECT) | DelayTime : ");
							else if( WM_MY_MSG_C_CONNECT_OK == m_iClientMsg)	strcat(szText, " | LastClientMsg(WM_MY_MSG_C_CONNECT_OK) | DelayTime : ");
							else if( WM_MY_MSG_C_ERROR == m_iClientMsg)			strcat(szText, " | LastClientMsg(WM_MY_MSG_C_ERROR) | DelayTime : ");
							else if( WM_MY_MSG_C_SUCCESS == m_iClientMsg)		strcat(szText, " | LastClientMsg(WM_MY_MSG_C_SUCCESS) | DelayTime : ");
							else if( WM_MY_MSG_C_IP_BLOCK == m_iClientMsg)		strcat(szText, " | LastClientMsg(WM_MY_MSG_C_IP_BLOCK) | DelayTime : ");
							else if( WM_FB_DATA == m_iClientMsg)				strcat(szText, " | LastClientMsg(WM_FB_DATA) | DelayTime : ");
							char szDisplaySeconds[MAX_PATH] = "";
							_itoa( m_iDisplaySeconds, szDisplaySeconds, 10);
							strcat(szText, szDisplaySeconds );
							SendLOG(szText, 226);
						}
						else
						{
							SendLOG( "Fail Client Run.", 226);
						}
					}
				}
			}
		}
		break;
	case TIME_ID_WAIT_OPTIMIZE:
		{
			int iReverseSeconds = ( WAIT_TIME_OPTIMIZE/1000 ) - m_iDisplaySeconds;
			if(m_szNoticeText.IsEmpty())
				m_szNoticeText = STR(4);

			CString szCurHelp;
			GetDlgItemText(IDC_TEXT, szCurHelp );
			if( szCurHelp.Find( STR(5)) != -1)
				SetText( (LPCTSTR) m_szNoticeText, iReverseSeconds );

			m_iDisplaySeconds++;
			if( m_iDisplaySeconds > WAIT_TIME_OPTIMIZE/1000 )
			{
				m_szNoticeText.Empty();
				m_iDisplaySeconds = 1;
				if( m_HTTPMANAGER )
					m_HTTPMANAGER->ResumeRun();
				KillTimer( TIME_ID_WAIT_OPTIMIZE );
			}
		}
		break;
	case TIME_ID_WAIT_START:
		{
			int iReverseSeconds = ( WAIT_TIME_START/1000 ) - m_iDisplaySeconds;
			if(m_szNoticeText.IsEmpty())
				m_szNoticeText = STR(6);

			CString szCurHelp;
			GetDlgItemText(IDC_TEXT, szCurHelp );
			if( szCurHelp.Find( STR(7)) != -1)
				SetText( (LPCTSTR) m_szNoticeText, iReverseSeconds );

			m_iDisplaySeconds++;
			if( m_iDisplaySeconds > WAIT_TIME_START/1000 )
			{
				m_szNoticeText.Empty();
				m_iDisplaySeconds = 1;
				if( m_HTTPMANAGER )
					m_HTTPMANAGER->ResumeRun();
				KillTimer( TIME_ID_WAIT_START );
				m_StartButton.EnableWindow( FALSE );
				m_XButton.EnableWindow( FALSE );
			}
		}
		break; 
	case TIME_ID_STEAM_LAUNCHER_CHECK:
		{
#ifdef STEAM_ATTACH
			KillTimer(TIME_ID_STEAM_LAUNCHER_CHECK);
			if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US )
			{
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if(pLocal)
				{
					ioLocalUS * pUSLocal = (ioLocalUS * )pLocal;
					if(pUSLocal->DownloadLauncher(m_szLauncherURL, m_HTTPMANAGER))
					{
					}
					PostQuitMessage(0);
				}
			}
			else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_BRAZIL )
			{
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if(pLocal)
				{
					ioLocalBrazil * pBRLocal = (ioLocalBrazil * )pLocal;
					if(pBRLocal->DownloadLauncher(m_szLauncherURL, m_HTTPMANAGER))
					{
					}
					PostQuitMessage(0);
				}
			}
#endif
		}
		break;
	}
}

void CAutoUpgradeDlg::ExecuteProcess( char* szFileName , char* szCmdLine )
{
	TCHAR param[4096];
	GenerateParameter(szCmdLine, param, sizeof(param));

	// 	FILE *fl;
	//  
	// 	fl = fopen("jal.jal", "wb");
	// 
	// 	if(fl)
	// 	{
	// 		fprintf(fl, "%s\r\n", param);
	// 	}
	// 
	// 	fclose(fl);

#ifdef KAMU_EAC
	sprintf(param, "%s%s", szFileName, szCmdLine);
#endif

	LOG.PrintTimeAndLog(0, "easyanti - szCmdLine - %s", param );
	Help::CreateProcessWithInfo( szFileName, param );
	
#ifdef _ADMIN_
	LOG.PrintTimeAndLog(0, "Complete - ExcuteProcess - %s - %s", szFileName, szCmdLine);
#else
	LOG.PrintTimeAndLog(0, "Complete - ExcuteProcess - %s", szFileName );
#endif
}

void CAutoUpgradeDlg::ParseCmd( IN const char *szCmdLine, OUT CString &szOptinosOnCmd )
{
	// cmd : check key option gameserverid
	CString szCopyedString = szCmdLine;

/*
	ioINILoaderAU kLoader( "info\\cmd.ini" );
	kLoader.SetTitle( "Cmdline" );
	kLoader.SaveString("SetLoginData::szCmdLine",szCmdLine);*/
	//kLoader.SaveString("SetLoginData::m_szLoginEncodePW",szLoginEncodePW);

	if( szCopyedString.IsEmpty() )
		return;

	int iCurPos = 0;
	// cmd parsing
	for(int i = 0; i < CT_MAX; i++)
	{
		CString szResult = szCopyedString.Tokenize( CMD_SECTION_TOKEN, iCurPos);
		if(szResult.IsEmpty()) 
			return;

		switch(i)
		{
		case CAutoUpgradeDlg::CT_EXPAND_VALUE:
			ParseExpandValue( (LPCTSTR) szResult );
			break;
		case CAutoUpgradeDlg::CT_VCODE:
			{
				if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_LATIN)
				{
					ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
					if(pLocal)
						pLocal->SetLoginData(m_szOGPKey, szResult);
				}
				else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND)
				{
					ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
					if(pLocal)
						pLocal->SetLoginData(m_szOGPKey, szResult);
				}
#ifdef STEAM_ATTACH
				if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US || ioLocalManager::GetLocalType() == ioLocalManager::LCT_BRAZIL )
				{
					ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
					if(pLocal)
					{
						m_HTTPMANAGER = new HTTPManager;
						pLocal->SetLoginData(m_szOGPKey, szResult);
					}
				}
				
#endif
			}
			break;
		case CAutoUpgradeDlg::CT_OPTIONS:
			szOptinosOnCmd = szResult;
			break;
		case CAutoUpgradeDlg::CT_FULLZIPVER:
			m_szFullZipVer = szResult;
			break;
		case CAutoUpgradeDlg::CT_COPY_FOLDER_SOURCE:
			m_szCFSourceFolder = szResult;
			break;
		case CAutoUpgradeDlg::CT_COPY_FOLDER_DEST:
			m_szCFDestFolder = szResult;
			break;
		}
	}
	return;
}


BOOL CAutoUpgradeDlg::RunOptionWithNoUI()
{
	// 정상 실행 체크
	CString szCSCmdLine = AfxGetApp()->m_lpCmdLine;
#ifdef _ADMIN_
	LOG.PrintTimeAndLog(0, "Complete - CMD - %s", (LPCTSTR)szCSCmdLine);
#endif
	CString szOptions;
	ParseCmd((LPCTSTR)szCSCmdLine, szOptions);

	if(szCSCmdLine.Find(NORMAL_EXCUTE_CHECK) == -1)
	{
		LOG.PrintTimeAndLog(0, "Error - Excute Check In Cmd");
		return FALSE;
	}

#ifdef _ADMIN_
	if(szOptions.IsEmpty())
	{
		LOG.PrintTimeAndLog(0, "Error - RunOptionWithNoUI - No Option");
		return FALSE;
	}

	if( ( (szOptions.GetLength() >= OT_EXTRACT_PAC + 1) 
		&& szOptions.GetAt(OT_EXTRACT_PAC) == '1' ) )	
	{
		m_bExtractPac = true;
		LOG.PrintTimeAndLog(0, "Complete - 1RunOptionWithNoUI - Extract pack");
	}
	else if( (szOptions.GetLength() >= OT_ADMIN_FOLDER_COPY + 1) 
		&& szOptions.GetAt(OT_ADMIN_FOLDER_COPY) == '1' )
	{
		m_bCopyFolder = true;
		LOG.PrintTimeAndLog(0, "Complete - 2RunOptionWithNoUI - copy folder");
	}
	else if( (szOptions.GetLength() >= OT_RESOURCE_UPDATE + 1) && szOptions.GetAt(OT_RESOURCE_UPDATE) == '0' 
		&& (szOptions.GetLength() >= OT_VISUAL_STUDIO_DEBUG + 1) && szOptions.GetAt(OT_VISUAL_STUDIO_DEBUG) == '1' )
	{
		ioINILoaderAU kLoader("config\\debug_connect.ini");
		kLoader.SetTitle("debug_connect");
		kLoader.SaveString("login_key", AfxGetApp()->m_lpCmdLine);

		LOG.PrintTimeAndLog(0, "Complete - 3RunOptionWithNoUI - no update and debugging");
		return FALSE;
	}
	else if( (szOptions.GetLength() >= OT_RESOURCE_UPDATE + 1) && szOptions.GetAt(OT_RESOURCE_UPDATE) == '0')
	{
		char rootDir[MAX_PATH]="";
		::GetCurrentDirectory(MAX_PATH,rootDir);

		char szClientName[MAX_PATH] = "";
		StringCbPrintf(szClientName, sizeof(szClientName), CLIENT_FILE_NAME,rootDir);

		char szLauncherName[MAX_PATH] = "";
		StringCbPrintf(szLauncherName, sizeof(szLauncherName), LAUNCHER_FILE_NAME, rootDir);

		char szCmdLine[MAX_PATH*2] = "";
		GenerateParameter(szCSCmdLine.GetBuffer(), szCmdLine, sizeof(szCmdLine));

		ExecuteProcess(szClientName, szCmdLine);

		LOG.PrintTimeAndLog(0, "Complete - 4RunOptionWithNoUI - no update and excute");
		return FALSE;
	}
	else if( (szOptions.GetLength() >= OT_VISUAL_STUDIO_DEBUG + 1) 
		&& szOptions.GetAt(OT_VISUAL_STUDIO_DEBUG) == '1' )
	{
		m_bVSDebug = true;
		LOG.PrintTimeAndLog(0, "Complete - 5RunOptionWithNoUI - debugging");
	}
	else if( (szOptions.GetLength() >= OT_RESOURCE_UPDATE + 1) 
		&& szOptions.GetAt(OT_RESOURCE_UPDATE) == '2' )
	{
		m_bPieceFileUpdate = true;
		LOG.PrintTimeAndLog(0, "Complete - 6RunOptionWithNoUI - piece files update");
	}
#endif

	return TRUE;
}


void CAutoUpgradeDlg::LoadExplorerDlg()
{
	if(!m_ExplrDlg)
		m_ExplrDlg.Create( IDD_EXPLORER_DLG );
	m_ExplrDlg.ShowWindow(SW_SHOW);
//	m_StartButton.EnableWindow(FALSE);
	SetMigrationButton( FALSE );

// 	int nResult = MessageBox("Do you wish to transfer your previous in-game data from Asiasoft?", 
// 		"Transfer Game Data", MB_YESNO);
// 
// 	if( nResult == IDYES)
// 	{
// 		if(!m_ExplrDlg)
// 			m_ExplrDlg.Create( IDD_EXPLORER_DLG );
// 		m_ExplrDlg.ShowWindow(SW_SHOW);
// 		m_StartButton.EnableWindow(FALSE);
// 	}
// 	else if(nResult == IDNO)
// 	{
// 
// 	}
}

bool CAutoUpgradeDlg::CheckParamVersion( char* param )
{
	std::string str = param;
	std::vector<std::string> tokens;

	tokenize(str, "?", tokens);

	int tokenCount = tokens.size();
	int versionIndex = 0, version = 0;
#ifdef SRC_KR
	versionIndex = 6;
#else
	versionIndex = 5;
#endif

	if( tokenCount <= versionIndex )
		return false;

	version = atoi( tokens[versionIndex].c_str() );
	if( version == PARAMETER_VERSION )
		return true;

	return false;
}

void CAutoUpgradeDlg::GenerateParameter(char* original, char* destination, const int length)
{
	ZeroMemory(destination, length);
	if(m_waitingDlg && m_waitingDlg->IsFine())
	{
		// 밸런서 처리가 되었으므로 아이피, 포트를 재조립한다
		std::string str = original;
		std::vector<std::string> tokens;

		string serverId;
		m_waitingDlg->GetADDR(serverId);

		// token : 4
		tokenize(str, "?", tokens);

		int tokenCount = tokens.size();
		//LOG.PrintTimeAndLog(0, "GenerateParameter : %d", tokenCount);
		for(int i = 0 ; i < tokenCount ; i++)
		{
			//LOG.PrintTimeAndLog(0, "GenerateParameter : [%d]%s", i, tokens[i].c_str());
#ifdef SRC_KR
			if(i == 4)
#else
			if(i == 3)
#endif
			{
				strcat(destination, serverId.c_str());
			}
			else
			{
				strcat(destination, tokens[i].c_str());
			}
			if(i != (tokenCount-1))
			{
				strcat(destination, "?");
			}
		}

		//LOG.PrintTimeAndLog(0, "GenerateParameter - Complete %s", destination);
	}
	else
	{
		// 원본으로 복사
		StringCbPrintf(destination, length, "%s", original);
	}
}

void CAutoUpgradeDlg::CentreWindowAndTopMost()
{
	RECT winrect, workrect;

	// Find how large the desktop work area is
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrect, 0);
	int workwidth = workrect.right -  workrect.left;
	int workheight = workrect.bottom - workrect.top;

	// And how big the window is
	GetWindowRect(&winrect);
	int winwidth = winrect.right - winrect.left;
	int winheight = winrect.bottom - winrect.top;
	// Make sure it"s not bigger than the work area
	winwidth = min(winwidth, workwidth);
	winheight = min(winheight, workheight);

	// Now center it
	SetWindowPos(
		NULL, /*&CWnd::wndTopMost,*/ // 창 항상 위로 
		workrect.left + (workwidth-winwidth) / 2,
		workrect.top + (workheight-winheight) / 2,
		winwidth, winheight, 
		SWP_SHOWWINDOW);

	SetForegroundWindow(); 
}

void CAutoUpgradeDlg::MinimizeAllWindows(BOOL bMinimize)
{
	HRESULT hr;

	if( FAILED(CoInitialize(NULL)) )
		return ;

	IShellDispatch *pDisp;
	hr = CoCreateInstance(CLSID_Shell,NULL,CLSCTX_INPROC_SERVER ,IID_IShellDispatch,(void**)&pDisp);
	if( SUCCEEDED(hr) )
	{
		if( bMinimize )
			pDisp->MinimizeAll();      
		else
			pDisp->UndoMinimizeALL();  

		pDisp->Release();
	}

	CoUninitialize();
}
BOOL CAutoUpgradeDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(pCopyDataStruct != NULL)
	{
		switch(pCopyDataStruct->dwData)
		{
		case WM_FB_DATA:
			{
				if( pCopyDataStruct->lpData )
				{
					string sEncrytData = (LPCTSTR)pCopyDataStruct->lpData;
					ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
					if( pLocal )
						pLocal->OnFacebookData( sEncrytData );
				}
			}
			break;
		case WM_MY_MSG_C_CONNECT:
			{
				m_szNoticeText = STR(1);
				SetText((LPCTSTR)m_szNoticeText, m_iDisplaySeconds);
				if( m_iAutoProcStep != ASP_EXECUTE_LOSA )
				{
					if(Help::CheckRunningProcess("LostSaga" , true))
					{
						char szText[MAX_PATH] = "Wrong Execute LostSaga - ";
						char szExecStep[MAX_PATH] = "";
						_itoa( m_iAutoProcStep,szExecStep, 10);
						strcat(szText, szExecStep );
						SendLOG( szText, 227); //로그 찍기
						LOG.PrintTimeAndLog(0, "Wrong Execute LostSaga");
					}
				}
				LOG.PrintTimeAndLog(0, "Complete - OnCopyData - WM_MY_MSG_C_CONNECT");
			}
			break;
		case WM_MY_MSG_C_CONNECT_OK:
			{
				m_szNoticeText = STR(2);
				SetText((LPCTSTR)m_szNoticeText, m_iDisplaySeconds);
				LOG.PrintTimeAndLog(0, "Complete - OnCopyData - WM_MY_MSG_C_CONNECT_OK");
			}
			break;
		case WM_MY_MSG_C_ERROR:
			{
				m_szNoticeText = STR(3);
				SetText((LPCTSTR)m_szNoticeText, m_iDisplaySeconds);
				LOG.PrintTimeAndLog(0, "Complete - OnCopyData - WM_MY_MSG_C_ERROR");
				SetTimer(TIME_ID_CLIENT_EXIT, 10000, NULL);
			}
			break;
		case WM_MY_MSG_C_SUCCESS:
			LOG.PrintTimeAndLog(0, "Complete - OnCopyData - WM_MY_MSG_C_SUCCESS");
			SetTimer(TIME_ID_CLIENT_EXIT, 1000, NULL);
			break;
		case WM_MY_MSG_C_IP_BLOCK:
			m_szNoticeText = STR(4);
			SetText((LPCTSTR)m_szNoticeText, m_iDisplaySeconds);
			LOG.PrintTimeAndLog(0, "Complete - OnCopyData - WM_MY_MSG_C_IP_BLOCK");
			SetTimer(TIME_ID_CLIENT_EXIT, 1000, NULL);
			break;
		}
		m_iClientMsg = pCopyDataStruct->dwData;
	}
	return CDialogSK::OnCopyData(pWnd, pCopyDataStruct);
}

void CAutoUpgradeDlg::SetWantEndDialog()
{
	static bool isExcuted = false;
	if(!isExcuted)
	{
		if(m_HTTPMANAGER)
			m_HTTPMANAGER->Close();
		SetTimer(TIME_ID_EXIT, 100, NULL);
		SetText(STR(1), 1);
		isExcuted = true;
	}
}

bool CAutoUpgradeDlg::EndDialogWithSendMsg()
{
	if(m_HTTPMANAGER)
	{
		if( m_HTTPMANAGER->GetRunEndType() == HTTPManager::ET_WAIT_CLIENT_MSG )
		{
			HWND hWnd = ::FindWindow(CLIENT_CLASS_NAME, NULL);
			if(hWnd != NULL)
			{
				COPYDATASTRUCT cds;
				cds.dwData = WM_MY_MSG_A_EXIT;
				cds.cbData = 0;
				cds.lpData = NULL;
				::SendMessage(hWnd, WM_COPYDATA,0,(LPARAM)&cds);

				EndDialog(0);
				return true;
			}
		}
	}
	return false;
}

void CAutoUpgradeDlg::SetText( const char *pText, int iCnt )
{
	CString szTimeCntText;
	szTimeCntText.Format("%s[%d]", pText, iCnt);
	SetDlgItemText(IDC_TEXT, szTimeCntText);
	m_Static.Invalidate();
}

void CAutoUpgradeDlg::ParseExpandValue( const char *szExpandVelue )
{
	// expand value안에 확장 타입이 있음
	// EDEW3940FVDP4950,10,20,30,1,autoupgrade_info.ini,300,39eidko2043049dkk,1
	// EDEW3940FVDP4950 : check value / 
	// 10 : 램 512M 이하에서 쓰레기 파일이 10메가를 넘으면 쓰레기 파일 정리 (10 숫자는 가변)
	// 20 : 램 512M~1G  에서 쓰레기 파일이 20메가를 넘으면 쓰레기 파일 정리 (20 숫자는 가변)
	// 30 : 램 1G   이상에서 쓰레기 파일이 30메가를 넘으면 쓰레기 파일 정리 (30 숫자는 가변)
	// 1  : 최적화를 클릭해도 최적화를 하지 않는 기준 용량 나타내면 1메가   (1  숫자는 가변)
	// autoupgrade_info.ini : ini 파일 이름 ( 채널링 마다 다르다 )
	// 300: ChannelingType 
	// 39eidko2043049dkk : mgame OTP 값 또는 buddy OTP 값 ( buddy는 256바이트 )
	// 1 : 게시판 사용 가능 여부 1 : 사용 가능     0 : 사용 불가 ( 일반유저는 무조건 1 )
	// 119483910 : 로컬 타입 ( LocalType )
	// 1a1204966af942deb5d299bf19741f02 : OGP Key
	// 2012-07-17 15 : mgame 선택적 셧다운 날짜, 북미 일경우 스팀 아이디값

	enum ExpandType
	{
		ET_CHECK      = 0,
		ET_GARBAGE_1  = 1,
		ET_GARBAGE_2  = 2,
		ET_GARBAGE_3  = 3,
		ET_OPTIMIZE   = 4,
		ET_INI_NAME   = 5,
		ET_CHANNELING = 6,
		ET_OTP        = 7,
		ET_LOGIN      = 8,
		ET_LOCAL      = 9,
		ET_OGPKEY     = 10,
		ET_SHUTDOWN   = 11,   
		ET_MAX,
	};
	// parse
	CString szCopyedString = szExpandVelue;
	if(szCopyedString.IsEmpty()) return;

	CString szSteamID;

	enum { MAX_GARBAGE = 5, };
	int iMaxGarbages[MAX_GARBAGE] = {0, 0, 0, 0, 0};
	int iCurPos = 0;
	for (int i = 0; i < ET_MAX ; i++)
	{
		CString szResult = szCopyedString.Tokenize( CMD_EXPAND_SECTION_TOKEN, iCurPos);
		//LOG.PrintTimeAndLog(0, "CAutoUpgradeDlg::ParseExpandValue \n ExpandType : %d  szResult: %s", i, szResult );
		if(szResult.IsEmpty()) 
			break;

		if( i == ET_CHECK ) 
			continue; // EDEW3940FVDP4950 통과
		else if( i == ET_GARBAGE_1 ||
			i == ET_GARBAGE_2 ||
			i == ET_GARBAGE_3 ||
			i == ET_OPTIMIZE )
		{
			iMaxGarbages[i] = atoi((LPCTSTR)szResult);
		}
		else if( i == ET_INI_NAME )
		{
			m_szININame = szResult;
		}
		else if( i == ET_CHANNELING )
		{
			ChannelingType eChannelingType = (ChannelingType) atoi( (LPCTSTR) szResult );
			g_ChannelingMgr.SetChannelingType( eChannelingType );
		}
		else if( i == ET_OTP )
		{
			if( g_ChannelingMgr.GetChannelingType() == CNT_MGAME || 
				g_ChannelingMgr.GetChannelingType() == CNT_BUDDY  )
			{
				LOG.PrintTimeAndLog(0, "OTP 파일생성 체크, 타입 : %d(MGAME = 300) \n szResult: %s", g_ChannelingMgr.GetChannelingType(), szResult );
				FILE *pFile = fopen( "info/cd","wb" ); // cd: channeling data
				if( pFile )
				{
					fwrite( (LPCTSTR) szResult, szResult.GetLength(), 1, pFile );
					fflush( pFile );
					fclose( pFile );
				}
			}
		}
		else if( i == ET_LOGIN )
		{
			ChannelingType ac = g_ChannelingMgr.GetChannelingType();
			if( g_ChannelingMgr.GetChannelingType() == CNT_MGAME || 
				g_ChannelingMgr.GetChannelingType() == CNT_BUDDY)
			{
				FILE *pFile = fopen( "info/cdl","wb" ); // cdl: channeling data login
				if( pFile )
				{
					fwrite( (LPCTSTR) szResult, szResult.GetLength(), 1, pFile );
					fflush( pFile );
					fclose( pFile );
				}
			}
		}
		else if(i == ET_OGPKEY)
		{
			m_szOGPKey = szResult;
		}
		else if( i == ET_SHUTDOWN )
		{
			if( g_ChannelingMgr.GetChannelingType() == CNT_MGAME && szResult != "0" )
			{
				char szDate[MAX_PATH]="";
				StringCbCopy( szDate, sizeof( szDate ),  (LPCTSTR) szResult );

				char szEncrypt[MAX_PATH]="";
				StringCbCopy( szEncrypt, sizeof( szEncrypt ), szDate );

				int iLength = strlen( szDate );
				for (int i = 0; i < iLength ; i++)
				{
					szEncrypt[i] = szDate[i] ^ 0xF1 ^ 0x35;
				}

				FILE *pFile = fopen( "info/cds", "wb" );
				if( pFile )
				{
					fwrite( szEncrypt, iLength, 1, pFile );
					fflush( pFile );
					fclose( pFile );
				}
			}
#ifdef STEAM_ATTACH
			if(ioLocalManager::GetLocalType() == ioLocalManager::LCT_US || ioLocalManager::GetLocalType() == ioLocalManager::LCT_BRAZIL)
			{
				szSteamID = szResult;
			}
#endif
		}
	}

	// get physical memory
	MEMORYSTATUS memoryStatus;
	ZeroMemory(&memoryStatus,sizeof(MEMORYSTATUS));
	memoryStatus.dwLength = sizeof (MEMORYSTATUS);
	::GlobalMemoryStatus (&memoryStatus);
	DWORD dwMemoryMB = (DWORD)(memoryStatus.dwTotalPhys/MEGA);

	// set max garbagemega
	if( dwMemoryMB > 1000 ) // 약 1G 이상 
	{
		m_iMaxGarbageMega = iMaxGarbages[ET_GARBAGE_3];
	}
	else if( dwMemoryMB > 500 ) // 약 512M 이상
	{
		m_iMaxGarbageMega = iMaxGarbages[ET_GARBAGE_2];
	}
	else
	{
		m_iMaxGarbageMega = iMaxGarbages[ET_GARBAGE_1];
	}
	m_iManualMaxOptimizeMega = iMaxGarbages[ET_OPTIMIZE];

#ifdef _ADMIN_
	LOG.PrintTimeAndLog(0, "Complete - Max Garbage Mega - %d", m_iMaxGarbageMega);
#endif
	LOG.PrintTimeAndLog(0, "Complete - Max Manual Optimize Mega - %d : %s", m_iManualMaxOptimizeMega, (LPCTSTR)m_szININame);
}

void CAutoUpgradeDlg::ShowErrorDlg( const char* szErrorText, int iErrorNumber, bool bExitProgram /*= true*/ )
{
	m_ErrorDlg.ShowErrorDlg(szErrorText, iErrorNumber, bExitProgram);
}

void CAutoUpgradeDlg::InitSocket()
{
	ioPortalHelp::InitWinSock();

	//소켓이 초기화 안될수도 있지만 무시 한다.
	if( m_pUDPSocket == NULL )
		m_pUDPSocket = new ioClientUDPSocket(1); // 1은 무조건 암호 통신
	if( m_pUDPSocket )
		m_pUDPSocket->InitUDP( m_hWnd, WM_P2PSOCKET, P2PSOCKET_PORT );
}

void CAutoUpgradeDlg::SendLOG( const char *szLog, int iErrorNumber , bool bUpgradeError /*= true*/ )
{
	if( !m_pUDPSocket ) return;
	if( m_szLogServerIP.IsEmpty() ) return;

	char szSendLog[MAX_PATH*2]="";
	StringCbPrintf(szSendLog, sizeof(szSendLog), "[%s]%s",m_pUDPSocket->GetMyIP(), szLog);
	SP2Packet kPacket( LUPK_LOG );
	if( bUpgradeError )
		kPacket << "UpgradeError";   // 로그 파일 타입
	else
		kPacket << "WebBrokerError"; // 로그 파일 타입
	kPacket << szSendLog;
	kPacket << iErrorNumber;   // 오류번호
	kPacket << true ;          // write db

	m_pUDPSocket->SendLoop( (LPCTSTR)m_szLogServerIP,m_iPort,kPacket );
}

void CAutoUpgradeDlg::LoadINI()
{
	CString szINIPath = "info\\";
	szINIPath += m_szININame;
	ioINILoaderAU kLoader( (LPCTSTR)szINIPath );
	kLoader.SetTitle("autoupgrade_info");

	char szBuff[MAX_PATH]="";
	memset( szBuff, 0, MAX_PATH );
	kLoader.LoadString("log_server_ip", "", szBuff, MAX_PATH);
	m_szLogServerIP = szBuff;
	m_iPort = kLoader.LoadInt("log_server_port", 0);

	memset( szBuff, 0, MAX_PATH );
	kLoader.LoadString("banner_url", "", szBuff, MAX_PATH);
	if( strcmp( szBuff, "") != 0 )
		m_WebBrowser.Navigate(szBuff, NULL, NULL, NULL, NULL);
	memset( m_szBackUrl, 0, MAX_PATH );

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_EU)
	{
		kLoader.LoadString("banner_url", "", szBuff, MAX_PATH);
		kLoader.LoadString("background_url", "", m_szBackUrl, MAX_PATH);
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_GERMANY )
	{
		kLoader.LoadString("banner_url_de", "", szBuff, MAX_PATH);
		kLoader.LoadString("background_url_de", "", m_szBackUrl, MAX_PATH);
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_FRANCE )
	{
		kLoader.LoadString("banner_url", "", szBuff, MAX_PATH);
		kLoader.LoadString("background_url", "", m_szBackUrl, MAX_PATH);
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_ITALIA )
	{
		kLoader.LoadString("banner_url_it", "", szBuff, MAX_PATH);
		kLoader.LoadString("background_url_it", "", m_szBackUrl, MAX_PATH);
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_POLAND )
	{
		kLoader.LoadString("banner_url", "", szBuff, MAX_PATH);
		kLoader.LoadString("background_url", "", m_szBackUrl, MAX_PATH);
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_TURKEY )
	{
		kLoader.LoadString("banner_url", "", szBuff, MAX_PATH);
		kLoader.LoadString("background_url", "", m_szBackUrl, MAX_PATH);
	}
	else
	{
		kLoader.LoadString("banner_url", "", szBuff, MAX_PATH);
		kLoader.LoadString("background_url", "", m_szBackUrl, MAX_PATH);
	}
#ifdef MIGRATION
 	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
 	{
 		m_WebBrowser.Navigate("http://www.nxcreative.co.kr/pageA.php", NULL, NULL, NULL, NULL);
 		m_WebBrowser.ShowWindow(SW_HIDE);
 	}
#else
	if( strcmp( szBuff, "") != 0 )
		m_WebBrowser.Navigate(szBuff, NULL, NULL, NULL, NULL);
#endif

	memset( m_szBackUrl, 0, MAX_PATH );
	kLoader.LoadString("background_url", "", m_szBackUrl, MAX_PATH);

	memset( szBuff, 0, MAX_PATH );
	// default값은 라이브버전
	kLoader.LoadString("launcher", "http://cdn.lostsaga.z8games.com/Launcher/LastVersion/", szBuff, MAX_PATH);
	m_szLauncherURL = szBuff;

	memset( szBuff, 0, MAX_PATH );
	kLoader.LoadString("start_ini_url", "", szBuff, MAX_PATH);
	m_szStartINIURL = szBuff;


}

void CAutoUpgradeDlg::SendSavedLog()
{
	ioINILoaderAU kIniLoder("Save\\default\\setting.ini");
	kIniLoder.SetTitle("Setting");
	int iErrorNumber = kIniLoder.LoadInt("UpgradeError", 0 );
	if( iErrorNumber != 0 )
	{
		kIniLoder.SaveInt("UpgradeError", 0 );
		char szErrorLog[MAX_PATH]="";
		StringCbCopy(szErrorLog, sizeof(szErrorLog), "Duplicate client" );
		SendLOG(szErrorLog, iErrorNumber );	
	}

	char szRegKey[MAX_PATH*2]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		StringCbPrintf( szRegKey , sizeof( szRegKey ), "%sWebBroker", pLocal->GetRegKey() );

	iErrorNumber = Help::RegReadInt(REG_SHCU, szRegKey, "Error", 0);
	if( iErrorNumber != 0 )
	{
		Help::RegWriteInt(REG_SHCU, szRegKey, "Error", 0);
		char szErrorLog[MAX_PATH]="";
		StringCbCopy(szErrorLog, sizeof(szErrorLog), "Web Broker Error" );
		SendLOG(szErrorLog, iErrorNumber , false);	
	}
}

void CAutoUpgradeDlg::ShowStartBtn()
{
	m_OptimizeBtn.ShowWindow(SW_HIDE);
	m_ProgressBar.ShowWindow(SW_HIDE);
#ifdef MIGRATION
 	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
 		m_StartButton.EnableWindow(FALSE);
#endif
	m_StartButton.ShowWindow(SW_SHOW);
	
	// 버튼이 교체되면 창이 다른창에 가려졌을때 이번 버튼의 이미지가 지워지지 않아 갱신 처리
	CRect Rect;
	GetClientRect(&Rect);
	Rect.top = Rect.bottom - 20;
	if( Rect < 0 )
		Rect = 0;
	InvalidateRect( &Rect );
}

void CAutoUpgradeDlg::SetOptimizeBtnDisable()
{
	m_OptimizeBtn.EnableWindow( FALSE );
}

BOOL CAutoUpgradeDlg::IsOptimizeChecked() const
{
	return m_OptimizeBtn.IsChecked();
}

BOOL CAutoUpgradeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg && pMsg->message == WM_KEYDOWN )
	{
		if( pMsg->wParam == VK_SPACE )
		{
#ifdef MIGRATION
			if(ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND)
			{
				if(m_ExplrDlg) OnStart();
			}
#else
			OnStart();
#endif
			return TRUE;
		}
		else if( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
			return FALSE;
		else if( pMsg->wParam == VK_F5)
			return FALSE;
	}

	return CDialogSK::PreTranslateMessage(pMsg);
}

void CAutoUpgradeDlg::ShowLogin()
{
	m_LoginDlg.ShowWindow( SW_SHOW );
	m_LoginDlg.LoadINI();
	m_LoginDlg.FirstConnectServer();
}

void CAutoUpgradeDlg::SetStartButton(bool bSet)
{
	m_StartButton.EnableWindow(bSet);
}

void CAutoUpgradeDlg::SetMigrationButton(bool bSet)
{
	m_MigrationButtonL.EnableWindow(bSet);
	m_MigrationButtonR.EnableWindow(bSet);
}

bool CAutoUpgradeDlg::IsLoginOk()
{
	return m_LoginDlg.IsLoginOk();
}

void CAutoUpgradeDlg::SetLoginNetworkState( NetworkState eNetworkState )
{
	m_LoginDlg.SetNetworkState( eNetworkState );
	m_LoginDlg.SetTextNetworkConnected( eNetworkState );
}

void CAutoUpgradeDlg::SetMigrationNetworkState(NetworkState eNetworkState)
{
	m_ExplrDlg.SetNetworkState(eNetworkState);
}

void CAutoUpgradeDlg::SetLoginTimer()
{
	m_LoginDlg.SetLoginTimer();
}

void CAutoUpgradeDlg::ResumeHttpManager()
{
	if( m_HTTPMANAGER )
		m_HTTPMANAGER->ResumeRun();
}

void CAutoUpgradeDlg::LoginDlgRecvPacket( SP2Packet &rkPacket )
{
	m_LoginDlg.RecvPacket( rkPacket );
}

void CAutoUpgradeDlg::MigrationDlgRecvPacket( SP2Packet &rkPacket )
{
	m_ExplrDlg.RecvPacket( rkPacket );
}

void CAutoUpgradeDlg::ShowOTP()
{
	m_OTPDlg.ShowWindow( SW_SHOW );
	m_OTPDlg.ConnectSocket();
}

bool CAutoUpgradeDlg::IsOTPOk()
{
	return m_OTPDlg.IsOTPOk();
}

__int64 CAutoUpgradeDlg::GetGameServerID()
{
	if( !m_waitingDlg )
		return 0;

	return m_waitingDlg->GetServerID();
}

BOOL CAutoUpgradeDlg::IsGameServerID()
{
	if( !m_waitingDlg )
		return FALSE;

	return m_waitingDlg->IsFine();
}

void CAutoUpgradeDlg::GetStrGameServerID( string &rsGameServerID )
{
	if( !m_waitingDlg )
		return;

	m_waitingDlg->GetADDR( rsGameServerID );
}

void CAutoUpgradeDlg::SetOTPNetworkState( NetworkState eNetworkState )
{
	m_OTPDlg.SetNetworkState( eNetworkState );
}

void CAutoUpgradeDlg::OTPDlgRecvPacket( SP2Packet &rkPacket )
{
	m_OTPDlg.RecvPacket( rkPacket );
}

void CAutoUpgradeDlg::BalancerRecvPacket( SP2Packet &rkPacket )
{
	m_waitingDlg->RecvPacket( rkPacket );

	if( m_waitingDlg->IsError())
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
			ShowErrorDlg(pLocal->GetConnectFailMent(), 228, true);
		else
			ShowErrorDlg("Error Connect", 228, true);
	}
	m_iAutoProcStep = ASP_LOGIN_CONNECT;
}

BOOL CAutoUpgradeDlg::IsWaitingDlgError()
{
	if(!m_waitingDlg)			// TODO : 미국 외에는 확인해 볼 것.
		return false;

	return m_waitingDlg->IsError();
}

BEGIN_EVENTSINK_MAP(CAutoUpgradeDlg, CDialogSK)
	ON_EVENT(CAutoUpgradeDlg, IDC_EXPLORER1, 259, DocumentCompleteExplorer1, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CAutoUpgradeDlg, IDC_EXPLORER1, 250, CAutoUpgradeDlg::BeforeNavigate2Explorer1, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

void CAutoUpgradeDlg::DocumentCompleteExplorer1(LPDISPATCH pDisp, VARIANT* URL)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	HRESULT hr = S_OK; 
	CComPtr<IWebBrowser2> pWB = NULL; 

	hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&pWB)); 

	if( !pWB )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IDispatch> pDocDisp; 
	hr = pWB->get_Document(&pDocDisp); 

	if( !pDocDisp )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLDocument2> pDoc; 
	hr = pDocDisp->QueryInterface(IID_IHTMLDocument2, reinterpret_cast<void **>(&pDoc)); 

	if( !pDoc )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLElement> pElement; 
	hr = pDoc->get_body(&pElement); 

	if( !pElement )
		return;
	if( FAILED(hr) )
		return;

	CComPtr<IHTMLStyle > pStyle;
	hr = pElement->get_style( &pStyle );

	if( !pStyle )
		return;
	if( FAILED(hr) )
		return;

	pStyle->put_border( CComBSTR(L"none") );

	CComPtr<IHTMLBodyElement> pBodyElement; 
	hr = pElement->QueryInterface(IID_IHTMLBodyElement, (void**)&pBodyElement); 

	if( !pBodyElement )
		return;
	if( FAILED(hr) )
		return;

	pBodyElement->put_scroll(CComBSTR(L"no")); 
}


void CAutoUpgradeDlg::BeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	GetID_PlayPark( pDisp );
//	GetID_AsiaSoft( pDisp );
//	GetID_PlayCyberGames( pDisp );
}


void CAutoUpgradeDlg::GetID_PlayPark(LPDISPATCH pDisp)
{
	// PlayPark ID
	// https://authen.playpark.net/authen_gateway/inpp/login_default.aspx
	HRESULT hr = S_OK; 
	CComPtr<IWebBrowser2> pWB = NULL; 

	hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&pWB)); 

	if( FAILED(hr) || !pWB )
		return;

	CComPtr<IDispatch> spIDispatchDoc = NULL;
	hr = pWB->get_Document( &spIDispatchDoc );

	if( FAILED(hr) || !spIDispatchDoc )
		return;

	CComPtr<IHTMLDocument2> spIHTMLDocument2 = NULL;
	hr = spIDispatchDoc.QueryInterface<IHTMLDocument2>( &spIHTMLDocument2 );

	if( FAILED(hr) || !spIHTMLDocument2 )
		return;

	CComPtr<IHTMLElementCollection> spIHTMLElementCollection = NULL;
	hr = spIHTMLDocument2->get_forms( &spIHTMLElementCollection );

	if( FAILED(hr) || !spIHTMLElementCollection )
		return;

	CComPtr<IDispatch> pElemDisp = NULL;
	CComPtr<IDispatch> pItem = NULL;
	hr = spIHTMLElementCollection->item( CComVariant(0), CComVariant(0), &pElemDisp );

	if( FAILED(hr) || !pElemDisp )
		return;

	CComPtr<IHTMLFormElement> spIHTMLFormElement = NULL;
	hr = pElemDisp->QueryInterface( IID_IHTMLFormElement, (void**)&spIHTMLFormElement );

	if( FAILED(hr) || !spIHTMLFormElement )
		return;

	CComPtr<IHTMLInputElement> spIHTMLInputElement = NULL;
	hr = spIHTMLFormElement->item( CComVariant(4), CComVariant(4), &pItem );

	if( FAILED(hr) || !pItem )
		return;

	pItem->QueryInterface( IID_IHTMLInputElement, (void**)&spIHTMLInputElement );

	if( FAILED(hr) || !spIHTMLInputElement )
		return;

	BSTR bstr;
	CString str;
	spIHTMLInputElement->get_value( &bstr );
	str = (LPCWSTR)bstr;
}


void CAutoUpgradeDlg::GetID_AsiaSoft(LPDISPATCH pDisp)
{
	// Asia soft 공식 홈페이지 ID
	// https://secure3.asiasoft.co.th/apmember/Default.aspx
	HRESULT hr = S_OK; 
	CComPtr<IWebBrowser2> pWB = NULL; 

	hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&pWB)); 

	if( FAILED(hr) || !pWB )
		return;

	CComPtr<IDispatch> spIDispatchDoc2 = NULL;
	hr = pWB->get_Document( &spIDispatchDoc2 );

	if( FAILED(hr) || !spIDispatchDoc2 )
		return;

	CComPtr<IHTMLDocument2> spIHTMLDocument2_1 = NULL;
	hr = spIDispatchDoc2.QueryInterface<IHTMLDocument2>( &spIHTMLDocument2_1 );

	if( FAILED(hr) || !spIHTMLDocument2_1 )
		return;

	CComPtr<IHTMLElement> spIHTMLElement = NULL;
	BSTR bstrID = L"pgMainAPManage";
	hr = spIHTMLDocument2_1->get_activeElement( &spIHTMLElement );

	if( FAILED(hr) || !spIHTMLElement )
		return;

	CComPtr<IDispatch> pDispatchDoc2 = NULL;
	hr = spIHTMLElement->get_document( &pDispatchDoc2 );

	if( FAILED(hr) || !pDispatchDoc2 )
		return;

	CComPtr<IHTMLDocument2> spIHTMLDocument2_2 = NULL;
	hr = pDispatchDoc2.QueryInterface<IHTMLDocument2>( &spIHTMLDocument2_2 );

	if( FAILED(hr) || !spIHTMLDocument2_2 )
		return;

	CComPtr<IHTMLElementCollection> spIHTMLElementCollection = NULL;
	hr = spIHTMLDocument2_2->get_forms( &spIHTMLElementCollection );

	if( FAILED(hr) || !spIHTMLElementCollection )
		return;

	CComPtr<IDispatch> pElemDisp = NULL;
	CComPtr<IDispatch> pItem = NULL;
	hr = spIHTMLElementCollection->item( CComVariant(0), CComVariant(0), &pElemDisp );

	if( FAILED(hr) || !pElemDisp )
		return;

	CComPtr<IHTMLFormElement> spIHTMLFormElement = NULL;
	hr = pElemDisp->QueryInterface( IID_IHTMLFormElement, (void**)&spIHTMLFormElement );

	if( FAILED(hr) || !spIHTMLFormElement )
		return;

	CComPtr<IHTMLInputElement> spIHTMLInputElement = NULL;
	hr = spIHTMLFormElement->item( CComVariant(0), CComVariant(0), &pItem );

	if( FAILED(hr) || !pItem )
		return;

	pItem->QueryInterface( IID_IHTMLInputElement, (void**)&spIHTMLInputElement );

	if( FAILED(hr) || !spIHTMLInputElement )
		return;

	BSTR bstr;
	CString str;

	spIHTMLInputElement->get_value( &bstr );
	str = (LPCWSTR)bstr;
}


void CAutoUpgradeDlg::GetID_PlayCyberGames(LPDISPATCH pDisp)
{
	// Play Cyber Games (TCG) ID
	// http://profile.playcybergames.com/Profile/Login.aspx
	HRESULT hr = S_OK; 
	CComPtr<IWebBrowser2> pWB = NULL; 

	hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void**>(&pWB)); 

	if( FAILED(hr) || !pWB )
		return;

	CComPtr<IDispatch> spIDispatchDoc = NULL;
	hr = pWB->get_Document( &spIDispatchDoc );

	if( FAILED(hr) || !spIDispatchDoc )
		return;

	CComPtr<IHTMLDocument2> spIHTMLDocument2 = NULL;
	hr = spIDispatchDoc.QueryInterface<IHTMLDocument2>( &spIHTMLDocument2 );

	if( FAILED(hr) || !spIHTMLDocument2 )
		return;

	CComPtr<IHTMLElementCollection> spIHTMLElementCollection = NULL;
	hr = spIHTMLDocument2->get_forms( &spIHTMLElementCollection );

	if( FAILED(hr) || !spIHTMLElementCollection )
		return;

	CComPtr<IDispatch> pElemDisp = NULL;
	CComPtr<IDispatch> pItem = NULL;
	hr = spIHTMLElementCollection->item( CComVariant(0), CComVariant(0), &pElemDisp );

	if( FAILED(hr) || !pElemDisp )
		return;

	CComPtr<IHTMLFormElement> spIHTMLFormElement = NULL;
	hr = pElemDisp->QueryInterface( IID_IHTMLFormElement, (void**)&spIHTMLFormElement );

	if( FAILED(hr) || !spIHTMLFormElement )
		return;

	CComPtr<IHTMLInputElement> spIHTMLInputElement = NULL;
	hr = spIHTMLFormElement->item( CComVariant(4), CComVariant(4), &pItem );

	if( FAILED(hr) || !pItem )
		return;

	pItem->QueryInterface( IID_IHTMLInputElement, (void**)&spIHTMLInputElement );

	if( FAILED(hr) || !spIHTMLInputElement )
		return;

	BSTR bstr;
	CString str;
	spIHTMLInputElement->get_value( &bstr );
	str = (LPCWSTR)bstr;
}

#ifdef STEAM_ATTACH
bool CAutoUpgradeDlg::CheckLauncher()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if(pLocal)
	{
#ifdef SRC_NA
		ioLocalUS * pUSLocal = (ioLocalUS * )pLocal;
		if(!pUSLocal->IsLoginDataValidate())
		{
			SetDlgItemText(IDC_TEXT, "Checking Launcher");
			UpdateData(FALSE);
			m_Static.Invalidate();

			SetTimer( TIME_ID_STEAM_LAUNCHER_CHECK, 1, NULL );
			return true;
		}
#else
		ioLocalBrazil * pBRLocal = (ioLocalBrazil * )pLocal;
		if(!pBRLocal->IsLoginDataValidate())
		{
			SetDlgItemText(IDC_TEXT, "Checking Launcher");
			UpdateData(FALSE);
			m_Static.Invalidate();

			SetTimer( TIME_ID_STEAM_LAUNCHER_CHECK, 1, NULL );
			return true;
		}
#endif
	}
	return false;
}
#endif

void CAutoUpgradeDlg::OnMigration()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND)
 			LoadExplorerDlg();
}

void CAutoUpgradeDlg::OnMigrationR()
{
	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
	{
		m_StartButton.EnableWindow(TRUE);
	}
}
