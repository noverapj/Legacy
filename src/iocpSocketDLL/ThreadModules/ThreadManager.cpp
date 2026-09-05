#include "../iocpSocketDLL.h"
#include "ThreadManager.h"
#include <process.h>


ThreadManager *ThreadManager::sg_Instance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ThreadManager::ThreadManager()
{
	m_vHandle_list.reserve(100);
}

ThreadManager::~ThreadManager()
{
	if(!m_vHandle_list.empty())
	{
		while(m_vHandle_list.size() > 0)
			m_vHandle_list.erase(m_vHandle_list.begin());
	}
	m_vHandle_list.clear();
}

ThreadManager *ThreadManager::GetInstance()
{
	if(sg_Instance == NULL)
		sg_Instance = new ThreadManager;
	return sg_Instance;
}

void ThreadManager::ReleaseInstance()
{
	if(sg_Instance)
		delete sg_Instance;
	sg_Instance = NULL;
}

void ThreadManager::Clear()      //생성 되어 있는 모든 쓰레드를 종료 시킨다.
{
	vHandle_iter iter = m_vHandle_list.begin();
	vHandle_iter iter_Prev;
	while(iter != m_vHandle_list.end())
	{
		iter_Prev = iter;
		::WaitForSingleObject(*iter_Prev,1);
		m_vHandle_list.erase(iter_Prev);
	}	
}

HANDLE ThreadManager::Spawn(unsigned (WINAPI *startAddress)(void *), LPVOID parameter,UINT &threadID,int threadCount)
{
	HANDLE threadHandle = 0;

	UINT threadId = 0;
	//threadHandle = CreateThread(0,0,startAddress,parameter,0,&threadID);
	for(int i=0; i<threadCount; i++)
	{
		threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(	
			NULL, 
			0, 
			startAddress, 
			parameter, 
			0, 
			&threadId ));
		if(!threadHandle)
		{
			return 0;
		}

		threadID = threadId;
		m_vHandle_list.push_back(threadHandle);
	}
	return threadHandle;
}


