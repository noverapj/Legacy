
#include "stdafx.h"
#include "LogData.h"
#include "ioLogDataPool.h"

ioLogDataPool::ioLogDataPool(void)
{
	Init();
}

ioLogDataPool::~ioLogDataPool(void)
{
	Destroy();
}

void ioLogDataPool::Init()
{
	m_dropCount = 0;
	m_maxUsingCount = 0;

	m_memPooler.CreatePool( 100, 10000, TRUE );
}

void ioLogDataPool::Destroy()
{
	m_memPooler.DestroyPool();
}

CLogData* ioLogDataPool::Pop()
{
	CLogData* logData = m_memPooler.Pop();
	if( logData != NULL )
	{
		logData->Init();

		int usingCount = GetToalCount() - GetRemainCount();
		if( m_maxUsingCount < usingCount )
			m_maxUsingCount = usingCount;
	}
	else
	{
		IncreaseDropCount();
	}
	return logData;
}

void ioLogDataPool::Push( CLogData* buffer )
{
	m_memPooler.Push( buffer );
}

void ioLogDataPool::IncreaseDropCount()
{
	++m_dropCount;
}
