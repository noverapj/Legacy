// AutoUpgrade.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "AutoUpgradeDlg.h"
#include "HelpFunc.h"
#include "Version.h"
#include "ioINILoaderAU.h"
#include "local\iolocalmanager.h"
#include "stringmanager\iostringmanager.h"
#include <strsafe.h>
#include "local\iolocalparent.h"
#include "channeling\iochannelingnodemanager.h"
#include "Util\ioHttpManager.h"

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
	m_pStringMgr     = NULL;
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

#ifdef _DEBUG
	LOG.OpenLog(1, "info/app.log");
#else
	LOG.OpenLog(0, "info/app.log");
#endif

	m_pLocalMgr = new ioLocalManager;
	if( m_pLocalMgr )
	{
		m_pLocalMgr->Init();
		m_pLocalMgr->ParseCmd( AfxGetApp()->m_lpCmdLine );
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	m_pStringMgr = new ioStringManager;
#ifdef SRC_OVERSEAS
	if( m_pStringMgr )
	{
		DWORD size = 0;
		const char * buffer = NULL;
		Help::LoadTextInResource(IDR_ERROR, size, buffer);

		if( m_pStringMgr)
			m_pStringMgr->LoadData( "", "", buffer, true );
	}

#else

	if( m_pStringMgr && pLocal )
		m_pStringMgr->LoadData( "", "", pLocal->GetMemTextList(), true  );

#endif

	m_pChannelingMgr = new ioChannelingNodeManager;
	if( m_pChannelingMgr )
		m_pChannelingMgr->Init();

	m_pHttpManager = new ioHttpManager;

#ifdef SHIPPING
	LOG.PrintTimeAndLog(0, "SHIPPING %d.%s" , FILEVER, STRINTERNALNAME);
#else
	LOG.PrintTimeAndLog(0, "ADMIN %d.%s" , FILEVER, STRINTERNALNAME);
#endif

	if(!IsDuplication())
	{
		LOG.CloseAndRelease();
  		return FALSE;
	}
 
  	CAutoUpgradeDlg dlg;
 	if(!dlg.RunOptionWithNoUI())
	{
		LOG.CloseAndRelease();
 		return FALSE;
	}

	/*
	HWND hWnd = NULL;
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg)
	hWnd = pDlg->GetSafeHwnd();

	if( MessageBox( hWnd, STR(1),
	APP_ERROR_NAME,
	MB_YESNO | MB_TOPMOST ) == IDNO ) return true;

	*/

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

	LOG.CloseAndRelease();
	SAFEDELETE(m_pStringMgr);
	SAFEDELETE(m_pLocalMgr);
	SAFEDELETE(m_pChannelingMgr);
	SAFEDELETE(m_pHttpManager);
	return FALSE;
}

BOOL CAutoUpgradeApp::IsDuplication()
{
	// auto
	m_h_mutex = CreateMutex(NULL, true, APPMUTEXNAME);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND hWndAuto = FindWindow( APPNAME, NULL );
		if(hWndAuto == NULL)
			hWndAuto = FindWindow( NULL,  APP_ERROR_NAME);
		if (hWndAuto)
			SetForegroundWindow(hWndAuto);				

		LOG.PrintTimeAndLog(0, "Error - Duplication - 1");
		return FALSE;
	}

#ifdef SHIPPING
	// client
	bool bDulication = false;
	if( !bDulication )
	{
		// client
		HANDLE hMutex = OpenMutex( MUTEX_ALL_ACCESS, FALSE, CLIENTMUTEXNAME );  
		if ( hMutex ) 
		{
			bDulication = true;
			CloseHandle( hMutex );
		}
	}
	//로사 실행 체크
	if (bDulication)
	{
		Help::MessageboxShow( NULL, STR(1), APP_ERROR_NAME, MB_TOPMOST );
		
		LOG.PrintTimeAndLog(0, "Error - Duplication - 2");
		Help::CreateDefaultSettingINI();
		ioINILoaderAU kIniLoder("Save\\default\\setting.ini");
		kIniLoder.SetTitle("Setting");
		kIniLoder.SaveInt("UpgradeError", 201 );
		return FALSE;
	}
#endif

	// class name set
	WNDCLASS wndClass;
	::ZeroMemory  (&wndClass, sizeof(WNDCLASS));
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wndClass);
	wndClass.lpszClassName = _T(APPNAME);
	AfxRegisterClass(&wndClass);

	return TRUE;
}


