
// LS_WatchdogDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "LS_Watchdog.h"
#include "LS_WatchdogDlg.h"
#include "afxdialogex.h"
#include <windows.h>
#include <tlhelp32.h>
#include <Psapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLS_WatchdogDlg 대화 상자


char WATCH_FILE[512];


CLS_WatchdogDlg::CLS_WatchdogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLS_WatchdogDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLS_WatchdogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MODULENAME, m_moduleName);
}

BEGIN_MESSAGE_MAP(CLS_WatchdogDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_WATCH, &CLS_WatchdogDlg::OnBnClickedButtonWatch)
END_MESSAGE_MAP()


// CLS_WatchdogDlg 메시지 처리기

BOOL CLS_WatchdogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	//ShowWindow(SW_MINIMIZE);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetTimer(1, 3000, NULL );
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLS_WatchdogDlg::OnPaint()
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
HCURSOR CLS_WatchdogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

ULONG ProcIDFromWnd(HWND hwnd) // 윈도우 핸들로 프로세스 아이디 얻기   
{   
	ULONG idProc;   
	GetWindowThreadProcessId( hwnd, &idProc );   
	return idProc;   
}

HWND GetWinHandle(ULONG pid) // 프로세스 아이디로 윈도우 핸들 얻기   
{   
	HWND tempHwnd = FindWindow(NULL,NULL); // 최상위 윈도우 핸들 찾기   

	while( tempHwnd != NULL )   
	{   
		if( GetParent(tempHwnd) == NULL ) // 최상위 핸들인지 체크, 버튼 등도 핸들을 가질 수 있으므로 무시하기 위해   
		if( pid == ProcIDFromWnd(tempHwnd) )   
			return tempHwnd;   

		tempHwnd = GetWindow(tempHwnd, GW_HWNDNEXT); // 다음 윈도우 핸들 찾기   
	}   
	return NULL;   
} 


void CLS_WatchdogDlg::CollectProcess()
{
	m_modules.clear();

	std::string matchFile = WATCH_FILE;
	
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(snapshot)
	{
		PROCESSENTRY32 processEntry32;
		BOOL found = FALSE;

		HMODULE mod;

		processEntry32.dwSize = sizeof(PROCESSENTRY32);
		found = Process32First(snapshot, &processEntry32);
		while( found )
		{
			TRACE(_T("%s[%d]\n"), processEntry32.szExeFile, processEntry32.th32ProcessID);

			if(matchFile == processEntry32.szExeFile)
			{
				HANDLE process = OpenProcess( SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processEntry32.th32ProcessID);
				if(process != NULL)
				{
					HWND handle = GetWinHandle(processEntry32.th32ProcessID);

					TCHAR fileExt[512];
					DWORD length = 0;

					if(EnumProcessModules(process, &mod, sizeof(mod), &length))
					{
						GetModuleFileNameEx(process, mod, fileExt, _countof(fileExt));

						std::string module = fileExt;

						m_modules[processEntry32.th32ProcessID] = module;
						TRACE(_T("module : %s\n"), fileExt);
					}
				}
			}
			found = Process32Next(snapshot, &processEntry32);
		}

		CloseHandle(snapshot);
	}
}

BOOL CLS_WatchdogDlg::FindProcess(const int PID)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(snapshot)
	{
		PROCESSENTRY32 processEntry32;
		BOOL found = FALSE;

		processEntry32.dwSize = sizeof(PROCESSENTRY32);
		found = Process32First(snapshot, &processEntry32);
		while( found )
		{
			if(PID == processEntry32.th32ProcessID)
			{
				return TRUE;
			}
			found = Process32Next(snapshot, &processEntry32);
		}

		CloseHandle(snapshot);
	}
	return FALSE;
}

void SetCurrentModulePath(char* temp)
{
	CString fileExt = temp;
	fileExt.Replace(WATCH_FILE, "");

	SetCurrentDirectory(fileExt);
}


void CLS_WatchdogDlg::WatchProcess()
{
	for(MODULES::iterator it = m_modules.begin(); it != m_modules.end() ; ++it)
	{
		int PID				= it->first;
		std::string module	= it->second;

		if(!FindProcess( PID))
		{
			char path[512];
			strcpy_s(path, sizeof(path), module.c_str());
			
			SetCurrentModulePath(path);

			TCHAR command[512];
			_stprintf_s(command, _countof(command), _T("%s"), module.c_str());
			HINSTANCE inst = ShellExecute(NULL , NULL, module.c_str() , command, NULL, SW_SHOW);
			PID = GetProcessId(inst);

			it = m_modules.erase(it);
		}
	}
}

void CLS_WatchdogDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case 1 :
		{
			WatchProcess();
			CollectProcess();
		}
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CLS_WatchdogDlg::OnBnClickedButtonWatch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString file;
	m_moduleName.GetWindowText(file);
	
	strcpy_s(WATCH_FILE, sizeof(WATCH_FILE), file);
	CollectProcess();
}
