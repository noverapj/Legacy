// PerfMonDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_launcher.h"
#include "PerfMonDlg.h"
#include "ServerDialog.h"
#include "Timer.h"
#include "afxdialogex.h"


// CPerfMonDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPerfMonDlg, CDialog)

CPerfMonDlg::CPerfMonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPerfMonDlg::IDD, pParent), m_init(FALSE), m_serverDlg(NULL)
{

}

CPerfMonDlg::~CPerfMonDlg()
{
}

void CPerfMonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_MONCPU, m_cpuBar);
	DDX_Control(pDX, IDC_PROGRESS_MONRAM, m_MemoryBar);
	DDX_Control(pDX, IDC_PROGRESS_MONNETWORK, m_networkBar);
	DDX_Control(pDX, IDC_STATIC_MONCPU, m_cpuText);
	DDX_Control(pDX, IDC_STATIC_MONMEM, m_memText);
	DDX_Control(pDX, IDC_STATIC_MONNET, m_netText);
}


BEGIN_MESSAGE_MAP(CPerfMonDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CPerfMonDlg 메시지 처리기입니다.

void CPerfMonDlg::SetConfig(CServerDialog* dlg, CString name)
{
	SetWindowText(name);

	m_serverDlg = dlg;
	m_init = TRUE;
	
	RECT rc;
	m_serverDlg->GetWindowRect(&rc);
	SetWindowPos( NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE );
}

void CPerfMonDlg::ShowResult()
{
	ShowWindow( SW_SHOW );
	SetTimer(TIMER_MONITOR, 2000, NULL);
}

BOOL CPerfMonDlg::IsInited()
{
	return m_init;
}

void CPerfMonDlg::SetCPU(UNITS& units)
{
	// def : 첫번째는 평균 CPU사용률, 이후는 CPU순서대로의 사용률
	m_cpuBar.SetRange(1, 100);
	m_CPUs.clear();
	m_CPUs = units;

	if(m_CPUs.size() >= 1)
	{
		CString text;
		text.Format(_T("%d%%"), m_CPUs[0]);
		m_cpuText.SetWindowText(text);

		m_cpuBar.SetBarColor( RGB(255,0,0) );
		m_cpuBar.SetBkColor( RGB(255,0,0) );
		m_cpuBar.SetPos( m_CPUs[0] );

		text = "CPU : ";
		for(uint32 n = 1 ; n < m_CPUs.size() ; n++)
		{
			CString param;
			if(n == 1)
				param.Format( _T("%d%%"), m_CPUs[n] );
			else
				param.Format( _T(", %d%%"), m_CPUs[n] );

			text.AppendFormat( _T("%s"), param);
		}
		m_cpuBar.SetToolTip( text );
		
	}
}

void CPerfMonDlg::SetMemory(UNITS& units)
{
	// def : 메모리사용률, 총메모리, 가능한 메모리, 총 페이징, 가능한 페이징, 총 가상메모리, 가능한 가상메모리
	m_MemoryBar.SetRange(1, 100);

	m_Memorys.clear();
	m_Memorys = units;

	if(m_Memorys.size() >= 1)
	{
		CString text;
		text.Format(_T("%d%%"), m_Memorys[0]);

		m_MemoryBar.SetPos(m_Memorys[0]);
		m_memText.SetWindowText(text);

		text.Format(_T("Memory : %dM / %dM"), units[2]/1024, units[1]/1024);
		m_MemoryBar.SetToolTip( text );
	}
}

void CPerfMonDlg::SetNetwork(UNITS& units)
{
	// def : 현재 Bytes, 받은 Bytes, 보낸 Bytes, Bandwidth
	// def : 받은 Bytes, 보낸 Bytes
	m_networkBar.SetRange(1, 100);
	
	m_Networks.clear();
	m_Networks = units;

	int totalBytes	= units[0] + units[1];

	uint32 value = (totalBytes * 100) / 1024 / 1024;
	m_networkBar.SetPos(value);

	CString text;
	text.Format(_T("%d%%"), value);
	m_netText.SetWindowText(text);
	text.Format(_T("Received(%dKB), Sent(%dKB)"), units[0], units[1]);
	m_networkBar.SetToolTip( text );
}

void CPerfMonDlg::SetDisk(UNITS& units)
{
	m_Disks.clear();
	m_Disks = units;
}

void CPerfMonDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case TIMER_MONITOR :
		if(m_serverDlg)
		{
			m_serverDlg->RequestPeformances();
		}
		break;

	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CPerfMonDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	KillTimer( TIMER_MONITOR );

	CDialog::OnClose();
}


BOOL CPerfMonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_cpuBar.InitToolTip();
	m_MemoryBar.InitToolTip();
	m_networkBar.InitToolTip();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
