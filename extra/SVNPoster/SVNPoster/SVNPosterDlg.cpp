
// SVNPosterDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "SVNPoster.h"
#include "SVNPosterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSVNPosterDlg 대화 상자


static int CALLBACK BrowseForFolder_CallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if(uMsg == BFFM_INITIALIZED)
	{
		SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData);
	}
	return 0;
}


CSVNPosterDlg::CSVNPosterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSVNPosterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSVNPosterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RIVISION, m_rivision);
	DDX_Control(pDX, IDC_CHECK_ZONE_ALPHA, m_zone_alpha);
	DDX_Control(pDX, IDC_CHECK_ZONE_DEV, m_zone_dev);
	DDX_Control(pDX, IDC_CHECK_ZONE_QA, m_zone_qa);
	DDX_Control(pDX, IDC_CHECK_ZONE_BETA, m_zone_beta);
	DDX_Control(pDX, IDC_EDIT_LOG, m_log);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_progressBar);
}

BEGIN_MESSAGE_MAP(CSVNPosterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SELECTFOLDER, &CSVNPosterDlg::OnBnClickedButtonSVNRepository)
	ON_BN_CLICKED(IDC_BUTTON_COPYTOCLIPBOARD, &CSVNPosterDlg::OnBnClickedButtonCopyToClipboard)
END_MESSAGE_MAP()


// CSVNPosterDlg 메시지 처리기

BOOL CSVNPosterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	//ShowWindow(SW_MINIMIZE);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	GetRegistry(_T("repository"), m_repository);
	//m_zone_dev.SetCheck(TRUE);
	m_rivision.SetFocus();

	return FALSE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSVNPosterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSVNPosterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSVNPosterDlg::MakeTemporaries()
{
	if(m_repository.IsEmpty())
	{
		OnBnClickedButtonSVNRepository();
	}

	m_progressBar.SetRange(1, 100);
	Progress( 10 );

	int fileNumber = rand() % 10000;
	m_fileName.Format(_T("%d.txt"), fileNumber);

	DeleteFile(m_fileName);

	CString temp;
	m_rivision.GetWindowText(temp);
	int revision = _tstoi(temp);

	Progress( 20 );
	TCHAR command[512];
	_stprintf_s(command, _countof(command), _T("/c \"svn.exe log %s -v -r %d >> %s\""), m_repository, revision, m_fileName);
	ShellExecute(NULL , NULL, _T("cmd.exe") , command, NULL, SW_HIDE);
	
	Progress( 40 );
	if(LoadTemporaries())
	{
		Progress( 70 );
		if(!ParseTemporaries(revision))
			return;

		Progress( 100 );
		Print();
	}

	DeleteFile(m_fileName);
}

BOOL CSVNPosterDlg::LoadTemporaries()
{
	for(int i = 0 ; i < 3 ; i++)
	{
		if(m_SVN.Load(m_fileName))
		{
			return TRUE;
		}

		Sleep(400);
		Progress( GetProgress() + 10 );
	}

	MessageBox(_T("Can't read revision"), _T("error"), MB_OK);
	return FALSE;
}

BOOL CSVNPosterDlg::ParseTemporaries(int& revision)
{
	CString dist;
	if(m_zone_alpha.GetCheck())
	{
		dist = dist + _T("알파존");
	}
	if(m_zone_dev.GetCheck())
	{
		if(!dist.IsEmpty())
			dist = dist + _T(", ");
		dist = dist + _T("개발존");
	}
	if(m_zone_qa.GetCheck())
	{
		if(!dist.IsEmpty())
			dist = dist + _T(", ");
		dist = dist + _T("QA존");
	}
	if(m_zone_beta.GetCheck())
	{
		if(!dist.IsEmpty())
			dist = dist + _T(", ");
		dist = _T("베타존");
	}

	if(dist.IsEmpty())
	{
		MessageBox(_T("Please check one of the ZONE"), _T("warning"), MB_OK);
		return FALSE;
	}

	m_output.Empty();
	m_SVN.Parse(dist, revision, m_output);
	return TRUE;
}

void CSVNPosterDlg::Print()
{
	CopyToClipBoard();
	m_log.SetWindowText(m_output);
}

BOOL CSVNPosterDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(WM_KEYDOWN == pMsg->message )
	{
		if(VK_RETURN == pMsg->wParam)
		{
			if(m_rivision.GetSafeHwnd() == pMsg->hwnd)
			{
				MakeTemporaries();
				return TRUE;
			}
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL SelectFolder(HWND hWnd, CString &folder)
{
	LPITEMIDLIST pidlBrowse;
	TCHAR         szFolder[MAX_PATH];

	BROWSEINFO brInfo;
	::ZeroMemory(&brInfo, sizeof(BROWSEINFO));
	brInfo.hwndOwner      = hWnd; // 윈도우 핸들
	brInfo.pidlRoot       = NULL;
	brInfo.pszDisplayName = NULL;
	brInfo.lpszTitle      = _T("Select SVN Repository");
	brInfo.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT; // 옵션 지정
	brInfo.lpfn           = BrowseForFolder_CallbackProc; //콜백함수
	brInfo.lParam         = (LPARAM)(LPCSTR)(LPCTSTR)(folder);

	::ZeroMemory(szFolder, sizeof(szFolder));

	pidlBrowse = ::SHBrowseForFolder(&brInfo); // 다이얼로그를 띄운다.
	if(pidlBrowse != NULL) 
	{
		::SHGetPathFromIDList(pidlBrowse, szFolder); // 패스를 얻어온다.
		folder = szFolder;
		return TRUE;
	}
	return FALSE;
}

void CSVNPosterDlg::OnBnClickedButtonSVNRepository()
{
	if(SelectFolder(GetSafeHwnd(), m_repository))
	{
		SetRegistry(_T("repository"), m_repository);
		SetCurrentDirectory(m_repository);
	}
}

void CSVNPosterDlg::SetRegistry(const TCHAR* option, CString& value)
{
	TCHAR regKey[512];
	_stprintf_s(regKey, _countof(regKey), _T("software\\lostsaga\\SVN_Poster"));
	
	HKEY key;
	DWORD disp = 0;
	LONG ret = RegCreateKeyEx(HKEY_CURRENT_USER, regKey, 
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp);
	if(ret != ERROR_SUCCESS) return;

	DWORD length = _tcslen(value.GetString()) * (sizeof(TCHAR)) + 1;
	RegSetValueEx(key, option, 0, REG_EXPAND_SZ, (BYTE*)value.GetBuffer(), length);

	RegCloseKey(key);
}

BOOL CSVNPosterDlg::GetRegistry(const TCHAR* option, CString& value)
{
	HKEY key;
	LONG ret = RegOpenKeyEx(HKEY_CURRENT_USER, _T("software\\lostsaga\\SVN_Poster"), 0, KEY_ALL_ACCESS, &key);
	if(ERROR_SUCCESS != ret) return 0;

	TCHAR temp[512];
	DWORD length = sizeof(temp);

	DWORD type = REG_EXPAND_SZ;
	if(RegQueryValueEx(key, option, NULL, &type, (LPBYTE)temp, &length) == ERROR_SUCCESS)
	{
		RegCloseKey(key);
		value = temp;
		return TRUE;
	}
	RegCloseKey(key);
	return FALSE;
}

void CSVNPosterDlg::CopyToClipBoard()
{
	HGLOBAL hMem; 
	if( OpenClipboard() )
	{
		if( EmptyClipboard() )
 		{
 			hMem = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, m_output.GetLength()+1 );
			if( hMem )
 			{
 				LPTSTR pClipData = (LPTSTR)GlobalLock(hMem);
 				pClipData[0] = 0;

 				memcpy( pClipData, m_output, m_output.GetLength() );
				pClipData[m_output.GetLength()] = NULL;

 				SetClipboardData( CF_OEMTEXT, hMem );
 				GlobalUnlock( hMem );
 			}

 			CloseClipboard();
 			GlobalFree( hMem );
 		}
 	}
}

void CSVNPosterDlg::OnBnClickedButtonCopyToClipboard()
{
	CopyToClipBoard();
}
