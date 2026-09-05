#pragma once


#include <boost/thread/mutex.hpp>
#include "CPU.h"

class ioLoginServerState
{
public:
	ioLoginServerState(void);
	virtual ~ioLoginServerState(void);

public:
	void InitData();

public:
	void IncrementAcceptCount()
	{
		m_acceptCount++;
		SetZero(m_acceptCount);
	}

	void IncrementCloseCount()
	{
		m_closeCount++;
		SetZero(m_closeCount);
	}

	void IncrementServer(long ntmp)	{ m_serverCount += ntmp;}
	void DecrementServer(long ntmp)	{m_serverCount -= ntmp; }
	const int AcceptCount() { return m_acceptCount; }
	const int CloseCount() { return m_closeCount; }
	void IncrementSendServerSide() { SetZero(m_sendCount); }
	const long SendCount() { return m_sendCount; }
	void IncrementRecvCount() { SetZero(m_recvCount); }
	const long RecvCount() { return m_recvCount; }
	void IncrementOPPool() 	{ SetZero(m_oppoolCount); }
	void DecrementOPPool() { SetZero(m_oppoolCount); }
	long OPPoolCount() { return m_oppoolCount; }
	void SetOPoolCount(long size) { m_oppoolCount = size; }

	template<typename T>
	void SetZero(T& val)
	{
		if(val > 10000000)
			val = 0;
	}

	long TestCount() const { return m_testCount; }

	void IncrementTestCount() 
	{
		if(m_testCount == 0)
		{
			if(m_timestate == false)

				m_startEl.restart();
			InterlockedIncrement(&m_testCount);
			m_el.restart();
			m_timestate = true;
			return;
		}
		InterlockedIncrement(&m_testCount);
		
	}
	void PrintTime();
	void PrintLowTime();
	int Now() const { return m_now; }
	void Now(int val) { m_now = val; }
	int OnClientCount() const { return m_onClientCount; }
	void OnClientCount(int val) { m_onClientCount = val; }
	int AvgClientCount() const { return m_avgClientCount; }
	void AvgClientCount(int val) { m_avgClientCount = val; }
	long CurrentAcceptCount() const { return m_currentAcceptCount; }
	void CurrentAcceptCount(long val) { m_currentAcceptCount = val; }
	int DllAcceptCount() const { return m_dllAcceptCount; }
	void DllAcceptCount(int val) { m_dllAcceptCount = val; }
	int DllAcceptTime() const { return m_dllAcceptTime; }
	void DllAcceptTime(int val) { m_dllAcceptTime = val; }

protected:
	long m_acceptCount;
	long m_closeCount;
	long m_serverCount;
	long m_sendCount;
	long m_recvCount;
	long m_oppoolCount;
	int m_now;
	int m_onClientCount;
	int m_avgClientCount;
	long m_currentAcceptCount;
	int m_dllAcceptCount;
	int m_dllAcceptTime;
	long m_testCount;
	boost::timer m_el;
	boost::timer m_startEl;
	boost::mutex m_lock;
	int m_sys;
	CPU m_cpuTime;
	bool m_timestate;
};

