// AnnounceDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "LSMonitorDlg.h"
#include "NetWork\SystemInfoUDPMsg.h"
#include ".\announcedlg.h"
#include "TCPSocketTable.h"

#include <strsafe.h>
#include "local\iolocalparent.h"
#include "local\iolocalmanager.h"


// CAnnounceDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAnnounceDlg, CDialog)
CAnnounceDlg::CAnnounceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnnounceDlg::IDD, pParent)
{
}

CAnnounceDlg::~CAnnounceDlg()
{
}

void CAnnounceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ANNOUNCE, m_EditAnnouce);
	DDX_Control(pDX, IDC_EDIT_USER_ID, m_EditUserID);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_EditPassWord);
	DDX_Control(pDX, IDC_CHECK_DIS, m_ButtonDisconnect);
}


BEGIN_MESSAGE_MAP(CAnnounceDlg, CDialog)
	ON_BN_CLICKED(IDSend, OnBnClickedSend)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedDefaultCancel) // esc 종료 하지 못하게
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAnnounceDlg 메시지 처리기입니다.

void CAnnounceDlg::OnBnClickedSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szAnnounce;
	CString szPassWord;
	CString szUserID;

	m_EditAnnouce.GetWindowText(szAnnounce);
	m_EditPassWord.GetWindowText(szPassWord);
	m_EditUserID.GetWindowText(szUserID);

	int iMsgType = ANNOUNCE_TYPE_ALL;

	if(m_ButtonDisconnect.GetCheck() == 0)
	{
		if(szAnnounce.IsEmpty())
		{
			AfxMessageBox("공지 내용이 비어있습니다.");
			return;
		}

		if(!szUserID.IsEmpty())
			iMsgType = ANNOUNCE_TYPE_ONE;
	}
	else // disconnect
	{
		if(szUserID.IsEmpty())
		{
			AfxMessageBox("유저아이디가 비어있습니다.");
			return;
		}
		else
			iMsgType = ANNOUNCE_TYPE_DISCONNECT;
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

	szAnnounce.Remove('\r');
	szAnnounce.Replace('\n', '#');

	ioHashString szSendAnnounce = szAnnounce.GetBuffer(0);
	ioHashString szSendUserId = szUserID.GetBuffer(0);
	SYSTEMTIME StartTime;
	DWORD dwEndTime = 0;

    StartTime.wYear   =  0; 
	StartTime.wMonth  =  0; 
	StartTime.wDay    =  0; 
	StartTime.wHour   =  0; 
	StartTime.wMinute =  0; 
	dwEndTime = 300000; // 5분 
	
	SP2Packet kPacket(CTPKM_ANNOUNCE);
	kPacket << szSendAnnounce;
	kPacket << iMsgType;
	if(iMsgType != ANNOUNCE_TYPE_ALL)
		kPacket << szSendUserId;
	kPacket << StartTime.wYear;
	kPacket << StartTime.wMonth;
	kPacket << StartTime.wDay;
	kPacket << StartTime.wHour;
	kPacket << StartTime.wMinute;
	kPacket << dwEndTime;

	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( !pDlg )
		return;

	g_TCPSocketTable.Send(WM_TCPSOCKET1+pDlg->GetCurZoneArray(), kPacket);

	MessageBox("Complete Sending.", "Announce");
	OnClose();
}

BOOL CAnnounceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_EditAnnouce.SetLimitText(200);
	m_EditAnnouce.SetWindowText("");
	m_EditPassWord.SetLimitText(20);
	m_EditPassWord.SetWindowText("");
	m_EditUserID.SetWindowText("");
	m_EditUserID.SetLimitText(ID_NUMBER);
	m_ButtonDisconnect.SetCheck(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAnnounceDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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

	m_EditAnnouce.SetWindowText("");
	m_EditPassWord.SetWindowText("");
	m_EditUserID.SetWindowText("");
	m_ButtonDisconnect.SetCheck(0);
	m_EditAnnouce.SetFocus();

	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( !pDlg ) return;
	char szTitle[MAX_PATH]="";
	StringCbPrintf(szTitle, sizeof(szTitle), "[%s] Announce", (LPCTSTR)pDlg->GetCurZoneName());
	SetWindowText(szTitle);
	pDlg->SetActiveTab( false );
}

void CAnnounceDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( pDlg ) 
		pDlg->SetActiveTab( true );
	EndDialog(0); // CDialog::OnClose()는 내부에서 OnCancel()를 호출함. 현재 OnCancel는 비어 있으므로 직접 EndDialog 호출
}

void CAnnounceDlg::OnBnClickedDefaultCancel() // esc로 종료하지 못하게 
{

}