// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DBAgentServer.h"
#include "DBAgentServerDoc.h"
#include "DBAgentServerView.h"
#include "MainFrm.h"
#include "DlgDBLogin.h"
#include "MainProcess.h"
#include "ThreadPool/ioThreadPool.h"
#include "Network/DBServer.h"
#include "Network/BroadCastUDP.h"
#include "Network/iocpHandler.h"
#include "Network/ioPacketQueue.h"
#include "NodeInfo/UserNodeManager.h"
#include "Database/cQueryManager.h"
#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs )
{
	static bool bHappenCrash = false;	
	ReportLOG.SaveLog();
	LOG.SaveLog();

	if( bHappenCrash )
		return EXCEPTION_EXECUTE_HANDLER;

	char szTemp[2048]="";
	LOG.PrintTimeAndLog( 0, "---- Crash Help Data ----" );
	LOG.PrintTimeAndLog( 0, "%s", GetFaultReason(pExPtrs) );
	LOG.PrintTimeAndLog( 0, "%s", GetRegisterString(pExPtrs) );

	const char * szBuff = GetFirstStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE,pExPtrs  );
	do
	{
		LOG.PrintTimeAndLog(0,"%s" , szBuff );	
		szBuff = GetNextStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE , pExPtrs );
	}
	while ( NULL != szBuff );

	LOG.PrintTimeAndLog(0, "---- Crash Help End ----");

	bHappenCrash = true;

	return EXCEPTION_EXECUTE_HANDLER;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_PROGRAM_EXIT, OnProgramExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	strcpy_s(m_dbip,"");
	m_pDBServer		= NULL;
	m_logicThread	= NULL;
	m_bOnProgramExit = false;
}

CMainFrame::~CMainFrame()
{
}

unsigned WINAPI Runner(LPVOID parameter)
{
	for(int i = 0 ; i < 5 ; i++)
	{
	}

	return 0;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HANDLE threadHandle = 0;
	UINT threadId;
	
	//threadHandle = CreateThread(0,0,startAddress,parameter,0,&threadID);
	threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(	
		NULL, 
		0, 
		Runner, 
		0, 
		0, 
		&threadId ));
	if(!threadHandle)
	{
		AfxMessageBox( "ThreadManager Spawn FAILED:%lu " );
	}

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	timeBeginPeriod(1);
	//---------------------------------
	//        ini 정보
	//---------------------------------
	char szCurrentPath[MAX_PATH] = "";
			
	GetCurrentDirectory(MAX_PATH, szCurrentPath);
	strcat_s(szCurrentPath, "\\DBAgentServerInfo.ini");

	int agentserver_port = GetPrivateProfileInt("DBAgent Server Infomation", "DBAgentServerPort", 10000,szCurrentPath);	
	
	GetPrivateProfileString("DBAgent Server Infomation", "DatabaseIP", "000.000.000.000", m_dbip,MAX_PATH, szCurrentPath);	

	int iDatabasePort = GetPrivateProfileInt("DBAgent Server Infomation", "DatabasePort", 1433, szCurrentPath);	

	if(strcmp(m_dbip,"000.000.000.000") == 0)
	{
		AfxMessageBox(_T("IP 주소가 없습니다."));
		PostQuitMessage(0);
		return 0;
	}
	//--------------------------------
	//     로그 기록
	//--------------------------------
	g_MainProc.LogCurrentStates( 0 );
	//--------------------------------
	//     DB 접속
	//--------------------------------
	char szName[MAX_PATH] = "";
	char szID[MAX_PATH] = "";
	char szPW[MAX_PATH] = "";
	CDlgDBLogin dlg;
	dlg.DoModal(szName,szID,szPW);   //

	if(!g_queryManager.AddDatabase(1, m_dbip, iDatabasePort, szName, szID, szPW))
	{
		AfxMessageBox(_T("DB 세션을 열지 못했습니다."));
		WritePrivateProfileString("Login Information", "PW", "", szCurrentPath);
		PostQuitMessage(0);
		return 0;
	}

	g_UserNodeManager.InitMemoryPool();

	//--------------------------------
	//     SOCKET Init
	//--------------------------------
	if( !BeginSocket() )
	{
		LOG.PrintTimeAndLog( 0, "BeginSocket Failed!");
		return 0;
	}

	// SET IP
	int iPrivateIPFirstByte = GetPrivateProfileInt( "DBAgent Server Infomation", "PrivateIPFirstByte", 0, szCurrentPath );
	if( !SetLocalIP(iPrivateIPFirstByte) )
	{
		AfxMessageBox(_T("Local IP FAILED!"));
		PostQuitMessage(0);
		return 0;
	}
	
	int  port    = 0;
	port = GetPrivateProfileInt( "DBAgent Server Infomation", "DBAgentServerPort",0,szCurrentPath);
	m_pDBServer = new ioDBServer; 
	if( !m_pDBServer->Start( (char*)m_szPrivateIP.c_str(), port ) )
	{
		AfxMessageBox(_T("ACCEPTOR INIT FAILED!"));
		PostQuitMessage(0);
		return 0;		
	}

	// 메인프로세스와 로직스레드 구동
	if( !g_iocp.Initialize() )
	{
		LOG.PrintTimeAndLog( 0, "IOCP Initialize Failed!");
		return 0;
	}

	if( !g_BroadCastUDP.Initialize( port ) ) 
	{
		LOG.PrintTimeAndLog( 0, "BroadCastUDP Initialize Failed!");
		return 0;
	}
	
	g_RecvQueue.Initialize();

	char window_name[MAX_PATH] ="";
	wsprintf( window_name, "%s(%s) %s:%d", szName, m_dbip, m_szPrivateIP.c_str(), agentserver_port );
	if(!g_MainProc.Initialize( m_hWnd, window_name ))
	{
		LOG.PrintTimeAndLog( 0, "MainProc Initialize Failed!");
		return 0;
	}

	m_logicThread = new LogicThread;
	m_logicThread->SetProcessor(&(g_MainProc));
	m_logicThread->Begin();

	if(true != g_ThreadPool.Initialize())
	{

	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.x = 0;
	cs.y = 0;
	cs.cx = 535;
	cs.cy = 0;
	cs.style ^= FWS_ADDTOTITLE;

	WNDCLASS wc;
	::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wc );
	wc.lpszClassName = _T(APPNAME);
	cs.lpszClass     = _T(APPNAME);
	if( AfxGetApp() )
		wc.hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	AfxRegisterClass( &wc );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if( !m_bOnProgramExit && AfxMessageBox("DB Agent Server Exit?",MB_YESNO) == IDNO )
		return;
	
	//Socket 종료
	//ioMainProcess::ReleaseInstance();
	//ioPacketQueue::ReleaseInstance();
	//SAFEDELETE( m_pDBServer );
	//iocpHandler::ReleaseInstance();
	//BroadCastUDP::ReleaseInstance();

	//Thread Pool 종료
//	ioThreadPoolMgr::ReleaseInstance();

	//Thread 종료
// 	ThreadManager::GetInstance()->Clear();
// 	ThreadManager::ReleaseInstance();

	//g_UserNodeManager.ReleaseMemoryPool();
	//UserNodeManager::ReleaseInstance();
	//로그 파일 종료
	LOG.CloseAndRelease();
	ReportLOG.CloseAndRelease();

	EndSocket();
	CFrameWnd::OnClose();

	timeEndPeriod(1);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CFrameWnd::OnTimer(nIDEvent);
}

LONG CMainFrame::OnProgramExit( WPARAM wParam, LPARAM lParam )
{
	LOG.PrintTimeAndLog( 0, "Exit from WM" );
	m_bOnProgramExit = true;
	OnClose();
	return 0; 
}

// WSAStartup() 호출 이후에 호출해야함
bool CMainFrame::GetLocalIpAddressList( OUT ioHashStringVec &rvIPList )
{
	char szHostName[MAX_PATH];
	ZeroMemory( szHostName, sizeof( szHostName ) );
	gethostname(szHostName, sizeof(szHostName));

	LPHOSTENT lpstHostent = gethostbyname(szHostName);
	if ( !lpstHostent ) 
	{
		LOG.PrintTimeAndLog(0,"%s lpstHostend == NULL.", __FUNCTION__ );
		return false;
	}

	enum { MAX_LOOP = 100, };
	LPIN_ADDR lpstInAddr = NULL;
	if( lpstHostent->h_addrtype == AF_INET )
	{
		for (int i = 0; i < MAX_LOOP ; i++) // 100개까지 NIC 확인
		{
			lpstInAddr = (LPIN_ADDR)* lpstHostent->h_addr_list;

			if( lpstInAddr == NULL )
				break;

			char szTemp[MAX_PATH]="";
			StringCbCopy( szTemp, sizeof( szTemp ), inet_ntoa(*lpstInAddr) );
			ioHashString sTemp = szTemp;
			rvIPList.push_back( sTemp );			

			lpstHostent->h_addr_list++;
		}
	}

	if( rvIPList.empty() )
	{
		LOG.PrintTimeAndLog(0,"%s Local IP empty.", __FUNCTION__ );
		return false;
	}

	return true;
}

bool CMainFrame::SetLocalIP( int iPrivateIPFirstByte )
{
	ioHashStringVec vIPList;
	if( !GetLocalIpAddressList( vIPList ) ) 
		return false;

	int iSize = vIPList.size();

	// 1, 2 아니면 에러
	if( !COMPARE( iSize, 1, 3 ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error %d", __FUNCTION__, iSize );
		return false;
	}

	// 1
	if( iSize == 1 ) 
	{
		m_szPublicIP  = vIPList[0];
		m_szPrivateIP = vIPList[0];

		if( m_szPrivateIP.IsEmpty() || m_szPublicIP.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Local IP Error %s:%s", __FUNCTION__, m_szPrivateIP.c_str(), m_szPublicIP.c_str() );
			return false;
		}

		return true;
	}

	// 2
	for (int i = 0; i < iSize ; i++)
	{
		if( atoi( vIPList[i].c_str() ) != iPrivateIPFirstByte )
		{
			m_szPublicIP = vIPList[i];
		}
		else
		{
			m_szPrivateIP = vIPList[i];
		}
	}

	if( m_szPrivateIP.IsEmpty() || m_szPublicIP.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Local IP Empty %s:%s", __FUNCTION__, m_szPrivateIP.c_str(), m_szPublicIP.c_str() );
		return false;
	}

	return true;
}