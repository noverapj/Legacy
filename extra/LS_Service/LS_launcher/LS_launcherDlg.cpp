// LS_launcherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "LS_launcherDlg.h"
#include "AddIPDialog.h"
#include "WinMessage.h"
#include "Timer.h"
#include "cFile.h"
#include <string>

const int TIME_MONITOR = 1000 * 2;
const int TIME_WATCHDOG = 1000 * 30;
const int TIME_REFRESH = 2000;
const int TIME_BATCH_WAIT = 1000;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLS_launcherDlg dialog


extern  void Tokenize(const CString& str, TOKENS& tokens, const CString& delimiters);

BOOL g_spread = FALSE;
std::vector<CLS_launcherDlg*> g_launchers;

CLS_launcherDlg::CLS_launcherDlg(const TCHAR* parameter, CWnd* pParent /*=NULL*/)
	: CDialog(CLS_launcherDlg::IDD, pParent), m_loaded(FALSE), m_tabCurFocus(0), m_fileName(_T("")), m_batchName(_T(""))
{
	m_parameter = parameter;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLS_launcherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SERVERS, m_serverTabs);
	DDX_Control(pDX, IDC_CHECK_WATCHDOG, m_chk_watchdog);
	DDX_Control(pDX, IDC_CHECK_APPLYALL, m_chk_applyall);
	DDX_Control(pDX, IDC_CHECK_RECURSIVE, m_chk_recusive);
	DDX_Control(pDX, IDC_CHECK_CONSOLE, m_chk_console);
	DDX_Control(pDX, IDC_CHECK_Monitor, m_chk_monitor);
}

BEGIN_MESSAGE_MAP(CLS_launcherDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
	ON_MESSAGE(IDC_SERVICE_ADD_IP, OnAddIP)
	ON_MESSAGE(IDC_SERVICE_DELETE_IP, OnDeleteIP)
	ON_MESSAGE(IDC_SERVICE_SPREAD, OnSpread)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SERVERS, &CLS_launcherDlg::OnTcnSelchangeTabServers)
	ON_BN_CLICKED(IDC_CHECK_WATCHDOG, &CLS_launcherDlg::OnBnClickedCheckWatchdog)
	ON_BN_CLICKED(IDC_CHECK_APPLYALL, &CLS_launcherDlg::OnBnClickedCheckApplyall)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_RECURSIVE, &CLS_launcherDlg::OnBnClickedCheckRecursive)
	ON_BN_CLICKED(IDC_CHECK_CONSOLE, &CLS_launcherDlg::OnBnClickedCheckConsole)
	ON_BN_CLICKED(IDC_CHECK_Monitor, &CLS_launcherDlg::OnBnClickedCheckMonitor)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// CLS_launcherDlg message handlers

BOOL CLS_launcherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if(m_parameter.GetLength() > 2)
	{
		Load(m_parameter);
	}

	m_serverTabs.CreatePopup();

	LoadOption();

	BeginBatches();
	SetTimer(TIMER_REFRESH, TIME_REFRESH, 0);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLS_launcherDlg::OnPaint()
{ 
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

BOOL IsAvailable(const int limitCount, const int currentCount)
{
	if(currentCount < limitCount)
		return FALSE;
	else
		return TRUE;
}

void CLS_launcherDlg::ConsoleStart()
{
	AllocConsole();
	freopen("CONOUT$", "wt", stdout);
}

void CLS_launcherDlg::ConsoleStop()
{
	if(m_batchManager.IsConsoleMode())
		FreeConsole();
}

BOOL CLS_launcherDlg::ParseToken(TOKENS &tokens)
{
	//BatchManager.h memeber m_batchType true == consoleType
	int tokenCount = tokens.size();
	int limitCount = 1;
	int tokenIndex =0;

	if(_tcscmp(tokens[tokenIndex].GetBuffer(0), _T("-c")) == 0)
	{
		ConsoleStart();
		m_batchManager.SetConsoleMode(TRUE);
		++tokenIndex;
		++limitCount;
	}
	
	if(!IsAvailable(limitCount, tokenCount))
		return FALSE;
	else
	{
		m_fileName = tokens[tokenIndex++]; 	
		if(limitCount == tokenCount)
			return TRUE;

	}
	m_batchName = tokens[tokenIndex];
	return TRUE;
}
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLS_launcherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLS_launcherDlg::Load(const TCHAR* filePath)
{
	// TODO: Add extra initialization here
	CString temp = filePath;

	TOKENS tokens;
	int tokenNum = 0; 
	Tokenize(temp, tokens, _T(" "));

	if(tokens.empty())				
		return FALSE;

	if(!ParseToken(tokens))
		return FALSE;

	if(!ReadFile(m_fileName.GetBuffer()))		
		return FALSE;
	
	if(!ParseServer())					
		return FALSE; //서버 연결.

	InitTab();

	if( !m_batchName.IsEmpty() )
	{
		if(!m_batchManager.AddBatchFile(m_batchName.GetBuffer(), this ) ) 
			return FALSE;
	}

	m_loaded = TRUE;
	return TRUE;
}

BOOL CLS_launcherDlg::LoadOption()
{
	OPTION()->LoadOptions();

	if(OPTION()->GetOption(OPTION_MONITOR))
	{
		m_chk_monitor.SetCheck(1);
		SetTimer(TIMER_MONITOR, TIME_MONITOR, 0);
	}
	if(OPTION()->GetOption(OPTION_RECURSIVE))
	{
		m_chk_recusive.SetCheck(1);
	}
	if(OPTION()->GetOption(OPTION_CONSOLE))
	{
		m_chk_console.SetCheck(1);
	}
	return TRUE;
}

BOOL CLS_launcherDlg::ReadFile(const TCHAR* filePath)
{
	cFileReader file;
	if(file.Open(filePath))
	{
		DWORD length = file.GetFileSize();

		BYTE* buffer = new BYTE[length+1];
		file.Read(buffer, length);
		buffer[length] = NULL;
		file.Close();

		switch(file.GetBOM())
		{
		case cFile::FILEFORMAT_ANSI :
			m_buffer = (char*)buffer;
			break;

		case cFile::FILEFORMAT_UNICODE_LE :
			m_buffer = (TCHAR*)buffer;
			break;

		default :
			break;
		}

		delete []buffer;
		_tprintf(_T("fileName : %s\n\n"), m_fileName);

		return TRUE;
	}

	if(m_batchManager.IsConsoleMode())
	{
		_tprintf(_T("Batch type error('-c' is only possible) or Ip Text fail\n"));
	}
	return FALSE;
}

BOOL CLS_launcherDlg::SaveFile()
{
	cFileWriter file;

	if(file.Open(m_fileName))
	{
		CString szServerInfo = GetServerInfo();

		file.WriteFormat(_T("%s\r\n"), szServerInfo);
		file.Close();
		return TRUE;
	}
	return FALSE;
}

CString CLS_launcherDlg::GetServerInfo()
{
	CString szServerInfo;
	CString name, ip, port;

	CServerInfo* serverInfo = NULL;
	SERVERINFO_LIST::iterator it;
	for(it = m_serverInfos.begin() ; it != m_serverInfos.end() ; ++it)
	{
		serverInfo = *it;
		if(serverInfo)
		{
			TCHAR tempPort[100] = {0};
			name.SetString(serverInfo->m_name);
			ip.SetString(serverInfo->m_IP);
			_itot_s(serverInfo->m_port, tempPort, 10);
			port.SetString(tempPort);

			szServerInfo.Append(name);
			szServerInfo.Append(_T(", "));
			szServerInfo.Append(ip);
			szServerInfo.Append(_T(", "));
			szServerInfo.Append(port);

			szServerInfo.Append(_T("\r\n"));
		}
	}

	return szServerInfo;
}

int CLS_launcherDlg::GetTabDialogCount()
{
	return m_serverTabs.GetCount();
}

CServerDialog* CLS_launcherDlg::GetTabDialog(int index)
{
	return m_serverTabs.GetDialog(index);
}

BOOL CLS_launcherDlg::ParseServer()
{
	CString buffer = m_buffer;
	TOKENS lines;
	Tokenize(buffer, lines, _T("\r\n"));
	if(lines.size() == 0) 
		return FALSE;

	m_serverInfos.clear();

	TOKENS tokens;
	CString name, IP;
	for(UINT i = 0 ; i < lines.size() ; i++)
	{
		tokens.clear();
		Tokenize(lines[i], tokens, _T(","));
		if(tokens.size() != 3) 
			return TRUE;

		name	= tokens[0];
		IP		= tokens[1];
		name.Trim();
		IP.Trim();

		if(g_spread && (g_launchers.size() != i))
		{
			continue;
		}

		CServerInfo* serverInfo = new CServerInfo;
		serverInfo->m_name	= name;
		serverInfo->m_IP	= IP;
		serverInfo->m_port	= _tstoi(tokens[2]);

		m_serverInfos.push_back(serverInfo);
	}
	return TRUE;
}

BOOL CLS_launcherDlg::InitTab()
{
	CServerInfo* serverInfo = NULL;
	SERVERINFO_LIST::iterator it;
	for(it = m_serverInfos.begin() ; it != m_serverInfos.end() ; ++it)
	{
		serverInfo = *it;
		if(serverInfo)
			m_serverTabs.AddTab(serverInfo->m_name, serverInfo->m_IP, serverInfo->m_port);
	}

	// 포커스 초기화 0으로..
	m_serverTabs.ShowTab(m_serverInfos.size() -1, FALSE);
	m_serverTabs.ShowTab(0, TRUE);
	return TRUE;
}

void CLS_launcherDlg::DeleteServerInfo(CString name)
{
	CServerInfo* serverInfo = NULL;
	SERVERINFO_LIST::iterator it;
	for(it = m_serverInfos.begin(); it != m_serverInfos.end(); ++it)
	{
		serverInfo = *it;
		if(serverInfo)
		{
			if(serverInfo->m_name == name)
			{
				m_serverInfos.remove(serverInfo);
				delete serverInfo;
				serverInfo = NULL;
				break;
			}
		}
	}
}

void CLS_launcherDlg::Quit()
{
	if(m_batchManager.IsConsoleMode())
	{
		CWnd* wnd=AfxGetMainWnd();
		if(wnd)
		{
			wnd->PostMessageW(WM_CLOSE);
		}
	}	
}

BOOL CLS_launcherDlg::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_KEYDOWN :
		{
			if(VK_RETURN == pMsg->wParam) return FALSE;
			if(VK_ESCAPE == pMsg->wParam) return FALSE;
			if(VK_F5 == pMsg->wParam)
			{
				TRACE(_T("global F5\n"));
				PostMessageToAll(NULL, ECOMMAND::REFRESH);
				return FALSE;
			}
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CLS_launcherDlg::OnDropFiles(HDROP hDropInfo)
{
	if(!m_loaded)
	{
		TCHAR pathName[512];
		int files = DragQueryFile(hDropInfo, 0xFFFFFFFF, pathName, _countof(pathName));
		if(files > 0)
		{
			DragQueryFile(hDropInfo, 0, pathName, _countof(pathName));
		}

		m_parameter = pathName;
		if(!Load(m_parameter))
		{
			MessageBox(_T("file error"), _T("error"), MB_OK);
			return;
		}

		CDialog::OnDropFiles(hDropInfo);
	}
}

LRESULT CLS_launcherDlg::OnAddIP(WPARAM wParam, LPARAM lParam)
{
	CAddIPDialog dlg;

	if(IDOK == dlg.DoModal())
	{
		CString name = dlg.GetTapName().GetBuffer();
		CString ip = dlg.GetIP().GetBuffer();

		m_serverTabs.AddTab( name, ip, dlg.GetPort() );
		AddServerInfo(name, ip, dlg.GetPort());

		// 파일 저장.
		SaveFile();
	}

	return 0;
}

LRESULT CLS_launcherDlg::OnDeleteIP(WPARAM wParam, LPARAM lParam)
{
	// 해당 탭 삭제.
	m_serverTabs.DeleteTab(m_tabCurFocus);

	SaveFile();

	return 0;
}

LRESULT CLS_launcherDlg::OnSpread(WPARAM wParam, LPARAM lParam)
{
	if(g_spread)
	{
		// 모으다
		g_spread = FALSE;
		int count = g_launchers.size();
		for(std::vector<CLS_launcherDlg*>::iterator it = g_launchers.begin() ; it != g_launchers.end() ; ++it)
		{
			CLS_launcherDlg* dlg = *it;
			if(dlg)
			{
				dlg->ShowWindow(SW_HIDE);
			}
		}

		ShowMainDlg(SW_SHOW);
	}
	else
	{
		// 퍼트리다
		g_spread = TRUE;

		int cx	= 0;
		int cy	= 0;

		CRect rect;
		GetWindowRect(rect);

		int cxIcon = GetSystemMetrics(SM_CXFULLSCREEN);
		int cyIcon = GetSystemMetrics(SM_CYFULLSCREEN);

		if(g_launchers.size() == 0)
		{
			for(uint32 i = 0 ; i < m_serverInfos.size() ; i++)
			{
				CLS_launcherDlg* dlg = new CLS_launcherDlg(m_parameter);
				dlg->Create(IDD_SERVICELAUNCHER_DIALOG, this);
				dlg->m_chk_applyall.SetCheck(this->m_chk_applyall.GetCheck());
				dlg->m_chk_console.SetCheck(this->m_chk_console.GetCheck());
				dlg->m_chk_recusive.SetCheck(this->m_chk_recusive.GetCheck());
				dlg->m_chk_watchdog.SetCheck(this->m_chk_watchdog.GetCheck());

				UINT style = GetWindowLong(m_hWnd, GWL_STYLE);
				style = style | WS_OVERLAPPEDWINDOW;
				style = style & ~WS_CHILD;
				SetWindowLong(m_hWnd, GWL_STYLE, style);

				g_launchers.push_back(dlg);
			}
		}

		ShowMainDlg(SW_HIDE);
		for(std::vector<CLS_launcherDlg*>::iterator it = g_launchers.begin() ; it != g_launchers.end() ; ++it)
		{                                    
			CLS_launcherDlg* dlg = *it;
			if(dlg)
			{
				dlg->SetWindowPos(&CWnd::wndBottom, cx, cy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				dlg->ShowWindow(SW_SHOW);

				//cx += rect.Width();
				cx += 90;
				if((cx+rect.Width()) > cxIcon)
				{
					cx = 0;
					//cy += rect.Height();
					cy += 70;
					if((cy+rect.Height()) > cyIcon)
					{
						cx = 90;
						cy = 70;
					}
				}
			}
		}
	}
	return 0;
}

void CLS_launcherDlg::AddServerInfo(CString title, CString IP, UINT port)
{
	CServerInfo* serverInfo = new CServerInfo;
	serverInfo->m_name	= title;
	serverInfo->m_IP	= IP;
	serverInfo->m_port	= port;

	m_serverInfos.push_back(serverInfo);
}

void CLS_launcherDlg::ShowMainDlg(int nCmdShow)
{
	CLS_launcherDlg* dlg = static_cast<CLS_launcherDlg*>( AfxGetMainWnd() );
	if(dlg)
	{
		dlg->ShowWindow(nCmdShow);
	}
}

void CLS_launcherDlg::PostMessageToAll(WPARAM wParam, LPARAM lParam)
{
	for(unsigned int i = 0 ; i < m_serverTabs.m_dialogs.size() ; ++i)
	{
		m_serverTabs.m_dialogs[i]->PostMessage(WM_AUTOMATIC, wParam, lParam);
	}
}

void CLS_launcherDlg::BeginBatches()
{
	if(!m_batchManager.IsConsoleMode() && !m_batchName.IsEmpty() && m_batchManager.HaveBaches())
	{
		m_batchManager.ShowDialog();
	}
		
	SetTimer( TIMER_BATCHSTARTCHECK, TIME_BATCH_WAIT, 0 );
}

void CLS_launcherDlg::EndBatches()
{
	KillTimer( TIMER_BATCHSTARTCHECK );
	KillTimer( TIMER_BATCHCONTINUECHECK );

	m_batchManager.End();
}

void CLS_launcherDlg::BatchStartCheck()
{
	// 타이머 먼저 죽이고
	if( m_serverTabs.IsAllReady() )
	{
		KillTimer( TIMER_BATCHSTARTCHECK );

		m_batchManager.ResetExecute();
		if( m_batchManager.Execute() )
		{
			// 모두 성공
			m_batchManager.EndProgress(100, _T("completed"), TRUE);
		}
		else
		{
			if( m_batchManager.GetCurrentState() != CST_WAITING )
			{
				m_batchManager.EndProgress(0, _T("failed"), TRUE);
			}
		}
	}
	//else
	//{
	//	m_batchManager.Error(_T("Server connect"), _T("failed"));
	//}
}

void CLS_launcherDlg::BatchContinueCheck()
{
	if( m_batchManager.CheckContinue() )
	{
		KillTimer( TIMER_BATCHCONTINUECHECK );

		m_batchManager.EndProgress(100, _T("completed"),FALSE);
		
		m_batchManager.Next();
		if( m_batchManager.Execute() )
		{
			// 모두 성공
			m_batchManager.EndProgress(100, _T("completed"), TRUE);
			return;
		}
	}

	if( m_batchManager.GetCurrentState() != CST_WAITING )
	{
		// error
		m_batchManager.EndProgress(0, _T("failed"), FALSE);
	}
}

void CLS_launcherDlg::BatchWait()
{
	KillTimer( TIMER_BATCHCONTINUECHECK );
	SetTimer( TIMER_BATCHCONTINUECHECK, TIME_BATCH_WAIT, 0 );
}

void CLS_launcherDlg::BatchProgress(int progress)
{
	m_batchManager.DoingProgress( progress );
}

void CLS_launcherDlg::OnTcnSelchangeTabServers(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CLS_launcherDlg::OnBnClickedCheckWatchdog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OPTION()->SetOption(OPTION_WATCHDOG, m_chk_watchdog.GetCheck());
	if(OPTION()->GetOption(OPTION_WATCHDOG))
	{
		SetTimer(TIMER_WATCHDOG, TIME_WATCHDOG, 0);
	}
	else
	{
		KillTimer(TIMER_WATCHDOG);
	}
}

void CLS_launcherDlg::OnBnClickedCheckMonitor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OPTION()->SetOption(OPTION_MONITOR, m_chk_monitor.GetCheck());
	if(OPTION()->GetOption(OPTION_MONITOR))
	{
		SetTimer(TIMER_MONITOR, TIME_MONITOR, 0);
	}
	else
	{
		KillTimer(TIMER_MONITOR);
	}
}

void CLS_launcherDlg::OnBnClickedCheckApplyall()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_chk_applyall.GetCheck() == TRUE)
	{
		if(TRUE == MessageBox(_T("Are you sure? Check ApplyAll"), _T("Warning"), MB_OKCANCEL))
		{
			OPTION()->SetOption(OPTION_APPLYALL, TRUE);
		}
		else
		{
			OPTION()->SetOption(OPTION_APPLYALL, FALSE);
			m_chk_applyall.SetCheck(FALSE);
		}
	}
	else
		OPTION()->SetOption(OPTION_APPLYALL, FALSE);
}

void CLS_launcherDlg::OnBnClickedCheckRecursive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OPTION()->SetOption(OPTION_RECURSIVE, m_chk_recusive.GetCheck());
}

void CLS_launcherDlg::OnBnClickedCheckConsole()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OPTION()->SetOption(OPTION_CONSOLE, m_chk_console.GetCheck());
}

void CLS_launcherDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case TIMER_REFRESH :
		{
			PostMessageToAll(NULL, ECOMMAND::REFRESH);
		}
		break;

	case TIMER_WATCHDOG :
		{
			PostMessageToAll(OPTION()->GetOption(OPTION_WATCHDOG), ECOMMAND::WATCHDOG);
		}
		break;

	case TIMER_MONITOR :
		{
			PostMessageToAll(OPTION()->GetOption(OPTION_MONITOR), ECOMMAND::MONITOR);
		}
		break;

	case TIMER_BATCHSTARTCHECK:
		{
			BatchStartCheck();
		}
		break;

	case TIMER_BATCHCONTINUECHECK:
		{
			BatchContinueCheck();
		}
		break;
	}
	CDialog::OnTimer(nIDEvent);
}


void CLS_launcherDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialog::OnWindowPosChanging(lpwndpos);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_batchManager.IsConsoleMode())
	{
 		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}
	else
	{
		lpwndpos->flags |= SWP_SHOWWINDOW;
	}
}