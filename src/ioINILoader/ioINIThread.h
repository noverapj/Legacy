#pragma once
#include <Windows.h>
#include <process.h>

class ioINIThread
{
private:
	static unsigned WINAPI HandleRunner(void* parameter);
	unsigned int m_threadID;

protected:
	HANDLE m_hThread;
	
public:
	virtual void Run() = 0;            //상속 받은 클래스가 구현한다.

public:
	DWORD GetThread(){ return m_threadID; }
	BOOL  Begin(int iThreadCount = 1);

public:
	ioINIThread();
	virtual ~ioINIThread();
};

