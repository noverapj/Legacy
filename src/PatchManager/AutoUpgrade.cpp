// AutoUpgrade.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "AutoUpgradeDlg.h"
#include "local\iolocalmanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeApp

BEGIN_MESSAGE_MAP(CAutoUpgradeApp, CWinApp)
	//{{AFX_MSG_MAP(CAutoUpgradeApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeApp construction

CAutoUpgradeApp::CAutoUpgradeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_h_mutex        = NULL;
	m_pLocalMgr      = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAutoUpgradeApp object

CAutoUpgradeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeApp initialization

BOOL CAutoUpgradeApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

// #ifdef _AFXDLL
// 	Enable3dControls();			// Call this when using MFC in a shared DLL
// #else
// 	Enable3dControlsStatic();	// Call this when linking to MFC statically
// #endif

	m_pLocalMgr = new ioLocalManager;
	if( m_pLocalMgr )
		m_pLocalMgr->Init();

	// check duplicate excute
	m_h_mutex = CreateMutex(NULL, true, APPMUTEXNAME);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND phWnd = FindWindow( APPNAME, NULL );
		if(phWnd == NULL)
			phWnd = FindWindow( NULL, "LOSTSAGA_AUTOUPGRADE - ERROR" );
		if (phWnd)
			SetForegroundWindow(phWnd);				
		
		SAFEDELETE(m_pLocalMgr);
		return FALSE;
	}
	HWND phWnd2 = FindWindow( "Lost Saga in Timegate - Prototype", NULL ) ;		
	if (phWnd2)
	{
		MessageBox(NULL, "\n\n\n      이미 [로스트사가]가 실행중 입니다.\n\n      [로스트사가]를 종료후 실행해 주세요.\n\n\n", "LOSTSAGA_AUTOUPGRADE - ERROR", MB_OK); 
		SAFEDELETE(m_pLocalMgr);
		return FALSE;
	}

	WNDCLASS wndClass;
    ::ZeroMemory  (&wndClass, sizeof(WNDCLASS));
    ::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wndClass);
    wndClass.lpszClassName = _T(APPNAME);
    BOOL bRet = AfxRegisterClass(&wndClass);
	//

	CAutoUpgradeDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	if(m_h_mutex != NULL)
		ReleaseMutex(m_h_mutex);

	SAFEDELETE(m_pLocalMgr);
	return FALSE;
}
