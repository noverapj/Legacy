
// LSControllerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "LSController.h"
#include "LSControllerDlg.h"
#include "ConfigLoader.h"
#include "ProcessManager.h"

#include <algorithm>
#include <memory>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT CLSControllerDlg::WM_APP_LOGDATA = WM_APP + 1;
const UINT CLSControllerDlg::WM_APP_STOPDONE = WM_APP + 2;


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLSControllerDlg dialog



CLSControllerDlg::CLSControllerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LSCONTROLLER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLSControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LIST, m_list);
	DDX_Control(pDX, IDC_CBO_LOG, m_cboLog);
	DDX_Control(pDX, IDC_CBO_FILE, m_cboFile);
	DDX_Control(pDX, IDC_EDT_LOG, m_edLog);
}

BEGIN_MESSAGE_MAP(CLSControllerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_START, &CLSControllerDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CLSControllerDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_BTN_RESTART, &CLSControllerDlg::OnBnClickedRestart)
	ON_BN_CLICKED(IDC_BTN_STARTALL, &CLSControllerDlg::OnBnClickedStartAll)
	ON_BN_CLICKED(IDC_BTN_STOPALL, &CLSControllerDlg::OnBnClickedStopAll)
	ON_CBN_SELCHANGE(IDC_CBO_LOG, &CLSControllerDlg::OnCbnSelchangeLog)
	ON_CBN_SELCHANGE(IDC_CBO_FILE, &CLSControllerDlg::OnCbnSelchangeFile)
	ON_MESSAGE(WM_APP_LOGDATA, &CLSControllerDlg::OnLogData)
	ON_MESSAGE(WM_APP_STOPDONE, &CLSControllerDlg::OnStopDone)
END_MESSAGE_MAP()


// CLSControllerDlg message handlers

BOOL CLSControllerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	SetupList();
	LoadConfig();
	RefreshStatus();
	UpdateButtons();

	SetTimer(IDT_STATUS, 1000, nullptr);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CLSControllerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLSControllerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display when the user drags
//  the minimized window.
HCURSOR CLSControllerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLSControllerDlg::SetupList()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("Name"),   LVCFMT_LEFT, 150);
	m_list.InsertColumn(1, _T("Status"), LVCFMT_LEFT, 70);
	m_list.InsertColumn(2, _T("PID"),    LVCFMT_LEFT, 60);
	m_list.InsertColumn(3, _T("Order"), LVCFMT_LEFT, 45);
	m_list.InsertColumn(4, _T("Config"), LVCFMT_LEFT, 120);

	m_edLog.LimitText(0);
	m_edLog.SetWindowText(_T("Select a server and a log file to tail."));
}

void CLSControllerDlg::LoadConfig()
{
	CString strPath;
	TCHAR szExe[MAX_PATH] = {};
	::GetModuleFileNameW(nullptr, szExe, MAX_PATH);
	LPTSTR pszSlash = _tcsrchr(szExe, _T('\\'));
	if (pszSlash) *(pszSlash + 1) = _T('\0');
	strPath = CString(szExe) + _T("config.ini");

	CString strError;
	if (!ConfigLoader::Load(strPath, m_arrServers, strError))
	{
		AfxMessageBox(strError, MB_ICONWARNING | MB_OK);
	}

	// Populate rows (row index == m_arrServers index)
	for (size_t i = 0; i < m_arrServers.size(); ++i)
	{
		const ServerEntry& e = m_arrServers[i];
		const int nRow = static_cast<int>(i);
		m_list.InsertItem(nRow, e.m_strName);
		m_list.SetItemText(nRow, 1, _T("Stopped"));
		m_list.SetItemText(nRow, 2, _T("-"));
		CString strOrder;
		strOrder.Format(_T("%d"), e.m_nOrder);
		m_list.SetItemText(nRow, 3, strOrder);
		m_list.SetItemText(nRow, 4, e.m_strIni);
	}

	PopulateLogCombo();
}

void CLSControllerDlg::PopulateLogCombo()
{
	m_cboLog.ResetContent();
	for (size_t i = 0; i < m_arrServers.size(); ++i)
	{
		const int nItem = m_cboLog.AddString(m_arrServers[i].m_strName);
		m_cboLog.SetItemData(nItem, static_cast<DWORD_PTR>(i));
	}
}

void CLSControllerDlg::PopulateFileCombo(int nServerIndex)
{
	m_cboFile.ResetContent();

	if (nServerIndex < 0 || nServerIndex >= static_cast<int>(m_arrServers.size()))
		return;

	// List the newest .log files in <Dir>\MLOG (newest first)
	CString strPattern = m_arrServers[nServerIndex].m_strDir + _T("\\MLOG\\*.log");

	struct LogFile
	{
		CString m_strName;
		CTime   m_time;
	};
	std::vector<LogFile> arrFiles;

	CFileFind finder;
	BOOL bFound = finder.FindFile(strPattern);
	while (bFound)
	{
		bFound = finder.FindNextFile();
		if (finder.IsDots())
			continue;

		LogFile lf;
		lf.m_strName = finder.GetFileName();
		finder.GetLastWriteTime(lf.m_time);
		arrFiles.push_back(lf);
	}
	finder.Close();

	std::sort(arrFiles.begin(), arrFiles.end(), [](const LogFile& a, const LogFile& b)
	{
		return a.m_time > b.m_time;
	});

	for (const auto& lf : arrFiles)
		m_cboFile.AddString(lf.m_strName);

	if (m_cboFile.GetCount() > 0)
		m_cboFile.SetCurSel(0);   // triggers OnCbnSelchangeFile
}

void CLSControllerDlg::StartTailer(int nServerIndex, const CString& strFileName)
{
	m_tailer.Stop();
	m_edLog.SetWindowText(_T(""));

	CString strPath = m_arrServers[nServerIndex].m_strDir + _T("\\MLOG\\") + strFileName;
	m_tailer.Start(strPath, GetSafeHwnd(), WM_APP_LOGDATA);
}

void CLSControllerDlg::OnCbnSelchangeLog()
{
	const int nSel = m_cboLog.GetCurSel();
	if (nSel < 0)
		return;

	m_tailer.Stop();
	m_edLog.SetWindowText(_T(""));
	PopulateFileCombo(static_cast<int>(m_cboLog.GetItemData(nSel)));
}

void CLSControllerDlg::OnCbnSelchangeFile()
{
	const int nServerSel = m_cboLog.GetCurSel();
	const int nFileSel = m_cboFile.GetCurSel();
	if (nServerSel < 0 || nFileSel < 0)
		return;

	const int nServerIndex = static_cast<int>(m_cboLog.GetItemData(nServerSel));
	if (nServerIndex < 0 || nServerIndex >= static_cast<int>(m_arrServers.size()))
		return;

	CString strFileName;
	m_cboFile.GetLBText(nFileSel, strFileName);
	StartTailer(nServerIndex, strFileName);
}

LRESULT CLSControllerDlg::OnLogData(WPARAM wParam, LPARAM /*lParam*/)
{
	std::unique_ptr<std::string> pStr(reinterpret_cast<std::string*>(wParam));
	if (pStr)
		AppendLogText(*pStr);
	return 0;
}

void CLSControllerDlg::AppendLogText(const std::string& strAnsi)
{
	if (strAnsi.empty())
		return;

	// Log files are ANSI / CP949
	const int nLen = ::MultiByteToWideChar(949, 0, strAnsi.data(), static_cast<int>(strAnsi.size()), nullptr, 0);
	CStringW strWide;
	::MultiByteToWideChar(949, 0, strAnsi.data(), static_cast<int>(strAnsi.size()), strWide.GetBuffer(nLen), nLen);
	strWide.ReleaseBuffer(nLen);

	// Trim the view when it grows beyond the line cap
	const int kMaxLines = 3000;
	const int nLines = m_edLog.GetLineCount();
	if (nLines > kMaxLines)
	{
		const int nChar = m_edLog.LineIndex(nLines - kMaxLines);
		if (nChar > 0)
		{
			m_edLog.SetSel(0, nChar);
			m_edLog.ReplaceSel(_T(""));
		}
	}

	m_edLog.SetSel(-1, -1);
	m_edLog.ReplaceSel(strWide);
	m_edLog.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

int CLSControllerDlg::GetSelectedEntryIndex() const
{
	const int nRow = m_list.GetNextItem(-1, LVNI_SELECTED);
	if (nRow < 0 || nRow >= static_cast<int>(m_arrServers.size()))
		return -1;
	return nRow;
}

bool CLSControllerDlg::AnyRunning() const
{
	for (const auto& e : m_arrServers)
	{
		if (e.m_bRunning)
			return true;
	}
	return false;
}

void CLSControllerDlg::RefreshStatus()
{
	for (size_t i = 0; i < m_arrServers.size(); ++i)
	{
		ServerEntry& e = m_arrServers[i];
		ProcessManager::UpdateStatus(e);

		const int nRow = static_cast<int>(i);
		m_list.SetItemText(nRow, 1, e.m_bRunning ? _T("Running") : _T("Stopped"));
		CString strPid;
		if (e.m_bRunning)
			strPid.Format(_T("%u"), e.m_dwPid);
		else
			strPid = _T("-");
		m_list.SetItemText(nRow, 2, strPid);
	}
}

void CLSControllerDlg::UpdateButtons()
{
	if (m_bStopping)
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_RESTART)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_STARTALL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_STOPALL)->EnableWindow(FALSE);
		return;
	}

	const int nSel = GetSelectedEntryIndex();
	const bool bSel = (nSel >= 0);
	const bool bRunning = bSel && m_arrServers[nSel].m_bRunning;

	GetDlgItem(IDC_BTN_START)->EnableWindow(bSel && !bRunning);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(bSel && bRunning);
	GetDlgItem(IDC_BTN_RESTART)->EnableWindow(bSel && bRunning);

	bool bAnyRunning = false, bAnyStopped = false;
	for (const auto& e : m_arrServers)
	{
		if (e.m_bRunning) bAnyRunning = true; else bAnyStopped = true;
	}
	GetDlgItem(IDC_BTN_STARTALL)->EnableWindow(bAnyStopped);
	GetDlgItem(IDC_BTN_STOPALL)->EnableWindow(bAnyRunning);
}

void CLSControllerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDT_STATUS)
	{
		// Do not touch server entries while the stop thread owns them
		if (!m_bStopping)
		{
			RefreshStatus();
			UpdateButtons();
		}
	}
	else if (nIDEvent == IDT_STARTALL)
	{
		StartAllNext();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CLSControllerDlg::OnBnClickedStart()
{
	const int nIdx = GetSelectedEntryIndex();
	if (nIdx < 0)
		return;

	CString strError;
	if (!ProcessManager::Start(m_arrServers[nIdx], &strError))
		AfxMessageBox(strError, MB_ICONERROR | MB_OK);

	RefreshStatus();
	UpdateButtons();
}

void CLSControllerDlg::OnBnClickedStop()
{
	const int nIdx = GetSelectedEntryIndex();
	if (nIdx < 0)
		return;

	BeginStopThread({ nIdx });
}

void CLSControllerDlg::OnBnClickedRestart()
{
	const int nIdx = GetSelectedEntryIndex();
	if (nIdx < 0)
		return;

	BeginStopThread({ nIdx }, nIdx);
}

void CLSControllerDlg::OnBnClickedStartAll()
{
	StartAllBegin();
}

void CLSControllerDlg::StartAllBegin()
{
	// Indices sorted by Order (stable for equal orders)
	std::vector<int> arrIdx(m_arrServers.size());
	for (int i = 0; i < static_cast<int>(m_arrServers.size()); ++i)
		arrIdx[i] = i;

	std::stable_sort(arrIdx.begin(), arrIdx.end(), [this](int a, int b)
	{
		return m_arrServers[a].m_nOrder < m_arrServers[b].m_nOrder;
	});

	m_arrPending = arrIdx;
	m_nPendingPos = 0;

	// Kick off the staggered sequence (one server per tick)
	SetTimer(IDT_STARTALL, 3000, nullptr);
	StartAllNext();
}

void CLSControllerDlg::StartAllNext()
{
	while (m_nPendingPos < m_arrPending.size())
	{
		const int nIdx = m_arrPending[m_nPendingPos++];

		if (!ProcessManager::UpdateStatus(m_arrServers[nIdx]))
		{
			CString strError;
			if (!ProcessManager::Start(m_arrServers[nIdx], &strError))
				AfxMessageBox(strError, MB_ICONERROR | MB_OK);
			break;   // wait for the next tick before starting the next one
		}
		// already running: skip immediately, continue with the next entry
	}

	if (m_nPendingPos >= m_arrPending.size())
		KillTimer(IDT_STARTALL);

	RefreshStatus();
	UpdateButtons();
}

void CLSControllerDlg::OnBnClickedStopAll()
{
	// Stop in reverse start order
	std::vector<int> arrIdx(m_arrServers.size());
	for (int i = 0; i < static_cast<int>(m_arrServers.size()); ++i)
		arrIdx[i] = i;

	std::stable_sort(arrIdx.begin(), arrIdx.end(), [this](int a, int b)
	{
		return m_arrServers[a].m_nOrder > m_arrServers[b].m_nOrder;
	});

	BeginStopThread(arrIdx);
}

void CLSControllerDlg::StopAll()
{
	for (auto& e : m_arrServers)
		ProcessManager::StopGraceful(e);
}

void CLSControllerDlg::BeginStopThread(std::vector<int> arrIndices, int nRestartIdx /* = -1 */)
{
	if (m_bStopping)
		return;
	if (m_threadStop.joinable())
		m_threadStop.join();

	m_bStopping = true;

	// Mark rows so the user sees what is happening
	for (const int nIdx : arrIndices)
	{
		if (nIdx >= 0 && nIdx < static_cast<int>(m_arrServers.size()))
			m_list.SetItemText(nIdx, 1, _T("Stopping..."));
	}

	UpdateButtons();

	m_threadStop = std::thread([this, arrIndices, nRestartIdx]()
	{
		m_arrStopResults.clear();

		for (const int nIdx : arrIndices)
		{
			if (nIdx < 0 || nIdx >= static_cast<int>(m_arrServers.size()))
				continue;

			ServerEntry& e = m_arrServers[nIdx];
			if (!ProcessManager::StopGraceful(e))
			{
				CString strMsg;
				strMsg.Format(_T("%s - still running after 10 s (not killed)"), e.m_strName.GetString());
				m_arrStopResults.push_back(strMsg);
				continue;
			}

			if (nIdx == nRestartIdx)
			{
				CString strError;
				if (!ProcessManager::Start(e, &strError))
				{
					CString strMsg;
					strMsg.Format(_T("%s - start failed: %s"), e.m_strName.GetString(), strError.GetString());
					m_arrStopResults.push_back(strMsg);
				}
			}
		}

		::PostMessageW(GetSafeHwnd(), WM_APP_STOPDONE, 0, 0);
	});
}

LRESULT CLSControllerDlg::OnStopDone(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (m_threadStop.joinable())
		m_threadStop.join();
	m_bStopping = false;

	RefreshStatus();
	UpdateButtons();

	if (!m_arrStopResults.empty())
	{
		CString strMsg = _T("These servers need attention:\r\n\r\n");
		for (const auto& str : m_arrStopResults)
		{
			strMsg += _T("  - ");
			strMsg += str;
			strMsg += _T("\r\n");
		}
		strMsg += _T("\r\nThey are still running and were NOT force-terminated.");
		AfxMessageBox(strMsg, MB_ICONWARNING | MB_OK);
		m_arrStopResults.clear();
	}
	return 0;
}
void CLSControllerDlg::OnCancel()
{
	m_tailer.Stop();

	// Wait for any in-flight stop sequence before touching the entries
	if (m_threadStop.joinable())
		m_threadStop.join();

	if (AnyRunning())
	{
		int nRunning = 0;
		for (const auto& e : m_arrServers)
		{
			if (e.m_bRunning) ++nRunning;
		}

		CString strMsg;
		strMsg.Format(_T("%d server(s) are still running.\n\nYes - stop them all before exiting\nNo  - leave them running\nCancel - stay"), nRunning);
		const int nRet = AfxMessageBox(strMsg, MB_ICONQUESTION | MB_YESNOCANCEL);
		if (nRet == IDCANCEL)
			return;
		if (nRet == IDYES)
		{
			SetCursor(::LoadCursor(nullptr, IDC_WAIT));
			StopAll();   // graceful; anything still running stays alive
		}
	}

	CDialogEx::OnCancel();
}
