#include "StdAfx.h"
#include "ioRelayServerState.h"


ioRelayServerState::ioRelayServerState(void)
{
	m_accetcount = 0;
	m_closecount = 0;
	m_servercount =0;
	m_recvcount = 0;
	m_servercount = 0;
	m_now = 0;
	m_onclientcount = 0;
	m_accetcount = 0;
	m_avgclientcount = 0;
	m_acceptnow = 0;
	m_testCount = 0;
	m_timestate = false;
	m_userCount = 0;
	m_RoomCount = 0;
	m_64DropCount= 0;
	m_256DropCount = 0;
	m_1024DropCount = 0;
	m_64UsingCount = 0;
	m_256UsingCount = 0;
	m_1024UsingCount = 0;
	m_sendCount = 0;
}

ioRelayServerState::~ioRelayServerState(void)
{
}

void ioRelayServerState::PrintTime()
{
	m_cpuTime.GetUsage(&m_sys,NULL);
	float ftime = m_elaplsedTime.elapsed();
	ReportLOG.PrintTimeAndLog(0,"TestCount : %0.3f(%d)[%0.3f]",ftime,m_sys,(float)100000/ftime);
	InterlockedExchange(&m_testCount,0);
	m_elaplsedTime.restart();
}

void ioRelayServerState::PrintLowTime()
{
	ReportLOG.PrintTimeAndLog(0,"Lowmemory time : %0.3f",m_startTime.elapsed());
}

long ioRelayServerState::GetUserCount() const
{
	return S_RelayServerUDPNode::instance()->GetUserCount();
}

long ioRelayServerState::RoomCount() const
{
	return g_ServerConnectMgr()->GetRoomCount();
}

void ioRelayServerState::IncrementTestCount()
{
	if(m_testCount == 0)
	{
		if(m_timestate == false)

			m_startTime.restart();
		InterlockedIncrement(&m_testCount);
		m_elaplsedTime.restart();
		m_timestate = true;
		return;
	}
	InterlockedIncrement(&m_testCount);
}



 