#pragma once

class ioThread
{
public:
	ioThread(void);
	virtual ~ioThread(void);

private:
	static unsigned WINAPI HandleRunner(LPVOID parameter);
	UINT m_threadID;

protected:
	HANDLE m_hThread;
	BOOL m_threadStart;

public:
	virtual void Run() = 0;            //상속 받은 클래스가 구현한다.

public:
	DWORD GetThread()				{ return m_threadID; }
	BOOL  Begin();

	BOOL IsStart() const			{ return m_threadStart; }
	void SetStart(const BOOL state)	{ m_threadStart = state; }
};

