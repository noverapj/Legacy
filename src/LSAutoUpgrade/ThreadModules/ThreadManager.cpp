// ThreadManger.cpp: implementation of the ThreadManger class.
//
//////////////////////////////////////////////////////////////////////

#include "../stdafx.h"
#include "ThreadManager.h"

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

HANDLE ThreadManager::Spawn(LPTHREAD_START_ROUTINE startAddress,LPVOID parameter,DWORD *threadID)
{
	HANDLE threadHandle = 0;

	threadHandle = CreateThread(0,0,startAddress,parameter,0,threadID);
	if(!threadHandle)
		return 0;

	m_vHandle_list.push_back(threadHandle);
	return threadHandle;
}


