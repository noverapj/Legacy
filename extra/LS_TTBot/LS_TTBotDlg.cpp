// LS_TTBotDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "LS_TTBot.h"
#include "LS_TTBotDlg.h"
//#include "ioTestls_loginsvr.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLS_TTBotDlg* g_Log;

enum TestTimers
{
	TESTTIMER_ID = 1
};

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CLS_TTBotDlg 대화 상자



CLS_TTBotDlg::CLS_TTBotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLS_TTBotDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLS_TTBotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INTERVAL, m_interval);
	DDX_Control(pDX, IDC_EDIT_COUNT, m_count);
	DDX_Control(pDX, IDC_EDIT_IP, m_IP);
	DDX_Control(pDX, IDC_EDIT_PORT, m_port);
	DDX_Control(pDX, IDC_COMBO_TESTCASE, m_testCases);
	DDX_Control(pDX, IDC_LIST_LOG, m_logs);
	DDX_Control(pDX, IDC_EDIT_CONCURRENT, m_concurrent);
}

BEGIN_MESSAGE_MAP(CLS_TTBotDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, OnBnClickedButtonDisconnect)
	ON_MESSAGE(WM_LOG,LOG)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CLS_TTBotDlg 메시지 처리기

BOOL CLS_TTBotDlg::OnInitDialog()
{
	g_Log = this;
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetDefault();
	 
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CLS_TTBotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CLS_TTBotDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

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
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CLS_TTBotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLS_TTBotDlg::SetDefault()
{
	m_sequence = 0;
	m_max = 0;

	m_IP.SetWindowText("172.20.20.115");
	m_port.SetWindowText("30911");
	m_interval.SetWindowText("0");
	m_concurrent.SetWindowText("30");
	m_count.SetWindowText("900000");
	
	m_logs.ResetContent();
	m_testCases.ResetContent();
	m_testCases.AddString("Case Connection");
	m_testCases.AddString("Case Login");
	m_testCases.AddString("Case Echo");
	m_testCases.AddString("case SendTest");
	m_testCases.AddString("case Query (simple)");
	m_testCases.AddString("case Query (guild)");
	m_testCases.SetCurSel(4);
}

void CLS_TTBotDlg::LOG(const TCHAR *format, ...)
{
	m_lock.lock();
	TCHAR buffer[2048]; 
	va_list marker; 

	va_start( marker, format ); 
	_vsnprintf( buffer, sizeof(buffer), format, marker ); 
	va_end( marker );

	m_logs.AddString( buffer );
	m_logs.SetTopIndex(m_logs.GetCount()-1);
	m_lock.unlock();
}
LRESULT CLS_TTBotDlg::LOG(WPARAM wParam, LPARAM lParam)
{
	m_lock.lock();
	if(wParam == NULL)
	{ 
		int nCount = m_logs.GetCount();
		if(nCount > 1000)
		{
			m_logs.ResetContent();
		}
		m_logs.AddString( _T("receive") );
		 
		m_logs.SetTopIndex(m_logs.GetCount()-1);


	}
	else
	{ 
		TCHAR* data = (char*)wParam;
		int nCount = m_logs.GetCount();
		if(nCount > 1000)
		{
			m_logs.ResetContent();
		}
		m_logs.AddString( data);

		m_logs.SetTopIndex(m_logs.GetCount()-1);
		delete data;


	}
	m_lock.unlock();
	return 0;

}

void CLS_TTBotDlg::OnBnClickedButtonPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LOG("Do play");
	TimerBegin();
}

void CLS_TTBotDlg::OnBnClickedButtonStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LOG("Do stop");
	TimerEnd();
}

void CLS_TTBotDlg::OnBnClickedButtonConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CLS_TTBotDlg::OnBnClickedButtonDisconnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CLS_TTBotDlg::TimerBegin()
{
	TimerEnd();

	CString IP, port, interval, max, concurrent;
	m_IP.GetWindowText(IP);
	m_port.GetWindowText(port);
	m_interval.GetWindowText(interval);
	m_concurrent.GetWindowText(concurrent);
	m_count.GetWindowText(max);

	m_max = atoi(max);
	m_sequence = 0;
	int testCount = atoi(concurrent);

	// 테스트 케이스 선택
	int index = m_testCases.GetCurSel();
	switch(index)
	{
	case 0 :
		for(int i = 0 ; i < testCount ; i++)
		{
			ioTestCase* testCase = new ioTestConnection;
			testCase->Startup(IP, atoi(port));
			m_cases.push_back(testCase);
		}
		SetTimer(TESTTIMER_ID, atoi(interval), NULL);
		break;

	case 1 :
		for(int i = 0 ; i < testCount ; i++)
		{
			ioTestCase* testCase = new ioTestLogin;
			testCase->Startup(IP, atoi(port));
			m_cases.push_back(testCase);
		}
		SetTimer(TESTTIMER_ID, atoi(interval), NULL);
		break;

	case 2 :
		for(int i = 0 ; i < testCount ; i++)
		{
			ioTestCase* testCase = new ioTestEcho;
			testCase->Startup(IP, atoi(port));
			m_cases.push_back(testCase);
		}
		SetTimer(TESTTIMER_ID, atoi(interval), NULL);
		break;
	case 3 :
		{
			for(int i = 0 ; i < testCount ; i++)
			{
				ioTestCase* testCase = new ioTestSend;
				testCase->Startup(IP, atoi(port));
				m_cases.push_back(testCase);
			}
			SetTimer(TESTTIMER_ID, atoi(interval), NULL);

		}
		break;
	case 4:
		{
			for(int i = 0 ; i < testCount ; i++)
			{
				ioTestCase* testCase = new ioTestQuerySend1;
				testCase->Startup(IP, atoi(port));
				m_cases.push_back(testCase);
			}
			SetTimer(TESTTIMER_ID, atoi(interval), NULL);

		}
		break;
	case 5:
		{
			for(int i = 0 ; i < testCount ; i++)
			{
				ioTestCase* testCase = new ioTestQuerySend2;
				testCase->Startup(IP, atoi(port));
				m_cases.push_back(testCase);
			}
			SetTimer(TESTTIMER_ID, atoi(interval), NULL);

		}
		break;
	}
}

void CLS_TTBotDlg::TimerEnd()
{
	LOG("End testing");
	KillTimer(TESTTIMER_ID);

	// 초기화
	DestroyTestCases();
}

void CLS_TTBotDlg::DestroyTestCases()
{
	LOG("RUN DestroyTestCases\n");
	for(TESTCASES::iterator it = m_cases.begin() ; it != m_cases.end() ; ++it)
	{
		ioTestCase* testCase = *it;
		if(!testCase) continue;

		testCase->DoDisconnect();
	}
	LOG("END DestroyTestCases\n");
	Sleep(100);
	LOG("WWWW");
	for(TESTCASES::iterator it = m_cases.begin() ; it != m_cases.end() ; ++it)
	{
		ioTestCase* testCase = *it;
		if(!testCase) continue;

		delete testCase;
	}
	LOG("EEEE");
	m_cases.clear();
}

void CLS_TTBotDlg::OnTimer(UINT nIDEvent)
{
	++m_sequence;
	if(m_sequence >= m_max)
	{
		TimerEnd();
		return;
	}

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case TESTTIMER_ID :
		{
			for(TESTCASES::iterator it = m_cases.begin() ; it != m_cases.end() ; ++it)
			{
				ioTestCase* testCase = *it;
				if(!testCase) continue;

				testCase->Run();
			}
		}
		break;
	default :
		TimerEnd();
		break;
	}

	CDialog::OnTimer(nIDEvent);
}
