#include "StdAfx.h"
#include "Manager.h"
#include "UserDefineSingleton.h"

extern void ShutDown();

bool tokenize(const std::string str, const std::string delimiters, std::vector<std::string>& tokens)
{
	tokens.clear();

	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	std::string token;
	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		token = str.substr(lastPos, pos - lastPos);
		tokens.push_back(token);

		lastPos = str.find_first_not_of(delimiters, pos);

		pos = str.find_first_of(delimiters, lastPos);
	}
	return (tokens.size() > 0) ? true : false;
}

BOOL ConsoleHandler(DWORD fdwCtrlType) 
{ 
	switch (fdwCtrlType) 
	{ 
		// Handle the CTRL+C signal. 
	case CTRL_C_EVENT: 
	case CTRL_CLOSE_EVENT: // CTRL+CLOSE: confirm! that the user wants to exit. 
	case CTRL_BREAK_EVENT: 
	case CTRL_LOGOFF_EVENT: 
	case CTRL_SHUTDOWN_EVENT: 
	default: 
		ShutDown();
	 
// 			LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
// 			LOG.CloseLog();
// 			ReportLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
// 			ReportLOG.CloseLog();
// 			HackLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
// 			HackLOG.CloseLog();
			Sleep(300);
	 
		return FALSE;
	} 
	return TRUE;
}	

Manager::Manager(void)
{
}

Manager::~Manager(void)
{
}

bool Manager::Init()
{



#if(1) //kyg 이거 꼭 살릴것
    //S_Test::instance()->Run();
#endif	
	timeBeginPeriod(1);
	
	if(!g_Config()->Init())
		return false;
	SetTcpLog();
	g_Queue()->InitPacketQueue();

	g_Iocp()->Init(g_Config()->GetWorkerCount());

	g_ServerConnectMgr()->Init();
	g_ServerConnectMgr()->InitMemoryPool();
 
	g_MonitorMgr()->InitMemoryPool();
	g_TimerMgr()->SetQueue(g_Queue());
	g_TimerMgr()->SetPool(g_OPMemPool());
	g_Logic()->Init();
	g_State()->SetOPoolCount(g_OPMemPool()->GetSize());
	g_RelayServerInfo()->Init();
	
 
	if(!g_RelayServerInfo()->Start(g_Config()->GetSIpAddr().c_str(),g_Config()->GetPort()))
	{
		LOG.PrintTimeAndLog(0,"RelayServer Acceptor Bind Error\n");
		return false;
	}

	g_UDPModule(); g_UDPNode();
	g_UDPNode()->InitMemory(3000, 16384 * 2, 6000, 6000, 6000, 1000, 10);
	std::vector<int>& m_ports = g_Config()->GetUdpPorts();
	
	std::string tmp ="0.0.0.0";
	int workercount = m_ports.size() + 8;
	if( !g_UDPModule()->SetUDPModule(m_ports,
		tmp,
		10399,
		g_UDPNode(),
		workercount)) 
		return FALSE;
	
	g_ServerConnectMgr()->CreateConnectoClients(g_Config()->ServerAddr());
	
	return true;
}

bool Manager::Run(TCHAR* scriptName)
{
	g_Config()->SetINI(scriptName);
	try
	{
		if(SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
			return false;
		if(!Init())
		{
			LOG.PrintTimeAndLog(0,"초기화 실패로 종료 로그를 확인하세요\n");
			return false;
		}
	}
	catch(_com_error & e)
	{
		LOG.PrintTimeAndLog(0,"Manager Failed");
		std::cout<<e.ErrorMessage();
	}
	return true;
}

void Manager::SetTcpLog()
{
	ioINILoader kLoader( "../global_define.ini" );
	kLoader.SetTitle("Log_Server");

	std::vector<std::pair<std::string,int>> ipAddrs;

	for(int i=1; i<= 100000; ++i)
	{
		std::vector<std::string> tokens;
		char ipAddr[100];
		char keyTemp[30];
		sprintf_s(keyTemp,"%d",i);
		kLoader.LoadString(keyTemp,"",ipAddr,sizeof(ipAddr));
		if(strcmp(ipAddr,"") == 0)
			break;
		tokenize(ipAddr,":",tokens);
		if(tokens.size() != 2) return;
		std::pair<std::string,int> prData;
		prData.first = tokens[0].c_str();
		prData.second = atoi(tokens[1].c_str());
		ipAddrs.push_back(prData);
	}
#if 0
	if(!ipAddrs.empty())
	{
		std::pair<std::string,int> prLogger = ipAddrs[0];
		std::string ipAddr = prLogger.first;
		int loggerPort = prLogger.second;
		LOG.SetTcpMode("RelayServer",g_Config()->GetPort(),loggerPort,(char*)ipAddr.c_str()); //자신만의 유니크한 포트를 부여함 (그래야 로그 선별이 가능)
		LOG.SetCategory("Log"); //카테코리셋팅은 최상에 해줘야함 
		HackLOG.SetCategory("HackLog");
		ReportLOG.SetCategory("ReportLog");
	}
#endif
}
