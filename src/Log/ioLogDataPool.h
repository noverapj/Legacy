#pragma once

#include "../include/MemPooler.h"
#include "../include/cSingleton.h"

class CLogData;

class ioLogDataPool
{
public:
	ioLogDataPool(void);
	~ioLogDataPool(void);

private:
	void Init();
	void Destroy();

private:
	void IncreaseDropCount();

private:
	int32 m_maxUsingCount;
	int32 m_dropCount;

	MemPooler<CLogData> m_memPooler;

public:
	CLogData* Pop();
	void Push( CLogData* sendBuffer );

	const int32 GetDropCount()		{ return m_dropCount; }
	const int GetRemainCount()		{ return m_memPooler.GetCount(); }
	const int GetToalCount()		{ return m_memPooler.GetTotalCount(); }
	const int GetMaxUsingCount()	{ return m_maxUsingCount; }
};

#define g_logDataPool cSingleton<ioLogDataPool>::GetInstance()


