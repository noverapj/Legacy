// AutoUpgradeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "AutoUpgradeDlg.h"
#include "ThreadModules/ThreadManager.h"
#include "InterfaceManager.h"
#include ".\autoupgradedlg.h"
#include "Version.h"

#include <strsafe.h>
#include <Shlobj.h>
#include "local/iolocalmanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//mini zip용
#include <iostream>
#include <fstream>
#include "MiniZip/unzip.h"
#include "Common.h"
#include <tlhelp32.h> 


#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

WNDPROC  CAutoUpgradeDlg::m_OldProc = NULL;

/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeDlg dialog

CAutoUpgradeDlg::CAutoUpgradeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoUpgradeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoUpgradeDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pPatchManager		= NULL;
	m_dwDisplaySec		= 0;
	m_bAdmin			= false;
	m_bWrongFile		= false;
	m_bFindTargetFolder = false;
	m_bExportBtn		= false;
	m_bVersionUp		= false;
	m_bZipFile			= false;
	m_bServerFile		= false;
//	m_iPatchType		= NORMAL_PATCH;
}

void CAutoUpgradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoUpgradeDlg)
	DDX_Control(pDX, IDC_BUTTON2, m_btnUpload);
	DDX_Control(pDX, IDC_BUTTON1, m_btnCreateInfo);
	DDX_Control(pDX, IDC_BUTTON4, m_btnShowLog);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
	DDX_Control(pDX, IDC_LIST2, m_ctrlList);
	DDX_Control(pDX, IDC_BUTTON3, m_btnDeleteList);
	DDX_Control(pDX, IDC_BTN_UPDATE, m_btnUpdate);
	DDX_Control(pDX, IDC_BTN_FULLZIP, m_btnFullZip);
	DDX_Control(pDX, IDC_DEL_NOSERVER_BTN, m_btnNoServer);
	DDX_Control(pDX, IDC_MAKE_FULLZIP_BTN, m_btnMakeFull);
	DDX_Control(pDX, IDC_BUTTON7, m_btnMakeDelFile);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BTN_FIX, m_btnPatchFix);
	DDX_Control(pDX, IDC_BTN_SYNC, m_btnCheckSync);
	DDX_Control(pDX, IDC_BTN_FOLDER, m_btnSelFolder);
	DDX_Control(pDX, IDC_BTN_FILE, m_btnSelFile);
	DDX_Control(pDX, IDC_BTN_P_PAC, m_btnMakePatch);
	DDX_Control(pDX, IDC_BTN_S_PAC, m_btnSyncPatch);
	
}

BEGIN_MESSAGE_MAP(CAutoUpgradeDlg, CDialog)
	//{{AFX_MSG_MAP(CAutoUpgradeDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedUpload)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedCreateInfo)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedShowLog)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedDeleteFileList)
	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBnClickedBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_FULLZIP, OnBnClickedBtnFullZip)
	ON_BN_CLICKED(IDC_DEL_NOSERVER_BTN, OnBnClickedNoServerDelBtn)
	ON_LBN_SELCHANGE(IDC_LIST2, &CAutoUpgradeDlg::OnLbnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON5, &CAutoUpgradeDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BTN_SYNC, &CAutoUpgradeDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BTN_FOLDER, OnBnClickedSelectFolder)
	ON_BN_CLICKED(IDC_BTN_FILE, OnBnClickedSelectFile)
	ON_BN_CLICKED(IDC_BTN_P_PAC, &CAutoUpgradeDlg::OnBnClickedBtnPPac)
	ON_BN_CLICKED(IDC_BTN_S_PAC, &CAutoUpgradeDlg::OnBnClickedBtnSPac)
	ON_BN_CLICKED(IDC_MAKE_FULLZIP_BTN, &CAutoUpgradeDlg::OnBnClickedMakeFullzipBtn)
	ON_BN_CLICKED(IDC_BUTTON7, &CAutoUpgradeDlg::OnBnClickedButton7)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoUpgradeDlg message handlers

BOOL CAutoUpgradeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CString szVersion;
	szVersion.Format( "Version: %s | %s", STRFILEVER, STRINTERNALNAME );
	SetDlgItemText( IDC_STATIC_STRING , (LPCTSTR)szVersion );


	CreateDirectory("config",NULL);
	::GetCurrentDirectory(MAX_PATH,m_szRootDir);
	InterfaceManager::GetInstance()->SetInterface(&m_ctrlProgress,&m_ctrlList,&m_btnCreateInfo,&m_btnUpload, &m_btnShowLog, &m_ComboBox
		                                        , &m_btnDeleteList, &m_btnUpdate, &m_btnNoServer, &m_btnFullZip, &m_btnPatchFix, &m_btnCheckSync, &m_btnMakePatch, &m_btnSyncPatch, &m_btnMakeFull, &m_btnMakeDelFile );
#ifdef SHIPPING
	GetDlgItem(IDC_BTN_FIX)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
	GetDlgItem(IDC_BTN_FULLZIP)->EnableWindow(false);
	GetDlgItem(IDC_DEL_NOSERVER_BTN)->EnableWindow(false);
#endif

	char szPath[MAX_PATH]="";
	StringCbPrintf(szPath, sizeof(szPath), "%s\\pm.ini", m_szRootDir);
	m_INILoader.SetFileName(szPath);

	// default zone의 local을 구한다.
	m_INILoader.SetTitle("Info");
	int iCurSel = m_INILoader.LoadInt("DefaultZone", 1); // 1~MaxZone
	char szTitle[MAX_PATH]="";
	StringCbPrintf( szTitle, sizeof( szTitle ), "Zone%d", iCurSel );
	m_INILoader.SetTitle( szTitle );
	ioLocalManager::SetLocalType( (ioLocalManager::LocalType) m_INILoader.LoadInt( "Local", ioLocalManager::LCT_KOREA ) );
	
	m_OldProc = (WNDPROC)::GetWindowLong(m_ctrlList.m_hWnd,GWL_WNDPROC);
	::SetWindowLong(m_ctrlList.m_hWnd,GWL_WNDPROC,(LONG)ListProc);

	SetTimer(TIME_ID_DISPLAY, 1000, NULL);

	LoadIniForZone();
	LoadInPatchInfo(); //패치될 파일 정보
	LoadFullZipInfo();
	m_brh.CreateSolidBrush(m_bgColor);

// 	if( ioLocalManager::GetLocalType() != ioLocalManager::LCT_KOREA )
// 	{
// 		InterfaceManager::GetInstance()->SetCreateInfoBtnEnable(false);
// 		InterfaceManager::GetInstance()->SetShowLogBtnEnable(false);
// 		InterfaceManager::GetInstance()->SetDeleteListBtnEnable(false);
// 		InterfaceManager::GetInstance()->SetDelNoServerBtnEnable(false);
// 		InterfaceManager::GetInstance()->SetChangeFullZipBtnEnable(false);
// 	}
// 	else
// 	{
// 		InterfaceManager::GetInstance()->SetShowLogBtnEnable(false);
// 		InterfaceManager::GetInstance()->SetCreateInfoBtnEnable(false);
// 		InterfaceManager::GetInstance()->SetChangeFullZipBtnEnable(false);
// 		InterfaceManager::GetInstance()->SetDeleteListBtnEnable(false);
// 		InterfaceManager::GetInstance()->SetDelNoServerBtnEnable(false);
// 	}

// 	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO1);
// 	pButton->SetCheck(true); 


	return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CALLBACK CAutoUpgradeDlg::ListProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_LBUTTONDOWN:
		return 0;
	}
	return CallWindowProc(m_OldProc,hWnd,iMessage,wParam,lParam);
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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoUpgradeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAutoUpgradeDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnClose();
}

BOOL CAutoUpgradeDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(wParam == IDOK) return FALSE;         //캔슬 방지.
	if(wParam == IDCANCEL) 
	{
		static bool isExcuted = false;
		if(!isExcuted)
		{
			if(m_pPatchManager)
				m_pPatchManager->Close();
			SetTimer(TIME_ID_EXIT, 100, NULL);
			InterfaceManager::GetInstance()->SetAnnounceFirstLine("Now Exiting... Please. Wait...");
			isExcuted = true;
		}
		return FALSE;
	}
	return  CDialog::OnCommand(wParam, lParam);
}


BOOL CAutoUpgradeDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	ThreadManager::GetInstance()->Clear();
	ThreadManager::ReleaseInstance();
       
	g_Pac.ReleaseInstance();

	if(m_pPatchManager)
	{
		delete m_pPatchManager;
		m_pPatchManager = NULL;
	}
	InterfaceManager::ReleaseInstance();
	
	return CDialog::DestroyWindow();
}




void CAutoUpgradeDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case TIME_ID_EXIT:	
		{
			if( !m_pPatchManager  
			  ||( m_pPatchManager && !m_pPatchManager->IsRunThread() )  )
			{
				KillTimer(TIME_ID_EXIT);
				CDialog::OnCommand(IDCANCEL, 0);
			}
			else
			{
				static int i100msCnt = 0;
				i100msCnt++;
				if(i100msCnt > 10)
				{
					static int i = 1;
					CString szTemp ="";
					szTemp.Format("[%d S]", i++);
					InterfaceManager::GetInstance()->SetAnnounceFirstLineAfterDel("Now Exiting... Please. Wait..." + szTemp);
					i100msCnt=0;
				}
			}

		}
		break;
	case TIME_ID_DISPLAY:
		if(m_dwDisplaySec != 0)
		{
			if(!m_szDisplayTitle.IsEmpty())
			{
				CString szTemp ="";
				szTemp.Format("%s [%u S]", (LPCTSTR)m_szDisplayTitle, m_dwDisplaySec);
				InterfaceManager::GetInstance()->SetAnnounceFirstLineAfterDel(szTemp);
				m_dwDisplaySec++;
			}
		}
		break;
	}		
}

void CAutoUpgradeDlg::GetPath( OUT char *szPath, int size, const char*szTitle )
{
	ITEMIDLIST      *pidlBrowse;	
    char            pszPathName[MAX_PATH*2]="";
	
    BROWSEINFO BrInfo;
    BrInfo.hwndOwner =GetSafeHwnd();
    BrInfo.pidlRoot = NULL;
	
    memset(&BrInfo, 0, sizeof(BrInfo));
    BrInfo.pszDisplayName = pszPathName;
    BrInfo.lpszTitle = _T(szTitle);
    BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	BrInfo.lpfn = NULL;
	BrInfo.lParam = NULL;
    BrInfo.iImage=NULL;

	
    pidlBrowse = ::SHBrowseForFolder(&BrInfo);    
    if( pidlBrowse != NULL)		
    {
        ::SHGetPathFromIDList(pidlBrowse, pszPathName);  // 폴더명 얻기 pszPathName
    }   
	
	memset(szPath, 0, size);

	if(sizeof(pszPathName) > size )
		strncpy(szPath, pszPathName, size-1);
	else
		strncpy(szPath, pszPathName, sizeof(pszPathName)-1);
}

bool CAutoUpgradeDlg::LoadPatchFolderINI()
{
	CString szBackUpRoot;
	CString szUploadRoot;

	enum
	{
		ROOT_BACKUP = 0,
		ROOT_UPLOAD = 1,
		MAX_ROOT,

		ITEM_KEY    = 0,
		ITEM_HELP   = 1,
		MAX_ITEM,
	};

	CString szRoot[MAX_ROOT];
	CString szItem[MAX_ROOT][MAX_ITEM]={"BackFolder",   "백업 폴더를 지정하세요.",
	                                    "UploadFolder", "업로드 폴더를 지정하세요."};

	for (int i = 0; i < MAX_ROOT ; i++)
		GetFolderName(szRoot[i], szItem[i][ITEM_KEY], szItem[i][ITEM_HELP]);

	if(m_pPatchManager)
	{
		m_pPatchManager->SetBackUpRoot(szRoot[ROOT_BACKUP]);
		m_pPatchManager->SetUploadRoot(szRoot[ROOT_UPLOAD]);
	}
	return true;

}

void CAutoUpgradeDlg::GetFolderName( OUT CString &rszRoot , const char *szINIKeyName, const char *szTitle )
{
	char szBuf[MAX_PATH]="";
	m_INILoader.LoadString( szINIKeyName, "", szBuf, MAX_PATH );
	if(!strcmp(szBuf, ""))
	{
		char path[1024]="";
		GetPath(path, sizeof(path), szTitle); //szTitle(백업폴더지정하세요)?
		rszRoot = path;
		rszRoot += '\\';
		m_INILoader.SaveString(szINIKeyName, (LPCTSTR)rszRoot);

	}
	else
		rszRoot = szBuf;
}

bool CAutoUpgradeDlg::LoadPatchConnectINI()
{
	CString szIP;
	CString szID;
	CString szPW;
	CString szPatchRoot;
	CString szPatchURL;
	CString szPort;
	int     iZoneType = ZONE_TYPE_SHIPPING;
	bool    bPassive = false;

	enum { MAX_KEY = 5, };
	const char szKeyName[MAX_KEY][MAX_PATH]={"IP","ID","PW","PATCHROOT","PatchURL"};

	for (int i = 0; i < MAX_KEY ; i++)
	{
		char szBuf[MAX_PATH]="";

		m_INILoader.LoadString( szKeyName[i], "", szBuf, MAX_PATH );
		if(strcmp(szBuf, ""))
		{
			switch(i)
			{
			case 0:
				szIP = szBuf;
				break;
			case 1:
				szID = szBuf;
			    break;
			case 2:
				szPW = szBuf;
				break;
			case 3:
				szPatchRoot = szBuf;
				break;
			case 4:
				szPatchURL = szBuf;
				break;
			}
		}
		else
		{
			InterfaceManager::GetInstance()->SetUploadBtnEnable(FALSE);
			InterfaceManager::GetInstance()->SetUpdateBtnEnable(FALSE);
			InterfaceManager::GetInstance()->SetDeleteListBtnEnable(FALSE);
			InterfaceManager::GetInstance()->SetCreateInfoBtnEnable(FALSE);
			InterfaceManager::GetInstance()->SetDelNoServerBtnEnable(FALSE);
			InterfaceManager::GetInstance()->SetPatchInfoFixEnable(FALSE);
			InterfaceManager::GetInstance()->SetCheckFileSyncEnable(FALSE);
			InterfaceManager::GetInstance()->SetMakeFullzipEnable(FALSE);
			return false;
		}
	}

	iZoneType = m_INILoader.LoadInt("ZoneType", 1);
	bPassive  = m_INILoader.LoadBool("PASSIVE", false);
	if( szIP.Find(":") != -1 )
	{
		AfxExtractSubString( szPort, szIP, 1, ':');
		AfxExtractSubString( szIP, szIP, 0, ':');
	}

	if(m_pPatchManager)
	{
		m_pPatchManager->SetIP( szIP );
		m_pPatchManager->SetID( szID );
		m_pPatchManager->SetPW( szPW );
		m_pPatchManager->SetPort( _ttoi(szPort) );
		m_pPatchManager->SetPatchRoot( szPatchRoot );
		m_pPatchManager->SetZoneType( (ZoneType) iZoneType );
		m_pPatchManager->SetPassive( bPassive );
		m_pPatchManager->SetPatchURL( szPatchURL );
	}

	return true;
}
void CAutoUpgradeDlg::OnBnClickedCreateInfo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( AfxMessageBox("시간이 많이 소요되는 작업입니다.\n\n패치 정보 파일을 새롭게 만드시겠습니까?",MB_YESNO) == IDNO)
	{
		return;
	}

	Run(CPatchManager::RS_CREATE_PATCHINFO);
}

void CAutoUpgradeDlg::OnBnClickedUpload() 
{
	// TODO: Add your control notification handler code here
	//CreateEditPad();
	Run(CPatchManager::RS_UPLOAD);
}

void CAutoUpgradeDlg::Run( CPatchManager::RunState eRunState )
{
	int iCurSel = m_ComboBox.GetCurSel();

	if(m_pPatchManager)
		delete m_pPatchManager;
	m_pPatchManager = new CPatchManager();

	if(m_pPatchManager)
	{
		m_pPatchManager->SetRunState( eRunState );
	}

	// set color
	char szTitle[MAX_PATH]="";
	StringCbPrintf(szTitle, sizeof(szTitle), "Zone%d", iCurSel+1);
	m_INILoader.SetTitle(szTitle);

	if(!LoadPatchFolderINI()) 
		return;

	LoadPatchConnectINI();

	if( m_pPatchManager )
	{
		m_pPatchManager->SetBackUpType( (CPatchManager::BackUpType) m_INILoader.LoadInt( "BackupType", CPatchManager::BUT_NONE ) );
		m_pPatchManager->SetUploadLimit( m_INILoader.LoadBool( "UploadLimit", false ) );
		m_pPatchManager->SetVersionPatchRoot( m_strVersionRoot );
	}

	ioLocalManager::SetLocalType( (ioLocalManager::LocalType) m_INILoader.LoadInt( "Local", ioLocalManager::LCT_KOREA ) );

	// 정보가 필요없다는 용이씨 요청으로 디서블
// 	if( m_pPatchManager )
// 	{
// 		enum { MAX_HELP = 7, };
// 		const CString szHelp[MAX_HELP]={"PATCH ROOT: " + m_pPatchManager->GetPatchRoot(),
// 			                            "ID: "		   + m_pPatchManager->GetID(),
// 										"IP: "         + m_pPatchManager->GetIP(),
// 										"PATCH URL:"   + m_pPatchManager->GetPatchURL(),
// 										"Upload: "     + m_pPatchManager->GetUploadRoot(),
// 										"BackUp: "     + m_pPatchManager->GetBackUpRoot(),
// 										"Sync: "       + m_pPatchManager->GetSyncRoot()};
// 		for (int i = 0; i < MAX_HELP ; i++)
// 			InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp[i]);
// 	}
	if( eRunState == CPatchManager::RS_CHECK_FILE_SYNC )
	{
		if(!CheckServerName())
		{
			m_pPatchManager->RunExit();
			return;
		}
	}

	if(m_pPatchManager)
	{
		m_pPatchManager->SetAdmin( m_bAdmin );
		m_pPatchManager->Begin();
	}
}

HBRUSH CAutoUpgradeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if(nCtlColor == CTLCOLOR_LISTBOX)
	{
		if(pWnd->GetDlgCtrlID() == IDC_LIST2)
		{
			//pDC->SetTextColor(RGB(255, 0, 0));
			pDC->SetBkColor(m_bgColor);
			return m_brh;
		}
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

DWORD CAutoUpgradeDlg::GetDisplaySec() const
{
	return m_dwDisplaySec;
}

void CAutoUpgradeDlg::SetDisplaySec( DWORD dwDisplaySec )
{
	m_dwDisplaySec = dwDisplaySec;
}

void CAutoUpgradeDlg::SetDisplayTitle( const CString &rszDisplayTitle )
{
	m_szDisplayTitle = rszDisplayTitle;
}

char * CAutoUpgradeDlg::GetRootDir()
{
	return m_szRootDir;
}


void CAutoUpgradeDlg::OnBnClickedShowLog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	SYSTEMTIME st;
	GetLocalTime(&st);

	CString szName;
	InterfaceManager::GetInstance()->GetCurComboBoxText(szName);
	char szCurLog[MAX_PATH] = "";
	StringCbPrintf( szCurLog, sizeof(szCurLog), "notepad.exe log\\pm[%s]%04d%02d%02d.log", (LPCTSTR)szName,
		            st.wYear, st.wMonth, st.wDay );
	WinExec( szCurLog, SW_SHOW );
}

void CAutoUpgradeDlg::LoadIniForZone()
{
	m_INILoader.SetTitle("Info");
	int iMax    = m_INILoader.LoadInt("MaxZone", 0);
	int iCurSel = m_INILoader.LoadInt("DefaultZone", 1); // 1~MaxZone
	iCurSel--;// combobox array로 변경
	
	if(!COMPARE(iCurSel, 0, iMax))
		iCurSel = 0;

	// add combobox
	for (int i = 0; i < iMax ; i++)
	{
		char szTitle[MAX_PATH]="";
		StringCbPrintf(szTitle, sizeof(szTitle), "Zone%d", i+1);
		m_INILoader.SetTitle(szTitle);

		char szBuf[MAX_PATH]="";
		m_INILoader.LoadString("Name", "", szBuf, sizeof(szBuf));
		if(strcmp(szBuf, ""))
			m_ComboBox.AddString(szBuf);
	}

	m_ComboBox.SetCurSel(iCurSel);
	LoadRGBAndName( iCurSel );

	m_INILoader.SetTitle("Info");
	int iAdmin = m_INILoader.LoadInt("Admin", 0 );
	if( iAdmin == ADMIN_CHECK_VALUE )
	{
		m_bAdmin = true;
	}
}
void CAutoUpgradeDlg::OnCbnSelchangeCombo1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// set color
	int iCurSel = m_ComboBox.GetCurSel();
	LoadRGBAndName(iCurSel);

	m_brh.DeleteObject();
	m_brh.CreateSolidBrush(m_bgColor);
	InvalidateRect(NULL);
}

afx_msg void CAutoUpgradeDlg::OnBnClickedDeleteFileList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Run( CPatchManager::RS_DELETELIST );
}

void CAutoUpgradeDlg::OnBnClickedBtnUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Run( CPatchManager::RS_UPDATE_PATCHINFO );
}

void CAutoUpgradeDlg::OnBnClickedBtnFullZip()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Run( CPatchManager::RS_CHECK_FULLZIP );
}

void CAutoUpgradeDlg::OnBnClickedNoServerDelBtn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Run( CPatchManager::RS_NO_SERVER_FILE_DELETE );
}

void CAutoUpgradeDlg::SaveUploadedDate( const CString &rszDate )
{
	int iCurSel = m_ComboBox.GetCurSel();
	char szTitle[MAX_PATH]="";
	StringCbPrintf(szTitle, sizeof(szTitle), "Zone%d", iCurSel+1);
	m_INILoader.SetTitle(szTitle);
	m_INILoader.SaveString( "UploadedDate", (LPCTSTR) rszDate );
}

void CAutoUpgradeDlg::LoadRGBAndName( int iCurSel )
{
	// rgb
	char szTitle[MAX_PATH]="";
	StringCbPrintf(szTitle, sizeof(szTitle), "Zone%d", iCurSel+1);
	m_INILoader.SetTitle(szTitle);

	int iR = m_INILoader.LoadInt("ListColorR", 0);
	int iG = m_INILoader.LoadInt("ListColorG", 0);
	int iB = m_INILoader.LoadInt("ListColorB", 0);
	m_bgColor = RGB(iR,iG,iB);

	// name and date
	InterfaceManager::GetInstance()->InitAnnounce();

	char szDate[MAX_PATH]="";
	m_INILoader.LoadString("UploadedDate", "", szDate, sizeof(szDate) );
	CString szAnounce;
	szAnounce.Format("Uploaded Date : %s", szDate );
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szAnounce);

	CString szCurName;
	m_ComboBox.GetLBText(iCurSel, szCurName);
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szCurName);
}



void CAutoUpgradeDlg::OnLbnSelchangeList2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//server patch cfg 파일 수정하는 버튼
void CAutoUpgradeDlg::OnBnClickedButton5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Run(CPatchManager::RS_FIX_SERVER_CFG_FILE);
}

//서버&클라 파일 sync 체크하는 기능
void CAutoUpgradeDlg::OnBnClickedButton6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CheckServerName();
	Run(CPatchManager::RS_CHECK_FILE_SYNC);
}

//combobox의 텍스트를 기준으로 체크할 서버를 확인합니다.
bool CAutoUpgradeDlg::CheckServerName()
{
	if( AfxMessageBox("Oops! This work requires a long time... T_T;\n(Compare the patch information and file)",MB_YESNO) == IDNO)
		return false;

	int iCurSel = m_ComboBox.GetCurSel();
	char szTitle[MAX_PATH]="";
	StringCbPrintf(szTitle, sizeof(szTitle), "Zone%d", iCurSel+1);
	m_INILoader.SetTitle(szTitle);

	char szBuf[MAX_PATH]="";
	m_INILoader.LoadString("Name", "", szBuf, sizeof(szBuf));

	CString strName = szBuf;
	CString strKey;
	//combobox를 통해 텍스트 확인
	if( strName.Find(("US")) != -1 ||  strName.Find(("NA")) != -1 )						strKey = "US";
	else if( strName.Find(("SA")) != -1 ||  strName.Find(("LATIN")) != -1 )				strKey = "SA";
	else if( strName.Find(("TL")) != -1 ||  strName.Find(("THAILAND")) != -1 )			strKey = "TL";
	else if( strName.Find(("ID")) != -1 ||  strName.Find(("INDONESIA")) != -1 )			strKey = "ID";
	else if( strName.Find(("EU")) != -1 ||  strName.Find(("EUROPE")) != -1 )			strKey = "EU";
	else if( strName.Find(("TW")) != -1 ||  strName.Find(("TAIWAN")) != -1 )			strKey = "TW";
	else 
	{
		MessageBox("pm.ini파일내의 네이밍 규칙(NAME)을 확인해주세요\n(규칙 : US,NA,SA,LATIN,TL,THAILAND,ID,INDONESIA,EU,EUROPE)\n(cf : 현재 KOR 제외)", "WARNING", MB_OK);
		return false;
	}

	m_INILoader.SetTitle("LIVE SERVER URL");
	char szURL[MAX_PATH]="";
	m_INILoader.LoadString(strKey, "", szURL, sizeof(szURL));
	CString strURL = szURL;
	if( strURL.IsEmpty() )
	{
		MessageBox("해당 LIVE URL이 pm.ini에 입력되지 않았습니다. 확인해주세요.", "WARNING", MB_OK);
		return false;
	}
	if( NULL != m_pPatchManager )
		m_pPatchManager->SetSyncKeynURL(strKey, strURL); //server_patch.cfg체크		

	return true;
}

//파일 폴더별로 패치할 수 있는 정보 로드
void CAutoUpgradeDlg::LoadInPatchInfo()
{
	SetPatchFileBtn( FALSE );
	m_INILoader.SetTitle("UpdateInfo");
	m_bVersionUp	= m_INILoader.LoadBool("VERSION_UP", false);
	
	char szBuf[MAX_PATH]="";
	m_INILoader.LoadString( "VERSIONI_INFO_ROOT", "", szBuf, MAX_PATH );
	m_strVersionRoot = szBuf;

	m_iPacDivide_Size = m_INILoader.LoadInt("DIVIDE_SIZE",0);

	m_INILoader.SetTitle("PatchInfo");
	int iMaxFileType  = m_INILoader.LoadInt("MaxFileType",0);

	
	if( iMaxFileType <= 0 ) //배포용 폴더 설정
	{
		InterfaceManager::GetInstance()->SetSyncPatchEnable(FALSE);
		InterfaceManager::GetInstance()->SetMakePatchEnable(TRUE);
		InterfaceManager::GetInstance()->SetPatchInfoFixEnable(TRUE);
		InterfaceManager::GetInstance()->SetCreateInfoBtnEnable(TRUE);
		return; //iop/ 폴더 업로드 사용 불가
	}
	
	m_bExportBtn = true;
	InterfaceManager::GetInstance()->SetCheckFileSyncEnable(FALSE);
	InterfaceManager::GetInstance()->SetUpdateBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetUploadBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetPatchInfoFixEnable(FALSE);
	InterfaceManager::GetInstance()->SetCreateInfoBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetDeleteListBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetChangeFullZipBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetDelNoServerBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetShowLogBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetMakePatchEnable(FALSE);

	m_vecStrFileType.reserve(iMaxFileType);
	for(int i = 0 ; i < iMaxFileType ; ++i)
	{
		char szBuf[MAX_PATH]="";
		CString GetFileType = "FileType_";
		CString strNum;
		strNum.Format("%d",i);
		GetFileType += strNum;

		m_INILoader.LoadString( GetFileType.GetBuffer(GetFileType.GetLength()), "", szBuf, MAX_PATH );
		if( strcmp(szBuf,"") == 0 )	//파일 정보 로드 실패
		{
			MessageBox("Oops! Sorry Patch File Info is not coincidence","ERROR",MB_OK);
			LOG.PrintTimeAndLog(0, "File Type Load Fail. %s - %s",__FUNCTION__, GetFileType);
			return;
		}
		m_vecStrFileType.push_back(szBuf);
	}

	ZeroMemory(szBuf, MAX_PATH);
	m_INILoader.LoadString("ClientPath","",szBuf, MAX_PATH);
	m_strClientPath = szBuf;

	ZeroMemory(szBuf, MAX_PATH);
	m_INILoader.LoadString("ServerPath","",szBuf, MAX_PATH);
	m_strServerPath = szBuf;

	SetPatchFileBtn( TRUE );


	
	ZeroMemory(szBuf, MAX_PATH);
	m_INILoader.LoadString("DefaultPath","",szBuf, MAX_PATH);
	m_strDefaultPath = szBuf;
}

void CAutoUpgradeDlg::LoadFullZipInfo()
{
	m_INILoader.SetTitle("Info");
	bool bMake = m_INILoader.LoadBool("MakeZip",false);
	
	if(bMake)
	{
		m_bExportBtn = true;
		InterfaceManager::GetInstance()->SetCheckFileSyncEnable(FALSE);
		InterfaceManager::GetInstance()->SetUpdateBtnEnable(FALSE);
		InterfaceManager::GetInstance()->SetUploadBtnEnable(FALSE);
		InterfaceManager::GetInstance()->SetCreateInfoBtnEnable(FALSE);
		InterfaceManager::GetInstance()->SetDeleteListBtnEnable(FALSE);
		InterfaceManager::GetInstance()->SetChangeFullZipBtnEnable(FALSE);
		InterfaceManager::GetInstance()->SetDelNoServerBtnEnable(FALSE);
		InterfaceManager::GetInstance()->SetMakePatchEnable(FALSE);
		InterfaceManager::GetInstance()->SetPatchInfoFixEnable(FALSE);
		InterfaceManager::GetInstance()->SetShowLogBtnEnable(TRUE);
		InterfaceManager::GetInstance()->SetMakeDelFileEnalbe(FALSE);
		InterfaceManager::GetInstance()->SetMakeFullzipEnable(TRUE);
	}
	else
		InterfaceManager::GetInstance()->SetMakeFullzipEnable(FALSE);

}

//버튼 위치가 겹쳐 on/off
void CAutoUpgradeDlg::SetPatchFileBtn( bool bState )
{
	m_btnSelFile.EnableWindow(bState);
	m_btnSelFile.ShowWindow(bState);
	m_btnSelFolder.EnableWindow(bState);
	m_btnSelFolder.ShowWindow(bState);

	m_btnUpload.EnableWindow(!bState);
	m_btnUpload.ShowWindow(!bState);
}


int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	switch (uMsg) 
	{
	case BFFM_INITIALIZED :
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;
}

void CAutoUpgradeDlg::OnBnClickedSelectFolder()
{
	//CString strFolderPath;
	m_bWrongFile = false;

	if(!m_vecPatchFileInfo.empty() )
		m_vecPatchFileInfo.clear();
	if(!GetSelectFolerPath( m_strDefaultPath ))
		return;
	SearchDirectory( m_strDefaultPath );
	if(m_bWrongFile)
	{
		char szMessage[MAX_PATH] = "";
		StringCbPrintf(szMessage, sizeof(szMessage), "Please Check Directory", m_strDefaultPath);
		MessageBox(szMessage,"Warning",MB_OK);
		return;
	}

	if( m_vecPatchFileInfo.size() != 0 )
	{
		m_strPatchInfoSavePath = m_vecPatchFileInfo[0].m_filePath.Left( ( m_vecPatchFileInfo[0].m_filePath.Find("Client\\")) + 7 );
		m_INILoader.SetTitle("PatchInfo");
		m_INILoader.SaveString("CurrentPatchPath",m_strPatchInfoSavePath); //패치 정보 파일을 저장할 디렉토리 기록
	}

	for( int i = 0 ; i < m_vecPatchFileInfo.size() ; ++i )
	{
		if( strcmp(m_vecPatchFileInfo[i].m_file_patch_path,"") == 0 )
		{	//파일 경로 생성
			CString strDestPath = m_strClientPath;
			strDestPath += m_vecPatchFileInfo[i].m_filePath.Right( ( (m_vecPatchFileInfo[i].m_filePath.GetLength() - m_vecPatchFileInfo[i].m_filePath.Find("Client\\")) - 6 ));
			m_vecPatchFileInfo[i].m_file_patch_path = strDestPath;
		}
		FindPatchFilePath(m_vecPatchFileInfo[i].m_fileName , m_vecPatchFileInfo[i].m_file_patch_path, m_vecPatchFileInfo[i].m_filePath );
		
	}

	Run(CPatchManager::RS_SELECT_FOLDER); //패치작업 진행
}

void CAutoUpgradeDlg::OnBnClickedSelectFile()
{
	SelectFileList(); //파일 선택하기

	if( m_vecPatchFileInfo.empty() ) //읽어 들인 파일이 없음
		return;

	LoadRightFileName(); //유효한 파일 명칭을 읽어들인다.

	CString strWrongFile;
	if( !m_vecRightFile.empty() )
	{
		for(int i = 0 ; i< m_vecPatchFileInfo.size() ; ++i )
		{
			if(! IsRightfile( m_vecPatchFileInfo[i].m_fileName ))
			{
				strWrongFile += m_vecPatchFileInfo[i].m_fileName;
				strWrongFile += "\n";
				LOG.PrintTimeAndLog(0,"Wrong File Name - %s (%s)",__FUNCTION__,m_vecPatchFileInfo[i].m_fileName);
			}
		}
	}

	if( !strWrongFile.IsEmpty() )
	{
		MessageBox(strWrongFile, "WrongFileList",MB_OK);
		return;
	}

	if( IsZip( m_vecPatchFileInfo[0].m_filePath ))
	{
		m_bZipFile = true;
		LOG.PrintConsoleLog(0, "%s - Operating zip file", m_vecPatchFileInfo[0].m_filePath);
		
		//서버파일 체크
		if( 0 != m_vecPatchFileInfo.size() )
		{
			switch( CheckServerFile() ) //패치 파일 중 검사
			{
			case GAMESVR_RUNNING:
				MessageBox("Please Exit. Game Server is running", "Oops");
				return;
				break;
			case BILLINGSVR_RUNNING:
				MessageBox("Please Exit. Billing Server is running", "Oops");
				return;
				break;
			case DBAGENT_RUNNING :
				MessageBox("Please Exit. Dbagent Server is running", "Oops");
				return;
				break;
			case MAINSVR_RUNNING : 
				MessageBox("Please Exit. Mains Server is running", "Oops");
				return;
				break;
			case LOGINSVR_RUNNING:
				MessageBox("Please Exit. Login Server is running", "Oops");
				return;
				break;
			case SERVER_CLEAN:
				break;
			}
		}
	}

	//복사할 경로 세팅
	for( int i = 0 ; i < m_vecPatchFileInfo.size() ; ++i )
	{
		if(!FindPatchFilePath(m_vecPatchFileInfo[i].m_fileName , m_vecPatchFileInfo[i].m_filePath, m_vecPatchFileInfo[i].m_file_patch_path ))
		{
			if( m_bZipFile )
			{
				LOG.PrintTimeAndLog( 0 , "%s - Not Support File : %s", __FUNCTION__, m_vecPatchFileInfo[i].m_filePath);
				CString strText = "Not Support File : ";
				strText += m_vecPatchFileInfo[i].m_fileName;
				InterfaceManager::GetInstance()->SetAnnounceFirstLine(strText);
				m_vecPatchFileInfo[i].m_fileName = "";
			}
			continue;
		}
	}
	Run(CPatchManager::RS_SELECT_FILE);
}

void CAutoUpgradeDlg::SelectFileList()
{
	m_vecPatchFileInfo.clear();
	CString strFileType;
	//set file ext
	for(int i= 0 ; i < m_vecStrFileType.size(); ++i)
	{
		char szPath[MAX_PATH]="";
		StringCbPrintf(szPath, sizeof(szPath), "%s (*.%s)|*.%s", m_vecStrFileType[i].MakeUpper(),m_vecStrFileType[i],m_vecStrFileType[i]);
		strFileType += szPath;
		strFileType += "|";
	}
	CString File;
	CString strFileList; 
	 // 2개 확장자 동시 노출. dds와 txt만 선택 가능하도록 하며, 한개의 아이템만 선택 가능
	CString strDefault = _T("Patch Files (*.txt,*.dds,*.zip)|*.txt;*.dds;*.zip||");
	CFileDialog dlg(TRUE, NULL, "Select Patch File", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST /*| OFN_ALLOWMULTISELECT*/, strDefault , this);
	dlg.m_ofn.lpstrInitialDir = m_szRootDir;

	const int c_cMaxFiles = 400 /*선택할 파일 숫자*/ ;	// 메모리 부족현상으로 확장 안해주면 몇개 못씀
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = strFileList.GetBuffer(c_cbBuffSize);
	dlg.GetOFN().nMaxFile = c_cbBuffSize;

	if( dlg.DoModal() == IDOK)
	{
		for(POSITION pos=dlg.GetStartPosition(); pos != NULL;)
		{
			// 전체삭제는 ResetContent
			File = dlg.GetNextPathName(pos);
			S_PATCHINFO sFileInfo;
			sFileInfo.m_filePath = File;
			sFileInfo.m_fileName = File.Right( File.GetLength() - File.ReverseFind('\\') - 1);
			m_vecPatchFileInfo.push_back(sFileInfo);
		}
	}
}

bool CAutoUpgradeDlg::IsRightfile( CString& strName )
{
	char szFileName[MAX_PATH] = {0,};
	char szExt[MAX_PATH] = {0,};
	::_splitpath(strName,NULL,NULL,szFileName,szExt);
	
	for( int i = 0 ; i < m_vecRightFile.size() ; ++i )
	{
		CString strName = szFileName;
		strName += szExt;
		CString strDiff = m_vecRightFile[i];
		if( strcmp( strName.MakeUpper(), m_vecRightFile[i].MakeUpper() ) == 0 )
			return true;
	}
	return false;
}

void CAutoUpgradeDlg::LoadRightFileName()
{
	m_vecRightFile.clear();
	char szFileName[MAX_PATH] = {0,};
	char szExt[MAX_PATH] = {0,};
	::_splitpath(m_vecPatchFileInfo[0].m_filePath,NULL,NULL,szFileName,szExt);
	m_INILoader.SetTitle(szExt+1); //.txt에서 .제거
	int iMaxName  = m_INILoader.LoadInt("MaxName",0);
	if( 0 != iMaxName)
	{
		for(int i = 0 ; i < iMaxName ; ++i)
		{
			char szName[MAX_PATH];
			char szNum[MAX_PATH];
			m_INILoader.LoadString(_itoa(i,szNum,10), "", szName, sizeof(szName));
			if(strcmp(szName,"") ==0)
				return;
			m_vecRightFile.push_back(szName);
		}
	}
	return;
}

bool CAutoUpgradeDlg::GetSelectFolerPath( OUT CString& strPath )
{
	char path[MAX_PATH] = {0,};

	BROWSEINFO bi;
	bi.hwndOwner		= this->m_hWnd;
	bi.pidlRoot			= NULL;
	bi.pszDisplayName	= NULL;					// 선택할 디렉토리가 저장될 버퍼
	bi.lpszTitle		= "Select Folder"; 
	bi.ulFlags			= BIF_RETURNONLYFSDIRS; // 디렉토리 선택 옵션
	bi.lpfn				= BrowseCallbackProc;	// 이벤트에 대한 사용자정의 함수

	if(strPath.IsEmpty())
	{
		::GetCurrentDirectory(MAX_PATH,m_szRootDir);
	}
	else
	{
		strcpy(m_szRootDir,strPath);
	}

	bi.lParam = (LPARAM)m_szRootDir; //최초 디렉토리

	LPITEMIDLIST  pidl = SHBrowseForFolder(&bi);
	// 경로를 얻어온다.
	SHGetPathFromIDList(pidl, path);

	// 사용자가 폴더지정을 한 후 확인(OK)를 눌렀다면 해당 폴더의 경로에 대한 문자열이
	// 존재하므로 end_pos 의 값은 0 이 아니다. 취소를 눌렀다면 0 의 값을 가진다.
	int end_pos = strlen(path);
	strPath = path;
	if(end_pos){
		CString str;
		str.Format("Selected Folder is %s", path);
		MessageBox(str);
		return true;
	}
	else
	{
		MessageBox("Oops No Select Folder~","ERROR",MB_OK);
		return false;
	}
}

void CAutoUpgradeDlg::SearchDirectory( IN CString& strPath, IN bool bGetFilePath )
{
	//하위 폴더를 검색하여 iop파일을 찾습니다.
	CFileFind cFileFinder;  
	BOOL bEnd = cFileFinder.FindFile( strPath + "\\*.*");//폴더 검색

	while(bEnd)
	{
		bEnd = cFileFinder.FindNextFile(); //끝인지 검사
		if( cFileFinder.IsDots())
			continue;

		if( cFileFinder.IsDirectory())
		{
			CString str = cFileFinder.GetFilePath();
			TRACE(_T("%s"),(LPCTSTR)str);
			SearchDirectory(str, bGetFilePath);
		}
		else
		{
			if( bGetFilePath )
			{
				CString strFileName = cFileFinder.GetFilePath();
				strFileName.Replace("\\","/");
				S_PATCHINFO sFileInfo;
				sFileInfo.m_filePath = strFileName;
				strFileName = strFileName.Right( strFileName.GetLength() - strFileName.ReverseFind('/') - 1);
				if( -1 != strFileName.Find(".iop") )
					strFileName = strFileName.Left( strFileName.GetLength() - 4 );
				sFileInfo.m_fileName = strFileName;
				sFileInfo.m_file_patch_path = "";
				m_vecPatchFileInfo.push_back(sFileInfo);
			}
			else
			{
				if( strcmp( cFileFinder.GetFilePath().Right(4), ".iop") == 0 )
				{
					S_PATCHINFO vF;
					vF.m_filePath = cFileFinder.GetFilePath();
					CString strName = vF.m_filePath.Right( vF.m_filePath.GetLength() - vF.m_filePath.ReverseFind('\\') - 1);
					vF.m_fileName = strName.Left( strName.GetLength() - 4); //.iop 삭제
					m_vecPatchFileInfo.push_back( vF );
				}
				else
				{
					char szError[MAX_PATH]="";
					LOG.PrintTimeAndLog(0, "ERROR - Patch File Error %s - %s", cFileFinder.GetFilePath(), __FUNCTION__ );
					m_bWrongFile = true;
				}
			}
		}
	}
	return;
	cFileFinder.Close();
}

bool CAutoUpgradeDlg::FindPatchDirectory( IN CString& strPath )
{
	CString strSearchDir = strPath;
	//클라이언트 패치 경로
	int iPoint = strSearchDir.ReverseFind(_T('\\'));
	strSearchDir = strSearchDir.Left( iPoint ); 

	CString strPatchDir;
	bool bFind = FindDirectory(strSearchDir,"Client", strPatchDir);
	while(!bFind)
	{
		iPoint = strSearchDir.ReverseFind(_T('\\'));
		strSearchDir = strSearchDir.Left( iPoint ); 
		if(FindDirectory(strSearchDir,"Client", strPatchDir))
		{
			bFind = true;
			break;
		}
	}
	if(bFind)
	{
			if(CopyPatchFile( m_strClientPath )) //복사할 경로찾기 D:\overseas\Tool\TotalPatchManager\EU\_BackUpFolder\20141229\ClientPatch\Client
			{ //패치 적용작업 진행
				return true;
			}
	}
	else
	{
		MessageBox("Sorry! Can not found patch path\nplease check to path of patch folder.", "ERROR", MB_OK);
		LOG.PrintTimeAndLog(0,"Sorry! Can not found patch path -%s",__FUNCTION__);
		return false;
	}
	return false;
}

bool CAutoUpgradeDlg::FindDirectory( IN CString& strPath, IN CString strKey, OUT CString& strFindPath)
{
	if(m_bFindTargetFolder) return true;
	CFileFind cFileFinder;  
	BOOL bEnd = cFileFinder.FindFile( strPath + "\\*.*");//폴더 검색

	while(bEnd)
	{
		bEnd = cFileFinder.FindNextFile(); //끝인지 검사
		if( cFileFinder.IsDots())
			continue;

		if( cFileFinder.IsDirectory())
		{
			CString str = cFileFinder.GetFileName();
			CString strFindKey = strKey;
			if( strcmp(str.MakeUpper(), strFindKey.MakeUpper()) == 0) 
			{
				strFindPath = cFileFinder.GetFilePath();
				m_bFindTargetFolder = true;
				return true;
			}
			else
			{
				if(m_bFindTargetFolder) return true;
				CString strFindTargetPath = strPath;
				strFindTargetPath += "\\";
				if(FindDirectory( strFindTargetPath + cFileFinder.GetFileName(), strKey, strFindPath))
					return true;
			}
		}
	}
	return false;
}

bool CAutoUpgradeDlg::CopyPatchFile( IN CString& strPath )
{
	if( !m_vecPatchFileInfo.empty() )
	{
		CString strPatchFileList ="Patch file list\n";
		for( int i = 0 ; i < m_vecPatchFileInfo.size() ; ++i )
		{
			strPatchFileList += m_vecPatchFileInfo[i].m_filePath;
			strPatchFileList += "\n";
		}
		strPatchFileList += "\ndo you agree this file patch?(File overwrite)";
		if( IDYES == AfxMessageBox( strPatchFileList, MB_YESNO))
			return true;
	}
	return false;
}

bool CAutoUpgradeDlg::FindPatchFilePath( IN CString strFileName, IN CString strFilePatch,  OUT CString& strDestPatch )
{
	if( m_bZipFile && m_strClientPath.IsEmpty() && m_strServerPath.IsEmpty() )
	{	//zip파일은 파일 복사 작업을 하므로, 경로가 없으면 파일을 검색할 수 없습니다.
		MessageBox("Can't not Found Patch Path\nPlease Check the Setting File", "Oops", MB_OK );
		return false;
	}
	m_bFindTargetFolder = false;
	::GetCurrentDirectory(MAX_PATH,m_szRootDir);
	CString strSearchDir = m_szRootDir;
	int iPoint = strSearchDir.ReverseFind(_T('\\'));
	strSearchDir = strSearchDir.Left( iPoint ); 

	CString strSearchPath = m_strClientPath;
	int iFileType = 0;
	if( -1 != strFilePatch.Find("ClientPatch") )
	{
		m_bServerFile = false;
		iFileType = CLIENT_FILE;
	}
	else if( -1 != strFilePatch.Find("ServerPatch") )
	{
		m_bServerFile = true;
		iFileType = SERVER_FILE;
		strSearchPath = m_strServerPath;
	}
	else if( -1 != strFilePatch.Find("ToolPatch") )
	{
		if( -1 != strFilePatch.Find("LSMonitor") || -1 != strFilePatch.Find("PatchManager")) 
			return false;
		m_bServerFile = false;
		iFileType = TOOL_FILE;
		strSearchPath = m_strClientPath;
	}

	//m_strPatchPat(패치 최상위 경로)h에서 파일을 검색
	//return FindDirectoryInFile( iFileType, m_strPatchPath, strFileName, strFilePatch, strDestPatch);//true;

	//m_strPatchPath 에서 패치할 경로검색
	return FindPatchDirectory(iFileType, strSearchPath, strFilePatch, strDestPatch);
}

bool CAutoUpgradeDlg::FindDirectoryInFile( IN int iFileType, IN CString strPath ,IN CString strFileName, IN CString strFilePatch, OUT CString& strDestPath )
{
	CFileFind cFileFinder;  
	BOOL bEnd = cFileFinder.FindFile( strPath + "\\*.*");//폴더 검색

	while(bEnd)
	{
		bEnd = cFileFinder.FindNextFile(); //끝인지 검사
		if( cFileFinder.IsDots())
			continue;

		if( cFileFinder.IsDirectory())
		{
			CString str = cFileFinder.GetFilePath();
			TRACE(_T("%s"),(LPCTSTR)str);
			if( FindDirectoryInFile( iFileType, str, strFileName, strFilePatch, strDestPath ) )
				return true;
		}
		else
		{
			CString strName = cFileFinder.GetFileName();
			if( -1 != strName.Find(".iop") )
				strName = strName.Left( strName.GetLength() - 4); //.iop 삭제
			if( strcmp( strFileName.MakeUpper(), strName.MakeUpper()) == 0 )
			{
				//client patch? server patch? 비교
				strDestPath = cFileFinder.GetFilePath();
				if( iFileType == 0 )
					return true;
				CString strDifPath = strDestPath;
				strDifPath.MakeUpper();
				if( iFileType == CLIENT_FILE && ( -1 != strDifPath.Find("PATCHSERVER")))
					return true;
				if( iFileType == SERVER_FILE && ( -1 != strDifPath.Find("ZONE")) )
					return true;
			}
		}
	}
	cFileFinder.Close();
	return false;
}

bool CAutoUpgradeDlg::FindPatchDirectory( IN int iType, IN CString strPatchPath, IN CString strFilePath ,OUT CString& DestPath )
{
	CFileFind cFileFinder = NULL;  
	BOOL bEnd = cFileFinder.FindFile( strPatchPath + "\\*.*");//폴더 검색

	if( iType == SERVER_FILE )
	{
		DestPath = strPatchPath;
		DestPath += strFilePath.Right( strFilePath.GetLength() - (strFilePath.Find("ServerPatch") + 12) );
		DestPath.Replace('\\','/');
		return true;
	}
	if( iType == CLIENT_FILE /*&& ( -1 != strDifPath.Find("PATCHSERVER"))*/ )
	{
		DestPath = strPatchPath;
		DestPath += strFilePath.Right( strFilePath.GetLength() - (strFilePath.Find("ClientPatch") + 12) );
		DestPath.Replace('\\','/');
		return true;
	}
	if( iType == TOOL_FILE )
	{
		DestPath = strPatchPath;
		DestPath += strFilePath.Right( strFilePath.GetLength() - (strFilePath.Find("ToolPatch") + 10) );
		DestPath.Replace('\\','/');
		return true;
	}


// 	while(bEnd)
// 	{
// 		bEnd = cFileFinder.FindNextFile(); //끝인지 검사
// 		if( cFileFinder.IsDots())
// 			continue;
// 
// 		if( cFileFinder.IsDirectory())
// 		{
// 			CString str = cFileFinder.GetFilePath();
// 			CString strDifPath = str;
// 			strDifPath.MakeUpper();
// 			TRACE(_T("%s"),(LPCTSTR)str);
// 			if( iType == CLIENT_FILE /*&& ( -1 != strDifPath.Find("PATCHSERVER"))*/ )
// 			{
// 				DestPath = str;
// 				DestPath += '\\';
// 				DestPath += strFilePath.Right( strFilePath.GetLength() - (strFilePath.Find("/Client/") + 8) );
// 				DestPath.Replace('\\','/');
// 				return true;
// 			}
//  			if( iType == SERVER_FILE /*&& ( -1 != strDifPath.Find("ZONE"))*/ )
//  			{
//  				DestPath = str;
//  				DestPath += '\\';
//  				DestPath += strFilePath.Right( strFilePath.GetLength() - (strFilePath.Find("ServerPatch") + 12) );
//  				DestPath.Replace('\\','/');
//  				return true;
//  			}
// 			if( FindPatchDirectory( iType, str , strFilePath ,DestPath ) )
// 				return true;
// 		}
// 	}
// 	cFileFinder.Close();
	return false;
}

void CAutoUpgradeDlg::OnBnClickedBtnPPac()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Run(CPatchManager::RS_MAKE_PATCH_PAC);
	return;
}


void CAutoUpgradeDlg::OnBnClickedBtnSPac()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Run(CPatchManager::RS_SYNC_PATCH_PAC);
	return;
}

bool CAutoUpgradeDlg::IsZip( IN const CString& strFilePath )
{
	if( -1 == strFilePath.Find(".zip"))
		return false;
	
	m_pPatchManager->DirectoryDelete();
	Sleep(2000);
	CString strText = "ExtractZip : ";
	strText += strFilePath;
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(strText);
	LOG.PrintConsoleLog(0,"%s Extract Zip file", __FUNCTION__);

	char newCmd[MAX_PATH] = {0,};
	sprintf( newCmd, " %s %s", "-d config", strFilePath );
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO         StartupInfo;
	StartupInfo.cb               = sizeof(STARTUPINFO);
	StartupInfo.lpReserved       = NULL;
	StartupInfo.lpDesktop        = NULL;
	StartupInfo.lpTitle          = NULL;
	StartupInfo.dwFlags          = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow      = SW_SHOWNORMAL;
	StartupInfo.cbReserved2      = 0;
	StartupInfo.lpReserved2      = NULL;

	
	CreateProcess( "unzip.exe", (LPSTR)newCmd, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo,&ProcessInfo);

	while( true )
	{
		if( !CheckRunningProcess("unzip.exe") )
			break;
	}

	m_vecPatchFileInfo.clear();
	char szPath[MAX_PATH]= "";
	::GetCurrentDirectory(MAX_PATH, szPath);
	CString strPath = szPath;
	strPath += "\\config\\";
	
	SearchDirectory( strPath , true );
// 	if(!ExtractZip( (LPSTR)(LPCTSTR)strFilePath,"\\config" ))
// 	{
// 		MessageBox("Error Zip file","Oops~",MB_OK);
// 		return false;
// 	}
	return true;
}

bool CAutoUpgradeDlg::ExtractZip( const string zipFile, const string dstPath )
{
	//Zip open
	unzFile uFile = unzOpen( zipFile.c_str() );
	if( 0 == uFile )
	{
		MessageBox("uFile is Error", "ERROR ZIP", MB_OK );
		return false;
	}

	int iRet = unzGoToFirstFile( uFile );
	bool bEnd = false;
	if( UNZ_OK == iRet)
	{
		m_vecPatchFileInfo.clear(); //초기화
		while( true )
		{
			if( bEnd ) break;
			unz_file_info file_Info;
			char szfileName[MAX_PATH] = "";
			iRet = unzGetCurrentFileInfo( uFile, &file_Info, szfileName, sizeof(szfileName), NULL, 0, NULL, 0 );
			
			if( UNZ_OK != iRet )
			{
				MessageBox( "Fail Get File Info", "ERROR ZIP", MB_OK );
				break;
			}

			iRet = unzOpenCurrentFile( uFile );
			if( UNZ_OK != iRet )
			{
				MessageBox( "Fail File Open", "ERROR ZIP", MB_OK );
				break;
			}

			char szBuff[MAX_PATH] = "";
			::GetCurrentDirectory( MAX_PATH, szBuff );
			string strTargetFileName = szBuff;
			strTargetFileName += dstPath;
			if( *strTargetFileName.rbegin()  != '/' )
				strTargetFileName.append( "/" );
			strTargetFileName.append( szfileName );

			MakePatchToDirectory( strTargetFileName );
			if( !IsDirectory( strTargetFileName ))
			{
				ofstream ofstrm( strTargetFileName.c_str() );
				if( !ofstrm )
				{
					InterfaceManager::GetInstance()->SetAnnounceFirstLine( "ERROR ZIP! "); //압축 해제 실패 
					break;
				}
				//ExtractZip
				char szBuf[MAX_BUFFER_SIZE] = "";
				while( true )
				{
					int iLen = unzReadCurrentFile( uFile, szBuf, sizeof(szBuf) );
					if( iLen < 0 ) 
					{
						bEnd = true;
						break;
					}
					if( iLen == 0 )
						break;

					ofstrm.write( szBuf, iLen );
					ZeroMemory(szBuf, sizeof(szBuf));
				}
				ofstrm.close();
				//패치 파일 리스트 추가
				CString strFileName = strTargetFileName.c_str();
				strFileName.Replace("\\","/");
				S_PATCHINFO sFileInfo;
				sFileInfo.m_filePath = strFileName;
				strFileName = strFileName.Right( strFileName.GetLength() - strFileName.ReverseFind('/') - 1);
				if( -1 != strFileName.Find(".iop") )
					strFileName = strFileName.Left( strFileName.GetLength() - 4 );
				sFileInfo.m_fileName = strFileName;
				sFileInfo.m_file_patch_path = "";
				m_vecPatchFileInfo.push_back(sFileInfo);
			}
			unzCloseCurrentFile( uFile );
			iRet = unzGoToNextFile( uFile );

			if( UNZ_END_OF_LIST_OF_FILE == iRet )
				break;
		}

	}
	else
	{
		MessageBox("Fail to unzGoToFirstFile()","ERROR ZIP",MB_OK );
		return false;
	}

	if( uFile )
	{
		unzCloseCurrentFile( uFile );
		unzClose( uFile );
	}
	return true;
}

bool CAutoUpgradeDlg::IsDirectory( const string & path )
{
	DWORD ret = GetFileAttributes( path.c_str() );

	if( ret == 0xffffffff)
		return false;

	if( ret & FILE_ATTRIBUTE_DIRECTORY )
		return true;

	return false;
}

bool CAutoUpgradeDlg::MakePatchToDirectory( const string & fullPath )
{
	string  path = fullPath;

	// 파일경로에서 디렉토리 경로만 얻음.
	string::size_type pos = path.find_last_of( "/\\" );
	if( pos != string::npos )
	{
		path.erase( ++pos, string::npos );
	}

	// 네트워크 경로 인가? 예제) "\\192.168.0.1\test" 
	pos = 0;
	if( path.compare( 0, 2, "\\\\") == 0 )
		pos = 2;

	// 상위 경로를 찾아가며 디렉토리를 순서대로 만든다.
	while( (pos = path.find_first_of( "/\\", pos )) != string::npos )
	{
		string subPath = path.substr( 0, pos++ );

		if(IsDirectory( subPath )==false)
			_mkdir( subPath.c_str() );
	}

	return IsDirectory( fullPath );
}

int CAutoUpgradeDlg::CheckServerFile()
{
	for( int i = 0 ; i < m_vecPatchFileInfo.size() ; ++i )
	{
		if( -1 != m_vecPatchFileInfo[i].m_filePath.Find("ls_billingsvr.exe"))
		{
			if( CheckRunningProcess("ls_billingsvr") )
				return BILLINGSVR_RUNNING;
		}
		if( -1 != m_vecPatchFileInfo[i].m_filePath.Find("ls_dbagent.exe"))
		{
			if( CheckRunningProcess("ls_dbagent") )
				return DBAGENT_RUNNING;
		}
		if( -1 != m_vecPatchFileInfo[i].m_filePath.Find("ls_gamesvr.exe"))
		{
			if( CheckRunningProcess("ls_gamesvr") )
				return GAMESVR_RUNNING;
		}
		if( -1 != m_vecPatchFileInfo[i].m_filePath.Find("ls_loginsvr.exe"))
		{
			if( CheckRunningProcess("ls_loginsvr") )
				return LOGINSVR_RUNNING;
		}
		if( -1 != m_vecPatchFileInfo[i].m_filePath.Find("ls_mainsvr.exe"))
		{
			if( CheckRunningProcess("ls_mainsvr") )
				return MAINSVR_RUNNING;
		}
	}
	return SERVER_CLEAN;
}

bool CAutoUpgradeDlg::CheckRunningProcess( IN CString szProcessName )
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 );
	if ( (int)hSnapshot != -1 )
	{
		szProcessName.MakeUpper();
		PROCESSENTRY32 pe32 ;
		pe32.dwSize=sizeof(PROCESSENTRY32);
		BOOL bContinue ;
		CString strProcessName;
		if ( (int)hSnapshot != -1 )
		{
			PROCESSENTRY32 pe32 ;
			pe32.dwSize=sizeof(PROCESSENTRY32);
			BOOL bContinue ;
			CString strProcessName;
			if ( Process32First ( hSnapshot, &pe32 ) )
			{
				do
				{
					strProcessName = pe32.szExeFile; //strProcessName이 프로세스 이름; 
					strProcessName = strProcessName.MakeUpper();
					if( ( strProcessName.Find(szProcessName,0) != -1 ) )
					{
						return true;
					}
					bContinue = Process32Next ( hSnapshot, &pe32 );
				} while ( bContinue );
			}
			CloseHandle( hSnapshot );
			return false;
		}
	}
	return false;
}

void CAutoUpgradeDlg::OnBnClickedMakeFullzipBtn()
{
	Run( CPatchManager::RS_MAKE_FULLZIP );
}


void CAutoUpgradeDlg::OnBnClickedButton7()
{
	//mkae delete list
	Run( CPatchManager::RS_MAKE_DELETE_PATCH );
}
