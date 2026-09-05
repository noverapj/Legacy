#pragma once

class LSLogAssist : public Thread
{
public:
	LSLogAssist(void);
	virtual ~LSLogAssist(void);

public:
	bool Init(DWORD dwProcessTime);
	void Run(); 
	void PutMessage(int debugid,LPSTR fmt,...);
	void PutQueue(LOG_* data);
	LOG_* GetQueue();

protected:
	boost::locking_queue<LOG_*> m_queue;
	TCHAR szPrevTime[MAX_PATH];
};

