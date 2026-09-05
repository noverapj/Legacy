// ControlDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "LSMonitorDlg.h"
#include "ControlDlg.h"
#include ".\controldlg.h"
#include "NetWork\SystemInfoUDPMsg.h"
#include "TCPSocketTable.h"

#include <strsafe.h>


// CControlDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CControlDlg, CDialog)
CControlDlg::CControlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CControlDlg::IDD, pParent)
{
	m_vCheckInfo.reserve(10);
	m_dwSendTime = 0;
	m_eActionType = AT_NONE;
}

CControlDlg::~CControlDlg()
{
	m_vCheckInfo.clear();
}

void CControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_VERSION, m_EditVersion);
	DDX_Control(pDX, IDC_BUTTON_SEND_COPY, m_ButtonSendCopy);
	DDX_Control(pDX, IDC_BUTTON_SEND_LOAD, m_ButtonSendLoad);
	DDX_Control(pDX, IDC_EDIT_IP, m_EditIP );
	DDX_Control(pDX, IDC_LIST_INFO, m_ListBoxInfo);
	DDX_Control(pDX, IDC_BUTTON_SEND_INFO, m_ButtonSendInfo);
	DDX_Control(pDX, IDC_EDIT_CHANGE, m_EditChange);
	DDX_Control(pDX, IDC_CHECK_SERVERIP, m_CheckBtnServerIP);
	DDX_Control(pDX, IDC_CHECK_CHANGE, m_CheckBtnChange);
}


BEGIN_MESSAGE_MAP(CControlDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SEND_COPY, OnBnClickedButtonSendCopy)
	ON_BN_CLICKED(IDC_BUTTON_SEND_LOAD, OnBnClickedButtonSendLoad)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedDefaultCancel) // esc 종료 하지 못하게
	ON_BN_CLICKED(IDOK, OnBnClickedDefaultOK ) // enter 종료 하지 못하게
	ON_BN_CLICKED(IDC_BUTTON_SEND_INFO, OnBnClickedButtonSendInfo)
	ON_BN_CLICKED(IDC_CHECK_SERVERIP, OnBnClickedCheckServerip)
	ON_BN_CLICKED(IDC_CHECK_CHANGE, OnBnClickedCheckChange)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CControlDlg 메시지 처리기입니다.

BOOL CControlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_vCheckInfo.clear();
	m_EditVersion.SetLimitText(10);
	m_EditVersion.SetWindowText("");
	m_EditIP.SetLimitText( 20 );
	m_EditIP.SetWindowText("");
	m_EditIP.EnableWindow( FALSE );
	m_EditChange.SetLimitText( 10 );
	m_EditChange.SetWindowText("");
	m_EditChange.EnableWindow( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CControlDlg::OnBnClickedButtonSendCopy()
{
	m_ListBoxInfo.InsertString(0, "" ); // 가독성
	LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendCopy - Start Copy ");
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_vCheckInfo.empty() )
	{
		MessageBox("Now Sending. Please Wait.", "Control");
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendCopy - m_vCheckInfo.empty == false ");
		return;
	}

	if( !AfxGetMainWnd() )
	{
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendCopy - AfxGetMainWnd == NULL ");
		return;
	}

	CLSMonitorDlg *pDlg = static_cast< CLSMonitorDlg* > ( AfxGetMainWnd() );
	if( !pDlg )
	{
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendCopy - pDlg == NULL ");
		return;
	}

	CString szIP;
	CString szVersion;
	m_EditVersion.GetWindowText(szVersion);
	m_EditIP.GetWindowText( szIP );

	if( szVersion.IsEmpty() )
	{
		AfxMessageBox("버전이 비어있습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::OnBnClickedButtonSendCopy Version is Empty" );
		return;
	}

	int iVersion = atoi((LPCSTR)szVersion);
	
	if( iVersion <= 0 )
	{
		AfxMessageBox("버전이 잘못되었습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::OnBnClickedButtonSendCopy Version is Wrong: %d", iVersion );
		return;
	}

	pDlg->SendCopyUDPMsg( m_vCheckInfo, szIP, iVersion );

	if( m_vCheckInfo.empty() )
	{
		AfxMessageBox("Send Copy를 모두 실패하였습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::OnBnClickedButtonSendCopy All Sending Fail." );
		return;
	}

	for(CControlDlg::vCheckInfo::iterator iter = m_vCheckInfo.begin(); iter != m_vCheckInfo.end(); ++iter)
	{
		CheckInfo &rCheckInfo = (*iter);
		CString szIpPort;
		szIpPort.Format( "Send Copy : %s:%d [%d]", (LPCTSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort, iVersion );

		m_ListBoxInfo.InsertString(0, (LPCTSTR) szIpPort );
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendCopy : Send :%s:%d [%d]", (LPCSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort, iVersion );
	}

	m_dwSendTime = timeGetTime();
	InActiveAllBtn();
	m_eActionType = AT_SEND_COPY;
	LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendCopy - End Copy ");
}

void CControlDlg::OnBnClickedButtonSendLoad()
{
	m_ListBoxInfo.InsertString(0, "" ); // 가독성
	LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendLoad - Start.");
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( !m_vCheckInfo.empty() )
	{
		MessageBox("Now Sending. Please Wait.", "Control");
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendLoad - m_vCheckInfo.empty == false ");
		return;
	}

	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) ( AfxGetMainWnd() );
	if( !pDlg )
	{
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendLoad - pDlg is NULL. ");
		return;
	}

	CString szIP;
	CString szVersion;
	CString szChange;
	m_EditVersion.GetWindowText(szVersion);
	m_EditIP.GetWindowText( szIP );
	m_EditChange.GetWindowText( szChange );

	if( szVersion.IsEmpty() )
	{
		AfxMessageBox("버전이 비어있습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::OnBnClickedButtonSendLoad Version is Empty" );
		return;
	}

	int iVersion = atoi((LPCSTR)szVersion);

	if( iVersion <= 0 )
	{
		AfxMessageBox("버전이 잘못되었습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::OnBnClickedButtonSendLoad Version is Wrong: %d", iVersion );
		return;
	}

	int iChange = 0;
	if( !szChange.IsEmpty() )
	{
		iChange = atoi((LPCSTR)szChange);
	}

	pDlg->GetLoadCheckFileInfo( m_vCheckInfo, szIP );
	if( m_vCheckInfo.empty() )
	{
		AfxMessageBox("Send Load를 모두 실패하였습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::OnBnClickedButtonSendLoad All Sending Fail." );
		return;
	}

	ioHashString szSendIP = (LPCTSTR)szIP;
	SP2Packet kPacket(CTPKM_LOAD_CS3_FILE);
	kPacket << szSendIP;
	kPacket << iVersion;
	kPacket << iChange;
	if( !g_TCPSocketTable.Send(WM_TCPSOCKET1+pDlg->GetCurZoneArray(), kPacket) )
	{
		AfxMessageBox("Load 요청이 실패하였습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::OnBnClickedButtonSendLoad Sending Fail." );
		return;
	}

	for(CControlDlg::vCheckInfo::iterator iter = m_vCheckInfo.begin(); iter != m_vCheckInfo.end(); ++iter)
	{
		CheckInfo &rCheckInfo = (*iter);
		CString szIpPort;
		szIpPort.Format( "Send Load : %s:%d [%d]", (LPCTSTR) rCheckInfo.m_szServerIP, rCheckInfo.m_iPort, iVersion );

		m_ListBoxInfo.InsertString(0, (LPCTSTR) szIpPort );
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendLoad : Send :%s:%d [%d:%d]", (LPCSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort, iVersion, iChange );
	}
	m_dwSendTime = timeGetTime();
	InActiveAllBtn();
	m_eActionType = AT_SEND_LOAD;
	LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendLoad - End.");
}

void CControlDlg::OnBnClickedButtonSendInfo()
{
	m_ListBoxInfo.InsertString(0, "" ); // 가독성
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendInfo - Start.");
	if( !m_vCheckInfo.empty() )
	{
		MessageBox("Now Sending. Please Wait.", "Control");
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendInfo - m_vCheckInfo.empty == false ");
		return;
	}

	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) ( AfxGetMainWnd() );
	if( !pDlg )
	{
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendInfo - pDlg is NULL. ");
		return;
	}

	CString szIP;
	m_EditIP.GetWindowText( szIP );

	pDlg->GetLoadCheckFileInfo( m_vCheckInfo, szIP );
	if( m_vCheckInfo.empty() )
	{
		AfxMessageBox("Send Info를 모두 실패하였습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::OnBnClickedButtonSendInfo All Sending Fail." );
		return;
	}

	ioHashString szSendIP = (LPCTSTR)szIP;
	SP2Packet kPacket(CTPKM_CS3_FILE_VERSION);
	kPacket << szSendIP;
	if( !g_TCPSocketTable.Send(WM_TCPSOCKET1+pDlg->GetCurZoneArray(), kPacket) )
	{
		AfxMessageBox("Info 요청이 실패하였습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::OnBnClickedButtonSendInfo Sending Fail." );
		return;
	}

	for(CControlDlg::vCheckInfo::iterator iter = m_vCheckInfo.begin(); iter != m_vCheckInfo.end(); ++iter)
	{
		CheckInfo &rCheckInfo = (*iter);
		CString szIpPort;
		szIpPort.Format( "Send Info : %s:%d", (LPCTSTR) rCheckInfo.m_szServerIP, rCheckInfo.m_iPort);

		m_ListBoxInfo.InsertString(0, (LPCTSTR) szIpPort );
		LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendInfo : Send :%s:%d", (LPCSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort );
	}
	m_dwSendTime = timeGetTime();
	InActiveAllBtn();
	m_eActionType = AT_SEND_INFO;
	LOG.PrintTimeAndLog(0, "CControlDlg::OnBnClickedButtonSendInfo - End.");
}


void CControlDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( pDlg ) 
		pDlg->SetActiveTab( true );
	KillTimer(CHECK_TIMER);
	m_vCheckInfo.clear();
	EndDialog(0);
}

void CControlDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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
	ActiveAllBtn();
	m_EditVersion.SetWindowText( "" );
	m_EditVersion.SetFocus();
	m_EditIP.SetWindowText( "" );
	m_EditChange.SetWindowText( "" );
	SetTimer(CHECK_TIMER, 10000, NULL); 
	m_eActionType = AT_NONE;

	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( !pDlg ) return;
	char szTitle[MAX_PATH]="";
	StringCbPrintf(szTitle, sizeof(szTitle), "[%s] Control", (LPCTSTR)pDlg->GetCurZoneName());
	SetWindowText(szTitle);
	pDlg->SetActiveTab( false );
}

void CControlDlg::OnBnClickedDefaultCancel()
{
	// 비어 있음.
}

void CControlDlg::OnBnClickedDefaultOK()
{
	// 비어 있음.
}
void CControlDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if( m_dwSendTime == 0 )
		return;
	enum { CHECK_TIME = 120000, };
	if( timeGetTime() - m_dwSendTime > CHECK_TIME )
	{
		m_dwSendTime = 0;

		for(CControlDlg::vCheckInfo::iterator iter = m_vCheckInfo.begin(); iter != m_vCheckInfo.end(); ++iter)
		{
			CheckInfo &rCheckInfo = (*iter);
			CString szIpPort;
			if( m_eActionType == AT_SEND_COPY )
			{
				szIpPort.Format( "Fail Send Copy %s:%d....................<Fail>", (LPCTSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort );
				LOG.PrintTimeAndLog(0, "CControlDlg::OnTimer : Copy Fail :%s:%d", (LPCSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort );
			}
			else if( m_eActionType == AT_SEND_DELETE )
			{
				szIpPort.Format( "Fail Delete Load %s:%d....................<Fail>", (LPCTSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort );
				LOG.PrintTimeAndLog(0, "CControlDlg::OnTimer : Delete Fail :%s:%d", (LPCSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort );
			}
			else if( m_eActionType == AT_SEND_LOAD )
			{
				szIpPort.Format( "Fail Send Load %s:%d....................<Fail>", (LPCTSTR) rCheckInfo.m_szServerIP, rCheckInfo.m_iPort );
				LOG.PrintTimeAndLog(0, "CControlDlg::OnTimer : Load Fail :%s:%d", (LPCSTR) rCheckInfo.m_szServerIP, rCheckInfo.m_iPort );
			}
			else if( m_eActionType == AT_SEND_INFO )
			{
				szIpPort.Format( "Fail Send Info %s:%d....................<Fail>", (LPCTSTR) rCheckInfo.m_szServerIP, rCheckInfo.m_iPort );
				LOG.PrintTimeAndLog(0, "CControlDlg::OnTimer : Info Fail :%s:%d", (LPCSTR) rCheckInfo.m_szServerIP, rCheckInfo.m_iPort );
			}
			m_ListBoxInfo.InsertString( 0, (LPCTSTR)szIpPort );
		}
		m_vCheckInfo.clear();
		ActiveAllBtn();
		m_eActionType = AT_NONE;
	}
	else
	{
		int iRemainSec = ( CHECK_TIME - ( timeGetTime() - m_dwSendTime ) ) / 1000 ;
		if( iRemainSec < 0 )
			iRemainSec = 0;
		CString szTemp;
		szTemp.Format( "Please. Wait %d Sec", iRemainSec );
		m_ListBoxInfo.InsertString(0, szTemp);
	}
	CDialog::OnTimer(nIDEvent);
}

void CControlDlg::OnCopyResult( int iResultType, int iRequestType, int iVersion, const CheckInfo &rReceiveInfo )
{
	// 메세지 박스 두지 말것
	for(CControlDlg::vCheckInfo::iterator iter = m_vCheckInfo.begin(); iter != m_vCheckInfo.end(); ++iter)
	{
		CheckInfo &rCheckInfo = (*iter);
		if( rCheckInfo.m_szSystemIP == rReceiveInfo.m_szSystemIP && rCheckInfo.m_iPort == rReceiveInfo.m_iPort )
		{
			m_vCheckInfo.erase( iter );
			break;
		}
	}

	if( iResultType == COPY_ANSWER_SUCCESS )
	{
		CString szText;
		szText.Format( "Copy Success : %s:%d[%d]....................<Success>", (LPCSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort, iVersion );
		m_ListBoxInfo.InsertString( 0, (LPCTSTR) szText );	
		LOG.PrintTimeAndLog(0, "CControlDlg::OnCopyResult: Success :%s:%d:%d:%d[%d]", (LPCSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort, iResultType, iRequestType, iVersion );
	}
	else 
	{
		CString szText;
		if( iResultType == COPY_ANSWER_FAIL )
			szText.Format( "파일복사에 실패하였습니다. :%s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort );
		else if( iResultType == COPY_ANSWER_FAIL_WRONG_VERSION )
		{
			szText.Format( "복사할 버전이 잘못되었습니다. :%s:%d [Ver:%d]....................<Fail>", (LPCTSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort, iVersion );
		}
		else if( iResultType == COPY_ANSWER_NOT_CS3 )
			szText.Format( "복사할 cs3 파일이 없습니다. :%s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort );
		else
			szText.Format( "알 수 없는 복사 에러 입니다. :%s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort );

		m_ListBoxInfo.InsertString( 0, (LPCTSTR) szText );
		LOG.PrintTimeAndLog(0, "CControlDlg::OnCopyResult: Fail :%s:%d:%d:%d[%d]", (LPCSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort, iResultType, iRequestType, iVersion );
	}

	if( m_vCheckInfo.empty() )
	{
		m_vCheckInfo.clear();
		m_dwSendTime = 0; // 시간 체크 정지
		m_eActionType = AT_NONE;
		LOG.PrintTimeAndLog(0, "CControlDlg::OnCopyResult : m_vCheckInfo.empty()" );
	}

	if( !m_vCheckInfo.empty() )
		return;

	// delete
	if( iRequestType != COPY_REQUEST_CS3 )
	{
		ActiveAllBtn();
		return;
	}

	SendDeleteMsg();
}


void CControlDlg::OnDeleteResult( int iResultType, int iRequestType, int iVersion, const CheckInfo &rReceiveInfo )
{
	// 메세지 박스 두지 말것
	for(CControlDlg::vCheckInfo::iterator iter = m_vCheckInfo.begin(); iter != m_vCheckInfo.end(); ++iter)
	{
		CheckInfo &rCheckInfo = (*iter);
		if( rCheckInfo.m_szSystemIP == rReceiveInfo.m_szSystemIP && rCheckInfo.m_iPort == rReceiveInfo.m_iPort )
		{
			m_vCheckInfo.erase( iter );
			break;
		}
	}

	if( iResultType == DELETE_ANSWER_SUCCESS )
	{
		CString szText;
		szText.Format( "Delete Success : %s:%d[%d]....................<Success>", (LPCSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort, iVersion );
		m_ListBoxInfo.InsertString( 0, (LPCTSTR) szText );
		LOG.PrintTimeAndLog(0, "CControlDlg::OnDeleteResult: Success :%s:%d[%d]", (LPCSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort, iVersion );
	}
	else 
	{
		CString szText;
		if( iResultType == DELETE_ANSWER_FAIL )
			szText.Format("파일삭제에 실패하였습니다. : %s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort );
		else if( iResultType == DELETE_ANSWER_FAIL_WRONG_VERSION )
		{
			szText.Format( "삭제할 버전이 잘못되었습니다. : %s:%d[Ver:%d]....................<Fail>", (LPCTSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort, iVersion );
		}
		else if( iResultType == DELETE_ANSWER_NOT_CS3 )
			szText.Format("삭제할 cs3 파일이 없습니다. : %s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort );
		else
			szText.Format("알 수 없는 삭제 에러 입니다. : %s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort );

		m_ListBoxInfo.InsertString( 0, (LPCTSTR) szText );
		LOG.PrintTimeAndLog(0, "CControlDlg::OnDeleteResult: Fail :%s:%d:%d[%d]", (LPCSTR) rReceiveInfo.m_szSystemIP, rReceiveInfo.m_iPort, iResultType, iVersion );
	}

	if( m_vCheckInfo.empty() )
	{
		m_vCheckInfo.clear();
		m_dwSendTime = 0; // 시간 체크 정지
		ActiveAllBtn();
		m_eActionType = AT_NONE;
	}
}

void CControlDlg::OnLoadResult( int iResultType, int iVersion, const CheckInfo &rReceiveInfo )
{
	// 메세지 박스 두지 말것
	for(CControlDlg::vCheckInfo::iterator iter = m_vCheckInfo.begin(); iter != m_vCheckInfo.end(); ++iter)
	{
		CheckInfo &rCheckInfo = (*iter);
		if( rCheckInfo.m_szServerIP == rReceiveInfo.m_szServerIP && rCheckInfo.m_iPort == rReceiveInfo.m_iPort )
		{
			m_vCheckInfo.erase( iter );
			break;
		}
	}

	if( iResultType == LOAD_CS3_FILE_SUCCESS )
	{	
		CString szText;
		szText.Format( "Load Success : %s:%d....................<Success>", (LPCSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort );
		m_ListBoxInfo.InsertString( 0, (LPCTSTR) szText );
		LOG.PrintTimeAndLog(0, "CControlDlg::OnLoadResult: Success :%s:%d", (LPCSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort );
	}
	else 
	{
		CString szText;
		if( iResultType == LOAD_CS3_FILE_NOT_FILE )
			szText.Format( "Load할 파일이 없습니다.: %s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort );
		else if( iResultType == LOAD_CS3_FILE_WRONG_VERSION )
			szText.Format( "Load할 파일에 버전이 잘못되었습니다.: %s:%d[Ver:%d]....................<Fail>", (LPCTSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort, iVersion );
		else if( iResultType == LOAD_CS3_FILE_CHANGE_WRONG )
			szText.Format( "Load할 교체 버전이 존재하지 않습니다.: %s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort );
		else if( iResultType == LOAD_CS3_FILE_EXIST_VERSION )
			szText.Format( "Load할 버전이 이미 존재합니다.: %s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort );
		else
			szText.Format( "Load에 실패하였습니다.: %s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort );

		m_ListBoxInfo.InsertString( 0, (LPCTSTR) szText );
		LOG.PrintTimeAndLog(0, "CControlDlg::OnLoadResult: Fail :%s:%d:%d", (LPCSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort, iResultType );
	}


	if( m_vCheckInfo.empty() )
	{
		m_vCheckInfo.clear();
		m_dwSendTime = 0; // 시간 체크 정지
		ActiveAllBtn();
		m_eActionType = AT_NONE;
	}
}

void CControlDlg::OnInfoResult( int iResultType, const CheckInfo &rReceiveInfo, SP2Packet &rkPacket )
{
	// 메세지 박스 두지 말것
	for(CControlDlg::vCheckInfo::iterator iter = m_vCheckInfo.begin(); iter != m_vCheckInfo.end(); ++iter)
	{
		CheckInfo &rCheckInfo = (*iter);
		if( rCheckInfo.m_szServerIP == rReceiveInfo.m_szServerIP && rCheckInfo.m_iPort == rReceiveInfo.m_iPort )
		{
			m_vCheckInfo.erase( iter );
			break;
		}
	}

	if( iResultType == CS3_VERSION_SUCCESS )
	{
		enum { MAX_VERSION_SIZE = 6, };
		for (int i = 0; i < MAX_VERSION_SIZE ; i++)
		{
			int iVersion = 0;
			rkPacket >> iVersion;
			
			CString szText;
			if( i == MAX_VERSION_SIZE-1)
				szText.Format( "Info Success : %s:%d...............(%d)", (LPCSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort, iVersion );
			else
				szText.Format( "Info Success : %s:%d..........[%d]", (LPCSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort, iVersion );
			m_ListBoxInfo.InsertString( 0, (LPCTSTR) szText );
			LOG.PrintTimeAndLog(0, "CControlDlg::OnLoadResult: Success :%s:%d[%d]", (LPCSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort, iVersion );
		}
		m_ListBoxInfo.InsertString( 0, "" ); // 가독성

		if( m_vCheckInfo.empty() )
		{
			m_ListBoxInfo.InsertString(0, "All Info Success....................<Success>" );
			m_vCheckInfo.clear();
			m_dwSendTime = 0; // 시간 체크 정지
			ActiveAllBtn();
			m_eActionType = AT_NONE;
		}
	}
	else
	{
		CString szText;
		szText.Format( "Info 확인에 실패하였습니다. : %s:%d....................<Fail>", (LPCTSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort );
		m_ListBoxInfo.InsertString( 0, (LPCTSTR) szText );
		LOG.PrintTimeAndLog(0, "CControlDlg::OnInfoResult: Fail :%s:%d:%d", (LPCSTR) rReceiveInfo.m_szServerIP, rReceiveInfo.m_iPort, iResultType );
	}
}

void CControlDlg::InActiveAllBtn()
{
	m_ButtonSendCopy.EnableWindow( FALSE );
	m_ButtonSendLoad.EnableWindow( FALSE );
	m_ButtonSendInfo.EnableWindow( FALSE );
	m_EditVersion.EnableWindow( FALSE );

	if( m_CheckBtnServerIP.GetCheck() == 0 )
		m_EditIP.EnableWindow( FALSE );
	if( m_CheckBtnChange.GetCheck() == 0 )
		m_EditChange.EnableWindow( FALSE );

	m_CheckBtnServerIP.EnableWindow( FALSE );
	m_CheckBtnChange.EnableWindow( FALSE );

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
		pSysMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_DISABLED); // Disable
}

void CControlDlg::ActiveAllBtn()
{
	m_ButtonSendCopy.EnableWindow( TRUE );
	m_ButtonSendLoad.EnableWindow( TRUE );
	m_ButtonSendInfo.EnableWindow( TRUE );
	m_EditVersion.EnableWindow( TRUE );

	if( m_CheckBtnServerIP.GetCheck() == 1 )
		m_EditIP.EnableWindow( TRUE );
	if( m_CheckBtnChange.GetCheck() == 1 )
		m_EditChange.EnableWindow( TRUE );

	m_CheckBtnServerIP.EnableWindow( TRUE );
	m_CheckBtnChange.EnableWindow( TRUE );

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
		pSysMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_ENABLED); // Enable
}

void CControlDlg::SendDeleteMsg()
{
	LOG.PrintTimeAndLog(0, "CControlDlg::SendDeleteMsg - Start Copy ");
	if( !m_vCheckInfo.empty() )
	{
		MessageBox("Now Sending. Please Wait.", "Control");
		LOG.PrintTimeAndLog(0, "CControlDlg::SendDeleteMsg - m_vCheckInfo.empty == false ");
		return;
	}

	CString szVersion;
	m_EditVersion.GetWindowText(szVersion);

	if( szVersion.IsEmpty() )
	{
		AfxMessageBox("삭제할 버전이 비어있습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::SendDeleteMsg Version is Empty" );
		return;
	}

	int iVersion = atoi((LPCSTR)szVersion);
	if( !AfxGetMainWnd() )
	{
		LOG.PrintTimeAndLog(0, "CControlDlg::SendDeleteMsg - AfxGetMainWnd == NULL ");
		return;
	}

	CLSMonitorDlg *pDlg = static_cast< CLSMonitorDlg* > ( AfxGetMainWnd() );
	if( !pDlg )
	{
		LOG.PrintTimeAndLog(0, "CControlDlg::SendDeleteMsg - pDlg == NULL ");
		return;
	}

	pDlg->SendDeleteUDPMsg( m_vCheckInfo, iVersion );

	if( m_vCheckInfo.empty() )
	{
		AfxMessageBox("Send Delete을 모두 실패하였습니다.");
		LOG.PrintTimeAndLog( 0, "CControlDlg::SendDeleteMsg All Sending Fail." );
		return;
	}

	for(CControlDlg::vCheckInfo::iterator iter = m_vCheckInfo.begin(); iter != m_vCheckInfo.end(); ++iter)
	{
		CheckInfo &rCheckInfo = (*iter);
		CString szIpPort;
		szIpPort.Format( "Send Delete : %s:%d [%d]", (LPCTSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort, iVersion );

		m_ListBoxInfo.InsertString(0, (LPCTSTR) szIpPort );
		LOG.PrintTimeAndLog(0, "CControlDlg::SendDeleteMsg : Send :%s:%d [%d]", (LPCSTR) rCheckInfo.m_szSystemIP, rCheckInfo.m_iPort, iVersion );
	}

	m_dwSendTime = timeGetTime();
	m_eActionType = AT_SEND_DELETE;
	LOG.PrintTimeAndLog(0, "CControlDlg::SendDeleteMsg - End Copy ");

}

BOOL CControlDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg )
	{
		if(pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CControlDlg::OnBnClickedCheckServerip()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_CheckBtnServerIP.GetCheck() == 1 )
		m_EditIP.EnableWindow( TRUE );
	else
		m_EditIP.EnableWindow( FALSE );
}


void CControlDlg::OnBnClickedCheckChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_CheckBtnChange.GetCheck() == 1 )
		m_EditChange.EnableWindow( TRUE );
	else
		m_EditChange.EnableWindow( FALSE );
}
