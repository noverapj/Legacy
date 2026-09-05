// ZoneDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_MonitorClient.h"
#include "ZoneDialog.h"
#include "DisplayDlg.h"
#include "CSDialog.h"
#include "LoginDialog.h"
#include "AnnounceDlg.h"
#include "OperatorDlg.h"
#include "GraphDlg.h"
#include "AuthDlg.h"
#include "cFile.h"
#include "afxdialogex.h"
#include "Network/SP2Packet.h"
#include "Network/Protocol.h"
#include "WinMessage.h"
#include "LS_MonitorClientDlg.h"
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

enum MonitorTimerIDs
{
	MONITOR_TIMER_NONE = 0,
	MONITOR_TIMER_TICK,
	MONITOR_TIMER_MAINSERVER,
	MONITOR_TIMER_LOGINSERVER,
	MONITOR_TIMER_BILLINGSERVER,
	MONITOR_TIMER_REQUEST_BILLINGUSER,
	MONITOR_TIMER_BILLINGSERVER_CONNECT,
	MONITOR_TIMER_MONITORSERVER,
	MONITOR_TIMER_END
};



// CZoneDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CZoneDialog, CDialogEx)

CZoneDialog::CZoneDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CZoneDialog::IDD, pParent), m_show(FALSE), m_level(0), m_maxUser(0)
											,m_currentUser(0), m_lastMax(0), m_selectedItem(0)
											, m_displayDlg(NULL), m_loginDlg(NULL), m_operatorDlg(NULL)
											, m_maxBillingUser(0)
											, m_CurrentBillingUser(0), m_lastBillingMax(0)
											, m_billingFlag(FALSE)
											, m_billingLOGReceiveCount(0), m_billingConnCount(0)
											, m_billingTimerFlag(FALSE)
											, m_billingConnTimerFlag(FALSE), m_bCountryCode(FALSE)
											, m_wdENG(0), m_wdGER(0), m_wdFRA(0), m_wdITA(0), m_wdPOL(0), m_wdTUR(0), m_wdETC(0)											
{
}

CZoneDialog::~CZoneDialog()
{
	SAFEDELETE(m_displayDlg);
	SAFEDELETE(m_loginDlg);
	SAFEDELETE(m_operatorDlg);
	SAFEDELETE(m_graphDlg);
	SAFEDELETE(m_file);

	for(int i = 0 ; i < (int)m_otherServerTable.size() ; i++)
		SAFEDELETE(m_otherServerTable[i]);

	for(int i = 0 ; i < (int)m_gameServerTable.size(); i++)
	{
		if(m_gameServerTable[i]->serviceName.Find('_') != -1)
			continue;
		SAFEDELETE(m_gameServerTable[i]);
	}
}

void CZoneDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GAMEDEMONS, m_services[1]);
	DDX_Control(pDX, IDC_LIST_OTHERDEMONS, m_services[0]);
	//DDX_Control(pDX, IDC_OSC_USERCOUNT, m_countGraph);
}

BEGIN_MESSAGE_MAP(CZoneDialog, CDialogEx)
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GAMEDEMONS, &CZoneDialog::OnLvnItemchangedListGamedemons)

	
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CZoneDialog)
	EASYSIZE(IDC_LIST_GAMEDEMONS,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDC_OSC_USERCOUNT,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,ES_HCENTER)
END_EASYSIZE_MAP

// CZoneDialog 메시지 처리기입니다.


BOOL CZoneDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CreateGraph();
	CreateList();
	CreateChild();

	INIT_EASYSIZE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CZoneDialog::Show()
{
	SERVERINFO* pkServerInfo = GetOtherServerInfo(SERVER_TYPE_MAIN);
	CLS_MonitorClientDlg* dlg = (CLS_MonitorClientDlg*)AfxGetMainWnd();
	if(NULL != pkServerInfo)
	{
		dlg->UpdateTitle(pkServerInfo->serviceIP, m_currentUser, m_maxUser);
	}

	ShowWindow(SW_SHOW);
	SetFocus();

	m_show = TRUE;
}

void CZoneDialog::Hide()
{
	ShowWindow(SW_HIDE);
	m_graphDlg->ShowWindow(SW_HIDE);
	m_loginDlg->ShowWindow(SW_HIDE);
	m_operatorDlg->ShowWindow(SW_HIDE);
	m_displayDlg->ShowWindow(SW_HIDE);

	m_show = FALSE;
}

void CZoneDialog::CreateMonitor(CString& title, CString& main, CString& login)
{
	SetBillingFlag( FALSE );
	SetTitle(title);

	int curPos = 0;
	m_MNTClientIP = main.Tokenize(_T(":"), curPos);
	CString mainPort = main.Tokenize(_T(":"), curPos);
	m_MNTClientPort = atoi(mainPort);

	// 소켓생성
	CreateSocket();

	// 로그인서버 판넬생성
	CreatePanel();

	// 팝업메뉴 추가
	CreateMenu();

	// 로그생성
	CreateLog();

	// 팝업생성
	CreatePopup();

	// 현제 날짜 동접정보 읽어오기
	//ReadCurrentUserCCU();
}

void CZoneDialog::CreateBillingMonitor(CString& title, BOOL bBillingFlag, BILLINGSERVERTABLE& tempBillingServerMap)
{
	SetTitle(title);

	SetBillingFlag( bBillingFlag );
	SetBillingTable( tempBillingServerMap );
	
	SetBillingViewList();

	// 소켓생성
	CreateSocket();

	// 로그인서버 판넬생성
	CreatePanel();

	// 팝업메뉴 추가
	CreateMenu();

	// 로그생성
	CreateLog();

	// 팝업생성
	CreatePopup();

	// 현제 날짜 동접정보 읽어오기
	//ReadCurrentUserCCU();
}

void CZoneDialog::SetBillingViewList()
{
	if(m_billingFlag == TRUE)
	{
		m_services[0].ModifyStyle(WS_VISIBLE, 0);		//게임서버 메뉴 숨김

		CRect rect;
		CStatic* staticSize = (CStatic*)GetDlgItem(IDC_LIST_GAMEDEMONS );
		staticSize->GetClientRect(rect);

		int iWidth = rect.Width();
		int iHeight = rect.Height() + 50;
		
		staticSize->MoveWindow(rect.left+15, rect.top+15, iWidth+3, iHeight);

		//LV_COLUMN lvColumn;
		
		for(int i = 0; i < 7; i++)
		{
			m_services[1].DeleteColumn(0);
		
		}
		m_services[1].DeleteAllItems();
		m_services[1].SetImageList(&m_imageList, LVSIL_SMALL);
		m_services[1].SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_FULLROWSELECT);
		
		m_services[1].InsertColumn(0, _T("BillingServer"), LVCFMT_LEFT, iWidth - 170);
		m_services[1].InsertColumn(1, _T("UserInfoCount"), LVCFMT_LEFT, 85);
		m_services[1].InsertColumn(2, _T("Reload INI"), LVCFMT_LEFT, 80);
		
	}
}

void CZoneDialog::OnConnect(const ServerTypes serverType, const int errCode, const int billingkeyIndex)
{
	if(errCode != 0)
	{
		//TRACE(_T("[%s]메인서버 접속실패\r\n"), GetTitle());
		SetTimer(MONITOR_TIMER_MONITORSERVER, 2000, NULL);
	}

	// 마스터
	if(errCode == 0)
	{
		TRACE(_T("[%s]메인서버 접속\r\n"), GetTitle());
		CreateTimer();
		UpdateServer(GetOtherServerInfo(SERVER_TYPE_MAIN), SERVERSTATE_ON);
	}
	else
	{
		UpdateServer(GetOtherServerInfo(SERVER_TYPE_MAIN), SERVERSTATE_OFF);
	}

	// 로그인
	if(errCode == 0)
	{
		TRACE(_T("[%s]로그인서버 접속\r\n"), GetTitle());

		SERVERINFO *serverInfo = GetOtherServerInfo(SERVER_TYPE_LOGIN);
		if(!serverInfo)
		{
			serverInfo = new SERVERINFO;
			serverInfo->serverType = SERVER_TYPE_LOGIN;
			serverInfo->serviceName.Format(_T("LOGIN SERVER"));
			serverInfo->serviceIP	= m_MNTClientIP;
			serverInfo->servicePort	= m_MNTClientPort;

			InsertServer(serverInfo, SERVERSTATE_ON);
			AddOtherServerInfo(serverInfo);
		}

		UpdateServer(GetOtherServerInfo(SERVER_TYPE_LOGIN), SERVERSTATE_ON);

		// 로그인 서버에 모니터링 노드로 접속 요청
		RequestConnect();
	}
	else
	{
		UpdateServer(GetOtherServerInfo(SERVER_TYPE_LOGIN), SERVERSTATE_OFF);
	}
}

void CZoneDialog::OnClose(const ServerTypes serverType, const int errCode, const int billingKeyIndex)
 {
	if(serverType == SERVER_TYPE_MONITOR)
	{
		SetTimer(MONITOR_TIMER_MONITORSERVER, 2000, NULL);
	}
	else if(serverType == SERVER_TYPE_MAIN)
	{
		m_level = ADMIN_LEVEL_NONE;

		TRACE(_T("[%s]메인서버 접속종료 : %d\r\n"), GetTitle(), errCode);
		UpdateServer(GetOtherServerInfo(SERVER_TYPE_MAIN), SERVERSTATE_OFF);
		SetTimer(MONITOR_TIMER_MAINSERVER, 2000, NULL);
	}
	else if(serverType == SERVER_TYPE_LOGIN)
	{
		TRACE(_T("[%s]로그인서버 접속종료 : %d\r\n"), GetTitle(), errCode);
		UpdateServer(GetOtherServerInfo(SERVER_TYPE_LOGIN), SERVERSTATE_OFF);

		SetTimer(MONITOR_TIMER_LOGINSERVER, 2000, NULL);
	}
	else if(serverType == SERVER_TYPE_BILLING)
	{
		m_level = ADMIN_LEVEL_NONE;

		TRACE(_T("[%d]빌링서버 접속종료 : %d\r\n"), billingKeyIndex, errCode);
		MinusBillingConnectCount();
		BILLINGSERVERINFO* serverInfo = GetBillingServerInfo(billingKeyIndex);
		serverInfo->reloadStatus = "";
		UpdateBillingServer(serverInfo, SERVERSTATE_OFF, billingKeyIndex);
		
		//접속중인 빌링서버 없을 경우
		if(GetBillingConnectCount() == 0)
		{
			DestroyTimer();
			SetBillingTimerFlag(FALSE);
		}
		if(!GetBillingConnTimerFlag())
		{
			SetBillingConnTimerFlag(TRUE);
			SetTimer(MONITOR_TIMER_BILLINGSERVER, 2000, NULL);
		}
		//SetTimer(MONITOR_TIMER_BILLINGSERVER, 2000, NULL);
	}
}

void CZoneDialog::OnReceive(const ServerTypes serverType, SP2Packet &rkPacket)
{
	switch(rkPacket.GetPacketID())
	{
	case TOOL_SVR::MSTMC_L_SERVER_RESPONSE:
		{
				OnLSResponse(rkPacket);
		}
		break;

	case TOOL_SVR::MSTMC_L_SERVER_DISCONNECT:
		{
			OnConnectState(rkPacket);
		}
		break;

	case TOOL_SVR::MSTMC_M_SERVER_DISCONNECT:
		{
			OnConnectState(rkPacket);
		}
		break;

	case TOOL_SVR::MSTMC_ANSWER_TODAY_CCU:
		{
			OnAnswerTodayCCU(rkPacket); //기존
		}
		break;

	case TOOL_SVR::MSTMC_ANSWER_NUM_CONNECT:
		{
			OnAnswerNumConnect(rkPacket); //기존
		}
		break;

	case TOOL_SVR::MSTMC_SERVER_DISCONNECT:
		{
			OnServerDisconnect(rkPacket);
		}
		break;

	case TOOL_SVR::MCTMS_ANSWER_SEARCH_CCU:
		{
			OnAnswerSearchCCU(rkPacket); //기존
		}
		break;

	case TOOL_SVR::MSTMC_S_ADMIN_RESPONSE :
		{
			OnAdminResponse(rkPacket);
		}
		break;

	case TOOL_SVR::MCTSC_S_UPDATE_CLIENT_VERSION :
		{
			OnUpdateClientVersion(rkPacket);
		}
		break;

	case STPKM_ANSWER_BILLING_CONNECT:
		{
			OnAnswerBillingConnect( rkPacket );
			
		}
		break;

	case STPKM_BILLING_USER:
		{
			OnBillingUserInfo(rkPacket);
		}
		break;

	case STPKM_BILLINGSERVER_RELOAD_INI :
		{
			OnBillingReloadINIStatus(rkPacket); 
		}
		break;


	case STPKM_SERVER_DISCONNECT:
		{
			TRACE(_T("[%s]STPKM_SERVER_DISCONNECT\r\n"), GetTitle());
			OnServerDisconnect(rkPacket);
		}
		break;
	
	case STPKM_LOAD_CS3_FILE :
		{
			OnLoadCS3File(rkPacket);
		}
		break;

	case STPKM_CS3_FILE_VERSION :
		{
			OnCS3FileVersion(rkPacket);

		}
		break;

	case STPKM_SERVER_INFO_ACK :
		{
			TRACE(_T("[%s]STPKM_SERVER_INFO_ACK\r\n"), GetTitle());
			OnServerInfoAck(rkPacket);
		}
		break;


	case STPKM_LICENSE_ALERT_CHECK : 
		{
			OnReceiveLicenseCheck( rkPacket );
		}
		break;

	default :
		TRACE(_T("[%s]잘못된 패킷ID : %lu\r\n"), GetTitle(), rkPacket.GetPacketID());
		break;
	}
}

void CZoneDialog::CreateSocket()
{
	if(m_billingFlag == FALSE)
	{
		m_sockMNTClient.Disconnect();		

		m_sockMNTClient.Create();

		m_sockMNTClient.Connect(SERVER_TYPE_MONITOR, this, m_MNTClientIP, m_MNTClientPort);		//main / login 접속 
	}
	else	 
	{
		int i = 0;
		for(BILLINGSERVERTABLE::iterator it = m_billingServerTable.begin(); it != m_billingServerTable.end() ; ++it)
		{
			it->second->serviceIP;
			it->second->servicePort;
			
			if(it->second->serviceIP == "")
				continue;
			else
			{
				it->second->billingSocket.Disconnect();
				it->second->billingSocket.Create();
				it->second->billingSocket.BillingServerConnect(SERVER_TYPE_BILLING, 
																this, 
																it->second->serviceIP, 
																it->second->servicePort, 
																it->first);
			}
		}
	}
}

void CZoneDialog::CreateGraph()
{
	CRect rc;
	GetDlgItem(IDC_OSC_USERCOUNT)->GetWindowRect(rc);

	ScreenToClient(rc);
	m_countGraph.DestroyWindow();
	m_countGraph.Create(WS_VISIBLE | WS_CHILD, rc, this); 
	m_countGraph.SetRange( true, 0, 100, 1);
	m_countGraph.SetYUnits(_T(""));

	GetDlgItem(IDC_OSC_USERCOUNT)->ShowWindow(SW_HIDE);
}

void CZoneDialog::CreateList()
{
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP_ALL);
	m_imageList.Create(19, 18, ILC_COLOR24 | ILC_MASK, 5, 5);
	m_imageList.Add(&bmp, RGB(100, 100, 100));
	
	bmp.DeleteObject();

	// 메인서버 및 로그인서버
	m_services[0].Startup(this, 0);
	m_services[0].DeleteAllItems();

	m_services[0].SetImageList(&m_imageList, LVSIL_SMALL);
	m_services[0].SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_FULLROWSELECT);
	m_services[0].InsertColumn(0, _T("Service"), LVCFMT_LEFT, 442);
	
	// 게임서버들
	m_services[1].Startup(this, 1);
	m_services[1].DeleteAllItems();

	m_services[1].SetImageList(&m_imageList, LVSIL_SMALL);
	m_services[1].SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_FULLROWSELECT);
	m_services[1].InsertColumn(0, _T("Service"), LVCFMT_LEFT, 109);
	m_services[1].InsertColumn(1, _T("User"), LVCFMT_LEFT, 60);
	m_services[1].InsertColumn(2, _T("Room"), LVCFMT_LEFT, 49);
	m_services[1].InsertColumn(3, _T("Plaza"), LVCFMT_LEFT, 49);
	m_services[1].InsertColumn(4, _T("BattleRoom"), LVCFMT_LEFT, 67);
	m_services[1].InsertColumn(5, _T("PingMS"), LVCFMT_LEFT, 47);
	m_services[1].InsertColumn(6, _T("DBMS"), LVCFMT_LEFT, 45);
}

void CZoneDialog::CreateChild()
{
	m_displayDlg = new CDisplayDlg;
	m_displayDlg->Create(IDD_DISPLAY_DIALOG, this);
	m_displayDlg->Startup(this);
}

void CZoneDialog::CreateLog()
{
	m_file = NULL;
	/*KISTEC
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	m_wLogYear = sysTime.wYear;
	m_wLogMonth = sysTime.wMonth;
	m_wLogDay = sysTime.wDay;

	CString file;
	file.Format(_T("%s\\%04lu%02lu%02lu.txt"), m_title, m_wLogYear, m_wLogMonth, m_wLogDay);
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\%s"), temp, m_title); 
	CreateDirectory(folder, NULL);
	
	m_file = new cFileWriter;
	if(!m_file->Open(file))
	{
		delete m_file;
		m_file = NULL;
		return;
	}

	m_file->Move(FILE_END, 0);
	*/
}

void CZoneDialog::CreateServerDownLog(SERVERINFO *serverInfo)
{
	Beep(294,500);
	Beep(330,1000);
	Beep(349,500);
	Beep(440,500);
	Beep(494,1000);
	Beep(523,500);
	Beep(523,1000);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	m_wLogYear = sysTime.wYear;
	m_wLogMonth = sysTime.wMonth;
	m_wLogDay = sysTime.wDay;

	CString strfile;
	strfile.Format(_T("ServerDown\\%s.txt"), m_title);
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\ServerDown"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}

	pkfile->Move(FILE_END, 0);

	pkfile->WriteFormat(_T("[%04lu-%02lu-%02lu %02lu:%02lu:%02lu] ServerInfo -> [Name:%s][IP:%s][]Port:%d]\r\n"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, serverInfo->serviceName, serverInfo->serviceIP, serverInfo->servicePort);
}

void CZoneDialog::CreateTimer()
{
	SetTimer(MONITOR_TIMER_TICK, 4000, NULL);	

	SP2Packet kPacket(TOOL_SVR::MCTMS_REQUEST_NUM_CONNECT);
	m_sockMNTClient.SendPacket( kPacket );	//main일 경우

	SetTimer(MONITOR_TIMER_REQUEST_BILLINGUSER, 10000, NULL);//유저인포카운트 요청

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	SP2Packet kPacketCCU(TOOL_SVR::MCTMS_REQUEST_TODAY_CCU);
	kPacketCCU << sysTime.wYear << sysTime.wMonth << sysTime.wDay;
	m_sockMNTClient.SendPacket( kPacketCCU );	//main일 경우
}

void CZoneDialog::DestroyTimer()
{
	KillTimer(MONITOR_TIMER_TICK);
}

void CZoneDialog::CreatePanel()
{
	CString temp;
	temp.Format(_T("%s - Monitoring"), GetTitle());

	m_loginDlg = new CLoginDialog;
	m_loginDlg->Create(IDD_LOGIN_DIALOG, this);
	m_loginDlg->ShowWindow(SW_HIDE);

	m_operatorDlg = new COperatorDlg;
	m_operatorDlg->Create(IDD_OPERATOR_DIALOG, this);
	m_operatorDlg->Startup(this);
	m_operatorDlg->ShowWindow(SW_HIDE);

	m_graphDlg = new CGraphDlg;
	m_graphDlg->Create(IDD_GRAPH_DIALOG, NULL); //추가
	m_graphDlg->Startup(GetTitle(), this);

	temp.Format(_T("%s - CCU Graph"), GetTitle());
	m_graphDlg->SetWindowText(temp);
	//m_graphDlg->SetCountryCode(m_bCountryCode); //ccu용
}

void CZoneDialog::CreatePopup()
{
	CString menu;
	menu = _T("Export reports");

	m_menu.CreatePopupMenu();
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_REPORT, (LPCTSTR)menu);
}


void CZoneDialog::InsertServer(SERVERINFO* serverInfo, const ServerStates state)
{
	int index = 0, control = 0;
	CString title;
	if(serverInfo->serverType == SERVER_TYPE_MAIN)
	{
		control = 0;
		title	= _T("MAIN SERVER");
		index	= m_services[0].GetItemCount();
		serverInfo->serverIndex = index;
	}
	else if(serverInfo->serverType == SERVER_TYPE_LOGIN)
	{
		control = 0;
		title	= _T("LOGIN SERVER");
		index	= m_services[0].GetItemCount();
		serverInfo->serverIndex = index;
	}
	else if(serverInfo->serverType == SERVER_TYPE_GAME)
	{
		control = 1;
		index	= m_services[1].GetItemCount();
		title = serverInfo->serviceName;
		serverInfo->serverIndex = index;
	}
	LVITEM item = {0};

	item.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	item.iItem		= index;
	item.pszText	= title.GetBuffer();
	item.iImage		= state;
	item.lParam		= (LPARAM)serverInfo->serverType;	

	m_services[control].InsertItem(&item);
}

void CZoneDialog::UpdateServer(SERVERINFO* serverInfo, const ServerStates state)
{
	if(!serverInfo)
		return;

	int index = 0, control = 0;
	if(serverInfo->serverType == SERVER_TYPE_MAIN)
	{
		control = 0;
		index	= serverInfo->serverIndex;
	}
	else if(serverInfo->serverType == SERVER_TYPE_LOGIN)
	{
		control = 0;
		index	= serverInfo->serverIndex;
	}
	else if(serverInfo->serverType == SERVER_TYPE_GAME)
	{
		control = 1;
		index	= serverInfo->serverIndex;
	}

	serverInfo->serverState = state;

	LVITEM item = {0};
	item.mask		= LVIF_IMAGE;
	item.iItem		= index;
	if(serverInfo->serviceName.Find('_') != -1)
		item.iImage		= SERVERSTATE_EMPTY;
	else
		item.iImage		= state;
	m_services[control].SetItem(&item);
}

void CZoneDialog::UpdateGameInfo(const int item, SERVERINFO& serverInfo)
{
	//TRACE("[%d], %lu-%lu-%lu-%lu\n", item, serverInfo.connect, serverInfo.roomCount, serverInfo.plazaCount, serverInfo.battleRoomCount);
	CString userCount, roomCount, plazaCount, battleRoomCount, pingMS, DBMS, delay;

	userCount.Format(_T("%lu"), serverInfo.connect);
	roomCount.Format(_T("%lu"), serverInfo.roomCount);
	plazaCount.Format(_T("%lu"), serverInfo.plazaCount);
	battleRoomCount.Format(_T("%lu"), serverInfo.battleRoomCount);
	pingMS.Format(_T("%lu"), serverInfo.pingMS);
	DBMS.Format(_T("%lu"), serverInfo.DBMS);
	delay.Format(_T("%lu"), serverInfo.delay);

	m_services[1].SetItemText(item, 1, userCount);
	if(serverInfo.servicePort != CCU_PORT)
	{
		m_services[1].SetItemText(item, 2, roomCount);
		m_services[1].SetItemText(item, 3, plazaCount);
		m_services[1].SetItemText(item, 4, battleRoomCount);
		m_services[1].SetItemText(item, 5, pingMS);
		m_services[1].SetItemText(item, 6, DBMS);
	}

	if(serverInfo.delay > 1000)
	{
		UpdateServer(&serverInfo, (ServerStates)serverInfo.serverState);
	}
	else
	{
		UpdateServer(&serverInfo, (ServerStates)serverInfo.serverState);
	}
}

void CZoneDialog::UpdateGraph(const int userCount)
{
	m_currentUser = userCount;
	int maxValue = m_lastMax;		//현재 접속한 유저카운트로 y 축 범위 지정
	if(m_lastMax <= m_currentUser)
	{
		if(m_currentUser / 100 == 0) maxValue = 100;
		else if(m_currentUser / 500 == 0) maxValue = 500;
		else if(m_currentUser / 1000 == 0) maxValue = 1000;
		else if(m_currentUser / 2500 == 0) maxValue = 2500;
		else if(m_currentUser / 5000 == 0) maxValue = 5000;
		else if(m_currentUser / 10000 == 0) maxValue = 10000;
		else if(m_currentUser / 20000 == 0) maxValue = 20000;
		else if(m_currentUser / 50000 == 0) maxValue = 50000;
		else if(m_currentUser / 100000 == 0) maxValue = 100000;
		else if(m_currentUser / 500000 == 0) maxValue = 500000;
		else if(m_currentUser / 1000000 == 0) maxValue = 1000000;
	}

	if(m_maxUser < m_currentUser)
	{
		m_maxUser = m_currentUser;
	}

	WriteLog(m_currentUser);
	if(m_lastMax != maxValue)
	{
		//ReadCurrentUserCCU();
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	CString xunits;
	xunits.Format(_T("%04d-%02d-%02d User(%06d/%06d)"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, m_currentUser, m_maxUser);
	m_countGraph.SetXUnits(xunits);
	
	if(m_bCountryCode)
	{
		m_countGraph.AppendCCUPoint(m_wdENG, m_wdGER, m_wdFRA, m_wdITA, m_wdPOL, m_wdTUR, m_wdETC);
		m_graphDlg->AppendCCUPoint(m_wdENG, m_wdGER, m_wdFRA, m_wdITA, m_wdPOL, m_wdTUR, m_wdETC);
	}
	m_countGraph.AppendPoint(m_bCountryCode, m_currentUser);

	m_graphDlg->SetXUnits(xunits);
	m_graphDlg->AppendPoint(m_bCountryCode, m_currentUser);

	if(IsShow())
	{
		SERVERINFO* pkServerInfo = GetOtherServerInfo(SERVER_TYPE_MAIN);
		CLS_MonitorClientDlg* dlg = (CLS_MonitorClientDlg*)AfxGetMainWnd();
		if(NULL != pkServerInfo)
		{
			dlg->UpdateTitle(pkServerInfo->serviceIP, m_currentUser, m_maxUser);
		}
	}
}

void CZoneDialog::UpdateBillingReloadStatus(const int item, BOOL status)
{
	CString strStatus;
	if( status == TRUE )
		strStatus = "O";
	else
		strStatus = "X";

	m_services[1].SetItemText(item, 2, strStatus);
}

void CZoneDialog::UpdateBillingUserCountInfo(const int item, BILLINGSERVERINFO& serverInfo, const int billingKeyIndex)
{
	CString strUserCount;
	strUserCount.Format(_T("%lu"), serverInfo.userInfoCount);

	m_services[1].SetItemText(item, 1, strUserCount);

	//UpdateBillingServer(&serverInfo, SERVERSTATE_ON, billingKeyIndex);
	
}

void CZoneDialog::UpdateBillingGraph()
{
	if (m_billingFlag == TRUE )
	{
		int maxValue = m_lastMax;		//현재 접속한 유저카운트로 y 축 범위 지정
		if(m_lastMax <= m_CurrentBillingUser)
		{
			if(m_CurrentBillingUser / 100 == 0) maxValue = 100;
			else if(m_CurrentBillingUser / 1000 == 0) maxValue = 1000;
			else if(m_CurrentBillingUser / 5000 == 0) maxValue = 5000;
			else if(m_CurrentBillingUser / 10000 == 0) maxValue = 10000;
			else if(m_CurrentBillingUser / 20000 == 0) maxValue = 20000;
			else if(m_CurrentBillingUser / 50000 == 0) maxValue = 50000;
			else if(m_CurrentBillingUser / 100000 == 0) maxValue = 100000;
			else if(m_CurrentBillingUser / 500000 == 0) maxValue = 500000;
			else if(m_CurrentBillingUser / 1000000 == 0) maxValue = 1000000;
		}
		
		if( m_maxBillingUser < m_CurrentBillingUser )
		{
			m_maxBillingUser = m_CurrentBillingUser;
		}
		if(m_lastMax != maxValue)
		{
			m_lastMax = maxValue;
			m_countGraph.SetRange(true, 0, maxValue, 1);
			m_graphDlg->SetRange(true, 0, maxValue, 1);
		}

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		CString xunits;
		xunits.Format(_T("%04d-%02d-%02d User(%06d/%06d)"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, m_CurrentBillingUser, m_maxBillingUser);

		m_countGraph.SetXUnits(xunits);
		m_countGraph.AppendPoint(false, m_CurrentBillingUser);

		m_graphDlg->SetXUnits(xunits);
		m_graphDlg->AppendPoint(false, m_CurrentBillingUser);

		if(IsShow())
		{
			SERVERINFO* pkServerInfo = GetOtherServerInfo(SERVER_TYPE_MAIN);
			CLS_MonitorClientDlg* dlg = (CLS_MonitorClientDlg*)AfxGetMainWnd();
			if(NULL != pkServerInfo)
			{
				dlg->UpdateTitle(pkServerInfo->serviceIP, m_currentUser, m_maxUser);
			}
		}

		m_billingLOGReceiveCount = 0;		
		m_CurrentBillingUser = 0;			//현재 접속유저는 다시 0으로 초기화 함.
	}
}

void CZoneDialog::InsertBillingServer(BILLINGSERVERINFO* serverInfo, const ServerStates state, int billingKeyIndex)
{
	if(!serverInfo)
		return;

	int index = 0, control = 0;
	
	CString sPort, title;
	
	
	
	control = 1;
	serverInfo->billingServerState	= state;		//상태값 변경
	index							= serverInfo->serverListIndex;
		
	sPort.Format(_T("%s:%d"), serverInfo->serviceIP, serverInfo->servicePort);
	title	= _T("BILLING SERVER_") + sPort;
		
	m_billingServerTable[billingKeyIndex] = serverInfo;
	
	LVITEM item = {0};
	item.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	item.iItem		= index;
	item.pszText	= title.GetBuffer();
	item.iImage		= state;
	item.lParam		= (LPARAM)billingKeyIndex;	//item.lParam 에는 m_billingServerTable 의 키값이 저장됨

	m_services[control].InsertItem(&item);
}

void CZoneDialog::UpdateBillingServer(BILLINGSERVERINFO* serverInfo, const ServerStates state, int billingKeyIndex)
{
	if(!serverInfo)
		return;

	int index = 0, control = 0;
	
	CString sPort, title;
	
	control = 1;
	serverInfo->billingServerState	= state;		//상태값 변경
	index							= serverInfo->serverListIndex;	//정렬된 리스트 인덱스 값
		
	sPort.Format(_T("%s:%d"), serverInfo->serviceIP, serverInfo->servicePort);
	title	= _T("BILLING SERVER_") + sPort;
		
	m_billingServerTable[billingKeyIndex] = serverInfo;

	if( state == SERVERSTATE_OFF )
	{
		m_services[1].SetItemText(index, 2, serverInfo->reloadStatus);
	}
	
	LVITEM item = {0};
	item.mask		= LVIF_IMAGE;
	item.iItem		= index;
	item.iImage		= state;
	m_services[control].SetItem(&item);
	
}

BOOL CZoneDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int itemIndex = lParam;
	switch(wParam)
	{
	case IDC_SERVICE_GAMEINFO :
		{
			m_selectedItem = itemIndex;
			RequestGameServerInfo(itemIndex);
		}
		return TRUE;

	case IDC_SERVICE_GAMEQUICKCLOSE :
		{
			RequestGameQuicklyClose(itemIndex);
		}
		return TRUE;

	case IDC_SERVICE_GAMESAFECLOSE :
		{
			RequestGameSafeyClose(itemIndex);
		}
		return TRUE;

	case IDC_SERVICE_RELAYSERVERCONTROL:
		{
			RequestChangeRelayServerState(itemIndex);	
		}
		return TRUE;

	case IDC_SERVICE_LOGINSHOWPANEL :
		{
			m_loginDlg->ShowWindow(SW_SHOW);
		}
		return TRUE;

	case IDC_SERVICE_MAININFO :
		{
			RequestMainInfo();
		}
		return TRUE;

	case IDC_SERVICE_MAINSAFECLOSE :
		{
			RequestMainClose();
		}
		return TRUE;
	
	case IDC_SERVICE_SHOWTOOL:
		{
			m_operatorDlg->ShowWindow(SW_SHOW);
		}
		return TRUE;

	case IDC_SERVICE_AUTHENTICATION:
		{
			CAuthDlg dlg;
			if(IDOK == dlg.DoModal())
			{
				CString ID, PWD;
				dlg.GetText(ID, PWD);

				RequestAuth(ID, PWD);
			}
		}
		return TRUE;

	case IDC_SERVICE_SHOWCS :
		{
			RequestShowCSFile();
		}
		return TRUE;

	case IDC_SERVICE_LOADCS :
		{
			RequestLoadCSFile();
		}
		return TRUE;

	case IDC_SERVICE_MAINRELOADINI :
		{
			RequestReloadGSINI();
		}
		return TRUE;

	case IDC_SERVICE_MAINRELOADOPTION:
		{
			RequestReloadOption();
		}
		return TRUE;
		
	case IDC_SERVICE_RELOAD_CLOSEINI :
		{
			RequestReloadCloseINI();
		}
		return TRUE;

	case IDC_SERVICE_MAINRESETEVENTSHOP :
		{
			ResetEventShopCount();
		}
		return TRUE;

	case IDC_SERVICE_MAINRESETCLOVERSHOP :
		{
			ResetCloverShopCount();
		}
		return TRUE;

	case IDC_SERVICE_MAINRESETMILEGESHOP:
		{
			ResetMilegeShopCount();
		}
		return TRUE;

	case IDC_SERVICE_MAINRESETBUYLOG :
		{
			ReserPurchaseLog();
		}
		return TRUE;
	case IDC_SERVICE_MAIN_RESET_MISSION :
		{
			ReserOldMissionData();
		}
		return TRUE;
	case IDC_SERVICE_GAMEQUICKCLOSEALL :
		{
			RequestGameQuicklyCloseAll();
		}
		return TRUE;

	case IDC_SERVICE_GAMESAFECLOSEALL :
		{
			RequestGameSafeyCloseAll();
		}
		return TRUE;

	case IDC_SERVICE_REPORT :
		{
			RequestExportReports();
		}
		return TRUE;

	case IDC_SERVICE_BILLINGRELOADINI :
		{
			RequestBillingReloadINI( itemIndex );		
		}
		return TRUE;

	case IDC_SERVICE_MAIN_WHITELISTON :
		{
			RequestWhiteList(TRUE, SERVER_TYPE_MAIN);
		}
		return TRUE;

	case IDC_SERVICE_MAIN_WHITELISTOFF : 
		{
			RequestWhiteList(FALSE, SERVER_TYPE_MAIN);
		}
		return TRUE;

	case IDC_SERVICE_LOGIN_WHITELISTON :
		{
			RequestWhiteList(TRUE, SERVER_TYPE_LOGIN);
		}
		return TRUE;

	case IDC_SERVICE_LOGIN_WHITELISTOFF :
		{
			RequestWhiteList(FALSE, SERVER_TYPE_LOGIN);
		}
		return TRUE;
		/* 130912 임시로 사용
	case IDC_SERVICE_BILLINGNEXONLOGOUT :
		{
			RequestBillingNexonPCRoomLogout( itemIndex );
		}
		return TRUE;
		*/
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CZoneDialog::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPoint pos;
	GetCursorPos(&pos);

	m_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);

	CDialogEx::OnRButtonUp(nFlags, point);
}

void CZoneDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent)
	{
	case MONITOR_TIMER_TICK :
		{
			SP2Packet kPacket(TOOL_SVR::MCTMS_REQUEST_NUM_CONNECT);
			m_sockMNTClient.SendPacket( kPacket );	//main일 경우
		}
			break;
		
	case MONITOR_TIMER_MONITORSERVER :
		{
			KillTimer(MONITOR_TIMER_MONITORSERVER);

			m_sockMNTClient.Disconnect();
			m_sockMNTClient.Create();
			m_sockMNTClient.Connect(SERVER_TYPE_MONITOR, this, m_MNTClientIP, m_MNTClientPort);
		}
		break;
		
	case MONITOR_TIMER_BILLINGSERVER :
		{
			//현재 연결된 서버 갯수와 ini에서 읽어온 서버 갯수 같을 경우 타이머 삭제
			if( GetBillingConnectCount() == m_billingServerTable.size() )
			{
				KillTimer(MONITOR_TIMER_BILLINGSERVER);	
				SetBillingConnTimerFlag(FALSE);		//killTimer 후 다시 타이머 설정할 수 있도록..
			}

			for(BILLINGSERVERTABLE::iterator it = m_billingServerTable.begin() ; it != m_billingServerTable.end() ; ++it)
			{
				if( !it->second->serviceIP ) continue;

				if( it->second->billingServerState == SERVERSTATE_OFF )
				{
					it->second->billingSocket.Disconnect();
					it->second->billingSocket.Create();
					it->second->billingSocket.BillingServerConnect(SERVER_TYPE_BILLING, 
																	this, 
																	it->second->serviceIP, 
																	it->second->servicePort, 
																	it->first);
				}
			}
		}
		break;
		
		//GetUserCount 요청함 -> 접속되어 있는 빌링서버에게만 패킷 요청
	case MONITOR_TIMER_REQUEST_BILLINGUSER:
		{
			this->RequestBillingUserInfoCount();
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

SERVERINFO* CZoneDialog::GetGameServerInfo(const int index)
{
	SERVERTABLE::iterator it = m_gameServerTable.find(index);
	if(it != m_gameServerTable.end())
	{
		return it->second;
	}
	return NULL;
}

SERVERINFO* CZoneDialog::GetGameServerInfo(ioHashString& serverName)
{
	for(SERVERTABLE::iterator it= m_gameServerTable.begin(); it != m_gameServerTable.end() ; ++it)
	{
		SERVERINFO* serverInfo = it->second;
		if(!serverInfo) continue;

		if(serverName == serverInfo->serviceName)
		{
			return serverInfo;
		}
	}
	return NULL;
}

SERVERINFO* CZoneDialog::GetGameServerInfo(ioHashString& serverIP, int serverPort)
{
	for(SERVERTABLE::iterator it = m_gameServerTable.begin(); it != m_gameServerTable.end() ; ++it)
	{
		SERVERINFO* serverInfo = it->second;
		if(!serverInfo) continue;

		if((serverIP == serverInfo->serviceIP) && (serverPort == serverInfo->servicePort))
		{
			return serverInfo;
		}
	}
	return NULL;
}

SERVERINFO* CZoneDialog::GetOtherServerInfo(const int index)
{
	SERVERTABLE::iterator it = m_otherServerTable.find(index);
	if(it != m_otherServerTable.end())
	{
		return it->second;
	}
	return NULL;
}

SERVERINFO* CZoneDialog::GetOtherServerInfo(const ServerTypes serverType)
{
	for(SERVERTABLE::iterator it = m_otherServerTable.begin() ; it != m_otherServerTable.end() ; ++it)
	{
		SERVERINFO* serverInfo = it->second;
		if(!serverInfo) continue;

		if(serverType == serverInfo->serverType)
			return serverInfo;
	}
	return NULL;
}

BILLINGSERVERINFO* CZoneDialog::GetBillingServerInfo(const int index)
{
	BILLINGSERVERTABLE::iterator it = m_billingServerTable.find(index);
	if(it != m_billingServerTable.end())
	{
		return it->second;
	}
	return NULL;
	
}

//m_billingServerTable는 ini에서 읽은 순서대로 저장
//클릭된 빌링서버 정보를 구할땐 m_billingServerList 를 이용한다. 
BILLINGSERVERINFO*  CZoneDialog::GetBillingServerListInfo(const int index)
{
	BILLINGSERVERINFO* serverInfo = NULL;
	serverInfo = m_billingServerList[index];

	return serverInfo;

}

void CZoneDialog::AddGameServerInfo(SERVERINFO* serverInfo)
{
	int count = m_gameServerTable.size();
	m_gameServerTable[count] = serverInfo;
}

void CZoneDialog::AddOtherServerInfo(SERVERINFO* serverInfo)
{
	int count = m_otherServerTable.size();
	m_otherServerTable[count] = serverInfo;
}

void CZoneDialog::RequestBillingUserInfoCount()
{
	for(BILLINGSERVERTABLE::iterator it = m_billingServerTable.begin() ; it != m_billingServerTable.end() ; ++it)
	{

		if( !it->second->serviceIP ) continue;

		if( it->second->billingServerState == SERVERSTATE_ON )
		{
			SP2Packet kPacket( CTPKM_BILLINGSERVER_USER_REQUEST );
			it->second->billingSocket.SendPacket( kPacket );
		}
	}
}

void CZoneDialog::ShowInfo(int serverType, int index, CString& title, CString& text)
{
	m_displayDlg->Show(serverType, index, title, text);
}

void CZoneDialog::ShowGraph(MSG* pMsg)
{
	if(m_countGraph.GetSafeHwnd() == pMsg->hwnd)
	{
		//TRACE("Message : %08x\n", pMsg->message);
		if(pMsg->message == WM_LBUTTONUP)
		{
			m_graphDlg->ShowWindow(SW_SHOW);
		}
	}
}

void CZoneDialog::WriteLog(int currentUser)
{
	if(m_file)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		if(false == (m_wLogYear == sysTime.wYear && m_wLogMonth == sysTime.wMonth && m_wLogDay == sysTime.wDay))
		{
			CreateLog();
		}

		m_file->WriteFormat(_T("[%04lu-%02lu-%02lu %02lu:%02lu:%02lu] [%d]\r\n"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, currentUser);
	}
}

void CZoneDialog::RequestInfo(int serverType, int index)
{
	switch(serverType)
	{
	case SERVER_TYPE_MAIN:
		RequestMainInfo();
		break;

	case SERVER_TYPE_GAME:
		RequestGameServerInfo(index);
		break;

	case SERVER_TYPE_BILLING:
		RequestBillingServerLOG(index);
		break;
	}
}

void CZoneDialog::GetAddress(const ServerTypes serverType, const int item, CString& name, CString& IP, int& port)
{
	switch(serverType)
	{
	case SERVER_TYPE_MAIN:
	case SERVER_TYPE_LOGIN:
		{
			SERVERINFO* serverInfo = GetOtherServerInfo(item);
			if(serverInfo)
			{
				name	= serverInfo->serviceName;				
				IP		= serverInfo->serviceIP;
				port	= serverInfo->servicePort;
			}
		}
		break;

	case SERVER_TYPE_GAME:
		{
			SERVERINFO* serverInfo = GetGameServerInfo(item);
			if(serverInfo)
			{
				name	= serverInfo->serviceName;
				IP		= serverInfo->serviceIP;
				port	= serverInfo->servicePort;
			}
		}
		break;
	}
}

void CZoneDialog::GetCCUINFO(const int item, CString& strName, CString& IP, int& port, int& ENGcnt, int& GERcnt, int& FRAcnt, int& ITAcnt, int& POLcnt, int& TURcnt, int& ETCcnt)
{
	SERVERINFO* serverInfo = GetGameServerInfo(item);
	if(serverInfo)
	{
		strName = serverInfo->serviceName;
		IP		= serverInfo->serviceIP;
		port	= serverInfo->servicePort;
// 		ENGcnt	= serverInfo->GB_Connect;
// 		GERcnt	= serverInfo->DE_Connect;
// 		FRAcnt	= serverInfo->FR_Connect;
// 		ITAcnt	= serverInfo->IT_Connect;
// 		POLcnt	= serverInfo->PL_Connect;
// 		TURcnt	= serverInfo->TR_Connect;
// 		ETCcnt	= serverInfo->ETC_Connect;
	}
}

void CZoneDialog::GetBillingAddress(const int serverType, const int item, CString& name, CString& IP, int& port)
{
	switch(serverType)
	{
	case SERVER_TYPE_BILLING:
		{
			BILLINGSERVERINFO* serverInfo = GetBillingServerInfo(item);
			if(serverInfo)
			{
				name	= serverInfo->serviceName;
				IP		= serverInfo->serviceIP;
				port	= serverInfo->servicePort;
			}
		}
		break;
	}
}

BOOL CZoneDialog::GetRelayFlag(int index)
{
	SERVERINFO *serverInfo = GetGameServerInfo(index);
	if(!serverInfo) return FALSE;

	return serverInfo->relayFlag;
}

BOOL CZoneDialog::GetIP(CString& localIP)
{
	CLS_MonitorClientDlg* dlg = (CLS_MonitorClientDlg*)AfxGetMainWnd();
	if(dlg)
	{
//OTGTEST		localIP = dlg->GetIP();
		return TRUE;
	}
	return FALSE;
}

void CZoneDialog::UpdateBegin()
{
	for(SERVERTABLE::iterator it = m_gameServerTable.begin() ; it != m_gameServerTable.end() ; ++it)
	{
		SERVERINFO* serverInfo = it->second;
		if(!serverInfo) continue;

		serverInfo->serverState = SERVERSTATE_OFF;
	}
}

void CZoneDialog::UpdateEnd()
{
	for(SERVERTABLE::iterator it = m_gameServerTable.begin() ; it != m_gameServerTable.end() ; ++it)
	{
		SERVERINFO* serverInfo = it->second;
		if(!serverInfo) continue;

		if(serverInfo->serverState == SERVERSTATE_OFF)
		{
			UpdateServer(serverInfo, SERVERSTATE_OFF);
		}
	}
}


void CZoneDialog::RequestTodayCCU(WORD wYear, WORD wMonth, WORD wDay)
{
	SP2Packet kPacketCCU(TOOL_SVR::MCTMS_REQUEST_TODAY_CCU);
	kPacketCCU << wYear << wMonth << wDay;
	m_sockMNTClient.SendPacket( kPacketCCU );	//main일 경우
}


void CZoneDialog::RequestSearchCCU(WORD wYear, WORD wMonth, WORD wDay)
{
	SP2Packet kPacketCCU(TOOL_SVR::MCTMS_REQUEST_SEARCH_CCU);
	kPacketCCU << wYear << wMonth << wDay;
	m_sockMNTClient.SendPacket( kPacketCCU );	//main일 경우
}

void CZoneDialog::RequestConnect()
{
	SP2Packet kPacket(TOOL_SVR::MCTMS_L_CONTROL_SERVER);
	int type = EMCONTROLTYPE::LS_GETSERVERINFO;
	kPacket << type;
	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestMainInfo()
{
	SERVERINFO *serverInfo = GetOtherServerInfo(SERVER_TYPE_MAIN);
	if(!serverInfo) return;

	SP2Packet kPacket( CTPKM_SERVER_INFO_REQUEST );
	ioHashString serverIP = serverInfo->serviceIP;
	kPacket << serverIP << serverInfo->servicePort;

	m_sockMNTClient.SendPacket( kPacket );
}

void CZoneDialog::RequestGameServerInfo(int index)
{
	SERVERINFO *serverInfo = GetGameServerInfo(index);
	if(!serverInfo) return;

	SP2Packet kPacket( CTPKM_SERVER_INFO_REQUEST );
	ioHashString serverIP = serverInfo->serviceIP;
	kPacket << serverIP << serverInfo->servicePort;

	m_sockMNTClient.SendPacket( kPacket );
}

void CZoneDialog::RequestMainClose()
{
	if( MessageBox( "Are you sure to shutdown?", "Warning", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_EXIT);
		kPacket << m_szID << m_szPwd;
		m_sockMNTClient.SendPacket(kPacket);
	}
}

//빌링서버에게 로그 요청 
void CZoneDialog::RequestBillingServerLOG(int billingKeyindex)
{
	SP2Packet kPacket( CTPKM_SERVER_INFO_REQUEST );
	
	BILLINGSERVERTABLE::iterator it = m_billingServerTable.find(billingKeyindex);
	if(it != m_billingServerTable.end())
	{
		it->second->billingSocket.SendPacket( kPacket );
	}
}

void CZoneDialog::RequestWhiteList(BOOL bWhiteList, ServerTypes eServerType)
{
	switch( eServerType )
	{
	case SERVER_TYPE_MAIN:
		{
			SP2Packet reqPacket( TOOL_SVR::MCTMS_C_WHITELIST_REQUEST );
			reqPacket << bWhiteList;
			m_sockMNTClient.SendPacket( reqPacket );
		}
		break;
	}
}

void CZoneDialog::WriteLogDlg( const TCHAR* format, ...)
{
	TCHAR buffer[4096];

	va_list marker;
	va_start(marker, format);
	_vstprintf_s(buffer, _countof(buffer), format, marker);
	va_end(marker);

	CString text( buffer,  _tcslen(buffer) );

	return WriteLogDlg( text );
}

void CZoneDialog::WriteLogDlg( CString text )
{
	return m_operatorDlg->WriteLogDlg( text );
}


void CZoneDialog::RequestGameSafeyClose(int index)
{
	if( MessageBox( "Are you sure to shutdown?", "Warning", MB_YESNO ) == IDYES )
	{
		SERVERINFO *serverInfo = GetGameServerInfo(index);
		if(!serverInfo) return;

		ioHashString serverIP = serverInfo->serviceIP;

		SP2Packet kPacket( TOOL_SVR::MCTMS_C_GAMESERVER_PROTOCAL );
		kPacket << m_szID << m_szPwd;
		kPacket << serverIP << serverInfo->servicePort;
		kPacket << (DWORD)CTPKM_GAMESERVER_SAFETY_EXIT;

		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::RequestGameQuicklyClose(int index)
{
	if( MessageBox( "Are you sure to shutdown?", "Warning", MB_YESNO ) == IDYES )
	{
		SERVERINFO *serverInfo = GetGameServerInfo(index);
		if(!serverInfo) return;

		ioHashString serverIP = serverInfo->serviceIP;

		SP2Packet kPacket( TOOL_SVR::MCTMS_C_GAMESERVER_PROTOCAL );
		kPacket << m_szID << m_szPwd;
		kPacket << serverIP << serverInfo->servicePort;
		kPacket << (DWORD)CTPKM_GAMESERVER_QUICK_EXIT;

		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::RequestGameSafeyCloseAll()
{
	if( MessageBox( _T("Are you sure to shutdown all?"), "Warning", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_SAFETYALLEXIT );

		DWORD dwType = ALL_SERVER_SAFETY_EXIT;	// safety
		kPacket << m_szID << m_szPwd;
		kPacket << dwType;
		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::RequestExportReports()
{
	if( MessageBox( _T("Are you sure to export all?"), "Warning", MB_YESNO ) == IDYES )
	{
		cFileWriter file;
		if(!file.Open("report.txt"))
		{
			MessageBox( _T("Failed to create reports"), "Warning", MB_OK );
			return;
		}

		file.Move(FILE_END, 0);

		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		file.WriteFormat("===========================================================\r\n");
		file.WriteFormat("[%04lu-%02lu-%02lu %02lu:%02lu:%02lu] Current Report\r\n", 
			sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		file.WriteFormat("===========================================================\r\n");

		int count = m_services[1].GetItemCount();
		for(int i = 0 ; i < count ; i++)
		{
			int index = 0;
			CString service			=  m_services[1].GetItemText( i, index++ );
			CString userCount		=  m_services[1].GetItemText( i, index++ );
			CString roomCount		=  m_services[1].GetItemText( i, index++ );
			CString plazaCount		=  m_services[1].GetItemText( i, index++ );
			CString battleRoomCount =  m_services[1].GetItemText( i, index++ );
			CString pingMS			=  m_services[1].GetItemText( i, index++ );
			CString DBMS			=  m_services[1].GetItemText( i, index++ );

			file.WriteFormat("%s\t%s\t%s\t%s\t%s\t%s\t%s\r\n", service, userCount, roomCount, plazaCount, battleRoomCount, pingMS, DBMS);
		}

		file.WriteFormat("\r\n\r\n");
		file.Close();
	}
}

void CZoneDialog::RequestGameQuicklyCloseAll()
{
	if( MessageBox( "Are you sure to shutdown all?", "Warning", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_QUICKALLEXIT );
		
		DWORD dwType = ALL_SERVER_QUICK_EXIT;	// quick
		kPacket << m_szID << m_szPwd;
		kPacket << dwType;
		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::RequestShowCSFile()
{
	m_operatorDlg->GetCSDlg()->Log("Begin downloading...");
	ioHashString serverIP = "";

	SP2Packet kPacket(CTPKM_CS3_FILE_VERSION);
	kPacket << serverIP;

	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestLoadCSFile()
{
	CString version, change;
	m_operatorDlg->GetCSDlg()->GetVersion(version, change);

	ioHashString serverIP = "";
	if( version.IsEmpty() )
	{
		MessageBox("Load CS failed - arguments");
		return;
	}

	int csVersion = atoi((LPCSTR)version);
	if( csVersion <= 0 )
	{
		MessageBox("Load CS failed - arguments");
		return;
	}

	int csChange = 0;
	if( !change.IsEmpty() )
	{
		csChange = atoi((LPCSTR)change);
	}

	SP2Packet kPacket(CTPKM_LOAD_CS3_FILE);
	kPacket << serverIP;
	kPacket << csVersion;
	kPacket << csChange;

	m_sockMNTClient.SendPacket(kPacket);

	RequestUpdateRivision(csVersion); 

	for(SERVERTABLE::iterator it = m_gameServerTable.begin() ; it != m_gameServerTable.end() ; ++it)
	{
		SERVERINFO* serverInfo = it->second;
		if(!serverInfo) continue;

		m_operatorDlg->GetCSDlg()->Log(_T("(Sending : %s:%d"), serverInfo->serviceIP, serverInfo->servicePort);
	}
}

void CZoneDialog::RequestUpdateRivision(int version)
{
	bool useVersion = (version != 0 ) ? true : false;

	SP2Packet kPacket(TOOL_SVR::MCTSC_C_UPDATE_CLIENT_VERSION);
	kPacket << useVersion;
	kPacket << version;

	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestKickUser(CString publicID)
{
	ioHashString ioPublicID	= publicID.GetBuffer(0);

	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);

	int type = ADMINCOMMAND_MTOOL_KICK;
	kPacket << type;
	kPacket << m_szID;
	kPacket << m_szPwd;
	kPacket << ioPublicID;
	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestPractice(CString publicID)
{
	ioHashString ioPublicID	= publicID.GetBuffer(0);

	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);

	int type = ADMINCOMMAND_MTOOL_PRACTICE;
	kPacket << type;
	kPacket << m_szID;
	kPacket << m_szPwd;
	kPacket << ioPublicID;
	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestAnnounce(int msgType, CString publicID, CString announce)
{
	ioHashString ioAnnounce = announce.GetBuffer(0);
	ioHashString ioPublicID	= publicID.GetBuffer(0);

	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);

	int type = ADMINCOMMAND_MTOOL_ANNOUNCE;
	kPacket << type;
	kPacket << m_szID;
	kPacket << m_szPwd;
	kPacket << ioAnnounce;
	kPacket << msgType;
	if(msgType == ANNOUNCE_TYPE_ONE)
	{
		kPacket << ioPublicID;
	}
	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestReloadCloseINI()
{
	if( MessageBox( "Are you sure to reload?", "Warning", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_RELOADCLOSEINFO );
		kPacket << m_szID;
		kPacket << m_szPwd;
		m_sockMNTClient.SendPacket(kPacket);
	}
}

//OnCommand에서 인덱스 번호를 넘겨줌
//리스트의 인덱스 번호를 이용해 reload 요청
void CZoneDialog::RequestBillingReloadINI( int billingKeyIndex )
{
	BILLINGSERVERINFO* serverInfo;
	serverInfo = GetBillingServerListInfo( billingKeyIndex );
	
	CString text;
	text.Format( "Are you sure to reload?");

	if( MessageBox( text, "Warning", MB_YESNO ) == IDYES )
	{
			

		SP2Packet kPacket( CTPKM_BILLINGSERVER_RELOAD_INI );
		CString localIP;
		ioHashString mgrServerIP;
		if(GetIP(localIP))
			mgrServerIP = localIP.GetBuffer(0);
		
		kPacket << mgrServerIP;
		

		if(serverInfo != NULL)
		{
			serverInfo->billingSocket.SendPacket(kPacket);
			
			
			if(m_file)
			{
				SYSTEMTIME sysTime;
				GetLocalTime(&sysTime);

				m_file->WriteFormat(_T("[%04lu-%02lu-%02lu %02lu:%02lu:%02lu] : [%s:%d]%20s\r\n"), 
					sysTime.wYear, 
					sysTime.wMonth, 
					sysTime.wDay, 
					sysTime.wHour, 
					sysTime.wMinute, 
					sysTime.wSecond, 
					serverInfo->serviceIP, 
					serverInfo->servicePort,
					"Request Billing Server Reload INI");
			}
		}
	}
}

void CZoneDialog::ResetEventShopCount()
{
	if( MessageBox( "Are you sure to reset?", "Warning", MB_YESNO ) == IDYES )
	{
		int type = 1;

		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_RESETEVENTSHOP );
		kPacket << type;

		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::ResetCloverShopCount()
{
	if( MessageBox( "Are you sure to reset?", "Warning", MB_YESNO ) == IDYES )
	{
		int type = 2;

		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_RESETEVENTSHOP );
		kPacket << type;

		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::ResetMilegeShopCount()
{
	if( MessageBox( "Are you sure to reset?", "Warning", MB_YESNO ) == IDYES )
	{
		int type = 3;

		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_RESETEVENTSHOP );
		kPacket << type;

		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::ReserPurchaseLog()
{
	if( MessageBox( "Are you sure to reset?", "Warning", MB_YESNO ) == IDYES )
	{
		int type = 0;

		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_RESETEVENTSHOP );
		kPacket << type;
		m_sockMNTClient.SendPacket(kPacket);
	}
}

/***********************************************************************
해외용 : 
Mission Data의 경우, Mission Code로 DB에 기록되는데
이는 Rotation Mission 기능을 사용할 때 미션코드가 반복되어 사용되므로,
문제가 발생된다. 따라서 오래된 미션 데이터를 삭제한다.
************************************************************************/
void CZoneDialog::ReserOldMissionData()
{
	if( MessageBox( "Are you sure to reset?", "Warning", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_RESET_OLDMISSIONDATA );
		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::RequestAuth(CString ID, CString PWD)
{
	m_szID = ID;
	m_szPwd = PWD;

	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);

	int type = ADMINCOMMAND_MTOOL_AUTH;
	kPacket << type;
	kPacket << m_szID;
	kPacket << m_szPwd;
	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestChangeRelayServerState(int index)
{
	if( MessageBox( "Are you sure to change the flag?", "Warning", MB_YESNO ) == IDYES )
	{
		SERVERINFO *serverInfo = GetGameServerInfo(index);
		if(!serverInfo) return;

		ioHashString serverIP	= serverInfo->serviceIP;
		BOOL relayFlag			= !(serverInfo->relayFlag);

		SP2Packet kPacket( TOOL_SVR::MCTMS_C_GAMESERVER_PROTOCAL );
		kPacket << m_szID << m_szPwd;
		kPacket << serverIP << serverInfo->servicePort;
		kPacket << (DWORD)CTPKM_GAMESERVER_CHANGE_RELAYSVR_STATE;
		kPacket << relayFlag;

		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::RequestItemInsert(CString userId, int presentType, int value1, int value2, int ment, int isPublicID)
{
	ioHashString ioUserId = userId;

	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);

	int type = ADMINCOMMAND_MTOOL_ITEMINSERT;
	kPacket << type;
	kPacket << m_szID;
	kPacket << m_szPwd;
	kPacket << ioUserId;
	kPacket << presentType;
	kPacket << value1;
	kPacket << value2;
	kPacket << ment;
	kPacket << isPublicID;

	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestSecretShopCountChange(int iCode, int iCount)
{
	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);

	int type = ADMINCOMMAND_MTOOL_SECRETSHOP;
	kPacket << type;
	kPacket << iCode;
	kPacket << iCount;

	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestCompensation(int iType, int iCode, int iValue, __int64 iEndDate)
{
	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);

	int type = ADMINCOMMAND_MTOOL_COMPENSATION;
	kPacket << type;
	kPacket << iType;
	kPacket << iCode;
	kPacket << iValue;
	kPacket << iEndDate;

	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestEventInsert(int values[], int valueCount)
{
	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);

	int type = ADMINCOMMAND_MTOOL_EVENTINSERT;
	kPacket << type;
	kPacket << m_szID;
	kPacket << m_szPwd;
	kPacket << valueCount;
	for(int i = 0 ; i < valueCount ; i++)
	{
		kPacket << values[i];
	}
	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestUserBlock(CString publicID, CString note, int64 limitTime, int level)
{
	if( !IsAdministrator() && !IsOperator() )
		return ;

	ioHashString ioPublicID		= publicID.GetBuffer(0);
	ioHashString ioReason		= _T(" ");
	ioHashString ioBlockNote	= note.GetBuffer(0);
		
	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);
	
	int type = ADMINCOMMAND_MTOOL_USERBLOCK;
	kPacket << type;
	kPacket << m_szID;
	kPacket << m_szPwd;
	kPacket << ioPublicID;
	kPacket << ioReason;
	kPacket << ioBlockNote;
	kPacket << limitTime;

	switch (level)
	{
	case BLOCKTYPE_WARNING:
		kPacket << BKT_WARNNING;
		break;
	case BLOCKTYPE_LIMITATION:
		kPacket << BKT_LIMITATION;
		break;
	case BLOCKTYPE_BLOCK:
		kPacket << BKT_BLOCK;
		break;
	}

	m_sockMNTClient.SendPacket(kPacket);
}

void CZoneDialog::RequestUserUnblock(CString publicID)
{
	if( !IsAdministrator() && !IsOperator() )
		return ;

	ioHashString ioPublicID		= publicID.GetBuffer(0);
	ioHashString ioReason		= _T(" ");
	ioHashString ioBlockNote	= _T("Unblock");

	SP2Packet kPacket(TOOL_SVR::MCTMS_C_MAINSERVER_ADMINCOMMAND);

	int type = ADMINCOMMAND_MTOOL_USERUNBLOCK;
	kPacket << type;
	kPacket << m_szID;
	kPacket << m_szPwd;
	kPacket << ioPublicID;
	kPacket << ioReason;
	kPacket << ioBlockNote;

	m_sockMNTClient.SendPacket(kPacket);	
}

void CZoneDialog::RequestReloadGSINI()
{
	if( MessageBox( "Are you sure to reload?", "Warning", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_GAMESERVER_RELOADINI );
		kPacket << m_szID;
		kPacket << m_szPwd;
		m_sockMNTClient.SendPacket(kPacket);
	}
}

void CZoneDialog::RequestReloadOption()
{
	if( MessageBox( "Are you sure to reload?", "Warning", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket( TOOL_SVR::MCTMS_C_MAINSERVER_GAMESERVER_OPTION );
		m_sockMNTClient.SendPacket(kPacket);
	}
}

BOOL Compare(SERVERINFO* left, SERVERINFO* right)
{
	if(left->serviceIP < right->serviceIP)
		return TRUE;
	return FALSE;
}

BOOL BillingPortCompare(BILLINGSERVERINFO* left, BILLINGSERVERINFO* right)
{
	if(left->servicePort < right->servicePort)
		return TRUE;
	return FALSE;
}

void CZoneDialog::SortBillingList()
{
	// 정렬
	std::sort(m_billingServerList.begin(), m_billingServerList.end(), BillingPortCompare);

	// 리스트 인덱스 재부여
	for(int i = 0 ; i < (int)m_billingServerList.size() ; i++)
	{
		
		//정렬된 순으로 리스트 인덱스 부여
		BILLINGSERVERINFO* serverInfo = m_billingServerList[i];
		
		if(serverInfo->serverListIndex != -1)
		{
			m_billingServerList[i]->serverListIndex = i;
			UpdateBillingServer(m_billingServerList[i],
								SERVERSTATE_ON, 
								m_billingServerList[i]->serverKeyIndex);
		}
		else
		{
			m_billingServerList[i]->serverListIndex = i;
			InsertBillingServer(m_billingServerList[i], SERVERSTATE_ON, m_billingServerList[i]->serverKeyIndex);
		}
	}
}

void CZoneDialog::OnAnswerBillingConnect(SP2Packet &rkPacket)
{
	////////////////////////////////////////////////////////////
	// 빌링서버 갱신
	////////////////////////////////////////////////////////////
	int billingKeyIndex = 0, userInfoCount = 0;
	rkPacket >> billingKeyIndex;
	rkPacket >> userInfoCount;

	BILLINGSERVERTABLE::iterator it = m_billingServerTable.find(billingKeyIndex);
	if(it != m_billingServerTable.end())
	{
		it->second->billingServerState	= SERVERSTATE_ON;
		it->second->userInfoCount       = userInfoCount;	//유저인포카운트도 함께 출력
		if( it->second->serverListIndex == -1 )	//리스트에 없는 빌링서버->첫연결시
		{
			m_billingServerList.push_back(it->second);
			SortBillingList();
		}
		else
			UpdateBillingServer(it->second, SERVERSTATE_ON, billingKeyIndex);
	}
	UpdateBillingUserCountInfo(it->second->serverListIndex, *(it->second), billingKeyIndex);		//컬럼 유저인포 갱신
}

void CZoneDialog::OnAnswerTodayCCU(SP2Packet &rkPacket)
{
	WORD wLogYear, wLogMonth, wLogDay;
	int iLastMaxCount = 0, iLineCount = 0;
	rkPacket >> wLogYear >> wLogMonth >> wLogDay >> iLastMaxCount >> iLineCount;

	if(iLineCount == 0)
	{
		return ;
	}

	m_lastMax = iLastMaxCount;
	m_countGraph.InitDate();
	m_graphDlg->InitDate();
	m_countGraph.SetRange(true,	0, m_lastMax, 1);
	m_graphDlg->SetRange(true,	0, m_lastMax, 1);

	for(int i = 0; i < iLineCount; i++)
	{
		WORD wHour =0, wMinute = 0;
		int iCount = 0;
		rkPacket >> wHour >> wMinute >> iCount;
			
		if(m_maxUser < iCount)
		{
			m_maxUser = iCount;
		}

		CString xunits;
		xunits.Format(_T("%04d-%02d-%02d User(%06d/%06d)"), wLogYear, wLogMonth, wLogDay, iCount, m_maxUser);
		m_countGraph.SetXUnits(xunits);

		m_countGraph.AppendDateTimePoint(wLogYear, wLogMonth, wLogDay, wHour, wMinute, 00, m_bCountryCode, iCount);
		m_graphDlg->SetXSearchUnits(xunits);
		m_graphDlg->AppendDateTimePoint(wLogYear, wLogMonth, wLogDay, wHour, wMinute, 00, iCount);
	}
	m_graphDlg->ComboBoxSetting(wLogYear, wLogMonth, wLogDay);
}

void CZoneDialog::OnAnswerSearchCCU(SP2Packet &rkPacket)
{
	WORD wLogYear, wLogMonth, wLogDay;
	int iLastMaxCount = 0, iLineCount = 0;
	rkPacket >> wLogYear >> wLogMonth >> wLogDay >> iLastMaxCount >> iLineCount;

	if(iLineCount == 0)
	{
		CString xunits;
		m_graphDlg->GraphCCUInit();
		xunits.Format(_T("  %04d-%02d-%02d Data Not Found File  "), wLogYear, wLogMonth, wLogDay);
		m_graphDlg->SetXSearchUnits(xunits);
		return ;
	}

	m_graphDlg->InitDate();
	m_lastMax = iLastMaxCount;
	m_graphDlg->SetRange(true,	0, m_lastMax, 1);
	int MaxUser = 0;

	CString xunits;
	for(int i = 0; i < iLineCount; i++)
	{
		WORD wHour =0, wMinute = 0;
		int iCount = 0;
		rkPacket >> wHour >> wMinute >> iCount;
			
		if(MaxUser < iCount)
		{
			MaxUser = iCount;
		}
		xunits.Format(_T("   %04d-%02d-%02d MaxUser(%06d)   "), wLogYear, wLogMonth, wLogDay, MaxUser);
		m_graphDlg->SetXSearchUnits(xunits);
		m_graphDlg->AppendDateTimePoint(wLogYear, wLogMonth, wLogDay, wHour, wMinute, 00, iCount);
	}
}

void CZoneDialog::OnAnswerNumConnect(SP2Packet &rkPacket)
{
	int serverCount=0;
	rkPacket >> serverCount;
	UpdateBegin();

	////////////////////////////////////////////////////////////
	// 게임서버 갱신
	////////////////////////////////////////////////////////////
	int userCount = 0;
	if(GetGameServerCount() == 0)
	{
		// 생성
		std::vector<SERVERINFO*> serverList;
		for(int i = 0 ; i < (serverCount-1) ; i++)
		{
			ioHashString serverName, serverIP;
			int serverPort;

			rkPacket >> serverName >> serverIP >> serverPort; 
			if(0 == serverPort) continue;

			SERVERINFO* serverInfo = new SERVERINFO;
			serverInfo->serverType	= SERVER_TYPE_GAME;
			serverInfo->serviceName = serverName.c_str();

			rkPacket >> serverInfo->connect >> serverInfo->roomCount >> serverInfo->plazaCount >> serverInfo->battleRoomCount;
			rkPacket >> serverInfo->pingMS >> serverInfo->DBMS >> serverInfo->delay >> serverInfo->relayFlag >> serverInfo->serverState;

			serverInfo->serviceIP	= serverIP.c_str();
			serverInfo->servicePort	= serverPort;

			serverList.push_back(serverInfo);

			userCount += serverInfo->connect;
		}
		
		// 정렬
		std::sort(serverList.begin(), serverList.end(), Compare);
		// 입력
		for(int i = 0 ; i < (int)serverList.size() ; i++)
		{
			SERVERINFO* serverInfo = serverList[i];

			AddGameServerInfo(serverInfo);
			
			InsertServer(serverInfo, SERVERSTATE_ON);
			UpdateGameInfo(serverInfo->serverIndex, *serverInfo);
		}
		if(m_bCountryCode) //CCU서버 추가
		{
			//park
			m_services[1].m_bCountryCount = m_bCountryCode;
			SetUserEachOfCountry(SERVER_TYPE_GAME, 0, CCU_ENG,	&OtherSvrInfo[CI_EG],	SERVERSTATE_ON);
			SetUserEachOfCountry(SERVER_TYPE_GAME, 0, CCU_GER,	&OtherSvrInfo[CI_GE],	SERVERSTATE_ON);
			SetUserEachOfCountry(SERVER_TYPE_GAME, 0, CCU_FRA,	&OtherSvrInfo[CI_FR],	SERVERSTATE_ON);
			SetUserEachOfCountry(SERVER_TYPE_GAME, 0, CCU_ITA,	&OtherSvrInfo[CI_IT],	SERVERSTATE_ON);
			SetUserEachOfCountry(SERVER_TYPE_GAME, 0, CCU_POL,	&OtherSvrInfo[CI_PL],	SERVERSTATE_ON);
			SetUserEachOfCountry(SERVER_TYPE_GAME, 0, CCU_TUR,	&OtherSvrInfo[CI_TR],	SERVERSTATE_ON);
			SetUserEachOfCountry(SERVER_TYPE_GAME, 0, CCU_ETC,	&OtherSvrInfo[CI_ETC],	SERVERSTATE_ON);
		}

		// 게임 서버 라이센스 30일 이하 알람 요청
		if( m_bCountryCode == TRUE )
		{			
			// 모니터링 툴 -> 메인서버  -> 게임서버
			SP2Packet reqPacket( CTPKM_LICENSE_REQUEST );
			m_sockMNTClient.SendPacket( reqPacket );
		}
	}
	else
	{
		//밑에서 서버정보 읽기위해
		SERVERINFO* EachserverInfo = new SERVERINFO;

		for(int i = 0 ; i < (serverCount-1) ; i++)
		{
			ioHashString serverName, serverIP;
			int serverPort;
			rkPacket >> serverName >> serverIP >> serverPort;
			if(0 == serverPort) continue;

			SERVERINFO *serverInfo = GetGameServerInfo(serverIP, serverPort);
			if(!serverInfo)
			{
				serverInfo = new SERVERINFO;
				serverInfo->serverType	= SERVER_TYPE_GAME;
				serverInfo->serviceName = serverName.c_str();
				serverInfo->serviceIP	= serverIP.c_str();
				serverInfo->servicePort	= serverPort;

				AddGameServerInfo(serverInfo);
				InsertServer(serverInfo, SERVERSTATE_ON);
			}

			rkPacket >> serverInfo->connect >> serverInfo->roomCount >> serverInfo->plazaCount >> serverInfo->battleRoomCount;
			rkPacket >> serverInfo->pingMS >> serverInfo->DBMS >> serverInfo->delay >> serverInfo->relayFlag >> serverInfo->serverState;

			// 갱신
			UpdateGameInfo(serverInfo->serverIndex, *serverInfo);
			userCount += serverInfo->connect;
		}
		SAFEDELETE(EachserverInfo);
	}
	UpdateEnd();

	ioHashString mainIP;
	int mainPort;
	int serverState;
	rkPacket >> mainIP >> mainPort;
	rkPacket >> serverState; // 이후 데이타는 모두 0

	////////////////////////////////////////////////////////////
	// 메인서버 갱신
	////////////////////////////////////////////////////////////
	SERVERINFO *serverInfo = GetOtherServerInfo(SERVER_TYPE_MAIN);
	if(!serverInfo)
	{
		serverInfo = new SERVERINFO;
		serverInfo->serverType = SERVER_TYPE_MAIN;

		AddOtherServerInfo(serverInfo);
		InsertServer(serverInfo, ServerStates(serverState));
	}
	else
	{
		UpdateServer(serverInfo, ServerStates(serverState));
	}

	serverInfo->serviceName.Format(_T("MAIN SERVER"));
	serverInfo->serviceIP		= mainIP.c_str();
	serverInfo->servicePort		= mainPort;
	serverInfo->serverState		= serverState;

	// 그래프 갱신
	if(serverCount > 1)	UpdateGraph(userCount);

}

void CZoneDialog::OnReceiveLicenseCheck(SP2Packet &rkPacket)
{
	DWORD dwPeriod, dwLicenseDate, dwLocalDate;
	ioHashString szLocalName;
	szLocalName.Clear();
	rkPacket >> dwPeriod >> dwLicenseDate >> dwLocalDate >> szLocalName;
	
	char chTemp[256] = {0,};
	sprintf_s( chTemp, "License Date : %d, Local Date : %d, Period : %d, Local : %s", dwLicenseDate, dwLocalDate, dwPeriod, szLocalName.c_str() );
	
	MessageBox( chTemp, _T("Warning!!"), MB_OK );
}

//국가별 사용자를 카운팅하기 위하여 사용할 함수입니다.
void CZoneDialog::SetUserEachOfCountry(int iSvrType, int iNum, char* cName, SERVERINFO* svrInfo, const ServerStates iSvrState)
{
	CString strToken;
	strToken.Format("%s", cName);
	if(iNum != 0)
		CStringAddINT(iNum+1, &strToken);

	svrInfo->serverType = iSvrType;
	svrInfo->serviceName = strToken;
	svrInfo->servicePort = CCU_PORT;
	InsertServer(svrInfo, iSvrState);
	AddGameServerInfo(svrInfo);
}


void CZoneDialog::CStringAddINT(int iData, CString* TStr)
{
	CString strToken;
	strToken.Format("%d", iData);
	*TStr += strToken;
}


void CZoneDialog::OnBillingUserInfo(SP2Packet &rkPacket)
{
	////////////////////////////////////////////////////////////
	//빌링서버 유저인포 카운트 갱신 -> 그래프
	////////////////////////////////////////////////////////////
	int iUserCount = 0;
	rkPacket >> iUserCount;

	m_CurrentBillingUser = m_CurrentBillingUser + iUserCount;	//빌링서버에서 받은 유저인포 카운트 저장
	
	++m_billingLOGReceiveCount;

	if(m_billingLOGReceiveCount >= GetBillingConnectCount())	//패킷받은 갯수와 현재 연결중인 서버갯수 비교(중간에 빌링서버 끊어졌을 경우 있음)
	{
		UpdateBillingGraph ();
		
	}
	return;	
}

void CZoneDialog::OnBillingReloadINIStatus(SP2Packet &rkPacket)
{
	int billingKeyIndex = 0;
	BOOL reloadINI = FALSE;
	
	rkPacket >> billingKeyIndex;
	rkPacket >> reloadINI;

	BILLINGSERVERINFO* info = GetBillingServerInfo(billingKeyIndex);
	if( info )
	{
		UpdateBillingReloadStatus(info->serverListIndex, reloadINI);
	}
}

void CZoneDialog::OnServerDisconnect(SP2Packet &rkPacket)
{
	ioHashString serverIP;
	int serverPort;

	rkPacket >> serverIP >> serverPort;

	SERVERINFO *serverInfo = GetGameServerInfo(serverIP, serverPort);
	if(!serverInfo) return;

	serverInfo->connect = 0;
	serverInfo->roomCount = 0;
	serverInfo->plazaCount = 0;
	serverInfo->battleRoomCount = 0;
	UpdateServer(serverInfo, SERVERSTATE_OFF);
	CreateServerDownLog(serverInfo);
}

void CZoneDialog::OnLoadCS3File(SP2Packet &rkPacket)
{
	int resultType = 0;
	int version    = 0;
	ioHashString serverIP;
	int serverPort = 0;

	rkPacket >> resultType;
	rkPacket >> serverIP;
	rkPacket >> serverPort;
	rkPacket >> version;

	if(LOAD_CS3_FILE_SUCCESS == resultType)
	{
		m_operatorDlg->GetCSDlg()->Log( "Load Success : %s:%d....................<Success>", serverIP.c_str(), serverPort );
	}
	else
	{
		switch(resultType)
		{
			case LOAD_CS3_FILE_NOT_FILE :
				{
					m_operatorDlg->GetCSDlg()->Log( "There is no file: %s:%d....................<Fail>", serverIP.c_str(), serverPort );
				}
				break;

			case LOAD_CS3_FILE_WRONG_VERSION :
				{
					m_operatorDlg->GetCSDlg()->Log( "Dismatch file vesion : %s:%d[Ver:%d]....................<Fail>", serverIP.c_str(), serverPort, version );
				}
				break;

			case LOAD_CS3_FILE_CHANGE_WRONG :
				{
					m_operatorDlg->GetCSDlg()->Log( "The version is wrong: %s:%d....................<Fail>", serverIP.c_str(), serverPort );
				}
				break;

			case LOAD_CS3_FILE_EXIST_VERSION :
				{
					m_operatorDlg->GetCSDlg()->Log( "Already exist version: %s:%d....................<Fail>", serverIP.c_str(), serverPort );
				}
				break;

			default:
				{
					m_operatorDlg->GetCSDlg()->Log( "Load failed: %s:%d....................<Fail>", serverIP.c_str(), serverPort );
				}
				break;
		}
	}
}

void CZoneDialog::OnCS3FileVersion(SP2Packet &rkPacket)
{
	ioHashString serverIP;
	int serverPort = 0;

	rkPacket >> serverIP;
	rkPacket >> serverPort;

	enum { MAX_VERSION_SIZE = 6, };
	for (int i = 0; i < MAX_VERSION_SIZE ; i++)
	{
		int version = 0;
		rkPacket >> version;
			
		CString szText;
		if( i == MAX_VERSION_SIZE-1)
		{
			szText.Format( "Info Success : %s:%d...............(%d)", serverIP.c_str(), serverPort, version );
		}
		else
		{
			szText.Format( "Info Success : %s:%d..........[%d]", serverIP.c_str(), serverPort, version );
		}

		m_operatorDlg->GetCSDlg()->Log( szText );
	}
	m_operatorDlg->GetCSDlg()->Log( "" );
}


void CZoneDialog::OnUpdateClientVersion(SP2Packet &rkPacket)
{
	ioHashString serverIP;
	int serverPort = 0;
	int version    = 0;

	rkPacket >> serverIP;
	rkPacket >> serverPort;
	rkPacket >> version;
			
	CString szText;
	szText.Format( "%s : %d..(%d)", serverIP.c_str(), serverPort, version );
	m_operatorDlg->GetCSDlg()->Log( szText );
}


void CZoneDialog::OnServerInfoAck(SP2Packet &rkPacket)
{
	int serverType = 0;
	rkPacket >> serverType;

	CString text;
	TCHAR CONNECTED[] = _T("CONNECTED"), NOTCONNECTED[] = _T("NOT CONNECTED");
	TCHAR COMPLETED[] = _T("COMPLETED"), NOTCOMPLETED[] = _T("NOT COMPLETED");

	if(serverType == 1) // 메인서버
	{
		MAINSERVERINFO info;
		rkPacket >> info;

		text.Format(
			"GLOBAL TIME : %d\r\n"
			"SERVER IP: %s:%s PORT: %d\r\n"
			"THREAD COUNT: %d\r\n"
			"JOIN SERVER: %d\r\n"
			"* GAME DBA : %s:%d(%s)\r\n"
			"* LOG DBA : %s:%d(%s)\r\n"
			"-------------------------------------------------------------------\r\n"
			"REMAINDER USER MEMPOOL: %d\r\n"
			"RECV PACKET: %d(64), %d(256), %d(1024), %d(big)\r\n"
			"REMAINDER LOGIC MEMPOOL: %d(64), %d(256), %d(1024), %d(big)\r\n"
			"LOG USING : %d/%d, REMAIN : %d, DROP : %d\r\n"
			"MAX GUILD: %d - MAX UPDATE GUILD: %d\r\n"
			"CAMP BATTLE : %s\r\n"
			"MAX TRADEITEM: %d\r\n"
			"EVENT SHOP : %s(%d Save Reserve)\r\n"
			"--------------------------------------------------------------------\r\n"
			"MAX TOOL CONNECT : %d\r\n"
			"CLIENT VERSION: %d:%d\r\n"
			"MAIN SERVER VERSION: %s | %s\r\n",
			info.dwGlobalTime, 
			info.szPublicIP, info.szPrivateIP, info.iPort, 
			info.ThreadCount, 
			info.JoinServerCount,
			info.szDBAgentIP, info.DBAgentPort, (info.bDBAConnected ? CONNECTED : NOTCONNECTED),
			info.szLogDBAgentIP, info.LogDBAgentPort, (info.bLogDBAConnected ? CONNECTED : NOTCONNECTED),
			info.RemainderMemPoolCount,
			info.RecvQueuePacketCount[0], info.RecvQueuePacketCount[1], info.RecvQueuePacketCount[2], info.RecvQueuePacketCount[3],
			info.RecvQueueRemainderCount[0], info.RecvQueueRemainderCount[1], info.RecvQueueRemainderCount[2], info.RecvQueueRemainderCount[3],
			info.usingLogCount, info.maxUsingLogCount, info.remainLogCount, info.dropLogCount,
			info.MaxGuildCount, info.MaxUpdateGuild,
			info.szCampStringHelp,
			info.MaxTradeItemCount,
			info.szEventShopState, info.EventGoodsSaveDataCount,
			info.MaxToolConnectCount,
			(int)info.bUseClientVersion, info.iClientVersion,
			info.szMainServerVersion, info.szMainServerName);

		CString title = _T("MAIN SERVER");
		ShowInfo(SERVER_TYPE_MAIN, 0, title, text);
	}
	else if(serverType == 2) // 게임서버
	{
		GAMESERVERINFO info;
		rkPacket >> info;
		int relayCount = 0;
		rkPacket >> relayCount;
		std::vector<SendRelayInfo_> relayInfos;
		for(int i=0; i<relayCount; ++i)
		{
			SendRelayInfo_ stTmp;
			rkPacket >> stTmp;
			relayInfos.push_back(stTmp);
		}
		CString title = _T("GAME SERVER");

		text.Format(
			"GLOBAL TIME : %d\r\n"
			"SERVERIP:%s CSPORT:%d SSPORT:%d MSPORT:%d\r\n"
			"GAME SERVER VERSION: %s | %s\r\n"
			"GAME SERVER ID: %s\r\n"
			"GAME SERVER NAME: %s\r\n"
			"CLIENT VERSION: %d:%d\r\n"
			"CHANNEL CREATE: %d(%d) : %d Copy Channel\r\n"
			"MEMO %d\r\n"
			"SERVER CONNECT %d : [%d](%d)\r\n"
			"THREAD COUNT: %d\r\n"
			"JOIN CLIENT: %d(%d) Client\r\n"
			"* GAME DBA : %d/%d(%s)\r\n"
			"* LOG DBA  : %s:%d(%s)\r\n"
			"* MAIN SERVER : %s:%d(%s)\r\n"
			"* BILLING SERVER : %s:%d(%s)\r\n"
			"* RELAY SERVER Connect : %d\r\n"
			"---------------------------------------------------------------------------\r\n"
			"REMAINDER USER MEMPOOL: %d\r\n"
			"ROOM CREATE: %d(%d:%d) - %d(%d:%d) ROOM\r\n"
			"REMAINDER ROOM MEMPOOL: %d\r\n"
			"BATTLEROOM CREATE: %d(%d) ROOM LimitMinRoom(%d)\r\n"
			"REMAINDER BATTLEROOM MEMPOOL: %d\r\n"
			"LADDERTEAM CREATE: %d(%d) - IsBattle(%d)\r\n"
			"REMAINDER MEMPOOL: %d LadderTeamMemCount\r\n"
			"RECV PACKET: %d(64) %d(256) %d(1024) %d(big) %d(udp)\r\n"
			"REMAINDER PACKET MEMPOOL: %d(64) %d(256) %d(1024) %d(big) %d(udp)\r\n"
			"SENDBUFFER USING : %d/%d , REMAIN : %d\r\n"
			"LOG USING : %d/%d, REMAIN : %d, DROP : %d\r\n"
			"HACKCHECK : Min:%d, Max:%d, Less:%d, Over:%d, LessOver:%d, Total:%d\r\n"
			"---------------------------------------------------------------------------\r\n"
			"UDP TRANSFER COUNT : %I64d(%I64d:%I64d)\r\n"
			"XTRAP CS3 VERSION %s\r\n"
			"EXPIRATION DATES : %d\r\n",
			info.dwGlobalTime,
			info.szPublicIP, info.csPort, info.SSPort, info.MSPort,
			info.szSTRFILEVER, info.szSTRINTERNALNAME,
			info.szGameServerID,
			info.szGameServerName,
			info.IsClientVersion, info.GetClientVersion,
			info.ChannelNodeSize, info.ChannelRemainderSize, info.ChannelCopyNodeSize,
			info.MemoNodeSize,
			info.GetServerIndex, info.ServerNodeSize, info.ServerRemainderNodeSize,
			info.ThreadCount,
			info.NodeSize, info.CopyNodeSize,
			info.DBClientNodeSize, info.DBClientTotalNodeSize, ((info.DBClientNodeSize==info.DBClientTotalNodeSize) ? COMPLETED : NOTCOMPLETED),
			info.szLogDBIP, info.LogDBPort, (info.IsMainServerActive ? CONNECTED : NOTCONNECTED),
			info.MainServerIP, info.MainServerPort, (info.IsMainServerActive ? CONNECTED : NOTCONNECTED),
			info.BillingIP, info.BillingPort, (info.IsBillingRelayServerActive ? CONNECTED :NOTCONNECTED),
			relayCount,

			info.RemainderNode,
			info.RoomNodeSize, info.PlazaNodeSize, info.HeapQuartersNodeSize, info.CopyRoomNodeSize, info.CopyPlazaNodeSize, info.CopyHeapQuartersNodeSize,
			info.RommRemainderNode,
			info.BattleRoomNodeSize, info.BattleRoomCopyNodeSize, info.PartyLevelCheckMinRoom,
			info.BattleRoomRemainderNode,
			info.LadderTeamNodeSize, info.LadderTeamCopyNodeSize, info.LadderTeamCampBattlePlay,
			info.LadderTeamRemainderNode,
			info.RecvQueueNodeSize[ 0 ], info.RecvQueueNodeSize[ 1 ], info.RecvQueueNodeSize[ 2 ], info.RecvQueueNodeSize[ 3 ], info.BroadCastUDPnRelayNodeSize,
			info.RecvQueueRemainderNodeSize[ 0 ], info.RecvQueueRemainderNodeSize[ 1 ], info.RecvQueueRemainderNodeSize[ 2 ], info.RecvQueueRemainderNodeSize[ 3 ], info.BroadCastUDPnRelayNodeSize,
			info.sendBufferUsingCnt, info.sendBufferMaxCnt, info.sendBufferRemainCnt,
			info.usingLogCount, info.maxUsingLogCount, info.remainLogCount, info.dropLogCount,
			info.HackCheckMin, info.HackCheckMax, info.HackCheckLess, info.HackCheckOver, info.HackCheckLessOver, info.HackCheckTotal,
			
			info.UDPTransferCount, info.UDPTransferTCPCount, info.UDPTransferTCPSendCount,
			info.XtrapVersion,
			(info.LicenseDate) ? info.LicenseDate : 0);
		text.Append("---------------------------------------------------------------------------\r\n");
		//relay server
		for(int i=0; i<(int)relayInfos.size(); ++i)
		{
			CString ctmp;
			ctmp.Format("RS(%s:%d)::User:[%04d]Room[%04d]UD:[%d]:[%d]:[%d] UU:[%d]:[%d]:[%d]\r\n",
				relayInfos[i].m_ipAddr,relayInfos[i].m_port,
				relayInfos[i].m_userCount,
				relayInfos[i].m_roomCount,
				relayInfos[i].m_64DropCount,relayInfos[i].m_256DropCount,relayInfos[i].m_1024DropCount,
				relayInfos[i].m_64UsingCount,relayInfos[i].m_256UsingCount,relayInfos[i].m_1024UsingCount);
			text.Append(ctmp);
		}
		if( info.m_bReserveLogout )
		{
			TCHAR temp[512];
			sprintf_s( temp, sizeof(temp), "EXIT AFTER %03d S\r\n", info.remainSecond );
			text.Append( temp );
		}



		ShowInfo(SERVER_TYPE_GAME, m_selectedItem, title, text);
	}
	
	else if(serverType == BILLING_ANSWER)
	{
		OnAnswerBillingRequest( rkPacket );
	}
}

void CZoneDialog::OnAnswerBillingRequest(SP2Packet &rkPacket)
{
	int serverType = 0;
	CString text;
	TCHAR CONNECTED[] = _T("CONNECTED"), NOTCONNECTED[] = _T("DISCONNECTED");
	TCHAR COMPLETED[] = _T("COMPLETED"), NOTCOMPLETED[] = _T("NOT COMPLETED");

	int nBillingIndex = 0, nPort = 0, handleCount = 0, nodeSize = 0, destroynodesize = 0 ,
		remaindernode = 0, usingCount0 = 0, usingCount1 = 0, usingCount2 = 0, usingCount3 = 0 ,
		remainCount0 = 0, remainCount1 = 0, remainCount2 = 0, remainCount3 = 0 ,
		threadNodeSize = 0, threadremainderNode = 0, threadActiveCount = 0, threadGetCount = 0,
		GetMemoryPoolCount = 0, GetUserInfoCount = 0;
	
	DWORD dwTime = 0; 
	BOOL bLogDB, bMgameBilling, bWemadeBuyServer, bToonilande, bNexon;
	ioHashString strFileVersion, strFileName, publicIP;
	

	rkPacket >> nBillingIndex; 
	rkPacket >> dwTime; 
	rkPacket >> publicIP;
	rkPacket >> nPort;
	rkPacket >> handleCount;
	rkPacket >> nodeSize;
	rkPacket >> destroynodesize;
	rkPacket >> remaindernode;
	rkPacket >> usingCount0;
	rkPacket >> usingCount1;
	rkPacket >> usingCount2;
	rkPacket >> usingCount3;
	rkPacket >> remainCount0;
	rkPacket >> remainCount1;
	rkPacket >> remainCount2;
	rkPacket >> remainCount3;
	rkPacket >> bLogDB;
	rkPacket >> bMgameBilling;
	rkPacket >> bWemadeBuyServer;
	rkPacket >> bToonilande;
	rkPacket >> bNexon;
	rkPacket >> GetMemoryPoolCount;
	rkPacket >> GetUserInfoCount;
	rkPacket >> threadNodeSize;
	rkPacket >> threadremainderNode;
	rkPacket >> threadActiveCount;
	rkPacket >> threadGetCount;
	rkPacket >> strFileVersion;
	rkPacket >> strFileName;
		
	text.Format(
		"GLOBAL TIME : %d\r\n"
		"SERVER IP: %s PORT: %d\r\n"
		"THREAD COUNT: %d\r\n"
		"JOIN SERVER: %d(%d)\r\n"
		"-------------------------------------------------------------------\r\n"
		"REMAINDER USER MEMPOOL: %d\r\n"
		"RECV PACKET: %d:%d:%d:%d QUEUE\r\n"
		"REMAINDER MEMPOOL: %d:%d:%d:%d PacketMemCount\r\n"
		"LOG DB SERVER : %s\r\n"
		"MGAME BILLING SERVER : %s\r\n"
		"WE MADE BUY SERVER : %s\r\n"
		"TOONILANDE BILLING SERVER : %s\r\n"
		"NEXON SESSION SERVER : %s\r\n"
		"UserInfo memPool: %d / %d\r\n"
		"--------------------------------------------------------------------\r\n"
		"DATA POOL : %d(%d)\r\n"
		"ACTIVE THREAD: %d/%d\r\n"
		"VERSION: %s | %s\r\n",
		dwTime, 
		publicIP.c_str(), 
		nPort, 
		handleCount, 
		nodeSize, 
		destroynodesize, 
		remaindernode, 
		usingCount0, 
		usingCount1, 
		usingCount2, 
		usingCount3, 
		remainCount0, 
		remainCount1, 
		remainCount2, 
		remainCount3, 
		(bLogDB ? CONNECTED : NOTCONNECTED),
		(bMgameBilling ? CONNECTED : NOTCONNECTED),
		(bWemadeBuyServer ? CONNECTED : NOTCONNECTED),
		(bToonilande ? CONNECTED : NOTCONNECTED),
		(bNexon ? CONNECTED : NOTCONNECTED),
		GetMemoryPoolCount,
		GetUserInfoCount, 
		threadNodeSize,
		threadremainderNode, 
		threadActiveCount, 
		threadGetCount, 
		strFileVersion.c_str(), 
		strFileName.c_str()
		);
	
	BILLINGSERVERINFO* serverInfo =  GetBillingServerInfo(nBillingIndex);
	if(serverInfo)
	{
		CString title, sPort;
		sPort.Format(_T("%s:%d"), serverInfo->serviceIP, serverInfo->servicePort);
		title	= _T("BILLING SERVER_") + sPort;
		ShowInfo((ServerTypes)nBillingIndex, 0, title, text);
	}
}

void CZoneDialog::OnAdminResponse(SP2Packet &rkPacket)
{
	int type = 0;
	rkPacket >> type;

	switch(type)
	{
	case ADMINCOMMAND_MTOOL_AUTH :
		{
			int level = 0;
			bool result = false;
			rkPacket >> result >> level;

			if(ADMIN_LEVEL_NONE == level)
			{
				MessageBox(_T("LostSaga authentication failed"), _T("Sorry"), MB_OK);
				exit(1);
			}
			else
			{
				m_level	= level;
				//m_operatorDlg->;(SW_SHOW);
			}
			TRACE("OnAdminResponse : ADMINCOMMAND_MTOOL_AUTH(%d)\n", result);
		}
		break;
	case ADMINCOMMAND_MTOOL_USERBLOCK:
		{
			ioHashString ioUserId;
			LONG success;
			rkPacket >> ioUserId >> success;

			if(m_file)
			{
				SYSTEMTIME sysTime;
				GetLocalTime(&sysTime);

				if( success == 1)
				{
					m_file->WriteFormat(_T("[%04lu-%02lu-%02lu %02lu:%02lu:%02lu] Userblock Success : %20s\n"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, ioUserId.c_str() );

					WriteLogDlg( _T("[Block] Success : %s"), ioUserId.c_str() );
				}
				else
				{
					m_file->WriteFormat(_T("[%04lu-%02lu-%02lu %02lu:%02lu:%02lu] Userblock Failed : %20s\n"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, ioUserId.c_str() );
					
					m_operatorDlg->ShowLogTab();
					WriteLogDlg( _T("[Block] Failed : %s"), ioUserId.c_str() );
				}
			}
			else
			{
				if( success )
				{
					WriteLogDlg( _T("[Block] Success : %s"), ioUserId.c_str() );
				}
				else
				{
					m_operatorDlg->ShowLogTab();
					WriteLogDlg( _T("[Block] Failed : %s"), ioUserId.c_str() );
				}
			}
		}
		break;
	}
}

void CZoneDialog::OnLoginInfoAck(SP2Packet &rkPacket)
{
	CString text;
	int max;
	int userCount;
	int sendCount;
	int serverId;
	int serverState;
	LoginServerInfo loginInfo;

	rkPacket >> loginInfo;
	rkPacket >> max;

	text.Format(
		"LOGIN SERVER(%s:%d)\r\n"
		"ACCEPT COUNT : %d(%d)\r\n"
		"CLOSE COUNT : %d\r\n"
		"CLIENT POOL COUNT : %d\r\n"
		"OP POOL COUNT : %d\r\n"
		"PACKET QUEUE COUNT : %d\r\n"
		"SERVER CONNECTION POOL : %d\r\n"
		"SERVER CONNECTION COUNT : %d\r\n"
		"USERINFO COUNT : %d\r\n",
		loginInfo.serverIP, loginInfo.serverPort,
		loginInfo.acceptCountPerSec, loginInfo.acceptCount,
		loginInfo.closeCount,
		loginInfo.clientPoolCount,
		loginInfo.opPoolCount,
		loginInfo.packetQueueCount,
		loginInfo.serverConnectorPoolCount,
		loginInfo.serverConnectCount,
		loginInfo.userInfoCount);

	for(int i=0; i<max; ++i)
	{
		TCHAR temp[512];
		rkPacket >> serverId >> userCount >> sendCount >> serverState;

		sprintf_s(temp, sizeof(temp), 
			"SERVER(%d)::USERCOUNT:%d::SENDCOUNT::%d STATE::%x\r\n",
			serverId, userCount, sendCount, serverState);

		text.Append(temp);
	}

	CString title = _T("LOGIN SERVER");
	ShowInfo(SERVER_TYPE_LOGIN, 0, title, text);
}

void CZoneDialog::OnLoginServerInfo(SP2Packet& rkPacket)
{
	CString tmp;
	int maxUser;
	int maxServer;
	int waitUser;
	int maxWait;
	int allBlock;
	int serverState;
	TCHAR serverIP[64];

	rkPacket >> maxUser;
	rkPacket >> maxServer;
	rkPacket >> waitUser;
	rkPacket >> maxWait;
	rkPacket >> allBlock;
	rkPacket >> serverIP;
	rkPacket >> serverState;

	SERVERINFO* pkServerInfo = GetOtherServerInfo(SERVER_TYPE_LOGIN);
	if(false == strcmp(serverIP, ""))
	{
		return ;
	}

	CString strServerIP = m_loginDlg->GetServerIP();
	if(true == strcmp(strServerIP, ""))
	{
		if(true == strcmp(serverIP, strServerIP))
		{
			return ;
		}
	}

	m_loginDlg->Startup(m_title, maxUser, maxServer, 8, this);
	m_loginDlg->SetServerIP(serverIP);
	for(int j = 0; j < maxServer ; j++)
	{
		ServerInfo serverInfo;

		rkPacket >> serverInfo.serverId;
		rkPacket >> serverInfo.serverName;
		rkPacket >> serverInfo.blockState;
		rkPacket >> serverInfo.serverIP;
		rkPacket >> serverInfo.serverPort;

		serverInfo.fullCount = maxUser;
		serverInfo.userCount = 0;
		serverInfo.sendCount = 0;
		serverInfo.serverState = -1;

		m_loginDlg->AddPanel(
			serverInfo.serverId, 
			serverInfo.serverName, 
			serverInfo.serverIP, 
			serverInfo.serverPort, 
			serverInfo.blockState);
	}

	if(SERVERSTATE_OFF == (ServerStates)serverState)
	{
		OnClose((ServerTypes)SERVER_TYPE_LOGIN, 0, 0);
	}
	else
	{
		UpdateServer(GetOtherServerInfo((ServerTypes)SERVER_TYPE_LOGIN), (ServerStates)serverState);
	}
}

void CZoneDialog::OnLoginServerRefresh(SP2Packet & rkPacket)
{
	int maxUser;
	int maxServer;
	int waitUser;
	int maxWait;
	TCHAR serverIP[64];

	rkPacket >> maxUser;
	rkPacket >> maxServer;
	rkPacket >> waitUser;
	rkPacket >> maxWait;
	rkPacket >> serverIP;

	m_loginDlg->UpdateBegin();
	int CCU = 0;
	for(int i=0; i < maxServer; i++)
	{
		ServerInfo serverInfo;

		rkPacket >> serverInfo.serverId;
		rkPacket >> serverInfo.userCount;
		rkPacket >> serverInfo.sendCount;
		rkPacket >> serverInfo.serverState;

		TRACE("상태변경 %d : %d\n", serverInfo.serverId, serverInfo.serverState);
		m_loginDlg->UpdatePanel(
			serverInfo.serverId, 
			serverInfo.userCount, 
			serverInfo.sendCount, 
			serverInfo.serverState);

		CCU += serverInfo.userCount;
	}

	m_loginDlg->UpdateEnd(CCU);
}

void CZoneDialog::OnLoginServerBlock(SP2Packet& rkPacket)
{
	int serverId, blockState;

	rkPacket >> serverId;
	rkPacket >> blockState;
}

void CZoneDialog::OnLSResponse(SP2Packet& rkPacket )
{
	int type;
	rkPacket >> type;

	switch(type)
	{
	case EMCONTROLTYPE::LS_GETSERVERINFO:
		{
			OnLoginServerInfo(rkPacket);
		}
		break;
	case EMCONTROLTYPE::LS_FILLSERVERINFO:
		{
			OnLoginServerRefresh(rkPacket);
		}
		break;
	case EMCONTROLTYPE::LS_SETSERVERBLOCK:
		{
			OnLoginServerBlock(rkPacket);
		}
		break;
	case EMCONTROLTYPE::LS_SETALLSERVERBLOCK:
		{
			//TCHAR serverIP[STR_IP_MAX];
			//int blockstate;
			//rkPacket >> serverIP;
			//rkPacket >> bloc*kstate;
			//for(int i=0; i<m_vLoginPanel.size();++i)
			//{
			//	if(strcmp(m_vLoginPanel[i]->serverIP.c_str(),serverIP) == 0)
			//	{
			//		m_vLoginPanel[i]->PostMessageA(WM_LSLOGINSERVER,type,blockstate);
			//	}
			//}

		}
		break;
	case EMCONTROLTYPE::LS_FILLINFODRAW:
		{
			OnLoginInfoAck(rkPacket);
		}
		break;
	}
}

void CZoneDialog::OnConnectState(SP2Packet &rkPacket)
{
	CString tmp;
	int ServerType;
	int ServerState;
	int waitUser;
	int maxWait;
	int allBlock;
	TCHAR serverIP[64];

	rkPacket >> ServerType;
	rkPacket >> ServerState;

	if(SERVERSTATE_OFF == ServerState)
	{
		OnClose((ServerTypes)ServerType, 0, 0);
	}
	else
	{
		UpdateServer(GetOtherServerInfo((ServerTypes)ServerType), (ServerStates)ServerState);
	}
}

void CZoneDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	UPDATE_EASYSIZE;

	if(m_countGraph.GetSafeHwnd() != NULL)
	{
		CRect client;
		GetClientRect(client);


		CRect rc;
		m_countGraph.GetClientRect(rc);

		int left = client.left + 12;
		int top = client.bottom - rc.Height() - 15;
		m_countGraph.SetWindowPos(NULL, left, top, 0, 0, SWP_NOSIZE);
	}
	
	if(m_billingFlag == TRUE)
	{
		if(m_countGraph.GetSafeHwnd() != NULL)
		{
			/*CRect client;
			GetClientRect(client);


			CRect rc;
			m_services[0].GetClientRect(rc);

			CRect rc_graph;
			m_countGraph.GetClientRect(rc_graph);

			
			int left = client.left;
			int top = client.top;

			
			m_services[0].SetWindowPos(NULL, left + 12, top + 15, cx - 30, cy - rc_graph.Height() - 60, SWP_NOZORDER);*/
			CRect client;
			GetClientRect(client);


			CRect rc;
			m_services[1].GetClientRect(rc);

			CRect rc_graph;
			m_countGraph.GetClientRect(rc_graph);

			
			int left = client.left;
			int top = client.top;

			
			m_services[1].SetWindowPos(NULL, left + 12, top + 15, cx - 30, cy - rc_graph.Height() - 60, SWP_NOZORDER);

		}
	}
}

void CZoneDialog::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL CZoneDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(RestrictMessage(pMsg))
		return FALSE;

	ShowGraph(pMsg);
	
	return CDialogEx::PreTranslateMessage(pMsg);
}

//EditBox 클릭시 이벤트
void CZoneDialog::OnLvnItemchangedListGamedemons(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	DWORD dwCurrentTime = GetCurrentTime();
// 	if(bClicked && (m_dwTime+2000 > dwCurrentTime))
// 		return;
// 	if(m_strCloseSvrName != _T(""))
// 		return;
// 	int iIndex = m_services[1].GetSelectionMark();			//게임서버의 인덱스 찾기
// 	if(iIndex != -1)
// 	{
// 		CString StrSvcName = m_services[1].GetItemText(iIndex,0); //이름을 가져온다
// 		if(m_strCloseSvrName == StrSvcName)
// 			return;
// 		if(StrSvcName.Find("#") != -1) //find the server name
// 		{
// 			if(!bClicked && !bClosed)
// 			{
// 				bClicked = true;
// 				SetCloseCCU(&StrSvcName);
// 				m_dwTime	= GetCurrentTime();
// 			}
// 			else
// 				return;
// 		}
// 	}

	*pResult = 0;
}

void CZoneDialog::MoveToItemData( int iSvrNum, int iSour, int iDest )
{

}



