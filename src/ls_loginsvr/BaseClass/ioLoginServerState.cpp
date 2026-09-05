#include "StdAfx.h"
#include "ioLoginServerState.h"


ioLoginServerState::ioLoginServerState(void)
{
	InitData();

}

ioLoginServerState::~ioLoginServerState(void)
{
}


void ioLoginServerState::InitData()
{
	m_acceptCount = 0;
	m_closeCount = 0;
	m_serverCount =0;
	m_recvCount = 0;
	m_serverCount = 0;
	m_now = 0;
	m_onClientCount = 0;
	m_acceptCount = 0;
	m_avgClientCount = 0;
	m_currentAcceptCount = 0;
	m_dllAcceptCount = 0;
	m_dllAcceptTime = 0;
	m_testCount = 0;
	m_timestate = false;

	m_el = std::chrono::steady_clock::now();
	m_startEl = std::chrono::steady_clock::now();
}

void ioLoginServerState::PrintTime()
{
	m_cpuTime.GetUsage(&m_sys,NULL);
	float ftime = std::chrono::duration<float>(
		std::chrono::steady_clock::now() - m_el
	).count();

	ReportLOG.PrintTimeAndLog(0,"TestCount : %0.3f(%d)[%0.3f]",ftime,m_sys,(float)100000/ftime);
	InterlockedExchange(&m_testCount,0);
	m_el = std::chrono::steady_clock::now();
}

void ioLoginServerState::PrintLowTime()
{
	float elapsed = std::chrono::duration<float>(
		std::chrono::steady_clock::now() - m_startEl
	).count();
	ReportLOG.PrintTimeAndLog(0, "Lowmemory time : %0.3f", elapsed);
}
