// Thread.h: interface for the Thread class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "SuperParent.h"

class IOCP_SOCKET_API Thread : public SuperParent 
{
private:
	static unsigned WINAPI HandleRunner(LPVOID parameter);
	UINT m_threadID;

protected:
	HANDLE m_hThread;
	
public:
	virtual void Run() = 0;            //상속 받은 클래스가 구현한다.

public:
	DWORD GetThread(){ return m_threadID; }
	BOOL  Begin(int iThreadCount = 1);

public:
	Thread();
	virtual ~Thread();
};
