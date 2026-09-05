#pragma once


#include <boost/thread/mutex.hpp>
#include "CPU.h"


class ioRelayServerState
{
public:
	ioRelayServerState(void);
	virtual ~ioRelayServerState(void);

public:
	void IncrementAccept()				{ m_accetcount++; SetZero(m_accetcount); }
	void IncrementClose()				{ m_closecount++;  SetZero(m_closecount); }
	void IncrementServer(long ntmp)		{ m_servercount += ntmp; }
	void DecrementServer(long ntmp)		{ m_servercount -= ntmp; }
	void IncrementSendServerSide()		{ _InterlockedIncrement(&m_sendCount); SetZero(m_sendCount); }
	void IncrementRecv()				{ SetZero(m_recvcount); }
	void DecrementOPPool()				{ SetZero(m_oppoolcount); }
	void IncrementOPPool()				{ SetZero(m_oppoolcount); }
	void IncrementRoomCount()			{ m_RoomCount++; }
	void IncrementUserCount()			{ m_userCount++; }
	void DecrementRoomCount()			{ m_RoomCount--; }
	void DecrementUserCount()			{ m_userCount--; }
	void IncrementTestCount();	

public:
	void PrintTime();
	void PrintLowTime();

public: // get/set 
	const int GetAcceptCount()				{ return m_accetcount; }
	const int GetCloseCount()				{ return m_closecount; }
	const long nSend()						{ return m_sendCount; }
	void SetSendCount(long x)				{ InterlockedExchange(&m_sendCount,x); }
	const long GetRecvCount()				{ return m_recvcount; }
	void IncrementTestSendCount()			{ InterlockedIncrement(&m_testSendCount); SetZero(m_testSendCount);}
	long GetOPPoolCount()					{ return m_oppoolcount; }
	void SetOPoolCount(long size)			{ m_oppoolcount = size; }
	long GetTestCount() const				{ return m_testCount; }
	int GetTimeNow() const					{ return m_now; }
	void SetTimeNow(int val)				{ m_now = val; }
	int GetOnClientCount() const			{ return m_onclientcount; }
	void SetOnClientCount(int val)			{ m_onclientcount = val; }
	int GetAvgClientCount() const			{ return m_avgclientcount; }
	void SetAvgClientCount(int val)			{ m_avgclientcount = val; }
	long GetAcceptNowCount() const			{ return m_acceptnow; }
	void SetAcceptNowCount(long val)		{ m_acceptnow = val; }
	long GetUserCount() const;
	void SetUserCount(long val)				{ m_userCount = val; }
	long RoomCount() const;
	void RoomCount(long val)				{ m_RoomCount = val; }
	long Get64DropCount() const				{ return m_64DropCount; }
	void Set64DropCount(long val)			{ m_64DropCount = val; }
	long Get256DropCount() const			{ return m_256DropCount; }
	void Set256DropCount(long val)			{ m_256DropCount = val; }
	long Get1024DropCount() const			{ return m_1024DropCount; }
	void Set1024DropCount(long val)			{ m_1024DropCount = val; }
	long Get64UsingCount() const			{ return m_64UsingCount; }
	void Set64UsingCount(long val)			{ m_64UsingCount = val; }
	long Get256UsingCount() const			{ return m_256UsingCount; }
	void Set256UsingCount(long val)			{ m_256UsingCount = val; }
	long Get1024UsingCount() const			{ return m_1024UsingCount; }
	void Set1024UsingCount(long val)		{ m_1024UsingCount = val; }
	long TestSendCount() const				{ return m_testSendCount; }

protected:
	template<typename T>
	void SetZero(T& val)
	{
		if(val > 10000000)
			val = 0;
	}

protected:
	long m_accetcount;
	long m_closecount;
	long m_servercount;
	long m_sendCount;
	long m_recvcount;
	long m_oppoolcount;
	int m_now;
	int m_onclientcount;
	int m_avgclientcount;
	long m_acceptnow;
	long m_userCount;
	long m_RoomCount; 
	long m_testCount;
	long m_64DropCount;
	long m_256DropCount;
	long m_1024DropCount;
	long m_64UsingCount;
	long m_256UsingCount;
	long m_1024UsingCount;
	long m_testSendCount;

protected:
	boost::timer m_elaplsedTime;
	boost::timer m_startTime;
	boost::mutex m_lock;
	int m_sys;
	CPU m_cpuTime;
	bool m_timestate;
};

