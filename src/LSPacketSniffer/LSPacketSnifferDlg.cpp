
#include "pch.h"
#include "framework.h"
#include "LSPacketSniffer.h"
#include "LSPacketSnifferDlg.h"
#include "OpcodeTable.h"
#include "afxdialogex.h"

#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum
{
	COL_SEQ = 0,
	COL_TIME,
	COL_PROTO,
	COL_DIR,
	COL_SRC,
	COL_DST,
	COL_ID,
	COL_NAME,
	COL_SIZE,
	COL_CS,
	COL_COUNT
};

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX, NULL)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CLSPacketSnifferDlg::CLSPacketSnifferDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LSPACKETSNIFFER_DIALOG, pParent)
	, m_bFamily{ true, true, false, false, true }
	, m_engine(&m_store)
	, m_bStarted(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

const wchar_t* CLSPacketSnifferDlg::FAMILY_LABELS[5] =
{
	L"CTPK", L"STPK", L"CUPK", L"SUPK", L"Other"
};

void CLSPacketSnifferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLSPacketSnifferDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BTN_START, &CLSPacketSnifferDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CLSPacketSnifferDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_BTN_EXPORT, &CLSPacketSnifferDlg::OnBnClickedExport)
	ON_BN_CLICKED(IDC_BTN_SELFTEST, &CLSPacketSnifferDlg::OnBnClickedSelftest)
	ON_BN_CLICKED(IDC_BTN_FIND, &CLSPacketSnifferDlg::OnBnClickedFind)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHK_CTPK, IDC_CHK_OTHER, &CLSPacketSnifferDlg::OnBnClickedFamily)
	ON_BN_CLICKED(IDC_CHK_STRICTCS, &CLSPacketSnifferDlg::OnBnClickedStrictCS)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PACKETS, &CLSPacketSnifferDlg::OnLvnGetdispinfoList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PACKETS, &CLSPacketSnifferDlg::OnLvnItemchangedList)
END_MESSAGE_MAP()

BOOL CLSPacketSnifferDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	ModifyStyle(0, WS_THICKFRAME, SWP_FRAMECHANGED);
	ModifyStyle(0, WS_MAXIMIZEBOX);
	{
		CRect rcWork;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
		int w = rcWork.Width() - 60;  if (w > 1280) w = 1280;
		int h = rcWork.Height() - 60; if (h > 820) h = 820;
		if (w < 980) w = 980;
		if (h < 520) h = 520;
		int x = rcWork.left + (rcWork.Width() - w) / 2;
		int y = rcWork.top + (rcWork.Height() - h) / 2;
		MoveWindow(x, y, w, h, FALSE);
	}

	m_fontMono.CreateFont(-13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
	                      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
	                      DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, _T("Consolas"));

	m_comboDevice.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP,
	                     CRect(0, 0, 10, 10), this, IDC_COMBO_DEVICE);
	m_comboDevice.SetFont(GetFont());

	m_stFilter.Create(_T("Filter:"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 10, 10), this, IDC_STATIC);
	m_stFilter.SetFont(GetFont());

	m_editFilter.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
	                    CRect(0, 0, 10, 10), this, IDC_EDIT_FILTER);
	m_editFilter.SetFont(GetFont());
	m_editFilter.SetWindowText(_T(CAP_DEFAULT_BPF));

	m_stPort.Create(_T("Ports:"), WS_CHILD | WS_VISIBLE | SS_RIGHT, CRect(0, 0, 10, 10), this, IDC_STATIC);
	m_stPort.SetFont(GetFont());

	m_editServerPort.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
	                        CRect(0, 0, 10, 10), this, IDC_EDIT_SERVERPORT);
	m_editServerPort.SetFont(GetFont());
	m_editServerPort.SetWindowText(_T("14009-14010"));

	m_btnStart.Create(_T("Start"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	                  CRect(0, 0, 10, 10), this, IDC_BTN_START);
	m_btnStart.SetFont(GetFont());

	m_btnClear.Create(_T("Clear"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	                  CRect(0, 0, 10, 10), this, IDC_BTN_CLEAR);
	m_btnClear.SetFont(GetFont());

	m_btnExport.Create(_T("Export"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	                   CRect(0, 0, 10, 10), this, IDC_BTN_EXPORT);
	m_btnExport.SetFont(GetFont());

	m_btnSelfTest.Create(_T("SelfTest"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	                     CRect(0, 0, 10, 10), this, IDC_BTN_SELFTEST);
	m_btnSelfTest.SetFont(GetFont());

	for (int i = 0; i < 5; i++)
	{
		m_chkFamily[i].Create(FAMILY_LABELS[i], WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
		                      CRect(0, 0, 10, 10), this, IDC_CHK_CTPK + i);
		m_chkFamily[i].SetFont(GetFont());
		m_chkFamily[i].SetCheck(m_bFamily[i] ? BST_CHECKED : BST_UNCHECKED);
	}

	m_chkStrictCS.Create(_T("CS only"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
	                     CRect(0, 0, 10, 10), this, IDC_CHK_STRICTCS);
	m_chkStrictCS.SetFont(GetFont());
	m_chkStrictCS.SetCheck(BST_CHECKED);

	m_stSearch.Create(_T("Search:"), WS_CHILD | WS_VISIBLE | SS_RIGHT, CRect(0, 0, 10, 10), this, IDC_STATIC);
	m_stSearch.SetFont(GetFont());

	m_editSearch.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
	                    CRect(0, 0, 10, 10), this, IDC_EDIT_SEARCH);
	m_editSearch.SetFont(GetFont());

	m_btnFind.Create(_T("Find"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	                 CRect(0, 0, 10, 10), this, IDC_BTN_FIND);
	m_btnFind.SetFont(GetFont());

	m_list.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS
	              | LVS_OWNERDATA | LVS_SINGLESEL | WS_TABSTOP,
	              CRect(0, 0, 10, 10), this, IDC_LIST_PACKETS);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES);

	m_list.InsertColumn(COL_SEQ,   _T("#"),        LVCFMT_RIGHT, 60);
	m_list.InsertColumn(COL_TIME,  _T("Time"),     LVCFMT_LEFT,  80);
	m_list.InsertColumn(COL_PROTO, _T("Proto"),    LVCFMT_CENTER, 46);
	m_list.InsertColumn(COL_DIR,   _T("Dir"),      LVCFMT_CENTER, 52);
	m_list.InsertColumn(COL_SRC,   _T("Source"),   LVCFMT_LEFT,  150);
	m_list.InsertColumn(COL_DST,   _T("Dest"),     LVCFMT_LEFT,  150);
	m_list.InsertColumn(COL_ID,    _T("ID"),       LVCFMT_RIGHT, 60);
	m_list.InsertColumn(COL_NAME,  _T("Opcode"),   LVCFMT_LEFT,  220);
	m_list.InsertColumn(COL_SIZE,  _T("Size"),     LVCFMT_RIGHT, 52);
	m_list.InsertColumn(COL_CS,    _T("CS"),       LVCFMT_CENTER, 46);

	m_editDetail.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL
	                    | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
	                    CRect(0, 0, 10, 10), this, IDC_EDIT_DETAIL);
	m_editDetail.SetFont(&m_fontMono);

	m_status.Create(WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, CRect(0, 0, 10, 10), this, IDC_STATUS_BAR);
	m_status.SetFont(GetFont());

	{
		CRect rc;
		GetClientRect(&rc);
		LayoutControls(rc.Width(), rc.Height());
	}

	RefreshDeviceList();

	SetTimer(LSSNIFF_TIMER_DRAIN, 150, nullptr);

	return TRUE;
}

void CLSPacketSnifferDlg::OnOK()
{
}

void CLSPacketSnifferDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLSPacketSnifferDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CLSPacketSnifferDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLSPacketSnifferDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	if (nType == SIZE_MINIMIZED) return;
	if (!m_list.GetSafeHwnd()) return;
	LayoutControls(cx, cy);
	UpdateStatusBar();
}

void CLSPacketSnifferDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 980;
	lpMMI->ptMinTrackSize.y = 520;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CLSPacketSnifferDlg::LayoutControls(int cx, int cy)
{
	if (cx < 200 || cy < 200) return;

	const int MARGIN = 10;
	const int TB_Y = 10;
	const int TB_H = 24;
	const int TB2_Y = 40;
	const int TB2_H = 22;
	const int GAP = 6;
	const int STATUS_H = 24;

	int rightX = cx - MARGIN;

	m_btnSelfTest.MoveWindow(rightX - 78, TB_Y, 78, TB_H);
	rightX -= 78 + GAP;
	m_btnExport.MoveWindow(rightX - 78, TB_Y, 78, TB_H);
	rightX -= 78 + GAP;
	m_btnClear.MoveWindow(rightX - 72, TB_Y, 72, TB_H);
	rightX -= 72 + GAP;
	m_btnStart.MoveWindow(rightX - 78, TB_Y, 78, TB_H);
	rightX -= 78 + 8;
	m_editServerPort.MoveWindow(rightX - 90, TB_Y + 1, 90, TB_H - 2);
	rightX -= 90 + 2;
	m_stPort.MoveWindow(rightX - 40, TB_Y + 4, 40, 16);
	rightX -= 40 + GAP;

	m_comboDevice.MoveWindow(MARGIN, TB_Y, 250, 300);
	m_stFilter.MoveWindow(MARGIN + 250 + 8, TB_Y + 4, 40, 16);

	int filterX = MARGIN + 250 + 8 + 44;
	int filterW = rightX - filterX - 4;
	if (filterW < 80) filterW = 80;
	m_editFilter.MoveWindow(filterX, TB_Y + 1, filterW, TB_H - 2);

	m_btnFind.MoveWindow(cx - MARGIN - 56, TB2_Y + 1, 56, TB2_H - 2);
	m_stSearch.MoveWindow(cx - MARGIN - 56 - 6 - 48, TB2_Y + 4, 48, 16);

	int searchRight = cx - MARGIN - 56 - 6 - 48 - 4;
	int chkX = MARGIN;
	for (int i = 0; i < 5; i++)
	{
		m_chkFamily[i].MoveWindow(chkX, TB2_Y + 1, 62, TB2_H - 2);
		chkX += 62 + 2;
	}
	m_chkStrictCS.MoveWindow(chkX, TB2_Y + 1, 70, TB2_H - 2);
	chkX += 70 + 4;
	int searchLeft = chkX;
	if (searchLeft > searchRight - 40) searchLeft = searchRight - 40;
	m_editSearch.MoveWindow(searchLeft, TB2_Y + 1, searchRight - searchLeft, TB2_H - 2);

	int listTop = TB2_Y + TB2_H + 8;
	int usableH = cy - listTop - STATUS_H - MARGIN;
	int listH = usableH * 45 / 100;
	if (listH < 80) listH = 80;
	int listBottom = listTop + listH;
	int detailTop = listBottom + 4;

	m_list.MoveWindow(MARGIN, listTop, cx - 2 * MARGIN, listH);
	m_editDetail.MoveWindow(MARGIN, detailTop, cx - 2 * MARGIN, cy - STATUS_H - 4 - detailTop);
	m_status.MoveWindow(0, cy - STATUS_H, cx, STATUS_H);
}

void CLSPacketSnifferDlg::UpdateStatusBar()
{
	if (!m_status.GetSafeHwnd()) return;

	CString s;
	s.Format(_T("  Packets: %u (shown: %u)     |     TCP drop: %u   UDP drop: %u     |     %s"),
	         (unsigned)m_packets.size(),
	         (unsigned)m_visIdx.size(),
	         m_engine.GetDroppedTcpCount(),
	         m_engine.GetDroppedUdpCount(),
	         m_engine.IsRunning() ? _T("CAPTURING") : _T("Stopped"));
	m_status.SetWindowText(s);
}

void CLSPacketSnifferDlg::RefreshDeviceList()
{
	m_devices.clear();
	m_comboDevice.ResetContent();

	if (!CaptureEngine::EnumerateDevices(m_devices))
	{
		m_comboDevice.AddString(_T("[EnumerateDevices gagal ? cek instalasi Npcap]"));
		m_comboDevice.SetCurSel(0);
		return;
	}

	int nDefault = -1;
	for (size_t i = 0; i < m_devices.size(); i++)
	{
		const CaptureDeviceInfo& d = m_devices[i];
		std::wstring label = d.sDesc + L"  (" + d.sName + L")";
		if (label.size() > 90) label = label.substr(0, 90) + L"...";
		int idx = m_comboDevice.AddString(label.c_str());

		if (d.sName.find(L"Loopback") != std::wstring::npos)
			nDefault = idx;
	}

	if (nDefault < 0 && !m_devices.empty())
		nDefault = 0;

	if (nDefault >= 0)
		m_comboDevice.SetCurSel(nDefault);
}

void CLSPacketSnifferDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent != LSSNIFF_TIMER_DRAIN)
	{
		CDialogEx::OnTimer(nIDEvent);
		return;
	}

	size_t nPending = m_store.PendingCount();
	if (nPending > 0)
	{
		int nOld = (int)m_packets.size();
		m_store.Drain(m_packets);
		PacketStore::AssignSeq(m_packets);
		int nNew = (int)m_packets.size();

		if (nNew > nOld)
		{
			for (int i = nOld; i < nNew; i++)
			{
				if (IsFamilyVisible(m_packets[i]))
					m_visIdx.push_back(i);
			}
			m_list.SetItemCount((int)m_visIdx.size());
			m_list.EnsureVisible((int)m_visIdx.size() - 1, FALSE);
		}
	}

	UpdateStatusBar();
}

std::wstring CLSPacketSnifferDlg::FormatTime(const SYSTEMTIME& st)
{
	wchar_t buf[32];
	swprintf_s(buf, L"%02d:%02d:%02d.%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return buf;
}

std::wstring CLSPacketSnifferDlg::FormatAddr(DWORD dwIP, WORD wPort)
{
	wchar_t buf[40];
	const BYTE* b = (const BYTE*)&dwIP;
	swprintf_s(buf, L"%u.%u.%u.%u:%u", b[0], b[1], b[2], b[3], wPort);
	return buf;
}

std::wstring CLSPacketSnifferDlg::FormatDir(const DecodedPacket& dp)
{
	if (!dp.bDirectionKnown) return L"?";
	if (dp.bToServer) return L"C->S";
	return L"S->C";
}

void CLSPacketSnifferDlg::OnLvnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	*pResult = 0;

	if (!(pDispInfo->item.mask & LVIF_TEXT))
		return;

	int iItem = pDispInfo->item.iItem;
	if (iItem < 0 || iItem >= (int)m_visIdx.size())
		return;

	const DecodedPacket& dp = m_packets[m_visIdx[iItem]];
	wchar_t buf[64];

	switch (pDispInfo->item.iSubItem)
	{
	case COL_SEQ:
		swprintf_s(buf, L"%u", dp.dwSeq);
		break;
	case COL_TIME:
		return (void)wcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax,
		                      FormatTime(dp.time).c_str());
	case COL_PROTO:
		return (void)wcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax,
		                      dp.bUDP ? L"UDP" : L"TCP");
	case COL_DIR:
		return (void)wcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax,
		                      FormatDir(dp).c_str());
	case COL_SRC:
		return (void)wcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax,
		                      FormatAddr(dp.dwSrcIP, dp.wSrcPort).c_str());
	case COL_DST:
		return (void)wcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax,
		                      FormatAddr(dp.dwDstIP, dp.wDstPort).c_str());
	case COL_ID:
		swprintf_s(buf, L"0x%04X", dp.dwPacketID);
		break;
	case COL_NAME:
		{
			std::wstring s = OpcodeNameOrHex(dp.dwPacketID);
			if (dp.bUDP && dp.bRelayEnvelope) s += L" [C->C]";
			return (void)wcsncpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax,
			                       s.c_str(), _TRUNCATE);
		}
	case COL_SIZE:
		swprintf_s(buf, L"%u", dp.dwTotalSize);
		break;
	case COL_CS:
		if (dp.bChecksumValid) wcscpy_s(buf, L"OK");
		else                   wcscpy_s(buf, L"-");
		break;
	default:
		return;
	}

	wcsncpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, buf, _TRUNCATE);
}

void CLSPacketSnifferDlg::OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		UpdateDetail(pNMLV->iItem);
	}
}

void CLSPacketSnifferDlg::UpdateDetail(int iItem)
{
	if (iItem < 0 || iItem >= (int)m_visIdx.size())
		return;

	const DecodedPacket& dp = m_packets[m_visIdx[iItem]];
	std::wstring s;
	wchar_t buf[512];

	swprintf_s(buf, L"===== Packet #%u  (%s, %s) =====\r\n",
	           dp.dwSeq, dp.bUDP ? L"UDP" : L"TCP", FormatDir(dp).c_str());
	s += buf;

	swprintf_s(buf, L"Time        : %s\r\n", FormatTime(dp.time).c_str());
	s += buf;
	swprintf_s(buf, L"Endpoint    : %s  ->  %s\r\n",
	           FormatAddr(dp.dwSrcIP, dp.wSrcPort).c_str(),
	           FormatAddr(dp.dwDstIP, dp.wDstPort).c_str());
	s += buf;
	swprintf_s(buf, L"Packet ID   : 0x%04X  (%s)\r\n", dp.dwPacketID, OpcodeNameOrHex(dp.dwPacketID).c_str());
	s += buf;
	swprintf_s(buf, L"TotalSize   : %u\r\n", dp.dwTotalSize);
	s += buf;
	swprintf_s(buf, L"CheckSum    : 0x%08X  [%s%s]\r\n", dp.dwCheckSum,
	           dp.bChecksumValid ? L"VALID" : L"NO MATCH",
	           dp.bChecksumValid ? (dp.bChecksumMD5 ? L" - MD5" : L" - hybrid SHA256-IV") : L"");
	s += buf;
	swprintf_s(buf, L"FSM State   : 0x%08X%s\r\n", dp.dwFSMState,
	           dp.dwFSMState == 0xFFFFFFFF ? L"  (MAGIC_TOKEN / TCP)" : L"");
	s += buf;

	if (dp.bRelayEnvelope)
	{
		swprintf_s(buf, L"Relay Dest  : %u.%u.%u.%u : %u   (C->C envelope)\r\n",
		           (dp.dwRelayDestIP) & 0xff, (dp.dwRelayDestIP >> 8) & 0xff,
		           (dp.dwRelayDestIP >> 16) & 0xff, (dp.dwRelayDestIP >> 24) & 0xff,
		           dp.dwRelayDestPort);
		s += buf;
	}

	swprintf_s(buf, L"Body        : %u bytes\r\n", (unsigned)dp.body.size());
	s += buf;

	s += L"\r\n--- Hex Dump (decrypted) ---\r\n";
	s += PacketDecoder::BodyToHexDump(dp.body);

	std::wstring strs = PacketDecoder::ExtractStrings(dp.body);
	if (!strs.empty())
	{
		s += L"\r\n--- Strings ---\r\n";
		s += strs;
	}

	m_editDetail.SetWindowText(s.c_str());

	m_editDetail.SetSel(0, 0);
	m_editDetail.SendMessage(EM_SCROLLCARET, 0, 0);
}

void CLSPacketSnifferDlg::AppendLogLine(const std::wstring& s)
{
	CString csOld;
	m_editDetail.GetWindowText(csOld);
	m_editDetail.SetWindowText(csOld + s.c_str());
}

void CLSPacketSnifferDlg::OnBnClickedStart()
{
	if (m_bStarted)
	{
		m_engine.Stop();
		m_bStarted = false;
		m_btnStart.SetWindowText(_T("Start"));
		UpdateStatusBar();
		return;
	}

	int nSel = m_comboDevice.GetCurSel();
	if (nSel < 0 || (size_t)nSel >= m_devices.size())
	{
		AfxMessageBox(_T("Pilih device capture dulu."));
		return;
	}

	CString csFilter;
	m_editFilter.GetWindowText(csFilter);
	if (csFilter.IsEmpty()) csFilter = CAP_DEFAULT_BPF;

	CString csPort;
	m_editServerPort.GetWindowText(csPort);
	std::vector<WORD> vPorts = ParsePortList(csPort);
	m_engine.SetServerPorts(vPorts);
	m_engine.SetStrictChecksum(m_chkStrictCS.GetCheck() == BST_CHECKED);

	std::wstring err;
	if (!m_engine.Start(m_devices[nSel].sName.c_str(), csFilter, err))
	{
		AfxMessageBox((LPCTSTR)err.c_str());
		return;
	}

	m_bStarted = true;
	m_btnStart.SetWindowText(_T("Stop"));
	UpdateStatusBar();
}

void CLSPacketSnifferDlg::OnBnClickedClear()
{
	m_packets.clear();
	m_visIdx.clear();
	m_list.SetItemCount(0);
	m_list.Invalidate();
	m_editDetail.SetWindowText(_T(""));
	UpdateStatusBar();
}

void CLSPacketSnifferDlg::OnBnClickedExport()
{
	if (m_visIdx.empty())
	{
		AfxMessageBox(_T("Tidak ada packet yang tampil (periksa filter family)."));
		return;
	}

	CFileDialog dlg(FALSE, _T("txt"), _T("LSSniffSession"),
	                OFN_OVERWRITEPROMPT, _T("Text Files (*.txt)|*.txt||"), this);
	if (dlg.DoModal() != IDOK)
		return;

	std::ofstream f(dlg.GetPathName().GetString(), std::ios::out | std::ios::binary);
	if (!f)
	{
		AfxMessageBox(_T("Gagal buka file."));
		return;
	}

	f.write("\xEF\xBB\xBF", 3);

	auto ToNarrow = [](const wchar_t* w) -> std::string
	{
		std::string s;
		while (*w) s.push_back((char)(*w++ & 0xFF));
		return s;
	};

	size_t nExport = 0;
	for (size_t i = 0; i < m_visIdx.size(); i++)
	{
		const DecodedPacket& dp = m_packets[m_visIdx[i]];
		wchar_t buf[512];

		swprintf_s(buf, L"[%06u] %s %s %s %s -> %s  ID=0x%04X (%s) size=%u cs=%s\r\n",
		           dp.dwSeq, FormatTime(dp.time).c_str(), dp.bUDP ? L"UDP" : L"TCP",
		           FormatDir(dp).c_str(),
		           FormatAddr(dp.dwSrcIP, dp.wSrcPort).c_str(),
		           FormatAddr(dp.dwDstIP, dp.wDstPort).c_str(),
		           dp.dwPacketID, OpcodeNameOrHex(dp.dwPacketID).c_str(),
		           dp.dwTotalSize, dp.bChecksumValid ? L"OK" : L"-");
		f << ToNarrow(buf);

		std::wstring strs = PacketDecoder::ExtractStrings(dp.body);
		if (!strs.empty())
			f << ToNarrow(strs.c_str());

		f << ToNarrow(PacketDecoder::BodyToHexDump(dp.body).c_str());
		f << "\r\n";
		nExport++;
	}

	f.close();

	CString msg;
	msg.Format(_T("Export %u packet selesai."), (unsigned)nExport);
	AfxMessageBox(msg);
}

void CLSPacketSnifferDlg::OnBnClickedSelftest()
{
	std::wstring report;
	bool bOK = PacketDecoder::SelfTest(report);
	AppendLogLine(report.c_str());
	AfxMessageBox(bOK ? _T("Self-Test PASS ? lihat detail pane.") : _T("Self-Test FAIL ? lihat detail pane."));
}

void CLSPacketSnifferDlg::OnBnClickedStrictCS()
{
	m_engine.SetStrictChecksum(m_chkStrictCS.GetCheck() == BST_CHECKED);
}

int CLSPacketSnifferDlg::FamilyIndexOf(const DecodedPacket& dp)
{
	const wchar_t* p = LookupOpcodeName(dp.dwPacketID);
	if (!p) return 4;
	if (wcsncmp(p, L"CTPK_", 5) == 0) return 0;
	if (wcsncmp(p, L"STPK_", 5) == 0) return 1;
	if (wcsncmp(p, L"CUPK_", 5) == 0) return 2;
	if (wcsncmp(p, L"SUPK_", 5) == 0) return 3;
	return 4;
}

bool CLSPacketSnifferDlg::IsFamilyVisible(const DecodedPacket& dp) const
{
	return m_bFamily[FamilyIndexOf(dp)];
}

void CLSPacketSnifferDlg::RebuildVisibleIndex()
{
	m_visIdx.clear();
	for (size_t i = 0; i < m_packets.size(); i++)
	{
		if (IsFamilyVisible(m_packets[i]))
			m_visIdx.push_back((int)i);
	}
	m_list.SetItemCount((int)m_visIdx.size());
	m_list.Invalidate();
}

void CLSPacketSnifferDlg::OnBnClickedFamily(UINT nID)
{
	int idx = (int)(nID - IDC_CHK_CTPK);
	if (idx < 0 || idx >= 5) return;
	m_bFamily[idx] = (m_chkFamily[idx].GetCheck() == BST_CHECKED);
	RebuildVisibleIndex();
	UpdateStatusBar();
}

bool CLSPacketSnifferDlg::PacketMatchesSearch(const DecodedPacket& dp, const CString& csNeedle) const
{
	CString sName(OpcodeNameOrHex(dp.dwPacketID).c_str());
	sName.MakeLower();
	if (sName.Find(csNeedle) >= 0)
		return true;

	wchar_t buf[16];
	swprintf_s(buf, L"0x%04x", dp.dwPacketID);
	CString sId(buf);
	if (sId.Find(csNeedle) >= 0)
		return true;

	std::wstring strs = PacketDecoder::ExtractStrings(dp.body);
	if (!strs.empty())
	{
		CString sBody(strs.c_str());
		sBody.MakeLower();
		if (sBody.Find(csNeedle) >= 0)
			return true;
	}
	return false;
}

void CLSPacketSnifferDlg::OnBnClickedFind()
{
	CString cs;
	m_editSearch.GetWindowText(cs);
	cs.MakeLower();
	cs.TrimLeft(_T(' '));
	cs.TrimRight(_T(' '));
	if (cs.IsEmpty()) return;

	int nCount = (int)m_visIdx.size();
	if (nCount == 0) return;

	int nStart = m_list.GetNextItem(-1, LVNI_SELECTED) + 1;
	if (nStart < 0 || nStart >= nCount) nStart = 0;

	for (int k = 0; k < nCount; k++)
	{
		int i = (nStart + k) % nCount;
		if (PacketMatchesSearch(m_packets[m_visIdx[i]], cs))
		{
			m_list.SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
			m_list.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			m_list.SetSelectionMark(i);
			m_list.EnsureVisible(i, FALSE);
			UpdateDetail(i);
			return;
		}
	}

	AfxMessageBox(_T("Tidak ketemu: ") + cs);
}

std::vector<WORD> CLSPacketSnifferDlg::ParsePortList(LPCTSTR sz)
{
	std::vector<WORD> v;
	CString s(sz);
	s.Remove(_T(' '));
	if (s.IsEmpty()) return v;

	int nStart = 0;
	while (nStart < s.GetLength())
	{
		int nEnd = s.Find(_T(','), nStart);
		CString tok = (nEnd < 0) ? s.Mid(nStart) : s.Mid(nStart, nEnd - nStart);

		int nDash = tok.Find(_T('-'));
		if (nDash > 0)
		{
			WORD wA = (WORD)_wtoi(tok.Left(nDash));
			WORD wB = (WORD)_wtoi(tok.Mid(nDash + 1));
			if (wA > 0 && wB >= wA && (wB - wA) <= 1000)
			{
				for (WORD p = wA; p <= wB; p++)
					v.push_back(p);
			}
		}
		else
		{
			WORD wP = (WORD)_wtoi(tok);
			if (wP > 0)
				v.push_back(wP);
		}

		if (nEnd < 0) break;
		nStart = nEnd + 1;
	}
	return v;
}

BOOL CLSPacketSnifferDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == m_editSearch.GetSafeHwnd())
		{
			OnBnClickedFind();
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
