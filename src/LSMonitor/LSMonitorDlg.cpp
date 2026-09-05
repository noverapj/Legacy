// LSMonitorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "LSMonitorDlg.h"
#include "SMS/ioINILoaderAU.h"
#include <strsafe.h>
#include "TCPSocketTable.h"
#include "NetWork/SystemInfoUDPMsg.h"
#include "Version.h"
#include ".\lsmonitordlg.h"
#include "local/iolocalmanager.h"
#include "LoingPanel.h"
#include "HttpApp.h"
#include "LS_ServerInfoDlg.h"

#include "ServerInfoDlg.h"
#include "NagleRefCountDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//------------------------------------------------------------------------------------------
HttpConnector::HttpConnector()
{
	m_sURL.Empty();
	m_bClose   = false;
}

HttpConnector::~HttpConnector()
{
}

int HttpConnector::OpenURL( IN const char *szURL, OUT char *szReturnData, IN int iReturnDataSize )
{
	HttpApp http;
	http.NewSession();

	CString szErrorMsg;
	if( !http.OpenUrl(szURL, szErrorMsg) )
	{
		LOG.PrintTimeAndLog( 0, "%s Error %s", __FUNCTION__, (LPCTSTR)szErrorMsg );
		return -1;
	}

	CInternetFile *pIF = http.GetInternetFile(); 
	if(!pIF)
	{
		http.DeleteSession();
		return -2;
	}

	pIF->Read( szReturnData, iReturnDataSize );

	http.DeleteInternetFile();
	http.DeleteSession();

	return 1;
}

void HttpConnector::SetURL( CString &rsURL )
{
	if( !m_sURL.IsEmpty() )
		return;

	m_sURL =  rsURL;
}

void HttpConnector::Run()
{
	while(1)
	{
		if( m_bClose )
			return;

		if( m_sURL.IsEmpty() )
		{
			Sleep(10);
			continue;
		}

		CString sBackUpURL = m_sURL;

		char szReturnData[MAX_PATH];
		ZeroMemory( szReturnData, sizeof( szReturnData ) );
		if( OpenURL( m_sURL.GetBuffer(0), szReturnData, sizeof( szReturnData ) ) == -2 ) // -2:웹서버 다운상태
		{
			m_sURL.Empty();
			continue;
		}
		m_sURL.Empty();

		if( strcmp( szReturnData, "1") != 0 )
			continue;

		CLSMonitorDlg *pDlg = (CLSMonitorDlg*) AfxGetApp()->GetMainWnd();
		if(pDlg == NULL)
			continue;

		bool bUpdateList = false;
		SYSTEMINFO *pInfo = pDlg->GetSystemInfoByURL( sBackUpURL.GetBuffer(0) , bUpdateList);
		if( !pInfo ) 
			continue;

		pInfo->iCPUUsed       = 0;
		pInfo->iMemoryRemain  = 500;
		pInfo->dbMemoryUsed   = 0;
		pInfo->iCDriveRemain  = 10000;
		pInfo->iDDreiveRemain = 10000;
		pInfo->dwLastRcvTime  = timeGetTime();
		pInfo->bReaction      = true;

		if( bUpdateList )
			pDlg->UpdateSystemListCtrl(pInfo);
		pDlg->SetSystemSMSErrorToNormal( pInfo->wSMSArray );
	}
}

void HttpConnector::Close()
{
	m_bClose = true;
}
//------------------------------------------------------------------------------

// CLSMonitorDlg 대화 상자
CLog  ConnectLog;
CLog  MaxLog;


CLSMonitorDlg::CLSMonitorDlg(CWnd* pParent /*=NULL*/)
	: ETSLayoutDialog(CLSMonitorDlg::IDD, pParent)
{
	m_hIcon        = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iMaxConnect  = 0;
	//m_eDlgType     = DT_NORMAL;
	m_pSMSDlg      = NULL;

	m_selectIndex	= 0;
}

void CLSMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVERINFO, m_ListCtrlServerInfo);
	DDX_Control(pDX, IDC_LIST_SYSTEMINFO, m_ListCtrlSystemInfo);
	DDX_Control(pDX, IDC_TAB_ZONE, m_TabCtrlZone );
}

BEGIN_MESSAGE_MAP(CLSMonitorDlg, ETSLayoutDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_MESSAGE (WM_TCPSOCKET1  , OnTCPSocket1  )
	ON_MESSAGE (WM_TCPSOCKET2  , OnTCPSocket2  )
	ON_MESSAGE (WM_TCPSOCKET3  , OnTCPSocket3  )
	ON_MESSAGE (WM_TCPSOCKET4  , OnTCPSocket4  )
	ON_MESSAGE (WM_TCPSOCKET5  , OnTCPSocket5  )
	ON_MESSAGE (WM_TCPSOCKET6  , OnTCPSocket6  )
	ON_MESSAGE (WM_TCPSOCKET7  , OnTCPSocket7  )
	ON_MESSAGE (WM_TCPSOCKET8  , OnTCPSocket8  )
	ON_MESSAGE (WM_TCPSOCKET9  , OnTCPSocket9  )
	ON_MESSAGE (WM_TCPSOCKET10 , OnTCPSocket10 )
	ON_MESSAGE (WM_TCPSOCKET11 , OnTCPSocket11 )
	ON_MESSAGE (WM_TCPSOCKET12 , OnTCPSocket12 )
	ON_MESSAGE (WM_TCPSOCKET13 , OnTCPSocket13 )
	ON_MESSAGE (WM_TCPSOCKET14 , OnTCPSocket14 )
	ON_MESSAGE (WM_TCPSOCKET15 , OnTCPSocket15 )
	ON_MESSAGE (WM_TCPSOCKET16 , OnTCPSocket16 )
	ON_MESSAGE (WM_TCPSOCKET17 , OnTCPSocket17 )
	ON_MESSAGE (WM_TCPSOCKET18 , OnTCPSocket18 )
	ON_MESSAGE (WM_TCPSOCKET19 , OnTCPSocket19 )
	ON_MESSAGE (WM_TCPSOCKET20 , OnTCPSocket20 )
	ON_MESSAGE (WM_TCPSOCKET21 , OnTCPSocket21 )
	ON_MESSAGE (WM_TCPSOCKET22 , OnTCPSocket22 )
	ON_MESSAGE (WM_TCPSOCKET23 , OnTCPSocket23 )
	ON_MESSAGE (WM_TCPSOCKET24 , OnTCPSocket24 )
	ON_MESSAGE (WM_TCPSOCKET25 , OnTCPSocket25 )
	ON_MESSAGE (WM_TCPSOCKET26 , OnTCPSocket26 )
	ON_MESSAGE (WM_TCPSOCKET27 , OnTCPSocket27 )
	ON_MESSAGE (WM_TCPSOCKET28 , OnTCPSocket28 )
	ON_MESSAGE (WM_TCPSOCKET29 , OnTCPSocket29 )
	ON_MESSAGE (WM_TCPSOCKET30 , OnTCPSocket30 )
	ON_MESSAGE (WM_TCPSOCKET31 , OnTCPSocket31 )
	ON_MESSAGE (WM_TCPSOCKET32 , OnTCPSocket32 )
	ON_MESSAGE (WM_TCPSOCKET33 , OnTCPSocket33 )
	ON_MESSAGE (WM_TCPSOCKET34 , OnTCPSocket34 )
	ON_MESSAGE (WM_TCPSOCKET35 , OnTCPSocket35 )
	ON_MESSAGE (WM_TCPSOCKET36 , OnTCPSocket36 )
	ON_MESSAGE (WM_TCPSOCKET37 , OnTCPSocket37 )
	ON_MESSAGE (WM_TCPSOCKET38 , OnTCPSocket38 )
	ON_MESSAGE (WM_TCPSOCKET39 , OnTCPSocket39 )
	ON_MESSAGE (WM_TCPSOCKET40 , OnTCPSocket40 )
	ON_MESSAGE (WM_TCPSOCKET41 , OnTCPSocket41 )
	ON_MESSAGE (WM_TCPSOCKET42 , OnTCPSocket42 )
	ON_MESSAGE (WM_TCPSOCKET43 , OnTCPSocket43 )
	ON_MESSAGE (WM_TCPSOCKET44 , OnTCPSocket44 )
	ON_MESSAGE (WM_TCPSOCKET45 , OnTCPSocket45 )
	ON_MESSAGE (WM_TCPSOCKET46 , OnTCPSocket46 )
	ON_MESSAGE (WM_TCPSOCKET47 , OnTCPSocket47 )
	ON_MESSAGE (WM_TCPSOCKET48 , OnTCPSocket48 )
	ON_MESSAGE (WM_TCPSOCKET49 , OnTCPSocket49 )
	ON_MESSAGE (WM_TCPSOCKET50 , OnTCPSocket50 )
	ON_MESSAGE (WM_TCPSOCKET51 , OnTCPSocket51 )
	ON_MESSAGE (WM_TCPSOCKET52 , OnTCPSocket52 )
	ON_MESSAGE (WM_TCPSOCKET53 , OnTCPSocket53 )
	ON_MESSAGE (WM_TCPSOCKET54 , OnTCPSocket54 )
	ON_MESSAGE (WM_TCPSOCKET55 , OnTCPSocket55 )
	ON_MESSAGE (WM_TCPSOCKET56 , OnTCPSocket56 )
	ON_MESSAGE (WM_TCPSOCKET57 , OnTCPSocket57 )
	ON_MESSAGE (WM_TCPSOCKET58 , OnTCPSocket58 )
	ON_MESSAGE (WM_TCPSOCKET59 , OnTCPSocket59 )
	ON_MESSAGE (WM_TCPSOCKET60 , OnTCPSocket60 )
	ON_MESSAGE (WM_TCPSOCKET61 , OnTCPSocket61 )
	ON_MESSAGE (WM_TCPSOCKET62 , OnTCPSocket62 )
	ON_MESSAGE (WM_TCPSOCKET63 , OnTCPSocket63 )
	ON_MESSAGE (WM_TCPSOCKET64 , OnTCPSocket64 )
	ON_MESSAGE (WM_TCPSOCKET65 , OnTCPSocket65 )
	ON_MESSAGE (WM_TCPSOCKET66 , OnTCPSocket66 )
	ON_MESSAGE (WM_TCPSOCKET67 , OnTCPSocket67 )
	ON_MESSAGE (WM_TCPSOCKET68 , OnTCPSocket68 )
	ON_MESSAGE (WM_TCPSOCKET69 , OnTCPSocket69 )
	ON_MESSAGE (WM_TCPSOCKET70 , OnTCPSocket70 )
	ON_MESSAGE (WM_TCPSOCKET71 , OnTCPSocket71 )
	ON_MESSAGE (WM_TCPSOCKET72 , OnTCPSocket72 )
	ON_MESSAGE (WM_TCPSOCKET73 , OnTCPSocket73 )
	ON_MESSAGE (WM_TCPSOCKET74 , OnTCPSocket74 )
	ON_MESSAGE (WM_TCPSOCKET75 , OnTCPSocket75 )
	ON_MESSAGE (WM_TCPSOCKET76 , OnTCPSocket76 )
	ON_MESSAGE (WM_TCPSOCKET77 , OnTCPSocket77 )
	ON_MESSAGE (WM_TCPSOCKET78 , OnTCPSocket78 )
	ON_MESSAGE (WM_TCPSOCKET79 , OnTCPSocket79 )
	ON_MESSAGE (WM_TCPSOCKET80 , OnTCPSocket80 )
	ON_MESSAGE (WM_TCPSOCKET81 , OnTCPSocket81 )
	ON_MESSAGE (WM_TCPSOCKET82 , OnTCPSocket82 )
	ON_MESSAGE (WM_TCPSOCKET83 , OnTCPSocket83 )
	ON_MESSAGE (WM_TCPSOCKET84 , OnTCPSocket84 )
	ON_MESSAGE (WM_TCPSOCKET85 , OnTCPSocket85 )
	ON_MESSAGE (WM_TCPSOCKET86 , OnTCPSocket86 )
	ON_MESSAGE (WM_TCPSOCKET87 , OnTCPSocket87 )
	ON_MESSAGE (WM_TCPSOCKET88 , OnTCPSocket88 )
	ON_MESSAGE (WM_TCPSOCKET89 , OnTCPSocket89 )
	ON_MESSAGE (WM_TCPSOCKET90 , OnTCPSocket90 )
	ON_MESSAGE (WM_TCPSOCKET91 , OnTCPSocket91 )
	ON_MESSAGE (WM_TCPSOCKET92 , OnTCPSocket92 )
	ON_MESSAGE (WM_TCPSOCKET93 , OnTCPSocket93 )
	ON_MESSAGE (WM_TCPSOCKET94 , OnTCPSocket94 )
	ON_MESSAGE (WM_TCPSOCKET95 , OnTCPSocket95 )
	ON_MESSAGE (WM_TCPSOCKET96 , OnTCPSocket96 )
	ON_MESSAGE (WM_TCPSOCKET97 , OnTCPSocket97 )
	ON_MESSAGE (WM_TCPSOCKET98 , OnTCPSocket98 )
	ON_MESSAGE (WM_TCPSOCKET99 , OnTCPSocket99 )
	ON_MESSAGE (WM_TCPSOCKET100, OnTCPSocket100)
	ON_MESSAGE(WM_UDPSOCKET, OnUDPSocket)
	ON_MESSAGE(WM_DISCONNETED,OnDisConnected)
	ON_MESSAGE(WM_SENDLOGINSERVER,&CLSMonitorDlg::SendPacket2LoginServer)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SERVERINFO, OnNMCustomdrawListServerInfo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_SYSTEMINFO, OnNMCustomdrawListSysteminfo)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_ZONE, OnTabSelChangeZone)
//	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_BTN_ANNOUNCE, OnBnClickedBtnAnnounce)
	ON_BN_CLICKED(ID_BTN_SYSTEM_SMS, OnBnClickedBtnSystemSms)
	ON_BN_CLICKED(ID_BTN_SERVER_SMS, OnBnClickedBtnServerSms)
	ON_BN_CLICKED(ID_BTN_VERSION, OnBnClickedButtonVersion)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedDefaultCancel) // esc 종료 하지 못하게
	ON_BN_CLICKED(ID_BTN_CONTROL, OnBnClickedButtonControl)
	ON_BN_CLICKED(ID_BTN_CONNECT_LOG, OnBnClickedBtnConnectLog)
	ON_NOTIFY( NM_RCLICK, IDC_LIST_SERVERINFO, OnNMRClickServerInfo )
	ON_NOTIFY( NM_CLICK, IDC_LIST_SERVERINFO, OnNMClickServerInfo )
	ON_NOTIFY( NM_DBLCLK, IDC_LIST_SERVERINFO, OnNMDBClickServerInfo )
	ON_COMMAND(ID_MAINSERVER_INFODRAW, OnMainServerInfoDrawReq)
	ON_COMMAND(ID_MAINSERVER_MAINSERVEREXIT, &CLSMonitorDlg::OnMainserverMainserverexit)
	ON_COMMAND(ID_GAMESERVER_INFODRAW, &CLSMonitorDlg::OnGameserverInfodraw)
	ON_COMMAND(ID_MAINSERVER_QUICKALLEXIT, &CLSMonitorDlg::OnMainserverQuickallexit)
	ON_COMMAND(ID_MAINSERVER_SAFETYALLEXIT, &CLSMonitorDlg::OnMainserverSafetyallexit)
	ON_COMMAND(ID_MAINSERVER_RELOADCLOSEINFO, &CLSMonitorDlg::OnMainserverReloadcloseinfo)
	ON_COMMAND(ID_MAINSERVER_DBAGENTEXTEND, &CLSMonitorDlg::OnMainserverDbagentextend)
	ON_COMMAND(ID_MAINSERVER_GAMESERVEROPTION, &CLSMonitorDlg::OnMainserverGameserveroption)
	ON_COMMAND(ID_MAINSERVER_GSRELOADINI, &CLSMonitorDlg::OnMainserverGsreloadini)
	ON_COMMAND(ID_MAINSERVER_EXTRAITEMINI, &CLSMonitorDlg::OnMainserverExtraitemini)
	ON_COMMAND(ID_MAINSERVER_EVENTSHOPINI, &CLSMonitorDlg::OnMainserverEventshopini)
	ON_COMMAND(ID_GAMESERVER_RELOADHACKCONSTANT, &CLSMonitorDlg::OnGameserverReloadhackconstant)
	ON_COMMAND(ID_GAMESERVER_RELOADUSERDISPERSION, &CLSMonitorDlg::OnGameserverReloaduserdispersion)
	ON_COMMAND(ID_GAMESERVER_RELOADPROCESSINI, &CLSMonitorDlg::OnGameserverReloadprocessini)
	ON_COMMAND(ID_GAMESERVER_RELOADINICLASS, &CLSMonitorDlg::OnGameserverReloadiniclass)
	ON_COMMAND(ID_GAMESERVER_RELOADINIDECO, &CLSMonitorDlg::OnGameserverReloadinideco)
	ON_COMMAND(ID_GAMESERVER_RELOADINIETC, &CLSMonitorDlg::OnGameserverReloadinietc)
	ON_COMMAND(ID_GAMESERVER_RELOADINIEVENT, &CLSMonitorDlg::OnGameserverReloadinievent)
	ON_COMMAND(ID_GAMESERVER_RELOADQUESTINI, &CLSMonitorDlg::OnGameserverReloadquestini)
	ON_COMMAND(ID_GAMESERVER_RELOADMODEINI, &CLSMonitorDlg::OnGameserverReloadmodeini)
	ON_COMMAND(ID_GAMESERVER_RELOADPRESENTINI, &CLSMonitorDlg::OnGameserverReloadpresentini)
	ON_COMMAND(ID_GAMESERVER_RELOADFISHINGINI, &CLSMonitorDlg::OnGameserverReloadfishingini)
	ON_COMMAND(ID_GAMESERVER_RELOADEXCA, &CLSMonitorDlg::OnGameserverReloadexca)
	ON_COMMAND(ID_GAMESERVER_RELOADITEMCOMPOUNDINI, &CLSMonitorDlg::OnGameserverReloaditemcompoundini)
	ON_COMMAND(ID_GAMESERVER_RELOADEXTRAITEMINI, &CLSMonitorDlg::OnGameserverReloadextraitemini)
	ON_COMMAND(ID_GAMESERVER_RELOADTRADEINI, &CLSMonitorDlg::OnGameserverReloadtradeini)
	ON_COMMAND(ID_GAMESERVER_RELOADLEVELMATCHINI, &CLSMonitorDlg::OnGameserverReloadlevelmatchini)
	ON_COMMAND(ID_GAMESERVER_RELOADITEMINITCONTROL, &CLSMonitorDlg::OnGameserverReloaditeminitcontrol)
	ON_COMMAND(ID_GAMESERVER_RELOADCONFIGINI, &CLSMonitorDlg::OnGameserverReloadconfigini)
	ON_COMMAND(ID_GAMESERVER_QUICKEXIT, &CLSMonitorDlg::OnGameserverQuickexit)
	ON_COMMAND(ID_GAMESERVER_SAFETYEXIT, &CLSMonitorDlg::OnGameserverSafetyexit)
	ON_COMMAND(ID_MAINSERVER_GSSETNAGLEREFCOUNT, &CLSMonitorDlg::OnMainserverGssetnaglerefcount)
	ON_COMMAND(ID_MAINSERVER_GSSETNAGLETIME, &CLSMonitorDlg::OnMainserverGssetnagletime)
	ON_COMMAND(ID_LOGINSERVER_LOGINSERVERINFODRAW, &CLSMonitorDlg::OnLoginServerInfodraw)
	ON_COMMAND(ID_LOGINSERVER_LOGINSERVERIN_FILL, &CLSMonitorDlg::OnLoginserverLoginserverinFill)
	END_MESSAGE_MAP()

//ID_INFO_SERVERINFOREQ
// CLSMonitorDlg 메시지 처리기

BOOL CLSMonitorDlg::OnInitDialog()
{
	ETSLayoutDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1); // 사용메모리 최소화

	ZeroMemory( m_szLogPrevTime, sizeof( m_szLogPrevTime ) );
	ZeroMemory( m_szConnectLogPrevTime, sizeof( m_szConnectLogPrevTime ) );
	ZeroMemory( m_szMaxLogPrevTime, sizeof( m_szMaxLogPrevTime ) );

	CString szVersion;
	szVersion.Format( "Version: %s | %s", STRFILEVER, STRINTERNALNAME );
	SetDlgItemText( IDC_STATIC_STRING , (LPCTSTR)szVersion );
	_OnTimerCreateNewLog( LOG, "BUG" , m_szLogPrevTime, sizeof( m_szLogPrevTime ) );

	m_SystemInfoUDPSocket.InitSocket();
	if (m_SystemInfoUDPSocket.InitUDP(GetSafeHwnd(), WM_UDPSOCKET, CreateUDPPort(UDPSOCKET_PORT+1)) == false) // +1은 서프 모니터링과 같이 실행위해
	{
		AfxMessageBox("Fail UDP SOCKET");
		EndDialog(0);
		return FALSE;
	}
	
	SetTimer(MAIN_TIMER, 10000, NULL); 
	SetTimer(LS_SERVER,  2500, NULL); 

	LoadINI();

	SendUDPMsg();

	SendWebServer();
	this->GetSafeHwnd();

	BOOL state = FALSE;
	for( int i = 0 ; i < m_vZoneInfo.size() ; ++i )
	{
		if( m_vZoneInfo[ i ].gradeType != DT_SYSTEM_SMS )
		{
			state = TRUE;
			break;
		}
	}

	if( state )
	{
		if(!g_TCPSocketTable.Init())
		{
			DestroyWindow();
			return FALSE;
		}

		if( !g_TCPSocketTable.AddAndInit( m_hWnd, m_vZoneInfo ) )
		{
			DestroyWindow();
			return FALSE;
		}
		g_TCPSocketTable.AddLoginServer(m_hWnd,m_vloginserverinfo);// kyg추가
	}

	//if( m_eDlgType != DT_SYSTEM_SMS )
	//{
	//	if(!g_TCPSocketTable.Init())
	//	{
	//		DestroyWindow();
	//		return FALSE;
	//	}

	//	if( !g_TCPSocketTable.AddAndInit( m_hWnd, m_vZoneInfo ) )
	//	{
	//		DestroyWindow();
	//		return FALSE;
	//	}
	//	g_TCPSocketTable.AddLoginServer(m_hWnd,m_vloginserverinfo);// kyg추가
	//}

	// define the Tab (as a Pane)
	CPane tabPane = paneTab( &m_TabCtrlZone, VERTICAL )
		<< item( IDC_LIST_SERVERINFO  )
	    << item( IDC_LIST_SYSTEMINFO  );

	CPane bottomPane = pane( HORIZONTAL, ABSOLUTE_VERT )
		<< itemGrowing(HORIZONTAL)
		<< item( ID_BTN_VERSION , NORESIZE )
		<< item( ID_BTN_SYSTEM_SMS , NORESIZE )
		<< item( ID_BTN_SERVER_SMS , NORESIZE )
		<< item( ID_BTN_CONTROL , NORESIZE )
		<< item( ID_BTN_CONNECT_LOG , NORESIZE )
		<< item( ID_BTN_ANNOUNCE , NORESIZE );

	CreateRoot( VERTICAL )
		<< tabPane
		<< item( IDC_STATIC_STRING, NORESIZE )
		<< bottomPane;

	UpdateLayout ();

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US ||
		ioLocalManager::GetLocalType() == ioLocalManager::LCT_GERMANY  )
		MoveWindow( 0, 0, 860, 320 );
	else
		MoveWindow( 0, 0, 860, 700 );

	m_ListCtrlServerInfo.ModifyStyle(0, LVS_REPORT, NULL); //kyg 추가
	m_ListCtrlServerInfo.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);

	SetTimer(1111,100,NULL);
	SetTimer(REQ_SVRINFO,1500,NULL);

	for(int i=0; i<(int)m_vloginserverinfo.size(); ++i)
	{
		CLoingPanel* pdlg = new CLoingPanel(this);
		pdlg->Create(CLoingPanel::IDD);
		pdlg->ipaddr = m_vloginserverinfo[i].szIP;
		pdlg->port = m_vloginserverinfo[i].iPort;
		pdlg->wmid = m_vloginserverinfo[i].nwmid;
		m_vLoginPanel.push_back(pdlg);
	}
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CLSMonitorDlg::OnPaint() 
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
		ETSLayoutDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CLSMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLSMonitorDlg::OnDestroy()
{
	ETSLayoutDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	KillTimer(MAIN_TIMER);

	SAFEDELETE( m_pServerInfoDlg );
	SAFEDELETE( m_pSMSDlg );

	int iSize = (int) m_vZoneInfo.size();
	for (int i = 0; i < iSize; i++)
	{
		m_vZoneInfo[i].vServerInfo.clear();
		m_vZoneInfo[i].vSystemInfo.clear();
	}
	m_vZoneInfo.clear();

	g_TCPSocketTable.CloseAndRelease();
	g_TCPSocketTable.Close();

	m_SystemInfoUDPSocket.CloseUDP();
	m_SystemInfoUDPSocket.CloseSocket();
	LOG.CloseAndRelease();
	ConnectLog.CloseAndRelease();

}

void CLSMonitorDlg::LoadINI()
{
	m_vloginserverinfo.reserve(10);
	char szPath[MAX_PATH*2]="";
	GetCurrentDirectory(sizeof(szPath), szPath);
	strcat(szPath, "\\config.ini");
	ioINILoaderAU iniLoader(szPath);

	iniLoader.SetTitle("Info");
	
	m_iMaxConnect = iniLoader.LoadInt("MaxConnect", 0);

	/*int iType = iniLoader.LoadInt("Type", 0);
	if( iType == 5 )
	{
		char szPassword[MAX_PATH]="";
		iniLoader.LoadString("ControlPassword", "", szPassword, sizeof( szPassword ) );
		if( strcmp( szPassword, "K65kfueymsleo$#" ) != 0 )
			iType = 0;
	}

	enum { NEXT_BTN_X_GAP = 80, };

	RECT rRect;
	GetDlgItem( ID_BTN_ANNOUNCE )->GetWindowRect( &rRect );
	ScreenToClient( &rRect );

	if( iType == 0)
	{
		HideChildBtns();
		m_eDlgType = DT_NORMAL;
	}
	else if( iType == 1)
	{
		HideChildBtns();
		
		GetDlgItem( ID_BTN_SERVER_SMS )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_SERVER_SMS )->MoveWindow( &rRect );

		m_eDlgType = DT_SERVER_SMS;
		m_pSMSDlg = new CSMSDlg;
		if( m_pSMSDlg )
			m_pSMSDlg->Create( IDD_SMS );
	}
	else if( iType == 2 || iType == 7 )
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_SYSTEM_SMS )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_SYSTEM_SMS )->MoveWindow( &rRect );

		m_eDlgType = DT_SYSTEM_SMS;
		m_pSMSDlg = new CSMSDlg;
		if( m_pSMSDlg )
			m_pSMSDlg->Create( IDD_SMS );
	}
	else if( iType == 3)
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_ANNOUNCE )->MoveWindow( &rRect );

		m_eDlgType = DT_ANNOUNCE;
		m_AnnounceDlg.Create( IDD_ANNOUNCE );		
	}
	else if( iType == 4)
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_ANNOUNCE )->MoveWindow( &rRect );
		rRect.left  -= NEXT_BTN_X_GAP;
		rRect.right -= NEXT_BTN_X_GAP;
		GetDlgItem( ID_BTN_VERSION )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_VERSION )->MoveWindow( &rRect );

		m_eDlgType = DT_ANNOUNCE_VERSION;
		m_AnnounceDlg.Create( IDD_ANNOUNCE );		
		m_VersionDlg.Create( IDD_CLIENT_VERSION );		
	}
	else if( iType == 5 )
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_ANNOUNCE )->MoveWindow( &rRect );
		rRect.left  -= NEXT_BTN_X_GAP;
		rRect.right -= NEXT_BTN_X_GAP;
		GetDlgItem( ID_BTN_CONTROL )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_CONTROL )->MoveWindow( &rRect );

		m_eDlgType = DT_ANNOUNCE_CONTROL;
		m_AnnounceDlg.Create( IDD_ANNOUNCE );		
		m_ControlDlg.Create( IDD_CONTROL );		
	}
	else if( iType == 6 )
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_ANNOUNCE )->MoveWindow( &rRect );
		rRect.left  -= NEXT_BTN_X_GAP;
		rRect.right -= NEXT_BTN_X_GAP;
		GetDlgItem( ID_BTN_CONNECT_LOG )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_CONNECT_LOG )->MoveWindow( &rRect );

		m_eDlgType         = DT_ANNOUNCE_CONNECT;
		m_iWriteConnectSec = iniLoader.LoadInt("WriteConnectSec", 600 ); // 600초 10분
		m_AnnounceDlg.Create( IDD_ANNOUNCE );	
	}
	else if( iType == DT_DEVELOPER )
	{
		m_eDlgType = DT_DEVELOPER;

		m_AnnounceDlg.Create( IDD_ANNOUNCE );
		m_ControlDlg.Create( IDD_CONTROL );
		m_VersionDlg.Create( IDD_CLIENT_VERSION );

		m_pServerInfoDlg = new CServerInfoDlg( this );
		if( m_pServerInfoDlg )
			m_pServerInfoDlg->Create( IDD_SERVER_INFO, this );

		m_pSMSDlg = new CSMSDlg;
		if( m_pSMSDlg )
			m_pSMSDlg->Create( IDD_SMS );
	}*/

	// Dlg 생성.
	{
		m_pSMSDlg = new CSMSDlg;
		if( m_pSMSDlg )
			m_pSMSDlg->Create( IDD_SMS );
		m_AnnounceDlg.Create( IDD_ANNOUNCE );
		m_VersionDlg.Create( IDD_CLIENT_VERSION );
		m_ControlDlg.Create( IDD_CONTROL );
		m_pServerInfoDlg = new CServerInfoDlg( this );
		if( m_pServerInfoDlg )
			m_pServerInfoDlg->Create( IDD_SERVER_INFO, this );
	}


	int iSize = (int) m_vZoneInfo.size();
	for (int i = 0; i < iSize; i++)
	{
		m_vZoneInfo[i].vServerInfo.clear();
		m_vZoneInfo[i].vSystemInfo.clear();
	}
	m_vZoneInfo.clear();
	int zoneid;

	for (int i=0; i<MAX_TCPSOCKET_MAP; i++)
	{
		char szTitle[MAX_PATH]="";
		StringCbPrintf(szTitle, sizeof( szTitle ), "Zone%d", i+1);
		iniLoader.SetTitle(szTitle);

		ZONEINFO kZoneInfo;
		char szBuf[MAX_PATH]="";
		iniLoader.LoadString("Name", "", szBuf, sizeof(szBuf));
		if(strcmp(szBuf, "") == 0)
			break;
		kZoneInfo.szName = szBuf;
		zoneid = i;
		
		memset(szBuf, 0, sizeof(szBuf));
		iniLoader.LoadString("Ip", "", szBuf, sizeof(szBuf));
		if(strcmp(szBuf, "") == 0)
			break;
		kZoneInfo.szIP = szBuf;
		kZoneInfo.iPort= iniLoader.LoadInt("Port", 0);
		kZoneInfo.gradeType = iniLoader.LoadInt( "Type", 3 );
		kZoneInfo.eLocalType = (ioLocalManager::LocalType) iniLoader.LoadInt( "Local", (int) ioLocalManager::LCT_KOREA );

		enum { MAX_INFO = 2000, };

		// server 
		for (int i=0; i<MAX_INFO; i++)
		{
			SERVERINFO kServerInfo;

			char szKeyName[MAX_PATH]="";
			char szBuf[MAX_PATH]="";
			StringCbPrintf( szKeyName, sizeof(szKeyName), "Server%d_Name", i+1 );
			iniLoader.LoadString( szKeyName, "", szBuf, sizeof(szBuf) );
			if( strcmp( szBuf, "" ) == 0 )
				continue;
			kServerInfo.szName = szBuf;

			memset(szKeyName, 0, sizeof(szKeyName));
			StringCbPrintf( szKeyName, sizeof(szKeyName), "Server%d_IP", i+1 );
			memset( szBuf, 0, sizeof(szBuf) );
			iniLoader.LoadString( szKeyName, "", szBuf, sizeof(szBuf) );
			if( strcmp( szBuf, "" ) == 0 )
				continue;
			kServerInfo.szIP = szBuf;

			memset(szKeyName, 0, sizeof(szKeyName));
			StringCbPrintf( szKeyName, sizeof(szKeyName), "Server%d_PORT", i+1 );
			kServerInfo.iPort= iniLoader.LoadInt(szKeyName, 0);
			kServerInfo.serverType = eServerType_GameServer;

			kServerInfo.wListCtrlArray = i;
			kZoneInfo.vServerInfo.push_back( kServerInfo );
		}
	

		// Main서버 추가.
		{
			SERVERINFO kServerInfo;

			kServerInfo.szName	= kZoneInfo.szName;
			kServerInfo.szIP	= kZoneInfo.szIP;
			kServerInfo.iPort	= kZoneInfo.iPort;
			kServerInfo.serverType = eServerType_MainServer;

			 
		}
		//kyg 로그인 서버 추가 --
		for (int j=0; j<5; j++)
		{
			SERVERINFO kServerInfo;
			kServerInfo.nstate = -1;
			kServerInfo.nwmid = WM_TCPLOGIN1 + m_vloginserverinfo.size();

			char szKeyName[MAX_PATH]="";
			char szBuf[MAX_PATH]="";
			StringCbPrintf( szKeyName, sizeof(szKeyName), "LoginServer%d_Name", j+1 );
			iniLoader.LoadString( szKeyName, "", szBuf, sizeof(szBuf) );
			if( strcmp( szBuf, "" ) == 0 )
				break;
			kServerInfo.szName = szBuf;

			memset(szKeyName, 0, sizeof(szKeyName));
			StringCbPrintf( szKeyName, sizeof(szKeyName), "LoginServer%d_IP", j+1 );
			memset( szBuf, 0, sizeof(szBuf) );
			iniLoader.LoadString( szKeyName, "", szBuf, sizeof(szBuf) );
			if( strcmp( szBuf, "" ) == 0 )
				continue;
			kServerInfo.szIP = szBuf;

			memset(szKeyName, 0, sizeof(szKeyName));
			StringCbPrintf( szKeyName, sizeof(szKeyName), "LoginServer%d_PORT", j+1 );
			kServerInfo.iPort= iniLoader.LoadInt(szKeyName, 0);
			kServerInfo.nzoneid = zoneid+1;

		//	kZoneInfo.vServerInfo.push_back( kServerInfo );
			m_vloginserverinfo.push_back( kServerInfo );
		}

		// system 
		for (int i=0; i<MAX_INFO; i++)
		{
			char szKeyName[MAX_PATH]="";
			StringCbPrintf( szKeyName, sizeof(szKeyName), "System%d_IP", i+1);
			char szBuf[MAX_PATH]="";
			iniLoader.LoadString(szKeyName, "", szBuf, sizeof(szBuf));

			SYSTEMINFO kSystemInfo;
			kSystemInfo.szIP = szBuf;

			memset(szKeyName, 0, sizeof(szKeyName));
			StringCbPrintf( szKeyName, sizeof(szKeyName), "System%d_Name", i+1);
			memset(szBuf, 0, sizeof(szBuf));
			iniLoader.LoadString(szKeyName, "", szBuf, sizeof(szBuf));
			if(strcmp(szBuf, "") == 0)
				continue;
			kSystemInfo.szName = szBuf;

			memset(szKeyName, 0, sizeof(szKeyName));
			StringCbPrintf( szKeyName, sizeof(szKeyName), "System%d_URL", i+1);
			memset(szBuf, 0, sizeof(szBuf));
			iniLoader.LoadString(szKeyName, "", szBuf, sizeof(szBuf));
			kSystemInfo.szURL  = szBuf;

			memset(szKeyName, 0, sizeof(szKeyName));
			StringCbPrintf( szKeyName, sizeof(szKeyName), "System%d_Control", i+1);
			if( iniLoader.LoadInt( szKeyName, 0 ) == 1 )
				kSystemInfo.bControl = true;
			else
				kSystemInfo.bControl = false;

			kSystemInfo.wListCtrlArray = i;
			kZoneInfo.vSystemInfo.push_back(kSystemInfo);
		}
		
		m_vZoneInfo.push_back( kZoneInfo );
	}

	// Tab Control Initialize
	TC_ITEM item;
	iSize = (int) m_vZoneInfo.size();
	for(int i=0;i<iSize;i++)
	{
		item.mask=TCIF_TEXT|TCIF_IMAGE;
		item.pszText= m_vZoneInfo[i].szName.GetBuffer(0);
		item.iImage=i;
		m_TabCtrlZone.InsertItem(i,&item);
	}
	m_TabCtrlZone.SetCurSel(0);

	// list control
	InitListServerInfo();
	InitListSystemInfo();

	SetServerDefaultList( 0 );
	SetSystemDefaultList( 0 );
	
	// sms
	if( m_pSMSDlg )
		m_pSMSDlg->InitServerInfo( m_vZoneInfo );

	// sms
	/*if( m_eDlgType == DT_SERVER_SMS )
	{
		if( m_pSMSDlg )
			m_pSMSDlg->InitServerInfo( m_vZoneInfo );
	}
	if( m_eDlgType == DT_SYSTEM_SMS )
	{
		if( m_pSMSDlg )
			m_pSMSDlg->InitSystemInfo( m_vZoneInfo );
	}*/
	
	// tab창 밑으로 리스트가 들어가는 문제 때문에
	m_ListCtrlServerInfo.BringWindowToTop();
	m_ListCtrlSystemInfo.BringWindowToTop();

	if( !m_vZoneInfo.empty() )
		ioLocalManager::SetLocalType( m_vZoneInfo[0].eLocalType );

	// http connector
	int iMax = (int) m_vZoneInfo.size();
	for (int i = 0; i < iMax; i++)
	{
		int iSize = (int)m_vZoneInfo[i].vSystemInfo.size();
		for (int j = 0; j < iSize ; j++)
		{
			SYSTEMINFO &kInfo = m_vZoneInfo[i].vSystemInfo[j];
			if( kInfo.szURL.IsEmpty() )
				continue;

			HttpConnector *pHttpConnector = new HttpConnector;
			if( !pHttpConnector )
				continue;

			pHttpConnector->Begin();
			m_vHttpConnectorVec.push_back( pHttpConnector );
		}
		
	}

	// tab
	UpdateBtnCtrl( m_vZoneInfo[ 0 ].gradeType );
}

void CLSMonitorDlg::UpdateServerListCtrl( const SERVERINFO  *pServerInfo )
{
	if( pServerInfo == NULL )
		return;

	if( pServerInfo->wListCtrlArray >= m_ListCtrlServerInfo.GetItemCount() )
		return;

	const int iMax = LIST_ITEM_STATE+1;
	CString szStateText = STATE_NORMAL;

	for (int i = LIST_ITEM_CONNECT; i < iMax ; i++)
	{
		char szItemText[MAX_PATH]="";
		switch(i)
		{
		case LIST_ITEM_CONNECT:
			StringCbPrintf(szItemText, sizeof(szItemText), "%d", pServerInfo->wConnect);
			SetServerStateText( pServerInfo, i, szStateText );      
			break;
		case LIST_ITEM_ROOMCNT:
			StringCbPrintf(szItemText, sizeof(szItemText), "%d", pServerInfo->wRoomCount);
			SetServerStateText( pServerInfo, i, szStateText );      
			break;
		case LIST_ITEM_PLAZACNT:
			StringCbPrintf(szItemText, sizeof(szItemText), "%d", pServerInfo->wPlazaCount);
			SetServerStateText( pServerInfo, i, szStateText );      
			break;
		case LIST_ITEM_BATTLEROOMCNT:
			StringCbPrintf(szItemText, sizeof(szItemText), "%d", pServerInfo->wBattleRoomCount);
			SetServerStateText( pServerInfo, i, szStateText );      
			break;
		case LIST_ITEM_PINGMS:
			StringCbPrintf(szItemText, sizeof(szItemText), "%d", pServerInfo->wPingMS);
			SetServerStateText( pServerInfo, i, szStateText );      
			break;
		case LIST_ITEM_DBMS:
			StringCbPrintf(szItemText, sizeof(szItemText), "%d", pServerInfo->wDBQueryMs );
			SetServerStateText( pServerInfo, i, szStateText );      
			break;
		case LIST_ITEM_STATE:
			SetServerStateText( pServerInfo, i, szStateText );
			StringCbCopy(szItemText, sizeof(szItemText), (LPCTSTR)szStateText);
			break;
		}
		m_ListCtrlServerInfo.SetItemText( pServerInfo->wListCtrlArray, i,  szItemText);
	}
	m_ListCtrlServerInfo.Invalidate(false);
}

void CLSMonitorDlg::UpdateSystemListCtrl( const SYSTEMINFO *pSystemInfo )
{
	if( !pSystemInfo )
		return;

	if(pSystemInfo->wListCtrlArray >= m_ListCtrlSystemInfo.GetItemCount())
		return;

	const int iMax = LIST_ITEM_STATE+1;
	CString szStateText = STATE_NORMAL;
	for (int i = LIST_SYSTEM_ITEM_CPU; i < iMax ; i++)
	{
		char szItemText[MAX_PATH]="";
		switch(i)
		{
		case LIST_SYSTEM_ITEM_CPU:
			StringCbPrintf(szItemText, sizeof( szItemText ), "%d %%", pSystemInfo->iCPUUsed);
			SetSystemStateText(pSystemInfo, i, szStateText);      
			break;
		case LIST_SYSTEM_ITEM_MEMORY:
			StringCbPrintf(szItemText, sizeof( szItemText ), "%d M", pSystemInfo->iMemoryRemain);
			SetSystemStateText(pSystemInfo, i, szStateText);      
			break;
		case LIST_SYSTEM_ITEM_C_MEMORY:
			StringCbPrintf(szItemText, sizeof( szItemText ), "%3.2f %%", pSystemInfo->dbMemoryUsed);
		    break;
		case LIST_SYSTEM_ITEM_CDRIVE:
			StringCbPrintf(szItemText, sizeof( szItemText ),"%d M", pSystemInfo->iCDriveRemain);
			SetSystemStateText(pSystemInfo, i, szStateText);
		    break;
		case LIST_SYSTEM_ITEM_DDRIVE:
			if(pSystemInfo->iDDreiveRemain != DONT_HAVE_DDRIVE) 
			{
				StringCbPrintf(szItemText, sizeof( szItemText ),"%d M",  pSystemInfo->iDDreiveRemain);
				SetSystemStateText(pSystemInfo, i, szStateText);
			}
			else
				StringCbPrintf(szItemText, sizeof( szItemText ), DONT_HAVE_DDRIVE_STRING);
			break;
		case LIST_SYSTEM_ITEM_STATE:
			SetSystemStateText(pSystemInfo, i, szStateText);
			StringCbCopy(szItemText, sizeof( szItemText ), (LPCTSTR)szStateText );
			break;
		}
		m_ListCtrlSystemInfo.SetItemText(pSystemInfo->wListCtrlArray, i,  szItemText);
	}
	m_ListCtrlSystemInfo.Invalidate(false);
}

void CLSMonitorDlg::UpdateBtnCtrl( int gradeType )
{
	enum { NEXT_BTN_X_GAP = 80, };

	RECT rRect;
	GetDlgItem( ID_BTN_ANNOUNCE )->GetWindowRect( &rRect );
	ScreenToClient( &rRect );

	if( gradeType == 0)
	{
		HideChildBtns();
	}
	else if( gradeType == 1)
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_SERVER_SMS )->ShowWindow( SW_SHOW );
	}
	else if( gradeType == 2 || gradeType == 7 )
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_SYSTEM_SMS )->ShowWindow( SW_SHOW );
	}
	else if( gradeType == 3)
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_SHOW );
	}
	else if( gradeType == 4)
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_VERSION )->ShowWindow( SW_SHOW );
	}
	else if( gradeType == 5 )
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_CONTROL )->ShowWindow( SW_SHOW );
	}
	else if( gradeType == 6 )
	{
		HideChildBtns();

		GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_CONNECT_LOG )->ShowWindow( SW_SHOW );
	}
	else if( gradeType == DT_DEVELOPER )
	{
		GetDlgItem( ID_BTN_SERVER_SMS )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_SYSTEM_SMS )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_CONTROL )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_VERSION )->ShowWindow( SW_SHOW );
		GetDlgItem( ID_BTN_CONNECT_LOG )->ShowWindow( SW_SHOW );
	}

	UpdateData( FALSE );
}

void CLSMonitorDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	// 10초마다 호출됨
	if(nIDEvent == LS_SERVER)
	{

	}
	else if(nIDEvent == REQ_SVRINFO)
	{
		SendReqSvrInfoPacekt();
	}
	else if(nIDEvent == 1111)
	{
		KillTimer(nIDEvent);
		SendMonitorPacekt();
	}
	else
	{
		_OnTimerSendMsg();
		_OnTimerUpdateNoReaciton();

		// 1분체크
		_OnTimer1Minutes();
		//

		// 10분 체크
		_OnTimer10Minutes();
		//

		_OnTimerWriteConnectLog();

		for( int i = 0 ; i < m_vZoneInfo.size() ; ++i )
		{
			if( m_vZoneInfo[ i ].gradeType == DT_SERVER_SMS || m_vZoneInfo[ i ].gradeType == DT_SYSTEM_SMS )
			{
				if( m_pSMSDlg )
					m_pSMSDlg->_OnTimerSMS();
			}
		}

		/*if( m_eDlgType == DT_SERVER_SMS || m_eDlgType == DT_SYSTEM_SMS )
		{
			if( m_pSMSDlg )
				m_pSMSDlg->_OnTimerSMS();
		}*/
	}
	ETSLayoutDialog::OnTimer(nIDEvent);
}

void CLSMonitorDlg::_OnTimerSendMsg()
{	
	int iMax = (int) m_vZoneInfo.size();
	//if( m_eDlgType != DT_SYSTEM_SMS )
	//{
		for (int i = 0; i < iMax; i++)
		{
			if( m_vZoneInfo[ i ].gradeType != DT_SYSTEM_SMS )
			{
				SP2Packet kPacket(CTPKM_REQUEST_NUM_CONNECT);
				g_TCPSocketTable.Send(WM_TCPSOCKET1+i, kPacket);
			}
		}
	//}

	SendUDPMsg();
	SendWebServer(); // 2분에 한번
}

void CLSMonitorDlg::_OnTimerUpdateNoReaciton()
{
	int iZoneSize = (int)m_vZoneInfo.size();
	for(int i = 0;i < iZoneSize;i++)
	{
		DWORD dwCurTime = timeGetTime();
		int iSize = (int)m_vZoneInfo[i].vServerInfo.size();
		for (int j = 0; j < iSize ; j++)
		{
			SERVERINFO &kInfo = m_vZoneInfo[i].vServerInfo[j];

			if( m_vZoneInfo[ i ].gradeType == DT_SERVER_SMS &&
				( ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == STOPING ) ||
			     ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == COUNTING_ERROR) ) )
				 continue;

			/*if(  m_eDlgType == DT_SERVER_SMS &&  
			   ( ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == STOPING ) ||
			     ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == COUNTING_ERROR) ) )
				continue;*/

			if( dwCurTime - kInfo.dwLastRcvTime >= NO_REACTION_TIME )
			{
				kInfo.bReaction = false;
				if( m_TabCtrlZone.GetCurSel() == i )
				{
					m_ListCtrlServerInfo.SetItemText(j, LIST_ITEM_STATE,  STATE_NO_REACTION);	
					m_ListCtrlServerInfo.Update(j);	
				}

				//if( m_eDlgType == DT_SERVER_SMS )
				if( m_vZoneInfo[ i ].gradeType == DT_SERVER_SMS )
				{
					if( m_pSMSDlg )
					{
						m_pSMSDlg->SetFirstError(kInfo.wSMSArray);
						m_pSMSDlg->SetErrorInfo(kInfo.szIP);
					}
				}
			}
		}
	}	

	for(int i = 0;i < iZoneSize;i++)
	{
		DWORD dwCurTime = timeGetTime();
		int iSize = (int) m_vZoneInfo[i].vSystemInfo.size();
		for (int j = 0; j < iSize ; j++)
		{

			SYSTEMINFO &kInfo = m_vZoneInfo[i].vSystemInfo[j];
			if( m_vZoneInfo[ i ].gradeType == DT_SYSTEM_SMS &&
				( ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == STOPING ) ||
			     ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == COUNTING_ERROR) ) )
				 continue;

			/*if( m_eDlgType == DT_SYSTEM_SMS && 
		      ( ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == STOPING ) ||
			    ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == COUNTING_ERROR ) ) )
				continue;*/
			
			DWORD dwDelayTime = NO_REACTION_TIME;
			if( !kInfo.szURL.IsEmpty() )
				dwDelayTime = NO_REACTION_TIME_LONG;

			if(dwCurTime - kInfo.dwLastRcvTime >= dwDelayTime)
			{
				kInfo.bReaction = false;
				if( m_TabCtrlZone.GetCurSel() == i )
				{
					m_ListCtrlSystemInfo.SetItemText(j, LIST_SYSTEM_ITEM_STATE,  STATE_NO_REACTION);	
					m_ListCtrlSystemInfo.Update(j);
				}

				//if( m_eDlgType == DT_SYSTEM_SMS )
				if( m_vZoneInfo[ i ].gradeType == DT_SYSTEM_SMS )
				{
					if( m_pSMSDlg )
					{
						m_pSMSDlg->SetFirstError(kInfo.wSMSArray);
						m_pSMSDlg->SetErrorInfo(kInfo.szIP);
					}
				}
			}
		}
	}

}

void CLSMonitorDlg::_OnTimerWriteConnectLog()
{
	/*if( m_eDlgType != DT_ANNOUNCE_CONNECT )
		return;*/

	bool bWrite = false;
	static int i10Sec = 0;
	if( (i10Sec * 10) >= m_iWriteConnectSec )
	{
		i10Sec = 0;
		bWrite = true;
	}
	i10Sec++;

	if( !bWrite )
		return;

	_OnTimerCreateNewLog( ConnectLog, "CONNECT", m_szConnectLogPrevTime, sizeof( m_szConnectLogPrevTime ) );

	int iZoneSize = (int)m_vZoneInfo.size();
	for(int i = 0;i < iZoneSize;i++)
	{
		int iTotalConnect = 0;
		int iSize = (int)m_vZoneInfo[i].vServerInfo.size();
		for (int j = 0; j < iSize ; j++)
		{
			if( m_vZoneInfo[ i ].gradeType == DT_ANNOUNCE_CONNECT )
				continue;

			SERVERINFO &kInfo = m_vZoneInfo[i].vServerInfo[j];
			iTotalConnect += kInfo.wConnect;
		}

		ConnectLog.PrintTimeAndLog( 0, "%s : %d", (LPCTSTR)m_vZoneInfo[i].szName, iTotalConnect );
	}
}

void CLSMonitorDlg::SetServerDefaultList( int iZoneArray )
{
	if(m_vZoneInfo.empty())
		return;

	if(!COMPARE( iZoneArray, 0, (int)m_vZoneInfo.size() ) )
		return;

	m_ListCtrlServerInfo.DeleteAllItems();

	enum { MAX_DEFAULT_ITEM = 4,};
	const int iSubItemList[MAX_DEFAULT_ITEM]={LIST_ITEM_NAME, LIST_ITEM_IP, LIST_ITEM_PORT, LIST_ITEM_STATE};

	ZONEINFO &kZoneInfo = m_vZoneInfo[iZoneArray];
    int iSize = (int)kZoneInfo.vServerInfo.size();
	for (int i = iSize-1; i  >= 0; i--)
	{
		SERVERINFO &kServerInfo = kZoneInfo.vServerInfo[i];
		for (int iItem = 0; iItem < MAX_DEFAULT_ITEM ; iItem++)
		{
			LV_ITEM lvitem;
			lvitem.mask=LVIF_TEXT;
			lvitem.iItem=0;
			lvitem.iSubItem=iSubItemList[iItem];
			switch( lvitem.iSubItem )
			{
			case LIST_ITEM_NAME:
				lvitem.pszText=kServerInfo.szName.GetBuffer(0);
				m_ListCtrlServerInfo.InsertItem(&lvitem);
				break;
			case LIST_ITEM_IP:
				lvitem.pszText=kServerInfo.szIP.GetBuffer(0);
				m_ListCtrlServerInfo.SetItem(&lvitem);
				break;
			case LIST_ITEM_PORT:
				{
				CString szTemp;
				szTemp.Format("%d", kServerInfo.iPort);
				lvitem.pszText=szTemp.GetBuffer(0);
				m_ListCtrlServerInfo.SetItem(&lvitem);
				}
				break;
			case LIST_ITEM_STATE:
				lvitem.pszText=STATE_STARTING;
				m_ListCtrlServerInfo.SetItem(&lvitem);
				break;
			}
		}
	}
#if 1

	for(int i=0; i<m_vloginserverinfo.size();++i)
	{
		if(iZoneArray+1 == m_vloginserverinfo[i].nzoneid)
		{
			SERVERINFO &kServerInfo = m_vloginserverinfo[i];
			LV_ITEM lvitem;
			lvitem.mask=LVIF_TEXT;
			lvitem.iItem=m_ListCtrlServerInfo.GetItemCount();

			for (int iItem = 0; iItem < MAX_DEFAULT_ITEM ; iItem++)
			{
				lvitem.iSubItem=iSubItemList[iItem];
				switch( lvitem.iSubItem )
				{
				case LIST_ITEM_NAME:
					lvitem.pszText=kServerInfo.szName.GetBuffer(0);
					m_ListCtrlServerInfo.InsertItem(&lvitem);
					break;
				case LIST_ITEM_IP:
					lvitem.pszText=kServerInfo.szIP.GetBuffer(0);
					m_ListCtrlServerInfo.SetItem(&lvitem);
					break;
				case LIST_ITEM_PORT:
					{
						CString szTemp;
						szTemp.Format("%d", kServerInfo.iPort);
						lvitem.pszText=szTemp.GetBuffer(0);
						m_ListCtrlServerInfo.SetItem(&lvitem);
					}
					break;
				case LIST_ITEM_STATE:
					lvitem.pszText=STATE_STARTING;
					m_ListCtrlServerInfo.SetItem(&lvitem);
					break;
				}
			}
		}
	}
#endif
	
}

void CLSMonitorDlg::SetSystemDefaultList( int iZoneArray )
{
	if(m_vZoneInfo.empty())
		return;

	if(!COMPARE( iZoneArray, 0, (int)m_vZoneInfo.size() ) )
		return;

	m_ListCtrlSystemInfo.DeleteAllItems();

	ZONEINFO &kZoneInfo = m_vZoneInfo[iZoneArray];
	enum { MAX_DEFAULT_ITEM = 3, };
	const int iSubItemList[MAX_DEFAULT_ITEM]={LIST_SYSTEM_ITEM_IP, LIST_SYSTEM_ITEM_NAME, LIST_SYSTEM_ITEM_STATE};

	int iSize = (int)kZoneInfo.vSystemInfo.size();
	for (int i = iSize-1; i  >= 0; i--)
	{
		for (int iItem = 0; iItem < MAX_DEFAULT_ITEM ; iItem++)
		{
			LV_ITEM lvitem;
			lvitem.mask=LVIF_TEXT;
			lvitem.iItem=0;
			lvitem.iSubItem=iSubItemList[iItem];
			switch(iItem)
			{
			case 0:
				lvitem.pszText= kZoneInfo.vSystemInfo[i].szIP.GetBuffer(0);
				m_ListCtrlSystemInfo.InsertItem(&lvitem);
				break;
			case 1:
				lvitem.pszText= kZoneInfo.vSystemInfo[i].szName.GetBuffer(0);
				m_ListCtrlSystemInfo.SetItem(&lvitem);
			    break;
			case 2:
				lvitem.pszText=STATE_STARTING;
				m_ListCtrlSystemInfo.SetItem(&lvitem);
			    break;
			}
		}
	}
}


void CLSMonitorDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	for( int i = 0 ; i < m_vZoneInfo.size() ; ++i )
	{
		//if( m_eDlgType == DT_ANNOUNCE_CONTROL && m_ControlDlg.IsWindowVisible() )
		if( m_vZoneInfo[ i ].gradeType == DT_ANNOUNCE_CONTROL && m_ControlDlg.IsWindowVisible() )
		{
			AfxMessageBox( "먼저 Control 윈도우을 닫아주세요.");
			return;
		}
	}

	if(AfxMessageBox("Do you want to Exit?",MB_YESNO) == IDNO)
		return;
	EndDialog(0);
	
	for(vHttpConnectorVec::iterator iter = m_vHttpConnectorVec.begin(); iter != m_vHttpConnectorVec.end(); ++iter)
	{
	    HttpConnector *pHttpConnector = (*iter);
		if( !pHttpConnector )
			continue;
		pHttpConnector->Close();
		Sleep(100);
		SAFEDELETE( pHttpConnector );
	}
	m_vHttpConnectorVec.clear();
}

void CLSMonitorDlg::_OnTimerCreateNewLog( CLog &rLog, const char *szFileHeadName, char *szPrevTime, int iPrevTimeSize )
{	
	SYSTEMTIME st;
	GetLocalTime(&st);
	char szCurTime[MAX_PATH] = "";
	StringCbPrintf(szCurTime, sizeof(szCurTime), "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);

	if( strcmp(szCurTime, szPrevTime) != 0)
	{
		memset(szPrevTime, 0, iPrevTimeSize);
		StringCbCopy(szPrevTime, iPrevTimeSize, szCurTime);

		rLog.CloseLog();

		char TimeLogName[MAX_PATH]="";
		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "MLOG\\%s%s.log", szFileHeadName, szCurTime);
		rLog.OpenLog(0, TimeLogName, true);
	}
}

void CLSMonitorDlg::SetServerStateText( const SERVERINFO *pServerInfo, int iItem, OUT CString &szState )
{
	if( pServerInfo == NULL )
		return;

	switch(iItem)
	{
	case LIST_ITEM_CONNECT:
		if (pServerInfo->wConnect >= MAXIMUM_CONNECT_PER * m_iMaxConnect)
			szState = STATE_MAXIMUM_CONNECT;      
		else if (pServerInfo->wConnect >= TOO_HIGH_CONNECT_PER * m_iMaxConnect)
			szState = STATE_TOO_HIGH_CONNECT;      
		else if (pServerInfo->wConnect >= HIGH_CONNECT * m_iMaxConnect)
			szState = STATE_HIGH_CONNECT;      
		break;
	case LIST_ITEM_STATE:
		if( !pServerInfo->bReaction )
			szState = STATE_NO_REACTION;

		int iZoneArray = m_TabCtrlZone.GetCurSel();
		if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SERVER_SMS && ( m_pSMSDlg && m_pSMSDlg->GetState(pServerInfo->wSMSArray) == STOPING ) )
		//if( m_eDlgType == DT_SERVER_SMS && ( m_pSMSDlg && m_pSMSDlg->GetState(pServerInfo->wSMSArray) == STOPING ) )
		{
			int iMinute = ( (m_pSMSDlg->GetRemainWaitTime(pServerInfo->wSMSArray)/1000) / 60 );
			CString szText;
			szText.Format("%s:%dmin", STATE_STOP_MONITOR, iMinute);
			szState = szText;
		}
		break;
	}
}

void CLSMonitorDlg::SetSystemStateText( const SYSTEMINFO *pSystemInfo, int iItem, OUT CString &szState )
{
	if( !pSystemInfo )
		return;

	enum { MAXIMUM_CPU_PER = 95, TOO_HIGH_CPU_PER = 70, HIGH_CPU_PER = 50, 
	       LOW_MEMORY_MB = 50, LOW_DRIEVE_MB = 10000, };
	switch(iItem)
	{
	case LIST_SYSTEM_ITEM_CPU:
		if (pSystemInfo->iCPUUsed >= MAXIMUM_CPU_PER)
			szState = STATE_MAXIMUM_CPU;      
		else if (pSystemInfo->iCPUUsed >= TOO_HIGH_CPU_PER)
			szState = STATE_TOO_HIGH_CPU;      
		else if (pSystemInfo->iCPUUsed >= HIGH_CPU_PER)
			szState = STATE_HIGH_CPU;      
		break;
	case LIST_SYSTEM_ITEM_MEMORY:
		if(pSystemInfo->iMemoryRemain < LOW_MEMORY_MB)
			szState = STATE_LOW_MEMORY;      
		break;
	case LIST_SYSTEM_ITEM_CDRIVE:
		if(pSystemInfo->iCDriveRemain < LOW_DRIEVE_MB)
			szState = STATE_LOW_CDRIVE;      
		break;
	case LIST_SYSTEM_ITEM_DDRIVE:
		if(pSystemInfo->iDDreiveRemain != DONT_HAVE_DDRIVE) 
		{
			if(pSystemInfo->iDDreiveRemain < LOW_DRIEVE_MB)
				szState = STATE_LOW_DDRIVE;      
		}
		break;
	case LIST_SYSTEM_ITEM_STATE:
		if( !pSystemInfo->bReaction )
			szState = STATE_NO_REACTION;

		int iZoneArray = m_TabCtrlZone.GetCurSel();
		if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SYSTEM_SMS && ( m_pSMSDlg && m_pSMSDlg->GetState(pSystemInfo->wSMSArray) == STOPING ) )	
		//if( m_eDlgType == DT_SYSTEM_SMS && ( m_pSMSDlg && m_pSMSDlg->GetState(pSystemInfo->wSMSArray) == STOPING ) )	
		{
			int iMinute = ( (m_pSMSDlg->GetRemainWaitTime(pSystemInfo->wSMSArray)/1000) / 60 );
			CString szText;
			szText.Format("%s:%dmin", STATE_STOP_MONITOR, iMinute);
			szState = szText;
		}
		break;
	}
}


SERVERINFO *CLSMonitorDlg::GetServerInfo( ZONEINFO *pZoneInfo, SP2Packet &rkPacket )
{
	if( pZoneInfo == NULL )
		return NULL;

	char szIP[MAX_PATH]   = "";
	int  iPort = 0;
	rkPacket >> szIP >> iPort;
	int iInfoSize = (int)pZoneInfo->vServerInfo.size();
	for(int i = 0;i < iInfoSize;i++)
	{
		if(	szIP   == pZoneInfo->vServerInfo[i].szIP &&
			iPort  == pZoneInfo->vServerInfo[i].iPort )
		{
			return &pZoneInfo->vServerInfo[i];
		}
	}
	return NULL;
}

SYSTEMINFO * CLSMonitorDlg::GetSystemInfo( IN const char* szIP , OUT bool &rbUpdateList )
{
	int iMax = (int) m_vZoneInfo.size();
	for (int i = 0; i < iMax ; i++)
	{
		int iSize = (int)m_vZoneInfo[i].vSystemInfo.size();
		for (int j = 0; j < iSize ; j++)
		{
			if(m_vZoneInfo[i].vSystemInfo[j].szIP == szIP )
			{
				if( m_TabCtrlZone.GetCurSel() == i )
					rbUpdateList = true;
				else
					rbUpdateList = false;

				return &m_vZoneInfo[i].vSystemInfo[j];
			}
		}
	}

	return NULL;
}

SYSTEMINFO * CLSMonitorDlg::GetSystemInfoByURL( IN const char* szURL , OUT bool &rbUpdateList )
{
	int iMax = (int) m_vZoneInfo.size();
	for (int i = 0; i < iMax ; i++)
	{
		int iSize = (int)m_vZoneInfo[i].vSystemInfo.size();
		for (int j = 0; j < iSize ; j++)
		{
			if(m_vZoneInfo[i].vSystemInfo[j].szURL == szURL )
			{
				if( m_TabCtrlZone.GetCurSel() == i )
					rbUpdateList = true;
				else
					rbUpdateList = false;

				return &m_vZoneInfo[i].vSystemInfo[j];
			}
		}
	}

	return NULL;
}

void CLSMonitorDlg::ProcessPacket(UINT wndMsg, SP2Packet &rkPacket )
{
	switch(rkPacket.GetPacketID())
	{
		case STPKM_ANSWER_NUM_CONNECT:
			OnNumConnect(wndMsg, rkPacket);
			break;	
		case STPKM_SERVER_DISCONNECT:
			OnServerDisconnect(wndMsg, rkPacket);
			break;
		case STPKM_LOAD_CS3_FILE:
			OnLoadCS3File(wndMsg, rkPacket);
			break;
		case STPKM_CS3_FILE_VERSION:
			OnCS3FileVersion(wndMsg, rkPacket);
			break;
		case STPKM_ALL_SERVER_INFO:
			OnAllServerInfo(wndMsg, rkPacket);
			break;

		case STPKM_SERVER_INFO_ACK:
			{
				OnServerInfoAck( wndMsg, rkPacket );
			}
			break;
		case EPROTOCOL::LSPTK_SERVER_RESPONSE:
			{
				OnLSResPonse(wndMsg, rkPacket);

				 
			}
			break;

	}
}

void CLSMonitorDlg::_OnTimer1Minutes()
{
	static int i10secondsCnt = 0;
	if(i10secondsCnt >= 3)
	{
		i10secondsCnt = 0;
		//
		_OnTimerCreateNewLog( LOG, "BUG", m_szLogPrevTime, sizeof( m_szLogPrevTime ) );

		g_TCPSocketTable.Reconnect(m_hWnd);
	//	SendMonitorPacekt(); //kyg 추가
		SetTimer(1111,100,NULL);
		
	}
	i10secondsCnt++;
}

void CLSMonitorDlg::_OnTimer10Minutes()
{
	static int i10secondsCnt = 0;
	if(i10secondsCnt >= 60)
	{
		i10secondsCnt = 0;
	}
	i10secondsCnt++;
}

void CLSMonitorDlg::Disconnect( UINT wndMsg )
{
	
	for(int i=0; i<m_vloginserverinfo.size();++i)
	{
		if(m_vloginserverinfo[i].nwmid == wndMsg)
		{
			m_vloginserverinfo[i].nstate = -1;
		}
	}
	
}

void CLSMonitorDlg::InitListServerInfo()
{
	m_ListCtrlServerInfo.SetExtendedStyle( LVS_EX_GRIDLINES);
	enum { MAX_COLUMN = 10, };

	int iColumnWidthList[MAX_COLUMN]={100,100,80,80,80,80,80,50,50,128};
	CString szColumnTextList[MAX_COLUMN]={"Name","Ip", "Port", "Connect", "Room/User", "Plaza", "BattleRoom", "PingMS", "DBMS", "STATE" };

	for (int i = 0; i < MAX_COLUMN ; i++)
	{
		LV_COLUMN lvcolumn;
		lvcolumn.mask=LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
		lvcolumn.fmt=LVCFMT_LEFT;
		lvcolumn.iSubItem=i;
		lvcolumn.cx=iColumnWidthList[i];
		lvcolumn.pszText=_T(szColumnTextList[i].GetBuffer(0));
		m_ListCtrlServerInfo.InsertColumn(i,&lvcolumn);
	}
}

void CLSMonitorDlg::InitListSystemInfo()
{
	m_ListCtrlSystemInfo.SetExtendedStyle( LVS_EX_GRIDLINES);
	enum { MAX_COLUMN = 8, };

	int iColumnWidthList[MAX_COLUMN]={100,100,100,100,100,100,100,130};
	CString szColumnTextList[MAX_COLUMN]={"IP","Name", "CPU", "Memory", "C_Memory", "C: hdd", "D: hdd", "STATE" };

	for (int i = 0; i < MAX_COLUMN ; i++)
	{
		LV_COLUMN lvcolumn;
		lvcolumn.mask=LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
		lvcolumn.fmt=LVCFMT_LEFT;
		lvcolumn.iSubItem=i;
		lvcolumn.cx=iColumnWidthList[i];
		lvcolumn.pszText=_T(szColumnTextList[i].GetBuffer(0));
		m_ListCtrlSystemInfo.InsertColumn(i,&lvcolumn);
	}
}

void CLSMonitorDlg::OnNMCustomdrawListServerInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = CDRF_DODEFAULT;
	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec ); // 각 라인 설정....
		// 글자색 바꾸기.. 
		COLORREF crTextColor = RGB_BLACK;     // 해당 라인의 글자 색 설정..
		COLORREF crBkColor = RGB_GRAY;        // 해당 라인의 배경 색 설정..

		char szState[MAX_PATH]="";
		StringCbCopy (szState ,sizeof(szState), m_ListCtrlServerInfo.GetItemText(nItem,LIST_ITEM_STATE));

		if(strcmp(szState, STATE_MAXIMUM_CONNECT) == 0 )
		{
			crTextColor = RGB_WHITE;      
			crBkColor = RGB_RED;    
		}
		else if(strcmp(szState, STATE_TOO_HIGH_CONNECT ) == 0)
		{
			crTextColor = RGB_BLACK;        
			crBkColor = RGB_LIGHT_RED;      
		}
		else if(strcmp(szState, STATE_HIGH_CONNECT) == 0)
		{
			crTextColor = RGB_BLACK;        
			crBkColor = RGB_MORE_LIGHT_RED;        
		}
		else if(strcmp(szState, STATE_NO_REACTION) == 0)
		{
			crTextColor = RGB_WHITE;     
			crBkColor   = RGB_LIGHT_GREEN;        
		}
		else if(strncmp(szState, STATE_STOP_MONITOR, strlen(STATE_STOP_MONITOR)) == 0)
		{
			crTextColor = RGB_WHITE;     
			crBkColor   = RGB_BLACK;        
		}

		// Store the color back in the NMLVCUSTOMDRAW struct.
		pLVCD->clrText = crTextColor;        
		pLVCD->clrTextBk = crBkColor;        

		// Tell Windows to paint the control itself.
		*pResult = CDRF_DODEFAULT;	
	}
}

void CLSMonitorDlg::OnNMCustomdrawListSysteminfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = CDRF_DODEFAULT;
	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec ); // 각 라인 설정....
		// 글자색 바꾸기.. 
		COLORREF crTextColor = RGB_BLACK;     // 해당 라인의 글자 색 설정..
		COLORREF crBkColor = RGB_GRAY;        // 해당 라인의 배경 색 설정..

		char szState[MAX_PATH]="";
		strcpy (szState , m_ListCtrlSystemInfo.GetItemText(nItem,LIST_SYSTEM_ITEM_STATE));

		if(strcmp(szState, STATE_MAXIMUM_CPU) == 0
			||strcmp(szState, STATE_LOW_MEMORY) == 0 
			||strcmp(szState, STATE_LOW_CDRIVE) == 0 
			||strcmp(szState, STATE_LOW_DDRIVE) == 0 )
		{
			crTextColor = RGB_WHITE;      
			crBkColor = RGB_RED;    
		}
		else if(strcmp(szState, STATE_TOO_HIGH_CPU ) == 0)
		{
			crTextColor = RGB_BLACK;        
			crBkColor = RGB_LIGHT_RED;      
		}
		else if(strcmp(szState, STATE_HIGH_CPU ) == 0)
		{
			crTextColor = RGB_BLACK;        
			crBkColor = RGB_MORE_LIGHT_RED;        
		}
		else if(strcmp(szState, STATE_NO_REACTION) == 0)
		{
			crTextColor = RGB_WHITE;     
			crBkColor   = RGB_LIGHT_GREEN;        
		}
		else if(strncmp(szState, STATE_STOP_MONITOR, strlen(STATE_STOP_MONITOR)) == 0)
		{
			crTextColor = RGB_WHITE;     
			crBkColor   = RGB_BLACK;        
		}

		// Store the color back in the NMLVCUSTOMDRAW struct.
		pLVCD->clrText = crTextColor;        
		pLVCD->clrTextBk = crBkColor;        

		// Tell Windows to paint the control itself.
		*pResult = CDRF_DODEFAULT;	
	}
}

int CLSMonitorDlg::CreateUDPPort( int iPort )
{
	int iRePort = iPort;
	for (int i = 0; i < 1000 ; i++)
	{
		if( CheckSocketUDPPortUsed( iRePort ) )
			iRePort++;
		else
			break;
	}

	return iRePort;
}
bool CLSMonitorDlg::CheckSocketUDPPortUsed( int iPort )
{
	SOCKET kTestSocket = INVALID_SOCKET;

	kTestSocket = socket( AF_INET, SOCK_DGRAM, 0 );
	if( kTestSocket == INVALID_SOCKET )
		return false;

	SOCKADDR_IN kLocalSockIn;
	memset( &kLocalSockIn, 0, sizeof(kLocalSockIn) );
	kLocalSockIn.sin_family		 = AF_INET;
	kLocalSockIn.sin_addr.s_addr = htonl( INADDR_ANY );
	kLocalSockIn.sin_port		 = htons( iPort );

	if( ::bind( kTestSocket, (LPSOCKADDR)&kLocalSockIn, sizeof(kLocalSockIn) ) == SOCKET_ERROR )
	{
		if( WSAGetLastError() == WSAEADDRINUSE )
		{
			closesocket( kTestSocket );
			return true;
		}
	}

	closesocket( kTestSocket );
	return false;
}

void CLSMonitorDlg::OnTabSelChangeZone(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( !COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
		return;

	ioLocalManager::SetLocalType( m_vZoneInfo[iZoneArray].eLocalType );
	
	SetServerDefaultList( iZoneArray );
	int iSize = (int) m_vZoneInfo[iZoneArray].vServerInfo.size();
	for (int i = 0; i < iSize ; i++)
	{
		UpdateServerListCtrl( &m_vZoneInfo[iZoneArray].vServerInfo[i] );
	}

	SetSystemDefaultList( iZoneArray );
	iSize = (int) m_vZoneInfo[iZoneArray].vSystemInfo.size();
	for (int i = 0; i < iSize ; i++)
	{
		UpdateSystemListCtrl( &m_vZoneInfo[iZoneArray].vSystemInfo[i] );
	}

	// tab
	UpdateBtnCtrl( m_vZoneInfo[ iZoneArray ].gradeType );

	CString szHelp;
	szHelp.Format("(max:%d) %d : %s" , m_vZoneInfo[ iZoneArray ].maxTotalCount, m_vZoneInfo[iZoneArray].iCurrentTotalCount, (LPCTSTR)m_vZoneInfo[iZoneArray].szName );
	SetWindowText(szHelp);
}

CString CLSMonitorDlg::GetCurZoneName()
{
	CString szName;
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
		szName = m_vZoneInfo[iZoneArray].szName;

	return szName;
}

void CLSMonitorDlg::UpdateListItem( int iSMSArray , const char *szText )
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( !COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
		return;

	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SERVER_SMS )
	//if( m_eDlgType == DT_SERVER_SMS )
	{
		bool bUpdate = false;
		const int iMax = (int)m_vZoneInfo[iZoneArray].vServerInfo.size();
		for (int i = 0; i < iMax ; i++)
		{
			SERVERINFO &rkServerInfo = m_vZoneInfo[iZoneArray].vServerInfo[i];
			if( rkServerInfo.wSMSArray == iSMSArray )
			{
				m_ListCtrlServerInfo.SetItemText(rkServerInfo.wListCtrlArray, LIST_ITEM_STATE,  szText);
				m_ListCtrlServerInfo.Update(rkServerInfo.wListCtrlArray);
				bUpdate = true;
				break;
			}
		}

		if( bUpdate )
			return;

		// all update
		for (int i = 0; i < iMax ; i++)
		{
			SERVERINFO &rkServerInfo = m_vZoneInfo[iZoneArray].vServerInfo[i];
			m_ListCtrlServerInfo.SetItemText(rkServerInfo.wListCtrlArray, LIST_ITEM_STATE,  szText);
			m_ListCtrlServerInfo.Update(rkServerInfo.wListCtrlArray);
		}
	}
	else if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SYSTEM_SMS )
	//else if( m_eDlgType == DT_SYSTEM_SMS )
	{	
		bool bUpdate = false;
		const int iMax = (int)m_vZoneInfo[iZoneArray].vSystemInfo.size();
		for (int i = 0; i < iMax ; i++)
		{
			SYSTEMINFO &rkSystemInfo = m_vZoneInfo[iZoneArray].vSystemInfo[i];
			if( rkSystemInfo.wSMSArray == iSMSArray )
			{
				m_ListCtrlSystemInfo.SetItemText(rkSystemInfo.wListCtrlArray, LIST_SYSTEM_ITEM_STATE,  szText);
				m_ListCtrlSystemInfo.Update(rkSystemInfo.wListCtrlArray);
				bUpdate = true;
				break;
			}
		}

		if( bUpdate )
			return;

		// all update
		for (int i = 0; i < iMax ; i++)
		{
			SYSTEMINFO &rkSystemInfo = m_vZoneInfo[iZoneArray].vSystemInfo[i];
			m_ListCtrlSystemInfo.SetItemText(rkSystemInfo.wListCtrlArray, LIST_SYSTEM_ITEM_STATE,  szText);
			m_ListCtrlSystemInfo.Update(rkSystemInfo.wListCtrlArray);
		}
	}
}


void CLSMonitorDlg::SetSMSComboList( OUT CComboBox &rComboBox )
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( !COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
		return;

	rComboBox.ResetContent();
	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SERVER_SMS )
	//if( m_eDlgType == DT_SERVER_SMS )
	{
		const int iMax =  (int)m_vZoneInfo[iZoneArray].vServerInfo.size();
		for (int i = 0; i < iMax ; i++)
		{
			SERVERINFO &kServerInfo = m_vZoneInfo[iZoneArray].vServerInfo[i];
			CString szPort;
			szPort.Format("%d",kServerInfo.iPort);
			CString szAddString = kServerInfo.szIP + " : " + szPort + " : " + kServerInfo.szName;
			rComboBox.AddString(szAddString);
		}
		rComboBox.AddString( "All" );
		rComboBox.SetCurSel(0);
	}
	else if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SYSTEM_SMS )
	//else if( m_eDlgType == DT_SYSTEM_SMS )
	{
		const int iMax =  (int)m_vZoneInfo[iZoneArray].vSystemInfo.size();
		for (int i = 0; i < iMax ; i++)
		{
			SYSTEMINFO &kSystemInfo = m_vZoneInfo[iZoneArray].vSystemInfo[i];
			CString szAddString = kSystemInfo.szIP + " : " + kSystemInfo.szName;
			rComboBox.AddString(szAddString);
		}
		rComboBox.AddString( "All" );
		rComboBox.SetCurSel(0);
	}

}


void CLSMonitorDlg::SetSystemSMSErrorToNormal( int iSMSArray )
{
	/*if( m_eDlgType == DT_SYSTEM_SMS )
	{
		if( m_pSMSDlg )
			m_pSMSDlg->SetErrorToNormal( iSMSArray );
	}*/

	for( int i = 0 ; i < m_vZoneInfo.size() ; ++i )
	{
		if( m_vZoneInfo[ i ].gradeType == DT_SYSTEM_SMS )
		//if( m_eDlgType == DT_SYSTEM_SMS )
		{
			if( m_pSMSDlg )
				m_pSMSDlg->SetErrorToNormal( iSMSArray );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LONG CLSMonitorDlg::OnTCPSocket1(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET1);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}

	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket2(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET2);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket3(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET3);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket4(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET4);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket5(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET5);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket6(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET6);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket7(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET7);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket8(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET8);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket9(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET9);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket10(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET10);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket11(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET11);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket12(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET12);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket13(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET13);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket14(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET14);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket15(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET15);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket16(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET16);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket17(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET17);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket18(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET18);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket19(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET19);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket20(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET20);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket21(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET21);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket22(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET22);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket23(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET23);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket24(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET24);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket25(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET25);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket26(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET26);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket27(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET27);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket28(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET28);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket29(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET29);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket30(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET30);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket31(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET31);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket32(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET32);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket33(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET33);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket34(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET34);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket35(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET35);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket36(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET36);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket37(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET37);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket38(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET38);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket39(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET39);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket40(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET40);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket41(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET41);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket42(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET42);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket43(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET43);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket44(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET44);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket45(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET45);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket46(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET46);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket47(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET47);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket48(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET48);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket49(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET49);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnTCPSocket50(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET50);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket51(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET51);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket52(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET52);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket53(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET53);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket54(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET54);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket55(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET55);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket56(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET56);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket57(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET57);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket58(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET58);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket59(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET59);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket60(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET60);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket61(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET61);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket62(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET62);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket63(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET63);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket64(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET64);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket65(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET65);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket66(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET66);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket67(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET67);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket68(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET68);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket69(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET69);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket70(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET70);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket71(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET71);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket72(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET72);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket73(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET73);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket74(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET74);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket75(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET75);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket76(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET76);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket77(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET77);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket78(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET78);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket79(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET79);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket80(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET80);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket81(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET81);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket82(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET82);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket83(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET83);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket84(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET84);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket85(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET85);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket86(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET86);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket87(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET87);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket88(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET88);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket89(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET89);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket90(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET90);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket91(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET91);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket92(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET92);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket93(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET93);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket94(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET94);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket95(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET95);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket96(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET96);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket97(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET97);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket98(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET98);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket99(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET99);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}
LONG CLSMonitorDlg::OnTCPSocket100(WPARAM wParam, LPARAM lParam)
{
	LONG result=0;
	ioSP2TCPSocket* pSocket = g_TCPSocketTable.GetTCPSocket(WM_TCPSOCKET100);
	if(pSocket)
	{
		result = pSocket->EvaluateSocket( wParam, lParam );
	}
	return result; 
}

LONG CLSMonitorDlg::OnUDPSocket(WPARAM wParam, LPARAM lParam)
{
	m_SystemInfoUDPSocket.EvaluateSocket(wParam, lParam);
	return 1;
}

int CLSMonitorDlg::GetCurZoneArray()
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
		return iZoneArray;

	return 0;
}

void CLSMonitorDlg::OnBnClickedBtnAnnounce()
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE || 
		m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE_VERSION || 
		m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE_CONTROL ||
		m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE_CONNECT ||
		m_vZoneInfo[ iZoneArray ].gradeType == DT_DEVELOPER )
	{
		m_AnnounceDlg.ShowWindow( SW_SHOW );
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*if( m_eDlgType == DT_ANNOUNCE || 
		m_eDlgType == DT_ANNOUNCE_VERSION || 
		m_eDlgType == DT_ANNOUNCE_CONTROL ||
		m_eDlgType == DT_ANNOUNCE_CONNECT ||
		m_eDlgType == DT_DEVELOPER )
	{
		m_AnnounceDlg.ShowWindow( SW_SHOW );
	}*/
}

void CLSMonitorDlg::OnBnClickedBtnServerSms()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SERVER_SMS || 
		m_vZoneInfo[ iZoneArray ].gradeType == DT_DEVELOPER )
	{
		if( m_pSMSDlg )
			m_pSMSDlg->ShowWindow( SW_SHOW );
	}
	/*if( m_eDlgType == DT_SERVER_SMS ||
		m_eDlgType == DT_DEVELOPER )
	{
		if( m_pSMSDlg )
			m_pSMSDlg->ShowWindow( SW_SHOW );
	}*/
}

void CLSMonitorDlg::OnBnClickedBtnSystemSms()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SYSTEM_SMS || 
		m_vZoneInfo[ iZoneArray ].gradeType == DT_DEVELOPER )
	{
		if( m_pSMSDlg )
			m_pSMSDlg->ShowWindow( SW_SHOW );
	}

	/*if( m_eDlgType == DT_SYSTEM_SMS ||
		m_eDlgType == DT_DEVELOPER )
	{
		if( m_pSMSDlg )
			m_pSMSDlg->ShowWindow( SW_SHOW );
	}*/
}

int CLSMonitorDlg::GetSMSArray( int iZoneArray, int iListArray )
{
	if( !COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
		return -1;
	
	int iSize = (int) m_vZoneInfo[iZoneArray].vServerInfo.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( m_vZoneInfo[ i ].gradeType == DT_SERVER_SMS )
		{
			SERVERINFO &rkInfo = m_vZoneInfo[iZoneArray].vServerInfo[i];
			if( rkInfo.wListCtrlArray == iListArray )
				return rkInfo.wSMSArray;
		}
		else if( m_vZoneInfo[ i ].gradeType == DT_SYSTEM_SMS )
		{
			SERVERINFO &rkInfo = m_vZoneInfo[iZoneArray].vServerInfo[i];
			if( rkInfo.wListCtrlArray == iListArray )
				return rkInfo.wSMSArray;
		}
	}

	return -1;

	/*if( m_eDlgType == DT_SERVER_SMS  )
	{
		int iSize = (int) m_vZoneInfo[iZoneArray].vServerInfo.size();
		for (int i = 0; i < iSize ; i++)
		{
			SERVERINFO &rkInfo = m_vZoneInfo[iZoneArray].vServerInfo[i];
			if( rkInfo.wListCtrlArray == iListArray )
				return rkInfo.wSMSArray;
		}

		return -1;
	}
	else if( m_eDlgType == DT_SYSTEM_SMS  )
	{
		int iSize = (int) m_vZoneInfo[iZoneArray].vSystemInfo.size();
		for (int i = 0; i < iSize ; i++)
		{
			SYSTEMINFO &rkInfo = m_vZoneInfo[iZoneArray].vSystemInfo[i];
			if( rkInfo.wListCtrlArray == iListArray )
				return rkInfo.wSMSArray;
		}

		return -1;
	}

	return -1;*/
}

int CLSMonitorDlg::GetCurSMSInfoSize( int iZoneArray )
{
	if( !COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
		return 0;

	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SERVER_SMS  )
		return (int)m_vZoneInfo[iZoneArray].vServerInfo.size();
	else if ( m_vZoneInfo[ iZoneArray ].gradeType == DT_SYSTEM_SMS )
		return (int)m_vZoneInfo[iZoneArray].vSystemInfo.size();

	/*if( m_eDlgType == DT_SERVER_SMS  )
		return (int)m_vZoneInfo[iZoneArray].vServerInfo.size();
	else if ( m_eDlgType == DT_SYSTEM_SMS )
		return (int)m_vZoneInfo[iZoneArray].vSystemInfo.size();*/

	return 0;
}

void CLSMonitorDlg::OnBnClickedButtonVersion()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE_VERSION ||
		m_vZoneInfo[ iZoneArray ].gradeType == DT_DEVELOPER )
		m_VersionDlg.ShowWindow( SW_SHOW );

	/*if( m_eDlgType == DT_ANNOUNCE_VERSION ||
		m_eDlgType == DT_DEVELOPER )
		m_VersionDlg.ShowWindow( SW_SHOW );*/
}

void CLSMonitorDlg::SendUDPMsg()
{
	int iMax = (int) m_vZoneInfo.size();
	CREQUEST_SYSTEM_INFO msg;
	msg.Set();
	for (int i = 0; i < iMax; i++)
	{
		int iSize = (int)m_vZoneInfo[i].vSystemInfo.size();
		for (int j = 0; j < iSize ; j++)
		{
			SYSTEMINFO &kInfo = m_vZoneInfo[i].vSystemInfo[j];
			if( kInfo.szIP.IsEmpty() )
				continue;
			if( !kInfo.szURL.IsEmpty() )
				continue;

			if( m_vZoneInfo[ i ].gradeType == DT_SYSTEM_SMS && ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == STOPING)  ) 
				continue;
			/*if(m_eDlgType == DT_SYSTEM_SMS && ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == STOPING)  ) 
				continue;*/

			m_SystemInfoUDPSocket.SendLoop(kInfo.szIP.GetBuffer(0), UDPSOCKET_PORT, (LPSTR)&msg ,sizeof(msg));	
		}
	}
}

void CLSMonitorDlg::SendCopyUDPMsg( OUT CControlDlg::vCheckInfo &rvCheckInfo, CString &rszIP, int iVersion )
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( !COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
	{
		LOG.PrintTimeAndLog( 0, "CLSMonitorDlg::SendCopyUDPMsg Over ZoneArray :%d", iZoneArray );
		return;
	}

	CREQUEST_COPY msg;
	msg.Set( COPY_REQUEST_CS3, iVersion );
	const int iMax = (int)m_vZoneInfo[iZoneArray].vSystemInfo.size();
	for (int i = 0; i < iMax ; i++)
	{
		SYSTEMINFO &rkSystemInfo = m_vZoneInfo[iZoneArray].vSystemInfo[i];
		if( rkSystemInfo.szIP.IsEmpty() )
			continue;
		if( rkSystemInfo.szName.Find( GAME_SERVER_NAME ) == -1 )
			continue;

		// check
		bool bSameIP = false;
		for(CControlDlg::vCheckInfo::iterator iter = rvCheckInfo.begin(); iter != rvCheckInfo.end(); ++iter)
		{
		    CControlDlg::CheckInfo &rCheckInfo = (*iter);
			if( rCheckInfo.m_szSystemIP == rkSystemInfo.szIP )
			{
				bSameIP = true;
				break;
			}
		}
		if( bSameIP )
			continue;

		if( !rszIP.IsEmpty() )
		{
			if( rszIP != rkSystemInfo.szIP )
				continue;
		}

		if( m_SystemInfoUDPSocket.SendLoop( rkSystemInfo.szIP.GetBuffer(0), UDPSOCKET_PORT, (LPSTR) &msg, sizeof( msg ) ) )
		{
			CControlDlg::CheckInfo kCheckInfo;
			kCheckInfo.m_szSystemIP = rkSystemInfo.szIP;
			kCheckInfo.m_iPort = UDPSOCKET_PORT;
			rvCheckInfo.push_back( kCheckInfo );
		}
	}
}

void CLSMonitorDlg::SendDeleteUDPMsg( OUT CControlDlg::vCheckInfo &rvCheckInfo, int iVersion )
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( !COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
	{
		LOG.PrintTimeAndLog( 0, "CLSMonitorDlg::SendDeleteUDPMsg Over ZoneArray :%d", iZoneArray );
		return;
	}

	CREQUEST_DELETE msg;
	msg.Set( DELETE_REQUEST_CS3, iVersion );
	const int iMax = (int)m_vZoneInfo[iZoneArray].vSystemInfo.size();
	for (int i = 0; i < iMax ; i++)
	{
		SYSTEMINFO &rkSystemInfo = m_vZoneInfo[iZoneArray].vSystemInfo[i];
		if( rkSystemInfo.szIP.IsEmpty() )
			continue;
		if( rkSystemInfo.szName.Find( GAME_SERVER_NAME ) == -1 )
			continue;
		if( m_SystemInfoUDPSocket.SendLoop( rkSystemInfo.szIP.GetBuffer(0), UDPSOCKET_PORT, (LPSTR) &msg, sizeof( msg ) ) )
		{
			CControlDlg::CheckInfo kCheckInfo;
			kCheckInfo.m_szSystemIP = rkSystemInfo.szIP;
			kCheckInfo.m_iPort = UDPSOCKET_PORT;
			rvCheckInfo.push_back( kCheckInfo );
			return; // 삭제는 1개의 게임서버에서만 처리하면 됨.
		}
	}
}

void CLSMonitorDlg::SetActiveTab( bool bActive )
{
	if( bActive )
		m_TabCtrlZone.EnableWindow( true );
	else
		m_TabCtrlZone.EnableWindow( false );
}


void CLSMonitorDlg::OnBnClickedDefaultCancel()
{
}

void CLSMonitorDlg::OnBnClickedButtonControl()
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE_CONTROL ||
		m_vZoneInfo[ iZoneArray ].gradeType == DT_DEVELOPER )
		m_ControlDlg.ShowWindow( SW_SHOW );

	/*if( m_eDlgType == DT_ANNOUNCE_CONTROL ||
		m_eDlgType == DT_DEVELOPER )
		m_ControlDlg.ShowWindow( SW_SHOW );*/
}

void CLSMonitorDlg::OnCopyResult( int iResultType, int iRequestType, int iVersion, const CControlDlg::CheckInfo &rReceiveInfo )
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE_CONTROL ||
		m_vZoneInfo[ iZoneArray ].gradeType == DT_DEVELOPER )
		m_ControlDlg.OnCopyResult( iResultType, iRequestType, iVersion, rReceiveInfo );

	/*if( m_eDlgType == DT_ANNOUNCE_CONTROL ||
		m_eDlgType == DT_DEVELOPER )
		m_ControlDlg.OnCopyResult( iResultType, iRequestType, iVersion, rReceiveInfo );*/
}

void CLSMonitorDlg::OnDeleteResult( int iResultType, int iRequestType, int iVersion, const CControlDlg::CheckInfo &rReceiveInfo )
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE_CONTROL ||
		m_vZoneInfo[ iZoneArray ].gradeType == DT_DEVELOPER )
		m_ControlDlg.OnDeleteResult( iResultType, iRequestType, iVersion, rReceiveInfo );

	/*if( m_eDlgType == DT_ANNOUNCE_CONTROL ||
		m_eDlgType == DT_DEVELOPER )
		m_ControlDlg.OnDeleteResult( iResultType, iRequestType, iVersion, rReceiveInfo );*/
}

void CLSMonitorDlg::GetLoadCheckFileInfo( OUT CControlDlg::vCheckInfo &rvCheckInfo, IN CString &rszOnlyIP )
{
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( !COMPARE( iZoneArray, 0, (int) m_vZoneInfo.size() ) )
	{
		LOG.PrintTimeAndLog( 0, "CLSMonitorDlg::GetLoadCheckFileInfo Over ZoneArray :%d", iZoneArray );
		return;
	}

	const int iMax = (int)m_vZoneInfo[iZoneArray].vServerInfo.size();
	for (int i = 0; i < iMax ; i++)
	{
		SERVERINFO &rkServerInfo = m_vZoneInfo[iZoneArray].vServerInfo[i];
		if( rkServerInfo.szIP.IsEmpty() )
			continue;

		if( !rszOnlyIP.IsEmpty() )
		{
			if( rszOnlyIP != rkServerInfo.szIP )
				continue;
		}

		if( rkServerInfo.serverType == eServerType_MainServer )
			continue;

		CControlDlg::CheckInfo kCheckInfo;
		kCheckInfo.m_szServerIP = rkServerInfo.szIP;
		kCheckInfo.m_iPort = rkServerInfo.iPort;
		rvCheckInfo.push_back( kCheckInfo );
	}
}

void CLSMonitorDlg::GetMyIP( OUT ioHashString &rszIP )
{
	rszIP = m_SystemInfoUDPSocket.GetMyIP();
}

void CLSMonitorDlg::OnBnClickedBtnConnectLog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SYSTEMTIME st;
	GetLocalTime(&st);

	CString szName;
	char szLogName[MAX_PATH]  = "";
	char szExecName[MAX_PATH] = "";
	StringCbPrintf( szLogName, sizeof( szLogName ), "Mlog\\CONNECT%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay );
	StringCbPrintf( szExecName, sizeof(szExecName), "notepad.exe %s", szLogName );
	CFile file;
	if(!file.Open( szLogName, CFile::modeRead | CFile::shareDenyNone))
	{
		AfxMessageBox( "금일 날짜 로그가 없습니다." );
		return;
	}	
	file.Close();

	WinExec( szExecName, SW_SHOW );
}

void CLSMonitorDlg::HideChildBtns()
{
	GetDlgItem( ID_BTN_SERVER_SMS )->ShowWindow( SW_HIDE );
	GetDlgItem( ID_BTN_SYSTEM_SMS )->ShowWindow( SW_HIDE );
	GetDlgItem( ID_BTN_ANNOUNCE )->ShowWindow( SW_HIDE );
	GetDlgItem( ID_BTN_CONTROL )->ShowWindow( SW_HIDE );
	GetDlgItem( ID_BTN_VERSION )->ShowWindow( SW_HIDE );
	GetDlgItem( ID_BTN_CONNECT_LOG )->ShowWindow( SW_HIDE );
}


void CLSMonitorDlg::SendWebServer()
{
	static int i10secondsCnt = 12;
	if(i10secondsCnt >= 12)
	{
		i10secondsCnt = 0;
		//
		int iMax = (int) m_vZoneInfo.size();
		for (int i = 0; i < iMax; i++)
		{
			int iSize = (int)m_vZoneInfo[i].vSystemInfo.size();
			for (int j = 0; j < iSize ; j++)
			{
				SYSTEMINFO &kInfo = m_vZoneInfo[i].vSystemInfo[j];
				if( kInfo.szURL.IsEmpty() )
					continue;

				if( m_vZoneInfo[ i ].gradeType == DT_SYSTEM_SMS && ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == STOPING)  ) 
					continue;
				/*if(m_eDlgType == DT_SYSTEM_SMS && ( m_pSMSDlg && m_pSMSDlg->GetState(kInfo.wSMSArray) == STOPING)  ) 
					continue;*/

				int iArray = (iMax * i) + j;
				if( COMPARE( iArray, 0, (int) m_vHttpConnectorVec.size() ) )
					m_vHttpConnectorVec[iArray]->SetURL( kInfo.szURL );
			}
		}
		//
	}
	i10secondsCnt++;
}

// receive
void CLSMonitorDlg::OnNumConnect( UINT wndMsg, SP2Packet &rkPacket )
{
	int iZoneArray = wndMsg - WM_TCPSOCKET1;
	if( !COMPARE( iZoneArray, 0, (int)m_vZoneInfo.size() ) )
		return;
	int iTotalConnet = 0;
	int iConnectServerSize = 0;
	rkPacket >> iConnectServerSize;
	for(int i = 0;i < iConnectServerSize;i++)
	{
		SERVERINFO *pServerInfo = GetServerInfo( &m_vZoneInfo[iZoneArray], rkPacket );
		if( pServerInfo )
		{
			WORD wCurDelaySec = 0;
			rkPacket >> pServerInfo->wConnect >> pServerInfo->wRoomCount >> pServerInfo->wPlazaCount 
				>> pServerInfo->wBattleRoomCount >> pServerInfo->wPingMS >> pServerInfo->wDBQueryMs >> wCurDelaySec;

			if( ( wCurDelaySec*1000 ) >= NO_REACTION_TIME )
				continue;

			// main server 카운트는 뺌.
			if( pServerInfo->serverType == eServerType_GameServer )
				iTotalConnet += pServerInfo->wConnect;

			pServerInfo->dwLastRcvTime = timeGetTime();
			pServerInfo->bReaction     = true;

			if( iZoneArray == m_TabCtrlZone.GetCurSel() )
				UpdateServerListCtrl( pServerInfo );

			if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SERVER_SMS )
			//if( m_eDlgType == DT_SERVER_SMS )
			{
				if( m_pSMSDlg )
					m_pSMSDlg->SetErrorToNormal(pServerInfo->wSMSArray);
			}
		}
		else
		{
			WORD wTemp1,wTemp2,wTemp3,wTemp4,wTemp5,wTemp6,wTemp7;
			rkPacket >> wTemp1 >> wTemp2 >> wTemp3 >> wTemp4 >> wTemp5 >> wTemp6 >> wTemp7 ;
		}
	}

	if( iZoneArray == m_TabCtrlZone.GetCurSel() )
	{
		// max 기록.
		if( m_vZoneInfo[ iZoneArray ].maxTotalCount < iTotalConnet )
		{
			m_vZoneInfo[ iZoneArray ].maxTotalCount = iTotalConnet;

			// log : file.
			_OnTimerCreateNewLog( MaxLog, "MaxLog", m_szMaxLogPrevTime, sizeof( m_szMaxLogPrevTime ) );

			MaxLog.PrintTimeAndLog( 0, "%s : %d (max connect)", (LPCTSTR)m_vZoneInfo[ iZoneArray ].szName, m_vZoneInfo[ iZoneArray ].maxTotalCount );
		}

		CString szHelp;
		szHelp.Format("(max:%d) %d : %s" , m_vZoneInfo[ iZoneArray ].maxTotalCount, iTotalConnet, (LPCTSTR)m_vZoneInfo[iZoneArray].szName );
		SetWindowText(szHelp);
		m_vZoneInfo[iZoneArray].iCurrentTotalCount = iTotalConnet;
	}
}

void CLSMonitorDlg::OnServerDisconnect( UINT wndMsg, SP2Packet &rkPacket )
{
	int iConnectArray = wndMsg - WM_TCPSOCKET1;
	SERVERINFO *pServerInfo = GetServerInfo( &m_vZoneInfo[iConnectArray], rkPacket );
	if( pServerInfo )
	{						
		pServerInfo->dwLastRcvTime = 0;
		_OnTimerUpdateNoReaciton();					
	}
}

void CLSMonitorDlg::OnLoadCS3File( UINT wndMsg, SP2Packet &rkPacket )
{
	int iZoneArray = wndMsg - WM_TCPSOCKET1;
	if( !COMPARE( iZoneArray, 0, (int)m_vZoneInfo.size() ) )
	{
		LOG.PrintTimeAndLog( 0, "CLSMonitorDlg::ProcessPacket : Over ZoneArray :%d:0x%x", iZoneArray, STPKM_LOAD_CS3_FILE );
		return;
	}
	int iResultType = 0;
	int iVersion    = 0;
	ioHashString szIP;
	int iPort = 0;
	rkPacket >> iResultType;
	rkPacket >> szIP;
	rkPacket >> iPort;
	rkPacket >> iVersion;
	CControlDlg::CheckInfo kCheckInfo;
	kCheckInfo.m_szServerIP  = szIP.c_str();
	kCheckInfo.m_iPort = iPort;

	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE_CONTROL ||
		m_vZoneInfo[ iZoneArray ].gradeType == DT_DEVELOPER )
		m_ControlDlg.OnLoadResult( iResultType, iVersion, kCheckInfo );

	/*if( m_eDlgType == DT_ANNOUNCE_CONTROL ||
		m_eDlgType == DT_DEVELOPER )
		m_ControlDlg.OnLoadResult( iResultType, iVersion, kCheckInfo );*/
}

void CLSMonitorDlg::OnCS3FileVersion( UINT wndMsg, SP2Packet &rkPacket )
{
	int iZoneArray = wndMsg - WM_TCPSOCKET1;
	if( !COMPARE( iZoneArray, 0, (int)m_vZoneInfo.size() ) )
	{
		LOG.PrintTimeAndLog( 0, "CLSMonitorDlg::ProcessPacket : Over ZoneArray :%d:0x%x", iZoneArray, STPKM_CS3_FILE_VERSION );
		return;
	}
	ioHashString szIP;
	int iPort = 0;
	rkPacket >> szIP;
	rkPacket >> iPort;
	CControlDlg::CheckInfo kCheckInfo;
	kCheckInfo.m_szServerIP  = szIP.c_str();
	kCheckInfo.m_iPort = iPort;

	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_ANNOUNCE_CONTROL ||
		m_vZoneInfo[ iZoneArray ].gradeType == DT_DEVELOPER )
		m_ControlDlg.OnInfoResult( CS3_VERSION_SUCCESS, kCheckInfo, rkPacket );

	//if( m_eDlgType == DT_ANNOUNCE_CONTROL ||
	//	m_eDlgType == DT_DEVELOPER )
	//	m_ControlDlg.OnInfoResult( CS3_VERSION_SUCCESS, kCheckInfo, rkPacket ); // 실패하는 경우가 없이 서버에서 타입을 받지 않고 로컬에서만 처리함 타입이 필요하면 서버에서 받아 올것
}

void CLSMonitorDlg::OnAllServerInfo( UINT wndMsg, SP2Packet &rkPacket )
{
	/*if( m_eDlgType == DT_SERVER_SMS || m_eDlgType == DT_SYSTEM_SMS )
	{
		return;
	}*/

	int iZoneArray = wndMsg - WM_TCPSOCKET1;
	if( !COMPARE( iZoneArray, 0, (int)m_vZoneInfo.size() ) )
		return;

	if( m_vZoneInfo[ iZoneArray ].gradeType == DT_SERVER_SMS || m_vZoneInfo[ iZoneArray ].gradeType == DT_SYSTEM_SMS )
		return;

	bool bUsePrivateIP = false;
	int  iServerSize   = 0;
	rkPacket >> bUsePrivateIP;
	rkPacket >> iServerSize;

	int iSystemArray = (int)m_vZoneInfo[iZoneArray].vSystemInfo.size();
	int iSystemServerArray = 0;
	m_vZoneInfo[iZoneArray].vServerInfo.clear();

	for (int i = 0; i < iServerSize ; i++)
	{
		// data
		ioHashString szPublicIP;
		ioHashString szPrivateIP;
		int iClientPort = 0;

		rkPacket >> szPublicIP;
		rkPacket >> szPrivateIP;
		rkPacket >> iClientPort;

		// server
		SERVERINFO kServerInfo;
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "GameProcess%03d", i+1 );
		kServerInfo.szName = szName;
		kServerInfo.szIP   = szPrivateIP.c_str();
		kServerInfo.iPort  = iClientPort;
		kServerInfo.wListCtrlArray = i;
		kServerInfo.serverType = eServerType_GameServer;
		m_vZoneInfo[iZoneArray].vServerInfo.push_back( kServerInfo );	

		// system
		bool bExist = false;
		int iSystemSize = (int)m_vZoneInfo[iZoneArray].vSystemInfo.size();
		for (int j = 0; j < iSystemSize ; j++)
		{
			if( bUsePrivateIP )
			{
				if( m_vZoneInfo[iZoneArray].vSystemInfo[j].szIP == szPrivateIP.c_str() )
				{
					bExist = true;
					break;
				}
			}
			else
			{
				if( m_vZoneInfo[iZoneArray].vSystemInfo[j].szIP == szPublicIP.c_str() )
				{
					bExist = true;
					break;
				}
			}
		}

		if( !bExist )
		{
			char szName[MAX_PATH]="";
			StringCbPrintf( szName, sizeof( szName ), "%s%03d", GAME_SERVER_NAME, iSystemServerArray+1 );
			iSystemServerArray++;

			SYSTEMINFO kInfo;
			kInfo.szName = szName;
			if( bUsePrivateIP )
				kInfo.szIP = szPrivateIP.c_str();
			else
				kInfo.szIP = szPublicIP.c_str();
			kInfo.wListCtrlArray = iSystemArray;
			iSystemArray++;
			m_vZoneInfo[iZoneArray].vSystemInfo.push_back( kInfo );
		}
	}

	// main
	{
		SERVERINFO kServerInfo;
		ioHashString	mainIP;
		int				mainPort;

		rkPacket >> mainIP >> mainPort;

		kServerInfo.szName	= "Main Server";
		kServerInfo.szIP	= mainIP.c_str();
		kServerInfo.iPort	= mainPort;
		kServerInfo.serverType = eServerType_MainServer;
		kServerInfo.wListCtrlArray = m_vZoneInfo[ iZoneArray ].vServerInfo.size();

		m_vZoneInfo[ iZoneArray ].vServerInfo.push_back( kServerInfo );
	}

	if( iZoneArray == m_TabCtrlZone.GetCurSel() )
	{
		SetServerDefaultList( iZoneArray );
		SetSystemDefaultList( iZoneArray );
	}

	LOG.PrintTimeAndLog( 0, "Set All Server [%d:%d:%d]", iZoneArray, iServerSize, iSystemServerArray );
}

void CLSMonitorDlg::OnServerInfoAck( UINT wndMsg, SP2Packet &rkPacket )
{
	int iZoneArray = wndMsg - WM_TCPSOCKET1;
	if( !COMPARE( iZoneArray, 0, (int)m_vZoneInfo.size() ) )
		return;
	
	if( m_pServerInfoDlg != NULL )
	{
		int ServerType = eServerType_Default;
		rkPacket >> ServerType;

		if( ServerType == eServerType_MainServer )
			m_pServerInfoDlg->PrintMainserverTextOut( rkPacket );
		else if( ServerType == eServerType_GameServer )
			m_pServerInfoDlg->PrintGameServerTextOut( rkPacket );

		m_pServerInfoDlg->ShowWindow( SW_SHOW );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "m_pServerInfoDlg is NULL..." );
	}
}
void CLSMonitorDlg::OnNMClickServerInfo( NMHDR *pNMHDR, LRESULT *pResult )
{
	 
	//m_ListCtrlServerInfo.SetBkColor(RGB(1680,184,208)); //kyg 추가

}
void CLSMonitorDlg::OnNMRClickServerInfo( NMHDR *pNMHDR, LRESULT *pResult )
{
	// Check : 개발자 아니면 return
	/*if( m_eDlgType != DT_DEVELOPER )
		return;*/
	int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( m_vZoneInfo[ iZoneArray ].gradeType != DT_DEVELOPER )
		return;

	CPoint pos;
	GetCursorPos( &pos );

	CMenu temp;
	temp.LoadMenu( IDR_MENU );
	CMenu* pContextMenu;

	// 선택한 컬럼의 index
	m_selectIndex = (int)m_ListCtrlServerInfo.GetFirstSelectedItemPosition();

	CString strName = m_ListCtrlServerInfo.GetItemText( m_selectIndex - 1, 0 );
	if( strName == "Main Server" || strName == "로컬존" )
	{
		pContextMenu = temp.GetSubMenu( eServerType_MainServer - 1 );
		if( pContextMenu == NULL )
			return;
	}
	else if( strName =="Login Server")
	{
		pContextMenu = temp.GetSubMenu( eServerType_LoginServer - 1 );
		if( pContextMenu == NULL )
			return;

	}
	else
	{
		pContextMenu = temp.GetSubMenu( eServerType_GameServer - 1 );
		if( pContextMenu == NULL )
			return;
	}

	// 띄우기
	pContextMenu->TrackPopupMenu( TPM_LEFTALIGN, pos.x, pos.y, this );
}

void CLSMonitorDlg::OnMainServerInfoDrawReq()
{
	if( m_selectIndex <= 0 || m_selectIndex > m_ListCtrlServerInfo.GetItemCount() )
		return;

	// tab Index
	int tabIndex = m_TabCtrlZone.GetCurSel();
	if( tabIndex >= (int)m_vZoneInfo.size() )
		return;

	ioHashString	szip;
	int port = 0;

	szip = m_vZoneInfo[ tabIndex ].vServerInfo[ m_selectIndex - 1 ].szIP;
	port = m_vZoneInfo[ tabIndex ].vServerInfo[ m_selectIndex - 1 ].iPort;

	SP2Packet kPacket( CTPKM_SERVER_INFO_REQUEST );
	kPacket << szip << port;
	
	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}

// Get ip & port
void CLSMonitorDlg::GetSelectedServerInfo( ioHashString& ip, int& port )
{
	if( m_selectIndex <= 0 || m_selectIndex > m_ListCtrlServerInfo.GetItemCount() )
		return;

	// tab Index
	int tabIndex = m_TabCtrlZone.GetCurSel();
	if( tabIndex >= (int)m_vZoneInfo.size() )
		return;

	ip		= m_vZoneInfo[ tabIndex ].vServerInfo[ m_selectIndex - 1 ].szIP;
	port	= m_vZoneInfo[ tabIndex ].vServerInfo[ m_selectIndex - 1 ].iPort;
}


// Get ip & port
void CLSMonitorDlg::GetSelectedLoginInfo( ioHashString& ip, int& port )
{
	if( m_selectIndex <= 0 || m_selectIndex > m_ListCtrlServerInfo.GetItemCount() )
		return;
	int nindex = m_ListCtrlServerInfo.GetItemCount();
 
 
	ip = m_ListCtrlServerInfo.GetItemText(nindex-1,1);
	port = atoi(m_ListCtrlServerInfo.GetItemText(nindex-1,2));

	 
}


//==========================================================================
// MainServer
void CLSMonitorDlg::OnGameserverInfodraw()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_SERVER_INFO_REQUEST );
	kPacket << szip << port;
	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}

void CLSMonitorDlg::OnMainserverMainserverexit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( AfxMessageBox( "MainServer Exit ?", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket( CTPKM_MAINSERVER_EXIT );
		g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
	}
}

void CLSMonitorDlg::OnMainserverQuickallexit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( AfxMessageBox( "GameServer QuickAll Exit ?", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket( CTPKM_MAINSERVER_QUICKALLEXIT );
		
		DWORD	dwType = ALL_SERVER_QUICK_EXIT;	// quick
		//DWORD	dwType = 32788;	// quick
		kPacket << dwType;
		g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
	}
}

void CLSMonitorDlg::OnMainserverSafetyallexit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( AfxMessageBox( "GameServer SafetyAll Exit ?", MB_YESNO ) == IDYES )
	{
		SP2Packet kPacket( CTPKM_MAINSERVER_SAFETYALLEXIT );

		DWORD	dwType = ALL_SERVER_SAFETY_EXIT;	// safety
		//DWORD	dwType = 32790;	// safety
		kPacket << dwType;
		g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
	}
}

void CLSMonitorDlg::OnMainserverReloadcloseinfo()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SP2Packet kPacket( CTPKM_MAINSERVER_RELOADCLOSEINFO );
	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}

void CLSMonitorDlg::OnMainserverDbagentextend()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SP2Packet kPacket( CTPKM_MAINSERVER_DBAGENT_EXTEND );
	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}

void CLSMonitorDlg::OnMainserverGameserveroption()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SP2Packet kPacket( CTPKM_MAINSERVER_GAMESERVER_OPTION );
	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}

void CLSMonitorDlg::OnMainserverGsreloadini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SP2Packet kPacket( CTPKM_MAINSERVER_GAMESERVER_RELOADINI );
	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}

void CLSMonitorDlg::OnMainserverGssetnaglerefcount()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CNagleRefCountDlg Dlg;
	if( Dlg.DoModal() == IDOK )
	{
		int refCount = Dlg.GetRefCount();

		SP2Packet kPacket( CTPKM_MAINSERVER_GS_SETNAGLE_REFCOUNT );
		kPacket << refCount;
		g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
	}
}

void CLSMonitorDlg::OnMainserverGssetnagletime()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CNagleRefCountDlg Dlg;
	if( Dlg.DoModal() == IDOK )
	{
		uint32 refCount = Dlg.GetRefCount();

		SP2Packet kPacket( CTPKM_MAINSERVER_GS_SETNAGLE_TIME );
		kPacket << refCount;
		g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
	}
}

void CLSMonitorDlg::OnMainserverExtraitemini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SP2Packet kPacket( CTPKM_MAINSERVER_EXTRAITEM_INI );
	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}

void CLSMonitorDlg::OnMainserverEventshopini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	SP2Packet kPacket( CTPKM_MAINSERVER_EVENTSHOP_INI );
	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


//==================================================================
// GameServer
void CLSMonitorDlg::OnGameserverReloadhackconstant()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_HACK_CONSTANT;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloaduserdispersion()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_USER_DISPERSION;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadprocessini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_PROCESS_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadiniclass()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_INI_CLASS;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadinideco()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_INI_DECO;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadinietc()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_INI_ETC;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadinievent()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_INI_EVENT;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadquestini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_QUEST_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadmodeini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_MODE_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadpresentini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_PRESENT_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadfishingini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_FISHING_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadexca()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_EXCA;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloaditemcompoundini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_ITEM_COMPOUND_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadextraitemini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_EXTRAITEM_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadtradeini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_TRADE_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadlevelmatchini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;
	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_LEVELMATCH_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloaditeminitcontrol()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_ITEM_INIT_CONTROL;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverReloadconfigini()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;

	// Get ip/port
	GetSelectedServerInfo( szip, port );

	SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
	kPacket << szip << port;
	kPacket << (DWORD)CTPKM_GAMESERVER_RELOAD_CONFIG_INI;

	g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
}


void CLSMonitorDlg::OnGameserverQuickexit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( AfxMessageBox( "GameServer Quick Exit ?", MB_YESNO ) == IDYES )
	{
		ioHashString	szip;
		int port = 0;

		// Get ip/port
		GetSelectedServerInfo( szip, port );

		SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
		kPacket << szip << port;
		kPacket << (DWORD)CTPKM_GAMESERVER_QUICK_EXIT;

		g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
	}
}


void CLSMonitorDlg::OnGameserverSafetyexit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( AfxMessageBox( "GameServer SafetyAll Exit ?", MB_YESNO ) == IDYES )
	{
		ioHashString	szip;
		int port = 0;

		// Get ip/port
		GetSelectedServerInfo( szip, port );

		SP2Packet kPacket( CTPKM_GAMESERVER_PROTOCAL );
		kPacket << szip << port;
		kPacket << (DWORD)CTPKM_GAMESERVER_SAFETY_EXIT;

		g_TCPSocketTable.Send( WM_TCPSOCKET1 + GetCurZoneArray(), kPacket );
	}
}

void CLSMonitorDlg::OnLoginServerInfodraw()
{
	ioHashString	szip;
	int port = 0;
	GetSelectedLoginInfo( szip,port);

	for(int i=0; i<(int)m_vLoginPanel.size(); ++i)
	{
		CLoingPanel* pdlg = m_vLoginPanel[i];
		 if(strcmp(szip.c_str() , pdlg->ipaddr.c_str()) == 0 &&
			 port == pdlg->port)
			m_vLoginPanel[i]->ShowWindow(SW_SHOW);
		 
		
	}
}

void CLSMonitorDlg::SendMonitorPacekt()
{
	for(int i=0; i<(int)m_vloginserverinfo.size();++i)
	{
		if(strcmp(m_vloginserverinfo[i].szName,LVSVER) == 0 &&
			m_vloginserverinfo[i].nstate == 1)
		{
			m_vloginserverinfo[i].nstate = 2;
			SP2Packet pk(EPROTOCOL::LSPTK_CONNECT_MONITOR);
			g_TCPSocketTable.Send(WM_TCPLOGIN1+i,pk);

			SP2Packet pkk(EPROTOCOL::LSPTK_CONTROL_SERVER);
			int type = EMCONTROLTYPE::LS_GETSERVERINFO;
			pkk << type;
			g_TCPSocketTable.Send(WM_TCPLOGIN1+i,pkk);

		}
	}
}

void CLSMonitorDlg::SendReqSvrInfoPacekt()
{
	for(int i=0; i<(int)m_vloginserverinfo.size();++i)
	{
		if(strcmp(m_vloginserverinfo[i].szName,LVSVER) == 0)
		{
 
			SP2Packet pk(EPROTOCOL::LSPTK_CONTROL_SERVER);
			int type = EMCONTROLTYPE::LS_FILLSERVERINFO;
			pk << type;
			g_TCPSocketTable.Send(WM_TCPLOGIN1+i,pk);
		}
	}
}

void CLSMonitorDlg::OnLSResPonse(UINT wndMsg, SP2Packet & rkPacket )
{
	int type;
	rkPacket >> type;
	switch(type)
	{
	case EMCONTROLTYPE::LS_GETSERVERINFO:
		{
			OnGetServerInfo(wndMsg,rkPacket);


		}
		break;
	case EMCONTROLTYPE::LS_FILLSERVERINFO:
		{
			OnFillServerInfo(wndMsg,rkPacket);


		}
		break;
	case EMCONTROLTYPE::LS_SETSERVERBLOCK:
		{
			SetServerBlock(wndMsg,rkPacket, type);

		}
		break;
	case EMCONTROLTYPE::LS_SETALLSERVERBLOCK:
		{
			TCHAR ipaddr[STR_IP_MAX];
			int blockstate;
			rkPacket >> ipaddr;
			rkPacket >> blockstate;
			for(int i=0; i<m_vLoginPanel.size();++i)
			{
				if(strcmp(m_vLoginPanel[i]->ipaddr.c_str(),ipaddr) == 0)
				{
					m_vLoginPanel[i]->PostMessageA(WM_LSLOGINSERVER,type,blockstate);
				}
			}

		}
		break;
	case EMCONTROLTYPE::LS_FILLINFODRAW:
		{
			m_pServerInfoDlg->PrintLoginServerTextOut( rkPacket );
			m_pServerInfoDlg->ShowWindow( SW_SHOW );
		}
		break;
	}

 
}

void CLSMonitorDlg::OnGetServerInfo( UINT wndMsg, SP2Packet & rkPacket )
{
	CString tmp;
	int maxuser;
	int maxserver;
	int waituser;
	int maxwait;
	int allblock;
	rkPacket >> maxuser;
	rkPacket >> maxserver;
	rkPacket >> waituser;
	rkPacket >>maxwait;
	rkPacket >> allblock;
	TCHAR ipaddr[STR_IP_MAX];
	rkPacket >> ipaddr;
	for(int i=0; i<(int)m_vLoginPanel.size();++i)
	{
		CLoingPanel* pdlg = m_vLoginPanel[i];

		if(strcmp(pdlg->ipaddr.c_str(),ipaddr) == 0 &&
			pdlg->wmid == wndMsg)
		{
			if(pdlg->m_bInit == false)
			{
				pdlg->m_blockstate = allblock;
				pdlg->m_bInit = true;
				for(int j =0; j<maxserver; ++j)
				{
					ServerInfo_ sttmp;
				 
					TCHAR nametmp[STR_IP_MAX];
					int   sendserveridtmp;
					int blocktmp;
					TCHAR ipaddrtmp[STR_IP_MAX];
					int porttmp;
				    rkPacket >> sendserveridtmp;
					rkPacket >> nametmp;
					rkPacket >> blocktmp;
					rkPacket >> ipaddrtmp;
					rkPacket >> porttmp;
					sttmp.wmid = wndMsg;
					sttmp.sendserverid = sendserveridtmp;
					sttmp.fullcount = maxuser;
					sttmp.servername = nametmp;
					sttmp.usercount = 0;
					sttmp.sendcount = 0;
					sttmp.serverblockstate = blocktmp;
					sttmp.ipaddr = ipaddrtmp;
					sttmp.port = porttmp;
					sttmp.nstate = -1;
					pdlg->m_vsvrInfo.push_back(sttmp);
					

				}
				pdlg->Init();

			}

		}
		
	}
}

void CLSMonitorDlg::OnFillServerInfo( UINT wndMsg, SP2Packet & rkPacket )
{
	int maxuser;
	int maxserver;
	int waituser;
	int maxwait;
	TCHAR ipaddr[STR_IP_MAX];
	rkPacket >> maxuser;
	rkPacket >> maxserver;
	rkPacket >> waituser;
	rkPacket >> maxwait;
	rkPacket >> ipaddr;

	for(int j = 0; j<(int)m_vLoginPanel.size();++j)
	{
		CLoingPanel* pdlg = m_vLoginPanel[j];
		if(strcmp(pdlg->ipaddr.c_str(),ipaddr) == 0 &&
			pdlg->wmid == wndMsg)
		{
			CString tmp;
			tmp.Format("( %04d/%04d )::LoginServer(%s:%d)",waituser,maxwait,pdlg->ipaddr.c_str(),pdlg->port);
			m_vLoginPanel[j]->SetWindowText(tmp);

			for(int x = 0; x<(int)pdlg->m_vsvrInfo.size(); ++x)
			{
				ServerInfo_& ptmp = pdlg->m_vsvrInfo[x];
				ptmp.nstate = -1;
			}
			for(int i=0; i<maxserver; ++i)
			{

				int sendserveridtmp;	int tmpusercount;int tmpsendcount;int serverstate;
				rkPacket >> sendserveridtmp ;  
				rkPacket >> tmpusercount; rkPacket >>tmpsendcount;
				rkPacket >> serverstate;
				for(int x = 0; x<(int)pdlg->m_vsvrInfo.size(); ++x)
				{
					ServerInfo_& ptmp = pdlg->m_vsvrInfo[x];
					if(ptmp.sendserverid == sendserveridtmp)
					{
						ptmp.fullcount = maxuser;
						ptmp.sendcount = tmpsendcount;
						ptmp.usercount = tmpusercount;
						ptmp.nstate = serverstate;

					}
				}
			}


		}
		pdlg->PostMessage(WM_LSLOGINSERVER,UPDATESVRINFO,UPDATESVRINFO);
	}
}

LONG CLSMonitorDlg::SendPacket2LoginServer( WPARAM wParam,LPARAM lParam )
{
	 
	SP2Packet *pk = (SP2Packet*)wParam;


	for(int i=0; i<(int)m_vloginserverinfo.size();++i)
	{
		if(strcmp(m_vloginserverinfo[i].szName,LVSVER) == 0)
		{
			g_TCPSocketTable.Send(WM_TCPLOGIN1+i,*pk);
		}
	}

	delete pk;
	return 0;
}

void CLSMonitorDlg::SetServerBlock(UINT wndMsg, SP2Packet & rkPacket, int type )
{
	TCHAR ipaddr[STR_IP_MAX];
	int port;
	int blockstate;
//	rkPacket >> ipaddr;
//	rkPacket >> port;
	int sendserverid;
	rkPacket >> sendserverid;
	rkPacket >> blockstate;
	for(int i=0; i< m_vLoginPanel.size(); ++i)
	{
		CLoingPanel* pdlg = m_vLoginPanel[i];
		for(int j=0; j< m_vLoginPanel[i]->m_vinfodlg.size();++j)
		{
			if(m_vLoginPanel[i]->wmid == wndMsg)
			{
				LS_ServerInfoDlg* childdlg = m_vLoginPanel[i]->m_vinfodlg[j];
				//			if(strcmp(childdlg->m_svrInfo.ipaddr.c_str(),ipaddr) == 0 &&
				//	childdlg->m_svrInfo.port == port) 
				if(childdlg->m_svrInfo.sendserverid == sendserverid )
				{
					childdlg->PostMessageA(WM_LSLOGINSERVER,type,blockstate);

				}
			}


		}
	}
}


void CLSMonitorDlg::OnLoginserverLoginserverinFill()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	ioHashString	szip;
	int port = 0;
	GetSelectedLoginInfo( szip,port);

	for(int i=0; i<(int)m_vloginserverinfo.size();++i)
	{
		if(strcmp(m_vloginserverinfo[i].szName,LVSVER) == 0 &&
			strcmp(m_vloginserverinfo[i].szIP,szip.c_str()) == 0 &&
			m_vloginserverinfo[i].iPort == port )
		{

			SP2Packet pk(EPROTOCOL::LSPTK_CONTROL_SERVER);
			int type = EMCONTROLTYPE::LS_FILLINFODRAW;
			pk << type;
			g_TCPSocketTable.Send(WM_TCPLOGIN1+i,pk);
		}
	}
	 
}

LONG CLSMonitorDlg::OnDisConnected( WPARAM wParam, LPARAM lParam )
{
	return 0;
}

void CLSMonitorDlg::OnConnect( int wmid )
{
	for(int i=0; i<m_vloginserverinfo.size();++i)
	{
		if(m_vloginserverinfo[i].nwmid == wmid)
		{
			m_vloginserverinfo[i].nstate = 1;
		}
	}

}

void CLSMonitorDlg::OnNMDBClickServerInfo( NMHDR *pNHDR, LRESULT *pResult )
{
	int a = 0;
		int iZoneArray = m_TabCtrlZone.GetCurSel();
	if( m_vZoneInfo[ iZoneArray ].gradeType != DT_DEVELOPER )
		return;

 
	// 선택한 컬럼의 index
	m_selectIndex = (int)m_ListCtrlServerInfo.GetFirstSelectedItemPosition();

	CString strName = m_ListCtrlServerInfo.GetItemText( m_selectIndex - 1, 0 );
	if( strName == "Main Server" || strName == "로컬존" )
	{
	
		OnMainServerInfoDrawReq();
	}
	else if( strName =="Login Server")
	{
		OnLoginserverLoginserverinFill();
		

	}
	else
	{
		 OnGameserverInfodraw();
	}

	// 띄우기
}
