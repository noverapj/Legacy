// ClientVersionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "LSMonitorDlg.h"
#include "ClientVersionDlg.h"
#include ".\clientversiondlg.h"
#include "TCPSocketTable.h"

#include <strsafe.h>
#include "local\iolocalparent.h"
#include "local\iolocalmanager.h"

// CClientVersionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CClientVersionDlg, CDialog)
CClientVersionDlg::CClientVersionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientVersionDlg::IDD, pParent)
{
}

CClientVersionDlg::~CClientVersionDlg()
{
}

void CClientVersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_USE, m_CheckButtonUse);
	DDX_Control(pDX, IDC_EDIT_VERSION, m_EditVersion);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_EditPassWord);
}


BEGIN_MESSAGE_MAP(CClientVersionDlg, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_SEND, OnBnClickedSend)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedDefaultCancel) // esc 종료 하지 못하게
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CClientVersionDlg 메시지 처리기입니다.

BOOL CClientVersionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_EditVersion.SetLimitText(10);
	m_EditVersion.SetWindowText("");
	m_EditPassWord.SetLimitText(20);
	m_EditPassWord.SetWindowText("");
	m_CheckButtonUse.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CClientVersionDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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

	m_EditVersion.SetWindowText("");
	m_EditPassWord.SetWindowText("");
	m_CheckButtonUse.SetCheck(1);
	m_EditVersion.SetFocus();

	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( !pDlg ) return;
	char szTitle[MAX_PATH]="";
	StringCbPrintf(szTitle, sizeof(szTitle), "[%s] Client Version", (LPCTSTR)pDlg->GetCurZoneName());
	SetWindowText(szTitle);
	pDlg->SetActiveTab( false );
}

void CClientVersionDlg::OnBnClickedSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szVersion;
	CString szPassWord;

	m_EditVersion.GetWindowText(szVersion);
	m_EditPassWord.GetWindowText(szPassWord);

	bool bUse = false;
	if(m_CheckButtonUse.GetCheck() == 1)
	{
		bUse = true;
		if(szVersion.IsEmpty())
		{
			AfxMessageBox("버전이 비어있습니다.");
			return;
		}
	}

	CString szLocalPassword;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		szLocalPassword = pLocal->GetMonitorPassword();

	if( szLocalPassword.IsEmpty() || szPassWord.CompareNoCase( (LPCTSTR) szLocalPassword )  != 0 ) 
	{
		AfxMessageBox("비밀번호가 틀립니다.");
		return;
	}

	SP2Packet kPacket(CTPKM_UPDATE_CLIENT_VERSION);
	kPacket << bUse;
	if( bUse )
	{
		int iClientVersion = atoi((LPCSTR)szVersion);
		kPacket << iClientVersion;
	}

	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( !pDlg )
		return;

	g_TCPSocketTable.Send(WM_TCPSOCKET1+pDlg->GetCurZoneArray(), kPacket);

	MessageBox("Complete Sending.", "Version");
	OnClose();
}


void CClientVersionDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( pDlg ) 
		pDlg->SetActiveTab( true );
	EndDialog(0);
}

void CClientVersionDlg::OnBnClickedDefaultCancel()
{

}