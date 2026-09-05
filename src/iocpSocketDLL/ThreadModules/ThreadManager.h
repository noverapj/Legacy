// ThreadManger.h: interface for the ThreadManger class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

typedef std::vector<HANDLE> vHandle;
typedef vHandle::iterator vHandle_iter;

class IOCP_SOCKET_API ThreadManager  
{
	static ThreadManager *sg_Instance;
	vHandle m_vHandle_list;
	
	private: 	/* Singleton Class */
	ThreadManager();
	virtual ~ThreadManager();

	public:
	static ThreadManager *GetInstance();
	static void ReleaseInstance();

	public:
	void Clear();
	HANDLE Spawn(unsigned (WINAPI *startAddress)(void *), LPVOID parameter,UINT &threadID,int threadCount);
	
	public:
	int GetHandleCount(){ return m_vHandle_list.size(); }
};
