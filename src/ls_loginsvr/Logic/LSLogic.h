#pragma once


#include <string>

class LSLogic : public CProcessor
{
public:
	LSLogic(void);
	virtual ~LSLogic(void);

public://function
	void InitData();
	void Init();
	void Destory();

public:
	void InitScheduler();
	void StartShcedule();

public:
	void SetQueue(LSPacketQueue* pQueue) { m_queue = pQueue; }
	virtual void Process(uint32& idleTime);	
	SchedulerNode* PScheduler() const { return m_scheduler; }
	void PScheduler(SchedulerNode* val) { m_scheduler = val; }

public:
	void SetNewdata(DWORD dwProcessTime = 0);
	void PrintTimeAndLog(int debuglv, LPSTR fmt );
	void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt );

public:
	void ShutDown(BOOL state);

private:
	LogicThread *m_logic;
	LSPacketQueue *m_queue;
	SchedulerNode* m_scheduler;
	DWORD m_logCheckTime;
};

