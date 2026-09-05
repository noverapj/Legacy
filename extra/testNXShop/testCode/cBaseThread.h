#pragma once


//////////////////////////////////////////////////////////////////////
// Thread 기본 클래스, 쓰레드 생성 및 가상함수 호출
//////////////////////////////////////////////////////////////////////

class cBaseThread  
{
public:
	cBaseThread();
	virtual ~cBaseThread();

	void Init();
	void Destroy();

public:
	BOOL IsRunning()		{ return m_thread ? TRUE : FALSE; }
    BOOL IsTerminated()		{ return m_terminated; }
    UINT GetThreadID()		{ return m_threadId;   }
	void Terminate()		{ m_terminated = TRUE; }

	BOOL StartThread();	// Thread 시작

protected:
    virtual void Process() = 0;
	static unsigned __stdcall ThreadProc( void* handle );

protected:
    UINT	m_threadId;
    HANDLE	m_thread;

	BOOL	m_terminated;
};