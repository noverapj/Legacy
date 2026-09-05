

#include "stdafx.h"
#include "ioThreadTaskManager.h"

ioThreadTaskManager* ioThreadTaskManager::m_pManager = NULL;

DWORD WINAPI _ThreadTaskManagerProc( LPVOID lpParameter )
{
    return ( (ioThreadTaskManager*)lpParameter )->ThreadTaskProc();
}

ioThreadTaskManager& ioThreadTaskManager::GetInstance()
{
	if( m_pManager )
		return *m_pManager;

	m_pManager = new ioThreadTaskManager;
	return *m_pManager;
}

void ioThreadTaskManager::ReleaseInstance()
{
	SAFEDELETE( m_pManager );
}

ioThreadTaskManager::ioThreadTaskManager()
{
	m_hQueueSemaphore = INVALID_HANDLE_VALUE;
	m_hTaskThread = INVALID_HANDLE_VALUE;

	m_pWorkTask = NULL;

	m_bTaskThreadDone = false;
	m_bDone = false;
}

ioThreadTaskManager::~ioThreadTaskManager()
{
	if( !m_bDone )
	{
		DestroyThreadObjects();
	}
}

void ioThreadTaskManager::Initialize()
{
	m_hQueueSemaphore = CreateSemaphore( NULL, 0, LONG_MAX, NULL );

	InitializeCriticalSection( &m_TaskSection );

	m_hTaskThread = CreateThread( NULL, 0, _ThreadTaskManagerProc, (LPVOID)this, CREATE_SUSPENDED, NULL );
	ResumeThread( m_hTaskThread );
}

void ioThreadTaskManager::DestroyThreadObjects()
{
	m_bDone = true;

	int iTryCount = 0;
	while( !m_bTaskThreadDone && iTryCount < 10 )
	{
		ReleaseSemaphore( m_hQueueSemaphore, 1, NULL );
		iTryCount++;

		::SleepEx( 500, FALSE );
	}

	if( iTryCount < 10 )
	{
		CloseHandle( m_hTaskThread );
		LOG.PrintTimeAndLog( 0, "Close TaskThread" );
	}
	else
	{
		TerminateThread( m_hTaskThread, 0 );
		LOG.PrintTimeAndLog( 0, "Terminate TaskThread" );
	}

	CloseHandle( m_hQueueSemaphore );
	DeleteCriticalSection( &m_TaskSection );

	// 만악 스레드가 터미네이트로 종료가 되었다면 현재 태스크가 작업중이었을 가능성이 있다.
	// 현재 작업중 태스크는 양쪽큐 어디에도 없으므로 여기서 삭제
	SAFEDELETE(m_pWorkTask);

	ThreadTaskQueue::iterator iter;
	for( iter = m_TaskQueue.begin() ; iter!=m_TaskQueue.end() ; ++iter )
	{
		delete *iter;
	}
	m_TaskQueue.clear();

	for( iter=m_DoneTaskQueue.begin() ; iter!=m_DoneTaskQueue.end() ; ++iter )
	{
		delete *iter;
	}
	m_DoneTaskQueue.clear();
}

struct TaskPrioritySort
{
	bool operator()( const IThreadTask *pLhs, const IThreadTask *pRhs ) const
	{
		if( pLhs->GetPriority() > pRhs->GetPriority() )
			return true;

		return false;
	}
};

void ioThreadTaskManager::AddTask( IThreadTask *pTask )
{
	if( !pTask )	return;

	EnterCriticalSection( &m_TaskSection );
	m_TaskQueue.push_back( pTask );
	std::stable_sort( m_TaskQueue.begin(), m_TaskQueue.end(), TaskPrioritySort() );
	LeaveCriticalSection( &m_TaskSection );

	ReleaseSemaphore( m_hQueueSemaphore, 1, NULL );
}

void ioThreadTaskManager::NotifyDoneTasks()
{
	if( m_DoneTaskQueue.empty() )
		return;

	ThreadTaskQueue vTempQueue;

	EnterCriticalSection( &m_TaskSection );
	vTempQueue = m_DoneTaskQueue;
	m_DoneTaskQueue.clear();
	LeaveCriticalSection( &m_TaskSection );

	ThreadTaskQueue::iterator iter = vTempQueue.begin();
	for( ; iter!=vTempQueue.end() ; ++iter )
	{
		IThreadTask *pTask = *iter;

		pTask->Done();
		delete pTask;
	}
}

int ioThreadTaskManager::ThreadTaskProc()
{
	m_bTaskThreadDone = false;

	while( !m_bDone )
	{
        WaitForSingleObject( m_hQueueSemaphore, INFINITE );

		if( m_bDone )
			break;

		m_pWorkTask = NULL;

		EnterCriticalSection( &m_TaskSection );
		if( !m_TaskQueue.empty() )
		{
			m_pWorkTask = m_TaskQueue.front();
			m_TaskQueue.pop_front();
		}
		LeaveCriticalSection( &m_TaskSection );

		if( m_pWorkTask )
		{
			m_pWorkTask->Process();

			EnterCriticalSection( &m_TaskSection );
			m_DoneTaskQueue.push_back( m_pWorkTask );
			m_pWorkTask = NULL;
			LeaveCriticalSection( &m_TaskSection );
		}
	}

	m_bTaskThreadDone = true;
	return 0;
}
