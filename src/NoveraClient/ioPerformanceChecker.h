#pragma once

class ioPerformanceChecker
{
protected:
	DWORD m_dwStartTime;

	DWORD m_dwPreCheckTime;
	int   m_iLoopCount;

	DWORD m_dwWaitTime;
	bool  m_bCheckNow;

	struct PointValue
	{
		DWORD dwSum;
		DWORD dwEven;

		PointValue()
		{
			dwSum = dwEven = 0;
		}
	};

	int   m_iEvenCount;
	DWORD m_dwEvenErrorSendTime;

	typedef std::map< int, PointValue > CheckPointMap;
	CheckPointMap m_CheckPointMap;

	bool m_bLogging;
	bool m_bSendLog;

public:
	void Initialize( bool bLogging, bool bSendLog );
	void FrameStart();
	void CheckPoint( int iPoint );
	void Analysis();

protected:
	void SendLog( int iPoint, DWORD dwEvenTime, DWORD dwCheckTime );

public:
	static ioPerformanceChecker& GetInstance();
	static void ReleaseInstance();

public:
	ioPerformanceChecker();
	~ioPerformanceChecker();
};

#define g_PerfChecker ioPerformanceChecker::GetInstance()

