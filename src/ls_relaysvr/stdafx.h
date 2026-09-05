// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//
#pragma once
#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <mmstream.h>
#include <comdef.h>
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

 

#include "Service/Service.h"
#include "Manager.h"
#include "../include/Log.h"
#include "Version.h"
/************************************************************************/
/* Boost                                                                     */
/************************************************************************/
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/timer.hpp>
#include <boost/chrono.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <boost/thread/once.hpp>
#include "BaseClass/locking_queue.h"
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
/************************************************************************/
/* UserLib                                                                     */
/************************************************************************/
#include "../iocpSocketDLL/iocpSocketDLL.h"
#include "../FrameTimerDLL/FrameTimerDLL.h"
#include "Minidump/File.h"
#include "Minidump/MiniDump.h"

/************************************************************************/
/* network                                                                     */
/************************************************************************/
#include "Network/SP2Packet.h"
#include "Network/IOCPHandler.h"
#include "Network/ioServerSecurity.h"
#include "Network/RelayServerUDPModule.h"
#include "Network/RelayServerUDPNode.h"
/************************************************************************/
/* BaseClass                                                                     */
/************************************************************************/
#include "BaseClass/Singleton.h"
#include "BaseClass/ioConfiguration.h"
#include "BaseClass/ioRelayServerState.h"
#include "BaseClass/ioTestClass.h"
#include "BaseClass/Operation.h"
#include "BaseClass/Handler.h"
#include "BaseClass/OpMemPool.h"
#include "BaseClass/LSTimer.h"
#include "BaseClass/LSTimerMgr.h"
#include "BaseClass/ioPacketChecker.h"
#include "BaseClass/ioProcessChecker.h"
#include "BaseClass/HackCheck.h"
/************************************************************************/
/* Scheduler                                                                     */
/************************************************************************/
#include "Scheduler/Scheduler.h"
#include "Scheduler/SchedulerNode.h"
/************************************************************************/
/* LS_Acceptor                                                                     */
/************************************************************************/
#include "LSAcceptor/AcceptorMgr.h"
#include "LSAcceptor/ioRelayServerBind.h"
#include "LSAcceptor/MonitorNode.h"
#include "LSAcceptor/MonitorsManager.h"
/************************************************************************/
/* Logic                                                                     */
/************************************************************************/
#include "Logic/LSLogic.h"
#include "Logic/LSLogicHandler.h"
#include "Logic/LSLogicOperations.h"
#include "Logic/LSPacketQueue.h"
 
/************************************************************************/
/* LSGameServerConnector                                                                     */
/************************************************************************/
#include "LSGameServerConnector/GameServerNode.h"
#include "LSGameServerConnector/ServerConnectorMgr.h"
#include "LSGameServerConnector/ConnectAssist.h"
/************************************************************************/
/* usercontanier                                                                     */
/************************************************************************/
//#include "../SharedContainer/ioVector.h"
/************************************************************************/
/* Define                                                                     */
/************************************************************************/
#include "UserDefine.h"
#include "UserDefineSingleton.h"
#include "UserDefineType.h"
#include "../ioINILoader/ioINILoader.h"
 
class CountTime
{
public:
	~CountTime()
	{
		End();
	}
	void Start()
	{
		tStart = boost::chrono::system_clock::now();
	}
	unsigned int End()
	{
		 
		tEnd = boost::chrono::system_clock::now();
		boost::chrono::milliseconds mill = boost::chrono::duration_cast<boost::chrono::milliseconds>(tEnd - tStart);
		return mill.count();
	}
protected:
	boost::chrono::system_clock::time_point tStart;
	boost::chrono::system_clock::time_point tEnd;

};

extern CLog LOG;
extern CLog ReportLOG;
extern CLog HackLOG;

extern void Trace( const char *format, ... );
extern void Debug( const char *format, ... );
extern void Information( const char *format, ... );