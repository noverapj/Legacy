// ioDaumDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "ioDaumDlg.h"
#include ".\iodaumdlg.h"
#include "HelpFunc.h"
#include "AutoUpgradeDlg.h"
#include "ioINILoaderAU.h"
#include <strsafe.h>


// ioDaumDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(ioDaumDlg, CDialog)
ioDaumDlg::ioDaumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ioDaumDlg::IDD, pParent)
{
	m_hKey = NULL;
}

ioDaumDlg::~ioDaumDlg()
{
}

void ioDaumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SHORTCUT, m_CheckBtnShortcut);
	//DDX_Control(pDX, IDC_CHECK_TOOLBAR, m_CheckBtnToolbar);
	DDX_Control(pDX, IDC_BUTTON_DESCRIPTION, m_BtnDescription);
}


BEGIN_MESSAGE_MAP(ioDaumDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_REGISTERED_MESSAGE(WM_LINK_CLICKED, OnLinkCliked)
END_MESSAGE_MAP()


// ioDaumDlg 메시지 처리기입니다.

void ioDaumDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(nStatus != 0) return;

	//----------창 중간에 띄우기 ---------------------
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
		NULL,
		workrect.left + (workwidth-winwidth) / 2,
		workrect.top + (workheight-winheight) / 2,
		winwidth, winheight, 
		SWP_SHOWWINDOW);
	//----------------------------------------------------------
}

BOOL ioDaumDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_KEYDOWN &&pMsg->wParam == VK_RETURN 
	  ||pMsg->message == WM_KEYDOWN &&pMsg->wParam == VK_ESCAPE) 
		return FALSE;

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL ioDaumDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_BtnDescription.SetRegularColor(RGB(255,0,0));
	m_BtnDescription.SetHoverColor(RGB(255,0,0));
	m_BtnDescription.SizeToContent();

	if (_daum_SetStartPage( false ) == 0) 
		m_CheckBtnShortcut.SetCheck(1);
	
	//if( !IsExistDaumToolbar() )
	//	m_CheckBtnToolbar.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void ioDaumDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	EndDialog(0);

	if ( m_CheckBtnShortcut.GetCheck() == 1 && _daum_SetStartPage( false ) == 0 ) 
	{
		//1. 시작페이지설정
		_daum_SetStartPage( true );
		_daum_SendStartPage("D_NGAME");
		// 2. 검색공급자세팅
		_daum_SetDefaultSearchPage();
	}

	// 3. 툴바 설치
// 	if( m_CheckBtnToolbar.GetCheck() == 1 && !IsExistDaumToolbar() )
// 	{
// 		if( !AfxGetApp() )
// 			return;
// 		if( !AfxGetApp()->GetMainWnd() )
// 			return;
// 		CAutoUpgradeDlg *pDlg = ( CAutoUpgradeDlg* ) AfxGetApp()->GetMainWnd();
// 		if( !pDlg )
// 			return;
// 
// 		// 폴더
// 		CString szINIPath = "info\\";
// 		szINIPath += pDlg->GetININame();
// 		ioINILoaderAU kIniLoder( (LPCTSTR)szINIPath );
// 		kIniLoder.SetTitle("autoupgrade_info");
// 		char szToolbarURL[MAX_PATH]="";
// 		kIniLoder.LoadString("toolbar_url", "", szToolbarURL, sizeof(szToolbarURL) );
// 	
// 		char szFileName[MAX_PATH]="";
// 		char szFileExt[MAX_PATH]="";
// 		_splitpath( szToolbarURL, NULL, NULL, szFileName, szFileExt );
// 
// 		char szCreateFilePath[MAX_PATH]="";
// 		StringCbPrintf( szCreateFilePath, sizeof( szCreateFilePath ), "%s\\losadown\\%s%s", pDlg->GetRootDir(), szFileName , szFileExt );
// 
// 		char szCreateFolder[MAX_PATH]="";
// 		StringCbPrintf( szCreateFolder, sizeof( szCreateFolder ), "%s\\losadown", pDlg->GetRootDir() );
// 
// 		if( Setup( szCreateFolder, szToolbarURL, szCreateFilePath ) )
// 		{
// 			StringCbCat( szCreateFilePath, sizeof( szCreateFilePath ), " /S" );
// 			WinExec( szCreateFilePath , SW_SHOW ); // pDlg->ExcuteProcess()로 하면 스텔스 설치가 안되어 WinExec 사용
// 		}
// 	}
}

// From DAUM
double  ioDaumDlg::_daum_GetVersion()
{
	return RELEASE_VERSION;
}


int ioDaumDlg::_daum_SendStartPage(LPCTSTR szCode)
{
	LPTSTR szURL;
	LPTSTR szRet;
	HINTERNET hSession = ::InternetOpen (_T("StartPage"), 0, NULL, NULL, NULL);

	if (hSession) 
	{
		szURL = new TCHAR[1024];
		_stprintf(szURL, _T("/startpage/index.html?hcode=%s"), szCode);
		HINTERNET hConnect = ::InternetConnect(hSession, _T("start.tiara.daum.net"), 80, NULL, NULL, INTERNET_SCHEME_HTTP, NULL, NULL);

		if (hConnect) 
		{
			HINTERNET hObject = ::HttpOpenRequest(hConnect, "GET", szURL, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_RELOAD, NULL);

			if (hObject) 
			{
				DWORD dwRead = 0;

				if(::HttpSendRequest(hObject, NULL, 0L, NULL, 0)) 
				{
					szRet = new TCHAR[1024];
					::InternetReadFile(hObject, szRet, 1023, &dwRead);
					::InternetCloseHandle(hObject);
					delete szRet;
					delete szURL;
					return 0;
				}
			}
		}
		delete szURL;
	}

	return -1;
}


int ioDaumDlg::_daum_SetStartPage( bool bSetPage )
{
	LPCTSTR lpValueName = _T("Start Page");
	LPTSTR lpValue;
	int	 isHome = 0;

	if( !OpenReg(HKEY_CURRENT_USER, MAIN) ) return -1;

	if( (lpValue = ReadReg(lpValueName)) != NULL ) {
		tolowers(lpValue);
		if( _tcsstr(lpValue, _T("www.daum.net")) ) isHome = 1;
		delete lpValue;
	}
	if (isHome == 0 && bSetPage ) {
		lpValue = _T("http://www.daum.net/");
		if( !WriteReg(lpValueName, REG_SZ, (LPBYTE)lpValue, _tcslen(lpValue) * sizeof(TCHAR)) ) {
			return -1;
		}
	}

	CloseReg();

	return isHome;  // 1:이미 시작페이지, 0:시작설정OK, -1:에러
}


int  ioDaumDlg::_daum_GetIEVersion()
{
	LPCTSTR lpValueName = _T("IE");
	LPTSTR lpValue;
	int rVal = 0;

	if( !OpenReg(HKEY_LOCAL_MACHINE, VERSION) ) return 0;

	if( (lpValue = ReadReg(lpValueName)) != NULL ) {
		rVal = _ttoi(lpValue);
		delete lpValue;
	}

	CloseReg();

	return rVal;
}


int  ioDaumDlg::_daum_SetDefaultSearchPage()
{
	int IEVersion;

	IEVersion = _daum_GetIEVersion();
	if(IEVersion == 7) {
		return SetDefaultSearchPage_forIE7();
	} else {
		return SetDefaultSearchPage_forIE6();
	}
}

bool ioDaumDlg::OpenReg(HKEY key, LPCTSTR subkey)
{
	DWORD dwDisp;
	LONG rVal = 0;

	rVal = RegCreateKeyEx(key, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,NULL, &m_hKey, &dwDisp);
	if (rVal == ERROR_SUCCESS) {
		return true;
	}

	return false;
}


void ioDaumDlg::CloseReg()
{
	if(m_hKey != NULL) RegCloseKey(m_hKey);
}


bool ioDaumDlg::WriteReg(LPCTSTR dwName, DWORD dwType, const BYTE* Value, DWORD ValueLength)
{
	if(m_hKey == NULL) return false;

	LONG rVal = 0;

	rVal = RegSetValueEx(m_hKey, dwName, 0, dwType, Value, ValueLength);
	if (rVal != ERROR_SUCCESS) {
		return false;
	}

	return true;
}


LPTSTR ioDaumDlg::ReadReg(LPCTSTR Name)
{
	if(m_hKey == NULL) return false;

	DWORD datasize;
	LPTSTR rbuf;
	LONG rVal = 0;

	RegQueryValueEx(m_hKey, Name, 0, NULL, NULL, &datasize);
	if(datasize != 0)
	{
		rbuf = new TCHAR[datasize];
		rVal = RegQueryValueEx(m_hKey, Name, 0, NULL, (LPBYTE)rbuf, &datasize);
		if (rVal != ERROR_SUCCESS)
		{
			return NULL;
		}

		return rbuf;
	}

	return NULL;
}


void ioDaumDlg::tolowers(LPTSTR buf)
{
	if(buf == NULL) return;
	int size = _tcslen(buf);

	for(int i=0; i<size; i++) {
		*buf = _totlower(*buf);
		buf ++;
	}
}


int ioDaumDlg::SetDefaultSearchPage_forIE6()
{
	LPTSTR lpValueName;
	LPTSTR lpValue;

	if( !OpenReg(HKEY_CURRENT_USER, SEARCH_URL) ) return -1;

	lpValueName = _T("provider");
	lpValue  = _T("koda");
	if( !WriteReg((LPCTSTR)lpValueName, REG_SZ, (LPBYTE)lpValue, _tcslen(lpValue) * sizeof(TCHAR)) ) return -1;
	lpValue  = _T("http://search.daum.net/search?nil_profile=ie&ref_code=NGAME&q=%s");
	if( !WriteReg(NULL, REG_SZ, (LPBYTE)lpValue, _tcslen(lpValue) * sizeof(TCHAR)) ) return -1;

	CloseReg();

	if( !OpenReg(HKEY_CURRENT_USER, AUTO_SEARCH) ) return -1;

	lpValueName = _T("{CFBFAE00-17A6-11D0-99CB-00C04FD64497}");
	lpValue = "";
	if( !WriteReg((LPCTSTR)lpValueName, REG_SZ, (LPBYTE)lpValue, _tcslen(lpValue) * sizeof(TCHAR)) ) return -1;

	CloseReg();

	return 0;  // 0:설정OK, -1:에러
}


int ioDaumDlg::SetDefaultSearchPage_forIE7()
{
	LPTSTR lpValueName;
	LPTSTR lpValue;

	if( !OpenReg(HKEY_CURRENT_USER, SEARCH_SCORP) ) return -1;

	lpValueName = _T("DisplayName");
	lpValue  = _T("Daum");
	if( !WriteReg((LPCTSTR)lpValueName, REG_SZ, (LPBYTE)lpValue, _tcslen(lpValue) * sizeof(TCHAR)) ) return -1;
	lpValueName = _T("URL");
	lpValue  = _T("http://search.daum.net/search?nil_profile=ie&ref_code=NGAME&q={searchTerms}");
	if( !WriteReg((LPCTSTR)lpValueName, REG_SZ, (LPBYTE)lpValue, _tcslen(lpValue) * sizeof(TCHAR)) ) return -1;
	lpValueName = _T("SortIndex");
	int tmp = 1;
	if( !WriteReg((LPCTSTR)lpValueName, REG_DWORD, (LPBYTE)&tmp, sizeof(int)) ) return -1;
	CloseReg();

	if( !OpenReg(HKEY_CURRENT_USER, DEFLT_SRCH_SCRP) ) return -1;
	lpValueName = _T("DefaultScope");
	lpValue = _T("{3A40E547-20FD-44a2-94D0-1C98342D1507}");
	if( !WriteReg((LPCTSTR)lpValueName, REG_SZ, (LPBYTE)lpValue, _tcslen(lpValue) * sizeof(TCHAR)) ) return -1;
	CloseReg();

	if( !OpenReg(HKEY_CURRENT_USER, AUTO_SEARCH) ) return -1;
	lpValueName = _T("{CFBFAE00-17A6-11D0-99CB-00C04FD64497}");
	lpValue = "";
	if( !WriteReg((LPCTSTR)lpValueName, REG_SZ, (LPBYTE)lpValue, _tcslen(lpValue) * sizeof(TCHAR)) ) return -1;
	CloseReg();

	return 0;  // 0:설정OK, -1:에러
}
// From DAUM END

bool ioDaumDlg::IsExistDaumToolbar()
{
	char szInstallDir[MAX_PATH*2]="";
	Help::RegReadString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Daum\\DaumToolbar", "InstallDir" ,"" ,szInstallDir,sizeof(szInstallDir));
	if( strcmp( szInstallDir, "" ) == 0 )
		return false;

	return true;
}

bool ioDaumDlg::Setup( LPCTSTR szCreateFolder, LPCTSTR szDownLoadURL, LPCTSTR szCreateFileName )
{
	Help::SetCreateDirectoryByFullPath( szCreateFolder );

	SetFileAttributes(szCreateFileName,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(szCreateFileName);

	HRESULT hr = URLDownloadToFile(0, szDownLoadURL, szCreateFileName, 0, NULL);
	if(hr != S_OK)
		return false;

	return true;
}

LRESULT ioDaumDlg::OnLinkCliked(WPARAM wParam, LPARAM lParam)
{
	UINT nLinkID = (UINT)wParam;

	switch(nLinkID)
	{
	case IDC_BUTTON_DESCRIPTION:
		MessageBox( "Daum 바로가기 패키지란, 바로가기패키지 설정 및 PC의 바탕화면, 빠른실행 영역에\n\n바로가기 아이콘을 제공하며 인터넷브라우저의 검색공급자를 Daum으로 설정함으로써\n\nDaum의 서비스를 보다 빠르고 편리하게 이용할 수 있는 기능입니다.", "Daum LostSaga" , MB_OK );
		break;
	}

	return 0;
}