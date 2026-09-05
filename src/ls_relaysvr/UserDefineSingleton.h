#pragma once

typedef Singleton<LSPacketQueue>			S_Queue;
typedef Singleton<IOCPHandler>				S_Iocp;
typedef Singleton<ioConfiguration>			S_Conf;
typedef Singleton<LSLogic>					S_Logic;
typedef Singleton<ioRelayServerBind>		S_RelayServer;
typedef Singleton<MonitorsManager>			S_MonitorsManager;
typedef Singleton<ioRelayServerState>		S_ioRelayServerState;
typedef Singleton<ioTestClass>				S_Test;
typedef Singleton<boost::asio::io_service>	S_IoServices;
typedef Singleton<ServerConnectorMgr>		S_ServerConnectMgr;
typedef Singleton<OpMemPool>				S_OPMemPool;
typedef Singleton<LSTimerMgr>				S_TimerMgr;
typedef Singleton<ioProcessChecker>			S_IOProcess;
typedef Singleton<ioPacketChecker>			S_IOPacket;
typedef Singleton<ConnectAssist>			S_ConnectAssist;
typedef Singleton<RelayServerUDPModule>		S_RelayServerUDPModule;
typedef Singleton<RelayServerUDPNode>		S_RelayServerUDPNode;

#define  g_IoServices()						S_IoServices::instance()
#define  g_Queue()							S_Queue::instance()
#define  g_Iocp()							S_Iocp::instance()
#define  g_Config()							S_Conf::instance()
#define  g_Logic()							S_Logic::instance()
#define  g_RelayServerInfo()				S_RelayServer::instance()
#define  g_MonitorMgr()						S_MonitorsManager::instance()
#define  g_State()							S_ioRelayServerState::instance()
#define  g_ServerConnectMgr()				S_ServerConnectMgr::instance()
#define  g_OPMemPool()						S_OPMemPool::instance()
#define  g_TimerMgr()						S_TimerMgr::instance()
#define  g_ProcessChecker()					S_IOProcess::instance()
#define  g_PacketChecker()					S_IOPacket::instance()
#define  g_ConnectAssist()					S_ConnectAssist::instance()
#define  g_UDPNode()						S_RelayServerUDPNode::instance()
#define  g_UDPModule()						S_RelayServerUDPModule::instance()
 


