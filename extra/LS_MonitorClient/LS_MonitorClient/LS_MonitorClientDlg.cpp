
// LS_MonitorClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "LS_MonitorClient.h"
#include "LS_MonitorClientDlg.h"
#include "WinMessage.h"
#include "afxdialogex.h"
#include "Define.h"
#include "ServerTypes.h"
#include "IOSocket.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLS_MonitorClientDlg 대화 상자


CLS_MonitorClientDlg::CLS_MonitorClientDlg(const TCHAR* parameter, CWnd* pParent /*=NULL*/)
	: CDialogEx(CLS_MonitorClientDlg::IDD, pParent)
{
	m_parameter = parameter;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLS_MonitorClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_ZONES, m_zoneTabs);
}

BEGIN_MESSAGE_MAP(CLS_MonitorClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CLS_MonitorClientDlg)
	EASYSIZE(IDC_TAB_ZONES,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,ES_HCENTER)
END_EASYSIZE_MAP

// CLS_MonitorClientDlg 메시지 처리기

BOOL CLS_MonitorClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: Add extra initialization here
	CString filePath = m_parameter;
	if(filePath.IsEmpty())
	{
		filePath = _T("config.ini");
	}

	LoadINI(filePath);
	LoadIP();
	 
	m_zoneTabs.ShowTab(0);  
	GetClientRect(m_clientRect);

	INIT_EASYSIZE;
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLS_MonitorClientDlg::OnPaint()
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
HCURSOR CLS_MonitorClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLS_MonitorClientDlg::LoadBillingFromINI(const TCHAR* file, CString section, BILLINGSERVERTABLE &tempBillingServerTable)
{
	BOOL flag = FALSE;
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);
	CString fullPath;
	CString defaultStr= "default";

	fullPath.Format(_T("%s\\%s"), path, file);
	CString name, billing;
	
	if(section == "")
		return FALSE;

	int i = 1;
	while(TRUE)
	{
		CString strNum;
		strNum.Format(_T("billing%d"), i);
		// name
		DWORD result= GetPrivateProfileString(
			section,		//zone
			strNum,			//billing
			defaultStr, 
			buffer, 
			_countof(buffer), 
			fullPath);

		buffer[result] = 0;
		if(defaultStr != buffer)
		{
			billing = buffer;

			i++;
			int curPos = 0;
			int count = tempBillingServerTable.size();		//count 값이 빌링 구분값

			BILLINGSERVERINFO* billingServer	= new BILLINGSERVERINFO;
			billingServer->serviceIP			= billing.Tokenize(_T(":"), curPos);	
			CString billingPort					= billing.Tokenize(_T(":"), curPos);
			billingServer->servicePort			= atoi(billingPort);		
			billingServer->billingServerState	= SERVERSTATE_OFF;
			billingServer->serverType			= SERVER_TYPE_BILLING;
			billingServer->serverKeyIndex		= count;
			
			tempBillingServerTable[count] = billingServer;
		}
		else
		{
			break;
		}
	}
	if( tempBillingServerTable.size() > 0 )
	{
		flag = TRUE;
	}
	return flag;
}

//. ini 에서 빌링서버옵션 읽을 수 있도록 추가 
void CLS_MonitorClientDlg::LoadINI(const TCHAR* file)
{
	BOOL billingFlag = FALSE;
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, file);

	CString defaultStr= "default", section;
	CString name, main, login, billing;

	int nCode = false;

	int iReult = 0;
	for(int i = 1 ; ; i++)
	{
		section.Format(_T("zone%d"), i);
		billingFlag = FALSE;	//초기화

		// name
		DWORD result= GetPrivateProfileString(
			section,
			_T("name"), 
			defaultStr, 
			buffer, 
			_countof(buffer), 
			fullPath);
		buffer[result] = 0;
		if(defaultStr == buffer)
			break;

		name = buffer;
		
		// main
		result= GetPrivateProfileString(
			section,
			_T("main"), 
			defaultStr, 
			buffer, 
			_countof(buffer), 
			fullPath);
		buffer[result] = 0;

		main = buffer;		

		// login
		result= GetPrivateProfileString(
			section,
			_T("login"), 
			defaultStr, 
			buffer, 
			_countof(buffer), 
			fullPath);
		buffer[result] = 0;

		login = buffer;


		nCode = GetPrivateProfileInt(section, _T("countrycode"), 0, fullPath); // EU 국가별 동접 표시

		//빌링 추가
		BILLINGSERVERTABLE billingServerTable;
		billingFlag = LoadBillingFromINI(file, section, billingServerTable);
		
		if(billingFlag == TRUE)
		{
			int index = m_zoneTabs.GetItemCount();
			//m_zoneTabs.AddTab(name, TRUE, billingServerTable); 
			m_zoneTabs.AddTab(name, main, login, TRUE, billingServerTable, false); 
			m_zoneTabs.ShowTab(index, FALSE);
			
		}

		// 탭추가 
		if(billingFlag == FALSE)
		{
			int index = m_zoneTabs.GetItemCount();
			m_zoneTabs.AddTab(name, main, login, FALSE, billingServerTable, (bool)nCode);  
			m_zoneTabs.ShowTab(index, FALSE);
		}
		
	}
}

void CLS_MonitorClientDlg::LoadIP()
{
	TCHAR hostName[512];
	gethostname(hostName, sizeof(hostName));
	hostent* host = gethostbyname(hostName);

	struct in_addr addr;
	int i = 0;
    while (host->h_addr_list[i] != 0) 
	{
        addr.s_addr = *(u_long *) host->h_addr_list[i++];
		m_IP = inet_ntoa(addr);
		if(!m_IP.IsEmpty())
		{
			CString title;
			title.Format(_T("LostSaga Monitor - %s"), m_IP);
			SetWindowText(title);
			break;
		}
    }
}


void CLS_MonitorClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
}


void CLS_MonitorClientDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int width = m_clientRect.Width()+16;
	pRect->right = pRect->left + width;

	EASYSIZE_MINSIZE(m_clientRect.Width()+16, m_clientRect.Height()+38, fwSide, pRect);
}


void CLS_MonitorClientDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//lpMMI->ptMaxSize.
	int width = m_clientRect.Width()+16;
	int spacing = lpMMI->ptMaxTrackSize.x - lpMMI->ptMaxSize.x;

	lpMMI->ptMaxSize.x = width;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


BOOL CLS_MonitorClientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(RestrictMessage(pMsg))
		return FALSE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CLS_MonitorClientDlg::UpdateTitle(const CString strIP, const int currentUser, const int maxUser)
{
	CString title;
	title.Format(_T("LostSaga Monitor Client - %s(%d/%d)"), strIP, currentUser, maxUser);
	SetWindowText(title);
}

void CLS_MonitorClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	VERIFY(m_zoneTabs.DeleteAllItems());
}
