#pragma once

#include <pdh.h>
#include <pdhmsg.h>
#include <vector>
#include "ioPDHCounter.h"

// pdh.lib 링크가 필요함
class ioPDH
{
public:
	ioPDH(void);
	~ioPDH(void);

	void Init();
	void Destroy();

public:
	int GetError()		{ return m_error; }

protected:
	BOOL Startup();
	void Cleanup();

	BOOL AddCounter(const TCHAR *counterName);
	BOOL CollectQuery();

	BOOL UpdateValue(PDHCOUNTERSTRUCT* pdhCounter);
	BOOL UpdateRawValue(PDHCOUNTERSTRUCT* pdhCounter);
	BOOL GetStatistics(long *valueMin, long *valueMax, long *valueMean, int index);
	LONGLONG GetValue(int index);
	PDHCOUNTERSTRUCT* GetCounter(int index);

	void SetError(int error)	{ m_error = error; }

	int GetCounters()	{ return m_counters.size(); }

protected:
	typedef std::vector<PDHCOUNTERSTRUCT*> PDHCOUNTERS;

	int m_index;
	int m_error;
	HQUERY m_query;
	PDHCOUNTERS m_counters;
};

