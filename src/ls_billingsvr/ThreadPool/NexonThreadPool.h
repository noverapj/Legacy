#pragma once

#include "ioThreadPool.h"

class NexonThreadPool : public SuperParent
{
private:
	static NexonThreadPool *sg_Instance;

public:
	static NexonThreadPool &GetInstance();
	static void ReleaseInstance();

protected:
	typedef std::vector< ioBillThread * > ioThreadVec;
	ioThreadVec m_vThread;

	ThreadPoolType m_eThreadPoolType;

protected:
	void Clear();
	bool SetThreadData( ioData &rData );

public:
	virtual void Initialize();

public:
	void SetData( const ioData &rData );

public:
	int GetActiveThreadCount();
	int GetThreadCount() { return m_vThread.size(); }

	int  GetNodeSize();
	int  RemainderNode();

	int  GetLoginNodeSize();
	int  RemainderLoginNode();

	void SetThreadPoolType(ThreadPoolType eThreadPoolType) { m_eThreadPoolType = eThreadPoolType; }

private:
	NexonThreadPool();
	virtual ~NexonThreadPool();
};
#define g_NexonThreadPool NexonThreadPool::GetInstance()