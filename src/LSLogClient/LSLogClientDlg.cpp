// LSLogClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "LSLogClient.h"
#include "LSLogClientDlg.h"
#include "NetWork/Protocol.h"
#include "LogTable.h"
#include ".\lslogclientdlg.h"
#include "./CrashFind/BugslayerUtil.h"
#include "Version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLSLogClientDlg 대화 상자



CLSLogClientDlg::CLSLogClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLSLogClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pTCPSocket = NULL;
	m_pDBSocket  = NULL;
}

void CLSLogClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLSLogClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE (WM_TCPSOCKET, OnTCPSocket)
	ON_MESSAGE (WM_DBSOCKET, OnDBSocket)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CLSLogClientDlg 메시지 처리기

BOOL CLSLogClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	MoveWindow(0,0,600,25);
	SetPos(0,0);
	SetTimer(TIMER_10SECOND, 10000, NULL);	
	SetTimer(TIMER_PING, 1800000, NULL);	
	CheckLogNewOpen();

	if(TRUE != SetCrashHandlerFilter(ExceptCallBack))
	{
		LOG.PrintTimeAndLog(0,"SetCrashHandler Failed");
		DestroyWindow();
		return false;
	}

	if( !ioPortalHelp::InitWinSock() )
	{
		LOG.PrintTimeAndLog( 0, "CLSLogServerDlg::OnInitDialog - InitSocket Failed" );
		DestroyWindow();
		return false;
	}

	LoadINI();

	m_pTCPSocket = new ioSP2TCPSocket;
	if( !m_pTCPSocket->InitClientTCP( m_hWnd, WM_TCPSOCKET, m_szIP.c_str(), m_iPort ) )
	{
		LOG.PrintTimeAndLog( 0, "CLSLogServerDlg::OnInitDialog - TCP Socket Failed(%d)", WSAGetLastError() );
		DestroyWindow();
		return false;
	}

	m_pDBSocket = new DBClientSocket;
	if( !m_pDBSocket->InitClientTCP( m_hWnd, WM_DBSOCKET, m_szDBIP.c_str(), m_iDBPort ) )
	{
		LOG.PrintTimeAndLog( 0, "CLSLogServerDlg::OnInitDialog - DB Client Socket Failed(%d)", WSAGetLastError() );
		DestroyWindow();
		return false;
	}

	CString szTitle;
	szTitle.Format("LSLogClient version:%d (REMOTE:%s:%d) Starting", FILEVER, m_szIP.c_str(), m_iPort);
	SetWindowText(szTitle);

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CLSLogClientDlg::OnPaint() 
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
HCURSOR CLSLogClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLSLogClientDlg::SetPos(int iX, int iY)
{
	RECT winrect;
	GetWindowRect(&winrect);
    int winwidth = winrect.right - winrect.left;
    int winheight = winrect.bottom - winrect.top;
	SetWindowPos(NULL,iX,iY,winwidth,winheight,SWP_SHOWWINDOW);
}

void CLSLogClientDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if(AfxMessageBox("Do you want to Exit ?",MB_YESNO) == IDNO)
		return;
	
	CDialog::OnClose();
}

void CLSLogClientDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == TIMER_10SECOND)
	{
		CheckLogNewOpen();
		Write5MinCheck();
		AutoReconnect();
	}
	else if( nIDEvent == TIMER_PING)
	{
		ProcessPing();
	}
	else if(nIDEvent == TIMER_EXIT)
	{
		if(   CheckSocketClose(m_pTCPSocket)
		   && CheckSocketClose(m_pDBSocket)  )
		{
			KillTimer(TIMER_EXIT);
			DestroyWindow();
		}

	}
	CDialog::OnTimer(nIDEvent);
}

void CLSLogClientDlg::CheckLogNewOpen()
{
	char szCurTime[MAX_PATH]="";
	static char szPrevTime[MAX_PATH]="";
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	wsprintf(szCurTime, "%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);	
	
	if(strcmp(szCurTime, szPrevTime)!=0)
	{
		strcpy(szPrevTime, szCurTime);
		LOG.CloseLog();
		char szName[MAX_PATH] = "MLOG\\LOG\\LOG";
		strcat(szName,szCurTime);
		LOG.OpenLog(0,szName,true);
		g_LogTable.ClearAllLogName();
	}
}

void CLSLogClientDlg::Write5MinCheck()
{
	static int timer = 0;
	timer++;
	if(timer > 30)
	{
		LOG.PrintTimeAndLog(0,"5Min Check");
		timer = 0;
	}
}

void CLSLogClientDlg::ReleaseAll()
{
	if( m_pTCPSocket )
	{
		m_pTCPSocket->CloseClientTCP();
		SAFEDELETE(m_pTCPSocket);
	}

	if(m_pDBSocket)
	{
		m_pDBSocket->CloseClientTCP();
		SAFEDELETE(m_pDBSocket);
	}

	ioPortalHelp::CloseWinSock();
	LOG.CloseAndRelease();
	g_LogTable.ReleaseInstance();
	KillTimer(TIMER_10SECOND);
	KillTimer(TIMER_PING);
}

LONG CLSLogClientDlg::OnTCPSocket(WPARAM wParam, LPARAM lParam)
{
	LONG result;
	if( m_pTCPSocket )
	{
		result = m_pTCPSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSLogClientDlg::OnDBSocket(WPARAM wParam, LPARAM lParam)
{
	LONG result;
	if( m_pDBSocket )
	{
		result = m_pDBSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

void CLSLogClientDlg::ProcessPacket( SP2Packet &rkPacket )
{
	__try
	{
		switch(rkPacket.GetPacketID())
		{
		case STPK_CONNECT:
			OnConnect(rkPacket);
			break;
		case STPK_LOG:
			OnLog(rkPacket);
			break;
		case STPK_PING:
			LOG.PrintTimeAndLog(0, "PING");
			break;
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		//
		LOG.PrintTimeAndLog( 0, "ProcessPacket Crash!!" );
		AfxGetApp()->GetMainWnd()->SetTimer(TIMER_EXIT, 5000, NULL);
	}
}

void CLSLogClientDlg::AutoReconnect()
{
	static int timer = 0;
	timer++;
	if(timer > 6)
	{
		_AutoReconnect(m_pTCPSocket, WM_TCPSOCKET, m_szIP, m_iPort );
		_AutoReconnect(m_pDBSocket, WM_DBSOCKET, m_szDBIP, m_iDBPort);

		timer = 0;
	}
}

void CLSLogClientDlg::OnConnect( SP2Packet & rkPacket )
{
	int iSubType;
	rkPacket >> iSubType;
	ioHashString szHelp;
	switch(iSubType)
	{
	case CONNECT_DUPLICATE:
		szHelp = "이미 다른 클라이언트가 접속중입니다.";
		break;
	case CONNECT_WRONG_CHECK_VALE:
		szHelp = "정상적인 클라이언트가 아닙니다.";
	    break;
	}

	if(iSubType != CONNECT_OK)
	{
		AfxMessageBox(szHelp.c_str());
		SetTimer(TIMER_EXIT, 5000, NULL);	
	}
	else
	{
		CString szTitle;
		szTitle.Format("LSLogClient version:%d (REMOTE:%s:%d) %s", FILEVER, m_szIP.c_str(), m_iPort, "Connected");
		SetWindowText(szTitle);
	}
}

void CLSLogClientDlg::OnLog( SP2Packet & rkPacket )
{
	ioHashString szLogName;
	ioHashString szLogContents;
	int          iErrorNumber;

	rkPacket >> szLogName;
	rkPacket >> szLogContents;
	rkPacket >> iErrorNumber;

	g_LogTable.WriteLogContents(szLogName, szLogContents, iErrorNumber);

	if(m_pDBSocket)
		m_pDBSocket->OnInsertLog(m_szZoneName.c_str(), szLogName.c_str(), szLogContents.c_str(), iErrorNumber);
}
void CLSLogClientDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ReleaseAll();
}

void CLSLogClientDlg::_AutoReconnect( ioClientTCPSocket *pClientSocket, UINT uMsg, ioHashString &szIP, int iPort )
{
	if(pClientSocket)
	{
		if(!pClientSocket->IsConnected())
		{
			pClientSocket->InitClientTCP( m_hWnd, uMsg, szIP.c_str(), iPort );
		}
	}
}

bool CLSLogClientDlg::CheckSocketClose( ioClientTCPSocket *pClientSocket )
{
	if(pClientSocket)
	{
		if(! pClientSocket->IsConnected() )
			return true;
		else
		{
			pClientSocket->CloseClientTCP();
			return false;
		}
	}
	
	return true;
}

void CLSLogClientDlg::LoadINI()
{
	ioINILoader kLoader( "LogClientInfo.ini" );
	kLoader.SetTitle( "NETWORK" );

	char szTemp[MAX_PATH]="";
	kLoader.LoadString("IP", "", szTemp, sizeof(szTemp));
	m_szIP = szTemp;
	m_iPort = kLoader.LoadInt("PORT", 9999);	

	memset(szTemp, 0, sizeof(szTemp));
	kLoader.LoadString("DBIP", "", szTemp, sizeof(szTemp));
	m_szDBIP = szTemp;
	m_iDBPort = kLoader.LoadInt("DBPORT", 9999);


	memset(szTemp, 0, sizeof(szTemp));
	kLoader.LoadString("ZONENAME", "", szTemp, sizeof(szTemp));
	m_szZoneName = szTemp;
}

void CLSLogClientDlg::ProcessPing()
{
	SP2Packet kPacket(CTPK_PING);
	m_pTCPSocket->SendLoop(kPacket);
}
