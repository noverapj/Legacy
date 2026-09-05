// LS_ServerInfoDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "LS_ServerInfoDlg.h"
#include "afxdialogex.h"
#define WM_UPDATETOOLTIP WM_USER + 505

// LS_ServerInfoDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(LS_ServerInfoDlg, CDialogEx)

LS_ServerInfoDlg::LS_ServerInfoDlg(CWnd* pParent,ServerInfo_& stdata)
	: CDialogEx(LS_ServerInfoDlg::IDD, pParent)
	, m_static_title(_T(""))
	, m_sendcount(_T(""))
	, m_usercount(_T(""))
	, m_strmaxuser(_T(""))
	, m_strmaxusercount(_T(""))
{
	m_svrInfo.ipaddr = stdata.ipaddr;
	m_svrInfo.port = stdata.port;
	m_svrInfo.sendcount = stdata.sendcount;
	m_svrInfo.usercount = stdata.usercount;
	m_svrInfo.fullcount = stdata.fullcount;
	m_svrInfo.servername = stdata.servername;
	m_svrInfo.serverblockstate = stdata.serverblockstate;
	m_svrInfo.sendserverid = stdata.sendserverid;
	m_maxcount = 0;
	
}

LS_ServerInfoDlg::~LS_ServerInfoDlg()
{
}


void LS_ServerInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_SVRINFO, m_static_title);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Text(pDX, IDC_STATIC_MAXUSER, m_strmaxusercount);
}
 


BEGIN_MESSAGE_MAP(LS_ServerInfoDlg, CDialogEx)
	 
	ON_MESSAGE(WM_LSLOGINSERVER,&LS_ServerInfoDlg::SendPacket)
	ON_STN_CLICKED(IDC_STATIC_SVRINFO, &LS_ServerInfoDlg::OnStnClickedStaticSvrinfo)
	ON_NOTIFY(TTN_NEEDTEXT,NULL,OnNeedText)


	ON_COMMAND(ID_DISABLE_CURRENTSERVER, &LS_ServerInfoDlg::OnDisableCurrentserver)
	ON_COMMAND(ID_ENABLE_CURRNETSERVER, &LS_ServerInfoDlg::OnEnableCurrnetserver)
END_MESSAGE_MAP()


// LS_ServerInfoDlg 메시지 처리기입니다.


void LS_ServerInfoDlg::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	 
}

LRESULT LS_ServerInfoDlg::SendPacket( WPARAM wParam,LPARAM lParam )
{
	if(wParam == EMCONTROLTYPE::LS_SETSERVERBLOCK)
	{
		if(lParam == 1)
			EnableServerState();
		else 
			DisableServerState();

	}
	if(lParam == UPDATESVRINFO)
	{
		
		ServerInfo_ *stdata = (ServerInfo_ *)(wParam);
		m_svrInfo.nstate = stdata->nstate;
		if(m_svrInfo.nstate == -1)
		{
			m_progress.SetBarColor(RGB(222,222,222));
			m_progress.SetBkColor(RGB(222,222,222));
		}
		else if(m_svrInfo.nstate == ESOCKET::LS_ZOMBIESERVERSTATE)
		{
			m_progress.SetBarColor(RGB(222,222,222));
			m_progress.SetBkColor(RGB(222,30,30));
		}
		else
		{
			m_progress.SetBarColor(RGB(60,222,120));
			m_progress.SetBkColor(RGB(30,30,30));
		}
//		m_svrInfo.ipaddr = stdata->ipaddr;
	//	m_svrInfo.port = stdata->port;
		
		m_svrInfo.sendcount = stdata->sendcount;
		m_svrInfo.usercount = stdata->usercount;
		m_svrInfo.fullcount = stdata->fullcount;
		m_progress.SetRange(0,m_svrInfo.fullcount);
		m_progress.SetPos(m_svrInfo.usercount);
		m_usercount.Format("User: %04d",m_svrInfo.usercount);
		m_sendcount.Format("Send: %04d",m_svrInfo.sendcount);
		m_strmaxuser.Format("%05d",m_svrInfo.fullcount);
		if(m_maxcount < m_svrInfo.usercount)
			m_maxcount = m_svrInfo.usercount;
		m_strmaxusercount.Format("Max:%0d",m_maxcount);
		SendMessage(WM_UPDATETOOLTIP,0,0);
		m_tooltip.UpdateData(TRUE);
		UpdateData(FALSE);
		
	} 
	return 0;
}


BOOL LS_ServerInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_progress.SetBarColor(RGB(60,222,120));
	m_progress.SetBkColor(RGB(30,30,30));
	m_static_title.Format("%s",m_svrInfo.servername.c_str());
	UpdateData(FALSE);
	m_tooltip.Create(this);
	m_tooltip.AddTool(&m_progress,LPSTR_TEXTCALLBACK);
	m_tooltip.AddTool(this,LPSTR_TEXTCALLBACK);
	
	//m_tooltip.Activate(true);
	m_tooltip.SetDelayTime(1);
	m_maxcount = 0;
	m_parrentmenu.LoadMenu( IDR_MENU );
	m_menu = m_parrentmenu.GetSubMenu(eServerType_LoginServerInfo -1);
	CString tmp;
	tmp.Format("%s",m_svrInfo.servername.c_str());
	m_menu->ModifyMenuA(ID_LOGINSERVERID,MF_DISABLED|MF_BYCOMMAND,ID_LOGINSERVERID,tmp);
	if(m_svrInfo.serverblockstate == 1)
		EnableServerState();
	else
		DisableServerState();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void LS_ServerInfoDlg::OnStnClickedStaticSvrinfo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL LS_ServerInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_UPDATETOOLTIP:
		m_tooltip.RelayEvent(pMsg);
		break;
	case WM_RBUTTONDOWN:
		{
			CPoint pos;
			GetCursorPos( &pos );
			//m_menu->SetMenuContextHelpId()
			
			
			m_menu->TrackPopupMenu( TPM_LEFTALIGN, pos.x, pos.y, this );
		}
		break;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void LS_ServerInfoDlg::OnNeedText( NMHDR *pnmh, LRESULT* pResult )
{
	TOOLTIPTEXT  *pttt  =  (TOOLTIPTEXT  *)pnmh; 
		 
	wsprintf(pttt->szText,  "%s:%d(%s)\n"
							"UserCount:%04d\n"
		                    "MaxCount:%04d\n"
							"MaxLimit:%04d\n",
							m_svrInfo.ipaddr.c_str(),
							m_svrInfo.port,
							m_svrInfo.servername.c_str(),
							m_svrInfo.usercount,
							m_maxcount,
							m_svrInfo.fullcount); 
	
}

BOOL LS_ServerInfoDlg::OnToolTipNotify( UINT id, NMHDR* pNMHDR, LRESULT* pResult )
{
	AfxMessageBox("aa");
	return 0;

}


void LS_ServerInfoDlg::OnDisableCurrentserver()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(IDYES== MessageBox("정말 진행 하시겠습니까? ","Disable Server",MB_YESNO))
	{
		//패킷 좌라락 ㄱ
		SendServerStatePacket(0);
		DisableServerState();

	}
	else
	{

	}
}


void LS_ServerInfoDlg::OnEnableCurrnetserver()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(IDYES== MessageBox("정말 진행 하시겠습니까? ","Enable Server",MB_YESNO))
	{
		//패킷 좌라락 ㄱ

		SendServerStatePacket(1);

		EnableServerState();

	}
	else
	{

	}
}

void LS_ServerInfoDlg::DisableServerState()
{
	m_menu->CheckMenuItem(ID_ENABLE_CURRNETSERVER,MF_UNCHECKED);
	m_menu->CheckMenuItem(ID_DISABLE_CURRENTSERVER,MF_CHECKED);
}

void LS_ServerInfoDlg::EnableServerState()
{
	m_menu->CheckMenuItem(ID_ENABLE_CURRNETSERVER,MF_CHECKED);
	m_menu->CheckMenuItem(ID_DISABLE_CURRENTSERVER,MF_UNCHECKED);
}

void LS_ServerInfoDlg::SendServerStatePacket( int a )
{
	SP2Packet *pk = new SP2Packet(EPROTOCOL::LSPTK_CONTROL_SERVER);
	int type = EMCONTROLTYPE::LS_SETSERVERBLOCK;
	(*pk) << type;
//	TCHAR ip[STR_IP_MAX];
//	strcpy_s(ip,m_svrInfo.ipaddr.c_str());
//	(*pk) << ip;
	(*pk) << m_svrInfo.sendserverid;
//	(*pk) << m_svrInfo.port;
	(*pk) << a;
	GetParent()->GetParent()->PostMessageA(WM_SENDLOGINSERVER,(WPARAM)pk);
}
