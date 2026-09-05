// ThreadManger.h: interface for the ThreadManger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADMANAGER_H__EBD06465_DD5C_4F78_B68C_73444A87BFDF__INCLUDED_)
#define AFX_THREADMANAGER_H__EBD06465_DD5C_4F78_B68C_73444A87BFDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
using namespace std;

typedef vector<HANDLE> vHandle;
typedef vHandle::iterator vHandle_iter;

class ThreadManager  
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
	HANDLE Spawn(LPTHREAD_START_ROUTINE startAddress,LPVOID parameter,DWORD *threadID);
	
	public:
	int GetHandleCount(){ return m_vHandle_list.size(); }
};

#endif // !defined(AFX_THREADMANAGER_H__EBD06465_DD5C_4F78_B68C_73444A87BFDF__INCLUDED_)
